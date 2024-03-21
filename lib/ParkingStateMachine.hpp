#ifndef _STATE_H_
#define _STATE_H_

#include "Communication.hpp"
#include "Display.hpp"
#include "MotorController.hpp"
#include "Sensor.hpp"
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

#define _PWM_ 50

#define LIGHT_THRESHOLD 15
#define DISTANCE_THRESHOLD 2.0
#define SLOW_DOWN_DISTANCE 10.0

#define SLOW_DOWN_DISTANCE_5 1.0

#define TARGET_DISTANCE 27.5
#define STATIC_COUNTER 150

#define STATIC_LIGHT_COUNTER 50

enum ControlState
{
    IDLE,
    PERPENDICULAR,
    MOVE_TO_25,
    TURN_CW_90,
    TURN_CCW_270,
    TURN_CW_180,
    MEASURE,
    PARK,
    MOVE_TO_5,
    PARK_2
};

ControlState state = IDLE;

int _move_steady_counter = 0;
int park_steady_counter = 0;

static unsigned long pause_start_time;

void pause_by(int time = 2000)
{
    motor.STOP();
    pause_start_time = millis();
    while (millis() < pause_start_time + time)
    {
        mpu.update();
        sensor.update();
        sensorDataDisplay();
    }
}

bool move_to_25()
{
    if (_move_steady_counter > STATIC_COUNTER)
    {
        _move_steady_counter = 0;
        return true;
    }
    float error = sensor.getAverageDistance();
    float delta = fabs(error) - 25.0;
    if (abs(delta) > DISTANCE_THRESHOLD)
    {
        float speed = abs(delta) < SLOW_DOWN_DISTANCE ? delta / SLOW_DOWN_DISTANCE : 1;
        delta > 0 ? motor.ADVANCE(_PWM_ * speed, true) : motor.BACK(_PWM_ * speed, true);
        _move_steady_counter = 0;
    }
    else
    {
        _move_steady_counter++;
        motor.STOP();
    }
    return false;
}

bool move_to_5()
{
    if (_move_steady_counter > STATIC_COUNTER)
    {
        _move_steady_counter = 0;
        return true;
    }
    float delta = sensor.getDistanceL() - 5.0;
    if (abs(delta) > DISTANCE_THRESHOLD)
    {
        delta > 0 ? motor.ADVANCE(_PWM_ * 0.75, false) : motor.BACK(_PWM_ * 0.75, false);
        _move_steady_counter = 0;
    }
    else
    {
        _move_steady_counter++;
        motor.STOP();
    }
    return false;
}

bool park()
{
    if (park_steady_counter > STATIC_LIGHT_COUNTER)
    {
        park_steady_counter = 0;
        return true;
    }
    int delta = sensor.getLightL() - sensor.getLightR();
    if (abs(delta) > LIGHT_THRESHOLD)
    {
        delta > 0 ? motor.LEFT(_PWM_) : motor.RIGHT(_PWM_);
        park_steady_counter = 0;
    }
    else
    {
        park_steady_counter++;
        motor.STOP();
    }
    return false;
}

inline void parkingStateMachine()
{
    switch (state)
    {
    case IDLE: // After start, wait for 2 sec
        sensorDataDisplay();
        break;

    case MOVE_TO_25: // Move to a location of 25cm from the wall, and wait for 2 sec.
        if (move_to_25())
        {
            pause_by();
            motor.setInitAngle(mpu.getAngleZ());
            motor.ROTATE_TO(-90);
            textDisplay("TURN_CW_90");
            state = TURN_CW_90;
        }
        break;

    case TURN_CW_90: // Turn CW 90° (-90°), wait 2 sec .
        if (motor.hasStopped())
        {
            pause_by();
            motor.ROTATE_TO(180);
            textDisplay("TURN_CCW_270");
            state = TURN_CCW_270;
        }
        break;

    case TURN_CCW_270: // Turn CCW 270°(90°), wait 2 sec.
        if (motor.hasStopped())
        {
            pause_by();
            motor.ROTATE_TO(0);
            textDisplay("TURN_CW_180");
            state = TURN_CW_180;
        }
        break;

    case TURN_CW_180: // Turn CW 180°(-180°), wait 2 sec.
        if (motor.hasStopped())
        {
            pause_by();
            textDisplay("PARK");
            state = PARK;
        }
        break;

    case PARK: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
        if (park())
        {
            pause_by();
            textDisplay("MOVE_TO_5");
            state = MOVE_TO_5;
        }
        break;

    case MOVE_TO_5:
        if (move_to_5())
        {
            pause_by();
            textDisplay("PARK_2");
            state = PARK_2;
        }
        break;

    case PARK_2: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
        if (park())
        {
            pause_by();
            state = IDLE;
        }
        break;

    default:
        break;
    }
}

#endif