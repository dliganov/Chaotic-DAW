#include "rosic_RealFunctions.h"
using namespace rosic;

void rosic::ellipticIntegral(double k, double *K, double *Kprime, int M)
{
  // this implementation is a port from the MatLab file ellipk by Sophocles Orfanidis

  double kmin = 1e-6; 
  double kmax = sqrt(1-kmin*kmin);

  double kp, L;
  double* v  = new double[M];
  double* vp = new double[M];

  int n;

  if( k == 1.0 )
   *K = INF;
  else if( k > kmax )
  {
    kp = sqrt(1.0-k*k);
    L  = -log(kp/4.0);
    *K = L + (L-1.0)*kp*kp/4.0; 
  }
  else
  {
    landen(k, M, v);
    for(n=0; n<M; n++)
      v[n] += 1.0;
    *K = product(v, M) * 0.5*PI;
  }

  if( k == 0.0 )
   *Kprime = INF;
  else if( k < kmin )
  {
    L       = -log(k/4.0);
    *Kprime = L + (L-1.0)*k*k/4.0;
  }
  else
  {
    kp = sqrt(1.0-k*k);                             
    landen(kp, M, vp);
    for(n=0; n<M; n++)
      vp[n] += 1.0;
    *Kprime = product(vp, M) * 0.5*PI;
  }

  delete[] v;
  delete[] vp;
}

