#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP
#include "mbed.h"
#include "pid.hpp"

enum class CatapultState {
    IDLE,
    MOVING_FORWARD,
    MOVING_BACKWARD,
    STOPPED
};
enum class CrossButtonState {
    IDLE,            // ボタンが押されていない
    PRESSED,         // ボタンが押されている
};
enum class DriveState {
    IDLE,
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT
};


void updateCatapultState(bool R2, bool catapult_limit, int catapult_revolutions);
void updateCrossButtonState(bool Cross, int servo , int SERVOVO_MODE0, int SERVOVO_MODE1);
void updateAndHandleInfura(bool Up, bool Down, bool Right, bool Left, int &infura0, int &infura1);


#endif