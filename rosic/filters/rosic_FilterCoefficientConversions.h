#ifndef rosic_FilterCoefficientConversions_h
#define rosic_FilterCoefficientConversions_h

// rosic-indcludes:
#include "../math/rosic_ComplexFunctions.h"

namespace rosic
{

  /**

  This file contains functions to convert between the coefficients for various filter realization
  structures.

  */

  /** Converts direct form FIR coefficients to FIR lattice filter reflection coefficients. */
  void directFormToLatticeFir(double *directFormCoeffs, int order, double *reflectionCoeffs);

  /** Converts FIR lattice filter reflection coefficients to direct form FIR coefficients. */
  void latticeToDirectFormFir(double *reflectionCoeffs, int order, double *directFormCoeffs);

  /** Converts complex poles and zeros into coefficients for a biquad cascade in which each stage
  implements the difference equation:
  \f[ y[n] = b_0 x[n] + b_1 x[n-1] + b_2 x[n-2] - a_1 y[n-1] - a_2 y[n-2] \f]   
  The arrays of poles and zeros are understood to contain only one pole (zero) for each complex 
  conjugate pair of poles and zeros of the actual filter. If there is a first order stage present
  (a real pole/zero) than this real pole/zero should be the last entry in the array and the flag 
  lastStageIsFirstOrder should be set to true. ...to be deprecated */
  void polesAndZerosToBiquadCascade(Complex *poles, int numPoles, Complex *zeros, int numZeros, 
    double *b0, double *b1, double *b2, double *a1, double *a2, bool lastStageIsFirstOrder);

  void polesAndZerosToBiquadCascade(Complex *poles, Complex *zeros, int order,
    double *b0, double *b1, double *b2, double *a1, double *a2);



  /**  !!! NOT YET FULLY IMPLEMETED !!!
  Converts the coefficients of a cascade biquad of biquad filters each of which having the form:
  \f[ y[n] = b_0 x[n] + b_1 x[n-1] + b_2 x[n-2] - a_1 y[n-1] - a_2 y[n-2] \f] 
  into coefficients for a direct form filter of the form:
  \f[ y[n] = sum_{k=0}^K b_k x[n-k] - sum_{m=1}^M a_m y[n-m] \f] 
  The b0, b1, b2, a1, a2 arrays should contain the coefficients for the individual biquad stages.
  Then, when B is the number of biquad stages, the b array will have to be of size 
  K=2*B+1 (b_0,..., b_K) and the a array will have to be of size M=2*B (a_1,...,a_M) */
  void biquadCascadeToDirectForm(int numBiquads, double *b0, double *b1, double *b2, double *a1, 
    double *a2, double *b, double *a);

  /** Calculates the magnitude-response of a digital biquad filter with coefficeints b0, b1, b2,
  a0, a1, a1 at the normalized radian frequency 'omega'. */
  double getBiquadMagnitudeAt(double b0, double b1, double b2, 
    double a1, double a2, double omega);

  /** Normalizes the biquad stages described by the given coefficients in such a way that each 
  stage has unit magnitude at the normalized radian frequency 'omega'. If a gainFactor is passed, it 
  will normalize to this gain factor.  */
  void normalizeBiquadStages(double *b0, double *b1, double *b2, 
    double *a1, double *a2, double omega, int numStages, double gainFactor = 1.0);




}  // end namespace rosic

#endif // rosic_FilterCoefficientConversions_h
