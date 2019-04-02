#ifndef BUTTERWORTH_FILTER_H_
#define BUTTERWORTH_FILTER_H_

#include <biquad_filter.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <complex>

namespace ssp{

using std::complex;
using std::vector;

template <typename T> inline void calcPoles(vector<complex<T> > & poles, const size_t order){
    for (int i=0;i<(order+1)/2;i++) {
        T theta = (2.0*i + 1.0) * M_PI / (2.0 * order);
        T real = -sin(theta);
        T imag = cos(theta);
        poles.push_back(complex<T>(real, imag));
        poles.push_back(complex<T>(real, -imag));
    }
}

template <typename T> inline T bilinearTransform(complex<T> & sz){
    complex<T> two = complex<T>(2.0, 0);
    complex<T> s = sz;
    sz = (two + s) / (two - s);
    return std::abs(complex<T>(two-s));
}

template <typename T> inline T splane2zplane(vector<complex<T> > & zeros,
                                             vector<complex<T> > & poles, const T gain){
    T ret = gain;
    for (int i=0;i<zeros.size();i++) {
        ret/=bilinearTransform(zeros[i]);
    }
    for (int i=0;i<poles.size();i++) {
        ret*=bilinearTransform(poles[i]);
    }
    return ret;
}

template <typename T> size_t zplane2SecondOrderSection(const vector<complex<T> > zeros,
                                                       const vector<complex<T> > poles, const size_t filterNum,
                                                       vector<T> &coeffs){
    vector<complex<T> >zerosTemp(zeros);
    zerosTemp.resize(filterNum);
    vector<complex<T> >polesTemp(poles);
    polesTemp.resize(filterNum);
    for (int i=zeros.size();i<filterNum;i++) {
		zerosTemp[i] = complex<T>(-1, 0);
	}
    size_t nsos = 0;
    for(int i=0;i<filterNum;i+=2){
        coeffs.push_back(-(polesTemp[i] + polesTemp[i+1]).real());
        coeffs.push_back((polesTemp[i] * polesTemp[i+1]).real());
        coeffs.push_back(-(zerosTemp[i] + zerosTemp[i+1]).real());
        coeffs.push_back((zerosTemp[i] * zerosTemp[i+1]).real());
        nsos++;
    }

    if (filterNum%2 == 1) {
        coeffs.push_back(-polesTemp[filterNum-1].real());
        coeffs.push_back(0);
        coeffs.push_back(-zerosTemp[filterNum-1].real());
        coeffs.push_back(0);
        nsos++;
    }
    return nsos;
}


template<class T>
class ButterworthFilter{
public:
    ButterworthFilter(vector<T> & coeffs, const size_t filterNum, const T overallGain){
        _cascade = new BiquadCascade<T>(coeffs, filterNum, overallGain);
    }
    virtual ~ButterworthFilter(){
        delete(_cascade);
    }

    static ButterworthFilter<T> *createLowPassFilter(T sampleRate, T cutoff, size_t order){
        T wrapedCutoff = (2.0 * tan(M_PI*cutoff/sampleRate));
        vector<complex<T> > zeros;
        vector<complex<T> > poles;
        vector<T> coeffs;
        calcPoles(poles, order);
        size_t numPoles = poles.size();
        T gain = 1.0;
        gain *= pow(wrapedCutoff, numPoles);
        T preGain = gain;
        for(int i=0;i<numPoles;i++){
            poles[i] *= complex<T>(wrapedCutoff, 0);
        }
        gain = splane2zplane(zeros, poles, gain);
        T overallGain = preGain * (preGain / gain);
        zplane2SecondOrderSection(zeros, poles, numPoles, coeffs);
        return new ButterworthFilter<T>(coeffs, order/2, overallGain);
    }

    BiquadCascade<T>* getCascade(){
        return _cascade;
    }

private:
    BiquadCascade<T> *_cascade;
};
}

#endif /* BUTTERWORTH_FILTER_H_ */