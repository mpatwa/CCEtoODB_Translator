// $Header: /CAMCAD/4.5/read_wrt/Fatf_out.cpp 32    8/05/06 4:54p Kurt Van Ness $

/*****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 2001. All Rights Reserved.


  Fabmaster is clockwise rotation

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
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

#define  ACCEL_SMALLNUMBER 0.1      // accurracy 

typedef struct
{
   CString  name;
   double   height, width;
   int      penwidthindex;
} FATFTextStruct;

typedef CTypedPtrArray<CPtrArray, FATFTextStruct*> TextArray;

typedef struct
{
   int            block_num;  //
   CString        name;
   double         drill;      // pad_stacks are only written for drilled pads, all others are exploded in
                              // the package section.
}FATFPadstack;
typedef CTypedPtrArray<CPtrArray, FATFPadstack*> CPadstackArray;

typedef struct
{
   int            block_num;
   CString        name;
}
FATFPadsymbol;
typedef CTypedPtrArray<CPtrArray, FATFPadsymbol*> CPadsymbolArray;

typedef struct
{
   CString        name;
   int            geomnum;
}FATFComponent;
typedef CTypedPtrArray<CPtrArray, FATFComponent*> CComponentArray;

typedef struct
{
   int            index;
   CString        name;
   CString        layersense;
   CString        layertype;
   int            mirrorlayerindex;

   void setName(const CString& Name) { name = Name; }
}FATFLayernames;
typedef CTypedPtrArray<CPtrArray, FATFLayernames*> CLayernamesArray;

typedef struct
{
   CString        name;
}FATFNet;
typedef CTypedPtrArray<CPtrArray, FATFNet*> CNetArray;

typedef struct
{
   int      index;      // pinnumber
   double   pinx, piny;
   double   rotation;
   CString  padstackname;
   CString  pinname;
   int      mechanical;
}FATFShapePinInst;

typedef CTypedPtrArray<CPtrArray, FATFShapePinInst*> ShapePinInstArray;
//
typedef struct
{
   int      geomnum;
   int      index;      // pinnumber
   double   x,y;
   char     type;       // D = drilled, T = topside, B = bottomside
   CString  pinname;
   CString  padstackname;
}FATFCompPinInst;
typedef CTypedPtrArray<CPtrArray, FATFCompPinInst*> CompPinInstArray;

//extern char             *layertypes[];

static CCEtoODBDoc       *doc;
static FormatStruct     *format;
static FILE             *flog;
static int              display_error;

static double           scale; // pageunits to output units


static   CString        ident;
static   int            identcnt = 0;

static   TextArray      textarray;
static   int            textcnt;

static   ShapePinInstArray shapepininstarray;   
static   int            shapepininstcnt;

static   CompPinInstArray  comppininstarray; 
static   int            comppininstcnt;

static   CPadstackArray padstackarray;    // this is the device - mapping
static   int            padstackcnt;

static   CComponentArray   componentarray;       
static   int            componentcnt;

static   CLayernamesArray  layernamesarray;      
static   int            layernamescnt;

static   CNetArray      netarray;    
static   int            netcnt;

static   CPadsymbolArray   padsymbolarray;
static   int            padsymbolcnt;

static   int            max_signalcnt;
static   int            output_units_accuracy = 1; // 1/10 of a mil.
 
static   double         ARC_ANGLE_DEGREE;

void FATF_WriteBoardData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int *index);

static int FATF_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
                   double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static void FATF_WriteLayerData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer, int negative_check, int patterndef);

static void FATF_WriteGeomData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer, int negative_check, int patterndef);

static void FATF_WriteViaData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer);

static void FATF_WritePCBInsertData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer);

static int load_FATFsettings(const CString fname);

/****************************************************************************/
/*
   in 100 mils
*/
static long cnv_units(double x)
{
   long u;
   
   u = lround(x * scale * 100);

   return u;
}

/*****************************************************************************/
/*
*/
static int ignore_non_manufacturing_info(DataStruct *np)
{

   if (np->getDataType() != T_INSERT)
   {
      if (np->getGraphicClass() == GR_CLASS_ANTI_ETCH)          return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)        return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)        return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)       return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPIN)          return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPIN)          return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPOUT)         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_UNROUTE)            return TRUE;
      if (np->getGraphicClass() == graphicClassTestPointKeepOut)   return TRUE;
      if (np->getGraphicClass() == graphicClassTestPointKeepIn)   return TRUE;
      if (np->getGraphicClass() == graphicClassPlacementRegion)   return TRUE;
   }
   else
   {
      if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD)    return TRUE;
      if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)   return TRUE;
      if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE)    return TRUE;

   }

   return FALSE;
}

/****************************************************************************/
/*
   clockwise rotation
*/
static int fatf_rot(double rot)
{
   int r = 360 - round(RadToDeg(rot));

   while (r < 0)     r += 360;
   while (r >= 360)  r -= 360;
   return r*10;
}

/*****************************************************************************/
/*
*/
static int update_textarray(double height, double width, int penwidthindex, double scale)
{
   int   i;

   for (i=0;i<textcnt;i++)
   {
      if (textarray[i]->height == height && textarray[i]->width == width &&
          textarray[i]->penwidthindex == penwidthindex)
          return i;
   }

   FATFTextStruct *t = new FATFTextStruct;
   textarray.SetAtGrow(textcnt, t);
   textcnt++;
   int h = round(height*scale);
   int w = round(doc->getWidthTable()[penwidthindex]->getSizeA()*scale);
   t->name.Format("H%dW%d",h,w);
   t->height = height;
   t->width = width;
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

      update_textarray(attrib->getHeight(), attrib->getWidth(), attrib->getPenWidthIndex(), scale);
   }

   return;
}

//--------------------------------------------------------------
static void FATF_GetTextData(CDataList *DataList,
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

      //if (ignore_non_manufacturing_info(np))
      // continue;

      // here do attributes
      attributes_GetTextData(np->getAttributesRef(), scale);

      switch(np->getDataType())
      {
         case T_POLY:
         break;

         case T_TEXT:
         {
            update_textarray(np->getText()->getHeight(), np->getText()->getWidth(), np->getText()->getPenWidthIndex(), scale);
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

               FATF_GetTextData(&(block->getDataList()), 
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
      FATF_GetTextData(&(block->getDataList()),
                   0.0,0.0, 0.0, 0,  scale, -1, -1);
   }

   // textstyledef
   FATF_GetTextData(&(file->getBlock()->getDataList()),
                    file->getInsertX(), file->getInsertY(),
                    file->getRotation(), file->isMirrored(), file->getScale() * scale, -1, -1);

   return 1;
}

/*****************************************************************************/
/*
*/
static int FATFPinNameCompareFunc( const void *arg1, const void *arg2 )
{
   FATFShapePinInst **a1, **a2;
   a1 = (FATFShapePinInst**)arg1;
   a2 = (FATFShapePinInst**)arg2;

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
static void FATF_SortPinData(const char *s)
{
   int   done = FALSE;
   int   sortcnt = 0;

   if (shapepininstcnt < 2)   return;

   qsort(shapepininstarray.GetData(), shapepininstcnt, sizeof(FATFShapePinInst *), FATFPinNameCompareFunc);
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
static void FATF_MakePinNames_Unique(const char *s)
{
   int   i, ii;

   for (i=0;i<shapepininstcnt;i++)
   {
      for (ii=0;ii<i;ii++)
      {
         FATFShapePinInst  *p1 = shapepininstarray[i];
         FATFShapePinInst  *p2 = shapepininstarray[ii];
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
static int get_padsymbolindex(const char *p)
{
   int   i;
   
   for (i=0;i<padsymbolcnt;i++)
   {
      if (!padsymbolarray[i]->name.CompareNoCase(p))
         return i;
   }

   fprintf(flog,"Padsymbol [%s] not found in index\n",p);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static int get_compindex(const char *c)
{
   int   i;
   
   for (i=0;i<componentcnt;i++)
   {
      if (!componentarray[i]->name.CompareNoCase(c))
         return i;
   }

   fprintf(flog,"Component [%s] not found in index\n",c);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static int get_netindex(const char *n)
{
   int   i;
   
   for (i=0;i<netcnt;i++)
   {
      if (!netarray[i]->name.CompareNoCase(n))
         return i;
   }

   fprintf(flog,"Net [%s] not found in index\n",n);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static int get_layernameindex(const char *l)
{
   int   i;
   
   for (i=0;i<layernamescnt;i++)
   {
      if (!layernamesarray[i]->name.CompareNoCase(l))
         return i;
   }

   fprintf(flog,"Layer [%s] not found in index\n",l);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static int get_pinindex(int geomnum, const char *p)
{
   int   i;
   
   for (i=0;i<comppininstcnt;i++)
   {
#ifdef _DEBUG
      FATFCompPinInst *c = comppininstarray[i];
#endif
      if (comppininstarray[i]->geomnum == geomnum && !comppininstarray[i]->pinname.Compare(p))
      {
         return i;
      }
   }

   fprintf(flog,"Componen Pin [%s] not found in index\n",p);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static int do_parts(FILE *fp, FileStruct *file)
{
   fprintf(fp,":PARTS\n");

   FATF_WritePCBInsertData(fp, &(file->getBlock()->getDataList()), file->getInsertX(),file->getInsertY(),
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale(), 0,-1);

/*
 1,FM1,"FORO (110)","AF110", 140, 1817, 3600,T;
 43,R5,"R06657","GT0603", 127, 734, 3600,T;
 44,L1,"L06630","L1082", 869, 1264, 3600,T;
*/
   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
/*
1,"+5V",S,
     ( (10,1),(11,2),(14,1),(20,2),(22,2),(31,1),
     (35,2),(37,2),(38,2),(39,2),(44,1));
2,"GND",S,
     ( (1,1),(2,1),(3,1),(4,1),(5,1),(6,1),
     (7,1),(10,4),(10,5),(11,1),(12,1),(14,2),
*/
static int do_nets(FILE *fp, FileStruct *file)
{
   fprintf(fp,":NETS\n");

   NetStruct *net;
   POSITION  netPos;
   
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if ((net->getFlags() & NETFLAG_UNUSEDNET)) continue;

      FATFNet  *c = new FATFNet;
      netarray.SetAtGrow(netcnt,c);  
      netcnt++;
      c->name = net->getNetName();
   
      int cnt = fprintf(fp, "%d,\"%s\",S,\n", netcnt, check_name('n', net->getNetName()));
      fprintf(fp,"  (");

      CompPinStruct *compPin;
      POSITION compPinPos;
      int   first = TRUE;
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         int cindex = get_compindex(compPin->getRefDes());
         if (cindex < 0)
         {
            // component not found
            fprintf(flog, "Component [%s] in Netlist not found\n", compPin->getRefDes());
            display_error++;
            continue;
         }
         int pindex = get_pinindex(componentarray[cindex]->geomnum,compPin->getPinName());
         if (!first) cnt +=fprintf(fp,",");
         if (cnt > 60)
            cnt = fprintf(fp,"\n   ");
         cnt +=fprintf(fp, "(%d,%d)",cindex+1, comppininstarray[pindex]->index);
         first = FALSE;
      }

      fprintf(fp,");\n", ident);
   }

   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
// only simple apertures, complex are exploded in the packages
// 
static int do_pad_symbols(FILE *fp, FileStruct *file)
{

   fprintf(fp,":PAD_SYMBOLS\n");

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      // create necessary aperture blocks
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         if (block->getShape() == T_COMPLEX)
         {
            // skip
            FATFPadsymbol *p = new FATFPadsymbol;
            padsymbolarray.SetAtGrow(padsymbolcnt,p);  
            padsymbolcnt++;
            p->block_num = block->getBlockNumber();
            p->name = block->getName();

            BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
            doc->CalcBlockExtents(subblock);
            fprintf(fp,"%d,P_BLOCK (%ld,%ld,%ld,%ld);\n", padsymbolcnt, 
               cnv_units(subblock->getXmin()),cnv_units(subblock->getYmin()),
               cnv_units(subblock->getXmax()),cnv_units(subblock->getYmax()));
         }
         else
         {
            FATFPadsymbol *p = new FATFPadsymbol;
            padsymbolarray.SetAtGrow(padsymbolcnt,p);  
            padsymbolcnt++;
            p->block_num = block->getBlockNumber();
            p->name = block->getName();

            double sizeA = block->getSizeA();
            double sizeB = block->getSizeB(); 
            switch (block->getShape())
            {
               case T_ROUND:
                  fprintf(fp,"%d,P_ROUND (%ld);\n", padsymbolcnt, cnv_units(block->getSizeA()));
               break;
               case T_SQUARE:
                  fprintf(fp,"%d,P_BLOCK (%ld,%ld,%ld,%ld);\n", padsymbolcnt, 
                     -cnv_units(block->getSizeA()/2), -cnv_units(block->getSizeA()/2),
                     cnv_units(block->getSizeA()/2),   cnv_units(block->getSizeA()/2));
               break;
               case T_DONUT:   
                  fprintf(fp,"%d,P_ROUND (%ld);\n", padsymbolcnt, cnv_units(block->getSizeA()));
               break;
               case T_OCTAGON:                  
                  fprintf(fp,"%d,P_ROUND (%ld);\n", padsymbolcnt, cnv_units(block->getSizeA()));
               break;
               case T_RECTANGLE:
               case T_OBLONG: 
               {
                  double width;
                  double x1,y1,x2,y2;
                  if (block->getSizeB() > block->getSizeA())
                  {
                     width = block->getSizeA();
                     x1 = -block->getSizeB()/2 + block->getXoffset();
                     y1 = block->getXoffset();
                     x2 = block->getSizeB()/2 + block->getXoffset();
                     y2 = block->getXoffset();
                  }
                  else
                  {
                     width = block->getSizeB();
                     x1 = -block->getSizeA()/2 + block->getYoffset();
                     y1 = block->getYoffset();
                     x2 = block->getSizeA()/2 + block->getYoffset();
                     y2 = block->getYoffset();
                  }

                  fprintf(fp,"%d,TRACK (%ld (%ld,%ld),(%ld,%ld));\n", padsymbolcnt, 
                     cnv_units(width),
                     cnv_units(x1), cnv_units(y1), cnv_units(x2), cnv_units(y2));
               }
               break;
               default:
                  fprintf(flog, "Unknown Aperture [%d] for Geometry [%s]\n", block->getShape(), block->getName());
                  display_error++;
               break;
            } // switch
         } // not complex
      } // aperture
   } // loop
/*
1,P_BLOCK (-20,-20,20,20);
2,P_BLOCK (-90,-90,90,90);
3,TRACK ( 40,(-19,0),(19,0));
4,TRACK ( 16,(-22,0),(22,0));
5,TRACK ( 56,(-27,0),(27,0));
6,TRACK ( 24,(-18,0),(18,0));
7,TRACK ( 32,(-24,0),(24,0));
8,TRACK ( 46,(-10,0),(10,0));
9,P_ROUND ( 40);
*/
   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
/*****************************************************************************/
/*
   Layers are written by type and not by layernames.
   common
   top
   bottom
   inner
   others
   assembly
   mask
   silk...
*/
static int do_layer_names(FILE *fp, FileStruct *file)
{
   int   j;

   fprintf(fp,":LAYER_NAMES\n");
/*
1,"Top",TOP, 2,ELECTRICAL;
2,"Bottom",BOTTOM, 1,ELECTRICAL;
3,"Comp Outline",TOP,0,ASSEMBLY;
*/
   for (j=0; j< doc->getMaxLayerIndex(); j++)  
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.
      // layer 1 is a special layer
      int   mirrorlayer = 0;
      CString  layersense, layertype;

      layersense = "TRANSPARENT"; // common, top, bottom, transparent
      layertype = "DOCUMENTATION"; // electrical documentation dxf board_cutout masking assembly silkscreen

      if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         layersense = "TOP";        // common, top, bottom, transparent
         layertype = "ELECTRICAL";  // electrical documentation dxf board_cutout masking assembly silkscreen
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
      {
         layersense = "TOP";        // common, top, bottom, transparent
         layertype = "ELECTRICAL";  // electrical documentation dxf board_cutout masking assembly silkscreen
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
      {
         layersense = "BOTTOM";        // common, top, bottom, transparent
         layertype = "ELECTRICAL";  // electrical documentation dxf board_cutout masking assembly silkscreen
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         layersense = "BOTTOM";        // common, top, bottom, transparent
         layertype = "ELECTRICAL";  // electrical documentation dxf board_cutout masking assembly silkscreen
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         layersense = "TRANSPARENT";   // common, top, bottom, transparent
         layertype = "ELECTRICAL";     // electrical documentation dxf board_cutout masking assembly silkscreen

      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_TOP)
      {
         layersense = "TOP";           // common, top, bottom, transparent
         layertype = "MASKING";        // electrical documentation dxf board_cutout masking assembly silkscreen

      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_BOTTOM)
      {
         layersense = "BOTTOM";        // common, top, bottom, transparent
         layertype = "MASKING";        // electrical documentation dxf board_cutout masking assembly silkscreen

      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_ALL)
      {
         layersense = "TRANSPARENT";   // common, top, bottom, transparent
         layertype = "MASKING";        // electrical documentation dxf board_cutout masking assembly silkscreen

      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_TOP)
      {
         layersense = "TOP";           // common, top, bottom, transparent
         layertype = "SILKSCREEN";     // electrical documentation dxf board_cutout masking assembly silkscreen

      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM)
      {
         layersense = "BOTTOM";        // common, top, bottom, transparent
         layertype = "SILKSCREEN";     // electrical documentation dxf board_cutout masking assembly silkscreen

      }
      else
      if (layer->getElectricalStackNumber())
      {
         layersense = "TRANSPARENT";   // common, top, bottom, transparent
         layertype = "ELECTRICAL";     // electrical documentation dxf board_cutout masking assembly silkscreen
      }
      else
      {
         layersense = "TRANSPARENT";   // common, top, bottom, transparent
         layertype = "DOCUMENTATION"; // electrical documentation dxf board_cutout masking assembly silkscreen
      }

      FATFLayernames *l = new FATFLayernames;
      layernamesarray.SetAtGrow(layernamescnt,l);  
      layernamescnt++;  
      l->index = j;
      l->setName(layer->getName());
      l->layersense = layersense;
      l->layertype = layertype;
      l->mirrorlayerindex = layer->getMirroredLayerIndex();
   }

   for (j=1;j<=255;j++)
   {
      for (int i=0;i<layernamescnt;i++)
      {
         if (layernamesarray[i]->index == j)
         {
            fprintf(fp,"%d,\"%s\",%s,%d,%s;\n", 
               layernamesarray[i]->index, layernamesarray[i]->name, layernamesarray[i]->layersense, 
               layernamesarray[i]->mirrorlayerindex, layernamesarray[i]->layertype);
            break;
         }
      }
   }

   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
// need to make layerssets, because this is how padstacks refer to.
// the best thing is to do them by type ??? or only top, bottom, all, inner, 
static int do_layer_sets(FILE *fp, FileStruct *file)
{
   int   j;

   fprintf(fp,":LAYER_SETS\n");
/*
1,"ALL",(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35);
2,"TOP",(2);
3,"BOT",(3);

*/
   for (j=1;j<=255;j++)
   {
      for (int i=0;i<layernamescnt;i++)
      {
         if (layernamesarray[i]->index == j)
         {
            fprintf(fp,"%d,\"%s\",(%d);\n", 
               layernamesarray[i]->index, layernamesarray[i]->name, layernamesarray[i]->index );
            break;
         }
      }
   }

   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
static int do_pad_stacks(FILE *fp, FileStruct *file)
{
   fprintf(fp,":PAD_STACKS\n");
/*
1,,0,B,((2,1));
2,,110,P,((1,2));
3,,0,B,((2,3));
4,,0,B,((2,5));
5,,0,B,((2,8));
6,,0,B,((2,7));
7,,0,B,((2,6));
8,,0,B,((2,4));
9,,23,P,((1,9));
*/
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK) || (block->getBlockType() == BLOCKTYPE_TESTPAD))
      {
         FATFPadstack   *p = new FATFPadstack;
         padstackarray.SetAtGrow(padstackcnt,p);  
         padstackarray[padstackcnt]->name = block->getName();
         padstackarray[padstackcnt]->block_num = block->getBlockNumber();
         padstackcnt++;

         int smd = FALSE;
         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 2);
         if (a)   smd = TRUE;

         double drill = 0.0;
         CString  plated;
         plated = "N";
         if (!smd)
         {
            drill = get_drill_from_block_no_scale(doc, block);
            plated = "P";
         }
         fprintf(fp,"%d,\"%s\",%ld,%s,(", padstackcnt, check_name('p', block->getName()), cnv_units(drill), plated);
         padstackarray[padstackcnt-1]->drill = drill;

         FATF_WritePADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1);
         
         fprintf(fp,");\n");

      }
   }


   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
static int do_pads(FILE *fp, FileStruct *file)
{
   fprintf(fp,":PADS\n");
/*
2,9,((868,990),(1045,951),(1203,951),(1045,793),(887,884),
     (848,703),(928,703),(1510,994),(1577,833),(1734,833),
     (270,1901),(270,1776),(653,415),(215,1478),(733,1359),
     (304,1310),(304,1209),(304,1103),(849,790));
*/
   FATF_WriteViaData(fp, &(file->getBlock()->getDataList()), file->getInsertX(),
                  file->getInsertY(),
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale(), 0,-1);


   fprintf(fp,":EOD\n\n");

   return 1;
}

//-----------------------------------------------------------------------------
static int has_pins(int blocknum)
{
   int cnt = 0;
   for (int j=0;j<comppininstcnt;j++)
   {
      if (comppininstarray[j]->geomnum != blocknum)   continue;
      cnt++;
   }
   return cnt;
}

//-----------------------------------------------------------------------------
static int used_as_part(int blocknum)
{
   int   i;
   for (i=0;i<componentcnt;i++)
   {
      if (componentarray[i]->geomnum == blocknum)
         return i+1;
   }
   return 0;
}

//-----------------------------------------------------------------------------
static int do_packages(FILE *fp, FileStruct *file)
{
   int   i;
   int   cnt = 0;

   fprintf(fp,":PACKAGES\n");
/*
8,"FUD-T",-1952,1952,-1952,1952
 (PINS (1,"",0,0,T))
 (1,LAYER (2 (
   ROUND (3900,0,0)))
  0,LAYER (18 (
   TRACK (4,(-1950,-1950),(1950,-1950),(1950,1950),(-1950,1950),
         (-1950,-1950)))));

*/
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (!used_as_part(block->getBlockNumber()))   continue;   // only write out what is used in a part

      if (block->getFlags() & BL_WIDTH)
         continue;
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         continue;
      if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
         continue;
      
      if (block->getBlockType() == BLOCKTYPE_UNKNOWN ||
          block->getBlockType() == BLOCKTYPE_DRAWING ||
          block->getBlockType() == BLOCKTYPE_GENERICCOMPONENT ||
          block->getBlockType() == BLOCKTYPE_MECHCOMPONENT ||
          block->getBlockType() == BLOCKTYPE_TESTPOINT ||
          block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
      {
         doc->CalcBlockExtents(block);

         fprintf(fp,"%d,\"%s\",%ld,%ld,%ld,%ld\n",
            ++cnt, check_name('s', block->getName()), 
            cnv_units(block->getXmin()),cnv_units(block->getYmin()),
            cnv_units(block->getXmax()),cnv_units(block->getYmax()));

         if (has_pins(block->getBlockNumber()))
         {
            int cnt = fprintf(fp," (PINS ");

            for (int j=0;j<comppininstcnt;j++)
            {
#ifdef _DEBUG
      FATFCompPinInst *c = comppininstarray[j];
#endif
               if (comppininstarray[j]->geomnum != block->getBlockNumber()) continue;
   
               if (cnt > 60)
               {
                  cnt = fprintf(fp,"\n     ");
               }

               cnt += fprintf(fp,"(%d,\"%s\",%ld,%ld,%c)",
                  comppininstarray[j]->index, comppininstarray[j]->pinname,
                  cnv_units(comppininstarray[j]->x),cnv_units(comppininstarray[j]->y),
                  comppininstarray[j]->type);
            }
            // here now all pins
            fprintf(fp," )\n");
         }

         // this opens for pads and graphic
         cnt = fprintf(fp," (\n");

         // here write pads
         for (int j=0;j<comppininstcnt;j++)
         {
#ifdef _DEBUG
      FATFCompPinInst *c = comppininstarray[j];
#endif
            if (comppininstarray[j]->geomnum != block->getBlockNumber()) continue;
   
            int padstackindex = get_padstackindex(comppininstarray[j]->padstackname)+1;
            fprintf(fp,"   %d,PAD (%d ((%ld,%ld)))\n",
               comppininstarray[j]->index, padstackindex,
               cnv_units(comppininstarray[j]->x),cnv_units(comppininstarray[j]->y));
         }

         // do graphic
         FATF_WriteGeomData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0,-1, FALSE, TRUE);

         fprintf(fp," );\n");

      }
   }

   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
static int do_test_points(FILE *fp, FileStruct *file)
{
   fprintf(fp,":TEST_POINTS\n");
   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
static int do_layers(FILE *fp, FileStruct *file)
{
   fprintf(fp,":LAYERS\n");

   FATF_WriteLayerData(fp, &(file->getBlock()->getDataList()), file->getInsertX(),
                  file->getInsertY(),
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale(), 0,-1, TRUE, FALSE);

/*
1,LAYER ( 1,( TRACK ( 20,( 567, 1209),( 502, 1209)),
      TRACK ( 20,( 762, 886),( 762, 788)),
      TRACK ( 20,( 762, 995),( 762, 886)),
      TRACK ( 20,( 436, 1104),( 502, 1104)),
      TRACK ( 20,( 502, 1104),( 502, 1104)),
      TRACK ( 20,( 437, 1776),( 437, 1545)),
      TRACK ( 20,( 437, 1545),( 436, 1209)),
      TRACK ( 12,( 437, 1776),( 437, 1704)),
      TRACK ( 15,( 357, 1704),( 437, 1704)),
      TRACK ( 20,( 869, 1264),( 869, 1116)),
      TRACK ( 20,( 502, 1209),( 502, 1104)),
      TRACK ( 20,( 762, 1019),( 762, 1007)),
      TRACK ( 20,( 502, 1104),( 502, 1019)),
      TRACK ( 20,( 502, 1019),( 762, 1019)),
      TRACK ( 20,( 762, 1007),( 762, 995)),
      TRACK ( 20,( 869, 1116),( 762, 1116)),
      TRACK ( 20,( 762, 1116),( 762, 1007)),
      TRACK ( 20,( 436, 1209),( 436, 1104))));
*/
   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
/*
   FATF_WriteLayerData(wfp, &(file->getBlock()->getDataList()), file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0,-1, TRUE, FALSE);
*/
static int do_board_data(FILE *fp, FileStruct *file)
{
   Mat2x2      m;
   double      insert_x = file->getInsertX() * scale;
   double      insert_y = file->getInsertY() * scale;
   RotMat2(&m, file->getRotation());

   CTime time = CTime::GetCurrentTime();

   int   index = 0;
   fprintf(fp,":BOARD_DATA\n");
   fprintf(fp,"%d,JOB(\"%s\",1,%02d-%02d-%d,%02d-%02d-%d);\n", ++index, file->getName(),
      time.GetDay(), time.GetMonth(), time.GetYear(),
      time.GetDay(), time.GetMonth(), time.GetYear() );

   
   FATF_WriteBoardData(fp, &(file->getBlock()->getDataList()), file->getInsertX(),file->getInsertY(),
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale(), 0,-1, &index);


/*
2,CONTOUR(( 2626, 6,0),( 2626, 242,0),( 2411, 242,0),
     ( 2391, 244,0),( 2372, 252,0),( 2355, 265,0),
     ( 2343, 281,0),( 2335, 300,0),( 2333, 321,0),
     ( 2333, 321,0),( 2333, 955,0),( 2214, 1073,0),
     ( 1061, 1073,0),( 1040, 1075,0),( 1021, 1083,0),
     ( 1005, 1096,0),( 992, 1112,0),( 985, 1131,0),
     ( 982, 1152,0),( 982, 1152,0),( 982, 1817,0),
     ( 864, 1935,0),( 120, 1935,0),( 2, 1817,0),
     ( 2, 124,0),( 120, 6,0),( 2626, 6,0));
*/
   doc->validateBlockExtents(file->getBlock());

   Point2   pmin, pmax; 
   pmin.x = file->getBlock()->getXmin();
   pmin.y = file->getBlock()->getYmin();
   pmax.x = file->getBlock()->getXmax();
   pmax.y = file->getBlock()->getYmax();
   TransPoint2(&pmin, 1, &m, insert_x, insert_y);
   TransPoint2(&pmax, 1, &m, insert_x, insert_y);

   fprintf(fp,"%d,WORK_SPACE (%ld,%ld,%ld, %ld);\n", ++index, 
      cnv_units(pmin.x), cnv_units(pmin.y), cnv_units(pmax.x), cnv_units(pmax.y));
/*
4,FIDUCIALS ( ( 850 1675),( 50 375),( 2275 925));
*/
   fprintf(fp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
/*
*/
static int FATF_patterndefpin(CDataList *DataList)
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
   
               point2.x = np->getInsert()->getOriginX();
               point2.y = np->getInsert()->getOriginY();

               //TransPoint2(&point2, 1, &m, insert_x, insert_y);

               double r = RadToDeg(np->getInsert()->getAngle());
   
               while (r < 0)     r+=360;
               while (r >= 360)  r-=360;

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               FATFShapePinInst *c = new FATFShapePinInst;
               shapepininstarray.SetAtGrow(shapepininstcnt,c);  
               shapepininstcnt++;   
               c->padstackname = block->getName();
               c->pinname = np->getInsert()->getRefname();
               c->rotation = r;
               c->pinx = point2.x;
               c->piny = point2.y;
               c->mechanical = FALSE;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               Point2 point2;
   
               point2.x = np->getInsert()->getOriginX();
               point2.y = np->getInsert()->getOriginY();

               //TransPoint2(&point2, 1, &m, insert_x, insert_y);

               double r = RadToDeg(np->getInsert()->getAngle());
   
               while (r < 0)     r+=360;
               while (r >= 360)  r-=360;

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               FATFShapePinInst *c = new FATFShapePinInst;
               shapepininstarray.SetAtGrow(shapepininstcnt,c);  
               shapepininstcnt++;   
               c->padstackname = block->getName();
               c->pinname = np->getInsert()->getRefname();
               c->rotation = r;
               c->pinx = point2.x;
               c->piny = point2.y;
               c->mechanical = TRUE;
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
static int  get_patterndef()
{
   int   i;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT) ||
           (block->getBlockType() == BLOCKTYPE_TESTPOINT) )
      {
         FATF_patterndefpin(&(block->getDataList()));
         FATF_MakePinNames_Unique(block->getName());
         FATF_SortPinData(block->getName());
			
			int pcnt=0;
         for (pcnt=0;pcnt<shapepininstcnt;pcnt++)
         {
            shapepininstarray[pcnt]->index = pcnt+1;
            FATFCompPinInst *c = new FATFCompPinInst;
            comppininstarray.SetAtGrow(comppininstcnt,c);  
            comppininstcnt++; 
            c->geomnum = block->getBlockNumber();
            c->pinname = shapepininstarray[pcnt]->pinname;
            c->index =  shapepininstarray[pcnt]->index;
            c->x = shapepininstarray[pcnt]->pinx;
            c->y = shapepininstarray[pcnt]->piny;
            c->type = 'T';
            c->padstackname = shapepininstarray[pcnt]->padstackname;
         }
         for (pcnt=0;pcnt<shapepininstcnt;pcnt++)
         {
            delete shapepininstarray[pcnt];
         }
         shapepininstcnt = 0;
      }
   }

   return 1;
}


/*****************************************************************************/
/*
*/
static int  do_textdef(FILE *wfp, double scale)
{
   int   i;

   if (!textcnt)
   {
      FATFTextStruct *t = new FATFTextStruct;
      textarray.SetAtGrow(textcnt, t);
      textcnt++;
      int h = round(100);
      int w = round(10);
      t->name.Format("H%dW%d",h,w);
      t->height = 100.0/scale;
      t->penwidthindex = 0;
      // if no text was loaded, put in a default text, just in case
   }

   fprintf(wfp,":FONTS\n");

   for (i=0;i<textcnt;i++)
   {
      double penWidth = doc->getWidthTable()[textarray[i]->penwidthindex]->getSizeA()*scale;

      fprintf(wfp,"%d,\"%s\",%ld,%ld,%ld,%ld;\n",
         i+1, textarray[i]->name, 
         cnv_units(textarray[i]->height),
         cnv_units(textarray[i]->width), cnv_units(textarray[i]->width),cnv_units(penWidth));

   }

   fprintf(wfp,":EOD\n\n");

   return 1;
}

//--------------------------------------------------------------
void FABMASTER_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format,
                     double UnitsFactor, int PageUnits)

{
   FileStruct  *file;
   FILE        *wfp;

   doc = Doc;
   format = Format;
   display_error = FALSE;
   ident = ""; 
   identcnt = 0;

   shapepininstarray.SetSize(10,10);
   shapepininstcnt = 0;

   comppininstarray.SetSize(10,10);
   comppininstcnt = 0;

   padstackarray.SetSize(10,10);
   padstackcnt = 0;

   componentarray.SetSize(100,100);
   componentcnt = 0;

   layernamesarray.SetSize(100,100);
   layernamescnt = 0;

   padsymbolarray.SetSize(100,100);
   padsymbolcnt = 0;

   netarray.SetSize(100,100);
   netcnt = 0;

   textarray.SetSize(100,100);
   textcnt = 0;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString tmp;

      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }
   
	CString logFile = GetLogfilePath("fatf.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage("Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }


   CString settingsFile( getApp().getExportSettingsFilePath("fatf.out") );
   load_FATFsettings(settingsFile);

   CString  checkFile( getApp().getSystemSettingsFilePath("fatf.chk") );
   check_init(checkFile);

   fprintf(wfp,"; %s FATF OUTPUT FILE\n", filename);
   fprintf(wfp,"; FATF OUTPUT - %s\n", getApp().getCamCadTitle());

   fprintf(wfp,"\n");
   fprintf(wfp,":FABMASTER FATF REV 11.1;\n");
   fprintf(wfp,"\n");
   fprintf(wfp,":UNITS = 1/100000 INCH;\n");         // in 100 mils                 
   fprintf(wfp,"\n");

   scale = Units_Factor(PageUnits, UNIT_MILS);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file

      CWaitCursor wait;
      //do_library(wfp, file, scale);
      get_patterndef();


      if (file->getBlockType() == BLOCKTYPE_PCB)
      {
         GetTextData(file, scale);
         do_board_data(wfp, file);
         do_parts(wfp, file);
         do_nets(wfp, file);
         do_pad_symbols(wfp, file);
         do_layer_names(wfp, file);
         do_layer_sets(wfp, file);      
         do_pad_stacks(wfp, file);      
         do_pads(wfp, file);  
         do_textdef(wfp, 1.0);
         do_packages(wfp, file);       // Graphic and SMD pins not yet done
         do_test_points(wfp, file);     
         do_layers(wfp, file);         
         break;   // only 1 PCB
      }
   }

   fprintf(wfp,":EOF\n");

   // it is deleted in patterdef
   shapepininstarray.RemoveAll();

   int   i;
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

   for (i=0;i<componentcnt;i++)
   {
      delete componentarray[i];
   }
   componentarray.RemoveAll();
   componentcnt = 0;

   for (i=0;i<layernamescnt;i++)
   {
      delete layernamesarray[i];
   }
   layernamesarray.RemoveAll();
   layernamescnt = 0;

   for (i=0;i<padsymbolcnt;i++)
   {
      delete padsymbolarray[i];
   }
   padsymbolarray.RemoveAll();
   padsymbolcnt = 0;

   for (i=0;i<netcnt;i++)
   {
      delete netarray[i];
   }
   netarray.RemoveAll();
   netcnt = 0;

   for (i=0;i<textcnt;i++)
   {
      delete textarray[i];
   }
   textcnt = 0;
   textarray.RemoveAll();

   fclose(wfp);

   if (check_report(flog)) display_error++;
   check_deinit();

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

   return -1;
}

/*****************************************************************************/
/*
*/
static CString clean_text(const char *t)
{
   CString  tt;

   for (int i=0; i<(int)strlen(t); i++)
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

   if (text_mirror)
      r = 360 - r;

   while (r < 0)     r += 360;
   while (r >= 360)  r -= 360;

   fprintf(fp, "%s(text (pt %1.*lf %1.*lf) \"%s\" (textStyleRef \"T:%s\")(rotation %1.1lf)(isFlipped %s))\n",
       ident, output_units_accuracy, x, output_units_accuracy, y, clean_text(text), textfont, r,
       (text_mirror)?"TRUE":"FALSE");

   return 1;
}


//--------------------------------------------------------------
void FATF_WriteLayerData(FILE *fp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int negative_check, int patterndef)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         lindex;
   int         layer;
   int         netindex;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      //if (ignore_non_manufacturing_info(np))
      // continue;

      if (np->getDataType() != T_INSERT)
      {
         // write if either GR_CLASS_BOARDOUTLINE
         // or on LAYTYPE_BOARD_OUTLINE
         if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE )   continue;

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         LayerStruct *l = doc->FindLayer(layer);
         if (l->getLayerType() == LAYTYPE_BOARD_OUTLINE)         continue;

         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
            netindex = get_netindex(get_attvalue_string(doc, a))+1;
         else
            netindex = 0;
         lindex = layernamesarray[get_layernameindex(l->getName())]->index;
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
               BOOL voided     = poly->isVoid();

               Point2   p;
               int      cnt;
               int      first = TRUE;

               cnt = fprintf(fp,"%d,LAYER (%d,",netindex,lindex);
/*                
37,LAYER (2 (
   TRACK (800,(1745000,1680000),(1742500,1682500)),
   TRACK (800,(1937500,1685000),(1934400,1685000),(1933100,1683800),(1933100,1682500)),
   TRACK (800,(1933800,1777500),(1949400,1777500),(1955600,1783800),(1967500,1783800)),
   TRACK (900,(1967500,1800000),(1967500,1783800)),
   TRACK (900,(1967500,1783800),(1967500,1783600))));
*/
               double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y;
                  p.bulge = pnt->bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);

                  if (first)
                  {
                     cnt += fprintf(fp,"(TRACK (%ld,", cnv_units(lineWidth));
                  }
                  else
                  {
                     cnt += fprintf(fp,",");
                  }

                  if (cnt > 60)
                  {
                     cnt = fprintf(fp,"\n   ");
                  }

                  cnt += fprintf(fp,"(%ld,%ld)", cnv_units(p.x), cnv_units(p.y));

                  first = FALSE;
               }
               fprintf(fp,")));\n");
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

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();

            int ptr = update_textarray(np->getText()->getHeight(), np->getText()->getWidth(), np->getText()->getPenWidthIndex(), scale);

            char  mirror = ' ';
            if (text_mirror)  mirror = 'M';
            int rot = round(RadToDeg(text_rot));
            fprintf(fp,"%d,LAYER (%d,",0,lindex);
            fprintf(fp,"(TEXT (%d,%ld,%ld,BOTTOM_LEFT,%d,%c,\"%s\")));\n", ptr+1, 
                  cnv_units(point2.x), cnv_units(point2.y), rot, mirror, np->getText()->getText());

         }
         break;
         
         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...
            if (np->getInsert()->getInsertType() != INSERTTYPE_MECHCOMPONENT && 
                np->getInsert()->getInsertType() != INSERTTYPE_GENERICCOMPONENT)  break;

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

               FATF_WriteLayerData(fp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, negative_check, patterndef);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end FATF_WriteLayerData */

//--------------------------------------------------------------
void FATF_WriteGeomData(FILE *fp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int negative_check, int patterndef)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         lindex;
   int         layer;
   int         netindex;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      //if (ignore_non_manufacturing_info(np))
      // continue;

      if (np->getDataType() != T_INSERT)
      {
         // write if either GR_CLASS_BOARDOUTLINE
         // or on LAYTYPE_BOARD_OUTLINE
         if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE )   continue;

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         LayerStruct *l = doc->FindLayer(layer);
         if (l->getLayerType() == LAYTYPE_BOARD_OUTLINE)         continue;

         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
            netindex = get_netindex(get_attvalue_string(doc, a))+1;
         else
            netindex = 0;
         lindex = layernamesarray[get_layernameindex(l->getName())]->index;
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
               BOOL voided     = poly->isVoid();

               Point2   p;
               int      cnt;
               int      first = TRUE;

               cnt = fprintf(fp,"  %d,LAYER (%d,",netindex,lindex);
/*                
37,LAYER (2 (
   TRACK (800,(1745000,1680000),(1742500,1682500)),
   TRACK (800,(1937500,1685000),(1934400,1685000),(1933100,1683800),(1933100,1682500)),
   TRACK (800,(1933800,1777500),(1949400,1777500),(1955600,1783800),(1967500,1783800)),
   TRACK (900,(1967500,1800000),(1967500,1783800)),
   TRACK (900,(1967500,1783800),(1967500,1783600))));
*/
               double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y;
                  p.bulge = pnt->bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);

                  if (first)
                  {
                     cnt += fprintf(fp,"(TRACK (%ld,", cnv_units(lineWidth));
                  }
                  else
                  {
                     cnt += fprintf(fp,",");
                  }

                  if (cnt > 60)
                  {
                     cnt = fprintf(fp,"\n   ");
                  }

                  cnt += fprintf(fp,"(%ld,%ld)", cnv_units(p.x), cnv_units(p.y));

                  first = FALSE;
               }
               fprintf(fp,")))\n");
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

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();

            int ptr = update_textarray(np->getText()->getHeight(), np->getText()->getWidth(), np->getText()->getPenWidthIndex(), scale);

            char  mirror = ' ';
            if (text_mirror)  mirror = 'M';
            int rot = round(RadToDeg(text_rot));
            fprintf(fp,"  %d,LAYER (%d,",0,lindex);
            fprintf(fp,"(TEXT (%d,%ld,%ld,BOTTOM_LEFT,%d,%c,\"%s\")));\n", ptr+1, 
                  cnv_units(point2.x), cnv_units(point2.y), rot, mirror, np->getText()->getText());

         }
         break;
         
         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (block->getBlockType() == BLOCKTYPE_PADSTACK)  break;

            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               FATF_WriteGeomData(fp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, negative_check, patterndef);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end FATF_WriteGeomData */

//--------------------------------------------------------------
void FATF_WriteViaData(FILE *fp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         netindex;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)                             continue;

      switch(np->getDataType())
      {
         case T_POLY:
         break;  // POLYSTRUCT

         case T_TEXT:
         break;
         
         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...
            if (np->getInsert()->getInsertType() != INSERTTYPE_VIA)      break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
            if (a)
               netindex = get_netindex(get_attvalue_string(doc, a))+1;
            else
               netindex = 0;

            fprintf(fp,"%d,%d,((%ld,%ld));\n", netindex, get_padstackindex(block->getName())+1, 
               cnv_units(point2.x), cnv_units(point2.y));

         } // case INSERT
         break;
      } // end switch
   } // end for

   return;
} /* end FATF_WriteViaData */

//--------------------------------------------------------------
static void FATF_WriteBoardData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int *index)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         layer;
   LayerStruct *l = NULL;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;
      
         l = doc->FindLayer(layer);
         if (l == NULL) 
            continue;

         // write if either GR_CLASS_BOARDOUTLINE
         // or on LAYTYPE_BOARD_OUTLINE
         if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE )   
         {
            // that is good
         }
         else
         if (l->getLayerType() == LAYTYPE_BOARD_OUTLINE)
         {
            // that's also good
         }
         else
         {
            continue;
         }
      } // T_INSERT

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
               Point2   p;

               int first = TRUE; 
               int cnt = 0;
               *index += 1;
               cnt = fprintf(wfp,"%d,CONTOUR (", *index);

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x * scale;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  if (!first) cnt += fprintf(wfp,",");
                  if (cnt > 60)
                     cnt = fprintf(wfp,"\n     ");
                  cnt += fprintf(wfp,"(%ld,%ld, 0)", cnv_units(p.x), cnv_units(p.y));
                  first = FALSE;
               }
               fprintf(wfp,");\n");
            }
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
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
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               FATF_WriteBoardData(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, index);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end FATF_WriteBoardData */

//--------------------------------------------------------------
void FATF_WritePCBInsertData(FILE *wfp, CDataList *DataList,
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
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)  break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX();
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY();
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (block_mirror)
               block_rot = rotation + PI2- np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT) 
            {
/*
 1,FM1,"FORO (110)","AF110", 140, 1817, 3600,T;
 43,R5,"R06657","GT0603", 127, 734, 3600,T;
 44,L1,"L06630","L1082", 869, 1264, 3600,T;
*/

               FATFComponent  *c = new FATFComponent;
               componentarray.SetAtGrow(componentcnt,c);  
               componentcnt++;
               c->geomnum = np->getInsert()->getBlockNumber();
               c->name = np->getInsert()->getRefname();

               CString  pattern, refname, device;
               Attrib *a;

               pattern = check_name('s', block->getName());
               refname = check_name('c', np->getInsert()->getRefname());

               device = "";
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
               {
                  device = get_attvalue_string(doc, a);
               }
               else
               {
                  device = block->getName();
                  fprintf(flog,"Component [%s] has not TypeList (Device) entry\n", refname);
                  display_error++;
               }

               
               fprintf(wfp,"%d,\"%s\",\"%s\",\"%s\",%ld,%ld,%d,%s;\n",
                  componentcnt, refname, device, pattern,
                  cnv_units(point2.x), cnv_units(point2.y), fatf_rot(block_rot),(block_mirror)?"B":"T");
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)  
            {
/*
               fprintf(wfp,"%s(pad (padNum 0) (padStyleRef \"%s\")(pt %1.*lf %1.*lf) )\n",
                  ident, block->name, 
                  output_units_accuracy,point2.x,  output_units_accuracy, point2.y);
*/
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)   
            {
/*

               fprintf(wfp,"%s(via (viaStyleRef \"%s\")(pt %1.*lf %1.*lf)",
                  ident, check_name('p',block->name), 
                  output_units_accuracy,point2.x,  output_units_accuracy, point2.y);
               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
               if (a)
               {
                  CString  netname;
                  netname = check_name('n', get_attvalue_string(doc, a));
                  fprintf(wfp,"(netNameRef \"%s\")", netname);
               }
               fprintf(wfp,")\n");
*/
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
               int block_layer = insertLayer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;
               FATF_WritePCBInsertData(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer);

            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end FATF_WritePCBInsertData */

//--------------------------------------------------------------
int FATF_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   cnt = 0;

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

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (block->getFlags() & BL_TOOL) break;   // no drill

            int ptr = get_padsymbolindex(block->getName());
            if (ptr > 0)
            {
               LayerStruct *lay = doc->FindLayer(block_layer);
               if (cnt) fprintf(fp,",");

               fprintf(fp,"(%d,%d)", block_layer, ptr+1);
               cnt++;
            }
         } // case INSERT
         break;
      } // end switch
   } // end FATF_WritePADSTACKData */
   return cnt;
}

/****************************************************************************/
/*
*/
static int load_FATFsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   ARC_ANGLE_DEGREE = 5;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp,"  ACCEL Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {

      }
   }

   fclose(fp);

   return 1;
}

// end FATF_OUT.CPP