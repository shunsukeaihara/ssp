#ifndef SSP_DELAY_H_
#define SSP_DELAY_H_

#include <common.hpp>
#include <iostream>
#include <ringbuffer.hpp>

namespace ssp {

template <class T>
class Delay {
   public:
    Delay(const T delayMs, const int delayCount, const T decay, const T fs) : _buffer(int((delayMs / 1000.0 * fs) * delayCount + 1)) {
        _delayCount = delayCount;
        _delaySamples = int(delayMs / 1000.0 * fs);
        _decay = decay;
    }
    virtual ~Delay() {}

    inline T filterOne(const T in) {
        T x = in;
        _buffer.push(in);
        for (int i = 1; i <= _delayCount; i++) {
            x += _buffer[-(_delaySamples * i)] * pow(_decay, i);
        }
        return x;
    }

    inline void filter(T *in, const int len) {
        for (int i = 0; i < len; i++) {
            in[i] = filterOne(in[i]);
        }
    }

   private:
    int _delaySamples;
    int _delayCount;
    T _decay;
    RingBuffer<T> _buffer;
};
}  // namespace ssp

#endif /* SSP_DELAY_H_ */