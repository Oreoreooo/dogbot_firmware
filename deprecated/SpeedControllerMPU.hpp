#ifndef __PID_ANGLE_HPP__
#define __PID_ANGLE_HPP__

#include "SpeedController.hpp"
#include <Arduino.h>

const float kP = 1.0;
const float kI = 0.0;
const float threshold = 2.0;
const float threshold_min = -2.0;

class SpeedControllerMPU : public SpeedController {
public:
    SpeedControllerMPU();
    virtual void begin() override;
    virtual void reset() override;
    inline virtual void perform() override;
    virtual void setPWM(int MOTOR_PWM) override;
    virtual void set_target(int target) override;

private:
    float _integral;
    float _set_point;

    unsigned long _prev_time;
    unsigned long _time_step;

    int _PWM_A_base;
    int _PWM_B_base;
    int _PWM_C_base;
    int _PWM_D_base;
};

SpeedControllerMPU::SpeedControllerMPU() : SpeedController() {
    _time_step = 10;
}

void SpeedControllerMPU::begin() {
    SpeedController::begin();
    _prev_time = millis();
}

void SpeedControllerMPU::reset() {
    SpeedController::reset();
    _integral = 0;
    _prev_time = millis();
}

inline void SpeedControllerMPU::perform() {
    if (millis() - _prev_time >= _time_step && _control_on)
    {
        _prev_time = millis();
        float _input = mpu.getAngleZ();
        float _error = _set_point - _input;
        _integral += _error * kI;
        float _output = kP * _error + _integral;

        if (_error > threshold) // car is drifting left, make right motor slower
        {
            _setPWM(_PWM_A_base, _PWM_B_base - round(_output), _PWM_C_base, _PWM_D_base - round(_output));
        }
        else if (_error < -threshold_min) // car is drifting right, make left motor slower
        {
            _setPWM(_PWM_A_base + round(_output), _PWM_B_base, _PWM_C_base + round(_output), _PWM_D_base);
        }
        else
        {
            _setPWM(_PWM_A_base, _PWM_B_base, _PWM_C_base, _PWM_D_base);
        }
    }
}

void SpeedControllerMPU::setPWM(int MOTOR_PWM) {
    SpeedController::setPWM(MOTOR_PWM);
}

void SpeedControllerMPU::set_target(int target) {
    _set_point = target;
}

#endif