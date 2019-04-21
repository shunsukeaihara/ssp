#ifndef NOISEGATE_H_
#define NOISEGATE_H_

#include <algorithm>
#include <common.hpp>
#include <envelope.hpp>

namespace ssp {

template <typename T>
class NoiseGate {
   public:
    NoiseGate(T attackMs, T releaseMs, T threshDb, T fs) : _env(EnvelopeGenerator<T>(attackMs, releaseMs, fs)) {
        _threshold = db2lin(threshDb);
    }
    virtual ~NoiseGate(){};

    inline void filter(T *in, const int len) {
        for (int i = 0; i < len; i++) {
            in[i] *= calcGain(in[i]);
        }
    }

    inline T filterOne(const T in) {
        return calcGain(fabs(in)) * in;
    }

    void reset() {
        _env.reset();
    }

   private:
    T _threshold;
    T _response;
    EnvelopeGenerator<T> _env;

    // keyは必ず絶対値を受け取る
    inline T calcGain(const T key) {
        return _env.filterOne(T(key > _threshold));
    }
};
}  // namespace ssp

#endif /* NOISEGATE_H_ */