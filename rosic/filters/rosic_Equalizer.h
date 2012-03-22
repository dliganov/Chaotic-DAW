#ifndef rosic_Equalizer_h
#define rosic_Equalizer_h

// includes from the STL:
#include <vector>
using std::vector;

// rosic-indcludes:
#include "rosic_TwoPoleFilter.h"
#include "../infrastructure/rosic_MutexLock.h"

namespace rosic
{

  /**

  This class implements a series connection of an arbitrary number of parametric equalizer filters.

  \todo: maybe handle the thread-safe accesses in the same way as in EchoLab


  */

  class Equalizer
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    Equalizer();

    /** Destructor. */
    ~Equalizer();

    //---------------------------------------------------------------------------------------------
    // setup:

    /** Sets up the sample-rate. */
    void setSampleRate(float newSampleRate);

    /** Adds a new filter into the vector. The integer return-value informs, at which index the 
    new band was inserted.  */
    int addBand(int newMode, double newFrequency, double newGain, 
      double newBandwidth = 2.0*asinh(1.0/sqrt(2.0))/log(2.0));

    /** Removes a filter from the vector. The return-value informs, if there was actually a
    band removed (if you try to remove a non-existing band it will return false */
    bool removeBand(unsigned int index);

    /** Removes all bands and returns true if there was at least one band to remove. */
    bool removeAllBands();

    /** Modifies the data of an existing band. If you try to modify a non-existent band
    or try to modify the data in such a way which is not allowed, it will return false. */
    bool modifyBand(unsigned int index, int newMode, double newFrequency, double newGain, 
      double newBandwidth);

    /** Changes the mode of one of the bands. */
    bool setBandMode(int index, int newMode);

    /** Changes the frequency of one of the bands. */
    bool setBandFrequency(int index, double newFrequency);

    /** Changes the gain of one of the bands. */
    bool setBandGain(int index, double newGain);

    /** Changes the bandwidth of one of the bands. */
    bool setBandBandwidth(int index, double newBadwidth);

    /** Sets the lower bandedge frequency for one of the bands by updating the bandwidth parameter 
    (keeping the center frequency constant). */
    bool setLowerBandedgeFrequency(unsigned int index, double newLowerBandedgeFrequency);

    /** Sets the upper bandedge frequency for one of the bands by updating the bandwidth parameter 
    (keeping the center frequency constant). */
    bool setUpperBandedgeFrequency(unsigned int index, double newLowerBandedgeFrequency);

    /** Changes the global gain in decibels. */
    void setGlobalGain(double newGainInDB) { globalGainFactor = dB2amp(newGainInDB); }

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the number of bands. */
    int getNumBands();

    /** Returns the mode of one of the bands (0 if index is out of range). */
    int getBandMode(unsigned int index);

    /** Returns the frequency of one of the bands (0.0 if index is out of range). */
    double getBandFrequency(unsigned int index);

    /** Returns the gain of one of the bands (0.0 if index is out of range). */
    double getBandGain(unsigned int index);

    /** Returns the bandwidth of one of the bands (0.0 if index is out of range). */
    double getBandBandwidth(unsigned int index);

    /** Returns the lower bandedge frequency (defined at the half dB-gain point) of one of the 
    bands (0.0 if index is out of range). */
    double getLowerBandedgeFrequency(unsigned int index);

    /** Returns the upper bandedge frequency (defined at the half dB-gain point) of one of the 
    bands (0.0 if index is out of range). */
    double getUpperBandedgeFrequency(unsigned int index);

    /** Returns true if the current mode of some band supports a bandwidth parameter, false 
    otherwise. */
    bool doesModeSupportBandwidth(unsigned int index);

    /** Returns true if the current mode of some band supports a gain parameter, false 
    otherwise. */
    bool doesModeSupportGain(unsigned int index);

    /** Fills the 'magnitudes' array with the magnitude response of the equalizer evaluated at the 
    frequencies passed in the 'frequencies' array. Both arrays are assumed to be numBins long. */
    void getMagnitudeResponse(double* frequencies, double* magnitudes, int numBins);

    /** Returns the global gain in decibels. */
    double getGlobalGain() const { return amp2dB(globalGainFactor); }

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates an output sample. */
    INLINE double getSample(double in);

    /** Calculates an output sample without acquiring the mutex-lock. Always wrap calls to that 
    function into acquireLock()/releaseLock() calls. */
    INLINE double getSampleWithoutLock(double in);

    /** Calculates a stereo output sampleframe. */
    INLINE void getSampleFrameStereo(double *inOutL, double *inOutR);

    /** Calculates an entire block of samples at once. */
    INLINE void processBlock(float *inOutL, float *inOutR, int numSamples);

    //---------------------------------------------------------------------------------------------
    // others:

    /** Aquires the mutex-lock for accessing the vector of the bands. */
    void acquireLock() { mutex.lock(); }

    /** Releases the mutex-lock for accessing the vector of the bands. */
    void releaseLock() { mutex.unlock(); }

    /** Resets the internal states of the filters. */
    void reset();

    //=============================================================================================

  protected:

    vector<TwoPoleFilter> bands;   // vector of the bands
    float     sampleRate;          // sample-rate
    double    globalGainFactor;    // a global gain factor
    MutexLock mutex;               // mutex-lock for accessing the array of filters

  };

  //-----------------------------------------------------------------------------------------------
  // from here: definitions of the functions to be inlined, i.e. all functions which are supposed
  // to be called at audio-rate (they can't be put into the .cpp file):

  INLINE double Equalizer::getSample(double in)
  {
    mutex.lock();

    double tmp = in;
    for(unsigned int i=0; i<bands.size(); i++)
      tmp = bands[i].getSample(tmp);

    mutex.unlock();
    return globalGainFactor*tmp;
  }

  INLINE double Equalizer::getSampleWithoutLock(double in)
  {
    double tmp = in;
    for(unsigned int i=0; i<bands.size(); i++)
      tmp = bands[i].getSample(tmp);
    return globalGainFactor*tmp;
  }

  INLINE void Equalizer::getSampleFrameStereo(double *inOutL, double *inOutR)
  {
    mutex.lock();

    for(unsigned int i=0; i<bands.size(); i++)
      bands[i].getSampleFrameStereo(inOutL, inOutR);
    *inOutL *= globalGainFactor;
    *inOutR *= globalGainFactor;

    mutex.unlock();
  }

  INLINE void Equalizer::processBlock(float *inOutL, float *inOutR, int numSamples)
  {
    mutex.lock();

    double tmpL, tmpR;
    for(int n=0; n<numSamples; n++)
    {
      tmpL = (double) inOutL[n];
      tmpR = (double) inOutR[n];

      for(unsigned int i=0; i<bands.size(); i++)
        bands[i].getSampleFrameStereo(&tmpL, &tmpR);

      inOutL[n] = (float) (globalGainFactor*tmpL);
      inOutR[n] = (float) (globalGainFactor*tmpR);
    }

    mutex.unlock();
  }

} // end namespace rosic

#endif // rosic_Equalizer_h
