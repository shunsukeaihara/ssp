#ifndef ESOLA_H_
#define ESOLA_H_

#include <common.hpp>
#include <zfr_epoch_detector.hpp>
#include <window.hpp>

namespace ssp{


template <typename T> int void findFirstEpoch(const RingBuffer<T> in, int offset, size_t maxIndex){
    for (int i=offset;i<maxIndex;i++){
        if (isEpochMark(in[i])){return i}
    }
    return -1;
}

template<class T>
class ESOLA{
public:
    ESOLA(T fs, T tsr, size_t frameSize, size_t inputsize): _frameSize(frameSize){
        _zfr = new ZFREpochDetector<T>(size_t(fs / 0.01, winsize, -2, 2);
        _synthesisBuffer = new RingBuffer<T>(winsize * tsr *2.5);
        _analysisBuffer = new RingBuffer<T>(winsize * 2.5);
        _epochBuffer = new T[inputsize];
        _a = 1.0 / tsr;
        _ss = framesize / 2; // ピッチだけ変えたい場合はここの詰め方を変える必要があるが、一応オリジナルのtimescale変更のみのモノでやる
        _sa = round(_ss/a);
        _l = _frameSize - ss;
        _window = new T[_l * 2];
        hammingWindow(_window, _l * 2);
    }

    virtual ~ESOLA(){
        delete _zfr;
        delete _synthesisBuffer;
        delete _ _analysisBuffer;
        delete _epochBuffer;
        delete _window;
    }

    inline size_t process(T *in, size_t len, T *out){
        // _epochbufferにepoch mark付与済みの信号が書き込まれる。初回はlenよりも若干少ないデータ数が処理される
        size_t outsize = _zfr->process(in, len, _epochBuffer);
        _analysisBuffer.write(_epochBuffer, outsize);
        // count epoch size
        while(True){
            // analysis frameが最低限取得出来るだけの容量が無いと終了
            if (!_analysisBuffer.filledFromCurrentPos(_frameSize))break;
            int k = calcK();
            // offsetした上で、frame分の容量が無いと終了
            if (!_analysisBuffer.filledFromCurrentPos(_frameSize + k))break;
            for (int i=0;i<_frameSize;i++){
                if (i < l){
                    bool epoch = false;
                    if (isEpoch(_synthesisBuffer[i]) || isEpochMark(_analysisBuffer[i+k])){
                        // 半分シフトではなく詰める場合にはピッチマークをゴリゴリ入れておくほうが良いけれど、半分シフトならこれ考えなくて良い
                        epoch = true;
                    }
                    T fade = _window[i];
                    _synthesisBuffer[i] = _synthesisBuffer[i] * fade + _analysisBuffer[i] * (1.0 - fade);
                    if (epoch){
                        setEpochMark(_synthesisBuffer[i]);
                    }else{
                        unsetEpochMark(_synthesisBuffer[i]);
                    }
                }else{
                    _synthesisBuffer[i] = _analysisBuffer[i+k];
                }
            }
            _synthesisBuffer.movePos(_ss);
            _synthesisBuffer.moveWritePos(_ss);
            _analysisBuffer.movePos(_sa);
        }
        // output synthesized frame;
        // _analysis bufferのreadPosからwritePosまでに合成済みの値が入っている
    }

    int read(T *out, size_t len){
        if (!_synthesisBuffer.filled(len)){
            return -1;
        }
    }

private:
    const size_t _frameSize;
    ZFREpochDetector<T> *_zfr;
    RingBuffer<T> *_analysisBuffer;
    RingBuffer<T> *_synthesisBuffer;
    T * _window;
    T *_epochBuffer;
    T _a;
    int _ss;
    int _sa;
    int _l;

    int calcK(){
        int l = findFirstEpoch(_synthesisBuffer, 0, _ss); // synthesys frameは_ss分だけ先に書き込まれている
        int n = findFirstEpoch(_analysisBuffer, l, _frameSize);
        return n - l;
    }
};
}

#endif /* ESOLA_H_ */