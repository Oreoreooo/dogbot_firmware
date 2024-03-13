#include "lib/CommunicationSerial.hpp"
#include "lib/Display_SSD1306.hpp"
#include "lib/Motor.hpp"
#include "lib/Sensor.hpp"
#include "lib/State.h"
#include <Arduino.h>

ControlState CONTROL_STATE = IDLE;
Display_SSD1306 display;
Sensor sensor;
Motor motor;

Communication comm(display, motor, sensor);

void setup()
{
  motor.STOP();
  display.begin();
  display.show("Calibrating...");
  sensor.begin();
  display.show("Alt-F4!");
  comm.begin();
}

void loop()
{
  static unsigned long rx_time;
  static unsigned long tx_time;
  static char sr_data;
  static char wr_data;

  sensor.update();

  if (millis() > (tx_time + 1000))
  {
    tx_time = millis();
    comm.serialSensorDataTX();
  }

  if (millis() > (rx_time + 15))
  {
    rx_time = millis();
    sr_data = comm.serialControlRX();
    wr_data = comm.wirelessControlRX();
  }

  switch (CONTROL_STATE)
  {
  case IDLE: // After start, wait for 2 sec
    if (sr_data == 'S' || wr_data == 'S')
    {
      delay(2000);
      CONTROL_STATE = MOVE;
    }
    break;

  case MOVE: // Move to a location of 25cm from the wall, and wait for 2 sec.
    delay(2000);
    CONTROL_STATE = TURN;
    break;

  case TURN: // Turn CW 90°, wait 2 sec → CCW 270°, wait 2 sec → CW 180°, wait 2 sec.
    delay(2000);
    delay(2000);
    delay(2000);
    CONTROL_STATE = MEASURE;
    break;

  case MEASURE: // Measure the distance and angle of the car to the wall, and wait for 2 sec.
    static char data[16];
    sprintf(data, "D: %.3f\nA: %.3f", sensor.getDepth() * 0.0001, sensor.getAngleZ());
    display.show(data);
    delay(2000);
    CONTROL_STATE = TRANSFER;
    break;

  case TRANSFER: // Transfer to the parking location.
    delay(2000);
    CONTROL_STATE = PARKING;
    break;

  case PARKING: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
    delay(2000);
    CONTROL_STATE = STOP;
    break;

  default:
    break;
  }
}