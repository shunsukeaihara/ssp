#include <unistd.h>
#include <common.hpp>
#include <iostream>
#include <zfr_epoch_detector.hpp>
using namespace ssp;

#define INSIZE 2048
#define OUTSIZE 2048
#define ZFR_AVEGERA_WINDOW_IN_SEC 0.001
#define FS 48000.0

int main() {
    ZFREpochDetector<double> zfr = ZFREpochDetector<double>(FS * ZFR_AVEGERA_WINDOW_IN_SEC, INSIZE, -2, 1);
    short in[INSIZE];
    double f[INSIZE];
    double fout[OUTSIZE];
    short out[OUTSIZE * 2];
    while (true) {
        size_t nread = read(0, (char *)in, sizeof(in));
        for (int i = 0; i < INSIZE; i++) {
            f[i] = ((double)in[i]) / 32768.0;
        }
        size_t processed = zfr.process(f, INSIZE, fout);
        for (int i = 0; i < processed; i++) {
            // 片側チャンネルにepoch embding結果を書き込み。audacityとかでepoch markを確認
            out[i * 2] = short(fout[i] * 32767.0);
            out[i * 2 + 1] = short(isEpochMark(fout[i]) * 32767);
        }
        write(1, (char *)out, processed * 4);
        if (nread < INSIZE * 2) break;
    }
}