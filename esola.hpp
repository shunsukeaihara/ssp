#ifndef ESOLA_H_
#define ESOLA_H_

#include <common.hpp>
#include <iostream>
#include <ringbuffer.hpp>
#include <window.hpp>

namespace ssp {

template <typename T>
int findFirstEpoch(const RingBuffer<T> &in, const int offset, const int maxIndex) {
    for (int i = offset; i < maxIndex; i++) {
        if (isEpochMark(in[i])) return i;
    }
    return -1;
}

template <typename T>
class ESOLA {
   public:
    ESOLA(T tsr, T winMs, int inputsize, T fs) : _frameSize(fs * winMs / 1000.0), _fs(fs), _analysisBuffer(inputsize * 2.5), _synthesisBuffer(RingBuffer<T>(inputsize * tsr * 2.5)) {
        _a = tsr;
        _ss = _frameSize / 2;
        _sa = round(_ss / _a);
        _l = _frameSize - _ss;
        _window = new T[_l * 2];
        hammingWindow(_window, _l * 2);
    }

    virtual ~ESOLA() {
        delete _window;
    }

    void process(const T *in, const int len) {
        if (len <= 0) return;
        // _epochbufferにepoch mark付与済みの信号が書き込まれる。初回はlenよりも若干少ないデータ数が処理される
        _analysisBuffer.write(in, len);
        // count epoch size
        while (true) {
            // analysis frameが最低限取得出来るだけの容量が無いと終了
            if (!_analysisBuffer.filledFromCurrentPos(_frameSize)) break;
            int k = calcK();
            // offsetした上で、frame分の容量が無いと終了
            if (!_analysisBuffer.filledFromCurrentPos(_frameSize + k)) break;
            for (int i = 0; i < _frameSize; i++) {
                if (i < _l) {
                    bool epoch = false;
                    if (isEpochMark(_synthesisBuffer[i]) || isEpochMark(_analysisBuffer[i + k])) {
                        epoch = true;
                    }
                    T fade = _window[i];
                    _synthesisBuffer[i] = _synthesisBuffer[i] * (1.0 - fade) + _analysisBuffer[i + k] * fade;
                    if (epoch) {
                        setEpochMark(_synthesisBuffer.at(i));
                    } else {
                        unsetEpochMark(_synthesisBuffer.at(i));
                    }
                } else {
                    _synthesisBuffer[i] = _analysisBuffer[i + k];
                }
            }
            _synthesisBuffer.movePos(_ss);
            _synthesisBuffer.moveWritePos(_ss);
            _analysisBuffer.movePos(_sa);
        }
    }

    int read(T *out, const int len) {
        if (!_synthesisBuffer.filled(len)) {
            return -1;
        }
        return _synthesisBuffer.read(out, len);
    }

   private:
    const int _frameSize;
    const T _fs;
    RingBuffer<T> _analysisBuffer;
    RingBuffer<T> _synthesisBuffer;
    T *_window;
    T _a;
    int _ss;
    int _sa;
    int _l;

    int calcK() const {
        int l = findFirstEpoch(_synthesisBuffer, 0, _l);
        if (l < 0) return 0;
        int n = findFirstEpoch(_analysisBuffer, l, _frameSize);
        if (n < 0) return 0;
        return n - l;
    }
};
}  // namespace ssp

#endif /* ESOLA_H_ */