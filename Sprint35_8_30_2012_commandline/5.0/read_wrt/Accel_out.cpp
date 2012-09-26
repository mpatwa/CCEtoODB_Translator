// $Header: /CAMCAD/5.0/read_wrt/Accel_out.cpp 54    6/17/07 8:55p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994, 2000. All Rights Reserved.

   layercontens is never be allowed to be empty !!!

                            
*/
                                   
#include "stdafx.h"
#include "ccdoc.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "graph.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include <math.h>
#include <float.h>
#include "ck.h"
#include "pcblayer.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4
#define  LAY_PLANE         -5

#define  ACCEL_SMALLNUMBER 0.1      // accurracy 

typedef struct
{
   int            block_num;  //
   CString        name;
   int            used_as_via;
   UINT64         layermap;
}ACCELPadstack;
typedef CTypedPtrArray<CPtrArray, ACCELPadstack*> CPadstackArray;

typedef struct
{
   CString  name;
   double   height;
   int      penwidthindex;
} ACCELTextStruct;

typedef CTypedPtrArray<CPtrArray, ACCELTextStruct*> TextArray;

//
typedef struct
{
   int      index;      // pinnumber
   double   pinx, piny;
   double   rotation;
   CString  padstackname;
   CString  pinname;
   int      mechanical;
   int      flipped;
}ACCELShapePinInst;

typedef CTypedPtrArray<CPtrArray, ACCELShapePinInst*> ShapePinInstArray;
//
typedef struct
{
   int      geomnum;
   int      index;      // pinnumber
   CString  pinname;
}ACCELCompPinInst;
typedef CTypedPtrArray<CPtrArray, ACCELCompPinInst*> CompPinInstArray;

//extern char             *layertypes[];

static CCEtoODBDoc       *doc;
static FormatStruct     *format;
static FILE             *flog;
static int              display_error;

static   CString        ident;
static   int            identcnt = 0;

static   ShapePinInstArray shapepininstarray;   
static   int            shapepininstcnt;

static   CompPinInstArray  comppininstarray; 
static   int            comppininstcnt;

static   CPadstackArray padstackarray;    // this is the device - mapping
static   int            padstackcnt;

static   int            max_signalcnt;
static   int            output_units_accuracy = 1; // 1/10 of a mil.
 
static   CACCELLayerArray accelArr;
static   int            maxArr = 0;

static   TextArray      textarray;
static   int            textcnt;

static   int            outside_koo;

static   double         ARC_ANGLE_DEGREE;
static   double         WORKSPACE_ENLARGE;

static void Accel_GetTextData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror, double scale, int embeddedLevel, int insertlayer);

static int Accel_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int viatype, 
      double drill, UINT64 *layermap);

static void Accel_WriteData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer, int negative_check, int patterndef);

static void Accel_WriteInsertData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer);

static void Accel_CheckPCBInsertData(FileStruct *file, CDataList *DataList);

static void Accel_WritePCBInsertData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer);

static void Accel_WriteCompInstData(FILE *wfp, CDataList *DataList);
static int update_textarray(double height, int penwidthindex, double scale);
static int load_ACCELsettings(const char *fname);
static void write_compdef(FILE *wfp, BlockStruct* block, TypeStruct* type);

/****************************************************************************/
/*
*/
static int   waccel_plusident()
{
   identcnt++;
   ident.Format("%*s",identcnt*2, " ");
   return 1;
}

/****************************************************************************/
/*
*/
static int   waccel_minusident()
{
   if (identcnt > 0)
   {
      identcnt--;
      if (identcnt == 0)
         ident = "";
      else
         ident.Format("%*s",identcnt*2, " ");
   }
   else
   {
      ErrorMessage("Error in minus ident\n");
      return -1;
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int ACCELPinNameCompareFunc( const void *arg1, const void *arg2 )
{
   ACCELShapePinInst **a1, **a2;
   a1 = (ACCELShapePinInst**)arg1;
   a2 = (ACCELShapePinInst**)arg2;

   // order so that mechanical holes come in last.
   if ((*a1)->mechanical && !(*a2)->mechanical)
      return -1;
   // order so that mechanical holes come in last.
   if (!(*a1)->mechanical && (*a2)->mechanical)
      return 1;

   return compare_name((*a1)->pinname, (*a2)->pinname);
}

/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is 
   because of the 2 pass to get artwork info.
*/
static void ACCEL_SortPinData(const char *s)
{
   int   done = FALSE;
   int   sortcnt = 0;

   if (shapepininstcnt < 2)   return;

   qsort(shapepininstarray.GetData(), shapepininstcnt, sizeof(ACCELShapePinInst *), ACCELPinNameCompareFunc);
   return;
}

/*****************************************************************************/
/*
*/
static int get_dummy_pinnr()
{
   int   i, dummy = 0;
   int   found = TRUE;

   while (found)
   {
      found = FALSE;
      CString  d;
      d.Format("Dummy%d", ++dummy);
      for (i=0;i<shapepininstcnt;i++)
      {
         if (shapepininstarray[i]->pinname.Compare(d) == 0)
            found = TRUE;
      }
   }
   return dummy;
}

/*****************************************************************************/
/*
   mechanical pins can have same names, but this is not allowed in ACCEL
*/
static void ACCEL_MakePinNames_Unique(const char *s)
{
   int   i, ii;

   for (i=0;i<shapepininstcnt;i++)
   {
      for (ii=0;ii<i;ii++)
      {
         ACCELShapePinInst *p1 = shapepininstarray[i];
         ACCELShapePinInst *p2 = shapepininstarray[ii];
         if(p1->pinname.Compare(p2->pinname) == 0)
         {
            if (p1->mechanical && p2->mechanical)
            {
               // both are mechanical
               p1->pinname.Format("Dummy%d", get_dummy_pinnr());
               p2->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
            else
            if (!p1->mechanical && p2->mechanical)
            {
               // p1 is electrical, p2 is mechanical
               p2->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
            else
            if (p1->mechanical && !p2->mechanical)
            {
               // p2 is electrical, p1 is mechanical
               p1->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
            else
            {
               fprintf(flog,"Pattern [%s] has 2 same electrical pinnames!\n", s);
               display_error++;
               p1->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
         }
      }
   
   }

   return;
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
static void get_vianames(CDataList *DataList)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               int ptr = get_padstackindex(block->getName());
               if (ptr < 0)
               {
                  fprintf(flog, "Geometry used as Via [%s] not defined as Padstack.\n", block->getName());
                  display_error++;
               }
               else
               {
                  padstackarray[ptr]->used_as_via = TRUE;
               }
               break;
            }

            if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               get_vianames(&(block->getDataList()));
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end get_vianames */
   return;
}

//--------------------------------------------------------------
static int get_layernum_from_stacknum(int signr)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (accelArr[i]->signr == signr)
         return accelArr[i]->stackNum;
   }
   return 0;
}

//--------------------------------------------------------------
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0, layernum=-99;
   char        typ = 'D';
   CString     accellay;

   int   j;
   int   doc_cnt = 0;
   int   highest_stacknum = 0;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      layer = doc->getLayerArray()[j];

      if (layer != NULL)
      {
         if (layer->getElectricalStackNumber() > highest_stacknum)
            highest_stacknum = layer->getElectricalStackNumber();
      }
   }

   maxArr = 0;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];

      if (layer != NULL)
      {
         signr = 0;
         typ = 'D';
         accellay = "";

         if (!layer->isFloating())
         {
            if (layer->getElectricalStackNumber())
            {
               typ = 'S';
               signr = layer->getElectricalStackNumber();

               if (layer->getLayerType() == LAYTYPE_PAD_TOP)
               {
                  layernum = 1;
                  accellay = "Top";
               }
               else
               if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
               {
                  layernum = 2;
                  accellay = "Bottom";
               }
               else
               if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
               {
                  accellay = "Top";
                  layernum = 1;
               }
               else
               if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
               {
                  accellay = "Bottom";
                  layernum = 2;
               }
               else
               if (layer->getLayerType() == LAYTYPE_POWERPOS)
               {
                  accellay.Format("Inner %d",signr);
                  layernum = get_layernum_from_stacknum(signr);
                  if (layernum == 0)
                    layernum = 12+signr;
               }  
               else
               if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
               {
                  accellay.Format("Inner %d",signr);
                  layernum = get_layernum_from_stacknum(signr);
                  if (layernum == 0)
                     layernum = 12+signr;
               }
               else
               if (layer->getLayerType() == LAYTYPE_POWERNEG)
               {
                  typ = 'P';
                  accellay.Format("Inner %d",signr);
                  layernum = get_layernum_from_stacknum(signr);
                  if (layernum == 0)
                     layernum = 12+signr;
               }
               else
               if (layer->getLayerType() == LAYTYPE_SPLITPLANE)
               {
                  typ = 'P';
                  accellay.Format("Inner %d",signr);
                  layernum = get_layernum_from_stacknum(signr);
                  if (layernum == 0)
                     layernum = 12+signr;
               }
               else
               if (layer->getLayerType() == LAYTYPE_PAD_INNER)
               {
                  accellay.Format("Inner %d",signr);
                  layernum = get_layernum_from_stacknum(signr);
                  if (layernum == 0)
                     layernum = 12+signr;
               }
            }  // stacknum
            else
            if (layer->getLayerType() == LAYTYPE_PAD_TOP)
            {
               signr = 1;
               typ = 'S';
               layernum = 1;
               accellay = "Top";
            }
            else
            if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
            {
               signr = highest_stacknum;
               typ = 'S';
               layernum = 2;
               accellay = "Bottom";
            }
            else
            if (layer->getLayerType() == LAYTYPE_PAD_INNER)
            {
               typ = 'S';
               layernum = 0;
               accellay = "Signal";
            }
            else
            if (layer->getLayerType() == LAYTYPE_PAD_THERMAL)
            {
               typ = 'S';
               layernum = 0;
               accellay = "Plane";
            }
            else
            if (layer->getLayerType() == LAYTYPE_PASTE_TOP || !layer->getName().CompareNoCase("Top Paste"))
            {
               accellay = "Top Paste";
               layernum = 8;
            }
            else
            if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM || !layer->getName().CompareNoCase("Bot Paste"))
            {
               accellay = "Bot Paste";
               layernum = 9;
            }
            else
            if (layer->getLayerType() == LAYTYPE_MASK_TOP || !layer->getName().CompareNoCase("Top Mask"))
            {
               accellay = "Top Mask";
               layernum = 4;
            }
            else
            if (layer->getLayerType() == LAYTYPE_MASK_BOTTOM || !layer->getName().CompareNoCase("Bot Mask"))
            {
               accellay = "Bot Mask";
               layernum = 5;
            }
            else
            if (layer->getLayerType() == LAYTYPE_SILK_TOP || !layer->getName().CompareNoCase("Top Silk"))
            {
               accellay = "Top Silk";
               layernum = 6;
            }
            else
            if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM || !layer->getName().CompareNoCase("Bot Silk"))
            {
               accellay = "Bot Silk";
               layernum = 7;
            }
            else
            if (layer->getLayerType() == LAYTYPE_BOARD_OUTLINE || !layer->getName().CompareNoCase("Board"))
            {
               accellay = "Board";
               layernum = 3;
            }
            else
            if (!layer->getName().CompareNoCase("Top")) // just in case it comes here
            {
               accellay = "Top";
               layernum = 1;
            }
            else
            if (!layer->getName().CompareNoCase("Bottom")) // just in case 
            {
               accellay = "Bottom";
               layernum = 2;
            }
            else
            {
               // do not allow electrical layers to be switched off.
               if (!doc->get_layer_visible(j, FALSE))
                  continue;
               accellay = layer->getName();
               doc_cnt++;
               layernum = 12+highest_stacknum+doc_cnt;
            }

            // max 10 char for a layername
            if (strlen(accellay) > 10)
            {
               accellay.Format("LAYER%d", layernum);
            }
            accellay = check_name('l', accellay);
            // the layer number is 12 fixed accel layers, than first the signals and than the documentation.

            //accelArr.SetSizes
            ACCELLayerStruct *accel = new ACCELLayerStruct;
            accel->stackNum = layernum;  // layernum
            accel->signr = signr;
            accel->layerindex = j;
            accel->on = TRUE;
            accel->type = typ;
            accel->oldName = layer->getName();
            accel->newName = accellay;
            accelArr.SetAtGrow(maxArr++, accel);
         }
      }
   }

   return;
}

//--------------------------------------------------------------
static   int   get_layernr_from_index(int index)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (accelArr[i]->layerindex == index)
         return accelArr[i]->stackNum;
   }

   return -99;
}

//--------------------------------------------------------------
static   int   get_layerptr_from_index(int index)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (accelArr[i]->layerindex == index)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static int do_padstacks(FILE *fp, double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK) || (block->getBlockType() == BLOCKTYPE_TESTPAD))
      {
         ACCELPadstack  *p = new ACCELPadstack;
         padstackarray.SetAtGrow(padstackcnt,p);  
         padstackarray[padstackcnt]->used_as_via = 0;       
         padstackarray[padstackcnt]->name = block->getName();
         padstackarray[padstackcnt]->block_num = block->getBlockNumber();
         fprintf(fp,"%s(padStyleDef \"%s\"\n", ident, check_name('p', block->getName()));
         waccel_plusident();

         int smd = FALSE;
         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 2);
         if (a)   smd = TRUE;

         double drill = 0.0;
         if (smd)
         {
            fprintf(fp,"%s(holeDiam 0.0)\n", ident);
            fprintf(fp,"%s(isHolePlated False)\n", ident);
         }
         else
         {
            drill = get_drill_from_block_no_scale(doc, block);
            fprintf(fp,"%s(holeDiam %1.*lf)\n",ident, output_units_accuracy, drill*scale);
         }
         UINT64   layermap;
         int typ = Accel_WritePADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1, FALSE, 
            drill*scale, &layermap);
         padstackarray[padstackcnt]->layermap = layermap;


/* this is an SMD pad definition
    (holeDiam 0.0)
    (isHolePlated False)
    (StartRange 1)
    (EndRange 2)
    (padShape (layerNumRef 1) (padShapeType Rect) (shapeWidth 70.0) (shapeHeight 102.0) )
    (padShape (layerNumRef 2) (padShapeType Rect) (shapeWidth 0.0) (shapeHeight 0.0) )
    (padShape (layerType Signal) (padShapeType Rect) (shapeWidth 0.0) (shapeHeight 0.0) )
    (padShape (layerType Plane) (padShapeType Thrm4_45) (outsideDiam 0.0) (insideDiam 0.0) )
    (padShape (layerType NonSignal) (padShapeType Ellipse) (shapeWidth 0.0) (shapeHeight 0.0) )
 */

         if (smd)
         {
            fprintf(fp,"%s(padShape (layerNumRef 2) (padShapeType Rect) (shapeWidth 0.0) (shapeHeight 0.0) )\n", ident);
            fprintf(fp,"%s(padShape (layerType Signal) (padShapeType Rect) (shapeWidth 0.0) (shapeHeight 0.0) )\n", ident);
            fprintf(fp,"%s(padShape (layerType Plane) (padShapeType Thrm4_45) (outsideDiam 0.0) (insideDiam 0.0) )\n", ident);
            fprintf(fp,"%s(padShape (layerType NonSignal) (padShapeType Ellipse) (shapeWidth 0.0) (shapeHeight 0.0) )\n", ident);
         }
         else
         {
            double psize = drill*scale+10;
            // must have top and bottom. (otherwise Accel EDA crashes)
            if (!(typ & 1))
               fprintf(fp,"%s(padShape (layerNumRef 1) (padShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
                  ident, output_units_accuracy, psize, output_units_accuracy, psize);
            if (!(typ & 2))
               fprintf(fp,"%s(padShape (layerNumRef 2) (padShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
                  ident, output_units_accuracy, psize, output_units_accuracy, psize);
            if (!(typ & 4))
               fprintf(fp,"%s(padShape (layerType Signal) (padShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
                  ident, output_units_accuracy, psize, output_units_accuracy, psize);
            if (!(typ & 8))
               fprintf(fp,"%s(padShape (layerType Plane) (padShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
                  ident, output_units_accuracy, psize, output_units_accuracy, psize);
            if (!(typ & 16))
               fprintf(fp,"%s(padShape (layerType NonSignal) (padShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
                  ident, output_units_accuracy, psize, output_units_accuracy, psize);
         }  // if smd

         waccel_minusident();
         fprintf(fp,"%s)\n", ident);

         padstackcnt++;
      }
   }

   return 1;
}

//--------------------------------------------------------------
static int do_viadef(FILE *fp, double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getBlockType() != BLOCKTYPE_PADSTACK && block->getBlockType() != BLOCKTYPE_TESTPAD)   continue;

      int ptr = get_padstackindex(block->getName());
      if (ptr > -1)
      {
         if (!padstackarray[ptr]->used_as_via)  continue;
#ifdef _DEBUG
   ACCELPadstack *pp = padstackarray[ptr];
#endif
         fprintf(fp,"%s(viaStyleDef \"%s\"\n", ident, check_name('p', block->getName()));
         waccel_plusident();

         double drill = get_drill_from_block_no_scale(doc, block);

         int blind = FALSE;
         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_BLINDSHAPE, 2);
         if (a)   blind = TRUE;

         int smd = FALSE;
         a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 2);
         if (a)   smd = TRUE;

         fprintf(fp,"%s(holeDiam %1.*lf)\n",ident, output_units_accuracy, drill*scale);
         if (blind || smd)
         {
            int   startlayer = max_signalcnt, endlayer = 1;
            int   i;
            for (i=0;i<max_signalcnt;i++)
            {
               UINT64 baseVal = 1L;
               if (padstackarray[ptr]->layermap & (baseVal << i))
               {
                  if ((i+1) < startlayer) startlayer = i+1;
                  if ((i+1) > endlayer)   endlayer = i+1;
               }
            }
            fprintf(fp,"%s(StartRange %d)(EndRange %d)\n", ident, startlayer, endlayer);
         }

         UINT64   layermap = 0;
         int typ = Accel_WritePADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1, TRUE, 
            drill*scale, &layermap);

         double psize = drill*scale+10;

         // must have top and bottom. (otherwise Accel EDA crashes)
         if (!(typ & 1))
            fprintf(fp,"%s(viaShape (layerNumRef 1) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);
         if (!(typ & 2))
            fprintf(fp,"%s(viaShape (layerNumRef 2) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);

         if (!(typ & 4))
            fprintf(fp,"%s(viaShape (layerType Signal) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);

         if (!(typ & 8))
            fprintf(fp,"%s(viaShape (layerType Plane) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);
         if (!(typ & 16))
            fprintf(fp,"%s(viaShape (layerType NonSignal) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);

         waccel_minusident();
         fprintf(fp,"%s)\n", ident);
      }
   }

   return 1;
}

//--------------------------------------------------------------
static int edit_layerlist()
{
   // fill array
   ACCELLayer acceldlg;

   acceldlg.arr = &accelArr;
   acceldlg.maxArr = maxArr;

   if (acceldlg.DoModal() != IDOK) 
      return FALSE;

   int   i;
   max_signalcnt = 0;

   for (i=0;i<maxArr;i++)
   {
      if (accelArr[i]->signr > max_signalcnt)
         max_signalcnt = accelArr[i]->signr;
   }
   return TRUE;
}

//--------------------------------------------------------------
static void free_layerlist()
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      delete accelArr[i];
   }
   accelArr.RemoveAll();

   return;
}

//--------------------------------------------------------------
static int do_netlistfile(FILE *fp, FileStruct *file)
{

   NetStruct *net;
   POSITION  netPos;
   int   unusedpincnt = 0;
   
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if ((net->getFlags() & NETFLAG_UNUSEDNET)) continue;

      fprintf(fp, "%s(net \"%s\"\n", ident, check_name('n', net->getNetName()));
      waccel_plusident();

      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         CString  c ,p;
         c = check_name('c', compPin->getRefDes());
         p = compPin->getPinName();
         fprintf(fp, "%s(node \"%s\" \"%s\")\n", ident, c, p);
      }
      waccel_minusident();
      fprintf(fp,"%s)\n\n", ident);
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_netlist(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(netlist \"Netlist_1\"\n", ident);
   waccel_plusident();

   // globalAttr

   // compinst
   Accel_WriteCompInstData(wfp, &(file->getBlock()->getDataList()));

   // do netlist
   do_netlistfile(wfp,file);

   waccel_minusident();
   fprintf(wfp,"%s)\n\n", ident);

   return 1;
}

//--------------------------------------------------------------
/*
*/
static int ACCEL_patterndefpin(FILE *wfp, CDataList *DataList, double scale)
{
   DataStruct *np;
   int         pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               Point2 point2;
   
               point2.x = np->getInsert()->getOriginX() * scale;
               point2.y = np->getInsert()->getOriginY() * scale;

               //TransPoint2(&point2, 1, &m, insert_x, insert_y);

               double r = RadToDeg(np->getInsert()->getAngle());
   
               while (r < 0)     r+=360;
               while (r >= 360)  r-=360;

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               ACCELShapePinInst *c = new ACCELShapePinInst;
               shapepininstarray.SetAtGrow(shapepininstcnt,c);  
               shapepininstcnt++;   
               c->padstackname = block->getName();
               c->pinname = np->getInsert()->getRefname();
               c->rotation = r;
               c->pinx = point2.x;
               c->piny = point2.y;
               c->mechanical = FALSE;
               c->flipped = np->getInsert()->getMirrorFlags();
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               Point2 point2;
   
               point2.x = np->getInsert()->getOriginX() * scale;
               point2.y = np->getInsert()->getOriginY() * scale;

               //TransPoint2(&point2, 1, &m, insert_x, insert_y);

               double r = RadToDeg(np->getInsert()->getAngle());
   
               while (r < 0)     r+=360;
               while (r >= 360)  r-=360;

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               ACCELShapePinInst *c = new ACCELShapePinInst;
               shapepininstarray.SetAtGrow(shapepininstcnt,c);  
               shapepininstcnt++;   
               c->padstackname = block->getName();
               c->pinname = np->getInsert()->getRefname();
               c->rotation = r;
               c->pinx = point2.x;
               c->piny = point2.y;
               c->mechanical = TRUE;
               c->flipped = np->getInsert()->getMirrorFlags();
            }
         }
         break;                                                                
      } // end switch
   } // end ACCEL_patterndefpin 
   return pincnt;
}

/*****************************************************************************/
/*
*/
static int  do_patterndef(FILE *wfp, double scale)
{
	int   i;

	for (i=0; i<doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);

      if ( block !=NULL && !(block->getFlags()&BL_WIDTH))
      {
		   if ( (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT))
		   {
			   fprintf(wfp,"%s(patternDef \"%s_1\"\n", ident, check_name('s',block->getName()));
			   waccel_plusident();

			   fprintf(wfp,"%s(originalName \"%s\")\n", ident, check_name('s',block->getName()));
			   // do pins

			   fprintf(wfp,"%s(multiLayer\n", ident);
			   waccel_plusident();
			   ACCEL_patterndefpin(wfp, &(block->getDataList()), scale);
			   ACCEL_MakePinNames_Unique(block->getName());
			   ACCEL_SortPinData(block->getName());
				
				int pcnt=0;
			   for (pcnt=0;pcnt<shapepininstcnt;pcnt++)
			   {
				   shapepininstarray[pcnt]->index = pcnt+1;
				   fprintf(wfp,"%s(pad (padNum %d) (padStyleRef \"%s\")(pt %1.*lf %1.*lf) (rotation %1.1lf)",
					   ident, pcnt+1, check_name('p', shapepininstarray[pcnt]->padstackname), 
					   output_units_accuracy, shapepininstarray[pcnt]->pinx, 
					   output_units_accuracy, shapepininstarray[pcnt]->piny, 
					   shapepininstarray[pcnt]->rotation);

				   if (shapepininstarray[pcnt]->flipped)
					   fprintf(wfp,"(isFlipped TRUE)");
				   fprintf(wfp,")\n");

				   ACCELCompPinInst *c = new ACCELCompPinInst;
				   comppininstarray.SetAtGrow(comppininstcnt,c);  
				   comppininstcnt++; 
				   c->geomnum = block->getBlockNumber();
				   c->pinname = shapepininstarray[pcnt]->pinname;
				   c->index =  shapepininstarray[pcnt]->index;
			   }

			   waccel_minusident();
			   fprintf(wfp,"%s)\n", ident);

			   // do graphic
			   Accel_WriteData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0,-1, FALSE, TRUE);

			   Attrib* attrib;

			   if (attrib = is_attvalue(doc, block->getAttributesRef(), ATT_REFNAME, 1))
			   {
				   fprintf(wfp,"%s(layerContents  (layerNumRef  6)\n", ident);

				   waccel_plusident();

				   fprintf(wfp,"%s(attr \"RefDes\" \"\" (pt 0.0 0.0)(isVisible True) (textStyleRef \"T:%s\"))\n",
					   ident, textarray[0]->name);

				   waccel_minusident();

				   fprintf(wfp,"%s)\n", ident);
			   }
			   else
			   {
				   fprintf(wfp,"%s(layerContents  (layerNumRef  6)\n", ident);

				   waccel_plusident();

				   fprintf(wfp,"%s(attr \"RefDes\" \"\" (pt 0.0 0.0)(isVisible True) (textStyleRef \"T:%s\"))\n",
					   ident, textarray[0]->name);

				   waccel_minusident();

				   fprintf(wfp,"%s)\n", ident);
			   }

			   CAttributes* map;
			   map = block->getAttributesRef();

            for (POSITION pos = map != NULL ? map->GetStartPosition() : NULL;pos != NULL;)
			   {
				   WORD keyword;
				   Attrib* attrib;

				   map->GetNextAssoc(pos, keyword, attrib);

				   if (keyword != doc->IsKeyWord(ATT_REFNAME, 0))
               {
				      char *temp = STRDUP(get_attvalue_string(doc, attrib));

				      fprintf(wfp,"%s(layerContents  (layerNumRef  6)\n", ident);

				      waccel_plusident();

				      fprintf(wfp,"%s(attr \"%s\" \"%s\" (textStyleRef \"(Default)\"))\n", ident, 
					      doc->getKeyWordArray()[keyword]->out, temp, textarray[0]->name);

				      waccel_minusident();

				      fprintf(wfp,"%s)\n", ident);
                  free (temp);
               }
			   }

			   waccel_minusident();
			   fprintf(wfp,"%s)\n", ident);

			   for (pcnt=0;pcnt<shapepininstcnt;pcnt++)
			   {
			      delete shapepininstarray[pcnt];
			   }
			   shapepininstcnt = 0;
		   }
      }
	}

	return 1;
}

/******************************************************************************
* do_compdef
*/
static void do_compdef(FILE *wfp, FileStruct *file, double scale)
{
	for (int blockNum=0; blockNum<doc->getMaxBlockIndex(); blockNum++)
	{
		BlockStruct *block = doc->getBlockAt(blockNum);
		if (block == NULL)
			continue;
		if (block->getFlags() & BL_WIDTH)
			continue;		        
		if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
			continue;

		bool hasType = false;
		CTypeList* typeList = &file->getTypeList();
		for (POSITION pos = typeList->GetHeadPosition(); pos != NULL;)
		{
			TypeStruct* type = typeList->GetNext(pos);
			if (type == NULL || type->getBlockNumber() != block->getBlockNumber())
				continue;

			write_compdef(wfp, block, type);
			hasType = true;		
		}

		if (!hasType)
			write_compdef(wfp, block, NULL);
   }
}

/******************************************************************************
* write_compdef
*/
static void write_compdef(FILE *wfp, BlockStruct* block, TypeStruct* type)
{
	if (block == NULL)
		return;

	if (type != NULL)
	{
		fprintf(wfp,"%s(compDef \"%s_1\"\n", ident, check_name('s', type->getName()));
		waccel_plusident();

		fprintf(wfp,"%s(originalName \"%s\")\n", ident, check_name('s',type->getName()));
	}
	else
	{
		fprintf(wfp,"%s(compDef \"%s_1\"\n", ident, check_name('s', block->getName()));
		waccel_plusident();

		fprintf(wfp,"%s(originalName \"%s\")\n", ident, check_name('s',block->getName()));
	}

	int pcnt = 0;
	fprintf(wfp,"%s(compHeader\n", ident);
	waccel_plusident();
	{
		for (int i=0;i<comppininstcnt;i++)
		{
			if (comppininstarray[i]->geomnum == block->getBlockNumber())
				pcnt++;
		}
		
		fprintf(wfp,"%s(sourceLibrary \"\")\n", ident);
		fprintf(wfp,"%s(numPins %d)\n", ident, pcnt);
		fprintf(wfp,"%s(numParts 1)\n", ident);
		fprintf(wfp,"%s(alts (ieeeAlt False) (deMorganAlt False))\n", ident);
		fprintf(wfp,"%s(refDesPrefix \"\")\n", ident);
	}
	waccel_minusident();
	fprintf(wfp,"%s)\n", ident);

	// physical / logical pin map
	int i=0;
	for (i=0; i<comppininstcnt; i++)
	{
		if (comppininstarray[i]->geomnum == block->getBlockNumber())
			fprintf(wfp,"%s(compPin \"%s\"(partNum 1) (symPinNum %d) (gateEq 0) (pinEq 0))\n",
			ident, comppininstarray[i]->pinname, comppininstarray[i]->index);
	}

	fprintf(wfp,"%s(attachedSymbol (partNum 1) (altType Normal) (symbolName \"%s\") )\n", ident, check_name('s', block->getName()));
	fprintf(wfp,"%s(attachedPattern (patternNum 1) (patternName \"%s\")\n", ident, check_name('s',block->getName()));
	waccel_plusident();
	{
		fprintf(wfp,"%s(numPads %d)\n", ident, pcnt);
		fprintf(wfp,"%s(padPinMap \n", ident);
		waccel_plusident();
		for (i=0;i<comppininstcnt;i++)
		{
			if (comppininstarray[i]->geomnum == block->getBlockNumber())
				fprintf(wfp,"%s(padNum %d) (compPinRef \"%s\")\n", ident, comppininstarray[i]->index, comppininstarray[i]->pinname);
		}
		waccel_minusident(); // pinPinMap
		fprintf(wfp,"%s)\n", ident);
	}
	waccel_minusident(); // attached pattern
	fprintf(wfp,"%s)\n", ident);

   if (type != NULL)
	{
		// add device attributes here
      if (type->getAttributesRef() != NULL)
      {
			for (POSITION pos = type->getAttributesRef()->GetStartPosition(); pos != NULL;)
		   {
			   WORD keyword;
				Attrib* attrib = NULL;
			   type->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);
   	        
			   if (attrib == NULL || keyword == doc->IsKeyWord(ATT_REFNAME, 0))
					continue;

				CString temp = get_attvalue_string(doc, attrib);
			   fprintf(wfp, "%s(attr \"%s\" \"%s\" (textStyleRef \"(Default)\"))\n", ident, 
						doc->getKeyWordArray()[keyword]->out, temp);
		   } 
      }
	}
			
	waccel_minusident();
	fprintf(wfp,"%s)\n", ident);
}

/*****************************************************************************/
/*
*/
static int  do_textdef(FILE *wfp, double scale)
{
   int   i;

   if (!textcnt)
   {
      ACCELTextStruct *t = new ACCELTextStruct;
      textarray.SetAtGrow(textcnt, t);
      textcnt++;
      int h = round(100);
      int w = round(10);
      t->name.Format("H%dW%d",h,w);
      t->height = 100.0/scale;
      t->penwidthindex = 0;
      // if no text was loaded, put in a default text, just in case
   }

	// (Default) text style definition
    fprintf(wfp,"%s(textStyleDef \"(Default)\"\n", ident);
    waccel_plusident();
    fprintf(wfp,"%s(font\n", ident);
    waccel_plusident();
    fprintf(wfp,"%s(fontType Stroke)\n", ident);
    fprintf(wfp,"%s(fontFamily SanSerif)\n", ident);
    fprintf(wfp,"%s(fontFace \"QUALITY\")\n", ident);
    fprintf(wfp,"%s(fontHeight 100.0)\n", ident);
    fprintf(wfp,"%s(strokeWidth 10.0)\n", ident);
    waccel_minusident();
    fprintf(wfp,"%s)\n", ident);
    waccel_minusident();
    fprintf(wfp,"%s)\n", ident);

	for (i=0;i<textcnt;i++)
   {
      fprintf(wfp,"%s(textStyleDef \"T:%s\"\n", ident, textarray[i]->name);
      waccel_plusident();
      fprintf(wfp,"%s(font\n", ident);
      waccel_plusident();
      fprintf(wfp,"%s(fontType Stroke)\n", ident);
      fprintf(wfp,"%s(fontFamily SanSerif)\n", ident);
      fprintf(wfp,"%s(fontFace \"QUALITY\")\n", ident);
      fprintf(wfp,"%s(fontHeight %1.*lf)\n", ident, output_units_accuracy, textarray[i]->height*scale);

      double penWidth = doc->getWidthTable()[textarray[i]->penwidthindex]->getSizeA()*scale;

      fprintf(wfp,"%s(strokeWidth %1.*lf)\n", ident, output_units_accuracy, penWidth);
      waccel_minusident();
      fprintf(wfp,"%s)\n", ident);

      fprintf(wfp,"%s(textStyleAllowTType False)\n", ident);
      fprintf(wfp,"%s(textStyleDisplayTType False)\n", ident);

      waccel_minusident();
      fprintf(wfp,"%s)\n", ident);
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int GetTextData(FileStruct *file, double scale)
{
   int   i;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      // textstyledef
      Accel_GetTextData(&(block->getDataList()),
                   0.0,0.0, 0.0, 0,  scale, -1, -1);
   }

   // textstyledef
   Accel_GetTextData(&(file->getBlock()->getDataList()),
                    file->getInsertX(), file->getInsertY(),
                    file->getRotation(), file->isMirrored(), file->getScale() * scale, -1, -1);

   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_library(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(library \"Library_1\"\n", ident);
   waccel_plusident();

   // paddef
   do_padstacks(wfp, scale);

   get_vianames(&(file->getBlock()->getDataList()));

   // viadef
   do_viadef(wfp, scale);

   GetTextData(file, scale);

   do_textdef(wfp, scale);

   // patterndef
   do_patterndef(wfp, scale);

   // compdef 
   do_compdef(wfp, file, scale);

   waccel_minusident();
   fprintf(wfp,"%s)\n\n", ident);

   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_layerdef(FILE *wfp)
{
   int   i, ii;
   int   maxstacknum = 0;

   for (i=0;i<maxArr;i++)
   {
      if (maxstacknum < accelArr[i]->stackNum)
         maxstacknum = accelArr[i]->stackNum;
   }

   for (ii=1;ii<=maxstacknum;ii++)  // order by stacknum
   {
      // layerdef
      for (i=0;i<maxArr;i++)
      {
         if (accelArr[i]->stackNum == ii)
         {
            fprintf(wfp,"%s(layerDef \"%s\"\n", ident, accelArr[i]->newName);
            waccel_plusident();
            fprintf(wfp,"%s(layerNum %d)\n", ident, accelArr[i]->stackNum);

            if (accelArr[i]->type == 'S')
            {
               fprintf(wfp,"%s(layerType Signal)\n", ident);

               fprintf(wfp,"%s(attr \"PadToPadClearance\" \"10.0mil\" (textStyleRef \"T:%s\") (constraintUnits mil) )\n", 
                     ident, textarray[0]->name);
               fprintf(wfp,"%s(attr \"PadToLineClearance\" \"10.0mil\" (textStyleRef \"T:%s\") (constraintUnits mil) )\n", 
                     ident, textarray[0]->name);
               fprintf(wfp,"%s(attr \"LineToLineClearance\" \"10.0mil\" (textStyleRef \"T:%s\") (constraintUnits mil) )\n", 
                     ident, textarray[0]->name);
               fprintf(wfp,"%s(attr \"ViaToPadClearance\" \"10.0mil\" (textStyleRef \"T:%s\") (constraintUnits mil) )\n", 
                     ident, textarray[0]->name);
               fprintf(wfp,"%s(attr \"ViaToLineClearance\" \"10.0mil\" (textStyleRef \"T:%s\") (constraintUnits mil) )\n", 
                     ident, textarray[0]->name);
               fprintf(wfp,"%s(attr \"ViaToViaClearance\" \"10.0mil\" (textStyleRef \"T:%s\") (constraintUnits mil) )\n", 
                     ident, textarray[0]->name);
            }
            else
            if (accelArr[i]->type == 'P')
               fprintf(wfp,"%s(layerType Plane)\n", ident);
            else
               fprintf(wfp,"%s(layerType NonSignal)\n", ident);

            // here write netnameref
            LayerStruct *lp = doc->FindLayer_by_Name(accelArr[i]->oldName);
            if (lp)
            {
               Attrib *a =  is_attvalue(doc, lp->getAttributesRef(), LAYATT_NETNAME, 2);
               if (a)
               {
                  CString l;
                  l = get_attvalue_string(doc, a); // can have multiple args

                  // can have multiple names
                  CString n;
                  char  *lp, tmp[1000];
                  strcpy(tmp,l);
                  if (lp = strtok(tmp,"\n"))
                  {
                     if (accelArr[i]->signr == 0) 
                        fprintf(wfp,"%s(netNameRef \"%s\")\n", ident, check_name('n', lp));
                  
                     while (lp = strtok(NULL,"\n"))
                     {
                        fprintf(flog, "ACCEL does not allow additional nets [%s] on Layer [%s]\n",
                           check_name('n', lp), accelArr[i]->newName);
                        display_error++;
                     }
                  }
               }
            }

            waccel_minusident();
            fprintf(wfp,"%s)\n\n", ident);
            break;   // only 1 stackup layer allowed
         }
      }
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_multilayer(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(multiLayer\n", ident);
   waccel_plusident();

   Accel_WritePCBInsertData(wfp, &(file->getBlock()->getDataList()), file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0,-1);
   waccel_minusident();
   fprintf(wfp,"%s)\n", ident);
   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_graphicmultilayer(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(multiLayer\n", ident);
   waccel_plusident();

   Accel_WriteInsertData(wfp, &(file->getBlock()->getDataList()), file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0,-1);
   waccel_minusident();
   fprintf(wfp,"%s)\n", ident);
   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_pcbdesignheader(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(pcbDesignHeader\n",ident);
   waccel_plusident();

   doc->validateBlockExtents(file->getBlock());

   double xmax = (file->getBlock()->getXmax() * scale);
   double ymax = (file->getBlock()->getYmax() * scale);

   if (file->getBlock()->getXmin() < 0)   xmax -= (file->getBlock()->getXmin() * scale);
   if (file->getBlock()->getYmin() < 0)   ymax -= (file->getBlock()->getYmin() * scale);

   // if the origin is moved !
   if (file->getInsertX() > 0)          xmax += (file->getInsertX() * scale);
   if (file->getInsertY() > 0)          ymax += (file->getInsertY() * scale); 

   xmax = xmax + WORKSPACE_ENLARGE;
   ymax = ymax + WORKSPACE_ENLARGE;

   fprintf(wfp,"%s(workspaceSize %1.*lf %1.*lf)\n", ident, output_units_accuracy, xmax, output_units_accuracy, ymax);
   fprintf(wfp,"%s(gridDfns\n", ident);
   waccel_plusident();
   fprintf(wfp,"%s(grid \"100.0mil\")\n", ident);
   waccel_minusident(); // fieldSet
   fprintf(wfp,"%s)\n\n", ident);

   fprintf(wfp,"%s(designInfo\n", ident);
   waccel_plusident();

   fprintf(wfp,"%s(fieldSet \"(Default)\"\n", ident);
   waccel_plusident();

   fprintf(wfp,"%s(fieldDef \"Date\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Time\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Author\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Revision\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Title\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Approved By\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Checked By\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Company Name\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Drawing Number\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Drawn By\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Engineer\" \"\")\n", ident);

   waccel_minusident(); // fieldSet
   fprintf(wfp,"%s)\n\n", ident);
   
   waccel_minusident(); // designInfo
   fprintf(wfp,"%s)\n\n", ident);

   fprintf(wfp,"%s(solderSwell 10.0)\n", ident);
   fprintf(wfp,"%s(pasteSwell 0.0)\n", ident);
   fprintf(wfp,"%s(planeSwell 10.0)\n", ident);

   waccel_minusident(); // pcbdesignheader
   fprintf(wfp,"%s)\n\n", ident);

   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_layercontents(FILE *wfp, FileStruct *file, double scale)
{
   Accel_WriteData(wfp, &(file->getBlock()->getDataList()), file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0,-1, TRUE, FALSE);
   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_pcbdesign(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(pcbDesign \"PcbDesign_1\"\n", ident);
   waccel_plusident();

   // pcbdesignheader
   do_pcbdesignheader(wfp,file,scale);

   // layerdef
   do_layerdef(wfp);

   // multilayer
   do_multilayer(wfp, file, scale);

   // layer contents
   do_layercontents(wfp, file, scale);

   // pcbprintsettings
   // drillsymsettings
   // gerbersettings
   // ncdrillsettings
   // programstate
   // layersets
   // reportsettings

   waccel_minusident();
   fprintf(wfp,"%s)\n\n", ident);

   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_graphicdesign(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(pcbDesign \"PcbDesign_1\"\n", ident);
   waccel_plusident();

   // pcbdesignheader
   do_pcbdesignheader(wfp,file,scale);

   // layerdef
   do_layerdef(wfp);

   // multilayer
   do_graphicmultilayer(wfp, file, scale);

   // layer contents
   do_layercontents(wfp, file, scale);

   // pcbprintsettings
   // drillsymsettings
   // gerbersettings
   // ncdrillsettings
   // programstate
   // layersets
   // reportsettings

   waccel_minusident();
   fprintf(wfp,"%s)\n\n", ident);

   return 1;
}

//--------------------------------------------------------------
void Accel_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits)
{
   FileStruct* file;
   double scale;

   doc = Doc;
   format = Format;
   display_error = FALSE;
   ident = ""; 
   identcnt = 0;
   outside_koo = FALSE;

   textarray.SetSize(10,10);
   textcnt = 0;

   shapepininstarray.SetSize(10,10);
   shapepininstcnt = 0;

   comppininstarray.SetSize(10,10);
   comppininstcnt = 0;

   padstackarray.SetSize(10,10);
   padstackcnt = 0;

   // open file for writting
   FILE  *wfp;
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString tmp;

      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }
   
	CString logFile = GetLogfilePath("accel.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString settingsFile( getApp().getExportSettingsFilePath("ACCEL.out") );
   load_ACCELsettings(settingsFile);

   CString checkFile( getApp().getSystemSettingsFilePath("ACCEL.chk") );
   check_init(checkFile);
   check_addbackslash(TRUE);

   fprintf(wfp,"ACCEL_ASCII \"%s\"\n\n", filename);

   fprintf(wfp,"%s(asciiHeader\n", ident);
   waccel_plusident();

   fprintf(wfp,"%s(asciiVersion 3 0)\n", ident);

   CTime time = CTime::GetCurrentTime();

   fprintf(wfp,"%s(timeStamp %d %d %d %d %d %d)\n", ident,
      time.GetYear(), time.GetMonth(), time.GetDay(), 
      time.GetHour(), time.GetMinute(), time.GetSecond());

   //(program "ACCEL P-CAD PCB" "13.00.38")
   //(copyright "Copyright © 1997 ACCEL Technologies, Inc.")
   fprintf(wfp,"%s(fileAuthor \"%s\")\n", ident, getApp().getCamCadTitle());
   fprintf(wfp,"%s(fileUnits Mil)\n", ident);
   scale = Units_Factor(pageUnits, UNIT_MILS);
   output_units_accuracy = 3;

   waccel_minusident();
   fprintf(wfp,"%s)\n\n", ident);


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

         do_library(wfp, file, scale);

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            Accel_CheckPCBInsertData(file, &(file->getBlock()->getDataList()));
				do_netlist(wfp, file, scale);
            do_pcbdesign(wfp, file, scale);
         }
         else
         {
            // graphic
            do_graphicdesign(wfp, file, scale);
         }
      } // edit_layer
   }

   free_layerlist();

	int i=0;
   for (i=0;i<textcnt;i++)
   {
      delete textarray[i];
   }
   textcnt = 0;
   textarray.RemoveAll();

   // it is deleted in patterdef
   shapepininstarray.RemoveAll();

   for (i=0;i<comppininstcnt;i++)
   {
      delete comppininstarray[i];
   }
   comppininstarray.RemoveAll();
   comppininstcnt = 0;

   for (i=0;i<padstackcnt;i++)
   {
      delete padstackarray[i];
   }
   padstackarray.RemoveAll();
   padstackcnt = 0;

   fclose(wfp);

   if (outside_koo)
   {
      CString  tmp;
      tmp.Format("Accel does not allow coordinates and items outside of +XY coordinates. Please move the Origin and rerun again!");
      ErrorMessage(tmp,"ACCEL, PCAD200x Error");
   }

   if (check_report(flog)) display_error++;
   check_deinit();

   fprintf(flog,"Please check the Clearance Settings after loading into ACCEL, PCAD200x\n");
   display_error++;

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*
   return -1 if not visible
*/
static int Layer_ACCEL(int l)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (accelArr[i]->on == 0)  continue;   // not visible
      if (accelArr[i]->layerindex == l)
         return i;
   }

   fprintf(flog,"Layer [%s] not translated.\n", doc->getLayerArray()[l]->getName());
   return -1;
}

/*****************************************************************************/
/*
*/
static CString clean_text(const char *t)
{
   CString  tt;

   for (int i=0;i<(int)strlen(t);i++)
   {
      if (t[i] == '\\') tt += '\\';
      if (t[i] == '"')  tt += '\\';
      tt += t[i];
   }

   return tt;
}

/*****************************************************************************/
/*
*/
static int  add_text( FILE *fp, const char *text, double x, double y,
                      const char *textfont, double rot, int text_mirror)
{
   double r = RadToDeg(rot);
   CString strVal = clean_text(text);

   //if (strVal.Find("\"") >0)
   strVal.Replace("\n", "\\r\\n\n");

   if (text_mirror)
      r = 360 - r;

   while (r < 0)     r += 360;
   while (r >= 360)  r -= 360;

   fprintf(fp, "%s(text (pt %1.*lf %1.*lf) \"%s\" (textStyleRef \"T:%s\")(rotation %1.1lf)(isFlipped %s))\n",
       ident, output_units_accuracy, x, output_units_accuracy, y, strVal, textfont, r,
       (text_mirror)?"TRUE":"FALSE");

   return 1;
}

/*****************************************************************************/
/*
   Add polyline must be changed to include voids

*/
static int  add_boundary(FILE *fp, Point2 *points, int cnt)
{
   int      i;
   double   oldx = FLT_MAX, oldy = FLT_MAX;
   int      written = 0;

   fprintf(fp,"%s(pourOutline\n", ident);
   waccel_plusident();

   for (i=0;i<cnt-1;i++)   // do not write the last entity
   {
      double da = atan(points[i].bulge) * 4;

      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         double x1,y1,x2,y2;   

         x1 = points[i].x;
         y1 = points[i].y;
         x2 = points[i+1].x;
         y2 = points[i+1].y;
         ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         // here arc to poly
         int ii, ppolycnt = 255; // only for an arc 
         // start center
         Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));

         if (ppoly == NULL)
         {
            ErrorMessage("Memory allocation failed for creation of point.", "Out Of Memory");
         }
         else
         {
            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));
   
            for (ii=0;ii<ppolycnt;ii++)
            {
               if (fabs(ppoly[ii].x - oldx) > ACCEL_SMALLNUMBER || fabs(ppoly[ii].y - oldy) > ACCEL_SMALLNUMBER)
               {
                  fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
                     output_units_accuracy, ppoly[ii].x, output_units_accuracy, ppoly[ii].y);
                  oldx = ppoly[ii].x;
                  oldy = ppoly[ii].y;
                  written++;
               }
            }
            free(ppoly);
         }
      }
      else
      {
         if (fabs(points[i].x - oldx) > ACCEL_SMALLNUMBER || fabs(points[i].y - oldy) > ACCEL_SMALLNUMBER)
         {
            fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
               output_units_accuracy, points[i].x, output_units_accuracy, points[i].y);
            oldx = points[i].x;
            oldy = points[i].y;
            written++;
         }
      }
   }

   waccel_minusident(); // copperpour95
   fprintf(fp,"%s)\n", ident);

   return written; 
}

/*****************************************************************************/
/*
   Add polyline must be changed to include voids

*/
static int  add_cutOutline(FILE *fp, Point2 *points, int cnt)
{
   int      i;
   double   oldx = FLT_MAX, oldy = FLT_MAX;
   int      written = 0;
   
   fprintf(fp,"%s(cutout\n", ident);
   waccel_plusident();

   fprintf(fp,"%s(cutoutOutline\n", ident);
   waccel_plusident();

   for (i=0;i<cnt-1;i++)   // do not write the last entity
   {
      double da = atan(points[i].bulge) * 4;

      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         double x1,y1,x2,y2;   

         x1 = points[i].x;
         y1 = points[i].y;
         x2 = points[i+1].x;
         y2 = points[i+1].y;
         ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         // here arc to poly
         int ii, ppolycnt = 255; // only for an arc 
         // start center
         Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));

         if (ppoly == NULL)
         {
            ErrorMessage("Memory allocation failed for creation of point.", "Out Of Memory");
         }
         else
         {
            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));
   
            for (ii=0;ii<ppolycnt;ii++)
            {
               if (fabs(ppoly[ii].x - oldx) > ACCEL_SMALLNUMBER || fabs(ppoly[ii].y - oldy) > ACCEL_SMALLNUMBER)
               {
                  fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
                     output_units_accuracy, ppoly[ii].x, output_units_accuracy, ppoly[ii].y);
                  oldx = ppoly[ii].x;
                  oldy = ppoly[ii].y;
                  written++;
               }
            }
            free(ppoly);
         }
      }
      else
      {
         if (fabs(points[i].x - oldx) > ACCEL_SMALLNUMBER || fabs(points[i].y - oldy) > ACCEL_SMALLNUMBER)
         {
            fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
               output_units_accuracy, points[i].x, output_units_accuracy, points[i].y);
            oldx = points[i].x;
            oldy = points[i].y;
            written++;
         }
      }
   }

   waccel_minusident(); // cutoutline
   fprintf(fp,"%s)\n", ident);

   waccel_minusident(); // cutout
   fprintf(fp,"%s)\n", ident);

   return written; 
}

/*****************************************************************************/
/*

*/
static int  add_poly(FILE *fp, Point2 *points, int cnt)
{
   int      i;
   double   oldx = FLT_MAX, oldy = FLT_MAX;
   int      written = 0;

   fprintf(fp,"%s(pcbPoly\n", ident);
   waccel_plusident();

   for (i=0;i<cnt-1;i++)   // do not write the last entity
   {
      double da = atan(points[i].bulge) * 4;

      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         double x1,y1,x2,y2;   

         x1 = points[i].x;
         y1 = points[i].y;
         x2 = points[i+1].x;
         y2 = points[i+1].y;
         ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         // here arc to poly
         int ii, ppolycnt = 255; // only for an arc 
         // start center
         Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));

         if (ppoly == NULL)
         {
            ErrorMessage("Memory allocation failed for creation of point.", "Out Of Memory");
         }
         else
         {
            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));
   
            for (ii=0;ii<ppolycnt;ii++)
            {
               if (fabs(ppoly[ii].x - oldx) > ACCEL_SMALLNUMBER || fabs(ppoly[ii].y - oldy) > ACCEL_SMALLNUMBER)
               {
                  fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
                     output_units_accuracy, ppoly[ii].x, output_units_accuracy, ppoly[ii].y);
                  written++;
                  oldx = ppoly[ii].x;
                  oldy = ppoly[ii].y;
               }
            }
            free(ppoly);
         }
      }
      else
      {
         if (fabs(points[i].x - oldx) > ACCEL_SMALLNUMBER || fabs(points[i].y - oldy) > ACCEL_SMALLNUMBER)
         {
            fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
               output_units_accuracy, points[i].x, output_units_accuracy, points[i].y);
            oldx = points[i].x;
            oldy = points[i].y;
            written++;
         }
      }
   }

   waccel_minusident(); // pcbPoly
   fprintf(fp,"%s)\n", ident);

   return written; 
}

/*****************************************************************************/
/*
*/
static int  add_islandOutline(FILE *fp, Point2 *points, int cnt)
{
   int      i;
   double   oldx = FLT_MAX, oldy = FLT_MAX;
   int      written = 0;

   fprintf(fp,"%s(islandOutline\n", ident);
   waccel_plusident();

   for (i=0;i<cnt-1;i++)   // do not write the last entity
   {
      double da = atan(points[i].bulge) * 4;

      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         double x1,y1,x2,y2;   

         x1 = points[i].x;
         y1 = points[i].y;
         x2 = points[i+1].x;
         y2 = points[i+1].y;
         ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         // here arc to poly
         int ii, ppolycnt = 255; // only for an arc 
         // start center
         Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));

         if (ppoly == NULL)
         {
            ErrorMessage("Memory allocation failed for creation of point.", "Out Of Memory");
         }
         else
         {
            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));
   
            for (ii=0;ii<ppolycnt;ii++)
            {
               if (fabs(ppoly[ii].x - oldx) > ACCEL_SMALLNUMBER || fabs(ppoly[ii].y - oldy) > ACCEL_SMALLNUMBER)
               {
                  fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
                     output_units_accuracy, ppoly[ii].x, output_units_accuracy, ppoly[ii].y);
                  written++;
                  oldx = ppoly[ii].x;
                  oldy = ppoly[ii].y;
               }
            }
            free(ppoly);
         }
      }
      else
      {
         if (fabs(points[i].x - oldx) > ACCEL_SMALLNUMBER || fabs(points[i].y - oldy) > ACCEL_SMALLNUMBER)
         {
            fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
               output_units_accuracy, points[i].x, output_units_accuracy, points[i].y);
            written++;
            oldx = points[i].x;
            oldy = points[i].y;
         }
      }
   }

   waccel_minusident(); // island outline
   fprintf(fp,"%s)\n", ident);

   return written; 
}

/*****************************************************************************/
/*
   Add polyline must be changed to include voids

*/
static int  add_polyline(FILE *fp, Point2 *points, int cnt, 
                         int polyFilled, int closed, int voidout, double lineWidth,
                         const char *netname)
{
   int   i;
   int   written = 0;

   for (i=0;i<cnt-1;i++)   // do not write the last entity, because line is written from i to i+1
   {

      double da = atan(points[i].bulge) * 4;

      if (fabs(da) > BULGE_THRESHOLD)
      {
         double cx,cy,r,sa;
         ArcPoint2Angle(points[i].x,points[i].y,points[i+1].x, points[i+1].y ,da,&cx,&cy,&r,&sa);

         if (da < 0)
         {
            sa += da;
            da = -da;
         }

         // convert to degree
         sa = RadToDeg(sa);
         
         while (sa < 0) sa += 360;
         while (sa >= 360) sa -= 360;

         da = RadToDeg(da);

         if (r > 0 && lineWidth > 0)
         {

            fprintf(fp,"%s(arc (pt %1.*lf %1.*lf) (radius %1.*lf) (startAngle %1.1lf) (sweepAngle %1.1lf)(width %1.*lf)", ident,
               output_units_accuracy, cx, output_units_accuracy, cy, 
               output_units_accuracy, r, sa, da, output_units_accuracy, lineWidth);
            written++;
            if (strlen(netname))
               fprintf(fp,"(netNameRef \"%s\")", netname); 
            fprintf(fp,")\n");

         }
         else
         {
            if (lineWidth == 0.0 && r > 0)  // if arc has zero width then change arc to poly
            {
               int cnt = 255;
               Point2 *poly = (Point2 *)calloc(255,sizeof(Point2));

               if (poly == NULL)
               {
                  ErrorMessage("Memory allocation failed for creation of point2.", "Out Of Memory");
               }
               else
               {
                  arc2poly(DegToRad(sa), DegToRad(da), DegToRad(ARC_ANGLE_DEGREE), cx, cy, r, poly, &cnt);
   
                  if (cnt > 0) 
                  {
                     for (int i=0; i < cnt-1; i++)
                     {
                        fprintf(fp,"%s(line (pt %1.*lf %1.*lf) (pt %1.*lf %1.*lf) (width %1.*lf)", ident,
                           output_units_accuracy, poly[i].x, output_units_accuracy, poly[i].y,  
                           output_units_accuracy, poly[i+1].x, output_units_accuracy, poly[i+1].y,
                           output_units_accuracy, lineWidth); 
                        written++;
                        if (strlen(netname))
                           fprintf(fp,"(netNameRef \"%s\")", netname);
                        fprintf(fp,")\n");
                     }
                  }
                  free (poly);
               }
            }
            else  // if arc has a zero radius then write a line with the arc center point.  This will make sure
                  // to have contents under layer contents.
            {
               fprintf(fp,"%s(line (pt %1.*lf %1.*lf) (pt %1.*lf %1.*lf) (width %1.*lf)", ident,
                  output_units_accuracy, cx, output_units_accuracy, cy, 
                  output_units_accuracy, cx, output_units_accuracy, cy,
                  output_units_accuracy, lineWidth);
               written++;
               if (strlen(netname))
                  fprintf(fp,"(netNameRef \"%s\")", netname); 
               fprintf(fp,")\n"); 
            }
         }           
      }
      else
      {
         fprintf(fp,"%s(line (pt %1.*lf %1.*lf) (pt %1.*lf %1.*lf) (width %1.*lf)", ident,
               output_units_accuracy, points[i].x, output_units_accuracy, points[i].y, 
               output_units_accuracy, points[i+1].x, output_units_accuracy, points[i+1].y,
               output_units_accuracy, lineWidth);
         written++;
         if (strlen(netname))
            fprintf(fp,"(netNameRef \"%s\")", netname); 
         fprintf(fp,")\n"); 
      }
   
   }

   // layercontens is never be allowed to be empty !!!

   return written; 
}

//--------------------------------------------------------------
void Accel_WriteData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int negative_check, int patterndef)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         lindex = -1;
   int         layer;
   CString     netname;

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

         if (doc->get_layer_visible(layer, mirror) && (lindex = Layer_ACCEL(layer)) >= 0 )
         {
            Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
            if (a)
               netname = check_name('n', get_attvalue_string(doc, a));
            else
               netname = "";
         }
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            if ( lindex >= 0 )
               fprintf(wfp,"%s(layerContents (layerNumRef %d)\n", ident, accelArr[lindex]->stackNum);
            waccel_plusident();

            int   boundary = 0, filled = 0, voided = 0;
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (!poly->isHidden())
               {
                  if (poly->isFloodBoundary()) boundary++;
                  if (poly->isFilled())       filled++;
                  if (poly->isVoid())     voided++;
               }
            }

            if (patterndef)   // this is not allowed in a patterndef
            {
               boundary = 0;
               voided = 0;
            }
            // POLY - normal filled area on all layers - can not have a netname

            if (boundary || voided || (filled && strlen(netname)))
            {
               // copperpour5 pour and voids on all layers
               fprintf(wfp,"%s(copperPour95\n", ident);
               waccel_plusident();

               double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;

               fprintf(wfp,"%s(pourType SolidPour)\n", ident);
               if (strlen(netname))
                 fprintf(wfp,"%s(netNameRef \"%s\")\n", ident, netname);

               if (lineWidth < 1)   lineWidth = 1;
               fprintf(wfp,"%s(width %1.*lf)\n", ident, output_units_accuracy, lineWidth);
               fprintf(wfp,"%s(pourSpacing 50.0)\n", ident);
               fprintf(wfp,"%s(pourBackoff 12.0)\n", ident);
               fprintf(wfp,"%s(useDesignRules True)\n", ident);
               fprintf(wfp,"%s(pourSmoothness 1)\n", ident);
               fprintf(wfp,"%s(thermalType Therm45)\n", ident);
               fprintf(wfp,"%s(thermalWidth %1.*lf)\n", ident, output_units_accuracy, lineWidth);
               fprintf(wfp,"%s(thermalSpokes 4)\n", ident);
               fprintf(wfp,"%s(islandRemoval (area 4.064e+006))\n", ident);

               int   island_open = FALSE;

               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);

                  if ( !poly->isHidden() && !poly->isThermalLine())
                  {

                     Point2   p;
                  
                     int vertexcnt = 0;

                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        vertexcnt++;
                     }        

                     Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1

                     if (points == NULL)
                     {
                        ErrorMessage("Memory allocation failed for creation of point2.", "Out Of Memory");
                     }
                     else
                     {
                        double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
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

                           if (negative_check)
                           {
                              if (points[vertexcnt].x < 0)
                                 outside_koo = TRUE;
                              if (points[vertexcnt].y < 0)
                                 outside_koo = TRUE;
                           }
                           vertexcnt++;
                        }

                        if (poly->isFloodBoundary())
                        {
                           if (add_boundary(wfp, points, vertexcnt) < 3)
                           {
                              fprintf(flog,"boundary [%ld] write error!\n", np->getEntityNumber());
                              display_error++;
                           }
                        }
                        else
                        if (poly->isVoid())
                        {

                           if (!island_open)
                           {
                              fprintf(wfp,"%s(island\n", ident);
                              waccel_plusident();
                              island_open = TRUE;
                           }
                           if (add_cutOutline(wfp, points, vertexcnt) < 3)
                           {
                              fprintf(flog,"cutOutline [%ld] write error!\n", np->getEntityNumber());
                              display_error++;
                           }
                        }
                        else
                        {
                           if (!boundary)
                           {
                              if (add_boundary(wfp, points, vertexcnt) < 3)
                              {
                                 fprintf(flog,"boundary [%ld] write error!\n", np->getEntityNumber());
                                 display_error++;
                              }
                              boundary++; 
                           }

                           if (island_open) // start a new island for each fill.
                           {
                              waccel_minusident(); // island
                              fprintf(wfp,"%s)\n", ident);
                           }

                           fprintf(wfp,"%s(island\n", ident);
                           waccel_plusident();
                           island_open = TRUE;
                           if (add_islandOutline(wfp, points, vertexcnt) < 3)
                           {
                              fprintf(flog,"islandOutline [%ld] write error!\n", np->getEntityNumber());
                              display_error++;
                           }
                        }
                        free(points);
                     }
                  }
               }

               Point2   p2, p;
               // here thermolines 
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);

                  if (poly->isThermalLine())
                  {     
                     int      first = TRUE;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        p.x = pnt->x * scale;
                        if (mirror) p.x = -p.x;
                        p.y = pnt->y * scale;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);

                        if (first)
                        {
                           p2 = p;
                           first = FALSE;
                        }
                        else
                        {
                           fprintf(wfp,"%s(thermal (pt %1.*lf %1.*lf) (pt %1.*lf %1.*lf))\n", ident, 
                              output_units_accuracy, p2.x, output_units_accuracy, p2.y,
                              output_units_accuracy, p.x, output_units_accuracy, p.y);
                        }
                     }
                  }
               }

               if(island_open)
               {
                  waccel_minusident(); // island
                  fprintf(wfp,"%s)\n", ident);
               }

               waccel_minusident(); // copperpour95
               fprintf(wfp,"%s)\n", ident);
            }
            else
            if (filled)
            {
               // loop thru polys
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);

                  if (!poly->isHidden())
                  {
                     BOOL polyFilled = poly->isFilled();
                     BOOL closed     = poly->isClosed();
                     BOOL voided     = poly->isVoid();

                     Point2   p;
                     
                     int vertexcnt = 0;

                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        vertexcnt++;
                     }        
                     Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1

                     if (points == NULL)
                     {
                        ErrorMessage("Memory allocation failed for creation of point2.", "Out Of Memory");
                     }
                     else
                     {
                        double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
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

                           if (negative_check)
                           {
                              if (points[vertexcnt].x < 0)
                                 outside_koo = TRUE;
                              if (points[vertexcnt].y < 0)
                                 outside_koo = TRUE;
                           }
                           vertexcnt++;
                        }
                        if (add_poly(wfp, points, vertexcnt) < 3)
                        {
                           fprintf(flog,"Poly write error!\n");
                           display_error++;
                        }
                        free(points);
                     }
                  }
               }
            }
            else
            {
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
                  
                  int vertexcnt = 0;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     vertexcnt++;
                  }        
                  Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1

                  if (points == NULL)
                  {
                     ErrorMessage("Memory allocation failed for creation of point2.", "Out Of Memory");
                  }
                  else
                  {
                     double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
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

                        if (negative_check)
                        {
                           if (points[vertexcnt].x < 0)
                              outside_koo = TRUE;
                           if (points[vertexcnt].y < 0)
                              outside_koo = TRUE;
                        }
                        vertexcnt++;
                     }
                     if (!add_polyline(wfp, points, vertexcnt, polyFilled, closed, voided, lineWidth, netname))
                     {
                        // layercontens is not allowed to be empty !!!
                        fprintf(flog,"Poly write error!\n");
                        display_error++;
                     }
                     free(points);
                  }
               }
            }  // if normal or copper95
            waccel_minusident(); // layercontents
            fprintf(wfp,"%s)\n", ident);
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

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();

            int ptr = update_textarray(np->getText()->getHeight(), np->getText()->getPenWidthIndex(), scale);

            if (lindex >= 0)
               fprintf(wfp,"%s(layerContents (layerNumRef %d)\n", ident, accelArr[lindex]->stackNum);
            waccel_plusident();

            add_text(wfp, np->getText()->getText(), point2.x, point2.y, textarray[ptr]->name, text_rot, text_mirror);
            waccel_minusident();
            fprintf(wfp,"%s)\n", ident);
         }
         break;
         
         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...
            if (np->getInsert()->getInsertType())
            {
               if (np->getInsert()->getInsertType() != INSERTTYPE_MECHCOMPONENT && 
                   np->getInsert()->getInsertType() != INSERTTYPE_GENERICCOMPONENT)  break;
            }

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

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
                  {
                     wpfw_Graph_Level(lname);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle());
                  }
*/
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               Accel_WriteData(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, negative_check, patterndef);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end Accel_WriteData */

//--------------------------------------------------------------
void Accel_WriteInsertData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   POSITION pos;
   int   lindex;
   int   layer;

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
         if ((lindex = Layer_ACCEL(layer)) < 0) continue;
      }

      switch(np->getDataType())
      {
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

            if (block->getBlockType() == BLOCKTYPE_PADSTACK || block->getBlockType() == BLOCKTYPE_TESTPAD)
            {
               fprintf(wfp,"%s(pad (padNum 0) (padStyleRef \"%s\")(pt %1.*lf %1.*lf) )\n",
                  ident, block->getName(), 
                  output_units_accuracy, point2.x,  output_units_accuracy, point2.y);
            }
            else
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
                  {
                     wpfw_Graph_Level(lname);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle());
                  }
*/
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;
               Accel_WriteInsertData(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end Accel_WriteInsertData */

/******************************************************************************
* Accel_CheckPCBInsertData
*/
void Accel_CheckPCBInsertData(FileStruct *file, CDataList *DataList)
{
	int typeKw = doc->getStandardAttributeKeywordIndex(standardAttributeTypeListLink);

	for (CDataListIterator dataListIterator(*DataList, insertTypePcbComponent); dataListIterator.hasNext();)
	{
		DataStruct* data = dataListIterator.getNext();
		BlockStruct* block = doc->getBlockAt(data->getInsert()->getBlockNumber());
		if (block == NULL || block->getBlockType() != blockTypePcbComponent)
			continue;

		CString pattern = check_name('s', block->getName());
		Attrib* attrib = NULL;

      if (data->getAttributes() != NULL && data->getAttributes()->Lookup((WORD)typeKw, attrib))
		{
			CString typeName = attrib->getStringValue();

			for (POSITION pos = file->getTypeList().GetHeadPosition(); pos != NULL;)
			{
				TypeStruct* type = file->getTypeList().GetNext(pos);
            
            if (type != NULL && type->getName().CompareNoCase(typeName) == 0 && type->getBlockNumber() != block->getBlockNumber())
				{
					CString newTypeName;
					newTypeName.Format("%s_%s", type->getName(), pattern);
					
					TypeStruct* newType = AddType(file, newTypeName);
					newType->setBlockNumber(block->getBlockNumber());
					doc->CopyAttribs(&newType->getAttributesRef(), type->getAttributesRef());

		         data->setAttrib(doc->getCamCadData(), typeKw, valueTypeString, newTypeName.GetBuffer(0), attributeUpdateOverwrite, NULL);
				}
			}
		}
		else
		{
			for (POSITION pos = file->getTypeList().GetHeadPosition(); pos != NULL;)
			{
				TypeStruct* type = file->getTypeList().GetNext(pos);

				if (type != NULL && type->getBlockNumber() == block->getBlockNumber())
				{
					CString typeName = type->getName();
					data->setAttrib(doc->getCamCadData(), typeKw, valueTypeString, typeName.GetBuffer(0), attributeUpdateOverwrite, NULL);            }
			}
		}
	}
}

void Accel_WritePCBInsertData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   POSITION pos;
   int   lindex;
   int   layer;

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
         if ((lindex = Layer_ACCEL(layer)) < 0) continue;
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)  break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;

            if (mirror) point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (block_mirror)
               block_rot = rotation + PI2- np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT && block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)  
            {
               double r = RadToDeg(block_rot);
               while (r < 0)     r +=360;
               while (r >=360)   r -=360;

               if(point2.x < 0)  
                  outside_koo = TRUE;

               if(point2.y < 0)  
                  outside_koo = TRUE;
   
               CString  pattern, refname;
               pattern = check_name('s', block->getName());
               refname = check_name('c', np->getInsert()->getRefname());

               fprintf(wfp,"%s(pattern (patternRef \"%s_1\") (refDesRef \"%s\") (pt %1.*lf %1.*lf) (rotation %1.1lf)(isFlipped %s)\n",
                  ident, pattern, refname,
                  output_units_accuracy,point2.x,  output_units_accuracy, point2.y, r,
                  (block_mirror)?"TRUE":"FALSE");

               // here (attr "RefDes" "" ....)
               Attrib* attrib;

               if (attrib = is_attvalue(doc, np->getAttributesRef(), ATT_REFNAME, 1))
               {
                  int ptr = update_textarray(attrib->getHeight(), attrib->getPenWidthIndex(), scale);

                  if (attrib->isVisible())
                  {
                     fprintf(wfp,"%s(attr \"RefDes\" \"\" (pt %1.*lf %1.*lf)(isVisible True) (textStyleRef \"T:%s\"))\n",
                        ident, output_units_accuracy, attrib->getX()*scale, output_units_accuracy, attrib->getY()*scale, 
                        textarray[ptr]->name);
                  }
               }

               fprintf(wfp,"%s)\n", ident);
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)  
            {
               fprintf(wfp,"%s(pad (padNum 0) (padStyleRef \"%s\")(pt %1.*lf %1.*lf) )\n",
                  ident, block->getName(), 
                  output_units_accuracy,point2.x,  output_units_accuracy, point2.y);
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)   
            {
               if(point2.x < 0)  
                  outside_koo = TRUE;

               if(point2.y < 0)
                  outside_koo = TRUE;

               fprintf(wfp,"%s(via (viaStyleRef \"%s\")(pt %1.*lf %1.*lf)",
                  ident, check_name('p',block->getName()), 
                  output_units_accuracy,point2.x,  output_units_accuracy, point2.y);

               Attrib* attrib =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

               if (attrib != NULL)
               {
                  CString  netname;
                  netname = check_name('n', get_attvalue_string(doc, attrib));
                  fprintf(wfp,"(netNameRef \"%s\")", netname);
               }

               fprintf(wfp,")\n");
            }
            else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
                  {
                     wpfw_Graph_Level(lname);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle());
                  }
*/
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer = insertLayer;

               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               Accel_WritePCBInsertData(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer);

            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end Accel_WritePCBInsertData */

//--------------------------------------------------------------
void Accel_WriteCompInstData(FILE *wfp, CDataList *DataList)
{
   DataStruct *np;

   for (POSITION pos = DataList->GetHeadPosition();pos != NULL;)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_INSERT)        
      {
         switch(np->getDataType())
         {
         case T_INSERT:
            {
               if (np->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)   break;

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
               if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
               {
                  fprintf(flog,"Component [%s] and a non electrical pattern [%s] -> ignored\n",
                     np->getInsert()->getRefname(), block->getName());
                  display_error++;
                  break;
               }
			
			      CString DeviceName;
			      Attrib *device;

			      if (device =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
				      DeviceName = STRDUP(get_attvalue_string(doc, device));
			      else
				      DeviceName = block->getName();

                  // do compinstance
                  fprintf(wfp,"%s(compInst \"%s\"\n", ident, check_name('c', np->getInsert()->getRefname()));
                  waccel_plusident();
                  fprintf(wfp,"%s(compRef \"%s_1\")\n", ident, check_name('s', DeviceName));
                  fprintf(wfp,"%s(originalName \"%s\")\n", ident, check_name('s', DeviceName));
                  fprintf(wfp,"%s(compValue \"{Value}\")\n", ident);

			      // Add comp instance attributes here
			      CAttributes* map;
			      map = np->getAttributesRef();

			      for (POSITION pos = map->GetStartPosition();pos != NULL;)
			      {
				      WORD keyword;
				      Attrib* attrib;

				      map->GetNextAssoc(pos, keyword, attrib);
   	            
                  if (keyword != doc->IsKeyWord(ATT_REFNAME, 0))
                  {
			   	      char *temp = STRDUP(get_attvalue_string(doc, attrib));

		   		      fprintf(wfp,"%s(attr \"%s\" \"%s\" (textStyleRef \"(Default)\"))\n", ident, 
	   				      doc->getKeyWordArray()[keyword]->out, temp);
   
                     free (temp);
                  }
			      } 

               waccel_minusident();
               fprintf(wfp,"%s)\n", ident);
            } // case INSERT
            break;
         } // end switch
      }
   } // end for

   return;
} /* end Accel_WriteCompInstData */

/*****************************************************************************/
/*
*/
static int one_shape(int blocknum)
{
   BlockStruct *b = doc->Find_Block_by_Num(blocknum);

   int   cnt = 0;
   int   filled = 0;

   if (b)
   {
      POSITION pos = b->getDataList().GetHeadPosition();
      while (pos != NULL)
      {
         DataStruct *np = b->getDataList().GetNext(pos);
      
         if (np->getDataType() != T_POLY)       return FALSE;

         CPoly *poly;
         POSITION polyPos;

         polyPos = np->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            poly = np->getPolyList()->GetNext(polyPos);
            cnt++;
            if (poly->isFilled())       filled++;
         }
      }
   }

   if (cnt == 1 && filled == 1)  return TRUE;
   return FALSE;
}

/*****************************************************************************/
/*
*/
static CString do_padshapeoutline(CDataList *DataList,
                    double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   CString  shapestring;

   shapestring = "";

   // this all works, it was tested in one_shape()
   POSITION pos = DataList->GetHeadPosition();

   while (pos != NULL)
   {
      DataStruct *np =DataList->GetNext(pos);
      
      CPoly    *poly;
      CPnt     *pnt;
      POSITION polyPos, pntPos;
      Point2   p;

      polyPos = np->getPolyList()->GetHeadPosition();
      while (polyPos != NULL)
      {
         poly = np->getPolyList()->GetNext(polyPos);

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
            CString tmp;
            tmp.Format("%s  (pt %1.*lf %1.*lf)\n", ident, output_units_accuracy, p.x, output_units_accuracy, p.y);
            shapestring += tmp;
         }        
      }
   }
   return shapestring;
}

//--------------------------------------------------------------
int Accel_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int viatype, 
      double drill, UINT64 *layerflg)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   typ = 0;
   CString  padprefix = "pad";
   
   if (viatype)   padprefix = "via";
   *layerflg = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() == T_INSERT)
      {
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

               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               //if (!doc->get_layer_visible(block_layer, mirror, np->getDataType()))  break;

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               if (block->getFlags() & BL_TOOL) break;   // no drill

               LayerStruct *lay = doc->FindLayer(block_layer);

               double   padxdim, padydim, padxoff, padyoff;

               int l = get_layernr_from_index(block_layer);
               if (l == -99) // NOT IN SIGLAY LITS
                  break;            

               // get LAY_TOP, LAY_BOT, LAY_ALL, LAY_INNER
               int l1 = l;

               if (lay != NULL)
               {
                  if (lay->getLayerType() == LAYTYPE_PAD_ALL)
                  {
                     l1 = LAY_ALL;
                  }
                  else
                  if (lay->getLayerType() == LAYTYPE_PAD_INNER)
                  {
                     if (!lay->getElectricalStackNumber())
                        l1 = LAY_INNER;
                  }
                  else
                  if (lay->getLayerType() == LAYTYPE_SIGNAL_INNER)
                  {
                     if (!lay->getElectricalStackNumber())
                        l1 = LAY_INNER;
                  }
                  else
                  if (lay->getLayerType() == LAYTYPE_PAD_THERMAL)
                  {
                     if (!lay->getElectricalStackNumber())
                        l1 = LAY_PLANE;
                  }
                  else
                  if (lay->getLayerType() == LAYTYPE_POWERNEG)
                  {
                     if (!lay->getElectricalStackNumber())
                        l1 = LAY_PLANE;
                  }

                  if (lay->getMirrorOnly())
                  {
                     l1 = LAY_TOP;  // this make it always on top
                  }
               }

               CString  padform;
               // block has the aperture of a padstack
               switch (block->getShape())
               {
                  case T_SQUARE:
                     padxdim = padydim = block->getSizeA() * scale;
                     padxoff = padyoff = 0;
                  
                     if (padxdim < drill) padxdim = drill;
                     if (padydim < drill) padydim = drill;

                     padform.Format("(%sShapeType Rect) (shapeWidth %1.*lf) (shapeHeight %1.*lf)",padprefix,
                        output_units_accuracy, padxdim, output_units_accuracy, padydim);
                  break;
                  case T_OCTAGON:
                  case T_DONUT:
                  case T_ROUND:
                     padxdim = padydim = block->getSizeA() * scale;
                     padxoff = padyoff = 0;

                     if (padxdim < drill) padxdim = drill;
                     if (padydim < drill) padydim = drill;

                     padform.Format("(%sShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf)",padprefix,
                        output_units_accuracy, padxdim, output_units_accuracy, padydim);
                  break;
                  case T_RECTANGLE:
                  {
                     int   r = round(RadToDeg(block->getRotation()+block_rot));

                     while (r < 0)     r += 360;
                     while (r >=360)   r -= 360;

                     switch (r)
                     {
                        case 0:
                           padxdim = block->getSizeA() * scale;
                           padydim = block->getSizeB() * scale;
                           padxoff = padyoff = 0;
                        break;
                        case 90:
                           padxdim = block->getSizeB() * scale;
                           padydim = block->getSizeA() * scale;
                           padxoff = padyoff = 0;
                        break;
                        case 180:
                           padxdim = block->getSizeB() * scale;
                           padydim = block->getSizeA() * scale;
                           padxoff = padyoff = 0;
                        break;
                        case 270:
                           padxdim = block->getSizeB() * scale;
                           padydim = block->getSizeA() * scale;
                           padxoff = padyoff = 0;
                        break;
                        default:
                        {
                           fprintf(flog,"Need complex padshape for odd rotation\n");
                           display_error++;
                           padxdim = block->getSizeA() * scale;
                           padydim = block->getSizeB() * scale;
                           padxoff = padyoff = 0;
                        }
                        break;
                     }

                     if (padxdim < drill) padxdim = drill;
                     if (padydim < drill) padydim = drill;

                     padform.Format("(%sShapeType Rect) (shapeWidth %1.*lf) (shapeHeight %1.*lf)", padprefix,
                              output_units_accuracy, padxdim, output_units_accuracy, padydim);

                  }
                  break;
                  case T_OBLONG:
                  {
                     int   r = round(RadToDeg(block->getRotation()+block_rot));

                     while (r < 0)     r += 360;
                     while (r >=360)   r -= 360;

                     switch (r)
                     {
                        case 0:
                           padxdim = block->getSizeA() * scale;
                           padydim = block->getSizeB() * scale;
                           padxoff = padyoff = 0;
                        break;
                        case 90:
                           padxdim = block->getSizeB() * scale;
                           padydim = block->getSizeA() * scale;
                           padxoff = padyoff = 0;
                        break;
                        case 180:
                           padxdim = block->getSizeB() * scale;
                           padydim = block->getSizeA() * scale;
                           padxoff = padyoff = 0;
                        break;
                        case 270:
                           padxdim = block->getSizeB() * scale;
                           padydim = block->getSizeA() * scale;
                           padxoff = padyoff = 0;
                        break;
                        default:
                        {
                           fprintf(flog,"Need complex padshape for odd rotation\n");
                           display_error++;
                           padxdim = block->getSizeA() * scale;
                           padydim = block->getSizeB() * scale;
                           padxoff = padyoff = 0;
                        }
                        break;
                     }
                     padform.Format("(%sShapeType Oval) (shapeWidth %1.*lf) (shapeHeight %1.*lf)",padprefix,
                           output_units_accuracy, padxdim, output_units_accuracy, padydim);
                  }
                  break;
                  case T_COMPLEX:
                  {
                     int   subblocknum = (int)block->getSizeA();
                     if (one_shape(subblocknum))
                     {
                        padform.Format("(%sShapeType Polygon) (shapeOutline\n",padprefix);

                        BlockStruct *b = doc->Find_Block_by_Num(subblocknum);
                        padform += do_padshapeoutline(&(b->getDataList()), 
                                  0.0, 0.0, 0.0, //rotation + np->getInsert().angle,
                                  0, scale );
                        //output_units_accuracy, padxdim, output_units_accuracy, padydim);

                        //   (pt x y)...
                        padform += ident;
                        padform += " )\n";
                     }
                     else
                     {
                        // unknown
                        if ( block->getShape() == -1 )
                        {
                           fprintf(flog,"Complex Padform [%s][Unknown] containing more than 1 polygon -> set to 5MIL round\n",
                              block->getName());
                        }
                        else  // undefined or complex
                        {   
                           fprintf(flog,"Complex Padform [%s][%s] containing more than 1 polygon -> set to 5MIL round\n",
                              block->getName(), shapes[block->getShape()]);
                        }
                        display_error++;
                        padxdim = padydim = 5; // 5 mil to units
                        padxoff = padyoff = 0;

                        padform.Format("(%sShapeType Oval) (shapeWidth %1.*lf) (shapeHeight %1.*lf)",padprefix,
                           output_units_accuracy, padxdim, output_units_accuracy, padydim);
                     }
                  }
                  break;
                  default:
                     if ( block->getShape() == -1 )
                     {
                        fprintf(flog,"Complex Padform [%s][Unknown] containing more than 1 polygon -> set to 5MIL round\n",
                           block->getName());
                     }
                     else  // undefined or complex
                     {   
                        fprintf(flog,"Complex Padform [%s][%s] containing more than 1 polygon -> set to 5MIL round\n",
                           block->getName(), shapes[block->getShape()]);
                     }                  display_error++;
                     padxdim = padydim = 5; // 5 mil to units
                     padxoff = padyoff = 0;

                     padform.Format("(%sShapeType Oval) (shapeWidth %1.*lf) (shapeHeight %1.*lf)",padprefix,
                           output_units_accuracy, padxdim, output_units_accuracy, padydim);
                  break;
               }

               if (strlen(padform) == 0)  break;

               if (l1 < 0) // l1 is the signalnumber of positive or a layertype if negative
               {
                  switch (l1)
                  {
                     case LAY_TOP:  //       -1
                        *layerflg |= 1;
                        // pad: layername, layernumber, type, xdim, ydim, xoff, yoff
                        //fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","TOP", 0,
                        // padxdim, padydim, padxoff, padyoff);
                     break;
                     case LAY_BOT:  //       -2
                     {
                        UINT64 baseVal = 1L;
                        *layerflg |= baseVal << (max_signalcnt-1);
                        //fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","BOTTOM", max_signalcnt-1,
                        // padxdim, padydim, padxoff, padyoff);
                     }
                     break;
                     case  LAY_INNER:  //    -3
                     {
                        for (int i=1;i<max_signalcnt-1;i++)
                        {
                           UINT64 baseVal = 1L;
                           *layerflg |= baseVal << i;
                           //fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","INNER", i,
                           // padxdim, padydim, padxoff, padyoff);
                        }
                        typ |= 4 ;
                        fprintf(fp,"%s(%sShape (layerType Signal) %s )\n", ident, padprefix, padform);
                     }
                     break;
                     case LAY_ALL:     //    -4
                     {
                        for (int i=0;i<max_signalcnt;i++)
                        {
                           UINT64 baseVal = 1L;
                           *layerflg |= baseVal << i;
                           //fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","INNER", i,
                           // padxdim, padydim, padxoff, padyoff);
                        }
                     }
                     break;
                     case  LAY_PLANE:  //    -5
                     {
                        typ |= 8;
                        fprintf(fp,"%s(%sShape (layerType Plane) %s )\n", ident, padprefix, padform);
                     }
                     break;
                     default:
                        fprintf(flog,"Unknown Layer layer\n");
                        display_error++;
                     break;
                  }
               }
               else
               {
                  UINT64 baseVal = 1L;
                  *layerflg |= baseVal << (lay->getElectricalStackNumber()-1);

                  if (l == 1) typ |= 1;
                  if (l == 2) typ |= 2;

                  fprintf(fp,"%s(%sShape (layerNumRef %d) %s )\n", ident, padprefix, l, padform);
               }
            } // case INSERT
            break;
         } // end switch
      }
   } // end Accel_WritePADSTACKData */
   return typ;
}

/*****************************************************************************/
/*
*/
static int update_textarray(double height, int penwidthindex, double scale)
{
   int   i;

   for (i=0;i<textcnt;i++)
   {
      if (textarray[i]->height == height &&
          textarray[i]->penwidthindex == penwidthindex)
          return i;
   }

   ACCELTextStruct *t = new ACCELTextStruct;
   textarray.SetAtGrow(textcnt, t);
   textcnt++;
   int h = round(height*scale);
   int w = round(doc->getWidthTable()[penwidthindex]->getSizeA()*scale);
   t->name.Format("H%dW%d",h,w);
   t->height = height;
   t->penwidthindex = penwidthindex;

   return textcnt-1;
}

//--------------------------------------------------------------
static void attributes_GetTextData(CAttributes* map, double scale)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      if (!attrib->isVisible())
      {
         continue;      // do not show an invisble attribute.
      }

      update_textarray(attrib->getHeight(), attrib->getPenWidthIndex(), scale);
   }

   return;
}

//--------------------------------------------------------------
void Accel_GetTextData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   DataStruct *np;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // here do attributes
      attributes_GetTextData(np->getAttributesRef(), scale);

      switch(np->getDataType())
      {
         case T_POLY:
         break;

         case T_TEXT:
         {
            update_textarray(np->getText()->getHeight(), np->getText()->getPenWidthIndex(), scale);
         }
         break;
         
         case T_INSERT:
         {
            Point2 point2;

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

               Accel_GetTextData(&(block->getDataList()), 
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1,-1);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

} /* end ACCEL_GetText */

/****************************************************************************/
/*
*/
static int load_ACCELsettings(const char *fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   ARC_ANGLE_DEGREE = 5;
   WORKSPACE_ENLARGE = 0;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"  ACCEL Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".ARCSTEPANGLE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            ARC_ANGLE_DEGREE = atof(lp);
            if (ARC_ANGLE_DEGREE < 4)     // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too small, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 4;
            }
            else
            if (ARC_ANGLE_DEGREE > 45)    // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too large, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 45;
            }
         }
         if (!STRICMP(lp,".WORKSPACEENLARGE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            WORKSPACE_ENLARGE = atof(lp);
         }
      }
   }

   fclose(fp);

   return 1;
}

// end ACCEL_OUT.CPP