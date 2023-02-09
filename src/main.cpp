#include <M5Stack.h>
#include <IMU_6886.h>
#include <math.h>
/*
#ifdef __cplusplus
extern "C"
{
  #include "IMU_6886.h"
}
#endif
*/

#define RAD2DEG ( 57.2958)

IMU_6886 imu6886;

float accX = 0;
float accY = 0;
float accZ = 0;
float accX_offset = 0;
float accY_offset = 0;
float accZ_offset = 0;

float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;

float roll =0 ;
float pitch = 0;
float yow = 0;

float temp = 0;
int pre_output_time=0;

void accTest_init(void);
void accTest(void);
void accLevel_init(void);
void accLevel(void);


void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(100, 0);

  //  accTest_init();
  accLevel_init();
  
  imu6886.Init(21, 22);
  Serial.begin(115200);
  
  for (int i=1; i<50;i++)
    {
        imu6886.getAccelData(&accX,&accY,&accZ);
	accX_offset += accX;
	accY_offset += accY;
	accZ_offset += accZ;
	delay(100);
    }
  accX_offset /= 50;
  accY_offset /= 50;
  accZ_offset /= 50;
  
      
}

void loop() {
  // put your main code here, to run repeatedly:
  imu6886.getGyroData(&gyroX,&gyroY,&gyroZ);
  imu6886.getAccelData(&accX,&accY,&accZ);
  imu6886.getTempData(&temp);

  accX -= accX_offset;
  accY -= accY_offset;
  accZ -= ( accZ_offset -1);
    
  pitch = atan2(accY,accZ) * RAD2DEG;
  roll = atan2(accX , sqrt(pow(accY,2) + pow(accZ,2))) * RAD2DEG;

  //  accTest();
  accLevel();
  delay(100);
  
  //  Serial.printf("Angle : %.2f   %.2f   %.2f  \n ",roll ,pitch , yow);
  if((millis() - pre_output_time) > 1000)
    {
      Serial.printf("%.2f %.2f \n" ,roll , pitch);
	pre_output_time =millis();
	  }
}

void accTest_init()
{
  M5.Lcd.println("MPU6886 TEST");
  M5.Lcd.setCursor(10, 25);
  M5.Lcd.println("  X      Y       Z");
}
  
void accTest()
{
   M5.Lcd.setCursor(10, 70);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ", gyroX, gyroY,gyroZ);
  M5.Lcd.setCursor(270, 70);
  M5.Lcd.print("o/s");
  M5.Lcd.setCursor(10, 140);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ",accX ,accY , accZ);
  M5.Lcd.setCursor(270, 140);
  M5.Lcd.print("G");
  M5.Lcd.setCursor(10, 210);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ",roll ,pitch , yow);
  M5.Lcd.setCursor(270, 210);
  M5.Lcd.print("deg");
}

void accLevel_init()
{
  M5.Lcd.println("MPU6886 Level");
  M5.Lcd.setCursor(80,120 );
  M5.Lcd.println("Calibration...");
  M5.Lcd.drawCircle(160,120,100,TFT_GREEN);  
}
  

  
void accLevel()
{
  int x_position = 160;
  int y_positon = 120;
  

  x_position = 160 + roll*200/180;
  y_positon = 120 - pitch*200/180;
    

  M5.Lcd.fillCircle(160,120,99,TFT_BLACK);
  M5.Lcd.fillCircle(x_position,y_positon,10,TFT_RED);
}

  
