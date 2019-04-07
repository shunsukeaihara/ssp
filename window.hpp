#ifndef WINDOW_H_
#define WINDOW_H_

#define _USE_MATH_DEFINES
#include <math.h>

namespace ssp{

template <typename T> inline void hammingWindow(T* win, const int len){
    T coef = M_PI * 2 / (len - 1);
    for (int i=0; i<len; i++){
        win[i] = 0.54 - 0.46 * cos(coef * i);
    }
}

template <typename T> inline void hanningWindow(T* win, const int len){
    T coef = M_PI * 2 / (len - 1);
    for (int i=0; i<len; i++){
        win[i] = 0.5 - 0.5 * cos(coef * i);
    }
}

template <typename T> inline void blackmanWindow(T *win, const int len){
        T coef_1 = M_PI * 2 / (len - 1);
        T coef_2 = M_PI * 4 / (len - 1);
       for (int i=0; i<len; i++){
            win[i] = 0.42 - 0.5 * cos(coef_1 * i) + 0.08 * cos(coef_2 * i);
        }
}

}

#endif /* WINDOW_H_ */
