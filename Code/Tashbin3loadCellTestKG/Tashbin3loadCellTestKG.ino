#include "HX711.h"

// สร้างวัตถุ HX711 สำหรับแต่ละ Load Cell
HX711 scale1;
HX711 scale2;
HX711 scale3;

// พินสัญญาณสำหรับ Load Cell แต่ละตัว
uint8_t dataPin1  = 12;  // D6 sck
uint8_t clockPin1 = 14; // D5  dt

uint8_t dataPin2  = 4;   // D2
uint8_t clockPin2 = 5;  // D1

uint8_t dataPin3  = 13;  // D7
uint8_t clockPin3 = 15;  // D8

void setup()
{
  Serial.begin(9600);
  Serial.println(__FILE__);
  Serial.print("HX711_LIB_VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  // เริ่มต้น Load Cell แต่ละตัว
  scale1.begin(dataPin1, clockPin1);
  scale2.begin(dataPin2, clockPin2);
  scale3.begin(dataPin3, clockPin3);

  // ตั้งค่า Load Cell แต่ละตัว
  scale1.set_scale(208.627792); // ปรับค่าตามการสอบเทียบ
  scale1.tare(20);  // รีเซ็ตค่าเป็น 0
  scale2.set_scale(208.627792);
  scale2.tare(20);
  scale3.set_scale(208.627792);
  scale3.tare(20);

  Serial.println("Load Cells Initialized");
}

void loop()
{
  // อ่านน้ำหนักจาก Load Cell แต่ละตัว
  if (scale1.is_ready() && scale2.is_ready() && scale3.is_ready())
  {
    float weightGram1 = scale1.get_units(1);
    float weightGram2 = scale2.get_units(1);
    float weightGram3 = scale3.get_units(1);

    float weightKg1 = weightGram1 / 1000.0;  // แปลงหน่วยจากกรัมเป็นกิโลกรัม
    float weightKg2 = weightGram2 / 1000.0;
    float weightKg3 = weightGram3 / 1000.0;

    // แสดงน้ำหนักแต่ละตัวใน Serial Monitor
    Serial.print("Weight (Load Cell 1): ");
    Serial.print(weightKg1);
    Serial.println(" kg");

    Serial.print("Weight (Load Cell 2): ");
    Serial.print(weightKg2);
    Serial.println(" kg");

    Serial.print("Weight (Load Cell 3): ");
    Serial.print(weightKg3);
    Serial.println(" kg");

    Serial.println("-------------------------");
  }
  else
  {
    Serial.println("Load Cell(s) not ready");
  }

  delay(1000);  // หน่วงเวลา 1 วินาที
}
