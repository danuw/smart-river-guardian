#include <Adafruit_TinyUSB.h>

// Wiring: TDS powered from 5V (separate), GND tied to XIAO GND, AO -> XIAO A0
const int   PIN        = A0;     // A1 on your board looks damaged
const float VREF       = 3.0f;   // analogReference(AR_INTERNAL_3_0)
const int   ADC_RANGE  = 4095;   // 12-bit
float       temperature= 25.0f;  // TODO: replace with DS18B20
float       tdsFactor  = 0.5f;   // NaCl ≈ 0.5
float       kValue     = 1.0f;   // calibration factor (start at 1.0)

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);
  pinMode(PIN, INPUT);
}

void loop() {
  int   raw = analogRead(PIN);
  float v   = (raw * VREF) / ADC_RANGE;

  // Gravity EC polynomial: V (volts) -> EC (µS/cm), then apply K
  float ec   = (133.42f*v*v*v - 255.86f*v*v + 857.39f*v) * kValue;  // µS/cm
  if (ec < 0) ec = 0; // clamp

  // Temperature compensation to 25°C
  float ec25 = ec / (1.0f + 0.02f * (temperature - 25.0f));         // µS/cm

  // TDS in ppm: NO *1000 here (µS/cm * factor ≈ ppm for NaCl solutions)
  float tds  = ec25 * tdsFactor;                                    // ppm

  Serial.print("raw=");  Serial.print(raw);
  Serial.print("  V=");  Serial.print(v, 3);
  Serial.print("  EC25="); Serial.print(ec25, 1); Serial.print(" uS/cm");
  Serial.print("  TDS~"); Serial.print(tds, 0); Serial.println(" ppm");

  delay(1000);
}
