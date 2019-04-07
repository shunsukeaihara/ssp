#include <stdio.h>
#include <biquad_filter.hpp>
#include <butterworth_filter.hpp>
#include <vector>
#include <iostream>

#define EPSILON 1.0e-14
#define EPSILON_COEFF 1.0e-4

using std::complex;
using std::vector;
using namespace ssp;

void calcPolesTest(){
    vector<complex<double> > poles;
    calcPoles(poles, 8);
    vector<complex<double> > base(8);
    base[0] = complex<double>(-0.19509032201613, 0.98078528040323);
	base[1] = complex<double>(-0.19509032201613, -0.98078528040323);
	base[2] = complex<double>(-0.55557023301960, 0.83146961230255);
	base[3] = complex<double>(-0.55557023301960, -0.83146961230255);
	base[4] = complex<double>(-0.83146961230255, 0.55557023301960);
	base[5] = complex<double>(-0.83146961230255, -0.55557023301960);
	base[6] = complex<double>(-0.98078528040323, 0.19509032201613);
	base[7] = complex<double>(-0.98078528040323, -0.19509032201613);
    for (int i=0; i<8; i++) {
        if (poles[i].real() - base[i].real() > EPSILON){
            std::cout << "error" << std::endl;
        }
        if (poles[i].imag() - base[i].imag() > EPSILON){
            std::cout << "error" << std::endl;
        }
    }
}

void checkBiquadFilterCoeffs(BiquadCascade<double> *bqc, double a0, double a1, double a2, double b0, double b1, double b2, int i){
    BiquadFilter<double> *f = bqc->getFilter(i);
    if (abs(a0 - f->_a0) > EPSILON_COEFF){
        std::cout << "error" << std::endl;
    }
    if (abs(a1 - f->_a1) > EPSILON_COEFF){
        std::cout << "error" << std::endl;
    }
    if (abs(a2 - f->_a2) > EPSILON_COEFF){
        std::cout << "error" << std::endl;
    }
    if (abs(b0 - f->_b0) > EPSILON_COEFF){
        std::cout << "error" << std::endl;
    }
    if (abs(b1 - f->_b1) > EPSILON_COEFF){
        std::cout << "error" << std::endl;
    }
    if (abs(b2 - f->_b2) > EPSILON_COEFF){
        std::cout << "error" << std::endl;
    }
}

void lowpassButterworthFilterTest(){
    ButterworthFilter<double> *bwf = ButterworthFilter<double>::createLowPassFilter(44100.0, 500, 8);
    checkBiquadFilterCoeffs(bwf->getCascade(), 1.0, -1.96762058043629, 0.97261960500367, 1.0, 2.0, 1.0, 0);
    checkBiquadFilterCoeffs(bwf->getCascade(), 1.0, -1.91907529383978, 0.92395098208778, 1.0, 2.0, 1.0, 1);
    checkBiquadFilterCoeffs(bwf->getCascade(), 1.0, -1.88350864178159, 0.88829396780773, 1.0, 2.0, 1.0, 2);
    checkBiquadFilterCoeffs(bwf->getCascade(), 1.0, -1.86480445083537, 0.86954225616013, 1.0, 2.0, 1.0, 3);
    delete bwf;
}

int main(){
    calcPolesTest();
    lowpassButterworthFilterTest();
    return 0;
}