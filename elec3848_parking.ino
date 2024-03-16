#include "lib/CommunicationSerial.hpp"
#include "lib/Display_SSD1306.hpp"
#include "lib/MotorController.hpp"
#include "lib/Sensor.hpp"
#include "lib/State.h"
#include <Arduino.h>

ControlState state = IDLE;
CommunicationSerial comm;
Display_SSD1306 display;
MotorController controller;
Sensor sensor;

bool start_flag;

void setup()
{
  controller.begin();
  controller.STOP();
  display.begin();
  display.show("Calibrating...");
  sensor.begin();
  display.show("Alt-F4!");
  comm.begin();
  controller.ADVANCE(128);
  // controller.measure();
}

void parkingStateMachine()
{
  switch (state)
  {
  case IDLE: // After start, wait for 2 sec
    if (start_flag)
    {
      delay(2000);
      state = MOVE;
    }
    break;

  case MOVE: // Move to a location of 25cm from the wall, and wait for 2 sec.
    delay(2000);
    state = TURN;
    break;

  case TURN: // Turn CW 90°, wait 2 sec → CCW 270°, wait 2 sec → CW 180°, wait 2 sec.
    delay(2000);
    delay(2000);
    delay(2000);
    state = MEASURE;
    break;

  case MEASURE: // Measure the distance and angle of the car to the wall, and wait for 2 sec.
    display.displayMeasured(&sensor);
    delay(2000);
    state = TRANSFER;
    break;

  case TRANSFER: // Transfer to the parking location.
    delay(2000);
    state = PARKING;
    break;

  case PARKING: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
    delay(2000);
    state = STOP;
    break;

  default:
    break;
  }
}

void loop()
{
  static unsigned long recv_time;
  static unsigned long send_time;
  static unsigned long pid_time;
  static char sr_data;
  static char wr_data;

  sensor.update();

  if (millis() > (send_time + 150))
  {
    send_time = millis();
    // comm.serialSensorDataTX(&sensor);
    // comm.wirelessSensorDataTX(&sensor);
    display.displaySensorData(&sensor);
  }

  if (millis() > (recv_time + 15))
  {
    recv_time = millis();
    // sr_data = comm.serialControlMotor(&controller);
    // wr_data = comm.wirelessControlMotor(&controller);
    start_flag = (sr_data == 'S' || wr_data == 'S');
  }

  controller.perform();

  // parkingStateMachine();

}