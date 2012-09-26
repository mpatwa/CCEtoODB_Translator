// $Header: /CAMCAD/4.5/read_wrt/DxfOut.cpp 31    8/05/06 4:54p Kurt Van Ness $

/****************************************************************************
* Project CAMCAD
* Router Solutions Inc.
* Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "dxflib.h"
#include "format_s.h"
#include "dbutil.h"
#include "lyr_lyr.h"
#include "graph.h"
#include "attrib.h"
#include "polylib.h"
#include "ck.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "DcaDftCommon.h"
#include "InFile.h"

static CCEtoODBDoc *doc;
static int     display_error;
static FILE    *flog;
static CDXFOutSetting dxfoutSettings;

static void DXF_WriteData(int explodeStyle, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int insertLayer);
/*****************************************************************************/
/* 
   Specify the Decimal of DXFout file 
*/
static int DxfGetDecimals(PageUnitsTag units)

{
   // The decimal can be adjusted based on 
   // the accurancy of the calculation in various unit
   // It will affect the result of DXFout file
   int decimals = 3; // default. 
   
   switch (units)
   {
   case pageUnitsInches:         decimals = 5; break;
   case pageUnitsMilliMeters:    decimals = 5; break;
   case pageUnitsMils:           
   case pageUnitsHpPlotter:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      break;
   }
   return decimals;
}

/////////////////////////////////////////////////////////////////////////////
// CDXFOutSetting
CDXFOutSetting::CDXFOutSetting()
{
   LoadDefaultSettings();
}

void CDXFOutSetting::LoadDefaultSettings()
{
   m_uselayerindex = FALSE;
   m_splitpolylines = FALSE;
   m_splitwidth = 0.;
   m_mintooldiam = 0.;
   m_toollayer = "TOOL";
   m_toolcodelayer = "TOOLCODE";
   m_handle = FALSE;
   m_dxfHeader.VersionNumber = "AC1009";
}

int CDXFOutSetting::load_DXFsettings(const CString FileName, FormatStruct *format)
{
   CInFile inFile;
   if(!inFile.open(FileName))
      return FALSE;

   LoadDefaultSettings();
   LoadFormatSettings(format);

   while (inFile.getNextCommandLine())
   {
      if(inFile.isCommand(DXFOUTCMD_USELAYERINDEX    ,2))           inFile.parseYesNoParam(1,m_uselayerindex,false);
      else if(inFile.isCommand(DXFOUTCMD_HANDLE, 2))                inFile.parseYesNoParam(1,m_handle,false);
      else if(inFile.isCommand(DXFOUTCMD_POLYCRACK    ,2))          inFile.parseYesNoParam(1,m_splitpolylines,false);
      else if(inFile.isCommand(DXFOUTCMD_POLYCRACKWIDTH    ,2))     m_splitwidth = inFile.getDoubleParam(1); 
      else if(inFile.isCommand(DXFOUTCMD_MINUMUMTOOLDIAM    ,2))    m_mintooldiam = inFile.getDoubleParam(1); 
      else if(inFile.isCommand(DXFOUTCMD_TOOLLAYER    ,2))          m_toollayer = inFile.getParam(1).MakeUpper(); 
      else if(inFile.isCommand(DXFOUTCMD_TOOLCODELAYER    ,2))      m_toolcodelayer = inFile.getParam(1).MakeUpper(); 
      else if(inFile.isCommand(DXFOUTCMD_ACADVER    ,2))            m_dxfHeader.VersionNumber = inFile.getParam(1); 
      else
      {
         fprintf(flog, "Unrecognized command line encountered, '%s', while reading '%s'\n",
            inFile.getCommandLineString(),inFile.getFilePath());
      }

   }

   inFile.close();

   return TRUE;
}

void CDXFOutSetting::LoadFormatSettings(FormatStruct *format)
{
   m_format_line_ends = format->AddLineEnds;
   m_format_RealWidth = format->RealWidth;
   m_format_ExportHiddenEntities = format->exporthiddenentities;
   m_format_UseProbeType = format->useprobetype;
   m_format_explodeStyle = format->explodeStyle;
   m_format_TextXscale = format->DXF.XscaleText;
   m_format_UnMirrorText = format->DXF.UnMirrorText;
}

/*****************************************************************************/
/* 
   This is to get layerindex numbers.
*/
const char *Get_DXFLayer_Layername(int layerNum)
{
/*#ifdef _DEBUG
   LayerStruct *ll = doc->LayerArray[layerNum];
#endif*/

   static CString tmp;
   if (dxfoutSettings.getUserLayerIndex())
   {
      tmp.Format("%d", layerNum);
      return tmp.GetBuffer(0);
   }
   else
   {
      if (doc->getLayerArray()[layerNum]->isFloating())
         return "0";
      return Get_Layer_Layername(layerNum);
   }

   return NULL;
}

/*****************************************************************************/
/* 
   apertures will not be mirrored, they do not have layers
*/
static int write_apertures(int explodeStyle, double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      CString   tmp;
      tmp = block->getName();

      if (strlen(tmp) == 0)   continue;   // block can not be referenced.

      // create necessary aperture blocks
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
        {
         if (block->getShape() == T_COMPLEX)
         {
            int dcode = block->getDcode();

            block = doc->Find_Block_by_Num((int)(block->getSizeA()));
            if (!block) continue;
            if (block->getDataList().IsEmpty()) continue;

            wdxf_Graph_Block_On(check_name('g',tmp), 0);
            DXF_WriteData(explodeStyle, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, -1);
            wdxf_Graph_Block_Off();
         }
         else
         {
            wdxf_Graph_Block_On(check_name('g', tmp), 0);
            wdxf_Graph_Level("0");
            wdxf_Graph_Aperture(block->getShape(), 0.0, 0.0, block->getSizeA()*scale, block->getSizeB()*scale, 0.0, 
               block->getXoffset()*scale, block->getYoffset()*scale);
            wdxf_Graph_Block_Off();
         }
      }
   }
   return 1;
}

//--------------------------------------------------------------
static void write_attributes_def(int explodeStyle, CAttributes* map,
                    double ix, double iy, double rotation, int mirror, double scale)
{
   int   mirror_start = mirror;
   if (map == NULL)
      return;

   double   TEXTRATIO = 0.75;

   CString  dxf_line;
   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      //if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      //if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      int      flg = 0; // visible
      double   height, angle;

      mirror = mirror_start;

      if (!attrib->isVisible())
         flg = 1;

      CString dxf_line;
      dxf_line = get_attvalue_string(doc, attrib);   // can have multiple args

      height = attrib->getHeight() * scale;

      if (mirror)
         angle = rotation - attrib->getRotationRadians();
      else
         angle = rotation + attrib->getRotationRadians();

      // ix, iy
      Point2   point2, off2;
      Mat2x2   m, m1;
      off2.x = 0, off2.y = 0;

      RotMat2(&m, rotation);

      point2.x = attrib->getX()*scale;

      if (mirror)
      {
         point2.x = -attrib->getX()*scale;
      }

      point2.y = attrib->getY()*scale;

      if (attrib->getMirrorDisabled()) 
      {
         if (mirror) // if never mirror, text start offset must be moved in -x
         {
            off2.x = -(strlen(dxf_line) * height*TEXTRATIO);
         }
      }

      RotMat2(&m1, angle);
      TransPoint2(&point2, 1, &m, ix, iy);
      TransPoint2(&off2, 1, &m1, 0.0, 0.0);
      
      const char *layname = "0";

      if (doc->get_layer_visible(attrib->getLayerIndex(), mirror))
         layname = Get_DXFLayer_Layername(doc->get_layer_mirror(attrib->getLayerIndex(), mirror));
      else
         flg = 1; // not visible

      wdxf_Graph_Level(layname);

      if (attrib->getMirrorDisabled()) mirror = FALSE;

      if (explodeStyle == EXPLODE_ALL)
      {
         // here do text
         if (flg == 0)  // this is visible
            wdxf_Graph_Text( dxf_line,  point2.x + off2.x, point2.y + off2.y, height, height*TEXTRATIO, angle, mirror, 0, attrib->getVerticalPosition(),  attrib->getHorizontalPosition());
      }
      else
      {
         wdxf_Graph_Attdef( doc->getKeyWordArray()[keyword]->out, 
            doc->getKeyWordArray()[keyword]->out, dxf_line, flg, point2.x + off2.x, point2.y + off2.y, height, angle );
      }
   }

   return;
}

/*****************************************************************************/
/*
*/
static int write_geometries(int explodeStyle, double scale, int mirror)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (block->getFlags() & BL_WIDTH)
         continue;

      // create necessary aperture blocks
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         continue;

      if (!(block->getFlags() & BL_FILE) || 
           (block->getFlags() & BL_FILE && doc->is_file_inserted(block->getBlockNumber()))) // fileblock in fileblock is now allowed.
      {
         CString  bname;

         if (mirror)
            bname.Format("$M_%s", block->getName());
         else
            bname.Format("%s", block->getName());

         int attr = 0;

         if (explodeStyle == EXPLODE_ROOT)
         {
            if (block->getAttributesRef())   attr = 1;
         }

         wdxf_Graph_Block_On(check_name('g',bname), attr);
         DXF_WriteData(explodeStyle, &(block->getDataList()), 0.0, 0.0, 0.0, mirror, scale, -1);

         if (attr)
         {
            write_attributes_def(explodeStyle, block->getAttributesRef(), 0.0, 0.0,
                        0.0, mirror, scale );
         }

         wdxf_Graph_Block_Off();
      }
   }
   return 1;
}

//--------------------------------------------------------------
void DXF_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, double scale)
{
   FILE        *wfp;
   FileStruct  *file;

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

   CString logFile = GetLogfilePath("DXF.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   
   CString settingsFile( getApp().getExportSettingsFilePath("DXF.out") );
   dxfoutSettings.load_DXFsettings(settingsFile,format);

   CString checkFile( getApp().getSystemSettingsFilePath("dxf.chk") );
   check_init(checkFile);

   int output_units_accuracy = DxfGetDecimals(intToPageUnitsTag(format->DXF.Units)); 

   wdxf_Graph_Init(0, output_units_accuracy, dxfoutSettings.getFormatRealWidth(),dxfoutSettings.getHandle(), 
      dxfoutSettings.getFormatTextXscale(), dxfoutSettings.getFormatUnMirrorText());
   // here we need to do decimal points, realwidth
   // from what width on lines and arcs and circles
   // are drawn with a stroke or width
   wdxf_Graph_File_Open(wfp);

   wdxf_Graph_Line_End(dxfoutSettings.getFormatLineEnds());

   // if EXPLODEALL,
   //   for each file:
   //     write entities of block (including any embedded inserts)
   if (dxfoutSettings.getFormatExplodeStyle() == EXPLODE_ALL)
   {
      POSITION pos;

      // write file blocks as exploded
      pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);
         if (!file->isShown())  continue;

         int mirror = 0;
         if (file->isMirrored()) mirror = 3;

         if (file->isMirrored()) mirror = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic
         DXF_WriteData(dxfoutSettings.getFormatExplodeStyle(), &(file->getBlock()->getDataList()), 
                file->getInsertX() * scale, file->getInsertY() * scale,
                file->getRotation(), mirror, 
                file->getScale() * scale, -1);
      }
   }
   else
   if (dxfoutSettings.getFormatExplodeStyle() == EXPLODE_ROOT)
   {
      int cnt = 0;
      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);
         if (!file->isShown())     continue;   // do not write files, which are not shows.
         cnt++;
      }

      if (cnt != 1)
      {
         CString  tmp;
         tmp.Format("Only 1 visible supported in \"KEEP GEOMETRY MODE\". There are %d file found.", cnt);
         MessageBox(NULL, tmp,"Error DXF Output", MB_OK | MB_ICONHAND);
      }
      else
      {
         write_apertures(dxfoutSettings.getFormatExplodeStyle(), scale);

         // write blocks non mirrored
         write_geometries(dxfoutSettings.getFormatExplodeStyle(), scale, 0);
         write_geometries(dxfoutSettings.getFormatExplodeStyle(), scale, MIRROR_LAYERS); // layer and grasphic

         // write file blocks as exploded
         pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);

            if (!file->isShown())     continue;   // do not write files, which are not shows.

            // only 1 visible files should be shown.
            //if (doc->is_file_inserted(file->getBlock()->num)) continue; // already done in block section

            int mirror = 0;
            if (file->isMirrored()) mirror = MIRROR_FLIP | MIRROR_LAYERS; // mirror flip and graphic
            DXF_WriteData(dxfoutSettings.getFormatExplodeStyle(), &(file->getBlock()->getDataList()), file->getInsertX() * scale,
               file->getInsertY() * scale,
               file->getRotation(), 
               mirror,
               file->getScale() * scale, -1);
            break;
         }
      }  // cnt visible files !
   }
   wdxf_Graph_File_Close(flog, &display_error,dxfoutSettings.getDXFHeader());

   // close write file
   fclose(wfp);

   if (check_report(flog)) display_error++;
   check_deinit();

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return ;
}

//--------------------------------------------------------------
static void write_attributes_ins(int explodeStyle, CAttributes* map,
                    double ix, double iy, double rotation, int mirror, double scale)
{
   int   mirror_start = mirror;
   if (map == NULL)
      return;

   double   TEXTRATIO = 0.75;

   CString  dxf_line;
   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      //if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      //if (keyword != doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      //if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      int      flg = 0; // 1 is not visible
      double   height, angle;

      mirror = mirror_start;

      if (!attrib->isVisible())
         flg = 1;

      CString dxf_line;
      dxf_line = get_attvalue_string(doc, attrib);   // can have multiple args

      height = attrib->getHeight() * scale;

      if (mirror)
         angle = rotation - attrib->getRotationRadians();
      else
         angle = rotation + attrib->getRotationRadians();

      // ix, iy
      Point2   point2, off2;
      Mat2x2   m, m1;
      off2.x = 0, off2.y = 0;

      RotMat2(&m, rotation);

      point2.x = attrib->getX()*scale;

      if (mirror)
      {
         point2.x = -attrib->getX()*scale;
      }

      point2.y = attrib->getY()*scale;

      if (attrib->getMirrorDisabled()) 
      {
         if (mirror) // if never mirror, text start offset must be moved in -x
         {
            off2.x = -(strlen(dxf_line) * height*TEXTRATIO);
         }
      }

      RotMat2(&m1, angle);
      TransPoint2(&point2, 1, &m, ix, iy);
      TransPoint2(&off2, 1, &m1, 0.0, 0.0);
      
      const char *layname = "0";

      if (doc->get_layer_visible(attrib->getLayerIndex(), mirror))
         layname = Get_DXFLayer_Layername(doc->get_layer_mirror(attrib->getLayerIndex(), mirror));
      else
         flg = 1; // not visible
         
      wdxf_Graph_Level(layname);

      if (attrib->getMirrorDisabled()) mirror = FALSE;

      if (explodeStyle == EXPLODE_ALL)
      {
         // here do text
         if (flg & 1)
            wdxf_Graph_Text( dxf_line,  point2.x + off2.x, point2.y + off2.y, height, height*TEXTRATIO, angle, mirror, 0, attrib->getVerticalPosition(),  attrib->getHorizontalPosition());
      }
      else
      {
         wdxf_Graph_Attins( dxf_line, doc->getKeyWordArray()[keyword]->out, flg, point2.x + off2.x, point2.y + off2.y, 
            height, angle, mirror );
      }
   }

   return;
}

/*****************************************************************************/
/*
*/
static int  DXF_DrawUndefinedTool(double x, double y, double toolradius, int tcode, CString extLayername)
{

   if (strlen(dxfoutSettings.getToolLayer()))
   {
      // layer 
      wdxf_Graph_Level(dxfoutSettings.getToolLayer() + extLayername);
      // circle
      wdxf_Graph_Circle(x, y,toolradius );
   }

   if (strlen(dxfoutSettings.getToolCodeLayer()))
   {
      CString  t;

      t.Format("T%02d",tcode);

      // text height is radis, to that text fits inside circle
      double height = toolradius;
      y = y - height/2;
      x = x - height*0.7*strlen(t)/2;  // width is 0.7 and move over half length
      
      wdxf_Graph_Level(dxfoutSettings.getToolCodeLayer()+ extLayername);
      wdxf_Graph_Text( t, x, y, height, height*0.7, 0.0, 0, 0 );
   }

   return 0;
}

static CString GetExtToolLayerName(CString blockname, int mirror)
{
   CString   pcbside = "";
   CString   probesize = "";

   char *pcbsideTable[] = {"TOP","BOT"};
   char *probesizeTable[] = {"50","75","100"};

   if(dxfoutSettings.getFormatUseProbeType())
   {
      blockname.MakeUpper();
      //Find PCB Side
      for(int i = 0; (i < sizeof(pcbsideTable)/sizeof(char*)) && pcbside.IsEmpty(); i++)
         pcbside = (blockname.Find(pcbsideTable[i]) > -1)?pcbsideTable[i]:"";
      
      //Find Probe Size
      for(int i = 0; (i < sizeof(probesizeTable)/sizeof(char*)) && probesize.IsEmpty(); i++)
         probesize = (blockname.Find(probesizeTable[i]) > -1)?probesizeTable[i]:"";
   }

   if(pcbside.IsEmpty())
      pcbside = (mirror)?pcbsideTable[1]:pcbsideTable[0];

   return (probesize+"_"+pcbside);
}

//--------------------------------------------------------------
void DXF_WriteData(int explodeStyle, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror, 
                    double scale,
                    int insertLayer)
{
   Mat2x2   m;
   Point2   point2;
   DataStruct *np;
   POSITION pos;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (!dxfoutSettings.getFormatExportHiddenEntities()) // do not write hidden elements.
      {
         if (np->isHidden())  continue;
      }

      if (np->getDataType() != T_INSERT)
      {
         // insert has not always a layer definition.
         int curLayerIndex = np->getLayerIndex();
         // if data layer is a floating layer, inherit the parent's layer
         if (doc->IsFloatingLayer(curLayerIndex))
            curLayerIndex = insertLayer;

         if(!doc->IsFloatingLayer(curLayerIndex))
         {
            if (!doc->get_layer_visible(curLayerIndex, mirror))
               continue;
            const char *layname = Get_DXFLayer_Layername(doc->get_layer_mirror(curLayerIndex, mirror));
            wdxf_Graph_Level(layname);
         }
         else
            wdxf_Graph_Level("0");
         
      }  

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            double w;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               BOOL polyvoid   = poly->isVoid();
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
            
               // if it is a circle
               w = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();

               double   cx, cy, radius;

               if (polyFilled)
               {
                  fprintf(flog,"DXF output does not support Filled Polygons.\n");
                  display_error++;
               }

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  Point2   p;

                  p.x = cx * scale;
                  if (mirror & MIRROR_FLIP) p.x = -p.x;
                  p.y = cy * scale;
                  p.bulge = 0.0;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);

                  radius *=scale;

                  if (polyFilled && !polyvoid)
                  {
                     wdxf_Graph_Aperture( T_ROUND, p.x, p.y, radius*2, 0.0, 0.0, 0.0, 0.0);
                  }
                  else
                  {
                     wdxf_Graph_Line_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);   
                     wdxf_Graph_Circle(p.x, p.y, radius );
                  }
                  continue;
               }

               // zero filled line of closed.
               if (polyFilled)
               {
                  wdxf_Graph_Line_Width(0.0);
                  w = 0.0;
                  polyFilled = FALSE;
               }
               else
               {
                  wdxf_Graph_Line_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);
                  w = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();

                  if (dxfoutSettings.getFormatLineEnds())
                  {
                     if (doc->getWidthTable()[poly->getWidthIndex()]->getShape() == T_RECTANGLE)
                     {
                        if (doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() == doc->getWidthTable()[poly->getWidthIndex()]->getSizeB())
                           wdxf_Graph_Line_End(2);
                        else
                        {
                           // rectangle lineend 
                           fprintf(flog, "Rectangle Lineend not supported\n");
                           display_error++;
                        }
                     }
                     else
                     if (doc->getWidthTable()[poly->getWidthIndex()]->getShape() == T_SQUARE)
                        wdxf_Graph_Line_End(2);
                     else
                        wdxf_Graph_Line_End(1);
                  }
               }              

#ifdef _VECTORFILL
               if (polyFilled)//will take back
               {
                  //fprintf(flog,"POLYFILL not yet implemented !\n");
                  //display_error++;
                  
                  //--------for polyFilled(fawn)-----------------
                  CPolyList *result;
                  CPoly *filledPoly;
                  result = VectorFillPoly(doc, np->getPolyList(), poly->getWidthIndex(), w/2);
                  int   split = dxfoutSettings.getsplitPolylines();

                  // make sure it has more than 2 vertex, and closed
                  int cnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     cnt++;
                  }
                  if (cnt < 3 || !closed) break;

                  if (w <= split_width)   split = FALSE;

                  // here make 2 vertex polylines, because on odd angles the 
                  // polyline makes funny spikes. 
                  if (dxfoutSettings.getFormatLineEnds() && split)
                  {
                     polyPos = result->GetHeadPosition();
                     while (polyPos != NULL)
                     {
                        filledPoly = result->GetNext(polyPos);
                        int first = TRUE;
                        Point2    firstp;
                        Point2   p;
                        pntPos = filledPoly->list.GetHeadPosition();
                        while (pntPos != NULL)
                        {
                           pnt = filledPoly->list.GetNext(pntPos);
                           p.x = pnt->x * scale;
                           if (mirror) p.x = -p.x;
                           p.y = pnt->y * scale;
                           p.bulge = pnt->bulge;
                           if (mirror) p.bulge = -p.bulge;
                           //TransPoint2(&p, 1, &m, insert_x, insert_y);
                           // here deal with bulge
                           if (first)
                           {
                              first = FALSE;
                              firstp = p;
                           }
                           else
                           {
                              wdxf_Graph_Polyline(firstp, 0);
                              wdxf_Graph_Vertex(p);
                              wdxf_Graph_PolyEnd();
                              first = TRUE;
                           }
                        }
                        // force close
                        if (firstp.x != p.x || firstp.y != p.y)
                           wdxf_Graph_Vertex(firstp);
                        wdxf_Graph_PolyEnd();
                     }
                  }
                  else
                  {
                     polyPos = result->GetHeadPosition();
                     while (polyPos != NULL)
                     {
                        filledPoly = result->GetNext(polyPos);
                        int first = TRUE;
                        Point2    firstp;
                        Point2   p;
                        pntPos = filledPoly->list.GetHeadPosition();
                        while (pntPos != NULL)
                        {
                           pnt = filledPoly->list.GetNext(pntPos);
                           p.x = pnt->x * scale;
                           if (mirror) p.x = -p.x;
                           p.y = pnt->y * scale;
                           p.bulge = pnt->bulge;
                           if (mirror) p.bulge = -p.bulge;

                           //TransPoint2(&p, 1, &m, insert_x, insert_y);
                           // here deal with bulge
                           if (first)
                           {
                              wdxf_Graph_Polyline(p, closed);
                              first = FALSE;
                              firstp = p;
                           }
                           else
                              wdxf_Graph_Vertex(p);
                        }
                        // force close
                        if (firstp.x != p.x || firstp.y != p.y)
                           wdxf_Graph_Vertex(firstp);
                        wdxf_Graph_PolyEnd();
                     }
                  }
                  Free_Polylist(result);
                  //--------end of for polyFilled(fawn)-----------
               }
#endif 
               int first = TRUE;
               Point2    firstp;
               Point2   p;
               int   split = dxfoutSettings.getSplitPolylines();

               // make sure it has more than 1 vertex, because otherwise Autocad chockes.
               int cnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
               if (cnt < 2)   continue;

               if (w <= dxfoutSettings.getSplitWidth())   split = FALSE;

               // here make 2 vertex polylines, because on odd angles the 
               // polyline makes funny spikes. 
               if (dxfoutSettings.getFormatLineEnds() && split)
               {
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror & MIRROR_FLIP) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     if (mirror & MIRROR_FLIP)  p.bulge = -p.bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (first)
                     {
                        first = FALSE;
                        firstp = p;
                     }
                     else
                     {
                        wdxf_Graph_Polyline(firstp, 0);
                        wdxf_Graph_Vertex(p);
                        wdxf_Graph_PolyEnd();
                        firstp = p;
                     }
                  }
               }
               else
               {
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror & MIRROR_FLIP) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     if (mirror & MIRROR_FLIP)  p.bulge = -p.bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (first)
                     {
                        wdxf_Graph_Polyline(p, closed);
                        first = FALSE;
                        firstp = p;
                     }
                     else
                        wdxf_Graph_Vertex(p);
                  }
                  // force close
                  if (polyFilled && (firstp.x != p.x || firstp.y != p.y))
                     wdxf_Graph_Vertex(firstp);
                  wdxf_Graph_PolyEnd();
               }
            }  //while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
				int textLines = 0;
            if ((textLines = np->getText()->getNumLines()) > 1)
            {
				   // Get the upper left extext X and Y
				   CPoint2d ULExtent = np->getText()->getTextBox(doc->getSettings().getTextSpaceRatio()).getUL();
				   Point2 ULPoint;

				   if (np->getText()->getHorizontalPosition() == horizontalPositionLeft)			
					   ULPoint.x = np->getText()->getPnt().x;							// left
				   else if (np->getText()->getHorizontalPosition() == horizontalPositionCenter)		
					   ULPoint.x = (np->getText()->getPnt().x - ULExtent.x/2);	// center
				   else if (np->getText()->getHorizontalPosition() == horizontalPositionRight)		
					   ULPoint.x = (np->getText()->getPnt().x - ULExtent.x);		// right

				   if (np->getText()->getVerticalPosition() == verticalPositionBaseline)			
					   ULPoint.y = (np->getText()->getPnt().y + ULExtent.y);		// bottom
				   else if (np->getText()->getVerticalPosition() == verticalPositionCenter)		
					   ULPoint.y = (np->getText()->getPnt().y + ULExtent.y/2);	// center
				   else if (np->getText()->getVerticalPosition() == verticalPositionTop)		
					   ULPoint.y = np->getText()->getPnt().y;							// top

				   int curPos = 0;
				   int lineCount = 0;
				   CString textString = np->getText()->getText();
				   CString text = textString.Tokenize("\n", curPos);
				   while (!text.IsEmpty())
				   {
                  Mat2x2   shiftm;
                  RotMat2(&shiftm, np->getText()->getRotation());

                  // rotate the dx and dy
                  Point2 deltapos;
                  deltapos.x = 0.;
                  deltapos.y = np->getText()->getHeight() * lineCount;
                  TransPoint2(&deltapos, 1, &shiftm, 0, 0);

                  // add dx, dy to text location
					   point2.x = (ULPoint.x - deltapos.x)  * scale;
					   point2.y = (ULPoint.y - (deltapos.y + np->getText()->getHeight()))  * scale;
					   TransPoint2(&point2, 1, &m, insert_x, insert_y);

						double text_rot;
						if (mirror & MIRROR_FLIP)
						{
							text_rot = rotation - np->getText()->getRotation();
							point2.x = -point2.x;
						}
						else
						{
							text_rot = rotation + np->getText()->getRotation();
						}

						int text_mirror = np->getText()->getResultantMirror(mirror);
						if (np->getText()->getMirrorDisabled())
							text_mirror = 0;

						wdxf_Graph_Text(text, point2.x, point2.y, np->getText()->getHeight()*scale, np->getText()->getWidth()*scale,
											text_rot, text_mirror, np->getText()->getOblique(), 
                                 np->getText()->getVerticalPosition(),  np->getText()->getHorizontalPosition());

						// Get next text
					   text = textString.Tokenize("\n", curPos).Trim();
					   lineCount++;
				   }
            }
            else
            {
					point2.x = (np->getText()->getPnt().x)*scale;
					point2.y = (np->getText()->getPnt().y)*scale;

					double text_rot;
					if (mirror & MIRROR_FLIP)
					{
						text_rot = rotation - np->getText()->getRotation();
						point2.x = -point2.x;
					}
					else
					{
						text_rot = rotation + np->getText()->getRotation();
					}

					TransPoint2(&point2, 1, &m, insert_x, insert_y);

					int text_mirror = np->getText()->getResultantMirror(mirror);
					if (np->getText()->getMirrorDisabled())
						text_mirror = 0;

					wdxf_Graph_Text(np->getText()->getText(), point2.x, point2.y, np->getText()->getHeight()*scale,
										np->getText()->getWidth()*scale, text_rot, text_mirror, np->getText()->getOblique(),
                              np->getText()->getVerticalPosition(),  np->getText()->getHorizontalPosition());
            }
         } 
         break;
         
         case T_INSERT:
         {

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot;

            if (mirror & MIRROR_FLIP)
               block_rot = rotation - np->getInsert()->getAngle();
            else
               block_rot = rotation + np->getInsert()->getAngle();

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror & MIRROR_FLIP) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            BOOL  DrawAp = FALSE;
            BOOL  Drawblock = FALSE;
            BOOL  IsTool = (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL);
            BOOL  IsAp = (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE);

            if (IsTool)
            {
               if (block->getToolDisplay()) // use another block's graphics to display this tool
               {
                  block = doc->Find_Block_by_Num(block->getToolBlockNumber());

                  if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE))
                     DrawAp = TRUE;
                  else
                     Drawblock = TRUE;
               }
               else // draw generic tool
               {
                  // check if drill layer is visible
                  if (!doc->get_layer_visible(np->getLayerIndex(), mirror))
                     continue;

                  double toolradius;
                  if (block->getToolSize() > SMALLNUMBER)
                  {
                     toolradius = block->getToolSize() / 2 * scale;
                  }
                  
                  if (toolradius < dxfoutSettings.getMinToolDiam()/2)
                     toolradius = dxfoutSettings.getMinToolDiam()/2;
                                    
                  CString extLayername = GetExtToolLayerName(block->getName(), block_mirror);                  

                  // here check for minumum tool size
                  DXF_DrawUndefinedTool(point2.x, point2.y, toolradius, block->getTcode(), extLayername);
               }
            } // end TOOL

            // if inserting an aperture
            if (IsAp || DrawAp)
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               block_rot = block_rot + block->getRotation();  // aperture rotation offset

               if ( (explodeStyle == NO_EXPLODE) || (explodeStyle == EXPLODE_ROOT) ) // write insert entity
               {
                  if (!doc->get_layer_visible(layer, block_mirror))
                     continue;

                  const char *w = Get_DXFLayer_Layername(doc->get_layer_mirror(layer, block_mirror)); // layer must be there. 
                  if (w)
                  {
                     int attflg = 0;
                     wdxf_Graph_Block_Reference_Layer(w, check_name('g', block->getName()),
                              point2.x,
                              point2.y,
                              block_rot, block_mirror,
                              np->getInsert()->getScale(), attflg);   // do not scale the aperture, because it is already done
                  }
               }

               if (explodeStyle == EXPLODE_ALL) // then call DXF_WriteData() recursively to write embedded entities
               {
                  if (!doc->get_layer_visible(layer, block_mirror))
                     continue;

                  wdxf_Graph_Level(Get_DXFLayer_Layername(doc->get_layer_mirror(layer, block_mirror)));

                  if (block->getShape() == T_COMPLEX)
                  {
                     int   bnum = (int)(block->getSizeA());
                     BlockStruct *subblock = doc->Find_Block_by_Num(bnum);

                     DXF_WriteData(explodeStyle, &(subblock->getDataList()),
                           point2.x, point2.y,
                           block_rot, block_mirror, scale * np->getInsert()->getScale(),
                           np->getLayerIndex());
                  }
                  else
                  {
                     wdxf_Graph_Aperture(block->getShape(),
                        point2.x,
                        point2.y,
                        block->getSizeA()*scale, block->getSizeB()*scale, block_rot,
                        block->getXoffset()*scale, block->getYoffset()*scale);
                  }
               }
            }

            if ((!IsTool && !IsAp) || Drawblock)
            {
               if ( (explodeStyle == NO_EXPLODE) || (explodeStyle == EXPLODE_ROOT) ) // write insert entity
               {
                  CString  bname;
                  if (block_mirror)
                     bname.Format("$M_%s", block->getName());
                  else
                     bname.Format("%s", block->getName());

                  int attflg = 0;
                  CString  refname;
                  refname = np->getInsert()->getRefname();

                  if (np->getAttributesRef() || strlen(refname))  attflg = 1;
                  wdxf_Graph_Block_Reference(check_name('g', bname),
                               point2.x,
                               point2.y,
                               block_rot, block_mirror,
                               np->getInsert()->getScale(), attflg);  // do not scale here

                  if (attflg)
                  {
                     // if there is no REFNAME attribute, make on invisible
                     Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_REFNAME, 2);
                     if (!a)
                     {
                        // not visible
                        wdxf_Graph_Attins( refname, ATT_REFNAME, 1, point2.x, point2.y, 0.0, 0.0, 0 );
                     }
                     write_attributes_ins(explodeStyle, np->getAttributesRef(), point2.x, point2.y,
                           block_rot, block_mirror, scale * np->getInsert()->getScale());
                     wdxf_Graph_SeqEnd();
                  }
               }
               else
               if (explodeStyle == EXPLODE_ALL) // then call DXF_WriteData() recursively to write embedded entities
               {
                  DXF_WriteData(explodeStyle, &(block->getDataList()),
                        point2.x, point2.y,
                        block_rot, block_mirror, scale * np->getInsert()->getScale(),
                        np->getLayerIndex());

                  // here write out attribute
                  write_attributes_def(explodeStyle, np->getAttributesRef(),point2.x, point2.y,
                        block_rot, block_mirror, scale * np->getInsert()->getScale());

               } // end if EXPLODE_ALL
            } // end else not aperture

         } // end case INSERT:
         break;
      } // end switch
   } // end for
   return;
} // end DXF_WriteData 

// end G_DXFOUT.CPP
