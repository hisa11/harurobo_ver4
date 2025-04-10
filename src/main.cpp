#include "mbed.h"
#include "key.hpp"
#include "serial_read.hpp"
#include "pid.hpp"
#include "c610.hpp"

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

BufferedSerial pc(USBTX, USBRX, 115200);
BufferedSerial arudino(PB_6, PA_10, 9600);
serial_unit serial(pc);

// c610.hpp 内で定義されているクラス名を仮に C610 とした場合のインスタンス宣言


CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PA_11, PA_12, (int)1e6);

C610 c610(can1);

PID catapult_pid(1.8, 0.0, 1.0);
double move_pid_Tilt_p = 1.0;
PID move_pid[4] = {PID(1.2 * move_pid_Tilt_p, 0.005, 1.0),
    PID(1.3 * move_pid_Tilt_p, 0.005, 1.0),
    PID(1.65 * move_pid_Tilt_p, 0.005, 1.0),
    PID(0.8 * move_pid_Tilt_p, 0.005, 1.0)};

void move(std::string msg) {
    msg.erase(0, 2);
    std::vector<double> joys = to_numbers(msg);
    double move_val[4] = {-(joys[0] + joys[1] - (WIDTH + DEPTH)) * ROTATE,
                      (-joys[0] - joys[1] + (WIDTH + DEPTH)) * ROTATE,
                      -(joys[0] - joys[1] - (WIDTH + DEPTH)) * ROTATE,
                      (joys[0] + joys[1] + (WIDTH + DEPTH)) * ROTATE};
    for (size_t i = 0; i < joys.size(); i++) {
        move_pid[i].set_goal(move_val[i]);
    }
}

void key_binding(){
    
}

void PID_calculation() {
    auto pre_time = HighResClock::now();
    while (1) {
        auto now_time = HighResClock::now();
        c610.param_update();

        double dt = std::chrono::duration_cast<std::chrono::microseconds>(
                        now_time - pre_time)
                        .count() /
                    1000000.0;
        for (int i = 0; i < 4; i++)
            move_pid[i].set_dt(dt);
        c610.set_power(1, move_pid[0].do_pid(c610.get_rpm(1)));
        c610.set_power(2, move_pid[1].do_pid(c610.get_rpm(2)));
        c610.set_power(3, move_pid[2].do_pid(c610.get_rpm(3)));
        c610.set_power(4, move_pid[3].do_pid(c610.get_rpm(4)));
        pre_time = now_time;
    }
}

int main(){
    Thread thread;
    thread.start(serial_read);
    pc.set_baud(115200);
    pc.set_blocking(false);
}