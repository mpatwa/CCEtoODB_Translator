// $Header: /CAMCAD/4.6/read_wrt/PadsIn.h 23    5/02/07 5:56p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved. */

// PADSIN.H

#if !defined(__PadsIn_H__)
#define __PadsIn_H__

#pragma once

#include "TypedContainer.h"
#include "DeviceType.h"

#define  MAX_LINE                2000  /* Max line length.           */
#define  TEXT_HEIGHTFACTOR       (1.0/1.4)   // if a text is .12 is PADS ASCII, it is really only
                                       // 0.1 in graphic
#define  TEXT_WIDTHFACTOR        (0.55)   

#define  FID_ATT                 1
#define  SMD_ATT                 2
#define  DEVICE_ATT              3
#define  HEIGHT_ATT              4
#define  FID_GEOM                5
#define  TOOL_GEOM               6

#define  ARC_COUNTERCLOCK        1
#define  ARC_CLOCK               2

#define  MAX_ALTDECAL            32    // in PowerPCB 2.0 is upded to 16 from 4 -> allow more if a type@shape is used
#define  MAX_LAYERS              255   /* Max number of layers.   */
#define  MAX_PADSTACK_SIZE       253   // size of padstack arrays

class CAttrCopyPair : public CObject
{
private:
   CString m_from; // attr name to copy value from
   CString m_to;  // attr name to copy value to

public:
   CString GetFromName()   { return m_from; }
   CString GetToName()     { return m_to; }

   CAttrCopyPair(CString from, CString to)   { m_from = from; m_to = to; }
};

class CAttrCopyList : public CTypedPtrArrayContainer<CAttrCopyPair*>
{
public:
   bool AddAttrPair(CString from, CString to);
   bool Apply(CCEtoODBDoc *doc, FileStruct *file);
};


typedef struct
{
   double   height,linewidth;
}PADSRefnamesize;

typedef struct
{
   CString  layerName;        // pads layer name
   char  layerType;           // layer attributes as defined in dbutil.h
   int   stacknumber;   // electrical stack number
} PADSAdef;

typedef struct 
{
   CString  ptypename;
   CString  originaldecalname[MAX_ALTDECAL];    // this is as defined in DECAL section
   CString  decalname[MAX_ALTDECAL];            // this is the name TYPE.DECAL
	CString	primaryDecalname;							// Usually same as decalname[0] and originaldecalname[0], but easier to access and more reliable, as the array entries may be altered during other processing
   unsigned int altused;                        // 0..15 alts are allowed, this flags which ones are used in the component list
   int      decalcnt;
   CString  value;
   CString  tolerance;
   CString  typ;
   CString  pinname;       // pinname , pinname 
   int      smdflag:1;     // flags derived from attributes.
   int      fiducial:1;
   int      alphapins:1;   // check if a part contains alpha pins 
   double   compheight;
} PADSPart;
typedef CTypedPtrArray<CPtrArray, PADSPart*> CPartArray;

typedef struct
{
   CString  name;
   int   atttype; // attribute type aka attribute name code
   DeviceTypeTag   devicetype; // camcad device type, only used when atttype is DEVICE_TYPE 
}PADSAttr;
typedef CTypedPtrArray<CPtrArray, PADSAttr*> CAttrArray;

typedef struct
{
   int      stacknumber;      
   int      component;     // this must be set to make the associated layers active
   CString  layer_name;
   CString  layer_type;
   CString  associated_silk_screen;
   CString  associated_paste_mask;
   CString  associated_solder_mask;
   CString  associated_assembly;
}PADSMiscLayer;
typedef CTypedPtrArray<CPtrArray, PADSMiscLayer*> CMiscLayerArray;

typedef struct
{
   CString  name;
}PADSViadef;
typedef CTypedPtrArray<CPtrArray, PADSViadef*> CViadefArray;

typedef struct
{
   int      pinnr;
   CString  pinname;
   CString  decalname;
}PADSPadsIgnored;
typedef CTypedPtrArray<CPtrArray, PADSPadsIgnored*> CPadsignoredArray;

typedef struct
{
   CString  compname;
   CString  geomname;
   int      pinnr;
   CString  pinname;
   int      already_reported;
}PADSCompPinIgnored;
typedef CTypedPtrArray<CPtrArray, PADSCompPinIgnored*> CCompPinignoredArray;

typedef struct
{
   CString  attrib;
   CString  mapattrib;
}PADSAttribmap;
typedef CTypedPtrArray<CPtrArray, PADSAttribmap*> CAttribmapArray;

typedef struct 
{
   CString  name;
   DataStruct  *data_adress;
} PADSPour;
typedef CTypedPtrArray<CPtrArray, PADSPour*> CPourArray;

typedef struct 
{
   int         pinnr;
   int         layer;
   BlockStruct *pinblock;
} PADSPincopper;
typedef CTypedPtrArray<CPtrArray, PADSPincopper*> CPincopperArray;

class PADSTerminal
{
private:
   CString  m_refname;
   double m_x;
   double m_y;
   int m_padstackindex;
   bool m_explicitlySet;

public:
   PADSTerminal(CString refname, double x, double y, int padstackIndx = -1, bool explicitlySet = false)
   { m_refname = refname; m_x = x; m_y = y; m_padstackindex = padstackIndx; m_explicitlySet = explicitlySet; }

   PADSTerminal()
   { m_x = 0.; m_y = 0.; m_padstackindex = -1; m_explicitlySet = false; }

   CString GetRefname()             { return m_refname; }
   void SetRefname(CString ref)     { m_refname = ref; }

   double GetX()                    { return m_x; }
   void SetX(double x)              { m_x = x; }

   double GetY()                    { return m_y; }
   void SetY(double y)              { m_y = y; }

   int GetPadstackIndex()           { return m_padstackindex; }
   void SetPadstackIndex(int psi)   { m_padstackindex = psi; }

   bool GetExplicitlySet()          { return m_explicitlySet; }
   void SetExplicitlySet(bool flg)  { m_explicitlySet = flg;  }
};
typedef CArray<PADSTerminal, PADSTerminal&> CTerminalArray;


class PADS_Paddef
{
private:
   CString m_padstackName;
   bool m_platedFlag;
   double m_drillsize;

public:
   
   int   padstack[MAX_PADSTACK_SIZE];  // -2 .. 250
   int   typ;           // 1 top, 3 inner, 2 bottom 4 drill
   

public:
   CString getPadstackName()        { return m_padstackName; }
   void setPadstackName(CString nm) { m_padstackName = nm; }

   bool getPlatedFlag() const { return m_platedFlag; }
   void setPlatedFlag(bool flag) { m_platedFlag = flag; }

   double getDrillSize()         { return m_drillsize; }
   void setDrillSize(double ds)  { m_drillsize = ds; }
};

class CPaddefArray : public CTypedPtrArrayContainer<PADS_Paddef*>
{
public:
   PADS_Paddef* getAt(int index);
};

//typedef CArray<PADS_Paddef, PADS_Paddef&> CPaddefArray;

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

static int pads_title_pads();
static int pad5_title_pads();
static int pwr1_title_inch(), pwr1_title_mil(), pwr1_title_metric(), pwr1_title_basic();
static int pwr15_title_inch(), pwr15_title_mil(), pwr15_title_metric(), pwr15_title_basic();
static int pad7_title_inch(), pad7_title_mil(), pad7_title_metric(), pad7_title_basic();
static int pad6_title_inch(), pad6_title_mil(), pad6_title_metric(), pad6_title_basic();
static int pad4_title_inch(), pad4_title_mil(), pad4_title_metric(), pad4_title_basic();
static int pad3_title_inch(), pad3_title_mil(), pad3_title_metric(), pad3_title_basic();
static int pads_title_inch(), pads_title_mil(), pads_title_metric(), pads_title_basic(), pads_end();
static int pwrunknown_title();
static int pads_misc(), pads_testpoint();
static int pwr2005_0_title_basic();
static int pwr2007_0_title_inch(), pwr2007_0_title_mil(), pwr2007_0_title_metric(), pwr2007_0_title_basic();
static int pwr2009_0_title_inch(), pwr2009_0_title_mil(), pwr2009_0_title_metric(), pwr2009_0_title_basic();

static List start_lst[] =
{
   "*PADS-PCB",                  	pads_title_pads,
   "*PADS-PCB-V5*",              	pad5_title_pads,
   "*SCHEMA-PCB",                	pads_title_pads,
   "*PADS2000-IN",               	pads_title_inch,
   "*PADS2000-MI",               	pads_title_mil,
   "*PADS2000-ME",               	pads_title_metric,
   "*PADS2000-BA",               	pads_title_basic,
   "*PADS2000-V.3-IN",           	pad3_title_inch,
   "*PADS2000-V.3-MI",           	pad3_title_mil,
   "*PADS2000-V.3-ME",           	pad3_title_metric,
   "*PADS2000-V.3-BA",           	pad3_title_basic,
   "*PADS2000-V4-IN",            	pad4_title_inch,
   "*PADS2000-V4-MI",            	pad4_title_mil,
   "*PADS2000-V4-ME",            	pad4_title_metric,
   "*PADS2000-V4-BA",            	pad4_title_basic,
   "*PADS-PERFORM-V6-INCHES",    	pad6_title_inch,
   "*PADS-PERFORM-V6-MILS",      	pad6_title_mil,
   "*PADS-PERFORM-V6-METRIC",    	pad6_title_metric,
   "*PADS-PERFORM-V6-BASIC",     	pad6_title_basic,
   "*PADS-WORK-V6-INCHES",       	pad6_title_inch,
   "*PADS-WORK-V6-MILS",         	pad6_title_mil,
   "*PADS-WORK-V6-METRIC",       	pad6_title_metric,
   "*PADS-WORK-V6-BASIC",        	pad6_title_basic,
   "*PADS-PERFORM-V7-INCHES",    	pad7_title_inch,
   "*PADS-PERFORM-V7-MILS",      	pad7_title_mil,
   "*PADS-PERFORM-V7-METRIC",    	pad7_title_metric,
   "*PADS-PERFORM-V7-BASIC",     	pad7_title_basic,
   "*PADS-WORK-V7-INCHES",       	pad7_title_inch,
   "*PADS-WORK-V7-MILS",         	pad7_title_mil,
   "*PADS-WORK-V7-METRIC",       	pad7_title_metric,
   "*PADS-WORK-V7-BASIC",        	pad7_title_basic,
   "!PADS-POWERPCB-V1-INCHES",   	pwr1_title_inch,
   "!PADS-POWERPCB-V1-MILS",     	pwr1_title_mil,
   "!PADS-POWERPCB-V1-METRIC",   	pwr1_title_metric,
   "!PADS-POWERPCB-V1-BASIC",    	pwr1_title_basic,
   "!PADS-POWERPCB-V1.5-INCHES", 	pwr15_title_inch,
   "!PADS-POWERPCB-V1.5-MILS",   	pwr15_title_mil,
   "!PADS-POWERPCB-V1.5-METRIC", 	pwr15_title_metric,
   "!PADS-POWERPCB-V1.5-BASIC",  	pwr15_title_basic,

	"!PADS-POWERPCB-V2005.0-BASIC!",	pwr2005_0_title_basic,	// PADS 2005.1

   "!PADS-POWERPCB-V2007.0-INCHES!",pwr2007_0_title_inch,
   "!PADS-POWERPCB-V2007.0-MILS!",  pwr2007_0_title_mil,
   "!PADS-POWERPCB-V2007.0-METRIC!",pwr2007_0_title_metric,
   "!PADS-POWERPCB-V2007.0-BASIC!", pwr2007_0_title_basic,

   // PADS 9.0 - Standard up-to-30 layer mode
   "!PADS-POWERPCB-V9.0-INCHES!",        pwr2009_0_title_inch,
   "!PADS-POWERPCB-V9.0-MILS!",          pwr2009_0_title_mil,
   "!PADS-POWERPCB-V9.0-METRIC!",        pwr2009_0_title_metric,
   "!PADS-POWERPCB-V9.0-BASIC!",         pwr2009_0_title_basic,

   // PADS 9.0 - 250 layer mode
   "!PADS-POWERPCB-V9.0-INCHES-250L!",   pwr2009_0_title_inch,
   "!PADS-POWERPCB-V9.0-MILS-250L!",     pwr2009_0_title_mil,
   "!PADS-POWERPCB-V9.0-METRIC-250L!",   pwr2009_0_title_metric,
   "!PADS-POWERPCB-V9.0-BASIC-250L!",    pwr2009_0_title_basic,

   // PADS 9.2 - Standard up-to-30 layer mode
   "!PADS-POWERPCB-V9.2-INCHES!",        pwr2009_0_title_inch,
   "!PADS-POWERPCB-V9.2-MILS!",          pwr2009_0_title_mil,
   "!PADS-POWERPCB-V9.2-METRIC!",        pwr2009_0_title_metric,
   "!PADS-POWERPCB-V9.2-BASIC!",         pwr2009_0_title_basic,

   // PADS 9.2 - 250 layer mode
   "!PADS-POWERPCB-V9.2-INCHES-250L!",   pwr2009_0_title_inch,
   "!PADS-POWERPCB-V9.2-MILS-250L!",     pwr2009_0_title_mil,
   "!PADS-POWERPCB-V9.2-METRIC-250L!",   pwr2009_0_title_metric,
   "!PADS-POWERPCB-V9.2-BASIC-250L!",    pwr2009_0_title_basic,

   // PADS 9.3 - Standard up-to-30 layer mode
   "!PADS-POWERPCB-V9.3-INCHES!",        pwr2009_0_title_inch,
   "!PADS-POWERPCB-V9.3-MILS!",          pwr2009_0_title_mil,
   "!PADS-POWERPCB-V9.3-METRIC!",        pwr2009_0_title_metric,
   "!PADS-POWERPCB-V9.3-BASIC!",         pwr2009_0_title_basic,

   // PADS 9.3 - 250 layer mode
   "!PADS-POWERPCB-V9.3-INCHES-250L!",   pwr2009_0_title_inch,
   "!PADS-POWERPCB-V9.3-MILS-250L!",     pwr2009_0_title_mil,
   "!PADS-POWERPCB-V9.3-METRIC-250L!",   pwr2009_0_title_metric,
   "!PADS-POWERPCB-V9.3-BASIC-250L!",    pwr2009_0_title_basic,

   "!PADS-POWERPCB",						pwrunknown_title,
};

#define  SIZ_START_LST             (sizeof(start_lst) / sizeof(List))

static int   pads_pcb(), pads_reuse(), pads_matrix(),pads_text();
static int   pads_lines(),pads_decal(),pad6_decal(), pads_type();
static int   pads_part(),pads_route(),pads_signal(),pads_connection();
static int   pads_pour(),pads_null();
static int   pad6_via(),pad6_signal();

static List  pcb_lst[] =
{
   "*PCB*",                      pads_pcb,
   "*MATRIX*",                   pads_matrix,
   "*TEXT*",                     pads_text,
   "*LINES*",                    pads_lines,
   "*VIA*",                      pad6_via,
   "*PARTDECAL*",                pads_decal,
   "*PARTTYPE*",                 pads_type,
   "*PART*",                     pads_part,
   "*ROUTE*",                    pads_route,
   "*CONNECTION*",               pads_connection,
   "*SIGNAL*",                   pads_signal,
   "*POUR*",                     pads_pour,
   "*MISC*",                     pads_misc,
   "*CLUSTER*",                  pads_null,
   "*JUMPER*",                   pads_null,
   "*TESTPOINT*",                pads_testpoint,
   "*END*",                      pads_end,
};

#define  SIZ_PCB_LST             (sizeof(pcb_lst) / sizeof(List))

static List  pcb6_lst[] =
{
   "*PCB*",                      pads_pcb,
   "*REUSE*",                    pads_reuse,
   "*MATRIX*",                   pads_matrix,
   "*TEXT*",                     pads_text,
   "*LINES*",                    pads_lines,
   "*VIA*",                      pad6_via,
   "*PARTDECAL*",                pad6_decal,
   "*PARTTYPE*",                 pads_type,
   "*PART*",                     pads_part,
   "*ROUTE*",                    pads_route,
   "*CONNECTION*",               pads_connection,
   "*SIGNAL*",                   pad6_signal,
   "*POUR*",                     pads_pour,
   "*MISC*",                     pads_misc,
   "*CLUSTER*",                  pads_null,
   "*TESTPOINT*",                pads_testpoint,
   "*CLUSTER*",                  pads_null,
   "*JUMPER*",                   pads_null,
   "*END*",                      pads_end,
};

#define  SIZ_PCB6_LST             (sizeof(pcb6_lst) / sizeof(List))

#endif
