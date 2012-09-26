// $Header: /CAMCAD/4.6/read_wrt/UnicamIn.h 11    5/29/07 10:46p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

/* Defines Sections *****************************************************************/

#define  UNICAMERR            "unicam.log"

#define  VER114               1
#define  VER115               2
#define  VER120               3

#define  FNULL                fnull
#define  FSKIP                fskip

static   double               TEXT_CORRECT  = 0.75;
static   double               TEXT_RATIO    = 0.66;      // width to height ratio

#define  MAX_LAYERS           255   /* Max number of layers.   */
#define  MAX_COM_PINS         5000  /* Max number of pins / comp. */

#define  U_CMM                0
#define  U_MIL                1

#define  SIZ_BRK_LST          (sizeof(brk_lst) / sizeof(List))
#define  BRK_SQUIGLY          0
#define  BRK_SQUARE           1
#define  BRK_B_SQUIGLY        2
#define  BRK_B_SQUARE         3

#define  SIZ_START_LST        (sizeof(start_lst) / sizeof(List))
#define  SIZ_PROJECT_LST      (sizeof(project_lst) / sizeof(List))
#define  SIZ_OPTIONS_LST      (sizeof(options_lst) / sizeof(List))
#define  SIZ_BOARD_LST        (sizeof(board_lst) / sizeof(List))
#define  SIZ_SHAPE_LST        (sizeof(shape_lst) / sizeof(List))
#define  SIZ_TERMINAL_LST     (sizeof(terminal_lst) / sizeof(List))
#define  SIZ_VIA_LST          (sizeof(via_lst) / sizeof(List))
#define  SIZ_FIDUCIAL_LST     (sizeof(fiducial_lst) / sizeof(List))
#define  SIZ_COMPONENT_LST    (sizeof(component_lst) / sizeof(List))
#define  SIZ_NETLIST_LST      (sizeof(netlist_lst) / sizeof(List))
#define  SIZ_NODELIST_LST     (sizeof(nodelist_lst) / sizeof(List))
#define  SIZ_GRAPHIC_LST      (sizeof(graphic_lst) / sizeof(List))
#define  SIZ_GLB_LST          (sizeof(glb_lst) / sizeof(List))
#define  SIZ_PANEL_LST        (sizeof(panel_lst) / sizeof(List))
#define  SIZ_PARAM_LST        (sizeof(param_lst) / sizeof(List))

/* Structures Section **************************************************************/

typedef struct
{
   CString  name;       // pdif layer name
   char  attr;          // layer attributes as defined in dbutil.h
   int   stacknumber;   // electrical stack number
} Adef;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  uni_name;
   CString  cc_name;
} UAttrmap;
typedef CTypedPtrArray<CPtrArray, UAttrmap*> CAttrmapArray;

typedef struct
{  
   int level;
}UPcblevel;
typedef CArray<UPcblevel, UPcblevel&> CPcblevelArray;

typedef struct
{
   char  *name;
   int   pincnt;
   char  **pinname;
} UShapeName;

typedef struct
{
   ApertureShapeTag     form;
   double   sizeA;
   double   sizeB;
}UPadform;
typedef CArray<UPadform, UPadform&> CPadformArray;

typedef struct
{
   double   x, y;
   double   rot;
   int      side;
}ULocation;

typedef struct
{
   CString  name;
   CString  cclass;     // class      : 2,
   CString  ctype;      // type       : "49-210019",
   CString  cadtype;    // %cadtype   : "49210019.PRT",
   CString  shapeid;    // shapeid    : "49210019.PRT",
   CString  package;    // %package   : " ",
   CString  pinref;     // %pinref
   CString  _pmabar;
   CString  _main_mark;
   CString  _sub_mark;
   CString  _term_seq;
   CString  _feeder_style;
   CString  _carry_mode;
   CString  descr;      // descr      : "CAP, .01uF, 50V, +/-10%",
   CString  insclass;   // insclass   : "SMT",
   CString  gentype;    // gentype    : "1206C-08",
   CString  machine;    // machine    : "TP11_P",
   CString  test;       // test       : "49-210019",
   CString  comment;
   CString  schname;
   int      polarized;
   int      updated;    // updated    : 1,
   int      bom;
   int      insorder;   // insorder   : 2,
   int      mtechn;     // %mtechn    : 2,
   ULocation   clocation;  // %clocation : {85344061,280035202,0,0},
   ULocation   location;   // location   : {85344061,280035202,0,0},
   CString  marking;    // %_marking  : ".01uF",
   CString  item;       // %_item     : "036" 
   double   ttolpos;    // %ttolpos   : "10"
   double   ttolneg;    // %ttolneg   : "10"
   CString  tvalue;     // %tvalue    : "100u"

   CString  _direction; // %_
   CString  _polarized; // %_
   CString  _package;   // %_
   CString  _traypickup;// %_

   BOOL     hasCLocation;
}UComponent;

typedef struct
{
   CString  name;
   CString  shapeid;
   CString  schname;
   CString  type;
   int      f_type;
   int      f_class;
   int      f_sel;
   ULocation   clocation;  // %clocation : {85344061,280035202,0,0},
   ULocation   location;   // location   : {85344061,280035202,0,0},
}UFiducial;

typedef  struct
{
   char  *token;
   int      (*function)();
} List;

/* List Section **************************************************************/
static int fnull();           // skipped, but reports the skip
static int fskip();           // skipped, but no logs

static List  brk_lst[] =
{
   "{",                    FNULL,
   "[",                    FNULL,
   "}",                    FNULL,
   "]",                    FNULL
};

/* Start of UNICAM file.  */
static int uni_project();
static int uni_end();

static List start_lst[] =
{
   "project",              uni_project,
   "end",                  uni_end,
};

/* Project_lst section */
static int uni_version();  
static int uni_options();  
static int uni_userdata(); 
static int uni_board(); 
static int uni_panel(); 
static int uni_linedef();  
static int uni_note();  
static int uni_view();  
static int uni_feeders();  
static int uni_useshapeorg(); 
static int uni_ecn();

static List project_lst[] =
{
   "version",              uni_version,
   "options",              uni_options,
   "fonts",                fskip,
   "%userdata",            uni_userdata,
   "board",                uni_board,
   "panel",                uni_panel,
   "note",                 uni_note,
   "linedef",              uni_linedef,
   "view",                 uni_view,
   "%feeders",             uni_feeders,
   "%useshapeorg",         uni_useshapeorg,
   "%ecn",                 uni_ecn,
};

/* options_lst section */
static int uni_format();
static int uni_unit();

static List options_lst[] =
{
   "format",               uni_format,
   "unit",                 uni_unit,
};

/* basic shared outline utility */
static int uni_outline_parser();

/* board_lst section */
static int uni_board_outline();
static int uni_shape();
static int uni_part();
static int uni_partid();
static int uni_fiducial();
static int uni_fidlinks();
static int uni_netlist();
static int uni_unconnected();
static int uni_vias_numbered();
static int uni_nextpartid();
static int uni_deleted();

static List board_lst[] =
{
   "outline",              uni_board_outline,
   "shape",                uni_shape,
   "part",                 uni_part,
   "fiducial",             uni_fiducial,
   "fidlinks",             uni_fidlinks,
   "%deleted",             uni_deleted,
   "%netlist",             uni_netlist,
	"%unconnected",			uni_unconnected,
   "%vias_numbered",       uni_vias_numbered,
   "%partid",              uni_partid,
   "%nextpartid",          uni_nextpartid
};

/* panel_lst section */
static int uni_param();
static int uni_boardinst();

static List panel_lst[] =
{
   "param",						uni_param,
   "board",						uni_boardinst,
};

/* param_lst section */
static int uni_sizex();
static int uni_sizey();
static int uni_thick();
static int uni_jig();

static List param_lst[] =
{
   "sizex",						uni_sizex,
   "sizey",						uni_sizey,
   "thick",						uni_thick,
   "jig",						uni_jig,
};

/* shape_lst section */
static int uni_shape_outline();
static int uni_terminals();
static int shape_ref();
static int shape_partno();
static int shape_desc();
static int shape_comm();

static List shape_lst[] =
{
   "outline",              uni_shape_outline,
   "terminals",            uni_terminals,
   "%ref",                 shape_ref,
   "%partno",              shape_partno,
   "%desc",                shape_desc,
   "%comm",                shape_comm,
};

/* terminal_lst section */
static int terminal_p1();
static int terminal_p2();
static int terminal_p3();

static List terminal_lst[] =
{
   "P1",                   terminal_p1,
   "P2",                   terminal_p2,
   "P3",                   terminal_p3,
};

/* via_lst sectioin */
static int via_p1();
static int via_p2();
static int via_p3();

static List via_lst[] =
{
   "P1",                   via_p1,
   "P2",                   via_p2,
   "P3",                   via_p3,
};

/* fiducial_lst section */
static int fiducial_f_sel();
static int fiducial_f_class();
static int fiducial_f_type();
static int fiducial_type();
static int fiducial_shapeid();
static int fiducial_location();
static int fiducial_clocation();
static int fiducial_schname();

static List fiducial_lst[] =
{
   "f_class",              fiducial_f_class,
   "f_type",               fiducial_f_type,
   "f_sel",                fiducial_f_sel,
   "type",                 fiducial_type,
   "shapeid",              fiducial_shapeid,
   "%clocation",           fiducial_clocation,
   "location",             fiducial_location,
   "%schname",             fiducial_schname,
};

/* component_lst section */
static int comp_type();
static int comp_class();
static int comp_cadtype();
static int comp_shapeid();
static int comp_package();
static int comp_pinref();
static int comp_descr();
static int comp_comment();
static int comp_schname();
static int comp_insclass();
static int comp_gentype();
static int comp_machine();
static int comp_test();
static int comp_updated();
static int comp_bom();
static int comp_insorder();
static int comp_mtechn();
static int comp_clocation();
static int comp_location();
static int comp_marking();
static int comp_item();
static int comp_ttolpos();
static int comp_ttolneg();
static int comp_tvalue();
static int comp_pmabar();
static int comp_main_mark();
static int comp_sub_mark();
static int comp_term_seq();
static int comp_feeder_style();
static int comp_carry_mode();
static int comp_polarized();
static int comp_direction();
static int comp__package();
static int comp__traypickup();

static List component_lst[] =
{
   "class",                comp_class,
   "type",                 comp_type,
   "%cadtype",             comp_cadtype,
   "shapeid",              comp_shapeid,
   "%package",             comp_package,
   "%pinref",              comp_pinref,
   "%direction",           comp_direction,
   "%_pmabar",             comp_pmabar,
   "%_Main_Mark",          comp_main_mark,
   "%_Sub_Mark",           comp_sub_mark,
   "%_Term_Seq",           comp_term_seq,
   "%_Feeder_Style",       comp_feeder_style,
   "%_Carry_Mode",         comp_carry_mode,
   "%polarized",           comp_polarized,
   "descr",                comp_descr,
   "comment",              comp_comment,
   "%schname",             comp_schname,
   "insclass",             comp_insclass,
   "gentype",              comp_gentype,
   "machine",              comp_machine,
   "test",                 comp_test,
   "updated",              comp_updated,
   "bom",                  comp_bom,
   "insorder",             comp_insorder,
   "%mtechn",              comp_mtechn,
   "%clocation",           comp_clocation,
   "location",             comp_location,
   "%_marking",            comp_marking,
   "%_item",               comp_item,
   "%ttolpos",             comp_ttolpos,
   "%ttolneg",             comp_ttolneg,
   "%tvalue",              comp_tvalue,
   "%_Package",            comp__package,    
   "%_TrayPickup",         comp__traypickup,
   "%_Direction",          comp_direction,
   "%_Polarized",          comp_polarized,
};

/* netlist_lst section */
static int netlist_node();

static List netlist_lst[] =
{
   "node",                 netlist_node,
};

/* nodelist_lst section */
static int node_pins();
static int node_vias();
static int node_traces();
static int node_nail();
static int node_short();

static List nodelist_lst[] =
{
   "pins",                 node_pins,
   "vias",                 node_vias,
   "traces",               node_traces,
   "nail",                 node_nail,
   "short",                node_short,
};

/* graphic_lst section */
static int gra_arc();
static int gra_pline();
static int gra_line();
static int gra_circle();
static int gra_rectangle();
static int gra_text();

static List graphic_lst[] =
{
   "L",                    gra_line,
   "PL",                   gra_pline,
   "R",                    gra_rectangle,
   "C",                    gra_circle,
   "A",                    gra_arc,
   "T",                    gra_text,
// "P1",                   gra_circularpad,
// "P2",                   gra_squarepad,
// "P3",                   gra_rectangularpad,
};

/* Square bracket "global" UNICAM settings. */
static int glb_width();
static int glb_pcblevel();
static int glb_visibility();

static List glb_lst[] =
{
   "V",                   glb_visibility,
   "L",                   glb_width,         // Width.      
   "N",                   glb_pcblevel,      // layer 1 = top, ...
};

//_____________________________________________________________________________
class CUnicamComponents
{
private:
   static CUnicamComponents* m_unicamComponents;
   CTypedPtrMap<CMapStringToPtr,CString,DataStruct*> m_components;

public:
   CUnicamComponents();

   static CUnicamComponents& getComponents();
   static void empty();

   void setAt(const CString& refDes,DataStruct& component);
   void fixPadStack(const CString& refDes,const CString& pinNumber,int visibility);
};
