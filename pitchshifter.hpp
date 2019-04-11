#ifndef PITCHSHIFTER_H_
#define PITCHSHIFTER_H_

#include <common.hpp>
#include <esola.hpp>
#include <resampler.hpp>
#include <ringbuffer.hpp>

namespace ssp {

template <class T>
class PitchShifter {
   public:
    PitchShifter(T shiftrate, int insize, int internalBufferSize, T fs) : _internalBufferSize(internalBufferSize) {
        _esolaBuffer = new T[internalBufferSize];
        _resampledBuffer = new T[int(internalBufferSize * shiftrate * 2.5)];
        _outputBuffer = new RingBuffer<T>(internalBufferSize * 11);
        _resampler = new Resampler<T>(fs * shiftrate, fs, insize);
        _esola = new ESOLA<T>(shiftrate, fs * 0.02, insize, fs);
    }
    virtual ~PitchShifter() {
        delete _esola;
        delete _resampler;
        delete _esolaBuffer;
        delete _resampledBuffer;
        delete _outputBuffer;
    }

    void process(T *in, int len) {
        _esola->process(in, len);
        while (_esola->read(_esolaBuffer, _internalBufferSize) > 0) {
            int resampled = _resampler->process(_esolaBuffer, _internalBufferSize, _resampledBuffer);
            _outputBuffer->write(_resampledBuffer, resampled);
        }
    }

    int read(T *out, int len) {
        if (!_outputBuffer->filled(len)) return -1;
        return _outputBuffer->read(out, len);
    }

   private:
    ESOLA<T> *_esola;
    Resampler<T> *_resampler;
    RingBuffer<T> *_outputBuffer;
    T *_esolaBuffer;
    T *_resampledBuffer;
    int _internalBufferSize;
};
}  // namespace ssp

#endif /* PITCHSHIFTER_H_ */