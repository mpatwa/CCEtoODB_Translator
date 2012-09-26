// $Header: /CAMCAD/5.0/Ta_Probe.cpp 22    6/30/07 2:38a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ta_probe.h"
#include "ta_optn.h"
#include "ccdoc.h"
#include "gauge.h"
#include "ta.h"
#include "ta_reprt.h"
#include "drc.h"
#include "drc_util.h"
#include "graph.h"
#include "crypt.h"
#include "pcbutil.h"
#include <float.h>
#include <math.h>
#include "CCEtoODB.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char *testaccesslayers[]; // from DBUTIL.CPP

extern TA_OptionsStruct TAoptions; // from TA_OPTN.CPP

extern TA_NetnameArray TA_netnameArray; // from TA_REPRT.CPP
extern int TA_netnameArrayCount; // from TA_REPRT.CPP

extern PinkooArray pinkooarray; // from TA.CPP
extern int pinkoocnt; // from TA.CPP

TA_PCBFileProbePlace TApcbfileprobeplace;

static ProbePlaceArray probePlaceArray;
static int probePlaceArrayCount;

static ProbePlaceNetArray probePlaceNetArray;
static int probePlaceNetArrayCount;

static void TA_process_probeplacement(CCEtoODBDoc *doc, FileStruct *pcbFile, TestProbeList *probelist);
static void do_probe_place(CCEtoODBDoc *doc, FileStruct *file, TestProbeList *probelist);
static void init_probeplace_allowedlayer();
static int probeplace_load(FILE *taLog, FileStruct *file, CCEtoODBDoc *doc, int eval_nonconn);
static int probeplace_number_of_probes(FileStruct *file, CCEtoODBDoc *doc);
static void probeplace_net_accesspoints();
static void update_probelayercost(int l); // 0 == top prefered, 1 bottom pref.
static void clear_probeplace();
static void do_probeplace(FILE *flog, int layer, TestProbeList *probelist);
static void update_probecnt(TestProbeList *probelist);
static int get_probeplace_from_netindex(int netindex);
static int get_probeplacefails(FILE *flog, int iterationcnt);
static TestProbeStruct *get_smallestprobesize(TestProbeList *probelist);
static TestProbeStruct *get_nextprobesize(TestProbeList *probelist, double prevsiz);
static int do_probeplace_net(FILE *flog, int netindex, int layer, TestProbeList *probelist);
static int do_optimizeprobeplace(FILE *flog, int layer, TestProbeList *probelist, int tp_index, double tp_size, 
      double tp_headsize, const char *tp_probename, const char *tp_probeprefix);
static int probeplace_calc_placed(CCEtoODBDoc *doc, FileStruct *file);
static TestProbeStruct *get_testprobestruct(TestProbeList *probelist, int index);
static void FillTestProbeList(CCEtoODBDoc *doc, TestProbeList *list);
static int probeplace_distance(double siz, int layer, double x, double y);
static int get_probeplacenetindex(const char *n, int nonconn);
static int get_highesttotalaccesscnt(int layer);

/******************************************************************************
* OnTestProbePlacement
*/
void CCEtoODBDoc::OnTestProbePlacement() 
{
   CString defaultFile = getApp().getSystemSettingsFilePath("default.dft");

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft)) 
   {
      ErrorAccess("You do not have a License for Test Probe Placement!");
      return;
   }*/

   int stat = 0; // 0 = no PCB file, 1 = PCB, but not visible, 2 = PCB and visible

   FileStruct *pcbFile = NULL;
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);
      if (file->getBlockType() == BLOCKTYPE_PCB)
      {
         if (file->isShown())
         {
            stat |= 2;
            pcbFile = file;
            break;
         }
         else
         {
            stat |= 1;
         }
      }
   }

   if (!pcbFile)
   {
      if (stat == 1)
         ErrorMessage("PCB File loaded, but not visible -> will be ignored!");
      else
         ErrorMessage("No PCB File loaded!");
      return;
   }

   if (!ta_options_loaded)
   {
      // load from file 
      TAload_defaultsettings(this, defaultFile, &TAoptions, getSettings().getPageUnits());
   }

   // here build the testprobeplace array.
   // the idea is to load test_access into the probeplacearray.
   //TA_TestProbeInit();
   if (!TA_TestProbeInit(this, pcbFile, TAoptions.Unusedpins)) 
   {
      TA_TestProbeDeInit();
      return;
   }

   TestProbePlacement dlg;
   dlg.doc = this;
   dlg.pcbFile = pcbFile;

   // this is number of accessable nets (number of total nets)
   dlg.m_totalnets.Format("%d", TAoptions.ProbePlace.totalaccessnets);        // total accessable nets
   dlg.m_totalnets_access.Format("(%d)", TAoptions.ProbePlace.totnets); // total all nets
   if (TAoptions.Unusedpins)
   {
      dlg.m_singleportnets.Format("%d", TAoptions.ProbePlace.singleaccessnets);// total accessable single pin nets
      dlg.m_singleportnets_access.Format("(%d)", TAoptions.ProbePlace.snets);
   }
   else
   {
      dlg.m_singleportnets = "-";
      dlg.m_singleportnets_access = " ";
   }
   dlg.m_multiportnets.Format("%d", TAoptions.ProbePlace.multiaccessnets);
   dlg.m_multiportnets_access.Format("(%d)", TAoptions.ProbePlace.mnets);

   dlg.DoModal();

}

/******************************************************************************
* TA_TestProbeInit
   LOAD PCB file (first visible) and init all array etc...
   delete LOG file
*/
int TA_TestProbeInit(CCEtoODBDoc *doc, FileStruct *pcbFile, int eval_nonconn)
{
   CString taLogFile = GetLogfilePath("ta.log");
   remove(taLogFile);

   FILE *taLog = fopen(taLogFile, "wt");
   if (!taLog)
   {
      CString err;
      err.Format("Error open [%s] file", taLogFile);
      ErrorMessage(err, "Error");
      return 0;
   }

   fclose(taLog);

   TA_netnameArray.SetSize(100,100);
   TA_netnameArrayCount = 0;
   
   CWaitCursor w;

   // load netlist into TA_netnameArray including the refgistration of NONCONN and single pin nets
   report_load_netlist(pcbFile, doc);

   // now I am ready to do all types of analysis
   probePlaceArray.SetSize(100,100);
   probePlaceArrayCount = 0; // counts access points and updates this array with probe placement info during process
   probePlaceNetArray.SetSize(100,100);
   probePlaceNetArrayCount = 0;

   int already_placed_probes = probeplace_load(taLog, pcbFile, doc, eval_nonconn);
   probeplace_number_of_probes(pcbFile, doc);

   if (already_placed_probes)
   {
      if (ErrorMessage("Previous placed Probes will be deleted during this process.\nDo you want to Continue?", "Continue Test Probe Placement", MB_YESNO | MB_DEFBUTTON2)!=IDYES)
         return 0;
   }

   if (probePlaceArrayCount == 0)
   {
      ErrorMessage(" To place Test Probes, you must run Test Access first!", "No Test Access Information found!");
      return -1;
   }


   // here update TA_ProbePlaceNet. 
	int i=0;
   for (i=0;i<probePlaceNetArrayCount;i++)
   {
#ifdef _DEBUG
      TA_ProbePlaceNet *nn = probePlaceNetArray[i];
#endif
      int netindex = get_report_netnameptr(probePlaceNetArray[i]->netname);
      if (TA_netnameArray[netindex]->nonconn)
         probePlaceNetArray[i]->nonconn = TA_netnameArray[netindex]->nonconn;
   }

   TAoptions.ProbePlace.mnets = 0;
   TAoptions.ProbePlace.snets = 0;
   for (i=0;i<TA_netnameArrayCount;i++)
   {
      TA_Netname *net = TA_netnameArray[i];

      if (net->nonconn)
         TAoptions.ProbePlace.snets++;
      else
         TAoptions.ProbePlace.mnets++;
   }

   if (!TAoptions.Unusedpins)
      TAoptions.ProbePlace.snets = 0;

   TAoptions.ProbePlace.totnets = TAoptions.ProbePlace.mnets + TAoptions.ProbePlace.snets;

   // calc nets for accessable
   probeplace_net_accesspoints();

   return 1;
}

/*************************************************************************
*  TA_TestProbeDeInit
   
   remove TA_ProbePlace array
*/
void TA_TestProbeDeInit()
{
   int   i;

   for (i=0;i<probePlaceArrayCount;i++)
   {
      delete probePlaceArray[i];
   }
   probePlaceArray.RemoveAll();
   probePlaceArrayCount = 0;

   for (i=0;i<probePlaceNetArrayCount;i++)
   {
      delete probePlaceNetArray[i];
   }
   probePlaceNetArray.RemoveAll();
   probePlaceNetArrayCount = 0;

   for (i=0;i<TA_netnameArrayCount;i++)
   {
      delete TA_netnameArray[i];
   }
   TA_netnameArray.RemoveAll();
   TA_netnameArrayCount = 0;
}

/******************************************************************************
* TA_process_probeplacement
*/
static void TA_process_probeplacement(CCEtoODBDoc *doc, FileStruct *pcbFile, TestProbeList *probelist)
{
   // first update the prefered layer
   if (!TAoptions.ProbePlace.allownetaccesstop && !TAoptions.ProbePlace.allownetaccessbottom ) 
   {
      ErrorMessage("You must \"Allow\" Top Surface NetAccess or Bottom Surface NetAccess!\nNo information processed.", 
         "Error Net Access Selection");
      return;
   }

   if (!TAoptions.ProbePlace.allownetaccesstop && TAoptions.ProbePlace.prefernetaccesssurface == 0 ) 
   {
      ErrorMessage("You selected Top Surface NetAccess \"Prefered\" but not \"Allowed\"!\nNo information processed.", 
         "Error Net Access Selection");
      return;
   }

   if (!TAoptions.ProbePlace.allownetaccessbottom && TAoptions.ProbePlace.prefernetaccesssurface == 1 ) 
   {
      ErrorMessage("You selected Bottom Surface NetAccess \"Prefered\" but not \"Allowed\"!\nNo information processed.", 
         "Error Net Access Selection");
      return;
   }

   doc->PrepareAddEntity(pcbFile);  
   do_probe_place(doc, pcbFile, probelist);
}

/******************************************************************************
* do_probe_place
*/
static void do_probe_place(CCEtoODBDoc *doc, FileStruct *file, TestProbeList *probelist)
{
   CString LogFile = GetLogfilePath("probeplace.log");
   FILE  *flog;
   CWaitCursor w;

   CProgressDlg *gauge = new CProgressDlg("Probe Placement Calculation", FALSE);
   gauge->Create();

   if ((flog = fopen(LogFile, "wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", LogFile);
      ErrorMessage(t, "Error");
   }

   init_probeplace_allowedlayer();
   probeplace_net_accesspoints();


   // here update layercost
   update_probelayercost(TAoptions.ProbePlace.prefernetaccesssurface);

   gauge->SetStatus("Initial Probe Placement");

   // first run !
   if (TAoptions.ProbePlace.prefernetaccesssurface == 0) // top
   {
      do_probeplace(flog, 1, probelist);
      do_probeplace(flog, 2, probelist);
   }
   else
   {
      do_probeplace(flog, 2, probelist);
      do_probeplace(flog, 1, probelist);
   }

   update_probecnt(probelist);
   int lastfails, fails = get_probeplacefails(flog, 0);
   int iterationcnt = 0, trycnt = 0;
   lastfails = fails + 1;


   // iterate as long as the result gets better and do 1 extra try.
   while (fails && (fails < lastfails) && trycnt < 2)
   {
      CString  g;
      g.Format("Placement Iteration [%d] Current fails [%d]", iterationcnt+1, fails);
      gauge->SetStatus(g);

      clear_probeplace();
      do_probeplace(flog, 3, probelist);
      update_probecnt(probelist);

      lastfails = fails;
      fails = get_probeplacefails(flog, iterationcnt+1);
      fprintf(flog, "Initial Probe placement [%d] Previous fails [%d] Current fails [%d]\n",
         ++iterationcnt, lastfails, fails);

      if (fails < lastfails)
         trycnt = 0;
      else
         trycnt++;

   }

   if (fails == 0)
   {
      fprintf(flog, "Success - all required probes placed !\n");
      gauge->SetStatus("Success - all required probes placed !");
   }
   else
   {
      fprintf(flog, "Failed %d - not all required probes placed !\n", fails);
   }

   if (TAoptions.ProbePlace.optimize)
   {
      double   tp_size;
      int      tp_index;
      CString  tp_probename, tp_probeprefix;

      gauge->SetStatus("Optimizing Probe Placement");

      TestProbeStruct *tp = get_smallestprobesize(probelist);
      if (tp != NULL)   
      {
         tp_size = tp->size;
         tp_probename = tp->probename;
         tp_probeprefix = tp->probeprefix;
         tp_index=  tp->index;
         while (tp = get_nextprobesize(probelist, tp_size))
         {
            tp_size = tp->size;
            tp_probename = tp->probename;
            tp_probeprefix = tp->probeprefix;
            // loop this process for number of times.
            int changes;
            while (changes = do_optimizeprobeplace(flog, 3, probelist, tp->index, tp->size, 
               tp->headsize, tp_probename, tp_probeprefix))
            {
               fprintf(flog, "Optimizing process [%s] -> %d probes changed!\n", tp_probename, changes);
            }
         }
      }
      update_probecnt(probelist);
   }

   // calculate number of nets to be probed and actually probed.
   probeplace_calc_placed(doc, file);

   delete gauge;

   fclose(flog);

   Notepad(LogFile);
}

/********************************************************************
* FillTestProbeList()
*
*  DESCRIPTION: Fills CTestProbeList 
*
*  PARAMETERS: 
*     list - Empty Linked List of TestProbeStructs
*     file - selected file
*/
static void FillTestProbeList(CCEtoODBDoc *doc, TestProbeList *list)
{
   TestProbeStruct *tp;
   FILE *fp;
   int   cnt = 0;

   char     line[255];
   char     *lp;
   double   scale = -1;
   int      version = -1;

   CString probeFile(getApp().getSystemSettingsFilePath("default.prb"));   
   if ((fp = fopen(probeFile,"rt")) != NULL)
   {
      while (fgets(line,255,fp))
      {
         if ((lp = get_string(line," \t\n")) == NULL) continue;
         if (lp[0] != '.')                         continue;

         if (!STRCMPI(lp,".VERSION"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            version = atoi(lp);
         }
         else
         if (!STRCMPI(lp,".PROBEUNITS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (!STRNICMP(lp,"IN",2))
            {
               scale = Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MI",2))
            {
               scale = Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MM",2))
            {
               scale = Units_Factor(UNIT_MM, doc->getSettings().getPageUnits());
            }
         }
         else
         if (!STRCMPI(lp,".PROBEDEF"))
         {
            if (version < 2)
            {
               ErrorMessage("Update your Probe defintions file", ".PROBEUNITS missing");
               break;
            }

            CString  probename, desc, prefix, drilltool;
            double   size;
            int      onoff, cost;

            if ((lp = get_string(NULL," \t")) == NULL)   continue; 
            onoff = atoi(lp);

            if ((lp = get_string(NULL," \t")) == NULL)   continue; 
            probename = lp;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            size = atof(lp);

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cost = atoi(lp);

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            desc = lp;

            prefix = "$$PROBE_";
            drilltool = "";
            double drillsize = 0, headsize = size;
            if (lp = get_string(NULL," \t\n"))
            {
               drilltool = lp;
               if (lp = get_string(NULL," \t\n"))
               {
                  prefix = lp;

                  if (lp = get_string(NULL," \t\n"))
                  {
                     drillsize = atof(lp);
                     if (lp = get_string(NULL," \t\n"))
                     {
                        headsize = atof(lp);
                     }
                  }
               }
            }  // drill tool 

            // memory is freed in dialog
            tp= new TestProbeStruct;
            tp->onoff =  onoff;
            tp->cost = cost;
            tp->probename = probename;
            tp->size = size*scale;
            tp->description = desc;
            tp->probeprefix = prefix;
            tp->probedrilltool = drilltool;
            tp->drillsize = drillsize * scale;
            tp->headsize = headsize * scale;
            tp->placedbot = 0;
            tp->placedtop = 0;
            tp->index = cnt++;
            list->AddTail(tp);

         }
         else
         if (!STRCMPI(lp,".PROBE"))
         {
            CString  probename, desc, prefix, drilltool;
            double   size, scale;
            int      onoff, cost;

            if ((lp = get_string(NULL," \t")) == NULL)   continue; 
            onoff = atoi(lp);

            if ((lp = get_string(NULL," \t")) == NULL)   continue; 
            probename = lp;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            size = atof(lp);

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cost = atoi(lp);

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (!STRNICMP(lp,"IN",2))
            {
               scale = Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MI",2))
            {
               scale = Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MM",2))
            {
               scale = Units_Factor(UNIT_MM, doc->getSettings().getPageUnits());
            }
            else
            {
               // unknown unit
               continue;
            }

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            desc = lp;

            prefix = "$$PROBE_";
            drilltool = "";
            double drillsize = 0, headsize = size;
            if (lp = get_string(NULL," \t\n"))
            {
               drilltool = lp;
               if (lp = get_string(NULL," \t\n"))
               {
                  prefix = lp;

                  if (lp = get_string(NULL," \t\n"))
                  {
                     drillsize = atof(lp);
                     if (lp = get_string(NULL," \t\n"))
                     {
                        headsize = atof(lp);
                     }
                  }
               }
            }  // drill tool 

            // memory is freed in dialog
            tp= new TestProbeStruct;
            tp->onoff =  onoff;
            tp->cost = cost;
            tp->probename = probename;
            tp->size = size*scale;
            tp->description = desc;
            tp->probeprefix = prefix;
            tp->probedrilltool = drilltool;
            tp->drillsize = drillsize * scale;
            tp->headsize = headsize * scale;
            tp->placedbot = 0;
            tp->placedtop = 0;
            tp->index = cnt++;
            list->AddTail(tp);
         }
      }
      fclose(fp);
   }
   else
   {
      CString tmp;
      tmp.Format("[%s] not found!", probeFile);
      ErrorMessage("Missing Probe Definition File. No probes loaded.", tmp);
   }

   // memory is freed in dialog
   if (version < 2)
   {
      CString tmp;
      tmp.Format(".VERSION in [%s] not .VERSION 2 !", probeFile);
      ErrorMessage("Missing Probe Definition File. No probes loaded.", tmp);
   }
}

/******************************************************************************
* probeplace_calc_placed
   1. calculate number of nets to be probed and actually probed
   2. number of probes required and actually placed (a net can have multiple probes required.
   
*/
static int probeplace_calc_placed(CCEtoODBDoc *doc, FileStruct *file)
{
   TApcbfileprobeplace.probed_nets_total = 0;
   TApcbfileprobeplace.probed_nets_multi = 0;
   TApcbfileprobeplace.probed_nets_single = 0;

   TApcbfileprobeplace.needed_probes = 0;
   TApcbfileprobeplace.placed_probes = 0;

	int i=0;
   for (i=0; i<probePlaceNetArrayCount; i++)
   {
#ifdef _DEBUG
      TA_ProbePlaceNet *nn = probePlaceNetArray[i];
#endif
      int   single_qty = 0, multi_qty = 0;
      for (int ii=0;ii<probePlaceArrayCount;ii++)
      {
#ifdef _DEBUG
   TA_ProbePlace *pp = probePlaceArray[ii];
#endif
         if (probePlaceArray[ii]->netindex != i)   continue;

         if (probePlaceNetArray[i]->nonconn)
         {
            if (probePlaceArray[ii]->probe_placed)    
               single_qty++;
         }
         else
         {
            if (probePlaceArray[ii]->probe_placed)    
               multi_qty++;
         }
      }
      if (single_qty || multi_qty)
      {
         TApcbfileprobeplace.probed_nets_total += 1;
      }
      if (multi_qty)
         TApcbfileprobeplace.probed_nets_multi += 1;
      if (single_qty)
         TApcbfileprobeplace.probed_nets_single += 1;
   }


   for (i=0;i<probePlaceNetArrayCount;i++)
   {
      TApcbfileprobeplace.needed_probes += probePlaceNetArray[i]->number_required_probes;
      TApcbfileprobeplace.placed_probes += probePlaceNetArray[i]->number_placed_probes;
   }

   doc->UnselectAll(TRUE);
   // first need to delete the old 
   POSITION drcPos = file->getDRCList().GetHeadPosition();
   while (drcPos != NULL)
   {
      DRCStruct *drc = file->getDRCList().GetNext(drcPos);

      if (drc->getAlgorithmType() == DFT_ALG_NET_WITHOUT_PROBE)
         RemoveOneDRC(doc, drc, file);
   }

   for (i=0;i<probePlaceNetArrayCount;i++)
   {

#ifdef _DEBUG
TA_ProbePlaceNet *ww = probePlaceNetArray[i];
#endif

      // Probes on non connected pins have no netname, but must have a position
      if (probePlaceNetArray[i]->nonconn) 
      {
         int ptr = get_probeplace_from_netindex(i);
         if (ptr > -1)
         {
#ifdef _DEBUG
TA_ProbePlace *ww = probePlaceArray[ptr];
#endif
            // from probePlaceArray I need to get the access entity
            // from access I need to get the pinkoarray entity
            if ((ptr = get_pinkoocnt_entity(probePlaceArray[ptr]->via_pin_entitynum)) > -1)
            {
               drc_singlepin_without_probe(doc, file, pinkooarray[ptr]->x, pinkooarray[ptr]->y, 
                             pinkooarray[ptr]->compname, pinkooarray[ptr]->pinname);
            }
         }

         continue;
      }

      if (probePlaceNetArray[i]->number_placed_probes < probePlaceNetArray[i]->number_required_probes)
      {
         drc_net_without_probe(doc, file, probePlaceNetArray[i]->netname);
      }
   }

   return 1;
}

/******************************************************************************
* do_optimizeprobeplace
*/
static int do_optimizeprobeplace(FILE *flog, int layer, TestProbeList *probelist, 
                                 int tp_index, double tp_size, double tp_headsize,
                                 const char *tp_probename, const char *tp_probeprefix)
{
   int   changes = 0;

   for (int i=0;i<probePlaceArrayCount;i++)
   {
      int   placecost = INT_MAX; // use the lowest probePlaceArray[i]->cost for the result.
      int   placeindex = -1;

      TA_ProbePlace *p = probePlaceArray[i];
      // if there was not probe placed, do not optimize it.
      if (p->probe_placed == 0)        continue;
      // only try to deal with probes, which are smaller in size
      if (p->probe_size >= tp_size) continue;

      // probe_placed temporarily cleared.
      p->probe_placed = 0;

      // elimitate this probe out of the probe array
      // try to find a bigger, optimized location

      // here loop thru all probes to find the lowest cost point on the same net
      for (int ii=0;ii<probePlaceArrayCount;ii++)
      {
         if (p->netindex != probePlaceArray[ii]->netindex)  continue;

         // layer is 1 or 2 or 3 (all layers allowed)
         if (!(probePlaceArray[ii]->allowedlayer & layer))
            continue;
 
         // distance of access to next obstical vs. probesize radius
         if (probePlaceArray[ii]->distance < tp_headsize/2)
            continue;

         if (probeplace_distance(tp_size, layer, probePlaceArray[ii]->x, probePlaceArray[ii]->y) == 1)
         {
            if (placecost > (probePlaceArray[ii]->cost+probePlaceArray[ii]->layercost))
            {
               placeindex = ii;
               placecost = probePlaceArray[ii]->cost+probePlaceArray[ii]->layercost;
            }
         }
      } // ii
      if (placeindex > -1)
      {
         TA_ProbePlace *pp = probePlaceArray[placeindex];
         p->probe_index = -1;
         p->probe_size = 0;
         p->probe_name = "";
         pp->probe_placed = TRUE;
         pp->probe_index = tp_index;
         pp->probe_size = tp_size;
         pp->probe_headsize = tp_headsize;
         pp->probe_name = tp_probename;
         changes++;
      }
      else
      {
         // restore the old location.
         p->probe_placed = TRUE;
      }
   }

   return changes;
}

/******************************************************************************
* probeplace_distance
   check here if a probe with siz and x,y on layer can be placed. If not, return the -index 
   of the probe that hindered it.
*/
static int probeplace_distance(double siz, int layer, double x, double y)
{
   for (int i=0; i<probePlaceArrayCount; i++)
   {
#ifdef _DEBUG
      TA_ProbePlace *p = probePlaceArray[i];
      CString n = probePlaceNetArray[probePlaceArray[i]->netindex]->netname;
#endif

      if ((probePlaceArray[i]->allowedlayer & layer) && probePlaceArray[i]->probe_placed)
      {
         double dist =  sqrt((probePlaceArray[i]->x - x)*(probePlaceArray[i]->x - x) + 
                          (probePlaceArray[i]->y - y)*(probePlaceArray[i]->y - y));

         // distance between a probesize to be placed and a probesize which is already there
         if (dist < (siz + probePlaceArray[i]->probe_size)/2) 
            return -i;
      }
   }

   return 1;
}

/******************************************************************************
* get_smallestprobesize
*/
static TestProbeStruct *get_smallestprobesize(TestProbeList *probelist)
{
   double   siz = -1;
   TestProbeStruct *tmp = NULL;

   POSITION pos = probelist->GetHeadPosition();

   while (pos != NULL)
   {
      TestProbeStruct *tp = probelist->GetNext(pos);
      if (!tp->onoff)      continue; // no switched on
      if (siz < 0)   
      {
         siz = tp->size;
         tmp = tp;
      }
      else
      {
         if (tp->size < siz)
         {
            siz = tp->size;
            tmp = tp;
         }
      }
   }

   return tmp; 
}

/******************************************************************************
* get_nextprobesize
*/
static TestProbeStruct *get_nextprobesize(TestProbeList *probelist, double prevsiz)
{
   TestProbeStruct *tmp = NULL;
   double   siz = INT_MAX;

   POSITION pos = probelist->GetHeadPosition();

   while (pos != NULL)
   {
      TestProbeStruct *tp = probelist->GetNext(pos);
      if (!tp->onoff)      continue; // no switched on
      if (tp->size > prevsiz && tp->size < siz) 
      {
         siz = tp->size;
         tmp = tp;
      }
   }

   return tmp; 
}

/******************************************************************************
* get_probeplacefails
*/
static int get_probeplacefails(FILE *flog, int iterationcnt)
{
   int      fails = 0;
   int      n;
   CString  iteration;

   iteration.Format("Iteration %2d", iterationcnt);

   for (n=0;n<probePlaceNetArrayCount;n++)
   {
      if (probePlaceNetArray[n]->number_placed_probes < probePlaceNetArray[n]->number_required_probes)
      {
         fprintf(flog, "%s: Probe fail Net [%s] Placed [%d] Required [%d]\n", 
            iteration, probePlaceNetArray[n]->netname,
            probePlaceNetArray[n]->number_placed_probes,
            probePlaceNetArray[n]->number_required_probes);

         fails++;
      }
   }
   fprintf(flog, "Total fail %s: %d\n\n", iteration, fails);
   return fails;
}

/******************************************************************************
* update_probecnt
   update the fill list 
*/
static void update_probecnt(TestProbeList *probelist)
{
   // set all probe cnts top 0;
   POSITION pos = probelist->GetHeadPosition();
   while (pos)
   {
      TestProbeStruct *tp = probelist->GetNext(pos);
      tp->placedbot = 0;
      tp->placedtop = 0;
   }

   for (int i=0; i<probePlaceArrayCount; i++)
   {
      if (!probePlaceArray[i]->probe_placed)
         continue;
      if (probePlaceArray[i]->slayer & 1)
      {
         POSITION pos = probelist->FindIndex(probePlaceArray[i]->probe_index);
         TestProbeStruct *tp = probelist->GetNext(pos);
         if (tp)  tp->placedtop++;
      }
      if (probePlaceArray[i]->slayer & 2)
      {
         POSITION pos = probelist->FindIndex(probePlaceArray[i]->probe_index);
         TestProbeStruct *tp = probelist->GetNext(pos);
         if (tp)  tp->placedbot++;
      }
   }
}

/******************************************************************************
* get_probeplace_from_netindex
*/
static int get_probeplace_from_netindex(int netindex)
{
   for (int i=0; i<probePlaceArrayCount; i++)
   {
#ifdef _DEBUG
TA_ProbePlace *ww = probePlaceArray[i];
#endif
      if (probePlaceArray[i]->netindex == netindex)
         return i;
   }

   return -1;
}

/******************************************************************************
* do_probeplace
*/
static void do_probeplace(FILE *flog, int layer, TestProbeList *probelist)
{
   int   highestprobecnt = get_highesttotalaccesscnt(layer);
   int   i, n;

   for (i=1;i<=highestprobecnt;i++)
   {
      // select placement of the lowest probe per net > 0
      for (n=0;n<probePlaceNetArrayCount;n++)
      {
#ifdef _DEBUG
TA_ProbePlaceNet *nn = probePlaceNetArray[n];
#endif
         if (probePlaceNetArray[n]->number_of_access_top+probePlaceNetArray[n]->number_of_access_bot != i)
            continue;
         
         // loop for number of needed probes.
         for (int ii=probePlaceNetArray[n]->number_placed_probes; ii < probePlaceNetArray[n]->number_required_probes; ii++)
         {
            probePlaceNetArray[n]->number_placed_probes += do_probeplace_net(flog, n, layer, probelist);
         }
      }
   }
}

/******************************************************************************
* get_highesttotalaccesscnt
*/
static int get_highesttotalaccesscnt(int layer)
{
   int cnt = 0;

   for (int i=0; i<probePlaceNetArrayCount; i++)
   {
      cnt = max(cnt, probePlaceNetArray[i]->number_of_access_top+probePlaceNetArray[i]->number_of_access_bot);
   }

   return cnt;
}

/******************************************************************************
* do_probeplace_net
   This is the initial placement. Probes start with the lowest cost. If a probe
   can not be placed here -> big problem

   to solve the problem of not placing a probe I try the following algorithm.
   Check which probe hinder a placement of a probe, up their placement cost and
   try again.

*/
static int do_probeplace_net(FILE *flog, int netindex, int layer, TestProbeList *probelist)
{
   int   placecost = INT_MAX; // use the lowest probePlaceArray[i]->cost for the result.
   int   placeindex = -1;

   // check which probes are allowed
   // select the smallest one to start
   // a net can determine the smallest allowed probesize
   TestProbeStruct *tp = get_smallestprobesize(probelist);
   int             failindex = -1;

#ifdef _DEBUG
      CString nn = probePlaceNetArray[netindex]->netname;
#endif

   if (tp == NULL)
   {
      ErrorMessage("No Probe found","do_probeplace_net");
      return 0;
   }

   // loop through here for lowest cost !
   for (int i=0;i<probePlaceArrayCount;i++)
   {
      if (probePlaceArray[i]->netindex != netindex)   
         continue;

#ifdef _DEBUG
      TA_ProbePlace *p = probePlaceArray[i];
      CString n = probePlaceNetArray[probePlaceArray[i]->netindex]->netname;
#endif

      if (!(probePlaceArray[i]->allowedlayer & layer))
         continue;
 
      // distance of access to next obstical vs. probesize radius to be placed
      if (probePlaceArray[i]->distance < tp->headsize/2)
         continue;

      if ((failindex = probeplace_distance(tp->size, probePlaceArray[i]->allowedlayer, probePlaceArray[i]->x, probePlaceArray[i]->y)) == 1)
      {
         if (placecost > (probePlaceArray[i]->cost+probePlaceArray[i]->layercost))
         {
            placeindex = i;
            placecost = probePlaceArray[i]->cost+probePlaceArray[i]->layercost;           
         }
      }
      else
      {
         failindex = abs(failindex);   // this probe caused a fail
      }
   }

   if (placeindex > -1)
   {
      probePlaceArray[placeindex]->probe_index = tp->index;
      probePlaceArray[placeindex]->probe_size = tp->size;
      probePlaceArray[placeindex]->probe_name = tp->probename;
      probePlaceArray[placeindex]->probe_prefix = tp->probeprefix;
      probePlaceArray[placeindex]->probe_headsize = tp->headsize;

      probePlaceArray[placeindex]->probe_placed = TRUE;
      return 1;
   }
   else
   if (failindex > -1)
   {
      // up the cost of the probe that caused the fail
      probePlaceArray[failindex]->cost += 100; // must be higher than preferred cost.
   }
   
   return 0;
}

/******************************************************************************
* probeplace_load
   This loads all TEST_ACCESS as the init for placing probes.
*/
static int probeplace_load(FILE *taLog, FileStruct *file, CCEtoODBDoc *doc, int eval_nonconn)
{
   DataStruct *np;
   Mat2x2   m;
   int      mirror;
   double   scale;
   double   rotation;
   double   insert_x, insert_y;
   int      nonconncnt = 0;
   int      already_placed_probes = 0;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();

   mirror = file->isMirrored();
   rotation = file->getRotation();
   scale = file->getScale();
   insert_x = file->getInsertX();
   insert_y = file->getInsertY();

   RotMat2(&m, rotation);

   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (np->getInsert()->getInsertType() == INSERTTYPE_TEST_PROBE)  
            {
               already_placed_probes++;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_TEST_ACCESSPOINT)  
            {
               // if no access, do not load.
               Attrib *a;
               double dist = DBL_MAX;

               // every single pin net has a ~comp pin~ netname. The netlist has the NETFLAG_UNUSEDNET flag.

               if (a = is_attvalue(doc, np->getAttributesRef(), ATT_TESTACCESS, 0))
               {
                  CString  l;
                  l = get_attvalue_string(doc, a);
                  if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_NONE]))
                     break;
               }

               if (a = is_attvalue(doc, np->getAttributesRef(), ATT_ACCESS_DISTANCE, 0))
               {
                  CString  l;
                  l = get_attvalue_string(doc, a);
                  dist = atof(l) * scale;
               }

               // EXPLODEALL - call DoWriteData() recursively to write embedded entities
               Point2   point2;
               point2.x = np->getInsert()->getOriginX() * scale;
               if (mirror)
                  point2.x = -point2.x * scale;
               
               point2.y = np->getInsert()->getOriginY();
               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               int netindex; 
               if (a = is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 0))
               {
                  CString  l;
                  l = get_attvalue_string(doc, a);
                  // here check for nonconn
                  int nptr = get_report_netnameptr(l);
                  if (TA_netnameArray[nptr]->nonconn && !eval_nonconn)
                     break;

                  netindex = get_probeplacenetindex(l, TA_netnameArray[nptr]->nonconn);
                  if (netindex < 0)
                  {
                     fprintf(taLog, "Testprobe has a netname [%s] which does not appear in the netlist!\n", l);
                     break;
                  }

               }

               TA_ProbePlace *c = new TA_ProbePlace;
               probePlaceArray.SetAtGrow(probePlaceArrayCount,c);  
               probePlaceArrayCount++; 

               //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               //double block_rot = rotation + np->getInsert()->getAngle();
               c->access_entitynum = np->getEntityNumber();
               c->via_pin_entitynum = -1;
               // default to placement layer
               if (np->getInsert()->getMirrorFlags() & MIRROR_LAYERS)
                  c->slayer = 2;
               else
                  c->slayer = 1;

               c->x = point2.x;
               c->y = point2.y;
               c->preference = 0;
               c->probe_placed = 0;
               c->probe_index = -1;
               c->probe_size = 0;
               c->probe_headsize = 0;
               c->net_numberofaccess_top = 0;
               c->net_numberofaccess_bot = 0;
               c->allowedlayer = 0;
               c->preference = 0;
               c->distance = dist;
               c->netindex = netindex;

               // must have a netname attribute
               if (a = is_attvalue(doc, np->getAttributesRef(), ATT_DDLINK, 0))
               {
                  CString  l;
                  l = get_attvalue_string(doc, a);
                  c->via_pin_entitynum = atol(l);                    
               }

               if (a = is_attvalue(doc, np->getAttributesRef(), ATT_TESTPREFERENCE, 0))
               {
                  CString p;
                  p = get_attvalue_string(doc, a);
                  c->preference = atoi(p);
               }
               c->cost = c->preference;
            } 
         } // case INSERT
         break;
      } // end switch
   } // while
   return already_placed_probes;
}

/******************************************************************************
* get_probeplacenetindex
*/
static int get_probeplacenetindex(const char *n, int nonconn)
{
   for (int i=0; i<probePlaceNetArrayCount; i++)
   {
#ifdef _DEBUG
   TA_ProbePlaceNet *pp = probePlaceNetArray[i];
#endif
      if (probePlaceNetArray[i]->netname.Compare(n) == 0)
         return i;
   }

   TA_ProbePlaceNet *c = new TA_ProbePlaceNet;
   probePlaceNetArray.SetAtGrow(probePlaceNetArrayCount,c);  
   probePlaceNetArrayCount++; 
   c->netname = n;
   c->nonconn = nonconn;
   c->number_required_probes = 1;
   c->number_placed_probes = 0;
   c->number_of_access_top = 0;
   c->number_of_access_bot = 0;

   return probePlaceNetArrayCount-1;
}

/******************************************************************************
* probeplace_net_accesspoints
*  - count how many access point a net has. The idea is a fewer access points a
*     net has, as more difficult it is to place a probe
*/
static void probeplace_net_accesspoints()
{
	int i=0;
   for (i=0; i<probePlaceArrayCount; i++)
   {
      TA_ProbePlace *probePlace1 = probePlaceArray[i];

      if (probePlace1->net_numberofaccess_top + probePlace1->net_numberofaccess_bot != 0)
         continue;

      // incase there is a single pin net
      if (probePlace1->allowedlayer == 1)
         probePlace1->net_numberofaccess_top++;
      else if (probePlace1->allowedlayer == 2)
         probePlace1->net_numberofaccess_bot++;

      // update counter for first net found including it self
		int ii=0;
      for (ii=i+1; ii<probePlaceArrayCount; ii++)
      {
         TA_ProbePlace *probePlace2 = probePlaceArray[ii];

         if (!probePlace1->allowedlayer)
            continue;

         if (probePlace1->netindex == probePlace2->netindex)
         {
            if (probePlace2->allowedlayer == 1)
               probePlace1->net_numberofaccess_top++;
            else if (probePlace2->allowedlayer == 2)
               probePlace1->net_numberofaccess_bot++;
         }
      }

      // now update every entry
      // update counter for first net found including it self
      for (ii=i+1;ii<probePlaceArrayCount;ii++)
      {
         TA_ProbePlace *probePlace2 = probePlaceArray[ii];

         if (!probePlace1->allowedlayer)
            continue;

         if (probePlace1->netindex == probePlace2->netindex)
         {
            probePlace2->net_numberofaccess_top = probePlace1->net_numberofaccess_top;
            probePlace2->net_numberofaccess_bot = probePlace1->net_numberofaccess_bot;
         }
      }

      probePlaceNetArray[probePlace1->netindex]->number_of_access_top = probePlace1->net_numberofaccess_top;
      probePlaceNetArray[probePlace1->netindex]->number_of_access_bot = probePlace1->net_numberofaccess_bot;
   }

   TAoptions.ProbePlace.totalaccessnets = 0;
   TAoptions.ProbePlace.singleaccessnets = 0;
   TAoptions.ProbePlace.multiaccessnets = 0;

   // here update counter for dialog
   for (i=0; i<probePlaceNetArrayCount; i++)
   {
      TA_ProbePlaceNet *probePlace1 = probePlaceNetArray[i];

      int atop = 0;
      int abot = 0;
      for (int ii=0; ii<probePlaceArrayCount; ii++)
      {
         TA_ProbePlace *probePlace2 = probePlaceArray[ii];

         if (i == probePlace2->netindex)
         {
            if (probePlace2->slayer & 1)
               atop++;
            if (probePlace2->slayer & 2)
               abot++;
         }
      }

      if (probePlace1->nonconn)
      {
         if (abot || atop)
            TAoptions.ProbePlace.singleaccessnets++;
         else
            ; // no access
      }
      else
      {
         if (abot || atop)
            TAoptions.ProbePlace.multiaccessnets++;
         else
            ; // no access
      }
   }
   
   if (TAoptions.Unusedpins)
      TAoptions.ProbePlace.totalaccessnets = TAoptions.ProbePlace.singleaccessnets + TAoptions.ProbePlace.multiaccessnets;
   else
   {
      TAoptions.ProbePlace.totalaccessnets = TAoptions.ProbePlace.multiaccessnets;
      TAoptions.ProbePlace.singleaccessnets = 0;
   }
}

/******************************************************************************
* probeplace_number_of_probes
*/
static int probeplace_number_of_probes(FileStruct *file, CCEtoODBDoc *doc)
{
   NetStruct   *net;
   POSITION    netPos;
   Attrib      *a;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      if (a = is_attvalue(doc, net->getAttributesRef(), ATT_TEST_NET_PROBES, 0))
      {
         CString  l;
         l = get_attvalue_string(doc, a);
         int n = get_probeplacenetindex(net->getNetName(), FALSE);
         probePlaceNetArray[n]->number_required_probes = atoi(l);
      }
   } // while

   return 1;
}

/******************************************************************************
* update_probelayercost
      the non prefered layer gets the cost
*/
static void update_probelayercost(int l)  // 0 == top prefered, 1 bottom pref.
{
   // reset all nets
	int n=0;
   for (n=0; n<probePlaceArrayCount; n++)
   {
      probePlaceArray[n]->layercost = 0;
   }

   // reset all probes
   for (n=0; n<probePlaceArrayCount; n++)
   {
      if (l == 0)
      {
         // top prefered
         if (probePlaceArray[n]->allowedlayer == 2)
            probePlaceArray[n]->layercost += 50;
      }
      else
      {
         // bottom prefered,
         if (probePlaceArray[n]->allowedlayer == 1)
            probePlaceArray[n]->layercost += 50;
      }
   }
}

/******************************************************************************
* clear_probeplace
*/
static void clear_probeplace()
{
   // reset all nets
	int n=0;
   for (n=0; n<probePlaceNetArrayCount; n++)
   {
      probePlaceNetArray[n]->number_placed_probes = 0;
   }

   // reset all probes
   for (n=0; n<probePlaceArrayCount; n++)
   {
      probePlaceArray[n]->probe_placed = FALSE;
      probePlaceArray[n]->probe_size = 0;
   }
}

/******************************************************************************
* TA_delete_probe_definitions
*/
void TA_delete_probe_definitions(CCEtoODBDoc *doc, TestProbeList *probelist)
{
   TestProbeStruct *tmp = NULL;

   POSITION pos = probelist->GetHeadPosition();

   while (pos != NULL)
   {
      tmp = probelist->GetNext(pos);
      BlockStruct *b = Graph_Block_Exists(doc, tmp->probename, -1);
      if (b)   doc->RemoveBlock(b);
   }
   return;
}

/******************************************************************************
* init_probeplace_allowedlayer
*/
static void init_probeplace_allowedlayer()
{
   // reset all probes
   for (int n=0; n<probePlaceArrayCount; n++)
   {
      TA_ProbePlace *probePlace = probePlaceArray[n];

      probePlace->allowedlayer = probePlace->slayer;
      if (!TAoptions.ProbePlace.allownetaccessbottom)
         probePlace->allowedlayer = probePlace->allowedlayer & 1;
      if (!TAoptions.ProbePlace.allownetaccesstop)
         probePlace->allowedlayer = probePlace->allowedlayer & 2;
   }
}

/******************************************************************************
* get_testprobestruct
*/
static TestProbeStruct *get_testprobestruct(TestProbeList *probelist, int index)
{
   TestProbeStruct *tmp = NULL;

   POSITION pos = probelist->GetHeadPosition();

   while (pos != NULL)
   {
      tmp = probelist->GetNext(pos);
      if (tmp->index == index)
         return tmp;
   }

   return tmp; 
}


static int TA_Probeplace_Update(CCEtoODBDoc *doc, FileStruct *pcbfile, TestProbeList *probelist);
static void TA_ProbeplaceDebugReport(const char *filename, CCEtoODBDoc *doc);
/////////////////////////////////////////////////////////////////////////////
// TestProbePlacement dialog
TestProbePlacement::TestProbePlacement(CWnd* pParent /*=NULL*/)
   : CDialog(TestProbePlacement::IDD, pParent)
{
   //{{AFX_DATA_INIT(TestProbePlacement)
   m_totalnets = _T("0");
   m_totalaccess = _T("0");
   m_singleportaccess = _T("0");
   m_singleportnets = _T("0");
   m_multiportaccess = _T("0");
   m_multiportnets = _T("0");
   m_bottomallowaccess = TRUE;
   m_topallowaccess = TRUE;
   m_bottomtotalaccess = _T("0");
   m_toptotalaccess = _T("0");
   m_prefer = 1;
   m_totalaccessPercent = _T("");
   m_singleportaccessPercent = _T("");
   m_multiportaccessPercent = _T("");
   m_alwaysOptimize = TRUE;
   m_placedprobes = _T("?");
   m_totalprobes = _T("?");
   m_totalnets_access = _T("");
   m_singleportnets_access = _T("");
   m_multiportnets_access = _T("");
   //}}AFX_DATA_INIT
}


void TestProbePlacement::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TestProbePlacement)
   DDX_Control(pDX, IDC_MAIN_LIST, m_list);
   DDX_Text(pDX, IDC_TOTAL_NETS, m_totalnets);
   DDX_Text(pDX, IDC_TOTAL_ACCESS, m_totalaccess);
   DDX_Text(pDX, IDC_SINGLE_PORT_ACCESS, m_singleportaccess);
   DDX_Text(pDX, IDC_SINGLE_PORT_NETS, m_singleportnets);
   DDX_Text(pDX, IDC_MULTI_PORT_ACCESS, m_multiportaccess);
   DDX_Text(pDX, IDC_MULTI_PORT_NETS, m_multiportnets);
   DDX_Check(pDX, IDC_BOTTOM_ALLOW_ACCESS, m_bottomallowaccess);
   DDX_Check(pDX, IDC_TOP_ALLOW_ACCESS, m_topallowaccess);
   DDX_Text(pDX, IDC_BOTTOM_TOTAL_ACCESS, m_bottomtotalaccess);
   DDX_Text(pDX, IDC_TOP_TOTAL_ACCESS, m_toptotalaccess);
   DDX_Radio(pDX, IDC_PREFER_TOP_BOTTOM, m_prefer);
   DDX_Text(pDX, IDC_TOTAL_ACCESS_PERCENT, m_totalaccessPercent);
   DDX_Text(pDX, IDC_SINGLE_PORT_ACCESS_PERCENT, m_singleportaccessPercent);
   DDX_Text(pDX, IDC_MULTI_PORT_ACCESS_PERCENT, m_multiportaccessPercent);
   DDX_Check(pDX, IDC_OPTIMIZE, m_alwaysOptimize);
   DDX_Text(pDX, IDC_TESTPROBE_PLACEDPROBE, m_placedprobes);
   DDX_Text(pDX, IDC_TESTPROBE_TOTALPROBE, m_totalprobes);
   DDX_Text(pDX, IDC_TOTAL_NETS_ACCESS, m_totalnets_access);
   DDX_Text(pDX, IDC_SINGLE_PORT_NETS_ACCESS, m_singleportnets_access);
   DDX_Text(pDX, IDC_MULTI_PORT_NETS_ACCESS, m_multiportnets_access);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestProbePlacement, CDialog)
   //{{AFX_MSG_MAP(TestProbePlacement)
   ON_NOTIFY(NM_DBLCLK, IDC_MAIN_LIST, OnDblclkTestProbe)
   ON_BN_CLICKED(IDC_PROCESS, OnProcess)
   ON_BN_CLICKED(IDC_VIEW, OnView)
   ON_BN_CLICKED(IDC_TA_PROBE_HELP, OnTaProbeHelp)
   ON_BN_CLICKED(IDC_CLEAR_PROBE, OnClearProbe)
   ON_BN_CLICKED(IDC_LOAD, OnLoad)
   ON_BN_CLICKED(IDC_SAVE, OnSave)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestProbePlacement message handlers
void TestProbePlacement::OnDblclkTestProbe(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // get selected geometry
   int count = m_list.GetItemCount();

   if (!count)
      return;

   BOOL Selected = FALSE;
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
      {
         Selected = TRUE;
         break;
      }
   }
   if (!Selected)
      return;

   TestProbeStruct *tp = (TestProbeStruct*)m_list.GetItemData(selItem); 
   
   // toggle on/off
   tp->onoff = !tp->onoff;

   *pResult = 0;

   FillList();
}

BOOL TestProbePlacement::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 11;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width;   column.pszText = "On/Off";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width;
   column.pszText = "Cost";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.cx = width*2;
   column.pszText = "Probe Name";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.cx = width;
   column.pszText = "Size";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width*2;
   column.pszText = "Description";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);

   column.cx = width;
   column.pszText = "Top";
   column.iSubItem = 5;
   m_list.InsertColumn(5, &column);

   column.cx = width;
   column.pszText = "Bottom";
   column.iSubItem = 6;
   m_list.InsertColumn(6, &column);

   column.cx = width;
   column.pszText = "Drill Tool";
   column.iSubItem = 7;
   m_list.InsertColumn(7, &column);

   column.cx = width;
   column.pszText = "Prefix";
   column.iSubItem = 8;
   m_list.InsertColumn(8, &column);
   
   column.cx = width;
   column.pszText = "DrillSize";
   column.iSubItem = 9;
   m_list.InsertColumn(9, &column);
   
   column.cx = width;
   column.pszText = "HeadSize";
   column.iSubItem = 10;
   m_list.InsertColumn(10, &column);
   

   // here fill initial
   RemoveAll();

   FillTestProbeList(doc, &list);

   // delete all previous defined Probes, because the new definition could have redefined them
   TA_delete_probe_definitions(doc, &list);

   // test max probesize against TAoptions.MaxProbeSize
   CString  tmp;
   tmp = "";
   POSITION pos = list.GetHeadPosition();
   while (pos != NULL)
   {
      TestProbeStruct *tp = list.GetNext(pos);
      if (tp->size > TAoptions.MaxProbeSize)
      {
         CString  mssg;
         mssg.Format("Defined Probesize %1.*lf is larger than Access calculated Size %1.*lf\n", 
            GetDecimals(doc->getSettings().getPageUnits()), tp->size,
            GetDecimals(doc->getSettings().getPageUnits()), TAoptions.MaxProbeSize);
         tmp += mssg;
      }
   }

   if (strlen(tmp))
   {
      CString  mssg;
      mssg = "Adjust .MaxProbeSize in default.prb and rerun Access Analysis!\n";
      mssg += tmp;
      ErrorMessage(tmp, "Used Probesize too large!");
   }

   //FillTestProbeList(doc, &list);
   FillList();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void TestProbePlacement::OnProcess() 
{
   UpdateData();  // this cause the dialog variables.

   TAoptions.ProbePlace.optimize = m_alwaysOptimize; // 
   TAoptions.ProbePlace.prefernetaccesssurface = m_prefer; // 0 = top, 1 bottom
   TAoptions.ProbePlace.allownetaccessbottom = m_bottomallowaccess;
   TAoptions.ProbePlace.allownetaccesstop = m_topallowaccess;

   // make sure there is at least one probe turn on
   BOOL probeOn = FALSE;
   POSITION pos = list.GetHeadPosition();
   while (pos != NULL)
   {
      TestProbeStruct *tp = list.GetNext(pos);
      if (!tp->onoff)
         continue; // no switched on

      probeOn = TRUE;
   }

   // if there is there is no probe turn on, then don't do anything
   if (!probeOn)
   {
      ErrorMessage("No probe is turn on.  Therefore, no probe is placed.");
      return;
   }


   // here the viakooarray and pinkooarray is recalculated for the current dialog settings.
   
   // the process calc starts with the preferred layer and than goes to the other layer. The
   // result is the TEST_PREFERENCE Points which are derived from the TEST_ACCESS and the dialog filter.
   // 

   TA_process_probeplacement(doc, pcbFile, &list);

   //m_bottomtotalaccess = "xx";
   //m_toptotalaccess = "cc";

   m_totalaccess.Format("%d", TApcbfileprobeplace.probed_nets_total);
   m_multiportaccess.Format("%d", TApcbfileprobeplace.probed_nets_multi);

   if (TAoptions.Unusedpins)
   {
      m_singleportaccess.Format("%d", TApcbfileprobeplace.probed_nets_single);
      TA_Percent(&m_singleportaccessPercent, 
         TApcbfileprobeplace.probed_nets_single, TAoptions.ProbePlace.singleaccessnets);
   }
   else
   {
      m_singleportaccess = "-";
      m_singleportaccessPercent = "";
   }

   TA_Percent(&m_totalaccessPercent, 
      TApcbfileprobeplace.probed_nets_total, TAoptions.ProbePlace.totalaccessnets);
   TA_Percent(&m_multiportaccessPercent, 
      TApcbfileprobeplace.probed_nets_multi, TAoptions.ProbePlace.multiaccessnets);

   //BOOL   m_alwaysOptimize;
   m_placedprobes.Format("%d", TApcbfileprobeplace.placed_probes);
   m_totalprobes.Format("%d", TApcbfileprobeplace.needed_probes );


   FillList(); // update the placed probe count
   UpdateData(FALSE);

}

void TestProbePlacement::OnView() 
{
   // TODO: Add your control notification handler code here
   // TODO: Add your control notification handler code here
   CString filename;
   CFileDialog FileDialog(FALSE, "csv", "*.csv",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Debug Access Report (*.csv)|*.csv|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   filename = FileDialog.GetPathName();
   
   TA_ProbeplaceDebugReport(filename, doc);
}

void TestProbePlacement::OnTaProbeHelp() 
{
   // TODO: Add your control notification handler code here
   ErrorMessage("On Help");
}

void TestProbePlacement::OnOK() 
{
   // TODO: Add extra validation here
   CDialog::OnOK();

   TA_Probeplace_Update(doc, pcbFile, &list);

   TA_TestProbeDeInit();
   RemoveAll();
   DestroyWindow();
   doc->UpdateAllViews(NULL);
}

void TestProbePlacement::OnCancel() 
{
   // TODO: Add extra cleanup here
   if (ErrorMessage("Test Probe Placement Information will not be updated.\nTo save the Test Probe Placement Information use \"Update Database\"\nDo you want to Cancel?", "Cancel Test Probe Placement", MB_YESNO | MB_DEFBUTTON2)==IDYES)
   {
      CDialog::OnCancel();
   
      TA_TestProbeDeInit();

      RemoveAll();
      DestroyWindow();
   }
}

void TestProbePlacement::FillList() 
{

   m_list.DeleteAllItems();

   CString buf;
   LV_ITEM item;
   int actualItem;
   int j = 0;
   POSITION pos = list.GetHeadPosition();
   while (pos != NULL)
   {
      TestProbeStruct *tp = list.GetNext(pos);

      item.mask = LVIF_TEXT;
      item.iItem = j++;
      item.iSubItem = 0;

      if (tp->onoff)
         buf = "On";
      else
         buf = "Off";
      item.pszText = (char*)(LPCTSTR)buf;

      actualItem = m_list.InsertItem(&item);
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_list.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_list.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_list.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_list.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_list.InsertItem(&item);
               }
            }
         }
      }

      m_list.SetItemData(actualItem, (DWORD)tp);

      item.iSubItem = 1;
      buf.Format("%d", tp->cost);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iItem = actualItem;
      item.iSubItem = 2;
      item.pszText = (char*)(LPCTSTR)tp->probename;
      m_list.SetItem(&item);

      item.iSubItem = 3;
      buf.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), tp->size);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 4;
      item.pszText = (char*)(LPCTSTR)tp->description;
      m_list.SetItem(&item);

      item.iSubItem = 5;
      buf.Format("%d", tp->placedtop);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 6;
      buf.Format("%d", tp->placedbot);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 7;
      buf.Format("%s", tp->probedrilltool);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 8;
      buf.Format("%s", tp->probeprefix);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 9;
      buf.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), tp->drillsize);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 10;
      buf.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), tp->headsize);
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

   }
}

void TestProbePlacement::RemoveAll() 
{
   m_list.DeleteAllItems();

   POSITION pos = list.GetHeadPosition();
   while (pos != NULL)
      delete list.GetNext(pos);

   list.RemoveAll();

}

void TestProbePlacement::OnClearProbe() 
{
   // TODO: Add your control notification handler code here
   ErrorMessage("Clear Probe ??");
}

void TestProbePlacement::OnLoad() 
{
   // TODO: Add your control notification handler code here
   
}

void TestProbePlacement::OnSave() 
{
   // TODO: Add your control notification handler code here
   
}

static void DeleteTestProbeData(CCEtoODBDoc *doc, CDataList *DataList);

/******************************************************************************
* refname_already_used
*/
static DataStruct* refname_already_used(CCEtoODBDoc *doc, FileStruct *pcbfile, const char *name)
{
   return datastruct_from_refdes(doc, pcbfile->getBlock(), name);
}

/******************************************************************************
* TA_Probeplace_Update
*/
static int TA_Probeplace_Update(CCEtoODBDoc *doc, FileStruct *pcbfile, TestProbeList *probelist)
{
   int   i;
   int   probecnt = 0;
   CWaitCursor w;

   // first delete all existing probes
   DeleteTestProbeData(doc, &(pcbfile->getBlock()->getDataList())); 

   // here loop through all probe names

   for (i=0;i<probePlaceArrayCount;i++)
   {
#ifdef _DEBUG
      TA_ProbePlace *pp = probePlaceArray[i];
#endif

      if (probePlaceArray[i]->probe_placed == 0)   
      {
         continue;
      }

      CString  lname, refname;
      DataStruct *d;

      if (!Graph_Block_Exists(doc, probePlaceArray[i]->probe_name, -1))
      {
         TestProbeStruct *tp = get_testprobestruct(probelist, probePlaceArray[i]->probe_index);
         generate_TestProbeGeometry(doc, tp->probename, tp->size*2, tp->probedrilltool, tp->drillsize);
      }

#ifdef _DEBUG
      TA_ProbePlace *p = probePlaceArray[i];
#endif

      int   mir = 0;
      if (probePlaceArray[i]->allowedlayer == 2)   mir = TRUE;

      do
      {
         refname.Format("%s%d", probePlaceArray[i]->probe_prefix, ++probecnt);
      } while (refname_already_used(doc, pcbfile, refname));

      d = Graph_Block_Reference(probePlaceArray[i]->probe_name, refname, 0, 
            probePlaceArray[i]->x, probePlaceArray[i]->y, 0.0, mir, 1.0, -1, TRUE);
      d->getInsert()->setInsertType(insertTypeTestProbe);
      
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
            VT_INTEGER,
            &(probePlaceArray[i]->access_entitynum), SA_APPEND, NULL);

      // do not make a netname for nonconn probes.
      if (!probePlaceNetArray[probePlaceArray[i]->netindex]->nonconn)
      {
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
            VT_STRING,
            probePlaceNetArray[probePlaceArray[i]->netindex]->netname.GetBuffer(0), SA_APPEND, NULL);
      }
   }
   return 1;
}

/******************************************************************************
* DeleteTestProbeData
*/
static void DeleteTestProbeData(CCEtoODBDoc *doc, CDataList *DataList)
{
   doc->UnselectAll(TRUE);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_TEST_PROBE)
         RemoveOneEntityFromDataList(doc, DataList, data);
   }
}

/******************************************************************************
* TA_ProbeplaceDebugReport 
*/
static void TA_ProbeplaceDebugReport(const char *filename, CCEtoODBDoc *doc)
{
   FILE *fp = fopen(filename, "wt");
   if (!fp)
   {
      CString err;
      err.Format("Unable to open file [%s] for writing.\nMake sure it is not Read-Only.", filename);
      ErrorMessage(err, "Unable to Save Report");
      return;
   }

   int output_units_accuracy = GetDecimals(doc->getSettings().getPageUnits()); 
   int   i;

   fprintf(fp,"Netname, number_required_probes, number_placed_probes, number_of_access_top, number_of_access_bot\n");
   for (i=0;i<probePlaceNetArrayCount;i++)
   {
      fprintf(fp,"%s, %d, %d, %d, %d\n",
         probePlaceNetArray[i]->netname,
         probePlaceNetArray[i]->number_required_probes,
         probePlaceNetArray[i]->number_placed_probes,
         probePlaceNetArray[i]->number_of_access_top,
         probePlaceNetArray[i]->number_of_access_bot);
   }

   fprintf(fp,"Netname, Cost, LayerCost, TestAccessNum, X, Y, Layer, AllowedLayer, A/Pref, A/Net Top, A/Net Bot, Placed, Probesize \n");
   for (i=0;i<probePlaceArrayCount;i++)
   {
      CString  n;

      n = probePlaceNetArray[probePlaceArray[i]->netindex]->netname;
      //          n   cos lcos ent  x    y    sl  al  pr  tp  bot pl  siz
      fprintf(fp,"%s, %d, %d, %ld, %lg, %lg, %d, %d, %d, %d, %d, %d, %lg\n",
         n,
         probePlaceArray[i]->cost,probePlaceArray[i]->layercost,
         probePlaceArray[i]->access_entitynum,
         probePlaceArray[i]->x, probePlaceArray[i]->y,
         probePlaceArray[i]->slayer, 
         probePlaceArray[i]->allowedlayer, 
         probePlaceArray[i]->preference,
         probePlaceArray[i]->net_numberofaccess_top, probePlaceArray[i]->net_numberofaccess_bot,
         probePlaceArray[i]->probe_placed, probePlaceArray[i]->probe_size );
   }
   fclose(fp);

   Notepad(filename);
}
