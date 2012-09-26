// $Header: /CAMCAD/4.5/read_wrt/MentorIn.h 24    4/04/06 5:05p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved. 
*/

#pragma once

#include "TypedContainer.h"
#include "PinRef.h"

/* Define section *********************************************************/

#define MAX_LINE              64000 // yes 64k
#define MAX_LAYERS            255
#define MAX_ATTR              2000

#define CREATE_UNKNOWN        -1
#define CREATE_PIN            1
#define CREATE_COMPONENT      2
#define CREATE_VIA            3
#define CREATE_GENERIC_PART   4
#define CREATE_STACKUP        5
#define CREATE_BOARD          0     // must be fixed later
#define CREATE_PANEL          6     // must be treated different -1
#define CREATE_DRAWING        7

#define POLYTYPE_INIT         1
#define POLYTYPE_TERM         2

#define TYPE_ATT              1     // attributes added to type list
#define HEIGHT_ATT            2     // attributes which indicates the component height.

#define PART_FIDUCIAL         1
#define PART_FIDUCIALTOP      2
#define PART_FIDUCIALBOT      3
#define PART_TOOL             4

// Data type that make up a geometry
#define DATA_POLYGON          1
#define DATA_ARC              2
#define DATA_CIRCLE           3
#define DATA_TEXT             4
#define DATA_PATH             5
#define DATA_ATTRIBUTE        6

#define SIZ_OF_COMMAND        (sizeof(command_lst) / sizeof(List))
#define SIZ_OF_BRD            (sizeof(brd_lst) / sizeof(List))
#define SIZ_OF_FIND           (sizeof(find_lst) / sizeof(List))
#define SIZ_OF_TECHNOLOGY     (sizeof(technology_lst) / sizeof(List))

/* Structure Section *********************************************************/

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

class MENLayer
{
public:
	MENLayer()
	{
		colorIndex = -1;
	};

   CString  name;                // layer name
   CString  topUserDefLayer;     // layer name of the top layer mapping defined by user
   CString  bottomUserDefLayer;  // layer name of the bottom layer mapping defined by user

private:
	int		colorIndex;				// layer color index as described in the mentor.clr file (if available)

public:
   CString getName() const { return name; }
   int getColorIndex() const { return colorIndex; }

   void setColorIndex(int index) { colorIndex = index; }
};
typedef CTypedPtrArray<CPtrArray, MENLayer*> CMENLayersArray;

class MENData
{
private:
   DataStruct* m_data;            // DataStruct pointer that is inserted on a layer
   CDataList*  m_dataList;        // CDataList pointer of where the DataStruct in is added to
   int         m_dataType;        // the type of data such as polygon, circle, text, attribute, path, or arc

public:
   MENData(DataStruct* data,CDataList* dataList,int dataType);

   DataStruct* getData() const { return m_data; }
   CDataList* getDataList() const { return m_dataList; }
   int getDataType() const { return m_dataType; }
};

typedef struct
{
   CString     layerName;        // layer name
   CPtrArray   dataArray;
   int         dataCnt;
}MENLayerData;

typedef struct
{
   int      stacknum;
   CString  physname;
   CString  logicalname;   // separated with comma
}MENPhyslayers;

typedef struct          // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  men_name;
   CString  cc_name;
} MEN4Attrmap;
typedef CTypedPtrArray<CPtrArray, MEN4Attrmap*> CAttrmapArray;

typedef struct          
{
   CString  men_name;
   CString  cc_name;
} MEN4Renamelayer;
typedef CTypedPtrArray<CPtrArray, MEN4Renamelayer*> CRenamelayerArray;

typedef struct          // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  vianame;    // via (1:2) name
   CString  origname;   // orignal via name (as in wire file)
   int      l1, l2;
} MEN4BuriedVia;
typedef CTypedPtrArray<CPtrArray, MEN4BuriedVia*> CBuriedViaArray;

typedef struct
{
   CString  name;       // layer name
   char  attr;          // layer attributes as defined in dbutil.h
   int   used;          // 
   int   mirror;        // Adef index for mirror
} MENAdef;

class CompPin
{
private:
   double m_x;
   double m_y;
   CString m_padStackName;
   CString m_pinName;
   Attrib* m_attrib;

public:
   CompPin(const CString& pinName)
   {
      m_x = m_y = 0.;
      m_pinName = pinName;
      m_attrib = NULL;
   }

   ~CompPin()
   {
      delete m_attrib;
   }

   double getX() { return m_x; }
   double getY() { return m_y; }
   CString getPadStackName() { return m_padStackName; }
   CString getPinName() { return m_pinName; }
   Attrib* getAttrib() { return m_attrib; }

   void setOrigin(double x,double y) { m_x = x; m_y = y; }
   void setPadStackName(const CString& padStackName) { m_padStackName = padStackName; }
   void setPinName(const CString& pinName) { m_pinName = pinName; }
   void setAttrib(Attrib* attrib) { m_attrib = attrib; }
};

typedef CTypedPtrArray<CPtrArray, CompPin*> CCompPinArray;

typedef struct
{
   double   x,y;
   double   rotation;
   int      placed;
}Centroid;

typedef struct
{
   char  *name;
   int   atttype;
   int   unit;       // some attributes get a unit flag
}Attr;

typedef struct
{
   CString  name;
   int      parttype;   // normal, fiducialtop, fiducialbot, fiducialboth
}MEN4Part;
typedef CTypedPtrArray<CPtrArray, MEN4Part*> CPartArray;

typedef struct
{
   CString  name;
}MEN4Powernet;
typedef CTypedPtrArray<CPtrArray, MEN4Powernet*> CPowernetArray;

typedef struct
{
   double d;
   int    blocknumber;
   char   unplated;
}MEN4Drill;
typedef CArray<MEN4Drill, MEN4Drill&> CDrillArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString              name;
   int                  smd;
   int                  via;                 // create_via 
   CMapStringToString   layer_elem_cnt;      // count how many elements per layer to check if this is a complex pad or not.
   CMapStringToString   complex_cnt;         // see if a complex aperture for this layers was already attached.                                 
} MEN4Padlist;
typedef CTypedPtrArray<CPtrArray, MEN4Padlist*> CPadlistArray;

typedef struct 
{
   char     form;
   double   sizeA;
   double   sizeB;
   double   offsetx, offsety;

   double getSizeA() { return sizeA; }
   double getSizeB() { return sizeB; }
   void setSizeA(double size) { sizeA = size; }
   void setSizeB(double size) { sizeB = size; }
} MenPadform;

//typedef CArray<MenPadform, MenPadform&> CPadformArray;
class CPadformArray : public CTypedPtrArrayContainer<MenPadform*>
{
};

typedef struct
{
   double   x,y;
   char     fill;    // 0 not
                     // 1 yes
   char     type;    // POLY_TYPE
} MenPoly;
typedef CArray<MenPoly, MenPoly&> CPolyArray;

class CMentorPadRule
{
private:
	CString m_padname;
	int m_startlayer;
	int m_endlayer;
	int m_topside;

public:
	CMentorPadRule(CString padname, int sl, int el, int topside)
	{
		m_padname = padname;
		m_startlayer = sl;
		m_endlayer = el;
		m_topside = topside;
	};

	CString getPadname() {return m_padname;}
	int getStartLayer() {return m_startlayer;}
	int getEndLayer() {return m_endlayer;}
	int getTopSide() {return m_topside;}
};
typedef CTypedPtrArray<CPtrArray, CMentorPadRule*> CPadRuleArray;

//-----------------------------------------------------------------------------
// CBoardLevelGeometry
//-----------------------------------------------------------------------------
class CBoardLevelGeometry
{
public:
   CBoardLevelGeometry(const CString name, BlockStruct* originalBlock);
   ~CBoardLevelGeometry();

private:
   CString m_name;
   BlockStruct* m_originalBlock;
   BlockStruct* m_derivedBlock;
   CTypedPtrListContainer<DataStruct*> m_boardInsertList;

   void reset();
   bool checkForContentOnSpecialLayer();

public:
   void addBoardLevelInsert(DataStruct* boardLeverInsertData);
   void derivedSpecialVersionOfBoardLevelGeometry();
};

//-----------------------------------------------------------------------------
// CBoardLevelGeometryMap
//-----------------------------------------------------------------------------
class CBoardLevelGeometryMap
{
public:
   CBoardLevelGeometryMap();
   ~CBoardLevelGeometryMap();

private:
   CTypedMapStringToPtrContainer<CBoardLevelGeometry*> m_boardLevelGeometryMap;

public:
   void addBoardLevelInsert(DataStruct* boardLeverInsertData);
   void derivedSpecialVersionOfBoardLevelGeometries();
   void empty();
};


/* List Section *********************************************************/

/* command_lst section */
static int prt_skip();
static int prt_create_pin();
static int prt_page();
static int prt_attribute();
static int prt_circle();
static int prt_create_component();
static int prt_text();
static int prt_path();
static int prt_polygon();
static int prt_create_via();
static int prt_create_generic_part();
static int prt_create_drawing();
static int prt_create_stackup();
static int prt_create_board();
static int prt_add();
static int prt_arc();
static int prt_initial();
static int prt_terminal();
static int prt_create_panel();
static int prt_template_path_width();
static int prt_pin();
static int prt_define_physical_layer();

static List command_lst[] =
{
   {"$$lock_windows",         prt_skip},  
   {"$$create_pin",           prt_create_pin},  
   {"$$create_via",           prt_create_via},
   {"$$create_component",     prt_create_component},
   {"$$create_generic_part",  prt_create_generic_part},
   {"$$create_drawing",       prt_create_drawing},
   {"$$create_stackup",       prt_create_stackup},
   {"$$create_board",         prt_create_board},
   {"$$create_panel",         prt_create_panel},
   {"$$page",                 prt_page},
   {"$$point_mode",           prt_skip},
   {"$$grid",                 prt_skip},
   {"$$attribute",            prt_attribute},
   {"$$circle",               prt_circle},
   {"$$text",                 prt_text},
   {"$$path",                 prt_path},
   {"$$template_line_style",  prt_skip},
   {"$$template_path_width",  prt_template_path_width},
   {"$$template_layer",       prt_skip},
   {"$$template_dimension",   prt_skip},
   {"$$template_pointer",     prt_skip},
   {"$$select_point",         prt_skip},
   {"$$polygon",              prt_polygon},
   {"$$add",                  prt_add},
   {"$$pin",                  prt_pin},
   {"$$view_layers",          prt_skip},
   {"$$arc",                  prt_arc},
   {"$$initial",              prt_initial},
   {"$$terminal",             prt_terminal},
   {"$$internal_name",        prt_skip},
   {"$$copy_layers",          prt_skip},
   {"$$add_dimension",        prt_skip},
   {"$$define_physical_layer",prt_define_physical_layer},
};

/* brd_lst section */
static int brd_create_board();
static int brd_create_panel();
static int brd_create_drawing();
static int brd_check_smdpin();
static int brd_check_attribute();
static int brd_check_path();
static int brd_check_circle();
static int brd_check_polygon();
static int brd_create_generic_part();
static int brd_create_component_part();

static List brd_lst[] =
{
   {"$$create_board",         brd_create_board},
   {"$$create_panel",         brd_create_panel},
   {"$$create_drawing",       brd_create_drawing},
   {"$$create_pin",           brd_check_smdpin},
   {"$$create_via",           brd_check_smdpin},
   {"$$create_generic_part",  brd_create_generic_part},
   {"$$create_component",     brd_create_component_part},
   {"$$attribute",            brd_check_attribute},
   {"$$path",                 brd_check_path},
   {"$$circle",               brd_check_circle},
   {"$$polygon",              brd_check_polygon},
};

/* brd_lst section */
static int find_create_board();

static List find_lst[] =
{
   {"$$create_board",         find_create_board},
};

/* brd_lst section */
static int prt_define_rule_pin();
static int prt_define_rule_via();

static List technology_lst[] =
{
   {"$set_transcript_mode",   prt_skip},
   {"$$lock_windows",         prt_skip},  
   {"$$define_technology",    prt_skip},
   {"$$set_endcode",          prt_skip},
   {"$$define_rule_board",    prt_skip},
   {"$$define_physical_layer",prt_define_physical_layer},
   {"$$define_rule_pin",      prt_define_rule_pin},
   {"$$define_rule_via",      prt_define_rule_via},
};

//Pin Type define in Pins.Pins
enum InitPropertyType
{
   InitPropertyType_PINREF,
   InitPropertyType_PINTYPE,
};

enum PinRefDisplayMode
{
   Mode_Unknown,
   Mode_Physical,
   Mode_Logical,
   Mode_Userdef,
   Mode_Hidden,
};

enum PINREF_Fields
{
   PinRefField_Unit = 0,
   PinRefField_X,
   PinRefField_Y,
   PinRefField_Orientation,
   PinRefField_Align,
   PinRefField_Height,
   PinRefField_Width,
   PinRefField_Pen,
   PinRefField_FontName,
   PinRefField_RightReading,
   PinRefField_DisplayMode,
   PinRefField_DefName,
   PinRefField_Layer
};

/******************************************************************************
* CMentorPinRef
*/
class CMentorPinRef : public CPinRef
{
private:
  PinRefDisplayMode  m_displaymode;
  bool               m_rightreadingflag;

private:
   HorizontalPositionTag getTextHorizontalAlignment(CString horizontalPos);
   VerticalPositionTag getTextVerticalAlignment(CString verticalPos);
   PinRefDisplayMode getDisplayMode(CString displaymode);

public:
   CMentorPinRef(int sysPageUnit);

   void setRefDisplayMode(PinRefDisplayMode val) {m_displaymode = val;} 
   PinRefDisplayMode getRefDisplayMode(){return m_displaymode;}

   bool getRightreadingflag(){return m_rightreadingflag;}
   void setRightreadingflag(bool val){m_rightreadingflag = val;}

   virtual double getAdjustedAngleDegree(double componentAngle, double pinAngle);
   virtual double getAdjustedAngleRadians(double componentAngle, double pinAngle);
   virtual bool setPinRef(CString CompName, CString PinNumber, CStringArray &propertyArray);
};



