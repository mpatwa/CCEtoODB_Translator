
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#define  MAX_LINE                500   /* Max line length. Long attributes can extent a line  */

#define  MAX_PDEF                1000  /* Max number of pad definition. */
#define  MAX_LAYERS              255   /* Max number of layers.   */
#define  MAX_COM_PINS            5000  /* Max number of pins / comp. */
#define  MAX_SHAPE               2000
#define  MAX_PLANENET            1000

#define  MAX_APERTURE            1000
#define  MAX_TYPE                1000

/* PDIF line styles.       */

#define  STL_SOLID               0
#define  STL_DASHED              1
#define  STL_DOTTED              2
#define  MAX_LINE_STYLE          3

/* Layer codes.      */

#define  LAY_ALL_LAYERS          -1
#define  LAY_NULL                -2

#define  U_CMM                   0
#define  U_MIL                   1
#define  U_DECIMM                2

typedef  struct
{
   int      electrical_layers;            /* number of electrical layers from lyrphid */
   int      grid;                         /* Grid size.     */
   int      max_layers;                   /* Number of layers defined.  */
   int      max_dlayers;                  /* Number of default layers.  */

   /* Current values that are changed with global  */
   /* PDIF commands.                                              */
   int      cur_layer;                    /* Layer.      */
   int      cur_style;                    /* Line style. */
   double   cur_width;                    /* Line width     */
   int      cur_widthindex;               //derived linewidth index
   double   cur_textsize;                 /* Line width     */

   /* Current Component info used for Instance.    */
   double   cur_pos_x;                    /* position.   */
   double   cur_pos_y;
   double   cur_scale_x;                  /* Scale.      */
   double   cur_scale_y;
   int      cur_rotation;                 /* Rotation.   */
   int      cur_mirror;                   /* Current mirror.   */

   /* Current pad stack info.    */
   int      cur_pad;                      /* Current pad.         */
   int      cur_pstack;                   /* Current padstack number. */
   int      cur_smd;
   double   cur_drillsize;                // from derivedrill
   int      cur_ty;
   /* Pad stacks */
   int      max_dpads;                    /* number of pad stacks. */
   int      cur_textmirror;
   int      cur_textjust;
   int      cur_textrot;
   int      cur_dimtextrot;
   int      cur_pdifversion;
   int      cur_polywidthindex;
   double   cur_polywidth;
} Global;

#define  FNULL                   fnull
#define  FSKIP                   fskip

static int fnull(); // skipped, but reports the skip
static int fskip(); // skipped, but no logs

typedef struct
{
   CString  name;       // pdif layer name
   char  attr;          // layer attributes as defined in dbutil.h
   int   stacknumber;   // electrical stack number
} Adef;

typedef struct
{
   CString  name;       // pdif layer name
   CString  map;        // pdif layer name
} Mapdef;

typedef struct
{
   CString  name;       // pdif layer name
} Ignoredef;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   int      defined;    
   int      layer;
   int      textjust;
   int      textrot;
   double   textsize;
   CString  name;
   double   x, y;
} PDIFRd;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  pdif_name;
   CString  cc_name;
} PDIFAttrmap;
typedef CTypedPtrArray<CPtrArray, PDIFAttrmap*> CAttrmapArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  name;
} PDIFUnknownaperture;
typedef CTypedPtrArray<CPtrArray, PDIFUnknownaperture*> CUnknownapertureArray;

typedef struct         // this is from DESIGN_OBJECTS | COMPONENT
{
   CString  layername;
   double   percent;
} PDIFDeriveDrill;
typedef CTypedPtrArray<CPtrArray, PDIFDeriveDrill*> CDeriveDrillArray;

typedef struct
{
   double d;   
   int    toolindex;
}PDIFDrill;
typedef CArray<PDIFDrill, PDIFDrill&> CDrillArray;

typedef struct
{
   char           *pstack_name;
   double         drill;
   int            pdnum;
   unsigned char  smd;
   unsigned char  used;
} Pdef;

typedef struct
{
   char  *name;
   int   pincnt;
   char  **pinname;
} PDIF_ShapeName;

typedef struct
{
   char  *net;
   char  *lay; // layername
} PDIF_Planenet;

typedef  struct
{
   char        *name;
   char        typ;        // 0 - throuhole pad
                           // 1 - smd pad
   int         pt;
   int         found;
   double      x,y;
   DataStruct  *datastruct;// pointer to datastruct, which is used to rename the padstack with comppadstack.
} PDIF_Pin;

typedef  struct
{
   CString  layer_name;
   unsigned char  color;
   unsigned char  used;
   int      physnr;        // done in do_phys
   int      electrical;    // is this an electrical layer
   int      mirror_index;
   DbFlag   flg;
} PDIFLayers;

typedef struct
{
   int      number;
   int      dcode;
   double   ow,oh;   // width, height
                     // ow = odia
   double   rotation;
   char     shape;
   int      sidecnt;
   Point2   sides[10];  // a gerber allows maximum 10 sides for a polygon
   int      used;
} PDIF_Aperture;

typedef struct
{
   double   x, y;
   double   offsetx, offsety; // aperture offset from pin center.
   int      layer;   
   int      pinnr;
   int      bnum;
}PDIF_comppadstack;  
typedef CTypedPtrArray<CPtrArray, PDIF_comppadstack*> CCompPadstackArray;

typedef struct
{
   CString  stackname;
}PDIF_comppadname;   
typedef CTypedPtrArray<CPtrArray, PDIF_comppadname*> CCompPadnameArray;

typedef struct
{
   CString  compname;
}PDIF_arrowname;  
typedef CTypedPtrArray<CPtrArray, PDIF_arrowname*> CArrowArray;

typedef  struct
{
   char  *token;
   int      (*function)();
} List;


/* Bracket definitions.       */

static List  brk_lst[] =
{
   "{",                    FNULL,
   "[",                    FNULL,
   "}",                    FNULL,
   "]",                    FNULL
};

#define  SIZ_BRK_LST       (sizeof(brk_lst) / sizeof(List))
#define  BRK_SQUIGLY       0
#define  BRK_SQUARE        1
#define  BRK_B_SQUIGLY     2
#define  BRK_B_SQUARE      3

/* Start of PDIF file.  */

static int      com_component();

static List  str_lst[] =
{
   "Component",            com_component,
};
#define  SIZ_STR_LST       (sizeof(str_lst) / sizeof(List))


/* Square bracket "global" PDIF settings. */

static int      glb_layer(),glb_style(),glb_width(),glb_textsize();
static int      glb_textjust(),glb_textrot(),glb_textmirror();
static int      glb_dimtextrot(), glb_arrowfilename();

static List  glb_lst[] =
{
   "Ly",                   glb_layer,           /* Layer.      */
   "Ls",                   glb_style,           /* Line style. */
   "Wd",                   glb_width,           /* Line width. */
   "Ts",                   glb_textsize,        /* Text size.     */
   "Tj",                   glb_textjust,        /* Text justification.  */
   "Tr",                   glb_textrot,         /* Text rotation.    */
   "Tm",                   glb_textmirror,      /* Text mirror.         */
   "Dwd",                  FSKIP,                  /* PCAD6.         */
   "Dts",                  FSKIP,                  /* PCAD6.         */
   "Dafd",                 FSKIP,                  /* PCAD6.         */
   "Cent",                 FSKIP,                  /* PCAD6.         */
   "Elf",                  FSKIP,                  /* PCAD6.         */
   "Eba",                  FSKIP,                  /* PCAD6.         */
   "Dli",                  FSKIP,                  /* PCAD6.         */
   "Dtl",                  FSKIP,                  /* PCAD6.         */
   "Dpt",                  FSKIP,                  /* PCAD6.         */
   "Dtd",                  glb_dimtextrot,         /* PCAD6.         */
   "Apt",                  FSKIP,                  /* PCAD6.         */
   "Amt",                  FSKIP,                  /* PCAD6.         */
   "Ddu",                  FSKIP,                  /* PCAD7.         */
   "Dus",                  FSKIP,                  /* PCAD7.         */
   "Ds",                   FSKIP,                  /* PCAD6.         */
   "Gdt",                  FSKIP,                  /* PCAD6.         */
   "Lss",                  FSKIP,                  /* PCAD6.         */
   "Mas",                  FSKIP,                  /* PCAD6.         */
   "Pfc",                  FSKIP,                  /* PCAD7.         */
   "Afn",                  glb_arrowfilename,      /* PCAD6.         */
   "Dmt",                  FSKIP,                  /* PCAD6.         */
   "Dor",                  FSKIP,                  /* PCAD6.         */
   "Dlt",                  FSKIP,                  /* PCAD6.         */
   "Ddt",                  FSKIP,                  /* PCAD6.         */
   "Rls",                  FSKIP,                  /* PCAD6.         */
};
#define  SIZ_GLB_LST       (sizeof(glb_lst) / sizeof(List))


/* Component PDIF sections.      */

static   int      com_environment(),com_user(),com_display();
static   int      com_symbol(),com_detail(),com_aperture_table();

static List  com_lst[] =
{
   "Environment",          com_environment,
   "User",                 com_user,
   "Display",              com_display,
   "Aperture_table",       com_aperture_table,
   "Symbol",               com_symbol,
   "Detail",               com_detail
};
#define  SIZ_COM_LST       (sizeof(com_lst) / sizeof(List))


/* Environment PDIF sections.    */

static   int      env_dbunit(),env_version(),env_grid(),env_layer();
static   int      env_lyrphid(),env_sstfile();
static   int      env_dbtype(), polyap();
static   int      env_program();

static List  env_lst[] =
{
   "PDIFvrev",          env_version,
   "Program",           env_program,
   "DBtype",            env_dbtype,
   "DBvrev",            FSKIP,
   "DBtime",            FSKIP,
   "DBunit",            env_dbunit,
   "DBgrid",            env_grid,
   "Lyrstr",            env_layer,
   "Apr",               FSKIP,
   "Lyrmap",            FSKIP,
   "Ssymtbl",           FSKIP,
   "PCLR",              FSKIP,
   "PSIZ",              FSKIP,
   "Polyap",            FSKIP,   // not needed
   "Rotp",              FSKIP,
   "SSTFile",           env_sstfile,
   "Lyrphid",           env_lyrphid,
};
#define  SIZ_ENV_LST       (sizeof(env_lst) / sizeof(List))


/* Aperture PDIF sections.    */

static   int      app_shp(),app_dcode(),app_owd(),app_oht();
static   int      app_odia(),app_apcomm(),app_aptype(),app_aprot();
static   int      app_idia(), app_sides();

static List  app_lst[] =
{
   "Shp",               app_shp,
   "Dcode",             app_dcode,
   "Odia",              app_odia,
   "Idia",              app_idia,
   "Owd",               app_owd,
   "Iwd",               FNULL,
   "Iht",               FNULL,
   "Oht",               app_oht,
   "Apcomm",            app_apcomm,
   "Aptype",            app_aptype,
   "Aprot",             app_aprot,
   "Sides",             app_sides,
   "Ti",                FSKIP,   // thermal tyins
   "Ta",                FSKIP,   // thermal spoke angle
   "Twd",               FSKIP,   // thermal width 
};
#define  SIZ_APP_LST       (sizeof(app_lst) / sizeof(List))


/* ApTable PDIF sections.     */

static   int      apt_apfile(),apt_aprnum();
static   int      apt_apver();

static List  apt_lst[] =
{
   "Apfile",            apt_apfile,
   "Aprnum",            apt_aprnum,
   "Apver",             apt_apver,
};
#define  SIZ_APT_LST       (sizeof(apt_lst) / sizeof(List))


/* User PDIF sections.     */

static List  usr_lst[] =
{
   "P-CAD",             FSKIP,
   "View",              FSKIP,
};
#define  SIZ_USR_LST       (sizeof(usr_lst) / sizeof(List))

/* Symbol PDIF sections.      */

static List  sym_lst[] =
{
   "Pin_Def",           FSKIP,
   "Pkg",               FSKIP,
   "Spkg",              FSKIP,
   "Pic",               FSKIP,
   "Atr",               FSKIP,
};
#define  SIZ_SYM_LST       (sizeof(sym_lst) / sizeof(List))

/* Detail PDIF sections.      */

static   int      det_annotate(),det_net_def(),det_pad_stack(),det_subcomp();

static List  det_lst[] =
{
   "Annotate",          det_annotate,
   "Net_def",           det_net_def,
   "Pad_stack",         det_pad_stack,
   "Subcomp",           det_subcomp
};
#define  SIZ_DET_LST       (sizeof(det_lst) / sizeof(List))


static   int      graph_text(),graph_line(),graph_rect(), graph_wire();
static   int      graph_fr(),graph_a(),graph_c(),graph_arc();
static   int      graph_poly(),graph_fl();
static   int      ddg_via(), ddg_nm();
static   int      graph_dimension();
static   int      graph_dimtext();

static List  graph_lst[] =
{
   "T",                    graph_text,
   "L",                    graph_line,
   "W",                    graph_wire,
   "R",                    graph_rect,
   "Fr",                   graph_fr,
   "A",                    graph_a,
   "Arc",                  graph_arc,
   "C",                    graph_c,
   "Fl",                   graph_fl,
   "Cv",                   FNULL,
   "Pv",                   FNULL,
   "Dimension",            graph_dimension,   // Not yet impl.
   "Poly",                 graph_poly,
   "V",                    ddg_via,
   "Nn",                   ddg_nm,
   "Ns",                   FNULL,
};
#define  SIZ_GRAPH_LST        (sizeof(graph_lst) / sizeof(List))


static   int   poly_ol(),poly_pv(),poly_cv();

static List  poly_lst[] = 
{
   "Polyap",            polyap,
   "Ol",                poly_ol,
   "Pv",                poly_pv,
   "Cv",                poly_cv,
};
#define  SIZ_POLY_LST         (sizeof(poly_lst) / sizeof(List))


static int  graph_arrow();

static List  dim_lst[] = 
{
   "DL",                graph_line,
   "DArc",              graph_arc,  
   "DR",                graph_rect,
   "DC",                graph_c,
   "DT",                graph_dimtext,
   "Arrow",             graph_arrow,            // this is done in in subdef and insert
};
#define  SIZ_DIM_LST         (sizeof(dim_lst) / sizeof(List))


/* Net_def in Detail PDIF sections.    */

static   int      p_net_name();

static List  net_lst[] =
{
   "N",                    p_net_name
};
#define  SIZ_NET_LST       (sizeof(net_lst) / sizeof(List))


/* Net_name in Net_def in Detail PDIF sections.    */

static   int      nnm_dg();
static   int      nnm_attr();

static List  nnm_lst[] =
{
   "dg",                   nnm_dg,
   "atr",                  nnm_attr,
};
#define  SIZ_NNM_LST       (sizeof(nnm_lst) / sizeof(List))


/* Pad_stack in Detail PDIF sections.     */

static   int      pdf_pad(),pdf_pad_def();

static List  pdf_lst[] =
{
   "Pad",               pdf_pad,
   "Pad_def",           pdf_pad_def
};
#define  SIZ_PDF_LST       (sizeof(pdf_lst) / sizeof(List))


/* Pad_def in pad_stack in Detail PDIF sections.      */

static   int      pdd_pic(),pdd_atr();

static List  pdd_lst[] =
{
   "Atr",                  pdd_atr,
   "Pic",                  pdd_pic
};
#define  SIZ_PDD_LST       (sizeof(pdd_lst) / sizeof(List))


static   int   pddatr_in();

static List  pddatr_lst[] =
{
   "IN",                   pddatr_in,
   "EX",                   FNULL,
};
#define  SIZ_PDDATR_LST       (sizeof(pddatr_lst) / sizeof(List))


static   int   netatr_in();
static   int   netatr_ex();

static List  netatr_lst[] =
{
   "IN",                   netatr_in,
   "EX",                   netatr_ex,
};
#define  SIZ_NETATR_LST       (sizeof(netatr_lst) / sizeof(List))


static   int   in_smd(), in_ty(), in_org();

static List  in_lst[] =
{
   "Org",                     in_org,
   "Ty",                      in_ty,
   "Smd",                     in_smd,
   "Jmp",                     FNULL,
};
#define  SIZ_IN_LST        (sizeof(in_lst) / sizeof(List))


static   int   nain_rats();

// nain = nets attribute internal
static List  nain_lst[] =
{
   "Rats",                    nain_rats,
   "Ns",                      FSKIP,         // net scope not needed
   "Un",                      FSKIP,         // not needed
};
#define  SIZ_NAIN_LST        (sizeof(nain_lst) / sizeof(List))


// naex = nets attribute external
static List  naex_lst[] =
{
   "Nat",                     FSKIP,         // not needed
};
#define  SIZ_NAEX_LST        (sizeof(naex_lst) / sizeof(List))


/* Pic in pad_def in pad_stack in Detail PDIF sections.     */

/* Subcomp in Detail PDIF sections.    */

static   int      cmp_comp_def(),cmp_i();

static List  cmp_lst[] =
{
   "comp_def",             cmp_comp_def,
   "I",                    cmp_i
};
#define  SIZ_CMP_LST       (sizeof(cmp_lst) / sizeof(List))


/* Comp_def in subcomp in Detail PDIF sections.    */

static   int      cpd_pin_def(),cpd_pic_def();

static List  cpd_lst[] =
{
   "pin_def",              cpd_pin_def,
   "pkg",                  FSKIP,
   "spkg",                 FSKIP,
   "pic",                  cpd_pic_def,
   "atr",                  pdd_atr,
};
#define  SIZ_CPD_LST       (sizeof(cpd_lst) / sizeof(List))


/* Pind_def in comp_def in subcomp in Detail PDIF sections.    */

static   int      pin_p();

static List  pin_lst[] =
{
   "P",                    pin_p
};
#define  SIZ_PIN_LST       (sizeof(pin_lst) / sizeof(List))


/* P in pind_def in comp_def in subcomp in Detail PDIF sections.     */

static   int      ppn_pt(), ppn_ploc(), ppn_lq();

static List  ppn_lst[] =
{
   "Pt",                   ppn_pt,
   "Lq",                   ppn_lq,
   "Ploc",                 ppn_ploc
};
#define  SIZ_PPN_LST       (sizeof(ppn_lst) / sizeof(List))


/* I in subcomp in Detail PDIF sections.     */

static   int   cmi_cn(),cmi_atr(), cmi_ipt();
static   int   cmi_asg();

static List  cmi_lst[] =
{
   "Cn",                   cmi_cn,
   "Asg",                  cmi_asg,
   "Ipt",                  cmi_ipt,
   "Atr",                  cmi_atr
};
#define  SIZ_CMI_LST       (sizeof(cmi_lst) / sizeof(List))


static int  asg_rd();
static int  asg_pn();

static List  asg_lst[] =
{
   "Rd",                   asg_rd,
   "Pn",                   asg_pn,
};
#define  SIZ_ASG_LST       (sizeof(asg_lst) / sizeof(List))


/* Atr in I in subcomp in Detail PDIF sections.    */

static   int   atr_in();
static   int   atr_ex();

static List  atr_lst[] =
{
   "IN",                   atr_in,
   "EX",                   atr_ex,
};
#define  SIZ_ATR_LST       (sizeof(atr_lst) / sizeof(List))


/* In in atr in I in subcomp in Detail PDIF sections.    */

static   int      ain_pl(),ain_sc(),ain_ro(),ain_mr(),ain_ps();
static   int      ain_iat(),ain_nl(),ain_pa();

static List  ain_lst[] =
{
   "Pl",                   ain_pl,
   "Sc",                   ain_sc,
   "Ro",                   ain_ro,
   "Pa",                   ain_pa,
   "Mr",                   ain_mr,
   "Ps",                   ain_ps,
   "Nl",                   ain_nl,
   "Iat",                  ain_iat,
   "Un",                   FSKIP,      // unnamed component
};
#define  SIZ_AIN_LST       (sizeof(ain_lst) / sizeof(List))


static int                 aex_at();

static List  aex_lst[] =
{
   "At",                   aex_at,
};
#define  SIZ_AEX_LST       (sizeof(aex_lst) / sizeof(List))

