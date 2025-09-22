#define PH_PIN A2
const float VREF = 3.0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(10);
  analogReference(AR_INTERNAL_3_0);
  Serial.println("pH calibration test. Dip probe into buffer solution.");
}

void loop() {
  int raw = analogRead(PH_PIN);
  float v = raw * (VREF / 1023.0);

  Serial.print("raw=");
  Serial.print(raw);
  Serial.print("  V=");
  Serial.print(v, 3);
  Serial.println(" V");

  delay(1000);
}
qwert7yiuh