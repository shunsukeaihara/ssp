#ifndef COMMON_H_
#define COMMON_H_

#define _USE_MATH_DEFINES
#include <math.h>

namespace ssp{

template <typename T> inline int isEpochMark(const T in){
    int *p = (int*)(&in);
    return (*p) & (1<<0);
}

template <typename T> inline void setEpochMark(T *in){
    int *p = (int*)(in);
    (*p) |= (1<<0);
}

template <typename T> inline void unsetEpochMark(T *in){
    int *p = (int*)(in);
    (*p) &= ~(1<<0);
}

int calcGcd(int a, int b){
    while(b!=0){
        int tmp = a;
        a = b;
        b = tmp%b;
    }
    return a;
}

template <typename T> inline T nyquistFromTwo(int a, int b){
    if (a > b)a = b;
    return T(a) / 2.2;
}

}
#endif