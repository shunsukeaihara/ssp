#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#include <common.hpp>

namespace ssp {

template <typename T>
inline T calcSinglePoleFactor(T tsInMs, T fs) {
    return exp((-1.0 / fs) / (tsInMs / 1000.0));
}

template <typename T>
inline T singlePoleLPF(T x, T y_1, T factor) {
    return x + factor * (y_1 - x);  // (1.0 - factor) * x + factor * y_1
}

template <class T>
class EnvelopeGenerator {
   public:
    EnvelopeGenerator(T attackMs, T releaseMs, T fs) : _attFactor(calcSinglePoleFactor(attackMs, fs)), calcSinglePoleFactor(calcStepResponse(releaseMs, fs)) {
        _y_1 = SSP_DC_OFFSET;
    }
    virtual ~EnvelopeGenerator() {}

    inline T filterOne(T x) {
        x = x + SSP_DC_OFFSET;
        if (x > y_1) {
            _y_1 = singlePoleLPF(x, _y_1, _attFactor);
        } else {
            _y_1 = singlePoleLPF(x, _y_1, _relFactor);
        }
        return _y_1 - SSP_DC_OFFSET;
    }

   private:
    const T _attFactor;
    const T _relFactor;
    T _y_1;
};
}  // namespace ssp

#endif /* ENVELOPE_H_ */