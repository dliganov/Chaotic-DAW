#ifndef rosic_DirectFormFilter_h
#define rosic_DirectFormFilter_h

#include <string.h> // for memmove

#include "../basics/GlobalDefinitions.h"

namespace rosic
{

  /**

  This is a generic filter using a Direct Form II implementation structure.

  */

  class DirectFormFilter
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor - the passed parameter will determine the maximum order which this filter will
    be able to realize.  */
    DirectFormFilter(int maximumOrder);   

    /** Destructor. */
    ~DirectFormFilter();

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    void setCoefficients(double *newCoeffsA, double *newCoeffsB, int newOrder);

    /** Resets the filter state. */
    void reset();

    /** Initializes the filter coefficients so as to realize a 'bypass' filter. */
    void initializeCoefficients();

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates a single filtered output-sample. */
    INLINE double getSample(double in);

    //=============================================================================================

  protected:

    double *w;       // state buffer
    double *a, *b;   // filter coefficients
    int    order;    // currently used filter order
    int    maxOrder; // maximum possible filter order

  };

  //-----------------------------------------------------------------------------------------------
  // from here: definitions of the functions to be inlined, i.e. all functions which are supposed 
  // to be called at audio-rate (they can't be put into the .cpp file):

  INLINE double DirectFormFilter::getSample(double in)
  {
    // obtain intermediate signal as output from feedback part:
    int k;
    double tmp = in + TINY;
    for(k=1; k<=order; k++)
      tmp -= a[k]*w[k];

    // obtain output signal as output from feedforward part:
    double y = tmp*b[0];
    for(k=1; k<=order; k++)
      y += b[k]*w[k];

    // update state variables:
    memmove(&w[2], &w[1], (order-1)*sizeof(double));
    w[1] = tmp;

    return y;
  }

} // end namespace rosic

#endif // rosic_DirectFormFilter_h
