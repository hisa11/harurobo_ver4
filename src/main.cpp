#include "mbed.h"
#include "key.hpp"
#include "serial_read.hpp"
#include "pid.hpp"
#include "c610.hpp"
#include "stateMachine.hpp"
#include "FP.hpp"
#include "QEI.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#define WIDTH 200
#define DEPTH 200
#define ROTATE 0.5
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
QEI catapult_encoder(D2, D3, NC, PPR, QEI::X4_ENCODING);

CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);

C610 c610(can1);
FP penguin(35, can2);

int servo[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int infura[2] = {0, 0};
int suction = 0;

PID catapult_pid(1.8, 0.0, 1.0);
double move_pid_Tilt_p = 1.0;
PID infura_pid[2] = {PID(1.0, 0.0, 0.0), PID(1.0, 0.0, 0.0)};
PID cone_pid(1.0, 0.0, 0.0);

void move(std::string msg)
{
    msg.erase(0, 2);
    std::vector<double> joys = to_numbers(msg);
    double move_val[4] = {-(joys[0] + joys[1] - (WIDTH + DEPTH)) * ROTATE,
                          (-joys[0] - joys[1] + (WIDTH + DEPTH)) * ROTATE,
                          -(joys[0] - joys[1] - (WIDTH + DEPTH)) * ROTATE,
                          (joys[0] + joys[1] + (WIDTH + DEPTH)) * ROTATE};
    char buf[100];
    snprintf(buf, sizeof(buf), "%f,%f,%f,%f\n", move_val[0], move_val[1], move_val[2], move_val[3]);
    nucleo.write(buf, strlen(buf));
}

void key_binding()
{
    updateCatapultState(R2, catapult_limit, catapult_encoder.getPulses());
    updateCrossButtonState(Cross, servo[0], servo_mode0, servo_mode1);
    updateconeState(Triangle, servo[1], suction, cone_mode0,cone_mode1, suction_power);
    updateAndHandleInfura(Up, Down, Right, Left, infura[0], infura[1]);
    koudaihou(Square, servo[1], kodaihou_mode0, kodaihou_mode1);
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
        // for (int i = 0; i < 4; i++)
        //     move_pid[i].set_dt(dt);
        for (int i = 0; i < 2; i++)
            infura_pid[i].set_dt(dt);
        catapult_pid.set_dt(dt);
        cone_pid.set_dt(dt);
        // c610.set_power(1, move_pid[0].do_pid(c610.get_rpm(1)));
        // c610.set_power(2, move_pid[1].do_pid(c610.get_rpm(2)));
        // c610.set_power(3, move_pid[2].do_pid(c610.get_rpm(3)));
        // c610.set_power(4, move_pid[3].do_pid(c610.get_rpm(4)));
        c610.set_power(5, infura_pid[0].do_pid(c610 .get_rpm(5)));
        c610.set_power(6, infura_pid[1].do_pid(c610 .get_rpm(6)));
        c610.set_power(7, catapult_pid.do_pid(c610.get_rpm(7)));
        c610.set_power(8, cone_pid.do_pid(c610.get_rpm(8)));
        pre_time = now_time;
    }
}

int main()
{
    Thread thread;
    thread.start(serial_read);
    Thread thread2;
    thread2.start(PID_calculation);
    Thread thread3;
    thread3.start(key_binding);
    pc.set_blocking(false);

    catapult_limit.mode(PullUp);

    servo[0] = cone_mode0;
    servo[1] = kodaihou_mode0;
    servo[7] = servo_mode0;
    CANMessage servo_msg(SERVO_canID, reinterpret_cast<uint8_t *>(servo), 8);
    can2.write(servo_msg);

    while (1)
    {
        penguin.pwm[0] = suction;
        penguin.pwm[1] = suction;
        CANMessage servo_msg(SERVO_canID, reinterpret_cast<uint8_t *>(servo), 8);
        c610.send_message();
        can2.write(servo_msg);
        penguin.send();
    }
}