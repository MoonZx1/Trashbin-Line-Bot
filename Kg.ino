#include "HX711.h"

HX711 scale;

// adjust pins if needed
uint8_t dataPin = 12;
uint8_t clockPin = 14;

void setup()
{
  Serial.begin(9600);
  Serial.println(__FILE__);
  Serial.print("HX711_LIB_VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);

  // Set scale factor (calibrate as per your load cell)
  scale.set_scale(208.627792); // ตัวเลขนี้ต้องถูกปรับตามการสอบเทียบ
  scale.tare(20);  // Reset scale to 0, take a few samples to clear any residual value
}
s
void loop()
{
  if (scale.is_ready())
  {
    float weightGram = scale.get_units(1);  // อ่านค่าที่ได้จาก load cell ในหน่วยกรัม
    float weightKg = weightGram / 1000.0;   // แปลงหน่วยจากกรัมเป็นกิโลกรัม

    // แสดงน้ำหนักเป็นกิโลกรัม
    Serial.print("Weight: ");
    Serial.print(weightKg);
    Serial.println(" kg");

    // คำสั่ง if-else สำหรับส่งข้อความตามน้ำหนัก
    if (weightKg >= 3.00 && weightKg <= 5.00) {
      Serial.println("น้ำหนักมากกว่า 5 กิโลกรัม");
    }
  else {
  }

  delay(1000);  // หน่วงเวลา 1 วินาทีเพื่อให้ดูผลลัพธ์
 }
}
