#include "rosic_BiquadDesigner.h"
using namespace rosic;

double BiquadDesigner::getBiquadMagnitudeAt(const double &b0, const double &b1, const double &b2, 
  const double &a1, const double &a2, const double &frequency, const double &sampleRate)
{
  double omega = 2.0 * PI * frequency / sampleRate; // optimize to one mul
  double c1  = cos(omega);
  double c2  = cos(2.0*omega);

  double a1m  = -a1; // we use the other sign-convention in the getSample-function
  double a2m  = -a2;

  double num = b0*b0 + b1*b1   + b2*b2   + 2.0*(b0*b1 + b1*b2)  *c1 + 2.0*b0*b2*c2;
  double den = 1.0   + a1m*a1m + a2m*a2m + 2.0*(  a1m + a1m*a2m)*c1 + 2.0*  a2m*c2;
  double mag = sqrt(num/den);

  return mag;
}