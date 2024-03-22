#include "Communication.hpp"
#include "Display.hpp"
#include "MotorController.hpp"
#include "ParkingStateMachine.hpp"
#include "Sensor.hpp"
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

MotorController motor;
Sensor sensor;
MPU6050 mpu(Wire);

bool _start_flag = true;

unsigned long serial_time = 0;

void setup()
{
  // https://forum.arduino.cc/t/mpu-6050-freezing/270013/12
  Wire.begin();
  Wire.setClock(400000);
  Wire.setWireTimeout(5000, true);

  setupCommunication();
  setupDisplay();
  textDisplay("Starting...v2!");

  sensor.begin();
  mpu.begin();
  mpu.calcOffsets();
  for (int i = 0; i < 50; i++)
  {
    mpu.update();
    sensor.update();
    sensorDataDisplay();
  }
  mpu.calcOffsets();
  motor.begin();
}

void loop()
{
  sensor.update();
  mpu.update();

  if (millis() > (serial_time + 50))
  {
    serial_time = millis();
    // serialSensorDataTX();
    // wirelessSensorDataTX();
    // wirelessControlMotor();
    // serialControlMotor();
    sensorDataDisplay();
  }

  if (_start_flag)
  {
    _start_flag = false;

    state = MOVE_TO_25;
  }

  parkingStateMachine();
  motor.balance();
}