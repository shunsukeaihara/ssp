
#include <delay.hpp>
#include <iostream>
using namespace ssp;

int main() {
    // 100Hzの入力で、1秒毎のディレイが3回
    Delay<double> delay = Delay<double>(1000, 3, 0.8, 100);
    // 1秒毎に1.0を書き込み -> 100サンプル前、200サンプル前、300サンプル前に1.0
    for (int i = 0; i < 300; i++) {
        if (i % 100 == 0) {
            delay.filterOne(1.0);
        } else {
            delay.filterOne(0.0);
        }
    }
    double expected = 1.0 + 1.0 * 0.8 + 1.0 * pow(0.8, 2) + 1.0 * pow(0.8, 3);
    double out = delay.filterOne(1.0);
    if (fabs(expected - out) > SSP_EPSILON) {
        std::cout << "error" << std::endl;
    }
    expected = 1.0;
    out = delay.filterOne(1.0);
    if (fabs(expected - out) > SSP_EPSILON) {
        std::cout << "error" << std::endl;
    }
    Delay<double> delay2 = Delay<double>(1000, 3, 0.8, 100);
    for (int i = 0; i < 300; i++) {
        // 2秒毎に書き込み -> 300サンプル前と100サンプル前に1.0書き込み
        if (i % 200 == 0) {
            delay2.filterOne(1.0);
        } else {
            delay2.filterOne(0.0);
        }
    }
    expected = 1.0 + 1.0 * pow(0.8, 1) + 1.0 * pow(0.8, 3);
    out = delay2.filterOne(1.0);
    if (fabs(expected - out) > SSP_EPSILON) {
        std::cout << "error" << std::endl;
    }
}