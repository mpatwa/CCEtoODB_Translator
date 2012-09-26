// $Header: /CAMCAD/5.0/read_wrt/ViscomAOI_Out.cpp 27    6/17/07 9:01p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// \:/*?"<>| these are illegal characters that should be replaced with _

#include "stdafx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "FileList.h"
#include "RwLib.h"
#include "pcbutil.h"
#include "RwUiLib.h"
#include "Outline.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define PACKAGE_NAME_GEOMETRY		"GEOMETRY"
#define PACKAGE_NAME_DUMMY			"DUMMY"
#define PACKAGE_NAME_PARTNUMBER	"PARTNUMBER"


/* Static Variable Section *********************************************************/
static FILE						*sErrFile;
static long                sDisplayErr;         // current number of errors
static double					sUnitFactor;

// Variable for setting file "Viscom.out"
static CString					sFidName;
static bool						sFidAsComp;
static CString					sWidthOffset;
static CString					sRommelNum;
static bool                sRommelComment;
static CString					sPackageName;
static bool						sXYAdjustByCentroid;
static bool						sRotationAdjustByCentroid;
static bool						sFiducialByBoard;


/* Function Prototypes *********************************************************/
static bool IsMixBoardPanel(CCEtoODBDoc *doc, FileStruct* panel);

static void WritePanelDefinition(CCEtoODBDoc *doc, FileStruct *panel, CString fileName, bool isBottom);
static void WritePanelFiducial(CCEtoODBDoc *doc, FileStruct *panel, FILE *pFile, bool isBottom);
static void WritePanelBoard(CCEtoODBDoc *doc, FileStruct *panel, FILE *pFile, bool isBottom);
static void WritePanelSize(CCEtoODBDoc *doc, FileStruct *panel, CString fileName, bool isBottom, CString widthOffset, CString rommelNum);
static void WritePanelBadBoard(CCEtoODBDoc& doc, FileStruct& panel, const CString fileName, const bool isBottom);

static void WriteBoardSize(CCEtoODBDoc *doc, FileStruct *activeFile, CString fileName, bool isBottom, CString widthOffset, CString rommelNum);
static void WriteBoardComponent(CCEtoODBDoc *doc, BlockStruct *pcbBoard, CString fileName, bool isBottom);
static void WriteBoardFiducial(CCEtoODBDoc *doc, BlockStruct *pcbBoard, CString fileName, bool isBottom, CString fidName, bool fidAsComp);

static FILE *FileOpen(CString fileName);
static int GetRotationAndMirror(int rotation, bool mirror, int &t1, int &t2, int &t3, int &t4);
static void LoadSettingFile(CString fileName);
CString ReplaceIllegalChars(CString ToReplace);

/******************************************************************************
* ViscomAOI_WriteFile
*/
void ViscomAOI_WriteFiles(CString pathName, CCEtoODBDoc *doc, FormatStruct *format, int pageUnits)
{
	// Open the log file
   CString logFile = GetLogfilePath("ViscomAOI.log");
	sErrFile = FileOpen(logFile);
	if (sErrFile == NULL)
		return;

   // Log file header
   CTime t;
   t = t.GetCurrentTime();
   fprintf(sErrFile, "# ---------------------------------------\n");
   fprintf(sErrFile, "# Viscom Export Log File\n");
   fprintf(sErrFile, "# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(sErrFile, "# Created : %s\n",t.Format("%A, %B %d, %Y at %H:%M:%S"));
	fprintf(sErrFile, "# ---------------------------------------\n\n\n");

	sDisplayErr = 0;
   sUnitFactor = Units_Factor(pageUnits, UNIT_MM) * format->Scale * 100;

	FileStruct *panel = NULL;
	int count = doc->getFileList().GetVisibleCount(blockTypePanel);
	if (count > 1)
	{
		SelectFileDlg dlg;
		dlg.doc = doc;
		dlg.SetFileType(blockTypePanel);
		if (dlg.DoModal() != IDOK)
			return;

		panel = dlg.file;
	}
	else
	{
		panel = doc->getFileList().GetFirstShown(blockTypePanel);
	}

	if (panel == NULL)
	{
		ErrorMessage("No panel is visible.  Aborting export.", "Error");
		return;
	}
	else if (IsMixBoardPanel(doc, panel))
	{
		ErrorMessage("Panel has mixed boards.  Aborting export.", "Error");
		return;
	}


   CString settingsFilePath( getApp().getExportSettingsFilePath("Viscom.out") );
	LoadSettingFile(settingsFilePath);

	// Output panel file
	CString fileName = pathName + panel->getBlock()->getName();
	WritePanelDefinition(doc, panel, fileName + "_Top.def", false);
	WritePanelDefinition(doc, panel, fileName + "_Bot.def", true);
	WritePanelSize(doc, panel, fileName + "_Top.size", false, sWidthOffset, sRommelNum);
	WritePanelSize(doc, panel, fileName + "_Bot.size", true, sWidthOffset, sRommelNum);
   WritePanelBadBoard(*doc, *panel, fileName + "_Top.bbs", false);
   WritePanelBadBoard(*doc, *panel, fileName + "_Bot.bbs", true);

	if (!sFiducialByBoard)
	{
		WriteBoardFiducial(doc,  panel->getBlock(), fileName + "_Top.ref", false, sFidName, sFidAsComp);
		WriteBoardFiducial(doc,  panel->getBlock(), fileName + "_Bot.ref", true, sFidName, sFidAsComp);
	}


	// Output board file
	CMapStringToString boardNameMap;
	boardNameMap.RemoveAll();
	POSITION pos = panel->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = panel->getBlock()->getDataList().GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;

		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypePcb)
			continue;
		
		BlockStruct *block = doc->getBlockAt(insert->getBlockNumber());
		if (block == NULL)
			continue;

		CString boardName = block->getName();
		if (!boardNameMap.Lookup(boardName, boardName))
			boardNameMap.SetAt(boardName, boardName);
		else
			continue;

		// Write the top files
		WriteBoardComponent(doc, block, fileName + "_Top.cad", false);
		if (sFiducialByBoard)
			WriteBoardFiducial(doc, block, fileName + "_Top.ref", false, sFidName, sFidAsComp);

		// Write the bottom files
		WriteBoardComponent(doc, block, fileName + "_Bot.cad", true);
		if (sFiducialByBoard)
			WriteBoardFiducial(doc, block, fileName + "_Bot.ref", true, sFidName, sFidAsComp);

		break;
	}
	

	// Finished writing files
	boardNameMap.RemoveAll();
	fclose(sErrFile);
}

/******************************************************************************
* CheckMixBoardPanel
*/
bool IsMixBoardPanel(CCEtoODBDoc *doc, FileStruct* panel)
{
	CString boardName;
	for (POSITION pos = panel->getBlock()->getHeadDataInsertPosition(); pos != NULL;)
	{
		DataStruct* data = panel->getBlock()->getNextDataInsert(pos);
		if (data == NULL)
			continue;

		InsertStruct* insert = data->getInsert();
		if (insert == NULL || insert->getInsertType() != insertTypePcb)
			continue;

		BlockStruct* board = doc->getBlockAt(insert->getBlockNumber());
		if (!boardName.IsEmpty() && boardName.CompareNoCase(board->getName()) != 0)
			return true;

		boardName = board->getName();
	}

	return false;
}

/******************************************************************************
* WritePanelDefinition
*/
void WritePanelDefinition(CCEtoODBDoc *doc, FileStruct *panel, CString fileName, bool isBottom)
{
	// Open the output file
	FILE *pFile = FileOpen(fileName);
	if (pFile == NULL)
		return;

	// Write panel definition
	int x = round(panel->getInsertX() * sUnitFactor);
	int y = round(panel->getInsertY() * sUnitFactor);
	int rotation = round(normalizeDegrees(panel->getRotation()));


	// Write header and comment
   CTime currentTime = CTime::GetCurrentTime();
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "* Created by : %s\n", getApp().getCamCadSubtitle());
	fprintf(pFile, "* Date       : %s\n", currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));
	fprintf(pFile, "*\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "* DEF-file\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "* TRA_IN     : Inputtransformation\n");
	fprintf(pFile, "* REF_GLO n  : Define Pcb n as global reference\n");
	fprintf(pFile, "* LP n       : Position of Pcb n in multi-panel\n");
	fprintf(pFile, "* REF n      : Position of fiducial n in multi-panel\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "*---------------------------------------------------------------------\n");


	// Panel information
	fprintf(pFile, "TRA_IN\n");
   // DR 794091 says no fields should have spaces or periods, so getting rid of periods in this one.
	// fprintf(pFile, "1.0000000\t0.0000000\t0.0000000\t1.0000000\t%d\t%d\t%d\n", x, y, rotation);
   fprintf(pFile, "1\t0\t0\t1\t%d\t%d\t%d\n", x, y, rotation);
	fprintf(pFile, "*\n");
	fprintf(pFile, "REF_GLO 1\n");

	// Write panel fiducial
	WritePanelBoard(doc, panel, pFile, isBottom);
	WritePanelFiducial(doc, panel, pFile, isBottom);

	// Finished writing files
	fclose(pFile);
}

/******************************************************************************
* WritePanelFiducial
*/
void WritePanelFiducial(CCEtoODBDoc *doc, FileStruct *panel, FILE *pFile, bool isBottom)
{
	int fidCount = 0;
	CDataList& dataList = panel->getBlock()->getDataList();
	POSITION pos = dataList.GetHeadPosition();
	while (pos)
	{
		const DataStruct* data = dataList.GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;
		
		const InsertStruct* insert = data->getInsert();
		if (sFiducialByBoard)
		{
			if (insert->getInsertType() != insertTypePcb)
				continue;
		}
		else
		{
			if (insert->getInsertType() != insertTypeFiducial || insert->getGraphicMirrored() != isBottom)
				continue;
		}
		fidCount++;

		int rotation = round(normalizeDegrees(insert->getAngleDegrees()));
		int t1, t2, t3, t4;
		t1 = t2 = t3 = t4 = 0;

		if (GetRotationAndMirror(rotation, insert->getGraphicMirrored(), t1, t2, t3, t4) == -1)
		{
			fprintf(sErrFile, "Fiducial #%d:  rotation is not othagonal [%d degreee], invalid rotation output", fidCount, rotation);
			sDisplayErr++;
		}

		int x = round(insert->getOriginX() * sUnitFactor);
		int y = round(insert->getOriginY() * sUnitFactor);

		fprintf(pFile, "*\n");
		fprintf(pFile, "* Position of fiducial %d in multi-panel\n", fidCount);
		fprintf(pFile, "*\n");
		fprintf(pFile, "REF %d\n", fidCount);
      // DR 794091 says no fields should have spaces or periods, so getting rid of periods in this one.
		// fprintf(pFile, "%d.00\t%d.00\t%d.00\t%d.00\t%d.00\t%d.00\t-%d\n", t1, t2, t3, t4, x, y, fidCount);
      fprintf(pFile, "%d\t%d\t%d\t%d\t%d\t%d\t-%d\n", t1, t2, t3, t4, x, y, fidCount);

		// Only write out one panel fiducial if by panel
		if (!sFiducialByBoard)
			break;
	}

   // DR 794091 says one "REF 1" is mandatory. So output one if we haven't already.
   // If we do, here, then also in the ref file we have fabricated some fiducials, and put out
   // a log message about it. I don't think we need another log message about fids from here.
   if (fidCount < 1)
   {
		fidCount = 1;
      
      fprintf(pFile, "*\n");
		fprintf(pFile, "* Position of fiducial %d in multi-panel\n", fidCount);
		fprintf(pFile, "*\n");
      
      fprintf(pFile, "REF %d\n", fidCount);
		int t1, t2, t3, t4, x, y;
		t1 = t2 = t3 = t4 = x = y = 0;
      fprintf(pFile, "%d\t%d\t%d\t%d\t%d\t%d\t-%d\n", t1, t2, t3, t4, x, y, fidCount);
   }
}

/******************************************************************************
* WritePanelBoard
*/
void WritePanelBoard(CCEtoODBDoc *doc, FileStruct *panel, FILE *pFile, bool isBottom)
{
	int pcbCount = 0;
	CDataList& dataList = panel->getBlock()->getDataList();
	POSITION pos = dataList.GetHeadPosition();
	while (pos)
	{
		const DataStruct* data = dataList.GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;
		
		const InsertStruct* insert = data->getInsert();
		if (insert->getInsertType() != insertTypePcb)
			continue;
		pcbCount++;

		int rotation = round(normalizeDegrees(insert->getAngleDegrees()));
		int t1, t2, t3, t4;
		t1 = t2 = t3 = t4 = 0;

		if (GetRotationAndMirror(rotation, insert->getGraphicMirrored(), t1, t2, t3, t4) == -1)
		{
			fprintf(sErrFile, "PCB #%d:  rotation is not othagonal [%d degreee], invalid rotation output", pcbCount, rotation);
			sDisplayErr++;
		}

		int x = round(insert->getOriginX() * sUnitFactor);
		int y = round(insert->getOriginY() * sUnitFactor);

		fprintf(pFile, "*\n");
		fprintf(pFile, "* Position of PCB %d in multi-panel\n", pcbCount);
		fprintf(pFile, "*\n");

		fprintf(pFile, "LP %d\n", pcbCount);
      // DR 794091 says no fields should have spaces or periods, so getting rid of periods in this one.
		// fprintf(pFile, "%d.00\t%d.00\t%d.00\t%d.00\t%d.00\t%d.00\t-%d\n", t1, t2, t3, t4, x, y, pcbCount);
      fprintf(pFile, "%d\t%d\t%d\t%d\t%d\t%d\t-%d\n", t1, t2, t3, t4, x, y, pcbCount);
	}
}

void WritePanelBadBoard(CCEtoODBDoc& doc, FileStruct& panel, const CString fileName, const bool isBottom)
{
	// Open the output file
	FILE *pFile = FileOpen(fileName);
	if (pFile == NULL)
		return;

	// Write header and comment
   CTime currentTime = CTime::GetCurrentTime();
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "* Created by : %s\n", getApp().getCamCadSubtitle());
	fprintf(pFile, "* Date       : %s\n", currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));
	fprintf(pFile, "*\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "* BBS-file\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "*\n");
   fprintf(pFile, "* ID\tTYP\tX\tY\tPHI\t0\t0\t0\n");
	fprintf(pFile, "*---------------------------------------------------------------------\n");

   int count = 0;
   for (CDataListIterator dataList(*panel.getBlock(), insertTypeXout); dataList.hasNext();)
   {
      InsertStruct* insert = dataList.getNext()->getInsert();    
      if (insert->getGraphicMirrored() == isBottom)
      {
         count++;

         CString Id = insert->getRefname();
         if (Id.IsEmpty())
         {
            Id.Format("BBS%d", count);
         }

         CString type = doc.getBlockAt(insert->getBlockNumber())->getName();
		   int x = round(insert->getOriginX() * sUnitFactor);
		   int y = round(insert->getOriginY() * sUnitFactor);
		   int rotation = round(normalizeDegrees(insert->getAngleDegrees()));
    
         fprintf(pFile, "%s\t%s\t%d\t%d\t0\t%d\t0\t0\n", ReplaceIllegalChars(Id), ReplaceIllegalChars(type), x, y, rotation);
      }
   }

   // DR 794091 says at least one reject mark is required, and asks to just make up a default
   // if there are none in the data.
   if (count < 1)
   {
      fprintf(sErrFile, "Insufficient reject marks in %s side data, fabricated a fake reject mark for export.\n",
            isBottom?"bottom":"top");
      CString type("DoesNotExist");
      int x, y, rotation;
      x = y = rotation = 0;
      fprintf(pFile, "%s\t%s\t%d\t%d\t0\t%d\t0\t0\n", "BBS1", ReplaceIllegalChars(type), x, y, rotation);
   }

	// Finished writing files
	fclose(pFile);
}

/******************************************************************************
* WritePanelSize
*/
void WritePanelSize(CCEtoODBDoc *doc, FileStruct *panel, CString fileName, bool isBottom, CString widthOffset, CString rommelNum)
{
	WriteBoardSize(doc, panel, fileName, isBottom, widthOffset, rommelNum);
}

/******************************************************************************
* WriteBoardSize
*/
void WriteBoardSize(CCEtoODBDoc *doc, FileStruct *activeFile, CString fileName, bool isBottom, CString widthOffset, CString rommelNum)
{
	// Open the output file
	FILE *pFile = FileOpen(fileName);
	if (pFile == NULL || doc == NULL)
		return;

	// Write header and comment
   CTime currentTime = CTime::GetCurrentTime();
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "* Created by : %s\n", getApp().getCamCadSubtitle());
	fprintf(pFile, "* Date       : %s\n", currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));
	fprintf(pFile, "*\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "* SIZE-file\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "* Size of PCB = BOARD_WIDTH + WIDTH_OFFSET\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "* BOARD_WIDTH  : width of pcb x 0.01 mm\n");
	fprintf(pFile, "* BOARD_LENGTH : length x 0.01mm\n");
	fprintf(pFile, "* WIDTH_OFFSET : offset for width-adjustment\n");
	fprintf(pFile, "*\n");


   CExtent extent;
   
   // This gets an extent not matter what. If result is  true then an outline was found, extent is probably accurate.
   // If result is false then we got extent of file geometry block, which may have fluff outside of PCB.
   // This function gets called twice, once for top and once for bottom. We don't need to present the
   // same message twice, so just present it when processing top. 
   if (!GetOutlineExtent(*doc, activeFile, extent) && !isBottom)
   {
      CString msg("No outline found for determining board extent. Using board's geometry block extent. \nThis may include objects that are not actually part of the PCB, such as annotation graphics.\n");
      ErrorMessage(msg);  // Popup for user.
      fprintf(sErrFile, msg); // Put it in the log too.
   }

   long width = round(extent.getXsize() * sUnitFactor);
   long length = round(extent.getYsize() * sUnitFactor);

	fprintf(pFile, "BOARD_WIDTH\t%d\n", width);
	fprintf(pFile, "BOARD_LENGTH\t%d\n", length);
	fprintf(pFile, "WIDTH_OFFSET\t%s\n", ReplaceIllegalChars(widthOffset));
	fprintf(pFile, "BOARD_THICKNESS\t0\n");
   fprintf(pFile, "%sROMMEL_NO\t%s\n", sRommelComment?"*":"", ReplaceIllegalChars(rommelNum));

	// Finished writing files
	fclose(pFile);
}

/******************************************************************************
* WriteBoardComponent
*/
void WriteBoardComponent(CCEtoODBDoc *doc, BlockStruct *pcbBoard, CString fileName, bool isBottom)
{
	// Open the output file
	FILE *pFile = FileOpen(fileName);
	if (pFile == NULL)
		return;

	// Write header and comment
   CTime currentTime = CTime::GetCurrentTime();
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "* Created by : %s\n", getApp().getCamCadSubtitle());
	fprintf(pFile, "* Date       : %s\n", currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));
	fprintf(pFile, "*\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "* CAD-file\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "*---------------------------------------------------------------------\n");
   fprintf(pFile, "*\n");
   fprintf(pFile, "* ID\tTYP\tX\tY\tPHI\t0\t0\t0\n");
   fprintf(pFile, "*---------------------------------------------------------------------\n");


	WORD partNumberKey = doc->RegisterKeyWord(ATT_PARTNUMBER, 0, VT_STRING);
	WORD testStrategyKey = doc->RegisterKeyWord(ATT_TEST_STRATEGY, 0, VT_STRING);
	WORD loadedKey = doc->RegisterKeyWord(ATT_LOADED, 0, VT_STRING);

	POSITION pos = pcbBoard->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = pcbBoard->getDataList().GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;
		
		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypePcbComponent || insert->getGraphicMirrored() != isBottom)
			continue;
		CString refdes = insert->getRefnameRef().Trim();

		CString shape = doc->getBlockAt(insert->getBlockNumber())->getName();
		if (sPackageName.CompareNoCase(PACKAGE_NAME_DUMMY) == 0)
			shape = PACKAGE_NAME_DUMMY;


		int x = round(insert->getOriginX() * sUnitFactor);
		int y = round(insert->getOriginY() * sUnitFactor);
		int rotation = round(normalizeDegrees(insert->getAngleDegrees()));
		bool compLoaded = true;

		if (sXYAdjustByCentroid || sRotationAdjustByCentroid) 
		{
			DataStruct *centroidData = doc->getBlockAt(insert->getBlockNumber())->GetCentroidData();  // centroid_exist_in_block(doc, doc->getBlockAt(insert->getBlockNumber()));
			if (centroidData != NULL)
			{
				if (sXYAdjustByCentroid)
				{
					CPoint2d point = centroidData->getInsert()->getOrigin2d();
					CTMatrix matrix = insert->getTMatrix();
					matrix.transform(point);

					x = round(point.x * sUnitFactor);
					y = round(point.y * sUnitFactor);
				}

				if (sRotationAdjustByCentroid)
					rotation = round(normalizeDegrees(insert->getAngleDegrees() + centroidData->getInsert()->getAngleDegrees())); 
			}
		}

		CString partNumber;
		CString commentChar;

		if (data->getAttributes() != NULL)
		{
			CAttribute* attrib = NULL;

         // DR 669263  If setting is not one of these then it is some other attribute to lookup, so do it.
         // We look it up and override shape value if attrib is present. If not present we leave shape as 
         // already defaulted. If the setting is PARTNUMBER then that gets set below where
         // partnumber is specifically handled instead of up here.
         if (sPackageName.CompareNoCase(PACKAGE_NAME_PARTNUMBER) != 0 &&  // Not PARTNUMBER
            sPackageName.CompareNoCase(PACKAGE_NAME_DUMMY) != 0 &&        // Not DUMMY
            sPackageName.CompareNoCase(PACKAGE_NAME_GEOMETRY) != 0)       // Not GEOMETRY
         {
            WORD someRandomAttribKey = doc->IsKeyWord(sPackageName, 0);
            if (data->getAttributes()->Lookup(someRandomAttribKey, attrib) && attrib != NULL)
            {
               shape = attrib->getStringValue();
            }
         }


			if (data->getAttributes()->Lookup(partNumberKey, attrib) && attrib != NULL)
			{
				partNumber = attrib->getStringValue();

				if (sPackageName.CompareNoCase(PACKAGE_NAME_PARTNUMBER) == 0)
					shape = partNumber;
			}
			else
				partNumber = shape;

			attrib = NULL;

			if (data->getAttributes()->Lookup(testStrategyKey, attrib) && attrib != NULL)
			{
				CString testStrategy = attrib->getStringValue();

				if (testStrategy.Find(ATT_VALUE_TEST_AOI) < 0)
					commentChar = "* ";
			}

			attrib = NULL;

			if (data->getAttributes()->Lookup(loadedKey, attrib) && attrib != NULL)
			{
				CString testGet = attrib->getStringValue();
				testGet.MakeUpper();

				// Comp is loaded if value is TRUE or YES
				compLoaded = (testGet.CompareNoCase("FALSE") == 0)?false:true;
			}
		}

		// TOM - Case 1204 - Add * to components that are not loaded
		if (!compLoaded)
				commentChar = "* ";

		refdes = ReplaceIllegalChars(refdes);
		shape = ReplaceIllegalChars(shape);
		partNumber = ReplaceIllegalChars(partNumber);

      shape = shape.Left(15);  // maximum of 15 chars
		partNumber = partNumber.Left(15);   // maximum of 15 chars
		
		fprintf(pFile, "%s%s\t%s\t%d\t%d\t%d\t%s\t0\t0\n", commentChar, refdes, shape, x, y, rotation, partNumber);
	}

	// Finished writing files
	fclose(pFile);
}

/******************************************************************************
* ReplaceIllegalChars
*/
CString ReplaceIllegalChars(CString ToReplace)
{
   // DR 794091 says no fields should have spaces or period. So added space and period to list of illegal chars.

	CString CharsToReplace = " .\\:/*?<>|";
	CString ReplaceWith = '_';
	CString NewString = ToReplace;
	while(CharsToReplace !="")
	{
		NewString.Replace(CharsToReplace.Left(1), ReplaceWith);
		CharsToReplace = CharsToReplace.Right(CharsToReplace.GetLength() -1);
	}
	return NewString;

}

/******************************************************************************
* WriteBoardFiducial
*/
void WriteBoardFiducial(CCEtoODBDoc *doc, BlockStruct *pcbBoard, CString fileName, bool isBottom, CString fidName, bool fidAsComp)
{
	// Open the output file
	FILE *pFile = FileOpen(fileName);
	if (pFile == NULL)
		return;

	// Write header and comment
   CTime currentTime = CTime::GetCurrentTime();
	fprintf(pFile, "*---------------------------------------------------------------------\n");
	fprintf(pFile, "* Created by : %s\n", getApp().getCamCadSubtitle());
	fprintf(pFile, "* Date       : %s\n", currentTime.Format("%A, %B %d, %Y %I:%M:%S %p"));
	fprintf(pFile, "*\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "* REF-file\n");
	fprintf(pFile, "*\n");
	fprintf(pFile, "*---------------------------------------------------------------------\n");


	if (fidAsComp == false)
      fidName = "REF";

	int fidCount = 0;
	POSITION pos = pcbBoard->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = pcbBoard->getDataList().GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;
		
		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypeFiducial) // || insert->getGraphicMirrored() != isBottom)
			continue;
		if (insert->getGraphicMirrored() != isBottom)
			continue;
		fidCount++;

		CString shape = doc->getBlockAt(insert->getBlockNumber())->getName();
		int x = round(insert->getOriginX() * sUnitFactor);
		int y = round(insert->getOriginY() * sUnitFactor);
		int rotation = round(normalizeDegrees(insert->getAngleDegrees()));
		fidName = ReplaceIllegalChars(fidName);
		shape = ReplaceIllegalChars(shape);
		fprintf(pFile, "%s_%d\t%s\t%d\t%d\t0\t%d\t0\t0\n", fidName, fidCount, shape, x, y, rotation);
	}

   // DR 794091 says two fids are required in ref file or it will not load into Viscom.
   // It suggests making up default fids. Okay, let's.
   bool first = true;
   for (int i = fidCount; i < 2; i++)
   {
      if (first)
      {
         fprintf(sErrFile, "Insufficient fiducials in %s side data, fabricated one or more fake fiducials for export.\n",
            isBottom?"bottom":"top");
         first = false;
      }
      CString shape("DoesNotExist");
      int x, y, rotation;
      x = y = rotation = 0;
      fprintf(pFile, "%s_%d\t%s\t%d\t%d\t0\t%d\t0\t0\n", fidName, (i+1), shape, x, y, rotation);
   }

	// Finished writing files
	fclose(pFile);
}

/******************************************************************************
* FileOpen
*/
FILE *FileOpen(CString fileName)
{
   FILE *pFile = fopen(fileName, "wt");
   if (pFile == NULL)
   {
      CString buf = "Can not open [%s]" + fileName;
      MessageBox(NULL, buf, "Error File Open", MB_OK | MB_ICONHAND);
   }

	return  pFile;
}

/******************************************************************************
* GetRotationAndMirror

	t1, t2, t3, t4 is  an expression to define for a given rotation and mirror
	The definition is in the PRD of Viscom AOI
*/
int GetRotationAndMirror(int rotation, bool mirror, int &t1, int &t2, int &t3, int &t4)
{
	t1 = 0;
	t2 = 0;
	t3 = 0;
	t4 = 0;
	int res = 0;

	if (rotation == 0 && mirror == false)
	{
		t1 = 1; t2 = 0; t3 = 0; t4 = 1;
	}
	else if (rotation == 0 && mirror == true) 
	{
		t1 = -1; t2 = 0; t3 = 0; t4 = 1;
	}
	else if (rotation == 180 && mirror == false) 
	{
		t1 = -1; t2 = 0; t3 = 0; t4 = -1;
	}
	else if (rotation == 180 && mirror == true) 
	{	
		t1 = 1; t2 = 0; t3 = 0; t4 = -1;
	}
	else if (rotation == 270 && mirror == false) 
	{
		t1 = 0; t2 = -1; t3 = 1; t4 = 0;
	}
	else if (rotation == 270 && mirror == true) 
	{
		t1 = 0; t2 = 1; t3 = 1; t4 = 0;
	}
	else if (rotation == 90 && mirror == false) 
	{
		t1 = 0; t2 = 1; t3 = -1; t4 = 0;
	}
	else if (rotation == 90 && mirror == false) 
	{
		t1 = 0; t2 = -1; t3 = -1; t4 = 0;
	}
	else
	{
		res = -1;
	}

	return res;
}

/******************************************************************************
* LoadSettingFile
*/
void LoadSettingFile(CString fileName)
{
	// Temporary variable, need to get the actual value from settin file "Viscom.out"
	sFidName = "FID";
	sFidAsComp = true;
	sWidthOffset = "50";  // Jesper Lykke by way of Mark says 50 is the conventional default for width offset.
	sRommelNum = "1";
   sRommelComment = true; // By default ROMMEL_NO is commented out in output.
	sPackageName = PACKAGE_NAME_GEOMETRY;
	sXYAdjustByCentroid = false;
	sRotationAdjustByCentroid = false;
	sFiducialByBoard = false;

	// Open the output file
	FILE *pFile = fopen(fileName, "rt");
   if (pFile == NULL)
   {
      // no settings file found 
      CString tmp;
      tmp.Format("File [%s] not found", fileName);
      ErrorMessage(tmp, "Viscom AOI Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   char line[255];
   char *lp;
   while (fgets(line, 255, pFile))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".FID_NAME"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				sFidName = lp;
			}
         else if (!STRICMP(lp, ".FIDUCIAL_AS_COMPONENT"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				if (toupper(lp[0]) == 'N')
					sFidAsComp = FALSE;
			}
         else if (!STRICMP(lp, ".WIDTH_OFFSET"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				sWidthOffset = lp;
			}
         else if (!STRICMP(lp, ".ROMMEL_NO"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				sRommelNum = lp;
            sRommelComment = false; // Un-comment Rommel line when .ROMMEL_NO command is present.
			}
         else if (!STRICMP(lp, ".PACKAGE_NAME"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				sPackageName = lp;
			}
         else if (!STRICMP(lp, ".XY_ADJUST"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				CString adjustOption = lp;
				if (adjustOption.CompareNoCase("centroid") == 0)
					sXYAdjustByCentroid = true;
			}
         else if (!STRICMP(lp, ".ROT_ADJUST"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				CString adjustOption = lp;
				if (adjustOption.CompareNoCase("centroid") == 0)
					sRotationAdjustByCentroid = true;
			}
         else if (!STRICMP(lp, ".FIDUCIAL"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;
				CString fiducialOption = lp;
				if (fiducialOption.CompareNoCase("board") == 0)
					sFiducialByBoard = true;
			}
		}
	}

	fclose(pFile);
}