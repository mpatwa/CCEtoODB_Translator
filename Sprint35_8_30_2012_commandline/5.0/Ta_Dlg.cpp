
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ta_dlg.h"
#include "crypt.h"
#include "ccdoc.h"
#include "drc_util.h"
#include "net_util.h"
#include "pcbutil.h"
#include "ta.h"
#include "ta_short.h"
#include "drc.h"
#include "graph.h"
#include "gauge.h"
#include "measure.h"
#include "RwLib.h"
#include <math.h>
#include <float.h>
#include "CCEtoODB.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char *testaccesslayers[]; // from DBUTIL.CPP

extern int SHOWOUTLINE; // from TA.CPP
extern CString T_OUTLINE_TOP; // from TA.CPP
extern CString T_OUTLINE_BOT; // from TA.CPP

extern TA_OptionsStruct TAoptions; // from TA_OPTN.CPP

extern FILE *taLog; // from TA.CPP
extern int taDisplayError; // from TA.CPP

extern PinkooArray pinkooarray; // from TA.CPP
extern int pinkoocnt; // from TA.CPP

TA_NetArray taNetArray;
int taNetArrayCount;

extern ViakooArray viakooarray; // from TA.CPP
extern int viakoocnt; // from TA.CPP

extern DRC_ComponentArray DRCcomponentarray; // from DRC.cpp
extern int DRCcomponentcnt; // from DRC.cpp

extern DRC_PadstackArray DRCpadstackarray; // from DRC_UTIL.CPP
extern int DRCpadstackcnt; // from DRC_UTIL.CPP

extern DRC_AperturePadstackArray DRCaperturepadstackarray; // from DRC.cpp
extern int DRCaperturepadstackcnt;

extern DRC_PinkooArray DRCpinkooarray; // from DRC.cpp
extern int DRCpinkoocnt;

extern TA_PCBFileTestAccess1 TApcbfiletestaccess; // from TA_OPTN.CPP

extern void CloseDRCList();

static void check_dftoutlines(CCEtoODBDoc *doc);
static void load_pcbfile_ta(CCEtoODBDoc *doc, FileStruct *pcbfile);
static void update_TEST_comp_pin(CCEtoODBDoc *doc, CDataList *DataList);
static int TA_CheckOutlineData(CCEtoODBDoc *doc, CDataList *DataList, long graphic_class);
static void do_allvias_ta(FILE *wfp, CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
static void do_netlistpinloc_ta(FILE *fp, CCEtoODBDoc *doc, CNetList *NetList, double scale, CDataList *DataList, int unusednet);
static int GetTaNetIndex(CString netname);
static int get_padlayer_from_drcpadstack(int pi, int mirror);
static void TA_DeleteDRCAll(CCEtoODBDoc *doc, FileStruct *pcbfile);
static void TA_RunTestShortsAnalysis(CCEtoODBDoc *doc, FileStruct *pcbfile);
static void TAinit(CCEtoODBDoc *doc);

static PadstackKooArray padstackKooArray; // this gets all padstacks, vias instances used in derive_connectivity
static int padstackKooArrayCount;
                      
/**************************************************************************
* OnTestabilityAnalyzer
*/
void CCEtoODBDoc::OnTestabilityAnalyzer() 
{
   CString defaultFile = getApp().getSystemSettingsFilePath("default.dft");

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft) && 
       !getApp().getCamcadLicense().isLicensed(camcadProductAgilent5DxWrite)) // 5dx output hat the Probable short analysis
   {
      ErrorAccess("You do not have a License for Testability Analyzer!");
      return;
   }*/

   CWaitCursor wait;

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

   OnGeneratePinloc();

   TAinit(this);

   TestabilityAnalyzer dlg;
   dlg.doc = this;
   dlg.pcbFile = pcbFile;
   dlg.DoModal();
}

/*************************************************************************
*  TAinit
   LOAD PCB file (first visible) and init all array etc...
   delete LOG file
*/
static void TAinit(CCEtoODBDoc *doc)
{
   CString taLogFile = GetLogfilePath("ta.log");
   remove(taLogFile);

   taLog = fopen(taLogFile,"wt");
   if (!taLog)
   {
      CString err;
      err.Format("Error open [%s] file", taLogFile);
      ErrorMessage(err, "Error");
      return;
   }

   fclose(taLog);
}

/******************************************************************************
* init_global 
*/
static void init_global()
{
   drc_init();

   pinkooarray.SetSize(100,100);
   pinkoocnt = 0;
   
   taNetArray.SetSize(100,100);
   taNetArrayCount = 0;

   viakooarray.SetSize(100,100);
   viakoocnt = 0;
}

/******************************************************************************
* deinit_global
*/
static void deinit_global()
{
   int   i;

   drc_deinit();

   // here delete
   for (i=0;i<viakoocnt;i++)
   {
      delete viakooarray[i];
   }
   viakooarray.RemoveAll();
   viakoocnt = 0;
   // here delete
   for (i=0;i<pinkoocnt;i++)
   {
      delete pinkooarray[i];
   }
   pinkooarray.RemoveAll();
   pinkoocnt = 0;

   for (i=0;i<taNetArrayCount;i++)
   {
      delete taNetArray[i];
   }
   taNetArray.RemoveAll();
   taNetArrayCount = 0;
}

/******************************************************************************
* do_allpadstacks
*/
static void do_allpadstacks(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      // if it is a component, it is done in netlist
      // otherwise do not go into hierachies
      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      //if ((a = is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 0)) == NULL) 
      // continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();

      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {
         int pi = drc_get_aperturepadstackindex(block->getName());
         if (pi > -1)
         {
            // here update padstackKooArray 
            TA_PadstackKoo *padstack = new TA_PadstackKoo;
            padstackKooArray.SetAtGrow(padstackKooArrayCount++, padstack);  
            padstack->entitynumber = data->getEntityNumber();
            padstack->x = point2.x;
            padstack->y = point2.y;
            padstack->rot = rotation + data->getInsert()->getAngle();
            padstack->mirror = block_mirror;
            padstack->layer = 0;
            padstack->netname = NET_UNUSED_PINS;

            Attrib *attrib;
            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 0))   
               padstack->netname = get_attvalue_string(doc, attrib);

            if (DRCaperturepadstackarray[pi]->typ & 1)
               padstack->layer |= 1;
            if (DRCaperturepadstackarray[pi]->typ & 2)
               padstack->layer |= 2;

            padstack->aperturepadstackindex = pi;  // here also check for mirror
         }
         else
         {
            fprintf(taLog,"AperturePadstack [%s] not found in index\n",pi);
            taDisplayError++;
         }
      }
   }
}

/******************************************************************************
* TA_InitAccessAnalysis
   InitAccessAnalysis
   pincnt, test_probecnt
*/
static void TA_InitAccessAnalysis(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   CWaitCursor w;

   init_global();

   // here check if geoms contain DFT Outlines
   check_dftoutlines(doc);

   load_pcbfile_ta(doc, pcbfile);

   update_TEST_comp_pin(doc, &(pcbfile->getBlock()->getDataList()));

   // unused nets get their own $$ comp pin netname !
   TApcbfiletestaccess.multiportnets = 0;
   TApcbfiletestaccess.singleportnets = 0;

	int i=0;
   for (i=0;i<taNetArrayCount;i++)
   {
#ifdef _DEBUG
      TA_net *nn = taNetArray[i];
#endif
      if (taNetArray[i]->pincnt > 1)   
         TApcbfiletestaccess.multiportnets++;
      else
      if (taNetArray[i]->pincnt == 1)     // there can be nets without any pins !
         TApcbfiletestaccess.singleportnets++;
   }  

   // IF THE FILE WAS A GERBER file with derived padstacks and freepads, than no pins
   // where placed in the netlist
   if ((TApcbfiletestaccess.multiportnets + TApcbfiletestaccess.singleportnets) == 0)
   {
      // also here check freepads, testpoints, vias 
      for (i=0;i<viakoocnt;i++)
      {
         if (viakooarray[i]->entity_type == INSERTTYPE_FREEPAD)
         {
            int nindex = viakooarray[i]->netindex;
            if (nindex < 0)            continue;
            if (nindex < taNetArrayCount) 
               taNetArray[nindex]->pincnt++; 
         }
      }

      // here recalc nets on "gerber" import.
      TApcbfiletestaccess.multiportnets = 0;
      TApcbfiletestaccess.singleportnets = 0;
      for (i=0;i<taNetArrayCount;i++)
      {
#ifdef _DEBUG
         TA_net *nn = taNetArray[i];
#endif
         if (taNetArray[i]->pincnt > 1)   
            TApcbfiletestaccess.multiportnets++;
         else
         if (taNetArray[i]->pincnt == 1)     // there can be nets without any pins !
            TApcbfiletestaccess.singleportnets++;
      }  
   }

   TApcbfiletestaccess.total_nets = TApcbfiletestaccess.multiportnets + TApcbfiletestaccess.singleportnets;

   TApcbfiletestaccess.net_access_total_bottom = 0;
   TApcbfiletestaccess.net_access_total_top = 0;

   return;
}

/*************************************************************************
* DeInitAccessAnalysis
*/
static void TA_DeInitAccessAnalysis(CCEtoODBDoc *doc)
{
   CWaitCursor w;

   deinit_global();
}


/******************************************************************************
* check_dftoutlines
*/
static void check_dftoutlines(CCEtoODBDoc *doc)
{
   CString LogFile = GetLogfilePath("dftoutline.log");
   FILE  *taLog;
   int   taDisplayError = 0;

   // make DFT_OUTLINE_TOP
   // make DFT_OUTLINE_BOTTOM
   // mirror layer
   if ((taLog = fopen(LogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", LogFile);
      ErrorMessage(t, "Error");
   }

   // get outline
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      
      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
      {
         if (!TA_CheckOutlineData(doc, &(block->getDataList()), GR_CLASS_PACKAGEOUTLINE))
         {
            fprintf(taLog,"No DFT Outline for [%s] -> Use Tools|DFT Outline Generation if needed!\n",
               block->getName());
            taDisplayError++;
         }
      }
   }

   fclose(taLog);
   if (taDisplayError)
      Notepad(LogFile);
}

/******************************************************************************
* TA_CheckOutlineData
*/
static int TA_CheckOutlineData(CCEtoODBDoc *doc, CDataList *DataList, long graphic_class)
{
   int outline_found = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() == T_POLY && data->getGraphicClass() == graphic_class)
         outline_found++;
   }

   return outline_found;
}

/******************************************************************************
* load_pcbfile_ta
   This is used in Test Access Analysis only. The previous TEST_ACCESS attribute
   is ignored !

   These are the arrays, which are populated.

   DRCpadstackarray, DRCpadstackcnt
   DRCcomponentarray, DRCcomponentcnt
   viakooarray, viakoocnt
   pinkooarray, pinkoocnt

*/
static void load_pcbfile_ta(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   int startstacknum;
   int endstacknum;

   //fprintf(taLog,"PinAccessHeader, Comp, Pin, X, Y, Layer, Netname\n");
   //fprintf(taLog,"ViaAccessHeader, X, Y, Layer, Netname\n");

   generate_PINLOC(doc,pcbfile,0);  // this function generates the PINLOC argument for all pins.
   generate_PADSTACKACCESSFLAG(doc, 0);

   if (drc_check_layers(doc, &startstacknum, &endstacknum) < 0)
   {
      ErrorMessage("No Electrical Layers found!");
      return;
   }

   // DRCaperturepadstackarray, DRCaperturepadstackcnt
   drc_do_aperturepadstacks(doc);

   // DRCpadstackarray, DRCpadstackcnt
   drc_do_padstacks(doc, startstacknum, endstacknum);

   // DRCcomponentarray, DRCcomponentcnt
   drc_load_allcomps(doc, &(pcbfile->getBlock()->getDataList()), 
            0.0, 0.0, 0.0, pcbfile->isMirrored(), pcbfile->getScale(), 0, -1);

   // viakooarray, viakoocnt
   do_allvias_ta(taLog, doc, &(pcbfile->getBlock()->getDataList()), 0.0, 0.0, 0.0, pcbfile->isMirrored(), pcbfile->getScale(), 0, -1);

   ExplodeNcPins(doc, pcbfile);

   // pinkooarray, pinkoocnt
   do_netlistpinloc_ta(taLog, doc, &pcbfile->getNetList(), pcbfile->getScale(), 
         &(pcbfile->getBlock()->getDataList()), TAoptions.Unusedpins);

   // here update test net status NETSTATUS_NOPROBE
   int   i, p;

   for (i=0;i<taNetArrayCount;i++)
   {
#ifdef _DEBUG
   TA_net *c = taNetArray[i];
#endif
      if (taNetArray[i]->net_status ==  NETSTATUS_NOPROBE ||
          taNetArray[i]->probes_needed == 0)
      {
         // loop through pinkoocnt and viakoocnt
         for (p=0;p<pinkoocnt;p++)
         {
            if (pinkooarray[p]->netindex == i)
            {
               pinkooarray[p]->layer = 0;
               pinkooarray[p]->net_not_tested = TRUE;
            }
         }
         for (p=0;p<viakoocnt;p++)
         {
            if (viakooarray[p]->netindex == i)
            {
               viakooarray[p]->layer = 0;
               viakooarray[p]->net_not_tested = TRUE;
            }
         }
      }
   }
   
   // here update the test_connector status
   for (i=0; i<DRCcomponentcnt; i++)
   {
#ifdef _DEBUG
   DRC_component *c = DRCcomponentarray[i];
#endif
      if (DRCcomponentarray[i]->test_connector)
      {
         // loop through pinkoocnt and viakoocnt
         for (p=0;p<pinkoocnt;p++)
         {
            if (!pinkooarray[p]->compname.Compare(DRCcomponentarray[i]->compname))
            {
               pinkooarray[p]->connector = TRUE;
            }
         }
      }
   }

   // here update single feature pins
   for (i=0;i<taNetArrayCount;i++)
   {
#ifdef _DEBUG
   TA_net *c = taNetArray[i];
#endif
      int pincnt = taNetArray[i]->pincnt;
      int viacnt = taNetArray[i]->viacnt;

#ifdef OLD
      // loop through pinkoocnt and viakoocnt
      for (p=0;p<pinkoocnt;p++)
      {
         if (pinkooarray[p]->netindex == i)
            pincnt++;
      }
      for (p=0;p<viakoocnt;p++)
      {
         if (viakooarray[p]->netindex == i)
            viacnt++;
      }
#endif

      if (pincnt == 1)
      {
         // this is a single pin
         for (p=0;p<pinkoocnt;p++)
         {
            if (pinkooarray[p]->netindex == i)
               pinkooarray[p]->single_feature = TRUE;
         }
         for (p=0;p<viakoocnt;p++)
         {
            if (viakooarray[p]->netindex == i)
               viakooarray[p]->single_feature = TRUE;
         }
      }
   }
}

/******************************************************************************
* do_netlistpinloc_ta
   This is used in Test_acess anaylsis. the previous TEST_ACCESS is ignored
*/
static void do_netlistpinloc_ta(FILE *fp, CCEtoODBDoc *doc, CNetList *NetList, double scale, CDataList *DataList, int unusednet)
{
   int netindex;

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)
      {
         netindex = -1;
         if (!unusednet)
            continue; // do not read unused net pins
      }
      else
      {
         netindex = GetTaNetIndex(net->getNetName());

         Attrib *attrib;

         if (attrib = is_attvalue(doc, net->getAttributesRef(), ATT_TEST_NET_PROBES, 0))  
         {
            CString l = get_attvalue_string(doc, attrib);
            taNetArray[netindex]->probes_needed = atoi(l);
         }

         if (attrib = is_attvalue(doc, net->getAttributesRef(), ATT_TEST_NET_STATUS, 0))  
         {
            CString valueString = get_attvalue_string(doc, attrib);

            if (!valueString.CompareNoCase("No Probes"))
            {
               taNetArray[netindex]->net_status = NETSTATUS_NOPROBE;
               taNetArray[netindex]->probes_needed = 0;
            }

            else if (!valueString.CompareNoCase("Critical"))
               taNetArray[netindex]->net_status = NETSTATUS_CRITICAL;

            else
               taNetArray[netindex]->net_status = NETSTATUS_NORMAL;
         }        
      }

      int soldermasklayer = 0;
      // SaveAttribs(stream, &net->getAttributesRef());
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         if (compPin->getPinCoordinatesComplete())
         {
            CString padstackname;

            if (compPin->getPadstackBlockNumber() > -1)
            {
               BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
               if (block)
               {
                  padstackname = block->getName();
                  //if (block->getFlags() & BL_SOLDERMASK_TOP)    soldermasklayer |= 1;
                  //if (block->getFlags() & BL_SOLDERMASK_BOTTOM) soldermasklayer |= 2;

                  Attrib *attrib;
                  if (attrib = is_attvalue(doc, block->getAttributesRef(), ATT_SOLDERMASK, 0))  
                  {
                     CString valueString = get_attvalue_string(doc, attrib);

                     if (!valueString.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_ALL]))
                        soldermasklayer = 3;
                     else if (!valueString.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_BOT]))
                        soldermasklayer = 2;
                     else if (!valueString.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_TOP]))
                        soldermasklayer = 1;
                     else if (!valueString.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_NONE]))
                        soldermasklayer = 0;
                  }
               }
               else
               {
                  fprintf(taLog, "Can not find a padstack for Component [%s] Pin [%s]\n", compPin->getRefDes(), compPin->getPinName());
                  taDisplayError++;
                  continue;
               }
            }

            int pi = drc_get_padstackindex(padstackname);
            if (pi < 0) // no padstack defined
            {
               fprintf(taLog,"Padstack [%s] has no definition Comp [%s] Pin [%s] skipped\n", 
                  padstackname, compPin->getRefDes(), compPin->getPinName());
               taDisplayError++;
               continue;
            }

            // here update pinkooarray
            TA_pinkoo *c = new TA_pinkoo;
            pinkooarray.SetAtGrow(pinkoocnt,c);  
            pinkoocnt++;   

            c->outline_top_distance = DBL_MAX;     
            c->outline_bottom_distance = DBL_MAX;

            c->entitynumber = compPin->getEntityNumber();
            c->compname = compPin->getRefDes();
            c->pinname = compPin->getPinName();
            c->x = compPin->getOriginX() * scale;
            c->y = compPin->getOriginY() * scale;
            c->result = -1;   // marked as outside
            c->smd = DRCpadstackarray[pi]->smd; // is pin and SMD
            c->test = FALSE;  // has pin a test attribute.
            c->test_preference = FALSE;   // has pin a test attribute.
            c->probe_assigned = 0;
            c->featuresizebottom_deleted = 0;
            c->featuresizetop_deleted = 0;
            c->soldermaskbottom_deleted = 0;
            c->soldermasktop_deleted = 0;
            c->single_feature = 0;
            c->connector = 0;
            c->featureallowedviatop_deleted = 0;
            c->featureallowedviabottom_deleted = 0;
            c->featureallowedsmdtop_deleted = 0;
            c->featureallowedsmdbottom_deleted = 0;
            c->featureallowedthrutop_deleted = 0;
            c->featureallowedthrubottom_deleted = 0;
            c->featureallowedtesttop_deleted = 0;
            c->featureallowedtestbottom_deleted = 0;
            c->featureallowedconnectortop_deleted = 0;
            c->featureallowedconnectorbottom_deleted = 0;

            c->test_access = -1;          // default not assigned
            c->net_not_tested = 0;
            c->padstackindex = pi;
            c->mirror = (compPin->getMirror())?1:0;
            c->boardoutline = 0;
            if (netindex < 0) // unused net
            {
               CString nname = GenerateSinglePinNetname(compPin->getRefDes(), compPin->getPinName());
               int nindex = GetTaNetIndex(nname);
               taNetArray[nindex]->unused_net = TRUE;
               taNetArray[nindex]->pincnt++;
               c->netindex = nindex;
               c->single_feature = 1;
            }
            else
            {
               c->netindex = netindex;
               taNetArray[netindex]->pincnt++;
            }

            c->compoutline = 0;
            // here padstack layer as the base for access
            c->soldermask = 0;
            c->cadlayer = c->layer = get_padlayer_from_drcpadstack(pi, compPin->getMirror());
            if (compPin->getMirror())
            {
               if (soldermasklayer & 1)   c->soldermask |= 2;
               if (soldermasklayer & 2)   c->soldermask |= 1;
            }
            else
            {
               if (soldermasklayer & 1)   c->soldermask |= 1;
               if (soldermasklayer & 2)   c->soldermask |= 2;
            }

            double ax = 0;
            double ay = 0;
            double ax1, ay1;
            double rot = RadToDeg(compPin->getRotationRadians());

            Attrib *attrib;
            if (attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST_OFFSET_TOP_X, 0))  
            {
               CString l = get_attvalue_string(doc, attrib);
               ax = atof(l) * scale;
            }
            if (attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST_OFFSET_TOP_Y, 0))  
            {
               CString l = get_attvalue_string(doc, attrib);
               ay = atof(l) * scale;
            }

            if (c->mirror)
            {
               ax *= -1;
               ay *= -1;
            }

            // rotate
            Rotate(ax,ay,rot,&ax1,&ay1);   
            c->access_top_x = c->x + ax1;
            c->access_top_y = c->y + ay1;

            ax = 0;
            ay = 0;
            if (attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST_OFFSET_BOT_X, 0))  
            {
               CString l = get_attvalue_string(doc, attrib);
               ax = atof(l) * scale;
            }
            if (attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST_OFFSET_BOT_Y, 0))  
            {
               CString l = get_attvalue_string(doc, attrib);
               ay = atof(l) * scale;
            }

            if (c->mirror)
            {
               ax *= -1;
               ay *= -1;
            }

            // rotate
            Rotate(ax,ay,rot,&ax1,&ay1);   
            c->access_bottom_x = c->x + ax1;
            c->access_bottom_y = c->y + ay1;

            // test is a mandatory test point
            // this is a stringvalue or none
            // no value is ALL
            // TOP is TOP ACCESS
            // BOTTOM is BOTTOM ACCESS
            // NONE is no test
            if (attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_SOLDERMASK, 0))   
            {
               CString l = get_attvalue_string(doc, attrib);

               if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_ALL]))
               {
                  c->soldermask = 3;
               }
               else
               if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_BOT]))
               {
                  c->soldermask = 2;
               }
               else
               if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_TOP]))
               {
                  c->soldermask = 1;
               }
               else
               if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_NONE]))
               {
                  c->soldermask = 0;
               }
            }

            if (attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TESTPREFERENCE, 0))  
            {
               CString l = get_attvalue_string(doc, attrib);
               c->test_preference = atoi(l); // has pin a test preference value
            }

            if (attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST, 0))   
            {
               c->test = TRUE;
               CString l = get_attvalue_string(doc, attrib);
               if (!l.CompareNoCase("none"))
               {
                  // kill all
                  c->layer = 0;
                  c->test = FALSE;
               }
               else
               if (!l.CompareNoCase("bottom"))
               {
                  // mask layer all but top
                  c->layer = c->layer & ~0x1;
                  c->layer = c->layer & ~0x8;
               }
               else
               if (!l.CompareNoCase("top"))
               {
                  // mask layer all but bottom
                  c->layer = c->layer & ~0x2;
                  c->layer = c->layer & ~0x10;
               }
               else
               {
                  // both or unknown does not do nothing.
               }
            }
         }
      }
   }
}

/******************************************************************************
* get_padlayer_from_drcpadstack
*/
static int get_padlayer_from_drcpadstack(int pi, int mirror)
{
   int layer;

   if (pi < 0) return 3;

   if (mirror)
   {
      if (DRCpadstackarray[pi]->typ & 0x10)
      {
         // use only bottom
         layer = 2;
      }
      else if (DRCpadstackarray[pi]->typ == 1)
         layer = 2;
      else if (DRCpadstackarray[pi]->typ == 2)
         layer = 1;
      else
         layer = DRCpadstackarray[pi]->typ;  
   }
   else
   {
      if (DRCpadstackarray[pi]->typ & 0x8)
      {
         // use only top
         layer = 1;
      }
      else if (DRCpadstackarray[pi]->typ == 1)
         layer = 1;
      else if (DRCpadstackarray[pi]->typ == 2)
         layer = 2;
      else
         layer = DRCpadstackarray[pi]->typ;
   }

   return layer;
}

/******************************************************************************
* GetTaNetIndex
*/
static int GetTaNetIndex(CString netname)
{
   for (int i=0; i<taNetArrayCount; i++)
   {
      if (!netname.Compare(taNetArray[i]->netname))
         return i;
   }
      
   TA_net *taNet = new TA_net;
   taNetArray.SetAtGrow(taNetArrayCount++, taNet);  

   taNet->netname = netname;
   taNet->net_used = 0;
   taNet->unused_net = 0;     // from the $UNUSED nonconn pins
   taNet->net_status = 0;
   taNet->probes_needed = 1;  // default to 1
   taNet->probes_done = 0;
   taNet->test_probecnt = 0;
   taNet->pincnt = 0;
   taNet->viacnt = 0;

   return taNetArrayCount-1;
}

/******************************************************************************
* do_allvias_ta
   This is used in test access analysis. The previous TEST ACCESS attribute is ignored.
*/
static void do_allvias_ta(FILE *wfp, CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA && data->getInsert()->getInsertType() != INSERTTYPE_TESTPAD &&
            data->getInsert()->getInsertType() != INSERTTYPE_FREEPAD) // freepad is generated by Gerber->Padstack generation
         continue;

      int netindex = 0;
      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      Attrib *attrib;
      if ((attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 0)) == NULL)  continue;
      
      CString l = get_attvalue_string(doc, attrib);
      netindex = GetTaNetIndex(l);

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();

      CString rr = data->getInsert()->getRefname();
      if (!rr.CompareNoCase("TP32"))
      {
         int r = 0;
      }

      int pi = -1;
      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {
         pi = drc_get_padstackindex(block->getName());
      }
      else
      {
         pi = drc_get_padstackindex(block->getName());
         int r = 0;
      }

      // here update viakooarray
      TA_viakoo *c = new TA_viakoo;
      viakooarray.SetAtGrow(viakoocnt,c);  
      viakoocnt++;   

      c->outline_bottom_distance = DBL_MAX;
      c->outline_top_distance = DBL_MAX;

      c->entitynumber = data->getEntityNumber();
      c->entity_type = data->getInsert()->getInsertType();
      c->refname = data->getInsert()->getRefname();
      c->x = point2.x;
      c->y = point2.y;
      c->mirror = block_mirror;
      c->result = -1;         // marked as outside
      c->featuresizebottom_deleted = 0;
      c->featuresizetop_deleted = 0;
      c->soldermaskbottom_deleted = 0;
      c->soldermasktop_deleted = 0;
      c->net_not_tested = 0;
      c->soldermask = 0;            // 1 = top, 2 = bottom, 3 = all
      c->single_feature = 0;
      c->connector = 0;
      c->featureallowedviatop_deleted = 0;
      c->featureallowedviabottom_deleted = 0;
      c->featureallowedsmdtop_deleted = 0;
      c->featureallowedsmdbottom_deleted = 0;
      c->featureallowedthrutop_deleted = 0;
      c->featureallowedthrubottom_deleted = 0;
      c->featureallowedtesttop_deleted = 0;
      c->featureallowedtestbottom_deleted = 0;
      c->featureallowedconnectortop_deleted = 0;
      c->featureallowedconnectorbottom_deleted = 0;

      if (data->getInsert()->getMirrorFlags())
      {
         //if (block->getFlags() & BL_SOLDERMASK_TOP) c->soldermask |= 2;
         //if (block->getFlags() & BL_SOLDERMASK_BOTTOM) c->soldermask |= 1;
      }
      else
      {
         //if (block->getFlags() & BL_SOLDERMASK_TOP) c->soldermask |= 1;
         //if (block->getFlags() & BL_SOLDERMASK_BOTTOM) c->soldermask |= 2;
      }

      double ax = 0, ay = 0;

      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST_OFFSET_TOP_X, 0))  
      {
         CString l = get_attvalue_string(doc, attrib);
         ax = atof(l) * scale;
      }
      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST_OFFSET_TOP_Y, 0))  
      {
         CString l = get_attvalue_string(doc, attrib);
         ay = atof(l) * scale;
      }

      // rotate
      c->access_top_x = c->x + ax;
      c->access_top_y = c->y + ay;

      ax = 0;
      ay = 0;
      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST_OFFSET_BOT_X, 0))  
      {
         CString l = get_attvalue_string(doc, attrib);
         ax = atof(l) * scale;
      }
      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST_OFFSET_BOT_Y, 0))  
      {
         CString l = get_attvalue_string(doc, attrib);
         ay = atof(l) * scale;
      }

      // rotate
      c->access_bottom_x = c->x + ax;
      c->access_bottom_y = c->y + ay;

      c->padstackindex = pi;

      c->cadlayer = c->layer = get_padlayer_from_drcpadstack(pi, data->getInsert()->getMirrorFlags());

      if (pi < 0)
      {
         c->smd = 0;
      }
      else
      {
         c->smd = DRCpadstackarray[pi]->smd; // is pin and SMD
      }

      if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT) 
         c->test = TRUE;         // has pin a test attribute.
      else
         c->test = FALSE;        // has pin a test attribute.

      c->test_preference = 0;    // has pin a test attribute.
      c->test_access = -1;       // default not assigned
      c->probe_assigned = 0;
      c->netindex = netindex;
      c->compoutline = 0;
      c->boardoutline = 0;

      // test is a mandatory test point
      // this is a stringvalue or none
      // no value is ALL
      // TOP is TOP ACCESS
      // BOTTOM is BOTTOM ACCESS
      // NONE is no test

      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TESTPREFERENCE, 0))  
      {
         CString l = get_attvalue_string(doc, attrib);
         if (atoi(l) == 0)
            c->test_preference = 1;
         else
            c->test_preference = atoi(l); // has pin a test preference value
      }

      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 0))   
      {
         c->test = TRUE;
         CString l = get_attvalue_string(doc, attrib);

         if (!l.CompareNoCase("none"))
         {
            // kill all
            c->layer = 0;
            c->test = FALSE;
         }
         else
         if (!l.CompareNoCase("top"))
         {
            // mask layer all but bottom
            c->layer = c->layer & ~0x2;
         }
         else
         if (!l.CompareNoCase("bottom"))
         {
            // mask layer all but top
            c->layer = c->layer & ~0x1;
         }
         else
         {
            // both or unknown does not do nothing.
         }
      }

      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_SOLDERMASK, 0))   
      {
         CString l = get_attvalue_string(doc, attrib);

         if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_ALL]))
            c->soldermask = 3;
         else if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_BOT]))
            c->soldermask = 2;
         else if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_TOP]))
            c->soldermask = 1;
         else if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_NONE]))
            c->soldermask = 0;
      }

      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST_CONNECTOR, 0))  
         c->connector = TRUE;
      taNetArray[netindex]->viacnt++;
   }
}

/******************************************************************************
* update_TEST_comp_pin
*  - Here check if a ATT_TEST was applied to geom or comp and update the pinkooarray with it.
*/
static void update_TEST_comp_pin(CCEtoODBDoc *doc, CDataList *DataList)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && data->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = data->getInsert()->getMirrorFlags();

      CString compName = data->getInsert()->getRefname();
      if (!strlen(compName))
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      Attrib *attrib;
      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST_CONNECTOR, 0))  
      {
         // here loop through pinkooarray
         for (int i=0; i<pinkoocnt; i++)
         {
            // match component name
            if (!pinkooarray[i]->compname.Compare(compName))
               pinkooarray[i]->connector = TRUE;    
         }
      }

      // if a pcb component was converted to a testpoint
      if ((attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 0)) || data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)   
      {
         CString testValue = "";
         if (attrib)
            testValue = get_attvalue_string(doc, attrib);

         // here loop through pinkooarray
         for (int i=0; i<pinkoocnt; i++)
         {
            TA_pinkoo *pin = pinkooarray[i];

            // match component name
            if (pin->compname.Compare(compName))
               continue;
            
            pin->test = TRUE;

            if (!testValue.CompareNoCase("none"))
            {
               // kill all
               pin->layer = 0;
               pin->test = FALSE;
            }
            else if (!testValue.CompareNoCase("top"))
            {
               // mask layer all but bottom
               pin->layer = pin->layer & ~0x2;
               pin->layer = pin->layer & ~0x10;
               pin->test = TRUE;
            }
            else if (!testValue.CompareNoCase("bottom"))
            {
               // mask layer all but top
               pin->layer = pin->layer & ~0x1;
               pin->layer = pin->layer & ~0x8;
               pin->test = TRUE;
            }
            else
            {
               // both or unknown does nothing.
            }
         }
      }
   }
}

/*************************************************************************
* TA_DeleteDRCbyType
*/
static void TA_DeleteDRCbyType(CCEtoODBDoc *doc, FileStruct *pcbfile, int drctype)
{
   doc->UnselectAll(TRUE);

   POSITION pos = pcbfile->getDRCList().GetHeadPosition();
   while (pos != NULL)
   {
      DRCStruct *drc = pcbfile->getDRCList().GetNext(pos);
      if (drc->getAlgorithmType() == drctype)
         RemoveOneDRC(doc, drc, pcbfile);
   }
}

/******************************************************************************
* TAsave_defaultsettings
*/
void TAsave_defaultsettings(const char *fname, TA_OptionsStruct *defaultsetting, int pageunits)
{
   FILE     *fp;
   CString  uu;

   uu = GetUnitName(pageunits);

   if ((fp = fopen(fname,"wt")) == NULL)
   {
      CString  tmp;

      tmp.Format("Unable to open file [%s] for writing.\nMake sure it is not Read-Only.", fname);
      ErrorMessage(tmp, "Unable to Save Settings");
      return;
   }

   fprintf(fp,"; DFT setup file\n");
   fprintf(fp,";\n");
   fprintf(fp,".VERSION 2\n");
   fprintf(fp,";\n");
   fprintf(fp,"; units are INCH, MIL, MM\n");

   fprintf(fp,"; show outline from test access calculation\n");
   fprintf(fp,".SHOW_OUTLINE = %d\n", SHOWOUTLINE);
   fprintf(fp,".TOP_OUTLINE = %s\n", T_OUTLINE_TOP);
   fprintf(fp,".BOT_OUTLINE = %s\n", T_OUTLINE_BOT);

   fprintf(fp,"; Include Single Net Pins (NC Pins) 0 = Off 1 = On\n");
   fprintf(fp,".Unusedpins = %d \n", defaultsetting->Unusedpins);

   fprintf(fp,"; Maximum usable Probesize\n");
   fprintf(fp,".MaxProbeSize = %lg %s \n", defaultsetting->MaxProbeSize, uu);

   fprintf(fp,";\n");
   fprintf(fp,"; AccessAnalysis\n");
   fprintf(fp,";\n");
   fprintf(fp,"; SolderMask -> 0 = Off 1 = On\n");
   fprintf(fp,".AccessAnalysis soldermaskTop = %d \n", defaultsetting->AccessAnalysis.soldermaskTop);
   fprintf(fp,".AccessAnalysis soldermaskBot = %d \n", defaultsetting->AccessAnalysis.soldermaskBot);
   fprintf(fp,".AccessAnalysis testgraphicsize = %lg %s \n", defaultsetting->AccessAnalysis.testGraphicSize, uu);
   fprintf(fp,".AccessAnalysis netaccesstop = %d \n", defaultsetting->AccessAnalysis.allowNetAccessTop);
   fprintf(fp,".AccessAnalysis netaccessbottom = %d \n", defaultsetting->AccessAnalysis.allowNetAccessBot);
   fprintf(fp,"; PreferredtestSurface -> 0 = Top 1 = Bottom\n");
   fprintf(fp,".AccessAnalysis preferNetAccessSurface = %d \n", defaultsetting->AccessAnalysis.preferNetAccessSurface);
   fprintf(fp,".AccessAnalysis compOutlineDistanceTop = %lg %s \n", defaultsetting->AccessAnalysis.compOutlineDistanceTop, uu);
   fprintf(fp,".AccessAnalysis compOutlineDistanceBot = %lg %s \n", defaultsetting->AccessAnalysis.compOutlineDistanceBot, uu);
   fprintf(fp,".AccessAnalysis boardOutlineDistanceTop = %lg %s \n", defaultsetting->AccessAnalysis.boardOutlineDistanceTop, uu);
   fprintf(fp,".AccessAnalysis boardOutlineDistanceBot = %lg %s \n", defaultsetting->AccessAnalysis.boardOutlineDistanceBot, uu);
   fprintf(fp,".AccessAnalysis minfeaturesizetop = %lg %s \n", defaultsetting->AccessAnalysis.featureSizeMinTop, uu);
   fprintf(fp,".AccessAnalysis minfeaturesizebottom = %lg %s \n", defaultsetting->AccessAnalysis.featureSizeMinBot, uu);
   fprintf(fp,".AccessAnalysis boardFeatureBot = %d \n", defaultsetting->AccessAnalysis.boardFeatureBot);
   fprintf(fp,".AccessAnalysis boardFeatureTop = %d \n", defaultsetting->AccessAnalysis.boardFeatureTop);
   fprintf(fp,".AccessAnalysis compFeatureBot = %d \n", defaultsetting->AccessAnalysis.compFeatureBot);
   fprintf(fp,".AccessAnalysis compFeatureTop = %d \n", defaultsetting->AccessAnalysis.compFeatureTop);
   fprintf(fp,".AccessAnalysis featureSizeBot = %d \n", defaultsetting->AccessAnalysis.featureSizeBot);
   fprintf(fp,".AccessAnalysis featureSizeTop = %d \n", defaultsetting->AccessAnalysis.featureSizeTop);

   // Priority
   fprintf(fp,";\n");
   fprintf(fp,"; AccessAnalysis Priorities\n");
   fprintf(fp,".AccessAnalysis topTestattribPriority = %d \n", defaultsetting->AccessAnalysis.topTestattribPriority);
   fprintf(fp,".AccessAnalysis topConnectorPriority = %d \n", defaultsetting->AccessAnalysis.topConnectorPriority);
   fprintf(fp,".AccessAnalysis topThroughholePriority = %d \n", defaultsetting->AccessAnalysis.topThroughholePriority);
   fprintf(fp,".AccessAnalysis topSMDPriority = %d \n", defaultsetting->AccessAnalysis.topSMDPriority);
   fprintf(fp,".AccessAnalysis topViaPriority = %d \n", defaultsetting->AccessAnalysis.topViaPriority);

   fprintf(fp,".AccessAnalysis botTestattribPriority = %d \n", defaultsetting->AccessAnalysis.botTestattribPriority);
   fprintf(fp,".AccessAnalysis botConnectorPriority = %d \n", defaultsetting->AccessAnalysis.botConnectorPriority);
   fprintf(fp,".AccessAnalysis botThroughholePriority = %d \n", defaultsetting->AccessAnalysis.botThroughholePriority);
   fprintf(fp,".AccessAnalysis botSMDPriority = %d \n", defaultsetting->AccessAnalysis.botSMDPriority);
   fprintf(fp,".AccessAnalysis botViaPriority = %d \n", defaultsetting->AccessAnalysis.botViaPriority);
   
   // Multi feature net on/off
   fprintf(fp,";\n; AccessAnalysis Mulit Feature Net 0 = Off 1 = On\n");
   fprintf(fp,".AccessAnalysis MFNtopTestattrib = %d \n", defaultsetting->AccessAnalysis.MFNtopTestattrib);
   fprintf(fp,".AccessAnalysis MFNtopConnector = %d \n", defaultsetting->AccessAnalysis.MFNtopConnector);
   fprintf(fp,".AccessAnalysis MFNtopThroughhole = %d \n", defaultsetting->AccessAnalysis.MFNtopThroughhole);
   fprintf(fp,".AccessAnalysis MFNtopSMD = %d \n", defaultsetting->AccessAnalysis.MFNtopSMD);
   fprintf(fp,".AccessAnalysis MFNtopVia = %d \n", defaultsetting->AccessAnalysis.MFNtopVia);

   fprintf(fp,".AccessAnalysis MFNbotTestattrib = %d \n", defaultsetting->AccessAnalysis.MFNbotTestattrib);
   fprintf(fp,".AccessAnalysis MFNbotConnector = %d \n", defaultsetting->AccessAnalysis.MFNbotConnector);
   fprintf(fp,".AccessAnalysis MFNbotThroughhole = %d \n", defaultsetting->AccessAnalysis.MFNbotThroughhole);
   fprintf(fp,".AccessAnalysis MFNbotSMD = %d \n", defaultsetting->AccessAnalysis.MFNbotSMD);
   fprintf(fp,".AccessAnalysis MFNbotVia = %d \n", defaultsetting->AccessAnalysis.MFNbotVia);

   // Single Feature Net on/off
   fprintf(fp,";\n; AccessAnalysis Single Feature Net 0 = Off 1 = On\n");
   fprintf(fp,".AccessAnalysis SFNtopTestattrib = %d \n", defaultsetting->AccessAnalysis.SFNtopTestattrib);
   fprintf(fp,".AccessAnalysis SFNtopConnector = %d \n", defaultsetting->AccessAnalysis.SFNtopConnector);
   fprintf(fp,".AccessAnalysis SFNtopThroughhole = %d \n", defaultsetting->AccessAnalysis.SFNtopThroughhole);
   fprintf(fp,".AccessAnalysis SFNtopSMD = %d \n", defaultsetting->AccessAnalysis.SFNtopSMD);
   fprintf(fp,".AccessAnalysis SFNtopVia = %d \n", defaultsetting->AccessAnalysis.SFNtopVia);

   fprintf(fp,".AccessAnalysis SFNbotTestattrib = %d \n", defaultsetting->AccessAnalysis.SFNbotTestattrib);
   fprintf(fp,".AccessAnalysis SFNbotConnector = %d \n", defaultsetting->AccessAnalysis.SFNbotConnector);
   fprintf(fp,".AccessAnalysis SFNbotThroughhole = %d \n", defaultsetting->AccessAnalysis.SFNbotThroughhole);
   fprintf(fp,".AccessAnalysis SFNbotSMD = %d \n", defaultsetting->AccessAnalysis.SFNbotSMD);
   fprintf(fp,".AccessAnalysis SFNbotVia = %d \n", defaultsetting->AccessAnalysis.SFNbotVia);

   fprintf(fp,";\n");

   fprintf(fp,"; ProbableShorts\n;\n");
   fprintf(fp,".ProbableShorts pin_pin = %d\n", defaultsetting->ProbableShorts.pin_pin);
   fprintf(fp,".ProbableShorts pin_pinSize = %lg %s\n", defaultsetting->ProbableShorts.pin_pinSize, uu);
   fprintf(fp,";\n");
   fprintf(fp,".ProbableShorts pin_feature = %d\n", defaultsetting->ProbableShorts.pin_feature);
   fprintf(fp,".ProbableShorts pin_featureSize = %lg %s\n", defaultsetting->ProbableShorts.pin_featureSize, uu);
   fprintf(fp,";\n");
   fprintf(fp,".ProbableShorts feature_feature = %d\n", defaultsetting->ProbableShorts.feature_feature);
   fprintf(fp,".ProbableShorts feature_featureSize = %lg %s\n", defaultsetting->ProbableShorts.feature_featureSize, uu);
   fprintf(fp,";\n");

   fprintf(fp,"; ProbeAssignment\n;\n");
   fprintf(fp,".ProbeAssignment top = %d\n", defaultsetting->ProbeAssignment.top);
   fprintf(fp,".ProbeAssignment bottom = %d\n", defaultsetting->ProbeAssignment.bottom);
   fprintf(fp,";\n");

   fprintf(fp,";\n;end %s\n", fname);

   fclose(fp);
}


/////////////////////////////////////////////////////////////////////////////
// TestabilityAnalyzer dialog
TestabilityAnalyzer::TestabilityAnalyzer(CWnd* pParent /*=NULL*/)
   : CDialog(TestabilityAnalyzer::IDD, pParent)
{
   //{{AFX_DATA_INIT(TestabilityAnalyzer)
   //}}AFX_DATA_INIT
}

TestabilityAnalyzer::~TestabilityAnalyzer()
{

}

void TestabilityAnalyzer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TestabilityAnalyzer)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TestabilityAnalyzer, CDialog)
   //{{AFX_MSG_MAP(TestabilityAnalyzer)
   ON_BN_CLICKED(IDC_CLEAR_ACCESS, OnClearAccess)
   ON_BN_CLICKED(IDC_CLEAR_PROBE, OnClearProbe)
   ON_BN_CLICKED(IDC_CLEAR_SHORT, OnClearShort)
   ON_BN_CLICKED(IDC_SAVE, OnSave)
   ON_BN_CLICKED(IDC_LOAD, OnLoad)
   ON_BN_CLICKED(IDC_RUN_ACCESS, OnRunAccess)
   ON_BN_CLICKED(IDC_RUN_SHORT, OnRunShort)
   ON_BN_CLICKED(IDC_ATTRIB_INSTANCE, OnAttribInstance)
   ON_BN_CLICKED(IDC_MOVE_ACCESS, OnMoveAccess)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void TestabilityAnalyzer::OnMoveAccess() 
{
   // TODO: Add your control notification handler code here
   ErrorMessage("Move Access Probes","xxx");
}

void TestabilityAnalyzer::OnRunAccess() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft))  // do not centralize this, because for HP the central check is bypassed.
   {
      ErrorAccess("You do not have a License for Testability Analyzer!");
      return;
   }*/

   // pcb file is loaded !
   TA_InitAccessAnalysis(doc, pcbFile);

   TA_Options_AccessAnalysis dlg;
   dlg.doc = doc;
   dlg.pcbFile = pcbFile;
   dlg.m_totalNets.Format("%d", TApcbfiletestaccess.total_nets);

   if (TAoptions.Unusedpins)
      dlg.m_totalSFNets.Format("%d", TApcbfiletestaccess.singleportnets);
   else
      dlg.m_totalSFNets = "-";

   dlg.m_totalMFNets.Format("%d", TApcbfiletestaccess.multiportnets);

   dlg.m_botSoldermask =  TAoptions.AccessAnalysis.soldermaskBot;
   dlg.m_topSoldermask =  TAoptions.AccessAnalysis.soldermaskTop;
   dlg.m_topAllowAccess = TAoptions.AccessAnalysis.allowNetAccessTop;
   dlg.m_botAllowAccess = TAoptions.AccessAnalysis.allowNetAccessBot;
   dlg.m_prefer = TAoptions.AccessAnalysis.preferNetAccessSurface;
   dlg.m_topboardfeaturemin.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.AccessAnalysis.boardOutlineDistanceTop);
   dlg.m_botBoardFeatureMin.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.AccessAnalysis.boardOutlineDistanceBot);
   dlg.m_topcomponentfeaturemin.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.AccessAnalysis.compOutlineDistanceTop);
   dlg.m_botCompFeatureMin.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.AccessAnalysis.compOutlineDistanceBot);
   dlg.m_topfeaturesizemin.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.AccessAnalysis.featureSizeMinTop);
   dlg.m_botFeatureSizeMin.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.AccessAnalysis.featureSizeMinBot);

   dlg.m_through_pin_access = TAoptions.AccessAnalysis.throughPinAccess; // 0=non mount side, 1 = both
   dlg.m_excludeSinglePin = TAoptions.AccessAnalysis.excludeSinglePin;

   dlg.m_botBoardFeature = TAoptions.AccessAnalysis.boardFeatureBot;
   dlg.m_botCompFeature = TAoptions.AccessAnalysis.compFeatureBot;
   dlg.m_botFeatureSize = TAoptions.AccessAnalysis.featureSizeBot;
   dlg.m_topboardfeature = TAoptions.AccessAnalysis.boardFeatureTop;
   dlg.m_topcomponentfeature = TAoptions.AccessAnalysis.compFeatureTop;
   dlg.m_topfeaturesize = TAoptions.AccessAnalysis.featureSizeTop;


   dlg.topTestattribPriority = TAoptions.AccessAnalysis.topTestattribPriority;
   dlg.topConnectorPriority = TAoptions.AccessAnalysis.topConnectorPriority;
   dlg.topSMDPriority = TAoptions.AccessAnalysis.topSMDPriority;
   dlg.topThroughholePriority = TAoptions.AccessAnalysis.topThroughholePriority;
   dlg.topViaPriority = TAoptions.AccessAnalysis.topViaPriority;

   dlg.m_MFNtopTestattrib = TAoptions.AccessAnalysis.MFNtopTestattrib;
   dlg.m_MFNtopConnector = TAoptions.AccessAnalysis.MFNtopConnector;
   dlg.m_MFNtopSMD = TAoptions.AccessAnalysis.MFNtopSMD;
   dlg.m_MFNtopThroughhole = TAoptions.AccessAnalysis.MFNtopThroughhole;
   dlg.m_MFNtopVia = TAoptions.AccessAnalysis.MFNtopVia;

   dlg.m_SFNtopTestattrib = TAoptions.AccessAnalysis.SFNtopTestattrib;
   dlg.m_SFNtopConnector = TAoptions.AccessAnalysis.SFNtopConnector;
   dlg.m_SFNtopSMD = TAoptions.AccessAnalysis.SFNtopSMD;
   dlg.m_SFNtopThroughhole = TAoptions.AccessAnalysis.SFNtopThroughhole;
   dlg.m_SFNtopVia = TAoptions.AccessAnalysis.SFNtopVia;

   dlg.botTestattribPriority = TAoptions.AccessAnalysis.botTestattribPriority;
   dlg.botConnectorPriority = TAoptions.AccessAnalysis.botConnectorPriority;
   dlg.botSMDPriority = TAoptions.AccessAnalysis.botSMDPriority;
   dlg.botThroughholePriority = TAoptions.AccessAnalysis.botThroughholePriority;
   dlg.botViaPriority = TAoptions.AccessAnalysis.botViaPriority;
   
   dlg.m_MFNbotTestattrib = TAoptions.AccessAnalysis.MFNbotTestattrib;
   dlg.m_MFNbotConnector = TAoptions.AccessAnalysis.MFNbotConnector;
   dlg.m_MFNbotSMD = TAoptions.AccessAnalysis.MFNbotSMD;
   dlg.m_MFNbotThroughhole = TAoptions.AccessAnalysis.MFNbotThroughhole;
   dlg.m_MFNbotVia = TAoptions.AccessAnalysis.MFNbotVia;

   dlg.m_SFNbotTestattrib = TAoptions.AccessAnalysis.SFNbotTestattrib;
   dlg.m_SFNbotConnector = TAoptions.AccessAnalysis.SFNbotConnector;
   dlg.m_SFNbotSMD = TAoptions.AccessAnalysis.SFNbotSMD;
   dlg.m_SFNbotThroughhole = TAoptions.AccessAnalysis.SFNbotThroughhole;
   dlg.m_SFNbotVia = TAoptions.AccessAnalysis.SFNbotVia;

   dlg.DoModal();

   // here update settings, so that on save everything from the dialog is saved.
   TAoptions.AccessAnalysis.soldermaskBot = dlg.m_botSoldermask;
   TAoptions.AccessAnalysis.soldermaskTop = dlg.m_topSoldermask;
   TAoptions.AccessAnalysis.allowNetAccessTop = dlg.m_topAllowAccess;
   TAoptions.AccessAnalysis.allowNetAccessBot = dlg.m_botAllowAccess;
   TAoptions.AccessAnalysis.preferNetAccessSurface = dlg.m_prefer;
   TAoptions.AccessAnalysis.boardOutlineDistanceTop = atof(dlg.m_topboardfeaturemin);
   TAoptions.AccessAnalysis.boardOutlineDistanceBot = atof(dlg.m_botBoardFeatureMin);
   TAoptions.AccessAnalysis.compOutlineDistanceTop = atof(dlg.m_topcomponentfeaturemin);
   TAoptions.AccessAnalysis.compOutlineDistanceBot = atof(dlg.m_botCompFeatureMin);
   TAoptions.AccessAnalysis.featureSizeMinTop = atof(dlg.m_topfeaturesizemin);
   TAoptions.AccessAnalysis.featureSizeMinBot = atof(dlg.m_botFeatureSizeMin);

   TAoptions.AccessAnalysis.boardFeatureBot = dlg.m_botBoardFeature;
   TAoptions.AccessAnalysis.compFeatureBot = dlg.m_botCompFeature;
   TAoptions.AccessAnalysis.featureSizeBot = dlg.m_botFeatureSize;
   TAoptions.AccessAnalysis.boardFeatureTop = dlg.m_topboardfeature;
   TAoptions.AccessAnalysis.compFeatureTop = dlg.m_topcomponentfeature;
   TAoptions.AccessAnalysis.featureSizeTop = dlg.m_topfeaturesize;


   TAoptions.AccessAnalysis.topTestattribPriority = dlg.topTestattribPriority;
   TAoptions.AccessAnalysis.topConnectorPriority = dlg.topConnectorPriority;
   TAoptions.AccessAnalysis.topSMDPriority = dlg.topSMDPriority;
   TAoptions.AccessAnalysis.topThroughholePriority = dlg.topThroughholePriority;
   TAoptions.AccessAnalysis.topViaPriority = dlg.topViaPriority;

   TAoptions.AccessAnalysis.MFNtopConnector = dlg.m_MFNtopConnector;
   TAoptions.AccessAnalysis.MFNtopSMD = dlg.m_MFNtopSMD;
   TAoptions.AccessAnalysis.MFNtopTestattrib = dlg.m_MFNtopTestattrib;
   TAoptions.AccessAnalysis.MFNtopThroughhole = dlg.m_MFNtopThroughhole;
   TAoptions.AccessAnalysis.MFNtopVia = dlg.m_MFNtopVia;

   TAoptions.AccessAnalysis.SFNtopConnector = dlg.m_SFNtopConnector;
   TAoptions.AccessAnalysis.SFNtopSMD = dlg.m_SFNtopSMD;
   TAoptions.AccessAnalysis.SFNtopTestattrib = dlg.m_SFNtopTestattrib;
   TAoptions.AccessAnalysis.SFNtopThroughhole = dlg.m_SFNtopThroughhole;
   TAoptions.AccessAnalysis.SFNtopVia = dlg.m_SFNtopVia;

   TAoptions.AccessAnalysis.botTestattribPriority = dlg.botTestattribPriority;
   TAoptions.AccessAnalysis.botThroughholePriority = dlg.botThroughholePriority;
   TAoptions.AccessAnalysis.botConnectorPriority = dlg.botConnectorPriority;
   TAoptions.AccessAnalysis.botSMDPriority = dlg.botSMDPriority;
   TAoptions.AccessAnalysis.botViaPriority = dlg.botViaPriority;

   TAoptions.AccessAnalysis.MFNbotConnector = dlg.m_MFNbotConnector;
   TAoptions.AccessAnalysis.MFNbotSMD = dlg.m_MFNbotSMD;
   TAoptions.AccessAnalysis.MFNbotTestattrib = dlg.m_MFNbotTestattrib;
   TAoptions.AccessAnalysis.MFNbotThroughhole = dlg.m_MFNbotThroughhole;
   TAoptions.AccessAnalysis.MFNbotVia = dlg.m_MFNbotVia;

   TAoptions.AccessAnalysis.SFNbotConnector = dlg.m_SFNbotConnector;
   TAoptions.AccessAnalysis.SFNbotSMD = dlg.m_SFNbotSMD;
   TAoptions.AccessAnalysis.SFNbotTestattrib = dlg.m_SFNbotTestattrib;
   TAoptions.AccessAnalysis.SFNbotThroughhole = dlg.m_SFNbotThroughhole;
   TAoptions.AccessAnalysis.SFNbotVia = dlg.m_SFNbotVia;

   TAoptions.AccessAnalysis.throughPinAccess = dlg.m_through_pin_access; // 0=non mount side, 1 = both
   TAoptions.AccessAnalysis.excludeSinglePin = dlg.m_excludeSinglePin; // 0=non mount side, 1 = both


   TA_DeInitAccessAnalysis(doc);

   return;
}

void TestabilityAnalyzer::OnClearAccess() 
{
   TA_DeleteTestAccess(doc, pcbFile);
}

void TestabilityAnalyzer::OnClearProbe() 
{
   //TA_DeleteProbe(doc);
   ErrorMessage("Clear Probe ??");
}

void TestabilityAnalyzer::OnRunShort() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft) && !getApp().getCamcadLicense().isLicensed(camcadProductAgilent5DxWrite))  // 5dx output hat the Probable short analysis
   {
      ErrorAccess("You do not have a License for Testability Analyzer!");
      return;
   }*/

   TA_ProbableShorts dlg;
   dlg.m_pin_pin = TAoptions.ProbableShorts.pin_pin;
   dlg.m_pin_pinSize.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.ProbableShorts.pin_pinSize);
   dlg.m_pin_feature = TAoptions.ProbableShorts.pin_feature;
   dlg.m_pin_featureSize.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.ProbableShorts.pin_featureSize);
   dlg.m_feature_feature = TAoptions.ProbableShorts.feature_feature;
   dlg.m_feature_featureSize.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), TAoptions.ProbableShorts.feature_featureSize);

   if (dlg.DoModal() == IDOK)
   {
      TAoptions.ProbableShorts.pin_pin = dlg.m_pin_pin;
      TAoptions.ProbableShorts.pin_pinSize = atof(dlg.m_pin_pinSize);
      TAoptions.ProbableShorts.pin_feature = dlg.m_pin_feature;
      TAoptions.ProbableShorts.pin_featureSize = atof(dlg.m_pin_featureSize);
      TAoptions.ProbableShorts.feature_feature = dlg.m_feature_feature;
      TAoptions.ProbableShorts.feature_featureSize = atof(dlg.m_feature_featureSize);

      TA_DeleteDRCbyType(doc, pcbFile, DRC_ALG_PROBABLESHORT_PINtoPIN);
      TA_DeleteDRCbyType(doc, pcbFile, DRC_ALG_PROBABLESHORT_PINtoFEATURE);
      TA_DeleteDRCbyType(doc, pcbFile, DRC_ALG_PROBABLESHORT_FEATUREtoFEATURE);

      TA_RunTestShortsAnalysis(doc, pcbFile);
   }
}

void TestabilityAnalyzer::OnClearShort() 
{
   CloseDRCList();
   TA_DeleteDRCAll(doc, pcbFile);
}

void TestabilityAnalyzer::OnSave() 
{
   CString filename;
   CFileDialog FileDialog(FALSE, "dft", "*.dft",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "CAMCAD DFT Settings File (*.dft)|*.dft|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   filename = FileDialog.GetPathName();
   
   TAsave_defaultsettings(filename.GetBuffer(0), &TAoptions, doc->getSettings().getPageUnits());
}

void TestabilityAnalyzer::OnLoad() 
{
   CString filename;
   CFileDialog FileDialog(TRUE, "dft", "*.dft",
         OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
         "CAMCAD DFT Settings File (*.dft)|*.DFT|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   
   filename = FileDialog.GetPathName();
   TAload_defaultsettings(doc, filename, &TAoptions, doc->getSettings().getPageUnits());

   UpdateData(FALSE);
}

void TestabilityAnalyzer::OnAttribInstance() 
{

}


/******************************************************************************
* DeleteDRCData
*/
static void DeleteDRCData(CCEtoODBDoc *doc, CDataList *DataList)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_DRCMARKER)
         RemoveOneEntityFromDataList(doc, DataList, data);
   }
}

/*************************************************************************
* DeleteTestAccess
*/
static void TA_DeleteDRCAll(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   if (ErrorMessage("Warning - This function can't be undone, and the undo buffer will be cleared. Prior steps will be cleared and can't be undone.\nDo you wish to continue?"  , "Delete DRC Markers", MB_YESNO | MB_DEFBUTTON2)==IDYES)
   {
      doc->UnselectAll(TRUE);
      DeleteDRCData(doc, &(pcbfile->getBlock()->getDataList())); 

      pcbfile->getDRCList().empty();

      //POSITION pos = pcbfile->getDRCList().GetHeadPosition();
      //while (pos != NULL)
      //{
      //   DRCStruct *drc = pcbfile->getDRCList().GetNext(pos);
      //   if (drc->getAttributesRef())
      //   {
      //      delete drc->getAttributesRef();
      //      drc->getAttributesRef() = NULL;
      //   }
      //   delete drc;
      //}
      //pcbfile->getDRCList().RemoveAll();

      doc->UpdateAllViews(NULL);
   }
}

/******************************************************************************
* ta_get_largestaperturesize
*/
static double ta_get_largestaperturesize(CCEtoODBDoc *doc)
{
   double   l = 0;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      // create necessary aperture blocks
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         doc->validateBlockExtents(block);

         if (l < fabs(block->getXmin())) l = fabs(block->getXmin());
         if (l < fabs(block->getYmin())) l = fabs(block->getYmin());
         if (l < fabs(block->getXmax())) l = fabs(block->getXmax());
         if (l < fabs(block->getYmax())) l = fabs(block->getYmax());

      }
   }

   return l * 2;
}

/******************************************************************************
* DRC_RunTestShortsAnalysis
*/
static int DRC_RunTestShortsAnalysis(FILE *taLog, FileStruct *file, CCEtoODBDoc *doc)
{
   CWaitCursor w;
   double smalldelta = 1;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   int pinToPinErrorCnt = 0;
   int pinToFeatureErrorCnt = 0;
   int featureToFeatureErrorCnt = 0;

	int i=0;
   for (i=0;i<decimals;i++)
      smalldelta /= 10;

   drc_init();

   padstackKooArray.SetSize(100,100);
   padstackKooArrayCount = 0;

   /** here start drc ***/
/*
   if (drc_check_layers(doc, &startstacknum, &endstacknum) < 0)
      return -1;
*/
   drc_do_aperturepadstacks(doc);

// drc_do_padstacks(doc, startstacknum, endstacknum);
   double largestaperturesize = ta_get_largestaperturesize(doc);
   double maxPinSpacingForError = (largestaperturesize * 2) + TAoptions.ProbableShorts.pin_pinSize;

   if (DRCaperturepadstackcnt == 0)
   {
      MessageBox(NULL, "No Padstack was found", "Error", MB_ICONEXCLAMATION | MB_OK);
      return -2;
   }

   generate_PINLOC(doc, file,0); // this function generates the PINLOC argument for all pins.

   drc_load_allcomps(doc, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, file->isMirrored(), file->getScale(), 0, -1);

   // now collect all component pins from netlist
   drc_do_aperturenetlistpinloc(taLog, doc, &file->getNetList(), file->getScale(), &(file->getBlock()->getDataList()));
         
   // collect all padstacks, vias, testpoint etc... (all but component pins) from Design file.
   do_allpadstacks(doc, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, file->isMirrored(), file->getScale(), 0, -1);

   // here is all loaded.

   // padstackKooArrayCount is vias, testpoints etc...
   // DRCpinkoocnt are all component pins
   // DRCpadstackcnt are all the padstack pointers

   int startstacknum;
   int endstacknum;

   if (drc_check_layers(doc, &startstacknum, &endstacknum) < 0)
   {
      ErrorMessage("No Electrical Layers found!");
      return -1;
   }

   // prepare DRC definitions
   CTime t;

   if (TAoptions.ProbableShorts.pin_pin)
   {  
      t = t.GetCurrentTime();
      fprintf(taLog,"Start Pin-Pin Short calculations at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

      CProgressDlg *gauge = new CProgressDlg;
      gauge->caption = "DRC Function";
      gauge->Create();
      gauge->SetStatus("Probable Pin-Pin Short Calculation");

      for (int p1=0; p1<DRCpinkoocnt; p1++)
      {
         gauge->SetPos(round(100.0 * p1 / DRCpinkoocnt));

         for (int p2=p1 + 1; p2<DRCpinkoocnt; p2++)
         {
            DRC_pinkoo* pinkoo1 = DRCpinkooarray[p1];
            DRC_pinkoo* pinkoo2 = DRCpinkooarray[p2];

            if (pinkoo1->aperturepadstackindex < 0)
               continue;
            if (pinkoo2->aperturepadstackindex < 0)
               continue;

            // if not unused net and netname is the same
            if (pinkoo1->netname.Compare(NET_UNUSED_PINS) &&! pinkoo1->netname.Compare(pinkoo2->netname))
               continue;

            double dx = (pinkoo2->x - pinkoo1->x);
            double dy = (pinkoo2->y - pinkoo1->y);

            if (fabs(dx) > maxPinSpacingForError ||
                fabs(dy) > maxPinSpacingForError    )
            {
               continue;
            }

            double dist; //sqrt(dx*dx + dy*dy);

            if ((pinkoo1->layer & 1) && (pinkoo2->layer & 1))
            {
               // check top
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;

               int num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_top;
               if (pinkoo2->mirror)
                  num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_bot;

               // Bug: At this point, num1 and num2 contain the block numbers for the top/bottom pads in the DRCaperturepadstackarray.
               // Unfortunately the insert information for the pads are not contained in the DRCaperturepadstackarray elements.
               // The result is that pads inserted with nonzero x, y, and rotation values have incorrect polygons generated
               // for them later during the MeasureApertureToAperture() call.
               // 20030731 - knv
               
               if (num1 > -1 && num2 > -1)   
               {
                  BlockStruct *b1 = doc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = doc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  dist = MeasureApertureToAperture(doc, 
                        b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
                        b2, pinkoo2->x, pinkoo2->y, pinkoo2->rotation, 0,
                        &result1, &result2);


                  if (dist > 0 && dist < TAoptions.ProbableShorts.pin_pinSize)
                  {
                     //double x = pinkoo1->x + dx / 2;
                     //double y = pinkoo1->y + dy / 2;
                     double x = (result1.x + result2.x)/2.;
                     double y = (result1.y + result2.y)/2.;
                     // here put a marker just for testing

                     CString  comment;
                     comment.Format("Top %1.*lf", decimals, dist);

                     // dist is between p1 and p2
                     //drc_probable_short_pin_pin(doc, file,  "DRC_PIN_PIN_TOP", comment, x, y, LAYTYPE_SIGNAL_TOP,
                     // startstacknum, TAoptions.ProbableShorts.pin_pinSize, dist,
                     // pinkoo1->entitynumber, pinkoo1->x, pinkoo1->y,
                     // pinkoo2->entitynumber, pinkoo2->x, pinkoo2->y);
                     drc_probable_short_pin_pin(doc, file,  "DRC_PIN_PIN_TOP", comment, x, y, LAYTYPE_SIGNAL_TOP,
                        startstacknum, TAoptions.ProbableShorts.pin_pinSize, dist,
                        pinkoo1->entitynumber, result1.x, result1.y,
                        pinkoo2->entitynumber, result2.x, result2.y);

                     pinToPinErrorCnt++;
                  }
               } // check top
            } // both exist on top

            if ((pinkoo1->layer & 2) && (pinkoo2->layer & 2))
            {
               // check bottom
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;

               int num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_bot;
               if (pinkoo2->mirror)
                  num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_top;


               if (num1 > -1 && num2 > -1)   
               {
                  //if (num1 > -1
                  BlockStruct *b1 = doc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = doc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  dist = MeasureApertureToAperture(doc, 
                        b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
                        b2, pinkoo2->x, pinkoo2->y, pinkoo2->rotation, 0,
                        &result1, &result2);

                  if (dist > 0 && dist < TAoptions.ProbableShorts.pin_pinSize)
                  {
                     //double x = pinkoo1->x + dx/2;
                     //double y = pinkoo1->y + dy/2;
                     double x = (result1.x + result2.x)/2.;
                     double y = (result1.y + result2.y)/2.;
                     // here put a marker just for testing

                     CString  comment;
                     comment.Format("Bottom %1.*lf", decimals, dist);

                     // dist is between p1 and p2
                     //drc_probable_short_pin_pin(doc, file,  "DRC_PIN_PIN_BOT", comment, x, y, LAYTYPE_SIGNAL_BOT,
                     // endstacknum, TAoptions.ProbableShorts.pin_pinSize, dist,
                     // pinkoo1->entitynumber, pinkoo1->x, pinkoo1->y,
                     // pinkoo2->entitynumber, pinkoo2->x, pinkoo2->y);
                     drc_probable_short_pin_pin(doc, file,  "DRC_PIN_PIN_BOT", comment, x, y, LAYTYPE_SIGNAL_BOT,
                        endstacknum, TAoptions.ProbableShorts.pin_pinSize, dist,
                        pinkoo1->entitynumber, result1.x, result1.y,
                        pinkoo2->entitynumber, result2.x, result2.y);

                     pinToPinErrorCnt++;
                  }
               } // check bottom
            } // both on bottom
         }
      }

      // deinit gauge
      delete gauge;

      t = t.GetCurrentTime();
      fprintf(taLog, "End Pin-Pin Short calculations at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   }

   if (TAoptions.ProbableShorts.pin_feature)
   {
      t = t.GetCurrentTime();
      fprintf(taLog, "Start Pin-Feature Short calculations at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

      CProgressDlg *gauge = new CProgressDlg;
      gauge->caption = "DRC Function";
      gauge->Create();
      gauge->SetStatus("Probable Pin-Feature Short Calculation");

      for (int p1=0; p1<DRCpinkoocnt; p1++)
      {
         gauge->SetPos(round(100.0*p1/DRCpinkoocnt));

         for (int p2=0; p2<padstackKooArrayCount; p2++)
         {
            DRC_pinkoo* pinkoo1 = DRCpinkooarray[p1];
            DRC_pinkoo* pinkoo2 = DRCpinkooarray[p2];

            if (pinkoo1->aperturepadstackindex < 0)
               continue;
            if (padstackKooArray[p2]-> aperturepadstackindex < 0)
               continue;

            double dx = (padstackKooArray[p2]->x - pinkoo1->x);
            double dy = (padstackKooArray[p2]->y - pinkoo1->y);

            if (fabs(dx) > (largestaperturesize + 2 * TAoptions.ProbableShorts.pin_featureSize))
               continue;
            if (fabs(dy) > (largestaperturesize + 2 * TAoptions.ProbableShorts.pin_featureSize))
               continue;

            if (pinkoo1->netname.Compare(NET_UNUSED_PINS) && !pinkoo1->netname.Compare(padstackKooArray[p2]->netname))
               continue;

            double dist; //sqrt(dx*dx + dy*dy);

            if ((pinkoo1->layer & 1) && (padstackKooArray[p2]->layer & 1))
            {
               // check top
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;

               if (num1 > -1 && num2 > -1)   
               {
                  //if (num1 > -1
                  BlockStruct *b1 = doc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = doc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  dist = MeasureApertureToAperture(doc, 
                        b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);
               
                  if (dist > 0 && dist < TAoptions.ProbableShorts.pin_featureSize)
                  {
                     double x = pinkoo1->x + dx/2;
                     double y = pinkoo1->y + dy/2;

                     CString  comment;
                     comment.Format("Top %1.*lf", decimals, dist);

                     drc_probable_short_pin_feature(doc, file,  "DRC_PIN_FEATURE_TOP", comment, x, y,
                        LAYTYPE_SIGNAL_TOP, startstacknum, TAoptions.ProbableShorts.pin_featureSize, dist,
                        pinkoo1->entitynumber, pinkoo1->x, pinkoo1->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     pinToFeatureErrorCnt++;
                  }  // if dist is smaller
               } // if both exist
            } // if both exist top


            if ((pinkoo1->layer & 2) && (padstackKooArray[p2]->layer & 2))
            {
               // check top
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;

               if (num1 > -1 && num2 > -1)   
               {
                  //if (num1 > -1
                  BlockStruct *b1 = doc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = doc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  dist = MeasureApertureToAperture(doc, 
                        b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);
         
                  if (dist > 0 && dist < TAoptions.ProbableShorts.pin_featureSize)
                  {
                     double x = pinkoo1->x + dx/2;
                     double y = pinkoo1->y + dy/2;

                     CString  comment;
                     comment.Format("Bottom %1.*lf", decimals, dist);

                     drc_probable_short_pin_feature(doc, file,  "DRC_PIN_FEATURE_BOT", comment, x, y, 
                        LAYTYPE_SIGNAL_BOT, endstacknum, TAoptions.ProbableShorts.pin_featureSize, dist,
                        pinkoo1->entitynumber, pinkoo1->x, pinkoo1->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     pinToFeatureErrorCnt++;
                  }  // if dist is smaller
               } // if both exist
            } // if both exist bottom
         }
      }

      // deinit gauge
      delete gauge;

      t = t.GetCurrentTime();
      fprintf(taLog, "End Pin-Via Short calculations at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   }

   if (TAoptions.ProbableShorts.feature_feature)
   {

      t = t.GetCurrentTime();
      fprintf(taLog, "Start Feature-Feature Short calculations at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

      CProgressDlg *gauge = new CProgressDlg;
      gauge->caption = "DRC Function";
      gauge->Create();
      gauge->SetStatus("Probable Feature-Feature Short Calculation");

      for (int p1=0; p1<padstackKooArrayCount; p1++)
      {
         gauge->SetPos(round(100.0*p1/padstackKooArrayCount));

         for (int p2=p1 + 1; p2<padstackKooArrayCount; p2++)
         {
            if (padstackKooArray[p1]->aperturepadstackindex < 0)
               continue;
            if (padstackKooArray[p2]-> aperturepadstackindex < 0)
               continue;

            double dx = (padstackKooArray[p2]->x - padstackKooArray[p1]->x);
            double dy = (padstackKooArray[p2]->y - padstackKooArray[p1]->y);

            if (fabs(dx) > (largestaperturesize + 2*TAoptions.ProbableShorts.feature_featureSize))
               continue;
            if (fabs(dy) > (largestaperturesize + 2*TAoptions.ProbableShorts.feature_featureSize))
               continue;

            if (padstackKooArray[p1]->netname.Compare(NET_UNUSED_PINS) &&
               !padstackKooArray[p1]->netname.Compare(padstackKooArray[p2]->netname))  continue;

            double dist; //sqrt(dx*dx + dy*dy);

            if ((padstackKooArray[p1]->layer & 1) && (padstackKooArray[p2]->layer & 1))
            {
               // check top
               int num1 = DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_top;
               if (padstackKooArray[p1]->mirror)
                  num1 = DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_bot;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;

               if (num1 > -1 && num2 > -1)   
               {
                  //if (num1 > -1
                  BlockStruct *b1 = doc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = doc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  dist = MeasureApertureToAperture(doc, 
                        b1, padstackKooArray[p1]->x, padstackKooArray[p1]->y, 0.0, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);

                  if (dist > 0 && dist < TAoptions.ProbableShorts.feature_featureSize)
                  {
                     double x = padstackKooArray[p1]->x + dx/2;
                     double y = padstackKooArray[p1]->y + dy/2;

                     CString  comment;
                     comment.Format("Top %1.*lf", decimals, dist);

                     drc_probable_short_feature_feature(doc, file, "DRC_FEATURE_FEATURE_TOP", comment, x, y,
                        LAYTYPE_SIGNAL_TOP, startstacknum, TAoptions.ProbableShorts.feature_featureSize, dist,
                        padstackKooArray[p1]->entitynumber, padstackKooArray[p1]->x, padstackKooArray[p1]->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     featureToFeatureErrorCnt++;
                  }  // if dist is smaller
               } // if both exist
            } // if both exist top


            if ((padstackKooArray[p1]->layer & 2) && (padstackKooArray[p2]->layer & 2))
            {
               // check top
               int num1 =  DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_bot;
               if (padstackKooArray[p1]->mirror)
                  num1 = DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_top;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;


               if (num1 > -1 && num2 > -1)   
               {
                  //if (num1 > -1
                  BlockStruct *b1 = doc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = doc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  dist = MeasureApertureToAperture(doc, 
                        b1, padstackKooArray[p1]->x, padstackKooArray[p1]->y, 0.0, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);

                  if (dist > 0 && dist < TAoptions.ProbableShorts.feature_featureSize)
                  {
                     double x = padstackKooArray[p1]->x + dx/2;
                     double y = padstackKooArray[p1]->y + dy/2;

                     CString  comment;
                     comment.Format("Bottom %1.*lf", decimals, dist);

                     drc_probable_short_feature_feature(doc, file, "DRC_FEATURE_FEATURE_BOT", comment, x, y,
                        LAYTYPE_SIGNAL_BOT, endstacknum, TAoptions.ProbableShorts.feature_featureSize, dist,
                        padstackKooArray[p1]->entitynumber, padstackKooArray[p1]->x, padstackKooArray[p1]->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     featureToFeatureErrorCnt++;
                  }  // if dist is smaller
               } // if both exist
            } // if both exist bottom
         }
      }

      // deinit gauge
      delete gauge;

      t = t.GetCurrentTime();
      fprintf(taLog, "End Via-Via Short calculations at %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   }

   // here set DRC??? layer attributes
   LayerStruct *l;
   
   if ((l = doc->FindLayer_by_Name("DRC_PIN_PIN_BOT")) != NULL)
      l->setLayerType(LAYTYPE_BOTTOM);
   if ((l = doc->FindLayer_by_Name("DRC_PIN_PIN_TOP")) != NULL)
      l->setLayerType(LAYTYPE_TOP);
   if ((l = doc->FindLayer_by_Name("DRC_PIN_FEATURE_BOT")) != NULL)
      l->setLayerType(LAYTYPE_BOTTOM);
   if ((l = doc->FindLayer_by_Name("DRC_PIN_FEATURE_TOP")) != NULL)
      l->setLayerType(LAYTYPE_TOP);
   if ((l = doc->FindLayer_by_Name("DRC_FEATURE_FEATURE_BOT")) != NULL)
      l->setLayerType(LAYTYPE_BOTTOM);
   if ((l = doc->FindLayer_by_Name("DRC_FEATURE_FEATURE_TOP")) != NULL)
      l->setLayerType(LAYTYPE_TOP);

#ifdef DEBUG_GRAPHIC
// just for testing
   doc->PrepareAddEntity(pcbfile);  

   for (i=0; i<padstackKooArrayCount; i++)
   {
      if (padstackKooArray[i]->aperturepadstackindex > -1)
      {
         int num = DRCaperturepadstackarray[padstackKooArray[i]->aperturepadstackindex]->aperture_num_top;
         if (padstackKooArray[i]->mirror)
            num = DRCaperturepadstackarray[padstackKooArray[i]->aperturepadstackindex]->aperture_num_bot;

         if (num > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(num);
            int lay = Graph_Level("PSTTOP", "", 0);
            DataStruct *data = Graph_Block_Reference(block->name, NULL, 0, 
               padstackKooArray[i]->x, padstackKooArray[i]->y , padstackKooArray[i]->rot, 0, 1.0, lay, TRUE);
         }

         // here reverse top/bottom
         num = DRCaperturepadstackarray[padstackKooArray[i]->aperturepadstackindex]->aperture_num_bot;
         if (padstackKooArray[i]->mirror)
            num = DRCaperturepadstackarray[padstackKooArray[i]->aperturepadstackindex]->aperture_num_top;

         if (num > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(num);
            int lay = Graph_Level("PSTBOT", "", 0);
            DataStruct *data = Graph_Block_Reference(block->name, NULL, 0, 
               padstackKooArray[i]->x, padstackKooArray[i]->y , padstackKooArray[i]->rot, 0, 1.0, lay, TRUE);
         }
      }
   }

   for (i=0; i<DRCpinkoocnt; i++)
   {
      if (DRCpinkooarray[i]->aperturepadstackindex > -1)
      {
         if (DRCpinkooarray[i]->layer & 1)
         {
            int num = DRCaperturepadstackarray[DRCpinkooarray[i]->aperturepadstackindex]->aperture_num_top;
            if (DRCpinkooarray[i]->mirror)
               num = DRCaperturepadstackarray[DRCpinkooarray[i]->aperturepadstackindex]->aperture_num_bot;

            if (num > -1)
            {
               BlockStruct *block = doc->Find_Block_by_Num(num);
               int lay = Graph_Level("DRCTOP", "", 0);
               DataStruct *data = Graph_Block_Reference(block->name, NULL, 0, 
                  DRCpinkooarray[i]->x, DRCpinkooarray[i]->y , DRCpinkooarray[i]->rotation, 0, 1.0, lay, TRUE);
            }
         }

         if (DRCpinkooarray[i]->layer & 2)
         {
            int num = DRCaperturepadstackarray[DRCpinkooarray[i]->aperturepadstackindex]->aperture_num_bot;
            if (DRCpinkooarray[i]->mirror)
               num = DRCaperturepadstackarray[DRCpinkooarray[i]->aperturepadstackindex]->aperture_num_top;

            if (num > -1)
            {
               BlockStruct *block = doc->Find_Block_by_Num(num);
               int lay = Graph_Level("DRCBOT", "", 0);
               DataStruct *data = Graph_Block_Reference(block->name, NULL, 0, 
                  DRCpinkooarray[i]->x, DRCpinkooarray[i]->y , DRCpinkooarray[i]->rotation, 0, 1.0, lay, TRUE);
            }
         }
      }

   }
#endif

   drc_deinit();

   for (i=0; i<padstackKooArrayCount; i++)
      delete padstackKooArray[i];
   padstackKooArray.RemoveAll();

   if (pinToPinErrorCnt == 0 && pinToFeatureErrorCnt == 0 && featureToFeatureErrorCnt == 0)
   {
      formatMessageBox(MB_ICONEXCLAMATION,"No Errors Encountered!");
   }
   else
   {
      formatMessageBox(MB_ICONINFORMATION,"%d pinToPin, %d pinToFeature, and %d featureToFeature errors.",
         pinToPinErrorCnt,pinToFeatureErrorCnt,featureToFeatureErrorCnt);
   }

   return 1;
}

/*************************************************************************
* TA_RunTestShortsAnalysis
*/
static void TA_RunTestShortsAnalysis(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   int   res;

   doc->PrepareAddEntity(pcbfile);     

   CString taLogFile = GetLogfilePath("ta.log");

   if ((taLog = fopen(taLogFile,"a+t")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", taLogFile);
      ErrorMessage(t, "Error");
   }
   else
   {
      fprintf(taLog, "REMARK Shorts Analysis\nREMARK ===============\n");

      if (( res = DRC_RunTestShortsAnalysis(taLog, pcbfile, doc)) < 0)
      {
         CString  t;
         t.Format("Error [%d] in ShortAnalysis", res);
         ErrorMessage(t, "Error");
      }

      fclose(taLog);
   }

   doc->UpdateAllViews(NULL);
}
