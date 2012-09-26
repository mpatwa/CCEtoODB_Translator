// $Header: /CAMCAD/4.5/read_wrt/Triaoi_o.cpp 21    4/28/06 2:23p Kurt Van Ness $

/****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "font.h"
#include <math.h>
#include "lyr_file.h"
#include "gauge.h"
#include "attrib.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "graph.h"
#include "ck.h"
#include "polylib.h"
#include <math.h>
#include <float.h>
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  MAX_PADSTACKS     1000

typedef struct
{
   int            block_num;  //
   CString        name;
   double         drill;
   int            typ;        // return 0x1 top
                              //        0x2 bottom
                              //        0x4 drill
   int            shapetypetop;
   int            shapetypebot;
   double         xsizetop, ysizetop, xofftop, yofftop, toprotation;
   double         xsizebot, ysizebot, xoffbot, yoffbot, botrotation;
}TRIPadstack;

struct TRILayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString Name;        // CAMCAD name
};
typedef CTypedPtrArray<CPtrArray, TRILayerStruct*> CTRILayerArray;

static   CTRILayerArray mlArr;
static   int         maxArr = 0;
static   int         max_signallayer = 0;

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

extern   CProgressDlg *progress;

static   CCEtoODBDoc        *doc;

static   int               display_error;
static   FILE              *flog;

static   TRIPadstack       padstack[MAX_PADSTACKS];
static   int               padstackcnt;

//--------------------------------------------------------------
static   int   get_layernr_from_index(int index)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (mlArr[i]->layerindex == index)
         return mlArr[i]->stackNum;
   }

   return -99;
}

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
static int TRI_GetPADSTACKData(CDataList *DataList, int insertLayer, 
                           double *drill, double scale)
{
   DataStruct *np;
   int layer = -1;
   int typ = 0;   

   *drill = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // no etch data.
      if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();
      }
      else
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)   continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD)    continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE)    continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLSYMBOL) continue;
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
               break;

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            // insert if mirror is either global mirror or block_mirror, but not if both.
            if ( block != NULL && (block->getFlags() & BL_TOOL))
            {
               *drill = block->getToolSize() * scale;
            }
            else
            if ( block != NULL && ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE)) )
            {
               double   sizeA, sizeB, xoffset, yoffset;

               layer = np->getLayerIndex();
               if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
                  layer = Get_PADSTACKLayer(doc, &(block->getDataList()));

               if (block->getShape() == T_COMPLEX)
               {
                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
                     layer = Get_PADSTACKLayer(doc, &(subblock->getDataList()));

                  // here we have to find the layer and block->size

                  doc->validateBlockExtents(block);

                  if (!block || block->getXmin() > block->getXmax())
                  {
                     sizeA = 0;
                     sizeB = 0;
                  }
                  else
                  {
                     sizeA = block->getXmax() - block->getXmin();
                     sizeB = block->getYmax() - block->getYmin();
                     xoffset = block->getXmin() + (block->getXmax() - block->getXmin())/2;
                     yoffset = block->getYmin() + (block->getYmax() - block->getYmin())/2;
                  }

               }
               else
               {
                  sizeA = block->getSizeA();
                  sizeB = block->getSizeB();
                  xoffset = block->getXoffset();
                  yoffset = block->getYoffset();
      
                  // round and square have no B size.
                  // if the Y in feature size is 0 feature is considered to be round
                  // else square
                  if (block->getShape() == T_SQUARE)
                     sizeB = sizeA;

               }

               if (layer < 0) 
                  break;

               LayerStruct *ll = doc->getLayerArray()[layer];

               // if multiple electrical layers
               int lay = get_layernr_from_index(layer);

               if (lay == 1 || lay == LAY_TOP)
               {
                  if (ll != NULL && ll->getNeverMirror())   
                     typ |= 0x8;
                  else
                     typ |= 0x1;

                  padstack[padstackcnt].shapetypetop = block->getShape();
                  padstack[padstackcnt].toprotation = block->getRotation()+np->getInsert()->getAngle();
                  padstack[padstackcnt].xsizetop = sizeA;
                  padstack[padstackcnt].ysizetop = sizeB;
                  padstack[padstackcnt].xofftop = xoffset;
                  padstack[padstackcnt].yofftop = yoffset;
               }
               else
               if (lay > 1 || lay == LAY_BOT)
               {
                  if (ll != NULL && ll->getMirrorOnly()) 
                     typ |= 0x10;
                  else
                     typ |= 0x2;
                  padstack[padstackcnt].shapetypebot = block->getShape();
                  padstack[padstackcnt].botrotation = block->getRotation()+np->getInsert()->getAngle();
                  padstack[padstackcnt].xsizebot = sizeA;
                  padstack[padstackcnt].ysizebot = sizeB;
                  padstack[padstackcnt].xoffbot = xoffset;
                  padstack[padstackcnt].yoffbot = yoffset;
               }
               else
               if (lay == LAY_ALL)
               {
                  typ |= 0x3;
                  padstack[padstackcnt].shapetypetop = block->getShape();
                  padstack[padstackcnt].toprotation = block->getRotation()+np->getInsert()->getAngle();
                  padstack[padstackcnt].xsizetop = sizeA;
                  padstack[padstackcnt].ysizetop = sizeB;
                  padstack[padstackcnt].xofftop = xoffset;
                  padstack[padstackcnt].yofftop = yoffset;

                  padstack[padstackcnt].shapetypebot = block->getShape();
                  padstack[padstackcnt].botrotation = block->getRotation()+np->getInsert()->getAngle();
                  padstack[padstackcnt].xsizebot = sizeA;
                  padstack[padstackcnt].ysizebot = sizeB;
                  padstack[padstackcnt].xoffbot = xoffset;
                  padstack[padstackcnt].yoffbot = yoffset;

               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
} /* end TRI_GetPadstackData */

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
         if (padstackcnt < MAX_PADSTACKS)
         {
            double drillsize;
            padstack[padstackcnt].name = block->getName();
            padstack[padstackcnt].block_num = block->getBlockNumber();


            int typ = TRI_GetPADSTACKData( &(block->getDataList()), -1, 
                     &drillsize, scale);
            padstack[padstackcnt].typ = typ;
            padstack[padstackcnt].drill = drillsize;
            padstackcnt++;
         }
         else
         {
            fprintf(flog,"Too many padstacks\n");
            display_error++;
         }
      }
/*
      else
      if ( (block->getBlockType() == BLOCKTYPE_PADSHAPE))
      {
         fprintf(flog, "PADSTACK [%s] with Graphic not yet supported\n", block->name);
         display_error++;
      }
*/
   }

   return 1;
}

//--------------------------------------------------------------
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0;
   char        typ = 'D';

   //#define   LAY_TOP           -1
   //#define   LAY_BOT           -2
   //#define   LAY_INNER         -3
   //#define   LAY_ALL           -4

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];

      if (layer == NULL)   continue; // could have been deleted.
      signr = 0;

      typ = 'D';

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         if (signr > max_signallayer)
            max_signallayer = signr;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         signr = LAY_INNER;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         signr = LAY_TOP;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
      {
         signr = LAY_TOP;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL)
      {
         signr = LAY_ALL;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         signr = LAY_BOT;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
      {
         signr = LAY_BOT;
      }

      if (signr == 0)
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
      }

      // here check if layer->getName() is not already done

      //mlArr.SetSizes
      TRILayerStruct *ml = new TRILayerStruct;
      ml->stackNum = signr;
      ml->layerindex = j;
      ml->on = TRUE;
      ml->type = typ;
      ml->Name = layer->getName();
      mlArr.SetAtGrow(maxArr++, ml);
   }

   return;
}

//--------------------------------------------------------------
static void free_layerlist()
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      delete mlArr[i];
   }
   mlArr.RemoveAll();
   maxArr = 0;

   return;
}

//--------------------------------------------------------------
static int do_pinloc(CCEtoODBDoc *doc, FILE *fp, FileStruct *f, double UnitsFactor, int output_units_accuracy)
{
   // loop throu the PCB file
   DataStruct *np;
   Point2  point2;
                           
   Mat2x2 m;
   RotMat2(&m, f->getRotation());

   double insert_x = f->getInsertX();
   double insert_y = f->getInsertY();
   double rotation = f->getRotation();
   int    mirror = f->isMirrored();
   double scale = f->getScale() * UnitsFactor;

   POSITION pos = f->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = f->getBlock()->getDataList().GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || 
                np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
            {
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
               CString compname = np->getInsert()->getRefname();

               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();
               point2.x = np->getInsert()->getOriginX() * scale;
               if (mirror) point2.x = -point2.x;
               point2.y = np->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x,insert_y);
               
               // has no reference designator
               if (strlen(compname) == 0) break;

               Attrib *a;
               CString grid_location;
               grid_location = "A1";

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_GRID_LOCATION, 1))
                  grid_location = get_attvalue_string(doc, a); // can have multiple args

               fprintf(fp,"%s  %1.*lf  %1.*lf  %1.1lf  %s  (%c)\n", compname,
                  output_units_accuracy, point2.x, output_units_accuracy, point2.y, 
                  RadToDeg(block_rot), grid_location, 
                  (np->getInsert()->getPlacedBottom())?'B':'T');
               fprintf(fp,"\n");

               // here loop throu block (component definition) and find PINPOS
               DataStruct *bp;
               POSITION bpos = block->getDataList().GetHeadPosition();
               while (bpos != NULL)
               {
                  bp = block->getDataList().GetNext(bpos);
      
                  if (bp->getDataType() != T_INSERT)        continue;
                  switch(bp->getDataType())
                  {
                     case T_INSERT:
                     {
                        if (bp->getInsert()->getInsertType() == INSERTTYPE_PIN)
                        {
                           // here is a pin
                           CString pinname = bp->getInsert()->getRefname();

                           BlockStruct *b = doc->Find_Block_by_Num(bp->getInsert()->getBlockNumber());
                           doc->CalcBlockExtents(b);
                           double sizeA = (b->getXmax() - b->getXmin()) * scale;
                           double sizeB = (b->getYmax() - b->getYmin()) * scale;
                           double offx  = b->getXmin() * scale + sizeA/2;
                           double offy  = b->getYmin() * scale + sizeB/2;

                           Point2  point2;
                           Mat2x2 m;
                           RotMat2(&m, block_rot);

                           point2.x = bp->getInsert()->getOriginX() * scale;
                           if (block_mirror)
                              point2.x = -point2.x;

                           point2.y = bp->getInsert()->getOriginY() * scale;

                           TransPoint2(&point2, 1, &m, insert_x, insert_y);
                           // absolute angle is Comprot + insert of pinrot
                           int pinrot = round(RadToDeg(block_rot + bp->getInsert()->getAngle())/90);
                           while (pinrot < 0)   pinrot += 4;
                           while (pinrot > 3)   pinrot -= 4;

                           for (int i=0;i<pinrot;i++)
                           {

                              double tmp = offx;
                              offx = offy;
                              offy = -tmp;
                              tmp = sizeA;
                              sizeB = sizeA;
                              sizeA = tmp;
                           }     

                           fprintf(fp," %s %1.*lf %1.*lf %1.*lf %1.*lf\n", pinname, 
                              output_units_accuracy, point2.x, output_units_accuracy, point2.y,
                              output_units_accuracy, sizeA, output_units_accuracy, sizeB);                    

                        } //this is a INSERTTYPE_PIN
                     }
                  }
               } // end loop thru block
               fprintf(fp,"\n");
            }
         } // case INSERT
         break;
      } // end switch
   } // end 

   return 1;
} // do_pinloc

//--------------------------------------------------------------
void TRI_AOI_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format,
                        int page_units)
{
   FileStruct *file;
   FILE        *wfp;

   doc = Doc;

   display_error = FALSE;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }

   CString logFile = GetLogfilePath("tri-aoi.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fprintf(flog,"Filename: %s\n",filename);
   CTime t;
   t = t.GetCurrentTime();
   fprintf(flog,"%s\n",t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));


   // Format (Fabmaster units are in mils)
   double unitsFactor = 1.0; // Units_Factor(page_units, UNIT_MILS);
   int    pcb_found = FALSE;
   int    output_units = page_units;

   // collect all padstacks
   mlArr.SetSize(100,100);
   maxArr = 0;
   max_signallayer = 0;
   do_layerlist();
         
   if (max_signallayer < 2)
      max_signallayer = 2;
         
   generate_PADSTACKACCESSFLAG(doc, 0);   // 
   padstackcnt = 0;
   do_padstacks(unitsFactor);
   int output_units_accuracy = GetDecimals(page_units); 

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         pcb_found = TRUE;
         progress->SetStatus("Preparing Data for TRI-AOI output...");

         CTime t;
         t = t.GetCurrentTime();

         // 15-Sep-1999 16:51
         fprintf(wfp,"BoardName %s     %s units               %s\n", file->getName(),
               GetUnitName(output_units), t.Format("%d-%b-%Y  %H:%M"));
         fprintf(wfp,"\n\n");
         fprintf(wfp,"Part  X  Y  Rot  Grid  T/B\n");
         fprintf(wfp," Pin Name X Y X_Length Y_Length\n");
         fprintf(wfp,"\n\n");

         // write out padstacks
         pcb_found = TRUE;
         do_pinloc(doc, wfp, file, unitsFactor, output_units_accuracy);
         
         break; // do only one pcb file
      } // if pcb file
   }

   fclose(wfp);

   free_layerlist();

   if (!pcb_found)
   {
      MessageBox(NULL, "No PCB file found !","TRI-AOI Output", MB_OK | MB_ICONHAND);
   }

   t = t.GetCurrentTime();
   fprintf(flog,"%s\n",t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));

   fprintf(flog,"Logfile closed\n");

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

// end TRIAOI_O.CPP

