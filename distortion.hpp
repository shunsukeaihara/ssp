#ifndef DISTORTION_H_
#define DISTORTION_H_

#include <common.hpp>

namespace ssp {

template <typename T>
class Distortion {
   public:
    Distortion(const T amount) {
        setAmount(amount);
    }
    virtual ~Distortion() {}

    inline T filterOne(const T in) const {
        return waveShaper(in);
    }

    inline void filter(T *in, const int len) const {
        for (int i = 0; i < len; i++) {
            x[i] = filterOne(x[i]);
        }
    }

    inline void setAmount(const T amount) {
        _amount = clip(amount, -0.999, 0.997);
        _k = 2 * _amount / (1.1 - _amount);
    }

   private:
    T _amount;
    T _k;

    // https://github.com/meta-meta/MaxPatches/blob/master/_3rdParty/stkr.waveshaping/absAndGenExprCode/stkr.waveshaping.genexpr

    inline T waveShaper(const T x) {
        // simple distortion
        return (1 + _k) * x / (1 + _k * fabs(x));
    }
};

}  // namespace ssp

#endif /* DISTORTION_H_ */