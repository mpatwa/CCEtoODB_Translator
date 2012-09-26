
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.


   Need to do

   - drill holes
   - refdes placement
   = text type
   - device section / comp types
   - purge unused PD400 padstacks and PD560 geoms.
   
   Version 9     
   Version 9 is not implemented, only the scanner is updated.

*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "logwrite.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

static CProgressDlg  *progress;
extern char          *testaccesslayers[];

/****************************************************************************/
/*
*/

// This must left layer is Component the more right layer is Solder layer
#define ONLAYER(flag,layer) ((1L<<(32-(layer))) & flag)
#define SETLAYER(layer)  (1L<<32-(layer))

/* Typedef ******************************************************************/

typedef    unsigned long   Lm;  /* layer array */

#define  CII_LOW           2   /* lowest database definition */
#define  CII_REVISION      10  /* current highest implemented database definition */

#define  MAX_LINE          127
#define  MAX_POLY          255

#define  MAXINT            0xffffffff;

#define  MAX_PLANENET      100
#define  MAX_NET           4000
#define  MAX_GRAPH         1000
#define  MAX_PIN514        1000
#define  MAX_VECTOR        5000
#define  MAX_WIDTH         255
#define  MAX_LIBRARY       1000
#define  MAX_PIN560        1000
#define  MAX_COMPLEXPIN    20000
#define  MAX_PADSHAPE      1000

#define  MAX_NEWSHAPEPAD   1000
#define  MAX_NEWSHAPEDRILL 1000

#define  ERR_UNIT_SEQUENCE 1
#define  ERR_LINE_LENGTH   2
#define  ERR_FILE_WRITE    3

#define  PADNAME_560       "PD560-"
#define  TSTNAME_400       "TST400-"
#define  PADNAME_400       "PD400-"
#define  PADNAME_514       "PD514-"
#define  PADTOP            "PADTOP"
#define  PADBOT            "PADBOT"
#define  PADALL            "PADALL"
#define  ALLLAYERS         "ALL"
#define  PADINNER          "PADINNER"
#define  SHAPENAME         "SHAPENAME"
#define  TEXTSILKSCREEN    "TXTSILK"
#define  OLDSHAPEOUTLINETOP "OLDSHAPE_TOP"
#define  OLDSHAPEOUTLINEBOT "OLDSHAPE_BOT"

// padtype for record 544
#define  TYP_FINGER        -1

/*
      Unit Abr.    Unit Name         Unit Number
   ---------------------------------------------------------------------
*/
#define    HIS       /* History */                 60
#define    GEN       /* General Information */     80
#define    GRD       /*  Grid */                   100
#define    CRD       /* Card */                    120
#define    HLT       /* Hole Template */           140
#define    HVO       /* Voids */                   160
#define    FVO       /* Feedthru Voids */          180
#define    TRG       /* Targets  */                200
#define    CUP       /* Coupons  */                220
#define    PCB       /* General PCB */             240
#define    CND       /* Conductors */              260
#define    VEC       /* Vectors  */                280
#define    DFL       /* Deferred List  */          300
#define    CMP       /* Components */              320
#define    STR       /* Stringing Information */   340
#define    PLB       /* Plating Bar */             360
#define    PSO       /* Pad Stack Occurrences */   380
#define    PSD       /* Pad Stack Definitions  */  400
#define    LAB       /* Labels */                  420
#define    SYM       /* Shapes/Symbols */          500
#define    NSH       /* New Shapes */              520
#define    PDG       /* Pad Geometry */            560
#define    GRP       /* Groups */                  580
#define    TVO       /* Trace Voids */             600
#define    PVO       /* Placement Voids */         620
#define    DVO       /* Dielectric Voids */        640
#define    ARF       /* Areafill */                660
#define    PRT       /* Parts */                   680
#define    DFT       /* Design Data Layer */       700
#define    SET       /* Setup */                   720
#define    RULE      /* Rules */                   740   

typedef struct
{
   double w,h;
   double offx, offy;
} CIIPdshape;

typedef struct
{
  CString   name;   
  int       id;   
} CIINetname;
typedef CTypedPtrArray<CPtrArray, CIINetname*> CNetNameArray;

typedef struct
{
  int typ;
  long   x1,y1,x2,y2;   
} CIIGraph;

typedef struct // only one pin per group allowed.
{
   long  x,y;
   int   pinnr;
   CString  pinname;
   int   flg;  // 1 = electrical 2 = mechanical
   Lm    layer;
} CIINewshapepin;

typedef struct // here is the collection of padshapes per pin
{
   int   typ;     // 
   long  x,y;
   int   geomptr; // for typ == TYP_FINGER the geomptr is the PAD_RECT pointer
   long  layer;
} CIINewshapepad;

typedef struct // here is a collection of drills per pin. Multiple are allowed.
{
   long  x,y;
   long  drill;
}CIINewshapedrill;

typedef struct
{
   int            complexpadcnt;
   CIINewshapepad    *complexpad;
   int            complexdrillcnt;
   CIINewshapedrill  *complexdrill;
}CIIComplexpin;

typedef  struct
{
   long  w,h,d;
   int   shape_type;
} CIIPin514;

typedef struct
{
   int   plateflg;
   int   padflg;
   long  hole;
   long  pad1;
   long  pad2;
   int   geomptr;
} CIIPindef400;

/* only used for via */
typedef struct
{
   int   used;    /* 1 used as pin 2 used as via */
   CIIPindef400   *pindef;
} CIIPin400;

typedef struct
{
   int   typ;
   long  x1,y1,x2,y2;
} CIIGeom;

/* only used for via */
typedef struct
{
   int      geomcnt;
   CIIGeom  *geom;
} CIIPin560;

typedef  struct
{
   long  x,y;
   int   arc;
} CIIVector;


typedef struct
{
   char  *name;
   long  centerx,centery;
   long  pin1x,pin1y;
   long  height;     // here is the maximum height used.
   char  oldshape;
} CIILibrary;

typedef struct
{
   CString  name;
   CString  libname;
   int      index;
   long     x,y;
   long     pin1x, pin1y;
   int      rot;
   int      mir;
   int      pin1placement;
   CString  pin1;
}CIICompname;
typedef CTypedPtrArray<CPtrArray, CIICompname*> CCompArray;

typedef struct
{
   int      padstack;
}CIIOpposite400;
typedef CTypedPtrArray<CPtrArray, CIIOpposite400*> COpposite400Array;

typedef struct
{
   Lm     layer;
   long   drill;
} CIIPinlayer;

typedef struct
{
   char  *net;
   char  shown;
} CIIPlanenet;

/** Global      *************************************************************/

static   char        token[MAX_LINE];
static   long        linecnt;
static   int         act_ciirev;       /* actual CII revision in 20 */
static   int         num_of_layers;    /* number of layers general info */

static   CIIGraph    *graph;
static   int         graphcnt;
static   CIIPin514   *pin514;
static   int         pin514cnt;

static   CIIVector   *vector;
static   int         vectorcnt;

static   long        *widthcode;
static   int         widthcodecnt;
static   int         gridcell;
static   long        gridspacing;

static   CIINewshapepin newshapepin;

static   CIILibrary  *library;
static   int         librarycnt;

static   CIIPin400   *pin400;
static   int         pin400cnt;

static   CIIPin560   *pin560;
static   int         pin560cnt;

static   CNetNameArray  netnamearray;
static   int         netnamecnt;

static   CCompArray  comparray;
static   int         compcnt;

static   COpposite400Array opposite400array;
static   int         opposite400cnt;

static   char        area_fill;
static   CIIPlanenet *planenet;
static   int         planenetcnt;
static   int         box_fill;
static   int         pin1placement;
static   int         max560rec;
static   int         ignore_oldshape;        // always ignored except the graphic. this is set in cii.in.
static   int         testpoint_convert;
static   BOOL        TESTPOINT_382;

static   CIIComplexpin  *complexpin;
static   int         complexpin_cnt;

static   CIINewshapepad newshapepad[MAX_NEWSHAPEPAD];
static   int         newshapepadcnt;

static   CIINewshapedrill  newshapedrill[MAX_NEWSHAPEDRILL];
static   int         newshapedrillcnt;

static   int         *nodenumberlist;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static   int         testpointcnt;

/* Function declaration *****************************************************/

static   double   cii_unit(int);
static   double   cii_unit(long);
static   double   cii_unit(double);
static   long     on_grid(long);
static   void     init_cii();
static   int      update_ciitec();
static   int      process_record(FILE *);
static   int      write_padstack514();
static   int      write_padstack400();
static   int      write_oppositepadstack400();
static   int      write_widthcode();
static   void     end_cii();
static   int      unit_HIS(FILE *);
static   int      unit_GEN(FILE *);
static   int      unit_GRD(FILE *);
static   int      unit_CRD(FILE *);
static   int      unit_HLT(FILE *);
static   int      nextline(FILE *);
static   void     p_error();
static   void     l_error();
static   void     t_error(int,int);
static   void     r_error(int,int);
static   int      unit_HVO(FILE *);
static   int      unit_FVO(FILE *);
static   int      unit_TRG(FILE *);
static   int      unit_CUP(FILE *);
static   int      unit_PCB(FILE *);
static   int      unit_CND(FILE *);
static   int      unit_VEC(FILE *);
static   int      unit_DFL(FILE *);
static   int      unit_CMP(FILE *);
static   int      unit_STR(FILE *);
static   int      unit_PLB(FILE *);
static   int      unit_PSO(FILE *);
static   int      unit_PSD(FILE *);
static   int      unit_LAB(FILE *);
static   int      unit_SYM(FILE *);
static   int      unit_NSH(FILE *);
static   int      unit_PDG(FILE *);
static   int      unit_GRP(FILE *);
static   int      unit_TVO(FILE *);
static   int      unit_PVO(FILE *);
static   int      unit_DVO(FILE *);
static   int      unit_ARF(FILE *);
static   int      unit_PRT(FILE *);
static   int      unit_GRD(FILE *);
static   int      unit_HLT(FILE *);
static   int      unit_DFT(FILE *);
static   int      unit_SET(FILE *);
static   int      unit_RULE(FILE *);

static   int      numword(int, int blankyes);
static   int      nextword();
static   int      write_vector();
static   int      write_vector284();
static   int      get_netname(char *,int);
static   int      get_widthcodelong(long);
static   int      do_pad560geom(char *,int, long offx, long offy);
static   int      create_pin514(char *,long,long);
static   void     rotate(long *,long *,int);
static   int      minmax(long *,long *,long);
static   int      write_graph(char *, long width, int graphic_class, int fill);
static   int      get_libptr(char *);
static   int      update_librarygeom();
static   int      get_pin514(long,long,long,int);
static   int      extent_layer(Lm *,int);
static   int      layermask(Lm *,char *);

static   int      do_020(FILE *);
static   int      do_062(FILE *);
static   int      do_064(FILE *);
static   int      do_066(FILE *);
static   int      do_068(FILE *);
static   int      do_070(FILE *);
static   int      do_082(FILE *);
static   int      do_102(FILE *);
static   int      do_110(FILE *);
static   int      do_112(FILE *);
static   int      do_114(FILE *);
static   int      do_116(FILE *);
static   int      do_122(FILE *);            // CC done
static   int      do_124(FILE *);
static   int      do_142(FILE *);
static   int      do_162(FILE *);
static   int      do_164(FILE *, int cnt);
static   int      do_182(FILE *);
static   int      do_184(FILE *, int cnt);
static   int      do_202(FILE *);
static   int      do_221(FILE *);
static   int      do_222(FILE *);
static   int      do_224(FILE *);
static   int      do_226(FILE *);
static   int      do_228(FILE *);
static   int      do_230(FILE *);
static   int      do_232(FILE *);
static   int      do_234(FILE *);
static   int      do_236(FILE *);
static   int      do_238(FILE *);
static   int      do_242(FILE *);
static   int      do_262(FILE *,int);        // CC done
static   int      do_264(FILE *,int);
static   int      do_282(FILE *,int);
static   int      do_284(FILE *,int);  // CC done
static   int      do_286(FILE *);
static   int      do_302(FILE *);
static   int      do_322(FILE *, int);
static   int      do_324(FILE *);
static   int      do_326(FILE *);
static   int      do_328(FILE *);
static   int      do_330(FILE *);
static   int      do_331(FILE *);
static   int      do_332(FILE *);
static   int      do_342(FILE *);
static   int      do_344(FILE *);
static   int      do_362_rev2(FILE *);
static   int      do_362(FILE *);
static   int      do_364(FILE *);
static   int      do_382(FILE *);
static   int      do_402(FILE *,int,int);
static   int      do_422(FILE *);
static   int      do_424(FILE *);
static   int      do_502(FILE *);
static   int      do_504(FILE *);
static   int      do_506(FILE *);
static   int      do_508(FILE *);
static   int      do_510(FILE *);
static   int      do_511(FILE *);
static   int      do_512(FILE *);
static   int      do_514(FILE *);
static   int      do_522(FILE *);
static   int      do_524(FILE *);
static   int      do_528(FILE *);
static   int      do_530(FILE *);
static   int      do_532(FILE *,int);
static   int      do_534(FILE *,int); // pingroup
static   int      do_536(FILE *);
static   int      do_538(FILE *);
static   int      do_542(FILE *);
static   int      do_544(FILE *,int); // pingroup
static   int      do_546(FILE *);
static   int      do_548(FILE *);
static   int      do_550(FILE *);
static   int      do_552(FILE *,int);
static   int      do_562(FILE *,int);
static   int      do_564(FILE *,int,int);
static   int      do_582(FILE *);
static   int      do_583(FILE *);
static   int      do_584(FILE *);
static   int      do_585(FILE *);
static   int      do_586(FILE *);
static   int      do_587(FILE *);
static   int      do_588(FILE *);
static   int      do_589(FILE *);
static   int      do_590(FILE *);
static   int      do_591(FILE *);
static   int      do_592(FILE *);
static   int      do_593(FILE *);
static   int      do_594(FILE *);
static   int      do_595(FILE *);
static   int      do_602(FILE *);
static   int      do_604(FILE *);
static   int      do_622(FILE *);
static   int      do_624(FILE *);
static   int      do_642(FILE *);
static   int      do_642(FILE *);
static   int      do_644(FILE *);
static   int      do_662(FILE *);
static   int      do_664(FILE *ifp, long w);
static   int      do_666(FILE *ifp, long w);
static   int      do_668(FILE *ifp, long w);
static   int      do_682(FILE *);
static   int      do_684(FILE *);
static   int      do_686(FILE *);
static   int      do_688(FILE *);
static   int      do_690(FILE *);
static   int      do_691(FILE *);
static   int      do_692(FILE *);
static   int      do_702(FILE *,int);
static   int      do_704(FILE *,char *);
static   int      do_706(FILE *);
static   int      do_708(FILE *,char *);
static   int      do_710(FILE *,int);
static   int      do_742(FILE *);
static   int      do_744(FILE *);
static   int      do_746(FILE *);
static   int      do_748(FILE *);

static   int      get_widthlong(long w);
 
/****************************************************************************/

/* Revision History

   Version  Date        Remark
   2        11-Nov-93   General fixup for Sundstrand. 
   3        21-Nov-93   MAX560REC in Tec file to compensate for
                        and error in V26 / CII 7 of the scicards output.
   4        21-Jan-94   Do not die on components without name.
   5        23-Feb-94   Error fixes for Attr-Layers and Non-extended Shape
                        definitions.
   6        22-Jun-94   Version 8 implemented
   7        18-Oct-95   Arcflag in Vector 284 fixed.
   8        30-Oct-95   Multiple pad on the same pin fixed.
   9        24-Apr-96   Record 324 in L_Comptype_name
   10       23-Jun-96   Fixed extended pad layer and support of body height.
*/

#define CIIERR          "cii.log"
#define MAXWORDLEN      16

//#define NUL             ((char)0)
//#define SPACE           ((char)0x20)

/*Typedef *******************************************************************/

/*Globals *******************************************************************/

static   int         write_complexpin_graphic();

static   char        linebfr[MAX_LINE];   /* Input line buffer, very long for safety. */
static   int         linepos = 0;         /* current pos in line */

static   int         last_unit = 0;
static   int         inverse_grid = FALSE;
static   int         max_nets = 0;

static   char        prosa[65];
static   int         libclass = 0;
static   int         shapclass = 0;
static   char        libname[33];
static   char        compname[33];
static   char        type_name[33];
static   int         compplcflg;
static   int         pincnt = 0;
static   int         curr_padstack;
static   int         bot_layer,ext_layer;
static   Lm          shapelayer;
static   long        minx,miny,maxx,maxy;
static   int         flg524;
static   int         oldcomp = -1;
static   long        pitch;
static   int         number_of_components;
static   long        oldwidth,oldx,oldy=-1;
static   int         oldlay;
static   int         t_testviacnt = 0;

static   double      compx,compy;

static   int         PageUnits;
static   FileStruct  *file = NULL;
static   FILE        *ferr;

static   int         display_error = 0;
static   CCEtoODBDoc  *doc;
static   double      faktor = 1;

static   CIIPdshape  pd_circle[MAX_PADSHAPE];
static   CIIPdshape  pd_rect[MAX_PADSHAPE];
static   CIIPdshape  pd_square[MAX_PADSHAPE];
static   CIIPdshape  pd_obl[MAX_PADSHAPE];
static   int         pd_circlecnt, pd_rectcnt, pd_oblcnt, pd_squarecnt;

static   double      drill[MAX_PADSHAPE];
static   int         drillcnt;

/****************************************************************************/
/*
    CII read write routine
*/

/***************************************************************************/
/*
   Find the next word in the line buffer.
   IN:     wordptr points to the first character of a word or a preceding
         space.   
   RETURN: A pointer to the first character of the word. If there are no
        more words, a pointer to the terminating NUL.
*/
static int nextline(FILE *ifp)
{
   int  i;

   /* Each loop processes one line. NOTE: if a line is longer than the
      input buffer the program may produce invalid results. The very large
      buffer makes this unlikely. */
   if (fgets( linebfr, sizeof(linebfr), ifp) != NULL)
   {
      linecnt++;

      linepos = 0;
      if ((i = strlen(linebfr)) == 0)
      {
         fprintf(ferr,"Linelength error at line %ld\n",linecnt);
         return(ERR_LINE_LENGTH);
      }
      if ( i >= MAX_LINE)
      {
         fprintf(ferr,"Linelength error at line %ld\n",linecnt);
         return(ERR_LINE_LENGTH);
      }

      /* no crlinefeed */
      if (linebfr[i-1] == '\n') linebfr[i-1] = '\0';
      return(1);
   }
   return(0);
}

/*****************************************************************************/
/*
*/
static int nextword()
{
  int  t = 0;
  /* eliminate leading space */

  while (isspace(linebfr[linepos]) && linebfr[linepos] !='\0')
  linepos++;

  while ( !isspace(linebfr[linepos]) && linebfr[linepos] !='\0')
  {
   token[t++] = linebfr[linepos++];
  }
  token[t] = '\0';
  return(t);
}

/*****************************************************************************/
/* 
   get number of char 
*/
static int numword(int n,int blankyes)
{
   int  i,t = 0;
   /* eliminate leading space */
/* 
  while (isspace(linebfr[linepos]) && linebfr[linepos] !='\0')
  linepos++;
*/
   linepos++;

   if (linepos > (int)strlen(linebfr))
   {
      token[0] = '\0';
      return 1;
   }

   for (i=0;i<n && linebfr[linepos] !='\0';i++)
   {
     if (linebfr[linepos] != ' ' || blankyes)
     {
          token[t] = linebfr[linepos];
          t++;
     }
     linepos++;
   }

  while (t > 0 && isspace(token[t-1]))  t--;

  token[t] = '\0';
  return(1);
}

/*****************************************************************************/
/*
*/
void p_error()
{
   fprintf(ferr,"Token expected at line %ld\n",linecnt);
   display_error++;
   return;
}

/****************************************************************************/
/*
  token length error 
*/
void t_error(int act,int exp)
{
   fprintf(ferr,"Record length error : Actual [%d] Expected [%d] at line %ld\n",
      act,exp,linecnt);
   display_error++;
}

/****************************************************************************/
/*
  record length error
*/
void  r_error(int act,int exp)
{
   fprintf(ferr,"Record error : Actual [%d] Expected [%d] at line %ld\n",
      act,exp,linecnt);
   display_error++;
}

/****************************************************************************/
/*
  line expected error 
*/
void l_error()
{
   fprintf(ferr,"Next line expexted after line %ld\n",linecnt);
   display_error++;
}

/****************************************************************************/
/*
  process record
  check for sequence
  check length
  call appr. function
*/
static int process_record(FILE *ifp)
{
   int  tok;

   int  err = 0;

   tok = atoi(token);

   if (tok < last_unit)
   {
      fprintf(ferr,"Error unit sequence wrong at line %ld\n",linecnt);  
      return 1;
   }

   switch(tok)
   {
      case  1:
      case  40:
      case  42:
      case  44:
      case  46:
         /* header records are fixed and not evaluated */
      break;
      case  20:
         /* actual CII revision */
         do_020(ifp);
      break;
      case  HIS:
         if (!act_ciirev)
         {
            /* actual ciirev must have been found by now */
            fprintf(ferr,"Error : No record 20 found (CII rev)\n");
            return(err);
         }

         if (act_ciirev < CII_LOW || act_ciirev > CII_REVISION)
         {
            /* actual ciirev must have been found by now */
            fprintf(ferr,"Error : CII Revision [%d] not supported\n",act_ciirev);
            return(err);
         }
         last_unit = HIS;
         unit_HIS(ifp);
      break;
      case GEN:
         last_unit = GEN;
         unit_GEN(ifp);
      break;
      case GRD:
         last_unit = GRD;
         unit_GRD(ifp);
      break;
      case CRD:
         last_unit = CRD;
         unit_CRD(ifp);
      break;
      case HLT:
         last_unit = HLT;
         unit_HLT(ifp);
      break;
      case HVO:
         last_unit = HVO;
         unit_HVO(ifp);
      break;
      case FVO:
         last_unit = FVO;
         unit_FVO(ifp);
      break;
      case TRG:
         last_unit = TRG;
         unit_TRG(ifp);
      break;
      case CUP:
         last_unit = CUP;
         unit_CUP(ifp);
      break;
      case PCB:
         last_unit = PCB;
         unit_PCB(ifp);
      break;
      case CND:
         last_unit = CND;
         unit_CND(ifp);
      break;
      case VEC:
         last_unit = VEC;
         unit_VEC(ifp);
      break;
      case DFL:
         last_unit = DFL;
         unit_DFL(ifp);
      break;
      case CMP:
         last_unit = CMP;
         unit_CMP(ifp);
      break;
      case STR:
         last_unit = STR;
         unit_STR(ifp);
      break;
      case PLB:
         last_unit = PLB;
         unit_PLB(ifp);
      break;
      case PSO:
         last_unit = PSO;
         unit_PSO(ifp);
      break;
      case PSD:
         last_unit = PSD;
         unit_PSD(ifp);
      break;
      case LAB:
         last_unit = LAB;
         unit_LAB(ifp);
      break;
      case SYM:
         last_unit = SYM;
         unit_SYM(ifp);
      break;
      case NSH:
         last_unit = NSH;
         unit_NSH(ifp);
       break;
      case PDG:
         last_unit = PDG;
         unit_PDG(ifp);
      break;
      case GRP:
         last_unit = GRP;
         unit_GRP(ifp);
      break;
      case TVO:
         last_unit = TVO;
         unit_TVO(ifp);
      break;
      case PVO:
         last_unit = PVO;
         unit_PVO(ifp);
      break;
      case DVO:
         last_unit = DVO;
         unit_DVO(ifp);
      break;
      case ARF:
         last_unit = ARF;
         unit_ARF(ifp);
      break;
      case PRT:
         last_unit = PRT;
         unit_PRT(ifp);
      break;
      case DFT:
         last_unit = DFT;
         unit_DFT(ifp);
      break;
      case SET:
         last_unit = SET;
         unit_SET(ifp);
      break;
      case RULE:
         last_unit = RULE;
         unit_RULE(ifp);
      break;
      default:
      break;
   }
   return(0);
}

/****************************************************************************/
/*
*/
static int pad_circle(double w)
{
   int   i;
   for (i=0;i<pd_circlecnt;i++)
   {
      if (pd_circle[i].w == w)
         return i;
   }

   if (pd_circlecnt < MAX_PADSHAPE)
   {
      pd_circle[pd_circlecnt].w = w;
      pd_circlecnt++;
   }
   else
   {
      ErrorMessage("Too many Circles");
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_drillindex(double w, int layernum)
{
   int   i;
   CString  drillname;

   if (w == 0) return -1;

   for (i=0;i<drillcnt;i++)
   {
      if (drill[i] == w)
         return i;
   }

   drillname.Format("DRILL_%d",drillcnt);

   Graph_Tool(drillname, 0, w, 0, 0, 0, 0L);

/*
   Graph_Aperture(drillname, T_ROUND, w, 0.0, 0.0, 0.0, 0.0, 0, 
            BL_DRILL, TRUE, &err);
*/
   if (drillcnt < MAX_PADSHAPE)
   {
      drill[drillcnt] = w;
      drillcnt++;
   }
   else
   {
      ErrorMessage("Too many drills");
   }

   return drillcnt-1;
}

/****************************************************************************/
/*
*/
static int pad_rect(double w, double h, double offx, double offy)
{
   int   i;
   for (i=0;i<pd_rectcnt;i++)
   {
      if (pd_rect[i].w == w && pd_rect[i].h == h &&
          pd_rect[i].offx == offx && pd_rect[i].offy == offy)
         return i;
   }

   if (pd_rectcnt < MAX_PADSHAPE)
   {
      pd_rect[pd_rectcnt].w = w;
      pd_rect[pd_rectcnt].h = h;
      pd_rect[pd_rectcnt].offx = offx;
      pd_rect[pd_rectcnt].offy = offy;
      pd_rectcnt++;
   }
   else
   {
      ErrorMessage("Too many Rectangles");
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pad_square(double w)
{
   int   i;
   for (i=0;i<pd_squarecnt;i++)
   {
      if (pd_square[i].w == w)
         return i;
   }

   if (pd_squarecnt < MAX_PADSHAPE)
   {
      pd_square[pd_squarecnt].w = w;
      pd_squarecnt++;
   }
   else
   {
      ErrorMessage("Too many Squares");
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pad_obl(double w, double h)
{
   int   i;
   for (i=0;i<pd_oblcnt;i++)
   {
      if (pd_obl[i].w == w && pd_obl[i].h == h)
         return i;
   }

   if (pd_oblcnt < MAX_PADSHAPE)
   {
      pd_obl[pd_oblcnt].w = w;
      pd_obl[pd_oblcnt].h = h;
      pd_oblcnt++;
   }
   else
   {
      ErrorMessage("Too many Oblong");
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int  make_pshapename(char *pshapename,char kind, double w,double h)
{
   int   p;

   switch (kind)
   {
      case 'C':
         if ((p = pad_circle(w)) < 0)
         {
            sprintf(pshapename,"PS_CIRC_%d",pd_circlecnt-1);
            int   err;
            Graph_Aperture(pshapename, T_ROUND, w, 0.0 , 0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         }
         else
            sprintf(pshapename,"PS_CIRC_%d",p);
      break;
      case 'R':
         if ((p = pad_rect(w, h, 0, 0)) < 0)
         {
            sprintf(pshapename,"PS_RECT_%d",pd_rectcnt-1);
            int   err;
            Graph_Aperture(pshapename, T_RECTANGLE, w, h , 0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         }
         else
            sprintf(pshapename,"PS_RECT_%d",p);
      break;
      case 'S':
         if ((p = pad_square(w)) < 0)
         {
            sprintf(pshapename,"PS_SQUARE_%d",pd_squarecnt-1);
            int   err;
            Graph_Aperture(pshapename, T_SQUARE, w, 0.0 , 0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         }
         else
            sprintf(pshapename,"PS_SQUARE_%d",p);
      break;
      case 'O':
         if ((p = pad_obl(w,h)) < 0)
         {
            sprintf(pshapename,"PS_OBL_%d",pd_oblcnt-1);
            int   err;
            Graph_Aperture(pshapename, T_OBLONG, w, h , 0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
         }
         else
            sprintf(pshapename,"PS_OBL_%d",p);
      break;
   }
   return 1;
}

/****************************************************************************/
/*
  unit HIS
*/
static int unit_HIS(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_062(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_062\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 020
*/
static int do_020(FILE *ifp)
{
   int  err  = 0;

   if (!nextword())  p_error();  /* length */
   if (atoi(token) != 2)  t_error(atoi(token),2);

   if (!nextword())  p_error();
   act_ciirev = atoi(token);
   if (act_ciirev > CII_REVISION || act_ciirev < CII_LOW)
      fprintf(ferr,"CII File Revision [%d] must be between [%d] and [%d] and may cause problems\n",
            act_ciirev,CII_LOW,CII_REVISION);
   else         
   {
      fprintf(ferr,"CII File Revision [%d]\n",act_ciirev);
      display_error++;
   }

   return(err);
}

/****************************************************************************/
/*
  do 062
*/
static int do_062(FILE *ifp)
{
   int  i,len;
   int  err  = 0;
   int  num_his,num_fld;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 62) r_error(atoi(token),62);

   if (!nextword())  p_error();
   len = atoi(token);

   switch (act_ciirev)
   {
      case  2:
         if (len != 28) t_error(len,28);
      break;
      default:
         if (len != 44) t_error(len,44);
      break;        
   }

   switch (act_ciirev)
   {
      case  2:
        if (!numword(32,0))  p_error();
        if (!nextword())  p_error();
      break;
      default:
         if (!numword(64,0))  p_error();
         if (!nextword())  p_error();
      break;
   }

   if (!numword(8,0))  p_error();
   if (!numword(8,0))  p_error();
   if (!nextword())  p_error();
   num_his = atoi(token);
   if (!nextword())  p_error();
   num_fld = atoi(token);

   /* read 64 */
   if ((err = do_064(ifp)) != 0)
   {
      fprintf(ferr,"Error in do_064\n");
      return -1;
   }

   for (i=0;i<num_his;i++)
      if ((err = do_066(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_062\n");
         return -1;
      }

   for (i=0;i<num_fld;i++)
   {
      if ((err = do_068(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_068\n");
         return -1;
      }
      if ((err = do_070(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_070\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 064
*/
static int do_064(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 64) r_error(atoi(token),64);

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 32) t_error(len,32);

   if (!numword(32,0))  p_error();
   if (!numword(32,0))  p_error();

   return(err);
}

/****************************************************************************/
/*
  do 066
*/
static int do_066(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 66) r_error(atoi(token),66);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 32) t_error(len,32);

   if (!numword(64,0))  p_error();

   return(err);
}

/****************************************************************************/
/*
  do 068
*/
static int do_068(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 68) r_error(atoi(token),68);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 32) t_error(len,32);

   if (!numword(64,0))  p_error();

   return(err);
}

/****************************************************************************/
/*
  do 070
*/
static int do_070(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 70) r_error(atoi(token),70);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 32) t_error(len,32);

   if (!numword(64,0))  p_error();

   return(err);
}

/****************************************************************************/
/*
  unit GEN
*/
static int unit_GEN(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 4) t_error(len,4);

   if (!nextword())  p_error();
   num_of_layers = atoi(token);
   if (!nextword())  p_error();
   num_rec = atoi(token);
   if (!nextword())  p_error();
   gridcell = atoi(token);

   /* to accommodate different outputs */
   if (inverse_grid)
   {
      if (gridcell)
         gridcell = 0;
      else
         gridcell = 1;
   }

   if (!nextword())  p_error();

   for (i=0;i<num_rec;i++)
      if ((err = do_082(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_082\n");
         return -1;
      }

   return(err);
}

/****************************************************************************/
/*
  do 082
*/
static int do_082(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 82) r_error(atoi(token),82);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 32) t_error(len,32);

   if (!numword(64,0))  p_error();
   return(err);
}

/****************************************************************************/
/*
  unit GRD
*/
static int unit_GRD(FILE *ifp)
{
   int   len,i;
   int   err  = 0;
   int   num_rec;
   long  pp,pt,tt;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 11) t_error(len,11);

   if (!nextword())  p_error();
   gridspacing = atol(token); 
   if (!nextword())  p_error();
   pp = atol(token); 
   if (!nextword())  p_error();
   pt = atol(token); 
   if (!nextword())  p_error();
   tt = atol(token); 
   if (!nextword())  p_error();
   /* rec.u.rec100.igrd_diam = atol(token); */
   if (!nextword())  p_error();
   num_rec = atoi(token);

   // here start polygon

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_102(ifp)) != 0)
      {
         fprintf(ferr,"Error do_102\n");
         return -1;
      }
   }

   if ((err = do_110(ifp)) != 0)
   {             
      fprintf(ferr,"Error do_110\n");
      return -1;
   }

   if ((err =   do_114(ifp)) != 0)
   {
      fprintf(ferr,"Error do_114\n");
      return -1;
   }

   return(err);
}

/****************************************************************************/
/*
  do 102
*/
static int do_102(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 102) r_error(atoi(token),102);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   /* rec.u.rec102.igrdx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec102.igrdy = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec102.i2grd_arcflg = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 110
*/
static int do_110(FILE *ifp)
{
   int  i,len;
   int  err  = 0;
   int  num_rec;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 110) r_error(atoi(token),110);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
      if ((err = do_112(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_112\n");
         return -1;
      }

   return(err);
}

/****************************************************************************/
/*
  do 112
*/
static int do_112(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 112) r_error(atoi(token),112);
   if (!nextword())  p_error();

   len = atoi(token);

   switch (act_ciirev)
   {
      case  2:
      case  4:
         if (len != 6) t_error(len,6);
      break;
      default:
         if (len != 7) t_error(len,7);
      break;        
   }

   if (!nextword())  p_error();
   /* rec.u.rec112.ipad_width = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec112.ipad_hight = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec112.ianti_diam = atol(token); */

   switch (act_ciirev)
   {
      case  2:
      case  3:
      case  4:
      break;
      case  5:
         if (!nextword())  p_error();
         /* rec.u.rec112.i2tanti_type = atoi(token); */
      break;
   }

   return(err);
}

/****************************************************************************/
/*
  do 114
*/
static int do_114(FILE *ifp)
{
   int  i,len;
   int  err  = 0;
   int  num_rec;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 114) r_error(atoi(token),114);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
      if ((err = do_116(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_116\n");
         return -1;
      }

   return(err);
}

/****************************************************************************/
/*
  do 116
*/
static int do_116(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 116) r_error(atoi(token),116);
   if (!nextword())  p_error();
   len = atoi(token);

   switch(act_ciirev)
   {
      case  2:
      case  3:
      case  4:
        if (len != 6) t_error(len,6);
      break;
      default:
        if (len != 7) t_error(len,7);
   }

   if (!nextword())  p_error();
   /* rec.u.rec116.itherm_width = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec116.itherm_pitch = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec116.itherm_diam = atol(token); */

   switch (act_ciirev)
   {
      case  2:
      case  3:
      case  4:
      break;
      default:
         if (!nextword())  p_error();
         /* rec.u.rec116.i2therm_type = atoi(token); */
   }

   return(err);
}

/****************************************************************************/
/*
  unit CRD
*/
static int unit_CRD(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  crd_num,tool_num;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 8) t_error(len,8);

   if (!nextword())  p_error();
   /* rec.u.rec120.icrdedge = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec120.icrdwidth = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec120.icrd_diam = atol(token); */
   if (!nextword())  p_error();
   crd_num = atoi(token);
   if (!nextword())  p_error();
   tool_num = atoi(token);
   vectorcnt = 0;
   for (i=0;i<crd_num;i++)
   {
      if ((err = do_122(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_112\n");
         return -1;
      }
   }

   vector[vectorcnt].x = vector[0].x;
   vector[vectorcnt].y = vector[0].y;
   vector[vectorcnt].arc = vector[0].arc;
   vectorcnt++;

   int level = Graph_Level("BOARDOUTLINE","",0);
   int widthindex = 0;  
   DataStruct *d = Graph_PolyStruct(level,0,0);
   d->setGraphicClass(GR_CLASS_BOARDOUTLINE);

   int filled = 0;
   int closed = 1;
   Graph_Poly(NULL, widthindex, filled , 0, closed);  // fillflag , negative, closed
   write_vector();
   
   vectorcnt = 0;
   for (i=0;i<tool_num;i++)
   {
      if ((err = do_124(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_114\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 122
*/
static int do_122(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 122) r_error(atoi(token),122);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (vectorcnt < MAX_VECTOR)
   {  
      if (!nextword())  p_error();
      vector[vectorcnt].x = on_grid(atol(token)); 
      if (!nextword())  p_error();
      vector[vectorcnt].y = on_grid(atol(token)); 
      if (!nextword())  p_error();
      vector[vectorcnt].arc = atoi(token); 
      vectorcnt++;
   }
   else
   {
      fprintf(ferr,"Too many vectors in CARD\n");
      return -1;
   }     
   return(err);
}

/****************************************************************************/
/*
  do 124
*/
static int do_124(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 124) r_error(atoi(token),124);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 10) t_error(len,10);

   if (!nextword())  p_error();
   /* rec.u.rec124.itool_width = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec124.icrd_begx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec124.icrd_begy = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec124.icrd_endx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec124.icrd_endy = atol(token); */

   return(err);
}

/*****************************************************************************/
/*
  unit HLT
*/
static int unit_HLT(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  mat_num;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   mat_num = atoi(token);

   for (i=0;i<mat_num;i++)
   {
      if ((err = do_142(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_142\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 142
*/
static int do_142(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 142) r_error(atoi(token),142);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 10) t_error(len,10);

   if (!nextword())  p_error();
   /* rec.u.rec142.i_hltx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec142.i_hlty = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec142.i2num_xhole = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec142.i2num_yhole = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec142.ixspacing = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec142.iyspacing = atol(token); */

   fprintf(ferr,"Hole Unit 142 not implemented.\n");
   display_error++;

   return(err);
}

/****************************************************************************/
/*
  unit HVO
*/
static int unit_HVO(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_162(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_162\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 162
*/
static int do_162(FILE *ifp)
{
   int   i,len;
   int   err  = 0;
   int   num_rec;
   long  diam;
   Lm    layer;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 162) r_error(atoi(token),162);
   if (!nextword())  p_error();
   len = atoi(token);
   switch (act_ciirev)
   {
      case  2:
         if (len != 5) t_error(len,5);
      break;
      default:
         if (len != 7) t_error(len,7);
      break;        
   }

   if (!nextword())  p_error();
   layermask(&layer,token);
   if (!nextword())  p_error();
   diam = atol(token); 
   if (!nextword())  p_error();
   num_rec = atoi(token);

   if (act_ciirev > 2)
   {
      if (!nextword())  p_error();
      /* rec.u.rec162.iptr_hvo2cmpvgrp = atol(token); */
   }

   vectorcnt = 0;
   for (i=0;i<num_rec;i++)
   {
      if ((err = do_164(ifp, i)) != 0)
      {
        fprintf(ferr,"Error in do_164\n");
         return -1;
      }
   }    

   // if only 1 vertex, this is a circle void
   Lm  alllayers = 0;

   for (i=0;i<num_of_layers;i++)
   {
      alllayers |= (1L << (32 - (i+1)));
   }

   if (vectorcnt > 1)
   {
      vector[vectorcnt].x = vector[0].x;
      vector[vectorcnt].y = vector[0].y;
      vector[vectorcnt].arc = vector[0].arc;
      vectorcnt++;
   }

   if (layer == alllayers)
   {
      int level = Graph_Level(ALLLAYERS,"",0);
      int widthindex = 0;  

      if (vectorcnt > 1)
      {
         int widthindex = 0;  
         DataStruct *d = Graph_PolyStruct(level,0,0);
         d->setGraphicClass(GR_CLASS_VIAKEEPOUT);

         int filled = 0;
         int closed = 1;
         Graph_Poly(NULL, widthindex, filled , 0, closed);  // fillflag , negative, closed
         write_vector();
      }
      else
      {
         int widthindex = 0;  
         DataStruct *d = Graph_Circle(level, cii_unit(vector[0].x), cii_unit(vector[0].y), 
            cii_unit(diam/2), 0, 0, 0, 0);
         d->setGraphicClass(GR_CLASS_VIAKEEPOUT);
      }
   }
   else
   {
      for (i=0;i<32;i++)
      {
         if (layer & (1L << (32-(i+1))))
         {
            char  ln[10];
            sprintf(ln,"%d",i+1);  
            int level = Graph_Level(ln,"",0);

            if (vectorcnt > 1)
            {
               int widthindex = 0;  
               DataStruct *d = Graph_PolyStruct(level,0,0);
               d->setGraphicClass(GR_CLASS_VIAKEEPOUT);

               int filled = 0;
               int closed = 1;
               Graph_Poly(NULL, widthindex, filled , 0, closed);  // fillflag , negative, closed
               write_vector();
            }
            else
            {
               int widthindex = 0;  
               DataStruct *d = Graph_Circle(level, cii_unit(vector[0].x), cii_unit(vector[0].y), 
                  cii_unit(diam/2), 0, 0, 0, 0);
               d->setGraphicClass(GR_CLASS_VIAKEEPOUT);
            }
         }
      }
   }
   vectorcnt = 0;

   return(err);
}

/****************************************************************************/
/*
  do 164
*/
static int do_164(FILE *ifp, int cnt)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 164) r_error(atoi(token),164);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   vector[cnt].x = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[cnt].y = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[cnt].arc = atoi(token); 
 
   return(err);
}

/****************************************************************************/
/*
  unit FVO
*/
static int unit_FVO(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_182(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_182\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 182
*/
static int do_182(FILE *ifp)
{
   int   i,len;
   int   err  = 0;
   int   num_rec;
   Lm    layer;   
   long  diameter;
   
   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 182) r_error(atoi(token),182);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 7) t_error(len,7);

   if (!nextword())  p_error();
   layermask(&layer,token); 
   if (!nextword())  p_error();
   /* rec.u.rec182.iptr_fvotocompvgrp = atol(token); */
   if (!nextword())  p_error();
   diameter = atol(token); 
   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_184(ifp, i)) != 0)
      {
         fprintf(ferr,"Error in do_184\n");
         return -1;
      }
   }    

   /* duplicate last */
   vectorcnt = num_rec;
   vector[vectorcnt].x = vector[0].x;
   vector[vectorcnt].y = vector[0].y;
   vector[vectorcnt].arc = 0;
   vectorcnt++;

   Lm  alllayers = 0;

   for (i=0;i<num_of_layers;i++)
   {
      alllayers |= (1L << (32 - (i+1)));
   }

   if (layer == alllayers)
   {
      int level = Graph_Level(ALLLAYERS,"",0);
      int widthindex = 0;  

      DataStruct *d = Graph_PolyStruct(level,0,0);
      d->setGraphicClass(GR_CLASS_VIAKEEPOUT);
      int filled = 0;
      int closed = 1;
      Graph_Poly(NULL,widthindex, filled , 0, closed);   // fillflag , negative, closed
      write_vector();
   }
   else
   {
      for (i=0;i<32;i++)
      {
         if (layer & (1L << (32-(i+1))))
         {
            char  ln[10];
            sprintf(ln,"%d",i+1);  
            int level = Graph_Level(ln,"",0);
            int widthindex = 0;  

            DataStruct *d = Graph_PolyStruct(level,0,0);
            d->setGraphicClass(GR_CLASS_VIAKEEPOUT);
            int filled = 0;
            int closed = 1;
            Graph_Poly(NULL,widthindex, filled , 0, closed);   // fillflag , negative, closed
            write_vector();
         }
      }
   }
   vectorcnt = 0;

   return(err);
}

/****************************************************************************/
/*
  do 184
*/
static int do_184(FILE *ifp, int cnt)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 184) r_error(atoi(token),184);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   vector[cnt].x = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[cnt].y = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[cnt].arc = atoi(token); 

   return(err);
}

/****************************************************************************/
/*
  unit TRG
*/
static int unit_TRG(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;
   
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_202(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_202\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 202
*/
static int do_202(FILE *ifp)
{
   int      len;
   int      err  = 0;
   long     x, y;
   int      padst;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 202) r_error(atoi(token),202);
   if (!nextword())  p_error();
   
   if (act_ciirev > 5)
   {
      if ((len = atoi(token)) != 7) t_error(len,7);
   }
   else
   {
      if ((len = atoi(token)) != 5) t_error(len,5);
   }

   if (!nextword())  p_error();
   x = on_grid(atol(token));
   if (!nextword())  p_error();
   y = on_grid(atol(token)); 
   if (!nextword())  p_error();
   padst = atoi(token);

   if (act_ciirev > 5)
   {
      if (!nextword())  p_error();  /* soldermask override */
   }

   CString  padname;
   padname.Format("%s%d",PADNAME_400,padst);
   
   DataStruct *d = Graph_Block_Reference(padname, NULL, -1, cii_unit(x),cii_unit(y),
                           DegToRad(0), 0 , 1.0, -1, TRUE);

   d->getInsert()->setInsertType(insertTypeFreePad);


   return(err);
}

/****************************************************************************/
/*
  unit CUP
*/
static int unit_CUP(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_221(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_221\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 221
*/
static int do_221(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_inst,num_crn;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 221) r_error(atoi(token),221);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 24) t_error(len,24);

   if (!numword(32,0))  p_error();
   if (!nextword())  p_error();
   if (!nextword())  p_error();
   if (!nextword())  p_error();
   if (!nextword())  p_error();
   num_inst = atoi(token);
   if (!nextword())  p_error();
   num_crn  = atoi(token);

   for (i=0;i<num_inst;i++)
   {
      if((err = do_222(ifp))!=0)
      {
         fprintf(ferr,"Error in do_222\n");
         return -1;
      }
   }

   for (i=0;i<num_crn;i++)
   {
      if((err = do_224(ifp))!=0)
      {
         fprintf(ferr,"Error in do_224\n");
         return -1;
      }
   }

   if((err = do_226(ifp))!=0)
   {
      fprintf(ferr,"Error in do_226\n");
      return -1;
   }

   return(err);
}

/****************************************************************************/
/*
  do 222
*/
static int do_222(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 222) r_error(atoi(token),222);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 27) t_error(len,27);

   if (!numword(32,0))  p_error();
   if (!nextword())  p_error();
   /* rec.u.rec222.i2cup_rot = atoi(token); */
   if (!nextword())  p_error();
   if (!nextword())  p_error();

   if (!nextword())  p_error();
  
   if (!nextword())  p_error();
   if (!nextword())  p_error();

   return(err);
}  

/****************************************************************************/
/*
  do 224
*/
static int do_224(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 224) r_error(atoi(token),224);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 4) t_error(len,4);
   
   if (!nextword())  p_error();
   /* rec.u.rec224.icup_cornerx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec224.icup_cornery = atol(token); */

   return(err);
}

/****************************************************************************/
/*
  do 226
*/
static int do_226(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int   target,pso,label,line,arc,cir;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 226) r_error(atoi(token),226);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 6) t_error(len,6);

   if (!nextword())  p_error();
   target = atoi(token);
   if (!nextword())  p_error();
   pso = atoi(token);
   if (!nextword())  p_error();
   label = atoi(token);
   if (!nextword())  p_error();
   line = atoi(token);
   if (!nextword())  p_error();
   arc = atoi(token);
   if (!nextword())  p_error();
   cir = atoi(token);

   for (i=0;i<target;i++)
   {
      if ((err = do_228(ifp))!=0)
      {
         fprintf(ferr,"Error do_228\n");
         return -1;
      }
   }

   for (i=0;i<pso;i++)
   {
      if ((err = do_230(ifp))!=0)
      {
         fprintf(ferr,"Error do_230\n");
         return -1;
      }
   }

   for (i=0;i<label;i++)
   {
      if ((err = do_232(ifp))!=0)
      {
         fprintf(ferr,"Error do_232\n");
         return -1;
      }
   }

   for (i=0;i<line;i++)
   {
      if ((err = do_234(ifp))!=0)
      {
         fprintf(ferr,"Error do_234\n");
         return -1;
      }
   }

   for (i=0;i<arc;i++)
   {
      if ((err = do_236(ifp))!=0)
      {
         fprintf(ferr,"Error do_236\n");
         return -1;
      }
   }

   for (i=0;i<cir;i++)
   {
      if ((err = do_238(ifp))!=0)
      {
        fprintf(ferr,"Error do_238\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 228
*/
static int do_228(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 228) r_error(atoi(token),228);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   /* rec.u.rec228.icup_targx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec228.icup_targy = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec228.i2ptr_cup2psd = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 230
*/
static int do_230(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 230) r_error(atoi(token),230);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   /* rec.u.rec230.icup_psox = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec230.icup_psoy = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec230.i2num_cup2psd = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 232
*/
static int do_232(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 232) r_error(atoi(token),232);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 41) t_error(len,41);

   if (!nextword())  p_error(); 
   /* layermask(&rec.u.rec232.icup_labmask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec232.itext_ht = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec232.i2label_rot = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec232.icup_labx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec232.icup_laby = atol(token); */
   if (!numword(64,0))  p_error();
   /* strncpy(&rec.u.rec232.alabel_txt[0],&token[0],sizeof(rec.u.rec232.alabel_txt)); */

   return(err);
}

/****************************************************************************/
/*
  do 234
*/
static int do_234(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 234) r_error(atoi(token),234);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 12) t_error(len,12);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec234.icup_lnmask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec234.icup_lnwidth = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec234.icup_begx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec234.icup_begy = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec234.icup_endx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec234.icup_endy = atol(token); */

   return(err);
}

/****************************************************************************/
/*
  do 236
*/
static int do_236(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 236) r_error(atoi(token),236);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 16) t_error(len,16);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec236.icup_arcmask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec236.icup_arcwdth = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec236.icup_arcx1 = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec236.icup_arcy1 = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec236.icup_arcx2 = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec236.icup_arcy2 = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec236.icup_arcx3 = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec236.icup_arcy3 = atol(token); */

   return(err);
}

/****************************************************************************/
/*
  do 238
*/
static int do_238(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 238) r_error(atoi(token),238);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 10) t_error(len,10);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec238.icup_cirmask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec238.icup_cirwidth = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec238.icup_cntrx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec238.icup_cntry = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec238.icup_rad = atol(token); */

   return(err);
}

/****************************************************************************/
/*
  unit PCB
*/
static int unit_PCB(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;
   char   netname[33];
   long   defaultwidth;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 3) t_error(len,3);

   if (!nextword())  p_error(); 
   max_nets = atoi(token); 

   if (nodenumberlist == NULL)   // otherwise it was already assigned.
   {
      if ((nodenumberlist = (int *)calloc(max_nets+1, sizeof(int))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   }

   if (!nextword())  p_error();
   /* rec.u.rec240.i2connect_flg = atoi(token); */
   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
       if ((err = do_242(ifp)) != 0)
        {
          fprintf(ferr,"Error in do_242\n");
           return -1;
        }
   }
   
   /* initialize all nets */
   defaultwidth = gridspacing /2;
   for (i=0;i<=max_nets;i++)
   {
      get_netname(netname,i);
/*
      if (i > 0)
         flg = NE_ELECTRICAL;
      else
         flg = NE_2DLINES;

      L_NetAttr(netname,inch_2_dbunit(cii_unit(defaultwidth)),
                get_widthlong(defaultwidth),get_widthlong(defaultwidth),
                get_widthlong(defaultwidth),flg);
*/
   }

   return(err);
}

/****************************************************************************/
/*
  do 242
*/
static int do_242(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();
   
   if (!nextword())  p_error();
   if (atoi(token) != 242) r_error(atoi(token),242);
   if (!nextword())  p_error();
   len = atoi(token);

   if (act_ciirev > 8)
   {
      if (len != 19) t_error(len,19);
   }
   else
   if (act_ciirev > 2)
   {
      if (len != 17) t_error(len,17);
   }
   else
   {
      if (len != 33) t_error(len,33);
   }

   if (!numword(32,0))  p_error();

   CIINetname *p = new CIINetname; 
   netnamearray.SetAtGrow(netnamecnt,p);
   netnamecnt++;

   p->name = token;

   if (!nextword())  p_error();
   p->id = atoi(token);

   if (atoi(token) > max_nets)
   {
      fprintf(ferr,"Nodenumber is larger than max nodes\n");
   }
   else
   {
      nodenumberlist[atoi(token)] = netnamecnt;
   }      

   if (act_ciirev > 8)
   {
      if (!nextword())  p_error();  // ??? rec 244
      int rec_244 = atoi(token);
      if (!nextword())  p_error();  // ??? rec 246
      int rec_246 = atoi(token);

		int i=0;
      for (i=0;i<rec_244; i++)
      {
         if (!nextline(ifp))  l_error();
      }
      for (i=0;i<rec_246; i++)
      {
         if (!nextline(ifp))  l_error();
      }
   }

   return(err);
}

/****************************************************************************/
/*
*/
void init_cii()
{

   pd_circlecnt = 0;
   pd_rectcnt   = 0;
   pd_oblcnt    = 0;
   pd_squarecnt = 0;
   drillcnt     = 0;

   if ((library = (CIILibrary *)calloc(MAX_LIBRARY,sizeof(CIILibrary))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   librarycnt = 0;

   comparray.SetSize(10,10);
   compcnt = 0;      // need

   opposite400array.SetSize(10,10);
   opposite400cnt = 0;     // need

   netnamearray.SetSize(10,10);
   netnamecnt = 0;      // need

   if ((vector = (CIIVector *)calloc(MAX_VECTOR,sizeof(CIIVector))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   vectorcnt = 0;

   if ((graph = (CIIGraph *)calloc(MAX_GRAPH,sizeof(CIIGraph))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

   graphcnt = 0;

   if ((pin514 = (CIIPin514 *)calloc(MAX_PIN514,sizeof(CIIPin514))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   pin514cnt = 0;

   pin400cnt = 0;
   pin560cnt = 0;

   if ((complexpin = (CIIComplexpin *)calloc(MAX_COMPLEXPIN,sizeof(CIIComplexpin))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   complexpin_cnt = 0;

   nodenumberlist = NULL;

}

/****************************************************************************/
/*
*/
void end_cii()
{
   int   i,l;

   for (i=0;i<pin560cnt;i++)
   {
      for (l=0;l<pin560[i].geomcnt;l++)
         free(pin560[i].geom);
   }
   if (pin560cnt)    free(pin560);
   pin560cnt = 0;

   for (i=0;i<pin400cnt;i++)
      free(pin400[i].pindef);
   if (pin400cnt)    free(pin400);
   pin400cnt = 0;

   for (i=0;i<netnamecnt;i++)
   {
      delete netnamearray[i];
   }

   free(vector);
   vectorcnt = 0;
   free(graph);
   graphcnt = 0;
   free(pin514);
   pin514cnt = 0;
   free(widthcode);
   widthcodecnt = 0;

   for (i=0;i<complexpin_cnt;i++)
   {
      if (complexpin[i].complexpad != NULL)
      {
         free(complexpin[i].complexpad);
      }
      if (complexpin[i].complexdrill != NULL)
      {
         free(complexpin[i].complexdrill);
      }
   }

   free(complexpin);
   complexpin_cnt = 0;

   for (i=0;i<compcnt;i++)
   {
      delete comparray[i];
   }

   for (i=0;i<opposite400cnt;i++)
   {
      delete opposite400array[i];
   }

   for (i=0;i<librarycnt;i++)
   {
/*
      if (library[i].height)
         L_SHeight(library[i].name,inch_2_dbunit(cii_unit(library[i].height)));
*/
      free(library[i].name);
   }
   free(library);
   librarycnt = 0;

   if (nodenumberlist)
      free(nodenumberlist);

   if (planenet)
   {
      for (i=0;i<planenetcnt;i++)
         free(planenet[i].net);
      free(planenet);
      planenet = NULL;
   }
}

/****************************************************************************/
/*
*/
static int get_widthlong(long w)
{
   int   i;

   for (i=0;i<widthcodecnt;i++)
   {
      if (widthcode[i] == w)
         return(i);
   }
   
   if (widthcodecnt < MAX_WIDTH)
   {
      // L_Width(inch_2_dbunit(cii_unit(w)),widthcodecnt);
      widthcode[widthcodecnt] = w;
      widthcodecnt++;
   }
   return(widthcodecnt-1);

}

/****************************************************************************/
/*
*/
static int write_vector()
{
   int      i;
   double   cx,cy,rad,sa,da;

   for (i=0;i<vectorcnt;i++)
   {
      if ((i+2 < vectorcnt) && vector[i+1].arc)
      {
         /* make arc */
         // coo is point on ARC
         // point i is first point
         // point i+1 is mid point
         // point i+2 is end point
         ArcPoint3(cii_unit(vector[i].x),cii_unit(vector[i].y),
                   cii_unit(vector[i+1].x),cii_unit(vector[i+1].y),
                   cii_unit(vector[i+2].x),cii_unit(vector[i+2].y),
                   &cx,&cy,&rad,&sa,&da);
         Graph_Vertex(cii_unit(vector[i].x),cii_unit(vector[i].y),tan(da/4));
         i++; /* advance when there is an arc ! */
      }
      else
      {
         Graph_Vertex(cii_unit(vector[i].x),cii_unit(vector[i].y),0.0);
      }
   }

   vectorcnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int write_vector284()
{
   int      i;
   double   rad, sa, da;
   CPnt     *lastp;

   for (i=0;i<vectorcnt;i++)
   {
      if ((i > 0) && (i < vectorcnt-1) && vector[i+1].arc)
      {
         /* make arc */
         // coo is point on ARC
         // point i is first point
         // point i+1 is end point
         // point i+2 is mid point
/*
         ArcCenter2(cii_unit(vector[i].x),cii_unit(vector[i].y),
                   cii_unit(vector[i+2].x),cii_unit(vector[i+2].y),
                   cii_unit(vector[i+1].x),cii_unit(vector[i+1].y),
                   &rad,&sa,&da);
*/

         double cx, cy;
         ArcPoint3(cii_unit(vector[i-1].x),cii_unit(vector[i-1].y),
                   cii_unit(vector[i+1].x),cii_unit(vector[i+1].y),
                   cii_unit(vector[i].x),cii_unit(vector[i].y),
                   &cx,&cy,&rad,&sa,&da);
         lastp->bulge = (DbUnit)tan(da/4);
         lastp = Graph_Vertex( cii_unit(vector[i].x), cii_unit(vector[i].y), 0.0);
         i++;
      }
      else
      {
         lastp = Graph_Vertex( cii_unit(vector[i].x), cii_unit(vector[i].y), 0.0);
      }
   }

   vectorcnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
long on_grid(long x)
{
   if (!gridcell)
      return(x + gridspacing /2);
   return(x);
}

/****************************************************************************/
/*
  convert layer from ASCII string -1-2-31-
*/
static int next_layer(char *a,int *ptr)
{
   int  i,t;

   t = 0;
   i = *ptr;

   // This is correct. Advange to next digit.
   for (;!isdigit(a[i]) && a[i] != '\0';i++)
      t = 0;   // this is only to avoid a waring

   while(isdigit(a[i]) && a[i] != '\0')
   {
      token[t++] = a[i++];
   }
    token[t] = '\0';
   *ptr = i;
   return(t);
}

/****************************************************************************/
/*
*/
static int layermask(Lm *l,char *a)
{
   /*
    [3] [2] [1] [0]
   */
   unsigned int  i;
   int  tokptr;
   Lm    lay;

   tokptr = 0;
   lay = 0;

   /* empty all non digits */
   for (i=0;i<strlen(a);i++)
      if (!isdigit(a[i])) a[i] = ' ';

   while (next_layer(a,&tokptr))
   {
      int ll = atoi(token);
      if (ll < 1 || ll > 32)  
      {
         *l = 0;
         return 0;
      }
      lay |= 1L << (32- ll);
   }
   *l = lay;
   return(0);
}

/****************************************************************************/
/*
*/
static int do_pad560geom(char *layer,int g, long offx, long offy)
{
   int      ii;
   long     startx = 0L,starty =0L;
// double   cx,cy,rad,sa,da;
   DbFlag   flg = 0;
   int      res = 0;
   int      lay = Graph_Level(layer,"",0);

   for (ii = 0; ii < pin560[g].geomcnt; ii++)
   {
      /* here do graphic */
      switch (pin560[g].geom[ii].typ)
      {
         case  1: /* line */
            fprintf(ferr,"Line point in 560 not implemented at %ld\n", linecnt);
            display_error++;

flg = 0;
#ifdef WOLF
            LG_Segm(0,inch_2_dbunitpoint2(cii_unit(startx),cii_unit(starty)),
               flg,layer,get_widthlong(0L),
               inch_2_dbunit(cii_unit(pin560[g].geom[ii].x1-startx)),
               inch_2_dbunit(cii_unit(pin560[g].geom[ii].y1-starty)));
            startx = pin560[g].geom[ii].x1 + offx;
            starty = pin560[g].geom[ii].y1 + offy;
            flg = GR_CONTINUE;
            res = 1;
#endif
         break;
         case  2: /* arc */
            fprintf(ferr,"Arc point in 560 not implemented at %ld\n", linecnt);
            display_error++;

flg = 0;
#ifdef WOLF
            /* make arc */
            ArcPoint3(cii_unit(startx),cii_unit(starty),
               cii_unit(pin560[g].geom[ii].x1),cii_unit(pin560[g].geom[ii].y1),
               cii_unit(pin560[g].geom[ii].x2),cii_unit(pin560[g].geom[ii].y2),
               &cx,&cy,&rad,&sa,&da);
            sa = RadToDeg(sa);
            if (sa < 0)       sa +=360.0;
            da = RadToDeg(da);
            LG_Arc(0,inch_2_dbunitpoint2(cx,cy),
                    flg,layer,get_widthlong(0L), /* widthcode */
                    inch_2_dbunit(rad),
                    degree_2_dbunit(sa),degree_2_dbunit(da));
            startx = pin560[g].geom[ii].x2 + offx;
            starty = pin560[g].geom[ii].y2 + offy;
            flg = GR_CONTINUE;
            res = 1;
#endif
         break;
         case  3: /* circle */
         {
            char  pshapename[80];   
            if (pin560[g].geom[ii].x2) // must have a size
            {
               make_pshapename(pshapename,'C',cii_unit(pin560[g].geom[ii].x2*2),0);
               Graph_Block_Reference(pshapename, NULL, 0, 
                  cii_unit(pin560[g].geom[ii].x1 + offx), cii_unit(pin560[g].geom[ii].y1 + offy), 0.0, 0 , 1.0, lay, TRUE);
            }

            flg = 0;
            res = 1;
         }
         break;
         case  4: /* start point */
            startx = pin560[g].geom[ii].x1 + offx;
            starty = pin560[g].geom[ii].y1 + offy;
            flg = 0;
            res = 1;
         break;
         case  5: /* box is lower left, upper right*/
         {
            char  pshapename[80];
            long  x,y;

            if (abs(pin560[g].geom[ii].x2-pin560[g].geom[ii].x1) &&
                abs(pin560[g].geom[ii].y2-pin560[g].geom[ii].y1))
            {
               make_pshapename(pshapename,'R',cii_unit(pin560[g].geom[ii].x2-pin560[g].geom[ii].x1),
                  cii_unit(pin560[g].geom[ii].y2-pin560[g].geom[ii].y1));

               x = pin560[g].geom[ii].x1 + (pin560[g].geom[ii].x2-pin560[g].geom[ii].x1) /2;
               y = pin560[g].geom[ii].y1 + (pin560[g].geom[ii].y2-pin560[g].geom[ii].y1) /2;
               Graph_Block_Reference(pshapename, NULL, 0, 
                  cii_unit(x + offx), cii_unit(y + offy), 0.0, 0 , 1.0, lay, TRUE);
            }
            flg = 0;
            res = 1;
         }
         break;
         case  6: /* rectangle */
         {
            char  pshapename[80];
            if (pin560[g].geom[ii].x2 && pin560[g].geom[ii].y2)
            {
               make_pshapename(pshapename,'R',cii_unit(pin560[g].geom[ii].x2), cii_unit(pin560[g].geom[ii].y2));
   
               Graph_Block_Reference(pshapename, NULL, 0, 
                  cii_unit(pin560[g].geom[ii].x1 + offx), cii_unit(pin560[g].geom[ii].y1 + offy), 0.0, 0 , 1.0, lay, TRUE);
            }

            flg = 0;
            res = 1;
         }
         break;
         case  7: /* circle with diameter */
         {
            char  pshapename[80];

            if (pin560[g].geom[ii].x2)
            {
               make_pshapename(pshapename,'C',cii_unit(pin560[g].geom[ii].x2),0);
               Graph_Block_Reference(pshapename, NULL, 0, 
                  cii_unit(pin560[g].geom[ii].x1 + offx), cii_unit(pin560[g].geom[ii].y1 + offy), 0.0, 0 , 1.0, lay, TRUE);
            }
            flg = 0;
            res = 1;
         }
         break;
         default:
            fprintf(ferr, "Unknown geometry in pin560 %d\n", pin560[g].geom[ii].typ);
            display_error++;
         break;
      } /* switch */
   } /* geom loop */

   return res;
}

/****************************************************************************/
/*
*/
static int write_padstack400()
{
   int   i,l;
   char  padname[30];
   char  layername[30];
   DbFlag   flg;

   int drilllayernum = Graph_Level("DRILLHOLE","",0);

   for (i=0;i<pin400cnt;i++)
   {
      if (pin400[i].used)
      {
         flg = 0;

         sprintf(padname,"%s%d",PADNAME_400,i+1);
      
         BlockStruct *b = Graph_Block_On(GBO_APPEND, padname,-1,0);
         b->setBlockType(BLOCKTYPE_PADSTACK);

         int drillindex = get_drillindex(cii_unit(pin400[i].pindef[0].hole), drilllayernum); 
         if (drillindex > -1)
         {
            CString  drillname;
            drillname.Format("DRILL_%d",drillindex);
            // must be filenum 0, because apertures are global.
            Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, drilllayernum, TRUE);
         }

         for (l=0;l<num_of_layers;l++)
         {
            if (l == 0)
               sprintf(layername,PADTOP);
            else
            if (l == (num_of_layers-1))
               sprintf(layername,PADBOT);
            else
               sprintf(layername,"PAD%d",l+1);

            if (pin400[i].pindef[l].geomptr)
            {
               do_pad560geom(layername,pin400[i].pindef[l].geomptr-1, 0L, 0L);
            }
            else
            {
               if (pin400[i].pindef[l].pad1 )
               {
                  if (!pin400[i].pindef[l].pad2)
                  {
                     if (pin400[i].pindef[l].pad1) // must have a size
                     {
                        char  pshapename[80];
                        make_pshapename(pshapename,'R',cii_unit(pin400[i].pindef[l].pad1),0);
                        Graph_Block_Reference(pshapename, NULL, 0, 
                           0.0, 0.0, 0.0, 0 , 1.0, Graph_Level(layername,"",0), TRUE);
                     }
                  }
                  else
                  if (pin400[i].pindef[l].pad1 == pin400[i].pindef[l].pad2)
                  {
                     if (pin400[i].pindef[l].pad1) // must have a size
                     {
                        char  pshapename[80];
                        make_pshapename(pshapename,'S',cii_unit(pin400[i].pindef[l].pad1),0);
                        Graph_Block_Reference(pshapename, NULL, 0, 
                           0.0, 0.0, 0.0, 0 , 1.0, Graph_Level(layername,"",0), TRUE);
                     }
                  }
                  else
                  {  
                     char  pshapename[80];
                     make_pshapename(pshapename,'R',cii_unit(pin400[i].pindef[l].pad1),cii_unit(pin400[i].pindef[l].pad2));
                     Graph_Block_Reference(pshapename, NULL, 0, 
                        0.0, 0.0, 0.0, 0 , 1.0, Graph_Level(layername,"",0), TRUE);
                  }
               }
            }
         }  // if used
         Graph_Block_Off();
      }
   }

   return(0);
}

/****************************************************************************/
/*
   This is used for bottom placed test points (vias)
*/
static int write_oppositepadstack400()
{
   int   i,l;
   char  padname[30];
   char  layername[30];
   DbFlag   flg;

   int drilllayernum = Graph_Level("DRILLHOLE","",0);

   for (i=0;i<opposite400cnt;i++)
   {
      int p = opposite400array[i]->padstack;
      flg = 0;

      sprintf(padname,"%s%d",PADNAME_400,-p);
      
      BlockStruct *b = Graph_Block_On(GBO_APPEND, padname,-1,0);
      b->setBlockType(BLOCKTYPE_PADSTACK);

      int drillindex = get_drillindex(cii_unit(pin400[p-1].pindef[0].hole), drilllayernum);  
      if (drillindex > -1)
      {
         CString  drillname;
         drillname.Format("DRILL_%d",drillindex);
         // must be filenum 0, because apertures are global.
         Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, drilllayernum, TRUE);
      }

      for (l=0;l<num_of_layers;l++)
      {
         // only the layername is mirrored
         int   lay = num_of_layers - (l + 1); // lay is layer starting at 0
         if (lay == 0)
            sprintf(layername,PADTOP);
         else
         if (lay == (num_of_layers-1))
            sprintf(layername,PADBOT);
         else
            sprintf(layername,"PAD%d",lay);

         if (pin400[p-1].pindef[l].geomptr)
         {
            do_pad560geom(layername,pin400[p-1].pindef[l].geomptr-1, 0L, 0L);
         }
         else
         {
            if (pin400[p-1].pindef[l].pad1 )
            {
               if (!pin400[p-1].pindef[l].pad2)
               {
                  if (pin400[p-1].pindef[l].pad1)  // must have a size
                  {
                     char  pshapename[80];
                     make_pshapename(pshapename,'R',cii_unit(pin400[p-1].pindef[l].pad1),0);
                     Graph_Block_Reference(pshapename, NULL, 0, 
                        0.0, 0.0, 0.0, 0 , 1.0, Graph_Level(layername,"",0), TRUE);
                  }
               }
               else
               if (pin400[p-1].pindef[l].pad1 == pin400[p-1].pindef[l].pad2)
               {
                  if (pin400[p-1].pindef[l].pad1)  // must have a size
                  {
                     char  pshapename[80];
                     make_pshapename(pshapename,'S',cii_unit(pin400[p-1].pindef[l].pad1),0);
                     Graph_Block_Reference(pshapename, NULL, 0, 
                        0.0, 0.0, 0.0, 0 , 1.0, Graph_Level(layername,"",0), TRUE);
                  }
               }
               else
               {  
                  char  pshapename[80];
                  make_pshapename(pshapename,'R',cii_unit(pin400[p-1].pindef[l].pad1),cii_unit(pin400[p-1].pindef[l].pad2));
                  Graph_Block_Reference(pshapename, NULL, 0, 
                        0.0, 0.0, 0.0, 0 , 1.0, Graph_Level(layername,"",0), TRUE);
               }
            }
         }
      }  // if used
      Graph_Block_Off();
   
   }

   return(0);
}

/****************************************************************************/
/*
*/
static int write_padstack514()
{
   int   i;
   char  padname[20];
   double   drill;


   for (i=0;i<pin514cnt;i++)
   {
      drill = cii_unit(pin514[i].d);
      sprintf(padname,"%s%d",PADNAME_514,i);

      fprintf(ferr,"Padstack 514 [%s]\n", padname);
      display_error++;

#ifdef WOLF
      L_PadStack(padname);
      L_PDrill(padname,inch_2_dbunit(drill));
      switch (pin514[i].shape_type)
      {
         case  1:    /* Comp */
            /* create circle */
            SCLG_PadStack(padname,PADALL);
            create_pin514(PADALL,pin514[i].w,pin514[i].h);
         break;
         case 2:  /* Edge */
            /* create circle */
            SCLG_PadStack(padname,PADTOP);
            create_pin514(PADTOP,pin514[i].w,pin514[i].h);
         break;
         case 3: /* on board connector */
            /* create circle */
            if (drill >= 0.001)
            {
               SCLG_PadStack(padname,PADALL);
               create_pin514(PADALL,pin514[i].w,pin514[i].h);
            }
            else
            {
               SCLG_PadStack(padname,PADTOP);
               create_pin514(PADTOP,pin514[i].w,pin514[i].h);
               SCLG_PadStack(padname,PADBOT);
               create_pin514(PADBOT,pin514[i].w,pin514[i].h);
            }
         break;
      } /* switch */
#endif
   }
   return(1);
}

/****************************************************************************/
/*
*/
static int create_pin514(char *layer,long w,long h)
{
#ifdef WOLF
   if (h)
   {
      if (h == w)
      {
         LG_Circle(0,inch_2_dbunitpoint2(0.0,0.0),
                  0L,layer,0,inch_2_dbunit(cii_unit(w)/2));
      }
      else
      if (h < w)
      {
         /* horiz */
         LG_Segm(0,inch_2_dbunitpoint2(cii_unit(-(w-h)/2),cii_unit(0L)),
              0L,layer,get_widthlong(h), /* widthcode */
              inch_2_dbunit(cii_unit(w-h)),
              inch_2_dbunit(cii_unit(0L)));
      }
      else
      {
         /* vert */
         LG_Segm(0,inch_2_dbunitpoint2(cii_unit(0L),cii_unit(-(h-w)/2)),
              0L,layer,get_widthlong(w), /* widthcode */
              inch_2_dbunit(cii_unit(0L)),
              inch_2_dbunit(cii_unit(h-w)));
      }
   }
   else
   {
      LG_Circle(0,inch_2_dbunitpoint2(0.0,0.0),
                  0L,layer,0,inch_2_dbunit(cii_unit(w)/2));
   }
#endif
   return  1;
}

/****************************************************************************/
/*
*/
static int write_complexpin_graphic()
{
   int   i, ii, li;
   char  l[80];
   Lm    alllayers = 0;
   Lm    allinner  = 0;
   int   larray[32];
   int   elec_layercnt;    // 0x1 top 0x2 bottom 0x3 more

   int drilllayernum = Graph_Level("DRILLHOLE","",0);
   LayerStruct *ll;
   if (ll = doc->FindLayer(drilllayernum))
   {
      ll->setComment("Drill");
      ll->setLayerType(LAYTYPE_DRILL);
   }

   for (i=0;i<32;i++)
      larray[i] = 0;   

   for (ii=0;ii<num_of_layers;ii++)
   {
      alllayers |= (1L << (32 - (ii+1)));
   }

   for (ii=1;ii<num_of_layers-1;ii++)
   {
      allinner |= (1L << (32 - (ii+1)));
   }

   for (i=0;i<complexpin_cnt;i++)
   {
      //fprintf(ferr,"Write complex pin graphic for [%s]\n",library[complexpin[i].libnameptr].name);
      //Graph_Block_On(GBO_APPEND,library[complexpin[i].libnameptr].name,-1,0);
      CString  stackname;
      stackname.Format("%s%d",PADNAME_560, i);
      BlockStruct *b = Graph_Block_On(GBO_APPEND, stackname, -1, 0);
      b->setBlockType(BLOCKTYPE_PADSTACK);
      elec_layercnt = 0;

      // here drill
      for (ii=0;ii<complexpin[i].complexdrillcnt;ii++)
      {
         int drillindex = get_drillindex(cii_unit(complexpin[i].complexdrill[ii].drill), drilllayernum); 
         if (drillindex > -1)
         {
            CString  drillname;
            drillname.Format("DRILL_%d",drillindex);
            // must be filenum 0, because apertures are global.
            Graph_Block_Reference(drillname, NULL, 0, 
                  cii_unit(complexpin[i].complexdrill[ii].x), cii_unit(complexpin[i].complexdrill[ii].y), 0.0,
                   0, 1.0, drilllayernum, TRUE);
         }
      }

      for (ii=0;ii<complexpin[i].complexpadcnt;ii++)
      {
         if (complexpin[i].complexpad[ii].layer == alllayers)
         {
            sprintf(l,PADALL);
            if (complexpin[i].complexpad[ii].typ ==TYP_FINGER)
            {
               char  pshapename[80];
               sprintf(pshapename,"PS_RECT_%d",complexpin[i].complexpad[ii].geomptr);
               int layernum = Graph_Level(l,"",0);
               Graph_Block_Reference(pshapename, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
            }
            else
            {
               do_pad560geom(l,complexpin[i].complexpad[ii].geomptr-1,
                    complexpin[i].complexpad[ii].x, complexpin[i].complexpad[ii].y);
            }
            elec_layercnt |= 0x3;
         }
         else
         if (complexpin[i].complexpad[ii].layer == allinner)
         {
            sprintf(l,PADINNER);
            if (complexpin[i].complexpad[ii].typ ==TYP_FINGER)
            {
               char  pshapename[80];
               sprintf(pshapename,"PS_RECT_%d",complexpin[i].complexpad[ii].geomptr);
               int layernum = Graph_Level(l,"",0);
               Graph_Block_Reference(pshapename, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
            }
            else
            {
               do_pad560geom(l,complexpin[i].complexpad[ii].geomptr-1, 
                  complexpin[i].complexpad[ii].x, complexpin[i].complexpad[ii].y);
            }
            elec_layercnt |= 0x3;
         }
         else
         for (li=0;li<32;li++)
         {
            CString  padlayer;
            if (complexpin[i].complexpad[ii].typ == 7)
               padlayer = "SOLDERRESIST_";
            else
               padlayer = "PAD";

            if (complexpin[i].complexpad[ii].layer & (1L << (32 - (li+1))))
            {
               larray[li] = 1;
               if (li == 0)
               {
                  sprintf(l,"%sTOP", padlayer);
                  if (complexpin[i].complexpad[ii].typ != 7)
                     elec_layercnt |= 0x1;
               }
               else
               if (li == (num_of_layers-1))
               {
                  sprintf(l,"%sBOT", padlayer);
                  if (complexpin[i].complexpad[ii].typ != 7)
                     elec_layercnt |= 0x2;
               }
               else
               {
                  sprintf(l,"%s%d", padlayer, li+1);
                  if (complexpin[i].complexpad[ii].typ != 7)
                     elec_layercnt |= 0x3;
               }

               if (complexpin[i].complexpad[ii].typ == TYP_FINGER)
               {
                  char  pshapename[80];
                  sprintf(pshapename,"PS_RECT_%d",complexpin[i].complexpad[ii].geomptr);
                  int layernum = Graph_Level(l,"",0);
                  Graph_Block_Reference(pshapename, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
               }
               else
               {
                  do_pad560geom(l,complexpin[i].complexpad[ii].geomptr-1, 
                     complexpin[i].complexpad[ii].x, complexpin[i].complexpad[ii].y);
               }
            }
         }
      } // complexpin[i]->complexpadcnt
      Graph_Block_Off();

      if (elec_layercnt == 1)
      {
      // here set SMD flag if no drill and 
         doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE, NULL, SA_OVERWRITE, NULL); //  
      }
      else
      if (elec_layercnt == 2)
      {
         // no smd
      }
      else
      {
         // all layers
      }
   }                             

   return 1;
}

/****************************************************************************/
/*
*/
static int load_ciisettings(const CString fname)
{
   FILE  *ciitec;
   char  tmp[255],*lp;

   area_fill = 'Y';  // init
   box_fill = FALSE;
   pin1placement = FALSE;
   max560rec = MAXINT;
   ignore_oldshape = FALSE;
   testpoint_convert = FALSE;
   ComponentSMDrule = 0;
   TESTPOINT_382 = FALSE;

   if ((ciitec = fopen(fname,"rt")) != NULL)
   {
      while (fgets(tmp,255,ciitec))
      {
         if (tmp[0] == '.')
         {
            lp = strtok(tmp," \t\n");
            if (!STRCMPI(lp,".ComponentSMDrule"))
            {
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               ComponentSMDrule = atoi(lp);
            }
            else
            if (!STRCMPI(lp,".MAX560REC"))
            {
               if ((lp = strtok(NULL," \t\n")) == NULL)
               {
                  fprintf(ferr,"Error in .MAX560REC\n");
                  exit(1);
               }
               max560rec = atoi(lp);
            }
            else
            if (!STRCMPI(lp,".TESTPOINTCONVERT"))
            {
               lp = strtok(NULL," \t\n");
               if (strlen(lp) > 0)
               {
                  if (lp[0] == 'Y' || lp[0] == 'y')
                     testpoint_convert = TRUE;
               }
            }
            else
            if (!STRCMPI(lp,".382_TESTPOINT"))
            {
               lp = strtok(NULL," \t\n");
               if (strlen(lp) > 0)
               {
                  if (lp[0] == 'Y' || lp[0] == 'y')
                     TESTPOINT_382 = TRUE;
               }
            }
            else
            if (!STRCMPI(lp,".GRIDSHIFT"))
            {
               lp = strtok(NULL," \t\n");
               if (strlen(lp) > 0)
               {
                  if (lp[0] == 'Y' || lp[0] == 'y')
                     inverse_grid = TRUE;
               }
            }
            else
            if (!STRCMPI(lp,".IGNOREOLDSHAPE"))
            {
               lp = strtok(NULL," \t\n");
               if (strlen(lp) > 0)
               {
                  if (lp[0] == 'Y' || lp[0] == 'y')
                     ignore_oldshape = TRUE;
               }
            }
            else
            if (!STRCMPI(lp,".BOXFILL"))
            {
               lp = strtok(NULL," \t\n");
               if (strlen(lp) > 0)
               {
                  if (lp[0] == 'Y' || lp[0] == 'y')
                     box_fill = TRUE;
               }
            }
            else
            if (!STRCMPI(lp,".PIN1PLACEMENT"))
            {
               lp = strtok(NULL," \t\n");
               if (strlen(lp) > 0)
               {
                  if (lp[0] == 'Y' || lp[0] == 'y')
                     pin1placement = TRUE;
               }
            }
            else
            if (!STRCMPI(lp,".PLANENET"))
            {
               lp = strtok(NULL," \t\n");
               if (planenetcnt < MAX_PLANENET)
               {
                  if ((planenet[planenetcnt].net = STRDUP(lp)) == NULL)
                  {
                     MemErrorMessage(__FILE__, __LINE__);
                     return -1;
                  }
                  planenet[planenetcnt].shown = FALSE;
                  planenetcnt++;
               }
               else
               {
                  fprintf(ferr,"Too many planenents\n");
                  return -1;
               }
            }
            else
            if (!STRCMPI(lp,".AREAFILL"))
            {
               lp = strtok(NULL," \t\n");
               if (strlen(lp) > 0)
               {
                  area_fill = toupper(lp[0]);
               }
            }
         }
      }
      fclose(ciitec);
   }

   if (testpoint_convert)
      log_information_none(ferr,"Test Point Vias converted to Components.", -1);

   return 1;
}


/****************************************************************************/
/*
  unit CND
*/
static int unit_CND(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_262(ifp,i)) != 0)
      {
         fprintf(ferr,"Error in do_262\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 262
*/
static int do_262(FILE *ifp,int copcnt)
{
   int   len,i;
   int   err  = 0;
   int   num_rec;
   Lm    layer;
   int   nodenr,planeflg;
   char  netname[33];
   char  ln[10];
   long  width;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 262) r_error(atoi(token),262);
   if (!nextword())  p_error();
   len = atoi(token);

   switch (act_ciirev)
   {
      case  2:
         if (len != 8) t_error(len,8);
      break;
      case  7:
      case  8:
      case  9:
      case 10:
         if (len != 11) t_error(len,11);
      break;
      default:
         if (len != 9) t_error(len,9);
      break;        
   }

   if (!nextword())  p_error();  /* layer mask */
   layermask(&layer,token); 
   if (!nextword())  p_error();  /* node # */
   nodenr = atoi(token);
   if (!nextword())  p_error();  /* node plane flag */
   planeflg = atoi(token);
   if (!nextword())  p_error();  /* comp pointer */
   if (!nextword())  p_error();  /* diameter */
   
   if (!nextword())  p_error();
   num_rec = atoi(token);

   width = 0;
   if (act_ciirev > 6)
   {
      if (!nextword())  p_error();  /* need cii 7 doc. */
      width = atol(token);
   }

   vectorcnt = num_rec;
   for (i=0;i<num_rec;i++)
   {
      if ((err = do_264(ifp,i))!=0)
      {
         fprintf(ferr,"Error in do_264\n");
         return -1;
      }
   }
   /* duplicate last */
   vector[vectorcnt].x = vector[0].x;
   vector[vectorcnt].y = vector[0].y;
   vector[vectorcnt].arc = 0;
   vectorcnt++;

   /* here write plane */
   get_netname(netname,nodenr);

   //sprintf(netname,"COP_%d",copcnt);
   switch (planeflg)
   {
      case 3: // split multinode
      case 2: // multinode layer
      case 1: // uninode
      {
         // also make layer number a plane
          for (i=0;i<32;i++)
          {
            if (layer & (1L << (32-(i+1))))
            {
               sprintf(ln,"%d",i+1);  
               int l = Graph_Level(ln,"",0);
               LayerStruct *ll = doc->FindLayer(l);
               if (planeflg == 1)
                  ll->setLayerType(LAYTYPE_POWERNEG);
               else
                  ll->setLayerType(LAYTYPE_SPLITPLANE);

               NetStruct *n = add_net(file,netname);
               doc->SetUnknownAttrib(&n->getAttributesRef(),ATT_POWERNET, "",
                   SA_OVERWRITE, NULL); //
               doc->SetUnknownAttrib(&ll->getAttributesRef(),LAYATT_NETNAME, netname,
                   SA_APPEND, NULL); // x, y, rot, height
            }
         }
      }
      break;
      default:
            // no plane
      break;
   }

   for (i=0;i<32;i++)
   {
      if (layer & (1L << (32-(i+1))))
      {
         sprintf(ln,"%d",i+1);  
         int level = Graph_Level(ln,"",0);
         int widthindex = 0;  

         int err;
         if (width == 0)
            widthindex = 0;
         else
            widthindex = Graph_Aperture("", T_ROUND, cii_unit(width), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

         DataStruct *d = Graph_PolyStruct(level,0,0);
         if (strlen(netname))
         {
            d->setGraphicClass(GR_CLASS_ETCH);
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
                  VT_STRING, (void *)netname, SA_APPEND, NULL); // x, y, rot, height
         }

         int filled = 0;
         int closed = 1;
         Graph_Poly(NULL,widthindex, filled , 0, closed);   // fillflag , negative, closed
         write_vector();
      }
   }
   
   return(err);
}

/****************************************************************************/
/*
  do 264
*/
static int do_264(FILE *ifp,int cnt)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 264) r_error(atoi(token),264);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   vector[cnt].x = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[cnt].y = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[cnt].arc = atoi(token); 

   return(err);
}

/****************************************************************************/
/*
  unit VEC
*/
static int unit_VEC(FILE *ifp)
{
   int  len,i;
   int  err  = 0;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
  /* rec.u.rec280.inum_vec = atol(token); */

   for (i=0;i<num_of_layers;i++)
   {
      if ((err = do_282(ifp,i+1)) != 0)
      {
         fprintf(ferr,"Error in do_282\n");
         return -1;
      }
  }
  return(err);
}

/****************************************************************************/
/*
  do 282
*/
static int do_282(FILE *ifp,int lay)
{
   int  len;
   int  err  = 0;
   long  i,num_vec;
      
   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 282) r_error(atoi(token),282);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
   num_vec = atol(token);

   for (i=0;i<num_vec;i++)
   {
      if ((err = do_284(ifp,lay))!=0)
      {
        fprintf(ferr,"Error in do_284 %d\n",err);
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 284
*/
static int do_284(FILE *ifp,int lay)
{
   int  len;
   int  err  = 0;
   int  i,num_vec = 0;
   int    nodenr;
   long width;
   char  netname[33];
   char  layer[10];
   DbFlag   flg = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 284) r_error(atoi(token),284);
   if (!nextword())  p_error();
   len = atoi(token);

   switch (act_ciirev)
   {
      case  2:
         if (len != 11) t_error(len,11);
      break;
      default:
         if (len != 15) t_error(len,15);
      break;
   }
   vectorcnt = 0;

   if (!nextword())  p_error();
   nodenr = atoi(token);
   if (!nextword())  p_error();
   width = atol(token);
   if (!nextword())  p_error();
   vector[0].x = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[0].y= on_grid(atol(token));
   vector[0].arc= 0;
   if (!nextword())  p_error();
   vector[1].x = on_grid(atol(token));
   if (!nextword())  p_error();
   vector[1].y = on_grid(atol(token));
   vector[1].arc= 0;
   vectorcnt = 2;

   switch (act_ciirev)
   {
      case  2:
      break;
      default:
        if (!nextword())  p_error();
        /* rec.u.rec284.i2vec_flg = atoi(token); */
        if (!nextword())  p_error();
        /* rec.u.rec284.iptr_vec2compvgrp = atol(token); */
        if (!nextword())  p_error();
        num_vec = atoi(token);
      break;
   }
   for (i=0;i<num_vec && !err;i++)
   {
      if ((err = do_286(ifp))!=0)
      {
         fprintf(ferr,"Error in do_286\n");
         return -1;
      }
   }

   sprintf(layer,"%d",lay);

   get_netname(netname,nodenr);

   int level = Graph_Level(layer,"",0);
   int widthindex;   
   if (width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, cii_unit(width), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   DataStruct *d = Graph_PolyStruct(level,0,0);
   if (strlen(netname))
   {
      d->setGraphicClass(GR_CLASS_ETCH);
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING,
            (void *)netname, SA_APPEND, NULL); // x, y, rot, height
   }

#ifdef _DEBUG
   CString val;
   val.Format("%ld", linecnt);
   doc->SetUnknownAttrib(&d->getAttributesRef(), "LINENR", val,
            SA_OVERWRITE, NULL); // x, y, rot, height
#endif

   int filled = 0;
   int closed = 0;
   Graph_Poly(NULL, widthindex, filled , 0, closed);  // fillflag , negative, closed
   write_vector284();
   
   vectorcnt = 0;
   
   return(err);
}

/****************************************************************************/
/*
  do 286
*/
static int do_286(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 286) r_error(atoi(token),286);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (vectorcnt < MAX_VECTOR)
   {   
      if (!nextword())  p_error();
      vector[vectorcnt].arc = atoi(token); 
      if (!nextword())  p_error();
      vector[vectorcnt].x = on_grid(atol(token));
      if (!nextword())  p_error();
      vector[vectorcnt].y = on_grid(atol(token)); 
      vectorcnt++; 
   }
   else
   {
      fprintf(ferr,"Too many vectors\n");
      display_error++;
      return -1;
   }
         
   return(err);
}

/****************************************************************************/
/*
  unit DFL
*/
static int unit_DFL(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_302(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_302\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 302
*/
static int do_302(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 302) r_error(atoi(token),302);
   if (!nextword())  p_error();
   len = atoi(token);

   switch (act_ciirev)
   {
      case  2:
        if (len != 34) t_error(len,34);
        if (!numword(32,0))  p_error();
        if (!nextword())  p_error();
        if (!numword(32,0))  p_error();
        if (!nextword())  p_error();
      break;
      default:
        if (len != 4) t_error(len,4);
        if (!nextword())  p_error();
        /* rec.u.rec302.i2ptr_dfl2cmp1 = atoi(token); */
        if (!nextword())  p_error();
        /* rec.u.rec302.i2dfl_pin1 = atoi(token); */
        if (!nextword())  p_error();
        /* rec.u.rec302.i2ptr_dfl2cmp2 = atoi(token); */
        if (!nextword())  p_error();
        /* rec.u.rec302.i2dfl_pin2 = atoi(token); */
      break;
   }

   return(err);
}

/****************************************************************************/
/*
  unit CMP
*/
static int unit_CMP(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
   /* rec.u.rec320.i2pads_flg = atoi(token); */
   if (!nextword())  p_error();
   number_of_components = num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_322(ifp, i+1)) != 0)
      {
         fprintf(ferr,"Error in do_322\n");
         return -1;
      }
      if ((err = do_324(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_324\n");
         return -1;
      }
      if ((err = do_326(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_326\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 322
*/
static int do_322(FILE *ifp, int index)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 322) r_error(atoi(token),322);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 33) t_error(len,33);
   if (!nextword())  p_error();
   /* rec.u.rec322.i2cmp_typeflg = atoi(token); */

   // it is possible for type 21 (ribbus) notnot have a compname and libame
   // I dont know why, but I will skip that

   if (!numword(31,0))  p_error(); /* this is on purpose */
   strcpy(compname,token);
   if (!numword(32,0))  p_error();
   strcpy(libname,token);
   strcpy(type_name,"");  // null out
   // Test only once.
   if (strlen(compname) == 0)
   {
      fprintf(ferr,"Error : Component has no Name at %ld -> %s\n",
                    linecnt,compname);
      fprintf(ferr,"Error : Component has no Name at %ld -> %s\n",
                    linecnt,compname);
   }

   if (strlen(compname))
   {
      CIICompname *p = new CIICompname; 
      comparray.SetAtGrow(compcnt,p);
      compcnt++;
      p->name = compname;
      p->libname = libname;
      p->index = index;
   }
   return(err);
}

/****************************************************************************/
/*
  do 324
*/
static int do_324(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 324) r_error(atoi(token),324);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 32) t_error(len,32);

   if (!numword(32,0))  p_error();
   strcpy(type_name,token);

   if (!numword(32,0))  p_error(); // this is assembly name -> ignore

   return(err);
}

/****************************************************************************/
/*
  do 326
*/
static int do_326(FILE *ifp)
{
   int   len;
   int   err  = 0;
   int   i,num_pstack,num_pin,num_group, rec_336 = 0, rec_334 = 0;
   Lm    layer;
   int   rot,desflg;
   long  x,y,offx,offy;
   unsigned char  mir;
   double   nmx = 0, nmy = 0; // place of compname

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 326) r_error(atoi(token),326);
   if (!nextword())  p_error();
   len = atoi(token);

   switch (act_ciirev)
   {
      case 10:
        if (len != 28) t_error(len,28);
      break;
      case 9:
        if (len != 26) t_error(len,26);
      break;
      case  2:
        if (len != 22) t_error(len,22);
      break;
      default:
        if (len != 24) t_error(len,24);
      break;
   }
   if (!nextword())  p_error();
   desflg = atoi(token);
   if (!nextword())  p_error();
   /* rec.u.rec326.i2cmp_posflg = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec326.i2cmp_fvoflg = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec326.i2cmp_tvoflg = atoi(token); */
   if (!nextword())  p_error();
   layermask(&layer,token); 
   if (!nextword())  p_error();
   rot = atoi(token);
   if (!nextword())  p_error();
   x = on_grid(atol(token));
   if (!nextword())  p_error();
   y = on_grid(atol(token));
   if (!nextword())  p_error();
   compplcflg = atoi(token);
   if (!nextword())  p_error();
   /* rec.u.rec326.ifld1 = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec326.ifld2 = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec326.i2fld3 = atoi(token); */
   if (!nextword())  p_error();
   offx = atol(token); 
   if (!nextword())  p_error();
   offy = atol(token);
   if (!nextword())  p_error();
   num_pstack = atoi(token);

   switch (act_ciirev)
   {
      case  2:
        num_pin = 0;
        num_group = 0;
      break;
      case 10:
        if (!nextword())  p_error();
        num_pin = atoi(token);
        if (!nextword())  p_error();
        num_group = atoi(token);
        if (!nextword())  p_error();   // number of component properties
        rec_334 = atoi(token);
        if (!nextword())  p_error();   // number of function instances
        rec_336 = atoi(token);   
        if (!nextword())  p_error();   // hybrid component flag
        if (!nextword())  p_error();   // fixed/free
      break;
      case 9:
        if (!nextword())  p_error();
        num_pin = atoi(token);
        if (!nextword())  p_error();
        num_group = atoi(token);
        if (!nextword())  p_error();   // ???
        rec_334 = atoi(token);
        if (!nextword())  p_error();
        rec_336 = atoi(token);   
      break;
      default:
        if (!nextword())  p_error();
        num_pin = atoi(token);
        if (!nextword())  p_error();
        num_group = atoi(token);
      break;
   }

   if (strlen(compname) == 0)
   {
      sprintf(compname,"$GEN$_%d",compcnt);
   }

   mir = 0;
   if (layer == (1L << (32-num_of_layers))) 
      mir = 1;

   BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
   Graph_Block_Off();

   rotate(&offx,&offy,rot);

   if (mir) rot = 360 - rot;
   while (rot < 0)      rot += 360;
   while (rot >= 360)   rot -= 360;

   DataStruct *d = Graph_Block_Reference(libname, compname , 0, cii_unit(x+offx),cii_unit(y+offy),
         DegToRad(rot), mir , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypePcbComponent);

/* this is done in 422 record */
   int laynr = Graph_Level("LIB_1", "" , 0);
   if (desflg)
   {
	   doc->SetVisAttrib(&d->getAttributesRef(),doc->RegisterKeyWord(ATT_REFNAME, TRUE, VT_STRING),
          VT_STRING,
          compname,    // this makes a "real" char *
          0.0, 0.0, 0.0, 700 * faktor, 400 * faktor, 1, 0, TRUE, SA_OVERWRITE, 0L, laynr, 0, 0, 0);
   }
   else
   {
	   doc->SetVisAttrib(&d->getAttributesRef(),doc->RegisterKeyWord(ATT_REFNAME, TRUE, VT_STRING),
          VT_STRING,
          compname,    // this makes a "real" char *
          0.0, 0.0, 0.0, 700 * faktor, 400 * faktor, 1, 0, FALSE, SA_OVERWRITE, 0L, laynr, 0, 0, 0);
   }

   if (strlen(type_name))
   {
      TypeStruct *t = AddType(file, type_name);
      if (t->getBlockNumber() < 0)
      {
         t->setBlockNumber( d->getInsert()->getBlockNumber());

         if (d)
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TYPELISTLINK, TRUE),
               VT_STRING, type_name, SA_OVERWRITE, NULL); //
         }
      }
      else
      {
         if (t->getBlockNumber() != d->getInsert()->getBlockNumber())
         {
            fprintf(ferr, "Component [%s] Part Name [%s] has already a Shape Name [%s] assigned!\n",
               compname, type_name, libname);
            display_error++;
         }
      }
   }

   compx = cii_unit(x+offx);
   compy = cii_unit(y+offy);

   comparray[compcnt-1]->x = x+offx;
   comparray[compcnt-1]->y = y+offy;
   comparray[compcnt-1]->rot = rot;
   comparray[compcnt-1]->mir = mir;
   comparray[compcnt-1]->pin1placement = compplcflg;

   for (i=0;i<num_pstack;i++)
   {
      if ((err = do_328(ifp)) != 0)
      {
        fprintf(ferr,"Error in do_328\n");
         return -1;
      }
   }

   pincnt = 0;  
   for (i=0;i<num_pin;i++)
   {
      if ((err = do_330(ifp)) != 0)
      {
        fprintf(ferr,"Error in do_330\n");
         return -1;
      }
   }

   for (i=0;i<num_group;i++)
   {
      if ((err = do_332(ifp)) != 0)
      {
        fprintf(ferr,"Error in do_332\n");
         return -1;
      }
   }  

   for (i=0;i<rec_334;i++)
   {
      if (!nextline(ifp))  l_error();
   }

   for (i=0;i<rec_336;i++)
   {
      if (!nextline(ifp))  l_error();
   }

   return(err);
}

/****************************************************************************/
/*
  do 328
*/
static int do_328(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 328) r_error(atoi(token),328);
   if (!nextword())  p_error();

   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
   /* rec.u.rec328.iptr_cmp2pso = atol(token); */

   return(err);
}

/****************************************************************************/
/*
  do 330
*/
static int do_330(FILE *ifp)
{
   int      len;
   int      err  = 0;
   int      nodenr;
   char     pinname[10];
   char     netname[33];
   Lm       lm;
   double   x,y;
   int      rec_331 = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 330) r_error(atoi(token),330);
   if (!nextword())  p_error();
   len = atoi(token);
   switch (act_ciirev)
   {
      case  10:
      case  9:
        if (len != 9) t_error(len,9);
      break;
      case  3:
        if (len != 6) t_error(len,6);
      break;
      default:
         if (len != 8) t_error(len,8);
      break;
   }

   pincnt++;   
   long  p1x, p1y;

   if (!nextword())  p_error();
   layermask(&lm,token); 
   if (!nextword())  p_error();
   p1x = on_grid(atol(token));
   x = cii_unit(p1x);
   if (!nextword())  p_error();
   p1y = on_grid(atol(token));
   y = cii_unit(p1y);

   if (pincnt == 1)
   {
      comparray[compcnt-1]->pin1x = p1x;
      comparray[compcnt-1]->pin1y = p1y;
   }

   /* Netlist for act_ciirev done in 382 */
   if (act_ciirev > 3)
   {
      if (!nextword())  p_error();
      nodenr = atoi(token); 
      if (!nextword())  p_error();     // pinflag 1 - electrical, 2 = mechanical
      /* rec.u.rec330.i2cmp_pinflg = atoi(token); */

      if (act_ciirev >= 9)
      {
         if (!nextword())  p_error();  // rec331
           rec_331 = atoi(token);   
      }
      
      if (STRLEN(compname) == 0)
         return 0;

      sprintf(pinname,"%d", pincnt);
      if (nodenr > 0)
      {
         get_netname(netname,nodenr);
         NetStruct *n = add_net(file, netname);
         add_comppin(file, n, compname, pinname);
      } 
   }

   for (int i=0;i<rec_331;i++)
   {
      if ((err = do_331(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_331\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 331
*/
static int do_331(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 331) r_error(atoi(token),331);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 4) t_error(len,4);

   if (!nextword())  p_error();  // prop name pointer
   if (!nextword())  p_error();  // prop value pointer

   return(err);
}

/****************************************************************************/
/*
  do 332
*/
static int do_332(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 332) r_error(atoi(token),332);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
   /* rec.u.rec332.iptr_cmp2grp = atol(token); */

   return(err);
}

/****************************************************************************/
/*
  unit STR
*/
static int unit_STR(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   oldcomp = -1;   

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_342(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_342\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 342
*/
static int do_342(FILE *ifp)
{
  int  len;
  int  err  = 0;
  int  i,num_rec;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 342) r_error(atoi(token),342);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 5) t_error(len,5);

  if (!nextword())  p_error();
  /* rec.u.rec342.i2ptr_str2cmp = atoi(token); */
  if (!nextword())  p_error();
  /* rec.u.rec342.i2base_pinnm = atoi(token); */
  if (!nextword())  p_error();
  /* rec.u.rec342.i2base_nodenm = atoi(token); */
  if (!nextword())  p_error();
  /* rec.u.rec342.i2str_nodenm = atoi(token); */
  if (!nextword())  p_error();
  num_rec = atoi(token);

  for (i=0;i<num_rec;i++)
    if ((err = do_344(ifp))!=0)
      {
        fprintf(ferr,"Error in do_344\n");
         return -1;
      }

  return(err);
}

/****************************************************************************/
/*
  do 344
*/
static int do_344(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 344) r_error(atoi(token),344);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 1) t_error(len,1);

  if (!nextword())  p_error();
  /* rec.u.rec344.i2str_pinnm = atoi(token); */

  return(err);
}

/****************************************************************************/
/*
  unit PLB
*/
static int unit_PLB(FILE *ifp)
{
  int  len,i;
  int  err  = 0;
  int  num_rec;

  if (!nextword())  p_error();
  if ((len = atoi(token)) != 1) t_error(len,1);

  switch (act_ciirev)
  {
    case  2:
      /* the record 362 is totally different from newer cii rev */
      /* therefore set it to 0 and skip 362 */
        if (!nextword())  p_error();
        num_rec = atoi(token);
      break;
    default:
        if (!nextword())  p_error();
        num_rec = atoi(token);
  }

  for (i=0;i<num_rec;i++)
    {
      switch (act_ciirev)
      {
        case  2:
          if ((err = do_362_rev2(ifp)) != 0)
            {
              fprintf(ferr,"Error in do_362_rev2\n");
              return -1;
            }
          break;
        default:
          if ((err = do_362(ifp)) != 0)
            {
              fprintf(ferr,"Error in do_362\n");
              return -1;
            }
      } /* switch */
    }
  return(err);
}

/****************************************************************************/
/*
  do 362
*/
static int do_362(FILE *ifp)
{
  int  len;
  int  err  = 0;
  int  i,num_rec;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 362) r_error(atoi(token),362);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 4) t_error(len,4);

  if (!nextword())  p_error();
  /* layermask(&rec.u.rec362.iplb_mask,token); */
  if (!nextword())  p_error();
  /* rec.u.rec362.i2ptr_plb2cmp = atoi(token); */
  if (!nextword())  p_error();
  num_rec = atoi(token);

  for (i=0;i<num_rec;i++)
    if ((err = do_364(ifp))!=0)
      {
        fprintf(ferr,"Error in do_364\n");
         return -1;
      }

  return(err);
}

/****************************************************************************/
/*
  do 362_rev2
  This record will not be converted !!!
  It is not compatible with newer revisiions
*/
static int do_362_rev2(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 362) r_error(atoi(token),362);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 23) t_error(len,23);

  /* name layer rot x y */
  if (!nextword())  p_error();
  if (!nextword())  p_error();
  if (!nextword())  p_error();
  if (!nextword())  p_error();

  return(err);
}

/****************************************************************************/
/*
  do 364
*/
static int do_364(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 364) r_error(atoi(token),364);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 10) t_error(len,10);

  if (!nextword())  p_error();
  /* rec.u.rec364.iplb_lnwdth = atol(token); */
  if (!nextword())  p_error();
  /* rec.u.rec364.iplb_begx = atol(token); */
  if (!nextword())  p_error();
  /* rec.u.rec364.iplb_begy = atol(token); */
  if (!nextword())  p_error();
  /* rec.u.rec364.iplb_endx = atol(token); */
  if (!nextword())  p_error();
  /* rec.u.rec364.iplb_endy = atol(token); */

  return(err);
}

/****************************************************************************/
/*
  unit PSO
*/
static int unit_PSO(FILE *ifp)
{
  int  len;
  int  err  = 0;
  long  i,num_rec;

  if (!nextword())  p_error();
  len = atoi(token);

  switch (act_ciirev)
  {
    case  2:
        if (len != 1) t_error(len,1);
      break;
    default:
        if (len != 2) t_error(len,2);
  }

  if (!nextword())  p_error();
  num_rec = atol(token);

  pincnt = 0;
  for (i=0;i<num_rec;i++)
    {
       if ((err = do_382(ifp)) != 0)
        {
          fprintf(ferr,"Error in do_382\n");
           return -1;
        }
    }
  return(err);
}

/****************************************************************************/
/*
*/
static int find_opposite400(int padst)
{
   int   i;

   for (i=0;i<opposite400cnt;i++)
   {
      if (opposite400array[i]->padstack == padst)
         return -padst;
   }

   CIIOpposite400 *p = new CIIOpposite400;
   opposite400array.SetAtGrow(opposite400cnt, p);
   opposite400cnt++;
   p->padstack = padst;

   return -padst;
}

/****************************************************************************/
/*
  do 382
*/
static int do_382(FILE *ifp)
{
   int      len;
   int      err  = 0;
   Lm       testpointlayer = 0;
   char     netname[33];
   CString  padname, tstname;    
   int      nodenr;
   long     x,y;
   int      padst;
   int      viaflg;
   int      compptr;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 382) r_error(atoi(token),382);
   if (!nextword())  p_error();
   
   switch (act_ciirev)
   {
      case  10:
      case  9:
         if ((len = atoi(token)) != 16) t_error(len,16);
      break;
      case  8:
         if ((len = atoi(token)) != 14) t_error(len,14);
      break;
      case  7:
      case  6:
         if ((len = atoi(token)) != 12) t_error(len,12);
      break;
      default:
         if ((len = atoi(token)) != 9) t_error(len,9);
      break;
   }

   if (!nextword())  p_error();
   x = on_grid(atol(token));
   if (!nextword())  p_error();
   y = on_grid(atol(token)); 
   if (!nextword())  p_error();
   padst = atoi(token);
   if (!nextword())  p_error();
   nodenr = atoi(token); 
   if (!nextword())  p_error();
   viaflg = atoi(token);
   if (!nextword())  p_error();
   compptr = atoi(token);

   if (compptr <= number_of_components)
   {
      if (oldcomp != compptr)
      {
         pincnt = 0;
         oldcomp = compptr;
      }  
   }

   switch (act_ciirev)
   {
      case 10:
      case  9:
         if (!nextword())  p_error();  /* component pinnr */
         if (!nextword())  p_error();  /* soldermask override */
         if (!nextword())  p_error();  /* Test point layer mask */
         layermask(&testpointlayer,token); 
         if (!nextword())  p_error();  // rotation ???
      break;
      case  8:
         if (!nextword())  p_error();  /* component pinnr */
         if (!nextword())  p_error();  /* soldermask override */
         if (!nextword())  p_error();  /* Test point layer mask */
         layermask(&testpointlayer,token); 
      break;
      case  7:
      case  6:
         if (!nextword())  p_error();  /* component pinnr */
         if (!nextword())  p_error();  /* soldermask override */
      break;
      default:
      break;
   }

   get_netname(netname,nodenr);

   if (act_ciirev <= 3 && compptr <= number_of_components)
   {
      if (viaflg == 1)
      {
         pincnt++;
         /* here do netlist */
#ifdef WOLF
         sprintf(pinname,"%d",pincnt);
         if ((err = L_NetPinComp(netname,complist[compptr-1].name,pinname)) != OK)
           fprintf(ferr,"Error %d [%s] [%s] [%s] at %ld\n",err,netname,compname,pinname,linecnt);
#endif
      }     
   }

   /* 2 = via 5 = shapevia */
   if (viaflg == 2 || viaflg == 5)  
   {
      DataStruct *d;

      // test point
      if (testpointlayer != 0)
      {
         int   place_bottom = FALSE;
         if (ONLAYER(testpointlayer,num_of_layers))
         {
            // if this is placed on the bottom, we should find or create a top padstack and define it
            // on the top and mirror the placement.
            padst = find_opposite400(padst);
            place_bottom = TRUE;
         }
         tstname.Format("%s%d",TSTNAME_400,padst);

         if (!Graph_Block_Exists(doc, tstname, -1))
         {
            padname.Format("%s%d",PADNAME_400,padst);
            // define the test point component with a pin for the netlist
            // define a geometry
            BlockStruct *b = Graph_Block_On(GBO_APPEND,tstname,-1,0);
            if (!TESTPOINT_382)
               b->setBlockType(BLOCKTYPE_PCBCOMPONENT);
            else
               b->setBlockType(BLOCKTYPE_TESTPOINT);

            // make a one pin component
            d = Graph_Block_Reference(padname, "1", 0, 0.0, 0.0, 0.0, 0 , 1.0,0, TRUE);
            d->getInsert()->setInsertType(insertTypePin);

            Graph_Block_Off();
         }
         CString  testrefname;
         testrefname.Format("$TP_%d$", ++testpointcnt);
         DataStruct *d = Graph_Block_Reference(tstname, testrefname, -1, cii_unit(x),cii_unit(y),
                           DegToRad(0), place_bottom , 1.0, -1, TRUE);
         if (!TESTPOINT_382)
            d->getInsert()->setInsertType(insertTypePcbComponent);
         else
            d->getInsert()->setInsertType(insertTypeTestPoint);
         if (ONLAYER(testpointlayer,1))
         {
            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TEST, testaccesslayers[ATT_TEST_ACCESS_TOP],
               SA_OVERWRITE, NULL); // x, y, rot, height
         }
         else
         if (ONLAYER(testpointlayer,num_of_layers))
         {
            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TEST, testaccesslayers[ATT_TEST_ACCESS_BOT],
               SA_OVERWRITE, NULL); // x, y, rot, height
         }
         else
         {
            // unknown test layer ???
            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TEST, "",
               SA_OVERWRITE, NULL); // x, y, rot, height
         }

         /* add to netlist */
         NetStruct *n = add_net(file, netname);
         CompPinStruct *compPin = add_comppin(file, n, testrefname, "1");

      }
      else
      {
         padname.Format("%s%d",PADNAME_400,padst);
   
         d = Graph_Block_Reference(padname, NULL, -1, cii_unit(x),cii_unit(y),
                           DegToRad(0), 0 , 1.0, -1, TRUE);

         d->getInsert()->setInsertType(insertTypeVia);

         if (strlen(netname))
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
               VT_STRING, netname, SA_APPEND, NULL); // x, y, rot, height
         }
      }

#ifdef _DEBUG
   CString val;
   val.Format("%ld", linecnt);
   doc->SetUnknownAttrib(&d->getAttributesRef(), "LINENR", val,
            SA_OVERWRITE, NULL); // x, y, rot, height
#endif
   } // only viaflg 2 and 5

   return(err);
}

/****************************************************************************/
/*
  unit PSD
*/
static int unit_PSD(FILE *ifp)
{
   int  len,l,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   pin400cnt = num_rec = atoi(token);

   if (num_rec)
   {
      /* init pad 400 padstack */
      if ((pin400 = (CIIPin400 *)calloc(num_rec,sizeof(CIIPin400))) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
         return -1;
      }
      for (i=0;i<num_rec;i++)
      {
         /* init pad 400 padstack */
         pin400[i].used = 1;
         if ((pin400[i].pindef = (CIIPindef400 *)calloc(num_of_layers, sizeof(CIIPindef400))) == NULL)
         {
            MemErrorMessage(__FILE__, __LINE__);
            return -1;
         }
      }
   }


  for (i=0;i<num_rec;i++)
    for (l=0;l<num_of_layers;l++)
       if ((err = do_402(ifp,i,l)) != 0)
        {
          fprintf(ferr,"Error in do_402\n");
           return -1;
        }

  return(err);
}

/****************************************************************************/
/*
  do 402
*/
static int do_402(FILE *ifp,int padnr,int lnr)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 402) r_error(atoi(token),402);
  if (!nextword())  p_error();
  len = atoi(token);

  switch (act_ciirev)
  {
    case  2:
      if (len != 8) t_error(len,8);
      break;
    default:
      if (len != 10) t_error(len,10);
  }

  if (!nextword())  p_error();
  pin400[padnr].pindef[lnr].plateflg = atoi(token);
  if (!nextword())  p_error();
  pin400[padnr].pindef[lnr].padflg = atoi(token);
  if (!nextword())  p_error();
  pin400[padnr].pindef[lnr].hole = atol(token);
  if (!nextword())  p_error();
  pin400[padnr].pindef[lnr].pad1 = atol(token);
  if (!nextword())  p_error();
  pin400[padnr].pindef[lnr].pad2 = atol(token);
  pin400[padnr].pindef[lnr].geomptr = 0;

  switch (act_ciirev)
  {
    case  2:
      break;
    default:
      if (!nextword())  p_error();
      /* rec.u.rec402.i2psd_schedflg = atoi(token); */
      if (!nextword())  p_error();
      pin400[padnr].pindef[lnr].geomptr = atoi(token);
  }

  return(err);
}

/****************************************************************************/
/*
  unit LAB
*/
static int unit_LAB(FILE *ifp)
{
   int  len;
   int  err  = 0;
   long  i,num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);
   
   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_422(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_422\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
*/
static int get_compptr(int compindex)
{
   int   i;

   for (i=0;i<compcnt;i++)
   {
#ifdef _DEBUG  
      CIICompname *c = comparray[i];
#endif
      if (comparray[i]->index == compindex)
      {
         return i;
      }
   }

   return -1;
}

/****************************************************************************/
/*
  do 422
*/
static int do_422(FILE *ifp)
{
   int      len,i;
   int      err  = 0;
   Lm       layer;   
   int      typ; /* 1 see layer mask */
                 /* 2 silkscreen text */
                 /* >2 type ignore */
   char     ln[10];
   int      rot;
   double     w,h,x,y;
   int      desflg;
   int      mir = 0;
   int      prop = FALSE;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 422) r_error(atoi(token),422);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 14) t_error(len,14);

   if (!nextword())  p_error();
   typ = atoi(token); 
   if (!nextword())  p_error();

   /* Layer is ignored in type label */
   layermask(&layer,token); 

   if (!nextword())  p_error();
   h = atol(token); 
   if (!nextword())  p_error();
   w = atol(token); 
   if (!nextword())  p_error();
   rot = atoi(token); 
   if (!nextword())  p_error();
   x = on_grid(atol(token));
   if (!nextword())  p_error();
   y = on_grid(atol(token)); 
   if (!nextword())  p_error();
   /* rec.u.rec422.i2font_flg = atoi(token); */
   if (!nextword())  p_error();
   desflg = atoi(token);

   if ((err = do_424(ifp)) != 0)
   {
        fprintf(ferr,"Error in do_424\n");
        return -1;
   }

   switch (typ)
   {
      case  1:
      {
         for (i=0;i<32;i++)
         {
            if (layer & (1L << (32-(i+1))))
            {
               sprintf(ln,"%d",i+1);   
               mir = 0;
               if ((i+1) == num_of_layers)   
               {
                  rot = 360 - rot;
                  mir = 1;
               }
               // here by layer
               int level = Graph_Level(ln,"",0);
               // cii_unit(w) 
               Graph_Text(level,prosa,cii_unit(x),cii_unit(y),
                  cii_unit(h),cii_unit(h)*6.0/8.0,DegToRad(rot),
                  0, prop, 
                  mir,  
                  0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
            }
         }
      }
      break;
      case  2:
      {
         /* normalize rotation */
         if (desflg)
         {
            int   cmpptr = get_compptr(desflg);

            if (cmpptr > -1)
            {
               double   x1, y1;
               DataStruct *d = datastruct_from_refdes(doc, file->getBlock(), comparray[cmpptr]->name);
			   
			   if ((d != NULL) && (d->getInsert() != NULL))
			   {
			      InsertStruct *thisComp = d->getInsert();
				  rot = rot - comparray[cmpptr]->rot;
                  x = cii_unit(x) - thisComp->getOriginX(); //x = x - comparray[cmpptr]->x;
                  y = cii_unit(y) - thisComp->getOriginY(); //y = y - comparray[cmpptr]->y;
                  if (rot < 0)   rot += 360;
                  Rotate(x,y,360.0-thisComp->getAngleDegrees(),&x1,&y1);
            
                  /* this is done in 422 record */
                  int laynr = Graph_Level("LIB_1", "" , 0);

                  doc->SetUnknownVisAttrib(&d->getAttributesRef(), ATT_REFNAME,
                     comparray[cmpptr]->name,    // this makes a "real" char *
                     x1, y1, DegToRad(rot), 
                     cii_unit(h),cii_unit(h)*6.0/8.0, 1, 0, TRUE, SA_OVERWRITE, 0L, laynr, 0, 0, 0);
			   }
            }
         } /* this is no component */
         else
         {
            /* this are other layers */
            sprintf(ln,"TXTTYP%d",typ);

            int level = Graph_Level(ln,"",0);
            // cii_unit(w) 
            Graph_Text(level,prosa,cii_unit(x),cii_unit(y),
               cii_unit(h),cii_unit(h)*6.0/8.0,DegToRad(rot),
               0, prop, 
               mir,  
               0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
         }
      }
      break;
      default:
      {
         /* this are other layers */
         sprintf(ln,"TXTTYP%d",typ-2);
         int level = Graph_Level(ln,"",0);
         // cii_unit(w) 
         Graph_Text(level,prosa,cii_unit(x),cii_unit(y),
            cii_unit(h),cii_unit(h)*6.0/8.0,DegToRad(rot),
            0, prop, 
            mir,  
            0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
      }
      break;
   }
   
   return(err);
}

/****************************************************************************/
/*
  do 424
*/
static int do_424(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 424) r_error(atoi(token),424);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 32) t_error(len,32);

  if (!numword(64,1))  p_error();
  strcpy(prosa,token);
  return(err);
}

/****************************************************************************/
/*
  unit SYM
*/
static int unit_SYM(FILE *ifp)
{
   int  len;
   int  err  = 0;
   long  i,num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      {
         if ((err = do_502(ifp)) != 0)
         {
            fprintf(ferr,"Error in do_502\n");
            return -1;
         }
         if ((err = do_504(ifp)) != 0)
         {
            fprintf(ferr,"Error in do_504\n");
            return -1;
         }
         if ((err = do_508(ifp)) != 0)
         {
            fprintf(ferr,"Error in do_508\n");
            return -1;
         }
         if ((err = do_511(ifp)) != 0)
         {
            fprintf(ferr,"Error in do_511\n");
            return -1;
         }
         if ((err = do_512(ifp)) != 0)
         {
            fprintf(ferr,"Error in do_512\n");
            return -1;
         }
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 502
*/
static int do_502(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 502) r_error(atoi(token),502);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 22) t_error(len,22);

   if (!nextword())  p_error();
   libclass = atoi(token); 
   if (!nextword())  p_error();
   shapclass = atoi(token);

   if (!numword(32,0))  p_error();
   strcpy(libname,token);

   if (!nextword())  p_error();             
   minx = miny = 0L; 
   maxx = atol(token); 
   if (!nextword())  p_error();
   maxy = atol(token);

   BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
   Graph_Block_Off();

   return(err);
}

/****************************************************************************/
/*
  do 504
*/
static int do_504(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;
   
   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 504) r_error(atoi(token),504);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   graphcnt = 0;
   for(i=0;i<num_rec;i++)
   {
      if ((err = do_506(ifp))!=0)
      {
        fprintf(ferr,"Error in do_506\n");
         return -1;
      }
   }

   if (libclass == 1) // only assembly Symbol definition
   {
      int   grclass = 0;
      BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
      write_graph(OLDSHAPEOUTLINETOP,0l, grclass, 0);
      Graph_Block_Off();
   }

   graphcnt = 0;
   return(err);
}

/****************************************************************************/
/*
  do 506
*/
static int do_506(FILE *ifp)
{
   int  len;
   int  err  = 0;
   int    typ;
   long x1,y1,x2,y2;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 506) r_error(atoi(token),506);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 9) t_error(len,9);

   if (!nextword())  p_error();
   typ = atoi(token); 
   if (!nextword())  p_error();
   x1 = atol(token); 
   if (!nextword())  p_error();
   y1 = atol(token); 
   if (!nextword())  p_error();
   x2 = atol(token); 
   if (!nextword())  p_error();
   y2 = atol(token); 

   if (graphcnt < MAX_GRAPH)
   {
      graph[graphcnt].typ = typ;
      graph[graphcnt].x1  = x1;
      graph[graphcnt].y1  = y1;
      graph[graphcnt].x2  = x2;
      graph[graphcnt].y2  = y2;
      graphcnt++;
   }
   else
   {
      fprintf(ferr, "Too many graphic elements\n");
      display_error++;
      return -1;
   }

   return(err);
}

/****************************************************************************/
/*
  do 508
*/
static int do_508(FILE *ifp)
{
  int  len,i;
  int  err  = 0;
  int  num_rec;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 508) r_error(atoi(token),508);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 1) t_error(len,1);

  if (!nextword())  p_error();
  num_rec = atoi(token);

  for(i=0;i<num_rec;i++)
  {
    if ((err = do_510(ifp))!=0)
      {
        fprintf(ferr,"Error in do_510\n");
         return -1;
      }
  }
  return(err);
}

/****************************************************************************/
/*
  do 510
*/
static int do_510(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 510) r_error(atoi(token),510);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 41) t_error(len,41);

  if (!nextword())  p_error();
  /* rec.u.rec510.i2text_type = atoi(token); */
  if (!nextword())  p_error();
  /* rec.u.rec510.i2sym_rot = atoi(token); */
  if (!nextword())  p_error();
  /* rec.u.rec510.isym_height = atol(token); */
  if (!nextword())  p_error();
  /* rec.u.rec510.i2_orien = atoi(token); */
  if (!nextword())  p_error();
  /* rec.u.rec510.isym_refx = atol(token); */
  if (!nextword())  p_error();
  /* rec.u.rec510.isym_refy = atol(token); */
  if (!numword(64,0))  p_error();
  /* strncpy(&rec.u.rec510.asym_text[0],&token[0],sizeof(rec.u.rec510.asym_text)); */

  return(err);
}

/****************************************************************************/
/*
  do 511
*/
static int do_511(FILE *ifp)
{
   int  len;
   int  err  = 0;
   long w,h,d;
   
   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 511) r_error(atoi(token),511);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 8) t_error(len,8);

   if (!nextword())  p_error();
   d = atol(token); 
   if (!nextword())  p_error();
   w = atol(token); 
   if (!nextword())  p_error();  
   h = atol(token); 
   if (!nextword())  p_error();
   pitch = atol(token); 

   //if (ignore_oldshape)    return(err);
   return (err);

   if (libclass != 2) return(err);
   curr_padstack = get_pin514(w,h,d,shapclass);

   return(err);
}

/****************************************************************************/
/*
  do 512
*/
static int do_512(FILE *ifp)
{
  int  len,i;
  int  err  = 0;
  int  num_rec;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 512) r_error(atoi(token),512);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 1) t_error(len,1);
   
  if (!nextword())  p_error();
  num_rec = atoi(token);

  pincnt = 0;
  for (i=0;i<num_rec;i++)
  {
    if ((err = do_514(ifp))!=0)
      {
        fprintf(ferr,"Error in do_514\n");
         return -1;
      }
  }
  return(err);
}

/****************************************************************************/
/*
  do 514
*/
static int do_514(FILE *ifp)
{
   int   len;
   int   err  = 0;
   long  x,y;
   long  width,height;
// char  padname[10],stackname[10];
   unsigned char  mir = 0;
      
   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 514) r_error(atoi(token),514);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 8) t_error(len,8);
   
   pincnt++;   

   if (!nextword())  p_error();
   x = atol(token); 
   if (!nextword())  p_error();
   y = atol(token); 
   if (!nextword())  p_error();
   width = atol(token); 
   if (!nextword())  p_error();
   height = atol(token); 

   //if (ignore_oldshape)  return(err);
   return (err);

#ifdef WOLF
   if (shapclass == 2)
   {
      x = (pitch * (pincnt-1));
      y = 0;
   }

   if (pincnt == 1)
   {
      library[librarycnt-1].pin1x = x;
      library[librarycnt-1].pin1y = y;
   }

   sprintf(padname,"%d",pincnt);
   sprintf(stackname,"%s%d",PADNAME_514,curr_padstack);
   if ((res = L_Pin(padname,libname,stackname,pincnt)) != OK)
   {
      switch (res)
      {
         case 1:
            fprintf(ferr,"Pin [%s] [%s] [%s] already exist\n",
               padname,libname,stackname);
         break;
         default:
            fprintf(ferr, "L_Pin 514 error %d [%s] [%s] [%s]\n",res,padname,libname,stackname);
         break;
      }
   }

   if ((res = L_PinPos(padname,libname,
                  inch_2_dbunitpoint2(cii_unit(x),cii_unit(y)),
                  mir,degree_2_dbunit(0.0))) != OK)
   {
      fprintf(ferr, "L_PinPos error %d\n",res);
      return -1;
   }
#endif
   return(err);
}

/****************************************************************************/
/*
  unit NSH
*/
static int unit_NSH(FILE *ifp)
{
   int  len;
   int  err  = 0;
   long  i,num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_522(ifp))!=0)
      {
         fprintf(ferr,"Error in do_522\n");
         return -1;
      }
      if ((err = do_524(ifp))!=0)
      {
         fprintf(ferr,"Error in do_524\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 522
*/
static int do_522(FILE *ifp)
{
   int  len;
   int  err  = 0;
   int  lptr;
   long maxheight;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 522) r_error(atoi(token),522);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 30) t_error(len,30);

   if (!numword(32,0))  p_error();
   strcpy(libname,token);

   if (!nextword())  p_error();
   /* rec.u.rec522.insh_offsetx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec522.insh_offsety = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec522.imin_height = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec522.imax_height = atol(token); */
   if (!nextword())  p_error();
   maxheight = atol(token);
   if (!nextword())  p_error();
   /* rec.u.rec522.i2lay_orien = atoi(token); */
   if (!nextword())  p_error();
   bot_layer = atoi(token);
   if (!nextword())  p_error();
   ext_layer = atoi(token);
   /* rec.u.rec522.iextend_flg = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec522.invert_flg = atoi(token); */

   BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
   b->setBlockType(BLOCKTYPE_PCBCOMPONENT);

   if (librarycnt < MAX_LIBRARY)
   {
      if ((library[librarycnt].name = STRDUP(libname)) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
         return -1;
      }
      library[librarycnt].oldshape = FALSE;
      library[librarycnt].height = maxheight;
      librarycnt++;
   }
   else
   {
      fprintf(ferr,"Too many library shapes\n");
      return -1;
   }

   Graph_Block_Off();
   lptr = get_libptr(libname);
   library[lptr].oldshape = FALSE;

   return(err);
}

/****************************************************************************/
/*
  do 524
*/
static int do_524(FILE *ifp)
{
   int  len;
   int  i,err  = 0;
   int  n_outline,n_pin,n_pad,n_hole,n_etch,n_void;
   int  n_finger,n_shttab,n_plbar,n_group;

   flg524 = TRUE;
   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 524) r_error(atoi(token),524);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 10) t_error(len,10);

   if (!nextword())  p_error();
   n_outline = atoi(token);
   if (!nextword())  p_error();
   n_pin = atoi(token);
   if (!nextword())  p_error();
   n_pad = atoi(token);
   if (!nextword())  p_error();
   n_hole = atoi(token);
   if (!nextword())  p_error();
   n_etch = atoi(token);
   if (!nextword())  p_error();
   n_void = atoi(token);
   if (!nextword())  p_error();
   n_finger = atoi(token);
   if (!nextword())  p_error();
   n_shttab = atoi(token);
   if (!nextword())  p_error();
   n_plbar = atoi(token);
   if (!nextword())  p_error();
   n_group = atoi(token);
   pincnt = 0; 

   BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
   b->setBlockType(BLOCKTYPE_PCBCOMPONENT);

   for (i=0;i<n_outline;i++)
   {
      if ((err = do_528(ifp))!=0)
      {
         fprintf(ferr,"Error in do_528\n");
         return -1;
      }
   }

   for (i=0;i<n_pin;i++)
   {
      fprintf(ferr, "Ungrouped pins ignored at %ld\n", linecnt);
      display_error++;
      if (!nextline(ifp))  l_error();

/*
      if ((err = do_532(ifp,n_pin))!=0)
      {
         fprintf(ferr,"Error in do_532\n");
         return -1;
      }
*/
   }


   for (i=0;i<n_pad;i++)
   {
      fprintf(ferr, "Ungrouped pads ignored at %ld\n", linecnt);
      display_error++;
      if (!nextline(ifp))  l_error();

/*
      if ((err = do_534(ifp,FALSE))!=0)   // do not translate if not in 552
      {
         fprintf(ferr,"Error in do_534\n");
         return -1;
      }
*/
   }

   for (i=0;i<n_hole;i++)
   {
      fprintf(ferr, "Ungrouped holes ignored at %ld\n", linecnt);
      display_error++;
      if (!nextline(ifp))  l_error();
/*
      if ((err = do_536(ifp))!=0)
      {
         fprintf(ferr,"Error in do_536\n");
         return -1;
      }
*/
   }

   for (i=0;i<n_etch;i++)
   {
      if ((err = do_538(ifp))!=0)
      {
         fprintf(ferr,"Error in do_538\n");
         return -1;
      }
   }

   for (i=0;i<n_void;i++)
   {
      if ((err = do_542(ifp))!=0)
      {
         fprintf(ferr,"Error in do_542\n");
         return -1;
      }
   }

   for (i=0;i<n_finger;i++)
   {
      if ((err = do_544(ifp, FALSE))!=0)
      {
         fprintf(ferr,"Error in do_544\n");
         return -1;
      }
   }

   for (i=0;i<n_shttab;i++)
   {
      if ((err = do_550(ifp))!=0)
      {     
         fprintf(ferr,"Error in do_550\n");
         return -1;
      }
   }

   for (i=0;i<n_plbar;i++)
   {
      if ((err = do_546(ifp))!=0)
      {
         fprintf(ferr,"Error in do_546\n");
         return -1;
      }
   }

   for (i=0;i<n_group;i++)
   {
      if ((err = do_552(ifp,n_group))!=0)
      {
         fprintf(ferr,"Error in do_552\n");
         return -1;
      }
   }
  
   Graph_Block_Off();

   flg524 = FALSE;
   return(err);
}

/****************************************************************************/
/*
*/ 
static int get_complexpin()
{
   int   i, ii, equal;

   // here check if it already exist
   for (i=0;i<complexpin_cnt;i++)
   {
      equal = 0;
      if (complexpin[i].complexdrillcnt != newshapedrillcnt)   continue;
      if (complexpin[i].complexpadcnt != newshapepadcnt)    continue;

      for (ii=0;ii<complexpin[i].complexpadcnt;ii++)
      {
         if (complexpin[i].complexpad[ii].geomptr != newshapepad[ii].geomptr) break;
         if (complexpin[i].complexpad[ii].layer != newshapepad[ii].layer)     break;
         if (complexpin[i].complexpad[ii].typ != newshapepad[ii].typ)         break;
         if (complexpin[i].complexpad[ii].x != (newshapepad[ii].x - newshapepin.x)) break;
         if (complexpin[i].complexpad[ii].y != (newshapepad[ii].y - newshapepin.y)) break;
         equal++;
      }

      if (equal != newshapepadcnt)  continue;

      for (ii=0;ii<newshapedrillcnt;ii++)
      {
         if (complexpin[i].complexdrill[ii].drill != newshapedrill[ii].drill) break;
         if (complexpin[i].complexdrill[ii].x != (newshapedrill[ii].x - newshapepin.x))   break;
         if (complexpin[i].complexdrill[ii].y != (newshapedrill[ii].y - newshapepin.y))   break;
         equal++;
      }

      // test against all matches ! every drill and shape
      if (equal == (newshapedrillcnt + newshapepadcnt))  
         return i;
   }

   // here make a new complexpin 
   if (complexpin_cnt < MAX_COMPLEXPIN)
   {

      if (newshapedrillcnt)
      {
         complexpin[complexpin_cnt].complexdrillcnt = newshapedrillcnt;
         complexpin[complexpin_cnt].complexdrill = (CIINewshapedrill *)calloc(newshapedrillcnt, sizeof(CIINewshapedrill));
         for (ii=0;ii<newshapedrillcnt;ii++)
         {
            complexpin[complexpin_cnt].complexdrill[ii].drill = newshapedrill[ii].drill;
            complexpin[complexpin_cnt].complexdrill[ii].x = newshapedrill[ii].x - newshapepin.x;
            complexpin[complexpin_cnt].complexdrill[ii].y = newshapedrill[ii].y - newshapepin.y;
         }
      }
      if (newshapepadcnt)
      {
         complexpin[complexpin_cnt].complexpadcnt = newshapepadcnt;
         complexpin[complexpin_cnt].complexpad = (CIINewshapepad *)calloc(newshapepadcnt, sizeof(CIINewshapepad));
         for (ii=0;ii<newshapepadcnt;ii++)
         {
            complexpin[complexpin_cnt].complexpad[ii].geomptr = newshapepad[ii].geomptr;
            complexpin[complexpin_cnt].complexpad[ii].layer = newshapepad[ii].layer;
            complexpin[complexpin_cnt].complexpad[ii].typ = newshapepad[ii].typ;
            complexpin[complexpin_cnt].complexpad[ii].x = newshapepad[ii].x - newshapepin.x;
            complexpin[complexpin_cnt].complexpad[ii].y = newshapepad[ii].y - newshapepin.y;
         }
      }
      complexpin_cnt++;
   }
   else
   {
      fprintf(ferr, "Too many new complex pins at %ld\n", linecnt);
      display_error++;
      return -1;
   }
   return complexpin_cnt -1;
}

/****************************************************************************/
/*
   The netlist pins 330 are done by pinsequence, but if a name is a name, I need
   to update the netlist

*/
static int update_all_comps_netlist(const char *libname, const char *pinname, 
                                    const char *pnr)
{
   int   i;
   
   for (i=0;i<compcnt;i++)
   {
      if (comparray[i]->libname.Compare(libname) == 0)
      {
         CompPinStruct *c = test_add_comppin(comparray[i]->name, pnr, file);
         if (c)
         {
            c->setPinName(pinname);
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
   placement flag 1 = pin1
                  2 = bodycenter
*/
static int update_all_comps_pin1(const char *libname, const char *pin1)
{
   int   i;
   
   for (i=0;i<compcnt;i++)
   {
      CIICompname *c = comparray[i];
      if (comparray[i]->libname.Compare(libname) == 0)
      {
         comparray[i]->pin1 = pin1;
      }
   }

   return 1;
}

/****************************************************************************/
/*
  do 552
*/ 
static int do_552(FILE *ifp,int ngroup)
{
   int  len;
   int  i,err  = 0;
   int  n_outln,n_pin,n_pad,n_hole,n_etch,n_void,n_finger,n_shttab;
   char  stackname[10];
   unsigned char  mir = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 552) r_error(atoi(token),552);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 8) t_error(len,8);

   if (!nextword())  p_error();
   n_outln = atoi(token);
   if (!nextword())  p_error();
   n_pin = atoi(token);
   if (!nextword())  p_error();
   n_pad = atoi(token);
   if (!nextword())  p_error();
   n_hole= atoi(token);
   if (!nextword())  p_error();
   n_etch = atoi(token);
   if (!nextword())  p_error();
   n_void = atoi(token);
   if (!nextword())  p_error();
   n_finger = atoi(token);
   if (!nextword())  p_error();
   n_shttab = atoi(token);

   if (n_pin > 1) 
   {
      fprintf(ferr,"Multiple Pin definitions in a single Pin in %s not allowed at %ld\n",libname, linecnt);
      display_error++;
   }
/*
   if (n_pad > 1) 
      fprintf(ferr,"Multiple pad definition in %s at %ld\n",libname, linecnt); 
*/
   if (n_hole > 1) 
   {
      fprintf(ferr,"Multiple drill hole definitions in a single Pin in %s at %ld\n",libname, linecnt);
      display_error++;
   }

   for (i=0;i<n_outln;i++)
   {
      if ((err = do_528(ifp))!=0)
      {
         fprintf(ferr,"Error in do_528\n");
         return -1;
      }
   }

   newshapepin.pinname = "";
   // here load newshapepin - only one is allowed
   for (i=0;i<n_pin;i++)
   {
      if ((err = do_532(ifp,ngroup))!=0)
      {
         fprintf(ferr,"Error in do_532 at %ld\n", linecnt);
         return -1;
      }
   }

   // here load multiple newshapepads
   newshapepadcnt = 0;
   for (i=0;i<n_pad;i++)
   {
      if ((err = do_534(ifp,TRUE))!=0)
      {
         fprintf(ferr,"Error in do_534 at %ld\n", linecnt);
         return -1;
      }
   }

   newshapedrillcnt = 0;
   // here load multiple newshapedrills
   for (i=0;i<n_hole;i++)
   {
      if ((err = do_536(ifp))!=0)
      {
         fprintf(ferr,"Error in do_536 at %ld\n", linecnt);
         return -1;
      }
   }

   for (i=0;i<n_etch;i++)
   {
      if ((err = do_538(ifp))!=0)
      {
         fprintf(ferr,"Error in do_538 at %ld\n", linecnt);
         return -1;
      }
   }

   for (i=0;i<n_void;i++)
   {
      if ((err = do_542(ifp))!=0)
      {
         fprintf(ferr,"Error in do_542\n");
         return -1;
      }
   }

   for (i=0;i<n_finger;i++)
   {
      if ((err = do_544(ifp, TRUE))!=0)
      {
         fprintf(ferr,"Error in do_544\n");
         return -1;
      }
   }

   for (i=0;i<n_shttab;i++)
   {
      if ((err = do_550(ifp))!=0)
      {
         fprintf(ferr,"Error in do_550\n");
         return -1;
      }
   }

   if (!pincnt)   return(err);   // there was a group, but this had not pin definitions.

   // here get a unique padstack
   sprintf(stackname,"%s%d",PADNAME_560, get_complexpin());

   // only check on electrical pins
   if (newshapepin.pinnr == 0 && newshapepin.flg == 1)
   {
      /* make unique pinnr */
      newshapepin.pinnr = n_pin + pincnt + 1;
      fprintf(ferr,"Unnamed Pin [%s -> %d] in shape %s at %ld\n",
                token,newshapepin.pinnr,libname,linecnt);
      display_error;
   }

   DataStruct *d = Graph_Block_Reference(stackname, newshapepin.pinname, 0, 
         cii_unit(newshapepin.x),cii_unit(newshapepin.y), 0.0, 0 , 1.0, -1, TRUE);
   if (newshapepin.flg == 1 && strlen(newshapepin.pinname))
   {
      d->getInsert()->setInsertType(insertTypePin);
      // do pinnr here
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, 0),
         VT_INTEGER, &(newshapepin.pinnr), SA_OVERWRITE, NULL); //

      CString  pnr;
      pnr.Format("%d", newshapepin.pinnr);
      // if pinname != pinnr, update netlist
      if (newshapepin.pinname.Compare(pnr))  // if the pinname and number is not equal.
      {
         update_all_comps_netlist(libname, newshapepin.pinname, pnr); 
      }

      if (newshapepin.pinnr == 1)
      {
         update_all_comps_pin1(libname, newshapepin.pinname); 
      }
   }
   else
   {
      d->getInsert()->setInsertType(insertTypeMechanicalPin);
   }

   return(err);
}

/****************************************************************************/
/*
  do 528
*/
static int do_528(FILE *ifp)
{
   int  len;
   int  i,geom,err  = 0;
   char layer[33];
   int  curr_libptr;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 528) r_error(atoi(token),528);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 3) t_error(len,3);

   if (!nextword())  p_error();
   layermask(&shapelayer,token);
   if (!nextword())  p_error();
   geom = atoi(token);

   minx = miny =  320000L; /* this is in 1/10 mil */
   maxx = maxy = -320000L;
   graphcnt = 0;

   if (!err)
   {
      for (i=0;i<geom;i++)
      {
         if ((err = do_530(ifp))!=0)
         {
            fprintf(ferr,"Error in do_530\n");
            return -1;
         }
      }
   }

   for (i=0;i<32;i++)
   {
      if (shapelayer & (1L << (32-(i+1))))
      {
         int   grclass = 0;

         /* get library minmax */
         update_librarygeom();
         sprintf(layer,"LIB_%d",i+1);

         if (i == 0)
         {
            grclass = GR_CLASS_COMPOUTLINE;
         }

         write_graph(layer,0l, grclass, 0);
         /* get library minmax */
         if (flg524)
         {
            curr_libptr = get_libptr(libname);
            update_librarygeom();
            library[curr_libptr].centerx = (maxx + minx) /2;
            library[curr_libptr].centery = (maxy + miny) /2;
         }
      }
   }
   graphcnt = 0;
   return(err);
}

/****************************************************************************/
/*
  do 530
*/
static int do_530(FILE *ifp)
{
   int  len;
   int  err  = 0;
   int    typ;
   long x1,y1,x2,y2;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 530) r_error(atoi(token),530);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 9) t_error(len,9);

   if (!nextword())  p_error();
   typ = atoi(token); 
   if (!nextword())  p_error();
   x1 = atol(token); 
   if (!nextword())  p_error();            
   y1 = atol(token); 
   if (!nextword())  p_error();
   x2 = atol(token); 
   if (!nextword())  p_error();
   y2 = atol(token); 

   if (graphcnt < MAX_GRAPH)
   {
      graph[graphcnt].typ = typ;
      graph[graphcnt].x1  = x1;
      graph[graphcnt].y1  = y1;
      graph[graphcnt].x2  = x2;
      graph[graphcnt].y2  = y2;
      graphcnt++;
   }
   else
   {
      fprintf(ferr,"Too many graphic elements\n");
      return -1;
   }

   return(err);
}

/****************************************************************************/
/*
  do 532
*/
static int do_532(FILE *ifp,int maxpin)
{
   int   len;
   int   err  = 0;
   int   curr_libptr;
   Lm    layer;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 532) r_error(atoi(token),532);
   if (!nextword())  p_error();
   switch (act_ciirev)
   {
      case 10:
      case  9:
      case  8:
      case  7:
         if ((len = atoi(token)) != 24) t_error(len,24);
      break;
      default:
         if ((len = atoi(token)) != 23) t_error(len,23);
      break;
   }

   if (!nextword())  p_error();
   newshapepin.flg = atoi(token); 
   if (!nextword())  p_error();
   layermask(&layer,token);
   extent_layer(&layer,bot_layer);

   newshapepin.layer = layer;
   if (!numword(32,0))  p_error();
   newshapepin.pinname = token;

   if (!nextword())  p_error();
   newshapepin.x = atol(token);
   if (!nextword())  p_error();
   newshapepin.y = atol(token);

   if (act_ciirev > 6)
   {
   if (!nextword())  p_error();
      /* this is pin number */
      newshapepin.pinnr = atoi(token);
   }

   pincnt++;
   if (newshapepin.pinnr == 1)
   {
      curr_libptr = get_libptr(libname);
      library[curr_libptr].pin1x = newshapepin.x;
      library[curr_libptr].pin1y = newshapepin.y;
   }

   return(err);
}

/****************************************************************************/
/*
  do 534
*/
static int do_534(FILE *ifp,int pingroup)
{
   int   len;
   int   err  = 0;
   int   typ;
   int   lptr, geomptr;
   long  x,y;
   Lm    layer;
   int   err1 = FALSE, err2 = FALSE , err3 = FALSE;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 534) r_error(atoi(token),534);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 10) t_error(len,10);

   if (!nextword())  p_error();
   typ = atoi(token);
   if (!nextword())  p_error();
   layermask(&layer,token);

   extent_layer(&layer,bot_layer);
   if (typ == 1)  // pin typ
      layer &= newshapepin.layer;
   // can only exist there also pin exist. I found samples on SMD where the PASTE in on -1-3- and the pin on -1-

   if (!nextword())  p_error();
   /* rec.u.rec534.i2schdflg = atoi(token); */
   if (!nextword())  p_error();
   x = atol(token); 
   if (!nextword())  p_error();
   y = atol(token);
   if (!nextword())  p_error();
   /* rec.u.rec534.ipadtype = atoi(token); */
   if (!nextword())  p_error();
   geomptr = atoi(token); 

   if (!pingroup) return(err);

   lptr = get_libptr(libname);

   // from 544 record -1 is TYPFINGER
   // pintype 1 Component pin
   // 2 Via
   // 3 test
   // 4 wire bond
   // 5 mechanical
   // 6 diemount - this is a component pin for SMD
   // 7 solder resist
   if (typ > 7)   return err;
   if (typ < 1)   return err;

   // at this moment, record 560 is not yet avail.
   if (newshapepadcnt < MAX_NEWSHAPEPAD)
   {
      newshapepad[newshapepadcnt].x = x;
      newshapepad[newshapepadcnt].y = y;
      newshapepad[newshapepadcnt].geomptr = geomptr;
      newshapepad[newshapepadcnt].layer = layer;
      newshapepad[newshapepadcnt].typ = typ;
      newshapepadcnt++;
      // break; // break from loop; may be multiple layers.
   }
   else
   {
      CString  t;
      t.Format("Too many complex pads [%d] at %ld", MAX_NEWSHAPEPAD, linecnt);
      ErrorMessage(t);
   }

   return(err);
}

/****************************************************************************/
/*
  do 536
*/
static int do_536(FILE *ifp)
{
   int  len;
   int  err  = 0;
   long  drill,x,y;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 536) r_error(atoi(token),536);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 10) t_error(len,10);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec536.ihole_mask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec536.i2hole_type = atoi(token); */
   if (!nextword())  p_error();
   drill = atol(token); 
   if (!nextword())  p_error();
   /* rec.u.rec536.i2plate_flg = atoi(token); */
   if (!nextword())  p_error();
   x = atol(token); 
   if (!nextword())  p_error();
   y = atol(token); 

   if (newshapedrillcnt < MAX_NEWSHAPEDRILL)
   {
      newshapedrill[newshapedrillcnt].drill = drill;
      newshapedrill[newshapedrillcnt].x = x;
      newshapedrill[newshapedrillcnt].y = y;
      newshapedrillcnt++;
   }
   else
   {
      fprintf(ferr,"Too many drills at %ld\n", linecnt);
      return -1;
   }

   return(err);
}

/****************************************************************************/
/*
  do 538 Etch item on a component shape
*/
static int do_538(FILE *ifp)
{
   int   len;
   int   i, geom, err  = 0;
   long  width;
   Lm    layer;
   int   fill;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 538) r_error(atoi(token),538);
   if (!nextword())  p_error();
   len = atoi(token);

   switch (act_ciirev)
   {
      case 10:
      case  9:
      case  8:
      case  7:
         if (len != 8) t_error(len,8);
      break;
      default:
         if (len != 7) t_error(len,7);
      break;        
   }

   if (!nextword())  p_error();
   /* rec.u.rec538.i2etch_flg = atoi(token); */
   if (!nextword())  p_error();
   layermask(&layer,token);
   extent_layer(&layer,bot_layer);

   if (!nextword())  p_error();
   width = atol(token); 
   if (!nextword())  p_error();
   fill = atol(token); 
   if (!nextword())  p_error();
   geom = atoi(token);

   if (act_ciirev > 6)
   {
      if (!nextword())  p_error();  /* need cii 7 doc. */
   }

   if (!err) 
   {
      for (i=0;i<geom;i++)
      {
         if ((err = do_530(ifp))!=0)
         {
            fprintf(ferr,"Error in do_530\n");
            return -1;
         }
      }
   }

   // here is a etch item on this pin or component shape.
   BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
   for (i=0;i<32;i++)
   {
      if (layer & (1L << (32-(i+1))))
      {
         char  lname[80];
         int   grclass = 0;
         sprintf(lname,"%d",i+1);
         write_graph(lname, width, grclass, fill);
      }
   }
   Graph_Block_Off();

   graphcnt = 0;

   return(err);
}

/****************************************************************************/
/*
  do 542
*/
static int do_542(FILE *ifp)
{
   int   len, void_flg;
   int   i,geom,err  = 0;
   Lm    layer;
   int   width = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 542) r_error(atoi(token),542);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 4) t_error(len,4);

   if (!nextword())  p_error();
   void_flg = atoi(token); 
   if (!nextword())  p_error();
   layermask(&layer,token);
   if (!nextword())  p_error();
   geom = atoi(token);

   if (!err) 
   {
      for (i=0;i<geom;i++)
         if ((err = do_530(ifp))!=0)
         {
            fprintf(ferr,"Error in do_530\n");
               return -1;
         }
   }

   // here is a etch item on this pin or component shape.
   BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
   for (i=0;i<32;i++)
   {
      if (layer & (1L << (32-(i+1))))
      {
         char  lname[80];
         int   grclass = 0; // has to be marked 

         switch (void_flg)
         {
            case 1: // trace void
               grclass = GR_CLASS_ROUTKEEPOUT;
            break;
            case 2: // via void
               grclass = GR_CLASS_VIAKEEPOUT;
            break;
            case 3: // placement void
               grclass = GR_CLASS_PLACEKEEPOUT;
            break;
            case 4: // hard void
               grclass = GR_CLASS_ALLKEEPOUT;
            break;
            default:
            break;
         }

         if (grclass)   // do not write voids which are not classified.
         {
            sprintf(lname,"%d",i+1);
            write_graph(lname, width, grclass, 0);
         }
      }
   }
   Graph_Block_Off();

  graphcnt = 0;   // elim 530 count
  return(err);
}

/****************************************************************************/
/*
  do 544
*/
static int do_544(FILE *ifp, int pingroup)
{
   int  len;
   int  i,geom,err  = 0;
   Lm layer;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 544) r_error(atoi(token),544);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 3) t_error(len,3);

   if (!nextword())  p_error();
   layermask(&layer,token);

   extent_layer(&layer,bot_layer);

   if (!nextword())  p_error();
   geom = atoi(token);
   graphcnt = 0;

   if (!err) 
   {
      for (i=0;i<geom;i++)
      {
         if ((err = do_530(ifp))!=0)
         {
            fprintf(ferr,"Error in do_530\n");
            return -1;
         }
      }
   }

   if (!pingroup) 
   {
      // here is a etch item on this pin or component shape.
      BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
      for (i=0;i<32;i++)
      {
         if (layer & (1L << (32-(i+1))))
         {
            char  lname[80];
            sprintf(lname,"%d",i+1);
            write_graph(lname, 0, 0, 0);
         }
      }
      Graph_Block_Off();
   }
   else
   {
      // if graphcnt == 1 && newshapepad[newshapepadcnt-1].geomptr == 0, this is a try, singe 
      // aperture oblong pad

      int lptr = get_libptr(libname);
      if (graphcnt == 1 && newshapepad[newshapepadcnt-1].geomptr == 0)
      {
         switch (graph[0].typ)
         {
            case 5:     // box x0 y0 is lower left, x1 y1 upper right
            {
               int    p;
               char   pshapename[80];
               double w,h, ox, oy;

               // normalize to pin koos.
               graph[0].x1 -= newshapepin.x;
               graph[0].y1 -= newshapepin.y;
               graph[0].x2 -= newshapepin.x;
               graph[0].y2 -= newshapepin.y;

               w = abs(graph[0].x2 - graph[0].x1);
               h = abs(graph[0].y2 - graph[0].y1);
               ox = graph[0].x1 + w/2;
               oy = graph[0].y1 + h/2;

               if ((p = pad_rect(cii_unit(w),cii_unit(h), cii_unit(ox), cii_unit(oy))) < 0)
               {
                  sprintf(pshapename,"PS_RECT_%d",pd_rectcnt-1);
                  int   err;
                  Graph_Aperture(pshapename, T_RECTANGLE, cii_unit(w), cii_unit(h) , 
                     cii_unit(ox), cii_unit(oy), 0.0, 0, BL_APERTURE, TRUE, &err);
               }  
               else
                  sprintf(pshapename,"PS_RECT_%d",p);
         

               // at this moment, record 560 is not yet avail.
               if (newshapepadcnt < MAX_NEWSHAPEPAD)
               {
                  newshapepad[newshapepadcnt].x = newshapepin.x;
                  newshapepad[newshapepadcnt].y = newshapepin.y;
                  newshapepad[newshapepadcnt].geomptr = p;
                  newshapepad[newshapepadcnt].layer = layer;
                  newshapepad[newshapepadcnt].typ = TYP_FINGER;
                  newshapepadcnt++;
               }
               else
               {
                  CString  t;
                  t.Format("Too many complex pads [%d] at %ld", MAX_NEWSHAPEPAD, linecnt);
                  ErrorMessage(t);
               }
            }
            break;
            default:
            {
               // complex padstack
               // here is a etch item on this pin or component shape.
               BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
               for (i=0;i<32;i++)
               {
                  if (layer & (1L << (32-(i+1))))
                  {
                     char  lname[80];
                     sprintf(lname,"%d",i+1);
                     write_graph(lname, 0, 0, 0);
                  }
               }
               Graph_Block_Off();
            }
            break;
         }

/*
         make_pshapename(pshapename,'R',cii_unit(pin400[i].pindef[l].pad1),cii_unit(pin400[i].pindef[l].pad2));
            Graph_Block_Reference(pshapename, NULL, 0, 
            0.0, 0.0, 0.0, 0 , 1.0, Graph_Level(layername,"",0), TRUE);

      
         // at this moment, record 560 is not yet avail.
         if (newshapepadcnt < MAX_NEWSHAPEPAD)
         {
            newshapepad[newshapepadcnt].x = x;
            newshapepad[newshapepadcnt].y = y;
            newshapepad[newshapepadcnt].geomptr = geomptr;
            newshapepad[newshapepadcnt].layer = layer;
            newshapepad[newshapepadcnt].typ = typ;
            newshapepadcnt++;
         }
         else
         {
            CString  t;
            t.Format("Too many complex pads [%d] at %ld", MAX_NEWSHAPEPAD, linecnt);
            ErrorMessage(t);
         }
*/
      }
      else
      {
         // complex padstack
         // here is a etch item on this pin or component shape.
         BlockStruct *b = Graph_Block_On(GBO_APPEND,libname,-1,0);
         for (i=0;i<32;i++)
         {
            if (layer & (1L << (32-(i+1))))
            {
               char  lname[80];
               sprintf(lname,"%d",i+1);
               write_graph(lname, 0, 0, 0);
            }
         }
         Graph_Block_Off();
      }

   } // !pingroup

   graphcnt = 0;

   return(err);
}

/****************************************************************************/
/*
  do 546
*/
static int do_546(FILE *ifp)
{
   int  len;
   int  i,lines,err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 546) r_error(atoi(token),546);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 3) t_error(len,3);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec546.ibar_mask,token); */
   if (!nextword())  p_error();
   lines = atoi(token);

   if (!err) 
   {
      for (i=0;i<lines;i++)
      {
         if ((err = do_548(ifp))!=0)
         {
            fprintf(ferr,"Error in do_548\n");
            return -1;
         }
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 548
*/
static int do_548(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 548) r_error(atoi(token),548);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 10) t_error(len,10);

   if (!nextword())  p_error();
   /* rec.u.rec548.iln_width = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec548.ibegx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec548.ibegy = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec548.iendx = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec548.iendy = atol(token); */

   return(err);
}

/****************************************************************************/
/*
  do 550
*/
static int do_550(FILE *ifp)
{
   int  len;
   int  i,lines,err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 550) r_error(atoi(token),550);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 3) t_error(len,3);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec550.itab_mask,token); */
   if (!nextword())  p_error();
   lines = atoi(token);

   if (!err) 
   {
      for (i=0;i<lines;i++)
      {
         if ((err = do_548(ifp))!=0)
         {
            fprintf(ferr,"Error in do_548\n");
            return -1;
         }
      }
   }
   return(err);
}

/****************************************************************************/
/*
  unit PDG
*/
static int unit_PDG(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int    num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   pin560cnt = num_rec = atoi(token);

   // There is a error in CII. Even that the
   // record count is greater 256, maximum 256 
   // records are written
   // This error is apparently fixed in SCI VERSION 28 - CII 8
   if (pin560cnt > max560rec && act_ciirev < 8)
   {
      fprintf(ferr, "Warning : PAD GEOMETRY UNIT : Record Error in CII File.\n");
      fprintf(ferr, "Actual Number of PAD GEOMETRY at %ld : %d -> limited to %d\n",linecnt,pin560cnt,max560rec);
      fprintf(ferr,"Warning : PAD GEOMETRY UNIT : Record Error in CII File.\n");
      fprintf(ferr,"Actual Number of PAD GEOMETRY at %ld : %d -> limited to %d\n",linecnt,pin560cnt,max560rec);
      num_rec = pin560cnt = max560rec;
   }

   if (num_rec)
   {
      /* init pad 560 padstack */
      if ((pin560 = (CIIPin560 *)calloc(num_rec,sizeof(CIIPin560))) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
         return -1;
      }
   }

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_562(ifp,i))!=0)
      {
         fprintf(ferr,"Error in do_562\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 562
*/
static int do_562(FILE *ifp,int padnr)
{
   if (!nextline(ifp))
      l_error();

   if (!nextword())
      p_error();
   if (atoi(token) != 562)
      r_error(atoi(token), 562);
   
   int len;
   if (!nextword())
      p_error();
   if ((len = atoi(token)) != 1)
      t_error(len, 1);

   if (!nextword())
      p_error();
   int num_rec = atoi(token);

   if (num_rec > 0)
   {
      /* init pad 560 padstack */
      if ((pin560[padnr].geom = (CIIGeom *)calloc(num_rec, sizeof(CIIGeom))) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
         return -1;
      }
      pin560[padnr].geomcnt = num_rec;
   }
   else
   {
      return -1;
   }

   int err = 0;
   for (int i=0; i<num_rec; i++)
   {
      if ((err = do_564(ifp, padnr, i))!=0)
      {
         fprintf(ferr, "Error in do_564\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 564
*/
static int do_564(FILE *ifp,int padnr,int cnt)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 564) r_error(atoi(token),564);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 9) t_error(len,9);

   if (!nextword())  p_error();
   pin560[padnr].geom[cnt].typ = atoi(token);
   if (!nextword())  p_error();
   pin560[padnr].geom[cnt].x1 = atol(token);
   if (!nextword())  p_error();
   pin560[padnr].geom[cnt].y1 = atol(token);
   if (!nextword())  p_error();
   pin560[padnr].geom[cnt].x2 = atol(token);
   if (!nextword())  p_error();
   pin560[padnr].geom[cnt].y2 = atol(token);

   return(err);
}

/****************************************************************************/
/*
*/
static int write_graph(char *layer, long width, int graphic_class, int fill)
{
   int      i, widthindex, err;
   double   cx,cy,rad,sa,da;
   CPnt     *lastvertex = NULL;

   if (width == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, cii_unit(width), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   for (i=0;i<graphcnt;i++)
   {
      switch (graph[i].typ)
      {
         case  1: /* line continuation*/
            lastvertex = Graph_Vertex(cii_unit(graph[i].x1), cii_unit(graph[i].y1), 0);
         break;
         case  2: /* arc */
            /* make arc */
            // graph[i] is midpoint
            // x,y endpoint
            if (lastvertex)
            {
               ArcPoint3(lastvertex->x,lastvertex->y,
                      cii_unit(graph[i].x1),cii_unit(graph[i].y1),
                      cii_unit(graph[i].x2),cii_unit(graph[i].y2),
                      &cx,&cy,&rad,&sa,&da);
               lastvertex->bulge = (DbUnit)tan(da/4);
            }
            lastvertex = Graph_Vertex(cii_unit(graph[i].x2), cii_unit(graph[i].y2), 0);
         break;
         case  3: /* circle */
         {
            int layernr = Graph_Level(layer,"",0);

            DataStruct *d = Graph_Circle(layernr, cii_unit(graph[i].x1), cii_unit(graph[i].y1), 
               cii_unit(graph[i].x2), 0, 0, 0, 0);
            d->setGraphicClass(graphic_class);
/*
            LG_Circle(0,inch_2_dbunitpoint2(cii_unit(graph[i].x1),cii_unit(graph[i].y1)),
                      0L,layer,get_widthlong(width), // widthcode 
                      inch_2_dbunit(cii_unit(graph[i].x2)));         
            flg = 0;
*/
         }
         break;
         case  4: /* start line */
         {
            int layernr = Graph_Level(layer,"",0);
            int closed = FALSE;  
            DataStruct *d = Graph_PolyStruct(layernr,0,0);
            d->setGraphicClass(graphic_class);

            if (graph[0].x1 == graph[graphcnt-1].x1 && graph[0].y1 == graph[graphcnt-1].y1)
            {
                closed = TRUE;
            }

            Graph_Poly(NULL,widthindex, fill, 0, closed);   // fillflag , negative, closed
            lastvertex = Graph_Vertex(cii_unit(graph[i].x1), cii_unit(graph[i].y1), 0);
         }
         break;
         case  5: /* box */
         {
            int layernr = Graph_Level(layer,"",0);
            DataStruct *d = Graph_PolyStruct(layernr,0,0);
            d->setGraphicClass(graphic_class);

            Graph_Poly(NULL,widthindex, fill, 0, 1);  // fillflag , negative, closed
            Graph_Vertex(cii_unit(graph[i].x1), cii_unit(graph[i].y1), 0);
            Graph_Vertex(cii_unit(graph[i].x2), cii_unit(graph[i].y1), 0);
            Graph_Vertex(cii_unit(graph[i].x2), cii_unit(graph[i].y2), 0);
            Graph_Vertex(cii_unit(graph[i].x1), cii_unit(graph[i].y2), 0);
            Graph_Vertex(cii_unit(graph[i].x1), cii_unit(graph[i].y1), 0);
         }
         break;
         default:
               fprintf(ferr,"I dont know graphtyp %d\n",graph[i].typ);
               display_error++;
         break;
      }
   }

   graphcnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int update_librarygeom()
{
   int   i;

   for (i=0;i<graphcnt;i++)
   {
      switch (graph[i].typ)
      {
         case  1: /* line */
             minmax(&minx,&maxx,graph[i].x1);
             minmax(&miny,&maxy,graph[i].y1);
         break;
         case  2: /* arc */
             minmax(&minx,&maxx,graph[i].x1);
             minmax(&miny,&maxy,graph[i].y1);
             minmax(&minx,&maxx,graph[i].x2);
             minmax(&miny,&maxy,graph[i].y2);
         break;
         case  3: /* circle */
             minmax(&minx,&maxx,graph[i].x1 - graph[i].x2/2);
             minmax(&miny,&maxy,graph[i].y1 - graph[i].x2/2);
             minmax(&minx,&maxx,graph[i].x1 + graph[i].x2/2);
             minmax(&miny,&maxy,graph[i].y1 + graph[i].x2/2);
         break;
         case  4: /* start line */
             minmax(&minx,&maxx,graph[i].x1);
             minmax(&miny,&maxy,graph[i].y1);
         break;
         case  5: /* box */
             minmax(&minx,&maxx,graph[i].x1);
             minmax(&miny,&maxy,graph[i].y1);
             minmax(&minx,&maxx,graph[i].x2);
             minmax(&miny,&maxy,graph[i].y2);
         break;
         default:
             fprintf(ferr,"I dont know graphtyp %d\n",graph[i].typ);
         break;
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
double cii_unit(int x)
{
   return faktor * x;
}

double cii_unit(long x)
{
   return faktor * x;
}

double cii_unit(double x)
{
   return faktor * x;
}

/****************************************************************************/
/*
long  w,h,d;   width,height,drill 
int   styp;    Shape type Comp,Edge,Onbo 
*/
static int get_pin514(long w,long h,long d,int styp)
{
   int   i;

   for (i=0;i<pin514cnt;i++)
   {
      if (pin514[i].w == w && pin514[i].h == h && 
          pin514[i].d == d && pin514[i].shape_type == styp)
         return(i);
   }

   if (pin514cnt < MAX_PIN514)
   {
      pin514[pin514cnt].w =w;
      pin514[pin514cnt].h =h;
      pin514[pin514cnt].d =d;
      pin514[pin514cnt].shape_type =styp;
      pin514cnt++;
   }
   else
   {
      fprintf(ferr,"Too many pin 514\n");
      return -1;
   }
   return(pin514cnt-1);
}

/****************************************************************************/
/*
*/
static int get_netname(char *n,int nodenr)
{
   if (nodenr == 0)
   {
      strcpy(n,"");
      return 0;         
   }

   if (nodenumberlist[nodenr])
   {
      strcpy(n,netnamearray[nodenumberlist[nodenr]-1]->name);
      return(0);
   }

/*
   for (i=0;i<netnamecnt;i++)
   {
      if (netname[i].id == nodenr)
      {
         strcpy(n,netname[i].name);
         return(0);
      }
   }
*/
   sprintf(n,"NET%d",nodenr);
   return(0);
}

/****************************************************************************/
/* 
   update global min max 
*/
static int minmax(long *mmin,long *mmax,long x)
{
   if (*mmin > x) *mmin = x;
  if (*mmax < x) *mmax = x;
  return(0);
}

/****************************************************************************/
/*
   bot is the bottom layer .
   if layer has bottom layer flag set then switch on all layers until num_of_layers
*/
static int extent_layer(Lm *layer,int bot)
{
   int   i;
   Lm    alllayers = 0;

   // if bottom layer is same as component layer, do not do anything.
   if (bot < 2)   return 0;

   for (i=0;i<bot;i++)
   {
      alllayers |= (1L << (32 - (i+1)));
   }

   // Check if layers should be extended.
   if (!ext_layer)   return 0;
   // if num_of_layer is 2 and bot is 2 everything is ok.
   if (num_of_layers == bot_layer)  return 0;

   // if an extended layer is present, it is the layer following the bottom
   // layer.
   int extended  = *layer & (1L << (32-(bot+1)));

   // now the bottom layer must be put to number of layers
   // the next from >1 <number of layers to all unused.

   // kill all above bot
   (*layer) = (*layer) & alllayers;

   // move bottom layers by adding the new and masking the old
   if (*layer & (1L << (32-(bot))))
   {
      *layer |= 1L << (32-(num_of_layers));
      *layer &= ~(1L << (32-(bot)));          // null out original layer
   }

   // here update inner layer
   if (extended)
   {
      for (i=1;i<num_of_layers-1;i++)
          *layer |= (1L << (32-(i+1)));
   }

   return(0);
}

/****************************************************************************/
/*
*/
static int get_libptr(char *shape)
{
   int   i;
   
   for (i=0;i<librarycnt;i++)
   {
      if (!strcmp(library[i].name,shape))
         return(i);
   }

   fprintf(ferr,"Library %s not found\n",shape);

   return -1;
}

/****************************************************************************/
/*
*/
void rotate(long *x,long *y,int rot)
{
   long  x1 = *x;
   long  y1 = *y;

   switch (rot)
   {
      case  90:
         *x = -y1;
         *y = x1;
      break;
      case  180:
         *x = -x1;
         *y = -y1;
      break;
      case  270:
         *x = y1;
         *y = -x1;
      break;
      default:
      break;
   }

   return;
}

/****************************************************************************/
/*
  unit GRP
*/
static int unit_GRP(FILE *ifp)
{
   int  len;
   int  err  = 0;
   long  i,num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
   num_rec = atol(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_582(ifp))!=0)
      {
         fprintf(ferr,"Error in do_582\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 582
*/
static int do_582(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  grp_outln,grp_hvo,grp_fvo,grp_tvo,grp_pvo,grp_dvo;
   int  grp_cnd,grp_vec,grp_finger,grp_shtab,grp_pins,grp_padst,grp_solder;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 582) r_error(atoi(token),582);
   if (!nextword())  p_error();
   
   switch (act_ciirev)
   {
      case 10:
      case  9:
      case  8:
      case  7:
      case  6:
         if ((len = atoi(token)) != 14) t_error(len,14);
      break;
      default:
         if ((len = atoi(token)) != 13) t_error(len,13);
      break;
   }
   if (!nextword())  p_error();
   if (!nextword())  p_error();
   grp_outln = atoi(token);
   if (!nextword())  p_error();
   grp_hvo = atoi(token);
   if (!nextword())  p_error();
   grp_fvo = atoi(token);
   if (!nextword())  p_error();
   grp_tvo = atoi(token);
   if (!nextword())  p_error();
   grp_pvo = atoi(token);
   if (!nextword())  p_error();
   grp_dvo = atoi(token);
   if (!nextword())  p_error();
   grp_cnd = atoi(token);
   if (!nextword())  p_error();
   grp_vec = atoi(token);
   if (!nextword())  p_error();
   grp_finger = atoi(token);
   if (!nextword())  p_error();
   grp_shtab = atoi(token);
   if (!nextword())  p_error();
   grp_pins = atoi(token);
   if (!nextword())  p_error();
   grp_padst = atoi(token);

   switch (act_ciirev)
   {
      case 10:
      case  9:
      case  8:
      case  7:
      case  6:
         if (!nextword())  p_error();
         grp_solder = atoi(token);
      break;
      default:
         grp_solder = 0;
      break;
   }

   for (i=0;i<grp_outln;i++)
   {
      if ((err = do_583(ifp))!=0)
      {
        fprintf(ferr,"Error in do_583\n");
         return -1;
      }
   }

   for (i=0;i<grp_hvo;i++)
   {
    if ((err = do_584(ifp))!=0)
      {
        fprintf(ferr,"Error in do_584\n");
         return -1;
      }
   }

   for (i=0;i<grp_fvo;i++)
   {
      if ((err = do_585(ifp))!=0)
      {
        fprintf(ferr,"Error in do_585\n");
         return -1;
      }
   }

   for (i=0;i<grp_tvo;i++)
   {
      if ((err = do_586(ifp))!=0)
      {
        fprintf(ferr,"Error in do_586\n");
         return -1;
      }
   }

   for (i=0;i<grp_pvo;i++)
   {
      if ((err = do_587(ifp))!=0)
      {
        fprintf(ferr,"Error in do_587\n");
         return -1;
      }
   }

   for (i=0;i<grp_dvo;i++)
   {
      if ((err = do_588(ifp))!=0)
      {
        fprintf(ferr,"Error in do_588\n");
         return -1;
      }
   }

   for (i=0;i<grp_cnd;i++)
   {
      if ((err = do_589(ifp))!=0)
      {
        fprintf(ferr,"Error in do_589\n");
         return -1;
      }
   }

   for (i=0;i<grp_vec;i++)
   {
      if ((err = do_590(ifp))!=0)
      {
        fprintf(ferr,"Error in do_590\n");
         return -1;
      }
   }

   for (i=0;i<grp_finger;i++)
   {
      if ((err = do_591(ifp))!=0)
      {
        fprintf(ferr,"Error in do_591\n");
         return -1;
      }
   }

   for (i=0;i<grp_shtab;i++)
   {
      if ((err = do_592(ifp))!=0)
      {
        fprintf(ferr,"Error in do_592\n");
         return -1;
      }
   }

   for (i=0;i<grp_pins;i++)
   {
      if ((err = do_593(ifp))!=0)
      {
        fprintf(ferr,"Error in do_593\n");
         return -1;
      }
   }

   for (i=0;i<grp_padst;i++)
   {
      if ((err = do_594(ifp))!=0)
      {
        fprintf(ferr,"Error in do_594\n");
         return -1;
      }
   }

   for (i=0;i<grp_solder;i++)
   {
      if ((err = do_595(ifp))!=0)
      {
        fprintf(ferr,"Error in do_595\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 583
*/
static int do_583(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 583) r_error(atoi(token),583);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec583.i2ptr_grp2nsh = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 584
*/
static int do_584(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 584) r_error(atoi(token),584);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec584.i2ptr_grp2hvo = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 585
*/
static int do_585(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 585) r_error(atoi(token),585);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec585.i2ptr_grp2fvo = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 586
*/
static int do_586(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 586) r_error(atoi(token),586);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec586.i2ptr_grpTtvo = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 587
*/
static int do_587(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 587) r_error(atoi(token),587);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec587.i2ptr_grp2pvo = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 588
*/
static int do_588(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 588) r_error(atoi(token),588);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec588.i2ptr_grp2dvo = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 589
*/
static int do_589(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 589) r_error(atoi(token),589);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec589.i2ptr_grp2cnd = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 590
*/
static int do_590(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 590) r_error(atoi(token),590);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
   /* rec.u.rec590.iptr_grp2vec = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 591
*/
static int do_591(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 591) r_error(atoi(token),591);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 2) t_error(len,2);

  if (!nextword())  p_error();
  /* rec.u.rec591.iptr_grp2vec2 = atoi(token); */

  return(err);
}

/****************************************************************************/
/*
  do 592
*/
static int do_592(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 592) r_error(atoi(token),592);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 2) t_error(len,2);

  if (!nextword())  p_error();
  /* rec.u.rec592.iptr_grp2vec3 = atoi(token);  */

  return(err);
}

/****************************************************************************/
/*
  do 593
*/
static int do_593(FILE *ifp)
{
  int  len;
  int  err  = 0;

  if (!nextline(ifp))  l_error();

  if (!nextword())  p_error();
  if (atoi(token) != 593) r_error(atoi(token),593);
  if (!nextword())  p_error();
  if ((len = atoi(token)) != 1) t_error(len,1);

  if (!nextword())  p_error();
  /* rec.u.rec593.i2cmp_pinnum = atoi(token); */

  return(err);
}

/****************************************************************************/
/*
  do 594
*/
static int do_594(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 594) r_error(atoi(token),594);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();
   /* rec.u.rec594.iptr_grp2pso = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  do 595
*/
static int do_595(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 595) r_error(atoi(token),595);
   if (!nextword())  p_error();

   switch (act_ciirev)
   {
      case 10:
      case  9:
      case  8:
      case  7:
        if ((len = atoi(token)) != 1) t_error(len,1);
      break;
      default:
        if ((len = atoi(token)) != 2) t_error(len,2);
      break;
   }

   if (!nextword())  p_error();

   return(err);
}

/****************************************************************************/
/*
  unit TVO
*/
static int unit_TVO(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_602(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_602\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 602
*/
static int do_602(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 602) r_error(atoi(token),602);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 7) t_error(len,7);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec602.itvo_laymask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec602.iptr_tvo2cmpvgrp = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec602.itvo_diam = atol(token); */
   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_604(ifp))!=0)
      {
         fprintf(ferr,"Error in do_604\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 604
*/
static int do_604(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 604) r_error(atoi(token),604);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   /* rec.u.rec604.itvo_x = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec604.itvo_y = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec604.i2tvo_arcflg = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  unit PVO
*/
static int unit_PVO(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_622(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_622\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 622
*/
static int do_622(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 622) r_error(atoi(token),622);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 7) t_error(len,7);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec622.ipvo_mask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec622.iptr_pvo2cmpvgrp = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec622.ipvo_diam = atol(token); */
   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_624(ifp))!=0)
      {
         fprintf(ferr,"Error in do_624\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 624
*/
static int do_624(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 624) r_error(atoi(token),624);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   /* rec.u.rec624.ipvo_x = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec624.ipvo_y = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec624.i2pvo_arcflg = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  unit DVO
*/
static int unit_DVO(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_642(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_642\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 642
*/
static int do_642(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 642) r_error(atoi(token),642);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 9) t_error(len,9);

   if (!nextword())  p_error();
   /* layermask(&rec.u.rec642.idvo_mask,token); */
   if (!nextword())  p_error();
   /* rec.u.rec642.iptr_dvo2cmpvgrp = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec642.idvo_diam = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec642.isize_delta = atol(token); */
   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_644(ifp))!=0)
      {
         fprintf(ferr,"Error in do_644\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 644
*/
static int do_644(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 644) r_error(atoi(token),644);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   /* rec.u.rec644.idvo_x = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec644.idvo_y = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec644.i2dvo_arcflg = atoi(token); */

   return(err);
}

/****************************************************************************/
/*
  unit ARF
*/
static int unit_ARF(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_662(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_662\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 662
*/
static int do_662(FILE *ifp)
{
   int  len;
   int  err  = 0;
   long  num_rec,i,w, num_out = 0, num_voids = 0;
   int   layernr,nodenr;
   char  netname[33];
   long  x1,y1,x2,y2;
   char  lname[80];

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 662) r_error(atoi(token),662);
   if (!nextword())  p_error();
   switch (act_ciirev)
   {
      case 10:
         if ((len = atoi(token)) != 29) t_error(len,29);
      break;
      case  9:
      case  8:
         if ((len = atoi(token)) != 24) t_error(len,24);
      break;
      default:
         if ((len = atoi(token)) != 16) t_error(len,16);
      break;
   }

   if (!nextword())  p_error();
   layernr = atoi(token); // this is the layer name not the Layer number
   sprintf(lname,"%d",layernr);
   layernr = Graph_Level(lname, "", 0);
   if (!nextword())  p_error();
   nodenr = atoi(token);
   if (!nextword())  p_error();
   x1 = on_grid(atol(token));
   if (!nextword())  p_error();
   y1 = on_grid(atol(token));
   if (!nextword())  p_error();
   x2 = on_grid(atol(token));
   if (!nextword())  p_error();
   y2 = on_grid(atol(token));
   if (!nextword())  p_error();
   w = atol(token);
   if (!nextword())  p_error();
   num_rec= atol(token);
   if (!nextword())  p_error();
  /* rec.u.rec662.ispace_fillvec = atol(token); */

  if (act_ciirev > 7)
  {
      if (!nextword())  p_error(); // Spacing of fill vectors
      if (!nextword())  p_error(); // Linewidth of contour vectors
      if (!nextword())  p_error(); // Linewidth of fill vectors
      if (!nextword())  p_error(); // Pattern
      if (!nextword())  p_error(); // Remove island
      if (!nextword())  p_error(); // number of outlines 666
      num_out = atol(token);
   }
   if (act_ciirev > 9)
   {
      if (!nextword())  p_error(); // areafill/line clearance
      if (!nextword())  p_error(); // areafill/pad clearance
      if (!nextword())  p_error(); // number of fillvoids 668
      num_voids = atol(token);
   }
                  
   if (num_rec)   // these are the fill lines, num_out are the outlines, which are not implemented.
   {
      get_netname(netname,nodenr);
      //SCLG_Netlist(netname);

      DataStruct *d = Graph_PolyStruct(layernr,0,0);
      if (strlen(netname))
      {
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, (void *)netname, SA_APPEND, NULL); //
      }
   }

   // these are the hatchlines
   for (i=0;i<num_rec;i++)
   {
      if ((err = do_664(ifp, w))!=0)
      {
        fprintf(ferr,"Error in do_664\n");
         return -1;
      }
   }

   // these are the fills
   for (i=0;i<num_out;i++)
   {
      graphcnt = 0;
      if ((err = do_666(ifp,w))!=0)
      {
         fprintf(ferr,"Error in do_666\n");
         return -1;
      }
      // SCLG_Fill ???
      int   grclass = 0;
      write_graph(lname, 0, 0, TRUE);
      graphcnt = 0;
   }

   // these are the voids 
   for (i=0;i<num_voids;i++)
   {
      graphcnt = 0;
      if ((err = do_668(ifp,w))!=0)
      {
         fprintf(ferr,"Error in do_668\n");
         return -1;
      }
      // SCLG_Fill ???
      // write_graph(layer);
      graphcnt = 0;
   }

   return(err);
}

/****************************************************************************/
/*
  do 664
*/
static int do_664(FILE *ifp, long w)
{
   int   len;
   int   err  = 0;
   long  x1,y1,x2,y2;
   int   widthindex;

   if (w == 0)
      widthindex = 0;
   else
      widthindex = Graph_Aperture("", T_ROUND, cii_unit(w), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);


   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 664) r_error(atoi(token),664);
   if (!nextword())  p_error();

   switch (act_ciirev)
   {
      case 10:
      case 9:
      case 8:
         if ((len = atoi(token)) != 13) t_error(len,13);
      break;
      default:
         if ((len = atoi(token)) != 8) t_error(len,8);
      break;
   }

   if (!nextword())  p_error();
   x1 = on_grid(atol(token)); 
   if (!nextword())  p_error();
   y1 = on_grid(atol(token));
   if (!nextword())  p_error();
   x2 = on_grid(atol(token));
   if (!nextword())  p_error();
   y2 = on_grid(atol(token));

   // in version 8 is Arcflag
   // in version 8 is ???
   // in version 8 is ???

   CPoly *cp = Graph_Poly(NULL,widthindex, FALSE, 0, FALSE);   // fillflag , negative, closed
   cp->setHatchLine(true);
   Graph_Vertex(cii_unit(x1), cii_unit(y1), 0);
   Graph_Vertex(cii_unit(x2), cii_unit(y2), 0);

   return(err);
}

/****************************************************************************/
/*
  do 666
*/
static int do_666(FILE *ifp, long w)
{
   int  len;
   int  err  = 0;
   int    typ;
   long x1,y1,x2,y2;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 666) r_error(atoi(token),666);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 9) t_error(len,9);

   if (!nextword())  p_error();
   typ = atoi(token); 
   if (!nextword())  p_error();
   x1 = atol(token); 
   if (!nextword())  p_error();            
   y1 = atol(token); 
   if (!nextword())  p_error();
   x2 = atol(token); 
   if (!nextword())  p_error();
   y2 = atol(token); 

   if (graphcnt < MAX_GRAPH)
   {
      graph[graphcnt].typ = typ;
      graph[graphcnt].x1  = x1;
      graph[graphcnt].y1  = y1;
      graph[graphcnt].x2  = x2;
      graph[graphcnt].y2  = y2;
      graphcnt++;
   }
   else
   {
      fprintf(ferr,"Too many graphic elements\n");
      display_error++;
      return -1;
   }

   return(err);
}

/****************************************************************************/
/*
  do 668
*/
static int do_668(FILE *ifp, long w)
{
   int  len;
   int  err  = 0;
// int    typ;
// long x1,y1,x2,y2;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 668) r_error(atoi(token),668);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 2) t_error(len,2);

   if (!nextword())  p_error();  // fill void number

   return(err);
}

/****************************************************************************/
/*
  unit PRT
*/
static int unit_PRT(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_682(ifp)) != 0)
      {
         fprintf(ferr,"Error in do_682\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 682
*/
static int do_682(FILE *ifp)
{
   int  len;
   int  err  = 0;
   int  pin,des,func,sig,i;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 682) r_error(atoi(token),682);
   if (!nextword())  p_error();

   switch (act_ciirev)
   {
      case 10:
      case 9:
         if ((len = atoi(token)) != 40) t_error(len,40);
      break;
      default:
         if ((len = atoi(token)) != 37) t_error(len,37);
      break;
   }

   if (!numword(32,0))  p_error();
   /* strncpy(&rec.u.rec682.aprt_name[0],&token[0],sizeof(rec.u.rec682.aprt_name)); */
   if (!nextword())  p_error();
   /* rec.u.rec682.i2prt_typeflg = atoi(token); */
   if (!numword(32,0))  p_error();
   /* strncpy(&rec.u.rec682.aprt_shpname[0],&token[0],sizeof(rec.u.rec682.aprt_shpname)); */
   if (!nextword())  p_error();
   pin = atoi(token);
   if (!nextword())  p_error();
   des = atoi(token);
   if (!nextword())  p_error();
   func = atoi(token);
   if (!nextword())  p_error();
   sig = atoi(token);

   for (i=0;i<des;i++)
   {
      if ((err = do_684(ifp))!=0)
      {
         fprintf(ferr,"Error in do_684\n");
         return -1;
      }
   }

   for (i=0;i<func;i++)
   {
      if ((err = do_686(ifp))!=0)
      {
         fprintf(ferr,"Error in do_686\n");
         return -1;
      }
   }
   
   for (i=0;i<sig;i++)
   {
      if ((err = do_690(ifp))!=0)
      {
         fprintf(ferr,"Error in do_690\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 684
*/
static int do_684(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 684) r_error(atoi(token),684);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   /* rec.u.rec684.i2ptr_prt2cmp = atoi(token); */
  
   return(err);
}

/****************************************************************************/
/*
  do 686
*/
static int do_686(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   int  num_rec, rec_691 = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 686) r_error(atoi(token),686);
   if (!nextword())  p_error();


   switch (act_ciirev)
   {
      case 10:
      case 9:
         if ((len = atoi(token)) != 37) t_error(len,37);
      break;
      default:
         if ((len = atoi(token)) != 34) t_error(len,34);
      break;
   }

   if (!numword(32,0))  p_error();
   /* strncpy(&rec.u.rec686.aslot_name[0],&token[0],sizeof(rec.u.rec686.aslot_name)); */
   if (!numword(32,0))  p_error();
   /* strncpy(&rec.u.rec686.aprt_functyp[0],&token[0],sizeof(rec.u.rec686.aprt_functyp)); */
   if (!nextword())  p_error();
   /* rec.u.rec686.i2units_flg = atoi(token); */
   if (!nextword())  p_error();
   num_rec = atoi(token);

   if (act_ciirev > 8)
   {
      if (!nextword())  p_error();  // 691
      rec_691 = atoi(token);
   }


   for (i=0;i<num_rec;i++)
   {
      if ((err = do_688(ifp))!=0)
      {
         fprintf(ferr,"Error  in do_688\n");
         return -1;
      }
   }

   for (i=0;i<rec_691;i++)
   {
      if ((err = do_691(ifp))!=0)
      {
         fprintf(ferr,"Error  in do_691\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 688
*/
static int do_688(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 688) r_error(atoi(token),688);
   if (!nextword())  p_error();
   switch (act_ciirev)
   {
      case 10: 
      case  9:
         if ((len = atoi(token)) != 40) t_error(len,40);
      break;
      case  8:
      case  7:
         if ((len = atoi(token)) != 38) t_error(len,38);
      break;
      case  6:
         if ((len = atoi(token)) != 22) t_error(len,22);
      break;
      default:
         if ((len = atoi(token)) != 6) t_error(len,6);
      break;
   }

   if (!nextword())  p_error();
   /* rec.u.rec688.i2prt_pinnum = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec688.i2func_usage = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec688.ihi_value = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec688.ilo_value = atol(token); */

   switch (act_ciirev)
   {
      case 10:
      case  9:
      case  8:
      case  7:
        if (!numword(16,0))  p_error();  /* netname */
      break;
   }
   return(err);
}

/****************************************************************************/
/*
  do 691
*/
static int do_691(FILE *ifp)
{
   int  len;
   int  err  = 0;
   int  rec_693;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 691) r_error(atoi(token),691);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   rec_693 =  atoi(token); 

   for (int i = 0;i < rec_693; i++)
   {
      if (!nextline(ifp))  l_error();
   }

   return(err);
}

/****************************************************************************/
/*
  do 690
*/
static int do_690(FILE *ifp)
{
   int  len,i;
   int  err  = 0;
   long  num_rec;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 690) r_error(atoi(token),690);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 19) t_error(len,19);

   if (!nextword())  p_error();
   /* rec.u.rec690.i2prt_sigflg = atoi(token); */
   if (!numword(32,0))  p_error();
   /* strncpy(&rec.u.rec690.asig_name[0],&token[0],sizeof(rec.u.rec690.asig_name)); */
   if (!nextword())  p_error();
   /* rec.u.rec690.i2sig_flg = atoi(token); */
   if (!nextword())  p_error();
   num_rec = atol(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_692(ifp))!=0)
      {
         fprintf(ferr,"Error in do_692\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 692
*/
static int do_692(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 692) r_error(atoi(token),692);
   if (!nextword())  p_error();

   switch (act_ciirev)
   {
      case 10:
      case  9:
         if ((len = atoi(token)) != 23) t_error(len,22);
      break;
      case  8:
      case  7:
         if ((len = atoi(token)) != 22) t_error(len,22);
      break;
      default:
         if ((len = atoi(token)) != 6) t_error(len,6);
      break;
   }

   if (!nextword())  p_error();
   /* rec.u.rec692.i2sig_pinum = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec692.i2sig_usage = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec692.isig_hival = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec692.isig_loval = atol(token); */

   if (!numword(22-6,0))  p_error(); /* need cii 7 docu */

   if (act_ciirev > 8)
   {
      if (!nextword())  p_error();
   }

   return(err);
}

/****************************************************************************/
/*
  unit DFT
*/
static int unit_DFT(FILE *ifp)
{
   int  i,len;
   int  err  = 0;
   int num_rec;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 1) t_error(len,1);

   if (!nextword())  p_error();
   num_rec = atoi(token);

   for (i=0;i<num_rec;i++)
   {
      if ((err = do_702(ifp,i))!=0)
      {
        fprintf(ferr,"Error in do_702\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  unit SET
*/
static int unit_SET(FILE *ifp)
{
  int  len;
  int  err  = 0;
  int  num_rec;

  if (!nextword())  p_error();
  if ((len = atoi(token)) != 1) t_error(len,1);

  if (!nextword())  p_error();
  num_rec = atoi(token);

  if (num_rec)
  {
      fprintf(ferr,"Setup records not impl.\n");
      display_error++;
      return -1;
  }

  return(err);
}

/****************************************************************************/
/*
  do 702
*/
static int do_702(FILE *ifp,int dd)
{
   int  i,len;
   int  err  = 0;
   int   num_of_text,num_of_draw;
   char  layer[32+1];


   // SCLG_Netlist("DesignDataLayerUnit");
   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 702) r_error(atoi(token),702);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 20) t_error(len,20);

   if (!numword(32,0))  p_error();
   strcpy(layer,token); 

   if (!nextword())  p_error();
   num_of_text = atoi(token);
   if (!nextword())  p_error();
   num_of_draw = atoi(token);

   for (i=0;i<num_of_text;i++)
   {
      if ((err = do_704(ifp,layer))!=0)
      {
        fprintf(ferr,"Error in do_704\n");
         return -1;
      }
   }

   for (i=0;i<num_of_draw;i++)
   {
      if ((err = do_708(ifp,layer))!=0)
      {
        fprintf(ferr,"Error in do_708\n");
         return -1;
      }
   }
   return(err);
}

/****************************************************************************/
/*
  do 704
*/
static int do_704(FILE *ifp,char *layer)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 704) r_error(atoi(token),704);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 7) t_error(len,7);

   if (!nextword())  p_error();
   /* rec.u.rec704.rot = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec704.height = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec704.xref = atol(token); */
   if (!nextword())  p_error();
   /* rec.u.rec704.yref = atol(token); */

   if ((err = do_706(ifp))!=0)
   {
      fprintf(ferr,"Error in do_706\n");
      return -1;
   }

   return(err);
}

/****************************************************************************/
/*
  do 706
*/
static int do_706(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 706) r_error(atoi(token),706);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 32) t_error(len,32);

   if (!numword(64,0))  p_error();
   /* strncpy(&rec.u.rec706.prosa[0],&token[0],sizeof(rec.u.rec706.prosa)); */

   return(err);
}

/****************************************************************************/
/*
  do 708
*/
static int do_708(FILE *ifp,char *lay)
{
   int   i,len,v;
   int   err  = 0;
   int   num_of_geom;
   long  width;
   int   typ;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 708) r_error(atoi(token),708);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 4) t_error(len,4);
   
   if (!nextword())  p_error();
   typ = atoi(token); 
   if (!nextword())  p_error();
   width = atol(token); 
   if (!nextword())  p_error();
   num_of_geom = atoi(token);

   for (i=0;i<num_of_geom;i++)
   {
     if ((err = do_710(ifp,i))!=0)
      {
       fprintf(ferr,"Error in do_710\n");
       return -1;
      }
   }

   int fill, closed;

   /* here do it */
   switch (typ)
   {
      case  2:
         fill = TRUE;
         closed = TRUE;
      break;
      default:
         fill = FALSE;
         closed = FALSE;
      break;
   }

   int layernr = Graph_Level(lay,"",0);
   CPnt     *lastvertex = NULL;

   int widthindex = 0;
   DataStruct *d = Graph_PolyStruct(layernr,0,0);
   Graph_Poly(NULL,widthindex, fill, 0, closed);   // fillflag , negative, closed

   for (v=1;v<num_of_geom;v++)
   {
      switch (vector[v].arc)
      {
         case  1: // line

            if (lastvertex == NULL)
            {
               Graph_Vertex(cii_unit(vector[v-1].x), cii_unit(vector[v-1].y), 0);
            }
            lastvertex = Graph_Vertex(cii_unit(vector[v].x), cii_unit(vector[v].y), 0);
         break;
         case 2: // arc point
         {
            double cx, cy, sa, da, rad;

            if (lastvertex) // NULL if arcpoint is the first
            {
               ArcPoint3(cii_unit(vector[v-1].x),cii_unit(vector[v-1].y),
                   cii_unit(vector[v+1].x),cii_unit(vector[v+1].y),
                   cii_unit(vector[v].x),cii_unit(vector[v].y),
                   &cx,&cy,&rad,&sa,&da);
               lastvertex->bulge = (DbUnit)tan(da/4);
            }
            lastvertex = Graph_Vertex( cii_unit(vector[v].x), cii_unit(vector[v].y), 0.0);
         }
         break;
         case 3: // do nothing. wait for radius
            fprintf(ferr,"Circle Center not impl. in 710 near %ld\n", linecnt);
            display_error++;
         break;
         case 4: // Radius. The preciding record should be 3 Circle center
            if (vector[v-1].arc == 3)
            {
               lastvertex = Graph_Vertex( cii_unit(vector[v-1].x) + cii_unit(vector[v].x), cii_unit(vector[v-1].y), 1.0);
               lastvertex = Graph_Vertex( cii_unit(vector[v-1].x) - cii_unit(vector[v].x), cii_unit(vector[v-1].y), 1.0);
               lastvertex = Graph_Vertex( cii_unit(vector[v-1].x) + cii_unit(vector[v].x), cii_unit(vector[v-1].y), 0.0);
               lastvertex = NULL;
            }
            else
            {
               fprintf(ferr,"Circle Center before Radius expected in 710 near %ld\n", linecnt);
               display_error++;
            }
         break;
         default:
            fprintf(ferr,"Entity Type [%d] not impl. in 710 near %ld\n",vector[v].arc,linecnt);
            display_error++;
         break;
      }
   }

   if (typ == 2)
   {
      Graph_Vertex(cii_unit(vector[0].x), cii_unit(vector[0].y), 0);
   }

   return(err);
}

/****************************************************************************/
/*
  do 710
*/
static int do_710(FILE *ifp,int vcnt)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 710) r_error(atoi(token),710);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 5) t_error(len,5);

   if (!nextword())  p_error();
   vector[vcnt].arc = atoi(token); 
   if (!nextword())  p_error();
   vector[vcnt].x = atol(token); 
   if (!nextword())  p_error();
   vector[vcnt].y = atol(token); 

   return(err);
}

/****************************************************************************/
/*
  unit RULE
*/
static int unit_RULE(FILE *ifp)
{
   int  i,len;
   int  err  = 0;
   int  num_pad,num_hole,num_line;

   if (!nextword())  p_error();
   if ((len = atoi(token)) != 3) t_error(len,3);

   if (!nextword())  p_error();
   num_pad = atoi(token);
   if (!nextword())  p_error();
   num_hole = atoi(token);
   if (!nextword())  p_error();
   num_line = atoi(token);

   for (i=0;i<num_pad;i++)
   {
      if ((err = do_742(ifp))!=0)
      {
        fprintf(ferr,"Error in do_742\n");
         return -1;
      }
   }

   for (i=0;i<num_hole;i++)
   {
      if ((err = do_746(ifp))!=0)
      {
        fprintf(ferr,"Error in do_746\n");
         return -1;
      }
   }

   for (i=0;i<num_line;i++)
   {
      if ((err = do_748(ifp))!=0)
      {
        fprintf(ferr,"Error in do_748\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 742
*/
static int do_742(FILE *ifp)
{
   int  i,len,num_of_comment;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 742) r_error(atoi(token),742);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 3) t_error(len,3);

   if (!nextword())  p_error();
   /* rec.u.rec742.pad_type_number = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec742.ptr_padgeom_unit = atoi(token); */
   if (!nextword())  p_error();
   num_of_comment = atoi(token);

   for (i=0;i<num_of_comment;i++)
   {
      if ((err = do_744(ifp))!=0)
      {
        fprintf(ferr,"Error in do_744\n");
         return -1;
      }
   }

   return(err);
}

/****************************************************************************/
/*
  do 744
*/
static int do_744(FILE *ifp)
{
   int  len;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 744) r_error(atoi(token),744);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 16) t_error(len,16);

   if (!numword(32,0))  p_error();
   /* strncpy(&rec.u.rec744.comment[0],&token[0],sizeof(rec.u.rec744.comment)); */

   return(err);
}

/****************************************************************************/
/*
  do 746
*/
static int do_746(FILE *ifp)
{
   int  i,len,num_of_comment;
   int  err  = 0;

   if (!nextline(ifp))  l_error();
   
   if (!nextword())  p_error();
   if (atoi(token) != 746) r_error(atoi(token),746);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 4) t_error(len,4);

   if (!nextword())  p_error();
   /* rec.u.rec746.hole_type_number = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec746.hole_diameter = atol(token); */
   if (!nextword())  p_error();
   num_of_comment = atoi(token);

   for (i=0;i<num_of_comment;i++)
      if ((err = do_744(ifp))!=0)
      {
         fprintf(ferr,"Error in do_744\n");
         return -1;
      }

  return(err);
}

/****************************************************************************/
/*
  do 748
*/
static int do_748(FILE *ifp)
{
   int  i,len,num_of_comment;
   int  err  = 0;

   if (!nextline(ifp))  l_error();

   if (!nextword())  p_error();
   if (atoi(token) != 748) r_error(atoi(token),748);
   if (!nextword())  p_error();
   if ((len = atoi(token)) != 4) t_error(len,4);

   if (!nextword())  p_error();
   /* rec.u.rec748.line_type_number = atoi(token); */
   if (!nextword())  p_error();
   /* rec.u.rec748.line_width = atol(token); */
   if (!nextword())  p_error();
   num_of_comment = atoi(token);

   for (i=0;i<num_of_comment;i++)
      if ((err = do_744(ifp))!=0)
      {
         fprintf(ferr,"Error in do_744\n");
         return -1;
      }

   return(err);
}

/****************************************************************************/
/*
*/
static int check_component_placement(CCEtoODBDoc *doc, FileStruct *file)
{  
   int       i;
   NetStruct *net;
   POSITION  netPos;

   generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

   for (i=0;i<compcnt;i++)
   {
      CIICompname *c = comparray[i];

      int         pin1found = FALSE;
      double      pin1x, pin1y;
      CString     pin1;

      pin1x = cii_unit(comparray[i]->pin1x);
      pin1y = cii_unit(comparray[i]->pin1y);
      pin1 = comparray[i]->pin1;

      // get pinloc from netlist
      // get pinloc from comparray

      netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         net = file->getNetList().GetNext(netPos);

         CompPinStruct *compPin;
         POSITION compPinPos;
   
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            compPin = net->getNextCompPin(compPinPos);
            if (compPin->getRefDes().Compare(comparray[i]->name) == 0)
            {
               if (compPin->getPinName().Compare(pin1) == 0)
               {
                  // pin1 found
                  pin1found = TRUE;
                  if (fabs(compPin->getOriginX() - pin1x) > cii_unit(1)  || fabs(compPin->getOriginY() - pin1y) > cii_unit(1))
                  {
                     // pin offset found
                     double dx, dy;
                     dx = compPin->getOriginX() - pin1x;
                     dy = compPin->getOriginY() - pin1y;
                     DataStruct *d = datastruct_from_refdes(doc, file->getBlock(), comparray[i]->name);
                     d->getInsert()->incOriginX(-dx);
                     d->getInsert()->incOriginY(-dy);
					 // bad, bad mojo, need to correct refname offsets now.
					 if (d->getAttributes() != NULL)
					 {
						if ((fabs(dx) > SMALLNUMBER) || (fabs(dy) > SMALLNUMBER))
						{
							WORD keyWord = doc->RegisterKeyWord(ATT_REFNAME, 0, VT_STRING);
							Attrib* attrib = NULL;
							if (d->getAttributes()->Lookup(keyWord, attrib) && attrib != NULL)
							{
                                double x1, y1, x2, y2;
								x1 = attrib->getX();
								y1 = attrib->getY();
								Rotate(dx,dy,360.0-d->getInsert()->getAngleDegrees(),&x2,&y2);
								attrib->setX(x1+x2);
								attrib->setY(y1+y2);
							}
						}
					 }
                  }
                  break;
               }
            }
         }
         if (pin1found) break;
      }

      if (!pin1found)
      {
         // big problem
         fprintf(ferr, "Component [%s] Pin [%s] could not be found in Pin Check function\n", 
            comparray[i]->name, pin1);
         display_error++;
      }
   }

   generate_PINLOC(doc,file,1);  // force the correction

   return 1;
}


/******************************************************************************
* ReadScicards
*/
void ReadScicards(const char *fullPath, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   // Scicards Encore
   // Apparantly related to Harris in some way as well.

   int   i;
   int   err = 0;
   FILE  *ifp;

   last_unit = 0;

   if ((planenet = (CIIPlanenet *)calloc(MAX_PLANENET, sizeof(CIIPlanenet))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   planenetcnt = 0;

   if ((widthcode = (long *)calloc(MAX_WIDTH, sizeof(long))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   widthcodecnt = 0;

   doc = Doc;
   file = NULL;
   display_error = 0;
   PageUnits = pageunits;

   testpointcnt = 0;

   // CII has 1/10 of mil
   faktor = Units_Factor(UNIT_MILS, PageUnits) / 10;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(fullPath,"rt")) == NULL)
   {
      ErrorMessage("Error opening file for reading.", fullPath);
      return;
   }

   CString logFile;
   ferr = getApp().OpenOperationLogFile(CIIERR, logFile);
   if (ferr == NULL) // error message already issued, just return.
      return;

   log_set_filename(fullPath);

   char  drive[_MAX_DRIVE];
   char  dir[_MAX_DIR];
   char  fname[_MAX_FNAME];
   char  ext[_MAX_EXT];
   _splitpath( fullPath, drive, dir, fname, ext );
   CString filename(fname);
   filename += ext;

   file = Graph_File_Start(filename, Type_CII_Layout);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());

   init_cii();       // first set inits, then update from .tec file.

   // settings must be loaded after fileinit.
   CString settingsFile = getApp().getImportSettingsFilePath("cii.in");
   {
      CString msg;
      msg.Format("\nSCICARDS: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_ciisettings(settingsFile);

   /* here load only library */
   /* dont forget to start new unit for all new units */
   linecnt = 0;   // the increment is immediately after fgets
   act_ciirev = 0;
 
   while (nextline(ifp))
   {
      if (nextword() != 0)
      if ((err = process_record(ifp)) != 0)
         return;
   }

   write_complexpin_graphic();
   write_padstack514();
   write_padstack400();
   write_oppositepadstack400();

   int   j;

   // here do all layers, even tha not all layers may be routed.
   for (j=1;j<= num_of_layers; j++)
   {
      CString  lname;
      lname.Format("%d", j);
      int lay = Graph_Level(lname,"",0);
      LayerStruct *l = doc->FindLayer(lay);
      if (j == 1)
         lname = "TOP SIDE";
      else
      if (j == num_of_layers)
         lname = "BOTTOM SIDE";
      else
         lname.Format("INNER %d", j-1);
      l->setComment(lname);
   }


   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      LayerStruct *l = doc->getLayerArray()[j];
      if (l == NULL) continue; // could have been deleted.

      CString  lname, pname;
      lname = l->getName();

      if (atoi(lname) == 1)
      {
         l->setLayerType(LAYTYPE_SIGNAL_TOP);
         l->setElectricalStackNumber(1);
      }
      else
      if (atoi(lname) == num_of_layers)
      {
         l->setLayerType(LAYTYPE_SIGNAL_BOT);
         l->setElectricalStackNumber(num_of_layers);
      }
      else
      if (atoi(lname) > 1 && atoi(lname) < num_of_layers)
      {
         if (l->getLayerType() == 0)  // could have been set to a planelayer with record 262.
            l->setLayerType(LAYTYPE_SIGNAL_INNER);
         l->setElectricalStackNumber(atoi(lname));
      }
      else
      if (lname.CompareNoCase(OLDSHAPEOUTLINETOP) == 0)
      {
         Graph_Level_Mirror(OLDSHAPEOUTLINETOP, OLDSHAPEOUTLINEBOT, "");
      }
      else
      if (lname.CompareNoCase(ALLLAYERS) == 0)
      {
         l->setLayerType(LAYTYPE_SIGNAL_ALL);
      }
      else
      if (lname.CompareNoCase(PADTOP) == 0)
      {
         l->setLayerType(LAYTYPE_PAD_TOP);
         l->setElectricalStackNumber(1);
         Graph_Level_Mirror(PADTOP, PADBOT, "");
      }
      else
      if (lname.CompareNoCase(PADBOT) == 0)
      {
         l->setLayerType(LAYTYPE_PAD_BOTTOM);
         l->setElectricalStackNumber(num_of_layers);
         Graph_Level_Mirror(PADTOP, PADBOT, "");
      }
      else
      if (lname.CompareNoCase(PADINNER) == 0)
      {
         l->setLayerType(LAYTYPE_PAD_INNER);
      }
      else
      if (lname.CompareNoCase(PADALL) == 0)
      {
         l->setLayerType(LAYTYPE_PAD_ALL);
      }
      else
      if (lname.CompareNoCase("SOLDERRESIST_TOP") == 0)
      {
         l->setLayerType(LAYTYPE_MASK_TOP);
         Graph_Level_Mirror("SOLDERRESIST_TOP", "SOLDERRESIST_BOT", "");
      }
      else
      if (lname.CompareNoCase("SOLDERRESIST_BOT") == 0)
      {
         l->setLayerType(LAYTYPE_MASK_BOTTOM);
         Graph_Level_Mirror("SOLDERRESIST_TOP", "SOLDERRESIST_BOT", "");
      }
      else
      if (lname.CompareNoCase("BOARDOUTLINE") == 0)
      {
         l->setLayerType(LAYTYPE_BOARD_OUTLINE);
      }
      else
      if (lname.CompareNoCase("LIB_1") == 0)
      {
         l->setLayerType(LAYTYPE_SILK_TOP);
         CString lname;
         lname.Format("LIB_%d",num_of_layers);
         int lay = Graph_Level(lname,"",0);
         l = doc->FindLayer(lay);
         l->setLayerType(LAYTYPE_SILK_BOTTOM);
         Graph_Level_Mirror("LIB_1", lname, "");
      }
   }


   // do the PAD layers here.
   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      LayerStruct *l = doc->getLayerArray()[j];
      if (l == NULL) continue; // could have been deleted.

      CString  lname, pname;
      lname = l->getName();

      for (i=2;i<=num_of_layers-1;i++)
      {
         pname.Format("PAD%d",i); 

         if (pname.CompareNoCase(lname) == 0)
         {
            CString  llname;
            llname.Format("%d", i);
            int lay = Graph_Level(llname,"",0);
            LayerStruct *ll = doc->FindLayer(lay);
            
            if (ll->getLayerType() == LAYTYPE_POWERNEG)
               l->setLayerType(LAYTYPE_PAD_THERMAL);
            else
            if (ll->getLayerType() == LAYTYPE_SPLITPLANE)
               l->setLayerType(LAYTYPE_PAD_THERMAL);
            else
               l->setLayerType(LAYTYPE_PAD_INNER);

            l->setElectricalStackNumber(i);
            break;
         }
      }
   }

   // must be after layer typing.
   update_smdpads(doc);
   RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.

   update_smdrule_geometries(doc,ComponentSMDrule);
   update_smdrule_components(doc, file, ComponentSMDrule);

   check_component_placement(doc, file);

   /* all is deinit */
   end_cii();

   EliminateSinglePointPolys(doc);                

   fclose(ifp);

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!logFile.IsEmpty())
      fclose(ferr);

   if (display_error && !logFile.IsEmpty())
      Logreader(logFile);

   return;
}


/*Ende **********************************************************************/




