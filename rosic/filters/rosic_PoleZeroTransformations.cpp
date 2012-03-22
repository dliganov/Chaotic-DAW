#include "rosic_PoleZeroTransformations.h"
using namespace rosic;

void rosic::prototypeToAnalogLowpass(Complex *poles, int numPoles, Complex *zeros, int numZeros,                      
                                     double *gain, double targetCutoff)
{
  int i;
  for(i=0; i<numPoles; i++)
    poles[i] = poles[i] * targetCutoff;
  for(i=0; i<numZeros; i++)
    zeros[i] = zeros[i] * targetCutoff;

  // \todo: adjust gain
}

void rosic::sPlanePrototypeToLowpass(Complex *prototypePoles, Complex *prototypeZeros, 
                                     Complex *targetPoles, Complex *targetZeros, 
                                     int prototypeOrder, double targetCutoff)
{
  for(int i=0; i<prototypeOrder; i++)
  {
    targetPoles[i] = prototypePoles[i] * targetCutoff;
    targetZeros[i] = prototypeZeros[i] * targetCutoff;
  }
}

void rosic::prototypeToAnalogHighpass(Complex *poles, int numPoles, Complex *zeros, int numZeros,              
                                      double *gain, double targetCutoff)
{
  int i;
  for(i=0; i<numPoles; i++)
    poles[i] = targetCutoff / poles[i];
  for(i=0; i<numZeros; i++)
    zeros[i] = targetCutoff / zeros[i];

  // there are numPoles-numZeros zeros at infinity in the lowpass-prototype and for for each such
  // zero at infinity, we obtain a zero at s=0 in the highpass filter:
  for(i=numZeros; i<numPoles; i++)
    zeros[i] = Complex(0.0, 0.0);

  // \todo: adjust gain
}

void rosic::sPlanePrototypeToHighpass(Complex *prototypePoles, Complex *prototypeZeros, 
                                      Complex *targetPoles, Complex *targetZeros, 
                                      int prototypeOrder, double targetCutoff)
{
  for(int i=0; i<prototypeOrder; i++)
  {
    targetPoles[i] = targetCutoff / prototypePoles[i];
    if( prototypeZeros[i].isInfinite() )
      targetZeros[i] = Complex(0.0, 0.0);
    else
      targetZeros[i] = targetCutoff / prototypeZeros[i];
  }
}

void rosic::prototypeToAnalogHighShelv(Complex *poles, int numPoles, Complex *zeros, int numZeros,
                                       double *gain, double targetCutoff)
{
  int i;
  for(i=0; i<numPoles; i++)
    poles[i] = zeros[i] * targetCutoff;
  for(i=0; i<numZeros; i++)
    zeros[i] = poles[i] * targetCutoff;

  // \todo: adjust gain - huh= this is not supposed to work when overwriting the poles in the first array
}

void rosic::sPlanePrototypeToHighShelv(Complex *prototypePoles, Complex *prototypeZeros, 
                                       Complex *targetPoles, Complex *targetZeros, 
                                       int prototypeOrder, double targetCutoff)
{
  for(int i=0; i<prototypeOrder; i++)
  {
    targetPoles[i] = prototypeZeros[i] * targetCutoff;
    targetZeros[i] = prototypePoles[i] * targetCutoff;
  }
}

void rosic::prototypeToAnalogBandpass(Complex *poles, int numPoles, Complex *zeros, int numZeros, 
                                      double *gain, 
                                      double targetLowCutoff, double targetHighCutoff)
{
  double wc = sqrt(targetLowCutoff*targetHighCutoff); // center (radian) frequency
  double bw = targetHighCutoff - targetLowCutoff;     // bandwidth
  Complex* tmpPoles = new Complex[2*numPoles];
  Complex* tmpZeros = new Complex[2*numPoles];

  // for each pole (or zero) in the prototype, we obtain a pair of poles (or zeros) in the 
  // bandpass-filter:
  int     i;
  Complex tmp1, tmp2;
  for(i=0; i<numPoles; i++)
  {
    tmp1                     = 0.5 * bw * poles[i];
    tmp2                     = -bw * poles[i];
    tmp2                     = 0.25 * tmp2*tmp2;
    tmp2                     = sqrtC(tmp2 - wc*wc);
    //tmpPoles[2*i]   = tmp1 + tmp2;
    //tmpPoles[2*i+1] = tmp1 - tmp2;
    tmpPoles[i]              = tmp1 + tmp2;
    tmpPoles[2*numPoles-i-1] = tmp1 - tmp2;
  }
  //for(i=0; i<numZeros; i++)
  for(i=0; i<numZeros; i++)
  {
    tmp1                     = 0.5 * bw * zeros[i];
    tmp2                     = -bw * zeros[i];
    tmp2                     = 0.25 * tmp2*tmp2;
    tmp2                     = sqrtC(tmp2 - wc*wc);
    //tmpZeros[2*i]   = tmp1 + tmp2;
    //tmpZeros[2*i+1] = tmp1 - tmp2;
    tmpZeros[i]              = tmp1 + tmp2;
    tmpZeros[2*numPoles-i-1] = tmp1 - tmp2;
  }

  // put additional zeros at 0.0 and infinity when there are less finite zeros than finite poles:
  //for(i=numZeros; i<(numPoles-numZeros); i++)
  for(i=numZeros; i<numPoles; i++)
  {
    //tmpZeros[2*i]   = 0.0;
    //tmpZeros[2*i+1] = INF;
    tmpZeros[i]              = 0.0;
    tmpZeros[2*numPoles-i-1] = INF;
  }

  //int         order = 2 * max(numPoles, numZeros);

  // copy the content of the temporary arrays into the output arrays:
  for(i=0; i<2*numPoles; i++)
  {
    zeros[i] = tmpZeros[i];
    poles[i] = tmpPoles[i];
  }

  // \todo: adjust gain

  // for debug:
  Complex pDbg[40];
  Complex zDbg[40];
  int k;
  for(k=0; k<2*numPoles; k++)
  {
    pDbg[k] = poles[k];
    zDbg[k] = zeros[k];
  }

  // free dynamically allocate memory:
  delete[] tmpPoles;
  delete[] tmpZeros;
}


void rosic::sPlanePrototypeToBandpass(Complex *prototypePoles, Complex *prototypeZeros, 
                                      Complex *targetPoles, Complex *targetZeros, 
                                      int prototypeOrder, 
                                      double targetLowerCutoff, double targetUpperCutoff)
{
  double wc = sqrt(targetLowerCutoff*targetUpperCutoff); // center (radian) frequency
  double bw = targetUpperCutoff - targetLowerCutoff;     // bandwidth

  // for each pole (or zero) in the prototype, we obtain a pair of poles (or zeros) in the 
  // bandpass-filter:
  Complex tmp1, tmp2;
  int k;
  for(k=0; k<prototypeOrder; k++)
  {
    // transform poles:
    tmp1 = 0.5 * bw * prototypePoles[k];
    tmp2 = -bw * prototypePoles[k];
    tmp2 = 0.25 * tmp2*tmp2;
    tmp2 = sqrtC(tmp2 - wc*wc);
    targetPoles[2*k]   = tmp1 + tmp2;
    targetPoles[2*k+1] = tmp1 - tmp2;

    // transform zeros:
    if( prototypeZeros[k].isInfinite() )
    {
      targetZeros[2*k]   = INF;
      targetZeros[2*k+1] = 0.0;
    }
    else
    {
      tmp1 = 0.5 * bw * prototypeZeros[k];
      tmp2 = -bw * prototypeZeros[k];
      tmp2 = 0.25 * tmp2*tmp2;
      tmp2 = sqrtC(tmp2 - wc*wc);
      targetZeros[2*k]   = tmp1 + tmp2;
      targetZeros[2*k+1] = tmp1 - tmp2;
    }
  }

  // re-arrange poles and zeros such that complex conjugate pairs (again) occupy successive slots:
  k = 1;
  while( k+1 < 2*prototypeOrder )
  {
    swap(targetPoles[k], targetPoles[k+1]);
    swap(targetZeros[k], targetZeros[k+1]);
    k += 4;
  }
}

void rosic::prototypeToAnalogBandstop(Complex *poles, int numPoles, Complex *zeros, int numZeros,
                                      double *gain, 
                                      double targetLowCutoff, double targetHighCutoff)
{
  double wc = sqrt(targetLowCutoff*targetHighCutoff); // center (radian) frequency
  double bw = targetHighCutoff - targetLowCutoff;     // bandwidth

  int         order = 2 * rmax(numPoles, numZeros);
  Complex* tmpPoles = new Complex[order];
  Complex* tmpZeros = new Complex[order];

  // for each pole (or zero) in the prototype, we obtain a pair of poles (or zeros) in the 
  // bandpass-filter:
  int     i;
  Complex tmp1, tmp2;
  for(i=0; i<numPoles; i++)
  {
    tmp1            = 0.5*bw / poles[i];
    tmp2            = -bw / poles[i];
    tmp2            = 0.25 * tmp2*tmp2;
    tmp2            = sqrtC(tmp2 - wc*wc);
    tmpPoles[2*i]   = tmp1 + tmp2;
    tmpPoles[2*i+1] = tmp1 - tmp2;
  }
  for(i=0; i<numZeros; i++)
  {
    tmp1            = 0.5*bw / zeros[i];
    tmp2            = -bw / zeros[i];
    tmp2            = 0.25 * tmp2*tmp2;
    tmp2            = sqrtC(tmp2 - wc*wc);
    tmpZeros[2*i]   = tmp1 + tmp2;
    tmpZeros[2*i+1] = tmp1 - tmp2;
  }

  // put additional zeros at s=j*wc and s=-j*wc when there are less finite zeros than finite poles:
  Complex j(0.0, 1.0); // imaginary unit
  for(i=numZeros; i<(numPoles-numZeros); i++)
  {
    tmpZeros[2*i]   =  j*wc;
    tmpZeros[2*i+1] = -j*wc;
  }

  // copy the content of the temporary arrays into the output arrays:
  for(i=0; i<order; i++)
  {
    zeros[i] = tmpZeros[i];
    poles[i] = tmpPoles[i];
  }

  // free dynamically allocate memory:
  delete[] tmpPoles;
  delete[] tmpZeros;

  // \todo: adjust gain
}

void rosic::sPlanePrototypeToBandreject(Complex *prototypePoles, Complex *prototypeZeros, 
                                        Complex *targetPoles, Complex *targetZeros, 
                                        int prototypeOrder, 
                                        double targetLowerCutoff, double targetUpperCutoff)
{
  double wc = sqrt(targetLowerCutoff*targetUpperCutoff); // center (radian) frequency
  double bw = targetUpperCutoff - targetLowerCutoff;     // bandwidth

  // for each pole (or zero) in the prototype, we obtain a pair of poles (or zeros) in the 
  // bandpass-filter:
  Complex tmp1, tmp2;  
  int k;
  for(k=0; k<prototypeOrder; k++)
  {
    // transform poles:
    tmp1 = 0.5 * bw / prototypePoles[k];
    tmp2 = -bw / prototypePoles[k];
    tmp2 = 0.25 * tmp2*tmp2;
    tmp2 = sqrtC(tmp2 - wc*wc);
    targetPoles[2*k]   = tmp1 + tmp2;
    targetPoles[2*k+1] = tmp1 - tmp2;

    // transform zeros:
    Complex j(0.0, 1.0); // imaginary unit
    if( prototypeZeros[k].isInfinite() )
    {
      targetZeros[2*k]   =  j*wc;
      targetZeros[2*k+1] = -j*wc;
    }
    else
    {
      tmp1 = 0.5 * bw / prototypeZeros[k];
      tmp2 = -bw / prototypeZeros[k];
      tmp2 = 0.25 * tmp2*tmp2;
      tmp2 = sqrtC(tmp2 - wc*wc);
      targetZeros[2*k]   = tmp1 + tmp2;
      targetZeros[2*k+1] = tmp1 - tmp2;
    }
  }

  // re-arrange poles and zeros such that complex conjugate pairs (again) occupy successive slots:
  k = 1;
  while( k+1 < 2*prototypeOrder )
  {
    swap(targetPoles[k], targetPoles[k+1]);
    int kp = (k-1)/4;
    if( !prototypeZeros[kp].isInfinite() )
      swap(targetZeros[k], targetZeros[k+1]); // don't want to swap the generated zeros at +-j*wc
    k += 4;
  }
}

void rosic::bilinearAnalogToDigital(Complex *poles, int numPoles, Complex *zeros, int numZeros,
                                    double sampleRate, double *gain)
{
  int     i;
  Complex z;
  double  scaler = 0.5/sampleRate;

  // for debug:
  Complex pDbg[40];
  Complex zDbg[40];
  int k;
  for(k=0; k<numPoles; k++)
  {
    pDbg[k] = poles[k];
    zDbg[k] = zeros[k];
  }

  for(i=0; i<numPoles; i++)
  {
    z        = scaler * poles[i];
    poles[i] = (1.0+z)/(1.0-z);
  }
  /*
  for(i=0; i<numZeros; i++)
  {  
    z        = scaler * zeros[i];    
    zeros[i] = (1.0+z)/(1.0-z);
  }
  // put additional zeros at z=-1.0 for all zeros at s=inf in the prototye:
  for(i=numZeros; i<numPoles; i++)
    zeros[i] = -1.0;
  */

  for(i=0; i<numZeros; i++)
  {
    if( zeros[i].isInfinite() )
      zeros[i] = Complex(-1.0, 0.0);
    else
    {
      z        = scaler * zeros[i];
      zeros[i] = (1.0+z)/(1.0-z);
    }
  }

  // for debug:
  for(k=0; k<numPoles; k++)
  {
    pDbg[k] = poles[k];
    zDbg[k] = zeros[k];
  }
  int dummy = 0;
}


