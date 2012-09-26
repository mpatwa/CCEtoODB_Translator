// $Header: /CAMCAD/4.5/read_wrt/GenCadIn.h 11    10/10/05 8:00p Rick Faltersack $

/*****************************************************************************/
/*
   this is needed to make a filetype check in CCDOC for make sure, that
   the CAD file is the first gencad file loaded.   
*/

#include "region.h"

int tst_gencadfiletype(FILE *ifp, const char *filename);

/* Define Section ***********************************************************/
#define GENCAD_UNKNOWN        0
#define GENCAD_CAD            1     
#define GENCAD_PANEL          2

#define GCAD_MAX_LINE         255    // input buffer size
#define GCAD_MAX_POLY         10000  // was 2000  max number of pts in polyline
#define GCAD_MAX_ATT          100    // max attributes per component
#define GCAD_MAX_LAYERS       255

#define GEN_UNKNOWN           0
#define GEN_PANEL             1
#define GEN_CAD               2

#define GENCADERR             "gencad.log"
#define TEXT_WIDTHFACTOR      (0.6) 

#define SIZ_OF_COMMAND        (sizeof(command_lst) / sizeof(List))

/* Struct Section ***********************************************************/
struct Plane
{
   DataStruct  *PlaneData;
   Region      *planeRegion;
   BOOL        voidPlane;
};
typedef CTypedPtrList<CPtrList, Plane*> CPlaneList;

typedef struct
{
   double      d;
   int         toolindex;
}GENCADDrill;
typedef CArray<GENCADDrill, GENCADDrill&> CDrillArray;

typedef struct
{
   CString     padname;
   double      drill;
}GENCADPad;
typedef CTypedPtrArray<CPtrArray, GENCADPad*> GENCADPadArray;

typedef struct
{
   CString     compname;
   CString     devicename;
   CString     modified_devicename;
}GENCADComp;
typedef CTypedPtrArray<CPtrArray, GENCADComp*> GENCADCompArray;

typedef struct
{
   CString     shape1;
   CString     shape2;
}GENCADMirrorShape;
typedef CTypedPtrArray<CPtrArray, GENCADMirrorShape*> GENCADMirrorShapeArray;

enum GENCADNetpinType
{
   netpinComponent,
   netpinTestpoint
};

typedef struct
{
   CString     compname;
   CString     pinname;
   CString     netname;
	GENCADNetpinType	netpintype;
}GENCADNetpin;
typedef CTypedPtrArray<CPtrArray, GENCADNetpin*> GENCADNetpinArray;

typedef struct
{
   DataStruct  *testprobe_data;
   double      x, y;
   int         bottom;     // testpin is either top or bottom
   CString     netname;
}GENCADTestpin;
typedef CTypedPtrArray<CPtrArray, GENCADTestpin*> GENCADTestpinArray;

typedef struct
{
   DataStruct  *feature_data;
   double      x;
   double      y;
   CString     netname;
   int         type;           // 0 = testpad, 1 = via
}GENCADFeature;
typedef CTypedPtrArray<CPtrArray, GENCADFeature*> GENCADFeatureArray;

typedef struct
{
   double      x,y,bulge;
   int         fill;    // 0 not, 1 yes
   int         startnew;
} GENCADPoly;

typedef struct
{
   double      x,y,bulge;
   int         fill;    // 0 not, 1 yes
   int         startnew;
} GENCADPolyPnt;
typedef CTypedPtrArray<CPtrArray, GENCADPolyPnt*> GENCADPolyPntArray;

typedef struct
{
   char        *key;
   char        *val;
}Att;

typedef struct
{
   CString     layer_name;
   unsigned    char  color;
   unsigned    char  used;
   int         physnr;     // done in do_phys
   int         electrical; // is this an electrical layer
   DbFlag      flg;
} GENCADLayers;

typedef struct
{
   CString     name;       // layer name
   char        attr;          // layer attributes as defined in dbutil.h
   int         stacknumber;   // electrical stack number
} Adef;

typedef struct
{
   char        *token;
   int         (*function)();
} List;
