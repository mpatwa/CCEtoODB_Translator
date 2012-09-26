// $Header: /CAMCAD/5.0/DFT.cpp 155   6/30/07 2:25a Kurt Van Ness $
     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "CCEtoODB.h"
#include "DFT.h"
#include "ccdoc.h"
#include "attrib.h"
#include "graph.h"
#include "find.h"
#include "lyrmanip.h"
#include "dft_func.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "compvalues.h"
#include "net_util.h"
#include "Debug.h"
#include "MainFrm.h"
#include "DcaCamCadFileWriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________
static   CProbeArray    probearray;
static   int            probecnt;

static   DFT_ProbeGraphic  probegraphic;

/*****************************************************************************/
/*
*/
static void put_probes(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, const char *prefix,
                     double refheight, int layernr, int nevermirror)

{
   // update refname and possible ATT_REFNAME
   CString  pname;
   CUnits curUnit(doc->getSettings().getPageUnits());

   for (int i=0;i<probecnt;i++)
   {
      CDataList *dataList;
      DataStruct *data = FindDataEntity(doc, probearray[i]->getEntityNumber(), &dataList, NULL);
      if (data)
      {
         pname.Format("%s%d",prefix,probearray[i]->sequencenr/* + nameincrement*/);
         data->getInsert()->setRefname(STRDUP(pname));

         // get the probe drill size, use 100mil as default
         double drillSize = 0.1;
         BlockStruct *probeBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
         if (probeBlock != NULL)
         {
            POSITION pos = probeBlock->getHeadDataInsertPosition();
            while (pos)
            {
               DataStruct *drillData = probeBlock->getNextDataInsert(pos);
               BlockStruct *drillBlock = doc->getBlockAt(drillData->getInsert()->getBlockNumber());

               if (drillBlock->getBlockType() == blockTypeDrillHole)
               {
                  drillSize = drillBlock->getToolSize();
                  break;
               }
            }
         }

         // Here fix up the ATT_REFNAME
         CreateTestProbeRefnameAttr(doc, data, pname, drillSize, nevermirror?true:false);

     }
      else
      {
         // not found ????
         ErrorMessage("Entity not found in put_probe", "DFT.CPP");
      }
   }

   return;
}

/*****************************************************************************/
/*
*/
static void get_probes(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, TestProbeSequenceSurface tpSurface)
{
   DataStruct  *np;
   int         cnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      BOOL placed = TRUE;
      if (np->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)  
         continue;

      if (Attrib* attrib = is_attvalue(doc, np->getAttributeMap(), ATT_PROBEPLACEMENT, 0))
      {
         CString placement = attrib->getStringValue();
         if (!placement.CompareNoCase("Unplaced"))
            placed = FALSE;
      }

      if (placed && tpSurface == tpssUnplaced)
         continue;
      if (!placed && tpSurface != tpssUnplaced)
         continue;

      if (np->getInsert()->getPlacedBottom() != (tpSurface == tpssBottom))               
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      DFT_probes *c = new DFT_probes;
      probearray.SetAtGrow(probecnt++, c);

      c->setEntityNumber(np->getEntityNumber());
      c->probename = np->getInsert()->getRefname();
      c->x = np->getInsert()->getOriginX();
      c->y = np->getInsert()->getOriginY();
      c->bottom = np->getInsert()->getPlacedBottom();
      c->sequencenr = ++cnt;
      c->placed = placed;
   } // end get_probes */
}

/*****************************************************************************/
/* 

*/
static   int   read_probedeffile(CCEtoODBDoc *doc)
{
   char     line[255];
   char     *lp;
   FILE     *fp;

   probegraphic.scale = -1;
   probegraphic.PROBEGRIDLAYER = "PROBEGRID";
   probegraphic.PROBEDRILLTOP = "PROBEDRILLTOP";
   probegraphic.PROBEDRILLBOTTOM = "PROBEDRILLBOTTOM";
   probegraphic.PROBETOP = "PROBETOP";
   probegraphic.PROBEBOTTOM = "PROBEBOTTOM";
   probegraphic.PROBENAMETOP = "PROBENAMETOP";
   probegraphic.PROBENAMEBOTTOM = "PROBENAMEBOTTOM";
   probegraphic.PROBEPATHTOP = "PROBEPATHTOP";
   probegraphic.PROBEPATHBOTTOM = "PROBEPATHBOTTOM";
   probegraphic.framewidth = -1;

   CString probeFile(getApp().getSystemSettingsFilePath("default.prb"));
   if ((fp = fopen(probeFile,"rt")) != NULL)
   {
      while (fgets(line,255,fp))
      {
         if ((lp = get_string(line," \t\n")) == NULL) continue;
         if (lp[0] != '.')                         continue;

         if (!STRCMPI(lp,".PROBEUNITS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (!STRNICMP(lp,"IN",2))
            {
               probegraphic.scale = Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MI",2))
            {
               probegraphic.scale = Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MM",2))
            {
               probegraphic.scale = Units_Factor(UNIT_MM, doc->getSettings().getPageUnits());
            }
         }
         else
         if (!STRCMPI(lp,".PROBEPATHTOP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBEPATHTOP = lp;
         }
         else
         if (!STRCMPI(lp,".PROBEPATHBOTTOM"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBEPATHBOTTOM = lp;
         }
         else
         if (!STRCMPI(lp,".PROBENAMETOP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBENAMETOP = lp;
         }
         else
         if (!STRCMPI(lp,".PROBENAMEBOTTOM"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBENAMEBOTTOM = lp;
         }
         else
         if (!STRCMPI(lp,".PROBEGRIDLAYER"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBEGRIDLAYER = lp;
         }
         else
         if (!STRCMPI(lp,".FRAMEWIDTH"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.framewidth = atof(lp) * probegraphic.scale;
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
   return 1;
}

/******************************************************************************
* 
*/
static int do_snake_sort(int startingPoint, double stripewidth)
{
   ErrorMessage("Snake Algorithm not implemented -> Rename to Read Order", "DFT");

   return 1;
}

/******************************************************************************
* 
*/
static int plot_probes(CCEtoODBDoc *doc, TestProbeSequenceSurface tpSurface, const char *probepathlayer)
{
   if (tpSurface == tpssUnplaced)
      return 1;

   doc->UnselectAll(FALSE);

   LayerStruct *ll;

   if ((ll = doc->FindLayer_by_Name(probepathlayer)) != NULL)
      DeleteLayer(doc, ll, TRUE);


   // top layer
   int l = Graph_Level(probepathlayer, "", 0);
   LayerStruct *lp = doc->FindLayer(l);

   if (tpSurface == tpssBottom) // only if no type yet assigned
      lp->setLayerType(LAYTYPE_BOTTOM);
   else if (tpSurface == tpssTop)
      lp->setLayerType(LAYTYPE_TOP);

   DataStruct *d = Graph_PolyStruct(l,0L,0);
   Graph_Poly(NULL, 0, 0, 0, 0);

   for (int i=0;i<probecnt;i++)
   {
#ifdef _DEBUG
   DFT_probes *pp = probearray[i];
#endif
      if (probearray[i]->bottom != (tpSurface == tpssBottom))   continue;
      Graph_Vertex(probearray[i]->x, probearray[i]->y, 0.0);
   }
   return 1;
}

/*****************************************************************************/
/*
   -1
   0
   +1
*/
static int probekooCompareFunc( const void *arg1, const void *arg2 )
{
   DFT_probes **a1, **a2;
   a1 = (DFT_probes**)arg1;
   a2 = (DFT_probes**)arg2;
   
   double dist1, dist2;

   //dist1 = (*a1)->xnorm + (*a1)->ynorm;
   //dist2 = (*a2)->xnorm + (*a2)->ynorm;
   dist1 = (*a1)->ynorm;
   dist2 = (*a2)->ynorm;
   
   if (dist1 < dist2)   return -1;
   if (dist1 > dist2)   return +1;

   // if equal use the lower x
   dist1 = (*a1)->xnorm;
   dist2 = (*a2)->xnorm;

   if (dist1 < dist2)   return -1;
   if (dist1 > dist2)   return +1;

   return 0;
}

/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is 
   because of the 2 pass to get artwork info.
*/
static void SortprobeData_LowerLeft_Stripe(int maxsteps)
{
   if (probecnt < 2) return;

   
   // set all to unused
   int   i;
   int   used = 0;

   for (i=0;i<probecnt;i++)
      probearray[i]->used = used;   // init to 0

/*
   // loop thru all xstep
   for (int s=0;s<maxsteps;s++)
   {
      double dist = FLT_MAX;
      int    distindex = -1;

      for (i=0;i<probecnt;i++)
      {
         if (probearray[i]->xstep != s)   continue;
         if (probearray[i]->used)         continue;

         // find lower left and unused.
         if ((probearray[i]->xnorm + probearray[i]->ynorm) < dist)
         {

            dist = probearray[i]->xnorm + probearray[i]->ynorm;
            distindex = i;
         }
      }

   }
*/
   qsort(probearray.GetData(),probecnt, sizeof(DFT_probes *), probekooCompareFunc);

   return;
}

/******************************************************************************
* 
*/
static int do_stripe_sort(FileStruct *file, int startingPoint, double stripewidth)
{
   int      xstep = 0;
   int      ystep = 0;
   double   maxx = file->getBlock()->getXmax(); //-FLT_MAX;
   double   maxy = file->getBlock()->getYmax(); //-FLT_MAX;
   double   minx = file->getBlock()->getXmin(); // FLT_MAX;
   double   miny = file->getBlock()->getYmin(); // FLT_MAX;
/*
   for (i=0;i<probecnt;i++)
   {
      if (probearray[i]->x > maxx)
         maxx = probearray[i]->x;
      if (probearray[i]->y > maxy)
         maxy = probearray[i]->y;
      if (probearray[i]->x < minx)
         minx = probearray[i]->x;
      if (probearray[i]->y < miny)
         miny = probearray[i]->y;
   }
*/
   int   maxxsteps;

   maxxsteps = (int)floor((maxx - minx) / stripewidth) + 1;

   // assume starting point lowerleft
   for (int i=0;i<probecnt;i++)
   {
#ifdef _DEBUG
   DFT_probes *pp = probearray[i];
#endif
      probearray[i]->xstep = (int)floor((probearray[i]->x - minx) / stripewidth); // if starting point is lower left
      probearray[i]->ystep = 0;
      probearray[i]->xnorm = probearray[i]->x - minx + probearray[i]->ystep*(maxx - minx);
      probearray[i]->ynorm = probearray[i]->y - miny + probearray[i]->xstep*(maxy - miny);
   }

   // the sort checks the xsteps first and than sorts inside the xsteps for distance
   SortprobeData_LowerLeft_Stripe(maxxsteps*2);

   return 1;
}

/*****************************************************************************/
/*
*/
static int do_layer(CCEtoODBDoc *doc, FileStruct *file, TestProbeSequenceSurface tpSurface, int algorithm, 
                     const char *prefix, int startingPoint, double bandwidth, 
                     int startNumber, int nameIncrement)
{
   probearray.SetSize(100,100);
   probecnt = 0;

   // get all probes
   get_probes(doc, file, &(file->getBlock()->getDataList()), tpSurface);

   // sort
   if (algorithm == 0)  // snake
      do_snake_sort(startingPoint, bandwidth);
   else
   if (algorithm == 1)  // stripe
      do_stripe_sort(file, startingPoint, bandwidth);
   else
      ErrorMessage("Unknown Sort Algorithm -> Rename to Read Order", "DFT");

   CString  probepathlayer;
   if (tpSurface == tpssBottom)
      probepathlayer = probegraphic.PROBEPATHBOTTOM;
   else
      probepathlayer = probegraphic.PROBEPATHTOP;

   plot_probes(doc, tpSurface, probepathlayer);

   // update the sequence number, used in names.
   int probeIndex = startNumber;
	int i=0;
   for (i=0;i<probecnt;i++)
      probearray[i]->sequencenr = probeIndex + (nameIncrement*i);
   probeIndex += nameIncrement * probecnt;

   int neverMirror = FALSE;
   double refnameheight = 0.05 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());

   // make refdes layers
   int l = Graph_Level(probegraphic.PROBENAMETOP, "", 0);
   LayerStruct *lp = doc->FindLayer(l);
   if (lp->getLayerType() == 0) // only if no type yet assigned
      lp->setLayerType(LAYTYPE_TOP);
   l = Graph_Level(probegraphic.PROBENAMEBOTTOM, "", 0);
   lp = doc->FindLayer(l);
   if (lp->getLayerType() == 0) // only if no type yet assigned
      lp->setLayerType(LAYTYPE_BOTTOM);
   Graph_Level_Mirror(probegraphic.PROBENAMETOP, probegraphic.PROBENAMEBOTTOM, "");

   // update names
   put_probes(doc, file, &(file->getBlock()->getDataList()), prefix, refnameheight,
               Graph_Level(probegraphic.PROBENAMETOP,"",0) , neverMirror);

   for (i=0;i<probecnt;i++)
   {
      delete probearray[i];
   }

   probearray.RemoveAll();
   probecnt = 0;

   return probeIndex;
}

/*****************************************************************************/
/*
*/
static int plot_frame(CCEtoODBDoc *doc, double stripewidth, double x_min, double y_min, double x_max, double y_max)
{
   LayerStruct *ll;
   if ((ll = doc->FindLayer_by_Name(probegraphic.PROBEGRIDLAYER)) != NULL)
      DeleteLayer(doc, ll, TRUE);

   int l = Graph_Level(probegraphic.PROBEGRIDLAYER, "", 0);

   int xintstep = (int)floor((x_max - x_min) / stripewidth) + 1;
   int yintstep = (int)floor((y_max - y_min) / stripewidth) + 1;

   double   totxwidth = xintstep * stripewidth;
   double   totywidth = yintstep * stripewidth;
   double   frameminx = x_min, framemaxx = x_max, frameminy= y_min, framemaxy = y_max;

   frameminx = x_min - (totxwidth - (x_max - x_min))/2;
   frameminy = y_min - (totywidth - (y_max - y_min))/2;
   framemaxx = frameminx + totxwidth;
   framemaxy = frameminy + totywidth;

   DataStruct *d = Graph_PolyStruct(l,0L,0);

   // do not make the first alpha devider !
   double step = frameminx + stripewidth;
   int   i;
   for (i=0;i<xintstep-1;i++) // this make the X deviders, not first, not last
   {
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( step, frameminy, 0.0);
      Graph_Vertex( step, frameminy-probegraphic.framewidth, 0.0);
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( step, framemaxy, 0.0);
      Graph_Vertex( step, framemaxy+probegraphic.framewidth, 0.0);
      step += stripewidth;
   }

   // do not make the first numner devider !
   step = frameminy + stripewidth;
   for (i=0;i<yintstep-1;i++) // this make the Y deviders, not first, not last
   {
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( frameminx, step, 0.0);
      Graph_Vertex( frameminx-probegraphic.framewidth, step, 0.0);
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( framemaxx, step, 0.0);
      Graph_Vertex( framemaxx+probegraphic.framewidth, step, 0.0);
      step += stripewidth;
   }

   // inner frame
   Graph_Poly(NULL, 0, 0, 0, 0);
   Graph_Vertex( frameminx, frameminy, 0.0);
   Graph_Vertex( framemaxx, frameminy, 0.0);
   Graph_Vertex( framemaxx, framemaxy, 0.0);
   Graph_Vertex( frameminx, framemaxy, 0.0);
   Graph_Vertex( frameminx, frameminy, 0.0);

   // outer frame
   Graph_Poly(NULL, 0, 0, 0, 0);
   Graph_Vertex( frameminx-probegraphic.framewidth, frameminy-probegraphic.framewidth, 0.0);
   Graph_Vertex( framemaxx+probegraphic.framewidth, frameminy-probegraphic.framewidth, 0.0);
   Graph_Vertex( framemaxx+probegraphic.framewidth, framemaxy+probegraphic.framewidth, 0.0);
   Graph_Vertex( frameminx-probegraphic.framewidth, framemaxy+probegraphic.framewidth, 0.0);
   Graph_Vertex( frameminx-probegraphic.framewidth, frameminy-probegraphic.framewidth, 0.0);

   // now nomenclature X = A..Z, Y = 1..9


   return 1;
}

/******************************************************************************
* 
   This function can also be called from TA and other modules
*/
int DFT_TestProbeSequence(CCEtoODBDoc *doc, FileStruct *file, int algorithm, 
                          int startingPoint, const char *prefix, double bandwidth, 
                          int startNumber, int nameIncrement)
{
   doc->PrepareAddEntity(file);

   LayerStruct *ll;
   if ((ll = doc->FindLayer_by_Name(probegraphic.PROBEGRIDLAYER)) != NULL)
      DeleteLayer(doc, ll, TRUE);
   if ((ll = doc->FindLayer_by_Name(probegraphic.PROBEPATHBOTTOM)) != NULL)
      DeleteLayer(doc, ll, TRUE);
   if ((ll = doc->FindLayer_by_Name(probegraphic.PROBEPATHTOP)) != NULL)
      DeleteLayer(doc, ll, TRUE);
   
   // recalculate here, so that the file extens are same for the second run.
   doc->CalcBlockExtents(file->getBlock());

   plot_frame(doc, bandwidth, 
      file->getBlock()->getXmin(), file->getBlock()->getYmin(),
      file->getBlock()->getXmax(), file->getBlock()->getYmax());

   int probeIndex = do_layer(doc, file, tpssTop, algorithm, prefix, startingPoint, bandwidth, startNumber, nameIncrement); // top
   probeIndex = do_layer(doc, file, tpssBottom, algorithm, prefix, startingPoint, bandwidth, probeIndex, nameIncrement); // bottom
   probeIndex = do_layer(doc, file, tpssUnplaced, algorithm, prefix, startingPoint, bandwidth, probeIndex, nameIncrement); // unplaced

   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// New DFT Toolstack Functions
/////////////////////////////////////////////////////////////////////////////

/******************************************************************************
* CreateSTDTool
*/
BlockStruct *CCEtoODBDoc::CreateSTDTool(CString toolStackGeom, int &block_type)
{
   CString toolName = toolStackGeom;
   int overwrite_err = 0, aperShape = T_UNDEFINED;
   double sizeInner, sizeOuter;

   toolName.MakeUpper();
   if (toolName == "T100X100TS")
   {
      aperShape = T_DONUT;
      sizeOuter = 0.0689;
      sizeInner = 0.0639;
      block_type = BLOCKTYPE_TEST_ACCESSPOINT;
   }
   else if (toolName == "T100X75TS")
   {
      aperShape = T_DONUT;
      sizeOuter = 0.0689;
      sizeInner = 0.055;
      block_type = BLOCKTYPE_TEST_ACCESSPOINT;
   }
   else if (toolName == "T100X50TS")
   {
      aperShape = T_DONUT;
      sizeOuter = 0.0689;
      sizeInner = 0.039;
      block_type = BLOCKTYPE_TEST_ACCESSPOINT;
   }
   else if (toolName == "T75X75TS")
   {
      aperShape = T_DONUT;
      sizeOuter = 0.055;
      sizeInner = 0.050;
      block_type = BLOCKTYPE_TEST_ACCESSPOINT;
   }
   else if (toolName == "T75X50TS")
   {
      aperShape = T_DONUT;
      sizeOuter = 0.055;
      sizeInner = 0.039;
      block_type = BLOCKTYPE_TEST_ACCESSPOINT;
   }
   else if (toolName == "T50X50TS")
   {
      aperShape = T_DONUT;
      sizeOuter = 0.039;
      sizeInner = 0.034;
      block_type = BLOCKTYPE_TEST_ACCESSPOINT;
   }
   else if (toolName == "BLOCKEDTS")
   {
      aperShape = T_TARGET;
      sizeOuter = 0.0689;
      block_type = BLOCKTYPE_TEST_ACCESSPOINT;
   }
   else if (toolName == "NAIL100TS")
   {
      aperShape = T_ROUND;
      sizeOuter = 0.0689;
      block_type = BLOCKTYPE_TEST_PROBE;
   }
   else if (toolName == "NAIL75TS")
   {
      aperShape = T_ROUND;
      sizeOuter = 0.055;
      block_type = BLOCKTYPE_TEST_PROBE;
   }
   else if (toolName == "NAIL50TS")
   {
      aperShape = T_ROUND;
      sizeOuter = 0.039;
      block_type = BLOCKTYPE_TEST_PROBE;
   }

   int width = Graph_Aperture(toolName, aperShape, sizeOuter * Units_Factor(UNIT_INCHES, getSettings().getPageUnits()), sizeInner * Units_Factor(UNIT_INCHES, getSettings().getPageUnits()),
      0.0, 0.0, 0.0, 0, 0, TRUE, &overwrite_err);

   BlockStruct *aper = NULL;
   if (!overwrite_err)
      aper = getWidthTable()[width];

   return aper;
}

/******************************************************************************
* InsertToolStack
*/
DataStruct *CCEtoODBDoc::InsertToolStack(CString pcb, CString toolStackID, CString toolStackGeom, double x, double y)
{
   POSITION pos = getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = (FileStruct*)getFileList().GetNext(pos);

      if (pcb.Compare(file->getBlock()->getName()))
         return InsertToolStack(file->getBlock()->getBlockNumber(), toolStackID, toolStackGeom, x, y);
   }
   return NULL;
}

/******************************************************************************
* InsertToolStack
*/
DataStruct *CCEtoODBDoc::InsertToolStack(int pcb, CString toolStackID, CString toolStackGeom, double x, double y)
{
   BlockStruct *pcbBlock = getBlockAt(pcb);
   if (!pcbBlock)
      return NULL;

   int blockType = 0;
   BlockStruct *toolBlock = Find_Block_by_Name(toolStackGeom, -1);
   if (!toolBlock)
   {
      if (!(toolBlock = CreateSTDTool(toolStackGeom, blockType)))
         return NULL;
   }

   // create the new DFT tool layers
   int dftAccessLayer = Graph_Level("DFT_Access", "", FALSE);
   int dftNailsLayer = Graph_Level("DFT_Nails", "", FALSE);

   // create the insert
   DataStruct *data = getCamCadData().getNewDataStruct(dataTypeInsert);
   //data->getInsert() = new InsertStruct();
   //data->setDataType(dataTypeInsert);
   data->getInsert()->setRefname(STRDUP(toolStackID));
   data->getInsert()->setBlockNumber(toolBlock->getBlockNumber());
   data->getInsert()->setOriginX(x);
   data->getInsert()->setOriginY(y);

   if (blockType == BLOCKTYPE_TEST_PROBE)
   {
      data->getInsert()->setInsertType(insertTypeTestProbe);
      data->setLayerIndex(dftNailsLayer);
   }
   else if (blockType == BLOCKTYPE_TEST_ACCESSPOINT)
   {
      data->getInsert()->setInsertType(insertTypeTestAccessPoint);
      data->setLayerIndex(dftAccessLayer);
   }

   Graph_Block_On(GBO_APPEND, pcbBlock->getName(), pcbBlock->getFileNumber(), pcbBlock->getFlags());
   AddEntity(data);
   Graph_Block_Off();

   return data;
}

/******************************************************************************
* DeleteToolStack
*/
void CCEtoODBDoc::DeleteToolStack(CString pcb, CString toolStackID)
{
   POSITION pos = getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = (FileStruct*)getFileList().GetNext(pos);

      if (pcb.Compare(file->getBlock()->getName()))
         return DeleteToolStack(file->getBlock()->getBlockNumber(), toolStackID);
   }
}

/******************************************************************************
* DeleteToolStack
*/
void CCEtoODBDoc::DeleteToolStack(int pcb, CString toolStackID)
{
   BlockStruct *pcbBlock = getBlockAt(pcb);
   if (!pcbBlock)
      return;

   POSITION pos = pcbBlock->getDataList().GetHeadPosition();
   while (pos)
   {
      // keep the position in case we need to remove this one
      POSITION remPos = pos;
      DataStruct *data = pcbBlock->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE ||
         data->getInsert()->getInsertType() != INSERTTYPE_TEST_ACCESSPOINT)
         continue;

      if (!toolStackID.Compare(data->getInsert()->getRefname()))
      {
         // remove the tool insert
         pcbBlock->getDataList().RemoveAt(remPos);

         delete data;
      }
   }
}

/******************************************************************************
* DeleteAllToolStacks
*/
void CCEtoODBDoc::DeleteAllToolStacks(CString pcb)
{
   POSITION pos = getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = (FileStruct*)getFileList().GetNext(pos);

      if (pcb.Compare(file->getBlock()->getName()))
         return DeleteAllToolStacks(file->getBlock()->getBlockNumber());
   }
}

/******************************************************************************
* DeleteAllToolStacks
*/
void CCEtoODBDoc::DeleteAllToolStacks(int pcb)
{
   BlockStruct *pcbBlock = getBlockAt(pcb);
   if (!pcbBlock)
      return;

   POSITION pos = pcbBlock->getDataList().GetHeadPosition();
   while (pos)
   {
      // keep the position in case we need to remove this one
      POSITION remPos = pos;
      DataStruct *data = pcbBlock->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE ||
         data->getInsert()->getInsertType() != INSERTTYPE_TEST_ACCESSPOINT)
         continue;

      // remove the tool insert
      pcbBlock->getDataList().RemoveAt(remPos);

      delete data;
   }
}


/******************************************************************************
* RemoveTestAccessAttributes
*/
void RemoveTestAccessAttributes(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top, bool bottom)
{
   WORD testAccessKey = (WORD)doc->RegisterKeyWord(ATT_TESTACCESS, 0, VT_STRING);
   WORD testAccessFailKey = (WORD)doc->RegisterKeyWord(ATT_TESTACCESS_FAIL, 0, VT_STRING);

   POSITION dataPos = pcbFile->getBlock()->getHeadDataInsertPosition();
   while (dataPos)
   {
      DataStruct *data = pcbFile->getBlock()->getNextDataInsert(dataPos);

      if (data->getInsert()->getInsertType() == insertTypeVia &&
         ((top && data->getInsert()->getPlacedTop()) || (bottom && data->getInsert()->getPlacedBottom())))
      {
         data->removeAttrib(testAccessKey);
         data->removeAttrib(testAccessFailKey);
      }
   }

   POSITION netPos = pcbFile->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = pcbFile->getNetList().GetNext(netPos);
      if (net == NULL)
         continue;

      POSITION comppinPos = net->getCompPinList().getHeadPosition();
      while (comppinPos)
      {
         CompPinStruct *comppin = net->getCompPinList().getNext(comppinPos);
         CAttributes *attribMap = comppin->getAttributes();
         if (attribMap != NULL)
         {
            RemoveAttrib(testAccessKey, &comppin->getAttributesRef());
            RemoveAttrib(testAccessFailKey, &comppin->getAttributesRef());
         }
      }
   }
}

//******************************************************************************

// These two funcs are for backward compatibility with existing code.
// Could refactor them away sometime.
void DeleteAllTestAccessPoints(CCEtoODBDoc *doc, FileStruct *pcbFile)
{ 
   DeleteTestAccessPoints(doc, pcbFile, true, true); 
}

void DeleteAllProbes(CCEtoODBDoc *doc, FileStruct *pcbFile)
{ 
   DeleteTestProbes(doc, pcbFile, true, true);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// Deleting an access marker requires deleting associated probes too.

void CCEtoODBDoc::OnDftDeleteAccessMarkersTop()
{
   FileStruct *pcbfile = this->getCamCadData().getSingleVisiblePcb();
   DeleteTestAccessPoints(this, pcbfile, true, false);
   DeleteTestProbes(this, pcbfile, true, false);
}

void CCEtoODBDoc::OnDftDeleteAccessMarkersBottom()
{
   FileStruct *pcbfile = this->getCamCadData().getSingleVisiblePcb();
   DeleteTestAccessPoints(this, pcbfile, false, true);
   DeleteTestProbes(this, pcbfile, false, true);
}

void CCEtoODBDoc::OnDftDeleteAccessMarkersBoth()
{
   FileStruct *pcbfile = this->getCamCadData().getSingleVisiblePcb();
   DeleteTestAccessPoints(this, pcbfile, true, true);
   DeleteTestProbes(this, pcbfile, true, true);
}

//------------------------------------------------------------------------------

void CCEtoODBDoc::OnDftDeleteTestProbesTop()
{
   DeleteTestProbes(this, this->getCamCadData().getSingleVisiblePcb(), true, false);
}

void CCEtoODBDoc::OnDftDeleteTestProbesBottom()
{
   DeleteTestProbes(this, this->getCamCadData().getSingleVisiblePcb(), false, true);
}

void CCEtoODBDoc::OnDftDeleteTestProbesBoth()
{
   DeleteTestProbes(this, this->getCamCadData().getSingleVisiblePcb(), true, true);
}

/******************************************************************************

/******************************************************************************
* DeleteAllTestAccessPoints
*/
void DeleteTestAccessPoints(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top, bool bottom)
{
   if (doc != NULL && pcbFile != NULL)
   {
      POSITION dataPos = pcbFile->getBlock()->getHeadDataInsertPosition();
      while (dataPos)
      {
         DataStruct *data = pcbFile->getBlock()->getNextDataInsert(dataPos);
         if (data->getInsert()->getInsertType() == insertTypeTestAccessPoint &&
            ((top && data->getInsert()->getPlacedTop()) || (bottom && data->getInsert()->getPlacedBottom())))
         {
            //pcbFile->getBlock()->RemoveDataFromList(doc, data);
            doc->removeDataFromDataList(*(pcbFile->getBlock()),data);
         }
      }

      POSITION drcPos = pcbFile->getDRCList().GetHeadPosition();
      while (drcPos)
      {
         DRCStruct *drc = pcbFile->getDRCList().GetNext(drcPos);

         if (drc->getAlgorithmType() == DFT_ALG_NET_WITHOUT_ACCESS)
            RemoveOneDRC(doc, drc, pcbFile);
      }

      CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
      frame->getNavigator().setDoc(doc);
   }
}

/******************************************************************************
* PlaceTestAccessPoint
*/
DataStruct *PlaceTestAccessPoint(CCEtoODBDoc *doc, BlockStruct *pcbFileBlock, CAAAccessLocation *accLoc, 
                                 int dataLink, int pageUnits)
{
   CString refname = accLoc->GetName();
   double x = accLoc->GetAccessLocation().x;
   double y = accLoc->GetAccessLocation().y;
   ETestSurface accessSurface = accLoc->GetAccessSurface();
   CString netName = accLoc->GetNetName();
   double exposedMetalDiameter = accLoc->GetExposedMetalDiameter();
   CString targetType = accLoc->GetTargetTypeToString();

   DataStruct* accData = PlaceTestAccessPoint(doc, pcbFileBlock, refname, x, y, accessSurface, netName, targetType, 
      dataLink, exposedMetalDiameter, doc->getSettings().getPageUnits(), accLoc->getId(), accLoc->getExposedDataId());

   return accData;
}

/******************************************************************************
* PlaceTestAccessPoint
*/
DataStruct *PlaceTestAccessPoint(CCEtoODBDoc *doc, BlockStruct *pcbFileBlock, CString refName, double x, double y, 
                                 ETestSurface surface, CString netName, CString targetType, int dataLink,
                                 double exposeMetalDiameter, int pageUnits,int accessPointId,int exposedDataId)
{
   double size = 0.03 * Units_Factor(DFT_DEFAULT_UNIT, pageUnits);
   int layerIndex = Graph_Level("0", "", TRUE);
   BlockStruct *testaccessblock =  generate_TestAccessGeometry(doc, "TEST_ACCESS", size);

   Graph_Block_On(pcbFileBlock);

   DataStruct *data = NULL;
   if (surface == DFT_SURFACE_TOP)
      data = Graph_Block_Reference(testaccessblock->getName(), refName, -1, x, y, 0.0, 0, 1.0, -1, TRUE);
   else
      data = Graph_Block_Reference(testaccessblock->getName(), refName, -1, x, y, 0.0, 1, 1.0, -1, TRUE);

   // Case dts0100465858.
   // If incoming refname was blank, then make up a name and assign it.
   // Made up name is based on entity number, which is "guaranteed" to be unique, so we don't have
   // to go checking all over to find all the names in use. Though we do have to alter the format
   // slightly, so it does not collide with $$ACCESS_nn (which has been teh standard naming format)
   // we'll use $$ACCESS_ENTITY_nn.
   // It also means we can't fix up the name until after the insert is made, just above here, since 
   // that is what will provide the entity number.
   if (data != NULL && refName.IsEmpty())
   {
      refName.Format("$$ACCESS_ENTITY_%d", data->getEntityNumber());
      data->getInsert()->setRefname(refName);
   }

   if (data != NULL)
   {
      data->getInsert()->setInsertType(insertTypeTestAccessPoint);

      int keyword = doc->IsKeyWord(ATT_REFNAME, 0);
      doc->SetAttrib(&data->getDefinedAttributes(), keyword, VT_STRING, refName.GetBuffer(0), SA_OVERWRITE, NULL);

      keyword = doc->IsKeyWord(ATT_NETNAME, 0);
      doc->SetAttrib(&data->getDefinedAttributes(), keyword, VT_STRING, netName.GetBuffer(0), SA_OVERWRITE, NULL);

      keyword = doc->RegisterKeyWord(DFT_ATT_TARGET_TYPE, 0, VT_STRING);
      doc->SetAttrib(&data->getDefinedAttributes(), keyword, VT_STRING, targetType.GetBuffer(0), SA_OVERWRITE, NULL);
   
      keyword = doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);
      doc->SetAttrib(&data->getDefinedAttributes(), keyword, VT_INTEGER, (void*)&dataLink, SA_OVERWRITE, NULL);

      keyword = doc->RegisterKeyWord(DFT_ATT_EXPOSE_METAL_DIAMETER, 0, valueTypeUnitDouble);
      doc->SetAttrib(&data->getDefinedAttributes(), keyword, valueTypeUnitDouble, (void*)&exposeMetalDiameter, SA_OVERWRITE, NULL);

      // Debug
      keyword = doc->RegisterKeyWord("AccessPointId", 0, valueTypeInteger);
      doc->SetAttrib(&data->getDefinedAttributes(), keyword, valueTypeInteger, (void*)&accessPointId, SA_OVERWRITE, NULL);

      keyword = doc->RegisterKeyWord("ExposedDataId", 0, valueTypeInteger);
      doc->SetAttrib(&data->getDefinedAttributes(), keyword, valueTypeInteger, (void*)&exposedDataId, SA_OVERWRITE, NULL);
   }

   Graph_Block_Off();

   return data;
}

/******************************************************************************
* FindAccessDataForEntityNum
*/
DataStruct *FindAccessDataForEntityNum(CCEtoODBDoc *doc, BlockStruct *pcbFileBlock, long entityNumber)
{
   if (doc != NULL && pcbFileBlock != NULL)
   {
      POSITION dataPos = pcbFileBlock->getHeadDataInsertPosition();
      while (dataPos)
      {
         DataStruct* accessData = pcbFileBlock->getNextDataInsert(dataPos);

         if (accessData != NULL && accessData->isInsertType(insertTypeTestAccessPoint))
         {
            Attrib *attrib = is_attvalue(doc, accessData->getAttributesRef(), ATT_DDLINK, 1);

            if (attrib != NULL && attrib->getIntValue() == entityNumber)
            {
               return accessData;
            }
         }
      }
   }

   return NULL;
}

/******************************************************************************
* PlaceTestProbe
*/
DataStruct *PlaceTestProbe(int probeNameNominalSizeMils, 
                           CCEtoODBDoc *doc,  BlockStruct *pcbFileBlock, 
                           CString refName, double x, double y, double rot, ETestSurface surface,  ETestResourceType testResourceType,
                           CString netName, DataStruct *accessData)
{
   // probeNameNominalSizeMils correlates to our standard probe definitions for which we
   // assign probe names based on nominal probe size in mils.
   // There are three recognized sizes, 100, 75, and 50.
   // Using int for param rather than string because param passing and testing is faster, and
   // it is not a general arbitrary name value.
   // We test for values 100 and 75, and if value is not one of these then you get a 50.

   // The default probe sizes are set up elsewhere in this cpp file.
   // The Add_TopProbes call is provided here as a comment to show where values come from.
   // Values in Add_TopProbes call are in Mils units.
   // We should put these into a table and have both places use the table rather than duplicate the constants.
   // Maybe later.
      
   // m_probeTemplates.Add_TopProbes(true, "100", 90, 67, 0.0,  50, false, 30);
   if (probeNameNominalSizeMils == 100)
   {
      return PlaceTestProbe(doc, pcbFileBlock,
         refName, x, y, rot, surface, testResourceType,
         netName, accessData,
         "100", .090, .067);
   }

   // m_probeTemplates.Add_TopProbes(true,  "75", 70, 55, 0.0, 100, false, 20);
   if (probeNameNominalSizeMils == 75)
   {
      return PlaceTestProbe(doc, pcbFileBlock,
         refName, x, y, rot, surface, testResourceType,
         netName, accessData,
         "75", .070, .055 );
   }

   // Default to 50
   // m_probeTemplates.Add_TopProbes(true,  "50", 49, 37, 0.0, 200, false, 10);

   return PlaceTestProbe(doc, pcbFileBlock,
      refName, x, y, rot, surface, testResourceType,
      netName, accessData,
      "50", .049, .037 );

}
                           
/******************************************************************************
* PlaceTestProbe
*/
DataStruct *PlaceTestProbe(CCEtoODBDoc *doc,  BlockStruct *pcbFileBlock, 
                           CString refName, double x, double y, double rot, ETestSurface surface,  ETestResourceType testResourceType,
									CString netName, DataStruct *accessData,
                           CString probeBaseName, double probediamInch, double drillsizeInch)
{
   double unitfactor = Units_Factor(UNIT_INCHES, doc->getPageUnits());
   int mirror = (surface == testSurfaceTop)?FALSE:TRUE;
   double drillsize =  drillsizeInch * unitfactor;
   double probdiam = probediamInch * unitfactor;

   CString probeGeomName(probeBaseName);
   probeGeomName += ((surface == testSurfaceTop) ? "_Top" : "_Bot");
   
   BlockStruct *probeBlock = CreateTestProbeGeometry(doc, probeGeomName, probdiam, probeGeomName, drillsize);

   Graph_Block_On(pcbFileBlock);
   //DataStruct *probeData = Graph_Block_Reference(probeBlock->getName(), refName, probeBlock->getFileNumber(), x, y, rot, mirror, 1.0, -1, FALSE);
   DataStruct *probeData = Graph_Block_Reference(probeBlock->getBlockNumber(), refName, x, y, rot, mirror, 1.0, -1);
   Graph_Block_Off();
   probeData->getInsert()->setInsertType(insertTypeTestProbe);
   

   // Set attributes
   // PROBEPLACEMENT and DDLINK depend on accessEntity
   int keyword = 0;
   if (accessData != NULL) // Access point available, mark probe as placed and visible, and set up datalink
   {
      keyword = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
      probeData->setAttrib(doc->getCamCadData(), keyword, valueTypeString, (void*)"Placed", attributeUpdateOverwrite, NULL);
      
      int accessEntityNum = accessData->getEntityNumber();
      keyword = doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);
      probeData->setAttrib(doc->getCamCadData(), keyword, valueTypeInteger, (void*)&accessEntityNum, attributeUpdateOverwrite, NULL);
      probeData->setHidden(false);
   }
   else // No access point, mark probe as unplaced and invisible, no datalink
   {
      keyword = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
      probeData->setAttrib(doc->getCamCadData(), keyword, valueTypeString, (void*)"Unplaced", attributeUpdateOverwrite, NULL);
      probeData->setHidden(true);
   }
   
   // Always set TEST_RESOURCE, NETNAME and REFNAME
   CString resourceType( testResourceType == testResourceTypePowerInjection ? "Power Injection" : "Test" );
   keyword = doc->RegisterKeyWord(ATT_TESTRESOURCE, 0, VT_STRING);
   probeData->setAttrib(doc->getCamCadData(), keyword, valueTypeString, resourceType.GetBuffer(0), attributeUpdateOverwrite, NULL);

   keyword = doc->IsKeyWord(ATT_NETNAME, 0);
   probeData->setAttrib(doc->getCamCadData(), keyword, valueTypeString, netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
   
   keyword = doc->RegisterKeyWord(ATT_REFNAME, 0, VT_STRING);
   probeData->setAttrib(doc->getCamCadData(), keyword, valueTypeString, refName.GetBuffer(0), attributeUpdateOverwrite, NULL);
   CreateTestProbeRefnameAttr(doc, probeData, refName, drillsize, true /*nevermirror*/);

   return probeData;
}

/******************************************************************************
* PlaceAccessAndProbe
*/
int PlaceAccessAndProbe(int probeNameNominalSizeMils, CCEtoODBDoc *doc,  FileStruct *cur_file, double x, double y, double rot, 
								long targetEntityNum, CString refName, CString netName, ETestSurface pcbside) 
{
   ETestSurface side;
   int keyword = 0;
   long accessDatalink = -1;
   
   side = pcbside;	
   doc->PrepareAddEntity(cur_file);

   DataStruct *accessData = FindAccessDataForEntityNum(doc, cur_file->getBlock(), targetEntityNum);

   if (accessData == NULL)
   {
      accessData = PlaceTestAccessPoint(doc, cur_file->getBlock(), "", x, y , side, netName, "SMD", targetEntityNum, 0,
						doc->getSettings().getPageUnits());
   }
   
   // Using access mark XY potentially overrides input XY, not sure if that is good, but it is in
   // keeping with original form of this function so we shall continue to do it until we find a
   // reason not to.
   // It will be the same as input XY if we just placed the access marker, but if access marker
   // already existed it might be different.
   if (accessData != NULL) // since this function is supposed to place both, maybe should not place probe if access is still null?
   {
      x = accessData->getInsert()->getOriginX();
      y = accessData->getInsert()->getOriginY();
   }
   DataStruct *probeData = PlaceTestProbe(probeNameNominalSizeMils, doc, cur_file->getBlock(), refName, x, y, rot, side, testResourceTypeTest, netName, accessData);

   doc->UpdateAllViews(NULL);
   return 0;
}

/******************************************************************************
* Kelvin Analysis Functions
******************************************************************************/
int setMultiTestNetProbes(CCEtoODBDoc *doc, DataStruct *data, FileStruct *file);

static void ClearKelvinResults(CCEtoODBDoc *doc, FileStruct *file)
{
   // Clear the attribute TEST_NET_PROBES, which is a result from Kelvin analysis.

   if (file != NULL)
   {
      WORD testNetProbesKW = doc->IsKeyWord(ATT_TEST_NET_PROBES, 0);
      
      for (POSITION netPos = file->getHeadNetPosition();netPos != NULL;)
      {
         NetStruct* net = file->getNextNet(netPos);

         if (net != NULL && net->getAttributesRef() != NULL)
         {
            net->getAttributesRef()->deleteAttribute(testNetProbesKW);
         }
      }
   }
}

int doKelvinAnalysis(CCEtoODBDoc *doc, bool useResistors, double resistorValue, bool useCapacitors, double capacitorValue, bool useInductor)
{
   ComponentValues resCompVal(resistorValue, "R");
   // This constructor does not work!  ComponentValues capCompVal(capacitorValue, "uF"); // value comes in as uF
   ComponentValues capCompVal(valueUnitFarad);
   CString capValStr;
   capValStr.Format("%fuF", capacitorValue);
   capCompVal.SetValue(capValStr);

   // Get the currently visible file (one and only one).
   FileStruct *visFile = doc->getFileList().GetOnlyShown(blockTypePcb);
   if (visFile == NULL)
   {
      ErrorMessage("No or multiple visible PCB file(s) found!", "Error", MB_OK);
      return 0;
   }

   WORD devTypeKW = doc->IsKeyWord(ATT_DEVICETYPE, 0);
   WORD valueKW = doc->IsKeyWord(ATT_VALUE, 0);
   if (devTypeKW < 0)
      return 0;

   ClearKelvinResults(doc, visFile);

   int netAffectedCount = 0;

   // Loop through each component
   BlockStruct *fileBlock = visFile->getBlock();
   POSITION dataPos = fileBlock->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = fileBlock->getDataList().GetNext(dataPos);

      if (data->isInsertType(insertTypePcbComponent) && data->getAttributesRef() != NULL)
      {
         Attrib *attrib = NULL;
         if (data->getAttributesRef()->Lookup(devTypeKW, attrib))
         {
            CString devType = get_attvalue_string(doc, attrib);

            if (useResistors && (!devType.CompareNoCase("Resistor") || !devType.CompareNoCase("Resistor_Array")))
            {
               if (data->getAttributesRef()->Lookup(valueKW, attrib))
               {
                  ComponentValues compVal(get_attvalue_string(doc, attrib));
                  if (compVal.IsUnitEqual(resCompVal))
                  {
                     if (compVal.CompareValueTo(resCompVal) <= 0)
                        netAffectedCount += setMultiTestNetProbes(doc, data, visFile);
                  }
               }
            }
            else if (useCapacitors && !devType.CompareNoCase("Capacitor"))
            {
               if (data->getAttributesRef()->Lookup(valueKW, attrib))
               {
                  ComponentValues compVal(get_attvalue_string(doc, attrib));        
                  if (compVal.IsUnitEqual(capCompVal))
                  {
                     if (compVal.CompareValueTo(capCompVal) >= 0)
                        netAffectedCount += setMultiTestNetProbes(doc, data, visFile);
                  }
               }
            }
            else if (useInductor && !devType.CompareNoCase("Inductor"))
            {
               netAffectedCount += setMultiTestNetProbes(doc, data, visFile);
            }
         }
      }
   }

   return netAffectedCount;
}

int setMultiTestNetProbes(CCEtoODBDoc *doc, DataStruct *data, FileStruct *file)
{
   if (data == NULL)
      return 0;

   if (!data->isInsertType(insertTypePcbComponent))
      return 0;

   BlockStruct *compBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
   if (compBlock == NULL)
      return 0;

   CString compName = data->getInsert()->getRefname();
   int netAffectedCount = 0;

   WORD tnpKW = doc->RegisterKeyWord(ATT_TEST_NET_PROBES, 0, VT_INTEGER);

   POSITION pos = compBlock->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = compBlock->getDataList().GetNext(pos);

      if (data->isInsertType(insertTypePin))
      {
         CString pinName = data->getInsert()->getRefname();

         Attrib *attrib = NULL;
         NetStruct *net = NULL;
         CompPinStruct *cp = FindCompPin(file, compName, pinName, &net);

         if (net != NULL)
         {
            if (net->getAttributesRef() != NULL && net->getAttributesRef()->Lookup(tnpKW, attrib))
            {
               if (attrib->getIntValue() < 2)
                  doc->SetUnknownAttrib(&net->getAttributesRef(), ATT_TEST_NET_PROBES, "2", SA_OVERWRITE, &attrib);
            }
            else
            {
               doc->SetUnknownAttrib(&net->getAttributesRef(), ATT_TEST_NET_PROBES, "2", SA_OVERWRITE, &attrib);
            }
         }
         netAffectedCount++;
      }
   }

   return netAffectedCount;
}

/////////////////////////////////////////////////////////////////////////////
// CDFTTarget
/////////////////////////////////////////////////////////////////////////////
CDFTTarget::CDFTTarget(bool Enable, CString Name)
{
   m_bModified = true;
   m_bEnable = Enable;
   m_sName = Name;
}

CDFTTarget::CDFTTarget(const CDFTTarget &Target)
{
   m_bModified = Target.m_bModified;
   m_bEnable = Target.m_bEnable;
   m_sName = Target.m_sName;
}

CDFTTarget& CDFTTarget::operator=(const CDFTTarget &Target)
{
   if (&Target != this)
   {
      // Copy everything in CDFTTarget to this
      m_bModified = Target.m_bModified;
      m_bEnable = Target.m_bEnable;
      m_sName = Target.m_sName;
   }

   return *this;
}

void CDFTTarget::SetEnabledFlag(bool Enable)
{ 
   m_bModified |= m_bEnable != Enable;
   m_bEnable = Enable;
}

void CDFTTarget::SetName(CString Name)
{ 
   m_bModified |= m_sName != Name;
   m_sName = Name;
}

void CDFTTarget::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Target %s::\n", indent, " ", m_sName);

   indent += 3;
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");
   file.WriteString("%*sm_bEnable = %s\n", indent, " ", m_bEnable?"True":"False");
   indent -= 3;
}



/////////////////////////////////////////////////////////////////////////////
// CDFTTargetPriority
/////////////////////////////////////////////////////////////////////////////
CDFTTargetPriority::~CDFTTargetPriority()
{
   RemoveAll_TopTargets();
   RemoveAll_BotTargets();
}

CDFTTargetPriority& CDFTTargetPriority::operator=(const CDFTTargetPriority &TargetPriority)
{
   if (&TargetPriority != this)
   {
      // Copy everything in targetPriority to this
      RemoveAll_TopTargets();
      for (int index=0; index<TargetPriority.m_targetsTop.GetCount(); index++)
      {
         CDFTTarget *target = new CDFTTarget(*TargetPriority.m_targetsTop[index]);

         m_targetsTop.Add(target);
      }

      RemoveAll_BotTargets();
      for (int index=0; index<TargetPriority.m_targetsBot.GetCount(); index++)
      {
         CDFTTarget *target = new CDFTTarget(*TargetPriority.m_targetsBot[index]);

         m_targetsBot.Add(target);
      }
   }

   return *this;
}

bool CDFTTargetPriority::IsModified() const
{
   if (m_bModified)
      return true;

   for (int index=0; index<m_targetsTop.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsTop[index];

      if (target->IsModified())
         return true;
   }

      for (int index=0; index<m_targetsBot.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsBot[index];

      if (target->IsModified())
         return true;
   }

   return false;
}

CDFTTarget *CDFTTargetPriority::Find_TopTargets(CString Name, int &index) const
{
   for (index=0; index<m_targetsTop.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsTop[index];

      if (target->GetName() == Name)
         return target;
   }

   return NULL;
}

CDFTTarget *CDFTTargetPriority::Find_BotTargets(CString Name, int &index) const
{
   for (index=0; index<m_targetsBot.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsBot[index];

      if (target->GetName() == Name)
         return target;
   }

   return NULL;
}

void CDFTTargetPriority::ResetModified()
{
   for (int index=0; index<m_targetsTop.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsTop[index];
      target->ResetModified();
   }

   for (int index=0; index<m_targetsBot.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsBot[index];
      target->ResetModified();
   }

   m_bModified = false;
}

int CDFTTargetPriority::Add_TopTargets(bool Enable, CString Name)
{
   CDFTTarget *target = new CDFTTarget(Enable, Name);
   m_bModified = true;
   return m_targetsTop.Add(target);
}

CDFTTarget *CDFTTargetPriority::InsertAt_TopTargets(int index, bool Enable, CString Name)
{
   CDFTTarget *target = new CDFTTarget(Enable, Name);
   m_targetsTop.InsertAt(index, target);

   m_bModified = true;
   return target;
}

void CDFTTargetPriority::Swap_TopTargets(int fromIndex, int toIndex)
{
   if (fromIndex  < 0 || fromIndex  >= m_targetsTop.GetCount() ||
       toIndex    < 0 || toIndex    >= m_targetsTop.GetCount())
       return;

   CDFTTarget *fromTarget = m_targetsTop[fromIndex];
   CDFTTarget *toTarget = m_targetsTop[toIndex];

   m_targetsTop.SetAt(toIndex, fromTarget);
   m_targetsTop.SetAt(fromIndex, toTarget);
}

void CDFTTargetPriority::RemoveAt_TopTargets(int index)
{
   CDFTTarget *target = m_targetsTop[index];
   
   m_targetsTop.RemoveAt(index);
   delete target;
   
   m_bModified = true;
}

void CDFTTargetPriority::RemoveAll_TopTargets()
{
   if (m_targetsTop.IsEmpty())
      return;

   for (int index=0; index<m_targetsTop.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsTop[index];
      delete target;
   }
   m_targetsTop.RemoveAll();

   m_bModified = true;
}

int CDFTTargetPriority::Add_BotTargets(bool Enable, CString Name)
{
   CDFTTarget *target = new CDFTTarget(Enable, Name);
   m_bModified = true;
   return m_targetsBot.Add(target);
}

CDFTTarget *CDFTTargetPriority::InsertAt_BotTargets(int index, bool Enable, CString Name)
{
   CDFTTarget *target = new CDFTTarget(Enable, Name);
   m_targetsBot.InsertAt(index, target);

   m_bModified = true;
   return target;
}

void CDFTTargetPriority::Swap_BotTargets(int fromIndex, int toIndex)
{
   if (fromIndex  < 0 || fromIndex  >= m_targetsBot.GetCount() ||
       toIndex    < 0 || toIndex    >= m_targetsBot.GetCount())
       return;

   CDFTTarget *fromTarget = m_targetsBot[fromIndex];
   CDFTTarget *toTarget = m_targetsBot[toIndex];

   m_targetsBot.SetAt(toIndex, fromTarget);
   m_targetsBot.SetAt(fromIndex, toTarget);
}

void CDFTTargetPriority::RemoveAt_BotTargets(int index)
{
   CDFTTarget *target = m_targetsBot[index];
   
   m_targetsBot.RemoveAt(index);
   delete target;
   
   m_bModified = true;
}

void CDFTTargetPriority::RemoveAll_BotTargets()
{
   if (m_targetsBot.IsEmpty())
      return;

   for (int index=0; index<m_targetsBot.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsBot[index];
      delete target;
   }
   m_targetsBot.RemoveAll();

   m_bModified = true;
}

void CDFTTargetPriority::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");

   file.WriteString("%*s::Top Targets::\n", indent, " ");
   indent += 3;
   for (int index=0; index<m_targetsTop.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsTop[index];
      target->DumpToFile(file, indent);
   }
   indent -= 3;

   file.WriteString("%*s::Bottom Targets::\n", indent, " ");
   indent += 3;
   for (int index=0; index<m_targetsBot.GetCount(); index++)
   {
      CDFTTarget *target = m_targetsBot[index];
      target->DumpToFile(file, indent);
   }
   indent -= 3;
}

void CDFTTargetPriority::WriteXML(CWriteFormat& writeFormat/*, CCamCadFileWriteProgress& progress*/)
{
   writeFormat.writef("<TargetPriorities>\n");
   
   writeFormat.pushHeader("  ");
   for (int i=0; i<m_targetsTop.GetCount(); i++)
   {
      CDFTTarget *target = m_targetsTop[i];
      writeFormat.writef("<Target Surface=\"Top\" Priority=\"%d\" Enable=\"%s\" Name=\"%s\"/>\n", i,
         target->GetEnabledFlag()?"True":"False", target->GetName());
   }

   for (int i=0; i<m_targetsBot.GetCount(); i++)
   {
      CDFTTarget *target = m_targetsBot[i];
      writeFormat.writef("<Target Surface=\"Bottom\" Priority=\"%d\" Enable=\"%s\" Name=\"%s\"/>\n", i,
         target->GetEnabledFlag()?"True":"False", target->GetName());
   }
   writeFormat.popHeader();

   writeFormat.writef("</TargetPriorities>\n");
}

int CDFTTargetPriority::LoadXML(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "TargetPriorities")
      return 0;

   RemoveAll_TopTargets();
   RemoveAll_BotTargets();

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString subNodeName;
      if (!subNode->GetName(subNodeName) || subNodeName != "Target")
         continue;

      CString surface, name, subValue;
      bool enabled = false;
      if (!subNode->GetAttrValue("Surface", surface))
         continue;
      if (!subNode->GetAttrValue("Name", name))
         continue;
      if (subNode->GetAttrValue("Enable", subValue))
         enabled = (subValue.CompareNoCase("True") == 0);

      surface.MakeLower();
      if (!surface.Compare("top"))
         Add_TopTargets(enabled, name);
      else if (!surface.Compare("bottom"))
         Add_BotTargets(enabled, name);

      delete subNode;
   }

   return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDFTProbeTemplate
/////////////////////////////////////////////////////////////////////////////
CDFTProbeTemplate::CDFTProbeTemplate()
{
   m_bModified = true;

   m_bUse = false;
   m_sName.Empty();
   m_dDiameter = 0.;
   m_dDrillSize = 0.;
   m_dTextSize = 0.;
   m_iCost = 0;
   m_bUseComponentOutline = false;
   m_dComponentOutlineDistance = 0.;
}

CDFTProbeTemplate::CDFTProbeTemplate(const CDFTProbeTemplate &Probe)
{
   m_bModified = Probe.m_bModified;

   m_bUse = Probe.m_bUse;
   m_sName = Probe.m_sName;
   m_dDiameter = Probe.m_dDiameter;
   m_dDrillSize = Probe.m_dDrillSize;
   m_dTextSize = Probe.m_dTextSize;
   m_iCost = Probe.m_iCost;
   m_bUseComponentOutline = Probe.m_bUseComponentOutline;
   m_dComponentOutlineDistance = Probe.m_dComponentOutlineDistance;
}

CDFTProbeTemplate::CDFTProbeTemplate(bool Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, bool UseComponentOutline, double ComponentOutlineDistance)
{
   m_bModified = true;

   m_bUse = Use;
   m_sName = Name;
   m_dDiameter = Diameter;
   m_dDrillSize = DrillSize;
   m_dTextSize = TextSize;
   m_iCost = Cost;
   m_bUseComponentOutline = UseComponentOutline;
   m_dComponentOutlineDistance = ComponentOutlineDistance;
}

CDFTProbeTemplate& CDFTProbeTemplate::operator=(const CDFTProbeTemplate &Probe)
{
   if (&Probe != this)
   {
      // Copy everything in CDFTTarget to this
      m_bModified = Probe.m_bModified;
      m_bUse = Probe.m_bUse;
      m_sName = Probe.m_sName;
      m_dDiameter = Probe.m_dDiameter;
      m_dDrillSize = Probe.m_dDrillSize;
      m_dTextSize = Probe.m_dTextSize;
      m_iCost = Probe.m_iCost;
      m_bUseComponentOutline = Probe.m_bUseComponentOutline;
      m_dComponentOutlineDistance = Probe.m_dComponentOutlineDistance;
   }

   return *this;
}

void CDFTProbeTemplate::SetUseFlag(bool Use)
{
   m_bModified |= m_bUse != Use;
   m_bUse = Use;
}

void CDFTProbeTemplate::SetName(CString Name)
{
   m_bModified |= m_sName != Name;
   m_sName = Name;
}

void CDFTProbeTemplate::SetDiameter(double Diameter)
{
   m_bModified |= m_dDiameter != Diameter;
   m_dDiameter = Diameter;
}

void CDFTProbeTemplate::SetDrillSize(double DrillSize)
{
   m_bModified |= m_dDrillSize != DrillSize;
   m_dDrillSize = DrillSize;
}

void CDFTProbeTemplate::SetTextSize(double TextSize)
{
   m_bModified |= m_dTextSize != TextSize;
   m_dTextSize = TextSize;
}

void CDFTProbeTemplate::SetCost(int Cost)
{
   m_bModified |= m_iCost != Cost;
   m_iCost = Cost;
}

void CDFTProbeTemplate::SetUseComponentOutline(bool UseComponentOutline)
{
   m_bModified |= m_bUseComponentOutline != UseComponentOutline;
   m_bUseComponentOutline = UseComponentOutline;
}

void CDFTProbeTemplate::SetComponentOutlineDistance(double ComponentOutlineDistance)
{
   m_bModified |= m_dComponentOutlineDistance != ComponentOutlineDistance;
   m_dComponentOutlineDistance = ComponentOutlineDistance;
}

void CDFTProbeTemplate::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Probe %s::\n", indent, " ", m_sName);

   indent += 3;
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");
   file.WriteString("%*sm_bUse = %s\n", indent, " ", m_bUse?"True":"False");
   file.WriteString("%*sm_dDiameter = %0.3f\n", indent, " ", m_dDiameter);
   file.WriteString("%*sm_dDrillSize = %0.3f\n", indent, " ", m_dDrillSize);
   file.WriteString("%*sm_iCost = %d\n", indent, " ", m_iCost);
   file.WriteString("%*sm_bUseComponentOutline = %s\n", indent, " ", m_bUseComponentOutline?"True":"False");
   file.WriteString("%*sm_dComponentOutlineDistance = %0.3f\n", indent, " ", m_dComponentOutlineDistance);
   indent -= 3;
}



/////////////////////////////////////////////////////////////////////////////
// CDFTProbeTemplates
/////////////////////////////////////////////////////////////////////////////
CDFTProbeTemplates::~CDFTProbeTemplates()
{
   RemoveAll_TopProbes();
   RemoveAll_BotProbes();
}

CDFTProbeTemplates& CDFTProbeTemplates::operator=(const CDFTProbeTemplates &Probes)
{
   if (&Probes != this)
   {
      // Copy everything in targetPriority to this
      RemoveAll_TopProbes();
      POSITION pos = Probes.m_probeTemplatesTop.GetHeadPosition();
      while (pos)
      {
         CDFTProbeTemplate *probeTemplate = new CDFTProbeTemplate(*Probes.m_probeTemplatesTop.GetNext(pos));

         m_probeTemplatesTop.AddTail(probeTemplate);
      }

      RemoveAll_BotProbes();
      pos = Probes.m_probeTemplatesBot.GetHeadPosition();
      while (pos)
      {
         CDFTProbeTemplate *probeTemplate = new CDFTProbeTemplate(*Probes.m_probeTemplatesBot.GetNext(pos));

         m_probeTemplatesBot.AddTail(probeTemplate);
      }
   }

   return *this;
}

bool CDFTProbeTemplates::IsModified() const
{
   if (m_bModified)
      return true;

   POSITION pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetNext(pos);

      if (probeTemplate->IsModified())
         return true;
   }

   pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetNext(pos);

      if (probeTemplate->IsModified())
         return true;
   }

   return false;
}

void CDFTProbeTemplates::ResetModified()
{
   POSITION pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetNext(pos);
      probeTemplate->ResetModified();
   }

   pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetNext(pos);
      probeTemplate->ResetModified();
   }

   m_bModified = false;
}

CDFTProbeTemplate *CDFTProbeTemplates::Find_TopProbes(CString Name, POSITION &pos) const
{
   pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetAt(pos);
      if (probeTemplate->GetName() == Name)
         return probeTemplate;

      m_probeTemplatesTop.GetNext(pos);
   }

   return NULL;
}

/******************************************************************************
* CDFTProbeTemplates::Find_Probe
*/
CDFTProbeTemplate *CDFTProbeTemplates::Find_Probe(CString Name, POSITION &pos, ETestSurface surface) const
{
   if (surface == testSurfaceTop)
   {
      return Find_TopProbes(Name, pos);
   }
   else if (surface == testSurfaceBottom)
   {
      return Find_BotProbes(Name, pos);
   }

   return NULL;
}

CDFTProbeTemplate *CDFTProbeTemplates::GetSmallest_UsedTopProbes(POSITION &pos) const
{
   pos = m_probeTemplatesTop.GetTailPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetAt(pos);
      if (probeTemplate->GetUseFlag())
         return probeTemplate;

      m_probeTemplatesTop.GetPrev(pos);
   }

   return NULL;
}

CDFTProbeTemplate *CDFTProbeTemplates::GetLargest_UsedTopProbes(POSITION &pos) const
{
   pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetAt(pos);
      if (probeTemplate->GetUseFlag())
         return probeTemplate;

      m_probeTemplatesTop.GetNext(pos);
   }

   return NULL;
}

POSITION CDFTProbeTemplates::GetTailPosition_UsedTopProbes() const
{
   POSITION pos;

   for (pos = m_probeTemplatesTop.GetTailPosition();pos != NULL;)
   {
      CDFTProbeTemplate* pTemplate = m_probeTemplatesTop.GetAt(pos);

      if (pTemplate->GetUseFlag())
      {
         break;
      }

      m_probeTemplatesTop.GetPrev(pos);
   }

   return pos;
}

CDFTProbeTemplate *CDFTProbeTemplates::GetPrev_UsedTopProbes(POSITION &pos) const
{
   CDFTProbeTemplate* pTemplate = m_probeTemplatesTop.GetPrev(pos);

   while (pos != NULL)
   {
      CDFTProbeTemplate* nextTemplate = m_probeTemplatesTop.GetAt(pos);

      if (nextTemplate->GetUseFlag())
      {
         break;
      }

      m_probeTemplatesTop.GetPrev(pos);
   }

   return pTemplate;
}

CDFTProbeTemplate *CDFTProbeTemplates::Find_BotProbes(CString Name, POSITION &pos) const
{
   pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetAt(pos);
      if (probeTemplate->GetName() == Name)
         return probeTemplate;

      m_probeTemplatesBot.GetNext(pos);
   }

   return NULL;
}

CDFTProbeTemplate *CDFTProbeTemplates::GetSmallest_UsedBotProbes(POSITION &pos) const
{
   pos = m_probeTemplatesBot.GetTailPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetAt(pos);
      if (probeTemplate->GetUseFlag())
         return probeTemplate;

      m_probeTemplatesBot.GetPrev(pos);
   }

   return NULL;
}

CDFTProbeTemplate *CDFTProbeTemplates::GetLargest_UsedBotProbes(POSITION &pos) const
{
   pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetAt(pos);
      if (probeTemplate->GetUseFlag())
         return probeTemplate;

      m_probeTemplatesBot.GetNext(pos);
   }

   return NULL;
}

POSITION CDFTProbeTemplates::GetTailPosition_UsedBotProbes() const
{
   POSITION pos;

   for (pos = m_probeTemplatesBot.GetTailPosition();pos != NULL;)
   {
      CDFTProbeTemplate* pTemplate = m_probeTemplatesBot.GetAt(pos);

      if (pTemplate->GetUseFlag())
      {
         break;
      }

      m_probeTemplatesBot.GetPrev(pos);
   }

   return pos;
}

CDFTProbeTemplate *CDFTProbeTemplates::GetPrev_UsedBotProbes(POSITION &pos) const
{
   CDFTProbeTemplate* pTemplate = m_probeTemplatesBot.GetPrev(pos);

   while (pos != NULL)
   {
      CDFTProbeTemplate* nextTemplate = m_probeTemplatesBot.GetAt(pos);

      if (nextTemplate->GetUseFlag())
      {
         break;
      }

      m_probeTemplatesBot.GetPrev(pos);
   }

   return pTemplate;
}

void CDFTProbeTemplates::Add_TopProbes(bool Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, bool UseComponentOutline, double ComponentOutlineDistance)
{
   // add the probe template from lesser to greater cost

   CDFTProbeTemplate *pNewTemplate = new CDFTProbeTemplate(Use, Name, Diameter, DrillSize, TextSize, Cost, UseComponentOutline, ComponentOutlineDistance);

   POSITION pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_probeTemplatesTop.GetAt(pos);

      if (pTemplate->GetCost() > pNewTemplate->GetCost())
         break;

      m_probeTemplatesTop.GetNext(pos);
   }

   if (pos)
      m_probeTemplatesTop.InsertBefore(pos, pNewTemplate);
   else
      m_probeTemplatesTop.AddTail(pNewTemplate);


   m_bModified = true;
}

void CDFTProbeTemplates::RemoveAt_TopProbes(POSITION pos)
{
   CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetAt(pos);
   m_probeTemplatesTop.RemoveAt(pos);
   delete probeTemplate;
   probeTemplate = NULL;

   m_bModified = true;
}

void CDFTProbeTemplates::RemoveAll_TopProbes()
{
   if (m_probeTemplatesTop.IsEmpty())
      return;

   POSITION pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetNext(pos);
      delete probeTemplate;
      probeTemplate = NULL;
   }
   m_probeTemplatesTop.RemoveAll();

   m_bModified = true;
}

void CDFTProbeTemplates::Add_BotProbes(bool Use, CString Name, double Diameter, double DrillSize, double TextSize, int Cost, bool UseComponentOutline, double ComponentOutlineDistance)
{
   // add the probe template from lesser to greater cost

   CDFTProbeTemplate *pNewTemplate = new CDFTProbeTemplate(Use, Name, Diameter, DrillSize, TextSize, Cost, UseComponentOutline, ComponentOutlineDistance);

   POSITION pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_probeTemplatesBot.GetAt(pos);

      if (pTemplate->GetCost() > pNewTemplate->GetCost())
         break;

      m_probeTemplatesBot.GetNext(pos);
   }

   if (pos)
      m_probeTemplatesBot.InsertBefore(pos, pNewTemplate);
   else
      m_probeTemplatesBot.AddTail(pNewTemplate);

   m_bModified = true;
}

void CDFTProbeTemplates::RemoveAt_BotProbes(POSITION pos)
{
   CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetAt(pos);
   m_probeTemplatesBot.RemoveAt(pos);
   delete probeTemplate;
   probeTemplate = NULL;

   m_bModified = true;
}

void CDFTProbeTemplates::RemoveAll_BotProbes()
{
   if (m_probeTemplatesBot.IsEmpty())
      return;

   POSITION pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetNext(pos);
      delete probeTemplate;
      probeTemplate = NULL;
   }
   m_probeTemplatesBot.RemoveAll();

   m_bModified = true;
}

void CDFTProbeTemplates::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");

   file.WriteString("%*s::Top Probe Templates::\n", indent, " ");
   indent += 3;
   POSITION pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesTop.GetNext(pos);
      probeTemplate->DumpToFile(file, indent);
   }
   indent -= 3;

   file.WriteString("%*s::Bottom Probe Templates::\n", indent, " ");
   indent += 3;
   pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = m_probeTemplatesBot.GetNext(pos);
      probeTemplate->DumpToFile(file, indent);
   }
   indent -= 3;
}

void CDFTProbeTemplates::WriteXML(CWriteFormat& writeFormat/*, CCamCadFileWriteProgress& progress*/)
{
   writeFormat.writef("<ProbeTemplates>\n");

   writeFormat.pushHeader("  ");
   POSITION pos = m_probeTemplatesTop.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_probeTemplatesTop.GetNext(pos);

      writeFormat.writef("<ProbeTemplate Surface=\"Top\" Use=\"%s\" Name=\"%s\" Diameter=\"%0.3f\" DrillSize=\"%0.3f\" TextSize=\"%0.3f\" Cost=\"%d\" UseCompOutline=\"%s\" CompOutlineDistance=\"%0.3f\"/>\n",
         pTemplate->GetUseFlag()?"True":"False", pTemplate->GetName(), pTemplate->GetDiameter(), pTemplate->GetDrillSize(), pTemplate->GetTextSize(), pTemplate->GetCost(),
         pTemplate->GetUseComponentOutline()?"True":"False", pTemplate->GetComponentOutlineDistance());
   }

   pos = m_probeTemplatesBot.GetHeadPosition();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_probeTemplatesBot.GetNext(pos);

      writeFormat.writef("<ProbeTemplate Surface=\"Bottom\" Use=\"%s\" Name=\"%s\" Diameter=\"%0.3f\" DrillSize=\"%0.3f\" TextSize=\"%0.3f\" Cost=\"%d\" UseCompOutline=\"%s\" CompOutlineDistance=\"%0.3f\"/>\n",
         pTemplate->GetUseFlag()?"True":"False", pTemplate->GetName(), pTemplate->GetDiameter(), pTemplate->GetDrillSize(), pTemplate->GetTextSize(), pTemplate->GetCost(),
         pTemplate->GetUseComponentOutline()?"True":"False", pTemplate->GetComponentOutlineDistance());
   }
   writeFormat.popHeader();

   writeFormat.writef("</ProbeTemplates>\n");
}

int CDFTProbeTemplates::LoadXML(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "ProbeTemplates")
      return 0;

   RemoveAll_TopProbes();
   RemoveAll_BotProbes();

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName) || nodeName != "ProbeTemplate")
         continue;

      CString surface, name, subValue;
      bool enabled = false, useCompOutline = false;
      double diameter = 0., drillSize = 0., textSize = 0., outlineDist = 0.;
      int cost = 0;
      if (!subNode->GetAttrValue("Surface", surface))
         continue;
      if (!subNode->GetAttrValue("Name", name))
         continue;
      if (subNode->GetAttrValue("Use", subValue))
         enabled = (subValue.CompareNoCase("True") == 0);
      if (subNode->GetAttrValue("Diameter", subValue))
         diameter = atof(subValue);
      if (subNode->GetAttrValue("DrillSize", subValue))
         drillSize = atof(subValue);
      if (subNode->GetAttrValue("TextSize", subValue))
         textSize = atof(subValue);
      if (subNode->GetAttrValue("Cost", subValue))
         cost = atoi(subValue);
      if (subNode->GetAttrValue("UseCompOutline", subValue))
         useCompOutline = (subValue.CompareNoCase("True") == 0);
      if (subNode->GetAttrValue("CompOutlineDistance", subValue))
         outlineDist = atof(subValue);

      // Backward compatibility
      // Probe Placement used to use the Diameter value for both the probe diameter and the
      // drill size. Case 1825 introduced separate Drill Size. For backward compatibility, if
      // the drill size is not in the XML probe template, then set it to the same as
      // Diameter. This will match the geometry that would have been produced, and may exist,
      // in the CC file.
      if (drillSize == 0.0)
         drillSize = diameter;

      surface.MakeLower();
      if (surface.Compare("top") == 0)
         Add_TopProbes(enabled, name, diameter, drillSize, textSize, cost, useCompOutline, outlineDist);
      else if (surface.Compare("bottom") == 0)
         Add_BotProbes(enabled, name, diameter, drillSize, textSize, cost, useCompOutline, outlineDist);

      delete subNode;
   }

   return 0;
}




/////////////////////////////////////////////////////////////////////////////
// CDFTFeature
/////////////////////////////////////////////////////////////////////////////
CDFTFeature::CDFTFeature()
{
   m_sComponent.Empty();
   m_sPin.Empty();
   m_sDevice.Empty();
}

CDFTFeature::CDFTFeature(const CDFTFeature &Feature)
{
   m_sComponent = Feature.m_sComponent;
   m_sPin = Feature.m_sPin;
   m_sDevice = Feature.m_sDevice;
}

CDFTFeature::CDFTFeature(CString Component, CString Pin, CString Device)
{
   m_sComponent = Component;
   m_sPin = Pin;
   m_sDevice = Device;
}

CDFTFeature& CDFTFeature::operator=(const CDFTFeature &Feature)
{
   if (&Feature != this)
   {
      m_sComponent = Feature.m_sComponent;
      m_sPin = Feature.m_sPin;
      m_sDevice = Feature.m_sDevice;
   }

   return *this;
}

void CDFTFeature::SetComponent(CString Component)
{
   m_bModified |= m_sComponent != Component;
   m_sComponent = Component;
}

void CDFTFeature::SetPin(CString Pin)
{
   m_bModified |= m_sPin != Pin;
   m_sPin = Pin;
}

void CDFTFeature::SetDevice(CString Device)
{
   m_bModified |= m_sDevice != Device;
   m_sDevice = Device;
}

void CDFTFeature::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Feature %s[%s]-%s::\n", indent, " ", m_sComponent, m_sDevice, m_sPin);

   indent += 3;
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");
   indent -= 3;
}



/////////////////////////////////////////////////////////////////////////////
// CDFTPreconditions
/////////////////////////////////////////////////////////////////////////////
CDFTPreconditions::~CDFTPreconditions()
{
   RemoveAll_ForcedFeatures();
   RemoveAll_NoProbedFeatures();
}

CDFTPreconditions& CDFTPreconditions::operator=(const CDFTPreconditions &Preconditions)
{
   if (&Preconditions != this)
   {
      RemoveAll_ForcedFeatures();
      RemoveAll_NoProbedFeatures();

      // Copy everything in targetPriority to this
      POSITION pos = Preconditions.m_forceProbedFeatures.GetHeadPosition();
      while (pos)
      {
         CDFTFeature *feature = new CDFTFeature(*Preconditions.m_forceProbedFeatures.GetNext(pos));

         m_forceProbedFeatures.AddTail(feature);
      }

      pos = Preconditions.m_noProbedFeatures.GetHeadPosition();
      while (pos)
      {
         CDFTFeature *feature = new CDFTFeature(*Preconditions.m_noProbedFeatures.GetNext(pos));

         m_noProbedFeatures.AddTail(feature);
      }
   }

   return *this;
}

bool CDFTPreconditions::IsModified() const
{
   if (m_bModified)
      return true;

   POSITION pos = m_forceProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_forceProbedFeatures.GetNext(pos);

      if (feature->IsModified())
         return true;
   }

   pos = m_noProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_noProbedFeatures.GetNext(pos);

      if (feature->IsModified())
         return true;
   }

   return false;
}

void CDFTPreconditions::ResetModified()
{
   POSITION pos = m_forceProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_forceProbedFeatures.GetNext(pos);
      feature->ResetModified();
   }

   pos = m_noProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_noProbedFeatures.GetNext(pos);
      feature->ResetModified();
   }

   m_bModified = false;
}

CDFTFeature *CDFTPreconditions::Find_ForcedFeatures(CString Component, CString Pin, POSITION &pos)
{
   pos = m_forceProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_forceProbedFeatures.GetAt(pos);
      if (feature->GetComponent() == Component && feature->GetPin() == Pin)
         return feature;

      m_forceProbedFeatures.GetNext(pos);
   }

   return NULL;
}

CDFTFeature *CDFTPreconditions::Find_NoProbedFeatures(CString Component, CString Pin, POSITION &pos)
{
   pos = m_noProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_noProbedFeatures.GetAt(pos);
      if (feature->GetComponent() == Component && feature->GetPin() == Pin)
         return feature;

      m_noProbedFeatures.GetNext(pos);
   }

   return NULL;
}

void CDFTPreconditions::AddHead_ForcedFeatures(CString Component, CString Pin, CString Device)
{
   m_forceProbedFeatures.AddHead(new CDFTFeature(Component, Pin, Device));

   m_bModified = true;
}

void CDFTPreconditions::AddTail_ForcedFeatures(CString Component, CString Pin, CString Device)
{
   m_forceProbedFeatures.AddTail(new CDFTFeature(Component, Pin, Device));

   m_bModified = true;
}

void CDFTPreconditions::RemoveAt_ForcedFeatures(POSITION pos)
{
   CDFTFeature *feature = m_forceProbedFeatures.GetAt(pos);
   m_forceProbedFeatures.RemoveAt(pos);
   delete feature;

   m_bModified = true;
}

void CDFTPreconditions::RemoveAll_ForcedFeatures()
{
   if (m_forceProbedFeatures.IsEmpty())
      return;

   POSITION pos = m_forceProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_forceProbedFeatures.GetNext(pos);
      delete feature;
   }
   m_forceProbedFeatures.RemoveAll();

   m_bModified = true;
}

void CDFTPreconditions::AddHead_NoProbedFeatures(CString Component, CString Pin, CString Device)
{
   m_noProbedFeatures.AddHead(new CDFTFeature(Component, Pin, Device));

   m_bModified = true;
}

void CDFTPreconditions::AddTail_NoProbedFeatures(CString Component, CString Pin, CString Device)
{
   m_noProbedFeatures.AddTail(new CDFTFeature(Component, Pin, Device));

   m_bModified = true;
}

void CDFTPreconditions::RemoveAt_NoProbedFeatures(POSITION pos)
{
   CDFTFeature *feature = m_noProbedFeatures.GetAt(pos);
   m_noProbedFeatures.RemoveAt(pos);
   delete feature;

   m_bModified = true;
}

void CDFTPreconditions::RemoveAll_NoProbedFeatures()
{
   if (m_noProbedFeatures.IsEmpty())
      return;

   POSITION pos = m_noProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_noProbedFeatures.GetNext(pos);
      delete feature;
   }
   m_noProbedFeatures.RemoveAll();

   m_bModified = true;
}

void CDFTPreconditions::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");

   file.WriteString("%*s::Force Probed Features::\n", indent, " ");
   indent += 3;
   POSITION pos = m_forceProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_forceProbedFeatures.GetNext(pos);
      feature->DumpToFile(file, indent);
   }
   indent -= 3;

   file.WriteString("%*s::No Probed Features::\n", indent, " ");
   indent += 3;
   pos = m_noProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_noProbedFeatures.GetNext(pos);
      feature->DumpToFile(file, indent);
   }
   indent -= 3;
}

void CDFTPreconditions::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Preconditions>\n");

   writeFormat.pushHeader("  ");
   POSITION pos = m_forceProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_forceProbedFeatures.GetNext(pos);

      writeFormat.writef("<ForcedProbe Comp=\"%s\" Pin=\"%s\" Device=\"%s\"/>\n", 
         CDcaCamCadFileWriter::convertHtmlMetaCharacters(feature->GetComponent()), 
         CDcaCamCadFileWriter::convertHtmlMetaCharacters(feature->GetPin()), 
         feature->GetDevice());
   }

   pos = m_noProbedFeatures.GetHeadPosition();
   while (pos)
   {
      CDFTFeature *feature = m_noProbedFeatures.GetNext(pos);

      writeFormat.writef("<NoProbe Comp=\"%s\" Pin=\"%s\" Device=\"%s\"/>\n", 
         CDcaCamCadFileWriter::convertHtmlMetaCharacters(feature->GetComponent()), 
         CDcaCamCadFileWriter::convertHtmlMetaCharacters(feature->GetPin()), 
         feature->GetDevice());
   }
   writeFormat.popHeader();

   writeFormat.writef("</Preconditions>\n");
}

int CDFTPreconditions::LoadXML(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "Preconditions")
      return 0;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName) || nodeName != "ForcedProbe" && nodeName != "NoProbe")
         continue;

      CString comp, pin, device;
      if (!subNode->GetAttrValue("Comp", comp))
         continue;
      if (!subNode->GetAttrValue("Pin", pin))
         continue;
      if (!subNode->GetAttrValue("Device", device))
         continue;

      if (!nodeName.Compare("ForcedProbe"))
         AddTail_ForcedFeatures(comp, pin, device);
      else if (!nodeName.Compare("NoProbe"))
         AddTail_NoProbedFeatures(comp, pin, device);

      delete subNode;
   }

   return 0;
}




/////////////////////////////////////////////////////////////////////////////
// CDFTProbeableNet
/////////////////////////////////////////////////////////////////////////////
CDFTProbeableNet::CDFTProbeableNet()
{
   m_bModified = true;

   m_sName.Empty();
   m_iPinCount = 0;
   m_eNetType = probeableNetTypeSignal;
   m_bNoProbe = 0;
   m_dVoltage = 0.0;
   m_dCurrent = 0.0;
   m_iTestResourcesPerNet = 0;
   m_iPowerInjectionResourcesPerNet = 0;
}

CDFTProbeableNet::CDFTProbeableNet(const CDFTProbeableNet &ProbeableNet)
{
   m_bModified = ProbeableNet.m_bModified;

   m_sName = ProbeableNet.m_sName;
   m_iPinCount = ProbeableNet.m_iPinCount;
   m_eNetType = ProbeableNet.m_eNetType;
   m_bNoProbe = ProbeableNet.m_bNoProbe;
   m_dVoltage = ProbeableNet.m_dVoltage;
   m_dCurrent = ProbeableNet.m_dCurrent;
   m_iTestResourcesPerNet = ProbeableNet.m_iTestResourcesPerNet;
   m_iPowerInjectionResourcesPerNet = ProbeableNet.m_iPowerInjectionResourcesPerNet;
}

CDFTProbeableNet::CDFTProbeableNet(CString Name, int PinCount, eProbeableNetType NetType, bool NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet)
{
   m_bModified = true;

   m_sName = Name;
   m_iPinCount = PinCount;
   m_eNetType = NetType;
   m_bNoProbe = NoProbe;
   m_dVoltage = Voltage;
   m_dCurrent = Current;
   m_iTestResourcesPerNet = TestResourcesPerNet;
   m_iPowerInjectionResourcesPerNet = PowerInjectionResourcesPerNet;
}

CDFTProbeableNet& CDFTProbeableNet::operator=(const CDFTProbeableNet &ProbeableNet)
{
   if (&ProbeableNet != this)
   {
      m_sName = ProbeableNet.m_sName;
      m_iPinCount = ProbeableNet.m_iPinCount;
      m_eNetType = ProbeableNet.m_eNetType;
      m_bNoProbe = ProbeableNet.m_bNoProbe;
      m_dVoltage = ProbeableNet.m_dVoltage;
      m_dCurrent = ProbeableNet.m_dCurrent;
      m_iTestResourcesPerNet = ProbeableNet.m_iTestResourcesPerNet;
      m_iPowerInjectionResourcesPerNet = ProbeableNet.m_iPowerInjectionResourcesPerNet;
   }

   return *this;
}

CString CDFTProbeableNet::GetNetTypeName() const
{
   CString retVal;

   switch (m_eNetType)
   {
   case probeableNetTypePower:
      retVal = "Power";
      break;
   case probeableNetTypeGround:
      retVal = "Ground";
      break;
   case probeableNetTypeSignal:
      retVal = "Signal";
      break;
   }

   return retVal;
}

void CDFTProbeableNet::SetName(CString Name)
{
   m_bModified |= m_sName != Name;
   m_sName = Name;
}

void CDFTProbeableNet::SetPinCount(int PinCount)
{
   m_bModified |= m_iPinCount != PinCount;
   m_iPinCount = PinCount;
}

void CDFTProbeableNet::SetNetType(eProbeableNetType NetType)
{
   m_bModified |= m_eNetType != NetType;
   m_eNetType = NetType;
}

void CDFTProbeableNet::SetNoProbe(bool NoProbe)
{
   m_bModified |= m_bNoProbe != NoProbe;
   m_bNoProbe = NoProbe;
}

void CDFTProbeableNet::SetVoltage(double Voltage)
{
   m_bModified |= m_dVoltage != Voltage;
   m_dVoltage = Voltage;
}

void CDFTProbeableNet::SetCurrent(double Current)
{
   m_bModified |= m_dCurrent != Current;
   m_dCurrent = Current;
}

void CDFTProbeableNet::SetTestResourcesPerNet(int TestResourcesPerNet)
{
   m_bModified |= m_iTestResourcesPerNet != TestResourcesPerNet;
   m_iTestResourcesPerNet = TestResourcesPerNet;
}

void CDFTProbeableNet::SetPowerInjectionResourcesPerNet(int PowerInjectionResourcesPerNet)
{
   m_bModified |= m_iPowerInjectionResourcesPerNet != PowerInjectionResourcesPerNet;
   m_iPowerInjectionResourcesPerNet = PowerInjectionResourcesPerNet;
}

void CDFTProbeableNet::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Net %s::\n", indent, " ", m_sName);
   indent += 3;
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");
   file.WriteString("%*sm_sName = %s\n", indent, " ", m_sName);
   file.WriteString("%*sm_iPinCount = %d\n", indent, " ", m_iPinCount);
   file.WriteString("%*sm_eNetType = %s\n", indent, " ", this->GetNetTypeName());
   file.WriteString("%*sm_bNoProbe = %s\n", indent, " ", m_bNoProbe?"True":"False");
   file.WriteString("%*sm_dVoltage = %.2f\n", indent, " ", m_dVoltage);
   file.WriteString("%*sm_dCurrent = %.2f\n", indent, " ", m_dCurrent);
   file.WriteString("%*sm_iTestResourcesPerNet = %d\n", indent, " ", m_iTestResourcesPerNet);
   file.WriteString("%*sm_iPowerInjectionResourcesPerNet = %d\n", indent, " ", m_iPowerInjectionResourcesPerNet);
   indent -= 3;
}

CString CDFTProbeableNet::GetFirstPinRef()
{
   CCEtoODBDoc *doc = (getActiveView() != NULL) ? (CCEtoODBDoc*)getActiveView()->GetDocument() : NULL;

   NetStruct *net = NULL;
   if (doc != NULL)
   {
      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL && net == NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(pos);
         if (file->isShown() && file->getBlockType() == BLOCKTYPE_PCB)  
         {
            net = FindNet(file, m_sName);
         }
      }
   }

   CompPinStruct *compPin = NULL;
   if (net != NULL)
   {
      // Just want first compPin
      POSITION compPinPos = net->getHeadCompPinPosition();
      if (compPinPos) compPin = net->getNextCompPin(compPinPos);
   }

   CString pinRef;
   if (compPin != NULL) pinRef = compPin->getPinRef('-');

   return pinRef;
}

void CDFTProbeableNet::WriteCSV(CFormatStdioFile &csvfile)
{

   CString pinRef;

   csvfile.WriteString("%s", m_sName);
   csvfile.WriteString(",%s", GetFirstPinRef());
   csvfile.WriteString(",%d", m_iTestResourcesPerNet);
   csvfile.WriteString(",%s", GetNetTypeName());

   csvfile.WriteString("\n");
}

void CDFTProbeableNet::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<NetCondition Name=\"%s\" NoProbe=\"%s\" TestResource=\"%d\" PowerInjectionResource=\"%d\"/>\n",
      SwapSpecialCharacters(m_sName), m_bNoProbe?"True":"False", m_iTestResourcesPerNet, m_iPowerInjectionResourcesPerNet);
}



/////////////////////////////////////////////////////////////////////////////
// CDFTNetConditions
/////////////////////////////////////////////////////////////////////////////
CDFTNetConditions::~CDFTNetConditions()
{
   RemoveAll_ProbeableNets();
}

CDFTNetConditions& CDFTNetConditions::operator=(const CDFTNetConditions &NetConditions)
{
   if (&NetConditions != this)
   {
      // Copy everything in targetPriority to this
      RemoveAll_ProbeableNets();
      POSITION pos = NetConditions.m_probeableNets.GetHeadPosition();
      while (pos)
      {
         CDFTProbeableNet *probeableNet = new CDFTProbeableNet(*NetConditions.m_probeableNets.GetNext(pos));

         m_probeableNets.AddTail(probeableNet);
      }
   }

   return *this;
}

bool CDFTNetConditions::IsModified() const
{
   if (m_bModified)
      return true;

   POSITION pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *pNet = m_probeableNets.GetNext(pos);

      if (pNet->IsModified())
         return true;
   }

   return false;
}

CDFTProbeableNet *CDFTNetConditions::Find_ProbeableNet(CString Name, POSITION &pos, bool caseSensitive) const
{
   pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *probeableNet = m_probeableNets.GetAt(pos);

      if ((caseSensitive && !Name.CompareNoCase(probeableNet->GetName())) ||
         (!caseSensitive && !Name.CompareNoCase(probeableNet->GetName())))
         return probeableNet;

      m_probeableNets.GetNext(pos);
   }

   return NULL;
}

void CDFTNetConditions::ResetModified()
{
   POSITION pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *pNet = m_probeableNets.GetNext(pos);
      pNet->ResetModified();
   }

   m_bModified = false;
}

void CDFTNetConditions::Initialize(CCEtoODBDoc *doc, FileStruct *pcbfile, CDFTSolution *dftSolution)
{
   this->RemoveAll_ProbeableNets();

   if (doc != NULL && pcbfile != NULL && dftSolution != NULL)
   {
      CString netName;
      int pinCount = 0, testResource, powerInjResource;
      eProbeableNetType netType = probeableNetTypeSignal;
      double voltage = 0.0, current = 0.0;
      bool noProbe = false;

      WORD netTypeKW = doc->RegisterKeyWord(ATT_NET_TYPE, 0, valueTypeString);
      WORD currentKW = doc->RegisterKeyWord(ATT_CURRENT, 0, valueTypeDouble);
      WORD voltageKW = doc->RegisterKeyWord(ATT_VOLTAGE, 0, valueTypeDouble);
      WORD trRequiredKW = doc->RegisterKeyWord(ATT_TR_REQUIRED, 0, valueTypeInteger);
      WORD pirRequiredKW = doc->RegisterKeyWord(ATT_PIR_REQUIRED, 0, valueTypeInteger);
      WORD noProbeKW = doc->RegisterKeyWord(ATT_NOPROBE, 0, valueTypeString);

      // gather all the nets from the net list
      POSITION pos = pcbfile->getHeadNetPosition();
      while (pos)
      {
         NetStruct *net = pcbfile->getNextNet(pos);
         Attrib *attrib = NULL;

         // get the net name if there is one
         netName = net->getNetName();
         noProbe = false;
         pinCount = net->getCompPinCount();

         if (net->lookUpAttrib(noProbeKW, attrib))
            noProbe = !((CString)get_attvalue_string(doc, attrib)).CompareNoCase("True");

         // If the net is not analyzed, then it should have no probe
         CAANetAccess* netAccess = dftSolution->GetAccessAnalysisSolution()->GetNetAccess(netName);
         if (netAccess == NULL || !netAccess->IsNetAnalyzed())
            noProbe = true;

         testResource = 1;
         if (net->lookUpAttrib(trRequiredKW, attrib))
            testResource = attrib->getIntValue();

         powerInjResource = 0;
         if (net->lookUpAttrib(pirRequiredKW, attrib))
            powerInjResource = attrib->getIntValue();

         netType = probeableNetTypeSignal;
         if (net->lookUpAttrib(netTypeKW, attrib))
         {
            CString val = get_attvalue_string(doc, attrib);
            if (val == "Power")
               netType = probeableNetTypePower;
            else if (val == "Ground")
               netType = probeableNetTypeGround;
         }

         voltage = 0.0;
         if (net->lookUpAttrib(voltageKW, attrib))
            voltage = attrib->getDoubleValue();

         current = 0.0;
         if (net->lookUpAttrib(currentKW, attrib))
            current = attrib->getDoubleValue();

         this->AddTail_ProbeableNets(netName, pinCount, netType, noProbe, voltage, current, testResource, powerInjResource);
      }

   }
}

void CDFTNetConditions::AddHead_ProbeableNets(CString Name, int PinCount, eProbeableNetType NetType, bool NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet)
{
   m_probeableNets.AddHead(new CDFTProbeableNet(Name, PinCount, NetType, NoProbe, Voltage, Current, TestResourcesPerNet, PowerInjectionResourcesPerNet));

   m_bModified = true;
}

void CDFTNetConditions::AddTail_ProbeableNets(CString Name, int PinCount, eProbeableNetType NetType, bool NoProbe, double Voltage, double Current, int TestResourcesPerNet, int PowerInjectionResourcesPerNet)
{
   m_probeableNets.AddTail(new CDFTProbeableNet(Name, PinCount, NetType, NoProbe, Voltage, Current, TestResourcesPerNet, PowerInjectionResourcesPerNet));

   m_bModified = true;
}

void CDFTNetConditions::RemoveAt_ProbeableNets(POSITION pos)
{
   CDFTProbeableNet *net = m_probeableNets.GetAt(pos);
   m_probeableNets.RemoveAt(pos);
   delete net;

   m_bModified = true;
}

void CDFTNetConditions::RemoveAll_ProbeableNets()
{
   if (m_probeableNets.IsEmpty())
      return;

   POSITION pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *net = m_probeableNets.GetNext(pos);
      delete net;
   }
   m_probeableNets.RemoveAll();

   m_bModified = true;
}

void CDFTNetConditions::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*sm_bModified = %s\n", indent, " ", m_bModified?"True":"False");

   file.WriteString("%*s::Probeable Nets::\n", indent, " ");
   indent += 3;
   POSITION pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *pNet = m_probeableNets.GetNext(pos);
      pNet->DumpToFile(file, indent);
   }
   indent -= 3;
}

bool CDFTNetConditions::WriteCSV(CString filename)
{
   CFormatStdioFile file;
   CFileException err;

   if (!file.Open(filename, CFile::modeCreate|CFile::modeWrite, &err))
      return false;
   
   file.WriteString("NetName,Dev-Pin,RequiredCount,NetType\n");

   POSITION pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *pNet = m_probeableNets.GetNext(pos);
      pNet->WriteCSV(file);
   }

   return true;
}

void CDFTNetConditions::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<NetConditions>\n");

   writeFormat.pushHeader("  ");
   POSITION pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *pNet = m_probeableNets.GetNext(pos);

      pNet->WriteXML(writeFormat, progress);
   }
   writeFormat.popHeader();

   writeFormat.writef("</NetConditions>\n");
}

int CDFTNetConditions::LoadXML(CXMLNode *node, FileStruct *file)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "NetConditions")
      return 0;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName) || nodeName != "NetCondition")
         continue;

      CString name, subValue;
      bool noProbe = false;
      int test = 0, pwrInj = 0;
      if (!subNode->GetAttrValue("Name", name))
         continue;
      if (subNode->GetAttrValue("NoProbe", subValue))
         noProbe = (subValue.CompareNoCase("True") == 0);
      if (subNode->GetAttrValue("TestResource", subValue))
         test = atoi(subValue);
      if (subNode->GetAttrValue("PowerInjectionResource", subValue))
         pwrInj = atoi(subValue);

      eProbeableNetType netType = probeableNetTypeSignal;
      double voltage = 0.0, current = 0.0;
      int pinCount = 0;

      NetStruct *net = FindNet(file, name);
      if (net != NULL)
      {
         pinCount = net->getCompPinCount();

         // TODO: get the net type
         netType = probeableNetTypeSignal;

         // TODO: get voltage
         voltage = 0.0;
         
         // TODO: get current
         current = 0.0;
      }

      AddTail_ProbeableNets(name, pinCount, netType, noProbe, voltage, current, test, pwrInj);

      delete subNode;
   }

   return 0;
}

void CDFTNetConditions::UpdateNetsInFile(FileStruct *file, CCEtoODBDoc *doc)
{
   WORD netTypeKW = doc->RegisterKeyWord(ATT_NET_TYPE, 0, valueTypeString);
   WORD currentKW = doc->RegisterKeyWord(ATT_CURRENT, 0, valueTypeDouble);
   WORD voltageKW = doc->RegisterKeyWord(ATT_VOLTAGE, 0, valueTypeDouble);
   WORD trRequiredKW = doc->RegisterKeyWord(ATT_TR_REQUIRED, 0, valueTypeInteger);
   WORD pirRequiredKW = doc->RegisterKeyWord(ATT_PIR_REQUIRED, 0, valueTypeInteger);
   WORD noProbeKW = doc->RegisterKeyWord(ATT_NOPROBE, 0, valueTypeString);

   POSITION pos = m_probeableNets.GetHeadPosition();
   while (pos)
   {
      CDFTProbeableNet *pNet = m_probeableNets.GetNext(pos);
      
      NetStruct *net = FindNet(file, pNet->GetName());

      if (net == NULL)
         continue;

      CString netType = "Signal";

      if (pNet->GetNetType() == probeableNetTypePower)
         netType = "Power";
      else if (pNet->GetNetType() == probeableNetTypeGround)
         netType = "Ground";

      net->setAttrib(doc->getCamCadData(), netTypeKW, valueTypeString, netType.GetBuffer(0), attributeUpdateOverwrite, NULL);

      int trRequired = pNet->GetTestResourcesPerNet();
      net->setAttrib(doc->getCamCadData(), trRequiredKW, valueTypeInteger, &trRequired, attributeUpdateOverwrite, NULL);

      int pirRequired = pNet->GetPowerInjectionResourcesPerNet();
      net->setAttrib(doc->getCamCadData(), pirRequiredKW, valueTypeInteger, &pirRequired, attributeUpdateOverwrite, NULL);

      CString noProbe = pNet->GetNoProbe()?"True":"False";
      net->setAttrib(doc->getCamCadData(), noProbeKW, valueTypeString, noProbe.GetBuffer(0), attributeUpdateOverwrite, NULL);

      double current = pNet->GetCurrent();
      net->setAttrib(doc->getCamCadData(), currentKW, valueTypeDouble, &current, attributeUpdateOverwrite, NULL);

      double voltage = pNet->GetVoltage();
      net->setAttrib(doc->getCamCadData(), voltageKW, valueTypeDouble, &voltage, attributeUpdateOverwrite, NULL);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CTestPlan
/////////////////////////////////////////////////////////////////////////////
int CTestPlan::m_nextId = 0;

CTestPlan::CTestPlan(PageUnitsTag pageUnits)
: m_pageUnits(pageUnits)
, m_heightAnalysisTop(pageUnits)
, m_heightAnalysisBot(pageUnits)
, m_constraintsTop(pageUnits)
, m_constraintsBot(pageUnits)
{
   m_id = m_nextId++;
   SetDefaults(pageUnits);
}

CTestPlan::CTestPlan(const CTestPlan &testPlan)
: m_pageUnits(testPlan.m_pageUnits)
, m_heightAnalysisTop(testPlan.m_pageUnits)
, m_heightAnalysisBot(testPlan.m_pageUnits)
, m_constraintsTop(testPlan.m_pageUnits)
, m_constraintsBot(testPlan.m_pageUnits)
{
   m_id = m_nextId++;
   *this = testPlan;
}

CTestPlan::CTestPlan(CString fileName,PageUnitsTag pageUnits)
: m_pageUnits(pageUnits)
, m_heightAnalysisTop(pageUnits)
, m_heightAnalysisBot(pageUnits)
, m_constraintsTop(pageUnits)
, m_constraintsBot(pageUnits)
{
   m_id = m_nextId++;
   SetDefaults(pageUnits);

   LoadFile(fileName, pageUnits);
}

CTestPlan& CTestPlan::operator=(const CTestPlan &testPlan)
{
   if (&testPlan != this)
   {
      m_bEnableRectangularOutline   = testPlan.m_bEnableRectangularOutline;
      m_bEnableOverwriteOutline     = testPlan.m_bEnableOverwriteOutline;
      m_bIncludeSinglePinComp       = testPlan.m_bIncludeSinglePinComp;

      m_bEnableSolermaskAnalysis    = testPlan.m_bEnableSolermaskAnalysis;
      m_iOutlineToUse               = testPlan.m_iOutlineToUse;
      m_iOutlinePriority            = testPlan.m_iOutlinePriority;
      m_bIgnore1PinCompOutline      = testPlan.m_bIgnore1PinCompOutline;
      m_bIgnoreUnloadedCompOutline  = testPlan.m_bIgnoreUnloadedCompOutline;

      m_heightAnalysisTop           = testPlan.m_heightAnalysisTop;
      m_heightAnalysisBot           = testPlan.m_heightAnalysisBot;
      m_constraintsTop              = testPlan.m_constraintsTop;  
      m_constraintsBot              = testPlan.m_constraintsBot;
      m_targetTypesTop              = testPlan.m_targetTypesTop;
      m_targetTypesBot              = testPlan.m_targetTypesBot;
      m_iSurface                    = testPlan.m_iSurface;
      m_iTesterType                 = testPlan.m_iTesterType;
      m_bIncludeMulPinNet           = testPlan.m_bIncludeMulPinNet;
      m_bIncludeSngPinNet           = testPlan.m_bIncludeSngPinNet;
      m_bIncludeNCNet               = testPlan.m_bIncludeNCNet;
      m_bCreateNCNet                = testPlan.m_bCreateNCNet;
      m_bRetainedExistingProbe      = testPlan.m_bRetainedExistingProbe;
      m_bMultipleReasonCodes        = testPlan.m_bMultipleReasonCodes;

      CString pinDirections = testPlan.m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getDirectionList().getEncodedString();

      m_accessOffsetOptions         = testPlan.m_accessOffsetOptions;
      m_bAAWriteIgnoredSurface      = testPlan.m_bAAWriteIgnoredSurface;

      
      m_bPPWriteIgnoredSurface      = testPlan.m_bPPWriteIgnoredSurface;
      m_targetPriority              = testPlan.m_targetPriority;
      m_probeTemplates              = testPlan.m_probeTemplates;
      m_bNeverMirrorRefname         = testPlan.m_bNeverMirrorRefname;

      m_bUseResistor                = testPlan.m_bUseResistor;
      m_dResistorValue              = testPlan.m_dResistorValue;
      m_bUseCapacitor               = testPlan.m_bUseCapacitor;
      m_dCapacitorValue             = testPlan.m_dCapacitorValue;
      m_bUseInductor                = testPlan.m_bUseInductor;
      m_iPowerInjectionUsage        = testPlan.m_iPowerInjectionUsage;
      m_iPowerInjectionValue        = testPlan.m_iPowerInjectionValue;


      m_preconditions               = testPlan.m_preconditions;
      m_netConditions               = testPlan.m_netConditions;

      m_iProbeStartNumber           = testPlan.m_iProbeStartNumber;
      m_bAllowDoubleWiring          = testPlan.m_bAllowDoubleWiring;
      m_eProbeSide                  = testPlan.m_eProbeSide;
      m_iProbeSidePreference        = testPlan.m_iProbeSidePreference;
      m_bUseProbeOffset             = testPlan.m_bUseProbeOffset;
      m_bPlaceOnAllAccessible       = testPlan.m_bPlaceOnAllAccessible;
      m_bOffsetSMDTop               = testPlan.m_bOffsetSMDTop;
      m_bOffsetPTHTop               = testPlan.m_bOffsetPTHTop;
      m_bOffsetVIATPTop             = testPlan.m_bOffsetVIATPTop;
      m_bOffsetSMDBot               = testPlan.m_bOffsetSMDBot;
      m_bOffsetPTHBot               = testPlan.m_bOffsetPTHBot;
      m_bOffsetVIATPBot             = testPlan.m_bOffsetVIATPBot;
      m_bUseExistingProbes          = testPlan.m_bUseExistingProbes;
      m_eProbeUse                   = testPlan.m_eProbeUse;
      m_bRetainProbeNames           = testPlan.m_bRetainProbeNames;
      m_bRetainProbePositions       = testPlan.m_bRetainProbePositions;
      m_bRetainProbeSizes           = testPlan.m_bRetainProbeSizes;
      m_bNetConditionCaseSensitive  = testPlan.m_bNetConditionCaseSensitive;
   }

   return *this;
}

int CTestPlan::getId() const
{
   return m_id;
}

bool CTestPlan::IsModified()
{
   m_bModified |= m_bPackageOutlineModified;
   m_bModified |= m_heightAnalysisTop.IsModified();
   m_bModified |= m_heightAnalysisBot.IsModified();
   m_bModified |= m_constraintsTop.IsModified();
   m_bModified |= m_constraintsBot.IsModified();
   m_bModified |= m_targetTypesTop.IsModified();
   m_bModified |= m_targetTypesBot.IsModified();
   m_bModified |= m_targetPriority.IsModified();
   m_bModified |= m_probeTemplates.IsModified();
   m_bModified |= m_preconditions.IsModified();
   m_bModified |= m_netConditions.IsModified();

   return m_bModified;
}

void CTestPlan::ResetModified()
{
   m_bModified = false;
   m_bPackageOutlineModified = false;
   m_heightAnalysisTop.ResetModified();
   m_heightAnalysisBot.ResetModified();
   m_constraintsTop.ResetModified();
   m_constraintsBot.ResetModified();
   m_targetTypesTop.ResetModified();
   m_targetTypesBot.ResetModified();
   m_targetPriority.ResetModified();
   m_probeTemplates.ResetModified();
   m_preconditions.ResetModified();
   m_netConditions.ResetModified();
}

void CTestPlan::SetDefaults(int pageUnit)
{
   m_bModified                   = false;
   m_bPackageOutlineModified     = false;

   // Access Analysis private members
   m_bEnableRectangularOutline   = false;
   m_bEnableOverwriteOutline     = true;
   m_bIncludeSinglePinComp       = true;

   m_bEnableSolermaskAnalysis    = true;
   m_iOutlineToUse               = DFT_OUTLINE_BOTH;
   m_iOutlinePriority            = DFT_OUTLINE_REALPART_PRIORITY;
   m_bIgnore1PinCompOutline      = true;
   m_bIgnoreUnloadedCompOutline  = true;
   
   m_iSurface                    = testSurfaceBottom;
   m_iTesterType                 = DFT_TESTER_FIXTURE;
   m_bIncludeMulPinNet           = true;
   m_bIncludeSngPinNet           = true;
   m_bIncludeNCNet               = false;
   m_bCreateNCNet                = true;
   m_bRetainedExistingProbe      = false;
   m_bMultipleReasonCodes        = false;

   m_accessOffsetOptions.initialize();
   m_bAAWriteIgnoredSurface      = true;

   m_heightAnalysisTop.SetDefault(intToPageUnitsTag(pageUnit));
   m_heightAnalysisBot.SetDefault(intToPageUnitsTag(pageUnit));
   m_constraintsTop.SetDefault(pageUnit);
   m_constraintsBot.SetDefault(pageUnit);
   m_targetTypesTop.SetDefault();
   m_targetTypesBot.SetDefault();
   SetPPDefaults();
}

void CTestPlan::SetPPDefaults()
{
   // Probe Placement private members
   m_targetPriority.RemoveAll_TopTargets();
   m_targetPriority.RemoveAll_BotTargets();
   m_probeTemplates.RemoveAll_TopProbes();
   m_probeTemplates.RemoveAll_BotProbes();

   m_bNeverMirrorRefname         = false;

   m_bPPWriteIgnoredSurface      = true;

   m_bUseResistor                = false;
   m_dResistorValue              = 0.0;
   m_bUseCapacitor               = false;
   m_dCapacitorValue             = 0.0;
   m_bUseInductor                = false;
   m_iPowerInjectionUsage        = 0;        // No power injection
   m_iPowerInjectionValue        = 0;

   m_preconditions.RemoveAll_ForcedFeatures();
   m_preconditions.RemoveAll_NoProbedFeatures();
   m_netConditions.RemoveAll_ProbeableNets();

   m_iProbeStartNumber           = 1;
   m_bAllowDoubleWiring          = false;
   m_eProbeSide                  = testSurfaceTop;
   m_iProbeSidePreference        = testSurfaceBottom  ;
   m_bUseProbeOffset             = false;
   m_bPlaceOnAllAccessible       = false;
   m_bOffsetSMDTop               = false;
   m_bOffsetPTHTop               = false;
   m_bOffsetVIATPTop             = false;
   m_bOffsetSMDBot               = false;
   m_bOffsetPTHBot               = false;
   m_bOffsetVIATPBot             = false;
   m_bUseExistingProbes          = false;
   m_eProbeUse                   = probeUsageUseExisting;
   m_bRetainProbeNames           = false;
   m_bRetainProbePositions       = false;
   m_bRetainProbeSizes           = false;
   m_bNetConditionCaseSensitive  = false;

   //////////////////////////////////////////////////
   // Add the default test probe templates
   m_probeTemplates.RemoveAll_TopProbes();
   m_probeTemplates.Add_TopProbes(true, "100", 90, 67, 0.0,  50, false, 30);
   m_probeTemplates.Add_TopProbes(true,  "75", 70, 55, 0.0, 100, false, 20);
   m_probeTemplates.Add_TopProbes(true,  "50", 49, 37, 0.0, 200, false, 10);

   m_probeTemplates.RemoveAll_BotProbes();
   m_probeTemplates.Add_BotProbes(true, "100", 90, 67, 0.0,  50, false, 30);
   m_probeTemplates.Add_BotProbes(true,  "75", 70, 55, 0.0, 100, false, 20);
   m_probeTemplates.Add_BotProbes(true,  "50", 49, 37, 0.0, 200, false, 10);
}

void CTestPlan::SetOutlineToUse(int value)
{
   m_bModified |= m_iOutlineToUse != value;
   m_iOutlineToUse = value;
}

void CTestPlan::SetOutlinePriority(int value)
{
   m_bModified |= m_iOutlinePriority != value;
   m_bPackageOutlineModified |= m_iOutlinePriority != value;
   m_iOutlinePriority = value;
}

void  CTestPlan::SetPackageOutlineModified()
{
   m_bPackageOutlineModified = true;
}

void CTestPlan::SetEnableSoldermaskAnalysis(bool enable)
{
   m_bModified |= m_bEnableSolermaskAnalysis != enable;
   m_bEnableSolermaskAnalysis = enable;
}

void CTestPlan::SetIgnore1PinCompOutline(bool enable)
{
   m_bModified |= m_bIgnore1PinCompOutline != enable;
   m_bPackageOutlineModified |= m_bIgnore1PinCompOutline != enable;
   m_bIgnore1PinCompOutline = enable;
}

void CTestPlan::SetIgnoreUnloadedCompOutline(bool enable)
{
   m_bModified |= m_bIgnoreUnloadedCompOutline != enable;
   m_bPackageOutlineModified |= m_bIgnoreUnloadedCompOutline != enable;
   m_bIgnoreUnloadedCompOutline = enable;
}

void CTestPlan::SetTopHeightAnalysis(CHeightAnalysis heightAnalysis)
{
   m_heightAnalysisTop = heightAnalysis;
   m_bModified |= heightAnalysis.IsModified();
}

void CTestPlan::SetBotHeightAnalysis(CHeightAnalysis heightAnalysis)
{
   m_heightAnalysisBot = heightAnalysis;
   m_bModified |= heightAnalysis.IsModified();
}

void CTestPlan::SetTopConstraints(CPhysicalConst constraints)
{
   m_constraintsTop = constraints;
   m_bModified |= constraints.IsModified();
}

void CTestPlan::SetBotConstraints(CPhysicalConst constraints)
{
   m_constraintsBot = constraints;
   m_bModified |= constraints.IsModified();
}

void CTestPlan::SetTopTargetTypes(CTargetType targetType)
{
   m_targetTypesTop = targetType;
   m_bModified |= targetType.IsModified();
}

void CTestPlan::SetBotTargetTypes(CTargetType targetType)
{
   m_targetTypesBot = targetType;
   m_bModified |= targetType.IsModified();
}

void CTestPlan::SetSurface(int value)
{
   m_bModified |= m_iSurface != value;
   m_iSurface = value;
}

void CTestPlan::SetTesterType(int value)
{
   m_bModified |= m_iTesterType != value;
   m_iTesterType = value;
}

void CTestPlan::SetIncludeMulPinNet(bool enable)
{
   m_bModified |= m_bIncludeMulPinNet != enable;
   m_bIncludeMulPinNet = enable;
}

void CTestPlan::SetIncludeSngPinNet(bool enable)
{
   m_bModified |= m_bIncludeSngPinNet != enable;
   m_bIncludeSngPinNet = enable;
}

void CTestPlan::SetIncludeNCNet(bool enable)
{
   m_bModified |= m_bIncludeNCNet != enable;
   m_bIncludeNCNet = enable;
}

void CTestPlan::SetCreateNCNet(bool enable)
{
   m_bModified |= m_bCreateNCNet != enable;
   m_bCreateNCNet = enable;
}

void CTestPlan::SetRetainedExistingProbe(bool enable)
{
   m_bModified |= m_bRetainedExistingProbe != enable;
   m_bRetainedExistingProbe = enable;
}

void CTestPlan::SetMultipleReasonCodes(bool enable)
{
   m_bModified |= m_bMultipleReasonCodes != enable;
   m_bMultipleReasonCodes = enable;
}

void CTestPlan::SetAAWriteIgnoreSurface(bool enable)                          
{
   m_bModified |= m_bAAWriteIgnoredSurface != enable;
   m_bAAWriteIgnoredSurface = enable;
}

void CTestPlan::SetPPWriteIgnoreSurface(bool enable)                          
{
   m_bModified |= m_bPPWriteIgnoredSurface != enable;
   m_bPPWriteIgnoredSurface = enable;
}

void CTestPlan::SetUseResistor(bool UseResistor)
{
   m_bModified |= m_bUseResistor != UseResistor;
   m_bUseResistor = UseResistor;
}

void CTestPlan::SetResistorValue(double ResistorValue)
{
   m_bModified |= (m_dResistorValue - ResistorValue < SMALLNUMBER);
   m_dResistorValue = ResistorValue;
}

void CTestPlan::SetUseCapacitor(bool UseCapacitor)
{
   m_bModified |= m_bUseCapacitor != UseCapacitor;
   m_bUseCapacitor = UseCapacitor;
}

void CTestPlan::SetCapacitorValue(double CapacitorValue)
{
   m_bModified |= (m_dCapacitorValue - CapacitorValue < SMALLNUMBER);
   m_dCapacitorValue = CapacitorValue;
}

void CTestPlan::SetUseInductor(bool UseInductor)
{
   m_bModified |= m_bUseInductor != UseInductor;
   m_bUseInductor = UseInductor;
}

void CTestPlan::SetPowerInjectionUsage(int PowerInjectionUsage)
{
   m_bModified |= m_iPowerInjectionUsage != PowerInjectionUsage;
   m_iPowerInjectionUsage = PowerInjectionUsage;
}

void CTestPlan::SetPowerInjectionValue(int PowerInjectionValue)
{
   m_bModified |= m_iPowerInjectionValue != PowerInjectionValue;
   m_iPowerInjectionValue = PowerInjectionValue;
}

void CTestPlan::SetProbeStartNumber(int ProbeStartNumber)
{
   m_bModified |= m_iProbeStartNumber != ProbeStartNumber;
   m_iProbeStartNumber = ProbeStartNumber;
}

void CTestPlan::SetAllowDoubleWiring(bool AllowDoubleWiring)
{
   m_bModified |= m_bAllowDoubleWiring != AllowDoubleWiring;
   m_bAllowDoubleWiring = AllowDoubleWiring;
}

void CTestPlan::SetProbeSide(ETestSurface ProbeSide)
{
   m_bModified |= m_eProbeSide != ProbeSide;
   m_eProbeSide = ProbeSide;
}

void CTestPlan::SetProbeSidePreference(ETestSurface ProbeSidePreference)
{
   if (ProbeSidePreference != testSurfaceTop && ProbeSidePreference != testSurfaceBottom)
      return;

   m_bModified |= m_iProbeSidePreference != ProbeSidePreference;
   m_iProbeSidePreference = ProbeSidePreference;
}

void CTestPlan::SetUseProbeOffset(bool UseProbeOffset)
{
   m_bModified |= m_bUseProbeOffset != UseProbeOffset;
   m_bUseProbeOffset = UseProbeOffset;
}

void CTestPlan::SetPlaceOnAllAccessible(bool PlaceOnAllAccessible)
{
   m_bModified |= m_bPlaceOnAllAccessible != PlaceOnAllAccessible;
   m_bPlaceOnAllAccessible = PlaceOnAllAccessible;
}

void CTestPlan::SetOffsetSMDTop(bool OffsetSMDTop)
{
   m_bModified |= m_bOffsetSMDTop != OffsetSMDTop;
   m_bOffsetSMDTop = OffsetSMDTop;
}

void CTestPlan::SetOffsetPTHTop(bool OffsetPTHTop)
{
   m_bModified |= m_bOffsetPTHTop != OffsetPTHTop;
   m_bOffsetPTHTop = OffsetPTHTop;
}

void CTestPlan::SetOffsetVIATPTop(bool OffsetVIATPTop)
{
   m_bModified |= m_bOffsetVIATPTop != OffsetVIATPTop;
   m_bOffsetVIATPTop = OffsetVIATPTop;
}

void CTestPlan::SetOffsetSMDBot(bool OffsetSMDBot)
{
   m_bModified |= m_bOffsetSMDBot != OffsetSMDBot;
   m_bOffsetSMDBot = OffsetSMDBot;
}

void CTestPlan::SetOffsetPTHBot(bool OffsetPTHBot)
{
   m_bModified |= m_bOffsetPTHBot != OffsetPTHBot;
   m_bOffsetPTHBot = OffsetPTHBot;
}

void CTestPlan::SetOffsetVIATPBot(bool OffsetVIATPBot)
{
   m_bModified |= m_bOffsetVIATPBot != OffsetVIATPBot;
   m_bOffsetVIATPBot = OffsetVIATPBot;
}

void CTestPlan::SetPlaceOnAllAccess(bool PlaceOnAllAccess)
{
   m_bModified |= m_bPlaceOnAllAccess != PlaceOnAllAccess;
   m_bPlaceOnAllAccess = PlaceOnAllAccess;
}

void CTestPlan::SetUseExistingProbes(bool UseExistingProbes)
{
   m_bModified |= m_bUseExistingProbes != UseExistingProbes;
   m_bUseExistingProbes = UseExistingProbes;
}

void CTestPlan::SetProbeUse(EProbeUsage ProbeUse)
{
   m_bModified |= m_eProbeUse != ProbeUse;
   m_eProbeUse = ProbeUse;
}

void CTestPlan::SetRetainProbeNames(bool RetainProbeNames)
{
   m_bModified |= m_bRetainProbeNames != RetainProbeNames;
   m_bRetainProbeNames = RetainProbeNames;
}

void CTestPlan::SetRetainProbePositions(bool RetainProbePositions)
{
   m_bModified |= m_bRetainProbePositions != RetainProbePositions;
   m_bRetainProbePositions = RetainProbePositions;
}

void CTestPlan::SetRetainProbeSizes(bool RetainProbeSizes)
{
   m_bModified |= m_bRetainProbeSizes != RetainProbeSizes;
   m_bRetainProbeSizes = RetainProbeSizes;
}

void CTestPlan::SetNetConditionCaseSensitive(bool caseSensitive)
{
   m_bModified |= m_bNetConditionCaseSensitive != caseSensitive;
   m_bNetConditionCaseSensitive = caseSensitive;
}

void CTestPlan::DumpToFile(CFormatStdioFile &file, int indent)
{
   CString tempBuf;

   file.WriteString("%*s::Test Plan::\n", indent, " ");
   {
      indent += 3;
      file.WriteString("%*s::Package Outline::\n", indent, " ");
      {
         indent += 3;
         file.WriteString("%*sm_bPackageOutlineModified = %s\n", indent, " ", m_bPackageOutlineModified?"True":"False");
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Height Analysis::\n", indent, " ");
      {
         indent += 3;
         file.WriteString("%*s::Top Height Analysis::\n", indent, " ");
         {
            indent += 3;
            m_heightAnalysisTop.DumpToFile(file, indent);
            indent -= 3;
         }
         file.WriteString("%*s::Bottom Height Analysis::\n", indent, " ");
         {
            indent += 3;
            m_heightAnalysisBot.DumpToFile(file, indent);
            indent -= 3;
         }
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Physical Constraints::\n", indent, " ");
      {
         indent += 3;
         file.WriteString("%*sm_bEnableSolermaskAnalysis = %s\n", indent, " ", m_bEnableSolermaskAnalysis?"True":"False");
         switch (m_iOutlineToUse)
         {
         case DFT_OUTLINE_NONE:
            tempBuf = "No Outline";
            break;
         case DFT_OUTLINE_REALPART:
            tempBuf = "Real Part Outline";
            break;
         case DFT_OUTLINE_DFT:
            tempBuf = "DFT outline";
            break;
         case DFT_OUTLINE_BOTH:
            tempBuf = "Real Part & DFT Outline";
            break;
         }
         file.WriteString("%*sm_iOutlineToUse = %s\n", indent, " ", tempBuf);
         file.WriteString("%*sm_iOutlinePriority = %s\n", indent, " ", (m_iOutlinePriority==DFT_OUTLINE_REALPART_PRIORITY)?"Real Part":"DFT Outline");
         file.WriteString("%*s::Top Constraints::\n", indent, " ");
         {
            indent += 3;
            m_constraintsTop.DumpToFile(file, indent);
            indent -= 3;
         }
         file.WriteString("%*s::Bottom Constraints::\n", indent, " ");
         {
            indent += 3;
            m_constraintsBot.DumpToFile(file, indent);
            indent -= 3;
         }
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Target Types::\n", indent, " ");
      {
         indent += 3;
         file.WriteString("%*s::Top Target Types::\n", indent, " ");
         {
            indent += 3;
            m_targetTypesTop.DumpToFile(file, indent);
            indent -= 3;
         }
         file.WriteString("%*s::Bottom Target Types::\n", indent, " ");
         {
            indent += 3;
            m_targetTypesBot.DumpToFile(file, indent);
            indent -= 3;
         }
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Options::\n", indent, " ");
      {
         indent += 3;
         switch (m_iSurface)
         {
         case testSurfaceTop:
            tempBuf = "Top";
            break;
         case testSurfaceBottom:
            tempBuf = "Bottom";
            break;
         case testSurfaceBoth:
            tempBuf = "Top & Bottom";
            break;
         }
         file.WriteString("%*sm_iSurface = %s\n", indent, " ", tempBuf);
         file.WriteString("%*sm_iTesterType = %s\n", indent, " ", (m_iTesterType==DFT_TESTER_FIXTURE)?"Fixture":"Fixtureless");
         file.WriteString("%*sm_bIncludeMulPinNet = %s\n", indent, " ", m_bIncludeMulPinNet?"True":"False");
         file.WriteString("%*sm_bIncludeSngPinNet = %s\n", indent, " ", m_bIncludeSngPinNet?"True":"False");
         file.WriteString("%*sm_bIncludeNCNet = %s\n", indent, " ", m_bIncludeNCNet?"True":"False");
         file.WriteString("%*sm_bRetainedExistingProbe = %s\n", indent, " ", m_bRetainedExistingProbe?"True":"False");
         file.WriteString("%*sm_bMultipleReasonCodes = %s\n", indent, " ", m_bMultipleReasonCodes?"True":"False");
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Target Priority::\n", indent, " ");
      {
         indent += 3;
         m_targetPriority.DumpToFile(file, indent);
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Probes Templates::\n", indent, " ");
      {
         indent += 3;
         m_probeTemplates.DumpToFile(file, indent);
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*sm_bNeverMirrorRefname = %s\n", indent, " ", m_bNeverMirrorRefname?"True":"False");
         

      file.WriteString("%*s::Kelvin & Power Injection::\n", indent, " ");
      {
         indent += 3;
         file.WriteString("%*sm_bUseResistor = %s\n", indent, " ", m_bUseResistor?"True":"False");
         file.WriteString("%*sm_dResistorValue = %.2f\n", indent, " ", m_dResistorValue);
         file.WriteString("%*sm_bUseCapacitor = %s\n", indent, " ", m_bUseCapacitor?"True":"False");
         file.WriteString("%*sm_dCapacitorValue = %.2f\n", indent, " ", m_dCapacitorValue);
         file.WriteString("%*sm_bUseInductor = %s\n", indent, " ", m_bUseInductor?"True":"False");
         switch (m_iPowerInjectionUsage)
         {
         case 0:
            tempBuf = "No power injection";
            break;
         case 1:
            tempBuf = "Number of probes per power rail";
            break;
         case 2:
            tempBuf = "Number of connections per probe for each power rail";
            break;
         case 3:
            tempBuf = "Number of nets per probe for each per power rail";
            break;
         }
         file.WriteString("%*sm_iPowerInjectionUsage = %s\n", indent, " ", tempBuf);
         file.WriteString("%*sm_iPowerInjectionValue = %d\n", indent, " ", m_iPowerInjectionValue);
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Preconditions::\n", indent, " ");
      {
         indent += 3;
         m_preconditions.DumpToFile(file, indent);
         indent -= 3;
      }
      file.WriteString("\n", indent, " ");

      file.WriteString("%*s::Net Conditions::\n", indent, " ");
      {
         indent += 3;
         m_netConditions.DumpToFile(file, indent);
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");

      file.WriteString("%*s::Options::\n", indent, " ");
      {
         indent += 3;
         file.WriteString("%*sm_iProbeStartNumber = %d\n", indent, " ", m_iProbeStartNumber);
         file.WriteString("%*sm_iSurface = %s\n", indent, " ", tempBuf);
         file.WriteString("%*sm_bAllowDoubleWiring = %s\n", indent, " ", m_bAllowDoubleWiring?"True":"False");
         switch (m_eProbeSide)
         {
         case testSurfaceTop:
            tempBuf = "Top";
            break;
         case testSurfaceBottom:
            tempBuf = "Bottom";
            break;
         case testSurfaceBoth:
            tempBuf = "Top & Bottom";
            break;
         }
         file.WriteString("%*sm_eProbeSide = %s\n", indent, " ", tempBuf);
         file.WriteString("%*sm_iProbeSidePreference = %s\n", indent, " ", (m_iProbeSidePreference==testSurfaceTop)?"Top":"Bottom");
         file.WriteString("%*sm_bUseProbeOffset = %s\n", indent, " ", m_bUseProbeOffset?"True":"False");
         file.WriteString("%*sm_bPlaceOnAllAccessible = %s\n", indent, " ", m_bPlaceOnAllAccessible?"True":"False");
         file.WriteString("%*sm_bOffsetSMDTop = %s\n", indent, " ", m_bOffsetSMDTop?"True":"False");
         file.WriteString("%*sm_bOffsetPTHTop = %s\n", indent, " ", m_bOffsetPTHTop?"True":"False");
         file.WriteString("%*sm_bOffsetVIATPTop = %s\n", indent, " ", m_bOffsetVIATPTop?"True":"False");
         file.WriteString("%*sm_bOffsetSMDBot = %s\n", indent, " ", m_bOffsetSMDBot?"True":"False");
         file.WriteString("%*sm_bOffsetPTHBot = %s\n", indent, " ", m_bOffsetPTHBot?"True":"False");
         file.WriteString("%*sm_bOffsetVIATPBot = %s\n", indent, " ", m_bOffsetVIATPBot?"True":"False");
         file.WriteString("%*sm_bUseExistingProbes = %s\n", indent, " ", m_bUseExistingProbes?"True":"False");
         switch (m_eProbeUse)
         {
         case probeUsageUseExisting:
            tempBuf = "Use Existing";
            break;
         case probeUsageAddAdditional:
            tempBuf = "Add Addtional";
            break;
         }
         file.WriteString("%*sm_eProbeUse = %s\n", indent, " ", tempBuf);
         file.WriteString("%*sm_bRetainProbeNames = %s\n", indent, " ", m_bRetainProbeNames?"True":"False");
         file.WriteString("%*sm_bRetainProbePositions = %s\n", indent, " ", m_bRetainProbePositions?"True":"False");
         file.WriteString("%*sm_bRetainProbeSizes = %s\n", indent, " ", m_bRetainProbeSizes?"True":"False");
         indent -= 3;
      }
      file.WriteString("%*s\n", indent, " ");
      indent -= 3;
   }
}

void CTestPlan::WriteAAReport(CFormatStdioFile &file, int indent)
{
   int decimals = GetDecimals(m_pageUnits);

   CString buf;
   indent += 3;
   {
      file.WriteString("%*sHeight Analysis (Name, Dist, Min Height, Max Height)\n", indent, " ");
      indent += 3;
      {
         if (m_iSurface != testSurfaceBottom)
         {
            // Not bottom only
            file.WriteString("%*sTop: %s\n", indent, " ", m_heightAnalysisTop.IsUse()?"Enabled":"Disabled");
            indent += 3;

            for (POSITION pos = m_heightAnalysisTop.GetStartPosition(); pos != NULL;)
            {
               CHeightRange* heightRange = m_heightAnalysisTop.GetNext(pos);
               if (heightRange == NULL)
                  continue;
   
               file.WriteString("%*s%s, ", indent, " ", heightRange->GetName());
               file.WriteString("%0.*f, ", decimals, heightRange->GetOutlineDistance());
               file.WriteString("%0.*f, ", decimals, heightRange->GetMinHeight());

               if (heightRange->GetMaxHeight() > 0)
                  file.WriteString("%0.*f\n", decimals, heightRange->GetMaxHeight());
               else
                  file.WriteString("Infinity\n");
            }
            file.WriteString("\n");

            indent -= 3;
         }

         if (m_iSurface != testSurfaceTop)
         {
            // Not top only
            file.WriteString("%*sBottom: %s\n", indent, " ", m_heightAnalysisBot.IsUse()?"Enabled":"Disabled");
            indent += 3;

            for (POSITION pos = m_heightAnalysisBot.GetStartPosition(); pos != NULL;)
            {
               CHeightRange* heightRange = m_heightAnalysisBot.GetNext(pos);
               if (heightRange == NULL)
                  continue;

               file.WriteString("%*s%s, ", indent, " ", heightRange->GetName());
               file.WriteString("%0.*f, ", decimals, heightRange->GetOutlineDistance());
               file.WriteString("%0.*f, ", decimals, heightRange->GetMinHeight());

               if (heightRange->GetMaxHeight() > 0)
                  file.WriteString("%0.*f\n", decimals, heightRange->GetMaxHeight());
               else
                  file.WriteString("Infinity\n");
            }
            file.WriteString("\n");

            indent -= 3;
         }
      }
      indent -=3;

      file.WriteString("%*sConstraints\n", indent, " ");
      indent += 3;
      {
         file.WriteString("%*sEnable Soldermask: %s\n", indent, " ", m_bEnableSolermaskAnalysis?"Yes":"No");
         switch (m_iOutlineToUse)
         {
         case DFT_OUTLINE_NONE:
            buf = "No Outline";
            break;
         case DFT_OUTLINE_REALPART:
            buf = "Real Part Outline";
            break;
         case DFT_OUTLINE_DFT:
            buf = "DFT outline";
            break;
         case DFT_OUTLINE_BOTH:
            buf = "Real Part & DFT Outline";
            break;
         }
         file.WriteString("%*sOutline Used: %s\n", indent, " ", buf);
         if (m_iOutlineToUse == DFT_OUTLINE_BOTH)
            file.WriteString("%*sOutline Priority: %s\n", indent, " ", m_iOutlinePriority==0?"Real Part Outline":"DFT Outline");
         else
            file.WriteString("%*sOutline Priority: None\n", indent, " ");
         file.WriteString("%*sIgnore One Pin Comp Outline: %s\n", indent, " ", m_bIgnore1PinCompOutline?"Yes":"No");
         file.WriteString("%*sIgnore Unloaded Comp Outline: %s\n", indent, " ", m_bIgnoreUnloadedCompOutline?"Yes":"No");
         file.WriteString("\n");

         if (m_iSurface != testSurfaceBottom)
         {
            // Not bottom only
            file.WriteString("%*sTop\n", indent, " ");
            indent += 3;
            {
               file.WriteString("%*sBoard Outline: %0.*f, %s\n", indent, " ", decimals, m_constraintsTop.GetValueBoardOutline(), m_constraintsTop.GetEnableBoardOutline()?"Enabled":"Disabled");
               file.WriteString("%*sComp Outline: %0.*f, %s\n", indent, " ", decimals, m_constraintsTop.GetValueCompOutline(), m_constraintsTop.GetEnableCompOutline()?"Enabled":"Disabled");
               file.WriteString("%*sFeature Size: %0.*f, %s\n", indent, " ", decimals, m_constraintsTop.GetValueMinFeatureSize(), m_constraintsTop.GetEnableMinFeatureSize()?"Enabled":"Disabled");    
               file.WriteString("\n");
            }
            indent -= 3;
         }

         if (m_iSurface != testSurfaceTop)
         {
            // Not top only
            file.WriteString("%*sBottom\n", indent, " ");
            indent += 3;
            {
               file.WriteString("%*sBoard Outline: %0.*f, %s\n", indent, " ", decimals, m_constraintsBot.GetValueBoardOutline(), m_constraintsBot.GetEnableBoardOutline()?"Enabled":"Disabled");
               file.WriteString("%*sComp Outline: %0.*f, %s\n", indent, " ", decimals, m_constraintsBot.GetValueCompOutline(), m_constraintsBot.GetEnableCompOutline()?"Enabled":"Disabled");
               file.WriteString("%*sFeature Size: %0.*f, %s\n", indent, " ", decimals, m_constraintsBot.GetValueMinFeatureSize(), m_constraintsBot.GetEnableMinFeatureSize()?"Enabled":"Disabled");
               file.WriteString("\n");
            }
            indent -= 3;
         }
      }
      indent -=3;
      
      file.WriteString("%*sTarget Type\n", indent, " ");    
      indent += 3;
      {
         if (m_iSurface != testSurfaceBottom)
         {
            buf.Format("%s%s%s%s%s%s%s",
               m_targetTypesTop.GetEnableTestAttrib()?"Test Attribute, ":"", 
               m_targetTypesTop.GetEnableVia()?"Via, ":"",
               m_targetTypesTop.GetEnableConnector()?"Connector, ":"",
               m_targetTypesTop.GetEnableSMD()?"Multiple Pin SMD, ":"",
               m_targetTypesTop.GetEnableSinglePinSMD()?"Single Pin SMD, ":"",
               m_targetTypesTop.GetEnableTHUR()?"Through Hole, ":"",
               m_targetTypesTop.GetEnableCADPadstack()?"CAD Padstack, ":"");
            buf = buf.Left(buf.GetLength()-2);
            file.WriteString("%*sTop: %s\n", indent, " ", buf);
         }

         if (m_iSurface != testSurfaceTop)
         {
            buf.Format("%s%s%s%s%s%s%s",
               m_targetTypesBot.GetEnableTestAttrib()?"Test Attribute, ":"", 
               m_targetTypesBot.GetEnableVia()?"Via, ":"",
               m_targetTypesBot.GetEnableConnector()?"Connector, ":"",
               m_targetTypesBot.GetEnableSMD()?"Multiple Pin SMD, ":"",
               m_targetTypesBot.GetEnableSinglePinSMD()?"Single Pin SMD, ":"",
               m_targetTypesBot.GetEnableTHUR()?"Through Hole, ":"",
               m_targetTypesBot.GetEnableCADPadstack()?"CAD Padstack, ":"");
            buf = buf.Left(buf.GetLength()-2);
            file.WriteString("%*sBottom: %s\n", indent, " ", buf);
         }
         file.WriteString("\n");
      }
      indent -=3; 

      file.WriteString("%*sOption:\n", indent, " ");
      indent +=3;
      {
         switch (m_iSurface)
         {
         case testSurfaceTop:
            buf = "Top";
            break;
         case testSurfaceBottom:
            buf = "Bottom";
            break;
         case testSurfaceBoth:
            buf = "Both";
            break;
         }
         file.WriteString("%*sSurface: %s\n", indent, " ", buf);
         file.WriteString("%*sTester Type: %s\n", indent, " ", m_iTesterType==testerTypeFixture?"Fixture":"Fixtureless");
         file.WriteString("%*sInclude Multiple Pin Net: %s\n", indent, " ", m_bIncludeMulPinNet?"Yes":"No");
         file.WriteString("%*sInclude Single Pin Net: %s\n", indent, " ", m_bIncludeSngPinNet?"Yes":"No");
         file.WriteString("%*sInclude Unconnected Net: %s\n", indent, " ", m_bIncludeNCNet?"Yes":"No");
         file.WriteString("%*sRetain Existing Probe: %s\n", indent, " ", m_bRetainedExistingProbe?"Yes":"No");
         file.WriteString("%*sMultiple Reason Codes: %s\n", indent, " ", m_bMultipleReasonCodes?"Yes":"No");

         if (m_accessOffsetOptions.getEnableOffsetFlag())
         {
            CString delimiter;
            buf = "Yes (";

            CAccessOffsetItem& viaAccessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia);

            if (viaAccessOffsetItem.getEnableSmdOffsetFlag())
            {
               buf += delimiter + "SMD Via";
               delimiter = ", ";
            }

            if (viaAccessOffsetItem.getEnableThOffsetFlag())
            {
               buf += delimiter + "TH Via";

               if (viaAccessOffsetItem.getEnableThOffsetFlag())
               {
                  buf.AppendFormat("(%s)",viaAccessOffsetItem.getThOffsetDirectionMaskString());
               }

               delimiter = ", ";
            }

            CAccessOffsetItem& pinAccessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin);

            if (pinAccessOffsetItem.getEnableSmdOffsetFlag())
            {
               buf += delimiter + "SMD Pin";
               delimiter = ", ";
            }

            if (pinAccessOffsetItem.getEnableThOffsetFlag())
            {
               buf += delimiter + "TH Pin";

               if (pinAccessOffsetItem.getEnableThOffsetFlag())
               {
                  buf.AppendFormat("(%s)",pinAccessOffsetItem.getThOffsetDirectionMaskString());
               }

               delimiter = ", ";
            }

            CAccessOffsetItem& tpAccessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp);

            if (tpAccessOffsetItem.getEnableSmdOffsetFlag())
            {
               buf += delimiter + "SMD TP";
               delimiter = ", ";
            }

            if (tpAccessOffsetItem.getEnableThOffsetFlag())
            {
               buf += delimiter + "TH TP";

               if (tpAccessOffsetItem.getEnableThOffsetFlag())
               {
                  buf.AppendFormat("(%s)",tpAccessOffsetItem.getThOffsetDirectionMaskString());
               }

               delimiter = ", ";
            }

            buf += ")";

            //buf.Format("Yes (%s%s%s", m_bEnableViaOffset?"Via, ":"", m_bEnableSmdOffset?"SMD, ":"", m_bEnableThruOffset?"Through Hole, ":"");
            //buf = buf.Left(buf.GetLength()-2) + ")";
         }
         else
         {
            buf = "No";
         }

         file.WriteString("%*sAccess Offset: %s\n", indent, " ", buf);
         file.WriteString("%*sReport Ignored Surface: %s\n", indent, " ", m_bAAWriteIgnoredSurface?"Yes":"No");
      }
      indent -= 3;
   }
   indent -= 3;
}

void CTestPlan::WriteReport(CFormatStdioFile &file, int indent)
{
   CString buf;

   file.WriteString("%*sReport Ignored Surface: %s\n\n", indent, " ", m_bPPWriteIgnoredSurface?"Yes":"No");

   ////////////////////////////////////////
   // Physical Constraints
   file.WriteString("%*sEnable Soldermask : %s\n", indent, " ", m_bEnableSolermaskAnalysis?"True":"False");

   buf.Empty();
   switch (m_iOutlineToUse)
   {
   case -1: // None
      buf = "None";
      break;
   case 0:  // Real Part outline
      buf = "Real Part Outline";
      break;
   case 1:  // DFT outline
      buf = "DFT Outline";
      break;
   case 2:  // Both
      buf = "Real Part & DFT Outline";
      break;
   }
   file.WriteString("%*sUse Outline : %s\n", indent, " ", buf);

   buf.Empty();
   switch (m_iOutlinePriority)
   {
   case 0:  // Real Part outline
      buf = "Real Part Outline";
      break;
   case 1:  // DFT outline
      buf = "DFT Outline";
      break;
   }
   file.WriteString("%*sOutline Priority : %s\n", indent, " ", buf);

   //CPhysicalConst  m_constraintsTop;          
   //CPhysicalConst  m_constraintsBot;

   //////////////////////////////////////////
   //// Target Type
   //CTargetType m_targetTypesTop;
   //CTargetType  m_targetTypesBot;

   //////////////////////////////////////////
   //// Options
   buf.Empty();
   switch (m_iSurface)
   {
   case 0:  // Top
      buf = "Top";
      break;
   case 1:  // Bottom
      buf = "Bottom";
      break;
   case 2:  // Both
      buf = "Top & Bottom";
      break;
   }
   file.WriteString("%*sAccessible Surface : %s\n", indent, " ", buf);

   buf.Empty();
   switch (m_iTesterType)
   {
   case 0:  // Fixture
      buf = "Fixture";
      break;
   case 1:  // Fixtureless
      buf = "Fixtureless";
      break;
   }
   file.WriteString("%*sTester Type : %s\n", indent, " ", buf);
   file.WriteString("%*sInclude MultiPin Nets : %s\n", indent, " ", m_bIncludeMulPinNet?"True":"False");
   file.WriteString("%*sInclude SinglePin Nets : %s\n", indent, " ", m_bIncludeSngPinNet?"True":"False");
   file.WriteString("%*sInclude NC Nets : %s\n", indent, " ", m_bIncludeNCNet?"True":"False");

   //////////////////////////////////////////
   //// Target Priority
   //CDFTTargetPriority m_targetPriority;

   //////////////////////////////////////////
   //// Probes
   //CDFTProbeTemplates m_probeTemplates;

   //////////////////////////////////////////
   //// Kelvin & Power Injection 
   ////     Kelvin Analysis
   file.WriteString("%*sUse Resistors : %s\n", indent, " ", m_bUseResistor?"True":"False");
   file.WriteString("%*sResistor Value : %.3f\n", indent, " ", m_dResistorValue);
   file.WriteString("%*sUse Capacitors : %s\n", indent, " ", m_bUseCapacitor?"True":"False");
   file.WriteString("%*sCapacitor Value : %.3f\n", indent, " ", m_dCapacitorValue);
   file.WriteString("%*sUse Inductors : %s\n", indent, " ", m_bUseInductor?"True":"False");

   buf.Empty();
   switch (m_iPowerInjectionUsage)
   {
   case 0:  // No power injection
      buf = "No power injection";
      break;
   case 1:  // Probes per power rail
      buf = "Probes per power rail";
      break;
   case 2:  // Probes per number of connection per power rail
      buf = "Probes per Number of Connections per Power Rail";
      break;
   case 3:  // Probes per number of nets per power rail
      buf = "Probes per number of nets per power rail";
      break;
   }
   file.WriteString("%*sPower Injection Usage : %s\n", indent, " ", buf);
   file.WriteString("%*sPower Injection Value : %d\n", indent, " ", m_iPowerInjectionValue);

   //////////////////////////////////////////
   //// Preconditions
   //CDFTPreconditions m_preconditions;

   //////////////////////////////////////////
   //// Net Conditions
   //CDFTNetConditions m_netConditions;

   //////////////////////////////////////////
   //// Options
   file.WriteString("%*sProbe Start Number : %d\n", indent, " ", m_iProbeStartNumber);
   file.WriteString("%*sAllow Double Wiring: %s\n\n", indent, " ", m_bAllowDoubleWiring?"True":"False");

   file.WriteString("%*sPlace Probes on all Access Markers: %s\n", indent, " ", m_bPlaceOnAllAccessible?"True":"False");
   buf.Empty();
   switch (m_eProbeSide)
   {
   case 0:  // Top
      buf = "Top";
      break;
   case 1:  // Bottom
      buf = "Bottom";
      break;
   case 2:  // Both
      buf = "Top & Bottom";
      break;
   }
   file.WriteString("%*sProbe Side : %s\n", indent, " ", buf);

   buf.Empty();
   switch (m_iProbeSidePreference)
   {
   case 0:  // Top
      buf = "Top";
      break;
   case 1:  // Bottom
      buf = "Bottom";
      break;
   }
   file.WriteString("%*sProbe Side Preference : %s\n", indent, " ", buf);
}

void CTestPlan::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   int decimals = GetDecimals(m_pageUnits);

   writeFormat.writef("<DFTTestplan>\n");

   CString buf;
   writeFormat.pushHeader("  ");
   {
      switch (m_iSurface)
      {
         case 0:  // Top
            buf = "Top";
            break;
         case 1:  // Bottom
            buf = "Bottom";
            break;
         case 2:  // Both
            buf = "Both";
            break;
      }
      writeFormat.writef("<AccessAnalysis Surface=\"%s\"", buf);

      // Package Outline
      writeFormat.writef(" RectangularOutline=\"%s\"", m_bEnableRectangularOutline?"True":"False");
      writeFormat.writef(" OverwriteOutline=\"%s\"", m_bEnableOverwriteOutline?"True":"False");
      writeFormat.writef(" IncludeSinglePinComp=\"%s\"", m_bIncludeSinglePinComp?"True":"False");

      // Option
      switch (m_iTesterType)
      {
         case 0:  // Fixture
            buf = "Fixture";
            break;
         case 1:  // Fixtureless
            buf = "Fixtureless";
            break;
      }  

      writeFormat.writef(" TesterType=\"%s\"", buf);
      writeFormat.writef(" MultiPinNets=\"%s\"", m_bIncludeMulPinNet?"True":"False");
      writeFormat.writef(" SinglePinNets=\"%s\"", m_bIncludeSngPinNet?"True":"False");
      writeFormat.writef(" UnconnectedNets=\"%s\"", m_bIncludeNCNet?"True":"False");
      writeFormat.writef(" RetainExistingProbes=\"%s\"", m_bRetainedExistingProbe?"True":"False");
      writeFormat.writef(" MultipleReasonCodes=\"%s\"", m_bMultipleReasonCodes?"True":"False");
      writeFormat.writef(" AccessOffset=\"%s\"", m_accessOffsetOptions.getEnableOffsetFlag()?"True":"False");

      writeFormat.writef(" ViaSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).getEnableSmdOffsetFlagString());
      writeFormat.writef(" ViaThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).getEnableThOffsetString());

      writeFormat.writef(" PinSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getEnableSmdOffsetString());
      writeFormat.writef(" PinThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getEnableThOffsetString());

      writeFormat.writef(" TpSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp).getEnableSmdOffsetFlagString());
      writeFormat.writef(" TpThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp).getEnableThOffsetString());

      writeFormat.writef(" WriteIgnoredSurface=\"%s\"", m_bAAWriteIgnoredSurface?"True":"False");
      writeFormat.writef(" CreateSinglePinNets=\"%s\">\n", m_bCreateNCNet?"True":"False");

      writeFormat.pushHeader("  ");
      {
         // Height Analysis
         writeFormat.writef("<HeightAnalysis UseTop=\"%s\" UseBottom=\"%s\">\n", 
            m_heightAnalysisTop.IsUse()?"True":"False", m_heightAnalysisBot.IsUse()?"True":"False");
         {
            writeFormat.pushHeader("  ");

            //Top Height Analysis
            POSITION pos = m_heightAnalysisTop.GetStartPosition();
            while (pos != NULL)
            {
               CHeightRange* range = m_heightAnalysisTop.GetNext(pos);
               if (range == NULL)
                  continue;

               writeFormat.writef("<HeightRange Surface=\"Top\"");
               writeFormat.writef(" Name=\"%s\"", range->GetName());
               writeFormat.writef(" PackageOutlineDistance=\"%.*f\"", decimals, range->GetOutlineDistance());
               writeFormat.writef(" MinHeight=\"%.*f\"", decimals, range->GetMinHeight());
               writeFormat.writef(" MaxHeight=\"%.*f\"/>\n", decimals, range->GetMaxHeight());
            }

            //Bottom Height Analysis
            pos = m_heightAnalysisBot.GetStartPosition();
            while (pos != NULL)
            {
               CHeightRange* range = m_heightAnalysisBot.GetNext(pos);
               if (range == NULL)
                  continue;

               writeFormat.writef("<HeightRange Surface=\"Bottom\"");
               writeFormat.writef(" Name=\"%s\"", range->GetName());
               writeFormat.writef(" PackageOutlineDistance=\"%.*f\"", decimals, range->GetOutlineDistance());
               writeFormat.writef(" MinHeight=\"%.*f\"", decimals, range->GetMinHeight());
               writeFormat.writef(" MaxHeight=\"%.*f\"/>\n", decimals, range->GetMaxHeight());
            }

            writeFormat.popHeader();
         }
         writeFormat.writef("</HeightAnalysis>\n");

         // Physical Constraints
         writeFormat.writef("<PhysicalConstraints EnableSoldermask=\"%s\"", m_bEnableSolermaskAnalysis?"True":"False");
         {
            switch (m_iOutlineToUse)
            {
               case -1: // None
                  buf = "None";
                  break;
               case 0:  // Real Part outline
                  buf = "Real Part";
                  break;
               case 1:  // DFT outline
                  buf = "DFT";
                  break;
               case 2:  // Both
                  buf = "Both";
                  break;
            }
            writeFormat.writef(" OutlineToUse=\"%s\"", buf);

            switch (m_iOutlinePriority)
            {
               case 0:  // Real Part outline
                  buf = "Real Part outline";
                  break;
               case 1:  // DFT outline
                  buf = "DFT outline";
                  break;
            }  
            writeFormat.writef(" OutlinePriority=\"%s\"", buf);
            writeFormat.writef(" Ignore1PinCompOutline=\"%s\"", m_bIgnore1PinCompOutline?"True":"False");
            writeFormat.writef(" IgnoreUnloadedCompOutline=\"%s\">\n", m_bIgnoreUnloadedCompOutline?"True":"False");

            writeFormat.pushHeader("  ");
            writeFormat.writef("<BoardOutline TopEnabled=\"%s\" TopDistance=\"%.*f\" BottomEnabled=\"%s\" BottomDistance=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableBoardOutline()?"True":"False", decimals, m_constraintsTop.GetValueBoardOutline(),
               m_constraintsBot.GetEnableBoardOutline()?"True":"False", decimals, m_constraintsBot.GetValueBoardOutline());

            writeFormat.writef("<ComponentOutline TopEnabled=\"%s\" TopDistance=\"%.*f\" BottomEnabled=\"%s\" BottomDistance=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableCompOutline()?"True":"False", decimals, m_constraintsTop.GetValueCompOutline(),
               m_constraintsBot.GetEnableCompOutline()?"True":"False", decimals, m_constraintsBot.GetValueCompOutline());

            writeFormat.writef("<FeatureSize TopEnabled=\"%s\" TopSize=\"%.*f\" BottomEnabled=\"%s\" BottomSize=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableMinFeatureSize()?"True":"False", decimals, m_constraintsTop.GetValueMinFeatureSize(),
               m_constraintsBot.GetEnableMinFeatureSize()?"True":"False", decimals, m_constraintsBot.GetValueMinFeatureSize());

            writeFormat.writef("<BeadProbeSize TopEnabled=\"%s\" TopSize=\"%.*f\" BottomEnabled=\"%s\" BottomSize=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableBeadProbeSize()?"True":"False", decimals, m_constraintsTop.GetValueBeadProbeSize(),
               m_constraintsBot.GetEnableBeadProbeSize()?"True":"False", decimals, m_constraintsBot.GetValueBeadProbeSize());
            
            writeFormat.popHeader();
         }
         writeFormat.writef("</PhysicalConstraints>\n");

         // Target Types
         writeFormat.writef("<TargetType>\n");
         {
            writeFormat.pushHeader("  ");
            writeFormat.writef("<TargetTestAttribute Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableTestAttrib()?"True":"False", m_targetTypesBot.GetEnableTestAttrib()?"True":"False");
            writeFormat.writef("<TargetVia Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableVia()?"True":"False", m_targetTypesBot.GetEnableVia()?"True":"False");
            writeFormat.writef("<TargetConnector Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableConnector()?"True":"False", m_targetTypesBot.GetEnableConnector()?"True":"False");
            writeFormat.writef("<TargetSMD Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableSMD()?"True":"False", m_targetTypesBot.GetEnableSMD()?"True":"False");
            writeFormat.writef("<TargetSinglePinSMD Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableSinglePinSMD()?"True":"False", m_targetTypesBot.GetEnableSinglePinSMD()?"True":"False");
            writeFormat.writef("<TargetThroughHole Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableTHUR()?"True":"False", m_targetTypesBot.GetEnableTHUR()?"True":"False");
            writeFormat.writef("<TargetCadPadstacks Top=\"%s\" Bottom=\"%s\">\n",
               m_targetTypesTop.GetEnableCADPadstack()?"True":"False", m_targetTypesBot.GetEnableCADPadstack()?"True":"False");
            
            writeFormat.pushHeader("  ");
            POSITION pos = m_targetTypesTop.GetPadstackHeadPosition();
            while (pos)
            {
               int padStack = m_targetTypesTop.GetPadstackNext(pos);

               // TODO: get padstack name (need the doc)
               writeFormat.writef("<CadPadstack Surface=\"Top\" Name=\"%d\"/>\n", padStack);
            }
            pos = m_targetTypesBot.GetPadstackHeadPosition();
            while (pos)
            {
               int padStack = m_targetTypesBot.GetPadstackNext(pos);

               // TODO: get padstack name (need the doc)
               writeFormat.writef("<CadPadstack Surface=\"Bottom\" Name=\"%d\"/>\n", padStack);
            }
            writeFormat.popHeader();

            writeFormat.writef("</TargetCadPadstacks>\n");
            writeFormat.popHeader();
         }
         writeFormat.writef("</TargetType>\n");
      }
      writeFormat.popHeader();

      writeFormat.writef("</AccessAnalysis>\n");
   }

   {
      writeFormat.writef("<ProbePlacement OutlineToUse=\"%d\"", m_iProbeStartNumber);
      writeFormat.writef(" EnableDoubleWiring=\"%s\"", m_bAllowDoubleWiring?"True":"False");
      writeFormat.writef(" PlaceOnAllAccessible=\"%s\"", m_bPlaceOnAllAccessible?"True":"False");
      switch (m_eProbeSide)
      {
         case 0:  // Top
            buf = "Top";
            break;
         case 1:  // Bottom
            buf = "Bottom";
            break;
         case 2:  // Both
            buf = "Both";
            break;
      }
      writeFormat.writef(" WriteIgnoredSurface=\"%s\"", m_bPPWriteIgnoredSurface?"True":"False");
      writeFormat.writef(" NetConditionCaseSensitive=\"%s\"", m_bNetConditionCaseSensitive?"True":"False");
      writeFormat.writef(" Surface=\"%s\">\n", buf);

      writeFormat.pushHeader("  ");
      {
         m_targetPriority.WriteXML(writeFormat/*, progress*/);
         m_probeTemplates.WriteXML(writeFormat/*, progress*/);

         writeFormat.writef("<ProbeRefname NeverMirror=\"%s\"/>\n", m_bNeverMirrorRefname?"True":"False");

         writeFormat.writef("<KelvinAnalysis ResEnable=\"%s\"", m_bUseResistor?"True":"False");
         writeFormat.writef(" ResValue=\"%0.3f\"", m_dResistorValue);
         writeFormat.writef(" CapEnable=\"%s\"", m_bUseCapacitor?"True":"False");
         writeFormat.writef(" CapValue=\"%0.3f\"", m_dCapacitorValue);
         writeFormat.writef(" InductorEnable=\"%s\"/>\n", m_bUseInductor?"True":"False");

         writeFormat.writef("<PowerInjection Usage=\"%d\" Value=\"%d\"/>\n", m_iPowerInjectionUsage, m_iPowerInjectionValue);

         m_preconditions.WriteXML(writeFormat, progress);
         m_netConditions.WriteXML(writeFormat, progress);
      }
      writeFormat.popHeader();

      writeFormat.writef("</ProbePlacement>\n");
   }
   writeFormat.popHeader();

   writeFormat.writef("</DFTTestplan>\n");
}

void CTestPlan::ValidateSettings(PageUnitsTag pageUnits)
{
   if(true == IsBeadProbeUpdateNeeded())
   {
      GetTopConstraints()->SetValueBeadProbeSize(0.004 * Units_Factor(DFT_DEFAULT_UNIT, pageUnits));
      GetBotConstraints()->SetValueBeadProbeSize(0.004 * Units_Factor(DFT_DEFAULT_UNIT, pageUnits));
      SetBeadProbeUpdateNeeded(false);
   }
}

void CTestPlan::Scale(double factor)
{
   m_heightAnalysisTop.Scale(factor);
   m_heightAnalysisBot.Scale(factor);

   m_constraintsTop.SetValueBoardOutline(m_constraintsTop.GetValueBoardOutline() * factor);
   m_constraintsTop.SetValueCompOutline(m_constraintsTop.GetValueCompOutline() * factor);
   m_constraintsTop.SetValueMinFeatureSize(m_constraintsTop.GetValueMinFeatureSize() * factor);
   m_constraintsTop.SetValueBeadProbeSize(m_constraintsTop.GetValueBeadProbeSize() * factor);

   m_constraintsBot.SetValueBoardOutline(m_constraintsBot.GetValueBoardOutline() * factor);
   m_constraintsBot.SetValueCompOutline(m_constraintsBot.GetValueCompOutline() * factor);
   m_constraintsBot.SetValueMinFeatureSize(m_constraintsBot.GetValueMinFeatureSize() * factor);
   m_constraintsBot.SetValueBeadProbeSize(m_constraintsBot.GetValueBeadProbeSize() * factor);
}

int CTestPlan::SaveToFile(CString fileName, int pageUnit)
{

   FILE *file = fopen(fileName, "w");
   if (file == NULL)
      return -1;

   int decimals = GetDecimals(m_pageUnits);

   CStreamFileWriteFormat writeFormat(file, 1024);
   writeFormat.setNewLineMode(true);

   int indent = 0;
   writeFormat.writef("<DFTTestplan TestPlanUnit=\"%d\">\n", pageUnit);

   CString buf;
   writeFormat.pushHeader("  ");
   {
      switch (m_iSurface)
      {
         case 0:  // Top
            buf = "Top";
            break;
         case 1:  // Bottom
            buf = "Bottom";
            break;
         case 2:  // Both
            buf = "Both";
            break;
      }
      writeFormat.writef("<AccessAnalysis Surface=\"%s\"", buf);

      // Package Outline
      writeFormat.writef(" RectangularOutline=\"%s\"", m_bEnableRectangularOutline?"True":"False");
      writeFormat.writef(" OverwriteOutline=\"%s\"", m_bEnableOverwriteOutline?"True":"False");
      writeFormat.writef(" IncludeSinglePinComp=\"%s\"", m_bIncludeSinglePinComp?"True":"False");

      // Option
      switch (m_iTesterType)
      {
         case 0:  // Fixture
            buf = "Fixture";
            break;
         case 1:  // Fixtureless
            buf = "Fixtureless";
            break;
      }  
      writeFormat.writef(" TesterType=\"%s\"", buf);
      writeFormat.writef(" MultiPinNets=\"%s\"", m_bIncludeMulPinNet?"True":"False");
      writeFormat.writef(" SinglePinNets=\"%s\"", m_bIncludeSngPinNet?"True":"False");
      writeFormat.writef(" UnconnectedNets=\"%s\"", m_bIncludeNCNet?"True":"False");
      writeFormat.writef(" RetainExistingProbes=\"%s\"", m_bRetainedExistingProbe?"True":"False");
      writeFormat.writef(" MultipleReasonCodes=\"%s\"", m_bMultipleReasonCodes?"True":"False");
      writeFormat.writef(" AccessOffset=\"%s\"", m_accessOffsetOptions.getEnableOffsetFlag()?"True":"False");

      writeFormat.writef(" ViaSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).getEnableSmdOffsetFlagString());
      writeFormat.writef(" ViaThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).getEnableThOffsetString());

      writeFormat.writef(" PinSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getEnableSmdOffsetString());
      writeFormat.writef(" PinThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getEnableThOffsetString());

      writeFormat.writef(" TpSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp).getEnableSmdOffsetFlagString());
      writeFormat.writef(" TpThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp).getEnableThOffsetString());

      writeFormat.writef(" WriteIgnoredSurface=\"%s\"", m_bAAWriteIgnoredSurface?"True":"False");
      writeFormat.writef(" CreateSinglePinNets=\"%s\">\n", m_bCreateNCNet?"True":"False");

      writeFormat.pushHeader("  ");
      {
         // Height Analysis
         writeFormat.writef("<HeightAnalysis UseTop=\"%s\" UseBottom=\"%s\">\n", 
            m_heightAnalysisTop.IsUse()?"True":"False", m_heightAnalysisBot.IsUse()?"True":"False");
         {
            writeFormat.pushHeader("  ");

            //Top Height Analysis
            POSITION pos = m_heightAnalysisTop.GetStartPosition();
            while (pos != NULL)
            {
               CHeightRange* range = m_heightAnalysisTop.GetNext(pos);
               if (range == NULL)
                  continue;

               writeFormat.writef("<HeightRange Surface=\"Top\"");
               writeFormat.writef(" Name=\"%s\"", range->GetName());
               writeFormat.writef(" PackageOutlineDistance=\"%.*f\"", decimals, range->GetOutlineDistance());
               writeFormat.writef(" MinHeight=\"%.*f\"", decimals, range->GetMinHeight());
               writeFormat.writef(" MaxHeight=\"%.*f\"/>\n", decimals, range->GetMaxHeight());
            }

            //Bottom Height Analysis
            pos = m_heightAnalysisBot.GetStartPosition();
            while (pos != NULL)
            {
               CHeightRange* range = m_heightAnalysisBot.GetNext(pos);
               if (range == NULL)
                  continue;

               writeFormat.writef("<HeightRange Surface=\"Bottom\"");
               writeFormat.writef(" Name=\"%s\"", range->GetName());
               writeFormat.writef(" PackageOutlineDistance=\"%.*f\"", decimals, range->GetOutlineDistance());
               writeFormat.writef(" MinHeight=\"%.*f\"", decimals, range->GetMinHeight());
               writeFormat.writef(" MaxHeight=\"%.*f\"/>\n", decimals, range->GetMaxHeight());
            }

            writeFormat.popHeader();
         }
         writeFormat.writef("</HeightAnalysis>\n");

         // Physical Cosntraints
         writeFormat.writef("<PhysicalConstraints EnableSoldermask=\"%s\"", m_bEnableSolermaskAnalysis?"True":"False");
         {
            switch (m_iOutlineToUse)
            {
               case -1: // None
                  buf = "None";
                  break;
               case 0:  // Real Part outline
                  buf = "Real Part";
                  break;
               case 1:  // DFT outline
                  buf = "DFT";
                  break;
               case 2:  // Both
                  buf = "Both";
                  break;
            }
            writeFormat.writef(" OutlineToUse=\"%s\"", buf);

            switch (m_iOutlinePriority)
            {
               case 0:  // Real Part outline
                  buf = "Real Part outline";
                  break;
               case 1:  // DFT outline
                  buf = "DFT outline";
                  break;
            }  
            writeFormat.writef(" OutlinePriority=\"%s\"", buf);
            writeFormat.writef(" Ignore1PinCompOutline=\"%s\"", m_bIgnore1PinCompOutline?"True":"False");
            writeFormat.writef(" IgnoreUnloadedCompOutline=\"%s\">\n", m_bIgnoreUnloadedCompOutline?"True":"False");

            writeFormat.pushHeader("  ");
            writeFormat.writef("<BoardOutline TopEnabled=\"%s\" TopDistance=\"%.*f\" BottomEnabled=\"%s\" BottomDistance=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableBoardOutline()?"True":"False", decimals, m_constraintsTop.GetValueBoardOutline(),
               m_constraintsBot.GetEnableBoardOutline()?"True":"False", decimals, m_constraintsBot.GetValueBoardOutline());

            writeFormat.writef("<ComponentOutline TopEnabled=\"%s\" TopDistance=\"%.*f\" BottomEnabled=\"%s\" BottomDistance=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableCompOutline()?"True":"False", decimals, m_constraintsTop.GetValueCompOutline(),
               m_constraintsBot.GetEnableCompOutline()?"True":"False", decimals, m_constraintsBot.GetValueCompOutline());

            writeFormat.writef("<FeatureSize TopEnabled=\"%s\" TopSize=\"%.*f\" BottomEnabled=\"%s\" BottomSize=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableMinFeatureSize()?"True":"False", decimals, m_constraintsTop.GetValueMinFeatureSize(),
               m_constraintsBot.GetEnableMinFeatureSize()?"True":"False", decimals, m_constraintsBot.GetValueMinFeatureSize());

            writeFormat.writef("<BeadProbeSize TopEnabled=\"%s\" TopSize=\"%.*f\" BottomEnabled=\"%s\" BottomSize=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableBeadProbeSize()?"True":"False", decimals, m_constraintsTop.GetValueBeadProbeSize(),
               m_constraintsBot.GetEnableBeadProbeSize()?"True":"False", decimals, m_constraintsBot.GetValueBeadProbeSize());


            writeFormat.popHeader();
         }
         writeFormat.writef("</PhysicalConstraints>\n");

         // Target Types
         writeFormat.writef("<TargetType>\n");
         {
            writeFormat.pushHeader("  ");
            writeFormat.writef("<TargetTestAttribute Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableTestAttrib()?"True":"False", m_targetTypesBot.GetEnableTestAttrib()?"True":"False");
            writeFormat.writef("<TargetVia Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableVia()?"True":"False", m_targetTypesBot.GetEnableVia()?"True":"False");
            writeFormat.writef("<TargetConnector Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableConnector()?"True":"False", m_targetTypesBot.GetEnableConnector()?"True":"False");
            writeFormat.writef("<TargetSMD Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableSMD()?"True":"False", m_targetTypesBot.GetEnableSMD()?"True":"False");
            writeFormat.writef("<TargetSinglePinSMD Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableSinglePinSMD()?"True":"False", m_targetTypesBot.GetEnableSinglePinSMD()?"True":"False");
            writeFormat.writef("<TargetThroughHole Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableTHUR()?"True":"False", m_targetTypesBot.GetEnableTHUR()?"True":"False");
            writeFormat.writef("<TargetCadPadstacks Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableCADPadstack()?"True":"False", m_targetTypesBot.GetEnableCADPadstack()?"True":"False");
            writeFormat.popHeader();
         }
         writeFormat.writef("</TargetType>\n");
      }
      writeFormat.popHeader();

      writeFormat.writef("</AccessAnalysis>\n");
   }

   {
      writeFormat.writef("<ProbePlacement ProbeStartNumber=\"%d\"", m_iProbeStartNumber);
      writeFormat.writef(" EnableDoubleWiring=\"%s\"", m_bAllowDoubleWiring?"True":"False");
      writeFormat.writef(" PlaceOnAllAccessible=\"%s\"", m_bPlaceOnAllAccessible?"True":"False");
      switch (m_eProbeSide)
      {
         case 0:  // Top
            buf = "Top";
            break;
         case 1:  // Bottom
            buf = "Bottom";
            break;
         case 2:  // Both
            buf = "Both";
            break;
      }
      writeFormat.writef(" WriteIgnoredSurface=\"%s\"", m_bPPWriteIgnoredSurface?"True":"False");
      writeFormat.writef(" NetConditionCaseSensitive=\"%s\"", m_bNetConditionCaseSensitive?"True":"False");
      writeFormat.writef(" Surface=\"%s\">\n", buf);

      writeFormat.pushHeader("  ");
      {
         m_targetPriority.WriteXML(writeFormat);
         m_probeTemplates.WriteXML(writeFormat);

         writeFormat.writef("<ProbeRefname NeverMirror=\"%s\"/>\n", m_bNeverMirrorRefname?"True":"False");

         writeFormat.writef("<KelvinAnalysis ResEnable=\"%s\"", m_bUseResistor?"True":"False");
         writeFormat.writef(" ResValue=\"%0.3f\"", m_dResistorValue);
         writeFormat.writef(" CapEnable=\"%s\"", m_bUseCapacitor?"True":"False");
         writeFormat.writef(" CapValue=\"%0.3f\"", m_dCapacitorValue);
         writeFormat.writef(" InductorEnable=\"%s\"/>\n", m_bUseInductor?"True":"False");

         writeFormat.writef("<PowerInjection Usage=\"%d\" Value=\"%d\"/>\n", m_iPowerInjectionUsage, m_iPowerInjectionValue);
      }
      writeFormat.popHeader();

      writeFormat.writef("</ProbePlacement>\n");
   }
   writeFormat.popHeader();

   writeFormat.writef("</DFTTestplan>\n");

   fclose(file);

   return 0;
}

int CTestPlan::LoadFile(CString fileName, int pageUnit)
{
   CXMLDocument xmlDoc;

   if (xmlDoc.LoadXMLFile(fileName))
   {
      CXMLNodeList *nodeList = xmlDoc.GetElementsByTagName("DFTTestplan");

      CXMLNode *node = NULL;
      nodeList->Reset();
      while (node = nodeList->NextNode())
         LoadXML(node, NULL, pageUnit);

      return 1;
   }

   return 0;
}

int CTestPlan::LoadFile_PPOnly(CString fileName)
{
   CXMLDocument xmlDoc;

   if (xmlDoc.LoadXMLFile(fileName))
   {
      CXMLNodeList *nodeList = xmlDoc.GetElementsByTagName("DFTTestplan");

      CXMLNode *node = NULL;
      nodeList->Reset();
      while (node = nodeList->NextNode())
         loadXML_PPOnly(node, NULL);

      return 1;
   }

   return 0;
}

int CTestPlan::loadXML_PPOnly(CXMLNode *node, FileStruct *file)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "DFTTestplan")
      return 0;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName))
      {
         if (nodeName == "ProbePlacement")
            loadProbePlacement(subNode, file);
      }

      delete subNode;
   }

   return 0;
}

int CTestPlan::LoadXML(CXMLNode *node, FileStruct *file, int pageUnit)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "DFTTestplan")
      return 0;

   CString value;
   double unitFactor = 1.0;
   if (node->GetAttrValue("TestPlanUnit", value))
   {
      int testPlanUnit = atoi(value);
      unitFactor = Units_Factor(testPlanUnit, pageUnit);
   }

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName))
      {
         if (nodeName == "AccessAnalysis")
         {
            loadAccessAnalysis(subNode, unitFactor, pageUnit);
         }
         else if (nodeName == "ProbePlacement")
         {
            loadProbePlacement(subNode, file);
         }
      }

      delete subNode;
   }

   return 0;
}

int CTestPlan::loadAccessAnalysis(CXMLNode *node, double unitFactor, int pageUnit)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "AccessAnalysis")
      return 0;

   CString value;
   if (node->GetAttrValue("Surface", value))
   {
      value.MakeLower();
      if (value.Compare("top") == 0)
         m_iSurface = 0;
      else if (value.Compare("bottom") == 0)
         m_iSurface = 1;
      else if (value.Compare("both") == 0)
         m_iSurface = 2;
   }

   // Package Outline
   if (node->GetAttrValue("RectangularOutline", value))
      m_bEnableRectangularOutline = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("OverwriteOutline", value))
      m_bEnableOverwriteOutline = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("IncludeSinglePinComp", value))
      m_bIncludeSinglePinComp = (value.CompareNoCase("True") == 0);

   // Option
   if (node->GetAttrValue("TesterType", value))
   {
      value.MakeLower();
      if (value.Compare("fixture") == 0)
         m_iTesterType = 0;
      else if (value.Compare("fixtureless") == 0)
         m_iTesterType = 1;
   }

   if (node->GetAttrValue("MultiPinNets", value))
      m_bIncludeMulPinNet = (value.CompareNoCase("True") == 0);

   if (node->GetAttrValue("SinglePinNets", value))
      m_bIncludeSngPinNet = (value.CompareNoCase("True") == 0);

   if (node->GetAttrValue("UnconnectedNets", value))
      m_bIncludeNCNet = (value.CompareNoCase("True") == 0);

   if (node->GetAttrValue("RetainExistingProbes", value))
      m_bRetainedExistingProbe = (value.CompareNoCase("True") == 0);

   if (node->GetAttrValue("MultipleReasonCodes", value))
      m_bMultipleReasonCodes = (value.CompareNoCase("True") == 0);

   if (node->GetAttrValue("AccessOffset", value))
      m_accessOffsetOptions.setEnableOffsetFlag(value.CompareNoCase("True") == 0);

   if (node->GetAttrValue("ViaSmdOffset", value))
   {
      m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).setEnableSmdOffsetFlag((value.CompareNoCase("True") == 0));
   }

   if (node->GetAttrValue("ViaThOffset", value))
   {
      CSupString valueString(value);
      CStringArray params;
      valueString.Parse(params,"()");
      CAccessOffsetItem& accessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia);

      if (params.GetCount() > 0 && params.GetAt(0).CompareNoCase("True") == 0)
      {
         accessOffsetItem.setEnableThOffsetFlag(true);

         if (params.GetCount() > 1)
         {
            accessOffsetItem.setThOffsetDirectionMaskString(params.GetAt(1));
         }
      }
   }

   if (node->GetAttrValue("PinSmdOffset", value))
   {
      CSupString valueString(value);
      CStringArray params;
      valueString.Parse(params,"()");
      CAccessOffsetItem& accessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin);

      if (params.GetCount() > 0 && params.GetAt(0).CompareNoCase("True") == 0)
      {
         accessOffsetItem.setEnableSmdOffsetFlag(true);

         if (params.GetCount() > 1)
         {
            accessOffsetItem.setSmdOffsetPlacement(params.GetAt(1));
         }
      }
      else
      {
         accessOffsetItem.setEnableSmdOffsetFlag(false);
      }
   }

   if (node->GetAttrValue("PinThOffset", value))
   {
      CSupString valueString(value);
      CStringArray params;
      valueString.Parse(params,"()");
      CAccessOffsetItem& accessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin);

      if (params.GetCount() > 0 && params.GetAt(0).CompareNoCase("True") == 0)
      {
         accessOffsetItem.setEnableThOffsetFlag(true);

         if (params.GetCount() > 1)
         {
            accessOffsetItem.setThOffsetDirectionMaskString(params.GetAt(1));
         }
      }
   }

   if (node->GetAttrValue("TpSmdOffset", value))
   {
      m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp).setEnableSmdOffsetFlag((value.CompareNoCase("True") == 0));
   }

   if (node->GetAttrValue("TpThOffset", value))
   {
      CSupString valueString(value);
      CStringArray params;
      valueString.Parse(params,"()");
      CAccessOffsetItem& accessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp);

      if (params.GetCount() > 0 && params.GetAt(0).CompareNoCase("True") == 0)
      {
         accessOffsetItem.setEnableThOffsetFlag(true);

         if (params.GetCount() > 1)
         {
            accessOffsetItem.setThOffsetDirectionMaskString(params.GetAt(1));
         }
      }
   }

   if (node->GetAttrValue("WriteIgnoredSurface", value))
      m_bAAWriteIgnoredSurface = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("CreateSinglePinNets", value))
      m_bCreateNCNet = (value.CompareNoCase("True") == 0);

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName))
      {
         if (nodeName == "HeightAnalysis")
         {
            loadHeightAnalysis(subNode, unitFactor, pageUnit);
         }
         else if (nodeName == "PhysicalConstraints")
         {
            loadPhysicalConstrants(subNode, unitFactor);
         }
         else if (nodeName == "TargetType")
         {
            loadTargetTypes(subNode);
         }
      }

      delete subNode;
   }

   return 0;
}

int CTestPlan::loadHeightAnalysis(CXMLNode *node, double unitFactor, int pageUnit)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "HeightAnalysis")
      return 0;


   // Clear all the height range from Height Analysis
   m_heightAnalysisTop.RemoveAllRange();
   m_heightAnalysisBot.RemoveAllRange();

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName) && nodeName.CompareNoCase("HeightRange") == 0)
      {
         CString surface, name, outlineDistance, minHeight, maxHeight;

         if (!subNode->GetAttrValue("Surface", surface) || surface.IsEmpty())
            continue;
         if (!subNode->GetAttrValue("Name", name) || name.IsEmpty())
            continue;
         if (!subNode->GetAttrValue("PackageOutlineDistance", outlineDistance) || outlineDistance.IsEmpty())
            continue;
         if (!subNode->GetAttrValue("MinHeight", minHeight) || minHeight.IsEmpty())
            continue;
         if (!subNode->GetAttrValue("MaxHeight", maxHeight) || maxHeight.IsEmpty())
            continue;

         if (surface.CompareNoCase("Top") == 0)
            m_heightAnalysisTop.AddHeightRange(name, atof(outlineDistance)*unitFactor, atof(minHeight)*unitFactor, atof(maxHeight)*unitFactor);
         else
            m_heightAnalysisBot.AddHeightRange(name, atof(outlineDistance)*unitFactor, atof(minHeight)*unitFactor, atof(maxHeight)*unitFactor);
      }
      delete subNode;
   }


   // If there is no height range in CC file then reset back to default range
   if (m_heightAnalysisTop.GetCount() == 0)
      m_heightAnalysisTop.SetDefault(intToPageUnitsTag(pageUnit));
   if (m_heightAnalysisBot.GetCount() == 0)
      m_heightAnalysisBot.SetDefault(intToPageUnitsTag(pageUnit));


   // Set the use of Height Analysis
   CString value;
   if (node->GetAttrValue("UseTop", value))
      m_heightAnalysisTop.SetUse((value.CompareNoCase("True") == 0));
   if (node->GetAttrValue("UseBottom", value))
      m_heightAnalysisBot.SetUse((value.CompareNoCase("True") == 0));

   return 0;
}

int CTestPlan::loadPhysicalConstrants(CXMLNode *node, double unitFactor)
{
   if (node == NULL)
      return -1;

   m_beadProbeUpdateNeeded = true;
   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "PhysicalConstraints")
      return 0;

   CString value;
   if (node->GetAttrValue("EnableSoldermask", value))
      m_bEnableSolermaskAnalysis = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("OutlineToUse", value))
   {
      value.MakeLower();
      if (value.Compare("none") == 0)
         m_iOutlineToUse = -1;
      else if (value.Compare("real part") == 0)
         m_iOutlineToUse = 0;
      else if (value.Compare("dft") == 0)
         m_iOutlineToUse = 1;
      else if (value.Compare("both") == 0)
         m_iOutlineToUse = 2;
   }
   if (node->GetAttrValue("OutlinePriority", value))
   {
      value.MakeLower();
      if (value.Compare("real part outline") == 0)
         m_iOutlinePriority = 0;
      else if (value.Compare("dft outline") == 0)
         m_iOutlinePriority = 1;
   }
   if (node->GetAttrValue("Ignore1PinCompOutline", value))
      m_bIgnore1PinCompOutline = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("IgnoreUnloadedCompOutline", value))
      m_bIgnoreUnloadedCompOutline = (value.CompareNoCase("True") == 0);

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName))
      {
         if (nodeName == "BoardOutline")
         {
            CString subValue;
            if (subNode->GetAttrValue("TopEnabled", subValue))
               m_constraintsTop.SetEnableBoardOutline(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("TopDistance", subValue))
               m_constraintsTop.SetValueBoardOutline(atof(subValue) * unitFactor);
            if (subNode->GetAttrValue("BottomEnabled", subValue))
               m_constraintsBot.SetEnableBoardOutline(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("BottomDistance", subValue))
               m_constraintsBot.SetValueBoardOutline(atof(subValue) * unitFactor);
         }
         else if (nodeName == "ComponentOutline")
         {
            CString subValue;
            if (subNode->GetAttrValue("TopEnabled", subValue))
               m_constraintsTop.SetEnableCompOutline(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("TopDistance", subValue))
               m_constraintsTop.SetValueCompOutline(atof(subValue) * unitFactor);
            if (subNode->GetAttrValue("BottomEnabled", subValue))
               m_constraintsBot.SetEnableCompOutline(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("BottomDistance", subValue))
               m_constraintsBot.SetValueCompOutline(atof(subValue) * unitFactor);
         }
         else if (nodeName == "FeatureSize")
         {
            CString subValue;
            if (subNode->GetAttrValue("TopEnabled", subValue))
               m_constraintsTop.SetEnableMinFeatureSize(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("TopSize", subValue))
               m_constraintsTop.SetValueMinFeatureSize(atof(subValue) * unitFactor);
            if (subNode->GetAttrValue("BottomEnabled", subValue))
               m_constraintsBot.SetEnableMinFeatureSize(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("BottomSize", subValue))
               m_constraintsBot.SetValueMinFeatureSize(atof(subValue) * unitFactor);
         }
         else if (nodeName == "BeadProbeSize")
         {
            CString subValue;
            if (subNode->GetAttrValue("TopEnabled", subValue))
               m_constraintsTop.SetEnableBeadProbeSize(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("TopSize", subValue))
               m_constraintsTop.SetValueBeadProbeSize(atof(subValue) * unitFactor);
            if (subNode->GetAttrValue("BottomEnabled", subValue))
               m_constraintsBot.SetEnableBeadProbeSize(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("BottomSize", subValue))
               m_constraintsBot.SetValueBeadProbeSize(atof(subValue) * unitFactor);
            m_beadProbeUpdateNeeded = false;
         }
      }

      delete subNode;
   }

   return 0;
}

int CTestPlan::loadTargetTypes(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "TargetType")
      return 0;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName))
      {
         if (nodeName == "TargetTestAttribute")
         {
            CString subValue;
            if (subNode->GetAttrValue("Top", subValue))
               m_targetTypesTop.SetEnableTestAttrib(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("Bottom", subValue))
               m_targetTypesBot.SetEnableTestAttrib(subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "TargetVia")
         {
            CString subValue;
            if (subNode->GetAttrValue("Top", subValue))
               m_targetTypesTop.SetEnableVia(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("Bottom", subValue))
               m_targetTypesBot.SetEnableVia(subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "TargetConnector")
         {
            CString subValue;
            if (subNode->GetAttrValue("Top", subValue))
               m_targetTypesTop.SetEnableConnector(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("Bottom", subValue))
               m_targetTypesBot.SetEnableConnector(subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "TargetSMD")
         {
            CString subValue;
            if (subNode->GetAttrValue("Top", subValue))
               m_targetTypesTop.SetEnableSMD(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("Bottom", subValue))
               m_targetTypesBot.SetEnableSMD(subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "TargetSinglePinSMD")
         {
            CString subValue;
            if (subNode->GetAttrValue("Top", subValue))
               m_targetTypesTop.SetEnableSinglePinSMD(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("Bottom", subValue))
               m_targetTypesBot.SetEnableSinglePinSMD(subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "TargetThroughHole")
         {
            CString subValue;
            if (subNode->GetAttrValue("Top", subValue))
               m_targetTypesTop.SetEnableTHUR(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("Bottom", subValue))
               m_targetTypesBot.SetEnableTHUR(subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "TargetCadPadstacks")
         {
            CString subValue;
            if (subNode->GetAttrValue("Top", subValue))
               m_targetTypesTop.SetEnableCADPadstack(subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("Bottom", subValue))
               m_targetTypesBot.SetEnableCADPadstack(subValue.CompareNoCase("True") == 0);

            CXMLNode *padStackNode = NULL;
            subNode->ResetChildList();
            while (padStackNode = subNode->NextChild())
            {
					CString padNodeName;
					if (padStackNode->GetName(padNodeName))
					{
						if (padNodeName == "CadPadstack")
						{
							CString padStackSurfaceValue, padStackGeomNumber;
							padStackNode->GetAttrValue("Surface", padStackSurfaceValue);
							if (padStackNode->GetAttrValue("Name", padStackGeomNumber))
							{
								if (is_number(padStackGeomNumber) > 0)
								{
									if (padStackSurfaceValue.CompareNoCase("Top") == 0)
										m_targetTypesTop.AddPadstackHead(atoi(padStackGeomNumber));
									else
										m_targetTypesBot.AddPadstackHead(atoi(padStackGeomNumber));
								}
							}					
						}
					}
            }
         }
      } // END if (subNode->GetName(nodeName)) ...

      delete subNode;
   }

   return 0;
}

int CTestPlan::loadProbePlacement(CXMLNode *node, FileStruct *file)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "ProbePlacement")
      return 0;

   CString value;
   if (node->GetAttrValue("ProbeStartNumber", value))
      m_iProbeStartNumber = atoi(value);
   if (node->GetAttrValue("EnableDoubleWiring", value))
      m_bAllowDoubleWiring = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("PlaceOnAllAccessible", value))
      m_bPlaceOnAllAccessible = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("Surface", value))
   {
      value.MakeLower();
      if (value.Compare("top") == 0)
         m_eProbeSide = testSurfaceTop;
      else if (value.Compare("bottom") == 0)
         m_eProbeSide = testSurfaceBottom;
      else if (value.Compare("both") == 0)
         m_eProbeSide = testSurfaceBoth;
   }
   if (node->GetAttrValue("WriteIgnoredSurface", value))
      m_bPPWriteIgnoredSurface = (value.CompareNoCase("True") == 0);
   if (node->GetAttrValue("NetConditionCaseSensitive", value))
      m_bNetConditionCaseSensitive = (value.CompareNoCase("True") == 0);

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName))
      {
         if (nodeName == "TargetPriorities")
         {
            m_targetPriority.LoadXML(subNode);
         }
         else if (nodeName == "ProbeTemplates")
         {
            m_probeTemplates.LoadXML(subNode);
         }
         else if (nodeName == "ProbeRefname")
         {
            CString subValue;
            if (subNode->GetAttrValue("NeverMirror", subValue))
               m_bNeverMirrorRefname = (subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "KelvinAnalysis")
         {
            CString subValue;
            if (subNode->GetAttrValue("ResEnable", subValue))
               m_bUseResistor = (subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("ResValue", subValue))
               m_dResistorValue = atof(subValue);
            if (subNode->GetAttrValue("CapEnable", subValue))
               m_bUseCapacitor = (subValue.CompareNoCase("True") == 0);
            if (subNode->GetAttrValue("CapValue", subValue))
               m_dCapacitorValue= atof(subValue);
            if (subNode->GetAttrValue("InductorEnable", subValue))
               m_bUseInductor = (subValue.CompareNoCase("True") == 0);
         }
         else if (nodeName == "PowerInjection")
         {
            CString subValue;
            if (subNode->GetAttrValue("Usage", subValue))
               m_iPowerInjectionUsage = atoi(subValue);
            if (subNode->GetAttrValue("Value", subValue))
               m_iPowerInjectionValue = atoi(subValue);
         }
         else if (nodeName == "Preconditions")
         {
            m_preconditions.LoadXML(subNode);
         }
         else if (nodeName == "NetConditions")
         {
            m_netConditions.LoadXML(subNode, file);
         }
      }

      delete subNode;
   }

   return 0;
}





/////////////////////////////////////////////////////////////////////////////
// CDFTSolution
/////////////////////////////////////////////////////////////////////////////
CDFTSolution::CDFTSolution(CString name, bool flipped, PageUnitsTag pageUnits)
: m_TestPlan(pageUnits)
{
   m_name = name;
   m_flipped = flipped;
   m_aaSolution = NULL;
   m_ppSolution = NULL;
}

CDFTSolution::~CDFTSolution()
{
}

void CDFTSolution::DestroySolutions()
{
   delete m_aaSolution;
   delete m_ppSolution;
}

//CDFTSolution& CDFTSolution::operator=(const CDFTSolution &dftSolution)
//{
// if (&dftSolution != this)
// {
//    m_name = dftSolution.m_name;
//    m_TestPlan = dftSolution.m_TestPlan;
//
//    // TODO: copy the content
//    m_aaSolution = dftSolution.m_aaSolution;
//    m_ppSolution = dftSolution.m_ppSolution;
// }
//
// return *this;
//}

CAccessAnalysisSolution *CDFTSolution::GetAccessAnalysisSolution()
{ 
   return m_aaSolution;
}

CProbePlacementSolution *CDFTSolution::GetProbePlacementSolution()
{
   return m_ppSolution;
}

bool CDFTSolution::DumpToFile(CString Filename)
{
   CFormatStdioFile file;
   CFileException err;
   CString tempBuf;

   if (!file.Open(Filename, CFile::modeCreate|CFile::modeWrite, &err))
      return false;

   int indent = 0;
   file.WriteString("//////////////////////////////////////////////\n");
   file.WriteString("// %s\n", Filename);
   file.WriteString("//\n");
   file.WriteString("// Dump of current testplan\n");
   file.WriteString("//////////////////////////////////////////////\n");
   file.WriteString("\n");

   m_TestPlan.DumpToFile(file, indent);
   dumpPPToFile(file, indent);
   dumpAAToFile(file, indent);

   file.Close();

   return true;
}

void CDFTSolution::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<DFTSolution name=\"%s\" flipped=\"%s\">\n", 
      SwapSpecialCharacters(m_name),
      IsFlipped() ? "True" : "False");

   writeFormat.pushHeader("  ");
   m_TestPlan.WriteXML(writeFormat, progress);
   if (m_aaSolution != NULL)
      m_aaSolution->WriteXML(writeFormat, progress);
   if (m_ppSolution != NULL)
      m_ppSolution->WriteXML(writeFormat, progress);
   writeFormat.popHeader();

   writeFormat.writef("</DFTSolution>\n");
}

int CDFTSolution::LoadXML(CXMLNode *node, CCEtoODBDoc *doc, FileStruct *file)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "DFTSolution")
      return 0;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (subNode->GetName(nodeName))
      {
         if (nodeName == "DFTTestplan")
         {
            m_TestPlan.LoadXML(subNode, file, doc->getSettings().getPageUnits());
         }
         else if (nodeName == "AASolution")
         {
            if (m_aaSolution == NULL)
               m_aaSolution = new CAccessAnalysisSolution(doc, file, &m_TestPlan);
            m_aaSolution->LoadXML(subNode);
         }
         else if (nodeName == "PPSolution")
         {
            if (m_ppSolution == NULL)
               m_ppSolution = new CProbePlacementSolution(doc, &m_TestPlan);
            m_ppSolution->LoadXML(subNode);
         }
      }

      delete subNode;
   }

   return 0;
}

void CDFTSolution::Scale(double factor)
{
   m_TestPlan.Scale(factor);
}


/////////////////////////////////////////////////////////////////////////////
// CDFTSolution
/////////////////////////////////////////////////////////////////////////////
CDFTSolution* CDFTSolutionList::AddNewDFTSolution(CString name, bool isFlipped, PageUnitsTag pageUnits)
{
   CDFTSolution *newSolution = FindDFTSolution(name, isFlipped);
   if (newSolution == NULL)
   {
      newSolution = new CDFTSolution(name, isFlipped, pageUnits);
      this->AddTail(newSolution);
   }

   return newSolution;
}

CDFTSolution* CDFTSolutionList::FindDFTSolution(CString name, bool isFlipped)
{
   POSITION pos = this->GetHeadPosition();
   while (pos)
   {
      CDFTSolution* dftSolution = this->GetNext(pos);
      if (dftSolution == NULL)
         continue;

      if (dftSolution->GetName().CompareNoCase(name) == 0 && dftSolution->IsFlipped() == isFlipped)
         return dftSolution;
   }

   return NULL;
}

void CDFTSolutionList::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   if (GetCount() <= 0)
      return;

   writeFormat.writef("<DFTSolutions");
   if (GetProbeNumberOffset() >= 0)
   {
      writeFormat.writef(" probeNumberOffset=\"%d\"", GetProbeNumberOffset());
   }
   writeFormat.writef(">\n");

   writeFormat.pushHeader("  ");
   POSITION pos = GetHeadPosition();
   while (pos)
   {
      CDFTSolution *pSolution = GetNext(pos);

      pSolution->WriteXML(writeFormat, progress); 
   }
   writeFormat.popHeader();

   writeFormat.writef("</DFTSolutions>\n");
}

int CDFTSolutionList::LoadXML(CString xmlString, CCEtoODBDoc *doc, FileStruct *file)
{
   CXMLDocument xmlDoc;

   if (xmlDoc.LoadXML(xmlString))
   {
      //------
      // Probably later, when we start doing the probe templates and therefore want DftTestPlan support
      // we'll move this value into the test plan in some way. Or maybe even make a new panel level settings
      // container within DftSolutions. But for now, maybe forever, we're just putting the probe number offset
      // in the DftSolutions "header" line.
      CXMLNodeList *nodeListP = xmlDoc.GetElementsByTagName("DFTSolutions");
      if (nodeListP != NULL)
      {
         nodeListP->Reset();
         CXMLNode *node = NULL;
         while (node = nodeListP->NextNode())
         {
            CString probeNumberOffsetStr;
            if (node->GetAttrValue("probeNumberOffset", probeNumberOffsetStr))
            {
               int probeNumberOffset = atoi(probeNumberOffsetStr);
               this->SetProbeNumberOffset(probeNumberOffset);
            }
         }
         delete nodeListP;
      }
      //------


      CXMLNodeList *nodeList = xmlDoc.GetElementsByTagName("DFTSolution");
      if (nodeList == NULL)
         return 0;      // no solutions

      nodeList->Reset();
      CXMLNode *node = NULL;
      while (node = nodeList->NextNode())
      {
         LoadXML_OneDftSolution(node, doc, file);
         delete node;
      }

      delete nodeList;

      // Return number of solutions
      return this->GetSize();
   }

   return -1; // XML Load Error
}

int CDFTSolutionList::LoadXMLFile(CString xmlFileName, CCEtoODBDoc *doc, FileStruct *file)
{
   CXMLDocument xmlDoc;

   if (xmlDoc.LoadXMLFile(xmlFileName))
   {
      //------
      // Probably later, when we start doing the probe templates and therefore want DftTestPlan support
      // we'll move this value into the test plan in some way. Or maybe even make a new panel level settings
      // container within DftSolutions. But for now, maybe forever, we're just putting the probe number offset
      // in the DftSolutions "header" line.
      CXMLNodeList *nodeListP = xmlDoc.GetElementsByTagName("DFTSolutions");
      if (nodeListP != NULL)
      {
         nodeListP->Reset();
         CXMLNode *node = NULL;
         while (node = nodeListP->NextNode())
         {
            CString probeNumberOffsetStr;
            if (node->GetAttrValue("probeNumberOffset", probeNumberOffsetStr))
            {
               int probeNumberOffset = atoi(probeNumberOffsetStr);
               this->SetProbeNumberOffset(probeNumberOffset);
            }
         }
         delete nodeListP;
      }
      //------

      CXMLNodeList *nodeList = xmlDoc.GetElementsByTagName("DFTSolution");
      if (nodeList == NULL)
         return 0;      // no solutions

      nodeList->Reset();
      CXMLNode *node = NULL;
      while (node = nodeList->NextNode())
      {
         LoadXML_OneDftSolution(node, doc, file);
         delete node;
      }

      delete nodeList;

      // Return number of solutions
      return this->GetSize();
   }

   return -1; // XML Load Error
}

int CDFTSolutionList::LoadXML_OneDftSolution(CXMLNode *node, CCEtoODBDoc *doc, FileStruct *file)
{
   if (node != NULL)
   {
      CString solutionName;
      if (node->GetAttrValue("name", solutionName))
      {
         // Page units default to match alread loaded doc.
         PageUnitsTag pageUnits = doc->getSettings().getPageUnits();

         CString pageUnitsStr;
         if (node->GetAttrValue("pageUnits", pageUnitsStr))
         {
            PageUnitsTag trialTag = intToPageUnitsTag(atoi(pageUnitsStr));
            if (trialTag != pageUnitsUndefined)
               pageUnits = trialTag;
         }

         // Flipped defaults to not flipped.
         bool isFlipped = false;
         CString flippedStr;
         if (node->GetAttrValue("flipped", flippedStr))
         {
            isFlipped = flippedStr.CompareNoCase("true") == 0 || flippedStr.CompareNoCase("yes") == 0;
         }

         CDFTSolution *curSol = new CDFTSolution(solutionName, isFlipped, pageUnits);
         AddTail(curSol);
         curSol->LoadXML(node, doc, file);
      }
   }

   // Return number of solutions
   return this->GetSize();

}

void CDFTSolutionList::Scale(double factor)
{
   POSITION dftSolutionPos = this->GetHeadPosition();
   while (dftSolutionPos)
   {
      CDFTSolution *dftSolution = this->GetNext(dftSolutionPos);
      if (dftSolution == NULL)
         continue;
      
      dftSolution->Scale(factor);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CDFTSolution
/////////////////////////////////////////////////////////////////////////////
CAccessAnalysisSolution *CDFTSolution::CreateAccessAnalysisSolution(CCEtoODBDoc *doc, FileStruct *PCBFile)
{
   if (m_aaSolution != NULL)
      return m_aaSolution;

   m_aaSolution = new CAccessAnalysisSolution(doc, PCBFile, &m_TestPlan);

   return m_aaSolution;
}

void CDFTSolution::CopyDFTSolution(const CDFTSolution& copyFromSolution)
{
   this->m_flipped = copyFromSolution.m_flipped;

   if (m_aaSolution != NULL)
   {
      delete m_aaSolution;
      m_aaSolution = NULL;
   }
   if (m_ppSolution != NULL)
   {
      delete m_ppSolution;
      m_ppSolution = NULL;
   }

   m_TestPlan = copyFromSolution.m_TestPlan;
   if (copyFromSolution.m_aaSolution)
      m_aaSolution = new CAccessAnalysisSolution(*copyFromSolution.m_aaSolution, &m_TestPlan);
   if (copyFromSolution.m_ppSolution)
      m_ppSolution = new CProbePlacementSolution(*copyFromSolution.m_ppSolution, &m_TestPlan);
}

void CDFTSolution::dumpAAToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Access Analysis Solution::\n", indent, " ");
   if (m_aaSolution!= NULL)
   {
      indent += 3;
      m_aaSolution->DumpToFile(file, indent);
      indent -= 3;
   }
}

/////////////////////////////////////////////////////////////////////////////
// CTestProbeData
/////////////////////////////////////////////////////////////////////////////

CTestProbeData::CTestProbeData(CString refname, CString netname, ETestSurface   pcbside, double x, double y)
: m_pinType(TestProbePinTypeVia)
, m_refname(refname)
, m_netname(netname)
, m_pcbside(pcbside)
, m_x(x) 
, m_y(y)
{
}

CTestProbeData::CTestProbeData(CString refname, CString netname, ETestSurface   pcbside, CString compname, CString pinname, 
										 double x, double y)
: m_pinType(TestProbePinTypeComppin)
, m_refname(refname)
, m_netname(netname)
, m_pcbside(pcbside)
, m_compname(compname)
, m_pinname(pinname)
, m_x(x) 
, m_y(y)
{
}

/////////////////////////////////////////////////////////////////////////////
// CTestProbeDataList
/////////////////////////////////////////////////////////////////////////////
CTestProbeDataList::CTestProbeDataList()
{
   this->empty();
   m_threadshold = 0.005;
}

void CTestProbeDataList::ClearAllData()
{
   RemoveTestData();
   RemoveViaList();
}

void CTestProbeDataList::RemoveTestData()
{
   int size = this->GetCount();
   POSITION pos =this->GetHeadPosition();

   while(pos)
   {
      CTestProbeData *testprobedata= this->GetNext(pos);
      if(testprobedata)
         delete testprobedata;      
   }
   this->RemoveAll();
}

void CTestProbeDataList::RemoveViaList()
{
   POSITION mappos = m_ViaListMap.GetStartPosition();
   while(mappos)
   {
      CString netname;
      CViaList* vialist = NULL;
      m_ViaListMap.GetNextAssoc(mappos,netname,vialist);
      if(vialist)
        delete vialist;
   }
   m_ViaListMap.RemoveAll();
}

CTestProbeDataList::~CTestProbeDataList()
{
   ClearAllData();
}

void CTestProbeDataList::Add(CString refname, CString netname, ETestSurface   pcbside, double x, double y)
{
   CTestProbeData *probetestpin = new CTestProbeData(refname, netname, pcbside, x, y);
   this->AddTail(probetestpin);
}

void CTestProbeDataList::Add(CString refname, CString netname, ETestSurface   pcbside, CString compname, CString pinname, 
									  double x, double y)
{
   CTestProbeData *probetestpin = new CTestProbeData(refname, netname, pcbside, compname, pinname, x, y);
   this->AddTail(probetestpin);
}

void CTestProbeDataList::Add(TTestPoints &testpoint)
{
   CTestProbeData *probetestpin = new CTestProbeData(testpoint.refname, testpoint.netname, testpoint.pcbside, testpoint.x, testpoint.y);
   this->AddTail(probetestpin);
}

DataStruct *CTestProbeDataList::findViaAt(FileStruct *pcbfile, CPoint2d location)
{

   if (pcbfile != NULL)
   {
      BlockStruct *pcbBlock = pcbfile->getBlock();

      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         if (data->isInsertType(insertTypeVia))
         {
            InsertStruct *via = data->getInsert();
            CPoint2d viaLocation = via->getOrigin2d();
            if (location.distance(viaLocation) < m_threadshold)           
               return data;
         }
      }
   }
   return NULL;
}

DataStruct *CTestProbeDataList::findViaAt(CCEtoODBDoc *doc, FileStruct *pcbfile, CTestProbeData *testprobedata)
{
   
   CPoint2d location(testprobedata->GetX(),testprobedata->GetY());
   bool isBottom = (testprobedata->GetPcbside() == testSurfaceBottom);

   if (pcbfile != NULL)
   {
      BlockStruct *pcbBlock = pcbfile->getBlock();

      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         if (data->isInsertType(insertTypeVia))
         {

            InsertStruct *via = data->getInsert();
            if (via && location.distance(via->getOrigin2d()) < m_threadshold)
            {
               Attrib *attrib = NULL;
               CString netname = "";
               
               WORD netnameKW = doc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);
               if(data->getAttributesRef()->Lookup(netnameKW, attrib))
                  netname = attrib->getStringValue();

               if(!netname.Compare(testprobedata->GetNetname()))
                  return data;
            }/*if*/
         }/*if*/
      }/*for*/
   }
   return NULL;
}

DataStruct *CTestProbeDataList::findViabyMapAt(FileStruct *pcbfile, CTestProbeData *testprobedata)
{
  
   CPoint2d location(testprobedata->GetX(),testprobedata->GetY());
   bool isBottom = (testprobedata->GetPcbside() == testSurfaceBottom);

   if (pcbfile == NULL)
      return NULL;

   CViaList* vialist = NULL;
   m_ViaListMap.Lookup(testprobedata->GetNetname(), vialist);
   if(vialist)
   {
      POSITION pos = vialist->GetHeadPosition();
      while(pos)
      {
         DataStruct* data = vialist->GetNext(pos);
         if(data)
         {
            InsertStruct *via = data->getInsert();
            if (via && location.distance(via->getOrigin2d()) < m_threadshold)
                  return data;

         }/*if*/
      }/*while*/
   }/*if*/

   return NULL;
}

CompPinStruct *CTestProbeDataList::findCompPinAt(FileStruct *pcbfile, CTestProbeData *testprobedata)
{

   CPoint2d location(testprobedata->GetX(),testprobedata->GetY());
   bool isBottom = (testprobedata->GetPcbside() == testSurfaceBottom);
   
   if (pcbfile == NULL)
      return NULL;

   NetStruct *net = pcbfile->getNetList().getNet(testprobedata->GetNetname());
   if(!net)
      return NULL;

   POSITION pos = net->getCompPinList().getHeadPosition();
   while(pos)
   {
      CompPinStruct *comppin = net->getCompPinList().getNext(pos);
      if(comppin)
      {
         CPoint2d comppinLocation = comppin->getOrigin();
         if (location.distance(comppinLocation) < m_threadshold)  
            return comppin;
      }
   }
   return NULL;
}

void CTestProbeDataList::BuildViaListMap(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   if (pcbfile != NULL)
   {
      BlockStruct *pcbBlock = pcbfile->getBlock();

      WORD netnameKW = doc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);
      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         if (data->isInsertType(insertTypeVia))
         {
            Attrib *attrib = NULL;
            CString netname = "";
            
            if(data->getAttributesRef()->Lookup(netnameKW, attrib))
               netname = attrib->getStringValue();
            
            CViaList *vialist = NULL;
            if(!netname.IsEmpty() && !m_ViaListMap.Lookup(netname,vialist))
            {
               vialist = new CViaList();
               m_ViaListMap.SetAt(netname,vialist);
            }
            if(vialist)
               vialist->AddTail(data);
            
         }/*if*/
      }/*for*/
   }/*if*/
}

void CTestProbeDataList::MarkProbeTestPointsInCamcad(CCEtoODBDoc *doc, FileStruct *pcbFile, CStringArray *logMessages)
{
   CString errormsg = "";

   POSITION pos = this->GetHeadPosition();
   while (pos != NULL)
   {
      CTestProbeData *testprobedata= this->GetNext(pos);
      DataStruct	*via = NULL;
      CompPinStruct	*comppin = NULL;
      double x = testprobedata->GetX();
      double y = testprobedata->GetY();
      long pinEntityNum = -1;
      double rot = 0.0;
      bool isbottom = false;

      if(testprobedata->GetTestPinType() == TestProbePinTypeVia)
      {
         //find via
         CPoint2d location(testprobedata->GetX(),testprobedata->GetY());
         via = this->findViaAt(pcbFile,location);
         if(via == NULL)
         {
            errormsg.Format("%s(%d) Error: target VIA not found!\n",__FUNCTION__,__LINE__);
            logMessages->Add(errormsg);
         }
         else
         {
            x = via->getInsert()->getOriginX();
            y = via->getInsert()->getOriginY();
            rot = via->getInsert()->getAngleDegrees();
            pinEntityNum = via->getEntityNumber();
         }
      }
      else if(testprobedata->GetTestPinType() == TestProbePinTypeComppin)
      {
         //find comppin
         NetStruct *net = NULL;
         comppin = FindCompPin(pcbFile,testprobedata->GetCompname(),	testprobedata->GetPinname(), &net);
         if(comppin == NULL)
         {
            errormsg.Format("%s(%d) Error: target CompPin not found!\n",__FUNCTION__,__LINE__);
            logMessages->Add(errormsg);
         }
         else
         {
            x = comppin->getOriginX();
            y = comppin->getOriginY();
            rot = comppin->getRotationDegrees();
            pinEntityNum = comppin->getEntityNumber();
         }
      }
      PlaceAccessAndProbe(50, doc, pcbFile, x, y, rot, pinEntityNum, 
         testprobedata->GetRefname(),testprobedata->GetNetname(),testprobedata->GetPcbside());
   }
}

void CTestProbeDataList::MarkViaAndComPinProbeTest(CCEtoODBDoc *doc, FileStruct *pcbFile, CStringArray *logMessages)
{
   CString errormsg = "";

   POSITION pos = this->GetHeadPosition();
   while (pos != NULL)
   {
      double x = 0.;
      double y = 0.;
      long pinEntityNum = -1;
      double rot = 0.0;
      
      CTestProbeData *testprobedata= this->GetNext(pos);
      if(testprobedata)
      {
         CompPinStruct	*comppin = NULL;
         comppin = this->findCompPinAt(pcbFile,testprobedata);
         if(comppin)
         {
            x = comppin->getOriginX();
            y = comppin->getOriginY();
            rot = comppin->getRotationDegrees();
            pinEntityNum = comppin->getEntityNumber();
         }
         else
         {
            DataStruct	*via = NULL;
            if(m_ViaListMap.GetCount())
               via = this->findViabyMapAt(pcbFile,testprobedata);
            else
               via = this->findViaAt(doc,pcbFile,testprobedata);

            if(via)
            {
               x = via->getInsert()->getOriginX();
               y = via->getInsert()->getOriginY();
               rot = via->getInsert()->getAngleDegrees();
               pinEntityNum = via->getEntityNumber();
            }
            else
            {
               errormsg.Format("%s(%d) Error: target Via/CompPin not found!\n",__FUNCTION__,__LINE__);
               logMessages->Add(errormsg);
            }
         }
      }      
      PlaceAccessAndProbe(50, doc, pcbFile, x, y, rot, pinEntityNum, 
         testprobedata->GetRefname(),testprobedata->GetNetname(),testprobedata->GetPcbside());
   }

}

/////////////////////////////////////////////////////////////////////////////
// CProbeTypeConversion
/////////////////////////////////////////////////////////////////////////////
CProbeTypeConversion::CProbeTypeConversion(CString probeName, CCEtoODBDoc *doc)
: m_probeName(probeName)
{
   m_unitFactor = Units_Factor(UNIT_MILS, doc->getPageUnits());
   m_ProbeSize = getProbeType(probeName);   
   m_PCBSide = getPCBSide(probeName);  
}

CProbeTypeConversion::CProbeTypeConversion(CCEtoODBDoc *doc)
{
   m_unitFactor = Units_Factor(UNIT_MILS, doc->getPageUnits());
}

double CProbeTypeConversion::getProbeDrillSize(ProbeTypeTag probeType)
{
   int ProbeDrillSize[] = {37, 55, 67};
  
   return ((probeType > ProbeTypeTag_UNKNOWN && probeType < ProbeTypeTag_Max)?(ProbeDrillSize[probeType] * m_unitFactor):0);
}

double CProbeTypeConversion::getDiamSize(ProbeTypeTag probeType)
{
   int ProbleDiamSize[] = {49, 70, 90};
  
   return ((probeType > ProbeTypeTag_UNKNOWN && probeType < ProbeTypeTag_Max)?(ProbleDiamSize[probeType] * m_unitFactor):0);
}

ProbeTypeTag CProbeTypeConversion::getProbeType(CString probeName)
{
    char *ProbeType[] = {"50", "75", "100"};

    ProbeTypeTag ProbeSize = ProbeTypeTag_UNKNOWN;
    for(int index = ProbeTypeTag_50 ; index < ProbeTypeTag_Max; index++)
    {
      if(probeName.Find(ProbeType[index]) > -1)
      {
         ProbeSize = (ProbeTypeTag)index;
         break;
      }
    }

    return ProbeSize;
}

ETestSurface CProbeTypeConversion::getPCBSide(CString probeName)
{
   char *PCBSide[] = {"TOP","BOT"};
   
   ETestSurface PCBSurface = testSurfaceUnset;
   for(int index = 0; index < sizeof(PCBSide)/sizeof(char*); index++)
   {
      if(probeName.Find(PCBSide[index]) > -1)
      {
         PCBSurface = (!index)?testSurfaceTop:testSurfaceBottom;
         break;
      }
   }  

   return PCBSurface;
}


void CProbePlacementSolution::ApplyToPcb(FileStruct *pcbFile)
{
   POSITION netResPos = this->m_netResultList.GetHeadPosition();
   while (netResPos != NULL)
   {
      CPPNetResult *netResult = this->m_netResultList.GetNext(netResPos);

      POSITION probePos = netResult->GetProbeHeadPosition();
      while (probePos != NULL)
      {
         CPPProbeResult *probeResult = netResult->GetProbeNext(probePos);

         int accessEntityNum = probeResult->GetAccessEntityNum();
         CString accessName = probeResult->GetAccessName();

         DataStruct *accessData = pcbFile->FindInsertData(accessName);

         if (accessData == NULL)
         {
            // Log error, could not find the access that is referenced in the probe.
            //*rcf Could also check if the net itself exists.
            CString msg;
            msg.Format("Access [%s] on net [%s] referenced by probe [%s] not found in CAD data. Probe changed to Unplaced.\n", 
               accessName, probeResult->GetNetName(), probeResult->GetProbeRefname());
            getApp().LogMessage(msg);
         }

         // Place the probe. Might actually be placing an unplaced probe, that is okay.
         // Used to be an else, but actually we always want the probe.
         // The NULL access mark just makes it unplaced.
         {
            CString probeRefName = probeResult->GetProbeRefname();
            double x = probeResult->GetLocation().x;
            double y = probeResult->GetLocation().y;
            double rot = 0.0;
            ETestSurface surface = (accessData != NULL) ? (accessData->getInsert()->getPlacedTop() ? testSurfaceTop : testSurfaceBottom) : (testSurfaceTop);
            ETestResourceType testResourceType = probeResult->GetResourceType();
            CString netName = probeResult->GetNetName();
            CString probeBaseName = probeResult->GetProbeTemplateName();

            //*rcf BUG DFT_CR diameters need work, these are not in probe solution, then are over in probe templates. Either need to add diameters or the templates
            double probediamInch = 0.065;
            double drillsizeInch = 0.050; //*rcf BUG BUG BUG Need to convey sizes

            DataStruct *probeData = PlaceTestProbe(m_pDoc,  pcbFile->getBlock(), 
               probeRefName, x, y, rot, surface, testResourceType,
               netName, accessData,
               probeBaseName, probediamInch, drillsizeInch);

         }
      }

   }

}

//========================================================================================

CSuperBoard::CSuperBoard(CCEtoODBDoc &doc, FileStruct *panelFile)
: m_doc(doc)
, m_camcadData(doc.getCamCadData())
, m_panelFile(panelFile)
, m_superBoard(NULL)
, m_pcbInstancePrefix("B")
, m_addedProbeCount(0)
, m_activeProbeNumberOffset(0)
, m_userProbeNumberOffset(0)
, m_highestOriginalProbeNumber(0)
, m_highestProbePlacedSoFar(0)
, m_probeOffsetTooSmall(false)
{
   if (m_panelFile != NULL)
   {
      // Get the probe number offset.
      FileStruct *dftSlnFile = m_camcadData.getFileList().FindByName(DFT_SOLUTION_XML_FILESTRUCT_NAME);
      if (dftSlnFile != NULL)
      {
         //CDFTSolutionList &dftSlnList = m_doc.getDFTSolutions(*dftSlnFile);
         //m_userProbeNumberOffset = dftSlnList.GetProbeNumberOffset();
      }

      // Create the panel's basic superboard.
      m_superBoard = m_camcadData.getFileList().addNewFile("SuperBoard", fileTypeCamcadUser);
		m_superBoard->setBlockType(blockTypePcb);
		m_superBoard->setShow(false);

      // Create an empty DftSolution for the superboard. Will populate as we go.
      /*  WIP Not working yet.
      m_superBoardDftSln = m_doc.AddNewDFTSolution(*m_superBoard, m_superBoard->getName(), false, m_doc.getPageUnits());
      m_superBoardDftSln->CreateAccessAnalysisSolution(&m_doc, m_superBoard);
      m_superBoardDftSln->CreateProbePlacementSolution(&m_doc);
      */

      // Find highest probe number in base PCAs.
      DetermineHighestOriginalProbeNumber();

      // Put panel level items in superboard. Traces, vias, text, panel outline, etc.
      // Everythng EXCEPT the PCB inserts.
      CopyNonPcbInsertData();

      // Put each PCB's stuff into the superboard.
      FlattenAllPcbs();

      // Adjust panel and board outline graphic classes. Board->Normal, Panel->Board.
      AdjustOutlines();

      // Reset superboard extents.
      bool checkOnlyVisibleEntities = false; // File is not visible now, so check all, not just visible because nothing is visible.
      m_superBoard->getBlock()->calculateBlockExtents(m_camcadData, checkOnlyVisibleEntities);
   }
}

void CSuperBoard::CopyNonPcbInsertData()
{
   // Copy all the regular panel stuff from panelFile to superBoard.
   // Everything except copies of PCB inserts.

   if (m_panelFile != NULL)
   {
      BlockStruct *panelBlock = m_panelFile->getBlock();
      if (panelBlock != NULL)
      {
         POSITION panelDataPos = panelBlock->getHeadDataPosition();
         while (panelDataPos != NULL)
         {
            DataStruct *dataInPanel = panelBlock->getNextData(panelDataPos);
            if (dataInPanel != NULL && !dataInPanel->isInsertType(insertTypePcb))
            {
               DataStruct *dataCopy = m_camcadData.getNewDataStruct(*dataInPanel, true);
               m_superBoard->getBlock()->getDataList().AddTail(dataCopy);
            }
         }
      }
   }
}

void CSuperBoard::FillPcbSequenceArray(BlockStruct *panelBlock, PcbSequenceArray &pcbArray)
{
   // PCB Inserts in panel's data list are not reliabley in Step & Repeat order.
   // So fill this array with their DataStruct ptr's in an order that matches
   // the PCB Sequence ID assignments.
   // Do it as an insertion sort.

   pcbArray.RemoveAll();

   if (panelBlock != NULL)
   {
      WORD pcaSeqIdKW = m_camcadData.getDefinedAttributeKeywordIndex("PCA_Sequence_ID", valueTypeInteger);

      PcbSequenceArray lostSouls; // Temp storage of PCBs that do not have the ordering attrib.

      POSITION panelDataPos = panelBlock->getHeadDataPosition();
      while (panelDataPos != NULL)
      {
         DataStruct *dataInPanel = panelBlock->getNextData(panelDataPos);
         if (dataInPanel != NULL && dataInPanel->isInsertType(insertTypePcb))
         {
            int orderId = -99;
            Attrib *attrib;
            if (dataInPanel->lookUpAttrib(pcaSeqIdKW, attrib))
               orderId = attrib->getIntValue();

            int debugSize = pcbArray.GetSize();
            int debugCount = pcbArray.GetCount();

            // Valid ordering starts at 1.
            
            if (orderId > 0)
            {
               bool added = false;
               for (int i = 0; i < pcbArray.GetSize() && !added; i++)
               {
                  DataStruct *existingData = pcbArray.GetAt(i);
                  Attrib *existingAttrib;
                  existingData->lookUpAttrib(pcaSeqIdKW, existingAttrib);
                  int existingOrderId = existingAttrib->getIntValue();

                  if (orderId < existingOrderId)
                  {
                     pcbArray.InsertAt(i, dataInPanel); // insert in order
                     added = true;
                  }
               }
               if (!added)
                  pcbArray.Add(dataInPanel); // to the end
            }
            else
            {
               lostSouls.Add(dataInPanel);
            }
         }
      }

      // Now add any lost souls to the end of the array.
      for (int i = 0; i < lostSouls.GetCount(); i++)
         pcbArray.Add( lostSouls.GetAt(i) );
   }
}

void CSuperBoard::FlattenAllPcbs()
{
   if (m_panelFile != NULL)
   {
      BlockStruct *panelBlock = m_panelFile->getBlock();
      if (panelBlock != NULL)
      {
         PcbSequenceArray sortedPcbArray;
         FillPcbSequenceArray(panelBlock, sortedPcbArray);

         int pcbCount = 0;
         for (int i = 0; i < sortedPcbArray.GetCount(); i++)
         {
            DataStruct *dataInPanel = sortedPcbArray.GetAt(i);
            if (dataInPanel != NULL && dataInPanel->isInsertType(insertTypePcb)) // jic
            {
               InsertStruct *pcbInsert = dataInPanel->getInsert();
               FlattenOnePcb(pcbInsert, pcbCount);
            }
         }
      }
   }
}


void CSuperBoard::FlattenOnePcb(InsertStruct *pcbInsert, int &pcbCount)
{
   BlockStruct *pcbBlock = (pcbInsert != NULL) ? (this->m_camcadData.getBlockAt(pcbInsert->getBlockNumber())) : (NULL);

   // Update net names on all items with netname attrib.
   // Update net names in netlist.
   // Update comp refnames in comppins.
   // Update comppin locations.

   if (pcbBlock != NULL)
   {
      pcbCount++;

      CString msg;
      BlockStruct *pcbBlock = this->m_camcadData.getBlockAt(pcbInsert->getBlockNumber());
      msg.Format("SB:Flattening PCB %d [%s] insert of [%s]\n", pcbCount, pcbInsert->getRefname(), pcbBlock?pcbBlock->getName():"no block");
      getApp().LogMessage(msg);

      CEntityNumberMap entityNumberMap;

      // All data except Access Marks and Test Probes.
      CopyData(pcbCount, pcbInsert, entityNumberMap);

      // Netlist
      CopyNetlist(pcbCount, pcbBlock, entityNumberMap);

      // Access and Probes go last, we need all the new entity numbers for
      // vias and comppins before this step.
      this->m_activeProbeNumberOffset = this->CalcProbeNumberOffset(pcbCount, pcbBlock);
      CopyAccessAndProbes(pcbCount, pcbInsert, entityNumberMap);

      // Update CompPin Locations. Do it silently, no log file, no progress bar.
      int errorCount = m_camcadData.generatePinLocations(NULL, NULL);
      if (errorCount > 0)
      {
         CString msg;
         msg.Format("Regeneration of CompPin locations encountered %d error%s.\n", errorCount, errorCount > 1 ? "s" : "");
         getApp().LogMessage(msg);
      }

   }
}

int CSuperBoard::CalcProbeNumberOffset(int pcaSeqId, BlockStruct *pcbBlock)
{
   // This is assuming the probes in base pca are contiguously numbered and start at 1.
   // Yeah, I know, that is a really bad assumption. We'll get fancier later. Like make a table
   // of probe number ranges per PCA. But for now...
   // Probably should do something to not only consider the count but also the actual numbers.
   // We are at least considering the highest probe number in the PCAs rather than just
   // assume count matches highest probe number.

   int offset = 0;

   // Offset only matters for 2nd to nth PCA
   if (pcaSeqId > 1)
   {
      if (this->m_userProbeNumberOffset == 0)
      {
         offset = 0; // No offset, use probe numbers as-is.
      }
      else if (this->m_userProbeNumberOffset == 1)
      {
         offset = this->m_highestProbePlacedSoFar;  // Contiguous sequential, basic offset is number of probes placed so far.
         // If we go with straight numeric offset of repeats, given a base with two probes, 3 and 5, we'd get this:
         // (b1  (b2)  (b3)   (b4)
         // 3,5, 8,10, 13,15, 18,20
         // Mark wants gaps squeezed out of the beginning of the repeated image.
         // This has an effect only when the lowest probe number in PCB about to be repeated is not 1.
         // So given probes 3 and 5 in base PCA solution, we get this:
         // (b1  (b2) (b3)  (b4)
         // 3,5, 6,8, 9,11, 12,14
         if (pcbBlock != NULL)
         {
            int lowestProbeNumberInThisPcb = this->GetLowestProbeNumber(pcbBlock);
            if (lowestProbeNumberInThisPcb != 1)
               offset -= (lowestProbeNumberInThisPcb - 1);
         }

      }
      else
      {
         // Basic offset
         offset = this->m_userProbeNumberOffset;

         // This works great, and has feature that it compensates for too small of a user offset setting by
         // repeatedly applying the offset until it is a good number. But Mark actually doesn't like this, he
         // wants it to just be an error if offset is too small.
         //
         // Increment offset by user setting until it is greater than or equal to highest probe number in use so far.
         while (offset < m_highestProbePlacedSoFar)
            offset += this->m_userProbeNumberOffset;

         // Factor offset by PCA sequence number. Straight numeric offset.
         int altOffset = (pcaSeqId - 1) * this->m_userProbeNumberOffset;
         //*rcf debug check
         if (altOffset != offset)
         {
            int jj = 0;
         }
         if (altOffset < m_highestProbePlacedSoFar)
         {
            int jj = 0;  // This would be Mark's offset setting error condition.

            // Flag a probe offset error in the superboard.
            m_probeOffsetTooSmall = true;
         }
      }
   }

   return offset;
}

class CMapInt : public CMap<int, int, int, int>  {};

void CSuperBoard::DetermineHighestOriginalProbeNumber()
{
   // Find the highest probe number in original PCAs.

   m_highestOriginalProbeNumber = 0;

   if (m_panelFile != NULL)
   {
      // Don't need to process same block more than once, so keep track in this map.
      CMapInt processedBlockNumberMap;

      BlockStruct *panelBlock = m_panelFile->getBlock();
      if (panelBlock != NULL)
      {
         POSITION panelDataPos = panelBlock->getHeadDataPosition();
         while (panelDataPos != NULL)
         {
            DataStruct *dataInPanel = panelBlock->getNextData(panelDataPos);
            if (dataInPanel != NULL && dataInPanel->isInsertType(insertTypePcb))
            {
               BlockStruct *pcbBlock = m_camcadData.getBlockAt( dataInPanel->getInsert()->getBlockNumber() );
               int notUsed;
               if (pcbBlock != NULL && !processedBlockNumberMap.Lookup(pcbBlock->getBlockNumber(), notUsed))
               {
                  processedBlockNumberMap.SetAt(pcbBlock->getBlockNumber(), 0);

                  int probeNumber = GetHighestProbeNumber(pcbBlock);
                  if (probeNumber > m_highestOriginalProbeNumber)
                  {
                     m_highestOriginalProbeNumber = probeNumber;
                  }

               }
            }
         }
      }
   }
}

int CSuperBoard::GetLowestProbeNumber(BlockStruct *pcbBlock)
{
   int lowestProbeNumber = 0;

   if (pcbBlock != NULL)
   {
      bool lookingForFirstProbe = true;

      POSITION pcbDataPos = pcbBlock->getHeadDataInsertPosition();
      while (pcbDataPos != NULL)
      {
         DataStruct *dataInPcb = pcbBlock->getNextDataInsert(pcbDataPos);
         if (dataInPcb != NULL && dataInPcb->isInsertType(insertTypeTestProbe))
         {
            CString refname( dataInPcb->getInsert()->getRefname() );
            int probeNumber = atoi(refname);

            if (lookingForFirstProbe)
            {
               lowestProbeNumber = probeNumber;
               lookingForFirstProbe = false;
            }
            else if (probeNumber < lowestProbeNumber)
            {
               lowestProbeNumber = probeNumber;
            }
         }
      }
   }

   return lowestProbeNumber;
}

int CSuperBoard::GetHighestProbeNumber(BlockStruct *pcbBlock)
{
   int highestProbeNumber = 0;

   if (pcbBlock != NULL)
   {
      bool lookingForFirstProbe = true;

      POSITION pcbDataPos = pcbBlock->getHeadDataInsertPosition();
      while (pcbDataPos != NULL)
      {
         DataStruct *dataInPcb = pcbBlock->getNextDataInsert(pcbDataPos);
         if (dataInPcb != NULL && dataInPcb->isInsertType(insertTypeTestProbe))
         {
            CString refname( dataInPcb->getInsert()->getRefname() );
            int probeNumber = atoi(refname);

            if (lookingForFirstProbe)
            {
               highestProbeNumber = probeNumber;
               lookingForFirstProbe = false;
            }
            else if (probeNumber > highestProbeNumber)
            {
               highestProbeNumber = probeNumber;
            }
         }
      }
   }

   return highestProbeNumber;
}

CString CSuperBoard::GetPcbInstancePrefix(int pcbNumber)
{
   CString z;
   z.Format("%s%d_", m_pcbInstancePrefix, pcbNumber);
   return z;
}

CString CSuperBoard::AdjustRefname(int pcbNum, CString originalRefname)
{
   // Originally just for refnames, this is works and is used for net names too.
   // This net name gets left alone. Is special name, prefixed version of this one does not work in CAMCAD.
   if (originalRefname.CompareNoCase(NET_UNUSED_PINS) == 0)
      return originalRefname;
   
   CString z;
   z.Format("%s%s",  GetPcbInstancePrefix(pcbNum), originalRefname);
   return z;
}

CString CSuperBoard::AdjustAttrib(int pcbNum, CAttributes *attributes, StandardAttributeTag attribTag)
{
   // Intended for standardAttributeNetName and standardAttributeRefName
   // but should work for any string value attribute.
   // Returns adjusted value.

   if (attributes != NULL)
   {
      Attrib *attrib = attributes->lookupAttribute(attribTag);
      if (attrib != NULL)
      {
         CString curVal = attrib->getStringValue();
         CString newVal = AdjustRefname(pcbNum, curVal);
         if (curVal.Compare(newVal) != 0)
            attributes->setAttribute(this->m_camcadData, attribTag, newVal);
         return newVal;
      }
   }

   return ""; // Nothing happened.
}

void CSuperBoard::AdjustDatalink(DataStruct *anyData, CEntityNumberMap &entityNumberMap)
{
   // DataStruct type does not matter, just process the datalink attrib on what ever it is.

   if (anyData != NULL && anyData->getAttributes() != NULL)
   {
      Attrib *attrib = anyData->getAttributes()->lookupAttribute(standardAttributeDataLink);
      if (attrib != NULL)
      {
         int oldDatalink = attrib->getIntValue();
         int newDatalink = -1;
         if (entityNumberMap.Lookup(oldDatalink, newDatalink))
         {
            anyData->getAttributes()->setAttribute(this->m_camcadData, standardAttributeDataLink, newDatalink);
         }
         else
         {
            // Error - This is serious, resulting superboard can't be right if this failed even once during process.
            CString msg;
            CString dataTypeStr = dataTypeTagToString( anyData->getDataType() );
            CString insertTypeStr;
            if (anyData->getDataType() == dataTypeInsert)
               insertTypeStr.Format("%s, %s, %s", 
               insertTypeToString(anyData->getInsert()->getInsertType()), 
               anyData->getInsert()->getPlacedTop() ? "Top" : "Bottom",
               anyData->getInsert()->getRefname());
            msg.Format("Entity mapping failure; Old entity number [%d] referenced by [%s][%s] not found in map.\n", oldDatalink, dataTypeStr, insertTypeStr);
            getApp().LogMessage(msg);
            anyData->getAttributes()->setAttribute(this->m_camcadData, standardAttributeDataLink, -1);
         }
      }
   }
}

void CSuperBoard::CopyData(int pcbNum, InsertStruct *pcbInsert, CEntityNumberMap &entityNumberMap)
{
   // Copy all except Access Marks and Test Probes.
   // Keep track of entity number changes on via only (to keep map smallish).
   // We only need things in map that access mark might reference.

   BlockStruct *pcbBlock = (pcbInsert != NULL) ? (this->m_camcadData.getBlockAt(pcbInsert->getBlockNumber())) : (NULL);
   if (pcbBlock == NULL)
      return;

   CTMatrix pcbInsertMatrix = pcbInsert->getTMatrix();

   bool pcbPlacedBottom = pcbInsert->getPlacedBottom();

   POSITION pcbDataPos = pcbBlock->getHeadDataPosition();
   while (pcbDataPos != NULL)
   {
      DataStruct *dataInPcb = pcbBlock->getNextData(pcbDataPos);
      if (dataInPcb != NULL && 
         !dataInPcb->isInsertType(insertTypeTestProbe) &&
         !dataInPcb->isInsertType(insertTypeTestAccessPoint))
      {
         DataStruct *dataCopy = m_camcadData.getNewDataStruct(*dataInPcb, true);

         if (pcbPlacedBottom && dataCopy->getInsert()->getAngleDegrees() != 0.)
            dataCopy->getInsert()->setAngleDegrees( -(dataCopy->getInsert()->getAngleDegrees()) );

         dataCopy->transform(pcbInsertMatrix, &m_camcadData, false);

         AdjustRefname(pcbNum, dataCopy);
         AdjustAttrib(pcbNum, dataCopy->getAttributes(), standardAttributeRefName);
         AdjustAttrib(pcbNum, dataCopy->getAttributes(), standardAttributeNetName);

         if (pcbPlacedBottom)
         {
            // Flip the layer in data if it is a mirrorable layer. Mostly this is for polylines and
            // text and such, but should do no harm to inserts.
            FlipMirrorLayers(pcbNum, dataCopy);

            // Flip the insert params for place bottom and mirror.
            if (dataCopy->isInsert())
            {
               dataCopy->getInsert()->setPlacedBottom(       !dataCopy->getInsert()->getPlacedBottom()    );
               dataCopy->getInsert()->setLayerMirrorFlag(    !dataCopy->getInsert()->getLayerMirrored()   );
               // Already done in dataCopy->transform - bad to do twice - dataCopy->getInsert()->setGraphicsMirrorFlag( !dataCopy->getInsert()->getGraphicMirrored() );
            }
         }

         entityNumberMap.SetAt(dataInPcb->getEntityNumber(), dataCopy->getEntityNumber());
         m_superBoard->getBlock()->getDataList().AddTail(dataCopy);
      }
   }
}

void CSuperBoard::FlipMirrorLayers(int pcbNum, DataStruct *data)
{
   int layerIndx = data->getLayerIndex();
   if (layerIndx != -1)
   {
      LayerStruct *layer = m_camcadData.getLayer(layerIndx);
      if (layer != NULL)
      {
         if (layer->getMirroredLayerIndex() != layerIndx)
            data->setLayerIndex(layer->getMirroredLayerIndex());
      }
   }
}

CString CSuperBoard::AdjustRefname(int pcbNum, DataStruct *insertData)
{
   // If we implement controls for how refname gets adjusted, it mainly means
   // changes here.

   if (insertData->getDataType() == dataTypeInsert)
   {
      CString originalRefname = insertData->getInsert()->getRefname();
      originalRefname.Trim();
      if (!originalRefname.IsEmpty())
      {
         insertData->getInsert()->setRefname(AdjustRefname(pcbNum, originalRefname));
         return insertData->getInsert()->getRefname();
      }
   }

   return ""; // Nothing happened.
}

void CSuperBoard::AdjustOutlines()
{
   // Adjust graphic class of panel and board outlines.
   // In superboard the original PanelOutline becomes the BoardOutline.
   // Pre-existing BoardOutlines become "normal" polylines.

   // This scheme assumes the "modern" convention of using graphic class to 
   // identify outline types. This does not consider the older style of
   // using an outline layer only. In general this might not be good since
   // it is somewhat incomplete, but in practice this is to support ODB data
   // from vPlan and in that case we reliable use the GCs. So using only the
   // GCs fully meets the vPlan needs, and that is all we need.

   // First change all existing gc Board Outline to gc Normal.
   AdjustOutlines(m_superBoard->getBlock(), graphicClassBoardOutline, graphicClassNormal);

   // Second change all existing graphic class Panel Outline to Board Outline.
   AdjustOutlines(m_superBoard->getBlock(), graphicClassPanelOutline, graphicClassBoardOutline);
}

void CSuperBoard::AdjustOutlines(BlockStruct *block, GraphicClassTag oldGc, GraphicClassTag newGc)
{
   // Adjust grapic class to newGc on any polys that have oldGc.
   if (block != NULL)
   {
      POSITION pos = block->getHeadDataPosition();
      while (pos != NULL)
      {
         DataStruct *data = block->getNextData(pos);
         switch (data->getDataType())
         {
         case dataTypePoly:
            if (data->getGraphicClass() == oldGc)
               data->setGraphicClass(newGc);
            break;

         case dataTypeInsert:
            AdjustOutlines( m_camcadData.getBlockAt(data->getInsert()->getBlockNumber()), oldGc, newGc);
            break;

         default:
            // Don't care about the rest.
            break;
         }
      }
   }
}

void CSuperBoard::CopyAccessAndProbes(int pcbNum, InsertStruct *pcbInsert, CEntityNumberMap &entityNumberMap)
{
   // Copy Access Marks and Test Probes.
   // Do Access Marks first, we need the entity number map entries for the probes.
   // Update DATALINK attrib as you go.

   BlockStruct *pcbBlock = (pcbInsert != NULL) ? (this->m_camcadData.getBlockAt(pcbInsert->getBlockNumber())) : (NULL);
   if (pcbBlock == NULL)
      return;

   CTMatrix pcbInsertMatrix = pcbInsert->getTMatrix();

   bool pcbPlacedBottom = pcbInsert->getPlacedBottom();

   // Access Marks
   POSITION pcbDataPos = pcbBlock->getHeadDataPosition();
   while (pcbDataPos != NULL)
   {
      DataStruct *dataInPcb = pcbBlock->getNextData(pcbDataPos);
      if (dataInPcb != NULL && dataInPcb->isInsertType(insertTypeTestAccessPoint))
      {
         DataStruct *dataCopy = m_camcadData.getNewDataStruct(*dataInPcb, true);
         if (pcbPlacedBottom && dataCopy->getInsert()->getAngleDegrees() != 0.)
            dataCopy->getInsert()->setAngleDegrees( -(dataCopy->getInsert()->getAngleDegrees()) );
         dataCopy->transform(pcbInsertMatrix, &m_camcadData, false);
         AdjustRefname(pcbNum, dataCopy);
         AdjustAttrib(pcbNum, dataCopy->getAttributes(), standardAttributeRefName);
         CString adjustedNetName = AdjustAttrib(pcbNum, dataCopy->getAttributes(), standardAttributeNetName);

         AdjustDatalink(dataCopy, entityNumberMap);

         if (pcbPlacedBottom)
         {
            // Flip the layer in data if it is a mirrorable layer. Mostly this is for polylines and
            // text and such, but should do no harm to inserts.
            FlipMirrorLayers(pcbNum, dataCopy);

            // Flip the insert params for place bottom and mirror.
            if (dataCopy->isInsert())
            {
               dataCopy->getInsert()->setPlacedBottom(       !dataCopy->getInsert()->getPlacedBottom()    );
               dataCopy->getInsert()->setLayerMirrorFlag(    !dataCopy->getInsert()->getLayerMirrored()   );
               // Already set by dataCopy->transform  - bad to do twice - dataCopy->getInsert()->setGraphicsMirrorFlag( !dataCopy->getInsert()->getGraphicMirrored() );
            }
         }

         entityNumberMap.SetAt(dataInPcb->getEntityNumber(), dataCopy->getEntityNumber());
         m_superBoard->getBlock()->getDataList().AddTail(dataCopy);

         //*rcf This might not be best here. This was fill-as-you-go attempt. The problem is
         // the feature entity is not in hand, would have to look it up to see what it is.
         // Probably better done in a post process pass that specifially extracts dft solution from all the inserts.
         /* Make a DftSolutuion for the SuperBoard.
         if (!adjustedNetName.IsEmpty())
         {
            ETestSurface testSurface = dataCopy->getInsert()->getPlacedTop() ? testSurfaceTop : testSurfaceBottom;
            CAANetAccess *netAccess = m_superBoardDftSln->GetAccessAnalysisSolution()->GetNetAccess(adjustedNetName);
            CAAAccessLocation *accLoc = new CAAAccessLocation(testSurface, dataCopy->getInsert()->getRefname(), dataCopy->getInsert()->getOrigin2d(), adjustedNetName);
            //accLoc->SetFeatureEntity(!!!!!);
            netAccess->AddAccessLocation(accLoc);
         }
         */
      }
   }

   // Test Probes
   /*POSITION*/ pcbDataPos = pcbBlock->getHeadDataPosition();
   while (pcbDataPos != NULL)
   {
      DataStruct *dataInPcb = pcbBlock->getNextData(pcbDataPos);
      if (dataInPcb != NULL && dataInPcb->isInsertType(insertTypeTestProbe))
      {
         DataStruct *dataCopy = m_camcadData.getNewDataStruct(*dataInPcb, true);
         if (pcbPlacedBottom && dataCopy->getInsert()->getAngleDegrees() != 0.)
            dataCopy->getInsert()->setAngleDegrees( -(dataCopy->getInsert()->getAngleDegrees()) );
         dataCopy->transform(pcbInsertMatrix, &m_camcadData, false);

         CString probeRefname = dataCopy->getInsert()->getRefname();
         int probeRefnameInt = atoi(probeRefname);

         if (this->m_activeProbeNumberOffset != 0)
         {
            // Apply the offset.
            probeRefnameInt += this->m_activeProbeNumberOffset;
            probeRefname.Format("%d", probeRefnameInt);
            dataCopy->getInsert()->setRefname(probeRefname);
            dataCopy->getDefinedAttributes()->setAttribute(this->m_camcadData, standardAttributeRefName, probeRefname);
         }
            
         if (probeRefnameInt > m_highestProbePlacedSoFar)
            m_highestProbePlacedSoFar = probeRefnameInt;

         AdjustAttrib(pcbNum, dataCopy->getAttributes(), standardAttributeNetName);

         AdjustDatalink(dataCopy, entityNumberMap);

         if (pcbPlacedBottom)
         {
            // Flip the insert params for place bottom and mirror.
            if (dataCopy->isInsert())
            {
               dataCopy->getInsert()->setPlacedBottom(       !dataCopy->getInsert()->getPlacedBottom()    );
               dataCopy->getInsert()->setLayerMirrorFlag(    !dataCopy->getInsert()->getLayerMirrored()   );
               ///dataCopy->getInsert()->setGraphicsMirrorFlag( !dataCopy->getInsert()->getGraphicMirrored() );
            }
         }

         entityNumberMap.SetAt(dataInPcb->getEntityNumber(), dataCopy->getEntityNumber());
         m_superBoard->getBlock()->getDataList().AddTail(dataCopy);
         this->m_addedProbeCount++;
      }
   }
}

void CSuperBoard::CopyNetlist(int pcbNum, BlockStruct *pcbBlock, CEntityNumberMap &entityNumberMap)
{
   // Copy the netlist. Keep track of entity number changes.

   FileStruct *pcbFile = m_camcadData.getFile(pcbBlock->getFileNumber());
   if (pcbFile != NULL)
   {
      POSITION netPos = pcbFile->getHeadNetPosition();
      while (netPos != NULL)
      {
         NetStruct *originalNet = pcbFile->getNextNet(netPos);
         CString adjustedNetName = AdjustRefname(pcbNum, originalNet->getNetName());

         NetStruct *adjustedNet = m_superBoard->getNetList().addNet(adjustedNetName);

         entityNumberMap.SetAt(originalNet->getEntityNumber(), adjustedNet->getEntityNumber());

         POSITION compPinPos = originalNet->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            CompPinStruct *cp = originalNet->getNextCompPin(compPinPos);
            CString adjustedCpRefname = AdjustRefname(pcbNum, cp->getRefDes());

            CompPinStruct *adjustedCp = adjustedNet->addCompPin(adjustedCpRefname, cp->getPinName());
            if (cp->getAttributesRef() != NULL)
               adjustedCp->getDefinedAttributes()->CopyAll(*cp->getAttributesRef());

            entityNumberMap.SetAt(cp->getEntityNumber(), adjustedCp->getEntityNumber());
         }
      }
   }
}