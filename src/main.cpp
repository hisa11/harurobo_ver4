#include "mbed.h"
#include "key.hpp"
#include "serial_read.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

BufferedSerial pc(USBTX, USBRX, 115200);
BufferedSerial arudino(PB_6, PA_10, 9600);
serial_unit serial(pc);


std::vector<double> to_numbers(const std::string &input) {
    std::vector<double> numbers;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ':')) { // ':'で区切る
        if (token.back() == '|') {         // 最後の '|' を削除
            token.pop_back();
        }
        numbers.push_back(std::stod(token)); // 文字列をdoubleに変換
    }
    return numbers;
}
void key_binding() {

}

void serial_read() {
    while (1) {

        std::string msg = serial.read_serial();
        if (msg != "") {
                key_puress(msg);
            }
        key_binding();
        // ThisThread::sleep_for(10ms);
    }
}

int main(){
    Thread thread;
    thread.start(serial_read);
    std::vector<double> joys;
    pc.set_baud(115200);
    pc.set_blocking(false);
}