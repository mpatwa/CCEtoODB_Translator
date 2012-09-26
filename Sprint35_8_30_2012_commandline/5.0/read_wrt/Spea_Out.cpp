// $Header: /CAMCAD/5.0/read_wrt/Spea_Out.cpp 74    6/17/07 9:00p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved. 
*/

#include "stdafx.h"
#include <math.h>
#include <string.h>    
#include "ccdoc.h"
#include "pcbutil.h"
#include "format_s.h"
#include "attrib.h"
#include "Centroid.h"
#include "net_util.h"
#include "RwLib.h"
#include "Ta.h"
#include "Find.h"
#include "Spea_Out.h"
#include "xform.h"
#include "dft.h"
#include "CCEtoODB.h"
#include "PolyLib.h"
#include "CompValues.h"
#include "DeviceType.h"
#include "Ck.h"
#include "Variant.h"
#include "RwUiLib.h"

#define ARC_ANGLE_DEGREE  5.0

#define SPEA_NO_PAD_CODE  -1

#define DOUBLE_NOT_SET -10203.0

/******************************************************************************
*/
class CMapInt : public CMap<int, int, int, int>
{
};
/*
******************************************************************************/

extern PinkooArray pinkooarray;	// from TA.CPP
extern int pinkoocnt;				// from TA.CPP
extern ViakooArray viakooarray;	// from TA.CPP
extern int viakoocnt;				// from TA.CPP
extern TA_NetArray taNetArray;	// from TA_DLG.CPP
extern int taNetArrayCount;		// from TA_DLG.CPP

void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);

/* Static Variables *********************************************************/
static FILE                *speaLogFp;
static long                sDisplayErr;         // current number of errors
static CMapStringToString	sDeviceTypeMap;
static bool						bExportVias;
static bool                bExportTestpointsAsComponents;
static bool                bAnnotateTestpoints;
static bool                bNetlistUseDftPinAccess;
static EExportTracesFlag	ExportTraces;
static CString					sPackageAttribute;
static CMapStringToString  sFiducialNameMap;
static CMapStringToPtr		sNetViaListMap;
static CMapStringToString	sAccessibleViaMap;

/* Function Prototypes *********************************************************/
static void LoadSettingsFile();
static void GenerateViaInfo(CCEtoODBDoc *doc, FileStruct *file);
static void GenerateAccessibleViaMap(CCEtoODBDoc *doc, FileStruct *file);
static CString GetFiducialName(int entityNum);
static CString GetValidPackageName(CString packageName, CMapStringToString* packageRenameMap);
static void GenerateFiducialNameMap(CCEtoODBDoc *doc, FileStruct *file);
static void MapBlockToPackageName(CCEtoODBDoc *doc, FileStruct *file, CMapStringToString* packageNameMap);
static int GetInsertCentroid(CCEtoODBDoc *doc, DataStruct *data, double &x, double &y);
static int GetTestTargetInfo(CCEtoODBDoc *doc, FileStruct *file, DataStruct *data, InsertTypeTag insertType, 
                             CString &refname, CString &pinname, int &contactCode, int &padCode, int &accessMarkEntityNum);

static void WriteHeader(FILE *fp);
static void WriteBoardInfo(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, int pageUnit);
static void WriteNetlist(FILE *fp, CCEtoODBDoc *doc, FileStruct *file);
static void WritePartNumberData(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, CMapStringToString* packageNameMap, CMapStringToString* packageRenameMap);

static void GatherTestpoints(CCEtoODBDoc *doc, FileStruct *file);
static void GatherTestpointsByInsertType(CCEtoODBDoc *doc, FileStruct *file, InsertTypeTag insertType,
													int &testpointNum, CMapInt *accessMarkEntityNumMap);

static void WriteTestpointSection(FILE *fp, CCEtoODBDoc *doc, FileStruct *file);
static void WriteTestpointArray(FILE *fp);

static void GatherParts(CCEtoODBDoc *doc, FileStruct *file, bool panelDesign);
static void WritePartlistSection(FILE *fp);
static void WritePart(FILE *fp, SpeaPart *part);
static void WritePart(FILE *fp, SpeaTestpoint *testpoint);
static void WritePart(FILE *fp, CString partNumber, int variantNumber, CString drawingRef, double x, double y, char mountSide, int rotation, CString annotation);

static void WritePackagesSection(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, CMapStringToString* packageNameMap, CMapStringToString* packageRenameMap);
static void WritePad(FILE *fp, CCEtoODBDoc *doc, FileStruct *file);
static void WritePanelSection(FILE *fp, FileStruct *panelFile,int offsetTP);
static void WriteVariantSection(FILE *fp, FileStruct *file);
static void WriteWiring(FILE *fp, CCEtoODBDoc *doc, FileStruct *file);
static void WriteRouting(FILE *fp, CCEtoODBDoc *doc, FileStruct *file);
static void WriteBoardOutline(FILE *fp, CCEtoODBDoc *doc, FileStruct *file);

static CString GetTechnology(CCEtoODBDoc *doc, CompPinStruct *compPin, DataStruct *componentdata);
static DataStruct *FindComponentData(FileStruct *file, CString refdes);
static bool IsThroughHole(CCEtoODBDoc *doc, FileStruct *file, CompPinStruct *cp, DataStruct *parentComp);

static BlockStruct *getFileBlock(FileStruct *file, CString sectionName);

static void speaWriteSHPoly(CCEtoODBDoc *doc, FILE *fp, int indent,const CPolyList *polyList, double unitFactor, bool isNetTrace, bool writeMirrored, int netIndx, int speaLayer);

static int OutlineLineNum = 0;
static FileStruct * getPcbFile(CCEtoODBDoc *doc, FileStruct *panelFile);
static SpeaUniqueBoardArray 	uniqueBoardArray;
static int         				uniqueBoardCount;
static BOOL in_boardarray(int blockNumber, int mirror);
static int FindUniqueBoardCount(FileStruct *file);
static int findHighestProbeNumber(FileStruct *file);
static SpeaTestpointArray     TestpointArray;
static SpeaPartArray          PartArray;

/******************************************************************************
* SPEA4040_WriteFile 
*/
void SPEA4040_WriteFile(CString fileName, CCEtoODBDoc *doc, FormatStruct *format, int pageUnits)
{
	CMapStringToString blockToPackageNameMap;
	CMapStringToString packageRenameMap;

	// Initialize module variables
	speaLogFp = NULL;
	sDisplayErr = 0;
	sDeviceTypeMap.RemoveAll();
   //deadcode bool panelDesign = false;
   TestpointArray.empty();
   PartArray.empty();

   FileStruct *file = NULL;
   FileStruct *panelFile = NULL;
   int probeNumberOffset;
   panelFile = doc->getFileList().GetFirstShown(blockTypePanel);
   if (panelFile != NULL)
   {
      int numberOfUniqueBoards = FindUniqueBoardCount(panelFile);
      if(1 == numberOfUniqueBoards)
      {
         file = getPcbFile(doc, panelFile);
         probeNumberOffset = findHighestProbeNumber(file);
         CSpeaOutProbeOffsetDlg dlg(probeNumberOffset);
         if (IDOK == dlg.DoModal())
         {
            probeNumberOffset = dlg.getProbeNumberOffset();
         }
      }
      else
      {
         //post a message that multi board panel is not supported.
         ErrorMessage("Multi-board and mirrored board panels are not supported.", "SPEA 4040 Export", MB_OK | MB_ICONEXCLAMATION);
         return;
      }
   }
   else
   {
      file = doc->getFileList().GetFirstShown(blockTypePcb);
   }


   if (!file)
   {
      ErrorMessage("A visible file is required for SPEA export.\nMake sure a file exists and is On, then retry export.", "No Visible File", MB_OK | MB_ICONHAND);
      return;
   }
   
   // Open log file.
   CString localLogFilename;
   speaLogFp = getApp().OpenOperationLogFile("spea.log", localLogFilename);
   if (speaLogFp == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(speaLogFp, "SPEA 4040");

	// Open the output file
   FILE *fp = fopen(fileName, "wt");
   if (!fp)
   {
      CString buf;
      buf.Format("Can not open [%s]", fileName);
      ErrorMessage(buf, "SPEA 4040 Export File Open", MB_OK | MB_ICONHAND);
   }
   else
   {
      // Perform the export.

      // Init namecheck util.
      CString nameCheckFile( getApp().getSystemSettingsFilePath("Spea4040.chk") );
      check_init(nameCheckFile, /*do not raise case*/ false);

      // General init, data gathering, prep.
      LoadSettingsFile();
      ExplodeNcPins(doc, file);
      GenerateViaInfo(doc, file);
      GenerateAccessibleViaMap(doc, file);  // call this after GenerateViaInfo() to make sure via names are already created
      GenerateFiducialNameMap(doc, file);
      MapBlockToPackageName(doc, file, &blockToPackageNameMap);
      doc->OnGenerateSmdComponentVisibleFilesOnly();

      bool panelDesign = NULL != panelFile;
      GatherParts(doc, file, panelDesign);
      GatherTestpoints(doc, file);

      // Generating output
      WriteHeader(fp);
      WriteBoardInfo(fp, doc, file, pageUnits);
      WritePartlistSection(fp);
      WriteNetlist(fp, doc, file);
      WritePartNumberData(fp, doc, file, &blockToPackageNameMap, &packageRenameMap);
      if (!bExportTestpointsAsComponents)
         WriteTestpointSection(fp, doc, file);  // If writing testpoints to partlist section then do not write them here
      WritePackagesSection(fp, doc, file, &blockToPackageNameMap, &packageRenameMap);
      WritePad(fp, doc, file);
      if(panelFile)
      {
         WritePanelSection(fp,panelFile,probeNumberOffset);
      }
      else
         WriteVariantSection(fp,file);

      if (ExportTraces != eExportTracesNone)
      {
         WriteWiring(fp, doc, file);
         WriteRouting(fp, doc, file);
      }

      WriteBoardOutline(fp, doc, file);

      // Export itself is now finished.
      fclose(fp);
   }

   // All done, close it, clean it up, go home.

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(speaLogFp);
      speaLogFp = NULL;

      // We want this to happen only for local log files, not when
      // system log file is in play.
      if (sDisplayErr)
         Logreader(localLogFilename);
   }

	// Reset all module variables
	sDisplayErr = 0;
	sDeviceTypeMap.RemoveAll();
	sFiducialNameMap.RemoveAll();
	sAccessibleViaMap.RemoveAll();
   TestpointArray.empty();
   PartArray.empty();

	blockToPackageNameMap.RemoveAll();
	packageRenameMap.RemoveAll();

	POSITION pos = sNetViaListMap.GetStartPosition();
	while (pos)
	{
		CString key;
		CMapStringToPtr *map = NULL;
		sNetViaListMap.GetNextAssoc(pos, key, (void*&)map);
		map->RemoveAll();
		delete map;
	}

	sNetViaListMap.RemoveAll();

   check_deinit();
}

/******************************************************************************
*/
static void WriteHeader(FILE *fp)
{
   // Write comment line
   CTime currentTime = CTime::GetCurrentTime();
   fprintf(fp, ";Created by %s, %s\n\n", getApp().getCamCadTitle(), currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));

   // Write version number
   fprintf(fp, ":CADFILEINFO\n");

   if (ExportTraces != eExportTracesNone)
      fprintf(fp, "2.20\n"); // Trace info requires version 2.20
   else
      fprintf(fp, "2.10\n"); // Older vesion w/o Trace info

   fprintf(fp, ":ENDCADFILEINFO\n\n");
}

/******************************************************************************
*/
static void LoadSettingsFile()
{
   FILE *fp;
   char line[255];
   char *lp;

   CString settingsFile( getApp().getExportSettingsFilePath("SPEA4040.out") );
   {
      CString msg;
      msg.Format("SPEA 4040 Export: Settings file [%s].\n\n", settingsFile);
      getApp().LogMessage(msg);
   }

	bExportVias = true;
	ExportTraces = eExportTracesAll;
	sPackageAttribute = "";
   bNetlistUseDftPinAccess = true;
   bExportTestpointsAsComponents = false;
   bAnnotateTestpoints = false;

   if ((fp = fopen(settingsFile, "rt")) == NULL)
   {
      // no settings file found 
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      ErrorMessage(tmp, "SPEA4040 Settings", MB_OK | MB_ICONHAND);

      return;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".DEVICE_TYPE_MAP"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

				CString deviceType = lp;
				deviceType.MakeUpper().Trim();

            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

				CString deviceCode = lp;
				deviceCode.Trim();

				if (deviceType != "" && deviceCode != "")
					sDeviceTypeMap.SetAt(deviceType, deviceCode);
         }
			else if (!STRICMP(lp, ".EXPORT_VIAS"))
			{
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

				if (toupper(lp[0]) == 'N')
					bExportVias = false;
			}
			else if (!STRICMP(lp, ".EXPORT_TESTPOINTS_AS_COMPONENTS"))
			{
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

				bExportTestpointsAsComponents = (toupper(lp[0]) == 'Y');
			}
			else if (!STRICMP(lp, ".ANNOTATE_TESTPOINTS"))  // Intended for development/debugging, could be used by customer but Y makes invalid output.
			{
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

				bAnnotateTestpoints = (toupper(lp[0]) == 'Y');
			}
         else if (!STRICMP(lp, ".EXPORT_TRACES"))
         {
            lp = get_string(NULL, " \t\n");

				if (lp == NULL)
					continue;

            if (toupper(lp[0]) == 'N')
					ExportTraces = eExportTracesNone;
				else if (toupper(lp[0]) == 'A')
					ExportTraces = eExportTracesAll;
				else if (toupper(lp[0]) == 'O')
					ExportTraces = eExportTracesOuter;
         }
			else if (!STRICMP(lp, ".PACKAGE_ATTRIBUTE"))
			{
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

				sPackageAttribute = lp;
			}
			else if (!STRICMP(lp, ".NETLIST_PIN_ACCESS"))
			{
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

				CString val(lp);
            if (val.CompareNoCase("CAD") == 0)
               bNetlistUseDftPinAccess = false;
			}
		}
	}

	fclose(fp);
}

/******************************************************************************
*/

SpeaPNData::SpeaPNData()
: caseCode(0)
, compXsize(0.)
, compYsize(0.)
{
}

SpeaPNData::SpeaPNData(CString partnum, CString pkgname, CString devcode, CString devname, CString val, CString plustol, CString minustol, 
                           int case_code, double xsize, double ysize)
: partNumber(partnum)
, packageName(pkgname)
, deviceTypeCode(devcode)
, deviceName(devname)
, value(val)
, tolPos(plustol)
, tolNeg(minustol)
, caseCode(case_code)
, compXsize(xsize)
, compYsize(ysize)
{
}

/******************************************************************************
* GenerateViaInfo
*/
static void GenerateViaInfo(CCEtoODBDoc *doc, FileStruct *file)
{
	WORD netnameKey = doc->IsKeyWord(ATT_NETNAME, 0);

	int viaCount = 0;

	POSITION pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = file->getBlock()->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypeVia || insert->getInsertType() == insertTypeTestPad)
			continue;

		CString refname = insert->getRefname();

		if (refname.IsEmpty())
		{
			viaCount++;
			refname.Format("via%d", viaCount);

			// Assign the via name to refname
			insert->setRefname(STRDUP(refname.GetBuffer(0)));
		}

		// Get net name
		Attrib *attrib = NULL;

		if (data->getAttributes() && data->getAttributes()->Lookup(netnameKey, attrib))
		{
			void *voidPtr = NULL;
			CString netname = attrib->getStringValue();

			CMapStringToPtr *viaMap = NULL;

			if (!sNetViaListMap.Lookup(netname, (void*&)viaMap))
			{
				viaMap = new CMapStringToPtr();
				sNetViaListMap.SetAt(netname, viaMap);
			}

			viaMap->SetAt(refname, data);
		}
	}
}

/******************************************************************************
* GenerateAccessibleViaMap
*/
static void GenerateAccessibleViaMap(CCEtoODBDoc *doc, FileStruct *file)
{
	// ** Always call this function after GenerateViaInfo() to make sure via names are already created

	WORD dataLinkKey = (WORD)doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);
	sAccessibleViaMap.RemoveAll();

	int viaCount = 0;
	POSITION pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		// Get the access point
		DataStruct *data = file->getBlock()->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypeTestAccessPoint || data->getAttributes() == NULL)
			continue;

		// Get the entity number link to the via
		Attrib *attrib = NULL;

		if (!data->getAttributes()->Lookup(dataLinkKey, attrib) || attrib == NULL)
			continue;

		//EEntityType entityType;
		int entityNum = attrib->getIntValue();
		//void *voidPtr = FindEntity(doc, entityNum, entityType);
      CEntity entity = CEntity::findEntity(doc->getCamCadData(),entityNum);

		//if (entityType != entityTypeData)
      if (entity.getEntityType() != entityTypeData)
			continue;

		// Get the via
		//data = (DataStruct*)voidPtr;
      data = entity.getData();

		if (data == NULL || data->isInsertType(insertTypeVia))
			continue;

		// Get via name and add to map
		CString viaName = data->getInsert()->getRefname();

		if (!viaName.Trim().IsEmpty())
			sAccessibleViaMap.SetAt(viaName, viaName);
	}
}

/******************************************************************************
* GetFiducialName
*/
static CString GetFiducialName(int entityNum)
{
	CString fidName;
	CString entity;

	entity.Format("%d", entityNum);

	if (!sFiducialNameMap.Lookup(entity, fidName))
	{
		fidName.Format("FID%d", sFiducialNameMap.GetCount() + 1);
		sFiducialNameMap.SetAt(entity, fidName);
	}

	return fidName;
}

/******************************************************************************
* GetValidPackageName
*/
static CString GetValidPackageName(CString packageName, CMapStringToString* packageRenameMap)
{
	if (packageRenameMap == NULL)
		return packageName;

	// This map is used to keep track of unique renamed package name
	// Only reset it when the packageRenameMap is new with zero count
	static int count;
	static CMapStringToString usedRenamedMap;

	if (packageRenameMap->GetCount() == 0)
	{
		count = 0;
		usedRenamedMap.RemoveAll();
	}

	CString rename;

	if (!packageRenameMap->Lookup(packageName, rename))
	{
		packageName.Replace("-", "_");

		if (packageName.GetLength() > 30)
		{
			// First truncate packageName to 30 character
			CString numberStr;
			CString truncatedName = packageName;
			truncatedName.Truncate(30);

			// If after truncate and the newly truncate package name is already used
			// then create unique truncated name by appending a number
			while (usedRenamedMap.Lookup(truncatedName, rename))
			{
				numberStr.Format("%d", count++);
				truncatedName = packageName;
				truncatedName.Truncate(30 - numberStr.GetLength());
				truncatedName.AppendFormat("%s", numberStr);
			}

			rename = truncatedName;
			fprintf(speaLogFp, "Package Name was Truncated: " + packageName + "-->" + rename + "\n");
		}
		else
		{
			rename = packageName;
		}
		
		usedRenamedMap.SetAt(rename, rename);
		packageRenameMap->SetAt(packageName, rename);
	}

	return rename;
}

/******************************************************************************
* GenerateFiducialNameMap
*/
static void GenerateFiducialNameMap(CCEtoODBDoc *doc, FileStruct *file)
{
	POSITION pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = file->getBlock()->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypeFiducial)
			continue;

		CString fidName = GetFiducialName(data->getEntityNumber());
	}
}

/******************************************************************************
* MapBlockToPackageName
*/
static void MapBlockToPackageName(CCEtoODBDoc *doc, FileStruct *file, CMapStringToString* packageNameMap)
{
	// User specify attribute to use as package, so loop throught all the insert to gather all teh package name
	// since user only apply the attribute at the insert level
	// If different package name is found for the same geometry, then only use the first one and report 
	// subsequence ones in log file
	if (sPackageAttribute.IsEmpty() == false)
	{
		WORD packageNameKey = (WORD)doc->IsKeyWord(sPackageAttribute, 0);

		if (packageNameKey > -1)
		{
			POSITION pos = file->getBlock()->getHeadDataInsertPosition();
			while (pos)
			{
				DataStruct *data = file->getBlock()->getNextDataInsert(pos);

				if (data == NULL || data->getInsert() == NULL || data->getAttributes() == NULL)
					continue;

				Attrib *attrib = NULL;
				data->getAttributes()->Lookup(packageNameKey, attrib);

				if (attrib == NULL || attrib->getValueType() != valueTypeString)
					continue;

				CString packageName = attrib->getStringValue();

				if (packageName.IsEmpty() == true)
					continue;

				CString prePackageName;
				BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());

				if (packageNameMap->Lookup(block->getName(), prePackageName))
				{
					// Geometry already rename, additional rename found so output to error log and ignore it
					CString errMessage;
					errMessage.Format("PACKAGES:  package (%s) already renamed to (%s), additional rename (%s) found and ignored\n", block->getName(), prePackageName, packageName);
					fprintf(speaLogFp, errMessage);
					sDisplayErr++;
				}
				else
				{
					// Add name to the map
					packageNameMap->SetAt(block->getName(), packageName);
				}
			}
		}
	}
}

static BlockStruct *getFileBlock(FileStruct *file, CString sectionName)
{
	if (file != NULL && file->getBlock() != NULL)
	{
		return file->getBlock();
	}

	fprintf(speaLogFp, "%s:  Board does not exist. Nothing to export.\n", sectionName);
	sDisplayErr++;

	return NULL;
}

/******************************************************************************
* WriteBoardInfo
*/
static void WriteBoardInfo(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, int pageUnits)
{
   CTime currentTime = CTime::GetCurrentTime();
	BlockStruct *block = getFileBlock(file, "BOARDINFO");

	if (!block)
		return;

	doc->CalcBlockExtents(block);
	CExtent extent = block->getExtent();
	double minX = (double)block->getXmin();
	double minY = (double)block->getYmin();
	double maxX = (double)block->getXmax();
	double maxY = (double)block->getYmax();

	// Print board information
	fprintf(fp, ":BOARDINFO\n\n");
	fprintf(fp, "%s ,", file->getName());								// Board Name	
	fprintf(fp, " ,");												// Part Number - leave it blank per specification
	fprintf(fp, "%.3f ,", minX);									// Minimum X
	fprintf(fp, "%.3f ,", minY);									// Minimum Y
	fprintf(fp, "%.3f ,", maxX);									// Maximum X
	fprintf(fp, "%.3f ,", maxY);									// Maximum Y
	fprintf(fp, "%s ,", currentTime.Format("%m/%d/%y"));	// Creation Date
	fprintf(fp, " ,");												// Board Release - leave it blank per specification

	switch (pageUnits)												// Measure Unit
	{
	case UNIT_INCHES:
		fprintf(fp, "INCH ,");
		break;
	case UNIT_MILS:
		fprintf(fp, "MILS ,");
		break;
	case UNIT_MM:
		fprintf(fp, "MM ,");
		break;
	default:
		fprintf(fp, " ,");
		fprintf(speaLogFp, "BOARDINFO:  Measure units is not allowed, leave as blank\n");
		sDisplayErr++;
		break;
	}

	// Get the board thichness
	WORD keyword = (WORD)doc->IsKeyWord(BOARD_THICKNESS, 0);
	Attrib *attrib = NULL;

	if (block->getAttributesRef() && block->getAttributesRef()->Lookup(keyword, attrib))
	{
		if (attrib)
			fprintf(fp, "%.3f ,", attrib->getDoubleValue());			// Thickness
		else
			fprintf(fp, "%s", "");									// Thickness
	}
	else
	{
		fprintf(fp, "%s", "");										// Thickness
	}

	// Get number of eletrical layers
	int layers = 0;

	for (int i=0; i<doc->getMaxLayerIndex(); i++)
	{
		LayerStruct *layer = doc->getLayerArray()[i];

		if (layer && layer->getElectricalStackNumber() >= 0)
			layers++;
	}

	if (layers == 0)
		layers = 2;

	fprintf(fp, "%d\n", layers);									// Layers
	fprintf(fp, "\n:ENDBOARDINFO\n\n");
}

/******************************************************************************
*/
static void WritePartlistSection(FILE *fp)
{
   fprintf(fp, ":PARTLIST\n\n");

   // Write actual parts.
   for (int i = 0; i < PartArray.GetCount(); i++)
   {
      SpeaPart *part = PartArray.GetAt(i);
      WritePart(fp, part);
   }

   // Optionally write testpoints as parts.
   if (bExportTestpointsAsComponents)
   {
      for (int i = 0; i < TestpointArray.GetCount(); i++)
      {
         SpeaTestpoint *tp = TestpointArray.GetAt(i);
         WritePart(fp, tp);
      }
   }

   fprintf(fp, "\n:ENDPARTLIST\n\n");
}

/******************************************************************************
*/
static void WritePart(FILE *fp, SpeaPart *part)
{
   if (part != NULL)
   {
      WritePart(fp,
         part->m_partNumber,
         part->m_variantNumber,
         part->m_refname,
         part->m_x,
         part->m_y,
         part->m_mountSide,
         part->m_rotation,
         ""
      );
   }
}

/******************************************************************************
*/
static void WritePart(FILE *fp, SpeaTestpoint *testpoint)
{
   if (testpoint != NULL)
   {
      CString annotation( bAnnotateTestpoints ? " testpoint" : "" );

      // From Mark: Change second field in PN Data to be TP_CompPin_PinName, eg via_64-1 becomes TP_via_64_1.
      // That's the refname, and he means PARTLIST section. We know that this means these refnames come
      // out different for .EXPORT_TESTPOINTS_AS_COMPONENTS Y and N modes. In N mode it still comes out
      // the old way.
      CString refname;
      refname.Format("TP_%s_%s", testpoint->m_targetRefname, testpoint->m_targetPinName);

      WritePart(fp,
         TESTPOINT_PARTNUMBER,   // partNumber
         0,          // variantNumber
         refname,
         testpoint->m_testAccessMarkX,
         testpoint->m_testAccessMarkY,
         testpoint->m_contactSide,
         0,          // rotation
         annotation
      );
   }
}

/******************************************************************************
*/
static void WritePart(FILE *fp, CString partNumber, int variantNumber, CString drawingRef, double x, double y, char mountSide, int rotation, CString annotation)
{
   // If there is no part number, then output this line of data as a comment.
   // Also write to the log file indicating part number is missing.

   if (partNumber.IsEmpty())
   {
      fprintf(fp, ";");                   // Add the comment character to make this line of data a comment           

      fprintf(speaLogFp, "PARTLIST:  [%s] is missing part number\n", drawingRef);
      sDisplayErr++;

      partNumber = "PNxx";             // Indicates missing partnumber
   }

   fprintf(fp, "%d ,", variantNumber);
   fprintf(fp, "%s ,", check_name('c', drawingRef));

   CString tmpCStr = check_name('b', partNumber);
   CString tmpPartNumber = tmpCStr;
   if(-1 != tmpCStr.Find(','))
      tmpPartNumber.Format("\"%s\"", tmpCStr);
   fprintf(fp, "%s ,", tmpPartNumber);

   if (x != DOUBLE_NOT_SET && y != DOUBLE_NOT_SET)
   {
      fprintf(fp, "%.3f ,", x);
      fprintf(fp, "%.3f ,", y);
   }
   else
   {
      fprintf(fp, " , ,");
   }

   fprintf(fp, "%c,", mountSide);
   fprintf(fp, "%d%s\n", rotation, annotation);
}

/******************************************************************************
*/
static void GatherParts(CCEtoODBDoc *doc, FileStruct *file, bool panelDesign)
{
	BlockStruct *block = getFileBlock(file, "PARTLIST");

	if (block == NULL)
		return;

	WORD testPointKey = doc->IsKeyWord(ATT_TEST, 0);
	WORD partNumKey = doc->IsKeyWord(ATT_PARTNUMBER, 0);
	WORD loadedKey = doc->IsKeyWord(ATT_LOADED, 2);
   int variantNumber = -1;
   POSITION variantPos=file->getVariantList().GetHeadPosition();
   do
   {
      ++variantNumber;
      CVariant* variant = NULL;
      if(NULL != variantPos)
         variant = file->getVariantList().GetNext(variantPos);
      POSITION pos = block->getHeadDataInsertPosition();      
      while (pos)
      {
         DataStruct *data = block->getNextDataInsert(pos);
         InsertStruct *insert = data->getInsert();

         InsertTypeTag insertType = insert->getInsertType();
         bool isVia    = (insertType == insertTypeVia);
         bool isComp   = (insertType == insertTypePcbComponent);
         bool isFid    = (insertType == insertTypeFiducial);

         if (!isVia && !isComp && !isFid)
         {
            continue;
         }

         if (isVia)
         {
            CString viaName;
            if (bExportVias == false && !sAccessibleViaMap.Lookup(insert->getRefname(), viaName))
               continue;
         }

         CString drawingRef;
         CString partNumber;

         if (isVia)
         {
            drawingRef = insert->getRefname();

            BlockStruct *subblock = doc->getBlockAt(insert->getBlockNumber());

            if (subblock)
               partNumber = subblock->getName();
         }
         else
         {
            if (isFid)
               drawingRef = GetFiducialName(data->getEntityNumber());
            else
               drawingRef = insert->getRefname();

            Attrib *attrib = NULL;

            if (data->getAttributes() && data->getAttributes()->Lookup(partNumKey, attrib))
            {
               if (attrib)
                  partNumber = attrib->getStringValue();
            }
            else if (data->getAttributesRef() && data->getAttributesRef()->Lookup(testPointKey, attrib))
            {
               BlockStruct *subblock = doc->getBlockAt(insert->getBlockNumber());

               if (subblock)
                  partNumber = subblock->getName();
            }

            // Case 1747, make up a part number if there isn't one
            if (isFid && partNumber.IsEmpty())
               partNumber = "PNFID";
         }

         CVariantItem* variantItem = NULL;
         if(NULL != variant)
         {
            variantItem = variant->FindItem(drawingRef);
            Attrib *attrib = NULL;
            if((NULL != variantItem) 
               && variantItem->getAttributes() 
               && variantItem->getAttributes()->Lookup(partNumKey, attrib))
            {
               if (attrib)
                  partNumber = attrib->getStringValue();
            }
         }

         // Get mount side
         Attrib *attrib = NULL;
         BOOL loaded = TRUE;

         if((NULL != variantItem)
            && variantItem->getAttributes() 
            && variantItem->getAttributes()->Lookup(loadedKey, attrib))
         {
            if (attrib)
            {
               CString value = attrib->getStringValue();
               value.MakeUpper().Trim();

               if (value == "FALSE")
                  loaded = FALSE;
            }
         }
         else if (data->getAttributes() && data->getAttributes()->Lookup(loadedKey, attrib))
         {
            if (attrib)
            {
               CString value = attrib->getStringValue();
               value.MakeUpper().Trim();

               if (value == "FALSE")
                  loaded = FALSE;
            }
         }

         char mountSide = '\0';

         if (loaded)
         {
            mountSide = 'T';

            // Per Mark's intruction, the code is changed from checking the mirror flag to checking the place bottom
            //if (data->getInsert()->getMirrorFlags() & MIRROR_ALL)
            if (data->getInsert()->getPlacedBottom())
               mountSide = 'B';
         }
         else
         {
            mountSide = 'P';

            // Per Mark's intruction, the code is changed from checking the mirror flag to checking the place bottom
            //if (data->getInsert()->getMirrorFlags() & MIRROR_ALL)
            if (data->getInsert()->getPlacedBottom())
               mountSide = 'M';
         }

         // Get rotation
         int rotation = (int)normalizeDegrees(round(insert->getAngleDegrees()));

         double locX = DOUBLE_NOT_SET;
         double locY = DOUBLE_NOT_SET;

         if (isVia || isFid)
         {
            locX = insert->getOriginX();
            locY = insert->getOriginY();
         }
         else
         {
            // Get the centroid X & Y of the insert
            double centroidX = 0.0;
            double centroidY = 0.0;

            if (GetInsertCentroid(doc, data, centroidX, centroidY) > 0)
            {
               Point2 point;
               DTransform xform(insert->getOriginX(), insert->getOriginY(), insert->getScale(), insert->getAngle(), insert->getMirrorFlags());
               point.x = centroidX;
               point.y = centroidY;
               xform.TransformPoint(&point);

               // There is centroid
               locX = point.x;
               locY = point.y;
            }
            // else locX and Y remain not set, that is okay.
         }

         SpeaPart *part = new SpeaPart(insertType, partNumber, variantNumber, drawingRef, locX, locY, mountSide, rotation);
         PartArray.Add(part);
      }
   }
   while((NULL != variantPos) && !panelDesign);

}

/******************************************************************************
* GetInsertCentroid
*/
static int GetInsertCentroid(CCEtoODBDoc *doc, DataStruct *data, double &x, double &y)
{
	BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());

	if (!block)
		return 0;

	DataStruct *centroidData = block->GetCentroidData();

	if (centroidData == NULL)
		return 0;

	InsertStruct *insert = centroidData->getInsert();
		
	x = insert->getOriginX() + block->getXoffset();
	y = insert->getOriginY() + block->getYoffset();

	return 1;
}


/******************************************************************************
*/
static int getPadCode(CCEtoODBDoc *doc, int padstackBlockNumber, ETestSurface& mountSide)
{
   if (doc != NULL && padstackBlockNumber >= 0)
   {
      // Get Pad Code by finding the first top pad
      BlockStruct *bs = doc->getBlockAt(padstackBlockNumber);

      if (bs != NULL)
      {
         for ( int surface = testSurfaceTop; surface < testSurfaceBoth; surface++)
         {
            POSITION dataPos = bs->getHeadDataInsertPosition();
            while (dataPos)
            {
               DataStruct *ds = bs->getNextDataInsert(dataPos);
               int padBlockNumber = ds->getInsert()->getBlockNumber();
               BlockStruct *moreBS = doc->getBlockAt(padBlockNumber);

               if (!moreBS || !moreBS->isAperture())
                  continue;

               short lyrIndex = ds->getLayerIndex();
               LayerStruct *lyrStruct = doc->getLayerAt(lyrIndex);
               LayerTypeTag lyrTypeTag = layerTypeUnknown;
               if (lyrStruct != NULL)			
                  lyrTypeTag = lyrStruct->getLayerType();

               if(surface == testSurfaceTop)
               {
                  // Either specifically "top", or "all", which includes top
                  if (lyrTypeTag == layerTypeSignalTop || lyrTypeTag ==layerTypePadTop ||
                     lyrTypeTag == layerTypeSignalAll || lyrTypeTag ==layerTypePadAll)
                  {
                     mountSide = testSurfaceTop;
                     return padBlockNumber;   // "padCode" is pad block number
                  }
               }
               else
               {
                  if (lyrTypeTag == layerTypeSignalBottom || lyrTypeTag ==layerTypePadBottom)
                  {
                     mountSide = testSurfaceBottom;
                     return padBlockNumber;   // "padCode" is pad block number
                  }
               }
            }
         }         
      }
   }

	return -1;
}

static void WriteNetTestpoints(FILE *fp, CCEtoODBDoc *doc, NetStruct *net, int netNumber)
{
   CString targetNetName( net->getNetName() );

   for (int i = 0; i < TestpointArray.GetCount(); i++)
   {
      SpeaTestpoint *tp = TestpointArray.GetAt(i);

      if (tp->m_netName.CompareNoCase(targetNetName) == 0)
      {
         CString annotation( bAnnotateTestpoints ? " testpoint" : "" );

         // tp->m_targetPinName is the actual comp's pinname that is probed. We do not
         // want that here, we want these to be pin "1" always. These are coerced 
         // single pin components.

         // Keep this in sync with Writepart(testpoint). Should make a testpoint get func that
         // returns this formatted name, to use for both places.
         CString refname;
         refname.Format("TP_%s_%s", tp->m_targetRefname, tp->m_targetPinName);

         if (tp->m_padCode != SPEA_NO_PAD_CODE)  // skip entries with no pad, related to dts0100564212
         {
            fprintf(fp, "%d ,",   netNumber);					                  // Net Number
            fprintf(fp, "%s ,",   check_name('n', net->getNetName()));	      // Net Name
            fprintf(fp, "%s ,",   check_name('c', refname));                  // Drawing Reference
            fprintf(fp, "%s ,",   check_name('p', "1"));                      // Pin Name     (NOT tp->m_targetPinName ON PURPOSE)
            fprintf(fp, "%.3f ,", tp->m_testAccessMarkX);                     // X Position
            fprintf(fp, "%.3f ,", tp->m_testAccessMarkY);                     // Y Position
            fprintf(fp, "%c,",    tp->m_contactSide);                         // Pin Access Side
            fprintf(fp, "%d%s\n", tp->m_padCode, annotation);                 // Pad Code
         }
      }
   }
}

static void WriteNetVias(FILE *fp, CCEtoODBDoc *doc, NetStruct *net, int netNumber)
{
   WORD testAccessKey = (WORD)doc->RegisterKeyWord(ATT_TESTACCESS, 0, VT_STRING);
   CMapStringToPtr *viaMap = NULL;

   if (sNetViaListMap.Lookup(net->getNetName(), (void*&)viaMap))
   {
      POSITION viaPos = viaMap->GetStartPosition();
      while (viaPos)
      {
         CString viaName;
         DataStruct *via = NULL;

         viaMap->GetNextAssoc(viaPos, viaName, (void*&)via);

         if (via->getDataType() != dataTypeInsert)
            continue;

         InsertStruct *insert = via->getInsert();

         if (insert->getInsertType() != insertTypeVia)
            continue;
         else
         {
            CString viaName;

            if (bExportVias == false && 
               bNetlistUseDftPinAccess &&
               !sAccessibleViaMap.Lookup(insert->getRefname(), viaName))
            {
               continue;
            }
         }


         // Find via access
         ETestSurface surface = testSurfaceTop;
         int padCode = getPadCode(doc, insert->getBlockNumber(), surface);
         char pinAccessSide = '\0';
         Attrib *attrib = NULL;

         // Case 2074
         if (bNetlistUseDftPinAccess)
         {
            // Pre-case 2074 behavior -- Using DFT or defaulting old style
            if (via->getAttributes() != NULL && via->getAttributes()->Lookup(testAccessKey, attrib) && attrib != NULL)
            {
               CString testAccess = attrib->getStringValue();

               if (testAccess.CompareNoCase(DFT_TESTACCESS_TOP) == 0)
                  pinAccessSide = 'T';
               else if (testAccess.CompareNoCase(DFT_TESTACCESS_BOTTOM) == 0)
                  pinAccessSide = 'B';
               else if (testAccess.CompareNoCase(DFT_TESTACCESS_BOTH) == 0)
                  pinAccessSide = 'A';
               else
                  pinAccessSide = 'N';
            }
            else
            {
               if (insert->getMirrorFlags())
                  pinAccessSide = 'B';
               else
                  pinAccessSide = 'T';
            }
         }
         else
         {
            // Case 2074 -- Using CAD data
            // Via rules, delivered via email by Mark:
            // Via - SMD: T or B depending on whether the pad is top or bottom, not the insert side as vias are always placed top.
            // Via - THRU: A for access All sides
            bool viaIsSmd = true; // presumptuous
            WORD techKw = (WORD)doc->IsKeyWord(ATT_TECHNOLOGY, 0);
            Attrib *attrib = NULL;
            if (via->lookUpAttrib(techKw, attrib))
            {
               CString attrVal = attrib->getStringValue();
               if (attrVal.CompareNoCase("THRU") == 0)
                  viaIsSmd = false;
            }
            if (viaIsSmd)
            {
               pinAccessSide = 'N';
               int insblocknum = via->getInsert()->getBlockNumber();
               BlockStruct *insblock = doc->Find_Block_by_Num(insblocknum);
               if (insblock != NULL)
               {
                  POSITION dataPos = insblock->getHeadDataInsertPosition();
                  while (dataPos && pinAccessSide == 'N')
                  {
                     DataStruct *ds = insblock->getNextDataInsert(dataPos);
                     int blockNumber = ds->getInsert()->getBlockNumber();
                     BlockStruct *deepblk = doc->getBlockAt(blockNumber);

                     if (deepblk != NULL && deepblk->isAperture())
                     {

                        short lyrIndex = ds->getLayerIndex();
                        LayerStruct *lyrStruct = doc->getLayerAt(lyrIndex);
                        LayerTypeTag lyrTypeTag = layerTypeUnknown;
                        if (lyrStruct != NULL)
                           lyrTypeTag = lyrStruct->getLayerType();

                        if (lyrTypeTag == layerTypeSignalTop || lyrTypeTag ==layerTypePadTop)
                        {
                           padCode = blockNumber;
                           if (via->getInsert()->getPlacedTop())
                              pinAccessSide = 'T';
                           else
                              pinAccessSide = 'B';
                        }
                        else if (lyrTypeTag == layerTypeSignalBottom || lyrTypeTag ==layerTypePadBottom)
                        {
                           padCode = blockNumber;
                           if (via->getInsert()->getPlacedTop())
                              pinAccessSide = 'B';
                           else
                              pinAccessSide = 'T';
                        }
                     }
                  }
               }
            }
            else
            {
               // THRU -- Access all sides
               pinAccessSide = 'A';
            }
         }			

         if (padCode != SPEA_NO_PAD_CODE)  // skip entries with no pad, related to dts0100564212
         {
            fprintf(fp, "%d ,", netNumber);					// Net Number
            fprintf(fp, "%s ,", check_name('n', net->getNetName()));	 // Net Name
            fprintf(fp, "%s ,", check_name('c', viaName));            // Drawing Reference
            fprintf(fp, "1 ,");									// Pin Name
            fprintf(fp, "%.3f ,", insert->getOriginX());	// X Position
            fprintf(fp, "%.3f ,", insert->getOriginY());	// Y Position
            fprintf(fp, "%c,", pinAccessSide);				// Pin Access Side
            fprintf(fp, "%d\n", padCode);						// Pad Code
         }
      }
   }
}

/******************************************************************************
* WriteNetlist
*/
static void WriteNetlist(FILE *fp, CCEtoODBDoc *doc, FileStruct *file)
{
	if (getFileBlock(file, "NETLIST") == NULL)
		return;

	fprintf(fp, ":NETLIST\n\n");

	WORD pinMapKey = (WORD)doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
	WORD testAccessKey = (WORD)doc->RegisterKeyWord(ATT_TESTACCESS, 0, VT_STRING);

	int netCount = 0;

	POSITION pos = file->getNetList().GetHeadPosition();
	while (pos)
	{
		NetStruct *net = file->getNetList().GetNext(pos);

		if (!net)
			continue;

		// Generate a sequencial number for net that is not NETFLAG_SINGLEPINNET
		int netNumber = 0;

		if (!(net->getFlags() & NETFLAG_SINGLEPINNET))
			netNumber = netCount++;

		POSITION comppinPos = net->getHeadCompPinPosition();
		while (comppinPos)
		{
			CompPinStruct *comppin = net->getNextCompPin(comppinPos);

			if (!comppin)
				continue;

			// Find the pin access side
			char pinAccessSide = '\0';
			Attrib *attrib = NULL;

         // Case 2074
         if (bNetlistUseDftPinAccess)
         {
            // bNetlistUseDftPinAccess == true delivers "standard" (backward compatible) results,
            // ie. same results as before case 2074.
            if (comppin->getAttributes() != NULL && comppin->getAttributes()->Lookup(testAccessKey, attrib) && attrib != NULL)
            {
               CString testAccess = attrib->getStringValue();

               if (testAccess.CompareNoCase(DFT_TESTACCESS_TOP) == 0)
                  pinAccessSide = 'T';
               else if (testAccess.CompareNoCase(DFT_TESTACCESS_BOTTOM) == 0)
                  pinAccessSide = 'B';
               else if (testAccess.CompareNoCase(DFT_TESTACCESS_BOTH) == 0)
                  pinAccessSide = 'A';
               else
                  pinAccessSide = 'N';
            }
            else
            {
               switch (comppin->getVisible())
               {
               case VISIBLE_TOP:
                  pinAccessSide = 'T';
                  break;
               case VISIBLE_BOTTOM:
                  pinAccessSide = 'B';
                  break;
               case VISIBLE_BOTH:
                  pinAccessSide = 'A';
                  break;
               case VISIBLE_NONE:
               default:
                  pinAccessSide = 'N';
                  break;
               }
            }
         }
         else
         {
            // Case 2074, Use CAD info only for pin access in netlist output
            // Simple rules: 
            // If SMD then access is same side as insert (output T for top insert and B for bottom insert).
            // If THRU then access is opposite side of insert (output B for top insert and T for bottom insert).
            // CAMCAD's notion of pin visibility or accessibility is irrelevant, to be ignored.

            pinAccessSide = 'N';
            CString refname = comppin->getRefDes();
            if (!refname.IsEmpty())
            {
               DataStruct *compdata = file->FindInsertData(refname);
               if (compdata != NULL && compdata->getInsert() != NULL)
               {
                  WORD techKw = (WORD)doc->IsKeyWord(ATT_TECHNOLOGY, 0);
                  Attrib *attrib = NULL;
                  if (compdata->lookUpAttrib(techKw, attrib))
                  {
                     CString attrVal = attrib->getStringValue();

                     if (attrVal.CompareNoCase("THRU") == 0)
                     {
                        // THRU -- Access opposite side from insert
                        if (compdata->getInsert()->getPlacedTop())
                           pinAccessSide = 'B';
                        else
                           pinAccessSide = 'T';
                     }
                     else
                     {
                        // SMD -- Access same side as insert
                        if (compdata->getInsert()->getPlacedTop())
                           pinAccessSide = 'T';
                        else
                           pinAccessSide = 'B';
                     }

                  }
                  else
                  {
                     // No TECHNOLOGY attrib
                     fprintf(speaLogFp, "NETLIST Pin Access Error: No TECHNOLOGY attribute on \"%s\"\n", refname);
                  }
               }
               else
               {
                  // No insert with refname
                  fprintf(speaLogFp, "NETLIST Pin Access Error: No component insert matches CompPin \"%s\"\n", refname);
               }
            }
            else
            {
               // Comp pin has blank refname
               fprintf(speaLogFp, "NETLIST Pin Access Error: CompPin on net \"%s\" has blank Refname\n", net->getNetName());             
            }

         }

			CString pinName = comppin->getPinName();
			CString pinFunction;
			
			attrib = NULL;

			if (comppin->getAttributes() && comppin->getAttributes()->Lookup(pinMapKey, attrib))
				pinFunction = attrib->getStringValue();

			if (pinFunction.CompareNoCase("positive") == 0)
				pinName = "P";
			else if (pinFunction.CompareNoCase("negative") == 0)
				pinName = "N";
			else if (pinFunction.CompareNoCase("anode") == 0)
				pinName = "A";
			else if (pinFunction.CompareNoCase("cathode") == 0 || pinFunction.CompareNoCase("catode") == 0)
				pinName = "K";
			else if (pinFunction.CompareNoCase("base") == 0)
				pinName = "B";
			else if (pinFunction.CompareNoCase("collector") == 0)
				pinName = "C";
			else if (pinFunction.CompareNoCase("emitter") == 0)
				pinName = "E";
			else if (pinFunction.CompareNoCase("drain") == 0)
				pinName = "D";
			else if (pinFunction.CompareNoCase("source") == 0)
				pinName = "S";
			else if (pinFunction.CompareNoCase("gate") == 0)
				pinName = "G";
			else if (pinFunction.CompareNoCase("anode 1") == 0)
				pinName = "A1";
			else if (pinFunction.CompareNoCase("anode 2") == 0)
				pinName = "A2";
			else if (pinFunction.CompareNoCase("1") == 0)
				pinName = "1";
			else if (pinFunction.CompareNoCase("2") == 0)
				pinName = "2";
			else if (pinFunction.CompareNoCase("cursor") == 0)
				pinName = "CR";
			else if (pinFunction.CompareNoCase(ATT_VALUE_NO_CONNECT)	 == 0)// Do not output if pin function is "no connect"
				continue;

         ETestSurface surface = testSurfaceTop;
			int padCode = getPadCode(doc, comppin->getPadstackBlockNumber(), surface);

         if (padCode != SPEA_NO_PAD_CODE)  // skip entries with no pad, related to dts0100564212
         {
            fprintf(fp, "%d ,", netNumber);									// Net Number
            fprintf(fp, "%s ,", check_name('n', net->getNetName()));	    // Net Name
            fprintf(fp, "%s ,", check_name('c', comppin->getRefDes()));  // Drawing Reference
            fprintf(fp, "%s ,", check_name('p', pinName));				    // Pin Name
            fprintf(fp, "%.3f ,", comppin->getOriginX());				// X Position
            fprintf(fp, "%.3f ,", comppin->getOriginY());				// Y Position
            fprintf(fp, "%c,", pinAccessSide);								// Pin Access Side
            fprintf(fp, "%d\n", padCode);	                           // Pad Code
         }
      }

      WriteNetVias(fp, doc, net, netNumber);

      // Optionally write testpoints
      if (bExportTestpointsAsComponents)
      {
         WriteNetTestpoints(fp, doc, net, netNumber);
      }
   }

	// Write fiducial last
	pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = file->getBlock()->getNextDataInsert(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypeFiducial)
			continue;

		CString fidName = GetFiducialName(data->getEntityNumber());

		// Find fiducial access
		char pinAccessSide = '\0';

		if (insert->getMirrorFlags())
			pinAccessSide = 'B';
		else
			pinAccessSide = 'T';

      ETestSurface surface = testSurfaceTop;
      int padCode = getPadCode(doc, insert->getBlockNumber(), surface);

		fprintf(fp, "%d ,", netCount++);					// Net Number
		fprintf(fp, "#%s ,", fidName);					// Net Name
		fprintf(fp, "%s ,", fidName);						// Drawing Reference
		fprintf(fp, "1 ,");									// Pin Name
		fprintf(fp, "%.3f ,", insert->getOriginX());	// X Position
		fprintf(fp, "%.3f ,", insert->getOriginY());	// Y Position
		fprintf(fp, "%c,", pinAccessSide);				// Pin Access Side
		fprintf(fp, "%d\n", padCode);						// Pad Code
	}

	fprintf(fp, "\n:ENDNETLIST\n\n");
}

/******************************************************************************
*/
static SpeaPNData* FabricateTestpointPNData(CCEtoODBDoc *doc)
{
   // There are potentially all sorts of different testpoints, different sizes, shapes, etc.
   // We assign all the same partnumber. So here, we can not use any particular one for
   // details, such as size. We are really really just making this PN Data up.

   CString partNumber(TESTPOINT_PARTNUMBER);
   CString packageName(TESTPOINT_PACKAGE_NAME);
   CString deviceTypeCode;
   CString deviceName(TESTPOINT_PARTNUMBER);
   CString value("0");
   CString tolPos("0");
   CString tolNeg("0");
   int caseCode = 15;  // This is what code elsewhere writes for technology=SMD, seems good for testpoint.

   CString deviceType("TEST_POINT");
   CString tmpCode;
   if (sDeviceTypeMap.Lookup(deviceType, tmpCode))
      deviceTypeCode = tmpCode.Trim();
   else
      deviceTypeCode = "800";  // Punt, this is value found in devtype map in shipped sample spea4040.out settings file, as of CCASM1.0.

   // Semi-punt on size. Like I said, this is covering all testpoints regardless of what geom they are.
   // Just pick the pad size of the first testpoint in the list.
   double compXsize = 0.;
   double compYsize = 0.;
   if (TestpointArray.GetCount() > 0)
   {
      SpeaTestpoint *tp = TestpointArray.GetAt(0);
      int padstackBlockNum = tp->m_padCode;
      BlockStruct *padstackBlock = doc->getBlockAt(padstackBlockNum);
      CExtent extent = padstackBlock->getExtent(doc->getCamCadData());
      compXsize = extent.getXsize();
      compXsize = extent.getYsize();
   }

   SpeaPNData *partData = new SpeaPNData(partNumber, packageName, deviceTypeCode, deviceName, value, tolPos, tolNeg, caseCode, compXsize, compYsize);

   return partData;
}

/******************************************************************************
*/
static void WritePartNumberData(FILE *fp, SpeaPNData *partData)
{
   // set to zero if no device type code
   if (partData->deviceTypeCode.IsEmpty())
      partData->deviceTypeCode = "0";

   // use part number if no device name
   if (partData->deviceName.IsEmpty())
      partData->deviceName = partData->partNumber;

   // set to zero if no tolearance was set
   if (partData->tolPos.IsEmpty())
   {
      partData->tolPos = "0";
      partData->tolNeg = "0";
   }

   CString tmpCStr = check_name('b', partData->partNumber);
   CString tmpPartNumber = tmpCStr;
   if(-1 != tmpCStr.Find(','))
      tmpPartNumber.Format("\"%s\"", tmpCStr);

   fprintf(fp, "%s ,", tmpPartNumber);	               // Part Number
   fprintf(fp, "%s ,", partData->deviceTypeCode);     // Device Type Code
   fprintf(fp, "%s ,", partData->deviceName);         // Device Name
   fprintf(fp, "%d ,", partData->caseCode);           // Case Code
   fprintf(fp, "%s ,", partData->value);              // Value
   fprintf(fp, "%s ,", partData->tolPos);             // Tolerance POS
   fprintf(fp, "%s ,", partData->tolNeg);             // Tolerance NEG
   fprintf(fp, "%s\n", partData->packageName);	      // Package Name
}



/******************************************************************************
*/
static void WritePartNumberData(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, CMapStringToString* packageNameMap, CMapStringToString* packageRenameMap)
{
   // Someday this could use separating into a GetherPartNumberData and WritePartNumberData. 

	if (getFileBlock(file, "Part Number Data") == NULL)
		return;

	fprintf(fp, ":PNDATA\n\n");

	CMapStringToPtr partNumMap;

	Attrib *attrib = NULL;
	WORD deviceStyleKey = (WORD)doc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);
	WORD partNumKey = (WORD)doc->IsKeyWord(ATT_PARTNUMBER, 0);
	WORD deviceTypeKey = (WORD)doc->IsKeyWord(ATT_DEVICETYPE, 0);
	WORD valueKey = (WORD)doc->RegisterKeyWord(ATT_VALUE, 0, VT_STRING);
	WORD technologyKey = (WORD)doc->RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);
	WORD toleranceKey = (WORD)doc->IsKeyWord(ATT_TOLERANCE, 0);
	WORD plusTolKey = (WORD)doc->IsKeyWord(ATT_PLUSTOLERANCE, 0);
	WORD minusTolKey = (WORD)doc->IsKeyWord(ATT_MINUSTOLERANCE, 0);
	WORD compHeightKey = (WORD)doc->IsKeyWord(ATT_COMPHEIGHT, 0);

	POSITION pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *data = file->getBlock()->getNextDataInsert(pos);

		if (data == NULL || data->getInsert() == NULL || data->getAttributes() == NULL)
			continue;

		// Case 1879, override exportVias setting if via has an access marker
		CString notImportant;
		if (data->isInsertType(insertTypeVia) && bExportVias == false &&
			!sAccessibleViaMap.Lookup(data->getInsert()->getRefname(), notImportant))
			continue;		

		BlockStruct* subBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());

		if (subBlock == NULL)
			continue;

		CString partNumber;
		CString packageName;
		double compXsize = -1.0;
		double compYsize = -1.0;
		double compZsize = -1.0;

		if (data->isInsertType(insertTypeVia))
		{
			// Get PART NUMBER
			partNumber = subBlock->getName();

			// Get X and Y DEIMENTION
			double topX = 0.0;
			double botX = 0.0;
			double drillSize = 0.0;

			POSITION pos = subBlock->getHeadDataInsertPosition();
			while (pos)
			{
				DataStruct *pad = subBlock->getNextDataInsert(pos);
				BlockStruct *padShape = doc->getBlockAt(pad->getInsert()->getBlockNumber());
				LayerStruct *layer = doc->getLayerArray()[pad->getLayerIndex()];
				
				if (padShape != NULL)
				{
					// Get the drill size, keep largest size encountered.
					if (padShape->getBlockType() == BLOCKTYPE_DRILLHOLE && padShape->getToolSize() > drillSize)
					{
						drillSize = padShape->getToolSize();
					}
               else if (padShape->getShape() != T_COMPLEX)  // Skip if the padshape is a complex aperture
               {
                  // Get the largest pad size for both pad top and bottom layers.
                  if (layer != NULL && layer->getLayerType() == LAYTYPE_PAD_BOTTOM && padShape->getSizeA() > botX)
                     botX = (double)padShape->getSizeA();
                  else if (layer != NULL && layer->getLayerType() == LAYTYPE_PAD_TOP && padShape->getSizeA() > topX)
                     topX = (double)padShape->getSizeA();
               }
				}
			}

			// Set compXsize to size of padshape on pad bottom layer if there is one,
			// else set to size of padshape on pad top layer.
			if (botX > 0.0)
				compXsize = botX;
			else
				compXsize = topX;

			// Set compYsize to drillsize
			compYsize = drillSize;

			// Get PACKAGE NAME, will be used as Z dimension
			packageName = "NO_PACKAGE";
		}
		else
		{
			// Get PART NUMBER, Skip if cannot find Part Number
			if (data->getAttributesRef()->Lookup(partNumKey, attrib))
				partNumber = attrib->getStringValue();

			// Case 1747, except don't skip fids, make up a part number if none present
			if (data->isInsertType(insertTypeFiducial) && partNumber.IsEmpty())
				partNumber = "PNFID";

			if (partNumber.IsEmpty())
				continue;

			// Get X and Y DIMENSION
			// Set compXsize & compYsize to the X & Y extent of package outline respectively
			POSITION pos = subBlock->getDataList().GetHeadPosition();
			while (pos)
			{
				DataStruct *data = subBlock->getDataList().GetNext(pos);

				if (data->getGraphicClass() == GR_CLASS_PACKAGEOUTLINE && data->getDataType() == dataTypePoly)
				{
					CExtent extent = data->getPolyList()->getExtent();
					CSize2d size = extent.getSize();
					compXsize = size.cx;
					compYsize = size.cy;

					break;
				}
			}


			// Get PACKAGE NAME, will be used as Z dimension
			packageName = subBlock->getName();

			if (sPackageAttribute.IsEmpty() == false)
			{
				CString packageRename;

				if (packageNameMap->Lookup(subBlock->getName(), packageRename) && packageRename.IsEmpty() == false)
					packageName = packageRename;
			}

			packageName = GetValidPackageName(packageName, packageRenameMap);

			if (data->isInsertType(insertTypeFiducial) && packageName.IsEmpty())
				packageName = "FIDGEOM";

		}


		// Get DEVICE TYPE CODE
		CString deviceType;

		if (data->getAttributesRef()->Lookup(deviceTypeKey, attrib))
		{
			if (attrib)
			{
				deviceType = attrib->getStringValue();
				deviceType.MakeUpper().Trim();
			}
		}

		CString deviceTypeCode;
		CString tmpCode;

		if (sDeviceTypeMap.Lookup(deviceType, tmpCode))
			deviceTypeCode = tmpCode.Trim();
		else if (data->isInsertType(insertTypeVia))
			deviceTypeCode = "802";
		else if (data->isInsertType(insertTypeFiducial))
			deviceTypeCode = "805";

		// Get DEVICE NAME
		CString deviceName;
		Attrib* attrib = NULL;
		WORD speaKW = doc->RegisterKeyWord(ATT_SPEA4040_SUBCLASS, 0, VT_STRING);

		if (data->getAttributesRef() && data->getAttributesRef()->Lookup(speaKW, attrib))
		{
			CString temp;
			temp = get_attvalue_string(doc,attrib);

			if (!temp.IsEmpty())
				deviceName = temp;
		}
		else if (data->getAttributesRef()->Lookup(deviceStyleKey, attrib))
		{
			if (attrib)
				deviceName = attrib->getStringValue().Trim();
		}

		// Get CASE CODE
		int caseCode = 0;

		if (data->getAttributes()->Lookup(technologyKey, attrib) && attrib != NULL)
		{
			CString techString = attrib->getStringValue().Trim();

			if (techString.CompareNoCase("SMD") == 0)
				caseCode = 15;
			else if (techString.CompareNoCase("THRU") == 0)
				caseCode = 2;
		}

		// Get VALUE
		CString value = "0";

		if (data->getAttributesRef()->Lookup(valueKey, attrib))
		{
			if (attrib)
			{
				value = attrib->getStringValue();
            ComponentValues componentValue(value);

            double numericValue = componentValue.GetValue();

            DeviceTypeTag deviceTypeTag = stringToDeviceTypeTag(deviceType);
            ValueUnitTag valueUnit = getDefaultValueUnitForDeviceType(deviceTypeTag);

            switch (valueUnit)
            {
            case valueUnitOhm:  // Output Value is alway in KOHM
               value = fpfmtExactPrecision(numericValue/1000.,3);
               break;
            case valueUnitFarad:  // Output Value is alway in uF
               value = fpfmtExactPrecision(numericValue * 1000. * 1000.,3);
               break;
            case valueUnitHenry:  // Output Value is alway in mH
               value = fpfmtExactPrecision(numericValue * 1000.,3);
               break;
            default:
               value = fpfmtExactPrecision(numericValue,3);
               break;
            }
			}
		}

		// Get TOLERANCE
		double numericTolPos = -1.0;
		double numericTolNeg = -1.0;

		if (data->getAttributesRef()->Lookup(toleranceKey, attrib) && attrib != NULL)
		{
			numericTolPos = attrib->getDoubleValue();
			numericTolNeg = numericTolPos;
		}

		if (data->getAttributesRef()->Lookup(plusTolKey, attrib) && attrib != NULL)
			numericTolPos = attrib->getDoubleValue();

		if (data->getAttributesRef()->Lookup(minusTolKey, attrib) && attrib != NULL)
			numericTolNeg = attrib->getDoubleValue();

		// Put all the parameter into the partNumMap to be output later
		void* voidPtr = NULL;

		if (numericTolPos < 0.)
			numericTolPos = numericTolNeg = 0.0;

		// Case 1789, format tolerance with no leading zeros, no trailing zeros.
		// But don't give up any precision, i.e. no rounding.
		// If no fractional digits, then get rid of decimal point.
		CString tolPos;
		CString tolNeg;
		tolPos.Format("%f", numericTolPos);
		tolNeg.Format("%f", numericTolNeg);
		tolPos.TrimLeft("0"); // Nuke leading zeros
		tolNeg.TrimLeft("0");
		tolPos.TrimRight("0"); // Nuke trailing zeros
		tolNeg.TrimRight("0");
		tolPos.TrimRight("."); // Nuke decimal pt if no digits to right
		tolNeg.TrimRight(".");
		
		if (partNumMap.Lookup(partNumber, voidPtr))
		{
			SpeaPNData *partData = (SpeaPNData*)voidPtr;

			// Add the value to any of the following fields if they are missing 
         if (partData->deviceTypeCode.IsEmpty())
				partData->deviceTypeCode = deviceTypeCode;

         if (partData->deviceName.IsEmpty())
				partData->deviceName = deviceName;

			if (partData->caseCode == 0)
				partData->caseCode = caseCode;

         if (partData->value.IsEmpty())
				partData->value = value;

         if (partData->tolPos.IsEmpty())
				partData->tolPos = tolPos;

         if (partData->tolNeg.IsEmpty())
				partData->tolNeg = tolNeg;

			if (partData->compXsize == -1.0)
				partData->compXsize = compXsize;

			if (partData->compYsize == -1.0)
				partData->compYsize = compYsize;

         if (partData->packageName.IsEmpty())
				partData->packageName = packageName;
		}
		else
		{
			SpeaPNData *partData = new SpeaPNData(partNumber, packageName, deviceTypeCode, deviceName, value, tolPos, tolNeg, caseCode, compXsize, compYsize);
			partNumMap.SetAt(partNumber, partData);
		}

	}

   // Now write them
	pos = partNumMap.GetStartPosition();
	while (pos)
	{
		CString key;
		void *voidPtr = NULL;
		partNumMap.GetNextAssoc(pos, key, voidPtr);
		SpeaPNData *partData = (SpeaPNData*)voidPtr;
      WritePartNumberData(fp, partData);
      delete partData;
   }

   partNumMap.RemoveAll();

   // If exporting testpoints as components then write a PNDATA for them.
   // Done here to keep it at the end, putting it into the map makes output order random.
   if (bExportTestpointsAsComponents)
   {
      SpeaPNData *partData = FabricateTestpointPNData(doc);
      WritePartNumberData(fp, partData);
      delete partData;
   }

   fprintf(fp, "\n:ENDPNDATA\n\n");
}

/******************************************************************************
*/
static void WriteTestpointSection(FILE *fp, CCEtoODBDoc *doc, FileStruct *file)
{
	if (getFileBlock(file, "TESTPOINT") == NULL)
		return;

	fprintf(fp, ":TESTPOINT\n\n");

   WriteTestpointArray(fp);

	fprintf(fp, "\n:ENDTESTPOINT\n\n");
}

/******************************************************************************
*/
static void WriteTestpointArray(FILE *fp)
{
   for (int i = 0; i < TestpointArray.GetCount(); i++)
   {
      SpeaTestpoint *tp = TestpointArray.GetAt(i);

      fprintf(fp, "%d ,", tp->m_testpointNum);
      fprintf(fp, "%s ,", check_name('c', tp->m_speaTestpointName));
      fprintf(fp, "%s ,", check_name('n', tp->m_netName));
      fprintf(fp, "%s ,", check_name('c', tp->m_targetRefname));
      fprintf(fp, "%s ,", check_name('p', tp->m_targetPinName));
      fprintf(fp, "%s ,", tp->m_testpointCode);
      fprintf(fp, "%d ,", tp->m_contactCode);
      fprintf(fp, "%.3f ,", tp->m_testAccessMarkX);
      fprintf(fp, "%.3f ,", tp->m_testAccessMarkY);
      fprintf(fp, "%c\n", tp->m_contactSide);
   }
}

/******************************************************************************
*/
static void GatherTestpoints(CCEtoODBDoc *doc, FileStruct *file)
{
	int testpointNum = 0;
	CMapInt accessMarkEntityNumMap;
	GatherTestpointsByInsertType(doc, file, insertTypeTestProbe, testpointNum, &accessMarkEntityNumMap);
	GatherTestpointsByInsertType(doc, file, insertTypeTestAccessPoint, testpointNum, &accessMarkEntityNumMap);
	accessMarkEntityNumMap.RemoveAll();
}

/******************************************************************************
*/
static void GatherTestpointsByInsertType(CCEtoODBDoc *doc, FileStruct *file, InsertTypeTag insertType,
													int &testpointNum, CMapInt *accessMarkEntityNumMap)
{
	WORD netnameKey = (WORD)doc->IsKeyWord(ATT_NETNAME, 0);

	POSITION pos = file->getBlock()->getHeadDataInsertPosition();
	while (pos)
	{
		CString speaTestpointName;
		CString netName;
		CString drawingRef;
		CString pinName;
		CString testpointCode;
		int contactCode = 0;
		double testAccessMarkX = 0.0;
		double testAccessMarkY = 0.0;
		char contactSide = '\0';
      int padCode = -1;
      int accessMarkEntityNum = -1;
		Attrib *attrib = NULL;

		DataStruct *data = file->getBlock()->getNextDataInsert(pos);

		if (GetTestTargetInfo(doc, file, data, insertType, drawingRef, pinName, contactCode, padCode, accessMarkEntityNum) < 1)
			continue;

		// This is the test point name for output, in the original form for this exporter.
      // This will only get used for output of classic :TESTPOINT section.
		speaTestpointName.Format("%s-%s", drawingRef, pinName);	

		// This is to keep track of testpoints aka access marks already processed. Since we do
      // probes first then access marks (and the probes just lead to access marks too) we do not
      // want access marks already processed by probe insert to be collected when processing
      // access mark inserts directly. We use the entity number of the access mark to track this,
      // to prevent writing duplicate testpoints.
      int ignored;
		if (accessMarkEntityNumMap->Lookup(accessMarkEntityNum, ignored))
			continue;
		else
			accessMarkEntityNumMap->SetAt(accessMarkEntityNum, accessMarkEntityNum);

		if (data->getAttributesRef()->Lookup(netnameKey, attrib))
		{
			if (attrib)
				netName = attrib->getStringValue();
		}

		if (data->isInsertType(insertTypeTestProbe))
			testpointCode = "M";		// Master (the 1st test point defined on the net)
		else 
			testpointCode = "K";		// Kelvin (all the other test point on the net)

		if (data->getInsert()->getMirrorFlags() & MIRROR_ALL)
			contactSide = 'B';
		else
			contactSide = 'T';

		testAccessMarkX = data->getInsert()->getOriginX();
		testAccessMarkY = data->getInsert()->getOriginY();

      //Skip Die Pins
      NetStruct *net = file->getNetList().getNet(netName);
      if(net && file->getNetList().getNet(netName)->IsDiePin(drawingRef, pinName))
      {
         CString testType = (data->isInsertType(insertTypeTestProbe))?"Probe":"Access Marker";
         fprintf(speaLogFp, ":TESTPOINT Net: %s %s: %s CompPin %s.%s - Skipped %s for Die Pin.\n", 
            check_name('n', netName), testType, data->getInsert()->getRefname(),
            check_name('c', drawingRef), check_name('p', pinName), testType);   
      }
      else
      {
         SpeaTestpoint *tp = new SpeaTestpoint(++testpointNum, speaTestpointName, accessMarkEntityNum, netName, drawingRef, pinName, testpointCode,
            contactCode, testAccessMarkX, testAccessMarkY, contactSide, padCode);
         TestpointArray.Add(tp);
      }
	}
}

/******************************************************************************
*/
static int GetTestTargetInfo(CCEtoODBDoc *doc, FileStruct *file, DataStruct *data, InsertTypeTag insertType, CString &refname,
										  CString &pinName, int &contactCode, int &padCode, int &accessMarkEntityNum)
{
	refname.Empty();
	pinName.Empty();
	contactCode = 0;
   padCode = 0;
   accessMarkEntityNum = 0;

	if (data == NULL || !data->isInsertType(insertType))
		return 0;

   if (data->getInsert()->getInsertType() == insertTypeTestAccessPoint)
   {
      accessMarkEntityNum = data->getEntityNumber();
   }

	// Get the entity number from the "Datalink" attrib
	Attrib *attrib = NULL;
	WORD keyword = (WORD)doc->IsKeyWord(ATT_DDLINK, 0);

	if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(keyword, attrib))
		return 0;

	if (attrib == NULL)
		return 0;
	
	// Get the feature from the entity number
	long entityNum = (long)attrib->getIntValue();
   CEntity entity = CEntity::findEntity(doc->getCamCadData(),entityNum);

   if (entity.getEntityType() == entityTypeData)	
	{
		//DataStruct *entityData = (DataStruct*)voidPtr;
      DataStruct* entityData = entity.getData();

		if (!entityData || entityData->getDataType() != dataTypeInsert)
			return 0;

		InsertStruct *insert = entityData->getInsert();

		if (insert->getInsertType() == insertTypeTestAccessPoint)
		{
			return GetTestTargetInfo(doc, file, entityData, insertTypeTestAccessPoint, refname, pinName, contactCode, padCode, accessMarkEntityNum);
		}	
		else if (insert->getInsertType() == insertTypeVia || insert->getInsertType() == insertTypeTestPad
         || insert->getInsertType() == insertTypeBondPad)
		{
			refname = insert->getRefname();
			pinName = "1";	// it is 1 because via has no pin
			contactCode = CDC_VIA;
         ETestSurface surface = testSurfaceTop;
         padCode = getPadCode(doc, insert->getBlockNumber(), surface);

			return 1;
		}
		else
		{
			// Anything else is a Test Point
			refname = data->getInsert()->getRefname();
			pinName = "1";	// it is 1 because via has no pin
			contactCode = CDC_TESTPOINT;
         ETestSurface surface = testSurfaceTop;
         padCode = getPadCode(doc, data->getInsert()->getBlockNumber(), surface);

			return 1;
		}
	}
	else if (entity.getEntityType() == entityTypeCompPin)
	{
      CompPinStruct* comppin = entity.getCompPin();

		if (comppin)
		{
			refname = comppin->getRefDes();
			pinName = comppin->getPinName();

			DataStruct *parentComp = FindComponentData(file, refname);
			if (parentComp != NULL && parentComp->isInsertType(insertTypeTestPoint))
				contactCode = CDC_TESTPOINT;
			else if (IsThroughHole(doc, file, comppin, parentComp))
				contactCode = CDC_THROUGH_HOLE;
			else
				contactCode = CDC_SMD;

         ETestSurface surface = testSurfaceTop;
         padCode = getPadCode(doc, comppin->getPadstackBlockNumber(), surface);

			return 1;
		}
	}

	// If anything else return 0
	return 0;
}

//------------------------------------------------------------------------

static CString GetTechnology(CCEtoODBDoc *m_pDoc, CompPinStruct *compPin, DataStruct *componentdata)
{
	// compPin is particular compPin
	// component data is associated "parent" pcbcomponent data

	CString technology;
	Attrib *techAttr;

	// Favor technology attr on compPin, if there is one

	if (compPin != NULL)
	{
		techAttr = is_attvalue(m_pDoc, compPin->getAttributesRef(), ATT_TECHNOLOGY, 0);
		if (techAttr)
			technology = get_attvalue_string(m_pDoc, techAttr);
	}
	
	if (technology.IsEmpty() && componentdata != NULL)
	{
		techAttr = is_attvalue(m_pDoc, componentdata->getAttributesRef(), ATT_TECHNOLOGY, 0);
		if (techAttr)
			technology = get_attvalue_string(m_pDoc, techAttr);
	}

	return (technology.IsEmpty() ? "SMD" : technology);
}

//------------------------------------------------------------------------

static DataStruct *FindComponentData(FileStruct *file, CString refdes)
{
	// Was originally limited to looking for INSERT TYPE PCBCOMPONENT.
	// That was too restrictive, as it left out TEST POINT inserts, which
	// are definitely of interest. So maybe any sort of insert is of interest,
	// so long as the refname matches, no more quibbling on insert type.

	//FileStruct *file = doc->FileList.GetOnlyShown(blockTypePcb);

	POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

		if (data->getDataType() == T_INSERT)
		{
			InsertStruct *insert = data->getInsert();
			CString insertname = insert->getRefname();
			if (data->getInsert()->getRefname().Compare(refdes) == 0)
			{
				return data;
			}
		}
	}

	return NULL;
}

//------------------------------------------------------------------------

static bool IsThroughHole(CCEtoODBDoc *doc, FileStruct *file, CompPinStruct *cp, DataStruct *parentComp)
{
	if (cp != NULL || parentComp != NULL)
	{
		CString techAttr = GetTechnology(doc, cp, parentComp);
		techAttr.MakeUpper();
		if (techAttr.Find("THRU") > -1)
			return true;
	}

	// default is smd
	return false;
}

/******************************************************************************
* WritePackage
*/
static void WritePackagesSection(FILE *fp, CCEtoODBDoc *doc, FileStruct *file, CMapStringToString* packageNameMap, CMapStringToString* packageRenameMap)
{
	if (getFileBlock(file, "PACKAGES") == NULL)
		return;

	WORD technologyKey = (WORD)doc->IsKeyWord(ATT_TECHNOLOGY, 0);

	fprintf(fp, ":PACKAGES\n\n");

	for (int i=0; i<=doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);

		if (block == NULL || block->getBlockType() != blockTypePcbComponent)
			continue;

		CString packageName = block->getName();
		CString packageType = "SMD";
		double sizeX = 0.0;
		double sizeY = 0.0;
		double sizeZ = 0.0;

		if (sPackageAttribute.IsEmpty() == false)
		{
			CString packageRename;

			if (packageNameMap->Lookup(block->getName(), packageRename) && packageRename.IsEmpty() == false)
				packageName = packageRename;
		}

		packageName = GetValidPackageName(packageName, packageRenameMap); 

		if (block->getAttributes())
		{
			Attrib *attrib = NULL;

			if (block->getAttributes()->Lookup(technologyKey, attrib) && attrib != NULL)
			{
				packageType = attrib->getStringValue();

				if (!packageType.CompareNoCase("THRU"))
					packageType = "TH";
			}
		}

		sizeX = block->getExtent().getSize().cx;
		sizeY = block->getExtent().getSize().cy;
		
		fprintf(fp, "%s ,", check_name('k', packageName));	 // Case Name
		fprintf(fp, "%s ,", packageType);			          // PackageType
		fprintf(fp, "%.3f ,", sizeX);					// X Dimension
		fprintf(fp, "%.3f ,", sizeY);					// Y Dimension
		fprintf(fp, "%.3f\n", sizeZ);					// Z Dimension
	}

   // If exporting testpoints as components then write a package entry for them.
   if (bExportTestpointsAsComponents)
   {
      double compXsize = 0.;
      double compYsize = 0.;
      if (TestpointArray.GetCount() > 0)
      {
         SpeaTestpoint *tp = TestpointArray.GetAt(0);
         int padstackBlockNum = tp->m_padCode;
         BlockStruct *padstackBlock = doc->getBlockAt(padstackBlockNum);
         CExtent extent = padstackBlock->getExtent(doc->getCamCadData());
         compXsize = extent.getXsize();  // using padstack extent for comp size
         compYsize = extent.getYsize();
      }

		fprintf(fp, "%s ,", check_name('k', TESTPOINT_PACKAGE_NAME));    // Case Name
		fprintf(fp, "%s ,", "SMD");                        // PackageType
		fprintf(fp, "%.3f ,", compXsize);                  // X Dimension
		fprintf(fp, "%.3f ,", compYsize);                  // Y Dimension
		fprintf(fp, "%.3f\n", 0.0);                        // Z Dimension
   }


	fprintf(fp, "\n:ENDPACKAGES\n\n");
}

/******************************************************************************
* WritePad
*/
static void WritePad(FILE *fp, CCEtoODBDoc *doc, FileStruct *file)
{
	if (getFileBlock(file, "PAD") == NULL)
		return;

	fprintf(fp, ":PAD\n\n");

	int padNameCount = 0;
	int count = 0;

	for (int i=0; i<doc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = doc->getBlockAt(i);

		if (block == NULL || !block->isAperture())
			continue;

		CString padName = block->getNameRef().Trim();
		if (padName.IsEmpty())
			padName.Format("PAD%d", ++padNameCount);


		CString shape;
		double sizeX = 0.0;
		double sizeY = 0.0;

		if (block->getShape() == apertureRound)
		{
			shape = "CIRCLE";
			sizeX = block->getSizeA();
			sizeY = sizeX;
		}
		else
		{
			shape = "RECT";

			if (block->getShape() == apertureComplex)
			{
				sizeX = block->getExtent().getSize().cx;
				sizeY = block->getExtent().getSize().cy;
			}
			else if (block->getShape() == apertureSquare)
			{
				sizeX = block->getSizeA();
				sizeY = sizeX;
			}
			else
			{
				sizeX = block->getSizeA();
				sizeY = block->getSizeB();
			}
		}



		double centerX = sizeX / 2;
		double centerY = sizeY / 2;

		fprintf(fp, "%d ,", block->getBlockNumber());	// Pad Code
		fprintf(fp, "%s ,", check_name('p', padName));  // Pad Name
		fprintf(fp, "%s ,", check_name('s', shape));		// Pad Shape
		fprintf(fp, "%.3f ,", sizeX);							// X Dimension
		fprintf(fp, "%.3f ,", sizeY);							// Y Dimension
		fprintf(fp, "%.3f ,", centerX);						// Center X
		fprintf(fp, "%.3f\n", centerY);						// Center Y
	}

	fprintf(fp, "\n:ENDPAD\n\n");
}

static void WriteWiring(FILE *fp, CCEtoODBDoc *doc, FileStruct *file)
{
	if (getFileBlock(file, "WIRING") == NULL)
		return;

	fprintf(fp, ":WIRING\n\n");

	int netIndx = 0;

	POSITION netPos = file->getNetList().GetHeadPosition();
	while (netPos)
	{
		NetStruct *net = file->getNetList().GetNext(netPos);

		if (net != NULL)
		{
			netIndx++;
			fprintf(fp,"%s,,,,,%d\n", check_name('n', net->getNetName()), netIndx);
		}
	}

	fprintf(fp, "\n:ENDWIRING\n\n");
}

static void WriteRouting(FILE *fp, CCEtoODBDoc *doc, FileStruct *file)
{
	BlockStruct *fileBlock;

	if ((fileBlock = getFileBlock(file, "ROUTING")) == NULL)
		return;

	WORD netnameKey = doc->IsKeyWord(ATT_NETNAME, 0);

	fprintf(fp, ":ROUTING\n\n");
	
	int netIndx = 0;

	POSITION netPos = file->getNetList().GetHeadPosition();
	while (netPos)
	{
		NetStruct *net = file->getNetList().GetNext(netPos);

		if (net != NULL)
		{
			netIndx++;

			POSITION blockPos = fileBlock->getDataList().GetHeadPosition();
			while (blockPos)
			{
				DataStruct *data = fileBlock->getDataList().GetNext(blockPos);

				if (data != NULL && data->getAttributes() != NULL)
				{
					Attrib *attrib = NULL;

					if (data->getAttributes()->Lookup(netnameKey, attrib))
					{
						CString netName = attrib->getStringValue();

						if (netName.Compare(net->getNetName()) == 0)
						{
							if (data->getDataType() == dataTypePoly && data->getGraphicClass() == graphicClassEtch)
							{
								int speaLayer = 0;  // Zero is undefined, spea layers start at 1
								LayerStruct *layer = doc->FindLayer(data->getLayerIndex());
								if (layer != NULL)
								{
									int stackNum = layer->getElectricalStackNumber();
								
									if (layer->isElectricalTop() || stackNum == 1)
												speaLayer = 1;  // Top
									else if (layer->isElectricalBottom() || stackNum == doc->getMaxElectricalLayerNumber())
												speaLayer = 2;  // Bottom
									else
									{
										if (stackNum < 1)
													speaLayer = 3; // top-most inner layer
										else
													speaLayer = 1 + stackNum;  // should be 3..n for inner layers top to bottom
									}
								}								

									if ((ExportTraces == eExportTracesAll) ||
											(ExportTraces == eExportTracesOuter && (speaLayer == 1 || speaLayer == 2)))
									{
											speaWriteSHPoly(doc, fp, 0, data->getPolyList(), 1.0/*UnitFactor*/, true, false, netIndx, speaLayer);
									}
							}
						}
					}
				}
			}
		}
	}

	fprintf(fp, "\n:ENDROUTING\n\n");
}

static void speaWriteSHPoly(CCEtoODBDoc *doc, FILE *fp, int indent,const CPolyList *polyList, double unitFactor, bool isNetTrace, bool writeMirrored, int netIndx, int speaLayer)
{
	// Output the list of polylines

	POSITION pos = polyList->GetHeadPosition();
	while(pos)
	{
		const CPoly *poly = polyList->GetNext(pos);
		double width = 0.0;

		if (isNetTrace)
		{
			// Get width for net trace
			if (poly->getWidthIndex() > -1)
			{
				BlockStruct *block = doc->getWidthTable()[poly->getWidthIndex()];
				width = block->getSizeA() * unitFactor;
			}
		}

		double cX = 0.0;
		double cY = 0.0;
		double radius = 0.0;

		if (PolyIsCircle(poly, &cX, &cY, &radius))
		{
			cX = cX * unitFactor;
			cY = cY * unitFactor;

			if (writeMirrored == true)
				cX = -cX;

			int cnt = 255;
			Point2 *poly = (Point2 *)calloc(255,sizeof(Point2));
			if (poly != NULL)
			{
				arc2poly(DegToRad(0.0), DegToRad(360.0), DegToRad(ARC_ANGLE_DEGREE), cX, cY, radius, poly, &cnt);
			
				if (cnt > 0)
				{
					for (int i = 0; i < cnt - 1; i++)
					{
						OutlineLineNum++;						
						{
							fprintf(fp, "%d, %.3f, %.3f, %.3f, %.3f", 
								netIndx > 0 ? netIndx : OutlineLineNum, 
								poly[i].x, poly[i].y, poly[i + 1].x, poly[i + 1].y);
						}
						
						if (isNetTrace)
						{
							fprintf(fp, ", %.3f, %d", width, speaLayer);
						}

						fprintf(fp, "\n");
					}
				}
				free (poly);
			}
			
		}
		else
		{
			bool firstPnt = true;
			CPnt prevPnt;
			CPntList pntList = poly->getPntList();

			POSITION pntPos = pntList.GetHeadPosition();
			while(pntPos)
			{
				CPnt *rawPnt = pntList.GetNext(pntPos);

				CPnt curPnt;
				curPnt.x = (DbUnit)(rawPnt->x * unitFactor);
				curPnt.y = (DbUnit)(rawPnt->y * unitFactor);
				curPnt.bulge = rawPnt->bulge;

				if (writeMirrored == true)
				{
					curPnt.x = -curPnt.x;

					if (!fpeq(curPnt.bulge, 0))
						curPnt.bulge = -curPnt.bulge;
				}

				if (!firstPnt)
				{
					OutlineLineNum++;

					fprintf(fp, "%d, %.3f, %.3f, %.3f, %.3f", 
						netIndx > 0 ? netIndx : OutlineLineNum,
						prevPnt.x, prevPnt.y, curPnt.x, curPnt.y);

					if (isNetTrace)
					{
						fprintf(fp, ", %.3f, %d", width, speaLayer);
					}

					fprintf(fp, "\n");
				}

				prevPnt = curPnt;
				firstPnt = false;

				while (!fpeq(curPnt.bulge, 0) && pntPos != NULL)
				{
					CPnt *rawPnt2 = pntList.GetNext(pntPos);

					CPnt nextPnt;
					nextPnt.x = (DbUnit)(rawPnt2->x * unitFactor);
					nextPnt.y = (DbUnit)(rawPnt2->y * unitFactor);
               nextPnt.bulge = rawPnt2->bulge;

					if (writeMirrored == true)
					{
						nextPnt.x = -nextPnt.x;
                  nextPnt.bulge = -nextPnt.bulge;
					}

					double startAngle, radius, xCenter, yCenter;
					double deltaAngle = atan(curPnt.bulge) * 4;

					ArcPoint2Angle(curPnt.x, curPnt.y, nextPnt.x, nextPnt.y, deltaAngle, &xCenter, &yCenter, &radius, &startAngle);

               int cnt = 255;
					Point2 *poly = (Point2 *)calloc(255,sizeof(Point2));
					if (poly != NULL)
					{
						arc2poly(startAngle, deltaAngle, DegToRad(ARC_ANGLE_DEGREE), xCenter, yCenter, radius, poly, &cnt);
					
   				   if (cnt > 0)
					   {
						   for (int i = 0; i < cnt - 1; i++)
						   {
							   OutlineLineNum++;

								fprintf(fp, "%d, %.3f, %.3f, %.3f, %.3f", 
									netIndx > 0 ? netIndx : OutlineLineNum,
									poly[i].x, poly[i].y, poly[i + 1].x, poly[i + 1].y);
							   
							   if (isNetTrace)
							   {
								   fprintf(fp, ", %.3f, %d", width, speaLayer);
							   }

							   fprintf(fp, "\n");
						   }

						   prevPnt.x = (DbUnit)poly[cnt - 1].x;
						   prevPnt.y = (DbUnit)poly[cnt - 1].y;
                     free(poly);
                  }
					}

               // In case we have back-to-back arcs, stay in "while" until no bulge or no more pts
               curPnt = nextPnt;
					
				}
			}
		}
	}
	
}

static void WriteBoardOutline(FILE *fp, CCEtoODBDoc *doc, FileStruct *file)
{
	// Patterned after Aeroflex bouard outline implementation

	BlockStruct *pcbBlock = getFileBlock(file, "BOARDOUTLINE");

	if (pcbBlock == NULL)
		return;

	fprintf(fp, ":BOARDOUTLINE\n\n");

	OutlineLineNum = 0;
	const CDataList& dataList = pcbBlock->getDataList();

	POSITION pos = dataList.GetHeadPosition();
	while(pos)
	{
		const DataStruct *data = dataList.GetNext(pos);

		if (data == NULL || data->getDataType() == dataTypeInsert)
			continue;

		if (data->getDataType() == dataTypePoly && (data->getGraphicClass() == graphicClassBoardOutline || data->getGraphicClass() == graphicClassPanelOutline))
			speaWriteSHPoly(doc, fp, 0, data->getPolyList(), 1.0/*UnitFactor*/, false, false, 0, 0);
		///else if (data->getDataType() == dataTypeText)
		///	writeSHText(file, indent, data->getText());
	}

	fprintf(fp, "\n:ENDBOARDOUTLINE\n\n");
}

FileStruct * getPcbFile(CCEtoODBDoc *doc, FileStruct *panelFile)
{
   int pcbBlockNumber = -1;
   FileStruct * file;
   DataStruct  *np;
   POSITION pos = panelFile->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = panelFile->getBlock()->getDataList().GetNext(pos);
      if (np->getDataType() != T_INSERT)  
         continue;

      if (np->getInsert()->getInsertType() == insertTypePcb)   
      {
         pcbBlockNumber = np->getInsert()->getBlockNumber();
         break;
      }
   }

   for (POSITION pos = doc->getFileList().GetHeadPosition();pos != NULL;)
   {
      file = doc->getFileList().GetNext(pos);
      if ((blockTypePcb == file->getBlockType()) && (file->getBlock()->getBlockNumber() == pcbBlockNumber))
      {
         return file;
      }
   }
   return NULL;
}

BOOL in_boardarray(int blockNumber, int mirror)
{

   for (int i=0; i<uniqueBoardCount; i++)
   {
      if (uniqueBoardArray[i]->blockNumber == blockNumber && uniqueBoardArray[i]->mirror == mirror)
         return TRUE;
   }

   return FALSE;
}

int FindUniqueBoardCount(FileStruct *file)
{
   DataStruct  *np;
   for (int i=0;i<uniqueBoardCount;i++)
   {
      delete uniqueBoardArray[i];
   }
   uniqueBoardArray.RemoveAll();
   uniqueBoardCount = 0;
   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = file->getBlock()->getDataList().GetNext(pos);
      if (np->getDataType() != T_INSERT)  
         continue;

      if (np->getInsert()->getInsertType() == insertTypePcb)   
      {
         if (!in_boardarray(np->getInsert()->getBlockNumber(), np->getInsert()->getMirrorFlags()))
         {
            SpeaUniqueBoard *c = new SpeaUniqueBoard;
            uniqueBoardArray.SetAtGrow(uniqueBoardCount,c);
            uniqueBoardCount++;
            c->blockNumber = np->getInsert()->getBlockNumber();
            c->mirror = np->getInsert()->getMirrorFlags();            
         }
      }
   }
   return uniqueBoardCount;
}

/******************************************************************************
* WritePanelSection
*/
static void WritePanelSection(FILE *fp, FileStruct *panelFile,int offsetTP)
{
   int panelID = 0;
   DataStruct  *np;
	// Print board information
	fprintf(fp, ":PANEL\n\n");
   double scale = panelFile->getScale();
   double angle = panelFile->getRotation();
   POSITION pos = panelFile->getBlock()->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = panelFile->getBlock()->getDataList().GetNext(pos);
      if (np->getDataType() != T_INSERT)  
         continue;

      if (np->getInsert()->getInsertType() == insertTypePcb)   
      {
         fprintf(fp, "%d ,", ++panelID);
         fprintf(fp, "%.3f ,", np->getInsert()->getOriginX() * scale);
         fprintf(fp, "%.3f ,", np->getInsert()->getOriginY() * scale);
         fprintf(fp, "%d ,", np->getInsert()->getAngleDegrees() + angle);
         fprintf(fp, "%d", offsetTP * (panelID - 1));
         fprintf(fp, "\n");
      }
   }
   fprintf(fp, "\n:ENDPANEL\n\n");
}

void WriteVariantSection(FILE *fp, FileStruct *file)
{
   int variantCode = -1;
   fprintf(fp, ":VARIANT\n\n");
   for (POSITION pos=file->getVariantList().GetHeadPosition(); pos!=NULL;)
   {
      CVariant* variant = file->getVariantList().GetNext(pos);
      if (variant != NULL)
      {
         fprintf(fp, "%d ,", ++variantCode);
         fprintf(fp, "%s", variant->GetName());
         fprintf(fp, "\n");
      }
   }
   fprintf(fp, "\n:ENDVARIANT\n\n");
}

int findHighestProbeNumber(FileStruct *file)
{
   int highestProbeNumber = 0;
   BlockStruct *pcbBlock = file->getBlock();
   if (pcbBlock == NULL)
      return highestProbeNumber;
   for (POSITION dataPos=pcbBlock->getHeadDataInsertPosition(); dataPos!=NULL; pcbBlock->getNextDataInsert(dataPos))
   {
      DataStruct *data = pcbBlock->getAtData(dataPos);
      InsertStruct *insert = data->getInsert();

      if (insert->getInsertType() == insertTypeTestProbe)
      {
         int thisProbeNumber = atoi(data->getInsert()->getRefname());
         if (thisProbeNumber > highestProbeNumber)
            highestProbeNumber = thisProbeNumber;
      }
   }
   return highestProbeNumber;
}

// CSpeaOutProbeOffsetDlg dialog

IMPLEMENT_DYNAMIC(CSpeaOutProbeOffsetDlg, CDialog)
CSpeaOutProbeOffsetDlg::CSpeaOutProbeOffsetDlg(int defaultOffset, CWnd* pParent /*=NULL*/)
   : CDialog(CSpeaOutProbeOffsetDlg::IDD, pParent)
   , m_probeNumberOffset(defaultOffset)
   , m_minimumOffset(defaultOffset)
{
}

CSpeaOutProbeOffsetDlg::~CSpeaOutProbeOffsetDlg()
{
}

void CSpeaOutProbeOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_PROBE_OFFSET, m_probeNumberOffset);
   DDV_MinMaxInt(pDX, m_probeNumberOffset, m_minimumOffset, 9000000);
}


BEGIN_MESSAGE_MAP(CSpeaOutProbeOffsetDlg, CDialog)
END_MESSAGE_MAP()


// CSpeaOutProbeOffsetDlg message handlers