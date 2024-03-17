#ifndef _STATE_H_
#define _STATE_H_

enum ControlState
{
    IDLE,
    MOVE_TO_25,
    TURN_CW_90,
    TURN_CCW_270,
    TURN_CW_180,
    MEASURE,
    PARK
};

#endif