#include "stateMachine.hpp"
#include "mbed.h"



CatapultState current_state = CatapultState::IDLE;

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
            catapult_pid.set_goal(8000);
            break;

        case CatapultState::MOVING_BACKWARD:
            catapult_pid.set_goal(-4000);
            break;

        case CatapultState::STOPPED:
        catapult_pid.set_goal(0);
            break;

        case CatapultState::IDLE:
            catapult_pid.set_goal(0);
            break;
    }
}

bool servovo_flag = false;
bool kodai_flag = false;

void updateCrossButtonState(bool Cross , int servo , int SERVOVO_MODE0 , int SERVOVO_MODE1) {
    CrossButtonState current_state = CrossButtonState::IDLE;
    // 状態遷移判定
    int servovo = 0;
    if (Cross) {
        current_state = CrossButtonState::PRESSED;
    } else {
        current_state = CrossButtonState::IDLE;
    }

    // 状態に応じた処理
    switch (current_state) {
        case CrossButtonState::PRESSED:
            if (!servovo_flag) {
                // サーボ角度をトグル
                servovo = (servovo == SERVOVO_MODE0) ? SERVOVO_MODE1 : SERVOVO_MODE0;
                servovo_flag = true;  // トグルしたのでフラグを設定
            }
            break;

        case CrossButtonState::IDLE:
            servovo_flag = false;  // ボタンが押されていない場合、フラグをリセット
            break;
    }
}

void koudaihou(bool square, int &Kodaiho, int KODAI_MODE0, int KODAI_MODE1) {
    SquareButtonState current_state = SquareButtonState::IDLE;
    if (square) {
        if (!kodai_flag) {
            Kodaiho = (Kodaiho == KODAI_MODE0) ? KODAI_MODE1 : KODAI_MODE0;
            kodai_flag = true;  // トグルしたのでフラグを設定
        }
    } else {
        kodai_flag = false;  // ボタンが押されていない場合、フラグをリセット
    }
    switch (current_state) {
        case SquareButtonState::PRESSED:
            if (!kodai_flag) {
                // サーボ角度をトグル
                Kodaiho = (Kodaiho == KODAI_MODE0) ? KODAI_MODE1 : KODAI_MODE0;
                servovo_flag = true;  // トグルしたのでフラグを設定
            }
            break;

        case SquareButtonState::IDLE:
            servovo_flag = false;  // ボタンが押されていない場合、フラグをリセット
            break;
    }
}


void updateconeState(bool triangle, int &servo, int &suction, int SERVOVO_MODE0, int SERVOVO_MODE1, int suction_power) {
    CrossButtonState current_state = CrossButtonState::IDLE;
    // 状態遷移判定
    if (triangle) {
        current_state = CrossButtonState::PRESSED;
    } else {
        current_state = CrossButtonState::IDLE;
    }

    // 状態に応じた処理
    switch (current_state) {
        case CrossButtonState::PRESSED:
            if (!servovo_flag) {
                // サーボ角度をトグル
                servo = (servo == SERVOVO_MODE0) ? SERVOVO_MODE1 : SERVOVO_MODE0;
                suction = (servo == SERVOVO_MODE1) ? suction_power : 0;
                servovo_flag = true;  // トグルしたのでフラグを設定
            }
            break;

        case CrossButtonState::IDLE:
            servovo_flag = false;  // ボタンが押されていない場合、フラグをリセット
            break;
    }
}

void updateAndHandleInfura(bool Up, bool Down, bool Right, bool Left, int &infura0, int &infura1) {
    if (Up) {
        infura0 = 5000;
        infura1 = 5000;
    } else if (Down) {
        infura0 = -5000;
        infura1 = -5000;
    } else if (Right) {
        infura0 = 2500;
        infura1 = -2500;
    } else if (Left) {
        infura0 = -2500;
        infura1 = 2500;
    } else {
        infura0 = 0;
        infura1 = 0;
    }
}
