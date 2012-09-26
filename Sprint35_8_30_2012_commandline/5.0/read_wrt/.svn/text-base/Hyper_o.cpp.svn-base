// $Header: /CAMCAD/4.5/read_wrt/Hyper_o.cpp 26    4/28/06 6:52p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1997. All Rights Reserved.

  NEED TO DO:
      - copper area
      - arc in Outline
      - pad forms
*/
                              
#include "stdafx.h"
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include <math.h>
#include "polylib.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  MAX_ATTR          5000
#define  MAX_SIGNALLAYERS  255
#define  MAX_PADSTACKS     1000

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

typedef struct
{
   CString  name;
   int      number;
   int      index;
}Siglay;

typedef struct
{
   int            block_num;  //
   CString        name;
   unsigned long  layerflg;   // bit 0 is layer 1
                              // bit n-1 is maxlayer
}HYPERLYNXPadstack;
typedef CTypedPtrArray<CPtrArray, HYPERLYNXPadstack*> CPadstackArray;

extern   CProgressDlg			*progress;
//extern   LayerTypeInfoStruct	layertypes[];

static   CCEtoODBDoc    *doc;
static   double      unitsFactor;

static   CPadstackArray padstackarray;    // this is the device - mapping
static   int         padstackcnt;

static   Siglay      siglay[MAX_SIGNALLAYERS];
static   int         siglaycnt;
static   int         max_signalcnt;

static   int         display_error;
static   FILE        *flog;
static   int         PageUnits;

static   CString     HYPER_KEY;

// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void HYPERLYNX_WritePRIMARYBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, long gclass);

static void HYPERLYNX_WriteCOMPONENTData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static void HYPERLYNX_WriteROUTESData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname);

static void HYPERLYNX_WritePADSTACKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, double drill);

static void HYPERLYNX_WritePADSHAPEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static int do_signalpinloc(FILE *fp, NetStruct *net, double scale);

//--------------------------------------------------------------
static double  cnv_units(double x)
{
   return x * unitsFactor;
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
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         double drill = 0;
/*
         if (block->toolindex > -1)
            drill = doc->ToolArray[block->toolindex]->size;
*/
         HYPERLYNXPadstack *p = new HYPERLYNXPadstack;
         padstackarray.SetAtGrow(padstackcnt,p);  

         padstackarray[padstackcnt]->name = block->getName();
         padstackarray[padstackcnt]->block_num = block->getBlockNumber();
         padstackarray[padstackcnt]->layerflg = 0;

         fprintf(fp,"{PADSTACK=%s, %lg\n",block->getName(), cnv_units(drill * scale));
         HYPERLYNX_WritePADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0,
            scale, 0, -1, cnv_units(drill * scale));
         fprintf(fp,"}\n");
         padstackcnt++;
      }
      else
      if ( (block->getBlockType() == BLOCKTYPE_PADSHAPE))
      {
         MessageBox(NULL, "PADSTACK with Graphic not yet supported !","HyperLynx Output", MB_OK | MB_ICONHAND);
      }
   }

   return 1;
}

//--------------------------------------------------------------
static int do_drill(FILE *fp, double scale)
{
   int      first = TRUE;

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
         if (!first) fprintf(fp,",");
         fprintf(fp," %lg",cnv_units(drill * scale));
         first = FALSE;
      }
   }

   return 1;
}

//--------------------------------------------------------------
static int do_routes(FILE *fp, FileStruct *file,CNetList *NetList)
{
   NetStruct *net;
   POSITION  netPos;
   
   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      fprintf(fp, "{NET=%s\n", net->getNetName());
      do_signalpinloc(fp, net, file->getScale());
      // order by signal name
      HYPERLYNX_WriteROUTESData(fp,&(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale(), 0, -1, net->getNetName());
      fprintf(fp,"}\n\n");
   }
   return 1;
}

//--------------------------------------------------------------
void HYPERLYNX_WritePADSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

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
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               double   llx, lly, uux, uuy;
               if (PolyIsRectangle(poly, &llx, &lly, &uux, &uuy))
               {
                  Point2   p[5];
                  int      cnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     if (cnt < 5)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        p[cnt].x = pnt->x * scale;
                        if (mirror) p[cnt].x = -p[cnt].x;
                        p[cnt].y = pnt->y * scale;
                        p[cnt].bulge = pnt->bulge;
                        TransPoint2(&p[cnt], 1, &m, insert_x, insert_y);
                        cnt++;
                     }
                     else
                     {
                        MessageBox(NULL, "Marked as Rectangle is wrong !","HyperLynx Output", MB_OK | MB_ICONHAND);
                        break;
                     }
                  }
                  // here rectangle
                  // wGENCAD_Graph_Rectangle( p[0].x, p[0].y, p[2].x, p[2].y);
               }
               else
               {  // not rectangle

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p2.x = pnt->x * scale;
                     if (mirror) p2.x = -p2.x;
                     p2.y = pnt->y * scale;
                     p2.bulge = pnt->bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (first)
                     {
                        firstp = p2;
                        p1 = p2;
                        first = FALSE;
                     }
                     else
                     {
                        // wGENCAD_Graph_Line( p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        p1 = p2;
                     }
                  }
               } /// not rectangle
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            // no text in PADSSHAPES
         break;
         
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

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
                  // here write padshape which is an aperture
                  fprintf(wfp,"ATTRRIBUTE APERTURE found\n");
/*
                     wGENCAD_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * unitsFactor,
                        block->getSizeB() * unitsFactor,
                        block->getShape(), np->getInsert().angle);
*/
            }
            else // not aperture
            {
               int block_layer = -1;
               HYPERLYNX_WritePADSHAPEData(wfp, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end HYPERLYNX_WritePADSHAPEData */
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
static int do_signalpinloc(FILE *fp, NetStruct *net, double scale)
{
   CompPinStruct *compPin;
   POSITION compPinPos;
   
   compPinPos = net->getHeadCompPinPosition();
   while (compPinPos != NULL)
   {
      compPin = net->getNextCompPin(compPinPos);

      if (compPin->getPinCoordinatesComplete())
      {
         double  pinx, piny, pinrot;
         int     padmirror;
         CString padstackname;

         pinx = compPin->getOriginX();
         piny = compPin->getOriginY();
         pinrot = RadToDeg(compPin->getRotationRadians());
         padmirror = compPin->getMirror();

         if (compPin->getPadstackBlockNumber() > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
            padstackname = block->getName();
         }

         fprintf(fp, "(PIN X=%lg Y=%lg R=%s.%s P=%s)\n",
               cnv_units(pinx*scale), cnv_units(piny*scale),
               compPin->getRefDes(), compPin->getPinName(),padstackname);
      }
   }
   return 1;
}                                                        

/****************************************************************************/
/*
*/
static int load_HYPERLYNXsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"HYPERLYNX Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".KEY"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            HYPER_KEY = lp;
         }
      }
   }

   fclose(fp);
   return 1;
}

// 
static void do_layerlist()
{
   LayerStruct *layer;

   for (int j=0; j< doc->getMaxLayerIndex(); j++)
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.

      if (layer->getElectricalStackNumber())
      {
         int signr = layer->getElectricalStackNumber();

         if (signr)
         {
            if (max_signalcnt < signr)
               max_signalcnt = signr;

            if (siglaycnt < MAX_SIGNALLAYERS)
            {
               siglay[siglaycnt].name = layer->getName();
               siglay[siglaycnt].number = signr;
               siglay[siglaycnt].index = layer->getLayerIndex();
               siglaycnt++;
            }
            else
            {
               // error;
               CString tmp;
               tmp.Format("Too many signal layers entries");
               MessageBox(NULL, tmp,"HYPERLYNX read", MB_OK | MB_ICONHAND);
            }
         }
         else
         {
            fprintf(flog,"Layer [%s] marked as %s but no valid number [%d] is assigned\n",
               layer->getName(), layerTypeToString(layer->getLayerType()), signr);
            display_error++;
         }
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         if (siglaycnt < MAX_SIGNALLAYERS)
         {
            siglay[siglaycnt].name = layer->getName();
            siglay[siglaycnt].number = LAY_ALL;
            siglay[siglaycnt].index = layer->getLayerIndex();
            siglaycnt++;
         }
         else
         {
            // error;
            CString tmp;
            tmp.Format("Too many signal layers entries");
            MessageBox(NULL, tmp,"HYPERLYNX read", MB_OK | MB_ICONHAND);
         }
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         if (siglaycnt < MAX_SIGNALLAYERS)
         {
            siglay[siglaycnt].name = layer->getName();
            siglay[siglaycnt].number = LAY_INNER;
            siglay[siglaycnt].index = layer->getLayerIndex();
            siglaycnt++;
         }
         else
         {
            // error;
            CString tmp;
            tmp.Format("Too many signal layers entries");
            MessageBox(NULL, tmp,"HYPERLYNX read", MB_OK | MB_ICONHAND);
         }
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         if (siglaycnt < MAX_SIGNALLAYERS)
         {
            siglay[siglaycnt].name = layer->getName();
            siglay[siglaycnt].number = LAY_TOP;
            siglay[siglaycnt].index = layer->getLayerIndex();
            siglaycnt++;
         }
         else
         {
            // error;
            CString tmp;
            tmp.Format("Too many signal layers entries");
            MessageBox(NULL, tmp,"HYPERLYNX read", MB_OK | MB_ICONHAND);
         }
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         if (siglaycnt < MAX_SIGNALLAYERS)
         {
            siglay[siglaycnt].name = layer->getName();
            siglay[siglaycnt].number = LAY_BOT;
            siglay[siglaycnt].index = layer->getLayerIndex();
            siglaycnt++;
         }
         else
         {
            // error;
            CString tmp;
            tmp.Format("Too many signal layers entries");
            MessageBox(NULL, tmp,"HYPERLYNX read", MB_OK | MB_ICONHAND);
         }
      }
   }
   return;
}

//-----------------------------------------------------------------------------
static int write_layer(FILE *fp)
{
   int   i,ii;

   for (i=1; i <=max_signalcnt;i++)
   {
      int   done = FALSE;
      for (ii=0;ii<siglaycnt && !done;ii++)
      {
         if (siglay[ii].number == i)
         {
            // SIGNAL DIALECTRIC PLANE
            fprintf(fp,"(SIGNAL T=0.0 L=%s)\n",siglay[ii].name);  
            done = TRUE;
         }
      }
   }
   return 1;
}

//-----------------------------------------------------------------------------
void HYPERLYNX_WriteFiles(const char *filename, CCEtoODBDoc *Doc,FormatStruct *format,
                       int page_units, double UnitsFactor, const char *includeFile)
{
   FILE *isf;
   FileStruct *file;

   int pcb_found = FALSE;
   doc = Doc;
   display_error = FALSE;
   PageUnits = page_units;

   CString logFile = GetLogfilePath("hyper.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString settingsFile( getApp().getExportSettingsFilePath("hyper.out") );
   load_HYPERLYNXsettings(settingsFile);

   // Set Static variables so don't have to pass them every time
   unitsFactor = UnitsFactor;

   max_signalcnt = siglaycnt = 0;

   padstackarray.SetSize(100,100);
   padstackcnt = 0;
   
   // open file for writting
   if ((isf = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %-70s",filename);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }

   fprintf(isf,"{VERSION=2.01}\n\n");

   // units are either INCH or CENTIMETER
   switch (page_units)
   {
      case UNIT_HPPLOTTER:
         fprintf(isf, "{UNITS=ENGLISH LENGTH}\n");
         unitsFactor *= 1/1016;
      break;
      case UNIT_INCHES:
         fprintf(isf, "{UNITS=ENGLISH LENGTH}\n\n");
         unitsFactor *= 1;
      break;
      case UNIT_MM:
         fprintf(isf, "{UNITS=METRIC LENGTH}\n\n");
         unitsFactor *= 0.1;
      break;
      case UNIT_MILS:
         fprintf(isf, "{UNITS=ENGLISH LENGTH}\n\n");
         unitsFactor *= 0.001;
      break;
      case UNIT_TNMETER:
         fprintf(isf, "{UNITS=ENGLISH LENGTH}\n\n");
         unitsFactor *= 1.0/2540000;
      break;
   }

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         do_layerlist();

         generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

         pcb_found = TRUE;

         fprintf(isf,"{BOARD\n");
         HYPERLYNX_WritePRIMARYBOARDData(isf, &(file->getBlock()->getDataList()), 
            file->getInsertX(), file->getInsertY(),
            file->getRotation(), file->isMirrored(), file->getScale(), 0, -1,
            GR_CLASS_BOARDOUTLINE);
         fprintf(isf,"}\n\n");

         fprintf(isf,"{STACKUP\n");
         write_layer(isf);
         fprintf(isf,"}\n\n");

         // write all parts
         fprintf(isf,"{DEVICES\n");
         HYPERLYNX_WriteCOMPONENTData(isf,&(file->getBlock()->getDataList()), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale(), 0, -1);
         fprintf(isf,"}\n\n");

         if (max_signalcnt < 2)
         {
            fprintf(flog,"Error : Less than 2 signal layers found\n");
            display_error++;
            break;
         }

         do_padstacks(isf,file->getScale());
         fprintf(isf,"\n");

         // routes
         do_routes(isf, file, &file->getNetList());
         break; // do only one pcb file
      } // if pcb file
   }
   // board file end
   fprintf(isf,"{END}\n");
   // board_xy end
   fprintf(isf,"\n%s\n", HYPER_KEY);
   fclose(isf);

   int   i;
   for (i=0;i<padstackcnt;i++)
   {
      delete padstackarray[i];
   }
   padstackarray.RemoveAll();
   
   if (!pcb_found)
   {
      MessageBox(NULL, "No PCB file found !","HYPERLYNX Output", MB_OK | MB_ICONHAND);
      return;
   }

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

/*****************************************************************************
* 
   wHYPER_Graph_PERIMETER
*/
static void wHYPER_Graph_ArcPerimeter(FILE *fp, double xc, double yc, double r, double sa, double da)
{
   double   x1,y1,x2,y2;

   x1 = cos(DegToRad(sa)) * r + xc;
   y1 = sin(DegToRad(sa)) * r + yc;
   x2 = cos(DegToRad(sa+da)) * r + xc;
   y2 = sin(DegToRad(sa+da)) * r + yc;

   // HYPERLYNX is clockwise
   if (da > 0)
      fprintf(fp,"(PERIMETER_ARC X1=%lg Y1=%lg X2=%lg Y2=%lg XC=%lg YC=%lg R=%lg)\n",
         x2,y2,x1,y1,xc,yc,r);
   else
      fprintf(fp,"(PERIMETER_ARC X1=%lg Y1=%lg X2=%lg Y2=%lg XC=%lg YC=%lg R=%lg)\n",
         x1,y1,x2,y2,xc,yc,r);
   return;
}


/*****************************************************************************
* 
   wHYPER_Graph_PERIMETER
*/
static void wHYPER_Graph_Perimeter(FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2 )
{
   // wGENCAD_Graph_Rectangle( points[0].x, points[0].y, points[2].x, points[2].y);
   // here check also for bulge
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;
      wHYPER_Graph_ArcPerimeter(fp,cx, cy, r, sa, da);
   }
   else
   {
      if (x1 == x2 && y1 == y2)  return;  // eliminate  segments which are Zero;
      fprintf(fp,"(PERIMETER_SEGMENT X1=%lg Y1=%lg X2=%lg Y2=%lg)\n",x1,y1,x2,y2);
   }

   return;
}


//--------------------------------------------------------------
void HYPERLYNX_WritePRIMARYBOARDData(FILE *isf,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, long graphic_class)
{
   Mat2x2 m;
   DataStruct *np;
   int layer;
   int outline_found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getGraphicClass() != graphic_class)
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
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  if (first)
                  {
                     firstp = p2;
                     p1 = p2;
                     first = FALSE;
                  }
                  else
                  {
                     wHYPER_Graph_Perimeter(isf, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                     p1 = p2;
                  }
               }
               outline_found++;
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            /* No Text is Board section */
         break;
         
         case T_INSERT:
            // no insert , graphic definitions are done in ARTWORKS
         break;
      } // end switch
   } // end HYPERLYNX_WritePRIMARYBOARDData */

   if (!outline_found)
   {
      // error - can not open the file.
      fprintf(flog,"Did not find a BoardOutline.");
      display_error++;
   }
   return;
}

//--------------------------------------------------------------
void HYPERLYNX_WriteCOMPONENTData(FILE *isf, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;

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

            if (block_mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               CString compname = np->getInsert()->getRefname();
               Attrib *a;
               if (a =  is_attvalue(doc, np->getAttributesRef(), "?", 2))
                  fprintf(isf,"(%s",get_attvalue_string(doc, a));
               else
                  fprintf(isf,"(?");

               fprintf(isf," REF=%s",compname);

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
                  fprintf(isf," NAME=%s",get_attvalue_string(doc, a));
               else
                  fprintf(isf," NAME=%s",block->getName());

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_VALUE, 2))
                  fprintf(isf," VAL=%s",get_attvalue_string(doc, a));

               fprintf(isf," L=%s",(block_mirror)?siglay[max_signalcnt].name:siglay[0].name);
               fprintf(isf,")\n");
               break;
            }
         } // case INSERT
         break;
      } // end switch
   } // end HYPERLYNX_WriteCOMPONENTData */
}

//--------------------------------------------------------------
static const char *get_blockpadname(CDataList *b)
{
   DataStruct *np;
   int   fidcnt = 0;

   POSITION pos = b->GetHeadPosition();
   while (pos != NULL)
   {
      np = b->GetNext(pos);
      
      if (np->getDataType() == T_INSERT)        
      {

         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            return block->getName();
         }
      }
   }
   return NULL;
}

//--------------------------------------------------------------
static   int   get_layernr_from_index(int index)
{
   int   i;

   for (i=0;i<siglaycnt;i++)
   {
      if (siglay[i].index == index)
         return siglay[i].number;
   }

   return -99;
}

//--------------------------------------------------------------
static int  write_seg(FILE *fp,double xx1, double yy1,double bulge1, 
                      double xx2, double yy2, double bulge2,
                      const char *layer, double width)
{
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;

      ArcPoint2Angle(xx1,yy1,xx2,yy2,da,&cx,&cy,&r,&sa);
      //x1 = cos(DegToRad(sa)) * r + xc;
      //y1 = sin(DegToRad(sa)) * r + yc;
      //x2 = cos(DegToRad(sa+da)) * r + xc;
      //y2 = sin(DegToRad(sa+da)) * r + yc;

      // HYPERLYNX is clockwise
      if (da > 0)
         fprintf(fp,"(ARC X1=%lg Y1=%lg X2=%lg Y2=%lg XC=%lg YC=%lg R=%lg W=%lg L=%s)\n",
            xx2, yy2, xx1, yy1, cx, cy, r, width, layer);
      else
         fprintf(fp,"(ARC X1=%lg Y1=%lg X2=%lg Y2=%lg XC=%lg YC=%lg R=%lg W=%lg L=%s)\n",
            xx1, yy1, xx2, yy2, cx, cy, r, width, layer);

   }
   else
   {
      if (xx1 != xx2 || yy1 != yy2)
         fprintf(fp,"(SEG X1=%lg Y1=%lg X2=%lg Y2=%lg W=%lg L=%s)\n",
            xx1, yy1, xx2, yy2, width, layer);
   }
   return 1;
}

//--------------------------------------------------------------
void HYPERLYNX_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   CString  lay;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != GR_CLASS_ETCH)      continue;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         LayerStruct *l = doc->FindLayer(layer);

         lay = l->getName();

         // here check if ATTR_NETNAME == netname
         Attrib *a =  is_attvalue(doc,np->getAttributesRef(),ATT_NETNAME, 1);
         if (a)
         {
            if (strcmp(get_attvalue_string(doc, a),netname))
               continue;
         }  
         else
            continue;
      }
      else
      {
         // here check if ATTR_NETNAME == netname

         // allow only vias and test points
         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
         {
            if (strcmp(get_attvalue_string(doc, a),netname))
               continue;
         }  
         else
            continue;
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
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               if (polyFilled)
               {
               }
               else
               {
                  int first = TRUE;
                  Point2   firstp;
                  Point2   p1,p2;

                  //wGENCAD_Graph_WidthCode(np->poly.widthIndex);
                  //wGENCAD_Graph_Layer(Get_Layer_Layername(layer));
                  pntPos = poly->getPntList().GetHeadPosition();
                  double width = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p2.x = pnt->x * scale;
                     if (mirror) p2.x = -p2.x;
                     p2.y = pnt->y * scale;
                     p2.bulge = pnt->bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (first)
                     {
                        firstp = p2;
                        p1 = p2;
                        first = FALSE;
                     }
                     else
                     {
                        write_seg(wfp,cnv_units(p1.x),cnv_units(p1.y),p1.bulge,
                           cnv_units(p2.x),cnv_units(p2.y),p2.bulge,
                           lay, width);
                        p1 = p2;
                     }
                  }
               } // not filled
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            // no text in route section
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;

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

            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               Attrib *a;
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_VIALAYER, 1))
                  fprintf(flog,"Via range not implemented\n");
               fprintf(wfp,"(VIA X=%lg Y=%lg P=%s)\n",
                  cnv_units(point2.x), cnv_units(point2.y),block->getName());
               break;
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)            
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();
               HYPERLYNX_WriteROUTESData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, netname);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end HYPERLYNX_WriteROUTESData */
   return;
}

//--------------------------------------------------------------
void HYPERLYNX_WritePADSTACKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, double drill)
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

/* This needs to be done if padshape is needed.
            HYPERLYNX_WritePADSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
               double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
*/
            LayerStruct *lay = doc->FindLayer(block_layer);

            double   padxdim, padydim, padxoff, padyoff;

            int l = get_layernr_from_index(block_layer);
            if (l == -99) // NOT IN SIGLAY LITS
               break;            

            // block has the aperture of a padstack
            switch (block->getShape())
            {
               case T_OCTAGON:
               case T_DONUT:
               case T_SQUARE:
               case T_ROUND:
                  padxdim = padydim = cnv_units(block->getSizeA());
                  padxoff = padyoff = 0;
               break;
               case T_RECTANGLE:
               case T_OBLONG:
                  padxdim = cnv_units(block->getSizeA());
                  padydim = cnv_units(block->getSizeB());
                  padxoff = padyoff = 0;
               break;
               default:
                  // unknown or complex
                  fprintf(flog,"Unevaluated Padform [%s] -> set to 5MIL round\n",shapes[block->getShape()]);
                  display_error++;
                  padxdim = padydim = cnv_units(5 * Units_Factor(UNIT_MILS, PageUnits)); // 5 mil to units
                  padxoff = padyoff = 0;
               break;
            }

            //if (drill > padxdim)  padxdim += drill;
            //if (drill > padydim)  padydim += drill;
            //padxoff = padyoff = 0;

            if (l < 0)
            {

               switch (l)
               {
                  case LAY_TOP:  //       -1
                     padstackarray[padstackcnt]->layerflg |= 1;
                     // pad: layername, layernumber, type, xdim, ydim, xoff, yoff
                     fprintf(wfp,"(%s,0, %lg, %lg, 0)\n",
                        siglay[0].name, padxdim, padydim);
                  break;
                  case LAY_BOT:  //       -2
                     padstackarray[padstackcnt]->layerflg |= 1 << max_signalcnt;
                     fprintf(wfp,"(%s,0, %lg, %lg, 0)\n",
                        siglay[siglaycnt-1].name, padxdim, padydim);
                  break;
                  case  LAY_INNER:  //    -3
                  {
                     for (int i=1;i<max_signalcnt-1;i++)
                     {
                        padstackarray[padstackcnt]->layerflg |= 1 << i;
                        fprintf(wfp,"(%s,0, %lg, %lg, 0)\n",
                           siglay[i].name, padxdim, padydim);
                     }
                  }
                  break;
                  case LAY_ALL:     //    -4
                  {
                     fprintf(wfp,"(MDEF,0, %lg, %lg, 0)\n", padxdim, padydim);
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
               padstackarray[padstackcnt]->layerflg |= 1 << (l-1);
               // pad: layername, layernumber, type, xdim, ydim, xoff, yoff
               if (lay)
                  fprintf(wfp,"(%s,0, %lg, %lg, 0)\n",lay->getName(), padxdim, padydim);
            }
         } // case INSERT
         break;
      } // end switch
   } // end HYPERLYNX_WritePADSTACKData */
}


/****************************************************************************/
/*
   end HYPER_out.CPP
*/
/****************************************************************************/

