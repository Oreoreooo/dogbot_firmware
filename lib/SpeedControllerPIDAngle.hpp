#ifndef __SPEED_CONTROLLER_PID_ANGLE_HPP__
#define __SPEED_CONTROLLER_PID_ANGLE_HPP__

#include <Arduino.h>
#include "Sensor.hpp"
#include "SpeedController.hpp"

const float kP = 1.0;
const float kI = 0.0;
const float kD = 0.0;

class SpeedControllerPIDAngle : public SpeedController
{
public:
    SpeedControllerPIDAngle(Sensor *sensor);
    virtual void begin() override;
    virtual void reset() override;
    inline virtual void perform() override;
    virtual void setPWM(int MOTOR_PWM) override;

private:
    Sensor *_sensor;
    unsigned long _prev_time;
    unsigned long _time_step;
    float _input;
    float _input_prev;
    float _error;
    float _integral;
    float _derivative;
    float _output;
    float _set_point;
};

SpeedControllerPIDAngle::SpeedControllerPIDAngle(Sensor *sensor) : SpeedController(), _sensor(sensor)
{
    _time_step = 10;
}

void SpeedControllerPIDAngle::begin()
{
    SpeedController::begin();
    _prev_time = millis();
}

void SpeedControllerPIDAngle::reset()
{
    SpeedController::reset();
    _integral = 0;
    _prev_time = millis();
}

inline void SpeedControllerPIDAngle::perform()
{
    if (millis() - _prev_time >= _time_step && _control_on)
    {
        _prev_time = millis();
        _input = _sensor->getAngleZ();
        _error = _set_point - _input;
        _integral += _error * kI;
        _derivative = _input - _input_prev;
        _output = kP * _error + _integral + kD * _derivative;
        _input_prev = _input;

        if (_error > 0)
        {
            _PWM_A -= _output / MOTOR_BALANCE_FACTOR_A;
            _PWM_B += _output / MOTOR_BALANCE_FACTOR_B;
            _PWM_C -= _output / MOTOR_BALANCE_FACTOR_C;
            _PWM_D += _output / MOTOR_BALANCE_FACTOR_D;
        }
        else if (_error < 0)
        {
            _PWM_A += _output / MOTOR_BALANCE_FACTOR_A;
            _PWM_B -= _output / MOTOR_BALANCE_FACTOR_B;
            _PWM_C += _output / MOTOR_BALANCE_FACTOR_C;
            _PWM_D -= _output / MOTOR_BALANCE_FACTOR_D;
        }
        _writePWM();
    }
}

void SpeedControllerPIDAngle::setPWM(int MOTOR_PWM)
{
    SpeedController::setPWM(MOTOR_PWM);
    _set_point = _sensor->getAngleZ();
}

#endif