
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once


/* Define Section *********************************************************/
#define MAX_LONGLINE            200000   // yes 200K lines xy koos
#define MAX_LINE                64000    // yes 64K lines
#define MAX_NETLINE             64000    // yes 64K lines
#define MAX_POLY                50000    // yes 50 thousand

#define MAX_CURPAD              100   // stackup for the current pad
#define MAX_LOCALPAD            1000  // list of current pads.

#define  MAX_ATTR             2000

#define WORD_DELIMETER          " \t\n\r"
#define TEXT_DELIMETER          "\""
#define NONET                   "NO_NET"
#define QTNETZERO               "(NET0)"

#define MAX_LAYERS                  255   /* Max number of layers.   */

// Case dts0100425301 - The case was about text justification not working, but case data
// supplied good and bad jpg images. The .8 aspect ratio made text too long.
// Changing to .66 made it match the good jpg quite nicely. It isn't clear if .8
// was chosen for any particular good reason, or just some out-of-the-air value, hence
// this comment about when and why it changed to .66.
//#define TEXT_ASPECT_RATIO            0.8
#define TEXT_ASPECT_RATIO            0.66

// Flags for text alignment
#define  VBTEXT_N_A          0x0000
#define  VBTEXT_H_LEFT       0x0001
#define  VBTEXT_H_CENTER     0x0002
#define  VBTEXT_H_RIGHT      0x0004
#define  VBTEXT_V_TOP        0x0010
#define  VBTEXT_V_CENTER     0x0020
#define  VBTEXT_V_BOTTOM     0x0040

#undef FILE_UNKNOWN   // in winbase.h "#define FILE_UNKNOWN                    5"

#define FILE_UNKNOWN            0
#define FILE_PLACEMENT_OUTLINE  1
#define FILE_ASSM_OUTL          2
#define FILE_ROUTE_OUTL         3
#define FILE_BOARD_OUTL         4
#define FILE_TRACE              5
#define FILE_POS_PLANE          6
#define CELL_ASSEMBLY_OUTLINE   7
#define FILE_LINE               8
#define FILE_ROUTE_OBS          9
#define FILE_PLACE_OBS          10
#define FILE_CONTOUR            11
#define FILE_PLANE_OUTL         12
#define FILE_VIA_OBS            13
#define FILE_TRACE_OBS          14
#define FILE_CMPLX_SHAPE        15
#define FILE_SILK_ARTWORK       17
#define FILE_SHAPE              18
#define FILE_PLANE_HOLE         19
#define FILE_PACK_OUTL          20
#define FILE_NET                21
#define FILE_CONDUCTIVE_AREA    22
#define FILE_SOLDER_MASK        23
#define FILE_ASSEMBLY_OUTL      24
#define FILE_INSERTION_OUTL     25
#define CELL_INSERTION_OUTLINE  26
#define CELL_FANOUT             27
#define FILE_NEG_PLANE          28
#define FILE_PANEL_OUTL         29 // panel
#define FILE_PANEL_BORDER       30 // panel
#define FILE_CELL_CONDUCTIVE_AREA 31
#define FILE_MANUFACTURING_OUTL 32 // board
#define FILE_DOCUMENTATION      33 // panel
#define FILE_SCORING_LINE       34 // panel
#define FILE_SHEARING_LINE      35 // panel
#define FILE_SOLDER_PASTE       37

#define T_TRIANGLE              -1

#define SMD_TOP                 "SMD_TOP"      // smd
#define SMD_BOT                 "SMD_BOT"      
#define PAD_TOP                 "PAD_TOP"      // throu hole
#define PAD_BOT                 "PAD_BOT"
#define PAD_INT                 "PAD_INT"

#define PAD_TOP_SOLDERMASK      "SOLDERMASK_TOP"
#define PAD_BOT_SOLDERMASK      "SOLDERMASK_BOT"
#define PAD_TOP_SOLDERPASTE     "SOLDERPASTE_TOP"
#define PAD_BOT_SOLDERPASTE     "SOLDERPASTE_BOT"

#define SMDMASK_TOP             "SMDMASK_TOP"  // smd mask
#define SMDMASK_BOT             "SMDMASK_BOT"     
#define SMDPASTE_TOP            "SMDPASTE_TOP" // smd paste 
#define SMDPASTE_BOT            "SMDPASTE_BOT"    
#define PAD_CLEARANCE           "PAD_CLEARANCE"
// These are the list sizes
#define  SIZ_LAYSTACK_LST              (sizeof(laystack_lst) / sizeof(List))
#define  SIZ_LAY_LST                   (sizeof(lay_lst) / sizeof(List))
#define  SIZ_PAD_LST                   (sizeof(pad_lst) / sizeof(List))
#define  SIZ_PADFORM_LST               (sizeof(padform_lst) / sizeof(List))
#define  SIZ_HOLE_LST                  (sizeof(hole_lst) / sizeof(List))
#define  SIZ_DRILLSYM_LST              (sizeof(drillSym_lst) / sizeof(List))
#define  SIZ_SYMFROMLIST_LST           (sizeof(symfromlist_lst) / sizeof(List))
#define  SIZ_CHARACTER_AS_SYMBOL_LST   (sizeof(character_as_symbol_lst) / sizeof(List))
#define  SIZ_STANDARDDRILLSYMBOL_LST   (sizeof(standarddrillsymbol_lst) / sizeof(List))
#define  SIZ_PADGEOM_LST               (sizeof(padgeom_lst) / sizeof(List))
#define  SIZ_CEL_LST                   (sizeof(cel_lst) / sizeof(List))
#define  SIZ_GRAPHIC_LST               (sizeof(graphic_lst) / sizeof(List))
#define  SIZ_CONTOUR_LST               (sizeof(contour_lst) / sizeof(List))
#define  SIZ_PanelInstance_LST         (sizeof(panelInstance_lst) / sizeof(List)) // panel
#define  SIZ_PanelInstanceOrigin_LST   (sizeof(panelInstanceOrigin_lst) / sizeof(List)) // panel
#define  SIZ_PART_GRAPH_LST            (sizeof(part_graph_lst) / sizeof(List))
#define  SIZ_NET_LST                   (sizeof(net_lst) / sizeof(List))
#define  SIZ_GENERATED_DATA_LST        (sizeof(generated_data_lst) / sizeof(List))
#define  SIZ_PLANE_LYR_LST             (sizeof(plane_lyr_lst) / sizeof(List))
#define  SIZ_CELPART_LST               (sizeof(celpart_lst) / sizeof(List))
#define  SIZ_CELPIN_LST                (sizeof(celpin_lst) / sizeof(List))
#define  SIZ_DESPIN_LST                (sizeof(despin_lst) / sizeof(List))
#define  SIZ_CELPAD_LST                (sizeof(celpad_lst) / sizeof(List))
#define  SIZ_PAD_PADSTACK_LST          (sizeof(pad_padstack_lst) / sizeof(List))
#define  SIZ_PAD_HOLENAME_LST          (sizeof(pad_holename_lst) / sizeof(List))
#define  SIZ_USERLAYER_LST             (sizeof(userlayer_lst) / sizeof(List))
#define  SIZ_NETPROPS_NETNAME_LST      (sizeof(netprops_netname_lst) / sizeof(List))
#define  SIZ_PDB_TYPENUMBER_LST        (sizeof(pdb_typenumber_lst) / sizeof(List))
#define  SIZ_NETPROPS_PINS_LST         (sizeof(netprops_pins_lst) / sizeof(List))
#define  SIZ_LAYERSTACKUP_LST          (sizeof(layerstackup_lst) / sizeof(List))
#define  SIZ_VIA_SPANS_LST             (sizeof(via_spans_lst) / sizeof(List))
#define  SIZ_VIA_SPANS_PADSTACK_LST    (sizeof(via_spans_padstack_lst) / sizeof(List))
#define  SIZ_LAYERCONTENS_LST          (sizeof(layercontens_lst) / sizeof(List))
#define  SIZ_USERLAYERNUM_LST          (sizeof(userlayernum_lst) / sizeof(List))
#define  SIZ_PADSTACK_LST              (sizeof(padstack_lst) / sizeof(List))
#define  SIZ_GRAPHLINE_LST             (sizeof(graphline_lst) / sizeof(List))
#define  SIZ_TEXT_LST                  (sizeof(text_lst) / sizeof(List))
#define  SIZ_GRAPHTEXT_LST             (sizeof(graphtext_lst) / sizeof(List))
#define  SIZ_GRAPHARC_LST              (sizeof(grapharc_lst) / sizeof(List))
#define  SIZ_GRAPHCIRCLE_LST           (sizeof(graphcircle_lst) / sizeof(List))
#define  SIZ_DES_LST                   (sizeof(des_lst) / sizeof(List))
#define  SIZ_NETPROPS_LST              (sizeof(netprops_lst) / sizeof(List))
#define  SIZ_PDB_LST                   (sizeof(pdb_lst) / sizeof(List))
#define  SIZ_JOBPREFS_LST              (sizeof(jobprefs_lst) / sizeof(List))
#define  SIZ_BOARD_ORG_LST             (sizeof(board_org_lst) / sizeof(List))
#define  SIZ_GLUESPOT_LST              (sizeof(gluespot_lst) / sizeof(List))
#define  SIZ_PART_INSTANCE_LST         (sizeof(part_instance_lst) / sizeof(List))
#define  SIZ_PART_PIN_LST              (sizeof(part_pin_lst) / sizeof(List))
#define  SIZ_PART_DEF_INSTANCE_LST     (sizeof(part_def_instance_lst) / sizeof(List))
#define  SIZ_VIA_INSTANCE_LST          (sizeof(via_instance_lst) / sizeof(List))
#define  SIZ_TESTPOINT_INSTANCE_LST    (sizeof(testpoint_instance_lst) / sizeof(List))
#define  SIZ_MH_INSTANCE_LST           (sizeof(mh_instance_lst) / sizeof(List))
#define  SIZ_FIDUCIAL_INSTANCE_LST     (sizeof(fiducial_instance_lst) / sizeof(List))

#define  SIZ_CELPAD_PADSTACK_LST         (sizeof(celpin_padstack_lst) / sizeof(List))
#define  SIZ_CELPAD_PADSTACK_NEGPAD_LST  (sizeof(celpin_padstack_negpad_lst) / sizeof(List))



#define  TEST_ATTR            1  

/* Structures Section *********************************************************/
typedef struct
{
   CString  attrib;
   CString  mapattrib;
} VBAttribmap;
typedef CTypedPtrArray<CPtrArray, VBAttribmap*> CAttribmapArray;

typedef struct
{
   char  *name;
   int   atttype;
}VB99Attr;

typedef struct
{
   double   d;
   int      toolindex;
} vb99Drill;
typedef CArray<vb99Drill, vb99Drill&> CDrillArray;

typedef struct
{
   CString  name;
   char     attr;
} VBAdef;

typedef struct
{
   double   x,y;
   char     side[80];
   double   height;
   double   width;
} Name;

typedef struct
{
   int      from_padlistptr; // index into padlist
   double   rotation;
   int      layerptr;
   char     padgeomname[80];
   double   x,y;
   double   drill;
   int      smd;
} CurPad;

typedef struct
{
   CString  shapename;
   CString  padname;
} vb99Via;

typedef struct
{
   int      curpadcnt;
   CurPad   *curpad;
} Localpad;

typedef struct
{
   CString  pinName;
   CString  netName;
   CString  pinOption;
   CPoint2d m_pinLoc;
   int      m_padstackBlkNum;
} VBPinNet;
typedef CArray<VBPinNet, VBPinNet&> CPinNetArray;

// poly_l graphic
// Global has layer and linewidth
// for Lines []xy are the vertex

// for Arc   [0]xy is center coo
//           G.startangle is start angle
//           G.deltaangle  is delta angle
//           G.radius  is pri radius
//           ---  is sec radiu

typedef struct
{
   CString  name;       
   CString  name_1;           // this is for part number or pin_options
   CString  pinname;
   CString  symbol;           // this is a childname to name i.e Padtstack to cell etc..
   CString  ref_des;             // ref_des text style
   CString  level;
   int      lyrnr;
   int      dialeccnt;
   double   thickness;
   CString  lay_dielectric_const;   // layer dielectric constant
   CString  lay_resistivity;        // layer dielectric constant
   CString  keepout_type;           // Keepout Type
   double   textwidth;
   int      just;                // horiz, vert
   int      push_tok;
   int      geomtyp;             // geomtype or parttype
   int      smd;
   int      pincnt;
   char     mirror;              // mirror 
   char     sidebot;             // side - is not mirror
   char     padstackshape;
   int      notplated;
   int      punched;             // punched flag for drill holes
   double   xdim,ydim,xoff,yoff;
   double   diameter;            // diameter or width
   double   radius;              // circle and arcs
   double   height;
   double   chamfer;
   double   tie_leg_width;
   double   thermal_clearance;
   double   rotation;            // in degree
   double   comp_x, comp_y, comp_rotation;
   double   testPointRotation;
   char     comp_mirror;
   char     fanout_trace;        //
   double   startangle, deltaangle; // in degree
   CString  text_type;
   CAttributes** AttribMap; 
   int      refdes_Count;
   CMapStringToWord  m_duplicatePinMapCount;
   CString packageGroup;
   CString  negPadName;
   CString  negPadLayer;
   CString  netName;
   double   negPadRot;
   double   plusTol;       // POSITIVE TOLERANCE
   double   minusTol;      // NEGATIVE TOLERANCE

   // Fix for dts0100504781 - Via Span Info is not exported
   CViaSpansSection*  viaSpansSection;
   CViaSpan*          curViaSpan;
   CString  unigroup;      // The group name of the component
   int layout_def;
} Global;

typedef  struct
{
   char     *token;
   int      (*function)();
} List;


/******************************************************************************/
// CVBPin
/******************************************************************************/
enum ESideOption
{
   sideOptionNone,
   sideOptionMountSide,
   sideOptionOppSide,
   sideOptionTop,
   sideOptionBottom,
};

class CVBPin
{
public:
   CVBPin(CString name, CString pinOption, int padstackBlkNum);
   CVBPin(const CVBPin& other);
   ~CVBPin();

private:
   CString m_name;
   ESideOption m_pinOption;
   int m_ccPadstackBlkNum;

public:
   CString GetName() const                         { return m_name;           }
   ESideOption GetPinOption() const                { return m_pinOption;      }

   int GetPadstackBlkNum()                         { return m_ccPadstackBlkNum; }
   void SetPadstackBlkNum(int num)                 { m_ccPadstackBlkNum = num;  }
};
typedef CTypedPtrListContainer<CVBPin*> CVBPinList;


/******************************************************************************/
// CVBGeom
/******************************************************************************/
class CVBGeom
{
public:
   CVBGeom(CString name, int geomNum);
   ~CVBGeom();

private:
   CString m_name;
   CString m_originalName;
   CVBPinList m_pinList; 
   CTypedPtrListContainer<CVBGeom*> m_deriveGeomList;
   int m_geomNum;
   ESideOption m_silkscreenSide;
   ESideOption m_soldermaskSide;

public:
   CString GetName() const                      { return m_name;           }
   int GetGeomNum() const                       { return m_geomNum;        }
   ESideOption GetSilkscreenSide() const        { return m_silkscreenSide; }
   ESideOption GetSoldermaskSide() const        { return m_soldermaskSide; }

   void SetName(const CString name)             { m_name = name;           }
   void SetGeomNum(const int geomNum)           { m_geomNum = geomNum;     }
   void SetSilkscreenSide(const CString side);
   void SetSoldermaskSide(const CString side);

   CVBPin* AddPin(CString pinName, CString pinOption, int padstackBlkNum);
   CVBPin* FindPin(CString pinName);
   bool IsDifference(CVBGeom* otherVBGeom);
   void AddDerivedGeom(CVBGeom* otherVBGeom);
   CVBGeom* CheckForDerivedGeom(CVBGeom* otherVBGeom);
   CString GetDerivedGeomName();
};


/******************************************************************************/
// CVBGeomMap
/******************************************************************************/
class CVBGeomMap
{
public:
   CVBGeomMap();
   ~CVBGeomMap();

private:
   CTypedMapStringToPtrContainer<CVBGeom*> m_geomMap;
   CVBGeom* lastAddedGeom;

public:
   CVBGeom* AddGeom(CString name, int geomNum);
   CVBGeom* FindGeom(CString name);
   CVBGeom* GetLastGeom();
   void Empty();
};

//_________________________________________________________________________________________________
class CVbPcbInstance
{
private:
   CString m_refDes;
   CPoint2d m_origin;
   double m_rotationRadians;
   CString m_localPath;
   CString m_sourcePath;
   CString m_hkpPath;
   bool m_topFacement;
   DataStruct* m_pcbInstanceData;

public:
   CVbPcbInstance(const CString& refDes);

   CString getRefDes() const;

   CString getLocalPath() const;
   void setLocalPath(const CString& path);

   CString getSourcePath() const;
   void setSourcePath(const CString& path);

   CString getHkpPath() const;
   void setHkpPath(const CString& path);

   DataStruct* getPcbInstanceData() const;
   void setPcbInstanceData(DataStruct* data);

   CPoint2d getOrigin() const;
   void setOrigin(const CPoint2d& origin);

   double getRotationRadians() const;
   void setRotationRadians(double radians);

   bool getTopFacement() const;
   void setTopFacement(bool flag);
};

//_________________________________________________________________________________________________
class CVbPcbInstances
{
private:
   static CVbPcbInstances* m_pcbInstances;

public:
   static CVbPcbInstances& getPcbInstances();
   static void release();

private:
   CTypedPtrListContainer<CVbPcbInstance*> m_pcbInstanceList;

public:
   CVbPcbInstances();
   void empty();

   CVbPcbInstance* addPanelInstance(const CString& refDes);
   int getCount() const;

   POSITION getHeadPosition() const;
   CVbPcbInstance* getNext(POSITION& pos) const;
   CVbPcbInstance& getLast();

   void insertPcbInstances(FileStruct& panelSubFile,FileStruct& pcbSubFile,const CString& pcbDesignPath);
};

//_________________________________________________________________________________________________
typedef struct
{
   bool hasVia;
   bool hasTrace;
}VB99NetContainer;

/******************************************************************************/
// CVBNetContainerMap
/******************************************************************************/
class CVBNetContainerMap: public CTypedPtrMap<CMapStringToPtr,CString,VB99NetContainer*>
{
public:
   void Release();
   VB99NetContainer* SetDefineNetAt(CString netName);
   void SetHasViaAt(CString netName, bool hasVia);
   void SetHasTraceAt(CString netName, bool hasTrace);
   void SetNetContainerAt(CString netName, VB99NetContainer *netdata);
   bool IsEmptyRoute(CString netName);
};

//_________________________________________________________________________________________________
enum VbHoleTypeTag
{
   vbHoleTypeMounting,
   vbHoleTypeShearing,
   vbHoleTypeTooling,
   vbHoleTypeUndefined,
};

CString VbHoleTypeTagToString(VbHoleTypeTag tagValue);

/* Lists Section *********************************************************/
static int vb99_null(int start_ident);
static int vb99_skip();
static int vb99_units();
static int vb99_xyr();


static int vb99_filetype();
static int vb99_name();
static int vb99_side();

static int celpart_shape();
static int vb99_font();
static int vb99_drillgraphic();


/* laystack_lst section */
static int laystack_layer_num();

static List laystack_lst[] =
{
   "LAYER_NUM",        laystack_layer_num,
};

/* lay_lst section */
static int vb99_layer();

static List lay_lst[] =
{
   "NAME",             vb99_layer,
   "ROUTING",          vb99_skip,
   "TRC_LEVEL_NUM",    vb99_skip,
   "NUM_BBVIAS",       vb99_skip,
};

/* pad_lst section */
static int pad_filetype();
static int pad_version();
static int padstack_units();
static int pad_pad();
static int pad_padstack();
static int pad_hole();
static int pad_custom_drill_symbol();

static List pad_lst[] =
{
   "FILETYPE",             pad_filetype,
   "VERSION",              pad_version,
   "CREATOR",              vb99_skip,
   "SCHEMA_VERSION",       vb99_skip,
   "DATE",                 vb99_skip,
   "UNITS",                padstack_units,
   "PAD",                  pad_pad,
   "PADSTACK",             pad_padstack,
   "HOLE",                 pad_hole,
   "CUSTOM_DRILL_SYMBOL",  pad_custom_drill_symbol,
};

/* padform_lst section */
static int padform_thermal();
static int padform_custom();
static int padform_square();
static int padform_octagon();
static int padform_round();
static int padform_round_donut();
static int padform_rectangle();
static int padform_radius_corner_rect();
static int padform_oblong();
static int padform_offset();
static int padform_octagonal_finger();
static int padform_round_finger();
static int padform_elongated_octagon();
static int padform_square_donut();
static int padform_chamfered_rectangle();
static int padform_4_web_square_thermal();
static int padform_4_web_square_thermal_45();
static int padform_4_web_round_thermal();
static int padform_4_web_round_thermal_45();
static int padform_2_web_round_thermal();
static int padform_2_web_round_thermal_45();
static int padform_4_web_rect_thermal();
static int padform_4_web_rect_thermal_45();
static int padform_4_web_oblong_thermal();
static int padform_4_web_oblong_thermal_45();

static List padform_lst[] =
{
   "2_WEB_ROUND_THERMAL",     padform_2_web_round_thermal,
   "2_WEB_ROUND_THERMAL_45",  padform_2_web_round_thermal_45,
   "4_WEB_ROUND_THERMAL",     padform_4_web_round_thermal,
   "4_WEB_ROUND_THERMAL_45",  padform_4_web_round_thermal_45,
   "4_WEB_SQUARE_THERMAL",    padform_4_web_square_thermal,
   "4_WEB_SQUARE_THERMAL_45", padform_4_web_square_thermal_45,
   "4_WEB_RECT_THERMAL",      padform_4_web_rect_thermal,
   "4_WEB_RECT_THERMAL_45",   padform_4_web_rect_thermal_45,
   "4_WEB_OBLONG_THERMAL",    padform_4_web_oblong_thermal,
   "4_WEB_OBLONG_THERMAL_45", padform_4_web_oblong_thermal_45,
   "CUSTOM",                  padform_custom,
   "SQUARE",                  padform_square,
   "OCTAGON",                 padform_octagon,
   "ROUND",                   padform_round,
   "ROUND_DONUT",             padform_round_donut,
   "RECTANGLE",               padform_rectangle,
   "RADIUS_CORNER_RECTANGLE", padform_radius_corner_rect,
   "OBLONG",                  padform_oblong,
   "OFFSET",                  padform_offset,
   "CHAMFERED_RECTANGLE",     padform_chamfered_rectangle,
   "OCTAGONAL_FINGER",        padform_octagonal_finger,
   "ELONGATED_OCTAGON",       padform_elongated_octagon,
   "ROUND_FINGER",            padform_round_finger,
   "SQUARE_DONUT",            padform_square_donut,
   "PAD_OPTIONS",             vb99_skip,
   "TIMESTAMP",               vb99_skip,
};

/* hole_lst section */
static int hole_option();
static int drill_symbol();
static int hole_positive_tolerance();
static int hole_negative_tolerance();
static List hole_lst[] =
{
   "ROUND",                   padform_round,
   "RECT",                    padform_rectangle,
   "SQUARE",                  padform_square,
   "SLOT",                    padform_oblong,
   "HOLE_OPTIONS",            hole_option,
   "TIMESTAMP",               vb99_skip,
   "DEPTH_ASSIGNMENT_METHOD", vb99_skip,
   "DRILL_SYMBOL",            drill_symbol,
   "POSITIVE_TOLERANCE",      hole_positive_tolerance,
   "NEGATIVE_TOLERANCE",      hole_negative_tolerance,
};

static int use_symbol_from_list();
static int use_character_as_symbol();
static List drillSym_lst[] =
{
   "ASSIGN_DURING_OUTPUT",    vb99_skip,
   "USE_CHARACTER_AS_SYMBOL", use_character_as_symbol,
   "USE_SYMBOL_FROM_LIST",    use_symbol_from_list,
};

static int character_symbol();
static int character_size();
static List character_as_symbol_lst[] = 
{
   "CHARACTER",               character_symbol,
   "SIZE",                    character_size,
};

static int standard_drill_symbol();
static int custom_drill_symbol();
static List symfromlist_lst[] =
{
   "STANDARD_SYMBOL",        standard_drill_symbol,
   "CUSTOM_SYMBOL",          custom_drill_symbol,
};

static int standard_drill_symbol_size();
static List standarddrillsymbol_lst[] =
{
   "SIZE",                  standard_drill_symbol_size,
};

/* padgeom_lst section */
static int padgeom_width();
static int vb99_radius_Rect();
static int padgeom_height();
static int padgeom_chamfer();
static int padgeom_tie_leg_width();
static int padgeom_thermal_clearance();

static List padgeom_lst[] =
{
   "DIAMETER",                padgeom_width,
   "RADIUS",                  vb99_radius_Rect,
   "WIDTH",                   padgeom_width,
   "HEIGHT",                  padgeom_height,
   "CHAMFER",                 padgeom_chamfer,
   "TIE_LEG_WIDTH",           padgeom_tie_leg_width,
   "THERMAL_CLEARANCE",       padgeom_thermal_clearance,
};

/* cel_lst section */
static int cel_filetype();
static int cel_version();
static int cel_drawing_cell();
static int cel_package_cell();
static int cel_mechanical_cell();

static List cel_lst[] =
{
   "FILETYPE",                cel_filetype,
   "VERSION",                 cel_version,
   "CREATOR",                 vb99_skip,
   "DATE",                    vb99_skip,
   "JOB",                     vb99_skip,
   "UNITS",                   vb99_units,
   "DRAWING_CELL",            cel_drawing_cell,
   "PANEL_CELL",              cel_drawing_cell,  // panel
   "PACKAGE_CELL",            cel_package_cell,
   "MECHANICAL_CELL",         cel_mechanical_cell,
};

/* graphic_lst section */
static int vb99_std_lyr();
static int vb99_user_lyr();
static int vb99_diameter();
static int poly_line();
static int poly_arc();
static int poly_circle();
static int vb99_fill();
static int graph_polyline_path();
static int graph_polyline_shape();
static int graph_circle_path();
static int graph_circle_shape();
static int graph_polyarc_path();
static int graph_polyarc_shape();
static int graph_aps_hatch_data();
static int graph_rect_shape();
static int graph_rect_path();
static int graph_side();
static int vb99_height();
static int vb99_conductive_area_options();
static int graph_actualPlaneShape();
static int aps_tie_leg();
static int vb99_netname();
/* contour_lst section */
static int contour_comp();
static int contour_direction();
static int contour_hole();


static List graphic_lst[] =
{
   "ROUTE_LYR",               vb99_std_lyr,
   "USER_LYR",                vb99_user_lyr,
   "WIDTH",                   vb99_diameter,
   "LINE",                    poly_line,  // this draws
   "ARC",                     poly_arc,
   "CIRCLE",                  poly_circle,
   "FILL",                    vb99_fill,
   "POLYLINE_PATH",           graph_polyline_path, 
   "POLYLINE_SHAPE",          graph_polyline_shape,   
   "CIRCLE_PATH",             graph_circle_path,   
   "CIRCLE_SHAPE",            graph_circle_shape,  
   "POLYARC_PATH",            graph_polyarc_path,
   "POLYARC_SHAPE",           graph_polyarc_shape,
   "APS_HATCH_DATA",          graph_aps_hatch_data,
   "RECT_SHAPE",              graph_rect_shape,
   "RECT_PATH",               graph_rect_path,
   "TRACE_OPTIONS",           vb99_skip,
   "OBSTRUCT",                vb99_skip,
   "DISPLAY_WIDTH",           vb99_diameter,
   "SIDE",                    graph_side,
   "HEIGHT",                  vb99_height,
   "UNDERSIDE_SPACE",         vb99_skip,
   "CONDUCTIVE_AREA_OPTIONS", vb99_conductive_area_options,
   "HATCH_PATTERN",           vb99_skip,
   "HATCH_WIDTH",             vb99_skip,
   "HATCH_SPACING",           vb99_skip,
   "NETNAME",                 vb99_netname,
   "TIMESTAMP",               vb99_skip,
   "ACTUAL_PLANE_SHAPE",      graph_actualPlaneShape,
   "APS_TIE_LEG",             aps_tie_leg, 
   "COMP",                    contour_comp,  // this draws
   "DIRECTION",               contour_direction,
   "HOLE",                    contour_hole,
};

static List contour_lst[] =
{
   "COMP",                    contour_comp,  // this draws
   "DIRECTION",               contour_direction,
   "HOLE",                    contour_hole,
   "POLYLINE_PATH",           graph_polyline_path,
   "POLYLINE_SHAPE",          graph_polyline_shape,   
   "CIRCLE_PATH",             graph_circle_path,   
   "CIRCLE_SHAPE",            graph_circle_shape,  
   "POLYARC_PATH",            graph_polyarc_path,
   "POLYARC_SHAPE",           graph_polyarc_shape,
   "APS_HATCH_DATA",          graph_aps_hatch_data,
   "RECT_SHAPE",              graph_rect_shape,
   "RECT_PATH",               graph_rect_path,
};

/* net_lst section */
static int net_conductive_area();
static int net_via();
static int net_testpoint();
static int net_trace();

static List net_lst[] =
{
   "CONDUCTIVE_AREA",         net_conductive_area,
   "TRACE",                   net_trace,
   "VIA",                     net_via,
   "TESTPOINT",               net_testpoint,
};

/* generated_data_lst section */
static int generated_plane_lyr();

static List generated_data_lst[] =
{
   "PLANE_LYR",      generated_plane_lyr,
   "THIEVING_LYR",   generated_plane_lyr,
};

/* plane_lyr_lst section */
static int plane_lyr_plane_type();
static int plane_lyr_plane_net();
static int thievingLayerThievingData();

static List plane_lyr_lst[] =
{
   "PLANE_TYPE",      plane_lyr_plane_type,
   "PLANE_NET",       plane_lyr_plane_net,
   "THIEVING_DATA",   thievingLayerThievingData,
};

/* celpart_lst section */
static int celpart_description();
static int celpart_shapename();
static int celpart_mount_type();
static int vbMountingHole();
static int vbShearingHole();
static int vbToolingHole();
static int vb99_gluespot();
static int celpart_pin();
static int vb99_solder_mask();
static int vb99_solder_paste();
static int celpart_silk_outline();
static int celpart_assembly_outline();
static int celpart_insertion_outline();
static int celpart_placement_outline();
static int celpart_conductive_area();
static int graph_text();
static int vb99_xy();
static int vb99_graphic();
static int celpart_trace();
static int celpart_via();
static int vb99_fiducial();
static int celpart_package_group();
static int celpart_placement_keepout();
static int celpart_contour();
static int celpart_obstruct();

static List celpart_lst[] =
{
   "NUMBER_LAYERS",           vb99_skip,
   "DESCRIPTION",             celpart_description,
   "TIMESTAMP",               vb99_skip,
   "HISTORY",                 vb99_skip,
   "SHAPE_NAME",              celpart_shapename,
   "MOUNT_TYPE",              celpart_mount_type,
   "MH",                      vbMountingHole,
   "MOUNTING_HOLE",           vbMountingHole,
   "SHEARING_HOLE",           vbShearingHole, // panel
   "TOOLING_HOLE",            vbToolingHole, // panel
   "GLUESPOT",                vb99_gluespot,
   "PIN",                     celpart_pin,
   "SOLDER_MASK",             vb99_solder_mask,
   "SOLDER_PASTE",            vb99_solder_paste,
   "SILKSCREEN_OUTLINE",      celpart_silk_outline,
   "ASSEMBLY_OUTLINE",        celpart_assembly_outline,
   "INSERTION_OUTLINE",       celpart_insertion_outline,
   "PLACEMENT_OUTLINE",       celpart_placement_outline,
   "CONDUCTIVE_AREA",         celpart_conductive_area,
   "PLACEMENT_KEEPOUT",       celpart_placement_keepout,
   "TEXT",                    graph_text,
   "XY",                      vb99_xy,         // used in cell  
   "GRAPHIC",                 vb99_graphic,
   "TRACE",                   celpart_trace,
   //"VIA",                     celpart_via,    // this are fanouts, but they are repeated in the Layout.hkp .USE_CELL_FANOUT_VIAS
   "PACKAGE_GROUP",           celpart_package_group,
   "OBSTRUCT",                celpart_obstruct,
   "FIDUCIAL",                vb99_fiducial, 
   "FID_",                    vb99_fiducial, 
   "ASSEMBLY_ORIGIN",         vb99_skip,
   "CONTOUR",                 celpart_contour,
};

/* celpin_lst section */
static int vb99_padstack();
static int vb99_pinname();
static int vb99_padrot();
static int vb99_pin_options();

static List celpin_lst[] =
{
   "XY",                      vb99_xy,
   "PADSTACK",                vb99_padstack,
   "PIN_NAME",                vb99_pinname,
   "ROTATION",                vb99_padrot,
   "NETNAME",                 vb99_skip,
   "PIN_OPTIONS",             vb99_pin_options,
};


static int vb99_celpin_padstack_negpadname();

static List celpin_padstack_lst[] =
{
   "NEG_PAD_NAME",                    vb99_celpin_padstack_negpadname,
};


static int vb99_celpin_padstack_negpad_padlyr();
static int vb99_celpin_padstack_negpad_padrot();

static List celpin_padstack_negpad_lst[] =
{
   "PAD_LYR",                    vb99_celpin_padstack_negpad_padlyr,
   "PAD_ROT",                    vb99_celpin_padstack_negpad_padrot,
};

/* despin_lst  section */
// can not use celpin, because this overwrites part koos etc...
static List despin_lst[] =
{
   "XY",                      vb99_skip,
   "PADSTACK",                vb99_skip,
   "PIN_NAME",                vb99_pinname,
   "ROT",                     vb99_skip,
   "DIA",                     vb99_skip,
};

/* celpad_lst section */
static List celpad_lst[] =
{
   "XY",                      vb99_xy,
   "PADC",                    vb99_padstack,
   "ROT",                     vb99_padrot,
   "LYR",                     vb99_std_lyr,    // this is used for line_obs in cell
};

/* pad_padstack_lst section */
static int padstack_smd();
static int padstack_technology();

static List pad_padstack_lst[] =
{
   "TIMESTAMP",               vb99_skip,
   "TECHNOLOGY_OPTIONS",      vb99_skip,
   "PADSTACK_TYPE",           padstack_smd,
   "TECHNOLOGY",              padstack_technology,
};

/* pad_holename_lst section */
static List pad_holename_lst[] =
{
   "OFFSET",                  padform_offset,
};

/* userlayer_lst section */
static int userlayer_layername();

static List userlayer_lst[] =
{
   "USER_LAYER_NAME",         userlayer_layername,
};

/* netprops_netname_lst section */
static int netprops_pins();
static int netprops_pin_section(); // new Mentor VB ASCII format

static List netprops_netname_lst[] =
{
   "CDB_NETNAME",             vb99_skip,  
   "CDB_NETNUMBER",           vb99_skip,  
   "ORDER_TYPE",              vb99_skip,  
   "MAX_CROSSTALK",           vb99_skip,  
   "PARALLEL_RULES_FACTOR",   vb99_skip,  
   "PINS",                    netprops_pins,          
   "PIN_SECTION",             netprops_pin_section,            
   "TIMING_DELAY",            vb99_skip,           
   "MAX_STUBLENGTH",          vb99_skip,           
   "MAX_VIAS",                vb99_skip,           
};

/* pdb_typenumber_lst section */
static int pdb_name();
static int pdb_label();
static int pdb_desc();
static int pdb_refprefix();
static int pdb_prop();
static int pdb_topcell();
static int pdb_botcell();

static List pdb_typenumber_lst[] =
{
   "Name",                    pdb_name,   
   "Label",                   pdb_label,  
   "Desc",                    pdb_desc,   
   "RefPrefix",               pdb_refprefix, 
   "TopCell",                 pdb_topcell,  
   "Modified",                vb99_skip,  
   "Prop",                    pdb_prop,   
   "SwapGroup",               vb99_skip,  
   "Symbol",                  vb99_skip,  
   "Slots",                   vb99_skip,  
   "AltCell",                 vb99_skip,  
   "BottomCell",              pdb_botcell,  
};

/* netprops_pins_lst section */
static int netprops_pins_refpin();

/******************************

.FILETYPE NETPROPERTIES_FILE
.VERSION "02.00"
.CREATOR "Mentor Graphics Corporation"
.DATE "Wednesday, June 28, 2000 08:48 AM"

.NETCLASS "(Default)"
..NETNAMES "B24"
.NETNAME "B24"
..CDB_NETNAME "B24"
..CDB_NETNUMBER 442369
..ORDER_TYPE Free
..PIN_SECTION 2                <++++++++++++++++++++++++++++++
...REF_PINNAME "P1-B24"
....TYPE S
...REF_PINNAME "TP7-1"
....TYPE S

.FILETYPE NETPROPERTIES_FILE
.VERSION "VB99.0"
.CREATOR "VeriBest PCB NetPropsDB2HKP Translator"
.DATE "Wednesday, March 10, 1999 08:54 AM"
.JOB "To Be Determined"

.UNITS th

.NETCLASS "(Default)"
..NETNAMES "ACLK[0]"
.NETNAME "ACLK[0]"
..CDB_NETNAME ""
..CDB_NETNUMBER 130
..ORDER_TYPE Free
..PINS
...COUNT 3
...REF_PIN "U22-27"   <+++++++++++++++++++++++++++++++++++
...TYPE S
...REF_PIN "U23-27"
...TYPE S
...REF_PIN "U1-18"
...TYPE L

******************************/

static List netprops_pins_lst[] =
{
   "COUNT",                   vb99_skip,  
   "REF_PIN",                 netprops_pins_refpin,   
   "REF_PINNAME",             netprops_pins_refpin,   
   "TYPE",                    vb99_skip,
};

/* layerstackup_lst section */
static int layerstackup_dialectric();
static int layerstackup_signal();
static int layerstackup_plane();

static List layerstackup_lst[] =
{
   "DIALECTRIC_LAYER",        layerstackup_dialectric,   // seen this 
   "DIELECTRIC_LAYER",        layerstackup_dialectric,   // and seen this !
   "SIGNAL_LAYER",            layerstackup_signal,
   "PLANE_LAYER",             layerstackup_plane,
};

/* via_spans_lst section */
static int via_spans_general_clearance();
static int via_spans_same_net_clearance();
static int via_spans_use_mnt_opp_pads();
static int via_spans_padstack();

static List via_spans_lst[] =
{
   "ENABLE_GENERAL_CLEARANCES",     via_spans_general_clearance,
   "ENABLE_SAME_NET_CLEARANCES",    via_spans_same_net_clearance,
   "USE_MNT_OPP_PADS_FOR_BB_VIAS",  via_spans_use_mnt_opp_pads,
   "PADSTACK_NAME",                 via_spans_padstack,
};

/* via_spans_lst section */
static int via_spans_padstack_fromLayer();
static int via_spans_padstack_toLayer();
static int via_spans_padstack_capacitance();
static int via_spans_padstack_inductance();
static int via_spans_padstack_delay();
static int via_spans_padstack_grid();
static int via_spans_padstack_attributes();

static List via_spans_padstack_lst[] =
{
   "FROM_LAYER",              via_spans_padstack_fromLayer,
   "TO_LAYER",                via_spans_padstack_toLayer,
   "VIA_SPAN_CAPACITANCE",    via_spans_padstack_capacitance,
   "VIA_SPAN_INDUCTANCE",     via_spans_padstack_inductance,
   "VIA_SPAN_DELAY",          via_spans_padstack_delay,
   "VIA_SPAN_GRID",           via_spans_padstack_grid,
   "VIA_SPAN_ATTRIBUTES",     via_spans_padstack_attributes,
};

/* layercontens_lst section */
static int layercontens_description();
static int layercontens_conductivenr();
static int layercontens_thickness();
static int layercontens_plane_net();
static int layercontens_resistivity();
static int layercontens_dielectric();
static int layercontens_plane_type();


static List layercontens_lst[] =
{
   "LAYER_DESCRIPTION",       layercontens_description,
   "CONDUCTIVE_LAYER_NUMBER", layercontens_conductivenr,
   "LAYER_THICKNESS",         layercontens_thickness,
   "PLANE_NET",               layercontens_plane_net,
   "DIELECTRIC_CONSTANT",     layercontens_dielectric,
   "LAYER_ENABLED",           vb99_skip,
   "DIRECTIONAL_BIAS",        vb99_skip,
   "TRACE_WIDTH",             vb99_skip,
   "LAYER_RESISTIVITY",       layercontens_resistivity,
   "LAYER_PAIR",              vb99_skip,     // I need to use this for mirror
   "PLANE_TYPE",              layercontens_plane_type,     // POSITIVE or NEGATIVE
   "LAYER_TECHNOLOGY",        vb99_skip,     //  
};

/* userlayernum_lst section */
static int userlayer_layernumber();

static List userlayernum_lst[] =
{
   "USER_LAYER_NUMBER",       userlayer_layernumber,
};

/* padstack_lst section */
static int padstack_top();
static int padstack_bot();
static int padstack_int();
static int padstack_thermal();
static int padstack_clearance();
static int padstack_topsoldermask();
static int padstack_botsoldermask();
static int padstack_topsolderpaste();
static int padstack_botsolderpaste();
static int padstack_holename();

static List padstack_lst[] =
{
   "TOP_PAD",                 padstack_top,
   "BOTTOM_PAD",              padstack_bot,
   "INTERNAL_PAD",            padstack_int,
   "THERMAL_PAD",             padstack_thermal,
   "CLEARANCE_PAD",           padstack_clearance,
   "TOP_SOLDERMASK_PAD",      padstack_topsoldermask,
   "BOTTOM_SOLDERMASK_PAD",   padstack_botsoldermask,
   "TOP_SOLDERPASTE_PAD",     padstack_topsolderpaste,
   "BOTTOM_SOLDERPASTE_PAD",  padstack_botsolderpaste,
   "HOLE_NAME",               padstack_holename,   
   "TECHNOLOGY_OPTIONS",      vb99_skip,
};

/* graphline_lst section */
static int vb99_shape_options();

static List graphline_lst[] =
{
   "XYR",                     vb99_xyr,
   "XY",                      vb99_xy,
   "WIDTH",                   vb99_diameter,
   "SHAPE_OPTIONS",           vb99_shape_options,
   "DISPLAY_WIDTH",           vb99_diameter,
};

/* text_lst section */
static int text_display_attr();
static int text_text_type();

static List text_lst[] =
{
   "TEXT_TYPE",               text_text_type,
   "DISPLAY_ATTR",            text_display_attr,
};

/* graphtext_lst section */
static int vb99_textwidth();
static int vb99_mir();
static int vb99_rot();
static int vb99_textoption();
static int vb99_horzjust();
static int vb99_vertjust();

static List graphtext_lst[] =
{
   "XY",                      vb99_xy,
   "WIDTH",                   vb99_textwidth,   // total length of string  
   "HEIGHT",                  vb99_height,
   "MIR",                     vb99_mir,
   "ROTATION",                vb99_rot,
   "USER_LYR",                vb99_user_lyr,
   "TEXT_LYR",                vb99_std_lyr,
   "TEXT_OPTIONS",            vb99_textoption,
   "HORZ_JUST",               vb99_horzjust,
   "VERT_JUST",               vb99_vertjust,
   "STROKE_WIDTH",            vb99_skip,        // attention - there is a unit error !
   "FONT",                    vb99_skip,  
};

/* grapharc_lst section */
static int vb99_radius();
static int vb99_startangle();
static int vb99_deltaangle();
static int vb99_sym();

static List grapharc_lst[] =
{
   "XY",                      vb99_xy,
   "PRI",                     vb99_radius,
   "SEC",                     vb99_radius,
   "ST_ANG",                  vb99_startangle,
   "SW_ANG",                  vb99_deltaangle,
   "WDT",                     vb99_diameter,
   "SYM",                     vb99_sym,
};

/* graphcircle_lst section */
static int vb99_circlediameter();
static int vb99_circleradius();

static List graphcircle_lst[] =
{
   "XY",                      vb99_xy,
   "DIA",                     vb99_circlediameter,
   "RADIUS",                  vb99_circleradius,
   "SYM",                     vb99_sym,
   "FILL",                    vb99_fill,
   "WIDTH",                   vb99_diameter,
   "SHAPE_OPTIONS",           vb99_shape_options,
   "DISPLAY_WIDTH",           vb99_diameter,
};

/* des_lsts section */
static int des_filetype();
static int des_version();
static int des_boardorg();
static int des_part();
static int des_cell();
static int des_mechanical();
static int des_route_outl();
static int des_board_outl();
static int des_manufacturing_outl();
static int des_documentation();
static int des_scoring_line();
static int des_shearing_line();
static int des_assembly_outl();
static int des_panel();         // Mark file as one for panel view
static int des_panel_outl();    // panel
static int des_panel_border();  // panel
static int des_insertion_outl();
static int des_line();
static int des_arc();
static int des_circle();
static int des_net();   
static int des_generated_data();
static int des_placement_keepout();
static int des_obstruct();
static int des_silkscreen_outl();
static int des_drill_graphic();
static int des_panelBoardInstance();  // panel
static int des_contour();             // Read Contour
static List des_lst[] =
{
   "FILETYPE",                des_filetype,
   "VERSION",                 des_version,
   "CREATOR",                 vb99_skip,
   "JOB",                     vb99_skip,
   "DATE",                    vb99_skip,
   "UNITS",                   vb99_units,
   "PANEL_DESIGN",            des_panel,   // Mark file as one for panel view
   "BOARDORG",                des_boardorg,
   "PACKAGE_CELL",            des_part,
   //"PANEL_CELL",              des_part,  // panel
   "DRAWING_CELL",            des_cell,
   "PANEL_CELL",              des_cell,  // panel
   "MECHANICAL_CELL",         des_mechanical,
   "ROUTE_OUTLINE",           des_route_outl,         
   "BOARD_OUTLINE",           des_board_outl,
   "MANUFACTURING_OUTLINE",   des_manufacturing_outl,
   "DOCUMENTATION",           des_documentation,   //dts0100505071
   "SCORING_LINE",            des_scoring_line,    //dts0100505072
   "SHEARING_LINE",           des_shearing_line,   //dts0100505073
   "SOLDER_MASK",             vb99_solder_mask,       
   "ASSEMBLY_OUTLINE",        des_assembly_outl,         
   "PANEL_OUTLINE",           des_panel_outl,     // panel      
   "PANEL_BORDER",            des_panel_border,   // panel      
   "INSERTION_OUTLINE",       des_insertion_outl,        
   "GRAPHIC",                 vb99_graphic,
   "LINE",                    des_line,
   "ARC",                     des_arc,
   "CIRCLE",                  des_circle,
   "TEXT",                    graph_text,
   "MH",                      vbMountingHole,
   "MOUNTING_HOLE",           vbMountingHole,
   "SHEARING_HOLE",           vbShearingHole,  // panel
   "TOOLING_HOLE",            vbToolingHole,  // panel
   "FIDUCIAL",                vb99_fiducial, 
   "NET",                     des_net,
   "MIGRATED_DESIGN",         vb99_skip,
   "GENERATED_DATA",          des_generated_data,
   "OBSTRUCT",                des_obstruct,
   "PLACEMENT_KEEPOUT",       des_placement_keepout,
   //"PLACEMENT_KEEPOUT",       vb99_skip,
   "SILKSCREEN_OUTLINE",      des_silkscreen_outl,
   "DRILL_GRAPHIC",           des_drill_graphic,      
   "BOARD_ORIGIN",            vb99_skip,
   "BOARD_ORIGIN",            vb99_skip,
   "NCDRILL_ORIGIN",          vb99_skip,
   "PANEL_BOARD_INSTANCE",    des_panelBoardInstance,  // panel
   "CONTOUR",                 des_contour,
   "SOLDER_PASTE",            vb99_solder_paste,
};

// panel
static int panelInstance_LocalPath();
static int panelInstance_SourcePath();
static int panelInstance_HkpPath();
static int panelInstance_Rotation();
static int panelInstance_Facement();
static int panelInstance_Origin();

static List panelInstance_lst[] =
{
   "JOB_NAME",                vb99_skip,
   "VARIANT_NAME",            vb99_skip,
   "LOCAL_PATH",              panelInstance_LocalPath,
   "SOURCE_PATH",             panelInstance_SourcePath,
   "HKP_PATH",                panelInstance_HkpPath,
   "XY",                      vb99_skip,
   "ROTATION",                panelInstance_Rotation,
   "FACEMENT",                panelInstance_Facement,
   "INSTANCE_ORIGIN",         panelInstance_Origin,
   "TIMESTAMP",               vb99_skip,
};

static int panelInstanceOrigin_Xy();

static List panelInstanceOrigin_lst[] =
{
   "XY",                      panelInstanceOrigin_Xy,
};

/* netprops_lst section */
static int netprops_filetype();
static int netprops_version();
static int netprops_netname();
static int netprops_netclass();

static List netprops_lst[] =
{
   "FILETYPE",                netprops_filetype,
   "VERSION",                 netprops_version,
   "CREATOR",                 vb99_skip,
   "DATE",                    vb99_skip,
   "UNITS",                   vb99_units,
   "NETNAME",                 netprops_netname,
   "NETCLASS",                netprops_netclass,
   "MATCHED_NETS",            vb99_skip,
   "DIFFERENTIAL_PAIRS",      vb99_skip,
};

/* pdb_lst section */
static int pdb_filetype();
static int pdb_version();
static int pdb_typenumber();

static List pdb_lst[] =
{
   "FILETYPE",                pdb_filetype,
   "VERSION",                 pdb_version,
   "CREATOR",                 vb99_skip,
   "Notation",                vb99_skip,  
   "DATE",                    vb99_skip,
   "UNITS",                   vb99_units,
   "NUMBER",                  pdb_typenumber,
};

/* jobprefs_lst section */
static int jobprefs_filetype();
static int jobprefs_version();
static int vb99_number_layers();
static int jobprefs_userlayer();
static int jobprefs_layerstackup();
static int jobprefs_via_spans();
static int jobprefs_padstack_technology();
static List jobprefs_lst[] =
{
   "FILETYPE",                      jobprefs_filetype,
   "VERSION",                       jobprefs_version,
   "CREATOR",                       vb99_skip,
   "DATE",                          vb99_skip,
   "UNITS",                         vb99_units,
   "DESIGN_UNITS",                  vb99_units,
   "VP_UNITS",                      vb99_skip,
   "PADSTACK_TECHNOLOGY",           jobprefs_padstack_technology,
   "PHYSICAL_LAYERS",               vb99_number_layers,
   "USER_DEFINED_LAYERS_SECTION",   jobprefs_userlayer,
   "LAYER_STACKUP_SECTION",         jobprefs_layerstackup,
   "TEST_POINT_CELL_NAME",          vb99_skip,
   "TEST_POINT_TEST_SIDE",          vb99_skip,
   "TEST_POINT_OBSTRUCTS",          vb99_skip,
   "TEST_POINT_TEST_SIDE",          vb99_skip,
   "TEST_POINT_PLACEMENT_GRID",     vb99_skip,
   "TEST_POINT_REFDES_PREFIX",      vb99_skip,
   "ALLOW_BURIED_RESISTORS",        vb99_skip,
   "SUBSTRATE_TOP",                 vb99_skip,
   "VIA_SPANS_SECTION",             jobprefs_via_spans,
   "TECHNOLOGY_SECTION",            vb99_skip,
   "TUNING_SECTION",                vb99_skip,
   "SIMULATOR_CONTROL_SECTION",     vb99_skip,
   "SIGNAL_ANALYZER_MODELS_SECTION",vb99_skip,
};

/* board_org_lst section */
static List board_org_lst[] =
{
   "XY",                      vb99_xy,
   "CS",                      vb99_skip,
};

/* gluespot_lst section */
static List gluespot_lst[] =
{
   "XY",                      vb99_xy,
};

/* part_instance_lst section */
static int part_instance_shape_name();
static int part_cell_xy();
static int vb99_facement();
static int part_cell_mir();
static int part_cell_rot();
static int part_pin();
static int part_instance_trace();
static int part_silkscreen_outline();
static int part_solder_mask();
static int part_instance_via();
static int part_package_group();
static int part_obstruct();
static int part_options();
static int part_conductive_area();
static int part_obstruct();
static List part_instance_lst[] =
{
   "SHAPE_NAME",              part_instance_shape_name,
   "CELL_NAME",               part_instance_shape_name,
   "XY",                      part_cell_xy,
   "FACEMENT",                vb99_facement,   // this is not the mirror flag.
   "PLACEMENT_LYR",           part_cell_mir,
   "ROTATION",                part_cell_rot,
   "GROUP_ID",                vb99_skip,
   "MOV",                     vb99_skip,
   "PIN",                     part_pin,
   "VIA",                     part_instance_via,
   "TRACE",                   part_instance_trace,
   "FIDUCIAL",                vb99_skip,
   "GLUESPOT",                vb99_skip,
   "MH",                      vb99_skip,
   "MOUNTING_HOLE",           vb99_skip,
   "PLACEMENT_OUTLINE",       vb99_skip,
   "PLACEMENT_KEEPOUT",       vb99_skip, //des_placement_keepout,
   "SILKSCREEN_OUTLINE",      vb99_skip, //part_silkscreen_outline,
   "SOLDER_MASK",             vb99_skip, //part_solder_mask,
   "OBSTRUCT",                part_obstruct, // For RF obstruct
   "ASSEMBLY_OUTLINE",        vb99_skip, //celpart_assembly_outline,
   "INSERTION_OUTLINE",       vb99_skip,
   "DRILL_GRAPHIC",           vb99_skip,
   "GRAPHIC",                 vb99_skip,
   "TEXT",                    graph_text,
   "COMPONENT_OPTIONS",       part_options,
   "ASSEMBLY_ORIGIN",         vb99_skip,
   "PACKAGE_GROUP",           part_package_group,
   "CONDUCTIVE_AREA",         part_conductive_area,
};

/* part_graph_lst section */
static List part_graph_lst[] =
{
   "SIDE",                    graph_side,
};

/* part_pin_lst section */
static int part_pin_net();
static int part_pin_xy();
static int part_pin_option();
static int part_pin_padstack();

static List part_pin_lst[] = 
{
   "NETNAME",                 part_pin_net,
   "XY",                      part_pin_xy,
   "PIN_OPTIONS",             part_pin_option,
   "PADSTACK",                part_pin_padstack,
};

/* part_def_instance_lst section */
static List part_def_instance_lst[] =
{
   "SHAPE_NAME",              part_instance_shape_name,
   "CELL_NAME",               part_instance_shape_name,
   "XY",                      part_cell_xy,
   "FACEMENT",                vb99_facement,   // this is not the mirror flag.
   "PLACEMENT_LYR",           part_cell_mir,
   "ROTATION",                part_cell_rot,
   "MOV",                     vb99_skip,
   "PIN",                     vb99_skip,
   "SILKSCREEN_OUTLINE",      celpart_silk_outline,
   "ASSEMBLY_OUTLINE",        celpart_assembly_outline,
   "INSERTION_OUTLINE",       celpart_insertion_outline,
   "PLACEMENT_OUTLINE",       celpart_placement_outline,
   "PLACEMENT_KEEPOUT",       des_placement_keepout,
   "TEXT",                    graph_text,
   "XY",                      vb99_xy,         // used in cell cell
   "DRILL_GRAPHIC",           vb99_drillgraphic,
   "GRAPHIC",                 vb99_graphic,
   "COMPONENT_OPTIONS",       vb99_skip,
   "FIDUCIAL",                vb99_skip,  
   "OBSTRUCT",                vb99_skip,
};

/* via_instance_lst section */
static int via_instance_layer_pair();
static int via_instance_netname();

static List via_instance_lst[] =
{
   "PADSTACK",                vb99_padstack,
   "XY",                      vb99_xy,
   "VIA_OPTIONS",             vb99_skip,
   "NETNAME",                 via_instance_netname,     // used for fanout vias.
   "LAYER_PAIR",              via_instance_layer_pair,
};

/* testpoint_instance_lst section */
static int testPoint_rotation();

static List testpoint_instance_lst[] =
{
   "FACEMENT",                vb99_facement,   // this is not the mirror flag.
   "PADSTACK",                part_instance_shape_name,
   "ROTATION",                testPoint_rotation,
   "XY",                      vb99_xy,
   "TEXT",                    graph_text,
};

/* mh_instance_lst section */
static List mh_instance_lst[] =
{
   "XY",                      vb99_xy,
   "ROTATION",                vb99_padrot,
   "NETNAME",                 vb99_skip,
   "PADSTACK",                vb99_padstack,
   "MOUNTING_HOLE_OPTIONS",   vb99_skip,
};

/* fiducial_instance_lst section */
static int vb99_side();

static List fiducial_instance_lst[] =
{
   "FIDC",                    vb99_circlediameter,
   "XY",                      vb99_xy,
   "SIDE",                    vb99_side,
   "PADSTACK",                vb99_padstack,
   "ROTATION",                vb99_padrot,
   "FIDUCIAL_OPTIONS",        vb99_skip,
   "FACEMENT",                vb99_facement,   // this is not the mirror flag.
};