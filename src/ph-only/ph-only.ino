// XIAO nRF52840 + pH board test (PO -> A2).
// Prints via BOTH SerialUSB (TinyUSB) and Serial.
// LED heartbeat proves the loop runs even if no serial is open.

#include <Adafruit_TinyUSB.h>
Adafruit_USBD_CDC SerialUSB;

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define PH_PIN A2     // pH board analog output "P"/"PO" to XIAO A2
const float VREF = 3.0f;   // using AR_INTERNAL_3_0 (try 3.3 if you switch to AR_VDD4)

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Start both serial paths; don't block waiting for host
  SerialUSB.begin(115200);
  Serial.begin(115200);

  // ADC: 10-bit (0..1023). Use 3.0 V internal ref for nRF52840.
  analogReadResolution(10);
  analogReference(AR_INTERNAL_3_0);
  // If you still see nothing later, try:
  // analogReference(AR_VDD4);  // then change VREF to 3.3f above

  SerialUSB.println("pH sensor test starting (SerialUSB)...");
  Serial.println("pH sensor test starting (Serial)...");
}

void loop() {
  // Heartbeat
  static bool on = false;
  on = !on;
  digitalWrite(LED_BUILTIN, on);

  int   raw = analogRead(PH_PIN);
  float v   = raw * (VREF / 1023.0f);

  // Quick first-cut estimate (you will calibrate with pH7/pH4)
  // Around pH 7 many boards ~2.5V; slope ~0.18V per pH.
  float pH = 7.0f + ((2.5f - v) / 0.18f);

  // Print to BOTH streams so whichever port the IDE binds shows data
  SerialUSB.print("raw="); SerialUSB.print(raw);
  SerialUSB.print("  V="); SerialUSB.print(v, 3);
  SerialUSB.print(" V  ->  pH≈ "); SerialUSB.println(pH, 2);

  Serial.print("raw="); Serial.print(raw);
  Serial.print("  V="); Serial.print(v, 3);
  Serial.print(" V  ->  pH≈ "); Serial.println(pH, 2);

  delay(1000);
}
