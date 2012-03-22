#include "rosic_PrototypeDesigner.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

PrototypeDesigner::PrototypeDesigner()
{
  L                     = 2;
  r                     = 0;
  N                     = L+r;
  approximationMethod   = BUTTERWORTH;
  prototypeMode         = LOWPASS_PROTOTYPE;
  numFinitePoles        = 2;
  numFiniteZeros        = 0;
  z                     = NULL;
  p                     = NULL;
  //gain                  = 1.0;
  Ap                    = amp2dB(sqrt(2.0));     // 3.01 dB passband ripple for lowpasses
  As                    = 60.0;                  // 60.0 dB stopband attenuation for lowpasses
  A                     = 0.0;                   // cut/boost in dB for shelvers
  A0                    = 0.0;                   // reference gain in dB for shelvers
  Rp                    = 0.95;                  // inner ripple as fraction of dB-peak-gain for shelv
  Rs                    = 0.05;                  // outer ripple as fraction of peka
  stateIsDirty          = true;                  // poles and zeros need to be evaluated
  allocatePolesAndZeros();
  updatePolesAndZeros();

  // debug:
  //double test = getNormalizedMagnitudeAt(1.0);
  //int dummy = 0;
}

PrototypeDesigner::~PrototypeDesigner()
{
  if( p != NULL )
    delete[] p;
  if( z != NULL )
    delete[] z;
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void PrototypeDesigner::setOrder(int newOrder)
{
  if( newOrder >= 1 && newOrder != N )
  {
    N = newOrder;
    if( isOdd(N) )
    {
      r = 1;
      L = (N-1)/2;
    }
    else
    {
      r = 0;
      L = N/2;
    }
    allocatePolesAndZeros();
    stateIsDirty = true;
  }
}

void PrototypeDesigner::setApproximationMethod(int newApproximationMethod)
{
  if( newApproximationMethod < BUTTERWORTH || newApproximationMethod > PAPOULIS )
    DEBUG_BREAK; // this is not one of the enumerated approximation methods

  if( newApproximationMethod != approximationMethod )
  {
    approximationMethod = newApproximationMethod;
    stateIsDirty        = true;
  }
}

void PrototypeDesigner::setPrototypeMode(int newPrototypeMode)
{
  if( newPrototypeMode == LOWPASS_PROTOTYPE || newPrototypeMode == LOWSHELV_PROTOTYPE )
  {
    prototypeMode = newPrototypeMode;
    stateIsDirty  = true;
  }
  else
    DEBUG_BREAK; // this is not one of the enumerated modes
}

void PrototypeDesigner::setPassbandRipple(double newPassbandRipple)
{
  if( newPassbandRipple >= 0.0 )
  {
    Ap           = newPassbandRipple;
    stateIsDirty = true;
  }
  else
    DEBUG_BREAK; // ripple (in dB) must be >= 0
}

void PrototypeDesigner::setStopbandRejection(double newStopbandRejection)
{
  if( newStopbandRejection >= 0.0 )
  {
    As           = newStopbandRejection;
    stateIsDirty = true;
  }
  else
    DEBUG_BREAK; // ripple (in dB) must be >= 0
}

void PrototypeDesigner::setGain(double newGain)
{
  if( newGain != A )
  {
    A            = newGain;
    stateIsDirty = true;
  }
}

void PrototypeDesigner::setReferenceGain(double newReferenceGain)
{
  if( newReferenceGain != A0 )
  {
    A0           = newReferenceGain;
    stateIsDirty = true;
  }
}

void PrototypeDesigner::setPassbandGainRatio(double newPassbandGainRatio)
{
  if( newPassbandGainRatio >= 1.0 ||
      newPassbandGainRatio <= 0.0 ||
      newPassbandGainRatio < Rs      )
  {
    DEBUG_BREAK; // this bandwidth gain ratio makes no sense (inequation 51 is violated)
    return;
  }
  if( newPassbandGainRatio != Rp  )
  {
    Rp           = newPassbandGainRatio;
    stateIsDirty = true;
  }
}

void PrototypeDesigner::setStopbandGainRatio(double newStopbandGainRatio)
{
  if( newStopbandGainRatio >= 1.0 ||
      newStopbandGainRatio <= 0.0 ||
      newStopbandGainRatio > Rp      )
  {
    DEBUG_BREAK; // this stopband gain ratio makes no sense (inequation 51 is violated)
    return;
  }
  if( newStopbandGainRatio != Rp  )
  {
    Rs           = newStopbandGainRatio;
    stateIsDirty = true;
  }
}

//-------------------------------------------------------------------------------------------------
// calculation of the required filter order:

double PrototypeDesigner::getRequiredButterworthOrder(double passbandFrequency,
  double passbandRipple, double stopbandFrequency, double stopbandRipple)
{
  double Gp = pow(10.0, -passbandRipple/20.0);                      // (1),Eq.1
  double Gs = pow(10.0, -stopbandRipple/20.0);                      // (1),Eq.1
  double ep = sqrt(1.0 / (Gp*Gp) - 1.0);                            // (1),Eq.2
  double es = sqrt(1.0 / (Gs*Gs) - 1.0);                            // (1),Eq.2
  return log(es/ep) / log(stopbandFrequency/passbandFrequency);     // (1),Eq.9
}

double PrototypeDesigner::getRequiredChebychevOrder(double passbandFrequency,
  double passbandRipple, double stopbandFrequency, double stopbandRipple)
{
  double Gp = pow(10.0, -passbandRipple/20.0);                       // (1),Eq.1
  double Gs = pow(10.0, -stopbandRipple/20.0);                       // (1),Eq.1
  double ep = sqrt(1.0 / (Gp*Gp) - 1.0);                             // (1),Eq.2
  double es = sqrt(1.0 / (Gs*Gs) - 1.0);                             // (1),Eq.2
  return acosh(es/ep) / acosh(stopbandFrequency/passbandFrequency);  // (1),Eq.9
}

double PrototypeDesigner::getRequiredEllipticOrder(double passbandFrequency,
  double passbandRipple, double stopbandFrequency, double stopbandRipple)
{
  double Gp = pow(10.0, -passbandRipple/20.0);                       // (1),Eq.1
  double Gs = pow(10.0, -stopbandRipple/20.0);                       // (1),Eq.1
  double ep = sqrt(1.0 / (Gp*Gp) - 1.0);                             // (1),Eq.2
  double es = sqrt(1.0 / (Gs*Gs) - 1.0);                             // (1),Eq.2
  double k  = passbandFrequency / stopbandFrequency;                 // (1),Eq.3
  double k1 = ep/es;                                                 // (1),Eq.3
  double  K, Kp, K1, K1p;
  ellipticIntegral(k,  &K,  &Kp);                                    // (1),Eq.19
  ellipticIntegral(k1, &K1, &K1p);
  return (K1p*K)/(K1*Kp);                                            // (1),Eq.34
}

//-------------------------------------------------------------------------------------------------
// inquiry:

Complex PrototypeDesigner::getFilterResponseAt(Complex s)
{
  Complex num, den;
  Complex tmp;
  int     Lz, Lp;

  // initialize the numerator and denominator:
  if( isOdd(numFiniteZeros) )
  {
    num = -z[L+r-1].re;
    Lz  = (numFiniteZeros-1)/2;
  }
  else
  {
    num = 1.0;
    Lz  = numFiniteZeros/2;
  }
  if( isOdd(numFinitePoles) )
  {
    den = -p[L+r-1].re;
    Lp  = (numFinitePoles-1)/2;
  }
  else
  {
    den = 1.0;
    Lp  = numFinitePoles/2;
  }

  // accumulate the product of the linear factors for the denominator (poles) and numerator
  // (zeros):
  int i;
  for(i=0; i<Lz; i++)
    num *= ((s-z[i]) * (s-z[i].getConjugate()));
  for(i=0; i<Lp; i++)
    den *= ((s-p[i]) * (s-p[i].getConjugate()));

  // return the quotient of the calculated numerator and denominator as result:
  return num/den;
}

double PrototypeDesigner::getMagnitudeAt(double w)
{
  return getFilterResponseAt(Complex(0.0, w)).getRadius();
}

double PrototypeDesigner::findFrequencyWithMagnitude(double magnitude, double wLow, double wHigh)
{
  // until we have something better, we search for the frequency at which the desired gain occurs
  // by means of the bisection method:

  double wMid  = 0.5 * (wLow+wHigh);
  double mLow  = getMagnitudeAt(wLow);
  double mMid  = getMagnitudeAt(wMid);
  double mHigh = getMagnitudeAt(wHigh);

  while( wHigh-wLow > 0.0001 )
  {
    wMid  = 0.5 * (wLow+wHigh);
    mLow  = getMagnitudeAt(wLow);
    mMid  = getMagnitudeAt(wMid);
    mHigh = getMagnitudeAt(wHigh);

    if( mMid > magnitude )
      wLow = wMid;
    else
      wHigh = wMid;

    //int dummy = 0;
  }


  //int dummy = 0;

  return 0.5 * (wLow+wHigh); //preliminary
}

int PrototypeDesigner::getNumFinitePoles()
{
  return numFinitePoles;
}

int PrototypeDesigner::getNumFiniteZeros()
{
  return numFiniteZeros;
}

int PrototypeDesigner::getNumNonRedundantFinitePoles()
{
  if( isEven(numFinitePoles) )
    return numFinitePoles/2;
  else
    return (numFinitePoles+1)/2;
}

int PrototypeDesigner::getNumNonRedundantFiniteZeros()
{
  if( isEven(numFiniteZeros) )
    return numFiniteZeros/2;
  else
    return (numFiniteZeros+1)/2;
}

void PrototypeDesigner::getPolesAndZeros(Complex *poles, Complex *zeros)
{
  if( stateIsDirty == true )
    updatePolesAndZeros(); // re-calculate only if necesarry
  for(int i=0; i<(L+r); i++)
  {
    poles[i] = p[i];
    zeros[i] = z[i];
  }
}

bool PrototypeDesigner::hasCurrentMethodRippleParameter()

{
  if( prototypeMode == LOWPASS_PROTOTYPE )
  {
    if( (approximationMethod == ELLIPTIC) || (approximationMethod == CHEBYCHEV) )
      return true;
    else
      return false;
  }
  else
  {
    if( (approximationMethod == ELLIPTIC) || (approximationMethod == CHEBYCHEV)
      || (approximationMethod == INVERSE_CHEBYCHEV) )
      return true;
    else
      return false;
  }
}

bool PrototypeDesigner::hasCurrentMethodRejectionParameter()
{
  if( prototypeMode == LOWPASS_PROTOTYPE )
  {
    return (approximationMethod == ELLIPTIC) || (approximationMethod == INVERSE_CHEBYCHEV);
  }
  else
    return false;
}

//-------------------------------------------------------------------------------------------------
// evaluation for the elliptic degree equation:

double PrototypeDesigner::ellipdeg(int N, double k_1)
{
  int L;
  if( isEven(N) )
    L = N/2;
  else
    L = (N-1)/2;

  double kmin = 1e-6;
  double k;
  if (k_1 < kmin)
    k = ellipdeg2(1.0 / (double) N, k_1);
  else
  {
    double kc = sqrt(1-k_1*k_1);			  // complement of k1

    double u_i;
    double prod = 1.0;
    for(int i=1; i<=L; i++)
    {
      u_i   = (double) (2*i-1) / (double) N;
      prod *= snC(u_i, kc).re;
    }
    prod      = prod*prod*prod*prod;

    double kp = pow(kc, (double) N) * prod; // complement of k
    k = sqrt(1.0-kp*kp);
  }

  return k;
}

double PrototypeDesigner::ellipdeg1(int N, double k)
{
  int L;
  if( isEven(N) )
    L = N/2;
  else
    L = (N-1)/2;

  double u_i;
  double prod = 1.0;
  for(int i=1; i<=L; i++)
  {
    u_i   = (double) (2*i-1) / (double) N;
    prod *= snC(u_i, k).re;
  }
  prod      = prod*prod*prod*prod;
  double k1 = pow(k, (double) N) * prod;

  return k1;
}

double PrototypeDesigner::ellipdeg2(double N, double k)
{
  int M = 7;

  double K;
  double Kprime;
  ellipticIntegral(k, &K, &Kprime);

  double q  = exp(-PI*Kprime/K);
  double q1 = pow(q, N);

  int m;
  double sum1 = 0.0;
  double sum2 = 0.0;
  for(m=1; m<=M; m++)
  {
    sum1 += pow(q1, (double) (m*(m+1)) );
    sum2 += pow(q1, (double) (m*m)     );
  }

  double tmp = (1.0+sum1)/(1.0+2.0*sum2);
  tmp       *= tmp;

  double k1  = 4.0 * sqrt(q1) * tmp;

  return k1;
}

void PrototypeDesigner::allocatePolesAndZeros()
{
  // re-allocate memory:
  if( p != NULL )
    delete[] p;
  if( z != NULL )
    delete[] z;
  p = new Complex[L+r];
  z = new Complex[L+r];
}

//-------------------------------------------------------------------------------------------------
// pole/zero calculation:

void PrototypeDesigner::rescalePoles(double targetGainAtUnitCutoff)
{
  /*
  // .... still under construction ....
  // perhaps the polynomial coefficients are still wrong....

  int dummy;

  // establish the polynomial coefficients of the denominator from its roots:
  double currentFactor[3]; // coefficients of the current quadratic or linear factor
  double coeffs[40];       // preliminary - use dynamic allocation later (2*(order+1) / 2*(N+1) coeffs are needed)

  int k;

  coeffs[0] = 1.0;
  for(k=1; k<=N; k++)
    coeffs[k] = 0.0;

  int length = 1;
  for(k=0; k<L; k++)
  {
    currentFactor[0] = 1.0;                                   // mulitplier for x^2
    currentFactor[1] = -2.0 * p[k].re;                        // multiplier for x
    currentFactor[2] = p[k].re * p[k].re + p[k].im * p[k].im; // constant term

    convolveInPlace(coeffs, length, currentFactor, 3);

    length += 2;

    dummy = 0;
  }

  if( r == 1 )
  {
    currentFactor[0] = 1.0;          // multiplier for x
    currentFactor[1] = -p[L+r-1].re; // constant term

    convolveInPlace(coeffs, length, currentFactor, 2);

    length += 1;

    dummy = 0;
  }

  // multiply the polynomial by itself to yield the squared value:
  convolveInPlace(coeffs, length, coeffs, length);
  length += length-1;


  // reverse the coefficient array such that the constant term comes first and the multiplier for
  // highest power comes last:
  reverse(coeffs, length);

  // subtract the target value of the denominator polynomial from the current constant term ("bring
  // it on the left hand side of the equation"):
  double target = 4.0; // preliminary
  coeffs[0] -= target;
  double root = Polynom::getRootNear(1.0, coeffs, length-1, -20.0, 20.0);



  // scale the poles:
  double scaler = 1.0 / root;
  for(k=0; k<L+r; k++)
    p[k] *= scaler;

  dummy = 1;
  */


  /*
  // preliminary we use a very crude algorithm:
  double targetMagnitude   = dB2amp(targetGainAtUnitCutoff);
  double currentMagnitude  = getFilterResponseAt(Complex(0.0, 1.0)).getMagnitude();
         currentMagnitude /= getFilterResponseAt(Complex(0.0, 0.0)).getMagnitude();
  double ratio             = currentMagnitude / targetMagnitude;
  while( fabs(amp2dB(ratio)) > 0.01 )
  {
    double scaler = pow(1.0/ratio, 0.1);

    for(int k=0; k<L+r; k++)
      p[k] *= scaler;

    targetMagnitude   = dB2amp(targetGainAtUnitCutoff);
    currentMagnitude  = getFilterResponseAt(Complex(0.0, 1.0)).getMagnitude();
    currentMagnitude /= getFilterResponseAt(Complex(0.0, 0.0)).getMagnitude();
    ratio             = currentMagnitude / targetMagnitude;

    int dummy = 0;
  }
  */


  /*
  double targetMagnitude = dB2amp(targetGainAtUnitCutoff);
  double freq            = findFrequencyWithMagnitude(targetMagnitude, 0.0, 1.5);
  double scaler          = 1.0 / freq;
  for(int k=0; k<L+r; k++)
    p[k] *= scaler;
    */








}

void PrototypeDesigner::updatePolesAndZeros()
{
  if( stateIsDirty == true )
  {
    if( prototypeMode == LOWPASS_PROTOTYPE )
    {
      switch( approximationMethod )
      {
      case BUTTERWORTH:       makeButterworthLowpass();         break;
      case CHEBYCHEV:         makeChebychevLowpass();           break;
      case INVERSE_CHEBYCHEV: makeInverseChebychevLowpass();    break;
      case ELLIPTIC:          makeEllipticLowpass();            break;
      case BESSEL:            makeBesselLowpass();              break;
      }
    }
    else if( prototypeMode == LOWSHELV_PROTOTYPE )
    {
      switch( approximationMethod )
      {
      case BUTTERWORTH:       makeButterworthLowShelv();        break;
      case CHEBYCHEV:         makeChebychevLowShelv();          break;
      case INVERSE_CHEBYCHEV: makeInverseChebychevLowShelv();   break;
      case ELLIPTIC:          makeEllipticLowShelv();           break;
      }
    }
  }
}

void PrototypeDesigner::makeBypass()
{
  numFinitePoles = 0;
  numFiniteZeros = 0;
}

void PrototypeDesigner::makeButterworthLowpass()
{
  numFinitePoles = N;
  numFiniteZeros = 0;

  // intermediate variables:
  Complex j(0.0, 1.0);                                     // imaginary unit
  //double  Gp   = pow(10.0, -Ap/20.0);                    // (1),Eq.1 - not used here ...
  double  Gp     = sqrt(0.5);                              // use -3 dB point for Butterworths
  double  ep     = sqrt(1.0/(Gp*Gp)-1.0);                  // (1),Eq.2
  double  ep_pow = pow(ep, -1.0/(double) N);

  // calculate the position of the real pole (if present):
  if( r == 1 )
  {
    p[L+r-1] = -ep_pow;                                    // Eq.70
    z[L+r-1] = INF;                                        // zero at infinity
  }
  // calculate the complex conjugate poles and zeros:
  double  u_i;
  for(int i=0; i<L; i++)
  {
    u_i  = (double) (2*(i+1)-1) / (double) N;              // Eq.69
    p[i] = ep_pow*j*expC(j*u_i*PI*0.5);                    // Eq.70
    z[i] = INF;                                            // zeros are at infinity
  }

  stateIsDirty = false;
}

void PrototypeDesigner::makeButterworthLowShelv()
{
  numFinitePoles = N;
  numFiniteZeros = N;

  // catch some special cases:
  if( A0 == -INF ) // lowpass-case
  {
    makeButterworthLowpass();
    return;
  }
  else if( abs(A-A0) <  0.001 )
    makeBypass();

  // intermediate variables:
  double G0   = dB2amp(A0);
  double G    = dB2amp(A);
  double GB   = sqrt(G0*G);                                // (2),Eq.52
  double ep   = sqrt( (G*G-GB*GB) / (GB*GB-G0*G0) );       // (2),Eq.12
  double g0   = pow(G0, 1.0 / (double) N);                 // (2),Eq.94
  double g    = pow(G,  1.0 / (double) N);                 // (2),Eq.94
  double wb   = 1.0;                                       // unit cutoff prototype
  double beta = wb * pow(ep, -1.0 / (double) N);           // (2),Eq.94

  // calculate the position of the real pole (if present):
  if( r == 1 )
  {
    p[L+r-1] = -beta;                                      // (2),Eq.93
    z[L+r-1] = -g*beta/g0;                                 // (2),Eq.93
  }
  // calculate the complex conjugate poles and zeros:
  double phi, s, c;
  for(int i=0; i<L; i++)
  {
    phi     = (double) (2*(i+1)-1)*PI / (double) (2*N);    // (2),Eq.95
    s       = sin(phi);                                    // (2),Eq.95
    c       = cos(phi);                                    // (2),Eq.95
    z[i].re = -s*g*beta/g0;                                // (2),Eq.93
    z[i].im =  c*g*beta/g0;                                // (2),Eq.93
    p[i].re = -s*beta;                                     // (2),Eq.93
    p[i].im =  c*beta;                                     // (2),Eq.93
  }

  stateIsDirty = false;
}


void PrototypeDesigner::makeChebychevLowpass()
{
  numFinitePoles = N;
  numFiniteZeros = 0;

  // intermediate variables:
  double  Gp   = pow(10.0, -Ap/20.0);            // Eq. 1
  double  ep   = sqrt(1.0/(Gp*Gp) - 1.0);        // Eq. 2
  double  v_0  = asinh(1.0/ep) / (N*PI*0.5);     // Eq. 72
  double  u_i;
  Complex j(0.0, 1.0); // imaginary unit

  // calculate the position of the real pole (if present):
  if( r == 1 )
  {
    p[L+r-1] = -sinh(v_0*PI*0.5);                    // Eq. 71
    z[L+r-1] = INF;
  }
  // calculate the complex conjugate poles and zeros:
  for(int i=0; i<L; i++)
  {
    u_i  = (double) (2*(i+1)-1) / (double) N;        // Eq. 69
    p[i] = j*cosC((u_i-j*v_0)*PI*0.5);               // Eq. 71
    z[i] = INF;                                      // zeros at infinity
  }

  //gain = 1.0 / getFilterResponseAt(Complex(0.0, 0.0)).getMagnitude();
  stateIsDirty = false;
}

void PrototypeDesigner::makeChebychevLowShelv()
{
  numFinitePoles = N;
  numFiniteZeros = N;

  // calculate the linear gain-factors:
  double G0 = dB2amp(A0);
  double G  = dB2amp(A);

  // catch some special cases:
  if( A0 == -INF ) // lowpass-case
  {
    makeChebychevLowpass();
    return;
  }
  else if( abs(A-A0) <  0.001 )
    makeBypass();

  // calculate intermediate variables:
  double Gp    = dB2amp(A0 + Rp*A);
  double ep    = sqrt( (G*G-Gp*Gp) / (Gp*Gp-G0*G0) );
  double g0    = pow(G0, 1.0 / (double) N);
  //double g     = pow(G,   1.0 / (double) N);
  double alpha = pow(1.0/ep + sqrt(1.0 + 1.0/(ep*ep)), 1.0/(double) N);
  double beta  = pow((G/ep + Gp*sqrt(1.0 + 1.0/(ep*ep)) ), 1.0/(double) N);
  double u     = log(beta/g0);
  double v     = log(alpha);
  double Gb    = sqrt(G0*G);
  double eb    = sqrt( (G*G-Gb*Gb) / (Gb*Gb-G0*G0) );
  double wb    = 1.0 / cosh( acosh(eb/ep) / (double)N ); // why 1/cosh(...) and not simply cosh?

  // calculate real pole and zero of the first order stage, if present and store them in the last
  // array slots:
  if( r == 1 )
  {
    p[L+r-1] = -wb*sinh(v);
    z[L+r-1] = -wb*sinh(u);
  }
  // calculate the complex conjugate poles and zeros:
  double phi_i; //, s, c;
  Complex j(0.0, 1.0); // imaginary unit
  for(int i=0; i<L; i++)
  {
    phi_i = (double) (2*(i+1)-1)*PI / (double) (2*N);
    z[i]  = j*wb*cosC(phi_i - j*u);
    p[i]  = j*wb*cosC(phi_i - j*v);
  }

  stateIsDirty = false;
}

void PrototypeDesigner::makeInverseChebychevLowpass()
{
  numFinitePoles = N;
  if( isEven(N) )
    numFiniteZeros = N;
  else
    numFiniteZeros = N-1;

  // declare/assign/calculate some repeatedly needed variables:
  double  Gs = pow(10.0, -As/20.0);                      // Eq. 1
  double  es = sqrt(1.0/(Gs*Gs)-1.0);                    // Eq. 2
  double  v0 = asinh(es) / (N*PI*0.5);                   // Eq. 74
  Complex j(0.0, 1.0);                                   // imaginary unit

  double  wb = 1.0;
    // wb = 1.0 leads to a gain of Gs (stopband-gain) at unity (w=1), we rescale it here so as to
    // have the -3 dB point at w=1:
  double  Gp = sqrt(0.5);
  double  ep = sqrt(1.0/(Gp*Gp)-1.0);
  wb         = cosh( acosh(es/ep) / N );                        // (1),Eq.9

  // calculate the position of the real pole (if present):
  double  ui;
  if( r == 1 )
  {
    p[L+r-1] = -wb / sinh(v0*PI*0.5);                           // Eq.73 with k=1
    z[L+r-1] = INF;
  }
  // calculate the complex conjugate poles and zeros:
  for(int i=0; i<L; i++)
  {
    ui   = (double) (2*(i+1)-1) / (double) N;                   // Eq.69
    z[i] = wb / (j*cosC(ui*PI/2));                              // Eq.73 with k=1
    p[i] = wb / (j*cosC((ui - j*v0)*PI/2));                     // Eq.73 with k=1
  }

  stateIsDirty = false;
}

void PrototypeDesigner::makeInverseChebychevLowShelv()
{
  numFinitePoles = N;
  numFiniteZeros = N;

  // calculate the linear gain-factors:
  double G0 = dB2amp(A0);
  double G  = dB2amp(A);

  // catch some special cases:
  if( A0 == -INF ) // lowpass-case
  {
    makeInverseChebychevLowpass();
    return;
  }
  else if( abs(A-A0) <  0.001 )
    makeBypass();

  // calculate intermediate variables (\todo check if the gains have reasonable values):
  //double Gs    = dB2amp(Rs*G + (1.0-Rs)*G0);
  double Gs    = dB2amp(A0 + Rs*A);
  double es    = sqrt( (G*G-Gs*Gs) / (Gs*Gs-G0*G0) );
  //double g0    = pow(G0, 1.0 / (double) N);
  double g     = pow(G,   1.0 / (double) N);
  double alpha = pow(es + sqrt(1.0 + es*es), 1.0 / (double) N);
  double beta  = pow((G0*es + Gs*sqrt(1.0 + es*es) ), 1.0/(double) N);
  double u     = log(beta/g);
  double v     = log(alpha);
  double Gb    = sqrt(G0*G);
  double eb    = sqrt( (G*G-Gb*Gb) / (Gb*Gb-G0*G0) );
  double wb    = cosh( acosh(es/eb) / (double) N );  // why not 1 / cosh(..)?

  // calculate real pole and zero of the first order stage, if present and store them in the last
  // array slots:
  if( r == 1 )
  {
    z[L+r-1] = -wb/sinh(u);
    p[L+r-1] = -wb/sinh(v);
  }
  // calculate the complex conjugate poles and zeros:
  double  phi_i;
  Complex j(0.0, 1.0); // imaginary unit
  for(int i=0; i<L; i++)
  {
    phi_i = (double) (2*(i+1)-1)*PI / (double) (2*N);
    z[i]  = wb / (j*cosC(phi_i-j*u));
    p[i]  = wb / (j*cosC(phi_i-j*v));
  }

  stateIsDirty = false;
}

void PrototypeDesigner::makeEllipticLowpass()
{
  numFinitePoles = N;
  if( isEven(N) )
    numFiniteZeros = N;
  else
    numFiniteZeros = N-1;

  // declare/assign/calculate some repeatedly needed variables:
  Complex j(0.0, 1.0);                                    // imaginary unit
  double  u_i;
  Complex zeta_i;
  double  Gp  = pow(10.0, -Ap/20.0);                      // Eq. 1
  double  Gs  = pow(10.0, -As/20.0);                      // Eq. 1
  double  ep  = sqrt(1.0/(Gp*Gp) - 1.0);                  // Eq. 2
  double  es  = sqrt(1.0/(Gs*Gs) - 1.0);                  // Eq. 2
  double  k1  = ep/es;                                    // Eq. 3
  double  k   = ellipdeg(N, k1);                          // solve degree equation for k
  double  v_0 =  (-j*asnC(j/ep, k1) / (double) N).re;    // from ellipap.m

  // calculate the position of the real pole (if present):
  if( r == 1 )
  {
    //p[L+r-1] = -Omega_p/sinh(v_0*PI*0.5*k);                     // Eq. 73
    p[L+r-1] = j * snC(j*v_0, k);                                // from ellipap.m
    z[L+r-1] = INF;
  }
  // calculate the complex conjugate poles and zeros:
  for(int i=0; i<L; i++)
  {
    u_i    = (double) (2*(i+1)-1) / (double) N;                  // Eq. 69
    zeta_i = cdC(u_i, k);                                        // from ellipap.m
    z[i]   = j / (k*zeta_i);                                     // Eq. 62
    p[i]   = j*cdC((u_i-j*v_0), k);
  }

  stateIsDirty = false;
}

void PrototypeDesigner::makeEllipticLowShelv()
{
  numFinitePoles = N;
  numFiniteZeros = N;

  // catch some special cases:
  if( A0 == -INF ) // lowpass-case
  {
    makeEllipticLowpass();
    return;
  }
  else if( abs(A-A0) <  0.001 )
    makeBypass();

  // intermediate variables:
  double  G0  = dB2amp(A0);                            // reference amplitude
  double  G   = dB2amp(A);                             // boost/cut amplitude
  double  Gp  = dB2amp(A0 + Rp*A);                     // passband-amplitude (Rp near 1)
  double  Gs  = dB2amp(A0 + Rs*A);                     // stopband-amplitude (Rs near 0)
  double  Gb  = sqrt(G0*G);                            // (2),Eq.52 (gain at the bandedges)
  double  ep  = sqrt( (G*G-Gp*Gp) / (Gp*Gp-G0*G0) );   // (2),Eq.12
  double  es  = sqrt( (G*G-Gs*Gs) / (Gs*Gs-G0*G0) );   // (2),Eq.39
  double  eb  = sqrt( (G*G-Gb*Gb) / (Gb*Gb-G0*G0) );   // (2),Eq.64
  double  k1  = ep/es;                                 // (2),Eq.39
  double  k   = ellipdeg(N, k1);                       // degree equation
  Complex u   = acdC(eb/ep, k1) / N;                   // following text after (2),Eq.65
  double  wb  = 1.0 / cdC(u, k).re;                    // ...ditto
  Complex j   = Complex(0.0, 1.0);                     // imaginary unit
  Complex ju0 = asnC(j*G/(ep*G0), k1) / N;             // line 111 in hpeq.m
  Complex jv0 = asnC(j  / ep,     k1) / N;             // line 113 in hpeq.m

  // calculate the position of the real pole (if present):
  if( r == 1 )
  {
    p[L+r-1] = wb*(j*cdC(-1.0+jv0,k)).re;              // line 148 in hpeq.m
    z[L+r-1] = wb*(j*cdC(-1.0+ju0,k)).re;              // line 145 in hpeq.m
  }
  // calculate the complex conjugate poles and zeros:
  double ui;
  for(int i=0; i<L; i++)
  {
    ui   = (double) (2*(i+1)-1) / (double) N;          // (2),Eq.37
    p[i] = j*wb * cdC( (ui-jv0), k );                  // line 179 in hpeq.m

    if( G0 == 0.0 && G != 0.0 )                        // lines 172-178 in hpeq.m
      z[i] = j*wb / (k*cdC(ui,k));   // lowpass
    else if( G0 != 0.0 && G == 0.0 )
      z[i] = j*wb * cdC(ui,k);       // highpass
    else
      z[i] = j*wb * cdC(ui-ju0,k);   // low-shelv
  }

  stateIsDirty = false;
}

void PrototypeDesigner::makeBesselLowpass()
{
  if( N > 25 )
  {
    DEBUG_BREAK; // Bessel filter poles are tabulated only up to order 25
    return;
  }

  numFinitePoles = N;
  numFiniteZeros = 0;

  for(int k=0; k<L+r; k++)
    z[k] = INF; // zeros at infinity


  /*
  //-----------------------------------
  // currently in test:
  double a[26];
  rosic::makeBesselPolynomial(a, N);
  Complex r[26];  
  rosic::findPolynomialRoots(a, N, r);
  if( isEven(N) )
  {
    for(int k=0; k<L; k++)
      p[k] = r[2*k+1];
  }
  else
  {
    for(int k=0; k<L; k++)
    {
      p[k] = r[2*k+2];
    }
    p[(N-1)/2] = r[1];
  } 
  // looks good so far, but still todo: rescale poles here to better match the butterworth response
  // ....maybetry to match it at some point far above the cutoff point ...like at 100 or so
  stateIsDirty = false;
  return;
  //-----------------------------------
  */
  

  switch(N)
  {
  case  1:
    {
      p[0].re = -1.0;
      p[0].im =  0.0;
    } break;
  case  2:
    {
      p[0].re = -0.8660254037844386;
      p[0].im = -0.5;
    } break;
  case  3:
    {
      p[0].re = -0.74564038584807668;
      p[0].im =  0.71136662497283532;
      p[1].re = -0.94160002653320674;
      p[1].im =  0.0;
    } break;
  case 4:
    {
      p[0].re = -0.65721117167188292;
      p[0].im = -0.83016143500487338;
      p[1].re = -0.904758796788245;
      p[1].im = -0.27091873300387465;
    } break;
  case 5:
    {
      p[0].re = -0.85155361936883955;
      p[0].im = -0.4427174639443327;
      p[1].re = -0.5905759446119192;
      p[1].im = -0.90720675645745497;
      p[2].re = -0.92644207738776019;
      p[2].im =  0.0;
    } break;
  case 6:
   {
     p[0].re = -0.90939068304722714;
     p[0].im = -0.18569643967930469;
     p[1].re = -0.79965418583282888;
     p[1].im = -0.56217173469373183;
     p[2].re = -0.53855268166931092;
     p[2].im = -0.96168768819542771;
   } break;
  case 7:
    {
      p[0].re = -0.88000293415233743;
      p[0].im = -0.32166527623077396;
      p[1].re = -0.75273554340932147;
      p[1].im = -0.65046963055225504;
      p[2].re = -0.49669172566723169;
      p[2].im = -1.0025085084544203;
      p[3].re = -0.91948715564902905;
      p[3].im = 0.0;
    } break;
  case 8:
    {
      p[0].re = -0.90968315466529104;
      p[0].im = -0.14124379766714229;
      p[1].re = -0.84732508023593345;
      p[1].im = -0.42590175382729351;
      p[2].re = -0.71113818084853997;
      p[2].im = -0.71865173141084016;
      p[3].re = -0.46217404125321221;
      p[3].im = -1.034388681126901;
    } break;
  case 9:
    {
      p[0].re = -0.89112170170797589;
      p[0].im = -0.25265809345821644;
      p[1].re = -0.81480211122690127;
      p[1].im = -0.50858156896314999;
      p[2].re = -0.67436226868547622;
      p[2].im = -0.77305462126911839;
      p[3].re = -0.43314155615536187;
      p[3].im = -1.0600736701359297;
      p[4].re = -0.91549577974990382;
      p[4].im =  0.0;
    } break;
  case 10:
    {
      p[0].re = -0.90913473209005025;
      p[0].im = -0.11395831373355111;
      p[1].re = -0.86884596412847648;
      p[1].im = -0.34300082337663101;
      p[2].re = -0.78376944131014414;
      p[2].im = -0.57591475384999469;
      p[3].re = -0.64175138669883158;
      p[3].im = -0.81758361671910174;
      p[4].re = -0.40832207328688613;
      p[4].im = -1.0812748428191246;
    } break;
 case 11:
   {
     p[0].re = -0.89636567057211658;
     p[0].im = -0.20804803750710318;
     p[1].re = -0.84530440147129626;
     p[1].im = -0.41786969178012484;
     p[2].re = -0.75469389347223026;
     p[2].im = -0.63191500507218468;
     p[3].re = -0.61268715549151942;
     p[3].im = -0.85478138933147652;
     p[4].re = -0.38681495100550911;
     p[4].im = -1.0991174667631209;
     p[5].re = -0.91290672445189824;
     p[5].im =  0.0;
   } break;
 case 12:
   {
     p[0].re = -0.90844782341406827;
     p[0].im = -0.095506365213450392;
     p[1].re = -0.88025343420168267;
     p[1].im = -0.28717795035242266;
     p[2].re = -0.82172969399390772;
     p[2].im = -0.48102121151006766;
     p[3].re = -0.72766816153951597;
     p[3].im = -0.67929611787646937;
     p[4].re = -0.58663693218614776;
     p[4].im = -0.88637727513207265;
     p[5].re = -0.36796400855263128;
     p[5].im = -1.1143735756415463;
   } break;
 case 13:
   {
     p[0].re = -0.89913146654751963;
     p[0].im = -0.17683429561610436;
     p[1].re = -0.86250941982605489;
     p[1].im = -0.35474137311729892;
     p[2].re = -0.79874606924709723;
     p[2].im = -0.53507521206968023;
     p[3].re = -0.70262346757212757;
     p[3].im = -0.71996118901713047;
     p[4].re = -0.56315598424301994;
     p[4].im = -0.91359003383251092;
     p[5].re = -0.35127923233898217;
     p[5].im = -1.1275915483177057;
     p[6].re = -0.91109146659841833;
     p[6].im =  0.0;
   } break;
 case 14:
   {
     p[0].re = -0.90779321383964873;
     p[0].im = -0.082196399419401503;
     p[1].re = -0.88695066749164453;
     p[1].im = -0.24700791787653331;
     p[2].re = -0.84411991609098513;
     p[2].im = -0.41316538251026924;
     p[3].re = -0.77665913870636238;
     p[3].im = -0.58191706773776086;
     p[4].re = -0.67942564251192328;
     p[4].im = -0.75528573050420333;
     p[5].re = -0.5418766775112297;
     p[5].im = -0.93730436835169195;
     p[6].re = -0.33638682249020374;
     p[6].im = -1.13917229783986;
   } break;
 case 15:
   {
     p[0].re = -0.90069816941769787;
     p[0].im = -0.15376811972784393;
     p[1].re = -0.87312646208349853;
     p[1].im = -0.30823524705642674;
     p[2].re = -0.82566314525871465;
     p[2].im = -0.46423487527343255;
     p[3].re = -0.75560271689707281;
     p[3].im = -0.62293963587582668;
     p[4].re = -0.65791965931109986;
     p[4].im = -0.78628955037225157;
     p[5].re = -0.52249540696583308;
     p[5].im = -0.9581787261092527;
     p[6].re = -0.32299630597664442;
     p[6].im = -1.1494161545836294;
     p[7].re = -0.90974823638490643;
     p[7].im =  0.0;
   } break;
 case 16:
   {
     p[0].re = -0.90720995950870009;
     p[0].im = -0.072142113041117326;
     p[1].re = -0.89117230703236472;
     p[1].im = -0.21670896599005765;
     p[2].re = -0.85842642315213302;
     p[2].im = -0.36216972718020657;
     p[3].re = -0.80747902932360038;
     p[3].im = -0.50929337511717998;
     p[4].re = -0.73561663047131165;
     p[4].im = -0.65919508778603941;
     p[5].re = -0.63795025140390671;
     p[5].im = -0.8137453537108762;
     p[6].re = -0.5047606444424767;
     p[6].im = -0.97671374777990905;
     p[7].re = -0.31087827556453879;
     p[7].im = -1.1585528411993304;
   } break;
 case 17:
   {
     p[0].re = -0.90162738507872864;
     p[0].im = -0.13602679951730245;
     p[1].re = -0.88011007044386269;
     p[1].im = -0.2725347156478804;
     p[2].re = -0.8433414495836129;
     p[2].im = -0.41007592829100215;
     p[3].re = -0.78976441477997084;
     p[3].im = -0.54937244052810885;
     p[4].re = -0.71668938423723494;
     p[4].im = -0.69149362863936092;
     p[5].re = -0.61937107173421446;
     p[5].im = -0.83824972528269925;
     p[6].re = -0.4884629337672704;
     p[6].im = -0.99329719563167818;
     p[7].re = -0.29984894599900819;
     p[7].im = -1.1667612729256689;
     p[8].re = -0.90871411613363973;
     p[8].im =  0.0;
   } break;
 case 18:
   {
     p[0].re = -0.90670043241627751;
     p[0].im = -0.064279241063930695;
     p[1].re = -0.8939764278132456;
     p[1].im = -0.19303746408947586;
     p[2].re = -0.86810955036288295;
     p[2].im = -0.32242049251632576;
     p[3].re = -0.8281885016242837;
     p[3].im = -0.45293856978159169;
     p[4].re = -0.77262850307395592;
     p[4].im = -0.58527781620866404;
     p[5].re = -0.69878214450052734;
     p[5].im = -0.72046965097266302;
     p[6].re = -0.60204826680906443;
     p[6].im = -0.86027089618936647;
     p[7].re = -0.47342680699161516;
     p[7].im = -1.0082343003148011;
     p[8].re = -0.28975920298804897;
     p[8].im = -1.174183010600059;
   } break;
 case 19:
   {
     p[0].re = -0.90219376393906603;
     p[0].im = -0.12195683818720265;
     p[1].re = -0.8849290585034385;
     p[1].im = -0.24425907575498182;
     p[2].re = -0.85557687656184211;
     p[2].im = -0.36729258963998723;
     p[3].re = -0.81317255515781972;
     p[3].im = -0.4915365035562459;
     p[4].re = -0.7561260971541629;
     p[4].im = -0.61764839179701791;
     p[5].re = -0.68184244129124416;
     p[5].im = -0.74662723579477608;
     p[6].re = -0.58586133212178326;
     p[6].im = -0.88018171310145665;
     p[7].re = -0.45950434497309883;
     p[7].im = -1.0217687769126713;
     p[8].re = -0.28048668514393699;
     p[8].im = -1.1809316284532918;
     p[9].re = -0.90789342178994048;
     p[9].im =  0.0;
   } break;
 case 20:
   {
     p[0].re = -0.90625701155767713;
     p[0].im = -0.057961780277849519;
     p[1].re = -0.89591509419257687;
     p[1].im = -0.1740317175918705;
     p[2].re = -0.87495603166733327;
     p[2].im = -0.29055592965679078;
     p[3].re = -0.84279074799566711;
     p[3].im = -0.40789173262919343;
     p[4].re = -0.79842511912906067;
     p[4].im = -0.52649423888171321;
     p[5].re = -0.74027803096467693;
     p[5].im = -0.64699752376052289;
     p[6].re = -0.66581205448299341;
     p[6].im = -0.7703721701100763;
     p[7].re = -0.57070268069157137;
     p[7].im = -0.89828290664682553;
     p[8].re = -0.44657006982051495;
     p[8].im = -1.0340977025608429;
     p[9].re = -0.27192995802516529;
     p[9].im = -1.187099379810886;
   } break;
 case 21:
   {
     p[0].re  = -0.90254280731926961;
     p[0].im  = -0.11052525727898564;
     p[1].re  = -0.88838081066644503;
     p[1].im  = -0.22130692150843503;
     p[2].re  = -0.86439158136432048;
     p[2].im  = -0.33262585125221872;
     p[3].re  = -0.82994354706744444;
     p[3].im  = -0.44481777394079564;
     p[4].re  = -0.78402879804083414;
     p[4].im  = -0.5583186348022855;
     p[5].re  = -0.72508396871066128;
     p[5].im  = -0.67374260630243821;
     p[6].re  = -0.6506315378609463;
     p[6].im  = -0.79203493426294913;
     p[7].re  = -0.55647664889185622;
     p[7].im  = -0.91481984058467236;
     p[8].re  = -0.43451689068152716;
     p[8].im  = -1.0453822558569865;
     p[9].re  = -0.26400415958340312;
     p[9].im  = -1.1927620319480525;
     p[10].re = -0.90722626531429573;
     p[10].im =  0.0;
   } break;
 case 22:
   {
     p[0].re  = -0.90587022699308728;
     p[0].im  = -0.052774908289999048;
     p[1].re  = -0.89729831381535308;
     p[1].im  = -0.15843519122898655;
     p[2].re  = -0.87996614556401764;
     p[2].im  = -0.26443630392015349;
     p[3].re  = -0.85347540368516872;
     p[3].im  = -0.37103893194823201;
     p[4].re  = -0.81716820884627206;
     p[4].im  = -0.47856194922027812;
     p[5].re  = -0.77003329305568169;
     p[5].im  = -0.58742554263511537;
     p[6].re  = -0.71053054564187856;
     p[6].im  = -0.69822662659245238;
     p[7].re  = -0.63624276832678273;
     p[7].im  = -0.81188750402463472;
     p[8].re  = -0.54309830563063033;
     p[8].im  = -0.92999478244398726;
     p[9].re  = -0.42325287456426286;
     p[9].im  = -1.0557556052275459;
     p[10].re = -0.25663769879393178;
     p[10].im = -1.1979824335552129;
   } break;
 case 23:
   {
     p[0].re  = -0.90275649799125046;
     p[0].im  = -0.1010534335314045;
     p[1].re  = -0.89092832424712509;
     p[1].im  = -0.20230246993812234;
     p[2].re  = -0.87094693955874158;
     p[2].im  = -0.30395819939500418;
     p[3].re  = -0.84238059480211269;
     p[3].im  = -0.4062657948237603;
     p[4].re  = -0.80455616420531761;
     p[4].im  = -0.50953059122272581;
     p[5].re  = -0.75646601468298802;
     p[5].im  = -0.61415948594760317;
     p[6].re  = -0.6965966033912705;
     p[6].im  = -0.72073413747530468;
     p[7].re  = -0.62259032287713423;
     p[7].im  = -0.83015583028129802;
     p[8].re  = -0.53049224638101922;
     p[8].im  = -0.94397603640183003;
     p[9].re  = -0.41269866175101488;
     p[9].im  = -1.0653287944755137;
     p[10].re = -0.24976972022089561;
     p[10].im = -1.2028131878706978;
     p[11].re = -0.90667324763249879;
     p[11].im =  0.0;
   } break;
 case 24:
   {
     p[0].re  = -0.90553123633727739;
     p[0].im  = -0.0484400665404787;
     p[1].re  = -0.89831051043978727;
     p[1].im  = -0.14540561338736102;
     p[2].re  = -0.88373580345557068;
     p[2].im  = -0.2426335234401383;
     p[3].re  = -0.86152783040163539;
     p[3].im  = -0.34032021126186246;
     p[4].re  = -0.83123264668132402;
     p[4].im  = -0.43869859335973055;
     p[5].re  = -0.79216954623434921;
     p[5].im  = -0.53806284909680169;
     p[6].re  = -0.74333922850885292;
     p[6].im  = -0.63880842162225682;
     p[7].re  = -0.68325658035365211;
     p[7].im  = -0.74150326950916512;
     p[8].re  = -0.6096221567378336;
     p[8].im  = -0.84702924330772023;
     p[9].re  = -0.51859145748203173;
     p[9].im  = -0.95690483852590547;
     p[10].re = -0.40278538551975179;
     p[10].im = -1.0741951965186747;
     p[11].re = -0.24334813375248696;
     p[11].im = -1.2072986837319726;}
   break;
 case 25:
   {
     p[0].re  = -0.90288333902280205;
     p[0].im  = -0.093077131185102968;
     p[1].re  = -0.89285514598835491;
     p[1].im  = -0.18630689698043007;
     p[2].re  = -0.8759497989677858;
     p[2].im  = -0.27985213217714089;
     p[3].re  = -0.85186168865540202;
     p[3].im  = -0.37389778759075948;
     p[4].re  = -0.820122604393688;
     p[4].im  = -0.46866685746569664;
     p[5].re  = -0.78004962781864973;
     p[5].im  = -0.56444412103497099;
     p[6].re  = -0.73065492718499681;
     p[6].im  = -0.66161496473577486;
     p[7].re  = -0.6704827128029559;
     p[7].im  = -0.76073488581678395;
     p[8].re  = -0.59728986613355572;
     p[8].im  = -0.86266763303880289;
     p[9].re  = -0.50733628610784687;
     p[9].im  = -0.96890063053448683;
     p[10].re = -0.39345298781910798;
     p[10].im = -1.0824339271738317;
     p[11].re = -0.2373280669322029;
     p[11].im = -1.2114766583825654;
     p[12].re = -0.90620738718117089;
     p[12].im =  0.0;
   } break;

  } // end of switch(N)


  rescalePoles( amp2dB(sqrt(0.5)) );

  stateIsDirty = false;
}









