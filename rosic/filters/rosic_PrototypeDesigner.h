#ifndef rosic_PrototypeDesigner_h
#define rosic_PrototypeDesigner_h

// rosic-indcludes:
#include "../math/rosic_ComplexFunctions.h"
//#include "../math/rosic_PolynomialAlgorithms.h"

namespace rosic
{

  /**

  This class determines the locations of poles and zeros in the s-plane for a continuous time, 
  unit cutoff lowpass or low-shelving prototype filter. It supports Butterworth, Chebychev, inverse 
  Chebychev, elliptic and Bessel designs. Bessel design is provided only for the lowpass case via 
  a hard-coded table for filters for orders up to 25. That means, Bessel filters can be designed 
  only up to order 25 (which should be more than enough for all practical purposes). The remaining 
  4 design methods are based on calculation and have no restrictions (other than numerical 
  considerations) on the order, therefore. The low-shelving design is a generalization of a unit
  cutoff lowpass filter and and its magnitude response can be seen as the lowpass-response raised 
  to a pedestal. The height of this pedestal is called the reference gain (which is zero in the 
  lowpass-case). The gain the 'passband' (represented by the member variable G) is either a boost 
  (when G > 1) or an attenuation/cut (when G < 1) - for standard lowpass designs, this gain would 
  be unity. 

  References: 
   -(1) Sophocles J. Orfanidis: Lecture Notes on Elliptic Filter Design
   -(2) Sophocles J. Orfanidis: High-Order Elliptical Equalizer Design

  \todo
   -check the gain calculation 
   -solve the degree quation for other values than the filter order
   -check getFilterResponseAt() - seems to be buggy
   -rescale Bessel-poles so as to have the -3 dB point at unit cutoff (needs getFilterResponseAt)

  */

  class PrototypeDesigner  
  {

  public:

    /** This is an enumeration of the available approximation methods. */
    enum approximationMethods
    {
      BUTTERWORTH = 1,   ///< Butterworth approximation method - maximally flat.
      CHEBYCHEV,         ///< Chebychev approximation method - equiripple in the passband.
      INVERSE_CHEBYCHEV, ///< Inverse Chebychev approximation method - equiripple in the stopband.
      ELLIPTIC,          ///< Elliptic design - equiripple in passband and stopband.
      BESSEL,            ///< Bessel design - approximates linear phase.
      PAPOULIS           ///< NOT implemented.
    };

    /** This enumerates the two possible prototype filter characterisitics. */
    enum prototypeModes
    {
      LOWPASS_PROTOTYPE = 1, 
      LOWSHELV_PROTOTYPE
    };

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    PrototypeDesigner();   

    /** Destructor. */
    ~PrototypeDesigner();  

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets ups the order of the filter - that is the number of first order sections. Each 
    section increases the slope by 6 dB/oct (for lowpass-designs). If the order is odd, a real 
    pole-/zero pair will be present, otherwise all poles and zeros will be complex conjugate. */
    void setOrder(int newOrder);      

    /** Chooses one of the approximation methods as enumerated above. */
    void setApproximationMethod(int newApproximationMethod);     

    /** Chooses a lowpass or low-shelving prototype as enumerated above. */
    void setPrototypeMode(int newPrototypeMode);

    /** Sets the ripple in the passband for lowpass designs in decibels. */
    void setPassbandRipple(double newPassbandRipple);

    /** Sets the rejection in the stopband for lowpass designs in decibels. */
    void setStopbandRejection(double newStopbandRejection);

    /** Sets the gain for shelving filters in the passband (in dB). This will be positive for a
    low-boost and negative for low-cut responses. */
    void setGain(double newGain);

    /** Sets up the reference gain for shelving filters (in dB). This will be usually unity. */
    void setReferenceGain(double newReferenceGain);

    /** Selects the fraction of the maximum gain, which the magnitude response assumes at the 
    passband-frequency. This parameter is relevant only for Chebychev and elliptic shelving 
    filters. It should be chosen close to unity (for example 0.95) in order to prevent excessive 
    ripple in the passband. */
    void setPassbandGainRatio(double newPassbandGainRatio);

    /** Selects the fraction of the maximum gain, which the magnitude response assumes at the 
    stopband-frequency. This parameter is only relevant for inverse Chebychev and elliptic shelving
    filters. It should be chosen close to zero (for example 0.05) in order to prevent excessive 
    ripple in the stopband. */   
    void setStopbandGainRatio(double newBandwidthGainRatio);

    /** Assigns the poles and zeros such that the resulting filter will just pass the signal
    through. */
    void makeBypass();

    //---------------------------------------------------------------------------------------------
    // calculation of the required filter order:

    /** Calculates the order required for a Butterworth filter to fullfill given design 
    specifications. The actual order should be chosen to be the next integer. */
    static double getRequiredButterworthOrder(double passbandFrequency, double passbandRipple,
      double stopbandFrequency, double stopbandRipple);

    /** Calculates the order required for a Chebychev filter (inverse or not) to fullfill given 
    design specifications. The actual order should be chosen to be the next integer. */
    static double getRequiredChebychevOrder(double passbandFrequency, double passbandRipple,
      double stopbandFrequency, double stopbandRipple);

    /** Calculates the order required for an elliptic filter to fullfill given design
    specifications. The actual order should be chosen to be the next integer. */
    static double getRequiredEllipticOrder(double passbandFrequency, double passbandRipple,
      double stopbandFrequency, double stopbandRipple);

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Re-calculates the poles and zeros (if necesarry) an writes them into the respective 
    arrays. For complex conjugate pairs, it will only write one representant of the pair into the 
    array. If N denotes the order as passed to setOrder(), let L be the number of second order 
    sections and r be either one or zero indicating presence or absence of a first order section. 
    Then, the array will be filled with one representant for each pole pair into poles[0...L-1] and 
    the real pole will be written into poles[L+r-1], if present. The same goes for the zeros. */
    void getPolesAndZeros(Complex* poles, Complex* zeros);

    /** Calculates and returns the complex frequency response at some value of the Laplace 
    transform variable s - seems to be buggy. */
    Complex getFilterResponseAt(Complex s);

    /** Returns the normalized (divided by the DC gain) magnitude at the given radian 
    frequency w. */
    double getMagnitudeAt(double w);

    /** Finds the radian frequency w inside the interval wLow...wHigh at which the specified 
    magnitude (as raw amplitude value, not in dB) occurs. For the function to work properly, the
    magnitude response inside the given interval should be monotonic and the specified magnitude
    should occur somewhere inside the given interval. */
    double findFrequencyWithMagnitude(double magnitude, double wLow, double wHigh);

    /** Returns the number of finite poles. */
    int getNumFinitePoles();

    /** Returns the number of finite zeros. */
    int getNumFiniteZeros();

    /** Returns the number of non-redundant finite zeros (each pair of complex conjugate zeros 
    counts only once). */
    int getNumNonRedundantFiniteZeros();

    /** Returns the number of non-redundant finite poles (each pair of complex conjugate poles 
    counts only once). */
    int getNumNonRedundantFinitePoles();

    /** Returns the passbandRipple in dB. */
    double getPassbandRipple() const { return Ap; }

    /** Returns the ratio between the peak gain (in dB) and the ripples inside the boosted/cutted 
    band for shelving modes. */
    double getPassbandGainRatio() const { return Rp; }

    /** Returns the approximation method to be used @see enum approximationMethods. */
    int getApproximationMethod() const { return approximationMethod; }

    /** Returns the mode of the prototype @see enum modes. */
    int getPrototypeMode() const { return prototypeMode; }

    /** Returns the filter order. */
    int getOrder() const { return N; }

    /** Returns true if the currently selected approximation method supports a ripple parameter. */
    bool hasCurrentMethodRippleParameter();
    //{ return (approximationMethod == ELLIPTIC) || (approximationMethod == CHEBYCHEV); }

    /** Returns true if the currently selected approximation method supports a rejection 
    parameter. */
    bool hasCurrentMethodRejectionParameter();
    //{ return (approximationMethod == ELLIPTIC) || (approximationMethod == INVERSE_CHEBYCHEV); }

    //=============================================================================================

  protected:

    /** Solves the elliptic degree equation (k from N, k1). */
    double ellipdeg(int N, double k_1);

    /** Solves the elliptic degree equation (k_1 from N, k1). */
    double ellipdeg1(int N, double k);

    /** Solves the elliptic degree equation (k_1 from N, k) using nomes. */
    double ellipdeg2(double N, double k);

    /** (Re-) allocates memory for the poles and zeros if neceasrry. */
    void allocatePolesAndZeros();

    /** Calculates the poles and zeros according to the selected approximation method and the 
    global gain factor. */
    void updatePolesAndZeros();

    /** Under the assumption that there are no finite zeros, this function re-scales the poles 
    in such a way to attain a specified level at unit cutoff (expressed in dB). This is used for 
    the Bessel prototype. 

    !!!NOT USABLE YET!!!
 
    */
    void rescalePoles(double targetGainAtUnitCutoff);

    /** Calculates the poles and zeros for a Butterworth lowpass filter. */
    void makeButterworthLowpass();

    /** Calculates the poles and zeros for a Butterworth low-shelving filter. */
    void makeButterworthLowShelv();

    /** Calculates the poles and zeros for a Chebychev lowpass filter. */
    void makeChebychevLowpass();

    /** Calculates the poles and zeros for a Chebychev low-shelving filter. */
    void makeChebychevLowShelv();

    /** Calculates the poles and zeros for an inverse Chebychev lowpass filter. */
    void makeInverseChebychevLowpass();

    /** Calculates the poles and zeros for an inverse Chebychev low-shelving filter. */
    void makeInverseChebychevLowShelv();

    /** Calculates the poles and zeros for an elliptic lowpass filter. */
    void makeEllipticLowpass();

    /** Calculates the poles and zeros for an elliptic low-shelving filter. */
    void makeEllipticLowShelv();

    /** Assigns the postions of the poles of a Bessel prototype filter. These positions are
    tabulated and hard-coded inside this class. */
    void makeBesselLowpass();

    // user parameters:
    int N;                   // prototype filter order: N = 2*L + r 
    int approximationMethod; // selected approximation method 
    int prototypeMode;       // selected mode (lowpass or low-shelv)
    double   Ap;             // passband ripple/attenuation for lowpass designs in dB
    double   As;             // stopband rejection for lowpass designs in dB
    double   A;              // boost/cut gain for low-shelv designs in dB
    double   A0;             // reference gain for low-shelv designs in dB
    double   Rp;             // ripple in the boosted/cutted band as fraction of dB-peak-gain 
                             // for shelvers (elliptic and chebychev)
    double   Rs;             // ripple outside the boosted/cutted band as fraction of dB-peak-gain 
                             // for shelvers (elliptic and inverse chebychev)

    // internal variables:
    int      L;              // number of second order sections
    int      r;              // number of first order sections (either zero or one)
    int      numFinitePoles; // number of poles (excluding those at infinity)
    int      numFiniteZeros; // number of zeros (excluding those at infinity). */
    Complex* z;              // array of the non-redundant zeros
    Complex* p;              // array of the non-redundant poles
    //double   gain;           // global gain factor (not used as of yet
    bool     stateIsDirty;   // this flag indicates, whether the poles, zeros and gain need to be 
                             // re-calculated or are still valid from a previous calculation

  };

}  // end namespace rosic

#endif // rosic_PrototypeDesigner_h
