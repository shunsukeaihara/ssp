#include <unistd.h>
#include <common.hpp>
#include <compressor.hpp>
#include <iostream>
#include <limitter.hpp>
#include <noisegate.hpp>
#include <pitchshifter.hpp>

using namespace ssp;

#define INSIZE 2048
#define OUTSIZE 2048
#define FS 48000.0

int main() {
    PitchShifter<double> shifter = PitchShifter<double>(0.8, 1.0, 20.0, INSIZE, 620, FS);
    NoiseGate<double> gate = NoiseGate<double>(5, 50, -50.0, FS);
    Compressor<double> comp = Compressor<double>(1, 100, -1, 1.0, FS);
    Limitter<double> limit = Limitter<double>(1, 100, -1, FS);
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
            for (int i = 0; i < OUTSIZE; i++) {
                double x = fout[i];
                x = gate.filterOne(x);
                x *= 6;
                x = comp.filterOne(x);
                x = limit.filterOne(x);
                out[i] = short(x * 32767.0);
            }
            write(1, (char*)out, OUTSIZE * 2);
        }
        if (nread < INSIZE * 2) break;
    }
}