#ifndef rosic_CrossOver_h
#define rosic_CrossOver_h

// rosic-indcludes:
#include "rosic_HighOrderEqualizerDesigner.h"
#include "rosic_BiquadCascade.h"

namespace rosic
{

  /**

  This class implements a 3-way crossover filter to split the signal into low, mid and high 
  frequency part using a Linkwitz/Riley filter of adjustable order.

  */

  class CrossOver //: public PresetRememberer
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    CrossOver(int newMaxButterworthOrder = 8);   

    /** Destructor. */
    ~CrossOver();  

    //---------------------------------------------------------------------------------------------
    // parameter settings:
      
    /** Sets up the sample rate for this filter. */
    void setSampleRate(double newSampleRate);

    /** Sets the crossover frequency between low and mid band. */
    void setLowMidFreq(double newLowMidFreq);

    /** Sets the order of the filters for low/mid splitting. */
    void setLowMidOrder(int newLowMidOrder);

    /** Sets the crossover frequency between mid and high band. */
    void setMidHighFreq(double newMidHighFreq);

    /** Sets the order of the filters for mid/high splitting. */
    void setMidHighOrder(int newMidHighOrder);

    /** Switches the crossover form 3-way into 2-way mode (or vice versa). */
    void setTwoWayMode(bool shouldBeTwoWay);

    /** Switches between stereo and mono mode. */
    void setMonoMode(bool shouldBeMono);

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the crossover frequency between low and mid band. */
    double getLowMidFreq();

    /** Returns the order of the filters for low/mid splitting. */
    int getLowMidOrder();

    /** Returns the crossover frequency between mid and high band. */
    double getMidHighFreq();

    /** Returns the order of the filters for mid/high splitting. */
    int getMidHighOrder();

    /** Fills the 'magnitudes' array with the magnitude response of the lowpass filter evaluated 
    at the frequencies passed in the 'frequencies' array. Both arrays are assumed to be numBins 
    long. */
    void getLowpassResponse(double* frequencies, double* magnitudes, int numBins);

    /** Fills the 'magnitudes' array with the magnitude response of the bandpass filter evaluated 
    at the frequencies passed in the 'frequencies' array. Both arrays are assumed to be numBins 
    long. */
    void getBandpassResponse(double* frequencies, double* magnitudes, int numBins);

    /** Fills the 'magnitudes' array with the magnitude response of the highpass filter evaluated 
    at the frequencies passed in the 'frequencies' array. Both arrays are assumed to be numBins 
    long. */
    void getHighpassResponse(double* frequencies, double* magnitudes, int numBins);

    /** Returns true when crossover is in 2-way mode, false for 3-way mode. */
    bool isInTwoWayMode() const;

    /** Returns true when crossover is in mono mode, false for stereo mode. */
    bool isInMonoMode() const;

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates a stereo-ouput frame. */
    INLINE void getSampleFrameStereo(double *inL, double *inR, double *outLowL, double *outLowR, 
      double *outMidL, double *outMidR, double *outHighL, double *outHighR);

    //---------------------------------------------------------------------------------------------
    // others:

    /** Resets the internal buffers of the filters to all zeros. */
    void resetBuffers();

    //=============================================================================================

  protected:

    /** Triggers a re-calculation of the filter coefficients. */
    void updateFilterCoefficients();

    double sampleRate;                // the sample-rate to work at
    double lowMidFreq, midHighFreq;   // crossover frequencies
    int    lowMidOrder, midHighOrder; // order of the Butterworth filters 
    bool   twoWayMode;                // indicates, whether this should be only a 2-way crossover. 
    bool   monoMode;                  // indicates, whether this crosoover should work in mono. 
    int    maxButterworthOrder;       // maximum order of the Butterworth filters 
    
    // Note: the actual Linkwitz-Riley filters consist of two (identical) Butterworth filters in 
    // series such that their order is twice as high as the respective Butterworth order parameters

    // embedded objects:
    BiquadCascade lowMidLowpassL1, lowMidLowpassL2, lowMidLowpassR1, lowMidLowpassR2;
    BiquadCascade lowMidHighpassL1, lowMidHighpassL2, lowMidHighpassR1, lowMidHighpassR2;
    BiquadCascade midHighLowpassL1, midHighLowpassL2, midHighLowpassR1, midHighLowpassR2;
    BiquadCascade midHighHighpassL1, midHighHighpassL2, midHighHighpassR1, midHighHighpassR2;

  };

  //-----------------------------------------------------------------------------------------------
  // from here: definitions of the functions to be inlined, i.e. all functions
  // which are supposed to be called at audio-rate (they can't be put into
  // the .cpp file):

  INLINE void CrossOver::getSampleFrameStereo(double *inL, double *inR, double *outLowL, 
    double *outLowR, double *outMidL, double *outMidR, double *outHighL, double *outHighR)
  {
    // process left channel:
    double tmpL  = *inL;

    // calculate low band:
    *outLowL = lowMidLowpassL2.getSampleDirect1(lowMidLowpassL1.getSampleDirect1(tmpL));

    // calculate high and mid band:
    tmpL = lowMidHighpassL2.getSampleDirect1(lowMidHighpassL1.getSampleDirect1(tmpL));

    if( twoWayMode == false )
    {
      // calculate mid band:
      *outMidL  = midHighLowpassL2.getSampleDirect1(midHighLowpassL1.getSampleDirect1(tmpL));

      // calculate high band:
      *outHighL  = midHighHighpassL2.getSampleDirect1(midHighHighpassL1.getSampleDirect1(tmpL));
    }
    else
    {
      *outMidL  = tmpL;
      *outHighL = 0.0;
    }

    // in mono-mode, copy the left channel data into the right channel, in stereo mode process 
    // right channel the same way:
    if( monoMode == true )
    {
      *outLowR  = *outLowL;
      *outMidR  = *outMidL;
      *outHighR = *outHighL;
    }
    else
    {
      double tmpR = *inR;
      *outLowR    = lowMidLowpassR2.getSampleDirect1(lowMidLowpassR1.getSampleDirect1(tmpR));
      tmpR        = lowMidHighpassR2.getSampleDirect1(lowMidHighpassR1.getSampleDirect1(tmpR));
      if( twoWayMode == false )
      {
        *outMidR  = midHighLowpassR2.getSampleDirect1(midHighLowpassR1.getSampleDirect1(tmpR));
        *outHighR = midHighHighpassR2.getSampleDirect1(midHighHighpassR1.getSampleDirect1(tmpR));
      }
      else
      {
        *outMidR  = tmpR;
        *outHighR = 0.0;
      }
    }
  }

} // end namespace rosic

#endif // rosic_CrossOver_h
