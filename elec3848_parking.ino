#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <MPU6050_light.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>

#include "motor.h"
#include "sensor.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 28  // 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SERIAL Serial
// BT Module on Serial 3 (D14 & D15)
#define BTSERIAL Serial3

#define LOG_DEBUG
#ifdef LOG_DEBUG
#define M_LOG SERIAL.print
#else
#define M_LOG BTSERIAL.println
#endif

// PWM Definition
#define MAX_PWM 2000
#define MIN_PWM 300
int Motor_PWM = 1900;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int window_size = 0;
int BT_alive_cnt = 0;
unsigned long time;

MPU6050 mpu(Wire);
Sensor sensor = Sensor();
Motor motor = Motor();

enum PARKING_STATE {
  IDLE,
  START,
  MOVE_25CM,
  TURN_ANGLE,
  MEASURE,
  TRANSFER,
  PARKING
} PARKING_STATE;

void USB_Control() {
  String tempString;
  // USB data
  /*
   * Check if USB Serial data contain brackets
   */

  if (SERIAL.available()) {
    char inputChar = SERIAL.read();
    if (inputChar == '(') {  // Start loop when left bracket detected
      tempString = "";
      inputChar = SERIAL.read();
      while (inputChar != ')') {
        tempString = tempString + inputChar;
        inputChar = SERIAL.read();
        if (!SERIAL.available()) {
          break;
        }  // Break when bracket closed
      }
    }

    moveControl(tempString[0]);

    if (tempString != "") {
      display.clearDisplay();
      display.setCursor(0, 0);  // Start at top-left corner
      display.println("Serial_Data = ");
      display.println(tempString);
      display.display();
    }
  }
}

char * BT_Control_Display(char command) {
  switch (command) {
    case 'A':
      return "Forward"
    case 'B':
      return "Forward Right"
    case 'C':
      return "Rotate Right"
    case 'D':
      return "Backward Right"
    case 'E':
      return "Backward"
    case 'F':
      return "Backward Left"
    case 'G':
      return "Rotate Left"
    case 'H':
      return "Forward Left"
    case 'Z':
      return "Stop"
    case 'z':
      return "Stop"
    case 'd':
      return "Left"
    case 'b':
      return "Right"
    case 'L':
      return "1500"
    case 'M':
      return "500"
  }
}

void BT_Control() {
  char BT_Data = 0;

  /*
    Receive data from app and translate it to motor movements
  */
  if (BTSERIAL.available()) {
    BT_Data = BTSERIAL.read();
    SERIAL.print(BT_Data);
    BTSERIAL.flush();
    BT_alive_cnt = 100;
    display.clearDisplay();
    display.setCursor(0, 0);  // Start at top-left corner
    display.println("BT_Data = ");

    display.println(BT_Control_Display(BT_Data));
    display.display();
  }

  if (BT_alive_cnt <= 0) {
    motor.STOP();
  } else {
    BT_alive_cnt = BT_alive_cnt - 1;
    moveControl(BT_Data);
  }
}

void moveControl(char command) {
  switch (command) {
    case 'A':
      motor.ADVANCE(Motor_PWM);
      M_LOG("Run!\r\n");
      break;
    case 'B':
      motor.ADVANCE_RIGHT(Motor_PWM);
      M_LOG("Right up!\r\n");
      break;
    case 'C':
      motor.ROTATE_CW(Motor_PWM);
      break;
    case 'D':
      motor.BACK_RIGHT(Motor_PWM);
      M_LOG("Right down!\r\n");
      break;
    case 'E':
      motor.BACK(Motor_PWM);
      M_LOG("Back!\r\n");
      break;
    case 'F':
      motor.BACK_LEFT(Motor_PWM);
      M_LOG("Left down!\r\n");
      break;
    case 'G':
      motor.ROTATE_CCW(Motor_PWM);
      break;
    case 'H':
      motor.ADVANCE_LEFT(Motor_PWM);
      M_LOG("Left up!\r\n");
      break;
    case 'Z':
      motor.STOP();
      M_LOG("Stop!\r\n");
      break;
    case 'z':
      motor.STOP();
      M_LOG("Stop!\r\n");
      break;
    case 'd':
      motor.LEFT(Motor_PWM);
      M_LOG("Left!\r\n");
      break;
    case 'b':
      motor.RIGHT(Motor_PWM);
      M_LOG("Right!\r\n");
      break;
    case 'L':
      Motor_PWM = 1500;
      break;
    case 'M':
      Motor_PWM = 500;
      break;
  }
}

void printSensorData() {
  SERIAL.print("LL: ");
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
  SERIAL.print(motor.getTurnsA());
  SERIAL.print(" EB: ");
  SERIAL.print(motor.getTurnsB());
  SERIAL.print(" EC: ");
  SERIAL.print(motor.getTurnsC());
  SERIAL.print(" ED: ");
  SERIAL.print(motor.getTurnsD());

  SERIAL.println("");
}

void setup() {
  SERIAL.begin(115200);  // USB serial setup
  SERIAL.println("Starting...");
  motor.STOP();          // Stop the robot
  BTSERIAL.begin(9600);  // BT serial setup

  // OLED Setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.cp437(true);                  // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 0);              // Start at top-left corner
  display.println("Calibrating...");
  display.display();

  Wire.begin();
  mpu.begin();
  mpu.calcGyroOffsets();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("AI Robot");
  display.display();
}

void loop() {
  // switch (PARKING_STATE) {
  //   case IDLE:
  //     break;
  //   case START:
  //     // Wait for 2 sec
  //     delay(2000);
  //     PARKING_STATE = MOVE_25CM;
  //     break;
  //   case MOVE_25CM:
  //     // Move to a location of 25cm from the wall, and wait for 2 sec.
  //     delay(2000);
  //     PARKING_STATE = TURN_ANGLE;
  //     break;
  //   case TURN_ANGLE:
  //     // Turn **CW 90°, wait 2 sec → CCW 270°, wait 2 sec → CW 180°, wait 2 sec**.
  //     delay(2000);
  //     delay(2000);
  //     delay(2000);
  //     PARKING_STATE = MEASURE;
  //     break;

  //   case MEASURE:
  //     // Measure the distance and angle of the car to the wall, and wait for 2 sec.
  //     delay(2000);
  //     PARKING_STATE = TRANSFER;
  //     break;
  //   case TRANSFER:
  //     // Transfer to the parking location.
  //     delay(2000);
  //     PARKING_STATE = PARKING;
  //     break;

  //   case PARKING:
  //     // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
  //     delay(2000);
  //     PARKING_STATE = IDLE;
  //     break;
  // }

  // run the code in every 20ms
  if (millis() > (time + 15)) {
    sensor.update();
    mpu.update();
    motor.update();

    time = millis();
    USB_Control();
    BT_Control();  // get USB and BT serial data
    printSensorData();
  }
}