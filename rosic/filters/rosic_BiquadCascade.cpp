#include "rosic_BiquadCascade.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

BiquadCascade::BiquadCascade(int newMaxNumStages)
{
  if( newMaxNumStages >= 1 )
    maxNumStages = newMaxNumStages;
  else
    maxNumStages = 12;

  // allcocate memory for coefficients and buffers:
  a0 = new double[maxNumStages];
  a1 = new double[maxNumStages];
  a2 = new double[maxNumStages];
  b0 = new double[maxNumStages];
  b1 = new double[maxNumStages];
  b2 = new double[maxNumStages];
  x1 = new double[maxNumStages];
  x2 = new double[maxNumStages];
  y1 = new double[maxNumStages];
  y2 = new double[maxNumStages];
  g1 = new double[maxNumStages];
  g2 = new double[maxNumStages];

  initBiquadCoeffs();        // initializes the filter-coefficents
  reset();                   // resets the filters memory buffers (to 0)
  numStages = maxNumStages;  // 
  gain      = 1.0;
  setSampleRate(44100.0);
}

BiquadCascade::~BiquadCascade()
{
  // free dynamically allocated memory:
  delete[] a0;
  delete[] a1;
  delete[] a2;
  delete[] b0;
  delete[] b1;
  delete[] b2;
  delete[] x1;
  delete[] x2;
  delete[] y1;
  delete[] y2;
  delete[] g1;
  delete[] g2;
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void BiquadCascade::setSampleRate(double newSampleRate)
{
  if( newSampleRate > 0.0 )
    sampleRate = newSampleRate;

  sampleRateRec = 1.0 / sampleRate;
}

void BiquadCascade::setNumStages(int newNumStages)
{
  if( (newNumStages >= 0 ) && (newNumStages <= maxNumStages) )
    numStages = newNumStages;
  else
    DEBUG_BREAK;
  reset();
}

void BiquadCascade::setOrder(int newOrder)
{
  if( isEven(newOrder) )
    setNumStages(newOrder/2);
  else
    setNumStages( (newOrder+1)/2 );
}

void BiquadCascade::setGlobalGainFactor(double newGain)
{
  gain = newGain;
}

void BiquadCascade::copySettingsFrom(BiquadCascade *other)
{
  setNumStages(other->getNumStages());

  double *pB0 = other->getAddressB0();
  double *pB1 = other->getAddressB1();
  double *pB2 = other->getAddressB2();
  double *pA0 = other->getAddressA0();
  double *pA1 = other->getAddressA1();
  double *pA2 = other->getAddressA2();
  for(int s=0; s<numStages; s++)
  {
    b0[s] = pB0[s]; b1[s] = pB1[s]; b2[s] = pB2[s]; 
    a0[s] = pA0[s]; a1[s] = pA1[s]; a2[s] = pA2[s]; 
  }
}

void BiquadCascade::multiplyFeedforwardCoeffsBy(double factor)
{
  for(int s=0; s<numStages; s++)
  {
    b0[s] *= factor; 
    b1[s] *= factor; 
    b2[s] *= factor; 
  }
}

//-------------------------------------------------------------------------------------------------
// inquiry:

void BiquadCascade::getMagnitudeResponse(double *frequencies, 
                                         double *magnitudes,
                                         int     numBins, 
                                         bool    inDecibels,
                                         bool    accumulate)
{
  long double num;  // numerator of the squared magnitude of one stage
  long double den;  // denominator of the squared magnitude of one stage
  long double accu; // accumulator which multiplicatively accumulates the squared magnitude of the individual stages

  double omega;     // current normalized radian frequency
  double cosOmega;  // cosine of the current omega
  double cos2Omega; // cosine of twice the current omega

  int k;            // index for the current frequency bin    
  int s;            // index for the current biquad-stage

  for(k=0; k<numBins; k++)
  {
    // convert frequency in Hz to normalized radian frequency omega:
    omega = (2.0*PI)*(frequencies[k]*sampleRateRec);

    // calculate cos(omega) and cos(2*omega) because these are needed in the 
    // numerator and denominator as well:
    cosOmega  = cos(omega);
    cos2Omega = cos(2*omega);

    // accumulate the squared magnitude responses of the individual 
    // biquad-stages:
    accu = 1.0;
    for(s=0; s<numStages; s++)
    {
      // calculate the numerator of the squared magnitude of stage s:
      num = b0[s]*b0[s] + b1[s]*b1[s] + b2[s]*b2[s]
      + 2*cosOmega*(b0[s]*b1[s] + b1[s]*b2[s])
        + 2*cos2Omega*b0[s]*b2[s];

      // calculate the denominator of the squared magnitude of stage s:
      den = a0[s]*a0[s] + a1[s]*a1[s] + a2[s]*a2[s]
      + 2*cosOmega*(a0[s]*a1[s] + a1[s]*a2[s])
        + 2*cos2Omega*a0[s]*a2[s];

      // multiply the accumulator with the squared magnitude of stage s:
      accu *= (num/den);

    } // end of "for(s=0; s<numStages; s++)"

    // take the square root of the accumulated squared magnitude response - this
    // is the desired magnitude of the biquad cascade at frequencies[k]:
    accu = sqrt(accu);

    // use zero as value for frequencies above sampleRate/2:
    if( omega > PI )
      accu = 0.0;

    // store the calculated value in the array "magnitudes", taking into acount,
    // if the value should be in dB or not and if it should accumulate to what's 
    // already there or not:
    if( !inDecibels && !accumulate )
      magnitudes[k]  = accu;
    else if( !inDecibels && accumulate )
      magnitudes[k] *= accu;
    else if( inDecibels && !accumulate )
      magnitudes[k]  = amp2dBWithCheck(accu, 0.0000001); // clip at -140 dB
    else if( inDecibels && accumulate )
      magnitudes[k] += amp2dBWithCheck(accu, 0.0000001);


    //magnitudes[k] = clip(magnitudes[k], -200.0, 200.0);

  } // end of " for(k=0; k<numBins; k++)"

  /*
  // convert to decibels if this is desired:
  if( inDecibels == true)
  for(k=0; k<numBins; k++)
  magnitudes[k] = 20 * log10(magnitudes[k]);
  //magnitudes[k] = 16.0; // test
  */

}

void BiquadCascade::getMagnitudeResponse(float *frequencies, 
                                         float *magnitudes,
                                         int    numBins, 
                                         bool   inDecibels,
                                         bool   accumulate)
{
  long double num;  // numerator of the squared magnitude of one stage
  long double den;  // denominator of the squared magnitude of one stage
  long double accu; // accumulator which multiplicatively accumulates
                    // the squared magnitude of the individual stages

  double omega;     // current normalized radian frequency
  double cosOmega;  // cosine of the current omega
  double cos2Omega; // cosine of twice the current omega

  int k;            // index for the current frequency bin    
  int s;            // index for the current biquad-stage

  for(k=0; k<numBins; k++)
  {
    // convert frequency in Hz to normalized radian frequency omega:
    omega = 2*PI*frequencies[k]/sampleRate;

    // calculate cos(omega) and cos(2*omega) because these are needed in the 
    // numerator and denominator as well:
    cosOmega  = cos(omega);
    cos2Omega = cos(2*omega);

    // accumulate the squared magnitude responses of the individual 
    // biquad-stages:
    accu = 1.0;
    for(s=0; s<numStages; s++)
    {
      // calculate the numerator of the squared magnitude of stage s:
      num = b0[s]*b0[s] + b1[s]*b1[s] + b2[s]*b2[s]
      + 2*cosOmega*(b0[s]*b1[s] + b1[s]*b2[s])
        + 2*cos2Omega*b0[s]*b2[s];

      // calculate the denominator of the squared magnitude of stage s:
      den = a0[s]*a0[s] + a1[s]*a1[s] + a2[s]*a2[s]
      + 2*cosOmega*(a0[s]*a1[s] + a1[s]*a2[s])
        + 2*cos2Omega*a0[s]*a2[s];

      // multiply the accumulator with the squared magnitude of stage s:
      accu *= (num/den);

    } // end of "for(s=0; s<numStages; s++)"

    // take the square root of the accumulated squared magnitude response - this
    // is the desired magnitude of the biquad cascade at frequencies[k]:
    accu = sqrt(accu);

    // use zero as value for frequencies above sampleRate/2:
    if( omega > PI )
      accu = 0.0;

    // store the calculated value in the array "magnitudes", taking into acount,
    // if the value should be in dB or not and if it should accumulate to what's 
    // already there or not:
    if( !inDecibels && !accumulate )
      magnitudes[k]  = (float) accu;
    else if( !inDecibels && accumulate )
      magnitudes[k] *= (float) accu;
    else if( inDecibels && !accumulate )
      magnitudes[k]  = (float) amp2dBWithCheck(accu, 0.00001);
    else if( inDecibels && accumulate )
      magnitudes[k] += (float) amp2dBWithCheck(accu, 0.00001);

  } // end of " for(k=0; k<numBins; k++)"

  /*
  // convert to decibels if this is desired:
  if( inDecibels == true)
  for(k=0; k<numBins; k++)
  magnitudes[k] = 20 * log10(magnitudes[k]);
  //magnitudes[k] = 16.0; // test
  */

}

//-------------------------------------------------------------------------------------------------
// others:

void BiquadCascade::initBiquadCoeffs()
{
  int i;
  for (i=0; i<maxNumStages; i++)
  {
    b0[i] = 1.0;
    b1[i] = 0.0;
    b2[i] = 0.0;
    a0[i] = 1.0;
    a1[i] = 0.0;
    a2[i] = 0.0;
  }
}

void BiquadCascade::reset()
{
  int i;
  for (i=0; i<maxNumStages; i++)
  {
    x2[i] = 0.0;
    x1[i] = 0.0;
    y2[i] = 0.0;
    y1[i] = 0.0;
    g2[i] = 0.0;
    g1[i] = 0.0;
  }
}




