#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// กำหนด Address (0x27 หรือ 0x3F), จำนวนตัวอักษร 16, จำนวนบรรทัด 2
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup() {
  // เริ่มต้นการทำงานของจอ (สำหรับ ESP8266 ต้องระบุขา SDA, SCL ใน Wire.begin ถ้าจำเป็น แต่ปกติ Library จัดการให้)
  // แต่ถ้าจอไม่ติด ลองใช้ lcd.begin(D2, D1); แทน init() ในบาง Library
  
  lcd.init();      // สั่ง initialize จอ
  lcd.backlight(); // สั่งเปิดไฟพื้นหลัง (Backlight)

  // แสดงข้อความบรรทัดที่ 1
  lcd.setCursor(0, 0); // (ตัวอักษรที่ 0, บรรทัดที่ 0)
  lcd.print("Hello ESP8266");

  // แสดงข้อความบรรทัดที่ 2
  lcd.setCursor(0, 1); // (ตัวอักษรที่ 0, บรรทัดที่ 1)
  lcd.print("I2C LCD Test");
}

void loop() {
  // ในตัวอย่างนี้ไม่มีอะไรใน loop เพราะแสดงข้อความค้างไว้เฉยๆ
  // หากต้องการทำนาฬิกาหรือนับเลข ให้เขียนโค้ด update หน้าจอที่นี่
}