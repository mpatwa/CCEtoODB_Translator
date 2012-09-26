// $Header: /CAMCAD/4.5/apRead.h 12    2/07/06 4:44p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

/* Define Section *********************************************************/
#define SEARCH_UNKNOWN           0

#define MIN_APSIZE                  0.3937   // .3937 mil = .010 mm, minumum apersize in MIL, case 1819: make min smaller than 1 mil (.0254 mm)
#define MAX_APSIZE               2100.0      // maximum size to validate according to Gerber in MIL

#define APP_UNITS_INCH           1
#define APP_UNITS_MIL            2
#define APP_UNITS_MM             3
#define APP_UNITS_TMIL           4
#define APP_UNITS_HMIL           5
#define APP_UNITS_MICRON         6

#define TOK_UNITS                1
#define TOK_IGNORE               2
#define TOK_TXTDEL               3
#define TOK_SCANLINE             4
#define TOK_WRDDEL               5
#define TOK_LOCALAPMIN           -1
#define TOK_LOCALAPMAX           -2

#define READ_TERADYNE_1          16
#define READ_OLD_PCGERBER        17
#define TOK_UNIT_MM              18
#define TOK_UNIT_MIL             19
#define TOK_UNIT_INCH            20
#define TOK_UNIT_TMIL            21
#define TOK_UNIT_HMIL            22
#define TOK_UNIT_MICRON          23
#define TOK_SKIPLINE             24
#define TOK_LOGICAL_EOF          25

#define  FORM_ROUND              6
#define  FORM_SQUARE             7
#define  FORM_RECT               8
#define  FORM_OBLONG             9
#define  FORM_THERM              10
#define  FORM_DONUT              11
#define  FORM_TARGET             12
#define  FORM_OCTAGON            13
#define  FORM_BLANK              14
#define  FORM_COMPLEX            15

#define SCAN_XSIZE               1
#define SCAN_YSIZE               2
#define SCAN_DCODE               3
#define SCAN_APNUM               4
#define SCAN_SKIPWORD            5
#define SCAN_FORM                6
#define SCAN_MUST_FORM_ROUND     7
#define SCAN_MUST_FORM_SQUARE    8
#define SCAN_MUST_FORM_RECT      9
#define SCAN_MUST_FORM_OBLONG    10
#define SCAN_MUST_FORM_THERM     11
#define SCAN_MUST_FORM_DONUT     12
#define SCAN_MUST_FORM_TARGET    13
#define SCAN_MUST_FORM_OCTAGON   14
#define SCAN_MUST_FORM_BLANK     15
#define SCAN_MUST_FORM_COMPLEX   16
#define SCAN_APROTATION          17
#define SCAN_UNIT_VAL            18

#define MAX_COMMAND              20   // max entities per scan line
#define MAX_DEF                  20
#define MAX_SCANLINE             20    // max scanlines per command
#define MAX_FORM                 20
#define MAX_SKIPLINE             20
#define MAX_APNUM                1000

#define SIZ_UNIT_LST             (sizeof(unit_lst) / sizeof(List))
#define SIZ_TOK_LST              (sizeof(tok_lst) / sizeof(List))
#define SIZ_SCAN_LST             (sizeof(scan_lst) / sizeof(List))


/* Structures Section *********************************************************/

typedef  struct
{
   char     *token;
   int      type;
} List;

// general Apstruct
typedef struct
{
   double   xsize;
   double   ysize;
   double   xoffset, yoffset;
   float     localApMin;
   float      localApMax;
   BOOL     localApSizeIsSet;
   int      apnum;
   int      dcode;
   int      rotation;   // in degree
   int      apptype;    // round etc..
   char     appform[20];   // this is the name given in the macro file.
                           // .FORM_xxx Name
} Apdef;    // this is the global aperture definition

typedef struct
{
   int      commandcnt;
   int      command[MAX_COMMAND];
}Scanline;

typedef struct
{
   char     *formname;
   int      formtype;      // this is the FORM_xxx define
}Form;

typedef struct
{
   char     listname[20];        // name of command list
   char     units;
   char     *space;              // space for word delimeter
   char     *ignore;             // ignore string
   char     txtdel[2];           // 2 char start and end text del
   char     *worddel;            // delimeter to seperate words
   char     *logical_eof;        // string to determine EOF
   int      formcnt;
   Form     form[MAX_FORM];
   int      scanlinecnt;
   float    localApMin;
   float    localApMax;
   BOOL     localApSizeSet;
   Scanline scanline[MAX_SCANLINE];   // scanlinecommand array
   int      skiplinecnt;
   char     *skipline[MAX_SKIPLINE];
   char     def_unit_mm[20];     // custom unit word
   char     def_unit_mil[20];    //
   char     def_unit_inch[20];   //
   char     def_unit_tmil[20];   //
   char     def_unit_hmil[20];   //
   char     def_unit_micron[20]; //
} Def;

typedef struct
{
   int      defcnt;
   Def      def[MAX_DEF];
}Maclist;


/* Lists Section *********************************************************/
static List  tok_lst[] =
{
   {".UNITS",              TOK_UNITS},
   {".IGNORE",             TOK_IGNORE},
   {".LOCALAPMIN",         TOK_LOCALAPMIN},
   {".LOCALAPMAX",         TOK_LOCALAPMAX},
   {".TEXTDELIMETER",      TOK_TXTDEL},
   {".SCANLINE",           TOK_SCANLINE},
   {".WORDDELIMETER",      TOK_WRDDEL},
   {".FORM_ROUND",         FORM_ROUND},
   {".FORM_SQUARE",        FORM_SQUARE},
   {".FORM_RECT",          FORM_RECT},
   {".FORM_OBLONG",        FORM_OBLONG},
   {".FORM_THERM",         FORM_THERM},
   {".FORM_DONUT",         FORM_DONUT},
   {".FORM_TARGET",        FORM_TARGET},
   {".FORM_OCTAGON",       FORM_OCTAGON},
   {".FORM_BLANK",         FORM_BLANK},
   {".FORM_COMPLEX",       FORM_COMPLEX},
   {".READ_TERADYNE_1",    READ_TERADYNE_1},
   {".READ_OLD_PCGERBER",  READ_OLD_PCGERBER},
   {".UNIT_MM",            TOK_UNIT_MM},
   {".UNIT_MIL",           TOK_UNIT_MIL},
   {".UNIT_INCH",          TOK_UNIT_INCH},
   {".UNIT_TMIL",          TOK_UNIT_TMIL},
   {".UNIT_HMIL",          TOK_UNIT_HMIL},
   {".UNIT_MICRON",        TOK_UNIT_MICRON},
   {".SKIPLINE",           TOK_SKIPLINE},
   {".LOGICAL_EOF",        TOK_LOGICAL_EOF},
};

static List     scan_lst[] =
{
   {".XSIZE",              SCAN_XSIZE},
   {".YSIZE",              SCAN_YSIZE},
   {".DCODE",              SCAN_DCODE},
   {".APNUMBER",           SCAN_APNUM},
   {".SKIPWORD",           SCAN_SKIPWORD},
   {".FORM",               SCAN_FORM},
   {".MUST_FORM_ROUND",    SCAN_MUST_FORM_ROUND},
   {".MUST_FORM_SQUARE",   SCAN_MUST_FORM_SQUARE},
   {".MUST_FORM_RECT",     SCAN_MUST_FORM_RECT},
   {".MUST_FORM_OBLONG",   SCAN_MUST_FORM_OBLONG},
   {".MUST_FORM_THERM",    SCAN_MUST_FORM_THERM},
   {".MUST_FORM_DONUT",    SCAN_MUST_FORM_DONUT},
   {".MUST_FORM_TARGET",   SCAN_MUST_FORM_TARGET},
   {".MUST_FORM_OCTAGON",  SCAN_MUST_FORM_OCTAGON},
   {".MUST_FORM_BLANK",    SCAN_MUST_FORM_BLANK},  
   {".MUST_FORM_COMPLEX",  SCAN_MUST_FORM_COMPLEX},
   {".APROT",              SCAN_APROTATION},       // aprot in degree
   {".UNIT_VAL",           SCAN_UNIT_VAL},
};


/* Function Prototypes *********************************************************/
int output_app(Apdef *app);
int get_inversion();

int apread(char *macrofile, const char *inputfile, char *logfile, int *ap_units, char *macName);

// special readers
int read_teradyne_1(const char *fname, FILE *flog);
int read_old_pcgerber(const char *fname, FILE *flog);

