// $Header: /CAMCAD/5.0/read_wrt/GenCam_o.cpp 54    6/17/07 8:57p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 2002. All Rights Reserved.

  Attributes need to get a register lookup in GENCAM.OUT

  GenCAM syntax checker available: http://www.gencam.org/ctmupload2.html 
*/
                              
#include "stdafx.h"
#include "ccdoc.h"   
#include "gencam.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include <math.h>
#include "pcbutil.h"
#include "outline.h"
#include "graph.h"
#include "pcblayer.h"
#include "compvalues.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  GENCAM_VERSION          "1.5"       // final definition Feb-2000
#define  GROUP_PRIMITIVES        1
#define  GROUP_PATTERN           2
#define  GROUP_DEVICES           3
#define  GROUP_LAYERS            4
#define  GROUP_PACKAGES          5
#define  GROUP_PADSTACKS         6
#define  GROUP_ARTWORK           7
#define  GROUP_COMPONENTS        8
#define  GROUP_ROUTES            9
#define  GROUP_FAMILIES          10
#define  GROUP_MECHANICALS       11
#define  GROUP_POWERS            12
#define  GROUP_TESTCONNECTS      13
// layer flags for connect layer table
#define  LAY_TOP                 -1
#define  LAY_BOT                 -2
#define  LAY_INNER               -3
#define  LAY_ALL                 -4
#define  LAY_OUTER               -5

typedef struct
{
   CString  camcadtype;
   CString  gencamtype;
} GENCAMTypemap;
typedef CTypedPtrArray<CPtrArray, GENCAMTypemap*> GENCAMTypemapArray;

typedef struct
{
   CString  netname;
   POSITION pos;
}GENCAMRoutes;
typedef CTypedPtrArray<CPtrArray, GENCAMRoutes*> GENCAMRoutesArray;


static   FileStruct     *cur_file;
static   CCEtoODBDoc     *doc;
static   int            output_units_accuracy = 3;

extern   CProgressDlg   *progress;

static   GENCAMTypemapArray   typemaparray;     // this is the device - mapping
static   int            typemapcnt;

static   GENCAMRoutesArray routesarray;   
static   int            routescnt;

static   int            display_error;
static   FILE           *flog;
static   CString        fiducialname;
static   CString        panelfiducialname;

static   CString        ident;
static   int            identcnt = 0;

static   int            viacnt = 0;
static   int            testpadcnt = 0;
static   int            holecnt = 0;

typedef  CArray<Point2, Point2&> CPolyArray;
static   CPolyArray     polyarray;
static   int            polycnt;

static   CGenCAMLayerArray glArr;
static   int         maxArr = 0;

static   int            SHOWOUTLINE = FALSE; // set in CCM.out

static   double         CONTINUITY_VOLTAGE;
static   double         CONTINUITY_CURRENT;
static   double         ISOLATION_VOLTAGE;

static   CString        toplegend;
CGenCamPadRotArray      PadRotarray;

static void wGENCAM_Graph_Header(FILE *fp, const char *fname, int unit);

static int GENCAM_OutlineBOARDData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
         double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, BOOL CheckGraphicClass, int graphicClass);
static void GENCAM_WritePADSTACKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void GENCAM_WritePRIMARYBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
static void GENCAM_WritePRIMARYHOLEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void GENCAM_WritePRIMARYPANELData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
// this function only loops throu entities marked other than BL_CLASS_BOARD 
static void GENCAM_WriteSHAPEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
static void GENCAM_ArtworkSHAPEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
static void GENCAM_WriteCOMPONENTData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, int filenum);
static void GENCAM_WritePACKAGEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
static void GENCAM_WriteROUTESData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname);
static void GENCAM_WritePANELBOARDSData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, 
      const char *fname, const char *ext);
static void GENCAM_GetROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
static const char *get_groupname(int grouptype, int filenum);
static int get_fiducial(FILE *fp, CDataList *DataList, int filenum, int first);

/****************************************************************************
*   get_XMLDate()
*/
static CString get_XMLDate()
{
   CTime time = CTime::GetCurrentTime();

   return time.Format("%Y-%m-%dT%H:%M:%S:+01:00"); // need to understand timezone.

}

/****************************************************************************/
/*
*/
static int   wgencam_plusident()
{
   identcnt++;
   ident.Format("%*s",identcnt*2, " ");
   return 1;
}

/****************************************************************************/
/*
*/
static int   wgencam_minusident()
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

/****************************************************************************/
/*
   return NULL if not visible
*/
static const char *Layer_Gencam(int l)
{

   for (int i=0;i<maxArr;i++)
   {
      if (glArr[i]->on == 0)  // not visible
         continue;
      if (glArr[i]->layerindex == l)
         return glArr[i]->newName;
   }

   return NULL;
}

/*****************************************************************************/
/*
*/
static int ignore_non_manufacturing_info(DataStruct *np)
{

   if (np->getDataType() != T_INSERT)
   {
      if (np->getGraphicClass() == GR_CLASS_ANTI_ETCH)          
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)      
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)     
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)     
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)    
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPIN)          
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)         
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPIN)       
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPOUT)      
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_UNROUTE)            
         return TRUE;
      if (np->getGraphicClass() == graphicClassTestPointKeepOut)   
         return TRUE;
      if (np->getGraphicClass() == graphicClassTestPointKeepIn)    
         return TRUE;
      if (np->getGraphicClass() == graphicClassPlacementRegion)
         return TRUE;
   }
   else
   {
      if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD)    
         return TRUE;
      if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)
         return TRUE;
      if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE) 
         return TRUE;

   }

   return FALSE;
}

/*****************************************************************************/
/*
*/
static int write_easyoutline(FILE *wfp, CPntList *pp, const char *group, const char *layer)
{
   CPnt  *p;
   
   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;
   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);
      Point2   p2;
      p2.x = p->x;
      p2.y = p->y;
      p2.bulge = p->bulge;

      polyarray.SetAtGrow(polycnt,p2);
      polycnt++;

   }

   if (polycnt == 0)
      return 0;

   Point2 p1, p2;
   p1 = polyarray.ElementAt(0);  

   fprintf(wfp,"%sOUTLINE: \"outline1\", \"%s\".\"%s\";\n",ident, group, layer);
   wgencam_plusident();

   CString  gname = get_groupname(GROUP_PRIMITIVES, -1);
   fprintf(wfp,"%sPOLYGON: \"%s\".\"LINEDESC%d\", \"%s\".\"HOLLOW\";\n",ident, gname, 0, gname);

   wgencam_plusident();

   wGENCAM_Graph_StartAt(ident, wfp, p1.x, p1.y);

   for (int i=1;i<polycnt;i++)
   {
      p2 = polyarray.ElementAt(i);  
      if (i == polycnt-1)
         wGENCAM_Graph_EndAt(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
      else
         wGENCAM_Graph_NextAt(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
      p1 = p2;
   }

   wgencam_minusident();   // polygondef
   wgencam_minusident();   // outline

   if (SHOWOUTLINE)
   {
      doc->PrepareAddEntity(cur_file);       
      int l = Graph_Level("DEBUG","",0);
      DataStruct *d = Graph_PolyStruct(l,0L, FALSE);  // poly is always with 0

      Graph_Poly(NULL,0, 0,0, 0);

      for (int i=0;i<polycnt;i++)
      {
         Point2 p;
         p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge);    // p1 coords
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/******************************************************************************
* GENCAM_OutlineBOARDData
*/
int GENCAM_OutlineBOARDData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, BOOL CheckGraphicClass, int graphicClass)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   int layer;
   int found = 0;
   BOOL blFlag = TRUE;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() == T_TEXT)  
         continue;

      if (data->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // never do etch
         if (data->getGraphicClass() == GR_CLASS_ETCH) 
            continue;

         if (CheckGraphicClass && data->getGraphicClass() != graphicClass)  
            continue;

         Outline_Add(data, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         switch (data->getInsert()->getInsertType())
         {
         case INSERTTYPE_PIN:
         case INSERTTYPE_VIA:
         case INSERTTYPE_PCBCOMPONENT:
         case INSERTTYPE_TESTPOINT:
         case INSERTTYPE_MECHCOMPONENT:
         case INSERTTYPE_TOOLING:
            continue;
         }

         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot = rotation + data->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (! (block->getFlags() & BL_TOOL || block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE) ) // if not tool or aperture, recursive
         {
            int block_layer = -1;

            if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
               block_layer = insertLayer;
            else
               block_layer = data->getLayerIndex();

            found += GENCAM_OutlineBOARDData(wfp, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror,
                  scale * data->getInsert()->getScale(), embeddedLevel+1, block_layer, CheckGraphicClass, graphicClass);
         }
      }
   }

   return found;
}

//--------------------------------------------------------------
static int GENCAM_OutlinePANELData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary,
      int gr_class)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_TEXT)    
         continue;
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // never do etch
         if (np->getGraphicClass() == GR_CLASS_ETCH)   
            continue;

         if (primary)
         {
            if (np->getGraphicClass() != gr_class)  
               continue;
         }
         else
         {
            // do it
         }
         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)         
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)         
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)   
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)  
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)    
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)     
            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_PCB)         
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

         if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               block_layer = insertLayer;
            else
               block_layer = np->getLayerIndex();

            found += GENCAM_OutlinePANELData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, primary, gr_class);
         } // end else not aperture
      } // if INSERT

   } // end GENCAM_OutlinePANELData */

   return found;
}

/****************************************************************************/
/*
*/
static const char *get_groupname(int grouptype, int filenum)
{
   FileStruct  *file;
   static   CString  gname;

   gname = "";

   switch (grouptype)
   {
      case  GROUP_PRIMITIVES:
         gname = "prim.";
      break;
      case  GROUP_PATTERN:
         gname = "pat.";
      break;
      case  GROUP_DEVICES:
         gname = "dev.";
      break;
      case  GROUP_LAYERS:
         gname = "layer.";
      break;
      case  GROUP_PACKAGES:
         gname = "pkg.";
      break;
      case  GROUP_PADSTACKS:
         gname = "pad.";
      break;
      case  GROUP_ARTWORK:
         gname = "art.";
      break;
      case  GROUP_COMPONENTS:
         gname = "cmp.";
      break;
      case  GROUP_FAMILIES:
         gname = "family.";
      break;
      case  GROUP_MECHANICALS:
         gname = "mech.";
      break;
      case  GROUP_POWERS:
         gname = "pwr.";
      break;
      case  GROUP_TESTCONNECTS:
         gname = "test.";
      break;
      case  GROUP_ROUTES:
         gname = "route.";
      break;
      default:
         gname = "nogroup.";
      break;
   }

   if (filenum < 0)
   {
      gname += "global";
      return gname.GetBuffer(0);
   }

   // here get file name
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (file->getFileNumber() == filenum)
      {
         gname += file->getName();
         return gname.GetBuffer(0);
      }
   } 

   gname += "Unknown_Group";
   return gname.GetBuffer(0);
}

//--------------------------------------------------------------
static int update_typemap(const char *cc, const char *gc)
{
   GENCAMTypemap  *c = new GENCAMTypemap;
   typemaparray.SetAtGrow(typemapcnt,c);  
   typemapcnt++;  

   c->camcadtype = cc;
   c->gencamtype = gc;

   return typemapcnt-1;
}

/****************************************************************************/
/*
*/
static int load_GENCAMsettings(CString fname)
{
   FILE  *fp;
   char  line[127];
   char  *lp;

   fiducialname = "fiducial";
   panelfiducialname = "panelfid";
   CONTINUITY_VOLTAGE = 5;
   CONTINUITY_CURRENT = .9;
   ISOLATION_VOLTAGE = 10;
   toplegend = "";

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp,"GENCAM Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,127,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".FIDUCIALNAME"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            fiducialname = lp;
         }
         else
         if (!STRCMPI(lp,".PANELFIDUCIALNAME"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            panelfiducialname = lp;
         }
         else
         if (!STRCMPI(lp,".TYPEMAP"))
         {
            CString  cc, gc;
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            cc = lp;
            cc.TrimLeft();
            cc.TrimRight();
            cc.MakeUpper();
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            gc = lp;
            gc.TrimLeft();
            gc.TrimRight();
            gc.MakeUpper();
            update_typemap(cc,gc);
         }
         else
         if (!STRCMPI(lp,".SHOWOUTLINE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
            {
               SHOWOUTLINE = TRUE;
            }
         }
         else
         if (!STRCMPI(lp,".CONTINUITY_VOLTAGE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            CONTINUITY_VOLTAGE = atof(lp);
         }
         else
         if (!STRCMPI(lp,".CONTINUITY_CURRENT"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            CONTINUITY_CURRENT = atof(lp);
         }
         else
         if (!STRCMPI(lp,".ISOLATION_VOLTAGE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;
            ISOLATION_VOLTAGE = atof(lp);
         }
      }
   }

   fclose(fp);
   return 1;
}

//--------------------------------------------------------------
static void write_attributes(FILE *fp,CAttributes* map, const char *GENCAMname, int *attcnt )
{
   // attribute need to follow a register lookup
   return;

   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;
   int   GENCAMcnt = *attcnt;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))   
         continue;

      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))
         continue;

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp,"ATTRIBUTE: \"%s_%d\", \"%s\", \"%d\";\n", 
               GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp,"ATTRIBUTE: \"%s_%d\", \"%s\", \"%lg\";\n", 
               GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            if (attrib->getStringValueIndex() < 0)
               break;
            char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
                  fprintf(fp,"ATTRIBUTE: \"%s_%d\", \"%s\", \"%s\";\n", 
                     GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
               else
                  fprintf(fp,"ATTRIBUTE: \"%s_%d\", \"%s\", \"\"\n", 
                     GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out);
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
      // here kill mask and layer
   }

   *attcnt = GENCAMcnt;
   return;
}

//--------------------------------------------------------------
static void write_DeviceAttribs(FILE *fp,CAttributes* map, 
                                const char *GENCAMname, int *attcnt )
{
   WORD keyword;
   Attrib* attrib;
   int   GENCAMcnt = *attcnt;

   if (map == NULL)  
      return;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))   
         continue;

      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))   
         continue;

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %d\n", 
               GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %lg\n", 
            GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            if (attrib->getStringValueIndex() < 0)  
               break;
            char *tok, *temp = STRDUP(attrib->getStringValue());
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               CString keyWordOut = doc->getKeyWordArray()[keyword]->out;
               if (attrib->getStringValueIndex() != -1)
               {
                  if (!keyWordOut.CompareNoCase("TOL") || !keyWordOut.CompareNoCase("NTOL") || 
                     !keyWordOut.CompareNoCase("PTOL") || !keyWordOut.CompareNoCase("DESC") ||
                     !keyWordOut.CompareNoCase("VOLTS"))
                     fprintf(fp,"%s %s\n", doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
                  else
                     fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
                        GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
               }
               else
                  fprintf(fp,"ATTRIBUTE %s_%d \"%s\" ?\n", 
                     GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out);
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   *attcnt = GENCAMcnt;

   return;
}

/******************************************************************************
* GENCAM_AddRotatedPadstackData
*/
static void GENCAM_AddRotatedPadstackData(CDataList *DataList, DbUnit insertAngle, CString& padstackName)
{
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = DataList->GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;

		InsertStruct* insert = data->getInsert();
		if (insert == NULL)
			continue;

      int rot = normalizeDegrees(round(RadToDeg(insert->getAngle() + insertAngle)));
		BlockStruct* block = doc->getBlockAt(insert->getBlockNumber());
      if(block)
      {
         padstackName = block->getName();
         if(rot)
         {
            CString padname;
			   padname.Format("%s_%d", padstackName, rot);
            PadRotarray.Set(padstackName, padname, rot);
         }
      }
   } 
}

/******************************************************************************
* GENCAM_AddRotatedandLayerPadstacks
*/
static void GENCAM_AddRotatedandLayerPadstacks(CGenCamPadstackMap &padstackMap)
{
   // support rotation
   for(POSITION filePOS = doc->getFileList().GetHeadPosition();filePOS;)
   {
      FileStruct *file = doc->getFileList().GetNext(filePOS);
      BlockStruct* fileBlock = (file)?file->getBlock():NULL;

      for(POSITION filebPos = (fileBlock)?fileBlock->getHeadDataInsertPosition():NULL;filebPos;)
      {
         DataStruct *data = fileBlock->getNextDataInsert(filebPos);
         if(data && data->getInsert())
         {
            BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
            if(block && (block->getBlockType() == blockTypeBondPad || block->getBlockType() == blockTypePadstack))
            {
               CString padstackName;
               GENCAM_AddRotatedPadstackData(&block->getDataList(), data->getInsert()->getAngle(), padstackName);

               CGenCamPadstack* padstack = NULL;
               if(padstackMap.Lookup(padstackName,padstack) && padstack)
               {
                  padstack->SetInsertLayer(data->getLayerIndex());
               }
            }
         }
      }//for
   }
}

//--------------------------------------------------------------
static void write_padstacks(FILE *fp, BlockStruct *padstack, double unitsFactor, 
                            int Insertlayer, int &padstackattcnt, double rotation)
{
   if(!rotation)
      fprintf(fp,"%sPADSTACK: \"%s\";\n",ident, padstack->getName());
   else
      fprintf(fp,"%sPADSTACK: \"%s_%.0f\";\n",ident, padstack->getName(),rotation);

   wgencam_plusident();
   GENCAM_WritePADSTACKData(fp, &(padstack->getDataList()), 0.0, 0.0, DegToRad(rotation), 0, unitsFactor, 0, Insertlayer);
   // attributes

   write_attributes(fp, padstack->getAttributesRef(), "PADST", &padstackattcnt);
   wgencam_minusident();
}

//--------------------------------------------------------------
static int do_padstacks(FILE *fp, int filenum)
{
   int      padstackattcnt = 0;
   double   unitsFactor = 1;
   int      first = TRUE;
   CGenCamPadstackMap padstackMap;

   if (filenum < 0)
   {
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);
         if (block == NULL)   
            continue;

         if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
         {
            if (first)
            {
               fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_PADSTACKS, filenum));
               wgencam_plusident();
            }
            first = FALSE;

            CString  drillname, groupname;
            drillname.Format("Drill_%lg", block->getToolSize());
            groupname = get_groupname(GROUP_PRIMITIVES, -1);

            fprintf(fp,"%sHOLEDEF: \"HOLE_%d\", ELECTRICAL, \"%s\".\"%s\", , , ;\n", 
                     ident, block->getBlockNumber(), 
                     groupname, drillname );
         }

         if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
         {
            CGenCamPadstack* padstack = NULL;
            if(!padstackMap.Lookup(block->getName(), padstack) || !padstack)
            {
               padstack = new CGenCamPadstack(block);
			      padstackMap.SetAt(block->getName(), padstack);
            }
         }
         GENCAM_AddRotatedandLayerPadstacks(padstackMap);
      }
   }


   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;

      if (block->getFlags() & BL_WIDTH)   
         continue;
      if (block->getFileNumber() != filenum)
         continue;
         
      // if (block->getFlags() & BL_TOOL) break;   // no drill

      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         if (first)
         {
            fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_PADSTACKS, filenum));
            wgencam_plusident();
         }
         first = FALSE;

         // double drill = get_drill_from_block_no_scale(doc, block);
      
         CGenCamPadstack* padstack = NULL;
         int Insertlayer = -1;
         if(padstackMap.Lookup(block->getName(), padstack) && padstack)
            Insertlayer = padstack->GetInsertLayer();

         write_padstacks(fp, block, unitsFactor, Insertlayer,padstackattcnt,0.0);

         // support bondpad rotation
         for(int idx = 0; idx < PadRotarray.GetCount(); idx++)
         {
            GenCamPadRotStruct *padrot = PadRotarray.Get(idx,block->getName());
            if(padrot)
            {
               write_padstacks(fp, block, unitsFactor, Insertlayer,padstackattcnt,padrot->rotation);
            }
         }
      }
/*
      else
      if ( (block->getBlockType() == BLOCKTYPE_PADSHAPE))
      {
         ErrorMessage("PADSTACK with Graphic not yet supported !","GENCAM Output", MB_OK | MB_ICONHAND);
      }
*/
   }

   if (!first)
      wgencam_minusident();

   return 1;
}

//--------------------------------------------------------------
static int do_pcbpackages(FILE *fp, int filenum, const CMapStringToString &packageCompHeightMap)
{
   double unitsFactor = 1; 
   int    first = TRUE;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (block->getFileNumber() != filenum)
         continue;
      
      // create necessary aperture blocks
      if ( (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT))
      {
         if (first)
         {
            fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_PACKAGES, filenum));
            wgencam_plusident();
            first = FALSE;
         }
	
			CString compHeightString = "";
			packageCompHeightMap.Lookup(block->getName(), compHeightString);

         fprintf(fp,"%sPACKAGE: \"%s\", , %s;\n", ident, block->getName(), compHeightString);
         wgencam_plusident();

         GENCAM_WritePACKAGEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, unitsFactor, 0, -1);
         wgencam_minusident();
      }
   }

   if (!first)
      wgencam_minusident();

   return 1;
}

//--------------------------------------------------------------
static int do_pcbshapes(FILE *fp, int filenum)
{
   double unitsFactor = 1;
   bool first = true;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;

      if (block->getFileNumber() != filenum)   
         continue;

      if (block->getFlags() & BL_FILE)
         continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
      if (block->getFlags() & BL_SMALLWIDTH)
         continue;
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         continue;
      if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL))
         continue;

      if ( block->getBlockType() == BLOCKTYPE_LIBRARY)     
         continue;
      if ( block->getBlockType() == BLOCKTYPE_TOOLGRAPHIC) 
         continue;
      if ( block->getBlockType() == BLOCKTYPE_SYMBOL)         
         continue;
      if ( block->getBlockType() == BLOCKTYPE_GATEPORT)    
         continue;
      if ( block->getBlockType() == BLOCKTYPE_SHEET)
         continue;
      if ( block->getBlockType() == BLOCKTYPE_PADSTACK)    
         continue;
      if ( block->getBlockType() == BLOCKTYPE_PADSHAPE)    
         continue;
      if ( block->getBlockType() == BLOCKTYPE_PCB)         
         continue;
      if ( block->getBlockType() == BLOCKTYPE_CENTROID)    
         continue;

      // create necessary aperture blocks
      //if ( (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT))
      if (first)
      {
         fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_PATTERN, filenum));
         wgencam_plusident();
         first = false;
      }

      fprintf(fp,"%sPATTERNDEF: \"%s\";\n", ident, block->getName());
      wgencam_plusident();
      GENCAM_WriteSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, unitsFactor, 0, -1);
      wgencam_minusident();
   }

   if (!first)
      wgencam_minusident();

   return 1;
}

//--------------------------------------------------------------
static int do_artworkshapes(FILE *fp, int filenum, int first)
{
   double      unitsFactor = 1;  
   //int  first = TRUE;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (block->getFileNumber() != filenum)
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;
      if (block->getFlags() & BL_SMALLWIDTH)
         continue;

      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         if (block->getShape() == T_TARGET)
         {
            if (first)
            {
               fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_ARTWORK, filenum));
               wgencam_plusident();
               first = FALSE;
            }
            double sizeA = block->getSizeA() * unitsFactor;
            double sizeB = block->getSizeB() * unitsFactor; 

            fprintf(fp,"%sTARGETDEF: \"%s\", MARKER, , ;\n", ident, block->getName());

            wgencam_plusident();
            wGENCAM_Graph_Line(ident, fp, -sizeA, 0.0, 1.0, +sizeA, 0.0, 0 );
            wGENCAM_Graph_Line(ident, fp, +sizeA, 0.0, 1.0, -sizeA, 0.0, 0 );
            wgencam_minusident();

         }
         else if (block->getShape() == T_COMPLEX)
         {
            // here needs to be a complex aperture.
            if (first)
            {
               fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_ARTWORK, filenum));
               wgencam_plusident();
               first = FALSE;
            }
            fprintf(fp,"%sARTWORKDEF: \"%s\";\n", ident, block->getName());
            block = doc->Find_Block_by_Num((int)(block->getSizeA()));
            wgencam_plusident();
            GENCAM_ArtworkSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, unitsFactor, 0, -1);
            wgencam_minusident();
         }        
         continue;
      }
/**** only apertures are done in Artwork, because artwork can not have any layers.
      all other blocks are done in Pattern

      if (block->getFlags() & BL_FILE)
         continue;
      if (block->getFlags() & BL_TOOL)
         continue;

      switch (block->getBlockType() )
      {
         case BLOCKTYPE_PANEL:
         case BLOCKTYPE_LIBRARY:
         case BLOCKTYPE_TOOLGRAPHIC:
         case BLOCKTYPE_SYMBOL:
         case BLOCKTYPE_GATEPORT:
         case BLOCKTYPE_SHEET:
         case BLOCKTYPE_PADSTACK:
         case BLOCKTYPE_PADSHAPE:
         case BLOCKTYPE_PCB:
         case BLOCKTYPE_CENTROID:
            // do nothing
         break;
         case BLOCKTYPE_PCBCOMPONENT:
         default:
         {
            if (first)
            {
               fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_ARTWORK, filenum));
               wgencam_plusident();
               first = FALSE;
            }

            fprintf(fp,"%sARTWORKDEF: \"%s\";\n", ident, block->name);
            wgencam_plusident();
            GENCAM_ArtworkSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, unitsFactor, 0, -1);
            wgencam_minusident();
         }
         break;
      }
*****/
   }

   if (!first)
      wgencam_minusident();

   return 1;
}

//--------------------------------------------------------------
static const char *get_gencamtype(const char *t)
{

   for (int i=0;i<typemapcnt;i++)
   {
      if (typemaparray[i] && typemaparray[i]->camcadtype.CompareNoCase(t) == 0)
      {
         return typemaparray[i]->gencamtype;
      }
   }

   fprintf(flog,"Type [%s] not found in .TYPEMAP\n", t);
   display_error++;

   update_typemap(t,"");

   return "";
}


/*****************************************************************************/
/*
*/
DataStruct *find_component_from_device(CCEtoODBDoc *doc, CDataList *DataList, const char *devicename)
{
   DataStruct *np;

	POSITION pos = DataList?DataList->GetHeadPosition():NULL;
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (!np || np->getDataType() != T_INSERT)
         continue;
                
      if (np->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      Attrib *a;
      if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
      {
         CString  dev;
         dev = get_attvalue_string(doc, a);
         if (!STRCMPI(dev, devicename))
            return np;
      }
   
   }

   return NULL;
}

//--------------------------------------------------------------
static int do_devices(FILE *fp, FileStruct *file, CTypeList *TypeList)
{
   TypeStruct *typ;
   POSITION typPos;
   int      deviceattcnt = 0;
   Attrib   *a;
   
	typPos = TypeList?TypeList->GetHeadPosition():NULL;
   while (typPos != NULL)
   {                                         
      typ = TypeList->GetNext(typPos);
		if (!typ)
			continue;
      BlockStruct *block = doc->Find_Block_by_Num(typ->getBlockNumber());

      if (block == NULL) 
         continue;

      fprintf(fp, "%sDEVICE: \"%s\", OTHER, \"%s\".\"%s\";\n", 
         ident, typ->getName(), get_groupname(GROUP_PACKAGES, block->getFileNumber()), block->getName());

      wgencam_plusident();

      DataStruct *cur_comp = NULL;
      if (typ->getBlockNumber() > -1) // this is not attached to a component.
         cur_comp = find_component_from_device(doc, &(file->getBlock()->getDataList()), typ->getName());

      CString  partnumber;
      partnumber = "";

      if (a =  is_attvalue(doc, typ->getAttributesRef(), "PARTNUMBER",  2))
      {
         partnumber = get_attvalue_string(doc, a);
      }
      else
      if (block)
      {
         if (a =  is_attvalue(doc, block->getAttributesRef(), "PARTNUMBER",   2))
         {
            partnumber = get_attvalue_string(doc, a);
         }
      }

      if (strlen(partnumber) == 0)
      {
         // find if a value of the device is on a component
         if (cur_comp)
         {
            if (a =  is_attvalue(doc, cur_comp->getAttributesRef(), "PARTNUMBER",   2))
            {
               partnumber = get_attvalue_string(doc, a);
            }
         }
      }

      if (strlen(partnumber))
         fprintf(fp, "%sPART: \"Mentor Grapphics\", \"%s\";\n", ident, partnumber);

/*
      // type DIODE 
      if (a =  is_attvalue(doc, typ->getAttributesRef(), ATT_GENCAMTYPE, 2))
      {
         CString val; 
         val = get_attvalue_string(doc, a);
         fprintf(fp, "%sTYPE: %s;\n", ident, val);
      }
      else
      if (a =  is_attvalue(doc, typ->getAttributesRef(), ATT_DEVICETYPE, 2))
      {
         CString val; 
         val = get_attvalue_string(doc, a);
         fprintf(fp, "%sTYPE: %s;\n", ident, get_gencamtype(val));
      }
      else
      if (block)
      {
         if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_DEVICETYPE, 2))
         {
            CString val; 
            val = get_attvalue_string(doc, a);
            fprintf(fp, "%sTYPE: %s;\n", ident, get_gencamtype(val));
         }
      }
*/

      CString val ;
      val = "";

      // pindesc "1", ANODE
      // VALUE
      if (a =  is_attvalue(doc, typ->getAttributesRef(), "VALUE", 2))
      {
         val = get_attvalue_string(doc, a);
      }
      else
      if (block)
      {
         if (a =  is_attvalue(doc, block->getAttributesRef(), "VALUE",  2))
         {
            val = get_attvalue_string(doc, a);
         }
      }

      if (strlen(val) == 0)
      {
         // find if a value of the device is on a component
         if (cur_comp)
         {
            if (a =  is_attvalue(doc, cur_comp->getAttributesRef(), "VALUE",  2))
            {
               val = get_attvalue_string(doc, a);
            }
         }
      }


      CString  ptol, ntol;
      ptol = "";
      ntol = "";

      // TOL
      if (a =  is_attvalue(doc, typ->getAttributesRef(), "TOLERANCE",   2))
      {
         CString val; 
         val = get_attvalue_string(doc, a);
         fprintf(fp, "%sTOL: \"%s\";\n", ident, val);
      }
      else
      if (block)
      {
         if (a =  is_attvalue(doc, block->getAttributesRef(), "TOLERANCE", 2))
         {
            ptol = get_attvalue_string(doc, a);
            ntol = ptol;
         }
      }

      if (cur_comp)
      {
         if (a =  is_attvalue(doc, cur_comp->getAttributesRef(), "TOLERANCE", 2))
         {
            ptol = get_attvalue_string(doc, a);
            ntol = ptol;
         }
      }

      if (a =  is_attvalue(doc, typ->getAttributesRef(), "+TOL",  2))
      {
         ptol = get_attvalue_string(doc, a);
      }
      else
      if (block)
      {
         if (a =  is_attvalue(doc, typ->getAttributesRef(), "+TOL",  2))
         {
            ptol = get_attvalue_string(doc, a);
         }
      }

      if (cur_comp)
      {
         if (a =  is_attvalue(doc, cur_comp->getAttributesRef(), "+TOL",   2))
         {
            ptol = get_attvalue_string(doc, a);
         }

      }

      if (a =  is_attvalue(doc, typ->getAttributesRef(), "-TOL",  2))
      {
         ntol = get_attvalue_string(doc, a);
      }
      else
      if (block)
      {
         if (a =  is_attvalue(doc, typ->getAttributesRef(), "-TOL",  2))
         {
            ntol = get_attvalue_string(doc, a);
         }
      }

      if (cur_comp)
      {
         if (a =  is_attvalue(doc, cur_comp->getAttributesRef(), "-TOL",   2))
         {
            ntol = get_attvalue_string(doc, a);
         }

      }

      // a value and a tolerance can only be written is a value exist !
      double valnumber;
      CString valunit;
      if (strlen(val))
      {
			ComponentValues compVal(val);
			if (compVal.GetValue(valnumber, valunit))
         {
            // need high precision for pico
            fprintf(fp, "%sVALUE: %.14lf, %s, %s, %s;\n", ident, valnumber, valunit, ntol, ptol);
         }
         else
         {
            fprintf(flog,"Can not normalize Value [%s]\n", val);
            display_error++;
         }
      }

      //write_DeviceAttribs(fp, typ->getAttributesRef(), "DEV", &deviceattcnt);
      wgencam_minusident();
   }

   return 1;
}

//--------------------------------------------------------------
static int do_routes(FILE *fp, FileStruct *file,CNetList *NetList, double scale)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   GENCAM_GetROUTESData(fp,&(file->getBlock()->getDataList()), 
         file->getInsertX() * scale, file->getInsertY() * scale,
         file->getRotation(), file->isMirrored(),scale, 0, -1);


   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (!net || (net->getFlags() & NETFLAG_UNUSEDNET))
         continue;

      fprintf(fp,"%sROUTE: \"%s\", SIGNAL;\n",ident, net->getNetName());
      wgencam_plusident();

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
			if (!compPin)
				continue;

         int blockNumber = compPin->getPadstackBlockNumber();
         BlockStruct *block = doc->getBlockAt(blockNumber);
         if(compPin->IsDiePin(doc->getCamCadData()))
         {
            //skip Die Pins
            fprintf(flog, "ROUTES: Net %s CompPin %s - Skipped CompPin for Die component.\n", 
               net->getNetName(), compPin->getPinRef());
         }
         else
         {
            fprintf(fp, "%sCOMPPIN: \"%s\".\"%s\", \"%s\", ( %lg, %lg) ;\n", 
               ident, get_groupname(GROUP_COMPONENTS, file->getFileNumber()), compPin->getRefDes(), compPin->getPinName(), 
               compPin->getOriginX()*scale, compPin->getOriginY()*scale);
         }
         //write_attributes(fp, compPin->getAttributesRef(), "COMPPIN", &comppinattcnt);
      }

      // order by signal name
      GENCAM_WriteROUTESData(fp,&(file->getBlock()->getDataList()), 
         file->getInsertX() * scale, file->getInsertY() * scale,
         file->getRotation(), file->isMirrored(), scale, 0, -1, net->getNetName());
      wgencam_minusident();
   }

   // unused net
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (!(net->getFlags() & NETFLAG_UNUSEDNET))
         continue;

      fprintf(fp,"%sROUTE: \"GENCAMEXTNC\", SIGNAL;\n",ident);
      wgencam_plusident();

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         fprintf(fp, "%sCOMPPIN: \"%s\" \"%s\", %lg, %lg;\n", 
            ident, compPin->getRefDes(), compPin->getPinName(), 
            compPin->getOriginX()*scale, compPin->getOriginY()*scale);
         //write_attributes(fp, compPin->getAttributesRef(), "COMPPIN", &comppinattcnt);
      }
/*
      // order by signal name
      GENCAM_WriteROUTESData(fp,&(file->getBlock()->getDataList()), 
         file->getInsertX() * scale, file->getInsertY() * scale,
         file->getRotation(), file->isMirrored(), scale, 0, -1, net->getNetName());
*/
      wgencam_minusident();
   }
   return 1;
}

//--------------------------------------------------------------
static int gencam_routes(FILE *wfp)
{
   FileStruct  *file;
   double unitsFactor = 1; 

   fprintf(wfp,"%s$ROUTES\n", ident);
   wgencam_plusident();

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  
         continue;
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

         fprintf(wfp,"%sGROUP:\"%s\";\n",ident, get_groupname(GROUP_ROUTES,file->getFileNumber()) );
         wgencam_plusident();
         do_routes(wfp, file, &file->getNetList(), file->getScale() * unitsFactor);
         wgencam_minusident();

      }
   }
   wgencam_minusident();
   fprintf(wfp,"%s$ENDROUTES\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
static int gencam_components(FILE *wfp)
{
   FileStruct  *file;
   double unitsFactor = 1; 

   fprintf(wfp,"%s$COMPONENTS\n", ident);
   wgencam_plusident();

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file || !file->isShown())
         continue;
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         fprintf(wfp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_COMPONENTS, file->getFileNumber()));
         wgencam_plusident();

         GENCAM_WriteCOMPONENTData(wfp,&(file->getBlock()->getDataList()), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, file->getFileNumber());

         wgencam_minusident();

      }
   }
   wgencam_minusident();
   fprintf(wfp,"%s$ENDCOMPONENTS\n\n",ident);

   return 1;
}

/******************************************************************************
* gencam_boards
*/
static int gencam_boards(FILE *wfp)
{
   double unitsFactor = 1; 
   double accuracy = get_accuracy(doc);

   fprintf(wfp, "%s$BOARDS\n", ident);
   wgencam_plusident();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = cur_file = doc->getFileList().GetNext(filePos);

      if (!file || !file->isShown())  
         continue;

      if (file->getBlockType() != BLOCKTYPE_PCB)
         continue;

      fprintf(wfp, "%sBOARD:\"%s\";\n",ident, file->getName());
      wgencam_plusident();
      //fprintf(wfp, "%sUSING:\"%s\";\n",ident, get_groupname(0, -1));
      //fprintf(wfp, "%sUSING:\"%s\";\n",ident, get_groupname(0, file->getFileNumber()));
      fprintf(wfp, "%sUSING:\"%s\";\n",ident, get_groupname(GROUP_ROUTES,file->getFileNumber()) );
      fprintf(wfp, "%sUSING:\"%s\";\n",ident, get_groupname(GROUP_COMPONENTS,file->getFileNumber()) );

      // here we need to look for a closed outline
      Outline_Start(doc);

      int found = GENCAM_OutlineBOARDData(wfp, &file->getBlock()->getDataList(),
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
         TRUE, GR_CLASS_BOARDOUTLINE);

      if (!found)
      {
         // now look for every visible outline
         int found = GENCAM_OutlineBOARDData(wfp, &file->getBlock()->getDataList(),
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
               FALSE, GR_CLASS_BOARDOUTLINE);

         if (!found)
         {
            fprintf(flog, "Did not find a Board Outline for [%s].\n", file->getName());
            display_error++;

            if (file->getBlock() && file->getBlock()->extentIsValid())
            {
               Point2 p1, p2;

               p1.x = file->getBlock()->getXmin();
               p1.y = file->getBlock()->getYmin();
               p1.bulge = 0;

               p2.x = file->getBlock()->getXmax();
               p2.y = file->getBlock()->getYmax();
               p2.bulge = 0;

               fprintf(wfp, "%sOUTLINE: \"%s\".\"%s\";\n",ident, get_groupname(GROUP_LAYERS, -1), "BOARD_OUTLINE");
               fprintf(flog, "%sOUTLINE: \"%s\".\"%s\";\n",ident, get_groupname(GROUP_LAYERS, -1), "BOARD_OUTLINE");
               wgencam_plusident();

               CString gname = get_groupname(GROUP_PRIMITIVES, -1);
               fprintf(wfp, "%sPOLYGON: \"%s\".\"LINEDESC%d\", \"%s\".\"HOLLOW\";\n",ident, gname, 0, gname);
               fprintf(flog, "%sPOLYGON: \"%s\".\"LINEDESC%d\", \"%s\".\"HOLLOW\";\n",ident, gname, 0, gname);
               wgencam_plusident();

               wGENCAM_Graph_StartAt(ident, wfp, p1.x, p1.y);
               wGENCAM_Graph_NextAt(ident, wfp, p1.x, p1.y, 0.0, p2.x, p1.y, 0.0 );
               wGENCAM_Graph_NextAt(ident, wfp, p2.x, p1.y, 0.0, p2.x, p2.y, 0.0 );
               wGENCAM_Graph_NextAt(ident, wfp, p2.x, p2.y, 0.0, p1.x, p2.y, 0.0 );
               wGENCAM_Graph_EndAt(ident, wfp, p1.x, p2.y, 0.0, p1.x, p1.y, 0.0 );

               wgencam_minusident();   // polygondef
               wgencam_minusident();   // outline
            }
            else
            {
               fprintf(flog,"Did not find a Board Extents for [%s].\n", file->getName());
               display_error++;
            }
         }
      }
      
      if (found)
      {
         int returnCode;
         if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), get_groupname(GROUP_LAYERS, -1), "BOARD_OUTLINE"))
         {
            fprintf(flog, "Error in easy outline for [%s]\n", file->getName());
            display_error++;
         }
      }

      Outline_FreeResults();

      // this other information, such as keepouts etc...
      GENCAM_WritePRIMARYBOARDData(wfp, &file->getBlock()->getDataList(), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

      // here are the holes
      GENCAM_WritePRIMARYHOLEData(wfp, &file->getBlock()->getDataList(),
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

      // here do BAREBOARDTEST
      CString name = file->getName();
      if (strlen(name) == 0)  name = "NoName";
      fprintf(wfp, "%sBAREBOARDTEST: \"%s\", @NULL@, @NULL@, @NULL@;\n",ident, name);

      wgencam_minusident();
      
      fprintf(wfp, "%sASSEMBLY:\"%s_Assy\",\"%s\";\n",ident, file->getName(), file->getName());
      wgencam_plusident();
      fprintf(wfp, "%sUSING:\"%s\";\n",ident, get_groupname(GROUP_COMPONENTS,file->getFileNumber()) );
      wgencam_minusident();
   }


   wgencam_minusident();
   fprintf(wfp,"%s$ENDBOARDS\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
static int gencam_drawings(FILE *wfp)
{
   FileStruct  *file;
   double unitsFactor = 1; 
   int      first = TRUE;


   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file || !file->isShown())
         continue;
      if (file->getBlockType() == BLOCKTYPE_PCB)  
         continue;   
      if (file->getBlockType() == BLOCKTYPE_PANEL)
         continue;   
      
      if (first)
      {
         fprintf(wfp,"%s$DRAWINGS\n", ident);
         wgencam_plusident();
      }
      first = FALSE;

      fprintf(wfp,"%sDRAWING:\"%s\";\n",ident, file->getName());
      wgencam_plusident();

      fprintf(wfp,"DWGTYPE\n");

      wgencam_minusident();
   }

   if (!first)
   {
      wgencam_minusident();
      fprintf(wfp,"%s$ENDDRAWINGS\n\n",ident);
   }

   return 1;
}

//--------------------------------------------------------------
static int gencam_panels(FILE *wfp)
{
   FileStruct  *file;
   double unitsFactor = 1; 
   int      first = TRUE;
   double   accuracy = get_accuracy(doc);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file || !file->isShown())  
         continue;
      if (file->getBlockType() == BLOCKTYPE_PANEL)   
      {
         if (first)
         {
            fprintf(wfp,"%s$PANELS\n", ident);
            wgencam_plusident();
         }
         first = FALSE;

         fprintf(wfp,"%sPANEL:\"%s\";\n",ident, file->getName());
         wgencam_plusident();

         // here we need to look for a closed outline
         Outline_Start(doc);

         int found = GENCAM_OutlinePANELData(wfp, &(file->getBlock()->getDataList()),
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
            TRUE, GR_CLASS_PANELOUTLINE);
         if (!found)
         {
            // now look for every visible outline
            int found = GENCAM_OutlinePANELData(wfp, &(file->getBlock()->getDataList()),
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
               FALSE, GR_CLASS_PANELOUTLINE);
            if (!found)
            {
               fprintf(flog,"Did not find a Panel Outline for [%s].\n", file->getName());
               display_error++;

               Point2   p1, p2;
               if (file->getBlock() && file->getBlock()->extentIsValid())
               {
                  p1.x = file->getBlock()->getXmin();
                  p1.y = file->getBlock()->getYmin();
                  p1.bulge = 0;
                  p2.x = file->getBlock()->getXmax();
                  p2.y = file->getBlock()->getYmax();
                  p2.bulge = 0;
                  fprintf(wfp,"%sOUTLINE: \"%s\".\"%s\";\n",ident, get_groupname(GROUP_LAYERS, -1), "PANEL_OUTLINE");
                  wgencam_plusident();

                  CString  gname = get_groupname(GROUP_PRIMITIVES, -1);
                  fprintf(wfp,"%sPOLYGON: \"%s\".\"LINEDESC%d\", \"%s\".\"HOLLOW\";\n",ident, gname, 0, gname);

                  wgencam_plusident();

                  wGENCAM_Graph_StartAt(ident, wfp, p1.x, p1.y);
                  wGENCAM_Graph_NextAt(ident, wfp, p1.x, p1.y, 0.0, p2.x, p1.y, 0.0 );
                  wGENCAM_Graph_NextAt(ident, wfp, p2.x, p1.y, 0.0, p2.x, p2.y, 0.0 );
                  wGENCAM_Graph_NextAt(ident, wfp, p2.x, p2.y, 0.0, p1.x, p2.y, 0.0 );
                  wGENCAM_Graph_EndAt(ident, wfp, p1.x, p2.y, 0.0, p1.x, p1.y, 0.0 );

                  wgencam_minusident();   // polygondef
                  wgencam_minusident();   // outline
               }
               else
               {
                  fprintf(flog,"Did not find a Panel Extents for [%s].\n", file->getName());
                  display_error++;
               }
            }
         }
         
         if (found)
         {
            int returnCode;
            if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), get_groupname(GROUP_LAYERS, -1), "PANEL_OUTLINE"))
            {
               fprintf(flog, "Error in easy outline for [%s]\n", file->getName());
               display_error++;
            }
         }

         Outline_FreeResults();

         wgencam_minusident();

         GENCAM_WritePRIMARYPANELData(wfp,&(file->getBlock()->getDataList()), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

      }
   }

   if (!first)
   {
      wgencam_minusident();
      fprintf(wfp,"%s$ENDPANELS\n\n",ident);
   }

   return 1;
}

/*---------------------------------------------------------------------------*/
/*
*/
static int swap_unique(const char *swaptest, const char *l1, const char *l2)
{
   if (strlen(swaptest) == 0) 
      return TRUE;

   CString  t;
   t.Format("%s,%s;", l1, l2);   // first second

   if (strstr(swaptest, t))
      return FALSE;

   t.Format("%s,%s;", l2, l1);   // second first

   if (strstr(swaptest, t))
      return FALSE;

   return TRUE;
}

/*---------------------------------------------------------------------------*/
/*
*/
static int do_layerswap(FILE *wfp, int comptop, int legendtop)
{
   int      swapcnt = 0;
   CString  swaptest;
   CString l1, l2, group;
   group = get_groupname(GROUP_LAYERS, -1);
   swaptest = "";

   if (!comptop)
   {
      l1 = "COMPTOP";
      l2 = "COMPBOT";
      if (strlen(l1) && strlen(l2) && swap_unique(swaptest, l1, l2))
      {
         fprintf(wfp,"%sLAYERSWAP: \"SWAP%d\", \"%s\".\"%s\", \"%s\".\"%s\";\n", ident, ++swapcnt,
            group, l1, group, l2 );
            swaptest += l1;
            swaptest += ',';
            swaptest += l2;
            swaptest += ';';
      }
   }

   if (!legendtop)
   {
      l1 = "OUTLINE_TOP";
      l2 = "OUTLINE_BOT";
      if (strlen(l1) && strlen(l2) && swap_unique(swaptest, l1, l2))
      {
         fprintf(wfp,"%sLAYERSWAP: \"SWAP%d\", \"%s\".\"%s\", \"%s\".\"%s\";\n", ident, ++swapcnt,
            group, l1, group, l2 );
            swaptest += l1;
            swaptest += ',';
            swaptest += l2;
            swaptest += ';';
      }
   }

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)   // could have been deleted.
         continue; 

      if (layer->getMirroredLayerIndex() != layer->getLayerIndex())
      {
         l1 = Layer_Gencam(layer->getLayerIndex());
         l2 = Layer_Gencam(layer->getMirroredLayerIndex());
         if (strlen(l1) && strlen(l2) && swap_unique(swaptest, l1, l2))
         {
            fprintf(wfp,"%sLAYERSWAP: \"SWAP%d\", \"%s\".\"%s\", \"%s\".\"%s\";\n", ident, ++swapcnt,
               group, l1, group, l2 );
            swaptest += l1;
            swaptest += ',';
            swaptest += l2;
            swaptest += ';';
         }
      }
   }

   return 1;
}

/*---------------------------------------------------------------------------*/
/*
      GenCAMLayerStruct *gl = new GenCAMLayerStruct;
      gl->stackNum = signr;
      gl->layerindex = j;
      gl->on = ON;
      gl->type = typ;
      gl->oldName = layer->getName();
      gl->newName = genlay;
      glArr.SetAtGrow(maxArr, gl);
      maxArr++;

*/
static int gencam_layers(FILE *wfp)
{
   int         cnt = 0;

   fprintf(wfp,"%s$LAYERS\n", ident);
   wgencam_plusident();

   // layers are always global in CAMCAD
   fprintf(wfp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_LAYERS, -1));

   wgencam_plusident();

   // first all electrical signal layers !
	int i=0;
   for (i=1;i<255;i++)
   {
      for (int j=0;j<maxArr;j++)
      {
         if (!glArr[j] || glArr[j]->type != 'S')    
            continue;
         if (glArr[j]->stackNum == i)
         {
            CString  surface, gencam_layer_type, material, material_code;
            double   thickness = 0;

            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "CONDUCTOR";
            material = "";
            material_code = "";

            LayerStruct *layer = doc->getLayerArray()[glArr[j]->layerindex];
            if (layer == NULL) // could have been deleted.
               continue; 

            if (doc->IsFloatingLayer(layer->getLayerIndex()))  // no floating layers, they should never be in a gencam file.
                  continue;

            if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
            {
               surface = "TOP";
               gencam_layer_type = "CONDUCTOR";
            }
            else
            if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
            {
               surface = "BOTTOM";
               gencam_layer_type = "CONDUCTOR";
            }
            else
            if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
            {
               surface = "INTERNAL";
               gencam_layer_type = "CONDUCTOR";
            }
            // here write electrical layer !
            fprintf(wfp,"%sLAYERSINGLE: \"%s\", %s, %s, %s, %s, %lf;\n", ident, 
               glArr[j]->newName, surface, gencam_layer_type, material, material_code, thickness );

            break;
         }
      }

   }

   LayerStruct *layer;
   CString     genlay;
   int         comptop = 0, compbot = 0, legendtop = 0, boardoutline = 0;

   // now the rest.
   for (int j=0;j<maxArr;j++)
   {
      if (!glArr[j] || glArr[j]->type == 'S') 
         continue;

      // do not write if the name is the same was done before
      int found = FALSE;

      // this layer could have been written in the SIGNAL section.
      for (i=0;i<maxArr;i++)
      {
         if (!glArr[j] || glArr[i]->type == 'S')    
         {
            if (!STRCMPI(glArr[j]->newName, glArr[i]->newName))   
               found = TRUE;
         }
      }
      for (i=0;i<j;i++)
      {
         if (!STRCMPI(glArr[j]->newName, glArr[i]->newName))   
            found = TRUE;
      }
      if (found)  
         continue;

      layer = doc->getLayerArray()[glArr[j]->layerindex];
      if (layer == NULL)   // could have been deleted.
         continue; 
      if (doc->IsFloatingLayer(layer->getLayerIndex()))  // no floating layers, they should never be in a gencam file.
         continue;

      if (glArr[j]->newName.Compare("BOARD_OUTLINE") == 0)  boardoutline = TRUE;
      if (glArr[j]->newName.Compare("COMPBOT") == 0)  compbot = TRUE;
      if (glArr[j]->newName.Compare("COMPTOP") == 0)  comptop = TRUE;

      if (strlen(glArr[j]->newName) == 0)
      {
         fprintf(flog, "Layer [%s] set invisible\n", layer->getName());
         display_error++;
         continue;
      }

      CString  surface, gencam_layer_type, material, material_code;
      double   thickness = 0;

      surface = "ALL";     // top, bottom, internal
      gencam_layer_type = "CONDUCTOR";
      material = "";
      material_code = "";

      switch (layer->getLayerType())
      {
         case LAYTYPE_UNKNOWN:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "GRAPHIC";
         break;
         case LAYTYPE_SIGNAL_ALL:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "CONDUCTOR";
         break;
         case LAYTYPE_DIALECTRIC:
            surface = "INTERNAL";      // top, bottom, internal
            gencam_layer_type = "DIELCORE";
         break;
         case LAYTYPE_PAD_TOP:
            surface = "TOP";     // top, bottom, internal
            gencam_layer_type = "PIN";
         break;
         case LAYTYPE_PAD_BOTTOM:
            surface = "BOTTOM";     // top, bottom, internal
            gencam_layer_type = "PIN";
         break;
         case LAYTYPE_PAD_INNER:
            surface = "INTERNAL";      // top, bottom, internal
            gencam_layer_type = "PIN";
         break;
         case LAYTYPE_PAD_ALL:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "PIN";
         break;
         case LAYTYPE_PAD_THERMAL:
            surface = "INTERNAL";      // top, bottom, internal
            gencam_layer_type = "PIN";
         break;
         case LAYTYPE_PASTE_TOP:
            surface = "TOP";     // top, bottom, internal
            gencam_layer_type = "PASTEMASK";
         break;
         case LAYTYPE_PASTE_BOTTOM:
            surface = "BOTTOM";     // top, bottom, internal
            gencam_layer_type = "PASTEMASK";
         break;
         case LAYTYPE_MASK_TOP:
            surface = "TOP";     // top, bottom, internal
            gencam_layer_type = "PASTEMASK";
         break;
         case LAYTYPE_MASK_BOTTOM:
            surface = "BOTTOM";     // top, bottom, internal
            gencam_layer_type = "SOLDERMASK";
         break;
         case LAYTYPE_SILK_TOP:
            surface = "TOP";     // top, bottom, internal
            gencam_layer_type = "LEGEND";
            toplegend = glArr[j]->newName;
            legendtop = TRUE;
         break;
         case LAYTYPE_SILK_BOTTOM:
            surface = "BOTTOM";     // top, bottom, internal
            gencam_layer_type = "LEGEND";
         break;
         case LAYTYPE_MASK_ALL:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "SOLDERMASK";
         break;
         case LAYTYPE_PASTE_ALL:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "PASTEMASK";
         break;
         case LAYTYPE_REDLINE:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "GRAPHIC";
         break;
         case LAYTYPE_DRILL:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "HOLEFILL";
         break;
         case LAYTYPE_TOP:
            surface = "TOP";     // top, bottom, internal
            gencam_layer_type = "GRAPHIC";
         break;
         case LAYTYPE_BOTTOM:
            surface = "BOTTOM";     // top, bottom, internal
            gencam_layer_type = "GRAPHIC";
         break;
         case LAYTYPE_ALL:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "GRAPHIC";
         break;
         case LAYTYPE_BOARD_OUTLINE:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "BOARDOUTLINE";
         break;
         case LAYTYPE_PAD_OUTER:
            surface = "BOTH";    // top, bottom, internal
            gencam_layer_type = "PIN";
         break;
         case LAYTYPE_SIGNAL_OUTER: 
            surface = "BOTH";    // top, bottom 
            gencam_layer_type = "CONDUCTOR";
         break;
         default:
            surface = "ALL";     // top, bottom, internal
            gencam_layer_type = "GRAPHIC";
         break;
      } // switch

      fprintf(wfp,"%sLAYERSINGLE: \"%s\", %s, %s ;\n", ident, glArr[j]->newName, surface, gencam_layer_type);
   }

   if (!boardoutline)
      fprintf(wfp,"%sLAYERSINGLE: \"%s\", ALL, BOARDOUTLINE ;\n", ident, "BOARD_OUTLINE");

   if (!comptop)
      fprintf(wfp,"%sLAYERSINGLE: \"%s\", TOP, COMPONENT ;\n", ident, "COMPTOP");
   if (!compbot)
      fprintf(wfp,"%sLAYERSINGLE: \"%s\", BOTTOM, COMPONENT ;\n", ident, "COMPBOT");

   if (!legendtop)
   {
      fprintf(wfp,"%sLAYERSINGLE: \"%s\", TOP, LEGEND ;\n", ident, "OUTLINE_TOP");
      toplegend = "OUTLINE_TOP";
   }

   // layerset
   // layerswap
   do_layerswap(wfp, comptop, legendtop);

   wgencam_minusident();

   wgencam_minusident();
   fprintf(wfp,"%s$ENDLAYERS\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
// do width and apertures
static int gencam_primitives(FILE *wfp)
{
   int         cnt = 0;
   double      unitsFactor = 1;  

   fprintf(wfp,"%s$PRIMITIVES\n", ident);
   wgencam_plusident();

   fprintf(wfp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_PRIMITIVES, -1));
   wgencam_plusident();

// some hardcoded stuff
   fprintf(wfp,"%sPAINTDESC: \"VOID\", VOID;\n", ident);
   fprintf(wfp,"%sPAINTDESC: \"FILL\", FILL;\n", ident);
   fprintf(wfp,"%sPAINTDESC: \"HOLLOW\", HOLLOW;\n", ident);

   fprintf(wfp,"%sBARRELDESC: \"Nonplated\";\n", ident);
   fprintf(wfp,"%sBARRELDESC: \"Plated\";\n", ident); 
   wgencam_plusident();
   fprintf(wfp,"%sBARREL: PLATE, \"Material\", 1, 1;\n", ident);
   wgencam_minusident();

   fprintf(wfp,"%sBARRELDESC: \"Unknown\";\n", ident); 


// here all linewidth
	int i=0;
   for (i=0;i<doc->getNextWidthIndex();i++)
   {
		BlockStruct *block = doc->getWidthTable()[i];
		if (block == NULL)
			continue;

      if (block->getFlags() & BL_WIDTH)
      {
         fprintf(wfp,"%sLINEDESC: \"LINEDESC%d\", %lg;\n", ident,i ,doc->getWidthTable()[i]->getSizeA() * unitsFactor); 
      }
   }

// here all apertures
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   
         continue;

      //if (block->getFlags() & BL_WIDTH) continue; <== write it if it has a name.
      if (block->getFlags() & BL_SMALLWIDTH) 
         continue;

      if (block->getName().GetLength() == 0)// if it has no name, it was written under aperture.
         continue;   
      // create necessary aperture blocks
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         if (block->getShape() == T_COMPLEX)
         {
            /* is done in ARTWORK */
         }
         else
         {
            double sizeA = block->getSizeA() * unitsFactor;
            double sizeB = block->getSizeB() * unitsFactor; 
            switch (block->getShape())
            {
               case T_SQUARE:
               {
                  fprintf(wfp,"%sRECTCENTERDEF: \"%s\", %lg, %lg;\n", 
                     ident, block->getName(), sizeA, sizeA);
               }
               break;
               case T_RECTANGLE:
               {
                  fprintf(wfp,"%sRECTCENTERDEF: \"%s\", %lg, %lg;\n", 
                     ident, block->getName(), sizeA, sizeB);
               }
               break;
               case T_ROUND:
                  fprintf(wfp,"%sCIRCLEDEF: \"%s\", %lg ;\n", ident, block->getName(), sizeA);
               break;
               case T_OCTAGON:
                  fprintf(wfp,"%sOCTAGONDEF: \"%s\", %lg;\n", ident, block->getName(), sizeA);
               break;
               case T_OBLONG:
                  fprintf(wfp,"%sOVALDEF: \"%s\", %lg, %lg;\n", ident, block->getName(), sizeA, sizeB);
               break;
               case T_THERMAL:
                  fprintf(wfp,"%sTHERMALDEF: \"%s\", ROUND, %lg, %lg ;\n", ident, 
                     block->getName(), sizeA, sizeB);
/*                
                  sprintf(prosa,"%%AMTHD%d*\n",block->getDcode());
                  wgerb_write2file(prosa);
                  sprintf(prosa,"1,1,%1.5lf,0,0*\n",sizeA);
                  wgerb_write2file(prosa);
                  sprintf(prosa,"1,0,%1.5lf,0,0*\n",sizeB);
                  wgerb_write2file(prosa);
                  sprintf(prosa,"21,0,%1.5lf,0.02,0.0,0.0,45.0*\n",sizeA+0.02);
                  wgerb_write2file(prosa);
                  sprintf(prosa,"21,0,%1.5lf,0.02,0.0,0.0,135.0*\n",sizeA+0.02);
                  wgerb_write2file(prosa);
                  wgerb_write2file("\n%%\n");
*/
               break;
               case T_DONUT:
                  fprintf(wfp,"%sDONUTDEF: \"%s\", ROUND, %lg, %lg ;\n", ident, 
                     block->getName(), sizeA, sizeB);
               break;
               case T_TARGET:

               break;
               default:
                  fprintf(wfp,"%sUNKNOWN: \"%s\", ;\n", ident, block->getName());

               break;
            } // switch
         } // complex
      } // aperture
   } // blocks

// here all tools
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   
         continue;

      if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
      {
         CString  drillname;
         drillname.Format("Drill_%lg", block->getToolSize());

         fprintf(wfp,"%sCIRCLEDEF: \"%s\", %lg;\n", ident,drillname, block->getToolSize()); 
      }
   }

   wgencam_minusident();   // usedin
   wgencam_minusident();   // primitives

   fprintf(wfp,"%s$ENDPRIMITIVES\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
static int gencam_padstacks(FILE *wfp)
{
   FileStruct  *file;
   int         cnt = 0;

   fprintf(wfp,"%s$PADSTACKS\n", ident);
   wgencam_plusident();

   // here global
   do_padstacks(wfp,-1);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      do_padstacks(wfp, file->getFileNumber());
   }

   wgencam_minusident();   // padstacks

   fprintf(wfp,"%s$ENDPADSTACKS\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
static int gencam_patterns(FILE *wfp)
{
   FileStruct  *file;
   int         cnt = 0;

   fprintf(wfp,"%s$PATTERNS\n", ident);
   wgencam_plusident();

   // here global
   do_pcbshapes(wfp,-1);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      do_pcbshapes(wfp, file->getFileNumber());
   }

   wgencam_minusident();   
   fprintf(wfp,"%s$ENDPATTERNS\n\n",ident);

   return 1;
}

/******************************************************************************
* gencam_packages
*/
static int gencam_packages(FILE *wfp)
{
	// Get comp height for packages from component insert
	CMapStringToString packageCompHeightMap;
	WORD compHeightKey = (WORD)doc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct* file = doc->getFileList().GetNext(pos);
		if (file == NULL)
			continue;

		BlockStruct* block = file->getBlock();
		if (block == NULL || block->getBlockType() != blockTypePcb)
			continue;

		POSITION dataPos = block->getHeadDataInsertPosition();
		while (dataPos != NULL)
		{
			DataStruct* data = block->getNextDataInsert(dataPos);
			if (data == NULL || data->getInsert() == NULL)
				continue;

			if (data->getAttributes() == NULL)
				continue;

			CString compHeightString = "";
			BlockStruct* subBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());

			if (packageCompHeightMap.Lookup(subBlock->getName(), compHeightString))
				continue;
			
			Attrib *attrib = NULL;
			if (data->getAttributes() && data->getAttributes()->Lookup(compHeightKey, attrib) && attrib != NULL)
			{
				compHeightString.Format("%f", attrib->getDoubleValue());
				packageCompHeightMap.SetAt(subBlock->getName(), compHeightString);
			}	
		}
   }


   fprintf(wfp,"%s$PACKAGES\n", ident);
   wgencam_plusident();

   // here global
   do_pcbpackages(wfp, -1, packageCompHeightMap);

   pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct* file = doc->getFileList().GetNext(pos);
      do_pcbpackages(wfp, file->getFileNumber(), packageCompHeightMap);
   }

   wgencam_minusident();
   fprintf(wfp,"%s$ENDPACKAGES\n\n",ident);

	packageCompHeightMap.RemoveAll();

   return 1;
}

//--------------------------------------------------------------
static int gencam_devices(FILE *wfp)
{
   FileStruct  *file;
   int         cnt = 0;

   fprintf(wfp,"%s$DEVICES\n", ident);
   wgencam_plusident();

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);

      POSITION typPos = file->getTypeList().GetHeadPosition();
      if (typPos) 
      {
         fprintf(wfp,"%sGROUP:\"%s\";\n",ident, get_groupname(GROUP_DEVICES, file->getFileNumber()));
         wgencam_plusident();
         do_devices(wfp, file, &file->getTypeList());
         wgencam_minusident();
      }
   }
   wgencam_minusident();   // devices

   fprintf(wfp,"%s$ENDDEVICES\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
static int get_fiducial(FILE *fp, CDataList *DataList, int filenum, int first)
{
   DataStruct *np;
   //int first = FALSE;
   static double size;


   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)     
         continue;
   
      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      if (block == NULL)
         continue;

      if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL) 
         if (block->getBlockType() == BLOCKTYPE_PADSTACK && filenum == block->getFileNumber())
         {
            get_fiducial(fp, &(block->getDataList()), filenum, first);
            if (first)
            {
               fprintf(fp,"%sGROUP: \"%s\";\n",ident, get_groupname(GROUP_ARTWORK, filenum));
               wgencam_plusident();
               first = FALSE;
            }
            fprintf(fp,"%sTARGETDEF: \"%s\", ALIGNMENT, ;\n", ident, block->getName());
            wgencam_plusident();
            fprintf(fp,"%sCIRCLE: %1.*lf, , , , (0,0)\n",ident,output_units_accuracy, size);
            wgencam_minusident();
         }

      
      if (block->getFlags() & BL_WIDTH)
         continue;
      if (block->getFlags() & BL_SMALLWIDTH)
         continue;
      if ( ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE)) && block->getShape() == T_ROUND )
         size = block->getSizeA();
   }
   if (!first)
      wgencam_minusident();
   return 1;
}


//--------------------------------------------------------------
static int gencam_artworks(FILE *wfp)
{
   FileStruct  *file;
   int         cnt = 0;

   fprintf(wfp,"%s$ARTWORKS\n", ident);
   wgencam_plusident();

   // here global
   do_artworkshapes(wfp,-1, TRUE);   

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      int first = TRUE;
      file = doc->getFileList().GetNext(pos);  
      get_fiducial(wfp, &file->getBlock()->getDataList(), file->getFileNumber(), first);
      do_artworkshapes(wfp, file->getFileNumber(), first);     
   }
   wgencam_minusident();   // artworks

   fprintf(wfp,"%s$ENDARTWORKS\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
static int gencam_header(FILE *wfp, int unit)
{
   FileStruct  *file;
   POSITION    pos;

   fprintf(wfp,"%s$HEADER\n", ident);
   wgencam_plusident();
   fprintf(wfp,"%sGENCAM: \"%s\";\n",ident,GENCAM_VERSION);
   fprintf(wfp,"%sGENERATEDBY: \"%s\", \"%s\";\n",ident, getApp().getCompanyNameString(), getApp().getCamCadTitle());

   // here loop throu all Boards
   pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())
         continue;
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         fprintf(wfp,"%sBOARD:\"%s\", \"BoardName\", \"BoardNumber\", \"BoardRev\";\n",ident, file->getName());
      }
   }
   // here loop throu all Assembly
   pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  
         continue;
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         fprintf(wfp,"%sASSEMBLY:\"%s_Assy\", \"AssyName\", \"AssyNumber\", \"AssyRev\";\n",ident, file->getName());
      }
   }
   // here loop throu all PANELS
   pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())
         continue;
      if (file->getBlockType() == BLOCKTYPE_PANEL)   
      {
         fprintf(wfp,"%sPANEL:\"%s\", \"PanelName\", \"PanelNumber\", \"PanelRev\";\n",ident, file->getName());
      }
   }

/*
  ASSEMBLY: "C100", "Modem C100 mrboard", "11149-14811", "Rev 566g 20";
    ATTRIBUTE: "alpha", "m_part", "BIS 9600";
    ATTRIBUTE: "alpha", "m_desc", "Issue 2";
    MODIFIED: "Harry", 1990-09-20, "changed part number";
  BOARD: "ModemBd", "11354-66540", "R1.3a", "Ver7";
  UNITS: THOU10, 50;
  ANGLEUNITS: DEGREES;
  COLOR: "Black", 0, 0, 0;
  COLOR: "White", 255, 255, 255;
  COLOR: "Shaded", 127, 127, 127;
  HISTORY: 1;
*/ 
   if (unit == UNIT_INCHES)
      fprintf(wfp,"%sUNITS: INCH;\n", ident);
   else
   if (unit == UNIT_MILS)
      fprintf(wfp,"%sUNITS: THOU;\n", ident);
   else
   if (unit == UNIT_MM)
      fprintf(wfp,"%sUNITS: MM;\n", ident);
   else
   {
      fprintf(flog, "Unknown units\n");
      display_error++;
   }

   fprintf(wfp,"%sANGLEUNITS: DEGREES;\n", ident);
   
   // history
   int increment = 1;   // number of iterations
   CString  originaldate = get_XMLDate();
   CString  lastchangeddate = get_XMLDate();
   fprintf(wfp,"%sHISTORY: %d, \"%s\", \"%s\";\n", ident, increment, originaldate, lastchangeddate);

   wgencam_minusident();
   fprintf(wfp,"%s$ENDHEADER\n\n",ident);

   return 1;
}

//--------------------------------------------------------------
static int gencam_administration(FILE *wfp)
{
   FileStruct *file;

   fprintf(wfp,"$ADMINISTRATION\n");
   wgencam_plusident();

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())
         continue;
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         fprintf(wfp,"%sBOARD:\"%s\", ;\n",ident, file->getName());
         fprintf(wfp,"%sASSEMBLY:\"%s_Assy\", ;\n",ident, file->getName());
      }
   }

   CString  xmltime = get_XMLDate();

   fprintf(wfp,"%sTRANSACTION: PO, \"1\", \"%s\";\n", ident, xmltime);

   fprintf(wfp,"%sPERSON:\"MRSMITH\", \"Person Name\", \"Company\", \"City\", \"Street_addr\", \"State\", \"Country\", \"Zip\";\n", ident);
   fprintf(wfp,"%sSENT:\"MRSMITH\";\n", ident);
   fprintf(wfp,"%sOWNER:\"MRSMITH\";\n", ident);
   fprintf(wfp,"%sRECEIVED:\"MRSMITH\";\n", ident);
   fprintf(wfp,"%sENTERPRISE: \"MENTOR GRAPHICS\",\"Company\",\"Street Address\",\"City\",\"State/Province\",\"US\",\"PostCode\",\"Phone\",\"Fax\",\"EMail\",\"URL\",;\n",
      ident);

   wgencam_minusident();
   fprintf(wfp,"$ENDADMINISTRATION\n\n");
   return 1;
}

//--------------------------------------------------------------
static void do_layerlist()
{
   LayerStruct *layer;
   int         ON, signr = 0;
   char        typ = 'D';
   CString     genlay;
   int         unknownlayer = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL) // could have been deleted.
         continue;
      if (doc->IsFloatingLayer(layer->getLayerIndex()))  // no floating layers, they should never be in a gencam file.
         continue;

      ON = TRUE;
      signr = 0;

      // here check if layer was in mentor.out. If not set it non visible
      genlay = layer->getName();
      typ = 'D';

      if (layer->getLayerType() == LAYTYPE_DIALECTRIC)
         continue;

      if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         genlay = "TOP";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         genlay = "BOTTOM";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
         genlay = "ALL";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
         genlay = "PADINNER";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL)
      {
         signr = LAY_ALL;
         genlay = "ALL";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_ALL)
      {
         signr = LAY_ALL;
         genlay = "ALL";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         signr = LAY_TOP;
         genlay = "PADTOP";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         signr = LAY_BOT;
         genlay = "PADBOTTOM";
         ON = TRUE;
      }
      else
      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         genlay.Format("INNER%d",signr-1);
         ON = TRUE;
      }
      else  // must be after INNER%d
      if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         signr = LAY_INNER;
         genlay = "INNER";
         ON = TRUE;
      }
      else  
      if (layer->getLayerType() == LAYTYPE_PAD_OUTER)
      {
         signr = LAY_OUTER;
         genlay = "PADOUTER";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PASTE_ALL)
      {
         signr = 0;
         genlay = "SOLDERPASTE";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PASTE_TOP)
      {
         signr = 0;
         genlay = "SOLDERPASTE_TOP";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
      {
         signr = 0;
         genlay = "SOLDERPASTE_BOTTOM";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_ALL)
      {
         signr = 0;
         genlay = "SOLDERMASK";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_TOP)
      {
         signr = 0;
         genlay = "SOLDERMASK_TOP";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_BOTTOM)
      {
         signr = 0;
         genlay = "SOLDERMASK_BOTTOM";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_TOP)
      {
         signr = 0;
         genlay = "SILKSCREEN_TOP";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM)
      {
         signr = 0;
         genlay = "SILKSCREEN_BOTTOM";
         ON = TRUE;
      }
      else
      if (layer->getLayerType() == LAYTYPE_BOARD_OUTLINE)
      {
         signr = 0;
         genlay = "ALL";
         ON = TRUE;
      }
      else
      {
         if (!doc->get_layer_visible(j, -1))
            continue;

         genlay.Format("LAYER%d", unknownlayer++);
      }

      if (signr == 0)
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, -1))
            continue;
      }
      else
      {
         // here check if layer->getName() is not already done
         if (!doc->get_layer_visible(j, -1))
         {
            // here a warning that an etch layer is not visible
            //CString t;
            //t.Format("Electrical Layer [%s] not visible!\nNon Visible Layers will NOT be translated.",layer->getName());
            //ErrorMessage(t, "Layer Display", MB_ICONEXCLAMATION | MB_OK);
         }
      }

      //glArr.SetSizes
      GenCAMLayerStruct *gl = new GenCAMLayerStruct;
      gl->stackNum = signr;
      gl->layerindex = j;
      gl->on = ON;
      gl->type = typ;
      gl->oldName = layer->getName();
      gl->newName = genlay;
      glArr.SetAtGrow(maxArr, gl);
      maxArr++;
   }

   return;
}

//--------------------------------------------------------------
static int edit_layerlist()
{
   // fill array
   GenCAMLayerDlg gldlg;

   gldlg.arr = &glArr;
   gldlg.maxArr = maxArr;

   if (gldlg.DoModal() != IDOK) 
      return FALSE;

   return TRUE;
}

//--------------------------------------------------------------
static void free_layerlist()
{

   for (int i=0;i<maxArr;i++)
   {
      delete glArr[i];
   }
   glArr.RemoveAll();
   maxArr = 0;

   return;
}

//--------------------------------------------------------------
void GENCAM_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format,
                     int page_units)
{
   FILE *wfp;

   doc = Doc;
   display_error = FALSE;

   ident = ""; 
   identcnt = 0;

   viacnt = 0;
   testpadcnt = 0;
   holecnt = 0;

   typemaparray.SetSize(100,100);
   typemapcnt = 0;

   routesarray.SetSize(100,100);
   routescnt = 0;
   
   PadRotarray.empty();

	CString logFile = GetLogfilePath("GENCAM.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      ErrorMessage("Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   
   CString settingsFile( getApp().getExportSettingsFilePath("GENCAM.out") );
   load_GENCAMsettings(settingsFile);

   check_value_attributes(doc);

   do_layerlist();
   if (edit_layerlist())
   {
      CWaitCursor wait;

      // open file for writting
      if ((wfp = fopen(filename, "wt")) == NULL)
      {
         // error - can not open the file.
         CString  tmp;
         tmp.Format("Can not open %s",filename);
         ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
         return;
      }

      // here switch on PCB files, if PANELS are on and use this files.
      switch_on_pcbfiles_used_in_panels(doc, TRUE);

      // the sequence is not changable
      wGENCAM_Graph_Init_Layout(output_units_accuracy);

      // header
      gencam_header(wfp, page_units);
   
      // administration
      gencam_administration(wfp);

      // primitives
      gencam_primitives(wfp);

      // artworks
      gencam_artworks(wfp);

      // layers
      gencam_layers(wfp);  // here store toplegend layer

      // padstacks
      gencam_padstacks(wfp);

      // patterns
      gencam_patterns(wfp);

      // packages
      gencam_packages(wfp);

      // power only write out if not empty

      // devices
      gencam_devices(wfp);

      // components
      gencam_components(wfp);

      // mechanicals only write out if not empty
      //fprintf(wfp,"$MECHANICALS\n");
      //fprintf(wfp,"$ENDMECHANICALS\n\n");

      // routes
      gencam_routes(wfp);

      // boards
      gencam_boards(wfp);

      // panels
      gencam_panels(wfp);

      // testconnects
      //fprintf(wfp,"$TESTCONNECTS\n");
      //fprintf(wfp,"$ENDTESTCONNECTS\n\n");

      // fixtures 
      //fprintf(wfp,"$FIXTURES\n");
      //fprintf(wfp,"$ENDFIXTURES\n\n");

      // drawings
      gencam_drawings(wfp); // all but FIXTURES, BOARDS, PANELS

      // changes
      //fprintf(wfp,"$CHANGES\n");
      //fprintf(wfp,"$ENDCHANGES\n\n");

      // logical end of file. 
      fprintf(wfp,"\n$$\n");
      wGENCAM_Graph_File_Close_Layout();

      switch_on_pcbfiles_used_in_panels(doc, FALSE);

      // close write file
      fclose(wfp);
   }
   free_layerlist();

   fprintf(flog,"\nGenCAM syntax checker available: http://www.gencam.org/ctmupload2.html\n");

   fclose(flog);

	int i=0;
   for (i=0;i<typemapcnt;i++)
   {
      delete typemaparray[i];
   }
   typemaparray.RemoveAll();

   for (i=0;i<routescnt;i++)
   {
      delete routesarray[i];
   }
   routesarray.RemoveAll();
   routescnt = 0;

   if (display_error)
      Logreader(logFile);

   return;
}

/******************************************************************************
* gencam_polyclosed
*/
static int gencam_polyclosed(FILE *wfp, const char *quallayer, const char *name, int gr_class,
      CPoly *poly, double insert_x, double insert_y, double scale, int mirror, double rotation)
{
   CPnt     *pnt;
   POSITION pntPos, lastPos;

   Mat2x2      m;
   RotMat2(&m, rotation);

   switch (gr_class)
   {
      case GR_CLASS_ROUTKEEPIN:
         fprintf(wfp,"%sKEEPIN: \"%s\", ROUTE, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_ROUTKEEPOUT:
         fprintf(wfp,"%sKEEPOUT: \"%s\", ROUTE, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_PLACEKEEPIN:
         return 0;   // not implemented
         fprintf(wfp,"%sKEEPIN: \"%s\", COMPONENT, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_PLACEKEEPOUT:
         fprintf(wfp,"%sKEEPOUT: \"%s\", COMPONENT, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_VIAKEEPIN:
         return 0;   // not implemented
         fprintf(wfp,"%sKEEPIN: \"%s\", VIA, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_VIAKEEPOUT:
         fprintf(wfp,"%sKEEPOUT: \"%s\", VIA, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_ALLKEEPIN:
         return 0;   // not implemented
         fprintf(wfp,"%sKEEPIN: \"%s\", ALL, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_ALLKEEPOUT:
         fprintf(wfp,"%sKEEPOUT: \"%s\", ALL, %s;\n",ident, name, quallayer);
      break;
      case GR_CLASS_MILLINGPATH:
         fprintf(wfp,"%sMILLING: \"%s\",  %s;\n",ident, name, quallayer);
      break;
      case graphicClassBoardCutout:
         fprintf(wfp,"%sCUTOUT: \"%s\", %s;\n",ident, name, quallayer);
      break;
      case graphicClassTestPointKeepOut:
         fprintf(wfp,"%sKEEPOUT: \"%s\", TESTPOINT, %s;\n",ident, name, quallayer);
      break;
      case graphicClassTestPointKeepIn:
         return 0;   // not implemented
         fprintf(wfp,"%sKEEPIN: \"%s\", TESTPOINT, %s;\n",ident, name, quallayer);
      break;
      default:
         return 0;
      break;
   }
   wgencam_plusident();

   CString  gname = get_groupname(GROUP_PRIMITIVES, -1);
   fprintf(wfp,"%sPOLYGON: \"%s\".\"LINEDESC%d\", \"%s\".\"HOLLOW\";\n",ident, gname, 0, gname);
   wgencam_plusident();

   int first = TRUE, last = FALSE;
   Point2   firstp;
   Point2   p1,p2;
   pntPos = poly->getPntList().GetHeadPosition();
   lastPos = poly->getPntList().GetTailPosition();

   while (pntPos != NULL)
   {
      if (pntPos == lastPos)  last = TRUE;
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
         wGENCAM_Graph_StartAt(ident, wfp, p2.x, p2.y);
      }
      else
      {
         if (last)
            wGENCAM_Graph_EndAt(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
         else
            wGENCAM_Graph_NextAt(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
         p1 = p2;
      }
   }
   wgencam_minusident();   // polyline
   wgencam_minusident();   // outline

   return 1;
}

/******************************************************************************
* GENCAM_WritePRIMARYBOARDData
*/
void GENCAM_WritePRIMARYBOARDData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   DataStruct *np;
   int         layer;
   Point2      point2;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_POLY && np->getDataType() != T_INSERT)    
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

         // this is already done in the GENCAM_BOARDOUTLINE routine, which is called before.
         if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
            continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            POSITION polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               CPoly *poly = np->getPolyList()->GetNext(polyPos);

               if (poly->isHidden()) 
                  continue;

               if (poly->isClosed())
               {
                  CString lyr, name;
                  lyr.Format("\"%s\".\"%s\"", get_groupname(GROUP_LAYERS, -1), Layer_Gencam(layer));
                  name.Format("Board_%d", np->getEntityNumber());
                  gencam_polyclosed(wfp, lyr, name, np->getGraphicClass(), poly, insert_x, insert_y, scale, mirror, rotation);
               }
            }  // while
         }
         break;  // POLYSTRUCT

         case T_INSERT:
            if (np->getInsert()->getInsertType() != INSERTTYPE_UNKNOWN && np->getInsert()->getInsertType() != INSERTTYPE_FIDUCIAL)
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

            if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL) // board fiducials
            {
               CString refname = np->getInsert()->getRefname();             
               LayerStruct *layer = doc->FindLayer(np->getLayerIndex());
               if (layer)
               {
                  CString layername = layer->getName();
                  BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
                  fprintf(wfp,"%sTARGETREF: \"%s\", \"%s\".\"%s\", %s, , (%lg, %lg);\n", 
                     ident, refname, get_groupname(GROUP_ARTWORK, block->getFileNumber()), 
                     block->getName(), layername, point2.x, point2.y);
               }
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
               //fprintf(wfp,"%sAPERTURE Reference found [%s]\n", ident, block->name);
               fprintf(flog, "DEBUG: Aperture found [%s]\n", block->getName());
               display_error++;
            }
             else // not aperture
            {
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else 
               if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               GENCAM_WritePRIMARYBOARDData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         break;
      } // end switch
   } // end GENCAM_WritePRIMARYBOARDData */
}

//--------------------------------------------------------------
void GENCAM_WritePRIMARYHOLEData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   DataStruct *np;
   Point2      point2;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
         continue;

      switch(np->getDataType())
      {
         case T_INSERT:
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

            if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
               BlockStruct *b = GetDrillInGeometry(doc, block);

               if (b)
               {
                  fprintf(wfp,"%sHOLEREF: \"%s\".\"HOLE_%d\", (%lg, %lg);\n", 
                     ident, get_groupname(GROUP_PADSTACKS, b->getFileNumber()), 
                     b->getBlockNumber(), point2.x, point2.y);
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)
            {
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               fprintf(wfp,"%sHOLEREF: \"%s\".\"HOLE_%d\", (%lg, %lg);\n", 
                  ident, get_groupname(GROUP_PADSTACKS, block->getFileNumber()), 
                  block->getBlockNumber(), point2.x, point2.y);
            }           
            else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               break;
            }
            else // not aperture
            {
               int block_layer = -1;

               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               GENCAM_WritePRIMARYHOLEData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         break;
      } // end switch
   } // end GENCAM_WritePRIMARYHOLEData */
   return;
}

//--------------------------------------------------------------
void GENCAM_WritePRIMARYPANELData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   DataStruct *np;
   int         layer;
   Point2      point2;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
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
            POSITION polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               CPoly *poly = np->getPolyList()->GetNext(polyPos);

               if (poly->isHidden())
                  continue;

               if (poly->isClosed())    
               {
                  CString  l, name;
                  name.Format("Panel_%d", np->getEntityNumber());
                  l.Format("\"%s\".\"%s\"",get_groupname(GROUP_LAYERS, -1), Layer_Gencam(layer));
                  gencam_polyclosed(wfp, l, name, np->getGraphicClass(), poly, insert_x, insert_y, scale, mirror, rotation);
               }
               else
               {
                  wgencam_plusident();
                  int first = TRUE;
                  Point2   firstp;
                  Point2   p1,p2;

                  POSITION pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntPos);
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
                        wGENCAM_Graph_Line(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        p1 = p2;
                     }
                  }
                  wgencam_minusident();
               } // if !closed
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            /* No Text is Panel section */
               fprintf(flog, "Text on Panel not implemented\n");
               display_error++;
         break;
         case T_INSERT:
            if (np->getInsert()->getInsertType() != INSERTTYPE_UNKNOWN &&
                np->getInsert()->getInsertType() != INSERTTYPE_FIDUCIAL &&
                np->getInsert()->getInsertType() != INSERTTYPE_PCB &&
                np->getInsert()->getInsertType() != INSERTTYPE_TOOLING &&
                np->getInsert()->getInsertType() != INSERTTYPE_DRILLHOLE)
            {
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               fprintf(flog, "Insert on Panel [%s] not implemented\n",block->getName());
               display_error++;
               break;
            }

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

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCB)
            {
               fprintf(wfp,"%sPLACEMENT: \"%s\", (%lg, %lg), %lg %s;\n", ident,
                  block->getName(), point2.x, point2.y, RadToDeg(block_rot),
                  (block_mirror)?", MIRROR": "");
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
               BlockStruct *b = GetDrillInGeometry(doc, block);

               if (b)
               {
                  fprintf(wfp,"%sHOLEREF: \"%s\".\"HOLE_%d\", (%lg, %lg);\n", 
                     ident, get_groupname(GROUP_PADSTACKS, b->getFileNumber()), 
                     b->getBlockNumber(), point2.x, point2.y);
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)
            {
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               fprintf(wfp,"%sHOLEREF: \"%s\".\"HOLE_%d\", (%lg, %lg);\n", 
                  ident, get_groupname(GROUP_PADSTACKS, block->getFileNumber()), 
                  block->getBlockNumber(), point2.x, point2.y);
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
            {
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
               CString refname = np->getInsert()->getRefname();          
               LayerStruct *layer = doc->FindLayer(np->getLayerIndex());
               if (layer)
               {
                  CString layername = layer->getName();
                  fprintf(wfp,"%sTARGETREF: \"%s\", \"%s\".\"%s\", %s, , (%lg, %lg);\n", 
                     ident, refname, get_groupname(GROUP_ARTWORK, block->getFileNumber()), 
                     block->getName(), layername, point2.x, point2.y);
               }
            }
            else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
               //fprintf(wfp,"%sAPERTURE Reference found [%s]\n", ident, block->name);
               fprintf(flog,"DEBUG: Aperture in PANEL not implemented [%s]\n", block->getName());
               display_error++;

            }
            else // not aperture
            {
               int block_layer = -1;

               if (insertLayer != -1)
                  block_layer = insertLayer;
               else 
                  if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               GENCAM_WritePRIMARYPANELData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         break;
      } // end switch
   } // end GENCAM_WritePRIMARYPANELData */
}

//--------------------------------------------------------------
static void GENCAM_WriteARTWORKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   int   artworkattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if ((np->getFlags() & GR_CLASS_BOARD_GEOM) == GR_CLASS_BOARD_GEOM)   
            continue;
         if ((np->getFlags() & GR_CLASS_ETCH)       == GR_CLASS_ETCH)      
            continue;
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
               if (poly->isHidden())
                  continue;

               if (poly->isClosed())
               {
                  CString  l, name;
                  name.Format("Artwork_%d", np->getEntityNumber());
                  l.Format("\"%s\".\"%s\"",get_groupname(GROUP_LAYERS, -1), Layer_Gencam(layer));
                  gencam_polyclosed(wfp, l, name, np->getGraphicClass(), poly, insert_x, insert_y, scale, mirror, rotation);
               }
               else
               {
                  int first = TRUE;
                  Point2   firstp;
                  Point2   p1,p2;

                  //wGENCAM_Graph_MaskLevel(wfp, Layer_Gencam(layer));
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
                        wGENCAM_Graph_Line(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        // must be written after every line
                        write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
                        p1 = p2;
                     }
                  }
               } // if closed
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            //wGENCAM_Graph_MaskLevel(wfp, Layer_Gencam(layer));
            wGENCAM_Graph_Text(ident, wfp, np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror);
            write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
         }  
         break;
         
         case T_INSERT:
            // skip insert
         break;
      } // end switch
   } // end GENCAM_WriteARTWORKData */
}

//--------------------------------------------------------------
void GENCAM_WriteSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)

{
   int layer;
   int pinattcnt = 0;
   int cnt = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = DataList->GetNext(pos);
		if (data == NULL)
			continue;

      if (ignore_non_manufacturing_info(data))
         continue;

		if (data->getDataType() == dataTypePoly)
		{
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if (!Layer_Gencam(layer))
            continue;

			CString tmpString = "";
         tmpString.Format("\"%s\".\"%s\"", get_groupname(GROUP_LAYERS, -1), Layer_Gencam(layer));
         fprintf(wfp, "%sARTWORK: %s, , (0, 0), 0;\n", ident, tmpString);
         wgencam_plusident();

         // loop thru polys
         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            CPoly* poly = data->getPolyList()->GetNext(polyPos);
            if (poly == NULL || poly->isHidden())
               continue;

            int first = TRUE;
            Point2 firstp;
            Point2 p1,p2;

            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt* pnt = poly->getPntList().GetNext(pntPos);
					if (pnt == NULL)
						continue;

               p2.x = pnt->x * scale;
               if (mirror)
						p2.x = -p2.x;
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
                  wGENCAM_Graph_Line(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                  p1 = p2;
               }
            }
         }

         wgencam_minusident();
		}
		else if (data->getDataType() == dataTypeInsert)
		{
         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
			InsertStruct* insert = data->getInsert();
			if (insert == NULL)
				continue;

			Point2 point2;
         point2.x = insert->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = insert->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ insert->getMirrorFlags();
         double block_rot = rotation + insert->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + insert->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(insert->getBlockNumber());

			if (!block)
				continue;

         if (insert->getInsertType() == insertTypeMechanicalPin)
         {
            CString pinname = data->getInsert()->getRefname();
            fprintf(wfp, "%sPADSTACKREF: \"%s\".\"%s\", \"%s\", (%lg, %lg), %lg ;\n", ident, 
                  get_groupname(GROUP_PADSTACKS, block->getFileNumber()),
                  block->getName(), pinname, point2.x, point2.y,RadToDeg(block_rot));

            write_attributes(wfp, data->getAttributesRef(), "MECHPIN", &pinattcnt);
         }
         else if (insert->getInsertType() == insertTypeDrillHole)
         {
            fprintf(flog, "Shape: Drill data found in [%s]\n", block->getName());
            display_error++;
         }
         else if (insert->getInsertType() == insertTypeDrillTool)
         {
            fprintf(flog, "Shape: Tooling data found in [%s]\n", block->getName());
            display_error++;
         }
         else if (insert->getInsertType() == insertTypeFiducial)
         {
            LayerStruct *layer = doc->FindLayer(data->getLayerIndex());
            if (layer != NULL)
            {
               CString layername = layer->getName();
               fprintf(wfp, "%sTARGETREF: \"%s\".\"%s\", %s, , (%lg, %lg);\n", 
                  ident, get_groupname(GROUP_ARTWORK, block->getFileNumber()), 
                  block->getName(), layername, point2.x, point2.y);
            }
         }
         else if (insert->getInsertType() == insertTypePin)
         {
            CString pinname = data->getInsert()->getRefname();
            fprintf(wfp,"%sPADSTACKREF: \"%s\".\"%s\", \"%s\", (%lg, %lg), %lg ;\n", ident, 
                  get_groupname(GROUP_PADSTACKS, block->getFileNumber()),
                  block->getName(), pinname, point2.x, point2.y, RadToDeg(block_rot));

				write_attributes(wfp, data->getAttributesRef(), "PIN", &pinattcnt);
			}
         else if ( block != NULL && (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
            //fprintf(wfp,"%sAPERTURE Reference found [%s]\n", ident, block->name);
            fprintf(flog, "DEBUG: Aperture found in WriteShape [%s]\n", block->getName());
            display_error++;
         }
         else // not aperture
         {
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (data->getLayerIndex() != -1)
               block_layer = data->getLayerIndex();

            GENCAM_WriteSHAPEData(wfp,&(block->getDataList()), point2.x, point2.y,
                  block_rot, block_mirror, scale * data->getInsert()->getScale(), embeddedLevel+1, block_layer);
         }
		}
   }
   return;
}

//--------------------------------------------------------------
void GENCAM_WritePACKAGEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)

{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int   pinattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
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
         // only inserts are done here
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

            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               fprintf(flog, "Package: Mechanical Pin data found in [%s]\n", block->getName());
               display_error++;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)
            {
               fprintf(flog, "Package: Drill data found in [%s]\n", block->getName());
               display_error++;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {
               fprintf(flog, "Package: Tooling data found in [%s]\n", block->getName());
               display_error++;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
            {
               LayerStruct *layer = doc->FindLayer(np->getLayerIndex());
               if (layer)
               {
                  CString layername = layer->getName();
                  fprintf(wfp,"%sTARGETREF: \"%s\".\"%s\", %s, , (%lg, %lg);\n", 
                     ident, get_groupname(GROUP_ARTWORK, block->getFileNumber()), 
                     block->getName(), layername, point2.x, point2.y);
               }
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               CString pinname = np->getInsert()->getRefname();
               // also mirror is allowed
               fprintf(wfp,"%sPIN: \"%s\", , , ,(%lg, %lg) , %lg;\n",ident, 
                     pinname, point2.x, point2.y,RadToDeg(block_rot));
               write_attributes(wfp, np->getAttributesRef(), "PIN", &pinattcnt);
            }
            else
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               //fprintf(wfp,"%sAPERTURE Reference found [%s]\n", ident, block->name);
               fprintf(flog, "DEBUG: Aperture found in Package [%s]\n", block->getName());
               display_error++;

            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else 
                  if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               GENCAM_WritePACKAGEData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end GENCAM_WritePACKAGESData */
   return;
}

//--------------------------------------------------------------
void GENCAM_ArtworkSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)

{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int   pinattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
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
               if (poly->isHidden()) 
                  continue;

               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               //wGENCAM_Graph_MaskLevel(wfp, Layer_Gencam(layer));
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
                     wGENCAM_Graph_Line(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                     // must be written after every line
                     //write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
                     p1 = p2;
                  }
               }
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            //wGENCAM_Graph_MaskLevel(wfp, Layer_Gencam(layer));
            wGENCAM_Graph_Text(ident, wfp, np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror);
            //write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
         }  
         break;
         
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN ||
                np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL ||
                np->getInsert()->getInsertType() == INSERTTYPE_TOOLING ||
                np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               // not done in artwork
               break;
            }
               
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
               //fprintf(wfp,"%sAPERTURE Reference found [%s]\n", ident, block->name);
               fprintf(flog, "DEBUG: Aperture in Shape data not implemented [%s].\n", block->getName());
               display_error++;
            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else 
                  if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();
               fprintf(wfp,"%sARTWORKREF: \"%s\";\n", ident, block->getName());
/*
               GENCAM_WriteSHAPEData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
*/
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end GENCAM_WriteSHAPEData */
   return;
}

//--------------------------------------------------------------
void GENCAM_GetROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   DataStruct *np;
   int   layer;
   Attrib   *a;

   POSITION pos1;
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      pos1 = pos; // pos is changed in getnext
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != GR_CLASS_ETCH)
            continue;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // here check if ATTR_NETNAME == netname
         a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

         if (!a)  continue;
      }
      else
      {
         // here check if ATTR_NETNAME == netname

         // allow only vias and test points
         a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (!a)  continue;
      }

      // here store it
      GENCAMRoutes *r = new GENCAMRoutes;
      routesarray.SetAtGrow(routescnt,r);  
      routescnt++;   
      r->netname = get_attvalue_string(doc, a);
      r->pos = pos1;
   } // end GENCAM_GetROUTESData */
}

//--------------------------------------------------------------
void GENCAM_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct *np;
   int         layer;
   const char  *l;

   int   routeattcnt = 0, viaattcnt = 0, bondpadcnt = 0;

   RotMat2(&m, rotation);

   for (int i=0;i<routescnt;i++)
   {
      // different netname
      if (strcmp(routesarray[i]->netname, netname))
         continue; 
      np = DataList->GetNext(routesarray[i]->pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != GR_CLASS_ETCH)   
            continue;

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

         l = Layer_Gencam(doc->get_layer_mirror(np->getLayerIndex(), mirror));
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
            POSITION polyPos, pntPos, lastPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden())
                  continue;

               if (poly->isFilled())
               {
                  int first = TRUE;
                  int last  = FALSE;

                  Point2   firstp;
                  Point2   p1,p2;
                  CString  gname = get_groupname(GROUP_PRIMITIVES, -1);

                  fprintf(wfp,"%sPLANE: \"%s\".\"%s\", \"%s\".\"FILL\";\n",ident, 
                     get_groupname(GROUP_LAYERS, -1), l, gname );
                  wgencam_plusident();
                  if (poly->isVoid())
                     fprintf(wfp,"%sPOLYGON: \"%s\".\"LINEDESC%d\", \"%s\".\"VOID\";\n",ident, 
                        gname, poly->getWidthIndex(), gname);
                  else
                     fprintf(wfp,"%sPOLYGON: \"%s\".\"LINEDESC%d\", \"%s\".\"FILL\";\n",ident, 
                        gname, poly->getWidthIndex(), gname);

                  wgencam_plusident();
                  pntPos = poly->getPntList().GetHeadPosition();
                  lastPos = poly->getPntList().GetTailPosition();

                  while (pntPos != NULL)
                  {
                     if (pntPos == lastPos)  last = TRUE;
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
                        wGENCAM_Graph_StartAt(ident, wfp, p2.x, p2.y);
                     }
                     else
                     if (last)
                     {
                        wGENCAM_Graph_EndAt(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                     }
                     else
                     {
                        wGENCAM_Graph_NextAt(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        p1 = p2;
                     }
                  }
                  wgencam_minusident();      // poly
                  wgencam_minusident();   // plane
               }
               else
               {
                  int first = TRUE;
                  Point2   firstp;
                  Point2   p1,p2;
                  CString  group, primgroup;
                  group = get_groupname(GROUP_LAYERS, -1);
                  primgroup = get_groupname(GROUP_PRIMITIVES, -1);
                  fprintf(wfp,"%sPATH: \"%s\".\"%s\", \"%s\".\"LINEDESC%d\";\n",ident, 
                     group, l, primgroup, poly->getWidthIndex());
                  wgencam_plusident();
                  fprintf(wfp,"%sPOLYLINE: ;\n",ident);
                  wgencam_plusident();
                  pntPos = poly->getPntList().GetHeadPosition();
                  lastPos = poly->getPntList().GetTailPosition();

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
                        wGENCAM_Graph_StartAt(ident, wfp, p2.x, p2.y);

                     }
                     else
                     {
                        wGENCAM_Graph_NextAt(ident, wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        p1 = p2;
                     }
                  }
                  wgencam_minusident();   // polyline

                  write_attributes(wfp, np->getAttributesRef(), "ROUTE", &routeattcnt);
                  wgencam_minusident();   // path
               }
            }  // while
            // attributes
            write_attributes(wfp, np->getAttributesRef(), "ROUTE", &routeattcnt);
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

            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
            {
               CString  tpname;

               if (np->getInsert()->getRefname())
                  tpname = np->getInsert()->getRefname();
               else
                  tpname.Format("testpad%d",++testpadcnt);

               fprintf(wfp,"%sTESTPAD: \"%s\", \"%s\".\"%s\", %lg %lg, %lg %s;\n", ident, 
                  tpname, get_groupname(GROUP_PADSTACKS, -1), block->getName(), point2.x, point2.y,
                  RadToDeg(block_rot),
                  (block_mirror & MIRROR_FLIP)? ", MIRROR": "");
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               CString  vianame;

               vianame = np->getInsert()->getRefname();
               vianame.TrimLeft();
               vianame.TrimRight();

               if (strlen(vianame) == 0)
                  vianame.Format("via%d",++viacnt);

               fprintf(wfp,"%sVIA: \"%s\", \"%s\".\"%s\", , NOPROBE, (%lg, %lg);\n", ident, 
                  vianame, get_groupname(GROUP_PADSTACKS, block->getFileNumber()), block->getName(), point2.x, point2.y);
               // attributes
               write_attributes(wfp, np->getAttributesRef(), "VIA", &viaattcnt);
               break;
            }
            else if (np->getInsert()->getInsertType() == insertTypeBondPad)
            {
               CString  bondpadname;
               CString  blockName;

               bondpadname = np->getInsert()->getRefname();
               bondpadname.TrimLeft();
               bondpadname.TrimRight();

               if (strlen(bondpadname) == 0)
                  bondpadname.Format("bondpad%d",++bondpadcnt);

               DataStruct *padstackdata = block->GetFirstPinInsertData();
               if(padstackdata)
               {                  
                  InsertStruct *padstackinsert = padstackdata->getInsert();
                  BlockStruct *padblock = GetBondPadPadstackBlock(doc->getCamCadData(), block);
                  if(padblock)
                  {
                     //Support rotated geometry 
                     double padrout = padstackinsert->getAngle();
                     double rot = block_rot + (padstackinsert->getGraphicMirrored()?(PI2 - padrout):padrout);
                     int rotdegree = normalizeDegrees(round(RadToDeg(rot)));

                     if(rotdegree)
                        blockName.Format("%s_%d", padblock->getName(), rotdegree);                     
                     else
                        blockName = padblock->getName();
                  }

                  fprintf(wfp,"%sVIA: \"%s\", \"%s\".\"%s\", , NOPROBE, (%lg, %lg);\n", ident, 
                  bondpadname, get_groupname(GROUP_PADSTACKS, block->getFileNumber()), blockName, point2.x, point2.y);
                  
                  // attributes
                  write_attributes(wfp, np->getAttributesRef(), "VIA", &viaattcnt);
               }

               break;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {
               fprintf(flog,"FreePad found in Net [%s] -> not supported in GENCAM\n",  netname);
               display_error++;
               break;
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
               fprintf(flog,"DEBUG: APERTURE Reference [%s] found in Net [%s] -> not supported in GENCAM\n", block->getName(), netname);
               display_error++;

               break;
            }
            else // not aperture
            {
               fprintf(flog,"Hierachical Reference [%s] found in Net [%s] -> not supported in GENCAM\n",  block->getName(), netname);
               display_error++;

/* can not process hierachical process, because GENCAM_WriteROUTESData loops through the net array
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               GENCAM_WriteROUTESData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, netname);
*/
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end GENCAMD_WriteROUTESData */
}

//--------------------------------------------------------------
void GENCAM_WriteCOMPONENTData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int filenum)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   compattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        
         continue;

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

/*
            if (block_mirror)
               block_rot = PI2 - (rotation + np->getInsert().angle);   // add 180 degree if mirrored.
*/
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               CString compname = np->getInsert()->getRefname();
               Attrib *a;

               fprintf(wfp,"%sCOMPONENT: \"%s\"",ident, compname);

               fprintf(wfp,", \"%s\".\"%s\"", get_groupname( GROUP_LAYERS, -1), (block_mirror & MIRROR_LAYERS)?"COMPBOT":"COMPTOP");
               fprintf(wfp,", (%lg, %lg)",point2.x, point2.y);
               fprintf(wfp,", %lg",RadToDeg(block_rot));
               if (block_mirror & MIRROR_FLIP)
                  fprintf(wfp,", MIRROR");
               fprintf(wfp,";\n",RadToDeg(block_rot));

               wgencam_plusident();

               fprintf(wfp,"%sPATTERNREF: \"%s\".\"%s\";\n",ident, get_groupname(GROUP_PATTERN, block->getFileNumber()), block->getName());
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
               {
                  fprintf(wfp,"%sDEVICEREF: \"%s\".\"%s\";\n",ident, 
                     get_groupname(GROUP_DEVICES, filenum), get_attvalue_string(doc, a));
               }

               // attributes
               write_attributes(wfp, np->getAttributesRef(), "COMPONENT", &compattcnt);
               wgencam_minusident();
            }
         } // case INSERT
         break;
      } // end switch
   } // end GENCAM_WriteCOMPONENTData */
   return;
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
void GENCAM_WritePANELBOARDSData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, 
      const char *filename, const char *ext)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   boardcnt = 0;
   int   boardattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)     
         continue;

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

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCB)
            {
               fprintf(wfp,"BOARD BOARD%d\n",++boardcnt);
               fprintf(wfp,"FILE %s%s\n",filename,ext);
               fprintf(wfp,"FIDUCIAL 0.0 0.0\n");
               fprintf(wfp,"ROTATION %lg\n",RadToDeg(block_rot));
               fprintf(wfp,"PLACE %lg %lg\n",point2.x, point2.y);
               // attributes
               fprintf(wfp,"ATTRIBUTE CAD %s ?\n",block->getName());
               write_attributes(wfp, np->getAttributesRef(), "BOARD", &boardattcnt);
               break;
            }
            else
            {
               fprintf(flog, "Insert [%s] in Panel found\n",block->getName());
               display_error++;
            }
         } // case INSERT
         break;
      } // end switch
   } // end GENCAM_WritePANELBOARDSData */
   return;
}

//--------------------------------------------------------------
void GENCAM_WritePADSTACKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   padattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)     
         continue;

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
            if (insertLayer != -1 && doc->IsFloatingLayer(np->getLayerIndex()))
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            // insert does not have a display type, therefore do not use.
            // if (!doc->get_layer_visible(block_layer, mirror, np->getDataType())) break;

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            
            double offx, offy;
            double rot = RadToDeg(block_rot + block->getRotation());
   
            while (rot < 0)   rot += 360;
            while (rot >= 360)   rot -= 360;

            Rotate(block->getXoffset()*scale, block->getYoffset()*scale, rot, &offx, &offy);

            if ( (block->getFlags() & BL_TOOL)) 
            {
               fprintf(wfp,"%sHOLEREF: \"%s\".\"HOLE_%d\", (%lg, %lg), %lg;\n", 
                  ident, get_groupname(GROUP_PADSTACKS, block->getFileNumber()), 
                  block->getBlockNumber(), point2.x+offx, point2.y+offy, rot);
            }
            else
            {
               //if (block->getBlockType() == BLOCKTYPE_DRILLHOLE)  break;

               CString  layername = Layer_Gencam(block_layer);
               // should have a pointx,y reference
               if (strlen(layername))
               {
                  // need to add offset and pad rotation.
                  CString  gname, fill;

                  fill = "";
                  gname = get_groupname(GROUP_PRIMITIVES, block->getFileNumber());
                  if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     if (block->getShape() == T_TARGET) // targets have to be defined in ARTWORK
                        gname = get_groupname(GROUP_ARTWORK, block->getFileNumber());
                     else
                     {
                        fill = "\"prim.global\".\"FILL\"";
                     }
                  }

                  fprintf(wfp,"%sPAD: \"%s\".\"%s\",", ident, get_groupname(GROUP_LAYERS, -1), layername);

                  fprintf(wfp,"\"%s\".\"%s\", , %s , , , (%lg, %lg), %lg;\n", gname, block->getName(), fill,
                     point2.x+offx, point2.y+offy, rot);
                  write_attributes(wfp, np->getAttributesRef(), "PAD", &padattcnt);
               };
            }
         } // case INSERT
         break;
      } // end switch
   } // end GENCAM_WritePADSTACKData */
   return;
}


/****************************************************************************/
// CGenCamPadRotArray
/****************************************************************************/
CGenCamPadRotArray::~CGenCamPadRotArray()
{
   empty();
}

bool CGenCamPadRotArray::Lookup(CString padname, double rotation)
{
   for(int index = 0; index < GetCount(); index++)
   {
      GenCamPadRotStruct *padrot = GetAt(index);
      if(padrot && !padname.Compare(padrot->padname) && rotation == padrot->rotation)
         return true;
   }
   return false;
}

void CGenCamPadRotArray::Set(CString padname, CString newpadname, double rotation)
{
   if(!Lookup(padname, rotation))
   {
      GenCamPadRotStruct *padrot = new GenCamPadRotStruct;
      
      padrot->padname = padname;
      padrot->newpadname = newpadname;
      padrot->rotation = rotation;

      Add(padrot);
   }
}

GenCamPadRotStruct* CGenCamPadRotArray::Get(int idx, CString padname)
{
   GenCamPadRotStruct *padrot = GetAt(idx);
   if(padrot && !padname.Compare(padrot->padname))
      return padrot;
   
   return NULL;
}

/****************************************************************************/
/*
   end GENCAM_O.CPP
*/
/****************************************************************************/

