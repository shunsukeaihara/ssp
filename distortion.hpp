#ifndef DISTORTION_H_
#define DISTORTION_H_

#include <common.hpp>

namespace ssp {

template <class T>
class Distortion {
   public:
    Distortion(const T amount) {
        _amount = amount;
        _k = 2 * amount / (1.1 - amount);
    }
    virtual ~Distortion() {}

    inline T filterOne(const T in) const {
        return waveShaper(in, _amount);
    }

    inline void filter(T *in, const int len) const {
        for (int i = 0; i < len; i++) {
            x[i] = filterOne(x[i]);
        }
    }

    inline void setAmount(const T amount) {
        if (amount <= 0) {
            _amount = 0.0;
        } else {
            _amount = amount;
        }
        _k = 2 * amount / (1.1 - amount);
    }

   private:
    T _amount;
    T _k;

    inline T waveShaper(const T x) {
        // simple distortion
        return (1 + _k) * x / (1 + _k * fabs(x));
    }

    inline T watteShaper(const T in) {
        // simple overdrive
        T x = tanh(in);
        x = clip(x * _amount, -1, 1);
        return 1.5 * x - 0.5 * x * x * x;
    }
};

}  // namespace ssp

#endif /* DISTORTION_H_ */