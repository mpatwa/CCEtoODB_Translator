
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
#include "padslib.h"
#include <math.h>
#include "ck.h"
#include "pcblayer.h"
#include "pcb_net.h"
#include "polylib.h"
#include "graph.h"
#include "pads_out.h"
#include "Apertur2.h"
#include "CamCadDatabase.h"
#include "Layer.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// dump for test the trace function.
//#define   DUMP
#undef DUMP

#define PadsMinTraceWidthInMils (.01) 
#define PadsMaxTraceWidthInMils (250.) 

//extern LayerTypeInfoStruct layertypes[];
extern CProgressDlg  *progress;
extern CViaArray *viaArray;         // from PCB_NET.CPP
extern CSegmentArray *segmentArray; // from PCB_NET.CPP
CString pads_layer[MAX_PADSLAYER]; // used in pcblayer.cpp

// from CamCadDatabase.cpp
void convertSmdViasToTestPoints(CCamCadDatabase& camCadDatabase);

/* Static Variables *********************************************************/
static CCEtoODBDoc          *doc;
static int                 page_units;
static FILE                *flog;
static int                 display_error;

// this seems to work for INCH/MIL combination
static int                 output_units_accuracy = 4; // attention that this matches DECAL section defined in MILS
static double              SMALLDELTA = 0.0001;
static double              onemil;
static int                 maxArr = 0;
static int                 max_signalcnt;

static CPADSLayerArray     padsArr;

static DecalPinNameArray   decalPinNameArray;   //every decal is referenced by the CC blocknum and every pin is

static CLayerMapArray      layermaparray;
static int                 layermapcnt = 0;

static CConnectLinkArray   connectlinkarray;
static int                 connectlinkcnt = 0;

static PadStackArray       padstackarray;
static int                 padstackcnt;

static PadShapeArray       padshapearray; // this is for padshapes on *VIA and *DECAL
static int                 padshapecnt;

static TerminalArray       terminalarray; // this is for padstacks in partdecal
static int                 terminalcnt;

static PartTypeArray       parttypearray; // this is for padstacks in partdecal
static int                 parttypecnt;

static ViaArray            viaarray;   // used via definitions
static int                 viacnt;

static ComplexErrorArray   complexerrorarray;   // used via definitions
static int                 complexerrorcnt;

static TraceViaArray       traceviaarray; // vias per net
static int                 traceviacnt;

static TracePinArray       tracepinarray; // pins per net
static int                 tracepincnt;

static int                 pourcnt = 0;
static bool                optionTranslateHatchLines = false;
static bool                optionWritePourOutOnly    = false;
static bool                optionColorAllLayers      = false;
static bool                optionAddPlaneTie         = false;

/* Function Prototypes *********************************************************/

static void PADS_WriteData(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror,
            double scale, double padsUnitsPerPageUnit, int insertLayer);
static void PADS_WriteSchematicData(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
            int mirror, double scale, double padsUnitsPerPageUnit, int insertLayer);
static int PADS_GetPADSTACKData(const char *name, CDataList *DataList, int insertLayer, double *drill, BOOL *plated,
            double scale, double rotation, int blind, int *startlayer, int *endlayer, int mirror = 0);
static int load_PADSsettings(const CString fname);
//static int get_layertype(const char *l);
static void clean_polys(CDataList *DataList, double small_element);
static void do_layerlist();
static int edit_layerlist();
static void get_vianames(CDataList *DataList);
static int do_pcbsection(FILE *fp, int units, int maxsiglayer, BlockStruct *fileblock, double insert_x, double insert_y,
            double rotation, int mirror, double scale, int insertLevel);
static int do_textsection(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
            int mirror, double scale, int insertLevel);
static int do_linesection(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
            int mirror, double scale, double padsUnitsPerPageUnit, int insertLevel);
static int do_viasection(FILE *fp, double scale);
//static int DoViaSection(FILE *fp, double scale);
static int GetViaPadstack(BlockStruct *padstack, int maxElectricalStackNum, double scale, double &drillSize,
            int &startLayerNum, int &endLayerNum, BOOL &blind, CMapStringToPtr *padstackMap);
static void WriteBlinBuriedPadstack(FILE *fp,CString viaName, double drillSize, int maxElectricalStackNum,
            int startLayerNum, int endLayerNum, CMapStringToPtr *padstackMap);
static void WriteThroughtHolePadstack(FILE *fp,CString viaName, double drillSize, int maxElectricalStackNum,
            CMapStringToPtr *padstackMap);
static int do_partdecalsection(FILE *fp, double scale, double padsUnitsPerPageUnit);
static int do_parttypesection(FILE *fp, CTypeList *TypeList, double scale);
static int do_partsection(FILE *fp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
            double rotation, int mirror, double scale, int insertLevel);
static int do_routesection(FILE *wfp, FileStruct *file, double unitsFactor, double default_linewidth);
static int do_poursection(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
            int mirror, double scale, double padsUnitsPerPageUnit, int insertLevel);
static int do_misc(FILE *fp, CDataList *DataList, const char *padsunit);
static void free_layerlist();
static void ExplodeLayerDependentComponents(CCEtoODBDoc *doc);
static BOOL ExploreBlock(CCEtoODBDoc *doc, FileStruct *file, BlockStruct *block, CMapStringToString &layerToTypeMap,
            CMapStringToString &mirrorGeomMap, CMapStringToString &exploredGeomMap);
static BOOL IsLayerDependent(int layerNum, CMapStringToString &layerToTypeMap, CString &layerType);
static void DuplicateDeviceType(FileStruct *file, CMapStringToString &mirrorGeomMap);
static void SetToOppositeLayerType(CMapStringToString &layerToTypeMap);
static void ConvertIllegalApertureToComplex(CCEtoODBDoc *doc);
static int save_padshape(BlockStruct *b, int level, double size, double finlength, double finoffset, double finori,
            const char *padshape, double idia);
static bool warnAboutPageSize();

static double limitPadsWidthInMils(double widthInMils);
static double getLimitedPadsWidth(double widthInPageUnits,double padsUnitsPerPageUnit,bool& limitExceededFlag);
static double getLimitedPadsWidth(double widthInPageUnits,double padsUnitsPerPageUnit);

static CUnits& getUnits()
{
   static CUnits s_units(pageUnitsMils);

   return s_units;
}

static CString getPlaneTieClause(bool planeTieFlag);
static CString getPlaneTieClause();

/******************************************************************************
* PADS_WriteFiles
*/
void PADS_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int pageunits)
{
   getUnits().setPageUnits(Doc->getPageUnits());

   FILE *wfp;
   double padsUnitsPerPageUnit = Units_Factor(pageunits, UNIT_INCHES) * format->Scale;
   char *padsunit = "INCHES";
   double default_linewidth;

   page_units = pageunits;
   doc = Doc;
   display_error = FALSE;

   //2=Inches 1=Metric 0=Mils
   if (format->PortFileUnits == 1)  // 2=Inch, 1=mm, 0=Mils
   {
      padsUnitsPerPageUnit = Units_Factor(pageunits, UNIT_MM) * format->Scale;
      output_units_accuracy = 2; // attention that this matches DECAL section defined in MILS
      SMALLDELTA = 0.01;
      padsunit = "METRIC";
      default_linewidth = 0.3;
      onemil = Units_Factor(UNIT_MILS, UNIT_MM);
   }
   else if (format->PortFileUnits == 0)   // 2=Inch, 1=mm, 0=Mils
   {
      padsUnitsPerPageUnit = Units_Factor(pageunits, UNIT_MILS) * format->Scale;
      output_units_accuracy = 1; // attention that this matches DECAL section defined in MILS
      SMALLDELTA = 0.1;
      padsunit = "MILS";
      default_linewidth = 12;
      onemil = 1;
   }
   else
   {
      padsUnitsPerPageUnit = Units_Factor(pageunits, UNIT_INCHES) * format->Scale;
      output_units_accuracy = 4; // attention that this matches DECAL section defined in MILS
      SMALLDELTA = 0.0001;
      padsunit = "INCHES";
      default_linewidth = 0.012;
      onemil = 0.001;
   }

   if (!warnAboutPageSize())
      return;

   // open file for writing
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      ErrorMessage( "Can not open file !", filename, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString logFile = GetLogfilePath("pads.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage( "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   padsArr.SetSize(10, 10);
   maxArr = 0;

   for (int p=0; p<MAX_PADSLAYER; p++)
   {
      pads_layer[p].Format("Layer_%d", p+1);
   }

   layermaparray.SetSize(10,10);
   layermapcnt = 0;

   CString settingsFile( getApp().getExportSettingsFilePath("pads.out") );
   load_PADSsettings(settingsFile);

   CString checkFile( getApp().getSystemSettingsFilePath("pads.chk") );
   check_init(checkFile);

   // Call this function to make sure that everything pin on a geometry has a pin number
   GeneratePinNumbers(doc);
   ExplodeLayerDependentComponents(doc);
   ConvertIllegalApertureToComplex(doc);

   // Convert all SMD via to test point prior to export
   CCamCadDatabase camCadDatabase(*Doc);
   convertSmdViasToTestPoints(camCadDatabase);

   wPADS_Graph_Init(output_units_accuracy , SMALLDELTA);
   if (wPADS_Graph_File_Open(wfp, 3, padsunit ))
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         if (!file->isShown())
            continue;

         if (file->getBlock() == NULL)
            continue;   // empty file

         GenerateMissingTypes(doc, file);

         do_layerlist();

         if (edit_layerlist())
         {
            CWaitCursor wait;

            if (file->getBlockType() == BLOCKTYPE_PCB)
            {
               padshapearray.SetSize(10,10);
               padshapecnt = 0;

               parttypearray.SetSize(10,10);
               parttypecnt = 0;

               decalPinNameArray.SetSize(0,1000);

               terminalarray.SetSize(10,10);
               terminalcnt = 0;

               padstackarray.SetSize(10,10);
               padstackcnt = 0;

               viaarray.SetSize(10,10);
               viacnt = 0;

               complexerrorarray.SetSize(10,10);
               complexerrorcnt = 0;

               traceviaarray.SetSize(10,10);
               traceviacnt = 0;

               tracepinarray.SetSize(10,10);
               tracepincnt = 0;

               connectlinkarray.SetSize(10,10);
               connectlinkcnt = 0;

               generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.
               EliminateSinglePointPolys(doc);

               get_vianames(&(file->getBlock()->getDataList()));
					int i=0;
               for (i=0; i<doc->getMaxBlockIndex(); i++)
               {
                  BlockStruct *block = doc->getBlockAt(i);
                  if (block == NULL)   continue;
                  if (block->getFlags() & BL_WIDTH)            continue;
                  if (block->getFlags() & BL_TOOL)             continue;
                  if (block->getFlags() & BL_APERTURE)         continue;
                  if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
                  BreakSpikePolys(block);


// clean_polys(&block->getDataList(), 0.0001); Removed 3/15/02 DD
// needed to remove because eliminated a point in a poly that hit a pin, and thus lost pin in do_routesection()
// consider using EliminateSinglePointPolys(), BreakSpikePolys(), Crack(), EliminateOverlappingTraces()
               }

/*
               netnamearray.SetSize(100,100);
               netnamecnt = 0;

               do_padstacks(file->getScale());
               get_netnames(&(file->getNetList()));
*/
               do_pcbsection(wfp, format->PortFileUnits, max_signalcnt, file->getBlock(), file->getInsertX() * padsUnitsPerPageUnit,
                     file->getInsertY() * padsUnitsPerPageUnit, file->getRotation(), file->isMirrored(), file->getScale() * padsUnitsPerPageUnit, -1);

               // all free text
               do_textsection(wfp, &file->getBlock()->getDataList(), file->getInsertX() * padsUnitsPerPageUnit, file->getInsertY() * padsUnitsPerPageUnit,
                     file->getRotation(), file->isMirrored(), file->getScale() * padsUnitsPerPageUnit, -1);

               // boardoutline, 2dlines, copper areas without boundary and void.
               do_linesection(wfp, &file->getBlock()->getDataList(), file->getInsertX() * padsUnitsPerPageUnit, file->getInsertY() * padsUnitsPerPageUnit,
                     file->getRotation(), file->isMirrored(), file->getScale(),padsUnitsPerPageUnit, -1);

               // do_clustersection
               do_viasection(wfp, padsUnitsPerPageUnit);
            //DoViaSection(wfp, padsUnitsPerPageUnit);

               // if here is a change, also make the round change in pegt_padform.
               // do not change or also fix 500/1000 save_pad size check

               double oldonemil = onemil;
               double oldsmalldelta = SMALLDELTA;
               int old_output_units_accuracy = output_units_accuracy;

               onemil = 1;
               SMALLDELTA = 0.1;
               output_units_accuracy = 2; // inch vs. mil (1/100 of a mil)

               wPADS_Graph_Units(output_units_accuracy , SMALLDELTA);
               do_partdecalsection(wfp, format->Scale, Units_Factor(pageunits, UNIT_MILS));  // all block definitions. Decal must be in MIL !!!

               onemil = oldonemil;
               SMALLDELTA = oldsmalldelta;
               output_units_accuracy = old_output_units_accuracy;

               wPADS_Graph_Units(output_units_accuracy , SMALLDELTA);

               // do_parttype          // typelist
               do_parttypesection(wfp, &file->getTypeList(), padsUnitsPerPageUnit);   // all block definitions

               // all component placement
               do_partsection(wfp, file, &file->getBlock()->getDataList(), file->getInsertX() * padsUnitsPerPageUnit, file->getInsertY() * padsUnitsPerPageUnit,
                     file->getRotation(), file->isMirrored(), file->getScale() * padsUnitsPerPageUnit, -1);

               // do_route             // all routing
               do_routesection(wfp, file, padsUnitsPerPageUnit, default_linewidth);

               // do_pour              // fill areas
               do_poursection(wfp, &file->getBlock()->getDataList(), file->getInsertX() * padsUnitsPerPageUnit, file->getInsertY() * padsUnitsPerPageUnit,
                     file->getRotation(), file->isMirrored(), file->getScale(),padsUnitsPerPageUnit, -1);

               // do_testpoints
               // do_misc
               do_misc(wfp, &(file->getBlock()->getDataList()), padsunit);

               for (i=0;i<padstackcnt;i++)
               {
                  delete padstackarray[i];
               }
               padstackarray.RemoveAll();

               for (i=0;i<parttypecnt;i++)
               {
                  delete parttypearray[i];
               }
               parttypearray.RemoveAll();

               for (i=0;i<viacnt;i++)
               {
                  delete viaarray[i];
               }
               viaarray.RemoveAll();

               for (i=0;i<complexerrorcnt;i++)
               {
                  delete complexerrorarray[i];
               }
               complexerrorarray.RemoveAll();

               for (i=0;i<decalPinNameArray.GetSize();i++)
               {
                  delete decalPinNameArray[i];
               }
               decalPinNameArray.RemoveAll();

               // the contents is removed in WriteTrace
               traceviaarray.RemoveAll();
               tracepinarray.RemoveAll();
               connectlinkarray.RemoveAll();
               // the contents is removed in do_via and do_decal
               padshapearray.RemoveAll();

               // the contents is removed in do_partdecal
               terminalarray.RemoveAll();

/*

               for (i=0;i<netnamecnt;i++)
               {
                  delete netnamearray[i];
               }
               netnamearray.RemoveAll();
               netnamecnt = 0;
*/
            }
            else
            {
               PADS_WriteData(wfp, &(file->getBlock()->getDataList()),
                  file->getInsertX() * format->Scale,
                  file->getInsertY() * format->Scale,
                  file->getRotation(), file->isMirrored(), file->getScale(), padsUnitsPerPageUnit, -1);
            }
         } // edit_layer
         free_layerlist();
      }
   }  // PADS output version OK.

   wPADS_Graph_File_Close();

   // close write file
   fclose(wfp);

   if (check_report(flog)) display_error++;
   check_deinit();

   fclose(flog);

   int i;
   for (i=0;i<layermapcnt;i++)
   {
      delete layermaparray[i];
   }
   layermaparray.RemoveAll();

   if (display_error)
      Logreader(logFile);
}

CString getPlaneTieClause(bool planeTieFlag)
{
   CString retval(planeTieFlag ? " THERMAL" : "");
   
   return retval;
}

CString getPlaneTieClause()
{
   return getPlaneTieClause(optionAddPlaneTie);
}

double limitPadsWidthInMils(double widthInMils)
{
   if (widthInMils < PadsMinTraceWidthInMils)
   {
      widthInMils = PadsMinTraceWidthInMils;
   }
   else if (widthInMils > PadsMaxTraceWidthInMils)
   {
      widthInMils = PadsMaxTraceWidthInMils;
   }

   return widthInMils;
}

double getLimitedPadsWidth(double widthInPageUnits,double padsUnitsPerPageUnit,bool& limitExceededFlag)
{
   double widthInMils      = getUnits().convertTo(pageUnitsMils,widthInPageUnits);
   double widthInPadsUnits = widthInPageUnits * padsUnitsPerPageUnit;

   limitExceededFlag = (widthInMils < PadsMinTraceWidthInMils || widthInMils > PadsMaxTraceWidthInMils);

   if (limitExceededFlag)
   {
      widthInMils      = limitPadsWidthInMils(widthInMils);
      widthInPageUnits = getUnits().convertFrom(pageUnitsMils,widthInMils);
      widthInPadsUnits = widthInPageUnits * padsUnitsPerPageUnit;
   }

   return widthInPadsUnits;
}

double getLimitedPadsWidth(double widthInPageUnits,double padsUnitsPerPageUnit)
{
   bool limitExceededFlag;

   return getLimitedPadsWidth(widthInPageUnits,padsUnitsPerPageUnit,limitExceededFlag);
}

/******************************************************************************
* PADS_WriteSchematicFiles
*/
void PADS_WriteSchematicFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format)
{
   FILE     *wfp;
   FileStruct *file;

   doc = Doc;

   // knv - what are the page units supposed to be for schematic files?
   double padsUnitsPerPageUnit = Units_Factor(doc->getPageUnits(), UNIT_INCHES) * format->Scale;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      return;
   }

   wPADS_GraphSchem_Init();
   wPADS_GraphSchem_File_Open(wfp);

   // EXPLODEALL,
   //   for each file:
   //     write entities of block (including any embedded inserts)
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);

      PADS_WriteSchematicData(wfp, &(file->getBlock()->getDataList()),
            file->getInsertX() * format->Scale,
            file->getInsertY() * format->Scale,
            file->getRotation(), file->isMirrored(), file->getScale() * format->Scale, padsUnitsPerPageUnit, -1);
   }

   wPADS_GraphSchem_File_Close();

   // close write file
   fclose(wfp);
}

/****************************************************************************/
/*
*/
static UINT64 get_padstacklayermap(const char *p, int mirror)
{
   int   i;

   for (i=0;i<padstackcnt;i++)
   {
      if (!strcmp(padstackarray[i]->padstackname,p))
      {
         if (mirror)
            return padstackarray[i]->mirrormap;

         return padstackarray[i]->layermap;
      }
   }
   return ALL_INT64;
}

//--------------------------------------------------------------
static int get_layermap(int ltype)
{
   int   i;

   for (i=0; i<layermapcnt; i++)
   {
      PADS_LayerMap *layerMap = layermaparray[i];
      if (layerMap != NULL && layerMap->getLayerType() == ltype)
         return layermaparray[i]->padslayernr;
   }

   return LAY_UNKNOWN;
}

/******************************************************************************
* do_layerlist
*/
static void do_layerlist()
{
   max_signalcnt = 0;
	int j=0;
   for (j=0; j<doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)
         continue;
      if (layer->getElectricalStackNumber() > max_signalcnt)
         max_signalcnt = layer->getElectricalStackNumber();
   }

   if (max_signalcnt > 20)
      ErrorMessage("Too many electrical Layers", "PADS Power Layer Error");

   maxArr = 0;
   for (j=0; j<doc->getMaxLayerIndex(); j++)      // getMaxLayerIndex() is the exact number of layers, not the next index.
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)
         continue;
      if (layer->isFloating())
         continue;

      int layeron = TRUE;
      int signr = 0;
      char typ = 'D';
      int layernum = 0;

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();

         if (layer->getLayerType() == layerTypePadTop)
         {
            layernum = LAY_TOP;
         }
         else if (layer->getLayerType() == layerTypePadBottom)
         {
            layernum = LAY_BOT;
         }
         else if (layer->getLayerType() == layerTypeSignalTop)
         {
            layernum = 1;
         }
         else if (layer->getLayerType() == layerTypeSignalBottom)
         {
            layernum = max_signalcnt;
         }
         else if (layer->getLayerType() == layerTypePowerPositive)
         {
            layernum = signr;
         }
         else if (layer->getLayerType() == layerTypeSignalInner)
         {
            layernum = signr;
         }
         else if (layer->getLayerType() == layerTypePowerNegative)
         {
            typ = 'P';
            layernum = signr;
         }
         else if (layer->getLayerType() == layerTypeSplitPlane)
         {
            typ = 'P';
            layernum = signr;
         }
         else if (layer->getLayerType() == layerTypePadInner)
         {
            layernum = LAY_INNER;
         }
      }
       // here no electrical stackup
      else if (layer->getLayerType() == layerTypeSignalAll || layer->getLayerType() == layerTypePadAll || layer->getLayerType() == layerTypeAll ||
               layer->getLayerType() == layerTypeSignalOuter || layer->getLayerType() == layerTypePadOuter)
      {
         layernum = LAY_ALL;
      }
      else if (layer->getLayerType() == layerTypePadInner || layer->getLayerType() == layerTypeSignalInner)
      {
         layernum = LAY_INNER;
      }
      else if (layer->getLayerType() == layerTypePadTop)
      {
         layernum = LAY_TOP;
      }
      else if (layer->getLayerType() == layerTypeSignalTop)
      {
         layernum = 1;
      }
      else if (layer->getLayerType() == layerTypeSignalBottom)
      {
         layernum = max_signalcnt;
      }
      else if (layer->getLayerType() == layerTypePadBottom)
      {
         layernum = LAY_BOT;
      }
      else if (layer->getLayerType() == layerTypeDielectric)
      {
         continue;   // not used in pads
      }
      else
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
         layernum = get_layermap(layer->getLayerType());

         if (layernum == LAY_UNKNOWN)
         {
            layeron = FALSE;
            layernum = 0;
         }
      }

      //padsArr.SetSizes
      PADSLayerStruct *pads = new PADSLayerStruct;
      pads->stackNum = layernum;  // layernum
      pads->signr = signr;
      pads->layerindex = j;
      pads->on = layeron;
      pads->type = typ;
      pads->oldName = layer->getName();
      if (layernum > -5)
         pads->newName.Format("%d", layernum);
      else
         pads->newName = "";

      padsArr.SetAtGrow(maxArr++, pads);
   }

   return;
}

/******************************************************************************
* edit_layerlist
*/
static int edit_layerlist()
{
   // fill array
   PadsLayerDlg padsdlg;
   padsdlg.arr = &padsArr;
   padsdlg.maxArr = maxArr;

   if (padsdlg.DoModal() != IDOK)
      return FALSE;

   // here update because load and save of layer files does not contain the
   // signr (this varies) and layerindex.
   // signr
   // layerindex;
   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      int signr = 0;
      int typ = 'D';

      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)
         continue; // could have been deleted.

      if (layer->isFloating())
         continue;

      if (layer->getElectricalStackNumber())
      {
         signr = layer->getElectricalStackNumber();
         if (layer->getLayerType() == LAYTYPE_POWERNEG || layer->getLayerType() == LAYTYPE_SPLITPLANE)
            typ = 'P';
         else
            typ = 'S';
      }

      for (int i=0; i<maxArr; i++)
      {
         if (padsArr[i]->oldName.CompareNoCase(layer->getName()) != 0)
            continue;

         padsArr[i]->signr = signr;
         padsArr[i]->layerindex = j;
         if (padsArr[i]->type != typ)
         {
            fprintf(flog, "Layer [%s] type [%c] was not correct assigned -> change to [%c] !\n",
                  padsArr[i]->oldName, padsArr[i]->type, typ);
            display_error++;
            padsArr[i]->type = typ; // type can be
         }

      }
   }

   return TRUE;
}

/******************************************************************************
* Layer_PADS
*  return -5 if not visible
*/
static int Layer_PADS(int l)
{
   for (int i=0; i<maxArr; i++)
   {
#ifdef _DEBUG
   PADSLayerStruct *pp = padsArr[i];
#endif
      if (padsArr[i]->on == 0)
         continue;   // not visible
      if (padsArr[i]->layerindex == l)
         return atoi(padsArr[i]->newName);
   }
   return -5;
}

/******************************************************************************
* free_layerlist
*/
static void free_layerlist()
{
   for (int i=0; i<maxArr; i++)
      delete padsArr[i];
   padsArr.RemoveAll();
}

/******************************************************************************
* pads_layer_used
*/
static int pads_layer_used(int l)
{
   for (int i=0; i<maxArr; i++)
   {
      if (padsArr[i]->on == 0)
         continue;
      if (padsArr[i]->stackNum == l)
         return 1;
   }
   return 0;
}

/******************************************************************************
* layer_color
*/
static int layer_color(FILE *fp,const char *lname, int defcolor, int maxsiglayer)
{
   int   color;

   fprintf(fp, lname);
   for (int l=1; l<=30; l++)
   {
      if (l == 1)
         color = 7;  // green
      else
      if (l == maxsiglayer)
         color = 3; // red
      else
         color = (l % 16) + 1;

      if (defcolor && (optionColorAllLayers || pads_layer_used(l)))
         fprintf(fp, " %d", color);
      else
         fprintf(fp, " %d", 0);
   }
   fprintf(fp, "\n");

   return 1;
}

/******************************************************************************
* do_pcbsection
*/
static int do_pcbsection(FILE *fp, int units, int maxsiglayer, BlockStruct *fileblock, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLevel)
{
   Mat2x2 m;
   Point2 point2;

   fprintf(fp, "*PCB*        GENERAL PARAMETERS OF THE PCB DESIGN\n");
   fprintf(fp, "*REMARK* ASCII file generated by %s\n", getApp().getCamCadTitle());
   fprintf(fp, "\n");
   fprintf(fp, "UNITS        %d             2=Inches 1=Metric 0=Mils\n", units);
   fprintf(fp, "MAXIMUMLAYER %-2d            Maximum routing layer \n", maxsiglayer);

   doc->CalcBlockExtents(fileblock);

   point2.x = fileblock->getXmin() * scale;
   point2.y = fileblock->getYmin() * scale;

   RotMat2(&m, rotation);
   TransPoint2(&point2, 1, &m, insert_x, insert_y);

   point2.x = -point2.x;
   point2.y = -point2.y;

   double add_offset = 0; // add to the next INCH
   if (units == 0)
      add_offset = 1000;
   if (units == 1)
      add_offset = 100;
   if (units == 2)
      add_offset = 1;

   if (point2.x > 0)
      point2.x += add_offset;
   if (point2.y > 0)
      point2.y += add_offset;

   fprintf(fp, "ORIGIN       %-5.0lf %-5.0lf   User defined origin location\n", point2.x, point2.y);

   // here colors in between
   fprintf(fp, "*REMARK*  Colors 0-16 for levels 1-30\n");
   layer_color(fp, "LINCOL", 1, maxsiglayer);
   layer_color(fp, "TXTCOL", 1, maxsiglayer);
   layer_color(fp, "TRKCOL", 1, maxsiglayer);
   layer_color(fp, "COPCOL", 1, maxsiglayer);
   layer_color(fp, "PADCOL", 1, maxsiglayer);
   layer_color(fp, "VIACOL", 1, maxsiglayer);
   layer_color(fp, "ERRCOL", 0, maxsiglayer);
   layer_color(fp, "CMTCOL", 1, maxsiglayer);
   layer_color(fp, "CMBCOL", 1, maxsiglayer);
   layer_color(fp, "RFDCOL", 1, maxsiglayer);
   layer_color(fp, "PRTCOL", 1, maxsiglayer);
   layer_color(fp, "LABCOL", 1, maxsiglayer);
   layer_color(fp, "KPTCOL", 1, maxsiglayer);

   fprintf(fp, "CONCOL 11\n");
   fprintf(fp, "FBGCOL 1 0\n");
   fprintf(fp, "HATCHDISP    1              Hatch display flag\n");
   fprintf(fp, "DRLOVERSIZE  0              Drill oversize for plated holes\n"); // this checks drill hole against padsize.
   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* save_padshape
*/
static int save_padshape(BlockStruct *b, int level, double size, double finlength, double finoffset, double finori,
                         const char *padshape, double idia)
{
   // first check that the same layer is not already saved
   for (int i=0; i<padshapecnt; i++)
   {
      if (padshapearray[i]->level == level)
      {
         // padlevel already exist !
         return -1;
      }
   }

   // decalscale is always in mils
   // max size = 0..500 mil
   // max finlen = 0..1000 mil
   if (size > 500)
   {
      fprintf(flog ,"Padshape [%s] changed: Size must be between 0..500 MILS\n", b->getName());
      display_error++;
      size = 500;
   }
   if (finlength > 1000)
   {
      fprintf(flog ,"Padshape [%s] changed: Length must be between 0..1000 MILS\n", b->getName());
      display_error++;
      finlength = 1000;
   }

   PADS_PadshapeStruct *p = new PADS_PadshapeStruct;
   padshapearray.SetAtGrow(padshapecnt++, p);
   p->level = level;
   p->padshape = b;
   p->size = size;
   p->finlength = finlength;
   p->finoffset = finoffset;
   p->finori = finori;
   p->shape = padshape;
   p->idia = idia;
   p->complex_error_reported = 0;
   p->stacknr = 0;
   return padshapecnt-1;
}

/******************************************************************************
* is_decal_layer
*/
static int is_decal_layer(int nplayer, int insertLayer, int mirror)
{
   int layer;
   int padslayer = -1;

   if (doc->IsFloatingLayer(nplayer) && insertLayer != -1)
      layer = insertLayer;
   else
      layer = nplayer;

   if (!doc->get_layer_visible(layer, mirror))
      return -1;

   layer = doc->get_layer_mirror(layer, mirror);
   padslayer = Layer_PADS(layer);
   if (padslayer == -1)
      padslayer = 1;
   else if (padslayer == -2)
      padslayer = max_signalcnt;
   else if (padslayer == -4)
      padslayer = 0;

   return padslayer;
}

/******************************************************************************
* PADS_GetDecalInfo
*/
static int PADS_GetDecalInfo(CDataList *DataList, double insert_x, double insert_y, double rotation,
                             int mirror, double scale, int insertLayer, int *pieces, int *terminals, int *text)
{
   Mat2x2 m;
   Point2 point2;

   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      if (data->getDataType() != T_INSERT)
      {
         int padslayer = is_decal_layer(data->getLayerIndex(), insertLayer, mirror);
         if (padslayer < 0)
            continue;
      }

      switch(data->getDataType())
      {
      case T_POLY:
         {
            if (data->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)
               break; //placement keepout not allowed in DECAL

            // need to go into Poly, a poly could have multiple polygons or voids.
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);
               int cnt = poly->getPntList().GetCount();
               if (cnt < 2)
                  break;
               *pieces = *pieces + 1;
            }
         }
         break;
      case T_TEXT:
         {
            *text = *text + 1;
         }
         break;
      case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();
            point2.x = data->getInsert()->getOriginX() * scale;
            point2.y = data->getInsert()->getOriginY() * scale;
            if (mirror)
               point2.x = -point2.x;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
            if (data->getInsert()->getInsertType() == INSERTTYPE_PIN || data->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL ||
                data->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN || data->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {
               *terminals = *terminals + 1;
               PADS_TerminalStruct *t = new PADS_TerminalStruct;
               terminalarray.SetAtGrow(terminalcnt, t);
               terminalcnt++;
               t->x = point2.x;
               t->y = point2.y;
               t->rot = block_rot;
               t->mirror = block_mirror;
               t->pinname = data->getInsert()->getRefname();
               t->padstackname = block->getName();
               t->pinnr = 0;
               t->pinData = data;

               WORD pinnrKW = doc->IsKeyWord(ATT_COMPPINNR, 0);
               Attrib* attrib;

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(pinnrKW, attrib))
               {
                  t->pinnr = attrib->getIntValue();
               }
            }

            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               if (block->getShape() == T_COMPLEX)
               {
                  int block_layer;
                  if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
                     block_layer = data->getLayerIndex();
                  else
                     block_layer = insertLayer;

                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  PADS_GetDecalInfo(&subblock->getDataList(), point2.x, point2.y, block_rot, mirror,
                        scale * data->getInsert()->getScale(), block_layer, pieces, terminals, text);
               }
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
                  block_layer = data->getLayerIndex();
               else
                  block_layer = insertLayer;

               PADS_GetDecalInfo(&block->getDataList(), point2.x, point2.y, block_rot, mirror,
                     scale * data->getInsert()->getScale(), block_layer, pieces, terminals, text);
            }
         }
         break;
      }
   }
   return 1;
} /* end PADS_GetDecalInfo */

/******************************************************************************
* get_terminalptr
*/
static int get_terminalptr(const char *n)
{
   if (n == NULL) return 0;
   if (!strlen(n)) return 0;

   for (int i=0; i<terminalcnt; i++)
   {
#ifdef _DEBUG
   PADS_TerminalStruct *t0 = terminalarray[i];
#endif
      if (!terminalarray[i]->pinname.CompareNoCase(n))
         return terminalarray[i]->pinnr;
   }
   return 0;
}

/******************************************************************************
* PADS_WriteDecalPieces
*/
static int PADS_WriteDecalPieces(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
                                 double rotation, int mirror, double scale, double padsUnitsPerPageUnit, int insertLayer, int pinnumber)
{
   Mat2x2 m;
   Point2 point2;

   int padslayer;
   int pinnr;

   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      if (data->getDataType() != T_INSERT)
      {
         padslayer = is_decal_layer(data->getLayerIndex(), insertLayer, mirror);
         if (padslayer < 0)
            continue;
         wPADS_Graph_LevelNum(padslayer);
      }
      pinnr = pinnumber;   // can be zerod when not on copper layer.
      switch(data->getDataType())
      {
         case T_POLY:
         {
            char *LINETYPE = "LINES";
            int polyVoid = 0;
            int polyFilled = 0;
            int closed = 0;
            int boundary = 0;
            int hatchline = 0;
            int cnt = 0;

            if (data->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)
               break; //Removed P restriction, placement keepout not allowed in DECAL

            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);
               cnt = poly->getPntList().GetCount();
               if (cnt < 2)
                  break;
               if (poly->isFilled())
                  polyFilled++;
               if (poly->isClosed())
               {
                  if (cnt < 3)
                     poly->setClosed(false);
                  closed++;
               }
               if (poly->isFloodBoundary())
                  boundary++;
               if (poly->isHatchLine())
                  hatchline++;
               if (poly->isVoid())
                  polyVoid++;
            }

            if (cnt < 2)
               break;

            // loop thru polys
            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               double cx, cy, rad;

               CPoly *poly = data->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed = poly->isClosed();
               BOOL circle = PolyIsCircle(poly, &cx, &cy, &rad);

               // note: decal data is written with decal units, which are always mils
               double widthInMils = limitPadsWidthInMils(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);
               //double widthInPadsUnits = getLimitedPadsWidth(doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
               //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
               //if (w < 0.01)
               //   w = 0.01;   // limits in PADS
               //if (w > 250)
               //   w = 250;

               CString piecetype;
               CString restriction = "";

               //if (padslayer >= 1 && padslayer <= max_signalcnt)
               if (data->getGraphicClass() == GR_CLASS_ETCH)
               {
                  if (polyFilled)
                  {
                     if (circle)
                        piecetype = "COPCIR";
                     else
                        piecetype = "COPCLS";
                  }
                  else
                  {
                     circle = FALSE;
                     piecetype = "COPOPN";
                  }
               }
               else if (data->getGraphicClass() == GR_CLASS_ROUTKEEPOUT ||
                        data->getGraphicClass() == GR_CLASS_VIAKEEPOUT ||
                        data->getGraphicClass() == GR_CLASS_ALLKEEPOUT ||
                        data->getGraphicClass() == graphicClassTestPointKeepOut)
               {
                  if (circle)
                     piecetype = "KPTCIR";
                  else
                     piecetype = "KPTCLS";

                  //if (data->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)
                  // restriction = "P"; //Removed P restriction, placement keepout not allowed in DECAL
                  if (data->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)
                     restriction = "RC";
                  else if (data->getGraphicClass() == GR_CLASS_VIAKEEPOUT)
                     restriction = "V";
                  else if (data->getGraphicClass() == graphicClassTestPointKeepOut)
                     restriction = "T";
                  else if (data->getGraphicClass() == GR_CLASS_ALLKEEPOUT)
                     restriction = "RCVT"; //Removed P restriction, placement keepout not allowed in DECAL
               }
               else
               {  // if the shape in padstack is complex(made with polys) then it's always made by copper and tied to the pin.
                  // it doesn't matter if it's on none electrical layer.
                  if (pinnumber > 0) // then it's a pad for pin
                  {
                     if (polyFilled)
                     {
                        if (circle)
                           piecetype = "COPCIR";
                        else
                           piecetype = "COPCLS";
                     }
                     else
                        piecetype = "COPOPN";
                     pinnr = pinnumber;
                  }
                  else  // poly is part of a component or a board
                  {
                     if (closed)
                     {
                        if (circle)
                           piecetype = "CIRCLE";
                        else
                           piecetype = "CLOSED";
                     }
                     else
                        piecetype = "OPEN";
                     pinnr = 0;
                  }
               }

               if (circle)
               {
                  Point2 p;
                  p.x = cx * scale * padsUnitsPerPageUnit;
                  p.y = cy * scale * padsUnitsPerPageUnit;
                  p.bulge = 0;
                  if (mirror)
                     p.x = -p.x;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  rad *= scale * padsUnitsPerPageUnit;

                  if (pinnr != 0)
                     fprintf(fp,"%s 2 %lg %d %d\n", piecetype, wPads_Units(output_units_accuracy, widthInMils), padslayer, pinnr - 1);
                  else if (restriction != "")
                     fprintf(fp,"%s 2 %lg %d %s\n", piecetype, wPads_Units(output_units_accuracy, widthInMils), padslayer, restriction);
                  else
                     fprintf(fp,"%s 2 %lg %d\n", piecetype, wPads_Units(output_units_accuracy, widthInMils), padslayer);

                  fprintf(fp,"%lg %lg\n", wPads_Units(output_units_accuracy, p.x - rad), wPads_Units(output_units_accuracy, p.y));
                  fprintf(fp,"%lg %lg\n", wPads_Units(output_units_accuracy, p.x + rad), wPads_Units(output_units_accuracy, p.y));
               }
               else
               {
                  Point2 p;
                  int vertexcnt = 0;
                  POSITION pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntPos);
                     vertexcnt++;
                  }

                  int tcnt = vertexcnt;
                  Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                  vertexcnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale * padsUnitsPerPageUnit;
                     p.y = pnt->y * scale * padsUnitsPerPageUnit;
                     p.bulge = pnt->bulge;
                     if (mirror)
                        p.x = -p.x;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     points[vertexcnt].x = p.x;
                     points[vertexcnt].y = p.y;
                     points[vertexcnt].bulge = p.bulge;
                     vertexcnt++;
                  }

                  if (pinnr != 0)
                     fprintf(fp,"%s %d %lg %d %d\n", piecetype, vertexcnt, wPads_Units(output_units_accuracy, widthInMils), padslayer, pinnr - 1);
                  else if (restriction != "")
                     fprintf(fp,"%s %d %lg %d %s\n", piecetype, vertexcnt, wPads_Units(output_units_accuracy, widthInMils), padslayer, restriction);
                  else
                     fprintf(fp,"%s %d %lg %d\n", piecetype, vertexcnt, wPads_Units(output_units_accuracy, widthInMils), padslayer);
						
						int i=0;
                  for (i=0; i<vertexcnt-1; i++)
                  {
                     // here figure out bulge, but only the first koo gets written.
                     // max bulge in output unis
                     wPADS_out_line(28000*onemil, points[i].x, points[i].y, points[i].bulge, points[i+1].x, points[i+1].y);
                  }
                  // the last can never have a bulge, so set the +1 coo to 0,0
                  wPADS_out_line(28000*onemil, points[i].x ,points[i].y ,points[i].bulge, 0, 0);
                  free(points);
               }
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
            int block_mirror = data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();
            point2.x = data->getInsert()->getOriginX() * scale * padsUnitsPerPageUnit;
            point2.y = data->getInsert()->getOriginY() * scale * padsUnitsPerPageUnit;
            if (mirror)
               point2.x = -point2.x;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
            if (data->getInsert()->getInsertType() == INSERTTYPE_PIN || data->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL ||
                data->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN || data->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {
               pinnr = get_terminalptr(data->getInsert()->getRefname()); // returns 0 if not found.
            }

            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               if (block->getShape() == T_COMPLEX)
               {
                  int block_layer;
                  if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
                     block_layer = data->getLayerIndex();
                  else
                     block_layer = insertLayer;

                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  block_rot += block->getRotation() + subblock->getRotation();
                  PADS_WriteDecalPieces(fp, &subblock->getDataList(), point2.x, point2.y, block_rot,
                        block_mirror, scale * data->getInsert()->getScale(), padsUnitsPerPageUnit, block_layer, pinnr);

               }
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
                  block_layer = data->getLayerIndex();
               else
                  block_layer = insertLayer;

               PADS_WriteDecalPieces(fp, &block->getDataList(), point2.x, point2.y, block_rot,
                     mirror, scale * data->getInsert()->getScale(), padsUnitsPerPageUnit, block_layer, pinnr);
            }
         } // case INSERT
         break;

      } // end switch
   } // end for
   return 1;
}

/*****************************************************************************/
/*
*/
static int PADS_WriteDecalText(FILE *fp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   int padslayer;

   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      if (data->getDataType() != T_INSERT)
      {
         padslayer = is_decal_layer(data->getLayerIndex(), insertLayer, mirror);
         if (padslayer < 0)
            continue;
         wPADS_Graph_LevelNum(padslayer);
      }

      switch(data->getDataType())
      {
         case T_POLY:
         {
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = data->getText()->getPnt().x * scale;
            point2.y = data->getText()->getPnt().y * scale;
            if (mirror)
               point2.x = -point2.x;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            int text_mirror = data->getText()->isMirrored(); // text is mirrored if mirror is set or text.mirror but not if none or both
            double text_rot = rotation + data->getText()->getRotation();
            double w = doc->getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA() * scale;

            wPADS_Graph_Text(data->getText()->getText(), point2.x, point2.y, data->getText()->getHeight()*scale,
                  data->getText()->getWidth()*scale, text_rot, text_mirror, w, FALSE, data->getText()->getHorizontalPosition(), data->getText()->getVerticalPosition());
         }
         break;

         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();
            point2.x = data->getInsert()->getOriginX() * scale;
            point2.y = data->getInsert()->getOriginY() * scale;
            if (mirror)
               point2.x = -point2.x;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               if (block->getShape() == T_COMPLEX)
               {
                  int block_layer;
                  if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
                     block_layer = data->getLayerIndex();
                  else
                     block_layer = insertLayer;

                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  PADS_WriteDecalText(fp, &subblock->getDataList(), point2.x, point2.y,
                        block_rot, block_mirror, scale * data->getInsert()->getScale(), block_layer);

               }
            }
            else
            {
               int block_layer;
               if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
                  block_layer = data->getLayerIndex();
               else
                  block_layer = insertLayer;

               PADS_WriteDecalText(fp, &block->getDataList(), point2.x, point2.y,
                     block_rot, mirror, scale * data->getInsert()->getScale(), block_layer);
            }
         } // case INSERT
         break;

      } // end switch
   } // end for
   return 1;
}

/*****************************************************************************/
/*
*/
static int get_differentstacks(int *stacks)
{
   *stacks = 1;
   PADS_TerminalStruct *t0 = terminalarray[0];  // debug only
   terminalarray[0]->used_as_default = TRUE;

   for (int i=1; i<terminalcnt; i++)
   {
      PADS_TerminalStruct *t1 = terminalarray[i];  // debug only

      if (terminalarray[0]->padstackname.Compare(terminalarray[i]->padstackname) ||
          terminalarray[0]->rot != terminalarray[i]->rot ||
          (terminalarray[0]->mirror & MIRROR_LAYERS) != (terminalarray[i]->mirror & MIRROR_LAYERS)) // not the same
      {
         terminalarray[i]->used_as_default = FALSE;
         *stacks = *stacks + 1;
      }
      else
      {
         terminalarray[i]->used_as_default = TRUE;
      }
   }
   return 0; // returns default stack index
}

/*****************************************************************************/
/*
*/
static int write_padshape(FILE *fp, PADS_PadshapeStruct *p)
{
   // R S A O OF RF
   if (!STRCMPI(p->shape,"R"))
      fprintf(fp,"%d %lg %s", p->level, wPads_Units(output_units_accuracy, p->size), p->shape);
   else
   if (!STRCMPI(p->shape,"S"))
      fprintf(fp,"%d %lg %s", p->level, wPads_Units(output_units_accuracy, p->size), p->shape);
   else
   if (!STRCMPI(p->shape,"RF"))
      fprintf(fp,"%d %lg %s %1.1lf %lg %lg", p->level, wPads_Units(output_units_accuracy, p->size),
            p->shape,  p->finori, wPads_Units(output_units_accuracy, p->finlength),
            wPads_Units(output_units_accuracy, p->finoffset));
   else
   if (!STRCMPI(p->shape,"OF"))
      fprintf(fp,"%d %lg %s %1.1lf %lg %lg", p->level,
         wPads_Units(output_units_accuracy, p->size),
         p->shape, p->finori,
         wPads_Units(output_units_accuracy, p->finlength),
         wPads_Units(output_units_accuracy, p->finoffset));
   else
      fprintf(fp,"%d %lg R", p->level, wPads_Units(output_units_accuracy, p->size));

   return 1;
}

/*****************************************************************************/
/*
typedef struct
{
   CString  padstackname;
   int      typ;                 // 1, 2
   unsigned long  layermap;      // normal layer
   unsigned long  mirrormap;     // mirrored layer (done for speed reasons once)
} PADS_PadStackStruct;
typedef CTypedPtrArray<CPtrArray, PADS_PadStackStruct*> PadStackArray;
*/
static int update_padstackarray(const char *name, int typ)
{
   int   i;

   for (i=0;i<padstackcnt;i++)
   {
      if (padstackarray[i]->padstackname.Compare(name) == 0)
         return i;
   }

   PADS_PadStackStruct *p = new PADS_PadStackStruct;
   p->padstackname = name;

   p->typ = typ;
   if (typ == 1)
   {
      UINT64 baseVal  = 1L;
      p->layermap =  0x1;
      p->mirrormap = baseVal << (max_signalcnt-1);
   }
   else
   if (typ == 2)
   {
      UINT64 baseVal  = 1L;
      p->layermap = baseVal << (max_signalcnt-1);
      p->mirrormap =  0x1;
   }
   else
   {
      p->layermap = ALL_INT64;
      p->mirrormap = ALL_INT64;
   }

   padstackarray.SetAtGrow(padstackcnt, p);
   padstackcnt++;

   return padstackcnt-1;
}

/*****************************************************************************/
/*
*/
static int write_stacks(FILE *fp, int terminalptr, int pinnr, double scale, int mirror = 0)
{
   int   p;

#ifdef _DEBUG
   PADS_TerminalStruct *tt = terminalarray[terminalptr];
#endif

   // terminalarray[terminalptr]
   // fprintf(fp,"PAD %d 4\n", pinnr);
   // fprintf(fp,"-2 60  R   35\n");
   // fprintf(fp,"-1 60  R\n");
   // fprintf(fp,"0  60  R\n");
   // fprintf(fp,"25 80  R\n");

   BlockStruct *b = doc->Find_Block_by_Name(terminalarray[terminalptr]->padstackname, -1);
   padshapecnt = 0;
   double      drillsize = 0;
   BOOL     plated = TRUE;
   char     pl = ' ';

   // return 0x1 top
   //        0x2 bottom
   //        0x4 drill
   //        0x8 inner
   int s1, e1; // start, endlayer not needed
   int typ = PADS_GetPADSTACKData(b->getName(), &b->getDataList(), -1, &drillsize, &plated, scale, terminalarray[terminalptr]->rot, 0, &s1, &e1, terminalarray[terminalptr]->mirror);
   update_padstackarray(b->getName(), typ);

   if (drillsize == 0)
      if (b->getToolType() == BLOCKTYPE_DRILLHOLE)
      {
         drillsize = b->getToolSize();
         if (!b->getToolHolePlated())
            pl = 'N';
      }

   if (!plated)
      pl = 'N';

   if (padshapecnt == 0)
   {
      fprintf(fp,"PAD %d 3\n", pinnr);
      fprintf(fp,"-2 0 R %lg %c\n", wPads_Units(output_units_accuracy, drillsize), pl);
      fprintf(fp,"-1 0 R\n");
      fprintf(fp,"0  0 R\n");
   }
   else
   {
      // make sure -2, -1 and 0 are there
      int found_minus2 = FALSE, found_minus1 = FALSE, found_0 = FALSE;
      for (int w=0;w<padshapecnt;w++)
      {
         if (padshapearray[w]->level == -2)
            found_minus2 = TRUE;
         if (padshapearray[w]->level == -1)
            found_minus1 = TRUE;
         if (padshapearray[w]->level == 0)
            found_0 = TRUE;
      }

      if (!found_minus2)
         save_padshape(b, -2, 0, 0, 0, 0, "R", 0);
      if (!found_minus1)
         save_padshape(b, -1, 0, 0, 0, 0, "R", 0);
      if (!found_0)
         save_padshape(b,  0, 0, 0, 0, 0, "R", 0);

      fprintf(fp,"PAD %d %d\n", pinnr, padshapecnt);

      // sort by layer
      int first = TRUE;
      for (int pp=-2;pp<=MAX_PADSLAYER;pp++)
      {
         for (p=0;p<padshapecnt;p++)
         {
            if (padshapearray[p]->level != pp)
               continue;
            write_padshape(fp, padshapearray[p]);
            if (first)
            {
               if (drillsize == 0)
                  pl = 'N';   // not plated.
               fprintf(fp," %lg %c", wPads_Units(output_units_accuracy, drillsize), pl);
            }
            fprintf(fp,"\n");
            first = FALSE;
         }
      }
   }  // padshapecnt

   for (p=0;p<padshapecnt;p++)
      delete padshapearray[p];
   padshapecnt = 0;

   return 1;
}

/*****************************************************************************/
/*
*/
static int PADSPinNumberCompareFunc( const void *arg1, const void *arg2 )
{
   PADS_TerminalStruct **a1, **a2;
   a1 = (PADS_TerminalStruct**)arg1;
   a2 = (PADS_TerminalStruct**)arg2;

   if ((*a1)->pinnr < (*a2)->pinnr)
      return -1;
   else if ((*a1)->pinnr > (*a2)->pinnr)
      return 1;
   else
      return 0;
}


/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is
   because of the 2 pass to get artwork info.
*/
static void sort_terminals()
{
   if (terminalcnt < 2) return;

   qsort(terminalarray.GetData(), terminalcnt, sizeof(PADS_TerminalStruct *), PADSPinNumberCompareFunc);
   return;
}


class CBooleanArray : public CArray<bool>
{
public:
   bool GetNextIndex(const bool value, int &index) const
   {
      do
      {
         index++;
         if (index < 0 || index >= GetCount())
         {
            index = -1;
            return false;
         }
      } while (GetAt(index) != value);

      return true;
   }

   void SetSizeInit(INT_PTR nNewSize, bool bInitVal = true, INT_PTR nGrowBy = -1)
   {
      SetSize(nNewSize, nGrowBy);
      for (int i=0; i<GetCount(); i++)
         SetAt(i, bInitVal);
   }
};

/******************************************************************************
* check_terminals
*
* Terminal Rules/Properties:
*  - pin numbers (PINNR attr) must equal 1 to the pin count
*  - existing pin numbers that are correctly in range must remain the same
*  - pin names must be unique
*  - duplicates result in renaming to its pin number (rule as to which terminal keeps it name)
*     - pin names the same as it's pin number keep the name
*     - the lowest pin number
*/
static int check_terminals()
{
   int renameCnt = 0;

   if (terminalcnt == 1)
   {
      if (terminalarray[0]->pinname.IsEmpty())
      {
         terminalarray[0]->pinnr = 1;
         terminalarray[0]->pinname.Format("PADS_RNP%03d", ++renameCnt);
      }
   }

   if (terminalcnt > 1)
   {
      // get all pin numbers that are in range
      CBooleanArray availablePinNumbers;
      availablePinNumbers.SetSizeInit(terminalcnt);

      for (int i=0; i<terminalcnt; i++)
      {
         PADS_TerminalStruct *terminal = terminalarray[i];

         if (terminal->pinnr > 0 && terminal->pinnr <= terminalcnt)
            availablePinNumbers[terminal->pinnr - 1] = false;
      }

      // assign pin numbers not in range if there were any
      int curAvailablePinNumber = -1;
      if (availablePinNumbers.GetNextIndex(true, curAvailablePinNumber))
      {
         curAvailablePinNumber = -1;
         for (int i=0; i<terminalcnt; i++)
         {
            PADS_TerminalStruct *terminal = terminalarray[i];

            if (terminal->pinnr <= 0 || terminal->pinnr > terminalcnt)
            { // if the terminals pin number is out of range
               if (availablePinNumbers.GetNextIndex(true, curAvailablePinNumber))
                  terminal->pinnr = curAvailablePinNumber + 1;
            }
         }
      }

      // sort the terminals
      sort_terminals();


      // determine terminals where the pin name is the same as the pin number
      CBooleanArray goodPinNameIndecies;
      goodPinNameIndecies.SetSizeInit(terminalcnt, false);
      for (int i=0; i<terminalcnt; i++)
      {
         PADS_TerminalStruct *terminal = terminalarray[i];

         CString pinnr;
         pinnr.Format("%d", terminal->pinnr);

         goodPinNameIndecies[i] = (pinnr == terminal->pinname);
      }

      // terminals where name is diff from number, check if those verified (above) use the same name
      //   if so set to PADS_RNP<3 digit index> or else set to verified
      for (int i=0; i<terminalcnt; i++)
      {
         if (goodPinNameIndecies[i])
            continue;

         PADS_TerminalStruct *terminal1 = terminalarray[i];
         if (terminal1->pinname.IsEmpty())
         {
            terminal1->pinname.Format("PADS_RNP%03d", ++renameCnt);
            continue;
         }

         for (int j=0; j<terminalcnt; j++)
         {
            // check only with varifiect ones
            if (!goodPinNameIndecies[j] || i == j)
               continue;

            PADS_TerminalStruct *terminal2 = terminalarray[j];

            if (!terminal1->pinname.CompareNoCase(terminal2->pinname))
            {
               // rename the pin name to something a pin name would never be
               terminal1->pinname.Format("PADS_RNP%03d", ++renameCnt);
               break;
            }
         }

         // this one is good now
         goodPinNameIndecies[i] = true;
      }
   } // END if (terminalcnt > 1) ...

   // write back to datas
   for (int i=0; i<terminalcnt; i++)
   {
      PADS_TerminalStruct *terminal = terminalarray[i];

      int keyword = doc->IsKeyWord(ATT_COMPPINNR, 0);
      terminal->pinData->setAttrib(doc->getCamCadData(), keyword, valueTypeInteger, &terminal->pinnr, attributeUpdateOverwrite, NULL);
      terminal->pinData->getInsert()->setRefname(STRDUP(terminal->pinname));
   }

   return 1;
}

/******************************************************************************
* write_DECALData
// note: decal data is written with decal units, which are always mils
*/
static int write_DECALData(FILE *fp, BlockStruct *b, CDataList *DataList, double scale, double padsUnitsPerPageUnit)
{
   int pieces = 0;
   int terminals = 0;
   int stacks = 0;   // need to be calculated from terminalcnt array
   int text = 0;
   int labels = 2;
   int mirror = 0;
   int defaultterminalptr = 0;
   int pinnr = 0;
   int i = 0;
   double insert_x = 0;
   double insert_y = 0;
   double rotation = 0;

   // *REMARK* NAME ORIX ORIY PIECES TERMINALS STACKS TEXT LABELS
   // DIP28-600        I 38100000 38100000  2 28 2 0 2

   // this collects the terminals and graphic pieces
   PADS_GetDecalInfo(DataList, insert_x, insert_y, rotation, mirror, scale * padsUnitsPerPageUnit, -1, &pieces, &terminals, &text);

   // every terminal must have a name in PADS, even that in CAMCAD fiducials etc... does not need them
   check_terminals();

   // MUST sort the terminals again because check_terminals() might create pinnr that cause the pinnr to be out of order
   sort_terminals();

   if (terminalcnt)
      defaultterminalptr = get_differentstacks(&stacks);

   CString decalName = check_name('g', b->getName());

   // complex pads have to be added to piece section !
   fprintf(fp, "%s I 1000 1000 %d %d %d %d %d\n",decalName, pieces, terminals, stacks, text, labels);

   // write pieces
   PADS_WriteDecalPieces(fp, DataList, insert_x, insert_y, rotation, mirror, scale, padsUnitsPerPageUnit, -1, pinnr);

   // write text
   PADS_WriteDecalText(fp, DataList, insert_x, insert_y, rotation, mirror, scale * padsUnitsPerPageUnit, -1);

   // write labels
   fprintf(fp, "VALUE          0        0   0.000  1         100          10 N   LEFT   DOWN\n");
   fprintf(fp, "Ref.Des.\n");
   fprintf(fp, "VALUE          0        0   0.000  1         100          10 N   LEFT   DOWN\n");
   fprintf(fp, "Part Type\n");

   // write pad definition
   for (i=0; i<terminalcnt; i++)
   {
      fprintf(fp,"T%lg %lg %lg %lg\n",
            wPads_Units(output_units_accuracy, terminalarray[i]->x),
            wPads_Units(output_units_accuracy, terminalarray[i]->y),
            wPads_Units(output_units_accuracy, terminalarray[i]->x),
            wPads_Units(output_units_accuracy, terminalarray[i]->y));
      PADS_DecalPinNameStruct *p = new PADS_DecalPinNameStruct;
      decalPinNameArray.Add(p);
      p->blocknr = b->getBlockNumber();
      p->pinname = terminalarray[i]->pinname;
      p->pinnr   = terminalarray[i]->pinnr;
   }

   // write padstack definition
   if (terminalcnt)
   {
      // default is always defaultterminalptr
      write_stacks(fp, defaultterminalptr, 0, scale * padsUnitsPerPageUnit);
      for (terminals=0; terminals<terminalcnt; terminals++)
      {
         if (terminalarray[terminals]->used_as_default)
            continue;
         write_stacks(fp, terminals, terminals+1, scale * padsUnitsPerPageUnit);
      }

      for (terminals=0; terminals<terminalcnt; terminals++)
      {
         delete terminalarray[terminals];
      }
      terminalcnt = 0;
   }

   fprintf(fp, "\n");
   return 1;
}

/*****************************************************************************/
/*
*/
static int get_complex_error(const char *name)
{
   int   i;

   for (i=0;i<complexerrorcnt;i++)
   {
      if (complexerrorarray[i]->padshapename.CompareNoCase(name) == 0)
         return i;
   }

   return -1;
}

/*****************************************************************************/
/*
   small_delta is smallest unit, normally 1/10 of mil or inch or mm
*/
static double round_off(double num, double small_delta)
{
   double n = num/small_delta;
   n = round(n);
   return n*small_delta;
}

//--------------------------------------------------------------
// R = round
// S = square
// A = annular
// O = odd
// OF = oval - finger
// RF = rectangle
//
static const char *get_padshape(BlockStruct *block, Point2 *p, double p_rot, double scale,
                                double *padsize,
                                double *finlength, double *finoffset,
                                double *finori, double *idia)
{
   int   padrot = round(RadToDeg(block->getRotation() + p_rot));
   int   prot;
   while (padrot < 0)
      padrot += 360;
   while (padrot >= 360)
      padrot -= 360;
   prot = padrot;

   double xoff = block->getXoffset() * scale, yoff = block->getYoffset() * scale;
   //Rotate(block->getXoffset() * scale, block->getYoffset() * scale, padrot, &xoff, &yoff);

   // rotate of the aperture and rotation of the insert of the aperture is the same effest on offset.
   // all rotations are around the insert

   char *shape = "R";
   *padsize = 0;
   *finlength = 0;
   *finoffset = 0;
   *finori = 0;
   *idia = 0;

   if (fabs(xoff) > SMALLNUMBER && fabs(yoff) > SMALLNUMBER)
   {
      fprintf(flog, "Pad [%s] offset in X and Y not allowed. Pad will not have an offset.\n", block->getName());
      display_error++;
      xoff = yoff = 0;
   }

   // Aperture Shape
   switch (block->getShape())
   {
   case T_ROUND:
      shape = "R";
      *padsize = block->getSizeA() * scale;
      break;
   case T_SQUARE:
      shape = "S";
      *padsize = block->getSizeA() * scale;
      break;
   case T_TARGET:
      shape = "R";
      *padsize = block->getSizeA() * scale;
      break;
   case T_THERMAL:
      shape = "R";
      *padsize = block->getSizeA() * scale;
      break;
   case T_COMPLEX:
      {
         // must have a small padsize, otherwise you can not attach copper to the pin !
         *padsize = 10 * Units_Factor(UNIT_MILS, page_units) * scale;
         shape = "R";
      }
      break;
   case T_DONUT:
      shape = "R";
      *padsize = block->getSizeA() * scale;
      break;
   case T_OCTAGON:
      shape = "R";
      *padsize = block->getSizeA() * scale;
      break;
   case T_RECTANGLE:
   case T_OBLONG:
      {
         if (block->getShape() == T_RECTANGLE)
            shape = "RF";
         else
            shape = "OF";

         *padsize = block->getSizeB() * scale;    // padsize is Y
         *finlength = block->getSizeA() * scale;  // length is X
         *finoffset = 0;
         *idia = 0;

         if (*padsize > *finlength)
         {
            double tmp = *padsize;
            *padsize = *finlength;
            *finlength = tmp;
            padrot += 90;

            *finoffset = p->y + yoff;
            if (fabs(xoff) > SMALLNUMBER)
            {
               fprintf(flog, "Pad [%s] offset in X not allowed. Pad will not have an offset.\n", block->getName());
               display_error++;
            }
         }
         else
         {
            *finoffset = p->x + xoff;
            if (fabs(yoff) > SMALLNUMBER)
            {
               fprintf(flog, "Pad [%s] offset in Y not allowed. Pad will not have an offset.\n", block->getName());
               display_error++;
            }
         }
      }
      break;
   case T_BLANK:
      shape = "R";
      // no other params
      break;

   case T_UNDEFINED:
   default:
      shape = "R";
      // no other params
      break;
   }

   // fractions may add up and make ikt incorrect.
   *padsize = round_off(*padsize, SMALLDELTA);
   *finlength = round_off(*finlength, SMALLDELTA);
   *finoffset = round_off(*finoffset, SMALLDELTA);

   padrot = padrot % 360;
   // if greater 360, it is because of the +90 on special oblong pads.
   if (padrot >= 180)   // this is padrot without the extra padsize/finlength rotation adjust.
      *finoffset *= -1;

   *finori = padrot % 180; // only 0..180 allowed

   if (fabs(*finoffset) < SMALLNUMBER)
      *finoffset = 0;

   if (fabs(*finoffset) > 0)
   {
      if ((fabs(*finoffset)+onemil) > (*finlength - *padsize) /2)
      {
         fprintf(flog, "Pad [%s] Offset adjusted (Offset %lg, FinLength %lg, Padsize %lg)\n",
            block->getName(), *finoffset, *finlength, *padsize);
         display_error++;

         if (fabs(*finlength - *padsize) < onemil)
            *finoffset = 0;
         else if (*finoffset < 0)
            *finoffset = (*finlength - *padsize - onemil)/-2;  // always 1 mil
         else
            *finoffset = (*finlength - *padsize - onemil)/2;
      }
   }

   return shape;
}

/******************************************************************************
* PADS_GetPADSTACKData
*  return 0x1 top
*         0x2 bottom
*         0x4 drill
*         0x8 inner
*/
static int PADS_GetPADSTACKData(const char *name, CDataList *DataList, int insertLayer,
                           double *drill, BOOL *plated, double scale, double rot,
                           int blind, int *startlayer, int *endlayer, int mirror)
{
   int typ = 0;
   Point2 point2;
   double rotation;

   *startlayer = 255;
   *endlayer = -255;
   *drill = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      // no etch data.
      if (data->getGraphicClass() == GR_CLASS_ETCH)
         continue;

      if (data->getDataType() != T_INSERT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || data->getInsert()->getInsertType() == INSERTTYPE_VIA)
         continue;

      // inside a padstack, rot = pin rotation, angle = padshape rotation inside a padstack
      rotation = rot + data->getInsert()->getAngle();

      // this is the insert of a padshape inside a padstack
      point2.x = data->getInsert()->getOriginX() * scale;
      point2.y = data->getInsert()->getOriginY() * scale;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if ( (block->getFlags() & BL_TOOL))
      {
         *drill = block->getToolSize() * scale;
         typ |= 0x4;
         *plated = block->getToolHolePlated();
      }
      else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         int layer = Get_ApertureLayer(doc, data, block, insertLayer);
         if (mirror & MIRROR_LAYERS)
            layer = doc->getLayerArray()[layer]->getMirroredLayerIndex();
         int padslayer = Layer_PADS(layer); // special deal with l->
         LayerStruct *l = doc->getLayerArray()[layer];

         if (l->getNeverMirror()) // layer appears only on top placement
         {
            fprintf(flog, "Padstack [%s] has placement dependend pad forms -> not supported in PADS.\n", name);
            display_error++;
            padslayer = LAY_TOP;
         }
         if (l->getMirrorOnly())  // layer appears only on bottom placement
         {
            fprintf(flog, "Padstack [%s] has placement dependend pad forms -> not supported in PADS.\n", name);
            display_error++;
            padslayer = LAY_TOP;
         }

         if (padslayer == -5)
            continue;

         if (padslayer == LAY_TOP)
            padslayer = -2;
         else if (padslayer == LAY_BOT)
            padslayer = 0;
         else if (padslayer == LAY_INNER)
            padslayer = -1;

         double padsize = 0.0;
         double finlength = 0.0;
         double finoffset = 0.0;
         double finori = 0.0;
         double idia = 0.0;
         CString padshape = get_padshape(block, &point2, rotation, scale, &padsize, &finlength, &finoffset, &finori, &idia);

         if (padslayer == LAY_ALL || l->getLayerType() == LAYTYPE_PAD_ALL || l->getLayerType() == LAYTYPE_SIGNAL_ALL )
         {
            save_padshape(block, -2, padsize, finlength, finoffset, finori, padshape, idia);
            save_padshape(block, -1, padsize, finlength, finoffset, finori, padshape, idia);
            save_padshape(block,  0, padsize, finlength, finoffset, finori, padshape, idia);
            typ |= 0x3;
         }
         else
         {
            if (padslayer == -2 || l->getLayerType() == LAYTYPE_PAD_TOP || l->getLayerType() == LAYTYPE_SIGNAL_TOP)
            {
               typ |= 0x1;
               padslayer = -2;
            }
            else if (l->getLayerType() == LAYTYPE_PAD_BOTTOM || l->getLayerType() == LAYTYPE_SIGNAL_BOT)
            {
               typ |= 0x2;
               padslayer = 0;
            }
            else if (padslayer == -1 || l->getLayerType() == LAYTYPE_PAD_INNER || l->getLayerType() == LAYTYPE_SIGNAL_INNER)
            {
               typ |= 0x8;
               padslayer = -1;
            }

            if (l->getElectricalStackNumber() < *startlayer)
               *startlayer = l->getElectricalStackNumber();
            if (l->getElectricalStackNumber() > *endlayer)
               *endlayer = l->getElectricalStackNumber();

            int parrptr = save_padshape(block,  padslayer, padsize, finlength, finoffset, finori, padshape, idia);
            if (parrptr > -1) // -1 is returned when a layer is already assigned.
            {
               if (!padshapearray[parrptr]->stacknr && l->getElectricalStackNumber())
               {
                  padshapearray[parrptr]->stacknr = l->getElectricalStackNumber();
               }
               else
               {
                  if (padslayer == -2)
                     padshapearray[parrptr]->stacknr = 1;
                  else if (padslayer == 0)
                     padshapearray[parrptr]->stacknr = max_signalcnt;
               }
            }
         }
      }
   }

   // if no inner_all found, use a inner_signr and copy it to inner -1
   int found_minus1 = FALSE;
   int found_inner = -1;
   int p;

   for (p=0; p<padshapecnt; p++)
   {
      if (padshapearray[p]->level > 1 && padshapearray[p]->level < max_signalcnt)
         found_inner = p;
      if (padshapearray[p]->level == -1)
         found_minus1 = TRUE;
   }

   // pads must have a generic inner.
   if (found_inner > -1 && !found_minus1)
   {
      BlockStruct *block = padshapearray[found_inner]->padshape;
      double finlength = padshapearray[found_inner]->finlength;
      double finoffset = padshapearray[found_inner]->finoffset;
      double finori = padshapearray[found_inner]->finori;
      double idia = padshapearray[found_inner]->idia;
      double padsize = padshapearray[found_inner]->size;
      CString padshape = padshapearray[found_inner]->shape;

      save_padshape(block, -1, padsize, finlength, finoffset, finori, padshape, idia);
   }

   if (!blind)
   {
      // if no BOTTOM found, use a max_signalcnt and copy it to maxsignr to 0
      int found_zero = FALSE;
      int found_bottom = -1;

      for (p=0; p<padshapecnt; p++)
      {
         if (padshapearray[p]->level == max_signalcnt)
            found_bottom = p;
         if (padshapearray[p]->level == 0)
            found_zero = TRUE;
      }

      // pads must have a bottom.
      if (found_bottom > -1 && !found_zero)
      {
         padshapearray[found_bottom]->level = 0;
      }

      // if no TOP found, use a 1 and copy it to 1 to -2
      int found_minus2 = FALSE;
      int found_top = -1;

      for (p=0; p<padshapecnt; p++)
      {
         if (padshapearray[p]->level == 1)
            found_top = p;
         if (padshapearray[p]->level == -2)
            found_minus2 = TRUE;
      }

      // pads must have a bottom.
      if (found_top> -1 && !found_minus2)
      {
         padshapearray[found_top]->level = -2;
      }
   }

   return typ;
} /* end PADS_GetPadstackData */

/*****************************************************************************/
/*
   int         level;
   int         stacknr;
   double      size;
   CString     shape;
   double      idia, finori, finlength, finoffset;
   BlockStruct *padshape;
   int         complex_error_reported;

*/
static int copy_padshape(int fromcopy, int tocopy)
{
   padshapearray[tocopy]->level = padshapearray[fromcopy]->level;
   padshapearray[tocopy]->stacknr = padshapearray[fromcopy]->stacknr;
   padshapearray[tocopy]->size = padshapearray[fromcopy]->size;
   padshapearray[tocopy]->shape = padshapearray[fromcopy]->shape;
   padshapearray[tocopy]->idia = padshapearray[fromcopy]->idia;
   padshapearray[tocopy]->finori = padshapearray[fromcopy]->finori;
   padshapearray[tocopy]->finlength = padshapearray[fromcopy]->finlength;
   padshapearray[tocopy]->finoffset = padshapearray[fromcopy]->finoffset;
   padshapearray[tocopy]->padshape = padshapearray[fromcopy]->padshape;
   padshapearray[tocopy]->complex_error_reported = padshapearray[fromcopy]->complex_error_reported;

   return 1;
}

/******************************************************************************
* do_viasection
*/
static int do_viasection(FILE *fp, double scale)
{
   int   i, p;
   int maxElectricalStackNum = GetMaxElectricalStackNum(doc);

   fprintf(fp,"*VIA*  ITEMS\n");
   fprintf(fp,"\n");
   fprintf(fp,"*REMARK*  NAME  DRILL STACKLINES [DRILL START] [DRILL END]\n");
   fprintf(fp,"*REMARK*  LEVEL SIZE SHAPE [INITIAL DIAMETER]\n");
   fprintf(fp,"\n");

   for (i=0;i<viacnt;i++)
   {
      BlockStruct *b = doc->Find_Block_by_Name(viaarray[i]->vianame, -1);
      padshapecnt = 0;
      double      drillsize = 0;

      Attrib   *a;
      int      blind = FALSE;
      if (a =  is_attvalue(doc, b->getAttributesRef(), ATT_BLINDSHAPE, 1))
         blind = TRUE;

      // return 0x1 top
      //        0x2 bottom
      //        0x4 drill
      //        0x8 inner
      int startlayer, endlayer;
      BOOL plated = TRUE;
      int typ = PADS_GetPADSTACKData( b->getName(), &(b->getDataList()), -1, &drillsize, &plated, scale, 0.0, blind,
         &startlayer, &endlayer);
      if ((startlayer == 255) && (endlayer == -255))
      {
         fprintf(flog,"Via [%s] definition incorrect. Via, [%s], written as through-hole. \n", b->getName(), check_name('v',b->getName()));
         display_error++;
         blind++;
         startlayer = 1;
         endlayer = maxElectricalStackNum;
      }

      if ((startlayer > 1) || (endlayer < maxElectricalStackNum))
         blind++;
      // make sure -2, -1 and 0 are there
      int found_minus2 = FALSE, found_minus1 = FALSE, found_0 = FALSE;
      for (p=0;p<padshapecnt;p++)
      {
#ifdef _DEBUG
PADS_PadshapeStruct *pp = padshapearray[p];
#endif
         if (padshapearray[p]->level == -2)  found_minus2 = p+1;  // top
         if (padshapearray[p]->level == -1)  found_minus1 = p+1;  // inner
         if (padshapearray[p]->level == 0)   found_0 = p+1;       // bottom
      }

      // on blind, the startlayer -2 is the startlayer of the blind via which may be layer 5
      // on blind, the endlayer 0 is the endlayer of the blind via which may be layer 6
      if (blind)
      {
#ifdef _DEBUG
PADS_PadshapeStruct *p2 = NULL;
if (found_minus2) p2 = padshapearray[found_minus2-1];
PADS_PadshapeStruct *p4 = NULL;
if (found_0)      p4 = padshapearray[found_0-1];
PADS_PadshapeStruct *p5 = NULL;
if (found_minus1) p5 = padshapearray[found_minus1-1];
#endif

         if (!found_minus1)   // if no inner found
         {
            if (found_minus2)
            {
               int ptr = save_padshape(b, -1, 0, 0, 0, 0, "R", 0);
               copy_padshape(found_minus2-1, ptr); // copy startvalues to topvalues
               found_minus1 = padshapecnt;
               padshapearray[ptr]->level = -1;
            }
            else
            if (found_0)
            {
               int ptr = save_padshape(b, -1, 0, 0, 0, 0, "R", 0);
               copy_padshape(found_0-1, ptr);   // copy startvalues to topvalues
               found_minus1 = padshapecnt;
               padshapearray[ptr]->level = -1;
            }
         }

         if (!found_minus2 && found_minus1)  // if no top found
         {
            // the inner layer becomes the start layer
            // startlayer must become -2
            int ptr = save_padshape(b, -2, 0, 0, 0, 0, "R", 0);
            copy_padshape(found_minus1-1, ptr); // copy startvalues to topvalues
            found_minus2 = padshapecnt;
            padshapearray[ptr]->level = -2;

         }
         if (!found_0 && found_minus1)       // if no bottom found
         {
            // the inner layer becomes the endlayer
            int ptr = save_padshape(b, 0, 0, 0, 0, 0, "R", 0);
            copy_padshape(found_minus1-1, ptr); // copy startvalues to topvalues
            found_0 = padshapecnt;
            padshapearray[ptr]->level = 0;
         }

         // here find what is the first and last padlayer
         int   startindex = -1, endindex = -1;
         for (p=0;p<padshapecnt;p++)
         {
#ifdef _DEBUG
PADS_PadshapeStruct *pp = padshapearray[p];
#endif
            if (!padshapearray[p]->stacknr)  continue;
            if (padshapearray[p]->stacknr == startlayer)
            {
               startindex = p;
            }
            if (padshapearray[p]->stacknr == endlayer)
            {
               endindex = p;
            }
         }

         if (startindex < 0)
         {
            startindex = found_minus2 - 1;
         }
         if (endindex < 0)
         {
            endindex = found_0 - 1;
         }

         // special treatment for single sided vias
         if (startlayer == endlayer)
         {
            // here do it normal !!!
            fprintf(fp,"%s %lg %d\n", check_name('v',viaarray[i]->vianame),
               wPads_Units(output_units_accuracy, drillsize), padshapecnt);
         }
         else
         {
#ifdef _DEBUG
PADS_PadshapeStruct *p1 = padshapearray[startindex];
PADS_PadshapeStruct *p2 = padshapearray[found_minus2-1];
PADS_PadshapeStruct *p3 = padshapearray[endindex];
PADS_PadshapeStruct *p4 = padshapearray[found_0-1];
PADS_PadshapeStruct *p5 = padshapearray[found_minus1-1];
#endif

            // here make startlayer  the same value as found_minus2
            if (startindex != found_minus2-1)
            {
               copy_padshape(startindex, found_minus2-1);   // copy startvalues to topvalues
               padshapearray[found_minus2-1]->level = -2;
            }

            // here make endlayer  the same value as found_0
            if (endindex != found_0-1)
            {
               copy_padshape(endindex, found_0-1);    // copy from end to bottom
               padshapearray[found_0-1]->level = 0;
            }
            fprintf(fp,"%s %lg %d %d %d\n", check_name('v',viaarray[i]->vianame),
               wPads_Units(output_units_accuracy, drillsize), padshapecnt, startlayer, endlayer);
         }
      }
      else
      {
         if (!found_minus2) save_padshape(b, -2, 0, 0, 0, 0, "R", 0);
         if (!found_minus1) save_padshape(b, -1, 0, 0, 0, 0, "R", 0);
         if (!found_0)      save_padshape(b, 0, 0, 0, 0, 0, "R", 0);

         fprintf(fp,"%s %lg %d\n", check_name('v',viaarray[i]->vianame),
               wPads_Units(output_units_accuracy, drillsize), padshapecnt);
      }

      // sort by layer
      for (int pp=-2;pp<=MAX_PADSLAYER;pp++)
      {
         for (p=0;p<padshapecnt;p++)
         {
            const char *vshape;
            if (padshapearray[p]->level != pp)  continue;
            vshape = padshapearray[p]->shape;
            if (STRCMPI(vshape,"R") && STRCMPI(vshape,"S"))
            {
               fprintf(flog,"Viashape [%s] not legal [%s] -> converted to Round\n", viaarray[i]->vianame, vshape);
               display_error++;
               vshape = "R";
            }
            fprintf(fp,"%d %lg %s\n", padshapearray[p]->level,
               wPads_Units(output_units_accuracy, padshapearray[p]->size),vshape);
         }
      }

      for (p=0;p<padshapecnt;p++)
         delete padshapearray[p];
      padshapecnt = 0;

      fprintf(fp,"\n");
   }

   fprintf(fp,"\n");
   return 1;
}

/******************************************************************************
* DoViaSection
*/
static int DoViaSection(FILE *fp, double scale)
{
   fprintf(fp, "*VIA*  ITEMS\n");
   fprintf(fp, "\n");
   fprintf(fp, "*REMARK*  NAME  DRILL STACKLINES [DRILL START] [DRILL END]\n");
   fprintf(fp, "*REMARK*  LEVEL SIZE SHAPE [INITIAL DIAMETER]\n");
   fprintf(fp, "\n");

   int maxElectricalStackNum = GetMaxElectricalStackNum(doc);

   for (int i=0; i<viacnt; i++)
   {
      BlockStruct *block = doc->Find_Block_by_Name(viaarray[i]->vianame, -1);
      if (!block)
         continue;

      double drillSize = 0.0;
      int startLayerNum = -1;
      int endLayerNum = -1;
      BOOL blindAndBuried = FALSE;
      CMapStringToPtr padstackMap;

      if (GetViaPadstack(block, maxElectricalStackNum, scale, drillSize, startLayerNum, endLayerNum, blindAndBuried, &padstackMap) > 0)
      {
         if (blindAndBuried)
            WriteBlinBuriedPadstack(fp, viaarray[i]->vianame, drillSize, maxElectricalStackNum, startLayerNum, endLayerNum, &padstackMap);
         else
            WriteThroughtHolePadstack(fp, viaarray[i]->vianame, drillSize, maxElectricalStackNum, &padstackMap);
      }

      // Clean up the padstack map
      POSITION pos = padstackMap.GetStartPosition();
      while (pos)
      {
         CString key = "";
         void *voidPtr;
         padstackMap.GetNextAssoc(pos, key, voidPtr);

         PADS_PadshapeStruct *pp = (PADS_PadshapeStruct*)voidPtr;
         pp->padshape = NULL;
         delete pp;
      }
      padstackMap.RemoveAll();


      //// Clean up the padstack map
      //POSITION pos = padstackMap.GetStartPosition();
      //while (pos)
      //{
      // CString key = "";
      // void *voidPtr;
      // padstackMap.GetNextAssoc(pos, key, voidPtr);
      // PadShapeArray *padshapeArray = (PadShapeArray*)voidPtr;

      // for (int i=0; i<padshapeArray->GetCount(); i++)
      // {
      //    PADS_PadshapeStruct *pp = padshapeArray[i];
      //    pp->padshape = NULL;
      //    delete pp;
      // }
      // padshapeArray->RemoveAll();
      // delete padshapeArray;
      //}
      //padstackMap.RemoveAll();
   }

   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* GetViaPadstack
*/
static int GetViaPadstack(BlockStruct *padstack, int maxElectricalStackNum, double scale, double &drillSize,
                          int &startLayerNum, int &endLayerNum, BOOL &blindAndBuried, CMapStringToPtr *padstackMap)
{
   drillSize = 0.0;
   startLayerNum = 255;
   endLayerNum = -255;

   BOOL hasTopLayer = FALSE;
   BOOL hasBotLayer = FALSE;

   POSITION pos = padstack->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *padData = padstack->getDataList().GetNext(pos);
      if (padData->getDataType() != T_INSERT)
         continue;
      if (padData->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || padData->getInsert()->getInsertType() == INSERTTYPE_VIA)
         continue;

      BlockStruct *subBlock = doc->getBlockAt(padData->getInsert()->getBlockNumber());
      if (subBlock->getFlags() & BL_TOOL)
      {
         drillSize = subBlock->getToolSize() * scale;
      }
      else if ((subBlock->getFlags() & BL_APERTURE) || (subBlock->getFlags() & BL_BLOCK_APERTURE))
      {
         int layerNum = Get_ApertureLayer(doc, padData, padstack, -1);
         LayerStruct *layer = doc->getLayerArray()[layerNum];
         if (!layer)
            continue;

         if (layer->getElectricalStackNumber() != 0 && layer->getElectricalStackNumber() < startLayerNum)
            startLayerNum = layer->getElectricalStackNumber();
         if (layer->getElectricalStackNumber() != 0 && layer->getElectricalStackNumber() > endLayerNum)
            endLayerNum = layer->getElectricalStackNumber();

         CString padstackKey = "";
         switch (layer->getLayerType())
         {
         case LAYTYPE_PAD_TOP:
            {
               // For TOP layers
               if (layer->getElectricalStackNumber() == 1)
               {
                  padstackKey = LAY_PADTOP_1;
                  hasTopLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() == 0)
               {
                  padstackKey = LAY_PADTOP_0;
                  hasTopLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() > 0)
               {
                  padstackKey.Format("%s_%d", LAY_ELESTK_N, layer->getElectricalStackNumber());
               }
            }
            break;
         case LAYTYPE_SIGNAL_TOP:
            {
               // For TOP layers
               if (layer->getElectricalStackNumber() == 1)
               {
                  padstackKey = LAY_SIGTOP_1;
                  hasTopLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() == 0)
               {
                  padstackKey = LAY_SIGTOP_0;
                  hasTopLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() > 0)
               {
                  padstackKey.Format("%s_%d", LAY_ELESTK_N, layer->getElectricalStackNumber());
               }
            }
            break;
         case LAYTYPE_PAD_BOTTOM:
            {
               // For BOTTOM layers
               if (layer->getElectricalStackNumber() == maxElectricalStackNum)
               {
                  padstackKey = LAY_PADBOT_n;
                  hasBotLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() == 0)
               {
                  padstackKey = LAY_PADBOT_0;
                  hasBotLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() > 0)
               {
                  padstackKey.Format("%s_%d", LAY_ELESTK_N, layer->getElectricalStackNumber());
               }
            }
            break;
         case LAYTYPE_SIGNAL_BOT:
            {
               // For BOTTOM layers
               if (layer->getElectricalStackNumber() == maxElectricalStackNum)
               {
                  padstackKey = LAY_SIGBOT_n;
                  hasBotLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() == 0)
               {
                  padstackKey = LAY_SIGBOT_0;
                  hasBotLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() > 0)
               {
                  padstackKey.Format("%s_%d", LAY_ELESTK_N, layer->getElectricalStackNumber());
               }
            }
            break;
         case LAYTYPE_PAD_INNER:
            {
               // For INNER layers
               if (layer->getElectricalStackNumber() > 0)
                  padstackKey.Format("%s_%d", LAY_PADINN_N, layer->getElectricalStackNumber());
               else if (layer->getElectricalStackNumber() == 0)
                  padstackKey = LAY_PADINN_0;   // INNER ALL
            }
            break;
         case LAYTYPE_SIGNAL_INNER:
            {
               // For INNER layers
               if (layer->getElectricalStackNumber() > 0)
                  padstackKey.Format("%s_%d", LAY_SIGINN_N, layer->getElectricalStackNumber());
               else if (layer->getElectricalStackNumber() == 0)
                  padstackKey = LAY_SIGINN_0;   // INNER ALL
            }
            break;
         case LAYTYPE_PAD_OUTER:
            {
               // For both TOP and BOTTOM layers
               padstackKey = LAY_PADOUT;
            }
            break;
         case LAYTYPE_SIGNAL_OUTER:
            {
               // For both TOP and BOTTOM layers
               padstackKey = LAY_SIGOUT;
            }
            break;
         case LAYTYPE_PAD_ALL:
            {
               // For TOP, BOTTOM, and INNER layers
               padstackKey = LAY_PADALL;
            }
            break;
         case LAYTYPE_SIGNAL_ALL:
            {
               // For TOP, BOTTOM, and INNER layers
               padstackKey = LAY_SIGALL;
            }
            break;
         default:
            {
               if (layer->getElectricalStackNumber() == 1)
               {
                  // For TOP layers
                  padstackKey = LAY_ELESTK_1;
                  hasTopLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() == maxElectricalStackNum)
               {
                  // For BOTTOM layers
                  padstackKey = LAY_ELESTK_n;
                  hasBotLayer = TRUE;
               }
               else if (layer->getElectricalStackNumber() > 0)
               {
                  // For INNER layers
                  padstackKey = LAY_ELESTK_N;
               }
               else
               {
                  continue;
               }
            }
            break;
         }

         // this is the insert of a padshape inside a padstack
         Point2 point;
         point.x = padData->getInsert()->getOriginX() * scale;
         point.y = padData->getInsert()->getOriginY() * scale;

         // Get the shape of the padshape
         double size = 0.0;
         double finLength = 0.0;
         double finOffset = 0.0;
         double finOrigin = 0.0;
         double innerDiameter = 0.0;
         double rot = padData->getInsert()->getAngle();
         CString shape = get_padshape(subBlock, &point, rot, scale, &size, &finLength, &finOffset, &finOrigin, &innerDiameter);


         // Add the padshape to the map
         PADS_PadshapeStruct *pp = NULL;
         void *voidPtr = NULL;
         if (!padstackMap->Lookup(padstackKey, voidPtr))
         {
            pp = new PADS_PadshapeStruct;
            pp->shape = shape;
            pp->padshape = subBlock;
            pp->level = layer->getElectricalStackNumber();
            pp->stacknr = layer->getElectricalStackNumber();
            pp->size = size;
            pp->finlength = finLength;
            pp->finoffset = finOffset;
            pp->finori = finOrigin;
            pp->idia = innerDiameter;
            pp->complex_error_reported = 0;
            padstackMap->SetAt(padstackKey, pp);
         }
         else
         {
            pp  = (PADS_PadshapeStruct*)voidPtr;
            if (size > pp->size)
            {
               pp->shape = shape;
               pp->padshape = subBlock;
               pp->level = layer->getElectricalStackNumber();
               pp->stacknr = layer->getElectricalStackNumber();
               pp->size = size;
               pp->finlength = finLength;
               pp->finoffset = finOffset;
               pp->finori = finOrigin;
               pp->idia = innerDiameter;
               pp->complex_error_reported = 0;
            }
         }
      }
   }

   if (hasTopLayer && hasBotLayer)
      blindAndBuried = FALSE;
   else
      blindAndBuried = TRUE;

   if (startLayerNum == 255)
      startLayerNum = 0;
   if (endLayerNum == -255)
      endLayerNum = 0;

   return 1;
}

/******************************************************************************
* WriteBlinBuriedPadstack
*/
static void WriteBlinBuriedPadstack(FILE *fp, CString viaName, double drillSize, int maxElectricalStackNum,
                                    int startLayerNum, int endLayerNum, CMapStringToPtr *padstackMap)
{
   void *voidPtr = NULL;
   BOOL hasInnerAll = FALSE;

   int padshapeCount = (endLayerNum - startLayerNum + 1) + 3;
   if (padstackMap->Lookup(LAY_PADINN_0, voidPtr) || padstackMap->Lookup(LAY_SIGINN_0, voidPtr) ||
       padstackMap->Lookup(LAY_PADALL, voidPtr) || padstackMap->Lookup(LAY_SIGALL, voidPtr))
   {
      hasInnerAll = TRUE;
      startLayerNum = 1;
      endLayerNum = maxElectricalStackNum - 1;
      padshapeCount = maxElectricalStackNum + 3;
   }

   // Output the definition of the padstack
   drillSize = wPads_Units(output_units_accuracy, drillSize);
   fprintf(fp, "%s %lg %d %d %d\n", check_name('v', viaName), drillSize, padshapeCount, startLayerNum, endLayerNum);
   fprintf(fp, "-2 0 R\n");
   fprintf(fp, "-1 0 R\n");
   fprintf(fp, "0 0 R\n");

   // Output the TOP padshape
   PADS_PadshapeStruct *pp = NULL;
   if (padstackMap->Lookup(LAY_PADTOP_1, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADTOP_0, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGTOP_1, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGTOP_0, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_ELESTK_1, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADOUT, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGOUT, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADALL, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGALL, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;

   if (pp)
   {
      if (STRCMPI(pp->shape, "R") && STRCMPI(pp->shape, "S"))
      {
         fprintf(flog, "Viashape [%s] not legal [%s] -> converted to Round\n", viaName, pp->shape);
         display_error++;
         pp->shape = "R";
      }

      double size = wPads_Units(output_units_accuracy, pp->size);
      fprintf(fp, "%d %lg %s\n", startLayerNum, size, pp->shape);
   }

   // Output individual inner padshape
   for (int i=2; i<maxElectricalStackNum; i++)
   {
      CString PADINN_N = "";
      CString SIGINN_N = "";
      CString ELESTK_N = "";
      PADINN_N.Format("%s_%d", LAY_PADINN_N, i);
      SIGINN_N.Format("%s_%d", LAY_SIGINN_N, i);
      ELESTK_N.Format("%s_%d", LAY_ELESTK_N, i);

      pp = NULL;
      if (padstackMap->Lookup(PADINN_N, voidPtr))
         pp = (PADS_PadshapeStruct*)voidPtr;
      else if (padstackMap->Lookup(SIGINN_N, voidPtr))
         pp = (PADS_PadshapeStruct*)voidPtr;
      else if (padstackMap->Lookup(ELESTK_N, voidPtr))
         pp = (PADS_PadshapeStruct*)voidPtr;
      else if (padstackMap->Lookup(LAY_PADINN_0, voidPtr))
         pp = (PADS_PadshapeStruct*)voidPtr;
      else if (padstackMap->Lookup(LAY_SIGINN_0, voidPtr))
         pp = (PADS_PadshapeStruct*)voidPtr;
      else if (padstackMap->Lookup(LAY_PADALL, voidPtr))
         pp = (PADS_PadshapeStruct*)voidPtr;
      else if (padstackMap->Lookup(LAY_SIGALL, voidPtr))
         pp = (PADS_PadshapeStruct*)voidPtr;

      if (pp)
      {
         if (STRCMPI(pp->shape, "R") && STRCMPI(pp->shape, "S"))
         {
            fprintf(flog, "Viashape [%s] not legal [%s] -> converted to Round\n", viaName, pp->shape);
            display_error++;
            pp->shape = "R";
         }

         double size = wPads_Units(output_units_accuracy, pp->size);
         fprintf(fp, "%d %lg %s\n", i, size, pp->shape);
      }
   }

   // Output the BOTTOM padshape
   pp = NULL;
   if (padstackMap->Lookup(LAY_PADBOT_n, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADBOT_0, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGBOT_n, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGBOT_0, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_ELESTK_n, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADOUT, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGOUT, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADALL, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGALL, voidPtr))
      pp = (PADS_PadshapeStruct*)voidPtr;

   if (pp)
   {
      if (STRCMPI(pp->shape, "R") && STRCMPI(pp->shape, "S"))
      {
         fprintf(flog, "Viashape [%s] not legal [%s] -> converted to Round\n", viaName, pp->shape);
         display_error++;
         pp->shape = "R";
      }

      double size = wPads_Units(output_units_accuracy, pp->size);
      fprintf(fp, "%d %lg %s\n", endLayerNum + 1, size, pp->shape);
   }

   fprintf(fp, "\n");
}

/******************************************************************************
* WriteThroughtHolePadstack
*/
static void WriteThroughtHolePadstack(FILE *fp, CString viaName, double drillSize, int maxElectricalStackNum,
                                      CMapStringToPtr *padstackMap)
{
   // Output the definition of the padstack
   drillSize = wPads_Units(output_units_accuracy, drillSize);
   fprintf(fp, "%s %lg 3\n", check_name('v', viaName), drillSize);

   void *voidPtr = NULL;

   // Find the TOP pad
   PADS_PadshapeStruct *topPad = NULL;
   if (padstackMap->Lookup(LAY_PADTOP_1, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADTOP_0, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGTOP_1, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGTOP_0, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_ELESTK_1, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADOUT, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGOUT, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADALL, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGALL, voidPtr))
      topPad = (PADS_PadshapeStruct*)voidPtr;

   // Find the INNER pad
   PADS_PadshapeStruct *innerPad = NULL;
   for (int i=2; i<maxElectricalStackNum; i++)
   {
      CString PADINN_N = "";
      PADINN_N.Format("%s_%d", LAY_PADINN_N, i);

      if (padstackMap->Lookup(PADINN_N, voidPtr))
      {
         innerPad = (PADS_PadshapeStruct*)voidPtr;
         break;
      }
   }
   if (!innerPad && padstackMap->Lookup(LAY_PADINN_0, voidPtr))
      innerPad = (PADS_PadshapeStruct*)voidPtr;
   if (!innerPad)
   {
      for (int i=2; i<maxElectricalStackNum; i++)
      {
         CString SIGINN_N = "";
         SIGINN_N.Format("%s_%d", LAY_SIGINN_N, i);

         if (padstackMap->Lookup(SIGINN_N, voidPtr))
         {
            innerPad = (PADS_PadshapeStruct*)voidPtr;
            break;
         }
      }
   }
   if (!innerPad && padstackMap->Lookup(LAY_SIGINN_0, voidPtr))
      innerPad = (PADS_PadshapeStruct*)voidPtr;
   if (!innerPad && padstackMap->Lookup(LAY_PADALL, voidPtr))
      innerPad = (PADS_PadshapeStruct*)voidPtr;
   if (!innerPad && padstackMap->Lookup(LAY_SIGALL, voidPtr))
      innerPad = (PADS_PadshapeStruct*)voidPtr;

   // Find the BOTTOM pad
   PADS_PadshapeStruct *botPad = NULL;
   if (padstackMap->Lookup(LAY_PADBOT_n, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADBOT_0, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGBOT_n, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGBOT_0, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_ELESTK_n, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADOUT, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGOUT, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_PADALL, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;
   else if (padstackMap->Lookup(LAY_SIGALL, voidPtr))
      botPad = (PADS_PadshapeStruct*)voidPtr;



   // Write the TOP pad, layer number -2 in PADS
   if (topPad)
   {
      if (STRCMPI(topPad->shape, "R") && STRCMPI(topPad->shape, "S"))
      {
         fprintf(flog, "Viashape [%s] not legal [%s] -> converted to Round\n", viaName, topPad->shape);
         display_error++;
         topPad->shape = "R";
      }

      double size = wPads_Units(output_units_accuracy, topPad->size);
      fprintf(fp, "-2 %lg %s\n", size, topPad->shape);
   }

   // Write the INNER pad, layer number -1 in PADS
   if (!innerPad || innerPad == botPad)
      innerPad = topPad;

   if (innerPad)
   {
      if (STRCMPI(innerPad->shape, "R") && STRCMPI(innerPad->shape, "S"))
      {
         fprintf(flog, "Viashape [%s] not legal [%s] -> converted to Round\n", viaName, innerPad->shape);
         display_error++;
         innerPad->shape = "R";
      }

      double size = wPads_Units(output_units_accuracy, innerPad->size);
      fprintf(fp, "-1 %lg %s\n", size, innerPad->shape);
   }

   // Write the BOTTOM pad, layer number 0 in PADS
   if (botPad)
   {
      if (STRCMPI(botPad->shape, "R") && STRCMPI(botPad->shape, "S"))
      {
         fprintf(flog, "Viashape [%s] not legal [%s] -> converted to Round\n", viaName, botPad->shape);
         display_error++;
         botPad->shape = "R";
      }

      double size = wPads_Units(output_units_accuracy, botPad->size);
      fprintf(fp, "0 %lg %s\n", size, botPad->shape);
   }

   fprintf(fp, "\n");
}

/******************************************************************************
* do_partdecalsection
*/
static int do_partdecalsection(FILE *fp, double scale, double padsUnitsPerPageUnit)
{
   fprintf(fp,"*PARTDECAL*  ITEMS\n");
   fprintf(fp,"\n");
   fprintf(fp,"*REMARK* NAME ORIX ORIY PIECES TERMINALS STACKS TEXT LABELS\n");
   fprintf(fp,"*REMARK* PIECETYPE CORNERS WIDTHHGHT LEVEL RESTRICTIONS\n");
   fprintf(fp,"*REMARK* XLOC YLOC BEGINANGLE DELTAANGLE\n");
   fprintf(fp,"*REMARK* XLOC YLOC ORI LEVEL HEIGHT WIDTH MIRRORED HJUST VJUST\n");
   fprintf(fp,"*REMARK* VISIBLE XLOC YLOC ORI HEIGTH WIDTH LEVEL MIRRORED HJUST VJUST RIGHTREADING\n");
   fprintf(fp,"*REMARK* T XLOC YLOC NMXLOC NMYLOC\n");
   fprintf(fp,"*REMARK* PAD PIN STACKLINES\n");
   fprintf(fp,"*REMARK* LEVEL SIZE SHAPE IDIA DRILL [PLATED]\n");
   fprintf(fp,"*REMARK* LEVEL SIZE SHAPE FINORI FINLENGTH FINOFFSET DRILL [PLATED]\n");
   fprintf(fp,"\n");


   // here write all blocks
   // create_pin
   // create_via and make name different from Padstack used for PINS and VIAS.
   // create_generic
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
      if (block->getFlags() & BL_TOOL)
         continue;
      if (block->getFlags() & BL_APERTURE)
         continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)
         continue;
      if (block->getFlags() & BL_FILE)
         continue;

      switch (block->getBlockType())
      {
      case BLOCKTYPE_DRAWING:
      case BLOCKTYPE_GENERICCOMPONENT:
      case BLOCKTYPE_MECHCOMPONENT:
      case BLOCKTYPE_PCBCOMPONENT:
         // do it
         write_DECALData(fp, block, &block->getDataList(), scale, padsUnitsPerPageUnit);
         break;
      default:
         // do nothing
         // blocktype_unknown is type 0 and done above
         break;
      }
   }

   fprintf(fp, "\n");
   return 1;
}

/*****************************************************************************/
/*
*/
static int get_gatecnt(int blocknum, int *pincnt)
{
   int   i;
   int   pinnamefound = FALSE;

   *pincnt = 0;
   for (i=0;i<decalPinNameArray.GetSize();i++)
   {
      if (decalPinNameArray[i]->blocknr != blocknum)  continue;
      *pincnt = *pincnt+1;
      CString pinnr;
      pinnr.Format("%d", decalPinNameArray[i]->pinnr);
      if (pinnr.Compare(decalPinNameArray[i]->pinname))  // if name is not the same.
         pinnamefound = TRUE;
   }

   return pinnamefound;
}

/*****************************************************************************/
/*
*/
static int write_parttype(FILE *fp, BlockStruct *block, const char *typname)
{
   CString  decalname;

   decalname = check_name('g', block->getName());
   // a gate information only needs to be written if pinnames occur.
   int pincnt;
   int gatecnt = get_gatecnt(block->getBlockNumber(), &pincnt);
   fprintf(fp,"%s %s I UND  %d  0  %d  0  Y\n", typname, decalname, gatecnt, (gatecnt)?pincnt:0);
   // write gate information -- all is assumed to have 1 gate
   // but this is needed to write pinnames vs. pinnumbers.
   if (gatecnt)
   {
      int i, cnt;
      fprintf(fp,"G 0 %d\n", pincnt);

      decalPinNameArray.normalizePinNumbers();

      cnt = 0;
      for (i=0;i<decalPinNameArray.GetSize();i++)
      {
         if (decalPinNameArray[i]->blocknr != block->getBlockNumber())   continue;

         if ((++cnt % 7) == 6)   fprintf(fp,"\n");
            fprintf(fp,"%d.0.U ", decalPinNameArray[i]->m_normalizedPinNumber);
      }
      fprintf(fp,"\n");
      cnt = 0;
      for (i=0;i<decalPinNameArray.GetSize();i++)
      {
         if (decalPinNameArray[i]->blocknr != block->getBlockNumber())   continue;

         if ((++cnt % 7) == 6)   fprintf(fp,"\n");
         fprintf(fp,"%s ", check_name('e',decalPinNameArray[i]->pinname));
      }
      fprintf(fp,"\n");
   }

   return 1;
}

/*****************************************************************************/
/*
   check if this name (blockname) is already defined as a type with a different block
*/
static int type_already_used(CTypeList *TypeList, const char *name, int blocknum)
{
   TypeStruct *typ;
   POSITION typPos;
   CString  typname;

   typPos = TypeList->GetHeadPosition();
   while (typPos != NULL)
   {
      typ = TypeList->GetNext(typPos);
      typname = check_name('t', typ->getName());
      if (typname.CompareNoCase(name) == 0)
      {
         // if the name is the same a type, thats good.
         if (typ->getBlockNumber() == blocknum)   return FALSE;
         return TRUE;
      }
   }

   return FALSE;
}

/*****************************************************************************/
/*
*/
static int update_parttype(const char *typname, BlockStruct *block)
{
   for (int i=0;i<parttypecnt;i++)
   {
      PADS_PartType *parttype = parttypearray[i];

      // MUST use case sensitive comparison because device type is case sensitive - Lynn fixed TSR #3815
      if (!parttype->typname.Compare(typname))
         return i;
   }

   PADS_PartType *t = new PADS_PartType;
   parttypearray.SetAtGrow(parttypecnt, t);
   parttypecnt++;
   t->typname = typname;
   t->decalname = block->getName();
   t->block = block;

   return parttypecnt-1;
}

/******************************************************************************
* do_parttypesection
*/
static int do_parttypesection(FILE *fp, CTypeList *TypeList, double scale)
{
   fprintf(fp, "*PARTTYPE*   ITEMS\n");
   fprintf(fp, "\n");
   fprintf(fp, "*REMARK* NAME DECALNM UNITS TYPE GATES SIGPINS PINNMS FLAGS\n");
   fprintf(fp, "*REMARK* G/S SWAPTYPE PINS\n");
   fprintf(fp, "*REMARK* PIN.SWAPTYPE.PINTYPE.FUNCNAME\n");
   fprintf(fp, "*REMARK* SIGPIN PIN SIGNAME\n");
   fprintf(fp, "\n");

   // now here also write the typelist
   POSITION typPos = TypeList->GetHeadPosition();
   while (typPos != NULL)
   {
      TypeStruct *typ = TypeList->GetNext(typPos);
      BlockStruct *block = doc->Find_Block_by_Num(typ->getBlockNumber());  // test if typelist entry has a geom

      if (!block)
         continue;

      update_parttype(typ->getName(), block);
   }


   // this can not overwrite a type block which already has is done.
	int i=0;
	for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
      if (block->getFlags() & BL_TOOL)
         continue;
      if (block->getFlags() & BL_APERTURE)
         continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)
         continue;
      if (block->getFlags() & BL_FILE)
         continue;

      switch (block->getBlockType())
      {
         case BLOCKTYPE_DRAWING:
         case BLOCKTYPE_GENERICCOMPONENT:
         case BLOCKTYPE_MECHCOMPONENT:
         case BLOCKTYPE_PCBCOMPONENT:
         {
            update_parttype(block->getName(), block);
         }
         break;
         default:
            // do nothing
            // blocktype_unknown is type 0 and done above
         break;
      }
   }

   for (i=0; i<parttypecnt; i++)
   {
      CString typname = check_name('t', parttypearray[i]->typname);
      write_parttype(fp, parttypearray[i]->block, typname);
      fprintf(fp, "\n");
   }

   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* write_textsection
*/
static int write_textsection(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
                   double rotation, int mirror, double scale, int insertLayer)
{
   Mat2x2 m;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);
      if (data->getDataType() != T_INSERT)
      {
         int layer = data->getLayerIndex();

         if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         layer = doc->get_layer_mirror(layer, mirror);

         int padslayer = Layer_PADS(layer);

         if (padslayer == -1)
            padslayer = 1;
         else if (padslayer == -2)
            padslayer = max_signalcnt;
         else if (padslayer == -4)
            padslayer = 0;

         if (padslayer < 0)
            continue;
         wPADS_Graph_LevelNum(padslayer);
      }

      switch(data->getDataType())
      {
         case T_TEXT:
         {
            Point2 point2;
            point2.x = (data->getText()->getPnt().x) * scale;
            if (mirror)
               point2.x = -point2.x;
            point2.y = (data->getText()->getPnt().y) * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            double text_rot = rotation + data->getText()->getRotation();
            bool text_mirror = data->getText()->isMirrored() != ((mirror & MIRROR_FLIP) != 0) ;  // text is mirrored if mirror is set or text.mirror but not if none or both
            double w = doc->getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA() * scale;

            wPADS_Graph_Text(data->getText()->getText(), point2.x, point2.y, data->getText()->getHeight() * scale,
               data->getText()->getWidth() * scale, text_rot, text_mirror, w, TRUE, data->getText()->getHorizontalPosition(), data->getText()->getVerticalPosition());
         }
         break;

         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (block->getBlockType() == BLOCKTYPE_DRAWING)
               break;   // done as a decal in comp section
            if (block->getBlockType() == BLOCKTYPE_GENERICCOMPONENT)
               break;   // done as a decal in comp section
            if (block->getBlockType() == BLOCKTYPE_MECHCOMPONENT)
               break;   // done as a decal in comp section
            if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
               break;   // done as a decal in comp section

            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {

            }
            else // not aperture
            {
               int block_layer = insertLayer;
               if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
                  block_layer = data->getLayerIndex();

               write_textsection(fp, &(block->getDataList()), point2.x, point2.y,
                  block_rot, block_mirror, scale * data->getInsert()->getScale(), block_layer);
            }
         }
         break;

      }
   }
   return 1;
}

/******************************************************************************
* component_attributes
*/
static int component_attributes(FILE *fp,CAttributes* map, int write, double scale)
{
   int attrcnt = 0;
   char *visible = "NONE";
   double ax = 0, ay = 0, aori = 0, aheight = 0, awidth = 0;
   int alevel = 1;

   Attrib *a = is_attvalue(doc, map, ATT_REFNAME, 1);

   if (a)
   {
      if (a->isVisible())
      {
         visible = "VALUE";
         ax = a->getX() * scale;
         ay = a->getY() * scale;
         aheight = a->getHeight() * scale * TEXTCORRECT;
         awidth = doc->getWidthTable()[a->getPenWidthIndex()]->getSizeA() * scale;
         aori = RadToDeg(a->getRotationRadians());

         int padslayer = Layer_PADS(a->getLayerIndex());

         if (padslayer > -1)
            alevel = padslayer;
         else
            visible = "NONE";
      }
   }

   attrcnt++;

   if (write)
   {
      // PADS crashes if height is too large
      if (aheight > (onemil * 1000))
         aheight = onemil * 500;

      fprintf(fp, "%s %lg %lg %1.1lf %d %lg %lg N   LEFT   DOWN\n", visible,
         wPads_Units(output_units_accuracy, ax), wPads_Units(output_units_accuracy, ay),
         aori, alevel, wPads_Units(output_units_accuracy, aheight), wPads_Units(output_units_accuracy, awidth));

      fprintf(fp, "Ref.Des.\n");
   }

   visible = "NONE";
   ax = 0, ay = 0, aori = 0, aheight = 0, awidth = 0;
   alevel = 1;
   attrcnt++;

   if (write)
   {
      fprintf(fp, "%s %lg %lg %1.3lf %d %lg %lg N   LEFT   DOWN\n", visible,
         wPads_Units(output_units_accuracy, ax), wPads_Units(output_units_accuracy, ay),
         aori, alevel, wPads_Units(output_units_accuracy, aheight), wPads_Units(output_units_accuracy, awidth));

      fprintf(fp, "Part Type\n");
   }

   if (map == NULL)
      return attrcnt;
   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      WORD keyword;
      Attrib* attrib;
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))              continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))              continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))            continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))         continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0))             continue;
      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))           continue;
      if (keyword == doc->IsKeyWord(BOARD_THICKNESS, 0))          continue;
      if (keyword == doc->IsKeyWord(ATT_FIDUCIAL_PLACE_BOTH, 0))  continue;

      char *tok;
      char *temp = STRDUP(get_attvalue_string(doc,attrib));

      if ((tok = strtok(temp, "\n")) == NULL)
      {
         char *visible = "NONE";
         double ax = 0, ay = 0, aori = 0, aheight = 0, awidth = 0;
         int alevel = 1;

         if (attrib->isVisible())
         {
            visible = "VALUE";
            ax = attrib->getX() * scale;
            ay = attrib->getY() * scale;
            aheight = attrib->getHeight() * scale * TEXTCORRECT;
            awidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA() * scale;
            aori = RadToDeg(attrib->getRotationRadians());

            int padslayer = Layer_PADS(attrib->getLayerIndex());

            if (padslayer > -1)
               alevel = padslayer;
            else
               visible = "NONE";
         }

         attrcnt++;

         if (write)
         {
            // PADS crashes if height is too large
            if (aheight > (onemil * 1000))
               aheight = onemil * 500;

            fprintf(fp, "%s %lg %lg %1.1lf %d %lg %lg N   LEFT   DOWN\n", visible,
               wPads_Units(output_units_accuracy, ax), wPads_Units(output_units_accuracy, ay),
               aori, alevel, wPads_Units(output_units_accuracy, aheight), wPads_Units(output_units_accuracy, awidth));

            fprintf(fp,"%s\n", doc->getKeyWordArray()[keyword]->out);  // multiple values are delimited by \n
         }
      }
      else
      {
         while (tok)
         {
            char *visible = "NONE";
            double ax = 0, ay = 0, aori = 0, aheight = 0, awidth = 0;
            int alevel = 1;

            if (attrib->isVisible())
            {
               visible = "VALUE";
               ax = attrib->getX() * scale;
               ay = attrib->getY() * scale;
               aheight = attrib->getHeight() * scale * TEXTCORRECT;
               awidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
               aori = RadToDeg(attrib->getRotationRadians());

               int padslayer = Layer_PADS(attrib->getLayerIndex());
               if (padslayer > -1)
                  alevel = padslayer;
               else
                  visible = "NONE";
            }

            attrcnt++;

            if (write)
            {
               // PADS crashes if height is too large
               if (aheight > (onemil * 1000))
                  aheight = onemil * 500;

               fprintf(fp, "%s %lg %lg %1.1lf %d %lg %lg N   LEFT   DOWN\n", visible,
                  wPads_Units(output_units_accuracy, ax), wPads_Units(output_units_accuracy, ay),
                  aori, alevel, wPads_Units(output_units_accuracy, aheight), wPads_Units(output_units_accuracy, awidth));

               fprintf(fp,"%s\n", doc->getKeyWordArray()[keyword]->out);  // multiple values are delimited by \n
            }

            tok = strtok(NULL, "\n");
         }
      }

      free(temp);
   }

   return attrcnt;
}

/******************************************************************************
* write_attributes
*/
static void write_attributes(FILE *fp,CAttributes* map)
{
   if (map == NULL)
      return;
   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      WORD keyword;
      Attrib* attrib;
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))              continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))              continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))            continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))         continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0))             continue;
      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))           continue;
      if (keyword == doc->IsKeyWord(BOARD_THICKNESS, 0))          continue;
      if (keyword == doc->IsKeyWord(ATT_FIDUCIAL_PLACE_BOTH, 0))  continue;

      char *tok;
      char *temp = STRDUP(get_attvalue_string(doc, attrib));

      if ((tok = strtok(temp, "\n")) == NULL)
      {
         fprintf(fp, "\"%s\"\n", doc->getKeyWordArray()[keyword]->out);   // multiple values are delimited by \n
      }
      else
      {
         while (tok)
         {
            fprintf(fp, "\"%s\" %s\n", doc->getKeyWordArray()[keyword]->out, tok); // multiple values are delimited by \n
            tok = strtok(NULL, "\n");
         }
      }

      free(temp);
   }

   return;
}

static void write_designRuleLayers(FILE* fp)
{
   int maxElectricalLayer = 0;
   CLayerArray& layerArray = doc->getLayerArray();

   for (int layerIndex = 0;layerIndex < layerArray.GetCount();layerIndex++)
   {
      LayerStruct* layer = layerArray.GetAt(layerIndex);

      if (layer != NULL)
      {
         if (layer->getElectricalStackNumber() > maxElectricalLayer)
         {
            maxElectricalLayer = layer->getElectricalStackNumber();
         }
      }
   }

   for (int electricalLayerNumber = 1;electricalLayerNumber <= maxElectricalLayer;electricalLayerNumber++)
   {
      fprintf(fp,"VALID_LAYER %d\n",electricalLayerNumber);
   }
}

static void write_designRuleVias(FILE* fp)
{
   for (int viaIndex=0;viaIndex < viacnt;viaIndex++)
   {
      CString viaName = check_name('v',viaarray[viaIndex]->vianame);

      fprintf(fp,"VALID_VIA_TYPE %s\n",viaName);
   }
}

static void write_designRules(FILE* fp)
{
   fprintf(fp,
"NET_CLASS DATA\n"
"GROUP DATA\n"
"DESIGN RULES\n"
"{\n"
"RULE_SET (3)\n"
"{\n"
"FOR :\n"
"{\n"
"DEFAULT :\n"
"}\n"
"AGAINST :\n"
"{\n"
"DEFAULT :\n"
"}\n"
"LAYER 0\n"
"ROUTE_RULE :\n"
"{\n"
"LENGTH_MINIMIZATION_TYPE 1\n"
"VIA_SHARE Y\n"
"TRACE_SHARE Y\n"
"AUTO_ROUTE Y\n"
"RIPUP Y\n"
"SHOVE Y\n"
"ROUTE_PRIORITY 3\n"
"MAX_NUMBER_OF_VIAS -1\n"
   );

   write_designRuleLayers(fp);
   write_designRuleVias(fp);

   fprintf(fp,
"}\n"
"}\n"
"}\n\n"
   );

}

/*****************************************************************************/
/*
LAYER_NAME Inner Layer 2
LAYER_TYPE ROUTING
PLANE MIXED
ROUTING_DIRECTION VERTICAL
ROUTABLE Y
VISIBLE Y
SELECTABLE Y
LAYER_THICKNESS 0.008
COPPER_THICKNESS 0.001
DIELECTRIC 3.800000
NET +3.3V
NET +FCOREV
COST 0

*/
static int write_layerattributes(FILE *fp)
{
   // loop layer 1..30
   for (int l=1; l<=30; l++)
   {
      fprintf(fp, "LAYER %d\n", l);
      fprintf(fp, "{\n");
      fprintf(fp, "LAYER_NAME %s\n", pads_layer[l-1]);

      fprintf(fp, "VISIBLE Y\n");
      fprintf(fp, "SELECTABLE Y\n");

      int layertype = 0;
      int typ = 'D';

      for (int i=0; i<maxArr; i++)
      {

#ifdef _DEBUG
   PADSLayerStruct *pp = padsArr[i];
#endif
         if (atoi(padsArr[i]->newName) != l)
            continue;

         // cuold be assigned a signal, but it is really a plane
         if (typ != 'P')
            typ = padsArr[i]->type;

         LayerStruct *layer = doc->getLayerArray()[padsArr[i]->layerindex];
         layertype = layer->getLayerType();
         Attrib *a = is_attvalue(doc, layer->getAttributesRef(), ATT_NETNAME, 1);

         if (a)
         {
            // can have multiple names
            CString v = get_attvalue_string(doc, a);
            char tmp[1000];
            strcpy(tmp, v);
            char *lp = strtok(tmp, "\n");

            while (lp)
            {
               fprintf(fp, "NET %s\n", lp);
               lp = strtok(NULL, "\n");
               typ = 'P';
            }
         }
      }

      // routing layers
      if (l >= 1 && l <= max_signalcnt)
      {
         fprintf(fp, "LAYER_TYPE ROUTING\n");
         fprintf(fp, "ROUTABLE Y\n");
      }

      if (typ == 'P')
         fprintf(fp, "PLANE mixed\n");
      else
         fprintf(fp, "PLANE NONE\n");

      switch (layertype)
      {
         case LAYTYPE_PASTE_TOP:
         case LAYTYPE_PASTE_BOTTOM:
         case LAYTYPE_PASTE_ALL:
            fprintf(fp, "LAYER_TYPE PASTE_MASK\n");
         break;
         case LAYTYPE_MASK_TOP:
         case LAYTYPE_MASK_BOTTOM:
         case LAYTYPE_MASK_ALL:
            fprintf(fp, "LAYER_TYPE SOLDER_MASK\n");
         break;
         case LAYTYPE_SILK_TOP:
         case LAYTYPE_SILK_BOTTOM:
            fprintf(fp, "LAYER_TYPE SILK_SCREEN\n");
         break;
         case LAYTYPE_DRILL:
            fprintf(fp, "LAYER_TYPE DRILL\n");
         break;
      }

      // layermap has the CAMCAD type (all execpt Assembly) stored with PADS level number.
      if (l==1)   // top layer
      {
         fprintf(fp, "COMPONENT Y\n");
         int ptr;
         if ((ptr = get_layermap(LAYTYPE_SILK_TOP)) > 0)
            fprintf(fp, "ASSOCIATED_SILK_SCREEN %s\n", pads_layer[ptr-1]);
         if ((ptr = get_layermap(LAYTYPE_PASTE_TOP)) > 0)
            fprintf(fp, "ASSOCIATED_PASTE_MASK %s\n", pads_layer[ptr-1]);
         if ((ptr = get_layermap(LAYTYPE_MASK_TOP)) > 0)
            fprintf(fp, "ASSOCIATED_SOLDER_MASK %s\n", pads_layer[ptr-1]);
         // not avail in CAMCAD.
         //ASSOCIATED_ASSEMBLY Assembly Drawing Top
      }
      else if (l==max_signalcnt)
      {
         fprintf(fp, "COMPONENT Y\n");
         int ptr;
         if ((ptr = get_layermap(LAYTYPE_SILK_BOTTOM)) > 0)
            fprintf(fp, "ASSOCIATED_SILK_SCREEN %s\n",pads_layer[ptr-1]);
         if ((ptr = get_layermap(LAYTYPE_PASTE_BOTTOM)) > 0)
            fprintf(fp, "ASSOCIATED_PASTE_MASK %s\n", pads_layer[ptr-1]);
         if ((ptr = get_layermap(LAYTYPE_MASK_BOTTOM)) > 0)
            fprintf(fp, "ASSOCIATED_SOLDER_MASK %s\n", pads_layer[ptr-1]);
         // not avail in CAMCAD
         //ASSOCIATED_ASSEMBLY Assembly Drawing Top   30
      }

      fprintf(fp, "}\n");
   }
   return 1;
}

/******************************************************************************
* write_decalattributes
*/
static int write_decalattributes(FILE *fp)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct* block = doc->getBlockAt(i);
      if (!block || block->getBlockType() != blockTypePcbComponent)
         continue;

      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         continue ;

      if(block->getAttributes() && !block->getAttributes()->IsEmpty())
      {
         fprintf(fp, "DECAL %s\n", check_name('g', block->getName()));

         fprintf(fp, "{\n");
         write_attributes(fp, block->getAttributesRef());
         fprintf(fp, "}\n");
      }
   }
   return 0;
}

/******************************************************************************
* write_partattributes
*/
static int write_partattributes(FILE *fp, CDataList *DataList)
{
   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (block->getBlockType() != BLOCKTYPE_DRAWING && block->getBlockType() != BLOCKTYPE_GENERICCOMPONENT &&
          block->getBlockType() != BLOCKTYPE_MECHCOMPONENT && block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         continue ;

      CString refname = check_name('c', data->getInsert()->getRefname());
      fprintf(fp, "PART %s\n", refname);

      fprintf(fp, "{\n");
      write_attributes(fp, data->getAttributesRef());
      fprintf(fp, "}\n");
   }
   return 1;
}

/******************************************************************************
* write_partsection
*/
static int write_partsection(FILE *fp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
                   double rotation, int mirror, double scale, int insertLayer)
{
   int nonamecnt = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;

      if (mirror)
         point2.x = -point2.x;

      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (block->getBlockType() != BLOCKTYPE_DRAWING && block->getBlockType() != BLOCKTYPE_GENERICCOMPONENT &&
          block->getBlockType() != BLOCKTYPE_MECHCOMPONENT && block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         continue;

      CString refname = data->getInsert()->getRefname();

      if (data->getInsert()->getScale() != 1)
      {
         fprintf(flog, "Scaled Insert of Component [%s] not allowed in PADS.\n", refname);
         display_error++;
      }

      if (strlen(refname) == 0)
      {
         refname.Format("NONAME%d", ++ nonamecnt);

         while (datastruct_from_refdes(doc, file->getBlock(), refname))
            refname.Format("NONAME%d", ++ nonamecnt);

         data->getInsert()->setRefname(STRDUP(refname));
      }

      refname = check_name('c', data->getInsert()->getRefname());

      CString typname = check_name('t', block->getName());
      Attrib *a = is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1);

      if (a)
         typname = check_name('t', get_attvalue_string(doc, a));

      double degree = RadToDeg(block_rot);

      if (block_mirror)
         degree = 360 - degree;

      while (degree < 0)
         degree += 360;

      while (degree >= 360)
         degree -= 360;

      int attributes = component_attributes(fp, data->getAttributesRef(), 0, scale);  // 0 = do not write,

      fprintf(fp, "%s %s %lg %lg %1.1lf U %c 0 -1 0 -1 %d\n", refname, typname,
            wPads_Units(output_units_accuracy, point2.x), wPads_Units(output_units_accuracy, point2.y),
            degree, (block_mirror)?'M':'N', attributes);

      component_attributes(fp, data->getAttributesRef(), 1, scale);  // 1 = write out
   }

   return 1;
}

/******************************************************************************
* do_partsection
*/
static int do_partsection(FILE *fp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLevel)
{

   fprintf(fp, "*PART*       ITEMS\n");
   fprintf(fp, "\n");
   fprintf(fp, "*REMARK* REFNM PTYPENM X Y ORI GLUE MIRROR ALT CLSTID CLSTATTR BROTHERID LABELS\n");
   fprintf(fp, "*REMARK* .REUSE. INSTANCE RPART\n");
   fprintf(fp, "*REMARK* VISIBLE XLOC YLOC ORI HEIGTH WIDTH LEVEL MIRRORED HJUST VJUST RIGHTREADING\n");
   fprintf(fp, "\n");

   write_partsection(fp, file, DataList, insert_x, insert_y, rotation, mirror, scale, insertLevel);

   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* do_misc
*/
static int do_misc(FILE *fp, CDataList *DataList, const char *padsunit)
{

   fprintf(fp, "*MISC*      MISCELLANEOUS PARAMETERS\n");
   fprintf(fp, "\n");

   // here layer typing
   // lunit = INCHES, MIL, MECTRIC
   fprintf(fp, "LAYER %s\n", padsunit);
   fprintf(fp, "{\n");
   write_layerattributes(fp);
   fprintf(fp, "}\n");

   // design rules
   fprintf(fp, "RULES_SECTION %s\n", padsunit);
   fprintf(fp, "{\n");
   write_designRules(fp);
   fprintf(fp, "}\n");

   // here part attributes
   fprintf(fp, "ATTRIBUTE VALUES\n");
   fprintf(fp, "{\n");
   write_partattributes(fp, DataList);  //insert level attributes
   write_decalattributes(fp);  //geom level attributes
   fprintf(fp, "}\n");

   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* write_linepoursection
   POURWRITE determinsed if this is in *LINE or *POUR mode.
   It is one routine, so that nothing falls through the cracks.
*/
static int write_linepoursection(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
                         double rotation, int mirror, double scale, double padsUnitsPerPageUnit, int insertLayer, int POURWRITE)
{
   Point2 point2;
   int layer;
   int padslayer;
   int hatchcnt = 0;
   int boardcnt = 0;
   double maxdesignspace = 56000 * onemil;
   CUnits units(doc->getPageUnits());

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         layer = doc->get_layer_mirror(layer, mirror);

         padslayer = Layer_PADS(layer);

         if (padslayer == -1)
            padslayer = 1;
         else if (padslayer == -2)
            padslayer = max_signalcnt;
         else if (padslayer == -4)
            padslayer = 0;

         if (padslayer < 0)
            continue;

         wPADS_Graph_LevelNum(padslayer);
      }

      switch(data->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt *pnt;
            POSITION polyPos, pntPos;
            char *LINETYPE = "LINES";

            // all BOARD
            // all copper with does not have a netname

            // copper with netname, check if it has a boundary or void -> write it to *POUR section.
            Attrib *attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
            int polyVoid = 0;
            int polyFilled = 0;
            int closed     = 0;
            int boundary   = 0;
            int hatchline  = 0;
            int thermallines  = 0;
            int pour = FALSE;

            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isFilled())
                  polyFilled++;
               if (poly->isClosed())
                  closed++;
               if (poly->isFloodBoundary())
                  boundary++;
               if (poly->isHatchLine())
                  hatchline++;
               if (poly->isVoid())
                  polyVoid++;
               if (poly->isThermalLine())
                  thermallines++;
            }

            // a boudary is always a pour.
            if (boundary) pour = TRUE;
            if (polyVoid) pour = TRUE;

            CString netname;

            if (attrib) // has netname
            {
               if (!closed && data->getGraphicClass() == GR_CLASS_ETCH)
               {
                  double widthMils = units.convertTo(pageUnitsMils,doc->getWidth(poly->getWidthIndex()));

                  if (widthMils <= PadsMaxTraceWidthInMils)
                  {
                     break;         // do in *ROUTE
                  }
               }

               // if closed and filled and has a netname !
               if (closed && polyFilled )
                  pour = TRUE;
               if (closed && polyFilled && hatchline)
                  pour = TRUE;
               if (closed && polyFilled && polyVoid)
                  pour = TRUE;
               netname = get_attvalue_string(doc, attrib);
            }
            else
               netname = "";

            if (!pour && POURWRITE)
               break;   // if not pour in POUR section - break
            if (pour && !POURWRITE)
               break;   // if pour in LINE section - break

            if (pour && POURWRITE)
            {
               CString masterpourname;

               masterpourname.Format("POUR%d", ++pourcnt);
               // here do POUR
               if (boundary)
               {
                  fprintf(fp, "*REMARK POUR boundary\n");

                  polyPos = data->getPolyList()->GetHeadPosition();
                  while (polyPos)
                  {
                     poly = data->getPolyList()->GetNext(polyPos);
                     if (!poly->isFloodBoundary())
                        continue;

                     int vertexcnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        vertexcnt++;
                     }

                     Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                     double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
                     //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
                     //if (w < 0.001)
                     //   w = 0.001;  // limits in PADS
                     //if (w > 250)
                     //   w = 250;

                     vertexcnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos )
                     {
                        pnt = poly->getPntList().GetNext(pntPos);

                        Point2 p;
                        p.x = pnt->x * scale * padsUnitsPerPageUnit;
                        if (mirror)
                           p.x = -p.x;
                        p.y = pnt->y * scale * padsUnitsPerPageUnit;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);

                        // need to check for bulge
                        int skip = FALSE;
                        if (vertexcnt)
                        {
                           if (fabs(p.x - points[vertexcnt-1].x) < SMALLDELTA &&
                               fabs(p.y - points[vertexcnt-1].y) < SMALLDELTA)
                              skip = TRUE;
                        }

                        if (!skip)
                        {
                           points[vertexcnt].x = p.x;
                           points[vertexcnt].y = p.y;
                           points[vertexcnt].bulge = p.bulge;
                           vertexcnt++;
                        }
                     }

                     fprintf(fp, "%s POUROUT %lg %lg 1 0 %s %s\n", masterpourname,
                        wPads_Units(output_units_accuracy, points[0].x),
                        wPads_Units(output_units_accuracy, points[0].y), masterpourname, netname);

                     wPADS_Graph_PourPoly(points, vertexcnt, polyFilled, widthInPadsUnits);
                     free(points);
                     break;   // can only have 1 boundary
                  }
               }
               else
               {
                  // take the first Filled | closed to make a POUR
                  fprintf(fp, "*REMARK POUR\n");
                  polyPos = data->getPolyList()->GetHeadPosition();
                  while (polyPos)
                  {
                     poly = data->getPolyList()->GetNext(polyPos);
                     if (!poly->isFilled())
                        continue;

                     int vertexcnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        vertexcnt++;
                     }

                     Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                     double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
                     //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
                     vertexcnt = 0;

                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);

                        Point2 p;
                        p.x = pnt->x * scale * padsUnitsPerPageUnit;
                        if (mirror)
                           p.x = -p.x;
                        p.y = pnt->y * scale * padsUnitsPerPageUnit;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);

                        // need to check for bulge
                        points[vertexcnt].x = p.x;
                        points[vertexcnt].y = p.y;
                        points[vertexcnt].bulge = p.bulge;
                        vertexcnt++;
                     }

                     fprintf(fp, "%s POUROUT %lg %lg 1 0 %s %s\n", masterpourname,
                        wPads_Units(output_units_accuracy, points[0].x),
                        wPads_Units(output_units_accuracy, points[0].y), masterpourname, netname);

                     wPADS_Graph_PourPoly(points, vertexcnt, polyFilled, widthInPadsUnits);
                     free(points);
                     break;   // can only have 1 boundary
                  }
               }

               // now a hatch
               if (polyFilled && !optionWritePourOutOnly)
               {
                  fprintf(fp, "*REMARK HATCH\n");
                  polyPos = data->getPolyList()->GetHeadPosition();
                  while (polyPos)
                  {
                     poly = data->getPolyList()->GetNext(polyPos);
                     if (poly->isFloodBoundary())
                        continue;
                     if (poly->isHatchLine())
                        continue;
                     if (poly->isVoid())
                        continue;
                     if (poly->isThermalLine())
                        continue;

                     int vertexcnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        vertexcnt++;
                     }

                     Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                     double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
                     //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
                     vertexcnt = 0;

                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);

                        Point2 p;
                        p.x = pnt->x * scale * padsUnitsPerPageUnit;
                        if (mirror)
                           p.x = -p.x;
                        p.y = pnt->y * scale * padsUnitsPerPageUnit;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);

                        // need to check for bulge
                        points[vertexcnt].x = p.x;
                        points[vertexcnt].y = p.y;
                        points[vertexcnt].bulge = p.bulge;
                        vertexcnt++;
                     }

                     fprintf(fp,"HAT%d HATOUT %lg %lg 1 0 %s\n", ++hatchcnt,
                        wPads_Units(output_units_accuracy, points[0].x),
                        wPads_Units(output_units_accuracy, points[0].y), masterpourname);

                     wPADS_Graph_PourPoly(points, vertexcnt, polyFilled, widthInPadsUnits);
                     free(points);
                  }

                  CString  hatchmaster;
                  hatchmaster.Format("HAT%d", hatchcnt);
                  // now voids if there inside HATCH
                  if (polyVoid && !optionWritePourOutOnly)
                  {
                     fprintf(fp, "*REMARK VOIDS\n");
                     polyPos = data->getPolyList()->GetHeadPosition();
                     while (polyPos)
                     {
                        poly = data->getPolyList()->GetNext(polyPos);
                        if (!poly->isVoid())
                           continue;

                        double cx, cy, rad;
                        int circle = FALSE;
                        double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
                        //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;

                        //if (w < 0.001)
                        //   w = 0.001;  // limits in PADS
                        //if (w > 250)
                        //   w = 250;

                        if (PolyIsCircle(poly, &cx, &cy, &rad))
                        {
                           Point2 p;
                           p.x = cx * scale * padsUnitsPerPageUnit;
                           if (mirror)
                              p.x = -p.x;
                           p.y = cy * scale * padsUnitsPerPageUnit;
                           p.bulge = 0;
                           TransPoint2(&p, 1, &m, insert_x, insert_y);

                           fprintf(fp, "HAT%d VOIDOUT %lg %lg 1 0 %s\n", ++hatchcnt,
                              wPads_Units(output_units_accuracy, p.x), wPads_Units(output_units_accuracy, p.y), hatchmaster);
                           fprintf(fp, "CIRCLE 2 0 %lg %d\n", wPads_Units(output_units_accuracy, widthInPadsUnits), padslayer);
                           fprintf(fp, "%lg %lg\n", wPads_Units(output_units_accuracy,-rad*scale * padsUnitsPerPageUnit), wPads_Units(output_units_accuracy, 0.0));
                           fprintf(fp, "%lg %lg\n", wPads_Units(output_units_accuracy, rad*scale * padsUnitsPerPageUnit), wPads_Units(output_units_accuracy, 0.0));
                           fprintf(fp, "\n");
                        }
                        else
                        {
                           int vertexcnt = 0;
                           pntPos = poly->getPntList().GetHeadPosition();
                           while (pntPos)
                           {
                              pnt = poly->getPntList().GetNext(pntPos);
                              vertexcnt++;
                           }

                           Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                           double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
                           //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
                           vertexcnt = 0;

                           pntPos = poly->getPntList().GetHeadPosition();
                           while (pntPos)
                           {
                              pnt = poly->getPntList().GetNext(pntPos);

                              Point2 p;
                              p.x = pnt->x * scale * padsUnitsPerPageUnit;
                              if (mirror)
                                 p.x = -p.x;
                              p.y = pnt->y * scale * padsUnitsPerPageUnit;
                              p.bulge = pnt->bulge;
                              TransPoint2(&p, 1, &m, insert_x, insert_y);

                              // need to check for bulge
                              points[vertexcnt].x = p.x;
                              points[vertexcnt].y = p.y;
                              points[vertexcnt].bulge = p.bulge;
                              vertexcnt++;
                           }

                           fprintf(fp, "HAT%d VOIDOUT %lg %lg 1 0 %s\n", ++hatchcnt,
                              wPads_Units(output_units_accuracy, points[0].x),
                              wPads_Units(output_units_accuracy, points[0].y),
                              hatchmaster);

                           wPADS_Graph_PourPoly(points, vertexcnt, polyFilled, widthInPadsUnits);
                           free(points);
                        }
                     } // is circle
                  }

                  // now thermals if there
                  if (thermallines)
                  {
#ifdef _DEBUG
                     fprintf(fp, "*REMARK THERMALS\n");
                     fprintf(flog, "THERMAL Lines not impl.\n");
                     display_error++;
#endif
                  }
                  // now hatchlines if there
                  if (hatchline)
                  {
#ifdef _DEBUG
                     fprintf(fp, "*REMARK HATCHLINES\n");
                     fprintf(flog, "HATCHLINES not impl.\n");
                     display_error++;
#endif
                  }
               }
               else
               {
                  // now voids if there inside HATCH. This is very funny and should not happen
                  if (polyVoid)
                  {
                     fprintf(fp, "*REMARK OUTSIDE FILL VOIDS\n");
                     fprintf(flog, "VOID without fill\n");
                     display_error++;
                  }

                  // now thermals if there
                  if (thermallines)
                  {
                     fprintf(fp, "*REMARK OUTSIDE FILL THERMALS\n");
                     fprintf(flog, "THERMALS without fill\n");
                     display_error++;
                  }

                  // now hatchlines if there
                  if (hatchline)
                  {
                     fprintf(fp, "*REMARK OUTSIDE FILL HATCHLINES\n");
                     fprintf(flog, "HATCHLINE without fill\n");
                     display_error++;
                  }
               }
            }
            else
            {
               // loop thru polys
               polyPos = data->getPolyList()->GetHeadPosition();
               while (polyPos)
               {
                  poly = data->getPolyList()->GetNext(polyPos);
                  BOOL polyFilled = poly->isFilled();
                  BOOL closed = poly->isClosed();

                  if (!optionTranslateHatchLines)
                  {
                     if (poly->isHatchLine())
                        continue;
                  }

                  double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
                  //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
                  //if (w < 0.001)
                  //   w = 0.001;  // limits in PADS
                  //if (w > 250)
                  //   w = 250;

                  char *restriction = "";
                  char *piecetype = "OPEN";
                  if (closed)
                     piecetype = "CLOSED";

                  LINETYPE = "LINES";
                  if (attrib || data->getGraphicClass() == GR_CLASS_ETCH )
                  {
                     if (padslayer >= 1 && padslayer <= max_signalcnt)
                     {
                        LINETYPE = "COPPER";

                        if (polyFilled)
                           piecetype = "COPCLS";
                        else
                           piecetype = "COPOPN";
                     }
                     else if(strlen(netname))
                     {
                        // netname, but not on electrical layer ????
                        fprintf(flog, "Copper area [%ld] has Netname [%s] but is not on electrical layer but on layer [%d]\n",
                           data->getEntityNumber(), netname, padslayer);
                        display_error++;
                        netname = "";
                     }
                  }

                  double cx, cy, rad;
                  int circle = FALSE;

                  if (PolyIsCircle(poly, &cx, &cy, &rad))
                  {
                     circle = TRUE;
                     closed = TRUE;
                     LINETYPE = "LINES";  // unfilled circle is always LINES CIRCLE
                     piecetype = "CIRCLE";
                     if (attrib || data->getGraphicClass() == GR_CLASS_ETCH )
                     {
                        if (polyFilled && padslayer >= 1 && padslayer <= max_signalcnt)
                        {
                           LINETYPE = "COPPER";
                           piecetype = "COPCIR";
                        }
                     }
                  }

                  if (closed && data->getGraphicClass() == GR_CLASS_ROUTKEEPOUT ||
                      data->getGraphicClass() == GR_CLASS_PLACEKEEPOUT ||
                      data->getGraphicClass() == GR_CLASS_VIAKEEPOUT ||
                      data->getGraphicClass() == graphicClassTestPointKeepOut ||
                      data->getGraphicClass() == GR_CLASS_ALLKEEPOUT)
                  {
                     LINETYPE = "KEEPOUT";

                     if (circle)
                        piecetype = "KPTCIR";
                     else
                        piecetype = "KPTCLS";

                     if (data->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)
                        restriction = "P";
                     else if (data->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)
                        restriction = "R";
                     else if (data->getGraphicClass() == GR_CLASS_VIAKEEPOUT)
                        restriction = "V";
                     else if (data->getGraphicClass() == graphicClassTestPointKeepOut)
                        restriction = "T";
                     else if (data->getGraphicClass() == GR_CLASS_ALLKEEPOUT)
                        restriction = "PRVT";
                  }
                  else if (closed && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
                  {
                     if (boardcnt == 0)   // PADS can only have 1 boardoutline otherwise it crashes.
                        LINETYPE = "BOARD";
                     wPADS_Graph_Level("0");

                     if (circle)
                        piecetype = "CIRCLE";
                     else
                        piecetype = "CLOSED";

                     boardcnt++;
                  }

                  if (circle)
                  {
                     Point2 p;
                     p.x = cx * scale * padsUnitsPerPageUnit;
                     if (mirror)
                        p.x = -p.x;
                     p.y = cy * scale * padsUnitsPerPageUnit;
                     p.bulge = 0;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);

                     fprintf(fp, "*LINES*      LINES ITEMS\n");
                     wPADS_Graph_Circle( p.x, p.y, rad*scale * padsUnitsPerPageUnit, widthInPadsUnits, LINETYPE, piecetype, restriction);
                     fprintf(fp, "\n");
                  }
                  else
                  {

                     int vertexcnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        vertexcnt++;
                     }
                     Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                     vertexcnt = 0;

                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);

                        Point2 p;
                        p.x = pnt->x * scale * padsUnitsPerPageUnit;
                        if (mirror)
                           p.x = -p.x;
                        p.y = pnt->y * scale * padsUnitsPerPageUnit;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);

                        // need to check for bulge
                        int skip = FALSE;
                        if (vertexcnt)
                        {
                           if (fabs(p.x - points[vertexcnt-1].x) < SMALLDELTA && fabs(p.y - points[vertexcnt-1].y) < SMALLDELTA)
                              skip = TRUE;
                        }

                        if (!skip)
                        {
                           points[vertexcnt].x = p.x;
                           points[vertexcnt].y = p.y;
                           points[vertexcnt].bulge = p.bulge;
                           vertexcnt++;
                        }
                     }

                     if (vertexcnt > 1)
                     {
                        fprintf(fp, "*LINES*      LINES ITEMS\n");
                        wPADS_Graph_Polyline(maxdesignspace, points, vertexcnt, polyFilled, widthInPadsUnits, LINETYPE,
                              piecetype, restriction);
                     }
                     free(points);
                  }
               }
            }
         }
         break;

         case T_INSERT:
         {
            int block_mirror = data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            point2.x = data->getInsert()->getOriginX() * scale * padsUnitsPerPageUnit;
            if (mirror)
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * scale * padsUnitsPerPageUnit;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (block->getBlockType() == BLOCKTYPE_DRAWING)
               break;   // done as a decal in comp section
            if (block->getBlockType() == BLOCKTYPE_GENERICCOMPONENT)
               break;   // done as a decal in comp section
            if (block->getBlockType() == BLOCKTYPE_MECHCOMPONENT)
               break;   // done as a decal in comp section
            if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
               break;   // done as a decal in comp section
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               break;

            int block_layer = insertLayer;;
            if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
               block_layer = data->getLayerIndex();

            write_linepoursection(fp, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror, 
               scale * data->getInsert()->getScale(), padsUnitsPerPageUnit,block_layer, POURWRITE);
         }
         break;

      } // end switch
   } // end for
   return 1;
}

/******************************************************************************
* do_textsection
*/
static int do_textsection(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
      int mirror, double scale, int insertLevel)
{
   fprintf(fp, "*TEXT*       FREE TEXT\n");
   fprintf(fp, "\n");
   fprintf(fp, "*REMARK* XLOC YLOC ORI LEVEL HEIGHT WIDTH MIRRORED HJUST VJUST .REUSE. INSTANCENM\n");
   fprintf(fp, "\n");

   write_textsection(fp, DataList, insert_x, insert_y, rotation, mirror, scale, insertLevel);

   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* do_linesection
*/
static int do_linesection(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
      int mirror, double scale, double padsUnitsPerPageUnit, int insertLevel)
{
   fprintf(fp, "*LINES*      LINES ITEMS\n");
   fprintf(fp, "\n");
   fprintf(fp, "*REMARK* NAME TYPE XLOC YLOC PIECES TEXT LABELS\n");
   fprintf(fp, "*REMARK* .REUSE. INSTANCE RSIGNAL\n");
   fprintf(fp, "*REMARK* PIECETYPE CORNERS WIDTHHGHT LEVEL RESTRICTIONS\n");
   fprintf(fp, "*REMARK* XLOC YLOC BEGINANGLE DELTAANGLE\n");
   fprintf(fp, "*REMARK* XLOC YLOC ORI LEVEL HEIGHT WIDTH MIRRORED HJUST VJUST\n");
   fprintf(fp, "\n");

   // line, not POUR
   write_linepoursection(fp, DataList, insert_x, insert_y, rotation, mirror, scale,padsUnitsPerPageUnit,insertLevel, 0);

   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* do_poursection
*/
static int do_poursection(FILE *fp, CDataList *DataList, double insert_x, double insert_y, double rotation,
      int mirror, double scale, double padsUnitsPerPageUnit, int insertLevel)
{
   fprintf(fp, "*POUR*       POUR ITEMS\n");
   fprintf(fp, "\n");
   fprintf(fp, "*REMARK* NAME TYPE XLOC YLOC PIECES FLAGS [OWNERNAME SIGNAME HATCHGRID HATCHRAD]\n");
   fprintf(fp, "*REMARK* PIECETYPE  CORNERS ARCS WIDTH LEVEL\n");
   fprintf(fp, "*REMARK* XLOC YLOC BEGINANGLE DELTAANGLE\n");
   fprintf(fp, "\n");

   // pour, not line
   pourcnt = 0;
   write_linepoursection(fp, DataList, insert_x, insert_y, rotation, mirror, scale,padsUnitsPerPageUnit,insertLevel, 1);

   fprintf(fp, "\n");
   return 1;
}

/******************************************************************************
* used_as_via
*/
static int used_as_via(const char *v)
{
   for (int i=0;i<viacnt;i++)
   {
      if (viaarray[i]->vianame.Compare(v) == 0)
         return 1;
   }

   return 0;
}

/******************************************************************************
* clean_polys
*/
static void clean_polys(CDataList *DataList, double small_element)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      switch (data->getDataType())
      {
      case T_POLY:
         {
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
               CleanPoly(data->getPolyList()->GetNext(polyPos), small_element);
         }
         break;

      case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (!(block->getFlags() & BL_TOOL || block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE))
               clean_polys(&block->getDataList(), small_element);
         }
         break;
      }
   }
}


/******************************************************************************
* get_vianames
*/
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
               if (!used_as_via(block->getName()))
               {
                  PADS_ViaStruct *v = new PADS_ViaStruct;
                  v->vianame = block->getName();
                  viaarray.SetAtGrow(viacnt, v);
                  viacnt++;
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

/******************************************************************************
* PCB_WriteROUTESData
*/
void PCB_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, double padsUnitsPerPageUnit, int embeddedLevel, int insertLayer, const char *netname)
{
   CUnits units(doc->getPageUnits());
   Mat2x2 m;
   Point2 point2;

   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      int layerNum;
      int padslayer;

      if (data->getDataType() != T_INSERT)
      {
         if (data->getGraphicClass() != GR_CLASS_ETCH)
            continue;

         if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layerNum = insertLayer;
         else
            layerNum = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layerNum, mirror))
            continue;

         padslayer = Layer_PADS(layerNum);
         if (padslayer == -1)
            padslayer = 1;
         else if (padslayer == -2)
            padslayer = max_signalcnt;
         else if (padslayer == -4)
            padslayer = 0;

         if (padslayer < 0)
            continue;

         Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
         if (attrib)
         {
            if (strcmp(get_attvalue_string(doc, attrib), netname))
               continue;
         }
         else
            continue;
      }
      else // T_INSERT
      {
         // here check if ATTR_NETNAME == netname

         // allow only vias and test points
         Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
         if (attrib)
         {
            if (strcmp(get_attvalue_string(doc, attrib), netname))
               continue;
         }
         else
            continue;
      }

      switch(data->getDataType())
      {
      case T_POLY:
         {
            char lay = padslayer;

            // loop thru polys
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);

               int widthIndex          = poly->getWidthIndex();
               double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[widthIndex]->getSizeA() * scale,padsUnitsPerPageUnit);

               // do not do poly thermallines
               if (poly->isThermalLine() || poly->isVoid() || poly->isFilled() || poly->isClosed())
                  continue;

               BOOL First = TRUE;
               Point2 firstp, p1, p2;

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale * padsUnitsPerPageUnit;
                  if (mirror)
                     p2.x = -p2.x;
                  p2.y = pnt->y * scale * padsUnitsPerPageUnit;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);

                  // here deal with bulge
                  if (First)
                  {
                     firstp = p2;
                     p1 = p2;
                     First = FALSE;
                  }
                  else
                  {
                     int err = 0;
                     double da = atan(p1.bulge) * 4;

                     if (fabs(da) > SMALLANGLE)
                     {
                        double cx,cy,r,sa;
                        ArcPoint2Angle(p1.x,p1.y,p2.x,p2.y,da,&cx,&cy,&r,&sa);
                        // make positive start angle.
                        if (sa < 0) sa += PI2;
                        // here arc to poly
                        int ii, ppolycnt = 255;
                        // start center
                        Point2 *ppoly = (Point2*)calloc(255, sizeof(Point2));

                        ArcPoly2(p1.x,p1.y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(15));
                        for (ii=1;ii<ppolycnt;ii++)
                           err += LoadSegment(ppoly[ii-1].x, ppoly[ii-1].y,
                                 ppoly[ii].x, ppoly[ii].y, lay, widthIndex);

                        free(ppoly);
                     }
                     else
                     {
                        err = LoadSegment(p1.x, p1.y, p2.x, p2.y, lay, widthIndex);
                     }

                     if (err & 1)
                     {
#ifdef DUMP
                        fprintf(flog, "Segment length Error -> SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                              p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, widthInPadsUnits);
#endif
                     }
                     else if (err & 2)
                     {
                        fprintf(flog, "Layer Error -> SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                              p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, widthInPadsUnits);
                     }
                     else
                     {
#ifdef DUMP
                        fprintf(flog, "SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                              p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, widthInPadsUnits);
#endif
                     }
                     p1 = p2;
                  }
               }
            }  // while
         }
         break;  // POLYSTRUCT

      case T_TEXT: // no text in route section
         break;

      case T_INSERT:
         {
            if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;
            if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
               break;
            if (data->getInsert()->getInsertType() == INSERTTYPE_CENTROID)
               break;

            point2.x = data->getInsert()->getOriginX() * scale * padsUnitsPerPageUnit;
            if (mirror)
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * scale * padsUnitsPerPageUnit;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (data->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_VIALAYER, 1);
               if (attrib)
                  fprintf(flog, "Via range not implemented\n");
#ifdef DUMP
               fprintf(flog, "%d VIA X=%lf Y=%lf P=%s\n", viacnt, point2.x, point2.y, block->name);
#endif
               UINT64 layermap;

               if (strlen(block->getName()))
                  layermap = get_padstacklayermap(block->getName(),0);
               else
                  layermap = 0xffffffff;

               LoadVia(traceviacnt, point2.x, point2.y, layermap);

               PADSTraceViaStruct *v = new PADSTraceViaStruct;
               v->vianame = check_name('v', block->getName());
               traceviaarray.SetAtGrow(traceviacnt, v);
               traceviacnt++;

               break;
            }

            if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;

               if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
                  block_layer = insertLayer;
               else
                  block_layer = data->getLayerIndex();

               PCB_WriteROUTESData(wfp, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror, 
                  scale * data->getInsert()->getScale(),padsUnitsPerPageUnit,
                  embeddedLevel+1, block_layer, netname);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   }
}

//--------------------------------------------------------------
static int do_signalpinloc(NetStruct *net, double scale)
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

         padstackname = "";
         pinx = compPin->getOriginX();
         piny = compPin->getOriginY();
         pinrot = RadToDeg(compPin->getRotationRadians());
         padmirror = compPin->getMirror();

         if (compPin->getPadstackBlockNumber() > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
            if (block)
               padstackname = block->getName();
         }


         UINT64 layermap;

         if (strlen(padstackname))
            layermap = get_padstacklayermap(padstackname,padmirror);
         else
            layermap = ALL_INT64;


#ifdef DUMP
         fprintf(flog, "index %d PIN X=%lf Y=%lf R=%s.%s P=%s layermap [%x] mirror %d\n",
               tracepincnt, pinx*scale, piny*scale,
               compPin->getRefDes(), compPin->getPinName(),padstackname, layermap, padmirror);
#endif

         LoadEndpoint(tracepincnt, pinx*scale, piny*scale, layermap);
         PADSTracePinStruct *p = new PADSTracePinStruct;
         p->compname = check_name('c',compPin->getRefDes());
         p->pinname = check_name('e',compPin->getPinName());
         p->x = pinx*scale;
         p->y = piny*scale;
         tracepinarray.SetAtGrow(tracepincnt, p);
         tracepincnt++;

      }
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static void Write_BADTraces(FILE *stream, CResultList *resultList, const char *netname, double scale,
                            double padsUnitsPerPageUnit,double defaultlinewidth)
{
   Net_Path *node;
   /* struct Net_Path
      {
         char type; // VIA_NODE, SEGMENT_NODE, ENDPOINT_NODE
         int index_id;
         BOOL reverse; // segment goes from p2 to p1
      }; */
   CPathList *path;

   POSITION resPos, pathPos;
   // this first thing is to check if the net is a good net, this means all 2 pin connections

   // here make a good loop
   // loop result paths
   resPos = resultList->GetHeadPosition();

   int startpin = 0;

   // the "bad segements are between these 2 pins. every segment starts and ends as a "open" line.
   fprintf(stream,"%s.%s  %s.%s\n",
            tracepinarray[startpin]->compname, tracepinarray[startpin]->pinname,
            tracepinarray[startpin+1]->compname, tracepinarray[startpin+1]->pinname);

   fprintf(stream,"%lg %lg 0 %lg 0%s\n",
            wPads_Units(output_units_accuracy, tracepinarray[startpin]->x),
            wPads_Units(output_units_accuracy, tracepinarray[startpin]->y),
            wPads_Units(output_units_accuracy, defaultlinewidth),
            getPlaneTieClause());

   int   segcnt = 0;
   while (resPos != NULL)
   {
      path = resultList->GetNext(resPos)->path;
      if (path->GetCount() == 1 && path->GetHead()->type == ENDPOINT_NODE)
      {
         //fprintf(stream, "*REMARK* Broken Net!  Moving this pin over.\n");
         continue;
      }

      int nextsegcnt = path->GetCount();
      if ((segcnt + nextsegcnt*2) > 1900) // LIMIT on PADS POWER 3
      {
         if(startpin < tracepincnt-2)
         {
            // last pin koos. All traces and vias are in between.
            fprintf(stream,"%lg %lg 31 %lg 0%s\n",
               wPads_Units(output_units_accuracy, tracepinarray[startpin+1]->x),
               wPads_Units(output_units_accuracy, tracepinarray[startpin+1]->y),
               wPads_Units(output_units_accuracy, defaultlinewidth),
               getPlaneTieClause());

            startpin++;
            segcnt = 0;
#ifdef _DEBUG
   PADSTracePinStruct *tt1 = tracepinarray[startpin];
   PADSTracePinStruct *tt2 = tracepinarray[startpin+1];
#endif
            // the "bad segments are between these 2 pins. every segment starts and ends as a "open" line.
            fprintf(stream,"%s.%s  %s.%s\n",
               tracepinarray[startpin]->compname, tracepinarray[startpin]->pinname,
               tracepinarray[startpin+1]->compname, tracepinarray[startpin+1]->pinname);

            fprintf(stream,"%lg %lg 0 %lg 0%s\n",
               wPads_Units(output_units_accuracy, tracepinarray[startpin]->x),
               wPads_Units(output_units_accuracy, tracepinarray[startpin]->y),
               wPads_Units(output_units_accuracy, defaultlinewidth),
               getPlaneTieClause());
         }
         else
         {
            // right now, it continues to write elements and PADS will generate a error message.
            fprintf(flog," Net [%s] -> Not enough Pins. More than 2000 segments between pins ! Segments will be lost!\n", netname);
            display_error++;
            // here need to close the net and write copper lines !!!
         }
      } // limit 2000 elements

      // loop nodes of this path
      pathPos = path->GetHeadPosition();

      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
#ifdef DUMP
               fprintf(stream, "*REM Endpoint ID=%d\n", node->index_id);
#endif
            break;
            case SEGMENT_NODE:
            {
#ifdef DUMP
               fprintf(stream, "*REM Segment index=%d\n", node->index_id);
#endif
               Net_Segment *seg = segmentArray->GetAt(node->index_id);

               double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[seg->widthIndex]->getSizeA() * scale,padsUnitsPerPageUnit);

               //double lineWidth = doc->widthTable[seg->widthIndex]->getSizeA()*scale;
               //if (lineWidth > 0.25) // this is in INCH
               //   lineWidth = 0.25;

               segcnt++;
               fprintf(stream,"%lg %lg %d %lg 0\n",
                     wPads_Units(output_units_accuracy, seg->x1),
                     wPads_Units(output_units_accuracy, seg->y1), seg->layer,
                     wPads_Units(output_units_accuracy, widthInPadsUnits));
               segcnt++;
               fprintf(stream,"%lg %lg 0 %lg 0\n",
                     wPads_Units(output_units_accuracy, seg->x2),
                     wPads_Units(output_units_accuracy, seg->y2),
                     wPads_Units(output_units_accuracy, widthInPadsUnits));
            }
            break;
            case VIA_NODE:
            {
               // a via spans from a lastlayer to a tolayer
#ifdef DUMP
               fprintf(stream, "# Via index=%d\n", node->index_id);
#endif
               Net_Via *via = viaArray->GetAt(node->index_id);
#ifdef DUMP
               fprintf(stream, "# %d Via (x=%+lf, y=%+lf) lm=%ul\n",
                  node->index_id, via->x, via->y, via->layermap);
#endif
               segcnt++;
               fprintf(stream,"%lg %lg %d %lg 0 %s%s\n",
                     wPads_Units(output_units_accuracy, via->x),
                     wPads_Units(output_units_accuracy, via->y), 0,
                     wPads_Units(output_units_accuracy, defaultlinewidth),
                     traceviaarray[via->id]->vianame,
                     getPlaneTieClause());
            }
            break;
         }
      }
   }

   // last pin koos. All traces and vias are in between.
   fprintf(stream,"%lg %lg 31 %lg 0%s\n",
         wPads_Units(output_units_accuracy, tracepinarray[startpin+1]->x),
         wPads_Units(output_units_accuracy, tracepinarray[startpin+1]->y),
         wPads_Units(output_units_accuracy, defaultlinewidth),
         getPlaneTieClause());

   // all segments are between 0 and 1 endpoint, all other endpoints are connected
   for (int i=startpin+2;i<tracepincnt;i++)
   {
      fprintf(stream,"%s.%s  %s.%s\n",
            tracepinarray[i-1]->compname, tracepinarray[i-1]->pinname,
            tracepinarray[i]->compname, tracepinarray[i]->pinname);

      fprintf(stream,"%lg %lg 0 %lg 0%s\n",
         wPads_Units(output_units_accuracy, tracepinarray[i-1]->x),
         wPads_Units(output_units_accuracy, tracepinarray[i-1]->y),
         wPads_Units(output_units_accuracy, defaultlinewidth),
         getPlaneTieClause());

      fprintf(stream,"%lg %lg 31 %lg 0%s\n",
         wPads_Units(output_units_accuracy, tracepinarray[i]->x),
         wPads_Units(output_units_accuracy, tracepinarray[i]->y),
         wPads_Units(output_units_accuracy, defaultlinewidth),
         getPlaneTieClause());
   }

   return;
} // Write_BADTraces

/*****************************************************************************/
/*
*/
static void Write_GOODTraces(FILE *stream, CResultList *resultList, const char *netname, double scale,
                             double padsUnitsPerPageUnit,double defaultlinewidth)
{
   Net_Path *node;
   CPathList *path;

   POSITION resPos, pathPos;
   // this first thing is to check if the net is a good net, this means all 2 pin connections

   // here make a good loop
   // loop result paths
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      path = resultList->GetNext(resPos)->path;

      if (path->GetCount() == 1 && path->GetHead()->type == ENDPOINT_NODE)
      {
         //node = path->GetNext(pathPos);
         //int endpoint1 = node->index_id;
         //fprintf(stream, "*REMARK* Broken Net!  Moving this pin [%s.%s] over.\n",
         //    tracepinarray[endpoint1]->compname, tracepinarray[endpoint1]->pinname);
         continue;
      }

      // loop nodes of this path
      pathPos = path->GetHeadPosition();

      // is Endpoint start and Endpoint end -> Wir
      // if Endpoint start or Endpoint end -> Antenna
      // else Incomplete.
      int ep = 0; // endpoint count
      int sp = 0; // seg count  (vertex and the first count start and end)
      int vp = 0; // via count

      int   endpoint1, endpoint2;
      endpoint1 = -1;
      endpoint2 = -1;

      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
               if(ep == 0) // first point
                  endpoint1 = node->index_id;
               else
               if (ep == 1)
                  endpoint2 = node->index_id;
               else
               {
                  fprintf(flog,"More than 2 endpoints in Net [%s]???\n", netname);
                  display_error++;
               }
               ep++;
            break;
            case SEGMENT_NODE:
               sp++;
            break;
            case VIA_NODE:
               vp++;
            break;
         }
      }

      fprintf(stream,"%s.%s  %s.%s\n",
            tracepinarray[endpoint1]->compname, tracepinarray[endpoint1]->pinname,
            tracepinarray[endpoint2]->compname, tracepinarray[endpoint2]->pinname);

      if (!sp && !vp)   // write a simple open connection
      {
         fprintf(stream,"%lg %lg 0 %lg\n",
            wPads_Units(output_units_accuracy, tracepinarray[endpoint1]->x),
            wPads_Units(output_units_accuracy, tracepinarray[endpoint1]->y),
            wPads_Units(output_units_accuracy, defaultlinewidth));
         fprintf(stream,"%lg %lg 31 %lg 0\n",
            wPads_Units(output_units_accuracy, tracepinarray[endpoint2]->x),
            wPads_Units(output_units_accuracy, tracepinarray[endpoint2]->y),
            wPads_Units(output_units_accuracy, defaultlinewidth));
         continue;
      }

      // loop nodes of this path
      pathPos = path->GetHeadPosition();
      int      first = TRUE;
      int      lastlayer = 0;
      double   lastlinewidth = 0;
      CString  cur_vianame;
      cur_vianame = "";

      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
#ifdef DUMP
               fprintf(stream, "# Endpoint ID=%d\n", node->index_id);
#endif
            break;
            case SEGMENT_NODE:
            {
#ifdef DUMP
               fprintf(stream, "# Segment index=%d\n", node->index_id);
#endif
               Net_Segment *seg = segmentArray->GetAt(node->index_id);

               double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[seg->widthIndex]->getSizeA() * scale,padsUnitsPerPageUnit);

               //double lineWidth = doc->widthTable[seg->widthIndex]->getSizeA()*scale;
               //if (lineWidth > 0.25) // this is in INCH
               //   lineWidth = 0.25;

               lastlinewidth = widthInPadsUnits;
               lastlayer = seg->layer;

               if (first)
               {
                  if (node->reverse)
                  {
                     fprintf(stream,"%lg %lg",
                        wPads_Units(output_units_accuracy, seg->x2),
                        wPads_Units(output_units_accuracy, seg->y2));
                  }
                  else
                  {
                    fprintf(stream,"%lg %lg",
                        wPads_Units(output_units_accuracy, seg->x1),
                        wPads_Units(output_units_accuracy, seg->y1));
                  }
               }

               fprintf(stream," %d %lg 0 %s%s\n", lastlayer,
                  wPads_Units(output_units_accuracy, lastlinewidth), cur_vianame,
                  getPlaneTieClause(optionAddPlaneTie && (first || cur_vianame.GetLength() > 0)));

               cur_vianame = "";

               if (node->reverse)
               {
                  fprintf(stream,"%lg %lg",
                     wPads_Units(output_units_accuracy, seg->x1),
                     wPads_Units(output_units_accuracy, seg->y1));
               }
               else
               {
                  fprintf(stream,"%lg %lg",
                     wPads_Units(output_units_accuracy, seg->x2),
                     wPads_Units(output_units_accuracy, seg->y2));
               }
#ifdef DUMP
               fprintf(stream, "# %d Segment (x1=%+lf, y1=%+lf) (x2=%+lf, y2=%+lf) l=%d w=%d r=%c\n",
                  node->index_id, seg->x1, seg->y1, seg->x2, seg->y2, seg->layer, seg->widthIndex,
                  (node->reverse?'T':'f'));
#endif
               first = FALSE;
            }
            break;
            case VIA_NODE:
            {
               // a via spans from a lastlayer to a tolayer
#ifdef DUMP
               fprintf(stream, "# Via index=%d\n", node->index_id);
#endif
               Net_Via *via = viaArray->GetAt(node->index_id);
#ifdef DUMP
               fprintf(stream, "# %d Via (x=%+lf, y=%+lf) lm=%ul\n",
                  node->index_id, via->x, via->y, via->layermap);
#endif

               //fprintf(stream,"%s\n",traceviaarray[via->id]->vianame);// write via instead of width
               cur_vianame = traceviaarray[via->id]->vianame;
               //first = TRUE;
            }
            break;
         }
      }

      // if the last was a via, do not do it. The secquence is
      // startx starty width layer to
      // endx endy and than the next from there.
      fprintf(stream, " 31 %lg 0 %s%s\n",
         wPads_Units(output_units_accuracy, lastlinewidth), cur_vianame,
         getPlaneTieClause()); // last layer, last width
   }
   return;
} // Write_GOODTraces

/*****************************************************************************/
/*
*/
static int in_connectlink(int id, int lastid)
{
   int   i;

   for (i=0;i<connectlinkcnt;i++)
   {
      if (lastid != -1)
      {
         if (lastid == connectlinkarray[i]->endpointindex)
         {
            // add id
            PADS_ConnectLinkMap *p = new PADS_ConnectLinkMap;
            connectlinkarray.SetAtGrow(connectlinkcnt, p);
            connectlinkcnt++;
            p->endpointindex = id;
            return TRUE;
         }
      }
      if (id == connectlinkarray[i]->endpointindex)
      {
         // add last
         if (lastid != -1)
         {
            PADS_ConnectLinkMap *p = new PADS_ConnectLinkMap;
            connectlinkarray.SetAtGrow(connectlinkcnt, p);
            connectlinkcnt++;
            p->endpointindex = lastid;
         }
         return TRUE;
      }
   } // lastid != -1

   // add id
   PADS_ConnectLinkMap *p = new PADS_ConnectLinkMap;
   connectlinkarray.SetAtGrow(connectlinkcnt, p);
   connectlinkcnt++;
   p->endpointindex = id;
   // add lastid
   if (lastid != -1)
   {
      PADS_ConnectLinkMap *p = new PADS_ConnectLinkMap;
      connectlinkarray.SetAtGrow(connectlinkcnt, p);
      connectlinkcnt++;
      p->endpointindex = lastid;
   }

   if (connectlinkcnt < 3) return TRUE;   // always good on 2 pins

   return FALSE;
}
/*****************************************************************************/
/*
*ROUTE*
*SIGNAL* GND 1.0         0  0  0 -2
J101.10  J101.11 P  0  0
14592.0    16850.0    0 12.0       0
11775.0    16000.0    0 12.0       0 PS0
12425.0    16000.0    0 12.0       0 PS0
11825.0    18525.0    0 12.0       0 PS0
12875.0    18525.0    0 12.0       0 PS0
10900.0    15925.0    1 50.0       0
10900.0    15930.0    0 50.0       0
10900.
*/
void Write_Traces(FILE *stream, CResultList *resultList, const char *netname,
                  double scale,double padsUnitsPerPageUnit, double default_linewidth)
{
   if (resultList == NULL) return;

   Net_Path *node;
   /* struct Net_Path
      {
         char type; // VIA_NODE, SEGMENT_NODE, ENDPOINT_NODE
         int index_id;
         BOOL reverse; // segment goes from p2 to p1
      }; */
   CPathList *path;

   int   good_net_connection = TRUE;

   POSITION resPos, pathPos;
   // this first thing is to check if the net is a good net, this means all 2 pin connections

   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      path = resultList->GetNext(resPos)->path;

      // loop nodes of this path
      pathPos = path->GetHeadPosition();

      int ep = 0; // endpoint count
      int lastendpoint = -1;

      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
               ep++;
               //if (lastendpoint == node->index_id)  // same endpoint in connection.
               // good_net_connection = FALSE;

               if (!in_connectlink(node->index_id,lastendpoint ))
                  good_net_connection = FALSE;

               // if this is a "broken connection"
               // U1.1 U1.2
               // U2.1 U2.3 all in one net vs.
               // U1.1 U1.2
               // U1.2 U2.1
               // etc...
               lastendpoint = node->index_id;
            break;
            case SEGMENT_NODE:
            break;
            case VIA_NODE:
            break;
         }
      }
      if (ep != 2)
         good_net_connection = FALSE;
   }

   // a good connection is if every trace starts and ends in a net. We can write this out very
   // optimized.
   if (good_net_connection)
   {
      Write_GOODTraces(stream, resultList, netname, scale,padsUnitsPerPageUnit, default_linewidth);
   }
   else
   {
      // here are connections not ending in pins, we need to make it like in laydbase.
      //fprintf(flog,"NET [%s] is not a good connection\n", netname);
      //fprintf(stream,"*REMARK* NET [%s] is not a good connection\n", netname);
      Write_BADTraces(stream, resultList, netname, scale,padsUnitsPerPageUnit, default_linewidth);
   } // not a good connection

   return;
}

/******************************************************************************
* do_routesection
*/
static int do_routesection(FILE *wfp, FileStruct *file, double padsUnitsPerPageUnit, double default_linewidth)
{
   fprintf(wfp, "*ROUTE*\n");
   fprintf(wfp, "\n");
   fprintf(wfp, "*REMARK* *SIGNAL* SIGNAME SIGFLAG COLOR\n");
   fprintf(wfp, "*REMARK* REFNM.PIN .REUSE. INSTANCE RSIG REFNM.PIN .REUSE. INSTANCE RSIG\n");
   fprintf(wfp, "*REMARK* XLOC YLOC LAYER SEGMENTWIDTH FLAGS [ARCDIR/VIANAME] [TEARDROP [P WID LEN [FLAGS]] [N WID LEN [FLAGS]]] [JMPNM JMPFLAG] REUSE INST RSIG\n");
   fprintf(wfp, "\n");

   // need to make a layername, layerbitmap function
   // loop throu netlist and store pins, vias, segments.
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

#ifdef DUMP
      fprintf(flog, "NET=%s\n", net->getNetName());
#endif
      traceviacnt = 0;
      tracepincnt = 0;
      connectlinkcnt = 0;

      fprintf(wfp, "*SIGNAL* %s 0 -2\n", check_name('n', net->getNetName()));

      // reset status
      CString  f;
      f.Format("Processing %s",net->getNetName());
      progress->SetStatus(f);

      do_signalpinloc(net, file->getScale()*padsUnitsPerPageUnit);

      // order by signal name
      SetMarginTrace(SMALLDELTA);
      SetNetTrace_ConnectUnusedToEndpoints(FALSE);    // FALSE means that unused traces are not connected

      PCB_WriteROUTESData(wfp, &file->getBlock()->getDataList(), file->getInsertX() * padsUnitsPerPageUnit, file->getInsertY() * padsUnitsPerPageUnit,
            file->getRotation(), file->isMirrored(), file->getScale(),padsUnitsPerPageUnit, 0, -1, net->getNetName());

      CResultList *resultList = FindShortestNetList();

#ifdef DUMP
      DumpResult(flog, resultList);
      display_error++;
#endif

      if (tracepincnt < 2)
      {
         fprintf(flog, "Net [%s] has only 1 Pin. This is not allowed (and possible) in PADS POWER.\n", net->getNetName());
         display_error++;
      }
      else
      {
         Write_Traces(wfp, resultList, net->getNetName(), file->getScale(), padsUnitsPerPageUnit, default_linewidth);
      }

      ClearTrace();
		int i=0;
      for (i=0; i<connectlinkcnt; i++)
         delete connectlinkarray[i];
      connectlinkcnt = 0;

      for (i=0;i<traceviacnt;i++)
         delete traceviaarray[i];
      for (i=0;i<tracepincnt;i++)
         delete tracepinarray[i];

      fprintf(wfp, "\n");
   }

   return 1;
}

//--------------------------------------------------------------
void PADS_WriteData(FILE *fp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale, double padsUnitsPerPageUnit,
                   int insertLayer)
{
   Mat2x2   m;
   Point2   point2;
   int      boardcnt = 0;
   double   maxdesignspace = 56000 * onemil;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
      {
         int   layer;
         if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;
         layer = doc->get_layer_mirror(layer, mirror);

         int lnr = Layer_PADS(layer);
         if (lnr == -1)
            lnr = 1;
         else if (lnr == -2)
            lnr = max_signalcnt;
         else if (lnr == -4)
            lnr = 0;

         if (lnr < 0)
            continue;

         CString lname;
         lname.Format("%d", lnr);
         wPADS_Graph_Level(lname);
      }

      switch(data->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            char  *LINETYPE = "LINES";
            char  *restriction = "";
            char  *piecetype = "OPEN";

            // loop thru polys
            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               if (closed && data->getGraphicClass() == GR_CLASS_ROUTKEEPOUT ||
                     data->getGraphicClass() == GR_CLASS_PLACEKEEPOUT ||
                     data->getGraphicClass() == GR_CLASS_VIAKEEPOUT ||
                     data->getGraphicClass() == GR_CLASS_ALLKEEPOUT ||
                     data->getGraphicClass() == graphicClassTestPointKeepOut)
               {
                  LINETYPE = "KEEPOUT";
                  piecetype = "KPTCLS";

                  if (data->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)
                     restriction = "P";
                  else if (data->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)
                     restriction = "R";
                  else if (data->getGraphicClass() == GR_CLASS_VIAKEEPOUT)
                     restriction = "V";
                  else if (data->getGraphicClass() == graphicClassTestPointKeepOut)
                     restriction = "T";
                  else if (data->getGraphicClass() == GR_CLASS_ALLKEEPOUT)
                     restriction = "PRVT";
               }
               else if (data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
               {
                  if (!closed)
                  {
                     //fprintf(flog, "Primary Board Outline found, which is not closed !\n");
                     //display_error++;
                  }
                  else
                  {
                     if (boardcnt == 0)   // PADS can only have 1 boardoutline otherwise it crashes.
                        LINETYPE = "BOARD";
                     wPADS_Graph_Level("0");
                     boardcnt++;
                  }
               }

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
               double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
               //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
               //if (w < 0.001) w = 0.001;  // limits in PADS
               //if (w > 250)   w = 250;

               vertexcnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x * scale * padsUnitsPerPageUnit;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y * scale * padsUnitsPerPageUnit;
                  p.bulge = pnt->bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  // need to check for bulge
                  points[vertexcnt].x = p.x;
                  points[vertexcnt].y = p.y;
                  points[vertexcnt].bulge = p.bulge;
                  vertexcnt++;
               }

               if (vertexcnt > 1)
               {
                  fprintf(fp, "*LINES*      LINES ITEMS\n");
                  wPADS_Graph_Polyline(maxdesignspace, points, vertexcnt, polyFilled, widthInPadsUnits, LINETYPE,
                        piecetype, restriction);
               }
               free(points);
            }
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            RotMat2(&m, rotation);
            point2.x = (data->getText()->getPnt().x)*scale * padsUnitsPerPageUnit;
            point2.y = (data->getText()->getPnt().y)*scale * padsUnitsPerPageUnit;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);
            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = data->getText()->isMirrored();

            double w = doc->getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA() * scale * padsUnitsPerPageUnit;

            wPADS_Graph_Text( data->getText()->getText(), point2.x, point2.y,
                              data->getText()->getHeight()*scale * padsUnitsPerPageUnit, data->getText()->getHeight()*scale * padsUnitsPerPageUnit,
                              data->getText()->getRotation(),text_mirror, w, TRUE,
                              data->getText()->getHorizontalPosition(), data->getText()->getVerticalPosition());
         }
         break;

         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = /*mirror ^*/ data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, data, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  wPADS_Graph_Level(PenNum);
                  penWidth = HP[PenNum].width * padsUnitsPerPageUnit;
                  wPADS_Graph_Aperture(block->ApShape,
                                      point2.x,
                                      point2.y,
                                      block->getSizeA() * padsUnitsPerPageUnit,
                                      block->getSizeB() * padsUnitsPerPageUnit, penWidth);
*/
               } // end if layer visible
            }
            else // not aperture
            {
               point2.x = data->getInsert()->getOriginX() * scale * padsUnitsPerPageUnit;
               point2.y = data->getInsert()->getOriginY() * scale * padsUnitsPerPageUnit;
               TransPoint2(&point2, 1, &m, insert_x,insert_y);
               PADS_WriteData(fp, &(block->getDataList()),
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * data->getInsert()->getScale(),
                               padsUnitsPerPageUnit,
                               insertLayer);
            } // end else not aperture
         }
         break;
         } // end switch
      } // end for
   return;
} /* end PADS_WriteData */

//--------------------------------------------------------------
void PADS_WriteSchematicData(FILE *fp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   double padsUnitsPerPageUnit,
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

      // no level needed.
      switch(np->getDataType())
      {
         case T_POLY:
            if (doc->get_layer_visible(np->getLayerIndex(), mirror))
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
                  double widthInPadsUnits = getLimitedPadsWidth(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale,padsUnitsPerPageUnit);
                  //double w = doc->widthTable[poly->getWidthIndex()]->getSizeA() * scale;
                  //if (w < 0.001) w = 0.001;  // limits in PADS
                  //if (w > 250)   w = 250;

                  vertexcnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale * padsUnitsPerPageUnit;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale * padsUnitsPerPageUnit;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // need to check for bulge
                     points[vertexcnt].x = p.x;
                     points[vertexcnt].y = p.y;
                     points[vertexcnt].bulge = p.bulge;
                     vertexcnt++;
                  }
                  if (vertexcnt > 1)
                  {
                     fprintf(fp,"*LINES*      LINES ITEMS\n");
                     wPADS_GraphSchem_Polyline(points,vertexcnt,polyFilled, widthInPadsUnits);
                  }
                  free(points);
               }
            } // if color
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            if (!doc->get_layer_visible(np->getLayerIndex(), mirror))
               continue;

            RotMat2(&m, rotation);
            point2.x = (np->getText()->getPnt().x)*scale * padsUnitsPerPageUnit;
            point2.y = (np->getText()->getPnt().y)*scale * padsUnitsPerPageUnit;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);
            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wPADS_GraphSchem_Text( np->getText()->getText(), point2.x, point2.y,
                                   np->getText()->getHeight()*scale * padsUnitsPerPageUnit, np->getText()->getHeight()*scale * padsUnitsPerPageUnit,
                                   np->getText()->getRotation(),text_mirror);
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
/*
                  wPADS_Graph_Level(PenNum);
                  penWidth = HP[PenNum].width * padsUnitsPerPageUnit;
                  wPADS_Graph_Aperture(block->ApShape,
                                      point2.x,
                                      point2.y,
                                      block->getSizeA() * padsUnitsPerPageUnit,
                                      block->getSizeB() * padsUnitsPerPageUnit, penWidth);
*/
               } // end if layer visible
            }
            else // not aperture
            {
               point2.x = np->getInsert()->getOriginX() * scale * padsUnitsPerPageUnit;
               point2.y = np->getInsert()->getOriginY() * scale * padsUnitsPerPageUnit;
               TransPoint2(&point2, 1, &m, insert_x,insert_y);
               PADS_WriteSchematicData(fp, &(block->getDataList()),
                               point2.x,
                               point2.y,
                               block_rot, //rotation + np->getInsert().angle,
                               block_mirror,
                               scale * np->getInsert()->getScale(),
                               padsUnitsPerPageUnit,
                               np->getLayerIndex());
            } // end else not aperture
         }
         break;
      } // end switch
   } // end for
   return;
} /* end PADS_WriteSchematicData */

/****************************************************************************/
/*
*/
static int load_PADSsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   optionTranslateHatchLines = true;
   optionWritePourOutOnly    = false;
   optionColorAllLayers      = false;
   optionAddPlaneTie         = false;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage( tmp,"PADS Settings", MB_OK | MB_ICONHAND);
      return 0;
   }

   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".LAYERMAP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
               continue;
            CString cclayer = _strupr(lp);
            cclayer.TrimLeft();
            cclayer.TrimRight();

            if ((lp = get_string(NULL," \t;\n")) == NULL)
               continue;
            int padslayernum = atoi(lp);

            int laytype = stringToLayerTypeTag(cclayer);
            PADS_LayerMap *l = new PADS_LayerMap;
            layermaparray.SetAtGrow(layermapcnt++, l);
            l->setLayerType(laytype);
            l->padslayernr = padslayernum;
         }
         else if (!STRCMPI(lp, ".TRANSLATE_HATCH"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'N')
               optionTranslateHatchLines = false;
         }
         else if (!STRICMP(lp, ".LAYERNAME"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            int level = atoi(lp);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString layername = lp;

            if (level >= 1 && level <= MAX_PADSLAYER)
               pads_layer[level-1] = layername;
         }
         else if (!STRCMPI(lp, ".WRITE_POUROUT_ONLY"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'Y')
               optionWritePourOutOnly = true;
         }
         else if (!STRCMPI(lp, ".COLOR_ALL_LAYERS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'Y')
               optionColorAllLayers = true;
         }
         else if (!STRCMPI(lp, ".ADD_PLANE_TIE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'Y')
               optionAddPlaneTie = true;
         }
      }
   }

   if (optionWritePourOutOnly)
      optionTranslateHatchLines = false;

   fclose(fp);
   return 1;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//   for (int i=0; i<MAX_LAYTYPE; i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return LAYTYPE_UNKNOWN;
//}

/******************************************************************************
* ExplodeLayerDependentComponent
*/
static void ExplodeLayerDependentComponents(CCEtoODBDoc *doc)
{
   CMapStringToString layerToTypeMap;
   CMapStringToString mirrorGeomMap;
   CMapStringToString exploredGeomMap;

   // Get the layer type and put it into layerToTypeMap
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (!layer)
         continue;

      if (layer->getNeverMirror())
         layerToTypeMap.SetAt(layer->getName(), "mirror_never");
      else if (layer->getMirrorOnly())
         layerToTypeMap.SetAt(layer->getName(), "mirror_always");
      else
         layerToTypeMap.SetAt(layer->getName(), "mirror_normal");
   }

   // Explode all the block that has layer dependent inserts
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      if (file->getBlockType() != BLOCKTYPE_PCB)
         continue;

      // Explore the block to see if there is any layer dependent inserts
      ExploreBlock(doc, file, file->getBlock(), layerToTypeMap, mirrorGeomMap, exploredGeomMap);
      DuplicateDeviceType(file, mirrorGeomMap);
   }

   // Set to opposite layer type (i.e PAD_TOP -> BAD_BOT) if layer is mirror always
   SetToOppositeLayerType(layerToTypeMap);

   layerToTypeMap.RemoveAll();
   mirrorGeomMap.RemoveAll();
   exploredGeomMap.RemoveAll();
}

/******************************************************************************
* ExplodeComponent

   Explore the block to see if there is any layer dependent inserts
*/
static BOOL ExploreBlock(CCEtoODBDoc *doc, FileStruct *file, BlockStruct *block, CMapStringToString &layerToTypeMap,
                         CMapStringToString &mirrorGeomMap, CMapStringToString &exploredGeomMap)
{
   CCamCadData& camCadData = doc->getCamCadData();

   BOOL createMirrorBlock = FALSE;
   CString layerType = "";

   // Go throught the block to see if there is any layer dependent inserts
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);
      if (!data)
         continue;
      if (data->getDataType() != T_INSERT && data->getDataType() != T_POLY && data->getDataType() != T_TEXT)
         continue;

      if (data->getDataType() == T_INSERT &&  data->getInsert()->getInsertType() != INSERTTYPE_APERTURE)
      {
         BlockStruct *subBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());

         if (subBlock->getFlags() & (BL_WIDTH | BL_TOOL | BL_APERTURE | BL_BLOCK_APERTURE))
         {
            if (IsLayerDependent(data->getLayerIndex(), layerToTypeMap, layerType))
               createMirrorBlock = TRUE;
         }
         else
         {
            // Explore the block if it not already did
            CString tmp = "";
            if (!exploredGeomMap.Lookup(subBlock->getName(), tmp))
            {
               if (ExploreBlock(doc, file, subBlock, layerToTypeMap, mirrorGeomMap, exploredGeomMap))
                  createMirrorBlock = TRUE;

               exploredGeomMap.SetAt(subBlock->getName(), subBlock->getName());
            }
         }
      }
      else if (IsLayerDependent(data->getLayerIndex(), layerToTypeMap, layerType))
      {
         createMirrorBlock = TRUE;
      }
   }

   // Create a mirror block if there is layer dependent inserts
   if (createMirrorBlock)
   {
      // Get a name for the mirrored block
      int count = 0;
      CString blockName = block->getName() + "[m]";
      while (Graph_Block_Exists(doc, blockName, block->getFileNumber()))
         blockName.Format("%s[m_%d]", block->getName(), ++count);

      // Create the mirrored block
      BlockStruct *mirrorBlock = Graph_Block_On(GBO_APPEND, blockName, file->getFileNumber(), 0L);
      Graph_Block_Off();
      mirrorBlock->setBlockType(block->getBlockType());
      doc->CopyAttribs(&mirrorBlock->getAttributesRef(), block->getAttributesRef());
      mirrorGeomMap.SetAt(block->getName(), mirrorBlock->getName());

      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
      {
         POSITION curPos = pos;
         DataStruct *data = block->getDataList().GetNext(pos);
         if (!data)
            continue;

         if (data->getDataType() == T_INSERT &&  data->getInsert()->getInsertType() != INSERTTYPE_APERTURE)
         {
            BlockStruct *subBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
            CString subMirBlockName = "";

            if (!(subBlock->getFlags() & (BL_WIDTH | BL_TOOL | BL_APERTURE | BL_BLOCK_APERTURE)))
            {
               if (mirrorGeomMap.Lookup(subBlock->getName(), subMirBlockName))
               {
                  // There is a mirrored block for this subblock
                  if (data->getInsert()->getMirrorFlags() & MIRROR_LAYERS)
                  {
                     // The inserted is mirrored, copy the original insert to the mirrored block
                     DataStruct *copy = camCadData.getNewDataStruct(*data);
                     mirrorBlock->getDataList().AddTail(copy);

                     // Then change the original insert to point to the mirrored subblock
                     BlockStruct *subMirBlock = doc->Find_Block_by_Name(subMirBlockName, file->getFileNumber());
                     data->getInsert()->setBlockNumber(subMirBlock->getBlockNumber());
                  }
                  else
                  {
                     // The inserted is not mirrored, copy the original insert to the mirrored block
                     DataStruct *copy = camCadData.getNewDataStruct(*data);
                     mirrorBlock->getDataList().AddTail(copy);

                     // Then change the copy insert to point to the mirrored subblock
                     BlockStruct *subMirBlock = doc->Find_Block_by_Name(subMirBlockName, file->getFileNumber());
                     copy->getInsert()->setBlockNumber(subMirBlock->getBlockNumber());
                  }
               }
               else
               {
                  // No mirrored block is found for the insert, make a copy and add to mirrored block
                  DataStruct *copy = camCadData.getNewDataStruct(*data);
                  mirrorBlock->getDataList().AddTail(copy);
               }

               continue;
            }
         }


         if (IsLayerDependent(data->getLayerIndex(), layerToTypeMap, layerType))
         {
            if (!layerType.CompareNoCase("mirror_always"))
            {
               // If layer is mirror always, remove from the original block and add to the mirrored block
               block->getDataList().RemoveAt(curPos);
               mirrorBlock->getDataList().AddTail(data);
            }
            //else
            //{
            // // If layer is mirror never, make a copy and add to mirrored block
            // DataStruct *copy = camCadData.getNewDataStruct(*data);
            // mirrorBlock->getDataList().AddTail(copy);
            //}

            // Set layer mirror to normal
            LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];
            layer->setLayerFlags(0);
         }
         else
         {
            // Layer is mirror normal, make a copy and add to mirrored block
            DataStruct *copy = camCadData.getNewDataStruct(*data);
            mirrorBlock->getDataList().AddTail(copy);
         }
      }
   }

   return createMirrorBlock;
}

/******************************************************************************
* IsLayerDependent
*/
static BOOL IsLayerDependent(int layerNum, CMapStringToString &layerToTypeMap, CString &layerType)
{
   if (layerNum < 0)
      return FALSE;

   LayerStruct *layer = doc->getLayerArray()[layerNum];
   layerType = "";

   if (layer == NULL)
      return FALSE;

   if (!layerToTypeMap.Lookup(layer->getName(), layerType))
   {
      if (layer->getNeverMirror())
         layerType = "mirror_never";
      else if (layer->getMirrorOnly())
         layerType = "mirror_always";
      else
         layerType = "mirror_normal";

      layerToTypeMap.SetAt(layer->getName(), layerType);
   }

   if (layerType.CompareNoCase("mirror_normal"))
      return TRUE;
   else
      return FALSE;
}

/******************************************************************************
* DuplicateDeviceType
   Copy the device type of the original and point it to this the mirrored block
*/
static void DuplicateDeviceType(FileStruct *file, CMapStringToString &mirrorGeomMap)
{
   POSITION pos = file->getTypeList().GetHeadPosition();
   while (pos)
   {
      TypeStruct *deviceType = file->getTypeList().GetNext(pos);
      if (!deviceType || deviceType->getBlockNumber() < 0)
         continue;

      BlockStruct *block = doc->getBlockAt(deviceType->getBlockNumber());
      if (!block || block->getName() == "")
         continue;

      CString mirrorBlockName = "";
      if (!mirrorGeomMap.Lookup(block->getName(), mirrorBlockName))
         continue;
      BlockStruct *mirrorBlock = doc->Find_Block_by_Name(mirrorBlockName, file->getFileNumber());

      // Create new device type for the mirrored block and copy the attribute of device type for the original block
      CString newTypeName = deviceType->getName() + "[m]";
      TypeStruct *newType = AddType(file, newTypeName);
      doc->CopyAttribs(&newType->getAttributesRef(), deviceType->getAttributesRef());
      newType->setBlockNumber( mirrorBlock->getBlockNumber());
   }
}

/******************************************************************************
* SetToOppositeLayerType
*/
static void SetToOppositeLayerType(CMapStringToString &layerToTypeMap)
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (!layer)
         continue;

      CString layerType = "";
      if (!layerToTypeMap.Lookup(layer->getName(), layerType))
         continue;

      // Skip if not mirror always
      if (layerType.CompareNoCase("mirror_always"))
         continue;

      if (layer->getLayerType() == layerTypeSignalTop)
         layer->setLayerType(layerTypeSignalBottom);

      else if (layer->getLayerType() == layerTypeSignalBottom)
         layer->setLayerType(layerTypeSignalTop);

      else if (layer->getLayerType() == layerTypePadTop)
         layer->setLayerType(layerTypePadBottom);

      else if (layer->getLayerType() == layerTypePadBottom)
         layer->setLayerType(layerTypePadTop);

      else if (layer->getLayerType() == layerTypePasteTop)
         layer->setLayerType(layerTypePasteBottom);

      else if (layer->getLayerType() == layerTypePasteBottom)
         layer->setLayerType(layerTypePasteTop);

      else if (layer->getLayerType() == layerTypeMaskTop)
         layer->setLayerType(layerTypeMaskBottom);

      else if (layer->getLayerType() == layerTypeMaskBottom)
         layer->setLayerType(layerTypeMaskTop);

      else if (layer->getLayerType() == layerTypeSilkTop)
         layer->setLayerType(layerTypeSilkBottom);

      else if (layer->getLayerType() == layerTypeSilkBottom)
         layer->setLayerType(layerTypeSilkTop);

      else if (layer->getLayerType() == layerTypeTop)
         layer->setLayerType(layerTypeBottom);

      else if (layer->getLayerType() == layerTypeBottom)
         layer->setLayerType(layerTypeTop);

      else if (layer->getLayerType() == layerTypeComponentDftTop)
         layer->setLayerType(layerTypeComponentDftBottom);

      else if (layer->getLayerType() == layerTypeComponentDftBottom)
         layer->setLayerType(layerTypeComponentDftTop);

      else if (layer->getLayerType() == layerTypeFluxTop)
         layer->setLayerType(layerTypeFluxBottom);

      else if (layer->getLayerType() == layerTypeFluxBottom)
         layer->setLayerType(layerTypeFluxTop);
   }
}

/******************************************************************************
* ConvertIllegalApertureToComplex
*/
void ConvertIllegalApertureToComplex(CCEtoODBDoc *doc)
{
   CCamCadData& camCadData = doc->getCamCadData();

   // Get the X & Y offset of all the apertures
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block || !block->isAperture())
         continue;

      if (block->getShape() == T_COMPLEX)
         continue;

      // Skip the aperture if there is not offset
      if (block->getXoffset() == 0 && block->getYoffset() == 0)
         continue;

      if (block->getShape() == T_RECTANGLE || block->getShape() == T_OBLONG)
      {
         block->setSizeA(block->getSizeA() - (DbUnit)Units_Factor(UNIT_MILS, page_units));
         block->setSizeB(block->getSizeB() - (DbUnit)Units_Factor(UNIT_MILS, page_units));
      }
      else if (block->getShape() == T_DONUT || block->getShape() == T_THERMAL)
      {
         block->setSizeA(block->getSizeA() - (DbUnit)Units_Factor(UNIT_MILS, page_units));
         block->setSizeB(block->getSizeB() + (DbUnit)Units_Factor(UNIT_MILS, page_units));
      }
      else if (block->getShape() == T_ROUND || block->getShape() == T_SQUARE || block->getShape() == T_OCTAGON ||
               block->getShape() == T_BLANK || block->getShape() == T_TARGET)
      {
         block->setSizeA(block->getSizeA() - (DbUnit)Units_Factor(UNIT_MILS, page_units));
      }

      // Get a unique name
      int count = 0;
      CString complexName = block->getName() + "_COMPLEX";
      while (Graph_Block_Exists(doc, complexName, block->getFileNumber()))
         complexName.Format("%s_COMPLEX%d", block->getName(), ++count);

      // Convert the aperture to poly and add it to the new block
      BlockStruct *complex = Graph_Block_On(GBO_APPEND, complexName, block->getFileNumber(), 0L);
      DataStruct *data = camCadData.getNewDataStruct(dataTypePoly);
      AddEntity(data);
      Graph_Block_Off();

      int err;
      int width = Graph_Aperture("OneMil", T_ROUND, onemil, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, &err);

      //data->getAttributesRef() = NULL;
      //data->setDataType(dataTypePoly);
      data->setLayerIndex(Graph_Level("0", "", 1));
      //data->setFlags(0);
      //data->setNegative(false);
      //data->setGraphicClass(graphicClassNormal);
      //data->setSelected(false);
      //data->setMarked(false);
      //data->setColorOverride(false);
      //data->setOverrideColor(0);
      //data->setHidden(false);

      CPolyList* polyList = ApertureToPoly_Base(block, 0, 0, 0, 0);

      if (polyList != NULL)
      {
         *(data->getPolyList()) = *polyList;
      }

      delete polyList;

      data->getPolyList()->setWidthIndex(width);


      // Change the aperturen to a complex and point it to the new block
      block->setShape(T_COMPLEX);
      block->setSizeA((DbUnit)complex->getBlockNumber());
      block->setFlags(BL_APERTURE | BL_GLOBAL);
      block->setSizeB(0);
      block->setXoffset(0);
      block->setYoffset(0);
      block->setRotation(0);
      block->setSizeC(0);
      block->setSizeD(0);
      block->setSpokeCount(0);
   }
}

//_____________________________________________________________________________
void DecalPinNameArray::normalizePinNumbers()
{
   CMapPtrToPtr minPinNumberMap;
   int minPinNumber;

   for (int index = 0;index < GetSize();index++)
   {
      PADS_DecalPinNameStruct* pinNameStruct = GetAt(index);

      if (pinNameStruct != NULL)
      {
         int blockNumber = pinNameStruct->blocknr;

         if (minPinNumberMap.Lookup((void*)blockNumber,(void*&)minPinNumber))
         {
            if (pinNameStruct->pinnr < minPinNumber)
            {
               minPinNumber = pinNameStruct->pinnr;
               minPinNumberMap.SetAt((void*)blockNumber,(void*)minPinNumber);
            }
         }
         else
         {
            minPinNumber = pinNameStruct->pinnr;
            minPinNumberMap.SetAt((void*)blockNumber,(void*)minPinNumber);
         }
      }
   }

   for (int index = 0;index < GetSize();index++)
   {
      PADS_DecalPinNameStruct* pinNameStruct = GetAt(index);

      if (pinNameStruct != NULL)
      {
         int blockNumber = pinNameStruct->blocknr;
         int delta = 0;

         if (minPinNumberMap.Lookup((void*)blockNumber,(void*&)minPinNumber))
         {
            if (minPinNumber < 1)
            {
               delta = 1 - minPinNumber;
            }
         }

         pinNameStruct->m_normalizedPinNumber = pinNameStruct->pinnr + delta;
      }
   }
}

//_____________________________________________________________________________
CPadsDecalPin::CPadsDecalPin(const CString& pinName,int pinNumber) :
   m_pinName(pinName),m_pinNumber(pinNumber)
{
}

//_____________________________________________________________________________
CPadsDecalPin& CPadsDecalPins::addPin(const CString& pinName,int pinNumber)
{
   CPadsDecalPin* pin = NULL;
   CString pinDescriptor;
   pinDescriptor.Format("%s[%d]",pinName,pinNumber);

   if (!Lookup(pinDescriptor,pin))
   {
      pin = NULL;
   }

   if (pin == NULL)
   {
      pin = new CPadsDecalPin(pinName,pinNumber);
      SetAt(pinDescriptor,pin);
   }

   return *pin;
}

//_____________________________________________________________________________
CPadsDecal::CPadsDecal(BlockStruct& block) :
   m_block(block)
{
}

//_____________________________________________________________________________
CPadsDecal& CPadsDecals::addDecal(BlockStruct& block)
{
   CPadsDecal* decal = NULL;

   if (! m_decals.Lookup(block.getBlockNumber(),decal))
   {
      decal = new CPadsDecal(block);
      m_decals.SetAt(block.getBlockNumber(),decal);
   }

   return *decal;
}

CPadsDecal& CPadsDecals::getDecal(BlockStruct& block)
{
   return addDecal(block);
}

static bool warnAboutPageSize()
{
   double factor = Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES);

   double xmin, xmax, ymin, ymax;
   doc->get_extents(&xmin, &xmax, &ymin, &ymax);
   double width = xmax - xmin;
   double height = ymax - ymin;

   if (width * factor > 56 || height * factor > 56 || xmax * factor > 56 || ymax * factor > 56 || xmin * factor < -56 || ymin * factor < -56)
   {
      if (ErrorMessage("The extents of the design is larger that PADS limit of 56 Inches.\nContinue anyways?", "PADS Page Size Error", MB_YESNO) == IDNO)
         return false;
   }

   return true;
}
