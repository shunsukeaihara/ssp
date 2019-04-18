#ifndef COMMON_H_
#define COMMON_H_

#define _USE_MATH_DEFINES
#include <math.h>

#define SSP_LOG_2_DB 8.6858896380650365530225783783321
#define SSP_DB_2_LOG 0.11512925464970228420089957273422
#define SSP_DC_OFFSET 1.0E-25
#define SSP_EPSILON 1.0e-4

namespace ssp {

template <typename T>
static inline int isEpochMark(const T in) {
    int *p = (int *)(&in);
    return (*p) & (1 << 0);
}

template <typename T>
static inline void setEpochMark(T *in) {
    int *p = (int *)(in);
    (*p) |= (1 << 0);
}

template <typename T>
static inline void unsetEpochMark(T *in) {
    int *p = (int *)(in);
    (*p) &= ~(1 << 0);
}

static inline int calcGcd(int a, int b) {
    while (b != 0) {
        int tmp = a;
        a = b;
        b = tmp % b;
    }
    return a;
}

template <typename T>
static inline T nyquistFromTwo(int a, int b) {
    if (a > b) a = b;
    return T(a) / 2.2;
}

template <typename T>
static inline T lin2dB(const T lin) {
    return log(lin) * SSP_LOG_2_DB;
}

template <typename T>
static inline T db2lin(const T db) {
    return exp(db * SSP_DB_2_LOG);
}

}  // namespace ssp
#endif