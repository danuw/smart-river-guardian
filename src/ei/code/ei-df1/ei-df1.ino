// XIAO nRF52840 — Edge Impulse CSV blocks (TinyUSB)
// Columns (pH last): timestamp,tds_v,tempC,ph_v
// Behavior:
//   - Repeat blocks of sampling with duration BLOCK_MS (default 100 ms)
//   - At the start of each block: print header
//   - During the block: print lines (no fixed delay) with timestamp=ms since block start
//   - End the block with a single space line
//   - Do this exactly BLOCK_COUNT times, then stop

#include <Adafruit_TinyUSB.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// -------- Pins --------
constexpr uint8_t PIN_ONEWIRE = 4;   // DS18B20 data on D4 (with 4.7k pull-up to 3V3)
constexpr uint8_t PIN_TDS     = A0;  // TDS analog
constexpr uint8_t PIN_PH      = A1;  // pH analog

// -------- ADC / scaling --------
constexpr float VREF = 3.3f;   // XIAO nRF5280 ADC reference
int adcMax = 1023;

// If sensors run at 5V and you used a 2:1 divider on their analog outputs, set scale=2.0.
// If output never exceeds 3.3V, set scale=1.0.
constexpr float DIVIDER_SCALE_TDS = 2.0f;
constexpr float DIVIDER_SCALE_PH  = 2.0f;

// -------- Block configuration --------
constexpr uint32_t BLOCK_MS    = 100; // sampling window per block (ms) — change if needed
constexpr uint16_t BLOCK_COUNT = 100; // number of blocks to emit before stopping

// -------- DS18B20 (non-blocking) --------
// 9-bit resolution ≈ 94 ms conversion -> fits inside a 100 ms block
constexpr uint8_t  DS_RES_BITS = 9;
constexpr uint32_t DS_CONV_MS  = 94;

OneWire oneWire(PIN_ONEWIRE);
DallasTemperature dallas(&oneWire);
DeviceAddress dsAddr{};
bool   haveDS          = false;
float  lastTempC       = NAN;
bool   tempLatched     = false;
uint32_t tempReadyAtMs = 0;

// -------- Helpers --------
float readVoltage(uint8_t pin, uint8_t samples = 4) {
  long acc = 0;
  for (uint8_t i = 0; i < samples; i++) acc += analogRead(pin);
  float avg = (float)acc / samples;
  return (avg / adcMax) * VREF;
}

void startDsConversion(uint32_t nowMs) {
  if (!haveDS) return;
  dallas.setWaitForConversion(false);   // don't block
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
  // Clean USB CDC via TinyUSB
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 3000)) { delay(10); }

  // Better ADC resolution
  #if defined(analogReadResolution)
    analogReadResolution(12);
    adcMax = 4095;
  #endif

  // DS18B20 init
  dallas.begin();
  if (dallas.getDeviceCount() > 0 && dallas.getAddress(dsAddr, 0) && dallas.validAddress(dsAddr)) {
    dallas.setResolution(dsAddr, DS_RES_BITS);
    haveDS = true;
  }
}

void loop() {
  static uint16_t blocksEmitted = 0;
  if (blocksEmitted >= BLOCK_COUNT) {
    while (1) { delay(1000); } // finished — stop forever
  }

  // Start of block
  uint32_t blockStart = millis();
  startDsConversion(blockStart);

  // Print header for this block
  Serial.println("timestamp,tds_v,tempC,ph_v");

  // Sample as fast as possible within the 100 ms window
  while ((int32_t)(millis() - blockStart) < (int32_t)BLOCK_MS) {
    uint32_t now = millis();
    uint32_t ts  = now - blockStart;

    // latch temperature when conversion is ready
    maybeLatchTemp(now);

    // read analogs and scale (pH last)
    float tds_v = readVoltage(PIN_TDS) * DIVIDER_SCALE_TDS;
    float ph_v  = readVoltage(PIN_PH)  * DIVIDER_SCALE_PH;

    // output CSV line
    Serial.print(ts);       Serial.print(',');
    Serial.print(tds_v, 5); Serial.print(',');
    if (isnan(lastTempC)) Serial.print(""); else Serial.print(lastTempC, 3);
    Serial.print(',');
    Serial.println(ph_v, 5);
  }

  // End block with a single space line
  Serial.println("");

  blocksEmitted++;
}
