#include "MotorController.hpp"
#include "ServoController.hpp"
#include "Display.hpp"
#include <Arduino.h>

MotorController motor;
ServoController servo;

//====================================================================//



const byte buffer_length = 64;

char char_buffer[buffer_length];
char temp_char_buffer[buffer_length];
char *token;
char rc;

float args[8] = {0.0};

const char start_marker = '<';
const char end_marker = '>';

boolean data_available = false;
boolean recv_in_progress = false;
byte ndx = 0;

// https://forum.arduino.cc/t/serial-input-basics-updated/382007/3
inline void serialReceiver()
{
    if (Serial.available() > 0 && !data_available)
    {
        rc = (char)Serial.read();
        if (rc == start_marker)
        {
            recv_in_progress = true;
            ndx = 0;
            data_available = false;
        }
        else if (recv_in_progress)
        {
            if (rc != end_marker)
            {
                char_buffer[ndx] = rc;
                ndx++;
                if (ndx >= buffer_length)
                {
                    ndx = buffer_length - 1;
                }
            }
            else
            {
                char_buffer[ndx] = '\0';
                recv_in_progress = false;
                ndx = 0;
                data_available = true;
            }
        }
    }
}

inline void serialHandler()
{
    switch (strtok(temp_char_buffer, ",")[0])
    {
    case 'S':
        Serial.println("<OK>");
        break;
    case 'M':
        for (int i = 0; i < 4; i++)
        {
            token = strtok(nullptr, ",");
            args[i] = atof(token);
        }
        motor.setTargetSpeed(args[0], args[1], args[2], args[3]);
        Serial.println("<OK>");
        break;
    case 'E':
        motor.sendFeedback();
        break;
    case 'P':
        for (int i = 0; i < 5; i++)
        {
            token = strtok(nullptr, ",");
            args[i] = atof(token);
        }
        servo.setServoPosition(args[0], args[1], args[2], args[3], args[4]);
        Serial.println("<OK>");
        break;
    default:
        Serial.println("<ERROR>");
        break;
    }
}

void setup()
{
    MotorController::begin();
    Serial.begin(115200);
}

void loop()
{
    serialReceiver();
    if (data_available)
    {
        strcpy(temp_char_buffer, char_buffer);
        serialHandler();
        data_available = false;
    }
    motor.control();
}