// $Header: /CAMCAD/DcaLib/DcaApertureShape.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaApertureShape_h__)
#define __DcaApertureShape_h__

#pragma once

enum ApertureShapeTag
{
   apertureUndefined = 0  , // T_UNDEFINED                 0
   apertureRound     = 1  , // T_ROUND                     1
   apertureSquare    = 2  , // T_SQUARE                    2
   apertureRectangle = 3  , // T_RECTANGLE                 3
   apertureTarget    = 4  , // T_TARGET                    4
   apertureThermal   = 5  , // T_THERMAL                   5
   apertureComplex   = 6  , // T_COMPLEX                   6
   apertureDonut     = 7  , // T_DONUT                     7
   apertureOctagon   = 8  , // T_OCTAGON                   8
   apertureOblong    = 9  , // T_OBLONG                    9
   apertureBlank     = 10 , // T_BLANK                     10
   apertureUnknown   = -1
};

CString apertureShapeToName(ApertureShapeTag apertureShape);
CString apertureShapeToString(int apertureShape);
CString apertureShapeToName(int apertureShape);
ApertureShapeTag intToApertureShape(int apertureShape);

#endif
