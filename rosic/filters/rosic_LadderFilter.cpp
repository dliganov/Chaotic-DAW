#include "rosic_LadderFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

LadderFilter::LadderFilter()
{
  parameters = new LadderFilterParameters;
  isMaster   = true;

  cutoff = 1000.0; 
  b0 = 1.0;
  a1 = 0.0;      
  k  = 0.0;
  c0 = 0.0;
  c1 = 0.0;
  c2 = 0.0;
  c3 = 0.0;
  c4 = 1.0;

  makeupGain    = 1.0;
  //makeupGainRec = 1.0;
  //makeupGainSq  = 1.0;

  allpass.setSampleRate(parameters->sampleRate);
  allpass.setMode(OnePoleFilterStereo::ALLPASS);
  //allpass.setCutoff(0.25*cutoff);
  allpass.setCutoff(20000.0);
  allpass.setShelvingGain(1.0);

  calculateCoefficients();
  reset();

  int dummy = 0;
}

LadderFilter::~LadderFilter()
{
  if( isMaster && parameters != NULL )
    delete parameters;
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void LadderFilter::setSampleRate(double newSampleRate)
{
  if( newSampleRate > 0.0 )
    parameters->sampleRate = newSampleRate;
  allpass.setSampleRate(parameters->sampleRate);

  calculateCoefficients();
  for(unsigned int s=0; s<slaves.size(); s++)
    slaves[s]->calculateCoefficients();
}

void LadderFilter::setAllpassFreq(double newAllpassFreq)
{
  parameters->allpassFreq = newAllpassFreq;
  allpass.setCutoff(newAllpassFreq);
  for(unsigned int s=0; s<slaves.size(); s++)
    slaves[s]->setAllpassFreq(newAllpassFreq);
}

void LadderFilter::setMakeUp(double newMakeUp, bool updateCoefficients)
{
  parameters->makeUp = newMakeUp;

  if( updateCoefficients == true )
  {
    calculateCoefficients();
    for(unsigned int s=0; s<slaves.size(); s++)
      slaves[s]->calculateCoefficients();
  }
}

void LadderFilter::setDrive(double newDrive)
{
  parameters->driveFactor = dB2amp(newDrive);
}

void LadderFilter::setDcOffset(double newDcOffset)
{
  parameters->dcOffset = newDcOffset;
}

void LadderFilter::setOutputStage(int newOutputStage)
{
  if( newOutputStage >= 0 && newOutputStage <= 4 )
    parameters->outputStage = newOutputStage;
  else
    DEBUG_BREAK; // ouput stage must be in 0...4
}

void LadderFilter::setMode(int newMode)
{
  if( newMode >= 0 && newMode < LadderFilterParameters::NUM_MODES )
    parameters->mode = newMode;
}

void LadderFilter::setMorph(double newMorph)
{
  parameters->morph = newMorph;

/*
c0 = [ 1  0  0  0  0];
c1 = [-4  1  0  0  0];
c2 = [ 6 -3  1  0  0];
c3 = [-4  3 -2  1  0];
c4 = [ 1 -1  1 -1  1];
*/

  // may be streamlined
  double m = newMorph;
  if( m < 0.25 )
  {
    c0 =  1.0 + 4.0*m* ( 0.0 -  1.0);
    c1 = -4.0 + 4.0*m* ( 1.0 - -4.0);
    c2 =  6.0 + 4.0*m* (-3.0 -  6.0);
    c3 = -4.0 + 4.0*m* ( 3.0 - -4.0);
    c4 =  1.0 + 4.0*m* (-1.0 -  1.0);
  }
  else if( m < 0.5 )
  {
    m  -= 0.25;
    c0  =  0.0 + 4.0*m* ( 0.0 -  0.0);
    c1  =  1.0 + 4.0*m* ( 0.0 -  1.0);
    c2  = -3.0 + 4.0*m* ( 1.0 - -3.0);
    c3  =  3.0 + 4.0*m* (-2.0 -  3.0);
    c4  = -1.0 + 4.0*m* ( 1.0 - -1.0);
  }
  else if( m < 0.75 )
  {
    m  -= 0.5;
    c0  =  0.0 + 4.0*m* ( 0.0 -  0.0);
    c1  =  0.0 + 4.0*m* ( 0.0 -  0.0);
    c2  =  1.0 + 4.0*m* ( 0.0 -  1.0);
    c3  = -2.0 + 4.0*m* ( 1.0 - -2.0);
    c4  =  1.0 + 4.0*m* (-1.0 -  1.0);
  }
  else if( m < 1.0 )
  {
    m  -= 0.75;
    c0  =  0.0 + 4.0*m* ( 0.0 -  0.0);
    c1  =  0.0 + 4.0*m* ( 0.0 -  0.0);
    c2  =  0.0 + 4.0*m* ( 0.0 -  0.0);
    c3  =  1.0 + 4.0*m* ( 0.0 -  1.0);
    c4  = -1.0 + 4.0*m* ( 1.0 - -1.0);
  }
  else
  {
    c0 = c1 = c2 = c3 = 0.0;
    c4 = 1.0;
  }


  // todo: try a polynomial between the breakpoints for more natural transition


  int dummy = 0;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

Complex LadderFilter::getTransferFunctionAt(Complex z, bool withFeedback, bool withMakeUpBoost, 
                                           bool withMakeUpGain, int stage)
{
  Complex G1, G4, G, H;

  G1 = b0 / (1 + a1*(1/z));             // G1: response of one stage
  G4 = G1*G1*G1*G1;                     // G4: response of four stages (without feedback)
  switch( stage )
  {
  case 0: G = 1.0;       break;
  case 1: G = G1;        break;
  case 2: G = G1*G1;     break;
  case 3: G = G1*G1*G1;  break;
  case 4: G = G4;        break;
  }                                     // G: response of the requested stage (without feedback)

  if( withFeedback == true )
    H  = G / (1 + k*(1/z)*G4);         // response of four stages with feedback
  else
    H = G;

  //if( withMakeUpBoost == true )
  //  H *= lowBooster.getTransferFunctionAt(z);

  if( withMakeUpGain == true )
    H *= makeupGain;

  return H;
}

double LadderFilter::getMagnitudeAt(double frequency, bool withFeedback, bool withMakeUpBoost,
                                   bool withMakeUpGain, int stage)
{
  double omega = 2*PI*frequency / parameters->sampleRate;

  Complex z = expC(Complex(0.0, omega));
  Complex H = getTransferFunctionAt(z, withFeedback, withMakeUpBoost, withMakeUpGain, stage);

  return H.getRadius();
}

void LadderFilter::getMagnitudeResponse(double *frequencies, double *magnitudes, int numBins, 
                                       bool inDecibels, bool accumulate)
{
  int k;
  int s = parameters->outputStage;
  if( inDecibels == false )
  {
    if( accumulate == false )
    {
      for(k=0; k<numBins; k++)
        magnitudes[k] = getMagnitudeAt(frequencies[k], true, true, true, s);
    }
    else
    {
      for(k=0; k<numBins; k++)
        magnitudes[k] *= getMagnitudeAt(frequencies[k], true, true, true, s);
    }
  }
  else
  {
    if( accumulate == false )
    {
      for(k=0; k<numBins; k++)
        magnitudes[k] = amp2dBWithCheck(getMagnitudeAt(frequencies[k], true, true, true, s));
    }
    else
    {
      for(k=0; k<numBins; k++)
        magnitudes[k] += amp2dBWithCheck(getMagnitudeAt(frequencies[k], true, true, true, s));
    }
  }
}

double LadderFilter::getCutoff()
{ 
  return cutoff; 
}

double LadderFilter::getResonance()
{ 
  return parameters->resonanceRaw; 
}

double LadderFilter::getDrive()
{
  return amp2dB(parameters->driveFactor);
}

int LadderFilter::getOutputStage()
{
  return parameters->outputStage;
}

double LadderFilter::getAllpassFreq()
{
  return parameters->allpassFreq;
}

double LadderFilter::getMakeUp()
{
  return parameters->makeUp;
}

//-------------------------------------------------------------------------------------------------
// master/slave config:

void LadderFilter::addSlave(LadderFilter* newSlave)
{
  // add the new slave to the vector of slaves:
  slaves.push_back(newSlave);

  // delete the original parameter-set of the new slave and redirect it to ours (with some safety 
  // checks):
  if( newSlave->parameters != NULL && newSlave->parameters != this->parameters )
  {
    delete newSlave->parameters;
    newSlave->parameters = this->parameters;
  }
  else
  {
    DEBUG_BREAK; 
    // the object to be added as slave did not contain a valid parameter-pointer - maybe it has 
    // been already added as slave to another master?
  }

  // set the isMaster-flag of the new slave to false: 
  newSlave->isMaster = false;

  // this flag will prevent the destructor of the slave from trying to delete the parameter-set 
  // which is now shared - only masters delete their parameter-set on destruction
}

//-------------------------------------------------------------------------------------------------
// others:

void LadderFilter::reset()
{
  allpass.resetBuffers();
  y1L   = 0.0;
  y2L   = 0.0;
  y3L   = 0.0;
  y4L   = 0.0;
  yOldL = 0.0;
  y1R   = 0.0;
  y2R   = 0.0;
  y3R   = 0.0;
  y4R   = 0.0;
  yOldR = 0.0;
}