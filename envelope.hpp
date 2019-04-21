#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#include <common.hpp>

namespace ssp {

// 遅れ時間msの一次IIRフィルタ係数を求める
template <typename T>
inline T calcSinglePoleFactorFromTC(const T tcInMs, const T fs) {
    return exp((-1.0 / fs) / (tcInMs / 1000.0));
}

// ステップ応答の99%点がtcのIIRフィルタ係数を求める
template <typename T>
inline T calcSinglePoleFactor(const T tcInMs, const T fs) {
    return pow(0.01, ((-1.0 / fs) / (tcInMs / 1000.0)));
}

template <typename T>
inline T singlePoleLPF(const T x, const T y_1, const T factor) {
    return x + factor * (y_1 - x);  // (1.0 - factor) * x + factor * y_1
}

template <typename T>
class EnvelopeGenerator {
   public:
    EnvelopeGenerator() {
        _y_1 = SSP_DC_OFFSET;
    };
    EnvelopeGenerator(const T attMs, const T relMs, const T fs) {
        _y_1 = SSP_DC_OFFSET;
        _relFactor = calcSinglePoleFactorFromTC(relMs, fs);
        _attFactor = calcSinglePoleFactorFromTC(attMs, fs);
    }
    virtual ~EnvelopeGenerator() {}

    inline T filterOne(const T in) {
        T x = in + SSP_DC_OFFSET;
        if (x > _y_1) {
            _y_1 = singlePoleLPF(x, _y_1, _attFactor);
        } else {
            _y_1 = singlePoleLPF(x, _y_1, _relFactor);
        }
        return _y_1 - SSP_DC_OFFSET;
    }

    void reset() {
        _y_1 = SSP_DC_OFFSET;
    }

   protected:
    T _attFactor;
    T _relFactor;
    T _y_1;
};
}  // namespace ssp

#endif /* ENVELOPE_H_ */