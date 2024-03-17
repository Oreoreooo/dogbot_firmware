#include "lib/Communication.hpp"
#include "lib/Display.hpp"
#include "lib/MotorController.hpp"
#include "lib/Sensor.hpp"
#include "lib/State.h"
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

ControlState state = IDLE;
MotorController controller;
MPU6050 mpu(Wire);
Sensor sensor;

bool parking_start = false;
unsigned long wait_time = 0;
unsigned long recv_time = 0;
unsigned long send_time = 0;
char sr_data = '\0';
char wr_data = '\0';

void setup()
{
  setupCommunication();
  setupDisplay();

  sensor.begin();
  controller.begin();
  mpu.begin();
  mpu.setFilterGyroCoef(0.965);

  displayShow("Calibrating...");
  delay(2000);
  mpu.calcGyroOffsets();
}

inline bool turn(int angle)
{
}

inline bool move(int target_distance)
{
}

inline bool park()
{
}

inline void parkingStateMachine()
{
  switch (state)
  {
  case IDLE: // After start, wait for 2 sec
    if (parking_start)
    {
      controller.STOP();
      delay(2000);
      state = MOVE_TO_25;
    }
    break;

  case MOVE_TO_25: // Move to a location of 25cm from the wall, and wait for 2 sec.
    if (move(25))
    {
      controller.STOP();
      delay(2000);
      state = TURN_CW_90;
    }

    break;

  case TURN_CW_90: // Turn CW 90° (-90°), wait 2 sec .
    if (turn(-90))
    {
      controller.STOP();
      delay(2000);
      state = TURN_CCW_270;
    }
    break;

  case TURN_CCW_270: // Turn CCW 270°(90°), wait 2 sec.
    if (turn(270))
    {
      controller.STOP();
      delay(2000);
      state = TURN_CW_180;
    }
    break;

  case TURN_CW_180: // Turn CW 180°(-180°), wait 2 sec.
    if (turn(-180))
    {
      controller.STOP();
      delay(2000);
      state = MEASURE;
      wait_time = millis();
    }
    break;

  case MEASURE: // Measure the distance and angle of the car to the wall, and wait for 2 sec.
    if (millis() > wait_time + 2000)
    {
      state = PARK;
    }
    break;

  case PARK: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
    if (park())
    {
      controller.STOP();
      delay(2000);
      state = IDLE;
    }
    break;
  }
}

void loop()
{
  sensor.update();
  mpu.update();

  if (millis() > (send_time + 150))
  {
    send_time = millis();
    // serialSensorDataTX();
    // wirelessSensorDataTX();
    displaySensorData();
  }

  if (millis() > (recv_time + 15))
  {
    recv_time = millis();
    // sr_data = serialControlMotor();
    wr_data = wirelessControlMotor();
    parking_start = (sr_data == 'S' || wr_data == 'S');
  }

  parkingStateMachine();
  controller.balance();
}