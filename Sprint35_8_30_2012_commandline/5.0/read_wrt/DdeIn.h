// $Header: /CAMCAD/4.5/read_wrt/DdeIn.h 17    11/02/05 2:25p Rick Faltersack $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/           

/* Define Section ***********************************************************/
#define   MAX_LINE            2000  // yes 2000 is allowed

#define   TEXT_RATIO          0.65

#define   STAT_UNKNOWN        0
#define   STAT_SHAPE          1
#define   STAT_COMPONENT      2
#define   STAT_NETLIST        3
#define   STAT_AREA           4
#define   STAT_TRACKCODE      5
#define   STAT_PADSHAPE       6
#define   STAT_BOARDOUTLINE   7
#define   STAT_PADSTACK       8
#define   STAT_DIM				9

#define   LAY_COMPNAME        1
#define   LAY_DEVICE          2     // Type - Value
#define   LAY_ARTICLE         3     // Partnumber

#define   GRAPH_COO           1
#define   GRAPH_CIR           2
#define   GRAPH_CEN           3
#define   GRAPH_NEW           4

#define   SIZ_PCB_LST         (sizeof(pcb_lst) / sizeof(List))

/* Struct Section ***********************************************************/
typedef struct
{
   CString  name;       // dde layer name
   char     attr;       // layer attributes as defined in dbutil.h
}DDEAdef;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
}DDEAttribmap;
typedef CTypedPtrArray<CPtrArray, DDEAttribmap*> CAttribmapArray;

typedef struct
{
   double   d;
   int      toolindex;
}DDEDrill;
typedef CArray<DDEDrill, DDEDrill&> CDrillArray;

typedef struct
{
   CString  name;
   CString  sys;
   CString  poly;
   BOOL     isBottomLayer; 
}DDELpm;
typedef CTypedPtrArray<CPtrArray, DDELpm*> CLpmArray;

typedef struct
{
   CString  name;
   CString  originalname;
   CString  kind;
   int      index;
   int      pincnt;
   double   xref,yref;
   char     master;
}DDESlist;
typedef CTypedPtrArray<CPtrArray, DDESlist*> CSlistArray;

typedef struct
{
   CString  name;          //compname
   CString  device;        // article
   CString  value;         // value
   double   x;
   double   y;
   bool     mir;
   bool     written;     // written into database
   bool     fiducial;
   bool     testpoint;
   bool     boardoutline;
   bool     exploded;
   bool     bottom;
   double   rot;           // 0..360 degree
   char     master;        // this is the master comp, which determines the padstack definition
   int      lay;
   int      shapeindex;
   int      compindex;
}DDEComp;
typedef CTypedPtrArray<CPtrArray, DDEComp*> CCompArray;

typedef struct
{
   CString  name;
   int      defined;
   int      index;
   int      stacknumber;
   char     plane;       // P argument
   char     solder;      // S argument
   char     electrical;  // e argument
   char     comp;        // C argument
   char     inner;       // I argument
   int      mirrorindex;
   CString  attr;
   int      layer_type; // this is from .sys
   CString  lpmname;    // if avail
   BOOL     isBottomLayer; 
}DDELlist;
typedef CTypedPtrArray<CPtrArray, DDELlist*> CLlistArray;

typedef struct
{
   double   x;
   double   y;
   char     cw;         // 0 = counter clock, 1 clock
   char     typ;        // graphtyp
   char     borderpart; // borderpart encountered
}DDEGraph;
typedef CArray<DDEGraph, DDEGraph&> CGraphArray;

typedef struct
{
   CString  name;
   int      index;
   double   drill;      // this is layer 255
   BOOL     isGood;     // indicate that is created with no problem
   BOOL     plated;     // indicate if the drill is plated or not
}DDEPadshape;
typedef CTypedPtrArray<CPtrArray, DDEPadshape*> CPadshapeArray;

typedef struct
{
   int      index;
   double   drill;   
}DDEPst;
typedef CTypedPtrArray<CPtrArray, DDEPst*> CPstArray;

typedef struct
{
   int      padcode;
   int      laynr;
   int      rot;
}DDEPad;
typedef CTypedPtrArray<CPtrArray, DDEPad*> CPadArray;

typedef struct
{
   int      padstackNum;
   double   drillSize;
   CString  padstackName;
   char     stackType;  //n = through hole
                           //s = SMD
                           //v = via

   int      creatFlag;     //0 = no padstack content during definetion of .pst
                           //1 = content is from definetion of.pst
                           //2 = content is from definetion of .wlp
}DDEPadstack;
typedef CTypedPtrArray<CPtrArray, DDEPadstack*> CPadstackArray;

typedef struct
{
   int      shapeindex;
   int      done;
}DDEPinl;

typedef struct
{
   char     *token;
   int      (*function)();
} List;


/* List Section ***********************************************************/
static int ipl_remark();
static int ipl_end();
static int ipl_lay();
static int ipl_mir();
static int ipl_lpm();
static int ipl_psh();
static int ipl_sec();
static int ipl_cir();
static int ipl_coo();
static int ipl_arc();
static int ipl_cen();
static int ipl_poe();
static int ipl_wlg();
static int ipl_wle();
static int ipl_tra();
static int ipl_dfa();
static int ipl_ena();
static int ipl_txt();
static int ipl_typ();
static int ipl_pad();
static int ipl_bur();
static int ipl_com();
static int ipl_tsh();
static int ipl_wlp();
static int ipl_sys();
static int ipl_bma();
static int ipl_cle();
static int ipl_cop();
static int ipl_pop();
static int ipl_pst();
static int ipl_tol();
static int ipl_pla();
static int ipl_uls();
static int ipl_pse();
static int ipl_iad();
static int ipl_blo();
static int ipl_wop();
static int ipl_dim();
static int ipl_die();
static int ipl_dob();
static int ipl_pul();
static int ipl_fon();
static int ipl_dop();

static List pcb_lst[] =
{
   ".rem",                 ipl_remark,
   ".cle",                 ipl_cle,
   ".end",                 ipl_end,
   ".lay",                 ipl_lay,
   ".mir",                 ipl_mir,
   ".psh",                 ipl_psh,
   ".sec",                 ipl_sec,
   ".bma",                 ipl_bma,
   ".cir",                 ipl_cir,
   ".arc",                 ipl_arc,
   ".poe",                 ipl_poe,
   ".wlg",                 ipl_wlg,
   ".wle",                 ipl_wle,
   ".tra",                 ipl_tra,
   ".txt",                 ipl_txt,
   ".typ",                 ipl_typ,
   ".pad",                 ipl_pad,
   ".bur",                 ipl_bur,
   ".com",                 ipl_com,
   ".dfa",                 ipl_dfa,
   //".iad",               ipl_iad,
   ".coo",                 ipl_coo,
   ".cen",                 ipl_cen,
   ".ena",                 ipl_ena,
   ".tsh",                 ipl_tsh,
   ".wlp",                 ipl_wlp,
   ".sys",                 ipl_sys,
   ".pst",                 ipl_pst,
   ".tol",                 ipl_tol,
   ".pla",                 ipl_pla,
   ".uls",                 ipl_uls,
   ".pse",                 ipl_pse,
   ".lpm",                 ipl_lpm,
   ".cop",                 ipl_cop,
   ".pop",                 ipl_pop,
   ".blo",                 ipl_blo,
   ".wop",                 ipl_wop,
   ".dim",                 ipl_dim,     
   ".die",                 ipl_die,     
   ".dob",                 ipl_dob,     
   ".pul",                 ipl_pul,
   ".fon",                 ipl_fon,
   ".dop",                 ipl_dop,
};