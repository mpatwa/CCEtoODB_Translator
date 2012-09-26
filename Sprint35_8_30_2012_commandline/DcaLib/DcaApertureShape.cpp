// $Header: /CAMCAD/DcaLib/DcaApertureShape.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaApertureShape.h"

//_____________________________________________________________________________
CString apertureShapeToName(ApertureShapeTag apertureShape)
{
   const char* retval;

   switch (apertureShape)
   {
   case apertureUndefined:  retval = "UNDEFINED";  break;
   case apertureRound:      retval = "ROUND";      break;
   case apertureSquare:     retval = "SQUARE";     break;
   case apertureRectangle:  retval = "RECTANGLE";  break;
   case apertureTarget:     retval = "TARGET";     break;
   case apertureThermal:    retval = "THERMAL";    break;
   case apertureComplex:    retval = "COMPLEX";    break;                               
   case apertureDonut:      retval = "DONUT";      break;
   case apertureOctagon:    retval = "OCTAGON";    break;
   case apertureOblong:     retval = "OBLONG";     break;
   case apertureBlank:      retval = "BLANK";      break;
   default:                 retval = "Undefined";  break;
   }

   return CString(retval);
}

//_____________________________________________________________________________
CString apertureShapeToString(int apertureShape)
{
   const char* retval;

   switch (apertureShape)
   {
   case apertureUndefined:  retval = "T_UNDEFINED";  break;
   case apertureRound:      retval = "T_ROUND";      break;
   case apertureSquare:     retval = "T_SQUARE";     break;
   case apertureRectangle:  retval = "T_RECTANGLE";  break;
   case apertureTarget:     retval = "T_TARGET";     break;
   case apertureThermal:    retval = "T_THERMAL";    break;
   case apertureComplex:    retval = "T_COMPLEX";    break;                               
   case apertureDonut:      retval = "T_DONUT";      break;
   case apertureOctagon:    retval = "T_OCTAGON";    break;
   case apertureOblong:     retval = "T_OBLONG";     break;
   case apertureBlank:      retval = "T_BLANK";      break;
   default:                 retval = "Undefined";    break;
   }

   return CString(retval);
}

//_____________________________________________________________________________
CString apertureShapeToName(int apertureShape)
{
   const char* retval;

   switch (apertureShape)
   {
   case apertureUndefined:  retval = "UNDEFINED";  break;
   case apertureRound:      retval = "ROUND";      break;
   case apertureSquare:     retval = "SQUARE";     break;
   case apertureRectangle:  retval = "RECTANGLE";  break;
   case apertureTarget:     retval = "TARGET";     break;
   case apertureThermal:    retval = "THERMAL";    break;
   case apertureComplex:    retval = "COMPLEX";    break;                               
   case apertureDonut:      retval = "DONUT";      break;
   case apertureOctagon:    retval = "OCTAGON";    break;
   case apertureOblong:     retval = "OBLONG";     break;
   case apertureBlank:      retval = "BLANK";      break;
   default:                 retval = "Undefined";  break;
   }

   return CString(retval);
}

//_____________________________________________________________________________
ApertureShapeTag intToApertureShape(int apertureShape)
{
   ApertureShapeTag retval = apertureUndefined;

   switch (apertureShape)
   {
   case apertureUndefined:  retval = apertureUndefined;  break;
   case apertureRound:      retval = apertureRound;      break;
   case apertureSquare:     retval = apertureSquare;     break;
   case apertureRectangle:  retval = apertureRectangle;  break;
   case apertureTarget:     retval = apertureTarget;     break;
   case apertureThermal:    retval = apertureThermal;    break;
   case apertureComplex:    retval = apertureComplex;    break;
   case apertureDonut:      retval = apertureDonut;      break;
   case apertureOctagon:    retval = apertureOctagon;    break;
   case apertureOblong:     retval = apertureOblong;     break;
   case apertureBlank:      retval = apertureBlank;      break;
   case apertureUnknown:    retval = apertureUnknown;    break;
   }

   return retval;
}
