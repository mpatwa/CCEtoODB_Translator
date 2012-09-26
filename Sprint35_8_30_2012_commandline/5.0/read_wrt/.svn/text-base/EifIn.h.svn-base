// $Header: /CAMCAD/4.5/read_wrt/EifIn.h 12    10/18/05 8:13p Kurt Van Ness $

/****************************************************************************/
/*  
	Project CAMCAD                                     
	Router Solutions Inc.
	Copyright © 1994-2000. All Rights Reserved.
*/           

#pragma once

/****************************************************************************/
/*
   Library for reading EIF databases

   if a V14 cor file is found -> take this.

   the following files are expected
   inch     cel  // shape definitions
   inch     clr  // clearance file -> not yet supported
   inch     cor  // correlation file
   inch     des  // design
   inch     nnn  // keyin netlist -> can be changed in TEC with .NETLISTEX...
   inch     pad  // padgeometry definition
   inch     pkg  // package file
   inch     stk  // padstack
   inch     tec  // technology, layerstacking

   Not implemented:
   Default trace width
   Text justifications

   Triangle graphic (padstack) are treated as Rectangle.
   Complex shape
   Ellipse
   Rotated pins   = only 90 degree rotation allowed
   local PART_DEF under PART_INSTANCE in design file.

   .PART_INSTANCE UNUSED will be skipped.

*/
/****************************************************************************/

/* Version history ***********************************************************/
/*
   Version  Date        Remarks
   2        28-Dec-94   Major rewrite of the scanner.
                        All reported error done reported by MITRON and
                        QUANTIC.
   3        11-Mar-95   Fixed for Quad:
                        - More rugged scanning
                        - Pinnames
                        - Netnames with blanks
   4        10-Jun-95   Fixed multiple test examples.
   5        13-Sep-95   Fixes again for DOUG.
   6        10-Oct-95   Fixes again for Doug   
   7        17-Nov-95   Cmplx shapes in cel amd des.
                        Pad forms square + oval fixed.
   8        13-Jan-96   Update for Version 14
   9        26-Apr-96   SMD Pins, xoff, yoff for pins.
   10       21-Jun-96   More completion.
                        Metric support 13-Aug-96
   11       11-May-97   Inch units support.
*/

 
/* Define Section ***********************************************************/

#define  VERSION						11
#define  TRUE							1
#define  FALSE							0

#define	EIFERR						"eif.log"

#define  MAX_LAYERS					255   /* Max number of layers.   */
#define  MAX_LINE						255
#define  MAX_POLY						1000

#define  MAX_CURPAD					100   // stackup for the current pad
#define  MAX_LOCALPAD				1000  // list of current pads.

#define  WORD_DELIMETER				" \t\n\r"
#define  TEXT_DELIMETER				"\""
#define  NONET							"NO_NET"

#define	TEXT_CORRECT				0.8

#undef FILE_UNKNOWN   // in winbase.h "#define FILE_UNKNOWN                    5"

#define  FILE_UNKNOWN				0
#define  FILE_ASSM_OUTL				2
#define  FILE_ROUTE_OUTL			3
#define  FILE_BOARD_OUTL			4
#define  FILE_TRACE					5
#define  FILE_POS_PLANE				6
#define  FILE_CONDUCTIVE_ARTWORK 7
#define  FILE_LINE					8
#define  FILE_ROUTE_OBS				9
#define  FILE_PLACE_OBS				10
#define  FILE_CONTOUR				11
#define  FILE_PLANE_OUTL			12
#define  FILE_VIA_OBS				13
#define  FILE_TRACE_OBS				14
#define  FILE_CMPLX_SHAPE			15
#define  FILE_CMPLX_LINE			16
#define  FILE_SILK_ARTWORK			17
#define  FILE_SHAPE					18
#define  FILE_PLANE_HOLE			19
#define  FILE_PACK_OUTL				20

#define	T_TRIANGLE					-1
#define	T_PLUS						-2

#define  PART_UNKNOWN				0
#define  PART_VIA						1
#define  PART_PART					2
#define  PART_SMD						3
#define  PART_CELL					4
#define  PART_EDGE_CONNECTOR		5
#define  PART_MECHANICAL			6
#define  PART_TESTPAD				7

#define  SMD_TOP						"TOP_SMD"		// this has never mirror
#define  SMD_BOT						"BTM_SMD"		// this is always mirror
#define  PAD_TOP						"TOP_PAD"
#define  PAD_BOT						"BTM_PAD"
#define  PAD_INT						"INT_CON_PAD"

#define  PST_UNKNOWN					0
#define  PST_CROSS_HAIR				1
#define  PST_ROUND					2
#define  PST_SQUARE					3

#define  PST_MAX						4  // needed for padoverwrite

#define	FILETYPE_UNKNOWN			0
#define	FILETYPE_PAD				1
#define	FILETYPE_PADSTACK			2
#define	FILETYPE_CELL				3
#define	FILETYPE_DESIGN			4
#define	FILETYPE_TECHNOLOGY		5
#define	FILETYPE_OLDNETLIST		6
#define	FILETYPE_NETLIST			7		// netlist has /name/
#define	FILETYPE_SCINETLIST		8		
#define	FILETYPE_CORRESPONDENCE	9

#define  SIZ_FILE_LST            (sizeof(file_lst) / sizeof(List))
#define  SIZ_TECH_LST            (sizeof(tech_lst) / sizeof(List))
#define  SIZ_VIA_PACKAGE_LST		(sizeof(via_package_lst) / sizeof(List))
#define  SIZ_LAYSTACK_LST        (sizeof(laystack_lst) / sizeof(List))
#define  SIZ_LAY_LST             (sizeof(lay_lst) / sizeof(List))
#define  SIZ_PAD_LST             (sizeof(pad_lst) / sizeof(List))
#define  SIZ_STK_LST             (sizeof(stk_lst) / sizeof(List))
#define  SIZ_CEL_LST             (sizeof(cel_lst) / sizeof(List))
#define  SIZ_GRAPH_LST				(sizeof(graph_lst) / sizeof(List))
#define  SIZ_OUTLINE_LST         (sizeof(outline_lst) / sizeof(List))
#define  SIZ_CELPART_LST         (sizeof(celpart_lst) / sizeof(List))
#define  SIZ_CELVIA_LST				(sizeof(celvia_lst) / sizeof(List))
#define  SIZ_CELPIN_LST          (sizeof(celpin_lst) / sizeof(List))
#define  SIZ_DESPIN_LST          (sizeof(despin_lst) / sizeof(List))
#define  SIZ_CELPAD_LST          (sizeof(celpad_lst) / sizeof(List))
#define  SIZ_PADSTACK_LST        (sizeof(padstack_lst) / sizeof(List))
#define  SIZ_GEOM_LST				(sizeof(geom_lst) / sizeof(List))
#define  SIZ_OUTL_LST				(sizeof(outl_lst) / sizeof(List))
#define  SIZ_CONTOUR_LST         (sizeof(contour_lst) / sizeof(List))
#define  SIZ_REFDES_LST          (sizeof(refdes_lst) / sizeof(List))
#define  SIZ_GRAPHLINE_LST       (sizeof(graphline_lst) / sizeof(List))
#define  SIZ_GRAPHSHAPE_LST      (sizeof(graphshape_lst) / sizeof(List))
#define  SIZ_GRAPHTEXT_LST       (sizeof(graphtext_lst) / sizeof(List))
#define  SIZ_GRAPHARC_LST        (sizeof(grapharc_lst) / sizeof(List))
#define  SIZ_GRAPHCIRCLE_LST     (sizeof(graphcircle_lst) / sizeof(List))
#define  SIZ_TEXT_LST				(sizeof(text_lst) / sizeof(List))
#define  SIZ_DES_LST             (sizeof(des_lst) / sizeof(List))
#define  SIZ_BOARD_ORG_LST       (sizeof(board_org_lst) / sizeof(List))
#define  SIZ_PART_INSTANCE_LST   (sizeof(part_instance_lst) / sizeof(List))
#define  SIZ_VIA_INSTANCE_LST		(sizeof(via_instance_lst) / sizeof(List))
#define  SIZ_MH_INSTANCE_LST		(sizeof(mh_instance_lst) / sizeof(List))
#define  SIZ_FIDUCIAL_INSTANCE_LST   (sizeof(fiducial_instance_lst) / sizeof(List))
#define  SIZ_PART_DEF_LST			(sizeof(part_def_lst) / sizeof(List))
#define  SIZ_TRACE_INSTANCE_LST   (sizeof(trace_instance_lst) / sizeof(List))
#define  SIZ_CORRESPONDENCE_LST   (sizeof(correspondence_lst) / sizeof(List))


/* Struct Section **********************************************************/

typedef struct			  
{
	CString	cellname;	// because a cellname can be like a padname, map it
	CString	vianame;
} EIFViaCell;
typedef CTypedPtrArray<CPtrArray, EIFViaCell*> CViaCellArray;

typedef struct			  
{
	CString	pinname;
	int		pinnr;
} EIFPartPin;
typedef CTypedPtrArray<CPtrArray, EIFPartPin*> CPartPinArray;

typedef struct 
{
   double	x;
   double	y;
	double	bulge;
} EIFPoly;

typedef struct
{
	double	d;
	int		toolindex;
}EIFDrill;
typedef CArray<EIFDrill, EIFDrill&> CDrillArray;

typedef struct
{
   char		*name;
   int		physnr;
	int		mirror;
	int		used;
} Eiflayer;

typedef struct
{
   CString	name;
   int		attr;
} Adef;

typedef  struct
{
   char		*token;
   int		(*function)();
} List;

typedef struct
{
   int		from_padlistptr; // index into padlist
   double	rotation;
   int		layerptr;
   char		padgeomname[80];
   double	x,y;
   double	drill;
}EIFCurPad;

typedef struct
{
	CString	shapename;
	CString	padname;
}EIFVia;

typedef struct
{
   int		 curpadcnt;
   EIFCurPad *curpad;
}Localpad;
	  

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
   char		curline[MAX_LINE];   // this is the current line
   char		name[80];
   char		name_1[80]; // this is for part number
   char		pinname[80];
   char		symbol[80]; // this is a childname to name i.e Padtstack to cell etc..
   char		level[80];
   int		levelnr;    // pointer into eiflayer list.
   int		lyrnr;
	int		push_tok;
   int		geomtyp;    // geomtype or parttype
   int		pincnt;
   char		mirror;     // mirror 
	char		sidebot;		// side - is not mirror
   char		padstackshape;
   int		notplated;
   int		noname_refdes;	// for parts, which have no reference designator
   double	xdim,ydim,xoff,yoff;
   double	diameter;  // diameter or width
   double	radius;    // circle and arcs
   double	height;
   double	rotation;  // in degree
	double	startangle, deltaangle;	// in degree
   //
	int		filetype;
	char		filetypename[80];
}EIFGlobal;


/* List Section ***********************************************************/

/* file_lst Section */
static int eif_filetype();
static int eif_version();
static int eif_creator();
static int eif_skip();
static int eif_units();

static List	file_lst[] = 
{
   "FILETYPE",         eif_filetype,
   "VERSION",          eif_version,
   "CREATOR",          eif_creator,
   "JOB",              eif_skip,
   "DATE",             eif_skip,
   "TIME",             eif_skip,
   "UNITS",            eif_units,
};

/* tech_lst Section */
static int tech_num_layers();
static int tech_default_trc_wdt();
static int tech_default_via();
static int tech_layer_stackup();
static int tech_via_package();

static List tech_lst[] =
{     
   "NUM_LAYERS",			tech_num_layers,
   "START_LAYER",			eif_skip,
   "ALLOW_VIAS",			eif_skip,
   "ALLOW_BBVIAS",		eif_skip,
   "DEFAULT_TRC_WDT",	tech_default_trc_wdt,
   "DEFAULT_VIA",			tech_default_via,
   "LAYER_STACKUP",		tech_layer_stackup,
   "VIA_PACKAGE",			tech_via_package,
   "ROUTING_T",			eif_skip,
   "ANNULUS",				eif_skip,
};

/* via_package_lst Section */
static List via_package_lst[] =
{
   "ALLOW_SMD",			eif_skip,
   "ALLOW_AXIAL",			eif_skip,
   "ALLOW_OTHER",			eif_skip,
};

/*	laystack_lst Section */
static int laystack_layer_num();

static List laystack_lst[] =
{
   "LAYER_NUM",			laystack_layer_num,
};

/*	lay_lst Section */
static int eif_layer();

static List lay_lst[] =
{
   "NAME",					eif_layer,
   "ROUTING",				eif_skip,
   "TRC_LEVEL_NUM",		eif_skip,
   "NUM_BBVIAS",			eif_skip,
   "PAD_LEVEL_NUM",		eif_skip,
};								

/* pad_lst Section */
static int pad_pad();

static List pad_lst[] =
{
   "PAD",					pad_pad,
};

/* stk_lst Section */
static int stk_padstack();

static List stk_lst[] =
{
   "PADSTACK",				stk_padstack,
};

/* cel_lst Section */
static int cel_part();
static int cel_mhshape();
static int cel_via();
static int cel_cell();

static List cel_lst[] =
{
   "PART",					cel_part,
   "MH_SHAPE",				cel_mhshape,
   "VIA",					cel_via,
   "CELL",					cel_cell,
};								

/* graph_lst Section */
// arc, line, circle as part of outline.
static int outl_line();
static int outl_line_obs();
static int outl_arc();
static int outl_circle();
static int graph_line_obs();
static int graph_text();
static int eif_lvl();
static int graph_outl();
static int graph_shape();

static List graph_lst[] =
{
   "LINE",					outl_line,	// this draws
   "LINE_OBS",				graph_line_obs,
   "ARC",					outl_arc,
   "CIRCLE",				outl_circle,
   "TEXT",					graph_text,
   "LVL",					eif_lvl,
   "ASSM_OUTL",			graph_outl,
   "SILK_OUTL",			graph_outl,
   "PACK_OUTL",			graph_outl,
   "SHAPE",					graph_shape,
};
 
/* outline_lst Section */
static int outl_side();
static int eif_diameter();
static int outl_outline();
static int poly_line();
static int poly_arc();
static int poly_circle();
static int eif_fill();

static List outline_lst[] =
{
   "SIDE",					outl_side,
   "LVL",					eif_lvl,
   "WDT",					eif_diameter,
   "LINE",					poly_line,	// this draws
   "ARC",					poly_arc,
   "CIRCLE",				poly_circle,
   "ASSM_OUTL",			graph_outl,
   "SILK_OUTL",			graph_outl,
   "PACK_OUTL",			graph_outl,
   "SHAPE",					graph_shape,
   "CMPLX_LINE",			outl_outline,
   "FILL",					eif_fill,
   "TEXT",					graph_text,
};

/* celpart_lst Section */
static int celpart_shapename();
static int celpart_mh();
static int celpart_pin();
static int celpart_pad();
static int celpart_asm();
static int celpart_silk();
static int celpart_pack();
static int eif_cmplx_shape();
static int celpart_shape();
static int eif_ref();
static int eif_just();
static int eif_font();
static int eif_partno();
static int eif_cmplx_line();
static int celpart_via_obs();
static int celpart_route_obs();
static int celpart_trace_obs();
static int celpart_line_obs();
static int celpart_line();
static int celpart_arc();
static int celpart_circle();
static int celpart_trace();
static int celpart_text();
static int eif_xy();

static List celpart_lst[] =
{
   "SHAPE_NAME",			celpart_shapename,
   "MH",						celpart_mh,
   "PIN",					celpart_pin,
   "PAD",					celpart_pad,
   "ASSM_OUTL",			celpart_asm,
   "SILK_OUTL",			celpart_silk,
   "PACK_OUTL",			celpart_pack,
   "CMPLX_SHAPE",			eif_cmplx_shape,
   "SHAPE",					celpart_shape,
   "REF",					eif_ref,
   "TJUST",					eif_just,
   "FJUST",					eif_just,
   "FONT",					eif_font,
   "PARTNO",				eif_partno,
   "CMPLX_LINE",			eif_cmplx_line,
   "VIA_OBS",				celpart_via_obs,
   "ROUTE_OBS",			celpart_route_obs,
   "TRACE_OBS",			celpart_trace_obs,
   "LINE_OBS",				celpart_line_obs,
   "LINE",					celpart_line,
   "ARC",					celpart_arc,
   "TRACE",					celpart_trace,
   "CIRCLE",				celpart_circle,
   "TEXT",					celpart_text,
   "XY",						eif_xy,         // used in cell cell
};

/* celvia_lst Section */
static int celvia_shapename();
static int celvia_pin();
static int celvia_pad();

// vias need to be treated special
static List celvia_lst[] =
{
   "SHAPE_NAME",			celvia_shapename,
   "PIN",               celvia_pin,
   "PAD",               celvia_pad,
};

/* celpin_lst Section */
static int eif_padstack();
static int eif_pinname();
static int eif_padrot();

static List celpin_lst[] =
{
   "XY",						eif_xy,
   "PADSTACK",          eif_padstack,
   "PIN_NAME",          eif_pinname,
   "ROT",               eif_padrot,
   "DIA",               eif_diameter,
};

/*	despin_lst Section */
// can not use celpin, because this overwrites part koos etc...
static List despin_lst[] =
{
   "XY",                eif_skip,
   "PADSTACK",          eif_skip,
   "PIN_NAME",          eif_pinname,
   "ROT",               eif_skip,
   "DIA",               eif_skip,
};

/* celpad_lst Section */
static int eif_lyr();

static List celpad_lst[] =
{
   "XY",                eif_xy,
   "PADC",              eif_padstack,
   "ROT",               eif_padrot,
   "LYR",               eif_lyr,    // this is used for line_obs in cell
};

static int padstack_shape();
static int padstack_plated();
static int padstack_top();
static int padstack_bot();
static int padstack_int();

static List padstack_lst[] =
{
   "SHAPE",					padstack_shape,
   "DIAMETER",				eif_diameter,
   "INDEX",					eif_skip,    
   "FONT",					eif_skip,    
   "CHAR",					eif_skip,    
   "RESTRICT",				eif_skip,    
   "CORR_INDEX",			eif_skip,    
   "PLATED",				padstack_plated,
   "TOP_PAD",				padstack_top,
   "BTM_PAD",				padstack_bot,
   "INT_CON_PAD",			padstack_int,
   "INT_NOT_CON_PAD",	eif_skip,
   "THERMAL_PAD",			eif_skip,
   "ANTI_PAD",				eif_skip,
};
 
/* geom_lst Section */
static int geom_typ();
static int geom_xdim();
static int geom_ydim();
static int geom_xoffset();
static int geom_yoffset();

static List geom_lst[] =
{
   "CUSTOM",				eif_skip,
   "GEOMETRY",				geom_typ,
   "X_DIM",					geom_xdim,
   "Y_DIM",					geom_ydim,
   "X_OFFSET",				geom_xoffset,
   "Y_OFFSET",				geom_yoffset,
};

/* outl_lst Section */
static int eif_net();

// outl uses only the LYR, not the LVL layer index.
static List outl_lst[] =	
{
   "NET",					eif_net,
   "SIDE",					outl_side,
   "LYR",					eif_lyr,    
   "LVL",					eif_skip,    // this is in DES file POS_PLANE .
   "OUTLINE",				outl_outline,
   "CMPLX_LINE",			outl_outline,
   "TEXT",					graph_text,	
   "LINE",					poly_line,	// graph_line does not write line
   "ARC",					poly_arc,
   "CIRCLE",				poly_circle,
   "SHAPE",					graph_shape,
};

/* contour_lst Section */
static List contour_lst[] =
{
   "OUTLINE",				outl_outline,
   "WDT",					eif_diameter,
   "PADSTACK",				eif_skip,
   "CUT",					eif_skip,
   "COMP",					eif_skip,
};

/* refdes_lst Section */
static List refdes_lst[] =
{
   "SIDE",					eif_skip,
	"XY",						eif_skip,
	"ROT",					eif_skip,
	"HGT",					eif_skip,
	"WDT",					eif_skip,
};
 
/* graphline_lst Section */
static int eif_sym();

static List graphline_lst[] =
{
   "XY",						eif_xy,
   "LVL",					eif_lvl,
   "LYR",					eif_lyr,    // this is used for line_obs in cell
   "WDT",					eif_diameter,
   "SYM",					eif_sym,
   "FILL",					eif_fill,
};

/* graphshape_lst Sectioin */
static List graphshape_lst[] =
{
   "XY",						eif_xy,
   "LVL",					eif_lvl,
   "FILL",					eif_fill,
   "WDT",					eif_diameter,
   "SYM",					eif_sym,
   "FILL",					eif_fill,
};

/* graphtext_lst Section */ 
static int eif_height();
static int eif_mir();
static int eif_rot();

static List graphtext_lst[] =
{
   "XY",						eif_xy,
   "LVL",					eif_lvl,
   "WDT",					eif_diameter,
   "HGT",					eif_height,
   "MIR",					eif_mir,
   "ROT",					eif_rot,
   "FLEN",					eif_skip,
   "FONT",					eif_skip,
   "FJUST",					eif_skip,
   "TJUST",					eif_skip,
   "SYM",					eif_sym,
};

/* grapharc_lst Section */
static int eif_radius();
static int eif_startangle();
static int eif_deltaangle();

static List grapharc_lst[] =
{
   "XY",						eif_xy,
   "LVL",					eif_lvl,
   "PRI",					eif_radius,
   "SEC",					eif_radius,
   "ST_ANG",				eif_startangle,
   "SW_ANG",				eif_deltaangle,
   "WDT",					eif_diameter,
   "SYM",					eif_sym,
};
 
/* graphcircle_lst Section */
static int eif_circlediameter();

static List graphcircle_lst[] =
{
   "XY",						eif_xy,
   "LVL",					eif_lvl,
   "DIA",					eif_circlediameter,
   "SYM",					eif_sym,
   "FILL",					eif_fill,
};

/* text_lst Section */
static List text_lst[] =
{
   "XY",						eif_xy,
   "LVL",					eif_lvl,
   "WDT",					eif_diameter,
   "HGT",					eif_height,
   "FLEN",					eif_skip,  // this is textlength ???
   "SIDE",					eif_skip,  // this is text side  ???
   "ROT",					eif_rot,
   "MIR",					eif_mir,
   "TJUST",					eif_just,
   "FJUST",					eif_just,
   "FONT",					eif_font,
   "SYM",					eif_sym,
};								

/* des_lst Section */
static int des_boardorg();
static int des_part();
static int des_cell();
static int des_via();
static int des_route_outl();
static int des_board_outl();
static int des_plane_outl();
static int des_silk_artwork();
static int des_contour();
static int des_via_obs();
static int des_trace_obs();
static int des_route_obs();
static int des_place_obs();
static int des_trace();
static int des_pos_plane();
static int des_conductive_artwork();
static int des_line();
static int des_line_obs();
static int des_arc();
static int des_circle();
static int des_text();
static int des_shape();
static int des_mh();
static int des_fiducial();	

static List des_lst[] =
{
   "BOARDORG",				des_boardorg,
   "PART_INSTANCE",		des_part,
   "CELL_INSTANCE",		des_cell,
   "VIA_INSTANCE",		des_via,
   "ROUTE_OUTL",			des_route_outl,			// uses only lyr, not lvl
   "BOARD_OUTL",			des_board_outl,			// uses only lyr, not lvl
   "PLANE_OUTL",			des_plane_outl,			// uses only lyr, not lvl
   "SILK_ARTWORK",		des_silk_artwork,			// uses only lyr, not lvl
   "CONTOUR",				des_contour,
   "CMPLX_SHAPE",			eif_cmplx_shape,
   "CMPLX_LINE",			eif_cmplx_line,
   "VIA_OBS",				des_via_obs,
   "TRACE_OBS",			des_trace_obs,
   "ROUTE_OBS",			des_route_obs,
   "PLACE_OBS",			des_place_obs,
   "TRACE",					des_trace,
   "POS_PLANE",			des_pos_plane,
   "CONDUCTIVE_ARTWORK", des_conductive_artwork,	// uses only lyr, not lvl
   "LINE",					des_line,
   "LINE_OBS",				des_line_obs,
   "ARC",					des_arc,
   "CIRCLE",				des_circle,
   "TEXT",					des_text,
   "SHAPE",					des_shape,
   "MH",						des_mh,
	"FIDUCIAL",				des_fiducial,	
};

/* board_org_lst Section */
static List board_org_lst[] =
{
   "XY",						eif_xy,
   "CS",						eif_skip,
};

/* part_instance_lst Section */
static int part_instance_shape_name();
static int part_instance_ref_des();
static int part_instance_part_number();
static int part_part_def();
static int part_cell_def();
static int eif_side();

static List part_instance_lst[] =
{
   "SHAPE_NAME",			part_instance_shape_name,
   "CELL_NAME",			part_instance_shape_name,
   "REF_DES",				part_instance_ref_des,
   "PART_NUMBER",			part_instance_part_number,
   "PART_DEF",				part_part_def,
   "CELL_DEF",				part_cell_def,
   "XY",						eif_xy,
   "SIDE",					eif_side,   // this is not the mirror flag.
   "MIR",					eif_mir,
   "ROT",					eif_rot,
   "MOV",					eif_skip,
};

/* via_instance_lst Section */
static int part_via_def();

static List via_instance_lst[] =
{
   "SHAPE_NAME",			part_instance_shape_name,
   "VIA_DEF",				part_via_def,
   "XY",						eif_xy,
   "NET",					part_instance_ref_des,
   "MOV",					eif_skip,
   "FTL",					eif_skip,
};

/* mh_instance_lst Section */
static List mh_instance_lst[] =
{
   "PADSTACK",				eif_skip,
   "XY",						eif_xy,
   "DIA",					eif_diameter,
	"SYM",					eif_skip,
};

/* fiducial_instance_lst Section */
static int eif_fidc();

static List fiducial_instance_lst[] =
{
   "FIDC",					eif_fidc,
   "XY",						eif_xy,
   "LVL",					eif_lvl,
   "ROT",					eif_rot,
};

/* part_def_lst Section */
static int despart_pin();

// this is the local cell definition in the design file
static List part_def_lst[] =
{
   "PARTNO",				eif_skip,
   "REF",					eif_skip,
   "PIN",					despart_pin,
	"PAD",					eif_skip,
	"MH",						eif_skip,
	"LINE",					eif_skip,
	"ARC",					eif_skip,
	"CIRCLE",				eif_skip,
	"TEXT",					eif_skip,
	"PACK_OUTL",			eif_skip,
	"ASSM_OUTL",			eif_skip,
	"SHAPE",					eif_skip,
	"ROUTE_OBS",			eif_skip,
   "CMPLX_SHAPE",			eif_skip,
   "CMPLX_LINE",			eif_skip,
   "LINE_OBS",				eif_skip,
   "VIA_OBS",				eif_skip,
	"TRACE",					eif_skip,
};

/* trace_instance_lst Section */
static List trace_instance_lst[] =
{
   "XY",						eif_xy,
   "LYR",					eif_lyr,
   "WDT",					eif_diameter,
   "NET",					part_instance_ref_des,
   "MOV",					eif_skip,
};

/* correspondence_lst Section */
static int corfile_pad();
static int corfile_padstack();
static int corfile_cell_lib();
static int corfile_clearance();
static int corfile_technology();
static int corfile_design();
static int corfile_pdb_ascii();
static int corfile_netlist();

static List correspondence_lst[] =
{
   "FILETYPE",				eif_filetype,
   "VERSION",				eif_version,
   "CREATOR",				eif_skip,
	"PAD",					corfile_pad,	
	"PADSTACK",				corfile_padstack,
	"CELL_LIB",				corfile_cell_lib,
	"CLEARANCE",			corfile_clearance,
	"TECHNOLOGY",			corfile_technology,
	"DESIGN",				corfile_design,
	"PDB_ASCII",			corfile_pdb_ascii,
	"NETLIST",				corfile_netlist,
};


