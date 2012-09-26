// $Header: /CAMCAD/DcaLib/DcaStandardAperture.cpp 7     6/21/07 8:30p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaCamCadData.h"
#include "DcaStandardAperture.h"
#include "DcaTMatrix.h"
#include "DcaWriteFormat.h"
#include "DcaVector2d.h"
#include "DcaArcPoint.h"
#include "DcaPoly.h"
#include "DcaPolygon.h"
#include "DcaText.h"
#include "DcaHorizontalPosition.h"
#include "DcaVerticalPosition.h"
#include "DcaPoint2.h"
#include "DcaDataListIterator.h"
#include "DcaDataType.h"

// tan(degreesToRadians(90) / 4.);
#define Bulge90 (0.4142135623730950488016887242097)
#define DbFlag0 (0)
#define NegativePoly true

inline bool fpeq3(double a,double b)
{
   bool retval;

   if (b != 0.)
   {
      retval = (fabs((a/b) - 1.) < SMALLNUMBER);
   }
   else
   {
      retval = (fabs(a) < SMALLNUMBER);
   }

   return retval;
}

CString standardApertureTypeTagToString(StandardApertureTypeTag tagValue)
{
   CString retval = "INVALID";

   switch (tagValue)
   {
   case standardApertureButterfly:                      retval = "standardApertureButterfly";                      break;
   case standardApertureBeveledRectangle:               retval = "standardApertureBeveledRectangle";               break;
   case standardApertureChamferedRectangle:             retval = "standardApertureChamferedRectangle";             break;
   case standardApertureCircle:                         retval = "standardApertureCircle";                         break;
   case standardApertureSplitCircle:                    retval = "standardApertureSplitCircle";                    break;
   case standardApertureDiamond:                        retval = "standardApertureDiamond";                        break;
   case standardApertureEllipse:                        retval = "standardApertureEllipse";                        break;
   case standardApertureHalfOval:                       retval = "standardApertureHalfOval";                       break;
   case standardApertureHole:                           retval = "standardApertureHole";                           break;
   case standardApertureHorizontalHexagon:              retval = "standardApertureHorizontalHexagon";              break;
   case standardApertureInvalid:                        retval = "standardApertureInvalid";                        break;
   case standardApertureMoire:                          retval = "standardApertureMoire";                          break;
   case standardApertureNotchedRectangle:               retval = "standardApertureNotchedRectangle";               break;
   case standardApertureNull:                           retval = "standardApertureNull";                           break;
   case standardApertureOctagon:                        retval = "standardApertureOctagon";                        break;
   case standardApertureOval:                           retval = "standardApertureOval";                           break;
   case standardApertureRectangle:                      retval = "standardApertureRectangle";                      break;
   case standardApertureRectangularThermal:             retval = "standardApertureRectangularThermal";             break;
   case standardApertureRectangularThermalOpenCorners:  retval = "standardApertureRectangularThermalOpenCorners";  break;
   case standardApertureRoundDonut:                     retval = "standardApertureRoundDonut";                     break;
   case standardApertureRoundedRectangle:               retval = "standardApertureRoundedRectangle";               break;
   case standardApertureRoundThermalRounded:            retval = "standardApertureRoundThermalRounded";            break;
   case standardApertureRoundThermalSquare:             retval = "standardApertureRoundThermalSquare";             break;
   case standardApertureSpurredRectangle:               retval = "standardApertureSpurredRectangle";               break;
   case standardApertureSquare:                         retval = "standardApertureSquare";                         break;
   case standardApertureSquareButterfly:                retval = "standardApertureSquareButterfly";                break;
   case standardApertureSquareDonut:                    retval = "standardApertureSquareDonut";                    break;
   case standardApertureRectangledPentagon:             retval = "standardApertureRectangledPentagon";             break;
   case standardApertureSquareRoundThermal:             retval = "standardApertureSquareRoundThermal";             break;
   case standardApertureSquareThermal:                  retval = "standardApertureSquareThermal";                  break;
   case standardApertureSquareThermalOpenCorners:       retval = "standardApertureSquareThermalOpenCorners";       break;
   case standardApertureTabbedRectangle:                retval = "standardApertureTabbedRectangle";                break;
   case standardApertureTriangle:                       retval = "standardApertureTriangle";                       break;
   case standardApertureUndefined:                      retval = "standardApertureUndefined";                      break;
   case standardApertureVeeCutRectangle:                retval = "standardApertureVeeCutRectangle";                break;
   case standardApertureVerticalHexagon:                retval = "standardApertureVerticalHexagon";                break;
   }

   return retval;
}

CString standardApertureTypeTagToApertureNameDescriptor(StandardApertureTypeTag tagValue)
{
   CString typeName;

   switch (tagValue)
   {
   case standardApertureButterfly:                      typeName = "Butterfly";                         break;
   case standardApertureBeveledRectangle:               typeName = "Beveled Rectangle";                 break;
   case standardApertureChamferedRectangle:             typeName = "Chamfered Rectangle";               break;
   case standardApertureCircle:                         typeName = "Circle";                            break;
   case standardApertureSplitCircle:                    typeName = "Split Circle";                      break;
   case standardApertureDiamond:                        typeName = "Diamond";                           break;
   case standardApertureEllipse:                        typeName = "Ellipse";                           break;
   case standardApertureHalfOval:                       typeName = "HalfOval";                          break;
   case standardApertureHole:                           typeName = "Hole";                              break;
   case standardApertureHorizontalHexagon:              typeName = "Horizontal Hexagon";                break;
   case standardApertureInvalid:                        typeName = "Invalid";                           break;
   case standardApertureMoire:                          typeName = "Moire";                             break;
   case standardApertureNotchedRectangle:               typeName = "Notched Rectangle";                 break;
   case standardApertureNull:                           typeName = "Null";                              break;
   case standardApertureOctagon:                        typeName = "Octagon";                           break;
   case standardApertureOval:                           typeName = "Oval";                              break;
   case standardApertureRectangle:                      typeName = "Rectangle";                         break;
   case standardApertureRectangularThermal:             typeName = "Rectangular Thermal";               break;
   case standardApertureRectangularThermalOpenCorners:  typeName = "Rectangular Thermal Open Corners";  break;
   case standardApertureRoundDonut:                     typeName = "Donut";                             break;
   case standardApertureRoundedRectangle:               typeName = "Rounded Rectangle";                 break;
   case standardApertureRoundThermalRounded:            typeName = "Round Thermal Rounded";             break;
   case standardApertureRoundThermalSquare:             typeName = "Round Thermal Square";              break;
   case standardApertureSpurredRectangle:               typeName = "Spurred Rectangle";                 break;
   case standardApertureSquare:                         typeName = "Square";                            break;
   case standardApertureSquareButterfly:                typeName = "Square Butterfly";                  break;
   case standardApertureSquareDonut:                    typeName = "Square Donut";                      break;
   case standardApertureRectangledPentagon:             typeName = "Rectangled Pentagon";               break;
   case standardApertureSquareRoundThermal:             typeName = "Square Round Thermal";              break;
   case standardApertureSquareThermal:                  typeName = "Square Thermal";                    break;
   case standardApertureSquareThermalOpenCorners:       typeName = "Square Thermal Open Corners";       break;
   case standardApertureTabbedRectangle:                typeName = "Tabbed Rectangle";                  break;
   case standardApertureTriangle:                       typeName = "Triangle";                          break;
   case standardApertureUndefined:                      typeName = "Undefined";                         break;
   case standardApertureVeeCutRectangle:                typeName = "V-Cut Rectangle";                   break;
   case standardApertureVerticalHexagon:                typeName = "Vertical Hexagon";                  break;
   }

   return typeName;
}

CString standardApertureTypeTagToDescriptorPrefix(StandardApertureTypeTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case standardApertureButterfly:                      retval = "butterfly";                      break;
   case standardApertureBeveledRectangle:               retval = "beveledRectangle";               break;
   case standardApertureChamferedRectangle:             retval = "chamferedRectangle";             break;
   case standardApertureCircle:                         retval = "round";                          break;
   case standardApertureSplitCircle:                    retval = "splitRound";                     break;
   case standardApertureDiamond:                        retval = "diamond";                        break;
   case standardApertureEllipse:                        retval = "ellipse";                        break;
   case standardApertureHalfOval:                       retval = "halfOval";                       break;
   case standardApertureHole:                           retval = "hole";                           break;
   case standardApertureHorizontalHexagon:              retval = "hexagon";                        break;
   case standardApertureInvalid:                        retval = "invalid";                        break;
   case standardApertureMoire:                          retval = "moire";                          break;
   case standardApertureNotchedRectangle:               retval = "notchedRectangle";               break;
   case standardApertureNull:                           retval = "null";                           break;
   case standardApertureOctagon:                        retval = "octagon";                        break;
   case standardApertureOval:                           retval = "oval";                           break;
   case standardApertureRectangle:                      retval = "rectangle";                      break;
   case standardApertureRectangularThermal:             retval = "rectangularThermal";             break;
   case standardApertureRectangularThermalOpenCorners:  retval = "rectangularThermalOpenCorners";  break;
   case standardApertureRoundDonut:                     retval = "donut";                          break;
   case standardApertureRoundedRectangle:               retval = "roundedRectangle";               break;
   case standardApertureRoundThermalRounded:            retval = "roundThermalRounded";            break;
   case standardApertureRoundThermalSquare:             retval = "roundThermalSquare";             break;
   case standardApertureSpurredRectangle:               retval = "spurredRectangle";               break;
   case standardApertureSquare:                         retval = "square";                         break;
   case standardApertureSquareButterfly:                retval = "squareButterfly";                break;
   case standardApertureSquareDonut:                    retval = "squareDonut";                    break;
   case standardApertureRectangledPentagon:             retval = "rectangledPentagon";             break;
   case standardApertureSquareRoundThermal:             retval = "squareRoundThermal";             break;
   case standardApertureSquareThermal:                  retval = "squareThermal";                  break;
   case standardApertureSquareThermalOpenCorners:       retval = "squareThermalOpenCorners";       break;
   case standardApertureTabbedRectangle:                retval = "tabbedRectangle";                break;
   case standardApertureTriangle:                       retval = "triangle";                       break;
   case standardApertureVeeCutRectangle:                retval = "veeCutRectangle";                break;
   case standardApertureVerticalHexagon:                retval = "verticalHexagon";                break;
   case standardApertureUndefined:
   default:                                             retval = "undefined";                      break;
   }

   return retval;
}

CString standardApertureTypeTagToValorDescriptorPrefix(StandardApertureTypeTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case standardApertureButterfly:                      retval = "bfr";          break;
   case standardApertureBeveledRectangle:               retval = "beveled";      break;
   case standardApertureChamferedRectangle:             retval = "rect";         break;
   case standardApertureCircle:                         retval = "r";            break;
   case standardApertureSplitCircle:                    retval = "sr";           break;
   case standardApertureDiamond:                        retval = "di";           break;
   case standardApertureEllipse:                        retval = "el";           break;
   case standardApertureHalfOval:                       retval = "oval_h";       break;
   case standardApertureHole:                           retval = "hole";         break;
   case standardApertureHorizontalHexagon:              retval = "hex_l";        break;
   case standardApertureInvalid:                        retval = "invalid";      break;
   case standardApertureMoire:                          retval = "moire";        break;
   case standardApertureNotchedRectangle:               retval = "notched";      break;
   case standardApertureNull:                           retval = "null";         break;
   case standardApertureOctagon:                        retval = "oct";          break;
   case standardApertureOval:                           retval = "oval";         break;
   case standardApertureRectangle:                      retval = "rect";         break;
   case standardApertureRectangularThermal:             retval = "rc_ths";       break;
   case standardApertureRectangularThermalOpenCorners:  retval = "rc_tho";       break;
   case standardApertureRoundDonut:                     retval = "donut_r";      break;
   case standardApertureRoundedRectangle:               retval = "rect";         break;
   case standardApertureRoundThermalRounded:            retval = "thr";          break;
   case standardApertureRoundThermalSquare:             retval = "ths";          break;
   case standardApertureSpurredRectangle:               retval = "spurred";      break;
   case standardApertureSquare:                         retval = "s";            break;
   case standardApertureSquareButterfly:                retval = "bfs";          break;
   case standardApertureSquareDonut:                    retval = "donut_s";      break;
   case standardApertureRectangledPentagon:             retval = "rc_pentagon";  break;
   case standardApertureSquareRoundThermal:             retval = "sr_ths";       break;
   case standardApertureSquareThermal:                  retval = "s_ths";        break;
   case standardApertureSquareThermalOpenCorners:       retval = "s_tho";        break;
   case standardApertureTabbedRectangle:                retval = "tabbed";       break;
   case standardApertureTriangle:                       retval = "tri";          break;
   case standardApertureVeeCutRectangle:                retval = "veecut";       break;
   case standardApertureVerticalHexagon:                retval = "hex_s";        break;
   case standardApertureUndefined:
   default:                                             retval = "undefined";    break;
   }

   return retval;
}

StandardApertureTypeTag intToStandardApertureTypeTag(int tagValue)
{
   StandardApertureTypeTag retval;

   switch (tagValue)
   {
   case standardApertureButterfly:                      retval = standardApertureButterfly;                      break;
   case standardApertureBeveledRectangle:               retval = standardApertureBeveledRectangle;               break;
   case standardApertureChamferedRectangle:             retval = standardApertureChamferedRectangle;             break;
   case standardApertureCircle:                         retval = standardApertureCircle;                         break;
   case standardApertureSplitCircle:                    retval = standardApertureSplitCircle;                    break;
   case standardApertureDiamond:                        retval = standardApertureDiamond;                        break;
   case standardApertureEllipse:                        retval = standardApertureEllipse;                        break;
   case standardApertureHalfOval:                       retval = standardApertureHalfOval;                       break;
   case standardApertureHole:                           retval = standardApertureHole;                           break;
   case standardApertureHorizontalHexagon:              retval = standardApertureHorizontalHexagon;              break;
   case standardApertureInvalid:                        retval = standardApertureInvalid;                        break;
   case standardApertureMoire:                          retval = standardApertureMoire;                          break;
   case standardApertureNotchedRectangle:               retval = standardApertureNotchedRectangle;               break;
   case standardApertureNull:                           retval = standardApertureNull;                           break;
   case standardApertureOctagon:                        retval = standardApertureOctagon;                        break;
   case standardApertureOval:                           retval = standardApertureOval;                           break;
   case standardApertureRectangle:                      retval = standardApertureRectangle;                      break;
   case standardApertureRectangularThermal:             retval = standardApertureRectangularThermal;             break;
   case standardApertureRectangularThermalOpenCorners:  retval = standardApertureRectangularThermalOpenCorners;  break;
   case standardApertureRoundDonut:                     retval = standardApertureRoundDonut;                     break;
   case standardApertureRoundedRectangle:               retval = standardApertureRoundedRectangle;               break;
   case standardApertureRoundThermalRounded:            retval = standardApertureRoundThermalRounded;            break;
   case standardApertureRoundThermalSquare:             retval = standardApertureRoundThermalSquare;             break;
   case standardApertureSpurredRectangle:               retval = standardApertureSpurredRectangle;               break;
   case standardApertureSquare:                         retval = standardApertureSquare;                         break;
   case standardApertureSquareButterfly:                retval = standardApertureSquareButterfly;                break;
   case standardApertureSquareDonut:                    retval = standardApertureSquareDonut;                    break;
   case standardApertureRectangledPentagon:             retval = standardApertureRectangledPentagon;             break;
   case standardApertureSquareRoundThermal:             retval = standardApertureSquareRoundThermal;             break;
   case standardApertureSquareThermal:                  retval = standardApertureSquareThermal;                  break;
   case standardApertureSquareThermalOpenCorners:       retval = standardApertureSquareThermalOpenCorners;       break;
   case standardApertureTabbedRectangle:                retval = standardApertureTabbedRectangle;                break;
   case standardApertureTriangle:                       retval = standardApertureTriangle;                       break;
   case standardApertureVeeCutRectangle:                retval = standardApertureVeeCutRectangle;                break;
   case standardApertureVerticalHexagon:                retval = standardApertureVerticalHexagon;                break;
   case standardApertureUndefined:
   default:                                             retval = standardApertureUndefined;                      break;
   }

   return retval;
}

StandardApertureTypeTag descriptorPrefixToStandardApertureTag(CString descriptorPrefix)
{
   StandardApertureTypeTag retval = standardApertureUndefined;
   descriptorPrefix.Trim().MakeLower();

   if (descriptorPrefix == "round" || descriptorPrefix == "r")
   {
      retval = standardApertureCircle;
   }
   else if (descriptorPrefix == "splitround" || descriptorPrefix == "sr")
   {
      retval = standardApertureSplitCircle;
   }
   else if (descriptorPrefix == "square" || descriptorPrefix == "s")
   {
      retval = standardApertureSquare;
   }
   else if (descriptorPrefix == "rectangle" || descriptorPrefix == "rect")
   {
      retval = standardApertureRectangle;
   }
   else if (descriptorPrefix == "beveledrectangle")
   {
      retval = standardApertureBeveledRectangle;
   }
   else if (descriptorPrefix == "chamferedrectangle")
   {
      retval = standardApertureChamferedRectangle;
   }
   else if (descriptorPrefix == "roundedrectangle")
   {
      retval = standardApertureRoundedRectangle;
   }
   else if (descriptorPrefix == "oval")
   {
      retval = standardApertureOval;
   }
   else if (descriptorPrefix == "diamond" || descriptorPrefix == "di")
   {
      retval = standardApertureDiamond;
   }
   else if (descriptorPrefix == "octagon" || descriptorPrefix == "oct")
   {
      retval = standardApertureOctagon;
   }
   else if (descriptorPrefix == "donut" || descriptorPrefix == "donut_r")
   {
      retval = standardApertureRoundDonut;
   }
   else if (descriptorPrefix == "squaredonut" || descriptorPrefix == "donut_s")
   {
      retval = standardApertureSquareDonut;
   }
   else if (descriptorPrefix == "hexagon" || descriptorPrefix == "hex_l")
   {
      retval = standardApertureHorizontalHexagon;
   }
   else if (descriptorPrefix == "verticalhexagon" || descriptorPrefix == "hex_s")
   {
      retval = standardApertureVerticalHexagon;
   }
   else if (descriptorPrefix == "butterfly" || descriptorPrefix == "bfr")
   {
      retval = standardApertureButterfly;
   }
   else if (descriptorPrefix == "squarebutterfly" || descriptorPrefix == "bfs")
   {
      retval = standardApertureSquareButterfly;
   }
   else if (descriptorPrefix == "triangle" || descriptorPrefix == "tri")
   {
      retval = standardApertureTriangle;
   }
   else if (descriptorPrefix == "halfoval" || descriptorPrefix == "oval_h")
   {
      retval = standardApertureHalfOval;
   }
   else if (descriptorPrefix == "roundthermalrounded" || descriptorPrefix == "thr")
   {
      retval = standardApertureRoundThermalRounded;
   }
   else if (descriptorPrefix == "roundthermalsquare" || descriptorPrefix == "ths")
   {
      retval = standardApertureRoundThermalSquare;
   }
   else if (descriptorPrefix == "squarethermal" || descriptorPrefix == "s_ths")
   {
      retval = standardApertureSquareThermal;
   }
   else if (descriptorPrefix == "squarethermalopencorners" || descriptorPrefix == "s_tho")
   {
      retval = standardApertureSquareThermalOpenCorners;
   }
   else if (descriptorPrefix == "squareroundthermal" || descriptorPrefix == "sr_ths")
   {
      retval = standardApertureSquareRoundThermal;
   }
   else if (descriptorPrefix == "rectangularthermal" || descriptorPrefix == "rc_ths")
   {
      retval = standardApertureRectangularThermal;
   }
   else if (descriptorPrefix == "rectangularthermalopencorners" || descriptorPrefix == "rc_tho")
   {
      retval = standardApertureRectangularThermalOpenCorners;
   }
   else if (descriptorPrefix == "notchedrectangle" || descriptorPrefix == "notched")
   {
      retval = standardApertureNotchedRectangle;
   }
   else if (descriptorPrefix == "spurredrectangle" || descriptorPrefix == "spurred")
   {
      retval = standardApertureSpurredRectangle;
   }
   else if (descriptorPrefix == "tabbedrectangle" || descriptorPrefix == "tabbed")
   {
      retval = standardApertureTabbedRectangle;
   }
   else if (descriptorPrefix == "veecutrectangle" || descriptorPrefix == "veecut")
   {
      retval = standardApertureVeeCutRectangle;
   }
   else if (descriptorPrefix == "rectangledpentagon" || descriptorPrefix == "rc_pentagon")
   {
      retval = standardApertureRectangledPentagon;
   }
   else if (descriptorPrefix == "ellipse" || descriptorPrefix == "el")
   {
      retval = standardApertureEllipse;
   }
   else if (descriptorPrefix == "moire")
   {
      retval = standardApertureMoire;
   }
   else if (descriptorPrefix == "hole")
   {
      retval = standardApertureHole;
   }
   else if (descriptorPrefix == "null")
   {
      retval = standardApertureNull;
   }
   else
   {
      retval = standardApertureUndefined;
   }

   return retval;
}

StandardApertureTypeTag stringToStandardApertureTag(const CString& string)
{
   StandardApertureTypeTag retval = standardApertureUndefined;

   for (int standardApertureType = standardApertureFirstValid;
        standardApertureType <= standardApertureLastValid;
        standardApertureType++)
   {
      StandardApertureTypeTag standardApertureTypeTag = 
         (StandardApertureTypeTag)standardApertureType;

      if ((string.CompareNoCase(standardApertureTypeTagToString(standardApertureTypeTag                )) == 0) ||
          (string.CompareNoCase(standardApertureTypeTagToDescriptorPrefix(standardApertureTypeTag      )) == 0) ||
          (string.CompareNoCase(standardApertureTypeTagToValorDescriptorPrefix(standardApertureTypeTag )) == 0) ||
          (string.CompareNoCase(standardApertureTypeTagToApertureNameDescriptor(standardApertureTypeTag)) == 0)    )
      {
         retval = standardApertureTypeTag;
         break;
      }
   }

   return retval;
}

//_____________________________________________________________________________
CString centerTagToString(CenterTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case centerOfExtents:    retval = "centerOfExtents";    break;
   case centerOfRectangle:  retval = "centerOfRectangle";  break;
   case centerOfMass:       retval = "centerOfMass";       break;
   case centerOfVertices:   retval = "centerOfVertices";   break;
   default:                 retval = "centerOfUndefined";   break;
   }

   return retval;
}

CString centerTagToDescription(CenterTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case centerOfExtents:    retval = "Center of Extents";     break;
   case centerOfRectangle:  retval = "Center of Rectangle";   break;
   case centerOfMass:       retval = "Center of Mass";        break;
   case centerOfVertices:   retval = "Center of Vertices";    break;
   default:                 retval = "Center of Undefined";   break;
   }

   return retval;
}

CString centerTagToDescriptorInfix(CenterTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case centerOfExtents:    retval = "coe";  break;
   case centerOfRectangle:  retval = "cor";  break;
   case centerOfMass:       retval = "com";  break;
   case centerOfVertices:   retval = "cov";  break;
   }

   return retval;
}

CenterTag intToCenterTag(int tagValue)
{
   CenterTag retval=centerOfUndefined;

   switch (tagValue)
   {
   case centerOfExtents:    retval = centerOfExtents;    break;
   case centerOfRectangle:  retval = centerOfRectangle;  break;
   case centerOfMass:       retval = centerOfMass;       break;
   case centerOfVertices:   retval = centerOfVertices;   break;
   }

   return retval;
}

//_____________________________________________________________________________
CenterTag CCenterTag::getNext(CenterTag center)
{
   CenterTag retval = centerOfExtents;

   if (center != centerOfMaskEmpty)
   {
      retval = (CenterTag)((center << 1) & centerOfMask);
   }

   return retval;
}

int CCenterTag::getCount() const              
{ 
   return contains(centerOfExtents)   +
          contains(centerOfRectangle) +
          contains(centerOfMass)      +
          contains(centerOfVertices); 
}

//_____________________________________________________________________________
CStandardApertureDialogParameter::CStandardApertureDialogParameter(
   CStatic& parameterCaption,CEdit& parameterEditBox,
   CStatic& parameterDescription,CStatic& genericParameterCaption) :
      m_parameterCaption(parameterCaption),
      m_parameterEditBox(parameterEditBox),
      m_parameterDescription(&parameterDescription),
      m_genericParameterCaption(&genericParameterCaption)

{
}

CStandardApertureDialogParameter::CStandardApertureDialogParameter(
   CStatic& parameterCaption,CEdit& parameterEditBox,CStatic& parameterDescription) :
      m_parameterCaption(parameterCaption),
      m_parameterEditBox(parameterEditBox),
      m_parameterDescription(&parameterDescription),
      m_genericParameterCaption(NULL)

{
}

CString CStandardApertureDialogParameter::getValue() const
{
   CString value;

   m_parameterEditBox.GetWindowText(value);

   return value;
}

void CStandardApertureDialogParameter::setValue(const CString& value)
{
   m_parameterEditBox.SetWindowText(value);
   m_parameterEditBox.ShowWindow(SW_SHOW);
}

CString CStandardApertureDialogParameter::getCaption() const
{
   CString caption;

   m_parameterCaption.GetWindowText(caption);

   return caption;
}

void CStandardApertureDialogParameter::setCaption(const CString& caption)
{
   m_parameterCaption.SetWindowText(caption);
   m_parameterCaption.ShowWindow(SW_SHOW);

   if (m_genericParameterCaption != NULL)
   {
      m_genericParameterCaption->SetWindowText(caption);
      m_genericParameterCaption->ShowWindow(SW_SHOW);
   }
}

CString CStandardApertureDialogParameter::getDescription() const
{
   CString description;

   if (m_parameterDescription != NULL)
   {
      m_parameterDescription->GetWindowText(description);
   }

   return description;
}

void CStandardApertureDialogParameter::setDescription(const CString& description)
{
   m_description = description;

   if (m_parameterDescription != NULL)
   {
      m_parameterDescription->SetWindowText(description);
      m_parameterDescription->ShowWindow(SW_SHOW);
   }
}

void CStandardApertureDialogParameter::clear()
{
   CString emptyString;

   m_parameterCaption.SetWindowText(emptyString);
   m_parameterEditBox.GetWindowText(emptyString);

   m_parameterCaption.ShowWindow(SW_HIDE);
   m_parameterEditBox.ShowWindow(SW_HIDE);

   if (m_parameterDescription != NULL)
   {
      m_parameterDescription->SetWindowText(emptyString);
      m_parameterDescription->ShowWindow(SW_HIDE);
   }

   if (m_genericParameterCaption != NULL)
   {
      m_genericParameterCaption->SetWindowText(emptyString);
      m_genericParameterCaption->ShowWindow(SW_HIDE);
   }
}

//_____________________________________________________________________________
CStencilApertureDialogParameter::CStencilApertureDialogParameter(
   CStatic& parameterCaption,CEdit& parameterEditBox,CStatic& parameterDescription)
: CStandardApertureDialogParameter(parameterCaption,parameterEditBox,parameterDescription)
{
}

CString CStencilApertureDialogParameter::getCaption() const
{
   return m_caption;
}

void CStencilApertureDialogParameter::setCaption(const CString& caption)
{
   m_caption = caption;
   m_parameterCaption.SetWindowText(getDescriptiveCaption());
   m_parameterCaption.ShowWindow(SW_SHOW);
}

CString CStencilApertureDialogParameter::getDescription() const
{
   CString description;

   if (m_description.GetLength() >= m_maxDescriptionLength)
   {
      description = m_description;
   }

   return description;
}

void CStencilApertureDialogParameter::setDescription(const CString& description)
{
   m_description = description;

   m_parameterCaption.SetWindowText(getDescriptiveCaption());
   m_parameterCaption.ShowWindow(SW_SHOW);

   if (m_parameterDescription != NULL)
   {
      m_parameterDescription->SetWindowText(getDescription());
      m_parameterDescription->ShowWindow(SW_SHOW);
   }
}

CString CStencilApertureDialogParameter::getDescriptiveCaption() const
{
   CString caption(m_caption);

   if (m_description.GetLength() < m_maxDescriptionLength)
   {
      caption += " - " + m_description;
   }

   return caption;
}

//_____________________________________________________________________________
CStandardApertureDialogParameters::CStandardApertureDialogParameters(bool displayUnitsInPageUnitsFlag)
   : m_displayUnitsInPageUnitsFlag(displayUnitsInPageUnitsFlag)
{
   m_parameters.SetSize(0,6);
   m_originGroupBox = NULL;
   m_originOptions  = NULL;
   m_displayCenters = NULL;
}

void CStandardApertureDialogParameters::addParameter(CStatic& parameterCaption,CEdit& parameterEditBox,
   CStatic& genericParameterDescription,CStatic& genericParameterCaption)
{
   CStandardApertureDialogParameter* parameter = new CStandardApertureDialogParameter(
      parameterCaption,parameterEditBox,
      genericParameterDescription,genericParameterCaption);

   m_parameters.Add(parameter);
}

void CStandardApertureDialogParameters::addParameter(CStatic& parameterCaption,CEdit& parameterEditBox,
   CStatic& genericParameterDescription)
{
   CStencilApertureDialogParameter* parameter = new CStencilApertureDialogParameter(
      parameterCaption,parameterEditBox,
      genericParameterDescription);

   m_parameters.Add(parameter);
}

void CStandardApertureDialogParameters::setParameters(int index,
   const CString& caption,const CString& description,
   const CString& descriptorDelimiter,const CString& value)
{
   if (index >= 0 && index < m_parameters.GetSize())
   {
      CStandardApertureDialogParameter* parameter = m_parameters.GetAt(index);

      if (parameter != NULL)
      {
         parameter->setCaption(caption);
         parameter->setDescription(description);
         parameter->setValue(value);
         parameter->setDescriptorDelimiter(descriptorDelimiter);
      }
   }
}

void CStandardApertureDialogParameters::setParameters(int index,
   const CString& caption,const CString& description,
   const CString& descriptorDelimiter,const CString& value,double rawValue)
{
   if (m_displayUnitsInPageUnitsFlag)
   {
      if (index >= 0 && index < m_parameters.GetSize())
      {
         CStandardApertureDialogParameter* parameter = m_parameters.GetAt(index);

         if (parameter != NULL)
         {
            parameter->setCaption(caption);
            parameter->setDescription(description);
            parameter->setValue(fpfmt(rawValue,4));
            parameter->setDescriptorDelimiter(descriptorDelimiter);
         }
      }
   }
   else
   {
      setParameters(index,caption,description,descriptorDelimiter,value);
   }
}

void CStandardApertureDialogParameters::addCenters(CStatic& originGroupBox,CComboBox& originOptions,CButton& displayCenters)
{
   m_originGroupBox = &originGroupBox;
   m_originOptions  = &originOptions;
   m_displayCenters = &displayCenters;
}

void CStandardApertureDialogParameters::setCenters(const CCenterTag& centers,CenterTag setCenter)
{
   if (centers.getCount() > 1)
   {
      if (m_originOptions != NULL)
      {
         m_originOptions->ShowWindow(SW_SHOW);
         int selectedIndex = -1;

         for (CenterTag center = CCenterTag::getNext();center != centerOfUndefined;
              center = CCenterTag::getNext(center))
         {
            if (centers.contains(center))
            {
               int index = m_originOptions->AddString(centerTagToDescription(center));
               m_originOptions->SetItemData(index,center);

               if (center == setCenter)
               {
                  selectedIndex = index;
               }
            }
         }

         m_originOptions->SetCurSel(selectedIndex);
      }

      if (m_originGroupBox != NULL)
      {
         m_originGroupBox->ShowWindow(SW_SHOW);
      }

      if (m_displayCenters != NULL)
      {
         m_displayCenters->ShowWindow(SW_SHOW);
      }
   }
   else
   {
      clearCenters();
   }
}

CStandardApertureDialogParameter* CStandardApertureDialogParameters::lookupCaption(const CString& caption)
{
   CStandardApertureDialogParameter* parameter = NULL;

   for (int index = 0;index < m_parameters.GetCount();index++)
   {
      parameter = m_parameters.GetAt(index);

      if (parameter != NULL && parameter->getCaption().CompareNoCase(caption) == 0)
      {
         break;
      }

      parameter = NULL;
   }

   return parameter;
}

CStandardApertureDialogParameter* CStandardApertureDialogParameters::lookupCaptionPrefix(const CString& captionPrefix)
{
   CStandardApertureDialogParameter* parameter = NULL;

   for (int index = 0;index < m_parameters.GetCount();index++)
   {
      parameter = m_parameters.GetAt(index);

      if (parameter != NULL)
      {
         CString parameterCaption = parameter->getCaption();

         if (parameterCaption.GetLength() > captionPrefix.GetLength())
         {
            parameterCaption.GetBufferSetLength(captionPrefix.GetLength());
         }

         if (parameterCaption.CompareNoCase(captionPrefix) == 0)
         {
            break;
         }
      }

      parameter = NULL;
   }

   return parameter;
}

void CStandardApertureDialogParameters::clear()
{
   for (int index = 0;index < m_parameters.GetCount();index++)
   {
      CStandardApertureDialogParameter* parameter = m_parameters.GetAt(index);

      if (parameter != NULL)
      {
         parameter->clear();
      }
   }

   clearCenters();
}

void CStandardApertureDialogParameters::clearCenters()
{
   if (m_originGroupBox != NULL)
   {
      m_originGroupBox->ShowWindow(SW_HIDE);
   }

   if (m_originOptions != NULL)
   {
      m_originOptions->ResetContent();
      m_originOptions->ShowWindow(SW_HIDE);
   }

   if (m_displayCenters != NULL)
   {
      m_displayCenters->ShowWindow(SW_HIDE);
   }
}

CString CStandardApertureDialogParameters::getDescriptorTemplate() const
{
   CString descriptorTemplate = m_descriptorPrefix;
   int parameterCount = 0;

   for (int index = 0;index < m_parameters.GetCount();index++)
   {
      CStandardApertureDialogParameter* parameter = m_parameters.GetAt(index);

      if (parameter != NULL)
      {
         if (! parameter->getCaption().IsEmpty())
         {
            if (parameterCount > 0)
            {
               descriptorTemplate += parameter->getDescriptorDelimiter();
            }

            descriptorTemplate += "<";
            descriptorTemplate += parameter->getCaption();
            descriptorTemplate += ">";
            parameterCount++;
         }
      }
   }

   return descriptorTemplate;
}

CenterTag CStandardApertureDialogParameters::getCenter() const
{
   CenterTag center = centerOfUndefined;

   if (m_originOptions != NULL)
   {
      int selectedIndex = m_originOptions->GetCurSel();

      if (selectedIndex >= 0)
      {
         center = intToCenterTag(m_originOptions->GetItemData(selectedIndex));
      }
   }

   return center;
}

//_____________________________________________________________________________
PageUnitsTag CStandardApertureShape::m_defaultPageUnits = pageUnitsMils;
double CStandardApertureShape::m_defaultDescriptorDimensionFactor = 1.0;
CString CStandardApertureShape::m_descriptorDelimiter("x");

CStandardApertureShape* CStandardApertureShape::create(StandardApertureTypeTag type,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent)
{
   CStandardApertureShape* standardApertureShape = NULL;

   if (descriptorDimensionFactorExponent < 0)
   {
      descriptorDimensionFactorExponent = CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(pageUnits);
   }

   switch (type)
   {
   case standardApertureButterfly:                      standardApertureShape = new CStandardApertureButterfly(                    pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureCircle:                         standardApertureShape = new CStandardApertureCircle(                       pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSplitCircle:                    standardApertureShape = new CStandardApertureSplitCircle(                  pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureDiamond:                        standardApertureShape = new CStandardApertureDiamond(                      pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureEllipse:                        standardApertureShape = new CStandardApertureEllipse(                      pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureHalfOval:                       standardApertureShape = new CStandardApertureHalfOval(                     pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureHole:                           standardApertureShape = new CStandardApertureHole(                         pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureHorizontalHexagon:              standardApertureShape = new CStandardApertureHorizontalHexagon(            pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureMoire:                          standardApertureShape = new CStandardApertureMoire(                        pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureNotchedRectangle:               standardApertureShape = new CStandardApertureNotchedRectangle(             pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureNull:                           standardApertureShape = new CStandardApertureNull(                         pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureOctagon:                        standardApertureShape = new CStandardApertureOctagon(                      pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureOval:                           standardApertureShape = new CStandardApertureOval(                         pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRectangle:                      standardApertureShape = new CStandardApertureRectangle(                    pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRoundedRectangle:               standardApertureShape = new CStandardApertureRoundedRectangle(             pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureBeveledRectangle:               standardApertureShape = new CStandardApertureBeveledRectangle(             pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureChamferedRectangle:             standardApertureShape = new CStandardApertureChamferedRectangle(           pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRectangularThermal:             standardApertureShape = new CStandardApertureRectangularThermal(           pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRectangularThermalOpenCorners:  standardApertureShape = new CStandardApertureRectangularThermalOpenCorners(pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRoundDonut:                     standardApertureShape = new CStandardApertureRoundDonut(                   pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRoundThermalRounded:            standardApertureShape = new CStandardApertureRoundThermalRounded(          pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRoundThermalSquare:             standardApertureShape = new CStandardApertureRoundThermalSquare(           pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSpurredRectangle:               standardApertureShape = new CStandardApertureSpurredRectangle(             pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSquare:                         standardApertureShape = new CStandardApertureSquare(                       pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSquareButterfly:                standardApertureShape = new CStandardApertureSquareButterfly(              pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSquareDonut:                    standardApertureShape = new CStandardApertureSquareDonut(                  pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureRectangledPentagon:             standardApertureShape = new CStandardApertureRectangledPentagon(           pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSquareRoundThermal:             standardApertureShape = new CStandardApertureSquareRoundThermal(           pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSquareThermal:                  standardApertureShape = new CStandardApertureSquareThermal(                pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureSquareThermalOpenCorners:       standardApertureShape = new CStandardApertureSquareThermalOpenCorners(     pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureTabbedRectangle:                standardApertureShape = new CStandardApertureTabbedRectangle(              pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureTriangle:                       standardApertureShape = new CStandardApertureTriangle(                     pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureVeeCutRectangle:                standardApertureShape = new CStandardApertureVeeCutRectangle(              pageUnits,descriptorDimensionFactorExponent);  break;
   case standardApertureVerticalHexagon:                standardApertureShape = new CStandardApertureVerticalHexagon(              pageUnits,descriptorDimensionFactorExponent);  break;
   default:                                             standardApertureShape = new CStandardApertureInvalid(                      pageUnits,descriptorDimensionFactorExponent);  break;
   }

   return standardApertureShape;
}

CStandardApertureShape* CStandardApertureShape::create(CString descriptor,PageUnitsTag pageUnits)
{
   CStandardApertureShape* standardApertureShape = NULL;

   CStringArray params;
   CString rotationString;
   CString typeString;
   CString exponentString;

   //double pageUnitsPerDescriptorUnits = 1.;
   //double dimension[6];
   int descriptorDimensionFactorExponent = 0;
   double rotation = 0.;

   PageUnitsTag destinationPageUnits = pageUnits;
   bool metricFlag  = false;
   bool englishFlag = false;
   CenterTag centerTag = centerOfUndefined;

   //pageUnitsPerDescriptorUnits = getUnitsFactor(getDisplayPageUnits(pageUnits),pageUnits) / descriptorDimensionFactor;

   //int minParamCount = -1;
   //int maxParamCount =  0;
   int numParams     =  0;
   int pos           = descriptor.FindOneOf("0123456789.");

   if (pos >= 0)
   {
      typeString      = descriptor.Left(pos);
      CSupString name = descriptor.Mid(pos);
      numParams       = name.Parse(params,"x");

      if (numParams > 0)
      {
         CString* lastParam = &(params[numParams - 1]);
         CString tail(lastParam != NULL ? *lastParam : "");

         for (centerTag = CCenterTag::getNext();;centerTag = CCenterTag::getNext(centerTag))
         {
            if (centerTag == centerOfUndefined)
            {
               centerTag = centerOfExtents;
               break;
            }

            CString descriptorInfix = centerTagToDescriptorInfix(centerTag);

            pos = tail.Find(descriptorInfix);

            if (pos >= 0)
            {
               if (lastParam != NULL)
               {
                  *lastParam = lastParam->Left(pos);
                  lastParam  = NULL;
               }

               tail = tail.Mid(pos + descriptorInfix.GetLength());
               break;
            }
         }

         pos = tail.ReverseFind('m');

         if (pos >= 0)
         {
            metricFlag = true;

            exponentString = tail.Mid(pos + 1);
            descriptorDimensionFactorExponent = atoi(exponentString);

            if (lastParam != NULL)
            {
               *lastParam = lastParam->Left(pos);
               lastParam  = NULL;
            }
         }
         else
         {
            pos = tail.ReverseFind('e');

            if (pos >= 0)
            {
               englishFlag = true;

               exponentString = tail.Mid(pos + 1);
               descriptorDimensionFactorExponent = atoi(exponentString);

               if (lastParam != NULL)
               {
                  *lastParam = lastParam->Left(pos);
                  lastParam  = NULL;
               }
            }
         }

         pos = tail.ReverseFind('_');

         if (pos >= 0)
         {
            rotationString = tail.Mid(pos + 1);

            if (lastParam != NULL)
            {
               *lastParam = lastParam->Left(pos);
               lastParam  = NULL;
            }
         }
      }

      StandardApertureTypeTag type = standardApertureUndefined;

      if (rotationString.GetLength() != 0)
      {
         char* endPtr;
         rotation = normalizeDegrees(-strtod(rotationString,&endPtr));

         if (*endPtr != '\0')
         {
            type = standardApertureInvalid;
         }
      }

      if (type == standardApertureUndefined)
      {
         type = descriptorPrefixToStandardApertureTag(typeString);
      }

      if (englishFlag && !::isEnglish(pageUnits))
      {
         pageUnits = pageUnitsMils;
      }
      else if (metricFlag && !::isMetric(pageUnits))
      {
         pageUnits = pageUnitsMilliMeters;
      }
      else if (!englishFlag && !metricFlag)
      {
         pageUnits = pageUnitsMils;
      }

      if (type == standardApertureRectangle          || 
          type == standardApertureRoundedRectangle   || 
          type == standardApertureChamferedRectangle    )
      {
         if (numParams > 2)
         {
            if (params[2].Left(1) == "r")
            {
               params[2] = params[2].Mid(1);

               if (type == standardApertureRectangle) type = standardApertureRoundedRectangle;
            }
            else if (params[2].Left(1) == "c")
            {
               params[2] = params[2].Mid(1);

               if (type == standardApertureRectangle) type = standardApertureChamferedRectangle;
            }
         }
      }

      switch (type)
      {
      case standardApertureButterfly:                      standardApertureShape = new CStandardApertureButterfly(                    params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureCircle:                         standardApertureShape = new CStandardApertureCircle(                       params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSplitCircle:                    standardApertureShape = new CStandardApertureSplitCircle(                  params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureDiamond:                        standardApertureShape = new CStandardApertureDiamond(                      params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureEllipse:                        standardApertureShape = new CStandardApertureEllipse(                      params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureHalfOval:                       standardApertureShape = new CStandardApertureHalfOval(                     params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureHole:                           standardApertureShape = new CStandardApertureHole(                         params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureHorizontalHexagon:              standardApertureShape = new CStandardApertureHorizontalHexagon(            params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureMoire:                          standardApertureShape = new CStandardApertureMoire(                        params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureNotchedRectangle:               standardApertureShape = new CStandardApertureNotchedRectangle(             params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureNull:                           standardApertureShape = new CStandardApertureNull(                         params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureOctagon:                        standardApertureShape = new CStandardApertureOctagon(                      params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureOval:                           standardApertureShape = new CStandardApertureOval(                         params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRectangle:                      standardApertureShape = new CStandardApertureRectangle(                    params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRoundedRectangle:               standardApertureShape = new CStandardApertureRoundedRectangle(             params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureChamferedRectangle:             standardApertureShape = new CStandardApertureChamferedRectangle(           params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureBeveledRectangle:               standardApertureShape = new CStandardApertureBeveledRectangle(             params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRectangularThermal:             standardApertureShape = new CStandardApertureRectangularThermal(           params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRectangularThermalOpenCorners:  standardApertureShape = new CStandardApertureRectangularThermalOpenCorners(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRoundDonut:                     standardApertureShape = new CStandardApertureRoundDonut(                   params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRoundThermalRounded:            standardApertureShape = new CStandardApertureRoundThermalRounded(          params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRoundThermalSquare:             standardApertureShape = new CStandardApertureRoundThermalSquare(           params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSpurredRectangle:               standardApertureShape = new CStandardApertureSpurredRectangle(             params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSquare:                         standardApertureShape = new CStandardApertureSquare(                       params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSquareButterfly:                standardApertureShape = new CStandardApertureSquareButterfly(              params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSquareDonut:                    standardApertureShape = new CStandardApertureSquareDonut(                  params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureRectangledPentagon:             standardApertureShape = new CStandardApertureRectangledPentagon(           params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSquareRoundThermal:             standardApertureShape = new CStandardApertureSquareRoundThermal(           params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSquareThermal:                  standardApertureShape = new CStandardApertureSquareThermal(                params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureSquareThermalOpenCorners:       standardApertureShape = new CStandardApertureSquareThermalOpenCorners(     params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureTabbedRectangle:                standardApertureShape = new CStandardApertureTabbedRectangle(              params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureTriangle:                       standardApertureShape = new CStandardApertureTriangle(                     params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureVerticalHexagon:                standardApertureShape = new CStandardApertureVerticalHexagon(              params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      case standardApertureVeeCutRectangle:                standardApertureShape = new CStandardApertureVeeCutRectangle(              params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      default:                                             standardApertureShape = new CStandardApertureInvalid(                      params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent);  break;
      }
   }

   if (standardApertureShape == NULL ||
        (! standardApertureShape->isValid() && standardApertureShape->getType() != standardApertureInvalid))
   {
      delete standardApertureShape;
      standardApertureShape = new CStandardApertureInvalid(params,centerTag,rotation,pageUnits,0);
   }
   else if (destinationPageUnits != pageUnits)
   {
      standardApertureShape->changeUnits(destinationPageUnits);

      standardApertureShape->setDescriptorDimensionFactorExponent(standardApertureShape->isMetric() ? 2 : 0);
   }

   return standardApertureShape;
}

CStandardApertureShape* CStandardApertureShape::create(const CStandardApertureShape& other)
{
   CStandardApertureShape* standardApertureShape = create(other.getDescriptor(),other.getPageUnits());

   standardApertureShape->setExteriorCornerRadius(other.getExteriorCornerRadius());

   return standardApertureShape;
}

PageUnitsTag CStandardApertureShape::getDisplayPageUnits(PageUnitsTag pageUnits)
{
   PageUnitsTag displayPageUnits;

   switch(pageUnits)
   {
   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsHpPlotter:
   default:
      displayPageUnits = pageUnitsMils;

      break;
   case pageUnitsMilliMeters:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      displayPageUnits = pageUnitsMilliMeters;

      break;
   }

   return displayPageUnits;
}

int CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(PageUnitsTag pageUnits)
{
   return (::isMetric(pageUnits) ? 3 : 0); // Case dts0100482778, support 3 decimal places for metric stencils
}

double CStandardApertureShape::getDimensionArrowLength() const
{
   return .09*getDefaultSize();
}

double CStandardApertureShape::getDefaultSize() const
{
   double defaultSize = 1.;
   PageUnitsTag pageUnits = getPageUnits();

   if (pageUnits == pageUnitsUndefined)
   {
      pageUnits = getDefaultPageUnits();
   }

   switch (pageUnits)
   {
   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsHpPlotter:
      defaultSize = 40. * getUnitsFactor(pageUnitsMils,getDefaultPageUnits());

      break;
   case pageUnitsMilliMeters:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      defaultSize = 1. * getUnitsFactor(pageUnitsMilliMeters,getDefaultPageUnits());

      break;
   }

   return defaultSize;
}

// 2--1
// |  |
// 3--4
int CStandardApertureShape::cornerSymmetry(int corners)
{
   int symmetry = 360;
   unsigned short mask = 0;

   for (int ind = 0;ind < 4;ind++)
   {
      switch (corners % 10)
      {
      case 1:  mask |= 1;  break;
      case 2:  mask |= 2;  break;
      case 3:  mask |= 4;  break;
      case 4:  mask |= 8;  break;
      }

      corners /= 10;
   }

   switch (mask)
   {
   case 0x0:  /* 0000 */  symmetry =  90;  break;
   case 0x5:  /* 0101 */  symmetry = 180;  break;
   case 0xa:  /* 1010 */  symmetry = 180;  break;
   case 0xf:  /* 1111 */  symmetry =  90;  break;
   }

   return symmetry;
}

int CStandardApertureShape::getCornerFlags(int corners,bool& corner1,bool& corner2,bool& corner3,bool& corner4)
{
   int cornerCount = 0;

   corner1 = false;
   corner2 = false;
   corner3 = false;
   corner4 = false;

   if (corners != 0)
   {
      while (corners != 0)
      {
         int corner = corners % 10;

         switch (corner)
         {
         case 1: corner1 = true;  break;
         case 2: corner2 = true;  break;
         case 3: corner3 = true;  break;
         case 4: corner4 = true;  break;
         }

         corners /= 10;
      }

      cornerCount = corner1 + corner2 + corner3 + corner4;
   }

   if (cornerCount == 0)
   {
      corner1 = true;
      corner2 = true;
      corner3 = true;
      corner4 = true;

      cornerCount = 4;
   }

   return cornerCount;
}

int CStandardApertureShape::getFeatureFlags(int features,bool& feature1,bool& feature2,bool& feature3,bool& feature4)
{
   return getCornerFlags(features,feature1,feature2,feature3,feature4);
}

int CStandardApertureShape::getCornerCount(int corners)
{
   bool corner1,corner2,corner3,corner4;
   int cornerCount = getCornerFlags(corners,corner1,corner2,corner3,corner4);

   return cornerCount;
}

int CStandardApertureShape::getFeatureCount(int features)
{
   return getCornerCount(features);
}

bool CStandardApertureShape::hasCorner(int corners,int cornerNumber)
{
   bool retval = (corners == 0);

   while (corners != 0 && !retval)
   {
      retval = (corners%10 == cornerNumber);
      corners /= 10;
   }

   return retval;
}

bool CStandardApertureShape::hasFeature(int features,int featureNumber)
{
   return hasCorner(features,featureNumber);
}

void CStandardApertureShape::buildSpokes(CCamCadData& camCadData,DataStruct* polyStruct,
   double outerRadius,double startAngleDegrees,int numSpokes,double spokeWidth)
{
   int widthIndex = camCadData.getDefinedWidthIndex(spokeWidth);

   for (int spokeInd = 0;spokeInd < numSpokes;spokeInd++)
   {
      double angle = degreesToRadians(startAngleDegrees + (spokeInd * 360./numSpokes));

      CPoly* poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

      double x = outerRadius * cos(angle);
      double y = outerRadius * sin(angle);

      camCadData.addVertex(*poly,0.,0.);
      camCadData.addVertex(*poly, x, y);
   }
}

void CStandardApertureShape::buildThermalOpenCorners(CCamCadData& camCadData,DataStruct* polyStruct,
   double width,double height,double airGap,
   double startAngleDegrees,int numSpokes,double spokeWidth)
{
   double id = width - airGap;
   double theta = atan(((spokeWidth/2.)/id - sin(Pi/numSpokes)) / (-cos(Pi/numSpokes)));
   double w = id * tan(theta);
   double alpha = (Pi / numSpokes) - theta;
   double thetaDegrees = radiansToDegrees(theta);

   for (int ind = 0;ind < numSpokes;ind++)
   {
      CTMatrix matrix;
      matrix.rotateDegrees(startAngleDegrees - 2.*thetaDegrees + ind*360./numSpokes);
      matrix.scale(1.,height/width);

      CPoint2d p0(id         , w);
      CPoint2d p1(id         ,-w);
      CPoint2d p2(id + airGap,-w);
      CPoint2d p3(id + airGap, w);

      matrix.transform(p0);
      matrix.transform(p1);
      matrix.transform(p2);
      matrix.transform(p3);

      CPoly* poly = camCadData.addFilledPoly(*polyStruct,0);
      camCadData.addVertex(*poly,p0.x,p0.y);
      camCadData.addVertex(*poly,p1.x,p1.y);
      camCadData.addVertex(*poly,p2.x,p2.y);
      camCadData.addVertex(*poly,p3.x,p3.y);
      camCadData.addVertex(*poly,p0.x,p0.y);
   }
}

CStandardApertureShape::CStandardApertureShape(int symmetry,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   m_pageUnits(pageUnits),
   m_descriptorDimensionFactorExponent(descriptorDimensionFactorExponent),
   //m_descriptorDimensionFactor(getDefaultDescriptorDimensionFactor()),
   m_center(centerOfExtents),
   m_symmetry(symmetry)
{
   m_isValid = true;

   m_dimension[0] = 0.;
   m_dimension[1] = 0.;
   m_dimension[2] = 0.;
   m_dimension[3] = 0.;
   m_dimension[4] = 0.;
   m_dimension[5] = 0.;

   m_rotationDegrees         = 0.;
   m_exteriorCornerRadius    = 0.;
   m_maxExteriorCornerRadius = 1.e20;

   if (m_pageUnits == pageUnitsUndefined)
   {
      m_pageUnits = getDefaultPageUnits();
   }
}

CStandardApertureShape::CStandardApertureShape(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent,
   CenterTag centerTag,double rotationDegrees,int symmetry) :
      m_pageUnits(pageUnits),
      m_descriptorDimensionFactorExponent(descriptorDimensionFactorExponent),
      //m_descriptorDimensionFactor(getDefaultDescriptorDimensionFactor()),
      m_center(centerTag),
      m_rotationDegrees(rotationDegrees),
      m_symmetry(symmetry)
{
   m_isValid = false;

   m_dimension[0] = 0.;
   m_dimension[1] = 0.;
   m_dimension[2] = 0.;
   m_dimension[3] = 0.;
   m_dimension[4] = 0.;
   m_dimension[5] = 0.;

   m_exteriorCornerRadius    = 0.;
   m_maxExteriorCornerRadius = 1.e20;
}

CStandardApertureShape::~CStandardApertureShape()
{
}

double CStandardApertureShape::getRotationRadians() const
{
   return degreesToRadians(m_rotationDegrees);
}

void CStandardApertureShape::setRotationRadians(double radians)
{
   m_rotationDegrees = radiansToDegrees(radians);
}

StandardApertureTypeTag CStandardApertureShape::getType() const
{
   return standardApertureUndefined;
}

double CStandardApertureShape::getExteriorCornerRadius() const
{
   return m_exteriorCornerRadius;
}

void CStandardApertureShape::setExteriorCornerRadius(double exteriorCornerRadius)
{
   m_exteriorCornerRadius = exteriorCornerRadius;
}

bool CStandardApertureShape::integrateExteriorCornerRadius()
{
   return false;
}

bool CStandardApertureShape::isThermal() const
{
   return false;
}

bool CStandardApertureShape::isValid() const
{
   return m_isValid;
}

bool CStandardApertureShape::isEnglish() const
{
   return ::isEnglish(m_pageUnits);;
}

bool CStandardApertureShape::isMetric() const
{
   return ::isMetric(m_pageUnits);;
}

double CStandardApertureShape::apertureUnitsToPageUnits(double apertureUnits) const
{
   double pageUnits = (apertureUnits / getDescriptorDimensionFactor()) * getUnitsFactor(getDisplayPageUnits(m_pageUnits),m_pageUnits);

   return pageUnits;
}

double CStandardApertureShape::getDescriptorDimensionFactor() const
{
   double descriptorDimensionFactor = 1.0;

   switch (m_descriptorDimensionFactorExponent)
   {
   case -6:  descriptorDimensionFactor = .000001;   break;
   case -5:  descriptorDimensionFactor = .00001;    break;
   case -4:  descriptorDimensionFactor = .0001;     break;
   case -3:  descriptorDimensionFactor = .001;      break;
   case -2:  descriptorDimensionFactor = .01;       break;
   case -1:  descriptorDimensionFactor = .1;        break;
   case  0:  descriptorDimensionFactor = 1.;        break;
   case  1:  descriptorDimensionFactor = 10.;       break;
   case  2:  descriptorDimensionFactor = 100.;      break;
   case  3:  descriptorDimensionFactor = 1000.;     break;
   case  4:  descriptorDimensionFactor = 10000.;    break;
   case  5:  descriptorDimensionFactor = 100000.;   break;
   case  6:  descriptorDimensionFactor = 1000000.;  break;
   }

   return descriptorDimensionFactor;
}

CString CStandardApertureShape::getDescriptorDimensionUnitsString() const
{
   CString descriptorDimensionUnitsString("Unknown");
   double descriptorDimensionFactorExponent = getDescriptorDimensionFactorExponent();

   switch (m_pageUnits)
   {
   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsHpPlotter:
      if (descriptorDimensionFactorExponent == 0)
      {
         descriptorDimensionUnitsString = "Mils";
      }
      else if (descriptorDimensionFactorExponent == 1)
      {
         descriptorDimensionUnitsString = "Decimils";
      }
      else if (descriptorDimensionFactorExponent == 2)
      {
         descriptorDimensionUnitsString = "Centimils";
      }

      break;
   case pageUnitsMilliMeters:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      if (descriptorDimensionFactorExponent == 0)
      {
         descriptorDimensionUnitsString = "Millimeters";
      }
      else if (descriptorDimensionFactorExponent == 1)
      {
         descriptorDimensionUnitsString = "Decimillimeters";
      }
      else if (descriptorDimensionFactorExponent == 2)
      {
         descriptorDimensionUnitsString = "Centimillimeters";
      }

      break;
   }

   return descriptorDimensionUnitsString;
}

//double CStandardApertureShape::getDescriptorDimensionFactor() const
//{
//   double descriptorDimensionFactor = getDescriptorDimensionFactor();
//
//   if (descriptorDimensionFactor == 0.)
//   {
//      switch (m_pageUnits)
//      {
//      case pageUnitsInches:
//      case pageUnitsMils:
//      case pageUnitsHpPlotter:
//         descriptorDimensionFactor = 1.;
//         break;
//      case pageUnitsMilliMeters:
//      case pageUnitsCentiMicroMeters:
//      case pageUnitsMicroMeters:
//         descriptorDimensionFactor = 100.;
//         break;
//      }
//   }
//
//   return descriptorDimensionFactor;
//}

double CStandardApertureShape::getApertureDimension(int index) const
{
   double apertureDimension = 0.0;

   switch (m_pageUnits)
   {
   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsHpPlotter:
      apertureDimension    = m_dimension[index] * getUnitsFactor(m_pageUnits,pageUnitsMils);

      break;
   case pageUnitsMilliMeters:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      apertureDimension    = m_dimension[index] * getUnitsFactor(m_pageUnits,pageUnitsMilliMeters);

      break;
   }

   return apertureDimension;
}

int CStandardApertureShape::getIntApertureDimension(int index) const
{
   double apertureDimension = getApertureDimension(index);
   int intApertureDimension = DcaRound(apertureDimension*getDescriptorDimensionFactor());

   return intApertureDimension;
}

CString CStandardApertureShape::getDescriptorDimension(int index) const
{
   CString descriptorDimension;

   int intDimension = getIntApertureDimension(index);
   descriptorDimension.Format("%d",intDimension);

   return descriptorDimension;
}

CString CStandardApertureShape::getDescriptorIntDimension(int index) const
{
   int intDimension = DcaRound(m_dimension[index]);
   CString descriptorDimension;
   descriptorDimension.Format("%d",intDimension);

   return descriptorDimension;
}

CString CStandardApertureShape::getDescriptorRotation() const
{
   CString descriptorRotation;

   int degrees = DcaRound(normalizeDegrees(m_rotationDegrees));

   if (degrees != 0)
   {
      descriptorRotation.Format("_%03d",degrees);
   }

   return descriptorRotation;
}

CString CStandardApertureShape::getCornersString(int index) const
{
   CString cornersString;
   int corners = (int)(m_dimension[index] + .5);

   if (corners != 1234 && corners != 0)
   {
      cornersString.Format("%d",corners);
   }

   return cornersString;
}

CString CStandardApertureShape::getDescriptorCorners(int index) const
{
   CString descriptorCorners = getCornersString(index);

   if (!descriptorCorners.IsEmpty())
   {
      descriptorCorners = getDescriptorDelimiter() + descriptorCorners;
   }

   return descriptorCorners;
}

CString CStandardApertureShape::getFeaturesString(int index) const
{
   return getCornersString(index);
}

CString CStandardApertureShape::getDescriptorFeatures(int index) const
{
   return getDescriptorCorners(index);
}

CString CStandardApertureShape::getDescriptor() const
{
   return "CStandardApertureShape";
}

void CStandardApertureShape::normalizeDimension(int index)
{
   int intApertureDimension = getIntApertureDimension(index);
   double dimension = 0.0;

   switch (m_pageUnits)
   {
   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsHpPlotter:
      dimension    = intApertureDimension / getUnitsFactor(m_pageUnits,pageUnitsMils);

      break;
   case pageUnitsMilliMeters:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      dimension    = intApertureDimension / getUnitsFactor(m_pageUnits,pageUnitsMilliMeters);

      break;
   }

   dimension /= getDescriptorDimensionFactor();

   m_dimension[index] = dimension;
}

void CStandardApertureShape::normalizeDimensions()
{
   normalizeDimension(0);
   normalizeDimension(1);
   normalizeDimension(2);
   normalizeDimension(3);
   normalizeDimension(4);
   normalizeDimension(5);
}

CString CStandardApertureShape::getDescriptorCenterInfix() const
{
   CString centerInfix;

   if (m_center != centerOfExtents)
   {
      centerInfix = centerTagToDescriptorInfix(m_center);
   }

   return centerInfix;
}

CString CStandardApertureShape::getDescriptorUnitsSuffix() const
{
   CString unitsSuffix;

   if (!(isEnglish() && getDescriptorDimensionFactorExponent() == 0))
   {
      unitsSuffix.Format("%c%d",(isMetric() ? 'm' : 'e'),getDescriptorDimensionFactorExponent());
   }

   return unitsSuffix;
}

CCenterTag CStandardApertureShape::getCenterOptions() const
{
   return CCenterTag(centerOfExtents);
}

void CStandardApertureShape::changeUnits(PageUnitsTag newPageUnits)
{
   PageUnitsTag oldPageUnits = getPageUnits();

   if (oldPageUnits != newPageUnits)
   {
      double conversionFactor = getUnitsFactor(oldPageUnits,newPageUnits);
      scale(conversionFactor);
   }

   setPageUnits(newPageUnits);
}

bool CStandardApertureShape::parseParameters(CStringArray& params)
{
   bool retval = true;
   char* endPtr;
   int numParams = params.GetCount();
   double value;

   for (int ind = 0;ind < numParams;ind++)
   {
      value = strtod(params[ind],&endPtr);
      setDimensionApertureUnits(ind,value);

      if (*endPtr != '\0')
      {
         retval = false;
      }
   }

   return retval;
}

void CStandardApertureShape::scale(double scaleFactor)
{
   m_dimension[0] *= scaleFactor;
   m_dimension[1] *= scaleFactor;
   m_dimension[2] *= scaleFactor;
   m_dimension[3] *= scaleFactor;
   m_dimension[4] *= scaleFactor;
   m_dimension[5] *= scaleFactor;
}

double CStandardApertureShape::getArea() const
{
   return 0.;
}

CBasesVector CStandardApertureShape::getMajorMinorAxes() const
{
   CBasesVector majorMinorAxes(0.,0.,1.,((m_dimension[0] < m_dimension[1]) ? 90. : 0.),false);

   return majorMinorAxes;
}

CBasesVector CStandardApertureShape::getSymmetryAxes() const
{
   return getMajorMinorAxes();
}

bool CStandardApertureShape::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct)
{
   bool retval = false;

   if (m_exteriorCornerRadius > 0.)
   {
      makeValid();
      CStandardApertureShape* insetAperture = CStandardApertureShape::create(*this);

      double exteriorCornerRadiusLimit = .9 * m_maxExteriorCornerRadius;
      double exteriorCornerRadius = ((m_exteriorCornerRadius > exteriorCornerRadiusLimit) ? exteriorCornerRadiusLimit : m_exteriorCornerRadius);

      if (insetAperture->inset(exteriorCornerRadius))
      {
         int widthIndex = camCadData.getDefinedWidthIndex(exteriorCornerRadius*2.);

         retval = insetAperture->getAperturePoly(camCadData,parentDataList,polyStruct,widthIndex);
      }

      delete insetAperture;
   }

   if (!retval)
   {
      int widthIndex = camCadData.getZeroWidthIndex();

      retval = getAperturePoly(camCadData,parentDataList,polyStruct,widthIndex);
   }

   return retval;
}

bool CStandardApertureShape::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   return false;
}

CExtent CStandardApertureShape::getExtent() const
{
   CExtent extent;

   return extent;
}

bool CStandardApertureShape::inset(double inset)
{
   bool retval = false;

   return retval;
}

void CStandardApertureShape::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();
}

void CStandardApertureShape::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   for (int index = 0;index < apertureParameters.getCount();index++)
   {
      CStandardApertureDialogParameter* parameter = apertureParameters.getAt(index);

      if (! parameter->getValue().IsEmpty())
      {
         double value = atof(parameter->getValue());

         if (apertureParameters.getDisplayUnitsInPageUnitsFlag())
         {
            setDimension(index,value);
         }
         else
         {
            setDimensionApertureUnits(index,value);
         }
      }
      else
      {
         if (apertureParameters.getDisplayUnitsInPageUnitsFlag())
         {
            setDimension(index,0.);
         }
         else
         {
            setDimensionApertureUnits(index,0);
         }
      }
   }

   CenterTag center = apertureParameters.getCenter();

   if (center != centerOfUndefined)
   {
      setCenter(center);
   }

   makeValid();
}

void CStandardApertureShape::setDimensionApertureUnits(int index,double dimension)
{
   dimension = apertureUnitsToPageUnits(dimension);

   setDimension(index,dimension);
}

double CStandardApertureShape::getMinimumDimension() const
{
   PageUnitsTag displayPageUnits = getDisplayPageUnits(getPageUnits());

   // 1.0 mil or .02 millimeters
   double minimumDimension = ((displayPageUnits == pageUnitsMils) ? 1.0 : .02);

   minimumDimension = minimumDimension * getUnitsFactor(displayPageUnits,getPageUnits());

   return minimumDimension;
}

bool CStandardApertureShape::transformPolyToCenter(DataStruct& polyStruct) const
{
   bool retval = false;

   if (getCenter() != centerOfExtents)
   {
      CPoint2d newCenter;

      if (getCenter() == centerOfMass)
      {
         newCenter = polyStruct.getPolyList()->getCenterOfMass(getPageUnits());
         retval = true;
      }
      else if (getCenter() == centerOfVertices)
      {
         newCenter = polyStruct.getPolyList()->getCenterOfVertices(getPageUnits());
         retval = true;
      }

      if (retval)
      {
         CTMatrix matrix;
         matrix.translateCtm(newCenter);
         matrix.invert();
         polyStruct.transform(matrix);
      }
   }

   return retval;
}

CPoint2d CStandardApertureShape::getRelativeCenterOfRectangle() const
{
   CPoint2d retval;

   return retval;
}

void CStandardApertureShape::setFromExtent(const CExtent& extent)
{
   double targetSize = min(extent.getXsize(),extent.getYsize());
   double factor = ((getDimension(0) != 0.) ? targetSize / getDimension(0) : 1.);
   scale(factor);

   makeValid();
}

void CStandardApertureShape::makeValid()
{
}

double CStandardApertureShape::getAnnotationDimensionOffset() const
{
   return getDefaultSize()/10.;
}

void CStandardApertureShape::addDiagramAnnotations(CCamCadData& camCadData) const
{
}

void CStandardApertureShape::addDimensionArrow(CCamCadData& camCadData,CPoly* poly,CTMatrix& matrix) const
{
   double du = getDimensionArrowLength()/3.;

   CPoint2d p0(0.,0.);
   CPoint2d p1(3.*du, du);
   CPoint2d p2(3.*du,-du);

   matrix.transform(p0);
   matrix.transform(p1);
   matrix.transform(p2);

   camCadData.addVertex(*poly,p0.x,p0.y);
   camCadData.addVertex(*poly,p1.x,p1.y);
   camCadData.addVertex(*poly,p2.x,p2.y);
   camCadData.addVertex(*poly,p0.x,p0.y);
}

void CStandardApertureShape::addPointAnnotation(CCamCadData& camCadData,double x,double y) const
{
   CExtent extent = getExtent();

   FileStruct *visiblePcbFile = camCadData.getSingleVisiblePcb();

   if (visiblePcbFile != NULL)
   {
      BlockStruct* fileBlock = visiblePcbFile->getBlock();
      int layerIndex = camCadData.getDefinedLayerIndex(QPointAnnotation);
      double du = (extent.getXsize() + extent.getYsize())/40.;

      DataStruct* polyStruct = camCadData.addPolyStruct(*fileBlock,layerIndex,graphicClassNormal,false);

      CPoly* poly = camCadData.addOpenPoly(*polyStruct,camCadData.getZeroWidthIndex());
      camCadData.addVertex(*poly,x - du, y);
      camCadData.addVertex(*poly,x + du, y);

      poly = camCadData.addOpenPoly(*polyStruct,camCadData.getZeroWidthIndex());
      camCadData.addVertex(*poly,x,y + du);
      camCadData.addVertex(*poly,x,y - du);
   }
}

void CStandardApertureShape::addCentroidAnnotations(CCamCadData& camCadData)
{
   BlockStruct complexApertureBlock;

   int layerIndex = camCadData.getDefinedLayerIndex(QPointAnnotation);
   DataStruct* polyStruct = camCadData.newPolyStruct(layerIndex,graphicClassNormal,false,graphicClassNormal);
   getAperturePoly(camCadData,complexApertureBlock.getDataList(),polyStruct);
   CPolyList* polyList = polyStruct->getPolyList();
   CPoint2d centerOfMassPoint,centerOfVerticesPoint,centerOfExtentsPoint,centerOfRectanglePoint;
   CExtent extent;
   int polyCount = 0;

   for (POSITION pos = polyList->GetHeadPosition();pos != NULL;)
   {
      CPoly* poly = polyList->GetNext(pos);

      if (poly != NULL)
      {
         poly->vectorize(getPageUnits());

         CPolygon polygon(getPageUnits());
         polygon.setVertices(*poly);

         centerOfMassPoint     = centerOfMassPoint     + polygon.getCenterOfMass();
         centerOfVerticesPoint = centerOfVerticesPoint + polygon.getCenterOfVertices();
         extent.update(polygon.getExtent());

         polyCount++;
      }
   }

   if (polyCount != 0)
   {
      centerOfVerticesPoint.x /= polyCount;
      centerOfVerticesPoint.y /= polyCount;

      centerOfExtentsPoint   = extent.getCenter();
      centerOfRectanglePoint = centerOfExtentsPoint + getRelativeCenterOfRectangle();

      FileStruct *visiblePcbFile = camCadData.getSingleVisiblePcb();
      if (visiblePcbFile != NULL)
      {
         BlockStruct* fileBlock = visiblePcbFile->getBlock();
         int layerIndex = camCadData.getDefinedLayerIndex(QPointAnnotation);
         double du = (extent.getXsize() + extent.getYsize())/80.;

         DataStruct* polyStruct = camCadData.addPolyStruct(*fileBlock,layerIndex,graphicClassNormal,false);
         CPoly* poly;
         CPoint2d annotationOrigin;

         if (getCenterOptions().contains(centerOfMass))
         {
            // equilateral triangle
            annotationOrigin = centerOfMassPoint;
            double du2 = du*2.;

            poly = camCadData.addClosedPoly(*polyStruct,camCadData.getZeroWidthIndex());
            camCadData.addVertex(*poly,annotationOrigin.x - du2/SqrtOf3,annotationOrigin.y - du2/3.);
            camCadData.addVertex(*poly,annotationOrigin.x              ,annotationOrigin.y + du2*(2./3.));
            camCadData.addVertex(*poly,annotationOrigin.x + du2/SqrtOf3,annotationOrigin.y - du2/3.);
            poly->close();
         }

         if (getCenterOptions().contains(centerOfVertices))
         {
            // cross
            annotationOrigin = centerOfVerticesPoint;

            poly = camCadData.addOpenPoly(*polyStruct,camCadData.getZeroWidthIndex());
            camCadData.addVertex(*poly,annotationOrigin.x - du,annotationOrigin.y - du);
            camCadData.addVertex(*poly,annotationOrigin.x + du,annotationOrigin.y + du);

            poly = camCadData.addOpenPoly(*polyStruct,camCadData.getZeroWidthIndex());
            camCadData.addVertex(*poly,annotationOrigin.x - du,annotationOrigin.y + du);
            camCadData.addVertex(*poly,annotationOrigin.x + du,annotationOrigin.y - du);
         }

         if (getCenterOptions().contains(centerOfRectangle))
         {
            // square
            annotationOrigin = centerOfRectanglePoint;

            poly = camCadData.addClosedPoly(*polyStruct,camCadData.getZeroWidthIndex());
            camCadData.addVertex(*poly,annotationOrigin.x - du,annotationOrigin.y - du);
            camCadData.addVertex(*poly,annotationOrigin.x - du,annotationOrigin.y + du);
            camCadData.addVertex(*poly,annotationOrigin.x + du,annotationOrigin.y + du);
            camCadData.addVertex(*poly,annotationOrigin.x + du,annotationOrigin.y - du);
            poly->close();
         }

         if (getCenterOptions().contains(centerOfExtents))
         {
            // circle
            annotationOrigin = extent.getCenter();

            poly = camCadData.addOpenPoly(*polyStruct,camCadData.getZeroWidthIndex());
            camCadData.addVertex(*poly,annotationOrigin.x - du,annotationOrigin.y,1.);
            camCadData.addVertex(*poly,annotationOrigin.x + du,annotationOrigin.y,1.);
            camCadData.addVertex(*poly,annotationOrigin.x - du,annotationOrigin.y);
         }
      }
   }
}

void CStandardApertureShape::addTextAnnotation(CCamCadData& camCadData,int layerIndex,
   double x,double y,const CString& text,
   HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const
{
   double textHeight = .2*getDefaultSize();
   double charWidth  = textHeight/2.;

   FileStruct *visiblePcbFile = camCadData.getSingleVisiblePcb();

   if (visiblePcbFile != NULL)
   {
      BlockStruct* fileBlock = visiblePcbFile->getBlock();

      DataStruct* textData = camCadData.addText(*fileBlock,layerIndex,text,x,y,textHeight,charWidth);
      
      if (textData != NULL)
      {
         TextStruct* textStruct = textData->getText();
         textStruct->setHorizontalPosition(horizontalPosition);
         textStruct->setVerticalPosition(verticalPosition);
      }
   }
}

void CStandardApertureShape::addTextAnnotation(CCamCadData& camCadData,double x,double y,const CString& text,
   HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const
{
   int layerIndex = camCadData.getDefinedLayerIndex(QTextAnnotation);

   addTextAnnotation(camCadData,layerIndex,x,y,text,horizontalPosition,verticalPosition);
}

void CStandardApertureShape::addDimensionAnnotation(CCamCadData& camCadData,
   double x0,double y0,double x1,double y1,const CString& text,
   HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const
{
   FileStruct *visiblePcbFile = camCadData.getSingleVisiblePcb();

   if (visiblePcbFile != NULL)
   {
      BlockStruct* fileBlock = visiblePcbFile->getBlock();
      int layerIndex = camCadData.getDefinedLayerIndex(QDimensionAnnotation);
      int widthIndex = camCadData.getZeroWidthIndex();  //.getDefinedWidthIndex(.1*getDefaultSize());

      DataStruct* polyStruct = camCadData.addPolyStruct(*fileBlock,layerIndex,graphicClassNormal,false);

      CPoint2d p0(x0,y0);
      CPoint2d p1(x1,y1);

      double angleRadians = atan2(y1 - y0,x1 - x0);

      if (p0.distance(p1) >= 3. * getDimensionArrowLength())
      {
         CPoly* poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

         camCadData.addVertex(*poly,x0,y0);
         camCadData.addVertex(*poly,x1,y1);

         poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

         CTMatrix matrix;
         matrix.translateCtm(x0,y0);
         matrix.rotateRadiansCtm(angleRadians);

         addDimensionArrow(camCadData,poly,matrix);

         poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

         matrix.initMatrix();
         matrix.translateCtm(x1,y1);
         matrix.rotateRadiansCtm(angleRadians + Pi);

         addDimensionArrow(camCadData,poly,matrix);
      }
      else
      {
         CPoly* poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

         CVector2d tail0(p0,p1);
         tail0.setLength(getDimensionArrowLength() * 2.);
         tail0.makePerpendicular(true);
         tail0.makePerpendicular(true);

         camCadData.addVertex(*poly,tail0.getOrigin().x,tail0.getOrigin().y);
         camCadData.addVertex(*poly,tail0.getTip().x   ,tail0.getTip().y);

         poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

         CVector2d tail1(p1,p0);
         tail1.setLength(getDimensionArrowLength() * 2.);
         tail1.makePerpendicular(true);
         tail1.makePerpendicular(true);

         camCadData.addVertex(*poly,tail1.getOrigin().x,tail1.getOrigin().y);
         camCadData.addVertex(*poly,tail1.getTip().x   ,tail1.getTip().y);

         poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

         CTMatrix matrix;
         matrix.translateCtm(x0,y0);
         matrix.rotateRadiansCtm(angleRadians + Pi);

         addDimensionArrow(camCadData,poly,matrix);

         poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

         matrix.initMatrix();
         matrix.translateCtm(x1,y1);
         matrix.rotateRadiansCtm(angleRadians);

         addDimensionArrow(camCadData,poly,matrix);
      }

      double x = (x0 + x1)/2.;
      double y = (y0 + y1)/2.;

      if (horizontalPosition == horizontalPositionLeft)
      {
         x += getAnnotationDimensionOffset();
      }

      addTextAnnotation(camCadData,layerIndex,x,y,text,horizontalPosition,verticalPosition);
   }
}

void CStandardApertureShape::addDimensionAnnotation(CCamCadData& camCadData,
   const CPoint2d& center,const CPoint2d& p0,const CPoint2d& p1,const CString& text,
   HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const
{
   FileStruct *visiblePcbFile = camCadData.getSingleVisiblePcb();

   if (visiblePcbFile != NULL)
   {
      BlockStruct* fileBlock = visiblePcbFile->getBlock();
      int layerIndex = camCadData.getDefinedLayerIndex(QDimensionAnnotation);
      int widthIndex = camCadData.getZeroWidthIndex();  //.getDefinedWidthIndex(.1*getDefaultSize());

      DataStruct* polyStruct = camCadData.addPolyStruct(*fileBlock,layerIndex,graphicClassNormal,false);

      //CPoint2d p0(x0,y0);
      //CPoint2d p1(x1,y1);

      //double angleRadians = atan2(y1 - y0,x1 - x0);
      CArcPoint arc(p0,p1,center);
      CVector2d v0(center,p0);
      CVector2d v1(center,p1);
      double theta0 = v0.getTheta();
      double theta1 = v1.getTheta();

      //if (p0.distance(p1) >= 3. * getDimensionArrowLength())
      {
         CPoly* poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

         camCadData.addVertex(*poly,arc.x,arc.y,arc.bulge);
         camCadData.addVertex(*poly,p1.x,p1.y);

         poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

         CTMatrix matrix;
         matrix.translateCtm(p0);
         matrix.rotateRadiansCtm(theta0 + Pi/2.);

         addDimensionArrow(camCadData,poly,matrix);

         poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

         matrix.initMatrix();
         matrix.translateCtm(p1);
         matrix.rotateRadiansCtm(theta1 - Pi/2.);

         addDimensionArrow(camCadData,poly,matrix);
      }
      //else
      //{
      //   CPoly* poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

      //   CVector2d tail0(p0,p1);
      //   tail0.setLength(getDimensionArrowLength() * 2.);
      //   tail0.makePerpendicular(true);
      //   tail0.makePerpendicular(true);

      //   camCadData.addVertex(*poly,tail0.getOrigin().x,tail0.getOrigin().y);
      //   camCadData.addVertex(*poly,tail0.getTip().x   ,tail0.getTip().y);

      //   poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

      //   CVector2d tail1(p1,p0);
      //   tail1.setLength(getDimensionArrowLength() * 2.);
      //   tail1.makePerpendicular(true);
      //   tail1.makePerpendicular(true);

      //   camCadData.addVertex(*poly,tail1.getOrigin().x,tail1.getOrigin().y);
      //   camCadData.addVertex(*poly,tail1.getTip().x   ,tail1.getTip().y);

      //   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

      //   CTMatrix matrix;
      //   matrix.translateCtm(x0,y0);
      //   matrix.rotateRadiansCtm(angleRadians + Pi);

      //   addDimensionArrow(camCadData,poly,matrix);

      //   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

      //   matrix.initMatrix();
      //   matrix.translateCtm(x1,y1);
      //   matrix.rotateRadiansCtm(angleRadians);

      //   addDimensionArrow(camCadData,poly,matrix);
      //}

      double theta  = (theta1 + theta0)/2.;
      double radius = v0.getLength();
      double x = radius * cos(theta);
      double y = radius * sin(theta);

      if (horizontalPosition == horizontalPositionLeft)
      {
         x += getAnnotationDimensionOffset();
      }

      addTextAnnotation(camCadData,layerIndex,x,y,text,horizontalPosition,verticalPosition);
   }
}

//_____________________________________________________________________________
CStandardApertureButterfly::CStandardApertureButterfly(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureCircle(pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

CStandardApertureButterfly::CStandardApertureButterfly(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureCircle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

StandardApertureTypeTag CStandardApertureButterfly::getType() const
{
   return standardApertureButterfly;
}

double CStandardApertureButterfly::getArea() const
{
   double area = (PiOver4 * getDiameter() * getDiameter()) / 2.;

   return area;
}

CBasesVector CStandardApertureButterfly::getMajorMinorAxes() const
{
   CBasesVector majorMinorAxes(0.,0.,1.,135.,false);

   return majorMinorAxes;
}

void CStandardApertureButterfly::makeValid()
{
   if (getDiameter() < getMinimumDimension())
   {
      setDiameter(getMinimumDimension());
   }

   setMaxExternalCornerRadius(getMinimumDimension()/4.);
}

bool CStandardApertureButterfly::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   double radius       = getRadius();
   CPoly* poly;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,-radius,      0);
   camCadData.addVertex(*poly,      0,      0);
   camCadData.addVertex(*poly,      0, radius,Bulge90);
   camCadData.addVertex(*poly,-radius,      0);

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, radius,      0);
   camCadData.addVertex(*poly,      0,      0);
   camCadData.addVertex(*poly,      0,-radius,Bulge90);
   camCadData.addVertex(*poly, radius,      0);

   return true;
}

void CStandardApertureButterfly::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double rComponent = getRadius() / SqrtOf2;

   addDimensionAnnotation(camCadData,-rComponent, rComponent,
                                          rComponent,-rComponent,
                "d", horizontalPositionLeft,verticalPositionBottom);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
CStandardApertureChamferedRectangle::CStandardApertureChamferedRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setCornerRadius(.25 * getDefaultSize());
   setCorners(1234);
}

CStandardApertureChamferedRectangle::CStandardApertureChamferedRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if ((params.GetCount() == 3) || (params.GetCount() == 4))
   {
      setValid(parseParameters(params));

      setSymmetry(max((fpeq3(getWidth(),getHeight()) ? 90 : 180),cornerSymmetry(getCorners())));
   }
}

StandardApertureTypeTag CStandardApertureChamferedRectangle::getType() const
{
   return standardApertureChamferedRectangle;
}

//CCenterTag CStandardApertureChamferedRectangle::getCenterOptions()
//{
//   return CCenterTag(centerOfExtents | centerOfRectangle | centerOfMass);
//}

CString CStandardApertureChamferedRectangle::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) + getDescriptorDelimiter() +
                 getDescriptorDimension(2) +
                 getDescriptorCorners(3) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

void CStandardApertureChamferedRectangle::normalizeDimensions()
{
   normalizeDimension(0);
   normalizeDimension(1);
   normalizeDimension(2);
}

double CStandardApertureChamferedRectangle::getCornerRadius() const
{
   return getDimension(2);
}

void CStandardApertureChamferedRectangle::setCornerRadius(double radius)
{
   setDimension(2,radius);
}

int CStandardApertureChamferedRectangle::getCorners() const
{
   return getIntDimension(3);
}

void CStandardApertureChamferedRectangle::setCorners(int corners)
{
   setDimension(3,corners);
}

void CStandardApertureChamferedRectangle::scale(double scaleFactor)
{
   setDimension(0,getDimension(0) * scaleFactor);
   setDimension(1,getDimension(1) * scaleFactor);
   setDimension(2,getDimension(2) * scaleFactor);
}

double CStandardApertureChamferedRectangle::getArea() const
{
   double cornerCutoutArea = getCornerRadius() * getCornerRadius() / 2.;
   double area = (getWidth() * getHeight()) - (getCornerCount(getCorners()) * cornerCutoutArea);

   return area;
}

CBasesVector CStandardApertureChamferedRectangle::getMajorMinorAxes() const
{
   double angleDegrees = 0;
   bool corner1,corner2,corner3,corner4;
   getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);

   bool pointedLeft = (corner2 + corner3 < corner1 + corner4);
   bool pointedDown = (corner3 + corner4 < corner1 + corner2);
   bool portrait    = (getHeight() > getWidth());
   bool mirrorFlag  = (pointedLeft != pointedDown) != portrait;

   if (portrait)
   {
      angleDegrees = 90.;

      if (pointedDown)
      {
         angleDegrees += 180.;
      }

      mirrorFlag = pointedLeft;
   }
   else
   {
      angleDegrees = 0.;

      if (pointedLeft)
      {
         angleDegrees += 180.;
      }
   }

   if (mirrorFlag)
   {
      angleDegrees += 180.;
   }

   CBasesVector majorMinorAxes(0.,0.,1.,angleDegrees,mirrorFlag);

   return majorMinorAxes;
}

CBasesVector CStandardApertureChamferedRectangle::getSymmetryAxes() const
{
   double angleDegrees = 0;
   bool corner1,corner2,corner3,corner4;
   getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);

   int cornerSum = corner1 + corner2 + corner3 + corner4;

   // if there is no symmetry or if all four directions are equally symmetrical, then just return the major/minor axis
   CBasesVector symmetryAxes;

   if ((cornerSum == 2) && (corner1 != corner3) && (corner2 != corner4))
   {
      if (corner1 && corner2)
      {
         angleDegrees =  90.;
      }
      else if (corner2 && corner3)
      {
         angleDegrees = 180.;
      }
      else if (corner3 && corner4)
      {
         angleDegrees = 270.;
      }
      else if (corner4 && corner1)
      {
         angleDegrees =   0.;
      }

      symmetryAxes = CBasesVector(0.,0.,1.,angleDegrees,false);
   }
   else
   {
      symmetryAxes = getMajorMinorAxes();
   }

   return symmetryAxes;
}

bool CStandardApertureChamferedRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;
   double  radius    = getCornerRadius();
   int corners       = getCorners();
   double radius1    = (hasCorner(corners,1) ? radius : 0.);
   double radius2    = (hasCorner(corners,2) ? radius : 0.);
   double radius3    = (hasCorner(corners,3) ? radius : 0.);
   double radius4    = (hasCorner(corners,4) ? radius : 0.);

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   // 2--1
   // |  |
   // 3--4
   // corner 1
   if (radius1 != 0.)
   {
      camCadData.addVertex(*poly,halfWidth,halfHeight - radius);
   }

   camCadData.addVertex(*poly,halfWidth - radius1,halfHeight);

   // corner 2
   if (radius2 != 0)
   {
      camCadData.addVertex(*poly,-halfWidth + radius,halfHeight);
   }

   camCadData.addVertex(*poly,-halfWidth,halfHeight - radius2);

   // corner 3
   if (radius3 != 0)
   {
      camCadData.addVertex(*poly,-halfWidth ,-halfHeight + radius);
   }

   camCadData.addVertex(*poly,-halfWidth + radius3,-halfHeight);

   // corner 4
   if (radius4 != 0)
   {
      camCadData.addVertex(*poly,halfWidth - radius,-halfHeight);
   }

   camCadData.addVertex(*poly,halfWidth,-halfHeight + radius4);

   camCadData.addVertex(*poly,halfWidth,halfHeight - radius1);

   transformPolyToCenter(*polyStruct);

   return true;
}

bool CStandardApertureChamferedRectangle::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);

      double cornerRadius = getCornerRadius() - sizeReduction;

      if (cornerRadius < 0.) cornerRadius = 0.;

      setCornerRadius(cornerRadius);

      retval = true;
   }

   return retval;
}

void CStandardApertureChamferedRectangle::makeValid()
{
   //if (getCornerRadius() <= getExteriorCornerRadius())
   //{
   //   setCornerRadius(getExteriorCornerRadius());
   //   setCorners(0);
   //}

   //double maxRadius = getCornerRadius();

   bool corner1,corner2,corner3,corner4;
   int cornerCount = getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);
   //double minDimension1 = max(getMinimumDimension(),2.0*getExteriorCornerRadius());
   double minDimension1 = getMinimumDimension();
   double minDimension2 = 2. * minDimension1;

   bool adjacentCornersFlag = ((corner1 || corner3) && (corner2 || corner4));

   if (adjacentCornersFlag && getWidth() < minDimension2)
   {  // allow for adjacent corners of minimum dimension radius
      setWidth(minDimension2);
   }
   else if (getWidth() < minDimension1)
   {
      setWidth(minDimension1);
   }

   if (adjacentCornersFlag && getHeight() < minDimension2)
   {  // allow for adjacent corners of minimum dimension radius
      setHeight(minDimension2);
   }
   else if (getHeight() < minDimension1)
   {
      setHeight(minDimension1);
   }

   bool adjacentCornersInWidthFlag  = (corner1 && corner2) || (corner3 && corner4);
   bool adjacentCornersInHeightFlag = (corner1 && corner4) || (corner2 && corner3);

   if (adjacentCornersInWidthFlag)
   {
      if (getCornerRadius() > getWidth()/2.)
      {
         setCornerRadius(getWidth()/2.);
      }
   }

   if (adjacentCornersInHeightFlag)
   {
      if (getCornerRadius() > getHeight()/2.)
      {
         setCornerRadius(getHeight()/2.);
      }
   }

   double minDimension = min(getWidth(),getHeight());
   double maxDimension = max(getWidth(),getHeight());

   if (!adjacentCornersFlag)
   {
      if (getCornerRadius() > minDimension)
      {
         setCornerRadius(minDimension);
      }

      if (cornerCount == 2 && getCornerRadius() > maxDimension - minDimension1)
      {
         setCornerRadius(maxDimension - minDimension1);
      }
   }

   if (getCornerRadius() < minDimension1)
   {
      setCornerRadius(minDimension1);
   }

   setMaxExternalCornerRadius(minDimension/2.);
}

void CStandardApertureChamferedRectangle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w"      ,"Width"        ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h"      ,"Height"       ,getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"r"      ,"Corner radius",getDescriptorDelimiter() + "c",getDescriptorDimension(2),getDimension(2));
   apertureParameters.setParameters(3,"corners",
      "Corners - a combination of 1, 2, 3, 4 to specify which corners are present - omitted if all 4 corners are present",
      getDescriptorDelimiter(),getCornersString(3));
}

void CStandardApertureChamferedRectangle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,halfHeight + du,
                                          halfWidth,halfHeight + du,
                "w", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData, halfWidth + du, halfHeight,
                                          halfWidth + du, halfHeight - .25 * getDefaultSize(),
                "r", horizontalPositionLeft,verticalPositionCenter);

   addTextAnnotation(camCadData, halfWidth + du, halfHeight + du,"1",horizontalPositionLeft ,verticalPositionBottom);
   addTextAnnotation(camCadData,-halfWidth - du, halfHeight + du,"2",horizontalPositionRight,verticalPositionBottom);
   addTextAnnotation(camCadData,-halfWidth - du,-halfHeight - du,"3",horizontalPositionRight,verticalPositionTop);
   addTextAnnotation(camCadData, halfWidth + du,-halfHeight - du,"4",horizontalPositionLeft ,verticalPositionTop);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);

   // corner1 origin
   addPointAnnotation(camCadData,halfWidth - getCornerRadius(),halfHeight - getCornerRadius());
}

//void CStandardApertureChamferedRectangle::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,apertureUnitsToPageUnits(atof(apertureParameters.getAt(0)->getValue())));
//   setDimension(1,apertureUnitsToPageUnits(atof(apertureParameters.getAt(1)->getValue())));
//   setDimension(2,apertureUnitsToPageUnits(atof(apertureParameters.getAt(2)->getValue())));
//   setDimension(3,atoi(apertureParameters.getAt(3)->getValue()));
//}

void CStandardApertureChamferedRectangle::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 3)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureBeveledRectangle::CStandardApertureBeveledRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureChamferedRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setBevelWidth( .25 * getWidth());
   setBevelHeight(.25 * getHeight());
   setCorners(1234);
}

CStandardApertureBeveledRectangle::CStandardApertureBeveledRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureChamferedRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if ((params.GetCount() == 4) || (params.GetCount() == 5))
   {
      setValid(parseParameters(params));

      setSymmetry(max(((fpeq3(getWidth(),getHeight()) && fpeq3(getBevelWidth(),getBevelHeight())) ? 90 : 180),cornerSymmetry(getCorners())));
   }
}

StandardApertureTypeTag CStandardApertureBeveledRectangle::getType() const
{
   return standardApertureBeveledRectangle;
}

CString CStandardApertureBeveledRectangle::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) + getDescriptorDelimiter() +
                 getDescriptorDimension(2) + getDescriptorDelimiter() +
                 getDescriptorDimension(3) +
                 getDescriptorCorners(4)   +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

void CStandardApertureBeveledRectangle::normalizeDimensions()
{
   normalizeDimension(0);
   normalizeDimension(1);
   normalizeDimension(2);
   normalizeDimension(3);
}

double CStandardApertureBeveledRectangle::getBevelWidth() const
{
   return getDimension(2);
}

void CStandardApertureBeveledRectangle::setBevelWidth(double width)
{
   setDimension(2,width);
}

double CStandardApertureBeveledRectangle::getBevelHeight() const
{
   return getDimension(3);
}

void CStandardApertureBeveledRectangle::setBevelHeight(double height)
{
   setDimension(3,height);
}

int CStandardApertureBeveledRectangle::getCorners() const
{
   return getIntDimension(4);
}

void CStandardApertureBeveledRectangle::setCorners(int corners)
{
   setDimension(4,corners);
}

void CStandardApertureBeveledRectangle::scale(double scaleFactor)
{
   setDimension(0,getDimension(0) * scaleFactor);
   setDimension(1,getDimension(1) * scaleFactor);
   setDimension(2,getDimension(2) * scaleFactor);
   setDimension(3,getDimension(3) * scaleFactor);
}

double CStandardApertureBeveledRectangle::getArea() const
{
   double cornerCutoutArea = getBevelWidth() * getBevelHeight() / 2.;
   double area = (getWidth() * getHeight()) - (getCornerCount(getCorners()) * cornerCutoutArea);

   return area;
}

//CBasesVector CStandardApertureBeveledRectangle::getMajorMinorAxes() const
//{
//   double angleDegrees = 0;
//   bool corner1,corner2,corner3,corner4;
//   getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);
//
//   bool pointedLeft = (corner2 + corner3 < corner1 + corner4);
//   bool pointedDown = (corner3 + corner4 < corner1 + corner2);
//   bool portrait    = (getHeight() > getWidth());
//   bool mirrorFlag  = (pointedLeft != pointedDown) != portrait;
//
//   if (portrait)
//   {
//      angleDegrees = 90.;
//
//      if (pointedDown)
//      {
//         angleDegrees += 180.;
//      }
//
//      mirrorFlag = pointedLeft;
//   }
//   else
//   {
//      angleDegrees = 0.;
//
//      if (pointedLeft)
//      {
//         angleDegrees += 180.;
//      }
//   }
//
//   if (mirrorFlag)
//   {
//      angleDegrees += 180.;
//   }
//
//   CBasesVector majorMinorAxes(0.,0.,1.,angleDegrees,mirrorFlag);
//
//   return majorMinorAxes;
//}

//CBasesVector CStandardApertureBeveledRectangle::getSymmetryAxes() const
//{
//   double angleDegrees = 0;
//   bool corner1,corner2,corner3,corner4;
//   getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);
//
//   int cornerSum = corner1 + corner2 + corner3 + corner4;
//
//   // if there is no symmetry or if all four directions are equally symmetrical, then just return the major/minor axis
//   CBasesVector symmetryAxes;
//
//   if ((cornerSum == 2) && (corner1 != corner3) && (corner2 != corner4))
//   {
//      if (corner1 && corner2)
//      {
//         angleDegrees =  90.;
//      }
//      else if (corner2 && corner3)
//      {
//         angleDegrees = 180.;
//      }
//      else if (corner3 && corner4)
//      {
//         angleDegrees = 270.;
//      }
//      else if (corner4 && corner1)
//      {
//         angleDegrees =   0.;
//      }
//
//      symmetryAxes = CBasesVector(0.,0.,1.,angleDegrees,false);
//   }
//   else
//   {
//      symmetryAxes = getMajorMinorAxes();
//   }
//
//   return symmetryAxes;
//}

bool CStandardApertureBeveledRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   double bevelWidth  = getBevelWidth();
   double bevelHeight = getBevelHeight();

   bool corner1,corner2,corner3,corner4;
   int cornerCount = getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   // 2--1
   // |  |
   // 3--4
   // corner 1
   if (corner1)
   {
      camCadData.addVertex(*poly,halfWidth,halfHeight - corner1*bevelHeight);
   }

   camCadData.addVertex(*poly,halfWidth - corner1*bevelWidth,halfHeight);

   // corner 2
   if (corner2)
   {
      camCadData.addVertex(*poly,-halfWidth + corner2*bevelWidth,halfHeight);
   }

   camCadData.addVertex(*poly,-halfWidth,halfHeight - corner2*bevelHeight);

   // corner 3
   if (corner3)
   {
      camCadData.addVertex(*poly,-halfWidth ,-halfHeight + corner3*bevelHeight);
   }

   camCadData.addVertex(*poly,-halfWidth + corner3*bevelWidth,-halfHeight);

   // corner 4
   if (corner4)
   {
      camCadData.addVertex(*poly,halfWidth - corner4*bevelWidth,-halfHeight);
   }

   camCadData.addVertex(*poly,halfWidth,-halfHeight + corner4*bevelHeight);

   camCadData.addVertex(*poly,halfWidth,halfHeight - corner1*bevelHeight);

   transformPolyToCenter(*polyStruct);

   return true;
}

bool CStandardApertureBeveledRectangle::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      double width  = getWidth()  - sizeReduction;
      double height = getHeight() - sizeReduction;

      double bw = getBevelWidth();
      double bh = getBevelHeight();
      double h  = sqrt(bw * bw + bh * bh);

      double dbh  = inset * h / bw;
      double bh2  = bh + dbh;

      double dbh2 = inset * bw / h;
      double bh3  = bh2 - inset - dbh2;

      double bw3 = bh3 * bw / bh;

      setWidth(width);
      setHeight(height);

      setBevelWidth(bw3);
      setBevelHeight(bh3);

      makeValid();

      retval = true;
   }

   return retval;
}

void CStandardApertureBeveledRectangle::makeValid()
{
   bool corner1,corner2,corner3,corner4;
   int cornerCount = getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);

   double minDimension1 = getMinimumDimension();
   double minDimension2 = 2. * minDimension1;
   bool adjacentCornersFlag = ((corner1 || corner3) && (corner2 || corner4));

   if (adjacentCornersFlag && getWidth() < minDimension2)
   {  // allow for adjacent corners of minimum dimension bevel width
      setWidth(minDimension2);
   }
   else if (getWidth() < minDimension1)
   {
      setWidth(minDimension1);
   }

   if (adjacentCornersFlag && getHeight() < minDimension2)
   {  // allow for adjacent corners of minimum dimension bevel height
      setHeight(minDimension2);
   }
   else if (getHeight() < minDimension1)
   {
      setHeight(minDimension1);
   }

   bool adjacentCornersInWidthFlag  = (corner1 && corner2) || (corner3 && corner4);
   bool adjacentCornersInHeightFlag = (corner1 && corner4) || (corner2 && corner3);

   if (adjacentCornersInWidthFlag)
   {
      if (getBevelWidth() > getWidth()/2.)
      {
         setBevelWidth(getWidth()/2.);
      }
   }
   else
   {
      if (getBevelWidth() > getWidth() - minDimension1)
      {
         setBevelWidth(getWidth() - minDimension1);
      }
   }

   if (adjacentCornersInHeightFlag)
   {
      if (getBevelHeight() > getHeight()/2.)
      {
         setBevelHeight(getHeight()/2.);
      }
   }
   else
   {
      if (getBevelHeight() > getHeight() - minDimension1)
      {
         setBevelHeight(getHeight() - minDimension1);
      }
   }

   if (getBevelWidth() < minDimension1)
   {
      setBevelWidth(minDimension1);
   }

   if (getBevelHeight() < minDimension1)
   {
      setBevelHeight(minDimension1);
   }

   double minDimension = min(getWidth(),getHeight());

   setMaxExternalCornerRadius(minDimension/2.);
}

void CStandardApertureBeveledRectangle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w"      ,"Width"        ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h"      ,"Height"       ,getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"bw"     ,"Bevel Width"  ,getDescriptorDelimiter(),getDescriptorDimension(2),getDimension(2));
   apertureParameters.setParameters(3,"bh"     ,"Bevel Height" ,getDescriptorDelimiter(),getDescriptorDimension(3),getDimension(3));
   apertureParameters.setParameters(4,"corners",
      "Corners - a combination of 1, 2, 3, 4 to specify which corners are present - omitted if all 4 corners are present",
      getDescriptorDelimiter(),getCornersString(4));
}

void CStandardApertureBeveledRectangle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,halfHeight + du,
                                          halfWidth,halfHeight + du,
                "w", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData, halfWidth + du,-halfHeight,
                                          halfWidth + du,-halfHeight + getBevelHeight(),
                "bh",horizontalPositionLeft,verticalPositionCenter);

   addDimensionAnnotation(camCadData, halfWidth                  , -halfHeight - du,
                                          halfWidth - getBevelWidth(), -halfHeight - du,
                "bw",horizontalPositionCenter,verticalPositionTop);

   addTextAnnotation(camCadData, halfWidth + du, halfHeight + du,"1",horizontalPositionLeft ,verticalPositionBottom);
   addTextAnnotation(camCadData,-halfWidth - du, halfHeight + du,"2",horizontalPositionRight,verticalPositionBottom);
   addTextAnnotation(camCadData,-halfWidth - du,-halfHeight - du,"3",horizontalPositionRight,verticalPositionTop);
   addTextAnnotation(camCadData, halfWidth + du,-halfHeight - du,"4",horizontalPositionLeft ,verticalPositionTop);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);

   // corner4 origin
   addPointAnnotation(camCadData, halfWidth - getBevelWidth(),-halfHeight + getBevelHeight());
}

//void CStandardApertureBeveledRectangle::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,apertureUnitsToPageUnits(atof(apertureParameters.getAt(0)->getValue())));
//   setDimension(1,apertureUnitsToPageUnits(atof(apertureParameters.getAt(1)->getValue())));
//   setDimension(2,apertureUnitsToPageUnits(atof(apertureParameters.getAt(2)->getValue())));
//   setDimension(3,atoi(apertureParameters.getAt(3)->getValue()));
//}

void CStandardApertureBeveledRectangle::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 4)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureCircle::CStandardApertureCircle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(360,pageUnits,descriptorDimensionFactorExponent)
{
   setDiameter(getDefaultSize());
}

CStandardApertureCircle::CStandardApertureCircle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,0)
{
   if (params.GetCount() == 1)
   {
      setValid(parseParameters(params));
   }
}

StandardApertureTypeTag CStandardApertureCircle::getType() const
{
   return standardApertureCircle;
}

CString CStandardApertureCircle::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureCircle::getDiameter() const
{
   return getDimension(0);
}

void CStandardApertureCircle::setDiameter(double diameter)
{
   setDimension(0,diameter);
}

double CStandardApertureCircle::getRadius() const
{
   return getDimension(0) / 2.;
}

double CStandardApertureCircle::getArea() const
{
   double area = PiOver4 * getDiameter() * getDiameter();

   return area;
}

bool CStandardApertureCircle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double radius = getRadius();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, radius,0.,1.);
   camCadData.addVertex(*poly,-radius,0.,1.);
   camCadData.addVertex(*poly, radius,0.);

   return true;
}

CExtent CStandardApertureCircle::getExtent() const
{
   CExtent extent;

   double radius = getRadius();
   extent.update(-radius,-radius);
   extent.update( radius, radius);

   return extent;
}

bool CStandardApertureCircle::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getDiameter() > sizeReduction)
   {
      setDiameter(getDiameter() - sizeReduction);

      retval = true;
   }

   return retval;
}

void CStandardApertureCircle::makeValid()
{
   if (getDiameter() < getMinimumDimension())
   {
      setDiameter(getMinimumDimension());
   }

   setMaxExternalCornerRadius(getMinimumDimension()/2.);
}

void CStandardApertureCircle::setExteriorCornerRadius(double exteriorCornerRadius)
{
   CStandardApertureShape::setExteriorCornerRadius(0.);
}

bool CStandardApertureCircle::integrateExteriorCornerRadius()
{
   return true;
}

void CStandardApertureCircle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"d","Diameter",getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
}

void CStandardApertureCircle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double rComponent = getRadius() / SqrtOf2;

   addDimensionAnnotation(camCadData,-rComponent, rComponent,
                                          rComponent,-rComponent,
                "d", horizontalPositionLeft,verticalPositionBottom);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
/* http://mathworld.wolfram.com/CircularSegment.html

                    s
          h --     ....
            --   .--c---.
             d  . \    / .
               .   \a /R  .
            -- .    \/    .
               .          .
                .        .
                 .      .
                   ....

Circular Segment
A portion of a disk whose upper boundary is a (circular) arc and whose lower boundary is a chord
making a central angle  radians (), illustrated above as the shaded region. 
The entire wedge-shaped area is known as a circular sector. 

Let:
R - the radius of the circle.
c - chord length.
s - arc length.
h - the height of the arced portion.
d - the height of the triangular portion.
a - central angle.

Then:

 R = h + d                     (1) 

 s = R * a                     (2) 

 d = R * cos(.5 * a)           (3) 
   = .5 * c * cot(.5 * a)      (4) 
   = .5 * sqrt(4*R*R - c*c)    (5) 

 c = 2*R * sin(.5 * a)         (6) 
   = 2*d * tan(.5 * a)         (7) 
   = 2 * sqrt(R*R - d*d)       (8) 
   = 2 * sqrt(h * (2*R - h))   (9) 

From elementary trigonometry, the angle  obeys the relationships 

 a = s / R                    (10) 
   = 2. * arccos(d / R)       (11) 
   = 2. * arctan(c / (2*d))   (12) 
   = 2. * arcsin(c / (2*R))   (13) 

The area  of the (shaded) segment is then simply given by the area of the circular sector
(the entire wedge-shaped portion) minus the area of the bottom triangular portion, 

 A = A(sector) - A(isosceles triangle)                           (14)
   = .5 * R*R * (a - sin(a))                                     (15)
   = .5 *(R*s - c*d)                                             (16)
   = R*R * arccos(d / R) - d * sqrt(R*R - d*d)                   (17)
   = R*R * arccos((R - h) / R) - (R - h) * sqrt(2*R*h - h*h)     (18)

*/

CStandardApertureSplitCircle::CStandardApertureSplitCircle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureCircle(pageUnits,descriptorDimensionFactorExponent)
{
   setSplitWidth(getDefaultSize()/8.);
}

CStandardApertureSplitCircle::CStandardApertureSplitCircle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureCircle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if (params.GetCount() == 2)
   {
      setValid(parseParameters(params));
   }
}

StandardApertureTypeTag CStandardApertureSplitCircle::getType() const
{
   return standardApertureSplitCircle;
}

CString CStandardApertureSplitCircle::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureSplitCircle::getSplitWidth() const
{
   return getDimension(1);
}

void CStandardApertureSplitCircle::setSplitWidth(double splitWidth)
{
   setDimension(1,splitWidth);
}

double CStandardApertureSplitCircle::getArea() const
{
   double circleArea = PiOver4 * getDiameter() * getDiameter();
   double r = getRadius();
   double c = getSplitWidth();
   double d = .5 * sqrt(4.*r*r - c*c);
   double h = r - d;
   double sectorArea = r*r * acos((r - h)/r);
   double triangleArea = (r - h) * sqrt(2.*r*h - h*h);
   //double segmentArea = r*r * acos((r - h)/r) - (r - h) * sqrt(2.*r*h - h*h);
   //double sectorArea = r*r * acos((r - h)/r) - (r - h) * sqrt(2.*r*h - h*h);
   //double rectArea = 2.*d*c;
   double area = circleArea - 2.*(sectorArea + triangleArea);

   return area;
}

bool CStandardApertureSplitCircle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double radius = getRadius();
   double halfWidth = getSplitWidth()/2.;
   double cosTheta  = halfWidth / radius;
   double sinTheta  = 1. - cosTheta * cosTheta;
   double theta  = acos(cosTheta);
   double deltaAngle = 2. * theta;
   double bulge  = tan(deltaAngle / 4.);
   double dx = radius*sinTheta;
   double dy = radius*cosTheta;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, dx, dy,bulge);
   camCadData.addVertex(*poly,-dx, dy);
   camCadData.addVertex(*poly, dx, dy);

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,-dx,-dy,bulge);
   camCadData.addVertex(*poly, dx,-dy);
   camCadData.addVertex(*poly,-dx,-dy);

   return true;
}

CExtent CStandardApertureSplitCircle::getExtent() const
{
   CExtent extent;

   double r = getRadius();
   double c = getSplitWidth();
   double d = .5 * sqrt(4.*r*r - c*c);

   extent.update(-d,-r);
   extent.update( d, r);

   return extent;
}

bool CStandardApertureSplitCircle::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getDiameter() - getSplitWidth() > sizeReduction)
   {
      setDiameter(getDiameter() - sizeReduction);
      setSplitWidth(getSplitWidth() + sizeReduction);

      retval = true;
   }

   return retval;
}

void CStandardApertureSplitCircle::makeValid()
{
   if (getDiameter() < 3.*getMinimumDimension())
   {
      setDiameter(2.*getMinimumDimension());
   }

   if (getSplitWidth() < getMinimumDimension())
   {
      setSplitWidth(getMinimumDimension());
   }

   if (getDiameter() - getSplitWidth() < 2.*getMinimumDimension())
   {
      setSplitWidth(getDiameter() - 2.*getMinimumDimension());
   }

   setMaxExternalCornerRadius((getRadius() - getSplitWidth()/2.)/2.);
}

//void CStandardApertureSplitCircle::setExteriorCornerRadius(double exteriorCornerRadius)
//{
//   CStandardApertureShape::setExteriorCornerRadius(0.);
//}

//bool CStandardApertureSplitCircle::integrateExteriorCornerRadius()
//{
//   return true;
//}

void CStandardApertureSplitCircle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"d","Diameter"    ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"sw","Split Width",getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
}

void CStandardApertureSplitCircle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double rComponent = getRadius() / SqrtOf2;

   addDimensionAnnotation(camCadData,-rComponent, rComponent,
                                          rComponent,-rComponent,
                "d", horizontalPositionLeft,verticalPositionBottom);

   double du             = getAnnotationDimensionOffset();
   double radius         = getRadius();
   double halfSplitWidth = getSplitWidth() / 2.;

   addDimensionAnnotation(camCadData,radius + du, -halfSplitWidth,
                                         radius + du,  halfSplitWidth,
                "sw", horizontalPositionLeft,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
CStandardApertureDiamond::CStandardApertureDiamond(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
}

CStandardApertureDiamond::CStandardApertureDiamond(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureDiamond::getType() const
{
   return standardApertureDiamond;
}

double CStandardApertureDiamond::getArea() const
{
   double area = (getWidth() * getHeight()) / 2.;

   return area;
}

bool CStandardApertureDiamond::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,         0, halfHeight);
   camCadData.addVertex(*poly,-halfWidth,          0);
   camCadData.addVertex(*poly,         0,-halfHeight);
   camCadData.addVertex(*poly, halfWidth,          0);
   camCadData.addVertex(*poly,         0, halfHeight);

   return true;
}

void CStandardApertureDiamond::makeValid()
{
   if (getWidth() < getMinimumDimension())
   {
      setWidth(getMinimumDimension());
   }

   if (getHeight() < getMinimumDimension())
   {
      setHeight(getMinimumDimension());
   }

   setMaxExternalCornerRadius(min(getWidth(),getHeight())/(2.*SqrtOf2));
}

//_____________________________________________________________________________
CStandardApertureRectangledPentagon::CStandardApertureRectangledPentagon(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setWidth( 2. * getDefaultSize());
   setHeight(     getDefaultSize());
   setAltitude(   getDefaultSize()/2.);

   setSymmetry(360);
}

CStandardApertureRectangledPentagon::CStandardApertureRectangledPentagon(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if (params.GetCount() == 3)
   {
      setValid(parseParameters(params));
      setSymmetry(180);
   }
}

StandardApertureTypeTag CStandardApertureRectangledPentagon::getType() const
{
   return standardApertureRectangledPentagon;
}

CString CStandardApertureRectangledPentagon::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) + getDescriptorDelimiter() +
                 getDescriptorDimension(2) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureRectangledPentagon::getAltitude() const
{
   return getDimension(2);
}

void   CStandardApertureRectangledPentagon::setAltitude(double altitude)
{
   setDimension(2,altitude);
}

double CStandardApertureRectangledPentagon::getArea() const
{
   double area = (getWidth() * getHeight()) - (getAltitude() * getWidth()/2.);

   return area;
}

CBasesVector CStandardApertureRectangledPentagon::getMajorMinorAxes() const
{
   CBasesVector majorMinorAxes(0.,0.,1.,90.,false);

   return majorMinorAxes;
}

bool CStandardApertureRectangledPentagon::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,-halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth, halfHeight - getAltitude());
   camCadData.addVertex(*poly,        0., halfHeight);
   camCadData.addVertex(*poly,-halfWidth, halfHeight - getAltitude());
   camCadData.addVertex(*poly,-halfWidth,-halfHeight);

   return true;
}

void CStandardApertureRectangledPentagon::makeValid()
{
   if (getWidth() < getMinimumDimension())
   {
      setWidth(getMinimumDimension());
   }

   if (getHeight() < 2. * getMinimumDimension())
   {
      setHeight(2. * getMinimumDimension());
   }

   if (getAltitude() < getMinimumDimension())
   {
      setAltitude(getMinimumDimension());
   }

   if (getAltitude() > getHeight() - getMinimumDimension())
   {
      setAltitude(getHeight() - getMinimumDimension());
   }

   setMaxExternalCornerRadius(min(getWidth(),getHeight())/2.);
}

void CStandardApertureRectangledPentagon::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w","Width"   ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h","Height"  ,getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"a","Altitude",getDescriptorDelimiter(),getDescriptorDimension(2),getDimension(2));
}

void CStandardApertureRectangledPentagon::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,halfHeight + du,
                                          halfWidth,halfHeight + du,
                "w", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData, halfWidth + du,halfHeight,
                                          halfWidth + du,halfHeight - getAltitude(),
                "a", horizontalPositionLeft,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);

   // corner origin
   addPointAnnotation(camCadData,halfWidth,halfHeight);
}

//_____________________________________________________________________________
CStandardApertureEllipse::CStandardApertureEllipse(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

CStandardApertureEllipse::CStandardApertureEllipse(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

StandardApertureTypeTag CStandardApertureEllipse::getType() const
{
   return standardApertureEllipse;
}

double CStandardApertureEllipse::getArea() const
{
   double area = PiOver4 * (getWidth() * getHeight());

   return area;
}

bool CStandardApertureEllipse::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   const int numPoints = 36;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   for (int angleInd = 0;angleInd <= numPoints;angleInd++)
   {
      double angle = angleInd * 2. * Pi / numPoints;
      double x = halfWidth  * cos(angle);
      double y = halfHeight * sin(angle);

      camCadData.addVertex(*poly,x,y);
   }

   return true;
}

void CStandardApertureEllipse::setExteriorCornerRadius(double exteriorCornerRadius)
{
   CStandardApertureShape::setExteriorCornerRadius(0.);
}

bool CStandardApertureEllipse::integrateExteriorCornerRadius()
{
   return true;
}

//_____________________________________________________________________________
CStandardApertureHalfOval::CStandardApertureHalfOval(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(360);
}

CStandardApertureHalfOval::CStandardApertureHalfOval(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(360);
   setValid(getWidth() >= getHeight());
}

StandardApertureTypeTag CStandardApertureHalfOval::getType() const
{
   return standardApertureHalfOval;
}

double CStandardApertureHalfOval::getArea() const
{
   double area = ((getWidth() - getHeight()/2.) * getHeight()) +
                 (PiOver4 * getHeight() * getHeight() / 2.);

   return area;
}

bool CStandardApertureHalfOval::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, halfWidth - halfHeight,-halfHeight, 1.);
   camCadData.addVertex(*poly, halfWidth - halfHeight, halfHeight);
   camCadData.addVertex(*poly,-halfWidth             , halfHeight);
   camCadData.addVertex(*poly,-halfWidth             ,-halfHeight);
   camCadData.addVertex(*poly, halfWidth - halfHeight,-halfHeight);

   transformPolyToCenter(*polyStruct);

   return true;
}

void CStandardApertureHalfOval::makeValid()
{
   if (getHeight() < 2.*getMinimumDimension())
   {
      setHeight(2.*getMinimumDimension());
   }

   if (getWidth() < getHeight()/2.)
   {
      setWidth(getHeight()/2.);
   }

   setMaxExternalCornerRadius(min(getWidth(),getHeight())/2.);
}

//_____________________________________________________________________________
CStandardApertureHole::CStandardApertureHole(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureCircle(pageUnits,descriptorDimensionFactorExponent)
{
}

CStandardApertureHole::CStandardApertureHole(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureCircle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureHole::getType() const
{
   return standardApertureHole;
}

CString CStandardApertureHole::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

//_____________________________________________________________________________
CStandardApertureHorizontalHexagon::CStandardApertureHorizontalHexagon(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setWidth(getDefaultSize());

   double sideLength = getWidth()/2.;
   setHeight(SqrtOf3 * sideLength);
   setCornerSize(sideLength/2.);

   setSymmetry(180);
}

CStandardApertureHorizontalHexagon::CStandardApertureHorizontalHexagon(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if (params.GetCount() == 3)
   {
      setValid(parseParameters(params));
      setSymmetry(180);
   }
}

StandardApertureTypeTag CStandardApertureHorizontalHexagon::getType() const
{
   return standardApertureHorizontalHexagon;
}

CString CStandardApertureHorizontalHexagon::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) + getDescriptorDelimiter() +
                 getDescriptorDimension(2) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureHorizontalHexagon::getCornerSize() const
{
   return getDimension(2);
}

void CStandardApertureHorizontalHexagon::setCornerSize(double size)
{
   setDimension(2,size);
}

double CStandardApertureHorizontalHexagon::getArea() const
{
   double cornerCutoutArea = getCornerSize() * getHeight() / 2.;
   double area = (getWidth() * getHeight()) - (4. * cornerCutoutArea);

   return area;
}

bool CStandardApertureHorizontalHexagon::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   double cornerSize  = getCornerSize();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, halfWidth             ,          0);
   camCadData.addVertex(*poly, halfWidth - cornerSize, halfHeight);
   camCadData.addVertex(*poly,-halfWidth + cornerSize, halfHeight);
   camCadData.addVertex(*poly,-halfWidth             ,          0);
   camCadData.addVertex(*poly,-halfWidth + cornerSize,-halfHeight);
   camCadData.addVertex(*poly, halfWidth - cornerSize,-halfHeight);
   camCadData.addVertex(*poly, halfWidth             ,          0);

   transformPolyToCenter(*polyStruct);

   return true;
}

bool CStandardApertureHorizontalHexagon::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);

      double cornerSize = getCornerSize() + (TwoOverSqrtOf3 * inset);

      if (cornerSize > getWidth() ) cornerSize = getWidth();
      if (cornerSize > getHeight()) cornerSize = getHeight();

      setCornerSize(cornerSize);

      retval = true;
   }

   return retval;
}

void CStandardApertureHorizontalHexagon::makeValid()
{
   double minDimension1 = getMinimumDimension();
   double minDimension2 = 2. * minDimension1;
   double minDimension3 = 3. * minDimension1;

   if (getWidth() < minDimension3)
   {
      setWidth(minDimension3);
   }

   if (getCornerSize() > (getWidth() - minDimension1)/2.)
   {
      setCornerSize((getWidth() - minDimension1)/2.);
   }

   if (getHeight() < minDimension1)
   {
      setHeight(minDimension1);
   }

   setMaxExternalCornerRadius(min(getWidth() - 2.*getCornerSize(),getHeight())/2.);
}

void CStandardApertureHorizontalHexagon::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w","Width"      ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h","Height"     ,getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"r","Corner size",getDescriptorDelimiter(),getDescriptorDimension(2),getDimension(2));
}

void CStandardApertureHorizontalHexagon::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,halfHeight + du,
                                          halfWidth,halfHeight + du,
                "w", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData, halfWidth - getCornerSize(),-halfHeight - du,
                                          halfWidth                  ,-halfHeight - du,
                "r", horizontalPositionCenter,verticalPositionTop);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);

   // corner origin
   addPointAnnotation(camCadData,halfWidth                  ,-halfHeight);
   addPointAnnotation(camCadData,halfWidth - getCornerSize(),-halfHeight);
}

//_____________________________________________________________________________
CStandardApertureMoire::CStandardApertureMoire(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(90,pageUnits,descriptorDimensionFactorExponent)
{
   int numRings = 4;
   double size = getDefaultSize();
   double halfSize = size/2.;

   setRingWidth(.3 * halfSize/numRings);
   setRingGap(  .7 * halfSize/numRings);
   setRingCount(numRings);
   setLineWidth(getRingWidth());
   setLineAngle(0.);
   setLineLength(size + size/numRings);
}

CStandardApertureMoire::CStandardApertureMoire(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,90)
{
   if (params.GetCount() == 6)
   {
      setValid(parseParameters(params));
   }
}

StandardApertureTypeTag CStandardApertureMoire::getType() const
{
   return standardApertureMoire;
}

CString CStandardApertureMoire::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0)    + getDescriptorDelimiter() +
                 getDescriptorDimension(1)    + getDescriptorDelimiter() +
                 getDescriptorIntDimension(2) + getDescriptorDelimiter() +
                 getDescriptorDimension(3)    + getDescriptorDelimiter() +
                 getDescriptorDimension(4)    + getDescriptorDelimiter() +
                 getDescriptorIntDimension(5) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

void CStandardApertureMoire::normalizeDimensions()
{
   normalizeDimension(0);
   normalizeDimension(1);
   normalizeDimension(3);
   normalizeDimension(4);
}

double CStandardApertureMoire::getRingWidth() const
{
   return getDimension(0);
}

void CStandardApertureMoire::setRingWidth(double width)
{
   setDimension(0,width);
}

double CStandardApertureMoire::getRingGap() const
{
   return getDimension(1);
}

void CStandardApertureMoire::setRingGap(double gap)
{
   setDimension(1,gap);
}

int CStandardApertureMoire::getRingCount() const
{
   return getIntDimension(2);
}

void CStandardApertureMoire::setRingCount(int count)
{
   setDimension(2,count);
}

double CStandardApertureMoire::getLineLength() const
{
   return getDimension(3);
}

void CStandardApertureMoire::setLineLength(double length)
{
   setDimension(3,length);
}

double CStandardApertureMoire::getLineWidth() const
{
   return getDimension(4);
}

void CStandardApertureMoire::setLineWidth(double width)
{
   setDimension(4,width);
}

double CStandardApertureMoire::getLineAngle() const
{
   return getDimension(5);
}

void CStandardApertureMoire::setLineAngle(double angle)
{
   setDimension(5,angle);
}

double CStandardApertureMoire::getDiameter() const
{
   double diameter = getRingCount() * (getRingGap() + getRingWidth());

   return diameter;
}

double CStandardApertureMoire::getArea() const
{
   // use outer ring for area
   double ringRadius = getDiameter()/2.;
   double area = Pi * ringRadius * ringRadius;

   return area;
}

bool CStandardApertureMoire::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double ringWidth      = getRingWidth();
   double ringGap        = getRingGap();
   int numRings          = getRingCount();
   double lineWidth      = getLineWidth();
   double halfLineLength = getLineLength()/2.;
   double lineAngle      = getLineAngle();

   int ringWidthIndex = camCadData.getDefinedWidthIndex(ringWidth);
   double ringRadius  = ringWidth + ringGap;

   for (int ringInd = 0;ringInd < numRings;ringInd++)
   {
      poly = camCadData.addClosedPoly(*polyStruct,ringWidthIndex);
      double radius = (ringInd + 1) * ringRadius;

      camCadData.addVertex(*poly, radius, 0, 1.);
      camCadData.addVertex(*poly,-radius, 0, 1.);
      camCadData.addVertex(*poly, radius, 0);
   }

   int lineWidthIndex = camCadData.getDefinedWidthIndex(lineWidth);

   for (int lineInd = 0;lineInd < 2;lineInd++)
   {
      double angle = degreesToRadians(lineAngle + lineInd * 90);
      double x = halfLineLength * cos(angle);
      double y = halfLineLength * sin(angle);

      poly = camCadData.addOpenPoly(*polyStruct,lineWidthIndex);

      camCadData.addVertex(*poly,-x,-y);
      camCadData.addVertex(*poly, x, y);
   }

   return true;
}

CExtent CStandardApertureMoire::getExtent() const
{
   CExtent extent;

   double ringWidth      = getRingWidth();
   double ringGap        = getRingGap();
   int numRings          = getRingCount();

   double ringRadius  = ringWidth + ringGap;
   double radius      = ringRadius * numRings;

   extent.update(-radius,-radius);
   extent.update( radius, radius);

   return extent;
}

void CStandardApertureMoire::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"rw","Ring line width",
      getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"rg","Ring gap - from center of ring to edge of adjacent ring",
      getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"c" ,"Ring count" ,getDescriptorDelimiter(),getDescriptorIntDimension(2));
   apertureParameters.setParameters(3,"ll","Line length",getDescriptorDelimiter(),getDescriptorDimension(3),getDimension(3));
   apertureParameters.setParameters(4,"lw","Line width" ,getDescriptorDelimiter(),getDescriptorDimension(4),getDimension(4));
   apertureParameters.setParameters(5,"la","Line angle" ,getDescriptorDelimiter(),getDescriptorDimension(5));
}

void CStandardApertureMoire::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getDiameter()/2.;
   double halfHeight = getDiameter()/2.;

   addDimensionAnnotation(camCadData,-halfWidth - du, getLineLength()/2.,
                                         -halfWidth - du,-getLineLength()/2.,
                "ll", horizontalPositionRight,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//void CStandardApertureMoire::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,apertureUnitsToPageUnits(atof(apertureParameters.getAt(0)->getValue())));
//   setDimension(1,apertureUnitsToPageUnits(atof(apertureParameters.getAt(1)->getValue())));
//   setDimension(2,atoi(apertureParameters.getAt(2)->getValue()));
//   setDimension(3,apertureUnitsToPageUnits(atof(apertureParameters.getAt(3)->getValue())));
//   setDimension(4,apertureUnitsToPageUnits(atof(apertureParameters.getAt(4)->getValue())));
//   setDimension(5,atof(apertureParameters.getAt(5)->getValue()));
//}

void CStandardApertureMoire::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 2 && index != 5)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureNull::CStandardApertureNull(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent)
{
   setDimension(0,getDefaultSize());
}

CStandardApertureNull::CStandardApertureNull(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,360)
{
   if (params.GetCount() == 1)
   {
      setValid(parseParameters(params));
      setDimension(0,1.);
   }
}

StandardApertureTypeTag CStandardApertureNull::getType() const
{
   return standardApertureNull;
}

CString CStandardApertureNull::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

int CStandardApertureNull::getExtensionNumber() const
{
   return getIntDimension(0);
}

bool CStandardApertureNull::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   return false;
}

void CStandardApertureNull::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"e","Extension number",getDescriptorDelimiter(),getDescriptorIntDimension(0));
}

//void CStandardApertureNull::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,atoi(apertureParameters.getAt(0)->getValue()));
//}

void CStandardApertureNull::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 0)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureOctagon::CStandardApertureOctagon(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureHorizontalHexagon(pageUnits,descriptorDimensionFactorExponent)
{
   setWidth(getDefaultSize());
   setHeight(getDefaultSize());
   double sideLength = getDefaultSize()/(1. + 2./SqrtOf2);
   setCornerSize(sideLength/SqrtOf2);

   setSymmetry(90);
}

CStandardApertureOctagon::CStandardApertureOctagon(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureHorizontalHexagon(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(90);
}

StandardApertureTypeTag CStandardApertureOctagon::getType() const
{
   return standardApertureOctagon;
}

double CStandardApertureOctagon::getArea() const
{
   double cornerCutoutArea = (getCornerSize() * getCornerSize()) / 2.;
   double area = (getWidth() * getHeight()) - (4. * cornerCutoutArea);

   return area;
}

bool CStandardApertureOctagon::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   double cornerSize  = getCornerSize();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, halfWidth             , halfHeight - cornerSize);
   camCadData.addVertex(*poly, halfWidth - cornerSize, halfHeight             );
   camCadData.addVertex(*poly,-halfWidth + cornerSize, halfHeight);
   camCadData.addVertex(*poly,-halfWidth             , halfHeight - cornerSize);
   camCadData.addVertex(*poly,-halfWidth             ,-halfHeight + cornerSize);
   camCadData.addVertex(*poly,-halfWidth + cornerSize,-halfHeight             );
   camCadData.addVertex(*poly, halfWidth - cornerSize,-halfHeight             );
   camCadData.addVertex(*poly, halfWidth             ,-halfHeight + cornerSize);
   camCadData.addVertex(*poly, halfWidth             , halfHeight - cornerSize);

   return true;
}

bool CStandardApertureOctagon::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);

      double cornerSize = getCornerSize() + (SqrtOf2Over2 * inset);

      if (cornerSize > getWidth() ) cornerSize = getWidth();
      if (cornerSize > getHeight()) cornerSize = getHeight();

      setCornerSize(cornerSize);

      retval = true;
   }

   return retval;
}

void CStandardApertureOctagon::makeValid()
{
   if (getWidth() < getMinimumDimension())
   {
      setWidth(getMinimumDimension());
   }

   if (getHeight() < getMinimumDimension())
   {
      setHeight(getMinimumDimension());
   }

   double cornerDiagonal = getCornerSize() / SqrtOf2;
   double cornerCompensation = 2.*(getCornerSize() - cornerDiagonal);

   setMaxExternalCornerRadius(min(getWidth() - cornerCompensation,getHeight() - cornerCompensation)/2.);
}

//_____________________________________________________________________________
CStandardApertureOval::CStandardApertureOval(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

CStandardApertureOval::CStandardApertureOval(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

StandardApertureTypeTag CStandardApertureOval::getType() const
{
   return standardApertureOval;
}

double CStandardApertureOval::getArea() const
{
   double length   = getLength();
   double diameter = getDiameter();

   double area = ((length - diameter) * diameter) +
                 (PiOver4 * diameter * diameter);

   return area;
}

bool CStandardApertureOval::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   if (halfWidth >= halfHeight)
   {
      camCadData.addVertex(*poly, halfWidth - halfHeight,-halfHeight, 1.);
      camCadData.addVertex(*poly, halfWidth - halfHeight, halfHeight);
      camCadData.addVertex(*poly,-halfWidth + halfHeight, halfHeight, 1.);
      camCadData.addVertex(*poly,-halfWidth + halfHeight,-halfHeight);
      camCadData.addVertex(*poly, halfWidth - halfHeight,-halfHeight);
   }
   else
   {
      camCadData.addVertex(*poly, halfWidth, halfHeight - halfWidth, 1.);
      camCadData.addVertex(*poly,-halfWidth, halfHeight - halfWidth);
      camCadData.addVertex(*poly,-halfWidth,-halfHeight + halfWidth, 1.);
      camCadData.addVertex(*poly, halfWidth,-halfHeight + halfWidth);
      camCadData.addVertex(*poly, halfWidth, halfHeight - halfWidth);
   }

   return true;
}

void CStandardApertureOval::setExteriorCornerRadius(double exteriorCornerRadius)
{
   CStandardApertureShape::setExteriorCornerRadius(0.);
}

bool CStandardApertureOval::integrateExteriorCornerRadius()
{
   return true;
}

//_____________________________________________________________________________
CStandardApertureRectangle::CStandardApertureRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(180,pageUnits,descriptorDimensionFactorExponent)
{
   setWidth( 2. * getDefaultSize());
   setHeight(     getDefaultSize());
}

CStandardApertureRectangle::CStandardApertureRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,0)
{
   if (params.GetCount() == 2)
   {
      setValid(parseParameters(params));
      setSymmetry(fpeq3(getWidth(),getHeight()) ? 90 : 180);
   }
}

StandardApertureTypeTag CStandardApertureRectangle::getType() const
{
   return standardApertureRectangle;
}

CString CStandardApertureRectangle::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureRectangle::getWidth()  const
{
   return getDimension(0);
}

void CStandardApertureRectangle::setWidth(double width)
{
   setDimension(0,width);
}

double CStandardApertureRectangle::getHeight() const
{
   return getDimension(1);
}

void CStandardApertureRectangle::setHeight(double height)
{
   setDimension(1,height);
}

void CStandardApertureRectangle::setFromExtent(const CExtent& extent)
{
   setWidth(extent.getXsize());
   setHeight(extent.getYsize());
   makeValid();
}

double CStandardApertureRectangle::getArea() const
{
   double area = getWidth() * getHeight();

   return area;
}

bool CStandardApertureRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,-halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth, halfHeight);
   camCadData.addVertex(*poly,-halfWidth, halfHeight);
   camCadData.addVertex(*poly,-halfWidth,-halfHeight);

   return true;
}

CExtent CStandardApertureRectangle::getExtent() const
{
   CExtent extent;

   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   extent.update(-halfWidth,-halfHeight);
   extent.update( halfWidth, halfHeight);

   return extent;
}

bool CStandardApertureRectangle::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);

      retval = true;
   }

   return retval;
}

void CStandardApertureRectangle::makeValid()
{
   if (getWidth() < getMinimumDimension())
   {
      setWidth(getMinimumDimension());
   }

   if (getHeight() < getMinimumDimension())
   {
      setHeight(getMinimumDimension());
   }

   setMaxExternalCornerRadius(min(getWidth(),getHeight())/2.);
}

void CStandardApertureRectangle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w","Width" ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h","Height",getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
}

void CStandardApertureRectangle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,halfHeight + du,
                                          halfWidth,halfHeight + du,
                "w", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
CStandardApertureRectangularThermal::CStandardApertureRectangularThermal(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setSpokeCount(4);
   setSpokeGap(getDefaultSize()/4.);
   setAirGap(  getDefaultSize()/4.);
   setSymmetry(180);
}

CStandardApertureRectangularThermal::CStandardApertureRectangularThermal(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if (params.GetCount() == 6)
   {
      setValid(parseParameters(params));

      if ((getSpokeCount() % 4) == 0 && fpeq3(getWidth(),getHeight()))
      {
         setSymmetry(90);
      }
      else if ((getSpokeCount() % 2) == 0)
      {
         setSymmetry(180);
      }
      else
      {
         setSymmetry(360);
      }
   }
}

StandardApertureTypeTag CStandardApertureRectangularThermal::getType() const
{
   return standardApertureRectangularThermal;
}

bool CStandardApertureRectangularThermal::isThermal() const
{
   return true;
}

CString CStandardApertureRectangularThermal::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0)    + getDescriptorDelimiter() +
                 getDescriptorDimension(1)    + getDescriptorDelimiter() +
                 getDescriptorIntDimension(2) + getDescriptorDelimiter() +
                 getDescriptorIntDimension(3) + getDescriptorDelimiter() +
                 getDescriptorDimension(4)    + getDescriptorDelimiter() +
                 getDescriptorDimension(5) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

void CStandardApertureRectangularThermal::normalizeDimensions()
{
   normalizeDimension(0);
   normalizeDimension(1);
   normalizeDimension(4);
   normalizeDimension(5);
}

double CStandardApertureRectangularThermal::getSpokeAngleRadians() const
{
   return degreesToRadians(getDimension(2));
}

void CStandardApertureRectangularThermal::setSpokeAngleRadians(double radians)
{
   setDimension(2,radiansToDegrees(radians));
}

double CStandardApertureRectangularThermal::getSpokeAngleDegrees() const
{
   return getDimension(2);
}

void CStandardApertureRectangularThermal::setSpokeAngleDegrees(double degrees)
{
   setDimension(2,degrees);
}

int CStandardApertureRectangularThermal::getSpokeCount() const
{
   return getIntDimension(3);
}

void CStandardApertureRectangularThermal::setSpokeCount(int count)
{
   setDimension(3,count);
}

double CStandardApertureRectangularThermal::getSpokeGap() const
{
   return getDimension(4);
}

void CStandardApertureRectangularThermal::setSpokeGap(double gap)
{
   setDimension(4,gap);
}

double CStandardApertureRectangularThermal::getAirGap() const
{
   return getDimension(5);
}

void CStandardApertureRectangularThermal::setAirGap(double gap)
{
   setDimension(5,gap);
}

void CStandardApertureRectangularThermal::scale(double scaleFactor)
{
   setDimension(0,getDimension(0) * scaleFactor);
   setDimension(1,getDimension(1) * scaleFactor);
   setDimension(4,getDimension(4) * scaleFactor);
   setDimension(5,getDimension(5) * scaleFactor);
}

bool CStandardApertureRectangularThermal::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   double startAngleDegrees  = getSpokeAngleDegrees();
   int numSpokes      = getSpokeCount();
   double spokeWidth  = getSpokeGap();
   double airGap      = getAirGap();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, halfWidth, halfHeight);
   camCadData.addVertex(*poly,-halfWidth, halfHeight);
   camCadData.addVertex(*poly,-halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth, halfHeight);

   polyStruct = camCadData.addPolyStruct(parentDataList,floatLayerIndex,graphicClassNormal,DbFlag0,NegativePoly);
   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   halfWidth  -= airGap;
   halfHeight -= airGap;

   camCadData.addVertex(*poly, halfWidth, halfHeight);
   camCadData.addVertex(*poly,-halfWidth, halfHeight);
   camCadData.addVertex(*poly,-halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth,-halfHeight);
   camCadData.addVertex(*poly, halfWidth, halfHeight);

   buildSpokes(camCadData,polyStruct,max(halfWidth,halfHeight)*SqrtOf2,startAngleDegrees,numSpokes,spokeWidth);

   return true;
}

bool CStandardApertureRectangularThermal::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);
      setSpokeGap(getSpokeGap() + sizeReduction);

      retval = true;
   }

   return retval;
}

void CStandardApertureRectangularThermal::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w" ,"Width"      ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h" ,"Height"     ,getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"a" ,"Angle"      ,getDescriptorDelimiter(),getDescriptorIntDimension(2));
   apertureParameters.setParameters(3,"c" ,"Spoke count",getDescriptorDelimiter(),getDescriptorIntDimension(3));
   apertureParameters.setParameters(4,"g" ,"Gap"        ,getDescriptorDelimiter(),getDescriptorDimension(4),getDimension(4));
   apertureParameters.setParameters(5,"ag","Air gap"    ,getDescriptorDelimiter(),getDescriptorDimension(5),getDimension(5));
}

void CStandardApertureRectangularThermal::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,halfHeight + du,
                                          halfWidth,halfHeight + du,
                "w", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//void CStandardApertureRectangularThermal::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,apertureUnitsToPageUnits(atof(apertureParameters.getAt(0)->getValue())));
//   setDimension(1,apertureUnitsToPageUnits(atof(apertureParameters.getAt(1)->getValue())));
//   setDimension(2,atof(apertureParameters.getAt(2)->getValue()));
//   setDimension(3,atoi(apertureParameters.getAt(3)->getValue()));;
//   setDimension(4,apertureUnitsToPageUnits(atof(apertureParameters.getAt(4)->getValue())));
//   setDimension(5,apertureUnitsToPageUnits(atof(apertureParameters.getAt(5)->getValue())));
//}

void CStandardApertureRectangularThermal::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 2 && index != 3)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureRectangularThermalOpenCorners::CStandardApertureRectangularThermalOpenCorners(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangularThermal(pageUnits,descriptorDimensionFactorExponent)
{
}

CStandardApertureRectangularThermalOpenCorners::CStandardApertureRectangularThermalOpenCorners(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangularThermal(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureRectangularThermalOpenCorners::getType() const
{
   return standardApertureRectangularThermalOpenCorners;
}

bool CStandardApertureRectangularThermalOpenCorners::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   double startAngleDegrees = getSpokeAngleDegrees();
   int numSpokes      = getSpokeCount();
   double spokeWidth  = getSpokeGap();
   double airGap      = getAirGap();

   buildThermalOpenCorners(camCadData,polyStruct,halfWidth,halfHeight,airGap,
      startAngleDegrees,numSpokes,spokeWidth);

   return true;
}

bool CStandardApertureRectangularThermalOpenCorners::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);
      setSpokeGap(getSpokeGap() + sizeReduction);

      retval = true;
   }

   return retval;
}

//_____________________________________________________________________________
CStandardApertureRoundDonut::CStandardApertureRoundDonut(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(0,pageUnits,descriptorDimensionFactorExponent)
{
   setOuterDiameter(getDefaultSize());
   setInnerDiameter(getDefaultSize()/2.);
}

CStandardApertureRoundDonut::CStandardApertureRoundDonut(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,0)
{
   if (params.GetCount() == 2)
   {
      setValid(parseParameters(params));
   }
}

StandardApertureTypeTag CStandardApertureRoundDonut::getType() const
{
   return standardApertureRoundDonut;
}

CString CStandardApertureRoundDonut::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureRoundDonut::getOuterDiameter() const
{
   return getDimension(0);
}

void CStandardApertureRoundDonut::setOuterDiameter(double diameter)
{
   setDimension(0,diameter);
}

double CStandardApertureRoundDonut::getInnerDiameter() const
{
   return getDimension(1);
}

void CStandardApertureRoundDonut::setInnerDiameter(double diameter)
{
   setDimension(1,diameter);
}

double CStandardApertureRoundDonut::getArea() const
{
   double area = (PiOver4 * getOuterDiameter() * getOuterDiameter()) -
                 (PiOver4 * getInnerDiameter() * getInnerDiameter());

   return area;
}

bool CStandardApertureRoundDonut::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   double outerRadius  = getOuterDiameter()/2.;
   double innerRadius  = getInnerDiameter()/2.;
   double ringWidth    = outerRadius - innerRadius + camCadData.getWidth(widthIndex);
   double radius       = (outerRadius + innerRadius)/2.;
   int ringWidthIndex  = camCadData.getDefinedWidthIndex(ringWidth);
   int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);

   CPoly* poly = camCadData.addClosedPoly(*polyStruct,ringWidthIndex);

   camCadData.addVertex(*poly, radius,0.,1.);
   camCadData.addVertex(*poly,-radius,0.,1.);
   camCadData.addVertex(*poly, radius,0.);

   //int zeroWidthIndex  = camCadData.getZeroWidthIndex();
   //int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);
   //CPoly* poly;

   //double outerRadius = getOuterDiameter()/2.;
   //double innerRadius = getInnerDiameter()/2.;

   //poly = camCadData.addFilledPoly(*polyStruct,zeroWidthIndex);

   //camCadData.addVertex(*poly, outerRadius,0.,1.);
   //camCadData.addVertex(*poly,-outerRadius,0.,1.);
   //camCadData.addVertex(*poly, outerRadius,0.);

   //polyStruct = camCadData.addPolyStruct(*parentDataList,
   //   floatLayerIndex,DbFlag0,NegativePoly,graphicClassNormal);
   //poly = camCadData.addFilledPoly(*polyStruct,zeroWidthIndex);

   //camCadData.addVertex(*poly, innerRadius,0.,1.);
   //camCadData.addVertex(*poly,-innerRadius,0.,1.);
   //camCadData.addVertex(*poly, innerRadius,0.);

   return true;
}

CExtent CStandardApertureRoundDonut::getExtent() const
{
   CExtent extent;

   double radius = getOuterDiameter()/2.;
   extent.update(-radius,-radius);
   extent.update( radius, radius);

   return extent;
}

bool CStandardApertureRoundDonut::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getOuterDiameter() - getInnerDiameter() > sizeReduction)
   {
      setOuterDiameter(getOuterDiameter() - inset);
      setInnerDiameter(getInnerDiameter() + inset);

      retval = true;
   }

   return retval;
}

void CStandardApertureRoundDonut::makeValid()
{
   if (getInnerDiameter() < getMinimumDimension())
   {
      setInnerDiameter(getMinimumDimension());
   }

   if (getOuterDiameter() - getInnerDiameter() < getMinimumDimension())
   {
      setOuterDiameter(getInnerDiameter() + getMinimumDimension());
   }

   setMaxExternalCornerRadius((getOuterDiameter() - getInnerDiameter())/2.);
}

void CStandardApertureRoundDonut::setExteriorCornerRadius(double exteriorCornerRadius)
{
   CStandardApertureShape::setExteriorCornerRadius(0.);
}

bool CStandardApertureRoundDonut::integrateExteriorCornerRadius()
{
   return true;
}

void CStandardApertureRoundDonut::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"od" ,"Outer diameter",getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"id" ,"Inner diameter",getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
}

void CStandardApertureRoundDonut::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getOuterDiameter()/2.;
   double halfHeight = getOuterDiameter()/2.;

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "od", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData,0., getInnerDiameter()/2.,
                                         0.,-getInnerDiameter()/2.,
                "id", horizontalPositionRight,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
CStandardApertureRoundedRectangle::CStandardApertureRoundedRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureChamferedRectangle(pageUnits,descriptorDimensionFactorExponent)
{
}

CStandardApertureRoundedRectangle::CStandardApertureRoundedRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureChamferedRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureRoundedRectangle::getType() const
{
   return standardApertureRoundedRectangle;
}

double CStandardApertureRoundedRectangle::getArea() const
{
   double cornerCutoutArea = (1. - PiOver4) * getCornerRadius() * getCornerRadius();
   double area = (getWidth() * getHeight()) - (getCornerCount(getCorners()) * cornerCutoutArea);

   return area;
}

bool CStandardApertureRoundedRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   double  radius     = getCornerRadius();
   int corners        = getCorners();
   double radius1     = (hasCorner(corners,1) ? radius : 0.);
   double radius2     = (hasCorner(corners,2) ? radius : 0.);
   double radius3     = (hasCorner(corners,3) ? radius : 0.);
   double radius4     = (hasCorner(corners,4) ? radius : 0.);

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   // 2--1
   // |  |
   // 3--4
   // corner 1
   if (radius1 != 0.)
   {
      camCadData.addVertex(*poly,halfWidth,halfHeight - radius,Bulge90);
   }

   camCadData.addVertex(*poly,halfWidth - radius1,halfHeight);

   // corner 2
   if (radius2 != 0)
   {
      camCadData.addVertex(*poly,-halfWidth + radius,halfHeight,Bulge90);
   }

   camCadData.addVertex(*poly,-halfWidth,halfHeight - radius2);

   // corner 3
   if (radius3 != 0)
   {
      camCadData.addVertex(*poly,-halfWidth ,-halfHeight + radius,Bulge90);
   }

   camCadData.addVertex(*poly,-halfWidth + radius3,-halfHeight);

   // corner 4
   if (radius4 != 0)
   {
      camCadData.addVertex(*poly,halfWidth - radius,-halfHeight,Bulge90);
   }

   camCadData.addVertex(*poly,halfWidth,-halfHeight + radius4);

   camCadData.addVertex(*poly,halfWidth,halfHeight - radius1);

   transformPolyToCenter(*polyStruct);

   return true;
}

void CStandardApertureRoundedRectangle::makeValid()
{
   double maxRadius = getCornerRadius();

   bool corner1,corner2,corner3,corner4;
   getCornerFlags(getCorners(),corner1,corner2,corner3,corner4);
   double minDimension1 = getMinimumDimension();
   double minDimension2 = 2. * minDimension1;
   bool adjacentCornersFlag = ((corner1 || corner3) && (corner2 || corner4));

   if (adjacentCornersFlag && getWidth() < minDimension2)
   {
      setWidth(minDimension2);
   }
   else if (getWidth() < minDimension1)
   {
      setWidth(minDimension1);
   }

   if (adjacentCornersFlag && getHeight() < minDimension2)
   {
      setHeight(minDimension2);
   }
   else if (getHeight() < minDimension1)
   {
      setHeight(minDimension1);
   }

   double minDimension = min(getWidth(),getHeight());

   if (adjacentCornersFlag)
   {
      if (getCornerRadius() > minDimension/2.)
      {
         setCornerRadius(minDimension/2.);
      }
   }
   else
   {
      if (getCornerRadius() > minDimension)
      {
         setCornerRadius(minDimension);
      }
   }

   if (getCornerRadius() < minDimension1)
   {
      setCornerRadius(minDimension1);
   }

   setMaxExternalCornerRadius(minDimension/2.);
}

bool CStandardApertureRoundedRectangle::integrateExteriorCornerRadius()
{
   bool retval = true;

   if (getExteriorCornerRadius() > 0.)
   {
      if (getCorners() == 1234)
      {
         CStandardApertureShape::setExteriorCornerRadius(0.);
      }
      else
      {
         retval = false;
      }
   }

   return retval;
}

void CStandardApertureRoundedRectangle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w"      ,"Width"        ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h"      ,"Height"       ,getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"r"      ,"Corner radius",getDescriptorDelimiter() + "r",getDescriptorDimension(2),getDimension(2));
   apertureParameters.setParameters(3,"corners",
      "Corners - a combination of 1, 2, 3, 4 to specify which corners are present - omitted if all 4 corners are present",
      getDescriptorDelimiter(),getCornersString(3));
}

//void CStandardApertureRoundedRectangle::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,apertureUnitsToPageUnits(atof(apertureParameters.getAt(0)->getValue())));
//   setDimension(1,apertureUnitsToPageUnits(atof(apertureParameters.getAt(1)->getValue())));
//   setDimension(2,apertureUnitsToPageUnits(atof(apertureParameters.getAt(2)->getValue())));
//   setDimension(3,atoi(apertureParameters.getAt(3)->getValue()));;
//}

void CStandardApertureRoundedRectangle::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 3)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureRoundThermalRounded::CStandardApertureRoundThermalRounded(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(90,pageUnits,descriptorDimensionFactorExponent)
{
   setOuterDiameter(      getDefaultSize());
   setInnerDiameter(.75 * getDefaultSize());
   setSpokeCount(4);
   setSpokeGap(getDefaultSize()/4.);
   setSpokeAngleDegrees(0.);
}

CStandardApertureRoundThermalRounded::CStandardApertureRoundThermalRounded(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,0)
{
   if (params.GetCount() == 5)
   {
      setValid(parseParameters(params));

      if (getSpokeCount() > 0 && ((360 / getSpokeCount()) * getSpokeCount() == 360))
      {
         setSymmetry(360 / getSpokeCount());
      }
      else
      {
         setSymmetry(360);
      }
   }
}

StandardApertureTypeTag CStandardApertureRoundThermalRounded::getType() const
{
   return standardApertureRoundThermalRounded;
}

bool CStandardApertureRoundThermalRounded::isThermal() const
{
   return true;
}

CString CStandardApertureRoundThermalRounded::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0)    + getDescriptorDelimiter() +
                 getDescriptorDimension(1)    + getDescriptorDelimiter() +
                 getDescriptorIntDimension(2) + getDescriptorDelimiter() +
                 getDescriptorIntDimension(3) + getDescriptorDelimiter() +
                 getDescriptorDimension(4) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

void CStandardApertureRoundThermalRounded::normalizeDimensions()
{
   normalizeDimension(0);
   normalizeDimension(1);
   normalizeDimension(4);
}

double CStandardApertureRoundThermalRounded::getOuterDiameter() const
{
   return getDimension(0);
}

void CStandardApertureRoundThermalRounded::setOuterDiameter(double diameter)
{
   setDimension(0,diameter);
}

double CStandardApertureRoundThermalRounded::getInnerDiameter() const
{
   return getDimension(1);
}

void CStandardApertureRoundThermalRounded::setInnerDiameter(double diameter)
{
   setDimension(1,diameter);
}

double CStandardApertureRoundThermalRounded::getSpokeAngleRadians() const
{
   return degreesToRadians(getDimension(2));
}

void CStandardApertureRoundThermalRounded::setSpokeAngleRadians(double radians)
{
   setDimension(2,radiansToDegrees(radians));
}

double CStandardApertureRoundThermalRounded::getSpokeAngleDegrees() const
{
   return getDimension(2);
}

void CStandardApertureRoundThermalRounded::setSpokeAngleDegrees(double degrees)
{
   setDimension(2,degrees);
}

int CStandardApertureRoundThermalRounded::getSpokeCount() const
{
   return getIntDimension(3);
}

void CStandardApertureRoundThermalRounded::setSpokeCount(int count)
{
   setDimension(3,count);
}

double CStandardApertureRoundThermalRounded::getSpokeGap() const
{
   return getDimension(4);
}

void CStandardApertureRoundThermalRounded::setSpokeGap(double gap)
{
   setDimension(4,gap);
}

void CStandardApertureRoundThermalRounded::scale(double scaleFactor)
{
   setDimension(0,getDimension(0) * scaleFactor);
   setDimension(1,getDimension(1) * scaleFactor);
   setDimension(4,getDimension(4) * scaleFactor);
}

double CStandardApertureRoundThermalRounded::getArea() const
{
   // use outer perimeter for area
   double area = PiOver4 * getOuterDiameter() * getOuterDiameter();

   return area;
}

bool CStandardApertureRoundThermalRounded::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndexParameter) const
{
   int zeroWidthIndex  = camCadData.getZeroWidthIndex();
   CPoly* poly;

   double outerRadius = getOuterDiameter()/2.;
   double innerRadius = getInnerDiameter()/2.;
   double startAngleRadians  = getSpokeAngleRadians();
   int numSpokes      = getSpokeCount();
   double spokeWidth  = getSpokeGap();

   if (numSpokes < 2) numSpokes = 2;

   double arcWidth = outerRadius - innerRadius;
   int widthIndex = camCadData.getDefinedWidthIndex(arcWidth);
   double radius   = (outerRadius + innerRadius)/2.;
   double circumference = 2. * Pi * radius;
   double sectorArcLen = circumference / numSpokes;
   // sectorArcLen = arcWidth + arcLen + gapArcLen
   double arcLen = sectorArcLen - arcWidth - spokeWidth;
   double arcAngleRadians = arcLen / radius;
   double angleOffsetRadians = ((spokeWidth / 2.) + (arcWidth / 2.)) / radius;
   double bulge = tan(arcAngleRadians / 4.);
   double sectorAngleRadians = 2. * Pi / numSpokes;

   for (int sectorInd = 0;sectorInd < numSpokes;sectorInd++)
   {
      double a0 = startAngleRadians + (sectorInd * sectorAngleRadians) + angleOffsetRadians;
      double a1 = a0 + arcAngleRadians;
      double x0 = radius * cos(a0);
      double y0 = radius * sin(a0);
      double x1 = radius * cos(a1);
      double y1 = radius * sin(a1);

      poly = camCadData.addOpenPoly(*polyStruct,widthIndex);

      camCadData.addVertex(*poly,x0,y0,bulge);
      camCadData.addVertex(*poly,x1,y1      );
   }

   return true;
}

CExtent CStandardApertureRoundThermalRounded::getExtent() const
{
   CExtent extent;

   double radius = getOuterDiameter()/2.;
   extent.update(-radius,-radius);
   extent.update( radius, radius);

   return extent;
}

bool CStandardApertureRoundThermalRounded::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;
   double maxReduction = (getOuterDiameter() - getInnerDiameter())/2.;

   if (sizeReduction < maxReduction)
   {
      setOuterDiameter(getOuterDiameter() - sizeReduction);
      setInnerDiameter(getInnerDiameter() + sizeReduction);
      setSpokeGap(     getSpokeGap()      + sizeReduction);

      retval = true;
   }

   return retval;
}

void CStandardApertureRoundThermalRounded::makeValid()
{
   if (getSpokeCount() < 2)
   {
      setSpokeCount(2);
   }
   else if (getSpokeCount() > 32)
   {
      setSpokeCount(32);
   }

   if (getSpokeGap() < getMinimumDimension())
   {
      setSpokeGap(getMinimumDimension());
   }

   if (getInnerDiameter() < getMinimumDimension())
   {
      setInnerDiameter(getMinimumDimension());
   }

   if (getOuterDiameter() - getInnerDiameter() < getMinimumDimension())
   {
      setOuterDiameter(getInnerDiameter() + getMinimumDimension());
   }

   double diameter         = (getInnerDiameter() + getOuterDiameter())/2.;
   double ringWidth        = (getOuterDiameter() - getInnerDiameter())/2.;
   double minCircumference = getSpokeCount() * (getSpokeGap() + ringWidth);
   double minDiameter      = minCircumference / Pi;

   if (diameter < minDiameter)
   {
      double minRingWidth = getMinimumDimension();
      minCircumference = getSpokeCount() * (getSpokeGap() + minRingWidth);
      minDiameter      = minCircumference / Pi;

      double newRingWidth;
      double newDiameter = diameter;

      if (diameter < minDiameter)
      {
         // The ring is too small to accomodate the number of spoke gaps and minimum spoke bridges.
         // To fix this, minimize the ring width, and expand the ring diameter.
         newRingWidth = getMinimumDimension();
         newDiameter  = minDiameter;
      }
      else
      {
         // The ring is too small to accomodate the number of spoke gaps and spoke bridges at ringWidth.
         // To fix this, reduce the ring width.
         double circumference = diameter * Pi;
         double totalBridgeLength = circumference - (getSpokeCount() * getSpokeGap());
         double bridgeLength = totalBridgeLength / getSpokeCount();

         newRingWidth = bridgeLength;
      }

      setInnerDiameter(newDiameter - newRingWidth);
      setOuterDiameter(newDiameter + newRingWidth);

      ringWidth = newRingWidth;
   }

   setMaxExternalCornerRadius(ringWidth);
}

void CStandardApertureRoundThermalRounded::setExteriorCornerRadius(double exteriorCornerRadius)
{
   CStandardApertureShape::setExteriorCornerRadius(0.);
}

bool CStandardApertureRoundThermalRounded::integrateExteriorCornerRadius()
{
   return true;
}

void CStandardApertureRoundThermalRounded::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"od","Outer diameter",getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"id","Inner diameter",getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"a" ,"Angle"         ,getDescriptorDelimiter(),getDescriptorIntDimension(2));
   apertureParameters.setParameters(3,"c" ,"Spoke count"   ,getDescriptorDelimiter(),getDescriptorIntDimension(3));
   apertureParameters.setParameters(4,"g" ,"Gap"           ,getDescriptorDelimiter(),getDescriptorDimension(4),getDimension(4));
}

void CStandardApertureRoundThermalRounded::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getOuterDiameter()/2.;
   double halfHeight = getOuterDiameter()/2.;

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "od", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData,0., getInnerDiameter()/2.,
                                         0.,-getInnerDiameter()/2.,
                "id", horizontalPositionRight,verticalPositionCenter);

   double medialDiameter  = getMedialDiameter();
   double circumference   = medialDiameter * Pi;
   double gapAngleRadians = (2. * Pi * getSpokeGap()) / circumference;
   double medialRadius = medialDiameter/2.;
   double cosAngle = cos(gapAngleRadians/2.);
   double sinAngle = sin(gapAngleRadians/2.);

   CPoint2d p0(medialRadius*cosAngle,-medialRadius*sinAngle);
   CPoint2d p1(medialRadius*cosAngle, medialRadius*sinAngle);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   addPointAnnotation(camCadData,p0.x,p0.y);
   addPointAnnotation(camCadData,p1.x,p1.y);

   double annotationRadius = medialRadius + du;

   CPoint2d origin;
   CPoint2d ap0(annotationRadius*cosAngle,-annotationRadius*sinAngle);
   CPoint2d ap1(annotationRadius*cosAngle, annotationRadius*sinAngle);

   addDimensionAnnotation(camCadData,origin,ap0,ap1,
                "g", horizontalPositionLeft,verticalPositionCenter);
}

//void CStandardApertureRoundThermalRounded::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,apertureUnitsToPageUnits(atof(apertureParameters.getAt(0)->getValue())));
//   setDimension(1,apertureUnitsToPageUnits(atof(apertureParameters.getAt(1)->getValue())));
//   setDimension(2,atof(apertureParameters.getAt(2)->getValue()));
//   setDimension(3,atoi(apertureParameters.getAt(3)->getValue()));;
//   setDimension(4,apertureUnitsToPageUnits(atof(apertureParameters.getAt(4)->getValue())));
//}

void CStandardApertureRoundThermalRounded::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 2 && index != 3)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureRoundThermalSquare::CStandardApertureRoundThermalSquare(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundThermalRounded(pageUnits,descriptorDimensionFactorExponent)
{
}

CStandardApertureRoundThermalSquare::CStandardApertureRoundThermalSquare(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundThermalRounded(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureRoundThermalSquare::getType() const
{
   return standardApertureRoundThermalSquare;
}

bool CStandardApertureRoundThermalSquare::getAperturePoly(CCamCadData& camCadData,CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   int zeroWidthIndex  = camCadData.getZeroWidthIndex();
   int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);

   double outerRadius = getOuterDiameter()/2.;
   double innerRadius = getInnerDiameter()/2.;
   double startAngleRadians = getSpokeAngleRadians();
   int numSpokes      = getSpokeCount();
   double spokeWidth  = getSpokeGap();
   double halfSpokeWidth = spokeWidth/2.;

   double segmentTheta = TwoPi/numSpokes;
   double innerHalfGapTheta = acos(halfSpokeWidth / innerRadius);
   double outerHalfGapTheta = acos(halfSpokeWidth / outerRadius);
   double innerSegmentTheta = segmentTheta - 2.* innerHalfGapTheta;
   double outerSegmentTheta = segmentTheta - 2.* outerHalfGapTheta;
   double innerBulge   = -tan(innerSegmentTheta/4.);
   double outerBulge   =  tan(outerSegmentTheta/4.);

   Point2 p0,p1,p2,p3;

   p1.bulge = outerBulge;
   p3.bulge = innerBulge;

   for (int segmentIndex = 0;segmentIndex < numSpokes;segmentIndex++)
   {
      double innerTheta = startAngleRadians + innerHalfGapTheta + segmentIndex*segmentTheta;
      double outerTheta = startAngleRadians + outerHalfGapTheta + segmentIndex*segmentTheta;

      p0.x = innerRadius * cos(innerTheta);
      p0.y = innerRadius * sin(innerTheta);

      p1.x = outerRadius * cos(outerTheta);
      p1.y = outerRadius * sin(outerTheta);

      p2.x = outerRadius * cos(outerTheta + outerSegmentTheta);
      p2.y = outerRadius * sin(outerTheta + outerSegmentTheta);

      p3.x = innerRadius * cos(innerTheta + innerSegmentTheta);
      p3.y = innerRadius * sin(innerTheta + innerSegmentTheta);

      CPoly* poly = camCadData.addFilledPoly(*polyStruct,zeroWidthIndex);

      poly->addVertex(p0);
      poly->addVertex(p1);
      poly->addVertex(p2);
      poly->addVertex(p3);
      poly->addVertex(p0);
   }

   return true;
}

//_____________________________________________________________________________
CStandardApertureSquare::CStandardApertureSquare(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(90,pageUnits,descriptorDimensionFactorExponent)
{
   setSize(getDefaultSize());
}

CStandardApertureSquare::CStandardApertureSquare(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,90)
{
   if (params.GetCount() == 1)
   {
      setValid(parseParameters(params));
   }
}

StandardApertureTypeTag CStandardApertureSquare::getType() const
{
   return standardApertureSquare;
}

CString CStandardApertureSquare::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureSquare::getSize() const
{
   return getDimension(0);
}

void CStandardApertureSquare::setSize(double size)
{
   setDimension(0,size);
}

double CStandardApertureSquare::getArea() const
{
   double area = getSize() * getSize();

   return area;
}

bool CStandardApertureSquare::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfSize = getSize()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,-halfSize,-halfSize);
   camCadData.addVertex(*poly, halfSize,-halfSize);
   camCadData.addVertex(*poly, halfSize, halfSize);
   camCadData.addVertex(*poly,-halfSize, halfSize);
   camCadData.addVertex(*poly,-halfSize,-halfSize);

   return true;
}

CExtent CStandardApertureSquare::getExtent() const
{
   CExtent extent;

   double halfSize = getSize()/2.;
   extent.update(-halfSize,-halfSize);
   extent.update( halfSize, halfSize);

   return extent;
}

bool CStandardApertureSquare::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getSize() > sizeReduction)
   {
      setSize(getSize() - sizeReduction);

      retval = true;
   }

   return retval;
}

void CStandardApertureSquare::makeValid()
{
   if (getSize() < getMinimumDimension())
   {
      setSize(getMinimumDimension());
   }

   setMaxExternalCornerRadius(getSize()/2.);
}

void CStandardApertureSquare::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"s","Diameter",getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
}

void CStandardApertureSquare::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getSize()/2.;
   double halfHeight = getSize()/2.;

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "s", horizontalPositionRight,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
CStandardApertureSquareButterfly::CStandardApertureSquareButterfly(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureSquare(pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

CStandardApertureSquareButterfly::CStandardApertureSquareButterfly(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureSquare(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(180);
}

StandardApertureTypeTag CStandardApertureSquareButterfly::getType() const
{
   return standardApertureSquareButterfly;
}

double CStandardApertureSquareButterfly::getArea() const
{
   double area = (getSize() * getSize()) / 2.;

   return area;
}

CBasesVector CStandardApertureSquareButterfly::getMajorMinorAxes() const
{
   CBasesVector majorMinorAxes(0.,0.,1.,135.,false);

   return majorMinorAxes;
}

void CStandardApertureSquareButterfly::makeValid()
{
   if (getSize() < getMinimumDimension())
   {
      setSize(getMinimumDimension());
   }

   setMaxExternalCornerRadius(getSize()/4.);
}

bool CStandardApertureSquareButterfly::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfSize = getSize()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,        0,        0);
   camCadData.addVertex(*poly,        0, halfSize);
   camCadData.addVertex(*poly,-halfSize, halfSize);
   camCadData.addVertex(*poly,-halfSize,        0);
   camCadData.addVertex(*poly,        0,        0);

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,        0,        0);
   camCadData.addVertex(*poly,        0,-halfSize);
   camCadData.addVertex(*poly, halfSize,-halfSize);
   camCadData.addVertex(*poly, halfSize,        0);
   camCadData.addVertex(*poly,        0,        0);

   return true;
}

//_____________________________________________________________________________
CStandardApertureSquareDonut::CStandardApertureSquareDonut(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundDonut(pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(90);
}

CStandardApertureSquareDonut::CStandardApertureSquareDonut(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundDonut(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(90);
   setValid(getOuterDiameter() > getInnerDiameter());
}

StandardApertureTypeTag CStandardApertureSquareDonut::getType() const
{
   return standardApertureSquareDonut;
}

double CStandardApertureSquareDonut::getArea() const
{
   double area = getOuterDiameter() * getOuterDiameter() -
                 getInnerDiameter() * getInnerDiameter();

   return area;
}

bool CStandardApertureSquareDonut::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);
   CPoly* poly;

   double outerRadius = getOuterDiameter()/2.;
   double innerRadius = getInnerDiameter()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, outerRadius, outerRadius);
   camCadData.addVertex(*poly,-outerRadius, outerRadius);
   camCadData.addVertex(*poly,-outerRadius,-outerRadius);
   camCadData.addVertex(*poly, outerRadius,-outerRadius);
   camCadData.addVertex(*poly, outerRadius, outerRadius);

   polyStruct = camCadData.addPolyStruct(parentDataList,floatLayerIndex,graphicClassNormal,DbFlag0,NegativePoly);
   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly, innerRadius, innerRadius);
   camCadData.addVertex(*poly,-innerRadius, innerRadius);
   camCadData.addVertex(*poly,-innerRadius,-innerRadius);
   camCadData.addVertex(*poly, innerRadius,-innerRadius);
   camCadData.addVertex(*poly, innerRadius, innerRadius);

   return true;
}

//_____________________________________________________________________________
CStandardApertureSquareRoundThermal::CStandardApertureSquareRoundThermal(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundThermalRounded(pageUnits,descriptorDimensionFactorExponent)
{
   setSymmetry(90);
}

CStandardApertureSquareRoundThermal::CStandardApertureSquareRoundThermal(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundThermalRounded(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if ((getSpokeCount() % 4) == 0)
   {
      setSymmetry(90);
   }
   else if ((getSpokeCount() % 2) == 0)
   {
      setSymmetry(180);
   }
   else
   {
      setSymmetry(360);
   }
}

StandardApertureTypeTag CStandardApertureSquareRoundThermal::getType() const
{
   return standardApertureSquareRoundThermal;
}

double CStandardApertureSquareRoundThermal::getArea() const
{
   // use outer perimeter for area
   double area = getOuterDiameter() * getOuterDiameter();

   return area;
}

bool CStandardApertureSquareRoundThermal::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   int zeroWidthIndex  = camCadData.getZeroWidthIndex();
   int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);
   CPoly* poly;

   double outerRadius = getOuterDiameter()/2.;
   double innerRadius = getInnerDiameter()/2.;
   double startAngleDegrees  = getSpokeAngleDegrees();
   int numSpokes      = getIntDimension(3);
   double spokeWidth  = getSpokeGap();

   poly = camCadData.addFilledPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly, outerRadius, outerRadius);
   camCadData.addVertex(*poly,-outerRadius, outerRadius);
   camCadData.addVertex(*poly,-outerRadius,-outerRadius);
   camCadData.addVertex(*poly, outerRadius,-outerRadius);
   camCadData.addVertex(*poly, outerRadius, outerRadius);

   polyStruct = camCadData.addPolyStruct(parentDataList,floatLayerIndex,graphicClassNormal,DbFlag0,NegativePoly);
   poly = camCadData.addFilledPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly, innerRadius, 0, 1.);
   camCadData.addVertex(*poly,-innerRadius, 0, 1.);
   camCadData.addVertex(*poly, innerRadius, 0);

   buildSpokes(camCadData,polyStruct,outerRadius*SqrtOf2,startAngleDegrees,numSpokes,spokeWidth);

   return true;
}

//_____________________________________________________________________________
CStandardApertureSquareThermal::CStandardApertureSquareThermal(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureSquareRoundThermal(pageUnits,descriptorDimensionFactorExponent)
{
}

CStandardApertureSquareThermal::CStandardApertureSquareThermal(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureSquareRoundThermal(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureSquareThermal::getType() const
{
   return standardApertureSquareThermal;
}

bool CStandardApertureSquareThermal::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   int zeroWidthIndex  = camCadData.getZeroWidthIndex();
   int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);
   CPoly* poly;

   double outerRadius = getOuterDiameter()/2.;
   double innerRadius = getInnerDiameter()/2.;
   double startAngleDegrees  = getSpokeAngleDegrees();
   int numSpokes      = getSpokeCount();
   double spokeWidth  = getSpokeGap();

   poly = camCadData.addFilledPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly, outerRadius, outerRadius);
   camCadData.addVertex(*poly,-outerRadius, outerRadius);
   camCadData.addVertex(*poly,-outerRadius,-outerRadius);
   camCadData.addVertex(*poly, outerRadius,-outerRadius);
   camCadData.addVertex(*poly, outerRadius, outerRadius);

   polyStruct = camCadData.addPolyStruct(parentDataList,floatLayerIndex,graphicClassNormal,DbFlag0,NegativePoly);
   poly = camCadData.addFilledPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly, innerRadius, innerRadius);
   camCadData.addVertex(*poly,-innerRadius, innerRadius);
   camCadData.addVertex(*poly,-innerRadius,-innerRadius);
   camCadData.addVertex(*poly, innerRadius,-innerRadius);
   camCadData.addVertex(*poly, innerRadius, innerRadius);

   buildSpokes(camCadData,polyStruct,outerRadius*SqrtOf2,startAngleDegrees,numSpokes,spokeWidth);

   return true;
}

//_____________________________________________________________________________
CStandardApertureSquareThermalOpenCorners::CStandardApertureSquareThermalOpenCorners(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundThermalRounded(pageUnits,descriptorDimensionFactorExponent)
{
}

CStandardApertureSquareThermalOpenCorners::CStandardApertureSquareThermalOpenCorners(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRoundThermalRounded(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureSquareThermalOpenCorners::getType() const
{
   return standardApertureSquareThermalOpenCorners;
}

double CStandardApertureSquareThermalOpenCorners::getArea() const
{
   // use outer perimeter for area
   double area = getOuterDiameter() * getOuterDiameter();

   return area;
}

bool CStandardApertureSquareThermalOpenCorners::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   double outerRadius = getOuterDiameter()/2.;
   double innerRadius = getInnerDiameter()/2.;
   double startAngleDegrees = getSpokeAngleDegrees();
   int numSpokes      = getSpokeCount();
   double spokeWidth  = getSpokeGap();

   double airGap = outerRadius - innerRadius;

   buildThermalOpenCorners(camCadData,polyStruct,outerRadius,outerRadius,airGap,
      startAngleDegrees,numSpokes,spokeWidth);

   return true;
}

bool CStandardApertureSquareThermalOpenCorners::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;
   double maxReduction = (getOuterDiameter() - getInnerDiameter())/2.;

   if (sizeReduction < maxReduction)
   {
      setOuterDiameter(getOuterDiameter() - sizeReduction);
      setInnerDiameter(getInnerDiameter() + sizeReduction);
      setSpokeGap(     getSpokeGap()      + (inset * SqrtOf2));

      retval = true;
   }

   return retval;
}

//_____________________________________________________________________________
CStandardApertureTriangle::CStandardApertureTriangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(360,pageUnits,descriptorDimensionFactorExponent)
{
   setBase(  getDefaultSize()/2.);
   setHeight(getDefaultSize());
}

CStandardApertureTriangle::CStandardApertureTriangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,360)
{
   if (params.GetCount() == 2)
   {
      setValid(parseParameters(params));
   }
}

StandardApertureTypeTag CStandardApertureTriangle::getType() const
{
   return standardApertureTriangle;
}

CString CStandardApertureTriangle::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

double CStandardApertureTriangle::getBase() const
{
   return getDimension(0);
}

void CStandardApertureTriangle::setBase(double base)
{
   setDimension(0,base);
}

double CStandardApertureTriangle::getHeight() const
{
   return getDimension(1);
}

void CStandardApertureTriangle::setHeight(double height)
{
   setDimension(1,height);
}

void CStandardApertureTriangle::setFromExtent(const CExtent& extent)
{
   setBase(extent.getXsize());
   setHeight(extent.getYsize());
   makeValid();
}

double CStandardApertureTriangle::getArea() const
{
   double area = (getBase() * getHeight()) / 2.;

   return area;
}

CBasesVector CStandardApertureTriangle::getMajorMinorAxes() const
{
   CBasesVector majorMinorAxes(0.,0.,1.,90.,false);

   return majorMinorAxes;
}

bool CStandardApertureTriangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfBase   = getBase()/2.;
   double halfHeight = getHeight()/2.;

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,        0, halfHeight);
   camCadData.addVertex(*poly,-halfBase,-halfHeight);
   camCadData.addVertex(*poly, halfBase,-halfHeight);
   camCadData.addVertex(*poly,        0, halfHeight);

   transformPolyToCenter(*polyStruct);

   return true;
}

CExtent CStandardApertureTriangle::getExtent() const
{
   CExtent extent;

   double halfBase   = getBase()/2.;
   double halfHeight = getHeight()/2.;
   extent.update(-halfBase,-halfHeight);
   extent.update( halfBase, halfHeight);

   return extent;
}

bool CStandardApertureTriangle::inset(double inset)
{
   bool retval = false;
   double halfBase = getBase()/2.;
   double sideLength = sqrt(getHeight()*getHeight() + halfBase*halfBase);
   double hypotenuseAltitude = getHeight() * halfBase / sideLength;

   if (hypotenuseAltitude > inset)
   {
      double reductionFactor = (hypotenuseAltitude - inset)/hypotenuseAltitude;

      setHeight(getHeight() * reductionFactor);
      setBase(  getBase()   * reductionFactor);

      retval = true;
   }

   return retval;
}

void CStandardApertureTriangle::makeValid()
{
   if (getBase() < getMinimumDimension())
   {
      setBase(getMinimumDimension());
   }

   if (getHeight() < getMinimumDimension())
   {
      setHeight(getMinimumDimension());
   }

   // reference - page 272 - CRC Standard Mathmatical Tables and Formulae - 30th Edition
   double b  = getBase();
   double hb = getHeight();
   double a  = sqrt(hb*hb + b*b/4.);
   double area = b*hb / 2.;
   double ha = 2.*area / a;
   double r = 1. / ( (2. / ha ) + (1. / hb));  // radius of inscribed circle;

   setMaxExternalCornerRadius(r);
}

void CStandardApertureTriangle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"b","Base"  ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h","Height",getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
}

void CStandardApertureTriangle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getBase()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,-halfHeight - du,
                                          halfWidth,-halfHeight - du,
                "b", horizontalPositionCenter,verticalPositionTop);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
CStandardApertureVerticalHexagon::CStandardApertureVerticalHexagon(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureHorizontalHexagon(pageUnits,descriptorDimensionFactorExponent)
{
   setWidth(getDefaultSize());

   double sideLength = getWidth()/SqrtOf3;
   setHeight(2. * sideLength);
   setCornerSize(sideLength/2.);
}

CStandardApertureVerticalHexagon::CStandardApertureVerticalHexagon(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureHorizontalHexagon(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureVerticalHexagon::getType() const
{
   return standardApertureVerticalHexagon;
}

double CStandardApertureVerticalHexagon::getArea() const
{
   double cornerCutoutArea = getCornerSize() * getWidth() / 2.;
   double area = (getWidth() * getHeight()) - (4. * cornerCutoutArea);

   return area;
}

bool CStandardApertureVerticalHexagon::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth   = getWidth()/2.;
   double halfHeight  = getHeight()/2.;
   double cornerSize  = getCornerSize();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   camCadData.addVertex(*poly,         0, halfHeight             );
   camCadData.addVertex(*poly,-halfWidth, halfHeight - cornerSize);
   camCadData.addVertex(*poly,-halfWidth,-halfHeight + cornerSize);
   camCadData.addVertex(*poly,         0,-halfHeight             );
   camCadData.addVertex(*poly, halfWidth,-halfHeight + cornerSize);
   camCadData.addVertex(*poly, halfWidth, halfHeight - cornerSize);
   camCadData.addVertex(*poly,         0, halfHeight             );

   return true;
}

void CStandardApertureVerticalHexagon::makeValid()
{
   if (getHeight() < 3.*getMinimumDimension())
   {
      setHeight(3.*getMinimumDimension());
   }

   if (getCornerSize() > getHeight()/3.)
   {
      setCornerSize(getHeight()/3.);
   }

   if (getWidth() < getMinimumDimension())
   {
      setWidth(getMinimumDimension());
   }

   setMaxExternalCornerRadius(min(getWidth(),getHeight() - 2.*getCornerSize())/2.);
}

void CStandardApertureVerticalHexagon::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,halfHeight + du,
                                          halfWidth,halfHeight + du,
                "w", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData, halfWidth + du,-halfHeight + getCornerSize(),
                                          halfWidth + du,-halfHeight,
                "r", horizontalPositionLeft,verticalPositionCenter);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);

   // corner origin
   addPointAnnotation(camCadData,halfWidth,-halfHeight);
   addPointAnnotation(camCadData,halfWidth,-halfHeight + getCornerSize());
}

//_____________________________________________________________________________
CStandardApertureNotchedRectangle::CStandardApertureNotchedRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setFeatureBase(getWidth()/2.);
   setFeatureAltitude(getHeight()/3.);
   setFeatures(2);
}

CStandardApertureNotchedRectangle::CStandardApertureNotchedRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
   if (params.GetCount() == 4 || params.GetCount() == 5)
   {
      setValid(parseParameters(params));
   }
}

StandardApertureTypeTag CStandardApertureNotchedRectangle::getType() const
{
   return standardApertureNotchedRectangle;
}

CCenterTag CStandardApertureNotchedRectangle::getCenterOptions() const
{
   return CCenterTag(centerOfExtents | centerOfRectangle | centerOfMass | centerOfVertices);
}

double CStandardApertureNotchedRectangle::getFeatureBase()   const
{
   return getDimension(2);
}

void   CStandardApertureNotchedRectangle::setFeatureBase(double width)
{
   setDimension(2,width);
}

double CStandardApertureNotchedRectangle::getFeatureAltitude() const
{
   return getDimension(3);
}

void   CStandardApertureNotchedRectangle::setFeatureAltitude(double height)
{
   setDimension(3,height);
}

int CStandardApertureNotchedRectangle::getFeatures() const
{
   return getIntDimension(4);
}

void CStandardApertureNotchedRectangle::setFeatures(int features)
{
   setDimension(4,features);
}

CString CStandardApertureNotchedRectangle::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));
   descriptor += getDescriptorDimension(0) + getDescriptorDelimiter() +
                 getDescriptorDimension(1) + getDescriptorDelimiter() +
                 getDescriptorDimension(2) + getDescriptorDelimiter() +
                 getDescriptorDimension(3) +
                 getDescriptorFeatures(4)  +
                 getDescriptorCenterInfix() +
                 getDescriptorUnitsSuffix();

   return descriptor;
}

void CStandardApertureNotchedRectangle::normalizeDimensions()
{
   normalizeDimension(0);
   normalizeDimension(1);
   normalizeDimension(2);
   normalizeDimension(3);
}

void CStandardApertureNotchedRectangle::scale(double scaleFactor)
{
   setDimension(0,getDimension(0) * scaleFactor);
   setDimension(1,getDimension(1) * scaleFactor);
   setDimension(2,getDimension(2) * scaleFactor);
   setDimension(3,getDimension(3) * scaleFactor);
}

double CStandardApertureNotchedRectangle::getArea() const
{
   double featureArea = getFeatureBase() * getFeatureAltitude();
   double area = (getWidth() * getHeight()) - (getFeatureCount(getFeatures()) * featureArea);

   return area;
}

CBasesVector CStandardApertureNotchedRectangle::getMajorMinorAxes() const
{
   double angleDegrees = 0;
   bool feature1,feature2,feature3,feature4;
   getFeatureFlags(getFeatures(),feature1,feature2,feature3,feature4);

   /*                2
              +-------------+
              |             |
            3 |             | 1
              |             |
              +-------------+
                     4
   */

   bool pointedLeft = (feature3 && !feature1);
   bool pointedDown = (feature4 && !feature2);
   bool portrait    = (getHeight() > getWidth());
   bool mirrorFlag  = (pointedLeft != pointedDown) != portrait;

   if (portrait)
   {
      angleDegrees = 90.;

      if (pointedDown)
      {
         angleDegrees += 180.;
      }

      mirrorFlag = pointedLeft;
   }
   else
   {
      angleDegrees = 0.;

      if (pointedLeft)
      {
         angleDegrees += 180.;
      }
   }

   if (mirrorFlag)
   {
      angleDegrees += 180.;
   }

   CBasesVector majorMinorAxes(0.,0.,1.,angleDegrees,mirrorFlag);

   return majorMinorAxes;
}

CBasesVector CStandardApertureNotchedRectangle::getSymmetryAxes() const
{
   /*                2
              +-------------+
              |             |
            3 |             | 1
              |             |
              +-------------+
                     4
   */

   double angleDegrees = 0;
   bool feature1,feature2,feature3,feature4;
   getFeatureFlags(getFeatures(),feature1,feature2,feature3,feature4);

   int featureSum = feature1 + feature2 + feature3 + feature4;

   // if there is no symmetry or if all four directions are equally symmetrical, then just return the major/minor axis,
   // otherwise point the symmetry in the direction of the feature.
   CBasesVector symmetryAxes;

   if ((featureSum == 1) || (featureSum == 3))
   {
      if (feature1 && !feature3)
      {
         angleDegrees =   0.;
      }
      else if (feature2 && !feature4)
      {
         angleDegrees =  90.;
      }
      else if (feature3 && !feature1)
      {
         angleDegrees = 180.;
      }
      else if (feature4 && !feature2)
      {
         angleDegrees = 270.;
      }

      symmetryAxes = CBasesVector(0.,0.,1.,angleDegrees,false);
   }
   else if ((featureSum == 2) && feature1 && feature3)
   {
      angleDegrees = 0.;

      symmetryAxes = CBasesVector(0.,0.,1.,angleDegrees,false);
   }
   else if ((featureSum == 2) && feature2 && feature4)
   {
      angleDegrees = 90.;

      symmetryAxes = CBasesVector(0.,0.,1.,angleDegrees,false);
   }
   else
   {
      symmetryAxes = getMajorMinorAxes();
   }

   return symmetryAxes;
}

bool CStandardApertureNotchedRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth           = getWidth()/2.;
   double halfHeight          = getHeight()/2.;
   double halfFeatureBase     = getFeatureBase()/2.;
   double halfFeatureAltitude = getFeatureAltitude()/2.;
   int features               = getFeatures();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   // *---2---*
   // |       |
   // 3       1
   // |       |
   // *---4---*

   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   // feature 1
   if (hasFeature(features,1))
   {
      camCadData.addVertex(*poly,halfWidth                       ,-halfFeatureBase);
      camCadData.addVertex(*poly,halfWidth - getFeatureAltitude(),-halfFeatureBase);
      camCadData.addVertex(*poly,halfWidth - getFeatureAltitude(), halfFeatureBase);
      camCadData.addVertex(*poly,halfWidth                       , halfFeatureBase);
   }

   camCadData.addVertex(*poly, halfWidth, halfHeight);

   // feature 2
   if (hasFeature(features,2))
   {
      camCadData.addVertex(*poly, halfFeatureBase,halfHeight);
      camCadData.addVertex(*poly, halfFeatureBase,halfHeight - getFeatureAltitude());
      camCadData.addVertex(*poly,-halfFeatureBase,halfHeight - getFeatureAltitude());
      camCadData.addVertex(*poly,-halfFeatureBase,halfHeight);
   }

   camCadData.addVertex(*poly,-halfWidth, halfHeight);

   // feature 3
   if (hasFeature(features,3))
   {
      camCadData.addVertex(*poly,-halfWidth                       , halfFeatureBase);
      camCadData.addVertex(*poly,-halfWidth + getFeatureAltitude(), halfFeatureBase);
      camCadData.addVertex(*poly,-halfWidth + getFeatureAltitude(),-halfFeatureBase);
      camCadData.addVertex(*poly,-halfWidth                       ,-halfFeatureBase);
   }

   camCadData.addVertex(*poly,-halfWidth,-halfHeight);

   // feature 4
   if (hasFeature(features,4))
   {
      camCadData.addVertex(*poly,-halfFeatureBase,-halfHeight);
      camCadData.addVertex(*poly,-halfFeatureBase,-halfHeight + getFeatureAltitude());
      camCadData.addVertex(*poly, halfFeatureBase,-halfHeight + getFeatureAltitude());
      camCadData.addVertex(*poly, halfFeatureBase,-halfHeight);
   }

   //
   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   transformPolyToCenter(*polyStruct);

   return true;
}

bool CStandardApertureNotchedRectangle::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction &&
       (getFeatureBase()  + sizeReduction) < (getWidth()  - sizeReduction) &&
       (getFeatureAltitude() + sizeReduction) < (getHeight() - sizeReduction))
   {
      setWidth( getWidth()  - sizeReduction);
      setHeight(getHeight() - sizeReduction);
      setFeatureBase( getFeatureBase()  + sizeReduction);
      setFeatureAltitude(getFeatureAltitude() + sizeReduction);

      retval = true;
   }

   return retval;
}

void CStandardApertureNotchedRectangle::makeValid()
{
   bool feature1,feature2,feature3,feature4;
   getFeatureFlags(getFeatures(),feature1,feature2,feature3,feature4);
   double minDimension1 = getMinimumDimension();
   double minDimension2 = 2. * minDimension1;
   double minDimension3 = 3. * minDimension1;
   double minDimension4 = 4. * minDimension1;
   double minDimension5 = 5. * minDimension1;
   double minWidth      = minDimension1;
   double minHeight     = minDimension1;

   if (getFeatureBase() < minDimension1)
   {
      setFeatureBase(minDimension1);
   }

   if (getFeatureAltitude() < minDimension1)
   {
      setFeatureAltitude(minDimension1);
   }

   if (feature2 || feature4)
   {
      if (feature2 && feature4)
      {
         if (feature1 || feature3)
         {
            minHeight = minDimension5;
         }
         else
         {
            minHeight = minDimension3;
         }
      }
      else
      {
         if (feature1 || feature3)
         {
            minHeight = minDimension4;
         }
         else
         {
            minHeight = minDimension2;
         }
      }

      minWidth = minDimension3;
   }

   if (feature1 || feature3)
   {
      if (feature1 && feature3)
      {
         if (feature2 || feature4)
         {
            minWidth = minDimension5;
         }
         else
         {
            minWidth = minDimension3;
         }
      }
      else
      {
         if (feature2 || feature4)
         {
            minWidth = minDimension4;
         }
         else
         {
            minWidth = max(minWidth,minDimension2);
         }
      }

      minHeight = max(minHeight,minDimension3);
   }

   setWidth( max(minWidth ,getWidth() ));
   setHeight(max(minHeight,getHeight()));

   if (feature2 || feature4)
   {
      if (getWidth() - getFeatureBase() < minDimension2)
      {
         setFeatureBase(getWidth() - minDimension2);
      }

      if (feature2 && feature4)
      {
         if (getHeight() - 2.*getFeatureAltitude() < minDimension2)
         {
            setFeatureAltitude((getHeight() - minDimension2)/2.);
         }
      }
      else
      {
         if (getHeight() - getFeatureAltitude() < minDimension1)
         {
            setFeatureAltitude(getHeight() - minDimension1);
         }
      }
   }

   if (feature1 || feature3)
   {
      if (getHeight() - getFeatureBase() < minDimension2)
      {
         setFeatureBase(getHeight() - minDimension2);
      }

      if (feature1 && feature3)
      {
         if (getWidth() - 2.*getFeatureAltitude() < minDimension2)
         {
            setFeatureAltitude((getWidth() - minDimension2)/2.);
         }
      }
      else
      {
         if (getWidth() - getFeatureAltitude() < minDimension1)
         {
            setFeatureAltitude(getWidth() - minDimension1);
         }
      }
   }

   if ((feature2 || feature4) && (feature1 || feature3))
   {
      bool potentialWidthConflict  = (getWidth()  - getFeatureBase() - minDimension2 < 2.*getFeatureAltitude());
      bool potentialHeightConflict = (getHeight() - getFeatureBase() - minDimension2 < 2.*getFeatureAltitude());

      if (potentialWidthConflict && potentialHeightConflict)
      {
         double maxAltitude = getFeatureAltitude();

         if (potentialWidthConflict)
         {
            maxAltitude = min(maxAltitude,(getWidth() - getFeatureBase() - minDimension2)/2.);
         }

         if (potentialHeightConflict)
         {
            maxAltitude = min(maxAltitude,(getHeight() - getFeatureBase() - minDimension2)/2.);
         }

         if (maxAltitude < minDimension1)
         {
            double minDimension = min(getWidth(),getHeight());

            if (minDimension - getFeatureBase() < minDimension4)
            {
               setFeatureBase(minDimension - minDimension4);
            }

            setFeatureAltitude(minDimension1);
         }
         else
         {
            setFeatureAltitude(maxAltitude);
         }
      }
   }

   double widthMaxExternalCornerRadius = min((getWidth() - ((feature2 || feature4) ? getFeatureBase() : 0.))/2.,
                                             (getWidth() - (feature1 + feature3) * getFeatureAltitude())/2.);

   double heightMaxExternalCornerRadius = min((getHeight() - ((feature1 || feature3) ? getFeatureBase() : 0.))/2.,
                                              (getHeight() - (feature2 + feature4) * getFeatureAltitude())/2.);

   setMaxExternalCornerRadius(min(widthMaxExternalCornerRadius,heightMaxExternalCornerRadius));
}

void CStandardApertureNotchedRectangle::storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters)
{
   apertureParameters.clear();

   apertureParameters.setParameters(0,"w"       ,"Width"           ,getDescriptorDelimiter(),getDescriptorDimension(0),getDimension(0));
   apertureParameters.setParameters(1,"h"       ,"Height"          ,getDescriptorDelimiter(),getDescriptorDimension(1),getDimension(1));
   apertureParameters.setParameters(2,"fb"      ,"Feature base"    ,getDescriptorDelimiter(),getDescriptorDimension(2),getDimension(2));
   apertureParameters.setParameters(3,"fa"      ,"Feature altitude",getDescriptorDelimiter(),getDescriptorDimension(3),getDimension(3));
   apertureParameters.setParameters(4,"features",
      "Features - a combination of 1, 2, 3, 4 to specify on which sides features are present - omitted if all 4 features are present"        ,
      getDescriptorDelimiter(),getFeaturesString(4)     );

   apertureParameters.setCenters(getCenterOptions(),getCenter());
}

void CStandardApertureNotchedRectangle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;
   double halfFeatureWidth  = getFeatureBase()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,-halfHeight - du,
                                          halfWidth,-halfHeight - du,
                "w", horizontalPositionCenter,verticalPositionTop);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData,-halfFeatureWidth,halfHeight + du,
                                          halfFeatureWidth,halfHeight + du,
                "fb", horizontalPositionCenter,verticalPositionBottom);

   addDimensionAnnotation(camCadData,halfFeatureWidth + du, halfHeight,
                                         halfFeatureWidth + du, halfHeight - getFeatureAltitude(),
                "fa", horizontalPositionLeft,verticalPositionCenter);

   // features
   addTextAnnotation(camCadData, halfWidth,         0.,"1",horizontalPositionRight  ,verticalPositionCenter);
   addTextAnnotation(camCadData,        0., halfHeight,"2",horizontalPositionCenter ,verticalPositionTop);
   addTextAnnotation(camCadData,-halfWidth,         0.,"3",horizontalPositionLeft   ,verticalPositionCenter);
   addTextAnnotation(camCadData,        0.,-halfHeight,"4",horizontalPositionCenter ,verticalPositionBottom);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//void CStandardApertureNotchedRectangle::loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters)
//{
//   setDimension(0,apertureUnitsToPageUnits(atof(apertureParameters.getAt(0)->getValue())));
//   setDimension(1,apertureUnitsToPageUnits(atof(apertureParameters.getAt(1)->getValue())));
//   setDimension(2,apertureUnitsToPageUnits(atof(apertureParameters.getAt(2)->getValue())));
//   setDimension(3,apertureUnitsToPageUnits(atof(apertureParameters.getAt(3)->getValue())));
//   setDimension(4,atoi(apertureParameters.getAt(4)->getValue()));;
//}

void CStandardApertureNotchedRectangle::setDimensionApertureUnits(int index,double dimension)
{
   if (index != 4)
   {
      dimension = apertureUnitsToPageUnits(dimension);
   }

   setDimension(index,dimension);
}

//_____________________________________________________________________________
CStandardApertureTabbedRectangle::CStandardApertureTabbedRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureNotchedRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setFeatureBase(.75*getHeight());
   setFeatureAltitude(.5*getHeight());
   setFeatures(2);
   setCenter(centerOfRectangle);
}

CStandardApertureTabbedRectangle::CStandardApertureTabbedRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureNotchedRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureTabbedRectangle::getType() const
{
   return standardApertureTabbedRectangle;
}

double CStandardApertureTabbedRectangle::getArea() const
{
   double featureArea = getFeatureBase() * getFeatureAltitude();
   double area = (getWidth() * getHeight()) + (getFeatureCount(getFeatures()) * featureArea);

   return area;
}

CBasesVector CStandardApertureTabbedRectangle::getMajorMinorAxes() const
{
   double angleDegrees = 0;
   bool feature1,feature2,feature3,feature4;
   getFeatureFlags(getFeatures(),feature1,feature2,feature3,feature4);

   /*                2
              +-------------+
              |             |
            3 |             | 1
              |             |
              +-------------+
                     4
   */

   double width  = getWidth()  + ((int)feature1 + (int)feature3) * getFeatureAltitude() * 1.001;
   double height = getHeight() + ((int)feature2 + (int)feature4) * getFeatureAltitude() * 1.001;

   bool pointedLeft = (feature3 && !feature1);
   bool pointedDown = (feature4 && !feature2);
   bool portrait    = (height > width);
   bool mirrorFlag  = (pointedLeft != pointedDown) != portrait;

   if (portrait)
   {
      angleDegrees = 90.;

      if (pointedDown)
      {
         angleDegrees += 180.;
      }

      mirrorFlag = pointedLeft;
   }
   else
   {
      angleDegrees = 0.;

      if (pointedLeft)
      {
         angleDegrees += 180.;
      }
   }

   if (mirrorFlag)
   {
      angleDegrees += 180.;
   }

   CBasesVector majorMinorAxes(0.,0.,1.,angleDegrees,mirrorFlag);

   return majorMinorAxes;
}

bool CStandardApertureTabbedRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth           = getWidth()/2.;
   double halfHeight          = getHeight()/2.;
   double halfFeatureBase     = getFeatureBase()/2.;
   double halfFeatureAltitude = getFeatureAltitude()/2.;
   int features               = getFeatures();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   // *---2---*
   // |       |
   // 3       1
   // |       |
   // *---4---*

   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   // feature 1
   if (hasFeature(features,1))
   {
      camCadData.addVertex(*poly,halfWidth                       ,-halfFeatureBase);
      camCadData.addVertex(*poly,halfWidth + getFeatureAltitude(),-halfFeatureBase);
      camCadData.addVertex(*poly,halfWidth + getFeatureAltitude(), halfFeatureBase);
      camCadData.addVertex(*poly,halfWidth                       , halfFeatureBase);
   }

   camCadData.addVertex(*poly, halfWidth, halfHeight);

   // feature 2
   if (hasFeature(features,2))
   {
      camCadData.addVertex(*poly, halfFeatureBase,halfHeight);
      camCadData.addVertex(*poly, halfFeatureBase,halfHeight + getFeatureAltitude());
      camCadData.addVertex(*poly,-halfFeatureBase,halfHeight + getFeatureAltitude());
      camCadData.addVertex(*poly,-halfFeatureBase,halfHeight);
   }

   camCadData.addVertex(*poly,-halfWidth, halfHeight);

   // feature 3
   if (hasFeature(features,3))
   {
      camCadData.addVertex(*poly,-halfWidth                       , halfFeatureBase);
      camCadData.addVertex(*poly,-halfWidth - getFeatureAltitude(), halfFeatureBase);
      camCadData.addVertex(*poly,-halfWidth - getFeatureAltitude(),-halfFeatureBase);
      camCadData.addVertex(*poly,-halfWidth                       ,-halfFeatureBase);
   }

   camCadData.addVertex(*poly,-halfWidth,-halfHeight);

   // feature 4
   if (hasFeature(features,4))
   {
      camCadData.addVertex(*poly,-halfFeatureBase,-halfHeight);
      camCadData.addVertex(*poly,-halfFeatureBase,-halfHeight - getFeatureAltitude());
      camCadData.addVertex(*poly, halfFeatureBase,-halfHeight - getFeatureAltitude());
      camCadData.addVertex(*poly, halfFeatureBase,-halfHeight);
   }

   //
   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   transformPolyToCenter(*polyStruct);

   return true;
}

CExtent CStandardApertureTabbedRectangle::getExtent() const
{
   CExtent extent;

   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;
   int features      = getFeatures();

   extent.update(-halfWidth,-halfHeight);
   extent.update( halfWidth, halfHeight);

   // feature 1
   if (hasFeature(features,1))
   {
      extent.update(halfWidth + getFeatureAltitude(),0.);
   }

   // feature 2
   if (hasFeature(features,2))
   {
      extent.update(0.,halfHeight + getFeatureAltitude());
   }

   // feature 3
   if (hasFeature(features,3))
   {
      extent.update(-halfWidth - getFeatureAltitude(),0.);
   }

   // feature 4
   if (hasFeature(features,4))
   {
      extent.update(0.,-halfHeight - getFeatureAltitude());
   }

   return extent;
}

bool CStandardApertureTabbedRectangle::transformPolyToCenter(DataStruct& polyStruct) const
{
   bool retval = false;

   if (getCenter() != centerOfRectangle)
   {
      CPoint2d newCenter;

      if (getCenter() == centerOfMass)
      {
         newCenter = polyStruct.getPolyList()->getCenterOfMass(getPageUnits());
         retval = true;
      }
      else if (getCenter() == centerOfVertices)
      {
         newCenter = polyStruct.getPolyList()->getCenterOfVertices(getPageUnits());
         retval = true;
      }
      else if (getCenter() == centerOfExtents)
      {
         newCenter = polyStruct.getPolyList()->getExtent().getCenter();
         retval = true;
      }

      if (retval)
      {
         CTMatrix matrix;
         matrix.translateCtm(newCenter);
         matrix.invert();
         polyStruct.transform(matrix);
      }
   }

   return retval;
}

// returns center of rectangle relative to center of extents
CPoint2d CStandardApertureTabbedRectangle::getRelativeCenterOfRectangle() const
{
   CPoint2d retval;

   double halfFeatureHeight  = getFeatureAltitude()/2.;
   int features              = getFeatures();

   // feature 1
   if (hasFeature(features,1))
   {
      retval.x -= halfFeatureHeight;
   }

   // feature 2
   if (hasFeature(features,2))
   {
      retval.y -= halfFeatureHeight;
   }

   // feature 3
   if (hasFeature(features,3))
   {
      retval.x += halfFeatureHeight;
   }

   // feature 4
   if (hasFeature(features,4))
   {
      retval.y += halfFeatureHeight;
   }

   return retval;
}

bool CStandardApertureTabbedRectangle::inset(double inset)
{
   bool retval = false;
   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth( getWidth()  - sizeReduction);
      setHeight(getHeight() - sizeReduction);

      if (getFeatureBase() < sizeReduction || getFeatureAltitude() < sizeReduction)
      {
         setFeatureBase( 0.);
         setFeatureAltitude(0.);
      }
      else
      {
         setFeatureBase( getFeatureBase()  - sizeReduction);
         setFeatureAltitude(getFeatureAltitude() - sizeReduction);
      }

      retval = true;
   }

   return retval;
}

void CStandardApertureTabbedRectangle::makeValid()
{
   double minDimension1 = getMinimumDimension();
   double minDimension2 = 2.*minDimension1;
   double minDimension3 = 3.*minDimension1;
   double minWidth  = minDimension1;
   double minHeight = minDimension1;

   bool feature1,feature2,feature3,feature4;
   getFeatureFlags(getFeatures(),feature1,feature2,feature3,feature4);

   if (feature2 || feature4)
   {
      minWidth = minDimension3;
   }

   if (feature1 || feature3)
   {
      minHeight = minDimension3;
   }

   setWidth(max(getWidth(),minWidth));
   setHeight(max(getHeight(),minHeight));

   if (getFeatureBase() < minDimension1)
   {
      setFeatureBase(minDimension1);
   }

   if (getFeatureAltitude() < minDimension1)
   {
      setFeatureAltitude(minDimension1);
   }

   if (feature2 || feature4)
   {
      if (getFeatureBase() > getWidth())
      {
         setFeatureBase(getWidth());
      }
   }

   if (feature1 || feature3)
   {
      if (getFeatureBase() > getHeight())
      {
         setFeatureBase(getHeight());
      }
   }

   if (feature1 || feature2 || feature3 || feature4)
   {
      setMaxExternalCornerRadius(min(getFeatureAltitude(),getFeatureBase())/2.);
   }
   else
   {
      setMaxExternalCornerRadius(min(getWidth(),getHeight())/2.);
   }
}

void CStandardApertureTabbedRectangle::addDiagramAnnotations(CCamCadData& camCadData) const
{
   double du = getAnnotationDimensionOffset();
   double halfWidth  = getWidth()/2.;
   double halfHeight = getHeight()/2.;
   double halfFeatureWidth  = getFeatureBase()/2.;

   addDimensionAnnotation(camCadData,-halfWidth,-halfHeight - du,
                                          halfWidth,-halfHeight - du,
                "w", horizontalPositionCenter,verticalPositionTop);

   addDimensionAnnotation(camCadData,-halfWidth - du, halfHeight,
                                         -halfWidth - du,-halfHeight,
                "h", horizontalPositionRight,verticalPositionCenter);

   addDimensionAnnotation(camCadData,-halfFeatureWidth,halfHeight - du,
                                          halfFeatureWidth,halfHeight - du,
                "fb", horizontalPositionCenter,verticalPositionTop);

   addDimensionAnnotation(camCadData,halfFeatureWidth + du, halfHeight,
                                         halfFeatureWidth + du, halfHeight + getFeatureAltitude(),
                "fa", horizontalPositionLeft,verticalPositionCenter);

   // features
   addTextAnnotation(camCadData, halfWidth,         0.,"1",horizontalPositionRight  ,verticalPositionCenter);
   addTextAnnotation(camCadData,        0., halfHeight,"2",horizontalPositionCenter ,verticalPositionBottom);
   addTextAnnotation(camCadData,-halfWidth,         0.,"3",horizontalPositionLeft   ,verticalPositionCenter);
   addTextAnnotation(camCadData,        0.,-halfHeight,"4",horizontalPositionCenter ,verticalPositionBottom);

   // origin
   addPointAnnotation(camCadData,0.,0.);
   //addCentroidAnnotations(camCadData);
}

//_____________________________________________________________________________
CStandardApertureSpurredRectangle::CStandardApertureSpurredRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureTabbedRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setFeatureBase(getHeight());
   setFeatureAltitude(getHeight()/2.);
   setFeatures(12);
}

CStandardApertureSpurredRectangle::CStandardApertureSpurredRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureTabbedRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureSpurredRectangle::getType() const
{
   return standardApertureSpurredRectangle;
}

double CStandardApertureSpurredRectangle::getArea() const
{
   double featureArea = (getFeatureBase() * getFeatureAltitude())/2.;
   double area = (getWidth() * getHeight()) + (getFeatureCount(getFeatures()) * featureArea);

   return area;
}

bool CStandardApertureSpurredRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth            = getWidth()/2.;
   double halfHeight           = getHeight()/2.;
   double halfFeatureBase      = getFeatureBase()/2.;
   double halfFeatureAltitude  = getFeatureAltitude()/2.;
   bool hasMaxFeatureWidth     = fpeq(halfWidth,halfFeatureBase);
   bool hasMaxFeatureHeight    = fpeq(halfHeight,halfFeatureBase);
   int features                = getFeatures();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   // *---2---*
   // |       |
   // 3       1
   // |       |
   // *---4---*

   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   // feature 1
   if (hasFeature(features,1))
   {
      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,halfWidth                    ,-halfFeatureBase);
      }

      camCadData.addVertex(*poly,halfWidth + getFeatureAltitude(),                0.);

      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,halfWidth                    , halfFeatureBase);
      }
   }

   camCadData.addVertex(*poly, halfWidth, halfHeight);

   // feature 2
   if (hasFeature(features,2))
   {
      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly, halfFeatureBase,halfHeight);
      }

      camCadData.addVertex(*poly,               0.,halfHeight + getFeatureAltitude());

      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly,-halfFeatureBase,halfHeight);
      }
   }

   camCadData.addVertex(*poly,-halfWidth, halfHeight);

   // feature 3
   if (hasFeature(features,3))
   {
      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,-halfWidth                    , halfFeatureBase);
      }

      camCadData.addVertex(*poly,-halfWidth - getFeatureAltitude(),                0.);

      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,-halfWidth                    ,-halfFeatureBase);
      }
   }

   camCadData.addVertex(*poly,-halfWidth,-halfHeight);

   // feature4
   if (hasFeature(features,4))
   {
      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly,-halfFeatureBase,-halfHeight);
      }

      camCadData.addVertex(*poly,                0.,-halfHeight - getFeatureAltitude());

      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly, halfFeatureBase,-halfHeight);
      }
   }

   //
   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   transformPolyToCenter(*polyStruct);

   return true;
}

bool CStandardApertureSpurredRectangle::inset(double inset)
{
   bool retval = false;

   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);

      double halfFeatureBase = getFeatureBase()/2.;
      double sideLength = sqrt(getFeatureAltitude()*getFeatureAltitude() + halfFeatureBase*halfFeatureBase);
      double hypotenuseAltitude = getFeatureAltitude() * halfFeatureBase / sideLength;

      if (hypotenuseAltitude > inset)
      {
         double reductionFactor = (hypotenuseAltitude - inset)/hypotenuseAltitude;

         setFeatureAltitude(getFeatureAltitude() * reductionFactor);
         setFeatureBase (getFeatureBase()  * reductionFactor);
      }
      else
      {
         setFeatureAltitude(0.);
         setFeatureBase (0.);
      }

      retval = true;
   }

   return retval;
}

//_____________________________________________________________________________
CStandardApertureVeeCutRectangle::CStandardApertureVeeCutRectangle(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureNotchedRectangle(pageUnits,descriptorDimensionFactorExponent)
{
   setFeatureBase(.75*getHeight());
   setFeatureAltitude(.5*getHeight());
   setFeatures(2);
}

CStandardApertureVeeCutRectangle::CStandardApertureVeeCutRectangle(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureNotchedRectangle(params,centerTag,rotation,pageUnits,descriptorDimensionFactorExponent)
{
}

StandardApertureTypeTag CStandardApertureVeeCutRectangle::getType() const
{
   return standardApertureVeeCutRectangle;
}

double CStandardApertureVeeCutRectangle::getArea() const
{
   double featureArea = .5 * getFeatureBase() * getFeatureAltitude();
   double area = (getWidth() * getHeight()) - (getFeatureCount(getFeatures()) * featureArea);

   return area;
}

bool CStandardApertureVeeCutRectangle::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   CPoly* poly;

   double halfWidth            = getWidth()/2.;
   double halfHeight           = getHeight()/2.;
   double halfFeatureBase      = getFeatureBase()/2.;
   double halfFeatureAltitude  = getFeatureAltitude()/2.;
   bool hasMaxFeatureWidth     = fpeq(halfWidth,halfFeatureBase);
   bool hasMaxFeatureHeight    = fpeq(halfHeight,halfFeatureBase);
   int features                = getFeatures();

   poly = camCadData.addFilledPoly(*polyStruct,widthIndex);

   // *---2---*
   // |       |
   // 3       1
   // |       |
   // *---4---*

   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   // feature 1
   if (hasFeature(features,1))
   {
      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,halfWidth                    ,-halfFeatureBase);
      }

      camCadData.addVertex(*poly,halfWidth - getFeatureAltitude(),                0.);

      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,halfWidth                    , halfFeatureBase);
      }
   }

   camCadData.addVertex(*poly, halfWidth, halfHeight);

   // feature 2
   if (hasFeature(features,2))
   {
      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly, halfFeatureBase,halfHeight);
      }

      camCadData.addVertex(*poly,               0.,halfHeight - getFeatureAltitude());

      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly,-halfFeatureBase,halfHeight);
      }
   }

   camCadData.addVertex(*poly,-halfWidth, halfHeight);

   // feature 3
   if (hasFeature(features,3))
   {
      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,-halfWidth                    , halfFeatureBase);
      }

      camCadData.addVertex(*poly,-halfWidth + getFeatureAltitude(),                0.);

      if (!hasMaxFeatureHeight)
      {
         camCadData.addVertex(*poly,-halfWidth                    ,-halfFeatureBase);
      }
   }

   camCadData.addVertex(*poly,-halfWidth,-halfHeight);

   // feature4
   if (hasFeature(features,4))
   {
      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly,-halfFeatureBase,-halfHeight);
      }

      camCadData.addVertex(*poly,                0.,-halfHeight + getFeatureAltitude());

      if (!hasMaxFeatureWidth)
      {
         camCadData.addVertex(*poly, halfFeatureBase,-halfHeight);
      }
   }

   //
   camCadData.addVertex(*poly, halfWidth,-halfHeight);

   transformPolyToCenter(*polyStruct);

   return true;
}

bool CStandardApertureVeeCutRectangle::inset(double inset)
{
   bool retval = false;

   double sizeReduction = 2. * inset;

   if (getWidth() > sizeReduction && getHeight() > sizeReduction)
   {
      setWidth(getWidth() - sizeReduction);
      setHeight(getHeight() - sizeReduction);

      double halfFeatureBase = getFeatureBase()/2.;
      double sideLength = sqrt(getFeatureAltitude()*getFeatureAltitude() + halfFeatureBase*halfFeatureBase);
      double hypotenuseAltitude = getFeatureAltitude() * halfFeatureBase / sideLength;

      if (hypotenuseAltitude > inset)
      {
         double reductionFactor = (hypotenuseAltitude + inset)/hypotenuseAltitude;

         setFeatureAltitude(getFeatureAltitude() * reductionFactor);
         setFeatureBase (getFeatureBase()  * reductionFactor);
      }
      else
      {
         setFeatureAltitude(0.);
         setFeatureBase (0.);
      }

      retval = true;
   }

   return retval;
}

void CStandardApertureVeeCutRectangle::makeValid()
{
   bool feature1,feature2,feature3,feature4;
   getFeatureFlags(getFeatures(),feature1,feature2,feature3,feature4);
   double minDimension1 = getMinimumDimension();
   double minDimension2 = 2. * minDimension1;
   double minDimension3 = 3. * minDimension1;
   double minDimension4 = 4. * minDimension1;
   double minDimension5 = 5. * minDimension1;
   double minWidth      = minDimension1;
   double minHeight     = minDimension1;

   if (getFeatureBase() < minDimension1)
   {
      setFeatureBase(minDimension1);
   }

   if (getFeatureAltitude() < minDimension1)
   {
      setFeatureAltitude(minDimension1);
   }

   if (feature2 || feature4)
   {
      if (feature2 && feature4)
      {
         minHeight = minDimension3;
      }
      else
      {
         minHeight = minDimension2;
      }

      minWidth = minDimension3;
   }

   if (feature1 || feature3)
   {
      if (feature1 && feature3)
      {
         minHeight = minDimension3;
      }
      else
      {
         minHeight = minDimension2;
      }

      minHeight = max(minHeight,minDimension3);
   }

   setWidth( max(minWidth ,getWidth() ));
   setHeight(max(minHeight,getHeight()));

   if (feature2 || feature4)
   {
      if (getWidth() - getFeatureBase() < minDimension2)
      {
         setFeatureBase(getWidth() - minDimension2);
      }

      if (feature2 && feature4)
      {
         if (getHeight() - 2.*getFeatureAltitude() < minDimension1)
         {
            setFeatureAltitude((getHeight() - minDimension1)/2.);
         }
      }
      else
      {
         if (getHeight() - getFeatureAltitude() < minDimension1)
         {
            setFeatureAltitude(getHeight() - minDimension1);
         }
      }
   }

   if (feature1 || feature3)
   {
      if (getHeight() - getFeatureBase() < minDimension2)
      {
         setFeatureBase(getHeight() - minDimension2);
      }

      if (feature1 && feature3)
      {
         if (getWidth() - 2.*getFeatureAltitude() < minDimension1)
         {
            setFeatureAltitude((getWidth() - minDimension1)/2.);
         }
      }
      else
      {
         if (getWidth() - getFeatureAltitude() < minDimension1)
         {
            setFeatureAltitude(getWidth() - minDimension1);
         }
      }
   }

   double gap = 1.e20;
   
   if (feature1 && feature3)
   {  // point to point
      gap = getWidth() - 2.*getFeatureAltitude();
   }
   else if (feature1 || feature3)
   {  // point to opposite side
      gap = getWidth() - getFeatureAltitude();
   }

   if (feature2 && feature4)
   {  // point to point
      gap = min(gap,getHeight() - 2.*getFeatureAltitude());
   }
   else if (feature2 || feature4)
   {  // point to opposite side
      gap = min(gap,getHeight() - getFeatureAltitude());
   }

   setMaxExternalCornerRadius(gap/2.);
}

//_____________________________________________________________________________
CStandardApertureInvalid::CStandardApertureInvalid(PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(0,pageUnits,descriptorDimensionFactorExponent)
{
   setValid(false);
}

CStandardApertureInvalid::CStandardApertureInvalid(CStringArray& params,CenterTag centerTag,double rotation,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent) :
   CStandardApertureShape(pageUnits,descriptorDimensionFactorExponent,centerTag,rotation,0)
{
   setValid(false);
}

StandardApertureTypeTag CStandardApertureInvalid::getType() const
{
   return standardApertureInvalid;
}

CString CStandardApertureInvalid::getDescriptor() const
{
   CString descriptor(standardApertureTypeTagToDescriptorPrefix(getType()));

   return descriptor;
}

bool CStandardApertureInvalid::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const
{
   int zeroWidthIndex  = camCadData.getZeroWidthIndex();
   CPoly* poly;

   double diameter = 20 * getUnitsFactor(pageUnitsMils,getPageUnits());
   double radius   = diameter / 2.;
   double unit     = radius   / 10.;

   poly = camCadData.addClosedPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly,     0., radius,1.);
   camCadData.addVertex(*poly,     0.,-radius,1.);
   camCadData.addVertex(*poly,     0., radius);

   poly = camCadData.addClosedPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly,-4.*unit, 6.*unit,1.);
   camCadData.addVertex(*poly,-4.*unit, 2.*unit,1.);
   camCadData.addVertex(*poly,-4.*unit, 6.*unit);

   poly = camCadData.addClosedPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly, 4.*unit, 6.*unit,1.);
   camCadData.addVertex(*poly, 4.*unit, 2.*unit,1.);
   camCadData.addVertex(*poly, 4.*unit, 6.*unit);

   poly = camCadData.addOpenPoly(*polyStruct,zeroWidthIndex);

   camCadData.addVertex(*poly,-6.*unit,-4.*unit,.5);
   camCadData.addVertex(*poly, 6.*unit,-4.*unit);

   return true;
}

//_____________________________________________________________________________
CStandardAperture::CStandardAperture(CString name,PageUnitsTag pageUnits)
{
   m_standardApertureShape = CStandardApertureShape::create(name,pageUnits);

   m_apertureBlock     = NULL;
}

CStandardAperture::CStandardAperture(StandardApertureTypeTag type,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent)
{
   m_standardApertureShape = CStandardApertureShape::create(type,pageUnits,descriptorDimensionFactorExponent);

   m_apertureBlock     = NULL;
}

CStandardAperture::CStandardAperture(ApertureShapeTag apertureShape,double sizeA,double sizeB,double sizeC,double sizeD,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent)
{
   constructAperture(apertureShape,sizeA,sizeB,sizeC,sizeD,pageUnits,descriptorDimensionFactorExponent);
}

CStandardAperture::CStandardAperture(const CStandardAperture& other)
{
   m_standardApertureShape = CStandardApertureShape::create(other.getDescriptor(),other.getPageUnits());
   m_standardApertureShape->setExteriorCornerRadius(other.getExteriorCornerRadius());

   m_apertureBlock     = NULL;
}

CStandardAperture::CStandardAperture(BlockStruct* block,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent)
{
   if (block != NULL)
   {
      constructAperture(block->getShape(),block->getSizeA(),block->getSizeB(),block->getSizeC(),block->getSizeD(),pageUnits,descriptorDimensionFactorExponent);

      m_apertureBlock = block;
      m_standardApertureShape->setRotationDegrees(block->getRotationDegrees());
   }
   else
   {
      m_apertureBlock = NULL;
      StandardApertureTypeTag type = standardApertureNull;
      m_standardApertureShape = NULL;

      if (descriptorDimensionFactorExponent < 0)
      {
         descriptorDimensionFactorExponent = CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(pageUnits);
      }
   }
}

CStandardAperture::~CStandardAperture()
{
}

bool CStandardAperture::isStandardAperture() const
{
   return getType() != standardApertureUndefined && getType() != standardApertureInvalid;
}

bool CStandardAperture::isStandardNormalAperture() const
{
   return getType() != standardApertureUndefined && getType() != standardApertureInvalid && getType() != standardApertureNull;
}

void CStandardAperture::constructAperture(ApertureShapeTag apertureShape,double sizeA,double sizeB,double sizeC,double sizeD,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent)
{
   m_apertureBlock = NULL;
   StandardApertureTypeTag type = standardApertureNull;
   m_standardApertureShape = NULL;

   if (descriptorDimensionFactorExponent < 0)
   {
      descriptorDimensionFactorExponent = CStandardApertureShape::getDefaultDescriptorDimensionFactorExponent(pageUnits);
   }

   switch (apertureShape)
   {
   case apertureRound:
      {
         CStandardApertureCircle* standardAperture = new CStandardApertureCircle(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->setDiameter(sizeA);
         //standardAperture->setSymmetry(90);

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureSquare:
      {
         CStandardApertureSquare* standardAperture = new CStandardApertureSquare(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->setSize(sizeA);
         //standardAperture->setSymmetry(90);

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureRectangle:
      {
         CStandardApertureRectangle* standardAperture = new CStandardApertureRectangle(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->setWidth( sizeA);
         standardAperture->setHeight(sizeB);
         //standardAperture->setSymmetry(180);

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureTarget:
      {
         CStandardApertureMoire* standardAperture = new CStandardApertureMoire(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->scale(sizeA/standardAperture->getDiameter());

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureThermal:
      {
         CStandardApertureRoundThermalSquare* standardAperture = new CStandardApertureRoundThermalSquare(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->setOuterDiameter(sizeA);
         standardAperture->setInnerDiameter(sizeB);
         standardAperture->setSpokeCount(4);
         standardAperture->setSpokeAngleRadians(sizeC);
         standardAperture->setSpokeGap(sizeD);
         //standardAperture->setSymmetry(90);

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureDonut:
      {
         CStandardApertureRoundDonut* standardAperture = new CStandardApertureRoundDonut(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->setOuterDiameter(sizeA);
         standardAperture->setInnerDiameter(sizeB);
         //standardAperture->setSymmetry(90);

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureOctagon:
      {
         CStandardApertureOctagon* standardAperture = new CStandardApertureOctagon(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->setWidth( sizeA);
         standardAperture->setHeight(sizeA);
         standardAperture->setCornerSize(sizeA / (2. + 1.4142));
         //standardAperture->setSymmetry(90);

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureOblong:
      {
         CStandardApertureOval* standardAperture = new CStandardApertureOval(pageUnits,descriptorDimensionFactorExponent);
         standardAperture->setWidth( sizeA);
         standardAperture->setHeight(sizeB);

         m_standardApertureShape = standardAperture;
      }

      break;
   case apertureBlank:
   case apertureComplex:
   default:
      {
         CStandardApertureNull* standardAperture = new CStandardApertureNull(pageUnits,descriptorDimensionFactorExponent);

         m_standardApertureShape = standardAperture;
      }

      break;
   }
}

double CStandardAperture::getExteriorCornerRadius() const
{
   double exteriorCornerRadius = 0.;

   if (m_standardApertureShape != NULL)
   {
      exteriorCornerRadius = m_standardApertureShape->getExteriorCornerRadius();
   }

   return exteriorCornerRadius;
}

void CStandardAperture::setExteriorCornerRadius(double exteriorCornerRadius)
{
   if (m_standardApertureShape != NULL)
   {
      m_standardApertureShape->setExteriorCornerRadius(exteriorCornerRadius);
   }
}

bool CStandardAperture::integrateExteriorCornerRadius()
{
   bool retval = false;

   if (m_standardApertureShape != NULL)
   {
      retval = m_standardApertureShape->integrateExteriorCornerRadius();
   }

   return retval;
}

void CStandardAperture::setDimensions(double dimension0,double dimension1,double dimension2,
                                      double dimension3,double dimension4,double dimension5)
{
   m_standardApertureShape->setDimension(0,dimension0);
   m_standardApertureShape->setDimension(1,dimension1);
   m_standardApertureShape->setDimension(2,dimension2);
   m_standardApertureShape->setDimension(3,dimension3);
   m_standardApertureShape->setDimension(4,dimension4);
   m_standardApertureShape->setDimension(5,dimension5);
}

CString CStandardAperture::getDescriptor() const
{
   return m_standardApertureShape->getDescriptor();
}

bool CStandardAperture::isEquivalent(const CStandardAperture& other) const
{
   bool retval = (getType() == other.getType() && getName() == other.getName());

   return retval;
}

ApertureShapeTag CStandardAperture::getApertureShape()
{
   ApertureShapeTag retval = apertureComplex;

   switch (getType())
   {
   case standardApertureCircle:                         retval = apertureRound;      break;
   case standardApertureSquare:                         retval = apertureSquare;     break;
   case standardApertureRectangle:                      retval = apertureRectangle;  break;
   case standardApertureOval:                           retval = apertureOblong;     break;
   case standardApertureRoundDonut:                     retval = apertureDonut;      break;
   case standardApertureHole:                           retval = apertureRound;      break;
   case standardApertureNull:                           retval = apertureRound;      break;
   }

   return retval;
}

bool CStandardAperture::getAperturePoly(CCamCadData& camCadData,
   CDataList& parentDataList,DataStruct* polyStruct)
{
   bool retval = m_standardApertureShape->getAperturePoly(camCadData,parentDataList,polyStruct);

   return retval;
}

CString CStandardAperture::getApertureBlockName(CCamCadData& camCadData) const
{
   CString apertureBlockName = "AP_" + getName();

   if (getExteriorCornerRadius() > 0.)
   {
      int widthIndex = camCadData.getDefinedWidthIndex(getExteriorCornerRadius()*2.);
      apertureBlockName.AppendFormat("-w%d",widthIndex);
   }

   return apertureBlockName;
}

BlockStruct* CStandardAperture::createNewAperture(CCamCadData& camCadData,CString apertureName,int fileNumber)
{
   CString widthInfix;

   if (getExteriorCornerRadius() > 0.)
   {
      int widthIndex = camCadData.getDefinedWidthIndex(getExteriorCornerRadius()*2.);
      widthInfix.Format("-w%d",widthIndex);

      apertureName += widthInfix;
   }

   if (getApertureShape() == apertureComplex || getExteriorCornerRadius() > 0.)
   {
      m_apertureBlock = &(camCadData.getDefinedAperture(apertureName,
         apertureComplex,0.,0.,0.,0.,getRotationRadians()));

      if (m_apertureBlock->getComplexApertureSubBlockNumber() == 0)
      {
         const DbFlag dbFlag = 0;
         const bool positive = false;
         const bool negative = true;

         CString complexApertureBlockNamePrefix = getName() + widthInfix + "-Cmplx";

         BlockStruct* complexApertureBlock =
            camCadData.getNewBlock(complexApertureBlockNamePrefix,"-%d",blockTypeUnknown);

         m_apertureBlock->setComplexApertureSubBlockNumber(complexApertureBlock->getBlockNumber());

         int floatLayerIndex = camCadData.getLayerIndex(ccLayerFloat);

         DataStruct* polyStruct = camCadData.addPolyStruct(
                                    *complexApertureBlock,floatLayerIndex,graphicClassNormal,dbFlag,positive);

         getAperturePoly(camCadData,complexApertureBlock->getDataList(),polyStruct);
      }
   }  // if (getApertureShape() == apertureComplex)
   else
   {
      m_apertureBlock = &(camCadData.getDefinedAperture(apertureName,
         getApertureShape(),getDimensionInPageUnits(0),getDimensionInPageUnits(1),0.,0.,getRotationRadians(),fileNumber));
   }

   return m_apertureBlock;
}

BlockStruct* CStandardAperture::findExistingAperture(CCamCadData& camCadData,CString& apertureName,int fileNumber)
{
   BlockStruct* apertureBlock = NULL;

   if (getApertureShape() == apertureComplex || getExteriorCornerRadius() > 0.)
   {
      int widthIndex = camCadData.getDefinedWidthIndex(getExteriorCornerRadius()*2.);

      apertureName.AppendFormat("-w%d",widthIndex);

      apertureBlock = camCadData.getBlock(apertureName,fileNumber);
   }
   else
   {
      apertureBlock = camCadData.getBlock(apertureName,fileNumber);
   }

   return apertureBlock;
}

BlockStruct* CStandardAperture::getDefinedAperture(CCamCadData& camCadData,const CString& apertureName,int fileNumber)
{
   if (m_apertureBlock == NULL)
   {
      CString apertureBlockName = camCadData.getNewBlockName(apertureName,"-%d");

      m_apertureBlock = createNewAperture(camCadData,apertureBlockName);
   }

   return m_apertureBlock;
}

BlockStruct* CStandardAperture::getDefinedAperture(CCamCadData& camCadData,int fileNumber)
{
   if (m_apertureBlock == NULL)
   {
      CString apertureName = "AP_" + getName();

      m_apertureBlock = findExistingAperture(camCadData,apertureName,fileNumber);

      if (m_apertureBlock == NULL)
      {
         CString newApertureName = camCadData.getNewBlockName(apertureName,"-%d",fileNumber);

         m_apertureBlock = createNewAperture(camCadData,newApertureName,fileNumber);
      }
   }

   return m_apertureBlock;
}

CExtent CStandardAperture::getExtent() const
{
   return m_standardApertureShape->getExtent();
}

bool CStandardAperture::inset(double inset)
{
   bool retval = false;

   if (m_standardApertureShape != NULL)
   {
      retval = m_standardApertureShape->inset(inset);

      if (retval)
      {
         m_apertureBlock = NULL;
      }
   }

   return retval;
}

void CStandardAperture::scale(double scaleFactor)
{
   m_standardApertureShape->scale(scaleFactor);

   m_apertureBlock = NULL;
}

double CStandardAperture::getArea()
{
   return m_standardApertureShape->getArea();
}

double CStandardAperture::getArea(CCamCadData& camCadData)
{
   double area = 0.;

   if (getType() == standardApertureNull && m_apertureBlock != NULL)
   {
      int zeroWidthIndex = camCadData.getZeroWidthIndex();
      double tolerance = camCadData.convertToPageUnits(pageUnitsMils,.1);

      BlockStruct* block = m_apertureBlock;

      if (block->isComplexAperture())
      {
         block = camCadData.getBlock(block->getComplexApertureSubBlockNumber());
      }

      for (CDataListIterator polyIterator(*block,dataTypePoly);polyIterator.hasNext();)
      {
         DataStruct* polyStruct = polyIterator.getNext();
         CPolyList& polyList = *(polyStruct->getPolyList());

         for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = polyList.GetNext(polyPos);

            int widthIndex = poly->getWidthIndex();
            double width = camCadData.getWidth(widthIndex);

            if (width != 0.)
            {
               CPoly outlinePoly(*poly);

               outlinePoly.convertToOutline(width,tolerance,zeroWidthIndex);

               area += outlinePoly.getArea();
            }
            else
            {
               area += poly->getArea();
            }
         }
      }
   }
   else
   {
      area = m_standardApertureShape->getArea();
   }

   return area;
}

int CStandardAperture::getWidthIndex(CCamCadData& camCadData)
{
   double width = 0.;

   if (getType() == standardApertureCircle || getType() == standardApertureSquare)
   {
      width = getDimensionInPageUnits(0);
   }

   int widthIndex = camCadData.getDefinedWidthIndex(width);

   return widthIndex;
}

void CStandardAperture::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;
   writeFormat.pushHeader("CStandardAperture: ");
   writeFormat.writef(
"m_name='%s', m_type='%s', m_dimension[0:5]=(%.3f, %.3f, %.3f, %.3f, %.3f, %.3f)\n",
(const char*)getName(),(const char*)standardApertureTypeTagToString(getType()),
getDimension(0),getDimension(1),getDimension(2),getDimension(3),getDimension(4),getDimension(5));

   writeFormat.popHeader();
}

