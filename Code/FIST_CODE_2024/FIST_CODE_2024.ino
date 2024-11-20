#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>
#include "HX711.h"

// พินสัญญาณสำหรับ Load Cell แต่ละตัว
uint8_t dataPin1  = D7;  
uint8_t clockPin1 = D6; 
uint8_t dataPin2  = D5;   
uint8_t clockPin2 = D2;    
uint8_t dataPin3  = D0;  
uint8_t clockPin3 = D1;

/* พินสัญญาณสำหรับ Relay 4 ch 
#define RELAY1 D8
#define RELAY2 1
#define RELAY3 3
#define RELAY4 9 */

#define SSID        "zxcvbnm"
#define PASSWORD    "1234567890"
#define LINE_TOKEN  "xsHWA2MCWfpW7F30ujyY7lHjGC1dxU402oEGy1h5Vsa"

// สร้างวัตถุ HX711 สำหรับแต่ละถัง
HX711 scale1;
HX711 scale2;
HX711 scale3;

// ตัวแปรสถานะสำหรับแต่ละถัง
bool hasNotifiedLowFull1 = false, hasNotifiedMediumFull1 = false, hasNotifiedFull1 = false, hasNotifiedEmpty1 = false; 
bool hasNotifiedLowFull2 = false, hasNotifiedMediumFull2 = false, hasNotifiedFull2 = false, hasNotifiedEmpty2 = false;
bool hasNotifiedLowFull3 = false, hasNotifiedMediumFull3 = false, hasNotifiedFull3 = false, hasNotifiedEmpty3 = false;

void setup() {
  Serial.begin(9600);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(400); }

  LINE.setToken(LINE_TOKEN);

  // เริ่มต้น Load Cell แต่ละตัว
  scale1.begin(dataPin1, clockPin1);
  scale2.begin(dataPin2, clockPin2);
  scale3.begin(dataPin3, clockPin3);

  // ตั้งค่า Load Cell แต่ละตัว
  scale1.set_offset(745098);
  scale1.set_scale(211.999008);
  scale1.tare(20);

  scale2.set_offset(719770);
  scale2.set_scale(223.998489);
  scale2.tare(20);

  scale3.set_offset(181523);
  scale3.set_scale(223.962357);
  scale3.tare(20);

  /* ตั้งค่า Relay
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW); */

  LINE.notify("ระบบพร้อมใช้งาน");
}

void loop() {
  // อ่านน้ำหนักจาก Load Cell แต่ละตัว
  float weightKg1 = scale1.is_ready() ? scale1.get_units(1) / 1000.0 : 0.0;
  float weightKg2 = scale2.is_ready() ? scale2.get_units(1) / 1000.0 : 0.0;
  float weightKg3 = scale3.is_ready() ? scale3.get_units(1) / 1000.0 : 0.0;

  // ตรวจสอบถัง
  handleTrashBin(1, weightKg1, hasNotifiedLowFull1, hasNotifiedMediumFull1, hasNotifiedFull1, hasNotifiedEmpty1);
  handleTrashBin(2, weightKg2, hasNotifiedLowFull2, hasNotifiedMediumFull2, hasNotifiedFull2, hasNotifiedEmpty2);
  handleTrashBin(3, weightKg3, hasNotifiedLowFull3, hasNotifiedMediumFull3, hasNotifiedFull3, hasNotifiedEmpty3);

  delay(1000);
}

// ฟังก์ชันจัดการสถานะและแจ้งเตือนของถังแต่ละถัง
void handleTrashBin(int binId, float weightKg, bool & hasNotifiedLowFull, bool &hasNotifiedMediumFull, bool &hasNotifiedFull, bool &hasNotifiedEmpty) {
  Serial.print("Trash Bin ");
  Serial.print(binId);
  Serial.print(": ");
  Serial.print(weightKg);
  Serial.println(" kg");

 // เต็มเล็กน้อย
  if (weightKg >= 2.00 && weightKg < 3.25 && !hasNotifiedLowFull) {
    LINE.notify("ถังขยะ " + String(binId) + " เต็มเล็กน้อย");
    hasNotifiedLowFull = true;
    hasNotifiedMediumFull = false;
  } 
  else if (weightKg >= 3.50 && weightKg < 4.50 && !hasNotifiedMediumFull) {
    LINE.notify("ถังขยะ " + String(binId) + " เต็มปลานกลาง");
    hasNotifiedMediumFull = true;
    hasNotifiedFull = false;
  }
  // เต็ม
  else if (weightKg >= 5.00 && !hasNotifiedFull) {
    LINE.notify("ถังขยะ " + String(binId) + " เต็มแล้ว กรุณานำไปเท");
    LINE.notify("https://www.google.com/maps/place/14.425016,99.905567");
    hasNotifiedFull = true;
    hasNotifiedEmpty = false;
    /* เปิด Relay1 (ตัวกลาง) และ RelayX (ตามถัง)
    digitalWrite(RELAY1, HIGH);
    digitalWrite(relayX, HIGH);  */
  }
  // กรณีถังว่างหลังเท
  else if (weightKg < 1.00 && hasNotifiedLowFull && hasNotifiedFull && !hasNotifiedEmpty) {
    LINE.notify("ถังขยะ " + String(binId) + " ถูกนำไปเทแล้ว");
    hasNotifiedEmpty = true;
    hasNotifiedMediumFull = false;
    hasNotifiedFull = false;

    //digitalWrite(RELAY1, LOW);
    //digitalWrite(relayX, LOW);
  }
}