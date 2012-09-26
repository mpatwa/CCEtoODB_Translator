// $Header: /CAMCAD/4.6/read_wrt/Ccm_out.cpp 56    4/03/07 5:49p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "graph.h"
#include <math.h>
#include "outline.h"
#include "polylib.h"
#include "centroid.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg			*progress;
//extern LayerTypeInfoStruct	layertypes[];

#define  SMALL_DELTA    0.001    // Smallest unit

static CCEtoODBDoc    *doc;
static double        unitsFactor;

static int           display_error;
static FILE          *flog;

static FileStruct    *cur_file;

static   CString     ident;
static   long        linecnt = 0;

typedef CArray<Point2, Point2&> CPolyArray;
static   CPolyArray  polyarray;
static   int         polycnt;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static   int         SHOWOUTLINE = FALSE; // set in CCM.out
static   int         WRITE_TRACES = FALSE;
static   CString     ccmprocess;

static int CCM_WriteOutlineData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, 
      long graphic_class, int layer_class);
static void CCM_WritePANELMiscData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale,
      int embeddedLevel, int insertLayer);
static char *clean_text(const char *t);
static void do_accesspoints(FILE *fp, FileStruct *file);
static void do_components(FILE *fp, FileStruct *file);
static void do_layerlist(FILE *fp);
static void do_padstacks(FILE *fp, FileStruct *file);
static void do_panelfiles(FILE *fp, FileStruct *file);
static void do_probepoints(FILE *fp, FileStruct *file);
static void do_shapes(FILE *fp, FileStruct *file);
static void do_signals(FILE *fp, FileStruct *file,CNetList *NetList);
static void load_CCMsettings(const CString fname);
static void plusident();
static void minusident();
static int write_easyoutline(FILE *wfp, CPntList *pntList, const char *graph_type);
static CPntList *CCM_CanJoinOutline(CDataList *DataList, long graphic_class, int layer_class);
static DataStruct *CCM_FindOutlineData(CDataList *DataList, long graphic_class, int layer_class);
static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance);
static void AddPntsToPntList(CPntList *pntList, DataStruct *data, BOOL tail, BOOL reversed);
static void CCM_WritePANELFileFiducials(FILE *wfp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
static int highest_stacknum;


/******************************************************************************
* CCM_WriteFiles
*/
void CCM_WriteFiles(const char *filename, CCEtoODBDoc *Doc,FormatStruct *format, int page_units)
{
   doc = Doc;
   unitsFactor = 1.0;
   display_error = FALSE;
   ident = "";
   linecnt = 0;
   SHOWOUTLINE = FALSE;
   ComponentSMDrule = 0;

   int pcb_found = FALSE;
   double accuracy  = get_accuracy(doc);

   CString logFile = GetLogfilePath("ccm.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   
   CString settingsFile( getApp().getExportSettingsFilePath("CCM.out") );
   load_CCMsettings(settingsFile);

   switch_on_pcbfiles_used_in_panels(doc, TRUE);   // set show

   // open file for writting
   FILE *wfp;
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %-70s",filename);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }

   polyarray.SetSize(100,100);
   polycnt = 0;

   fprintf(wfp, "# CAMCAD Intermediate CAM File %s\n", filename);
   fprintf(wfp, "%s{Mentor Graphics CCM File}\n", ident);
   fprintf(wfp, "%s{CCMVersion 2.1}\n", ident);

   fprintf(wfp, "%s{job \"%s\"\n", ident, clean_text(filename));
   linecnt++;
   plusident();

   // units
   switch (page_units)
   {
   case UNIT_INCHES:
      fprintf(wfp, "%s{units \"INCHES\"}\n", ident);
      break;
   case UNIT_MILS:
      fprintf(wfp, "%s{units \"MILS\"}\n", ident);
      break;
   case UNIT_MM:
      fprintf(wfp, "%s{units \"MM\"}\n", ident);
      break;
   default:
      MessageBox(NULL, "Error !", "Page Units must be [inches, mils, mm]", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   update_smdrule_geometries(doc, ComponentSMDrule);

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = cur_file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;
   
      if (file->getBlockType() == BLOCKTYPE_PANEL)   
      {
         fprintf(wfp, "%s{panel\n", ident);
         plusident();

         fprintf(wfp, "%s{panel outline\n", ident);
         plusident();

         DataStruct *outlineData = CCM_FindOutlineData(&file->getBlock()->getDataList(), GR_CLASS_PANELOUTLINE, LAYTYPE_PANEL_OUTLINE);
         if (outlineData)
         {
            CPoly *outlinePoly = (CPoly*)outlineData->getPolyList()->GetHead();
            if (!write_easyoutline(wfp, &outlinePoly->getPntList(), "panel outline"))
            {
               fprintf(flog, "Error in easy outline for Panel Outline\n");
               display_error++;
            }
         }
         else
         {
            CPntList *joinedPntList = CCM_CanJoinOutline(&file->getBlock()->getDataList(), GR_CLASS_PANELOUTLINE, LAYTYPE_PANEL_OUTLINE);
            if (joinedPntList)
            {
               if (!write_easyoutline(wfp, joinedPntList, "panel outline"))
               {
                  fprintf(flog, "Error in easy outline for Panel Outline\n");
                  display_error++;
               }
               FreePntList(joinedPntList);
            }
            else
            {
               Outline_Start(doc);

               int outline_found = CCM_WriteOutlineData(&file->getBlock()->getDataList(), 
                     file->getInsertX(), file->getInsertY(), file->getRotation(), file->isMirrored(), file->getScale(), 0, -1,
                     GR_CLASS_PANELOUTLINE, LAYTYPE_PANEL_OUTLINE);

               if (!outline_found)
               {
                  fprintf(flog, "Did not find a PanelOutline.\n");
                  display_error++;
               }
               else
               {
                  int returnCode;
                  if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "panel outline"))
                  {
                     fprintf(flog, "Error in easy outline for Panel Outline\n");
                     display_error++;
                  }
               }

               Outline_FreeResults();
            }
         }

         minusident();
         fprintf(wfp,"%send panel outline}\n",ident);

         do_panelfiles(wfp,file);
         CCM_WritePANELMiscData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1);

         minusident();
         fprintf(wfp,"%send panel}\n",ident);   
      }
      else if (file->getBlockType() == BLOCKTYPE_PCB)   
      {
         double u = unitsFactor;
         pcb_found = TRUE;
   
         update_smdrule_components(doc, file, ComponentSMDrule);
         generate_PINLOC(doc, file, 0);   // this function generates the PINLOC argument for all pins.
         generate_CENTROIDS(doc);

         fprintf(wfp, "%s{file\n",ident);
         plusident();

         fprintf(wfp, "%s{name \"%s\"}\n", ident, clean_text(file->getName()));
         fprintf(wfp, "%s{type \"%s\"}\n", ident, blockTypeToDisplayString(file->getBlockType()));

         BlockStruct *block =  doc->Find_Block_by_Num(file->getBlock()->getToolBlockNumber());
         fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
      
         fprintf(wfp, "# import CAD system, not yet in data structure\n");
         fprintf(wfp, "# attributes under file\n");

         do_layerlist(wfp);

         fprintf(wfp, "%s{board outline\n", ident);
         plusident();

         DataStruct *outlineData = CCM_FindOutlineData(&file->getBlock()->getDataList(), GR_CLASS_BOARDOUTLINE, LAYTYPE_BOARD_OUTLINE);

         if (outlineData)
         {
            CPoly *outlinePoly = (CPoly*)outlineData->getPolyList()->GetHead();
            if (!write_easyoutline(wfp, &outlinePoly->getPntList(), "board outline"))
            {
               fprintf(flog, "Error in easy outline for Board Outline\n");
               display_error++;
            }
         }
         else
         {
            CPntList *joinedPntList = CCM_CanJoinOutline(&file->getBlock()->getDataList(), GR_CLASS_BOARDOUTLINE, LAYTYPE_BOARD_OUTLINE);
            if (joinedPntList)
            {
               if (!write_easyoutline(wfp, joinedPntList, "board outline"))
               {
                  fprintf(flog, "Error in easy outline for Board Outline\n");
                  display_error++;
               }
               FreePntList(joinedPntList);
            }
            else
            {
               Outline_Start(doc);

               int outline_found = CCM_WriteOutlineData(&file->getBlock()->getDataList(), 
                     file->getInsertX(), file->getInsertY(), file->getRotation(), file->isMirrored(), file->getScale(), 0, -1,
                     GR_CLASS_BOARDOUTLINE, LAYTYPE_BOARD_OUTLINE);

               if (!outline_found)
               {
                  outline_found = CCM_WriteOutlineData(&file->getBlock()->getDataList(), 
                        file->getInsertX(), file->getInsertY(), file->getRotation(), file->isMirrored(), file->getScale(), 0, -1,
                        GR_CLASS_BOARD_GEOM, 0);
               }

               if (!outline_found)
               {
                  fprintf(flog, "Did not find a BoardOutline.\n");
                  display_error++;
               }
               else
               {
                  int returnCode;
                  if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "board outline"))
                  {
                     fprintf(flog, "Error in easy outline for Board Outline\n");
                     display_error++;
                  }
               }

               Outline_FreeResults();
            }
         }

         minusident();
         fprintf(wfp,"%send board outline}\n",ident);

         do_padstacks(wfp,file);
         do_shapes(wfp,file);
         do_components(wfp,file);
         do_signals(wfp, file, &file->getNetList());
         do_accesspoints(wfp,file);
         do_probepoints(wfp,file);

         minusident();  // end file
         fprintf(wfp,"%send file}\n",ident); 
         // allows to do more PCB files
      } // if pcb file
   }

   minusident();  // end job
   fprintf(wfp, "%send job}\n",ident); 

   fprintf(wfp, "# end\n");
   fclose(wfp);

   switch_on_pcbfiles_used_in_panels(doc, FALSE);  // set show

   if (!pcb_found)
   {
      MessageBox(NULL, "No PCB file found !", "CCM Output", MB_OK | MB_ICONHAND);
      return;
   }

   fclose(flog);
   polyarray.RemoveAll();

   if (display_error)
      Logreader(logFile);

   if (strlen(ccmprocess))
   {
      // 0  The system is out of memory or resources.
      // ERROR_BAD_FORMAT  The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).
      // ERROR_FILE_NOT_FOUND The specified file was not found.
      // ERROR_PATH_NOT_FOUND The specified path was not found.

      CString p = ccmprocess;
      p += " ";
      p += filename;

      int res = WinExec(p, SW_SHOW);

      switch (res)
      {
      case 0:
         MessageBox(NULL, "The system is out of memory or resources!","CCM Process", MB_OK | MB_ICONHAND);
         break;
      case ERROR_BAD_FORMAT:
         MessageBox(NULL, "The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)!","CCM Process", MB_OK | MB_ICONHAND);
         break;
      case ERROR_FILE_NOT_FOUND:
         {
            CString t;
            t.Format("The specified file [%s] was not found!", ccmprocess);
            MessageBox(NULL, t, "CCM Process", MB_OK | MB_ICONHAND);
         }
         break;
      case ERROR_PATH_NOT_FOUND:
         {
            CString t;
            t.Format("The specified path [%s] was not found!", ccmprocess);
            MessageBox(NULL, t, "CCM Process", MB_OK | MB_ICONHAND);
         }
         break;
      }
   }
}

/******************************************************************************
* plusident
*/
static void plusident()
{
   ident += ' ';
}

/******************************************************************************
* minusident
*/
static void minusident()
{
   if (strlen(ident) > 0)
   {
      CString t = ident.Left(strlen(ident)-1);
      ident = t;
   }
   else
   {
      fprintf(flog, "Ident error at %ld\n", linecnt);      
      display_error++;
   }
}

/******************************************************************************
* clean_text
*/
static char *clean_text(const char *t)
{
   static char tmp[255];
   unsigned int i, tcnt = 0;

   for (i=0; i<strlen(t); i++)
   {
      if (t[i] == '"')
         tmp[tcnt++] = '\\';
      tmp[tcnt++] = t[i];     
   }
   tmp[tcnt] = '\0';
   return tmp;
}

/******************************************************************************
* write_attributes
*/
static void write_attributes(FILE *fp, CAttributes* map)
{
   if (map == NULL)
      return;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
		WORD keyword;
		Attrib* attrib;

      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 1))      continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 1))      continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 1))    continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 1)) continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 1))     continue;

      char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));

      if ((tok = strtok(temp, "\n")) == NULL)
      {
         fprintf(fp, "%s{attribute \"%s\" \"\"}\n", ident, doc->getKeyWordArray()[keyword]->out);   // multiple values are delimited by \n
      }
      else
      {
         while (tok)
         {
            fprintf(fp, "%s{attribute \"%s\" \"%s\"}\n", ident, doc->getKeyWordArray()[keyword]->out,tok);  // multiple values are delimited by \n
            tok = strtok(NULL, "\n");
         }
      }

      free(temp);
   }
}

/******************************************************************************
* write_all_attributes
*/
static void write_all_attributes(FILE *fp, CAttributes* map)
{
   if (map == NULL)
      return;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
		WORD keyword;
		Attrib* attrib;

      map->GetNextAssoc(pos, keyword, attrib);

      char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));

      if ((tok = strtok(temp, "\n")) == NULL)
      {
         fprintf(fp, "%s{attribute \"%s\" \"\"}\n", ident, doc->getKeyWordArray()[keyword]->out);   // multiple values are delimited by \n
      }
      else
      {
         while (tok)
         {
            fprintf(fp, "%s{attribute \"%s\" \"%s\"}\n", ident, doc->getKeyWordArray()[keyword]->out,tok);  // multiple values are delimited by \n
            tok = strtok(NULL, "\n");
         }
      }

      free(temp);
   }
}

/******************************************************************************
* do_typelist
*/
static void do_typelist(FILE *fp, CTypeList *TypeList, const char *type_name)
{
   POSITION typePos = TypeList->GetHeadPosition();
   while (typePos)
   {                                         
      TypeStruct *type = TypeList->GetNext(typePos);
      if (strcmp(type->getName(), type_name))
         continue;

      write_attributes(fp, type->getAttributesRef());
   }
}

/******************************************************************************
* get_ccmlayername
*/
static CString get_ccmlayername(int layerindex)
{
   CString lname;
   LayerStruct *layer = doc->getLayerArray()[layerindex];

   if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)           
      lname = "TOP";
   else if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)  
      lname = "BOTTOM";
   else if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER || 
				layer->getLayerType() == LAYTYPE_SIGNAL ||
				layer->getLayerType() == LAYTYPE_POWERPOS ||
				layer->getLayerType() == LAYTYPE_SPLITPLANE ||
				layer->getLayerType() == LAYTYPE_POWERNEG)
   {
      lname.Format("INNER_%d", layer->getElectricalStackNumber());
   }
   else
   {
      if (layer->getElectricalStackNumber())
         lname.Format("INNER_%d", layer->getElectricalStackNumber());
      else
         lname = layer->getName();
   }

   return lname;
}

/******************************************************************************
* CCM_WriteROUTESData
*/
static void CCM_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
										  double rotation, int mirror, double scale, int embeddedLevel,
										  int insertLayer, const char *netname)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      switch (data->getDataType())
      {
      case T_POLY:
         {
            if (!WRITE_TRACES)
					break;

            Attrib *a =  is_attvalue(doc, data->getAttributesRef(),ATT_NETNAME, 1);
            if (!a)
					break;

            if (a->getValueType() == VT_STRING)
            {
               if (strcmp(get_attvalue_string(doc, a), netname))
                  break;

               CString ccmlayer = get_ccmlayername(data->getLayerIndex());
               if (strlen(ccmlayer) == 0)
                  break;

               int cnt = fprintf(wfp, "%s{polygon %s\n ", ident, ccmlayer);
               plusident();

               POSITION polyPos = data->getPolyList()->GetHeadPosition();
               while (polyPos)
               {  
                  CPoly *poly = data->getPolyList()->GetNext(polyPos);
                  if (poly->isHidden())
                     continue;
                  
                  fprintf(wfp, "%s{poly\n", ident);
                  plusident();

                  if (poly->isThermalLine()) fprintf(wfp, "%s{thermallines}\n", ident);
                  if (poly->isFilled())       fprintf(wfp, "%s{filled}\n", ident);
                  if (poly->isHatchLine())   fprintf(wfp, "%s{hatchlines}\n", ident);
                  if (poly->isVoid())     fprintf(wfp, "%s{voidpoly}\n", ident);
                  if (poly->isClosed())       fprintf(wfp, "%s{closed}\n", ident);

                  double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
                  fprintf(wfp,"%s{width \"%lf\"}\n", ident, lineWidth);

                  int cnt = fprintf(wfp, "%s{polyline ",ident);
                  plusident();

                  POSITION pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntPos);   
                     if (cnt > 70)
                        cnt = fprintf(wfp, "\n%s",ident);
                     cnt +=fprintf(wfp, "\"%lf, %lf, %lf\" ", pnt->x, pnt->y, pnt->bulge);
                  }  
                  fprintf(wfp, "}\n"); // end polyline
                  minusident();

                  fprintf(wfp, "%send poly}\n", ident); // end poly
                  minusident();
                  
               }

               // attributes
               write_attributes(wfp, data->getAttributesRef());

               fprintf(wfp, "%send polygon}\n", ident); // end polygon
               minusident();
            }  // same netname
         }
         break;
      case T_INSERT:
			{
				if (data->getInsert()->getInsertType() == INSERTTYPE_VIA) 
				{
					Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
					if (!attrib)
						break;

					if (attrib->getValueType() == VT_STRING)
               {
                  if (strcmp(get_attvalue_string(doc, attrib), netname))
                     break;

                  Point2 point2;
                  point2.x = data->getInsert()->getOriginX() * scale;
                  if (mirror)
                     point2.x = -point2.x;
                  point2.y = data->getInsert()->getOriginY() * scale;
                  TransPoint2(&point2, 1, &m, insert_x, insert_y);

                  //insert if mirror is either global mirror or block_mirror, but not if both.
                  //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
                  //double block_rot = rotation + np->getInsert()->getAngle();

                  BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

                  fprintf(wfp, "%s{via \"%s\" %lf %lf %d\n", ident, clean_text(block->getName()), point2.x, point2.y, block->getBlockNumber());
                  plusident();
                  fprintf(wfp, "%s{Entity %ld}\n", ident, data->getEntityNumber());

                  // attributes
                  write_attributes(wfp, data->getAttributesRef());

                  minusident();
                  fprintf(wfp, "%send via}\n", ident);
               }
				} 
				else if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPAD) 
				{
					Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
					if (!attrib)
						break;
					
               if (attrib->getValueType() == VT_STRING)
               {
                  //CString l = get_attvalue_string(doc, a);
                  if (strcmp(get_attvalue_string(doc, attrib), netname))
                     break;

                  Point2 point2;
                  point2.x = data->getInsert()->getOriginX() * scale;
                  if (mirror)
                     point2.x = -point2.x;
                  point2.y = data->getInsert()->getOriginY() * scale;
                  TransPoint2(&point2, 1, &m, insert_x, insert_y);

                  // insert if mirror is either global mirror or block_mirror, but not if both.
                  //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
                  //double block_rot = rotation + np->getInsert()->getAngle();

                  BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

                  fprintf(wfp, "%s{testpad \"%s\" %lf %lf %d\n",
                     ident, clean_text(block->getName()),point2.x, point2.y, block->getBlockNumber());
                  plusident();
                  fprintf(wfp, "%s{Entity %ld}\n", ident, data->getEntityNumber());

                  // attributes
                  write_attributes(wfp, data->getAttributesRef());

                  minusident();
                  fprintf(wfp, "%send testpad}\n", ident);
               }
				} 
				else if (data->getInsert()->getInsertType() == INSERTTYPE_FREEPAD) 
				{
					Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
					if (!attrib)
						break;
					
               if (attrib->getValueType() == VT_STRING)
               {
                  if (strcmp(get_attvalue_string(doc, attrib), netname))
                     break;

                  Point2 point2;
                  point2.x = data->getInsert()->getOriginX() * scale;
                  if (mirror)
                     point2.x = -point2.x;
                  point2.y = data->getInsert()->getOriginY() * scale;
                  TransPoint2(&point2, 1, &m, insert_x, insert_y);

                  // insert if mirror is either global mirror or block_mirror, but not if both.
                  //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
                  //double block_rot = rotation + np->getInsert()->getAngle();

                  BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

                  fprintf(wfp, "%s{freepad \"%s\" %lf %lf %d\n", ident, clean_text(block->getName()), point2.x, point2.y, block->getBlockNumber());
                  plusident();
                  fprintf(wfp, "%s{Entity %ld}\n", ident, data->getEntityNumber());

                  // attributes
                  write_attributes(wfp, data->getAttributesRef());

                  minusident();
                  fprintf(wfp, "%send freepad}\n", ident);
               }
            }  
         }
         break;
      } 
   }
}

/******************************************************************************
* do_signals
*/
static void do_signals(FILE *fp, FileStruct *file,CNetList *NetList)
{
   progress->SetStatus("Writing Signals...");
   fprintf(fp, "%s{nets\n", ident);
   plusident();

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);
      if (!net || net->getFlags() & NETFLAG_UNUSEDNET)
			continue;

      fprintf(fp, "%s{name \"%s\"\n", ident, clean_text(net->getNetName()));
      plusident();

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         fprintf(fp, "%s{node \"%s\" ", ident, clean_text(compPin->getRefDes())); 
         fprintf(fp, "\"%s\"\n", clean_text(compPin->getPinName()));

         plusident();
         fprintf(fp, "%s{Entity %ld}\n", ident, compPin->getEntityNumber());

			minusident(); 
         fprintf(fp, "%send node}\n", ident);
      }

      // here write vias 
      // order by signal name
      CCM_WriteROUTESData(fp, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, net->getNetName());

      minusident(); 
      fprintf(fp, "%send name}\n", ident);
   }

   minusident();
   fprintf(fp, "%send nets}\n", ident);

	fprintf(fp, "%s{nonet\n", ident);
   plusident();

   // here do Non connects
   netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);
      if (!net || !(net->getFlags() & NETFLAG_UNUSEDNET))
			continue;

      // SaveAttribs(stream, &net->getAttributesRef());
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         fprintf(fp, "%s{node \"%s\" ", ident, clean_text(compPin->getRefDes())); 
         fprintf(fp, "\"%s\"\n", clean_text(compPin->getPinName()));

         plusident();
         fprintf(fp, "%s{Entity %ld}\n", ident, compPin->getEntityNumber());

         minusident();  // netname
         fprintf(fp, "%send node}\n", ident);
      }
   }
   minusident();
   fprintf(fp, "%send nonet}\n", ident);
}

/******************************************************************************
* CCM_WriteCOMPONENTPinData
*/
static void CCM_WriteCOMPONENTPinData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
                                      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos )
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = np->getInsert()->getMirrorFlags();
      double block_rot = rotation + np->getInsert()->getAngle();

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
			point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x,insert_y);

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      // here pin found.
      CString pinname = np->getInsert()->getRefname();
      Attrib *a;

      if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE )
      {
         fprintf(wfp, "%s{hole\n", ident);
         plusident();
         fprintf(wfp, "%s{type \"DRILL\"}\n", ident);

         if (strlen(pinname))
            fprintf(wfp, "%s{name \"%s\"}\n", ident, pinname);

         double drill = get_drill_from_block_no_scale(doc, block);

         fprintf(wfp, "%s{drill %lf}\n", ident, drill * scale);
         fprintf(wfp, "%s{loc %lf %lf}\n", ident,point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_NONPLATED, 2))
            fprintf(wfp, "%s{plated \"FALSE\"}\n", ident);
         else
            fprintf(wfp, "%s{plated \"TRUE\"}\n", ident);

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_STARTLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%startlayer \"%s\"}\n", l);
         }

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_ENDLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%endlayer \"%s\"}\n", l);
         }
         minusident();
         fprintf(wfp,"%send hole}\n", ident);
      }
      else if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL) 
      {
         int c_bottom = np->getInsert()->getPlacedBottom();
         LayerStruct *layer = doc->FindLayer(np->getLayerIndex());
         if (layer == NULL)
            continue;
         if (layer->getLayerType() < LAYTYPE_SIGNAL_TOP || layer->getLayerType() > LAYTYPE_PAD_THERMAL)
            continue;
         if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT || layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
            c_bottom = 1;

         fprintf(wfp, "%s{fiducial\n", ident);
         plusident();
         fprintf(wfp, "%s{type \"FIDUCIAL\"}\n", ident);

         if (np->getInsert()->getRefname())
            fprintf(wfp, "%s{name \"%s\"}\n", ident, clean_text(np->getInsert()->getRefname()));

         fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
         fprintf(wfp, "%s{loc %lf %lf}\n", ident ,point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
         fprintf(wfp, "%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
         minusident();
         fprintf(wfp, "%send fiducial}\n", ident);

         if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL || layer->getLayerType() == LAYTYPE_PAD_ALL)
         {
            c_bottom = !c_bottom;

            fprintf(wfp, "%s{fiducial\n", ident);
            plusident();
            fprintf(wfp, "%s{type \"FIDUCIAL\"}\n", ident);

            if (np->getInsert()->getRefname())
               fprintf(wfp, "%s{name \"%s\"}\n", ident, clean_text(np->getInsert()->getRefname()));

            fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
            fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
            fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
            fprintf(wfp, "%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
            minusident();
            fprintf(wfp, "%send fiducial}\n", ident);
         }
      }
      else if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING) 
      {
         fprintf(wfp, "%s{hole\n", ident);
         plusident();
         fprintf(wfp, "%s{type \"TOOLING\"}\n", ident);

         if (strlen(pinname))
            fprintf(wfp,"%s{name \"%s\"}\n", ident, pinname);

         double drill = get_drill_from_block_no_scale(doc, block);

         fprintf(wfp, "%s{drill %lf}\n", ident, drill * scale);
         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_NONPLATED, 2))
            fprintf(wfp, "%s{plated \"FALSE\"}\n", ident);
         else
            fprintf(wfp, "%s{plated \"TRUE\"}\n", ident);

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_STARTLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%startlayer \"%s\"}\n", l);
         }

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_ENDLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%endlayer \"%s\"}\n", l);
         }
         minusident();
         fprintf(wfp, "%send hole}\n", ident);
      }
      else if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
      {
         fprintf(wfp, "%s{pin\n", ident);
         plusident();
          fprintf(wfp, "%s{name \"%s\"}\n", ident, pinname);

         if (a = is_attvalue(doc, np->getAttributesRef(), ATT_COMPPINNR, 0))
            fprintf(wfp, "%s{number \"%s\"}\n", ident, get_attvalue_string(doc, a));

         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
         fprintf(wfp, "%s{mirror \"%s\"}\n", ident, (mirror)?"TRUE":"FALSE");
         fprintf(wfp, "%s{shape \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());

         // attributes
         write_attributes(wfp, np->getAttributesRef());

         minusident();
         fprintf(wfp, "%send pin}\n", ident);
      }
   } 
}

/******************************************************************************
* CCM_WritePANELMiscData
*/
static void CCM_WritePANELMiscData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, 
											  double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = np->getInsert()->getMirrorFlags();
      double block_rot = rotation + np->getInsert()->getAngle();

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
			point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x,insert_y);

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      // here pin found.
      CString pinname = np->getInsert()->getRefname();
      Attrib *a;

      if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL) 
      {
         CString fidName = np->getInsert()->getRefname();

		 int c_bottom = np->getInsert()->getPlacedBottom();
         LayerStruct *layer = doc->FindLayer(np->getLayerIndex());
         if (layer == NULL)
            continue;
         /*if (layer->getLayerType() < LAYTYPE_SIGNAL_TOP || layer->getLayerType() > LAYTYPE_PAD_THERMAL)
            continue;*/  //commented per request from DD. 3/13/2006 MN.  IF the fiducial is in there it is in there, 
							//regardless of what layer it is inserted on.
         if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT || layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
            c_bottom = 1;

         fprintf(wfp, "%s{fiducial\n", ident);
         plusident();
         fprintf(wfp, "%s{type \"FIDUCIAL\"}\n", ident);

         if (np->getInsert()->getRefname())
            fprintf(wfp, "%s{name \"%s\"}\n", ident, clean_text(np->getInsert()->getRefname()));

         fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
         fprintf(wfp, "%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
         minusident();
         fprintf(wfp, "%send fiducial}\n", ident);

         if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL || layer->getLayerType() == LAYTYPE_PAD_ALL)
         {
            c_bottom = !c_bottom;

            fprintf(wfp, "%s{fiducial\n", ident);
            plusident();
            fprintf(wfp, "%s{type \"FIDUCIAL\"}\n", ident);

            if (np->getInsert()->getRefname())
               fprintf(wfp, "%s{name \"%s\"}\n", ident, clean_text(np->getInsert()->getRefname()));

            fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
            fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
            fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
            fprintf(wfp, "%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
            minusident();
            fprintf(wfp, "%send fiducial}\n", ident);
         }
      }

		//(np->getInsert()->getInsertType() == INSERTTYPE_REJECTMARK) ||
		//	(np->getInsert()->getInsertType() == INSERTTYPE_XOUT))   
		else if ((np->getInsert()->getInsertType() == INSERTTYPE_REJECTMARK) ||
			(np->getInsert()->getInsertType() == INSERTTYPE_XOUT)) 
      {
         int c_bottom = np->getInsert()->getPlacedBottom();
         LayerStruct *layer = doc->FindLayer(np->getLayerIndex());
         if (layer == NULL)
            continue;
         if (layer->getLayerType() < LAYTYPE_SIGNAL_TOP || layer->getLayerType() > LAYTYPE_PAD_THERMAL)
            continue;
         if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT || layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
            c_bottom = 1;

         fprintf(wfp, "%s{xout\n", ident);
         plusident();
         fprintf(wfp, "%s{type \"XOUT\"}\n", ident);

         if (np->getInsert()->getRefname())
            fprintf(wfp, "%s{name \"%s\"}\n", ident, clean_text(np->getInsert()->getRefname()));

         fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
         fprintf(wfp, "%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
         minusident();
         fprintf(wfp, "%send xout}\n", ident);

         if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL || layer->getLayerType() == LAYTYPE_PAD_ALL)
         {
            c_bottom = !c_bottom;

            fprintf(wfp, "%s{xout\n", ident);
            plusident();
            fprintf(wfp, "%s{type \"XOUT\"}\n", ident);

            if (np->getInsert()->getRefname())
               fprintf(wfp, "%s{name \"%s\"}\n", ident, clean_text(np->getInsert()->getRefname()));

            fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
            fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
            fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
            fprintf(wfp, "%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
            minusident();
            fprintf(wfp, "%send xout}\n", ident);
         }
      }

      else if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE )
      {
         fprintf(wfp, "%s{hole\n",ident);
         plusident();
         fprintf(wfp, "%s{type \"DRILL\"}\n", ident);

         if (strlen(pinname))
            fprintf(wfp, "%s{name \"%s\"}\n", ident, pinname);

         double drill = get_drill_from_block_no_scale(doc, block);

         fprintf(wfp, "%s{drill %lf}\n", ident, drill * scale);
         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_NONPLATED, 2))
            fprintf(wfp, "%s{plated \"FALSE\"}\n", ident);
         else
            fprintf(wfp, "%s{plated \"TRUE\"}\n", ident);

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_STARTLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%startlayer \"%s\"}\n", l);
         }

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_ENDLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%endlayer \"%s\"}\n", l);
         }
         minusident();
         fprintf(wfp, "%send hole}\n", ident);
      }
      else if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING) 
      {
         fprintf(wfp, "%s{hole\n",ident);
         plusident();
         fprintf(wfp, "%s{type \"TOOLING\"}\n", ident);

         if (strlen(pinname))
            fprintf(wfp, "%s{name \"%s\"}\n", ident, pinname);

         double drill = get_drill_from_block_no_scale(doc, block);

         fprintf(wfp, "%s{drill %lf}\n", ident, drill * scale);
         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_NONPLATED, 2))
            fprintf(wfp, "%s{plated \"FALSE\"}\n",ident);
         else
            fprintf(wfp, "%s{plated \"TRUE\"}\n",ident);

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_STARTLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%startlayer \"%s\"}\n", l);
         }

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_ENDLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%endlayer \"%s\"}\n", l);
         }
         minusident();
         fprintf(wfp,"%send hole}\n", ident);
      }
   } 
}

/******************************************************************************
* CCM_WriteCOMPONENTToolData
*/
static int CCM_WriteCOMPONENTToolData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
												  double rotation, int mirror, double scale, int embeddedLevel, int insertType)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   int cnt = 0;
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = np->getInsert()->getMirrorFlags();
      double block_rot = rotation + np->getInsert()->getAngle();

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
			point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x,insert_y);

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      // here pin found.
      CString pinname = np->getInsert()->getRefname();
      Attrib *a;

      double drill = get_drill_from_block_no_scale(doc, block);

      if (drill > 0) 
      {
         cnt++;
         fprintf(wfp, "%s{hole\n", ident);
         plusident();

         if (insertType == INSERTTYPE_TOOLING)
            fprintf(wfp, "%s{type \"TOOLING\"}\n", ident);
         else if (insertType == INSERTTYPE_DRILLHOLE)
            fprintf(wfp, "%s{type \"DRILL\"}\n", ident);
         else
            fprintf(wfp, "%s{type \"UNKNOWN\"}\n", ident);

         if (strlen(pinname))
            fprintf(wfp, "%s{name \"%s\"}\n", ident, pinname);

         fprintf(wfp, "%s{drill %lf}\n",ident, drill * scale);
         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_NONPLATED, 2))
            fprintf(wfp, "%s{plated \"FALSE\"}\n", ident);
         else
            fprintf(wfp, "%s{plated \"TRUE\"}\n", ident);

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_STARTLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%startlayer \"%s\"}\n", l);
         }

         if (a = is_attvalue(doc, np->getAttributesRef(), DRILL_ENDLAYER, 2))
         {
            CString l = get_attvalue_string(doc, a);
            fprintf(wfp, "%endlayer \"%s\"}\n", l);
         }
         minusident();
         fprintf(wfp, "%send hole}\n", ident);
      }
   }

   return cnt;
}

/******************************************************************************
* write_easyoutline
*/
static int write_easyoutline(FILE *wfp, CPntList *pntList, const char *graph_type)
{
   if (!pntList)
      return 0;

   polycnt = 0;
   POSITION pntPos = pntList->GetHeadPosition();
   while (pntPos)
   {
      CPnt *pnt = pntList->GetNext(pntPos);

      if (fabs(pnt->bulge) > BULGE_THRESHOLD)
      {
         CPnt *p2;
         if (pntPos == NULL)
         {
            // ErrorMessage("Poly Bulge data error !");
            fprintf(flog, "Bulge error in write_outline -> there is a bulge with no following entity\n");
            polyarray.RemoveAll();
            polycnt = 0;
            return 0;
         }
         else
			{
            p2 = pntList->GetAt(pntPos);
			}
                     
         double da = atan(pnt->bulge) * 4;
         double cx,cy,r,sa;
         ArcPoint2Angle(pnt->x, pnt->y, p2->x, p2->y, da, &cx, &cy, &r, &sa);

         // make positive start angle.
         if (sa < 0)
            sa += PI2;

         Point2 ppoly[255];
         int ppolycnt = 255;
         ArcPoly2(pnt->x, pnt->y, cx, cy, da, 1.0, ppoly, &ppolycnt, DegToRad(5)); // 5 degree

         for (int i=0; i<ppolycnt; i++)
         {
            polyarray.SetAtGrow(polycnt, ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2 px;
         px.x = pnt->x;
         px.y = pnt->y;
         px.bulge = pnt->bulge;
         polyarray.SetAtGrow(polycnt++, px);
      }
   }

   BOOL done = FALSE;

   while (!done)
   {
      done = TRUE;
      for (int i=1; i<polycnt; i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);

         if (fabs(p1.x - p2.x) < SMALLNUMBER && fabs(p1.y - p2.y) < SMALLNUMBER)
         {
            polyarray.RemoveAt(i);
            done = FALSE;
            polycnt--;
            break;
         }
      }
   }

   fprintf(wfp, "%s{graphics %s\n", ident, graph_type);
   plusident();

   int cnt = fprintf(wfp, "%s{line ",ident);
   for (int i=0; i<polycnt; i++)
   {
      Point2 p = polyarray.ElementAt(i);
      if (cnt > 70)
         cnt = fprintf(wfp, "\n%s", ident);
      cnt += fprintf(wfp, "\"%lf, %lf\" ", p.x, p.y);
   }

   fprintf(wfp, "}\n");
   minusident();
   fprintf(wfp, "%send graphics}\n",ident);

   // this is C++ debug flag - never have debug_outline running without the compiler debug.
   if (SHOWOUTLINE)
   {
      doc->PrepareAddEntity(cur_file);       
      int layerNum = Graph_Level("DEBUG", "", 0);

      Graph_PolyStruct(layerNum, 0L, FALSE); // poly is always with 0
      Graph_Poly(NULL, 0, 0,0, 0);

      for (int i=0; i<polycnt; i++)
      {
         Point2 p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge); 
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   return 1;
}

/******************************************************************************
* use_layer_for_compoutline
   These layertypes should not be evaluated as compoutline
*/
static int use_layer_for_compoutline(int ltype)
{
   if (ltype == LAYTYPE_PAD_TOP)       return FALSE;
   if (ltype == LAYTYPE_SIGNAL_TOP)    return FALSE;

   if (ltype == LAYTYPE_PAD_BOTTOM)    return FALSE;
   if (ltype == LAYTYPE_SIGNAL_BOT)    return FALSE;

   if (ltype == LAYTYPE_PAD_INNER)     return FALSE;
   if (ltype == LAYTYPE_SIGNAL_INNER)  return FALSE;

   if (ltype == LAYTYPE_PAD_ALL)       return FALSE;
   if (ltype == LAYTYPE_SIGNAL_ALL)    return FALSE;
   if (ltype == LAYTYPE_SIGNAL)        return FALSE;
   if (ltype == LAYTYPE_PAD_OUTER)     return FALSE;

   if (ltype == LAYTYPE_PASTE_TOP)     return FALSE;
   if (ltype == LAYTYPE_PASTE_BOTTOM)  return FALSE;
   if (ltype == LAYTYPE_PASTE_ALL)     return FALSE;

   if (ltype == LAYTYPE_STENCIL_TOP)     return FALSE;
   if (ltype == LAYTYPE_STENCIL_BOTTOM)  return FALSE;

   if (ltype == LAYTYPE_CENTROID_TOP)     return FALSE;
   if (ltype == LAYTYPE_CENTROID_BOTTOM)  return FALSE;

   if (ltype == LAYTYPE_MASK_TOP)      return FALSE;
   if (ltype == LAYTYPE_MASK_BOTTOM)   return FALSE;
   if (ltype == LAYTYPE_MASK_ALL)      return FALSE;

   if (ltype == LAYTYPE_POWERNEG)      return FALSE;
   if (ltype == LAYTYPE_POWERPOS)      return FALSE;
   if (ltype == LAYTYPE_SPLITPLANE)    return FALSE;

   return TRUE;
}

/******************************************************************************
* CCM_FindOutlineData
*
*  Find one closed poly of the correct graphicClass or on the correct layer
*/
static DataStruct *CCM_FindOutlineData(CDataList *DataList, long graphic_class, int layer_class)
{
   DataStruct *outlineData = NULL;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      if (data->getDataType() != T_POLY)
         continue;
      if (data->getGraphicClass() == GR_CLASS_ETCH)
         continue;

      // no electrical layer for body outline
      LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];

      // if either the layer_class or the graphic_class
      if (!(layer_class && layer->getLayerType() == layer_class) && !(graphic_class && data->getGraphicClass() == graphic_class))
         continue;

      if (!use_layer_for_compoutline(layer->getLayerType()))
         continue;

      if (outlineData)
         return NULL;

      CPoly *poly = data->getPolyList()->GetHead();
      if (!poly->isClosed())
         return NULL;

      outlineData = data;
   }

   return outlineData;
}

/******************************************************************************
* CCM_CanJoinOutline
*/
static CPntList *CCM_CanJoinOutline(CDataList *DataList, long graphic_class, int layer_class)
{
   CDataList dataList(false);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      if (data->getDataType() != T_POLY)
         continue;
      if (data->getGraphicClass() == GR_CLASS_ETCH)
         continue;

      // no electrical layer for body outline
      LayerStruct *layer = doc->getDefinedLayerAt(data->getLayerIndex());

      // if either the layer_class or the graphic_class
      if (!(layer_class && layer->getLayerType() == layer_class) && !(graphic_class && data->getGraphicClass() == graphic_class))
         continue;

      if (!use_layer_for_compoutline(layer->getLayerType()))
         continue;

      if (data->getPolyList()->GetCount() != 1)
         return NULL;

      CPoly *poly = data->getPolyList()->GetHead();
      if (poly->isClosed())
         return NULL;

      dataList.AddTail(data);
   }

   double variance = doc->getSettings().JOIN_autoSnapSize;
   if (variance < SMALLNUMBER)
   {
      int decimals = GetDecimals(doc->getSettings().getPageUnits());
      variance = 1;
      for (int i=0; i<decimals; i++)
         variance *= 0.1;
   }

   if (dataList.GetCount() == 0)
      return NULL;

   CPntList *pntList = new CPntList;   
   DataStruct *data = dataList.RemoveHead();
   AddPntsToPntList(pntList, data, TRUE, FALSE);

   while (dataList.GetCount())
   {
      BOOL foundMatch = FALSE;
      CPnt *head = pntList->GetHead();
      CPnt *tail = pntList->GetTail();

      POSITION dataPos = dataList.GetHeadPosition();
      while (dataPos)
      {
         POSITION tempPos = dataPos;
         DataStruct *data = dataList.GetNext(dataPos);

         CPoly *poly = data->getPolyList()->GetHead();
         CPnt *polyHead = poly->getPntList().GetHead();
         if (PntsOverlap(polyHead, tail, variance))
         {
            dataList.RemoveAt(tempPos);
            AddPntsToPntList(pntList, data, TRUE, FALSE);
            foundMatch = TRUE;
            break;
         }
         else if (PntsOverlap(polyHead, head, variance))
         {
            dataList.RemoveAt(tempPos);
            AddPntsToPntList(pntList, data, FALSE, TRUE);
            foundMatch = TRUE;
            break;
         }

         CPnt *polyTail = poly->getPntList().GetTail();
         if (PntsOverlap(polyTail, tail, variance))
         {
            dataList.RemoveAt(tempPos);
            AddPntsToPntList(pntList, data, TRUE, TRUE);
            foundMatch = TRUE;
            break;
         }
         else if (PntsOverlap(polyTail, head, variance))
         {
            dataList.RemoveAt(tempPos);
            AddPntsToPntList(pntList, data, FALSE, FALSE);
            foundMatch = TRUE;
            break;
         }
      }

      if (!foundMatch)
      {
         FreePntList(pntList);
         return NULL;
      }
   }

   CPoly *poly = data->getPolyList()->GetHead();
   CPnt *head = pntList->GetHead();
   CPnt *tail = pntList->GetTail();

   if (PntsOverlap(head, tail, variance))
      return pntList;
   else
   {
      FreePntList(pntList);
      return NULL;
   }
}

/******************************************************************************
* PntsOverlap
*/
static BOOL PntsOverlap(CPnt *p1, CPnt *p2, double variance)
{ 
	return ((fabs(p1->x - p2->x) < variance) && (fabs(p1->y - p2->y) < variance));
}

/******************************************************************************
* AddPntsToPntList
*/
static void AddPntsToPntList(CPntList *pntList, DataStruct *data, BOOL tail, BOOL reversed)
{
   CPoly *poly = data->getPolyList()->GetHead();

   // copy poly's pntList
   CPntList newPntList;
   POSITION pntPos = poly->getPntList().GetHeadPosition();
   while (pntPos)
   {
      CPnt *pnt = poly->getPntList().GetNext(pntPos);
      CPnt *newPnt = new CPnt(*pnt);
      newPntList.AddTail(newPnt);
   }

   // fix bulges
   if (reversed)
      ReversePoly(&newPntList);

   if (tail)
   {
      while (!newPntList.IsEmpty())
      {
         CPnt *pnt = newPntList.GetHead();
         newPntList.RemoveHead();
         pntList->AddTail(pnt);
      }
   }
   else // add to head
   {
      while (!newPntList.IsEmpty())
      {
         CPnt *pnt = newPntList.GetTail();
         newPntList.RemoveTail();
         pntList->AddHead(pnt);
      }
   }
}

/******************************************************************************
* CCM_WriteOutlineData
*/
static int CCM_WriteOutlineData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror,
										  double scale, int embeddedLevel, int insertLayer, long graphic_class, int layer_class)
{
   int outline_found = 0;
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      if (data->getGraphicClass() == GR_CLASS_ETCH)
         continue;
      if (data->getDataType() == T_TEXT)
         continue;

      if (data->getDataType() != T_INSERT)
      {
         int layerNum = data->getLayerIndex();
         if (insertLayer != -1)
            layerNum = insertLayer;

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layerNum, mirror))
            continue;

         // no electrical layer for body outline
         LayerStruct *layer = doc->getLayerArray()[layerNum];
			if (!(layer_class && layer->getLayerType() == layer_class) && !(graphic_class && data->getGraphicClass() == graphic_class))
				continue;

         if (!use_layer_for_compoutline(layer->getLayerType()))
            continue;

         Outline_Add(data, scale, insert_x, insert_y, rotation, mirror);
         outline_found++;
      }
      else
      {
         if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
            continue;

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

         if (!(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_BLOCK_APERTURE) )
         {
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (data->getLayerIndex() != -1)
               block_layer = data->getLayerIndex();

            outline_found += CCM_WriteOutlineData(&block->getDataList(), 
                  point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(),
                  embeddedLevel+1, block_layer, graphic_class, layer_class);
         }
      }
   }

   return outline_found;
}

/******************************************************************************
* CCM_WriteCP
*/
static void CCM_WriteCP(FILE *fp,  FileStruct *f, const char *comp)
{
   POSITION netPos = f->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = f->getNetList().GetNext(netPos);

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         // here comp/pin found = all done
         if (strcmp(compPin->getRefDes(), comp)) 
				continue;
         
         fprintf(fp, "%s{pin\n", ident);
         plusident();
         fprintf(fp, "%s{name \"%s\"}\n", ident, compPin->getPinName());

         Attrib *a;
         if (a = is_attvalue(doc, compPin->getAttributesRef(), ATT_COMPPINNR, 0))
            fprintf(fp, "%s{number \"%s\"}\n", ident, get_attvalue_string(doc, a));

         fprintf(fp, "%s{loc %lf %lf}\n", ident, compPin->getOriginX(), compPin->getOriginY());
         fprintf(fp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(compPin->getRotationRadians())));
         fprintf(fp, "%s{mirror \"%s\"}\n", ident,( compPin->getMirror())?"TRUE":"FALSE");
      
         BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
         if (block)
            fprintf(fp, "%s{shape \"%s\" %d}\n", ident,block->getName(), block->getBlockNumber());
         else
         {
            fprintf(fp, "%s{shape \"%s\"}\n", ident, "NULLSHAPE");
            fprintf(flog, "Pin [%s] [%s] has no shape\n", compPin->getRefDes(), compPin->getPinName());
            display_error++;
         }
         fprintf(fp, "%s{Entity %ld}\n", ident, compPin->getEntityNumber());

         // attributes
         write_attributes(fp, compPin->getAttributesRef());

         minusident();
         fprintf(fp, "%send pin}\n", ident);         
      }
   }
   return;
}

/******************************************************************************
* CCM_WriteCOMPONENTData
*/
static void CCM_WriteCOMPONENTData(FILE *wfp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
											  double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   double accuracy = get_accuracy(doc);

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;

      if (np->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)           continue;   
      if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)               continue;   
      if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPAD)           continue;   
      if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)               continue;   
      if (np->getInsert()->getInsertType() == INSERTTYPE_TEST_ACCESSPOINT)  continue;   
      if (np->getInsert()->getInsertType() == INSERTTYPE_TEST_PROBE)        continue;   

      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int c_mirror = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
      int c_bottom = np->getInsert()->getPlacedBottom();
      int block_mirror = mirror ^ c_mirror;
      double block_rot = rotation + np->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      
      CString compname = np->getInsert()->getRefname();
      Attrib *a;

      // tooling hole
      if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE )
      {
         // if a component was marked as tooling, but did not contain any pins, write the 
         // tooling location
         int cnt = CCM_WriteCOMPONENTToolData(wfp, &(block->getDataList()), point2.x, point2.y, 
					block_rot, block_mirror, scale, 0, np->getInsert()->getInsertType());

         if (cnt == 0)
         {
            double drill = get_drill_from_block_no_scale(doc, block);

            fprintf(wfp, "%s{hole\n",ident);
            plusident();
            fprintf(wfp, "%s{type \"DRILL\"}\n", ident);

            if (strlen(compname))
               fprintf(wfp, "%s{name \"%s\"}\n", ident, compname);
            
            fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
            fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
            fprintf(wfp, "%s{drill %lf}\n", ident, drill * scale);
            minusident();
            fprintf(wfp, "%send hole}\n", ident);
         }
      }
      else if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
      {
         // if a component was marked as tooling, but did not contain any pins, write the 
         // tooling location
         int cnt = CCM_WriteCOMPONENTToolData(wfp, &(block->getDataList()), point2.x, point2.y, 
					block_rot, block_mirror, scale, 0, np->getInsert()->getInsertType());

         if (cnt == 0)
         {
            double drill = get_drill_from_block_no_scale(doc, block);

            fprintf(wfp, "%s{hole\n", ident);
            plusident();
            fprintf(wfp, "%s{type \"TOOLING\"}\n", ident);

            if (strlen(compname))
               fprintf(wfp,"%s{name \"%s\"}\n", ident, compname);
            
            fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
            fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
            fprintf(wfp, "%s{drill %lf}\n", ident, drill * scale);
            minusident();
            fprintf(wfp, "%send hole}\n", ident);
         }
      }
      else
      {
         if (!c_bottom && c_mirror)
         {
            fprintf(flog, "Component [%s] placed on TOP but MIRRORED\n", compname);
            display_error++;
         }
         else if (c_bottom && !c_mirror)
         {
            fprintf(flog, "Component [%s] placed on BOTTOM but NOT MIRRORED\n", compname);
            display_error++;
         }

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         fprintf(wfp, "%s{component \"%s\"\n", ident, insertTypeToDisplayString(np->getInsert()->getInsertType()));
         plusident();
         fprintf(wfp, "%s{name \"%s\"}\n", ident, compname);
         fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());

         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_SMDSHAPE, 1))
            fprintf(wfp, "%s{technology \"SMD\"}\n", ident);
         else
            fprintf(wfp, "%s{technology \"THRU\"}\n", ident);

         fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
         fprintf(wfp," %s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");

         // attributes
         write_attributes(wfp, np->getAttributesRef());

         if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
         {
            fprintf(wfp, "%s{devicetype \"%s\"}\n", ident, get_attvalue_string(doc, a));
            do_typelist(wfp, &file->getTypeList(), get_attvalue_string(doc, a));
         }

         if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         {
            fprintf(wfp, "%s{component outline\n", ident);
            plusident();

            Outline_Start(doc);

            int outline_found = CCM_WriteOutlineData(&block->getDataList(), point2.x, point2.y, block_rot, 
						block_mirror, 1.0, 0, -1, GR_CLASS_COMPOUTLINE, LAYTYPE_COMPONENTOUTLINE);

            if (!outline_found)
               outline_found = CCM_WriteOutlineData(&block->getDataList(), point2.x, point2.y, block_rot, block_mirror, 1.0, 0, -1, 0, 0);

            if (!outline_found)
            {
               fprintf(flog, "Did not find a Component Outline for [%s] [%s].\n", compname, block->getName());
               display_error++;
            }
            else
            {
               int returnCode;
               if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "component outline"))
               {
                  fprintf(flog, "Error in easy outline for [%s]\n", block->getName());
                  display_error++;
               }
            }

            Outline_FreeResults();

            minusident();
            fprintf(wfp, "%send component outline}\n", ident);
         }

         // find if already exist
         DataStruct *d;
         if ((d = centroid_exist_in_block(block)) != NULL)
         {
            Point2 point1;

            // here update position
            point1.x = d->getInsert()->getOriginX() * scale;
            if (block_mirror)
               point1.x = -point1.x;
            point1.y = d->getInsert()->getOriginY() * scale;

            Mat2x2 mm;
            RotMat2(&mm, block_rot);

            TransPoint2(&point1, 1, &mm, 0.0, 0.0);
            fprintf(wfp, "%s{centroid %lf %lf %1.3lf}\n", ident, point2.x + point1.x , point2.y + point1.y, 
						normalizeDegrees(RadToDeg(block_rot + d->getInsert()->getAngle())));
         }

         CCM_WriteCP(wfp, file, compname);
      
         minusident();
         fprintf(wfp, "%send component}\n", ident);
		}  
   } 
}

/******************************************************************************
* CCM_WritePANELFileData
*/
static void CCM_WritePANELFileData(FILE *wfp, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() != INSERTTYPE_PCB)
			continue;   

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int c_mirror = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
      int c_bottom = np->getInsert()->getPlacedBottom();
      int block_mirror = mirror ^ c_mirror;
      double block_rot = rotation + np->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      CString pcbname = np->getInsert()->getRefname();

      fprintf(wfp, "%s{file\n", ident);
      plusident();
      fprintf(wfp, "%s{type \"PCB\"}\n", ident);

      if (strlen(pcbname))
         fprintf(wfp, "%s{name \"%s\"}\n", ident, pcbname);

      fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
      fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
      fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
      fprintf(wfp, "%s{mirror \"%s\"}\n", ident, (block_mirror)?"TRUE":"FALSE");
      minusident();
      fprintf(wfp, "%send file}\n", ident);
   }
}

/******************************************************************************
* CCM_WritePROBEPOINTSData
*/
static void CCM_WritePROBEPOINTSData(FILE *wfp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
												 double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)
			continue;   

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int c_mirror = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
      int c_bottom = np->getInsert()->getPlacedBottom();
      int block_mirror = mirror ^ c_mirror;
      double block_rot = rotation + np->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      CString compname = np->getInsert()->getRefname();

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      fprintf(wfp, "%s{probepoint\n",ident);
      plusident();
      fprintf(wfp, "%s{name \"%s\"}\n", ident, compname);
      fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
      fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
      fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
      fprintf(wfp, "%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
      fprintf(wfp, "%s{Entity %ld}\n", ident, np->getEntityNumber());

      // all attributes, including netnames
      write_all_attributes(wfp, np->getAttributesRef());

      minusident();
      fprintf(wfp, "%send probepoint}\n", ident);
   } 
}

/******************************************************************************
* CCM_WriteACCESSPOINTSData
*/
static void CCM_WriteACCESSPOINTSData(FILE *wfp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
												  double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() != INSERTTYPE_TEST_ACCESSPOINT)
			continue;   

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int c_mirror = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
      int c_bottom = np->getInsert()->getPlacedBottom();
      int block_mirror = mirror ^ c_mirror;
      double block_rot = rotation + np->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      CString compname = np->getInsert()->getRefname();

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      fprintf(wfp, "%s{accesspoint\n", ident);
      plusident();
      fprintf(wfp, "%s{name \"%s\"}\n", ident, compname);
      fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
      fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
      fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
      fprintf(wfp,"%s{layer \"%s\"}\n", ident, (c_bottom)?"BOTTOM":"TOP");
      fprintf(wfp, "%s{Entity %ld}\n", ident, np->getEntityNumber());

      // all attributes, including netnames
      write_all_attributes(wfp, np->getAttributesRef());

      minusident();
      fprintf(wfp,"%send accesspoint}\n", ident);
   }  
}

/******************************************************************************
* do_components
*/
static void do_components(FILE *fp, FileStruct *file)
{
   progress->SetStatus("Writing Components...");

   fprintf(fp, "%s{components\n", ident);
   plusident();

   CCM_WriteCOMPONENTData(fp, file, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   minusident();
   fprintf(fp, "%send components}\n", ident);
}

/******************************************************************************
* do_panelfiles
*/
static void do_panelfiles(FILE *fp, FileStruct *file)
{
   progress->SetStatus("Writing Files...");

	fprintf(fp, "%s{files\n", ident);
   plusident();

   CCM_WritePANELFileData(fp, file, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   minusident();
   fprintf(fp, "%send files}\n", ident);
	//CCM_WritePANELFileFiducials(fp, file, &(file->getBlock()->getDataList()), 
   //      file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
   //      file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

}

/******************************************************************************
* do_probepoints
*/
static void do_probepoints(FILE *fp, FileStruct *file)
{
   progress->SetStatus("Writing Probe Points...");

	fprintf(fp, "%s{probepoints\n", ident);
   plusident();

   CCM_WritePROBEPOINTSData(fp, file, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   minusident();
   fprintf(fp, "%send probepoints}\n", ident);
}

/******************************************************************************
* do_accesspoints
*/
static void do_accesspoints(FILE *fp, FileStruct *file)
{
   progress->SetStatus("Writing Access Points...");

   fprintf(fp, "%s{accesspoints\n", ident);
   plusident();

   CCM_WriteACCESSPOINTSData(fp, file, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   minusident();
   fprintf(fp, "%send accesspoints}\n", ident);
}

/******************************************************************************
* do_shapes
*/
static void do_shapes(FILE *fp, FileStruct *file)
{
   // do not write outline if in geometry mode. 
   int showoutline = SHOWOUTLINE;
   SHOWOUTLINE = FALSE;
   double accuracy = get_accuracy(doc);

   fprintf(fp, "%s{pcbgeometries\n", ident);
   plusident();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
			continue;
      if (block->getBlockType() != BLOCKTYPE_TOOLING && block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
			continue;

		fprintf(fp, "%s{geometry \"%s\" %d\n", ident, block->getName(), block->getBlockNumber());
      plusident();

      Attrib *a;
      if (a = is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
         fprintf(fp, "%s{technology \"SMD\"}\n", ident);
      else
         fprintf(fp, "%s{technology \"THRU\"}\n", ident);

      // attributes
      write_attributes(fp, block->getAttributesRef());

      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
      {
         fprintf(fp, "%s{component outline\n", ident);
         plusident();                                          

         Outline_Start(doc);

         int outline_found = CCM_WriteOutlineData(&(block->getDataList()), 
					0.0, 0.0, 0.0, 0, 1.0, 0, -1,GR_CLASS_COMPOUTLINE,LAYTYPE_COMPONENTOUTLINE);

         if (!outline_found)
            outline_found = CCM_WriteOutlineData(&(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1,0, 0);

         if (!outline_found)
         {
            fprintf(flog, "Did not find a Component Outline for [%s].\n", block->getName());
            display_error++;
         }
         else
         {
            int returnCode;
            if (!write_easyoutline(fp, Outline_GetOutline(&returnCode, accuracy), "component outline"))
            {
               fprintf(flog, "Error in easy outline for [%s]\n", block->getName());
               display_error++;
            }

         }  

         Outline_FreeResults();

         minusident();
         fprintf(fp, "%send component outline}\n", ident);
      }
      
      CCM_WriteCOMPONENTPinData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1);

      // find if already exist
      DataStruct *data;
      if ((data = centroid_exist_in_block(block)) != NULL)
      {
         // here update position
         fprintf(fp, "%s{centroid %lf %lf %1.3lf}\n", ident, data->getInsert()->getOriginX(), data->getInsert()->getOriginY(),
						normalizeDegrees(RadToDeg(data->getInsert()->getAngle())));
      }
      minusident();
      fprintf(fp, "%send geometry}\n", ident);
   }

   minusident();
   fprintf(fp, "%send pcbgeometries}\n", ident);

   // reset
   SHOWOUTLINE = showoutline;
}

/******************************************************************************
* CCM_WritePADSTACKData
	return 0x1 top
			 0x2 bottom
			 0x4 drill
*/
static int CCM_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
											int mirror, double scale, int embeddedLevel, int insertLayer, double *drill)
{
   *drill = 0;
	Mat2x2 mPadstack;
   RotMat2(&mPadstack, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
         continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPAD)
         continue;

      Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
      point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &mPadstack, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      if ( (block->getFlags() & BL_TOOL))
      {
         *drill = block->getToolSize() * scale;
      }
      else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         int layer = Get_ApertureLayer(doc, np, block, insertLayer);

         fprintf(fp, "%s{padshape \"%s\" %d\n", ident, block->getName(), block->getBlockNumber());
         plusident();
         fprintf(fp, "%s{layer \"%s\"}\n", ident, layerTypeToString(doc->getLayerArray()[layer]->getLayerType()));
		 int stknum = doc->getLayerArray()[layer]->getElectricalStackNumber();
		 if ((stknum > 0) && (stknum <= highest_stacknum))
			 fprintf(fp, "%s{stacknum \"%d\"}\n", ident, stknum);
		 else
		 {
            stknum = 0;
            switch (doc->getLayerArray()[layer]->getLayerType())
            {
               case  LAYTYPE_PAD_TOP:
               case  LAYTYPE_SIGNAL_TOP:
                  stknum = 1;
                  break;
               case  LAYTYPE_PAD_BOTTOM:
               case  LAYTYPE_SIGNAL_BOT:
                  stknum = highest_stacknum;
                  break;
               case  LAYTYPE_PAD_INNER:
               case  LAYTYPE_SIGNAL_INNER:
                  stknum = -1;
                  break;
               case  LAYTYPE_PAD_OUTER:
               case  LAYTYPE_SIGNAL_OUTER:
                  stknum = -2;
                  break;
               case  LAYTYPE_PAD_ALL:
               case  LAYTYPE_SIGNAL_ALL:
                  stknum = -3;
                  break;
            }
			if (stknum) fprintf(fp, "%s{stacknum \"%d\"}\n", ident, stknum);
		 }


         LayerStruct *l = doc->FindLayer(layer);
         if (l->getNeverMirror())          
            fprintf(fp, "%s{layerusage \"NEVERMIRROR\"}\n", ident);
         else if (l->getMirrorOnly())           
            fprintf(fp, "%s{layerusage \"MIRRORONLY\"}\n", ident);


         // coalesced = aperture insert angle PLUS aperture definition angle
         double coalescedApertureRotation = np->getInsert()->getAngleRadians() + block->getRotation();

         Mat2x2 mApInsertRot;
         RotMat2(&mApInsertRot, coalescedApertureRotation);
         Point2 offsetPt(block->getXoffset() * scale, block->getYoffset() * scale);
         TransPoint2(&offsetPt, 1, &mApInsertRot, 0.0, 0.0);

         fprintf(fp, "%s{shape \"%s\"}\n", ident, shapes[block->getShape()]);
         fprintf(fp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
         fprintf(fp, "%s{offset %lf %lf}\n", ident, offsetPt.x, offsetPt.y);
         fprintf(fp, "%s{size %lf %lf}\n", ident, block->getSizeA(), block->getSizeB());
         minusident();
         fprintf(fp, "%send padshape}\n", ident);
      }
   }

   return 0;
} 

/******************************************************************************
* CCM_GetPADSTACKData
	return 0x1 top
			0x2 bottom
			0x4 drill
*/
static int CCM_GetPADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
										 int mirror, double scale, int embeddedLevel, int insertLayer, double *drill)
{
	int typ = 0;
   *drill = 0;
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
         continue;
      if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPAD)
         continue;

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
			point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      if ( (block->getFlags() & BL_TOOL))
      {
         *drill = block->getToolSize() * scale;
      }
      else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         int layer = Get_ApertureLayer(doc, np, block, insertLayer);
         LayerStruct *l = doc->FindLayer(layer);

         if (l->getNeverMirror())          
         {
            typ |= 8;
         }
         else if (l->getMirrorOnly())           
         {
            typ |= 16;
         }
         else
         {
            switch (doc->getLayerArray()[layer]->getLayerType())
            {
            case  LAYTYPE_PAD_TOP:
            case  LAYTYPE_SIGNAL_TOP:
               typ |= 1;
               break;
            case  LAYTYPE_PAD_BOTTOM:
            case  LAYTYPE_SIGNAL_BOT:
               typ |= 2;
               break;
            case  LAYTYPE_PAD_INNER:
            case  LAYTYPE_SIGNAL_INNER:
               typ |= 4;
               break;
            case  LAYTYPE_PAD_ALL:
            case  LAYTYPE_SIGNAL_ALL:
               typ |= 7;
               break;
            }
         }
      }
   }

   return typ;
}

/******************************************************************************
* do_layerlist
*/
static void do_layerlist(FILE *fp)
{
   fprintf(fp, "%s{layerlist\n", ident);
   plusident();

   highest_stacknum = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (!layer)
			continue; 

      if (layer->getElectricalStackNumber() > highest_stacknum)
         highest_stacknum = layer->getElectricalStackNumber();
   }

   for (int i=1;i<= highest_stacknum; i++)
   {
      for (int j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
      {
         LayerStruct *layer = doc->getLayerArray()[j];
         if (!layer)
				continue;
         if (layer->getElectricalStackNumber() != i)
				continue;

         // only electrical
         if (is_electricallayer(layer->getLayerType()))
         {
            fprintf(fp, "%s{layer %s\n", ident, get_ccmlayername(j));
            plusident();

            // layer stack
            fprintf(fp, "%s{stacknum %d}\n",ident, layer->getElectricalStackNumber());

            // layertype,
            if (layer->getElectricalStackNumber() == 1)
               fprintf(fp, "%s{technology \"TOP\"}\n", ident);
            else if (layer->getElectricalStackNumber() == highest_stacknum) 
               fprintf(fp, "%s{technology \"BOTTOM\"}\n", ident);
            else
               fprintf(fp, "%s{technology \"INNER\"}\n", ident);

            if (layer->getLayerType() == LAYTYPE_SPLITPLANE || layer->getLayerType() == LAYTYPE_POWERNEG) 
               fprintf(fp, "%s{polarity \"NEGATIVE\"}\n", ident);
            else
               fprintf(fp, "%s{polarity \"POSITIVE\"}\n", ident);

            // attributes
            write_attributes(fp, layer->getAttributesRef());

            minusident();
            fprintf(fp, "%send layer}\n", ident);
				break; // only do 1 layer per stacknum
         }
      }
   }

   minusident();
   fprintf(fp, "%send layerlist}\n", ident);
}

/******************************************************************************
* do_padstacks
*/
static void do_padstacks(FILE *fp, FileStruct *file)
{
   fprintf(fp, "%s{pad_stacks\n", ident);
   plusident();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
			continue;
      if (block->getFlags() & BL_WIDTH)
			continue;
      if (block->getFlags() & BL_APERTURE)
			continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)
			continue;
      if (block->getFlags() & BL_FILE && !doc->is_file_inserted(block->getBlockNumber())) // fileblock in fileblock is now allowed.
			continue;
		if (block->getBlockType() != BLOCKTYPE_PADSTACK)
			continue;

      fprintf(fp, "%s{padstack \"%s\" %d\n", ident, block->getName(), block->getBlockNumber());
      plusident();

      double drill;
      Attrib *a;
      if (a = is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
         fprintf(fp, "%s{technology \"SMD\"}\n", ident);
      else if (a = is_attvalue(doc, block->getAttributesRef(), ATT_BLINDSHAPE, 1))
         fprintf(fp, "%s{technology \"BLIND\"}\n", ident);
      else
      {
         // here make sure the pad is on copper layers
         int typ = CCM_GetPADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1, &drill);

         switch (typ)
         {
         case 1:
            fprintf(fp, "%s{technology \"TOP\"}\n", ident);
            break;
         case 2:
            fprintf(fp, "%s{technology \"BOTTOM\"}\n", ident);
            break;
         case 3:
            fprintf(fp, "%s{technology \"OUTER\"}\n", ident);
            break;
         case 4:
            fprintf(fp, "%s{technology \"INNER\"}\n", ident);
            break;
         case 7:
            fprintf(fp, "%s{technology \"THRU\"}\n", ident);
            break;
         case 8:
            fprintf(fp, "%s{technology \"CONDITIONAL_TOP\"}\n", ident);
            break;
         case 16:
            fprintf(fp, "%s{technology \"CONDITIONAL_BOT\"}\n", ident);
            break;
         default:
            fprintf(fp, "%s{technology \"NONE\"}\n", ident);
            break;
         }
      }

      CCM_WritePADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1, &drill);

      if (drill > 0)
         fprintf(fp, "%s{drill %lf}\n", ident, drill);

      minusident();
      fprintf(fp, "%send padstack}\n", ident);
   }

   minusident();
   fprintf(fp, "%send pad_stacks}\n", ident);
}

/******************************************************************************
* load_CCMsettings
*/
static void load_CCMsettings(const CString fname)
{
   FILE *fp;
   char line[255];
   char *lp;

   ccmprocess = "";
   WRITE_TRACES = FALSE;

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"CCM Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = strtok(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
					continue;
            ComponentSMDrule = atoi(lp);
         }
         else if (!STRCMPI(lp, ".SHOWOUTLINE"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
					continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               SHOWOUTLINE = TRUE;
         }
         else if (!STRCMPI(lp, ".WRITE_TRACES"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL)
					continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               WRITE_TRACES = TRUE;
         }
         else if (!STRCMPI(lp, ".CCMPROCESS"))
         {
            if ((lp = strtok(NULL, "\"")) == NULL)
					continue;   // get to first \"
            if ((lp = strtok(NULL, "\"")) == NULL)
					continue;
            ccmprocess = lp;
         }
      }
   }

   fclose(fp);
}

static void CCM_WritePANELFileFiducials(FILE *wfp, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
			continue;
      if (np->getInsert()->getInsertType() != INSERTTYPE_FIDUCIAL)
			continue;   

		Point2 point2;
      point2.x = np->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = np->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int c_mirror = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
      int c_bottom = np->getInsert()->getPlacedBottom();
      int block_mirror = mirror ^ c_mirror;
      double block_rot = rotation + np->getInsert()->getAngle();

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      CString pcbname = np->getInsert()->getRefname();

      fprintf(wfp, "%s{fiducial\n", ident);
      plusident();
      fprintf(wfp, "%s{type \"Fiducial\"}\n", ident);

      if (strlen(pcbname))
         fprintf(wfp, "%s{name \"%s\"}\n", ident, pcbname);

      fprintf(wfp, "%s{geometry \"%s\" %d}\n", ident, block->getName(), block->getBlockNumber());
      fprintf(wfp, "%s{loc %lf %lf}\n", ident, point2.x, point2.y);
      fprintf(wfp, "%s{rotation %1.3lf}\n", ident, normalizeDegrees(RadToDeg(block_rot)));
      fprintf(wfp, "%s{Layer \"%s\"}\n", ident, (block_mirror)?"BOTTOM":"TOP");
      minusident();
      fprintf(wfp, "%send fiducial}\n", ident);
   }
}
