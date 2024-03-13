#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <MPU6050_light.h>
// #include <SoftwareSerial.h>
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <SPI.h>
#include <Wire.h>
#include "motor.h"
#include "sensor.h"

// Motor Feedback
#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B
Encoder ECDA(ECDAA, ECDAB);
Encoder ECDB(ECDBA, ECDBB);
Encoder ECDC(ECDCA, ECDCB);
Encoder ECDD(ECDDA, ECDDB);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 28    // 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int window_size = 0;

// Serial Communication
#define SERIAL Serial
// #define BTSERIAL Serial3 // BT Module on Serial 3 (D14 & D15)

// PWM Definition
#define MAX_PWM 2000
#define MIN_PWM 300
int Motor_PWM = 1900;

MPU6050 mpu(Wire);
Sensor sensor;
Motor motor;

unsigned long time;

enum PARKING_STATE
{
  IDLE,
  START,
  MOVE_25CM,
  TURN_ANGLE,
  MEASURE,
  TRANSFER,
  PARKING
} PARKING_STATE;

// String Control_Display(char command)
// {
//   switch (command)
//   {
//   case 'A':
//     return "Forward";
//   case 'B':
//     return "Forward Right";
//   case 'C':
//     return "Rotate Right";
//   case 'D':
//     return "Backward Right";
//   case 'E':
//     return "Backward";
//   case 'F':
//     return "Backward Left";
//   case 'G':
//     return "Rotate Left";
//   case 'H':
//     return "Forward Left";
//   case 'Z':
//     return "Stop";
//   case 'z':
//     return "Stop";
//   case 'd':
//     return "Left";
//   case 'b':
//     return "Right";
//   case 'L':
//     return "1500";
//   case 'M':
//     return "500";
//   }
// }

void SR_Control()
{
  char SR_Data = 0;
  /*
    Receive data from Serial and translate it to motor movements
  */
  if (SERIAL.available())
  {
    SR_Data = SERIAL.read();
    SERIAL.flush();
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println("SR_Data = ");
    display.println(SR_Data);
    display.display();
    wheelControl(SR_Data);
  }
}

// void BT_Control()
// {
//   char BT_Data = 0;
//   /*
//     Receive data from app and translate it to motor movements
//   */
//   if (BTSERIAL.available())
//   {
//     BT_Data = BTSERIAL.read();
//     BTSERIAL.flush();
//     BT_alive_cnt = 100;
//     display.clearDisplay();
//     display.setCursor(0, 0); // Start at top-left corner
//     display.println("BT_Data = ");
//     display.println(BT_Data);
//     display.display();
//     wheelControl(BT_Data);
//   }
// }

void wheelControl(char data)
{
  switch (data)
  {
  case 'A':
    motor.ADVANCE(Motor_PWM);
    break;
  case 'B':
    motor.ADVANCE_RIGHT(Motor_PWM);
    break;
  case 'C':
    motor.ROTATE_CW(Motor_PWM);
    break;
  case 'D':
    motor.BACK_RIGHT(Motor_PWM);
    break;
  case 'E':
    motor.BACK(Motor_PWM);
    break;
  case 'F':
    motor.BACK_LEFT(Motor_PWM);
    break;
  case 'G':
    motor.ROTATE_CCW(Motor_PWM);
    break;
  case 'H':
    motor.ADVANCE_LEFT(Motor_PWM);
    break;
  case 'Z':
    motor.STOP();
    break;
  case 'z':
    motor.STOP();
    break;
  case 'd':
    motor.LEFT(Motor_PWM);
    break;
  case 'b':
    motor.RIGHT(Motor_PWM);
    break;
  case 'L':
    Motor_PWM = 1500;
    break;
  case 'M':
    Motor_PWM = 500;
    break;
  default:
    break;
  }
}

void sendSensor()
{
  SERIAL.print("[LL: ");
  SERIAL.print(sensor.getLightL());
  SERIAL.print(" LR: ");
  SERIAL.print(sensor.getLightR());
  SERIAL.print(" DT: ");
  SERIAL.print(sensor.getDistance() * 0.0001);
  SERIAL.print(" AX: ");
  SERIAL.print(mpu.getAngleX());
  SERIAL.print(" AY: ");
  SERIAL.print(mpu.getAngleY());
  SERIAL.print(" AZ: ");
  SERIAL.print(mpu.getAngleZ());
  SERIAL.print(" EA: ");
  SERIAL.print(ECDA.read());
  SERIAL.print(" EB: ");
  SERIAL.print(ECDB.read());
  SERIAL.print(" EC: ");
  SERIAL.print(ECDC.read());
  SERIAL.print(" ED: ");
  SERIAL.print(ECDD.read());
  SERIAL.println("]");
}

void setup()
{
  SERIAL.begin(115200);
  motor.STOP();
  // BTSERIAL.begin(9600); // BT serial setup

  // OLED Setup - Address 0x3C for 128x32
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 0);             // Start at top-left corner
  display.println("Calibrating...");
  display.display();

  // MPU6050 Setup
  Wire.begin();
  mpu.begin();
  delay(1000);
  mpu.calcGyroOffsets();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Alt-F4!");
  display.display();
}

void loop()
{
  switch (PARKING_STATE)
  {
  case IDLE:
    if (0)
    {
      PARKING_STATE = START;
    }
    break;
  case START:
    delay(2000);
    PARKING_STATE = MOVE_25CM;
    break;
  case MOVE_25CM:
    // Move to a location of 25cm from the wall, and wait for 2 sec.
    delay(2000);
    PARKING_STATE = TURN_ANGLE;
    break;
  case TURN_ANGLE:
    // Turn **CW 90°, wait 2 sec → CCW 270°, wait 2 sec → CW 180°, wait 2 sec**.
    delay(2000);
    delay(2000);
    delay(2000);
    PARKING_STATE = MEASURE;
    break;

  case MEASURE:
    // Measure the distance and angle of the car to the wall, and wait for 2 sec.
    delay(2000);
    PARKING_STATE = TRANSFER;
    break;
  case TRANSFER:
    // Transfer to the parking location.
    delay(2000);
    PARKING_STATE = PARKING;
    break;

  case PARKING:
    // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
    delay(2000);
    PARKING_STATE = IDLE;
    break;
  }

  // run the code in every 20ms

  if (millis() > (time + 15))
  {
    sensor.update();
    mpu.update();
    time = millis();

    SR_Control();
    // BT_Control(); // get BT serial data
    sendSensor();
  }
}