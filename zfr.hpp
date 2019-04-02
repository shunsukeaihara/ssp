#ifndef ZFR_H_
#define ZFR_H_

#include <math.h>
#include <common.hpp>
#include <ring_buffer.hpp>

namespace ssp{

template<class T>
class ZFR {
public: 
    ZFR(size_t avgWindow, size_t winsize, T a1, T a2): _avgWindow(avgWindow), _winsize(winsize), _a1(a1), _a2(a2){
        init();
    }

    virtual ~ZFR(){
        delete(_y2);
    }
   
    size_t process(T *s, size_t len, T *out){
        for (int i=0; i<len; i++){
            // preprocessed to remove the lowfrequency bias
            T x = s[i] - _s_1;
            _s_1 = s[i];
            T _y1 = -(_a1 * _y1_1 + x + _a2 * _y1_2 + x);
            _y2[i] = -(_a1 * _y2[i-1] + _y1 + _a2 * _y2[i-2] + _y1);
            _y1_2 = _y1_1;
            _y1_1 = _y1;
        }
        size_t processed=0;
        T last = 0;
        for (int i=--_startOffset; i< len-_avgWindow; i++){
            T sum = 0;
            for (int j=-_avgWindow; j<=_avgWindow; j++){
                sum += _y2[i+j];
            }
            out[i+_startOffset] = _y2[i] - sum/(_avgWindow * 2 + 1);
            // positive zero cross pointの判定
            if (signbit(last * out[i+_startOffset]) && signbit(out[i+_startOffset])){
                // 前回との積が負で、新しい値が正の時epoch mark -> LSBを1にする
                // little endian前提のコードなのでbig endian系の場合はまた考慮する必要がある
                setEpochMark(&out[i+_startOffset]);
            }else{
                //LSBを0にする
                unsetEpochMark(&out[i+_startOffset]);
            }
            last = out[i+_startOffset];
            processed += 1;
        }
        _startOffset = _avgWindow;
        _y2->movePos(len);
        return processed;
    }

    void reset(){
        delete(_y2);
        init();
    }
private:
    const size_t _avgWindow;
    const size_t _winsize;
    const T _a1;
    const T _a2;
    int _startOffset;
    RingBuffer<T> *_y2;
    T _s_1;
    T _y1_1;
    T _y1_2;

    void init(){
        _y2 = new RingBuffer<T>(_winsize + _avgWindow * 3);
        _startOffset = 0;
        _s_1 = 0;
        _y1_1 = 0;
        _y1_2 = 0;
    }
};
}

#endif  /* ZFR_H_ */
