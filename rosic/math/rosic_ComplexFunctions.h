#ifndef rosic_ComplexFunctions_h
#define rosic_ComplexFunctions_h

// rosic includes:
#include "rosic_Complex.h" 
#include "rosic_RealFunctions.h"
#include "../basics/rosic_HelperFunctions.h"

namespace rosic
{

  /**

  A collection of functions for complex arguments.

  \todo: a few of the elliptic functions are still missing - implement them

  */

  /** Inverse complex Jacobian elliptic function cd with elliptic modulus k. */
  Complex acdC(Complex w, double k);   

  /** Complex inverse cosine (arccosine) function. */
  Complex acosC(Complex z);   

  /** Complex inverse hyperbolic cosine function. */
  Complex acoshC(Complex z);   

  /** Complex inverse sine (arcsine) function. */
  Complex asinC(Complex z);   

  /** Complex inverse hyperbolic sine function. */
  Complex asinhC(Complex z);   

  /** Inverse complex Jacobian elliptic function sn with elliptic modulus k. */
  Complex asnC(Complex w, double k);   

  /** Complex inverse tangent (arctangent) function. */
  Complex atanC(Complex z);   

  /** Complex inverse hyperbolic tangent function. */
  Complex atanhC(Complex z);   

  /** Complex Jacobian elliptic function cd with elliptic modulus k. */
  Complex cdC(Complex u, double k);   

  /** Complex cosine function. */
  Complex cosC(Complex z);   

  /** Complex hyperbolic cosine function. */
  Complex coshC(Complex z);   

  /** Calculates the complex exponential of a complex number. */
  Complex expC(Complex z);    

  /** Calculates the natural (base e) logarithm of a complex number - as the complex 
  logarithm is a multifunction, it returns the main value. */
  Complex logC(Complex z);   

  /** Raises a complex number to a complex power. */
  Complex powC(Complex basis, Complex exponent);

  /** Complex sine function. */
  Complex sinC(Complex z);   

  /** Complex hyperbolic sine function. */
  Complex sinhC(Complex z);   

  /** Complex Jacobian elliptic function sn with elliptic modulus k. */
  Complex snC(Complex u, double k);   

  /** Calculates the (primitive) square root of a complex number. The second square root 
  is obtained by using the negative value. */
  Complex sqrtC(Complex z);  

  /** Complex tangent function. */
  Complex tanC(Complex z);   

  /** Complex hyperbolic tangent function. */
  Complex tanhC(Complex z);   

} // end namespace rosic

#endif // #ifndef rosic_ComplexFunctions_h