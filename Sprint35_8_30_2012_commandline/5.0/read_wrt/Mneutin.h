// $Header: /CAMCAD/4.4/read_wrt/Mneutin.h 15    10/20/04 5:54p Dean Admin $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/
#if !defined(__MneutIn_h__)
#define __MneutIn_h__

#pragma once

#include <afxtempl.h>
#include "TypedContainer.h"
#include "Data.h"
#include "File.h"

#define  MAX_TOKEN            127
#define  MAX_ATT              255
#define  MAX_COMPPIN          1000
#define  MAX_LAYERS           255
#define  MAX_ATTR             2000

#define  MEN_UNKNOWN          0
#define  MEN_GEOM             1     
#define  MEN_COMP             2
#define  MEN_WIRE             3
#define  MEN_NETLIST          4

#define  CREATE_PANEL         -1    // yes must be 0 or less
#define  CREATE_BOARD         0
#define  CREATE_PAD           1
#define  CREATE_GEOM          2
#define  CREATE_VIA           3
#define  CREATE_MULTIPLEBOARD 4

#define  POLYTYPE_INIT        1
#define  POLYTYPE_TERM        2

#define  NEUT_GEOM            0x01
#define  NEUT_B_ATTR          0x02
#define  NEUT_COMP            0x04
#define  NEUT_PAD             0x08

#define  FIDUCIAL_ATTR        1
#define  GENERIC_ATTR         2
#define  TOOLING_ATTR         3  
#define  PADIGNORE_ATTR       4
#define  PADFIDUCIAL_ATTR     5
#define  XOUTGEOM_ATTR        6
#define  GEOMTOFIDUCIAL_ATTR  7

#define  SURF_TOP             "PAD_1"
#define  SURF_BOT             "PAD_2"
#define  LAYER_ALL            "ALL"
 
#define  PADTYPE_THRU         0
#define  PADTYPE_SURF         1
#define  PADTYPE_BLIND        2
#define  PADTYPE_BURIED       3

// this is the tolerance to find Via testpoints NTEST
#define  TOL                  0.002

typedef struct
{
   CString  geomName;
   int      bottom;
} NeutralGeomToFid;

typedef struct
{
   CString  geomName;
   int      bottom;
} NeutralXOutGeom;

typedef struct
{
   char  *name;
   int   atttype;
} NeutralAttr;

typedef struct
{
   CString  name;       // theda layer name
   char  attr;          // layer attributes as defined in dbutil.h
} NeutralAdef;

typedef struct
{
   char  *name;
   int   mirrorLayerNum;   
   int   padelectrical; // that a number 1..x
} NeutralLayer;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
} NeutralAttribmap;
typedef CTypedPtrArray<CPtrArray, NeutralAttribmap*> CAttribmapArray;

typedef struct             // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  name;
   double   centerx;       // center x and y is in native units, because
   double   centery;       // at the moment of collectoin, I do not know the
                           // units.

   BOOL     centroidfound; // are below values initialized
   double   centroidx, centroidy,centroidangle; // this is on units (already converted with scale)

   int      boardnumber;
   int      place_outline; // component_placement_outline
} NeutralGeom;
typedef CTypedPtrArray<CPtrArray, NeutralGeom*> GeomArray;

typedef struct             // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  name;
   int      pinaccess;
   int      padtype;
   CString  layerlist;     // list for pads layer , komma seperated.
   int      hasContent;    // added DF 2004.10.20 to track padstacks without content
//PAD PIN 40TP25rd12 Thru 0.012 PHYSICAL_1 PHYSICAL_8 PTH
//PAD PIN 0.254x1.65 Surf none PHYSICAL_1 PHYSICAL_1 PHYSICAL_8 PHYSICAL_8
   CString  from_topplace, to_topplace;            // this is for SMD and BURIED;
   CString  from_bottomplace, to_bottomplace;
} NeutralPadstack;
typedef CTypedPtrArray<CPtrArray, NeutralPadstack*> PadstackArray;

typedef struct             
{
   CString  compname;
   CString  partnumber;
   CString  pn_ext;
   int      bnum;
} NeutralPartnumber;
typedef CTypedPtrArray<CPtrArray, NeutralPartnumber*> PartnumberArray;

typedef struct
{
   double   x,y;
   char  fill;    // 0 not
                  // 1 yes
   char  type;    // POLY_TYPE
} NeutralPoly;
typedef CArray<NeutralPoly, NeutralPoly&> CPolyArray;

typedef struct
{
   char     form;
   double   sizeA;
   double   sizeB;

   double getSizeA() { return sizeA; }
   double getSizeB() { return sizeB; }
   void setSizeA(double size) { sizeA = size; }
   void setSizeB(double size) { sizeB = size; }
} NeutralPadform;

//typedef CArray<NeutralPadform, NeutralPadform&> CPadformArray;
class CPadformArray : public CTypedPtrArrayContainer<NeutralPadform*>
{
};

typedef struct
{
   double d;
   int    toolindex;
   char   unplated;
} NeutralDrill;
typedef CArray<NeutralDrill, NeutralDrill&> CDrillArray;

typedef struct
{
   CString  refname;
   CString  orig_geomname;
   CString  new_geomname;
   double   x, y;
   double   rot;
   int      mirror;
} NeutralComp;

typedef struct
{
   CString  name;
   double   x, y, rot;
   int      filenum;
   int      flag;       // this checks which board has what entities NEUT_GEOM | NEUT_COMP 
   BOOL     skip_board;
} NeutralBoard;

typedef struct
{
   FileStruct *panelFile;
   BOOL HasPanelOutline;
   double xmin;
   double xmax;
   double ymin;
   double ymax;
} NeutralPanel;
typedef CTypedPtrArray<CPtrArray, NeutralPanel*> PanelArray;

typedef struct
{
   DataStruct  *data;            // DataStruct pointer that is inserted on a layer
   CDataList   *dataList;        // CDataList pointer of where the DataStruct in is added to
   int         dataType;         // the type of data such as polygon, circle, text, attribute, path, or arc
}NeutralData;

typedef struct
{
   CString     layerName;        // layer name
   CPtrArray   dataArray;
   int         dataCnt;
}NeutralLayerData;

static int neut_net();
static int neut_geom();
static int neut_board();
static int neut_g_attr();
static int neut_g_pin();
static int neut_b_attr();
static int neut_comp();
static int neut_c_prop();
static int neut_n_prop();
static int neut_p_addp();
static int neut_b_addp();
static int neut_hole();
static int neut_pad();
static int neut_p_shape();
static int neut_n_pin();
static int neut_n_via();
static int neut_skip();
static int neut_c_pin();
static int neut_n_test();
static int net_board();
static int neut_t_test();

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

static List command_lst[] =
{
   {"B_UNITS",       neut_skip},  
// {"NET",           neut_net},  
   {"GEOM",          neut_geom},
   {"BOARD",         neut_board},
   {"COMP",          neut_comp},
   {"PAD",           neut_pad},
   {"C_PROP",        neut_c_prop},
   {"C_PIN",         neut_c_pin},      // local component pin definition
   {"G_ATTR",        neut_g_attr},
   {"G_PIN",         neut_g_pin},
   {"B_ATTR",        neut_b_attr},
   {"B_ADDP",        neut_b_addp},
   {"HOLE",          neut_hole},
   {"P_ADDP",        neut_p_addp},
   {"P_SHAPE",       neut_p_shape}, 
// {"N_PIN",         neut_n_pin},      
// {"N_VIA",         neut_n_via},      
// {"N_TEST",        neut_n_test},     
// {"N_PROP",        neut_n_prop},  // skip is not eliminated.
};
#define  SIZ_OF_COMMAND (sizeof(command_lst) / sizeof(List))

static List netlist_lst[] =
{
   {"BOARD",         net_board}, // this only counts the boards
   {"NET",           neut_net},  
   {"N_PIN",         neut_n_pin},      
   {"N_VIA",         neut_n_via},      
   {"N_TEST",        neut_n_test},     
   {"N_PROP",        neut_n_prop},  // no skiptest done. 
   {"T_TEST",        neut_t_test},  // testprobe
};
#define  SIZ_OF_NETLIST (sizeof(netlist_lst) / sizeof(List))

enum EMneutTestPinType
{
   mneutTestPinTypeComppin,
   mneutTestPinTypeVia
};

class CMneutTestPin
{
private:
   EMneutTestPinType m_pinType;  // mneutTestPinTypeComppin or mneutTestPinTypeVia
   int m_testAccessCode;         // ATT_TEST_ACCESS_ALL, ATT_TEST_ACCESS_TOP, or ATT_TEST_ACCESS_BOT
   CString m_refname;
   NetStruct *m_net;

   // Pin name used for comppins only
   CString m_pinname;

   // Location used for vias only
   double m_x; 
   double m_y;

public:
   CMneutTestPin(CString refname, CString pinname, int testaccess, NetStruct *net);     // For comppins
   CMneutTestPin(CString refname, double x, double y, int testaccess, NetStruct *net);  // For vias

   EMneutTestPinType GetTestPinType()     { return m_pinType; }
   CString GetRefname()                   { return m_refname; }
   CString GetPinname()                   { return m_pinname; }
   int GetTestAccessCode()                { return m_testAccessCode; }
   NetStruct *GetNet()                    { return m_net; }
   double GetX()                          { return m_x; }
   double GetY()                          { return m_y; }

   void SetTestAttrib(CCEtoODBDoc *doc, CAttributes *attributes);
};

class CMneutTestPinList : public CTypedPtrListContainer<CMneutTestPin*>
{
public:
   void Add(CString refname, CString pinname, int testaccess, NetStruct *net);
   void Add(CString refname, double x, double y, int testaccess, NetStruct *net);
   void MarkTestPinsInCamcad(CCEtoODBDoc *doc, FileStruct *pcbFile);
};

#endif /*__MneutIn_h__*/