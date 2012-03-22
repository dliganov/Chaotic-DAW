#ifndef rosic_IntegerFunctions_h
#define rosic_IntegerFunctions_h

#include "../basics/rosic_FunctionTemplates.h"

namespace rosic
{

  /** Calculates the binomial coefficient n over k. */
  INLINE unsigned int binomialCoefficient(unsigned int n, unsigned int k);

  /** Calculates the factorial of some integer n >= 0. */
  INLINE unsigned int factorial(unsigned int n);

  /** Calculates the greatest common divisor of n and m. */
  INLINE unsigned int gcd(unsigned int m, unsigned int n);

  /** Calculates the least common multiple of n and m. */
  INLINE unsigned int lcm(unsigned int m, unsigned int n);

  /** Power function for integers. Multiplies the base exponent-1 time with itself and returns the 
  result. */
  INLINE int powInt(int base, int exponent);

  /** Product of all integers between min and max (min and max inclusive). */
  INLINE unsigned int product(unsigned int min, unsigned int max);

  /** Sum of all integers between min and max (min and max inclusive). */
  INLINE unsigned int sum(unsigned int min, unsigned int max);

  /** Wraps a number into the range from 0 (inclusive) to length (exclusive). */  
  template <class T>
  INLINE T wrapAround(T numberToWrap, T length);

  //===============================================================================================
  // implementation:

  INLINE unsigned int binomialCoefficient(unsigned int n, unsigned int k)
  {
    if( k == 0 || k == n )
      return 1;
    else
    {
      unsigned int i, j;
      unsigned int*  bcFlat = new unsigned int [(n+1)*(n+1)]; 
      unsigned int** bc     = new unsigned int*[(n+1)]; 
      for(i=0; i<=n; i++)
        bc[i] = & bcFlat[i*(n+1)];

      for(i=0; i<=n; i++) 
        bc[i][0] = 1;
      for(j=0; j<=n; j++) 
        bc[j][j] = 1;
      for(i=1; i<=n; i++)
      {
        for(j=1; j<i; j++)
          bc[i][j] = bc[i-1][j-1] + bc[i-1][j];
      }

      unsigned int result = bc[n][k];
      delete[] bc;
      delete[] bcFlat;
      return result;
    }
  }

  INLINE unsigned int factorial(unsigned int n)
  {
    unsigned int result = 1;
    for(unsigned int i=1; i<=n; i++)
      result *= i;
    return result;
  }

  INLINE unsigned int gcd(unsigned int m, unsigned int n)
  {
    unsigned int lo  = rmin(n, m);
    unsigned int hi  = rmax(n, m);
    unsigned int tmp = hi;
    while( lo != 0 )
    {
      tmp = hi;
      hi  = lo;
      lo  = tmp % lo;
    }
    return hi;
  }

  INLINE unsigned int lcm(unsigned int m, unsigned int n)
  {
    return n*m / gcd(n, m);
  }

  INLINE int powInt(int base, int exponent)
  {
    int result = base;
    for(int p=1; p<exponent; p++)
      result *= base;
    return result;
  }

  INLINE unsigned int product(unsigned int min, unsigned int max)
  {
    if(max < min)
      return 1;
    else
    {
      unsigned int accu = 1;
      for(unsigned int i=min; i<=max; i++)
        accu *= i;
      return accu;
    }
  }

  INLINE unsigned int sum(unsigned int min, unsigned int max)
  {
    if(max < min)
      return 0;
    else
      return ( max*(max+1) - min*(min-1) ) / 2;
  }

  template <class T>
  INLINE T wrapAround(T numberToWrap, T length)
  {
    while( numberToWrap >= length )
      numberToWrap -= length;
    while( numberToWrap < 0 )
      numberToWrap += length;   
    return numberToWrap;
  }

} // end namespace rosic

#endif // #ifndef rosic_IntegerFunctions_h