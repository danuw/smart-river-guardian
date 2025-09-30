/*
  XIAO nRF52840 | DS18B20 (D4) + TDS (A0, 5V) + pH (A2, 5V) + NeoPixel (D8, 5 LEDs)

  - EI_DATA_FORWARD=0 : human-readable debug
  - EI_DATA_FORWARD=1 : Edge Impulse Data Forwarder lines: tempC,tds_ppm,ph
  - DS18B20: robust address-based blocking read @12-bit (no "stuck at 25C")
  - TDS: manual EC polynomial + temp compensation (EC25), TDS=EC25*0.5
  - pH: simple 2-constant formula; calibrate with buffers
  - NeoPixel: R/A/G by TDS thresholds (SCENARIO 1=drinking, 2=river)

  Wiring:
    DS18B20: VCC->3V3, GND->GND, DATA->D4 with 4.7k pull-up D4->3V3 (NOT 5V)
    TDS board: V+->5V, GND->GND, OUT->A0
    pH board:  V+->5V, GND->GND, PO ->A2 (ignore DO/T0)
    NeoPixel:  V+->5V, GND->GND, DIN->D8 (~330Ω series recommended)
*/

#include <Adafruit_TinyUSB.h>
Adafruit_USBD_CDC SerialUSB;   // explicit TinyUSB CDC (we'll also print to Serial)

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>
#include <Print.h>              // for Print& helper

// --------------------- SWITCHES ---------------------
#define EI_DATA_FORWARD   1    // 0 = debug; 1 = Edge Impulse forwarder lines
#define EI_SAMPLING_HZ    1    // samples/sec in EI mode

// --------------------- CONFIG ---------------------
#define SCENARIO          1    // 1=drinking water, 2=river water
#define ONE_WIRE_PIN      4    // DS18B20 data pin (D4), 4.7k pull-up to 3V3
#define TDS_PIN           A0   // TDS analog out
#define PH_PIN            A2   // pH analog out (PO)
#define NEOPIXEL_PIN      8
#define NUM_LEDS          5
#define LED_BRIGHT        100
const float ADC_VREF = 3.0f;   // AR_INTERNAL_3_0 reference volts

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// --------------------- OBJECTS ---------------------
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature tempSensor(&oneWire);
Adafruit_NeoPixel strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// --------------------- pH CAL (set after buffer tests) ---------------------
// pH = 7 + (PH_V_AT_7 - Vmeas) / PH_SLOPE_VP
float PH_V_AT_7   = 2.50f;   // volts measured in pH7 buffer (typ. ~2.5 V)
float PH_SLOPE_VP = 0.18f;   // volts per pH unit from pH7 to pH4 (typ. ~0.18)

// --------------------- HELPERS ---------------------
// EC from voltage (DFRobot Gravity TDS polynomial)
static inline float ec_from_voltage(float v) {
  return (133.42f*v*v*v - 255.86f*v*v + 857.39f*v); // µS/cm
}
static inline float ec25_from_voltage(float v, float tempC) {
  float ec = ec_from_voltage(v);
  if (ec < 0) ec = 0;
  return ec / (1.0f + 0.02f * (tempC - 25.0f));     // temp compensated to 25°C
}
static inline float tds_from_ec25(float ec25) {
  return (ec25 * 0.5f); // ppm (NaCl factor)
}
static inline float ph_from_voltage(float v) {
  return 7.0f + ((PH_V_AT_7 - v) / PH_SLOPE_VP);
}

// C++11-safe printer for debug mode
void printDebugLine(Print &out,
                    float tempC,
                    int tds_raw, float tds_v, float ec25, float tds_ppm,
                    int ph_raw, float ph_v, float pH) {
  out.print("T=");            out.print(tempC, 2);
  out.print(" °C  |  TDS raw="); out.print(tds_raw);
  out.print(" V=");           out.print(tds_v, 3);
  out.print("  EC25=");       out.print(ec25, 1);
  out.print(" uS/cm  TDS~");  out.print(tds_ppm, 0);
  out.print(" ppm  |  pH raw="); out.print(ph_raw);
  out.print(" V=");           out.print(ph_v, 3);
  out.print("  pH~");         out.println(pH, 2);
}

// --------------------- STATE ---------------------
bool haveDS18 = false;
DeviceAddress dsAddr = {0};
static float lastGoodTempC = 25.0f;

// EI timing
#if EI_DATA_FORWARD
static const unsigned long EI_INTERVAL_MS = (1000UL / (EI_SAMPLING_HZ > 0 ? EI_SAMPLING_HZ : 1));
static unsigned long ei_last_ms = 0;
#endif

// Try to find a DS18B20 and store its address
bool findDS18B20() {
  DeviceAddress addr;
  oneWire.reset_search();
  while (oneWire.search(addr)) {
    // accept DS18B20 family (0x28) and verify via Dallas helper
    if (tempSensor.validAddress(addr) && (addr[0] == 0x28)) {
      memcpy(dsAddr, addr, sizeof(DeviceAddress));
      tempSensor.setResolution(dsAddr, 12);
      return true;
    }
  }
  return false;
}

// --------------------- SETUP ---------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Strongly recommended on nRF52 fallback OneWire: ensure pull-up on data pin
  pinMode(ONE_WIRE_PIN, INPUT_PULLUP);

  // Serial (both paths)
  SerialUSB.begin(115200);
  Serial.begin(115200);

  // ADC
  analogReadResolution(10);          // 0..1023
  analogReference(AR_INTERNAL_3_0);  // 3.0 V ref

  // DS18B20
  tempSensor.begin();
  tempSensor.setResolution(12);            // ~750 ms conversion
  tempSensor.setWaitForConversion(true);   // block until data ready
  delay(50);                               // small settle

  haveDS18 = findDS18B20();
  if (!EI_DATA_FORWARD) {
    if (haveDS18) {
      SerialUSB.println("DS18B20 detected (addressed reads).");
      Serial.println("DS18B20 detected (addressed reads).");
    } else {
      SerialUSB.println("No DS18B20 found (will keep trying).");
      Serial.println("No DS18B20 found (will keep trying).");
    }
  }

  // NeoPixel
  strip.begin();
  strip.setBrightness(LED_BRIGHT);
  strip.show();

  if (!EI_DATA_FORWARD) { SerialUSB.println("Init complete.\n"); Serial.println("Init complete.\n"); }
}

// --------------------- LOOP ---------------------
void loop() {
  // heartbeat
  static bool hb = false; hb = !hb;
  digitalWrite(LED_BUILTIN, hb);

  // ---- Temperature (address-based, blocking @12-bit) ----
  float tempC = lastGoodTempC;
  if (haveDS18) {
    tempSensor.requestTemperaturesByAddress(dsAddr);  // blocks until done
    float t = tempSensor.getTempC(dsAddr);
    if (t == DEVICE_DISCONNECTED_C || t < -55.0f || t > 125.0f) {
      // lost / bad read -> mark missing, keep last good
      haveDS18 = false;
#if !EI_DATA_FORWARD
      SerialUSB.println("DS18B20 read error, will rediscover...");
      Serial.println("DS18B20 read error, will rediscover...");
#endif
    } else {
      tempC = t;
      lastGoodTempC = t;
    }
  } else {
    // try to rediscover every 2s
    static uint32_t lastTry = 0;
    if (millis() - lastTry > 2000) {
      lastTry = millis();
      if (findDS18B20()) {
        haveDS18 = true;
#if !EI_DATA_FORWARD
        SerialUSB.println("DS18B20 detected again.");
        Serial.println("DS18B20 detected again.");
#endif
      }
    }
  }

  // ---- TDS (A0) ----
  int   tds_raw = analogRead(TDS_PIN);
  float tds_v   = tds_raw * (ADC_VREF / 1023.0f);
  float ec25    = ec25_from_voltage(tds_v, tempC);
  float tds_ppm = tds_from_ec25(ec25);

  // ---- pH (A2) ----
  int   ph_raw = analogRead(PH_PIN);
  float ph_v   = ph_raw * (ADC_VREF / 1023.0f);
  float pH     = ph_from_voltage(ph_v);   // set PH_V_AT_7 / PH_SLOPE_VP after calibration

  // ---- Output ----
#if EI_DATA_FORWARD
  // Emit one CSV line per sample: tempC,tds_ppm,ph
  unsigned long now = millis();
  if (now - ei_last_ms >= EI_INTERVAL_MS) {
    ei_last_ms = now;

    SerialUSB.print(tempC, 4);  SerialUSB.print(',');
    SerialUSB.print(tds_ppm, 4); SerialUSB.print(',');
    SerialUSB.println(pH, 4);

    Serial.print(tempC, 4);  Serial.print(',');
    Serial.print(tds_ppm, 4); Serial.print(',');
    Serial.println(pH, 4);
  }
#else
  // Human-readable debug on both streams
  printDebugLine(SerialUSB, tempC, tds_raw, tds_v, ec25, tds_ppm, ph_raw, ph_v, pH);
  printDebugLine(Serial,    tempC, tds_raw, tds_v, ec25, tds_ppm, ph_raw, ph_v, pH);
#endif

  // ---- NeoPixel by TDS thresholds ----
  uint32_t color;
  if (SCENARIO == 1) {            // Drinking water
    color = (tds_ppm <= 500.0f)  ? strip.Color(0,255,0) :
            (tds_ppm <= 1000.0f) ? strip.Color(255,150,0) :
                                   strip.Color(255,0,0);
  } else {                        // River water
    color = (tds_ppm <= 250.0f)  ? strip.Color(0,255,0) :
            (tds_ppm <= 1000.0f) ? strip.Color(255,150,0) :
                                   strip.Color(255,0,0);
  }
  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, color);
  strip.show();

#if !EI_DATA_FORWARD
  delay(1000); // keep 1 Hz in debug mode
#endif
}
