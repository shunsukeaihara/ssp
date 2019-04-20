#include <unistd.h>
#include <common.hpp>
#include <iostream>
#include <resampler.hpp>

using namespace ssp;

#define INSIZE 512
#define OUTSIZE 2048
#define FS 16000.0

int main() {
    Resampler<double> resampler = Resampler<double>(FS, 48000, INSIZE);
    short in[INSIZE];
    double f[INSIZE];
    double fout[OUTSIZE];
    short out[OUTSIZE];
    while (true) {
        size_t nread = read(0, (char*)in, sizeof(in));
        for (int i = 0; i < INSIZE; i++) {
            f[i] = ((double)in[i]) / 32768.0;
        }
        int resampled = resampler.process(f, INSIZE, fout);
        //std::cout << resampled << std::endl;
        for (int i = 0; i < resampled; i++) {
            out[i] = short(fout[i] * 32767.0);
        }
        write(1, (char*)out, resampled * 2);
        if (nread < INSIZE * 2) break;
    }
}