#include <iostream>
#include <resampler.hpp>

using namespace ssp;
int main() {
    Resampler<float> r = Resampler<float>(160000, 480000, 10);
    float in[10] = {};
    float out[30] = {};
    size_t outlen = r.process(in, 10, out);
    if (outlen != 30) {
        std::cout << "error" << std::endl;
    }
}