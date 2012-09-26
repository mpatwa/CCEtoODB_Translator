// $Header: /CAMCAD/4.5/read_wrt/Unicam_o.h 8     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once


/* Define Section *********************************************************/

// layer flags for connect layer table
#define  LAY_TOP              -1
#define  LAY_BOT              -2
#define  LAY_INNER            -3
#define  LAY_ALL              -4
#define  LAY_OUTER            -5

// optimize complex apertures
#define  PADSHAPE_UNKNOWN     0x0      
#define  PADSHAPE_CIRCLE      0x1
#define  PADSHAPE_POLYGON     0x2
#define  PADSHAPE_RECTANGULAR 0x4


/* Structures Section *********************************************************/

class CUnicamPadstack
{
private:
   ApertureShapeTag m_shapeTypeTop;
   ApertureShapeTag m_shapeTypeBottom;

public:
   CUnicamPadstack();

   int            block_num;  
   CString        name;
   double         drill;
   int            typ;        // return 0x1 top, 0x2 bottom, and 0x4 drill
   double         xsizetop;
	double			ysizetop;
	double			xofftop;
	double			yofftop;
	double			toprotation;
   double         xsizebot;
	double			ysizebot;
	double			xoffbot;
	double			yoffbot;
	double			botrotation;

public:
   ApertureShapeTag getShapeTypeTop() const { return m_shapeTypeTop; }
   void setShapeTypeTop(ApertureShapeTag shapeType) { m_shapeTypeTop = shapeType; }

   ApertureShapeTag getShapeTypeBottom() const { return m_shapeTypeBottom; }
   void setShapeTypeBottom(ApertureShapeTag shapeType) { m_shapeTypeBottom = shapeType; }

   ApertureShapeTag getShapeType() const;
};

typedef CTypedPtrArray<CPtrArray, CUnicamPadstack*> UNICAMPadstackArray;

typedef struct
{
   int            block_num;  
   CString        name;
   double         xMax;
	double			yMax;
	double			xMin;
	double			yMin;
	double			xOffset;
	double			yOffset;
}UNICAMclocation;
typedef CTypedPtrArray<CPtrArray, UNICAMclocation*> UNICAMclocationArray;

typedef struct
{
   CString			netname;
   POSITION			pos;
}UNICAMRoutes;
typedef CTypedPtrArray<CPtrArray, UNICAMRoutes*> UNICAMRoutesArray;

typedef struct
{
   double			pinx;
	double			piny;
   double			rotation;
   CString			padstackname;
   char				mirror;
   char				drill; 
   CString			pinname;
}UNICAMCompPinInst;
typedef CTypedPtrArray<CPtrArray, UNICAMCompPinInst*> CompPinInstArray;

typedef struct
{
   CString  		compname;
   CString  		devicename;    
   CString  		geomname;
   int      		geomnum;
   int      		devicefile_written;
}UNICAMCompList;
typedef CTypedPtrArray<CPtrArray, UNICAMCompList*> CComplistArray;

typedef struct
{
   int      		geomnum;
   CString  		name;
   int      		index;
}UNICAMShapePin;
typedef CTypedPtrArray<CPtrArray, UNICAMShapePin*> CShapePinArray;
