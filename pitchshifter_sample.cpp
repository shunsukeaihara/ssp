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
    PitchShifter<double> shifter = PitchShifter<double>(1.2, INSIZE, 620, FS);
    NoiseGate<double> gate = NoiseGate<double>(5, 50, -50.0, FS);
    short in[INSIZE];
    double f[INSIZE];
    double fout[OUTSIZE];
    short out[OUTSIZE];
    int sumRead = 0;
    int sumWrite = 0;
    while (true) {
        size_t nread = read(0, (char*)in, sizeof(in));
        sumRead += nread / 2;
        for (int i = 0; i < INSIZE; i++) {
            f[i] = ((double)in[i]) / 32768.0;
        }
        gate.filter(f, INSIZE);
        shifter.process(f, INSIZE);
        while (true) {
            int readCount = shifter.read(fout, OUTSIZE);
            sumWrite += readCount;
            if (readCount < 0) {
                break;
            }
            for (int i = 0; i < OUTSIZE; i++) {
                out[i] = short(fout[i] * 32767.0);
            }
            write(1, (char*)out, OUTSIZE * 2);
        }
        if (nread < INSIZE * 2) break;
    }
}