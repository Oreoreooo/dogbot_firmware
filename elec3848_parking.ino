#include "lib/Communication.hpp"
#include "lib/Display.hpp"
#include "lib/MotorController.hpp"
#include "lib/Sensor.hpp"
#include "lib/State.h"
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

#define RUNNING_PWM 100
#define DISTANCE_THRESHOLD 1
#define STATIC_THRESHOLD 500

ControlState state = IDLE;
MotorController controller;
MPU6050 mpu(Wire);
Sensor sensor;

bool parking_start = false;
static bool _debug_flag = true;
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

inline bool turn(float angle)
{
  static float target_orientation = mpu.getAngleZ() + angle;
  static float initial_orientation = mpu.getAngleZ();

  if (target_orientation)

    controller.ROTATE_BY(RUNNING_PWM, angle);

  // decide when to stop
  // get current orientation -> compute target orientation -> compare instant orientation with target orientation

  // return when finished turning process
}

inline bool move_to(int target_distance)
{
  static unsigned long static_count = 0;

  // By approaching the final destination, the pwm value should keep decreasing correspondingly

  if (static_count > STATIC_THRESHOLD)
  {
    static_count = 0;
    return true;
  }
  else if (target_distance - sensor.getDistance() > DISTANCE_THRESHOLD)
  {
    controller.ADVANCE(RUNNING_PWM);
    static_count = 0;
    return false;
  }
  else if (target_distance - sensor.getDistance() < -DISTANCE_THRESHOLD)
  {
    controller.BACK(RUNNING_PWM);
    static_count = 0;
    return false;
  }
  else
  {
    controller.STOP();
    static_count++;
    return false;
  }
}

inline bool park()
{
  // first adjust the car perpendicular to the wall
  while (mpu.getAngleZ() != 0)
  {
    // when angle < 0, turn left
    if (mpu.getAngleZ() > 0)
    {
      controller.ROTATE_CW(RUNNING_PWM);
    }
    // when angle < 0, turn left
    if (mpu.getAngleZ() < 0)
    {
      controller.ROTATE_CCW(RUNNING_PWM);
    }
  }

  // center the LED
  while (sensor.getLightLeft() != sensor.getLightRight())
  {
    if (sensor.getLightLeft() > sensor.getLightRight())
    { // left brigter->move left
      controller.LEFT(RUNNING_PWM);
    }
    if (sensor.getLightLeft() < sensor.getLightRight())
    { // right brigter->move right
      controller.RIGHT(RUNNING_PWM);
    }
  }

  // move to 5cm position to the wall
  while (sensor.getDistance() > 5)
  {
    controller.ADVANCE(RUNNING_PWM);
  }
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
    if (move_to(25))
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
  
  if (millis() > (send_time + 500))
  {
    send_time = millis();
    // serialSensorDataTX();
    // wirelessSensorDataTX();
    displaySensorData();
  }

  if (millis() > (recv_time + 500))
  {
    recv_time = millis();
    sr_data = serialControlMotor();
    // wr_data = wirelessControlMotor();
    parking_start = (sr_data == 'S' || wr_data == 'S');
  }

  // parkingStateMachine();
  if (_debug_flag)
  {
    _debug_flag = false;
    controller.ROTATE_TO(RUNNING_PWM, -90);
  }

  controller.balance();
}