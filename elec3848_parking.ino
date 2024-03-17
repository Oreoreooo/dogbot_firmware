#include "lib/CommunicationSerial.hpp"
#include "lib/Display_SSD1306.hpp"
#include "lib/MotorController.hpp"
#include "lib/Sensor.hpp"
#include "lib/State.h"
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

ControlState state = IDLE;
CommunicationSerial communication;
Display_SSD1306 display;
MotorController controller;
MPU6050 mpu(Wire);
Sensor sensor;

bool parking_start;
unsigned long recv_time;
unsigned long send_time;
char sr_data;
char wr_data;

void setup()
{
  communication.begin();
  controller.begin();
  controller.STOP();
  display.begin();
  sensor.begin();

  display.show("Calibrating...");
  unsigned long start_time = millis();
  mpu.begin();
  mpu.setFilterGyroCoef(0.965);
  while (millis() - start_time < 1000)
  {
    sensor.update();
  }
  mpu.calcGyroOffsets();
  // controller.measure();
}

inline bool turn(int angle)
{
}

inline bool move(int target_distance)
{
}

inline void parkingStateMachine()
{
  switch (state)
  {
  case IDLE: // After start, wait for 2 sec
    if (parking_start)
    {
      delay(2000);
      state = MOVE;
    }
    break;

  case MOVE: // Move to a location of 25cm from the wall, and wait for 2 sec.
    if (move(25))
    {
      state = TURN_CW_90;
    }

    break;

  case TURN_CW_90: // Turn CW 90° (-90°), wait 2 sec .
    if (turn(-90))
    {
      state = TURN_CCW_270;
    }
    break;

  case TURN_CCW_270: // Turn CCW 270°(90°), wait 2 sec.
    if (turn(270))
    {
      state = TURN_CW_180;
    }
    break;

  case TURN_CW_180: // Turn CW 180°(-180°), wait 2 sec.
    if (turn(-180))
    {
      state = MEASURE;
    }
    break;

  case MEASURE: // Measure the distance and angle of the car to the wall, and wait for 2 sec.
    display.displayMeasured(&sensor, &mpu);
    delay(2000);
    state = PARK;
    break;

  case PARK: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
    delay(2000);
    state = STOP;
    break;

  default:
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
    // comm.serialSensorDataTX(&sensor, &mpu);
    // communication.wirelessSensorDataTX(&sensor, &mpu);
    display.displaySensorData(&sensor, &mpu);
  }

  if (millis() > (recv_time + 15))
  {
    recv_time = millis();
    // sr_data = comm.serialControlMotor(&controller);
    wr_data = communication.wirelessControlMotor(&controller);
    parking_start = (sr_data == 'S' || wr_data == 'S');
  }

  parkingStateMachine();
  controller.balance();
}