#include "rosic_Equalizer.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

Equalizer::Equalizer()
{
  mutex.lock();
  sampleRate       = 44100.0;
  globalGainFactor = 1.0;
  mutex.unlock();
}

Equalizer::~Equalizer()
{
  mutex.lock();
  bands.clear();
  mutex.unlock();
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void Equalizer::setSampleRate(float newSampleRate)
{
  mutex.lock();
  if( newSampleRate <= 0.0 )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return;
  }
  sampleRate = newSampleRate;
  for(unsigned int i=0; i<bands.size(); i++)
    bands[i].setSampleRate(sampleRate);
  mutex.unlock();
}

int Equalizer::addBand(int newMode, double newFrequency, double newGain, double newBandwidth)
{
  int result = -1;

  mutex.lock();
  TwoPoleFilter newBand;
  newBand.setMode(      newMode,      false);
  newBand.setFrequency( newFrequency, false);
  newBand.setGain(      newGain,      false);
  newBand.setBandwidth( newBandwidth, false);
  newBand.setSampleRate(sampleRate,   true);

  bands.push_back(newBand);
  result = bands.size()-1;
  mutex.unlock();

  return result;
}

bool Equalizer::removeBand(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return false;
  }
  bands.erase(bands.begin()+index);
  mutex.unlock();
  return true;
}

bool Equalizer::removeAllBands()
{
  mutex.lock();
  bool result = (bands.size() != 0);
  bands.clear();
  mutex.unlock();
  return result;
}

bool Equalizer::modifyBand(unsigned int index, int newMode, double newFrequency, double newGain,
                           double newBandwidth)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return false;
  }
  bands[index].setParameters(newMode, newFrequency, newGain, newBandwidth, true);
  mutex.unlock();
  return true;
}

bool Equalizer::setBandMode(int index, int newMode)
{
  return modifyBand(index, newMode, getBandFrequency(index), getBandGain(index),
    getBandBandwidth(index));
}

bool Equalizer::setBandFrequency(int index, double newFrequency)
{
  return modifyBand(index, getBandMode(index), newFrequency, getBandGain(index),
    getBandBandwidth(index));
}

bool Equalizer::setBandGain(int index, double newGain)
{
  return modifyBand(index, getBandMode(index), getBandFrequency(index), newGain,
    getBandBandwidth(index));
}

bool Equalizer::setBandBandwidth(int index, double newBandwidth)
{
  return modifyBand(index, getBandMode(index), getBandFrequency(index), getBandGain(index),
    newBandwidth);
}

bool Equalizer::setLowerBandedgeFrequency(unsigned int index, double newLowerBandedgeFrequency)
{
  bool result = false;
  mutex.lock();
  if( index < bands.size() )
  {
    bands[index].setLowerBandedgeFrequency(newLowerBandedgeFrequency);
    result = true;
  }
  mutex.unlock();
  return result;
}

bool Equalizer::setUpperBandedgeFrequency(unsigned int index, double newUpperBandedgeFrequency)
{
  bool result = false;
  mutex.lock();
  if( index < bands.size() )
  {
    bands[index].setUpperBandedgeFrequency(newUpperBandedgeFrequency);
    result = true;
  }
  mutex.unlock();
  return result;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

int Equalizer::getNumBands()
{
  int result = 0;
  mutex.lock();
  result = bands.size();
  mutex.unlock();
  return result;
}

int Equalizer::getBandMode(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0;
  }
  int result = bands[index].getMode();
  mutex.unlock();
  return result;
}

double Equalizer::getBandFrequency(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0.0;
  }
  double result = bands[index].getFrequency();
  mutex.unlock();
  return result;
}

double Equalizer::getBandGain(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0.0;
  }
  double result = bands[index].getGain();
  mutex.unlock();
  return result;
}

double Equalizer::getBandBandwidth(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0.0;
  }
  double result = bands[index].getBandwidth();
  mutex.unlock();
  return result;
}

double Equalizer::getLowerBandedgeFrequency(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0.0;
  }
  double result = bands[index].getLowerBandedgeFrequency();
  mutex.unlock();
  return result;
}

double Equalizer::getUpperBandedgeFrequency(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0.0;
  }
  double result = bands[index].getUpperBandedgeFrequency();
  mutex.unlock();
  return result;
}

bool Equalizer::doesModeSupportBandwidth(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0.0;
  }
  bool result = bands[index].doesModeSupportBandwidth();
  mutex.unlock();
  return result;
}

bool Equalizer::doesModeSupportGain(unsigned int index)
{
  mutex.lock();
  if( index < 0 || index >= bands.size() )
  {
    DEBUG_BREAK;
    mutex.unlock();
    return 0.0;
  }
  bool result = bands[index].doesModeSupportGain();
  mutex.unlock();
  return result;
}

void Equalizer::getMagnitudeResponse(double *frequencies, double *magnitudes, int numBins)
{
  mutex.lock();
  for(int k=0; k<numBins; k++)
  {
    magnitudes[k] = globalGainFactor;
    if( frequencies[k] < 0.5*sampleRate )
    {
      for(unsigned int s=0; s<bands.size(); s++)
        magnitudes[k] *= bands[s].getMagnitudeAt(frequencies[k]);
      magnitudes[k] = amp2dBWithCheck(magnitudes[k], 0.0001);
    }
    else
      magnitudes[k] = -120.0;
  }
  mutex.unlock();
}

//-------------------------------------------------------------------------------------------------
// others:

void Equalizer::reset()
{
  mutex.lock();
  for(unsigned int s=0; s<bands.size(); s++)
    bands[s].reset();
  mutex.unlock();
}





