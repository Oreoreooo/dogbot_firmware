#ifndef __SENSOR_H__
#define __SENSOR_H__
#include <Arduino.h>

#define PTR_L A0 // Phtotransistor Left PIN
#define PTR_R A2 // Phtotransistor Right PIN
#define LSR A3   // LaserPING PIN

class Sensor
{
public:
  Sensor(void);

  void update();

  int getLightL()
  {
    return this->lightL;
  }
  int getLightR()
  {
    return this->lightR;
  }
  int getSonarL()
  {
    return this->sonarL;
  }
  int getSonarR()
  {
    return this->sonarR;
  }
  long getDistance()
  {
    return this->distance;
  }

private:
  long distance = 0;
  int lightL = 0;
  int lightR = 0;
  int sonarL = 0;
  int sonarR = 0;
};

#endif