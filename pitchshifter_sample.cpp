#include <unistd.h>
#include <common.hpp>
#include <iostream>
#include <noisegate.hpp>
#include <pitchshifter.hpp>

using namespace ssp;

#define INSIZE 2048
#define OUTSIZE 2048
#define FS 48000.0

int main() {
    PitchShifter<double> shifter = PitchShifter<double>(0.8, 0.001, 0.02, INSIZE, 620, FS);
    NoiseGate<double> gate = NoiseGate<double>(5, 50, -50.0, FS);
    short in[INSIZE];
    double f[INSIZE];
    double fout[OUTSIZE];
    short out[OUTSIZE];
    while (true) {
        size_t nread = read(0, (char*)in, sizeof(in));
        for (int i = 0; i < INSIZE; i++) {
            f[i] = ((double)in[i]) / 32768.0;
        }
        shifter.process(f, INSIZE);
        while (true) {
            int readCount = shifter.read(fout, OUTSIZE);
            if (readCount < 0) {
                break;
            }
            gate.filter(fout, OUTSIZE);
            for (int i = 0; i < OUTSIZE; i++) {
                out[i] = short(fout[i] * 32767.0);
            }
            write(1, (char*)out, OUTSIZE * 2);
        }
        if (nread < INSIZE * 2) break;
    }
}