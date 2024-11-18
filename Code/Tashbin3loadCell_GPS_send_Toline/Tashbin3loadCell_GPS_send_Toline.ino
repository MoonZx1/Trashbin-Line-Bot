#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "HX711.h"

// WiFi และ Line Notify
#define SSID        "zxcvbnm"
#define PASSWORD    "1234567890"
#define LINE_TOKEN  "xsHWA2MCWfpW7F30ujyY7lHjGC1dxU402oEGy1h5Vsa"

// GPS
#define RXPin 4   // GPS RX pin (to GPS TX)
#define TXPin 5  // GPS TX pin (to GPS RX)
SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

// Load Cell
uint8_t dataPin =  0; // D3
uint8_t clockPin = 2; // D4

float calibrationFactor = 208.627792;
HX711 scale;

// ตัวแปรสถานะสำหรับการแจ้งเตือน
bool hasNotifiedCloseToFull = false;
bool hasNotifiedFull = false;
bool hasNotifiedEmpty = false;

void setup() {
  Serial.begin(115200);

  // เริ่มต้น GPS
  gpsSerial.begin(9600);

  // เชื่อมต่อ WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println("\nWiFi connected. IP Address: " + WiFi.localIP().toString());

  // ตั้งค่า Line Notify
  LINE.setToken(LINE_TOKEN);

  // เริ่มต้น Load Cell
  scale.begin(dataPin, clockPin);
  scale.set_scale(calibrationFactor);
  scale.tare(20);
  Serial.println("Load Cell Initialized");

  LINE.notify("ระบบพร้อมใช้งาน");
}

void loop() {
  // อ่านข้อมูล GPS
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c); // แปลงข้อมูล GPS
  }

  // อ่านน้ำหนักจาก Load Cell
  if (scale.is_ready()) {
    float weightKg = scale.get_units(1) / 1000.0;
    handleTrashBin(weightKg);
  }

  delay(1000); // หน่วงเวลา 1 วินาที
}

// จัดการสถานะและแจ้งเตือน
void handleTrashBin(float weightKg) {
  Serial.print("Weight: ");
  Serial.print(weightKg);
  Serial.println(" kg");

  if (weightKg >= 1.00 && weightKg < 3.00 && !hasNotifiedCloseToFull) {
    sendNotification("ใกล้เต็มแล้ว", weightKg);
    hasNotifiedCloseToFull = true;
    hasNotifiedFull = false;
    hasNotifiedEmpty = false;
  } else if (weightKg >= 3.00 && !hasNotifiedFull) {
    sendNotification("เต็มแล้ว กรุณานำไปเท", weightKg);
    hasNotifiedFull = true;
    hasNotifiedCloseToFull = false;
    hasNotifiedEmpty = false;
  } else if (weightKg < 1.00 && hasNotifiedFull && !hasNotifiedEmpty) {
    sendNotification("ถูกนำไปเทแล้ว ระบบพร้อมใช้งานใหม่", weightKg);
    hasNotifiedEmpty = true;
    hasNotifiedCloseToFull = false;
    hasNotifiedFull = false;
  }
}

// ฟังก์ชันส่งแจ้งเตือน
void sendNotification(String message, float weightKg) {
  String locationMessage = "";

  if (gps.location.isValid()) {
    float lat = gps.location.lat();
    float lon = gps.location.lng();
    locationMessage = " พิกัด: https://www.google.com/maps/place/" + String(lat, 6) + "," + String(lon, 6);
    Serial.print("Latitude: ");
    Serial.print(lat, 6);
    Serial.print(" | Longitude: ");
    Serial.println(lon, 6);
  } else {
    locationMessage = " (ไม่สามารถรับพิกัด GPS ได้)";
  }

  String notification = "ถังขยะ " + message + " (น้ำหนัก: " + String(weightKg, 2) + " กิโลกรัม)" + locationMessage;
  LINE.notify(notification);
  Serial.println(notification);
}
