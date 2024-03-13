#ifndef __SENSOR_H__
#define __SENSOR_H__
#include <Arduino.h>

class Sensor
{
public:
  Sensor(void);

  void update();

  int getLightL()
  {
    return this->_light_l;
  }
  int getLightR()
  {
    return this->_light_r;
  }
  int getSonarL()
  {
    return this->_sonar_l;
  }
  int getSonarR()
  {
    return this->_sonar_r;
  }
  long getDistance()
  {
    return this->_length;
  }

private:
  long _length = 0;
  int _light_l = 0;
  int _light_r = 0;
  int _sonar_l = 0;
  int _sonar_r = 0;
};

#endif