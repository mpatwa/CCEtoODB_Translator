// $Header: /CAMCAD/4.5/read_wrt/Pfw_out.cpp 32    4/28/06 6:53p Kurt Van Ness $

/*****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 2000. All Rights Reserved.

  The expected UNITS are INCH (from PORT.CPP)
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "pfwlib.h"
#include <math.h>
#include "pcblayer.h"
#include "graph.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4
#define  LAY_NONE          -5

typedef struct
{
   int            block_num;  //
   CString        name;
   int            padform[3];
   double         padrot[3];
   double         padxdim[3], padydim[3]; // 0 top, 1 inner, 2 bottom
   double         padxoff[3], padyoff[3];
   double         holesize;
}PFWOUTPadstack;     // there is a PFWPadstack in PFWIN defined ! This makes this structure unpredictable.
typedef CTypedPtrArray<CPtrArray, PFWOUTPadstack*> CPadstackArray;

typedef struct
{
   CString  refname;
   int      compcnt;
   int      geomnum;
   CString  pinname;       // pinname
   int      pinindex;      // pinindex
}PFWCompPin;
typedef CTypedPtrArray<CPtrArray, PFWCompPin*> CCompPinArray;

typedef struct
{
   CString  netname;
}PFWNetname;
typedef CTypedPtrArray<CPtrArray, PFWNetname*> CNetNameArray;

//extern char *layertypes[];

char  *protel_layer[] =    // used in pcblayer.cpp
{
   "No Layer",
   "Top",               //1
   "Mid Layer 1",       //2
   "Mid Layer 2",       //3
   "Mid Layer 3",       //4
   "Mid Layer 4",       //5
   "Mid Layer 5",       //6
   "Mid Layer 6",       //7
   "Mid Layer 7",       //8
   "Mid Layer 8",       //9
   "Mid Layer 9",       //10
   "Mid Layer 10",      //11
   "Mid Layer 11",      //12
   "Mid Layer 12",      //13
   "Mid Layer 13",      //14
   "Mid Layer 14",      //15
   "Bottom",            //16
   "Top Silkscreen",    //17
   "Bottom Silkscreen", //18
   "Top Paste Mask",    //19
   "Bottom Paste Mask", //20
   "Top Solder Mask",   //21
   "Bottom Solder Mask",//22
   "Plane 1",           //23
   "Plane 2",           //24
   "Plane 3",           //25
   "Plane 4",           //26
   "Drill Guide",       //27
   "Keep Out Layer",    //28
   "Mechanical Layer 1",//29
   "Mechanical Layer 2",//30
   "Mechanical Layer 3",//31
   "Mechanical Layer 4",//32
   "Drill Drawing",     //33
   "Multi Layer",       //34
};

static CCEtoODBDoc    *doc;
static FormatStruct  *format;
static FILE          *wfp;
static FILE          *flog;
static int           display_error;

static   CPadstackArray padstackarray;    // this is the device - mapping
static   int         padstackcnt;
static   int         max_signalcnt;

static   CProtelLayerArray protelArr;
static   int         maxArr = 0;

static   CCompPinArray  cmppinarray;   
static   int         cmppincnt;
 
static   CNetNameArray  netnamearray;  
static   int         netnamecnt;

static   int         MAX_PGCOUNT;
 
static void PFW_GetPADSTACKData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static void Protel_WriteData(CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer);

static void Protel_WritePCBData(CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel, int layer);

// used to find components
static void do_COMPONENTData(CDataList *DataList, CNetList *NetList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel);

// this writes the contens of components.
static void Protel_WriteCOMPONENTData(const char *refname, int compcnt, 
                   CNetList *NetList,
                   CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel, int level, int *pincnt);

static void Protel_WriteSchematicData(CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int insertLayer);

static int do_netlist(FILE *fp, FileStruct *file,CNetList *NetList);

static int get_comppinindex(const char *c, const char *p);

static int get_netnames(CNetList *NetList);
static int get_padstackindex(const char *p);

// 
//--------------------------------------------------------------
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0, layernum = LAY_NONE;
   char        typ = 'D';

   int   j;
   int   doc_cnt = 0;
   int   power_cnt = 0;
   int   highest_stacknum = 0;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.
      if (layer->getElectricalStackNumber() > highest_stacknum)
         highest_stacknum = layer->getElectricalStackNumber();
   }

   if (highest_stacknum > 16) 
   {
      ErrorMessage("Too many electrical Layers", "Protel Layer Error");
   }

   maxArr = 0;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.

      signr = 0;
      typ = 'D';

      if (layer->isFloating())    continue;

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();

         if (layer->getLayerType() == LAYTYPE_PAD_TOP)
         {
            layernum = 1;
         }
         else
         if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
         {
            layernum =16;
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
         {
            layernum = 1;
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
         {
            layernum = 16;
         }
         else
         if (layer->getLayerType() == LAYTYPE_POWERPOS)
         {
            layernum = signr;
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
         {
            layernum = signr;
         }
         else
         if (layer->getLayerType() == LAYTYPE_POWERNEG)
         {
            layernum = 23+power_cnt;
            power_cnt++;
         }
         else
         if (layer->getLayerType() == LAYTYPE_SPLITPLANE)
         {
            layernum = signr;
         }
         else
         if (layer->getLayerType() == LAYTYPE_PAD_INNER)
         {
            layernum = signr;
         }
      }
      else // here no electrical stackup
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         layernum = LAY_ALL;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL)
      {
         layernum = LAY_ALL;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         layernum = 1;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         layernum =16;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         layernum = LAY_INNER;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         layernum = LAY_INNER;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PASTE_TOP)
      {
         layernum = 19;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
      {
         layernum = 20;
      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_TOP)
      {
         layernum = 21;
      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_BOTTOM)
      {
         layernum = 22;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_TOP)
      {
         layernum = 17;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM)
      {
         layernum = 18;
      }
      else
      if (layer->getLayerType() == LAYTYPE_BOARD_OUTLINE)
      {
         layernum = 28;
      }
      else
      if (layer->getLayerType() == LAYTYPE_DRILL)
      {
         layernum = 33;
      }
      else
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
         doc_cnt++;
         layernum = 28+(doc_cnt % 4);
      }
   
      //accelArr.SetSizes
      ProtelLayerStruct *protel = new ProtelLayerStruct;
      protel->stackNum = layernum;  // layernum
      protel->signr = signr;
      protel->layerindex = j;
      protel->on = TRUE;
      protel->type = typ;
      protel->oldName = layer->getName();
      if (layernum > 0)
         protel->newName =protel_layer[layernum];
      else
         protel->newName = "";
      protelArr.SetAtGrow(maxArr++, protel);
   }

   return;
}

//--------------------------------------------------------------
static int edit_layerlist()
{
   // fill array
   ProtelLayerDlg proteldlg;

   proteldlg.arr = &protelArr;
   proteldlg.maxArr = maxArr;

   if (proteldlg.DoModal() != IDOK) 
      return FALSE;

   int   i;
   max_signalcnt = 0;

   for (i=0;i<maxArr;i++)
   {
      if (protelArr[i]->stackNum > max_signalcnt)
         max_signalcnt = protelArr[i]->stackNum;
   }
   return TRUE;
}

//--------------------------------------------------------------
static void free_layerlist()
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      delete protelArr[i];
   }
   protelArr.RemoveAll();

   return;
}

//--------------------------------------------------------------
static int get_viasize(BlockStruct *block, double *viasize, double *holesize, int *layerpair)
{
   *layerpair = 0;

   int p = get_padstackindex(block->getName());

   if (p > -1)
   {
      *holesize = padstackarray[p]->holesize;
      if (padstackarray[p]->padform[0] > 0)
         *viasize = padstackarray[p]->padxdim[0];
      else
      if (padstackarray[p]->padform[1] > 0)
         *viasize = padstackarray[p]->padxdim[1];
      else
      if (padstackarray[p]->padform[2] > 0)
         *viasize = padstackarray[p]->padxdim[2];
      else
         *viasize = 0;
      return 1;
   }

   fprintf(flog,"No via size found\n");
   display_error = TRUE;

   return 0;   // returns true is via values found.
}

//--------------------------------------------------------------
static int get_padstackindex(const char *p)
{
   int   i;
   
   for (i=0;i<padstackcnt;i++)
   {
      if (!padstackarray[i]->name.CompareNoCase(p))
         return i;
   }

   fprintf(flog,"Padstack [%s] not found in index\n",p);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static int do_padstacks(double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         double drill = 0;
/*
         if (block->toolindex > -1)
            drill = doc->ToolArray[block->toolindex]->size;
*/
         PFWOUTPadstack *p = new PFWOUTPadstack;
         padstackarray.SetAtGrow(padstackcnt,p);  

         padstackarray[padstackcnt]->name = block->getName();
         padstackarray[padstackcnt]->block_num = block->getBlockNumber();
         padstackarray[padstackcnt]->padform[0] = -1;
         padstackarray[padstackcnt]->padform[1] = -1;
         padstackarray[padstackcnt]->padform[2] = -1;
         padstackarray[padstackcnt]->holesize = get_drill_from_block_no_scale(doc, block) * scale;

         PFW_GetPADSTACKData(&(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1);
         padstackcnt++;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int load_PFWsettings(const char *fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   MAX_PGCOUNT = 1000;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage( tmp,"  PFW Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".MAX_PGVERTEX"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            MAX_PGCOUNT = atoi(lp);
         }
      }
   }

   fclose(fp);
   return 1;
}

//--------------------------------------------------------------
void Protel_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double scale)
{
   FileStruct *file;

   doc = Doc;
   format = Format;
   display_error = FALSE;

   generate_PADSTACKACCESSFLAG(doc, 1);  // - *Joanne

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }
   
   CString logFile = GetLogfilePath("protel.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage( "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   protelArr.SetSize(10,10);
   maxArr = 0;

   CString settingsFile( getApp().getExportSettingsFilePath("pfw.out") );
   load_PFWsettings(settingsFile);

   wpfw_Graph_Init_Layout();

   wpfw_Graph_File_Open_Layout(wfp);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file

      do_layerlist();

      if (edit_layerlist())
      {
         CWaitCursor wait;

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            padstackarray.SetSize(100,100);
            padstackcnt = 0;

            cmppinarray.SetSize(100,100);
            cmppincnt = 0;

            netnamearray.SetSize(100,100);
            netnamecnt = 0;

            do_padstacks(file->getScale() * scale);
            get_netnames(&(file->getNetList()));

            // first write all components
            do_COMPONENTData(&(file->getBlock()->getDataList()), &(file->getNetList()),
                  file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0);

            // now write all free graphic
            Protel_WritePCBData(&(file->getBlock()->getDataList()), file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0, -1);

            // now write netlist
            do_netlist(wfp, file, &(file->getNetList()));
   
            int   i;
            for (i=0;i<padstackcnt;i++)
            {
               delete padstackarray[i];
            }
            padstackarray.RemoveAll();

            for (i=0;i<cmppincnt;i++)
            {
               delete cmppinarray[i];
            }
            cmppinarray.RemoveAll();

            for (i=0;i<netnamecnt;i++)
            {
               delete netnamearray[i];
            }
            netnamearray.RemoveAll();
            netnamecnt = 0;
         }  
         else
         {
            Protel_WriteData(&(file->getBlock()->getDataList()), file->getInsertX() * scale,
               file->getInsertY() * scale,
               file->getRotation(), 
               file->isMirrored(), 
               file->getScale() * scale, 0,-1);
         }
      } // edit_layer

      free_layerlist();

   }
   // Does end of COMPONENT
   wpfw_Graph_File_Close_Layout();

   // close write file
   fclose(wfp);
   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

//--------------------------------------------------------------
void Protel_WriteSchematicFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double scale)
{
   FILE *wfp;
   FileStruct *file;

   doc = Doc;
   format = Format;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      return;
   }

   wpfw_Graph_Init_Schematic();

   wpfw_Graph_File_Open_Schematic(wfp);

   // EXPLODEALL,
   //   for each file:
   //     write entities of block (including any embedded inserts)
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file

      Protel_WriteSchematicData(&(file->getBlock()->getDataList()), file->getInsertX() * scale,
            file->getInsertY() * scale,
            file->getRotation(), 
            file->isMirrored(), 
            file->getScale() * scale, -1);
   }

   // Does end of COMPONENT
   wpfw_Graph_File_Close_Schematic();

   // close write file
   fclose(wfp);

   return;
}

/****************************************************************************/
/*
   return -1 if not visible
*/
static int Layer_PROTEL(int l)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (protelArr[i]->on == 0) continue;   // not visible
      if (protelArr[i]->layerindex == l)
         return protelArr[i]->stackNum;
   }

   fprintf(flog,"Layer [%s] not translated.\n", doc->getLayerArray()[l]->getName());
   return -1;
}

//--------------------------------------------------------------
static int get_netindex(const char *n)
{
   int   i;

   for (i=0;i<netnamecnt;i++)
   {
      if (netnamearray[i]->netname.Compare(n) == 0)
         return i+1;
   }

   return 0;
}

//--------------------------------------------------------------
void Protel_WritePCBData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   POSITION pos;
   int   layer;
   char  *cpfp = "FP";

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)   
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;
         layer = doc->get_layer_mirror(layer, mirror);

         int lnr = Layer_PROTEL(layer);
         if (lnr < 0)            continue;

         wpfw_Graph_LevelNum(lnr);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            int netindex = 0;
            Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
            if (a)
               netindex = get_netindex(get_attvalue_string(doc, a));

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL voided     = poly->isVoid();

               Point2   p;

               if (voided) polyFilled = FALSE;

               //if (!polyFilled)
               if (1)
               {
                  int vertexcnt = 0;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     vertexcnt++;
                  }        
                  int tcnt = vertexcnt;
                  Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                  
                  if (points != NULL)
                  {
                     wpfw_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);
                     vertexcnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        p.x = pnt->x * scale;
                        if (mirror) p.x = -p.x;
                        p.y = pnt->y * scale;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);
                        // need to check for bulge
                        points[vertexcnt].x = p.x;
                        points[vertexcnt].y = p.y;
                        points[vertexcnt].bulge = p.bulge;
                        vertexcnt++;
                     }
                     if (polyFilled && (vertexcnt > MAX_PGCOUNT))
                     {
                        fprintf(flog,"Polygon exceeds [%d] Verteces -> see pfw.out\n", vertexcnt);
                        display_error++;
                        polyFilled = FALSE;
                     }
                     wpfw_Graph_Polyline(points,vertexcnt,polyFilled, netindex);
                     free(points);
                  }
               }
               else
               {
                  // poly filled
/*
                  CPolyList *result;
                  result = VectorFillPoly(doc, np->poly.polylist, widthindex, spacing);

                  DataStruct *data = Graph_PolyStruct(s->p->layer, s->p->flag, s->p->isNegative()); // graph new data
                  delete data->poly.polylist;
                  data->poly.polylist = result; // put in new polylist
                  
                  FreePoly(result);
*/
               }
            }
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            if (mirror) point2.x = -point2.x;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wpfw_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror);

         }
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)
               break;

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               double viasize, holesize;
               int    layerpair, netindex = 0;
               // here get padform diameter and drill and layerpair
               CString  padstackname = block->getName();
               int pi = get_padstackindex(padstackname);

               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
               if (a)
                  netindex = get_netindex(get_attvalue_string(doc, a));

               get_viasize(block, &viasize, &holesize, &layerpair);
               //  via must be always round
               wpfw_via(point2.x, point2.y, viasize, holesize, netindex );
            }
            else
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
                  int lnr = Layer_PROTEL(np->getLayerIndex());
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  if (lnr > -1)
                  {
                     wpfw_Graph_LevelNum(lnr);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle(), cpfp);
                  }
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               Protel_WritePCBData(&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

} /* end Protel_WritePCBData */

//--------------------------------------------------------------
void do_COMPONENTData(CDataList *DataList,CNetList *netlistdata, 
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel)
{
   Mat2x2 m, mm;
   Point2 point2;
   DataStruct *np;
   POSITION pos;
   int      compcnt = 0;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)   continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            double xmin,ymin,xmax,ymax;

            if (np->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && np->getInsert()->getInsertType() != INSERTTYPE_MECHCOMPONENT)
               break;

            compcnt++;
            fprintf(wfp,"COMP\n");
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();
/*
            if (np->getInsert()->getMirrorFlags() & MIRROR_FLIP)  
               block_rot = PI2 - block_rot;
*/
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);
            
            xmin = xmax = point2.x;
            ymin = ymax = point2.y;

            doc->validateBlockExtents(block);

            RotMat2(&mm, block_rot);

            Point2   pp;
            pp.x = block->getXmin() * scale;
            if (block_mirror) pp.x = -pp.x;
            pp.y = block->getYmin() * scale;
            TransPoint2(&pp, 1, &mm, 0.0, 0.0); // zero to the block origin
            xmin = pp.x;
            ymin = pp.y;
            pp.x = block->getXmax() * scale;
            if (block_mirror) pp.x = -pp.x;
            pp.y = block->getYmax() * scale;

            TransPoint2(&pp, 1, &mm, 0.0, 0.0); // zero to the block origin
            xmax = pp.x;
            ymax = pp.y;
         
            if (xmax < xmin)  
            {
               double tmp;
               tmp = xmin;
               xmin = xmax;
               xmax = tmp;
            }
            if (ymax < ymin)  
            {
               double tmp;
               tmp = ymin;
               ymin = ymax;
               ymax = tmp;
            }

            fprintf(wfp,"%s\n",block->getName()); // need to limit to 12 char with no spaces
            // here one line
            wpfwcomp_Header( point2.x, point2.y, xmin+point2.x, ymin+point2.y, 
                             xmax+point2.x, ymax+point2.y, block_rot, 1, 0);

            // refdes
            wpfw_Graph_LevelNum(17);
            if (block_mirror) 
               wpfw_Graph_LevelNum(18);

            Attrib *a;
            if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_REFNAME, 1))
            {
               int layer = a->getLayerIndex();
               layer = doc->get_layer_mirror(layer, block_mirror);

               int lnr = Layer_PROTEL(layer);
               if (lnr < 0)            
               {
               }
               else
               {
                  wpfw_Graph_LevelNum(lnr);
               }

               if (block_mirror)
                  RotMat2(&mm, np->getInsert()->getAngle()-a->getRotationRadians());
               else
                  RotMat2(&mm, np->getInsert()->getAngle()+a->getRotationRadians());
   
               Point2   pp;
               pp.x = a->getX() * scale;
               if (block_mirror) pp.x = -pp.x;
               pp.y = a->getY() * scale;
               TransPoint2(&pp, 1, &mm, 0.0, 0.0); // zero to the block origin

               CString refName(np->getInsert()->getRefname());
               wpfwcomp_Graph_Text(refName.GetBuffer(), pp.x+point2.x, pp.y+point2.y,
                              a->getHeight()*scale,0.01*scale,
                              block_rot+a->getRotationRadians(), block_mirror);

            }
            else
            {
               CString refName(np->getInsert()->getRefname());
               wpfwcomp_Graph_Text(refName.GetBuffer(), point2.x, point2.y,
                              0.07*scale,0.01*scale,
                              block_rot, block_mirror);
            }

            wpfwcomp_Graph_Text( "", point2.x, point2.y,
                              0.07*scale,0.01*scale,
                              block_rot,block_mirror);
            int pincnt = 0;
            Protel_WriteCOMPONENTData(np->getInsert()->getRefname(), compcnt, 
                  netlistdata,
                  &(block->getDataList()),
                  point2.x, point2.y,
                  block_rot, block_mirror, scale * np->getInsert()->getScale(),
                  embeddedLevel+1, -1, &pincnt);
            fprintf(wfp,"ENDCOMP\n");
         } // case INSERT
         break;
      } // end switch
   } // end for

} /* end do_COMPONENTData */

//--------------------------------------------------------------
static int protel_padform(int appform)
{
   // 1 round
   // 2 rectangle
   // 3 octagonal

   if (appform == T_SQUARE)      return 2;
   if (appform == T_RECTANGLE)   return 2;
   if (appform == T_OCTAGON)     return 3;

   return 1;
}

//--------------------------------------------------------------
static int get_compin_netindex(const char *refname, const char *pinname, CNetList *NetList)
{
   NetStruct      *net;
   CompPinStruct  *compPin;
   POSITION       compPinPos, netPos;
   int            netindex = 0;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      netindex++;
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         if (compPin->getRefDes().Compare(refname) == 0 && compPin->getPinName().Compare(pinname) == 0)
            return netindex;
      }
   }
   return 0;
}

//--------------------------------------------------------------
void Protel_WriteCOMPONENTData(const char *refname, int compcnt, CNetList *Netlist, CDataList *DataList, 
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int *pcnt)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         layer;
   char        *cpfp = "CP";

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)   
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         layer = doc->get_layer_mirror(layer, mirror);

         int lnr = Layer_PROTEL(layer);
         if (lnr < 0)            continue;

         wpfw_Graph_LevelNum(lnr);
      }

      switch(np->getDataType())
      {
         case T_POLY:
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               Point2   p;
                  
               int vertexcnt = 0;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  vertexcnt++;
               }        
               int tcnt = vertexcnt;
               Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1

               if (points != NULL)
               {
                  wpfw_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);
                  vertexcnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     points[vertexcnt].x = p.x;
                     points[vertexcnt].y = p.y;
                     if (mirror) p.bulge = -p.bulge;
                     points[vertexcnt].bulge = p.bulge;
                     vertexcnt++;
                  }
                  wpfwcomp_Graph_Polyline(points,vertexcnt,polyFilled);
                  free(points);
               }
            }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            if (mirror) point2.x = -point2.x;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wpfwcomp_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror);

         }
         break;
         
         case T_INSERT:
         {
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN ||
                np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               // here pin found.
               CString pinname = np->getInsert()->getRefname();
               CString  padstackname = block->getName();
               int pi = get_padstackindex(padstackname);
               int netindex = get_compin_netindex(refname, pinname, Netlist);

               double topx = 0, topy = 0, innerx = 0, innery = 0, botx = 0, boty = 0;
               double topoffx = 0, topoffy = 0, inneroffx = 0, inneroffy = 0, botoffx = 0, botoffy = 0;
               int    topform = 1, innerform = 1, botform = 1;
               double degree = RadToDeg(block_rot);
               double holesize = 0;


               if (pi < 0)
               {
                  fprintf(flog,"Can not find PADSTACK [%s] in Padstack list!\n", padstackname);
                  display_error++;
               }
               else
               {
#ifdef _DEBUG
PFWOUTPadstack *ppp = padstackarray[pi];
#endif
                  if (padstackarray[pi]->padform[1] > -1)
                  {
                     innerform = protel_padform(padstackarray[pi]->padform[1]);
                     innerx = padstackarray[pi]->padxdim[1];
                     innery = padstackarray[pi]->padydim[1];
                     inneroffx = padstackarray[pi]->padxoff[1];
                     inneroffy = padstackarray[pi]->padyoff[1];
                     degree = RadToDeg(block_rot + padstackarray[pi]->padrot[1]);
                  }

                  if (!block_mirror)
                  {
                     if (padstackarray[pi]->padform[0] > -1)
                     {
                        topform = protel_padform(padstackarray[pi]->padform[0]);
                        topx = padstackarray[pi]->padxdim[0];
                        topy = padstackarray[pi]->padydim[0];
                        topoffx = padstackarray[pi]->padxoff[0];
                        topoffy = padstackarray[pi]->padyoff[0];
                        degree = RadToDeg(block_rot + padstackarray[pi]->padrot[0]);
                     }

                     if (padstackarray[pi]->padform[2] > -1)
                     {
                        botform = protel_padform(padstackarray[pi]->padform[2]);
                        botx = padstackarray[pi]->padxdim[2];
                        boty = padstackarray[pi]->padydim[2];
                        botoffx = padstackarray[pi]->padxoff[2];
                        botoffy = padstackarray[pi]->padyoff[2];
                        degree = RadToDeg(block_rot + padstackarray[pi]->padrot[2]);
                     }
                  }
                  else
                  {
                     if (padstackarray[pi]->padform[2] > -1)
                     {
                        topform = protel_padform(padstackarray[pi]->padform[2]);
                        topx = padstackarray[pi]->padxdim[2];
                        topy = padstackarray[pi]->padydim[2];
                        topoffx = padstackarray[pi]->padxoff[2];
                        topoffy = padstackarray[pi]->padyoff[2];
                        degree = RadToDeg(block_rot + padstackarray[pi]->padrot[2]);
                     }
   
                     if (padstackarray[pi]->padform[0] > -1)
                     {
                        botform = protel_padform(padstackarray[pi]->padform[0]);
                        botx = padstackarray[pi]->padxdim[0];
                        boty = padstackarray[pi]->padydim[0];
                        botoffx = padstackarray[pi]->padxoff[0];
                        botoffy = padstackarray[pi]->padyoff[0];
                        degree = RadToDeg(block_rot + padstackarray[pi]->padrot[0]);
                     }
                  }

                  while (degree < 0)   degree += 360;
                  while (degree >=360) degree -= 360;
                  holesize = padstackarray[pi]->holesize;
               }
               *pcnt += 1;


               BOOL TOP = (block->getFlags() & BL_ACCESS_TOP);    // - *Joanne
               BOOL BOTTOM = (block->getFlags() & BL_ACCESS_BOTTOM);

               int layernum = 34; 
               if (TOP && BOTTOM)
                  layernum = 34;
               else if (TOP)
                  layernum = 1;
               else if (BOTTOM)
                  layernum = 16;     

               wpfwcomp_pin( point2.x, point2.y, pinname, topx, topy, topform, innerx, innery, innerform,
                     botx, boty, botform, holesize, netindex, degree, layernum);   // - Joanne

               
               PFWCompPin  *p = new PFWCompPin;
               cmppinarray.SetAtGrow(cmppincnt,p);  
               cmppincnt++;
               p->refname = refname;
               p->compcnt = compcnt;
               p->pinindex = *pcnt;
               p->pinname = pinname;
            }
            else
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
                  layer = doc->get_layer_mirror(layer, mirror);

                  int lnr = Layer_PROTEL(layer);
                  if (lnr > -1)
                  {
                     wpfw_Graph_LevelNum(lnr);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle(), cpfp);

                     // this will be an unnamed pin, increment the count.
                     if (!STRCMPI(cpfp,"CP"))   *pcnt += 1;
                  }
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;
               Protel_WriteCOMPONENTData(refname, compcnt, Netlist, &(block->getDataList()), 
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, pcnt);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

} /* end Protel_WriteCOMPONENTData */

//--------------------------------------------------------------
// this is only used in graphic mode
void Protel_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         layer;
   char        *cpfp = "FP";

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         layer = doc->get_layer_mirror(layer, mirror);

         int lnr = Layer_PROTEL(layer);
         if (lnr < 0)            continue;

         wpfw_Graph_LevelNum(lnr);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               Point2   p;
                  
               int vertexcnt = 0;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  vertexcnt++;
               }        
               int tcnt = vertexcnt;
               Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1

               if (points != NULL)
               {
                  wpfw_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);
                  vertexcnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     points[vertexcnt].x = p.x;
                     points[vertexcnt].y = p.y;
                     points[vertexcnt].bulge = p.bulge;
                     vertexcnt++;
                  }
                  if (polyFilled && (vertexcnt > MAX_PGCOUNT))
                  {
                     fprintf(flog,"Polygon exceeds [%d] Verteces -> see pfw.out\n", vertexcnt);
                     display_error++;
                     polyFilled = FALSE;
                  }
                  wpfw_Graph_Polyline(points,vertexcnt,polyFilled, 0);
                  free(points);
               }
            }
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            if (mirror) point2.x = -point2.x;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wpfw_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror);

         }
         break;
         
         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               layer = doc->get_layer_mirror(layer, mirror);
               if (doc->get_layer_visible(layer, mirror))
               {
                  int lnr = Layer_PROTEL(layer);
                  if (lnr > -1)
                  {
                     wpfw_Graph_LevelNum(lnr);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle(), cpfp);
                  }
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;
               Protel_WriteData(&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

} /* end Protel_WriteData */


//--------------------------------------------------------------
void Protel_WriteSchematicData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int insertLayer)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         doc->get_layer_color(np->getLayerIndex(), mirror);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            if (!doc->get_layer_visible(np->getLayerIndex(), mirror))
               continue;
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               Point2   p;
                  
               int vertexcnt = 0;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  vertexcnt++;
               }        
               int tcnt = vertexcnt;
               Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1

               if (points != NULL)
               {
                  wpfw_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);
                  vertexcnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     points[vertexcnt].x = p.x;
                     points[vertexcnt].y = p.y;
                     points[vertexcnt].bulge = p.bulge;
                     vertexcnt++;
                  }
                  wpfwschem_Graph_Polyline(points,vertexcnt,polyFilled);
                  free(points);
               }
            }
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            if (!doc->get_layer_visible(np->getLayerIndex(), mirror))
               continue;

            point2.x = (np->getText()->getPnt().x)*scale;
            if (mirror) point2.x = -point2.x;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror = np->getText()->getResultantMirror(mirror);
            wpfwschem_Graph_Text( np->getText()->getText(), point2.x, point2.y,1,
                              text_rot, text_mirror);
         }
         break;
         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  //wpfw_Graph_Level(get_layername(layer));
                  //wpfw_Graph_Aperture(
                  //   point2.x,
                  //   point2.y,
                  //   block->getSizeA() * unitsFactor,
                  //   block->getSizeB() * unitsFactor,
                  //   block->ApShape, np->getInsert().angle);
               } // end if layer visible
            }
            else // not aperture
            {
               point2.x = np->getInsert()->getOriginX() * scale;
               if (mirror) point2.x = -point2.x;
               point2.y = np->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x,insert_y);
               Protel_WriteSchematicData(&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     np->getLayerIndex());
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for
   return;
} /* end Protel_WriteSchematicData */

//--------------------------------------------------------------
void PFW_GetPADSTACKData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   boardcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (block->getFlags() & BL_TOOL) break;

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();
   
               if (!doc->get_layer_visible(block_layer, mirror))  break;

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.


               LayerStruct *lay = doc->FindLayer(block_layer);
   
               double   padxdim, padydim, padxoff, padyoff;
   
               // block has the aperture of a padstack
               switch (block->getShape())
               {
                  case T_OCTAGON:
                  case T_DONUT:
                  case T_SQUARE:
                  case T_ROUND:
                     padxdim = padydim = block->getSizeA() * scale;
                     padxoff = padyoff = 0;
                  break;
                  case T_RECTANGLE:
                  case T_OBLONG:
                     padxdim = block->getSizeA() * scale;
                     padydim = block->getSizeB() * scale;
                     padxoff = padyoff = 0;
                  break;
                  default:
                     // unknown or complex
                     fprintf(flog,"Unevaluated Padform [%s] -> set to 5MIL round\n",apertureShapeToName(block->getShape()));
                     
                     display_error++;

                     doc->validateBlockExtents(block);

                     padxdim = (block->getXmax() - block->getXmin()) * scale;
                     padydim = (block->getYmax() - block->getYmin()) * scale;
                     // offset is not tested !
                     padxoff = ((block->getXmax() - block->getXmin())/2 - block->getXmin()) * scale;
                     padyoff = ((block->getYmax() - block->getYmin())/2 - block->getYmin()) * scale;
                  break;
               }

               if (lay != NULL)
               {
                  switch (lay->getLayerType())
                  {
                     case LAYTYPE_PAD_TOP:   
                     case LAYTYPE_SIGNAL_TOP:
                        padstackarray[padstackcnt]->padform[0] = block->getShape();
                        padstackarray[padstackcnt]->padrot[0] = block->getRotation();
                        padstackarray[padstackcnt]->padxdim[0] = padxdim;
                        padstackarray[padstackcnt]->padydim[0] = padydim;
                        padstackarray[padstackcnt]->padxoff[0] = padxoff;
                        padstackarray[padstackcnt]->padyoff[0] = padyoff;
                     break;
                     case LAYTYPE_PAD_BOTTOM:   
                     case LAYTYPE_SIGNAL_BOT:
                        padstackarray[padstackcnt]->padform[2] = block->getShape();
                        padstackarray[padstackcnt]->padrot[2] = block->getRotation();
                        padstackarray[padstackcnt]->padxdim[2] = padxdim;
                        padstackarray[padstackcnt]->padydim[2] = padydim;
                        padstackarray[padstackcnt]->padxoff[2] = padxoff;
                        padstackarray[padstackcnt]->padyoff[2] = padyoff;
                     break;
                     case LAYTYPE_SIGNAL_INNER: 
                     case LAYTYPE_PAD_INNER: 
                        padstackarray[padstackcnt]->padform[1] = block->getShape();
                        padstackarray[padstackcnt]->padrot[1] = block->getRotation();
                        padstackarray[padstackcnt]->padxdim[1] = padxdim;
                        padstackarray[padstackcnt]->padydim[1] = padydim;
                        padstackarray[padstackcnt]->padxoff[1] = padxoff;
                        padstackarray[padstackcnt]->padyoff[1] = padyoff;
                     break;
                     case LAYTYPE_PAD_ALL:      //    -4
                     case LAYTYPE_SIGNAL:       //    -4
                     case LAYTYPE_SIGNAL_ALL:   //    -4
                        padstackarray[padstackcnt]->padform[0] = block->getShape();
                        padstackarray[padstackcnt]->padrot[0] = block->getRotation();
                        padstackarray[padstackcnt]->padxdim[0] = padxdim;
                        padstackarray[padstackcnt]->padydim[0] = padydim;
                        padstackarray[padstackcnt]->padxoff[0] = padxoff;
                        padstackarray[padstackcnt]->padyoff[0] = padyoff;
                        padstackarray[padstackcnt]->padform[1] = block->getShape();
                        padstackarray[padstackcnt]->padrot[1] = block->getRotation();
                        padstackarray[padstackcnt]->padxdim[1] = padxdim;
                        padstackarray[padstackcnt]->padydim[1] = padydim;
                        padstackarray[padstackcnt]->padxoff[1] = padxoff;
                        padstackarray[padstackcnt]->padyoff[1] = padyoff;
                        padstackarray[padstackcnt]->padform[2] = block->getShape();
                        padstackarray[padstackcnt]->padrot[2] = block->getRotation();
                        padstackarray[padstackcnt]->padxdim[2] = padxdim;
                        padstackarray[padstackcnt]->padydim[2] = padydim;
                        padstackarray[padstackcnt]->padxoff[2] = padxoff;
                        padstackarray[padstackcnt]->padyoff[2] = padyoff;
                     break;
                     default:
                        fprintf(flog,"Unknown Pad Layer [%s]\n", lay->getName());
                        display_error++;
                     break;
                  }  
               }
            } // if aperture
            //write_attributes(wfp,&(np->getAttributesRef()));
         } // case INSERT
         break;
      } // end switch
   } // end PFW_GetPADSTACKData */
}

//--------------------------------------------------------------
static int do_netlist(FILE *fp, FileStruct *file,CNetList *NetList)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      fprintf(fp, "NETDEF\n");
      fprintf(fp,"%s\n", net->getNetName());
      fprintf(fp, "0 0 2 2\n");
      fprintf(fp,"(\n");
      compPinPos = net->getHeadCompPinPosition();
      int out = 0;
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         int   ptr = get_comppinindex(compPin->getRefDes(), compPin->getPinName());
         if(ptr > -1)
         {
#ifdef _DEBUG
   PFWCompPin *cp = cmppinarray[ptr];
#endif

            fprintf(fp," %d %d\n", cmppinarray[ptr]->compcnt, cmppinarray[ptr]->pinindex);
            out++;
         }
      }
      fprintf(fp,")\n");

      fprintf(fp,"{\n");
      for (int i=1;i<out;i++)
         fprintf(fp," %d %d 8 0 0\n", i, i+1);
      fprintf(fp,"}\n");

      fprintf(fp,"1 2 34\n");
      fprintf(fp,"34 0 0\n");
   }
   return 1;
}

//--------------------------------------------------------------
static int get_netnames( CNetList *NetList)
{
   NetStruct *net;

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      PFWNetname *n = new PFWNetname;
      netnamearray.SetAtGrow(netnamecnt,n);  
      netnamecnt++;
      n->netname = net->getNetName();
   }
   return 1;
}

//--------------------------------------------------------------
static int get_comppinindex(const char *c, const char *p)
{
   int   i;

   for (i=0;i<cmppincnt;i++)
   {
      if (cmppinarray[i]->refname.Compare(c) == 0 &&
          cmppinarray[i]->pinname.Compare(p) == 0)
         return i;
   }
   return -1;
}


// end PFW_OUT.CPP