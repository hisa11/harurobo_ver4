#include "stateMachine.hpp"
#include "mbed.h"
#include "pid.hpp"

enum class CatapultState {
    IDLE,
    MOVING_FORWARD,
    MOVING_BACKWARD,
    STOPPED
};

CatapultState current_state = CatapultState::IDLE;
int speed = 0;

void updateCatapultState(bool R2, bool catapult_limit, int catapult_revolutions) {
    // 状態遷移判定
    if (R2 && catapult_limit) {
        if (catapult_revolutions <= -250 && catapult_revolutions >= -450) {
            current_state = CatapultState::MOVING_BACKWARD;
        } else {
            current_state = CatapultState::MOVING_FORWARD;
        }
    } else if (!catapult_limit || !R2 || catapult_revolutions < -300) {
        current_state = CatapultState::STOPPED;
    } else {
        current_state = CatapultState::IDLE;
    }

    // 状態に応じた処理
    switch (current_state) {
        case CatapultState::MOVING_FORWARD:
            catapult_pid.set_goal(0);
            break;

        case CatapultState::MOVING_BACKWARD:
            speed = -4000;
            break;

        case CatapultState::STOPPED:
            speed = 0;
            break;

        case CatapultState::IDLE:
            speed = 0;
            break;
    }
}
