#include <unistd.h>
#include <iostream>
#include <common.hpp>
#include <esola.hpp>

using namespace ssp;

#define INSIZE 512
#define OUTSIZE 620

int main(){
    std::ios_base::sync_with_stdio(false);
    ESOLA<float> *esola = new ESOLA<float>(16000.0, 1.2, 16000 * 0.02, 512);
    short in[INSIZE];
    float f[INSIZE];
    float fout[OUTSIZE];
    short out[OUTSIZE];
    int sumRead = 0;
    int sumWrite = 0;
    while(true){
        size_t nread = read(0, (char*)in, sizeof(in));
        sumRead += nread / 2;
        for (int i=0;i<INSIZE;i++){
            f[i] = ((float)in[i]) / 32768.0;
        }
        esola->process(f, INSIZE);
        while(true){
            int readCount = esola->read(fout, OUTSIZE);
            sumWrite += readCount;
            if (readCount<0){
                break;
            }
            for(int i=0; i<OUTSIZE;i++){
                out[i] = short(fout[i] * 32767.0);
            }
            write(1, (char*)out, OUTSIZE * 2);
        }
        if (nread < INSIZE * 2)break;
    }
    delete esola;
}