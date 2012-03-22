#ifndef rosic_RealFunctionEvaluationAlgorithms_h
#define rosic_RealFunctionEvaluationAlgorithms_h

// standard includes:
#include <math.h>

// rosic includes:
#include "../basics/GlobalDefinitions.h"

namespace rosic
{

  /** Performs M iterations of the Landen transformation of an elliptic modulus k and returns the 
  results in the array v which must be of length M. */
  INLINE void landen(double k, int M, double* v);

  //===============================================================================================
  // implementation for the inline functions:

  INLINE void landen(double k, int M, double* v)
  {
    // performs M iterations of the Landen transformation of an elliptic modulus k and returns the 
    // results in the array v which must be of length M+1
    int n;
    if( k == 0.0 || k == 1.0 )
    {
      v[0] = k;
      for(n=1; n<M; n++)
        v[n] = 0.0;
      return;
    }
    else
    {
      for(n=0; n<M; n++)
      {
        k    = (k/(1.0+sqrt(1.0-k*k)));
        k   *= k;
        v[n] = k;
      }
    }
  }


} // end namespace rosic

#endif // #ifndef rosic_RealFunctionEvaluationAlgorithms_h