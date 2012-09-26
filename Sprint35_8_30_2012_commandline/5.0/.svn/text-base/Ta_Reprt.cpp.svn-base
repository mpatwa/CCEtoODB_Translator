// $Header: /CAMCAD/4.6/Ta_Reprt.cpp 38    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

/*************************RECENT UPDATES*****************************
*
*  05-24-02: TSR 3639 - Sadek Noureddine
*
*
*
*
*
*
*/

#include "stdafx.h"
#include "ta_reprt.h"
#include "ccdoc.h"
#include "crypt.h"
#include "pcbutil.h"
#include "ta.h"
#include "net_util.h"
#include "dirdlg.h"
#include <direct.h>
#include "CCEtoODB.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static void TA_Report(CCEtoODBDoc *doc, int testability, const char *testabilityfilename, int testprobe, const char *testprobefilename);
static int testability_report(const char *fname, CCEtoODBDoc *doc, FileStruct *file, int output_units_accuracy);
static int testprobe_report(const char *fname, CCEtoODBDoc *doc, FileStruct *file, int output_units_accuracy);
static void report_load_testprobes(FILE *flog, FileStruct *file, CCEtoODBDoc *doc);
static void report_signal_no_probes(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, const char *netname, int output_units_accuracy);
static void report_signal_no_access(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, const char *netname, int output_units_accuracy);
static void report_nonconn_no_access(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, long ent, int output_units_accuracy);
static void report_nonconn_no_probes(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, long ent, int output_units_accuracy);
static int get_datalink_netnameptr(long ddlink);
static int get_report_probenameptr(CString probeName);
static int get_probenet_from_accessnet(long ddlink);
static char *get_pinsurface(CCEtoODBDoc *doc, CompPinStruct *compPin);

TA_NetnameArray TA_netnameArray;
int TA_netnameArrayCount;

static TestAccessArray testAccessArray;
static int testAccessArrayCount;

static TestProbeArray testProbeArray;
static int testProbeArrayCount;

static CStringArray probeNameArray;
static int probeNameArrayCount;

/**************************************************************************
* OnTestabilityReport
*/
void CCEtoODBDoc::OnTestabilityReport() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft)) 
   {
      ErrorAccess("You do not have a License for Testability Report!");
      return;
   }*/

   TA_Reports dlg;
   dlg.m_testability = TRUE;
   dlg.m_testabilityFile = this->GetProjectPath(getApp().getUserPath()) + "testability.rep";

   dlg.m_testprobe = FALSE;
   dlg.m_testprobeFile = this->GetProjectPath(getApp().getUserPath()) + "testprobe.rep";

   if (dlg.DoModal() != IDOK)
      return;

   TA_Report(this, dlg.m_testability, dlg.m_testabilityFile, dlg.m_testprobe, dlg.m_testprobeFile);
}

/*************************************************************************
* TA_Report
*/
void TA_Report(CCEtoODBDoc *doc, int testability, const char *testabilityfilename, int testprobe, const char *testprobefilename)
{
	CString taLogFile = GetLogfilePath("ta.log");
   remove(taLogFile);

   FILE *fLog = fopen(taLogFile, "wt");
   if (!fLog)
   {
      CString err;
      err.Format("Error open [%s] file", taLogFile);
      ErrorMessage(err, "Error");
      return;
   }

   int output_units_accuracy = GetDecimals(doc->getSettings().getPageUnits()); 
   generate_PADSTACKACCESSFLAG(doc, 0);

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      if (file->isShown() && file->getBlockType() == BLOCKTYPE_PCB)
      {
         testAccessArray.SetSize(100,100);
         testAccessArrayCount = 0;

         testProbeArray.SetSize(100,100);
         testProbeArrayCount = 0;

         TA_netnameArray.SetSize(100,100);
         TA_netnameArrayCount = 0;

         probeNameArray.SetSize(100,100);
         probeNameArrayCount = 0;

         CWaitCursor w;

         report_load_netlist(file, doc);
         report_load_testprobes(fLog, file, doc);

         if (testability)
         {
            testability_report(testabilityfilename, doc, file, output_units_accuracy);
            Notepad(testabilityfilename);
         }
         if (testprobe)
         {
            testprobe_report(testprobefilename, doc, file, output_units_accuracy);
            Notepad(testprobefilename);
         }

         int   i;
         for (i=0;i<testAccessArrayCount;i++)
         {
            delete testAccessArray[i];
         }
         testAccessArray.RemoveAll();
         testAccessArrayCount = 0;

         for (i=0;i<testProbeArrayCount;i++)
         {
            delete testProbeArray[i];
         }
         testProbeArray.RemoveAll();
         testProbeArrayCount = 0;

         for (i=0;i<TA_netnameArrayCount;i++)
         {
            delete TA_netnameArray[i]; 
         }
         TA_netnameArray.RemoveAll();
         TA_netnameArrayCount = 0;

         probeNameArray.RemoveAll();
         probeNameArrayCount = 0;
      }
   }

   fclose(fLog);
}

/******************************************************************************
* testability_report
*/
static int testability_report(const char *fname, CCEtoODBDoc *doc, FileStruct *file, int output_units_accuracy)
{

   FILE *fp;

   if ((fp = fopen(fname,"wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Unable to open file [%s] for writing.\nMake sure it is not Read-Only.", fname);
      ErrorMessage(tmp, "Unable to Save Report");
      return 0;
   }

   fprintf(fp, "Testability Access and Probe Placement Report\n\n");
   fprintf(fp, "%s\n", getApp().getCompanyNameString());
   fprintf(fp, "Created by %s\n", getApp().getCamCadTitle());
   CTime t = t.GetCurrentTime();
   fprintf(fp, "%s\n", t.Format("Date & Time : %A, %B %d, %Y at %H:%M:%S"));
   fprintf(fp, "\n");
   fprintf(fp, "Project  : %s\n", doc->GetProjectPath());
   fprintf(fp, "PCB File : %s\n", file->getName());
   fprintf(fp, "Units    : %s\n", GetUnitName(doc->getSettings().getPageUnits()));
   fprintf(fp, "\n");
   // loop throu all PCB files.

   int multi_qty = 0;
   int multi_top = 0;
   int multi_bot = 0;
   int multi_not = 0;
   int single_qty = 0;
   int single_top = 0;
   int single_bot = 0;
   int single_not = 0;
   CString multi_cov_top;
   CString multi_cov_bot;
   CString single_cov_top;
   CString single_cov_bot;
   
	int i=0;
   for (i=0; i<TA_netnameArrayCount; i++)
   {
#ifdef _DEBUG
      TA_Netname *nn = TA_netnameArray[i];
#endif

      int top = 0;
      int bot = 0;
      for (int ii=0; ii<testAccessArrayCount; ii++)
      {
         if (testAccessArray[ii]->netindex == i)
         {
            if (testAccessArray[ii]->layer & 1)
               top++;
            if (testAccessArray[ii]->layer & 2)
               bot++;
         }
      }

      if (TA_netnameArray[i]->nonconn)
      {
         single_qty++;
         if (top)
            single_top++;
         if (bot)
            single_bot++;
         if (!top && !bot)
            single_not++;
      }
      else
      {
         multi_qty++;
         if (top)
            multi_top++;
         if (bot)
            multi_bot++;
         if (!top && !bot)
            multi_not++;
      }
   }

   TA_Percent(&multi_cov_top, multi_top, multi_qty);
   TA_Percent(&multi_cov_bot, multi_bot, multi_qty);
   TA_Percent(&single_cov_top, single_top, single_qty);
   TA_Percent(&single_cov_bot, single_bot, single_qty);

   fprintf(fp, "-----------------------------------------------------------------\n");
   fprintf(fp, "Global Net Access Information\n\n");
   fprintf(fp, "                          Accessibility      Coverage\n");
   fprintf(fp, "                    Qty   Top   Bot   None   Top    Bot\n");
   fprintf(fp, "\n");
   fprintf(fp, "Multi Point Nets    %-5d %-5d %-5d %-5d  %-6s %-6s\n",
         multi_qty, multi_top, multi_bot, multi_not, multi_cov_top, multi_cov_bot);
   fprintf(fp, "Single Point Nets   %-5d %-5d %-5d %-5d  %-6s %-6s\n",
         single_qty, single_top, single_bot, single_not, single_cov_top, single_cov_bot);
   fprintf(fp,"\n");

   single_qty = 0;
   single_not = 0;
   single_top = 0;
   single_bot = 0;
   multi_qty = 0;
   multi_not = 0;
   multi_top = 0;
   multi_bot = 0;

   for (i=0; i<TA_netnameArrayCount; i++)
   {
#ifdef _DEBUG
TA_Netname *n = TA_netnameArray[i];
#endif
      int top = 0;
      int bot = 0;
      for (int ii=0; ii<testProbeArrayCount;ii++)
      {
#ifdef _DEBUG
TA_TestProbe *t = testProbeArray[ii];
#endif
         if (testProbeArray[ii]->netindex == i)
         {
            if (testProbeArray[ii]->layer & 1)
               top++;
            if (testProbeArray[ii]->layer & 2)
               bot++;
         }
      }

      if (TA_netnameArray[i]->nonconn)
      {
         single_qty++;
         if (top)
            single_top++;
         if (bot)
            single_bot++;
         if (!top && !bot)
            single_not++;
      }
      else
      {
         multi_qty++;
         if (top)
            multi_top++;
         if (bot)
            multi_bot++;
         if (!top && !bot)
            multi_not++;
      }
   }

   TA_Percent(&multi_cov_top, multi_top, multi_qty);
   TA_Percent(&multi_cov_bot, multi_bot, multi_qty);
   TA_Percent(&single_cov_top, single_top, single_qty);
   TA_Percent(&single_cov_bot, single_bot, single_qty);

   fprintf(fp,"-----------------------------------------------------------------\n");
   fprintf(fp,"Global Probe Placement Information\n\n");
   fprintf(fp,"                  Probes Placed        Coverage\n");      
   fprintf(fp,"                  Qty   Top   Bot      Top    Bot\n");
   fprintf(fp,"\n");
   fprintf(fp,"Multi Point Nets  %-5d %-5d %-5d    %-6s %-6s\n",
         multi_qty, multi_top, multi_bot, multi_cov_top, multi_cov_bot );      
   fprintf(fp,"\n");

   // here get the different probes
   for (i=0; i<probeNameArrayCount; i++)
   {
      int p_qty = 0;
      int p_top = 0;
      int p_bot = 0;
      for (int ii=0;ii<testProbeArrayCount;ii++)
      {
         if (testProbeArray[ii]->netindex < 0)
            continue; // probe has no net index.
         if (TA_netnameArray[testProbeArray[ii]->netindex]->nonconn)
            continue;

         if (testProbeArray[ii]->probeindex == i)
         {
            p_qty++;
            if (testProbeArray[ii]->layer & 1)
               p_top++;
            if (testProbeArray[ii]->layer & 2)
               p_bot++;
         }
      }
      if (p_qty)
         fprintf(fp, "%-17s %-5d %-5d %-5d\n", probeNameArray[i], p_qty, p_top, p_bot);                
   }

   fprintf(fp, "\n");
   fprintf(fp, "Single Point Nets %-5d %-5d %-5d    %-6s %-6s\n", 
         single_qty, single_top, single_bot, single_cov_top, single_cov_bot );      
   fprintf(fp, "\n");

   // here get the different probes
   for (i=0; i<probeNameArrayCount; i++)
   {
      int p_qty = 0;
      int p_top = 0;
      int p_bot = 0;
      for (int ii=0; ii<testProbeArrayCount; ii++)
      {
#ifdef _DEBUG
TA_TestProbe *t = testProbeArray[ii];
#endif
         if (testProbeArray[ii]->netindex < 0)
            continue; // probe has no net index.
         if (!TA_netnameArray[testProbeArray[ii]->netindex]->nonconn)
            continue;

         if (testProbeArray[ii]->probeindex == i)
         {
            p_qty++;
            if (testProbeArray[ii]->layer & 1)
               p_top++;
            if (testProbeArray[ii]->layer & 2)
               p_bot++;
         }
      }
      if (p_qty)
         fprintf(fp,"%-17s %-5d %-5d %-5d\n", probeNameArray[i], p_qty, p_top, p_bot);              
   }

   fprintf(fp, "\n");
   fprintf(fp, "-----------------------------------------------------------------\n");
   fprintf(fp, "Multi Point Nets With No Access\n\n");
   fprintf(fp, "Feature              Surface  X Loc   Y Loc   Violation\n");
   fprintf(fp, "\n");             

   for (i=0; i<TA_netnameArrayCount; i++)
   {
      if (!TA_netnameArray[i]->nonconn && !TA_netnameArray[i]->accesscnt)
      {
         // this can be multiple per net
         report_signal_no_access(fp, doc, file, TA_netnameArray[i]->netname, output_units_accuracy);
      }
   }

   fprintf(fp, "\n");
   fprintf(fp, "-----------------------------------------------------------------\n");
   fprintf(fp, "Single Point Nets With No Access\n\n");
   fprintf(fp, "Feature              Surface  X Loc   Y Loc   Violation\n");
   fprintf(fp, "\n");             

   for (i=0; i<TA_netnameArrayCount; i++)
   {
      if (TA_netnameArray[i]->nonconn && !TA_netnameArray[i]->accesscnt)
      {
         // this can only be one per unused net.
         // fprintf(fp,"Net : %s\n", TA_netnameArray[i]->netname);
         report_nonconn_no_access(fp, doc, file, TA_netnameArray[i]->datalink, output_units_accuracy);
      }
   }

   fprintf(fp, "\n");
   fprintf(fp, "-----------------------------------------------------------------\n");
   fprintf(fp, "Multi Point Nets Needing Additional Probes\n\n");
   fprintf(fp, "Feature             Surface  X Loc  Y Loc  Violation\n");
   fprintf(fp, "\n");             

   for (i=0; i<TA_netnameArrayCount; i++)
   {
#ifdef _DEBUG
TA_Netname *n = TA_netnameArray[i];
#endif
      if (!TA_netnameArray[i]->nonconn && TA_netnameArray[i]->probecnt < TA_netnameArray[i]->probeneeded)
      {
         // this can be multiple per net
         report_signal_no_probes(fp, doc, file, TA_netnameArray[i]->netname, output_units_accuracy);
      }
   }

   fprintf(fp, "\n");
   fprintf(fp, "-----------------------------------------------------------------\n");
   fprintf(fp, "Single Point Nets Needing Additional Probes\n");
   fprintf(fp, "Feature             Surface  X Loc  Y Loc  Violation\n");
   fprintf(fp, "\n");             

   for (i=0; i<TA_netnameArrayCount; i++)
   {
#ifdef _DEBUG
TA_Netname *n = TA_netnameArray[i];
#endif
      if (TA_netnameArray[i]->nonconn && TA_netnameArray[i]->probecnt < TA_netnameArray[i]->probeneeded)
      {
         // this can only be one per unused net.
         // fprintf(fp,"Net : %s\n", TA_netnameArray[i]->netname);
         report_nonconn_no_probes(fp, doc, file, TA_netnameArray[i]->datalink, output_units_accuracy);
      }
   }

   fprintf(fp, "\n");
   fprintf(fp, "\nEnd\n");
   fclose(fp);
   return 1;
}

/******************************************************************************
* report_nonconn_no_probes
*/
static void report_nonconn_no_probes(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, long ent, int output_units_accuracy)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   Attrib   *a;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
 
      // there can be single features in netlist
      //if (!(net->getFlags() & NETFLAG_UNUSEDNET)) continue;

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (compPin->getEntityNumber() == ent)
         {
            CString  errmsg;
            if (a = is_attvalue(doc, compPin->getAttributesRef(), ATT_TESTACCESS_FAIL, 0))
               errmsg = get_attvalue_string(doc, a);
            else
            if (a = is_attvalue(doc, compPin->getAttributesRef(), ATT_TESTPROBE_FAIL, 0))
               errmsg = get_attvalue_string(doc, a);
            else
               errmsg = "";

            CString  cp, px, py, surface;
            cp.Format("%s-%s", compPin->getRefDes(), compPin->getPinName());
            px.Format("%1.*lf", output_units_accuracy, compPin->getOriginX());
            py.Format("%1.*lf", output_units_accuracy, compPin->getOriginY());

            surface = get_pinsurface(doc, compPin);
            fprintf(fp,"%-20s %-8s %-7s %-7s %s\n", cp, surface, px, py, errmsg);
         }
      }
   }
}

/******************************************************************************
* report_nonconn_no_access
*/
static void report_nonconn_no_access(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, long ent, int output_units_accuracy)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   Attrib   *a;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
 
      // there can be single features in netlist
      //if (!(net->getFlags() & NETFLAG_UNUSEDNET)) continue;

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (compPin->getEntityNumber() == ent)
         {
            CString  errmsg;
            if (a = is_attvalue(doc, compPin->getAttributesRef(), ATT_TESTACCESS_FAIL, 0))
               errmsg = get_attvalue_string(doc, a);
            else
               errmsg = "";

            CString  cp, px, py, surface;
            cp.Format("%s-%s", compPin->getRefDes(), compPin->getPinName());
            px.Format("%1.*lf", output_units_accuracy, compPin->getOriginX());
            py.Format("%1.*lf", output_units_accuracy, compPin->getOriginY());

            surface = get_pinsurface(doc, compPin);

            fprintf(fp,"%-20s %-8s %-7s %-7s %s\n", cp, surface, px, py, errmsg);
            return;
         }
      }
   }
}

/******************************************************************************
* report_signal_no_access
*/
static void report_signal_no_access(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, const char *netname, int output_units_accuracy)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   Attrib   *a;
   int      written = FALSE;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);

      if (net->getNetName().Compare(netname))  continue;   // if not equal

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         CString  errmsg;
         if (a = is_attvalue(doc, compPin->getAttributesRef(), ATT_TESTACCESS_FAIL, 0))
            errmsg = get_attvalue_string(doc, a);
         else
            errmsg = "";

         CString  cp, px, py, surface;
         cp.Format("%s-%s", compPin->getRefDes(), compPin->getPinName());
         px.Format("%1.*lf", output_units_accuracy, compPin->getOriginX());
         py.Format("%1.*lf", output_units_accuracy, compPin->getOriginY());

         surface = get_pinsurface(doc, compPin);

         if (!written)
            fprintf(fp,"NET : %s\n", netname);
         written = TRUE;
         fprintf(fp,"%-20s %-8s %-7s %-7s %s\n", cp, surface, px, py, errmsg);
      }
   } // while

   if (written)
      fprintf(fp,"\n");
}

/******************************************************************************
* report_signal_no_probes
*/
static void report_signal_no_probes(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, const char *netname, int output_units_accuracy)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   Attrib   *a;
   int      written = FALSE;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);

      if (net->getNetName().Compare(netname))  continue;   // if not equal

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         CString  errmsg;

         // if no access, there is no probe
         if (a = is_attvalue(doc, compPin->getAttributesRef(), ATT_TESTACCESS_FAIL, 0))
            errmsg = get_attvalue_string(doc, a);
         else
         if (a = is_attvalue(doc, compPin->getAttributesRef(), ATT_TESTPROBE_FAIL, 0))
            errmsg = get_attvalue_string(doc, a);
         else
            errmsg = "";

         CString  cp, px, py, surface;
         cp.Format("%s-%s", compPin->getRefDes(), compPin->getPinName());
         px.Format("%1.*lf", output_units_accuracy, compPin->getOriginX());
         py.Format("%1.*lf", output_units_accuracy, compPin->getOriginY());

         surface = get_pinsurface(doc, compPin);

         if (!written)
            fprintf(fp,"NET : %s\n", netname);
         written = TRUE;            
         fprintf(fp,"%-20s %-8s %-7s %-7s %s\n", cp, surface, px, py, errmsg);
      }
   } // while

   if (written)
      fprintf(fp,"\n");
}

/******************************************************************************
* get_pinsurface
*/
static char *get_pinsurface(CCEtoODBDoc *doc, CompPinStruct *compPin)
{
   char  *surface;
   int   access = 0;

   surface = "None";
   if (compPin->getPadstackBlockNumber() > -1)
   {
      BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());

      if (block == NULL)
      {
#ifdef _DEBUG
         CString tmp;

         tmp.Format("Entity %ld Component [%s] Pin [%s] Padstack Num %d found found", 
            compPin->getEntityNumber(), compPin->getRefDes(), compPin->getPinName(), compPin->getPadstackBlockNumber());
         ErrorMessage(tmp, "Error in get_pinsurface");
#endif
         access = 0;
      }
      /* TSR 3639: "|| block->getFlags() & BL_ACCESS_TOP_ONLY" and "|| block->getFlags() & BL_ACCESS_BOTTOM_ONLY" added.
      Before, if the flag is top only,the access wasn't being assigned to top and the same happens to bottom!*/

      else if (block->getFlags() & BL_ACCESS_NONE)
         access = 0;
      else if ((block->getFlags() & BL_ACCESS_OUTER) == BL_ACCESS_OUTER)
         access = 3;
      /*********************************TSR 3639******************************
      Before: else if (block->getFlags() & BL_ACCESS_TOP)*/

      else if (block->getFlags() & BL_ACCESS_TOP || block->getFlags() & BL_ACCESS_TOP_ONLY)
         access = 1;
      else if (block->getFlags() & BL_ACCESS_BOTTOM || block->getFlags() & BL_ACCESS_BOTTOM_ONLY)
         access = 2;
   
      if (access == 1 && compPin->getMirror()) 
         access = 2;
      else if (access == 2 && compPin->getMirror())  
         access = 1;

      if (access == 1)
         surface = "Top";
      else if (access == 2)
         surface = "Bottom";
      else if (access == 3)
         surface = "Both";
   }
   else
   {
      surface = "unknown";
   }

   return surface;
}

/******************************************************************************
* testprobe_report
*/
static int testprobe_report(const char *fname, CCEtoODBDoc *doc, FileStruct *file, int output_units_accuracy)
{
   FILE  *fp;

   if ((fp = fopen(fname,"wt")) == NULL)
   {
      CString  tmp;

      tmp.Format("Unable to open file [%s] for writing.\nMake sure it is not Read-Only.", fname);
      ErrorMessage(tmp, "Unable to Save Report");
      return 0;
   }

   fprintf(fp,"Probe XY Report\n\n");
   fprintf(fp,"%s\n", getApp().getCompanyNameString());
   fprintf(fp,"Created by %s\n", getApp().getCamCadTitle());
   CTime t = t.GetCurrentTime();
   fprintf(fp,"%s\n",t.Format("Date & Time : %A, %B %d, %Y at %H:%M:%S"));
   fprintf(fp,"\n");

   fprintf(fp,"Project  : %s\n", doc->GetProjectPath());

   fprintf(fp,"PCB File : %s\n", file->getName());
   fprintf(fp,"Units    : %s\n", GetUnitName(doc->getSettings().getPageUnits()));
   fprintf(fp,"\n");
   // loop throu all PCB files.

   int ii;
   fprintf(fp,"Num  Ref Name   Probe Name     Surface   Loaded   Drill   XLoc    YLoc    Netname\n");
   for (ii=0;ii<testProbeArrayCount;ii++)
   {
      TA_TestProbe *t = testProbeArray[ii];
      char     *l = "NONE";
      double   drillsize = 0.0; 
      if (testProbeArray[ii]->layer == 1)
         l = "TOP";
      else
      if (testProbeArray[ii]->layer == 2)
         l = "BOT";
      else
         l = "BOTH";

      CString  px, py, dr;
      dr.Format("%1.*lf", output_units_accuracy, drillsize);
      px.Format("%1.*lf", output_units_accuracy, t->x);
      py.Format("%1.*lf", output_units_accuracy, t->y);

      fprintf(fp,"%-04d %-10s %-14s %-9s %-8s %-7s %-7s %-7s %s\n",
         ii+1, t->refname, t->geomname, l, "YES", dr, px, py, TA_netnameArray[testProbeArray[ii]->netindex]->netname);
   }

   fprintf(fp,"\n");

   fprintf(fp,"\nEnd\n");

   fclose(fp);

   return 1;
}

/******************************************************************************
* report_load_netlist
*/
void report_load_netlist(FileStruct *file, CCEtoODBDoc *doc)
{
   int netindex;
   int singlepin = FALSE;

   WORD testNetProbes = doc->IsKeyWord(ATT_TEST_NET_PROBES, 0);

   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);
 
      if (net->getFlags() & NETFLAG_UNUSEDNET)
      {
         netindex = -1;
         singlepin = TRUE;
      }
      else
      {
         singlepin = FALSE;

         if (net->getCompPinCount() < 2)
            singlepin = TRUE;

         netindex = get_report_netnameptr(net->getNetName());

         Attrib* attrib;

         if (net->getAttributesRef() && net->getAttributesRef()->Lookup(testNetProbes, attrib))
         {
            if (attrib && attrib->getIntValue() > 1)
               TA_netnameArray[netindex]->probeneeded = attrib->getIntValue();
         }
      }

      // SaveAttribs(stream, &net->getAttributesRef());
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         if (singlepin) // unused net or single pin
         {
            int nptr = netindex;
            if (netindex < 0)
            {
               CString nname = GenerateSinglePinNetname(compPin->getRefDes(), compPin->getPinName());
               nptr = get_report_netnameptr(nname);
            }
            TA_netnameArray[nptr]->nonconn = singlepin;
            TA_netnameArray[nptr]->datalink = compPin->getEntityNumber();
         }
      }
   }
}

/******************************************************************************
* report_load_testprobes
*/
static void report_load_testprobes(FILE *flog, FileStruct *file, CCEtoODBDoc *doc)
{
   int      mirror;
   double   scale;
   double   rotation;
   double   insert_x, insert_y;
   int      nonconncnt = 0;

   mirror = file->isMirrored();
   rotation = file->getRotation();
   scale = file->getScale();
   insert_x = file->getInsertX();
   insert_y = file->getInsertY();

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPAD || data->getInsert()->getInsertType() == INSERTTYPE_TEST_ACCESSPOINT) 
      {
         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x * scale;
         point2.y = data->getInsert()->getOriginY();
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         TA_TestAccess *c = new TA_TestAccess;
         testAccessArray.SetAtGrow(testAccessArrayCount,c);  
         testAccessArrayCount++; 
         c->entitynumber = data->getEntityNumber();
         //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         //double block_rot = rotation + np->getInsert()->getAngle();
         c->geomname = block->getName();
         c->refname = data->getInsert()->getRefname();

         // default to placement layer
         if (data->getInsert()->getMirrorFlags() & MIRROR_LAYERS)
            c->layer = 2;
         else
            c->layer = 1;

         c->x = point2.x;
         c->y = point2.y;
         c->netindex = -1;
         c->usecnt = 0;
         c->datalink = 0;
         Attrib *a;

         if (a = is_attvalue(doc, data->getAttributesRef(), ATT_DDLINK, 0))
         {
            CString  l;
            l = get_attvalue_string(doc, a);
            c->datalink = atol(l);
         }

         if (a = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 0))
         {
            CString  l;
            l = get_attvalue_string(doc, a);
            c->netindex = get_report_netnameptr(l);
            c->usecnt++;
         }
         else
         {
            if (c->datalink)
            {
               // single pin nets have no netname, get one - just not the same as one before
               c->netindex = get_datalink_netnameptr(c->datalink);   
            }
            else
            {
               //ErrorMessage("No Datatlink available ???", "DFT Report");
               fprintf(flog, " Testpoint/TestAccess [%s] Entity nr [%ld] must have a NETNAME or DDLINK\n",
                     data->getInsert()->getRefname(), data->getEntityNumber());
            }

            c->usecnt++;
         }
      } 
      else if (data->getInsert()->getInsertType() == INSERTTYPE_TEST_PROBE) 
      {
         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x * scale;
         point2.y = data->getInsert()->getOriginY();
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         TA_TestProbe *c = new TA_TestProbe;
         testProbeArray.SetAtGrow(testProbeArrayCount,c);  
         testProbeArrayCount++;  

         Attrib *a;

         if (a = is_attvalue(doc, data->getAttributesRef(), ATT_DDLINK, 0))
         {
            CString  l;
            l = get_attvalue_string(doc, a);
            c->datalink = atol(l);
         }
         else
         {
            ErrorMessage("Probe without a datalink");
            c->datalink = 0;
         }

         //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         //double block_rot = rotation + np->getInsert()->getAngle();
         c->geomname = block->getName();
         c->refname = data->getInsert()->getRefname();

         // default to placement layer
         if (data->getInsert()->getPlacedBottom())
            c->layer = 2;
         else
            c->layer = 1;
         c->probeindex = get_report_probenameptr(block->getName());
         c->x = point2.x;
         c->y = point2.y;
         c->netindex = -1;
         c->usecnt = 0;
      } 
   }


   // here now assign the netnames to the probes.
   // accesspoints have netnames, probe link to access points.
	int i=0;
   for (i=0; i<testProbeArrayCount; i++)
   {
      testProbeArray[i]->netindex = get_probenet_from_accessnet(testProbeArray[i]->datalink);
      TA_netnameArray[testProbeArray[i]->netindex]->probecnt++;
   }

   // here update TA_netnameArray with access cnt;
   for (i=0; i<testAccessArrayCount; i++)
   {
      if (testAccessArray[i]->netindex > -1)
      {
         TA_netnameArray[testAccessArray[i]->netindex]->accesscnt++;
      }
   }
}

/******************************************************************************
* get_probenet_from_accessnet
*/
static int get_probenet_from_accessnet(long ddlink)
{
   for (int i=0;i<testAccessArrayCount;i++)
   {
      if (testAccessArray[i]->entitynumber == ddlink)
         return testAccessArray[i]->netindex;
   }

   return -1;
}

/******************************************************************************
* get_datalink_netnameptr 
   ATT_DDLINK.
   This is to assign the NONCON elements to the correct net
*/
static int get_datalink_netnameptr(long ddlink)
{
   // get here the data link
   for (int i=0; i<TA_netnameArrayCount; i++)
   {
      if (ddlink == TA_netnameArray[i]->datalink)
         return i;
   }


   //ErrorMessage("???", "Could not find DDlink");

   return -1;
}

/******************************************************************************
* get_report_netnameptr
*/
int get_report_netnameptr(const char *c)
{
   int   i;

   for (i=0;i<TA_netnameArrayCount;i++)
   {
      if (!strcmp(TA_netnameArray[i]->netname, c))
      {
         return i;
      }
   }

   TA_Netname *n = new TA_Netname;
   TA_netnameArray.SetAtGrow(TA_netnameArrayCount,n);  
   TA_netnameArrayCount++; 
   n->netname = c;
   n->probecnt = 0;
   n->probeneeded = 1;
   n->accesscnt = 0;
   n->nonconn = 0;
   n->datalink = 0;
   return TA_netnameArrayCount-1;
}

/******************************************************************************
* get_report_probenameptr
*/
static int get_report_probenameptr(CString probeName)
{
   for (int i=0;i<probeNameArrayCount;i++)
   {
      if (!probeName.Compare(probeNameArray[i]))
         return i;
   }

   probeNameArray.SetAtGrow(probeNameArrayCount++, probeName);  

   return probeNameArrayCount-1;
}


/////////////////////////////////////////////////////////////////////////////
// TA_Reports dialog
TA_Reports::TA_Reports(CWnd* pParent /*=NULL*/)
   : CDialog(TA_Reports::IDD, pParent)
{
   //{{AFX_DATA_INIT(TA_Reports)
   m_testability = FALSE;
   m_testabilityFile = _T("");
   m_testprobe = FALSE;
   m_testprobeFile = _T("");
   //}}AFX_DATA_INIT
}

void TA_Reports::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TA_Reports)
   DDX_Check(pDX, IDC_TESTABILITY, m_testability);
   DDX_Text(pDX, IDC_TESTABILITY_FILE, m_testabilityFile);
   DDX_Check(pDX, IDC_TESTPROBE, m_testprobe);
   DDX_Text(pDX, IDC_TESTPROBE_FILE, m_testprobeFile);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TA_Reports, CDialog)
   //{{AFX_MSG_MAP(TA_Reports)
   ON_BN_CLICKED(IDC_CD_TESTABILITY, OnCdTestability)
   ON_BN_CLICKED(IDC_CD_TESTPROBE, OnCdTestprobe)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TA_Reports message handlers
void TA_Reports::OnCdTestability() 
{
   UpdateData();

   CFileDialog FileDialog(FALSE, "rep", m_testabilityFile,
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Testability Report (*.rep)|*.rep|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_testabilityFile = FileDialog.GetPathName();

   UpdateData(FALSE);
}

void TA_Reports::OnCdTestprobe() 
{
   UpdateData();

   CFileDialog FileDialog(FALSE, "rep", m_testprobeFile,
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Testability Report (*.rep)|*.rep|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   m_testprobeFile = FileDialog.GetPathName();

   UpdateData(FALSE);
}
