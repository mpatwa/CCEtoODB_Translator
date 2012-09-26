// $Header: /CAMCAD/5.0/read_wrt/Huntrn_o.cpp 44    6/17/07 8:57p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "float.h"
#include "ccdoc.h"
#include "extents.h"
#include "find.h"
#include "pcbutil.h"
#include "net_util.h"
#include "xform.h"
#include "centroid.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

class CAccessPointMap
{
private:
   CTypedMapIntToPtrContainer<DataStruct*> m_entityToLinkedAccessPointMap;

public:
   CAccessPointMap(BlockStruct *pcbBlock);
   DataStruct* getMatchingAccessPoint(int entityNumber);

private:
   void init(BlockStruct *pcbBlock);

};


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static FILE *fp;
static CCEtoODBDoc *doc;
static FileStruct *panelFile;
static int decimals;
static CMapStringToString prefixMap;
static BOOL isHuntronStyle;

static CAccessPointMap *accessPointMap = NULL;


static void LoadHuntronSettings();
static void ExplodePcbNcPins();
static void GetSize(BlockStruct *block, int graphicClass, double *xSize, double *ySize);
static void GenerateRefNames(CCEtoODBDoc *doc);
static void WriteFiducials(BlockStruct *block);
static void WriteHoles(BlockStruct *block);
static void WriteBoards(BlockStruct *block);
static void WriteAllBoardsInfo();
static void WriteComponents(BlockStruct *boardGeom);
static void WriteVias(FileStruct *pcbFile, BlockStruct *boardGeom);
static void WriteCompPinTestpoints(CNetList *netList, BlockStruct *boardGeom, CFormatStdioFile &logFile);
static void WriteFeatureTestpoints(FileStruct *pcbFile, BlockStruct *boardGeom);
static void WriteProbes(FileStruct *pcbFile, BlockStruct *boardGeom, CFormatStdioFile &logFile);
static CString CleanString(CString string);
static bool GetHuntrnLogFile(CFormatStdioFile &logFile, CString outputfolderpath, CString logname);

/******************************************************************************
* Huntron_WriteFiles
*/
void Huntron_WriteFiles(const char *filename, CCEtoODBDoc *Doc)
{
   doc = Doc;
   decimals = GetDecimals(doc->getSettings().getPageUnits());

   // find panel  
   panelFile = NULL;
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {      
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() == BLOCKTYPE_PANEL && file->isShown())
      {
         if (panelFile)
         {
            MessageBox(NULL, "Only one Panel can be on", "Too many Panels", MB_OK | MB_ICONHAND);
            return;
         }

         panelFile = file;
      }
   }

   //
   CFormatStdioFile logFile;
   GetHuntrnLogFile(logFile, filename, "HuntrnFPT.log");

   // open file
   fp = fopen(filename, "wt");
   if (!fp)
   {
      CString buf;
      buf.Format("Can not open [%s]", filename);
      MessageBox(NULL, buf, "Error File Open", MB_OK | MB_ICONHAND);
      return;
   }

	LoadHuntronSettings();

	if (isHuntronStyle && !panelFile)
	{
		MessageBox(NULL, "Panel required for HAF export.\nMake sure a Panel exists and is On, then retry export.", "No Visible Panels", MB_OK | MB_ICONHAND);
		return;
	}

	generate_CENTROIDS(doc);
	GeneratePinNumbers(doc);
	GenerateRefNames(doc);	

	if (panelFile)
	{
		ExplodePcbNcPins();

		double xSize, ySize;
		GetSize(panelFile->getBlock(), GR_CLASS_PANELOUTLINE, &xSize, &ySize);

		// File Information Line Header
		fprintf(fp, "\"FILE TYPE\",\"PROGRAM\",\"VERSION\",\"PANEL NAME\",\"SOURCE FILENAME\",\"PANEL LENGTH\",\"PANEL WIDTH\",\"DATE\",\"TIME\",\"UNITS\"\n");

		// File Information Line Data
		CTime time = time.GetCurrentTime();
		fprintf(fp, "\"HAF ASCII CAD\",\"CAMCAD\",\"%s\",\"%s\",\"%s\",\"%.*lf\",\"%.*lf\",\"%s\",\"%s\",\"%s\"\n", 
				getApp().getVersionString(), 
				panelFile->getName(), 
				doc->docTitle, 
				decimals, xSize, 
				decimals, ySize, 
				time.Format("%B %d, %Y"), 
				time.Format("%I:%M%p"), 
				GetUnitName(doc->getSettings().getPageUnits()));

		fprintf(fp, "\n");
		fprintf(fp, "\n");

		// Record Information Line Header
		fprintf(fp, "\"RECORD IDENTIFIER\",\"BOARD NAME\",\"REFDES\",\"DEVICE CLASS\",\"GEOMETRY NAME\",\"PART NUMBER\",\"BOUNDARY SCAN\",\"PACKAGE\",\"VALUE\",\"PTOL\",\"NTOL\",\"NUMBER OF PINS\",\"X LOC\",\"Y LOC\",\"ROTATION\",\"SIDE\",\"HEIGHT\",\"TEST\",\"PIN NUM\",\"PIN NAME\",\"ACCESS SURFACE\",\"NET NAME\",\"NET POTENTIAL\",\"PIN FUNCTION\",\"PIN DIRECTION\",\"PIN XLOC (ACCESS X)\",\"PIN YLOC (ACCESS Y)\",\"XSIZE\",\"YSIZE\",\"PAD SHAPE\",\"TECHNOLOGY\",\"DRILL SIZE\",\"COMMENT\"\n");

		fprintf(fp, "\n");

		// Panel Fiducials
		WriteFiducials(panelFile->getBlock());

		// Panel Holes
		WriteHoles(panelFile->getBlock());

		// Panel Boards
		WriteBoards(panelFile->getBlock());

		fprintf(fp, "\n");
	}
	else
	{
		// File Information Line Header
		fprintf(fp, "\"FILE TYPE\",\"PROGRAM\",\"VERSION\",\"PANEL NAME\",\"SOURCE FILENAME\",\"PANEL LENGTH\",\"PANEL WIDTH\",\"DATE\",\"TIME\",\"UNITS\"\n");

		// File Information Line Data
		CTime time = time.GetCurrentTime();
		fprintf(fp, "\"HAF ASCII CAD\",\"CAMCAD\",\"%s\",\"%s\",\"%s\",\"%.*lf\",\"%.*lf\",\"%s\",\"%s\",\"%s\"\n", 
				getApp().getVersionString(), 
				"", 
				doc->docTitle, 
				decimals, 0.0, 
				decimals, 0.0, 
				time.Format("%B %d, %Y"), 
				time.Format("%I:%M%p"), 
				GetUnitName(doc->getSettings().getPageUnits()));

		fprintf(fp, "\n");
		fprintf(fp, "\n");

		// Record Information Line Header
		fprintf(fp, "\"RECORD IDENTIFIER\",\"BOARD NAME\",\"REFDES\",\"DEVICE CLASS\",\"GEOMETRY NAME\",\"PART NUMBER\",\"BOUNDARY SCAN\",\"PACKAGE\",\"VALUE\",\"PTOL\",\"NTOL\",\"NUMBER OF PINS\",\"X LOC\",\"Y LOC\",\"ROTATION\",\"SIDE\",\"HEIGHT\",\"TEST\",\"PIN NUM\",\"PIN NAME\",\"ACCESS SURFACE\",\"NET NAME\",\"NET POTENTIAL\",\"PIN FUNCTION\",\"PIN DIRECTION\",\"PIN XLOC (ACCESS X)\",\"PIN YLOC (ACCESS Y)\",\"XSIZE\",\"YSIZE\",\"PAD SHAPE\",\"TECHNOLOGY\",\"DRILL SIZE\",\"COMMENT\"\n");

		fprintf(fp, "\n");

		// Board Info of every board
		WriteAllBoardsInfo();

		fprintf(fp, "\n");
	}

   filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *pcbFile = doc->getFileList().GetNext(filePos);

      if (pcbFile->getBlockType() != BLOCKTYPE_PCB)
         continue;

		accessPointMap = new CAccessPointMap(pcbFile->getBlock());

      generate_PINLOC(doc, pcbFile, TRUE);

      // Board Fiducials
      WriteFiducials(pcbFile->getBlock());

      // Board Holes
      WriteHoles(pcbFile->getBlock());

      // Board Components
      WriteComponents(pcbFile->getBlock());

      // Board Vias
      WriteVias(pcbFile, pcbFile->getBlock());

      // Board Testpoints
      WriteCompPinTestpoints(&pcbFile->getNetList(), pcbFile->getBlock(), logFile);
      WriteFeatureTestpoints(pcbFile, pcbFile->getBlock());

      // Board Probes
      WriteProbes(pcbFile, pcbFile->getBlock(), logFile);

      fprintf(fp, "\n");

		delete accessPointMap;
   }
   
   fprintf(fp, "EOF\n");

   fclose(fp);

   //ErrorMessage("TIP: Save a CAMCAD Data file (*.CC) to retain all the ECAD data,\n        user changes and changes made during this export.",
   //      "Save a CAMCAD Data file (*.CC)", MB_ICONINFORMATION);
}

/******************************************************************************
* GetHuntrnLogFile
*/
static bool GetHuntrnLogFile(CFormatStdioFile &logFile, CString outputfolderpath, CString logname)
{
   CString logPath = GetLogfilePath(logname);
   if (!outputfolderpath.IsEmpty())
   {
      CFilePath logFilePath(outputfolderpath);
      logFilePath.setBaseFileName("HuntrnFPT");
      logFilePath.setExtension("log");
      logPath = logFilePath.getPath();
   }

   if (logFile.Open(logPath, CFile::modeWrite|CFile::modeCreate))
   {
      CString buf;
      CTime time = CTime::GetCurrentTime();

      logFile.WriteString("Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      logFile.WriteString("%s\n\n", time.Format("%A, %B %d, %Y - %X%p"));
      
      return true;
   }

   return false;
}

/******************************************************************************
* WriteFiducials
*/
void WriteFiducials(BlockStruct *block)
{
   int hafRefNameKW = doc->RegisterKeyWord(ATT_HAF_REFNAME, 0, VT_STRING);
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_FIDUCIAL)
         continue;

      BlockStruct *fiducialGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());
      doc->validateBlockExtents(fiducialGeom);

      CString boardName;
      if (block->getBlockType() == BLOCKTYPE_PANEL)
         boardName = "PANEL";
      else
         boardName = block->getName();

      CString refDes;
      if (data->getInsert()->getRefname())
         refDes = data->getInsert()->getRefname();
      else
         refDes.Empty();

      CString side;
      if (data->getInsert()->getPlacedBottom())
         side = "Bottom";
      else
         side = "Top";

      int padShape = T_UNDEFINED;
      POSITION padPos = fiducialGeom->getDataList().GetHeadPosition();
      while (padPos)
      {
         DataStruct *padData = fiducialGeom->getDataList().GetNext(padPos);

         if (padData->getDataType() != T_INSERT)
            continue;

         if (padData->getLayerIndex() == -1)
            continue;

         BlockStruct *padGeom = doc->getBlockAt(padData->getInsert()->getBlockNumber());

         LayerStruct *layer = doc->getLayerArray()[padData->getLayerIndex()];
         switch (layer->getLayerType())
         {
         case LAYTYPE_PAD_ALL:
         case LAYTYPE_PAD_TOP:
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_PAD_BOTTOM:
            if (!padShape)
               padShape = padGeom->getShape();
            break;
         case LAYTYPE_SIGNAL_TOP:
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_SIGNAL_BOT:
            if (!padShape)
               padShape = padGeom->getShape();
            break;
         }
      }

      CString comment;

      if (data->getAttributesRef())
      {
         Attrib* attrib;

         if (data->getAttributesRef()->Lookup(hafRefNameKW, attrib))
         {
            refDes = attrib->getStringValue();
         }

         if (data->getAttributesRef()->Lookup(hafCommentKW, attrib))
         {
            comment = attrib->getStringValue();
         }
      }

      double x = data->getInsert()->getOriginX();
      double y = data->getInsert()->getOriginY();
      DataStruct *centroidData = centroid_exist_in_block(doc->getBlockAt(data->getInsert()->getBlockNumber()));

      if (centroidData != NULL)
      {
         DTransform xform(data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 1, data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags());
         Point2 p;
         p.x = centroidData->getInsert()->getOriginX();
         p.y = centroidData->getInsert()->getOriginY();

         xform.TransformPoint(&p);

         x = p.x;
         y = p.y;
      }  

      fprintf(fp, "\"FIDUCIAL\",\"%s\",\"%s\",,\"%s\",,,,,,,,\"%.*lf\",\"%.*lf\",,\"%s\",,,,,,,,,,,,\"%.*lf\",\"%.*lf\",\"%s\",,,\"%s\"\n",
            CleanString(boardName),
            CleanString(refDes),
            CleanString(fiducialGeom->getName()),
            decimals, x,
            decimals, y,
            side,
            decimals, fiducialGeom->getXmax() - fiducialGeom->getXmin(), 
            decimals, fiducialGeom->getYmax() - fiducialGeom->getYmin(), 
            shapes[padShape],
            CleanString(comment));

      doc->SetAttrib(&data->getAttributesRef(), hafRefNameKW, VT_STRING, refDes.GetBuffer(0), SA_OVERWRITE, NULL);
   }
}

/******************************************************************************
* WriteHoles
*/
void WriteHoles(BlockStruct *block)
{
   int hafRefNameKW = doc->RegisterKeyWord(ATT_HAF_REFNAME, 0, VT_STRING);
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_TOOLING)
         continue;

      BlockStruct *holeGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());
      doc->validateBlockExtents(holeGeom);

      CString boardName;
      if (block->getBlockType() == BLOCKTYPE_PANEL)
         boardName = "PANEL";
      else
         boardName = block->getName();

      CString refDes;
      if (data->getInsert()->getRefname())
         refDes = data->getInsert()->getRefname();
      else
         refDes.Empty();

      CString side;
      if (data->getInsert()->getPlacedBottom())
         side = "Bottom";
      else
         side = "Top";

      double drillSize = 0;
      POSITION padPos = holeGeom->getDataList().GetHeadPosition();
      while (padPos)
      {
         DataStruct *padData = holeGeom->getDataList().GetNext(padPos);

         if (padData->getDataType() != T_INSERT)
            continue;

         if (padData->getLayerIndex() == -1)
            continue;

         BlockStruct *padGeom = doc->getBlockAt(padData->getInsert()->getBlockNumber());

         LayerStruct *layer = doc->getLayerArray()[padData->getLayerIndex()];
         if (layer->getLayerType() == LAYTYPE_DRILL)
            drillSize = padGeom->getToolSize();
      }

      CString comment;
      double accessX = data->getInsert()->getOriginX(), accessY = data->getInsert()->getOriginY();

      if (data->getAttributesRef())
      {
         Attrib* attrib;

         if (data->getAttributesRef()->Lookup(hafRefNameKW, attrib))
         {
            refDes = attrib->getStringValue();
         }
         
         if (data->getAttributesRef()->Lookup(hafCommentKW, attrib))
         {
            comment = attrib->getStringValue();
         }
      }

      fprintf(fp, "\"HOLE\",\"%s\",\"%s\",,\"%s\",,,,,,,,,,,,,,,,,,,,,\"%.*lf\",\"%.*lf\",,,,,\"%.*lf\",\"%s\"\n",
            CleanString(boardName),
            CleanString(refDes),
            CleanString(holeGeom->getName()),
            decimals, accessX,
            decimals, accessY,
            decimals, drillSize,
            CleanString(comment));

      doc->SetAttrib(&data->getAttributesRef(), hafRefNameKW, VT_STRING, refDes.GetBuffer(0), SA_OVERWRITE, NULL);
   }
}

/******************************************************************************
* WriteBoards
*/
void WriteBoards(BlockStruct *block)
{
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCB)
         continue;

      CString comment;

      if (data->getAttributesRef())
      {
         Attrib* attrib;

         if (data->getAttributesRef()->Lookup(hafCommentKW, attrib))
         {
            comment = attrib->getStringValue();
         } 
      }

      BlockStruct *boardGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

      double xSize, ySize;
      GetSize(boardGeom, GR_CLASS_BOARDOUTLINE, &xSize, &ySize);

      fprintf(fp, "\"BOARD\",\"%s\",\"%s\",,,,,,,,,,\"%.*lf\",\"%.*lf\",\"%.2lf\",\"%s\",,,,,,,,,,,,\"%.*lf\",\"%.*lf\",,,,\"%s\"\n",
            CleanString(boardGeom->getName()),
            (data->getInsert()->getRefname()?CleanString(data->getInsert()->getRefname()):""), 
            decimals, data->getInsert()->getOriginX(),
            decimals, data->getInsert()->getOriginY(),
            RadToDeg(data->getInsert()->getAngle()), 
            (data->getInsert()->getMirrorFlags()?"Bottom":"Top"),
            decimals, xSize, 
            decimals, ySize,
            CleanString(comment));
   }
}

/******************************************************************************
* WriteAllBoardsInfo

	- This function is called to write out board information when there is no
	  panel.  Only happend for Goepel style.
*/
void WriteAllBoardsInfo()
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *pcbFile = doc->getFileList().GetNext(filePos);
      if (pcbFile->getBlockType() != BLOCKTYPE_PCB)
         continue;

      double xSize, ySize;
      GetSize(pcbFile->getBlock(), GR_CLASS_BOARDOUTLINE, &xSize, &ySize);

      fprintf(fp, "\"BOARD\",\"%s\",\"%s\",,,,,,,,,,\"%.*lf\",\"%.*lf\",\"%.2lf\",\"%s\",,,,,,,,,,,,\"%.*lf\",\"%.*lf\",,,,\"%s\"\n",
            CleanString(pcbFile->getBlock()->getName()),
            "",					// board insert on panel
            decimals, 0.0,		// panel length
            decimals, 0.0,		// panel width
            0.0,					// panel rotation
            "TOP",				// Always TOP because it is not panel insert
            decimals, xSize, 
            decimals, ySize,
            "");					// No comment
	}
}

/******************************************************************************
* WriteComponents
*/
void WriteComponents(BlockStruct *boardGeom)
{
   int hafDeviceClassKW = doc->RegisterKeyWord(ATT_HAF_DEVICECLASS, 0, VT_STRING);
   int hafPartNumberKW = doc->RegisterKeyWord(ATT_HAF_PARTNUMBER, 0, VT_STRING);
   int hafPackageKW = doc->RegisterKeyWord(ATT_HAF_PACKAGE, 0, VT_STRING);
   int hafValueKW = doc->RegisterKeyWord(ATT_HAF_VALUE, 0, VT_STRING);
   int hafPTolKW = doc->RegisterKeyWord(ATT_HAF_PTOL, 0, VT_DOUBLE);
   int hafNTolKW = doc->RegisterKeyWord(ATT_HAF_NTOL, 0, VT_DOUBLE);
   int hafCompHeightKW = doc->RegisterKeyWord(ATT_HAF_COMPHEIGHT, 0, VT_UNIT_DOUBLE);
   int hafTestKW = doc->RegisterKeyWord(ATT_HAF_TEST, 0, VT_STRING);
   int hafBoundaryScanKW = doc->RegisterKeyWord(ATT_HAF_BOUNDARYSCAN, 0, VT_STRING);
   int hafNetPotentialKW = doc->RegisterKeyWord(ATT_HAF_NETPOTENTIAL, 0, VT_STRING);
   int hafPinDirectionKW = doc->RegisterKeyWord(ATT_HAF_PINDIRECTION, 0, VT_STRING);
   int hafPinFunctionKW = doc->RegisterKeyWord(ATT_HAF_PINFUNCTION, 0, VT_STRING);
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);

   POSITION compPos = boardGeom->getDataList().GetHeadPosition();
   while (compPos)
   {
      DataStruct *compData = boardGeom->getDataList().GetNext(compPos);

      if (compData->getDataType() != T_INSERT)
         continue;

      if (compData->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      BlockStruct *compGeom = doc->getBlockAt(compData->getInsert()->getBlockNumber());
      doc->validateBlockExtents(compGeom);

      int pinCount = 0;
      POSITION pinPos = compGeom->getDataList().GetHeadPosition();
      while (pinPos)
      {
         DataStruct *pinData = compGeom->getDataList().GetNext(pinPos);

         if (pinData->getDataType() == T_INSERT && pinData->getInsert()->getInsertType() == INSERTTYPE_PIN)
            pinCount++;
      }


      CString deviceClass;

      if (compData->getInsert()->getRefname())
      {
         CString prefix;
         int i=0;
         while (isalpha(compData->getInsert()->getRefname()[i]))
            prefix += compData->getInsert()->getRefname()[i++];

         prefix.MakeUpper();

         prefixMap.Lookup(prefix, deviceClass);
      }


      CString partNumber;
      CString package;
      CString value;
      CString pTolString;
      CString nTolString;
      CString compHeightString;
      CString test = "TRUE";
      CString boundaryScan;
      CString comment;

      if (compData->getAttributesRef())
      {
         Attrib* attrib;

         if (compData->getAttributesRef()->Lookup(hafDeviceClassKW, attrib))
         {
            deviceClass = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_PARTNUMBER, 0), attrib))
         {
            partNumber = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(hafPartNumberKW, attrib))
         {
            partNumber = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_PACKAGELINK, 0), attrib))
         {
            package = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(hafPackageKW, attrib))
         {
            package = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_VALUE, 0), attrib))
         {
            value = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(hafValueKW, attrib))
         {
            value = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TOLERANCE, 0), attrib))
         {
            pTolString.Format("%.2lf", attrib->getDoubleValue());
            nTolString.Format("%.2lf", attrib->getDoubleValue());
         }

         if (compData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_PLUSTOLERANCE, 0), attrib))
         {
            pTolString.Format("%.2lf", attrib->getDoubleValue());
         }

         if (compData->getAttributesRef()->Lookup(hafPTolKW, attrib))
         {
            pTolString.Format("%.2lf", attrib->getDoubleValue());
         }

         if (compData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_MINUSTOLERANCE, 0), attrib))
         {
            nTolString.Format("%.2lf", attrib->getDoubleValue());
         }

         if (compData->getAttributesRef()->Lookup(hafNTolKW, attrib))
         {
            nTolString.Format("%.2lf", attrib->getDoubleValue());
         }

         if (compData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_COMPHEIGHT, 0), attrib))
         {
            compHeightString.Format("%.*lf", decimals, attrib->getDoubleValue());
         }

         if (compData->getAttributesRef()->Lookup(hafCompHeightKW, attrib))
         {
            compHeightString.Format("%.*lf", decimals, attrib->getDoubleValue());
         }

         if (compData->getAttributesRef()->Lookup(hafTestKW, attrib))
         {
            test = attrib->getStringValue();
         }

         if (compData->getAttributesRef()->Lookup(hafBoundaryScanKW, attrib))
         {
            if (!attrib->getStringValue().CompareNoCase("TRUE"))
               boundaryScan = "TRUE";
         }

         if (compData->getAttributesRef()->Lookup(hafCommentKW, attrib))
         {
            comment = attrib->getStringValue();
         }
      }

      double x = compData->getInsert()->getOriginX();
      double y = compData->getInsert()->getOriginY();
      DataStruct *centroidData = centroid_exist_in_block(compGeom);

      if (centroidData)
      {
         DTransform xform(compData->getInsert()->getOriginX(), compData->getInsert()->getOriginY(), 1, compData->getInsert()->getAngle(), compData->getInsert()->getMirrorFlags());
         Point2 p;
         p.x = centroidData->getInsert()->getOriginX();
         p.y = centroidData->getInsert()->getOriginY();

         xform.TransformPoint(&p);

         x = p.x;
         y = p.y;
      }  

      double xSize, ySize;
      GetSize(compGeom, GR_CLASS_PACKAGEOUTLINE, &xSize, &ySize);

      fprintf(fp, "\"COMP\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%.*lf\",\"%.*lf\",\"%.2lf\",\"%s\",\"%s\",\"%s\",,,,,,,,,,\"%.*lf\",\"%.*lf\",,,,\"%s\"\n",
            CleanString(boardGeom->getName()),
            (compData->getInsert()->getRefname()?CleanString(compData->getInsert()->getRefname()):""),
            CleanString(deviceClass),
            CleanString(compGeom->getName()),
            CleanString(partNumber),
            boundaryScan,
            CleanString(package),
            CleanString(value),
            pTolString, 
            nTolString,
            pinCount,
            decimals, x,
            decimals, y,
            RadToDeg(compData->getInsert()->getAngle()),
            (compData->getInsert()->getPlacedBottom()?"Bottom":"Top"),
            compHeightString,
            test,
            decimals, xSize,
            decimals, ySize,
            CleanString(comment));

      doc->SetAttrib(&compData->getAttributesRef(), hafDeviceClassKW, VT_STRING, deviceClass.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&compData->getAttributesRef(), hafPartNumberKW, VT_STRING, partNumber.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&compData->getAttributesRef(), hafPackageKW, VT_STRING, package.GetBuffer(0), SA_OVERWRITE, NULL);
      doc->SetAttrib(&compData->getAttributesRef(), hafValueKW, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);

      if (!pTolString.IsEmpty())
      {
         double pTol = atof(pTolString);
         doc->SetAttrib(&compData->getAttributesRef(), hafPTolKW, VT_DOUBLE, &pTol, SA_OVERWRITE, NULL);
      }

      if (!nTolString.IsEmpty())
      {
         double nTol = atof(nTolString);
         doc->SetAttrib(&compData->getAttributesRef(), hafNTolKW, VT_DOUBLE, &nTol, SA_OVERWRITE, NULL);
      }

      if (!compHeightString.IsEmpty())
      {
         double compHeight = atof(compHeightString);
         doc->SetAttrib(&compData->getAttributesRef(), hafCompHeightKW, VT_UNIT_DOUBLE, &compHeight, SA_OVERWRITE, NULL);
      }

      doc->SetAttrib(&compData->getAttributesRef(), hafTestKW, VT_STRING, test.GetBuffer(0), SA_OVERWRITE, NULL);

      // Write Pins
      pinPos = compGeom->getDataList().GetHeadPosition();
      while (pinPos)
      {
         DataStruct *pinData = compGeom->getDataList().GetNext(pinPos);

         if (pinData->getDataType() != T_INSERT)
            continue;
         
         if (pinData->getInsert()->getInsertType() != INSERTTYPE_PIN)
            continue;

         BlockStruct *pinGeom = doc->getBlockAt(pinData->getInsert()->getBlockNumber());
         doc->validateBlockExtents(pinGeom);

         int pinNumber = 0;
         CString pinDirection;
         CString pinFunction;
         CString comment;

         if (pinData->getAttributesRef())
         {
            Attrib* attrib;

            if (pinData->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_COMPPINNR, 0), attrib))
            {
               pinNumber = attrib->getIntValue();
            }

            if (pinData->getAttributesRef()->Lookup(hafPinDirectionKW, attrib))
            {
               pinDirection = attrib->getStringValue();
            }

            if (pinData->getAttributesRef()->Lookup(hafPinFunctionKW, attrib))
            {
               pinFunction = attrib->getStringValue();
            }

            if (pinData->getAttributesRef()->Lookup(hafCommentKW, attrib))
            {
               comment = attrib->getStringValue();
            }
         }

         CString netName;
         double padstackXSize = 0, padstackYSize = 0;
         CString side, technology;
         CString access;
         int padShape = T_UNDEFINED;
         double drillSize = 0;
         double accessX = 0, accessY = 0;
         CString netPotential;

			CString myTmp = compData->getInsert()->getRefname();

         if (compData->getInsert()->getRefname() && pinData->getInsert()->getRefname())
         {
            NetStruct *net = NULL;
            CompPinStruct *compPin = FindCompPin_ByName(doc, compData->getInsert()->getRefname(),
               pinData->getInsert()->getRefname(), NULL, &net);
   
            if (net)
            {
               Attrib* attrib;

               if (net->getAttributesRef() && net->getAttributesRef()->Lookup(hafNetPotentialKW, attrib))
               {
                  netPotential = attrib->getStringValue();
               }
            }

            if (compPin)
            {
               netName = net->getNetName();

					// Case 1383, use access pt location if there is one, otherwise use pin location
					DataStruct *accessPt = accessPointMap->getMatchingAccessPoint(compPin->getEntityNumber());

               if (accessPt != NULL)
					{
						// Use access pt location
						accessX = accessPt->getInsert()->getOriginX();
						accessY = accessPt->getInsert()->getOriginY();
					}
					else
					{
						// No access point, use pin location
						accessX = compPin->getOriginX();
						accessY = compPin->getOriginY();
					}

               if (compPin->getAttributesRef())
               {
                  Attrib* attrib;

                  if (compPin->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TESTACCESS, 0), attrib))
                  {
                     access = attrib->getStringValue();
                  }

                  if (compPin->getAttributesRef()->Lookup(hafPinDirectionKW, attrib))
                  {
                     pinDirection = attrib->getStringValue();
                  }

                  if (compPin->getAttributesRef()->Lookup(hafPinFunctionKW, attrib))
                  {
                     pinFunction = attrib->getStringValue();
                  }

                  if (compPin->getAttributesRef()->Lookup(hafCommentKW, attrib))
                  {
                     comment = attrib->getStringValue();
                  }
               }

               BOOL TopPad = FALSE;
               BOOL BottomPad = FALSE;
               BlockStruct *padstackGeom = doc->getBlockAt(compPin->getPadstackBlockNumber());
               if (padstackGeom != NULL)
               {
                  doc->validateBlockExtents(padstackGeom);

                  padstackXSize = padstackGeom->getXmax() - padstackGeom->getXmin();
                  padstackYSize = padstackGeom->getYmax() - padstackGeom->getYmin();


                  POSITION padPos = padstackGeom->getDataList().GetHeadPosition();
                  while (padPos)
                  {
                     DataStruct *padData = padstackGeom->getDataList().GetNext(padPos);

                     if (padData->getDataType() != T_INSERT)
                        continue;

                     if (padData->getLayerIndex() == -1)
                        continue;

                     BlockStruct *padGeom = doc->getBlockAt(padData->getInsert()->getBlockNumber());
                     LayerStruct *layer = doc->getLayerArray()[padData->getLayerIndex()];

                     int mirror = compPin->getMirror();

                     if (mirror && !layer->getNeverMirror() && !layer->getMirrorOnly())
                     {
                        if (layer->getLayerType() == LAYTYPE_PAD_ALL)
                        {
                           TopPad = TRUE;
                           BottomPad = TRUE;
                           padShape = padGeom->getShape();
                        }
                        else if (layer->getLayerType() == LAYTYPE_PAD_TOP || layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
                        {
                           BottomPad = TRUE;
                           padShape = padGeom->getShape();
                        }
                        else if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM || layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
                        {
                           TopPad = TRUE;
                           if (!padShape)
                              padShape = padGeom->getShape();
                        }
                     }
                     else if ((!mirror && !layer->getMirrorOnly()) || (mirror && layer->getMirrorOnly()))
                     {
                        if (layer->getLayerType() == LAYTYPE_PAD_ALL)
                        {
                           TopPad = TRUE;
                           BottomPad = TRUE;
                           padShape = padGeom->getShape();
                        }
                        else if (layer->getLayerType() == LAYTYPE_PAD_TOP || layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
                        {
                           TopPad = TRUE;
                           padShape = padGeom->getShape();
                        }
                        else if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM || layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
                        {
                           BottomPad = TRUE;
                           if (!padShape)
                              padShape = padGeom->getShape();
                        }
                     }

                     if (layer->getLayerType() == LAYTYPE_DRILL)
                        drillSize = padGeom->getToolSize();
                  }

               }

               if (TopPad && BottomPad)
               {
                  side = "BOTH";
                  technology = "THRU";
               }
               else if (TopPad)
               {
                  side = "TOP";
                  technology = "SMD";
               }
               else if (BottomPad)
               {
                  side = "BOTTOM";
                  technology = "SMD";
               }
               else
               {
                  side = "NONE";
                  technology.Empty();
               }
            }
         }


         fprintf(fp, "\"PIN\",\"%s\",\"%s\",,\"%s\",,,,,,,,,,\"%.2lf\",\"%s\",,,\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%s\",\"%s\",\"%.*lf\",\"%s\"\n",
               CleanString(boardGeom->getName()),
               (compData->getInsert()->getRefname()?CleanString(compData->getInsert()->getRefname()):""),
               CleanString(pinGeom->getName()),
               RadToDeg(pinData->getInsert()->getAngle()),
               side,
               pinNumber,
               (pinData->getInsert()->getRefname()?CleanString(pinData->getInsert()->getRefname()):""),
               access,
               CleanString(netName),
               CleanString(netPotential),
               CleanString(pinFunction),
               CleanString(pinDirection),
               decimals, accessX,
               decimals, accessY,
               decimals, padstackXSize, 
               decimals, padstackYSize,
               shapes[padShape],
               technology,
               decimals, drillSize,
               CleanString(comment));
      }
   }
}

/******************************************************************************
* WriteVias
*/
void WriteVias(FileStruct *pcbFile, BlockStruct *boardGeom)
{
   int hafRefNameKW = doc->RegisterKeyWord(ATT_HAF_REFNAME, 0, VT_STRING);
   int hafNetPotentialKW = doc->RegisterKeyWord(ATT_HAF_NETPOTENTIAL, 0, VT_STRING);
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);

   POSITION dataPos = boardGeom->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = boardGeom->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_VIA &&
         data->getInsert()->getInsertType() != insertTypeBondPad)
         continue;

      BlockStruct *viaGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());
      
      //get padstack geometry in bondpad
      if(data->isInsertType(insertTypeBondPad))
         viaGeom = GetBondPadPadstackBlock(doc->getCamCadData(), viaGeom);

      doc->validateBlockExtents(viaGeom);

      CString refDes;
      if (data->getInsert()->getRefname())
         refDes = data->getInsert()->getRefname();
      else
         refDes.Empty();

      CString netname;
      CString access;

      double accessX = data->getInsert()->getOriginX();
		double accessY = data->getInsert()->getOriginY();
		// Case 1383, use access pt location if there is one, otherwise use pin location
		DataStruct *accessPt = accessPointMap->getMatchingAccessPoint(data->getEntityNumber());

		if (accessPt != NULL)
		{
			// Use access pt location
			accessX = accessPt->getInsert()->getOriginX();
			accessY = accessPt->getInsert()->getOriginY();
		}

      CString netPotential;
      CString comment;

      if (data->getAttributesRef())
      {
         Attrib* attrib;

         if (data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_NETNAME, 0), attrib))
         {
            netname = attrib->getStringValue();
         }

         if (data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TESTACCESS, 0), attrib))
         {
            access = attrib->getStringValue();
         }

         if (data->getAttributesRef()->Lookup(hafRefNameKW, attrib))
         {
            refDes = attrib->getStringValue();
         }

         if (data->getAttributesRef()->Lookup(hafCommentKW, attrib))
         {
            comment = attrib->getStringValue();
         }
      }

      if (!netname.IsEmpty())
      {
         NetStruct *net = FindNet(pcbFile, netname);

         Attrib* attrib;

         if (net && net->getAttributesRef() && net->getAttributesRef()->Lookup(hafNetPotentialKW, attrib))
         {
            netPotential = attrib->getStringValue();
         }
      }

      BOOL TopPad = FALSE, BottomPad = FALSE;
      int padShape = T_UNDEFINED;
      double drillSize = 0;
      POSITION padPos = viaGeom->getDataList().GetHeadPosition();
      while (padPos)
      {
         DataStruct *padData = viaGeom->getDataList().GetNext(padPos);

         if (padData->getDataType() != T_INSERT)
            continue;

         if (padData->getLayerIndex() == -1)
            continue;

         BlockStruct *padGeom = doc->getBlockAt(padData->getInsert()->getBlockNumber());

         LayerStruct *layer = doc->getLayerArray()[padData->getLayerIndex()];
         if(doc->IsFloatingLayer(padData->getLayerIndex()) && data->getLayerIndex() != -1)
         {
            layer = doc->getLayerArray()[data->getLayerIndex()];
         }

         switch (layer->getLayerType())
         {
         case LAYTYPE_PAD_ALL:
            TopPad = TRUE;
            BottomPad = TRUE;
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_PAD_TOP:
            TopPad = TRUE;
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_PAD_BOTTOM:
            BottomPad = TRUE;
            if (!padShape)
               padShape = padGeom->getShape();
            break;
         case LAYTYPE_SIGNAL_TOP:
            TopPad = TRUE;
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_SIGNAL_BOT:
            BottomPad = TRUE;
            if (!padShape)
               padShape = padGeom->getShape();
            break;
         case LAYTYPE_DRILL:
            drillSize = padGeom->getToolSize();
            break;
         }
      }

      CString side, technology;

      if (TopPad)
      {
         if (BottomPad)
         {
            side = "BOTH";
            technology = "THRU";
         }
         else
         {
            side = "TOP";
            technology = "BLIND";
         }
      }
      else
      {
         if (BottomPad)
         {
            side = "BOTTOM";
            technology = "BLIND";
         }
         else
         {
            side = "NONE";
            technology = "BURIED";
         }
      }

      fprintf(fp, "\"VIA\",\"%s\",\"%s\",,\"%s\",,,,,,,,,,,\"%s\",,,,,\"%s\",\"%s\",\"%s\",,,\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%s\",\"%s\",\"%.*lf\",\"%s\"\n",
            CleanString(boardGeom->getName()), 
            CleanString(refDes),
            CleanString(viaGeom->getName()),
            side,
            access,
            CleanString(netname),
            CleanString(netPotential),
            decimals, accessX,
            decimals, accessY,
            decimals, viaGeom->getXmax() - viaGeom->getXmin(),
            decimals, viaGeom->getYmax() - viaGeom->getYmin(),
            shapes[padShape],
            technology,
            decimals, drillSize,
            CleanString(comment));

      doc->SetAttrib(&data->getAttributesRef(), hafRefNameKW, VT_STRING, refDes.GetBuffer(0), SA_OVERWRITE, NULL);
   }
}

/******************************************************************************
* WriteCompPinTestpoints
*/
void WriteCompPinTestpoints(CNetList *netList, BlockStruct *boardGeom, CFormatStdioFile &logFile)
{
   int hafNetPotentialKW = doc->RegisterKeyWord(ATT_HAF_NETPOTENTIAL, 0, VT_STRING);
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);

   POSITION netPos = netList->GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = netList->GetNext(netPos);

      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(cpPos);

         if (!compPin->getAttributesRef())
            continue;

         Attrib* attrib;

         if (!compPin->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TEST, 0), attrib))
            continue;

         CString access;

         if (compPin->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TESTACCESS, 0), attrib))
         {
            access = attrib->getStringValue();
         }

         double accessX = compPin->getOriginX();
         double accessY = compPin->getOriginY();
			// Case 1383, use access pt location if there is one, otherwise use pin location
			DataStruct *accessPt = accessPointMap->getMatchingAccessPoint(compPin->getEntityNumber());

			if (accessPt != NULL)
			{
				// Use access pt location
				accessX = accessPt->getInsert()->getOriginX();
				accessY = accessPt->getInsert()->getOriginY();
			}	

         int pinNumber = 0;

         if (atoi(compPin->getPinName()))
            pinNumber = atoi(compPin->getPinName());

         if (compPin->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_COMPPINNR, 0), attrib))
         {
            pinNumber = attrib->getIntValue();
         }

         CString netPotential;

         if (net->getAttributesRef() && net->getAttributesRef()->Lookup(hafNetPotentialKW, attrib))
         {
            netPotential = attrib->getStringValue();
         }

         CString comment;

         if (compPin->getAttributesRef()->Lookup(hafCommentKW, attrib))
         {
            comment = attrib->getStringValue();
         }

         BlockStruct *padstackGeom = doc->getBlockAt(compPin->getPadstackBlockNumber());
         doc->validateBlockExtents(padstackGeom);

         double padstackXSize = padstackGeom->getXmax() - padstackGeom->getXmin();
         double padstackYSize = padstackGeom->getYmax() - padstackGeom->getYmin();

         BOOL TopPad = FALSE, BottomPad = FALSE;
         int padShape = T_UNDEFINED;
         CString side, technology;
         double drillSize = 0;

         POSITION padPos = padstackGeom->getDataList().GetHeadPosition();
         while (padPos)
         {
            DataStruct *padData = padstackGeom->getDataList().GetNext(padPos);

            if (padData->getDataType() != T_INSERT)
               continue;

            if (padData->getLayerIndex() == -1)
               continue;

            BlockStruct *padGeom = doc->getBlockAt(padData->getInsert()->getBlockNumber());

            LayerStruct *layer = doc->getLayerArray()[padData->getLayerIndex()];

            switch (layer->getLayerType())
            {
            case LAYTYPE_PAD_ALL:
               TopPad = TRUE;
               BottomPad = TRUE;
               padShape = padGeom->getShape();
               break;
            case LAYTYPE_PAD_TOP:
               TopPad = TRUE;
               padShape = padGeom->getShape();
               break;
            case LAYTYPE_PAD_BOTTOM:
               BottomPad = TRUE;
               if (!padShape)
                  padShape = padGeom->getShape();
               break;
            case LAYTYPE_SIGNAL_TOP:
               TopPad = TRUE;
               padShape = padGeom->getShape();
               break;
            case LAYTYPE_SIGNAL_BOT:
               BottomPad = TRUE;
               if (!padShape)
                  padShape = padGeom->getShape();
               break;
            case LAYTYPE_DRILL:
               drillSize = padGeom->getToolSize();
               break;
            }
         }

         if (TopPad)
         {
            if (BottomPad)
            {
               side = "BOTH";
               technology = "THRU";
            }
            else
            {
               side = "TOP";
               technology = "SMD";
            }
         }
         else
         {
            if (BottomPad)
            {
               side = "BOTTOM";
               technology = "SMD";
            }
            else
            {
               side = "NONE";
               technology.Empty();
            }
         }
         //Skip Die Pin
         if(!compPin->IsDiePin(doc->getCamCadData()))
         {
            fprintf(fp, "\"TESTPOINT\",\"%s\",\"%s\",,\"%s\",,,,,,,,,,,\"%s\",,,\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"Pin Function\",\"Pin Direction\",\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%s\",\"%s\",\"%.*lf\",\"%s\"\n",
                  CleanString(boardGeom->getName()),
                  CleanString(compPin->getRefDes()),
                  CleanString(padstackGeom->getName()),
                  side,
                  pinNumber,
                  CleanString(compPin->getPinName()),
                  access,
                  CleanString(net->getNetName()),
                  CleanString(netPotential),
                  decimals, accessX,
                  decimals, accessY,
                  decimals, padstackXSize, 
                  decimals, padstackYSize,
                  shapes[padShape],
                  technology,
                  decimals, drillSize,
                  CleanString(comment));
         }
         else
         {
            logFile.WriteString("TESTPOINT - Net: %s CompPin %s - Skipped CompPin for Die component.\n", CleanString(net->getNetName()),
               CleanString(compPin->getPinRef()));
         }

      }
   }
}

/******************************************************************************
* WriteFeatureTestpoints
*/
void WriteFeatureTestpoints(FileStruct *pcbFile, BlockStruct *boardGeom)
{
   int hafRefNameKW = doc->RegisterKeyWord(ATT_HAF_REFNAME, 0, VT_STRING);
   int hafNetPotentialKW = doc->RegisterKeyWord(ATT_HAF_NETPOTENTIAL, 0, VT_STRING);
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);

   POSITION dataPos = boardGeom->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = boardGeom->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && data->getInsert()->getInsertType() != INSERTTYPE_VIA)
         continue;

      if (!data->getAttributesRef())
         continue;

      Attrib* attrib;

      if (!data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TEST, 0), attrib))
         continue;

      CString refDes;

      if (data->getInsert()->getRefname())
         refDes = data->getInsert()->getRefname();
      else
         refDes.Empty();

      if (data->getAttributesRef()->Lookup(hafRefNameKW, attrib))
      {
         refDes = attrib->getStringValue();
      }

      CString netname;
      CString netPotential;

      if (data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_NETNAME, 0), attrib))
      {
         netname = attrib->getStringValue();

         NetStruct *net = FindNet(pcbFile, netname);

         if (net && net->getAttributesRef() && net->getAttributesRef()->Lookup(hafNetPotentialKW, attrib))
         {
            netPotential = attrib->getStringValue();
         }
      }

      CString access;

      if (data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TESTACCESS, 0), attrib))
      {
         access = attrib->getStringValue();
      }

      double accessX = data->getInsert()->getOriginX();
      double accessY = data->getInsert()->getOriginY();
		// Case 1383, use access pt location if there is one, otherwise use pin location
		DataStruct *accessPt = accessPointMap->getMatchingAccessPoint(data->getEntityNumber());

		if (accessPt != NULL)
		{
			// Use access pt location
			accessX = accessPt->getInsert()->getOriginX();
			accessY = accessPt->getInsert()->getOriginY();
		}		

      CString comment;

      if (data->getAttributesRef()->Lookup(hafCommentKW, attrib))
      {
         comment = attrib->getStringValue();
      }

      BlockStruct *padstackGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());
      BlockStruct *compGeom = NULL;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
      {
         compGeom = padstackGeom;
         POSITION pinPos = compGeom->getDataList().GetHeadPosition();
         while (pinPos)
         {
            DataStruct *pinInsert = compGeom->getDataList().GetNext(pinPos);

            if (pinInsert->getDataType() != T_INSERT)
               continue;

            if (pinInsert->getInsert()->getInsertType() != INSERTTYPE_PIN)
               continue;

            padstackGeom = doc->getBlockAt(pinInsert->getInsert()->getBlockNumber());
            break;
         }
      }

      doc->validateBlockExtents(padstackGeom);

      BOOL TopPad = FALSE, BottomPad = FALSE;
      int padShape = T_UNDEFINED;
      double drillSize = 0;
      POSITION padPos = padstackGeom->getDataList().GetHeadPosition();
      while (padPos)
      {
         DataStruct *padData = padstackGeom->getDataList().GetNext(padPos);

         if (padData->getDataType() != T_INSERT)
            continue;

         if (padData->getLayerIndex() == -1)
            continue;

         BlockStruct *padGeom = doc->getBlockAt(padData->getInsert()->getBlockNumber());

         LayerStruct *layer = doc->getLayerArray()[padData->getLayerIndex()];
         switch (layer->getLayerType())
         {
         case LAYTYPE_PAD_ALL:
            TopPad = TRUE;
            BottomPad = TRUE;
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_PAD_TOP:
            TopPad = TRUE;
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_PAD_BOTTOM:
            BottomPad = TRUE;
            if (!padShape)
               padShape = padGeom->getShape();
            break;
         case LAYTYPE_SIGNAL_TOP:
            TopPad = TRUE;
            padShape = padGeom->getShape();
            break;
         case LAYTYPE_SIGNAL_BOT:
            BottomPad = TRUE;
            if (!padShape)
               padShape = padGeom->getShape();
            break;
         case LAYTYPE_DRILL:
            drillSize = padGeom->getToolSize();
            break;
         }
      }

      CString side, technology;
      if (TopPad)
      {
         if (BottomPad)
         {
            side = "BOTH";
            technology = "THRU";
         }
         else
         {
            side = "TOP";
            technology = "BLIND";
         }
      }
      else
      {
         if (BottomPad)
         {
            side = "BOTTOM";
            technology = "BLIND";
         }
         else
         {
            side = "NONE";
            technology = "BURIED";
         }
      }

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
      {
         POSITION pinPos = compGeom->getDataList().GetHeadPosition();
         while (pinPos)
         {
            DataStruct *pinInsert = compGeom->getDataList().GetNext(pinPos);

            if (pinInsert->getDataType() != T_INSERT)
               continue;

            if (pinInsert->getInsert()->getInsertType() != INSERTTYPE_PIN)
               continue;

            int pinNumber = 0;

            if (pinInsert->getInsert()->getRefname())
               pinNumber = atoi(pinInsert->getInsert()->getRefname());

            if (pinInsert->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_COMPPINNR, 0), attrib))
            {
               pinNumber = attrib->getIntValue();
            }

            CString pinName;

            if (pinInsert->getInsert()->getRefname())
               pinName = pinInsert->getInsert()->getRefname();

            NetStruct *net = NULL;
            FindCompPin_ByName(doc, refDes, pinName, NULL, &net);

            if (net && net->getAttributesRef() && net->getAttributesRef()->Lookup(hafNetPotentialKW, attrib))
            {
               netPotential = attrib->getStringValue();
            }

            fprintf(fp, "\"TESTPOINT\",\"%s\",\"%s\",,\"%s\",,,,,,,,,,,\"%s\",,,\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",,,\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%s\",\"%s\",\"%.*lf\",\"%s\"\n",
                  CleanString(boardGeom->getName()),
                  CleanString(refDes),
                  CleanString(compGeom->getName()),
                  side,
                  pinNumber,
                  CleanString(pinName),
                  access,
                  CleanString(net->getNetName()),
                  CleanString(netPotential),
                  decimals, accessX,
                  decimals, accessY,
                  decimals, padstackGeom->getXmax() - padstackGeom->getXmin(),
                  decimals, padstackGeom->getYmax() - padstackGeom->getYmin(),
                  shapes[padShape],
                  technology,
                  decimals, drillSize,
                  CleanString(comment));
         }
      }
      else
      {
         fprintf(fp, "\"TESTPOINT\",\"%s\",\"%s\",,\"%s\",,,,,,,,,,,\"%s\",,,,,\"%s\",\"%s\",\"%s\",,,\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%s\",\"%s\",\"%.*lf\",\"%s\"\n",
               CleanString(boardGeom->getName()), 
               CleanString(refDes),
               CleanString(padstackGeom->getName()),
               side,
               access,
               CleanString(netname),
               CleanString(netPotential),
               decimals, accessX,
               decimals, accessY,
               decimals, padstackGeom->getXmax() - padstackGeom->getXmin(),
               decimals, padstackGeom->getYmax() - padstackGeom->getYmin(),
               shapes[padShape],
               technology,
               decimals, drillSize,
               CleanString(comment));
      }

      doc->SetAttrib(&data->getAttributesRef(), hafRefNameKW, VT_STRING, refDes.GetBuffer(0), SA_OVERWRITE, NULL);
   }
}

/******************************************************************************
* WriteProbes
*/
void WriteProbes(FileStruct *pcbFile, BlockStruct *boardGeom, CFormatStdioFile &logFile)
{
   int hafNetPotentialKW = doc->RegisterKeyWord(ATT_HAF_NETPOTENTIAL, 0, VT_STRING);
   int hafCommentKW = doc->RegisterKeyWord(ATT_HAF_COMMENT, 0, VT_STRING);

   POSITION dataPos = boardGeom->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = boardGeom->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)
         continue;

      BlockStruct *probeGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());
      doc->validateBlockExtents(probeGeom);

      CString refDes;
      if (data->getInsert()->getRefname())
         refDes = data->getInsert()->getRefname();
      else
         refDes.Empty();

      CString netname;
      CString access;
      double accessX = data->getInsert()->getOriginX(), accessY = data->getInsert()->getOriginY();
      CString netPotential;
      CString comment;

      if (data->getAttributesRef())
      {
         Attrib* attrib;

         if (data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_NETNAME, 0), attrib))
         {
            netname = attrib->getStringValue();

            NetStruct *net = FindNet(pcbFile, netname);

            if (net && net->getAttributesRef() && net->getAttributesRef()->Lookup(hafNetPotentialKW, attrib))
            {
               netPotential = attrib->getStringValue();
            }
         }

         if (data->getAttributesRef()->Lookup(doc->IsKeyWord(ATT_TESTACCESS, 0), attrib))
         {
            access = attrib->getStringValue();
         }

         if (data->getAttributesRef()->Lookup(hafCommentKW, attrib))
         {
            comment = attrib->getStringValue();
         }
      }

      double drillSize = 0;
      POSITION padPos = probeGeom->getDataList().GetHeadPosition();
      while (padPos)
      {
         DataStruct *padData = probeGeom->getDataList().GetNext(padPos);

         if (padData->getDataType() != T_INSERT)
            continue;

         if (padData->getLayerIndex() == -1)
            continue;

         BlockStruct *padGeom = doc->getBlockAt(padData->getInsert()->getBlockNumber());

         LayerStruct *layer = doc->getLayerArray()[padData->getLayerIndex()];
         if (layer->getLayerType() == LAYTYPE_DRILL)
            drillSize = padGeom->getToolSize();
      }

      //skip probes on the Die pins
      if(!IsProbedDiePin(doc->getCamCadData(), data))
      {
         fprintf(fp, "\"PROBE\",\"%s\",\"%s\",,\"%s\",,,,,,,,,,,\"%s\",,,,,,\"%s\",\"%s\",,,\"%.*lf\",\"%.*lf\",\"%.*lf\",\"%.*lf\",,,\"%.*lf\",\"%s\"\n",
               CleanString(boardGeom->getName()), 
               CleanString(refDes),
               CleanString(probeGeom->getName()),
               (data->getInsert()->getPlacedBottom()?"Bottom":"Top"),
               CleanString(netname),
               CleanString(netPotential),
               decimals, accessX,
               decimals, accessY,
               decimals, probeGeom->getXmax() - probeGeom->getXmin(),
               decimals, probeGeom->getYmax() - probeGeom->getYmin(),
               decimals, drillSize,
               CleanString(comment));
      }
      else
      {
         logFile.WriteString("PROBE - Net: %s Probe %s - Skipped Probe for Die pin.\n",
            CleanString(netname), refDes);
      }

   }
}

/******************************************************************************
* ExplodePcbNcPins
*  - explode single pin nets out of unused nets for files that are in the panel file
*/
static void ExplodePcbNcPins()
{
   POSITION dataPos = panelFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = panelFile->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCB)
      {
         POSITION filePos = doc->getFileList().GetHeadPosition();
         while (filePos)
         {
            FileStruct *file = doc->getFileList().GetNext(filePos);

            if (file->getBlock()->getBlockNumber() == data->getInsert()->getBlockNumber())
               ExplodeNcPins(doc, file);
         }
      }
   }
}

/******************************************************************************
* LoadHuntronSettings
*/
void LoadHuntronSettings()
{
   prefixMap.RemoveAll();
	isHuntronStyle = TRUE;

   CString settingsFile( getApp().getExportSettingsFilePath("haf.out") );
   FILE *fSettings = fopen(settingsFile, "rt");
   if (!fSettings)   
   {
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      MessageBox(NULL, tmp, "HAF Settings File", MB_OK | MB_ICONHAND);
      return;
   }

   char line[255];
   char *lp;

   while (fgets(line, 255, fSettings))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == ';')
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".CompPrefType"))
         {
            CString prefix;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            prefix = _strupr(lp);
            prefix.TrimLeft();
            prefix.TrimRight();

            CString deviceClass;
            if ((lp = get_string(NULL," \t;\n")) == NULL) continue; 
            deviceClass = _strupr(lp);
            deviceClass.TrimLeft();
            deviceClass.TrimRight();

            prefixMap.SetAt(prefix, deviceClass);
         }
			else if (!STRICMP(lp, ".EXPORT_STYLE"))
			{
				if ((lp = get_string(NULL, " \t\n")) != NULL) 
				{
					// By default is it always Huntron style
					// H = Huntron
					// G = Geopel
					if (toupper(lp[0]) == 'G')
						isHuntronStyle = FALSE;
				}
			}
      }
   }

   fclose(fSettings);
}


/******************************************************************************
* GetSize
*/
void GetSize(BlockStruct *block, int graphicClass, double *xSize, double *ySize)
{
   *xSize = *ySize = 0;

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

   Mat2x2 m;
   RotMat2(&m, 0);

   // find panel outline
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // found Panel Outline
      if (data->getGraphicClass() == graphicClass && data->getDataType() == T_POLY)
      {
         ExtentRect extents;
         PolyExtents(doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, FALSE);

         // get extents
         if (extents.left < xmin) xmin = extents.left;
         if (extents.right > xmax) xmax = extents.right;
         if (extents.bottom < ymin) ymin = extents.bottom;
         if (extents.top > ymax) ymax = extents.top;
      }
   }

   if (xmax > xmin)
   {
      *xSize = xmax - xmin;
      *ySize = ymax - ymin;
   }
}


/******************************************************************************
* GenerateRefNames
*/
void GenerateRefNames(CCEtoODBDoc *doc)
{
   int hafRefNameKW = doc->RegisterKeyWord(ATT_HAF_REFNAME, 0, VT_STRING);

   int maxVia = 0;
   int maxFiducial = 0;
   int maxHole = 0;
   int maxTestPoint = 0;
	int i=0;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         switch (data->getInsert()->getInsertType())
         {
         case INSERTTYPE_VIA:
         case INSERTTYPE_FIDUCIAL:
         case INSERTTYPE_TOOLING:
         case INSERTTYPE_TESTPOINT:
            break;
         default:
            continue;
         }

         if (data->getInsert()->getRefname())
         {
            CString refName = data->getInsert()->getRefname();

            int num;
            switch (data->getInsert()->getInsertType())
            {
            case INSERTTYPE_VIA:
               if (refName.Left(4) == "VIA_")
               {
                  num = atoi(refName.Mid(4));
                  if (num > maxVia)
                     maxVia = num;
               }
               break;
            case INSERTTYPE_FIDUCIAL:
               if (refName.Left(4) == "FID_")
               {
                  num = atoi(refName.Mid(4));
                  if (num > maxFiducial)
                     maxFiducial = num;
               }
               break;
            case INSERTTYPE_TOOLING:
               if (refName.Left(5) == "HOLE_")
               {
                  num = atoi(refName.Mid(4));
                  if (num > maxHole)
                     maxHole = num;
               }
               break;
            case INSERTTYPE_TESTPOINT:
               if (refName.Left(3) == "TP_")
               {
                  num = atoi(refName.Mid(4));
                  if (num > maxTestPoint)
                     maxTestPoint = num;
               }
               break;
            default:
               continue;
            }

         }

         if (!data->getAttributesRef())
            continue;

         Attrib* attrib;

         if (!data->getAttributesRef()->Lookup(hafRefNameKW, attrib))
            continue;

         CString refName = attrib->getStringValue();

         int num;

         switch (data->getInsert()->getInsertType())
         {
         case INSERTTYPE_VIA:
            if (refName.GetLength() < 4)
               break;

            num = atoi(refName.Mid(4));

            if (num > maxVia)
               maxVia = num;

            break;
         case INSERTTYPE_FIDUCIAL:
            if (refName.GetLength() < 4)
               break;

            num = atoi(refName.Mid(4));

            if (num > maxFiducial)
               maxFiducial = num;

            break;
         case INSERTTYPE_TOOLING:
            if (refName.GetLength() < 5)
               break;

            num = atoi(refName.Mid(5));

            if (num > maxHole)
               maxHole = num;

            break;
         case INSERTTYPE_TESTPOINT:
            if (refName.GetLength() < 3)
               break;

            num = atoi(refName.Mid(3));

            if (num > maxTestPoint)
               maxTestPoint = num;

            break;
         }
      }
   }

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
            continue;

         Attrib* attrib;

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(hafRefNameKW, attrib))
         {
            CString refName = attrib->getStringValue();

            if (!refName.IsEmpty())
               continue;
         }

         CString refName;

         switch (data->getInsert()->getInsertType())
         {
         case INSERTTYPE_VIA:
            refName.Format("VIA_%d", ++maxVia);
            break;
         case INSERTTYPE_FIDUCIAL:
            refName.Format("FID_%d", ++maxFiducial);
            break;
         case INSERTTYPE_TOOLING:
            refName.Format("HOLE_%d", ++maxHole);
            break;
         case INSERTTYPE_TESTPOINT:
            refName.Format("TP_%d", ++maxTestPoint);
            break;
         default:
            continue;
         }

         doc->SetAttrib(&data->getAttributesRef(), hafRefNameKW, VT_STRING, refName.GetBuffer(0), SA_OVERWRITE, NULL);
      }
   }
}


/******************************************************************************
* CleanString
*/
CString CleanString(CString string)
{
   string.Replace('\"', '_');

/* string.Replace("\\", "\\\\");
   string.Replace("\"", "\\\"");*/

   return string;
}

//=============================================================================

//_____________________________________________________________________________
CAccessPointMap::CAccessPointMap(BlockStruct *pcbBlock) :
   m_entityToLinkedAccessPointMap(1000,false)
{
	init(pcbBlock);
}

/*
   Test Access Points - insert DataStructs with an insert type of insertTypeTestAccessPoint.
   They are associated with component pins (CompPinStructs), or with vias or test points (DataStructs).
   The association is defined by a data link attribute, ATT_DDLINK, whose value
   is the entity number of a component pin, via, or testpoint.
*/

void CAccessPointMap::init(BlockStruct *pcbBlock)
{

	if (pcbBlock != NULL)
	{
		m_entityToLinkedAccessPointMap.InitHashTable(nextPrime2n(5000));

		WORD dataLinkKey = (WORD)doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);

		for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
		{
			DataStruct* testData = insertIterator.getNext();

			if (testData->getInsert()->getInsertType() == insertTypeTestAccessPoint)
			{
				Attrib* dataLinkAttribute;

				if (testData->getAttributes()->Lookup(dataLinkKey,dataLinkAttribute))
				{
					int linkedEntityNumber = dataLinkAttribute->getIntValue();

					m_entityToLinkedAccessPointMap.SetAt(linkedEntityNumber,testData);
					
				}
			}
		}
	}
}


DataStruct* CAccessPointMap::getMatchingAccessPoint(int entityNumber)
{
   DataStruct* accessPoint = NULL;

   if (!m_entityToLinkedAccessPointMap.Lookup(entityNumber,accessPoint))
   {
      accessPoint = NULL;
   }

   return accessPoint;
}

