// $Header: /CAMCAD/4.5/read_wrt/Accelin.h 22    10/31/06 11:50p Rick Faltersack $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
          
   Vias always get the V: prefix

*/           

#pragma once

class CStandardAperture;

/* Defines *****************************************************************/

#define  ACCELERR                "accel.log"
#define  SHAPENAME               "SHAPENAME"

#define  FNULL                   fnull

#define  MAX_COMPPINREFNAMELENGTH   10
//#define  MAX_LINE                8000    // Max line length. Infopoints can be very long.  
#define  MAX_LAYERS              255      // Max number of layers.   
#define  MAX_SHAPE               1000
#define  MAX_COMPDEF             1000
#define  MAX_LIBPIN              2000     // max is 2000 pins per comp
#define  MAX_COMPOUTLINE         10
#define  MAX_NETS                10000    // 10000 Max number of nets in FILE.  
#define  MAX_COMPONENT           10000
#define  MAX_ATTR                2000

enum AccelLayerTypeTag
{
   accelLayerTypeAllLayers = -1,
   accelLayerTypeNull      = -2,
   accelLayerTypeNonSignal =  0,
   accelLayerTypeSignal    =  1,
   accelLayerTypePlane     =  2,
};

AccelLayerTypeTag stringToAccelLayerType(const CString& layerTypeString);

#define  STAT_UNKNOWN            0
#define  STAT_SHAPE              1
#define  STAT_DESIGN             2

#define  BRK_ROUND               0
#define  BRK_B_ROUND             1

#define  FIDUCIAL_GEOM        1
#define  TOOLING_GEOM         2  

/* Defines Size *****************************************************************/
#define  SIZ_ATTR_UNITS                (sizeof(char *) / sizeof(attr_units))
#define  SIZ_BRK_LST                   (sizeof(brk_lst) / sizeof(List))
#define  SIZ_STR_LST                   (sizeof(str_lst) / sizeof(List))
#define  SIZ_STR_Pass1_LST             (sizeof(str_pass1_lst) / sizeof(List))
#define  SIZ_ASC_LST                   (sizeof(asc_lst) / sizeof(List))
#define  SIZ_LIB_LST                   (sizeof(lib_lst) / sizeof(List))
#define  SIZ_LAYERCONTENTS_LST         (sizeof(layercontents_lst) / sizeof(List))
#define  SIZ_TITLESHEET_LST            (sizeof(titlesheet_lst) / sizeof(List))
#define  SIZ_PADSTYLE_LST              (sizeof(padstyle_lst) / sizeof(List))
#define  SIZ_PADSHAPE_LST              (sizeof(padshape_lst) / sizeof(List))
#define  SIZ_ATTACHEDPATTERN_LST       (sizeof(attachedpattern_lst) / sizeof(List))
#define  SIZ_TEXTSTYLE_LST             (sizeof(textstyle_lst) / sizeof(List))
#define  SIZ_NETNET_LST                (sizeof(netnet_lst) / sizeof(List))
#define  SIZ_FONT_LST                  (sizeof(font_lst) / sizeof(List))
#define  SIZ_NET_LST                   (sizeof(net_lst) / sizeof(List))
#define  SIZ_PCB_LST                   (sizeof(pcb_lst) / sizeof(List))
#define  SIZ_COMPDEF_LST               (sizeof(compdef_lst) / sizeof(List))
#define  SIZ_COMPINST_LST              (sizeof(compinst_lst) / sizeof(List))
#define  SIZ_PCBMULTI_LST              (sizeof(pcbmulti_lst) / sizeof(List))
#define  SIZ_FROMTO_LST                (sizeof(fromto_lst) / sizeof(List))
#define  SIZ_PADPIN_LST                (sizeof(padpin_lst) / sizeof(List))
#define  SIZ_INFOPOINT_LST             (sizeof(infopoint_lst) / sizeof(List))
#define  SIZ_VIA_LST                   (sizeof(via_lst) / sizeof(List))
#define  SIZ_PT_LST                    (sizeof(pt_lst) / sizeof(List))
#define  SIZ_POLY_LST                  (sizeof(poly_lst) / sizeof(List))
#define  SIZ_PCBPOLY_LST               (sizeof(pcbpoly_lst) / sizeof(List))
#define  SIZ_PLANEOBJ_LST              (sizeof(planeobj_lst) / sizeof(List))
#define  SIZ_COPPERPOUR95_LST          (sizeof(copperpour95_lst) / sizeof(List))
#define  SIZ_ISLAND_LST                (sizeof(island_lst) / sizeof(List))
#define  SIZ_CUTOUT_LST                (sizeof(cutout_lst) / sizeof(List))
#define  SIZ_DIMGRAPHICS_LST           (sizeof(dimgraphics_lst) / sizeof(List))
#define  SIZ_DIMGRAPHIC_LST            (sizeof(dimgraphic_lst) / sizeof(List))
#define  SIZ_DIMENSION_LST             (sizeof(dimension_lst) / sizeof(List))
#define  SIZ_GRAPH_LST                 (sizeof(graph_lst) / sizeof(List))
#define  SIZ_PAD_LST                   (sizeof(pad_lst) / sizeof(List))
#define  SIZ_LIBPATTERN_LST            (sizeof(libpattern_lst) / sizeof(List))
#define  SIZ_LIBPATTERNEXTENDED_LST    (sizeof(libpatternextended_lst) / sizeof(List))
#define  SIZ_PCBPATTERN_LST            (sizeof(pcbpattern_lst) / sizeof(List))
#define  SIZ_PCBPATTERNGRAPHICSREF_LST (sizeof(pcbpatterngraphicsref_lst) / sizeof(List))
#define  SIZ_ATTR_LST                  (sizeof(attr_lst) / sizeof(List))
#define  SIZ_PATTERNGRAPHICSDEF_LST    (sizeof(patterngraphicsdef_lst) / sizeof(List))
#define  SIZ_PATTERNMULTI_LST          (sizeof(patternmulti_lst) / sizeof(List))
#define  SIZ_LAYERDEF_LST              (sizeof(layerdef_lst) / sizeof(List))
#define  SIZ_PcbDesignHeader_LST       (sizeof(pcbDesignHeader_lst) / sizeof(List))
#define  SIZ_PCB_Pass1_LST             (sizeof(pcb_pass1_lst) / sizeof(List))
#define  SIZ_POLYCUTOUT_LST            (sizeof(polyCutOutcontents_lst) / sizeof(List))


/* Structures **************************************************************/

typedef struct
{
   double   x;
   double   y;
}Coor;

typedef  struct
{
   char     *net_name;
   int      netnr;
   int      layernumber;      // this is used to mark a PLANE net.
}ACCELNetl;

typedef struct
{
   CString  font;
   double   height;
   double   width;  // stroke line width
}ACCELTextStyle;
typedef CTypedPtrArray<CPtrArray, ACCELTextStyle*> CTextStyleArray;

typedef struct
{
	CString	patternName;			// patterndef name
	CString	graphicName;		// graphic name within a patterndef
   CString  original;         // original name
   CString  geomName;         // name of geometry (block), created by patternName + "_" + graphicName
										// if patternName ends with "_1" then remove it from patternName before creing geomName

   int      pads_flipped;     // this pattern has flipped pads
   int      number_of_pads;
   int      flip_component;   // if all pads are mirrored, I will flip the component vs. individual pads.

	BlockStruct *geomBlock;
}ACCELPatterndef;
typedef CTypedPtrArray<CPtrArray, ACCELPatterndef*> CPatternDefArray;

typedef struct
{
   char     *name;
   int      pincnt;
   char     **pinname;
}ACCELShapeName;

typedef struct
{
   int      pinnum;        // guarantied a pinumber, but for vias is it a MAX_INT
   char     *pindes;       // this is a pinname defined in the pattern
   char     *padstack;
   int      padtype;       // 0 for pin, 1 for via, -1 for already written out
   double   x,y;
   double   rotation;
   int      mirrored;
}ACCELLibpin;

typedef struct
{
   int			pinNumber;                             // this is the ACCEL padNum
	CString		compPinRef;
}ACCELPadPin;
typedef CTypedPtrArray<CPtrArray, ACCELPadPin*> CPadPinArray;

typedef struct
{
	CString			compDefName;	// name of compDef
	CString			original;		// original name of compDef
	CString			patternName;	// reference of pattern name
	CString			patternNum;		// reference of pattern number
	CPadPinArray	padPinArray;
   int				pincnt;
   double			height;			// compdef_attr height
	CString			value;
}ACCELCompdef;
class CCompDefArray : public CTypedPtrArray<CPtrArray, ACCELCompdef*>
{
};

typedef struct
{
	CString	compInst;
	CString	compRef;
	CString	compRefOriginal;
   CString  value;
   double   height;
}ACCELCompInst;
typedef CTypedPtrArray<CPtrArray, ACCELCompInst*> CCompInstArray;

typedef struct
{
   CString  key;
   CString  val;
   double   x;
   double   y;
   double   rotation;
   double   height, width;
   int      visible;
   int      mirror;
   int      justify;
   int      attr_units;
}ACCELAtt;
typedef CTypedPtrArray<CPtrArray, ACCELAtt*> CAttArray;

typedef struct         
{
   CString  accel_name;
   CString  cc_name;
} ACCELAttrmap;
typedef CTypedPtrArray<CPtrArray, ACCELAttrmap*> CAttrmapArray;

typedef  struct
{
   char     *token;
   int      (*function)();
} List;

typedef struct
{
   double d;   
   int    toolindex;
   int  plated;
}AccelDrill;
typedef CArray<AccelDrill, AccelDrill&> CDrillArray;

typedef struct
{
   char     form;
   double   sizeA;
   double   sizeB;
   double   offsetx;
   double   offsety;
   double   rotation;
   int blockNum;
}AccelPadform;
typedef CArray<AccelPadform, AccelPadform&> CPadformArray;

typedef struct
{
   char  *name;
   int   geomType;
} AccelGeomTypeSwap;

/* List **************************************************************/
static char *attr_units[] =
{
   "mil",
   "inch",
   "millimeter",
   "centimeter",
   "micrometer",
   "nanometer",
   "picometer",
   "meter",
   "layername",
   "viastyle",
   "radian",
   "degree",
   "ohm",
   "mho",
   "volt",
   "millivolt",
   "microvolt",
   "nanovolt",
   "picovolt",
   "ampere",
   "milliampere",
   "microampere",
   "nanoampere",
   "picoampere",
   "henry",
   "millihenry",
   "microhenry",
   "nanohenry",
   "picohenry",
   "farad",
   "millifarad",
   "microfarad",
   "nanofarad",
   "picofarad",
   "second",
   "millisecond",
   "microsecond",
   "nanosecond",
   "picosecond",
   "bool",
   "hertz",
   "kilohertz",
   "megahertz",
   "gigahertz",
   "watt",
   "milliwatt",
   "microwatt",
   "nanowatt",
   "picowatt",
   "quantity",
   "string",
};

static int   fnull();
static int   fskip();

/* Start of PDIF file */
int tan_asciiheader();
int tan_library();
int tan_netlist();
int tan_pcbdesign();
int tan_pcbdesign_pass1();

static List str_lst[] =
{
   "asciiHeader",            tan_asciiheader,
   "library",                tan_library,
   "netlist",                tan_netlist,
   "pcbDesign",              tan_pcbdesign,
};

static List str_pass1_lst[] =
{
   "pcbDesign",              tan_pcbdesign_pass1,
};

/* Bracket definitions */
static List  brk_lst[] =
{
   "(",                    FNULL,
   ")",                    FNULL,
};

/* ASCIIHEADER sections */
static int asc_ascversion();
static int asc_program();
static int asc_fileunits();

static List  asc_lst[] =
{
   "asciiVersion",         asc_ascversion,
   "timeStamp",            fskip,
   "program",              asc_program,
   "copyright",            fskip,
   "fileAuthor",           fskip,
   "headerString",         fskip,
   "fileUnits",            asc_fileunits,
   "guidString",           fskip,
};


/* library sections */
static int lib_padstyledef();
static int lib_viastyledef();
static int lib_textstyledef();
static int lib_patterndef();
static int lib_patterndefextended();
static int lib_compdef();

static List lib_lst[] =
{
   "PadStyleDef",          lib_padstyledef,
   "ViaStyleDef",          lib_viastyledef,
   "TextStyleDef",         lib_textstyledef,
   "PatternDef",           lib_patterndef,
   "PatternDefExtended",   lib_patterndefextended,
   "CompDef",              lib_compdef,
};

/* layercontents sections  */
static int tan_layernumref();
static int tan_line();
static int tan_text();
static int tan_arc();            // poly[0] = center, poly[1].x = radius, poly[2].x = startangle, poly[2].y = sweepangle
static int tan_triple_pt_arc();
static int tan_poly();
static int tan_polykeepout();
static int tan_attr();           // this skips it.
static int tan_dimension();
static int tan_planeobj();
static int tan_copperpour95();
static int tan_attribute();      // this does it
static int tan_pcbpoly();
static int tan_planeoutline();
static int tan_poly_cut_out();

static List layercontents_lst[] =
{
   "layerNumRef",          tan_layernumref,
   "line",                 tan_line,
   "text",                 tan_text,
   "arc",                  tan_arc,
   "triplePointArc",       tan_triple_pt_arc,
   "poly",                 tan_poly,
   "dimension",            tan_dimension,
   "polyCutOut",           tan_poly_cut_out,
   "polyKeepOut",          tan_polykeepout,
   "lineKeepOut",          fskip,
   "planeObj",             tan_planeobj,
   "attr",                 tan_attribute,
   "copperPour",           fskip,
   "copperPour95",         tan_copperpour95,
   "field",                fskip,
   "table",                fskip,
   "pcbpoly",              tan_pcbpoly,
   "pickPoint",            fskip,         // p-cad 2001
   "gluePoint",            fskip,         // p-cad 2001
};

/* titlesheet section */
static List titlesheet_lst[] =
{
   "line",                 tan_line,
   "text",                 tan_text,
   "arc",                  tan_arc,
	"triplePointArc",       tan_triple_pt_arc,
   "poly",                 tan_poly,
   "dimension",            tan_dimension,
   "attr",                 tan_attribute,
};

/* padStyle section */
static int padstyle_hole(), padstyle_isplated();
static int padstyle_holeoffset();
static int padstyle_padshape();
static int padstyle_localswell();

static List padstyle_lst[] =
{
   "holeDiam",             padstyle_hole,
   "isHolePlated",         padstyle_isplated,
   "holeOffset",           padstyle_holeoffset,
   "StartRange",           fskip,
   "EndRange",             fskip,
   "padShape",             padstyle_padshape,
   "viaShape",             padstyle_padshape,
   "useGlobalSwell",       fskip,
   "localSwell",           padstyle_localswell,
};

/* padShape section */
static int tan_height(), tan_width(), tan_layertype();
static int padstyle_padshape(), padstyle_padshapetype();
static int padstyle_shapeoutline();

static List padshape_lst[] =
{
   "layerNumRef",          tan_layernumref,
   "layerType",            tan_layertype,
   "padShapeType",         padstyle_padshapetype,
   "viaShapeType",         padstyle_padshapetype,
   "shapeOutline",         padstyle_shapeoutline,
   "shapeWidth",           tan_width,
   "shapeHeight",          tan_height,
   "outsideDiam",          tan_width,
   "insideDiam",           tan_height,
   "spokeWidth",           fskip,
};

/* attached pattern section */
static int attached_patternNum();
static int attached_patternname();
static int pattern_padpinmap();
static int pattern_numpads();

static List attachedpattern_lst[] =
{
   "patternNum",           attached_patternNum,
   "patternName",          attached_patternname,
   "numPads",              pattern_numpads,
   "padPinMap",            pattern_padpinmap,
};

/* textstyle section. */
static int textstyle_font();

static List textstyle_lst[] =
{
   "font",              textstyle_font,
   "textStyleAllowTType",     fskip,
   "textStyleDisplayTType",   fskip,
};

/* net net section. */
static int netnet_node();
static int netnet_isplane();
static int netnet_attribute();

static List netnet_lst[] =
{
   "node",              netnet_node,
   "isPlane",           netnet_isplane,
   "isVisible",         fskip,
   "isVisibleOnDrag",   fskip,
   "attr",              netnet_attribute,
   "planeColor",        fskip,
};

static int font_type();
static int font_family();
static int font_face();

/* font section */
static List font_lst[] =
{
   "fontType",          font_type,
   "fontFamily",        font_family,
   "fontFace",          font_face,
   "fontHeight",        tan_height,
   "strokeWidth",       tan_width,
   "fontWeight",        fskip,
   "fontCharSet",       fskip,
   "fontOutPrecision",  fskip,
   "fontClipPrecision", fskip,
   "fontQuality",       fskip,
   "fontPitchAndFamily",fskip,
};

/* netlist sections */
static int net_compinst();
static int net_net();

static List net_lst[] =
{
   "compInst",          net_compinst,
   "net",               net_net,
   "netClass",          fskip,
   "classToClassRules", fskip,
   "globalAttrs",       fskip,
};

/* pcbdesign sections */

static int pcb_pcbdesignheader();
static int pcb_pcblayerdef();
static int pcb_pcblayerdef_pass1();
static int pcb_pcbmultilayer();
static int tan_layercontents();
static int pcb_pcbprintsettings();
static int pcb_drillsymsettings();
static int pcb_gerbersettings();
static int pcb_ncdrillsettings();
static int pcb_programstate();
static int pcb_layercontents();

static List pcb_lst[] =
{
   "layerDef",          pcb_pcblayerdef,
   "multiLayer",        pcb_pcbmultilayer,
   "layerContents",     pcb_layercontents,
   "pcbPrintSettings",  pcb_pcbprintsettings,
   "drillSymSettings",  pcb_drillsymsettings,
   "gerberSettings",    pcb_gerbersettings,
   "ncDrillSettings",   pcb_ncdrillsettings,
   "programState",      pcb_programstate,
   "layerSets",         fskip,
   "reportSettings",    fskip,
};

static List pcb_pass1_lst[] =
{
   "pcbDesignHeader",   pcb_pcbdesignheader,
   "layerDef",          pcb_pcblayerdef_pass1,
};

/* pcbDesignHeader sections */

static int pcbDesignHeader_pasteSwell();
static int pcbDesignHeader_solderSwell();
static int pcbDesignHeader_planeSwell();

static List pcbDesignHeader_lst[] =
{
   "pasteSwell",   pcbDesignHeader_pasteSwell,
   "solderSwell",  pcbDesignHeader_solderSwell,
   "planeSwell",   pcbDesignHeader_planeSwell,
};

/* compdef sections */
static int tan_originalname();
static int compdef_compheader();
static int compdef_comppin();
static int compdef_attachedpattern();
static int compdef_attr();

static List compdef_lst[] =
{
   "originalName",         tan_originalname,
   "compHeader",           compdef_compheader,
   "compPin",              compdef_comppin,
   "attachedPattern",      compdef_attachedpattern,
   "attachedSymbol",       fskip,
   "attr",                 compdef_attr,
};

/* compinst section */
static int tan_compref();
static int tan_compvalue();
static int compinst_attr();

static List  compinst_lst[] =
{
   "originalName",         tan_originalname,
   "compRef",              tan_compref,
   "compValue",            tan_compvalue,
   "attr",                 compinst_attr,
};

/* pcbmultilayer sections */
static int pcbmulti_pattern();
static int pcbmulti_fromto();
static int pcbmulti_via();
static int pcbmulti_pad();
static int pcbmulti_polykeepout();
static int pcbmulti_linekeepout();
static int pcbmulti_infopoint();

static List pcbmulti_lst[] =
{
   "pattern",              pcbmulti_pattern,
   "fromTo",               pcbmulti_fromto,
   "via",                  pcbmulti_via,
   "pad",                  pcbmulti_pad,
   "polyKeepOut",          pcbmulti_polykeepout,
   "lineKeepOut",          pcbmulti_linekeepout,
   "infoPoint",            pcbmulti_infopoint,
   "refPoint",             fskip,
   "pickpoint",            fskip,
   "gluepoint",            fskip,
};

/* fromto sections */
static int tan_netnameref();
static int tan_pt();

static List fromto_lst[] =
{
   "netNameRef",           tan_netnameref,
   "pt",                   tan_pt,
};

/* padpin sections.    */
static int padpin_padnum();
static int padpin_comppinref();

static List padpin_lst[] =
{
   "padNum",               padpin_padnum,
   "compPinRef",           padpin_comppinref,
};

/* infoPoint section */
static List infopoint_lst[] =
{
   "pt",                      fskip,
   "number",                  fskip,
   "desc",                    fskip,
   "infoPointRuleCategory",   fskip,
   "infoPointViolationType",  fskip,
};

/* via sections */
static int tan_viastyleref();
static int tan_rotation();
static int tan_isflipped();

static List via_lst[] =
{
   "viaStyleRef",          tan_viastyleref,
   "pt",                   tan_pt,
   "netNameRef",           tan_netnameref,
   "rotation",             tan_rotation,
   "isFlipped",            tan_isflipped,
};

/* poly pt sections.    */
static List  pt_lst[] =
{
   "pt",                   tan_pt,
};

/* poly pt sections.    */
static int poly_pcbpoly();

static List poly_lst[] =
{
   "pt",                   tan_pt,
   "netNameRef",           tan_netnameref,
   "poly",                 tan_poly,
   "pcbpoly",              poly_pcbpoly,
};

/* pcbpoly pt sections.    */
static List pcbpoly_lst[] =
{
   "pt",                   tan_pt,
   "netNameRef",           tan_netnameref,
   "fillets",              fskip,
};

/* planeobj section */
static List planeobj_lst[] =
{
   "width",                tan_width,
   "netNameRef",           tan_netnameref,
   "pcbpoly",              tan_pcbpoly,
   "planeOutline",         tan_planeoutline,
};

/* copperpour95 sections.    */
static int tan_island();
static int tan_boundaryPoly();

static List copperpour95_lst[] =
{
   "pourType",             fskip,
   "pourSpacing",          fskip,
   "pourBackoff",          fskip,
   "pourSmoothness",       fskip,
   "islandRemoval",        fskip,
   "thermalType",          fskip,
   "thermalWidth",         tan_width,
   "thermalSpokes",        fskip,
   "netNameRef",           tan_netnameref,
   "pourOutline",          tan_boundaryPoly,
   "pcbpoly",              tan_boundaryPoly,
   "island",               tan_island, // outline and cutout
   "width",                fskip,
   "useDesignRules",       fskip,
};

/* island section */
static int island_islandoutline();
static int island_cutout();
static int island_thermal();

static List island_lst[] =
{
   "islandOutline",        island_islandoutline,
   "cutout",               island_cutout, // cutout outline
   "thermal",              island_thermal,
};

/* cutout section */
static int cutout_cutoutoutline();

static List cutout_lst[] =
{
   "cutoutOutline",        cutout_cutoutoutline,
};

/* dimgraphics section */
static int dimgraphics_dimgraphic();

static List dimgraphics_lst[] =
{
   "dimgraphics",          dimgraphics_dimgraphic,
};

/* dimgraphic section */
static   int   dimgraphic();

static List dimgraphic_lst[] =
{
   "dimgraphic",           dimgraphic,
};


/* dimension pt section */
static int dimgraphics();

static List dimension_lst[] =
{
   "pt",                   fskip,
   "dimStyle",             fskip,
   "rotation",             fskip,
   "isFlipped",            fskip,
   "dimOrient",            fskip,
   "dimTextOrient",        fskip,
   "dimPrecision",         fskip,
   "dimDisplayUnits",      fskip,
   "dimUnits",             fskip,
   "dimLineWidth",         fskip,
   "dimLeaderStyle",       fskip,
   "dimLeaderSize",        fskip,
   "dimCenterSize",        fskip,
   "dimPlusLinearTol",     fskip,
   "dimMinusLinearTol",    fskip,
   "dimPlusDegTol",        fskip,
   "dimMinusDegTol",       fskip,
   "dimShowTolerance",     fskip,
   "dimShowDiaSymbol",     fskip,
   "dimDimLineGraphics",   dimgraphics,
   "dimExtLineGraphics",   dimgraphics,
   "dimTextGraphics",      dimgraphics,
   "dimPoints",            fskip,
   "arrowheadWidth",       fskip,
   "arrowheadHeight",      fskip,
   "dimensionID",          fskip,
   "dimensionOffsets",     fskip,
};

/* graphic sections.    */
static int tan_width();
static int tan_pt();
static int tan_radius();
static int tan_startangle();
static int tan_sweepangle();
static int tan_textstyleref();
static int tan_extent();
static int tan_justify();

static List  graph_lst[] =
{
   "width",                tan_width,
   "pt",                   tan_pt,
   "netNameRef",           tan_netnameref,
   "radius",               tan_radius,
   "startAngle",           tan_startangle,
   "sweepAngle",           tan_sweepangle,
   "textStyleRef",         tan_textstyleref,
   "rotation",             tan_rotation,
   "extent",               tan_extent,
   "justify",              tan_justify,
   "isFlipped",            tan_isflipped,
   "dimensionRef",         fskip,
   "style",                fskip,
};

/* pad sections.    */
static int pad_padNum();
static int pad_defaultPinDes();
static int pad_padStyleRef();
static int pad_rotation();

static List pad_lst[] =
{
   "padNum",               pad_padNum,
   "padStyleRef",          pad_padStyleRef,
   "viaStyleRef",          pad_padStyleRef,
   "pt",                   tan_pt,
   "rotation",             pad_rotation,
   "isFlipped",            tan_isflipped,
   "netNameRef",           tan_netnameref,
   "defaultPinDes",        pad_defaultPinDes,
   "dimensionRef",         fskip,
};

/* library pattern sections.    */
static int pattern_multi();

static List libpattern_lst[] =
{
   "originalName",         tan_originalname,
   "multiLayer",           pattern_multi,
   "layerContents",        tan_layercontents,
};

/* libpatternextended section */
static int libpatternextended_originalname();
static int libpatternextended_patterngraphicsnameref();
static int libpatternextended_patterngraphicsdef();

static List libpatternextended_lst[] =
{
   "originalName",            tan_originalname,
   "patternGraphicsNameRef",  libpatternextended_patterngraphicsnameref,
   "patternGraphicsDef",      libpatternextended_patterngraphicsdef,
};

/* comp pattern section */
static int tan_patternref();
static int tan_refdesref();
static int tan_isfixed();
static int pattern_attribute();
static int pattern_patterngraphicsnameref();
static int pattern_patterngraphicsref();

static List pcbpattern_lst[] =
{
   "patternRef",           tan_patternref,
   "refDesRef",            tan_refdesref,
   "pt",                   tan_pt,
   "attr",                 pattern_attribute,
   "rotation",             tan_rotation,
   "isFlipped",            tan_isflipped,
   "isFixed",              tan_isfixed,
   "patternGraphicsNameRef", pattern_patterngraphicsnameref,
   "patternGraphicsRef",   pattern_patterngraphicsref
};

/* pcbpatterngraphicsref section */
static List  pcbpatterngraphicsref_lst[] =
{
   "patternGraphicsNameRef",  pattern_patterngraphicsnameref,
   "attr",                    pattern_attribute,
};

/* attr section */
static int attr_pt();
static int attr_rotation();
static int attr_isflipped();
static int attr_isvisible();
static int attr_textstyleref();
static int attr_justify();
static int attr_constraintunits();

static List attr_lst[] =
{
   "pt",                   attr_pt,
   "rotation",             attr_rotation,
   "isFlipped",            attr_isflipped,
   "isVisible",            attr_isvisible,
   "textStyleRef",         attr_textstyleref,
   "justify",              attr_justify,
   "constraintUnits",      attr_constraintunits,
};

/* patterngraphicsdef section */
static int libpatternextended_patterngraphicsnamedef();

static List patterngraphicsdef_lst[] =
{
   "multiLayer",              pattern_multi,
   "layerContents",           tan_layercontents,
   "patternGraphicsNameDef",  libpatternextended_patterngraphicsnamedef,
};

/* patternmultilayer sections */
static int patternmulti_pad();
static int patternmulti_via();

static List patternmulti_lst[] =
{
   "pad",                  patternmulti_pad,
   "via",                  patternmulti_via,
   "polyKeepOut",          pcbmulti_polykeepout,
   "lineKeepOut",          pcbmulti_linekeepout,
   "gluepoint",            fskip,
   "pickpoint",            fskip,  
   "gluepoint",            fskip,  
};

/* layerdef in pcbDesign section. */
static int layerdef_layertype();
static int layerdef_pad2padclear();
static int layerdef_pad2lineclear();
static int layerdef_line2lineclear();
static int layerdef_titlesheet();
static int layerdef_layernumref();
static int layerdef_netnameref();

static List layerdef_lst[] =
{
   "layerNum",             layerdef_layernumref,
   "layerType",            layerdef_layertype,
   "layerBias",            fskip,
   "attr",                 fskip,
   "netNameRef",           layerdef_netnameref,
   "padToPadClearance",    layerdef_pad2padclear,
   "padToLineClearance",   layerdef_pad2lineclear,
   "lineToLineClearance",  layerdef_line2lineclear,
   "viaToPadClearance",    fskip,
   "viaToLineClearance",   fskip,
   "viaToViaClearance",    fskip,
   "fieldSetRef",          fskip,
   "titleSheet",           layerdef_titlesheet,
};

static List polyCutOutcontents_lst[] =
{
   "pcbPoly",              tan_pcbpoly,
};

////_____________________________________________________________________________
//class CAccelDefaultLayer
//{
//private:
//   int           m_layerIndex;
//   CString       m_name; // layername
//   int           m_mirrorIndex;
//   LayerTypeTag  m_type; // layer attributes as defined in dbutil.h
//
//public:
//   CAccelDefaultLayer(int layerIndex);
//
//   int getLayerIndex() const { return m_layerIndex; }
//
//   CString getName() const { return m_name; }
//   void setName(const CString& name) { m_name = name; }
//
//   int getMirrorIndex() const { return m_mirrorIndex; }
//   void setMirrorIndex(int index) { m_mirrorIndex = index; }
//
//   LayerTypeTag getType() const { return m_type; }
//   void setType(LayerTypeTag type);
//
//   void dump(CWriteFormat& writeFormat,int depth) const;
//};
//
//class CAccelDefaultLayerArray
//{
//private:
//   CTypedPtrArrayContainer<CAccelDefaultLayer*> m_layers;
//
//public:
//   CAccelDefaultLayerArray();
//   void empty() { m_layers.empty(); }
//
//   int getSize() const { return m_layers.GetSize(); }
//   CAccelDefaultLayer* getAt(int layerIndex);
//   CAccelDefaultLayer* getAt(const char* layerName);
//   LayerStruct* getLayerWithType(LayerTypeTag layerType);
//   int getLayerIndex(const char* layerName);
//   CAccelDefaultLayer* add();
//
//   void dump(CWriteFormat& writeFormat,int depth) const;
//};

//_____________________________________________________________________________
class CAccelLayer
{
private:
   int          m_id;
   CString      m_name;
   int          m_number;
   AccelLayerTypeTag m_accelLayerType;
   bool         m_used;
   int          m_physicalNumber;
   CString      m_netName;
   mutable LayerStruct* m_camCadLayer;

public:
   CAccelLayer(int id);

   CString getName() const { return m_name; }
   void setName(const CString& name);

   CString getNetName() const { return m_netName; }
   void setNetName(const CString& netName);

   int getNumber() const { return m_number; }
   void setNumber(int number) { m_number = number; }

   int getPhysicalNumber() const { return m_physicalNumber; }
   void setPhysicalNumber(int number) { m_physicalNumber = number; }

   AccelLayerTypeTag getAccelLayerType() const { return m_accelLayerType; }
   void setAccelLayerType(AccelLayerTypeTag type);
   void setAccelLayerType(const CString& layerTypeString);

   LayerTypeTag getLayerType() const { return getDefinedCamCadLayer().getLayerType(); }
   void setLayerType(LayerTypeTag layerType) { getDefinedCamCadLayer().setLayerType(layerType); }

   bool getUsed() const { return m_used; }
   void setUsed(bool flag) { m_used = flag; }

   LayerStruct& getDefinedCamCadLayer() const;

   void dump(CWriteFormat& writeFormat,int depth) const;
};

//_____________________________________________________________________________
class CAccelLayerArray
{
private:
   CTypedPtrArrayContainer<CAccelLayer*> m_layers;

public:
   CAccelLayerArray();
   void empty() { m_layers.empty(); }

   int getSize() const { return m_layers.GetSize(); }
   CAccelLayer* getAt(int layerIndex);
   CAccelLayer* add();

   void dump(CWriteFormat& writeFormat,int depth) const;
};

//_____________________________________________________________________________
class CAccelLayers
{
private:
   CAccelLayerArray m_accelLayers;
   CTypedPtrArray<CPtrArray,CAccelLayer*>* m_accelLayerArray;
   CTypedPtrMap<CMapStringToPtr,CString,CAccelLayer*>* m_accelLayerMap;
   CAccelLayer* m_currentAccelLayer;
   CMapStringToString m_defaultLayerMap;
   CMapStringToString m_mirroredLayerMap;
   LayerStruct* m_allLayer;
   LayerStruct* m_floatingLayer;
   LayerStruct* m_signalLayer;
   LayerStruct* m_planeLayer;
   LayerStruct* m_pasteTopLayer;
   LayerStruct* m_pasteBottomLayer;
   LayerStruct* m_maskTopLayer;
   LayerStruct* m_maskBottomLayer;

public:
   CAccelLayers();
   ~CAccelLayers();
   void empty();

   int getSize() const { return m_accelLayers.getSize(); }
   CAccelLayer* getAt(int layerIndex) { return m_accelLayers.getAt(layerIndex); }

   void addDefaultLayer(const CString& accelLayerName,const CString& layerTypeString);
   void addMirroredLayers(CString layerName1,CString layerName2);
   CAccelLayer& getCurrentAccelLayer();
   void addAccelLayer(const CString& accelLayerName);
   void instantiateCamCadLayers(CCEtoODBDoc& camCadDoc);

   LayerStruct& getLayer(int layerNumber);
   LayerStruct& getLayer(const CString& layerName);
   LayerStruct& getAllLayer();
   LayerStruct& getFloatingLayer();
   LayerStruct& getSignalLayer();
   LayerStruct& getPlaneLayer();
   LayerStruct& getPasteTopLayer();
   LayerStruct& getPasteBottomLayer();
   LayerStruct& getMaskTopLayer();
   LayerStruct& getMaskBottomLayer();
   void syncAccelLayerArray();
   void syncAccelLayerMap();
   void releaseAccelLayerArray();
   void releaseAccelLayerMap();
};

//_____________________________________________________________________________
class CAccelPadDefState
{
private:
   static CAccelPadDefState* m_accelPadDefState;

   double m_pasteSwell;
   double m_solderSwell;
   double m_planeSwell;
   double m_localSwell;

   CDataList m_pads;
   CString m_padstackName;

public:
   CAccelPadDefState();

   void initialize();

   double getPasteSwell() { return m_pasteSwell; }
   void setPasteSwell(double swell);

   double getSolderSwell() { return m_solderSwell; }
   void setSolderSwell(double swell);

   double getPlaneSwell();
   void setPlaneSwell(double swell);
   void setLocalSwell(double swell);

   CString getPadstackName() { return m_padstackName; }
   void setPadstackName(const CString& name) { m_padstackName = name; }

   void addPad(DataStruct* pad);

   void instantiateImpliedPads(bool topPadFlag,bool bottomPadFlag);
   BlockStruct* getApertureBlock(CStandardAperture& padAperture);
   //void instantiateImpliedPads(bool topFlag);

   static CAccelPadDefState& getAccelPadDefState();
   static void releaseAccelPadDefState();
   CString createComplexExtendedPad(BlockStruct* const block, double swell );
};

