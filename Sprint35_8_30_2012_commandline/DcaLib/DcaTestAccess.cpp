
#include "StdAfx.h"
#include "DcaTestAccess.h"

CString TestAccessTagToString(TestAccessTag tagValue)
{
   const char* stringValue;

   switch (tagValue)
   {
   case testAccessAll:     stringValue = "BOTH";    break;
   case testAccessBottom:  stringValue = "BOTTOM";  break;
   case testAccessTop:     stringValue = "TOP";     break;
   default:
   case testAccessNone:    stringValue = "NONE";    break;
   }

   return CString(stringValue);
}
