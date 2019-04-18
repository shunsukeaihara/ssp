#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

#include <common.hpp>
#include <envelope.hpp>

namespace ssp {
template <class T>
class Compressor {
   public:
    Compressor(const T attackMs, const T releaseMs, const T threshDb, const T ratio, const T fs) : _env(EnvelopeGenerator<T>(attackMs, releaseMs, fs)) {
        _threshold = db2lin(threshDb);
        _thresholdDb = threshDb;
        _ratio = ratio;
        _envelopeDb = SSP_DC_OFFSET;
    }
    virtual ~Compressor() {}

    inline T filterOne(const T in) {
        return calcGain(fabs(in)) * in;
    }

    // keyは絶対値
    inline T filterOne(const T in, const T key) {
        return calcGain(key) * in;
    }

   private:
    T _ratio;
    T _thresholdDb;
    T _threshold;
    T _envelopeDb;
    EnvelopeGenerator<T> _env;

    inline T calcGain(const T key) {
        T keydb = lin2dB(key + SSP_DC_OFFSET);
        T over = keydb - _thresholdDb;
        if (over < 0.0) over = 0.0;
        _envelopeDb = _env.filterOne(over);
        return db2lin(_envelopeDb * (_ratio - 1.0));
    }
};

}  // namespace ssp

#endif /* COMPRESSOR_H_ */