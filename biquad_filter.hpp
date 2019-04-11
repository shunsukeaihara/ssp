#ifndef BIQUAD_FILTER_H_
#define BIQUAD_FILTER_H_

#include <common.hpp>
#include <vector>

namespace ssp {

using std::vector;

template <typename T>
inline T calcOmega(T sampleRate, T cutoff) {
    return 2.0 * M_PI * (cutoff / sampleRate);
}

template <typename T>
inline T calcAlpha(T omega, T q) {
    return sin(omega) / (2.0 * q);
}

template <class T>
class BiquadFilter {
   public:
    const T _a0;
    const T _a1;
    const T _a2;
    const T _b0;
    const T _b1;
    const T _b2;

    BiquadFilter(T a0, T a1, T a2, T b0, T b1, T b2) : _a0(a0), _a1(a1 / a0), _a2(a2 / a0), _b0(b0 / a0), _b1(b1 / a0), _b2(b2 / a0) {
        reset();
    }
    virtual ~BiquadFilter() {}

    static BiquadFilter<T> *createLowPassfilter(T fs, T cutoff, T q) {
        T omega = calcOmega(fs, cutoff);
        T alpha = calcAlpha(omega, q);
        T a0 = 1.0 + alpha;
        T a1 = -(2.0 * sin(omega));
        T a2 = 1.0 - alpha;
        T b0 = (1.0 - sin(omega)) / 2.0;
        T b1 = 1.0 - sin(omega);
        T b2 = (1.0 - sin(omega)) / 2.0;
        return new BiquadFilter<T>(a0, a1, a2, b0, b1, b2);
    }

    static BiquadFilter<T> *createHighPassfilter(T fs, T cutoff, T q) {
        T omega = calcOmega(fs, cutoff);
        T alpha = calcAlpha(omega, q);
        T a0 = 1.0 + alpha;
        T a1 = -(2.0 * cos(omega));
        T a2 = 1.0 - alpha;
        T b0 = (1.0 + cos(omega)) / 2.0;
        T b1 = -(1.0 + cos(omega));
        T b2 = (1.0 - cos(omega)) / 2.0;
        return new BiquadFilter<T>(a0, a1, a2, b0, b1, b2);
    }

    void reset() {
        _in1 = 0.0;
        _in2 = 0.0;
        _out1 = 0.0;
        _out2 = 0.0;
    }

    inline T filterOne(const T s) {
        T out = _b0 * s + _b1 * _in1 + _b2 * _in2 - _a1 * _out1 - _a2 * _out2;
        _in2 = _in1;
        _in1 = s;
        _out2 = _out1;
        _out1 = out;
        return out;
    }

    inline void filter(T *in, int len) {
        for (int i = 0; i < len; i++) {
            in[i] = filterOne(in[i]);
        }
    }

   private:
    T _in1;
    T _in2;
    T _out1;
    T _out2;
};

template <class T>
class BiquadCascade {
   public:
    BiquadCascade(vector<T> &coeffs, const int filterNum, const T overallGain) : _overallGain(overallGain) {
        for (int i = 0; i < filterNum; i++) {
            _filters.push_back(new BiquadFilter<T>(1.0, coeffs[4 * i], coeffs[4 * i + 1], 1.0, coeffs[4 * i + 2], coeffs[4 * i + 3]));
        }
    }
    virtual ~BiquadCascade() {
        for (int i = 0; i < _filters.size(); i++) {
            delete _filters[i];
        }
    }

    void reset() {
        for (int i = 0; i < _filters.size(); i++) {
            _filters[i].reset();
        }
    }

    inline T filterOne(const T s) {
        T out = _filters[0]->filterOne(s * _overallGain);
        for (int i = 1; i < _filters.size(); i++) {
            out = _filters[i]->filterOne(out);
        }
        return out;
    }

    inline void filter(T *in, int len) {
        for (int i = 0; i < len; i++) {
            in[i] = filterOne(in[i]);
        }
    }

    BiquadFilter<T> *getFilter(const int i) {
        return _filters[i];
    }

    inline T getOverallGain() {
        return _overallGain;
    }

   private:
    vector<BiquadFilter<T> *> _filters;
    const T _overallGain;
};

}  // namespace ssp

#endif /* BIQUAD_FILTER_H_ */