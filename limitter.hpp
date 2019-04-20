#ifndef LIMITTER_H_
#define LIMITTER_H_

#include <common.hpp>
#include <envelope.hpp>

#define SSP_LIMITTER_BUFFERSIZE 1024

namespace ssp {

template <class T>
class Limitter {
   public:
    Limitter(const T attackMs, const T releaseMs, const T threshDb, const T fs) : _peakHold(fs * attackMs / 1000.0), _mask(SSP_LIMITTER_BUFFERSIZE - 1), _env(EnvelopeGenerator<T>(attackMs / 5.0, releaseMs / 5.0, fs)) {
        _threshold = db2lin(threshDb);
        _buffer = new T[SSP_LIMITTER_BUFFERSIZE]();
        reset();
    }

    virtual ~Limitter() {
        delete _buffer;
    }

    void reset() {
        _peak = _threshold;
        _envelope = _threshold;
        _holdCount = 0;
        _cursor = 0;
        std::fill_n(_buffer, SSP_LIMITTER_BUFFERSIZE, 0);
    }

    inline T filterOne(const T in) {
        T gain = calcGain(fabs(in));
        unsigned int idx = (_cursor - _peakHold) & _mask;  // mod 1024
        _buffer[_cursor] = in;
        ++_cursor &= _mask;
        return _buffer[idx] * gain;
    }

    inline void filter(T *in, const int len) {
        for (int i = 0; i < len; i++) {
            in[i] = filterOne(in[i]);
        }
    }

   private:
    T _threshold;
    const int _peakHold;
    const int _mask;
    unsigned int _holdCount;
    unsigned int _cursor;

    T _peak;
    T _envelope;
    T *_buffer;
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