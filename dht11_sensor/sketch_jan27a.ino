#include "DHT.h"

// กำหนดขาที่เชื่อมต่อ
#define DHTPIN D2     // ขา DO ของ DHT11 ต่อที่ขา D2 ของ NodeMCU
#define DHTTYPE DHT11 // กำหนดรุ่นเซนเซอร์ (ถ้าใช้ DHT22 ก็แก้เป็น DHT22)

// ประกาศ object dht
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 Test!");

  dht.begin(); // เริ่มต้นการทำงานของเซนเซอร์
}

void loop() {
  // DHT11 ควรอ่านค่าห่างกันอย่างน้อย 2 วินาที
  delay(2000);

  // อ่านค่าความชื้น (Humidity)
  float h = dht.readHumidity();
  // อ่านค่าอุณหภูมิ (Temperature) ในหน่วย Celsius
  float t = dht.readTemperature();

  // ตรวจสอบว่าอ่านค่าได้หรือไม่ (ถ้าสายหลุด หรือเซนเซอร์เสีย ค่าจะเป็น NaN)
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return; // จบลูปนี้ แล้วรอรอบหน้า
  }

  // แสดงผลที่ Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");
}