#include <ESP8266WiFi.h> 
#include <TridentTD_LineNotify.h>
#include "HX711.h"

uint8_t dataPin  = 13;  //D6 //sck
uint8_t clockPin = 15; //D5 //DT

#define SSID        "zxcvbnm"
#define PASSWORD    "1234567890"
#define LINE_TOKEN  "xsHWA2MCWfpW7F30ujyY7lHjGC1dxU402oEGy1h5Vsa"

HX711 scale;

// ตัวแปรสถานะสำหรับการแจ้งเตือน
bool hasNotifiedCloseToFull = false;
bool hasNotifiedFull = false;
bool hasNotifiedEmpty = false; // ตัวแปรสำหรับสถานะ "นำไปเทแล้ว"

void setup() {
  Serial.begin(9600); Serial.println();
  Serial.println(LINE.getVersion());
  
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  

  LINE.setToken(LINE_TOKEN);

  Serial.println(__FILE__);
  Serial.print("HX711_LIB_VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);

  // Set scale factor (calibrate as per your load cell)
  scale.set_scale(208.627792); // ตัวเลขนี้ต้องถูกปรับตามการสอบเทียบ
  scale.tare(20);
  LINE.notify("พร้อมใช้งาน");
}

void loop() {
  if (scale.is_ready()) {
    float weightGram = scale.get_units(1);  // อ่านค่าที่ได้จาก load cell ในหน่วยกรัม
    float weightKg = weightGram / 1000.0;   // แปลงหน่วยจากกรัมเป็นกิโลกรัม

    // แสดงน้ำหนักเป็นกิโลกรัม
    Serial.print("Weight: ");
    Serial.print(weightKg);
    Serial.println(" kg");

    // เงื่อนไข "ใกล้เต็ม" (น้ำหนักระหว่าง 1 ถึง 3 กิโลกรัม)
    if (weightKg >= 1.00 && weightKg < 3.00 && !hasNotifiedCloseToFull) {
      LINE.notify("ถังขยะใกล้เต็มแล้ว (น้ำหนัก: " + String(weightKg, 2) + " กิโลกรัม)");
      hasNotifiedCloseToFull = true; // อัพเดทสถานะว่าได้แจ้งเตือน "ใกล้เต็ม" แล้ว
      hasNotifiedFull = false;      // รีเซ็ตสถานะ "เต็ม" เผื่อกรณีลดน้ำหนัก
    }
    // เงื่อนไข "เต็ม" (น้ำหนักตั้งแต่ 3 กิโลกรัมขึ้นไป)
    else if (weightKg >= 3.00 && !hasNotifiedFull) {
      LINE.notify("ถังขยะเต็มแล้ว กรุณานำขยะไปทิ้ง (น้ำหนัก: " + String(weightKg, 2) + " กิโลกรัม)");
      hasNotifiedFull = true;       // อัพเดทสถานะว่าได้แจ้งเตือน "เต็ม" แล้ว
      hasNotifiedEmpty = false;     // รีเซ็ตสถานะ "นำไปเท" เผื่อกรณีลดน้ำหนัก
    }
    // เงื่อนไข "นำขยะไปเทแล้ว" (น้ำหนักลดลงต่ำกว่า 1 กิโลกรัมหลังจากเต็ม)
    else if (weightKg < 1.00 && hasNotifiedFull && !hasNotifiedEmpty) {
      LINE.notify("ถังขยะถูกนำไปเทแล้ว ระบบพร้อมใช้งานใหม่");
      hasNotifiedEmpty = true;     // อัพเดทสถานะว่าได้แจ้งเตือน "นำไปเทแล้ว" แล้ว
      hasNotifiedCloseToFull = false; // รีเซ็ตสถานะ "ใกล้เต็ม"
      hasNotifiedFull = false;       // รีเซ็ตสถานะ "เต็ม"
    }
  }

  delay(1000);  // หน่วงเวลา 1 วินาทีเพื่อให้ดูผลลัพธ์
}
