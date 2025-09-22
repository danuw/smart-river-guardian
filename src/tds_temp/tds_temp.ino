#include <Adafruit_TinyUSB.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- TDS wiring ---
const int   TDS_PIN    = A0;     // TDS AO -> A0 (A1 on your board is damaged)
const float VREF       = 3.0f;   // analogReference(AR_INTERNAL_3_0)
const int   ADC_RANGE  = 4095;   // 12-bit ADC
float       tdsFactor  = 0.5f;   // NaCl factor (~0.5)
float       kValue     = 1.0f;   // calibration factor; adjust after calibrating

// --- DS18B20 wiring ---
const int   ONEWIRE_PIN = D4;    // DS18B20 DATA -> D2, 4.7k pull-up to 3V3
OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // ADC setup
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);
  pinMode(TDS_PIN, INPUT);

  // DS18B20 start
  pinMode(ONEWIRE_PIN, INPUT);   // external 4.7k pull-up is recommended
  sensors.begin();
}

void loop() {
  // 1) Read temperature (°C)
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  if (temperature == DEVICE_DISCONNECTED_C || temperature < -50 || temperature > 125) {
    temperature = 25.0f; // fallback if probe not detected
  }

  // 2) Read TDS
  int   raw = analogRead(TDS_PIN);
  float v   = (raw * VREF) / ADC_RANGE;

  // Gravity (DFRobot) EC polynomial: V -> EC (µS/cm), apply K
  float ec   = (133.42f*v*v*v - 255.86f*v*v + 857.39f*v) * kValue; // µS/cm
  if (ec < 0) ec = 0;

  // Temperature compensation to 25 °C
  float ec25 = ec / (1.0f + 0.02f * (temperature - 25.0f));        // µS/cm

  // ppm estimate
  float tds  = ec25 * tdsFactor;                                   // ppm

  // 3) Print
  Serial.print("T=");   Serial.print(temperature, 2); Serial.print(" °C  ");
  Serial.print("raw="); Serial.print(raw);
  Serial.print("  V="); Serial.print(v, 3);
  Serial.print("  EC25="); Serial.print(ec25, 1); Serial.print(" uS/cm  ");
  Serial.print("TDS~"); Serial.print(tds, 0); Serial.println(" ppm");

  delay(1000);
}
