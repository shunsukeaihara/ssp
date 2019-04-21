#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <butterworth_filter.hpp>
#include <common.hpp>

namespace ssp {

template <typename T>
class Resampler {
   public:
    Resampler(const int fromFs, const int toFs, const int capacity) : _fromFs(fromFs), _toFs(toFs) {
        int gcd = calcGcd(fromFs, toFs);
        _fromMul = toFs / gcd;
        _toDiv = fromFs / gcd;
        int bufferSize = capacity * _fromMul;
        int outBufferSize = bufferSize * _toDiv;
        _buffer = new T[bufferSize]();
        _outbuffer = new T[outBufferSize]();
        _filter = ButterworthFilter<T>::createLowPassFilter(fromFs * _fromMul, nyquistFromTwo<T>(fromFs, toFs), 8);
    }

    virtual ~Resampler() {
        delete _buffer;
        delete _outbuffer;
        delete _filter;
    }

    int process(const T *in, const int len, T *out) {
        int bufferLen = upsampling(in, len);
        return downsampling(bufferLen, out);
    }

    int getGetFromMul() {
        return _fromMul;
    }

    int getGetToDiv() {
        return _toDiv;
    }

   private:
    T *_buffer;
    T *_outbuffer;
    const int _fromFs;
    const int _toFs;
    int _fromMul;
    int _toDiv;
    ButterworthFilter<T> *_filter;

    inline int upsampling(const T *in, const int len) {
        if (_fromMul > 1) {
            int j, k;
            for (int i = 0; i < len; i++) {
                j = i * _fromMul;
                _buffer[j] = in[i] * _fromMul;
                for (k = 1; k < _fromMul; k++) {
                    _buffer[j + k] = 0.0;
                }
            }
            _filter->filter(_buffer, len * _fromMul);
        } else {
            memcpy(_buffer, in, sizeof(T) * len);
        }
        return len * _fromMul;
    }

    inline int downsampling(const int bufferLen, T *out) {
        if (_toDiv > 1) {
            int j;
            for (int i = 0; i < bufferLen; i += _toDiv) {
                j = i / _toDiv;
                out[j] = _buffer[i];
            }
            return bufferLen / _toDiv;
        } else {
            memcpy(out, _buffer, sizeof(T) * bufferLen);
            return bufferLen;
        }
    }
};
}  // namespace ssp

#endif /* RESAMPLER_H_ */