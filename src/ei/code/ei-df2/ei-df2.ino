// XIAO nRF52840 — EI CSV export + pH calibration + robust smoothing (TinyUSB)
// Columns per block line: timestamp,tds_v,tempC,ph_v,ph
// Block behavior: print header, stream for 100 ms, print single-space line, repeat.
//
// Your calibration (from helper):
//   Water volts: 2.99307 -> pH 7.20
//   Coke  volts: 3.01459 -> pH 2.50
//   pH = CAL_M * ph_v + CAL_B

#include <Adafruit_TinyUSB.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

// -------- Pins --------
constexpr uint8_t PIN_ONEWIRE = 4;   // DS18B20 data (4.7k pull-up to 3V3)
constexpr uint8_t PIN_TDS     = A0;  // TDS analog input
constexpr uint8_t PIN_PH      = A2;  // pH  analog input

// -------- ADC / scaling --------
constexpr float VREF = 3.3f;   // ADC reference
int adcMax = 1023;

// If your TDS output into the XIAO is already <=3.3 V, set to 1.0f.
constexpr float DIVIDER_SCALE_TDS = 2.0f;
constexpr float DIVIDER_SCALE_PH  = 1.0f;  // pH board @3.3 V → direct to A2

// -------- CSV block configuration --------
constexpr uint32_t BLOCK_MS    = 100; // sampling window per block (ms)
constexpr uint16_t BLOCK_COUNT = 100; // number of blocks before stopping

// -------- DS18B20 (non-blocking) --------
constexpr uint8_t  DS_RES_BITS = 9;   // ≈94 ms conversion at 9-bit
constexpr uint32_t DS_CONV_MS  = 94;

OneWire oneWire(PIN_ONEWIRE);
DallasTemperature dallas(&oneWire);
DeviceAddress dsAddr{};
bool   haveDS          = false;
float  lastTempC       = NAN;
bool   tempLatched     = false;
uint32_t tempReadyAtMs = 0;

// ===== pH Calibration (from your helper) =====
constexpr float CAL_M = -218.4242858887f;
constexpr float CAL_B = 660.9588623047f;
inline float ph_from_voltage(float v) { return CAL_M * v + CAL_B; }

// -------- Running average (ring buffer) --------
class RunningAverage {
public:
  explicit RunningAverage(uint8_t n = 16) : N(n) {
    if (N < 1) N = 1;
    buf = new float[N];
    for (uint8_t i = 0; i < N; ++i) buf[i] = 0.0f;
  }
  ~RunningAverage() { delete[] buf; }

  void reset() {
    idx = 0; filled = 0; sum = 0.0f;
    for (uint8_t i = 0; i < N; ++i) buf[i] = 0.0f;
  }

  float push(float x) {
    sum -= buf[idx];
    buf[idx] = x;
    sum += x;
    idx = (uint8_t)((idx + 1) % N);
    if (filled < N) filled++;
    return sum / (float)filled;
  }

private:
  float   *buf = nullptr;
  uint8_t  N   = 16;
  uint8_t  idx = 0;
  uint8_t  filled = 0;
  float    sum = 0.0f;
};

// Window sizes (samples within the 100 ms block). Try 8–32.
constexpr uint8_t AVG_N_TDS = 16;
constexpr uint8_t AVG_N_PH  = 16;

// -------- Helpers --------
inline float readVoltage(uint8_t pin, uint8_t oversample = 8) {
  // Oversample raw ADC counts and average
  uint32_t acc = 0;
  for (uint8_t i = 0; i < oversample; i++) acc += (uint16_t)analogRead(pin);
  float counts = (float)acc / oversample;
  return (counts / adcMax) * VREF;  // volts at pin
}

inline bool isFiniteF(float v) { return isfinite(v); }
inline float safeFloat(float v, float fallback = 0.0f) { return isFiniteF(v) ? v : fallback; }

void startDsConversion(uint32_t nowMs) {
  if (!haveDS) return;
  dallas.setWaitForConversion(false);
  dallas.requestTemperaturesByAddress(dsAddr);
  tempReadyAtMs = nowMs + DS_CONV_MS;
  tempLatched   = false;
}

void maybeLatchTemp(uint32_t nowMs) {
  if (!haveDS || tempLatched) return;
  if ((int32_t)(nowMs - tempReadyAtMs) >= 0) {
    float t = dallas.getTempC(dsAddr);
    if (t != DEVICE_DISCONNECTED_C && t > -60 && t < 125) lastTempC = t;
    tempLatched = true;
  }
}

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 3000)) { delay(10); }

  #if defined(analogReadResolution)
    analogReadResolution(12);  // nRF52: 12-bit ADC
    adcMax = 4095;
  #endif

  dallas.begin();
  if (dallas.getDeviceCount() > 0 && dallas.getAddress(dsAddr, 0) && dallas.validAddress(dsAddr)) {
    dallas.setResolution(dsAddr, DS_RES_BITS);
    haveDS = true;
  }

  pinMode(PIN_TDS, INPUT);
  pinMode(PIN_PH,  INPUT);
}

void loop() {
  static uint16_t blocksEmitted = 0;
  if (blocksEmitted >= BLOCK_COUNT) {
    while (1) { delay(1000); } // stop after requested number of blocks
  }

  // Per-block smoothing state
  RunningAverage tdsAvg(AVG_N_TDS);
  RunningAverage phAvg (AVG_N_PH);

  uint32_t blockStart = millis();
  startDsConversion(blockStart);

  // CSV header for this block
  Serial.println("timestamp,tds_v,tempC,ph_v,ph");

  // Stream lines within the 100 ms window
  while ((int32_t)(millis() - blockStart) < (int32_t)BLOCK_MS) {
    uint32_t now = millis();
    uint32_t ts  = now - blockStart;

    maybeLatchTemp(now);

    // Read raw volts and apply divider scales where applicable
    float tds_v_raw = readVoltage(PIN_TDS) * DIVIDER_SCALE_TDS;
    float ph_v_raw  = readVoltage(PIN_PH)  * DIVIDER_SCALE_PH;

    // Safety clamp
    tds_v_raw = safeFloat(tds_v_raw, 0.0f);
    ph_v_raw  = safeFloat(ph_v_raw,  0.0f);

    // Sliding-window averages (smoother values in CSV)
    float tds_v = tdsAvg.push(tds_v_raw);
    float ph_v  = phAvg.push(ph_v_raw);

    // Calibrated pH from smoothed ph_v
    float pH = ph_from_voltage(ph_v);

    // Output CSV line
    Serial.print(ts);          Serial.print(',');
    Serial.print(tds_v, 5);    Serial.print(',');
    if (isFiniteF(lastTempC)) Serial.print(lastTempC, 3);
    Serial.print(',');
    Serial.print(ph_v, 5);     Serial.print(',');
    Serial.println(pH, 3);
  }

  // End of block marker (single space line)
  Serial.println(" ");

  blocksEmitted++;
}
