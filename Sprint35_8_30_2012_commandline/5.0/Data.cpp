// $Header: /CAMCAD/5.0/Data.cpp 178   3/22/07 12:54a Kurt Van Ness $

#include "Stdafx.h"
#include "Data.h"
#include "Attribute.h"
#include "Graph.h"
#include "WriteFormat.h"
#include "TMState.h"
#include "RwLib.h"
#include "PolyLib.h"
#include "Measure.h"
#include "EntityNumber.h"
#include "Qfe.h"
#include "Polygon.h"
#include "ArcPoint.h"
#include "Debug.h"
#include <Float.h>

//_____________________________________________________________________________
int propagateLayer(int parentLayer,int childLayer)
{
   int retval = childLayer;
/*
   Parent   Child  Result
  -1       -1       -1
  -1        f        f
  -1        Lc       Lc
   f       -1        f
   f        f        f
   f        Lc       Lc
   Lp      -1        Lp
   Lp       f        Lp
   Lp       Lc       Lc
*/

   if (parentLayer == -1)
   {
      if (childLayer == -1)
      {
         retval = -1;
      }
      else if (childLayer == FloatingLayer)
      {
         retval = FloatingLayer;
      }
      else
      {
         retval = childLayer;
      }
   }
   else if (parentLayer == FloatingLayer)
   {
      if (childLayer == -1)
      {
         retval = FloatingLayer;
      }
      else if (childLayer == FloatingLayer)
      {
         retval = FloatingLayer;
      }
      else
      {
         retval = childLayer;
      }
   }
   else
   {
      if (childLayer == -1)
      {
         retval = parentLayer;
      }
      else if (childLayer == FloatingLayer)
      {
         retval = parentLayer;
      }
      else
      {
         retval = childLayer;
      }
   }

   return retval;
}

#if !defined(EnableDcaCamCadDocLegacyCode)
//_____________________________________________________________________________
ExtentRect::ExtentRect()
{  
   reset();
}

ExtentRect& ExtentRect::operator=(const CExtent& extent)
{
   left   = extent.getXmin();
   bottom = extent.getYmin();
   right  = extent.getXmax();
   top    = extent.getYmax();

   return *this;
}

void ExtentRect::reset()
{  
   left   =  DBL_MAX;
   bottom =  DBL_MAX;
   right  = -DBL_MAX;
   top    = -DBL_MAX;
}

bool ExtentRect::isCorrupt()
{  
   bool retval = false;
   
   if (!(_finite(left) && _finite(right) && _finite(bottom) && _finite(top)))
   {
      retval = true;
   }

   return retval;
}

bool ExtentRect::isValid()
{  
   bool retval = !isCorrupt();

   if (retval)
   {
      retval = (left <= right) && (bottom <= top);
   }

   return retval;
}
#endif






