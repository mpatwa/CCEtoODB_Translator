// $Header: /CAMCAD/4.4/read_wrt/AllegOut.h 11    3/07/04 12:28p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

#define  MAX_TEXTBLOCKS    100
#define  MAX_LAYERS        100
#define  MAX_MAPLAYERS     1000  // max layers x number of class types
#define  MAX_NEWLAYERS     1000
#define  MAX_THERMALNAME   1000

#define  PAD_136_TOPLAYER  1
#define  PAD_136_INTLAYER  2
#define  PAD_136_BOTLAYER  3
#define  PAD_136_SMTOP     4
#define  PAD_136_SMBOT     5
#define  PAD_136_PSTTOP    6
#define  PAD_136_PSTBOT    7
#define  PAD_136_FILMTOP   8
#define  PAD_136_FILMBOT   9
#define  PAD_136_MAX_LAYER 10 // change this number if you add more PAD_136_XXX

#define  CLASS_UNKNOWN           0
#define  CLASS_BOARD_GEOMETRY    1
#define  CLASS_PACKAGE_GEOMETRY  2
#define  CLASS_ETCH              3
#define  CLASS_DRAWING_FORMAT    4
#define  CLASS_MANUFACTURING     5
#define  CLASS_COMPONENT_VALUE   6
#define  CLASS_DEVICE_TYPE       7
#define  CLASS_REF_DES           8
#define  CLASS_TOLERANCE         9
#define  CLASS_USER_PART_NUMBER  10
#define  CLASS_PACKAGE_KEEPIN    11
#define  CLASS_PACKAGE_KEEPOUT   12
#define  CLASS_ROUTE_KEEPIN      13
#define  CLASS_ROUTE_KEEPOUT     14
#define  CLASS_VIA_KEEPOUT       15
#define  CLASS_ANTI_ETCH         16
#define  CLASS_PIN               17
#define  CLASS_VIA_CLASS         18

#define  CLASS_MAX_CLASSES       19

// here are the classes with underscore, thats easer to scan from the .out file
static char *lclasses[] = 
{
   "UNKNOWN",
   "BOARD_GEOMETRY",
   "PACKAGE_GEOMETRY",
   "ETCH",
   "DRAWING_FORMAT",
   "MANUFACTURING",
   "COMPONENT_VALUE",
   "DEVICE_TYPE",
   "REF_DES",
   "TOLERANCE",
   "USER_PART_NUMBER",
   "PACKAGE_KEEPIN",
   "PACKAGE_KEEPOUT",
   "ROUTE_KEEPIN",
   "ROUTE_KEEPOUT",
   "VIA_KEEPOUT",
   "ANTI_ETCH",
   "PIN",
   "VIA_CLASS",
};

// original are like the lclasses, but no underscore
static char *origclasses[] = 
{
   "UNKNOWN",
   "BOARD GEOMETRY",
   "PACKAGE GEOMETRY",
   "ETCH",
   "DRAWING FORMAT",
   "MANUFACTURING",
   "COMPONENT VALUE",
   "DEVICE TYPE",
   "REF DES",
   "TOLERANCE",
   "USER PART NUMBER",
   "PACKAGE KEEPIN",
   "PACKAGE KEEPOUT",
   "ROUTE KEEPIN",
   "ROUTE KEEPOUT",
   "VIA KEEPOUT",
   "ANTI ETCH",
   "PIN",
   "VIA CLASS",
};


typedef struct
{
   CString  name;
}Thermalname;

typedef struct
{
   CString  in;
   CString  out;
}Deflay;

typedef struct
{
   int      index;
   double   height;
}Textblock;

typedef struct
{
   CString  setupname;
   int      classtype;  // etch etc...
   CString  subclass;
}Maplay;

typedef struct
{
   CString  layername;
   CString  subclass;
}Maperror;

typedef struct
{
   int      nonetch;    
   int      classtype;  // etch etc...
   CString  subclass;
}Newlay;

typedef struct
{
   CString  compname;
   CString  devicename;    
   CString  geomname;
   int      devicefile_written;
}CompList;
typedef CTypedPtrArray<CPtrArray, CompList*> CComplistArray;

typedef struct
{
   double sizeA;
   double sizeB;
   double xoffset;
   double yoffset;
   double rotation;
   int shape;
   int complexGeomNum;
   BOOL created;
   BOOL thermal;

   double getSizeA() { return sizeA; }
   double getSizeB() { return sizeB; }
   void setSizeA(double size) { sizeA = size; }
   void setSizeB(double size) { sizeB = size; }
}PadInsert;

//-----------------------------------------------------------------------------

class CAllegroNameManager
{
private:
   CMapStringToInt      m_mapNameToBlkNum;
   CMapStringToString   m_mapBlkNumToName;
 
public:
   CString GetName(BlockStruct *block);

};
