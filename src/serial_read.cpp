#include "serial_read.hpp"
#include "mbed.h"
#include "key.hpp"
#include <string>

std::string serial_unit::read_serial()
{
    static std::string buff_str = "";
    char buf[1];

    while (men_serial.readable())
    {
        men_serial.read(buf, 1);
        if (buf[0] == '|')
        {
            buff_str = str;
            str.clear();
            return buff_str;
        }
        else
        {
            str += buf[0];
        }
    }
    return "";
}

void serial_read() {
    while (1) {

        std::string msg = serial.read_serial();
        if (msg != "") {
            if (msg[0] == 'n') {
                if (R3 && Right) {
                    msg = "n:-0.500000:0.000000:0.000000:0.000000|";
                    printf("Updated msg: %s\n",
                           msg.c_str()); // 増加した値を表示
                } else if (R3 && Left) {
                    msg = "n:0.500000:0.000000:0.000000:0.000000|";
                    printf("Updated msg: %s\n",
                           msg.c_str()); // 増加した値を表示
                } else if (R3 && Up) {
                    msg = "n:0.000000:0.500000:0.000000:0.000000|";
                    printf("Updated msg: %s\n",
                           msg.c_str()); // 増加した値を表示
                } else if (R3 && Down) {
                    msg = "n:0.000000:-0.500000:0.000000:0.000000|";
                    printf("Updated msg: %s\n",
                           msg.c_str()); // 増加した値を表示
                }
                move(msg);
            } else {
                key_puress(msg);
            }
        }
        key_binding();
        // ThisThread::sleep_for(10ms);
    }
}