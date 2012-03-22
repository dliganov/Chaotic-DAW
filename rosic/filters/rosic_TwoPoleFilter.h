#ifndef rosic_TwoPoleFilter_h
#define rosic_TwoPoleFilter_h

// rosic-indcludes:
#include "rosic_BiquadStereoDF1.h"
#include "rosic_BiquadDesigner.h"

namespace rosic
{

  /**

  This class implements a single biquad equalizer band designed by Sophocles Orfanidis' design
  procedure to match the magnitude of an analog equalizer at the Nyquist frequency. Its 
  user-parameters are sample-rate in Hz, the center frequency in Hz, the boost/cut gain in dB, and 
  the bandwidth in octaves.

  */

  class TwoPoleFilter : public BiquadStereoDF1
  {

  public:

    /** Enumeration of the available filter modes. */
    enum modes
    {
      BYPASS = 0,
      PEAK,
      LOW_SHELF,
      HIGH_SHELF,
      LOWPASS6,
      LOWPASS12,
      HIGHPASS6,
      HIGHPASS12,
      BANDREJECT,
      BANDPASS,
      //LOWHIGH_PASS_CHAIN,

      REAL_POLE,
      REAL_ZERO,
      POLE_PAIR,
      ZERO_PAIR,

      NUM_FILTER_MODES
    };

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    TwoPoleFilter();

    /** Destructor. */
    ~TwoPoleFilter();

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate (in Hz) at which the filter runs. */
    void setSampleRate(double newSampleRate, bool updateCoefficients = true);

    /** Sets the filter mode as one of the values in enum modes. */
    void setMode(int newMode, bool updateCoefficients = true);

    /** Sets the center frequency in Hz. */
    void setFrequency(double newFrequency, bool updateCoefficients = true);

    /** Sets the boost/cut gain in dB. */
    void setGain(double newGain, bool updateCoefficients = true);

    /** Sets the bandwidth in octaves. */
    void setBandwidth(double newBandwidth, bool updateCoefficients = true);

    /** Sets the radius for the pole/zero filter types. */
    void setRadius(double newRadius, bool updateCoefficients = true);

    /** Sets up the frequency, gain and bandwidth at once. */
    void setParameters(int newMode, double newFrequency, double newGain, double newBandwidth, 
      bool updateCoefficients = true);

    /** Sets the lower bandedge frequency by updating the bandwidth parameter (keeping the center 
    frequency constant). */
    void setLowerBandedgeFrequency(double newLowerBandedgeFrequency, 
      bool updateCoefficients = true);

    /** Sets the upper bandedge frequency by updating the bandwidth parameter (keeping the center 
    frequency constant). */
    void setUpperBandedgeFrequency(double newUpperBandedgeFrequency, 
      bool updateCoefficients = true);

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Sets the filter mode as one of the values in enum modes. */
    int getMode() const { return mode; }

    /** Returns the center frequency in Hz. */
    double getFrequency() const { return frequency; }

    /** Returns the boost/cut gain in dB. */
    double getGain() const { return gain; }

    /** Returns the bandwidth in octaves. */
    double getBandwidth() const { return bandwidth; }

    /** Returns the magnitude of this equalizer band at a given frequency in Hz. */
    double getMagnitudeAt(double frequency) const
    { return BiquadDesigner::getBiquadMagnitudeAt(b0, b1, b2, a1, a2, frequency, sampleRate); }

    /** Returns the lower bandedge frequency (defined at the half dB-gain point). */
    double getLowerBandedgeFrequency() const { return (frequency/pow(2.0, 0.5*bandwidth)); }

    /** Returns the upper bandedge frequency (defined at the half dB-gain point). */
    double getUpperBandedgeFrequency() const { return (frequency*pow(2.0, 0.5*bandwidth)); }

    /** Returns true if the current mode supports a bandwidth parameter, false otherwise. */
    bool doesModeSupportBandwidth() const;

    /** Returns true if the current mode supports a gain parameter, false otherwise. */
    bool doesModeSupportGain() const;

    /** Returns true if the current mode supports a frequency parameter, false otherwise. */
    bool doesModeSupportFrequency() const { return mode != BYPASS; }

    /** Returns true if the current mode supports a radius parameter, false otherwise. */
    bool doesModeSupportRadius() const;

    //---------------------------------------------------------------------------------------------
    // others:

    /** Calculates the biquad coefficients from the specification. */
    void updateCoeffs();

    //=============================================================================================

  protected:

    double frequency, gain, bandwidth, radius;
    double sampleRate;
    int    mode;

  };

} // end namespace rosic

#endif // rosic_TwoPoleFilter_h
