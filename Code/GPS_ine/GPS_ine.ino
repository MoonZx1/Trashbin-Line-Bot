#include <TridentTD_LineNotify.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// WiFi และ Line Notify
#define SSID        "Siriphon-2.4Ghz"  // ชื่อ WiFi
#define PASSWORD    "08955190"        // รหัสผ่าน WiFi
#define LINE_TOKEN  "xsHWA2MCWfpW7F30ujyY7lHjGC1dxU402oEGy1h5Vsa" // Line Notify Token

// GPS
#define RXPin 4 // GPIO4 (D2) รับข้อมูลจาก GPS TX
#define TXPin 5  // GPIO0 (D3) ส่งข้อมูลไปยัง GPS RX
SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200); 
  gpsSerial.begin(9600); // Start GPS serial communication
  
  Serial.println("Starting...");

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP: ");
  Serial.println(WiFi.localIP());

  // ตั้งค่า Line Notify Token
  LINE.setToken(LINE_TOKEN);
  LINE.notify("พร้อมใช้งาน");
}

void loop() {
  // อ่านข้อมูล GPS
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c); // แปลงข้อมูล GPS
  }

  // รับค่าละติจูดและลองจิจูดจาก GPS
  float lat = gps.location.lat();
  float lon = gps.location.lng();

  // แสดงข้อมูล GPS ใน Serial Monitor
  Serial.print("Latitude: ");
  Serial.print(lat, 6);
  Serial.print(" | Longitude: ");
  Serial.println(lon, 6);

  // ส่งข้อความพร้อมตำแหน่งไปที่ Line Notify
  String message = "https://www.google.com/maps/place/";
  message += String(lat, 6) + "," + String(lon, 6);
  LINE.notify(message);

  delay(1000); // ส่งข้อมูลทุก 1 วินาที
}
