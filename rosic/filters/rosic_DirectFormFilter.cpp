#include "rosic_DirectFormFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

DirectFormFilter::DirectFormFilter(int maximumOrder)
{
  maxOrder = maximumOrder;
  order    = maxOrder;
  w        = new double[order+1];
  a        = new double[order+1];
  b        = new double[order+1];
  initializeCoefficients();
  reset();  
}

DirectFormFilter::~DirectFormFilter()
{
  delete[] w;
  delete[] a;
  delete[] b;
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void DirectFormFilter::setCoefficients(double *newCoeffsA, double *newCoeffsB, int newOrder)
{
  initializeCoefficients();
  for(int k=0; k<=newOrder; k++)
  {
    a[k] = newCoeffsA[k];
    b[k] = newCoeffsB[k];
  }
}

//-------------------------------------------------------------------------------------------------
// others:

void DirectFormFilter::reset()
{
  for(int i=0; i<=maxOrder; i++)
    w[i] = 0.0;
}

void DirectFormFilter::initializeCoefficients()
{
  b[0] = 1.0;
  a[0] = 1.0;
  for(int i=1; i<=maxOrder; i++)
  {
    b[i] = 0.0;
    a[i] = 0.0;
  }
}


