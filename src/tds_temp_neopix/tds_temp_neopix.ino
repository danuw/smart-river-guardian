/*
  XIAO nRF52840 + DS18B20 + TDS (manual calc) + NeoPixel (5 LEDs)
  Pins:
    - DS18B20 data -> D4  (4.7k pull-up to 3V3), VCC->3V3, GND->GND
    - TDS analog out -> A0, TDS VCC->5V (VBUS), GND common with XIAO
    - NeoPixel DIN -> D8, VCC->5V, GND common (330Ω in series on DIN recommended)
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>

// --------------------- CONFIG ---------------------
#define SCENARIO 1          // 1 = drinking water, 2 = river water
#define ONE_WIRE_PIN 4      // DS18B20 data pin (D4)
#define TDS_PIN      A0     // TDS analog output pin
#define NEOPIXEL_PIN 8      // WS2812B DIN (D8)
#define NUM_LEDS     5      // number of LEDs on your strip
#define LED_BRIGHT   100    // 0-255

// --------------------- OBJECTS --------------------
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature tempSensor(&oneWire);
Adafruit_NeoPixel strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// --------------------- HELPERS --------------------
uint8_t rom[8];
bool haveDS18 = false;

// Convert ADC voltage (V) to EC (µS/cm), apply temp compensation to EC25,
// then convert to TDS (ppm). Polynomial from DFRobot Gravity TDS docs.
float calcTDS_fromVoltage(float voltage, float tempC, float k = 1.0f) {
  // EC in µS/cm from sensor voltage (polynomial fit)
  float ec = (133.42f * voltage * voltage * voltage
             -255.86f * voltage * voltage
             +857.39f * voltage) * k;

  // Temperature compensation to 25°C (approx 2%/°C)
  float ec25 = ec / (1.0f + 0.02f * (tempC - 25.0f));

  // TDS conversion factor (commonly 0.5 for NaCl equivalence)
  float tds = ec25 * 0.5f;

  // Guard against negatives due to noise at very low V
  if (tds < 0) tds = 0;
  return tds;
}

float calcEC25_fromVoltage(float voltage, float tempC, float k = 1.0f) {
  float ec = (133.42f * voltage * voltage * voltage
             -255.86f * voltage * voltage
             +857.39f * voltage) * k;
  float ec25 = ec / (1.0f + 0.02f * (tempC - 25.0f));
  return (ec25 < 0) ? 0 : ec25;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // NeoPixel init
  strip.begin();
  strip.setBrightness(LED_BRIGHT);
  strip.show();

  // ADC setup: keep 10-bit since we use 1023 in scaling; set 3.0V ref
  analogReadResolution(10);
  analogReference(AR_INTERNAL_3_0);  // nRF52840 internal 3.0 V reference

  // DS18B20 init + detect
  tempSensor.begin();
  oneWire.reset_search();
  if (oneWire.search(rom)) {
    haveDS18 = true;
    Serial.println("DS18B20 detected.");
  } else {
    Serial.println("No DS18B20 found on the bus. Check D4 wiring & 4.7k pull-up.");
  }

  Serial.println("Init complete. Reading every 1s...\n");
}

void loop() {
  // ---- Read temperature ----
  float tempC = 25.0f;
  if (haveDS18) {
    tempSensor.requestTemperatures();
    float t = tempSensor.getTempCByIndex(0);
    if (t == DEVICE_DISCONNECTED_C) {
      Serial.println("DS18B20 not detected!");
      haveDS18 = false;  // stop hammering until rediscovered
    } else {
      tempC = t;
    }
  } else {
    // Try to rediscover periodically
    static uint32_t lastTry = 0;
    if (millis() - lastTry > 2000) {
      lastTry = millis();
      oneWire.reset_search();
      if (oneWire.search(rom)) {
        haveDS18 = true;
        Serial.println("DS18B20 detected again.");
      }
    }
  }

  // ---- Read TDS analog ----
  int raw = analogRead(TDS_PIN);
  float v = raw * (3.0f / 1023.0f);  // because AR_INTERNAL_3_0 & 10-bit ADC

  // ---- Calculate EC25 and TDS (manual) ----
  float ec25 = calcEC25_fromVoltage(v, tempC);  // µS/cm at 25°C
  float tds  = ec25 * 0.5f;                     // ppm

  // ---- Debug print (matches your earlier style) ----
  Serial.print("T=");
  Serial.print(tempC, 2);
  Serial.print("  °C  |  raw=");
  Serial.print(raw);
  Serial.print("  V=");
  Serial.print(v, 3);
  Serial.print("  |  EC25=");
  Serial.print(ec25, 1);
  Serial.print("  uS/cm  |  TDS~");
  Serial.print(tds, 0);
  Serial.println("  ppm");

  // ---- LED color by scenario thresholds ----
  uint32_t color;

  if (SCENARIO == 1) { // Drinking water
    // Green ≤500, Amber 500–1000, Red >1000 ppm
    color = (tds <= 500.0f)  ? strip.Color(0,255,0) :
            (tds <= 1000.0f) ? strip.Color(255,150,0) :
                               strip.Color(255,0,0);
  } else {             // River water
    // Green ≤250, Amber 250–1000, Red >1000 ppm
    color = (tds <= 250.0f)  ? strip.Color(0,255,0) :
            (tds <= 1000.0f) ? strip.Color(255,150,0) :
                               strip.Color(255,0,0);
  }

  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, color);
  strip.show();

  delay(1000);
}
