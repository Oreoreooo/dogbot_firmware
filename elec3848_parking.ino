#include "lib/Communication.hpp"
#include "lib/Display.hpp"
#include "lib/MotorController.hpp"
#include "lib/Sensor.hpp"
#include "lib/ParkingStateMachine.hpp"
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

MotorController motor;
Sensor sensor;
MPU6050 mpu(Wire);

bool _debug_flag = true;

unsigned long recv_time = 0;
unsigned long send_time = 0;

void setup()
{
  // handle the b***s***t freeze problem which cost me 3 days to understand in no rest:  https://forum.arduino.cc/t/mpu-6050-freezing/270013/12
  Wire.begin();
  Wire.setClock(400000);
  Wire.setWireTimeout(5000, true);

  setupCommunication();
  setupDisplay();
  textDisplay("Calibrating...");

  sensor.begin();
  mpu.begin(1, 1);
  mpu.setFilterGyroCoef(0.5);
  mpu.calcOffsets();
  for (int i = 0; i < 500; i++)
  {
    mpu.update();
    sensor.update();
    delay(1);
  }
  motor.begin();
}

void loop()
{
  sensor.update();
  mpu.update();

  if (millis() > (send_time + 50))
  {
    send_time = millis();
    // serialSensorDataTX();
    // wirelessSensorDataTX();
    sensorDataDisplay();
  }

  if (millis() > (recv_time + 500))
  {
    recv_time = millis();
    // serialControlMotor();
    wirelessControlMotor();
  }

  if (_debug_flag)
  {
    _debug_flag = false;
    state = MOVE_TO_25;
  }

  parkingStateMachine(state);
  motor.balance();
}