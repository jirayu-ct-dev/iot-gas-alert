#define LDR_PIN D0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int lightValue = analogRead(LDR_PIN);

  Serial.print("Light Value: ");
  Serial.println(lightValue);

  delay(500);
}