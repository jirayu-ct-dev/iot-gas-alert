#define TRIG_PIN D2
#define ECHO_PIN D3

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // เคลียร์ TRIG
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // ส่งคลื่น Ultrasonic
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // รับค่าจาก ECHO
  duration = pulseIn(ECHO_PIN, HIGH);

  // คำนวณระยะทาง (cm)
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
