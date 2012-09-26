// $Header: /CAMCAD/5.0/read_wrt/Hp3070_o.cpp 99    6/17/07 8:57p Kurt Van Ness $

/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.

   Limits
      Nodenames 40 char
      Device designators 48 char
      Pinnames 8 char
      Pathnames 1024

   Partnumbers and Device names can not have [_] [.]
   Device Names, Node Names or messages can not have [~]
   ResistorMinimum / Maximum

   Need to do
   Check_Viamask

#define  ATT_3070_COMMENT           "3070_COMMENT"
#define  ATT_3070_COMMON_PIN        "3070_COMMON_PIN"
#define  ATT_3070_CONTACT_PIN       "3070_CONTACT_PIN"
#define  ATT_3070_DEVICECLASS       "3070_DEVICECLASS"
#define  ATT_3070_FAIL_MESSAGE      "3070_FAILURE_MESSAGE"
#define  ATT_3070_HI_VALUE          "3070_HI_VAL"
#define  ATT_3070_LOW_VALUE         "3070_LOW_VAL"
#define  ATT_3070_PROBE_ACCESS      "3070_PROBE_ACCESS"
#define  ATT_3070_NTOL              "3070_NEG_TOL"
#define  ATT_3070_PTOL              "3070_POS_TOL"
#define  ATT_3070_TYPE              "3070_TYPE"
#define  ATT_3070_VALUE             "3070_VALUE"

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
#include <float.h>
#include "ck.h"
#include "outline.h"
#include "hp3070_o.h"
#include "fmt3070.h"
#include "data.h"
#include "CCEtoODB.h"
#include "DFT.h"
#include "UserDefinedMessage.h"
#include "Variant.h"
#include "RwUiLib.h"
#include "DeviceType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress; // from PORT.CPP
extern char *testaccesslayers[]; // from DBUTIL.CPP

#define ATTR_TESTSIDE_NOTPRESENT  0
#define ATTR_TESTSIDE_TOP         1
#define ATTR_TESTSIDE_BOT         2


/* STATIC PROTOTYPES ************************************************************************/

// this function only loops through entities marked with the BL_CLASS_BOARD_GEOM
static int HP3070_WriteBOARDData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary);
static int HP3070_WritePANELData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary);
static void HP3070_WriteALLCOMPONENTData(FILE *fBxy, FileStruct *pcbFile, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer,
      double accuracy);
static void HP3070_WriteToolingData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static void FreeData();
static int load_hp3070settings(const char *fname, int pageUnits);
static int copy_default(FILE *fp, const char *fname);
static long cnv_units(double dbl);

static void write_outline_with_vectored_arcs(FILE *fBxy, FileStruct *file, CPntList *pntList, int bottom, const char *outname);

static void write_bxy(bool useDFTAccessibility, FILE *fBxy, FileStruct *pcbFile, double accuracy, int IncludeVias, BOOL single_not_connected_net);
static void write_brd(CCEtoODBDoc* pDoc, FILE *fBrd, FileStruct *pcbFile, BOOL single_not_connected_net);
static void writeVariantVersion(FILE *fBrd, FileStruct *pcbFile);
static void WriteComponentDevices(CCEtoODBDoc* pDoc, FILE *fBrd, FileStruct *pcbFile, CString deviceName, int maxAllowedPinCount);
static void WriteUnusedComponents(FILE *fBrd, CDataList *DataList);

static void CollectIllegalPartnumbers(FileStruct *pcbFile, CDataList *dataList);
static void ReplaceIllegalPartnumbers(CCEtoODBDoc *doc, CDataList *dataList);
static void CollectAccess(FileStruct *pcbFile);
static void CollectProbes(FileStruct *pcbFile);

static int is_comp_test(const char *n);
static int is_comp_smd(const char *n);

static void DoAllVias(FILE *fBxy, FileStruct *pcbFile, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
      double scale, int embeddedLevel, int insertlevel, CNetList *NetList, BOOL IncludeVias, bool useDFTAccessibility, BOOL single_not_connected_net);
static void DoComponentDevices(CCEtoODBDoc* pDoc, FILE *fBrd, FileStruct *pcbFile);
static void do_boardarray(FileStruct *panelFile);
static void WritePinMapSection(FILE *fBrd, FileStruct *pcbFile);

static void AssignComponentDevice(CString compName, int hpDeviceIndex, DataStruct *compData, CVariantItem* variantItem, const CString variantVersion, CString userName);
static void AssignComponentDevices(FileStruct *pcbFile);
static BOOL AssignByCompTestArray(CString compName, int pinCount);
static BOOL AssignByAttrArray_CompName(CString compName, int pinCount, DataStruct *compData, CVariantItem* variantItem, const CString variantVersion);

static int UpdateAccessAndProbe(long accessEntityNum, long dataEntityNum, long probeEntityNum);
static int UpdateFeatureAccess(DataStruct *data, BOOL TestAttribute, int testMask,
      int padstackIndex, double pinRot, double scale, double *pinX, double *pinY, CString *probeAccess, CString *probeSide, bool useDFTAccessibility);
static int UpdateCompPinAccess(FileStruct *pcbFile, CompPinStruct *compPin /*unsigned long entityNum*/, BOOL TestAttribute, CAttributes** attribMap, int padstackIndex,
      int padMirror, int compBottom, double pinRot, double *pinX, double *pinY, CString *probeAccess, CString *probeSide, int pinCount, bool useDFTAccessibility);
static void UpdateCompTest(int hpDeviceIndex, CString compName, CString testString, const CString version);
static int UpdateAttr(CString attrName, CString deviceName, int type);
static void UpdateCompDeviceList(FileStruct *pcbFile);

static BOOL CompAlreadyDone(CString compName);

static void CalcAccessFeature(CAttributes* attribMap, CString& probeSide, CString& probeAccess);
static void CalcAccessComppin(CAttributes* attribMap, CString& probeSide, CString& probeAccess, int padstackIndex, int pinCount);

static int GetDeviceIndex(CString deviceName);
static int GetCompIndex(CString name);
static CString GetCompPrefix(CString compName);
static CString GetHpDeviceName(DeviceTypeTag cczDevType);
static int GetAccessProbe(long dataEntityNum);
static int GetPadstackAccess(int padstackIndex, int padMirror);

static const char* GetPadBlockName(CDataList *dataList);
static double GetViaSize(BlockStruct *block, double scale);

static int IsAttr(CString attrName, int attrType);
static int IsCompName(CString compName, int attrType);
static int IsCompPrefix(CString compName);

static void UpdateComponentAttribute(CAttributes** attribMap, CString keyword, CString value);
BlockStruct* findPadStackFromCompPin(CCEtoODBDoc *doc, CompPinStruct* compPin);

static CString getHpDevicePinName(int hpDeviceTypeIndex, CString mappedPinFunction);

/* STATIC VARIABLES ************************************************************************/

static CCEtoODBDoc					*doc;
static FileStruct					*currentFile;
static FILE							*logFp;
static FormatStruct				*Format;
static int							display_error;
static double						unitsFactor;
static double						minviadia; // minimum viasize to test
static BOOL							IncludeUnconnectedPins;
static BOOL							ProbesPlaced;

static Conndefault				conndefault;
static Capdefault					capdefault;
static Diodedefault				diodedefault;
static Fetdefault					fetdefault;
static Fusedefault				fusedefault;
static Inductordefault			inductordefault;
static Jumperdefault				jumperdefault;
static Potentiometerdefault	potentiometerdefault;
static Resistordefault			resistordefault;
static Transistordefault		transistordefault;
static Zenerdefault				zenerdefault;

static PinMapArray				pinMapArray;

static HPAccessProbeArray		accessProbeArray;
static int							accessProbeCount;

static PadstackArray				padstackarray;
static int							padstackcnt;

static TestkooArray				testkooArray;
static int							testkooCount;

static HP3070CompArray			compArray;
static int							compCount;

static HP3070CompTestMap		compTestMap;

static HPAttrArray				attrArray;  
static int							attrCount;

static HPViaArray					viaarray;   
static int							viacnt;

static PCBInstance 				pcbInstanceArray[MAX_BOARDS];
static int         				pcbInstanceCount;

static UniqueBoard 				uniqueBoardArray[MAX_BOARDS];
static int         				uniqueBoardCount;

// Variables used in load_hp3070settings
static double						resistorminimum;
static double						resistormaximum;
static bool							ShowPartNumber = true;
static bool							SHOWOUTLINE = false;
static double						ARC_ANGLE_DEGREE = 5.0;
static int                    MAX_OUTLINE_PNT_COUNT = 120;
static CString						testpointflag;
static bool							SUPPRESS_PARTNUMBERandMESSAGE = false;
static bool							useTestAttribForProbeSide = false;
static CString                pinlibPNFormat;
static CString                normalPNFormat;
static bool                   NoProbeToRemoved = false;
static CVariant*              defaultVariant;
static bool                   exportDeviceOutline;

//---------------------------------------------------------------
static CUserDefinedMessage *usrDefMsg = NULL;

static CUserDefinedMessage* GetUserDefMsg()
{
   if (usrDefMsg == NULL)
      usrDefMsg = new CUserDefinedMessage;

   return usrDefMsg;
}

static void ClearUserDefMsg()
{
   if (usrDefMsg != NULL)
      delete usrDefMsg;

   usrDefMsg = NULL;
}

//---------------------------------------------------------------

/******************************************************************************
* HP3070_WriteFiles
*/
void HP3070_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int pageUnits, double UnitsFactor, 
      const char *settingsFilename, const char *boardDefaultFilename, const char *nameCheckFilename, 
      const char *hp3070LogFilename)
{
	Format = format;
   IncludeUnconnectedPins = format->IncludeUnconnectedPins;
   doc = Doc;
   BOOL single_not_connected_net = format->OneNCNet;

   display_error = FALSE;

   if (pageUnits != UNIT_INCHES && pageUnits != UNIT_MILS && pageUnits != UNIT_MM) // if pageunits == HP return
   {
      ErrorMessage("Page UNITS must be INCHES, MILS, MM", "Error 3070 Units", MB_OK | MB_ICONHAND);
      return;
   }

   // Open log file.
   // Because of chooser in 3070 Format GUI, we just do readwrite job versus standard 
   // CAMCAD log file open differently. It is too much trouble to merge the two. It would
   // be okay if we were allowed GUI changes on CAMCAD side but we don't want that if
   // it can be avoided.
   CString localLogFilename( hp3070LogFilename ); // This would be the standard interactive CAMCAD log.

   if (getApp().m_readWriteJob.IsActivated())
   {
      // Is readwrite job aka vPlan mode.
      logFp = getApp().OpenOperationLogFile("3070.log", localLogFilename);
      if (logFp == NULL) // error message already issued, just return.
         return;
   }
   else
   {
      // Standard CCASM CAMCAD product.
      if ((logFp = fopen(localLogFilename, "wt")) == NULL) // rewrite file
      {
         ErrorMessage("Can not open Logfile!", localLogFilename, MB_ICONEXCLAMATION | MB_OK);
         return;
      }
   }

   WriteStandardExportLogHeader(logFp, "Agilent 3070");

   if (format->Delete3070Attributes)
   {
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_COMMON_PIN, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_CONTACT_PIN, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_DEVICECLASS, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_HI_VALUE, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_LOW_VALUE, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_NTOL, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_PTOL, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_TYPE, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_VALUE, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_PROBESIDE, 0));
      doc->DeleteOccurancesOfKeyword(doc->IsKeyWord(ATT_3070_PROBEACCESS, 0));
   }

   check_init(nameCheckFilename);

   attrArray.SetSize(100, 100);
   attrCount = 0;

   padstackarray.SetSize(100, 100);
   padstackcnt = 0;

   compArray.SetSize(100, 100);
   compCount = 0;

   compTestMap.empty();

   testkooArray.SetSize(100, 100);
   testkooCount = 0;

   viaarray.SetSize(100, 100);
   viacnt = 0;

   accessProbeArray.SetSize(100, 100);
   accessProbeCount = 0;

   minviadia = 0.0;

   ProbesPlaced = FALSE;

   // Settings File
   CString settingsFile( settingsFilename );  // Default from 3070 Format GUI, interactive CAMCAD mode.

   if (getApp().m_readWriteJob.IsActivated())
   {
      // Is readwrite job aka vPlan mode.
      settingsFile = getApp().getExportSettingsFilePath("3070.out");

      CString msg;
      msg.Format("Agilent 3070: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }

   load_hp3070settings(settingsFile, pageUnits);

   unitsFactor = UnitsFactor;

   FILE *fBrd = fopen(filename, "wt"); // this is the board file .brd
   if (!fBrd)
   {
      ErrorMessage(filename, "Can Not Open File", MB_OK | MB_ICONHAND);
      return;
   }

   // here check it there is a panel file also
   // open file for writting
   char bxyFilename[_MAX_PATH];
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath(filename, drive, dir, fname, ext);
   strcpy(bxyFilename, drive);
   strcat(bxyFilename, dir);
   strcat(bxyFilename, fname);
   strcat(bxyFilename, ".bxy");
   
   FILE *fBxy = fopen(bxyFilename, "wt");

   if (!fBxy)
   {
      ErrorMessage(bxyFilename, "Can Not Open File", MB_OK | MB_ICONHAND);
      return;
   }

   // board file heading
   fprintf(fBrd, "! Start of 3070 Family board file (%s)\n", filename);
   fprintf(fBrd, "! Created by %s ", getApp().getCamCadTitle());

   CTime time;
   time = time.GetCurrentTime();
   fprintf(fBrd, "%s\n", time.Format("date : %A, %B %d, %Y at %H:%M:%S"));

   fprintf(fBrd,"\n");
   fprintf(fBrd,"HEADING\n");
   fprintf(fBrd,"   \"%s\";\n", filename);
   fprintf(fBrd,"\n");

   fprintf(fBxy,"! Start of 3070 Family board_xy file (%s)\n", bxyFilename);
   fprintf(fBxy,"! Created by %s ", getApp().getCamCadTitle());

   time = time.GetCurrentTime();
   fprintf(fBxy,"%s\n", time.Format("date : %A, %B %d, %Y at %H:%M:%S"));
   fprintf(fBxy,"\n");

   CString progressString;
   progressString.Format("%s + %s", filename, bxyFilename);
   progress->SetStatus(progressString);

   // board_xy scale
   // board_xy units
   switch (pageUnits)
   {
      case UNIT_INCHES:
         fprintf(fBxy, "scale .0001;\n\n");
         fprintf(fBxy, "units inches;\n\n");
         unitsFactor *= 10000;
      break;
      case UNIT_MM:
         fprintf(fBxy, "scale .01;\n\n");
         fprintf(fBxy, "units mm;\n\n");
         unitsFactor *= 100;
      break;
      case UNIT_MILS:
         fprintf(fBxy, "scale .1;\n\n");
         fprintf(fBxy, "units mils;\n\n");
         unitsFactor *= 10;
      break;
   }

   double accuracy = get_accuracy(doc);

   generate_PADSTACKACCESSFLAG(doc, 0);

   // count PCBs and PANELs
   int pcbCount = 0;
   int panelCount = 0;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      currentFile = file;

      if (file->getBlockType() == BLOCKTYPE_PCB)
         pcbCount++;

      if (file->getBlockType() == BLOCKTYPE_PANEL)
         panelCount++;
   }

   // if a panel is switched on, do the panel; otherwise do the PCB
   if (!pcbCount && !panelCount)
   {
      ErrorMessage("No PCB or PANEL File found!", "3070 Output Error");
   }
   else if (panelCount == 1) 
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *panelFile = doc->getFileList().GetNext(filePos);

         if (!panelFile->isShown())
            continue;

         if (panelFile->getBlockType() != BLOCKTYPE_PANEL)
            continue;

         currentFile = panelFile;

         // board_xy include
         // board_xy placement
         fprintf(fBxy, "PLACEMENT %ld, %ld %d;\n\n",
               cnv_units(panelFile->getInsertX()), cnv_units(panelFile->getInsertY()),
               round(RadToDeg(panelFile->getRotation())));

         do_boardarray(panelFile);
         // here we basically have the non-mirrored and the mirrored issues.
   
         // board_xy panel outline
         {
            fprintf(fBxy, "OUTLINE");

            Outline_Start(doc, false, true);

            // first try to find panel outline
            int outline_found = HP3070_WritePANELData(&(panelFile->getBlock()->getDataList()), 
                  0.0, 0.0, 0.0, panelFile->isMirrored(), panelFile->getScale(), 0, -1, TRUE);
      
            if (!outline_found)
            {
               // now try to find any board outline geom 
               outline_found = HP3070_WritePANELData(&(panelFile->getBlock()->getDataList()), 
                     0.0, 0.0, 0.0, panelFile->isMirrored(), panelFile->getScale(), 0, -1, FALSE);
            }
         
            if (outline_found)
            {
               int returnCode;
               write_outline_with_vectored_arcs(fBxy, panelFile, Outline_GetOutline(&returnCode, accuracy), 0, "PANELOUTLINE");
            }
            else
            {
               fprintf(logFp,"Did not find a Panel Outline.\n");
               display_error++;
            }

            Outline_FreeResults();
         }

         fprintf(fBxy, ";\n\n");

         // board file global options
         copy_default(fBrd, boardDefaultFilename);

         // now here place boards
         fprintf(fBxy, "BOARDS\n");
         fprintf(fBrd, "BOARDS\n");

         // these are the placed boards
			int i=0;
         for (i=0; i<pcbInstanceCount; i++) 
         {
            PCBInstance pcbInstance = pcbInstanceArray[i];

            BlockStruct *block = doc->Find_Block_by_Num(pcbInstance.blockNum);
            fprintf(fBxy, "%d %s %ld %ld %d;\n", i+1, check_name('r', block->getName(), FALSE),
                  cnv_units(pcbInstance.x), cnv_units(pcbInstance.y), pcbInstance.rotation);
            fprintf(fBrd, "%d %s \"BOARD_%d\"\n", i+1, check_name('r', block->getName(), FALSE), i+1);
         }

         fprintf(fBxy, "\n\n");
         fprintf(fBrd, "\n\n");

         CDataList partList(false);

         for (i=0; i<uniqueBoardCount; i++)
         {
            UniqueBoard uniqueBoard = uniqueBoardArray[i];

            FileStruct *pcbFile = doc->Find_File_by_BlockPtr(uniqueBoard.block);
            
            CollectIllegalPartnumbers(pcbFile, &partList);
         }

         ReplaceIllegalPartnumbers(doc, &partList);

         // now here loop through all unique boards and write them
         for (i=0; i<uniqueBoardCount; i++) // these are the different boards
         {
            UniqueBoard uniqueBoard = uniqueBoardArray[i];

            FileStruct *pcbFile = doc->Find_File_by_BlockPtr(uniqueBoard.block);


				bool useDFTAccessibility = false;
				if (format->GenerateDFT3070==1)
				{
					/*if (doc->GetCurrentDFTSolution(*pcbFile) != NULL &&	doc->GetCurrentDFTSolution(*pcbFile)->GetTestPlan() != NULL)
						useDFTAccessibility = true;*/
				}
					
            fprintf(fBxy, "BOARD %s\n\n", check_name('r', uniqueBoard.block->getName(), FALSE));
            write_bxy(useDFTAccessibility, fBxy, pcbFile, accuracy, format->IncludeVias, single_not_connected_net);

            fprintf(fBrd, "BOARD %s\n\n", check_name('r', uniqueBoard.block->getName(), FALSE));
            write_brd(doc, fBrd, pcbFile, single_not_connected_net);

            if (i < uniqueBoardCount-1)
            {
               fprintf(fBxy, "END BOARD\n\n\n");
               fprintf(fBrd, "END BOARD\n\n\n");

               FreeData();

               load_hp3070settings(settingsFilename, pageUnits);
            }
         }
      }
   }
   else if (pcbCount == 1) 
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *pcbFile = doc->getFileList().GetNext(filePos);

         if (!pcbFile->isShown())
            continue;

         if (pcbFile->getBlockType() != BLOCKTYPE_PCB)
            continue;

         currentFile = pcbFile;

         CDataList dataList(false);
         CollectIllegalPartnumbers(pcbFile, &dataList);
         ReplaceIllegalPartnumbers(doc, &dataList);

         // board_xy include
         // board_xy placement
         fprintf(fBxy, "PLACEMENT %ld, %ld %d;\n\n", 
               cnv_units(pcbFile->getInsertX()), cnv_units(pcbFile->getInsertY()), round(RadToDeg(pcbFile->getRotation())));

			bool useDFTAccessibility = false;
			if (format->GenerateDFT3070==1)
			{
				/*if (doc->GetCurrentDFTSolution(*pcbFile) != NULL &&	doc->GetCurrentDFTSolution(*pcbFile)->GetTestPlan() != NULL)
					useDFTAccessibility = true;*/
			}

         write_bxy(useDFTAccessibility, fBxy, pcbFile, accuracy, format->IncludeVias, single_not_connected_net);

         // board file global options
         copy_default(fBrd, boardDefaultFilename);
         write_brd(doc, fBrd, pcbFile, single_not_connected_net);
      }
   }
   else if (panelCount > 1)
   {
      // too many panels
      ErrorMessage("Multiple PANEL Files Found.  Not Supported!", "3070 Output Error");
   }
   else if (pcbCount > 1)
   {
      // too many pcbs
      ErrorMessage("Multiple PCB Files Found.  Not Supported!", "3070 Output Error");
   }

   // board file end
   fprintf(fBrd, "\nEND\n");
   fclose(fBrd);

   // board_xy end
   fprintf(fBxy, "\nEND\n");
   fclose(fBxy);

   if (check_report(logFp)) 
      display_error++;
   check_deinit();

   FreeData();
   ClearUserDefMsg();

   time = time.GetCurrentTime();
   fprintf(logFp,"Close Time %s - %s\n", filename, time.Format("date : %A, %B %d, %Y at %H:%M:%S"));

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(logFp);

      // We want this to happen only for local log files, not when
      // system log file is in play.
      if (display_error)
         Logreader(localLogFilename);
   }
}

/******************************************************************************
* CollectIllegalPartnumbers
*/
static void CollectIllegalPartnumbers(FileStruct *pcbFile, CDataList *dataList)
{
   POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && data->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)
         continue;

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_PARTNUMBER, 2);

      if (attrib)
      {
         CString origPartNumber = get_attvalue_string(doc, attrib);
         CString newPartNumber = check_name('b', origPartNumber, FALSE);

         if (origPartNumber.Compare(newPartNumber))
            dataList->AddTail(data);
      }
   }
}

/******************************************************************************
* ReplaceIllegalPartnumbers
*/
static void ReplaceIllegalPartnumbers(CCEtoODBDoc *doc, CDataList *dataList)
{
   if (getApp().m_readWriteJob.IsActivated())
   {
      // vPlan read/write job mode, silent, no dialog. Leaving PN's as-is.
   }
   else
   {
      // Traditional CAMCAD mode.
      PartNumbersDlg dlg;
      dlg.doc = doc;
      dlg.dataList = dataList;
      dlg.DoModal();
   }
}

/******************************************************************************
* FreeData
*/
static void FreeData()
{
   int i;

   for (i=0; i<padstackcnt; i++)
      delete padstackarray[i];

   padstackarray.RemoveAll();
   padstackcnt = 0;

   for (i=0; i<attrCount; i++)
      delete attrArray[i];

   attrArray.RemoveAll();
   attrCount = 0;

   compTestMap.empty();

   for (i=0; i<compCount; i++)
      delete compArray[i];

   compArray.RemoveAll();
   compCount = 0;

   for (i=0; i<viacnt; i++)
      delete viaarray[i];

   viaarray.RemoveAll();
   viacnt = 0;

   for (i=0; i<pinMapArray.GetCount(); i++)
      delete pinMapArray[i];
   pinMapArray.RemoveAll();

   for (i=0; i<accessProbeCount; i++)
      delete accessProbeArray[i];

   accessProbeArray.RemoveAll();
   accessProbeCount = 0;

   for (i=0; i<testkooCount; i++)
      delete testkooArray[i];

   testkooArray.RemoveAll();
   testkooCount = 0;
}


//******************************************************************************

static int get_test_koo_index(long x, long y, int layer)
{
   for (int i=0; i<testkooCount; i++)
   {
      HP3070_testkoo *testkoo = testkooArray[i];

      if (testkoo->x == x && testkoo->y == y) // && (testkoo->layer & layer))
      {
         return i;
      }
   }

   return -1;
}

/* check_double_placement
   1= top
   2= bottom
   3= all
*/
static bool check_double_placement(long *x, long *y, int layer)
{
   bool shifted = false;

   long checkedX = *x;
   long checkedY = *y;
   int increment = 1;
   int quadrant = 1;
   while (get_test_koo_index(checkedX, checkedY, layer) > -1)
   {
      // koo already exist, shift and try again
      shifted = true;

      if (quadrant > 4) // tried all quadrants at current increment
      {
         quadrant = 1;  // start over
         increment++;   // shift outward one more unit
      }
   
      switch (quadrant)
      {
      case 1:
         checkedX = (*x) + increment;
         checkedY = (*y) + increment;
         break;
      case 2:
         checkedX = (*x) - increment;
         checkedY = (*y) + increment;
         break;
      case 3:
         checkedX = (*x) - increment;
         checkedY = (*y) - increment;
         break;
      case 4:
         checkedX = (*x) + increment;
         checkedY = (*y) - increment;
         break;
      }

      quadrant++; // quadrant to try next

   }

   *x = checkedX;
   *y = checkedY;

   HP3070_testkoo *newTestkoo = new HP3070_testkoo;

   newTestkoo->x = *x;
   newTestkoo->y = *y;
   newTestkoo->layer = layer;

   testkooArray.SetAtGrow(testkooCount, newTestkoo);
   testkooCount++;

   return shifted;
}

/******************************************************************************
* cnv_units
*  - converts from actual to HP units.
*/
static long cnv_units(double dbl)
{
   long lng = (long)floor(dbl * unitsFactor + 0.5);
   return lng;
}

/******************************************************************************
* do_padstacks
*/
static int do_padstacks(double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block) 
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {
         HP3070Padstack *padstack = new HP3070Padstack;
         padstackarray.SetAtGrow(padstackcnt++, padstack);  

         padstack->name = block->getName();
         padstack->block_num = block->getBlockNumber();

         padstack->flag = block->getFlags();
         padstack->drill = get_drill_from_block_no_scale(doc, block)*scale;
         padstack->smd = FALSE;

         if (is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
            padstack->smd = TRUE;
      }
   }

   return 1;
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

   fprintf(logFp,"Padstack [%s] not found in index\n",p);
   display_error++;

   return -1;
}

/*****************************************************************************/
/*
*/
static int is_comp_smd(const char *name)
{
   int   i;

   for (i=0;i<compCount;i++)
   {
      if (!strcmp(compArray[i]->name, name) && compArray[i]->smdflag)
         return i+1;
   }

   return 0;
}

/*****************************************************************************/
/*
*/
static int is_comp_smd_compindex(int i)
{
   if (compArray[i]->smdflag)
      return i+1;

   return 0;
}

//-----------------------------------------------------------------------------
//
// test if it is a component and not testnone
//
static bool is_pcbcomp(const char *c)
{
   for (int i = 0; i < compCount; i++)
   {
      if (strcmp(compArray[i]->name, c) == 0 && 
          (compArray[i]->inserttype == INSERTTYPE_PCBCOMPONENT ||
          compArray[i]->inserttype == INSERTTYPE_TESTPOINT))
      {
         return (!compArray[i]->testnone);
      }
   }

   return false;
}

//-----------------------------------------------------------------------------
//
// test if it is a component and not testnone
//
static bool is_pcbcomp_compindex(int i)
{
   if (compArray[i]->inserttype == INSERTTYPE_PCBCOMPONENT ||
       compArray[i]->inserttype == INSERTTYPE_TESTPOINT)
   {
      return (!compArray[i]->testnone);
   }

   return false;
}

/******************************************************************************
* do_signals
*/
static int do_signals(FILE *fp, CNetList *netList, BOOL single_not_connected_net)
{
   int netCount = 0;

   fprintf(fp,"\nCONNECTIONS\n");

   POSITION netPos = netList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = netList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET || net->getFlags() & NETFLAG_SINGLEPINNET)
         continue;

      netCount++;

      BOOL NetNameWritten = FALSE;
      CString netName = net->getNetName();

      // SaveAttribs(stream, &net->getAttributesRef());
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         // check that this is not a tooling or fiducial
         if (is_pcbcomp(compPin->getRefDes()))
         {
            if (!NetNameWritten)
            {
               netName = check_name('n',netName, FALSE);
               fprintf(fp, "   \"%s\"", netName);
            }
            NetNameWritten = TRUE;

            fprintf(fp, "\n      %s.%s", check_name('c', compPin->getRefDes(), FALSE), check_name('p', compPin->getPinName(), FALSE));
         }
      }

      if (NetNameWritten)  
         fprintf(fp, ";\n");

      //add modified NetName to attributes
      net->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttribute3070Netname), valueTypeString, (void*)netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
      netName.ReleaseBuffer();
   }

   if (!netCount)
      ErrorMessage("No Netlist found in PCB file", "Warning : 3070 Output", MB_OK | MB_ICONHAND);

   // here do Non connects
   if (IncludeUnconnectedPins)
   {
      BOOL NetNameWritten = FALSE;

      POSITION netPos = netList->GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = netList->GetNext(netPos);

         if (!(net->getFlags() & NETFLAG_UNUSEDNET) && !(net->getFlags() & NETFLAG_SINGLEPINNET))
            continue;

         CString netName = net->getNetName();

         // all unused get their own netname, this is much easier for HP to do ECOs
         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);

            // check that this is a pcb compnent -> if it was changed to a tooling or 
            // other changes.
            if (is_pcbcomp(compPin->getRefDes()))
            {
               if (single_not_connected_net)
               {
                  CString reName = check_name('n', netName, FALSE);
                  if(reName.Compare(netName))
                     netName = "NC";

                  if (!NetNameWritten)
                     fprintf(fp, "   \"NC\"");
                  NetNameWritten = TRUE;

                  fprintf(fp, "\n      %s.%s", check_name('c', compPin->getRefDes(), FALSE), check_name('p', compPin->getPinName(), FALSE));                 
               }
               else
               {
                  CString unconnectedNetName;
                  unconnectedNetName.Format("NC__%s.%s", compPin->getRefDes(), compPin->getPinName());
                  netName = check_name('n', unconnectedNetName, FALSE);
                  fprintf(fp, "   \"%s\"", netName);
                  fprintf(fp, "\n      %s.%s", check_name('c', compPin->getRefDes(), FALSE), check_name('p', compPin->getPinName(), FALSE));
                  fprintf(fp, ";\n");
               }
            }
         }

         //add modified NetName to attributes
         net->setAttrib(doc->getCamCadData(), doc->getStandardAttributeKeywordIndex(standardAttribute3070Netname), valueTypeString, (void*)netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
         netName.ReleaseBuffer();
      }

      if (NetNameWritten)  
         fprintf(fp, ";\n");

   }

   fprintf(fp,"\n\n");

   return 1;
}

//--------------------------------------------------------------
static int do_unnamednets(CNetList *NetList )
{
   NetStruct      *net;
   CompPinStruct  *compPin;
   POSITION       compPinPos, netPos;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         // check that this is not a tooling or fiducial or not testnone
         if (is_pcbcomp(compPin->getRefDes()))
         {
            if (net->getFlags() & NETFLAG_UNNAMED)  
            {
               CString  t;
               char     *argv[3];

               t.Format("$$__%s.%s",compPin->getRefDes(), compPin->getPinName());

               argv[0] = STRDUP(".s:n:n");
               argv[1] = STRDUP(net->getNetName());
               argv[2] = STRDUP(t);

               check_add_command(3, argv);

               free(argv[0]);
               free(argv[1]);
               free(argv[2]);
               break;
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* do_crossref
*/
static void do_crossref(FILE *fBrd, FileStruct *file, CNetList *NetList, BOOL single_not_connected_net)
{
   fprintf(fBrd, "\nDEVICES\n");

   for (int i=0; i<compCount; i++)
   {
      if (compArray[i]->written_out == FALSE)
         continue;

      if (compArray[i]->testnone == TRUE)
         continue;

      fprintf(fBrd, "   %s", check_name('c', compArray[i]->name, FALSE));  //\n return is done on Net

      POSITION netPos = NetList->GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = NetList->GetNext(netPos);

         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);

            if (strcmp(compArray[i]->name, compPin->getRefDes()))
               continue;

            CString netName;

            if (net->getFlags() & NETFLAG_UNUSEDNET || net->getFlags() & NETFLAG_SINGLEPINNET)
            {
               if (IncludeUnconnectedPins)
               {
                  if (single_not_connected_net)
                     netName = "NC";
                  else
                     netName.Format("NC__%s.%s", compPin->getRefDes(), compPin->getPinName());
               }
            }
            else
            {
               netName = net->getNetName();
            }
               
            fprintf(fBrd, "\n      %s.\"%s\"", check_name('p', compPin->getPinName(), FALSE), check_name('n', netName, FALSE));
         }
      }

      fprintf(fBrd, ";\n");
   }

   fprintf(fBrd, "\n");
}

// check if a component was marked as a test component
static   int   is_comp_test_compindex(int i)
{
   if (compArray[i]->testflag)
      return i+1;

   return 0;
}

// check if a component was marked as a test component
static   int   is_comp_test(const char *n)
{
   int   i;

   for (i=0;i<compCount;i++)
   {
      if (!strcmp(compArray[i]->name,n) && compArray[i]->testflag)
         return i+1;
   }

   return 0;
}


/*****************************************************************************/
/*
*/
static int calc_access_offset(double *topx, double *topy, 
                              double *botx, double *boty,
                              double rotation, 
                              CAttributes* attribMap)
{
   double   ax = 0, ay = 0, ax1, ay1;
   double   rot = RadToDeg(rotation);
   Attrib   *a;

   if (a = is_attvalue(doc, attribMap, ATT_TEST_OFFSET_TOP_X, 0)) 
   {
      CString l = get_attvalue_string(doc, a);
      ax = atof(l);
   }
   if (a = is_attvalue(doc, attribMap, ATT_TEST_OFFSET_TOP_Y, 0)) 
   {
      CString l = get_attvalue_string(doc, a);
      ay = atof(l);
   }

   // rotate
   Rotate(ax,ay,rot,&ax1,&ay1);   
   *topx = *topx + ax1;
   *topy = *topy + ay1;

   ax = 0;
   ay = 0;

   if (a = is_attvalue(doc, attribMap, ATT_TEST_OFFSET_BOT_X, 0)) 
   {
      CString l = get_attvalue_string(doc, a);
      ax = atof(l);
   }

   if (a = is_attvalue(doc, attribMap, ATT_TEST_OFFSET_BOT_Y, 0)) 
   {
      CString l = get_attvalue_string(doc, a);
      ay = atof(l);
   }

   // rotate
   Rotate(ax,ay,rot,&ax1,&ay1);   
   *botx = *botx + ax1;
   *boty = *boty + ay1;

   return 1;
}

static int testSideAttribOverride(bool useDFTAccessibility, BlockStruct *pcbBlock, CString refDes)
{
	// Return ATTR_TESTSIDE_NOTPRESENT if not overriding
	// Return ATTR_TESTSIDE_TOP if top
	// Return ATTR_TESTSIDE_BOT if bottom
	
	if (!useDFTAccessibility && useTestAttribForProbeSide && 
		pcbBlock != NULL  && !refDes.IsEmpty())
	{
		//CString refdes = compPin->getRefDes();

		DataStruct *data = pcbBlock->FindData(refDes);

		// Applies only when in "use 3070.out rules" mode is active
		if (data != NULL)
		{
			Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 0);

			if (attrib)   
			{
				CString val = get_attvalue_string(doc, attrib);

				if (!val.CompareNoCase("TOP")) 
						return ATTR_TESTSIDE_TOP;

				if (!val.CompareNoCase("BOTTOM")) 
						return ATTR_TESTSIDE_BOT;
			}
		}
	}

	return ATTR_TESTSIDE_NOTPRESENT;
}


/******************************************************************************
* do_otherpinloc
*/
static void do_otherpinloc(FILE *fBxy, FileStruct *pcbFile, CNetList *NetList, double scale, CDataList *DataList, bool useDFTAccessibility)
{
   fprintf(fBxy, "\nOTHER\n");
   fprintf(fBxy, "   ALTERNATES\n");

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);

      // SaveAttribs(stream, &net->getAttributesRef());
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         int compIndex = GetCompIndex(compPin->getRefDes());

         if (compIndex < 0)
         {
            continue;
         }

         if (!is_pcbcomp_compindex(compIndex))
         {
            continue;
         }

         HP3070Comp* hpComp = compArray[compIndex];

         if (compPin->getPinCoordinatesComplete())
         {
            double pinx, piny, pinrot;
            int padmirror;
            CString padstackName;

            pinx = compPin->getOriginX();
            piny = compPin->getOriginY();
            pinrot = RadToDeg(compPin->getRotationRadians());
            padmirror = compPin->getMirror();

            if (compPin->getPadstackBlockNumber() > -1)
            {
               //BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
               BlockStruct *block = findPadStackFromCompPin(doc, compPin);

               if (block)
                  padstackName = block->getName();
               else
               {
                  fprintf(logFp, "Can not find a padstack for Component [%s] Pin [%s]\n", compPin->getRefDes(), compPin->getPinName());
                  display_error++;
                  continue;
               }
            }
            else
            {
               fprintf(logFp, "Component [%s] Pin [%s] missing padstack info\n", compPin->getRefDes(), compPin->getPinName());
               display_error++;
               continue;
            }

            int padstackIndex = get_padstackindex(padstackName);

            if (padstackIndex < 0)
            {
               fprintf(logFp, "Can't find Padstack [%s] for Component [%s] Pin [%s]\n", padstackName, compPin->getRefDes(), compPin->getPinName());
               display_error++;
               continue;
            }

            int test_attribute = FALSE;

            // here I need to test if the component has a testpoint attribute
            if (is_comp_test_compindex(compIndex))
               test_attribute = TRUE;

            CString probeAccess, probeSide;

            int pl = UpdateCompPinAccess(pcbFile, compPin, test_attribute, &compPin->getAttributesRef(), padstackIndex, padmirror, 
						hpComp->bottom, pinrot, &pinx, &piny, &probeAccess, &probeSide, hpComp->pincnt, useDFTAccessibility);

            long px = cnv_units(pinx * scale);
            long py = cnv_units(piny * scale);

            long originalpx = px;
            long originalpy = py;

            bool isdoubled = check_double_placement(&px, &py, pl); // need to update layer for double ptr

            if (isdoubled)
            {
               fprintf(logFp, "Double Test coordinate found at (%ld, %ld) shifted to (%ld, %ld) (%s.%s)\n",
                  originalpx, originalpy, px, py, check_name('c', compPin->getRefDes(), FALSE), check_name('p', compPin->getPinName(), FALSE));

               display_error++;
				}


				//int testSide = ATTR_TESTSIDE_NOTPRESENT;

				//if (useTestAttribForProbeSide)
				//{
				//	Attrib *attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST, 0);

				//	if (attrib)   
				//	{
				//		CString val = get_attvalue_string(doc, attrib);

				//		if (!val.CompareNoCase("TOP")) 
				//				testSide = ATTR_TESTSIDE_TOP;

				//		if (!val.CompareNoCase("BOTTOM")) 
				//				testSide =  ATTR_TESTSIDE_BOT;
				//	}
				//}


				//if (testSide != ATTR_TESTSIDE_NOTPRESENT)
				//{
				//	probeSide = (testSide == ATTR_TESTSIDE_TOP ? "TOP" : "NO_MANUAL");
				//}

            // if HPDEVICE name is undefind - no_probe
            fprintf(fBxy, "      %ld, %ld %s.%s %s %s;\n",  // if top
                  px, py, check_name('c', compPin->getRefDes(), FALSE), check_name('p', compPin->getPinName(), FALSE), probeAccess, probeSide);

            //UpdateComponentAttribute(&compPin->getAttributesRef(), ATT_3070_PROBESIDE  , probeSide);      
            //UpdateComponentAttribute(&compPin->getAttributesRef(), ATT_3070_PROBEACCESS, probeAccess);   
         }
      }
   }
}

/****************************************************************************/
/*
   Update a geometry with the TEST attribute.
   if a testshape is a via, we should update the via instances.
*/
static int update_testshape(const char *s)
{
   int      found = 0;

   // it is in CCDOC doc-> not needed.
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)   continue;

      if (block->getName().CompareNoCase(s) == 0)
      {
         found++;
         doc->SetAttrib(&block->getAttributesRef(),doc->IsKeyWord(ATT_TEST, TRUE),
            VT_STRING, "", SA_OVERWRITE, NULL); //  
      }
   }

   if (!found)
   {
      fprintf(logFp,"Update Test geometry [%s] not found in design.\n",s);
      display_error++;
   }

   return 1;
}

/****************************************************************************/
/*
   numberM = 10e6
   number M = 10e6
   number K = 10e3
   number k = 10e3
   number m = 10e-3
   number u = 10e-6

*/
static double  normalize_resistorvalue(const char *val)
{
   unsigned int      i;
   double            v = 0;
   CString           num;
   char              l = ' ';

   num = "";

   // wild card, no value
   if (val[0] == '*')   
      return -1.0;

   // get number part
   for (i=0;i<strlen(val);i++)
   {
      if (isspace(val[i])) continue;
      if (isalpha(val[i])) break;
      if (val[i] == ',')   continue;

      num += val[i]; 
   }

   if (strlen(num) == 0)   return -1;

   // get unit
   for (;i<strlen(val);i++)
   {
      if (isspace(val[i])) continue;

      l = val[i];

      break;
   }

   if (strlen(num) == 0)   
      return -1.0;
   else
   {
      // make sure that this is a legal value.
      double r;
      int    u;
      u = sscanf(num,"%lf", &r); 

      if (u != 1)
         return -1;
   }

   v = atof(num);

   switch (l)
   {
      case  'M':
         v *= 1000000;
      break;
      case  'k':
      case  'K':
         v *= 1000;
      break;
      case  'm':
         v *= 0.0001;
      break;
      case  'u':
      case  'U':
         v *= 0.0000001;
      break;
   }

   return v;
}

/******************************************************************************
* load_hp3070settings
*/
static int load_hp3070settings(const char *fname, int pageUnits)
{
   resistorminimum = 0;
   ShowPartNumber = true;
   resistormaximum = DBL_MAX;   
   capdefault.defunit = "";
   resistordefault.defunit = "";
   inductordefault.defunit = "";
   fusedefault.defunit = "";
   potentiometerdefault.defunit = "";
   testpointflag = "MANDATORY";  // other possible value is PREFERRED
   ARC_ANGLE_DEGREE = 5.0;
   MAX_OUTLINE_PNT_COUNT = 120; // This was hardcoded value until this .out option came along.
	SUPPRESS_PARTNUMBERandMESSAGE = false;
   SHOWOUTLINE = false;
	useTestAttribForProbeSide = false;
   pinlibPNFormat.Empty();
   normalPNFormat.Empty();
   NoProbeToRemoved = false;
   exportDeviceOutline = false;

   // These are for keeping log output minimized, prevent duplicated messages.
   bool encounteredDeviceTypeCommand    = false;
   bool encounteredDeviceMapCommand     = false;
   bool encounteredCompTestCommand      = false;
   bool encounteredCompPrefTypeCommand  = false;
   bool encounteredCompNameTypeCommand  = false;
   bool encounteredKeyValTypeCommand    = false;

	// preset defaults
   //.DEVICEDEFAULT  CAPACITOR      VALUE=** +TOL=10 -TOL=10 3070_TYPE=F            
   //.DEVICEDEFAULT  CONNECTOR      TEST=NT           
   //.DEVICEDEFAULT  DIODE          HI-VAL=.8 LO-VAL=.4
   //.DEVICEDEFAULT  FET            HI-RES=200 LO-RES=100 3070_TYPE=N
   //.DEVICEDEFAULT  FUSE           MAX-CURRENT=1.0
   //.DEVICEDEFAULT  INDUCTOR       VALUE=** +TOL=10 -TOL=10 SERIES-R=10 3070_TYPE=F
   //.DEVICEDEFAULT  JUMPER         HP_TYPE=C
   //.DEVICEDEFAULT  POTENTIOMETER  VALUE=** +TOL=10 -TOL=10
   //.DEVICEDEFAULT  RESISTOR       VALUE=** +TOL=10 -TOL=10 3070_TYPE=F   
   //.DEVICEDEFAULT  SWITCH         COMMON-PIN=** CONTACT=OFF ;caution new
   //.DEVICEDEFAULT  TRANSISTOR     HI-BETA=200 LO-BETA=50 HP_TYPE=N
   //.DEVICEDEFAULT  ZENER          VOLTAGE=** +TOL=10 -TOL=10    

   conndefault.test = "NT";

   capdefault.mintol = "10";
   capdefault.plustol = "10";
   capdefault.typ = "F";
   capdefault.value = "**";

   diodedefault.hival = ".8";
   diodedefault.lowval = ".4";

   fetdefault.hires = "200";
   fetdefault.lowres = "100";

   fusedefault.maxcurrent = "1.0";

   inductordefault.mintol = "10";
   inductordefault.plustol = "10";
   inductordefault.series = "10";
   inductordefault.typ = "F";
   inductordefault.value = "**";

   jumperdefault.typ = "C";

   potentiometerdefault.mintol = "10";
   potentiometerdefault.plustol = "10";
   potentiometerdefault.value = "**";

   resistordefault.mintol = "10";
   resistordefault.plustol = "10";
   resistordefault.typ = "F";
   resistordefault.value = "**";

   transistordefault.hibeta = "200";
   transistordefault.lowbeta = "50";
   transistordefault.typ = "N";

   zenerdefault.mintol = "10";
   zenerdefault.plustol = "10";
   zenerdefault.voltage = "**";

   FILE *fp;
   char line[255];
   char *lp;
   long lcnt = 0;

	if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "3070 Settings", MB_OK | MB_ICONHAND);

      return 0;
   }

   fprintf(logFp, "\n-------------------------------------------------\n");
   fprintf(logFp, "Loading settings file: %s\n\n", fname);
              
   while (fgets(line, 255, fp))
   {                                
      lcnt++;
      CString tmp = line;
      tmp.TrimLeft();
      tmp.TrimRight();                 

      int f = tmp.Find(';');
      strcpy(line, tmp);

      if (f > -1)
			line[f] = '\0';

      if ((lp = get_string(line, " \t\n")) == NULL)
			continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".ResistorMinimum"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
					continue;

            resistorminimum = normalize_resistorvalue(lp);

            if (resistorminimum < 0)
            {
               fprintf(logFp, "Unknown ResistorMinimum [%s] in %s\n", lp, fname);
               display_error++;
               resistorminimum = 0;
            }
         }
         else if (!STRICMP(lp, ".ResistorMaximum"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
					continue;

            resistormaximum = normalize_resistorvalue(lp);

            if (resistormaximum < 0)
            {
               fprintf(logFp, "Unknown ResistorMaximum [%s] in %s\n", lp, fname);
               display_error++;
               resistormaximum = DBL_MAX;
            }
         }
         else if (GetUserDefMsg()->isKeyword(lp))
         {
            CString kw(lp);

				lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

            GetUserDefMsg()->append(kw, lp);
         }
         else if (!STRICMP(lp, ".MINVIADIA"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
					continue;

            minviadia = atof(lp);

            if ((lp = get_string(NULL, " \t\n")) == NULL) 
            {
               // units are assumed to be in HP units / mils
               minviadia = minviadia / 10; // now this is mils
               minviadia *= Units_Factor(UNIT_MILS, pageUnits);
            }
            else
            {
               int u;

               if ((u =  GetUnitIndex(lp)) > -1)
                  minviadia *= Units_Factor(u, pageUnits);
            }
         }
         else if (!STRICMP(lp, ".ARCSTEPANGLE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            ARC_ANGLE_DEGREE = atof(lp);

            if (ARC_ANGLE_DEGREE < 4)     // 360 / 120 not allowed
            {
               fprintf(logFp, ".ARCSTEPANGLE too small, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 4;
            }
            else if (ARC_ANGLE_DEGREE > 45)    // 360 / 120 not allowed
            {
               fprintf(logFp, ".ARCSTEPANGLE too large, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 45;
            }
         }
         else if (!STRICMP(lp, ".TESTPOINTFLAG"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            testpointflag = lp;
            testpointflag.MakeUpper();
            testpointflag.TrimLeft();
            testpointflag.TrimRight();
         }
         else if (!STRICMP(lp, ".TESTSHAPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            update_testshape(lp);
         }
         else if (!STRICMP(lp, ".TESTPREF"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            CString prefName = lp;
            prefName.MakeUpper();
            prefName.TrimLeft();
            prefName.TrimRight();
            UpdateAttr(prefName, "", HP_TESTPREF);
         }
         else if (!STRICMP(lp, ".TOOLINGPREF"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            CString prefname = lp;
            prefname.MakeUpper();
            prefname.TrimLeft();
            prefname.TrimRight();
            UpdateAttr(prefname, "", HP_TOOLINGPREF);
         }
         else if (!STRICMP(lp, ".TOOLINGSHAPE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL)
					continue;

            CString prefname = lp;
            prefname.MakeUpper();
            prefname.TrimLeft();
            prefname.TrimRight();
            UpdateAttr(prefname, "", HP_TOOLINGSHAPE);
         }
         else if (!STRICMP(lp, ".COMPTEST"))
         {
            if (!encounteredCompTestCommand)
            {
               encounteredCompTestCommand = true;
               fprintf(logFp, "%s command is obsolete, shall be ignored. Use Data Doctor to manage this property.\n", ".COMPTEST");
            }
         }
         else if (!STRICMP(lp, ".DEVICETYPE"))
         {
            if (!encounteredDeviceTypeCommand)
            {
               encounteredDeviceTypeCommand = true;
               fprintf(logFp, "%s command is obsolete, shall be ignored. Use Data Doctor to manage this property.\n", ".DEVICETYPE");
            }
         }
         else if (!STRNICMP(lp, ".DEVICEMAP", strlen(".DEVICEMAP") ))
         {
            if (!encounteredDeviceMapCommand)
            {
               encounteredDeviceMapCommand = true;
               fprintf(logFp, "%s command is obsolete, shall be ignored. Use Data Doctor to manage this property.\n", ".DEVICEMAP");
            }
         }
         else if (!STRICMP(lp, ".COMPPREFTYPE"))
         {
            if (!encounteredCompPrefTypeCommand)
            {
               encounteredCompPrefTypeCommand = true;
               fprintf(logFp, "%s command is obsolete, shall be ignored. Use Data Doctor to manage this property.\n", ".COMPPREFTYPE");
            }
         }
         else if (!STRICMP(lp, ".COMPNAMETYPE"))
         {
            if (!encounteredCompNameTypeCommand)
            {
               encounteredCompNameTypeCommand = true;
               fprintf(logFp, "%s command is obsolete, shall be ignored. Use Data Doctor to manage this property.\n", ".COMPNAMETYPE");
            }
         }
         else if (!STRICMP(lp, ".KEYVALTYPE"))
         {
            if (!encounteredKeyValTypeCommand)
            {
               encounteredKeyValTypeCommand = true;
               fprintf(logFp, "%s command is obsolete, shall be ignored. Use Data Doctor to manage this property.\n", ".KEYVALTYPE");
            }
         }
         else if (!STRICMP(lp, ".DEVICEDEFAULT"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            CString devicetype = lp;
            devicetype.MakeUpper();
            devicetype.TrimLeft();
            devicetype.TrimRight();

            int devindex = GetDeviceIndex(devicetype);
            CString val, key;

            switch (devindex)
            {
            case 0:  // "NODE LIBRARY",      // this are all devices, which do not fall into the other classes
               break;
            case 1:  // "CAPACITOR",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL) 
							continue;

                  val = lp;

                  if (!key.CompareNoCase("VALUE"))
                     capdefault.value = val;
                  else if (!key.CompareNoCase("+TOL"))
                     capdefault.plustol = val;
                  else if (!key.CompareNoCase("-TOL"))
                     capdefault.mintol = val;
                  else if (!key.CompareNoCase("3070_TYPE"))
                     capdefault.typ = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT CAPACITOR\n", key);
                     display_error++;
                  }
               }
               break;
            case 2:  // "CONNECTOR",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("TEST"))
                     conndefault.test = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT CONNECTOR\n", key);
                     display_error++;
                  }
               }
               break;
            case 3:  // "DIODE",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("HI-VAL"))
                     diodedefault.hival = val;
                  else if (!key.CompareNoCase("LO-VAL"))
                     diodedefault.lowval = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT DIODE\n", key);
                     display_error++;
                  }
               }
               break;
            case 4:  // "FET",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("HI-RES"))
                     fetdefault.hires = val;
                  else if (!key.CompareNoCase("LO-RES"))
                     fetdefault.lowres = val;
                  else if (!key.CompareNoCase("3070_TYPE"))
                     fetdefault.typ = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT FET\n", key);
                     display_error++;
                  }
               }
					break;
            case 5:  // "JUMPER",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("3070_TYPE"))
                     jumperdefault.typ = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT JUMPER\n", key);
                     display_error++;
                  }
               }
               break;
            case 6:  //"FUSE",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("MAX-CURRENT"))
                     fusedefault.maxcurrent = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT FUSE\n", key);
                     display_error++;
                  }
               }
               break;
            case 7:  // "INDUCTOR",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("VALUE"))
                     inductordefault.value = val;
                  else if (!key.CompareNoCase("+TOL"))
                     inductordefault.plustol = val;
                  else if (!key.CompareNoCase("-TOL"))
                     inductordefault.mintol = val;
                  else if (!key.CompareNoCase("SERIES-R"))
                     inductordefault.series = val;
                  else if (!key.CompareNoCase("3070_TYPE"))
                     inductordefault.typ = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT INDUCTOR\n", key);
                     display_error++;
                  }
               }

               break;
            case 8:  // "POTENTIOMETER",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("VALUE"))
                     potentiometerdefault.value = val;
                  else if (!key.CompareNoCase("+TOL"))
                     potentiometerdefault.plustol = val;
                  else if (!key.CompareNoCase("-TOL"))
                     potentiometerdefault.mintol = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT POTENTIOMETER\n", key);
                     display_error++;
                  }
               }
               break;
            case 9:  // "RESISTOR",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("3070_TYPE"))
                     resistordefault.typ = val;
                  else if (!key.CompareNoCase("VALUE"))
                     resistordefault.value = val;
                  else if (!key.CompareNoCase("+TOL"))
                     resistordefault.plustol = val;
                  else if (!key.CompareNoCase("-TOL"))
                     resistordefault.mintol = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT RESISTOR\n", key);
                     display_error++;
                  }
               }
               break;
            case 10: // "SWITCH",
               break;
            case 11: //"TRANSISTOR",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("HI-BETA"))
                     transistordefault.hibeta = val;
                  else if (!key.CompareNoCase("LO-BETA"))
                     transistordefault.lowbeta = val;
                  else if (!key.CompareNoCase("3070_TYPE"))
                     transistordefault.typ = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT TRANSISTOR\n", key);
                     display_error++;
                  }
               }
               break;
            case 12: // "ZENER",
               while ((lp = get_string(NULL, "=")) != NULL)
               {
                  key = lp;
                  key.TrimLeft();
                  key.TrimRight();

                  if ((lp = get_string(NULL, " \t\n")) == NULL)
							continue;

                  val = lp;

                  if (!key.CompareNoCase("VOLTAGE"))
                     zenerdefault.voltage = val;
                  else if (!key.CompareNoCase("+TOL"))
                     zenerdefault.plustol = val;
                  else if (!key.CompareNoCase("-TOL"))
                     zenerdefault.mintol = val;
                  else
                  {
                     fprintf(logFp, "Unknown parameter [%s] in .DEVICEDEFAULT ZENER\n", key);
                     display_error++;
                  }
               }
               break;
            case 13: // "UNDEFINED",
            case 14: // "PIN LIBRARY",        // must be last 
               break;
            default:
               fprintf(logFp, "Unknown Devicetype [%s] in 3070.out\n", devicetype);
               display_error++;
               break;
            }
         }
         else if (!STRCMPI(lp, ".DEFAULTUNIT_CAPACITOR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            capdefault.defunit = lp;
         }
         else if (!STRCMPI(lp, ".DEFAULTUNIT_INDUCTOR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            inductordefault.defunit = lp;
         }
         else if (!STRCMPI(lp, ".DEFAULTUNIT_FUSE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            fusedefault.defunit = lp;
         }
         else if (!STRCMPI(lp, ".DEFAULTUNIT_POTENTIOMETER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            potentiometerdefault.defunit = lp;
         }
         else if (!STRCMPI(lp, ".DEFAULTUNIT_RESISTOR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            resistordefault.defunit = lp;
         }
         else if (!STRCMPI(lp, ".SHOWOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            SHOWOUTLINE = (toupper(lp[0]) == 'Y');
         }
         else if (!STRCMPI(lp, ".MAX_OUTLINE_POINTS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            MAX_OUTLINE_PNT_COUNT = atoi(lp);
         }
         else if (!STRICMP(lp, ".PARTNUMBER2MESSAGE"))   
         {
				// While fixing TSR 3070 out, implement to read this keyword to put part number
				// The default is to put partnumber.

            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if (lp[0] == 'N' || lp[0] == 'n')
               ShowPartNumber = false;
         }
         else if (!STRICMP(lp, ".UseTestAttribute"))   
         {
				// Use TEST attribute to determine probe side, active only in "3070.out rules" mode,
				// has no effect in DFT mode.

            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if (lp[0] == 'Y' || lp[0] == 'y')
               useTestAttribForProbeSide = true;
         }
         else if (!STRICMP(lp, ".PINMAP"))
         {
            int deviceindex;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if ((deviceindex = GetDeviceIndex(lp)) < 0)
            {
               fprintf(logFp, ".PINMAP command contains unknown 3070 Device Type [%s].\n", lp);
               display_error++;
               continue;
            }

            if ((lp = get_string(NULL," \t\n")) == NULL)
					continue;

            CString oldPin = lp;

            if ((lp = get_string(NULL," \t\n")) == NULL)
					continue;

            CString newPin = lp;

            pinMapArray.AddPinMap(deviceindex, oldPin, newPin);
         }
         else if (!STRICMP(lp, ".SUPPRESS_PARTNUMBERandMESSAGE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if (toupper(lp[0]) == 'Y')
					SUPPRESS_PARTNUMBERandMESSAGE = true;
			}
         else if (!STRICMP(lp, ".PIN_LIBRARY") || !STRICMP(lp, ".PIN_LIBRARY_PN"))
         {
            // Is white space delimited multi-field value
            // E.g. .PIN_LIBRARY_PN    PARTNUMBER SUBCLASS
            if ((lp = get_string(NULL, "\n")) == NULL)
					continue;

            pinlibPNFormat = lp;
			}
         else if (!STRICMP(lp, ".NORMAL_PN"))
         {
            // Is white space delimited multi-field value
            // E.g. .NORMAL_PN    PARTNUMBER SUBCLASS
            if ((lp = get_string(NULL, "\n")) == NULL)
					continue;

            normalPNFormat = lp;
			}
         else if (!STRICMP(lp, ".NOPROBE_TO_REMOVED"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if (toupper(lp[0]) == 'Y')
					NoProbeToRemoved = true;
			}
         else if(!STRICMP(lp, ".DEVICE_OUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")))
            {
               exportDeviceOutline = (toupper(lp[0]) == 'Y')?true:false;
            }
         }
      }
   }

   fprintf(logFp, "\nFinished loading settings file.\n");
   fprintf(logFp, "-------------------------------------------------\n\n");

   fclose(fp);
   return 1;
}

/******************************************************************************
*/
bool PinMapArray::AddPinMap(int hpDeviceIndex, CString oldPin, CString newPin)
{
   // Can't have two that map same oldPin

   if (!oldPin.IsEmpty() && !newPin.IsEmpty())
   {
      if (!FindPinMap(hpDeviceIndex, oldPin))
      {
         PinMapEntry *mapEntry = new PinMapEntry(hpDeviceIndex, oldPin, newPin);
         this->Add(mapEntry);

         return true;
      }
   }

   return false; // Failed to add due to duplication of oldPin, or one or both names blank.
}

PinMapEntry *PinMapArray::FindPinMap(int hpDeviceIndex, CString oldPin)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      PinMapEntry *mapEntry = this->GetAt(i);
      if (mapEntry->m_hpDeviceIndex == hpDeviceIndex && mapEntry->m_oldPin.CompareNoCase(oldPin) == 0)
      {
         return mapEntry;
      }
   }

   return NULL;
}

/******************************************************************************
*/
static int copy_default(FILE *fp, const char *fname)
{
   FILE  *cfp;
   char  line[127+1];

   // open file for writting
   if ((cfp = fopen(fname, "rt")) == NULL)
   {
      // error - can not open the file.
      CString tmp;
      tmp.Format("Can not open %-70s",fname);
      ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);

      return 0;
   }

   fprintf(fp,"\n! start of default file %s\n",fname);

   while (fgets(line,127,cfp))
      fputs(line,fp);

   fprintf(fp,"\n! end of default file\n\n");               
   fclose(cfp);

   return 1;
}

/******************************************************************************
* GetPinCount
*/
static int GetPinCount(CDataList *dataList)
{
   int pinCount = 0;

   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);
      
      if (data && data->isInsertType(insertTypePin))
         pinCount++;
   }

   return pinCount;
}

/******************************************************************************
* UpdateCompDeviceList
*/
static void UpdateCompDeviceList(FileStruct *pcbFile)
{
   POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && data->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)   
         continue;

      CString compName = data->getInsert()->getRefname();

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      // 3. look at .Devicetype : This is done upfront, so that the component does not get other thru some filters
      Attrib *attrib;

		//check for subclass
      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_3070_DEVICECLASS, 2))
      {
         CString deviceName = get_attvalue_string(doc, attrib);
         DeviceTypeTag devTypeTag = stringToDeviceTypeTag(deviceName);
         CString deviceMapName = GetHpDeviceName(devTypeTag);
         if (GetDeviceIndex(deviceMapName) > -1)
            UpdateAttr(compName, deviceMapName, HP_COMPNAME);
      }
		// check if devicetype was setup
      else if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_DEVICETYPE, 2))
      {
         CString deviceName = get_attvalue_string(doc, attrib);
         DeviceTypeTag devTypeTag = stringToDeviceTypeTag(deviceName);
         CString deviceMapName = GetHpDeviceName(devTypeTag);  // Not all things map, might come back empty.
         if (GetDeviceIndex(deviceMapName) > -1)               // This weeds out the unsupported device type.
            UpdateAttr(compName, deviceMapName, HP_COMPNAME);
      }
      else
      {
         // check is the devicetype is part of the type list

         // find typelink on comp
         CString type_name = "";
         Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TYPELISTLINK, 1);

         if (!attrib)
            continue;

         type_name = get_attvalue_string(doc, attrib);
      
         // now loop thru typelist 
         POSITION typePos = pcbFile->getTypeList().GetHeadPosition();
         while (typePos)
         {                                         
            TypeStruct *type = pcbFile->getTypeList().GetNext(typePos);

            if (strcmp(type->getName(), type_name))  
               continue;

            // here typelist found
            // now look if there is a ATT_DEVICETYPE entry
            Attrib *attrib = is_attvalue(doc, type->getAttributesRef(), ATT_DEVICETYPE, 2);

            if (attrib)
            {
               CString deviceName = get_attvalue_string(doc, attrib);
               DeviceTypeTag devTypeTag = stringToDeviceTypeTag(deviceName);
               CString deviceMapName = GetHpDeviceName(devTypeTag);
               if (GetDeviceIndex(deviceMapName) > -1)
                  UpdateAttr(compName, deviceMapName, HP_COMPNAME);
            }
         }
      }
   }
}


/*****************************************************************************/
/*
   update the vias instances if the geometry is set to TEST.
*/
static int update_via_comp_list(FileStruct *file)
{
   DataStruct *np;

   POSITION pos1;
   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      pos1 = pos; // pos is changed in getnext
      np = file->getBlock()->getDataList().GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   
            {
               Attrib *attrib = is_attvalue(doc, np->getAttributesRef(), ATT_TEST, 2); // here check if the geom has a test attribute

               if (attrib)
               {
                  int testnone = FALSE;

                  if (attrib->getValueType() == VT_STRING)
                  {
                     CString layerName = get_attvalue_string(doc, attrib);
                     if (!layerName.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_NONE]))
                        testnone = TRUE;
                  }

                  CString compname = np->getInsert()->getRefname();
                  // set testflag if block was set to test
                  int   cindex = GetCompIndex(compname);

                  if (cindex > -1)
                  {
                     compArray[cindex]->testflag = TRUE;                   
                     compArray[cindex]->testnone = testnone;
                     compArray[cindex]->bottom = np->getInsert()->getPlacedBottom();
                  }
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)   
            {
               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_TEST, 2);  // here check if the geom has a test attribute

               if (a)
               {
                  int   testnone = FALSE;

                  if (a->getValueType() == VT_STRING)
                  {
                     CString l = get_attvalue_string(doc, a);

                     if (!l.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_NONE]))
                        testnone = TRUE;
                  }

                  CString compname = np->getInsert()->getRefname();
                  // set testflag if block was set to test
                  int   cindex = GetCompIndex(compname);

                  if (cindex > -1)
                  {
                     compArray[cindex]->testflag = TRUE;                   
                     compArray[cindex]->testnone = testnone;
                     compArray[cindex]->bottom = np->getInsert()->getPlacedBottom();
                  }
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_VIA ||
                     np->getInsert()->getInsertType() == INSERTTYPE_TESTPAD ||
                     np->isInsertType(insertTypeBondPad))  
            {
               // all test attributes are handles layer, when v->pos calls the get_attributes 
               // functions.
               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

               if (a)
               {
                  HPVias *v = new HPVias;
                  viaarray.SetAtGrow(viacnt,v);  
                  viacnt++;   
                  v->netname = get_attvalue_string(doc, a);
                  v->pos = pos1;
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // while
   return 1;
}

/*****************************************************************************/
/*
   here is also the function .TESTPREF handles.
   .TESTPREF HP_TESTPREF only adds the ATT_TEST attribute to  a component insert
*/
static int update_testpref_list(FileStruct *file)
{
   DataStruct *np;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT ||
                np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)   
               // only write out mirrored components.
            {
               CString compname = np->getInsert()->getRefname();
               CString  pref =  GetCompPrefix(compname);

               // now here loop throu components and look for matching prefix
               if (IsAttr(pref, HP_TESTPREF) > -1)
               {
                  int   cindex = GetCompIndex(compname);

                  if (cindex > -1)
                  {
                     compArray[cindex]->testflag = TRUE;
                     compArray[cindex]->bottom = np->getInsert()->getPlacedBottom();
                  }
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // while
   return 1;
}

/*****************************************************************************/
/*
*/
static int update_component_tooling_list(FileStruct *file)
{
   DataStruct *np;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);
      
		if (!np->isInsertType(insertTypePcbComponent) &&
			!np->isInsertType(insertTypeTestPoint))
			continue;

      switch(np->getDataType())
      {
         case T_INSERT:    // any insert can be a tooling.
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (block == NULL)   break;

            // compname converts NULL to ""
            CString compname = np->getInsert()->getRefname();

            if (strlen(compname))
            {
               CString compname = np->getInsert()->getRefname();
               CString  pref =  GetCompPrefix(compname);

               // now here loop throu tooling and look for matching prefix
               if (IsAttr(pref, HP_TOOLINGPREF) > -1)
               {
                  if (np->getInsert()->getInsertType() != INSERTTYPE_TOOLING)
                  {
                     np->getInsert()->setInsertType(insertTypeDrillTool); 
                     fprintf(logFp, "Component [%s] changed to TOOLING type\n",compname);
                     display_error++;
                  }
               }
            }

            // now here loop throu toolingshape and look for matching
            if (IsAttr(block->getName(), HP_TOOLINGSHAPE) > -1)
            {
               if (np->getInsert()->getInsertType() != INSERTTYPE_TOOLING)
               {
                  np->getInsert()->setInsertType(insertTypeDrillTool); 

                  if (strlen(compname))
                     fprintf(logFp, "Component [%s] changed to TOOLING type\n",compname);
                  else
                     fprintf(logFp, "ComponentShape [%s] changed to TOOLING type\n",
                        block->getName());

                  display_error++;
               }
            }

            if (strlen(compname))
            {
               int cindex = GetCompIndex(compname);
               
               if (cindex > -1)
               {
                  compArray[cindex]->inserttype = np->getInsert()->getInsertType();
                  compArray[cindex]->bottom = np->getInsert()->getPlacedBottom();
                  compArray[cindex]->pincnt = GetPinCount(&(block->getDataList()));
                  Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_SMDSHAPE,1);

                  if (a)
                     compArray[cindex]->smdflag = TRUE;
               }

               if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
                  UpdateCompTest(GetDeviceIndex(HPDEV_PINLIBRARY) ,compname, "", "");
            }
         } // case INSERT
         break;
      } // end switch
   } // while
   return 1;
}


typedef CTypedPtrArray<CPtrArray, Point2*> Point2Array;
/******************************************************************************
* write_outline_with_vectored_arcs
*/
static void write_outline_with_vectored_arcs(FILE *fBxy, FileStruct *file, CPntList *pntList, int bottom, const char *outname)
{
   if (!pntList)
      return;

   //
   // Convert poly with bulge to poly with vectored out arcs.
   //
   Point2Array polyArray;
   int polyCount = 0;

   int arcFound = 0;

   POSITION pntPos = pntList->GetHeadPosition();
   while (pntPos)
   {
      CPnt *pnt = pntList->GetNext(pntPos);

      // Process bulge on all but last point. Bulge always needs a next point and last point
      // does not have a next point. Ignore the bulge on a last point.
      bool hasBulge = (fabs(pnt->bulge) > BULGE_THRESHOLD);
      bool isLast = (pntPos == NULL);

      if (hasBulge && isLast)
      {
         // I like this better: ..
         fprintf(logFp, "Outline Curve Error. Exporter is disregarding curve from last point to nowhere in [%s].\n", outname);
      }

      if (hasBulge && !isLast)
      {
         double da = atan(pnt->bulge) * 4;

         CPnt *pnt2 = pntList->GetAt(pntPos);
                     
         double cx, cy, r, sa;
         ArcPoint2Angle(pnt->x, pnt->y, pnt2->x, pnt2->y, da, &cx, &cy, &r, &sa);

         // make positive start angle.
         while (sa < 0)
            sa += PI2;

         int ppolycnt = 255;
         Point2 ppoly[255];

         // start center
         ArcPoly2(pnt->x, pnt->y, cx, cy, da, 1.0, ppoly, &ppolycnt, DegToRad(ARC_ANGLE_DEGREE));
         arcFound++;

         for (int i=0; i<ppolycnt; i++)
         {
            Point2 *point = &ppoly[i];

            Point2 *newPoint = new Point2(*point);

            polyArray.SetAtGrow(polyCount, newPoint);
            polyCount++;
         }
      }
      else
      {
         Point2 *point = new Point2;
         point->x = pnt->x;
         point->y = pnt->y;
         point->bulge = 0;
         polyArray.SetAtGrow(polyCount, point);
         polyCount++;
      }
   }

   // Filter out points that are too close to bother with.
   for (int i=1; i<polyCount; i++)
   {
      Point2 *p1 = polyArray[i-1];
      Point2 *p2 = polyArray[i];

      if (fabs(p1->x - p2->x) < SMALL_DELTA && fabs(p1->y - p2->y) < SMALL_DELTA)
      {
         polyArray.RemoveAt(i);
         delete p2;
         i--;
         polyCount--;
      }
   }

   //
   // Prepare debug poly data if we are writing debug poly back to CCZ.
   //
   DataStruct *debugPolyData = NULL;
   if (SHOWOUTLINE)
   {
      doc->PrepareAddEntity(file);

      int layerNumber;

      if (bottom)
         layerNumber = Graph_Level("DEBUG_BOT","",0);
      else
         layerNumber = Graph_Level("DEBUG_TOP","",0);

      debugPolyData = Graph_PolyStruct(layerNumber, 0, FALSE);

      Graph_Poly(NULL, 0, 0, 0, 0);
   }

   //
   // Output the outline
   //
   if (polyCount > MAX_OUTLINE_PNT_COUNT)
   {
      // Too many points, write bounding box.
      double xmin, xmax, ymin, ymax;
      xmin = ymin = DBL_MAX;
      xmax = ymax = -DBL_MAX;

      for (int i=1; i<polyCount; i++)
      {
         Point2 *p = polyArray[i];
         
         if (p->x < xmin)
            xmin = p->x;

         if (p->x > xmax)
            xmax = p->x;

         if (p->y < ymin)
            ymin = p->y;

         if (p->y > ymax)
            ymax = p->y;
      }  

      fprintf(fBxy, "\n%ld, %ld", cnv_units(xmin), cnv_units(ymin));
      if (SHOWOUTLINE)         Graph_Vertex(xmin,            ymin, 0.);

      fprintf(fBxy, "\n%ld, %ld", cnv_units(xmax), cnv_units(ymin));
      if (SHOWOUTLINE)         Graph_Vertex(xmax,            ymin, 0.);

      fprintf(fBxy, "\n%ld, %ld", cnv_units(xmax), cnv_units(ymax));
      if (SHOWOUTLINE)         Graph_Vertex(xmax,            ymax, 0.);

      fprintf(fBxy, "\n%ld, %ld", cnv_units(xmin), cnv_units(ymax));
      if (SHOWOUTLINE)         Graph_Vertex(xmin,            ymax, 0.);

      fprintf(fBxy, "\n%ld, %ld", cnv_units(xmin), cnv_units(ymin));
      if (SHOWOUTLINE)         Graph_Vertex(xmin,            ymin, 0.);

      fprintf(logFp, "Outline [%s] has [%d] Vertex points -> approximated to the containing rectangle.\n", outname, polyCount);

      if (arcFound)
         fprintf(logFp, "Outline contains [%d] Arcs with a step angle of [%1.0lf] degree. Increase .ARCSTEPANGLE in .out file.\n",
               arcFound, ARC_ANGLE_DEGREE);

      display_error++;
   }
   else
   {
      // Write out the poly we ended up with.
      for (int i=0; i<polyCount; i++)
      {
         Point2 *p = polyArray[i];  
         fprintf(fBxy, "\n%ld, %ld", cnv_units(p->x), cnv_units(p->y));

         if (SHOWOUTLINE)
            Graph_Vertex(p->x, p->y, p->bulge);
      }
   }

   for (int i=0; i<polyCount; i++)
      delete polyArray[i];

   polyArray.RemoveAll();
   polyCount = 0;
}

/*****************************************************************************/
/*
*/
static int count_visible_pchfiles()
{
   int   cnt = 0;
   POSITION pos = doc->getFileList().GetHeadPosition();

   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);

      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
         cnt++;
   }

   return cnt;
}

// the devices are normalized devicetypes[] in stdafx.cpp
/******************************************************************************
* write_brd
*/
static void write_brd(CCEtoODBDoc* pDoc, FILE *fBrd, FileStruct *pcbFile, BOOL single_not_connected_net)
{
   if (Format->ExportVariant)
      writeVariantVersion(fBrd, pcbFile);

   DoComponentDevices(pDoc, fBrd, pcbFile);

   WritePinMapSection(fBrd, pcbFile);

   do_signals(fBrd, &pcbFile->getNetList(), single_not_connected_net);

   do_crossref(fBrd, pcbFile, &pcbFile->getNetList(), single_not_connected_net);
}

/******************************************************************************
* write_bxy
*/
static void write_bxy(bool useDFTAccessibility, FILE *fBxy, FileStruct *pcbFile, double accuracy, int IncludeVias, BOOL single_not_connected_net)
{
   // always must be first, it build the central component list
   // must be first before NC net, so that they do not
   // get into the NCnetlist.

   update_component_tooling_list(pcbFile);

   generate_PINLOC(doc, pcbFile, 0); // this function generates the PINLOC argument for all pins.
   generate_PADSTACKSMDATTRIBUTE(doc,false);  // generate SMD attributes for padstacks

   do_padstacks(unitsFactor);

   do_unnamednets(&pcbFile->getNetList());

   update_testpref_list(pcbFile); // this must be before component_list, because testpref_list // adds the ATT_TEST to the component 

   RefreshInheritedAttributes(doc, SA_OVERWRITE); // among others, move ATT_TEST to instance

   update_via_comp_list(pcbFile); // must be before component_list, because it adds the TEST att.
         
   UpdateCompDeviceList(pcbFile); // component_list, which flags the netlist to mark mandatory testpoints.

   if (useDFTAccessibility) //run those funtions if we are getting info from DFT only
	{
		CollectAccess(pcbFile);
		CollectProbes(pcbFile);
	}

   // board_xy outline
   {
      fprintf(fBxy, "OUTLINE");

      Outline_Start(doc, false, true);

      // first try to find primary board outline
      int outline_found = HP3070_WriteBOARDData(&(pcbFile->getBlock()->getDataList()),
            0.0, 0.0, 0.0, pcbFile->isMirrored(), pcbFile->getScale(), 0, -1, TRUE);

      if (!outline_found)
      {
         // now try to find any board outline geom 
         outline_found = HP3070_WriteBOARDData(&(pcbFile->getBlock()->getDataList()), 
               0.0, 0.0, 0.0, pcbFile->isMirrored(), pcbFile->getScale(), 0, -1, FALSE);
      }

      if (outline_found)
      {
         int returnCode;
         write_outline_with_vectored_arcs(fBxy, pcbFile, Outline_GetOutline(&returnCode, accuracy), 0, "BOARDOUTLINE");
      }
      else
      {
         fprintf(logFp,"Did not find a Board Outline.\n");
         display_error++;
      }

      Outline_FreeResults();
   }

   fprintf(fBxy, ";\n\n");

   // board_xy tooling
   // need to do tooling drill size.
   fprintf(fBxy, "TOOLING\n");
   HP3070_WriteToolingData(fBxy, &(pcbFile->getBlock()->getDataList()), 0.0, 0.0, 0.0, pcbFile->isMirrored(), pcbFile->getScale(), 0, -1);
   fprintf(fBxy, "\n");
   // board_xy keepout

   // board_xy node
   // this here is the netlist with all vialocs.
   DoAllVias(fBxy, pcbFile, &(pcbFile->getBlock()->getDataList()), 0.0, 0.0, 0.0, pcbFile->isMirrored(), pcbFile->getScale(), 0, -1, 
		&pcbFile->getNetList(), IncludeVias, useDFTAccessibility, single_not_connected_net);

	do_otherpinloc(fBxy, pcbFile, &pcbFile->getNetList(), 1.0, &(pcbFile->getBlock()->getDataList()), useDFTAccessibility);
   fprintf(fBxy, "\n\n");
   // board_xy devices
   fprintf(fBxy, "DEVICES\n");
   HP3070_WriteALLCOMPONENTData(fBxy, pcbFile, 0.0, 0.0, 0.0, pcbFile->isMirrored(), pcbFile->getScale(), 0, -1, accuracy);
   fprintf(fBxy, "\n");
}

/******************************************************************************
* HP3070_GatherOutlineData
*/
static int HP3070_GatherOutlineData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, GraphicClassTag graphic_class)
{
   int outline_found = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getDataType() == T_TEXT)
         continue;

      if (data->getDataType() != T_INSERT)
      {
         if (graphic_class && (data->getGraphicClass() != graphic_class))
            continue;

         int layer;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         Outline_Add(data, scale, insert_x, insert_y, rotation, mirror);
         outline_found++;
      }
      else
      {
         if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
            continue;

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;

         if (mirror)
            point2.x = -point2.x;

         point2.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot = rotation + data->getInsert()->getAngle();

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (data->getLayerIndex() != -1)
               block_layer = data->getLayerIndex();

            outline_found += HP3070_GatherOutlineData(&(block->getDataList()), point2.x, point2.y, block_rot, block_mirror,
                  scale * data->getInsert()->getScale(), embeddedLevel+1, block_layer, graphic_class);
         }
      }
   } 

   return outline_found;
}

/******************************************************************************
* HP3070_WriteALLCOMPONENTData
*/
void HP3070_WriteALLCOMPONENTData(FILE *fBxy, FileStruct *pcbFile, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, double accuracy)
{
   int viacnt = 0;

   POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      // insert if mirror is either global mirror or block_mirror, but not if both.
      int flipmirror = data->getInsert()->getMirrorFlags() & MIRROR_FLIP;
      int block_mirror = mirror ^ flipmirror;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT) 
      {
         CString compname = data->getInsert()->getRefname();

         fprintf(fBxy, " %s %s", check_name('c', compname, FALSE), data->getInsert()->getPlacedBottom() ? "bottom" : "top");

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int botcomp = 0;

         if (data->getInsert()->getPlacedBottom())
            botcomp = 1;

         int block_bottom = mirror ^ botcomp;
         double block_rot = rotation + data->getInsert()->getAngle();

         Mat2x2 m;
         RotMat2(&m, block_rot);

         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;

         if (mirror)
            point2.x = -point2.x;

         point2.y = data->getInsert()->getOriginY() * scale;

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (block->getName().CompareNoCase("monp_agd06030a0_C149") == 0) //*rcf debug
         {
            int jj = 0;
         }

         // Component outline 
         Outline_Start(doc, false, false);

			// Try and get the DFT package outline if it is present
         int outline_found = HP3070_GatherOutlineData(&(block->getDataList()), 
            point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(), 
            0, -1, graphicClassPackageOutline);

			// Get outline from component if not yet found
			if (!outline_found)
			{
				outline_found = HP3070_GatherOutlineData(&(block->getDataList()), 
					point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(), 
					0, -1, graphicClassComponentOutline);
			}

			/*
			/* Case 879:  Removed this code block as requested by Mark Laing and Chris Czernel
			/*	"nothing is better than something, if that something is not known to be good" - CC
			/*
			// Get outline from nornal if not yet found
         if (!outline_found)
         {
            outline_found = HP3070_GatherOutlineData(&(block->getDataList()), 
               point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(), 
               0, -1, 0);
         }
			*/

         if (!outline_found)
         {     
            fprintf(logFp,"Did not find a Component Outline for [%s].\n", block->getName());
            display_error++;
         }
         else if(exportDeviceOutline) 
         {
            int returnCode;
            write_outline_with_vectored_arcs(fBxy, pcbFile, Outline_GetOutline(&returnCode, accuracy), block_bottom, block->getName());
         }

         Outline_FreeResults();

         // end of outline stuff

         fprintf(fBxy, ";\n");
      }
      else if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)  
      {
         Point2   point2;
         Mat2x2   m;
         int botcomp = 0;

         if (data->getInsert()->getPlacedBottom())
            botcomp = 1;

         CString compname = data->getInsert()->getRefname();

         fprintf(fBxy," %s %s", check_name('c',compname, FALSE), (botcomp)?"bottom":"top");

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_bottom = mirror ^ botcomp;
         double block_rot = rotation + data->getInsert()->getAngle();

         RotMat2(&m, block_rot);

         point2.x = data->getInsert()->getOriginX() * scale;

         if (mirror)
            point2.x = -point2.x;

         point2.y = data->getInsert()->getOriginY() * scale;
         fprintf(fBxy, ";\n");
      }
   }
}

/******************************************************************************
* CompAlreadyDone
*  - component is either already written to test or is not supposed to be tested at all
*/
static BOOL CompAlreadyDone(CString compName)
{
   for (int i=0; i<compCount; i++)
   {
      HP3070Comp *comp = compArray[i];

      if (!compName.Compare(comp->name) && (comp->written_out || comp->testnone))
         return TRUE;
   }

   return FALSE;
}


/*****************************************************************************/
/*
*/
static char *clean_tol(const char *tol, const char *def)
{
   static CString t; // static only gets init once.

   t = tol;
   // HP3070 can not handle the% as tolerance
   if (t.Right(1) == '%')
   {
      CString w = t.Left(strlen(t)-1);
      t = w;
   }

   if (t.Left(1) == '-')
   {
      CString w = t.Right(strlen(t)-1);
      t = w;
   }

   if (t.Left(1) == '+')
   {
      CString w = t.Right(strlen(t)-1);
      t = w;
   }

   // here check for numbers
   int   i;
   int   num = TRUE;

   for (i=0;i<(int)strlen(t);i++)
   {
      if (isalpha(t[i]))
         num = FALSE;
   }

   if (!num)
   {
      fprintf(logFp,"Bad Tolerance value [%s] -> changed to [%s]\n",t, def);
      display_error++;
      t = def;
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static const char *clean_capvalue(CString c, const char *u)
{
   static   CString  t;

   t = c;

   // HP3070 can not handle the UF or pF
   if (c.Right(1) == 'F' || c.Right(1) == 'f')
   {
      t = c.Left(strlen(c)-1);
   }

   // this puts a default ending from 3070.out
   char e = c.GetAt(strlen(c)-1);

   if (!isalpha(e) && e != '*')
   {
      t += u;
   }

   // here check for numbers -just first because it can be .01u
   if (isalpha(t[0]))
   {
      fprintf(logFp,"Bad Capacitor value [%s] -> changed to **\n",t);
      display_error++;
      t = "**";
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static const char *clean_inductorvalue(CString c, const char *u)
{
   static   CString  t;

   t = c;

   // HP3070 can not handle the H or h
   if (c.Right(1) == 'H' || c.Right(1) == 'h')
   {
      t = c.Left(strlen(c)-1);
   }

   // this puts a default ending from 3070.out
   char e = c.GetAt(strlen(c)-1);

   if (!isalpha(e) && e != '*')
   {
      t += u;
   }

   // just test first, because it can be .100u
   if (isalpha(t[0]))
   {
      fprintf(logFp,"Bad Inductor value [%s] -> changed to **\n",t);
      display_error++;
      t = "**";
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static const char *clean_fusevalue(CString c, const char *u)
{
   static   CString  t;

   t = c;

   // HP3070 can not handle the A or a
   if (c.Right(1) == 'A' || c.Right(1) == 'a')
   {
      t = c.Left(strlen(c)-1);
   }

   // this puts a default ending from 3070.out
   char e = c.GetAt(strlen(c)-1);

   if (!isalpha(e) && e != '*')
   {
      t += u;
   }

   // just test first, because it can be .10A
   if (isalpha(t[0]))
   {
      fprintf(logFp,"Bad Fuse value [%s] -> changed to **\n",t);
      display_error++;
      t = "**";
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static const char *clean_zenervoltage(CString c, const char *u)
{
   static   CString  t;

   t = c;

   // HP3070 can not handle the V or v
   if (c.Right(1) == 'V' || c.Right(1) == 'v')
   {
      t = c.Left(strlen(c)-1);
   }

   return t.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static int get_hp_value(CCEtoODBDoc *doc,CAttributes* map, CString *val)
{
   Attrib *a;
   int    res = 0;
   CString l;

   l = "";

   if (a =  is_attvalue(doc, map,ATT_3070_VALUE,2))
   {
      l = get_attvalue_string(doc, a);
      res++;
   }
   else if (a =  is_attvalue(doc, map,"VALUE",2))
   {
      l = get_attvalue_string(doc, a);
      res++;
   }

   if (strlen(l))
      *val = l;

   return res;
}

/*****************************************************************************/
/*
*/
static int get_hp_type(CCEtoODBDoc *doc,CAttributes* map, CString *typ)
{
   Attrib *a;
   int    res = 0;

   if (a =  is_attvalue(doc, map,ATT_3070_TYPE,2))
   {
      *typ = get_attvalue_string(doc, a);
      res++;
   }
   else if (a =  is_attvalue(doc, map,"HP_TYPE",2)) // just for old reference
   {
      *typ = get_attvalue_string(doc, a);
      res++;
   }
   
   return res;
}

/*****************************************************************************/
/*
*/
static int get_tolerance(CCEtoODBDoc *doc,CAttributes* map, CString *plustol, CString *mintol)
{
   Attrib *a;
   int    res = 0;
   //*plustol = "";  -- loaded with defaults.
   //*mintol = "";

   if (a =  is_attvalue(doc, map,"TOL",2))
   {
      *plustol = clean_tol(get_attvalue_string(doc, a), *plustol);
      *mintol = clean_tol(get_attvalue_string(doc, a), *mintol);
      res++;
   }

   if (a =  is_attvalue(doc, map,ATT_TOLERANCE,2))
   {
      *plustol = clean_tol(get_attvalue_string(doc, a), *plustol);
      *mintol = clean_tol(get_attvalue_string(doc, a), *mintol);
      res++;
   }

   if (a =  is_attvalue(doc, map,"+TOL",2))
   {
      *plustol = clean_tol(get_attvalue_string(doc, a), *plustol);
      res++;
   }

   if (a =  is_attvalue(doc, map,"-TOL",2))
   {
      *mintol = clean_tol(get_attvalue_string(doc, a), *mintol);
      res++;
   }

   if (a =  is_attvalue(doc, map,ATT_3070_NTOL,2))
   {
      *mintol = clean_tol(get_attvalue_string(doc, a), *mintol);
      res++;
   }

   if (a =  is_attvalue(doc, map,ATT_3070_NTOL,2))
   {
      *mintol = clean_tol(get_attvalue_string(doc, a), *mintol);
      res++;
   }

   if (a =  is_attvalue(doc, map,ATT_3070_PTOL,2))
   {
      *plustol = clean_tol(get_attvalue_string(doc, a), *mintol);
      res++;
   }

   return res;
}

//******************************************************************************

static CString MakeFormattedPN(CString pnFormat, CAttributes *attribMap)
{
   CSupString upperFmt(pnFormat);
   upperFmt.MakeUpper();
   upperFmt.Trim();

   CStringArray fmtAttribNames;
   upperFmt.ParseQuote(fmtAttribNames, " ,");

   CString formattedPN;
   for (int i = 0; i < fmtAttribNames.GetCount(); i++)
   {
      CString attrNameI(fmtAttribNames.GetAt(i));
      Attrib *attrI = NULL;
      if (attrI = is_attvalue(doc, attribMap, attrNameI, 2))
      {
         CString attrValue = get_attvalue_string(doc, attrI);
         if (!attrValue.IsEmpty())
         {
            if (!formattedPN.IsEmpty())
               formattedPN += " ";

            formattedPN += attrValue;
         }
      }
   }

   return formattedPN;
}

/******************************************************************************
* AssignComponentDevice 
*/
static void AssignComponentDevice(CString compName, int hpDeviceIndex, DataStruct *compData, CVariantItem* variantItem, const CString variantVersion, CString userName)
{
   if (compData == NULL && variantItem == NULL)
      return;

   CAttributes** attribMap;
   if (variantItem != NULL)
   {
      // Check for VariantItem first, if there is then use attribute map from VariantItem
      attribMap = &variantItem->getAttributesRef();
   }
   else if (compData != NULL)
   {
     attribMap = &compData->getAttributesRef();
   }

   if (!(*attribMap))
      return;

   HPDevice *hpDevice = &hpDeviceArray[hpDeviceIndex];

   if (compData != NULL && variantItem == NULL)
   {
      // Only update attribute if there is no VariantItem
      UpdateComponentAttribute(attribMap, ATT_3070_DEVICECLASS, hpDevice->deviceName);      
   }

   CString testString;
   Attrib* attrib = NULL;

   switch (hpDeviceIndex)
   {
   case HPDEVICE_NODE_LIBRARY: // this are all devices, which do not fall into the other classes
      break;

   case HPDEVICE_CAPACITOR:
      {
         Capdefault cap = capdefault;

         get_hp_value(doc, *attribMap, &cap.value);
         get_tolerance(doc, *attribMap, &cap.plustol, &cap.mintol);
         get_hp_type(doc, *attribMap, &cap.typ);

         cap.value = clean_capvalue(cap.value, capdefault.defunit);

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_NTOL, cap.mintol);        
            UpdateComponentAttribute(attribMap, ATT_3070_PTOL, cap.plustol);       
            UpdateComponentAttribute(attribMap, ATT_3070_VALUE, cap.value);        
            UpdateComponentAttribute(attribMap, ATT_3070_TYPE, cap.typ);        
         }

         CString tmp;
         tmp.Format("%s %s %s %s", cap.value, cap.plustol, cap.mintol, cap.typ);
         testString += tmp;
      }
      break;

   case HPDEVICE_CONNECTOR: 
      {
         Conndefault conn = conndefault; // must be always NT (notest)

         // do not look for attribute "TEST" 
         testString += conn.test;
      }
      break;

   case HPDEVICE_DIODE:
      {
         Diodedefault diode = diodedefault;

         if (attrib = is_attvalue(doc, *attribMap, "HI-VAL",2))
            diode.hival = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, "LO-VAL",2))
            diode.lowval = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, ATT_3070_HI_VALUE,2))
            diode.hival = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, ATT_3070_LOW_VALUE,2))
            diode.lowval = get_attvalue_string(doc, attrib);

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_HI_VALUE, diode.hival);      
            UpdateComponentAttribute(attribMap, ATT_3070_LOW_VALUE, diode.lowval);       
         }

         CString tmp;
         tmp.Format("%s %s", diode.hival, diode.lowval);
         testString += tmp;
      }

      break;

   case HPDEVICE_FET:
      {
         Fetdefault fet = fetdefault;

         if (attrib = is_attvalue(doc, *attribMap, "HI-RES",2))
            fet.hires = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, "LO-RES",2))
            fet.lowres = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, ATT_3070_HI_VALUE,2))
            fet.hires = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, ATT_3070_LOW_VALUE,2))
            fet.lowres = get_attvalue_string(doc, attrib);

         get_hp_type(doc, *attribMap, &fet.typ);

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_HI_VALUE, fet.hires);        
            UpdateComponentAttribute(attribMap, ATT_3070_LOW_VALUE, fet.lowres);      
            UpdateComponentAttribute(attribMap, ATT_3070_TYPE, fet.typ);        
         }

         CString  tmp;
         tmp.Format("%s %s %s",fet.hires, fet.lowres, fet.typ);
         testString += tmp;
      }

      break;

   case HPDEVICE_JUMPER:
      {
         Jumperdefault jumper = jumperdefault;
         get_hp_type(doc, *attribMap, &jumper.typ);

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_TYPE, jumper.typ);        
         }

         testString += jumper.typ;
      }

      break;

   case HPDEVICE_FUSE:
      {
         Fusedefault fuse = fusedefault;

         if (attrib = is_attvalue(doc, *attribMap, "MAX-CURRENT",2))
            fuse.maxcurrent = get_attvalue_string(doc, attrib);
         else // else get value
            get_hp_value(doc, *attribMap, &fuse.maxcurrent);

         fuse.maxcurrent = clean_fusevalue(fuse.maxcurrent, fusedefault.defunit);

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_VALUE, fuse.maxcurrent);        
         }

         testString += fuse.maxcurrent;
      }

      break;

   case HPDEVICE_INDUCTOR:
      {
         Inductordefault inductor = inductordefault;

         get_hp_value(doc, *attribMap, &inductor.value);
         get_tolerance(doc, *attribMap, &inductor.plustol, &inductor.mintol);
         get_hp_type(doc, *attribMap, &inductor.typ);

         inductor.value = clean_inductorvalue(inductor.value,inductordefault.defunit);

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_PTOL, inductor.plustol);        
            UpdateComponentAttribute(attribMap, ATT_3070_NTOL, inductor.mintol);      
            UpdateComponentAttribute(attribMap, ATT_3070_VALUE, inductor.value);      
            UpdateComponentAttribute(attribMap, ATT_3070_TYPE, inductor.typ);      
         }

         CString tmp;
         tmp.Format("%s %s %s %s %s", inductor.value, inductor.plustol, inductor.mintol, inductor.series, inductor.typ);
         testString += tmp;
      }

      break;

   case HPDEVICE_POTENTIOMETER:
      {
         Potentiometerdefault potentiometer = potentiometerdefault;

         get_hp_value(doc, *attribMap, &potentiometer.value);
         get_tolerance(doc, *attribMap, &potentiometer.plustol, &potentiometer.mintol);

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_PTOL, potentiometer.plustol);      
            UpdateComponentAttribute(attribMap, ATT_3070_NTOL, potentiometer.mintol);       
            UpdateComponentAttribute(attribMap, ATT_3070_VALUE, potentiometer.value);       
         }

         CString tmp;
         tmp.Format("%s %s %s",potentiometer.value, potentiometer.plustol, potentiometer.mintol);
         testString += tmp;
      }

      break;

   case HPDEVICE_RESISTOR:
      {
         Resistordefault resistor = resistordefault;

         get_hp_value(doc, *attribMap, &resistor.value);
         get_tolerance(doc, *attribMap, &resistor.plustol, &resistor.mintol);
         get_hp_type(doc, *attribMap, &resistor.typ);

         // here now check if a resistor should be a jumper
         double resistorValue = -1; 
         resistorValue = normalize_resistorvalue(resistor.value); // returns -1 on no value or zerostring

         if (resistorValue == -1)   // illegal value
            resistor.value = "**";

         if (resistorValue > -1 && resistorValue < resistorminimum)
         {
            fprintf(logFp, "Component [%s] forced from RESISTOR [%s] into CLOSED JUMPER because of RESISTOR MINIMUM\n",
                  compName, resistor.value);

            hpDeviceIndex = HPDEVICE_JUMPER;

            testString += "Closed";
         }
         else if (resistorValue > resistormaximum)
         {
            fprintf(logFp, "Component [%s] forced from RESISTOR [%s] into OPEN JUMPER because of RESISTOR MAXIMUM\n",
                  compName, resistor.value);

            hpDeviceIndex = HPDEVICE_JUMPER;

            testString += "Open";
         }
         else
         {
            if (compData != NULL && variantItem == NULL)
            {
               // Only update attribute if there is no VariantItem
               UpdateComponentAttribute(attribMap, ATT_3070_PTOL, resistor.plustol);        
               UpdateComponentAttribute(attribMap, ATT_3070_NTOL, resistor.mintol);      
               UpdateComponentAttribute(attribMap, ATT_3070_VALUE, resistor.value);      
               UpdateComponentAttribute(attribMap, ATT_3070_TYPE, resistor.typ);      
            }

            CString tmp;
				CString val = "";
				if (resistor.value.Find("Ohm",0) != -1)
				{
					val.Format("%f", resistorValue);
					val.TrimRight("0");
					if (!val.Right(1).CompareNoCase("."))
						val += "0";
				}
				else
					val = resistor.value;

				tmp.Format("%s %s %s %s", val, resistor.plustol, resistor.mintol, resistor.typ);
            testString += tmp;
         }
      }

      break;

   case HPDEVICE_SWITCH:
      {
         // default because a CAD system does not know 
         // common pins or ON/OFF position.
         testString += " 1 CLOSED";
      }

      break;

   case HPDEVICE_TRANSISTOR:
      {
         Transistordefault transistor = transistordefault;

         if (attrib = is_attvalue(doc, *attribMap, "HI-BETA", 2))
            transistor.hibeta = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, "LO-BETA", 2))
            transistor.lowbeta = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, ATT_3070_LOW_VALUE, 2))
            transistor.lowbeta = get_attvalue_string(doc, attrib);

         if (attrib = is_attvalue(doc, *attribMap, ATT_3070_HI_VALUE, 2))
            transistor.hibeta = get_attvalue_string(doc, attrib);
         
         get_hp_type(doc, *attribMap, &transistor.typ);

         CString tmp;
         tmp.Format("%s %s %s", transistor.hibeta, transistor.lowbeta, transistor.typ);
         testString += tmp;
      }

      break;

   case HPDEVICE_ZENER:
      {
         Zenerdefault zener = zenerdefault;

         // on zener, voltage and value are nearly the same. If no voltage is found, check if there is a value.
         if (attrib = is_attvalue(doc, *attribMap, "VOLTAGE", 2))
            zener.voltage = get_attvalue_string(doc, attrib);
         else
            get_hp_value(doc, *attribMap, &zener.voltage);
      
         get_tolerance(doc, *attribMap, &zener.plustol, &zener.mintol);

         zener.voltage = clean_zenervoltage(zener.voltage, "");

         if (compData != NULL && variantItem == NULL)
         {
            // Only update attribute if there is no VariantItem
            UpdateComponentAttribute(attribMap, ATT_3070_VALUE, zener.voltage);       
            UpdateComponentAttribute(attribMap, ATT_3070_PTOL, zener.plustol);        
            UpdateComponentAttribute(attribMap, ATT_3070_NTOL, zener.mintol);      
         }

         CString tmp;
         tmp.Format("%s %s %s", zener.voltage, zener.plustol, zener.mintol);
         testString += tmp;
      }

      break;

   case HPDEVICE_PIN_LIBRARY:
      {
         int i=0;
      }

      break;

   case HPDEVICE_UNDEFINED:
      break;

   default:
      fprintf(logFp, "Unknown Devicetype [%s] in 3070.out\n", hpDevice->deviceName);
      display_error++;
      break;
   }

   if (userName.GetLength())
   {
      if (testString.GetLength())
         testString += " ";
      testString += "NT";
   }

   CString failureMessage;

   if (userName.GetLength())
      failureMessage = userName;

   if (!failureMessage.IsEmpty())
      failureMessage += " ";

   if (compData != NULL)
   {
      if (variantItem != NULL && compData->getDataType() == dataTypeInsert)
      {
         failureMessage += GetUserDefMsg()->format(*doc, compData->getInsert(), variantItem->getAttributes());
      }
      else
      {
         failureMessage += GetUserDefMsg()->format(doc, compData);
      }
   }

	if (!SUPPRESS_PARTNUMBERandMESSAGE || hpDeviceIndex == HPDEVICE_PIN_LIBRARY)
	{
      CString formattedPN;

      if (hpDeviceIndex == HPDEVICE_PIN_LIBRARY && !pinlibPNFormat.IsEmpty())
      {
         formattedPN = MakeFormattedPN(pinlibPNFormat, *attribMap);
      }
      else if (!normalPNFormat.IsEmpty())
      {
         formattedPN = MakeFormattedPN(normalPNFormat, *attribMap);
      }

      if (formattedPN.IsEmpty())
      {
         if (attrib = is_attvalue(doc, *attribMap, ATT_PARTNUMBER, 2))
            formattedPN = get_attvalue_string(doc, attrib);
      }

      if (!formattedPN.IsEmpty())
      {
         formattedPN = check_name('b', formattedPN, FALSE);

         if (!testString.IsEmpty())
            testString += " ";

         testString += "PN\"";
         testString += formattedPN;
         testString += "\"";
      }

		if (failureMessage.GetLength())
		{
			testString += " \"";
			testString += failureMessage.Left(40);
			testString += "\"";
		}
      else if (ShowPartNumber == true && !formattedPN.IsEmpty())
		{
			testString += " \"";
			testString += formattedPN;
			testString += "\"";
		}
	}

   if ((attrib = is_attvalue(doc, *attribMap, ATT_TEST_STRATEGY, keyWordSectionCC)) != NULL)
   {
      if (attrib->getStringValue().Find(ATT_VALUE_TEST_ICT) < 0)
         testString += " NT";
   }

   if (variantItem != NULL && (attrib = is_attvalue(doc, *attribMap, ATT_LOADED, keyWordSectionCC)) != NULL)
   {
      CString loadedValue = attrib->getStringValue();
      loadedValue.MakeUpper();
      if (loadedValue != "TRUE")
          testString += " NP";
   }

   UpdateCompTest(hpDeviceIndex, compName, testString, variantVersion);
}

static void writeVariantVersion(FILE *fBrd, FileStruct *pcbFile)
{
   if (fBrd == NULL || pcbFile == NULL || pcbFile->getVariantList().GetCount() == 0)
      return;

   fprintf(fBrd, "VERSIONS\n");

   int versionCount = 0;

   // Set default Variant
   defaultVariant = pcbFile->getVariantList().GetDefaultVariant();
   if (defaultVariant == NULL)
   {
      // If there is no default Variant, then use the first Variant as default
      defaultVariant = pcbFile->getVariantList().GetHead();
   }

   for (POSITION pos=pcbFile->getVariantList().GetHeadPosition(); pos!=NULL;)
   {
      CVariant* variant = pcbFile->getVariantList().GetNext(pos);
      if (variant != NULL && variant->GetName() != defaultVariant->GetName())
      {
         fprintf(fBrd, "   version_%d\n", ++versionCount);
      }
   }

   fprintf(fBrd, "\n");
}

/******************************************************************************
* WriteComponentDevices
*/
void WriteComponentDevices(CCEtoODBDoc* pDoc, FILE *fBrd, FileStruct *pcbFile, CString deviceName, int maxAllowedPinCount)
{
   int hpDeviceIndex = GetDeviceIndex(deviceName);
	WORD testStrategyKey = pDoc->RegisterKeyWord(ATT_TEST_STRATEGY, 0, VT_STRING);

   POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);

      if (data->isInsertType(insertTypePcbComponent))
      {
         CString refname = data->getInsert()->getRefname();
         int compIndex = GetCompIndex(refname);

         if (compIndex >= 0 && !CompAlreadyDone(refname))
         {
            HP3070CompTest *compTest = NULL;
            if (compTestMap.Lookup(refname, compTest)&& compTest->getHpDeviceIndex() == hpDeviceIndex)
            {
               compTest->writeTestString(fBrd);
               compArray[compIndex]->written_out = TRUE;
            }
         }
      }
   } 
}

/******************************************************************************
* WriteUnusedComponents
*/
void WriteUnusedComponents(FILE *fBrd, CDataList *DataList)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      CString compName = data->getInsert()->getRefname();

      if (CompAlreadyDone(compName))
         continue;

      fprintf(fBrd, "   %s NT;\n", check_name('c', compName, FALSE));
   }
}

//--------------------------------------------------------------
void HP3070_WriteToolingData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   fidcnt = 0;

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

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {
// need to get real padstack drill size;
               const char  *padname = GetPadBlockName(&(block->getDataList()));
               fprintf(wfp,"%ld %ld, %ld;\n",2000, cnv_units(point2.x), cnv_units(point2.y));
               break;
            }
         } // case INSERT

         break;
      } // end switch
   } // end HP3070_WriteToolingData */
}

/******************************************************************************
* DoAllVias
*/
void DoAllVias(FILE *fBxy, FileStruct *pcbFile, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
      double scale, int embeddedLevel, int insertLayer, CNetList *NetList, BOOL IncludeVias, bool useDFTAccessibility, BOOL single_not_connected_net)
{
   Mat2x2 m;
   RotMat2(&m, rotation);
   
   UnConnectViaArray uncViaList;
   CString uncRemoveStr ="";
   bool hasSingleUnConnectNet = false;

   WORD noProbeKW = doc->RegisterKeyWord(ATT_NOPROBE, 0, valueTypeString);

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = NetList->GetNext(netPos);
      bool unConnectNetFlag = (net->getFlags() & NETFLAG_UNUSEDNET || net->getFlags() & NETFLAG_SINGLEPINNET);

      if(unConnectNetFlag && !IncludeUnconnectedPins)
         continue;

      BOOL First = TRUE;

      for (int i=0; i<viacnt; i++)
      {
         if (strcmp(viaarray[i]->netname, net->getNetName()))
            continue; 

         DataStruct *data = DataList->GetNext(viaarray[i]->pos);
      
         if (data->getDataType() != T_INSERT)
            continue;

         if (data->getInsert()->getInsertType() != INSERTTYPE_VIA && data->getInsert()->getInsertType() != INSERTTYPE_TESTPAD
            && !data->isInsertType(insertTypeBondPad))
            continue;

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;

         if (mirror)
            point2.x = -point2.x;

         point2.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         
         //get padstack geometry in bondpad
         if(data->isInsertType(insertTypeBondPad))
            block = GetBondPadPadstackBlock(doc->getCamCadData(), block);

         Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 0);

         if (!attrib)   
            continue;

         // now check is netname matches
         CString netname = get_attvalue_string(doc, attrib);

         if (netname == net->getNetName())        
         {
            if (First)
            {
               // Case dts0100376598, output "REMOVE_PROBE for nets with NoProbe set in net conditions
               // when option is turned on in .out file.
               Attrib *attrib = NULL;
               CString RemovedStr; // blank  is default value
               if (NoProbeToRemoved && net->lookUpAttrib(noProbeKW, attrib))
               {
                  if (((CString)get_attvalue_string(doc, attrib)).CompareNoCase("True") == 0)
                     RemovedStr = " REMOVE_PROBE"; // include space in value for field separator
               }

               if(unConnectNetFlag && single_not_connected_net)
               {
                  uncRemoveStr = RemovedStr;
                  hasSingleUnConnectNet = true;
               }
               else
               {
                  fprintf(fBxy, "NODE   \"%s\"%s\n", check_name('n', net->getNetName(), FALSE), RemovedStr);
                  fprintf(fBxy, "   ALTERNATES\n");
               }
            }

            First = FALSE;
            BOOL TestVia = FALSE;

            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 2))   
               TestVia = TRUE;

            // if not include vias and not a test via, skip it.

            if (!IncludeVias && !TestVia) // if include vias not checked, only do test vias.
               continue;
            
            CString probeAccess = HP_NORMAL_ACCESS;
            CString probeSide = "";
            int padstackIndex = get_padstackindex(block->getName());

            int testlayer = UpdateFeatureAccess(data, TestVia, 3, padstackIndex, 
                  0.0, scale, &point2.x, &point2.y, &probeAccess, &probeSide, useDFTAccessibility);

            //if (padstackIndex >= 0)
            //{
            //   int testlayer = UpdateFeatureAccess(data, TestVia, 3, padstackIndex, 
            //         0.0, &point2.x, &point2.y, &probeAccess, &probeSide, useDFTAccessibility);

            //   block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            //   double viaSize = GetViaSize(block, scale);

            //   // Case 2195 - Do not apply the 3070.out accesibility rule for min
            //   // via size if we are using DFT accessibility
            //   if (!useDFTAccessibility && viaSize < minviadia)
            //   {
            //      probeAccess = "NO_PROBE";
            //      probeSide = "";
            //   }
            //}
            //else
            //{
            //   probeAccess = "NO_PROBE";
            //   probeSide = "";
            //}

            long x = cnv_units(point2.x);
            long y = cnv_units(point2.y);
               
            long originalx = x;
            long originaly = y;
               
            if (check_double_placement(&x, &y, 3)) // all layers
            {
               fprintf(logFp, "Double Test coordinate found at (%ld, %ld) shifted to (%ld, %ld) (Via)\n",
                  originalx, originaly, x, y);
               display_error++;
            }

				//// case 1748
				//// Applies only when in "use 3070.out rules" mode is active
				//int testSide = ATTR_TESTSIDE_NOTPRESENT;

				//if (useTestAttribForProbeSide)
				//{
				//	Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 0);

				//	if (attrib)   
				//	{
				//		CString val = get_attvalue_string(doc, attrib);

				//		if (!val.CompareNoCase("TOP")) 
				//				testSide = ATTR_TESTSIDE_TOP;

				//		if (!val.CompareNoCase("BOTTOM")) 
				//				testSide =  ATTR_TESTSIDE_BOT;
				//	}
				//}
				//// end case 1748

				//if ( testSide  != ATTR_TESTSIDE_NOTPRESENT)
				//{
				//	if (testSide == ATTR_TESTSIDE_TOP)
				//		probeSide = "TOP";
				//	else if (testSide == ATTR_TESTSIDE_BOT)
				//		probeSide = "NO_MANUAL";
				//	else
				//		probeSide = "BOTH";
				//	// only update the attribute if test side is flag is present
				//	UpdateComponentAttribute(&data->getAttributesRef(), ATT_3070_PROBESIDE  , probeSide);      
				//	UpdateComponentAttribute(&data->getAttributesRef(), ATT_3070_PROBEACCESS, probeAccess); 
				//}

            //With Unconnected Net and Netname = "NC"
            if(hasSingleUnConnectNet)
            {
               HP3070UnConnectVia *uncVia = new HP3070UnConnectVia;
               uncVia->posX = x;
               uncVia->posY = y;
               uncVia->IsProbeAccessEmpty = probeAccess.IsEmpty();
               uncVia->probeAccess = probeAccess;
               uncVia->probeSide = probeSide;
               uncViaList.Add(uncVia);
            }
            else
            {
               fprintf(fBxy, "      %ld, %ld %s %s %s;\n", x, y, (probeAccess.IsEmpty() ? "" : " "), probeAccess, probeSide);
            }             
         }
      }

      if (!First && !hasSingleUnConnectNet)
         fprintf(fBxy, "\n");
   }

   // Include unConnect Pins with unConnect Nets
   if(hasSingleUnConnectNet)
   {
      fprintf(fBxy, "NODE   \"%s\"%s\n", "NC", uncRemoveStr);
      fprintf(fBxy, "   ALTERNATES\n");
      for(int idx = 0; idx < uncViaList.GetCount(); idx++)
      {
         HP3070UnConnectVia *uncVia = uncViaList.GetAt(idx);
         if(uncVia)
            fprintf(fBxy, "      %ld, %ld %s %s %s;\n", uncVia->posX, uncVia->posY, (uncVia->IsProbeAccessEmpty ? "" : " "), 
               uncVia->probeAccess, uncVia->probeSide);
      }

      uncViaList.empty();
      fprintf(fBxy, "\n");
   }
}

/******************************************************************************
* HP3070_WriteBOARDData
*/
int HP3070_WriteBOARDData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary)
{
   int found = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getDataType() == T_TEXT)
         continue;

      if (data->getDataType() != T_INSERT)
      {
         int layer;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if (data->getGraphicClass() == GR_CLASS_ETCH) 
            continue;   

         if (primary)
         {
            if (data->getGraphicClass() != GR_CLASS_BOARDOUTLINE)  
               continue;
         }
         else
         {
            if (data->getGraphicClass() != GR_CLASS_BOARD_GEOM) 
               continue;
         }

         Outline_Add(data, scale, insert_x, insert_y, rotation, mirror);

         found++;
      }
      else
      {
         if (data->getInsert()->getInsertType() != 0)  // do not do any inside hierachies, only 
            continue;                     // flat graphics.

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
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

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (data->getLayerIndex() != -1)
               block_layer = data->getLayerIndex();

            found += HP3070_WriteBOARDData(&(block->getDataList()), 
                  point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(),
                  embeddedLevel+1, block_layer, primary);
         } // end else not aperture
      } // if INSERT
   } // end HP3070_WriteBOARDData */

   return found;
}

//--------------------------------------------------------------
int HP3070_WritePANELData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary)
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

      if (np->getDataType() == T_TEXT)    continue;
      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if (np->getGraphicClass() == GR_CLASS_ETCH)   
            continue;   

         if (primary)
         {
            if (np->getGraphicClass() != GR_CLASS_PANELOUTLINE) 
               continue;
         }

         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);

         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() != 0) // do not do any inside hierachies, only 
            continue;                     // flat graphics.

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
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            found += HP3070_WritePANELData(&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, primary);
         } // end else not aperture
      } // if INSERT

   } // end HP3070_WritePANELData */

   return found;
}

/******************************************************************************
* in_boardarray
*  - Check if a board is already part of a panel
*/
static BOOL in_boardarray(int blockNumber, int mirror)
{
   for (int i=0; i<uniqueBoardCount; i++)
   {
      if (uniqueBoardArray[i].blockNumber == blockNumber && uniqueBoardArray[i].mirror == mirror)
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* do_boardarray 
*/
static void do_boardarray(FileStruct *panelFile)
{
   double xmin, ymin, xmax, ymax;
   xmin = ymin = FLT_MAX;
   xmax = ymax = -FLT_MAX;

   pcbInstanceCount = 0;

   POSITION dataPos = panelFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = panelFile->getBlock()->getDataList().GetNext(dataPos);

      // found Panel Outline
      if (data->getGraphicClass() == GR_CLASS_PANELOUTLINE && data->getDataType() == T_POLY)
      {
         // get extents
         CPoly *poly = data->getPolyList()->GetHead();
         POSITION pntPos = poly->getPntList().GetHeadPosition();

         while (pntPos)
         {
            CPnt* pnt = poly->getPntList().GetNext(pntPos);

            if (pnt->x < xmin)
               xmin = pnt->x;

            if (pnt->x > xmax)
               xmax = pnt->x;

            if (pnt->y < ymin)
               ymin = pnt->y;

            if (pnt->y > ymax)
               ymax = pnt->y;
         }
      }
      else
      {
         if (data->getDataType() != T_INSERT)         
            continue;

         if (data->getInsert()->getInsertType() != INSERTTYPE_PCB)
            continue;

         if (pcbInstanceCount < MAX_BOARDS)
         {
            pcbInstanceArray[pcbInstanceCount].refName = data->getInsert()->getRefname();              
            pcbInstanceArray[pcbInstanceCount].blockNum = data->getInsert()->getBlockNumber();
            pcbInstanceArray[pcbInstanceCount].x = data->getInsert()->getOriginX();
            pcbInstanceArray[pcbInstanceCount].y = data->getInsert()->getOriginY();             
            pcbInstanceArray[pcbInstanceCount].rotation = round(RadToDeg(data->getInsert()->getAngle()));             
            pcbInstanceArray[pcbInstanceCount].mirror = data->getInsert()->getMirrorFlags();
            pcbInstanceCount++;
         }
         else
         {
            fprintf(logFp, "Too many boards on a panel\n");
            display_error++;
         }

         if (in_boardarray(data->getInsert()->getBlockNumber(), data->getInsert()->getMirrorFlags()))
            continue;

         if (uniqueBoardCount < MAX_BOARDS)
         {
            uniqueBoardArray[uniqueBoardCount].blockNumber = data->getInsert()->getBlockNumber();
            uniqueBoardArray[uniqueBoardCount].boardName = data->getInsert()->getRefname();
            uniqueBoardArray[uniqueBoardCount].mirror = data->getInsert()->getMirrorFlags();
            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
            uniqueBoardArray[uniqueBoardCount].block = block;  
            uniqueBoardCount++;
         }
         else
         {
            fprintf(logFp, "Too many boards in a panel\n");
            display_error++;
         }
      }
   }

   if (xmin > xmax)
      ErrorMessage("No PANEL Outline found !");
}

/******************************************************************************
* WritePinMapSection
*/
static void WritePinMapSection(FILE *fBrd, FileStruct *pcbFile)
{
   // DR 809121 - Pin Mapping - This is not looking at pin mapping from DD at all.

   // Add pin mapping from comppins to pinmap table.
   // The first shot at this looped on comppins in nets. That worked poorly because the pins for a given
   // component are spread all over the nets, causing initial mapping to be based on mix of pins from mix
   // of components. In order to keep the mapping consistent, loop on components and find all the comppins
   // for component in hand.

   // Could use some optimization here to keep from processing same block more than once.

   POSITION dataPos = pcbFile->getBlock()->getHeadDataInsertPosition();
   while (dataPos != NULL)
   {
      DataStruct *compInsertData = pcbFile->getBlock()->getNextDataInsert(dataPos);
      if (compInsertData != NULL && compInsertData->isInsertType(insertTypePcbComponent))
      {
         BlockStruct *compBlock = doc->getBlockAt(compInsertData->getInsert()->getBlockNumber());
         if (compBlock != NULL)
         {
            CString compRefname(compInsertData->getInsert()->getRefname());

            Attrib *attrib = is_attvalue(doc, compInsertData->getAttributesRef(), ATT_DEVICETYPE, 2);
            if (attrib != NULL)
            {
               CString deviceName = get_attvalue_string(doc, attrib);
               DeviceTypeTag devTypeTag = stringToDeviceTypeTag(deviceName);

               // Default mapping of ccz type to hp type
               CString hpDeviceTypeName( GetHpDeviceName(devTypeTag) );

               // Default mapping may be overridden, check.
               HP3070CompTest *hpCompTest = NULL;
               if (compTestMap.Lookup(compRefname, hpCompTest))
               {
                  HPDevice *hpDevice = &hpDeviceArray[hpCompTest->getHpDeviceIndex()];
                  hpDeviceTypeName = hpDevice->deviceName;
               }

               int hpDeviceTypeIndex = GetDeviceIndex(hpDeviceTypeName);
               if (hpDeviceTypeIndex > -1 && hpDeviceTypeIndex != HPDEVICE_PIN_LIBRARY)
               {
                  POSITION pinPos = compBlock->getHeadDataInsertPosition();
                  while (pinPos != NULL)
                  {
                     DataStruct *pinData = compBlock->getNextDataInsert(pinPos);
                     if (pinData != NULL && pinData->isInsertType(insertTypePin))
                     {
                        CString pinName(pinData->getInsert()->getRefname());

                        CompPinStruct *cp = pcbFile->getNetList().getCompPin(compRefname, pinName);

                        if (cp != NULL)
                        {
                           Attrib *pinmapAttrib = is_attvalue(doc, cp->getAttributesRef(), ATT_DEVICETOPACKAGEPINMAP, 2);
                           if (pinmapAttrib != NULL)
                           {
                              CString mappedPinFunction( pinmapAttrib->getStringValue() );
                              CString hpPinName( getHpDevicePinName(hpDeviceTypeIndex, mappedPinFunction) );
                              if (hpPinName.CompareNoCase(pinName) != 0) // Don't need to map if both are already the same.
                              {
                                 PinMapEntry *pinMapEntry = pinMapArray.FindPinMap(hpDeviceTypeIndex, pinName);
                                 if (pinMapEntry != NULL)
                                 {
                                    // Entry for this pin name exists already, okay if compatible, error if not.
                                    if (pinMapEntry->m_newPin.CompareNoCase(hpPinName) != 0)
                                    {
                                       // Mismatch, can't map same ccz pin name to more than one HP pin name.
                                       fprintf(logFp, "PIN MAP ERROR for device type [%s], component [%s], pin [%s] cannot be mapped to [%s] because it is already mapped to [%s]\n", hpDeviceTypeName, compRefname, pinName, hpPinName, pinMapEntry->m_newPin);
                                    }
                                 }
                                 else
                                 {
                                    // Not present, try to add it.
                                    if (!pinMapArray.AddPinMap(hpDeviceTypeIndex, pinName, hpPinName))
                                    {
                                       // Failed to add new pin mapping.
                                       fprintf(logFp, "PIN MAP ERROR for device type [%s], component [%s], pin [%s] could not  be mapped to [%s]\n", hpDeviceTypeName, compRefname, pinName, hpPinName);
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }

   // here loop through the HP devices
   int maxHPDeviceArray = sizeof(hpDeviceArray) / sizeof(HPDevice);

   fprintf(fBrd, "\nPIN_MAP\n");

   for (int i=0; i<maxHPDeviceArray; i++) // do not do the last one, because PIN LIBRARY
                                          // will collect all components not done into the pin library
   {
      HPDevice *hpDevice = &hpDeviceArray[i];
      
      // loop thru the device list
      int First = TRUE;

      for (int ii=0; ii<pinMapArray.GetCount(); ii++)
      {
         PinMapEntry *pinMapEntry = pinMapArray.GetAt(ii);
         if (pinMapEntry->m_hpDeviceIndex == i)
         {
            if (First)
               fprintf(fBrd, "\n%s", hpDevice->deviceName); // start of device group

            First = FALSE;
            fprintf(fBrd, "\n     %s %s; ", pinMapEntry->m_oldPin, pinMapEntry->m_newPin);
         }
         
      }
   }

   fprintf(fBrd, "\nEND\n\n");
}

/******************************************************************************
* DoComponentDevices
*/
static void DoComponentDevices(CCEtoODBDoc* pDoc, FILE *fBrd, FileStruct *pcbFile)
{
   // assign
   AssignComponentDevices(pcbFile);

   // write
   int maxHPDeviceArray = sizeof(hpDeviceArray) / sizeof(HPDevice);

   for (int i=0; i<maxHPDeviceArray-1; i++)
   {
      HPDevice *hpDevice = &hpDeviceArray[i];

      fprintf(fBrd, "%s\n", hpDevice->deviceName);
      WriteComponentDevices(pDoc, fBrd, pcbFile, hpDevice->deviceName, hpDevice->maxPinCount);
      fprintf(fBrd, "\n");
   }

   fprintf(fBrd, "%s\n", hpDeviceArray[HPDEVICE_UNDEFINED].deviceName);
   WriteUnusedComponents(fBrd, &pcbFile->getBlock()->getDataList());
   fprintf(fBrd, "\n\n");
}

/******************************************************************************
* CollectAccess
*/
static void CollectAccess(FileStruct *pcbFile)
{
   Mat2x2 m;
   RotMat2(&m, pcbFile->getRotation());

   POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *accessPoint = pcbFile->getBlock()->getDataList().GetNext(dataPos);
      
      if (accessPoint->getDataType() != T_INSERT)
         continue;

      if (accessPoint->getInsert()->getInsertType() != INSERTTYPE_TEST_ACCESSPOINT)
         continue;

      long dataEntityNum = -1;
      Attrib *attrib = is_attvalue(doc, accessPoint->getAttributesRef(), ATT_DDLINK, 1); // this links to an data entity

      if (attrib)
         dataEntityNum = atol(get_attvalue_string(doc, attrib));

      int accessProbeIndex = UpdateAccessAndProbe(accessPoint->getEntityNumber(), dataEntityNum, 0);
      HPAccessProbe *accessProbe = accessProbeArray[accessProbeIndex];

      Point2 point2;
      point2.x = accessPoint->getInsert()->getOriginX() * pcbFile->getScale();

      if (pcbFile->isMirrored())
         point2.x = -point2.x;

      point2.y = accessPoint->getInsert()->getOriginY() * pcbFile->getScale();
      TransPoint2(&point2, 1, &m, pcbFile->getInsertX(), pcbFile->getInsertY());

      accessProbe->accessX = point2.x;
      accessProbe->accessY = point2.y;
   }
}

/******************************************************************************
* CollectProbes
*/
static void CollectProbes(FileStruct *pcbFile)
{
   Mat2x2 m;
   RotMat2(&m, pcbFile->getRotation());

   POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *testProbe = pcbFile->getBlock()->getDataList().GetNext(dataPos);
      
      if (testProbe->getDataType() != T_INSERT)
         continue;

      if (testProbe->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)
         continue;

      long accessEntityNum = -1;
      Attrib *attrib = is_attvalue(doc, testProbe->getAttributesRef(), ATT_DDLINK, 1); // this links to an access entity

      if (attrib)
         accessEntityNum = atol(get_attvalue_string(doc, attrib));

      int accessProbeIndex = UpdateAccessAndProbe(accessEntityNum, 0, testProbe->getEntityNumber());

      HPAccessProbe *accessProbe = accessProbeArray[accessProbeIndex];

      Point2 point2;
      point2.x = testProbe->getInsert()->getOriginX() * pcbFile->getScale();

      if (pcbFile->isMirrored())
         point2.x = -point2.x;

      point2.y = testProbe->getInsert()->getOriginY() * pcbFile->getScale();
      TransPoint2(&point2, 1, &m, pcbFile->getInsertX(), pcbFile->getInsertY());

      accessProbe->probeX = point2.x;
      accessProbe->probeY = point2.y;
   }
}

/******************************************************************************
* AssignComponentDevices
// sequence is 
// 1. look at .Comptest 100% name and userdef teststring
// 2. look at .CompNameType 100% match
// 3. look at .Devicetype : This is done upfront, so that the component 
// 4. test at .KEYVAL
// 5. look at .CompPreftype
*/
void AssignComponentDevices(FileStruct *pcbFile)
{
   POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && data->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)
         continue;

      CString compName = data->getInsert()->getRefname();

      if (CompAlreadyDone(compName))
         continue;

      // here test for pincnt;
      // if more than maxpincnt than make the comp a pin library
      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      block->getPinCount();
      int pinCount = GetPinCount(&block->getDataList());

      if (!Format->ExportVariant)
      {
         // 1. look at .CompTest 100% name and userdef teststring
         if (!AssignByCompTestArray(compName, pinCount))
         {
            AssignByAttrArray_CompName(compName, pinCount, data, NULL, "");
         }
      }
      else if (defaultVariant != NULL)
      {
         // Now do the AssignBy using attributes of component from Variant
         
         // First export the default Variant       
         CVariantItem* variantItem = defaultVariant->FindItem(compName);
         if (variantItem != NULL)
         {
            AssignByAttrArray_CompName(compName, pinCount, data, variantItem, "");
         }

         // Now export the rest of the variant and skip the default one
         int versionCount = 0;
         for (POSITION pos=pcbFile->getVariantList().GetHeadPosition(); pos!=NULL;)
         {
            CVariant* variant = pcbFile->getVariantList().GetNext(pos);
            if (variant != NULL && variant->GetName() != defaultVariant->GetName())
            {
               CString variantVersion;
               variantVersion.AppendFormat("version_%d", ++versionCount);
               CVariantItem* variantItem = variant->FindItem(compName);
               if (variantItem != NULL)
               {
                  AssignByAttrArray_CompName(compName, pinCount, data, variantItem, variantVersion);
               }
            }
         }
      }
   }
}

/******************************************************************************
* AssignByCompTestArray
*/
static BOOL AssignByCompTestArray(CString compName, int pinCount)
{
   BOOL retval = FALSE;

   HP3070CompTest *compTest = NULL;
   if (compTestMap.Lookup(compName, compTest))
   {
      HPDevice *hpDevice = &hpDeviceArray[compTest->getHpDeviceIndex()];

      if (pinCount > hpDevice->maxPinCount)
      {
         fprintf(logFp, "Component [%s] forced from %s into PIN LIBRARY because of Pincnt [%d, %d]\n",
               compName, hpDevice->deviceName, pinCount, hpDevice->maxPinCount);

         display_error++;

         // update comptest to pinlibrary
         // check if already in attr list - change it otherwise add a new one
         int attrIndex = IsAttr(compName, HP_COMPNAME);

         if (attrIndex > -1)
         {
            HPAttr *attr = attrArray[attrIndex];
            attr->hpDeviceIndex = GetDeviceIndex(HPDEV_PINLIBRARY);
         }
         else
            UpdateAttr(compName, HPDEV_PINLIBRARY, HP_COMPNAME);
      }

      retval = TRUE;
   }

   return retval;

}

/******************************************************************************
* AssignByAttrArray_CompName
*/
static BOOL AssignByAttrArray_CompName(CString compName, int pinCount, DataStruct *compData, CVariantItem* variantItem, const CString variantVersion)
{

   if (compData == NULL && variantItem == NULL)
      return FALSE;

   int compAttrIndex = IsCompName(compName, HP_COMPNAME);

   if (compAttrIndex < 0)
      return FALSE;

   HPAttr *compAttr = attrArray[compAttrIndex];

   HPDevice *hpDevice = &hpDeviceArray[compAttr->hpDeviceIndex];

   int newHPDeviceIndex = compAttr->hpDeviceIndex;

   if (pinCount > hpDevice->maxPinCount)
   {
      fprintf(logFp, "Component [%s] forced from %s into PIN LIBRARY because of actual Pincnt [%d] mismatch to required [%d]\n",
            compName, hpDevice->deviceName, pinCount, hpDevice->maxPinCount);
      display_error++;

      newHPDeviceIndex = HPDEVICE_PIN_LIBRARY;

      // update comptest to pinlibrary
      // check if already in attr list - change it otherwise add a new one
      int attrIndex = IsAttr(compName, HP_COMPNAME);

      if (attrIndex > -1)
      {
         HPAttr *attr = attrArray[attrIndex];
         attr->hpDeviceIndex = GetDeviceIndex(HPDEV_PINLIBRARY);
      }
      else
         UpdateAttr(compName, HPDEV_PINLIBRARY, HP_COMPNAME);
   }

   // switch according to HP type
   // all types have different parameters
   AssignComponentDevice(compName, newHPDeviceIndex, compData, variantItem, variantVersion, compAttr->userName);

   return TRUE;

}

/******************************************************************************
* UpdateAccessAndProbe
   First the access which fills the data_entity (comppin, vias, etc...)
   Second run if the probe entity with is updated to the access entity.
*/
static int UpdateAccessAndProbe(long accessEntityNum, long dataEntityNum, long probeEntityNum)
{
   if (probeEntityNum)
      ProbesPlaced = TRUE;

   for (int i=0; i<accessProbeCount; i++)
   {
      HPAccessProbe *accessProbe = accessProbeArray[i];

      if (accessProbe->accessEntityNum == accessEntityNum)
      {
         if (probeEntityNum)
            accessProbe->probeEntityNum = probeEntityNum;

         if (dataEntityNum)
            accessProbe->dataEntityNum = dataEntityNum;

         return i;
      }
   }

   HPAccessProbe *accessProbe = new HPAccessProbe;
   accessProbeArray.SetAtGrow(accessProbeCount++, accessProbe);  
   accessProbe->accessEntityNum = accessEntityNum;
   accessProbe->dataEntityNum = dataEntityNum;
   accessProbe->probeEntityNum = probeEntityNum;

   return accessProbeCount-1;
}

/******************************************************************************
* UpdateFeatureAccess
   This function determines the generation of the probe access and side and 
   updates the x,y position if the access position was changed

   it returns the layer 1 = top, 2 bottom, 3 all, 0 none
            and 3070_probeaccess and 3070_probeside

   testAttribute: if the feature(or component) was marked with a ATT_TEST attribute.
   testMask: this is the layer 1=top, 2=bottom,3=all mask derived from the test_attribute
   attribute map :
   padstackindex
*/
// features is used for vias and testpoints
static int UpdateFeatureAccess(DataStruct *data, BOOL TestAttribute, int testMask,
      int padstackIndex, double pinRot, double scale,  double *pinX, double *pinY, CString *probeAccess, CString *probeSide, bool useDFTAccessibility)
{
   CString newProbeSide;
   CString newProbeAccess = HP_NORMAL_ACCESS;
   int padstackAccess = 0;

   if (padstackIndex >= 0)
   {
      unsigned long entityNum = data->getEntityNumber();
      CAttributes *attribMap = data->getAttributesRef();
      int padMirror = data->getInsert()->getMirrorFlags() & MIRROR_FLIP;

      // CAD access
      padstackAccess = GetPadstackAccess(padstackIndex, padMirror);
      switch (padstackAccess)
      {
      case 1:
         newProbeSide = "TOP";
         break;

      case 2:
         newProbeSide = "NO_MANUAL";
         break;

      case 3:
         newProbeSide = "BOTH";
         break;

      default:
         newProbeSide = "NO_MANUAL";
         break;
      }

	   if (ProbesPlaced && !is_attvalue(doc, attribMap, ATT_3070_PROBESIDE, 0) 
		   && !is_attvalue(doc, attribMap, ATT_3070_PROBEACCESS, 0))
	   {
		   // TEST_ACCESS attribute
         CalcAccessFeature(attribMap, newProbeSide, newProbeAccess);

   	   // if has probe
		   int accessProbeIndex = GetAccessProbe(entityNum);

		   if (accessProbeIndex >= 0)
		   {
			   HPAccessProbe *accessProbe = accessProbeArray[accessProbeIndex];

			   if (accessProbe->probeEntityNum)
			   {
               //UpdateComponentAttribute(&attribMap, ATT_3070_PROBESIDE  ,*probeSide);      
               //UpdateComponentAttribute(&attribMap, ATT_3070_PROBEACCESS,testpointflag);   

               newProbeAccess = testpointflag;
			   }
		   }
	   }

	   if (TestAttribute && !is_attvalue(doc, attribMap, ATT_3070_PROBESIDE, 0) 
		   && !is_attvalue(doc, attribMap, ATT_3070_PROBEACCESS, 0))
	   {
         //UpdateComponentAttribute(&attribMap, ATT_3070_PROBESIDE  ,*probeSide);      
         //UpdateComponentAttribute(&attribMap, ATT_3070_PROBEACCESS,testpointflag);   

         newProbeAccess = testpointflag;
	   }

	   // if no probes, then test according to TEST attribute
	   if (!ProbesPlaced && TestAttribute && !useDFTAccessibility) //use the test attribute is option 0 is selected
		   newProbeAccess = testpointflag;

	   if (useDFTAccessibility)  //if option 1 (using DFT) is selected, get info from DFT solution
	   {
		   // TEST_ACCESS attribute
         CalcAccessFeature(attribMap, newProbeSide, newProbeAccess);

   	   // if has probe
		   int accessProbeIndex = GetAccessProbe(entityNum);

		   if (accessProbeIndex >= 0)
		   {
			   HPAccessProbe *accessProbe = accessProbeArray[accessProbeIndex];

			   if (accessProbe->probeEntityNum)
			   {
				   newProbeAccess = testpointflag;
				   //case 1064: we shouldn't change teh coordinates, do not use that of probe
				   //*pinX = accessProbe->probeX;
				   //*pinY = accessProbe->probeY;
			   }
            // Case 2194 says if using DFT Accessibility then a line gets the
            // Preferred Flag (aka testpointflag in the code) ONLY IF there is
            // a placed probe. See also UpdateCompPinAccess().
            else
            {
               // Watch out in case CalcAccessFeature already set NO_PROBE
               if (!probeAccess->CompareNoCase("NO_PROBE") == 0)
                  newProbeAccess = HP_NORMAL_ACCESS;
            }

		   }

         //UpdateComponentAttribute(&attribMap, ATT_3070_PROBESIDE  ,*probeSide);      
         //UpdateComponentAttribute(&attribMap, ATT_3070_PROBEACCESS,*probeAccess); 
	   }

	   // overwrite with 3070 attributes
	   if (useDFTAccessibility && !ProbesPlaced || !useDFTAccessibility)
	   {
		   Attrib *attrib;
		   if (attrib = is_attvalue(doc, attribMap, ATT_3070_PROBESIDE, 0)) 
			   newProbeSide = get_attvalue_string(doc, attrib);

		   if (attrib = is_attvalue(doc, attribMap, ATT_3070_PROBEACCESS, 0))  
			   newProbeAccess = get_attvalue_string(doc, attrib);
	   }

      BlockStruct* block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      double viaSize = GetViaSize(block, scale);

      // Case 2195 - Do not apply the 3070.out accesibility rule for min
      // via size if we are using DFT accessibility
      if (!useDFTAccessibility && viaSize < minviadia)
      {
         newProbeAccess = "NO_PROBE";
         newProbeSide = "";
      }
   }
   else
   {
      newProbeAccess = "NO_PROBE";
      newProbeSide = "";
   }

	// case 1748
	// Applies only when in "use 3070.out rules" mode is active
	int testSide = ATTR_TESTSIDE_NOTPRESENT;
	if (useTestAttribForProbeSide)
	{
		Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_TEST, 0);
		if (attrib)   
		{
			CString val = get_attvalue_string(doc, attrib);
			if (!val.CompareNoCase("TOP")) 
					testSide = ATTR_TESTSIDE_TOP;

			if (!val.CompareNoCase("BOTTOM")) 
					testSide =  ATTR_TESTSIDE_BOT;
		}
	}
	// end case 1748

	if ( testSide  != ATTR_TESTSIDE_NOTPRESENT)
	{
		if (testSide == ATTR_TESTSIDE_TOP)
			newProbeSide = "TOP";
		else if (testSide == ATTR_TESTSIDE_BOT)
			newProbeSide = "NO_MANUAL";
		else
			newProbeSide = "BOTH";
		// only update the attribute if test side is flag is present
		//UpdateComponentAttribute(&data->getAttributesRef(), ATT_3070_PROBESIDE  , probeSide);      
		//UpdateComponentAttribute(&data->getAttributesRef(), ATT_3070_PROBEACCESS, probeAccess); 
	}


   Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_3070_PROBESIDE, 0);
   if (attrib == NULL) 
   {
      UpdateComponentAttribute(&data->getAttributesRef(), ATT_3070_PROBESIDE, newProbeSide);
   }
   else
   {
		newProbeSide = get_attvalue_string(doc, attrib);
   }
   *probeSide = newProbeSide;      

   attrib = is_attvalue(doc, data->getAttributesRef(), ATT_3070_PROBEACCESS, 0);
   if (attrib == NULL)
   {
      UpdateComponentAttribute(&data->getAttributesRef(), ATT_3070_PROBEACCESS, newProbeAccess);
   }
   else
   {
	   newProbeAccess = get_attvalue_string(doc, attrib);
   }
   *probeAccess = newProbeAccess; 

   return padstackAccess;
}

/******************************************************************************
* UpdateCompPinAccess
   This function determines the generation of the probe access and side and 
   updates the x,y position if the access position was changed

   it returns the layer 1 = top, 2 bottom, 3 all, 0 none
            and 3070_probeaccess and 3070_probeside

   test_attribute: if the feature(or component) was marked with a ATT_TEST attribute.
   attribute map :
   padstackindex
*/
static int UpdateCompPinAccess(FileStruct *pcbFile, CompPinStruct *compPin, BOOL TestAttribute, CAttributes** attribMap, int padstackIndex,
      int padMirror, int compBottom, double pinRot, double *pinX, double *pinY, CString *probeAccess, CString *probeSide, int pinCount, bool useDFTAccessibility)
{
   CString newProbeSide;
   CString newProbeAccess = HP_NORMAL_ACCESS;

   // CAD access
   int padstackAccess = GetPadstackAccess(padstackIndex, padMirror);
   HP3070Padstack *padstack = padstackarray[padstackIndex];

   //CDebugWriteFormat::getWriteFormat().writef("entityNum=%d, padstackAccess=%d\n",
   //   entityNum,padstackAccess);

   if (padstack->smd)
   {
      if (padstackAccess == 1)
      {
         newProbeSide = "TOP";
         newProbeAccess = "NO_PROBE";
      }
      else if (padstackAccess == 2)
      {
         newProbeSide = "NO_MANUAL";
         newProbeAccess = "NO_PROBE";
      }
   }
   else
   {  
      if (compBottom)
      {
         // if component is placed on bottom, access is on top; 
         padstackAccess = padstackAccess & 1;   // take out 2
      }
      else
      {
         // if component is placed on top, access is on bottom; 
         padstackAccess = padstackAccess & 2;   // take out 1
      }

      if (padstackAccess == 1)   // pin top access
      {
         newProbeSide = "TOP";
         newProbeAccess = HP_NORMAL_ACCESS; // Accessible
      }
      else if (padstackAccess == 2) // pin bottom access
      {
         newProbeSide = "NO_MANUAL";
         newProbeAccess = HP_NORMAL_ACCESS; // Accessible
      }
   }

   // check if the pin has TEST attribute
   Attrib *attrib = is_attvalue(doc, *attribMap, ATT_TEST, 2);
   if (attrib)
   {
      if (attrib->getValueType() == VT_STRING)
      {
         CString test = get_attvalue_string(doc, attrib);
         if (!test.CompareNoCase(testaccesslayers[ATT_TEST_ACCESS_NONE]))
         {
            TestAttribute = FALSE;
            padstackAccess = 0;
         }
         else
			{
            TestAttribute = TRUE;
			}
      }
   } 


	if (TestAttribute)
	{
		// Probe Rules for TESTPOINTS
		//
		// If TESTPOINT surface = TOP then
		//		TOP       MANDATORY      default(MANUAL)
		// 
		// If TESTPOINT surface = BOTTOM then
		//		default(BOTTOM)    MANDATORY      NO_MANUAL
		// 
		// If TESTPOINT surface = THRU then
		//		default(BOTTOM)	   MANDATORY      NO_MANUAL
		//
		CString technology = "";
		attrib = is_attvalue(doc, *attribMap, ATT_TECHNOLOGY, 2);
		if (attrib)
			technology = get_attvalue_string(doc, attrib);

		if (technology.CompareNoCase("THRU") == 0)
		{
			if (compBottom)
			{
            newProbeSide = "TOP";
				newProbeAccess = "MANDATORY";
			}
			else 
			{
            newProbeSide = "NO_MANUAL";
				newProbeAccess = "MANDATORY";
			}
		}
		else
		{
			if (compBottom)
			{
				newProbeSide = "NO_MANUAL";
				newProbeAccess = "MANDATORY";
			}
			else 
			{
				newProbeSide = "TOP";
				newProbeAccess = "MANDATORY";
			}
		}
	}
	

	if (ProbesPlaced && !is_attvalue(doc, *attribMap, ATT_3070_PROBESIDE, 0) 
		&& !is_attvalue(doc, *attribMap, ATT_3070_PROBEACCESS, 0))
	{
		// TEST_ACCESS attribute
      CalcAccessFeature(*attribMap, newProbeSide, newProbeAccess);

		// Potential override with TEST attribute
		int testSide = testSideAttribOverride(useDFTAccessibility, pcbFile->getBlock(), compPin->getRefDes());
		if (testSide != ATTR_TESTSIDE_NOTPRESENT)
		{
			newProbeSide = (testSide == ATTR_TESTSIDE_TOP ? "TOP" : "NO_MANUAL"); // "" is BOTTOM
		}

   	// if has probe
		int accessProbeIndex = GetAccessProbe(compPin->getEntityNumber());

		if (accessProbeIndex >= 0)
		{
			HPAccessProbe *accessProbe = accessProbeArray[accessProbeIndex];

			if (accessProbe->probeEntityNum)
			{
            //UpdateComponentAttribute(attribMap, ATT_3070_PROBESIDE  ,*probeSide);      
            //UpdateComponentAttribute(attribMap, ATT_3070_PROBEACCESS,testpointflag);   

            newProbeAccess = testpointflag;
			}
		}
	}


	// if no probes, then test according to TEST attribute
	if (!useDFTAccessibility && !ProbesPlaced && TestAttribute) //use TEST attribute only if we are using rules file
		newProbeAccess = testpointflag;

	if (useDFTAccessibility) //use DFT solution
	{
		// TEST_ACCESS attribute
		CalcAccessComppin(*attribMap, newProbeSide, newProbeAccess, padstackIndex, pinCount); 
		
		// Potential override with TEST attribute
		int testSide = testSideAttribOverride(useDFTAccessibility, pcbFile->getBlock(), compPin->getRefDes());
		if (testSide != ATTR_TESTSIDE_NOTPRESENT)
		{
			newProbeSide = (testSide == ATTR_TESTSIDE_TOP ? "TOP" : "NO_MANUAL"); // "" is BOTTOM
		}

		int accessProbeIndex = GetAccessProbe(compPin->getEntityNumber());

		if (accessProbeIndex >= 0)
		{
			HPAccessProbe *accessProbe = accessProbeArray[accessProbeIndex];

			if (accessProbe->probeEntityNum) // if has probe
			{
				newProbeAccess = testpointflag;
			}
         // Case 2194 says if using DFT Accessibility then a line gets the
         // Preferred Flag (aka testpointflag in the code) ONLY IF there is
         // a placed probe. So stop doing the following else and do the
         // new else.
         ////else if (!ProbesPlaced && accessProbe->accessEntityNum) // AA done but not PP
			////{
         ////   newProbeAccess = testpointflag;
         ////}
         else   // this is the new else for case 2194
         {
            // Watch out in case CalcAccessCompin already set NO_PROBE
            if (!newProbeAccess.CompareNoCase("NO_PROBE") == 0)
               newProbeAccess = HP_NORMAL_ACCESS;
         }
		}
      //UpdateComponentAttribute(attribMap, ATT_3070_PROBESIDE  ,*probeSide);      
      //UpdateComponentAttribute(attribMap, ATT_3070_PROBEACCESS,*probeAccess);
   }
   else 	if (TestAttribute && !is_attvalue(doc, *attribMap, ATT_3070_PROBESIDE, 0) 
		&& !is_attvalue(doc, *attribMap, ATT_3070_PROBEACCESS, 0))
	{
		// Potential override with TEST attribute
		int testSide = testSideAttribOverride(useDFTAccessibility, pcbFile->getBlock(), compPin->getRefDes());
		if (testSide != ATTR_TESTSIDE_NOTPRESENT)
		{
			newProbeSide = (testSide == ATTR_TESTSIDE_TOP ? "TOP" : "NO_MANUAL"); // "" is BOTTOM
		}

      //UpdateComponentAttribute(attribMap, ATT_3070_PROBESIDE  ,*probeSide);      
      //UpdateComponentAttribute(attribMap, ATT_3070_PROBEACCESS,testpointflag);   
	}

	int testSide = ATTR_TESTSIDE_NOTPRESENT;
	if (useTestAttribForProbeSide)
	{
		Attrib *attrib = is_attvalue(doc, compPin->getAttributesRef(), ATT_TEST, 0);
		if (attrib)   
		{
			CString val = get_attvalue_string(doc, attrib);
			if (!val.CompareNoCase("TOP")) 
					testSide = ATTR_TESTSIDE_TOP;

			if (!val.CompareNoCase("BOTTOM")) 
					testSide =  ATTR_TESTSIDE_BOT;
		}
	}

	if (testSide != ATTR_TESTSIDE_NOTPRESENT)
	{
		newProbeSide = (testSide == ATTR_TESTSIDE_TOP ? "TOP" : "NO_MANUAL");
	}

    // overwrite with 3070 attributes if it is already there
   attrib = is_attvalue(doc, *attribMap, ATT_3070_PROBESIDE, 0);
   if (attrib == NULL) 
      UpdateComponentAttribute(attribMap, ATT_3070_PROBESIDE, newProbeSide);
   else
		newProbeSide = get_attvalue_string(doc, attrib);

   attrib = is_attvalue(doc, *attribMap, ATT_3070_PROBEACCESS, 0);
   if (attrib == NULL)
      UpdateComponentAttribute(attribMap, ATT_3070_PROBEACCESS, newProbeAccess);
   else
	   newProbeAccess = get_attvalue_string(doc, attrib);

   *probeSide = newProbeSide;      
   *probeAccess = newProbeAccess; 

   return padstackAccess;
}

/******************************************************************************
* UpdateAttr
*/
static int UpdateAttr(CString attrName, CString deviceName, int attrType)
{
   int deviceIndex = -1;
   CString userName = "";

   // HP_TESTPREF is only the Component Prefix, but no device. This is only used to
   // update the component entry with the ATT_TEST attribute.
   if (attrType != HP_TESTPREF && attrType != HP_TOOLINGPREF && attrType != HP_TOOLINGSHAPE)
   {
      deviceIndex = GetDeviceIndex(deviceName);

      if (deviceIndex < 0)
      {
         // if it was an unknown device - make it a 
         // pin library and writeout later a remark line.
         deviceIndex = GetDeviceIndex(HPDEV_PINLIBRARY);
         userName = deviceName;
      }
   }

   HPAttr *attr = new HPAttr;
   attrArray.SetAtGrow(attrCount, attr);  
   attrCount++;   

   attr->attrName = attrName;
   attr->hpDeviceIndex = deviceIndex;
   attr->userName = userName;
   attr->attrType = attrType;

   return attrCount-1;
}

/******************************************************************************
* CalcAccessFeature
*/
static void CalcAccessFeature(CAttributes* attribMap, CString& probeSide, CString& probeAccess)
{
   Attrib *attrib = is_attvalue(doc, attribMap, ATT_TESTACCESS, 0);
   if (attrib) 
   {
      CString testAccess = get_attvalue_string(doc, attrib);

      if (!testAccess.CompareNoCase("NONE"))
         probeAccess = "NO_PROBE";
      else if (!testAccess.CompareNoCase("TOP"))
         probeSide = "TOP";
      else if (!testAccess.CompareNoCase("BOTTOM"))
         probeSide = "NO_MANUAL";
      else if (!testAccess.CompareNoCase("BOTH"))
         probeSide = "BOTH";
   }
}

/******************************************************************************
* CalcAccessComppin
*/
static void CalcAccessComppin(CAttributes* attribMap, CString& probeSide, CString& probeAccess, int padstackIndex, int pinCount)
{
   Attrib *attrib = is_attvalue(doc, attribMap, ATT_TESTACCESS, 0);
   if (attrib)
   {
      CString testAccess = get_attvalue_string(doc, attrib);

      if (!testAccess.CompareNoCase("NONE"))
         probeAccess = "NO_PROBE";
      else
      {
         if (padstackarray[padstackIndex]->smd && pinCount > 1)
            probeAccess = "UNRELIABLE";
         else
         {
            if (!testAccess.CompareNoCase("TOP")) // and not SMD
            {
               probeSide = "TOP"; // test access, not component
               probeAccess = HP_NORMAL_ACCESS; // Accessible  // case dts0100375847   was: "NO_PROBE";
            }
            else if (!testAccess.CompareNoCase("BOTTOM"))
            {
               probeSide = "NO_MANUAL"; // test access, not component
               probeAccess = HP_NORMAL_ACCESS; // Accessible
            }
            else
               probeAccess = HP_NORMAL_ACCESS; // Accessible
         }
      }
   }
}

/******************************************************************************
* GetDeviceIndex
*  - here check first that the devicename is a HP device name
*/
static int GetDeviceIndex(CString deviceName)
{
   int maxHPDeviceArray = sizeof(hpDeviceArray) / sizeof(HPDevice);

   for (int i=0; i<maxHPDeviceArray;i++)
   {
      HPDevice *hpDevice = &hpDeviceArray[i];

      if (!STRCMPI(hpDevice->deviceName, deviceName))
         return i;
   }

   return -1;
}

/******************************************************************************
* GetCompIndex
*/
static int GetCompIndex(CString name)
{
   if (name.IsEmpty())
      return -1;

   for (int i=0; i<compCount; i++)
   {
      HP3070Comp *comp = compArray[i]; 

      if (!comp->name.Compare(name))
         return i;
   }

   HP3070Comp *comp = new HP3070Comp;
   compArray.SetAtGrow(compCount, comp);  
   compCount++;   

   comp->name = name;
   comp->testnone = 0;     // flagged as not to be tested
   comp->testflag = 0;     // flagged as having a forced test attribute
   comp->written_out = 0;  // if false, component must be still written out
   comp->smdflag = 0;      // if ATT_SMD was set
   comp->inserttype = 0;   // np stored inserttype;
   comp->pincnt = 0;
   comp->bottom = 0;

   return compCount-1;
}

/******************************************************************************
* GetPadBlockName
*/
static const char *GetPadBlockName(CDataList *dataList)
{
   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
      {
         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         return block->getName();
      }
   }

   return NULL;
}

/******************************************************************************
* GetViaSize
// This function will take the drill of the first pin found. 
// only makes sense for single pin comps.
*/
static double GetViaSize(BlockStruct *block, double scale)
{
   double viaSize = 0;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      // this now must be a flash
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         // if it is an aperture, find if on top or bottom layer
         int layerNum = Get_ApertureLayer(doc, data, block, -1);

         if (doc->IsFloatingLayer(layerNum))
            break;

         LayerStruct *layer = doc->FindLayer(layerNum);

         switch (layer->getLayerType())
         {
            case LAYTYPE_SIGNAL_TOP:
            case LAYTYPE_SIGNAL_ALL:
            case LAYTYPE_SIGNAL_BOT:
            case LAYTYPE_PAD_ALL:
            case LAYTYPE_PAD_TOP:
            case LAYTYPE_PAD_BOTTOM:
            {
               double sizeA = block->getSizeA() * scale;

               if (sizeA > viaSize)
                  viaSize = sizeA;
            }
            break;
         }
      }
   }

   return viaSize;
}

/******************************************************************************
* GetPadstackAccess
*  - result can only be 1 ot 2
*/
static int GetPadstackAccess(int padstackIndex, int padMirror)
{
   int padstackAccess = 0;

   HP3070Padstack *padstack = padstackarray[padstackIndex];

   if (padstack->flag & BL_ACCESS_TOP)
   {
      if (padMirror)
         padstackAccess |= 2;
      else
         padstackAccess |= 1;
   }

   if (padstack->flag & BL_ACCESS_BOTTOM)
   {
      if (padMirror)
         padstackAccess |= 1;
      else
         padstackAccess |= 2;
   }

   if (padstack->flag & BL_ACCESS_TOP_ONLY && !padMirror)
      padstackAccess |= 1;

   if (padstack->flag & BL_ACCESS_BOTTOM_ONLY && padMirror)
      padstackAccess |= 2;

   return padstackAccess;
}

/******************************************************************************
* GetAccessProbe
*/
static int GetAccessProbe(long dataEntityNum)
{
   // Some dataEntities have more than one access marker.
   // E.g. a THRU compPin with access marks on top and bottom, see case 2154, "TP1".
   // So can not stop this loop on first access marker match.
   // Stop loop only if access marker matches and has a probe.
   // If access marker matches, but has no probe, there might be another access marker
   // that matches and does have a probe.
   // A shame really, as it makes every search much longer.
   // Maybe it can be enhanced slightly by checking is THRU or SMD, since SMD should
   // have only one, and THRU might have more. Maybe even for THRU, if we find two then
   // we can stop looking, probe or not.

   int chosenAP = -1;
   bool secondAccessFound = false;

   for (int i=0; i<accessProbeCount; i++)
   {
      HPAccessProbe *accessProbe = accessProbeArray[i];

      // If accessprobe is on this data entity it is a candidate
      if (accessProbe->dataEntityNum == dataEntityNum)
      {
         if (chosenAP != -1)
            secondAccessFound = true;

         chosenAP = i;

         // If it has a probe we can return it now
         if (accessProbe->probeEntityNum > 0)
            return chosenAP;
      }

      // If second access marker was found, we may as well quit looping.
      // It makes sense there might be two, one on each surface, but it
      // seems unlikely there will be a third. Users will no doubt let us
      // know if this is a mistake.
      if (secondAccessFound)
         return chosenAP;
   }

   return chosenAP;
}

/******************************************************************************
* GetCompPrefix
*/
static CString GetCompPrefix(CString compName)
{
   CString prefix;

   int i = 0;

   while (isalpha(compName[i]))
      prefix += compName[i++];

   return prefix;
}

/******************************************************************************
* GetDeviceMapName
*  - here get the CAMCAD devicetype mapped to HP device

#define HPDEV_NODELIBRARY    "NODE LIBRARY"
#define HPDEV_CAPACITOR      "CAPACITOR"
#define HPDEV_CONNECTOR      "CONNECTOR"
#define HPDEV_DIODE          "DIODE"
#define HPDEV_FET            "FET"
#define HPDEV_JUMPER         "JUMPER"
#define HPDEV_FUSE           "FUSE"
#define HPDEV_INDUCTOR       "INDUCTOR"
#define HPDEV_POTENTIOMETER  "POTENTIOMETER"
#define HPDEV_RESISTOR       "RESISTOR"   
#define HPDEV_SWITCH         "SWITCH"
#define HPDEV_TRANSISTOR     "TRANSISTOR"
#define HPDEV_ZENER          "ZENER"
#define HPDEV_PINLIBRARY     "PIN LIBRARY"
#define HPDEV_UNDEFINED      "UNDEFINED"

*/
static CString GetHpDeviceName(DeviceTypeTag cczDevType)
{
   CString hpDeviceType;

   switch (cczDevType)
   {
   case deviceTypeUnknown:                hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeBattery:                hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeCapacitor:              hpDeviceType = HPDEV_CAPACITOR; break;
   case deviceTypeCapacitorPolarized:     hpDeviceType = HPDEV_CAPACITOR; break;
   case deviceTypeCapacitorTantalum:      hpDeviceType = HPDEV_CAPACITOR; break;

   case deviceTypeConnector:              hpDeviceType = HPDEV_CONNECTOR; break;

   case deviceTypeCapacitorArray:         hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeCrystal:                hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeDiode:                  hpDeviceType = HPDEV_DIODE; break;
   case deviceTypeDiodeLed:               hpDeviceType = HPDEV_DIODE; break;

   case deviceTypeDiodeArray:             hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeDiodeLedArray:          hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeDiodeZener:             hpDeviceType = HPDEV_ZENER; break;

   case deviceTypeFilter:                 hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeFuse:                   hpDeviceType = HPDEV_JUMPER; break;

   case deviceTypeIC:                     hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeICDigital:              hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeICLinear:               hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeInductor:               hpDeviceType = HPDEV_INDUCTOR; break;

   case deviceTypeJumper:                 hpDeviceType = HPDEV_JUMPER; break;

   case deviceTypeNoTest:                 hpDeviceType = ""; break;                 // Blank means no mapping to HP
   case deviceTypeOscillator:             hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypePotentiometer:          hpDeviceType = HPDEV_POTENTIOMETER; break;

   case deviceTypePowerSupply:            hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeRelay:                  hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeSwitch:                 hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeTransformer:            hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeResistor:               hpDeviceType = HPDEV_RESISTOR; break;

   case deviceTypeResistorArray:          hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeSpeaker:                hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeTestPoint:              hpDeviceType = ""; break;                 // Blank means no mapping to HP
   case deviceTypeTransistorArray:        hpDeviceType = HPDEV_PINLIBRARY; break;

   case deviceTypeTransistorFetNpn:       hpDeviceType = HPDEV_FET; break;
   case deviceTypeTransistorFetPnp:       hpDeviceType = HPDEV_FET; break;
   case deviceTypeTransistorMosfetNpn:    hpDeviceType = HPDEV_FET; break;
   case deviceTypeTransistorMosfetPnp:    hpDeviceType = HPDEV_FET; break;

   case deviceTypeTransistor:             hpDeviceType = HPDEV_TRANSISTOR; break;
   case deviceTypeTransistorNpn:          hpDeviceType = HPDEV_TRANSISTOR; break;
   case deviceTypeTransistorPnp:          hpDeviceType = HPDEV_TRANSISTOR; break;

   case deviceTypeTransistorScr:          hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeTransistorTriac:        hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeVoltageRegulator:       hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeOpto:                   hpDeviceType = HPDEV_PINLIBRARY; break;
   case deviceTypeUndefined:              hpDeviceType = HPDEV_PINLIBRARY; break;

   default:
          hpDeviceType = HPDEV_PINLIBRARY;
          break;
   }

   return hpDeviceType;
}

/******************************************************************************
*/
static CString getHpDevicePinName(int hpDeviceTypeIndex, CString mappedPinFunction)
{
   CString hpPinName;

   switch (hpDeviceTypeIndex)
   {
   
   case HPDEVICE_CAPACITOR:
      if (mappedPinFunction.CompareNoCase("Positive") == 0)
         hpPinName = "1";
      else if (mappedPinFunction.CompareNoCase("Negative") == 0)
         hpPinName = "2";
      break;

   case HPDEVICE_DIODE:
   case HPDEVICE_ZENER:
      if (mappedPinFunction.CompareNoCase("Anode") == 0)
         hpPinName = "A";
      else if (mappedPinFunction.CompareNoCase("Cathode") == 0)
         hpPinName = "C";
      break;

   case HPDEVICE_FET:
      if (mappedPinFunction.CompareNoCase("Source") == 0)
         hpPinName = "S";
      else if (mappedPinFunction.CompareNoCase("Drain") == 0)
         hpPinName = "D";
      else if (mappedPinFunction.CompareNoCase("Gate") == 0)
         hpPinName = "G";
      break;

   case HPDEVICE_TRANSISTOR:
      if (mappedPinFunction.CompareNoCase("Base") == 0)
         hpPinName = "B";
      else if (mappedPinFunction.CompareNoCase("Collector") == 0)
         hpPinName = "C";
      else if (mappedPinFunction.CompareNoCase("Emitter") == 0)
         hpPinName = "E";
      break;

   case HPDEVICE_RESISTOR:
   case HPDEVICE_POTENTIOMETER:
   case HPDEVICE_JUMPER:
   case HPDEVICE_FUSE:
   case HPDEVICE_INDUCTOR:
   case HPDEVICE_SWITCH:
   case HPDEVICE_CONNECTOR:
   case HPDEVICE_NODE_LIBRARY:
   case HPDEVICE_PIN_LIBRARY:
   case HPDEVICE_UNDEFINED:
   default:
      // Leave blank, no mapping for any pin for these device types.
      break;
   }

   return hpPinName;
}

/******************************************************************************
*/
static int IsCompName(CString compName, int attrType)
{
   for (int i=0; i<attrCount; i++)
   {
      HPAttr *attr = attrArray[i];

      if (!attr->attrName.CompareNoCase(compName) && attr->attrType == attrType)
         return i;
   }

   return -1;
}

/******************************************************************************
* IsAttr
*/
static int IsAttr(CString attrName, int attrType)
{
   if (attrName.IsEmpty())
      return -1;

   for (int i=0; i<attrCount; i++)
   {
      HPAttr *attr = attrArray[i];

      if (!attr->attrName.CompareNoCase(attrName) && attr->attrType == attrType)
         return i;
   }

   return -1;
}

/******************************************************************************
* IsCompPrefix
*/
static int IsCompPrefix(CString compName)
{
   CString prefix = GetCompPrefix(compName);

   for (int i=0; i<attrCount; i++)
   {
      HPAttr *attr = attrArray[i];

      if (attr->attrType == HP_COMPPREF && !attr->attrName.CompareNoCase(prefix))
         return i;
   }

   return -1;
}

/******************************************************************************
* UpdateComponentAttribute 
*/
static void UpdateComponentAttribute(CAttributes** attribMap, CString keyword, CString value)
{
   doc->SetUnknownAttrib(attribMap, keyword, value, SA_OVERWRITE, NULL); 
}
//------------------------------------------------------------------------

static DataStruct *FindComponentData(CCEtoODBDoc *doc, CString refdes)
{
	for (int i = 0; i < doc->getMaxBlockIndex(); i++)
	{
		BOOL found = FALSE;
		BlockStruct *block = doc->getBlockAt(i);

		if (block != NULL)
		{
			POSITION pos = block->getDataList().GetHeadPosition();
			while (pos)
			{
				DataStruct *data = block->getDataList().GetNext(pos);

				if (data->getDataType() == dataTypeInsert && data->getInsert() != NULL &&
					(data->getInsert()->getInsertType() == insertTypePcbComponent || data->getInsert()->getInsertType() == insertTypeTestPoint))
				{
					if (data->getInsert()->getRefname().Compare(refdes) == 0)
					{
						return data;
					}
				}
			}
		}
	}

	return NULL;
}

/******************************************************************************
* findPadStackFromCompPin 
*/
BlockStruct* findPadStackFromCompPin(CCEtoODBDoc *doc, CompPinStruct* compPin)
{
	if (doc != NULL && compPin != NULL)
	{
		DataStruct* dataFound = FindComponentData(doc, compPin->getRefDes());

		if (dataFound != NULL && dataFound->getInsert() != NULL)
		{
			BlockStruct *padStack = NULL;

			BlockStruct *block = doc->getBlockAt(dataFound->getInsert()->getBlockNumber());

			if (block != NULL)
			{
				POSITION pinDataPos = block->getDataList().GetHeadPosition();
				while (pinDataPos)
				{
					DataStruct *pinData = block->getDataList().GetNext(pinDataPos);

					if (pinData != NULL && 
						pinData->getInsert() != NULL &&
						pinData->getInsert()->getInsertType() == insertTypePin &&
						pinData->getInsert()->getRefname().Compare(compPin->getPinName()) == 0)
					{
						// Note that there is no enforcement that this "padstack" is actually
						// a block of type padstack.
						
						BlockStruct *padStack = doc->getBlockAt(pinData->getInsert()->getBlockNumber());
						return padStack;
					}
				}
			}
		}
	}

   return NULL;
}

//-----------------------------------------------------------------------------
// HP3070CompTest
//-----------------------------------------------------------------------------
HP3070CompTest::HP3070CompTest(const CString compName, const int hpDeviceIndex, const CString testString)
{
   m_compName = compName;
   m_hpDeviceIndex = hpDeviceIndex;
   m_testStringArray.SetSize(0, 5);
   addTestString(testString, "");
}

HP3070CompTest::~HP3070CompTest()
{
   m_testStringArray.RemoveAll();
}

void HP3070CompTest::addTestString(const CString testString, const CString version)
{
   if (m_testStringArray.GetCount() == 0)
   {
      m_testStringArray.SetAtGrow(m_testStringArray.GetCount(), testString);
   }
   else if (!version .IsEmpty())
   {
      // If there is already a testString and there is a version, then check to see if the exact testString is already exist.
      // If it doesn't exist, then add it to the array

      bool found = false;
      for (int i=0; i<m_testStringArray.GetCount(); i++)
      {
         CString existingTestString = m_testStringArray.GetAt(i);

         // Remove the version part from the existing test string and compare it the new test string
         int index = existingTestString.Find("version");
         if (index > -1)
         {
            existingTestString = existingTestString.Left(index);
            existingTestString.Trim();
         }

         if (testString.CompareNoCase(existingTestString) == 0)
            found = true;
      }

      if (!found)
      {
         m_testStringArray.SetAtGrow(m_testStringArray.GetCount(), testString + " version " + version);
      }
   }
}

void HP3070CompTest::writeTestString(FILE *fBrd)
{
   for (int i=0; i<m_testStringArray.GetCount(); i++)
   {
      CString testString = m_testStringArray.GetAt(i);
      fprintf(fBrd, "   %s %s;\n", check_name('c', m_compName, FALSE), testString);
   }
 }

/******************************************************************************
* UpdateCompTest
*/
static void UpdateCompTest(int hpDeviceIndex, CString compName, CString testString, const CString version)
{
   HP3070CompTest *compTest = NULL;
   if (compTestMap.Lookup(compName, compTest))
   {
      compTest->addTestString(testString, version);
   }
   else
   {
      compTest = new HP3070CompTest(compName, hpDeviceIndex, testString);
   }
   compTestMap.SetAt(compName, compTest);  
}