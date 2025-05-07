#include "mbed.h"
#include "key.hpp"
#include "serial_read.hpp"
#include "pid.hpp"
#include "c610.hpp"
#include "stateMachine.hpp"
#include "FP.hpp"


#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#define WIDTH 200
#define DEPTH 200
#define ROTATE 20000
#define PPR 2048
#define SERVO_canID 140

int servo_mode0 = 225;
int servo_mode1 = 0;
int cone_mode0 = 0;
int cone_mode1 = 255;
int kodaihou_mode0 = 65;
int kodaihou_mode1 = 45;
int suction_power = 3000;
int cone_speed = 1000;



BufferedSerial pc(USBTX, USBRX, 115200);
BufferedSerial nucleo(PA_0, PA_1, 115200);
serial_unit serial(pc);

DigitalIn catapult_limit(D4);
DigitalIn cone_limit(D5);

// c610.hpp 内で定義されているクラス名を仮に C610 とした場合のインスタンス宣言
CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);


FP penguin(35, can2);
FP penguin2(40, can2);
C610 c610(can1);

int servo[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int infura[2] = {0, 0};
int suction = 0;


double move_pid_Tilt_p = 1.0;
PID move_pid[4] = {PID(0.7, 0.5, 0.0), PID(0.7, 0.5, 0.0), PID(0.7, 0.5, 0.0), PID(0.7, 0.5, 0.0)};
PID baisuto_pid(0.7, 1.0, 0.0);
PID kyouyuu_pid[2] = {PID(0.7, 0.5, 0.0), PID(0.7, 0.5, 0.0)};
float move_val[4];

void move_aa(std::string msg)
{
    // printf("msg: %s\n", msg.c_str());
    msg.erase(0, 2);
    std::vector<double> joys_d = to_numbers(msg);
    std::vector<float> joys(joys_d.begin(), joys_d.end());
    for (auto &joy : joys) {
        if (joy > -0.08 && joy < 0.08) {
            joy = 0.0;
        }
    }
    
    // printf("joys: %f %f %f %f\n", joys[0], joys[1], joys[2], joys[3]);
    move_val[0] = (joys[0] - joys[1]+joys[2]) * ROTATE;
    move_val[1] = (-joys[0] + joys[1]+joys[2]) * ROTATE;
    move_val[2] = (joys[0] + joys[1]+joys[2]) * ROTATE;
    move_val[3] = -(-joys[0] - joys[1]+joys[2]) * ROTATE;
    // move_val[0] = (joys[0] - joys[1]-joys[2]) * ROTATE;
    // move_val[1] = (-joys[0] - joys[1]-joys[2]) * ROTATE;
    // move_val[2] = -(joys[0] - joys[1]+joys[2]) * ROTATE;
    // move_val[3] = -(-joys[0] - joys[1]+joys[2]) * ROTATE;
    // for(int i = 0;i<4;i++){
    //     move_pid[i].set_goal(move_val[i]);
    // }
    for(int i = 0;i<4;i++){
        penguin.pwm[i] = move_val[i];
    }
    
    // printf("move_val: %d %d %d %d\n", (int)move_val[0], (int)move_val[1], (int)move_val[2], (int)move_val[3]);
}
void key_binding() {
    // 必要に応じて処理を追加
    if(Up == 1) {
        // 上キーが押されたときの処理
        // penguin.pwm[0] = 5000;
        baisuto_pid.set_goal(5000);
        // c610.set_power(5,12000);
    }else if(Down == 1) {
        // 下キーが押されたときの処理
        // penguin.pwm[0] = -5000;
        baisuto_pid.set_goal(-5000);
        // c610.set_power(5,-3000);
    }else {
        // 上下キーが押されていないときの処理
        // penguin.pwm[0] = 0;
        baisuto_pid.set_goal(0);
        // c610.set_power(5,8000);
    }
    if(R1 == 1){
        penguin2.pwm[2] = 12000;
        printf("R1\n");
    }
    else if(L1 == 1){
        penguin2.pwm[2] = -12000;
    }
    else{
        penguin2.pwm[2] = 0;
    }
    if(Triangle == 1){
        kyouyuu_pid[0].set_goal(4000);
    }else if(Cross == 1){
        kyouyuu_pid[0].set_goal(-4.000);
    }else{
        kyouyuu_pid[0].set_goal(0);
    }
    if(Circle == 1){
        kyouyuu_pid[1].set_goal(8000);
    }else if(Square == 1){
        kyouyuu_pid[1].set_goal(-8000);
    }else{
        kyouyuu_pid[1].set_goal(0);
    }
}

void PID_calculation()
{
    auto pre_time = HighResClock::now();
    while (1)
    {
        auto now_time = HighResClock::now();
        c610.param_update();

        double dt = std::chrono::duration_cast<std::chrono::microseconds>(
                        now_time - pre_time)
                        .count() /
                    1000000.0;
        double baisuto_dt = std::chrono::duration_cast<std::chrono::microseconds>(
                        now_time - pre_time)
                        .count() /
                    1000000.0;
        double kyouyuu_dt = std::chrono::duration_cast<std::chrono::microseconds>(
                        now_time - pre_time)
                        .count() /
                    1000000.0;
        baisuto_pid.set_dt(baisuto_dt);
        kyouyuu_pid[0].set_dt(kyouyuu_dt);
        kyouyuu_pid[1].set_dt(kyouyuu_dt);
        for (int i = 0; i < 4; i++)
            move_pid[i].set_dt(dt);
        // c610.set_power(1, move_pid[0].do_pid(c610.get_rpm(1)));
        // c610.set_power(2, move_pid[1].do_pid(c610.get_rpm(2)));
        // c610.set_power(3, move_pid[2].do_pid(c610.get_rpm(3)));
        // c610.set_power(4, move_pid[3].do_pid(c610.get_rpm(4)));
        c610.set_power(5, baisuto_pid.do_pid(c610.get_rpm(5)));
        c610.set_power(6, kyouyuu_pid[0].do_pid(c610.get_rpm(6)));
        // c610.set_power(7, kyouyuu_pid[1].do_pid(c610.get_rpm(7)));
        ThisThread::sleep_for(20ms);
        pre_time = now_time;
    }
}

int main()
{
    Thread thread;
    thread.start(serial_read);
    Thread thread2;
    thread2.start(PID_calculation);
    pc.set_blocking(false);
    
    catapult_limit.mode(PullUp);
    
    // CANMessage servo_msg(SERVO_canID, reinterpret_cast<uint8_t *>(servo), 8);
    // can2.write(servo_msg);
    
    while (1)
    {
        // for(int i = 0;i<4;i++){
            //     // penguin.pwm[i]=move_val[i];
            //     c610.set_power(i + 1, move_val[i]);
        // }
        // printf("pwm: %d %d %d %d\n", penguin.pwm[0], penguin.pwm[1], penguin.pwm[2], penguin.pwm[3]);
        
        penguin.send();
        penguin2.send();
        c610.send_message();
        // printf("%d\n",move_pid[0].get_send_power);
    }
}