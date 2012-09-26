// $Header: /CAMCAD/5.0/read_wrt/Unicam_o.cpp 61    6/17/07 9:00p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1999. All Rights Reserved.

*/

#include "stdafx.h"
#include "ccdoc.h"   
#include "graph.h"             
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include <math.h>
#include <float.h>
#include "pcbutil.h"
#include "pcblayer.h"
#include "apertur2.h"
#include "polylib.h"
#include "Unicam_o.h"
#include "CCEtoODB.h"
#include "centroid.h"
#include "RwUiLib.h"

//extern LayerTypeInfoStruct  layertypes[];
extern CProgressDlg        *progress;

/* Static Variables *********************************************************/
static CCEtoODBDoc          *doc;
static double              unitsFactor;
static int                 pageunits;
static int                 output_units_accuracy = 3;
static long                cur_artworkcnt;
static int                 display_error;
static FILE                *flog;
static CString             testpinprefix;
static CString             fiducialname;
static CString             panelfiducialname;
static int                 viacnt = 0;
static int                 testpadcnt = 0;

static CComplistArray      complistarray;    // this is the device - mapping
static int                 complistcnt;

static CompPinInstArray    comppininstarray; 
static int                 comppininstcnt;

static CShapePinArray      shapepinarray; 
static int                 shapepincnt;

static UNICAMclocationArray clocationarray; 
static int                 clocationcnt;

static UNICAMPadstackArray padstackarray; 
static int                 padstackcnt;

static UNICAMRoutesArray   routesarray;   
static int                 routescnt;

static int                 max_signalcnt;    // this is the number of XRF layers
static int                 men_solderlayer;  // this is the max number of Signal layers
static double              defaultdrillsize;
static double              defaultnopadsize;
static int                 convert_tp_comps; // convert testpoints to component and create a shape with the 
                                             // padstack as it's contents.

static CString             testpointshapeprefix;
static int                 update_device_attribute;
static int                 namespace_allowed;
static int                 write_artwork;
static int                 write_Fiducial_props;
static char                ident[80];
static int                 identcnt;


/* Function Prototypes *********************************************************/

static void InitializeMember();
static void FreeMember();
static int load_UNICAMsettings(const CString fname, int page_units);
static int do_padstacks(double scale);
static int get_padstackindex(const char *p);
static int plusident();
static int close_b(FILE *fo);
static int do_pcbshapes(FILE *fp, double scale);
static int do_routes(FILE *fp, FileStruct *file,CNetList *NetList);
static long cnv_unit(double x);
static void WritePCB(FILE* pFile, FileStruct *pcbFile);
static void WritePanel(FILE* pFile, FileStruct *panelFile);

// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static int UNICAM_WritePRIMARYBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, long gclass, int layertype);

// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static int UNICAM_WriteBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, int layertype);

// this function only loops throu entities marked other than BL_CLASS_BOARD 
static int UNICAM_WriteSHAPEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, UNICAMclocation *c);
static void UNICAM_WriteCOMPONENTData(FILE *fp, FileStruct *file, CDataList *DataList, 
      double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
static void UNICAM_WriteROUTESData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname);
static void UNICAM_WriteVIASData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname);
static void UNICAM_GetROUTESData(CDataList *DataList, int embeddedLevel, int insertLayer);
static int UNICAM_GetPADSTACKData(CDataList *DataList, int insertLayer, double *drill, double scale, CUnicamPadstack *padstack);

/******************************************************************************
* UNICAM_WriteFiles
*/
void UNICAM_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, double UnitsFactor, int PageUnits)
{
   FILE *wfp;
   CString  boardname;   
   int pcb_found = FALSE;

   doc = Doc;
   display_error = FALSE;
   viacnt = 0;
   testpadcnt = 0;
   ident[0] = '\0';
   identcnt = 0;

   // Set Static variables so don't have to pass them every time
   // 2/3 nanometer;
   unitsFactor = Units_Factor(PageUnits, UNIT_INCHES) * 38100000.0;
   pageunits = PageUnits;
   cur_artworkcnt = 0;

   InitializeMember();

   CString unicamPdwLogFile;
   flog = getApp().OpenOperationLogFile("unicam.log", unicamPdwLogFile);
   if (flog == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(flog, "Unicam PDW");
	fprintf(flog, "\n\n");
   
   output_units_accuracy = GetDecimals(PageUnits); 

   CString settingsFile( getApp().getExportSettingsFilePath("unicam.out") );
   load_UNICAMsettings(settingsFile, UNIT_MILS);   // output always in MILS

   FileStruct *file = doc->getFileList().GetOnlyShown(blockTypePcb);
   int pcbBlockNumberUsed = -1;

   if (file != NULL)
   {
      pcb_found = TRUE;
   }
   else
   {
      file = doc->getFileList().GetOnlyShown(blockTypePanel);

      if (file != NULL)
      {
         POSITION pos = file->getBlock()->getHeadDataInsertPosition();
         while (pos)
         {
            DataStruct *data = file->getBlock()->getNextDataInsert(pos);
            InsertStruct *insert = data->getInsert();

            if (insert->getInsertType() == insertTypePcb)
            {
               if (pcbBlockNumberUsed != -1 && pcbBlockNumberUsed != insert->getBlockNumber())
               {
                  ErrorMessage("Heterogeonious panels are not supported by Unicam!", "Unicam Output", MB_OK | MB_ICONHAND);

                  fprintf(flog, "\n-------END OF UNICAM EXPORT LOG-------\n");
                  if (!unicamPdwLogFile.IsEmpty())
                     fclose(flog);

                  return;
               }
               else
               {
                  pcbBlockNumberUsed = insert->getBlockNumber();
               }
            }
         }
      }
   }

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %s", filename);
      ErrorMessage( tmp, "Error File open", MB_OK | MB_ICONHAND);

      fprintf(flog, "\n-------END OF UNICAM EXPORT LOG-------\n");
      if (!unicamPdwLogFile.IsEmpty())
         fclose(flog);

      return;
   }

   //switch_on_pcbfiles_used_in_panels(doc, TRUE);

   CWaitCursor wait;

   // header
   fprintf(wfp, "$ UCW Job File\n");
   fprintf(wfp, "$ ------------------------------------\n");
   fprintf(wfp, "$ Job: %s\n", filename);
   fprintf(wfp, "$ Rev: 1\n");
   fprintf(wfp, "$ Generated by : %s\n", getApp().getCamCadTitle());
   fprintf(wfp, "$ ------------------------------------\n");

   fprintf(wfp, "project \"%s\"\n", filename);
   fprintf(wfp, "%s{\n", ident);
   plusident(); 
   fprintf(wfp,"%sversion {1,1,4}\n", ident);
   fprintf(wfp, "%soptions\n", ident);
   fprintf(wfp, "%s{\n", ident);
   plusident();
   fprintf(wfp, "%sformat : 0,\n", ident);
   fprintf(wfp, "%sunit   : 3\n", ident);
   close_b(wfp);
   fprintf(wfp, "%sfonts\n", ident);
   fprintf(wfp, "%s{ 1   : {\"CG Times\",\"\"} }\n", ident);

   boardname = filename;

   if (file != NULL)
   {
      if (file->getBlockType() == blockTypePanel)
      {
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos)
         {
            FileStruct *pcbFile = doc->getFileList().GetNext(pos);

            if (pcbFile->getBlockType() != blockTypePcb)
               continue;

            if (pcbFile->getBlock()->getBlockNumber() == pcbBlockNumberUsed)
            {
               WritePCB(wfp, pcbFile);
               pcb_found = TRUE;

               break;      // only write one board
            }
         }

         if (pcb_found)
         {
            WritePanel(wfp, file);
         }
      }
      else
      {
         WritePCB(wfp, file);
      }
   }

   if (!pcb_found)
   {
      ErrorMessage("No PCB file found !","Unicam Output", MB_OK | MB_ICONHAND);

      fprintf(flog, "\n-------END OF UNICAM EXPORT LOG-------\n");
      if (!unicamPdwLogFile.IsEmpty())
         fclose(flog);

      return;
   }

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);

      if (!file->isShown())
         continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         doc->CalcBlockExtents(file->getBlock());

         fprintf(wfp, "%sview 0, \"GLOBAL\"\n", ident);
         fprintf(wfp, "%s{\n", ident);
         plusident();
         fprintf(wfp, "%ssize\n", ident);
         fprintf(wfp, "%s{\n", ident);
         plusident();
         fprintf(wfp, "%sformat : \"Unknown\",\n", ident);
         fprintf(wfp, "%sorient : 0,\n", ident);

         long w = cnv_unit(file->getBlock()->getXmax() - file->getBlock()->getXmin());
         long h = cnv_unit(file->getBlock()->getYmax() - file->getBlock()->getYmin());

         fprintf(wfp, "%swidth  : %ld,\n", ident, w);
         fprintf(wfp, "%sheight : %ld\n", ident, h);
         close_b(wfp);  // size
         close_b(wfp);  // close view

         break;
      }
   }

   close_b(wfp);  // end of project
   fprintf(wfp, "end\n");

   fclose(wfp);

   fprintf(flog, "\n-------END OF UNICAM EXPORT LOG-------\n");

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!unicamPdwLogFile.IsEmpty())
      fclose(flog);


   FreeMember();

   switch_on_pcbfiles_used_in_panels(doc, FALSE);

   if (display_error && !unicamPdwLogFile.IsEmpty())
      Logreader(unicamPdwLogFile);

   return;
}

void WritePCB(FILE* pFile, FileStruct *pcbFile)
{
   generate_PINLOC(doc, pcbFile, 0);     // this function generates the PINLOC argument for all pins.
   generate_PADSTACKACCESSFLAG(doc, 0);   // 

   padstackcnt = 0;
   do_padstacks(1.0);

   int headerattcnt = 0;

   fprintf(pFile, "%sboard %d\n", ident, 1);
   fprintf(pFile, "%s{\n", ident);
   plusident();
   fprintf(pFile, "%soutline\n", ident);
   fprintf(pFile, "%s{\n", ident);
   plusident();

   // all graphic structures 
   int found = UNICAM_WritePRIMARYBOARDData(pFile, &(pcbFile->getBlock()->getDataList()), 
         pcbFile->getInsertX(), pcbFile->getInsertY(), pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale(), 0, -1,
         GR_CLASS_BOARDOUTLINE, LAYTYPE_BOARD_OUTLINE);

   if (!found)
   {
         UNICAM_WriteBOARDData(pFile, &(pcbFile->getBlock()->getDataList()), pcbFile->getInsertX(), pcbFile->getInsertY(),
         pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale(), 0, -1, LAYTYPE_BOARD_OUTLINE);
   }

   fprintf(pFile, "\n");
   close_b(pFile);  // end outline

   // shapes - only PCB shapes
   do_pcbshapes(pFile, pcbFile->getScale());

   UNICAM_WriteCOMPONENTData(pFile, pcbFile, &(pcbFile->getBlock()->getDataList()), 
         pcbFile->getInsertX(), pcbFile->getInsertY(), pcbFile->getRotation(), pcbFile->isMirrored(), pcbFile->getScale(), 0, -1);

   fprintf(pFile, "%s%%netlist\n", ident);
   fprintf(pFile, "%s{\n", ident);
   plusident();
   
   do_routes(pFile, pcbFile, &pcbFile->getNetList());

   close_b(pFile);
   close_b(pFile);  // end board 
}

void WritePanel(FILE* pFile, FileStruct *panelFile)
{
   if (!panelFile->getBlock()->extentIsValid())
      doc->OnRegen();

   CExtent extent = panelFile->getBlock()->getExtent();

   fprintf(pFile, "%spanel %d\n", ident, 1);
   fprintf(pFile, "%s{\n", ident);
   plusident();
   fprintf(pFile, "%sparam\n", ident);
   fprintf(pFile, "%s{\n", ident);
   plusident();
   fprintf(pFile, "%ssizex : %ld\n", ident, cnv_unit(extent.getXsize()));
   fprintf(pFile, "%ssizey : %ld\n", ident, cnv_unit(extent.getYsize()));
   fprintf(pFile, "%sthick : %ld\n", ident, 0);
   fprintf(pFile, "%sjig   : \"%s\"\n", ident, "");
   close_b(pFile);

   fprintf(pFile, "%sboard\n", ident);
   fprintf(pFile, "%s{\n", ident);
   plusident();

   int boardCount = 0;

   POSITION pos = panelFile->getBlock()->getHeadDataInsertPosition();
   while (pos)
   {
      DataStruct *data = panelFile->getBlock()->getNextDataInsert(pos);
      InsertStruct *insert = data->getInsert();

      if (insert->getInsertType() == insertTypePcb)
      {
         fprintf(pFile, "%s%ld : {1, %ld, %ld, %.0f, 0, \"\"}", ident, ++boardCount,
            cnv_unit(insert->getOriginX() - extent.getXmin()), cnv_unit(insert->getOriginY() - extent.getYmin()), insert->getAngleDegrees() * 10);

         if (pos != NULL)
            fprintf(pFile, ",\n");
         else
            fprintf(pFile, "\n");
      }
   }

   close_b(pFile);
   close_b(pFile);   // end panel
}


/******************************************************************************
* InitializeMember
*/
static void InitializeMember()
{
   padstackarray.SetSize(100,100);
   padstackcnt = 0;

   clocationarray.SetSize(100,100);
   clocationcnt = 0;

   shapepinarray.SetSize(100,100);
   shapepincnt = 0;

   routesarray.SetSize(100,100);
   routescnt = 0;

   complistarray.SetSize(100,100);
   complistcnt = 0;
}

/******************************************************************************
* FreeMember
*/
static void FreeMember()
{
	int i=0;
   for (i=0; i<padstackcnt; i++)
      delete padstackarray[i];

   padstackarray.RemoveAll();
   padstackcnt = 0;

   for (i=0; i<clocationcnt; i++)
      delete clocationarray[i];

   clocationarray.RemoveAll();
   clocationcnt = 0;

   for (i=0; i<routescnt; i++)
      delete routesarray[i];

   routesarray.RemoveAll();
   routescnt = 0;

   for (i=0; i<shapepincnt; i++)
      delete shapepinarray[i];

   shapepinarray.RemoveAll();
   shapepincnt = 0;

   for (i=0; i<complistcnt; i++)
      delete complistarray[i];

   complistarray.RemoveAll();
}

/******************************************************************************
* clean_name
*/
static char *clean_name(const char *n)
{
   static CString tmp;
   tmp = "";

   for (int i=0; i<(int)strlen(n); i++)
   {
      if (n[i] == '"')
         tmp += "\\";
      else if (n[i] == '\\')
         tmp += "\\";

      tmp += n[i];
   }

   return tmp.GetBuffer(0);
}

/******************************************************************************
* plusident
*/
static int plusident()
{
   if (identcnt < 80)
   {
      ident[identcnt] = ' ';
      ident[++identcnt] = '\0';
   }
   else
   {
      fprintf(flog, "Error in plus ident\n");
      display_error++;

      return -1;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int   minusident()
{
   if (identcnt > 0)
   {
      ident[--identcnt] = '\0';
   }
   else
   {
      fprintf(flog, "Error in minus ident\n");
      display_error++;

      return -1;
   }

   return 1;
}

/******************************************************************************
* close_b
*/
static int close_b(FILE *fo)
{
   minusident(); 
   fprintf(fo,"%s}\n", ident);

   return 1;
}

/******************************************************************************
* cnv_unit
*/
static long cnv_unit(double x)
{
   // accuracy 0.000000047 FLT_EPSILON in float.h
   double a = dbunitround(x);
   long xx = round(a * unitsFactor);

   return xx;
}

//--------------------------------------------------------------
static int cnv_rot(double degrees)
{
   degrees = normalizeDegrees(degrees);

   return round(degrees*10);
}

//--------------------------------------------------------------
static int cnv_angle(double degrees)
{
   degrees = normalizeDegrees(degrees);

   return round(degrees*10);
}

/*****************************************************************************/
/*
*/
static int ignore_non_manufacturing_info(DataStruct *np)
{

   if (np->getDataType() != T_INSERT)
   {
      if (np->getGraphicClass() == GR_CLASS_ANTI_ETCH)          return TRUE;
      if (np->getGraphicClass() == graphicClassRouteKeepIn)         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)        return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)        return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)       return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPIN)          return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPIN)          return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPOUT)         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_UNROUTE)            return TRUE;
      if (np->getGraphicClass() == graphicClassTestPointKeepOut)  return TRUE;
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

/*****************************************************************************/
/*
*/
static int wUNICAM_linewidth(FILE *fp,int index)
{
   static int oldwidthindex;

   if (index > -1)   
   {
      if (oldwidthindex != index)
      {
         fprintf(fp,"%s[L:%ld]\n", ident, cnv_unit(doc->getWidthTable()[index]->getSizeA()));
      }
   }

   oldwidthindex = index;

   return 1;
}

/*****************************************************************************/
/*
*/
static int wUNICAM_layer(FILE *fp,int layer)
{
   static int oldlayer;

   if (layer > -1)
   {
      if (oldlayer != layer)
      {
         fprintf(fp,"%s[N:%d]\n", ident, layer);
      }
   }

   oldlayer = layer;

   return 1;
}

//--------------------------------------------------------------
static int wUNICAM_Line(FILE *fp, double x1, double y1, double bulge1, 
                                  double x2, double y2, double bulge2, int out,
                                  int widthindex, int layerstack)
{
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;

      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

      // make positive start angle.
      if (sa < 0) sa += PI2;

      if (out) fprintf(fp,",\n");

      wUNICAM_linewidth(fp, widthindex);
      wUNICAM_layer(fp, layerstack);

     fprintf(fp,"%sA { %ld, %ld, %ld, %ld, %d, %d}", ident,
         cnv_unit(cx-r), cnv_unit(cy-r), cnv_unit(cx+r), cnv_unit(cy+r),
         cnv_angle(RadToDeg(sa)), round(RadToDeg(da)*10));
   }
   else
   {
      if (x1 == x2 && y1 == y2)  return 0;   // eliminate  segments which are Zero;

      if (out) fprintf(fp,",\n");

      wUNICAM_linewidth(fp, widthindex);
      wUNICAM_layer(fp, layerstack);

      // preferred by UNICAM
      fprintf(fp,"%sPL {0,{ %ld, %ld},{ %ld, %ld}}", ident,
         cnv_unit(x1), cnv_unit(y1), cnv_unit(x2), cnv_unit(y2));
/*
      fprintf(fp,"%sL { %ld, %ld, %ld, %ld}", ident,
         cnv_unit(x1), cnv_unit(y1), cnv_unit(x2), cnv_unit(y2));
*/
   }  
   return 1;
}

//--------------------------------------------------------------
static int wUNICAM_Circle(FILE *fp, double x1, double y1, double radius, int out)
{
   if ((double)cnv_unit(radius) == 0)
      return 0;

   if (out) fprintf(fp,",\n");

   fprintf(fp,"%sC { %ld, %ld, %ld}", ident,
      cnv_unit(x1), cnv_unit(y1), cnv_unit(radius));

   return 1;
}

//--------------------------------------------------------------
static CString gen_string(const char *nn)
{
   CString  name, n;
   int      i, space = 0;

   n = nn;

   if (!namespace_allowed)
   {
      n.Replace(" ","_");
      n.Replace("\t","_");
   }

   name = "";

   for (i=0;i<(int)strlen(n);i++)
   {
      if (isspace(n[i]))
         space++;

      if (n[i] == '"')
         name += '\\';

      name += n[i];
   }

   if (space)
   {
      CString  tmp;

      tmp = '"';
      tmp += name;
      tmp += '"';
      name = tmp;
   }

   return name;
}

//--------------------------------------------------------------
static int complist_device_written(const char *geom, const char *device)
{
   int   i;

   for (i=0;i<complistcnt;i++)
   {
      if (complistarray[i]->geomname.Compare(geom) == 0 &&
          complistarray[i]->devicename.Compare(device) == 0)
      {
         complistarray[i]->devicefile_written = TRUE;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int load_UNICAMsettings(const CString fname, int page_units)
{
   FILE  *fp;
   char  line[127];
   char  *lp;

   fiducialname = "fiducial";
   panelfiducialname = "panelfid";
   testpinprefix = "tstpin_";
   defaultdrillsize = 0.01;
   defaultnopadsize = 0.01;
   convert_tp_comps = FALSE;
   testpointshapeprefix = "SHAPE_";
   update_device_attribute = FALSE;
   namespace_allowed = TRUE;
   write_artwork = TRUE;
   write_Fiducial_props = FALSE;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp,"UNICAM Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,127,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;

      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".FIDUCIALNAME"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            fiducialname = lp;
         }
         else if (!STRCMPI(lp,".PANELFIDUCIALNAME"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            panelfiducialname = lp;
         }
         else if (!STRCMPI(lp,".TESTPINPREFIX"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            testpinprefix = lp;
         }
         else if (!STRCMPI(lp,".CONVERT_TP_COMPS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               convert_tp_comps = TRUE;
         }
         else if (!STRCMPI(lp,".UPDATE_DEVICE_ATTRIBUTES"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               update_device_attribute = TRUE;
         }
         else if (!STRCMPI(lp,".NAMESPACE_ALLOWED"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (lp[0] == 'N' || lp[0] == 'n')
               namespace_allowed = FALSE;
         }
         else if (!STRCMPI(lp,".WRITE_ARTWORK"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (lp[0] == 'N' || lp[0] == 'n')
               write_artwork = FALSE;
         }
         else if (!STRCMPI(lp,".WRITE_FIDUCIAL_PROPS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               write_Fiducial_props = TRUE;
         }
         else if (!STRCMPI(lp,".OUTPUT_UNIT_ACCURACY"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            output_units_accuracy = atoi(lp);
         }
         else if (!STRCMPI(lp,".DEFAULTDRILLSIZE"))
         {
            char  c;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue;   // I = inches M = MM

            c = lp[0];

            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (c == 'm' || c == 'M')
            {  
               defaultdrillsize = atof(lp) * Units_Factor(UNIT_MM, page_units);
            }
            else
            {
               defaultdrillsize = atof(lp) * Units_Factor(UNIT_INCHES, page_units);
            }
         }
         else if (!STRCMPI(lp,".NO_PADSIZE"))
         {
            char  c;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue;   // I = inches M = MM

            c = lp[0];

            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (c == 'm' || c == 'M')
            {  
               defaultnopadsize = atof(lp) * Units_Factor(UNIT_MM, page_units);
            }
            else
            {
               defaultnopadsize = atof(lp) * Units_Factor(UNIT_INCHES, page_units);
            }
         }
      }
   }

/*
   if (update_device_attribute)
   {
      if (ErrorMessage("The .UPDATE_DEVICE_ATTRIBUTE command may change the CAMCAD database.\nDo you want to proceed?", "Update Attribute is not reversable", MB_YESNO | MB_DEFBUTTON2)!=IDYES)
      {
         update_device_attribute = FALSE;
      }
   }
*/
   fclose(fp);

   return 1;
}

/*****************************************************************************/
/*
*/
static int has_blank(const char *l)
{
   CString  tmp;
   tmp  = l;

   if (tmp.Find(" ") > -1)    return 1;

   if (tmp.Find("\t") > -1)   return 1;

   return 0;
}

//--------------------------------------------------------------
static void write_attributes(FILE *fp,CMapWordToPtr *map, 
                                const char *gencadname, int *attcnt )
{
   if (map == NULL)
      return;

   WORD keyword;
   void *voidPtr;
   Attrib   *a;
   int   gencadcnt = *attcnt;

   POSITION pos = map->GetStartPosition();
   while (pos != NULL)
   {
      map->GetNextAssoc(pos, keyword, voidPtr);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))   continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0))    continue;
      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))  continue;
      if (keyword == doc->IsKeyWord(BOARD_THICKNESS, 0)) continue;

      a = (Attrib*)voidPtr;

      if (a->isInherited()) continue;

      char *tok, *temp = STRDUP(get_attvalue_string(doc, a));

      if ((tok = strtok(temp,"\n")) == NULL)
      {
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"??\"\n", 
            gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out);  // multiple values are delimited by \n
      }
      else
      {
         while (tok)
         {
            fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
               gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n

            tok = strtok(NULL,"\n");
         }
      }

      free(temp);
   }

   *attcnt = gencadcnt;

   return;
}

/*****************************************************************************/
/*
   If a string contains a blank, it must be inside quotes.
   If a string contains a quote or a \, it must be preceeded by a \
*/
static const char *do_string(const char *t)
{
   static   CString  tmp;
   CString  t1;

   tmp = t;
   tmp.TrimLeft();
   tmp.TrimRight();

   if (tmp.Find("\\\"",0) > -1)
   {
      t1 = "";

      for (int i=0;i<(int)strlen(tmp);i++)
      {
         if (tmp[i] == '\\' || tmp[i] == '"')
            t1 += '\\';

         t1 += tmp[i];
      }

      tmp = t1;
   }

   // find can only do one character at a time
   if (tmp.Find(" ",0) > -1)
   {
      // put in quotes
      t1 = '"';
      t1 += tmp;
      t1 += '"';
      tmp = t1;
   }
   else if (tmp.Find("\t",0) > -1)
   {
      // put in quotes
      t1 = '"';
      t1 += tmp;
      t1 += '"';
      tmp = t1;
   }

   return tmp.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static int UNICAMPinNameCompareFunc( const void *arg1, const void *arg2 )
{
   UNICAMCompPinInst **a1, **a2;

   a1 = (UNICAMCompPinInst**)arg1;
   a2 = (UNICAMCompPinInst**)arg2;

   // order so that drill holes come in last.
   if ((*a1)->drill < (*a2)->drill)
      return -1;

   return compare_name((*a1)->pinname, (*a2)->pinname);
}

/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is 
   because of the 2 pass to get artwork info.
*/
static void UNICAM_SortPinData(const char *s)
{
   int   done = FALSE;
   int   sortcnt = 0;

   if (comppininstcnt < 2) return;

   qsort(comppininstarray.GetData(), comppininstcnt, sizeof(UNICAMCompPinInst *), UNICAMPinNameCompareFunc);

   return;
}

/******************************************************************************
* do_pcbshapes
*/
static int do_pcbshapes(FILE *fp, double scale)
{
   int shapeattcnt = 0;
   int pcnt;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;

      // create necessary aperture blocks
      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT || block->getBlockType() == BLOCKTYPE_TESTPOINT ||
          block->getBlockType() == BLOCKTYPE_FIDUCIAL     || block->getBlockType() == BLOCKTYPE_TOOLING ||
          block->getBlockType() == BLOCKTYPE_GENERICCOMPONENT /* case 1638, lack of GENERICCOMPOENT here caused cloc == NULL later */  )
      {
         doc->CalcBlockExtents(block);

         fprintf(fp, "%sshape \"%s\"\n", ident, clean_name(block->getName()));
         fprintf(fp, "%s{\n", ident);
         plusident();
         fprintf(fp, "%soutline\n", ident);
         fprintf(fp, "%s{\n", ident);
         plusident();

         UNICAMclocation *cloc = new UNICAMclocation;
         clocationarray.SetAtGrow(clocationcnt++, cloc);
         cloc->name = block->getName();
         cloc->block_num = block->getBlockNumber();
         cloc->xOffset = 0.0;
         cloc->yOffset = 0.0;

         comppininstarray.SetSize(100, 100);
         comppininstcnt = 0;

         if (!UNICAM_WriteSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1, cloc))
         {
            fprintf(flog, "No primary outline or Layertype [%s] found for Shape [%s]\n",
                  layerTypeToString(LAYTYPE_COMPONENTOUTLINE), block->getName());
            display_error++;

            // make an outline from Block Extents
            // What if block has no extents???
            if (block->getXmax() <= block->getXmin())
            {
               // Block has no X extents
               if (block->getYmax() <= block->getYmin())
               {
                  //block has no extents at all
                  fprintf(fp,"%sPL {0,{ -1, -1},{ -1, 1}},\n", ident);
                  fprintf(fp,"%sPL {0,{ -1, 1},{ 1, 1}},\n", ident);
                  fprintf(fp,"%sPL {0,{ 1, 1},{ 1, -1}},\n", ident);
                  fprintf(fp,"%sPL {0,{ 1, -1},{ -1, -1}}", ident);
               }
               else
               {
                  // block has no X extents
                  fprintf(fp, "%sPL {0,{-1, %ld},{1, %ld}},\n", 
                     ident, cnv_unit(block->getYmin()*scale), cnv_unit(block->getYmin()*scale));
                  fprintf(fp, "%sPL {0,{1, %ld},{1, %ld}},\n", 
                     ident, cnv_unit(block->getYmin()*scale), cnv_unit(block->getYmax()*scale));
                  fprintf(fp, "%sPL {0,{1, %ld},{-1, %ld}},\n", 
                     ident, cnv_unit(block->getYmax()*scale), cnv_unit(block->getYmax()*scale));
                  fprintf(fp, "%sPL {0,{-1, %ld},{-1, %ld}}", 
                     ident, cnv_unit(block->getYmax()*scale), cnv_unit(block->getYmin()*scale));
               }
            }
            else if (block->getYmax() <= block->getYmin())
            {
               // block has no Y extents
                  fprintf(fp, "%sPL {0,{%ld, -1},{%ld, 1}},\n", 
                     ident, cnv_unit(block->getXmin()*scale), cnv_unit(block->getXmin()*scale));
                  fprintf(fp, "%sPL {0,{%ld, 1},{%ld, 1}},\n", 
                     ident, cnv_unit(block->getXmin()*scale), cnv_unit(block->getXmax()*scale));
                  fprintf(fp, "%sPL {0,{%ld, 1},{%ld, -1}},\n", 
                     ident, cnv_unit(block->getXmax()*scale), cnv_unit(block->getXmax()*scale));
                  fprintf(fp, "%sPL {0,{%ld, -1},{%ld, -1}}", 
                     ident, cnv_unit(block->getXmax()*scale), cnv_unit(block->getXmin()*scale));
            }
            else
            {
               wUNICAM_Line(fp, block->getXmin()*scale, block->getYmin()*scale, 0, 
                              block->getXmax()*scale, block->getYmin()*scale, 0, 0, -1, -1);
               wUNICAM_Line(fp, block->getXmax()*scale, block->getYmin()*scale, 0, 
                              block->getXmax()*scale, block->getYmax()*scale, 0, 0, -1, -1);
               wUNICAM_Line(fp, block->getXmax()*scale, block->getYmax()*scale, 0, 
                              block->getXmin()*scale, block->getYmax()*scale, 0, 0, -1, -1);
               wUNICAM_Line(fp, block->getXmin()*scale, block->getYmax()*scale, 0, 
                              block->getXmin()*scale, block->getYmin()*scale, 0, 0, -1, -1);
            }
         }

         fprintf(fp, "\n");
         close_b(fp);

         UNICAM_SortPinData(block->getName());

         fprintf(fp, "%sterminals\n", ident);
         fprintf(fp, "%s{\n", ident);
         plusident();

         for (pcnt=0; pcnt<comppininstcnt; pcnt++)
         {
            if (pcnt)
               fprintf(fp, ",\n");

            UNICAMCompPinInst *compPin = comppininstarray[pcnt];
            CUnicamPadstack *padstack = padstackarray[get_padstackindex(compPin->padstackname)];

            UNICAMShapePin *p = new UNICAMShapePin;
            shapepinarray.SetAtGrow(shapepincnt++, p);  
            p->geomnum = block->getBlockNumber();
            p->index = pcnt+1;
            p->name = compPin->pinname;

            int ptyp;

            switch (padstack->getShapeType())
            {
            case apertureRound:   ptyp = 1;  break;
            case apertureSquare:  ptyp = 2;  break;
            default:              ptyp = 3;  break; 
            }

            double sa, sb, offx, offy, rot;

            if (padstack->typ & 1)
            {
               sa = padstack->xsizetop;
               sb = padstack->ysizetop;
               offx = padstack->xofftop;
               offy = padstack->yofftop;
               rot = RadToDeg(padstack->toprotation + compPin->rotation);
            }
            else if (padstack->typ & 2)
            {
               sa = padstack->xsizebot;
               sb = padstack->ysizebot;
               offx = padstack->xoffbot;
               offy = padstack->yoffbot;
               rot = RadToDeg(padstack->botrotation + compPin->rotation);
            }
            else
            {
               sa = 0.0;
               sb = 0.0;
               offx = padstack->xoffbot;
               offy = padstack->yoffbot;
               rot = 0.0;
            }

            rot = normalizeDegrees(rot);

            // padrotation adjust
            if (rot != 0)
            {
               // only 90 degree steps allowed;
               int tmpRot = round(rot / 90);
               for (int i=0; i<tmpRot; i++)
               {
						// Swap offsets
                  double tmp = offx;
                  offx = -offy;
                  offy = tmp;
               }

					// Case 1649
					// Circle and square only have A dimension, leave it alone.
					// If Rectangle, swap A and B dims if 90 or 270 rotation
					if (ptyp == 3 && (tmpRot == 1 || tmpRot == 3))
					{
						double tmp = sa;
						sa = sb;
						sb = tmp;
					}
            }


            // Update geometry offset to UNICAMclocation
            cloc->xOffset += (compPin->pinx+offx);
            cloc->yOffset += (compPin->piny+offy);

            // here need to write out by pinform 1..3 1=round, 2=square, 3=retcangle
            if (ptyp == 1)
            {
               fprintf(fp, "%sP1 {%d, %ld, %ld, %ld}", ident, pcnt+1, 
                  cnv_unit(compPin->pinx+offx), cnv_unit(compPin->piny+offy), cnv_unit(sa));
            }
            else if (ptyp == 2)
            {
               fprintf(fp, "%sP2 {%d, %ld, %ld, %ld}", ident, pcnt+1, 
                  cnv_unit(compPin->pinx+offx), cnv_unit(compPin->piny+offy), cnv_unit(sa));
            }
            else if (cnv_unit(sa) == 0)
            {
               if (cnv_unit(sb) == 0)
               {
                  fprintf(fp, "%sP3 {%d, %ld, %ld, 1, 1}",ident, pcnt+1, 
                     cnv_unit(compPin->pinx+offx), cnv_unit(compPin->piny+offy));
               }
               else
               {
                  fprintf(fp, "%sP3 {%d, %ld, %ld, 1, %ld}",ident, pcnt+1, 
                     cnv_unit(compPin->pinx+offx), cnv_unit(compPin->piny+offy), cnv_unit(sb));
               }
            }
            else if (cnv_unit(sb) == 0)
            {
               fprintf(fp, "%sP3 {%d, %ld, %ld, %ld, 1}",ident, pcnt+1, 
                  cnv_unit(compPin->pinx+offx), cnv_unit(compPin->piny+offy), cnv_unit(sa));
            }
            else 
            {
               fprintf(fp, "%sP3 {%d, %ld, %ld, %ld, %ld}",ident, pcnt+1, 
                  cnv_unit(compPin->pinx+offx), cnv_unit(compPin->piny+offy),  cnv_unit(sa), cnv_unit(sb));
            }
         }

         fprintf(fp, "\n");
         close_b(fp);

         if (comppininstcnt > 0)
         {
            cloc->xOffset /= comppininstcnt;
            cloc->yOffset /= comppininstcnt;
         }

         for (pcnt=0; pcnt<comppininstcnt; pcnt++)
            delete comppininstarray[pcnt];

         comppininstarray.RemoveAll();

         close_b(fp);
      }
   }

   return shapeattcnt;
}

//--------------------------------------------------------------
static int get_pinindex(int cptr, const char *p)
{
   int   pinnr;
   int   i;

   for (i=0;i<shapepincnt;i++)
   {
      UNICAMShapePin *pp = shapepinarray[i];

      if (shapepinarray[i]->geomnum == complistarray[cptr]->geomnum)
      {
         if (shapepinarray[i]->name.Compare(p) == 0)
            return shapepinarray[i]->index;
      }

   }

   pinnr = atoi(p);

   return pinnr;
}

//--------------------------------------------------------------
static int is_component(const char *c)
{
   int   i;

   for (i=0;i<complistcnt;i++)
   {
      if (complistarray[i]->compname.Compare(c) == 0)
         return i+1;
   }

   fprintf(flog,"Component [%s] not written in $COMPONENT section\n", c);
   display_error++;

   return 0;
}

/******************************************************************************
* do_routes
*/
static int do_routes(FILE *fp, FileStruct *file,CNetList *NetList)
{   
   UNICAM_GetROUTESData(&(file->getBlock()->getDataList()), 0, -1);

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

      fprintf(fp, "%snode \"%s\"\n", ident, clean_name(net->getNetName()));
      fprintf(fp, "%s{\n", ident);
      plusident();

      fprintf(fp, "%spins\n", ident);
      fprintf(fp, "%s{\n", ident);
      plusident();

      int out = 0;

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         int cptr;

         if (cptr = is_component(compPin->getRefDes()))   // check if is was not an converted FIDUCIAL or TOOL
         {
            if (out) 
               fprintf(fp, ",\n");

            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());

            int pp = 3;

            if (block)
            {
               int ptr = get_padstackindex(block->getName());

               if (padstackarray[ptr]->typ == 1)
               {
                  if (compPin->getMirror())
                     pp = 2;
                  else
                     pp = 1;
               }
               else if (padstackarray[ptr]->typ == 2)
               {
                  if (compPin->getMirror())
                     pp = 1;
                  else
                     pp = 2;
               }
            }

            int pinindex = get_pinindex(cptr-1, compPin->getPinName());
				CString pinNameStr;
				if (!compPin->getPinName().IsEmpty())
					pinNameStr.Format("\"%s\"", compPin->getPinName());

            fprintf(fp, "%s{ \"%s\", %d, %s, , %d, %d}", ident, 
					compPin->getRefDes(), pinindex, pinNameStr, pp ,pp);
            out++;
         }
      }

      fprintf(fp, "\n");
      close_b(fp);   // end pins

      // order by signal name
      fprintf(fp, "%svias\n", ident);
      fprintf(fp, "%s{\n", ident);
      plusident();

      UNICAM_WriteVIASData(fp,&(file->getBlock()->getDataList()), file->getInsertX(), file->getInsertY(),
         file->getRotation(), file->isMirrored(), file->getScale(), 0, -1, net->getNetName());

      fprintf(fp, "\n");
      close_b(fp);

      fprintf(fp, "%straces\n", ident);
      fprintf(fp, "%s{\n", ident);
      plusident();

      UNICAM_WriteROUTESData(fp,&(file->getBlock()->getDataList()), file->getInsertX(), file->getInsertY(),
         file->getRotation(), file->isMirrored(), file->getScale(), 0, -1, net->getNetName());

      fprintf(fp, "\n");
      close_b(fp);

      close_b(fp);
   }

   return 1;
}

/******************************************************************************
* get_defaultlayer
*/
static const char *get_defaultlayer(const char *l, int *ON)
{
/*
   int i;

   for (i=0;i<deflaycnt;i++)
   {
      if (deflay[i].in.Compare(l) == 0)
      {
         *ON = TRUE;
         return deflay[i].out;
      }
   }

   *ON = FALSE;
*/
   return l;
}

/****************************************************************************/
/*
*/
static int get_padstack(const char* p)
{
   int   i;

   for (i=0;i<padstackcnt;i++)
   {
      if (!STRCMPI(padstackarray[i]->name,p))
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static int check_devicefiles()
{
   int   i;

   for (i=0;i<complistcnt;i++)
   {
      if (complistarray[i]->devicefile_written) continue;

      fprintf(flog, "No device file created for Component [%s] Package [%s] Device [%s]\n",
         complistarray[i]->compname, complistarray[i]->geomname, complistarray[i]->devicename);

      display_error++;
   }

   return 1;
}

//--------------------------------------------------------------
int  UNICAM_WriteBOARDData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int layertype)
{
   Mat2x2      m;
   DataStruct *np;
   int         layer;
   int         out = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // this is done in PRIMARY...
      if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
         continue;

      if (np->getGraphicClass() == GR_CLASS_PANELOUTLINE)
         continue;

      if (np->getGraphicClass() == graphicClassBoardCutout)
         continue;

      if (np->getGraphicClass() == graphicClassPanelCutout)
         continue;

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

         // If segment has a netname, do not write
         // here check if ATTR_NETNAME == netname
         if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))
            continue;

         LayerStruct *ll = doc->FindLayer(layer);

         if (layertype && ll->getLayerType() == layertype) // only check for class if the layertype is not equal
            continue; // can done in PrimaryBoard
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
                     out += wUNICAM_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge, out, -1, -1);
                     p1 = p2;
                  }
               }
            }  // while
         }

         break;  // POLYSTRUCT
      case T_TEXT: 
         break;
      case T_INSERT:
            // no insert , graphic definitions are done in ARTWORKS
         break;
      } // end switch
   } // end UNICAM_WriteBOARDData */
   return out;
}

//--------------------------------------------------------------
int UNICAM_WritePRIMARYBOARDData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, 
      long graphic_class, int layertype)
{
   Mat2x2   m;
   DataStruct *np;
   int      layer;
   int      cutoutcnt = 0;
   int      found = 0;

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

         // If segment has a netname, do not write
         // here check if ATTR_NETNAME == netname
         if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))
            continue;

         LayerStruct *ll = doc->FindLayer(layer);

         if (layertype && ll->getLayerType() == layertype) // only check for class if the layertype is not equal
         {
            // if the layertype is defined and element has this layertype, do not check
            //int r = 0;
         }
         else
         {
            if (np->getGraphicClass() != graphic_class)
               continue;
         }
      }

      switch (np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            if (np->getGraphicClass() == graphicClassBoardCutout || 
                np->getGraphicClass() == graphicClassPanelCutout)
            {
               break;
            }

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (poly->isHidden()) continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;
               double   cx, cy, radius;

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  fprintf(wfp,"%sC {%ld, %ld, %ld}\n", ident, cnv_unit(cx), cnv_unit(cy), cnv_unit(radius));
               }
               else
               {  
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
                        found += wUNICAM_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge, found, -1, -1);
                        p1 = p2;
                     }
                  }
               } 
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
   } // end UNICAM_WritePRIMARYBOARDData */

   return found;
}

//--------------------------------------------------------------
static BlockStruct *get_blockpadname(CDataList *b)
{
   DataStruct *np;
   int   fidcnt = 0;
   BlockStruct *bb = NULL;

   POSITION pos = b->GetHeadPosition();
   while (pos != NULL)
   {
      np = b->GetNext(pos);
      
      if (np->getDataType() == T_INSERT)        
      {
         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if (block->getBlockType() == BLOCKTYPE_PADSTACK || 
             block->getBlockType() == BLOCKTYPE_PADSHAPE)
         {
            return block;
         }
         else
         {
            bb = get_blockpadname(&(block->getDataList()));
         }
      }
   }

   return bb;
}

/******************************************************************************
* UNICAM_WriteSHAPEData
*/
int UNICAM_WriteSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, UNICAMclocation *cloc)
{
   Point2 point2;
   int out = 0;
   Mat2x2 m;
   RotMat2(&m, rotation);
   bool firstPin = true;

   // Initialize cloc min and max to 0.0 to clear out garbage in case no pins on pcb component(I know, its not correct).
   cloc->xMax = 0.0;
   cloc->xMin = 0.0;
   cloc->yMax = 0.0;
   cloc->yMin = 0.0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      LayerStruct *layer = NULL;

      if (np->getDataType() != T_INSERT)
      {
         int layerIndex = np->getLayerIndex();

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layerIndex = insertLayer;

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layerIndex, mirror))
            continue;

         layer = doc->FindLayer(layerIndex);
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

               BOOL polyFilled = poly->isFilled();
               BOOL closed = poly->isClosed();

               int first = TRUE;
               Point2 firstp;
               Point2 p1;
               Point2 p2;

               //if (1/*np->getGraphicClass() == GR_CLASS_COMPOUTLINE || layer->getLayerType() == LAYTYPE_COMPONENTOUTLINE*/)  
               //{
               double cx, cy, radius;

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  Point2 p;
                  p.x = cx * scale;

                  if (mirror)
                     p.x = -p.x;

                  p.y = cy * scale;
                  radius = radius * scale;

                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  out += wUNICAM_Circle(wfp, p.x, p.y, radius, out);
               }
               else
               {  // not rectangle
                  POSITION pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntPos);

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
                        out += wUNICAM_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge, out, -1, -1);
                        p1 = p2;
                     }
                  }
               }
               //}
            } 
         }

         break;  // POLYSTRUCT
      case T_TEXT:

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

            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN || np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               CString padlayer = "ALL";
               CString pinname = np->getInsert()->getRefname();

               int pptr = get_padstack(block->getName());

               if (pptr > -1)
               {
                  padlayer = "TOP"; // this is not the PAD layer, but TOP means the pads are placed as
                                       // defined in the layers of the PADSTACK.
                                       // BOTTOM means like mirrored - see manual page 25.

                                       // this can be used for different pads on TOP or BOTTOM as in MENTOR.
               }

               // add pin locations to extents
               if (firstPin == true)
               {
                  cloc->xMin = np->getInsert()->getOriginX();
                  cloc->xMax = np->getInsert()->getOriginX();
                  cloc->yMin = np->getInsert()->getOriginY();
                  cloc->yMax = np->getInsert()->getOriginY();

                  firstPin = false;
               }

               if (np->getInsert()->getOriginX() < cloc->xMin)
                  cloc->xMin = np->getInsert()->getOriginX();

               if (np->getInsert()->getOriginX() > cloc->xMax)
                  cloc->xMax = np->getInsert()->getOriginX();

               if (np->getInsert()->getOriginY() < cloc->yMin)
                  cloc->yMin = np->getInsert()->getOriginY();

               if (np->getInsert()->getOriginY() > cloc->yMax)
                  cloc->yMax = np->getInsert()->getOriginY();

               UNICAMCompPinInst *c = new UNICAMCompPinInst;
               comppininstarray.SetAtGrow(comppininstcnt++, c);  
                  
               c->padstackname = block->getName();
               c->pinname = pinname;
               c->drill = FALSE;
               c->pinx = point2.x;
               c->piny = point2.y;
               c->rotation = block_rot;

               break;
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;

               if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               out += UNICAM_WriteSHAPEData(wfp, &(block->getDataList()), point2.x, point2.y, block_rot, block_mirror,
                               scale * np->getInsert()->getScale(), embeddedLevel+1, block_layer, cloc);
            } 
         } 

         break;                                                                
      } 
   } 

   return out;
}

//--------------------------------------------------------------
void UNICAM_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2      m;
   DataStruct  *np;
   int         layer;
   POSITION    pos;
   RotMat2(&m, rotation);
   int         out = 0;

   wUNICAM_layer(wfp,-1);  // init layer
   wUNICAM_linewidth(wfp,-1); // init layer

   for (int i=0;i<routescnt;i++)
   {
      pos = routesarray[i]->pos;

      // different netname
      if (strcmp(routesarray[i]->netname, netname))   continue; 

      np = DataList->GetNext(pos);  // modifies pos

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() == T_INSERT)
         continue;

      if (np->getGraphicClass() != graphicClassEtch)
         continue;

      if (insertLayer != -1)
         layer = insertLayer;
      else
         layer = np->getLayerIndex();

      // insert has not always a layer definition.
      if (!doc->get_layer_visible(layer, mirror))
         continue;

      LayerStruct *ll = doc->FindLayer(layer);

      if (ll->getElectricalStackNumber() == 0)
      {
         fprintf(flog,"Electrical Trace Layer [%s] has no Electrical Stackup Number -> ignored\n", 
            ll->getName());

         display_error++;

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

               if (poly->isHidden()) continue;

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
                     out += wUNICAM_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge, out, 
                        poly->getWidthIndex(), ll->getElectricalStackNumber());
                     p1 = p2;
                  }
               }
            }  // while
            // attributes
         }

         break;  // POLYSTRUCT
      case T_TEXT:
            // no text in route section
         break;         
      case T_INSERT:
         break;
      } // end switch
   } // end UNICAM_WriteROUTESData */
}

//--------------------------------------------------------------
// UNICAM_WritePADSTACK_Circle
static void UNICAM_WritePADSTACK_Circle(FILE *wfp, CUnicamPadstack *padstack, Point2 &location)
{
   double   radius = 0;
   if (padstack->typ & 1)
      radius = padstack->xsizetop;
   else if (padstack->typ & 2)
      radius = padstack->xsizebot;

   fprintf(wfp,"%s[V:%d,W:%d]\n", ident, padstack->typ, padstack->typ);

   if (radius == 0.0)
   {
      fprintf(wfp,"%sP1 {%d,%ld,%ld,1}", ident, ++viacnt, 
         cnv_unit(location.x), cnv_unit(location.y));
   }
   else
   {
      fprintf(wfp,"%sP1 {%d,%ld,%ld,%ld}", ident, ++viacnt, 
         cnv_unit(location.x), cnv_unit(location.y), cnv_unit(radius));
   }
}

//--------------------------------------------------------------
// UNICAM_WritePADSTACK_Rectangle
static void UNICAM_WritePADSTACK_Rectangle(FILE *wfp, CUnicamPadstack *padstack, Point2 &location, double insertAngle)
{
   double width = 0., height = 0.;
   if (padstack->typ & 1)
   {
      width = padstack->xsizetop;
      height = padstack->ysizetop;
   }
   else if (padstack->typ & 2)
   {
      width = padstack->xsizebot;
      height = padstack->ysizebot;
   }

   double rot = normalizeDegrees(round(RadToDeg(insertAngle)));
   if (round(rot/90) & 0x01)
      swap(width, height);

   fprintf(wfp,"%s[V:%d,W:%d]\n", ident, padstack->typ, padstack->typ);
   fprintf(wfp,"%sP3 {%d,%ld,%ld,%ld,%ld}", ident, ++viacnt, 
            cnv_unit(location.x), cnv_unit(location.y), cnv_unit(width), cnv_unit(height));
}

//--------------------------------------------------------------
// UNICAM_WritePADSTACK
static void UNICAM_WritePADSTACK(FILE *wfp, CUnicamPadstack *padstack, Point2 &location, double insertAngle)
{
   ApertureShapeTag shape = padstack->getShapeType();

   switch(shape)
   {
   case apertureSquare:
      padstack->ysizebot = padstack->xsizebot;
      padstack->ysizetop = padstack->xsizetop;
   case apertureRectangle:
      UNICAM_WritePADSTACK_Rectangle(wfp, padstack, location, insertAngle);
      break;
   default:
      UNICAM_WritePADSTACK_Circle(wfp, padstack, location);     
      break;
   }
}

//--------------------------------------------------------------
void UNICAM_WriteVIASData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;  
   int         first = TRUE;

   RotMat2(&m, rotation);

   for (int i=0;i<routescnt;i++)
   {
      pos = routesarray[i]->pos;

      // different netname
      if (strcmp(routesarray[i]->netname, netname))
         continue; 

      np = DataList->GetNext(pos);  // modifies pos

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
         continue;

      switch(np->getDataType())
      {
      case T_POLY:

         break;  // POLYSTRUCT
      case T_TEXT:
            // no text in route section
         break;         
      case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != INSERTTYPE_VIA 
               && !np->isInsertType(insertTypeBondPad))
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
            
            //Get padstack in bonpad block
            if(np->isInsertType(insertTypeBondPad))
               block = GetBondPadPadstackBlock(doc->getCamCadData(), block);

            if(!block) continue;

            if (!first) fprintf(wfp,",\n");

            int   ptr = get_padstackindex(block->getName());
            if(ptr == -1) continue;

            if(!padstackarray[ptr]->typ)
               padstackarray[ptr]->typ = UNICAM_GetPADSTACKData( &(block->getDataList()), np->getLayerIndex(), &padstackarray[ptr]->drill, scale, padstackarray[ptr]);

            UNICAM_WritePADSTACK(wfp, padstackarray[ptr], point2, block_rot);

            first = FALSE;
            // attrcibutes - do not write attributes on Vias
            // write_attributes(wfp, np->getAttributesRef(), "VIA", &viaattcnt);
         } // case INSERT
         break;
      } // end switch
   } // end UNICAM_WriteVIASData */

   return;
}

//--------------------------------------------------------------
void UNICAM_GetROUTESData(CDataList *DataList, int embeddedLevel, int insertLayer)
{
   DataStruct *np;
   int   layer;
   Attrib   *a;
   int      mirror = FALSE;

   POSITION pos1;
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      pos1 = pos; // pos is changed in getnext
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != graphicClassEtch)
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

         if (!a) continue;
      }
      else
      {
         // allow only vias and test points
         a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

         if (!a)  continue;
      }

      // here store it
      UNICAMRoutes *r = new UNICAMRoutes;
      routesarray.SetAtGrow(routescnt,r);  
      routescnt++;   
      r->netname = get_attvalue_string(doc, a);
      r->pos = pos1;
   } // end UNICAM_GetROUTESData */

   return;
}


/******************************************************************************
* getCLocation
*/
UNICAMclocation* getCLocation(int blockNumber)
{
   UNICAMclocation* retval = NULL;

   for (int i=0; i<clocationcnt; i++)
   {
      UNICAMclocation* cloc = clocationarray.GetAt(i);

      if (cloc != NULL && cloc->block_num == blockNumber)
      {
         retval = cloc;

         break;
      }
   }

   return retval;
}

//--------------------------------------------------------------
void UNICAM_WriteCOMPONENTData(FILE *wfp, FileStruct *file, CDataList *DataList, 
      double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   RotMat2(&m, rotation);

   

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* np = DataList->GetNext(pos);      
      if (np->getDataType() != T_INSERT)
         continue;

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            
			 // case dts0100396591 MN
			CString technology;
			CString unicamTechnology;
			Attrib *attrib;
			attrib = is_attvalue(doc, np->getAttributeMap(),ATT_TECHNOLOGY , 0);
			technology = attrib?attrib->getStringValue():"";
			
			// end
			 
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
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.
*/
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            UNICAMclocation* cloc = getCLocation(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || 
                np->getInsert()->getInsertType() == INSERTTYPE_TOOLING ||
                np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE ||
                np->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)
            {
               CString compname = np->getInsert()->getRefname();
               Attrib *a;

               CString  devicename;
               devicename = "";

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
               {
                  devicename = get_attvalue_string(doc, a);
               }
               else
               {
                  devicename = block->getName();
                  fprintf(flog,"Component [%s] has not TypeList (Device) entry\n", compname);
                  display_error++;
               }

               UNICAMCompList *c = new UNICAMCompList;
               complistarray.SetAtGrow(complistcnt,c);  
               complistcnt++;

               c->compname = compname;
               c->geomname = block->getName();
               c->geomnum  = block->getBlockNumber();
               c->devicefile_written = FALSE;
               c->devicename = devicename;

               fprintf(wfp,"%spart \"%s\"\n", ident, clean_name(compname));
               fprintf(wfp,"%s{\n", ident);
               plusident();

			   

               fprintf(wfp,"%stype       : \"%s\",\n", ident, 
                  np->getInsert()->getInsertType() == INSERTTYPE_TOOLING?"TOOLING_HOLE":clean_name(devicename));

               fprintf(wfp,"%sshapeid    : \"%s\",\n", ident, clean_name(block->getName()));
               fprintf(wfp,"%sinsclass   : \"\"\n",ident);  // insert class is not known
               fprintf(wfp,"%sgentype    : \"%s\"\n",ident, clean_name(block->getName()));
               fprintf(wfp,"%smachine    : \"\"\n",ident);
               fprintf(wfp,"%supdated    : 0,\n", ident);   // always 0
			   if (technology.MakeUpper() == "SMD")
					fprintf(wfp,"%s%%mtechn	 : 2,\n", ident);
			   else
					fprintf(wfp,"%s%%mtechn	 : 1,\n", ident);
			   

               int side = (np->getInsert()->getPlacedBottom()?1:0);

               if (np->getInsert()->getPlacedBottom() && !np->getInsert()->getMirrorFlags())
               {
                  fprintf(flog,"Part [%s] is placed on Bottom but not mirrored.\n", compname);
                  display_error++;
               }
               else if (!np->getInsert()->getPlacedBottom() && np->getInsert()->getMirrorFlags())
               {
                  fprintf(flog,"Part [%s] is mirrored bit not placed on Bottom.\n", compname);
                  display_error++;
               }
					
					// case 1750, the cloc that was being used here is wrong.
					// Get our own correct centroid for cloc, still switch on presence of
					// cloc, as it may be coordinating something.
               if (cloc != NULL) /* case 1638, cloc may be null */
               {
						// Case 1750, part 1, use centroid of pin locations
						double centx, centy;
						BlockStruct *shapeBlock = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
						CalculateCentroid_PinCenters(doc, shapeBlock, &centx, &centy);
						CPoint2d point(centx,centy);
                  ////CPoint2d point(cloc->xOffset, cloc->yOffset);  // case 1750, this cloc is in wrong place, it seems to be extent of block, we want extent of pin locations
                  
						CPoint2d placementPoint(point2.x, point2.y);
                  CTMatrix t;
                  t.scale(side?-1:1, 1);
                  t.rotateRadians(block_rot);
                  t.translate(placementPoint);
                  t.transform(point);

                  if (side)
                     block_rot = PI2 - block_rot;

                  fprintf(wfp,"%s%%clocation   : {%ld,%ld,%d,%d}\n", ident, 
                     cnv_unit(point.x), cnv_unit(point.y), cnv_rot(RadToDeg(block_rot)), side);
               }

               fprintf(wfp,"%slocation   : {%ld,%ld,%d,%d}\n", ident, 
                  cnv_unit(point2.x), cnv_unit(point2.y), cnv_rot(RadToDeg(block_rot)), side);

               close_b(wfp);

               break;
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
            {
               if (convert_tp_comps)   // if set to FALSE, a via record is generated.
               {
                  CString compname = np->getInsert()->getRefname();
                  Attrib *a;

                  fprintf(wfp,"COMPONENT %s\n", gen_string(compname));
                  fprintf(wfp,"PLACE %1.*lf %1.*lf\n",output_units_accuracy, point2.x, output_units_accuracy, point2.y);
         
                  if (block->getBlockType() == BLOCKTYPE_PADSTACK)
                  {
                     // BL_ACCESS_TOP              
                     // BL_ACCESS_BOTTOM           
                     // BL_ACCESS_OUTER            
                     // BL_ACCESS_NONE                
                     int   padlayertop = TRUE;
                     int   padlayerflip = 0;

                     if ((block->getFlags() & BL_ACCESS_OUTER) == BL_ACCESS_OUTER)
                     {
                        padlayertop = !(np->getInsert()->getPlacedBottom());
                        padlayerflip = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
                     }
                     else if (block->getFlags() & BL_ACCESS_NONE)
                     {
                        padlayertop = !(np->getInsert()->getPlacedBottom());
                        padlayerflip = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
                     }
                     else if (block->getFlags() & BL_ACCESS_TOP)
                     {
                        padlayertop = TRUE;
                        padlayerflip = FALSE;

                        // the idea is that the pseudo component should be placed on the layer the padstack is on
                        if (np->getInsert()->getMirrorFlags() & MIRROR_FLIP && np->getInsert()->getPlacedBottom())
                        {
                           padlayertop = FALSE;
                           padlayerflip = TRUE;
                        }
                        else if (np->getInsert()->getPlacedBottom())
                        {
                           padlayertop = FALSE;
                           padlayerflip = FALSE;
                        }
                     }
                     else if (block->getFlags() & BL_ACCESS_BOTTOM)
                     {
                        // the idea is that the pseudo component should be placed on the layer the padstack is on
                        if (np->getInsert()->getMirrorFlags() & MIRROR_FLIP && np->getInsert()->getPlacedBottom())
                        {
                           padlayertop = TRUE;
                           padlayerflip = TRUE;
                        }
                        else if (np->getInsert()->getPlacedBottom())
                        {
                           padlayertop =TRUE;
                           padlayerflip = FALSE;
                        }
                        else
                        {
                           // component is not mirrored, but padstack is on bottom
                           padlayertop = FALSE;
                           padlayerflip = FALSE;
                        }
                     }
                     else
                     {
                        // no special treatment
                        padlayertop = !(np->getInsert()->getPlacedBottom());
                        padlayerflip = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
                     }

                     fprintf(wfp,"LAYER %s\n",(!padlayertop)?"BOTTOM":"TOP");
                     fprintf(wfp,"ROTATION %1.2lf\n",RadToDeg(block_rot));

                     fprintf(wfp,"SHAPE %s%s %s\n",
                        testpointshapeprefix, gen_string(block->getName()), (padlayerflip)?"MIRRORY FLIP":"0 0");

                     fprintf(wfp,"DEVICE    %s%s\n", testpointshapeprefix, gen_string(block->getName()));
                  }
                  else
                  {
                     fprintf(wfp,"LAYER %s\n",(np->getInsert()->getPlacedBottom())?"BOTTOM":"TOP");
                     fprintf(wfp,"ROTATION %1.2lf\n",RadToDeg(block_rot));
                     fprintf(wfp,"SHAPE %s %s\n", gen_string(block->getName()), (block_mirror & MIRROR_FLIP)?"MIRRORY FLIP":"0 0");
                     fprintf(wfp,"DEVICE    %s\n", gen_string(block->getName()));
                  }


                  CString  devicename;
                  devicename = "";

                  if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
                  {
                     devicename = get_attvalue_string(doc, a);
                     fprintf(wfp,"DEVICE %s\n", gen_string(devicename));
                  }
   
                  UNICAMCompList *c = new UNICAMCompList;
                  complistarray.SetAtGrow(complistcnt,c);  
                  complistcnt++;

                  c->compname = compname;
                  c->geomname = block->getName();
                  c->devicefile_written = FALSE;
                  c->devicename = devicename;
               }

               break;
            }
            else if (np->getInsert()->getInsertType() == insertTypeFiducial)
            {
               CString fidName = np->getInsert()->getRefname();
               Attrib *a;

               CString  devicename;
               devicename = "";

               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
               {
                  devicename = get_attvalue_string(doc, a);
               }
               else
               {
                  devicename = block->getName();
                  fprintf(flog,"Fiducial [%s] has not TypeList (Device) entry\n", fidName);
                  display_error++;
               }
            
               fprintf(wfp,"%sfiducial \"%s\"\n", ident, clean_name(fidName));
               fprintf(wfp,"%s{\n", ident);
               plusident();

               if (write_Fiducial_props == TRUE)
               {
                  fprintf(wfp,"%sf_class    : 2,\n",ident);
                  fprintf(wfp,"%sf_type    : 1,\n",ident);
                  fprintf(wfp,"%sf_sel    : 2,\n",ident);
               }

               fprintf(wfp,"%stype       : \"%s\",\n", ident, clean_name(devicename));
               fprintf(wfp,"%sshapeid    : \"%s\",\n", ident, clean_name(block->getName()));
               fprintf(wfp,"%smachine    : \"\"\n",ident);
			   //fprintf(wfp,"%s%%mtechn	 : \"%s\"\n", ident, unicamTechnology);


               int side = (np->getInsert()->getPlacedBottom()?1:0);

               if (np->getInsert()->getPlacedBottom() && !np->getInsert()->getMirrorFlags())
               {
                  fprintf(flog,"Fiducial [%s] is placed on Bottom but not mirrored.\n", fidName);
                  display_error++;
               }
               else if (!np->getInsert()->getPlacedBottom() && np->getInsert()->getMirrorFlags())
               {
                  fprintf(flog,"Fiducial [%s] is mirrored but not placed on Bottom.\n", fidName);
                  display_error++;
               }

               if (cloc != NULL)
					{
//                CPoint2d point = block->getPinCentroid(),
                  CPoint2d point(cloc->xOffset, cloc->yOffset);
                  CPoint2d placementPoint(point2.x, point2.y);
                  CTMatrix t;
                  t.scale(side?-1:1, 1);
                  t.rotateRadians(block_rot);
                  t.translate(placementPoint);
                  t.transform(point);

                  if (side)
                     block_rot = PI2 - block_rot;

                  fprintf(wfp,"%s%%clocation   : {%ld,%ld,%d,%d}\n", ident, 
                     cnv_unit(point.x), cnv_unit(point.y), cnv_rot(RadToDeg(block_rot)), side);
               }

               fprintf(wfp,"%slocation   : {%ld,%ld,%d,%d}\n", ident, 
                  cnv_unit(point2.x), cnv_unit(point2.y), cnv_rot(RadToDeg(block_rot)), side);

               close_b(wfp);

               break;
            }              
         } // case INSERT

         break;
      } // end switch
   } // end UNICAM_WriteCOMPONENTData */

   return;
}

/******************************************************************************
* UNICAM_GetPADSTACKData
   return   0x1 top
            0x2 bottom
            0x4 drill
*/
static int UNICAM_GetPADSTACKData(CDataList *DataList, int insertLayer, double *drill, double scale, CUnicamPadstack *padstack)
{
   int layer;
   int typ = 0;   

   *drill = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *np = DataList->GetNext(pos);

      if (np->getGraphicClass() == graphicClassEtch)
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1  || doc->IsFloatingLayer(layer))
            layer = insertLayer;
         else
            layer = np->getLayerIndex();
      }

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;

            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA || np->isInsertType(insertTypeBondPad))
               break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_TOOL))
            {
               *drill = block->getToolSize() * scale;
            }
            else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               layer = np->getLayerIndex();

               if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
               {
                  // block_layer is not set.
                  if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     if (block->getShape() == apertureComplex)
                        block = doc->Find_Block_by_Num((int)block->getSizeA());

                     layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
                  }
               }
               
               if (insertLayer != -1 && doc->IsFloatingLayer(layer))
               {
                  layer = insertLayer;
               }

               if (layer == -1)
                  layer = insertLayer;

               if (layer < 0)
                  break;

               LayerStruct *l = doc->getLayerArray()[layer];

               doc->CalcBlockExtents(block);

               CPoint2d point = block->getExtent().getCenter();
               CPoint2d placementPoint(np->getInsert()->getOrigin2d());
               CTMatrix t;
               t.scale(np->getInsert()->getGraphicMirrored()?-1:1, 1);
               t.rotateRadians(np->getInsert()->getAngleRadians());
               t.translate(placementPoint);
               t.transform(point);
               
               ApertureShapeTag shape = block->getShape();
               double sizeA = block->getXmax() - block->getXmin();
               double sizeB = block->getYmax() - block->getYmin();                  
               double Xoffset = point.x;
               double Yoffset = point.y;

               // Case #1649
               if (shape == apertureComplex)
               {
                  BlockStruct* subBlock = doc->getBlockAt(block->getComplexApertureSubBlockNumber());

                  if (subBlock->getDataList().GetCount() == 1)
                  {
                     DataStruct* polyStruct = subBlock->getDataList().GetHead();

                     if (polyStruct->getDataType() == dataTypePoly)
                     {
                        if (polyStruct->getPolyList()->GetCount() == 1)
                        {
                           CPoly* poly = polyStruct->getPolyList()->GetHead();

                           if (poly->isFilled() && poly->isClosed())
                           {
                              double cx,cy,radius;

                              if (PolyIsCircle(poly,&cx,&cy,&radius))
                              {
                                 int overWriteError;

                                 int widthIndex = Graph_Aperture("",apertureRound,2.*radius,0.,0.,0.,0.,0,0,false,&overWriteError);
                                 block = doc->getWidthBlock(widthIndex);
                                 shape = block->getShape();
                                 sizeA = block->getSizeA();
                                 sizeB = block->getSizeB();
                                 Xoffset += cx;
                                 Yoffset += cy;

                                 double tolerance = doc->convertToPageUnits(pageUnitsMils,.05);

                                 if (fpnear(Xoffset,0.,tolerance)) Xoffset = 0.;
                                 if (fpnear(Yoffset,0.,tolerance)) Yoffset = 0.;
                              }
                           }
                        }
                     }
                  }
               }

               // only evaluate this type of apertures;
               if (shape != apertureRound   && shape != apertureSquare && shape != apertureRectangle &&
                   shape != apertureOctagon && shape != apertureOblong && shape != apertureDonut        )   
               {
                  // complex aperture not evaluated in DRC - get padstack ???
                  shape   = apertureRectangle;
               }
         
               if (l->getLayerType() == LAYTYPE_PAD_TOP || l->getLayerType() == LAYTYPE_SIGNAL_TOP)
               {
                  if (l->getNeverMirror()) 
                     typ |= 0x8;
                  else
                     typ |= 0x1;

                  padstack->setShapeTypeTop(shape);
                  padstack->xsizetop = sizeA;
                  padstack->ysizetop = sizeB;
                  padstack->xofftop = Xoffset;
                  padstack->yofftop = Yoffset;
                  padstack->toprotation = np->getInsert()->getAngle();
               }
               else if (l->getLayerType() == LAYTYPE_PAD_BOTTOM || l->getLayerType() == LAYTYPE_SIGNAL_BOT)
               {

                  if (l->getMirrorOnly())  
                     typ |= 0x10;
                  else
                     typ |= 0x2;

                  padstack->setShapeTypeBottom(shape);
                  padstack->xsizebot = sizeA;
                  padstack->ysizebot = sizeB; 
                  padstack->xoffbot = Xoffset;
                  padstack->yoffbot = Yoffset;
                  padstack->botrotation = np->getInsert()->getAngle();
               }
               else if (l->getLayerType() == LAYTYPE_PAD_ALL || l->getLayerType() == LAYTYPE_SIGNAL_ALL || l->getLayerType() == LAYTYPE_PAD_OUTER)
               {
                  typ |= 0x3;
                  padstack->setShapeTypeTop(shape);
                  padstack->xsizetop = sizeA;
                  padstack->ysizetop = sizeB;
                  padstack->xofftop = Xoffset;
                  padstack->yofftop = Yoffset;
                  padstack->toprotation = np->getInsert()->getAngle();

                  padstack->setShapeTypeBottom(shape);
                  padstack->xsizebot = sizeA;
                  padstack->ysizebot = sizeB;
                  padstack->xoffbot = Xoffset;
                  padstack->yoffbot = Yoffset;
                  padstack->botrotation = np->getInsert()->getAngle();
               }
            }
         }

         break;
      } 
   }

   return typ;
} 

/******************************************************************************
* do_padstacks
*/
 static int do_padstacks(double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         double drillsize;
         CUnicamPadstack *c = new CUnicamPadstack;
         padstackarray.SetAtGrow(padstackcnt++, c);  
         c->name = block->getName();
         c->block_num = block->getBlockNumber();

         int typ = UNICAM_GetPADSTACKData( &(block->getDataList()), -1, &drillsize, scale, c);
         c->typ = typ;
         c->drill = drillsize;
      }
   }

   return 1;
}

/******************************************************************************
* get_padstackindex
*/
static int get_padstackindex(const char *p)
{
   for (int i=0; i<padstackcnt; i++)
   {
      if (!padstackarray[i]->name.CompareNoCase(p))
         return i;
   }

   fprintf(flog, "Padstack [%s] not found in index\n", p);
   display_error++;

   return -1;
}

//_____________________________________________________________________________
CUnicamPadstack::CUnicamPadstack()
: m_shapeTypeTop(apertureUndefined)
, m_shapeTypeBottom(apertureUndefined)
{
}

ApertureShapeTag CUnicamPadstack::getShapeType() const
{
   ApertureShapeTag retval = m_shapeTypeBottom;

   if (retval == apertureUndefined)
   {
      retval = m_shapeTypeTop;
   }

   return retval;
}

