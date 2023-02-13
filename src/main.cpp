#include <M5Stack.h>
#include <IMU_6886.h>
#include <Kalman.h>
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
#define CIRCLE_D (80) //level計の円の大きさ

IMU_6886 imu6886;
Kalman KalmanR;
Kalman KalmanP;

int t = 0, loop_t = 0, old_t = 0;
float dt = 0.0, print_time=0.0;


float accX = 0, accY = 0, accZ = 0;
float accX_offset = 0, accY_offset = 0, accZ_offset = 0;

float gyroX = 0, gyroY = 0, gyroZ = 0;
float gyroX_offset = 0, gyroY_offset = 0, gyroZ_offset = 0;

float roll =0, pitch = 0, yow = 0;
float roll_dot =0,pitch_dot = 0, yow_dot = 0;
float kal_roll = 0, kal_pitch = 0;

float temp = 0;
int pre_output_time=0;
bool disp_flg=true;


void get_RPY_angle(void);
void get_RPY_dot(void);
void cal_IMU_offset(void);
void accTest_LCDinit(void);
void accTest(void);
void accLevel_LCDinit(void);
void accLevel(void);



void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(100, 0);

  M5.Lcd.setCursor(80,120 );
  M5.Lcd.println("Calibration...");

  accTest_LCDinit();
  
  imu6886.Init(21, 22);
  cal_IMU_offset();
  Serial.begin(115200);


  get_RPY_angle();
  get_RPY_dot();
  
  KalmanR.setAngle(roll);
  KalmanP.setAngle(pitch);
      
}

void loop() {
  M5.update();
  
  t = micros();
  loop_t = t - old_t;
  old_t = t;
  dt = (float)loop_t / 1000000.0;
 
  get_RPY_angle();
  get_RPY_dot();

  
     
  ////// Use Kalman Filter///////////
    kal_roll = KalmanR.getAngle(roll, roll_dot ,dt);
    kal_pitch = KalmanP.getAngle(pitch, pitch_dot ,dt);

    if(M5.BtnA.wasPressed() && disp_flg==true)
      {
	disp_flg = false;
	accLevel_LCDinit();
	Serial.println("pushed A");
      }

    if(M5.BtnC.wasPressed() && disp_flg==false)
      {
	disp_flg = true;
	accTest_LCDinit();
	Serial.println("pushed C");
		     
      }

    if(disp_flg == true)
      {
	accTest();  //数値で表示
      }else{
       accLevel();   //レベル計で表示
    }
    
  delay(100);

  
  if((t - pre_output_time) > 100)
    {
      print_time = (float)t / 1000000.0;
      Serial.printf("%.2f %.2f %.2f %.2f %.2f \n" ,print_time, roll , pitch, kal_roll,kal_pitch );
      pre_output_time = t;
	  }
}

void cal_IMU_offset()
{

  for (int i=1; i<50;i++)
    {
        imu6886.getAccelData(&accX,&accY,&accZ);
	accX_offset += accX;
	accY_offset += accY;
	accZ_offset += accZ;

	imu6886.getGyroData(&gyroX,&gyroY,&gyroZ);
	gyroX_offset += gyroX;
	gyroY_offset += gyroY;
	gyroZ_offset += gyroZ;	

	delay(100);
	
    }

  accX_offset /= 50;
  accY_offset /= 50;
  accZ_offset /= 50;

  gyroX_offset /=50;
  gyroY_offset /=50;
  gyroZ_offset /=50;
}


void get_RPY_angle()
{
  
  imu6886.getAccelData(&accX,&accY,&accZ);

  accX -= accX_offset;
  accY -= accY_offset;
  accZ -= ( accZ_offset -1);
  
  roll = atan2(accY, accZ) * RAD2DEG;
  //  pitch = atan2(accX,accZ) * RAD2DEG;
  pitch =  atan2(-accX , sqrt(accY * accY + accZ * accZ)) * RAD2DEG;
}

void get_RPY_dot()
{
  imu6886.getGyroData(&gyroX,&gyroY,&gyroZ);
  roll_dot = gyroX - gyroX_offset;
  pitch_dot = gyroY - gyroY_offset;
  yow_dot = gyroZ - gyroZ_offset;
  
}


void accTest_LCDinit()
{
  M5.Lcd.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("MPU6886 TEST");
  M5.Lcd.setCursor(10, 30);
  M5.Lcd.println("  X      Y       Z");
}
  
void accTest()
{
   M5.Lcd.setCursor(10, 70);
   //  M5.Lcd.printf("%.2f   %.2f   %.2f   ", gyroX, gyroY,gyroZ);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ", roll_dot, pitch_dot, gyroZ);
  M5.Lcd.setCursor(270, 70);
  M5.Lcd.print("o/s");
  M5.Lcd.setCursor(10, 110);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ",accX ,accY , accZ);
  M5.Lcd.setCursor(270, 110);
  M5.Lcd.print("G");
  M5.Lcd.setCursor(10, 150);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ",roll ,pitch , yow);
  M5.Lcd.setCursor(270, 150);
  M5.Lcd.print("deg");
  M5.Lcd.setCursor(10, 190);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ",kal_roll ,kal_pitch , yow);
  M5.Lcd.setCursor(270, 190);
  M5.Lcd.print("deg");
}

void accLevel_LCDinit()
{
  M5.Lcd.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("MPU6886 Level");
  M5.Lcd.drawCircle(160,120,CIRCLE_D,TFT_GREEN);  
}
  

  
void accLevel()
{

  int x_position = 160;
  int y_position = 120;
  int kalx_position = 160;
  int kaly_position = 120;

  x_position = 160 - pitch*(CIRCLE_D*2)/180;
  y_position = 120 - roll*(CIRCLE_D*2)/180;
  kalx_position = 160 - kal_pitch*(CIRCLE_D*2)/180;
  kaly_position = 120 - kal_roll*(CIRCLE_D*2)/180;
    

  M5.Lcd.fillCircle(160,120, CIRCLE_D-1 ,TFT_BLACK);
  M5.Lcd.fillCircle(x_position,y_position,10,TFT_RED);   //フィルタなし
  M5.Lcd.fillCircle(kalx_position,kaly_position,10,TFT_BLUE);   //フィルタあり
  
}

  
