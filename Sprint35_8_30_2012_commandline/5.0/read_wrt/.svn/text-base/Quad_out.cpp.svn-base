// $Header: /CAMCAD/5.0/read_wrt/Quad_out.cpp 29    6/17/07 8:59p Kurt Van Ness $

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
#define  MAX_CHECKNAME     1000
#define  MAX_SIGNALLAYERS  255
#define  MAX_PADSTACKS     1000

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

#define  CHECK_NODE     1
typedef struct
{
   char  *origname;
   char  *changename;
   int   typ;     // node or other entities
}Check;

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
}Padstack;

static   Check       *checkname;
static   int         checknamecnt;

extern   CProgressDlg			*progress;
//extern   LayerTypeInfoStruct	layertypes[];

static   CCEtoODBDoc    *doc;
static   double         unitsFactor;

static   Padstack    padstack[MAX_PADSTACKS];
static   int         padstackcnt;

static   Siglay      siglay[MAX_SIGNALLAYERS];
static   int         siglaycnt;
static   int         max_signalcnt;

static   int   display_error;
static   FILE  *flog;
static   int   PageUnits;

// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void QUAD_WritePRIMARYBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, long gclass);

static void QUAD_WriteCOMPONENTData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static void QUAD_WriteROUTESData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname);

static void QUAD_WritePADSTACKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static void QUAD_WritePADSHAPEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static const char *check_names(const char *name, int typ);
static int do_signalpinloc(FILE *fp, NetStruct *net, double scale);
static const char *do_connectmap(unsigned long l, int mirror);

//--------------------------------------------------------------
static long cnv_units(double x)
{
   long l;
   l = (long)floor(x * unitsFactor + 0.5);
   return l;
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
         if (padstackcnt < MAX_PADSTACKS)
         {
            padstack[padstackcnt].name = block->getName();
            padstack[padstackcnt].block_num = block->getBlockNumber();
            padstack[padstackcnt].layerflg = 0;
            fprintf(fp,"PS: %d,\"%s\", %ld, UNKNOWN\n",
               padstackcnt,block->getName(), cnv_units(drill * scale));
            QUAD_WritePADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1);
            padstackcnt++;
         }
         else
         {
            fprintf(flog,"Too many padstacks\n");
            display_error++;
         }
      }
      else
      if ( (block->getBlockType() == BLOCKTYPE_PADSHAPE))
      {
         MessageBox(NULL, "PADSTACK with Graphic not yet supported !","Quad Output", MB_OK | MB_ICONHAND);
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
         fprintf(flog,"Need to insert drill\n");
         if (!first) fprintf(fp,",");
         fprintf(fp," %ld",cnv_units(drill * scale));
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

      fprintf(fp, "NET: \"%s\"\n", net->getNetName());
      do_signalpinloc(fp, net, file->getScale());
      // order by signal name
      QUAD_WriteROUTESData(fp,&(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale(), 0, -1, net->getNetName());
   }
   fprintf(fp,"\n\n");
   return 1;
}

//--------------------------------------------------------------
void QUAD_WritePADSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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
                        MessageBox(NULL, "Marked as Rectangle is wrong !","Quad Output", MB_OK | MB_ICONHAND);
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
               QUAD_WritePADSHAPEData(wfp, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end QUAD_WritePADSHAPEData */
   return;
}

//--------------------------------------------------------------
static int do_signals(FILE *fp, CNetList *NetList)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      int   first   = TRUE;
      int   llen = 0;
      while (compPinPos != NULL)
      {
         if (first)
         {
            llen = fprintf(fp, "   \"%s\" : ", check_names(net->getNetName(), CHECK_NODE));
         }
         else
            llen += fprintf(fp,",");
         first = FALSE;

         if (llen > 80)
            llen = fprintf(fp,"\n    ");

         compPin = net->getNextCompPin(compPinPos);
         llen +=fprintf(fp, " %s %s", compPin->getRefDes(), compPin->getPinName());
      }
      if (!first)
         fprintf(fp,";\n");
   }

   return 1;
}

// 
static const char *do_connectmap(unsigned long l, int mirror)
{
   static CString t;
   int    i;

   t = "";

   for (i=0;i<max_signalcnt;i++)
   {
      if (l & (1<<i))
         t +="1";
      else
         t +="0";
   }

   if (mirror)
      t.MakeReverse();

   return t;
}

//--------------------------------------------------------------
static int get_padstackindex(const char *p)
{
   int   i;
   
   for (i=0;i<padstackcnt;i++)
   {
      if (!padstack[i].name.CompareNoCase(p))
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
         int pi = get_padstackindex(padstackname);
         if (pi > -1)
         {
            fprintf(fp, "PIN: %s, %s, 1, %ld, %ld, %d, %s\n", 
               compPin->getRefDes(), compPin->getPinName(), cnv_units(pinx*scale), cnv_units(piny*scale),
               pi, do_connectmap(padstack[pi].layerflg, padmirror));
         }
         else
         {
            fprintf(flog,"Can not find padstack [%s] in padstacklist\n",padstackname);
            display_error++;
         }
      }
   }
   return 1;
}                                                        

/****************************************************************************/
/*
*/
static const char *check_names(const char *name, int typ)
{
   int   i;
   char  newname[80];

   // here check first that the devicename is a HP device name
   for (i=0;i<checknamecnt;i++)
   {
      if (!STRCMPI(checkname[i].origname,name) && checkname[i].typ == typ)
         return checkname[i].changename;
   }

   strcpy(newname,name);

   switch (typ)
   {
      case CHECK_NODE:
      {
         // no ~ - gets changed to .
         for (i=0;i<(int)strlen(name);i++)
         {

            if (name[i] == '~')
            {
               newname[i] = '.';
            }
            else
            if (isspace(name[i]))
            {
               newname[i] = '_';
            }
            else
               newname[i] = name[i];
         }
      }
      break;
      default:
      break;
   }
   newname[i] = '\0';

   // no modification done
   if (!strcmp(newname,name))
      return name;

   if (checknamecnt < MAX_CHECKNAME)
   {
      if ((checkname[checknamecnt].origname = STRDUP(name)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      if ((checkname[checknamecnt].changename = STRDUP(newname)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      checkname[checknamecnt].typ = typ;
      checknamecnt++;
   }
   else
   {
      // error
      CString tmp;
      tmp.Format("Too many check entries");
      MessageBox(NULL, tmp,"QUAD read", MB_OK | MB_ICONHAND);
   }
   return checkname[checknamecnt-1].changename;
}

/****************************************************************************/
/*
*/
static int load_QUADsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"QUAD Settings", MB_OK | MB_ICONHAND);
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
               MessageBox(NULL, tmp,"QUAD read", MB_OK | MB_ICONHAND);
            }
         }
         else
         {
            fprintf(flog,"Layer [%s] marked as %s but no valid number [%d] is assigned\n",
               layer->getName(),layerTypeToString(layer->getLayerType()), signr);
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
            MessageBox(NULL, tmp,"QUAD read", MB_OK | MB_ICONHAND);
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
            MessageBox(NULL, tmp,"QUAD read", MB_OK | MB_ICONHAND);
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
            MessageBox(NULL, tmp,"QUAD read", MB_OK | MB_ICONHAND);
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
            MessageBox(NULL, tmp,"QUAD read", MB_OK | MB_ICONHAND);
         }
      }
   }
   return;
}

//--------------------------------------------------------------
void QUAD_WriteFiles(const char *filename, CCEtoODBDoc *Doc,FormatStruct *format,
                       int page_units, double UnitsFactor, const char *includeFile)
{
   FILE *isf, *qnf;
   FileStruct *file;

   int   pcb_found = FALSE;
   doc = Doc;
   display_error = FALSE;
   PageUnits = page_units;

   // if pageunits == HP return
   if (page_units == UNIT_HPPLOTTER)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Page UNITS must be INCHES, MILS, MM, NANOMETER");
      MessageBox(NULL, tmp,"Error Units", MB_OK | MB_ICONHAND);
      return;
   }

   CString logFile = GetLogfilePath("QUAD.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   if ((checkname = (Check  *)calloc(MAX_CHECKNAME,sizeof(Check))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   checknamecnt = 0;     

   CString settingsFile( getApp().getExportSettingsFilePath("QUAD.out") );
   load_QUADsettings(settingsFile);

   // Set Static variables so don't have to pass them every time
   unitsFactor = UnitsFactor;

   max_signalcnt = siglaycnt = 0;
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

   // here check it there is a panel file also
   // open file for writting
   char f[_MAX_PATH];
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( filename, drive, dir, fname, ext );
   strcpy(f,drive);
   strcat(f,dir);
   strcat(f,fname);
   strcat(f,".qnf");
   
   if ((qnf = fopen(f, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %-70s",f);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);             
      return;
   }

   // board file include

   // board file heading

   fprintf(isf,"# Start of QUAD isf file (%s)\n", filename);
   fprintf(isf,"# Created by %s\n", getApp().getCamCadTitle());
   fprintf(isf,"\n");
   fprintf(isf,"ISF: 2 0\n");

   fprintf(qnf,"# Start of QUAD qnf file (%s)\n", f);
   fprintf(qnf,"# Created by %s\n", getApp().getCamCadTitle());
   fprintf(qnf,"\n");
   fprintf(qnf,"DESIGN = %s\n",fname);
   fprintf(qnf,"DEFAULTS\n");

   CString w;
   w.Format("%s + %s",filename,f);
   progress->SetStatus(w);

   // board_xy scale
   // board_xy units
   switch (page_units)
   {
      case UNIT_INCHES:
         fprintf(isf, "UNITS: 10000\n");
         fprintf(qnf, "   UNITS_PER_INCH = 10000;\n");
         unitsFactor *= 10000;
      break;
      case UNIT_MM:
         fprintf(isf, "UNITS: 254\n");
         fprintf(qnf, "   UNITS_PER_INCH = 254;\n");
         unitsFactor *= 10;
      break;
      case UNIT_MILS:
         fprintf(isf, "UNITS: 10\n");
         fprintf(qnf, "   UNITS_PER_INCH = 10;\n");
         unitsFactor *= 10;
      break;
      case UNIT_TNMETER:
         fprintf(isf, "UNITS: 1\n");
         fprintf(qnf, "   UNITS_PER_INCH = 2540000;\n");
      break;
   }

   fprintf(qnf,"   ROTATION = 0;\n");
   fprintf(qnf,"END_DEFAULTS\n\n");

   fprintf(isf,"INTERFACE: %s\n", getApp().getCamCadTitle());

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         // isf  graphic
         // qnf  netlist
         do_layerlist();

         generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

         pcb_found = TRUE;

         // write all parts
         fprintf(qnf,"PARTS\n");
         QUAD_WriteCOMPONENTData(qnf,&(file->getBlock()->getDataList()), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale(), 0, -1);
         fprintf(qnf,"END_PARTS\n\n");

         fprintf(qnf,"NETDEF\n");
         do_signals(qnf,&file->getNetList());
         fprintf(qnf,"END_NETDEF\n\n");
         
         fprintf(isf,"\n");

         fprintf(isf,"W: ");
			int i=0;
         for (i=0;i<doc->getNextWidthIndex();i++)
         {
            if (i > 0)
               fprintf(isf,",");
            fprintf(isf," %ld",cnv_units(doc->getWidthTable()[i]->getSizeA() * file->getScale()));
         }
         fprintf(isf,"\n");

         fprintf(isf,"D: ");
         do_drill(isf,file->getScale());
         fprintf(isf,"\n");

         fprintf(isf,"Q: ");
         do_drill(isf,file->getScale());
         fprintf(isf,"\n");

         QUAD_WritePRIMARYBOARDData(isf, &(file->getBlock()->getDataList()), 
            file->getInsertX(), file->getInsertY(),
            file->getRotation(), file->isMirrored(), file->getScale(), 0, -1,
            GR_CLASS_BOARDOUTLINE);

         if (max_signalcnt < 2)
         {
            fprintf(flog,"Error : Less than 2 signal layers found\n");
            display_error++;
            break;
         }

         for (i=1; i <=max_signalcnt;i++)
         {
            int   done = FALSE;
            for (int ii=0;ii<siglaycnt && !done;ii++)
            {
               if (siglay[ii].number == i)
               {
                  fprintf(isf,"X: %s, %d\n",siglay[ii].name,i-1); // starts from 0 to n-1
                  done = TRUE;
               }
            }
         }
         fprintf(isf,"\n");

         do_padstacks(isf,file->getScale());
         fprintf(isf,"\n");

         // routes
         do_routes(isf, file, &file->getNetList());
         break; // do only one pcb file
      } // if pcb file
   }
   // board file end
   fprintf(isf,"\n# END\n");
   // board_xy end
   fprintf(qnf,"\nEND_DESIGN\n");
   fclose(isf);
   fclose(qnf);
   
   for (int i=0;i<checknamecnt;i++)
   {
      free(checkname[i].origname);
      free(checkname[i].changename);
   }
   free(checkname);
       
   if (!pcb_found)
   {
      MessageBox(NULL, "No PCB file found !","QUAD Output", MB_OK | MB_ICONHAND);
      return;
   }

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

//--------------------------------------------------------------
void QUAD_WritePRIMARYBOARDData(FILE *isf,CDataList *DataList, double insert_x, double insert_y,
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

         // If segment has a netname, do not write
         // here check if ATTR_NETNAME == netname
         if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 0))
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

               if (!closed)   break;   // must be a closed board outline.

               Point2   p;

               int vertexcnt = 0;
               double xmin, ymin, xmax, ymax;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x * scale;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  if (!vertexcnt)
                  {
                     xmax = xmin = p.x;
                     ymax = ymin = p.y;
                  }
                  else
                  {
                     xmin = min(xmin,p.x);
                     ymin = min(ymin,p.y);
                     xmax = max(xmax,p.x);
                     ymax = max(xmax,p.y);
                  }

                  if (fabs(p.bulge) > BULGE_THRESHOLD)
                  {
                     int     i, ppolycnt;
                     Point2  ppoly[255];
                     double cx,cy,r,sa;
                     Point2 p2;
                     double da = atan(p.bulge) * 4;

                     CPnt *pnt1 = poly->getPntList().GetNext(pntPos);

                     p2.x = pnt1->x * scale;
                     if (mirror) p2.x = -p2.x;
                     p2.y = pnt1->y * scale;
                     p2.bulge = pnt1->bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);

                     ArcPoint2Angle(p.x,p.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                     // make positive start angle.
                     if (sa < 0) sa += PI2;
                     ppolycnt = 255;
                     // start center
                     ArcPoly2(p.x,p.y,cx,cy,da,
                        1.0,ppoly,&ppolycnt,DegToRad(5));   // 5 degree

                     for (i=0;i<ppolycnt;i++)
                     {
                        xmin = min(xmin,ppoly[i].x);
                        ymin = min(ymin,ppoly[i].y);
                        xmax = max(xmax,ppoly[i].x);
                        ymax = max(xmax,ppoly[i].y);
                     }
                     vertexcnt += ppolycnt;
                  }
                  else
                     vertexcnt++;
               }        

               fprintf(isf,"B: %d, %ld, %ld, %ld, %ld\n",
                  vertexcnt,cnv_units(xmin), cnv_units(xmax), cnv_units(ymin), cnv_units(ymax));

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
                  if (fabs(p.bulge) > BULGE_THRESHOLD)
                  {
                     int     i, ppolycnt;
                     Point2  ppoly[255];
                     double cx,cy,r,sa;
                     Point2 p2;
                     double da = atan(p.bulge) * 4;

                     CPnt *pnt1 = poly->getPntList().GetNext(pntPos);

                     p2.x = pnt1->x * scale;
                     if (mirror) p2.x = -p2.x;
                     p2.y = pnt1->y * scale;
                     p2.bulge = pnt1->bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);

                     ArcPoint2Angle(p.x,p.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                     // make positive start angle.
                     if (sa < 0) sa += PI2;

                     ppolycnt = 255;
                     // start center
                     ArcPoly2(p.x,p.y,cx,cy,da,
                        1.0,ppoly,&ppolycnt,DegToRad(5));   // 5 degree

                     for (i=0;i<ppolycnt;i++)
                     {
                        fprintf(isf,"VER: %ld, %ld\n",cnv_units(ppoly[i].x), cnv_units(ppoly[i].y));
                     }
                  }
                  else
                     fprintf(isf,"VER: %ld, %ld\n",cnv_units(p.x), cnv_units(p.y));
               }        
               fprintf(isf,"\n");
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
   } // end QUAD_WritePRIMARYBOARDData */

   if (!outline_found)
   {
      // error - can not open the file.
      fprintf(flog,"Did not find a closed Polygon as BoardOutline.");
      display_error++;
   }
   else
   if (outline_found > 1)
   {
      // error - can not open the file.
      fprintf(flog,"Multiple closed Polygon as BoardOutline found.");
      display_error++;
   }
   return;
}

//--------------------------------------------------------------
void QUAD_WriteCOMPONENTData(FILE *isf, CDataList *DataList, double insert_x, double insert_y,
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
               CString compname = "COMPNAME";
               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_REFNAME, 1);

               if (a)
               {
                  if (a->getValueType() == VT_STRING)
                     compname = a->getStringValue();
               }

               fprintf(isf,"   %s = ",compname);

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
                  fprintf(isf," \"%s\"",get_attvalue_string(doc, a));
               else
                  fprintf(isf," \"%s\"",block->getName());

               fprintf(isf,"( ");                  
               fprintf(isf,"X_POS =  %ld, Y_POS = %ld, ",cnv_units(point2.x), cnv_units(point2.y));
               fprintf(isf,"ROT = %lg, ",RadToDeg(block_rot));
               fprintf(isf,"PL = %s, ",(block_mirror)?"BOTTOM":"TOP");
               fprintf(isf,"PKG = \"%s\"",block->getName());
               fprintf(isf,");\n");
               break;
            }
         } // case INSERT
         break;
      } // end switch
   } // end QUAD_WriteCOMPONENTData */
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
                      int layernr, int widthindex)
{
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      int     i, ppolycnt;
      Point2  ppoly[255];
      double cx,cy,r,sa;

      ArcPoint2Angle(xx1,yy1,xx2,yy2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;

      ppolycnt = 255;
      // start center
      ArcPoly2(xx1,yy1,cx,cy,da,
            1.0,ppoly,&ppolycnt,DegToRad(5));   // 5 degree

      for (i=1;i<ppolycnt;i++)
      {
         write_seg(fp,ppoly[i-1].x, ppoly[i-1].y, 0.0, 
                      ppoly[i].x, ppoly[i].y, 0.0,layernr, widthindex);
      }
   }
   else
   {
      long x1 = cnv_units(xx1);
      long y1 = cnv_units(yy1);
      long x2 = cnv_units(xx2);
      long y2 = cnv_units(yy2);

      if (x1 != x2 || y1 != y2)
         fprintf(fp,"SEG: %ld, %ld, %ld, %ld, %d , %d\n",
            x1, y1, x2, y2, layernr, widthindex);
   }
   return 1;
}

//--------------------------------------------------------------
void QUAD_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   int   viacnt = 0;
   int   layernr;

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

         // here check if ATTR_NETNAME == netname
         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

         if (a)
         {
            if (strcmp(get_attvalue_string(doc, a),netname))
               continue;
         }  
         else
            continue;
         if ((layernr = get_layernr_from_index(layer) - 1) < 0)
            continue;   // not an electrical layer
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
                  Point2   p;
                  int vertexcnt = 0;
                  double xmin, ymin, xmax, ymax;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     if (!vertexcnt)
                     {
                        xmax = xmin = p.x;
                        ymax = ymin = p.y;
                     }
                     else
                     {
                        xmin = min(xmin,p.x);
                        ymin = min(ymin,p.y);
                        xmax = max(xmax,p.x);
                        ymax = max(xmax,p.y);
                     }

                     if (fabs(p.bulge) > BULGE_THRESHOLD)
                     {
                        int     i, ppolycnt;
                        Point2  ppoly[255];
                        double cx,cy,r,sa;
                        Point2 p2;
                        double da = atan(p.bulge) * 4;
      
                        CPnt *pnt1 = poly->getPntList().GetNext(pntPos);

                        p2.x = pnt1->x * scale;
                        if (mirror) p2.x = -p2.x;
                        p2.y = pnt1->y * scale;
                        p2.bulge = pnt1->bulge;
                        TransPoint2(&p2, 1, &m, insert_x, insert_y);

                        ArcPoint2Angle(p.x,p.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                        // make positive start angle.
                        if (sa < 0) sa += PI2;
                        ppolycnt = 255;
                        // start center
                        ArcPoly2(p.x,p.y,cx,cy,da,
                           1.0,ppoly,&ppolycnt,DegToRad(5));   // 5 degree

                        for (i=0;i<ppolycnt;i++)
                        {
                           xmin = min(xmin,ppoly[i].x);
                           ymin = min(ymin,ppoly[i].y);
                           xmax = max(xmax,ppoly[i].x);
                           ymax = max(xmax,ppoly[i].y);
                        }
                        vertexcnt += ppolycnt;
                     }
                     else
                        vertexcnt++;
                  }        

                  fprintf(wfp,"AREA: %d, %ld, %ld, %ld, %ld,%d,  SOLID, POS\n",
                     vertexcnt,cnv_units(xmin), cnv_units(xmax), 
                     cnv_units(ymin), cnv_units(ymax), layernr);

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
                     if (fabs(p.bulge) > BULGE_THRESHOLD)
                     {
                        int     i, ppolycnt;
                        Point2  ppoly[255];
                        double cx,cy,r,sa;
                        Point2 p2;
                        double da = atan(p.bulge) * 4;

                        CPnt *pnt1 = poly->getPntList().GetNext(pntPos);

                        p2.x = pnt1->x * scale;
                        if (mirror) p2.x = -p2.x;
                        p2.y = pnt1->y * scale;
                        p2.bulge = pnt1->bulge;
                        TransPoint2(&p2, 1, &m, insert_x, insert_y);

                        ArcPoint2Angle(p.x,p.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                        // make positive start angle.
                        if (sa < 0) sa += PI2;

                        ppolycnt = 255;
                        // start center
                        ArcPoly2(p.x,p.y,cx,cy,da,
                           1.0,ppoly,&ppolycnt,DegToRad(5));   // 5 degree

                        for (i=0;i<ppolycnt;i++)
                        {
                           fprintf(wfp,"VER: %ld, %ld\n",cnv_units(ppoly[i].x), cnv_units(ppoly[i].y));
                        }
                     }
                     else
                        fprintf(wfp,"VER: %ld, %ld\n",cnv_units(p.x), cnv_units(p.y));
                  } // while        
                  fprintf(wfp,"\n");
               }  
               else
               {
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
                        write_seg(wfp,p1.x,p1.y,p1.bulge,p2.x,p2.y,p2.bulge,
                           layernr, poly->getWidthIndex());
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
               int pi = get_padstackindex(block->getName());

               if (pi > -1)
                  fprintf(wfp,"VIA: %ld, %ld, %d, %s\n",
                     cnv_units(point2.x), cnv_units(point2.y),pi,
                     do_connectmap(0xffffffff,0));
               else
               {
                  fprintf(flog,"Via index [%s] not found\n",block->getName());
                  display_error++;
               }
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
               QUAD_WriteROUTESData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, netname);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end QUAD_WriteROUTESData */
   return;
}

//--------------------------------------------------------------
void QUAD_WritePADSTACKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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

/* This needs to be don if padshape is needed.
            QUAD_WritePADSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
               double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
*/
            LayerStruct *lay = doc->FindLayer(block_layer);

            long  padxdim, padydim, padxoff, padyoff;

            padxdim = padydim = cnv_units(5 * Units_Factor(UNIT_MILS, PageUnits)); // 5 mil to units
            padxoff = padyoff = 0;
            int l = get_layernr_from_index(block_layer);
            if (l == -99) // NOT IN SIGLAY LITS
               break;            

            if (l < 0)
            {

               switch (l)
               {
                  case LAY_TOP:  //       -1
                     padstack[padstackcnt].layerflg |= 1;
                     // pad: layername, layernumber, type, xdim, ydim, xoff, yoff
                     fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","TOP", 0,
                        padxdim, padydim, padxoff, padyoff);
                  break;
                  case LAY_BOT:  //       -2
                     padstack[padstackcnt].layerflg |= 1 << (max_signalcnt-1);
                     fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","BOTTOM", max_signalcnt-1,
                        padxdim, padydim, padxoff, padyoff);
                  break;
                  case  LAY_INNER:  //    -3
                  {
                     for (int i=1;i<max_signalcnt-1;i++)
                     {
                        padstack[padstackcnt].layerflg |= 1 << i;
                        fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","INNER", i,
                           padxdim, padydim, padxoff, padyoff);
                     }
                  }
                  break;
                  case LAY_ALL:     //    -4
                  {
                     for (int i=0;i<max_signalcnt;i++)
                     {
                        padstack[padstackcnt].layerflg |= 1 << i;
                        fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n","INNER", i,
                           padxdim, padydim, padxoff, padyoff);
                     }
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
               padstack[padstackcnt].layerflg |= 1 << (l-1);
               // pad: layername, layernumber, type, xdim, ydim, xoff, yoff
               fprintf(wfp,"PAD: %s, %d, R, %ld, %ld, %ld, %ld\n",lay->getName(), l-1,
                  padxdim, padydim, padxoff, padyoff);

            }
         } // case INSERT
         break;
      } // end switch
   } // end QUAD_WritePADSTACKData */
}


/****************************************************************************/
/*
   end QUAD_out.CPP
*/
/****************************************************************************/

