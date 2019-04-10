#ifndef NOISEGATE_H_
#define NOISEGATE_H_

#include <algorithm>
#include <common.hpp>
#include <envelope.hpp>

namespace ssp {

template <class T>
class NoiseGate {
   public:
    NoiseGate(T attackMs, T releaseMs, T threshDb, T fs) : _env(EnvelopeGenerator(attackMs, releaseMs, fs)) {
        _threashold = db2lin(threshDb);
    }
    virtual ~NoiseGate(){};

    inline void filterStereo(T *in, int len) {
        for (int i = 0; i < len * 2; i += 2) {
            filterOneStereo(in[i], in[i + 1]);
        }
    }

    inline void filter(T *in, int len) {
        for (int i = 0; i < len; i++) {
            in[i] *= filterOne(in[i]);
        }
    }

    inline T filterOne(T in) {
        return calcGain(fabs(in));
    }

    inline void filterOneStereo(T &in1, T &in2) {
        T gain = calcGain(std::max(fabs(in1), fabs(in2)));
        in1 *= gain;
        in2 *= gain;
    }

   private:
    T _threshold;
    T _response;
    EnvelopeGenerator _env;

    // keyは必ず絶対値を受け取る
    inline T calcGain(T key) {
        return _env.filterOne(T(key > _threshold));
    }
};
}  // namespace ssp

#endif /* NOISEGATE_H_ */