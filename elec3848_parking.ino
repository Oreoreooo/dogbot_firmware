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

Communication communication(display, motor, sensor);

void setup()
{
  motor.STOP();
  display.begin();
  display.show("Calibrating...");
  sensor.begin();
  display.show("Alt-F4!");
  communication.begin();
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
    communication.serialSensorDataTX();
  }

  if (millis() > (rx_time + 15))
  {
    rx_time = millis();
    sr_data = communication.serialControlRX();
    wr_data = communication.wirelessControlRX();
  }

  switch (CONTROL_STATE)
  {
  case IDLE:
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

    delay(2000);
    CONTROL_STATE = TRANSFER;
    break;
  case TRANSFER: // Transfer to the parking location.

    delay(2000);
    CONTROL_STATE = PARKING;
    break;

  case PARKING: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.

    delay(2000);
    CONTROL_STATE = IDLE;
    break;
  }
}