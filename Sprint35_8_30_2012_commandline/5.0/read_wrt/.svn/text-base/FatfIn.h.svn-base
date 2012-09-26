// $Header: /CAMCAD/4.5/read_wrt/FatfIn.h 11    4/06/05 11:32a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "RegularExpression.h"

/* Define Section *****************************************************************************/

#define  DUMMYPADSTACK        "$PADSTACK"
#define FATFERR               "fatf.log"

#define MAX_LINE                1000  /* Max line length. Long attributes can extent a line  */
#define MAX_LAYERS              255   /* Max number of layers.   */
#define MAX_COM_PINS            5000  /* Max number of pins / comp. */

#define BRK_SQUIGLY           0
#define BRK_B_SQUIGLY         1

#define  SIZ_BRK_LST          (sizeof(brk_lst)        / sizeof(List))
#define  SIZ_INIT_LST         (sizeof(init_lst)       / sizeof(List))
#define  SIZ_PADSTACK_LST     (sizeof(padstack_lst)   / sizeof(List))
#define  SIZ_NETS_LST         (sizeof(nets_lst)       / sizeof(List))
#define  SIZ_FINAL_LST        (sizeof(final_lst)      / sizeof(List))
#define  SIZ_BOARD_DATA_LST   (sizeof(board_data_lst) / sizeof(List))

/* Struct Section *****************************************************************************/

typedef struct
{
   double d;   
   int    toolindex;
}FATFDrill;
typedef CArray<FATFDrill, FATFDrill&> CDrillArray;

//Nail         X         Y   Type Grid T/B  Net   Net Name   Virtual Pin/Via
typedef struct          
{
   CString  nailname;
   double   x,y;
   int      bottom;
   int      type;       // this is a probe type. 
   CString  probedef;   // this is the CAMCAD name done through lookup from the fabascii.in 
   int      netnr;
   CString  grid;
   CString  netname;
   char  via;        // is this a via or comp.pin record
   char  pinxyfound;
   CString  comp,pin;   
   double   pinx, piny; // from layout check with updated CAMCAD coos
   CompPinStruct *targetPin;  // the target if is part/pin (i.e. not via)
   DataStruct *targetVia;     // the target if is via
} FABNail;
typedef CTypedPtrArray<CPtrArray, FABNail*> CNailArray;

typedef struct          
{
   int      type;
   CString  probename;
} FABNaildef;
typedef CTypedPtrArray<CPtrArray, FABNaildef*> CNaildefArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  uni_name;
   CString  cc_name;
} UAttrmap;
typedef CTypedPtrArray<CPtrArray, UAttrmap*> CAttrmapArray;

typedef struct          
{
   int      index;
   CString  name;
   double   charheight;
   double   charwidth;
   double   strokewidth;
   double   charspace;
} FATFFont;
typedef CTypedPtrArray<CPtrArray, FATFFont*> CFontArray;

typedef struct          
{
   int      form;
   double   sizeA;
   double   sizeB;
   double   offx, offy;
   double   rot;

   double getSizeA() { return sizeA; }
   double getSizeB() { return sizeB; }
   void setSizeA(double size) { sizeA = size; }
   void setSizeB(double size) { sizeB = size; }
} FATFAperture;
typedef CTypedPtrArray<CPtrArray, FATFAperture*> CApertureArray;

typedef struct          
{
   int      index;
   CString  name;
   char  power_or_signal;
} FATFNetname;
typedef CTypedPtrArray<CPtrArray, FATFNetname*> CNetnameArray;

typedef struct          
{
   int      index;
   CString  name;
   CString  package;
} FATFComp;
typedef CTypedPtrArray<CPtrArray, FATFComp*> FATFCompArray;

typedef struct          
{
   int      index;
   CString  name;
} FATFPacklist;
typedef CTypedPtrArray<CPtrArray, FATFPacklist*> CPacklistArray;

typedef struct          
{
   int      netindex;
   int      compindex;
   int      pinindex;
   long     ifp_line;         // just for later errormessages
} FATFComppin;
typedef CTypedPtrArray<CPtrArray, FATFComppin*> CComppinArray;

typedef struct          
{
   int      packindex;
   int      pinindex;
   CString  pinname;
} FATFPackpin;
typedef CTypedPtrArray<CPtrArray, FATFPackpin*> CPackpinArray;

typedef struct          
{
   int      pinID;
   double   x, y;
   CString  pinName;
   char  pinType;
   CString  padstackName;
} FATFPackagePin;
typedef CTypedPtrArray<CPtrArray, FATFPackagePin*> PackagePinArray;

typedef struct          
{
   int      index;
   CString  name;
   CString  side;
   int      mirrorindex;
   CString  layertype;
} FATFLayername;
typedef CTypedPtrArray<CPtrArray, FATFLayername*> CLayernameArray;

typedef struct          
{
   int      index;
   CString  name;
   CString  set;
} FATFLayerset;
typedef CTypedPtrArray<CPtrArray, FATFLayerset*> CLayersetArray;

typedef struct          
{
   int      index;
   CString  name;
} FATFPadstack;
typedef CTypedPtrArray<CPtrArray, FATFPadstack*> CPadstackArray;

typedef CArray<Point2, Point2&> CPolyArray;

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

//_____________________________________________________________________________
class CFatfReader
{
private:
   CCEtoODBDoc& m_camCadDoc;
   FileStruct* m_pcbFile;
   int m_startingBlockIndex;
   CRegularExpressionList m_fiducialExpressionList;
   CRegularExpressionList m_testPointExpressionList;
   CRegularExpressionList m_toolingExpressionList;

public:
   CFatfReader(CCEtoODBDoc& camCadDoc);

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   void setPcbFile(FileStruct* pcbFile) { m_pcbFile = pcbFile; }

   void addFiducialExpression(const CString& regularExpressionString);
   bool isFiducialEntityName(const CString& entityName);

   void addTestPointExpression(const CString& regularExpressionString);
   bool isTestPointEntityName(const CString& entityName);

   void addToolingExpression(const CString& regularExpressionString);
   bool isToolingEntityName(const CString& entityName);

   void setStartingBlockIndex(int blockIndex) { m_startingBlockIndex = blockIndex; }

   void fixBlockTypesAndInsertTypes();
};


/* List Section *****************************************************************************/

/* Bracket definitions */
static int fnull();

static List brk_lst[] =
{
   "{",                    fnull,
   "}",                    fnull,
};

/* init_lst section */
static int fatf_remark();
static int fatf_fabmaster();
static int fatf_units();
static int fatf_noautorotate();
static int fatf_notrace();
static int fatf_no_pad_stack_group();
static int fatf_layer_names();
static int fatf_layer_sets();
static int fatf_fonts();
static int skip_endofsection();
static int fatf_pad_symbols();
static int fatf_end();

static List init_lst[] =
{
   ";",                    fatf_remark,
   ":FABMASTER",           fatf_fabmaster,
   ":UNITS",               fatf_units,
   ":NOAUTOROTATE",        fatf_noautorotate,
   ":NOTRACE",             fatf_notrace,
   ":NO_PAD_STACK_GROUP",  fatf_no_pad_stack_group,
   ":LAYER_NAMES",         fatf_layer_names,
   ":LAYER_SETS",          fatf_layer_sets,
   ":FONTS",               fatf_fonts,
   ":BOARD_DATA",          skip_endofsection,
   ":PARTS",               skip_endofsection,
   ":NETS",                skip_endofsection,
   ":PAD_SYMBOLS",         fatf_pad_symbols,
   ":PAD_STACKS",          skip_endofsection,
   ":PADS",                skip_endofsection,
   ":PACKAGES",            skip_endofsection,
   ":TEST_POINTS",         skip_endofsection,
   ":LAYERS",              skip_endofsection,         // this is traces

   ":EOF",                 fatf_end,
};

/* padstack_lst section */
static int fatf_pad_stacks();

static List padstack_lst[] =
{
   ";",                    fatf_remark,
   ":FABMASTER",           skip_endofsection,
   ":UNITS",               skip_endofsection,
   ":NOAUTOROTATE",        skip_endofsection,
   ":NOTRACE",             skip_endofsection,
   ":NO_PAD_STACK_GROUP",  skip_endofsection,
   ":LAYER_NAMES",         skip_endofsection,
   ":LAYER_SETS",          skip_endofsection,
   ":FONTS",               skip_endofsection,
   ":BOARD_DATA",          skip_endofsection,
   ":PARTS",               skip_endofsection,
   ":NETS",                skip_endofsection,
   ":PAD_SYMBOLS",         skip_endofsection,
   ":PAD_STACKS",          fatf_pad_stacks,
   ":PADS",                skip_endofsection,
   ":PACKAGES",            skip_endofsection,
   ":TEST_POINTS",         skip_endofsection,
   ":LAYERS",              skip_endofsection,         // this is traces

   ":EOF",                 fatf_end,
};

/* start_lst section */
static int fatf_board_data();
static int fatf_parts();
static int fatf_nets();
static int fatf_pads();
static int fatf_packages();
static int fatf_test_points();
static int fatf_layers();

static List nets_lst[] =
{
   ";",                    fatf_remark,
   ":FABMASTER",           skip_endofsection,
   ":UNITS",               skip_endofsection,
   ":NOAUTOROTATE",        fatf_noautorotate,
   ":NOTRACE",             skip_endofsection,
   ":NO_PAD_STACK_GROUP",  skip_endofsection,
   ":BOARD_DATA",          skip_endofsection,
   ":PARTS",               skip_endofsection,
   ":NETS",                fatf_nets,
   ":PAD_SYMBOLS",         skip_endofsection,
   ":LAYER_NAMES",         skip_endofsection,
   ":LAYER_SETS",          skip_endofsection,
   ":PAD_STACKS",          skip_endofsection,
   ":PADS",                skip_endofsection,
   ":FONTS",               skip_endofsection,
   ":PACKAGES",            skip_endofsection,
   ":TEST_POINTS",         skip_endofsection,
   ":LAYERS",              skip_endofsection,         // this is traces

   ":EOF",                 fatf_end,
};

static List final_lst[] =
{
   ";",                    fatf_remark,
   ":FABMASTER",           fatf_fabmaster,
   ":UNITS",               fatf_units,
   ":NOAUTOROTATE",        fatf_noautorotate,
   ":NOTRACE",             fatf_notrace,
   ":NO_PAD_STACK_GROUP",  fatf_no_pad_stack_group,
   ":BOARD_DATA",          fatf_board_data,
   ":PARTS",               fatf_parts,
   /////uncomment this to cause a bug ":NETS",                fatf_nets,
   ":PAD_SYMBOLS",         skip_endofsection,
   ":LAYER_NAMES",         skip_endofsection,
   ":LAYER_SETS",          skip_endofsection,
   ":PAD_STACKS",          skip_endofsection,
   ":PADS",                fatf_pads,
   ":FONTS",               skip_endofsection,
   ":PACKAGES",            fatf_packages,
   ":TEST_POINTS",         fatf_test_points,
   ":LAYERS",              fatf_layers,         // this is traces

   ":EOF",                 fatf_end,
};

/* board_data_lst section */
static int fatf_board_data_job();
static int fatf_board_data_contour();
static int fatf_board_data_work_space();
static int fatf_board_data_fiducials();
static int fatf_board_data_thickness();

static List board_data_lst[] =
{
   "JOB",                  fatf_board_data_job,
   "CONTOUR",              fatf_board_data_contour,
   "WORK_SPACE",           fatf_board_data_work_space,
   "FIDUCIALS",               fatf_board_data_fiducials,
   "THICKNESS",               fatf_board_data_thickness,
};

