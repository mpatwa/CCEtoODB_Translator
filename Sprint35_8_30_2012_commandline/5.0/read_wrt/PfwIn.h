// $Header: /CAMCAD/4.5/read_wrt/PfwIn.h 16    6/09/06 10:53p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "TypedContainer.h"


/* Define Section *********************************************************/
#define MAX_POLY             2000
#define MAX_LINE             10000    // yes 10K  
#define MAX_LAYERS           255
#define TEXTRATIO            (0.7)

#define  SIZ_OF_COMMAND					(sizeof(command_lst) / sizeof(List))
#define  SIZ_OF_COMMANDV3_1stPASS	(sizeof(commandv3_1stPass_lst) / sizeof(List))
#define  SIZ_OF_COMMANDV3				(sizeof(commandv3_lst) / sizeof(List))


/* Structures Section *********************************************************/

static char *protel_layer[] = // these names are syncronized with Protel V3/V4
{
   "Unknown",
   "TOP",               //1
   "MID1",              //2
   "MID2",              //3
   "MID3",              //4
   "MID4",              //5
   "MID5",              //6
   "MID6",              //7
   "MID7",              //8
   "MID8",              //9
   "MID9",              //10
   "MID10",             //11
   "MID11",             //12
   "MID12",             //13
   "MID13",             //14
   "MID14",             //15
   "BOTTOM",            //16
   "TOPOVERLAY",        //17
   "BOTTOMOVERLAY",     //18
   "TOPPASTE",          //19
   "BOTTOMPASTE",       //20
   "TOPSOLDER",         //21
   "BOTTOMSOLDER",      //22
   "PLANE1",            //23
   "PLANE2",            //24
   "PLANE3",            //25
   "PLANE4",            //26
   "DRILLGUIDE",        //27
   "KEEPOUT",           //28
   "MECHANICAL1",       //29
   "MECHANICAL2",       //30
   "MECHANICAL3",       //31
   "MECHANICAL4",       //32
   "DRILLDRAWING",      //33
   "MULTILAYER",        //34
};

// this is for version v2.8 and v3 and up
static char *protel_padform[] =
{
   "Unknown",
   "Round",
   "Rectangle",
   "Octagon",
};

//_____________________________________________________________________________
class CPfwLayer
{
private:
   int m_pfwLayerIndex;
   CString m_name;
   bool m_usedFlag;
   int m_mirroredPfwLayerIndex;

public:
   CPfwLayer(int pfwLayerIndex);

   CString getName() const { return m_name; }
   void setName(const CString& name) { m_name = name; }

   bool getUsed() const { return m_usedFlag; }
   void setUsed(bool flag) { m_usedFlag = flag; }

   int getPfwLayerIndex() const { return m_pfwLayerIndex; }

   int getMirroredPfwLayerIndex() const { return m_mirroredPfwLayerIndex; }
   void setMirroredPfwLayerIndex(int layerIndex) { m_mirroredPfwLayerIndex = layerIndex; }

   //CString  	name;
   //int   		mirrorptr;  
   //int   		used:1;
   //int   		electrical:1;
   //int   		padelectrical;	// that a number 1..x
};

//_____________________________________________________________________________
class CPfwLayerArray/* : public CTypedPtrArrayContainer<CPfwLayer*>*/
{
private:
   CTypedPtrArrayContainer<CPfwLayer*> m_layerArray;

public:
   CPfwLayer* getAt(int index);
   int getSize() const { return m_layerArray.GetSize(); }
   void empty() { m_layerArray.empty(); }

   void mirrorLayers(int pfwLayerIndex1,int pfwLayerIndex2);
   void unmirrorLayer(int pfwLayerIndex);
};

//_____________________________________________________________________________
typedef struct					// this is from DESIGN_OBJECTS | COMPONENT
{
   int      	geomindex;
   CString  	pinname;
   double   	pinx;
	double		piny;
	double		pinrot;
   int      	insertType;	// -1 is a duplicated pin, >-1 is data->inserttype
   int      	pinnr;		// pin count
   int      	netid;
	int			padstackIndex;
	double      cse;
	double      cpe;
   bool addTestPointAttrTop;
   bool addTestPointAttrBottom;  
} PfwPin;
typedef CTypedPtrArray<CPtrArray, PfwPin*> PinArray;

typedef struct					// this is from DESIGN_OBJECTS | COMPONENT
{
   int      	geomindex;
   double   	x1;
	double		y1;
	double		x2;
	double		y2;
	double		width;
	CString		layer;
} PfwCT;
typedef CTypedPtrArray<CPtrArray, PfwCT*> CTArray;

typedef struct
{
	int geomindex;
	double x1;
	double y1;
	double radius;
	double startAngle;
	double endAngle;
	double width;
	CString layer;
} PfwCA;
typedef CTypedPtrArray<CPtrArray, PfwCA*>CAArray;

typedef struct					// this is from DESIGN_OBJECTS | COMPONENT
{
   DataStruct *compdata;
   CString  	shapename;
   int      	compid;     // needed for V3/V4
   CString  	compdes;
   CString     partnumber;
   double   	centerx;    
   double   	centery;    
   double   	rotation;
   int			mirror;
   int			master;     // first, master comp definition
   int      	pincnt;     // Protel V3
   double   	desx, desy, desrot, desheight;
   BOOL     	deslindex, desfound; // des string is compdes
   double   	commentx, commenty, commentrot, commentheight;
   int      	commentlindex, commentfound;
   CString  	comment;

	PinArray 	pinArray;
	int			pinArrayCount;

	CTArray		ctArray;
	CAArray		caArray;
} PfwGeom;
typedef CTypedPtrArray<CPtrArray, PfwGeom*> GeomArray;

typedef struct         
{
   CString		shapename;
   CString		compdes;
} PfwFreePad;
typedef CTypedPtrArray<CPtrArray, PfwFreePad*> FreePadArray;

typedef struct					// this is from DESIGN_OBJECTS | COMPONENT
{
   CString		netname;
   int			netnr;
} PfwNet;
typedef CTypedPtrArray<CPtrArray, PfwNet*> NetArray;

typedef struct					// this is from DESIGN_OBJECTS | COMPONENT
{
   DataStruct  *np;
   int         layer_error_msg;  // only write a layer error message once
} PfwPolyData;
typedef CTypedPtrArray<CPtrArray, PfwPolyData*> PolyDataArray;

typedef struct					// this is from DESIGN_OBJECTS | COMPONENT
{
   int			netnr;
   int			compnr;
   int			pinnr;
} PfwNetCompPin;
typedef CTypedPtrArray<CPtrArray, PfwNetCompPin*> NetCompPinArray;

typedef struct
{
   double   	x,y;
   char     	arcflg;
   char     	fill;			// 0=no, 1=yes
   char     	type;			// POLY_TYPE
} PfwPoly;

typedef struct
{
   char     	form;
   double   	sizeA;
   double   	sizeB;
   double   	offsetx, offsety;
   double   	rotation;
} PfwPadform;
typedef CArray<PfwPadform, PfwPadform&> PadformArray;

class PFWPadstack
{
private:
   double   	m_topx, m_topy;
   int      	m_topshape;
   double   	m_midx, m_midy;
   int      	m_midshape;
   double   	m_botx, m_boty;
   int      	m_botshape;
   double   	m_drill;
   double   	m_soldermask_expansion;
   double   	m_pastemask_expansion;
   CString     m_layer;
   int         m_layerpair;  // valid values are 0..8, -1 = not used
   
public:
   PFWPadstack(
      double topx, double topy, int topshape,
      double midx, double midy, int midshape,
      double botx, double boty, int botshapem,
      double drill, double solderexp, double pasteexp,
      CString layer, int layerpair);

   PFWPadstack(PFWPadstack &other);

#ifdef NOT_USED_BUT_IF_EVER_NEEDED_HERE_THEY_ARE
   double getTopX()  { return m_topx; }
   double getTopY()  { return m_topy; }
   int getTopShape() { return m_topshape; }

   double getMidX()  { return m_midx; }
   double getMidY()  { return m_midy; }
   int getMidShape() { return m_midshape; }

   double getBotX()  { return m_botx; }
   double getBotY()  { return m_boty; }
   int getBotShape() { return m_botshape; }

   double getDrill() { return m_drill; }
   double getSolderExpansion()   { return m_soldermask_expansion; }
   double getpasteExpansion()    { return m_pastemask_expansion; }
   CString getLayer()            { return m_layer; }
   int getLayerPair()            { return m_layerpair; }
#endif

   bool operator==(PFWPadstack &other);

};

class PFWPadstackArray
{
private:
   CTypedPtrArrayContainer<PFWPadstack*> m_padstacks;

public:
   int FindIndex(PFWPadstack *pattern);

   void empty()               { m_padstacks.empty(); }
   void Add(PFWPadstack* ps)  { m_padstacks.Add(ps); }
   void Add(PFWPadstack& ps)  { m_padstacks.Add( new PFWPadstack(ps) ); }
   int getNextIndex()         { return m_padstacks.GetCount(); }
   
};

typedef struct
{
   double		d;
   int			toolindex;
} PFWDrill;
typedef CArray<PFWDrill, PFWDrill&> DrillArray;

typedef struct
{
   char			*token;
   int			(*function)();
} List;


/* Lists Section *********************************************************/

/* command_lst section */
static int pfw_comp();
static int pfw_endcomp();
static int pfw_ct();
static int pfw_cp();
static int pfw_cs();
static int pfw_ca();
static int pfw_cf();
static int pfw_cv();
static int pfw_ft();
static int pfw_ff();
static int pfw_fv();
static int pfw_fs();
static int pfw_fa();
static int pfw_fp();
static int pfw_pg();
static int pfw_netdef();

static List command_lst[] =
{
   {"COMP",       pfw_comp},  
   {"ENDCOMP",    pfw_endcomp},
   {"CT",         pfw_ct},
   {"CP",         pfw_cp},
   {"CS",         pfw_cs},
   {"CA",         pfw_ca},
   {"CF",         pfw_cf},
   {"CV",         pfw_cv},
   {"FT",         pfw_ft},
   {"FF",         pfw_ff},
   {"FV",         pfw_fv},
   {"FS",         pfw_fs},
   {"FA",         pfw_fa},
   {"FP",         pfw_fp},
   {"PG",         pfw_pg},
   {"NETDEF",     pfw_netdef},
};

/* commandv3_1stPass_lst section */
static int pfwv3_board();
static int pfwv3_component_1stPass();
static int pfwv3_pad_1stPass();
static int pfwv3_text_1stPass();

static List commandv3_1stPass_lst[] =
{
   {"Board",                           pfwv3_board},  
   {"Component",                       pfwv3_component_1stPass},
   {"Pad",                             pfwv3_pad_1stPass},
   {"Text",                            pfwv3_text_1stPass},
};

/* commandv3_lst section */
static int pfwv3_skiprecord();
static int pfwv3_net();
static int pfwv3_component();
static int pfwv3_polygon();
static int pfwv3_track();
static int pfwv3_fill();
static int pfwv3_pad();
static int pfwv3_arc();
static int pfwv3_text();
static int pfwv3_via();
static int pfwv3_rule();

static List commandv3_lst[] =
{
   {"Board",                           pfwv3_board},  
   {"EngineeringChangeOrderOptions",   pfwv3_skiprecord},
   {"OutputOptions",                   pfwv3_skiprecord},
   {"PrinterOptions",                  pfwv3_skiprecord},
   {"GerberOptions",                   pfwv3_skiprecord},
   {"AdvancedPlacerOptions",           pfwv3_skiprecord},
   {"AdvancedRouterOptions",           pfwv3_skiprecord},
   {"DesignRuleCheckerOptions",        pfwv3_skiprecord},
   {"Connection",                      pfwv3_skiprecord},
   {"Class",                           pfwv3_skiprecord},
   {"Rule",                            pfwv3_rule},
   {"Net",                             pfwv3_net},
   {"Component",                       pfwv3_component},
   {"Polygon",                         pfwv3_polygon},   // not implemented.
   {"Track",                           pfwv3_track},
   {"Fill",                            pfwv3_fill},
   {"Pad",                             pfwv3_pad},
   {"Arc",                             pfwv3_arc},
   {"Text",                            pfwv3_text},
   {"Via",                             pfwv3_via},
};