/*************************************************************
  Project: IoT Gas Detector (Mirror LCD to Blynk)
  Board: ESP8266 (NodeMCU)
 *************************************************************/

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

// จอจริง (Hardware LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// จอในแอป (Blynk Widget LCD) -> ใช้ V3
WidgetLCD blynkLCD(V3); 

BlynkTimer timer;

// ตัวแปร Global
int modeLevel = 2;       
int gasThreshold = 500;  
bool manualFan = false;  
int fanDelay = 0;        

// --- ฟังก์ชันสั่ง Relay (แบบตัดขา แก้ค้าง) ---
void turnFanON() {
  pinMode(RELAY_PIN, OUTPUT);   
  digitalWrite(RELAY_PIN, LOW); 
}

void turnFanOFF() {
  digitalWrite(RELAY_PIN, HIGH); 
  pinMode(RELAY_PIN, INPUT);     
}

// --- รับค่าต่างๆ จาก Blynk ---
BLYNK_WRITE(V1) {
  modeLevel = param.asInt();
  switch (modeLevel) {
    case 1: gasThreshold = 300; break; 
    case 2: gasThreshold = 500; break; 
    case 3: gasThreshold = 800; break; 
    default: gasThreshold = 500;
  }
}

BLYNK_WRITE(V2) {
  manualFan = param.asInt();
}

void sendSensor()
{
  int sensorValue = analogRead(MQ2_PIN);
  Blynk.virtualWrite(V0, sensorValue); 

  // --- เตรียมข้อความบรรทัดที่ 1 (ค่าแก๊ส) ---
  String line1 = "Gas:" + String(sensorValue) + " Lim:" + String(gasThreshold);
  
  // แสดงบนจอจริง
  lcd.setCursor(0, 0); 
  lcd.print(line1 + "   "); // เติมช่องว่างทับตัวเก่า
  
  // แสดงบนแอป (x=0, y=0)
  blynkLCD.print(0, 0, line1 + "     "); 

  // --- Logic หลัก ---
  String line2 = ""; // ตัวแปรเก็บข้อความบรรทัด 2

  // 1. เช็กปุ่ม Manual
  if (manualFan == true) {
    turnFanON(); 
    
    if (sensorValue <= gasThreshold) {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH); 
      line2 = "Mode: Manual ON ";
    } else {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
      line2 = "DANGER + MANUAL ";
    }
  }
  // 2. ระบบ Auto
  else if (sensorValue > gasThreshold) {
    // >> อันตราย <<
    fanDelay = 10; 
    turnFanON(); 
    digitalWrite(LED_PIN, HIGH);    
    digitalWrite(BUZZER_PIN, LOW);  
    
    line2 = "STATUS: DANGER! ";

    static bool alertSent = false;
    if (!alertSent) {
      Blynk.logEvent("gas_alert", "Gas Detected!"); 
      alertSent = true;
    }
  } 
  else {
    // >> ปลอดภัย <<
    digitalWrite(LED_PIN, LOW);     
    digitalWrite(BUZZER_PIN, HIGH); 

    if (fanDelay > 0) {
      turnFanON(); 
      line2 = "Clearing: " + String(fanDelay) + "s   ";
      fanDelay--; 
    } else {
      turnFanOFF(); 
      line2 = "STATUS: Safe    ";
    }
  }

  // --- ส่งข้อความบรรทัดที่ 2 ขึ้นจอทั้งสอง ---
  
  // จอจริง
  lcd.setCursor(0, 1);
  lcd.print(line2);

  // จอแอป (x=0, y=1)
  blynkLCD.print(0, 1, line2 + "     ");
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  pinMode(RELAY_PIN, INPUT);
  digitalWrite(RELAY_PIN, HIGH); 

  digitalWrite(BUZZER_PIN, HIGH); 
  digitalWrite(LED_PIN, LOW);     

  Wire.begin(D2, D1); 
  lcd.begin(16, 2);   
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Loading..");
  
  // เคลียร์จอแอปก่อนเริ่ม
  blynkLCD.clear(); 

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.syncVirtual(V1); 
  Blynk.syncVirtual(V2);

  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}