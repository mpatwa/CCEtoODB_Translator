// $Header: /CAMCAD/4.4/read_wrt/Dde9_in.h 22    3/21/04 3:41a Kurt Van Ness $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/           

#pragma once

/* Define Section ***********************************************************/
#define MAX_LINE              2000  // yes 2000 is allowed
#define MAX_LAYER             256

#define TEXT_RATIO            0.65

#define STATE_UNKNOWN         0
#define STATE_DESIGN          1
#define STATE_POLYGON         2
#define STATE_MATERIAL        3
#define STATE_LAYER           4
#define STATE_PADSTACK        5
#define STATE_TYPE            6
#define STATE_AREA            7
#define STATE_PIN             8
#define STATE_COMPONENT       9
#define STATE_NETLIST         10

#define LAY_COMPNAME          1
#define LAY_DEVICE            2     // Type - Value
#define LAY_ARTICLE           3     // Partnumber

#define GRAPH_COO             1
#define GRAPH_CIR             2
#define GRAPH_CEN             3
#define GRAPH_NEW             4

#define NOT_PCB_DESIGN        -2    // indicate not a design file for version 9
#define NO_PROPERT_THERMAL    -3    // indicate that property "SaveStackInsts" is not set to "Y" and should stop import

#define SIZ_PCB_LST           (sizeof(pcb_lst) / sizeof(List))

/* Struct Section **********************************************************/

typedef struct
{
   CString  attrib;
   CString  mapAttrib;
}DDEAttribMap;
typedef CTypedPtrArray<CPtrArray, DDEAttribMap*> CAttribMapArray;

typedef struct
{
   int      geomNum;    
   int      index;
   CString  name;
}
DDEPolygon;
typedef CTypedPtrArray<CPtrArray, DDEPolygon*> CDDEPolygonArray;

typedef struct
{
   double   x;
   double   y;
   double   radius;
   BOOL     clockWise;
   BOOL     borderPart;
   int      type; 
}
DDEGraphList;
typedef CArray<DDEGraphList, DDEGraphList&> CDDEGraphArray;

typedef struct
{
   CString  name;
   CString  value;
}
DDEProperty;
typedef CTypedPtrArray<CPtrArray, DDEProperty*> CDDEPropertyArray;

typedef struct
{
   CString  name;
   int      protect;
   double   dielectric;
   double   resistance;
   double   icapacity;

   CDDEPropertyArray property;
   int      propertyCnt;
}
DDEMaterial;
typedef CTypedPtrArray<CPtrArray, DDEMaterial*> CDDEMaterialArray;

typedef struct
{
   int      layerNum;      // layer->getLayerIndex() in CAMCAD
   int      index;         // refnum of the layer in DDE
   int      mirrorIndex;
   CString  name;          // name of the layer in DDE
   int      layerType;     // type of the layer
   COLORREF color;         // color of the layer
   BOOL     bottom;
   int      electrialStackNum;   // eletrical stack number
}
DDELayerList;
typedef CTypedPtrArray<CPtrArray, DDELayerList*> CDDELayerArray;

typedef struct
{
   int			geomNum;
   int			index;
   CString		name;
	DataStruct	*masterDrillData;
	BlockStruct	*masterBlock;
}
DDEPadstackList;
typedef CTypedPtrArray<CPtrArray, DDEPadstackList*> CDDEPadstackArray;

typedef struct
{
   int      geomNum;
   int      index;
   CString  name;
   CString  originalName;
   double   x;
   double   y;
   double   width;
   double   height;
   CDataList *polyDataList;
   BOOL     boardOutline;
}
DDEFootprint;
typedef CTypedPtrArray<CPtrArray, DDEFootprint*> CDDEFootprintArray;

typedef struct
{
   CString  name;
   double   trackWidth;
   double   xOffset;
   double   yOffset;
   BOOL     filled;
   DataStruct *polyData;
}
DDEArea;

typedef struct
{
   int      index;
   CString  name;
   double   x;
   double   y;
   DataStruct *data;
   BOOL     boardOutline;
}
DDEComponent;
typedef CTypedPtrArray<CPtrArray, DDEComponent*> CDDEComponentArray;

typedef struct
{
   char     *token;
   int      (*function)();
}
List;       

struct DDE_Setting
{
   BOOL HatchLineFill;
};

typedef struct
{
   CString  name;       // dde layer name
   int      layerType;        // layer attributes as defined in dbutil.h
}DDELayerAttr;
typedef CTypedPtrArray<CPtrArray, DDELayerAttr*> CDDELayerAttrArray;


/* List Section ***********************************************************/

/* HEADER SECTION */
static int ipl_pcb();
static int ipl_pro();
static int ipl_rem();

/* RULESET SECTION */
static int ipl_rul();

/* POLYGON SECTION */
static int ipl_pol();
static int ipl_cir();
static int ipl_coo();
static int ipl_cen();
static int ipl_poe();

/* MATERIAL SECTION */
static int ipl_maf();
static int ipl_mat();
static int ipl_mae();

/* LAYER SECTION */
static int ipl_lay();
static int ipl_atr();

/* MIRROR MAPPING SECTION */
static int ipl_mir();

/* PADSTACK SECTION */
static int ipl_pst();
static int ipl_ali();
static int ipl_tol();
static int ipl_lpl();
static int ipl_psi();

/* FOOTPRINTS SECTION */
static int ipl_typ();
static int ipl_via();
static int ipl_tra();
static int ipl_arc();
static int ipl_txt();
static int ipl_dfa();
static int ipl_iad();
static int ipl_ena();
static int ipl_dim();       
static int ipl_dob();       
static int ipl_die();
static int ipl_str();
static int ipl_nod();
static int ipl_edg();
static int ipl_ste();
static int ipl_wlp();

/* COMPONENT SECTION */
static int ipl_com();

/* NET SECTION */
static int ipl_wlg();
static int ipl_wle();

/* CLASS SECTION */
static int ipl_lac();
static int ipl_ncl();
static int ipl_ncc();
static int ipl_ccl();
static int ipl_ccc();
static int ipl_mem();

/* SKIP COMMAND */
static int ipl_skip();

/* END */
static int ipl_end();

static List pcb_lst[] =
{
                              /* HEADER SECTION */
   ".pcb",     ipl_pcb,       // PCB file type     
   ".pro",     ipl_pro,       // property       : name value         
   ".rem",     ipl_rem,       // remark            
   
                              /* RULESET SECTION */
   ".rul",     ipl_rul,       // ruleset        : rulesetName

                              /* POLYGON SECTION */
   ".pol",     ipl_pol,       // polygon        : polygonRefnum polygonName [polygonFileName]
   ".cir",     ipl_cir,       // circle         : radius       
   ".coo",     ipl_coo,       // coordinate     : xLoc yLoc
   ".cen",     ipl_cen,       // center         : xLoc yLoc rotation
   ".poe",     ipl_poe,       // section send   

                              /* MATERIAL SECTION */
   ".maf",     ipl_maf,       // material file  : materialFileName   
   ".mat",     ipl_mat,       // material       : materialName protect dielectric resistance icapacity
                                                   // protected = "n"|"n"

   ".mae",     ipl_mae,       // material end

                              /* LAYER SECTION */
   ".lay",     ipl_lay,       // layer          : layerRefnum layerFlags layerName  
                                                   // layerFlags = "n" | "d" | layerKind | layerKind "d" | layerKind "p" | layerKind "dp"    
                                                   // layerKind = 
   ".atr",     ipl_atr,       // attribute      : "color" colorIndex redValue greenValue blueValue 

                              /* MIRROR MAPPING SECTION */
   ".mir",     ipl_mir,       // mirror map     : layerFromRefnum layerToRefnum

                              /* PADSTATCK SECTION */
   ".pst",     ipl_pst,       // padstack       : padstackRefnum drillSize plated/non filled/non stackKind padstackName
   ".ali",     ipl_ali,       // alias          : aliasName
   ".tol",     ipl_tol,       // tolerance      : tolerance
   ".lpl",     ipl_lpl,       // layer polygon  : layerRefnum polygonusage polygonRefnum
   ".psi",     ipl_psi,       // pastack inst   : padstackInstRefnum

                              /* FOOTPRINTS SECTION */
   ".typ",     ipl_typ,       // type           : xLoc yLoc width heigh footprintRefnum footprintName
   ".via",     ipl_via,       // via            : padstackRefnum xLoc yLoc rotation
   ".tra",     ipl_tra,       // track          : trackWidth startX startY endX endY layerRefnum
   ".arc",     ipl_arc,       // arc            : centerX centerY layerRefnum radius startAngle endAngle trackWidth
   ".txt",     ipl_txt,       // text           : xLoc yLoc layerRefnum rotation mirrored option size trackWidth text
   ".dfa",     ipl_dfa,       // def arean      : areaKind layerRefnum trackWidth areaFlag spacing areaName
   ".iad",     ipl_iad,       // installed area border
   ".ena",     ipl_ena,       // end area
   ".dim",     ipl_dim,       // dimension      : dimensionKind spacing modeRotation layerRefnum dimensionName
   ".dob",     ipl_dob,       // dimension obj  : dimensionObjKind xLoc yLoc layerRefnum dimensionObjName
   ".die",     ipl_die,       // dimension end
   ".str",     ipl_str,       // stripline      : layerRefnum netName
   ".nod",     ipl_nod,       // node           : nodeRefnum xloc yLoc
   ".edg",     ipl_edg,       // edge           : edgeRefnum nodeRefnum1 nodeRefnum2 [radius]
   ".ste",     ipl_ste,       // stripline end
   ".wlp",     ipl_wlp,       // pin            : componentRefnum xLoc yLoc rotation pinName padstackRefnum 

                              /* COMPONENT SECTION */
   ".com",     ipl_com,       // component      : componentRefnum xLoc yLoc rotation layerRefnum mirror fixFlag status footprintRefnum componentName

                              /* NET SECTION */
   ".wlg",     ipl_wlg,       // netlist        : netStatus protect/non netName
   ".wle",     ipl_wle,       // netlist end

                              /* CLASS SECTION */
   ".lac",     ipl_lac,       // layerclass              : layerClassNum layerName
   ".ncl",     ipl_ncl,       // net class               : netClassName
   ".ncc",     ipl_ncc,       // net class class         : netClassClassName
   ".ccl",     ipl_ccl,       // component class         : compClassName
   ".ccc",     ipl_ccc,       // component class class   : compClassClassName
   ".mem",     ipl_mem,       // member                  : memberName

   ".end",     ipl_end,       // end

   // The following commands are skipped
   ".cam",     ipl_skip,
   ".ope",     ipl_skip,
   ".enc",     ipl_skip,
   ".enc",     ipl_skip,
   ".fon",     ipl_skip,
};