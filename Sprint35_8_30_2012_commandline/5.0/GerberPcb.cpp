// $Header: /CAMCAD/5.0/GerberPcb.cpp 52    3/12/07 12:41p Kurt Van Ness $
  
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

// GERBERPCB.CPP
/****************************************************************************/
/*
   need to do:

   padstack generation needs to react to drill.
   tracing needs to look at
      width
      crossing traces
      power planes
      connections inside copper plane / void
      preexisting netlist - protel/ipc356/mentor neutral

   Need to check for shorts - gerber vs loaded netlist

   Not done yet
   Aperture other than round and square
   Aperture offset
   complex aperture

   Layer groups for blind and buried vias
   round vs sqaure

   need to understand the idea to convert everything into polygons and test then against each other

*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "gpcbdlgs.h"   // DIAL
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "graph.h"
#include "find.h"
#include <math.h>
#include <float.h>
#include "draw.h"
#include "crypt.h"
#include "net_util.h"
#include "measure.h"
#include "gerberpcb.h"
#include "apertur2.h"
#include "rgn.h"
#include "region.h"
#include "polylib.h"
#include "extents.h"
//#include "RwLib.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern   BOOL HitLine(Point2 *start, Point2 *end, double width, double x, double y, double range); // from search.cpp


/* Static Variables ***************************************************************************/

static   CAppkooArray   appkooarray;   // used in padstack and derive
static   int            appkoocnt;

static   CDrillkooArray drillkooarray; // used in padstack and derive
static   int            drillkoocnt;

static   CPadnameArray  padnamearray;  // used in padstack and derive
static   int            padnamecnt;

static   CPadstackConvertArray   padstackconvertarray;   // used in convert_padstacks_pins_via
static   int            padstackconvertcnt;

static   CCompPinkooArray  comppinkooarray;  // used to assign netnames from cad files to gerber files.
static   int            comppinkoocnt;

void CombineTouchingApertures(CCEtoODBDoc *doc, CDataList *dataList); 
static void OptimizeTouchingApertures(CCEtoODBDoc *doc, CDataList *dataList);

/******************************************************************************
* count_visible_files
*/
static int count_visible_files(CCEtoODBDoc *doc)
{
   FileStruct *file;
   int         fcnt = 0;
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())
         continue;
      fcnt++;  
   }
   return fcnt;
}

/******************************************************************************
* get_masks
*/
static int get_masks(CCEtoODBDoc *doc, int layer)
{
   LayerStruct *l = doc->FindLayer(layer);

   if (l)
   {
      if (l->getLayerType() == LAYTYPE_MASK_ALL)
         return 3;
      if (l->getLayerType() == LAYTYPE_MASK_TOP)
         return 1;
      if (l->getLayerType() == LAYTYPE_MASK_BOTTOM)
         return 2;

      if (l->getLayerType() == LAYTYPE_PASTE_ALL)
         return 12;
      if (l->getLayerType() == LAYTYPE_PASTE_TOP)
         return 4;
      if (l->getLayerType() == LAYTYPE_PASTE_BOTTOM)
         return 8;
   }

   return 0;
}

/******************************************************************************
* get_topbottom
*/
static int get_topbottom(CCEtoODBDoc *doc, int layer)
{
   int soldermask = 0;
   LayerStruct *l = doc->FindLayer(layer);

   if (l)
   {
      if (l->getLayerType() == LAYTYPE_PAD_TOP)
         return 1;
      if (l->getLayerType() == LAYTYPE_SIGNAL_TOP)
         return 1;
      if (l->getLayerType() == LAYTYPE_PAD_BOTTOM)
         return 2;
      if (l->getLayerType() == LAYTYPE_SIGNAL_BOT)
         return 2;
   }

   return 0;
}

/******************************************************************************
* GetALLAPERTUREData
*/
static void GetALLAPERTUREData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
                               double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)

{
   int viacnt = 0;
   Attrib *a;

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);
      
      if (data->getDataType() != T_INSERT)
         continue;
      if (data->getLayerIndex() < 0)
         continue;

      if (!doc->get_layer_visible(data->getLayerIndex(), mirror))
         continue;

      switch(data->getDataType())
      {
         case T_INSERT:
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
               double block_rot = rotation + data->getInsert()->getAngle();

               Mat2x2 m;
               RotMat2(&m, block_rot);

               if (mirror)
                  block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

               Point2 point2;
               point2.x = data->getInsert()->getOriginX() * scale;
               if (mirror)
                  point2.x = -point2.x;
               point2.y = data->getInsert()->getOriginY() * scale;

               BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

               if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               {
                  // here update appkooarray
                  GPCB_appkoo *c = new GPCB_appkoo;
                  appkooarray.SetAtGrow(appkoocnt++, c);  
                  c->x = point2.x;
                  c->y = point2.y;
                  c->layer = data->getLayerIndex(); 
                  c->data = data;
                  c->padstackid = -1;
                  c->curid = -1;
                  c->bnum = data->getInsert()->getBlockNumber();
                  c->drill = 0;  // this flag is assigned in process_drillinfo
                  c->masks = get_masks(doc, data->getLayerIndex());
                  c->topbottom = get_topbottom(doc, data->getLayerIndex());
                  c->freepad = NULL;
                  c->pad_usage = 0;

                  if (a = is_attvalue(doc, data->getAttributesRef(), ATT_PAD_USAGE, 0))
                     c->pad_usage = a->getIntValue();
               }
               else if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
               {
                  // here update drillkooarray
                  GPCB_drillkoo *c = new GPCB_drillkoo;
                  drillkooarray.SetAtGrow(drillkoocnt++, c);  
                  c->x = point2.x;
                  c->y = point2.y;
                  c->bnum = data->getInsert()->getBlockNumber();
                  c->data = data;
                  c->used = FALSE;
                  c->layer = data->getLayerIndex(); 
                  c->pad_usage = 0;

                  if (a = is_attvalue(doc, data->getAttributesRef(), ATT_PAD_USAGE, 0))
                     c->pad_usage = a->getIntValue();
               }
            } 
            break;
      } 
   } 
}

/******************************************************************************
* update_drillmode_1
   Update drill if drill was found
*/
static int update_drillmode_1(double tolerance, int startmask1)
{
   if (drillkoocnt == 0)
   {
      CString mess = "No Drill Information found !\n\
Without Drill Info, only SMD padstacks are generated.\n\nDo you want do continue?";

      if (ErrorMessage(mess, "Warning: Missing Drill Information !", MB_YESNO | MB_DEFBUTTON2) == IDNO)
         return -1;
      else
         return 1;   
   }

   // here put all padstack together which are potential connected
   long cur_id = 0;
   for (int d=0; d<drillkoocnt; d++)
   {
      GPCB_drillkoo *drillkoo = drillkooarray[d];
      cur_id++;

      for (int i=0; i<startmask1; i++)
      {
         GPCB_appkoo *appkoo = appkooarray[i];

         if (appkoo->masks)
            continue; // masks are done in process_mask in generate_padstack
         if (appkoo->curid > -1)
            continue; // do not do one which is used

         if (fabs(appkoo->x - drillkoo->x) < tolerance && fabs(appkoo->y - drillkoo->y) < tolerance)
         {
            appkoo->curid = cur_id;
            appkoo->drill = d + 1;
            drillkoo->used = TRUE;
         }
      }
   }

   // here are now padstacks with do not have cur_id, because they are SMD without a padstack
   for (int i=0; i<startmask1; i++)
   {
      GPCB_appkoo *appkoo = appkooarray[i];

      if (appkoo->masks)
         continue; // masks are done in process_mask in generate_padstack
      if (appkoo->curid > -1)
         continue; // do not do one which is used

      appkoo->curid = ++cur_id;
   }

   return 1;
}

/******************************************************************************
* update_drillmode_2
   use layer as drill
*/
static int update_drillmode_2(double tolerance, int drilllayer, int startmask1)
{
   int cur_id = 0;

   // here put all padstack together which are potential connected
	int i=0;
   for (i=0; i<startmask1; i++)
   {
      GPCB_appkoo *appkoo1 = appkooarray[i];

      if (appkoo1->masks)
         continue; // masks are done in process_mask in generate_padstack
      if (appkoo1->curid > -1)
         continue; // do not do one which is used
      if (appkoo1->layer != drilllayer)
         continue; // skip if a aperture is not on a drill layer

      appkoo1->curid = ++cur_id;

      for (int d=0; d<startmask1; d++)
      {
         GPCB_appkoo *appkoo2 = appkooarray[d];

         if (appkoo2->masks)
            continue; // masks are done in process_mask in generate_padstack
         if (appkoo2->layer == drilllayer)
            continue; // skip if a aperture is on a drill layer
         if (appkoo2->curid > -1)
            continue; // do not do one which is used
         if (appkoo1->pad_usage != appkoo2->pad_usage)
            continue;

         if (fabs(appkoo1->x - appkoo2->x) < tolerance && fabs(appkoo1->y - appkoo2->y) < tolerance )
            appkooarray[d]->curid = appkooarray[i]->curid;
      }
   }

   // here are now padstacks with do not have cur_id, because they are SMD without a padstack
   for (i=0; i<startmask1; i++)
   {
      GPCB_appkoo *appkoo = appkooarray[i];

      if (appkoo->masks)
         continue; // masks are done in process_mask in generate_padstack
      if (appkoo->curid > -1)
         continue; // do not do one which is used

      appkoo->curid = ++cur_id;
   }

   return 1;
}

/******************************************************************************
* update_drillmode_3
   Assume drill if drill is 2 apertures are on any layer matching. This off-course
   will not work on most SMD designs
*/
static int update_drillmode_3(double tolerance, int startmask1)
{
   int cur_id = 0;

   // here put all padstack together which are potential connected
	int i=0;
   for (i=0; i<startmask1; i++)
   {
      GPCB_appkoo *appkoo1 = appkooarray[i];

      if (appkoo1->curid > -1)
         continue; // do not do one which is used
      if (appkoo1->masks)
         continue; // masks are done in process_mask in generate_padstack

      appkoo1->curid = ++cur_id;

      for (int d=i+1; d<startmask1; d++)
      {
         GPCB_appkoo *appkoo2 = appkooarray[d];

         if (appkoo2->masks)
            continue; // masks are done in process_mask in generate_padstack
         if (appkoo1->pad_usage != appkoo2->pad_usage)
            continue;
         if (appkoo1->layer == appkoo2->layer) 
            continue; // must be on different layer

         if (fabs(appkoo1->x - appkoo2->x) < tolerance && fabs(appkoo1->y - appkoo2->y) < tolerance)  
         {
            appkoo2->curid = appkoo1->curid;
            break;
         }
      }
   }

   // here are now padstacks with do not have cur_id, because they are SMD without a padstack
   for (i=0; i<startmask1; i++)
   {
      GPCB_appkoo *appkoo = appkooarray[i];

      if (appkoo->masks)
         continue; // masks are done in process_mask in generate_padstack
      if (appkoo->curid > -1)
         continue; // do not do one which is used

      appkoo->curid = ++cur_id;
   }

   return 1;
}

/******************************************************************************
* AppkooMaskCompareFunc
*/
static int AppkooMaskCompareFunc( const void *arg1, const void *arg2 )
{
   GPCB_appkoo **a1 = (GPCB_appkoo**)arg1;
   GPCB_appkoo **a2 = (GPCB_appkoo**)arg2;

   return ((*a1)->masks - (*a2)->masks);
}

/******************************************************************************
* sort_appkoo_by_mask
*/
static int sort_appkoo_by_mask()
{
   qsort( appkooarray.GetData(), appkoocnt, sizeof(GPCB_appkoo *), AppkooMaskCompareFunc);
   return 1;
}

/******************************************************************************
* process_drillinfo
   here now assign the drill flag on the appkooarray
*/
static int process_drillinfo(CCEtoODBDoc *doc, double tolerance, int drillmode, int drillayer)
{
   // sort by mask
   sort_appkoo_by_mask();

   // get start mask, end mask
   int startmask = -1;
   int endmask = -1;

   for (int i=0; i<appkoocnt; i++)
   {
      if (startmask < 0 && appkooarray[i]->masks)
         startmask = i;
      if (appkooarray[i]->masks)
         endmask = i;
   }

   // mask is always after the no mask entries.
   int startmask1 = startmask;   // is the start of the mask or appkoocnt
   if (startmask < 0) 
   {
      startmask = 0;
      startmask1 = appkoocnt;
   }

   if (endmask < 0)     
      endmask = appkoocnt;

   switch (drillmode)
   {
      case 0:  // use drill info
         {
            // checks for drill holes.
            if (update_drillmode_1(tolerance, startmask1) < 0)
               return -1;
         }
         break;
      case 1:  // use a layer as drill
         {
            // looks for a aperture on a defined layer
            if (update_drillmode_2(tolerance, drillayer, startmask1) < 0)
               return -1;
         }
         break;
      case 2:  // assume drill
         {
            // assume drill is if 2 apertures are on the same xy koo on different layers
            if (update_drillmode_3(tolerance, startmask1) < 0)
               return -1;
         }
         break;
   }

   return 1;
}

/******************************************************************************
* process_masks
*/
static int process_masks(FILE *flog, double tolerance)
{
   // sort by mask
   sort_appkoo_by_mask();

   // get start mask, ent mask
   int startmask = -1;
   int endmask = -1;
	int i=0; 
   for (i=0; i<appkoocnt; i++)
   {
      if (startmask < 0 && appkooarray[i]->masks)
         startmask = i;
      if (appkooarray[i]->masks)
         endmask = i;
   }

   int startmask1 = startmask;
   if (startmask < 0)   
   {
      startmask = 0;
      startmask1 = appkoocnt;
   }

   if (endmask < 0)     
      endmask = appkoocnt;

   // here put all padstack together which are potential connected
   for (i=startmask; i<endmask; i++)
   {
      GPCB_appkoo *appkoo1 = appkooarray[i];

      // here find nonmasks apertures
      // mask 16, 32 is top, bottom copper layers
      if (appkoo1->masks)
         continue;       

      // Here now is a app koo without a mask
      // Now find a mask without already being used.
      for (int ii=0; ii<startmask1; ii++)
      {
         GPCB_appkoo *appkoo2 = appkooarray[ii];

         if (!appkoo2->masks)
            continue;       
         if (appkoo2->curid > -1)
            continue; // do not do one which is used

         if (fabs(appkoo1->x - appkoo2->x) < tolerance && fabs(appkoo1->y - appkoo2->y) < tolerance)
         {
            // a soldermask all must be always done
            // a pastemask all must be always done
            // a soldermask_top is only done if a layer top is in the 
            // 1 = top, 2 = bottom, 3 = all for Soldermask
            // 4 = top, 8 = bottom, 12 = all for pastemask

            int ok = FALSE;
            if ((appkoo1->topbottom & 1) && (appkoo2->masks & 1))
               ok = TRUE;
            if ((appkoo1->topbottom & 2) && (appkoo2->masks & 2))
               ok = TRUE;
            if ((appkoo1->topbottom & 1) && (appkoo2->masks & 4))
               ok = TRUE;
            if ((appkoo1->topbottom & 2) && (appkoo2->masks & 8))
               ok = TRUE;

            if (ok)
               appkooarray[ii]->curid = appkoo1->curid;
         }
      }
   }
   return 1;
}

/******************************************************************************
* collect_aperture_array
*/
static int collect_aperture_array(FILE *flog, CCEtoODBDoc *doc, CProgressDlg *progress, double tolerance, int drillmode, int drillayer)
{
   FileStruct *file;
   int fcnt = 0;

   progress->SetStatus("Collect Apertures...");
   CTime t = CTime::GetCurrentTime();
   fprintf(flog, " Start Collect : %s\n", t.Format("%a %b %d %I:%M%p"));

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())
         continue;

      doc->PrepareAddEntity(file);        
      GetALLAPERTUREData(doc, file, &(file->getBlock()->getDataList()), 0.0, 0.0 ,  0.0, file->isMirrored(), file->getScale(), 0, -1);

      t = CTime::GetCurrentTime();
      fprintf(flog, "Start Process Drill Mode (%d) : %s\n", drillmode, t.Format("%a %b %d %I:%M%p"));

      // process_drillinfo does the aperture curid update.
      if (process_drillinfo(doc, tolerance, drillmode, drillayer) < 0)
         return -1;

      t = CTime::GetCurrentTime();
      fprintf(flog, "End Process Drill : %s\n", t.Format("%a %b %d %I:%M%p"));

   }

   t = CTime::GetCurrentTime();
   fprintf(flog, "End Collect : %s\n", t.Format("%a %b %d %I:%M%p"));

   return fcnt;
}

/******************************************************************************
* AppkooLayerCompareFunc
*/
static int AppkooLayerCompareFunc( const void *arg1, const void *arg2 )
{
   GPCB_appkoo **a1 = (GPCB_appkoo**)arg1;
   GPCB_appkoo **a2 = (GPCB_appkoo**)arg2;

   return ((*a1)->layer - (*a2)->layer);
}

/******************************************************************************
* AppkooEntityCompareFunc
*/
static int AppkooEntityCompareFunc( const void *arg1, const void *arg2 )
{
   GPCB_appkoo **a1 = (GPCB_appkoo**)arg1;
   GPCB_appkoo **a2 = (GPCB_appkoo**)arg2;

   return ((*a1)->data->getEntityNumber() - (*a2)->data->getEntityNumber());
}

/******************************************************************************
* sort_appkoo_by_layer
*/
static int sort_appkoo_by_layer()
{
   qsort(appkooarray.GetData(), appkoocnt, sizeof(GPCB_appkoo *), AppkooLayerCompareFunc);
   return 1;
}

/******************************************************************************
* sort_appkoo_by_entitynumber
*/
static int sort_appkoo_by_entitynumber()
{
   qsort(appkooarray.GetData(), appkoocnt, sizeof(GPCB_appkoo *), AppkooEntityCompareFunc);
   return 1;
}

/******************************************************************************
* get_padnameptr
*/
static int get_padnameptr(const char *p)
{
   for (int i=0;i<padnamecnt;i++)
   {
      if (p == padnamearray[i]->stackname)
         return i;
   }

   GPCB_padname *c = new GPCB_padname;
   padnamearray.SetAtGrow(padnamecnt++, c);  
   c->stackname = p;

   return padnamecnt-1;
}

/******************************************************************************
* save_padstacks
*/
static int save_padstacks(CCEtoODBDoc *doc, int filenum)
{
   for (int i=0; i<padnamecnt; i++)
   {
      CString name;
      name.Format("$$PADSTACK_%d", i);

      BlockStruct *curblock = Graph_Block_On(GBO_APPEND, name, filenum, 0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);
      Graph_Block_Off();

      char *tmp = STRDUP(padnamearray[i]->stackname);
      char *lp = strtok(tmp, ",");
      int pcnt = atoi(lp);

      lp = strtok(NULL, ",");
      int drillindex = atoi(lp);

      lp = strtok(NULL,",");
      int dnum = atoi(lp);

      for (int ii=0; ii<pcnt; ii++)
      {
         lp = strtok(NULL, ",");
         int lnum = atoi(lp);

         lp = strtok(NULL, ",");
         int bnum = atoi(lp);

         BlockStruct *block = doc->Find_Block_by_Num(bnum);
         BlockStruct *curblock = Graph_Block_On(GBO_APPEND, name, -1 ,0);
         Graph_Block_Reference(block->getName(), NULL, -1, 0.0, 0.0, 0.0, 0, 1.0, lnum, TRUE);
         Graph_Block_Off();
      }

      if (drillindex)
      {
         BlockStruct *block = doc->Find_Block_by_Num(drillindex);
         BlockStruct *curblock = Graph_Block_On(GBO_APPEND, name, -1, 0);
         Graph_Block_Reference(block->getName(), NULL, -1, 0.0, 0.0, 0.0, 0, 1.0, dnum, TRUE);
         Graph_Block_Off();   
      }

      free(tmp);
   }

   return 1;
}

/******************************************************************************
* delete_exchanged_apertures
*/
static void delete_exchanged_apertures(CCEtoODBDoc *doc, CProgressDlg *progress, FileStruct *file,  CDataList *DataList)
{
   progress->SetStatus("Sort Apertures by Entities...");
   sort_appkoo_by_entitynumber();

   progress->SetStatus("Exchange Apertures with Padstacks...");
   CWaitCursor hourglass;

   // watch out, we can not guarantee that np->entities are in any order !!!
	int i=0;
   for (i=0; i<appkoocnt; i++)
   {
      GPCB_appkoo *appkoo = appkooarray[i];

      // aperture is not converted to a padstack!
      if (appkoo->freepad == NULL)
         continue;

      doc->CopyAttribs(&appkoo->freepad->getAttributesRef(), appkoo->data->getAttributesRef());
      RemoveOneEntityFromDataList(doc, DataList, appkoo->data);
   }

   for (i=0; i<drillkoocnt; i++)
   {
      GPCB_drillkoo *drillkoo = drillkooarray[i];

      if (drillkoo->used == 0)
         continue;
      if (drillkoo->data)
         RemoveOneEntityFromDataList(doc, DataList, drillkoo->data);
   }

   return;
}

/****************************************************************************/
/*
   All apertures are already sorted by layer
*/
static int generate_padstacks(FILE *flog, CCEtoODBDoc *doc, CProgressDlg *progress, 
                              double tolerance, FileStruct *file)
{
   int            i, ii;
   int            cur_id = 0;
   GPCB_padstack  tmppadstack[MAX_GPCBLAYERS+1];
   int            tmppadstackcnt;

   progress->SetStatus("Generate Padstacks...");

   CTime t = CTime::GetCurrentTime();
   fprintf(flog,"Start Process Mask: %s\n", t.Format("%a %b %d %I:%M%p"));
   process_masks(flog, tolerance);
   t = CTime::GetCurrentTime();
   fprintf(flog,"End Process Mask : %s\n", t.Format("%a %b %d %I:%M%p"));

   sort_appkoo_by_layer();

   // collect all same padstack ids
   for (i=0;i<appkoocnt;i++)
   {
      if (appkooarray[i]->padstackid > -1)   continue; // do not do one which is used
      if (appkooarray[i]->curid < 0)         continue; // do not do one which is used
      tmppadstackcnt = 0;
      tmppadstack[tmppadstackcnt].drill = appkooarray[i]->drill;
      tmppadstack[tmppadstackcnt].layer = appkooarray[i]->layer;
      tmppadstack[tmppadstackcnt].bnum = appkooarray[i]->bnum;
      tmppadstack[tmppadstackcnt].index = i;
      tmppadstackcnt++;

      for (ii=i+1;ii<appkoocnt;ii++)
      {
         if (appkooarray[ii]->curid != appkooarray[i]->curid)  
            continue; // do not do one which is used

         tmppadstack[tmppadstackcnt].layer = appkooarray[ii]->layer;
         tmppadstack[tmppadstackcnt].bnum = appkooarray[ii]->bnum;
         tmppadstack[tmppadstackcnt].index = ii;
         tmppadstackcnt++;
      }

      CString  pname;
      pname.Format("%d,%d,%d",tmppadstackcnt, 
         (tmppadstack[0].drill > 0)?drillkooarray[tmppadstack[0].drill-1]->bnum:0, 
         (tmppadstack[0].drill > 0)?drillkooarray[tmppadstack[0].drill-1]->layer:0);

      // here is now a tmp padstack, which says how many layers one padstack has.
		int tt=0;
      for (tt=0;tt<tmppadstackcnt;tt++)
      {
         CString  tmp;
         tmp.Format(",%d,%d", tmppadstack[tt].layer ,tmppadstack[tt].bnum);
         pname += tmp;
      }
      
      int padnameptr = get_padnameptr(pname);
      // check against an existing padstack
      pname.Format("$$PADSTACK_%d",padnameptr);
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,pname, file->getFileNumber(),0);
      Graph_Block_Off();

      // place i
      DataStruct *d = Graph_Block_Reference(pname, "", file->getFileNumber(), 
               appkooarray[i]->x, appkooarray[i]->y, 0.0, 0 , 1.0, -1, TRUE);
      d->getInsert()->setInsertType(insertTypeFreePad);

      // mark all found with the padstack id.
      for (tt=0;tt<tmppadstackcnt;tt++)
      {
         appkooarray[tmppadstack[tt].index]->padstackid = padnameptr;   
         appkooarray[tmppadstack[tt].index]->freepad = d;
      }
   }

   save_padstacks(doc, file->getFileNumber());

   for (i=0;i<padnamecnt;i++)
   {
      CString pname;
      pname.Format("$$PADSTACK_%d",i);
      fprintf(flog, "Index %d: Padstack [%s] Stackname [%s] created\n", i, pname, padnamearray[i]->stackname);
   }
/*
   for (i=0;i<appkoocnt;i++)
   {
      fprintf(flog,"X %g Y %g Entity %ld Layer %d Pad %d Block %d Drill %d Padname %s\n",
         appkooarray[i]->x, appkooarray[i]->y, appkooarray[i]->getEntityNumber(), 
         appkooarray[i]->layer, appkooarray[i]->padstackid, appkooarray[i]->bnum,
         appkooarray[i]->drill,
         (appkooarray[i]->padstackid > -1)?padnamearray[appkooarray[i]->padstackid]->stackname:"NULL");
      display_error++;
   }
*/
   return 1;
}

/*****************************************************************************/
/*
   If a packstack is used in a Component, kill it, or convert it to a via.
*/
static void delete_comppin_padstacks(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList)
{
   int   i;

   for (i=0;i<padstackconvertcnt;i++)
   {
      if (padstackconvertarray[i]->comppin)
      {
         RemoveOneEntityFromDataList(doc, DataList, padstackconvertarray[i]->datastruct);
      }
      else
      {
         padstackconvertarray[i]->datastruct->getInsert()->setInsertType(insertTypeVia);
      }
   }

   return;
}

/*****************************************************************************/
/*
   here I run through the netlist to find which padstacks are touching pins.
*/
static int attach_padstacks_netlist(CCEtoODBDoc *doc, FileStruct *file, double tol)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   int      pinfound = 0;


   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (compPin->getPinCoordinatesComplete())
         {
            for (int i=0;i<padstackconvertcnt;i++)
            {
               if (padstackconvertarray[i]->comppin)  continue; // already attached.

               // we need to find the component layer and check that the padstack has 
               // a access on the compennt placement layer.

               if (fabs(compPin->getOriginX() - padstackconvertarray[i]->datastruct->getInsert()->getOriginX()) < tol &&
                   fabs(compPin->getOriginY() - padstackconvertarray[i]->datastruct->getInsert()->getOriginY()) < tol )
               {
                  padstackconvertarray[i]->comppin = compPin;
                  pinfound++;
                  break; // only 1 padstack can touch
               }
            }
         }
      }
   }

   return pinfound;
}

//--------------------------------------------------------------
static void collect_padstacks(CCEtoODBDoc *doc, FileStruct *file,  CDataList *DataList)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)  continue;
      if (np->getInsert()->getInsertType() == 0 || np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
      {
         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
         if (block->getBlockType() == BLOCKTYPE_PADSTACK)
         {
            GPCB_padstack_convert *p = new GPCB_padstack_convert;
            padstackconvertarray.SetAtGrow(padstackconvertcnt,p);  
            padstackconvertcnt++;   
            p->datastruct = np;
            p->comppin = NULL;
            p->blockstruct = block;
         }
      }

   } // end collect_padstacks */

   return;
}

/****************************************************************************/
/*
   the concept is:
   1. collect all padstacks
   2. attach padstacks to netlist
   3. attach padstacks to pcbcomponents (watch out for mirrored etc...)
   4. assign vias, testpoints, fiducials if possible.
*/
static int convert_padstacks_pins_via(CCEtoODBDoc *doc, CProgressDlg *progress, double tol)
{
   int   cnt;
   int   i;

   cnt = count_visible_files(doc);

   if (cnt == 0)
   {
      MessageBox(NULL, "No visible file", NULL, MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }
   else
   if (cnt  != 1)
   {
      CString  tmp;

      tmp.Format("Only 1 file is allowed to be visible.\n\
1. Merge Files, which should be part of PadStack generation\n\
2. Switch Off Files, which should not be part of PadStack generation.\n");

      MessageBox(NULL, tmp, "Too many visible files", MB_ICONEXCLAMATION | MB_OK);
   
      return -1;
   }

   // here set file to active !!!
   // only 1 file was allowed up to this point
   FileStruct *file;
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      doc->PrepareAddEntity(file);     
      break;
   }

   file->setBlockType(blockTypePcb);

// here do the different runs. 
   CString gpcbLogFile = GetLogfilePath(GERBERPCBLogFile);
   FILE *flog;

   if ((flog = fopen(gpcbLogFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", GERBERPCBLogFile, MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }
   int display_error = 0;

   CTime t = CTime::GetCurrentTime();
   fprintf(flog,"Start : %s\n", t.Format("%a %b %d %I:%M%p"));

   padstackconvertarray.SetSize(100,100);
   padstackconvertcnt = 0;

   // collect padstacks instances
   collect_padstacks(doc, file, &(file->getBlock()->getDataList()));

   if (padstackconvertcnt)
   {
      fprintf(flog,"Numbers of padstacks found : %d\n", padstackconvertcnt);
      display_error++;

      // check how many can be assigned to a netlist !
      generate_PINLOC( doc, file, 0);        // this function generates the PINLOC argument for all pins.
      generate_PADSTACKACCESSFLAG(doc, 0); 

      int found = attach_padstacks_netlist(doc, file, tol);

      delete_comppin_padstacks(doc, file, &(file->getBlock()->getDataList()));   // deletes comppin padstacks from database and converts the others to vias.
   }

   for (i=0;i<padstackconvertcnt;i++)
   {
      delete padstackconvertarray[i];
   }
   padstackconvertarray.RemoveAll();

   t = CTime::GetCurrentTime();
   fprintf(flog,"End : %s\n", t.Format("%a %b %d %I:%M%p"));

   fclose(flog);

#ifdef _DEBUG
   if (display_error)
      Notepad(GetLogfilePath(GERBERPCBLogFile));
#endif

   return 1;
}

/****************************************************************************/
/*
   drilllayer only evaluated in drillmode 1
*/
int exchange_padstacks(FILE *flog, CCEtoODBDoc *doc, CProgressDlg *progress, double tol, 
                       int drillmode, int drillayer)
{
   int   cnt;

   cnt = count_visible_files(doc);

   if (cnt == 0)
   {
      MessageBox(NULL, "No visible file", NULL, MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }
   else
   if (cnt  != 1)
   {
      CString  tmp;

      tmp.Format("Only 1 file is allowed to be visible.\n\
1. Merge Files, which should be part of PadStack generation\n\
2. Switch Off Files, which should not be part of PadStack generation.\n");

      MessageBox(NULL, tmp, "Too many visible files", MB_ICONEXCLAMATION | MB_OK);
   
      return -1;
   }

   progress->SetStatus("Collect Padstacks...");

   // here set file to active !!!
   // only 1 file was allowed up to this point
   FileStruct *file = NULL;
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      doc->PrepareAddEntity(file);     
      break;
   }

   file->setBlockType(blockTypePcb);

   CTime t = CTime::GetCurrentTime();
   fprintf(flog,"Start : %s\n", t.Format("%a %b %d %I:%M%p"));

   drillkooarray.SetSize(100,100);
   drillkoocnt = 0;
   appkooarray.SetSize(100,100);
   appkoocnt = 0;
   padnamearray.SetSize(100,100);
   padnamecnt = 0;

   // collect apertures and assign cur_id dependend on drillmode
   if (collect_aperture_array(flog, doc, progress, tol, drillmode, drillayer) > -1)
   {
      if (appkoocnt)
      {
         fprintf(flog,"Numbers of visible apertures found : %d\n", appkoocnt);
         //display_error++;
         // generate padstacks

         CTime t = CTime::GetCurrentTime();

         fprintf(flog," Start Sort all apertures : %s\n", t.Format("%a %b %d %I:%M%p"));
      
         // sort all apertures by layer
         progress->SetStatus("Sort Apertures...");

         t = CTime::GetCurrentTime();
         fprintf(flog,"End Sort : %s\n", t.Format("%a %b %d %I:%M%p"));

         t = CTime::GetCurrentTime();
         fprintf(flog," Start Generate : %s\n", t.Format("%a %b %d %I:%M%p"));
         fprintf(flog,"PadStackName Number_of_Lay/Bnum, DrillBnum, DrillLayer, Layer, Bnum...\n");
         generate_padstacks(flog, doc, progress, tol, file);

         t = CTime::GetCurrentTime();
         fprintf(flog," End Generate : %s\n", t.Format("%a %b %d %I:%M%p"));

         // delete padstacks for apertures
         t = CTime::GetCurrentTime();
         fprintf(flog,"Start Exchange : %s\n", t.Format("%a %b %d %I:%M%p"));

         progress->SetStatus("Convert Apertures to Padstacks...");
         delete_exchanged_apertures(doc, progress, file, &(file->getBlock()->getDataList()));

         t = CTime::GetCurrentTime();
         fprintf(flog,"End Exchange : %s\n", t.Format("%a %b %d %I:%M%p"));
      }
      else
      {
         MessageBox(NULL,  "No Free Apertures found to Generate Padstacks!", "Generate Padstacks", MB_ICONEXCLAMATION | MB_OK);
      }
   }

   int   i;
   for (i=0;i<appkoocnt;i++)
   {
      delete appkooarray[i];
   }
   appkooarray.RemoveAll();

   for (i=0;i<drillkoocnt;i++)
   {
      delete drillkooarray[i];
   }
   drillkooarray.RemoveAll();

   for (i=0;i<padnamecnt;i++)
   {
      delete padnamearray[i];
   }
   padnamearray.RemoveAll();

   t = CTime::GetCurrentTime();
   fprintf(flog,"End : %s\n", t.Format("%a %b %d %I:%M%p"));

   return 1;
}

//--------------------------------------------------------------
static int find_comppinkoo(double range, double x, double y)
{
   int   i;

   for (i=0;i<comppinkoocnt;i++)
   {
      if (fabs(comppinkooarray[i]->x - x) < range && fabs(comppinkooarray[i]->y - y) < range)
      {
         return i;
      }
   }

   return -1;
}

//--------------------------------------------------------------
static void GetALLPADSTACKData(CCEtoODBDoc *doc, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)

{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   POSITION pos;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != INSERTTYPE_FREEPAD)           break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);
// loop this against the netlist
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

double range = 0.01;          

            int   pi;
            if ((pi = find_comppinkoo(range, point2.x, point2.y)) > -1)
            {
               // assign net
#ifdef _DEBUG
   GPCB_comppinkoo *cc = comppinkooarray[pi];
#endif
               comppinkooarray[pi]->used = TRUE;
               doc->SetUnknownAttrib(&np->getAttributesRef(),LAYATT_NETNAME, comppinkooarray[pi]->netname, SA_OVERWRITE, NULL);

            }
         } // case INSERT
         break;
      } // end switch
   } // end GetALLPADSTACKData */
}

//--------------------------------------------------------------
static int Get_PinLoc(CCEtoODBDoc *doc, FileStruct *gerberfile, CNetList *NetList, 
                     double file_x, double file_y, double file_angle, int file_mirror, double scale)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   Mat2x2 m;
   Point2 point2;

   RotMat2(&m, file_angle);

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      // update netnames
      NetStruct *nn = add_net(gerberfile, net->getNetName());
      nn->setFlags( net->getFlags());

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (compPin->getPinCoordinatesComplete())
         {
            double  pinx, piny, pinrot;
            int     padmirror;
            CString padstackname;

            //pinx = compPin->getOriginX();
            //piny = compPin->getOriginY();
            //pinrot = compPin->getRotationRadians();
            //padmirror = compPin->getMirror();

            point2.x = compPin->getOriginX();
            if (file_mirror)
               point2.x = -point2.x;

            point2.y = compPin->getOriginY();
         
            TransPoint2(&point2, 1, &m, file_x, file_y);

            pinx = point2.x;
            piny = point2.y;
            pinrot = compPin->getRotationRadians()+file_angle;
            padmirror = file_mirror ^ compPin->getMirror();

            if (compPin->getPadstackBlockNumber() > -1)
            {
               BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
               padstackname = block->getName();
            }

            GPCB_comppinkoo *c = new GPCB_comppinkoo;
            comppinkooarray.SetAtGrow(comppinkoocnt,c);  
            comppinkoocnt++;  
            c->netname = net->getNetName();
            c->comp = compPin->getRefDes();
            c->pin = compPin->getPinName();
            c->x = point2.x;
            c->y = point2.y;
            c->used = FALSE;
            c->layer = 3;     // all layers
         }
      }
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int compare_gerber_netlist(CCEtoODBDoc *doc, CProgressDlg *progress,
                                  FileStruct *gerberfile, FileStruct *cadfile)
{
   FILE *flog;
   if ((flog = fopen(GetLogfilePath(GERBERPCBLogFile), "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", GERBERPCBLogFile, MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }
   int display_error = 0;

   CTime t = CTime::GetCurrentTime();
   fprintf(flog,"Start : %s\n", t.Format("%a %b %d %I:%M%p"));

   progress->SetStatus("Merge Netlist and Artwork...");

   comppinkooarray.SetSize(100,100);
   comppinkoocnt = 0;

   // get and normalize the netlist
   Get_PinLoc(doc, gerberfile, &cadfile->getNetList(),  cadfile->getInsertX(), cadfile->getInsertY(),
             cadfile->getRotation(), cadfile->isMirrored(), cadfile->getScale());

   if (comppinkoocnt)
   {
      // assign Gerber padstacks with netnames from netlist
      GetALLPADSTACKData(doc, gerberfile, &(gerberfile->getBlock()->getDataList()), 
                    gerberfile->getInsertX(), gerberfile->getInsertY(),
                    gerberfile->getRotation(), gerberfile->isMirrored(), gerberfile->getScale(), 0, -1);

      // switch off the gerber 

      // here now run compare
      DeriveNetlist(doc, gerberfile);
   }
   else
   {
      ErrorMessage("No Pins in Netlist found!", "Gerber->Netlist");
   }

   int   i;

   for (i=0;i<comppinkoocnt;i++)
   {
      if (comppinkooarray[i]->used == FALSE)
      {
         fprintf(flog,"Component [%s] Pin [%s] could not find a FREEPAD at %lg:%lg\n",
            comppinkooarray[i]->comp,comppinkooarray[i]->pin, comppinkooarray[i]->x, comppinkooarray[i]->y);
         display_error++;
      }
      delete comppinkooarray[i];
   }
   comppinkooarray.RemoveAll();

   fclose(flog);

   Notepad(GetLogfilePath(GERBERPCBLogFile));

   //ErrorMessage("File setup correct", "Gerber -> Netlist Compare");
   return 1;
}

/******************************************************************************
* OnConvertPadstacksToPinsVias
*/
void CCEtoODBDoc::OnConvertPadstacksToPinsVias() 
{
   UnselectAll(FALSE);

   CWaitCursor hourglass;
   CProgressDlg *progress;

   progress = new CProgressDlg("Processing Padstack Generation...", FALSE);
   progress->Create();

   // do here  
   double tol = 1 * Units_Factor(UNIT_MILS, getSettings().getPageUnits());

   if ( convert_padstacks_pins_via(this, progress, tol) < 0)
   {
      // return;
      ErrorMessage("Error in Convert Padstacks", "No Padstacks are converted");
   }
   else
   {
      MessageBox(NULL,  "Convert Padstacks to Pins/Vias completed!", "Convert Padstacks", MB_ICONEXCLAMATION | MB_OK);
   }

   this->UpdateAllViews(NULL);

   progress->DestroyWindow();
   delete progress;
}

/****************************************************************************/
/*
*/
static int find_drilllayer(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      if (doc->getLayerArray()[i] == NULL)  continue;
      if (doc->getLayerArray()[i]->getLayerType() == LAYTYPE_DRILL)
         return i;
   }
   return 0;
}

/****************************************************************************/
/*
   drillmode 0 = use drill info
             1 = use a layer as drill info
                 assume drill 
*/
void CCEtoODBDoc::OnGerberinCreatepadstacks() 
{
   CString lgpcbLogFile = GetLogfilePath(GERBERPCBLogFile);
   FILE  *flog;

   if ((flog = fopen(lgpcbLogFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", GERBERPCBLogFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   int display_error = 0;

   UnselectAll(FALSE);
   
   CreatePadstacks dlg;
   dlg.m_layer = find_drilllayer(this);
   dlg.m_apertures = 0; // 0=all, 1=selected
   dlg.m_drill = 0; // 0=Drill Info, 1=Use Layer, 2=Assume Drill
   dlg.doc = this;

   int decimals = GetDecimals(getSettings().getPageUnits());
   dlg.m_tolerance.Format("%.*lf", decimals, 
            5 * Units_Factor(UNIT_MILS, getSettings().getPageUnits()));
   
   if (dlg.DoModal() != IDOK) return;

   int drillmode = dlg.m_drill; // 0=Drill Info, 1=Use Layer, 2=Assume Drill
   int drillayer = dlg.m_layer;

   CTime t = CTime::GetCurrentTime();
   fprintf(flog,"Start Padstack generation: %s\n", t.Format("%a %b %d %I:%M%p"));

   CWaitCursor hourglass;
   CProgressDlg *progress;

   progress = new CProgressDlg("Processing Padstack Generation...", FALSE);
   progress->Create();

   // do here  
   int res = exchange_padstacks(flog, this, progress, atof(dlg.m_tolerance), drillmode, drillayer);
   this->UpdateAllViews(NULL);

   progress->DestroyWindow();
   delete progress;

   t = CTime::GetCurrentTime();
   fprintf(flog," %s End Padstack Generation : %s\n", (res < 0)?"Error":"OK", t.Format("%a %b %d %I:%M%p"));

   if (res < 0)
   {
      // return;
      ErrorMessage("Error in Generate Padstacks", "No Padstacks are generated");
   }
   else
   {
      MessageBox(NULL,  "Generate Padstacks completed!", "Generate Padstacks", MB_ICONEXCLAMATION | MB_OK);
   }

   fclose(flog);
#ifdef _DEBUG
      Notepad(GetLogfilePath(GERBERPCBLogFile));
#endif

   return;
}

/////////////////////////////////////////////////////////////////////////////
// CreatePadstacks dialog
CreatePadstacks::CreatePadstacks(CWnd* pParent /*=NULL*/)
   : CDialog(CreatePadstacks::IDD, pParent)
{
   //{{AFX_DATA_INIT(CreatePadstacks)
   m_tolerance = _T("");
   m_apertures = -1;
   m_drill = -1;
   m_layer = -1;
   //}}AFX_DATA_INIT
}

void CreatePadstacks::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CreatePadstacks)
   DDX_Control(pDX, IDC_LAYER, m_layerCB);
   DDX_Text(pDX, IDC_TOLERANCE, m_tolerance);
   DDX_Radio(pDX, IDC_APERTURES, m_apertures);
   DDX_Radio(pDX, IDC_DRILL, m_drill);
   DDX_CBIndex(pDX, IDC_LAYER, m_layer);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CreatePadstacks, CDialog)
   //{{AFX_MSG_MAP(CreatePadstacks)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CreatePadstacks::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      if (doc->getLayerArray()[i] == NULL)  continue;
      int index = m_layerCB.AddString(doc->getLayerArray()[i]->getName());
      if (i == m_layer)
         m_layerCB.SetCurSel(index);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
* OnConvertFlashesToPolys
*  - This function converts selected flashed into draws
*/
void CCEtoODBDoc::OnConvertFlashesToPolys() 
{
   CWaitCursor wait;

	CFlashToPolyDlg dlg;
   if (dlg.DoModal() == IDCANCEL)
		return;

   // Open error log file
   CString errorLogFile = GetLogfilePath("FlashesToPolys.log");
	FILE *fileErr = fopen(errorLogFile, "wt");
	long displayErr = 0;

   if (fileErr == NULL)
   {
      CString msg = "";
      msg.Format("Error open [%s] file.  No error log file will be generated.", errorLogFile);
      ErrorMessage(msg, "Error");
      return;
   }
   fprintf(fileErr, "FlashesToPoly Log\n\n");

   BlockStruct *subblock;
   int selected = 0;
   int complexcnt = 0;

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);
      if (!file->isShown() || file->isHidden())
         continue;

      PrepareAddEntity(file);

      double file_rot = file->getRotation();
      if (file->isMirrored())
			file_rot = -file_rot;

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         int layer = data->getLayerIndex();

         if (!IsEntityVisible(data, &subblock, this, (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0), &layer, FALSE))
            continue;

         if (data->isMarked() || data->isSelected())
         {
            if (data->getDataType() == T_INSERT)
            {
               selected++;
               int done = FALSE;
               BlockStruct *block = this->Find_Block_by_Num(data->getInsert()->getBlockNumber());

               if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               {
						double x1 = 0.0;
						double y1 = 0.0;
						double x2 = 0.0;
						double y2 = 0.0;
						int widthIndex = 0;
						int err = 0;
						
						// round, square, rectangle, oblong, complex.
                  double sizeA = block->getSizeA();
                  double sizeB = block->getSizeB(); 
                  double xoffset= block->getXoffset();
                  double yoffset= block->getYoffset();
						double penWidth = dlg.m_Width * Units_Factor(dlg.m_Unit, this->getSettings().getPageUnits());

                  switch (block->getShape())
                  {
                  case T_RECTANGLE:
                     {
                        if (sizeA < sizeB)
                        {
                           Rotate(xoffset, yoffset - (sizeB-sizeA)/2, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                           Rotate(xoffset, yoffset + (sizeB-sizeA)/2, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x2, &y2);
                           widthIndex = Graph_Aperture("", T_RECTANGLE, sizeA, sizeA, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                        }
                        else
                        {
                           Rotate(xoffset - (sizeA-sizeB)/2, yoffset, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                           Rotate(xoffset + (sizeA-sizeB)/2, yoffset, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x2, &y2);
                           widthIndex = Graph_Aperture("", T_RECTANGLE, sizeB, sizeB, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                        }

                        Graph_PolyStruct(data->getLayerIndex(), 0L, FALSE); // poly is always with 0
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE); // filled and closed.
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);
                        Graph_Vertex(data->getInsert()->getOriginX() + x2, data->getInsert()->getOriginY() + y2, 0.0);
                        done = TRUE;
                     }
                     break;

                  case T_OBLONG:
                     {
                        if (sizeA < sizeB)
                        {
                           Rotate(xoffset, yoffset - (sizeB-sizeA)/2, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                           Rotate(xoffset, yoffset + (sizeB-sizeA)/2, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x2, &y2);
                           widthIndex = Graph_Aperture("", T_ROUND, sizeA , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                        }
                        else
                        {
                           Rotate(xoffset - (sizeA-sizeB)/2, yoffset, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                           Rotate(xoffset + (sizeA-sizeB)/2, yoffset, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x2, &y2);
                           widthIndex = Graph_Aperture("", T_ROUND, sizeB , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                        }

                        Graph_PolyStruct(data->getLayerIndex(), 0L, FALSE); // poly is always with 0
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE); // filled and closed.
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);
                        Graph_Vertex(data->getInsert()->getOriginX() + x2, data->getInsert()->getOriginY() + y2, 0.0);
                        done = TRUE;
                     }
                     break;

                  case T_ROUND:
                     {	
								double radius = sizeA/2 - penWidth/2;
								widthIndex = Graph_Aperture("", T_ROUND, penWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
								x1 = data->getInsert()->getOriginX() + xoffset;
								y1 = data->getInsert()->getOriginY() + yoffset;

                        Graph_Circle(data->getLayerIndex(), x1, y1, radius, 0L, widthIndex, FALSE, TRUE);
                        done = TRUE;
							}
							break;

                  case T_SQUARE:
                     {
								double size = sizeA/2 - penWidth/2;
								widthIndex = Graph_Aperture("", T_SQUARE, penWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

                        Graph_PolyStruct(data->getLayerIndex(), 0L, FALSE); 
                        Graph_Poly(NULL, widthIndex, TRUE, FALSE, TRUE); 								

								// 1st point
								Rotate(xoffset - size, yoffset + size, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 2nd point
								Rotate(xoffset + size, yoffset + size, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 3rd point
								Rotate(xoffset + size, yoffset - size, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 4th point
								Rotate(xoffset - size, yoffset - size, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 5st point to close the square
								Rotate(xoffset - size, yoffset + size, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);
                        done = TRUE;
							}
							break;

						case T_TARGET:
							{
								// For aperture of type T_TARGET, we ignore the penWidth when calculating the size
								double radius = sizeA/4;
								widthIndex = Graph_Aperture("", T_ROUND, penWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
								x1 = data->getInsert()->getOriginX() + xoffset;
								y1 = data->getInsert()->getOriginY() + yoffset;

								// Create the circle
                        DataStruct *newData = Graph_Circle(data->getLayerIndex(), x1, y1, radius, 0L, widthIndex, FALSE, FALSE);
								
								// Create the horizontal line
                        Graph_Poly(newData, widthIndex, FALSE, FALSE, TRUE); 	
								Rotate(xoffset - sizeA/2, yoffset, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								Rotate(xoffset + sizeA/2, yoffset, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// Create the vertical line
                        Graph_Poly(newData, widthIndex, FALSE, FALSE, TRUE); 								
								Rotate(xoffset, yoffset + sizeA/2, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								Rotate(xoffset, yoffset - sizeA/2, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);
								done = TRUE;
							}
							break;

						case T_THERMAL:
							{
								// 60 degree arc, therefore 30 degree between them
								DbUnit bulge = (DbUnit)0.268;	// bulge = tan(da/4), da = 60 degree
								double sin30 = 0.5;
								double cos30 = 0.866;

								// If sizeA is greater then sizeB, then calculate the penWidth and size from aperture
								// else use the penWidth given by the user and size = sizeA
								double radius = 0.0;
								if (sizeA > sizeB)
								{
									radius = (sizeA + sizeB) / 4;
                           penWidth = (sizeA - sizeB) / 2;
								}
								else
								{
									radius = sizeA;
								}

								widthIndex = Graph_Aperture("", T_BLANK, penWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                        Graph_PolyStruct(data->getLayerIndex(), 0L, FALSE); 

								// Top arc
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE); 	
								Rotate(xoffset + sin30 * radius, yoffset + cos30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, bulge);

								Rotate(xoffset - sin30 * radius, yoffset + cos30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// Bottom arc
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE); 	
								Rotate(xoffset + sin30 * radius, yoffset - cos30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, -bulge);

								Rotate(xoffset - sin30 * radius, yoffset - cos30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// Right arc
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE); 	
								Rotate(xoffset + cos30 * radius, yoffset + sin30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, -bulge);

								Rotate(xoffset + cos30 * radius, yoffset - sin30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// Left arc
                        Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE); 	
								Rotate(xoffset - cos30 * radius, yoffset + sin30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, bulge);

								Rotate(xoffset - cos30 * radius, yoffset - sin30 * radius, RadToDeg(block->getRotation() + data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);
								done = TRUE;
							}
							break;

						case T_DONUT:
							{
								// For aperture of type T_DONUT, we calculate the penWidth from the aperture
								double radius = (sizeA + sizeB) / 4;
								penWidth = (sizeA - sizeB) / 2;
								widthIndex = Graph_Aperture("", T_ROUND, penWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
								x1 = data->getInsert()->getOriginX() + xoffset;
								y1 = data->getInsert()->getOriginY() + yoffset;

                        Graph_Circle(data->getLayerIndex(), x1, y1, radius, 0L, widthIndex, FALSE, FALSE);
                        done = TRUE;
							}
							break;

						case T_OCTAGON:
							{
								double radius = sizeA/2 - penWidth/2;
								double halfSide = sizeA/4.83 - penWidth/2;	// sizeA/4.83 is half of one side of octagon
								widthIndex = Graph_Aperture("", T_SQUARE, penWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

                        Graph_PolyStruct(data->getLayerIndex(), 0L, FALSE); 
                        Graph_Poly(NULL, widthIndex, TRUE, FALSE, TRUE); 	

								// 1st point
								Rotate(xoffset - radius, yoffset + halfSide, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 2nd point
								Rotate(xoffset - halfSide, yoffset + radius, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 3rd point
								Rotate(xoffset + halfSide, yoffset + radius, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 4th point
								Rotate(xoffset + radius, yoffset + halfSide, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 5th point
								Rotate(xoffset + radius, yoffset - halfSide, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 6th point
								Rotate(xoffset + halfSide, yoffset - radius, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 7th point
								Rotate(xoffset - halfSide, yoffset - radius, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 8st point
								Rotate(xoffset - radius, yoffset - halfSide, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);

								// 9st point
								Rotate(xoffset - radius, yoffset + halfSide, RadToDeg(block->getRotation()+data->getInsert()->getAngle()), &x1, &y1);
                        Graph_Vertex(data->getInsert()->getOriginX() + x1, data->getInsert()->getOriginY() + y1, 0.0);
								done = TRUE;
							}
							break;

                  case T_COMPLEX:
							{
								fprintf(fileErr, "Insert [%s] of aperture [%s] is of type COMPLEX; therefore, it is skipped.\n",
										  data->getInsert()->getRefname(), subblock->getName());
								displayErr++;
							}
                     break;

						case T_UNDEFINED:
							{
								fprintf(fileErr, "Insert [%s] of aperture [%s] is of type UNDEFINED; therefore, it is skipped.\n", 
										  data->getInsert()->getRefname(), subblock->getName());
								displayErr++;
							}
							break;

						case T_BLANK:
							{
								fprintf(fileErr, "Insert [%s] of aperture [%s] is of type BLANK; therefore, it is skipped.\n",
										  data->getInsert()->getRefname(), subblock->getName());
								displayErr++;
							}
							break;
                  }
               }
               // delete data from this layer.
               if (done)
                  RemoveOneEntityFromDataList(this, &(file->getBlock()->getDataList()), data);
            }
         }
      }

   }

	fprintf(fileErr, "\nEnd of FlashesToPoly");

	fclose(fileErr);
   UnselectAll(FALSE);

   if (!selected)
	{
      ErrorMessage("No Selected Apertures found!", "Convert Aperture");
	}
   else
	{
      UpdateAllViews(NULL);
		if (displayErr)
			Logreader(errorLogFile);
	}

   return;
}

/******************************************************************************
* OnConvertSelectedToFlash
*  - This function converts all selected into 1 aperture.
*/
void CCEtoODBDoc::OnConvertSelectedToFlash() 
{
   CWaitCursor wait;

   BlockStruct *subblock;
   int selected = 0;
   int complexcnt = 0;
   double accuracy = get_accuracy(this);

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      PrepareAddEntity(file);

      double file_rot = file->getRotation();
      if (file->isMirrored())
         file_rot = -file_rot;

      CString  pshapename, appname;
      while (TRUE)
      {
         pshapename.Format("SHAPE_%d", ++complexcnt);
         if (!Graph_Block_Exists(this, pshapename, -1))
            break;
      }

      Graph_Block_On(GBO_APPEND,pshapename, -1, 0);
      int lnum = 0;  // all datas are merged into the layer of the last selected element.
      double firstx = 0, firsty = 0;
      BOOL First = TRUE;   

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         int layer = data->getLayerIndex();

         if (!IsEntityVisible(data, &subblock, this, (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0), &layer, FALSE))
            continue;

         if (data->isMarked() || data->isSelected())
         {
            if (data->getDataType() == T_POLY)
            {
               selected++;
               // make a complex aperture.
               // copy this entity and normalize to first coo
               // place complex aperture
               // delete data from this layer.
               lnum = data->getLayerIndex();

               DataStruct *d = Graph_PolyStruct(Graph_Level("0", "", 1), 0L, FALSE); // poly is always with 0

               // here is know that data is a T_POLY (is_poly_data)
               POSITION polyPos = data->getPolyList()->GetHeadPosition();
               while (polyPos)
               {
                  CPoly *poly = data->getPolyList()->GetNext(polyPos);
               
                  Graph_Poly(NULL,poly->getWidthIndex(), poly->isFilled(), poly->isVoid(), poly->isClosed()); // filled and closed.
                  POSITION pos = poly->getPntList().GetHeadPosition();
                  while (pos)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pos);
               
                     if (First)
                     {
                        firstx = pnt->x;
                        firsty = pnt->y;
                        First = FALSE;
                     }
                     Graph_Vertex(pnt->x-firstx, pnt->y-firsty, pnt->bulge);
                  }
               }
               // delete data from this layer.
               RemoveOneEntityFromDataList(this, &file->getBlock()->getDataList(), data);
            }
         }
      }                               

      Graph_Block_Off();
      appname = "C" + pshapename;
      Graph_Complex(appname, 0, pshapename, 0.0, 0.0, 0.0);
      Graph_Block_Reference(appname, NULL, -1, firstx, firsty, 0.0, 0, 1.0, lnum, TRUE);

      double accuracy = 0.001;
      if (getSettings().getPageUnits() == UNIT_MILS)
         accuracy = 1;
      if (getSettings().getPageUnits() == UNIT_MM)
         accuracy = 0.0254;

      OptimizeComplexApertures(this, &file->getBlock()->getDataList(), accuracy, FALSE);
      OptimizeCoveredApertures(this, &file->getBlock()->getDataList());
      OptimizeDuplicateApertures(this, accuracy);
   }

   UnselectAll(FALSE);

   if (!selected)
      ErrorMessage("No Selected Elements found!", "Convert Aperture");
   else
      UpdateAllViews(NULL);
}

/******************************************************************************
* IsConvertablePoly
   -This function returns true if a polyline can be converted into a Aperture.
   -The min and max of the poly extents are considered.
*/
static BOOL IsConvertablePoly(CCEtoODBDoc *doc, DataStruct *data, double minWidth, double maxWidth)
{
   if (data->getDataType() != T_POLY)
      return FALSE;

   if (data->getPolyList()->GetCount() != 1)
      return FALSE;

   Mat2x2 m;
   RotMat2(&m, 0.0);

   ExtentRect rect1;
   PolyExtents(doc, data->getPolyList(), &rect1, 1, 0, 0, 0, &m, TRUE);

   double length = fabs(rect1.left - rect1.right);
   double width = fabs(rect1.top - rect1.bottom);

   if (length < minWidth || length > maxWidth)
      return FALSE;
   else if (width < minWidth || width > maxWidth)
      return FALSE;
   else
      return TRUE;
}

/******************************************************************************
* OnCleanGerber
*  - This function tries to clean up gerber files.
*/
void CCEtoODBDoc::OnCleanGerber() 
{
   CWaitCursor hourglass;

   CProgressDlg *progress;

   progress = new CProgressDlg("Clean Gerber...", FALSE);
   progress->Create();

   double accuracy = get_accuracy(this);

   UnselectAll(FALSE);

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;
   
      PrepareAddEntity(file);

      //progress->SetStatus("Crack Polys...");
      //Crack(this,file->getBlock(), FALSE);         

      //progress->SetStatus("Eliminate MidPoint Polys...");
      //EliminateMidPointPolys(this, accuracy);               
      
      progress->SetStatus("Eliminate Small Polys...");
      EliminateSmallPolys(this, accuracy);                 
      
      progress->SetStatus("Eliminate Overlapping Polys...");
      EliminateOverlappingTraces(this, file->getBlock(), FALSE, accuracy);                
      
      progress->SetStatus("Break Spike Polys...");
      BreakSpikePolys(file->getBlock());
   }

   progress->DestroyWindow();
   delete progress;

   UpdateAllViews(NULL);
}

/******************************************************************************
* OnConvertPolysToFlashes
*  - This function tries to auto convert all possible draw combinations
*  - only highlevel polygons
*  - If some thing is not a poly, then it will not be take into consideration
*/
void CCEtoODBDoc::OnConvertPolysToFlashes() 
{
   PolyDrawToFlash dlg;
   dlg.m_exclude_dcodes = "";
   dlg.m_process_area = 1; // 0 selected, 1 total database
   int decimals = GetDecimals(getSettings().getPageUnits());
   
   dlg.m_maximum_width.Format("%.*lf", decimals, 100 * Units_Factor(UNIT_MILS, getSettings().getPageUnits()));
   dlg.m_minimum_width.Format("%.*lf", decimals, 1 * Units_Factor(UNIT_MILS, getSettings().getPageUnits()));
   dlg.m_tolerance.Format("%.*lf", decimals, 1 * Units_Factor(UNIT_MILS, getSettings().getPageUnits()));

   if (dlg.DoModal() != IDOK)
      return;

#if defined(EnableNewPolyToFlashConverter)
   bool useNewConverter = true;

   if (useNewConverter)
   {
      double maxWidth  = atof(dlg.m_maximum_width);
      double minWidth  = atof(dlg.m_minimum_width);
      double tolerance = atof(dlg.m_tolerance);
      bool processArea = (dlg.m_process_area != 0);

      CPolyToFlashConverter converter(*this);
      converter.convertPolysToFlashes(processArea,tolerance,minWidth,maxWidth);
      return;
   }
#endif

   CWaitCursor wait;
   COperationProgress progress;

   double accuracy = atof(dlg.m_tolerance);
   int polyCount = 0;
   int complexCount = 0;
   int shapeCount = 0;
   int floatingLayerIndex = Graph_Level("0", "", 1);
   //int dataCount = 0;

   //for (POSITION filePos = FileList.GetHeadPosition();filePos != NULL;)
   //{
   //   FileStruct *file = FileList.GetNext(filePos);

   //   if (file->isShown() && !file->isHidden())
   //   {
   //      dataCount += file->getBlock()->getDataList().GetCount();
   //   }
   //}

   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      CString statusMessage;
      statusMessage.Format("Converting Polys to Flashes for file %d",file->getFileNumber());
      progress.updateStatus(statusMessage,file->getBlock()->getDataList().GetCount());

      PrepareAddEntity(file);

      double file_rot = file->getRotation();
      if (file->isMirrored())
         file_rot = -file_rot;
      
      for (POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         progress.incrementProgress();

         // only polys can be converted
         if (data->getDataType() != T_POLY)
            continue;

         int layer = data->getLayerIndex();
         BlockStruct *subblock;
         if (!IsEntityVisible(data, &subblock, this, (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0), &layer, FALSE))
            continue;

         if (!(data->isMarked() || data->isSelected() || dlg.m_process_area))
            continue;

         if (!IsConvertablePoly(this, data, atof(dlg.m_minimum_width), atof(dlg.m_maximum_width)))
            continue;

         // this attribute will not allow a flash conversion.
         // it is set by the user or/and cleangerber function.
         if (is_attvalue(this, data->getAttributesRef(), ATT_KEEPDRAW, 0))
            continue;

         polyCount++;

         CString padShapeName;
         while (TRUE)
         {
            padShapeName.Format("SHAPE_%d", ++shapeCount);
            if (!Graph_Block_Exists(this, padShapeName, -1))
               break;
         }

         Graph_Block_On(GBO_APPEND, padShapeName, -1, 0);

         Graph_PolyStruct(floatingLayerIndex, 0L, FALSE);

         double xmax, xmin, ymax, ymin;
         xmax = ymax = -DBL_MAX;
         xmin = ymin = DBL_MAX;
         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos)
         {
            CPoly *poly = data->getPolyList()->GetNext(polyPos);
         
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);

               if (pnt->x > xmax)
                  xmax = pnt->x;
               if (pnt->x < xmin)
                  xmin = pnt->x;
               if (pnt->y > ymax)
                  ymax = pnt->y;
               if (pnt->y < ymin)
                  ymin = pnt->y;
            }
         }

         double cx = (xmax + xmin) / 2;
         double cy = (ymax + ymin) / 2;

         polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos)
         {
            CPoly *poly = data->getPolyList()->GetNext(polyPos);
         
            Graph_Poly(NULL, poly->getWidthIndex(), poly->isFilled(), poly->isVoid(), poly->isClosed());

            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos)
            {
               CPnt *pnt = poly->getPntList().GetNext(pntPos);
      
               Graph_Vertex(pnt->x - cx, pnt->y - cy, pnt->bulge);
            }
         }
         // delete data from this layer.
         RemoveOneEntityFromDataList(this, &file->getBlock()->getDataList(), data);

         Graph_Block_Off();

         // Insert complex aperture back into the file where it belong
         CString complexName = "C" + padShapeName;
         while (TRUE)
         {
            complexName.Format("C%s_%d", padShapeName, ++complexCount);
            if (!Graph_Block_Exists(this, complexName, -1))
               break;
         }
         Graph_Complex(complexName, 0, padShapeName, 0.0, 0.0, 0.0);
         Graph_Block_Reference(complexName, NULL, -1, cx, cy, 0.0, 0, 1.0, layer, TRUE);
      }

      if (polyCount) // Only do optimization if there are polys converted to flashes
      {
         OptimizeComplexApertures(this, &file->getBlock()->getDataList(), accuracy, FALSE);
//       OptimizeCoveredApertures(this, &file->getBlock()->getDataList());
         OptimizeDuplicateApertures(this, accuracy);

         // Do this after all the other optimization because CombineTouchAperture
         // will create uniquely individual two level complex apertures
//       CombineTouchingApertures(this, &file->getBlock()->getDataList());
         OptimizeTouchingApertures(this, &file->getBlock()->getDataList());
      }
   }

   UnselectAll(FALSE);

   if (!polyCount)
      ErrorMessage("No Polys found to convert Draw to Flash!", "Convert Draw to Flash");
   else
      UpdateAllViews(NULL);
}

struct ApStruct
{
   DataStruct *data;
   POSITION dataPos;
   Region *region;
};
typedef CTypedPtrList<CPtrList, ApStruct*> CApList;

/******************************************************************************
* OptimizeTouchingApertures
*/
static void OptimizeTouchingApertures(CCEtoODBDoc *doc, CDataList *dataList) 
{
   float scaleFactor = (float)(1000.0 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits()));
   int shapeCount = 0;
   int complexCount = 0;

   COperationProgress progress;
   progress.updateStatus("Combining overlapped apertures - Region generation",dataList->GetCount());

   CApList apertureList;
   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      POSITION tempPos = dataPos;
      DataStruct *data = dataList->GetNext(dataPos);
      progress.incrementProgress();

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());

      if (!(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_BLOCK_APERTURE))
         continue;

      CPolyList *polylist;

      switch (block->getShape())
      {
      case T_UNDEFINED:
      case T_BLANK:
         continue;
      case T_COMPLEX:
         {
            BlockStruct *subblock = doc->Find_Block_by_Num((int)block->getSizeA());

            if (subblock->getDataList().GetCount() > 1)
               continue;

            DataStruct *polyData = subblock->getDataList().GetHead();

            if (polyData->getDataType() != T_POLY)
               continue;

            polylist = new CPolyList;
            POSITION polyPos = polyData->getPolyList()->GetHeadPosition();
            {
               CPoly *poly = polyData->getPolyList()->GetNext(polyPos);

               CPoly *newPoly = new CPoly;
               newPoly->setWidthIndex(poly->getWidthIndex());
               newPoly->setFilled(poly->isFilled());
               newPoly->setClosed(poly->isClosed());
               newPoly->setVoid(poly->isVoid());
               polylist->AddTail(newPoly);

               Mat2x2 m;
               RotMat2(&m, block->getRotation());

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);
                  CPnt *newPnt = new CPnt;
                  newPnt->bulge = 0.0;
                  Point2 p;
                  p.x = pnt->x;
                  p.y = pnt->y;
                  TransPoint2(&p, 1, &m, data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
                  newPnt->x = (DbUnit)p.x;
                  newPnt->y = (DbUnit)p.y;
                  newPoly->getPntList().AddTail(newPnt);
               }
            }
         }
         break;

      default:
         polylist = ApertureToPoly(block, data->getInsert(), 0);
         break;
      }

      ApStruct *ap = new ApStruct;
      ap->data = data;
      ap->dataPos = tempPos;
      ap->region = RegionFromPolylist(doc, polylist, scaleFactor);
      apertureList.AddTail(ap);
   }

//return; // debug

   int floatingLayerIndex = Graph_Level("0", "", 1);

   // Loop through the list of apertures to find intersecting apertures
   progress.updateStatus("Combining overlapped apertures - Region analysis",apertureList.GetCount());

   while (apertureList.GetCount())
   {
      progress.updateProgress(apertureList.GetCount());

      CApList touchingList;
      touchingList.AddTail(apertureList.RemoveHead());

      // loop trough touchingList (including ones added to the tail) to find other touching apertures
      POSITION touchingPos = touchingList.GetHeadPosition();
      while (touchingPos)
      {
         ApStruct *testAp1 = touchingList.GetAt(touchingPos);

         POSITION aperturePos = apertureList.GetHeadPosition();
         while (aperturePos)
         {
            POSITION tempPos = aperturePos;
            ApStruct *testAp2 = apertureList.GetNext(aperturePos);

            if (testAp1->data->getLayerIndex() == testAp2->data->getLayerIndex())
            {
               if (DoRegionsTouch(testAp1->region, testAp2->region)) // touching -> add to touchingList
               {
                  apertureList.RemoveAt(tempPos);
                  touchingList.AddTail(testAp2);
               }
            }
         }

         touchingList.GetNext(touchingPos); // wait until end because we might have added some
      }


/*    // loop through touchingList to see if any are completely covered
      POSITION touchingPos1 = touchingList.GetHeadPosition();
      while (touchingPos1)
      {
         POSITION tempPos1 = touchingPos1;
         ApStruct *testAp1 = touchingList.GetNext(touchingPos1);

         POSITION touchingPos2 = touchingPos1;
         while (touchingPos2)
         {
            POSITION tempPos2 = touchingPos2;
            ApStruct *testAp2 = touchingList.GetNext(touchingPos2);

            Region *intersectRegion = IntersectRegions(testAp1->region, testAp2->region);

            if (!intersectRegion)
               continue;
               
            if (intersectRegion->IsEmpty())
            {
               delete intersectRegion;
               continue;
            }

            if (AreRegionsEqual(intersectRegion, testAp1->region))
            {
               touchingList.RemoveAt(tempPos1);
               delete testAp1->region;
               dataList->RemoveAt(testAp1->dataPos); // remove from database because completely covered
               delete testAp1->data;
            }
            else if (AreRegionsEqual(intersectRegion, testAp2->region))
            {
               touchingList.RemoveAt(tempPos2);
               delete testAp2->region;
               dataList->RemoveAt(testAp2->dataPos); // remove from database because completely covered
               delete testAp2->data;
            }
         }
      }
*/
      // free region memory
      POSITION tempPos = touchingList.GetHeadPosition();
      while (tempPos)
      {
         ApStruct *ap = touchingList.GetNext(tempPos);
         delete ap->region;
      }

      // nothing touching to combine
      if (touchingList.GetCount() < 2)
      {
         touchingPos = touchingList.GetHeadPosition();
         while (touchingPos)
         {
            ApStruct *ap = touchingList.GetNext(touchingPos);
            delete ap;
         }
         touchingList.RemoveAll();
         continue;
      }
      
      // Find an unused name for the padshape
      CString padshapeName = "";
      while (TRUE)
      {
         padshapeName.Format("SHAPE_%d", ++shapeCount);
         if (!Graph_Block_Exists(doc, padshapeName, -1))
            break;
      }

      // Create a complex aperture from the list of intersected apertures
      BlockStruct *complexAperture = Graph_Block_On(GBO_APPEND, padshapeName, -1, 0);
      Graph_Block_Off();

      BOOL first = TRUE;
      int firstLayer = 0;
      
      for (touchingPos = touchingList.GetHeadPosition();touchingPos!= NULL;)
      {
         ApStruct *ap = touchingList.GetNext(touchingPos);

         if (first)
         {
            firstLayer = ap->data->getLayerIndex();
            first = FALSE;
         }

         ap->data->setLayerIndex(floatingLayerIndex);

         complexAperture->getDataList().AddTail(ap->data);
         dataList->RemoveAt(ap->dataPos);
      }

      double xMin,xMax,yMin,yMax;
      block_extents(doc,&xMin,&xMax,&yMin,&yMax,&(complexAperture->getDataList()),0.,0.,0.,0,1.,-1,FALSE);
      double xCenter = (xMin + xMax)/2.;
      double yCenter = (yMin + yMax)/2.;
      
      for (touchingPos = touchingList.GetHeadPosition();touchingPos!= NULL;)
      {
         ApStruct *ap = touchingList.GetNext(touchingPos);

         ap->data->getInsert()->incOriginX(-xCenter);
         ap->data->getInsert()->incOriginY(-yCenter);

         delete ap;
      }

      touchingList.RemoveAll();

      // Insert complex aperture back into the file where it belong
      CString complexName = "C" + padshapeName;

      while (TRUE)
      {
         complexName.Format("C%s_%d", padshapeName, ++complexCount);

         if (!Graph_Block_Exists(doc, complexName, -1))
            break;
      }

      Graph_Complex(complexName, 0, padshapeName, 0.0, 0.0, 0.0);
      Graph_Block_Reference(complexName, NULL, -1,xCenter,yCenter, 0.0, 0, 1.0, firstLayer, TRUE); 
   }
}

/******************************************************************************
* CombineTouchingApertures
*
void CombineTouchingApertures(CCEtoODBDoc *doc, CDataList *dataList) 
{
   int shapeCount = 0;
   int complexCount = 0;

   CDataList apertureList;
   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->BlockArray[data->getInsert()->getBlockNumber()];

      if (!(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_BLOCK_APERTURE))
         continue;
         
      apertureList.AddTail(data);
   }


   // Loop through the list of apertures to find intersecting apertures
   while (apertureList.GetCount())
   {
      CDataList touchingList;
      touchingList.AddTail(apertureList.RemoveHead());

      // Loop through the list of intersected apertures
      // against the reminding apertures to find more intersection
      POSITION touchingPos = touchingList.GetHeadPosition();
      while (touchingPos)
      {
         DataStruct *testData = touchingList.GetAt(touchingPos);

         BlockStruct *block1 = doc->BlockArray[testData->getInsert()->getBlockNumber()];

         InsertStruct insert1;
         insert1.pnt.x = testData->getInsert()->getOriginX();
         insert1.pnt.y = testData->getInsert()->getOriginY();
         insert1.angle = testData->getInsert()->getAngle();

         POSITION aperturePos = apertureList.GetHeadPosition();
         while (aperturePos)
         {
            POSITION tempPos = aperturePos;
            DataStruct *aperture = apertureList.GetNext(aperturePos);

            BlockStruct *block2 = doc->BlockArray[aperture->getInsert()->getBlockNumber()];

            InsertStruct insert2;
            insert2.pnt.x = aperture->getInsert()->getOriginX();
            insert2.pnt.y = aperture->getInsert()->getOriginY();
            insert2.angle = aperture->getInsert()->getAngle();

            Point2 result1;
            Point2 result2;

            double distance = MeasureApertureToAperture(doc, block1, insert1.pnt.x, insert1.pnt.y, insert1.angle,
                  0, block2, insert2.pnt.x, insert2.pnt.y, insert2.angle, 0, &result1, &result2);

            if (distance <= 0)
            {
               apertureList.RemoveAt(tempPos);
               touchingList.AddTail(aperture);
            }
         }

         touchingList.GetNext(touchingPos);
      }

      if (touchingList.GetCount() < 2)
      {
         touchingList.RemoveAll();
         continue;
      }
      
      // Find an unused name for the padshape
      CString padshapeName = "";
      while (TRUE)
      {
         padshapeName.Format("SHAPE_%d", ++shapeCount);
         if (!Graph_Block_Exists(doc, padshapeName, -1))
            break;
      }

      // Create a complex aperture from the list of intersected apertures
      BlockStruct *complexAperture = Graph_Block_On(GBO_APPEND, padshapeName, -1, 0);
      Graph_Block_Off();

      BOOL first = TRUE;
      double firstX = 0.0;
      double firstY = 0.0;
      int firstLayer = 0;
      touchingPos = touchingList.GetHeadPosition();
      while (touchingPos)
      {
         DataStruct *data = touchingList.GetNext(touchingPos);

         if (first)
         {
            firstX = data->getInsert()->getOriginX();
            firstY = data->getInsert()->getOriginY();
            firstLayer = data->getLayerIndex();
            first = FALSE;
         }
         
         data->getInsert()->getOriginX() -= (float)firstX;
         data->getInsert()->getOriginY() -= (float)firstY;

         complexAperture->getDataList().AddTail(data);
         dataList->RemoveAt(dataList->Find(data));

      }
      touchingList.RemoveAll();

      // Insert complex aperture back into the file where it belong
      CString complexName = "C" + padshapeName;
      while (TRUE)
      {
         complexName.Format("C%s_%d", padshapeName, ++complexCount);
         if (!Graph_Block_Exists(doc, complexName, -1))
            break;
      }
      Graph_Complex(complexName, 0, padshapeName, 0.0, 0.0, 0.0);
      Graph_Block_Reference(complexName, NULL, -1, firstX, firstY, 0.0, 0, 1.0, firstLayer, TRUE); 
   }
}
*/

///////////////////////////////////////////////////////////////
// PolyDrawToFlash dialog			                            //
///////////////////////////////////////////////////////////////
PolyDrawToFlash::PolyDrawToFlash(CWnd* pParent /*=NULL*/)
   : CDialog(PolyDrawToFlash::IDD, pParent)
{
   //{{AFX_DATA_INIT(PolyDrawToFlash)
   m_maximum_width = _T("");
   m_minimum_width = _T("");
   m_tolerance = _T("");
   m_exclude_dcodes = _T("");
   m_process_area = 1;
   //}}AFX_DATA_INIT
}

void PolyDrawToFlash::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PolyDrawToFlash)
   DDX_Text(pDX, IDC_MAXIMUM_WIDTH, m_maximum_width);
   DDX_Text(pDX, IDC_MINIMUM_WIDTH, m_minimum_width);
   DDX_Text(pDX, IDC_TOLERANCE, m_tolerance);
   DDX_Text(pDX, IDC_EXCLUDE_DCODES, m_exclude_dcodes);
   DDX_Radio(pDX, IDC_PROCESS_AREA, m_process_area);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PolyDrawToFlash, CDialog)
   //{{AFX_MSG_MAP(PolyDrawToFlash)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()



///////////////////////////////////////////////////////////////
// CFlashToPolyDlg dialog			                            //
///////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CFlashToPolyDlg, CDialog)
CFlashToPolyDlg::CFlashToPolyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlashToPolyDlg::IDD, pParent)
	, m_WidthOption(FALSE)
{
	m_Unit = UNIT_INCHES;
	m_Width = 0.0;
}

CFlashToPolyDlg::~CFlashToPolyDlg()
{
}

void CFlashToPolyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_ZEROWIDTH, m_WidthOption);
	DDX_Control(pDX, IDC_UNIT, m_UnitComboBox);
}

BEGIN_MESSAGE_MAP(CFlashToPolyDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_UNIT, OnCbnSelchangeUnit)
	ON_BN_CLICKED(IDC_ZEROWIDTH, OnBnClickedZerowidth)
	ON_BN_CLICKED(IDC_WIDTH, OnBnClickedWidth)
END_MESSAGE_MAP()

// CFlashToPolyDlg message handlers
void CFlashToPolyDlg::OnOK()
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

BOOL CFlashToPolyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->m_UnitComboBox.AddString("INCH");
	this->m_UnitComboBox.SetItemData(0, UNIT_INCHES);

	this->m_UnitComboBox.AddString("MIL");
	this->m_UnitComboBox.SetItemData(1, UNIT_MILS);

	this->m_UnitComboBox.AddString("MM");
	this->m_UnitComboBox.SetItemData(2, UNIT_MM);

	// Set the unit combo box to show INCH 
	this->m_UnitComboBox.SetCurSel(0);

	return TRUE;  
}

void CFlashToPolyDlg::OnCbnSelchangeUnit()
{
	m_Unit = m_UnitComboBox.GetItemData(m_UnitComboBox.GetCurSel());

	switch (m_Unit)
	{
	case UNIT_INCHES:
		GetDlgItem(IDC_WIDTH)->SetWindowText("0.002");	// This number is per the specification of Dean Faber
		break;

	case UNIT_MILS:
		GetDlgItem(IDC_WIDTH)->SetWindowText("2.0");		// This number is per the specification of Dean Faber
		break;

	case UNIT_MM:
		GetDlgItem(IDC_WIDTH)->SetWindowText("0.05");	// This number is per the specification of Dean Faber
		break;
	}
}

void CFlashToPolyDlg::OnBnClickedZerowidth()
{
	m_Width = 0.0;
}

void CFlashToPolyDlg::OnBnClickedWidth()
{
	CString width = "0";
	GetDlgItem(IDC_WIDTH)->GetWindowText(width);
	m_Width = atof(width);
}

#if defined(EnableNewPolyToFlashConverter)
//_____________________________________________________________________________
CPolyToFlashConverter::CPolyToFlashConverter(CCEtoODBDoc& camCadDoc) :
   m_camCadDoc(camCadDoc),
   m_camCadDatabase(camCadDoc)
{
}

CPolyToFlashConverter::~CPolyToFlashConverter()
{
}

void CPolyToFlashConverter::convertPolysToFlashes(bool processAreaFlag,double tolerance,
   double minWidth,double maxWidth)
{
   CWaitCursor wait;
   COperationProgress progress;

   int keepDrawKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_KEEPDRAW);
   int floatingLayerIndex = m_camCadDatabase.getLayer(ccLayerFloat)->getLayerIndex();
   int shapeCount = 0;
   int apertureCount = 0;

   for (POSITION filePos = m_camCadDoc.FileList.GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = m_camCadDoc.FileList.GetNext(filePos);

      if (!file->isShown() || file->isHidden())
      {
         continue;
      }

      CString statusMessage;
      statusMessage.Format("Converting Polys to Flashes for file %d",file->getFileNumber());
      progress.updateStatus(statusMessage,file->getBlock()->getDataCount());

      bool mirrorLayersFlag = ((file->isMirrored() ^ m_camCadDoc.Bottom) != 0);
      Attrib* attrib;
      CString padShapeName,apertureName;
      int convertedPolyCount = 0;
      BlockStruct* fileBlock = file->getBlock();

      for (POSITION dataPos = fileBlock->getHeadDataPosition();dataPos != NULL;)
      {
         progress.incrementProgress();

         POSITION oldDataPos = dataPos;
         DataStruct* polyStruct = file->getBlock()->getNextData(dataPos);

         if (polyStruct->getDataType() != dataTypePoly)
         {
            continue;
         }

         if (! polyStruct->isVisible(m_camCadDoc,NULL,mirrorLayersFlag))
         {
            continue;
         }

         if (!(polyStruct->getMarked() || polyStruct->isSelected() || processAreaFlag))
         {
            continue;
         }

         if (polyStruct->getAttributes() != NULL &&
             polyStruct->getAttributes()->Lookup(keepDrawKeywordIndex,attrib) != 0)
         {
            continue;
         }

         if (polyStruct->getPolyList()->GetCount() != 1)
         {
            continue;
         }

         CExtent polyExtent = polyStruct->getPolyList()->getExtent(m_camCadDoc);

         if ((min(polyExtent.getXsize(),polyExtent.getYsize()) < minWidth) ||
             (max(polyExtent.getXsize(),polyExtent.getYsize()) > maxWidth)    )
         {
            continue;
         }

         convertedPolyCount++;

         CPoint2d polyExtentCenter = polyExtent.getCenter();
         int polyLayerIndex = polyStruct->getLayerIndex();

         padShapeName.Format("SHAPE_%d",++shapeCount);

         BlockStruct* shapeGeometry = m_camCadDatabase.getNewBlock(padShapeName,"%d",BLOCKTYPE_UNKNOWN,-1);
         //DataStruct* newPolyStruct = new DataStruct(*polyStruct,false);
         CTMatrix matrix;
         matrix.translate(-polyExtentCenter);
         polyStruct->transform(matrix,&m_camCadDoc);
         polyStruct->setLayerIndex(floatingLayerIndex);

         shapeGeometry->getDataList().AddTail(polyStruct);
         //dataToRemove.AddTail(polyStruct);
         file->getBlock()->getDataList().RemoveAt(oldDataPos);

         apertureName.Format("C%s_%d",(const char*)shapeGeometry->getName(),++apertureCount);
         BlockStruct* apertureGeometry = m_camCadDatabase.getDefinedAperture(apertureName,apertureComplex,
            shapeGeometry->getBlockNumber());
         m_camCadDatabase.referenceBlock(fileBlock,apertureGeometry,insertTypeUnknown,apertureName,
            polyLayerIndex,polyExtentCenter.x,polyExtentCenter.y);
      }

      if (convertedPolyCount > 0)
      {
         OptimizeComplexApertures(fileBlock->DataList,tolerance);
         OptimizeDuplicateApertures(tolerance);
         OptimizeTouchingApertures(*fileBlock);
      }
   }

   m_camCadDoc.UnselectAll(false);
}

void CPolyToFlashConverter::OptimizeComplexApertures(CDataList& padstackDataList,double accuracy)
{
   COperationProgress progress;
   progress.updateStatus("Optimizing complex apertures",padstackDataList.GetCount());
   
   for (POSITION dataPos = padstackDataList.GetHeadPosition();dataPos != NULL;)
   {
      DataStruct *data = padstackDataList.GetNext(dataPos);
      progress.incrementProgress();

      if (data->isInsert())
      {
         BlockStruct *insertedGeom = m_camCadDatabase.getBlock(data->getInsert()->getBlockNumber());

         OptimizeComplexApertureBlock(*insertedGeom,data->getLayerIndex(),accuracy);
      }
   }

   m_camCadDatabase.invalidateBlockIndex();
}

void CPolyToFlashConverter::OptimizeDuplicateApertures(double accuracy)
{
   COperationProgress progress;

   CArray<int,int> apertures;
   apertures.SetSize(0,5000);

   for (int index = 0;index < m_camCadDoc.getMaxBlockIndex();index++)
   {
      BlockStruct* block = m_camCadDoc.getBlockAt(index);

      if (block != NULL && 
         ((block->getFlags() & (BL_APERTURE | BL_BLOCK_APERTURE))) != 0)
      {
         apertures.Add(index);
      }
   }

   int apertureCount = apertures.GetCount();
   int iterationCount = (((apertureCount - 1) * (apertureCount - 1))/2) + apertureCount;

   progress.updateStatus("Consolidating duplicate apertures",iterationCount);

   int index1,index2,blockIndex1,blockIndex2;
   BlockStruct *block1,*block2;

   for (index1 = 0;index1 < apertures.GetCount();index1++)
   {
      blockIndex1 = apertures.GetAt(index1);
      block1 = m_camCadDoc.getBlockAt(blockIndex1);

      if (block1 != NULL)
      {
         for (index2 = index1 + 1;index2 < apertures.GetCount();index2++)
         {
            progress.incrementProgress();

            blockIndex2 = apertures.GetAt(index2);
            block2 = m_camCadDoc.getBlockAt(blockIndex2);

            if (block2 != NULL)
            {
               if (IsEqualAperture(&m_camCadDoc,block1,block2,accuracy))
               {
                  ExchangeAperture(&m_camCadDoc,blockIndex1,blockIndex2);
               }
            }
         }
      }
   }

   m_camCadDatabase.invalidateBlockIndex();
}

void CPolyToFlashConverter::OptimizeTouchingApertures(BlockStruct& fileBlock)
{
   CDataList& dataList = fileBlock.DataList;

   float scaleFactor = (float)(1000.0 * Units_Factor(UNIT_INCHES, m_camCadDoc.Settings.PageUnits));
   int shapeCount = 0;
   int complexCount = 0;

   COperationProgress progress;
   progress.updateStatus("Combining overlapped apertures - Region generation",dataList.GetCount());

   CApList apertureList;
   
   for (POSITION dataPos = dataList.GetHeadPosition();dataPos != NULL;)
   {
      POSITION tempPos = dataPos;
      DataStruct *data = dataList.GetNext(dataPos);
      progress.incrementProgress();

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());

      if (!(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_BLOCK_APERTURE))
         continue;

      CPolyList *polylist;

      switch (block->getShape())
      {
      case T_UNDEFINED:
      case T_BLANK:
         continue;
      case T_COMPLEX:
         {
            BlockStruct *subblock = m_camCadDoc.Find_Block_by_Num((int)block->getSizeA());

            if (subblock->getDataList().GetCount() > 1)
               continue;

            DataStruct *polyData = subblock->getDataList().GetHead();

            if (polyData->getDataType() != T_POLY)
               continue;

            polylist = new CPolyList;
            POSITION polyPos = polyData->getPolyList()->GetHeadPosition();
            {
               CPoly *poly = polyData->getPolyList()->GetNext(polyPos);

               CPoly *newPoly = new CPoly;
               newPoly->widthIndex = poly->getWidthIndex();
               newPoly->Filled = poly->isFilled();
               newPoly->Closed = poly->isClosed();
               newPoly->VoidPoly = poly->isVoid();
               polylist->AddTail(newPoly);

               Mat2x2 m;
               RotMat2(&m, block->getRotation());

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);
                  CPnt *newPnt = new CPnt;
                  newPnt->bulge = 0.0;
                  Point2 p;
                  p.x = pnt->x;
                  p.y = pnt->y;
                  TransPoint2(&p, 1, &m, data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
                  newPnt->x = (DbUnit)p.x;
                  newPnt->y = (DbUnit)p.y;
                  newPoly->list.AddTail(newPnt);
               }
            }
         }
         break;

      default:
         polylist = ApertureToPoly(block, data->getInsert(), 0);
         break;
      }

      ApStruct *ap = new ApStruct;
      ap->data = data;
      ap->dataPos = tempPos;
      ap->region = RegionFromPolylist(&m_camCadDoc, polylist, scaleFactor);
      apertureList.AddTail(ap);
   }

//return; // debug

   int floatingLayerIndex = m_camCadDatabase.getLayer(ccLayerFloat)->getLayerIndex();

   // Loop through the list of apertures to find intersecting apertures
   int listCount = apertureList.GetCount();
   progress.updateStatus("Combining overlapped apertures - Region analysis",listCount);

   while (apertureList.GetCount())
   {
      progress.updateProgress(listCount - apertureList.GetCount());

      CApList touchingList;
      touchingList.AddTail(apertureList.RemoveHead());

      // loop trough touchingList (including ones added to the tail) to find other touching apertures
      POSITION touchingPos = touchingList.GetHeadPosition();
      while (touchingPos)
      {
         ApStruct *testAp1 = touchingList.GetAt(touchingPos);

         POSITION aperturePos = apertureList.GetHeadPosition();
         while (aperturePos)
         {
            POSITION tempPos = aperturePos;
            ApStruct *testAp2 = apertureList.GetNext(aperturePos);

            if (testAp1->data->getLayerIndex() == testAp2->data->getLayerIndex())
            {
               if (DoRegionsTouch(testAp1->region, testAp2->region)) // touching -> add to touchingList
               {
                  apertureList.RemoveAt(tempPos);
                  touchingList.AddTail(testAp2);
               }
            }
         }

         touchingList.GetNext(touchingPos); // wait until end because we might have added some
      }


/*    // loop through touchingList to see if any are completely covered
      POSITION touchingPos1 = touchingList.GetHeadPosition();
      while (touchingPos1)
      {
         POSITION tempPos1 = touchingPos1;
         ApStruct *testAp1 = touchingList.GetNext(touchingPos1);

         POSITION touchingPos2 = touchingPos1;
         while (touchingPos2)
         {
            POSITION tempPos2 = touchingPos2;
            ApStruct *testAp2 = touchingList.GetNext(touchingPos2);

            Region *intersectRegion = IntersectRegions(testAp1->region, testAp2->region);

            if (!intersectRegion)
               continue;
               
            if (intersectRegion->IsEmpty())
            {
               delete intersectRegion;
               continue;
            }

            if (AreRegionsEqual(intersectRegion, testAp1->region))
            {
               touchingList.RemoveAt(tempPos1);
               delete testAp1->region;
               dataList.RemoveAt(testAp1->dataPos); // remove from database because completely covered
               delete testAp1->data;
            }
            else if (AreRegionsEqual(intersectRegion, testAp2->region))
            {
               touchingList.RemoveAt(tempPos2);
               delete testAp2->region;
               dataList.RemoveAt(testAp2->dataPos); // remove from database because completely covered
               delete testAp2->data;
            }
         }
      }
*/
      // free region memory
      POSITION tempPos = touchingList.GetHeadPosition();
      while (tempPos)
      {
         ApStruct *ap = touchingList.GetNext(tempPos);
         delete ap->region;
      }

      // nothing touching to combine
      if (touchingList.GetCount() < 2)
      {
         touchingPos = touchingList.GetHeadPosition();
         while (touchingPos)
         {
            ApStruct *ap = touchingList.GetNext(touchingPos);
            delete ap;
         }
         touchingList.RemoveAll();
         continue;
      }
      
      // Find an unused name for the padshape
      CString padShapeName;
      padShapeName.Format("SHAPE_%d", ++shapeCount);
      BlockStruct* complexApertureGeometry = m_camCadDatabase.getNewBlock(padShapeName,"%d",BLOCKTYPE_UNKNOWN);

      BOOL first = TRUE;
      int firstLayer = 0;
      
      for (touchingPos = touchingList.GetHeadPosition();touchingPos!= NULL;)
      {
         ApStruct *ap = touchingList.GetNext(touchingPos);

         if (first)
         {
            firstLayer = ap->data->getLayerIndex();
            first = FALSE;
         }

         ap->data->getLayerIndex() = floatingLayerIndex;

         complexApertureGeometry->getDataList().AddTail(ap->data);
         dataList.RemoveAt(ap->dataPos);
      }

      double xMin,xMax,yMin,yMax;
      block_extents(&m_camCadDoc,&xMin,&xMax,&yMin,&yMax,&(complexApertureGeometry->DataList),0.,0.,0.,0,1.,-1,FALSE);
      double xCenter = (xMin + xMax)/2.;
      double yCenter = (yMin + yMax)/2.;
      
      for (touchingPos = touchingList.GetHeadPosition();touchingPos!= NULL;)
      {
         ApStruct *ap = touchingList.GetNext(touchingPos);

         ap->data->getInsert()->getOriginX() -= (DbUnit)xCenter;
         ap->data->getInsert()->getOriginY() -= (DbUnit)yCenter;

         delete ap;
      }

      touchingList.RemoveAll();

      // Insert complex aperture back into the file where it belong Graph_Complex
      CString complexName;
      complexName.Format("C%s_%d",(const char*)padShapeName, ++complexCount);
      BlockStruct* complexAperture = m_camCadDatabase.getDefinedAperture(complexName,apertureComplex,
                                         complexApertureGeometry->getBlockNumber());

      m_camCadDatabase.referenceBlock(&fileBlock,complexAperture,insertTypeUnknown,complexName,firstLayer,xCenter,yCenter);
   }
}

bool CPolyToFlashConverter::OptimizeComplexApertureBlock(BlockStruct& block,int insertLayer,double accuracy)
{
   bool retval = ((block.flag & BL_TOOL) == 0) &&
                 ((block.flag & (BL_APERTURE | BL_BLOCK_APERTURE)) != 0) &&
                 block.getShape() == apertureComplex;
         
   if (retval)
   {
      int subBlockNumber = block.getComplexApertureSubBlockNumber();
      BlockStruct *subblock = m_camCadDatabase.getBlock(subBlockNumber);

      double sizeA = 0;
      double sizeB = 0;
      double orient = 0;
      double xoffset = 0;
      double yoffset = 0;
      int shape;  // can be either rectangle or oblong

      if (Complex_2Circles1Rectangle(&subblock->getDataList(), accuracy, &sizeA, &sizeB, &orient, &xoffset, &yoffset))
      {
         if (fabs(sizeA - sizeB) < accuracy && fabs(xoffset) < accuracy && fabs(yoffset) < accuracy)
         {
            block.shape = T_ROUND;
            block.sizeA = (DbUnit)sizeA;
            block.sizeB = 0;
            block.sizeC = 0;
            block.sizeD = 0;
            block.xoffset = (DbUnit)xoffset;
            block.yoffset = (DbUnit)yoffset;
            block.rotation += 0;
         }
         else
         {
            block.shape = T_OBLONG;
            block.sizeA = (DbUnit)sizeA;
            block.sizeB = (DbUnit)sizeB;
            block.sizeC = 0;
            block.sizeD = 0;
            block.xoffset = (DbUnit)xoffset;
            block.yoffset = (DbUnit)yoffset;
            block.rotation += (DbUnit)OptimizeRotation(-orient);
         }

         if (fabs(xoffset) < accuracy)
            block.xoffset = 0;
         if (fabs(yoffset) < accuracy)
            block.yoffset = 0;

         // need to exchange all subblocknum
         Exchange_SubBlock(&m_camCadDoc,subBlockNumber,&block);
      }      
      else if (Complex_1Rectangle(&m_camCadDoc, &subblock->getDataList(), accuracy, &sizeA, &sizeB, &orient, &xoffset, &yoffset))
      {
         if (fabs(sizeA - sizeB) < accuracy)
         {
            block.shape = T_SQUARE;
            block.sizeA = (DbUnit)sizeA;
            block.sizeB = 0;
            block.sizeC = 0;
            block.sizeD = 0;
            block.xoffset = (DbUnit)xoffset;
            block.yoffset = (DbUnit)yoffset;
            block.rotation += (DbUnit)OptimizeRotation(orient);
         }
         else
         {
            block.shape = T_RECTANGLE;
            block.sizeA = (DbUnit)sizeA;
            block.sizeB = (DbUnit)sizeB;
            block.sizeC = 0;
            block.sizeD = 0;
            block.xoffset = (DbUnit)xoffset;
            block.yoffset = (DbUnit)yoffset;
            block.rotation += (DbUnit)OptimizeRotation(orient);
         }     

         if (fabs(xoffset) < accuracy)
            block.xoffset = 0;
         if (fabs(yoffset) < accuracy)
            block.yoffset = 0;

         // need to exchange all subblocknum
         Exchange_SubBlock(&m_camCadDoc,subBlockNumber,&block);
      }
      else if (Complex_1Circle(&m_camCadDoc, &subblock->getDataList(),accuracy, &sizeA, &xoffset, &yoffset))
      {
         block.shape = T_ROUND;
         block.sizeA = (DbUnit)sizeA;
         block.sizeB = 0;
         block.sizeC = 0;
         block.sizeD = 0;
         block.xoffset = (DbUnit)xoffset;
         block.yoffset = (DbUnit)yoffset;
         block.rotation += 0.0;
         
         if (fabs(xoffset) < accuracy)
            block.xoffset = 0;
         if (fabs(yoffset) < accuracy)
            block.yoffset = 0;
                  
         // need to exchange all subblocknum
         Exchange_SubBlock(&m_camCadDoc,subBlockNumber,&block);
      }
      else if (Complex_2PointLine(&m_camCadDoc, &subblock->getDataList(), accuracy, &sizeA, &sizeB, &orient, &xoffset, &yoffset, &shape))
      {
         if (fabs(xoffset) < accuracy) xoffset = 0;
         if (fabs(yoffset) < accuracy) yoffset = 0;

         if (shape == T_OBLONG && fabs(sizeA - sizeB) < accuracy)
         {
            block.shape = T_ROUND;
            block.sizeA = (DbUnit)sizeA;
            block.sizeB = 0;
            block.sizeC = 0;
            block.sizeD = 0;
            block.xoffset = (DbUnit)xoffset;
            block.yoffset = (DbUnit)yoffset;
            block.rotation += 0;
         }
         else
         {
            block.shape = shape;
            block.sizeA = (DbUnit)sizeA;
            block.sizeB = (DbUnit)sizeB;
            block.sizeC = 0;
            block.sizeD = 0;
            block.xoffset = (DbUnit)xoffset;
            block.yoffset = (DbUnit)yoffset;
            block.rotation += (DbUnit)OptimizeRotation(orient);
         }     

         if (fabs(xoffset) < accuracy)
            block.xoffset = 0;
         if (fabs(yoffset) < accuracy)
            block.yoffset = 0;

         // need to exchange all subblocknum
         Exchange_SubBlock(&m_camCadDoc,subBlockNumber,&block);
      }
   }

   return retval;
}

#endif  // EnableNewPolyToFlashConverter




