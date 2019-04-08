#ifndef ESOLA_H_
#define ESOLA_H_

#include <common.hpp>
#include <zfr_epoch_detector.hpp>
#include <window.hpp>

namespace ssp{


template <typename T> int findFirstEpoch(const RingBuffer<T> *in, int offset, int maxIndex){
    for (int i=offset;i<maxIndex;i++){
        if (isEpochMark((*in)[i])){return i;}
    }
    return -1;
}

template<class T>
class ESOLA{
public:
    ESOLA(T fs, T tsr, int frameSize, int inputsize): _frameSize(frameSize), _fs(fs){
        _zfr = new ZFREpochDetector<T>(int(fs * 0.005), inputsize, -2, 1);
        _synthesisBuffer = new RingBuffer<T>(inputsize * tsr *11);
        _analysisBuffer = new RingBuffer<T>(inputsize * 5);
        _epochBuffer = new T[inputsize*2];
        _a = tsr;
        _ss = frameSize / 2;
        _sa = round(_ss/_a);
        _l = _frameSize - _ss;
        _window = new T[_l * 2];
        hammingWindow(_window, _l * 2);
    }

    virtual ~ESOLA(){
        delete _zfr;
        delete _synthesisBuffer;
        delete _analysisBuffer;
        delete _epochBuffer;
        delete _window;
    }

    void process(const T *in, const int len){
        if (len<=0){return;}
        // _epochbufferにepoch mark付与済みの信号が書き込まれる。初回はlenよりも若干少ないデータ数が処理される
        int outsize = _zfr->process(in, len, _epochBuffer);
        _analysisBuffer->write(_epochBuffer, outsize);
        // count epoch size
        while(true){
            // analysis frameが最低限取得出来るだけの容量が無いと終了
            if (!_analysisBuffer->filledFromCurrentPos(_frameSize))break;
            int k = calcK();
            // offsetした上で、frame分の容量が無いと終了
            if (!_analysisBuffer->filledFromCurrentPos(_frameSize + k))break;
            for (int i=0;i<_frameSize;i++){
                if (i < _l){
                    bool epoch = false;
                    if (isEpochMark((*_synthesisBuffer)[i]) || isEpochMark((*_analysisBuffer)[i+k])){
                        epoch = true;
                    }
                    T fade = _window[i];
                    (*_synthesisBuffer)[i] = (*_synthesisBuffer)[i] * (1.0 - fade) + (*_analysisBuffer)[i+k] * fade;
                    if (epoch){
                        setEpochMark(_synthesisBuffer->at(i));
                    }else{
                        unsetEpochMark(_synthesisBuffer->at(i));
                    }
                }else{
                    (*_synthesisBuffer)[i] = (*_analysisBuffer)[i+k];
                }
            }
            _synthesisBuffer->movePos(_ss);
            _synthesisBuffer->moveWritePos(_ss);
            _analysisBuffer->movePos(_sa);
        }
    }

    int read(T *out, const int len){
        if (!_synthesisBuffer->filled(len)){
            return -1;
        }
        return _synthesisBuffer->read(out, len);
    }

private:
    const int _frameSize;
    ZFREpochDetector<T> *_zfr;
    RingBuffer<T> *_analysisBuffer;
    RingBuffer<T> *_synthesisBuffer;
    T * _window;
    T *_epochBuffer;
    T _a;
    int _ss;
    int _sa;
    int _l;
    T _fs;

    int calcK() const{
        int l = findFirstEpoch(_synthesisBuffer, 0, _frameSize); // synthesys frameは_ss分だけ先に書き込まれている
        if (l < 0)return 0;
        int n = findFirstEpoch(_analysisBuffer, l, _frameSize);
        if (n < 0)return 0;
        return n - l;
    }
};
}

#endif /* ESOLA_H_ */