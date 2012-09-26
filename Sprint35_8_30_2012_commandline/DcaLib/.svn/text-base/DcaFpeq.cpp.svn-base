// $Header: /CAMCAD/DcaLib/DcaFpeq.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaFpeq.h"
#include <Math.h>

//_____________________________________________________________________________
bool fpeq(double a,double b)
{
   return (fabs(a - b) < SMALLNUMBER);
}

bool fpeq(double a,double b,double smallNumber)
{
   bool retval;
   double diff;

   if (b != 0. && a != 0.)
   {
      diff = (a/b) - 1.;
   }
   else
   {
      diff = a - b;
   }

   retval = (fabs(diff) < smallNumber);

   return retval; 
}

bool fpnear(double a,double b,double tolerance)
{
   bool retval;

   double diff = a - b;

   retval = (fabs(diff) < tolerance);

   return retval; 
}
