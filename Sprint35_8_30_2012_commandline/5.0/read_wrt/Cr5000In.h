// $Header: /CAMCAD/4.6/read_wrt/Cr5000In.h 27    9/07/06 12:26p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "TypedContainer.h"

//#define EnableVertexTracking

enum ECr5000FileMode
{
   CR5000_NO_FILE,  // init, unknown, not set, etc
   CR5000_FTF_FILE, // footprint
   CR5000_PCF_FILE, // printed circuit
   CR5000_PNF_FILE  // panel
};

typedef  struct   // can not have CString because of memset.
{
   int      cur_id;
   int      cur_status;
   /* Current values that are changed with global  */
   int      cur_layerindex;               /* Layer index */
   int      cur_linepenshape;             // aperture used, default T_ROUND
   int      cur_widthindex;               /* Line width     */
   double   cur_width;
   double   cur_height;                   /* Line height     */
   double   cur_left, cur_right;
   /* Current Component info used for Instance.    */
   double   cur_pos_x;                    /* position.   */
   double   cur_pos_y;
   double   cur_rotation;                 /* Rotation.   */
   char     cur_mirror;                   /* Current mirror.   */
   /* Current pad stack info.    */
   int      cur_pincnt;                   // counts number of pins on a packaltdef
   /* Pad stacks */
   int      cur_textmirror;
   int      cur_textjust;
   int      cur_textrot;
   int      cur_from_layer;               // electrical layer number
   int      cur_to_layer;                 // electrical layer number
   //
   int      cur_geomcnt;
   //
   char     cur_side;
   char     cur_type;                     // used by layer and padshape
   char     cur_filltype;                 // TRUE = filled. This depends in fillwidth
   char     cur_figtype;
   char     name[80];                     // from cr5000_name routine.
   /* CR5000 layer definition */
   int      numberoflayers;
   /* Tangent Arc (aka bulge in polyline) */
   bool     cur_tarc_on;
   double   cur_tarc_radius;
   /* Building a panel or building a board ? */
   ECr5000FileMode      curInputFileMode;
} Global;

static int fnull(); // this writes to log file
static int fskip(); // fskip does not write to log file

#define  TEXTCORRECT          0.875
#define  TEXTHEIGHT           0.8
#define  LINESPACING          1.5

const double TEXT_CORRECT  = 0.7;

#define  CR5000ERR               "cr5000.log"
#define  CR5000TEC               "cr5000.in"

#define  FNULL                   fnull

#define  MAX_LINE                64000  /* Max line length.  64K text item could be on one line */
#define  MAX_TOKEN               64000 // 64K -- notes and text can be very long

#define  MAX_LAYERS              255   /* Max number of layers.   */
#define  MAX_PACKAGE             1000
#define  MAX_PINPERCOMP          2000

/* Layer codes.      */
#define  LAY_SIGNAL              1

#define  LAY_ALL_LAYERS          -1
#define  LAY_NULL                -2

typedef struct
{
   double   pcf;
   double   ftf;
} CR5000_Version;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
} CR5000Attribmap;

typedef CTypedPtrArray<CPtrArray, CR5000Attribmap*> CAttribmapArray;


typedef struct
{
   CString  name;       // cadif layer name
   char  attr;          // layer attributes as defined in dbutil.h
} CADIFAdef;

enum EAttrSettingTag
{
   attrSettingFiducial,
   attrSettingGeneric,
   attrSettingTooling,
   attrSettingTestPoint
};

class CCR5000AttrSettingEntry : public CObject
{
private:
   CString m_name;
   EAttrSettingTag m_atttype;

public:
   CCR5000AttrSettingEntry(CString nm, EAttrSettingTag ty) { m_name = nm; m_atttype = ty; }
   CString           GetName()   { return m_name;    }
   EAttrSettingTag   GetType()   { return m_atttype; }

};

typedef CTypedObArrayContainer<CCR5000AttrSettingEntry*> CCR5000AttrSettingsArray;

class CCR5000AttrSettings : public CCR5000AttrSettingsArray
{
private:
   bool CheckName(CString *SS, CString *BN);

public:
   bool IsAttrib(CString s, EAttrSettingTag atttype);
   void UpdateAttr(CString lp, EAttrSettingTag typ);
   BlockTypeTag GetBlockType(CString name, BlockTypeTag defaultBlockType);
   InsertTypeTag GetInsertType(CString name, InsertTypeTag defaultInsertType);
   InsertTypeTag GetInsertType(BlockStruct *b, InsertTypeTag defaultInsertType);
   void Dump(FILE *fp);
};

class CCr5000Pt : public Point2
{
   // The purpose of this class is to be able to capture the "raw" Cr5000 point
   // information, particular when TARC is involved, while still being able to handle
   // the item as a standard CPoint (used through this exporter) when TARC is not
   // an issue. Handling of TARC requires a post processing step, on-the-fly does not
   // work becasue if the TARC is the first item in a polyline, we need the last (closing)
   // segment of the line in order to calculate the first point in CAMCAD terms.

   // When processing TARC, new pts are determines. If a line is adjacent to a tarc, the line
   // will require a new endpoint (where the arc is tangent to that line). If a pt IS a tarc,
   // then new (additional) points may be required becasue the pt given in the data is
   // an endpoint on the line that the tarc is tangent to, and not a point anywhere on
   // the arc itself. If two tarcs are adjacent, we likely have to had a line segment (more
   // new points) between the two arcs, as the arcs themselves likely do not actually
   // touch each other (e.g. a rectangle defined with 4 TARC endpoints actually needs
   // 4 arcs and 4 lines in camcad).

public:
   double cr5_x;
   double cr5_y;
   double width;
   bool isTarc;
   double tarcRadius;
   Point2 tarcPt1;
   Point2 tarcPt2;

   CCr5000Pt() { x = cr5_x = 0.0; y = cr5_y = 0.0; width = 0.0; isTarc = false; tarcRadius = 0.0; }

};

class CLayerMapEntry : public CObject
{
public:
   CString m_footlayerName;
   CStringArray m_boardlayerNames;
};

typedef CTypedObArrayWithMapContainer<CLayerMapEntry> CLayerMapArray;

class CLayerMap : public CLayerMapArray
{
private:
   CString m_mapName;
   bool m_isSMD;
   bool m_isBottom;

public:
   CLayerMap(CString mapname, bool isSMD, bool isBottom) { m_mapName = mapname; m_isSMD = isSMD; m_isBottom = isBottom; }
   CString getMapName()       { return m_mapName; }
   bool getIsSMD()            { return m_isSMD; }
   bool getIsBottom()         { return m_isBottom; }
};

typedef struct
{
   CString  lay1;       // 
   CString  lay2;
} CR5000Mirror;

typedef struct
{
   double d;
   int    toolindex;
} CR5000Drill;

typedef CArray<CR5000Drill, CR5000Drill&> CDrillArray;

typedef struct
{
   CString  reference;
   int      gateCount;
   int      pinCount;
   CString  part;
   CString  package;
   CString  footprint;
   CString  reversefootprint;
   CString  originalReference;
   int      placed;
   int      placementSide;    // 0 = top, 1 = bottom
   int      locationLock;
   int      angleLock;
   int      placementSideLock;
   int      packageSymbol;
   int      outOfBoard;
   Point2   outOfBoardLocation; // this is not evaluated. not placed components are placed at 0 0
   Point2   location;
   double   angle;
   int      drawRefDes;
} CR5000CompInst;

typedef CTypedPtrArray<CPtrArray, CR5000CompInst*> CCompInstArray;

typedef struct
{
   CString  footprintLayerName;
   CString  footprintLayerType;
   CString  boardLayerName;
   CString  boardLayerType;
   CString  comment;
   bool     conductive;
   int      conductiveLayerNumber;
} CR5000LayerDef;

typedef CTypedPtrArray<CPtrArray, CR5000LayerDef*> CCR5000LayerDefArray;

typedef struct
{
   CString  pinName;
   double   x,y;
   CString  padstackGeomName;
   double   padstackx, padstacky;
   double   padrotation;
} CR5000PinInst;

typedef struct
{
   CString  string;
   int      font;
   int      kFont;
   double   width;
   double   height;
   double   space;
   double   vSpace;
   double   strokeWidth;
   double   angle;
   double   x;
   double   y;
   int      dir;
   CString  justify; 
   int      flip;
   int      reverse;
   int      layerIndex;
} CR5000TextInst;

typedef struct
{  
   int      ccw;     // 
   double   radius;
   double   centerx, centery;
} CR5000ArcInst;

typedef struct
{  
   double   out, in;
   int      nbridge;
   double   bridgewidth;
   double   bridgeangle;
} CR5000ApertureInst;

#ifdef EnableVertexTracking
//_____________________________________________________________________________
enum VertexEntryTag
{
   vertexEntryPt,
   vertexEntryArc
};

class CVertexEntry
{
private:
   static int m_ptDepth;
   static int m_arcDepth;
   static int m_ptCnt;
   static int m_arcCnt;
   static int m_expectedArcPts;
   static bool m_mixedFlag;

   VertexEntryTag m_type;

public:
   CVertexEntry(VertexEntryTag type);
   ~CVertexEntry();

   static void reset();
};
#endif

//_____________________________________________________________________________
class CCr5000LineNumberStamp
{
private:
   static int m_sourceLineNumberKeywordIndex;

   int m_lineNumber;

public:
   CCr5000LineNumberStamp();

   DataStruct* graphPolyStruct(int layer, DbFlag flg, BOOL negative);
   DataStruct* graphBlockReference(const char *block_name, const char *refname, int filenum, double x, double y,
         double angle, int mirror, double scale, int layer, int global, BlockTypeTag blockType = blockTypeUndefined);

   void stamp(DataStruct* data);

   static void resetKeywordIndices();
};

//_____________________________________________________________________________

typedef struct
{
   char *token;
   int (*function)();
} List;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* Bracket definitions.       */
static List brk_lst[] =
{
   "(", FNULL,
   ")", FNULL,
};

#define  SIZ_BRK_LST       (sizeof(brk_lst) / sizeof(List))
#define  BRK_ROUND         0
#define  BRK_B_ROUND       1

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* Start of CR5000 files.  */
static int      cr5000_unit();

static int      start_ftf();
static int      start_pcf();
static int      start_pnf();

static List start_lst[] =
{
   "ftf", start_ftf,
   "pcf", start_pcf,
   "pnf", start_pnf,
};

#define SIZ_START_LST (sizeof(start_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* FTF / PCF sections.      */
static   int      pcf_header();
static   int      ftf_header();
static   int      ftf_technologyContainer();
static   int      ftf_footprintContainer();

static List  ftf_lst[] =
{
   "header",               ftf_header,
   "technologyContainer",  ftf_technologyContainer,
   "footprintContainer",   ftf_footprintContainer,
};

#define  SIZ_FTF_LST       (sizeof(ftf_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int      pcf_technologyContainer();
static   int      pcf_boardContainer();

static List  pcf_lst[] =
{
   "header",               pcf_header,
   "technologyContainer",  pcf_technologyContainer,
   "boardContainer",       pcf_boardContainer,
   "libraryEntities",      fskip,
   "parameter",            fskip,
};

#define  SIZ_PCF_LST       (sizeof(pcf_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int      pnf_boardContainer();

static List  pnf_lst[] =
{
   "header",               pcf_header, // pnf and pcf headers are the same as of this writing
   "technologyContainer",  fskip,   // we got all we need from technology container from pcf file, expect this tech conents to be the same, would be a problem if it were not, e.g. used entirely different layer mappings
   "boardContainer",       pnf_boardContainer,
   "libraryEntities",      fskip,
   "parameter",            fskip,
};

#define  SIZ_PNF_LST       (sizeof(pnf_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfheader_version();

static List  pcfheader_lst[] =
{
   "version",              pcfheader_version,
   "timeZone",             fskip,
   "unit",                 cr5000_unit,
};

#define  SIZ_PCFHEADER_LST      (sizeof(pcfheader_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcf_technology();

static List  pcftechnologyContainer_lst[] =
{
   "technology",           pcf_technology,
};

#define  SIZ_PCFTECHNOLOGYCONTAINER_LST   (sizeof(pcftechnologyContainer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List  ftftechnologyContainer_lst[] =
{
   "technology",           fskip,
};

#define  SIZ_FTFTECHNOLOGYCONTAINER_LST   (sizeof(ftftechnologyContainer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfboard_nets();
static   int   pcfboard_components();
static   int   pcfboard_boardlayout();

static List  pcfboardContainer_lst[] =
{
   "components",           pcfboard_components,
   "boardLayout",          pcfboard_boardlayout,
   "nets",                 pcfboard_nets,
   "outComponentValid",    fskip,
};

#define  SIZ_PCFBOARDCONTAINER_LST  (sizeof(pcfboardContainer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///*rcf static   int   pcfboard_nets();
///*rcf static   int   pcfboard_components();
static   int   pnfboard_boardlayout();

static List  pnfboardContainer_lst[] =
{
   "components",           pcfboard_components,
   "boardLayout",          pnfboard_boardlayout,
};

#define  SIZ_PNFBOARDCONTAINER_LST  (sizeof(pnfboardContainer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftf_footprints();
static   int   ftf_pads();
static   int   ftf_padstacks();

static List  ftffootprintContainer_lst[] =
{
   "commonParameters",     fskip,
   "padstackGroups",       fskip,
   "footprints",           ftf_footprints,
   "pads",                 ftf_pads,
   "padstacks",            ftf_padstacks,
};

#define  SIZ_FTFFOOTPRINTCONTAINER_LST (sizeof(ftffootprintContainer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfboardlayout_layout();

static List  pcfboard_boardlayout_lst[] =
{
   "layout",               pcfboardlayout_layout,
};

#define  SIZ_PCFBOARD_BOARDLAYOUT_LST  (sizeof(pcfboard_boardlayout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pnfboardlayout_layout();

static List  pnfboard_boardlayout_lst[] =
{
   "layout",               pnfboardlayout_layout,
};

#define  SIZ_PNFBOARD_BOARDLAYOUT_LST  (sizeof(pnfboard_boardlayout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfcomponents_component();

static List  pcfboard_components_lst[] =
{
   "component",            pcfcomponents_component,
};

#define  SIZ_PCFBOARD_COMPONENTS_LST   (sizeof(pcfboard_components_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfnets_net();

static List  pcfboard_nets_lst[] =
{
   "net",                  pcfnets_net,
};

#define  SIZ_PCFBOARD_NETS_LST   (sizeof(pcfboard_nets_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfheelprint_layer();

static List  ftfheelprintlayout_lst[] =
{
   "layer",             ftfheelprint_layer,
};

#define  SIZ_FTFHEELPRINTLAYOUT_LST (sizeof(ftfheelprintlayout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfheelprint_layout();

static List  ftfheelprint_lst[] =
{
   "minrect",           fskip,
   "layout",            ftfheelprint_layout,
};

#define  SIZ_FTFHEELPRINT_LST (sizeof(ftfheelprint_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int ftftoeprint_pin();

static List  ftftoeprint_lst[] =
{
   "pin",               ftftoeprint_pin,
};

#define  SIZ_FTFTOEPRINT_LST  (sizeof(ftftoeprint_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pin_pt();
static   int   ftfpin_layout();

static List  ftftoeprintpin_lst[] =
{
   "minrect",           fskip,
   "pt",                pin_pt,
   "layout",            ftfpin_layout,
};

#define  SIZ_FTFTOEPRINTPIN_LST  (sizeof(ftftoeprintpin_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcflayout_layer();

static List  pcfboardlayout_lst[] =
{
   "layer",             pcflayout_layer,
};

#define  SIZ_PCFBOARDLAYOUT_LST  (sizeof(pcfboardlayout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pnfboardlayout_layer();

static List  pnfboardlayout_lst[] =
{
   "layer",             pnfboardlayout_layer,
};

#define  SIZ_PNFBOARDLAYOUT_LST  (sizeof(pnfboardlayout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pnflayer_boardassembly_subboard();
static   int   pnflayer_boardassembly_angle();
static   int   pnflayer_boardassembly_pt();

static List  pnflayer_boardassembly_lst[] =
{
   "subBoard",    pnflayer_boardassembly_subboard,
   "geometry",    fskip,
   "angle",       pnflayer_boardassembly_angle,
   "pt",          pnflayer_boardassembly_pt,
};

#define  SIZ_PNFLAYERBOARDASSEMBLY_LST  (sizeof(pnflayer_boardassembly_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfpinlayout_layer();

static List  ftfpinlayout_lst[] =
{
   "layer",             ftfpinlayout_layer,
};

#define  SIZ_FTFPINLAYOUT_LST (sizeof(ftfpinlayout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphic_line();
static   int   graphic_layer_surface();
static   int   graphic_text();
static   int   graphic_area();
static   int   layer_conductive();
static   int   layer_nonconductive();
static   int   layer_footlayer();
static   int   layer_systemlayer();
static   int   layer_drawof();
static   int   layer_infoof();
static   int   pcf_padstack();
static   int   pcf_pad();
static   int   pcf_hole();
static   int   pnflayer_boardassembly();
static   int   layer_fpad();

static List  cr5000layer_lst[] =
{
   "nonConductive",     layer_nonconductive,
   "conductive",        layer_conductive,
   "footLayer",         layer_footlayer,
   "systemLayer",       layer_systemlayer,
   "drawof",            layer_drawof,
   "infoof",            layer_infoof,
   "line",              graphic_line,
   "surface",           graphic_layer_surface,
   "text",              graphic_text,
   "area",              graphic_area,
   "padstack",          pcf_padstack,
   "hole",              pcf_hole,
   "boardAssembly",     pnflayer_boardassembly,
   "info",              fskip,
   "message",           fskip,
   "fpadstack",         fskip,
   "fpad",              layer_fpad,
   //"dimension",       fskip,
};

#define  SIZ_CR5000LAYER_LST  (sizeof(cr5000layer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfcomponent_reference();
static   int   pcfcomponent_gatecount();
static   int   pcfcomponent_pincount();
static   int   pcfcomponent_part();
static   int   pcfcomponent_package();
static   int   pcfcomponent_footprint();
static   int   pcfcomponent_reversefootprint();
static   int   pcfcomponent_originalreference();
static   int   pcfcomponent_placed();
static   int   pcfcomponent_placementside();
static   int   pcfcomponent_locationlock();
static   int   pcfcomponent_anglelock();
static   int   pcfcomponent_placementsidelock();
static   int   pcfcomponent_packagesymbol();
static   int   pcfcomponent_outofboard();
static   int   pcfcomponent_outofboardlocation();
static   int   pcfcomponent_location();
static   int   pcfcomponent_angle();
static   int   pcfcomponent_drawrefdes();
static   int   pcfcomponent_layout();

static List  pcfcomponent_lst[] =
{
   "reference",         pcfcomponent_reference,
   "gateCount",         pcfcomponent_gatecount,
   "pinCount",          pcfcomponent_pincount,
   "part",              pcfcomponent_part,
   "package",           pcfcomponent_package,
   "footprint",         pcfcomponent_footprint,
   "reverseFootprint",  pcfcomponent_reversefootprint,
   "originalReference", pcfcomponent_originalreference,
   "placed",            pcfcomponent_placed,
   "placementSide",     pcfcomponent_placementside,
   "locationLock",      pcfcomponent_locationlock,
   "angleLock",         pcfcomponent_anglelock,
   "placementSideLock", pcfcomponent_placementsidelock,
   "packageSymbol",     pcfcomponent_packagesymbol,
   "outOfBoard",        pcfcomponent_outofboard,
   "outOfBoardLocation",fskip, //pcfcomponent_outofboardlocation,
   "Location",          pcfcomponent_location,
   "minRect",           fskip,
   "angle",             pcfcomponent_angle,
   "drawRefDes",        pcfcomponent_drawrefdes,
   "layout",            pcfcomponent_layout,
   "pin",               fskip,
   "gate",              fskip,
   "footprintSpec",     fskip,
   "stockId",           fskip,
   "propertyS",         fskip,
   "propertyI",         fskip,
};

#define  SIZ_PCFCOMPONENT_LST    (sizeof(pcfcomponent_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfcomponent_layout_layer();

static List  pcfcomponent_layout_lst[] =
{
   "layer",               pcfcomponent_layout_layer,
};

#define  SIZ_PCFCOMPONENTLAYOUT_LST      (sizeof(pcfcomponent_layout_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfcomponent_layout_layer_symboltext();
static   int   pcfcomponent_layout_layer_refer();
static   int   pcfcomponent_layout_layer_padstack();

static List  pcfcomponent_layout_layer_lst[] =
{
   "nonConductive",     layer_nonconductive,///*rcf pcfcomponent_layout_layer_nonconductive,
   "conductive",        fskip,
   "footLayer",         fskip,
   "systemLayer",       fskip,
   "padstack",          pcfcomponent_layout_layer_padstack,
   "refer",             pcfcomponent_layout_layer_refer,
   "symbolText",        pcfcomponent_layout_layer_symboltext,
};

#define  SIZ_PCFCOMPONENTLAYOUTLAYER_LST      (sizeof(pcfcomponent_layout_layer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfcomponent_layout_layer_symboltext_geometry();
static   int   pcfcomponent_layout_layer_symboltext_type();

static List  pcfcomponent_layout_layer_symboltext_lst[] =
{
   "type",              pcfcomponent_layout_layer_symboltext_type,
   "font",              fskip,
   "geometry",          pcfcomponent_layout_layer_symboltext_geometry,
};

#define  SIZ_PCFCOMPONENTLAYOUTLAYERSYMBOLTEXT_LST      (sizeof(pcfcomponent_layout_layer_symboltext_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfcomponent_layout_layer_refer_area();

static List  pcfcomponent_layout_layer_refer_lst[] =
{
   "area",           pcfcomponent_layout_layer_refer_area,
};

#define  SIZ_PCFCOMPONENTLAYOUTLAYERREFER_LST      (sizeof(pcfcomponent_layout_layer_refer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfcomponent_layout_layer_refer_area_geometry();

static List  pcfcomponent_layout_layer_refer_area_lst[] =
{
   "geometry",          pcfcomponent_layout_layer_refer_area_geometry,
};

#define  SIZ_PCFCOMPONENTLAYOUTLAYERREFERAREA_LST      (sizeof(pcfcomponent_layout_layer_refer_area_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pfc_comppin();

static List  pcfnetcomp_lst[] =
{
   "pin",               pfc_comppin,
};

#define  SIZ_PCFNETCOMP_LST      (sizeof(pcfnetcomp_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfnet_comp();
static   int   pcfnet_type();

static List  pcfnet_lst[] =
{
   "comp",              pcfnet_comp,
   "type",              pcfnet_type,
   "lockMode",          fskip,
   "pen",               fskip,
   "drawRatsNest",      fskip,
   "rebuildRatsNest",   fskip,
};

#define  SIZ_PCFNET_LST          (sizeof(pcfnet_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_numberofconductorlayer();
static   int   pcftechnology_layermapping();
static   int   pcftechnology_footprintlayer();
static   int   pcftechnology_nonconductivelayer();
static   int   pcftechnology_conductivelayer();

static List  pcftechnology_lst[] =
{
   "numberOfConductorLayer",  pcftechnology_numberofconductorlayer,
   "layerMapping",            pcftechnology_layermapping,
   "footprintLayer",          pcftechnology_footprintlayer,
   "nonConductiveLayer",      pcftechnology_nonconductivelayer,
   "conductiveLayer",         pcftechnology_conductivelayer,
   "subLayer",                fskip,
   "padstackGroup",           fskip,   
};

#define  SIZ_PCFTECHNOLOGY_LST   (sizeof(pcftechnology_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_layermapping_map();

static List  pcftechnology_layermapping_lst[] =
{
   "map",  pcftechnology_layermapping_map,   
};

#define  SIZ_PCFTECHNOLOGY_LAYERMAPPING_LST   (sizeof(pcftechnology_layermapping_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_layermapping_map_correspondence();

static List  pcftechnology_layermapping_map_lst[] =
{
   "correspondence",  pcftechnology_layermapping_map_correspondence,
};

#define  SIZ_PCFTECHNOLOGY_LAYERMAPPING_MAP_LST   (sizeof(pcftechnology_layermapping_map_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_layermapping_map_corr_footlayer();
static   int   pcftechnology_layermapping_map_corr_boardlayer();

static List  pcftechnology_layermapping_map_corr_lst[] =
{
   "footLayer",   pcftechnology_layermapping_map_corr_footlayer,
   "boardLayer",  pcftechnology_layermapping_map_corr_boardlayer,
};

#define  SIZ_PCFTECHNOLOGY_LAYERMAPPING_MAP_CORR_LST   (sizeof(pcftechnology_layermapping_map_corr_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_layermapping_map_corr_boardlayer_conductive();
static   int   pcftechnology_layermapping_map_corr_boardlayer_nonconductive();

static List  pcftechnology_layermapping_map_corr_boardlayer_lst[] =
{
   "conductive",  pcftechnology_layermapping_map_corr_boardlayer_conductive,
   "nonConductive",  pcftechnology_layermapping_map_corr_boardlayer_nonconductive,
};

#define  SIZ_PCFTECHNOLOGY_LAYERMAPPING_MAP_CORR_BOARDLAYER_LST   (sizeof(pcftechnology_layermapping_map_corr_boardlayer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_footprintlayer_layer();

static List  pcftechnology_footprintlayer_lst[] =
{
   "layer",  pcftechnology_footprintlayer_layer,   
};

#define  SIZ_PCFTECHNOLOGY_FOOTPRINTLAYER_LST   (sizeof(pcftechnology_footprintlayer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_nonconductivelayer_layer();

static List  pcftechnology_nonconductivelayer_lst[] =
{
   "layer",  pcftechnology_nonconductivelayer_layer,   
};

#define  SIZ_PCFTECHNOLOGY_NONCONDUCTIVELAYER_LST   (sizeof(pcftechnology_nonconductivelayer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_conductivelayer_layernumber();

static List  pcftechnology_conductivelayer_lst[] =
{
   "layerNumber",  pcftechnology_conductivelayer_layernumber,   
};

#define  SIZ_PCFTECHNOLOGY_CONDUCTIVELAYER_LST   (sizeof(pcftechnology_conductivelayer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_conductivelayer_layernumber_reefer();

static List  pcftechnology_conductivelayer_layernumber_lst[] =
{
   "type",        fskip,
   "soldering",   fskip,
   "refer",       pcftechnology_conductivelayer_layernumber_reefer,   
};

#define  SIZ_PCFTECHNOLOGY_CONDUCTIVELAYER_LAYERNUMBER_LST   (sizeof(pcftechnology_conductivelayer_layernumber_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_conductivelayer_layernumber_ref_type();

static List  pcftechnology_conductivelayer_layernumber_ref_lst[] =
{
   "type",        pcftechnology_conductivelayer_layernumber_ref_type,
   "userDefType", fskip,   
};

#define  SIZ_PCFTECHNOLOGY_CONDUCTIVELAYER_LAYERNUMBER_REF_LST   (sizeof(pcftechnology_conductivelayer_layernumber_ref_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcftechnology_layer_type();
static   int   pcftechnology_layer_propertyS();

static List  pcftechnology_layer_lst[] =
{
   "type",        pcftechnology_layer_type,
   "propertyS",   pcftechnology_layer_propertyS,
};

#define  SIZ_PCFTECHNOLOGY_LAYER_LST   (sizeof(pcftechnology_layer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftf_footprint();

static List  ftffootprints_lst[] =
{
   "footprint",         ftf_footprint,    //  
};

#define  SIZ_FTFFOOTPRINTS_LST      (sizeof(ftffootprints_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int ftf_pad(); 

static List  ftfpads_lst[] =
{
   "pad",               ftf_pad,
};

#define  SIZ_FTFPADS_LST            (sizeof(ftfpads_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int ftf_padstack();  

static List  ftfpadstacks_lst[] =
{
   "padstack",          ftf_padstack,     //  
};

#define  SIZ_FTFPADSTACKS_LST       (sizeof(ftfpadstacks_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int ftfpadset_footlayer();
static   int ftfpadset_connect();
static   int ftfpadset_clearance();
static   int ftfpadset_thermal();

static List  ftfpadset_lst[] =
{
   "footLayer",         ftfpadset_footlayer,
   "connect",           ftfpadset_connect,
   "clearance",         ftfpadset_clearance,
   "thermal",           ftfpadset_thermal,
   "noconnect",         fskip,
};

#define  SIZ_FTFPADSET_LST       (sizeof(ftfpadset_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfhole_geometry();

static List  ftfhole_lst[] =
{
   "footLayer",         ftfpadset_footlayer,
   "geometry",          ftfhole_geometry,    //  
   "holetype",          fskip,
   "fromTo",            fskip,
   "drillType",         fskip,
};

#define  SIZ_FTFHOLE_LST         (sizeof(ftfhole_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfpadset_pad_connect();

static List  ftfpadset_connect_lst[] =
{
   "pad",               ftfpadset_pad_connect,
};

#define  SIZ_FTFPADSET_CONNECT_LST (sizeof(ftfpadset_connect_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfpadset_pad_clearance();

static List  ftfpadset_clearance_lst[] =
{
   "pad",               ftfpadset_pad_clearance,
};

#define  SIZ_FTFPADSET_CLEARANCE_LST (sizeof(ftfpadset_clearance_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfpadset_pad_thermal();

static List  ftfpadset_thermal_lst[] =
{
   "pad",               ftfpadset_pad_thermal,
};

#define  SIZ_FTFPADSET_THERMAL_LST (sizeof(ftfpadset_thermal_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftffootprint_heelprint();
static   int   ftffootprint_toeprint();

static List  ftffootprint_lst[] =
{
   "uver",              fskip,
   "area",              fskip,
   "grid",              fskip,
   "polarity",          fskip,
   "minrect",           fskip,
   "panelUse",          fskip,
   "heelprint",         ftffootprint_heelprint,    //  
   "toeprint",          ftffootprint_toeprint,     //  
   "propertyI",         fskip,
   "propertyS",         fskip,
};

#define  SIZ_FTFFOOTPRINT_LST    (sizeof(ftffootprint_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfpad_geometry();

static List  ftfpad_lst[] =
{
   "uver",              fskip,
   "panelUse",          fskip,
   "photo",             fskip,
   "grid",              fskip,
   "area",              fskip,
   "propertyS",         fskip,
   "geometry",          ftfpad_geometry,     //  
};

#define  SIZ_FTFPAD_LST          (sizeof(ftfpad_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfpadstack_hole();
static   int   ftfpadstack_padset();
static   int   ftfpadstack_throughmode();

static List  ftfpadstack_lst[] =
{
   "uver",              fskip,
   "area",              fskip,
   "panelUse",          fskip,
   "type",              fskip,
   "propertyS",         fskip,
   "throughMode",       ftfpadstack_throughmode,         
   "padset",            ftfpadstack_padset,     //  
   "hole",              ftfpadstack_hole,    //  
};

#define  SIZ_FTFPADSTACK_LST     (sizeof(ftfpadstack_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfpadstackgroup_padstack();

static List  ftfpadstackgroup_lst[] =
{
   "padstack",          ftfpadstackgroup_padstack,    //  
};

#define  SIZ_FTFPADSTACKGROUP_LST      (sizeof(ftfpadstackgroup_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftf_fpadstack();
static   int   ftf_fpad();

static List  ftfpinlayer_lst[] =
{
   "fpadstack",         ftf_fpadstack,
   "fpad",              ftf_fpad,
   "footLayer",         layer_conductive,
};

#define  SIZ_FTFPINLAYER_LST  (sizeof(ftfpinlayer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   cr5000_pt();
static   int   cr5000_angle();
static   int   ftf_padstackgroup();

static List  ftffpadstack_lst[] =
{
   "pt",                cr5000_pt,
   "angle",             cr5000_angle,
   "padstackgroup",     ftf_padstackgroup,
};

#define  SIZ_FTFFPADSTACK_LST (sizeof(ftffpadstack_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List  ftffpad_lst[] =
{
   "pt",                cr5000_pt,
   "angle",             cr5000_angle,
};

#define  SIZ_FTFFPAD_LST   (sizeof(ftffpad_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphicline_geometry();
static   int   cr5000_net();

static List  graphicline_lst[] =
{
   "net",                  cr5000_net,
   "msgTag",               fskip,
   "geometry",             graphicline_geometry,
   "thermalData",          fskip,
   "ignoreRebuildNet",     fskip,
   "propertyS",            fskip,
   "propertyI",            fskip,
};

#define  SIZ_GRAPHICLINE_LST  (sizeof(graphicline_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   layer_surface_geometry();

static List  GRAPHIC_layer_surface_lst[] =
{
   "net",                  cr5000_net,
   "msgTag",               fskip,
   "geometry",             layer_surface_geometry,
   "propertyS",            fskip,
   "propertyI",            fskip,
};

#define  SIZ_GRAPHICLAYERSURFACE_LST  (sizeof(GRAPHIC_layer_surface_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   pcfpadstack_istestpad();
static   int   pcfpadstack_fromto();
static   int   pcfpadstack_propertys();
static   int   pcfpadstack_side();
static   int   pcfpadstack_flip();

static List pcfpadstack_lst[] =
{
   "fromTo",            pcfpadstack_fromto,
   "msgTag",            fskip,
   "deleteLock",        fskip,
   "moveLock",          fskip,
   "conductive",        fskip,
   "isBuildupVia",      fskip,
   "net",               cr5000_net, 
   "side",              pcfpadstack_side,
   "flip",              pcfpadstack_flip,
   "pt",                cr5000_pt,
   "propertyS",         pcfpadstack_propertys,
   "isTestPad",         pcfpadstack_istestpad,
};

#define  SIZ_PCFPADSTACK_LST        (sizeof(pcfpadstack_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List pcfpad_lst[] =
{
   "net",               cr5000_net, 
   "pt",                pin_pt,
};

#define  SIZ_PCFPADSTACK_LST        (sizeof(pcfpadstack_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   istestpad_side();
static   int   istestpad_id();

static List pcfpadstack_istestpad_lst[] =
{
   "side",              istestpad_side,
   "id",                istestpad_id,
};

#define  SIZ_PCFPADSTACK_ISTESTPAD_LST (sizeof(pcfpadstack_istestpad_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphicarea_geometry();

static List  graphicarea_lst[] =
{
   "upperHeight",          fskip,
   "lowerHeight",          fskip,
   "geometry",             graphicarea_geometry,
};

#define  SIZ_GRAPHICAREA_LST  (sizeof(graphicarea_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphictext_geometry();

static List  graphictext_lst[] =
{
   "msgTag",               fskip,
   "font",                 fskip,
   "geometry",             graphictext_geometry,
};

#define  SIZ_GRAPHICTEXT_LST  (sizeof(graphictext_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   linegeometry_line();

static List  linegeometry_lst[] =
{
   "line",              linegeometry_line,
};

#define  SIZ_LINEGEOMETRY_LST (sizeof(linegeometry_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   layersurfacegeometry_surface();

static List  layersurfacegeometry_lst[] =
{
   "surface",           layersurfacegeometry_surface,
};

#define  SIZ_LAYERSURFACEGEOMETRY_LST (sizeof(layersurfacegeometry_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   padgeometry_circle();
static   int   padgeometry_rectangle();
static   int   padgeometry_roundthermal();
static   int   padgeometry_donut();
static   int   padgeometry_oblong();
static   int   padgeometry_line();
static   int   padgeometry_surface();

static List  padgeometry_lst[] =
{
   "circle",            padgeometry_circle,
   "rectangle",         padgeometry_rectangle,
   "roundThermal",      padgeometry_roundthermal,
   "donut",             padgeometry_donut,
   "oblong",            padgeometry_oblong,
   "line",              padgeometry_line,
   "surface",           padgeometry_surface,
};

#define  SIZ_PADGEOMETRY_LST  (sizeof(padgeometry_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   holegeometry_circle();

static List  holegeometry_lst[] =
{
   "circle",            holegeometry_circle,
};

#define  SIZ_HOLEGEOMETRY_LST (sizeof(holegeometry_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   areageometry_surface();

static List  areageometry_lst[] =
{
   "surface",           areageometry_surface,
};

#define  SIZ_AREAGEOMETRY_LST (sizeof(areageometry_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   textgeometry_text();

static List  textgeometry_lst[] =
{
   "text",           textgeometry_text,
};

#define  SIZ_TEXTGEOMETRY_LST (sizeof(textgeometry_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   systemlayer_type();

static List  systemlayer_lst[] =
{
   "type",           systemlayer_type,
   "nonconductive",  layer_nonconductive,
   "conductive",     layer_conductive,
};

#define  SIZ_SYSTEMLAYER_LST  (sizeof(systemlayer_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   cr5000text_string();
static   int   cr5000text_font();
static   int   cr5000text_kfont();
static   int   cr5000text_space();
static   int   cr5000text_vspace();
static   int   cr5000text_width();
static   int   cr5000text_height();
static   int   cr5000text_strokewidth();
static   int   cr5000text_pt();
static   int   cr5000text_angle();
static   int   cr5000text_dir();
static   int   cr5000text_justify();
static   int   cr5000text_flip();
static   int   cr5000text_reverse();

static   int   cr5000_width();
static   int   cr5000_height();

static List  texttext_lst[] =
{
   "string",            cr5000text_string,
   "font",              cr5000text_font,
   "kFont",             cr5000text_kfont,
   "width",             cr5000text_width,
   "height",            cr5000text_height,
   "space",             cr5000text_space,
   "vSpace",            cr5000text_vspace,
   "strokeWidth",       cr5000text_strokewidth,
   "pt",                cr5000text_pt,
   "textAngle",         cr5000text_angle,
   "dir",               cr5000text_dir,
   "justify",           cr5000text_justify,
   "flip",              cr5000text_flip,
   "reverse",           cr5000text_reverse,
};

#define  SIZ_TEXTTEXT_LST  (sizeof(texttext_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphline_vertex();
static   int   graphline_penshape();

static List  linegeometryline_lst[] =
{
   "penshape",          graphline_penshape,
   "vertex",            graphline_vertex,
   "type",              fskip,
   "dashLine",          fskip,
};

#define  SIZ_LINEGEOMETRYLINE_LST   (sizeof(linegeometryline_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphsurface_vertex();
static   int   graphsurface_openshape();
static   int   graphsurface_fillwidth();

static List  surfacegeometrysurface_lst[] =
{
   "vertex",            graphsurface_vertex,
   "rAngle",            fskip,
   "fillAngle",         fskip,
   "alreadySpread",     fskip,
   "outlineWidth",      fskip,
   "fillWidth",         graphsurface_fillwidth,
   "openShape",         graphsurface_openshape,
};

#define  SIZ_SURFACEGEOMETRYSURFACE_LST   (sizeof(surfacegeometrysurface_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   graphopenshape_vertex();

static List  surfacegeometryopenshape_lst[] =
{
   "vertex",            graphopenshape_vertex,
   "outlineWidth",      fskip,
};

#define  SIZ_SURFACEGEOMETRYOPENSHAPE_LST (sizeof(surfacegeometryopenshape_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   grapharea_vertex();
static   int   grapharea_fillwidth();

static List  areageometrysurface_lst[] =
{
   "vertex",            grapharea_vertex,
   "rAngle",            fskip,
   "fillAngle",         fskip,
   "outlineWidth",      fskip,
   "fillWidth",         grapharea_fillwidth,
};

#define  SIZ_AREAGEOMETRYSURFACE_LST   (sizeof(areageometrysurface_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   ftfheader_version();

static List  ftfheader_lst[] =
{
   "version",              ftfheader_version,
   "timeZone",             fskip,
   "unit",                 cr5000_unit,
};

#define  SIZ_FTFHEADER_LST      (sizeof(ftfheader_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   cr5000_arc();

static List  pt_lst[] =
{
   "pt",                   cr5000_pt,
   "arc",                  cr5000_arc,
};

#define  SIZ_PT_LST              (sizeof(pt_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int     cr5000_pt_width();
static int     pt_tarc();

static List  ptparam_lst[] =
{
   "width",                   cr5000_pt_width,
   "tarc",                    pt_tarc,
};

#define  SIZ_PTPARAM_LST           (sizeof(ptparam_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int     tarc_radius();

static List  tarc_lst[] =
{
   "r",                    tarc_radius,
};

#define  SIZ_TARC_LST           (sizeof(tarc_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   cr5000_arc_radius();
static   int   cr5000_arc_begin();
static   int   cr5000_arc_center();
static   int   cr5000_arc_end();

static List  arc_lst[] =
{
   "r",                       cr5000_arc_radius,
   "begin",                   cr5000_arc_begin,
   "center",                  cr5000_arc_center,  // only the sign of the center coordinates are significant 
   "end",                     cr5000_arc_end,
};

#define  SIZ_ARC_LST             (sizeof(arc_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     (pt 0 0)
//     (width 254000)
//     (height 50800)
//     (rAngle 0.000000)
//     (outlineWidth 254)
//     (fillWidth 254)
//     (fillAngle 0.000000)

static List  rectangle_lst[] =
{
   "pt",                   cr5000_pt,
   "width",                cr5000_width,
   "height",               cr5000_height,
   "rAngle",               fskip,
   "fillAngle",            fskip,
   "outlineWidth",         fskip,
   "fillWidth",            fskip,
};

#define  SIZ_RECTANGLE_LST       (sizeof(rectangle_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static   int   roundthermal_in();
static   int   roundthermal_out();
static   int   roundthermal_nbridge();
static   int   roundthermal_bridgewidth();
static   int   roundthermal_bridgeangle();

static List  roundthermal_lst[] =
{
   "pt",                   cr5000_pt,
   "in",                   roundthermal_in,
   "out",                  roundthermal_out,
   "nbridge",              roundthermal_nbridge,
   "bridgewidth",          roundthermal_bridgewidth,
   "bridgeangle",          roundthermal_bridgeangle,
};

#define  SIZ_ROUNDTHERMAL_LST    (sizeof(roundthermal_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List  donut_lst[] =
{
   "pt",                   cr5000_pt,
   "in",                   roundthermal_in,
   "out",                  roundthermal_out,
};

#define  SIZ_DONUT_LST           (sizeof(donut_lst) / sizeof(List))

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static List  oblong_lst[] =
{
   "pt",                   cr5000_pt,
   "width",                cr5000_width,
};

#define  SIZ_OBLONG_LST          (sizeof(oblong_lst) / sizeof(List))
