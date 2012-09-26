// $Header: /CAMCAD/4.6/read_wrt/Ingun_o.cpp 27    5/15/07 1:15p Rick Faltersack $

/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994, 2000. All Rights Reserved.
*/

// this is now called TRI or TRI-MDA
                              
#include "stdafx.h"
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "graph.h"
#include <math.h>
#include <float.h>
#include "export.h"
#include "find.h"
#include "fabmlib.h"
#include "Ipl_out.h"  // Used for output of nets.asc and parts.asc
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
extern char          *netstatus[];

static CCEtoODBDoc   *doc;

static int           display_error;
static FILE          *logFp;
CSubclassList m_subclassList;
bool IsValidDeviceTypeForSubClassUse(CString deviceType);

typedef struct
{
   char  *name;
   char  *pref;
   char  *default_value;
   int   maxpinallowed;
}TRIdevice;

// Define index for NO-TEST, but not for any others.
// Make all the rest of the code dependent on the table itself,
// with no reliance attached to order or position in the table,
// except that NO_TEST is at index 0.
#define  TRI_NOTEST        			0
#define	TRI_POWERINJ_NAME				10000

static TRIdevice tridevices[] =
{
   "No_Test",						"Skip",  "1",     999,  // MUST BE FIRST (really, must be at index TRI_NOTEST)
   "Resistor",						"R",     "1.00",  2,    
   "Capacitor",					"C",     "1.00",  2, 
   "Capacitor_Polarized",		"C",     "1.00",  2, 
   "Capacitor_Tantalum",		"C",     "1.00",  2, 
   "Inductor",						"L",     "1.00",  2,    
   "Diode",							"D",     "0.7",   2, 
   "Diode_LED",	   			"D",     "0.7",   2,
   "Diode_Zener",					"D",     "0.7",   2,    
   "Transistor_HFE",				"QH",    "1.0",   3,    // DEVICETYPE must be "Transistor" and SUBCLASS must be "HFT"  
   "Transistor",					"Q",     "1.0",   3,    
   "Transistor_NPN",				"Q",     "1.0",   3,    
   "Transistor_PNP",				"Q",     "1.0",   3,    
   "Photo_Coupler",				"PC",    "0.7",   4,    // DEVICETYPE must be "IC_Linear" and SUBCLASS must be "Photo_Coupler"
   "Jumper",						"J",     "1.00",  2,    
   "Fuse",							"J",     "1.00",  2,    
	"Transistor_Mosfet_NPN",	"QF",    "1.00",  2,    
   "Transistor_Mosfet_PNP",   "QF",    "1.00",  2,    
   "Transistor_FET_NPN",      "QF",    "1.00",  2,    
   "Transistor_FET_PNP",      "QF",    "1.00",  2,    
   "Transistor_SCR",          "QS",    "1.00",  2,    
   "Transistor_Triac",        "QS",    "1.00",  2,    
   "Crystal",						"O",     "1.00",  2,    
   "Unknown",						"?",     "1",     2,    
   "IC",								"IC",    "",      999,  
   "IC_Digital",					"IC",    "",      999,
};

// this is not really a components, but a 2 pin list of test pins.
typedef struct
{
   CString  name;   // Component refDes + element name    // list of components in the datalist
   CString  originalName; //Component refDes in CAMCAD database
   CString  value;
   double   ptol, ntol;
   CString  device;
   int      ingundevicetype;
   int      smdflag;       // if ATT_SMD was set
   int      inserttype;    // np stored inserttype;
   int      pincnt;
   int      bottom;
   int      not_implemented_ingunclass;
}TRIComp;
typedef CTypedPtrArray<CPtrArray, TRIComp*> TRICompArray;

// this keeps track of netlist status
typedef struct
{
   CString  netname;
   char     groundnet;     // 0 is no, 1 = ground, 2 = power
   int      probecnt;
}TRInetname;
typedef CTypedPtrArray<CPtrArray, TRInetname*> TRINetnameArray;

// this is the internal test koo array, which the Takaya maschine needs.
typedef struct
{
   CString           pin;
   CString           comp; // Component refDes + element name
   CString           orginalCompName; //Component refDes in CAMCAD database
   char              tested;
   unsigned long     entitynr;
   int               netindex;      // -1 is not connected, needs a singe special probe, otherwise a netprobe is ok.
   double            x, y;
   int               testprobeptr;  // -1 is not initialized.
}TRIpintest;
typedef CTypedPtrArray<CPtrArray, TRIpintest*> CPintestArray;

// Testprobe is from CC file. This is normally populated with TA.
typedef struct
{
   CString  geomname;   // probe def   
   int      refnum;     // must be an integer
   int      netindex;   // which net does this probe test
   int      bottom;     // testprobe on bottom or top
   int      probetype;  // needed for nails.asc probe 1 is the largest, 2 is next etc..
   double   x,y;
   long     datalink;   // to access point in TK_testprobeaccesslink
}TRItestprobe;
typedef CTypedPtrArray<CPtrArray, TRItestprobe*> CTestprobeArray;

typedef struct
{
   long  accessentity;     // entity of the access point
   long  featurelink;      // datalink to the feature, such as comppin, via or other
}TRItestprobeaccesslink;
typedef CTypedPtrArray<CPtrArray, TRItestprobeaccesslink*> CTestprobeaccesslinkArray;

typedef struct
{
   CString  pinfunction;      // base, collector, emitter
   CString  pinname; 
}TRIpinfunction;
typedef CTypedPtrArray<CPtrArray, TRIpinfunction*> CPinfunctionArray;

typedef CArray<Point2, Point2&> CPolyArray;

typedef struct _TRInaillinklist
{
	int nailindex;
	int netindex;
	struct _TRInaillinklist*  next;
}TRInaillinklist;
typedef CTypedPtrArray<CPtrArray, TRInaillinklist*> CNaillinklistArray;

static   CPolyArray  polyarray;
static   int         polycnt;

static   TRINetnameArray   netnamearray;
static   int         netnamecnt;

static   TRICompArray   comparray;
static   int         compcnt;

static   TRIOutputArray trioutputarray;
static   int         trioutputcnt;

static   CTestprobeArray   testprobearray;
static   int         testprobecnt;

static   CTestprobeaccesslinkArray  testprobeaccesslinkarray;
static   int         testprobeaccesslinkcnt;

static   CPinfunctionArray pinfunctionarray;
static   int         pinfunctioncnt;

static   CPintestArray  pintestarray;
static   int         pintestcnt;

// this is a pintest for only 1 component.
static   CPintestArray  comppinarray;
static   int         comppincnt;

static	CNaillinklistArray naillistarray;
static	int max_refnum;

static   int         is_comp_test(const char *n);
static   int         is_comp_smd(const char *n);
static   int         load_INGUNsettings(const CString fname, int pageunits);

static   FileStruct  *cur_file;
static   int         probenum = 0;
static   int settingsFile_pageunits;
/*****************************************************************************/
/*
*/
static int INGUNPinNameCompareFunc( const void *arg1, const void *arg2 )
{
   TRIpintest **a1, **a2;
   a1 = (TRIpintest**)arg1;
   a2 = (TRIpintest**)arg2;

   return compare_name((*a1)->pin, (*a2)->pin);
}

/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is 
   because of the 2 pass to get artwork info.
*/
static void INGUN_SortPinData()
{
   if (comppincnt < 2)  return;

   qsort(comppinarray.GetData(), comppincnt, sizeof(TRIpintest *), INGUNPinNameCompareFunc);
   return;
}


/*****************************************************************************/
/*
*/
static int is_comp_smd(const char *name)
{
   int   i;

   for (i=0;i<compcnt;i++)
   {
      if (!strcmp(comparray[i]->name, name) && comparray[i]->smdflag)
         return i+1;
   }

   return 0;
}

/*****************************************************************************/
/*
*/
static   int   get_compindex(const char *n)
{
   int   i;

   if (strlen(n) == 0)  
      return -1;

   for (i=0;i<compcnt;i++)
   {
      if (!strcmp(comparray[i]->name,n))
         return i;
   }

   TRIComp *c = new TRIComp;
   comparray.SetAtGrow(compcnt,c);  
   compcnt++;  

   c->name = n;
   c->ingundevicetype = TRI_NOTEST;    // none
   c->smdflag = 0;                     // if ATT_SMD was set
   c->inserttype = 0;                  // np stored inserttype;
   c->pincnt = 0;
   c->bottom = 0;
   c->not_implemented_ingunclass = FALSE;

   return compcnt-1;
}

//-----------------------------------------------------------------------------
//
// test if it is a component and not testnone
//
static int is_pcbcomp(const char *c)
{
   int   i;

   for (i=0;i<compcnt;i++)
   {
      if (!strcmp(comparray[i]->name, c) && 
          comparray[i]->inserttype == INSERTTYPE_PCBCOMPONENT)
      {
         return i+1;
      }
   }

   return 0;
}

//-----------------------------------------------------------------------------
static int get_netnameptr(const char *c)
{
   int   i;

   for (i=0;i<netnamecnt;i++)
   {
      if (!strcmp(netnamearray[i]->netname, c))
      {
         return i;
      }
   }

   return -1;
}

//--------------------------------------------------------------
static int  update_netlist(CNetList *NetList)
{
   NetStruct *net;
   POSITION  netPos;
   Attrib    *a;
   int       gndfound = 0;
   int       vccfound = 0;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      TRInetname *c = new TRInetname;
      netnamearray.SetAtGrow(netnamecnt,c);  
      netnamecnt++;  
      c->netname = net->getNetName();
      c->probecnt = 0;
      c->groundnet = 0;
      if (a = is_attvalue(doc, net->getAttributesRef(), ATT_TEST_NET_STATUS, 0)) 
      {
         CString l = get_attvalue_string(doc, a);
         if (l.Compare(netstatus[NETSTATUS_GROUND]) == 0)
         {
            c->groundnet = 1;
            gndfound++;
         }
         else
         if (l.Compare(netstatus[NETSTATUS_POWER]) == 0)
         {
            c->groundnet = 2;
            vccfound++;
         }
      }
   }

   if (gndfound != 1)
   {
      // check if a ground net was set, otherwise skip this net
      fprintf(logFp, "No Net identified with Attribute [%s] Value [%s] Status for IC testing.\n", 
               ATT_TEST_NET_STATUS, netstatus[NETSTATUS_GROUND]);
      display_error++;
      return 0;
   }
   if (vccfound != 1)
   {
      // check if a ground net was set, otherwise skip this net
      fprintf(logFp, "No Net identified with Attribute [%s] Value [%s] Status for IC testing.\n", 
               ATT_TEST_NET_STATUS, netstatus[NETSTATUS_POWER]);
      display_error++;
      return 0;
   }

   return 1;
}

//--------------------------------------------------------------
static  int update_pintest(const char *comp, const char *pin, int tested)
{
   int   i;

   for (i=0;i<pintestcnt;i++)
   {
      if (pintestarray[i]->comp.Compare(comp) == 0 && pintestarray[i]->pin.Compare(pin) == 0)
      {
         pintestarray[i]->tested |= tested;
         return i;
      }
   }

   TRIpintest *c = new TRIpintest;
   pintestarray.SetAtGrow(pintestcnt,c);  
   pintestcnt++;  

   c->comp = comp;
   c->pin = pin;
   c->tested = tested;
   c->entitynr = 0;
   c->testprobeptr = -1;

   return pintestcnt-1;
}

static CString getElementName(CString refDes, CString pinName)
{
   CString elementName ="";

   DataStruct *data = datastruct_from_refdes(doc, cur_file->getBlock(), refDes);
   if (data != NULL)
   {
      Attrib * attrib = is_attvalue(doc, data->getAttributesRef(), ATT_SUBCLASS, 2);
      CString subClass = get_attvalue_string(doc, attrib);
      attrib = is_attvalue(doc, data->getAttributesRef(), ATT_DEVICETYPE, 2);
      CString deviceType = get_attvalue_string(doc, attrib);

      CSubclass *sc = m_subclassList.findSubclass(subClass);
      if(sc != NULL && IsValidDeviceTypeForSubClassUse(deviceType))
      {
         for(int elementNum = 0; elementNum < sc->getNumElements(); elementNum++)
         {
            CSubclassElement *el = sc->getElementAt(elementNum);
            if (el != NULL)
            {
               if(el->containsPin(pinName))
                  return el->getName();
            }
         }
      }
   }

   return elementName;
}
//--------------------------------------------------------------
// this function returns all pins of a component in the testkoocnt array.
// the free_testpin function resets this back to zero.
//
static int get_testpinarray(CNetList *NetList)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      int netindex = -1;

      net = NetList->GetNext(netPos);

      if (!(net->getFlags() & NETFLAG_UNUSEDNET))
         netindex = get_netnameptr(net->getNetName());

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         CString elementName;
         elementName  = getElementName(compPin->getRefDes(), compPin->getPinName());
         CString compName = compPin->getRefDes();
         if(!elementName.IsEmpty())
            compName += "_" + elementName;
         int pintestptr = update_pintest(compName, compPin->getPinName(), 0);
         pintestarray[pintestptr]->entitynr = compPin->getEntityNumber();
         pintestarray[pintestptr]->netindex = netindex;
         pintestarray[pintestptr]->x = compPin->getOriginX();
         pintestarray[pintestptr]->y = compPin->getOriginY();
         pintestarray[pintestptr]->orginalCompName = compPin->getRefDes();
      }
   }

   return 1;
}

//--------------------------------------------------------------
static const char *capacitor_value(const char *v)
{
   static   CString  val1;
   float    val;
   char     lp[80];
   unsigned int      i;

   // you can have .22 uf 
   val1 = "";
   for (i=0;i < strlen(v);i++)
   {
      if (isspace(v[i]))   continue;
      val1 += v[i];
   }
   
   // seperate value from unit
   lp[0] = '\0';
   sscanf(val1,"%f%s",&val,lp);

   if (strlen(lp) == 0)
   {
      val1 += "u";
   }
   else
   {
      int len = strlen(lp);
      CString  val2, val3;
      val2 = lp;
      val3 = val1.Left(strlen(val1)-len);
      val1 = val3;
      val1 += (char)tolower(lp[0]);
   }

   return val1.GetBuffer(0);
}

//--------------------------------------------------------------
static const char *inductor_value(const char *v)
{
   static   CString  val1;
   float    val;
   char     lp[80];

   val1 = v;
   // seperate value from unit
   lp[0] = '\0';
   sscanf(v,"%f%s",&val,lp);

   if (strlen(lp) == 0)
   {
      val1 += "m";
   }
   else
   {
      int len = strlen(lp);
      CString  val2, val3;
      val2 = lp;
      val3 = val1.Left(strlen(v)-len);
      val1 = val3;
      val1 += (char)tolower(lp[0]);
   }

   return val1.GetBuffer(0);
}

//--------------------------------------------------------------
static const char *zenerdiode_value(const char *v)
{
   static   CString  val1;

   val1 = v;
   
   if (v != "*")
   {
      double vv = atof(v);
      if (vv == 0)
         val1 = "*";
      else
      {
         char  *lp = STRDUP(val1);
         while (strlen(lp) && isalpha(lp[strlen(lp)-1]))
            lp[strlen(lp)-1] = '\0';
         val1 = lp;
         free(lp);
      }
   }

   return val1.GetBuffer(0);
}

//--------------------------------------------------------------
static const char *resistor_value(const char *v)
{
   static   CString  val1;
   float    val;
   char     lp[80];

   val1 = v;
   // seperate value from unit
   lp[0] = '\0';
   sscanf(v,"%f%s",&val,lp);

   if (strlen(lp) == 0)
   {
      val1 += "K";
   }

   return val1.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static int get_pinfunction_ptr(const char *p, const char *f)
{
   int   i;

   for (i=0;i<pinfunctioncnt;i++)
   {
      if (pinfunctionarray[i]->pinname.CompareNoCase(p) == 0)
      {
         if (!STRNICMP(pinfunctionarray[i]->pinfunction, f, strlen(f)))
            return i;
      }
   }

   return -1;
}

/*****************************************************************************/
/*
*/
static CompPinStruct *get_comppin_from_net(FileStruct *f, const char *n, double x, double y)
{
   CompPinStruct *found = NULL;
   double   dist = FLT_MAX;

   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   netPos = f->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = f->getNetList().GetNext(netPos);
      if (net->getNetName().Compare(n))  continue;

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         if (compPin->getPinCoordinatesComplete())
         {
            if ((fabs(compPin->getOriginX() - x) + fabs(compPin->getOriginY() - y)) < dist)
            {        
               dist = fabs(compPin->getOriginX() - x) + fabs(compPin->getOriginY() - y);
               found = compPin;
            }
         }
         else
         {
            return compPin;
         }
      }
   }

   return found;
}

/*****************************************************************************/
/*
   get a comppin entity nr
   need to go and find access probe
   and than the probe index
*/
static int get_probe_from_access(int comppinentitynr)
{
   int   i;

   for (i=0;i<testprobeaccesslinkcnt;i++)
   {
      if (testprobeaccesslinkarray[i]->featurelink != comppinentitynr)  continue;
      int p;
   
      for (p=0;p<testprobecnt;p++)
      {
         if (testprobearray[p]->datalink == testprobeaccesslinkarray[i]->accessentity)
            return p;
      }
   }

   return -1;
}

/*****************************************************************************/
/*
   get the index to the testprobe array from the pintest entity number
*/
static int get_testprobeindex(int p)
{
   if (pintestarray[p]->netindex < 0)
   {
      // if netindex is unused, find an exact access and probe
      if ((get_probe_from_access(pintestarray[p]->entitynr)) < 0)
      {
         fprintf(logFp,"No Probe access for [%s] [%s]\n", pintestarray[p]->comp, pintestarray[p]->pin);
         display_error++;
      }
   }
   else
   {
      // if netindex is a net, find a probe on the net.
      for (int i=0;i<testprobecnt;i++)
      {
         //TRItestprobe *t = testprobearray[i];
         if (testprobearray[i]->netindex == pintestarray[p]->netindex)
            return i;   // this starts with 1
      }
   }

   return 0;   // this is no probe
}

/*****************************************************************************/
/*
         format->TRI.Capacitor = dlg.m_testtypecapacitors;
         format->TRI.Diode = dlg.m_testtypediodes;
         format->TRI.Inductor = dlg.m_testtypeinductors;
         format->TRI.Jumper = dlg.m_testtypejumpers;
         format->TRI.Photocoupler = dlg.m_testtypephotocouplers;
         format->TRI.Resistor = dlg.m_testtyperesitors;
         format->TRI.Triac = dlg.m_testtypesrc_triac;
         format->TRI.Transistor = dlg.m_testtypetransistors;
         format->TRI.Transistorhfe = dlg.m_testtypetransistorshfe;
         format->TRI.Unknown = dlg.m_testtypeunknown;

*/
static int load_comp_triarray(FileStruct *file, FormatStruct *format)
{
   for (int i=0; i<compcnt; i++)
   {
      if (comparray[i]->ingundevicetype == TRI_NOTEST)
      {
         // eliminate components which are not classified.
      }
      else if (STRICMP(tridevices[comparray[i]->ingundevicetype].pref, "IC") == 0)
      {
         // do some special for components more than 2 pins.

         TRIoutput *c = new TRIoutput;
         trioutputarray.SetAtGrow(trioutputcnt++, c);  

         c->onoff = TRUE;
         c->refname = comparray[i]->name;
         c->tri_type = tridevices[comparray[i]->ingundevicetype].name;
         c->value = "";
         c->ptol = 0;
         c->ntol = 0;
         c->file = "ICN";
         c->testpin[0] = 0;   // hi
         c->testpin[1] = 0;   // lo
         c->testpin[2] = 0;   // gp1
         c->testpin[3] = 0;   // gp2
         c->testpin[4] = 0;   // gp3
         c->testpin[5] = 0;   // gp4
         c->testpin[6] = 0;   // gp5
      }
      else
      {
         TRIoutput *c = new TRIoutput;
         trioutputarray.SetAtGrow(trioutputcnt++, c);  

         c->onoff = TRUE;
         c->refname = comparray[i]->name;
         c->tri_type = tridevices[comparray[i]->ingundevicetype].name;
         c->value = comparray[i]->value;
         c->ptol = comparray[i]->ptol;
         c->ntol = comparray[i]->ntol;
         c->file = "DAT";
         c->testpin[0] = 0;   // hi
         c->testpin[1] = 0;   // lo
         c->testpin[2] = 0;   // gp1
         c->testpin[3] = 0;   // gp2
         c->testpin[4] = 0;   // gp3
         c->testpin[5] = 0;   // gp4
         c->testpin[6] = 0;   // gp5

         int cnt = 0;
         for (int p=0; p<pintestcnt; p++)
         {
            if (pintestarray[p]->comp.Compare(comparray[i]->name))
					continue;   // not the same name

            if (cnt < 4)
            {
               c->testpin[cnt] = get_testprobeindex(p);
               cnt++;
            }
            else
            {
               // only upto 4 pins (test probes) max are allowed. 
               fprintf(logFp, "Component [%s] has too many pins [%d] for Type [%s]!\n", 
                  comparray[i]->name, comparray[i]->pincnt, tridevices[comparray[i]->ingundevicetype].name);
               display_error++;
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* getDeviceIndexByName
*/
static int getDeviceIndexByName(CString devicename)
{
   // here check first that the devicename is a TRI device name
   for (int i=0; i<(sizeof(tridevices) / sizeof(TRIdevice)); i++)
   {
      if (!STRCMPI(tridevices[i].name, devicename))
         return i;
   }

   return TRI_NOTEST;
}

/******************************************************************************
* getDeviceIndexByNameAndSubclass
*/
static int getDeviceIndexByNameAndSubclass(CString devicename, CString subclass)
{
	// The device "Transistor" and "IC_Linear" are treated when they have the specific
	// subclass to support the device type for TRI MDA.  This rule was given by Mark
	// to resolve case #940.  Do not change this without talking to Lynn or Mark.

	if (devicename.Trim().CompareNoCase("Transistor") == 0 && subclass.Trim().CompareNoCase("HFE") == 0)
		devicename = "Transistor_HFE";
	else if (devicename.Trim().CompareNoCase("IC_Linear") == 0 && subclass.Trim().CompareNoCase("Photo_Coupler") == 0)
		devicename = "Photo_Coupler";

   // here check first that the devicename is a TRI device name
   for (int i=0; i<(sizeof(tridevices) / sizeof(TRIdevice)); i++)
   {
      if (!STRCMPI(tridevices[i].name, devicename))
         return i;
   }

   return TRI_NOTEST;
}

/******************************************************************************
* IsPowerInjection
*/
bool IsPowerInjection(CCEtoODBDoc *doc, DataStruct *testProbeDat)
{
   bool retval = false;
   WORD testSourceKW = (WORD)doc->getStandardAttributeKeywordIndex(standardAttributeTestResource);
   Attrib* attrib = NULL;

   if (testProbeDat->getAttributes() != NULL && testProbeDat->getAttributes()->Lookup(testSourceKW, attrib) && attrib != NULL)
   {
      CString value = attrib->getStringValue(); 
      if (value.CompareNoCase("Power Injection") == 0)
         retval = true;
   }

   return retval;
}

/******************************************************************************
* update_comp_list
*/
static int update_comp_list(FileStruct *file, int bottomcomp, int topcomp)
{
   CMapBlockToProbeVal   BlockToProbeMap;
   int noclasscomp = 0;
	int totcomp = 0;
   int mirror = file->isMirrored();
   double rotation = file->getRotation();
   double scale = file->getScale();
   double insert_x = file->getInsertX();
   double insert_y = file->getInsertY();

   Mat2x2 m;
   RotMat2(&m, rotation);

   // the first probe is a 0 name (no probe)
   TRItestprobe *c = new TRItestprobe;
   testprobearray.SetAtGrow(testprobecnt++, c);  
   c->datalink = 0;
   c->geomname = "NOPROBE";
   c->refnum = 0;
   c->netindex = -1;
   c->probetype = -1;


   POSITION pos = file->getBlock()->getHeadDataInsertPosition();
   while (pos != NULL)
   {
      DataStruct* data = file->getBlock()->getNextDataInsert(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;
      
		InsertStruct* insert = data->getInsert();
		if (insert == NULL)
			continue;

      Point2 point2;
      point2.x = insert->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x * scale;         
      point2.y = insert->getOriginY();
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      BlockStruct *block = doc->getBlockAt(insert->getBlockNumber());

      if (insert->getInsertType() == insertTypePcbComponent)   
      {
         CString compname = insert->getRefname();
         if (strlen(compname) == 0)
				continue;

         totcomp++;
         if (insert->getPlacedBottom())
         {
            // bottom placed
            if (!bottomcomp)
            {
               fprintf(logFp, "Component [%s] placed on BOTTOM and ignored in incremental test.\n", compname);
               display_error++;
               break;
            }
         }
         else
         {
            // top placed
            // bottom placed
            if (!topcomp)
            {
               fprintf(logFp, "Component [%s] placed on TOP and ignored in incremental test.\n", compname);
               display_error++;
               break;
            }
         }

         Attrib * attrib = is_attvalue(doc, data->getAttributesRef(), ATT_SUBCLASS, 2);
         CString subClass = get_attvalue_string(doc, attrib);
         attrib = is_attvalue(doc, data->getAttributesRef(), ATT_DEVICETYPE, 2);
         CString deviceType = get_attvalue_string(doc, attrib);         
         CSubclass *sc = m_subclassList.findSubclass(subClass);
         if(sc != NULL && IsValidDeviceTypeForSubClassUse(deviceType))
         {
            for(int elementNum = 0; elementNum < sc->getNumElements(); elementNum++)
            {
               CSubclassElement *el = sc->getElementAt(elementNum);
               if (el != NULL)
               {          
                  CString name = compname + "_" + el->getName();
                  int cindex = get_compindex(name);
                  comparray[cindex]->ingundevicetype = TRI_NOTEST;
                  comparray[cindex]->device = "";
                  comparray[cindex]->value = "1.0";   // default value.
                  comparray[cindex]->ptol = 10.0;  // default value.
                  comparray[cindex]->ntol = 10.0;  // default value.
                  comparray[cindex]->originalName = compname;
                  int pincnt = 0;
                  for (int i = 1; i <= 4; i++) 
                     if(!el->getPinNName(i).IsEmpty()) ++pincnt;
                  comparray[cindex]->pincnt = pincnt;
                  comparray[cindex]->inserttype = insert->getInsertType();
                  comparray[cindex]->bottom = insert->getPlacedBottom();

                  attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_SMDSHAPE,1);
                  if (attrib)
                     comparray[cindex]->smdflag = TRUE;

                  if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1))
                  {
                     comparray[cindex]->device = get_attvalue_string(doc, attrib);
                  }

                  if (el->getDeviceType() != deviceTypeUnknown)
                  {
                     CString deviceType = deviceTypeTagToValueString(el->getDeviceType());

                     attrib = is_attvalue(doc, data->getAttributesRef(), ATT_SUBCLASS, 2);
                     CString subClass = get_attvalue_string(doc, attrib);

                     comparray[cindex]->ingundevicetype = getDeviceIndexByNameAndSubclass(deviceType, subClass);
                     if (comparray[cindex]->pincnt > tridevices[comparray[cindex]->ingundevicetype].maxpinallowed)
                     {
                        fprintf(logFp, "Component [%s] TestClass [%s] has more pins [%d] than allowed [%d] -> ignored.\n",
                           comparray[cindex]->name, tridevices[comparray[cindex]->ingundevicetype].name,
                           comparray[cindex]->pincnt, tridevices[comparray[cindex]->ingundevicetype].maxpinallowed);
                        display_error++;
                        comparray[cindex]->ingundevicetype = TRI_NOTEST;
                     }
                     comparray[cindex]->value = tridevices[comparray[cindex]->ingundevicetype].default_value;  // default value.
                  }
                  else
                  {
                     noclasscomp++;
                     fprintf(logFp, "Component [%s] has unknown %s\n", comparray[cindex]->name, ATT_DEVICETYPE);
                     display_error++;
                  }
                  
                  if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_VALUE, 2))
                  {
                     CString l = get_attvalue_string(doc, attrib);
                     l = l.Trim();

                     if (l.Right(3).CompareNoCase("Ohm") == 0)
                     {
                        l = l.Left(l.GetLength() - 3).Trim();
                     }
                     comparray[cindex]->value = l;
                  }
                  if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TOLERANCE, 2))
                  {
                     CString l = get_attvalue_string(doc, attrib);
                     comparray[cindex]->ptol = atof(l);
                     comparray[cindex]->ntol = atof(l);
                  }
                  if (attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_PLUSTOLERANCE, 2))
                  {
                     CString l = get_attvalue_string(doc, attrib);
                     comparray[cindex]->ptol = atof(l);
                  }
                  if (attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_MINUSTOLERANCE, 2))
                  {
                     CString l = get_attvalue_string(doc, attrib);
                     comparray[cindex]->ntol = atof(l);
                  }

                  if(!el->getValue().IsEmpty())
                  {
                     CString l = el->getValue();
                     l = l.Trim();

                     if (l.Right(3).CompareNoCase("Ohm") == 0)
                     {
                        l = l.Left(l.GetLength() - 3).Trim();
                     }
                     comparray[cindex]->value = l;
                  }                  
                  
                  if (!el->getPlusTol().IsEmpty())
                  {
                     CString l = el->getPlusTol();
                     comparray[cindex]->ptol = atof(l);
                  }
                  
                  if (!el->getMinusTol().IsEmpty())
                  {
                     CString l = el->getMinusTol();
                     comparray[cindex]->ntol = atof(l);
                  }
               }
            }
         }
         else
         {
            int cindex = get_compindex(compname);
            comparray[cindex]->ingundevicetype = TRI_NOTEST;
            comparray[cindex]->device = "";
            comparray[cindex]->value = "1.0";   // default value.
            comparray[cindex]->ptol = 10.0;  // default value.
            comparray[cindex]->ntol = 10.0;  // default value.
            comparray[cindex]->pincnt = get_pincnt(&(block->getDataList()));
            comparray[cindex]->inserttype = insert->getInsertType();
            comparray[cindex]->bottom = insert->getPlacedBottom();
            comparray[cindex]->originalName = compname;

            attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_SMDSHAPE,1);
            if (attrib)
               comparray[cindex]->smdflag = TRUE;

            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1))
            {
               comparray[cindex]->device = get_attvalue_string(doc, attrib);
            }

            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_DEVICETYPE, 2))
            {
               CString deviceType = get_attvalue_string(doc, attrib);

               attrib = is_attvalue(doc, data->getAttributesRef(), ATT_SUBCLASS, 2);
               CString subClass = get_attvalue_string(doc, attrib);

               comparray[cindex]->ingundevicetype = getDeviceIndexByNameAndSubclass(deviceType, subClass);
               if (comparray[cindex]->pincnt > tridevices[comparray[cindex]->ingundevicetype].maxpinallowed)
               {
                  fprintf(logFp, "Component [%s] TestClass [%s] has more pins [%d] than allowed [%d] -> ignored.\n",
                     comparray[cindex]->name, tridevices[comparray[cindex]->ingundevicetype].name,
                     comparray[cindex]->pincnt, tridevices[comparray[cindex]->ingundevicetype].maxpinallowed);
                  display_error++;
                  comparray[cindex]->ingundevicetype = TRI_NOTEST;
               }
               comparray[cindex]->value = tridevices[comparray[cindex]->ingundevicetype].default_value;  // default value.
            }
            else
            {
               noclasscomp++;
               fprintf(logFp, "Component [%s] has no %s\n", comparray[cindex]->name, ATT_DEVICETYPE);
               display_error++;
            }

            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_VALUE, 2))
            {
               CString l = get_attvalue_string(doc, attrib);
               l = l.Trim();

               if (l.Right(3).CompareNoCase("Ohm") == 0)
               {
                  l = l.Left(l.GetLength() - 3).Trim();
               }
               comparray[cindex]->value = l;
            }
            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TOLERANCE, 2))
            {
               CString l = get_attvalue_string(doc, attrib);
               comparray[cindex]->ptol = atof(l);
               comparray[cindex]->ntol = atof(l);
            }
            if (attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_PLUSTOLERANCE, 2))
            {
               CString l = get_attvalue_string(doc, attrib);
               comparray[cindex]->ptol = atof(l);
            }
            if (attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_MINUSTOLERANCE, 2))
            {
               CString l = get_attvalue_string(doc, attrib);
               comparray[cindex]->ntol = atof(l);
            }
         }
      }
      else if (insert->getInsertType() == insertTypeTestAccessPoint)
      {
         TRItestprobeaccesslink *c = new TRItestprobeaccesslink;
         testprobeaccesslinkarray.SetAtGrow(testprobeaccesslinkcnt++, c);  

         Attrib *attrib;
         if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_DDLINK, 0))
         {
            CString l = get_attvalue_string(doc, attrib);
            c->accessentity = data->getEntityNumber();
            c->featurelink = atol(l);
         }
         else
         {
            ErrorMessage("Access without a Datalink", "TRI Output");
         }
      }
      else if (insert->getInsertType() == insertTypeTestProbe)  
      {
         // EXPLODEALL - call DoWriteData() recursively to write embedded entities

         TRItestprobe *c = new TRItestprobe;
         testprobearray.SetAtGrow(testprobecnt++, c);  
         c->geomname = block->getName();

         if (!atoi(insert->getRefname()))
         {
            fprintf(logFp, "Test Probe Name [%s] must be a number [1..n] -> %d\n", insert->getRefname(), ++probenum);
            display_error++;
            c->refnum = probenum;
         }
         else
            c->refnum = atoi(insert->getRefname());
       
         if(IsPowerInjection(doc,data)== true)
            c->refnum = TRI_POWERINJ_NAME;
         
         if(max_refnum < c->refnum && c->refnum != TRI_POWERINJ_NAME)
            max_refnum = c->refnum;

         if (insert->getPlacedBottom())
            c->bottom = 1;
         else
            c->bottom = 0;

         c->x = point2.x;
         c->y = point2.y;
         c->netindex = -1;
         c->probetype = BlockToProbeMap.LookupProbeNumber(block);

			// must have a netname attribute
         Attrib *attrib;
         if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_DDLINK, 0))
         {
            CString l = get_attvalue_string(doc, attrib);
            c->datalink = atol(l);
         }
         else
         {
//                  ErrorMessage("Probe without a Datalink", "TRI Output");
         }

         if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 0)) 
         {
            CString l = get_attvalue_string(doc, attrib);
            c->netindex = get_netnameptr(l);
            if (c->netindex < 0)
            {
               fprintf(logFp, "Testprobe has a netname [%s] which does not appear in the netlist!\n", l);
               display_error++;
            }
            else
            {
               netnamearray[c->netindex]->probecnt++;
            }
         }
		} 
   }
   
   CStringArray probeTable;
   BlockToProbeMap.GetProbeTable(probeTable);
   for(int i = 0; i < probeTable.GetCount(); i++)
   {
      CString lines = probeTable.GetAt(i);
      fprintf(logFp,"%s",lines.GetBuffer(0));
   }

   if (noclasscomp)
   {
      CString tmp;
      tmp.Format("%d components of %d without [%s] found!\nThese components will not be tested.",
         noclasscomp, totcomp, ATT_DEVICETYPE);
      ErrorMessage(tmp, "CompClass Message");
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_format_asc(FileStruct *file, const char *pathname,int page_units)
{
   FILE  *wfp;
   CString  fname;

   fname = pathname;
   fname += "Format.asc";

   if ((wfp = fopen(fname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s",fname);
      ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   // in fabmlib.cpp
   wFABM_Format_ASC(wfp, doc, file, page_units, settingsFile_pageunits);
   fclose(wfp);

   return 1;
}

/*****************************************************************************/
/*
 Test Fixture Nails     874/4274 Selected Drills           15-Sep-1999 16:49
                        874 Nails,  995 Nets               INCH units

Nail         X         Y   Type Grid T/B  Net   Net Name   Virtual Pin/Via

$1        -1.300     0.378   2  B1   (B)  #951  VSDA               PIN J10.12
$2        -1.300     0.468   2  B1   (B)  #903  VBHS               PIN J10.13
$3        -1.300     0.558   2  B1   (B)  #905  VBVS               PIN J10.14
$4        -1.100     0.288   2  A1   (B)  #917  VGREDFF            PIN J10.1
$5        -1.100     0.378   2  A1   (B)  #914  VGGREENFF          PIN J10.2
*/
static int write_nails_asc(FileStruct *file, const char *pathname,int page_units)
{
   FILE  *wfp;
   CString  fname;

   double scale = Units_Factor(page_units, settingsFile_pageunits);

   fname = pathname;
   fname += "Nails.asc";

   if ((wfp = fopen(fname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s",fname);
      ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   CTime t;
   t = t.GetCurrentTime();
   CString UnitName = GetUnitName(settingsFile_pageunits);

   fprintf(wfp,"Test Fixture Nails              Selected Drills            %s\n", t.Format("%d-%b-%Y  %H:%M"));
   fprintf(wfp,"%d Nails, %d Nets	%s units\n", 
               testprobecnt - 1, netnamecnt, (char *)(LPCTSTR)UnitName.MakeUpper());

   fprintf(wfp,"Nail            X         Y   Type Grid T/B  Net   Net Name   Virtual Pin/Via\n");
   fprintf(wfp,"\n");

   unsigned int   netnamelength = 6;
   int   p;

   // here find the longest netname
   for (p=0;p<netnamecnt;p++)
   {
      if (strlen(netnamearray[p]->netname) > netnamelength)
         netnamelength = strlen(netnamearray[p]->netname);
   }

   //sort 0,$1,$2,$3...$max_refnum,TRI_POWERINJ_NAME
   int powering_index = max_refnum + 1;
   max_refnum += 2; 
   naillistarray.SetSize(max_refnum, max_refnum);

   // sort by probe number and net number
   for (p=1;p<testprobecnt;p++)
   {
	   int k;
	   TRInaillinklist *currentnailnode;
	
	   TRInaillinklist *c = new TRInaillinklist;
	   c->nailindex = p;
	   c->netindex = testprobearray[p]->netindex + 1;
	   c->next = NULL;

      k = (testprobearray[p]->refnum == TRI_POWERINJ_NAME)? powering_index:testprobearray[p]->refnum;	   
	   if((currentnailnode = naillistarray.GetAt(k)) == NULL)
	   {
		   naillistarray.SetAtGrow(k,c);
	   }
	   else
	   {
		   if(c->netindex < currentnailnode->netindex)
		   {
			   naillistarray.SetAtGrow(k,c);
			   c->next = currentnailnode;	
		   }
		   else
		   {
			   for(;(currentnailnode->next && c->netindex > currentnailnode->netindex);currentnailnode = currentnailnode->next);
			   c->next = currentnailnode->next;
			   currentnailnode->next = c;
		   }			
	   }
   }

   for (p=0;p<max_refnum;p++)  // start with 1, because 0 is no nail !!!
   {
      CString  n;

      n = "";

	  TRInaillinklist *currentnode = naillistarray.GetAt(p);
	  for(;currentnode;currentnode = currentnode->next)
	  {
		  int k = currentnode->nailindex;

		  if (testprobearray[k]->netindex > -1)
			  n = netnamearray[testprobearray[k]->netindex]->netname;
		  
        if(!IsProbedDiePin(doc->getCamCadData(), testprobearray[k]->datalink))
        {
		      // $1        -1.300     0.378   2  B1   (B)  #951  VSDA               PIN J10.12		  
		      fprintf(wfp,"$%-5d %7.3lf  %7.3lf  %-2d  A1  (%c)   #%-4d   %-*s\n",
				      testprobearray[k]->refnum, 
				      testprobearray[k]->x*scale, testprobearray[k]->y*scale, 
				      testprobearray[k]->probetype,
				      (testprobearray[k]->bottom)?'B':'T',
				      testprobearray[k]->netindex+1,   // netindex starts with 1, 0 is no net
				      netnamelength, n);
		  		  
        }
        else
        {
            fprintf(logFp,"%s - Net: %-*s Probe $%-5d - Skipped Probe for Die pin.\n", fname, netnamelength, n, testprobearray[k]->refnum);
        }
	  }
   }  

   fclose(wfp);

   return 1;
}


/*****************************************************************************/
/*
*/
static int write_pins_asc(FileStruct *file, const char *pathname,int page_units)
{
   FILE  *wfp;
   CString  fname;

   double scale = Units_Factor(page_units, settingsFile_pageunits);

   fname = pathname;
   fname += "Pins.asc";

   if ((wfp = fopen(fname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s",fname);
      ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }


   wFABM_Pins_ASC_Header(wfp, doc, file, page_units, settingsFile_pageunits);
   for (int i=0;i<compcnt;i++)
   {
      // in fabmlib.cpp
      fprintf(wfp,"Part %s (%c)\n", comparray[i]->name, (comparray[i]->bottom)?'B':'T');

      // get all pins of the component

      comppinarray.SetSize(100,100);
      comppincnt = 0;
		int p=0;
      for (p=0;p<pintestcnt;p++)
      {
         if (pintestarray[p]->comp.Compare(comparray[i]->name))   continue;   // if not the same
         comppinarray.SetAtGrow(comppincnt, pintestarray[p]);  
         comppincnt++;  
      }

      // sort comppinarray (TRIpintest) 
      INGUN_SortPinData();
      // wFABM_Pins_ASC_Comp(wfp, doc, file, page_units, UNIT_INCHES, comparray[i]->name);
   
      unsigned int   netnamelength = 6;

      // here find the longest netname
      for (p=0;p<netnamecnt;p++)
      {
         if (strlen(netnamearray[p]->netname) > netnamelength)
            netnamelength = strlen(netnamearray[p]->netname);
      }

      int   layer = 1; // 1 = top, 0 = bottom ??? 
      for (p=0;p<comppincnt;p++)
      {
         // only for debug
         //TRIpintest *c = comppinarray[p];

         CString  n, probename;
         n = "(NC)";
         probename = "";

         if (comppinarray[p]->testprobeptr > 0)
            probename.Format("%d", testprobearray[comppinarray[p]->testprobeptr]->refnum);

         if (comppinarray[p]->netindex > -1)
            n = netnamearray[comppinarray[p]->netindex]->netname;

         fprintf(wfp,"%-4s %-4d %9.3lf %9.3lf %5d    %-*s %s\n",
            comppinarray[p]->pin, p+1, 
            comppinarray[p]->x*scale, comppinarray[p]->y*scale, layer, netnamelength, n, probename);
      }

      // do not delete comppinarray[i], because this is just the pointer to pintestarray
      comppinarray.RemoveAll();
      comppincnt = 0;

      fprintf(wfp,"\n");
   }

   fclose(wfp);

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_ingun_dat(FileStruct *file, const char *pathname, FormatStruct *format)
{
	bool ICT = (format->TRI.datFormat == TriDatFormatICT);

   FILE  *wfp;
   CString  fname;

   fname = pathname;
   fname += "Tri.dat";

   if ((wfp = fopen(fname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s",fname);
      ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   fprintf(wfp,"!Step   Parts-N  Actual-V  LC  Hi-P  Lo-P  G-P1  G-P2  G-P3  G-P4  G-P5  Skip  TYPE");
	if (ICT) fprintf(wfp, "  CanATL  AnaSkip  TJ_Source  TJ-Frequency");
	fprintf(wfp, "\n");

   fprintf(wfp,"!       Meas-V   Stand-V  +Lm%%  -Lm%%  DLY  MODE  AVGE  RPT   SMP  OFFSET  DEV%%");
	if (ICT) fprintf(wfp, "  Ver");
	fprintf(wfp, "\n");

   int   step = 0;
   for (int i=0;i<trioutputcnt;i++)
   {
      if (trioutputarray[i]->onoff == 0)  continue;
      if (!trioutputarray[i]->file.CompareNoCase("DAT"))
      {
         CString  pref = "?";

         int devicetype =  getDeviceIndexByName(trioutputarray[i]->tri_type);

         if (devicetype < 0)  continue;
         pref = tridevices[devicetype].pref;

         fprintf(wfp,"%-4d %-12s %s    A1     %4d  %4d  %4d  %4d  %4d  %4d   %4d  0  %s",
            ++step, trioutputarray[i]->refname,
            trioutputarray[i]->value, 
            testprobearray[trioutputarray[i]->testpin[0]]->refnum, 
            testprobearray[trioutputarray[i]->testpin[1]]->refnum, 
            testprobearray[trioutputarray[i]->testpin[2]]->refnum, 
            testprobearray[trioutputarray[i]->testpin[3]]->refnum, 
            testprobearray[trioutputarray[i]->testpin[4]]->refnum, 
            testprobearray[trioutputarray[i]->testpin[5]]->refnum, 
            testprobearray[trioutputarray[i]->testpin[6]]->refnum, 
            pref);
			if (ICT) fprintf(wfp, "  0  0  0.0  0.000");
			fprintf(wfp, "\n");

         fprintf(wfp,"       4.000     %s   %1.1lf  %1.1lf   0     0     0     0     0   0.0    0.0",
            trioutputarray[i]->value, trioutputarray[i]->ptol, trioutputarray[i]->ntol);
			if (ICT) fprintf(wfp, "  0");
			fprintf(wfp, "\n");
      }
   }

   fclose(wfp);

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_ingun_ic(FileStruct *file, const char *pathname)
{
   FILE  *wfp;    // icn file
   FILE  *ofp;    // icp file
   CString  fname;

   fname = pathname;
   fname += "Tri.icn";

   if ((wfp = fopen(fname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s",fname);
      ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   fname = pathname;
   fname += "Tri.icp";
   if ((ofp = fopen(fname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s",fname);
      ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   //fprintf(wfp,"Num Name Loc Pinn Vccn Vssn Icpx skip LLmt HLmt\n");

   int icp_linecnt = 1;
   int step = 0;
   for (int i=0;i<trioutputcnt;i++)
   {
      if (trioutputarray[i]->onoff == 0)  continue;
      if (!trioutputarray[i]->file.CompareNoCase("ICN"))
      {
         int cptr = get_compindex(trioutputarray[i]->refname);
         if (cptr < 0)  continue;

#ifdef _DEBUG
   TRIComp  *cc = comparray[cptr];
#endif

         //                      sect. totpin  vccpin gndpin    pinindex to icp 
         CString  vccpin , gndpin;
         int      p;

         vccpin = "0";
         gndpin = "0";
         for (p=0;p<pintestcnt;p++)
         {
            //TRIpintest *pp = pintestarray[p];
            if (pintestarray[p]->comp.Compare(trioutputarray[i]->refname)) continue;   // not the same name
            if (pintestarray[p]->netindex < 0)                             continue;
            if (netnamearray[pintestarray[p]->netindex]->groundnet == 1)
               gndpin = pintestarray[p]->pin;
            if (netnamearray[pintestarray[p]->netindex]->groundnet == 2)
               vccpin = pintestarray[p]->pin;
         }

                                                            //     skip
         fprintf(wfp,"%4d %-12s   B2  %4d %4s %4s %6d 0 30 30 0 0\n",
            ++step, trioutputarray[i]->refname,
            comparray[cptr]->pincnt, vccpin, gndpin,
            icp_linecnt);


         comppinarray.SetSize(100,100);
         comppincnt = 0;

         for (p=0;p<pintestcnt;p++)
         {
            if (pintestarray[p]->comp.Compare(comparray[cptr]->name))   continue;   // if not the same
            comppinarray.SetAtGrow(comppincnt, pintestarray[p]);  
            comppincnt++;  
         }

         // sort comppinarray (TRIpintest) 
         INGUN_SortPinData();

         // here now write out all pins
         for (p=0;p<comppincnt;p++)
         {
            //TRIpintest *pp = pintestarray[p];
            int ptr = comppinarray[p]->testprobeptr;
            fprintf(ofp,"ICP / %d = %d\n", icp_linecnt, testprobearray[ptr]->refnum);
            icp_linecnt++;
         }

         // do not delete comppinarray[i], because this is just the pointer to pintestarray
         comppinarray.RemoveAll();
         comppincnt = 0;

      }
   }

   fclose(wfp);
   fclose(ofp);

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_nets_and_parts_asc(FileStruct *file, const char *pathname,int page_units)
{
	// Returns bit flag error code
	// 0 = no error
	// 0x01 = problem with nets.asc
	// 0x02 = problem with parts.asc


	int error = 0;

   try
   {
      CIplWriter ipl(*doc, 
         false, // case 1592, don't use ipl_18xx.out settings 
         NULL,  // log goes to camcad folder
         true   // skip power injection probes
         );
	  
	  double scale = Units_Factor(page_units, settingsFile_pageunits);
	  ipl.setOutputScale(scale);

      if (ipl.writeNets(pathname) != 0)
      {
         // error - can not open/write the file.
         CString fname = pathname;
         fname += "Nets.asc";
         CString tmp;
         tmp.Format("Can not open %s",fname);
         ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
         error |= 0x01;
      }

      if (ipl.writeParts(pathname) != 0)
      {
         // error - can not open/write the file.
         CString fname = pathname;
         fname += "Parts.asc";
         CString tmp;
         tmp.Format("Can not open %s",fname);
         ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
         error |= 0x02;
      }
   }
   catch (CString exception)
   {
      ErrorMessage(exception, "TRI-MDA Error", MB_OK);
   }

   return error;
}

/*****************************************************************************/
/*
   we need to write 
   format->name.dat
   format->name.icn
   format->name.icp
   Format.asc
   Nails.asc
   Pins.asc
*/
static int write_ingun_files(FileStruct *file, const char *pathname, FormatStruct *format, int page_units)
{
   if (!write_format_asc(file, pathname, page_units)) // outline in FABMASTER format
   {
      ErrorMessage("FORMAT.ASC empty -> no primary Boardoutline found!", "TRI-MDA Error");
   }

   if (!write_nails_asc(file, pathname, page_units))  //test probes in FABMASTER format
   {
      ErrorMessage("NAILS.ASC empty -> no test probes found!", "TRI-MDA Error");
   }

   if (!write_pins_asc(file, pathname, page_units))   // pinlist in FABMASTER format
   {
      ErrorMessage("PINS.ASC empty -> Component / Pins found!", "TRI-MDA Error");
   }

   if (!write_ingun_dat(file, pathname, format))  //test pins in FABMASTER format
   {
      ErrorMessage("TRI.DAT empty!", "TRI-MDA Error");
   }

   if (!write_ingun_ic(file, pathname))   //test pins in FABMASTER format
   {
      ErrorMessage("TRI.ICN, TRI.IPC empty!", "TRI-MDA Error");
   }

   if (int errcode = write_nets_and_parts_asc(file, pathname, page_units))   //nets in Teradyne Z1800 format
   {
		if (errcode & 0x01)
			ErrorMessage("TRI.ICN, NETS.ASC empty!", "TRI-MDA Error");

		if (errcode & 0x02)
			ErrorMessage("TRI.ICN, PARTS.ASC empty!", "TRI-MDA Error");
   }

   return 1;
}

//--------------------------------------------------------------
// the path is in pathname, the prefix is in format->filename
// For search: trimda tri-mda tri mda
void TRI_MDA_WriteFiles(const char *pathname, CCEtoODBDoc *Doc,FormatStruct *format, int page_units)
{
   FileStruct *file;
	
   int pcb_found = FALSE;
   doc = Doc;

   display_error = FALSE;

   // Open log file.
   CString localLogFilename;
   logFp = getApp().OpenOperationLogFile("tri-mda.log", localLogFilename);
   if (logFp == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(logFp, "TRI MDA");

   display_error++;

   comparray.SetSize(100,100);
   compcnt = 0;

   trioutputarray.SetSize(100,100);
   trioutputcnt = 0;
 
   polyarray.SetSize(100,100);
   polycnt = 0;

   testprobearray.SetSize(100,100);
   testprobecnt = 0;

   testprobeaccesslinkarray.SetSize(100,100);
   testprobeaccesslinkcnt = 0;

   pinfunctionarray.SetSize(100,100);
   pinfunctioncnt = 0;

   pintestarray.SetSize(100,100);
   pintestcnt = 0;

   netnamearray.SetSize(100,100);
   netnamecnt = 0;

   max_refnum = 0;
   probenum = 0;

   CString settingsFile( getApp().getExportSettingsFilePath("tri-mda.out") );
   {
      CString msg;
      msg.Format("TRI MDA: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_INGUNsettings(settingsFile, page_units);
   m_subclassList.Load(&doc->getCamCadData().getTableList());   

	

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      cur_file = file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         if (!getApp().m_readWriteJob.IsActivated())
            progress->SetStatus("Preparing Data for TRI-MDA output...");

         // write class resistor parts
         // write class capacitor
         generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

         pcb_found = TRUE;
         update_netlist(&file->getNetList());  // fill netnamearray, needed for a netindex

         update_comp_list(file, format->TRI.BottomPopulated, format->TRI.TopPopulated);   // must be before component_list, because it adds the TEST att.
         get_testpinarray(&file->getNetList());   //  loads pintestarray

         // here now assig hi-lo test probes to pins.
         load_comp_triarray(file, format);
         
         // here assign the probes to the pintest array
         for (int p=0;p<pintestcnt;p++)
         {
            pintestarray[p]->testprobeptr = get_testprobeindex(p);
         }

         bool performOutput = true;
         if (!getApp().m_readWriteJob.IsActivated())
         {
            TRIOutput dlg;
            dlg.array = &trioutputarray;
            dlg.count = trioutputcnt;
            if (dlg.DoModal() != IDOK)
            {
               performOutput = false;
               display_error = 0;   // do not show log
               progress->SetStatus("User CANCEL : No output written!...");
            }
         }

         if (performOutput)
         {
            // here now write out all pins
            write_ingun_files(file, pathname, format, page_units);
         }

         break; // do only one pcb file
      } // if pcb file
   }

   int i;

   for (i=0;i<compcnt;i++)
   {
      delete comparray[i];
   }
   comparray.RemoveAll();
   compcnt = 0;

   for (i=0;i<trioutputcnt;i++)
   {
      delete trioutputarray[i];
   }
   trioutputarray.RemoveAll();
   trioutputcnt = 0;

   if (!pcb_found)
   {
      ErrorMessage("No PCB file found !","TRI-MDA Output", MB_OK | MB_ICONHAND);
   }

   polyarray.RemoveAll();

   for (i=0;i<pintestcnt;i++)
   {
      delete pintestarray[i];
   }
   pintestarray.RemoveAll();
   pintestcnt = 0;
   pintestarray.RemoveAll();

   for (i=0;i<netnamecnt;i++)
   {
      delete netnamearray[i];
   }
   netnamearray.RemoveAll();
   netnamecnt = 0;
   netnamearray.RemoveAll();

   for(i=0;i<naillistarray.GetSize();i++)
   {
	  TRInaillinklist *currentnode;
	  TRInaillinklist *tmpnode;
	  for(currentnode = naillistarray[i]; currentnode;currentnode = tmpnode)
	  {
		  tmpnode = currentnode->next;
		  delete currentnode;		
	  }
   }
   max_refnum = 0;
   naillistarray.RemoveAll();

   for (i=0;i<testprobecnt;i++)
   {
      delete testprobearray[i];	  	 
   }
   testprobearray.RemoveAll();
   testprobecnt = 0;

   for (i=0;i<testprobeaccesslinkcnt;i++)
   {
      delete testprobeaccesslinkarray[i];
   }
   testprobeaccesslinkarray.RemoveAll();
   testprobeaccesslinkcnt = 0;

   for (i=0;i<pinfunctioncnt;i++)
   {
      delete pinfunctionarray[i];
   }
   pinfunctionarray.RemoveAll();
   pinfunctioncnt = 0;

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(logFp);

      // We want this to happen only for local log files, not when
      // system log file is in play.
      if (display_error)
         Logreader(localLogFilename);
   }

   fclose(logFp);

   return;
}

/*****************************************************************************/
/*
*/
static char *clean_tol(const char *tol, const char *def)
{
   static CString t; // static only gets init once.

   t = tol;

   if (t.Right(1) == '%')
   {
      CString w = t.Left(strlen(t)-1);
      t = w;
   }

   if (t.Left(1) == '-')
   {
      CString w = t.Right(strlen(t)-1);
      t = w;
   }

   if (t.Left(1) == '+')
   {
      CString w = t.Right(strlen(t)-1);
      t = w;
   }


   // here check for numbers
   int   i;
   int   num = TRUE;

   for (i=0;i<(int)strlen(t);i++)
   {
      if (isalpha(t[i]))
         num = FALSE;
   }

   if (!num)
   {
      fprintf(logFp,"Bad Tolerance value [%s] -> changed to [%s]\n",t, def);
      display_error++;
      t = def;
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static const char *clean_capvalue(CString c, const char *u)
{
   static   CString  t;

   t = c;


   if (c.Right(1) == 'F' || c.Right(1) == 'f')
   {
      t = c.Left(strlen(c)-1);
   }

   // this puts a default ending from takaya.out
   char e = c.GetAt(strlen(c)-1);
   if (!isalpha(e) && e != '*')
   {
      t += u;
   }

   // here check for numbers -just first because it can be .01u
   if (isalpha(t[0]))
   {
      fprintf(logFp,"Bad Capacitor value [%s] -> changed to **\n",t);
      display_error++;
      t = "**";
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static const char *clean_inductorvalue(CString c, const char *u)
{
   static   CString  t;

   t = c;


   if (c.Right(1) == 'H' || c.Right(1) == 'h')
   {
      t = c.Left(strlen(c)-1);
   }

   // this puts a default ending from takaya.out
   char e = c.GetAt(strlen(c)-1);
   if (!isalpha(e) && e != '*')
   {
      t += u;
   }

   // just test first, because it can be .100u
   if (isalpha(t[0]))
   {
      fprintf(logFp,"Bad Inductor value [%s] -> changed to **\n",t);
      display_error++;
      t = "**";
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static const char *clean_zenervoltage(CString c, const char *u)
{
   static   CString  t;

   t = c;

   if (c.Right(1) == 'V' || c.Right(1) == 'v')
   {
      t = c.Left(strlen(c)-1);
   }

   return t.GetBuffer(0);
}

/****************************************************************************/
/*
*/
static int load_INGUNsettings(const CString fname, int pageunits)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   settingsFile_pageunits = pageunits;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp,"TRI-MDA Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".PINFUNCTION"))
         {
            CString  fname, pname;
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            fname = lp;
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            pname = lp;

            TRIpinfunction *p = new TRIpinfunction;
            pinfunctionarray.SetAtGrow(pinfunctioncnt,p);  
            pinfunctioncnt++; 
            p->pinfunction = fname;
            p->pinname = pname;
         }
		 else if(!STRCMPI(lp,".OUTPUT_UNITS"))
		 {
			 CString  unitname;
             if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
             unitname = lp;
			 if((settingsFile_pageunits = unitStringToTag(unitname)) == pageUnitsUndefined)
				settingsFile_pageunits = pageunits;
			
		 }
      }	   
   }
     
   fclose(fp);
   return 1;
}
bool IsValidDeviceTypeForSubClassUse(CString deviceType)
{
   return (!deviceType.CompareNoCase("Resistor_Array") ||
      !deviceType.CompareNoCase("Diode_Array") ||
      !deviceType.CompareNoCase("Capacitor_Array") ||
      !deviceType.CompareNoCase("Filter"));
}

/****************************************************************************/
/*
   end TAKAYA_O.CPP
*/
/****************************************************************************/

