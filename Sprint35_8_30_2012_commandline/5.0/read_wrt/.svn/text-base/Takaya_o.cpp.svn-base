
/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994, 2000. All Rights Reserved.

   testpoints in TAKAYA are TEST_PROBES on a net, not the component pins themselves.

   Takaya tests components, this means if 2 components are parallel, 2 tests have to be performed.

*/

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
#include "drc.h"                                                          
#include "takaya_o.h"
#include "DFT.h"
#include "RealPart.h"
#include "DeviceType.h"
#include "CCEtoODB.h"
#include "xform.h"
#include "dft.h"
#include "CcView.h"
#include "TakayaFPTWrite.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
extern char* testaccesslayers[];
extern char* netstatus[];
extern char* device_subclass[];


int PanReference(CCEtoODBView *view, const char *ref);	// from PanRef.cpp


/* Variable Definition *****************************************************************************/
static CCEtoODBDoc*			doc;
static double					unitsFactor;
static FILE*					flog;
static int						display_error;

static CString					sTopBrdRefPntCommand = "";
static CString					sTopAuxRefPntCommand = "";
static CString					sBotBrdRefPntCommand = "";
static CString					sBotAuxRefPntCommand = "";

static TK_PCBList				pcblist[MAX_BOARDS];
static int						pcblistcnt;

static TKTestkooArray		testkooarray;
static int						testkoocnt;

static TKShortArray			shortarray;
//static int						shortcnt;

static TKNetnameArray		netnamearray(2000);
//static int						netnamecnt;

static TKCompArray			comparray(1000);
//static int						compcnt;

static TKOutputArray			outputarray;
static int						outputcnt;

static TKOutputProbeArray	outputProbeArray;
static int						outputProbeTotalCnt;
static int						outputProbeTopCnt;
static int						outputProbeBottomCnt;

static CTestaccessArray		testaccessarray;
static int						testaccesscnt;

static CPinfunctionArray	pinfunctionarray;
static int						pinfunctioncnt;

static CPintestArray			pintestarray;
static int						pintestcnt;

static FileStruct*			cur_file;
static CMapStringToString	probeUsedForICCapacitanceMap;
static CMapStringToString	probeUsedForICDiodeMap;
static CMapStringToString  compUsedForICOpenTestMap;
static ICOpenTestGeomMap*	openTestGeomMap;
static CIcDiodeCommandMap  icDiodeCommandMap;
static CAptModelMap*			modelMap;

// Variables for setting file
static double					TAKAYA_PROBE_SIZE = 0.1;
static bool						POLARIZED_CAP_VISION = false;
static bool						TANTALUM_CAP_VISION  = false;   
static bool						ALL_COMPONENT_VISION = false;
static bool						SELECT_VISION = false;
static int						EXPORE_MODE = 1;
static CString					APT_MODEL_FILE_NAME = "";
static CString					shortsType;
static CMapStringToString	pinToPinDeviceTypeMap;
static double					pinToPinDistance;
static int						maxVisionCount;
static CString             icDiodePrefix;
static CString             icCapacitorPrefix;
static CString             takayaLibraryDirectory;
static bool                userSpecificCapacitorTest;
static bool                userSpecificDiodeTest;
static bool                isIcOpenDescAttribute;
static CString             icOpenDescription;
static CString             commentAttribute;


static int is_comp_test(const char *n);

static int load_TAKAYAsettings(const CString fname, int pageunits, FormatStruct* format, CCEtoODBDoc* document, CIcDiodeCommandMap& icDiodeCommandMap);
static int do_boardarray(FileStruct *file);


static int get_ProbeNumber(long x, long y, CString netName, BOOL onOff, BOOL topBottom, long  testAccessEntityNumber);
static TK_testkoo* getTestkooByPinNum(CString pinNum);
static CAptModel* getAptModel(CString aptModelName);
static bool updateOutputByAptModel(CAptModel* aptModel, const int filenum, const bool onoff, bool topbottom, TKComp *comp,
						CString value, CString comment, CString location, CString element, const bool version9, 
						const bool ignoreTestedPins, const CString alternaiveTakayaRefname = "");
static int update_TK_output(int filenum, BOOL onoff, BOOL topbottom, const char *refname, const char *takayarefname,
                  const char *val, const char *comment, const char *loc, const char *element, long x1, long y1, 
                  long x2, long y2, int pin1, int pin2, int testProbeIndex1, int testProbeIndex2,
                  CString positiveTol, CString negativeTol, BOOL openTest, BOOL Version9, int deviceType,
						CString mergedStatus, CString mergedValue, const CString testStrategy, bool isICOpenTest,
						int groundNetTestAccessIndex1, int groundNetTestAccessIndex2, int isCompSurfaceBottom, CString subClass = "", TKComp *comp = NULL);


static int PinFunction_QuadBridgeRectifier(const char *compName, int testPinIndex, int *anodePinIndex, int *cathodePinIndex);
static void getComponentXY(const int filenum, TKComp* comp, int testsurface, long& compX, long& compY);

#include "TakayaFPTWrite.h"

/******************************************************************************
* TAKAYA_WriteFiles
*/
void TAKAYA_WriteFiles(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *format, int page_units, BOOL Version9)
{
   // TAKAYA units are in micrometers
   unitsFactor = Units_Factor(page_units, UNIT_MM) * 1000;
   
   TakayaFPTWrite takayaWriter(*Doc, pathname);
   takayaWriter.write();
}


//--------------------------------------------------------------
// converts from actual to TAKAYA units.
static long cnv_units(double x)
{
   long l;
   l = (long)floor(x * unitsFactor + 0.5);
   return l;
}

/******************************************************************************
* get_compindex
*/
static int get_compindex(const char *name)
{
   if (strlen(name) == 0)  
      return -1;


   TKComp* comp;   
   if (!comparray.lookup(name,comp))
   {
      comp = comparray.getDefined(name);
      comp->name = name;
      comp->takayadevicetype = 0;      // none
      comp->takayadevicesubclass = -1;
      comp->smdflag = 0;               // if ATT_SMD was set
      comp->inserttype = 0;            // np stored inserttype;
      comp->pincnt = 0;
		comp->not_implemented_takayaclass = false;
		comp->writeSelectVision = true;		// will be set to false when comp is output as tested
		comp->isICOpenTest = false;
		comp->compInsert = NULL;
      comp->IcOpenDescription.Empty();
      comp->comment.Empty();
   }

   return comp->getIndex();
}

/******************************************************************************
* do_boardarray
*/
static int do_boardarray(FileStruct *file)
{
   DataStruct *data;

   pcblistcnt = 0;

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      data = file->getBlock()->getDataList().GetNext(dataPos);
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCB) 
      {
         if (pcblistcnt < MAX_BOARDS)
         {
            pcblist[pcblistcnt].refname = data->getInsert()->getRefname();              
            pcblist[pcblistcnt].geomnum = data->getInsert()->getBlockNumber();
            pcblist[pcblistcnt].x = data->getInsert()->getOriginX();
            pcblist[pcblistcnt].y = data->getInsert()->getOriginY();             
            pcblist[pcblistcnt].rotation = data->getInsert()->getAngle();               
            pcblist[pcblistcnt].mirror = data->getInsert()->getMirrorFlags();
            pcblistcnt++;
         }
      }
   }

   return 1;
}

/******************************************************************************
* is_pcbcomp
* test if it is a component and not testnone
*/
static int is_pcbcomp(const char *name)
{
   int retval = 0;
   TKComp* comp;
   
   if (comparray.lookup(name,comp))
   {
      if (comp->inserttype == insertTypePcbComponent)
      {
         retval = comp->getIndex() + 1;
      }
   }

   return retval;
}

/******************************************************************************
* get_netnameptr
*/
static int get_netnameptr(const char *name)
{
   int retval = -1;
   TK_netname* net;

   if (netnamearray.lookup(name,net))
   {
      retval = net->getIndex();

   }

   return retval;
}

/******************************************************************************
* get_testprobe
*/
static int get_testprobe(int netindex, int maxhits, int bottom)
{
   int retval = -1;
   int found = -1;
   int cur_targetTypePriority = INT_MAX;
   bool max_hits_detected = false;   // a net maxhits is exceeded if every pin maxhits is exceeded. 
   Bool3Tag accessNotOnSameSurface = boolUnknown;

   if (netindex >= 0)                            
   {
      if (!netnamearray.getAt(netindex)->maxhits_exceeded)   
      {
         for (int i=0; i<testaccesscnt; i++)
         {
            TK_testaccess* testaccess = testaccessarray[i];
            if (testaccess->netindex != netindex)
               continue;
            if (testaccess->bottom != bottom )
            {
               if (accessNotOnSameSurface == boolUnknown)
                  accessNotOnSameSurface = boolTrue;
               continue;
            }

            if (testaccess->usecnt < maxhits)
            {
               accessNotOnSameSurface = boolFalse;
               max_hits_detected = false;
			      if (testaccess->hasTakayaAccessAttrib)
			      {
				      // Found the test access with TAKAYA_ACCESS so stop look
				      retval = i;
				      break;
			      }
               else if (cur_targetTypePriority > testaccess->targetTypePriority)
               {
                  cur_targetTypePriority = testaccess->targetTypePriority;
                  retval = i;
               }
            }
            else
            {
               max_hits_detected = true;
            }
         }
      }
   }

   if (max_hits_detected)
   {
      netnamearray.getAt(netindex)->maxhits_exceeded = TRUE;
      retval = -1;
   }
   else if (accessNotOnSameSurface == boolTrue && retval < 0)
   {
      retval = -4; // -4 indicate that access of the pin is on opposite surface as the testing surface
   }

   return retval;
}

/******************************************************************************
* int update_netlist
*/
static int  update_netlist(CNetList *NetList)
{
   Attrib *a;
   int found = 0;

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

      TK_netname* netname = netnamearray.getDefined(net->getNetName());

      //TK_netname *netname = new TK_netname;
      //netnamearray.SetAtGrow(netnamecnt, netname);  
      //netnamecnt++;  
      netname->netname = net->getNetName();
      netname->maxhits_exceeded = 0;
      netname->probecnt = 0;
      netname->groundnet = 0;

      if (a = is_attvalue(doc, net->getAttributesRef(), ATT_TEST_NET_STATUS, 0)) 
      {
         CString l = get_attvalue_string(doc, a);
         netname->groundnet = !l.CompareNoCase(netstatus[NETSTATUS_GROUND]);
         found++;
      }
   }

   if (!found)
   {
      // check if a ground net was set, otherwise skip this net
      fprintf(flog, "No Net identified with Attribute [%s] Value [%s] Status for IC testing.\n", 
               ATT_TEST_NET_STATUS, netstatus[NETSTATUS_GROUND]);
      display_error++;
      return 0;
   }

   return 1;
}

static TK_pintest* getTKPinTest(const CString compName, const CString pinName)
{
   for (int i=0; i<pintestcnt; i++)
   {
		TK_pintest* pinTest = pintestarray[i];
      if (pinTest->comp.CompareNoCase(compName) == 0 && pinTest->pin.CompareNoCase(pinName) == 0)
      {
			return pinTest;
      }
   }

   return NULL;
}

/******************************************************************************
* update_pintest
*/
static TK_pintest* update_pintest(const char *comp, const char *pin, ETestFlag tested)
{
   TK_pintest *pinTest = getTKPinTest(comp, pin);
   if (pinTest != NULL)
   {
		if ((pinTest->tested == testTop && tested == testBottom) || (pinTest->tested == testBottom && tested == testTop))
			pinTest->tested = testBoth;
		else if (tested != testNone)
			pinTest->tested = tested;
   }
   else
   {
      pinTest = new TK_pintest;
      pintestarray.SetAtGrow(pintestcnt++, pinTest);  
      pinTest->comp = comp;
      pinTest->pin = pin;
      pinTest->tested = tested;
      pinTest->netnameindex = -1;
      pinTest->testprobeptr = -2;
      pinTest->probelayer = 0;  
   }

   return pinTest;
}

/******************************************************************************
* get_testpins
* this function returns all pins of a component in the testkoocnt array.
* the free_testpin function resets this back to zero.
*/
static int get_testpins(const char *refname, CNetList *NetList, int maxhits, int testsurface)
{
	WORD pinMapKey = (WORD)doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
	int unused_pin_found = 0;
   
   POSITION netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

      int netindex = get_netnameptr(net->getNetName());
      int testprobeptr = -2;  // init

      POSITION compPinPos = net->getHeadCompPinPosition();
       while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         if (compPin->getRefDes().CompareNoCase(refname))
            continue;
   
         // find the testprobe assigned to this net.
         if (testprobeptr == -2)
            testprobeptr = get_testprobe(netindex, maxhits, testsurface);

         if (testprobeptr > -1)
         {
            double tx = testaccessarray[testprobeptr]->x;
            if (testsurface)  // bottom
               tx = -tx;

            double ty = testaccessarray[testprobeptr]->y;         
            if (compPin->getPinCoordinatesComplete())
            {
               TK_testkoo *testkoo = new TK_testkoo;
               testkooarray.SetAtGrow(testkoocnt++, testkoo);

               // used the test probe assigned to this net, not the pin itself
               testkoo->x = cnv_units(tx);
               testkoo->y = cnv_units(ty);
               testkoo->pinname = compPin->getPinName();
               testkoo->testprobeptr = testprobeptr;
               testkoo->groundnet = netnamearray.getAt(netindex)->groundnet;
               testkoo->already_done = 0;

					Attrib* attrib = NULL;
					if (compPin->getAttributes() && compPin->getAttributes()->Lookup(pinMapKey, attrib) && attrib != NULL)
						testkoo->pinMap = attrib->getStringValue();
            }
         }

			ETestFlag tested = testsurface==0?testTop:testBottom;
			TK_pintest* pinTest = update_pintest(compPin->getRefDes(), compPin->getPinName(), testNone);
			if (pinTest != NULL)
			{
				pinTest->netnameindex = netindex;
				pinTest->testprobeptr = testprobeptr;

				if (pinTest->tested == testNone)
					 unused_pin_found++;

				if (testprobeptr < 0) 
				{
					// could have been tested on other layers
					//pintestarray[pintestptr]->tested = 0;
				}
				else
				{
					if (testsurface)
						pinTest->probelayer |= 2;
					else
						pinTest->probelayer |= 1;
				}
			}
      }
   }

   return unused_pin_found;
}

/******************************************************************************
* free_testpins
*/
static void free_testpins()
{
   for (int i=0; i<testkoocnt; i++)
      delete testkooarray[i];
   testkoocnt = 0;
}

/******************************************************************************
* char *capacitor_value
*/
static const char *capacitor_value(const char *v)
{
   static CString val1;
   float val;
   char lp[80];

   // you can have .22 uf 
   val1 = "";
   for (int i=0; i<(int)strlen(v); i++)
   {
      if (isspace(v[i]))
         continue;
      val1 += v[i];
   }
   
   // seperate value from unit
   lp[0] = '\0';
   sscanf(val1, "%f%s", &val, lp);

   if (strlen(lp) == 0)
   {
      //val1 += "u";
   }
   else
   {
      int len = strlen(lp);
      CString val2 = lp;
      CString val3 = val1.Left(strlen(val1)-len);
      val1 = val3;
      val1 += (char)tolower(lp[0]);
   }

   return val1.GetBuffer(0);
}

/******************************************************************************
* char *inductor_value
*/
static const char *inductor_value(const char *v)
{
   static CString val1;
   float val;
   char lp[80];

   val1 = v;
   // seperate value from unit
   lp[0] = '\0';
   sscanf(v, "%f%s", &val, lp);

   if (strlen(lp) == 0)
   {
      //val1 += "m";
   }
   else
   {
      int len = strlen(lp);
      CString val2 = lp;
      CString val3 = val1.Left(strlen(v)-len);
      val1 = val3;
      val1 += (char)tolower(lp[0]);
   }

   return val1.GetBuffer(0);
}

/******************************************************************************
* char *zenerdiode_value
*/
static const char *zenerdiode_value(const char *v)
{
   static CString val1;

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

/******************************************************************************
* char *resistor_value
*/
static const char *resistor_value(const char *v)
{
   static CString val1;
   float val;
   char lp[80];

   val1 = v;
   // seperate value from unit
   lp[0] = '\0';
   sscanf(v, "%f%s", &val, lp);

   return val1.GetBuffer(0);
}

/******************************************************************************
* get_pinfunction_ptr
*/
static int get_pinfunction_ptr(const char *pin, const char *function)
{
   for (int i=0; i<pinfunctioncnt; i++)
   {
      TK_pinfunction *pinfunction = pinfunctionarray[i];

      if (!pinfunction->pinname.CompareNoCase(pin))
      {
         if (!STRNICMP(pinfunction->pinfunction, function, strlen(function)))
            return i;
      }
   }

   return -1;
}

//--------------------------------------------------------------
// get one pin name and find which function it belongs.
// than find the next matching pin.
static int pinfunctarray_generic(const char *cmp, const char *p, int *p1, int *p2)
{
   int   found = 0;
   int   t;
   *p1 = -1;
   *p2 = -1;

   if ((*p1 = get_pinfunction_ptr(p, "f")) > -1)
   {
      // pf is the pin function name
      CString  pf = pinfunctionarray[*p1]->pinfunction;

      // now for all pins find the other pin with the same function name.
      for (t =0;t<testkoocnt;t++)
      {     
         if ((*p2 = get_pinfunction_ptr(testkooarray[t]->pinname, "f")) > -1)
         {
            if (pinfunctionarray[*p2]->pinfunction.CompareNoCase(pf) == 0)
            {
               // p2 has the same function as p1
               if (testkooarray[*p1]->already_done && testkooarray[*p2]->already_done)
               {
                  found = 0;
                  *p1 = -1;
                  *p2 = -1;
               }
               else
               {
                  found = 2;
                  if (*p1 > -1)  testkooarray[*p1]->already_done = TRUE;
                  if (*p2 > -1)  testkooarray[*p2]->already_done = TRUE;
                  return found;
               }
            }
         }
         else
         {
            fprintf(flog,"Generic [%s] PinName [%s] not in .PINFUNCTION list\n", 
               cmp, testkooarray[t]->pinname);
            display_error++;
         }
      }
   }

   return found;
}

/******************************************************************************
* PinFunction_QuadBridgeRectifier
*/
static int PinFunction_QuadBridgeRectifier(const char *compName, int testPinIndex, int *anodePinIndex, int *cathodePinIndex)
{
   *anodePinIndex = -1;
   *cathodePinIndex = -1;

   if (testPinIndex < 0)
      return 0;
   
   TK_testkoo *testKoo = testkooarray[testPinIndex];

   if (get_pinfunction_ptr(testKoo->pinname, "COMMON_ANODE") > -1)  // test pin is common_anode
   {
      // loop to find pin that is neither the test pin nor common_cathode pin
      for (int i=0; i<testkoocnt; i++) 
      {     
         TK_testkoo *iKoo = testkooarray[i];

         if (i == testPinIndex)
            continue;

         if (get_pinfunction_ptr(iKoo->pinname, "COMMON_CATHODE") > -1)
            continue;

         if (testKoo->already_done && iKoo->already_done)
            continue;

         // anode & cathode pins are found
         *anodePinIndex = testPinIndex;
         *cathodePinIndex = i;
         testKoo->already_done = TRUE;
         iKoo->already_done = TRUE;
         return 2;
      }
   }
   else if (get_pinfunction_ptr(testKoo->pinname, "COMMON_CATHODE") > -1)  // test pin is common_cathode
   {
      // loop to find pin that is neither the test pin nor common_anode pin
      for (int i=0; i<testkoocnt; i++)
      {     
         TK_testkoo *iKoo = testkooarray[i];

         if (i == testPinIndex)
            continue;

         if (get_pinfunction_ptr(iKoo->pinname, "COMMON_ANODE") > -1)
            continue;

         if (testKoo->already_done && iKoo->already_done)
            continue;

         *anodePinIndex = i;
         *cathodePinIndex = testPinIndex;
         testKoo->already_done = TRUE;
         iKoo->already_done = TRUE;
         return 2;
      }
   }
   else  // test pin is neither common_anode or common_cathode
   {
      // loop to find pin that is either a common_anode or common_cathode pin
      for (int i=0; i<testkoocnt; i++)
      {     
         TK_testkoo *iKoo = testkooarray[i];

         if (i == testPinIndex)
            continue;

         if (get_pinfunction_ptr(iKoo->pinname, "COMMON_ANODE") > -1)
         {
            if (testKoo->already_done && iKoo->already_done)
               continue;

            *anodePinIndex = testPinIndex;
            *cathodePinIndex = i;
            testKoo->already_done = TRUE;
            iKoo->already_done = TRUE;
            return 2;
         }
         else if (get_pinfunction_ptr(iKoo->pinname, "COMMON_CATHODE") > -1)
         {
            if (testKoo->already_done && iKoo->already_done)
               continue;

            *anodePinIndex = i;
            *cathodePinIndex = testPinIndex;
            testKoo->already_done = TRUE;
            iKoo->already_done = TRUE;
            return 2;
         }
      }
   }

   return 0;
}

/******************************************************************************
* pinfunctarray_diode
* get one pin name and find which function it belongs.
* than find the next matching pin.
*/
static int pinfunctarray_diode(const char *compName, const char *pinName, int *anodePinIndex, int *cathodePinIndex)
{
   *anodePinIndex = -1;    // anode
   *cathodePinIndex = -1;  // cathode

   // find the test pin index
   int testPinIndex = -1;
	int i=0;
   for (i=0; i<testkoocnt; i++)
   {     
      if (testkooarray[i]->pinname.CompareNoCase(pinName) == 0)
      {
         testPinIndex = i;
         break;
      }
   }

   if (testPinIndex < 0)  // no test pin with that pinName
      return 0;

   TK_testkoo *testKoo = testkooarray[testPinIndex];
   int anodeFuncIndex = -1;
   int cathodeFuncIndex = -1;

   if ((anodeFuncIndex = get_pinfunction_ptr(pinName, "A")) > -1)
   {
      CString end1 = pinfunctionarray[anodeFuncIndex]->pinfunction.Right(2); // function: ANODE

      // now for all pins find the other pin with the same function name.
      for (i=0; i<testkoocnt; i++)
      {  
         TK_testkoo *iKoo = testkooarray[i];

         if (i == testPinIndex) // do not find the same pin.
            continue;

         if ((cathodeFuncIndex = get_pinfunction_ptr(iKoo->pinname, "C")) > -1)
         {
            CString end2 = pinfunctionarray[cathodeFuncIndex]->pinfunction.Right(2); // function: CATHODE

            if (end1.CompareNoCase(end2))
               continue;

            if (testKoo->already_done && iKoo->already_done) // p2 has the same function as p1
               continue;

            *anodePinIndex = testPinIndex;
            *cathodePinIndex = i;
            testKoo->already_done = TRUE;
            iKoo->already_done = TRUE;
            return 2;
         }
         else
         {
            fprintf(flog, "Diode [%s] PinName [%s] not in .PINFUNCTION list\n", compName, iKoo->pinname);
            display_error++;
         }
      }
   }
   else if ((cathodeFuncIndex = get_pinfunction_ptr(pinName, "C")) > -1)
   {
      CString end1 = pinfunctionarray[cathodeFuncIndex]->pinfunction.Right(2); // function: CATHODE

      // now for all pins find the other pin with the same function name.
      for (i=0; i<testkoocnt; i++)
      {  
         TK_testkoo *iKoo = testkooarray[i];

         if (i == testPinIndex) // do not find the same pin.
            continue;

         if ((anodeFuncIndex = get_pinfunction_ptr(testKoo->pinname, "A")) > -1)
         {
            CString end2 = pinfunctionarray[anodeFuncIndex]->pinfunction.Right(2); // function: ANODE

            if (end1.CompareNoCase(end2))
               continue;

            if (testKoo->already_done && iKoo->already_done) // p2 has the same function as p1
               continue;

            *anodePinIndex = i;
            *cathodePinIndex = testPinIndex;
            testKoo->already_done = TRUE;
            iKoo->already_done = TRUE;
            return 2;
         }
         else
         {
            fprintf(flog, "Diode [%s] PinName [%s] not in .PINFUNCTION list\n", compName, iKoo->pinname);
            display_error++;
         }
      }
   }

   return 0;
}

/******************************************************************************
* sort_baseemittercollector
*/
static int sort_baseemittercollector(const char *cmp, int *base, int *emitter, int *collector)
{
   int found = 0;
   int b = -1;
   int e = -1;
   int c = -1;
   
   for (int t = 0; t < testkoocnt; t ++)
   {
      found = 0;
      TK_testkoo *testKoo = testkooarray[t];
      
      for (int i = 0; i < pinfunctioncnt; i++)
      {
         TK_pinfunction *pinFunction = pinfunctionarray[i];

         if (testKoo->pinname.CompareNoCase(pinFunction->pinname) == 0)
         {
            if (!pinFunction->pinfunction.CompareNoCase("BASE"))        b = t, found++;
            if (!pinFunction->pinfunction.CompareNoCase("EMITTER"))     e = t, found++;
            if (!pinFunction->pinfunction.CompareNoCase("COLLECTOR"))   c = t, found++;
         }
      }

      if (!found)
      {
         fprintf(flog, " Transistor [%s] PinName [%s] not in .PINFUNCTION list\n", cmp, testKoo->pinname);
         display_error++;
      }
   }

   *base = b;
   *emitter = e;
   *collector = c;

   return found;
}

/******************************************************************************
* get_groundpin
*/
static int get_groundpin(long *gx, long *gy, int maxhits, int testSurface)
{
   int cur_targetTypePriority = INT_MAX;
   int groundpin = -1;

   for (int i=0; i<testaccesscnt; i++)
   {
      TK_testaccess *testAccess = testaccessarray[i];
      if (testAccess->groundnet && (testAccess->usecnt < maxhits) && testAccess->bottom == testSurface)
      {
			if (testAccess->hasTakayaAccessAttrib)
			{
				// Found the test access with TAKAYA_ACCESS so stop look
            *gx = cnv_units(testAccess->x);
            *gy = cnv_units(testAccess->y);
				groundpin = i;
				break;
			}
         else if (cur_targetTypePriority > testAccess->targetTypePriority)
         {
            *gx = cnv_units(testAccess->x);
            *gy = cnv_units(testAccess->y);
            cur_targetTypePriority = testAccess->targetTypePriority;
            groundpin = i;
         }
      }
   }

   return groundpin;
}

/******************************************************************************
* getTwoPairOfGroundPin
*/
static int getTwoPairOfGroundPin(int& groundPinIndex1, int& groundPinIndex2, int maxhits, int isCompSufaceBottom)
{
   int cur_targetTypePriority = INT_MAX;

	groundPinIndex1 = -1;
	groundPinIndex2 = -1;

	// Find the first gound pin
   for (int i=0; i<testaccesscnt; i++)
   {
      TK_testaccess *tmpTestAccess = testaccessarray[i];
      if (tmpTestAccess->groundnet && tmpTestAccess->bottom == isCompSufaceBottom && tmpTestAccess->usecnt < maxhits)
		{
			if (tmpTestAccess->hasTakayaAccessAttrib)
			{
				// Found the test access with TAKAYA_ACCESS so stop look
				groundPinIndex1 = i;
				break;
			}
			else if (cur_targetTypePriority > tmpTestAccess->targetTypePriority)
			{
				cur_targetTypePriority = tmpTestAccess->targetTypePriority;
				groundPinIndex1 = i;
			}
		}
   }

	//if (groundPinIndex1 == -1)
	//	return 0;		

	CPoint2d groundPinOrigin1;
	if (groundPinIndex1 != -1)
	{
		TK_testaccess *tmpTestAccess = testaccessarray[groundPinIndex1];
		groundPinOrigin1.x = tmpTestAccess->x;
		groundPinOrigin1.y = tmpTestAccess->y;			
	}
	else
	{
		return 0;
	}


	// Find the second ground pin
	double distance = 0.0;
	cur_targetTypePriority = INT_MAX;
   for (int i=0; i<testaccesscnt; i++)
   {
      TK_testaccess *tmpTestAccess = testaccessarray[i];
      if (tmpTestAccess->groundnet && tmpTestAccess->bottom == isCompSufaceBottom && tmpTestAccess->usecnt < maxhits)
		{
			if (tmpTestAccess->hasTakayaAccessAttrib)
			{
				// Found the test access with TAKAYA_ACCESS so stop look
				groundPinIndex2 = i;
				break;
			}
			else if (cur_targetTypePriority > tmpTestAccess->targetTypePriority)
			{
				if (groundPinIndex1 == i)
					continue;

				double tmpDistance = groundPinOrigin1.distance(CPoint2d(tmpTestAccess->x, tmpTestAccess->y));
				if (tmpDistance > distance)
				{
					cur_targetTypePriority = tmpTestAccess->targetTypePriority;
					distance = tmpDistance;
					groundPinIndex2 = i;
				}
			}
		}
   }

	if (groundPinIndex1 > -1 && groundPinIndex2 > -1)
	{
		return 1;
	}
	else
	{
		groundPinIndex1 = -1;
		groundPinIndex2 = -1;
	   return 0;
	}
}

/******************************************************************************
* get_baseemittercollector
*/
static int get_baseemittercollector(const char *compname, FileStruct *f, 
                                    int *base, int *emitter, int *collector)
{
   //TypeStruct *typ;
   //POSITION typPos;
   int      found = 0;
   Attrib   *a;

/* 
   typPos = TypeList->GetHeadPosition();
   while (typPos != NULL)
   {                                         
      typ = TypeList->GetNext(typPos);
      if (typ->Name.CompareNoCase(devicename))  continue;
      // here found
      if (a =  is_attvalue(doc, typ->getAttributesRef(), ATT_PINFUNCTION, 2))
      {
         CString l = get_attvalue_string(doc, a);
         fprintf(flog, "Pin function found\n");
         display_error++;
      }
   }
*/

   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   netPos = f->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = f->getNetList().GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      int netindex = get_netnameptr(net->getNetName());

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         for (int t = 0; t < testkoocnt; t ++)
         {
            if (!compPin->getRefDes().CompareNoCase(compname) && !compPin->getPinName().CompareNoCase(testkooarray[t]->pinname))
            {
               // pin found
               if (a =  is_attvalue(doc, compPin->getAttributesRef(), ATT_PINFUNCTION, 2))
               {
                  CString l = get_attvalue_string(doc, a);
                  if (!l.CompareNoCase("BASE")) 
                  {
                     *base = t;
                  }
                  else
                  if (!l.CompareNoCase("EMITTER")) 
                  {
                     *emitter = t;
                  }
                  else
                  if (!l.CompareNoCase("COLLECTOR"))  
                  {
                     *collector = t;
                  }
               }
            }
         }
      }
   }

   if (*base > -1 && *emitter > -1 && *collector > -1)
      found = TRUE;

   return found;
}

/******************************************************************************
* getAptModel

	Find the Apt Model being reference by the aptModelName.  The variable aptModelName
	is from the attribute ATT_SUBCLASS. 
*/
static CAptModel* getAptModel(CString aptModelName)
{
	if (modelMap == NULL)
		return NULL;

	CAptModel* aptModel = NULL;
	if (!modelMap->Lookup(aptModelName, aptModel) || aptModel == NULL)
		return NULL;

	return aptModel;
}

/******************************************************************************
* updateOutputByAptModel

	If there is a Apt Model then use that to write out the test(s) for the 
	specified components.  This function is being called from do_Resistor, do_IC,..etc.

*/
static bool updateOutputByAptModel(CAptModel* aptModel, const int filenum, const bool onoff, bool isbottom, TKComp *comp, 
											  CString value, CString comment, CString location, CString element, const bool version9,
											  const bool ignoreTestedPins, const CString alternaiveTakayaRefname)
{
   bool compBottom = comp->bottom==1;
	if (aptModel == NULL)// || compBottom != isbottom)
		return false;

	POSITION pos = aptModel->GetHeadPosition();
	while (pos)
	{
		CAptTest* aptTest = aptModel->GetNext(pos);
		if (aptTest == NULL)
			continue;

		// Get the cordinate for the pins and the probes for those pins
		TK_testkoo* testkoo1 = getTestkooByPinNum(aptTest->GetPinNum1());
      if (testkoo1 == NULL && !aptTest->GetPinNum1().IsEmpty())
      {
         if (comp->pincnt == testkoocnt)
         {
            fprintf(flog, "Comp [%s] has no pin name matched in Model [%s] Pin [%s].\n", comp->name, aptModel->GetName(), aptTest->GetPinNum1());
         }
         else
         {
            TK_pintest* pintest = getTKPinTest(comp->name, aptTest->GetPinNum1());
            if (pintest != NULL && pintest->testprobeptr == -4)
            {
               fprintf(flog, "Probe access for Comp [%s] Pin [%s] on opposite side of board to Pin 2.\n", comp->name, aptTest->GetPinNum1());
            }
         }
      }

		TK_testkoo* testkoo2 = getTestkooByPinNum(aptTest->GetPinNum2());
      if (testkoo2 == NULL && !aptTest->GetPinNum2().IsEmpty())
      {
         if (comp->pincnt == testkoocnt)
         {
            fprintf(flog, "Comp [%s] has no pin name matched in Model [%s] Pin [%s].\n", comp->name, aptModel->GetName(), aptTest->GetPinNum2());
         }
         else
         {
            TK_pintest* pintest = getTKPinTest(comp->name, aptTest->GetPinNum2());
            if (pintest != NULL && pintest->testprobeptr == -4)
            {
               fprintf(flog, "Probe access for Comp [%s] Pin [%s] on opposite side of board to Pin 1.\n", comp->name, aptTest->GetPinNum2());
            }
         }
      }

		TK_testkoo* testkoo3 = getTestkooByPinNum(aptTest->GetPinNum3());
      if (testkoo3 == NULL && !aptTest->GetPinNum3().IsEmpty())
      {
         if (comp->pincnt == testkoocnt)
         {
            fprintf(flog, "Comp [%s] has no pin name matched in Model [%s] Pin [%s].\n", comp->name, aptModel->GetName(), aptTest->GetPinNum3());
         }
         else
         {
            TK_pintest* pintest = getTKPinTest(comp->name, aptTest->GetPinNum3());
            if (pintest != NULL && pintest->testprobeptr == -4)
            {
               fprintf(flog, "Probe access for Comp [%s] Pin [%s] on opposite side of board to Pin 4.\n", comp->name, aptTest->GetPinNum3());
            }
         }
      }

		TK_testkoo* testkoo4 = getTestkooByPinNum(aptTest->GetPinNum4());
      if (testkoo4 == NULL && !aptTest->GetPinNum4().IsEmpty())
      {
         if (comp->pincnt == testkoocnt)
         {
            fprintf(flog, "Comp [%s] has no pin name matched in Model [%s] Pin [%s].\n", comp->name, aptModel->GetName(), aptTest->GetPinNum4());
         }
         else
         {
            TK_pintest* pintest = getTKPinTest(comp->name, aptTest->GetPinNum4());
            if (pintest != NULL && pintest->testprobeptr == -4)
            {
               fprintf(flog, "Probe access for Comp [%s] Pin [%s] on opposite side of board to Pin 3.\n", comp->name, aptTest->GetPinNum4());
            }
         }
      }

		if (testkoo1 == NULL || testkoo2 == NULL)
         continue;


      // Make sure the same comppin is not already write to a file
      ETestFlag tested = isbottom?testBottom:testTop;
	   ETestFlag p1tested = testNone;
	   ETestFlag p2tested = testNone;

      for (int i=0; i<pintestcnt; i++)
      {
			TK_pintest* pinTest = pintestarray[i];
         if (pinTest->comp.CompareNoCase(comp->name) == 0 && pinTest->pin.CompareNoCase(testkoo1->pinname) == 0)
			{
				p1tested = pinTest->tested;
				break;
			}
      }

      for (int i=0; i<pintestcnt; i++)
      {
			TK_pintest* pinTest = pintestarray[i];
         if (pinTest->comp.CompareNoCase(comp->name) == 0 && pinTest->pin.CompareNoCase(testkoo2->pinname) == 0)
			{
				p2tested = pinTest->tested;
				break;
			}
      }

	   if (p1tested != testNone && p2tested != testNone && p1tested != tested)
		   continue;
	   else if (p1tested != testNone && p1tested != tested)
		   continue;
	   else if (p2tested != testNone && p2tested != tested)
		   continue;

		TK_testaccess *testaccess1 =  testaccessarray[testkoo1->testprobeptr];
		TK_testaccess *testaccess2 =  testaccessarray[testkoo2->testprobeptr];
      TK_testaccess *testaccess3 =  testkoo3!=NULL?testaccessarray[testkoo3->testprobeptr]:NULL;
		TK_testaccess *testaccess4 =  testkoo4!=NULL?testaccessarray[testkoo4->testprobeptr]:NULL;
		if ((testaccess1 == NULL || testaccess2 == NULL) ||
          (testaccess1->bottom != testaccess2->bottom))
			continue;

      if (testaccess1 == testaccess2)
      {
			fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n", 
					comp->name, testkoo1->pinname, testkoo2->pinname);
			display_error++;
         continue;
      }

		TK_netname *netname1 = netnamearray.getAt(testaccess1->netindex);
		TK_netname *netname2 = netnamearray.getAt(testaccess2->netindex);
      TK_netname *netname3 = testaccess3!=NULL?netnamearray.getAt(testaccess3->netindex):NULL;
      TK_netname *netname4 = testaccess4!=NULL?netnamearray.getAt(testaccess4->netindex):NULL;
		if (netname1 == NULL || netname2 == NULL)
			continue;

		// get the value, comment, location, element, option from aptTet if they are there
		// otherwise, use whatever that is passed it

      CString refname = aptTest->GetRefdes();
      refname.Replace(TK_REF_REPLACEMENT, alternaiveTakayaRefname.IsEmpty()?comp->takayarefname:alternaiveTakayaRefname);

		if (!aptTest->GetValue().IsEmpty())
			value = aptTest->GetValue();

		if (!aptTest->GetComment().IsEmpty())
		{
			comment = aptTest->GetComment();

         if (comment.CompareNoCase(TK_PART_REPLACEMENT) == 0)
         {
            comment = comp->partNumber;
         }
         else
         {
			   comment.Replace(TK_REF_REPLACEMENT, comp->takayarefname);
         }
		}

		if (!aptTest->GetLocation().IsEmpty())
			location = aptTest->GetLocation();

		if (aptTest->GetElement() != '\0')
			element = aptTest->GetElement();

		CString option = "";
		if (version9)
		{
			option = aptTest->GetOption();
			if (!option.IsEmpty())
			{
				option.Replace(TK_PTOL_REPLACEMENT, comp->positiveTol);
				option.Replace(TK_NTOL_REPLACEMENT, comp->negativeTol);
			}
		}

      
      long compX = 0;
      long compY = 0;
      getComponentXY(filenum, comp, isbottom, compX, compY);

      if (testkoo1 != NULL && testkoo2 != NULL)
      {
		   long x1 = testkoo1->x;
		   long y1 = testkoo1->y;
		   long x2 = testkoo2->x;
		   long y2 = testkoo2->y;

		   // x1, y1, x2, y2 are absolute takaya output. bottom is mirrored -x;
		   // also these koos are relative to the file origin.
		   if (isbottom) // unmirror koos
		   {
			   x1 = -x1;
			   x2 = -x2;
		   }

		   // this is file offset
		   double x = x1 / unitsFactor;
		   double y = y1 / unitsFactor;
		   if (pcblist[filenum].mirror)
			   x = -x;

		   // filenum pointers into the offset
		   Rotate(x, y, RadToDeg(pcblist[filenum].rotation), &x, &y);
		   x1 = cnv_units(x + pcblist[filenum].x);
		   y1 = cnv_units(y + pcblist[filenum].y);

		   // this is file offset
		   x = x2 / unitsFactor;
		   y = y2 / unitsFactor;
		   if (pcblist[filenum].mirror)
			   x = -x;

		   // filenum pointers into the offset
		   Rotate(x, y, RadToDeg(pcblist[filenum].rotation), &x, &y);
		   x2 = cnv_units(x + pcblist[filenum].x);
		   y2 = cnv_units(y + pcblist[filenum].y);

		   if (pcblist[filenum].mirror)  
			   isbottom = !isbottom; // switch top and bottom

		   if (isbottom) // now mirror them again.
		   {
			   x1 = -x1;
			   x2 = -x2;
		   }

		   TKoutput *output = new TKoutput;
		   outputarray.SetAtGrow(outputcnt++, output);  
		   output->filenumber = filenum;
		   output->onoff = onoff;
		   output->topbottom = isbottom;    // 0 = top, 1 = bottom
		   output->refname = comp->name;
		   output->tkrefname = refname;       // alternaiveTakayaRefname.IsEmpty()?comp->takayarefname:alternaiveTakayaRefname;
         output->deviceType = aptTest->GetDeviceType(); //!=deviceTypeUnknown?aptTest->GetDeviceType():comp->takayadevicetype;
		   output->subClass = comp->subClass;
		   output->comment = comment;
		   output->location = location;
		   output->element = element;
		   output->option = option;
         output->compX = compX;
         output->compY = compY;
		   output->mergedStatus = comp->mergedStatus;
		   output->testStrategy = comp->testStrategy;
		   output->writeSelectVision = false;
		   output->isICOpenTest = false;
		   output->gNetName1 = "";
		   output->gNetName2 = "";
		   output->gx1 = 0;
		   output->gy1 = 0;
		   output->gx2 = 0;
		   output->gy2 = 0;
		   output->compSurfaceBottom = comp->bottom;
		   output->testAccessEntityNumber1 = 0;
		   output->testAccessEntityNumber2 = 0;	
		   output->gTestAccessEntityNubmer1 = 0;
		   output->gTestAccessEntityNubmer2 = 0;
		   output->value = value;
		   output->mergedValue = comp->mergedValue;
		   output->x1 = x1;
		   output->y1 = y1;
		   output->x2 = x2;
		   output->y2 = y2;
		   output->netName1 = netname1->netname;
		   output->netName2 = netname2->netname;
		   output->testAccessEntityNumber1 = testaccess1->entityNumber;
		   output->testAccessEntityNumber2 = testaccess2->entityNumber;	

		   testaccessarray[testkoo1->testprobeptr]->usecnt++;
		   update_pintest(comp->name, testkoo1->pinname, isbottom==false?testTop:testBottom);
         testaccessarray[testkoo2->testprobeptr]->usecnt++;
		   update_pintest(comp->name, testkoo2->pinname, isbottom==false?testTop:testBottom);
      }

		TKoutput* output2 = NULL;
      if (testkoo3 != NULL && testaccess3 != NULL && netname3 != NULL)
      {
		   long x1 = testkoo3->x;
		   long y1 = testkoo3->y;

		   // x1, y1, x2, y2 are absolute takaya output. bottom is mirrored -x;
		   // also these koos are relative to the file origin.
		   if (isbottom) // unmirror koos
			   x1 = -x1;

		   // this is file offset
		   double x = x1 / unitsFactor;
		   double y = y1 / unitsFactor;
		   if (pcblist[filenum].mirror)
			   x = -x;

		   // filenum pointers into the offset
		   Rotate(x, y, RadToDeg(pcblist[filenum].rotation), &x, &y);
		   x1 = cnv_units(x + pcblist[filenum].x);
		   y1 = cnv_units(y + pcblist[filenum].y);

		   if (pcblist[filenum].mirror)  
			   isbottom = !isbottom; // switch top and bottom

		   if (isbottom) // now mirror them again.
			   x1 = -x1;

		   output2 = new TKoutput;
		   outputarray.SetAtGrow(outputcnt++, output2);  
		   output2->filenumber = filenum;
		   output2->onoff = onoff;
		   output2->topbottom = isbottom;   // 0 = top, 1 = bottom
		   output2->refname = comp->name;
		   output2->tkrefname = refname;    // alternaiveTakayaRefname.IsEmpty()?comp->takayarefname:alternaiveTakayaRefname;
         output2->deviceType = aptTest->GetDeviceType(); //!=deviceTypeUnknown?aptTest->GetDeviceType():comp->takayadevicetype;		
		   output2->subClass = comp->subClass;
		   output2->comment = comment;
		   output2->location = location;
		   output2->element = element;
		   output2->option = option;
         output2->compX = compX;
         output2->compY = compY;
		   output2->mergedStatus = comp->mergedStatus;
		   output2->testStrategy = comp->testStrategy;
		   output2->writeSelectVision = false;
		   output2->isICOpenTest = false;
		   output2->gNetName1 = "";
		   output2->gNetName2 = "";
		   output2->gx1 = 0;
		   output2->gy1 = 0;
		   output2->gx2 = 0;
		   output2->gy2 = 0;
		   output2->compSurfaceBottom = comp->bottom;
		   output2->testAccessEntityNumber1 = 0;
		   output2->testAccessEntityNumber2 = 0;	
		   output2->gTestAccessEntityNubmer1 = 0;
		   output2->gTestAccessEntityNubmer2 = 0;
		   output2->value = value;
		   output2->mergedValue = comp->mergedValue;

		   output2->x1 = x1;
		   output2->y1 = y1;
		   output2->x2 = 0;
		   output2->y2 = 0;
		   output2->netName1 = netname3->netname;
		   output2->netName2 = "";
		   output2->testAccessEntityNumber1 = testaccess3->entityNumber;
		   output2->testAccessEntityNumber2 = 0;	

		   testaccessarray[testkoo3->testprobeptr]->usecnt++;
		   update_pintest(comp->name, testkoo3->pinname, isbottom==false?testTop:testBottom);
      }

      if (testkoo4 != NULL && testaccess4 != NULL && netname4 != NULL && output2 != NULL)
      {
		   long x2 = testkoo4->x;
		   long y2 = testkoo4->y;

		   // x1, y1, x2, y2 are absolute takaya output. bottom is mirrored -x;
		   // also these koos are relative to the file origin.
		   if (isbottom) // unmirror koos
			   x2 = -x2;

		   // this is file offset
		   double x = x2 / unitsFactor;
		   double y = y2 / unitsFactor;
		   if (pcblist[filenum].mirror)
			   x = -x;

		   // filenum pointers into the offset
		   Rotate(x, y, RadToDeg(pcblist[filenum].rotation), &x, &y);
		   x2 = cnv_units(x + pcblist[filenum].x);
		   y2 = cnv_units(y + pcblist[filenum].y);

		   if (pcblist[filenum].mirror)  
			   isbottom = !isbottom; // switch top and bottom

		   if (isbottom) // now mirror them again.
			   x2 = -x2;

         output2->x2 = x2;
		   output2->y2 = y2;
		   output2->netName2 = netname4->netname;
		   output2->testAccessEntityNumber2 = testaccess4->entityNumber;

		   testaccessarray[testkoo4->testprobeptr]->usecnt++;
		   update_pintest(comp->name, testkoo4->pinname, isbottom==false?testTop:testBottom);
      }
	}

	return true;
}

/******************************************************************************
* getTestkooByPinNum
*/
static TK_testkoo* getTestkooByPinNum(CString pinNum)
{
	//CString tmpPinName = "";
	//tmpPinName.Format("%d", pinNum);
	for (int i=0; i<testkoocnt; i++)
	{
		TK_testkoo* testkoo = testkooarray[i];
		if (testkoo->pinname.CompareNoCase(pinNum) == 0)
			return testkoo;
	}

	return NULL;
}

/******************************************************************************
* update_TK_output
  return  0 = output array
         -1 = duplicated test
         -2 = same probe test ???
*/
static int update_TK_output(int filenum, BOOL onoff, BOOL topbottom, const char *refname, const char *takayarefname,
                  const char *val, const char *comment, const char *loc, const char *element, long x1, long y1, 
                  long x2, long y2, int pin1, int pin2, int testProbeIndex1, int testProbeIndex2,
                  CString positiveTol, CString negativeTol, BOOL openTest, BOOL Version9, int deviceType,
						CString mergedStatus, CString mergedValue, const CString testStrategy, bool isICOpenTest,
						int groundNetTestAccessIndex1, int groundNetTestAccessIndex2, int isCompSurfaceBottom, CString subClass, TKComp *comp)
{
	if (x1 == x2 && y1 == y2)
      return -2; // two pins use the same probe ????


	// 1st - Check to make sure that pin1 and pin2 is not the same probe
	if (pin1 > -1 && pin2 > -1)
   {
		TK_testkoo* testkoo1 = testkooarray[pin1];
		TK_testkoo* testkoo2 = testkooarray[pin2];

      if (testkoo1->testprobeptr == testkoo2->testprobeptr) 
         return -2; // use the same probe
	}


	ETestFlag tested = topbottom==FALSE?testTop:testBottom;
	ETestFlag p1tested = testNone;
	ETestFlag p2tested = testNone;

	if (pin1 > -1)
   {
		TK_testkoo* testkoo1 = testkooarray[pin1];
      for (int i=0; i<pintestcnt; i++)
      {
			TK_pintest* pinTest = pintestarray[i];
         if (pinTest->comp.CompareNoCase(refname) == 0 && pinTest->pin.CompareNoCase(testkoo1->pinname) == 0)
			{
				p1tested = pinTest->tested;
				break;
			}
      }
   }

   if (pin2 > -1)
   {
		TK_testkoo* testkoo2 = testkooarray[pin2];
      for (int i=0; i<pintestcnt; i++)
      {
			TK_pintest* pinTest = pintestarray[i];
         if (pinTest->comp.CompareNoCase(refname) == 0 && pinTest->pin.CompareNoCase(testkoo2->pinname) == 0)
			{
				p2tested = pinTest->tested;
				break;
			}
      }
   }

	if ((pin1 > -1 && pin2 > -1) && (p1tested != testNone && p2tested != testNone))
		return -1;
	else
	if (pin1 > -1 && p1tested != testNone && p1tested != tested)
		return -1;
	else
	if (pin2 > -1 && p2tested != testNone && p2tested != tested)
		return -1;


	// If it is ICOpenTest make sure testprobe is on the same surface as component
	if (isICOpenTest && groundNetTestAccessIndex1 > -1 && groundNetTestAccessIndex2 > -1)
	{
		TK_testaccess *testaccess1 =  testaccessarray[groundNetTestAccessIndex1];
		TK_testaccess *testaccess2 =  testaccessarray[groundNetTestAccessIndex2];

		if (testaccess1->bottom != isCompSurfaceBottom || testaccess2->bottom != isCompSurfaceBottom)
			return -1;
	}

   long compX = 0;
   long compY = 0;
   if (comp != NULL)
   {
      getComponentXY(filenum, comp, topbottom, compX, compY);
   }

   // x1, y1, x2, y2 are absolute takaya output. bottom is mirrored -x;
   // also these koos are relative to the file origin.
   if (topbottom) // unmirror koos
   {
      x1 = -x1;
      x2 = -x2;
   }

   // this is file offset
   double x = x1 / unitsFactor;
   double y = y1 / unitsFactor;
   if (pcblist[filenum].mirror)
      x = -x;

   // filenum pointers into the offset
   Rotate(x, y, RadToDeg(pcblist[filenum].rotation), &x, &y);
   x1 = cnv_units(x + pcblist[filenum].x);
   y1 = cnv_units(y + pcblist[filenum].y);

   // this is file offset
   x = x2 / unitsFactor;
   y = y2 / unitsFactor;
   if (pcblist[filenum].mirror)
      x = -x;

   // filenum pointers into the offset
   Rotate(x, y, RadToDeg(pcblist[filenum].rotation), &x, &y);
   x2 = cnv_units(x + pcblist[filenum].x);
   y2 = cnv_units(y + pcblist[filenum].y);

   if (pcblist[filenum].mirror)  
      topbottom = (topbottom + 1) % 2; // switch top and bottom

   if (topbottom) // now mirror them again.
   {
      x1 = -x1;
      x2 = -x2;
   }

   TKoutput *output = new TKoutput;
   outputarray.SetAtGrow(outputcnt++, output);  
   output->filenumber = filenum;
   output->onoff = onoff;
   output->topbottom = topbottom;   // 0 = top, 1 = bottom
   output->refname = refname;
   output->tkrefname = takayarefname;
	output->deviceType = deviceType;
	output->subClass = subClass;
   output->comment = comment;
   output->location = loc;
   output->element = element;
   output->compX = compX;
   output->compY = compY;
   output->x1 = x1;
   output->y1 = y1;
   output->x2 = x2;
   output->y2 = y2;
	output->mergedStatus = mergedStatus;
	output->testStrategy = testStrategy;
	output->writeSelectVision = false;
	output->isICOpenTest = false;
	output->gNetName1 = "";
	output->gNetName2 = "";
	output->gx1 = 0;
	output->gy1 = 0;
	output->gx2 = 0;
	output->gy2 = 0;
	output->compSurfaceBottom = isCompSurfaceBottom;
	output->testAccessEntityNumber1 = 0;
	output->testAccessEntityNumber2 = 0;	
	output->gTestAccessEntityNubmer1 = 0;
	output->gTestAccessEntityNubmer2 = 0;

	if (deviceType == deviceTypeIC || deviceType == deviceTypeICDigital || 
		 deviceType == deviceTypeICLinear || deviceType == deviceTypeCapacitorPolarized ||
		 deviceType == deviceTypeCapacitorTantalum )
	{
		// If it is an IC open test then need to get the ground pin locations and netnames
		if (isICOpenTest && groundNetTestAccessIndex1 > -1 && groundNetTestAccessIndex2 > -1)
		{
			output->isICOpenTest = true;

			TK_testaccess *testaccess1 =  testaccessarray[groundNetTestAccessIndex1];
			TK_testaccess *testaccess2 =  testaccessarray[groundNetTestAccessIndex2];
			TK_netname *netname1 = ((testaccess1->netindex != -1) ? netnamearray.getAt(testaccess1->netindex) : NULL);
			TK_netname *netname2 = ((testaccess2->netindex != -1) ? netnamearray.getAt(testaccess2->netindex) : NULL);

			output->gNetName1 = netname1 ? netname1->netname : ""; 
			output->gNetName2 = netname2 ? netname2->netname : "";
			output->gTestAccessEntityNubmer1 = testaccess1->entityNumber;
			output->gTestAccessEntityNubmer2 = testaccess2->entityNumber;

			double tmpX1 = testaccess1->x;
			double tmpY1 = testaccess1->y;
			double tmpX2 = testaccess2->x;
			double tmpY2 = testaccess2->y;

			// Fixed case #2191, do not need to do unmirror of coordinate because the they were not mirrored when
			// they were stored inside TK_testaccess
			//if (topbottom)
			//{
			//	tmpX1 = -tmpX1;
			//	tmpX2 = -tmpX2;
			//}

			if (pcblist[filenum].mirror)
			{
				tmpX1 = -tmpX1;
				tmpX2 = -tmpX2;
			}

			Rotate(tmpX1, tmpY1, RadToDeg(pcblist[filenum].rotation), &tmpX1, &tmpY1);
			long gx1 = cnv_units(tmpX1 + pcblist[filenum].x);
			long gy1 = cnv_units(tmpY1 + pcblist[filenum].y);


			Rotate(tmpX2, tmpY2, RadToDeg(pcblist[filenum].rotation), &tmpX2, &tmpY2);
			long gx2 = cnv_units(tmpX2 + pcblist[filenum].x);
			long gy2 = cnv_units(tmpY2 + pcblist[filenum].y);

			if (pcblist[filenum].mirror)  
				topbottom = (topbottom + 1) % 2; // switch top and bottom

			if (topbottom) // now mirror them again.
			{
				gx1 = -gx1;
				gx2 = -gx2;
			}

			output->gx1 = gx1;
			output->gy1 = gy1;
			output->gx2 = gx2;
			output->gy2 = gy2;
			output->comment = comment;
		}
	}

	output->value = val;
	output->mergedValue = mergedValue;


	output->netName1 = "";
	output->netName2 = "";
	if (testProbeIndex1 > -1)
	{
		TK_testaccess *testaccess1 =  testaccessarray[testProbeIndex1];
		TK_netname *netname1 = ((testaccess1->netindex != -1) ? netnamearray.getAt(testaccess1->netindex) : NULL);
		output->netName1 = netname1 ? netname1->netname : "";
		output->testAccessEntityNumber1 = testaccess1->entityNumber;
	}
	if (testProbeIndex2 > -1)
	{
		TK_testaccess *testaccess2 =  testaccessarray[testProbeIndex2];
		TK_netname *netname2 = ((testaccess2->netindex != -1) ? netnamearray.getAt(testaccess2->netindex) : NULL);
		output->netName2 = netname2 ? netname2->netname : "";
		output->testAccessEntityNumber2 = testaccess2->entityNumber;
	}


   if (Version9)
   {
      if (openTest)
         output->option = "@K OP";
		else if (output->isICOpenTest)
			output->option = "@PL H 0 0 1 0 @T 100 30";
      else
      {
         positiveTol.Replace("+", "");
         negativeTol.Replace("-", "");
         output->option.Format("@T %s %s", positiveTol, negativeTol);
      }
   }
   else
   {
      output->option = "";
   }

   return outputcnt - 1;
}

/******************************************************************************
* get_ProbeNumber
*/
static int get_ProbeNumber(long x, long y, CString netName, BOOL onOff, BOOL topBottom, long  testAccessEntityNumber)
{
   for (int i=0; i<outputProbeTotalCnt; i++)
   {
      TKoutputProbe *outputProbe = outputProbeArray[i];
		if (outputProbe->testAccessEntityNumber != testAccessEntityNumber)
			continue;

      //if (netName.CompareNoCase(outputProbe->netName))
      //   continue;
      //if (x != outputProbe->x)
      //   continue;
      //if (y != outputProbe->y)
      //   continue;
      //if (topBottom != outputProbe->topBottom)
      //   continue;

      return outputProbe->probeNumber;
   }

   TKoutputProbe *outputProbe = new TKoutputProbe;
   outputProbeArray.SetAtGrow(outputProbeTotalCnt++, outputProbe);
   outputProbe->netName = netName;
   outputProbe->x = x;
   outputProbe->y = y;
   outputProbe->topBottom = topBottom;
   outputProbe->onOff = onOff;
	outputProbe->testAccessEntityNumber = testAccessEntityNumber;

   if (topBottom)
   {
      // probeNumber is always the last probe created because it starts at 1 not 0
      outputProbeBottomCnt++; 
      outputProbe->probeNumber = outputProbeBottomCnt;
   }
   else
   {
      // probeNumber is always the last probe created because it starts at 1 not 0
      outputProbeTopCnt++;
      outputProbe->probeNumber = outputProbeTopCnt;
   }

   return  outputProbe->probeNumber;
}

/******************************************************************************
* TKTestKooCompareFunc
*/
static int TKTestKooCompareFunc( const void *arg1, const void *arg2 )
{
   TK_testkoo **a1, **a2;
   a1 = (TK_testkoo**)arg1;
   a2 = (TK_testkoo**)arg2;

   return compare_name((*a1)->pinname, (*a2)->pinname);
}

/******************************************************************************
* sort_testkooarray
* If actually calls sort twice, once without any pins, once with. That is 
* because of the 2 pass to get artwork info.
*/
static void sort_testkooarray()
{
   if (testkoocnt < 2)
      return;

   qsort(testkooarray.GetData(), testkoocnt, sizeof(TK_testkoo *), TKTestKooCompareFunc);
   return;
}



static void getComponentXY(const int filenum, TKComp* comp, int testsurface, long& compX, long& compY)
{
	CPoint2d centroidXY;
   if (!comp->compInsert->getCentroidLocation(doc->getCamCadData(), centroidXY))
		centroidXY = comp->compInsert->getOrigin2d();

	double tmpX = centroidXY.x;
	double tmpY = centroidXY.y;

	if (pcblist[filenum].mirror)
		tmpX = -tmpX;

	Rotate(tmpX, tmpY, RadToDeg(pcblist[filenum].rotation), &tmpX, &tmpY);

	compX = cnv_units(tmpX + pcblist[filenum].x);
	compY = cnv_units(tmpY + pcblist[filenum].y);

	if (pcblist[filenum].mirror)  
		testsurface = testsurface==0?1:0; // switch top and bottom

	if (testsurface == 1) // now mirror them again.
		compX = -compX;
}



/******************************************************************************
* updateOutputStatus
*/
static void updateOutputStatus(int result, TKComp *comp, TK_testkoo *testKoo1, TK_testkoo *testKoo2, int testSurface)
{
	if (result == -2)
	{
		fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
				comp->name, testKoo1->pinname, testKoo2->pinname);
		display_error++;
	}
	else if (result >= 0)
	{
      if (testKoo1 != NULL)
      {
		   testaccessarray[testKoo1->testprobeptr]->usecnt++;
		   update_pintest(comp->name, testKoo1->pinname, testSurface==0?testTop:testBottom);
      }

      if (testKoo2 != NULL)
      {
		   testaccessarray[testKoo2->testprobeptr]->usecnt++;
		   update_pintest(comp->name, testKoo2->pinname, testSurface==0?testTop:testBottom);
      }
	}
}

/******************************************************************************
* do_Resistor
*/
#ifdef DEADCODE
static bool do_Resistor(int filenum, FileStruct *file, int maxhit, TKComp *comp, int testsurface, BOOL Version9)
{
	if (comp == NULL)
		return false;

	if (comp->takayadevicetype != deviceTypeResistor &&
		 comp->takayadevicetype != deviceTypeJumper && 
		 comp->takayadevicetype != deviceTypeFuse && 
		 comp->takayadevicetype != deviceTypeResistorArray)
		return false;

   comp->not_implemented_takayaclass = false;

	bool compIsTested = false;

   TK_testkoo *testkoo1;
   TK_testkoo *testkoo2;
   CString value = resistor_value(comp->value);
   CString comment = "";
   CString location = "";
   CString element = "R";

   if (Version9)
   {
		if (comp->takayadevicetype == deviceTypeFuse)
			value = "*";
      else if (comp->takayadevicetype == deviceTypeJumper)
         value = "0O";

      comment = comp->comment;

      if (comp->GridLoc != "")
         location = comp->GridLoc;
      else
         location = "*";

		if (comp->takayadevicetype == deviceTypeResistor && comp->mergedStatus.CompareNoCase("primary") == 0)
			comp->takayarefname.AppendFormat("*");
	}
   else
   {
      comment = value;
      location = "*";
   }


	int unusedPin = get_testpins(comp->name, &file->getNetList(), maxhit, testsurface);
	if (unusedPin <= 0)
	{
		free_testpins();
		return false;
	}
   sort_testkooarray();


	CAptModel* aptModel = getAptModel(comp->aptModelName); 
	if (aptModel != NULL)
	{
		compIsTested = updateOutputByAptModel(aptModel, filenum, true, (testsurface?true:false), comp, value, comment, location, element, (Version9?true:false), false);
	}
	else
	{
		int res = -1;
		if (testkoocnt == 2)
		{
			testkoo1 = testkooarray[0];
			testkoo2 = testkooarray[1];

			res = update_TK_output(filenum, TRUE, testsurface, comp->name, comp->takayarefname, value, comment, location,
					element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, 0 , 1, testkoo1->testprobeptr, 
					testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol, FALSE, Version9, comp->takayadevicetype,
					comp->mergedStatus, comp->mergedValue, comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

			if (res == -2)
			{
				fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n", 
						comp->name, testkoo1->pinname, testkoo2->pinname);
				display_error++;
			}
			else if (res >= 0)
			{
				testaccessarray[testkoo1->testprobeptr]->usecnt++;
				update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
				testaccessarray[testkoo2->testprobeptr]->usecnt++;
				update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
				compIsTested = true;
			}
		}
		else if (testkoocnt > 2)
		{
			// if more than 2 pins, find func_1, func_2 etc...
			if (comp->takayadevicesubclass > -1)
			{
				switch (comp->takayadevicesubclass)
				{
					case ATT_DEVICE_SUBCLASS_SIP_ISOLATED: // pin n with n+1, 
					{
						if (testkoocnt % 2 > 0)
						{
							fprintf(flog, "SIP ISOLATED Resistor [%s] has odd number of pin, test skipped\n", comp->name);
							display_error++;
							break;
						}

						// pins are sorted
						for (int ii=0; ii<testkoocnt; ii=ii+2)
						{
							testkoo1 = testkooarray[ii];
							testkoo2 = testkooarray[ii+1];

							int res = update_TK_output(filenum, 1, testsurface, comp->name, comp->takayarefname, value, comment, 
									location, element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, ii, ii+1, testkoo1->testprobeptr,
									testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol, FALSE, Version9, comp->takayadevicetype,
									comp->mergedStatus, comp->mergedValue, comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

							if (res == -2)
							{
								fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
										comp->name, testkoo1->pinname, testkoo2->pinname);
								display_error++;
							}
							else if (res >= 0)
							{
								testaccessarray[testkoo1->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
								testaccessarray[testkoo2->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
								compIsTested = true;
							}
						}
	               
					}
					break;

					case ATT_DEVICE_SUBCLASS_SIP_BUSSED:   // pin 1 is common, all others are resitors to pin 1
					{
						// pins are sorted
						for (int ii=1; ii<testkoocnt; ii++)
						{
							testkoo1 = testkooarray[0];
							testkoo2 = testkooarray[ii];

							int res = update_TK_output(filenum, 1, testsurface, comp->name, comp->takayarefname, value, comment, 
									location, element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, 0, ii, 
									testkoo1->testprobeptr, testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol, 
									FALSE, Version9, comp->takayadevicetype, comp->mergedStatus, comp->mergedValue, 
									comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

							if (res == -2)
							{
								fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
										comp->name, testkoo1->pinname, testkoo2->pinname);
								display_error++;
							}
							else if (res >= 0)
							{
								TK_testkoo *testkooLast = testkooarray[testkoocnt-1];

								testaccessarray[testkooLast->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
								testaccessarray[testkoo2->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
								compIsTested = true;
							}
						}
					}
					break;

					case ATT_DEVICE_SUBCLASS_DIP_ISOLATED: // 1 to n, 2 to n-1 etc..
					{
						if (testkoocnt % 2 > 0)
						{
							fprintf(flog, "DIP ISOLATED Resistor [%s] has odd number of pin, test skipped\n", comp->name);
							display_error++;
							break;
						}

						// pins are sorted
						for (int ii=0; ii<testkoocnt/2; ii++)
						{
							testkoo1 = testkooarray[ii];
							testkoo2 = testkooarray[testkoocnt-ii-1];

							int res = update_TK_output(filenum, 1, testsurface, comp->name, comp->takayarefname, value, comment, 
									location, element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, ii, testkoocnt-ii-1,
									testkoo1->testprobeptr, testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol, 
									FALSE, Version9, comp->takayadevicetype, comp->mergedStatus, comp->mergedValue,
									comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

							if (res == -2)
							{
								fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
										comp->name, testkoo1->pinname, testkoo2->pinname);
								display_error++;
							}
							else if (res >= 0)
							{
								testaccessarray[testkoo1->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
								testaccessarray[testkoo2->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
								compIsTested = true;
							}
						}
					}
					break;

					case ATT_DEVICE_SUBCLASS_DIP_BUSSED:   // highest is common, all other goto it
					{
						// pins are sorted
						for (int ii=0; ii<testkoocnt-1; ii++)
						{
							testkoo1 = testkooarray[ii];
							testkoo2 = testkooarray[testkoocnt-1];

							int res = update_TK_output(filenum, 1, testsurface, comp->name, comp->takayarefname, value, comment, 
									location, element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, ii, testkoocnt-1,
									testkoo1->testprobeptr, testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol,
									FALSE, Version9, comp->takayadevicetype, comp->mergedStatus, comp->mergedValue, 
									comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

							if (res == -2)
							{
								fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
										comp->name, testkoo1->pinname, testkoo2->pinname);
								display_error++;
							}
							else if (res >= 0)
							{
								testaccessarray[testkoo1->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
								testaccessarray[testkoo2->testprobeptr]->usecnt++;
								update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
								compIsTested = true;
							}
						}
					}
					break;

					default:
						fprintf(flog,"RESISTOR Subclass [%s] not implemented!\n", 
							device_subclass[comp->takayadevicesubclass]);
						display_error++;
					break;
				}
			}
			else // comp->takayadevicesubclass is less than or equal to -1
			{
				for (int ii=0; ii<testkoocnt; ii++)
				{
					int p1, p2;
					if (pinfunctarray_generic(comp->name, testkooarray[ii]->pinname, &p1, &p2) == 2)
					{

						testkoo1 = testkooarray[p1];
						testkoo2 = testkooarray[p2];
						int res = update_TK_output(filenum, 1, testsurface, comp->name, comp->takayarefname, value, comment,
								location, element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, p1, p2, 
								testkoo1->testprobeptr, testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol,
								FALSE, Version9, comp->takayadevicetype, comp->mergedStatus, comp->mergedValue, 
								comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

						if (res == -2)
						{
							fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
								comp->name, testkoo1->pinname, testkoo2->pinname);
							display_error++;
						}
						else if (res >= 0)
						{
							testaccessarray[testkoo1->testprobeptr]->usecnt++;
							update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
							testaccessarray[testkoo2->testprobeptr]->usecnt++;
							update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
							compIsTested = true;
						}
					}
				}
			} // if subdevice found
		}
	}

   free_testpins();
	return compIsTested;
}
#endif
/******************************************************************************
* do_Capacitor
*/
#ifdef DEADCODE
static bool do_Capacitor(int filenum, FileStruct *file, int maxhit, TKComp *comp, int testsurface, BOOL Version9)
{
	if (comp == NULL)
		return false;

	if (comp->takayadevicetype != deviceTypeCapacitor &&
		 comp->takayadevicetype != deviceTypeCapacitorArray)
		return false;


   comp->not_implemented_takayaclass = false;

	bool compIsTested = false;
   TK_testkoo *testkoo1;
   TK_testkoo *testkoo2;
   CString value = capacitor_value(comp->value);
   CString comment = "";
   CString location = "";
   CString element = "C";

   if (Version9)
   {
      if (value != "*")
      {
         value.Replace("FARADS", "F"); // value has "FARADS" as suffix, then replace with "F"
         value.Replace("FARAD", "F");  // value has "FARAD" as suffix, then replace with "F"
         if (value.Right(1) != "F")    // value does not end in "F", then attach "F"
            value += "F";
      }

      comment = comp->comment;

     if (comp->GridLoc != "")
         location = comp->GridLoc;
      else
         location = "*";

		if (comp->takayadevicetype == deviceTypeCapacitor && comp->mergedStatus.CompareNoCase("primary") == 0)
			comp->takayarefname.AppendFormat("*");
	}
   else
   {
      comment = value;
      value += "f";
      location = "*";
   }


	int unusedPin = get_testpins(comp->name, &file->getNetList(), maxhit, testsurface);
	if (unusedPin <= 0)
	{
		free_testpins();
		return false;
	}
   sort_testkooarray();

	CAptModel* aptModel = getAptModel(comp->aptModelName); 
	if (aptModel != NULL)
	{
		compIsTested = updateOutputByAptModel(aptModel, filenum, true, (testsurface?true:false), comp, value, comment, location, element, (Version9?true:false), false);
	}
	else
	{
		if (testkoocnt == 2)
		{
			testkoo1 = testkooarray[0];
			testkoo2 = testkooarray[1];

			int res = update_TK_output(filenum, TRUE, testsurface, comp->name, comp->takayarefname, value, comment, 
					location, element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, 0 ,1, testkoo1->testprobeptr,
					testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol, FALSE, Version9, comp->takayadevicetype,
					comp->mergedStatus, comp->mergedValue, comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

			if (res == -2)
			{
				fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
						comp->name, testkoo1->pinname, testkoo2->pinname);
				display_error++;
			}
			else if (res >= 0)
			{
				testaccessarray[testkoo1->testprobeptr]->usecnt++;
				update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
				testaccessarray[testkoo2->testprobeptr]->usecnt++;
				update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
				compIsTested = true;
			}
		}
		else if (testkoocnt > 2)
		{
			// if more than 2 pins, find func_1, func_2 etc...

			for (int ii=0; ii<testkoocnt; ii++)
			{
				int p1, p2;
				if (pinfunctarray_generic(comp->name, testkooarray[ii]->pinname, &p1, &p2) == 2)
				{
					testkoo1 = testkooarray[p1];
					testkoo2 = testkooarray[p2];

					int res = update_TK_output(filenum, TRUE, testsurface, comp->name, comp->takayarefname, value, comment,
							location, element, testkoo1->x, testkoo1->y, testkoo2->x, testkoo2->y, p1, p2, testkoo1->testprobeptr,
							testkoo2->testprobeptr, comp->positiveTol, comp->negativeTol, FALSE, Version9, comp->takayadevicetype,
							comp->mergedStatus, comp->mergedValue, comp->testStrategy, false, -1, -1, comp->bottom, "", comp);

					if (res == -2)
					{
						fprintf(flog, "Component [%s] Pin [%s] and Pin [%s] have the same Probe / Net!\n",
								comp->name, testkoo1->pinname, testkoo2->pinname);
						display_error++;
					}
					else if (res >= 0)
					{
						testaccessarray[testkoo1->testprobeptr]->usecnt++;
						update_pintest(comp->name, testkoo1->pinname, testsurface==0?testTop:testBottom);
						testaccessarray[testkoo2->testprobeptr]->usecnt++;
						update_pintest(comp->name, testkoo2->pinname, testsurface==0?testTop:testBottom);
						compIsTested = true;
					}
				}
			}
		}
	}

   free_testpins();
	return compIsTested;
}

#endif
/******************************************************************************
* get_devicesubclassindex
*/
static int get_devicesubclassindex(const char *subclass)
{
   for (int i=0; i<=MAX_DEVICE_SUBCLASS; i++)
   {
      if (!STRICMP(device_subclass[i],subclass))
         return i;
   }
   return -1;  // NOTEST 
}


/******************************************************************************
* writeFileHeader
*/
static void writeFileHeader(FILE *outfile)
{
	if (outfile == NULL)
		return;

   CTime curTime;
	curTime = curTime.GetCurrentTime();

	fprintf(outfile, "// Generated by %s on %s\n", getApp().getCamCadSubtitle(), curTime.Format("%A, %B %d, %Y at %H:%M:%S"));

	CString boardNames = "";
   for (int p=0; p<pcblistcnt; p++) // now loop through all files
   {
		FileStruct *file = doc->Find_File_by_BlockGeomNum(pcblist[p].geomnum);
		if (file == NULL)
			continue;

		fprintf(outfile, "// Board Name: %s\n", file->getName());
		boardNames.AppendFormat("%s ", file->getName());
	}

	boardNames.Trim();
	fprintf(outfile, "\n@M %s\n", boardNames);
}

static int writeTakayaOutput(FileStruct *file, CString fileName, int topBottom, BOOL Version9, CPinToPinShortTest& pinToPinShortTest) 
{
	if (fileName.IsEmpty())
		return 0;

   FILE *wfp;
   UNLINK(fileName);  // delete enventual old files.
   if ((wfp = fopen(fileName, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s", fileName);
      ErrorMessage( tmp, "Error File open", MB_OK | MB_ICONHAND);
      return 0;
   }

   progress->SetStatus(fileName);
	if (Version9)
      fprintf(flog, "# Start of TAKAYA CA9 (%s)\n", fileName);
   else
      fprintf(flog, "# Start of TAKAYA CA8 (%s)\n", fileName);
	writeFileHeader(wfp);

	//output reference point settings
	if (topBottom == 0)
	{
		if (!sTopBrdRefPntCommand.IsEmpty() && sTopBrdRefPntCommand.CompareNoCase(""))
			fprintf(wfp,"%s\n", sTopBrdRefPntCommand);

		if (!sTopAuxRefPntCommand.IsEmpty() && sTopAuxRefPntCommand.CompareNoCase(""))
			fprintf(wfp,"%s\n", sTopAuxRefPntCommand);
	}
	else
	{
		if (!sBotBrdRefPntCommand.IsEmpty() && sBotBrdRefPntCommand.CompareNoCase(""))
			fprintf(wfp,"%s\n", sBotBrdRefPntCommand);

		if (!sBotAuxRefPntCommand.IsEmpty() && sBotAuxRefPntCommand.CompareNoCase(""))
			fprintf(wfp,"%s\n", sBotAuxRefPntCommand);
	}

   TKoutput *output = NULL;
   TKoutputProbe *outputProbe;
   CString probeNum1;
   CString probeNum2;
	CString ICOpenG1ProbeNum;
	CString ICOpenG2ProbeNum;
	int visionTestCount = 0;
	int i = 0;

   for (i=0; i<outputcnt; i++)
   {
      bool writeVision = false;
	   bool sameAsLastComp = false;
	   bool lastCompWasICOpenTest = false;
      output = outputarray[i];

      if (output->onoff == 1 && output->topbottom == topBottom)
      {
			CString commentOperator = "";
			if (output->mergedStatus.CompareNoCase("ignored") == 0)
				commentOperator = "//";
			else if (!output->testStrategy.IsEmpty() && output->testStrategy.Find(ATT_VALUE_TEST_FPT) < 0)
				commentOperator = "//";

			CString valueString = output->mergedStatus.CompareNoCase("primary")?output->value:output->mergedValue;
			writeVision = (ALL_COMPONENT_VISION || output->writeSelectVision && SELECT_VISION				||
							  (output->deviceType == deviceTypeCapacitorPolarized && POLARIZED_CAP_VISION)	||
							  (output->deviceType == deviceTypeCapacitorTantalum && TANTALUM_CAP_VISION)		);

			if (i > 0)
			{
				TKoutput* lastOutput = outputarray[i-1];
				if (lastOutput != NULL)
				{
					sameAsLastComp = lastOutput->refname.CompareNoCase(output->refname) == 0;
					lastCompWasICOpenTest = lastOutput->isICOpenTest;
				}
			}

			if (Version9)
         {
				if (!sameAsLastComp && lastCompWasICOpenTest)
					fprintf(wfp, "@CE\n");

				if (!output->writeSelectVision)
				{
					if (output->isICOpenTest)
					{
						if (!sameAsLastComp)
						{
							ICOpenG1ProbeNum.Format("N%d", get_ProbeNumber(output->gx1, output->gy1, output->gNetName1, output->onoff, output->topbottom, output->gTestAccessEntityNubmer1));
							ICOpenG2ProbeNum.Format("N%d", get_ProbeNumber(output->gx2, output->gy2, output->gNetName2, output->onoff, output->topbottom, output->gTestAccessEntityNubmer2));

							fprintf(wfp, "@A G1:%s G2:%s\n", ICOpenG1ProbeNum, ICOpenG2ProbeNum);
						}

						probeNum1.Format("N%d", get_ProbeNumber(output->x1, output->y1, output->netName1, output->onoff, output->topbottom, output->testAccessEntityNumber1));

						if (probeNum1.CompareNoCase(ICOpenG1ProbeNum) != 0 && probeNum1.CompareNoCase(ICOpenG2ProbeNum) != 0 )
						{
							// Only output test if the probe number is not the same as the one in @A G1:N G2:N
							fprintf(wfp, "%s %s %s %s %s %s %d %d %s\n", commentOperator, output->refname, valueString,
									output->comment, output->location, probeNum1, output->x2, output->y2, output->option);
						}
					}
					else if ( output->deviceType == deviceTypeOpto)
					{
						CString probeNum3;
						CString probeNum4;
						TKoutput* nextOutput = NULL;
						if (i+1 < outputcnt)
						{
                     ++i;
							nextOutput = outputarray[i];
						}

						if (nextOutput != NULL)
						{
							// Anode
							probeNum1.Format("N%d", get_ProbeNumber(output->x1, output->y1, output->netName1, output->onoff, output->topbottom, output->testAccessEntityNumber1));
							// Cathode
							probeNum2.Format("N%d", get_ProbeNumber(output->x2, output->y2, output->netName2, output->onoff, output->topbottom, output->testAccessEntityNumber2));
							// Collector
							probeNum3.Format("N%d", get_ProbeNumber(nextOutput->x1, nextOutput->y1, nextOutput->netName1, nextOutput->onoff, nextOutput->topbottom, nextOutput->testAccessEntityNumber1));
							// Emitter
							probeNum4.Format("N%d", get_ProbeNumber(nextOutput->x2, nextOutput->y2, nextOutput->netName2, nextOutput->onoff, nextOutput->topbottom, nextOutput->testAccessEntityNumber2));

							fprintf(wfp, "%s@PC %s %s * %s %s %s %s\n", commentOperator, output->tkrefname, output->comment, probeNum1, probeNum2, probeNum3, probeNum4);
							fprintf(wfp, "%s@CE\n", commentOperator);
							fprintf(wfp, "%s%s C-E %s * R %s %s @K OP @K JP\n", commentOperator, output->tkrefname, output->comment, probeNum3, probeNum4);
							fprintf(wfp, "%s%s A-K %s * D %s %s @T 25 25 @K JP\n", commentOperator, output->tkrefname, output->comment, probeNum1, probeNum2);
						}
					}
					else if ((output->deviceType == deviceTypeTransistor				|| output->deviceType == deviceTypeTransistorNpn			||
							    output->deviceType == deviceTypeTransistorPnp			|| output->deviceType == deviceTypeTransistorFetNpn		||
								 output->deviceType == deviceTypeTransistorFetPnp		|| output->deviceType == deviceTypeTransistorMosfetNpn	||
								 output->deviceType == deviceTypeTransistorMosfetPnp	) && output->refname.CompareNoCase("SHORT") != 0)
					{
						// output is not SHORT and is Transistor, Transistor NPN, or Transistor PNP

						CString probeNum3;
						CString probeNum4;
						TKoutput* nextOutput = NULL;
						if (i+1 < outputcnt)
						{
                     ++i;
							nextOutput = outputarray[i];
						}

                  if (nextOutput != NULL)
                  {
							// Base or Gate
							probeNum1.Format("N%d", get_ProbeNumber(output->x1, output->y1, output->netName1, output->onoff, output->topbottom, output->testAccessEntityNumber1));
							// Collector or Source
							probeNum2.Format("N%d", get_ProbeNumber(output->x2, output->y2, output->netName2, output->onoff, output->topbottom, output->testAccessEntityNumber2));
							// Emitter or Drain
							if (nextOutput != NULL)
								probeNum3.Format("N%d", get_ProbeNumber(nextOutput->x1, nextOutput->y1, nextOutput->netName1, nextOutput->onoff, nextOutput->topbottom, nextOutput->testAccessEntityNumber1));

                     CString partNumber;
                     CString option;
						   if (output->deviceType  == deviceTypeTransistorPnp)
                     {
                        partNumber = ((output->comment.Compare("*") == 0) ? "PNP" : output->comment);
                        option = output->option.IsEmpty()?"@T 20 20":output->option;
							   fprintf(wfp, "%s@Q2 %s %s * %s\n", commentOperator, output->tkrefname, partNumber, option);
                     }
						   else if (output->deviceType  == deviceTypeTransistor || output->deviceType  == deviceTypeTransistorNpn)
                     {
                        partNumber = ((output->comment.Compare("*") == 0) ? "NPN" : output->comment);
                        option = output->option.IsEmpty()?"@T 20 20":output->option;
							   fprintf(wfp, "%s@Q2 %s %s * %s\n", commentOperator, output->tkrefname, partNumber, option);
                     }
						   else if (output->deviceType  == deviceTypeTransistorFetNpn || output->deviceType  == deviceTypeTransistorMosfetNpn)
						   {
                        partNumber = ((output->comment.Compare("*") == 0) ? "FET_N" : output->comment);
                        option = output->option;
                        if (option.Find("@MM") < 0)
                           option.Format("@MM %s", output->subClass.IsEmpty()?"FETEN":output->subClass);

							   fprintf(wfp, "%s@F %s %s * %s\n", commentOperator, output->tkrefname, partNumber, option);
                     }
						   else if (output->deviceType  == deviceTypeTransistorFetPnp || output->deviceType  == deviceTypeTransistorMosfetPnp)
                     {
                        partNumber = ((output->comment.Compare("*") == 0) ? "FET_P" : output->comment);
                        option = output->option;
                        if (option.Find("@MM") < 0)
                           option.Format("@MM %s", output->subClass.IsEmpty()?"FETEP":output->subClass);

							   fprintf(wfp, "%s@F %s %s * %s\n", commentOperator, output->tkrefname,partNumber, option);
                     }
                  }

						fprintf(wfp, "%s %s\n", commentOperator, probeNum1);
						fprintf(wfp, "%s %s\n", commentOperator, probeNum2);
						fprintf(wfp, "%s %s\n", commentOperator, probeNum3);
						fprintf(wfp, "%s@CE\n", commentOperator);
					}
					else
					{
						probeNum1.Format("N%d", get_ProbeNumber(output->x1, output->y1, output->netName1, output->onoff, output->topbottom, output->testAccessEntityNumber1));
						probeNum2.Format("N%d", get_ProbeNumber(output->x2, output->y2, output->netName2, output->onoff, output->topbottom, output->testAccessEntityNumber2));
	            
                  CString option = output->option;
                  if (output->deviceType == deviceTypeDiodeZener && option.Find("@MM") < 0)
                  {
                     option.AppendFormat(" @MM DCZD");
                  }

						fprintf(wfp, "%s %s %s %s %s %s %s %s %s\n", commentOperator, output->tkrefname, valueString,
								output->comment, output->location, output->element, probeNum1, probeNum2, option);
					}
				}

				if (SELECT_VISION && !output->writeSelectVision && commentOperator == "//" &&
						output->mergedStatus.CompareNoCase("ignored") == 0)
				{
					// output->writeSelectVision is FALSE, but the paralleled test
					// has been commented out so write out a version test
					writeVision = true;
				}
         }
         else
         {
            fprintf(wfp, "%s %-8s %6s %14s %4s %s %7ld %7ld %7ld %7ld\n", commentOperator, output->tkrefname, valueString,
						output->comment, output->location, output->element, output->x1, output->y1, output->x2, output->y2);
         }

			output->writeSelectVision = writeVision;
		}

		// Write out vision test for the top component
      if (output->onoff == 1 && output->compSurfaceBottom == topBottom)
		{
          if (writeVision && visionTestCount < maxVisionCount)
         {
			   CString valueString = "*"; // Mark said to always output "*" as value for all vision test

			   if (Version9)
			   {
				   CString versionCommand = "";
				   if (output->deviceType == deviceTypeCapacitorPolarized)
					   versionCommand = "@V1";
				   else if (output->deviceType == deviceTypeCapacitorTantalum)
					   versionCommand = "@V2";
				   else if (output->deviceType == deviceTypeIC)
					   versionCommand = "@V3";
				   else
					   versionCommand = "@V5";

				   fprintf(wfp, " %s %s %s %s %s %d %d\n", versionCommand, output->tkrefname, valueString,
						   output->comment, output->location, output->compX, output->compY);
			   }
			   else
			   {
				   fprintf(wfp, " @V %-5s %6s %14s %4s %7ld %7ld %7ld %7ld\n", output->tkrefname, valueString,
						   output->comment, output->location, output->x1, output->y1, output->x2, output->y2);
			   }
			   visionTestCount++;
         }
         else if (output->writeSelectVision && !sameAsLastComp)
         {
            // Need to write vision but didn't get writen then log it in the log file
            fprintf(flog, "Comp [%s] not written in Takaya output file.\n", output->refname);
         }
		}
	}

	if (Version9)
   {
		// output pin to poin short test
		pinToPinShortTest.writePinToPinOutput(wfp, topBottom==1?true:false);

		DeleteAllProbes(doc, file);   
		doc->PrepareAddEntity(file);

		int probePlacementKw = doc->getStandardAttributeKeywordIndex(standardAttributeProbePlacement);
		int dataLinkKw = doc->getStandardAttributeKeywordIndex(standardAttributeDataLink);
		int netNameKw = doc->getStandardAttributeKeywordIndex(standardAttributeNetName);
      int refnameKW = doc->getStandardAttributeKeywordIndex(standardAttributeRefName);

		fprintf(wfp, "@N\n");
      for (i=0; i<outputProbeTotalCnt; i++)
      {
         outputProbe = outputProbeArray[i];
         if (outputProbe->onOff == 1 && outputProbe->topBottom == topBottom)
            fprintf(wfp, "%d: %d, %d %s\n", outputProbe->probeNumber, outputProbe->x, outputProbe->y, outputProbe->netName);

			// Update probes on CamcadDoc
			CEntity entity = CEntity::findEntity(doc->getCamCadData(), outputProbe->testAccessEntityNumber, entityTypeData);
			if (entity.getEntityType() != entityTypeData)
				continue;

			DataStruct* testAccess = entity.getData();
			if (testAccess->getInsert() == NULL || testAccess->getInsert()->getInsertType() != insertTypeTestAccessPoint)
				continue;

			Attrib* attrib = NULL;
			double testAccessFeatureSize = 0.0;
			if (attrib = is_attvalue(doc, testAccess->getAttributes(), DFT_ATT_EXPOSE_METAL_DIAMETER, 0))
				testAccessFeatureSize = attrib->getDoubleValue();

			CString probeBlockName;
			probeBlockName.Format("%0.3f_%s", testAccessFeatureSize, (outputProbe->topBottom == 0)?"Top":"Bottom");
			BlockStruct* probeBlock = probeBlock = CreateTestProbeGeometry(doc, probeBlockName, testAccessFeatureSize, probeBlockName, testAccessFeatureSize);

			CString probeName, probeRefname;
			probeName.Format("N%d_%s", outputProbe->probeNumber, (outputProbe->topBottom == 0)?"Top":"Bottom");
         probeRefname.Format("N%d", outputProbe->probeNumber);
			DataStruct *probeData = Graph_Block_Reference(probeBlock->getName(), probeName, probeBlock->getFileNumber(), 
				testAccess->getInsert()->getOriginX(), testAccess->getInsert()->getOriginY(), 0, (outputProbe->topBottom == 0)?FALSE:TRUE, 1.0, -1, FALSE);
			probeData->getInsert()->setInsertType(insertTypeTestProbe);
         probeData->setHidden(false);

         void* voidPtr = (void*)"Placed";
         probeData->setAttrib(doc->getCamCadData(), probePlacementKw, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
         probeData->setAttrib(doc->getCamCadData(), dataLinkKw, valueTypeInteger, (void*)&outputProbe->testAccessEntityNumber, attributeUpdateOverwrite, NULL);
			probeData->setAttrib(doc->getCamCadData(), netNameKw, valueTypeString, outputProbe->netName.GetBuffer(0), attributeUpdateOverwrite, NULL);

         // Add probe number as REFNAME attribute
			CreateTestProbeRefnameAttr(doc, probeData, probeRefname, testAccessFeatureSize);
		}
      fprintf(wfp, "@CE\n");
   }

   fclose(wfp);

	return 1;
}




/******************************************************************************
* output_koo_done 
* do not write the same testgraphic twice
*/
static int output_koo_done (int cur_koo, int xy)
{
   for (int i=0; i<cur_koo; i++)
   {
      if (outputarray[i]->onoff == 0)  continue;
      if (outputarray[i]->topbottom != outputarray[cur_koo]->topbottom) continue;

      if (xy == 1)   // try to write xy1
      {
         if (outputarray[i]->x1 == outputarray[cur_koo]->x1 && outputarray[i]->y1 == outputarray[cur_koo]->y1)
             return FALSE;
         if (outputarray[i]->x2 == outputarray[cur_koo]->x1 && outputarray[i]->y2 == outputarray[cur_koo]->y1)
             return FALSE;
      }
      else           // try to write xy2
      {
         if (outputarray[i]->x1 == outputarray[cur_koo]->x2 && outputarray[i]->y1 == outputarray[cur_koo]->y2)
             return FALSE;
         if (outputarray[i]->x2 == outputarray[cur_koo]->x2 && outputarray[i]->y2 == outputarray[cur_koo]->y2)
             return FALSE;
      }
   }

   return FALSE;
}





/******************************************************************************
* *clean_tol
*/
static char *clean_tol(const char *tol, const char *def)
{
   static CString t = tol;

   if (t.Right(1) == '%')
   {
      CString w = t.Left(strlen(t) - 1);
      t = w;
   }

   if (t.Left(1) == '-')
   {
      CString w = t.Right(strlen(t) - 1);
      t = w;
   }

   if (t.Left(1) == '+')
   {
      CString w = t.Right(strlen(t) - 1);
      t = w;
   }

   int num = TRUE;
   for (int i=0; i<(int)strlen(t); i++)
   {
      if (isalpha(t[i]))
         num = FALSE;
   }

   if (!num)
   {
      fprintf(flog, "Bad Tolerance value [%s] -> changed to [%s]\n", t, def);
      display_error++;
      t = def;
   }

   return t.GetBuffer(0);
}

/******************************************************************************
* char *clean_capvalue
*/
static const char *clean_capvalue(CString c, const char *u)
{
   static CString t = c;
   if (c.Right(1) == 'F' || c.Right(1) == 'f')
      t = c.Left(strlen(c)-1);

   // this puts a default ending from takaya.out
   char e = c.GetAt(strlen(c)-1);
   if (!isalpha(e) && e != '*')
      t += u;

   // here check for numbers -just first because it can be .01u
   if (isalpha(t[0]))
   {
      fprintf(flog, "Bad Capacitor value [%s] -> changed to **\n", t);
      display_error++;
      t = "**";
   }

   return t.GetBuffer(0);
}

/******************************************************************************
* char *clean_inductorvalue
*/
static const char *clean_inductorvalue(CString c, const char *u)
{
   static CString t = c;
   if (c.Right(1) == 'H' || c.Right(1) == 'h')
      t = c.Left(strlen(c)-1);

   // this puts a default ending from takaya.out
   char e = c.GetAt(strlen(c)-1);
   if (!isalpha(e) && e != '*')
      t += u;

   // just test first, because it can be .100u
   if (isalpha(t[0]))
   {
      fprintf(flog, "Bad Inductor value [%s] -> changed to **\n", t);
      display_error++;
      t = "**";
   }

   return t.GetBuffer(0);
}

/******************************************************************************
* char *clean_zenervoltage
*/
static const char *clean_zenervoltage(CString c, const char *u)
{
   static CString t = c;
   if (c.Right(1) == 'V' || c.Right(1) == 'v')
      t = c.Left(strlen(c)-1);

   return t.GetBuffer(0);
}

/******************************************************************************
* load_TAKAYAsettings
*/
static int load_TAKAYAsettings(const CString fname, int pageunits, FormatStruct* format, CCEtoODBDoc* document, CIcDiodeCommandMap& icDiodeCommandMap)
{
   FILE *fp;
   char line[255];
   char *lp;

   format->Tak.Capacitor = TRUE;
   format->Tak.Diode = TRUE;
   format->Tak.Inductor = TRUE;
   format->Tak.Short = TRUE;
   format->Tak.IC_Capacitor = TRUE;
   format->Tak.IC_Diode = TRUE;
   format->Tak.Resistor = TRUE;
   format->Tak.Transistor = TRUE;
   format->Tak.ZenerDiode = TRUE;
	format->Tak.BottomPopulated = TRUE;
	format->Tak.TopPopulated = TRUE;
	format->Tak.TopPopulated = TRUE;
   format->Tak.MaxHitCount = 50;
	format->Tak.SidePreference = 0;
	format->Tak.TestSurface = 0;

	/*if (document->getFileList().GetOnlyShown(blockTypePcb) != NULL && document->GetCurrentDFTSolution(*(document->getFileList().GetOnlyShown(blockTypePcb))) != NULL
		 && document->GetCurrentDFTSolution(*(document->getFileList().GetOnlyShown(blockTypePcb)))->GetTestPlan() != NULL)
	{
		format->Tak.TestSurface = document->GetCurrentDFTSolution(*(document->getFileList().GetOnlyShown(blockTypePcb)))->GetTestPlan()->GetSurface();
      format->Tak.SidePreference = document->GetCurrentDFTSolution(*(document->getFileList().GetOnlyShown(blockTypePcb)))->GetTestPlan()->GetProbeSidePreference();
	}*/


   TAKAYA_PROBE_SIZE = 0.1 * Units_Factor(UNIT_INCHES, pageunits);
	POLARIZED_CAP_VISION = false;
	TANTALUM_CAP_VISION  = false; 
	ALL_COMPONENT_VISION = false;
	SELECT_VISION = false;
	EXPORE_MODE = 1;
	APT_MODEL_FILE_NAME = "aptmodel.asc";
	shortsType.Empty();
	pinToPinDeviceTypeMap.RemoveAll();
	pinToPinDistance = -1;
	maxVisionCount = 240;
   icDiodePrefix.Empty();
   icCapacitorPrefix.Empty();
   takayaLibraryDirectory.Empty();
   isIcOpenDescAttribute = false;
   icOpenDescription.Empty();
   commentAttribute.Empty();

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage( tmp, "TAKAYA Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = strtok(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".TEST_PROBE_GRAPHIC"))
         {
            if ((lp = strtok(NULL, " =\t\n")) == NULL)
               continue;
            TAKAYA_PROBE_SIZE = atof(lp);
               
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            int u;
            if ((u =  GetUnitIndex(lp)) > -1)
               TAKAYA_PROBE_SIZE = TAKAYA_PROBE_SIZE * Units_Factor(u, pageunits);
         }
         else if (!STRICMP(lp, ".PINFUNCTION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            CString fname = lp;

            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
            CString pname = lp;

            TK_pinfunction *p = new TK_pinfunction;
            pinfunctionarray.SetAtGrow(pinfunctioncnt++, p);  
            p->pinfunction = fname;
            p->pinname = pname;
         }
         else if (!STRICMP(lp, ".BOTTOM_POPULATED"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'N')
					format->Tak.BottomPopulated = FALSE;
         }
         else if (!STRICMP(lp, ".TOP_POPULATED"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'N')
					format->Tak.TopPopulated = FALSE;
         }
         else if (!STRICMP(lp, ".MAX_HIT_COUNT"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				format->Tak.MaxHitCount = atoi(lp);
         }
         else if (!STRICMP(lp, ".POLARIZED_CAP_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					POLARIZED_CAP_VISION = true;
         }
         else if (!STRICMP(lp, ".TANTALUM_CAP_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					TANTALUM_CAP_VISION = true;
         }
         else if (!STRICMP(lp, ".ALL_COMPONENT_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					ALL_COMPONENT_VISION = true;
         }
         else if (!STRICMP(lp, ".SELECT_VISION"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'Y')
					SELECT_VISION = true;
         }
			else if (!STRICMP(lp, ".MODE"))
			{
            if ((lp = strtok(NULL, " \t\n")) == NULL)
               continue;
				if (atoi(lp) == 2)
					EXPORE_MODE = 2;
			}
			else if (!STRICMP(lp, ".TEST_LIB"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				APT_MODEL_FILE_NAME = lp;
			}
			else if (!STRICMP(lp, ".SHORTS"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				shortsType = lp;
			}
			else if (!STRICMP(lp, ".PIN_TO_PIN_TYPE"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				CString deviceType = lp;
				deviceType.MakeUpper();
				pinToPinDeviceTypeMap.SetAt(deviceType, deviceType);
			}
			else if (!STRICMP(lp, ".PIN_TO_PIN_DISTANCE"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				pinToPinDistance = atof(lp);
			}		
			else if (!STRICMP(lp, ".MAX_VISION_COUNT"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
				maxVisionCount = atoi(lp);
			}		
			else if (!STRICMP(lp, ".IC_DIODE"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            CString partNumber = lp;
            partNumber.MakeUpper();

            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            CString pinName = lp;

            bool swap = false;
            if ((lp = strtok(NULL, " \"\t\n")) != NULL)
               swap = STRCMPI(lp, "TRUE")==0?true:false;

            icDiodeCommandMap.addCommand(partNumber, pinName, swap);
         }
			else if (!STRICMP(lp, ".IC_DIODE_PREFIX"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            icDiodePrefix = lp;
         }
			else if (!STRICMP(lp, ".IC_CAP_PREFIX"))
			{
            if ((lp = strtok(NULL, " \"\t\n")) == NULL)
               continue;
            icCapacitorPrefix = lp;
         }
         else if (!STRICMP(lp, ".TAKAYA_LIBRARY"))
         {
            if ((lp = strtok(NULL, "\"\t\n")) == NULL)
               continue;
            takayaLibraryDirectory = lp;
         }
         else if (!STRICMP(lp, ".IC_OPEN_DESC"))
         {
            if ((lp = strtok(NULL, "\"\t\n")) == NULL)
               continue;
            icOpenDescription = lp;

            if (icOpenDescription.Left(1) == "<" && icOpenDescription.Right(1) == ">")
            {
               // If the string is inside a pair of <>, then it is the name of an attribute
               icOpenDescription = icOpenDescription.Mid(1, icOpenDescription.GetLength()-2);
               isIcOpenDescAttribute = true;
            }
         }
         else if (!STRICMP(lp, ".COMMENT"))
         {
            if ((lp = strtok(NULL, "\"\t\n")) == NULL)
               continue;
            commentAttribute = lp;
         }
		}
   }

   fclose(fp);
   return 1;
}

//_____________________________________________________________________________
TKComp::TKComp(int index) : m_index(index)
{
}

//_____________________________________________________________________________
TKCompArray::TKCompArray(int size) : CTypedPtrArrayWithMapContainer<TKComp>(size)
{
}

TKComp* TKCompArray::getDefined(const CString& name)
{
   TKComp* comp;

   if (!lookup(name,comp))
   {
      int index = getSize();
      comp = new TKComp(index);
      setAt(index,name,comp);
   }

   return comp;
}

//_____________________________________________________________________________
TK_netname::TK_netname(int index) : m_index(index)
{
}

//_____________________________________________________________________________
TKNetnameArray::TKNetnameArray(int size) : CTypedPtrArrayWithMapContainer<TK_netname>(size)
{
}

TK_netname* TKNetnameArray::getDefined(const CString& name)
{
   TK_netname* net;

   if (!lookup(name,net))
   {
      int index = getSize();
      net = new TK_netname(index);
      setAt(index,name,net);
   }

   return net;
}

//_____________________________________________________________________________
CCamCadPinEntityMap::CCamCadPinEntityMap(CCamCadPinMap& camCadPinMap) :
   m_camCadPinEntities(nextPrime2n(camCadPinMap.getSize()),false)
{
   int size = camCadPinMap.getSize();

   for (int index = 0;index < size;index++)
   {
      CCamCadPin* pin = camCadPinMap.getAt(index);

      int entityNumber = pin->getCompPin()->getEntityNumber();
      m_camCadPinEntities.SetAt(entityNumber,pin);
   }
}

CCamCadPin* CCamCadPinEntityMap::getAt(int entityNumber)
{
   CCamCadPin* pin = NULL;

   if (!m_camCadPinEntities.Lookup(entityNumber,pin))
   {
      pin = NULL;
   }

   return pin;
}

//_____________________________________________________________________________
TK_short::TK_short(const CString& netName1,const CString& netName2) :
   netname1(netName1),netname2(netName2)
{
   tested = 0;
}

//_____________________________________________________________________________
TKShortArray::TKShortArray()
{
}

bool TKShortArray::hasShort(const CString& netname1,const CString& netname2)
{
   CString descriptor;

   if (netname1 < netname2)
   {
      descriptor = netname1 + ":" + netname2;
   }
   else
   {
      descriptor = netname2 + ":" + netname1;
   }

   TK_short* shortEntry;
   bool retval = lookup(descriptor,shortEntry);

   return retval;
}

TK_short* TKShortArray::addShort(const CString& netname1,const CString& netname2)
{
   CString descriptor;

   if (netname1 < netname2)
   {
      descriptor = netname1 + ":" + netname2;
   }
   else
   {
      descriptor = netname2 + ":" + netname1;
   }

   TK_short* shortEntry = NULL;

   if (!lookup(descriptor,shortEntry))
   {
      shortEntry = new TK_short(netname1,netname2);
      add(descriptor,shortEntry);
   }

   return shortEntry;
}


//_____________________________________________________________________________
ICOpenTestGeom::ICOpenTestGeom(CCamCadData& camCadData, BlockStruct* compGeomBlock, BlockStruct *realPartPkgBlock, double sensorDiameter)
: m_shrinkOkay(true)  // assume okay until failure occurs
{
	m_name = "";
   m_sensorDiameter = sensorDiameter;
	m_pinLocationMap.empty();

	if (compGeomBlock != NULL)  // Okay if realPartPkgBlock is NULL, it is optional
	{
		m_name = compGeomBlock->getName();
		createPinLocationMap(camCadData, compGeomBlock, realPartPkgBlock);
	}
}

ICOpenTestGeom::~ICOpenTestGeom()
{
	m_pinLocationMap.empty();
}

void ICOpenTestGeom::createPinLocationMap(CCamCadData& camCadData, BlockStruct* geomBlock, BlockStruct *realPartPkgBlock)
{
   // sensorDiameter is in Page Units, i.e. same units as rest of loaded cc data

	CPoly* outlinePoly = NULL;

   // 1) Need to creat outline that is 5mm smaller than DFT outline
   for (CDataListIterator dataListIterator(*geomBlock); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
      if (data != NULL)
      {

         if (data->getDataType() == dataTypeInsert && data->getInsert()->getInsertType() == insertTypePin)
         {
            CPoint2d* newPinLoc = new CPoint2d(data->getInsert()->getOrigin2d());
            m_pinLocationMap.SetAt(data->getInsert()->getRefname(), newPinLoc);
         }
         else if (data->getDataType() == dataTypePoly && data->getPolyList() != NULL && data->getPolyList()->GetCount() == 1)
         {
            // Make sure it is either a DFT Package Outline or RealPart Outline
            if (data->getGraphicClass() != graphicClassPackageOutline && 
               data->getGraphicClass() != graphicClassPackageBody)
               continue;

            LayerStruct* layer = camCadData.getLayer(data->getLayerIndex());
            if (layer->getName().CompareNoCase(RP_LAYER_PKG_BODY_TOP)		!= 0 &&
               layer->getName().CompareNoCase(RP_LAYER_PKG_BODY_BOTTOM)	!= 0 &&
               layer->getName().CompareNoCase(DFT_OUTLINE_TOP)				!= 0 &&
               layer->getName().CompareNoCase(DFT_OUTLINE_BOTTOM)			!= 0 )
               continue;

            // Get the outline and shrink it by 5mm
            outlinePoly = new CPoly(*data->getPolyList()->GetHead());
            if (outlinePoly)
            {
               if (!outlinePoly->shrink(m_sensorDiameter, camCadData.getPageUnits()))
               {
                  delete outlinePoly;
                  outlinePoly = NULL;
                  this->m_shrinkOkay = false;
               }
               else
               {
                  outlinePoly->setFilled(false);
                  outlinePoly->setClosed(true);
               }
            }
         }
      }
   }

   // 2) If we did not get outline from compGeomBlock, then check for outline in realPartPkgBlock
   if (outlinePoly == NULL && realPartPkgBlock != NULL)
   {
      for (CDataListIterator dataListIterator(*realPartPkgBlock); dataListIterator.hasNext();)
      {
         DataStruct* data = dataListIterator.getNext();
         if (data != NULL)
         {
            if (data->getDataType() == dataTypePoly && data->getPolyList() != NULL && data->getPolyList()->GetCount() == 1)
            {
               // Make sure it is either a DFT Package Outline or RealPart Outline
               if (data->getGraphicClass() != graphicClassPackageOutline && 
                  data->getGraphicClass() != graphicClassPackageBody)
                  continue;

               LayerStruct* layer = camCadData.getLayer(data->getLayerIndex());
               if (layer->getName().CompareNoCase(RP_LAYER_PKG_BODY_TOP)		!= 0 &&
                  layer->getName().CompareNoCase(RP_LAYER_PKG_BODY_BOTTOM)	!= 0 &&
                  layer->getName().CompareNoCase(DFT_OUTLINE_TOP)				!= 0 &&
                  layer->getName().CompareNoCase(DFT_OUTLINE_BOTTOM)			!= 0 )
                  continue;

               // Get the outline and shrink it by sensorDiameter
               outlinePoly = new CPoly(*data->getPolyList()->GetHead());
               if (outlinePoly)
               {
                  if (!outlinePoly->shrink(m_sensorDiameter, camCadData.getPageUnits()))
                  {
                     delete outlinePoly;
                     outlinePoly = NULL;
                     this->m_shrinkOkay = false;
                  }
                  else
                  {
                     outlinePoly->setFilled(false);
                     outlinePoly->setClosed(true);
                  }
               }
            }
         }
      }
   }

	// 3) Need to find pin locations that are on the new outline, if there is an outline.
   if (outlinePoly != NULL)
   {
      for (POSITION pos=m_pinLocationMap.GetStartPosition(); pos != NULL;)
      {
         CString pinName;
         CPoint2d* pinLoc = NULL;
         m_pinLocationMap.GetNextAssoc(pos, pinName, pinLoc);
         if (pinLoc != NULL)
         {
            outlinePoly->distanceTo(*pinLoc, pinLoc);
         }
      }

      delete outlinePoly;
      outlinePoly = NULL;
   }
}

CPinLocationMap* ICOpenTestGeom::CreatePinLocationMapForInsert(InsertStruct* insert, int testsurface)
{
   // fyi, This does not create a PinLocationMap from scratch. It uses the generic one from
   // the comp geom and offsets it by this insert's location/rotation/side.

	if (insert == NULL || m_pinLocationMap.GetCount() == 0)
		return NULL;

   DTransform xform(insert->getOrigin2d().x, insert->getOrigin2d().y, insert->getScale(), insert->getAngle(), insert->getMirrorFlags());
	CPinLocationMap* newPinLocationMap = new CPinLocationMap();
	for (POSITION pos=m_pinLocationMap.GetStartPosition(); pos!=NULL;)
	{
		CString pinName;
		CPoint2d* pinLoc;
		m_pinLocationMap.GetNextAssoc(pos, pinName, pinLoc);
		if (pinLoc == NULL)
			continue;

		// Need to offset location by insert location, rotation, and mirror
		Point2 pnt(pinLoc->x, pinLoc->y);
		xform.TransformPoint(&pnt);

		// Fixed case #2191, need to mirror x-coordinate if it is using for bottom surface
		if (testsurface)
			pnt.x = -pnt.x;

		CPoint2d* newPinLoc = new CPoint2d(pnt.x, pnt.y);
		newPinLocationMap->SetAt(pinName, newPinLoc);
	}

	return newPinLocationMap;
}


//_____________________________________________________________________________
ICOpenTestGeomMap::ICOpenTestGeomMap()
{
	m_openTestGeomMap.empty();
}

ICOpenTestGeomMap::~ICOpenTestGeomMap()
{
	m_openTestGeomMap.empty();
}

ICOpenTestGeom* ICOpenTestGeomMap::GetICOpenTestGeom(CCamCadData& camCadData, BlockStruct* compGeomBlock, BlockStruct *realPartPkgBlock, double sensorDiameter)
{
	if (compGeomBlock == NULL)
		return NULL;

	ICOpenTestGeom* icOpenTestGeom = NULL;
	if (!m_openTestGeomMap.Lookup(compGeomBlock->getName(), icOpenTestGeom))
	{
		icOpenTestGeom = new ICOpenTestGeom(camCadData, compGeomBlock, realPartPkgBlock, sensorDiameter);
		m_openTestGeomMap.SetAt(icOpenTestGeom->GetName(), icOpenTestGeom);
	}

	return icOpenTestGeom;
}


CString refPntChoiceToString(ERefPntChoice refPntChoice)
{
	CString retval;
	switch (refPntChoice)
	{
	case refPntTopBoard:				retval = "Top Board Reference Point";				break;
	case refPntTopAuxilliary:		retval = "Top Auxilliary Reference Point";		break;
	case refPntBottomBoard:			retval = "Bottom Board Reference Point";			break;
	case refPntBottomAuxilliary:	retval = "Bottom Auxilliary Reference Point";	break;
	default:								retval = "Undefined";									break;
	}

	return retval;
}

ERefPntChoice stringToRefPntChoice(CString refPntString)
{
	ERefPntChoice retval = refPntUndefine;

	if (refPntChoiceToString(refPntTopBoard).CompareNoCase(refPntString) == 0)
		retval = refPntTopBoard;
	else if (refPntChoiceToString(refPntTopAuxilliary).CompareNoCase(refPntString) == 0)
		retval = refPntTopAuxilliary;
	else if (refPntChoiceToString(refPntBottomBoard).CompareNoCase(refPntString) == 0)
		retval = refPntBottomBoard;
	else if (refPntChoiceToString(refPntBottomAuxilliary).CompareNoCase(refPntString) == 0)
		retval = refPntBottomAuxilliary;

	return retval;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CRefPntSetting
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
CRefPntSetting::CRefPntSetting(bool isAuxilliarry)
{
	m_eRefPnt = noneRefPnt;	// 0=NONE  1=MANUAL  2=FIDUCIAL
	m_iManualX1 = "";
	m_iManualY1 = "";
	m_iManualX2 = "";
	m_iManualY2 = "";
	m_pFiducial1 = NULL;
	m_pFiducial2 = NULL;
	m_bIsAuxilliary = isAuxilliarry;
}

CRefPntSetting::~ CRefPntSetting()
{
	m_pFiducial1 = NULL;
	m_pFiducial2 = NULL;
}

CString CRefPntSetting::GetRefPntCommand(bool isBottom)
{
	CString command = "";

	if (m_bIsAuxilliary)
	{	
		CString x1Str, y1Str, x2Str, y2Str;
		x1Str = y1Str = x2Str = y2Str = "";

		if (m_eRefPnt == manualRefPnt)
		{
         // Write coords in pairs, if one of X or Y is non-blank then write both X and Y.
         // If both are blank skip it. A blank X or Y with a non-blank X or Y will get a
         // zero output.
         // Include the space delimiter in the value. Since values may turn out blank
         // we do not want the blank delimiters and no values output.
         // E.g. don't want "@H<blank><blank><blank>".
         if (!m_iManualX1.IsEmpty() || !m_iManualY1.IsEmpty())
         {
            x1Str.Format(" %d", cnv_units(atof(m_iManualX1)));
            y1Str.Format(" %d", cnv_units(atof(m_iManualY1)));
            
         }
         if (!m_iManualX2.IsEmpty() || !m_iManualY2.IsEmpty())
         {
			   x2Str.Format(" %d", cnv_units(atof(m_iManualX2)));
			   y2Str.Format(" %d", cnv_units(atof(m_iManualY2)));
         }

         // The field delimiter blanks are in the values, don't put them in the format.
			command.Format("@H%s%s%s%s", x1Str, y1Str, x2Str, y2Str);
		}
		else if (m_eRefPnt == fiducialRefPnt)
		{
         // Like above, field delimiters sent with the values, don't put in the format.
         // Easiest to do optional fields this way.

         // DR 738656 Says make second ref pt optional.
         // So act on first, report error if it is not set.
         // But use second if it is set, ignore it if it is not.
			if (m_pFiducial1 == NULL || m_pFiducial1->getInsert() == NULL)
			{
				if (isBottom)
					ErrorMessage("Reference to fiducial is in valid.  Bottom auxilliary reference by fiducial is ignored.", "Bottom Auxilliary Reference Error");
				else
					ErrorMessage("Reference to fiducial is in valid.  Top auxilliary reference by fiducial is ignored.", "Top Auxilliary Reference Error");

				return command;
			}

         // First fid, used for sure.
         y1Str.Format(" %d", cnv_units(m_pFiducial1->getInsert()->getOriginY()));
			if (isBottom)
				x1Str.Format(" %d", -cnv_units(m_pFiducial1->getInsert()->getOriginX()));
			else
				x1Str.Format(" %d", cnv_units(m_pFiducial1->getInsert()->getOriginX()));

         // Second fid, optional.
         if (m_pFiducial2 != NULL)
         {
            y2Str.Format(" %d", cnv_units(m_pFiducial2->getInsert()->getOriginY()));
			   if (isBottom)
				   x2Str.Format(" %d", -cnv_units(m_pFiducial2->getInsert()->getOriginX()));
			   else
				   x2Str.Format(" %d", cnv_units(m_pFiducial2->getInsert()->getOriginX()));
         }

			command.Format("@H%s%s%s%s", x1Str, y1Str, x2Str, y2Str);
		}
	}
	else
	{
      // No optional fields on this side, just write them.

		CString x1Str = "";
		CString y1Str = "";

		if (m_eRefPnt == manualRefPnt)
		{
			x1Str.Format("%d", cnv_units(atof(m_iManualX1)));
			y1Str.Format("%d", cnv_units(atof(m_iManualY1)));

			command.Format("@O %s %s", x1Str, y1Str);
		}
		else if (m_eRefPnt == fiducialRefPnt)
		{
			if (m_pFiducial1 == NULL || m_pFiducial1->getInsert() == NULL)
			{
				if (isBottom)
					ErrorMessage("Reference to fiducial is in valid.  Bottom board reference by fiducial is ignored.", "Bottom Board Reference Error");
				else
					ErrorMessage("Reference to fiducial is in valid.  Top board reference by fiducial is ignored.", "Top Board Reference Error");

				return command;
			}

			if (isBottom)
				x1Str.Format("%d", -cnv_units(m_pFiducial1->getInsert()->getOriginX()));
			else
				x1Str.Format("%d", cnv_units(m_pFiducial1->getInsert()->getOriginX()));

			y1Str.Format("%d", cnv_units(m_pFiducial1->getInsert()->getOriginY()));

			command.Format("@O %s %s", x1Str, y1Str);
		}
	}

	return command;
}

CPoint2d CRefPntSetting::GetReferencePoint()
{
   CPoint2d refPoint;

   if (m_eRefPnt == manualRefPnt)
	{
      refPoint.x = atof(m_iManualX1);
      refPoint.y = atof(m_iManualY1);
   }
   else if (m_eRefPnt == fiducialRefPnt)
   {
      refPoint = m_pFiducial1->getInsert()->getOrigin2d();
   }

   return refPoint;
}

// TakayaBoardPointSettingsDlg dialog
IMPLEMENT_DYNAMIC(TakayaBoardPointSettingsDlg, CDialog)

TakayaBoardPointSettingsDlg::TakayaBoardPointSettingsDlg(CCamCadData& camCadData, TakayaOriginMode originMode, CWnd* pParent /*=NULL*/)
: CDialog(TakayaBoardPointSettingsDlg::IDD, pParent)
, m_camCadData(camCadData)
//, m_camCadDoc(doc)
, m_eRefPntChoice(refPntTopBoard)
, m_eRefPnt(noneRefPnt)
, m_bICCapacitance(TRUE)
, m_bICDiode(TRUE)
, m_originMode(originMode)
{
	m_topBrdRefPntSetting = new CRefPntSetting(false);
	m_topAuxRefPntSetting =  new CRefPntSetting(true);
	m_botBrdRefPntSetting =  new CRefPntSetting(false);
	m_botAuxRefPntSetting =  new CRefPntSetting(true);
}

TakayaBoardPointSettingsDlg::~TakayaBoardPointSettingsDlg()
{
	delete m_topBrdRefPntSetting;
	delete m_topAuxRefPntSetting;
	delete m_botBrdRefPntSetting;
	delete m_botAuxRefPntSetting;
}

void TakayaBoardPointSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAKAYA_COMBO_FIDUCIAL, m_cboFiducial1);
	DDX_Control(pDX, IDC_TAKAYA_COMBO_FID2, m_cboFiducial2);
	DDX_Control(pDX, IDC_COMBO1, m_cboTestNetStatus);
	DDX_Check(pDX, IDC_IC_CAPACITANCE, m_bICCapacitance);
	DDX_Check(pDX, IDC_IC_DIODE, m_bICDiode);

	if (pDX->m_bSaveAndValidate)
	{	
		int refPntChoice = 0;
		int refPnt = 0;

		DDX_Radio(pDX, IDC_RADIO_TOP_BOARD, refPntChoice);
		DDX_Radio(pDX, IDC_TAKAYA_RADIO_NONE, refPnt);

		m_eRefPntChoice = (ERefPntChoice)refPntChoice;
		m_eRefPnt = (ERefPnt)refPnt;
	}
	else if (!pDX->m_bSaveAndValidate)
	{
		int refPntChoice = m_eRefPntChoice;
		int refPntSetting = m_eRefPnt;

		DDX_Radio(pDX, IDC_RADIO_TOP_BOARD, refPntChoice);
		DDX_Radio(pDX, IDC_TAKAYA_RADIO_NONE, refPntSetting);
	}
}


BEGIN_MESSAGE_MAP(TakayaBoardPointSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_TAKAYA_RADIO_NONE, OnBnClickedTakayaRadioNone)
	ON_BN_CLICKED(IDC_TAKAYA_RADIO_MANUAL, OnBnClickedTakayaRadioManual)
	ON_BN_CLICKED(IDC_TAKAYA_RADIO_FIDUCIAL, OnBnClickedTakayaRadioFiducial)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)

	ON_BN_CLICKED(IDC_RADIO_TOP_BOARD, OnBnClickedRefPointChoice)
	ON_BN_CLICKED(IDC_RADIO_TOP_AUX, OnBnClickedRefPointChoice)
	ON_BN_CLICKED(IDC_RADIO_BOT_BOARD, OnBnClickedRefPointChoice)
	ON_BN_CLICKED(IDC_RADIO_BOT_AUX, OnBnClickedRefPointChoice)
	ON_CBN_SELCHANGE(IDC_TAKAYA_COMBO_FIDUCIAL, OnCbnSelchangeTakayaComboFiducial1)
	ON_CBN_SELCHANGE(IDC_TAKAYA_COMBO_FID2, OnCbnSelchangeTakayaComboFiducial2)
END_MESSAGE_MAP()

BOOL TakayaBoardPointSettingsDlg::OnInitDialog()
{
	UpdateData(FALSE);

	//fill nets combo
	FileStruct* file = m_camCadData.getFileList().GetOnlyShown(blockTypePcb);
	if (!file)
		return FALSE;

	POSITION netPos = file->getNetList().GetHeadPosition();
	while (netPos)
	{
		NetStruct* net = file->getNetList().GetNext(netPos);
		if (!net)
			continue;
		m_cboTestNetStatus.SetItemData(m_cboTestNetStatus.AddString(net->getNetName()), (DWORD)net);
	}


	fillFiducialCombo();
	loadSettingFromAttribute();
	UpdateData(FALSE);

	OnBnClickedRefPointChoice();


	return TRUE;
}

void TakayaBoardPointSettingsDlg::loadSettingFromAttribute()
{
	WORD keyword = (WORD)m_camCadData.getAttributeKeywordIndex(standardAttributeTakayaReferencePointSetting);
	FileStruct* file = m_camCadData.getFileList().GetFirstShown(blockTypePcb);
	if (file == NULL)
		return;

	Attrib* attrib = NULL;
	if (file->getBlock()->getAttributes() == NULL || !file->getBlock()->getAttributes()->Lookup(keyword, attrib))
		return;
	
	CStringArray atributesStringArray;
	CSupString attributesSupString = attrib->getStringValue();
	attributesSupString.Parse(atributesStringArray, "|");
	
	for (int i=0; i<atributesStringArray.GetCount(); i++)
	{	
		CRefPntSetting* refPntSetting = NULL;
		CString attributeString = atributesStringArray.GetAt(i);
		if (attributeString.Find(refPntChoiceToString(refPntTopBoard)) > -1)
		{
			refPntSetting = m_topBrdRefPntSetting;
		}
		else if (attributeString.Find(refPntChoiceToString(refPntTopAuxilliary)) > -1)
		{
			refPntSetting = m_topAuxRefPntSetting;
		}
		else if (attributeString.Find(refPntChoiceToString(refPntBottomBoard)) > -1)
		{
			refPntSetting = m_botBrdRefPntSetting;
		}
		else if (attributeString.Find(refPntChoiceToString(refPntBottomAuxilliary)) > -1)
		{
			refPntSetting = m_botAuxRefPntSetting;	
		}
		else if (attributeString.Find("IC") > -1)
		{
			CStringArray params;
			CSupString attributeSupString = atributesStringArray.GetAt(i);
			attributeSupString.ParseQuote(params, "=");
			
			if (params.GetCount() > 1)
			{
				if (params[0].CompareNoCase(QICCapacitance) == 0)
				{
					m_bICCapacitance = params[1].CompareNoCase("FALSE")==0?FALSE:TRUE;
				}
				else if (params[0].CompareNoCase(QICDiode) == 0)
				{
					m_bICDiode = params[1].CompareNoCase("FALSE")==0?FALSE:TRUE;
				}
				else if (params[0].CompareNoCase(QICGroundNet) == 0)
				{
					m_cboTestNetStatus.SelectString(-1, params[1]);
				}
			}

			continue;
		}

		CStringArray atributeStringArray;
		CSupString attributeSupString = attributeString;
		attributeSupString.Parse(atributeStringArray, ";");

		for (int index=0; index<atributeStringArray.GetCount(); index++)
		{		
			CStringArray params;
			CSupString attributeSupString(atributeStringArray.GetAt(index));
			attributeSupString.ParseQuote(params, "=");

			if (params.GetCount() > 1)
			{
				if (params[0].CompareNoCase(QManualX1) == 0)
				{
					refPntSetting->SetManualX1(params[1]);
				}
				else if (params[0].CompareNoCase(QManualY1) == 0)
				{
					refPntSetting->SetManualY1(params[1]);
				}
				else if (params[0].CompareNoCase(QManualX2) == 0)
				{
					refPntSetting->SetManualX2(params[1]);
				}
				else if (params[0].CompareNoCase(QManualY2) == 0)
				{
					refPntSetting->SetManualY2(params[1]);
				}
				else if (params[0].CompareNoCase(QFiducial1) == 0)
				{
					refPntSetting->SetFiducial1(findFiducial(params[1]));
				}
				else if (params[0].CompareNoCase(QFiducial2) == 0)
				{
					refPntSetting->SetFiducial2(findFiducial(params[1]));
				}
			}
		}

		if (refPntSetting != NULL)
		{
			if (!refPntSetting->GetManualX1().IsEmpty() && !refPntSetting->GetManualY1().IsEmpty())
			{
				refPntSetting->SetRefPnt(manualRefPnt);
			}
			else if (refPntSetting->GetFiducial1() != NULL)
			{
				refPntSetting->SetRefPnt(fiducialRefPnt);
			}
			else
			{
				refPntSetting->SetRefPnt(noneRefPnt);
			}
		}
	}
}

void TakayaBoardPointSettingsDlg::fillFiducialCombo()
{
	m_cboFiducial1.ResetContent();
	m_cboFiducial2.ResetContent();

   int unNamedCount = 0;

	for (int i=0; i< m_camCadData.getMaxBlockIndex(); i++)
	{
		BlockStruct* block = m_camCadData.getBlockAt(i);
		if (!block)
			continue;

		POSITION insertPos = block->getHeadDataInsertPosition();
		while (insertPos)
		{
			DataStruct* fiducial = block->getNextDataInsert(insertPos);
			if (!fiducial || fiducial->getInsert()->getInsertType() != insertTypeFiducial)
				continue;

         CString optionMenuFidRefName(fiducial->getInsert()->getRefname());
         optionMenuFidRefName.Trim();
         if (optionMenuFidRefName.IsEmpty())
            optionMenuFidRefName.Format("<NoName%03d>", ++unNamedCount);
         
			m_cboFiducial1.SetItemData(m_cboFiducial1.AddString(optionMenuFidRefName), (DWORD)fiducial);
			m_cboFiducial2.SetItemData(m_cboFiducial2.AddString(optionMenuFidRefName), (DWORD)fiducial);
		}
	}
}

DataStruct* TakayaBoardPointSettingsDlg::findFiducial(CString fiducialName)
{
	for (int i=0; i< m_camCadData.getMaxBlockIndex(); i++)
	{
		BlockStruct* block = m_camCadData.getBlockAt(i);
		if (!block)
			continue;

		POSITION insertPos = block->getHeadDataInsertPosition();
		while (insertPos)
		{
			DataStruct* fiducial = block->getNextDataInsert(insertPos);
			if (!fiducial || fiducial->getInsert()->getInsertType() != insertTypeFiducial)
				continue;

			CString refdes = fiducial->getInsert()->getRefname();
			if (refdes.CompareNoCase(fiducialName) == 0)
				return fiducial;
		}
	}

	return NULL;
}

void TakayaBoardPointSettingsDlg::diableControls()
{
	m_cboFiducial1.EnableWindow(0);
	m_cboFiducial2.EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_STATIC_FID1)->EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_STATIC_FID2)->EnableWindow(0);

	GetDlgItem(IDC_TAKAYA_EDIT_X)->EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_EDIT_Y)->EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_STATIC_X)->EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_STATIC_Y)->EnableWindow(0);

	GetDlgItem(IDC_TAKAYA_EDIT_X2)->EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_EDIT_Y2)->EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_STATIC_X2)->EnableWindow(0);
	GetDlgItem(IDC_TAKAYA_STATIC_Y2)->EnableWindow(0);
}

bool TakayaBoardPointSettingsDlg::saveSetting()
{
	UpdateData(TRUE);

	CRefPntSetting* refPntSetting = NULL;
	if (m_eRefPntChoice == refPntTopBoard)
		refPntSetting = m_topBrdRefPntSetting;
	else if (m_eRefPntChoice == refPntTopAuxilliary)
		refPntSetting = m_topAuxRefPntSetting;
	else if (m_eRefPntChoice == refPntBottomBoard)
		refPntSetting = m_botBrdRefPntSetting;
	else if (m_eRefPntChoice == refPntBottomAuxilliary)
		refPntSetting = m_botAuxRefPntSetting;


	// Make sure if choose fiducial, then fiducial(s) must be selected
	if (m_eRefPnt == fiducialRefPnt)
	{
		int fidIndex = -1;
		DataStruct* fiducial = NULL;
		if ((fidIndex = m_cboFiducial1.GetCurSel()) != CB_ERR)
			fiducial = (DataStruct*)m_cboFiducial1.GetItemData(fidIndex);

		if (fiducial != NULL)
			refPntSetting->SetFiducial1((DataStruct*)m_cboFiducial1.GetItemData(fidIndex));
		else
		{
			ErrorMessage("First fiducial is not selected.  Please select a fiducial.", "No Fiducial");
			m_cboFiducial1.SetFocus();
			return false;
		}


		fidIndex = -1;
		fiducial = NULL;
		if ((fidIndex = m_cboFiducial2.GetCurSel()) != CB_ERR)
			fiducial = (DataStruct*)m_cboFiducial2.GetItemData(fidIndex);

		if (fiducial != NULL)
			refPntSetting->SetFiducial2(fiducial);
		else if (m_cboFiducial2.IsWindowEnabled() == TRUE) // only an error if combo box 2 is enable
		{
         // DR 738656 says this is not an error, requests 2nd fid be made optional.
         // That's easy, just don't report error or return. Act on setting if it was made,
         // as above in "if", but not an error in the "else", just ignore it and go.
			//ErrorMessage("Second fiducial is not selected.  Please select a fiducial.", "No Fiducial");
			//m_cboFiducial2.SetFocus();
			//return false;
		}
	}


	// Save the settings
	refPntSetting->SetRefPnt(m_eRefPnt);

	CString value;
	GetDlgItem(IDC_TAKAYA_EDIT_X)->GetWindowText(value);
   refPntSetting->SetManualX1(value);

   value.Empty();
	GetDlgItem(IDC_TAKAYA_EDIT_Y)->GetWindowText(value);
	refPntSetting->SetManualY1(value);

   value.Empty();
	GetDlgItem(IDC_TAKAYA_EDIT_X2)->GetWindowText(value);
	refPntSetting->SetManualX2(value);

   value.Empty();
	GetDlgItem(IDC_TAKAYA_EDIT_Y2)->GetWindowText(value);
	refPntSetting->SetManualY2(value);

	return true;
}

void TakayaBoardPointSettingsDlg::saveSettingToAttribute()
{
	FileStruct* file = m_camCadData.getFileList().GetFirstShown(blockTypePcb);
	if (file == NULL)
		return;

	int keyword = m_camCadData.getAttributeKeywordIndex(standardAttributeTakayaReferencePointSetting);

	CString netname;
	m_cboTestNetStatus.GetWindowText(netname);

	CString attributeString;
	attributeString.AppendFormat("%s|", getSettingString(m_topBrdRefPntSetting, refPntTopBoard));
	attributeString.AppendFormat("%s|", getSettingString(m_topAuxRefPntSetting, refPntTopAuxilliary));
	attributeString.AppendFormat("%s|", getSettingString(m_botBrdRefPntSetting, refPntBottomBoard));
	attributeString.AppendFormat("%s|", getSettingString(m_botAuxRefPntSetting, refPntBottomAuxilliary));
	attributeString.AppendFormat("%s=%s|", QICCapacitance, m_bICCapacitance?"TRUE":"FALSE");
	attributeString.AppendFormat("%s=%s|", QICDiode, m_bICDiode?"TRUE":"FALSE");
	attributeString.AppendFormat("%s=\"%s\"", QICGroundNet, netname);

	file->getBlock()->setAttrib(m_camCadData, keyword, valueTypeString, attributeString.GetBuffer(0), attributeUpdateOverwrite, NULL);
}

CString TakayaBoardPointSettingsDlg::getSettingString(CRefPntSetting* refPntSetting, ERefPntChoice refPntChoice)
{
	CString retval;

	retval.Format("%s=\"%s\"", QRefPntChoice, refPntChoiceToString(refPntChoice));
	if (refPntSetting->GetRefPnt() == manualRefPnt)
	{
		retval.AppendFormat(";%s=%s", QManualX1, refPntSetting->GetManualX1());
		retval.AppendFormat(";%s=%s", QManualY1, refPntSetting->GetManualY1());
		retval.AppendFormat(";%s=%s", QManualX2, refPntSetting->GetManualX2());
		retval.AppendFormat(";%s=%s", QManualY2, refPntSetting->GetManualY2());
	}
	else if (refPntSetting->GetRefPnt() == fiducialRefPnt)
	{
		if (refPntSetting->GetFiducial1() != NULL && refPntSetting->GetFiducial1()->getInsert() != NULL)
			retval.AppendFormat(";%s=\"%s\"", QFiducial1, refPntSetting->GetFiducial1()->getInsert()->getRefname());

		if (refPntSetting->GetFiducial2() != NULL && refPntSetting->GetFiducial2()->getInsert() != NULL)
			retval.AppendFormat(";%s=\"%s\"", QFiducial2, refPntSetting->GetFiducial2()->getInsert()->getRefname());
	}

	return retval;
}

void TakayaBoardPointSettingsDlg::OnBnClickedRefPointChoice()
{
	UpdateData(TRUE);

	CRefPntSetting* refPntSetting = NULL;
	if (m_eRefPntChoice == refPntTopBoard)
		refPntSetting = m_topBrdRefPntSetting;
	else if (m_eRefPntChoice == refPntTopAuxilliary)
		refPntSetting = m_topAuxRefPntSetting;
	else if (m_eRefPntChoice == refPntBottomBoard)
		refPntSetting = m_botBrdRefPntSetting;
	else if (m_eRefPntChoice == refPntBottomAuxilliary)
		refPntSetting = m_botAuxRefPntSetting;


	// Reset by clearing textbos and selections and diabling the controls
	m_cboFiducial1.SetCurSel(-1);
	m_cboFiducial2.SetCurSel(-1);
	GetDlgItem(IDC_TAKAYA_EDIT_X)->SetWindowText("");
	GetDlgItem(IDC_TAKAYA_EDIT_Y)->SetWindowText("");
	GetDlgItem(IDC_TAKAYA_EDIT_X2)->SetWindowText("");
	GetDlgItem(IDC_TAKAYA_EDIT_Y2)->SetWindowText("");


	// Apply the settings
	GetDlgItem(IDC_TAKAYA_EDIT_X)->SetWindowText(refPntSetting->GetManualX1());
	GetDlgItem(IDC_TAKAYA_EDIT_Y)->SetWindowText(refPntSetting->GetManualY1());
	GetDlgItem(IDC_TAKAYA_EDIT_X2)->SetWindowText(refPntSetting->GetManualX2());
	GetDlgItem(IDC_TAKAYA_EDIT_Y2)->SetWindowText(refPntSetting->GetManualY2());

	if (refPntSetting->GetFiducial1() != NULL)
   {
      // DR 691215 Subtle badness in .SelectString(...), the string param is a prefix, not
      // an exact match string, so this has trouble mixing up, e.g. "C1" and "C10".
      // So don't do the following:
      //m_cboFiducial1.SelectString(-1, refPntSetting->GetFiducial1()->getInsert()->getRefname());

      // Instead, find index for exact string, if it is found then set selection.
      int sel = m_cboFiducial1.FindStringExact(-1, refPntSetting->GetFiducial1()->getInsert()->getRefname());
      if (sel != CB_ERR)
         m_cboFiducial1.SetCurSel(sel);
   }

	if (refPntSetting->GetFiducial2() != NULL)
   {
      int sel = m_cboFiducial2.FindStringExact(-1, refPntSetting->GetFiducial2()->getInsert()->getRefname());
      if (sel != CB_ERR)
         m_cboFiducial2.SetCurSel(sel);
   }


	// Eable the control accordingly
	if (refPntSetting->GetRefPnt() == noneRefPnt)
		OnBnClickedTakayaRadioNone();
	else if (refPntSetting->GetRefPnt() == manualRefPnt)
		OnBnClickedTakayaRadioManual();
	else if (refPntSetting->GetRefPnt() == fiducialRefPnt)
		OnBnClickedTakayaRadioFiducial();
}

void TakayaBoardPointSettingsDlg::OnBnClickedTakayaRadioNone()
{
	m_eRefPnt = noneRefPnt;
	diableControls();

	UpdateData(FALSE);
}

void TakayaBoardPointSettingsDlg::OnBnClickedTakayaRadioManual()
{
	m_eRefPnt = manualRefPnt;
	diableControls();

	GetDlgItem(IDC_TAKAYA_EDIT_X)->EnableWindow(1);
	GetDlgItem(IDC_TAKAYA_EDIT_Y)->EnableWindow(1);
	GetDlgItem(IDC_TAKAYA_STATIC_X)->EnableWindow(1);
	GetDlgItem(IDC_TAKAYA_STATIC_Y)->EnableWindow(1);
	
	if (m_eRefPntChoice == refPntTopAuxilliary || m_eRefPntChoice == refPntBottomAuxilliary)
	{
		GetDlgItem(IDC_TAKAYA_EDIT_X2)->EnableWindow(1);
		GetDlgItem(IDC_TAKAYA_EDIT_Y2)->EnableWindow(1);
		GetDlgItem(IDC_TAKAYA_STATIC_X2)->EnableWindow(1);
		GetDlgItem(IDC_TAKAYA_STATIC_Y2)->EnableWindow(1);
	}

	UpdateData(FALSE);
}

void TakayaBoardPointSettingsDlg::OnBnClickedTakayaRadioFiducial()
{
	m_eRefPnt = fiducialRefPnt;
	diableControls();

	m_cboFiducial1.EnableWindow(1);
	GetDlgItem(IDC_TAKAYA_STATIC_FID1)->EnableWindow(1);

	if (m_eRefPntChoice == refPntTopAuxilliary || m_eRefPntChoice == refPntBottomAuxilliary)
	{
		m_cboFiducial2.EnableWindow(1);
		GetDlgItem(IDC_TAKAYA_STATIC_FID2)->EnableWindow(1);
	}

	UpdateData(FALSE);
}

void TakayaBoardPointSettingsDlg::OnBnClickedApply()
{
	saveSetting();
}

void TakayaBoardPointSettingsDlg::OnBnClickedOk()
{
	int nIndex;
	if ((nIndex = m_cboTestNetStatus.GetCurSel()) != CB_ERR)
	{
		NetStruct* net = (NetStruct*)m_cboTestNetStatus.GetItemData(nIndex);
      m_camCadData.setAttribute(net->attributes(), standardAttributeTestNetStatus, "Ground");
   }
	else
	{
		ErrorMessage("Please select a test net.");
		return;
	}

	if (saveSetting())
	{
		saveSettingToAttribute();

		OnOK();
	}
}

void TakayaBoardPointSettingsDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void TakayaBoardPointSettingsDlg::OnCbnSelchangeTakayaComboFiducial1()
{
	int fidIndex;
	if ((fidIndex = m_cboFiducial1.GetCurSel()) != CB_ERR)
	{
		DataStruct* fiducial = (DataStruct*)m_cboFiducial1.GetItemData(fidIndex);
		if (fiducial != NULL)
			PanReference(getActiveView(), fiducial->getInsert()->getRefname());
	}
}

void TakayaBoardPointSettingsDlg::OnCbnSelchangeTakayaComboFiducial2()
{
	int fidIndex;
	if ((fidIndex = m_cboFiducial2.GetCurSel()) != CB_ERR)
	{
		DataStruct* fiducial = (DataStruct*)m_cboFiducial2.GetItemData(fidIndex);
		if (fiducial != NULL)
			PanReference(getActiveView(), fiducial->getInsert()->getRefname());
	}
}


//-----------------------------------------------------------------------------
// CPinToPinShortAnalyzer
//-----------------------------------------------------------------------------
CPcbComponentPinToPinAnalyzer::CPcbComponentPinToPinAnalyzer(BlockStruct& block)
	: CTypedMapSortStringToObContainer<CPcbComponentPinToPinShort>(nextPrime2n(200))
	, m_block(block)
{
}

CPcbComponentPinToPinAnalyzer::~CPcbComponentPinToPinAnalyzer()
{
}

void CPcbComponentPinToPinAnalyzer::analysisPinToPinShort()
{
	if (pinToPinDistance < 0)
		return;

	CTypedPtrArrayContainer<InsertStruct*> pinArray1(50, false);
	CTypedPtrArrayContainer<InsertStruct*> pinArray2(50, false);

	for (CDataListIterator dataList(m_block, insertTypePin); dataList.hasNext();)
	{
		InsertStruct* pinInsert = dataList.getNext()->getInsert();
		pinArray1.SetAtGrow(pinArray1.GetCount(), pinInsert);
		pinArray2.SetAtGrow(pinArray2.GetCount(), pinInsert);
	}

	for (int index1=0; index1<pinArray1.GetCount(); index1++)
	{
		InsertStruct* pinInsert1 = pinArray1.GetAt(index1);
		InsertStruct* closestPinInsert = NULL;

		double shortestDistance = DBL_MAX;
		for (int index2=index1+1; index2<pinArray2.GetCount(); index2++)
		{
			InsertStruct* pinInsert2 = pinArray2.GetAt(index2);

			if (pinInsert1 != NULL && pinInsert2 != NULL)
			{
				double distance = pinInsert1->getOrigin2d().distance(pinInsert2->getOrigin2d());
				if (distance < shortestDistance)
				{
					shortestDistance = distance;
					closestPinInsert = pinInsert2;
				}
			}
		}

		// Add pin to pin short
		if (shortestDistance < pinToPinDistance && pinInsert1 != NULL && closestPinInsert != NULL)
		{
			if (pinInsert1->getSortableRefDes() < closestPinInsert->getSortableRefDes())
			{
				CPcbComponentPinToPinShort* pinToPinShort = new CPcbComponentPinToPinShort(pinInsert1->getRefname(), closestPinInsert->getRefname());
				this->SetAt(pinInsert1->getSortableRefDes(), pinToPinShort);
			}
			else
			{
				CPcbComponentPinToPinShort* pinToPinShort = new CPcbComponentPinToPinShort(closestPinInsert->getRefname(), pinInsert1->getRefname());
				this->SetAt(closestPinInsert->getSortableRefDes(), pinToPinShort);
			}
		}
	}

	this->Sort();
}


//-----------------------------------------------------------------------------
// CPowerRailShortOutput
//-----------------------------------------------------------------------------
CPowerRailShortOutput::CPowerRailShortOutput()
{
}

CPowerRailShortOutput::~CPowerRailShortOutput()
{
}


//-----------------------------------------------------------------------------
// CPinToPinOutput
//-----------------------------------------------------------------------------
CPinToPinOutput::CPinToPinOutput() 
{
}

CPinToPinOutput::~CPinToPinOutput() 
{
}


//-----------------------------------------------------------------------------
// CPinToPinShortGeomMap
//-----------------------------------------------------------------------------
CPinToPinShortTest::CPinToPinShortTest(CCamCadDatabase& camcadDatabase)
	: m_camCadDatabase(camcadDatabase)
{
	m_pcbComponentPinToPinAnalyzerMap.InitHashTable(nextPrime2n(200));
	m_testedPowerGroundNetMap.InitHashTable(nextPrime2n(200));
	m_testedComponentMap.InitHashTable(nextPrime2n(200));
	m_powerNetCount = 0;
	m_groundNetCount = 0;
}

CPinToPinShortTest::~CPinToPinShortTest()
{
	m_pcbComponentPinToPinAnalyzerMap.empty();
	m_testedPowerGroundNetMap.RemoveAll();
	m_testedComponentMap.RemoveAll();
	m_powerRailOutputList.empty();
	m_pinToPinOutputList.empty();
}

CPcbComponentPinToPinAnalyzer* CPinToPinShortTest::getPinToPinShortGeom(int blockNumber)
{
	CPcbComponentPinToPinAnalyzer* pcbComponentPinToPinAnalyzer = NULL;
	if (!m_pcbComponentPinToPinAnalyzerMap.Lookup(blockNumber, pcbComponentPinToPinAnalyzer))
	{
		BlockStruct* block = m_camCadDatabase.getBlock(blockNumber);
		if (block != NULL)
		{
			pcbComponentPinToPinAnalyzer = new CPcbComponentPinToPinAnalyzer(*block);
			pcbComponentPinToPinAnalyzer->analysisPinToPinShort();
			m_pcbComponentPinToPinAnalyzerMap.SetAt(blockNumber, pcbComponentPinToPinAnalyzer);
		}
	}

	return pcbComponentPinToPinAnalyzer;
}

void CPinToPinShortTest::doShortTests(FileStruct& pcbFile, int maxhit, bool bottom)
{
	doPowerRailShortTest(pcbFile, maxhit, bottom);
	doPinToPinShortTest(pcbFile, maxhit, bottom);
}

void CPinToPinShortTest::doPowerRailShortTest(FileStruct& pcbFile, int maxhit, bool bottom)
{
	WORD netTypeKw = (WORD)m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeNetType);

	CString powerNetName;
	CString groundNetName;
	CTypedMapStringToPtrContainer<NetStruct*> netMap1(nextPrime2n(200), false);
	CTypedMapStringToPtrContainer<NetStruct*> netMap2(nextPrime2n(200), false);

	for (POSITION pos = pcbFile.getNetList().GetHeadPosition(); pos != NULL;)
	{
		NetStruct* net = pcbFile.getNetList().GetNext(pos);
		Attrib* attrib = NULL;
		if (net == NULL || net->getAttributes() == NULL || !net->getAttributes()->Lookup(netTypeKw, attrib))
			continue;

		CString netName;
		if (m_testedPowerGroundNetMap.Lookup(net->getNetName(), netName))
			continue;

		CString netType = attrib->getStringValue();
		if (netType.CompareNoCase("POWER") == 0)
		{
			powerNetName.Format("P%d", ++m_powerNetCount);
			netMap1.SetAt(powerNetName, net);
			netMap2.SetAt(powerNetName, net);
		}		
		else if (netType.CompareNoCase("GROUND") == 0)
		{
			groundNetName.Format("G%d", ++m_groundNetCount);
			netMap1.SetAt(groundNetName, net);
			netMap2.SetAt(groundNetName, net);
		}
	}

	for (POSITION pos1 = netMap1.GetStartPosition(); pos1 != NULL;)
	{
		NetStruct* net1 = NULL;
		CString powerGroundName1;
		netMap1.GetNextAssoc(pos1, powerGroundName1, net1);
		if (net1 == NULL)
			continue;

		int netindex1 = get_netnameptr(net1->getNetName());
		int testprobeptr1 = get_testprobe(netindex1, maxhit, bottom?TRUE:FALSE);
		if (testprobeptr1 < 0)
			continue;
		TK_testaccess* testAccess1 = testaccessarray[testprobeptr1];

		for (POSITION pos2 = pos1; pos2 != NULL;)
		{
			NetStruct* net2 = NULL;
			CString powerGroundName2;
			netMap2.GetNextAssoc(pos2, powerGroundName2, net2);
			if (net2 == NULL)
				continue;

			int netindex2 = get_netnameptr(net2->getNetName());
			int testprobeptr2 = get_testprobe(netindex2, maxhit, bottom?TRUE:FALSE);
			if (testprobeptr2 < 0)
				continue;
			TK_testaccess* testAccess2 = testaccessarray[testprobeptr2];

			if (pcbFile.isMirrored())
				bottom = !bottom;

			CTMatrix matrix = pcbFile.getTMatrix();
			CPoint2d testaccessPoint1(testAccess1->x, testAccess1->y);
			CPoint2d testaccessPoint2(testAccess2->x, testAccess2->y);
			matrix.transform(testaccessPoint1);
			matrix.transform(testaccessPoint2);

			if (bottom)
			{
				testaccessPoint1.x = -testaccessPoint1.x;
				testaccessPoint2.x = -testaccessPoint2.x;
			}

			CPowerRailShortOutput* powerRailOutput = new CPowerRailShortOutput();
			powerRailOutput->setRefdes(powerGroundName1 + powerGroundName2);
			powerRailOutput->setNetName1(net1->getNetName());
			powerRailOutput->setNetName2(net2->getNetName());
			powerRailOutput->setTestAccess1(cnv_units(testaccessPoint1.x), cnv_units(testaccessPoint1.y));
			powerRailOutput->setTestAccess2(cnv_units(testaccessPoint2.x), cnv_units(testaccessPoint2.y));
			powerRailOutput->setBottom(bottom);
			powerRailOutput->setTestAccessEntityNumber1(testAccess1->entityNumber);
			powerRailOutput->setTestAccessEntityNumber2(testAccess2->entityNumber);

			m_powerRailOutputList.AddTail(powerRailOutput);
		}

		m_testedPowerGroundNetMap.SetAt(net1->getNetName(), net1->getNetName());
	}
}

void CPinToPinShortTest::doPinToPinShortTest(FileStruct& pcbFile, int maxhit, bool bottom)
{
	if (shortsType.CompareNoCase(TK_SHORTS_PIN_TO_PIN) != 0 || pinToPinDeviceTypeMap.GetCount() == 0 || pinToPinDistance < 0)
		return;

	BlockStruct* fileBlock = pcbFile.getBlock();
	if (fileBlock == NULL)
		return;

	WORD deviceTypeKw = (WORD)m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDeviceType);
	WORD partNumberKw = (WORD)m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributePartNumber);
	for (CDataListIterator dataList(*fileBlock, insertTypePcbComponent); dataList.hasNext();)
	{
		DataStruct* compData = dataList.getNext();
		Attrib* attrib = NULL;
		if (compData->getAttributes() == NULL || !compData->getAttributes()->Lookup(deviceTypeKw, attrib))
			continue;

		CString compRefdes = compData->getInsert()->getRefname();
		if (m_testedComponentMap.Lookup(compRefdes, compRefdes))
			continue;

		CString deviceType = attrib->getStringValue();
		deviceType.MakeUpper();
		if (!pinToPinDeviceTypeMap.Lookup(deviceType, deviceType))
			continue;

		CString partNumber;
		if (compData->getAttributes()->Lookup(partNumberKw, attrib))
		{
			partNumber = attrib->getStringValue();
		}

		CPcbComponentPinToPinAnalyzer* pcbCompPinToPinAnalyzer = getPinToPinShortGeom(compData->getInsert()->getBlockNumber());
		if (pcbCompPinToPinAnalyzer == NULL)
			continue;

		int unusedPin = get_testpins(compRefdes, &pcbFile.getNetList(), maxhit, bottom?1:0);
		if (testkoocnt > 0)
		{	
			// Add pin to pin short to output
			//for (POSITION pos = pcbCompPinToPinAnalyzer->GetStartPosition(); pos != NULL;)
			CPcbComponentPinToPinShort* pinToPinShort;
			CString *key = NULL;
			pcbCompPinToPinAnalyzer->GetFirstSorted(key, pinToPinShort);
			while (pinToPinShort != NULL)
			{
				TK_testkoo* testkoo1 = getTestkooByPinNum(pinToPinShort->getPinRefdes1());
				TK_testkoo* testkoo2 = getTestkooByPinNum(pinToPinShort->getPinRefdes2());
				if (testkoo1 == NULL || testkoo2 == NULL)
				{
					pcbCompPinToPinAnalyzer->GetNextSorted(key, pinToPinShort);
					continue;
				}

				TK_testaccess* testaccess1 =  testaccessarray[testkoo1->testprobeptr];
				TK_testaccess* testaccess2 =  testaccessarray[testkoo2->testprobeptr];
				if (testaccess1 == NULL || testaccess2 == NULL)
				{
					pcbCompPinToPinAnalyzer->GetNextSorted(key, pinToPinShort);
					continue;
				}

				TK_netname* netname1 = ((testaccess1->netindex != -1) ? netnamearray.getAt(testaccess1->netindex) : NULL);
				TK_netname* netname2 = ((testaccess2->netindex != -1) ? netnamearray.getAt(testaccess2->netindex) : NULL);
				if (netname1 == NULL || netname2 == NULL)
				{
					pcbCompPinToPinAnalyzer->GetNextSorted(key, pinToPinShort);
					continue;
				}

				if (pcbFile.isMirrored())
					bottom = !bottom;

				CTMatrix matrix = pcbFile.getTMatrix();
				CPoint2d testaccessPoint1(testaccess1->x, testaccess1->y);
				CPoint2d testaccessPoint2(testaccess2->x, testaccess2->y);
				matrix.transform(testaccessPoint1);
				matrix.transform(testaccessPoint2);

				if (bottom)
				{
					testaccessPoint1.x = -testaccessPoint1.x;
					testaccessPoint2.x = -testaccessPoint2.x;
				}

				CPinToPinOutput* pinToPinOutput = new CPinToPinOutput();
				pinToPinOutput->setCompRefdes(compRefdes);
				pinToPinOutput->setPartNumber(partNumber);
				pinToPinOutput->setPinRefdes1(pinToPinShort->getPinRefdes1());
				pinToPinOutput->setPinRefdes2(pinToPinShort->getPinRefdes2());
				pinToPinOutput->setNetName1(netname1->netname);
				pinToPinOutput->setNetName2(netname2->netname);
				pinToPinOutput->setTestAccess1(cnv_units(testaccessPoint1.x), cnv_units(testaccessPoint1.y));
				pinToPinOutput->setTestAccess2(cnv_units(testaccessPoint2.x), cnv_units(testaccessPoint2.y));
				pinToPinOutput->setBottom(bottom);
				pinToPinOutput->setTestAccessEntityNumber1(testaccess1->entityNumber);
				pinToPinOutput->setTestAccessEntityNumber2(testaccess2->entityNumber);

				m_pinToPinOutputList.AddTail(pinToPinOutput);

				pcbCompPinToPinAnalyzer->GetNextSorted(key, pinToPinShort);
			}

			m_testedComponentMap.SetAt(compRefdes, compRefdes);
		}
		free_testpins();
	}
}

void CPinToPinShortTest::writePinToPinOutput(FILE* outputFile, bool bottom)
{
	CString probeNum1Num2, probeNum2Num1;
	CMapStringToString alreadyTestProbePair;

	for (POSITION pos = m_powerRailOutputList.GetHeadPosition(); pos != NULL;)
	{
		CPowerRailShortOutput* powerRailOutput = m_powerRailOutputList.GetNext(pos);
		if (powerRailOutput != NULL && powerRailOutput->isBottom() == bottom)
		{
			int probeNum1 = get_ProbeNumber(powerRailOutput->getTestAccessX1(), powerRailOutput->getTestAccessY1(),
													  powerRailOutput->getNetName1(), TRUE, powerRailOutput->isBottom()?TRUE:FALSE, powerRailOutput->getTestAccessEntityNumber1());
			int probeNum2 = get_ProbeNumber(powerRailOutput->getTestAccessX2(), powerRailOutput->getTestAccessY2(),
													  powerRailOutput->getNetName2(), TRUE, powerRailOutput->isBottom()?TRUE:FALSE, powerRailOutput->getTestAccessEntityNumber2());


			probeNum1Num2.Format("N%dN%d", probeNum1, probeNum2);
			probeNum2Num1.Format("N%dN%d", probeNum2, probeNum1);
			if (alreadyTestProbePair.Lookup(probeNum1Num2, probeNum1Num2) || 
				 alreadyTestProbePair.Lookup(probeNum2Num1, probeNum2Num1))
			{
				fprintf(flog, "Duplicate shorts test not generated %s P%s-%s\n",  powerRailOutput->getRefdes(), 
					powerRailOutput->getNetName1(), powerRailOutput->getNetName2());
				continue;
			}
			alreadyTestProbePair.SetAt(probeNum1Num2, probeNum1Num2);
			alreadyTestProbePair.SetAt(probeNum2Num1, probeNum2Num1);

			fprintf(outputFile, " %s P-Auto %s-%s * R N%d N%d @K OP\n", powerRailOutput->getRefdes(), 
				powerRailOutput->getNetName1(), powerRailOutput->getNetName2(), probeNum1, probeNum2);
		}
	}

	for (POSITION pos = m_pinToPinOutputList.GetHeadPosition(); pos != NULL;)
	{
		CPinToPinOutput* pinToPinOutput = m_pinToPinOutputList.GetNext(pos);
		if (pinToPinOutput != NULL && pinToPinOutput->isBottom() == bottom)
		{
			int probeNum1 = get_ProbeNumber(pinToPinOutput->getTestAccessX1(), pinToPinOutput->getTestAccessY1(),
													  pinToPinOutput->getNetName1(), TRUE, pinToPinOutput->isBottom()?TRUE:FALSE, pinToPinOutput->getTestAccessEntityNumber1());
			int probeNum2 = get_ProbeNumber(pinToPinOutput->getTestAccessX2(), pinToPinOutput->getTestAccessY2(),
													  pinToPinOutput->getNetName2(), TRUE, pinToPinOutput->isBottom()?TRUE:FALSE, pinToPinOutput->getTestAccessEntityNumber2());

			if (probeNum1 == probeNum2)
			{
				fprintf(flog, "Shorts test not generated %s P%s-%s pins on the same net\n",  pinToPinOutput->getCompRefdes(), 
					pinToPinOutput->getPinRefdes1(), pinToPinOutput->getPinRefdes2());
				continue;
			}
			else
			{
				probeNum1Num2.Format("N%dN%d", probeNum1, probeNum2);
				probeNum2Num1.Format("N%dN%d", probeNum2, probeNum1);
				if (alreadyTestProbePair.Lookup(probeNum1Num2, probeNum1Num2) || 
					 alreadyTestProbePair.Lookup(probeNum2Num1, probeNum2Num1))
				{
					fprintf(flog, "Duplicate shorts test not generated %s P%s-%s\n",  pinToPinOutput->getCompRefdes(), 
						pinToPinOutput->getPinRefdes1(), pinToPinOutput->getPinRefdes2());
					continue;
				}
				alreadyTestProbePair.SetAt(probeNum1Num2, probeNum1Num2);
				alreadyTestProbePair.SetAt(probeNum2Num1, probeNum2Num1);


				if (pinToPinOutput->getPartNumber().IsEmpty())
				{
					fprintf(outputFile, " %s P%s-%s * * R N%d N%d @K OP\n", pinToPinOutput->getCompRefdes(), 
						pinToPinOutput->getPinRefdes1(), pinToPinOutput->getPinRefdes2(), probeNum1, probeNum2);
				}
				else
				{
					fprintf(outputFile, " %s P%s-%s %s * R N%d N%d @K OP\n", pinToPinOutput->getCompRefdes(), 
						pinToPinOutput->getPinRefdes1(), pinToPinOutput->getPinRefdes2(), pinToPinOutput->getPartNumber(),
						probeNum1, probeNum2);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// CIcDiodeCommandMap
//-----------------------------------------------------------------------------
CIcDiodeCommandMap::CIcDiodeCommandMap()
   : m_icDiodeCommandMap(nextPrime2n(200),true)
{
}

CIcDiodeCommandMap::~CIcDiodeCommandMap()
{
   empty();
}

void CIcDiodeCommandMap::addCommand(const CString partNumber, const CString pinName, const bool swap)
{
   CIcDiodeCommand* icDiodeCommand = NULL;
   CString upperCasePartNumber = partNumber;
   upperCasePartNumber.MakeUpper();
   if (m_icDiodeCommandMap.Lookup(upperCasePartNumber, icDiodeCommand))
   {
      delete icDiodeCommand;
      icDiodeCommand = NULL;
   }

   icDiodeCommand = new CIcDiodeCommand(upperCasePartNumber, pinName, swap);
   m_icDiodeCommandMap.SetAt(upperCasePartNumber, icDiodeCommand);
}

int CIcDiodeCommandMap::getGroundPin(const CString partNumber, const CString compName, const int maxhits, const int testSurface, long& groundPinX, long& groundPinY, bool& swap)
{
   int groundTestProbeIndex = -1;
   CIcDiodeCommand* icDiodeCommand = NULL;
   CString upperCasePartNumber = partNumber;
   upperCasePartNumber.MakeUpper();
   
   if (m_icDiodeCommandMap.Lookup(upperCasePartNumber, icDiodeCommand))
   { 
      TK_pintest* pintest = update_pintest(compName, icDiodeCommand->getPinName(), testNone);
      if (pintest != NULL)
      {
         if (pintest->testprobeptr > -1)
         {
            TK_testaccess* testAccess = testaccessarray[pintest->testprobeptr];
            if (testAccess == NULL || testAccess->usecnt >= maxhits)
            {
               pintest->testprobeptr = get_testprobe(pintest->netnameindex, maxhits, testSurface);
            }

            testAccess = testaccessarray[pintest->testprobeptr];
            if (testAccess != NULL)
            {
               groundPinX = cnv_units(testAccess->x);
               groundPinY = cnv_units(testAccess->y);
               groundTestProbeIndex = pintest->testprobeptr;
               swap = icDiodeCommand->isSwap();
            }
         }
      }
   }

   return groundTestProbeIndex;
}