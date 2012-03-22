#ifndef rosic_BiquadStereoDF2_h
#define rosic_BiquadStereoDF2_h

// rosic-indcludes:
#include "rosic_BiquadBase.h"

namespace rosic
{

  /**

  This class implements a biquad filter which realize the pair of difference equations:

  \f[ w[n] = x[n] + a_1 y[n-1] + a_2 y[n-2]     \f]
  \f[ y[n] = b_0 w[n] + b_1 w[n-1] + b_2 w[n-2] \f]

  note the positive sign of the feedback part, as opposed to negative sign seen in many DSP
  textbooks - this has been chosen to allow for better potential for parallelization.

  ...performance tests indicate that DF1 seems to be more efficient

  */

  class BiquadStereoDF2 : public BiquadBase
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    BiquadStereoDF2();

    /** Destructor. */
    ~BiquadStereoDF2();

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates a single filtered output-sample in place. */
    INLINE void getSampleInPlace(double &inOut);

    /** Calculates a single filtered output-sample. */
    INLINE double getSample(double in);

    /** Calculates a single filtered output-sample. */
    INLINE float getSample(float in);

    /** Calculates one output stereo sample-frame at a time. */
    INLINE void getSampleFrameStereo(double *inL, double *inR, double *outL, double *outR);

    /** Calculates one output stereo sample-frame at a time. */
    INLINE void getSampleFrameStereo(float *inL, float *inR, float *outL, float *outR);

    //---------------------------------------------------------------------------------------------
    // others:

    /** Sets the buffers for the previous input and output samples to zero. */
    void reset();

    //=============================================================================================

  protected:

    // internal state buffers:
    double lw1, lw2, rw1, rw2;

  };

  //-----------------------------------------------------------------------------------------------
  // from here: definitions of the functions to be inlined, i.e. all functions which are supposed
  // to be called at audio-rate (they can't be put into the .cpp file):

  INLINE void BiquadStereoDF2::getSampleInPlace(double &inOut)
  {
    doubleA tmp = inOut;

    tmp = tmp + (a1*lw1 + a2*lw2);
    tmp = (b0*tmp+TINY) + (b1*lw1 + b2*lw2);
      // parentheses facilitate out-of-order execution

    lw2 = lw1;
    lw1 = tmp;

    inOut = tmp;
  }

  INLINE double BiquadStereoDF2::getSample(double in)
  {
    doubleA tmp;

    tmp = (in+TINY) + (a1*lw1 + a2*lw2);
    tmp = (b0*tmp+TINY) + (b1*lw1 + b2*lw2);
      // parentheses facilitate out-of-order execution

    lw2 = lw1;
    lw1 = tmp;

    return tmp;
  }

  INLINE float BiquadStereoDF2::getSample(float in)
  {
    return (float) getSample((double) in);
  }

  INLINE void BiquadStereoDF2::getSampleFrameStereo(double *inL, double *inR, double *outL, double *outR)
  {
    doubleA tmp;

    // left:
    tmp   = (*inL+TINY) + (a1*lw1 + a2*lw2);
    tmp   = (b0*tmp) + (b1*lw1 + b2*lw2);
    lw2   = lw1;
    lw1   = tmp;
    *outL = tmp;

    // right:
    tmp   = (*inR+TINY) + (a1*rw1 + a2*rw2);
    tmp   = (b0*tmp) + (b1*rw1 + b2*rw2);
    rw2   = rw1;
    rw1   = tmp;
    *outR = tmp;
  }

  INLINE void BiquadStereoDF2::getSampleFrameStereo(float *inL, float *inR, float *outL, float *outR)
  {
    doubleA tmp;

    // left:
    tmp   = (*inL+TINY) + (a1*lw1 + a2*lw2);
    tmp   = (b0*tmp) + (b1*lw1 + b2*lw2);
    lw2   = lw1;
    lw1   = tmp;
    *outL = (float) tmp;

    // right:
    tmp   = (*inR+TINY) + (a1*rw1 + a2*rw2);
    tmp   = (b0*tmp) + (b1*rw1 + b2*rw2);
    rw2   = rw1;
    rw1   = tmp;
    *outR = (float) tmp;
  }

} // end namespace rosic

#endif // rosic_BiquadStereoDF2_h
