// $Header: /CAMCAD/5.0/read_wrt/ExcelOut.cpp 40    5/29/07 6:14p Lynn Phung $

/****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "gerblib.h"
#include "font.h"
#include <math.h>
#include "lyr_file.h"
#include "gauge.h"
#include "fixture_out.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg  *progress;

/* Structures Section ***************************************************************************/

typedef struct
{
   int   toolnr;
   double   x,y;
}ExcellonOut_Drill;
typedef CArray<ExcellonOut_Drill, ExcellonOut_Drill&> CDrillArray;


/******************************************************************************
* CExcellonPilotDrillSolution 
*/
class CExcellonPilotDrillSolution : protected CPilotDrillSolution
{
public:
	CExcellonPilotDrillSolution(CCEtoODBDoc *doc, CTestPlan *testPlan, FileStruct *file=NULL) : CPilotDrillSolution(doc, testPlan, file) {};
	~CExcellonPilotDrillSolution() {};

	void DumpToFile(CFormatStdioFile &file, int indent);
	int RunAnalysis();

	void WriteTools(FILE *fp, double scale, int &maxTCode);
	void WriteDrills(FILE *fp, int metric, int zeroSuppression, int &maxTCode, double unitsFactor, int wholeDigits, int significantDigits, int probeSurface);
};
class CExcellonPilotDrillSolutionMap : public CTypedMapStringToPtrContainer<CExcellonPilotDrillSolution*>
{
public:
	CExcellonPilotDrillSolutionMap() {};
	CExcellonPilotDrillSolutionMap(CCEtoODBDoc *doc, FileStruct *file);
};


/* Static Variables Section ***************************************************************************/

static CCEtoODBDoc    *doc;
static FILE          *flog;
static int           display_error;
static double        SMALL_DELTA = 1;

static CDrillArray   drillarray;
static int           drillcnt;
static int				drillTypes;
static int				probeDrillSurface;

static bool				IncludeToolingHoles;

static FormatStruct *ExcGlobalFormat = NULL;

static CExcellonPilotDrillSolutionMap *pilotDrillSolutions;

/* Prototype Section ***************************************************************************/

static void Excellon_WriteData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, double mmscale, bool filterInserts);
static int write_tools(FILE *fp, double scale, int wholeDigits, int significantDigits);
static int print_xy(FILE *fp, double x, double y, int metric, int zeroSuppression, int wholeDigits, int significantDigits);
static int write_drilldata(FILE *fp, int metric, int zeroSuppression, int wholeDigits, int significantDigits);
static int process_pilotdrills(FileStruct *lastVisibleFile);
static int write_pilotdrills(FILE *fp, int metric, int zeroSuppression, FileStruct *lastVisibleFile, double unitsFactor, int wholeDigits, int significantDigits, int probeSurface);


/******************************************************************************
* EXCELLON_WriteFiles
*/
void EXCELLON_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int page_units, double scale)
{
   doc = Doc;
   double unitsFactor = Units_Factor(page_units, UNIT_INCHES);
	double mmFactor = Units_Factor(page_units, UNIT_MM);
   display_error = FALSE;
	pilotDrillSolutions = NULL;
	drillTypes = format->EX.drillTypes;
	probeDrillSurface = format->EX.probeDrillSurface;
	IncludeToolingHoles = format->EX.includeToolingHoles ? true : false;

	bool includePilotHoles = format->EX.includePilotHoles?true:false;
	

	ExcGlobalFormat = format;
       
   // open file for writting
   FILE *wfp;
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      ErrorMessage("Error open file", filename);
      return;
   }

   CString logFile = GetLogfilePath("EXCELLON.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   drillarray.SetSize(100,100);
   drillcnt = 0;

   double toolFactor;
   int decimal;   
   if (format->EX.units == 1) // mm
   {
      fprintf(wfp, "M48\n");
      fprintf(wfp, "VER,1\n");
      fprintf(wfp, "FMAT,2\n");
      if (format->EX.zeroSuppression == 0)     // 0-Leading; 1-Trailing suppression; 2-No suppression
         fprintf(wfp, "METRIC,TZ,0000.00\n");
      else
         fprintf(wfp, "METRIC,LZ,00d\n");
      unitsFactor = Units_Factor(page_units, UNIT_MM);
      toolFactor =  Units_Factor(page_units, UNIT_MM);
      decimal = 2;
   }
   else
   {
      fprintf(wfp, "M48\n");
      fprintf(wfp, "VER,1\n");
      fprintf(wfp, "FMAT,2\n");
      if (format->EX.zeroSuppression == 0)     // 0-Leading; 1-Trailing suppression
         fprintf(wfp, "INCH,TZ\n");
      else
         fprintf(wfp, "INCH,LZ\n");
      unitsFactor = Units_Factor(page_units, UNIT_MILS);
      toolFactor =  Units_Factor(page_units, UNIT_INCHES);
      decimal = 1;
   }

	FileStruct *lastVisibleFile = doc->getFileList().GetLastShown(blockTypePcb);

	if (drillTypes==1 && includePilotHoles)
		process_pilotdrills(lastVisibleFile);

	// here do tools are in INCHES
   write_tools(wfp, toolFactor, format->EX.wholeDigits, format->EX.significantDigits);

   fprintf(wfp, "DETECT,ON\n");
   fprintf(wfp, "%%\n");
   if (format->EX.units == 1) // metric
   {
      // default 0000.00 1/100 mm
      fprintf(wfp, "M71\n");
   }
   else
   {
      // inches is always 00.0000   1/10 mil
      fprintf(wfp, "M72\n");
   }
   fprintf(wfp, "G90\n");

   SMALL_DELTA = 1;
   for (int i=0; i<decimal; i++)
      SMALL_DELTA *= 0.1;

   // EXPLODEALL,
   //   for each file:
   //     write entities of block (including any embedded inserts)
   for (POSITION filePos = doc->getFileList().GetHeadPosition(); filePos!=NULL; doc->getFileList().GetNext(filePos))
   {
      FileStruct *file = doc->getFileList().GetAt(filePos);

      if (!file->isShown())
         continue;
      if (file->getBlock() == NULL)
         continue;   // empty file

      Excellon_WriteData(&(file->getBlock()->getDataList()), file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, file->getScale() * mmFactor, true);
   }

   write_drilldata(wfp, format->EX.units, format->EX.zeroSuppression, format->EX.wholeDigits, format->EX.significantDigits);
	if (drillTypes==1 && includePilotHoles)
		write_pilotdrills(wfp, format->EX.units, format->EX.zeroSuppression, lastVisibleFile, unitsFactor, format->EX.wholeDigits, format->EX.significantDigits, format->EX.probeDrillSurface);

	// end of file
   fprintf(wfp,"M30\n");

   // close write file
   fclose(wfp);
   fclose(flog);

	delete pilotDrillSolutions;
	pilotDrillSolutions = NULL;

	drillarray.RemoveAll();

   if (display_error)
      Logreader(logFile);

   return;
}

static int getMaxTCode()
{
	int maxTCode = -1;

   // first do AM - this are all non round or square app
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (!block->getMarked())
         continue;   // is a block selected 

      // make sure this is a tool
      if ( !(block->getFlags() & BL_TOOL) && !(block->getFlags() & BL_BLOCK_TOOL) )
			continue;

		if (maxTCode < block->getTcode())
			maxTCode = block->getTcode();
   }

   return maxTCode;
}


/******************************************************************************
* write_tools

   T1F00S00C0.025
   T2F00S00C0.028
   T3F00S00C0.030
   T4F00S00C0.032
*/
static int write_tools(FILE *fp, double scale, int wholeDigits, int significantDigits)
{
   // first do AM - this are all non round or square app
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (!block->getMarked())
         continue;   // is a block selected 

      // create necessary aperture blocks
      if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
      {
			if (ExcGlobalFormat && !ExcGlobalFormat->EX.includeDecimalPoint)
			{
				// Old format, probably should be 4.3, but 1.3 is how it was, and how it was is what is wanted
				// Note decimal point is included, the includeDecimalPoint flag does not apply to decimal here,
				// but is indicator that we want the old format.
				fprintf(fp,"T%02dF00S00C%1.3lf\n", block->getTcode(), block->getToolSize()*scale);
			}
			else
			{
				// Use digits controls for format
				fprintf(fp,"T%02dF00S00C%0*.*lf\n", block->getTcode(), wholeDigits+significantDigits+1, significantDigits, block->getToolSize()*scale);
			}
		}
   }

	// write the pilot drill tools
	if (pilotDrillSolutions != NULL)
	{
		int maxTCode = getMaxTCode();
		if (maxTCode < 0)
			maxTCode = 0;

		POSITION solutionPos = pilotDrillSolutions->GetStartPosition();
		while (solutionPos)
		{
			CString pcbBlockName;
			CExcellonPilotDrillSolution *drillSolution = NULL;
			pilotDrillSolutions->GetNextAssoc(solutionPos, pcbBlockName, drillSolution);

			drillSolution->WriteTools(fp, scale, maxTCode);
		}
	}

	return 1;
}

/******************************************************************************
* print_xy

   Zero Suppresson: 
   0 = Leading zero suppression
   1 = Trailing zero suppression
	2 = No zero suppression
*/
static int print_xy(FILE *fp, double x, double y, int metric, int zeroSuppressionIGNORED, int wholeDigits, int significantDigits)
{
   double lx = x, ly = y;

	// Due to change in spec, we no longer care about zeroSuppression (050106 - AC)
	// Case 1489, Customers still seem to care about zero suppression. (050930 - rcf)
	
	if (ExcGlobalFormat && ExcGlobalFormat->EX.includeDecimalPoint)
	{
		// If we are set to inches, incoming values are in mils.
		// But if we are set to metric, incoming values are already in mm.
		if (metric == 0)
		{
			lx /= 1000;
			ly /= 1000;
		}

		CString xCoord, yCoord;
		xCoord.Format("%.*f", significantDigits, lx);
		yCoord.Format("%.*f", significantDigits, ly);

		xCoord = xCoord.TrimLeft("0");
		yCoord = yCoord.TrimLeft("0");
		fprintf(fp, "X%sY%s\n", xCoord, yCoord);
	}
	else
	{
		// Zero Suppresson: 
		// 0 = Leading zero suppression
		// 1 = Trailing zero suppression
		// 2 = No zero suppression

		int zeroSuppression = 2;
		if (ExcGlobalFormat)
			zeroSuppression = ExcGlobalFormat->EX.zeroSuppression;

		// If we are set to inches, incoming values are in mils.
		// But if we are set to metric, incoming values are already in mm.
		if (metric == 1)
		{
			lx *= 100.0;
			ly *= 100.0;
		}
		else
		{
			lx *= 10.0;  // "Inch" is in 1/10 mil units
			ly *= 10.0;
		}

		CString xCoord;
		xCoord.Format("%06d", DcaRound(fabs(lx)));
		CString yCoord;
		yCoord.Format("%06d", DcaRound(fabs(ly)));

		if (zeroSuppression == 0)
		{
			// Leading Suppression
			xCoord.TrimLeft("0");
			yCoord.TrimLeft("0");
		}
		else if (zeroSuppression == 1)
		{
			// Trailing suppression
			xCoord.TrimRight("0");
			yCoord.TrimRight("0");
		}
		// else nothing needed for No Zero Suppression

		CString xyOut;
		xyOut.Format("X%s%sY%s%s",
			lx < 0.0 ? "-" : "", xCoord,
			ly < 0.0 ? "-" : "", yCoord);

		fprintf(fp, "%s\n", xyOut);

	}


   return 1;
}

/******************************************************************************
* write_drilldata

      T01
      X03279Y00094
      Y00394
      X03879
*/
static int write_drilldata(FILE *fp, int metric, int zeroSuppression, int wholeDigits, int significantDigits)
{
   int used_tool[1000];
	int i=0;
   for (i=0; i<1000; i++)
      used_tool[i] = 0;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (!block->getMarked())
         continue;   // is a block selected 

      // create necessary aperture blocks
      if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
      {
         if (block->getTcode() >= 0 && block->getTcode() < 1000)
            used_tool[block->getTcode()] = 1;
         else
         {
            fprintf(flog, "Tool code [%d] for Tool [%s] out of range\n", block->getTcode(), block->getName());
            display_error++;
         }
      }
   }

   for (int ii=0; ii<1000; ii++)
   {
      if (!used_tool[ii])
         continue;
      fprintf(fp, "T%02d\n", ii);

      for (int i=0; i<drillcnt; i++)
      {
         ExcellonOut_Drill t = drillarray.GetAt(i);
         if (t.toolnr == ii)
            print_xy(fp, t.x, t.y, metric, zeroSuppression, wholeDigits, significantDigits); // koos are in INCH or MM dependent on
      }
   }

   return 1;
}

void CExcellonPilotDrillSolution::DumpToFile(CFormatStdioFile &file, int indent)
{
	CPilotDrillSolution::DumpToFile(file, indent);
}

int CExcellonPilotDrillSolution::RunAnalysis()
{
	return CPilotDrillSolution::RunAnalysis();
}

void CExcellonPilotDrillSolution::WriteTools(FILE *fp, double scale, int &maxTCode)
{
	CMapStringToString toolMap;

	// gather probes grouping them by template names (drill/tool sizes)
	POSITION netPos = m_ppNets.GetStartPosition();
	while (netPos)
	{
		CString netName;
		CPPNet *ppNet = NULL;
		m_ppNets.GetNextAssoc(netPos, netName, ppNet);

		for (POSITION probePos=ppNet->GetHeadPosition_Probes(); probePos!=NULL; ppNet->GetNext_Probes(probePos))
		{
			CPPProbe *probe = ppNet->GetAt_Probes(probePos);
			CString temp;

			if (!probe->IsPlaced())
				continue;

			if (!toolMap.Lookup(probe->GetProbeTemplateName(), temp))
			{
				POSITION dummyPos = NULL;
				CPPAccessibleLocation *pAccLoc = probe->GetAccessibleLocation();
				if (pAccLoc == NULL)
					continue;

				CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_Probe(probe->GetProbeTemplateName(), dummyPos, pAccLoc->GetSurface());
				if (pTemplate == NULL)
					continue;

		      fprintf(fp,"T%02dF00S00C%1.3lf\n", ++maxTCode, pTemplate->GetDiameter()*Units_Factor(pageUnitsMils, m_pDoc->getSettings().getPageUnits())*scale);
				toolMap.SetAt(probe->GetProbeTemplateName(), probe->GetProbeTemplateName());
			}				
		}
	}
}

class CPPProbeListItem
{
public:
	CString m_sTemplateName;
	CPPProbeList m_list;
};
void CExcellonPilotDrillSolution::WriteDrills(FILE *fp, int metric, int zeroSuppression, int &maxTCode, double unitsFactor, int wholeDigits, int significantDigits, int probeSurface)
{
	CTypedMapStringToPtrContainer<CPPProbeListItem*> templateToProbeMap(10, false);
	CTypedPtrListContainer<CPPProbeListItem*> templateToProbeList(false);

	// gather probes grouping them by template names (drill/tool sizes)
	POSITION netPos = m_ppNets.GetStartPosition();
	while (netPos)
	{
		CString netName;
		CPPNet *ppNet = NULL;
		m_ppNets.GetNextAssoc(netPos, netName, ppNet);

		for (POSITION probePos=ppNet->GetHeadPosition_Probes(); probePos!=NULL; ppNet->GetNext_Probes(probePos))
		{
			CPPProbe *probe = ppNet->GetAt_Probes(probePos);
			CPPProbeListItem *probeListItem = NULL;

			if (!probe->IsPlaced() || probe->GetProbeTemplateName().IsEmpty())
				continue;

			if (!templateToProbeMap.Lookup(probe->GetProbeTemplateName(), probeListItem))
			{
				probeListItem = new CPPProbeListItem();
				probeListItem->m_sTemplateName = probe->GetProbeTemplateName();
				templateToProbeMap.SetAt(probe->GetProbeTemplateName(), probeListItem);
				templateToProbeList.AddTail(probeListItem);
			}				
			probeListItem->m_list.AddTail(probe);
		}
	}

	// write the pilot drills
	//POSITION tCodePos = templateToProbeMap.GetStartPosition();
	POSITION tCodePos = templateToProbeList.GetHeadPosition();
	while (tCodePos)
	{
		//CString templateName;
		//CPPProbeList *probeList = NULL;
		//templateToProbeMap.GetNextAssoc(tCodePos, templateName, probeList);
		CPPProbeListItem *probeListItem = templateToProbeList.GetNext(tCodePos);

		fprintf(fp, "T%02d\n", ++maxTCode);

		for (POSITION probePos=probeListItem->m_list.GetHeadPosition(); probePos!=NULL; probeListItem->m_list.GetNext(probePos))
		{
			CPPProbe *probe = probeListItem->m_list.GetAt(probePos);

			if (!probe->IsPlaced())
				continue;

			CFixtureAccLoc *pAccLoc = (CFixtureAccLoc*)probe->GetAccessibleLocation();
			if (pAccLoc == NULL)
				continue;

			//		surface != BOTH	surface of probe != BOTH	surface of probe is not the same
			if (probeSurface!=2 && pAccLoc->GetSurface()!=2 && pAccLoc->GetSurface() != probeSurface)
				continue;
			if (!pAccLoc->IsForFixtureFile())
				continue;
			if (!pAccLoc->IsProbed())
				continue;

         print_xy(fp, pAccLoc->GetLocation().x*unitsFactor, pAccLoc->GetLocation().y*unitsFactor, metric, zeroSuppression, wholeDigits, significantDigits);
		}
	}
}

CExcellonPilotDrillSolutionMap::CExcellonPilotDrillSolutionMap(CCEtoODBDoc *doc, FileStruct *file)
{
	if (doc == NULL || file == NULL)
		return;

	if (file->getBlockType() == blockTypePanel)
	{
		// create a pilot drill solution for every board in the panel with a board file
		BlockStruct *panelBlock = file->getBlock();
		if (panelBlock == NULL)
			return;

		for (POSITION boardPos=panelBlock->getHeadDataInsertPosition(); boardPos!=NULL; panelBlock->getNextDataInsert(boardPos))
		{
			DataStruct *boardData = panelBlock->getAtData(boardPos);
			InsertStruct *boardInsert = boardData->getInsert();

			if (boardInsert->getInsertType() != insertTypePcb)
				continue;

			FileStruct *boardFile = doc->getFileList().FindByBlockNumber(boardInsert->getBlockNumber());
			if (boardFile == NULL)
				continue;

			/*if (doc->GetCurrentDFTSolution(*boardFile) == NULL)
				continue;*/

			CExcellonPilotDrillSolution *drillSolution = NULL;
			if (Lookup(boardFile->getBlock()->getName(), drillSolution))
				continue;

			/*drillSolution = new CExcellonPilotDrillSolution(doc, doc->GetCurrentDFTSolution(*boardFile)->GetTestPlan(), boardFile);
			SetAt(boardFile->getBlock()->getName(), drillSolution);*/
		}
	}
	else if (file->getBlockType() == blockTypePcb)
	{
		/*if (doc->GetCurrentDFTSolution(*file) != NULL)
		{
			CExcellonPilotDrillSolution *drillSolution = new CExcellonPilotDrillSolution(doc,doc->GetCurrentDFTSolution(*file)->GetTestPlan(), file);
			SetAt(file->getBlock()->getName(), drillSolution);
		}*/
	}
}

static int process_pilotdrills(FileStruct *lastVisibleFile)
{
	if (lastVisibleFile == NULL)
		return 0;

	// create a pilot drill solutions
	pilotDrillSolutions = new CExcellonPilotDrillSolutionMap(doc, lastVisibleFile);
	if (pilotDrillSolutions == NULL)
		return 0;

	// loop through all the solutions and run the analysis
	POSITION pos=pilotDrillSolutions->GetStartPosition();
	while (pos)
	{
		CString pcbBlockName;
		CExcellonPilotDrillSolution *drillSolution = NULL;
		pilotDrillSolutions->GetNextAssoc(pos, pcbBlockName, drillSolution);

		drillSolution->RunAnalysis();
	}

	CFileException e;
	CFormatStdioFile file;
   CString dumpFileName( getApp().getSystemSettingsFilePath("excellon.dbg") );
	if (file.Open(dumpFileName, CFile::modeCreate|CFile::modeWrite, &e))
	{
		file.WriteString("Filename: %s\n", dumpFileName);
		CTime t;
		t = t.GetCurrentTime();
		file.WriteString("%s\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));
		file.WriteString("\n");

		pos = pilotDrillSolutions->GetStartPosition();
		while (pos)
		{
			CString pcbBlockName;
			CExcellonPilotDrillSolution *drillSolution = NULL;
			pilotDrillSolutions->GetNextAssoc(pos, pcbBlockName, drillSolution);

			file.WriteString("--------------------------------------\n");
			file.WriteString("Pilot Drill Solution for %s\n", pcbBlockName);
			drillSolution->DumpToFile(file, 3);
		}

		file.Close();
	}

	return 1;
}

static int write_pilotdrills(FILE *fp, int metric, int zeroSuppression, FileStruct *lastVisibleFile, double unitsFactor, int wholeDigits, int significantDigits, int probeSurface)
{
	if (pilotDrillSolutions == NULL)
		return 0;

	int maxTCode = getMaxTCode();
	if (maxTCode < 0)
		maxTCode = 0;

	POSITION solutionPos = pilotDrillSolutions->GetStartPosition();
	while (solutionPos)
	{
		CString pcbBlockName;
		CExcellonPilotDrillSolution *drillSolution = NULL;
		pilotDrillSolutions->GetNextAssoc(solutionPos, pcbBlockName, drillSolution);

		drillSolution->WriteDrills(fp, metric, zeroSuppression, maxTCode, unitsFactor, wholeDigits, significantDigits, probeSurface);
	}

	return 1;
}

/******************************************************************************
* test_duplicate
*/
static int test_duplicate(int tcode, double x, double y)
{
   for (int i=0; i<drillcnt; i++)
   {
      ExcellonOut_Drill t = drillarray.GetAt(i);
      if (t.toolnr == tcode && fabs(t.x - x) < SMALL_DELTA && fabs(t.y - y) < SMALL_DELTA)
         return 1;
   }
   return 0;
}

/******************************************************************************
* Excellon_WriteData
*/
void Excellon_WriteData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror,
                    double scale, double mmscale, bool filterInserts)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   int PenNum;
   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
      {
         // insert has not always a layer definition.
         if (data->getLayerIndex() < 0)
            PenNum = 1;
         else if (!doc->get_layer_visible(data->getLayerIndex(), mirror))
            continue;
      }

      switch(data->getDataType())
      {
      case T_POLY:
         {
 				CPolyList *polyList = data->getPolyList();
				if (polyList->GetCount() < 1)
					continue;
				// TOM - Case 1179 - This is where a problem occurs.  Poly with -1 width index
            CPoly *poly = polyList->GetHead();

				short polyWidthIndex = poly->getWidthIndex();
				if (polyWidthIndex < 0)
					continue;

				BlockStruct *blockStructPtr = doc->getWidthTable()[polyWidthIndex];
				double sizeA = blockStructPtr->getSizeA();

            double lineWidth = sizeA * scale;
            double w = sizeA;
         
          
            // loop thru polys without width
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               int first = TRUE;

               // this is drawn with the original dcode of the unfilled poly.
               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);

                  Point2 p;
                  p.x = pnt->x * scale;
                  if (mirror)
                     p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;

                  TransPoint2(&p, 1, &m, insert_x, insert_y);

                  // here deal with bulge
                  if (first)
                     first = FALSE;
               }
            }
         }
         break;  // POLYSTRUCT
  
      case T_TEXT:
         {
            Point2 p1;
            p1.x = (data->getText()->getPnt().x) * scale;
            p1.y = (data->getText()->getPnt().y) * scale;

            double text_rot;
            if (mirror)
            {
               text_rot = rotation - data->getText()->getRotation();
               p1.x = -p1.x;
            }
            else
            {
               text_rot = rotation + data->getText()->getRotation();
            }

            TransPoint2(&p1, 1, &m, insert_x, insert_y);

            int text_mirror = data->getText()->getResultantMirror(mirror);
         }
         break;

      case T_INSERT:
         {
				////CString strD = insertTypeToDisplayString(data->getInsert()->getInsertType());
				////CString str  = insertTypeToString(data->getInsert()->getInsertType());

				// Case 1535 - If we are processing a panel view, then override the
				// insert filtering. Let control pass through to the next recursive
				// call, which will be the datalist of the pcb, and then we'll
				// apply the filter. When recursed into an insert (eg a Probe) do
				// not filter, the parent was already filtered.

				if (data->getInsert()->getInsertType() != insertTypePcb)
				{
					// not clear why embeddedlevel matters, but that's what was here before

					if (filterInserts)
					{
						// skip if we are are excluding tool holes and this is one
						// same deal for both drill type exports
						if (data->getInsert()->getInsertType() == insertTypeDrillTool)
						{
							// tool holes

							if (!IncludeToolingHoles)
								continue;
						}
						else
						{
							// everything else

							if (drillTypes == 1 /*probe drills*/ && data->getInsert()->getInsertType() != insertTypeTestProbe)
								continue;

							// skip if we are doing PCB drills and this is a test probe
							if (drillTypes == 0 /*PCB drills*/ && data->getInsert()->getInsertType() == insertTypeTestProbe)
								continue;

							if (data->getInsert()->getInsertType() == insertTypeTestProbe)
							{
								int probeSurface = data->getInsert()->getGraphicMirrored();
								if (probeDrillSurface!=2 && probeSurface!=2 && probeDrillSurface!=probeSurface)
									continue;
							}
						}
					}
				}

				if (data->getInsert()->getInsertType() == insertTypeTestProbe)
				{
					Attrib *attrib = NULL;
					WORD placedKw = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
					if (!data->lookUpAttrib(placedKw, attrib))
						continue;

					CString placementString = get_attvalue_string(doc, attrib);
					if (placementString.CompareNoCase("Placed"))
						continue;
				}


				Point2 p1;
				p1.x = data->getInsert()->getOriginX() * scale;
				if (mirror)
					p1.x = -p1.x;
				p1.y = data->getInsert()->getOriginY() * scale;

				TransPoint2(&p1, 1, &m, insert_x, insert_y);

				// insert if mirror is either global mirror or block_mirror, but not if both.
				int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
				double block_rot;

				if (mirror)
					block_rot = rotation - data->getInsert()->getAngle();
				else
					block_rot = rotation + data->getInsert()->getAngle();

				BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
				if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
				{

					if (!block->getMarked())
						break;   // is a block selected 

					if (!test_duplicate(block->getTcode(), p1.x, p1.y))
					{
						ExcellonOut_Drill drill;
						drill.toolnr = block->getTcode();
						drill.x = p1.x;
						drill.y = p1.y;
						drillarray.SetAtGrow(drillcnt++, drill);  
					}
				}
				else
				{
					Excellon_WriteData(&(block->getDataList()), p1.x, p1.y,  block_rot, block_mirror, scale * data->getInsert()->getScale(),
						mmscale * data->getInsert()->getScale(), (data->getInsert()->getInsertType() == insertTypePcb) );
				} 
			}

			break;
      }
   }
} 
