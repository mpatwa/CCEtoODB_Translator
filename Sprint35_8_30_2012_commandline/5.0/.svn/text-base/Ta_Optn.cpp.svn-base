
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ta_optn.h"
#include "ccdoc.h"
#include "ta.h"
#include "drc_util.h"
#include "graph.h"
#include "pcbutil.h"
#include "outline.h"
#include "polylib.h"
#include "xform.h"
#include "measure.h"
#include "drc.h"
#include "lyrmanip.h"
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern PinkooArray pinkooarray; // from TA.CPP
extern int pinkoocnt; // from TA.CPP

extern ViakooArray viakooarray; // from TA.CPP
extern int viakoocnt; // from TA.CPP

extern TA_NetArray taNetArray; // from TA_DLG.CPP
extern int taNetArrayCount; // from TA_DLG.CPP

extern DRC_ComponentArray DRCcomponentarray; // from DRC.cpp
extern int DRCcomponentcnt; // from DRC.cpp

extern DRC_PadstackArray DRCpadstackarray; // from DRC_UTIL.CPP
extern int DRCpadstackcnt; // from DRC_UTIL.CPP

TA_PCBFileTestAccess1 TApcbfiletestaccess;
extern int SHOWOUTLINE; // from TA.CPP
extern CString T_OUTLINE_TOP; // from TA.CPP
extern CString T_OUTLINE_BOT; // from TA.CPP

extern FILE *taLog; // from TA.CPP
extern int taDisplayError; // from TA.CPP

TA_OptionsStruct TAoptions;               

static void TA_DeleteTestGraphic(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList);
static void calc_featuresize(CCEtoODBDoc *doc, int layer);
static void calc_physical(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer, int output_units_accuracy);
static void calc_netaccess(int layer);
static void calc_portnets(CCEtoODBDoc *doc, FileStruct *file, int *multi, int *single);
static void calc_feature_accessible(int layer);
static void calc_soldermask(int layer);
static void calc_boardoutlinedistance(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer, int output_units_accuracy);
static void calc_compoutlinedistance(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer);
static double get_padsize(int padstackindex, int mirror, int bottom);
static void TA_GetALLCOMPONENTData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int layer);
static int TA_GetOutlineData(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, long graphic_class, int layertype);
static int eval_compoutline(CCEtoODBDoc *doc, CPntList *pntList, int mirror, double extandSize, const char *blockname);
static void get_boardoutline(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer, int output_units_accuracy);
static int eval_boardoutline(CCEtoODBDoc *doc, CPntList *pntList, double shrinkSize, int bottom, int output_units_accuracy);
static void TA_DebugReport(const char *filename, CCEtoODBDoc *doc);

/******************************************************************************
* TA_process_access_analysis_form
*/
static void TA_process_access_analysis_form(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   int   i;

   // first update the prefered layer
   if (!TAoptions.AccessAnalysis.allowNetAccessTop && !TAoptions.AccessAnalysis.allowNetAccessBot ) 
   {
      ErrorMessage("You must \"Allow\" Top Surface NetAccess or Bottom Surface NetAccess!\nNo information processed.", 
         "Error Net Access Selection");
      return;
   }

   if (!TAoptions.AccessAnalysis.allowNetAccessTop && TAoptions.AccessAnalysis.preferNetAccessSurface == 0 ) 
   {
      ErrorMessage("You selected Top Surface NetAccess \"Prefered\" but not \"Allowed\"!\nNo information processed.", 
            "Error Net Access Selection");
      return;
   }

   if (!TAoptions.AccessAnalysis.allowNetAccessBot && TAoptions.AccessAnalysis.preferNetAccessSurface == 1 ) 
   {
      ErrorMessage("You selected Bottom Surface NetAccess \"Prefered\" but not \"Allowed\"!\nNo information processed.", 
            "Error Net Access Selection");
      return;
   }

   // init all layers
   for (i=0; i<pinkoocnt; i++)
   {
      TA_pinkoo *pinkoo = pinkooarray[i];
      pinkoo->net_access = pinkoo->layer; // do not use cadlayer, because layer has ATT_TEST
   }
   
   for (i=0; i<viakoocnt; i++)
   {
      TA_viakoo *viakoo = viakooarray[i];
      viakoo->net_access = viakoo->layer; // do not use cadlayer, because layer has ATT_TEST
   }

   if (!TAoptions.AccessAnalysis.allowNetAccessBot)
   {
      for (i=0; i<pinkoocnt; i++)
      {
         TA_pinkoo *pinkoo = pinkooarray[i];
         pinkoo->net_access = pinkoo->layer & 1;   // unmask 2
      }
      for (i=0; i<viakoocnt; i++)
      {
         TA_viakoo *viakoo = viakooarray[i];
         viakoo->net_access = viakoo->layer & 1;   // unmask 2
      }
   }

   if (!TAoptions.AccessAnalysis.allowNetAccessTop)
   {
      for (i=0; i<pinkoocnt; i++)
      {
         TA_pinkoo *pinkoo = pinkooarray[i];
         pinkoo->net_access = pinkoo->layer & 2;   // unmask 1
      }

      for (i=0; i<viakoocnt; i++)
      {
         TA_viakoo *viakoo = viakooarray[i];
         viakoo->net_access = viakoo->layer & 2;   // unmask 1
      }
   }


   // remove mount side access to throughhole pins
   if (TAoptions.AccessAnalysis.throughPinAccess == 0) // 0 - non mount side only
   {
      for (i=0; i<DRCcomponentcnt; i++) // loop comps
      {
         DRC_component *comp = DRCcomponentarray[i];

         if (TAoptions.AccessAnalysis.excludeSinglePin) // count pins and exclude single pin components
         {
            int pinCount = 0;

            for (int p=0; p<pinkoocnt; p++)
            {
               TA_pinkoo *pinkoo = pinkooarray[p];

               if (pinkoo->smd)  
                  continue;

               if (!pinkoo->compname.Compare(comp->compname))
               {
                  pinCount++;
                  if (pinCount > 1)
                     break;
               }
            }

            if (pinCount <= 1)
               continue;
         }

         // loop throughhole pins and remove mount side access
         for (int p=0; p<pinkoocnt; p++)
         {
            TA_pinkoo *pinkoo = pinkooarray[p];

            if (pinkoo->smd)  
               continue;

            if (!pinkoo->compname.Compare(comp->compname))
            {
               if (comp->bottom_placed)
                  pinkoo->net_access = pinkoo->net_access & 1; // unmask 2
               else
                  pinkoo->net_access = pinkoo->net_access & 2; // unmask 1
            }
         }
      }
   }

   // clear previous access flag. This is only for where a padstack surface exist, not 
   // any other filter
   for (i=0; i<taNetArrayCount; i++)
   {
      TA_net *netlist = taNetArray[i];

      netlist->padstack_bottom_surface_access = 0;
      netlist->padstack_top_surface_access = 0;
      netlist->physical_top_surface_access = 0;
      netlist->physical_bottom_surface_access = 0;
      netlist->feature_top_surface_access = 0;
      netlist->feature_bottom_surface_access = 0;
      netlist->option_top_surface_access = 0;
      netlist->option_bottom_surface_access = 0;
      netlist->net_used = 0;  // 1 top 2 bottom
   }


   TApcbfiletestaccess.net_access_total_bottom = 0;
   TApcbfiletestaccess.net_access_total_top = 0;
   TApcbfiletestaccess.net_access_physical_bottom = 0;
   TApcbfiletestaccess.net_access_physical_top = 0;
   TApcbfiletestaccess.net_access_feature_bottom = 0;
   TApcbfiletestaccess.net_access_feature_top = 0;

   if (SHOWOUTLINE) // delete component outline
   {
      TA_DeleteTestGraphic(doc, pcbFile, &(pcbFile->getBlock()->getDataList())); 

      doc->PrepareAddEntity(pcbFile);  
   }

   CWaitCursor w;

   int output_units_accuracy = GetDecimals(doc->getSettings().getPageUnits()); 

   // init physical for the current layer to be evaluated.
   for (i=0;i<pinkoocnt;i++)
   {
      TA_pinkoo *pinkoo = pinkooarray[i];
      DRC_Padstack *c1 = DRCpadstackarray[pinkoo->padstackindex];

      pinkoo->physical_accessible = pinkoo->net_access;  
   }

   for (i=0;i<viakoocnt;i++)
      viakooarray[i]->physical_accessible = viakooarray[i]->net_access;


   // do Preferred surface then other surface
   if (TAoptions.AccessAnalysis.preferNetAccessSurface == 0) // Top is Prefered
   {
      calc_netaccess(1);
      calc_physical(doc, pcbFile, 1, output_units_accuracy);   // this modifies 
      calc_feature_accessible(1);   // this also modifies physical_accessible, - if features are switched off

      // now do it for not prefered
      calc_netaccess(2);
      calc_physical(doc, pcbFile, 2, output_units_accuracy);   // this modifies 
      calc_feature_accessible(2);   // this also modifies physical_accessible, - if features are switched off
   }
   else // Bottom is Preferred
   {
      calc_netaccess(2);
      calc_physical(doc, pcbFile, 2, output_units_accuracy);   // this modifies 
      calc_feature_accessible(2);   // this also modifies physical_accessible, - if features are switched off

      // now do it for not prefered
      calc_netaccess(1);
      calc_physical(doc, pcbFile, 1, output_units_accuracy);   // this modifies 
      calc_feature_accessible(1);   // this also modifies physical_accessible, - if features are switched off
   }


   // final last line result !
   TApcbfiletestaccess.multiport_access_nets = 0;
   TApcbfiletestaccess.singleport_access_nets = 0;
   calc_portnets(doc, pcbFile, &TApcbfiletestaccess.multiport_access_nets, &TApcbfiletestaccess.singleport_access_nets);

   TApcbfiletestaccess.total_access_nets = TApcbfiletestaccess.multiport_access_nets + TApcbfiletestaccess.singleport_access_nets;

   // only if created
   if (doc->FindLayer_by_Name(T_OUTLINE_TOP))
   {
      Graph_Level(T_OUTLINE_TOP, "", 0);
      Graph_Level(T_OUTLINE_BOT, "", 0);
      Graph_Level_Mirror(T_OUTLINE_BOT, T_OUTLINE_TOP, "");
      
      LayerStruct *layer;
      if (layer = doc->FindLayer_by_Name(T_OUTLINE_BOT))
         layer->setLayerType(LAYTYPE_BOTTOM);
      if (layer = doc->FindLayer_by_Name(T_OUTLINE_TOP))
         layer->setLayerType(LAYTYPE_TOP);
   }
}

/******************************************************************************
* calc_feature_accessible
*/
static void calc_feature_accessible(int layer)
{
   // the last result is the total 
   // init all layers for physical 
	int i=0;
   for (i=0;i<pinkoocnt;i++)
   {
      TA_pinkoo *pin = pinkooarray[i];

      pin->feature_accessible = pin->physical_accessible;
   }
   
   for (i=0;i<viakoocnt;i++)
   {
      TA_viakoo *via = viakooarray[i];

      via->feature_accessible = via->physical_accessible;
   }

   for (i=0;i<pinkoocnt;i++)
   {
      TA_pinkoo *pin = pinkooarray[i];

      if (pin->test)
      {
         if (pin->single_feature)
         {
            if (!TAoptions.AccessAnalysis.SFNtopTestattrib)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedtesttop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.SFNbotTestattrib)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedtestbottom_deleted = TRUE;
            }
         }
         else
         {
            if (!TAoptions.AccessAnalysis.MFNtopTestattrib)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedtesttop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.MFNbotTestattrib)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedtestbottom_deleted = TRUE;
            }
         }
      }
      else if (pin->connector)
      {
         if (pin->single_feature)
         {
            if (!TAoptions.AccessAnalysis.SFNtopConnector)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedconnectortop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.SFNbotConnector)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedconnectorbottom_deleted = TRUE;
            }
         }
         else
         {
            if (!TAoptions.AccessAnalysis.MFNtopConnector)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedconnectortop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.MFNbotConnector)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedconnectorbottom_deleted = TRUE;
            }
         }
      }
      else if (pin->smd)
      {
         if (pin->single_feature)
         {
            if (!TAoptions.AccessAnalysis.SFNtopSMD)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedsmdtop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.SFNbotSMD)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedsmdbottom_deleted = TRUE;
            }
         }
         else
         {
            if (!TAoptions.AccessAnalysis.MFNtopSMD)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedsmdtop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.MFNbotSMD)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedsmdbottom_deleted = TRUE;
            }
         }

      }
      else
      {  
         if (pin->single_feature)
         {
            if (!TAoptions.AccessAnalysis.SFNtopThroughhole)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedthrutop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.SFNbotThroughhole)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedthrubottom_deleted = TRUE;
            }
         }
         else
         {
            if (!TAoptions.AccessAnalysis.MFNtopThroughhole)
            {
               pin->feature_accessible = pin->feature_accessible & 2;   
               pin->featureallowedthrutop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.MFNbotThroughhole)
            {
               pin->feature_accessible = pin->feature_accessible & 1;   
               pin->featureallowedthrubottom_deleted = TRUE;
            }
         }
      }
   }

   for (i=0; i<viakoocnt; i++)
   {
      TA_viakoo *via = viakooarray[i];

      if (via->test)
      {
         if (via->single_feature)
         {
            if (!TAoptions.AccessAnalysis.SFNtopTestattrib)
            {
               via->feature_accessible = via->feature_accessible & 2;
               via->featureallowedsmdbottom_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.SFNbotTestattrib)
            {
               via->feature_accessible = via->feature_accessible & 1;
               via->featureallowedsmdbottom_deleted = TRUE;
            }
         }
         else
         {
            if (!TAoptions.AccessAnalysis.MFNtopTestattrib)
            {
               via->feature_accessible = via->feature_accessible & 2;
               via->featureallowedsmdbottom_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.MFNbotTestattrib)
            {
               via->feature_accessible = via->feature_accessible & 1;
               via->featureallowedsmdbottom_deleted = TRUE;
            }
         }
      }
      else if (via->smd)
      {
         if (via->single_feature)
         {
            if (!TAoptions.AccessAnalysis.SFNtopSMD)
            {
               via->feature_accessible = via->feature_accessible & 2;
               via->featureallowedsmdbottom_deleted = TRUE;
            }
      
            if (!TAoptions.AccessAnalysis.SFNbotSMD)
            {
               via->feature_accessible = via->feature_accessible & 1;
               via->featureallowedsmdbottom_deleted = TRUE;
            }
         }
         else
         {
            if (!TAoptions.AccessAnalysis.MFNtopSMD)
            {
               via->feature_accessible = via->feature_accessible & 2;
               via->featureallowedsmdbottom_deleted = TRUE;
            }
      
            if (!TAoptions.AccessAnalysis.MFNbotSMD)
            {
               via->feature_accessible = via->feature_accessible & 1;
               via->featureallowedsmdbottom_deleted = TRUE;
            }
         }
      }
      else if (via->entity_type == INSERTTYPE_VIA)
      {
         if (via->single_feature)
         {
            if (!TAoptions.AccessAnalysis.SFNtopVia)
            {
               via->feature_accessible = via->feature_accessible & 2; // remove top accessibility
               via->featureallowedviatop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.SFNbotVia)
            {
               via->feature_accessible = via->feature_accessible & 1; // remove bottom accessibility
               via->featureallowedviabottom_deleted = TRUE;
            }
         }
         else
         {
            if (!TAoptions.AccessAnalysis.MFNtopVia)
            {
               via->feature_accessible = via->feature_accessible & 2; // remove top accessibility
               via->featureallowedviatop_deleted = TRUE;
            }

            if (!TAoptions.AccessAnalysis.MFNbotVia)
            {
               via->feature_accessible = via->feature_accessible & 1; // remove bottom accessibility
               via->featureallowedviabottom_deleted = TRUE;
            }
         }
      }
   }


   // process, which ones are feature accessable
   for (i=0; i<taNetArrayCount; i++)
   {
      TA_net *net = taNetArray[i];

      for (int pi = 0; pi < pinkoocnt; pi++)
      {
         TA_pinkoo *pin = pinkooarray[pi];

         if (pin->netindex != i)
            continue;

         if (layer == 1 && pin->feature_accessible & 1)
            net->feature_top_surface_access++;
         if (layer == 2 && pin->feature_accessible & 2)
            net->feature_bottom_surface_access++;
      }

      for (int vi = 0; vi < viakoocnt; vi++)
      {
         TA_viakoo *via = viakooarray[vi];

         if (via->netindex != i)
            continue;

         if (layer == 1 && via->feature_accessible & 1)
            net->feature_top_surface_access++;
         if (layer == 2 && via->feature_accessible & 2)
            net->feature_bottom_surface_access++;
      }
   }  

   for (i=0;i<taNetArrayCount;i++)
   {
      TA_net *net = taNetArray[i];

      if (layer == 1)
      {
         // do top first, than bottom
         if (!(net->net_used & 2) && net->feature_top_surface_access)
         {
            TApcbfiletestaccess.net_access_feature_top++;
            net->net_used |= 1;
         }
      }

      if (layer == 2)
      {
         if (!(net->net_used & 1) && net->feature_bottom_surface_access)
         {
            TApcbfiletestaccess.net_access_feature_bottom++;
            net->net_used |= 2;
         }
      }
   }
}

/******************************************************************************
* calc_soldermask
*/
static void calc_soldermask(int layer)
{
   if (layer == 1 && TAoptions.AccessAnalysis.soldermaskTop)
   {
		int i=0;
      for (i=0; i<pinkoocnt; i++)
      {
         TA_pinkoo *pin = pinkooarray[i];

         if (pin->soldermask & 1)
         {
            pin->soldermasktop_deleted = TRUE;
            pin->physical_accessible &= 2;
         }
      }

      for (i=0; i<viakoocnt; i++)
      {
         TA_viakoo *via = viakooarray[i];

         if (via->soldermask & 1)
         {
            via->soldermasktop_deleted = TRUE;
            via->physical_accessible &= 2;
         }
      }
   }

   if (layer == 2 && TAoptions.AccessAnalysis.soldermaskBot)
   {
		int i=0;
      for (i=0; i<pinkoocnt; i++)
      {
         TA_pinkoo *pin = pinkooarray[i];

         if (pin->soldermask & 2)
         {
            pin->soldermaskbottom_deleted = TRUE;
            pin->physical_accessible &= 1;
         }
      }

      for (i=0; i<viakoocnt; i++)
      {
         TA_viakoo *via = viakooarray[i];

         if (via->soldermask & 2)
         {
            via->soldermaskbottom_deleted = TRUE;
            via->physical_accessible &= 1;
         }
      }
   }
}

/******************************************************************************
* calc_featuresize
*/
static void calc_featuresize(CCEtoODBDoc *doc, int layer)
{
   int      i;
   double   smalldist = 1;

   int      dec = GetDecimals(doc->getSettings().getPageUnits());

   for (i=0;i<dec;i++)
      smalldist *= 10;

   if (layer == 1 && TAoptions.AccessAnalysis.featureSizeTop)
   {
      for (i=0;i<pinkoocnt;i++)
      {
#ifdef _DEBUG
         TA_pinkoo *c = pinkooarray[i];
         DRC_Padstack *c1 = DRCpadstackarray[pinkooarray[i]->padstackindex];
#endif
         if (pinkooarray[i]->net_access & 1)
         {
            double siz = get_padsize(pinkooarray[i]->padstackindex, pinkooarray[i]->mirror, 0);
            siz = round(siz * smalldist) / smalldist; // elimitate rounding errors
            if (siz < TAoptions.AccessAnalysis.featureSizeMinTop)
            {
               pinkooarray[i]->featuresizetop_deleted = TRUE;
               pinkooarray[i]->physical_accessible &= 2;
            }
         }
      }

      for (i=0;i<viakoocnt;i++)
      {
#ifdef _DEBUG
         TA_viakoo *c = viakooarray[i];
         DRC_Padstack *c1 = DRCpadstackarray[viakooarray[i]->padstackindex];
#endif

         if (viakooarray[i]->net_access & 1)
         {
            double siz = get_padsize(viakooarray[i]->padstackindex, viakooarray[i]->mirror, 0);
            siz = round(siz * smalldist) / smalldist; // elimitate rounding errors
            if (siz < TAoptions.AccessAnalysis.featureSizeMinTop)
            {
               viakooarray[i]->featuresizetop_deleted = TRUE;
               viakooarray[i]->physical_accessible &= 2;
            }
         }
      }
   }

   if (layer == 2 && TAoptions.AccessAnalysis.featureSizeBot)
   {
      for (i=0;i<pinkoocnt;i++)
      {
#ifdef _DEBUG
   TA_pinkoo *c = pinkooarray[i];
   DRC_Padstack *c1 = DRCpadstackarray[pinkooarray[i]->padstackindex];
#endif
         if (pinkooarray[i]->net_access & 2)
         {
            double siz = get_padsize(pinkooarray[i]->padstackindex, pinkooarray[i]->mirror, 1);

            siz = round(siz * smalldist) / smalldist; // elimitate rounding errors
            if (siz < TAoptions.AccessAnalysis.featureSizeMinBot)
            {

               pinkooarray[i]->featuresizebottom_deleted = TRUE;
               pinkooarray[i]->physical_accessible &= 1;
            }
         }
      }
      for (i=0;i<viakoocnt;i++)
      {
#ifdef _DEBUG
         TA_viakoo *c = viakooarray[i];
         DRC_Padstack *c1 = DRCpadstackarray[viakooarray[i]->padstackindex];
#endif

         if (viakooarray[i]->net_access & 2)
         {
            double siz = get_padsize(viakooarray[i]->padstackindex, viakooarray[i]->mirror, 1);  // <== bottom padsize
            siz = round(siz * smalldist) / smalldist; // elimitate rounding errors
            if (siz < TAoptions.AccessAnalysis.featureSizeMinBot)
            {
               viakooarray[i]->featuresizebottom_deleted = TRUE;
               viakooarray[i]->physical_accessible &= 1;
            }
         }
      }
   }
}

/******************************************************************************
* get_padsize
   mirror      == padstack is mirrored
   bottom      == looks for padstack om bottom or top

   typ;        // return 0x1 top
               //        0x2 bottom
               //        0x4 drill
               //        0x8 top only        - layer attribute
               //        0x10 bottom only    - layer attribute
*/
// this is not perfect yet !
static double get_padsize(int padstackindex, int mirror, int bottom)
{
   double siz;
   DRC_Padstack *c = DRCpadstackarray[padstackindex];

   if (mirror)
   {
      if (bottom)
      {
         if (c->typ & 0x10)   // MIRRORONLY <=== Added this check to fix TSR #3988
            siz = DRCpadstackarray[padstackindex]->sizebot;
         else
            siz = DRCpadstackarray[padstackindex]->sizetop;
      }
      else
      {
         if (c->typ & 0x8)    // NEVERMIRROR <=== Added this check to fix TSR #3988
            siz = DRCpadstackarray[padstackindex]->sizetop;
         else
            siz = DRCpadstackarray[padstackindex]->sizebot;
      }
   }
   else
   {
      if (bottom)
         siz = DRCpadstackarray[padstackindex]->sizebot;
      else
         siz = DRCpadstackarray[padstackindex]->sizetop;
   }
   return siz;
}

/******************************************************************************
* calc_physical
*/
static void calc_physical(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer, int output_units_accuracy) 
{
   int   i;

   calc_soldermask(layer);
   calc_boardoutlinedistance(doc, pcbfile, layer, output_units_accuracy);
   calc_compoutlinedistance(doc, pcbfile, layer);
   calc_featuresize(doc, layer);

   // process, which ones are physically accessable
   // here get the CAD layer, meaning which PADSTACK was accessable by definition.
   for (i=0;i<taNetArrayCount;i++)
   {
      for (int pi = 0; pi < pinkoocnt; pi++)
      {
         if (pinkooarray[pi]->netindex != i) continue;
         if (layer == 1 && pinkooarray[pi]->physical_accessible & 1)
            taNetArray[i]->physical_top_surface_access++;
         if (layer == 2 && pinkooarray[pi]->physical_accessible & 2)
            taNetArray[i]->physical_bottom_surface_access++;
      }

      for (int vi = 0; vi < viakoocnt; vi++)
      {
         if (viakooarray[vi]->netindex != i) continue;
         if (layer == 1 && viakooarray[vi]->physical_accessible & 1)
            taNetArray[i]->physical_top_surface_access++;
         if (layer == 2 && viakooarray[vi]->physical_accessible & 2)
            taNetArray[i]->physical_bottom_surface_access++;
      }
   }  


   for (i=0;i<taNetArrayCount;i++)
   {
      if (layer == 1)
      {
         if (!(taNetArray[i]->net_used & 2) && taNetArray[i]->physical_top_surface_access)
            TApcbfiletestaccess.net_access_physical_top++;
      }

      if (layer == 2)
      {
         if (!(taNetArray[i]->net_used & 1) && taNetArray[i]->physical_bottom_surface_access)
            TApcbfiletestaccess.net_access_physical_bottom++;
      }
   }
}

/******************************************************************************
* calc_compoutlinedistance
*/
static void calc_compoutlinedistance(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer)
{
   if (TAoptions.AccessAnalysis.compFeatureBot || TAoptions.AccessAnalysis.compFeatureTop)
   {
      TA_GetALLCOMPONENTData(doc, pcbfile, &(pcbfile->getBlock()->getDataList()), 
         0.0, 0.0, 0.0, pcbfile->isMirrored(), pcbfile->getScale(), 0, -1, layer);
   }
}

/******************************************************************************
* TA_GetALLCOMPONENTData
*/
// This function gets the access attribute by outline
// vias and components are tested in write_easy outline.
//
static void TA_GetALLCOMPONENTData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int layer)
{
   int         viacnt = 0;
   double      accuracy  = get_accuracy(doc);
   doc->PrepareAddEntity(file);     

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = data->getInsert()->getMirrorFlags();
      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT) 
      {

         double s_size;
         if (!block_mirror)   // top component
         {
            if (!TAoptions.AccessAnalysis.compFeatureTop)
               continue;

            if (layer == 2)
               continue;   
            s_size = TAoptions.AccessAnalysis.compOutlineDistanceTop;   // bottom component
         }
         else
         {
            if (!TAoptions.AccessAnalysis.compFeatureBot)
               continue;

            if (layer == 1)
               continue;

            s_size = TAoptions.AccessAnalysis.compOutlineDistanceBot;
         }

         Point2   point2;
         Mat2x2   m;

         CString compname = data->getInsert()->getRefname();

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot = rotation + data->getInsert()->getAngle();

         Attrib *a;
         if (a = is_attvalue(doc, data->getAttributesRef(), ATT_TEST_IGNORE_OUTLINE, 0))
         {
            continue;   // do not evaluate a component outline
         }
         if (a = is_attvalue(doc, data->getAttributesRef(), ATT_LOADED, 0))
         {
            CString l = "";
            l = get_attvalue_string(doc, a);
            
            if (l.CompareNoCase("TRUE"))
               continue;   // do not evaluate a not loaded component/ but footprints are there
         }

         RotMat2(&m, block_rot);

         if (mirror)
            block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = data->getInsert()->getOriginY() * scale;

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         // here is new outline stuff
         Outline_Start(doc);
         
         // outline algorithm
         // 
         int outline_found = 0;
         outline_found = TA_GetOutlineData(doc, &(block->getDataList()), 
               point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(), 
               0, -1, GR_CLASS_PACKAGEOUTLINE, 0);

         if (outline_found)
         { 
            int returnCode;
            CPntList *pp = Outline_GetOutline(&returnCode, accuracy);
            eval_compoutline(doc, pp, block_mirror, s_size , block->getName() );
         }

         Outline_FreeResults();
      }
   } 
}

/******************************************************************************
* TA_GetOutlineData 
*/
static int TA_GetOutlineData(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, long graphic_class, int layertype)
{
   Mat2x2   m;
   DataStruct *np;
   Point2   point2;
   int      layer;
   int      outline_found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_TEXT)
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         LayerStruct *ll = doc->FindLayer(layer);

         if (layertype && ll->getLayerType() != layertype) // only check for class if the layertype is not equal
         {
            // if layertype mentioned and layertype is not equal
            continue;
         }
         else if (graphic_class && (np->getGraphicClass() != graphic_class))
         {
            // if graphic_class mentioned and graphic class is not equal
            continue;
         }

         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         outline_found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            continue;

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         point2.x = np->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;

         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         double block_rot = rotation + np->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else 
            if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            outline_found += TA_GetOutlineData(doc, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, graphic_class, layertype);
         }
      }
   }

   return outline_found;
}

/******************************************************************************
* eval_compoutline
   if mirror of component, expandsize is bottom else top
*/
static int eval_compoutline(CCEtoODBDoc *doc, CPntList *pntList, int mirror, double extandSize, const char *blockname)
{
   if (!pntList)
      return 0;

   // shrink
   CPntList *newPntList = ShrinkPoly(pntList,doc->getPageUnits(), -extandSize, true);
   if (!newPntList)
   {
      // for debugging
      {
         static int debugCompOutlineCount = 0;
         debugCompOutlineCount++;

         int layerIndex = Graph_Level("DebugCompOutlines","",false);

         CString blockName;
         blockName.Format("DebugCompOutlines-%d",debugCompOutlineCount);
         BlockStruct* block = Graph_Block_On(GBO_APPEND,blockName,-1,0);
         DataStruct* polyStruct = Graph_PolyStruct(layerIndex,0,false);

         CPoly* poly = new CPoly(*pntList);
         polyStruct->getPolyList()->AddTail(poly);

         CString refName;
         refName.Format("DB%d",debugCompOutlineCount);

         Graph_Block_Off();

         Graph_Block_Reference(blockName,refName,1,0.,0.,0.,0,1.,-1,0);
      }

      fprintf(taLog, "Geom [%s] : Shrink Failed\n", blockname);
      taDisplayError++;
      return 0;
   }

   // find extents
   double minx, miny, maxx, maxy;
   minx = miny = DBL_MAX;
   maxx = maxy = -DBL_MAX;
   POSITION pos = newPntList->GetHeadPosition();
   while (pos)
   {
      CPnt *pnt = newPntList->GetNext(pos);

      if (pnt->x < minx) minx = pnt->x;
      if (pnt->x > maxx) maxx = pnt->x;
      if (pnt->y < miny) miny = pnt->y;
      if (pnt->y > maxy) maxy = pnt->y;
   }


   // check pins
	int i=0;
   for (i=0;i<pinkoocnt;i++)
   {
#ifdef _DEBUG
      TA_pinkoo *c = pinkooarray[i];
#endif
      if(mirror)
      {
         // test bottom layer, but if pin is not accessable, do not do it.
         if (!(pinkooarray[i]->physical_accessible & 2)) continue;
      }
      else
      {
         // test top layer, but if pin is not accessable, do not do it.
         if (!(pinkooarray[i]->physical_accessible & 1)) continue;
      }

      CPnt checkpoint;
      checkpoint.x = (DbUnit)pinkooarray[i]->x;
      checkpoint.y = (DbUnit)pinkooarray[i]->y;

      // box check for speed reasons if it is outside if the poly extens
      int outside_of_poly = FALSE;
      if (checkpoint.x < minx)   outside_of_poly = TRUE;
      if (checkpoint.y < miny)   outside_of_poly = TRUE;
      if (checkpoint.x > maxx)   outside_of_poly = TRUE;
      if (checkpoint.y > maxy)   outside_of_poly = TRUE;

      // for speed reasons, switch on check
      int   outline_tst = TRUE;
      if ((checkpoint.x + TAoptions.MaxProbeSize) < minx)   outline_tst = FALSE;
      if ((checkpoint.y + TAoptions.MaxProbeSize) < miny)   outline_tst = FALSE;
      if ((checkpoint.x - TAoptions.MaxProbeSize) > maxx)   outline_tst = FALSE;
      if ((checkpoint.y - TAoptions.MaxProbeSize) > maxy)   outline_tst = FALSE;

      // if result is not outside and pinkoo not marked as inside -> update
      // only if clearly inside.
      if (!outside_of_poly)
      {
         if (Is_Pnt_Inside_PntList(newPntList, &checkpoint,doc->getPageUnits()))
         {
            pinkooarray[i]->result = 0;
            if (mirror)
            {
               pinkooarray[i]->outline_bottom_distance = 0;
               pinkooarray[i]->compoutline |= 2;
               pinkooarray[i]->physical_accessible = pinkooarray[i]->physical_accessible & 1;   // unmask 2
            }
            else
            {
               pinkooarray[i]->outline_top_distance = 0;
               pinkooarray[i]->compoutline |= 1;
               pinkooarray[i]->physical_accessible = pinkooarray[i]->physical_accessible & 2;   // unmask 1
            }
         }
      }

      if (outline_tst)
      {
         DTransform  xform;
         Point2      accesspos;
         Point2      result;
         accesspos.x = checkpoint.x;
         accesspos.y = checkpoint.y;
         double dist = MeasurePointToPntList(&accesspos, newPntList, 0.0, &xform, TRUE, &result);
         if (mirror)
         {
            if (dist < pinkooarray[i]->outline_bottom_distance)
               pinkooarray[i]->outline_bottom_distance = dist;
         }
         else
         {
            if (dist < pinkooarray[i]->outline_top_distance)
               pinkooarray[i]->outline_top_distance = dist;
         }
      }
   }


   // check vias
   for (i=0;i<viakoocnt;i++)
   {
      if(mirror)
      {
         // test bottom layer, but if via is not accessable, do not do it.
         if (!(viakooarray[i]->physical_accessible & 2)) continue;
      }
      else
      {
         // test top layer, but if via is not accessable, do not do it.
         if (!(viakooarray[i]->physical_accessible & 1)) continue;
      }

      CPnt checkpoint;
      checkpoint.x = (DbUnit)viakooarray[i]->x;
      checkpoint.y = (DbUnit)viakooarray[i]->y;

      // box check for speed reasons if it is outside if the poly extens
      int outside_of_poly = FALSE;
      if (checkpoint.x < minx)   outside_of_poly = TRUE;
      if (checkpoint.y < miny)   outside_of_poly = TRUE;
      if (checkpoint.x > maxx)   outside_of_poly = TRUE;
      if (checkpoint.y > maxy)   outside_of_poly = TRUE;

      // for speed reasons, switch on check
      int   outline_tst = TRUE;
      if ((checkpoint.x + TAoptions.MaxProbeSize) < minx)   outline_tst = FALSE;
      if ((checkpoint.y + TAoptions.MaxProbeSize) < miny)   outline_tst = FALSE;
      if ((checkpoint.x - TAoptions.MaxProbeSize) > maxx)   outline_tst = FALSE;
      if ((checkpoint.y - TAoptions.MaxProbeSize) > maxy)   outline_tst = FALSE;

      // if result is not outside and pinkoo not marked as inside -> update
      // only if clearly inside.
      if (!outside_of_poly)
      {
         if (Is_Pnt_Inside_PntList(newPntList, &checkpoint,doc->getPageUnits()))
         {
            viakooarray[i]->result = 0;
            if (mirror)
            {
               viakooarray[i]->outline_bottom_distance = 0;
               viakooarray[i]->compoutline |= 2;
               viakooarray[i]->physical_accessible = viakooarray[i]->physical_accessible & 1;   // unmask 2
            }
            else
            {
               viakooarray[i]->outline_top_distance = 0;
               viakooarray[i]->compoutline |= 1;
               viakooarray[i]->physical_accessible = viakooarray[i]->physical_accessible & 2;   // unmask 1
            }
         }
      }  

      if (outline_tst)
      {
         DTransform  xform;
         Point2      accesspos;
         Point2      result;
         accesspos.x = checkpoint.x;
         accesspos.y = checkpoint.y;
         double dist = MeasurePointToPntList(&accesspos, newPntList, 0.0, &xform, TRUE, &result);
         if (mirror)
         {
            if (dist < viakooarray[i]->outline_bottom_distance)
               viakooarray[i]->outline_bottom_distance = dist;
         }
         else
         {
            if (dist < viakooarray[i]->outline_top_distance)
               viakooarray[i]->outline_top_distance = dist;
         }
      }
   }

   // graph results for component outline
   if (SHOWOUTLINE)
   {
      int   l;
      int   filled = TRUE;

      if (mirror)
         l = Graph_Level(T_OUTLINE_BOT,"",0);
      else
         l = Graph_Level(T_OUTLINE_TOP,"",0);

      DataStruct *d = Graph_PolyStruct(l, 0L, FALSE); // poly is always with 0

      Graph_Poly(NULL,0, filled, 0, 0);

      POSITION pos = newPntList->GetHeadPosition();
      while (pos)
      {
         CPnt *pnt = newPntList->GetNext(pos);
         Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
      }
   }


   // free
   FreePntList(newPntList);

   return 1;
}

/******************************************************************************
* calc_boardoutlinedistance
*/
static void calc_boardoutlinedistance(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer, int output_units_accuracy)
{
   if (layer == 2 && TAoptions.AccessAnalysis.boardFeatureBot)
      get_boardoutline(doc, pcbfile, layer, output_units_accuracy);
   if (layer == 1 && TAoptions.AccessAnalysis.boardFeatureTop)
      get_boardoutline(doc, pcbfile, layer, output_units_accuracy);
}

/******************************************************************************
* get_boardoutline
   layer 1 = top
   layer 2 = bottom
*/
static void get_boardoutline(CCEtoODBDoc *doc, FileStruct *pcbfile, int layer, int output_units_accuracy)
{
   double s_size;
   double accuracy = get_accuracy(doc);
   
   if (layer == 1)                                    
      s_size = TAoptions.AccessAnalysis.boardOutlineDistanceTop;  
   else
      s_size = TAoptions.AccessAnalysis.boardOutlineDistanceBot;

   Outline_Start(doc);
               
   int outline_found = TA_GetOutlineData(doc, &(pcbfile->getBlock()->getDataList()), 
         0.0, 0.0, 0.0, pcbfile->isMirrored(), pcbfile->getScale(), 0, -1, GR_CLASS_BOARDOUTLINE, 0);

   if (!outline_found)
   {
      outline_found = TA_GetOutlineData(doc, &(pcbfile->getBlock()->getDataList()), 
         0.0, 0.0, 0.0, pcbfile->isMirrored(), pcbfile->getScale(), 0, -1, 0, LAYTYPE_BOARD_OUTLINE);
   }

   if (!outline_found)
   {     
      fprintf(taLog,"No \"Primary Board Outline\" or a LayerType \"Board Outline\" found.\n");
      taDisplayError++;
   }
   else
   { 
      int returnCode;
      CPntList *pp = Outline_GetOutline(&returnCode, accuracy);
      eval_boardoutline(doc, pp, s_size, (layer == 2), output_units_accuracy);   // bottom is layer 2
   }

   Outline_FreeResults();
}

/******************************************************************************
* eval_boardoutline
*/
static int eval_boardoutline(CCEtoODBDoc *doc, CPntList *pntList, double shrinkSize, int bottom, int output_units_accuracy)
{
   if (!pntList)
      return 0;

   // shrink
   CPntList *newPntList = ShrinkPoly(pntList,doc->getPageUnits(), shrinkSize);
   if (!newPntList)
   {
      CString  tmp;

      tmp.Format("The selected Boardoutline Distance [%1.*lf] in %s Surface will not allow a testable area. This Value is ignored!",
         output_units_accuracy,  shrinkSize, (bottom)?"Bottom":"Top");

      ErrorMessage(tmp,"Board Outline Distance wrong!");
      return 0;
   }

   // check pins
	int i=0;
   for (i=0;i<pinkoocnt;i++)
   {
#ifdef _DEBUG
         TA_pinkoo *c = pinkooarray[i];
#endif
      if(bottom)
      {
         // test bottom layer, but if pin is not accessable, do not do it.
         if (!(pinkooarray[i]->physical_accessible & 2)) continue;
      }
      else
      {
         // test top layer, but if pin is not accessable, do not do it.
         if (!(pinkooarray[i]->physical_accessible & 1)) continue;
      }

      CPnt checkpoint;
      checkpoint.x = (DbUnit)pinkooarray[i]->x;
      checkpoint.y = (DbUnit)pinkooarray[i]->y;

      // if result is not outside and pinkoo not marked as inside -> update
      // only if clearly inside.
      if (!Is_Pnt_Inside_PntList(newPntList, &checkpoint,doc->getPageUnits()))
      {
         pinkooarray[i]->result = 0;
         if (bottom)
         {
            pinkooarray[i]->outline_bottom_distance = 0;
            pinkooarray[i]->boardoutline |= 2;
            pinkooarray[i]->physical_accessible = pinkooarray[i]->physical_accessible & 1;   // unmask 2
         }
         else
         {
            pinkooarray[i]->outline_top_distance = 0;
            pinkooarray[i]->boardoutline |= 1;
            pinkooarray[i]->physical_accessible = pinkooarray[i]->physical_accessible & 2;   // unmask 1
         }
      }

      if (TRUE)
      {
         DTransform  xform;
         Point2      accesspos;
         Point2      result;
         accesspos.x = checkpoint.x;
         accesspos.y = checkpoint.y;
         double dist = MeasurePointToPntList(&accesspos, newPntList, 0.0, &xform, TRUE, &result);
         if (bottom)
         {
            if (dist < pinkooarray[i]->outline_bottom_distance)
               pinkooarray[i]->outline_bottom_distance = dist;
         }
         else
         {
            if (dist < pinkooarray[i]->outline_top_distance)
               pinkooarray[i]->outline_top_distance = dist;
         }
      }

   }


   // check vias
   for (i=0;i<viakoocnt;i++)
   {
      if(bottom)
      {
         // test bottom layer, but if pin is not accessable, do not do it.
         if (!(viakooarray[i]->physical_accessible & 2)) continue;
      }
      else
      {
         // test top layer, but if pin is not accessable, do not do it.
         if (!(viakooarray[i]->physical_accessible & 1)) continue;
      }

      CPnt checkpoint;
      checkpoint.x = (DbUnit)viakooarray[i]->x;
      checkpoint.y = (DbUnit)viakooarray[i]->y;

      // if result is not outside and pinkoo not marked as inside -> update
      // only if clearly inside.
      if (!Is_Pnt_Inside_PntList(newPntList, &checkpoint,doc->getPageUnits()))
      {
         viakooarray[i]->result = 0;
         if (bottom)
         {
            viakooarray[i]->outline_bottom_distance = 0;
            viakooarray[i]->boardoutline |= 2;
            viakooarray[i]->physical_accessible = viakooarray[i]->physical_accessible & 1;   // unmask 2
         }
         else
         {
            viakooarray[i]->outline_top_distance = 0;
            viakooarray[i]->boardoutline |= 1;
            viakooarray[i]->physical_accessible = viakooarray[i]->physical_accessible & 2;   // unmask 1
         }
      }

      if (TRUE)
      {
         DTransform  xform;
         Point2      accesspos;
         Point2      result;
         accesspos.x = checkpoint.x;
         accesspos.y = checkpoint.y;
         double dist = MeasurePointToPntList(&accesspos, newPntList, 0.0, &xform, TRUE, &result);
         if (bottom)
         {
            if (dist < viakooarray[i]->outline_bottom_distance)
               viakooarray[i]->outline_bottom_distance = dist;
         }
         else
         {
            if (dist < viakooarray[i]->outline_top_distance)
               viakooarray[i]->outline_top_distance = dist;
         }
      }
   }


   // graph results for boardoutline
   if (SHOWOUTLINE)
   {
      int   l;
      int   filled = FALSE;

      if (bottom)
         l = Graph_Level(T_OUTLINE_BOT,"",0);
      else
         l = Graph_Level(T_OUTLINE_TOP,"",0);

      DataStruct *d = Graph_PolyStruct(l, 0L, FALSE); // poly is always with 0

      Graph_Poly(NULL, 0, filled, 0, 0);

      POSITION pos = newPntList->GetHeadPosition();
      while (pos)
      {
         CPnt *pnt = newPntList->GetNext(pos);
         Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
      }
   }

   // free
   FreePntList(newPntList);

   return 1;
}

/******************************************************************************
* calc_netaccess
*/
static void calc_netaccess(int layer)
{
   int   i;

/*
   // init physical for the current layer to be evaluated.
   for (i=0;i<pinkoocnt;i++)
      pinkooarray[i]->physical_accessible = pinkooarray[i]->net_access; 
   for (i=0;i<viakoocnt;i++)
      viakooarray[i]->physical_accessible = viakooarray[i]->net_access;
*/

   // here get the CAD layer, meaning which PADSTACK was accessible by definition.
   for (i=0; i<taNetArrayCount; i++)
   {
      TA_net *netlist = taNetArray[i];

      for (int pi=0; pi<pinkoocnt; pi++)
      {
         TA_pinkoo *pinkoo = pinkooarray[pi];

         if (pinkoo->netindex != i) 
            continue;

         if (layer == 1 && pinkoo->physical_accessible & 1)
            netlist->padstack_top_surface_access++;
         if (layer == 2 && pinkoo->physical_accessible & 2)
            netlist->padstack_bottom_surface_access++;
      }

      for (int vi=0; vi<viakoocnt; vi++)
      {
         TA_viakoo *viakoo = viakooarray[vi];

         if (viakoo->netindex != i) 
            continue;

         if (layer == 1 && viakoo->physical_accessible & 1)
            netlist->padstack_top_surface_access++;
         if (layer == 2 && viakoo->physical_accessible & 2)
            netlist->padstack_bottom_surface_access++;
      }
   }  

   // now the taNetArray has a total number of padstack_top/bottom_surface_access available
   for (i=0; i<taNetArrayCount; i++)
   {
      TA_net *netlist = taNetArray[i];

      // do top first, than bottom
      if (layer == 1)
      {
         if (!(netlist->net_used & 2) && netlist->padstack_top_surface_access)
            TApcbfiletestaccess.net_access_total_top++;
      }

      if (layer == 2)
      {
         if (!(netlist->net_used & 1) && netlist->padstack_bottom_surface_access)
            TApcbfiletestaccess.net_access_total_bottom++;
      }
   }
}

/******************************************************************************
* calc_portnets
*/
static void calc_portnets(CCEtoODBDoc *doc, FileStruct *file, int *multi, int *single)
{
   *multi = 0;
   *single = 0;   // this unused net is always index 0

   // here count the net features one more time.
	int i=0;
   for (i=0; i<taNetArrayCount; i++)
   {
      if (taNetArray[i]->feature_top_surface_access || taNetArray[i]->feature_bottom_surface_access)
      {
         TA_net *net = taNetArray[i];

         if (net->pincnt > 1) 
            *multi = *multi + 1;
         else if (net->pincnt == 1)    // there can be nets without any pins !
            *single = *single + 1;
      }
   }  

   doc->UnselectAll(TRUE);
   // first need to delete the old 
   POSITION drcPos = file->getDRCList().GetHeadPosition();
   while (drcPos)
   {
      DRCStruct *drc = file->getDRCList().GetNext(drcPos);

      if (drc->getAlgorithmType() == DFT_ALG_NET_WITHOUT_ACCESS)
         RemoveOneDRC(doc, drc, file);
   }

   // here update DRC table
   for (i=0; i<taNetArrayCount; i++)
   {
      TA_net *net = taNetArray[i];

      if (!net->feature_top_surface_access && !net->feature_bottom_surface_access)
      {
         if (net->pincnt > 1) 
         {
            drc_net_without_access(doc, file, net->netname);
         }
         else
         {
            //drc_net_without_access(doc, file, taNetArray[i]->netname)

            //int algIndex = GetAlgorithmNameIndex(doc, "Net Without Access");
            
            //DRCStruct *drc = add_drc(file, DRC_CODE_NetwithoutAccess, algIndex);
            //drc->string = taNetArray[i]->netname; 
            for (int p=0; p<pinkoocnt; p++)
            {
               TA_pinkoo *pin = pinkooarray[p];

               if (pin->netindex == i)
               {
                  drc_singlepin_without_access(doc, file, pin->x, pin->y, pin->compname, pin->pinname);
                  break;
               }
            }
         }
      }
   }  
}

/******************************************************************************
* TA_DeleteTestGraphic
*/
static void TA_DeleteTestGraphic(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList)

{
   doc->UnselectAll(FALSE);
   
   LayerStruct *l;

   if ((l = doc->FindLayer_by_Name(T_OUTLINE_BOT)) != NULL)
      DeleteLayer(doc, l, TRUE);
   if ((l = doc->FindLayer_by_Name(T_OUTLINE_TOP)) != NULL)
      DeleteLayer(doc, l, TRUE);
}

/*************************************************************************
* DeleteTestGraphics
*/
static void TA_DeleteTestGraphics(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   // need to make a component outline for every component
   TA_DeleteTestGraphic(doc, pcbfile, &(pcbfile->getBlock()->getDataList())); 

   doc->UpdateAllViews(NULL);
}


static void TA_UpdateDatabase_TESTACCESS(CCEtoODBDoc *doc, FileStruct *pcbfile);

/////////////////////////////////////////////////////////////////////////////
// TA_Options_AccessAnalysis dialog
TA_Options_AccessAnalysis::TA_Options_AccessAnalysis(CWnd* pParent /*=NULL*/)
   : CDialog(TA_Options_AccessAnalysis::IDD, pParent)
{
   //{{AFX_DATA_INIT(TA_Options_AccessAnalysis)
   m_topboardfeaturemin = _T("0");
   m_topfeaturesizemin = _T("0");
   m_topphysicalaccess = _T("0");
   m_topfeaturetypetotal = _T("0");
   m_prefer = -1;
   m_topcomponentfeaturemin = _T("0");
   m_topboardfeature = FALSE;
   m_topcomponentfeature = FALSE;
   m_topfeaturesize = FALSE;
   m_through_pin_access = 0;
   m_SFNbotConnector = FALSE;
   m_SFNbotSMD = FALSE;
   m_SFNbotTestattrib = FALSE;
   m_SFNbotThroughhole = FALSE;
   m_SFNbotVia = FALSE;
   m_SFNtopConnector = FALSE;
   m_SFNtopSMD = FALSE;
   m_SFNtopTestattrib = FALSE;
   m_SFNtopThroughhole = FALSE;
   m_SFNtopVia = FALSE;
   m_MFNbotConnector = FALSE;
   m_MFNbotSMD = FALSE;
   m_MFNbotTestattrib = FALSE;
   m_MFNbotThroughhole = FALSE;
   m_MFNbotVia = FALSE;
   m_MFNtopConnector = FALSE;
   m_MFNtopSMD = FALSE;
   m_MFNtopTestattrib = FALSE;
   m_MFNtopThroughhole = FALSE;
   m_MFNtopVia = FALSE;
   m_botAllowAccess = FALSE;
   m_botBoardFeatureMin = _T("0");
   m_botBoardFeature = FALSE;
   m_botCompFeatureMin = _T("0");
   m_botCompFeature = FALSE;
   m_botFeatureSizeMin = _T("0");
   m_botFeatureSize = FALSE;
   m_botFeatureTypeTotal = _T("0");
   m_botPhysicalAccess = _T("0");
   m_topSoldermask = FALSE;
   m_botSoldermask = FALSE;
   m_botTotalAccess = _T("0");
   m_totalNets = _T("0");
   m_totalMFNets = _T("0");
   m_totalSFNets = _T("0");
   m_totalAccess = _T("");
   m_totalAccessPercent = _T("0");
   m_totalMFAccess = _T("0");
   m_totalMFAccessPercent = _T("0");
   m_totalSFAccess = _T("0");
   m_totalSFAccessPercent = _T("0");
   m_topTotalAccess = _T("0");
   m_topAllowAccess = FALSE;
   m_excludeSinglePin = FALSE;
   //}}AFX_DATA_INIT
}

void TA_Options_AccessAnalysis::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TA_Options_AccessAnalysis)
   DDX_Control(pDX, IDC_TOP_PRIORITY_LB, m_topPriorityLB);
   DDX_Control(pDX, IDC_BOT_PRIORITY_LB, m_botPriorityLB);
   DDX_Text(pDX, IDC_TOP_BOARD_FEATURE, m_topboardfeaturemin);
   DDX_Text(pDX, IDC_TOP_FEATURE_SIZE, m_topfeaturesizemin);
   DDX_Text(pDX, IDC_TOP_PYISICAL_ACCESS, m_topphysicalaccess);
   DDX_Text(pDX, IDC_TOP_FEATURE_TYPE_TOTAL, m_topfeaturetypetotal);
   DDX_Radio(pDX, IDC_PREFER_TOP_BOTTOM, m_prefer);
   DDX_Text(pDX, IDC_TOP_COMPONENT_FEATURE, m_topcomponentfeaturemin);
   DDX_Check(pDX, IDC_TOP_BOARD_FEATURE_ONOFF, m_topboardfeature);
   DDX_Check(pDX, IDC_TOP_COMPONENT_FEATURE_ONOFF, m_topcomponentfeature);
   DDX_Check(pDX, IDC_TOP_FEATURE_SIZE_ONOFF, m_topfeaturesize);
   DDX_Radio(pDX, IDC_THROUGH_PIN_ACCESS, m_through_pin_access);
   DDX_Check(pDX, IDC_SFN_BOT_CONNECTOR, m_SFNbotConnector);
   DDX_Check(pDX, IDC_SFN_BOT_SMD, m_SFNbotSMD);
   DDX_Check(pDX, IDC_SFN_BOT_TEST_ATTRIB, m_SFNbotTestattrib);
   DDX_Check(pDX, IDC_SFN_BOT_THROUGHHOLE, m_SFNbotThroughhole);
   DDX_Check(pDX, IDC_SFN_BOT_VIA, m_SFNbotVia);
   DDX_Check(pDX, IDC_SFN_TOP_CONNECTOR, m_SFNtopConnector);
   DDX_Check(pDX, IDC_SFN_TOP_SMD, m_SFNtopSMD);
   DDX_Check(pDX, IDC_SFN_TOP_TEST_ATTRIB, m_SFNtopTestattrib);
   DDX_Check(pDX, IDC_SFN_TOP_THROUGHHOLE, m_SFNtopThroughhole);
   DDX_Check(pDX, IDC_SFN_TOP_VIA, m_SFNtopVia);
   DDX_Check(pDX, IDC_MFN_BOT_CONNECTOR, m_MFNbotConnector);
   DDX_Check(pDX, IDC_MFN_BOT_SMD, m_MFNbotSMD);
   DDX_Check(pDX, IDC_MFN_BOT_TEST_ATTRIB, m_MFNbotTestattrib);
   DDX_Check(pDX, IDC_MFN_BOT_THROUGHHOLE, m_MFNbotThroughhole);
   DDX_Check(pDX, IDC_MFN_BOT_VIA, m_MFNbotVia);
   DDX_Check(pDX, IDC_MFN_TOP_CONNECTOR, m_MFNtopConnector);
   DDX_Check(pDX, IDC_MFN_TOP_SMD, m_MFNtopSMD);
   DDX_Check(pDX, IDC_MFN_TOP_TEST_ATTRIB, m_MFNtopTestattrib);
   DDX_Check(pDX, IDC_MFN_TOP_THROUGHHOLE, m_MFNtopThroughhole);
   DDX_Check(pDX, IDC_MFN_TOP_VIA, m_MFNtopVia);
   DDX_Check(pDX, IDC_BOT_ALLOW_ACCESS, m_botAllowAccess);
   DDX_Text(pDX, IDC_BOT_BOARD_FEATURE, m_botBoardFeatureMin);
   DDX_Check(pDX, IDC_BOT_BOARD_FEATURE_ONOFF, m_botBoardFeature);
   DDX_Text(pDX, IDC_BOT_COMPONENT_FEATURE, m_botCompFeatureMin);
   DDX_Check(pDX, IDC_BOT_COMPONENT_FEATURE_ONOFF, m_botCompFeature);
   DDX_Text(pDX, IDC_BOT_FEATURE_SIZE, m_botFeatureSizeMin);
   DDX_Check(pDX, IDC_BOT_FEATURE_SIZE_ONOFF, m_botFeatureSize);
   DDX_Text(pDX, IDC_BOT_FEATURE_TYPE_TOTAL, m_botFeatureTypeTotal);
   DDX_Text(pDX, IDC_BOT_PHYSICAL_ACCESS, m_botPhysicalAccess);
   DDX_Check(pDX, IDC_TOP_SOLDERMASK_ONOFF, m_topSoldermask);
   DDX_Check(pDX, IDC_BOT_SOLDERMASK_ONOFF, m_botSoldermask);
   DDX_Text(pDX, IDC_BOT_TOTAL_ACCESS, m_botTotalAccess);
   DDX_Text(pDX, IDC_TOTAL_NETS, m_totalNets);
   DDX_Text(pDX, IDC_TOTAL_MF_NETS, m_totalMFNets);
   DDX_Text(pDX, IDC_TOTAL_SF_NETS, m_totalSFNets);
   DDX_Text(pDX, IDC_TOTAL_ACCESS, m_totalAccess);
   DDX_Text(pDX, IDC_TOTAL_ACCESS_PERCENT, m_totalAccessPercent);
   DDX_Text(pDX, IDC_TOTAL_MF_ACCESS, m_totalMFAccess);
   DDX_Text(pDX, IDC_TOTAL_MF_ACCESS_PERCENT, m_totalMFAccessPercent);
   DDX_Text(pDX, IDC_TOTAL_SF_ACCESS, m_totalSFAccess);
   DDX_Text(pDX, IDC_TOTAL_SF_ACCESS_PERCENT, m_totalSFAccessPercent);
   DDX_Text(pDX, IDC_TOP_TOTAL_ACCESS, m_topTotalAccess);
   DDX_Check(pDX, IDC_TOP_ALLOW_ACCESS, m_topAllowAccess);
   DDX_Check(pDX, IDC_THROUGH_PIN_ACCESS_SINGLE_PINS, m_excludeSinglePin);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TA_Options_AccessAnalysis, CDialog)
   //{{AFX_MSG_MAP(TA_Options_AccessAnalysis)
   ON_BN_CLICKED(IDC_PROCESS, OnProcess)
   ON_BN_CLICKED(IDC_VIEW, OnView)
   ON_BN_CLICKED(IDC_TA_PROBE_HELP, OnTaProbeHelp)
   ON_BN_CLICKED(IDC_MFN_TOP_VIA, OnTopVia)
   ON_BN_CLICKED(IDC_MFN_TOP_THROUGHHOLE, OnTopThroughhole)
   ON_BN_CLICKED(IDC_MFN_TOP_TEST_ATTRIB, OnTopTestAttrib)
   ON_BN_CLICKED(IDC_MFN_TOP_SMD, OnTopSmd)
   ON_BN_CLICKED(IDC_TOP_MOVE_UP, OnTopMoveUp)
   ON_BN_CLICKED(IDC_TOP_MOVE_DOWN, OnTopMoveDown)
   ON_BN_CLICKED(IDC_MFN_TOP_CONNECTOR, OnTopConnector)
   ON_BN_CLICKED(IDC_MFN_BOT_VIA, OnBotVia)
   ON_BN_CLICKED(IDC_MFN_BOT_THROUGHHOLE, OnBotThroughhole)
   ON_BN_CLICKED(IDC_MFN_BOT_TEST_ATTRIB, OnBotTestAttrib)
   ON_BN_CLICKED(IDC_MFN_BOT_SMD, OnBotSmd)
   ON_BN_CLICKED(IDC_BOT_MOVE_UP, OnBotMoveUp)
   ON_BN_CLICKED(IDC_BOT_MOVE_DOWN, OnBotMoveDown)
   ON_BN_CLICKED(IDC_MFN_BOT_CONNECTOR, OnBotConnector)
   ON_BN_CLICKED(IDC_BOT_ALLOW_ACCESS, OnAllowAccessBot)
   ON_BN_CLICKED(IDC_TOP_ALLOW_ACCESS, OnAllowAccessTop)
   ON_BN_CLICKED(IDC_MOUNT_SIDE_OK, OnEnableSinglePinMountSide)
   ON_BN_CLICKED(IDC_SFN_TOP_TEST_ATTRIB, OnTopTestAttrib)
   ON_BN_CLICKED(IDC_SFN_TOP_CONNECTOR, OnTopConnector)
   ON_BN_CLICKED(IDC_SFN_TOP_THROUGHHOLE, OnTopThroughhole)
   ON_BN_CLICKED(IDC_SFN_TOP_SMD, OnTopSmd)
   ON_BN_CLICKED(IDC_SFN_TOP_VIA, OnTopVia)
   ON_BN_CLICKED(IDC_SFN_BOT_TEST_ATTRIB, OnBotTestAttrib)
   ON_BN_CLICKED(IDC_SFN_BOT_CONNECTOR, OnBotConnector)
   ON_BN_CLICKED(IDC_SFN_BOT_THROUGHHOLE, OnBotThroughhole)
   ON_BN_CLICKED(IDC_SFN_BOT_SMD, OnBotSmd)
   ON_BN_CLICKED(IDC_SFN_BOT_VIA, OnBotVia)
   ON_BN_CLICKED(IDC_THROUGH_PIN_ACCESS, OnEnableSinglePinMountSide)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TA_Options_AccessAnalysis message handlers
BOOL TA_Options_AccessAnalysis::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   FillTopPriorityLB();
   FillBotPriorityLB();

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void TA_Options_AccessAnalysis::OnProcess() 
{  
   UpdateData();

   TAoptions.AccessAnalysis.preferNetAccessSurface = m_prefer; // 0 = top, 1 bottom
   TAoptions.AccessAnalysis.allowNetAccessBot = m_botAllowAccess;
   TAoptions.AccessAnalysis.allowNetAccessTop = m_topAllowAccess;

   TAoptions.AccessAnalysis.soldermaskBot = m_botSoldermask;
   TAoptions.AccessAnalysis.soldermaskTop = m_topSoldermask;
   TAoptions.AccessAnalysis.boardFeatureBot = m_botBoardFeature;
   TAoptions.AccessAnalysis.boardFeatureTop = m_topboardfeature;
   TAoptions.AccessAnalysis.compFeatureBot = m_botCompFeature;
   TAoptions.AccessAnalysis.compFeatureTop = m_topcomponentfeature;
   TAoptions.AccessAnalysis.featureSizeBot = m_botFeatureSize;
   TAoptions.AccessAnalysis.featureSizeTop = m_topfeaturesize;

   // boardoutline
   TAoptions.AccessAnalysis.boardOutlineDistanceTop = atof(m_topboardfeaturemin);
   TAoptions.AccessAnalysis.boardOutlineDistanceBot = atof(m_botBoardFeatureMin);

   // component outline
   TAoptions.AccessAnalysis.compOutlineDistanceTop = atof(m_topcomponentfeaturemin);
   TAoptions.AccessAnalysis.compOutlineDistanceBot = atof(m_botCompFeatureMin);

   TAoptions.AccessAnalysis.featureSizeMinTop = atof(m_topfeaturesizemin);
   TAoptions.AccessAnalysis.featureSizeMinBot = atof(m_botFeatureSizeMin);

   TAoptions.AccessAnalysis.topTestattribPriority = topTestattribPriority;
   TAoptions.AccessAnalysis.topConnectorPriority = topConnectorPriority;
   TAoptions.AccessAnalysis.topThroughholePriority = topThroughholePriority;
   TAoptions.AccessAnalysis.topSMDPriority = topSMDPriority;
   TAoptions.AccessAnalysis.topViaPriority = topViaPriority;
   TAoptions.AccessAnalysis.botTestattribPriority = botTestattribPriority;
   TAoptions.AccessAnalysis.botConnectorPriority = botConnectorPriority;
   TAoptions.AccessAnalysis.botThroughholePriority = botThroughholePriority;
   TAoptions.AccessAnalysis.botSMDPriority = botSMDPriority;
   TAoptions.AccessAnalysis.botViaPriority = botViaPriority;

   TAoptions.AccessAnalysis.SFNtopTestattrib = m_SFNtopTestattrib;
   TAoptions.AccessAnalysis.SFNtopConnector = m_SFNtopConnector;
   TAoptions.AccessAnalysis.SFNtopThroughhole = m_SFNtopThroughhole;
   TAoptions.AccessAnalysis.SFNtopSMD = m_SFNtopSMD;
   TAoptions.AccessAnalysis.SFNtopVia = m_SFNtopVia;
   TAoptions.AccessAnalysis.SFNbotTestattrib = m_SFNbotTestattrib;
   TAoptions.AccessAnalysis.SFNbotConnector = m_SFNbotConnector;
   TAoptions.AccessAnalysis.SFNbotThroughhole = m_SFNbotThroughhole;
   TAoptions.AccessAnalysis.SFNbotSMD = m_SFNbotSMD;
   TAoptions.AccessAnalysis.SFNbotVia = m_SFNbotVia;

   TAoptions.AccessAnalysis.MFNtopTestattrib = m_MFNtopTestattrib;
   TAoptions.AccessAnalysis.MFNtopConnector = m_MFNtopConnector;
   TAoptions.AccessAnalysis.MFNtopThroughhole = m_MFNtopThroughhole;
   TAoptions.AccessAnalysis.MFNtopSMD = m_MFNtopSMD;
   TAoptions.AccessAnalysis.MFNtopVia = m_MFNtopVia;
   TAoptions.AccessAnalysis.MFNbotTestattrib = m_MFNbotTestattrib;
   TAoptions.AccessAnalysis.MFNbotConnector = m_MFNbotConnector;
   TAoptions.AccessAnalysis.MFNbotThroughhole = m_MFNbotThroughhole;
   TAoptions.AccessAnalysis.MFNbotSMD = m_MFNbotSMD;
   TAoptions.AccessAnalysis.MFNbotVia = m_MFNbotVia;

   TAoptions.AccessAnalysis.throughPinAccess = m_through_pin_access; // 0=non mount side, 1 = both
   TAoptions.AccessAnalysis.excludeSinglePin = m_excludeSinglePin;

   // here the viakooarray and pinkooarray is recalculated for the current dialog settings.
   
   // the process calc starts with the preferred layer and than goes to the other layer. The
   // result is the TEST_PREFERENCE Points which are derived from the TEST_ACCESS and the dialog filter.
   // 

   TA_process_access_analysis_form(doc, pcbFile);

   // first do prefered layer
   // now do the rest

   m_botTotalAccess.Format("%d", TApcbfiletestaccess.net_access_total_bottom);
   m_topTotalAccess.Format("%d", TApcbfiletestaccess.net_access_total_top);

   m_botPhysicalAccess.Format("%d", TApcbfiletestaccess.net_access_physical_bottom);
   m_topphysicalaccess.Format("%d", TApcbfiletestaccess.net_access_physical_top);

   m_botFeatureTypeTotal.Format("%d", TApcbfiletestaccess.net_access_feature_bottom);
   m_topfeaturetypetotal.Format("%d", TApcbfiletestaccess.net_access_feature_top);

   m_totalAccess.Format("%d", TApcbfiletestaccess.total_access_nets);

   if (TAoptions.Unusedpins)
      m_totalSFAccess.Format("%d", TApcbfiletestaccess.singleport_access_nets);
   else
      m_totalSFAccess = "-";

   m_totalMFAccess.Format("%d", TApcbfiletestaccess.multiport_access_nets);

   // make sure it nevers end up 100 through rounding if not equal
   TA_Percent(&m_totalAccessPercent, TApcbfiletestaccess.total_access_nets, TApcbfiletestaccess.total_nets);

   // make sure it nevers end up 100 through rounding if not equal
   if (TAoptions.Unusedpins)
      TA_Percent(&m_totalSFAccessPercent, TApcbfiletestaccess.singleport_access_nets, TApcbfiletestaccess.singleportnets);
   else
      m_totalSFAccessPercent = "";

   // make sure it nevers end up 100 through rounding if not equal
   TA_Percent(&m_totalMFAccessPercent, TApcbfiletestaccess.multiport_access_nets, TApcbfiletestaccess.multiportnets);

   UpdateData(FALSE);
}

void TA_Options_AccessAnalysis::OnView() 
{
   CString filename;
   CFileDialog FileDialog(FALSE, "csv", "*.csv", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Debug Access Report (*.csv)|*.csv|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   filename = FileDialog.GetPathName();
   
   //TA_Report(filename, doc);
   TA_DebugReport(filename, doc);
}

void TA_Options_AccessAnalysis::OnOK() 
{
   CWaitCursor wait;

   UpdateData();

   // this updates the physically accessible layer
   TA_UpdateDatabase_TESTACCESS(doc, pcbFile);

   CDialog::OnOK();
}

void TA_Options_AccessAnalysis::OnTaProbeHelp() 
{
   
}

void TA_Options_AccessAnalysis::OnCancel() 
{
   if (ErrorMessage("Test Access Analysis Information will not be updated.\nTo save the Access Analysis Information use \"Update Database\"\nDo you want to Cancel?", "Cancel Test Access Analysis", MB_YESNO | MB_DEFBUTTON2)==IDYES)
   {
      TA_DeleteTestGraphics(doc, pcbFile);
      CDialog::OnCancel();
   }
}


void TA_Options_AccessAnalysis::FillTopPriorityLB()
{
   m_topPriorityLB.ResetContent();

   for (int i=1; i<=5; i++)
   {
      if (topTestattribPriority == i)
         m_topPriorityLB.AddString("Test Attrib");
      else if (topConnectorPriority == i)
         m_topPriorityLB.AddString("Connector");
      else if (topThroughholePriority == i)
         m_topPriorityLB.AddString("Through Hole");
      else if (topSMDPriority == i)
         m_topPriorityLB.AddString("SMD");
      else if (topViaPriority == i)
         m_topPriorityLB.AddString("Via");
   }
}

void TA_Options_AccessAnalysis::FillBotPriorityLB()
{
   m_botPriorityLB.ResetContent();

   for (int i=1; i<=5; i++)
   {
      if (botTestattribPriority == i)
         m_botPriorityLB.AddString("Test Attrib");
      else if (botConnectorPriority == i)
         m_botPriorityLB.AddString("Connector");
      else if (botThroughholePriority == i)
         m_botPriorityLB.AddString("Through Hole");
      else if (botSMDPriority == i)
         m_botPriorityLB.AddString("SMD");
      else if (botViaPriority == i)
         m_botPriorityLB.AddString("Via");
   }
}


void TA_Options_AccessAnalysis::OnTopTestAttrib() 
{
   UpdateData();

   if ((m_MFNtopTestattrib || m_SFNtopTestattrib) && !topTestattribPriority)
      topTestattribPriority = m_topPriorityLB.GetCount() + 1;
   else if (!(m_MFNtopTestattrib || m_SFNtopTestattrib) && topTestattribPriority)
   {
      if (topConnectorPriority > topTestattribPriority)
         topConnectorPriority--;
      if (topThroughholePriority > topTestattribPriority)
         topThroughholePriority--;
      if (topSMDPriority > topTestattribPriority)
         topSMDPriority--;
      if (topViaPriority > topTestattribPriority)
         topViaPriority--;

      topTestattribPriority = 0;
   }

   FillTopPriorityLB();
}

void TA_Options_AccessAnalysis::OnTopConnector() 
{
   UpdateData();

   if ((m_MFNtopConnector || m_SFNtopConnector) && !topConnectorPriority)
      topConnectorPriority = m_topPriorityLB.GetCount() + 1;
   else if (!(m_MFNtopConnector || m_SFNtopConnector) && topConnectorPriority)
   {
      if (topTestattribPriority > topConnectorPriority)
         topTestattribPriority--;
      if (topThroughholePriority > topConnectorPriority)
         topThroughholePriority--;
      if (topSMDPriority > topConnectorPriority)
         topSMDPriority--;
      if (topViaPriority > topConnectorPriority)
         topViaPriority--;

      topConnectorPriority = 0;
   }

   FillTopPriorityLB();
}

void TA_Options_AccessAnalysis::OnTopThroughhole() 
{
   UpdateData();

   if ((m_MFNtopThroughhole || m_SFNtopThroughhole) && !topThroughholePriority)
      topThroughholePriority = m_topPriorityLB.GetCount() + 1;
   else if (!(m_MFNtopThroughhole || m_SFNtopThroughhole) && topThroughholePriority)
   {
      if (topTestattribPriority > topThroughholePriority)
         topTestattribPriority--;
      if (topConnectorPriority > topThroughholePriority)
         topConnectorPriority--;
      if (topSMDPriority > topThroughholePriority)
         topSMDPriority--;
      if (topViaPriority > topThroughholePriority)
         topViaPriority--;

      topThroughholePriority = 0;
   }

   FillTopPriorityLB();
}

void TA_Options_AccessAnalysis::OnTopSmd() 
{
   UpdateData();

   if ((m_MFNtopSMD || m_SFNtopSMD) && !topSMDPriority)
      topSMDPriority = m_topPriorityLB.GetCount() + 1;
   else if (!(m_MFNtopSMD || m_SFNtopSMD) && topSMDPriority)
   {
      if (topTestattribPriority > topSMDPriority)
         topTestattribPriority--;
      if (topConnectorPriority > topSMDPriority)
         topConnectorPriority--;
      if (topThroughholePriority > topSMDPriority)
         topThroughholePriority--;
      if (topViaPriority > topSMDPriority)
         topViaPriority--;

      topSMDPriority = 0;
   }

   FillTopPriorityLB();
}

void TA_Options_AccessAnalysis::OnTopVia() 
{
   UpdateData();

   if ((m_MFNtopVia || m_SFNtopVia) && !topViaPriority)
      topViaPriority = m_topPriorityLB.GetCount() + 1;
   else if (!(m_MFNtopVia || m_SFNtopVia) && topViaPriority)
   {
      if (topTestattribPriority > topViaPriority)
         topTestattribPriority--;
      if (topConnectorPriority > topViaPriority)
         topConnectorPriority--;
      if (topThroughholePriority > topViaPriority)
         topThroughholePriority--;
      if (topSMDPriority > topViaPriority)
         topSMDPriority--;

      topViaPriority = 0;
   }

   FillTopPriorityLB();
}

void TA_Options_AccessAnalysis::OnBotTestAttrib() 
{
   UpdateData();

   if ((m_MFNbotTestattrib || m_SFNbotTestattrib) && !botTestattribPriority)
      botTestattribPriority = m_botPriorityLB.GetCount() + 1;
   else if (!(m_MFNbotTestattrib || m_SFNbotTestattrib) && botTestattribPriority)
   {
      if (botConnectorPriority > botTestattribPriority)
         botConnectorPriority--;
      if (botThroughholePriority > botTestattribPriority)
         botThroughholePriority--;
      if (botSMDPriority > botTestattribPriority)
         botSMDPriority--;
      if (botViaPriority > botTestattribPriority)
         botViaPriority--;

      botTestattribPriority = 0;
   }

   FillBotPriorityLB();
}

void TA_Options_AccessAnalysis::OnBotConnector() 
{
   UpdateData();

   if ((m_MFNbotConnector || m_SFNbotConnector) && !botConnectorPriority)
      botConnectorPriority = m_botPriorityLB.GetCount() + 1;
   else if (!(m_MFNbotConnector || m_SFNbotConnector) && botConnectorPriority)
   {
      if (botTestattribPriority > botConnectorPriority)
         botTestattribPriority--;
      if (botThroughholePriority > botConnectorPriority)
         botThroughholePriority--;
      if (botSMDPriority > botConnectorPriority)
         botSMDPriority--;
      if (botViaPriority > botConnectorPriority)
         botViaPriority--;

      botConnectorPriority = 0;
   }

   FillBotPriorityLB();
}

void TA_Options_AccessAnalysis::OnBotThroughhole() 
{
   UpdateData();

   if ((m_MFNbotThroughhole || m_SFNbotThroughhole) && !botThroughholePriority)
      botThroughholePriority = m_botPriorityLB.GetCount() + 1;
   else if (!(m_MFNbotThroughhole || m_SFNbotThroughhole) && botThroughholePriority)
   {
      if (botTestattribPriority > botThroughholePriority)
         botTestattribPriority--;
      if (botConnectorPriority > botThroughholePriority)
         botConnectorPriority--;
      if (botSMDPriority > botThroughholePriority)
         botSMDPriority--;
      if (botViaPriority > botThroughholePriority)
         botViaPriority--;

      botThroughholePriority = 0;
   }

   FillBotPriorityLB();
}

void TA_Options_AccessAnalysis::OnBotSmd() 
{
   UpdateData();

   if ((m_MFNbotSMD || m_SFNbotSMD) && !botSMDPriority)
      botSMDPriority = m_botPriorityLB.GetCount() + 1;
   else if (!(m_MFNbotSMD || m_SFNbotSMD) && botSMDPriority)
   {
      if (botTestattribPriority > botSMDPriority)
         botTestattribPriority--;
      if (botConnectorPriority > botSMDPriority)
         botConnectorPriority--;
      if (botThroughholePriority > botSMDPriority)
         botThroughholePriority--;
      if (botViaPriority > botSMDPriority)
         botViaPriority--;

      botSMDPriority = 0;
   }

   FillBotPriorityLB();
}

void TA_Options_AccessAnalysis::OnBotVia() 
{
   UpdateData();

   if ((m_MFNbotVia || m_SFNbotVia) && !botViaPriority)
      botViaPriority = m_botPriorityLB.GetCount() + 1;
   else if (!(m_MFNbotVia || m_SFNbotVia) && botViaPriority)
   {
      if (botTestattribPriority > botViaPriority)
         botTestattribPriority--;
      if (botConnectorPriority > botViaPriority)
         botConnectorPriority--;
      if (botThroughholePriority > botViaPriority)
         botThroughholePriority--;
      if (botSMDPriority > botViaPriority)
         botSMDPriority--;

      botViaPriority = 0;
   }

   FillBotPriorityLB();
}


void TA_Options_AccessAnalysis::OnTopMoveUp() 
{
   int sel = m_topPriorityLB.GetCurSel();
   if (sel == LB_ERR)
      return;
   if (!sel)
      return;

   int *a = NULL, *b = NULL;
   
   if (topTestattribPriority == sel+1)
      a = &topTestattribPriority;
   if (topTestattribPriority == sel)
      b = &topTestattribPriority;

   if (topConnectorPriority == sel+1)
      a = &topConnectorPriority;
   if (topConnectorPriority == sel)
      b = &topConnectorPriority;

   if (topThroughholePriority == sel+1)
      a = &topThroughholePriority;
   if (topThroughholePriority == sel)
      b = &topThroughholePriority;

   if (topSMDPriority == sel+1)
      a = &topSMDPriority;
   if (topSMDPriority == sel)
      b = &topSMDPriority;

   if (topViaPriority == sel+1)
      a = &topViaPriority;
   if (topViaPriority == sel)
      b = &topViaPriority;

   if (a && b)
   {
      *a = sel;
      *b = sel+1;
      FillTopPriorityLB();
      m_topPriorityLB.SetCurSel(sel-1);
   }
}

void TA_Options_AccessAnalysis::OnTopMoveDown() 
{
   int sel = m_topPriorityLB.GetCurSel();
   if (sel == LB_ERR)
      return;

   int *a = NULL, *b = NULL;
   
   if (topTestattribPriority == sel+1)
      a = &topTestattribPriority;
   if (topTestattribPriority == sel+2)
      b = &topTestattribPriority;

   if (topConnectorPriority == sel+1)
      a = &topConnectorPriority;
   if (topConnectorPriority == sel+2)
      b = &topConnectorPriority;

   if (topThroughholePriority == sel+1)
      a = &topThroughholePriority;
   if (topThroughholePriority == sel+2)
      b = &topThroughholePriority;

   if (topSMDPriority == sel+1)
      a = &topSMDPriority;
   if (topSMDPriority == sel+2)
      b = &topSMDPriority;

   if (topViaPriority == sel+1)
      a = &topViaPriority;
   if (topViaPriority == sel+2)
      b = &topViaPriority;

   if (a && b)
   {
      *a = sel+2;
      *b = sel+1;
      FillTopPriorityLB();
      m_topPriorityLB.SetCurSel(sel+1);
   }
}

void TA_Options_AccessAnalysis::OnBotMoveUp() 
{
   int sel = m_botPriorityLB.GetCurSel();
   if (sel == LB_ERR)
      return;
   if (!sel)
      return;

   int *a = NULL, *b = NULL;
   
   if (botTestattribPriority == sel+1)
      a = &botTestattribPriority;
   if (botTestattribPriority == sel)
      b = &botTestattribPriority;

   if (botConnectorPriority == sel+1)
      a = &botConnectorPriority;
   if (botConnectorPriority == sel)
      b = &botConnectorPriority;

   if (botThroughholePriority == sel+1)
      a = &botThroughholePriority;
   if (botThroughholePriority == sel)
      b = &botThroughholePriority;

   if (botSMDPriority == sel+1)
      a = &botSMDPriority;
   if (botSMDPriority == sel)
      b = &botSMDPriority;

   if (botViaPriority == sel+1)
      a = &botViaPriority;
   if (botViaPriority == sel)
      b = &botViaPriority;

   if (a && b)
   {
      *a = sel;
      *b = sel+1;
      FillBotPriorityLB();
      m_botPriorityLB.SetCurSel(sel-1);
   }
}

void TA_Options_AccessAnalysis::OnBotMoveDown() 
{
   int sel = m_botPriorityLB.GetCurSel();
   if (sel == LB_ERR)
      return;

   int *a = NULL, *b = NULL;
   
   if (botTestattribPriority == sel+1)
      a = &botTestattribPriority;
   if (botTestattribPriority == sel+2)
      b = &botTestattribPriority;

   if (botConnectorPriority == sel+1)
      a = &botConnectorPriority;
   if (botConnectorPriority == sel+2)
      b = &botConnectorPriority;

   if (botThroughholePriority == sel+1)
      a = &botThroughholePriority;
   if (botThroughholePriority == sel+2)
      b = &botThroughholePriority;

   if (botSMDPriority == sel+1)
      a = &botSMDPriority;
   if (botSMDPriority == sel+2)
      b = &botSMDPriority;

   if (botViaPriority == sel+1)
      a = &botViaPriority;
   if (botViaPriority == sel+2)
      b = &botViaPriority;

   if (a && b)
   {
      *a = sel+2;
      *b = sel+1;
      FillBotPriorityLB();
      m_botPriorityLB.SetCurSel(sel+1);
   }
}

void TA_Options_AccessAnalysis::OnEnableSinglePinMountSide() 
{
   UpdateData();

   GetDlgItem(IDC_THROUGH_PIN_ACCESS_SINGLE_PINS)->EnableWindow(!m_through_pin_access);   
}

void TA_Options_AccessAnalysis::OnAllowAccessTop() 
{
   UpdateData();

   GetDlgItem(IDC_TOP_SOLDERMASK_ONOFF)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_TOP_BOARD_FEATURE)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_TOP_BOARD_FEATURE_ONOFF)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_TOP_COMPONENT_FEATURE)->EnableWindow(m_topAllowAccess);  
   GetDlgItem(IDC_TOP_COMPONENT_FEATURE_ONOFF)->EnableWindow(m_topAllowAccess);  
   GetDlgItem(IDC_TOP_FEATURE_SIZE)->EnableWindow(m_topAllowAccess); 
   GetDlgItem(IDC_TOP_FEATURE_SIZE_ONOFF)->EnableWindow(m_topAllowAccess); 
   GetDlgItem(IDC_TOP_PRIORITY_LB)->EnableWindow(m_topAllowAccess);  
   GetDlgItem(IDC_TOP_MOVE_UP)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_TOP_MOVE_DOWN)->EnableWindow(m_topAllowAccess); 
   GetDlgItem(IDC_MFN_TOP_TEST_ATTRIB)->EnableWindow(m_topAllowAccess); 
   GetDlgItem(IDC_SFN_TOP_TEST_ATTRIB)->EnableWindow(m_topAllowAccess); 
   GetDlgItem(IDC_MFN_TOP_CONNECTOR)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_SFN_TOP_CONNECTOR)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_MFN_TOP_THROUGHHOLE)->EnableWindow(m_topAllowAccess); 
   GetDlgItem(IDC_SFN_TOP_THROUGHHOLE)->EnableWindow(m_topAllowAccess); 
   GetDlgItem(IDC_MFN_TOP_VIA)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_SFN_TOP_VIA)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_MFN_TOP_SMD)->EnableWindow(m_topAllowAccess);   
   GetDlgItem(IDC_SFN_TOP_SMD)->EnableWindow(m_topAllowAccess);   
}

void TA_Options_AccessAnalysis::OnAllowAccessBot() 
{
   UpdateData();

   GetDlgItem(IDC_BOT_SOLDERMASK_ONOFF)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_BOT_BOARD_FEATURE)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_BOT_BOARD_FEATURE_ONOFF)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_BOT_COMPONENT_FEATURE)->EnableWindow(m_botAllowAccess);  
   GetDlgItem(IDC_BOT_COMPONENT_FEATURE_ONOFF)->EnableWindow(m_botAllowAccess);  
   GetDlgItem(IDC_BOT_FEATURE_SIZE)->EnableWindow(m_botAllowAccess); 
   GetDlgItem(IDC_BOT_FEATURE_SIZE_ONOFF)->EnableWindow(m_botAllowAccess); 
   GetDlgItem(IDC_BOT_PRIORITY_LB)->EnableWindow(m_botAllowAccess);  
   GetDlgItem(IDC_BOT_MOVE_UP)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_BOT_MOVE_DOWN)->EnableWindow(m_botAllowAccess); 
   GetDlgItem(IDC_MFN_BOT_TEST_ATTRIB)->EnableWindow(m_botAllowAccess); 
   GetDlgItem(IDC_SFN_BOT_TEST_ATTRIB)->EnableWindow(m_botAllowAccess); 
   GetDlgItem(IDC_MFN_BOT_CONNECTOR)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_SFN_BOT_CONNECTOR)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_MFN_BOT_THROUGHHOLE)->EnableWindow(m_botAllowAccess); 
   GetDlgItem(IDC_SFN_BOT_THROUGHHOLE)->EnableWindow(m_botAllowAccess); 
   GetDlgItem(IDC_MFN_BOT_VIA)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_SFN_BOT_VIA)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_MFN_BOT_SMD)->EnableWindow(m_botAllowAccess);   
   GetDlgItem(IDC_SFN_BOT_SMD)->EnableWindow(m_botAllowAccess);   
}


/******************************************************************************
* get_via_access_fail_message
*/
static char *get_via_access_fail_message(TA_viakoo *c)
{
   if (c->net_not_tested)              return "Net is not tested";
   if (c->boardoutline & 1)            return "Board Outline Distance Top";
   if (c->boardoutline & 2)            return "Board Outline Distance Bottom";
   if (c->compoutline & 1)             return "Component Outline Distance Top";
   if (c->compoutline & 2)             return "Component Outline Distance Bottom";
   if (c->cadlayer == 0)               return "No CAD access layer";
   if (c-> layer== 0)                  return "No Test access layer";
   if (c->net_access == 0)             return "No Net access";
   if (c->soldermaskbottom_deleted)    return "Soldermask Bottom";
   if (c->soldermasktop_deleted)       return "Soldermask Top";
   if (c->featuresizebottom_deleted)   return "Feature Bottom too small";
   if (c->featuresizetop_deleted)      return "Feature Top too small";

   if (c->featureallowedtesttop_deleted)     return "Feature TEST not allowed Top";
   if (c->featureallowedtestbottom_deleted)  return "Feature TEST not allowed Bottom";

   if (c->featureallowedviatop_deleted)      return "Feature VIA not allowed Top";
   if (c->featureallowedviabottom_deleted)   return "Feature VIA not allowed Bottom";

   if (c->featureallowedsmdtop_deleted)      return "Feature SMD not allowed Top";
   if (c->featureallowedsmdbottom_deleted)   return "Feature SMD not allowed Bottom";

   if (c->feature_accessible == 0)     return "No Feature access";
   if (c->physical_accessible == 0)    return "No Physical access";

   return "Unknown Access Fail ??";
}

/******************************************************************************
* update_allvias_TESTACCESS
*/
static void update_allvias_TESTACCESS(CCEtoODBDoc *doc, CDataList *DataList)
{
   doc->UnselectAll(TRUE);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA && data->getInsert()->getInsertType() != INSERTTYPE_TESTPAD &&
            data->getInsert()->getInsertType() != INSERTTYPE_TEST_ACCESSPOINT) 
         continue;

      for (int i=0; i<viakoocnt; i++)
      {
         if (viakooarray[i]->entitynumber == data->getEntityNumber())
         {
            //switch (viakooarray[i]->physical_accessible)
            switch (viakooarray[i]->feature_accessible)
            {
            case 0: // no test layer
               {
                  doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE), VT_STRING, "NONE", SA_OVERWRITE, NULL);

                  CString msg;
                  msg = get_via_access_fail_message(viakooarray[i]);

                  // here needs to be the ATT_TESTACCESS_FAIL message
                  doc->SetUnknownAttrib(&data->getAttributesRef(),ATT_TESTACCESS_FAIL, msg, SA_OVERWRITE, NULL);
               }
               break;

            case 1: // top test layer
               doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE), VT_STRING, "TOP", SA_OVERWRITE, NULL);
               break;

            case 2: // bottom test layer
               doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE), VT_STRING, "BOTTOM", SA_OVERWRITE, NULL);
               break;

            case 3: // all test layer
               doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE), VT_STRING, "BOTH", SA_OVERWRITE, NULL);
               break;
            }
         }
      }
   }
}

/******************************************************************************
* get_pin_access_fail_message
*/
static char *get_pin_access_fail_message(TA_pinkoo *c)
{
   static char msg[255];

   if (c->net_not_tested)              return "Net is not tested";
   if (c->boardoutline & 1)            return "Board Outline Distance Top";
   if (c->boardoutline & 2)            return "Board Outline Distance Bottom";
   if (c->compoutline & 1)             return "Component Outline Distance Top";
   if (c->compoutline & 2)             return "Component Outline Distance Bottom";
   if (c->cadlayer == 0)               return "No CAD access layer";
   if (c->layer == 0)                  return "No Test access layer";
   if (c->net_access == 0)             return "No Net access";
   if (c->soldermaskbottom_deleted)    return "Soldermask Bottom";
   if (c->soldermasktop_deleted)       return "Soldermask Top";
   if (c->featuresizetop_deleted)      return "Feature Top too small";
   if (c->featuresizebottom_deleted)   return "Feature Bottom too small";

   if (c->featureallowedtesttop_deleted)     return "Feature TEST not allowed Top";
   if (c->featureallowedtestbottom_deleted)  return "Feature TEST not allowed Bottom";

   if (c->featureallowedconnectortop_deleted)      return "Feature CONNECTOR not allowed Top";
   if (c->featureallowedconnectorbottom_deleted)   return "Feature CONNECTOR not allowed Bottom";

   if (c->featureallowedsmdtop_deleted)      return "Feature SMD not allowed Top";
   if (c->featureallowedsmdbottom_deleted)   return "Feature SMD not allowed Bottom";
   if (c->featureallowedthrutop_deleted)     return "Feature THRU not allowed Top";
   if (c->featureallowedthrubottom_deleted)  return "Feature THRU not allowed Bottom";

   if (c->feature_accessible == 0)     return "No Feature access";
   if (c->physical_accessible == 0)    return "No Physical access";

   return "Unknown Access Fail ??";
}

/******************************************************************************
* update_otherpinloc_TESTACCESS
*/
static void update_otherpinloc_TESTACCESS(CCEtoODBDoc *doc, CNetList *NetList, CDataList *DataList)
{
   doc->UnselectAll(TRUE);

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);

      // SaveAttribs(stream, &net->getAttributesRef());

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         int i = get_pinkoocnt_entity(compPin->getEntityNumber());
         if (i < 0)
         {
#ifdef _DEBUG
            ErrorMessage("Comppin not found in TA update_otherpinloc_TESTACCESS", "DEBUG Error");
#endif
            continue;
         }
#ifdef _DEBUG
      TA_pinkoo *c = pinkooarray[i];   // debug only
#endif
         if (compPin->getEntityNumber() == pinkooarray[i]->entitynumber)
         {
            //switch (pinkooarray[i]->physical_accessible)
            switch (pinkooarray[i]->feature_accessible)
            {
               case 0:  // no test layer
               {
                  doc->SetAttrib(&compPin->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE),
                     VT_STRING,
                     "NONE",
                     SA_OVERWRITE, NULL);
                  CString msg;
                  msg = get_pin_access_fail_message(pinkooarray[i]);

                  // here needs to be the ATT_TESTACCESS_FAIL message
                  doc->SetUnknownAttrib(&compPin->getAttributesRef(),ATT_TESTACCESS_FAIL, msg,
                     SA_OVERWRITE, NULL);
               }
               break;
               case 1:  // top test layer
               {
                  doc->SetAttrib(&compPin->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE),
                     VT_STRING,
                     "TOP",
                     SA_OVERWRITE, NULL);
               }
               break;
               case 2:  // bottom test layer
               {
                  doc->SetAttrib(&compPin->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE),
                     VT_STRING,
                     "BOTTOM",
                     SA_OVERWRITE, NULL);
               }
               break;
               case 3:  // all test layer
               {
                  doc->SetAttrib(&compPin->getAttributesRef(),doc->IsKeyWord(ATT_TESTACCESS, TRUE),
                     VT_STRING,
                     "BOTH",
                     SA_OVERWRITE, NULL);
               }
               break;
            }
         }
      }
   }
}

/******************************************************************************
* GetTestPriority_Pin
*  - returns the lowest priority
*/
static int GetTestPriority_Pin(int index, int layer)
{
   int prior = INT_MAX;

   if (layer & 1) // top
   {
      if (TAoptions.AccessAnalysis.topConnectorPriority)
      {
         if (pinkooarray[index]->connector)  
            prior = min(prior, TAoptions.AccessAnalysis.topConnectorPriority*10);
      }
      if (TAoptions.AccessAnalysis.topTestattribPriority)
      {
         if (pinkooarray[index]->test) 
            prior = min(prior, TAoptions.AccessAnalysis.topTestattribPriority*10);
      }
      if (TAoptions.AccessAnalysis.topSMDPriority)
      {
         if (!pinkooarray[index]->test && pinkooarray[index]->smd)   
            prior = min(prior, TAoptions.AccessAnalysis.topSMDPriority*10);
      }
      if (TAoptions.AccessAnalysis.topThroughholePriority)
      {
         if (!pinkooarray[index]->test && !pinkooarray[index]->smd)  
            prior = min(prior, TAoptions.AccessAnalysis.topThroughholePriority*10);
      }
   }

   if (layer & 2) // bot
   {
      if (TAoptions.AccessAnalysis.botConnectorPriority)
      {
         if (pinkooarray[index]->connector)  
            prior = min(prior, TAoptions.AccessAnalysis.botConnectorPriority*10);
      }
      if (TAoptions.AccessAnalysis.botTestattribPriority)
      {
         if (pinkooarray[index]->test) 
            prior = min(prior, TAoptions.AccessAnalysis.botTestattribPriority*10);
      }
      if (TAoptions.AccessAnalysis.botSMDPriority)
      {
         if (!pinkooarray[index]->test && pinkooarray[index]->smd)   
            prior = min(prior, TAoptions.AccessAnalysis.botSMDPriority*10);
      }
      if (TAoptions.AccessAnalysis.botThroughholePriority)
      {
         if (!pinkooarray[index]->test && !pinkooarray[index]->smd)  
            prior = min(prior, TAoptions.AccessAnalysis.botThroughholePriority*10);
      }
   }

   if (prior == INT_MAX)
      return -1;  
   else
      return prior;
}

/******************************************************************************
* GetTestPriority_Via
   return the lowest priority
*/
static int GetTestPriority_Via(int index, int layer)
{
   TA_viakoo *via = viakooarray[index];

   int priority = INT_MAX;

   // test 
   if (layer & 1)
   {
      if (TAoptions.AccessAnalysis.topTestattribPriority)
      {
         if (via->test) 
            priority = min(priority, TAoptions.AccessAnalysis.topTestattribPriority*10);
      }
      if (TAoptions.AccessAnalysis.topSMDPriority)
      {
         if (!via->test && viakooarray[index]->smd)   
            priority = min(priority, TAoptions.AccessAnalysis.topSMDPriority*10);
      }
      if (TAoptions.AccessAnalysis.topThroughholePriority)
      {
         if (!via->test && !viakooarray[index]->smd && 
             via->entity_type != INSERTTYPE_VIA)   
            priority = min(priority, TAoptions.AccessAnalysis.topThroughholePriority*10);
      }
      if (!via->test && via->entity_type == INSERTTYPE_VIA)
         if ( (via->single_feature && TAoptions.AccessAnalysis.SFNtopVia) || 
               (!via->single_feature && TAoptions.AccessAnalysis.MFNtopVia) )
            priority = min(priority, TAoptions.AccessAnalysis.topViaPriority*10);
   }
   else if (layer & 2)
   {
      if (TAoptions.AccessAnalysis.botTestattribPriority)
      {
         if (via->test) 
            priority = min(priority, TAoptions.AccessAnalysis.botTestattribPriority*10);
      }
      if (TAoptions.AccessAnalysis.botSMDPriority)
      {
         if (!via->test && via->smd)   
            priority = min(priority, TAoptions.AccessAnalysis.botSMDPriority*10);
      }
      if (TAoptions.AccessAnalysis.botThroughholePriority)
      {
         if (!via->test && !via->smd && 
             via->entity_type != INSERTTYPE_VIA)   
            priority = min(priority, TAoptions.AccessAnalysis.botThroughholePriority*10);
      }
      if (!via->test && via->entity_type == INSERTTYPE_VIA) 
         if ( (via->single_feature && TAoptions.AccessAnalysis.SFNbotVia) || 
               (!via->single_feature && TAoptions.AccessAnalysis.MFNbotVia) )
            priority = min(priority, TAoptions.AccessAnalysis.botViaPriority*10);
   }

   if (priority == INT_MAX)
      return -1;  
   else
      return priority;
}

/******************************************************************************
* create_TESTACCESS_insert
*/
static void create_TESTACCESS_insert(CCEtoODBDoc *doc, FileStruct *pcbfile, CDataList *DataList)
{
   int accesscnt = 0;
   CString refname;

   doc->PrepareAddEntity(pcbfile);  

   BlockStruct *testaccessblock =  generate_TestAccessGeometry(doc, "TEST_ACCESS", TAoptions.AccessAnalysis.testGraphicSize);

	int i=0;
   for (i=0; i<pinkoocnt; i++)
   {
      CString  lname;
      DataStruct *d;

#ifdef _DEBUG
      TA_pinkoo *c = pinkooarray[i];
#endif

      //if (pinkooarray[i]->probe_assigned == 0)   continue;
      if (pinkooarray[i]->feature_accessible == 0)          continue;

      switch (pinkooarray[i]->feature_accessible)
      {
         case 1:
         {
            // must be filenum 0, because apertures are global.
            refname.Format("$$ACCESS_%d",++accesscnt);
            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 pinkooarray[i]->access_top_x, pinkooarray[i]->access_top_y, 0.0, 0, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);

            if (!taNetArray[pinkooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[pinkooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(pinkooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            int prior = GetTestPriority_Pin(i,1);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (pinkooarray[i]->outline_top_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(pinkooarray[i]->outline_top_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }
         }
         break;
         case 2:
         {
            // must be filenum 0, because apertures are global.
            refname.Format("$$ACCESS_%d",++accesscnt);
            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 pinkooarray[i]->access_bottom_x, pinkooarray[i]->access_bottom_y, 
                                 0.0, 1, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);
            if (!taNetArray[pinkooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[pinkooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(pinkooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            int prior = GetTestPriority_Pin(i,2);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (pinkooarray[i]->outline_bottom_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(pinkooarray[i]->outline_bottom_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }
         }
         break;
         case 3:
         {
            // place 2 top and bottom
            refname.Format("$$ACCESS_%d",++accesscnt);
            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 pinkooarray[i]->access_top_x, pinkooarray[i]->access_top_y, 0.0, 0, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);
            if (!taNetArray[pinkooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[pinkooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(pinkooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            int prior = GetTestPriority_Pin(i,1);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (pinkooarray[i]->outline_top_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(pinkooarray[i]->outline_top_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            refname.Format("$$ACCESS_%d",++accesscnt);
            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 pinkooarray[i]->access_bottom_x, pinkooarray[i]->access_bottom_y, 
                                 0.0, 1, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);
            if (!taNetArray[pinkooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[pinkooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(pinkooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            prior = GetTestPriority_Pin(i,2);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (pinkooarray[i]->outline_bottom_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(pinkooarray[i]->outline_bottom_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }
         }
         break;
         default:
            // 
            fprintf(taLog,"Error in test probe layer\n");
            taDisplayError++;
            continue;
         break;
      }
   }

   for (i=0;i<viakoocnt;i++)
   {
      CString  lname;
      DataStruct *d;

#ifdef _DEBUG
      TA_viakoo *c = viakooarray[i];
#endif

      //if (viakooarray[i]->probe_assigned == 0)   continue;
      if (viakooarray[i]->feature_accessible == 0)          continue;

      // if not top it is bottom
      switch (viakooarray[i]->feature_accessible)
      {
         case 1:
         {
            // must be filenum 0, because apertures are global.
            refname.Format("$$ACCESS_%d",++accesscnt);

            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 viakooarray[i]->access_top_x, viakooarray[i]->access_top_y, 0.0, 0, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);
            if (!taNetArray[viakooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[viakooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(viakooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            int prior = GetTestPriority_Via(i,1);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (viakooarray[i]->outline_top_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(viakooarray[i]->outline_top_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }
         }
         break;
         case 2:
         {
            refname.Format("$$ACCESS_%d",++accesscnt);
            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 viakooarray[i]->access_bottom_x, viakooarray[i]->access_bottom_y, 0.0, 1, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);

            if (!taNetArray[viakooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[viakooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(viakooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            int prior = GetTestPriority_Via(i,2);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (viakooarray[i]->outline_bottom_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(viakooarray[i]->outline_bottom_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }
         }
         break;
         case 3:
         {
            // top 
            refname.Format("$$ACCESS_%d",++accesscnt);
            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 viakooarray[i]->access_top_x, viakooarray[i]->access_top_y, 0.0, 0, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);
            if (!taNetArray[viakooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[viakooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(viakooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            int prior = GetTestPriority_Via(i,1);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (viakooarray[i]->outline_top_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(viakooarray[i]->outline_top_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            // bottom
            refname.Format("$$ACCESS_%d",++accesscnt);
            d = Graph_Block_Reference(testaccessblock->getName(), refname, 0, 
                                 viakooarray[i]->access_bottom_x, viakooarray[i]->access_bottom_y, 0.0, 1, 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeTestAccessPoint);
            if (!taNetArray[viakooarray[i]->netindex]->unused_net)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
                  VT_STRING,
                  taNetArray[viakooarray[i]->netindex]->netname.GetBuffer(0),  // this makes a "real" char *
                  SA_APPEND, NULL);
            }

            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
               VT_INTEGER,
               &(viakooarray[i]->entitynumber),  // this makes a "real" char *
               SA_APPEND, NULL);

            prior = GetTestPriority_Via(i,2);
            if (prior > -1)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TESTPREFERENCE, 1),
                     VT_INTEGER,
                     &prior,   // 
                     SA_APPEND, NULL);
            }

            if (viakooarray[i]->outline_bottom_distance < DBL_MAX)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_ACCESS_DISTANCE, 1),
                  VT_UNIT_DOUBLE,
                  &(viakooarray[i]->outline_bottom_distance),   // this makes a "real" char *
                  SA_APPEND, NULL);
            }
         }
         break;
         default:
            // 
            fprintf(taLog,"Error in test probe layer\n");
            taDisplayError++;
            continue;
         break;
      }
   }
}

/******************************************************************************
* TA_UpdateDatabase_TESTACCESS
*/
static void TA_UpdateDatabase_TESTACCESS(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   // delete all old access graphic
   doc->UnselectAll(TRUE);
   CWaitCursor w;  // must be after unselectAll

   // need to make a component outline for every component
   DeleteTestAccessData(doc, &(pcbfile->getBlock()->getDataList())); 
   // delete test access in the netlist
   DeleteTestAccessNetlist(doc, &(pcbfile->getNetList()));

   // this deletes the component outline
   //TA_DeleteTestGraphic(doc, pcbfile, &(pcbfile->getBlock()->getDataList())); 

   update_allvias_TESTACCESS(doc, &(pcbfile->getBlock()->getDataList()));
   update_otherpinloc_TESTACCESS(doc, &pcbfile->getNetList(), &(pcbfile->getBlock()->getDataList()));

   create_TESTACCESS_insert(doc, pcbfile, &(pcbfile->getBlock()->getDataList()));
   doc->UpdateAllViews(NULL);
}

/******************************************************************************
* TA_DebugReport 
*/
static void TA_DebugReport(const char *filename, CCEtoODBDoc *doc)
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

   fprintf(fp,"P/V,E,C,P,X,Y,CL,SM,NA,PA,FA,LA,NI,SMD,TEST,STD,SBD,FTD,FBD,TA,TP,Re,PA,CO,BO,PS\n");
   for (i=0;i<pinkoocnt;i++)
      fprintf(fp,"P,%ld,%s,%s,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
         pinkooarray[i]->entitynumber,
         pinkooarray[i]->compname,
         pinkooarray[i]->pinname,
         pinkooarray[i]->x,
         pinkooarray[i]->y,
         pinkooarray[i]->cadlayer,
         pinkooarray[i]->soldermask,
         pinkooarray[i]->net_access,
         pinkooarray[i]->physical_accessible,
         pinkooarray[i]->feature_accessible,
         pinkooarray[i]->layer,
         pinkooarray[i]->netindex,
         pinkooarray[i]->smd,       
         pinkooarray[i]->test,
         pinkooarray[i]->soldermasktop_deleted,
         pinkooarray[i]->soldermaskbottom_deleted,
         pinkooarray[i]->featuresizetop_deleted,
         pinkooarray[i]->featuresizebottom_deleted,
         pinkooarray[i]->test_access,
         pinkooarray[i]->test_preference,
         pinkooarray[i]->result, 
         pinkooarray[i]->probe_assigned,
         pinkooarray[i]->compoutline,     
         pinkooarray[i]->boardoutline,    
         pinkooarray[i]->padstackindex);
   
   for (i=0;i<viakoocnt;i++)
      fprintf(fp,"V,%ld,%s,%s,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
         viakooarray[i]->entitynumber,
         "",
         "",
         viakooarray[i]->x,
         viakooarray[i]->y,
         viakooarray[i]->cadlayer,
         viakooarray[i]->soldermask,
         viakooarray[i]->net_access,
         viakooarray[i]->physical_accessible,
         viakooarray[i]->feature_accessible,
         viakooarray[i]->layer,
         viakooarray[i]->netindex,
         viakooarray[i]->smd,       
         viakooarray[i]->test,
         viakooarray[i]->soldermasktop_deleted,
         viakooarray[i]->soldermaskbottom_deleted,
         viakooarray[i]->featuresizetop_deleted,
         viakooarray[i]->featuresizebottom_deleted,
         viakooarray[i]->test_access,
         viakooarray[i]->test_preference,
         viakooarray[i]->result, 
         viakooarray[i]->probe_assigned,
         viakooarray[i]->compoutline,     
         viakooarray[i]->boardoutline,    
         viakooarray[i]->padstackindex);     


   fclose(fp);

   Notepad(filename);
}
