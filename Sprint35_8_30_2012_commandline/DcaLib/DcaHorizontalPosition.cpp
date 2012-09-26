// $Header: /CAMCAD/DcaLib/DcaHorizontalPosition.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "Stdafx.h"
#include "DcaHorizontalPosition.h"

//_____________________________________________________________________________
HorizontalPositionTag intToHorizontalPositionTag(int value)
{
   HorizontalPositionTag retval = horizontalPositionLeft;

   if (value >= horizontalPositionLeft && value <= horizontalPositionRight)
   {
      retval = (HorizontalPositionTag)value;
   }

   return retval;
}

HorizontalPositionTag oppositeHorizontalPosition(HorizontalPositionTag tagValue)
{
   HorizontalPositionTag oppositeTagValue = tagValue;

   switch (tagValue)
   {
   case horizontalPositionLeft:   oppositeTagValue = horizontalPositionRight;  break;
   case horizontalPositionRight:  oppositeTagValue = horizontalPositionLeft;   break;
   }

   return oppositeTagValue;
}

CString horizontalPositionTagToFriendlyString(HorizontalPositionTag tagValue)
{
   const char* retval;

   switch (tagValue)
   {
   case horizontalPositionLeft:    retval = "Left";       break;
   case horizontalPositionCenter:  retval = "Center";     break;
   case horizontalPositionRight:   retval = "Right";      break;
   default:                        retval = "Undefined";  break;
   }

   return retval;
}

