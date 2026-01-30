/*************************************************************
  Project: IoT Smart Gas Detector (Fan Delay 10s)
  Board: ESP8266 (NodeMCU)
 *************************************************************/

/* ข้อมูลจาก Blynk Template ของคุณ */
#define BLYNK_TEMPLATE_ID "TMPL6mFlx70-i"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "zS9rL_cpCxIJctXGMSlNHyTCH9W_t5uD"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

char ssid[] = "lxibel";
char pass[] = "louis8053";

// --- กำหนดขาอุปกรณ์ ---
#define MQ2_PIN A0      
#define LED_PIN D5      
#define BUZZER_PIN D6   
#define RELAY_PIN D7    

LiquidCrystal_I2C lcd(0x27, 16, 2); 

BlynkTimer timer;
int gasThreshold = 600; 
bool isAlerting = false; 

// ตัวแปรสำหรับนับถอยหลังพัดลม
int fanCooldownTimer = 0; 

void sendSensor()
{
  int sensorValue = analogRead(MQ2_PIN);
  Blynk.virtualWrite(V0, sensorValue);

  Serial.print("Gas: ");
  Serial.print(sensorValue);
  Serial.print(" | Fan Timer: ");
  Serial.println(fanCooldownTimer);

  // แสดงผลค่าแก๊สบรรทัดบน
  lcd.setCursor(0, 0); 
  lcd.print("Gas Val: ");
  lcd.print(sensorValue);
  lcd.print("    "); 

  // --- ตรวจสอบความปลอดภัย ---
  if (sensorValue > gasThreshold) {
    // >>> อันตราย (เกิน 600) <<<
    
    // ตั้งเวลารอไว้ที่ 10 วินาทีเสมอ ตราบใดที่ค่าแก๊สยังสูง
    fanCooldownTimer = 10; 

    digitalWrite(LED_PIN, HIGH);     // ไฟติด
    digitalWrite(BUZZER_PIN, LOW);   // เสียงดัง
    
    // เปิดพัดลม
    pinMode(RELAY_PIN, OUTPUT);      
    digitalWrite(RELAY_PIN, LOW);    
    
    lcd.setCursor(0, 1);
    lcd.print("STATUS: DANGER!");

    if (!isAlerting) {
      Blynk.logEvent("gas_alert", "Warning! Gas Leak Detected!"); 
      isAlerting = true;
    }
    
  } else {
    // >>> ค่าลดลงมาปกติแล้ว (ต่ำกว่า 600) <<<
    
    // ปิดเสียงและไฟทันที (ไม่ต้องรอ)
    digitalWrite(LED_PIN, LOW);      
    digitalWrite(BUZZER_PIN, HIGH);  
    isAlerting = false;

    // --- เช็คเงื่อนไขพัดลม (หน่วงเวลา 10 วิ) ---
    if (fanCooldownTimer > 0) {
      // ถ้ายังมีเวลาเหลือ ให้เปิดพัดลมต่อ
      pinMode(RELAY_PIN, OUTPUT);
      digitalWrite(RELAY_PIN, LOW);
      
      lcd.setCursor(0, 1);
      lcd.print("Clearing Smoke.."); // แจ้งเตือนว่ากำลังระบายควัน
      
      fanCooldownTimer--; // ลดเวลาลงทีละ 1 วินาที
    } 
    else {
      // ถ้าหมดเวลาแล้ว (เป็น 0) ให้ปิดพัดลมจริงๆ
      digitalWrite(RELAY_PIN, HIGH);   
      pinMode(RELAY_PIN, INPUT); // ตัดไฟ Relay
      
      lcd.setCursor(0, 1);
      lcd.print("STATUS: Safe    ");
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Relay เริ่มต้นแบบตัดวงจร
  pinMode(RELAY_PIN, INPUT);     
  digitalWrite(RELAY_PIN, HIGH); 

  digitalWrite(BUZZER_PIN, HIGH); 
  digitalWrite(LED_PIN, LOW);     

  Wire.begin(D2, D1); 
  lcd.begin(16, 2);   
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}