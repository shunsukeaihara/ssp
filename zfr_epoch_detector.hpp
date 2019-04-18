#ifndef ZFREPOCHDETECDTOR_H_
#define ZFREPOCHDETECDTOR_H_

#include <common.hpp>
#include <iostream>
#include <ringbuffer.hpp>

namespace ssp {

template <class T>
class ZFREpochDetector {
   public:
    ZFREpochDetector(const T winMs, const int insize, const T a1, const T a2, const T fs) : _avgWindow(fs * winMs / 1000.0), _insize(insize), _a1(a1), _a2(a2), _fs(fs), _s(RingBuffer<T>(insize + _avgWindow * 2)), _y2(RingBuffer<T>(insize + _avgWindow * 2)) {
        _startOffset = 0;
        _y1_1 = 0;
        _y1_2 = 0;
        _s_1 = 0;
    }

    virtual ~ZFREpochDetector() {}

    inline size_t process(const T *s, int const len, T *out) {
        // epochの判定ができた分の入力音声をepoch mark付けてoutに書き込み。書き込んだ分をreturn
        if (len <= 0) {
            return 0;
        }
        for (int i = 0; i < len; i++) {
            _s[i] = s[i];
            // preprocessed to remove the lowfrequency bias
            T x = s[i] - _s_1;
            _s_1 = s[i];
            // firset zero frequency resonator
            T _y1 = -(_a1 * _y1_1 + x + _a2 * _y1_2 + x);
            //  zero frequency resonator
            _y2[i] = -(_a1 * _y2[i - 1] + _y1 + _a2 * _y2[i - 2] + _y1);
            _y1_2 = _y1_1;
            _y1_1 = _y1;
        }
        size_t processed = 0;
        T last = 0;
        for (int i = -_startOffset; i < (len - _avgWindow); i++) {
            T sum = 0;
            for (int j = -_avgWindow; j <= _avgWindow; j++) {
                sum += _y2[i + j];
            }
            T winAvg = _y2[i] - sum / (float)(_avgWindow * 2 + 1);
            out[i + _startOffset] = _s[i];
            // positive zero cross pointの判定
            if (last * winAvg < 0 && winAvg > last) {
                // 前回との積が負で、新しい値が正の時epoch mark -> LSBを1にする
                // little endian前提のコードなのでbig endian系の場合はまた考慮する必要がある
                setEpochMark(&out[i + _startOffset]);
            } else {
                //LSBを0にする
                unsetEpochMark(&out[i + _startOffset]);
            }
            last = winAvg;
            processed += 1;
        }
        _startOffset = _avgWindow;
        _y2.movePos(len);
        _s.movePos(len);
        return processed;
    }

    void reset() {
        _s.reset();
        _y2.reset();
        _startOffset = 0;
        _s_1 = 0;
        _y1_1 = 0;
        _y1_2 = 0;
    }

   private:
    const int _avgWindow;
    const int _insize;
    const T _a1;
    const T _a2;
    const T _fs;
    RingBuffer<T> _s;
    RingBuffer<T> _y2;
    int _startOffset;
    T _s_1;
    T _y1_1;
    T _y1_2;
};
}  // namespace ssp

#endif /* ZFREPOCHDETECDTOR_H_ */
