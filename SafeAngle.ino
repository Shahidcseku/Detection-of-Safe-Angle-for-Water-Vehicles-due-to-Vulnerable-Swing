#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <ADXL345.h>

//Init parameters 
float Y_Factor = 1.5;
// Servo motor
Servo myservo;
int pos = 0;    // variable to store the servo position

// Accelerometer + compass + gyro
ADXL345 adxl;

// LCD
LiquidCrystal lcd(12,13,8,9,10,11); // RS,EN,D4,D5,D6,D7

 //=============================================================== INITIALIZATION ============================================== 

void init_lcd()
{
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("Welcome");
  delay(500);
}
void play_sound()
{
    Serial.println("Danger!");
    pinMode(4,OUTPUT);
    digitalWrite(4,HIGH);

}
void init_adxl()
{
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
}
void init_motor()
{
myservo.attach(5);  // attaches the servo on pin 9 to the servo object
}
//=============================================================== SETUP ==============================================
void setup(){
  Serial.begin(9600);
  init_lcd();
  init_adxl();
  init_motor();
}
//=============================================================== LOOP ==============================================
void loop(){

 // Radian to degree
  double ax,ay,az;
  double xyz[3];
  int x,y,z;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
  
   ax = ax / 16384.0;
   ay = ay / 16384.0;
   az = az / 16384.0;
   
   x = RAD_TO_DEG * atan(ax / sqrt(ay * ay + az * az));
   y = RAD_TO_DEG * atan(ay / sqrt(ax * ax + az * az));
   z = RAD_TO_DEG * atan(az / sqrt(ay * ay + ax * ax));

   Serial.print("X = ");
   Serial.print(x);
   Serial.print(" Y= ");
   Serial.print(y);
   Serial.print(" Z= ");
   Serial.println(z);
    //
   lcd.setCursor(0,0);
   lcd.print("X=");
   lcd.print(x);
   lcd.print(" Y=");
   lcd.print(y);
   lcd.print(" Z=");
   lcd.println(z);
   
   double target = 0;
   double gain = sqrt(x*x+2*y*y);  // Minimum tolarable gain of x and y
   Serial.print("Gain = ");
   Serial.println(gain);

   if( x!=0 && (abs(x)>15 || abs(y)>7))  // Threshold vlaue from Tx and Ty
   {
     target = atan(abs(x)/(Y_Factor*abs(y)))/ 0.0174533;

//     Normalize
     if(target>90)
     {
      target = target/90.0;
     }

//     Direction Calculation
     if(x>=0 && y>=0)
     {
      target = abs(target);    
     }
     else if(x>=0 && y<0)
     {
      target = abs(target)*-1;    
     }
     else if(x<0 && y>=0)
     {
      target = abs(target)*-1;    
     }
     else if(x<0 && y<0)
     {
      target = abs(target);    
     }

     
     Serial.print("Target = ");
     Serial.println(target);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("X=");
     lcd.print(x);
     lcd.print(" Y=");
     lcd.print(y);
     lcd.print(" Z=");
     lcd.println(z);
     lcd.setCursor(0,1);
     lcd.print("Target= ");
     lcd.print(target);
     lcd.print((char)223);
     
     play_sound();
     myservo.write(target);
     delay(5000);
     lcd.clear();
   }
   else
   {
     lcd.setCursor(0,1);
     lcd.print("Safe!!");
   
     myservo.write(0);
   }
   
   delay(1000);
}
