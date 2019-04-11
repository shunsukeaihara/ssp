#include <unistd.h>
#include <common.hpp>
#include <esola.hpp>
#include <iostream>
#include <noisegate.hpp>

using namespace ssp;

#define INSIZE 2048
#define OUTSIZE 2048
#define FS 48000.0

int main() {
    ESOLA<double> esola = ESOLA<double>(0.9, FS * 0.02, INSIZE, FS);
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
        esola.process(f, INSIZE);
        while (true) {
            int readCount = esola.read(fout, OUTSIZE);
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