#include <unistd.h>
#include <common.hpp>
#include <esola.hpp>
#include <iostream>
#include <noisegate.hpp>
#include <zfr_epoch_detector.hpp>
using namespace ssp;

#define INSIZE 2048
#define OUTSIZE 2048
#define FS 48000.0
#define ZFR_AVEGERA_WINDOW_IN_MS 1

int main() {
    ZFREpochDetector<double> zfr = ZFREpochDetector<double>(1, INSIZE, -2, 1, FS);
    ESOLA<double> esola = ESOLA<double>(1.5, 20, INSIZE, FS);
    NoiseGate<double> gate = NoiseGate<double>(5, 50, -50.0, FS);
    short in[INSIZE];
    double f[INSIZE];
    double epochBuffer[INSIZE];
    double fout[OUTSIZE];
    short out[OUTSIZE];
    while (true) {
        size_t nread = read(0, (char*)in, sizeof(in));
        for (int i = 0; i < INSIZE; i++) {
            f[i] = ((double)in[i]) / 32768.0;
        }
        size_t epoched = zfr.process(f, INSIZE, epochBuffer);
        esola.process(epochBuffer, epoched);
        while (true) {
            int readCount = esola.read(fout, OUTSIZE);
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