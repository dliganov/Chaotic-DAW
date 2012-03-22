#include "rosic_TwoPoleFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

TwoPoleFilter::TwoPoleFilter()
{
  sampleRate = 44100.0;
  mode       = PEAK;
  frequency  = 1000.0;
  gain       = 0.0;
  bandwidth  = 1.0;
  radius     = 0.9;
  updateCoeffs();
}

TwoPoleFilter::~TwoPoleFilter()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void TwoPoleFilter::setSampleRate(double newSampleRate, bool updateCoefficients)
{
  if( newSampleRate <= 0.0 )
  {
    DEBUG_BREAK;
    return;
  }
  sampleRate = newSampleRate;
  if( updateCoefficients == true )
    updateCoeffs();
}

void TwoPoleFilter::setMode(int newMode, bool updateCoefficients)
{
  if( newMode >= BYPASS && newMode < NUM_FILTER_MODES )
    mode = newMode;
  if( updateCoefficients == true )
    updateCoeffs();
}

void TwoPoleFilter::setFrequency(double newFrequency, bool updateCoefficients)
{
  frequency = clip(newFrequency, 2.0, 20000.0);
  if( updateCoefficients == true )
    updateCoeffs();
}

void TwoPoleFilter::setBandwidth(double newBandwidth, bool updateCoefficients)
{
  bandwidth = clip(newBandwidth, 0.25, 6.0);
  //bandwidth = rmax(0.001, newBandwidth);
  if( updateCoefficients == true )
    updateCoeffs();
}

void TwoPoleFilter::setRadius(double newRadius, bool updateCoefficients)
{
  radius = newRadius;
  if( updateCoefficients == true )
    updateCoeffs();
}

void TwoPoleFilter::setGain(double newGain, bool updateCoefficients)
{
  gain = newGain;
  if( updateCoefficients == true )
    updateCoeffs();
}

void TwoPoleFilter::setParameters(int newMode, double newFrequency, double newGain,             
                                  double newBandwidth, bool updateCoefficients)
{
  setMode(newMode, false);
  setFrequency(newFrequency, false);
  setGain(newGain, false);
  setBandwidth(newBandwidth, updateCoefficients);
}

void TwoPoleFilter::setLowerBandedgeFrequency(double newLowerBandedgeFrequency, 
                                              bool updateCoefficients)
{
  setBandwidth(2.0*log2(frequency/newLowerBandedgeFrequency));
}

void TwoPoleFilter::setUpperBandedgeFrequency(double newUpperBandedgeFrequency,                                                    
                                              bool updateCoefficients)
{
  setBandwidth(2.0*log2(newUpperBandedgeFrequency/frequency));
}

//-------------------------------------------------------------------------------------------------
// inquiry:

bool TwoPoleFilter::doesModeSupportBandwidth() const
{
  if(  mode == PEAK || mode == BANDREJECT || mode == LOW_SHELF || mode == HIGH_SHELF 
    || mode == BANDPASS )
    return true;
  else 
    return false;
}

bool TwoPoleFilter::doesModeSupportGain() const
{
  if(  mode == PEAK || mode == LOW_SHELF || mode == HIGH_SHELF 
    || mode == LOWPASS12  || mode == HIGHPASS12)
    return true;
  else 
    return false;
}

bool TwoPoleFilter::doesModeSupportRadius() const
{
  if( mode == REAL_POLE || mode == REAL_ZERO || mode == POLE_PAIR || mode == ZERO_PAIR )
    return true;
  else 
    return false;
}

//-------------------------------------------------------------------------------------------------
// others:

void TwoPoleFilter::updateCoeffs()
{
  //double w0 = 2.0*PI*frequency/sampleRate;
  //double q2  = 1.0 / (2.0*sinh( 0.5*log(2.0) * bandwidth * w0/sin(w0) ) );
  double q2  = 1.0 / (2.0*sinh( 0.5*log(2.0) * bandwidth ));
  //q = sqrt(0.5);

  switch(mode)
  {
  case BYPASS:
      BiquadDesigner::makeBypassBiquad(b0, b1, b2, a1, a2);
      break;
  case PEAK:
    BiquadDesigner::calculatePrescribedNyquistGainEqCoeffs(b0, b1, b2, a1, a2,
      1.0/sampleRate, frequency, bandwidth, dB2amp(gain), 1.0);
      break;
  case LOW_SHELF:
    BiquadDesigner::calculateCookbookLowShelvCoeffs(b0, b1, b2, a1, a2, 1.0/sampleRate, 
      frequency, q2, dB2amp(0.5*gain));
      break;
  case HIGH_SHELF:
    BiquadDesigner::calculateCookbookHighShelvCoeffs(b0, b1, b2, a1, a2, 1.0/sampleRate, 
      frequency, q2, dB2amp(0.5*gain));
      break;
      /*
  case LOWPASS6:
    BiquadDesigner::calculateFirstOrderLowpassCoeffs(b0, b1, b2, a1, a2, 1.0/sampleRate, 
      frequency);
      break;*/
  case LOWPASS6:
    BiquadDesigner::calculateFirstOrderLowpassCoeffsBilinear(b0, b1, b2, a1, a2, 1.0/sampleRate, 
      frequency);
      break;
  case LOWPASS12:
    BiquadDesigner::calculateCookbookLowpassCoeffs(b0, b1, b2, a1, a2, 1.0/sampleRate,      
      frequency, dB2amp(gain));
      break;
  case HIGHPASS6:
    BiquadDesigner::calculateFirstOrderHighpassCoeffsBilinear(b0, b1, b2, a1, a2, 1.0/sampleRate, 
      frequency);
      break;
  case HIGHPASS12:
    BiquadDesigner::calculateCookbookHighpassCoeffs(b0, b1, b2, a1, a2, 1.0/sampleRate, 
      frequency, dB2amp(gain));
      break;
  case BANDREJECT:
    BiquadDesigner::calculateCookbookBandrejectCoeffsViaBandwidth(b0, b1, b2, a1, a2, 
      1.0/sampleRate, frequency, bandwidth); 
      break;
  case BANDPASS:
    BiquadDesigner::calculateCookbookBandpassConstSkirtCoeffsViaBandwidth(b0, b1, b2, a1, a2, 
      1.0/sampleRate, frequency, bandwidth); 
      break;
      /*
  case LOWHIGH_PASS_CHAIN: 
    {

    }
    break;
    */
  case REAL_POLE:
    BiquadDesigner::calculateOnePoleCoeffs(b0, b1, b2, a1, a2, radius);   
    break;
  case REAL_ZERO:
    BiquadDesigner::calculateOneZeroCoeffs(b0, b1, b2, a1, a2, radius);   
    break;
  case POLE_PAIR:
    {
      double tmpRadius = radius;
      if( fabs(radius) > 0.999999 )
        tmpRadius = sign(radius) * 0.999999;
      BiquadDesigner::calculatePolePairCoeffs(b0, b1, b2, a1, a2, tmpRadius, 
        2*PI*frequency/sampleRate);   
    }
    break;
  case ZERO_PAIR:
    BiquadDesigner::calculateZeroPairCoeffs(b0, b1, b2, a1, a2, radius, 2*PI*frequency/sampleRate);   
    break;

      /*
  case BANDREJECT:
    BiquadDesigner::calculatePrescribedNyquistGainEqCoeffs(b0, b1, b2, a1, a2,
      1.0/sampleRate, frequency, bandwidth, 0.0, 1.0);
      break; */
  default:
    BiquadDesigner::calculatePrescribedNyquistGainEqCoeffs(b0, b1, b2, a1, a2,
      1.0/sampleRate, frequency, bandwidth, dB2amp(gain), 1.0);
  }
}



