// $Header: /CAMCAD/4.4/UnidatIn.h 1     5/24/04 6:32p Kurt Van Ness $

#if !defined(__UnidatIn_h__)
#define __UnidatIn_h__

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

typedef struct
{
   char  *key;
   char  *val;
}UNIDATAtt;

typedef struct
{
   double d;   
   int    toolindex;
}UNIDATDrill;
typedef CArray<UNIDATDrill, UNIDATDrill&> CDrillArray;

typedef struct         
{
   CString  unidat_name;
   CString  cc_name;
} UNIDATAttrmap;
typedef CTypedPtrArray<CPtrArray, UNIDATAttrmap*> CAttrmapArray;

typedef struct         
{
   CompPinStruct  *comppin;      // struct from add_comppin
   double         x, y;
   CString        padtop, padbot;
   double         toprot, botrot;
} UNIDATCompPin;
typedef CTypedPtrArray<CPtrArray, UNIDATCompPin*> CCompPinArray;

typedef struct         
{
   CString        name;
   CString        geom;
   int            mirror;
   int            rotation;      // degrees
   DataStruct     *data;
} UNIDATComp;
typedef CTypedPtrArray<CPtrArray, UNIDATComp*> CCompArray;

typedef CArray<Point2, Point2&> CPolyArray;

static int fskip(), fnull();
static int unidat_info();
static int unidat_outline();
static int unidat_fiducials();
static int unidat_component();
static int unidat_other_drillings();
static int unidat_pad();
static int unidat_shape();
static int unidat_via();
static int unidat_track();
static int unidat_component_pin();
static int unidat_skipsection();
static int make_localcomps();
static int do_assign_layers();
static int update_padstackddrill();
static void setFileReadProgress(const CString& status);

static List start_lst[] =
{
   {"%%%INFO",             unidat_info},  
   {"%%OUTLINE",           unidat_outline},  
   {"%%FIDUCIALS",         unidat_fiducials},  
   {"%%COMPONENT_PIN",     unidat_component_pin},  
   {"%%COMPONENT",         unidat_component},  
   {"%%OTHER_DRILLINGS",   unidat_other_drillings},  
   {"%%PAD",               unidat_pad},  
   {"%%SHAPE",             unidat_shape},  
   {"%%VIA",               unidat_via},  
   {"%%TRACK",             unidat_track},  
   {"%%SUMMARY",           unidat_skipsection},  
   {"%%SUBCOMPONENT",      unidat_skipsection},  
   {"%%TESTPAD",           unidat_skipsection},  
};
#define  SIZ_START_LST     (sizeof(start_lst) / sizeof(List))

#endif
