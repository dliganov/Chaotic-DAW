#include "rosic_CrossOver.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

CrossOver::CrossOver(int newMaxButterworthOrder) :
// contructor-list for embedded objects:
lowMidLowpassL1(newMaxButterworthOrder), lowMidLowpassL2(newMaxButterworthOrder),
lowMidLowpassR1(newMaxButterworthOrder), lowMidLowpassR2(newMaxButterworthOrder),
lowMidHighpassL1(newMaxButterworthOrder), lowMidHighpassL2(newMaxButterworthOrder),
lowMidHighpassR1(newMaxButterworthOrder), lowMidHighpassR2(newMaxButterworthOrder),
midHighLowpassL1(newMaxButterworthOrder), midHighLowpassL2(newMaxButterworthOrder),
midHighLowpassR1(newMaxButterworthOrder), midHighLowpassR2(newMaxButterworthOrder),
midHighHighpassL1(newMaxButterworthOrder), midHighHighpassL2(newMaxButterworthOrder),
midHighHighpassR1(newMaxButterworthOrder), midHighHighpassR2(newMaxButterworthOrder)
{
  if( newMaxButterworthOrder < 1 )
    DEBUG_BREAK;
  maxButterworthOrder = newMaxButterworthOrder;

  // initialize parameters:
  sampleRate   = 44100.0;
  lowMidFreq   = 250.0;
  midHighFreq  = 4000.0;
  lowMidOrder  = 2;
  midHighOrder = 2;
  twoWayMode   = false;
  monoMode     = false;
  updateFilterCoefficients();
}

CrossOver::~CrossOver()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void CrossOver::setSampleRate(double newSampleRate)
{
  if( newSampleRate > 0.0 && newSampleRate != sampleRate )
  {
    sampleRate = newSampleRate;
    lowMidLowpassL1.setSampleRate(sampleRate);
    lowMidLowpassL2.setSampleRate(sampleRate);
    lowMidLowpassR1.setSampleRate(sampleRate); 
    lowMidLowpassR2.setSampleRate(sampleRate);
    lowMidHighpassL1.setSampleRate(sampleRate); 
    lowMidHighpassL2.setSampleRate(sampleRate);
    lowMidHighpassR1.setSampleRate(sampleRate); 
    lowMidHighpassR2.setSampleRate(sampleRate);  
    midHighLowpassL1.setSampleRate(sampleRate);
    midHighLowpassL2.setSampleRate(sampleRate);
    midHighLowpassR1.setSampleRate(sampleRate);
    midHighLowpassR2.setSampleRate(sampleRate);  
    midHighHighpassL1.setSampleRate(sampleRate); 
    midHighHighpassL2.setSampleRate(sampleRate);
    midHighHighpassR1.setSampleRate(sampleRate);
    midHighHighpassR2.setSampleRate(sampleRate);
  }
  updateFilterCoefficients();
}

void CrossOver::setLowMidFreq(double newLowMidFreq)
{
  if( newLowMidFreq <= 20000.0 )
    lowMidFreq = newLowMidFreq;
  updateFilterCoefficients();
}

void CrossOver::setLowMidOrder(int newLowMidOrder)
{
  if( newLowMidOrder >= 1 && newLowMidOrder <= maxButterworthOrder )
    lowMidOrder = newLowMidOrder;
  updateFilterCoefficients();
}

void CrossOver::setMidHighFreq(double newMidHighFreq)
{
  if( newMidHighFreq <= 20000.0 )
    midHighFreq = newMidHighFreq;
  updateFilterCoefficients();
}

void CrossOver::setMidHighOrder(int newMidHighOrder)
{
  if( newMidHighOrder >= 1 && newMidHighOrder <= maxButterworthOrder )
    midHighOrder = newMidHighOrder;
  updateFilterCoefficients();
}

void CrossOver::setTwoWayMode(bool shouldBeTwoWay)
{
  twoWayMode = shouldBeTwoWay;
}

void CrossOver::setMonoMode(bool shouldBeMono)
{
  monoMode = shouldBeMono;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

double CrossOver::getLowMidFreq()
{
  return lowMidFreq;
}

int CrossOver::getLowMidOrder()
{
  return lowMidOrder;
}

double CrossOver::getMidHighFreq()
{
  return midHighFreq;
}

int CrossOver::getMidHighOrder()
{
  return midHighOrder;
}

bool CrossOver::isInTwoWayMode() const
{
  return twoWayMode;
}

bool CrossOver::isInMonoMode() const
{
  return monoMode;
}

void CrossOver::getLowpassResponse(double *frequencies, double *magnitudes, int numBins)
{
  lowMidLowpassL1.getMagnitudeResponse(frequencies,   magnitudes, numBins, true, false);
  lowMidLowpassL1.getMagnitudeResponse(frequencies,   magnitudes, numBins, true, true);
}

void CrossOver::getBandpassResponse(double *frequencies, double *magnitudes, int numBins)
{
  lowMidHighpassL1.getMagnitudeResponse(frequencies,  magnitudes, numBins, true, false);
  lowMidHighpassL1.getMagnitudeResponse(frequencies,  magnitudes, numBins, true, true);
  if( twoWayMode == false )
  {
    midHighLowpassL1.getMagnitudeResponse(frequencies,  magnitudes, numBins, true, true); 
    midHighLowpassL1.getMagnitudeResponse(frequencies,  magnitudes, numBins, true, true);
  }
}

void CrossOver::getHighpassResponse(double *frequencies, double *magnitudes, int numBins)
{
  if( twoWayMode == false )
  {
    lowMidHighpassL1.getMagnitudeResponse(frequencies,  magnitudes, numBins, true, false);
    lowMidHighpassL1.getMagnitudeResponse(frequencies,  magnitudes, numBins, true, true);
    midHighHighpassL1.getMagnitudeResponse(frequencies, magnitudes, numBins, true, true);
    midHighHighpassL1.getMagnitudeResponse(frequencies, magnitudes, numBins, true, true);
  }
  else
  {
    for(int i=0; i<numBins; i++)
      magnitudes[i] = -120.0; 
      // this value is somewhat arbitrarily chosen such that it is out of the visible range on the 
      // GUI
  }
}

//-------------------------------------------------------------------------------------------------
// others:

void CrossOver::resetBuffers()
{
  lowMidLowpassL1.reset();
  lowMidLowpassL2.reset();
  lowMidLowpassR1.reset(); 
  lowMidLowpassR2.reset();
  lowMidHighpassL1.reset(); 
  lowMidHighpassL2.reset();
  lowMidHighpassR1.reset(); 
  lowMidHighpassR2.reset();  
  midHighLowpassL1.reset();
  midHighLowpassL2.reset();
  midHighLowpassR1.reset();
  midHighLowpassR2.reset();  
  midHighHighpassL1.reset(); 
  midHighHighpassL2.reset();
  midHighHighpassR1.reset();
  midHighHighpassR2.reset();
}

void CrossOver::updateFilterCoefficients()
{
  HighOrderEqualizerDesigner designer;
  designer.setSampleRate(sampleRate);
  designer.setApproximationMethod(PrototypeDesigner::BUTTERWORTH);

  // design the low/mid lowpass and highpass-filter:
  designer.setPrototypeOrder(lowMidOrder);
  designer.setFrequency(lowMidFreq);

  lowMidLowpassL1.setOrder(lowMidOrder);
  designer.setMode(HighOrderEqualizerDesigner::LOWPASS);
  designer.getBiquadCascadeCoefficients(lowMidLowpassL1.getAddressB0(), 
    lowMidLowpassL1.getAddressB1(), lowMidLowpassL1.getAddressB2(), 
    lowMidLowpassL1.getAddressA1(), lowMidLowpassL1.getAddressA2() );
  lowMidLowpassL2.copySettingsFrom(&lowMidLowpassL1);
  lowMidLowpassR1.copySettingsFrom(&lowMidLowpassL1);
  lowMidLowpassR2.copySettingsFrom(&lowMidLowpassL1);

  lowMidHighpassL1.setOrder(lowMidOrder);
  designer.setMode(HighOrderEqualizerDesigner::HIGHPASS);
  designer.getBiquadCascadeCoefficients(lowMidHighpassL1.getAddressB0(), 
    lowMidHighpassL1.getAddressB1(), lowMidHighpassL1.getAddressB2(), 
    lowMidHighpassL1.getAddressA1(), lowMidHighpassL1.getAddressA2() );
  lowMidHighpassL2.copySettingsFrom(&lowMidHighpassL1);
  lowMidHighpassR1.copySettingsFrom(&lowMidHighpassL1);
  lowMidHighpassR2.copySettingsFrom(&lowMidHighpassL1);

  // we need to invert the phase of one of the ouputs for Linkwitz/Riley designs that started from
  // odd order butterworths, do so by using negative b-coefficients on the highpass part:
  if( isOdd(lowMidOrder) )
  {
    lowMidHighpassL2.setGlobalGainFactor(-1.0);
    lowMidHighpassR2.setGlobalGainFactor(-1.0);
  }
  else
  {
    lowMidHighpassL2.setGlobalGainFactor(+1.0);
    lowMidHighpassR2.setGlobalGainFactor(+1.0);
  }

  // design the mid/high lowpass and highpass-filter (same procedure a s above):
  designer.setPrototypeOrder(midHighOrder);
  designer.setFrequency(midHighFreq);

  midHighLowpassL1.setOrder(midHighOrder);
  designer.setMode(HighOrderEqualizerDesigner::LOWPASS);
  designer.getBiquadCascadeCoefficients(midHighLowpassL1.getAddressB0(), 
    midHighLowpassL1.getAddressB1(), midHighLowpassL1.getAddressB2(), 
    midHighLowpassL1.getAddressA1(), midHighLowpassL1.getAddressA2() );
  midHighLowpassL2.copySettingsFrom(&midHighLowpassL1);
  midHighLowpassR1.copySettingsFrom(&midHighLowpassL1);
  midHighLowpassR2.copySettingsFrom(&midHighLowpassL1);

  midHighHighpassL1.setOrder(midHighOrder);
  designer.setMode(HighOrderEqualizerDesigner::HIGHPASS);
  designer.getBiquadCascadeCoefficients(midHighHighpassL1.getAddressB0(), 
    midHighHighpassL1.getAddressB1(), midHighHighpassL1.getAddressB2(), 
    midHighHighpassL1.getAddressA1(), midHighHighpassL1.getAddressA2() );
  midHighHighpassL2.copySettingsFrom(&midHighHighpassL1);
  midHighHighpassR1.copySettingsFrom(&midHighHighpassL1);
  midHighHighpassR2.copySettingsFrom(&midHighHighpassL1);

  if( isOdd(midHighOrder) )
  {
    midHighHighpassL2.setGlobalGainFactor(-1.0);
    midHighHighpassR2.setGlobalGainFactor(-1.0);
  }
  else
  {
    midHighHighpassL2.setGlobalGainFactor(+1.0);
    midHighHighpassR2.setGlobalGainFactor(+1.0);
  }
}