#include "lib/Communication.hpp"
#include "lib/Display.hpp"
#include "lib/Motor.hpp"
#include "lib/Sensor.hpp"
#include "lib/State.h"
#include <Arduino.h>
// #include <MPU6050_light.h>
#include <Wire.h>

#define _PWM_ 100

#define SCALE_FACTOR 0.5
#define DISTANCE_THRESHOLD 3.0
#define SLOW_DOWN_DISTANCE 10.0
#define TARGET_DISTANCE 27.5
#define STATIC_COUNTER 200

ControlState state = IDLE;
MPU6050 mpu(Wire);

unsigned long move_to_target_counter = 0;
bool parking_start = false;
bool _debug_flag = true;
unsigned long recv_time = 0;
unsigned long send_time = 0;

void setup()
{
  setupCommunication();
  setupDisplay();
  displayShow("Calibrating...");
  setupSensor();
  mpu.begin();
  mpu.setFilterGyroCoef(0.965);
  mpu.calcGyroOffsets();
  mpu.update();
  setupMotor();
}

inline bool move_to_target()
{
  if (move_to_target_counter > STATIC_COUNTER)
  {
    move_to_target_counter = 0;
    return true;
  }
  float delta = getDistance() - TARGET_DISTANCE; // -> 0 cm

  if (delta > DISTANCE_THRESHOLD)
  {
    if (delta < SLOW_DOWN_DISTANCE)
    {
      ADVANCE(SCALE_FACTOR * _PWM_ * delta / SLOW_DOWN_DISTANCE);
    }
    else
    {
      ADVANCE(SCALE_FACTOR * _PWM_);
    }
    move_to_target_counter = 0;
    return false;
  }

  else if (delta < -DISTANCE_THRESHOLD)
  {
    if (abs(delta) < SLOW_DOWN_DISTANCE)
    {
      BACK(0.5 * _PWM_ * delta / SLOW_DOWN_DISTANCE);
    }
    else
    {
      BACK(0.5 * _PWM_);
    }
    move_to_target_counter = 0;
    return false;
  }
  else
  {
    STOP();
    move_to_target_counter++;
    return false;
  }
}

inline bool park()
{
  while (mpu.getAngleZ() != 0)
  {
    if (mpu.getAngleZ() > 0)
    {
      ROTATE_CW(_PWM_);
    }
    if (mpu.getAngleZ() < 0)
    {
      ROTATE_CCW(_PWM_);
    }
  }

  while (_light_L != _light_R)
  {
    if (_light_L > _light_R)
    {
      LEFT(_PWM_);
    }
    if (_light_L < _light_R)
    {
      RIGHT(_PWM_);
    }
  }

  while (getDistance() > 5)
  {
    ADVANCE(_PWM_);
  }
}

void loop()
{
  updateSensor();
  mpu.update();

  if (millis() > (send_time + 50))
  {
    send_time = millis();
    // serialSensorDataTX();
    // wirelessSensorDataTX();
    // displaySensorData();
    tmpDisplay(state);
  }

  // if (millis() > (recv_time + 500))
  // {
  //   recv_time = millis();
  //   sr_data = serialControlMotor();
  //   // wr_data = wirelessControlMotor();
  //   parking_start = (sr_data == 'S' || wr_data == 'S');
  // }

  if (_debug_flag)
  {
    _debug_flag = false;
    state = MOVE_TO_25;
  }

  switch (state)
  {
  case IDLE: // After start, wait for 2 sec
    if (parking_start)
    {
      STOP();
      delay(2000);
      state = MOVE_TO_25;
    }
    break;

  case MOVE_TO_25: // Move to a location of 25cm from the wall, and wait for 2 sec.
    if (true)
    {
      STOP();
      delay(2000);
      ROTATE_TO(_PWM_, -90);
      state = TURN_CW_90;
    }
    break;

  case TURN_CW_90: // Turn CW 90° (-90°), wait 2 sec .
    if (_is_stopped)
    {
      STOP();
      delay(2000);
      ROTATE_TO(_PWM_, 180);
      state = TURN_CCW_270;
    }
    break;

  case TURN_CCW_270: // Turn CCW 270°(90°), wait 2 sec.
    if (_is_stopped)
    {
      delay(2000);
      ROTATE_TO(_PWM_, 0);
      state = TURN_CW_180;
    }
    break;

  case TURN_CW_180: // Turn CW 180°(-180°), wait 2 sec.
    if (_is_stopped)
    {
      delay(2000);
      state = MEASURE;
    }
    break;

  case MEASURE: // Measure the distance and angle of the car to the wall, and wait for 2 sec.
    delay(2000);
    state = PARK;
    break;

  case PARK: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
    if (true)
    {
      delay(2000);
      state = IDLE;
    }
    break;
  default:
    break;
  }
  balance();
}