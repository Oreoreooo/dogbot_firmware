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

bool is_finished = false;

#define LIGHT_THRESHOLD 10
#define DISTANCE_THRESHOLD 3.0
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

float dist_err_threshold = 1.5;
float dist_offset = 2.5;
int perpen_counter = 0;

int move_25_steady_counter = 0;
int move_5_steady_counter = 0;

int park_steady_counter = 0;
bool parking_start = false;
int wait_time = 0;

inline void pause(int time = 2000)
{
    wait_time = millis();
    while (millis() < (wait_time + time))
    {
        mpu.update();
        sensor.update();
        // sensorDataDisplay();
    }
    mpu.calcOffsets();
}

inline bool perpendicular()
{
    float delta_angle = 10.0;
    static float before_distL, before_distR;
    float delta_dist = sensor.getDistanceL() - sensor.getDistanceR() - dist_offset;

    textDisplay(delta_dist);

    if (perpen_counter > 30)
    {
        textDisplay("perpen");
        perpen_counter = 0;
        return true;
    }
    if (abs(delta_dist) < dist_err_threshold)
    {
        textDisplay("waiting");
        perpen_counter += 1;
        return false;
    }
    if (delta_dist > 0) // rotate CW
    {
        textDisplay(delta_dist);
        perpen_counter = 0;
        motor.ROTATE_TO(delta_angle);
        return false;
    }
    else if (delta_dist < 0) // rotate CCW
    {
        textDisplay(delta_dist);
        perpen_counter = 0;
        motor.ROTATE_TO(-delta_angle);
        return false;
    }
}

inline bool move_to_25()
{
    if (move_25_steady_counter > STATIC_COUNTER)
    {
        move_25_steady_counter = 0;
        return true;
    }
    float delta = sensor.getDistanceL() - 25.0;
    if (abs(delta) > DISTANCE_THRESHOLD)
    {
        float speed = abs(delta) < SLOW_DOWN_DISTANCE ? delta / SLOW_DOWN_DISTANCE : 1;
        delta > 0 ? motor.ADVANCE(_PWM_ * speed, true) : motor.BACK(_PWM_ * speed, true);
        move_25_steady_counter = 0;
    }
    else
    {
        move_25_steady_counter++;
        motor.STOP();
    }
    return false;
}

inline bool move_to_5()
{
    if (move_5_steady_counter > STATIC_COUNTER)
    {
        move_5_steady_counter = 0;
        return true;
    }
    float delta = sensor.getDistanceL() - 5.0;
    if (abs(delta) > DISTANCE_THRESHOLD)
    {
        delta > 0 ? motor.ADVANCE(_PWM_, true) : motor.BACK(_PWM_, true);
        move_5_steady_counter = 0;
    }
    else
    {
        move_5_steady_counter++;
        motor.STOP();
    }
    return false;
}

inline bool park()
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

inline void parkingStateMachine(ControlState state)
{
    switch (state)
    {

    case IDLE: // After start, wait for 2 sec
        break;

    case PERPENDICULAR:
        if (perpendicular())
        {
            motor.STOP();
            // WAIT();
            state = MOVE_TO_25;
        }
        pause(200);
        break;

    case MOVE_TO_25: // Move to a location of 25cm from the wall, and wait for 2 sec.
        if (move_to_25())
        {
            motor.STOP();
            pause();
            motor.setInitAngle(mpu.getAngleZ());
            motor.ROTATE_TO(-90);
            state = TURN_CW_90;
        }
        break;

    case TURN_CW_90: // Turn CW 90° (-90°), wait 2 sec .
        if (motor.hasStopped())
        {
            motor.STOP();
            pause();
            motor.ROTATE_TO(180);
            state = TURN_CCW_270;
        }
        break;

    case TURN_CCW_270: // Turn CCW 270°(90°), wait 2 sec.
        if (motor.hasStopped())
        {
            pause();
            motor.ROTATE_TO(0);
            state = TURN_CW_180;
        }
        break;

    case TURN_CW_180: // Turn CW 180°(-180°), wait 2 sec.
        if (motor.hasStopped())
        {
            pause();
            state = PARK;
        }
        break;

    case PARK: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
        if (park())
        {
            motor.STOP();
            pause();
            state = MOVE_TO_5;
        }
        break;

    case MOVE_TO_5:
        if (move_to_5())
        {
            motor.STOP();
            pause();
            state = PARK_2;
        }
        break;

    case PARK_2: // Final position of the car parked at 5cm from the wall, center to the LED bar and perpendicular to the wall.
        if (park())
        {
            motor.STOP();
            pause();
            is_finished = true;
            state = IDLE;
        }
        break;

    default:
        break;
    }
}

#endif