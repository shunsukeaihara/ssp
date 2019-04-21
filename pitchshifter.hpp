#ifndef PITCHSHIFTER_H_
#define PITCHSHIFTER_H_

#include <common.hpp>
#include <esola.hpp>
#include <resampler.hpp>
#include <ringbuffer.hpp>
#include <zfr_epoch_detector.hpp>

namespace ssp {

template <typename T>
class PitchShifter {
   public:
    PitchShifter(const T shiftrate, const T zfrWinMs, const T esolaWinMs, const int insize, const int internalBufferSize, const T fs) : _internalBufferSize(internalBufferSize) {
        _zfrBuffer = new T[insize];
        _esolaBuffer = new T[internalBufferSize];
        _resampledBuffer = new T[int(internalBufferSize * shiftrate * 2.5)];
        _outputBuffer = new RingBuffer<T>(internalBufferSize * 11);
        _resampler = new Resampler<T>(fs * shiftrate, fs, insize);
        _zfr = new ZFREpochDetector<T>(zfrWinMs, insize, -2, 1, fs);
        _esola = new ESOLA<T>(shiftrate, esolaWinMs, insize, fs);
    }
    virtual ~PitchShifter() {
        delete _zfr;
        delete _esola;
        delete _resampler;
        delete _esolaBuffer;
        delete _zfrBuffer;
        delete _resampledBuffer;
        delete _outputBuffer;
    }

    void process(const T *in, const int len) {
        size_t zfrOut = _zfr->process(in, len, _zfrBuffer);
        _esola->process(_zfrBuffer, zfrOut);
        while (_esola->read(_esolaBuffer, _internalBufferSize) > 0) {
            int resampled = _resampler->process(_esolaBuffer, _internalBufferSize, _resampledBuffer);
            _outputBuffer->write(_resampledBuffer, resampled);
        }
    }

    int read(T *out, const int len) {
        if (!_outputBuffer->filled(len)) return -1;
        return _outputBuffer->read(out, len);
    }

   private:
    ESOLA<T> *_esola;
    ZFREpochDetector<T> *_zfr;
    Resampler<T> *_resampler;
    RingBuffer<T> *_outputBuffer;
    T *_zfrBuffer;
    T *_esolaBuffer;
    T *_resampledBuffer;
    int _internalBufferSize;
};
}  // namespace ssp

#endif /* PITCHSHIFTER_H_ */