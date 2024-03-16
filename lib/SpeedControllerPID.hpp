#ifndef __SPEED_CONTROLLER_PID_HPP__
#define __SPEED_CONTROLLER_PID_HPP__

#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Arduino.h>
#include "SpeedController.hpp"
#include <Encoder.h>

#define ECDAA 18 // Motor A Encoder PIN A
#define ECDAB 31 // Motor A Encoder PIN B
#define ECDBA 19 // Motor B Encoder PIN A
#define ECDBB 38 // Motor B Encoder PIN B
#define ECDCA 3  // Motor C Encoder PIN A
#define ECDCB 49 // Motor C Encoder PIN B
#define ECDDA 2  // Motor D Encoder PIN A
#define ECDDB A1 // Motor D Encoder PIN B

const float kP = 5.0;
const float kI = 0.0;
const float kD = 0.0;

struct MotorPID
{
    int32_t ECD_val;
    int32_t ECD_val_prev;
    int32_t ECD_val_diff;
    float integral;
    float error;
    float prev_input;
    float derivative;
};

class SpeedControllerPID : public SpeedController
{
public:
    SpeedControllerPID(void);
    virtual void begin() override;
    virtual void reset() override;
    inline virtual void perform() override;
    inline virtual void setPWM(int MOTOR_PWM, bool rectify) override;

    virtual void measure() override;

private:
    void _read();

    unsigned long _dT;
    unsigned long _prev_time;
    unsigned long _time_step;

    struct MotorPID _MTRA = {0, 0, 0, 0.0, 0.0, 0.0, 0.0};
    struct MotorPID _MTRB = {0, 0, 0, 0.0, 0.0, 0.0, 0.0};
    struct MotorPID _MTRC = {0, 0, 0, 0.0, 0.0, 0.0, 0.0};
    struct MotorPID _MTRD = {0, 0, 0, 0.0, 0.0, 0.0, 0.0};

    Encoder _ECDA;
    Encoder _ECDB;
    Encoder _ECDC;
    Encoder _ECDD;

    float _set_point;
};

SpeedControllerPID::SpeedControllerPID(void) : SpeedController(), _ECDA(Encoder(ECDAA, ECDAB)), _ECDB(Encoder(ECDBA, ECDBB)), _ECDC(Encoder(ECDCA, ECDCB)), _ECDD(Encoder(ECDDA, ECDDB))
{
    _time_step = 10;
}

void SpeedControllerPID::begin()
{
    SpeedController::begin();
    pinMode(ECDAA, INPUT);
    pinMode(ECDAB, INPUT);
    pinMode(ECDBA, INPUT);
    pinMode(ECDBB, INPUT);
    pinMode(ECDCA, INPUT);
    pinMode(ECDCB, INPUT);
    pinMode(ECDDA, INPUT);
    pinMode(ECDDB, INPUT);
    _prev_time = millis();
}

void SpeedControllerPID::reset()
{
    SpeedController::reset();
    _MTRA.integral = 0;
    _MTRB.integral = 0;
    _MTRC.integral = 0;
    _MTRD.integral = 0;
    _prev_time = millis();
}

inline void SpeedControllerPID::perform()
{
    _dT = millis() - _prev_time;
    if (_dT >= _time_step)
    {
        _prev_time = millis();
        _read();

        _MTRA.error = _set_point - (float)_MTRA.ECD_val_diff / (float)_dT * TIME_INTERVAL;
        _MTRB.error = _set_point - (float)_MTRB.ECD_val_diff / (float)_dT * TIME_INTERVAL;
        _MTRC.error = _set_point - (float)_MTRC.ECD_val_diff / (float)_dT * TIME_INTERVAL;
        _MTRD.error = _set_point - (float)_MTRD.ECD_val_diff / (float)_dT * TIME_INTERVAL;

        _MTRA.integral = constrain(_MTRA.integral + kI * _MTRA.error, MIN_PWM_f, MAX_PWM_f);
        _MTRB.integral = constrain(_MTRB.integral + kI * _MTRB.error, MIN_PWM_f, MAX_PWM_f);
        _MTRC.integral = constrain(_MTRC.integral + kI * _MTRC.error, MIN_PWM_f, MAX_PWM_f);
        _MTRD.integral = constrain(_MTRD.integral + kI * _MTRD.error, MIN_PWM_f, MAX_PWM_f);

        _MTRA.derivative = _MTRA.ECD_val_diff - _MTRA.prev_input;
        _MTRB.derivative = _MTRB.ECD_val_diff - _MTRB.prev_input;
        _MTRC.derivative = _MTRC.ECD_val_diff - _MTRC.prev_input;
        _MTRD.derivative = _MTRD.ECD_val_diff - _MTRD.prev_input;

        _PWM_A = constrain(kP * _MTRA.error + _MTRA.integral + kD * _MTRA.derivative, MIN_PWM, MAX_PWM);
        _PWM_B = constrain(kP * _MTRB.error + _MTRB.integral + kD * _MTRB.derivative, MIN_PWM, MAX_PWM);
        _PWM_C = constrain(kP * _MTRC.error + _MTRC.integral + kD * _MTRC.derivative, MIN_PWM, MAX_PWM);
        _PWM_D = constrain(kP * _MTRD.error + _MTRD.integral + kD * _MTRD.derivative, MIN_PWM, MAX_PWM);

        _MTRA.prev_input = _MTRA.ECD_val_diff;
        _MTRB.prev_input = _MTRB.ECD_val_diff;
        _MTRC.prev_input = _MTRC.ECD_val_diff;
        _MTRD.prev_input = _MTRD.ECD_val_diff;

        _writePWM();
        _log();
    }
}

inline void SpeedControllerPID::_read()
{
    _MTRA.ECD_val_prev = _MTRA.ECD_val;
    _MTRB.ECD_val_prev = _MTRB.ECD_val;
    _MTRC.ECD_val_prev = _MTRC.ECD_val;
    _MTRD.ECD_val_prev = _MTRD.ECD_val;

    _MTRA.ECD_val = -_ECDA.read();
    _MTRB.ECD_val = _ECDB.read();
    _MTRC.ECD_val = -_ECDC.read();
    _MTRD.ECD_val = _ECDD.read();

    _MTRA.ECD_val_diff = _MTRA.ECD_val - _MTRA.ECD_val_prev;
    _MTRB.ECD_val_diff = _MTRB.ECD_val - _MTRB.ECD_val_prev;
    _MTRC.ECD_val_diff = _MTRC.ECD_val - _MTRC.ECD_val_prev;
    _MTRD.ECD_val_diff = _MTRD.ECD_val - _MTRD.ECD_val_prev;
}

void SpeedControllerPID::setPWM(int MOTOR_PWM, bool rectify)
{
    _set_point = (float)MOTOR_PWM * MIN_MOTOR_RPPWM;
}

// WARNING: leaving the wheels in the air before run this function.
void SpeedControllerPID::measure()
{
    setPWM(255, false);
    unsigned long curr_time;
    unsigned long prev_time;
    unsigned long interval;
    while (true)
    {
        curr_time = micros();
        interval = curr_time - prev_time;
        if (interval >= 10000)
        {
            prev_time = curr_time;
            _read();
            Serial.print(interval);
            Serial.print(",");
            Serial.print(_MTRA.ECD_val_diff);
            Serial.print(",");
            Serial.print(_MTRB.ECD_val_diff);
            Serial.print(",");
            Serial.print(_MTRC.ECD_val_diff);
            Serial.print(",");
            Serial.println(_MTRD.ECD_val_diff);
        }
    }
}

#endif