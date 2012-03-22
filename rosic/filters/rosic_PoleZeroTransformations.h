#ifndef rosic_PoleZeroTransformations_h
#define rosic_PoleZeroTransformations_h

// rosic-indcludes:
#include "../math/rosic_ComplexFunctions.h"

namespace rosic
{

  /**

  This file conatins conversion functions a set poles and zeros of some prototype filter in the 
  complex s-plane (or z-plane) to another set of poles and zeros of the corresponding unnormalized 
  filter. Additionaly, it also implements a mapping between the s-plane and z-plane via the 
  bilinear transform.

  */

  /** Performs an s-plane lowpass-to-lowpass transform. This transform is done in place which 
  means, the result is stored in the same arrays as the input. These arrays must be of length 
  numPoles and numZeros respectively. The target cutoff frequency is assumed to be expressed
  as radian frequency (Omega = 2*pi*frequency). ...to be deprecated. */
  void prototypeToAnalogLowpass(Complex *poles, int numPoles, Complex *zeros, 
    int numZeros, double *gain, double targetCutoff);

  void sPlanePrototypeToLowpass(Complex *prototypePoles, Complex *prototypeZeros, 
    Complex *targetPoles, Complex *targetZeros, int prototypeOrder, double targetCutoff);


  /** Performs an s-plane lowpass-to-highpass transform. This transform is done in place which 
  means, the result is stored in the same arrays as the input. These arrays must BOTH be of 
  length max(numPoles, numZeros) because we get additional finite zeros at s=0 for zeros at 
  s=inf in the prototype. ...to be deprecated. */
  void prototypeToAnalogHighpass(Complex *poles, int numPoles, Complex *zeros, 
    int numZeros, double *gain, double targetCutoff);

  void sPlanePrototypeToHighpass(Complex *prototypePoles, Complex *prototypeZeros, 
    Complex *targetPoles, Complex *targetZeros, int prototypeOrder, double targetCutoff);

  /** Same as prototypeToAnalogLowpass but additionally exchanges the roles of poles and 
  zeros. */
  void prototypeToAnalogHighShelv(Complex *poles, int numPoles, Complex *zeros, 
    int numZeros, double *gain, double targetCutoff);

  void sPlanePrototypeToHighShelv(Complex *prototypePoles, Complex *prototypeZeros, 
    Complex *targetPoles, Complex *targetZeros, int prototypeOrder, double targetCutoff);


  /** Performs an s-plane lowpass-to-bandpass transform. This transform is done in place which 
  means, the result is stored in the same arrays as the input. These arrays must BOTH be of 
  length 2*max(numPoles, numZeros) because each finite pole and zero maps to a pair of finite 
  poles and zeros and we get additional finite zeros for zeros at s=inf in the prototype. */
  void prototypeToAnalogBandpass(Complex *poles, int numPoles, Complex *zeros, int numZeros, 
    double *gain, double targetLowCutoff, double targetHighCutoff);

  void sPlanePrototypeToBandpass(Complex *prototypePoles, Complex *prototypeZeros, 
    Complex *targetPoles, Complex *targetZeros, int prototypeOrder, 
    double targetLowerCutoff, double targetUpperCutoff);


  /** Performs an s-plane lowpass-to-bandpass transform. This transform is done in place which 
  means, the result is stored in the same arrays as the input. These arrays must BOTH be of 
  length 2*max(numPoles, numZeros) because each finite pole and zero maps to a pair of finite 
  poles and zeros and we get additional finite zeros for zeros at s=inf in the prototype. */
  void prototypeToAnalogBandstop(Complex *poles, int numPoles, Complex *zeros, 
    int numZeros, double *gain, double targetLowCutoff, double targetHighCutoff);

  void sPlanePrototypeToBandreject(Complex *prototypePoles, Complex *prototypeZeros, 
    Complex *targetPoles, Complex *targetZeros, int prototypeOrder, 
    double targetLowerCutoff, double targetUpperCutoff);


  /*
  void analogToDigitalLowpassToLowpass(Complex *poles, int numPoles, 
  Complex *zeros, int numZeros, double *gain, double targetCutoff);
  */

  /** Performs a bilinear transform from the s-plane to the z-plane. */
  void bilinearAnalogToDigital(Complex *poles, int numPoles, Complex *zeros, int numZeros, 
    double sampleRate, double *gain);


  //void bilinearDigitalToAnalog(Complex *poles, int numPoles, Complex *zeros, int numZeros, 
  //  double sampleRate, double *gain);
  /**< Performs a bilinear transform from the z-plane to the s-plane. */


}  // end namespace rosic

#endif // rosic_PoleZeroTransformations_h
