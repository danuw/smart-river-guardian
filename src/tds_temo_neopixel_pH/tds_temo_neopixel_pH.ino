/*
  XIAO nRF52840  |  DS18B20 (D4) + TDS (A0, 5V) + pH (A2, 5V) + NeoPixel (D8, 5 LEDs)

  - Prints via BOTH SerialUSB (TinyUSB) and Serial each second:
      Temp (°C)
      TDS: raw / volts / EC25 / TDS ppm   (manual calc)
      pH : raw / volts / approx pH        (simple formula; calibrate later)
  - LED strip shows Red/Amber/Green by TDS thresholds (SCENARIO 1=drinking, 2=river)
*/

#include <Adafruit_TinyUSB.h>
Adafruit_USBD_CDC SerialUSB;                 // explicit TinyUSB CDC

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>
#include <Print.h>                            // for Print& helper

// --------------------- CONFIG ---------------------
#define SCENARIO     1      // 1 = drinking water, 2 = river water
#define ONE_WIRE_PIN 4      // DS18B20 data pin (D4) with 4.7k pull-up to 3V3
#define TDS_PIN      A0     // TDS analog out (board powered from 5V)
#define PH_PIN       A2     // pH board analog out PO (board powered from 5V)
#define NEOPIXEL_PIN 8      // WS2812B DIN
#define NUM_LEDS     5
#define LED_BRIGHT   100    // 0..255
const  float VREF   = 3.0f; // using AR_INTERNAL_3_0

#ifndef LED_BUILTIN
#define LED_BUILTIN  13
#endif

// --------------------- OBJECTS ---------------------
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature tempSensor(&oneWire);
Adafruit_NeoPixel strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// --------------------- HELPERS (TDS) ---------------------
// EC from voltage (polynomial fit from Gravity TDS docs)
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

// C++11-safe printer helper
void printLine(Print &out,
               float tempC,
               int tds_raw, float tds_v, float ec25, float tds_ppm,
               int ph_raw, float ph_v, float pH) {
  out.print("T=");          out.print(tempC, 2);
  out.print(" °C  |  TDS raw="); out.print(tds_raw);
  out.print(" V=");         out.print(tds_v, 3);
  out.print("  EC25=");     out.print(ec25, 1);
  out.print(" uS/cm  TDS~");out.print(tds_ppm, 0);
  out.print(" ppm  |  pH raw="); out.print(ph_raw);
  out.print(" V=");         out.print(ph_v, 3);
  out.print("  pH≈");       out.println(pH, 2);
}

// --------------------- STATE ---------------------
uint8_t rom[8];
bool haveDS18 = false;

// --------------------- SETUP ---------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Start both serial paths; do not block waiting for host
  SerialUSB.begin(115200);
  Serial.begin(115200);

  // NeoPixel
  strip.begin();
  strip.setBrightness(LED_BRIGHT);
  strip.show();

  // ADC setup: 10-bit + 3.0V internal ref
  analogReadResolution(10);
  analogReference(AR_INTERNAL_3_0);

  // DS18B20 init + detect
  tempSensor.begin();
  oneWire.reset_search();
  if (oneWire.search(rom)) {
    haveDS18 = true;
    SerialUSB.println("DS18B20 detected.");
    Serial.println("DS18B20 detected.");
  } else {
    SerialUSB.println("No DS18B20 found on D4 (will keep trying).");
    Serial.println("No DS18B20 found on D4 (will keep trying).");
  }

  SerialUSB.println("Init complete. Reading every 1s...");
  Serial.println("Init complete. Reading every 1s...");
}

// --------------------- LOOP ---------------------
void loop() {
  // Heartbeat LED
  static bool on = false;
  on = !on;
  digitalWrite(LED_BUILTIN, on);

  // ---- Temperature (DS18B20) ----
  float tempC = 25.0f;
  if (haveDS18) {
    tempSensor.requestTemperatures();
    float t = tempSensor.getTempCByIndex(0);
    if (t == DEVICE_DISCONNECTED_C) {
      haveDS18 = false;
    } else {
      tempC = t;
    }
  } else {
    // Try to rediscover occasionally
    static uint32_t lastTry = 0;
    if (millis() - lastTry > 2000) {
      lastTry = millis();
      oneWire.reset_search();
      if (oneWire.search(rom)) {
        haveDS18 = true;
        SerialUSB.println("DS18B20 detected again.");
        Serial.println("DS18B20 detected again.");
      }
    }
  }

  // ---- TDS (A0) ----
  int   tds_raw = analogRead(TDS_PIN);
  float tds_v   = tds_raw * (VREF / 1023.0f);
  float ec25    = ec25_from_voltage(tds_v, tempC);
  float tds_ppm = tds_from_ec25(ec25);

  // ---- pH (A2) ----
  int   ph_raw = analogRead(PH_PIN);
  float ph_v   = ph_raw * (VREF / 1023.0f);
  // Quick first-cut estimate (calibrate later with pH7 & pH4):
  // Around pH7 many boards ~2.5V; slope ~0.18V/pH (varies by board).
  float pH = 7.0f + ((2.5f - ph_v) / 0.18f);

  // ---- Debug prints (both streams) ----
  printLine(SerialUSB, tempC, tds_raw, tds_v, ec25, tds_ppm, ph_raw, ph_v, pH);
  printLine(Serial,    tempC, tds_raw, tds_v, ec25, tds_ppm, ph_raw, ph_v, pH);

  // ---- LED color by TDS threshold ----
  uint32_t color;
  if (SCENARIO == 1) {            // Drinking water
    // Green ≤500, Amber 500–1000, Red >1000 ppm
    color = (tds_ppm <= 500.0f)  ? strip.Color(0,255,0) :
            (tds_ppm <= 1000.0f) ? strip.Color(255,150,0) :
                                   strip.Color(255,0,0);
  } else {                        // River water
    // Green ≤250, Amber 250–1000, Red >1000 ppm
    color = (tds_ppm <= 250.0f)  ? strip.Color(0,255,0) :
            (tds_ppm <= 1000.0f) ? strip.Color(255,150,0) :
                                   strip.Color(255,0,0);
  }
  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, color);
  strip.show();

  delay(1000);
}
