// $Header: /CAMCAD/DcaLib/DcaVerticalPosition.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "Stdafx.h"
#include "DcaVerticalPosition.h"

//_____________________________________________________________________________
VerticalPositionTag intToVerticalPositionTag(int value)
{
   VerticalPositionTag retval = verticalPositionBaseline;

   if (value >= verticalPositionBaseline && value <= verticalPositionBottom)
   {
      retval = (VerticalPositionTag)value;
   }

   return retval;
}

CString verticalPositionTagToFriendlyString(VerticalPositionTag tagValue)
{
   const char* retval;

   switch (tagValue)
   {
   case verticalPositionBaseline:  retval = "Baseline";   break;
   case verticalPositionTop:       retval = "Top";        break;
   case verticalPositionCenter:    retval = "Center";     break;
   case verticalPositionBottom:    retval = "Bottom";     break;
   default:                        retval = "Undefined";  break;
   }

   return retval;
}
