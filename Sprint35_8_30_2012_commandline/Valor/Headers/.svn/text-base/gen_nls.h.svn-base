/***********************************************************
 *
 *  Module: gen_nls.h
 *
 *  Description:
 *
 *    This module includes declarations used by the gen_nls.c module.
 *
 *  Creation Date: 	18 Sep 1996
 *  Author:		Eran Aharon
 ***********************************************************/

#ifndef  GEN_NLS_DEF
#define GEN_NLS_DEF

/* Error codes */

#define E_GEN_NLS_INTERNAL	7001
#define E_GEN_NLS_MEM		7002
#define E_GEN_NLS_FILE_NEXIST	7003
#define E_GEN_NLS_OPEN_FILE	7004

/* Supported Languages */

#define NLS_LANG_ENGLISH             0
#define NLS_LANG_JAPANESE            1
#define NLS_LANG_TAIWANESE           2
#define NLS_LANG_CHINESE_GB          3
#define NLS_LANG_KOREAN              4
#define NLS_LANG_GERMAN              5
#define NLS_LANG_FRENCH              6
#define NLS_LANG_ITALIAN             7
#define NLS_LANG_SPANISH             8
#define NLS_LANG_DUTCH               9
#define NLS_LANG_SWEDISH            10
#define NLS_LANG_CHINESE_BIG5       11          


#define LANG_ENGLISH_NAME        "C"
#define LANG_JAPANESE_NAME       "ja"
#define LANG_TAIWANESE_NAME      "tw"
#define LANG_CHINESE_GB_NAME     "gb"
#define LANG_CHINESE_BIG5_NAME   "big5"
#define LANG_KOREAN_NAME         "ko"
#define LANG_GERMAN_NAME         "gr"
#define LANG_FRENCH_NAME         "fr"
#define LANG_ITALIAN_NAME        "it"
#define LANG_SPANISH_NAME        "sp"
#define LANG_DUTCH_NAME          "du"
#define LANG_SWEDISH_NAME        "sw"

typedef enum {
   NLS_ENCODING_NONE = 0,
   NLS_ENCODING_EUC  = 1,
   NLS_ENCODING_SJIS = 2,
   NLS_ENCODING_EUC_KR = 3,
   NLS_ENCODING_EUC_TW = 4,
   NLS_ENCODING_GB   = 5,
   NLS_ENCODING_BIG5 = 6
} encoding_enum;


#define NLS_HEADER     "@$@#"
#define NLS_GENERAL    "GENERAL"
#define NLS_STR_HEADER "$@"

#define XXSTR(s) #s
#define XSTR(s)  XXSTR(s)

#ifdef NLS_GROUP
#define NLSD(x) (NLS_HEADER __FILE__ NLS_HEADER NLS_GROUP   NLS_HEADER XSTR(__LINE__) NLS_HEADER x)
#else
#define NLSD(x) (NLS_HEADER __FILE__ NLS_HEADER NLS_GENERAL NLS_HEADER XSTR(__LINE__) NLS_HEADER x)
#endif  

#define NLST(x)  gen__nls_nlst((x))
#define NLSAP(x,y)  gen__nls_nlsa_get_att_prompt((x),(y))
#define NLSAN(x)  gen__nls_nlsa_get_att_name((x))
#define NLSI(x)  NLST(NLSD(x))
#define NLSQ(x)  QString::fromLocal8Bit(NLSI(x))

/* For ODB purpose */

#define NLSE(x)      x                            /* Just for marking the text   */
#define NLST_ODB(x)  gen__nls_nlst_odb((x))       /* Translate without searching */
                                                  /* for file or group           */

#endif /* GEN_NLS_DEF */
