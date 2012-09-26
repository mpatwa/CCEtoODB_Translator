// $Header: /CAMCAD/DcaLib/DcaFillStyle.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaFillStyle.h"

//_____________________________________________________________________________
short ValidatedFillStyle(short proposedFillStyle)
{
   // If the proposed fill style is within the range of known good fill style
   // values then just return it. If it is not, then return "not set".

   switch (proposedFillStyle)
   {
   case HS_NOT_SET:     // defined by CAMCAD
   case HS_SOLID:       // defined by CAMCAD

   case HS_HORIZONTAL:  // defined by Bill Gates in wingdi.h
   case HS_VERTICAL:    // defined by Bill Gates in wingdi.h
   case HS_FDIAGONAL:   // defined by Bill Gates in wingdi.h
   case HS_BDIAGONAL:   // defined by Bill Gates in wingdi.h
   case HS_CROSS:       // defined by Bill Gates in wingdi.h
   case HS_DIAGCROSS:   // defined by Bill Gates in wingdi.h
      return proposedFillStyle;
   }

   return HS_NOT_SET;
}
