#ifndef LIMITTER_H_
#define LIMITTER_H_

#include <common.hpp>
#include <envelope.hpp>
#include <ringbuffer.hpp>

namespace ssp {

template <typename T>
class Limitter {
   public:
    Limitter(const T attackMs, const T releaseMs, const T threshDb, const T fs) : _peakHold(fs * attackMs / 1000.0), _buffer(RingBuffer<T>(_peakHold + 1)), _env(EnvelopeGenerator<T>(attackMs / 5.0, releaseMs / 5.0, fs)) {
        _threshold = db2lin(threshDb);
        reset();
    }

    virtual ~Limitter() {
    }

    void reset() {
        _peak = _threshold;
        _envelope = _threshold;
        _holdCount = 0;
        _buffer.reset();
    }

    inline T filterOne(const T in) {
        _buffer.push(in);
        T gain = calcGain(fabs(in));
        return _buffer[-_peakHold] * gain;
    }

    inline void filter(T *in, const int len) {
        for (int i = 0; i < len; i++) {
            in[i] = filterOne(in[i]);
        }
    }

   private:
    const int _peakHold;
    RingBuffer<T> _buffer;
    unsigned int _holdCount;
    T _threshold;
    T _peak;
    T _envelope;
    EnvelopeGenerator<T> _env;

    // keyは絶対値
    inline T calcGain(T key) {
        if (key < _threshold) key = _threshold;
        if ((++_holdCount >= _peakHold) || (key > _peak)) {
            _holdCount = 0;
            _peak = key;
        }
        _envelope = _env.filterOne(_peak);
        return _threshold / _envelope;
    }
};

}  // namespace ssp

#endif /* LIMITTER_H_ */