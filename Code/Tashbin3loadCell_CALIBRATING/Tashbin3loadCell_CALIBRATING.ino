#include "HX711.h"

// พินสัญญาณสำหรับ Load Cell แต่ละตัว
uint8_t dataPin1 = 14; // DOUT
uint8_t clockPin1 = 12; // SCK


uint8_t dataPin2  = 4;   // D2
uint8_t clockPin2 = 5;  // D1

uint8_t dataPin3  = 13;  // D7
uint8_t clockPin3 = 15;  // D8

// วัตถุ HX711 สำหรับแต่ละ Load Cell
HX711 scale1, scale2, scale3;

void setup()
{
  Serial.begin(9600);
  Serial.println(__FILE__);
  Serial.print("HX711_LIB_VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  // เริ่มต้น Load Cell ทั้ง 3 ตัว
  scale1.begin(dataPin1, clockPin1);
  scale2.begin(dataPin2, clockPin2);
  scale3.begin(dataPin3, clockPin3);

  Serial.println("พร้อมเริ่มการคาลิเบรต");
}

void loop()
{
  calibrateAll(); // เรียกใช้ฟังก์ชันคาลิเบรตสำหรับทั้ง 3 ตัว
  delay(10000);   // หน่วงเวลาเพื่อให้การคาลิเบรตเสร็จก่อนเริ่มใหม่
}

void calibrateAll()
{
  // คาลิเบรต Load Cell แต่ละตัว
  Serial.println("\n--- CALIBRATING SCALE 1 ---");
  calibrate(scale1);

  Serial.println("\n--- CALIBRATING SCALE 2 ---");
  calibrate(scale2);

  Serial.println("\n--- CALIBRATING SCALE 3 ---");
  calibrate(scale3);
}

void calibrate(HX711 &scale)
{
  Serial.println("===========");
  Serial.println("remove all weight from the loadcell");
  while (Serial.available()) Serial.read(); // ล้าง Serial input

  Serial.println("and press enter\n");
  while (Serial.available() == 0); // รอให้กด Enter

  Serial.println("Determine zero weight offset");
  scale.tare(20); // เฉลี่ย 20 ค่าเพื่อหาค่า offset
  int32_t offset = scale.get_offset();

  Serial.print("OFFSET: ");
  Serial.println(offset);
  Serial.println();

  Serial.println("place a weight on the loadcell");
  while (Serial.available()) Serial.read(); // ล้าง Serial input

  Serial.println("enter the weight in (whole) grams and press enter");
  uint32_t weight = 0;
  while (Serial.peek() != '\n')
  {
    if (Serial.available())
    {
      char ch = Serial.read();
      if (isdigit(ch))
      {
        weight *= 10;
        weight = weight + (ch - '0');
      }
    }
  }
  Serial.print("WEIGHT: ");
  Serial.println(weight);

  scale.calibrate_scale(weight, 20); // คำนวณค่า scale factor
  float scaleFactor = scale.get_scale();

  Serial.print("SCALE:  ");
  Serial.println(scaleFactor, 6);

  Serial.print("\nuse scale.set_offset(");
  Serial.print(offset);
  Serial.print("); and scale.set_scale(");
  Serial.print(scaleFactor, 6);
  Serial.print(");\n");
  Serial.println("in the setup of your project");

  Serial.println("\n\n");
}
