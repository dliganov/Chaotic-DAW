#include "rosic_EngineersFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

EngineersFilter::EngineersFilter() : BiquadCascade(24)
{
  numStages = 1;
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void EngineersFilter::setSampleRate(double newSampleRate)
{
  BiquadCascade::setSampleRate(newSampleRate);
  designer.setSampleRate(newSampleRate);
  updateCoefficients();
}

void EngineersFilter::setMode(int newMode)
{
  designer.setMode(newMode);
  BiquadCascade::setNumStages(designer.getNumBiquadStages());
  updateCoefficients();
}

void EngineersFilter::setApproximationMethod(int newApproximationMethod)
{
  designer.setApproximationMethod(newApproximationMethod);
  updateCoefficients();
}

void EngineersFilter::setFrequency(double newFrequency)
{
  designer.setFrequency(newFrequency);
  updateCoefficients();
}

void EngineersFilter::setPrototypeOrder(int newOrder)
{
  designer.setPrototypeOrder(newOrder);
  BiquadCascade::setNumStages(designer.getNumBiquadStages());
  updateCoefficients();
}

void EngineersFilter::setBandwidth(double newBandwidth)
{
  designer.setBandwidth(newBandwidth);
  updateCoefficients();
}

void EngineersFilter::setGain(double newGain)
{
  designer.setGain(newGain);
  updateCoefficients();
}

void EngineersFilter::setRipple(double newRipple)
{
  designer.setRipple(newRipple);
  updateCoefficients();
}

void EngineersFilter::setStopbandRejection(double newStopbandRejection)
{
  designer.setStopbandRejection(newStopbandRejection);
  updateCoefficients();
}

//-------------------------------------------------------------------------------------------------
// inquiry:



//-------------------------------------------------------------------------------------------------
// internal functions:

void EngineersFilter::updateCoefficients()
{
  BiquadCascade::initBiquadCoeffs();
  designer.getBiquadCascadeCoefficients(b0, b1, b2, a1, a2);
}