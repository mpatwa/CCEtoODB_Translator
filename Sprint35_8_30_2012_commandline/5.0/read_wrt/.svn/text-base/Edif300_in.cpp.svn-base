// $Header: /CAMCAD/4.6/read_wrt/Edif300_in.cpp 154   4/27/07 3:46p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved. 
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <string.h>    
#include "gauge.h"
#include "attrib.h"
#include "lyrmanip.h"
#include "Polylib.h"
#include "Net_Util.h"
#include "API.h"	// Use this to return the error code RC_INVALID_FORMAT
#include "CCEtoODB.h"
#include "CamCadDatabase.h"
#include "File.h"
#include "Sch_Lib.h"
#include "Edif300_in.h"
#include "RwUiLib.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)



extern CProgressDlg *progress;
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);


/* Static Variables *********************************************************/
static CCEtoODBDoc          *sDoc;
static FileStruct          *sFileStruct;        // schematic page file that represent a sheet
static int                 sPageUnit;
                           
static FILE                *sFileIn; 
static long                sFileInLineCnt;      // current line number
static FILE                *sFileErr;
static long                sDisplayErr;         // current number of errors
static BOOL                sHasHierarchy;       // indicate the schematic has hierarchy structure
                           
static char                sToken[MAX_TOKEN];   // current token
static BOOL                sIsTokenName;
static BOOL                sPushToken;
                           
static char                sCurrentLine[MAX_LINE];
static BOOL                sIsNewLine;

// There variables store the layer index for the difference kind of graphics and attributes display
static int                 sTextLayer;
static int                 sPortDesLayer;
static int                 sSymbolDesLayer;
static int                 sPortGrpLayer;
static int                 sSymbolGrpLayer;
static int                 sSignalGrpLayer;
static int                 sBorderGrpLayer;
static int                 sAttribLayer;


// ** NEW Implementation **
static CEdif					edifStruct;

// These variables store data that are use by the entire Edif file and are not change once they are created
static CMapStringToPtr     sUnitMap;
static CMapStringToPtr     sFontMap;
static CEdifPortMap			sGlobalPortMap;
static CMapStringToString  sEdifKeywordMap;     // map of a Edif property name to CAMCAD keyword index
                                                // the function property() is the only place that add to this map

static EdifPhysicalDefault sDefaultSetting;
static CCamCadDatabase*		sCamCadDatabase;

// These variables store data that are temperary and are create either at each library, cell, cluster or view
static CFigureGroupMap     sFigureGroupMap;
static CEdifPortMap			sPortMap;

// These variables are use for temperary storing the current data
static CString             sUnitRef;            // current unit reference, use this when you don't know what unit to use
static CString             sDistanceUnitRef;    // current unit reference for distance measurement
static CString             sAngleUnitRef;       // current unit reference for angle measurement
static CString             sObjectRef;          // current object reference
static CString             sLibraryRef;         // current library reference
static CString             sClusterRef;         // current cluster reference
static CString             sInstanceRef;        // current instance reference
static CString             sSymbolRef;          // current schematic symbol reference
static CString             sSignalRef;          // current signal reference
static CString             sClusterConfigRef;   // current cluster configuration reference
static CString             sViewRef;            // current viewRef;
static CString             sRipperHotspotRef;   // current ripperHotspot reference

static CString             sLibraryName;        // current library name
static CString             sCellName;           // current cell name
static CString             sClusterName;        // current cluster name
static CString					sPageName;				// current page name
static CString             sViewName;           // current view name
static CString             sInterfaceDes;       // current interface designator, apply to all schematic symbol within the cluster
static CString             sPropertyValue;      // current property value 
static int                 sPropertyType;       // current property type:  valueTypeString, VT_INTEGER, VT_DOUBLE
                                                
static BOOL                sRemoveDisplay;      // indicate wheather a property display is removed
static BOOL                sDesignCommand;      // indicate wheather Edif file has a "design" command callout
static EEdifProgram        sEdifProgram;
                                                
static double              sTextHeight;         // current text height
static int                 sJustification;      // current justification
                                                
static EdifFontDef         *sFont;              // current font structure
static EdifUnitDef         *sUnit;              // current unit structure
static CFigureGroup			sFigureGroup;        // current figure group structure
static CStringArray        sPortRefArr;         // current list of port reference
                                                
static BlockStruct         *sCurrentBlock;      // current geometry
static DataStruct          *sCurrentData;       // current insert
static CPolyList           *sPolyList;          // current poly list structure
static CPoly               *sPoly;              // current poly structure     

// These lists are use as stack to push and pop objects
static CStringList         sDesList;            // list of designators 
static CTransformList		sTransformList;      // list of transforms
static CDisplayList			sDisplayList;			// list of displays

static CMapWordToAttributesList	sAttribMapList;      // list of attribute map
static COcurrenceAnnotateList		sOccurrenceAnnotate;	

// These are varible used for setting file
static CMapStringToString  sNotLogicSymbol;              // a map of all the symbols that is type NOT as logic symbols
static BOOL                sOverWriteAttributeDisplay;   // indicate how if attribute display should be overwrite so always use the last display
static CStringArray        sDesignatorBackannotateArray; // name of attribute that will be use as designator
static CStringArray        sPinNumBackannotateArray;     // name of attribute that will be use as pin number
static CString             sNetBackannotate;             // name of attribute that will be use as the net annotated name
static CString             sDesignatorLocBackannotate;   // name of attribute's location that will be use as designator location
static CString             sPinNumLocBackannotate;       // name of attribute's location that will be use as pin number location
static double              sFontScale;                   // number to scale Edif text 
static BOOL                sExchangeXYScale;             // indicate whether to swap scaleX to scaleY and scaleY to scaleX
static double					sMinimumPageSize;					// indicate the minimum size a page must be
static CAttributeVisibilityMap sAttribVisibleMap;

/* Function Prototypes *********************************************************/

static void InitializeMembers();
static void FreeMembers();
static void LoadSettingFile();
static void CreateRequiredLayer();
static void CreateDefaultGeometry();
static void CheckPageSize();
static int LoopCommand(List *list, int size);
static int GoCommand(List *lisst, int size);
static int GetToken();
static int PushToken();
static int TokenSearch(List *list, int size);
static int GetNext(char *inputLine, int isNewLine);
static int GetLine(char *inputLine, int size);
static void convert_special_char(char *t);
static int ParsingErr();
static int GetString(CString &stringValue);
static int GetIntegerToken(int &integerValue);
static int GetScaleInteger(double &scaleValue);
static int GetBoolean(BOOL &boolValue);
static int GetPoint(CPnt *pnt, BOOL convertUnit = TRUE);
static int GetPoints(CPntList &pntList);
static int GetPointList(CPntList &pntList);
static int GetCurve(CPntList &pntList);
static double ConvertUnit(CString unitRef, double numOfNewUnit, void *basicUnitName);
static CString GetBasicUnitName(CString unitRef);
static int SetCurrentFigureGroup(CString figureGroupRef);
static DataStruct *InsertGlobalGeometry(CString blockName, CTransform transform, CString refdes, int insertType);
static DataStruct *InsertSchematicSymbol(CInstance *instance, CTransform transform);
static int CopyANDRemoveAttribMap(CAttributes** copyToMap, CAttributes* fromMap);
static int DoStringAttributeValue(CString keyword, CString value);
static int DoStringAttributeDisplay(CString keyword);
static void CreatePoly();
static void AddPoly();
static BOOL IsNotLogicSymbol(CString libraryName, CString cellName, CString clusterName, CString symbolName);
static CString ParseBusBundleName(const CString busBundleName);
static int GraphTextDisplays(CDisplay* display, int textLayerIndex, CString text);
static int AddAttributeDisplays(Attrib* attrib, CDisplay* display, CString layerName);


static CString GetAttribStringValue(Attrib *attrib)
{
   CString retval;

   if (attrib != NULL)
   {
#if CamCadMajorMinorVersion > 406  //  > 4.6
      retval = attrib->getStringValue();
#else
      retval = sDoc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
   }

   return retval;
}

/******************************************************************************
* ReadEDIF300
*/
int ReadEDIF300(CString fileName, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
	int res = 1;
   Doc->getSettings().Crosshairs = FALSE;
   sDoc = Doc;
   sPageUnit = pageunits;
   sEdifProgram = edifProgramUndefined;

   // Open input file
   sFileIn = fopen(fileName, "rt");
   sFileInLineCnt = 0;
   if (sFileIn == NULL)
   {
      CString msg;
      msg.Format("Error open [%s] file", fileName);
      ErrorMessage(msg, "Error");
      return res;
   }

   // Open error log file
   CString edifLogFile = GetLogfilePath("edif.log");
   sFileErr = fopen(edifLogFile, "wt");
   sDisplayErr = 0;
   if (sFileErr == NULL)
   {
      CString msg;
      msg.Format("Error open [%s] file", edifLogFile);
      ErrorMessage(msg, "Error");
      return res;
   }
   fprintf(sFileErr, "File : %s\n", fileName);

   // Perpare for reader
   InitializeMembers();
   LoadSettingFile();
   CreateRequiredLayer();

   // register this keyword here since it is use many time later
   sDoc->RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString);
   sDoc->RegisterKeyWord(SCH_ATT_DISPLAYNAME, 0, valueTypeString);
   sDoc->RegisterKeyWord(SCH_ATT_PORTNAME, 0, valueTypeString);
   sDoc->RegisterKeyWord(SCH_ATT_LIBRARYNAME, 0, valueTypeString);
   sDoc->RegisterKeyWord(SCH_ATT_CELLNAME, 0, valueTypeString);
   sDoc->RegisterKeyWord(SCH_ATT_CLUSTERNAME, 0, valueTypeString);
   sDoc->RegisterKeyWord("NetDisplayName", 0, valueTypeString);
   sDoc->RegisterKeyWord("PortDirection", 0, valueTypeString);

   // Start reading file
   if (LoopCommand(startEdit_lst, SIZ_STARTEDIT_LST) < 0)
   {
      CString msg;
      msg.Format("File did not imported sucessfully because EDIF syntax error encountered close to line %ld\n\n!!! PLEASE SEE %s for messages!!!", sFileInLineCnt, edifLogFile );
      ErrorMessage(msg, "EDIF Read Error");

		if (sFileStruct->getBlock() == NULL)
		{
#if CamCadMajorMinorVersion > 406  //  > 4.6
         sDoc->getFileList().deleteFile(sFileStruct);
#else
			POSITION filePos = sDoc->getFileList().Find(sFileStruct);
			sDoc->FreeFile(sFileStruct);
			sDoc->getFileList().RemoveAt(filePos);
#endif
			sFileStruct = NULL;
		}
   }
   else
   {
		if (sFileStruct->getBlock() == NULL)
		{
#if CamCadMajorMinorVersion > 406  //  > 4.6
         sDoc->getFileList().deleteFile(sFileStruct);
#else
			POSITION filePos = sDoc->getFileList().Find(sFileStruct);
			sDoc->FreeFile(sFileStruct);
			sDoc->getFileList().RemoveAt(filePos);
#endif
			sFileStruct = NULL;

			ErrorMessage("This Edif file does not contain any schematic sheet.  Netlist only schematic files are not support in CAMCAD.", "Edif File Error\n");
			res = RC_INVALID_FORMAT;
		}
		else if (sFileStruct->getBlock())
		{
			// If there is no view set to show and there is only one view, then set that one view to show
			if (!sFileStruct->isShown())
				sFileStruct->setShow(true);

         sAttribVisibleMap.applyAttributeVisibility(*sDoc);
			CheckPageSize();

         CString designatorBackAnnotate, pinNumBackAnnotate;
         AutoDetectSchematicDesignator(*sDoc, *sFileStruct, designatorBackAnnotate, pinNumBackAnnotate, sDesignatorBackannotateArray, sPinNumBackannotateArray);
			
         if (!designatorBackAnnotate.IsEmpty())
         {
            if (!pinNumBackAnnotate.IsEmpty() && pinNumBackAnnotate != SCH_ATT_DESIGNATOR)
            {
               // If pin designator Keyword is not the same as "DESIGNATOR" keyword, then rename
		         ////RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), designatorBackAnnotate, pinNumBackAnnotate, sDesignatorLocBackannotate, sPinNumLocBackannotate);
               RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), sDesignatorBackannotateArray, pinNumBackAnnotate, sDesignatorLocBackannotate, sPinNumLocBackannotate);
            }
            else if (designatorBackAnnotate != SCH_ATT_DESIGNATOR)
            {
               // If symbol designator Keyword is not the same as "DESIGNATOR" keyword, then rename
		         ////RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), designatorBackAnnotate, "", sDesignatorLocBackannotate, sPinNumLocBackannotate);
               RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), sDesignatorBackannotateArray, "", sDesignatorLocBackannotate, sPinNumLocBackannotate);
            }
            else if (!sDesignatorLocBackannotate.IsEmpty() || !sPinNumLocBackannotate.IsEmpty())
            {
               // If there is symbol or pin designator lcoation backAnnotate, then process it
		         ////RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), designatorBackAnnotate, pinNumBackAnnotate, sDesignatorLocBackannotate, sPinNumLocBackannotate);
               RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), sDesignatorBackannotateArray, pinNumBackAnnotate, sDesignatorLocBackannotate, sPinNumLocBackannotate);
            }
         }

			int err = CheckMissingOrDuplicateDesignator(sDoc, sFileStruct, sFileErr, sDisplayErr);
			if (err == SCH_ERR_DESIGNATOR_BLANK || err == SCH_ERR_DESIGNATOR_DUPLICATE)
			{
				fprintf(sFileErr, "NOTE: This schematic might not be suitable for use with Schematic Link because some instances are either missing designator or has duplicate designator.\n");
				sDisplayErr++;
			}
		}
	}


   // Release resource
   FreeMembers();
   fclose(sFileErr);
   fclose(sFileIn);

   if (sDisplayErr)
      Logreader(edifLogFile);

   return res;
}

CString getValueFromPortNameAttrib(CAttributes* attributeMap)
{
   CString retval;
   Attrib* attrib = NULL;
   WORD portNameKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_PORTNAME, 0, valueTypeString);

   if (attributeMap != NULL && attributeMap->Lookup(portNameKw, attrib) && attrib != NULL)
   {
      retval = GetAttribStringValue(attrib);
   }

   return retval;
}

CString getValueFromDesignatorAttrib(CAttributes* attributeMap)
{
   CString retval;
   Attrib* attrib = NULL;
   WORD designatorKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString);

   if (attributeMap != NULL && attributeMap->Lookup(designatorKw, attrib) && attrib != NULL)
   {
      retval = GetAttribStringValue(attrib);
   }

   return retval;
}

CString getValueFromPrimaryNameAttrib(CAttributes* attributeMap)
{
   CString retval;
   Attrib* attrib = NULL;
   WORD primaryNamerKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_PRIMARYNAME, 0, valueTypeString);

   if (attributeMap != NULL && attributeMap->Lookup(primaryNamerKw, attrib) && attrib != NULL)
   {
      retval = GetAttribStringValue(attrib);
   }

   return retval;
}

/******************************************************************************
* InitializeMembers
*/
static void InitializeMembers()
{
	edifStruct.reset();

   sIsTokenName = FALSE;
   sIsNewLine = TRUE;
   sPushToken = FALSE;
   sCurrentLine[0] = '\0';

   sFileStruct = NULL;
   sHasHierarchy = FALSE;

   sDefaultSetting.unitRef.Empty();
   sDefaultSetting.textHeight = 0;
	sCamCadDatabase = new CCamCadDatabase(*sDoc);

   sPortRefArr.SetSize(20, 20);

   sUnitRef.Empty();
   sDistanceUnitRef.Empty();
   sAngleUnitRef.Empty();
   sObjectRef.Empty();
   sLibraryRef.Empty();
   sClusterRef.Empty();
   sInstanceRef.Empty();
   sSymbolRef.Empty();

   sLibraryName.Empty();
   sCellName.Empty();
	sPageName.Empty();
   sClusterName.Empty();
   sInterfaceDes.Empty();
   sPropertyValue.Empty();

   sRemoveDisplay = FALSE;
   sDesignCommand = FALSE;

   sTextHeight = 0;
   sJustification = 0;

   sFont = NULL;
   sUnit = NULL;

   sCurrentBlock = NULL;
   sCurrentData = NULL;
   sPolyList = NULL;
   sPoly = NULL;

   sNotLogicSymbol.RemoveAll();
   sAttribVisibleMap.empty();
}

/******************************************************************************
* FreeMembers
*/
static void FreeMembers()
{
	edifStruct.reset();

   CString key;
   void *voidPtr;
   POSITION pos;
   
   // sUnitMap
   pos = sUnitMap.GetStartPosition();
   while (pos)
   {
      sUnitMap.GetNextAssoc(pos, key, voidPtr);
      EdifUnitDef *e = (EdifUnitDef*)voidPtr;
      delete e;
		e = NULL;
   }
   sUnitMap.RemoveAll();

   // sFontMap
   pos = sFontMap.GetStartPosition();
   while (pos)
   {
      sFontMap.GetNextAssoc(pos, key, voidPtr);
      EdifFontDef *e = (EdifFontDef*)voidPtr;
      delete e;
		e = NULL;
   }
   sFontMap.RemoveAll();

   // sGlobalPortMap
	sGlobalPortMap.empty();

   // sPortMap
	sPortMap.empty();

   // sFigureGroupMap
   pos = sFigureGroupMap.GetStartPosition();
   while (pos)
   {
		CFigureGroup *e = NULL;
      sFigureGroupMap.GetNextAssoc(pos, key, e);
      delete e;
		e = NULL;
   }
   sFigureGroupMap.RemoveAll();

   // sEdifKeywordMap
   sEdifKeywordMap.RemoveAll();

	// sTransformList
	pos = sTransformList.GetHeadPosition();
	while (pos != NULL)
	{
		CTransform* transform = sTransformList.GetNext(pos);
		delete transform;
		transform = NULL;
	}
	sTransformList.RemoveAll();

	// sDisplayList
	pos = sDisplayList.GetHeadPosition();
	while(pos != NULL)
	{
		CDisplay* display = sDisplayList.GetNext(pos);
		delete display;
		display = NULL;
	}
	sDisplayList.RemoveAll();

	//sOccurrenceAnnotate
	pos = sOccurrenceAnnotate.GetHeadPosition();
	while (pos != NULL)
	{
		COccurrenceAnnotate* occurrentAnnotate = sOccurrenceAnnotate.GetNext(pos);
		delete occurrentAnnotate;
		occurrentAnnotate = NULL;
	}
	sOccurrenceAnnotate.RemoveAll();

	sDesList.RemoveAll();

   sNotLogicSymbol.RemoveAll();
   sAttribVisibleMap.empty();
}


/******************************************************************************
* LoadSettingFile
*/
static void LoadSettingFile()
{
   // Initialize all the setting file variables
   sOverWriteAttributeDisplay = TRUE;
   sDesignatorBackannotateArray.RemoveAll();
   sDesignatorBackannotateArray.SetSize(0, 10);
   sPinNumBackannotateArray.RemoveAll();
   sPinNumBackannotateArray.SetSize(0, 10);
   sNetBackannotate.Empty();
   sDesignatorLocBackannotate.Empty();
   sPinNumLocBackannotate.Empty();
   sFontScale = 1.0;
   sExchangeXYScale = FALSE;
	sMinimumPageSize = 0;
   sAttribVisibleMap.empty();

   FILE *fp;
   char line[255];
   char *lp;

   CString settingsFile( getApp().getImportSettingsFilePath("Edif300.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nEDIF300 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);

   if ((fp = fopen(settingsFile, "rt")) == NULL)
   {
      // no settings file found 
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      ErrorMessage(tmp, "Edif 300 Settings", MB_OK | MB_ICONHAND);
      return;
   }

   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".NOT_A_LOGIC_SYMBOL"))
         {
            // Get library name
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString tmpString = lp;

            // Get cell name
            if ((lp = get_string(NULL, " \t\n")) != NULL)
               tmpString = tmpString + SCH_DELIMITER + lp;

            // Get cluster name
            if ((lp = get_string(NULL, " \t\n")) != NULL)
               tmpString = tmpString + SCH_DELIMITER + lp;

            // Get symbol name
            if ((lp = get_string(NULL, " \t\n")) != NULL)
               tmpString = tmpString + SCH_DELIMITER + lp;

            sNotLogicSymbol.SetAt(tmpString, tmpString);
         }
         else if (!STRICMP(lp, ".OVERWRITE_ATTRIBUTE_DISPLAY"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if(toupper(lp[0]) == 'N')
               sOverWriteAttributeDisplay = FALSE;
         }
         else if (!STRICMP(lp, ".DESIGNATOR_BACKANNOTATE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            sDesignatorBackannotateArray.SetAtGrow(sDesignatorBackannotateArray.GetCount(), lp);
         }
         else if (!STRICMP(lp, ".PINNO_BACKANNOTATE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            sPinNumBackannotateArray.SetAtGrow(sPinNumBackannotateArray.GetCount(), lp);
         }
         else if (!STRICMP(lp, ".NET_BACKANNOTATE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            sNetBackannotate = lp;
         }
         else if (!STRICMP(lp, ".DESIGNATOR_LOCATION_BACKANNOTATE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            sDesignatorLocBackannotate = lp;
         }
         else if (!STRICMP(lp, ".PINNO_LOCATION_BACKANNOTATE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            sPinNumLocBackannotate = lp;
         }
         else if (!STRICMP(lp, ".FONT_SCALE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            sFontScale = atof(lp);
         }
         else if (!STRICMP(lp, ".Exchange_XY_Scale"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if(toupper(lp[0]) == 'Y')
               sExchangeXYScale = TRUE;
         }
         else if (!STRICMP(lp, ".MINIMUM_PAGE_SIZE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
				sMinimumPageSize = atof(lp);
			}
         else if (!STRICMP(lp, ".MAKE_ATTRIBUTE_VISIBLE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString keyword = lp;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            double height = Units_Factor(UNIT_MM, sPageUnit) * atof(lp);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            double width = Units_Factor(UNIT_MM, sPageUnit)* atof(lp);

            sAttribVisibleMap.addAttributeVisibility(keyword.MakeUpper(), height, width);
         }
      }
   }

	fclose(fp);

}

/******************************************************************************
* CreateRequiredLayer
   - create the required layers for Edif
*/
static void CreateRequiredLayer()
{
   Graph_Level("0", "", 1);
   sTextLayer = Graph_Level(SCH_LAYER_TEXT, "", FALSE);
   sPortDesLayer = Graph_Level(SCH_LAYER_PORT_DESIGNATOR, "", FALSE);
   sSymbolDesLayer = Graph_Level(SCH_LAYER_SYMBOL_DESIGNATOR, "", FALSE);
   sPortGrpLayer = Graph_Level(SCH_LAYER_PORT_GRAPHIC, "", FALSE);
   sSymbolGrpLayer = Graph_Level(SCH_LAYER_SYMBOL_GRAPHIC, "", FALSE);
   sSignalGrpLayer = Graph_Level(SCH_LAYER_SIGNAL_GRAPHIC, "", FALSE);
   sBorderGrpLayer = Graph_Level(SCH_LAYER_BORDER_GRAPHIC, "", FALSE);
   sAttribLayer = Graph_Level(SCH_LAYER_ATTRIBUTE, "", FALSE);
}

/******************************************************************************
* CreateDefaultGeometry
   - create the default geometry for ports, junctions, and interconnectors
   - these geometries will be use if there is no graphical presentation in
     the definition of ports, junctions, or interconnectors
*/
static void CreateDefaultGeometry()
{
	// Make the size of half MM in X and Y
   double x = 0.5 * Units_Factor(UNIT_MM, sPageUnit);
   double y = 0.5 * Units_Factor(UNIT_MM, sPageUnit);

   // create default geometry for ports which as a graphic of an X
   BlockStruct *block = Graph_Block_On(GBO_APPEND, "DEFAULTPORT", sFileStruct->getFileNumber(), 0L);
   block->setBlockType(blockTypeGatePort);
   Graph_PolyStruct(sPortGrpLayer, 0L, FALSE);

   Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
   Graph_Vertex(-x, -y, 0.0);
   Graph_Vertex(x, y, 0.0);

   Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
   Graph_Vertex(-x, y, 0.0);
   Graph_Vertex(x, -y, 0.0);  
   Graph_Block_Off();

   // create default geometry for interconnect terminator which as a graphic of a square
   block = Graph_Block_On(GBO_APPEND, "DEFAULTTERMINATOR", sFileStruct->getFileNumber(), 0L);
   block->setBlockType(blockTypeUnknown);
   Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);

   Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
   Graph_Vertex(-x * 2, -y, 0.0);
   Graph_Vertex(-x * 2, y, 0.0);
   Graph_Vertex(x * 2, y, 0.0);
   Graph_Vertex(x * 2, -y, 0.0); 
   Graph_Vertex(-x * 2, -y, 0.0);
   Graph_Block_Off();

   // create default geometry for interconnect terminator which as a graphic of a square
   block = Graph_Block_On(GBO_APPEND, "DEFAULTTIEDOT", sFileStruct->getFileNumber(), 0L);
   block->setBlockType(blockTypeUnknown);
   Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);

   Graph_Poly(NULL, 0, TRUE, FALSE, TRUE);
   Graph_Vertex(-x, 0.0, 1.0);
   Graph_Vertex(x, 0.0, 1.0);
   Graph_Vertex(-x, 0.0, 0.0);
   Graph_Block_Off();
}

/******************************************************************************
* CheckPageSize
	- This function check to see if the X-size or Y-size of the largest schematic sheet
	  is at least the size of the minimum page size specificed in the ".in" file	
*/
static void CheckPageSize()
{
	if (sMinimumPageSize <= 0)
		return;


	// sMinimumPageSize is in INCHES so need to convert to page unit
	double minSize = sMinimumPageSize * Units_Factor(UNIT_INCHES, sPageUnit);

	double factor = 1;
	double xSize = 0;
	double ySize = 0;

	for (int i=0; i<sDoc->getMaxBlockIndex(); i++)
	{
		BlockStruct* block = sDoc->getBlockAt(i);
		if (block == NULL || block->getBlockType() != blockTypeSheet)
			continue;

		sDoc->validateBlockExtents(block);

		if (block->getExtent().getXsize() > xSize)
			xSize = block->getExtent().getXsize();

		if (block->getExtent().getXsize() > ySize)
			ySize = block->getExtent().getYsize();
	}

	while (xSize < minSize || ySize < minSize)
	{
		xSize *= 10;
		ySize *= 10;
		factor *= 10;
	}

	// No need to scale if factor hasn't change
	if (factor == 1)
		return;


	for (int i=0; i<sDoc->getMaxBlockIndex(); i++)
	{
		BlockStruct* block = sDoc->getBlockAt(i);
		if (block == NULL)
			continue;

		// Don't scale aperturn
		if (block->isAperture())
			continue;

		// Don't scale default geometries
		if (block->getName().CompareNoCase("DEFAULTPORT") == 0 ||
			 block->getName().CompareNoCase("DEFAULTTERMINATOR") == 0 ||
			 block->getName().CompareNoCase("DEFAULTTIEDOT") == 0)
			 continue;

		block->Scale(factor);
	}


	POSITION filePos = sDoc->getFileList().GetHeadPosition();
	while (filePos)
	{
		FileStruct *file = sDoc->getFileList().GetNext(filePos);

		file->setInsertX((DbUnit)(file->getInsertX() * factor));
		file->setInsertY((DbUnit)(file->getInsertY() * factor));
		
		file->getNetList().Scale(factor);
		file->getBusList().Scale(factor);
	}

	sDoc->OnFitPageToImage();
}

/******************************************************************************
* LoopCommand
*/
static int LoopCommand(List *list, int size)
{
   int repeat = TRUE;
   while (repeat)
   {
      if (GoCommand(list, size) < 0)
         return -1;

      if (!GetToken())
      {
         if (feof(sFileIn))
            return 0;
         else
            return ParsingErr();
      }

      if (TokenSearch(parenthesis_lst, SIZ_PARENTHESIS_LST) == SCH_PARENTHESIS_RIGHT)
         repeat = FALSE;
      PushToken();
   }

   return 1;
}

/******************************************************************************
* go_command()
*   Call function associated with next token. Tokens enclosed by () are searched for on the local token list.
*/
static int GoCommand(List *list, int size)
{
   if (!GetToken()) 
      return ParsingErr();

   switch(TokenSearch(parenthesis_lst, SIZ_PARENTHESIS_LST))
   {
   case SCH_PARENTHESIS_LEFT:
      {
         int i = -1;
         if (GetToken() && (i = TokenSearch(list, size)) >= 0)
         {
            if ((*list[i].function)() < 0)
               return -1;
         }
         else
         {
#ifdef _DEBUG
            fprintf(sFileErr, "Line: %d  Token: %s  not in list\n", sFileInLineCnt, sToken);
            sDisplayErr++;
#endif
            skipCommand();
         }
      }
      break;
   case SCH_PARENTHESIS_RIGHT:
      {
         PushToken();                                                          
         return 1;
      }
   default:
      return ParsingErr();
   }

   if (!GetToken() || TokenSearch(parenthesis_lst, SIZ_PARENTHESIS_LST) != SCH_PARENTHESIS_RIGHT)
      return ParsingErr();

   return 1;
}


/******************************************************************************
* get_tok()
*  - Get a token from the input file.
*/
static int GetToken()
{
   if (!sPushToken)
   {
      while (!GetNext(sCurrentLine, sIsNewLine))
      {
         if (!GetLine(sCurrentLine, MAX_LINE))
            return(FALSE);
         else
            sIsNewLine = TRUE;
      }
      sIsNewLine = FALSE;
   }
   else
      sPushToken = FALSE;

   return(TRUE);
}

/******************************************************************************
* PushToken
*/
static int PushToken()
{
   return (sPushToken = TRUE);
}

/******************************************************************************
* tok_search
*  - Search for the token in a command token list.
*/
static int TokenSearch(List *list, int size)
{
   for (int i=0; i<size; ++i)
   {
      if (!STRCMPI(sToken, list[i].token))
      {
         if (sToken[0] == '(')
            return SCH_PARENTHESIS_LEFT;
         else if (sToken[0] == ')')
            return SCH_PARENTHESIS_RIGHT;
         else
            return i;
      }
   }

   return -1;
}

/******************************************************************************
* GetNext
*/
static int GetNext(char *inputLine, int isNewLine)
{
   int i = 0;
   sIsTokenName = FALSE;

   static char *line;
   if (sIsNewLine)
      line = inputLine;

   while(isspace(*line) && *line != '\0')
      ++line;

   switch(*line)
   {
   case '\0':
      return 0;
   case '(':
   case ')':
      sToken[i++] = *(line++);
      break;
   case '\"':
      {
         sToken[i] = '\0';
         sIsTokenName = TRUE;
         for (++line; *line != '\"'; ++line, ++i)
         {
            if (i >= MAX_TOKEN - 1)
            {
               fprintf(sFileErr, "Error in Get Token - Token too long at %ld\n", sFileInLineCnt);
               sDisplayErr++;
            }

            if (*line == '\n')
            {
               /* text spans over 1 line */
               if (!GetLine(sCurrentLine, MAX_LINE))
               {
                  fprintf(sFileErr, "Error in Line Read\n");
                  sDisplayErr++;
               }

               line = sCurrentLine;
               if (*line == '\"')   // new line starts with a end_quote.
                  break;
            }

            sToken[i] = *line;
         }
         ++line;
      }
      break;
   default:
      {
         while (!isspace(*line) && *line != '\"' && *line != '(' && *line != ')' && *line != '\0')
         {
            if (*line == '\\')
               ++line;
            sToken[i] = *line;
            ++line;
            ++i;
         }
      }
      break;
   }

   if (!i && !sIsTokenName)
      return(FALSE);

   sToken[i] = '\0';
   convert_special_char(sToken);

   return(TRUE);
}

/******************************************************************************
* GetLine
   Get a line from the input file.
*/
static int GetLine(char *inputLine, int size)
{
   if (fgets(inputLine, size, sFileIn) == NULL)
   {
      if (!feof(sFileIn))
      {
         ErrorMessage("Fatal EDIF Parse Error", "Read Error !", MB_OK | MB_ICONHAND);
         return FALSE;
      }
      else
      {
         inputLine[0] = '\0';
         return(FALSE);
      }
   }
   else
   {
      if (inputLine[strlen(inputLine)-1] != '\n')
      {
         fprintf(sFileErr, " No end of line found at %ld\n", sFileInLineCnt);
			fprintf(sFileErr, " Probable internal error, buffer overrun. Please go to http://www.mentor.com/supportnet for support.\n");
         sDisplayErr++;
         return (FALSE);
      }
   }
   ++sFileInLineCnt;

   return(TRUE);
}

/******************************************************************************
* static void  convert_special_char
   name or rename
*/
static void convert_special_char(char *t)
{
   int esc = 0;
   CString n;
   CString m;

   for (int i=0; i<(int)strlen(t); i++)
   {
      if (t[i] == '%')
      {
         if (esc)
         {
            char tmp[255];
            char *lp;
            strcpy(tmp, m);
            if (lp = strtok(tmp, " \t"))
            {
               n += (char)atoi(lp);
               while (lp = strtok(NULL, " \t"))
               {
                  n += (char)atoi(lp);
               }
            }

            m.Empty();
            esc = FALSE;
         }
         else
            esc = TRUE;
      }
      else
      {
         if (esc)
            m += t[i];
         else
            n += t[i];
      }
   }

   strcpy(t, n);
   return;
}

/******************************************************************************
* skip
   Skip everything assocated with this command
*/
static int skipCommand()
{
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!GetToken())
         return ParsingErr();

      switch (TokenSearch(parenthesis_lst, SIZ_PARENTHESIS_LST))
      {
      case SCH_PARENTHESIS_LEFT:
         ++parenthesisCount;
         break;
      case SCH_PARENTHESIS_RIGHT:
         if (parenthesisCount-- == 0)
         {
            PushToken();
            return 1;
         }
      default:
         break;
      }
   }
}

/******************************************************************************
* ParsingErr
*/
static int ParsingErr()
{
   fprintf(sFileErr, "Syntax Error : Token \"%s\" on line %ld\n", sToken, sFileInLineCnt);
   sDisplayErr++;
   return -1;
}

/******************************************************************************
* GetString
   Get the name
*/
static int GetString(CString &stringValue)
{
   stringValue.Empty();

   if (!GetToken())
      return ParsingErr();

   if (sToken[0] == '(' && !sIsTokenName)
      PushToken();
   else
   {
      stringValue = sToken;
      stringValue.Replace("%", "%%");

      // Make the string upper case if it is not inside a pair quotes
      if (!sIsTokenName)
         stringValue.MakeUpper();
   }

   return 1;
}

/******************************************************************************
* GetIntegerToken
*/
static int GetIntegerToken(int &integerValue)
{
   integerValue = 0;
   if (!GetToken())
      return ParsingErr();

   integerValue = atoi(sToken);
   return 1;
}

/******************************************************************************
* GetScaleInteger
*/
static int GetScaleInteger(double &scaleValue)
{
   scaleValue = 0.0;
   if (!GetToken())  // get the '('
      return ParsingErr();

   if (sToken[0] == '(')
   {
      if (!GetToken())  // get the 'e'
         return ParsingErr();
      if (sToken[0] != 'e')
         return ParsingErr();

      if (!GetToken())  // get the mantissa
         return ParsingErr();
      double mantissa = atof(sToken);

      if (!GetToken()) // get the exponent
         return ParsingErr();
      double exponent = atof(sToken);

      if (!GetToken()) // get the ")"
         return ParsingErr();

      scaleValue = mantissa * pow(10., exponent);
   }
   else
      scaleValue = atof(sToken);

   return 1;   
}

/******************************************************************************
* GetBoolean
*/
static int GetBoolean(BOOL &boolValue)
{
   boolValue = FALSE;
   if (!GetToken())  // get the "("
      return ParsingErr();
   if (!GetToken())  // get the boolean value
      return ParsingErr();
   
   if (!STRCMPI(sToken, "TRUE"))
      boolValue = TRUE;
   else
      boolValue = FALSE;

   if (!GetToken())  // get the ")"
      return ParsingErr();

   return 1;
}

/******************************************************************************
* GetPoint
   - Get the X & Y for the construct "pt", "numberPoint", "scaleX", and "scaleY"
*/
static int GetPoint(CPnt *pnt, BOOL convertUnit)
{
   pnt->x = 0.0;
   pnt->y = 0.0;
   pnt->bulge = 0.0;

   if (!GetToken()) // Get the "("
      return ParsingErr();
   if (sToken[0] != '(')
   {
      PushToken();
      return 0;
   }

   if (!GetToken())
      return ParsingErr();
   if (STRCMPI(sToken, "pt") && STRCMPI(sToken, "numberPoint") &&
       STRCMPI(sToken, "scaleX") && STRCMPI(sToken, "scaleY"))
      return ParsingErr();

   double x = 0.0;
   double y = 0.0;
   int res;
   if ((res = GetScaleInteger(x)) < 0)
      return res;
   if ((res = GetScaleInteger(y)) < 0)
      return res;

   if (convertUnit)
   {
      pnt->x = (float)ConvertUnit(sDistanceUnitRef, x, NULL);
      pnt->y = (float)ConvertUnit(sDistanceUnitRef, y, NULL);
   }
   else
   {
      pnt->x = (float)x;
      pnt->y = (float)y;
   }
   pnt->bulge = 0;

   if (!GetToken())  // Get the ")"
      return ParsingErr();

   return 1;
}

/******************************************************************************
* GetPoints
   Get the list of points within the same construct such
*/
static int GetPoints(CPntList &pntList)
{
   while (TRUE)
   {
      CPnt *pnt = new CPnt();
      int res = GetPoint(pnt, TRUE);
      if (res > 0)
      {
         pntList.AddTail(pnt);
      }
      else
      {
         delete pnt;
         return res;
      }
   }
}

/******************************************************************************
* GetPointList
   - This is similar to GetPoints except that it is inside a "pointList" construct
*/
static int GetPointList(CPntList &pntList)
{
   if (!GetToken())  // get the "("
      return ParsingErr();
   if (!GetToken())  // get the word "pointList"
      return ParsingErr();
   if (STRCMPI(sToken, "pointList"))
      return ParsingErr();

   int res;
   if ((res = GetPoints(pntList)) < 0)
      return res;

   if (!GetToken())  // Get the ")"
      return ParsingErr();

   return 1;
}

/******************************************************************************
* GetCurve
*/
static int GetCurve(CPntList &pntList)
{
   if (!GetToken())  // get the "("
      return ParsingErr();
   if (!GetToken())  // get the word "curve"
      return ParsingErr();
   if (STRCMPI(sToken, "curve"))
      return ParsingErr();
   if (!GetToken())  // get the "("
      return ParsingErr();
          
   BOOL isArc = FALSE;
   CPnt *pntOnArc = NULL;
   CPnt *pnt = NULL;
   while(sToken[0] == '(')
   {
      if (!GetToken())  // Get the word "pt", "numberPoint", or "arc"
      {  
         if (pntOnArc)
            delete pntOnArc;
         return ParsingErr();
      }

      if (!STRCMPI(sToken, "arc"))
      {
         if (!GetToken())  // Get the "("
         {  
            if (pntOnArc)
               delete pntOnArc;
            return ParsingErr();
         }
         isArc = TRUE;
         continue;
      }
      else if (!STRCMPI(sToken, "numberPoint")) // point on the arc
      {
         double x = 0.0;
         double y = 0.0;
         int res;
         if ((res = GetScaleInteger(x)) < 0)
         {  
            if (pntOnArc)
               delete pntOnArc;
            return res;
         }
         if ((res = GetScaleInteger(y)) < 0)
         {  
            if (pntOnArc)
               delete pntOnArc;
            return res;
         }
   
         pntOnArc = new CPnt();
         pntOnArc->x = (float)ConvertUnit(sDistanceUnitRef, x, NULL);
         pntOnArc->y = (float)ConvertUnit(sDistanceUnitRef, y, NULL);
         pntOnArc->bulge = 0;

         if (!GetToken())  // Get the ")"
         {  
            if (pntOnArc)
               delete pntOnArc;
            return ParsingErr();
         }
         if (!GetToken())  // Get the "("
         {  
            if (pntOnArc)
               delete pntOnArc;
            return ParsingErr();
         }
      }
      else if (!STRCMPI(sToken, "pt"))
      {
         double x = 0.0;
         double y = 0.0;
         int res;
         if ((res = GetScaleInteger(x)) < 0)
         {  
            if (pntOnArc)
               delete pntOnArc;
            return res;
         }
         if ((res = GetScaleInteger(y)) < 0)
         {  
            if (pntOnArc)
               delete pntOnArc;
            return res;
         }

         pnt = new CPnt();
         pnt->x = (float)ConvertUnit(sDistanceUnitRef, x, NULL);
         pnt->y = (float)ConvertUnit(sDistanceUnitRef, y, NULL);
         pnt->bulge = 0;

         if (pntOnArc)  // there is an arc so find the bulge for the starting point
         {
            double startAngle;
            double deltaAngle;
            double centerX;
            double centerY;
            double radius;
            CPnt *pntStart = pntList.GetTail();

            ArcPoint3(pntStart->x, pntStart->y, pntOnArc->x, pntOnArc->y, pnt->x, pnt->y, &centerX, &centerY, &radius, &startAngle, &deltaAngle);
            pntStart->bulge = (float)tan(deltaAngle/4);

            delete pntOnArc;
            pntOnArc = NULL;

            if (!GetToken())  // Get the ")"
            {  
               if (pntOnArc)
                  delete pntOnArc;
               delete pnt;
               return ParsingErr();
            }
         }
         pntList.AddTail(pnt);

         if (!GetToken())  // Get the ")"
         {  
            if (pntOnArc)
               delete pntOnArc;
            return ParsingErr();
         }
         if (!GetToken())  // Get the "("
         {  
            if (pntOnArc)
               delete pntOnArc;
            return ParsingErr();
         }
      }
      else
      {  
         if (pntOnArc)
            delete pntOnArc;
         return ParsingErr();
      }
   }

   return 1;
}

/******************************************************************************
* GetDate
*/
static int GetDate(int &year, int &month, int &date)
{
   year = 0;
   month = 0;
   date = 0;

   if (!GetToken())  // get the "("
      return ParsingErr();
   if (!GetToken())  // get the word "date"
      return ParsingErr();
   if (STRCMPI(sToken, "date"))
      return ParsingErr();

   if (!GetToken())
      return ParsingErr();
   year = atoi(sToken);

   if (!GetToken())
      return ParsingErr();
   month = atoi(sToken);

   if (!GetToken())
      return ParsingErr();
   date = atoi(sToken);

   if (!GetToken())  // get the ")"
      return ParsingErr();

   return 1;
}

/******************************************************************************
* ConvertUnit
   - if basic unit is meter, then convert to pageunit & multiply numOfNewUnit
   - if basic unit is degree, then convert to radian  & multiply numOfNewUnit
   - if anything else, then just multiply numOfNewUnit
*/
static double ConvertUnit(CString unitRef, double numOfNewUnit, void *basicUnitName)
{
   void *voidPtr;
   if (sUnitMap.Lookup(unitRef, voidPtr))
   {
      EdifUnitDef *unit = (EdifUnitDef*)voidPtr;

      basicUnitName = "";
      double value = (unit->numOfBasicUnits / unit->numOfNewUnits);

      if (!unit->basicUnitName.CompareNoCase("meter"))
      {
         return numOfNewUnit * value * Units_Factor(UNIT_MM, sPageUnit) * 1000;
      }
      else if (!unit->basicUnitName.CompareNoCase("degree"))
      {
         return DegToRad(value * numOfNewUnit);
      }
      else
      {
         basicUnitName = (void*)&(unit->basicUnitName);
         return numOfNewUnit;
      }
   }

   basicUnitName = "";

   return numOfNewUnit;
}

/******************************************************************************
* GetBasicUnitName
*/
static CString GetBasicUnitName(CString unitRef)
{
   void *voidPtr;
   if (sUnitMap.Lookup(unitRef, voidPtr))
   {
      EdifUnitDef *unit = (EdifUnitDef*)voidPtr;
      return unit->basicUnitName;
   }

   return "";
}
/******************************************************************************
* edit300
*/
static int edif300()
{
   CString edifFileName;
   int res;

   if ((res = GetString(edifFileName)) < 0)
      return res;

	edifStruct.reset();

   sFileStruct = Graph_File_Start(edifFileName, Type_EDIF);
   sFileStruct->setBlockType(blockTypeSheet);
   sFileStruct->setBlock(NULL);   // Initialize to NULL, will be set to sCurrentBlock in page()
   sFileStruct->setShow(false);       // Initialize to FALSE, will be set to true in SchematicInstanceImplementation()

   CreateDefaultGeometry();

   res = LoopCommand(edif300_lst, SIZ_EDIF300_LST);

   // Create netlist
   edifStruct.createNetList();

   return res;
}

/******************************************************************************
* edifHeader
*/
static int edifHeader()
{
   return LoopCommand(edifHeader_lst, SIZ_EDIFHEADER_LST);
}

/******************************************************************************
* design
*/
static int design()
{
   sObjectRef.Empty();
   sLibraryRef.Empty();

   CString designName;
   int res;

   if ((res = GetString(designName)) < 0)
      return res;

   // Before starting the Design section, we need to create physical representation of the logical schematic
   // if it is not already done so we can to annotation properly
   if (!edifStruct.isSchematicConvertionDone())
   {
      edifStruct.convertLogicalSchematicToPhysicalSchematic();
   }

   if ((res = LoopCommand(design_lst, SIZ_DESIGN_LST)) < 0)
      return res;

	CEdifCell* cell = edifStruct.findCell(sLibraryRef, sObjectRef);
	if (cell != NULL)
   {
		BlockStruct* pageBlock = cell->getFirstPage();
		if (pageBlock != NULL)
		{
         // Show the first page of the view specified by design
			sFileStruct->setBlock(pageBlock);

			WORD hierarchyKeyword = (WORD)sDoc->RegisterKeyWord(SCH_ATT_HIERARCHGEOM, 0, valueTypeString);
			Attrib* attrib = NULL;
			if (pageBlock->getAttributes() && pageBlock->getAttributes()->Lookup(hierarchyKeyword, attrib))
			{
				RemoveAttrib(hierarchyKeyword, &pageBlock->getAttributesRef());
			}
		}
	}

   sDesignCommand = TRUE;
   return res;
}

/******************************************************************************
* external
   External is the same as library, except it is referencing a library not 
   actually present within the current EDIF data.  It is a library with minimal
   information and is known to be incomplete
*/
static int external()
{
   CString libraryName;
   int res;

   if ((res = GetString(libraryName)) < 0)
      return res;

	edifStruct.addLibrary(libraryName);

   CString msg;
   msg.Format("Edif is referencing an external libray, %s, which is known to be incomplete.", libraryName);
   ErrorMessage(msg, "Warning");

   return LoopCommand(library_lst, SIZ_LIBRARY_LST);
}

/******************************************************************************
* library
*/
static int library()
{
   int res;
   if ((res = GetString(sLibraryName)) < 0)
      return res;

	edifStruct.addLibrary(sLibraryName);

   return LoopCommand(library_lst, SIZ_LIBRARY_LST);
}

/******************************************************************************
* edifLevel
*/
static int edifLevel()
{ 
   int level;
   int res;
   if ((res = GetIntegerToken(level)) < 0)
      return res;

   if (level > 0)
   {
      CString msg;
      msg.Format("EDIF Level %d found at %ld\nOnly Level 0 is supported.", level, sFileInLineCnt);
      ErrorMessage(msg, "Unsupported EDIF Level",  MB_OK | MB_ICONHAND);
   }

   return 1;
}

/******************************************************************************
* keywordMap
*/
static int keywordMap()
{
   return LoopCommand(keywordMap_lst, SIZ_KEYWORDMAP_LST);
}

/******************************************************************************
* notSupportKeywordLevel
*/
static int notSupportKeywordLevel()
{
   ErrorMessage("Unable to continue reading because this Edit file contain keywords mapping.", "Error");
   return -1;
}

/******************************************************************************
* unitDefinitions
   - get the unit definition and put them in a global array sUnitDefArray
*/
static int unitDefinitions()
{
   return LoopCommand(unitDefinitions_lst, SIZ_UNITDEFINITIONS_LST);
}

/******************************************************************************
* unit
   - get the new unit name and the number of new and basic units
   - the basic unit name will be given in unitExponent()
*/
static int unit()
{
   int res;
   CString newUnitName;

   if ((res = GetString(newUnitName)) < 0)
      return res;

   // Create a newly define unit and initialize it
   sUnit = new EdifUnitDef();
   sUnit->basicUnitName.Empty();

   if ((res = GetScaleInteger(sUnit->numOfNewUnits)) < 0)
   {
      delete sUnit;
      return res;
   }
   if ((res = GetScaleInteger(sUnit->numOfBasicUnits)) < 0)
   {
      delete sUnit;
      return res;
   }
   if ((res = LoopCommand(unit_lst, SIZ_UNIT_LST)) < 0)
   {
      delete sUnit;
      return res;
   }

   void *voidPtr;
   if (sUnitMap.Lookup(newUnitName, voidPtr))
   {
      EdifUnitDef *tmpUnit = (EdifUnitDef*)voidPtr;
      delete tmpUnit;
   }
   sUnitMap.SetAt(newUnitName, sUnit);
   sUnit = NULL;
   return 1;
}

/******************************************************************************
* unitExponent
   - get the basic unit name
   - Here we are assuming the exponent is either 1 or -1, if not then change later 
*/
static int unitExponent()
{
   CString unitName = sToken;
   double exponent;
   int res;
   if ((res = GetScaleInteger(exponent)) < 0)
      return res;

   if (sUnit->basicUnitName == "")  // first time, just one basic unit
      sUnit->basicUnitName = unitName;
   else if (exponent > 0)              // not first time, more than one basic unit. Ex: kilogram meter
      sUnit->basicUnitName.Format("%s %s", sUnit->basicUnitName, unitName);
   else if (exponent < 0)              // not first time, more than one basic unit. Ex: miles / second
      sUnit->basicUnitName.Format("%s/%s", sUnit->basicUnitName, unitName);

   return 1;
}

/******************************************************************************
* fontDefinitions
   - fines the list of fonts to be use in the Edif file
*/
static int fontDefinitions()
{
   return LoopCommand(fontDefinitions_lst, SIZ_FONTDEFINITIONS_LST);
}

/******************************************************************************
* fonts
*/
static int fonts()
{
   return LoopCommand(fonts_lst, SIZ_FONTS_LST);
}

/******************************************************************************
* setDistance
   - set the unit to be use form measurement
*/
static int setDistance()
{
   int res = LoopCommand(setUnit_lst, SIZ_SETUNIT_LST);
   if (res < 0)
      return res;

   sDistanceUnitRef = sUnitRef;
   return 1;
}

/******************************************************************************
* unitRef
   - get the unit reference name
*/
static int unitRef()
{
   return GetString(sUnitRef);
}

/******************************************************************************
* font
   - defines an individual font style
   - we only can about the font height and width and ignore the rest
*/
static int font()
{
   int res;
   CString fontName;

   if ((res = GetString(fontName)) < 0)
      return res;
      
   sFont = new EdifFontDef();
   sFont->fontHeight = 0;
   sFont->fontWidth = 0;

   if ((res = LoopCommand(font_lst, SIZ_FONT_LST)) < 0)
   {
      delete sFont;
      return res;
   }

   void *voidPtr;
   if (sFontMap.Lookup(fontName, voidPtr))
   {
      EdifFontDef *tmpFont = (EdifFontDef*)voidPtr;
      delete tmpFont;
   }

   // If there is not font width, then font width is 4/7 of the height
   if (sFont->fontWidth == 0)
      sFont->fontWidth = 4/7 * sFont->fontHeight;

   sFontMap.SetAt(fontName, sFont);
   sFont = NULL;
   return 1;
}

/******************************************************************************
* fontProportions
   - defines the size of the font
*/
static int fontProportions()
{
   int res = LoopCommand(fontProportions_lst, SIZ_FONTPROPORTIONS_LST);
   if (res < 0)
      return res;

   return 1;
}

/******************************************************************************
* fontHeight
   - get the font height and conver it to the pageunit
*/
static int fontHeight()
{
   int height;
   int res;
   if ((res = GetIntegerToken(height)) < 0)
      return res;

   // The font height unit is measured in the unit defined in Edif file
   sFont->fontHeight = ConvertUnit(sDistanceUnitRef, height, NULL);
   return 1;
}

/******************************************************************************
* fontWidth
   - get the font width and convert it to the pageunit
*/
static int fontWidth()
{
   int width;
   int res;
   if ((res = GetIntegerToken(width)) < 0)
      return res;

   // The font width unit is measured in the unit defined in Edif file
   sFont->fontWidth = ConvertUnit(sDistanceUnitRef, width, NULL);
   return 1;
}

/******************************************************************************
* physicalDefaults
   - defines the schematicRequireDefaults
*/
static int physicalDefaults()
{
   return LoopCommand(physicalDefaults_lst, SIZ_PHYSICALDEFAULTS_LST);
}

/******************************************************************************
* schematicRequireDefaults
   - defines the default unit, textheight, and font to use for the Edif file
*/
static int schematicRequireDefaults()
{
   sDistanceUnitRef.Empty();
   sTextHeight = 0;
   sFont = NULL;

   int res = LoopCommand(schematicRequireDefaults_lst, SIZ_SCHEMATICREQUIREDEFAULTS_LST);
   if (res < 0)
      return res;

   sDefaultSetting.unitRef = sDistanceUnitRef;
   if (sFont)
   {
      sDefaultSetting.fontHeight = sFont->fontHeight;
      sDefaultSetting.fontWidth = sFont->fontWidth;
      sFont = NULL;
   }

   if (sTextHeight > 0)
   {
      if (sDefaultSetting.fontHeight > 0)
      {
         //Edif use textheight to scale the text to the desire height
         double textScale = sTextHeight / sDefaultSetting.fontHeight;

         sDefaultSetting.fontHeight *= textScale;
         sDefaultSetting.fontWidth *= textScale;
      }
      else
      {
         // There was no font width so use the text height as font height
         sDefaultSetting.fontHeight = sTextHeight;
         sDefaultSetting.fontWidth = sTextHeight * 4/7;  // width will be 4/7 of font height
      }
   }

   if (sFontScale > 0)
   {
      sDefaultSetting.fontHeight *= sFontScale;
      sDefaultSetting.fontWidth *= sFontScale;
   }

   return 1;
}

/******************************************************************************
* schematicMetric
   - defines the scaling for schematic
   - currenly we only care about the unit and ignore the rest
*/
static int schematicMetric()
{
   return LoopCommand(schematicMetric_lst, SIZ_SCHEMATICMETRIC_LST);
}

/******************************************************************************
* fontRef
   - get the font referece name and fine the position in the font map;
*/
static int fontRef()
{
   CString fontName;
   int res = GetString(fontName);
   if (res < 0)
      return res;

   void *voidPtr;
   if (sFontMap.Lookup(fontName, voidPtr))
      sFont = (EdifFontDef*)voidPtr;
   else
      return -1;

   return 1;
}

/******************************************************************************
* textHeight
   - ge the text height
*/
static int textHeight()
{
   int height;
   int res;
   if ((res = GetIntegerToken(height)) < 0)
      return res;

   // The text height unit is measured in the unit defined in Edif file
   sTextHeight = ConvertUnit(sDistanceUnitRef, height, NULL);
   return 1;
}

/******************************************************************************
* globalPortDefinitions
   - defines the list of global ports
*/
static int globalPortDefinitions()
{                                  
   return LoopCommand(globalPortDefinitions_lst, SIZ_GLOBALPORTDEFINITIONS_LST);
}

/******************************************************************************
* globalPort
   - defines the individual global port or global port bundle
*/
static int globalPort()
{
   sPortRefArr.RemoveAll();

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   int res;
   CString globalPortName;

   if ((res = GetString(globalPortName)) < 0)
      return res;

   Attrib* attrib;

   // Add the port name as DisplayName attribute, this can be change later to something else
   int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, globalPortName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   // Add the port name as PortName attribute, so the port name is saved
   keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, globalPortName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   // Add GlobalPort attribute to indicate this port is global
   keyword = sDoc->RegisterKeyWord("GlobalPort", 0, valueTypeString);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, "", attributeUpdateOverwrite, NULL);

   if ((res = LoopCommand(globalPort_lst, SIZ_GLOBALPORT_LST)) < 0)
      return res;

   CEdifPort* globalPort = new CEdifPort();
   globalPort->isBundle = FALSE;
   globalPort->attribMap = NULL;
   
   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&globalPort->attribMap, attribMap);
   delete attribMap;
   attribMap = NULL;

   if (sPortRefArr.GetSize() > 0)
   {
      // There is a list of global port ref; therefore, this global port bundle 
      globalPort->portRefArr.Copy(sPortRefArr);
      globalPort->isBundle = TRUE;
      sPortRefArr.RemoveAll();
   }

   CEdifPort* tmpPort = NULL;
   if (sGlobalPortMap.Lookup(globalPortName, tmpPort))
   {
      delete tmpPort;
		tmpPort = NULL;
   }
   sGlobalPortMap.SetAt(globalPortName, globalPort);
   return 1;
}

/******************************************************************************
* CopyANDRemoveAttribMap
   - copy fromMap to copyToMap 
   - delete fromMap
*/
static int CopyANDRemoveAttribMap(CAttributes** copyToMap, CAttributes* fromMap)
{
   if (!fromMap || fromMap->GetCount() == 0)
      return -1;

   if (*copyToMap == NULL) // copyToMap is NULL so just assign fromMap to it  
   {
      sDoc->CopyAttribs(&(*copyToMap), fromMap);
      return 1;
   }

   // copyToMap is not NULL, so copy fromMap to it
   POSITION pos = fromMap->GetStartPosition();
   while (pos)
   {                                                          
      WORD keyword;
      Attrib* fromAttrib;
      Attrib* toAttrib;

      fromMap->GetNextAssoc(pos, keyword, fromAttrib);

      if ((*copyToMap)->Lookup(keyword, toAttrib))   // the same attribute exist in copyToMap
      {
         if (fromAttrib->getFlags() != 0)
         {
            if (fromAttrib->getFlags() & ATTRIB_CHANGE_VALUE)
            {
               // the attribute of fromMap has no visible location 
               // so just copy the value and delete the attribute in fromMap
               if (fromAttrib->getValueType() == valueTypeString)
                  toAttrib->setStringValueIndex(fromAttrib->getStringValueIndex());
               else if (fromAttrib->getValueType() == VT_INTEGER)
                  toAttrib->setIntValue(fromAttrib->getIntValue());
               else if (fromAttrib->getValueType() == VT_DOUBLE)
                  toAttrib->setDoubleValue(fromAttrib->getDoubleValue());
            }
            
            if (fromAttrib->getFlags() & ATTRIB_CHANGE_LOCATION)
            {
               // the attribute of fromMap has only modified location and not value
               // so save the old value, copy the attribute of fromMap to copyToMap,
               // then re-assign the value back to the attribute of copyToMap
               double oldValStr;          
               int oldValType = toAttrib->getValueType();

               if (oldValType == valueTypeString)
                  oldValStr = toAttrib->getStringValueIndex();
               else if (oldValType == VT_INTEGER)
                  oldValStr = toAttrib->getIntValue();
               else if (oldValType == VT_DOUBLE)
                  oldValStr = toAttrib->getDoubleValue();

               *toAttrib = *fromAttrib;

               if (oldValType == valueTypeString)
                  toAttrib->setStringValueIndex((int)oldValStr);
               else if (oldValType == VT_INTEGER)
                  toAttrib->setIntValue((int)oldValStr);
               else if (oldValType == VT_DOUBLE)
                  toAttrib->setDoubleValue(oldValStr);
            }
            
            if (fromAttrib->getFlags() & ATTRIB_CHANGE_VISIBLE)
				{
               toAttrib->setVisible( fromAttrib->isVisible());
					toAttrib->setLayerIndex( fromAttrib->getLayerIndex());
				}

            toAttrib->setFlagBits(fromAttrib->getFlags());
         }
         else
         {
            // the attribute of fromMap has new value and location
            // so assign it to the attribute of copyToMap to overwrite it
            *toAttrib = *fromAttrib;
         }
      }
      else
      {
         // there is not such attribute exist in copyToMap
         // so add it to the map of copyToMap
         Attrib *newAttrib = fromAttrib->allocateCopy();
         (*copyToMap)->SetAt(keyword, newAttrib);
      }
   }

   return 1;
}

/******************************************************************************
* portList
   - defines the list of global port and master port reference
*/
static int portList()
{
   sPortRefArr.RemoveAll();
   return LoopCommand(portList_lst, SIZ_PORTLIST_LST);
}

/******************************************************************************
* globalPortRef
   - get the global port reference
*/
static int globalPortRef()
{
   CString globalPortNameRef;
   int res;

   if ((res = GetString(globalPortNameRef)) < 0)
      return res;

   sPortRefArr.Add(globalPortNameRef);
   return 1;
}

/******************************************************************************
* portRef
   - Reference the master port that is define inside the "interface" construct
*/
static int portRef()
{
   CString portRef;
   int res;

   if ((res = GetString(portRef)) < 0)
      return res;

   sPortRefArr.Add(portRef);
   return 1;
}

/******************************************************************************
* nameInformation
   - defines the display name
*/
static int nameInformation()
{
   return LoopCommand(nameInformation_lst, SIZ_NAMEINFORMATION_LST);
}

/******************************************************************************
* primaryName
   - get the primary name, which is basic name of the object in the source system
*/
static int primaryName()
{
   CString name;
   int res;

   if ((res = GetString(name)) < 0)
      return res;

   DoStringAttributeValue(SCH_ATT_PRIMARYNAME, name);

   return LoopCommand(primaryName_lst, SIZ_PRIMARYNAME_LST);
}

/******************************************************************************
* displayName
   - get the string displayed on the source system which gives a visual name to the object.
     This may not be the actual name used internally in the source system for referencing the object. 
*/
static int displayName()
{
   CString name;
   int res;

   if ((res = GetString(name)) < 0)
      return res;

   if (!name.IsEmpty())
      return DoStringAttributeValue(SCH_ATT_DISPLAYNAME, name);
   else
      return res;
}

/******************************************************************************
* property
   - get the property name and the value that
*/
static int property()
{
   sPropertyValue.Empty();
   sPropertyType = valueTypeString;
   CString edifPropertyName;
   int res;

   if ((res = GetString(edifPropertyName)) < 0)
      return res;

   if ((res = LoopCommand(property_lst, SIZ_PROPERTY_LST)) < 0)
      return res;
   
   CString ccKeyword = edifPropertyName;
   int keywordIndex = sDoc->IsKeyWord(ccKeyword, 0);
   if (keywordIndex < 0)
   {
      keywordIndex = sDoc->RegisterKeyWord(ccKeyword, 0, sPropertyType);
   }
   else if (sDoc->getKeyWordArray()[keywordIndex]->getValueType() != sPropertyType)
   {
      ccKeyword.Insert(0, "Edif.");
      keywordIndex = sDoc->RegisterKeyWord(ccKeyword, 0, sPropertyType);
   }
   sEdifKeywordMap.SetAt(edifPropertyName, ccKeyword);


   CAttributes* attribMap = sAttribMapList.GetTail();
   if (attribMap == NULL)
      attribMap = new CAttributes();

   Attrib* attrib;

   if (attribMap->Lookup(keywordIndex, attrib))
   {
      if (attrib->getValueType() == valueTypeString)
         attrib->setStringValueIndex(sDoc->RegisterValue(sPropertyValue));
      else if (attrib->getValueType() == VT_INTEGER)
         attrib->setDoubleValue(atoi(sPropertyValue));
      else if (attrib->getValueType() == VT_DOUBLE)
         attrib->setDoubleValue(atof(sPropertyValue));
   }
   else 
   {
      sDoc->SetAttrib(&attribMap, keywordIndex, sPropertyType, sPropertyValue.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   }

   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   return 1;
}

/******************************************************************************
* booleanPropertyValue
   - get the boolean value of a property
*/
static int booleanPropertyValue()
{
   BOOL value = FALSE;
   int res;
   if ((res = GetBoolean(value)) < 0)
      return res;

   sPropertyType = (int)valueTypeString;
   if (value)
      sPropertyValue = "TRUE";
   else
      sPropertyValue = "FALSE";

   return 1;
}

/******************************************************************************
* integerPropertyValue
   - get the integer value of a property
*/
static int integerPropertyValue()
{
   if (!GetToken())
      return ParsingErr();

   double value = atof(sToken);

   CString unitName;
   double tmpValue = ConvertUnit(sUnitRef, value, unitName.GetBuffer(0));   

   sPropertyType = (int)valueTypeString;

   if (value > floor(value))
      sPropertyValue.Format("%f", value);
   else 
      sPropertyValue.Format("%0.0f", value);

   if (unitName != "")  // not a unit support by CAMCAD so attach the unit name to the value
      sPropertyValue.AppendFormat(" %s", unitName);

   return 1;
}

/******************************************************************************
* miNoMaxPropertyValue
   - get the minimal-nominal-maximal value of a property
   - it is a single regular number or three number or string for the minimal, nominal, and maximal
     ex: (mnm 25 30 (e 335 -1))
         (mnm (unconstrained) (undefined) 60), .. etc.
   - the resulting value wil be a string
     ex: "min=25 nom=30 max=0.0029"
         "min=unconstrained nom=undefined max=60"
*/
static int miNoMaxPropertyValue()
{
   CString value; 
   double dblValue = 0;
   int res = GetScaleInteger(dblValue);

   if (res < 0 && !STRCMPI(sToken, "mnm")) // this is a minimal-nominal-maximal value
   {
      // get the minimal number
      res = GetScaleInteger(dblValue);
      value = "min=";
      if (res < 0)
      {
         value += sToken;  // not a number, it is a string of "undefined" or "unconstrainted"
         if (!GetToken())  // get the ')'
            return ParsingErr();
      }
      else
      {
         CString unitName;
         dblValue = ConvertUnit(sUnitRef, dblValue, unitName.GetBuffer(0));

         if (unitName != "")  // not a unit support by CAMCAD so attach the unit name to the value
            value.Format("%s%f %s", value, dblValue, unitName);
         else
            value.Format("%s%f", value, dblValue);
      }

      // get the nominal number
      res = GetScaleInteger(dblValue);
      value += " nom=";
      if (res < 0)
      {
         value += sToken;  
         if (!GetToken())
            return ParsingErr();
      }
      else
      {
         CString unitName;
         dblValue = ConvertUnit(sUnitRef, dblValue, unitName.GetBuffer(0));

         if (unitName != "")  // not a unit support by CAMCAD so attach the unit name to the value
            value.Format("%s%f %s", value, dblValue, unitName);
         else
            value.Format("%s%f", value, dblValue);
      }
   
      // get the maximal number
      value += " max=";
      if (res < 0)
      {
         value += sToken;

         if (!GetToken())
            return ParsingErr();
      }
      else
      {
         CString unitName;
         dblValue = ConvertUnit(sUnitRef, dblValue, unitName.GetBuffer(0));

         if (unitName != "")  // not a unit support by CAMCAD so attach the unit name to the value
            value.Format("%s%f %s", value, dblValue, unitName);
         else
            value.Format("%s%f", value, dblValue);
      }

      if (!GetToken())  // get the ')'
         return ParsingErr();
   }
   else  // it is a single regular number
   {
      CString unitName;
      dblValue = ConvertUnit(sUnitRef, dblValue, unitName.GetBuffer(0));

      if (unitName != "")  // not a unit support by CAMCAD so attach the unit name to the value
         value.Format("%s%f %s", value, dblValue, unitName);
      else
         value.Format("%s%f", value, dblValue);
   }

   sPropertyValue = value;    // add property value
   sPropertyType = valueTypeString;

   return 1;
}

/******************************************************************************
* numberPropertyValue
   - get the numeric value of a property
*/
static int numberPropertyValue()
{
   double dblValue = 0.0;
   int res;
    if ((res = GetScaleInteger(dblValue)) < 0)
      return ParsingErr();

   CString unitName;
   double tmpDblValue = ConvertUnit(sUnitRef, dblValue, unitName.GetBuffer(0));   

   sPropertyType = (int)valueTypeString;
   if (dblValue > floor(dblValue))
      sPropertyValue.Format("%f", dblValue);
   else 
      sPropertyValue.Format("%0.0f", dblValue);

   if (unitName != "")  // not a unit support by CAMCAD so attach the unit name to the value
      sPropertyValue.AppendFormat(" %s", unitName);

   return 1;
}

/******************************************************************************
* pointPropertyValue
   - get the point value of a property
*/
static int pointPropertyValue()
{
   CPnt *pnt = new CPnt();
   int res;
   if ((res = GetPoint(pnt, FALSE)) < 1)
   {
      delete pnt;
      return -1;
   }

   CString unitName;
   double x = ConvertUnit(sUnitRef, pnt->x, unitName.GetBuffer(0));
   double y = ConvertUnit(sUnitRef, pnt->y, unitName.GetBuffer(0));

   sPropertyType = (int)valueTypeString;
   if (unitName != "")  // not a unit support by CAMCAD so attach the unit name to the value
      sPropertyValue.Format("x=%f %s, y=%f %s", x, unitName, y, unitName);
   else
      sPropertyValue.Format("x=%f y=%f", x, y);

   delete pnt;
   return 1;
}

/******************************************************************************
* stringPropertyValue
   - get the string value of a property
*/
static int stringPropertyValue()
{
   CString value;
   int res;

   if ((res = GetString(value)) < 0)
      return res;

   sPropertyType = (int)valueTypeString;
   sPropertyValue = value;    // add property value

   return 1;
}

/******************************************************************************
* physicalScaling
   - defines the measurement units schematic, connectivity, and more.
   - currently we only care about the units for schematic
*/
static int physicalScaling()
{
   return LoopCommand(physicalScaling_lst, SIZ_PHYSICALSCALING_LST);
}

static int status()
{
   return LoopCommand(status_lst, SIZ_STATUS_LST);
}

static int written()
{
   return LoopCommand(written_lst, SIZ_WRITTEN_LST);
}

static int program()
{
	int res = 0;
	CString programName;

   if ((res = GetString(programName)) < 0)
		return res;

	if (programName.Find("mgsiwrite") > -1)
      sEdifProgram = edifProgramMgsiwrite;
   else if (programName.Find("c2esch") > -1)
      sEdifProgram = edifProgramC2esch;

	// Skip the rest of other command
	skipCommand();

	return 1;
}

/******************************************************************************
* schematicUnits
   - defines the measurement units for distance, angle, time, etc.
   - currently we only care about the distance and angle measurement
*/
static int schematicUnits()
{
   return LoopCommand(schematicUnits_lst, SIZ_SCHEMATICUNITS_LST);
}

/******************************************************************************
* setAngle
   - defines the measurement units for angle
*/
static int setAngle()
{
   int res = LoopCommand(setUnit_lst, SIZ_SETUNIT_LST);
   if (res < 0)
      return res;

   sAngleUnitRef = sUnitRef;
   return 1;
}

/******************************************************************************
* libraryHeader
   - defines the technology and properties about the library.
   - properties are currently ignore since there is no needed for yet
*/
static int libraryHeader()
{
   return LoopCommand(libraryHeader_lst, SIZ_LIBRARYHEADER_LST);
}

/******************************************************************************
* technology
   - defines the physcial scaling and figure groups fro the library
*/
static int technology()
{
   return LoopCommand(technology_lst, SIZ_TECHNOLOGY_LST);
}

/******************************************************************************
* figureGroup
   - fines the path width and display attributes 
*/
static int figureGroup()
{
	sFigureGroup.Reset();

   sTextHeight = 0;
   sFont = NULL;

   int res;
   CString figureGroupName;

   if ((res = GetString(figureGroupName)) < 0)
      return res;

   if ((res = LoopCommand(figureGroup_lst, SIZ_FIGUREGROUP_LST)) < 0)
      return res;

   if (sFont)
   {
		sFigureGroup.SetFontHeight(sFont->fontHeight);
      sFigureGroup.SetFontWidth(sFont->fontWidth);
      sFont = NULL;
   }

   if (sTextHeight > 0)
   {
      if (sFigureGroup.GetFontHeight() > 0)
      {
         //Edif use textheight to scale the text to the desire height
         double textScale = sTextHeight / sFigureGroup.GetFontHeight();

         sFigureGroup.SetFontHeight(sFigureGroup.GetFontHeight() * textScale);
         sFigureGroup.SetFontWidth(sFigureGroup.GetFontWidth() * textScale);
      }
      else
      {
         // There was no font width so use the text height as font height
         sFigureGroup.SetFontHeight(sTextHeight);
         sFigureGroup.SetFontWidth(sTextHeight * 4/7);  // width will be 4/7 of font height
      }
   }

   if (sFontScale > 0)
   {
      sFigureGroup.SetFontHeight(sFigureGroup.GetFontHeight() * sFontScale);
      sFigureGroup.SetFontWidth(sFigureGroup.GetFontWidth() * sFontScale);
   }


   CFigureGroup *figureGrp = new CFigureGroup(sFigureGroup);

   CFigureGroup *tmpFigureGroup = NULL;
   if (sFigureGroupMap.Lookup(figureGroupName, tmpFigureGroup))
   {
      delete tmpFigureGroup;
		tmpFigureGroup = NULL;
   }
   sFigureGroupMap.SetAt(figureGroupName, figureGrp);
   return 1;
}

/******************************************************************************
* displayAttributes
   - defines the color, text height, horizontal and vertical justicafication
*/
static int displayAttributes()
{
   return LoopCommand(displayAttributes_lst, SIZ_DISPLAYATTRIBUTES_LST);
}

/******************************************************************************
* color
   - get the RGB color value
   - since the color construct is only used by displayAttributes which is only used by figureGroup
     so we can assign it directly into the last figureGroup created
*/
static int color()
{
   int red = 0;
   int green = 0;
   int blue = 0;
   int res;
   if ((res = GetIntegerToken(red)) < 0)
      return res;
   if ((res = GetIntegerToken(green)) < 0)
      return res;
   if ((res = GetIntegerToken(blue)) < 0)
      return res;

   sFigureGroup.SetColor(RGB(red, green, blue));

   return 1;
}

/******************************************************************************
* horizontalJustification
   - get the horizontal justification value
   - since the this construct is only used by displayAttributes which is only used by figureGroup
     so we can assign it directly into the last figureGroup created
*/
static int horizontalJustification()
{
   int res = LoopCommand(justification_lst, SIZ_JUSTIFICATION_LST);
   if (res < 0)
      return res;

   sFigureGroup.SetHorizontalAlignment(sJustification);

   return 1;
}

/******************************************************************************
* verticalJustification
   - get the vertical justification value
   - since the thiss construct is only used by displayAttributes which is only used by figureGroup
     so we can assign it directly into the last figureGroup created
*/
static int verticalJustification()
{
   int res = LoopCommand(justification_lst, SIZ_JUSTIFICATION_LST);
   if (res < 0)
      return res;

   sFigureGroup.SetVerticalAlignment(sJustification);

   return 1;
}

/******************************************************************************
* getJustification
   - get the justification value
*/
static int getJustification()
{
   if (!STRCMPI(sToken, "leftJustify"))
      sJustification = SCH_JUSTIFY_LEFT;
   else if (!STRCMPI(sToken, "rightJustify"))
      sJustification = SCH_JUSTIFY_RIGHT;
   else if (!STRCMPI(sToken, "centerJustify") || !STRCMPI(sToken, "middleJustify"))
      sJustification = SCH_JUSTIFY_CENTER;
   else if (!STRCMPI(sToken, "topJustify") || !STRCMPI(sToken, "caplineJustify"))
      sJustification = SCH_JUSTIFY_TOP;
   else if (!STRCMPI(sToken, "bottomJustify") || !STRCMPI(sToken, "baselineJustify"))
      sJustification = SCH_JUSTIFY_BOTTOM;
   else
      sJustification = -1;

   return sJustification;
}

/******************************************************************************
* visible
   - get the visible value for display
*/
static int visible()
{
	BOOL tmpVisible = false;
   int res = GetBoolean(tmpVisible);
	sFigureGroup.SetVisible((tmpVisible==0)?false:true);

	return res;
}

/******************************************************************************
* pathWidth
   - get the pathWidth value
   - since the pathWidth construct is only used by figureGroup
     so we can assign it directly into the last figureGroup created
*/
static int pathWidth()
{
   double width;
   if (!GetToken())
      return ParsingErr();

   if (sToken[0] == '(') // token is "("
   {
      if (!GetToken())  // get the string "minimalWidth"
         return ParsingErr();
      if (STRCMPI(sToken, "minimalWidth"))
         return ParsingErr();

      // width is minimalWidth
      width = 0.0;   

      if (!GetToken())  // get the ")"
         return ParsingErr();
   }
   else
      width = ConvertUnit(sDistanceUnitRef, atof(sToken), NULL);

   int err;
   CString widthName;
   widthName.Format("PathWidth_%f", width);
   int widthIndex = Graph_Aperture(widthName, T_ROUND, width, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err); 
	sFigureGroup.SetWidthIndex(widthIndex);

   return 1;
}

/******************************************************************************
* geometryMacro
   - define a set of graphical entities for later use within a figure
   - this create a geometry
*/
static int geometryMacro()
{
   CString geometryMacroName;
   int res;

   if ((res = GetString(geometryMacroName)) < 0)
      return res;

   CString blockName;

   blockName.Format("geomMacro_%s%c%s", sLibraryName, SCH_DELIMITER, geometryMacroName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(geometryMacro_lst, SIZ_GEOMETRYMACRO_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
   delete data->getPolyList();
   data->getPolyList() = sPolyList;
   Graph_Block_Off();
   
   sPolyList = NULL;
   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* geometryMacroHeader
*/
static int geometryMacroHeader()
{
   return LoopCommand(geometryMacroHeader_lst, SIZ_GEOMETRYMACROHEADER_LST);
}

/******************************************************************************
* circle
   - The two set of points are the end of the diameter of the circle
   - The width is specified in the widthIndex of sFigureGroup.
*/
static int circle()
{
   CreatePoly();
   int res = GetPoints(sPoly->getPntList());
   if (res > -1)
   {
      CPnt *firstPnt = (CPnt*)sPoly->getPntList().GetHead();
      firstPnt->bulge = 1;

      CPnt *lastPnt = (CPnt*)sPoly->getPntList().GetTail();
      lastPnt->bulge = 1;

      // Add the last point to close the circle
      lastPnt = new CPnt();
      lastPnt->x = firstPnt->x;
      lastPnt->y = firstPnt->y;
      lastPnt->bulge = 0.0;
      sPoly->getPntList().AddTail(lastPnt);
      sPoly->setClosed(true);
   }
   AddPoly();

   return res;
}

/******************************************************************************
* dot
   - The geometric entity defined by a single point and use as marker. 
   - Neither the size of the dot nor the shape is specified. 
   - The width is specified in the widthIndex of sFigureGroup.
*/
static int dot()
{
   CreatePoly();
   int res = GetPoints(sPoly->getPntList());
   AddPoly();

   return res;
}

/******************************************************************************
* openShape
   - It is like a path except that it may have arc, and it is not fill
   - The width is specified in the widthIndex of sFigureGroup.
*/
static int openShape()
{
   CreatePoly();
   int res = GetCurve(sPoly->getPntList());
   AddPoly();

   return res;
}

/******************************************************************************
* Shape
   - It is like a polygon except that it may have arcs, and it is closed by definition.
   - If the last point specified is not the same as the first point specified,
     add a point back to the first point to close the shape.
   - The width is specified in the widthIndex of sFigureGroup.
*/
static int shape()
{
   CreatePoly();
   int res = GetCurve(sPoly->getPntList());
   if (res > -1)
   {
      CPnt *firstPnt = (CPnt*)sPoly->getPntList().GetHead();
      CPnt *lastPnt = (CPnt*)sPoly->getPntList().GetTail();

      if (fabs(firstPnt->x - lastPnt->x) > SMALLNUMBER || fabs(firstPnt->y - lastPnt->y) > SMALLNUMBER)
      {
         // Add the last point to close the shape
         lastPnt = new CPnt();
         lastPnt->x = firstPnt->x;
         lastPnt->y = firstPnt->y;
         lastPnt->bulge = 0.0;
         sPoly->getPntList().AddTail(lastPnt);
      }
      sPoly->setClosed(true);
   }
   AddPoly();

   return res;
}

/******************************************************************************
* path
   - It is a collection of joining line segments with an associated width.
   - The width is specified in the widthIndex of sFigureGroup.
*/
static int path()
{
   CreatePoly();
   int res = GetPointList(sPoly->getPntList());
   AddPoly();

   return res;
}

/******************************************************************************
* polygon
   - It is a collection of joining line segments with an associated width.
   - It is closed by definition.
   - If the last point specified is not the same as the first point specified,
     add a point back to the first point to close the polygon.
   - The width is specified in the widthIndex of sFigureGroup.
*/
static int polygon()
{
   CreatePoly();
   int res = GetPointList(sPoly->getPntList());
   if (res > -1)
   {
      CPnt *firstPnt = (CPnt*)sPoly->getPntList().GetHead();
      CPnt *lastPnt = (CPnt*)sPoly->getPntList().GetTail();

      if (fabs(firstPnt->x - lastPnt->x) > SMALLNUMBER || fabs(firstPnt->y - lastPnt->y) > SMALLNUMBER)
      {
         // Add the last point to close the shape
         lastPnt = new CPnt();
         lastPnt->x = firstPnt->x;
         lastPnt->y = firstPnt->y;
         lastPnt->bulge = 0.0;
         sPoly->getPntList().AddTail(lastPnt);
      }
      sPoly->setClosed(true);
   }
   AddPoly();
   
   return res;
}

/******************************************************************************
* rectangle
   - The set of points are the end points of either diagonal of teh rectangle
     which is the first and third points of the rectangle if we counting the
     points in consecutive order
*/
static int rectangle()
{
   CreatePoly();
   CPntList pntList;
   int res = GetPoints(pntList);
   if (res > -1 && pntList.GetCount() == 2)
   {
      CPnt *thirdPnt = (CPnt*)pntList.RemoveTail();   // remove second set of points from list
      CPnt *firstPnt = (CPnt*)pntList.RemoveTail();   // remove first set of points from list

      // add the first point of rectangle
      sPoly->getPntList().AddTail(firstPnt);

      // add the second point of rectangle
      CPnt *pnt = new CPnt();
      pnt->x = firstPnt->x;
      pnt->y = thirdPnt->y;
      pnt->bulge = 0;
      sPoly->getPntList().AddTail(pnt);

      // add the third point of rectangle
      sPoly->getPntList().AddTail(thirdPnt);

      // add the forth point of rectangle
      pnt = new CPnt();
      pnt->x = thirdPnt->x;
      pnt->y = firstPnt->y;
      pnt->bulge = 0;
      sPoly->getPntList().AddTail(pnt);

      // copy the first point to the last point and add it to close the rectangle
      pnt = new CPnt();
      pnt->x = firstPnt->x;
      pnt->y = firstPnt->y;
      pnt->bulge = 0;
      sPoly->getPntList().AddTail(pnt);
      sPoly->setClosed(true);
   }
   AddPoly();

   return res;
}

/******************************************************************************
* geometryMacroUnits
*/
static int geometryMacroUnits()
{
   return LoopCommand(geometryMacroUnits_lst, SIZ_GEOMETRYMACROUNITS_LST);
}

/******************************************************************************
* pageBorderTemplate
   - defines the graphics associated with the border of a schematic page.
   - This typically includes graphics for such items as the border itself, company logos, etc. 
*/
static int pageBorderTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("pageBorder_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(pageBorderTemplate_lst, SIZ_PAGEBORDERTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data = Graph_PolyStruct(sBorderGrpLayer, 0L, FALSE);
   delete data->getPolyList();
   data->getPolyList() = sPolyList;
   Graph_Block_Off();

   sPolyList = NULL;
   sCurrentBlock = NULL;
   
   return 1;
}

/******************************************************************************
* commentGraphics
   - is used to include comment figures and text within an object
*/
static int commentGraphics()
{
   return LoopCommand(commentGraphics_lst, SIZ_COMMENTGRAPHICS_LST);
}

/******************************************************************************
* pageTitleBlockTemplate
   - defines the graphic for later use by pageTitleBlock
*/
static int pageTitleBlockTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("pageTitle_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(pageTitleBlockTemplate_lst, SIZ_PAGETITLEBLOCKTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data = Graph_PolyStruct(sAttribLayer, 0L, FALSE);
   delete data->getPolyList();
   data->getPolyList() = sPolyList;
   Graph_Block_Off();

   sPolyList = NULL;
   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* schematicFigureMacro
   - creates the geometry that defines figures and annotation text to be use in many places later on
*/
static int schematicFigureMacro()
{
   CString figureMacroName;
   int res;

   if ((res = GetString(figureMacroName)) < 0)
      return res;

   CString blockName;
   blockName.Format("figMacro_%s%c%s", sLibraryName, SCH_DELIMITER, figureMacroName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicFigureMacro_lst, SIZ_SCHEMATICFIGUREMACRO_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
   delete data->getPolyList();
   data->getPolyList() = sPolyList;
   Graph_Block_Off();

   sPolyList = NULL;
   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* schematicTemplateHeader
   - defines the measurement unit and properties for schematicFigureMacro
*/
static int schematicTemplateHeader()
{
   return LoopCommand(schematicTemplateHeader_lst, SIZ_SCHEMATICTEMPLATEHEADER_LST);
}

/******************************************************************************
* schematicGlobalPortTemplate
   - defines the graphic representation for global port
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicGlobalPortTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("globalPort_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeGatePort);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicGlobalPortTemplate_lst, SIZ_SCHEMATICGLOBALPORTTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      data = Graph_Block_Reference("DEFAULTPORT", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
      delete sPolyList;
      sPolyList = NULL;
   }
   else
   {
      data = Graph_PolyStruct(sPortGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }

   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
} 

/******************************************************************************
* schematicInterconnectTerminatorTemplate
   - defines the graphic representation for interconnect terminator
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicInterconnectTerminatorTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("terminator_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(getTemplate_lst, SIZ_GETTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      data = Graph_Block_Reference("DEFAULTTERMINATOR", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
      delete sPolyList;
      sPolyList = NULL;
   }
   else
   {
      data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }
   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* schematicJunctionTemplate
   - defines the graphic representation for a tie dot
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicJunctionTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("tieDot_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(getTemplate_lst, SIZ_GETTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      delete sPolyList;
      sPolyList = NULL;
      data = Graph_Block_Reference("DEFAULTTIEDOT", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
   }
   else
   {
      data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }
   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* schematicMasterPortTemplate
   - defines the graphic representation for a master port
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicMasterPortTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("masterPort_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeGatePort);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicMasterPortTemplate_lst, SIZ_SCHEMATICMASTERPORTTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      delete sPolyList;
      sPolyList = NULL;
      data = Graph_Block_Reference("DEFAULTPORT", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
   }
   else
   {
      data = Graph_PolyStruct(sPortGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }
   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
}        

/******************************************************************************
* schematicOffPageConnectorTemplate
   - defines the graphic representation for off page connector
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicOffPageConnectorTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("offPage_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(getTemplate_lst, SIZ_GETTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      delete sPolyList;
      sPolyList = NULL;
      data = Graph_Block_Reference("DEFAULTTERMINATOR", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
   }
   else
   {
      data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }
   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* schematicOnPageConnectorTemplate
   - defines the graphic representation for on page connector
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicOnPageConnectorTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("onPage_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(getTemplate_lst, SIZ_GETTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      delete sPolyList;
      sPolyList = NULL;
      data = Graph_Block_Reference("DEFAULTTERMINATOR", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
   }
   else
   {
      data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }
   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* schematicRipperTemplate
   - defines the graphic representation for ripper
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicRipperTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("ripper_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeUnknown);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(getTemplate_lst, SIZ_GETTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      delete sPolyList;
      sPolyList = NULL;
      data = Graph_Block_Reference("DEFAULTTERMINATOR", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
   }
   else
   {
      data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }
   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* schematicSymbolPortTemplate
   - defines the graphic representation for port
   - create a geometry
   - if there is no graphic for it use default graphic
*/
static int schematicSymbolPortTemplate()
{
   CString templateName;
   int res;

   if ((res = GetString(templateName)) < 0)
      return res;

   CString blockName;
   blockName.Format("symbolPort_%s%c%s", sLibraryName, SCH_DELIMITER, templateName);
   blockName.MakeUpper();
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeGatePort);

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicSymbolPortTemplate_lst, SIZ_SCHEMATICSYMBOLPORTTEMPLATE_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   DataStruct *data;
   if (sPolyList->GetCount() == 0)
   {
      delete sPolyList;
      sPolyList = NULL;
      data = Graph_Block_Reference("DEFAULTPORT", "", sFileStruct->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, sPortGrpLayer, TRUE);
      data->getInsert()->setInsertType(insertTypeUnknown);
   }
   else
   {
      data = Graph_PolyStruct(sPortGrpLayer, 0L, FALSE);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;
      sPolyList = NULL;
   }
   Graph_Block_Off();

   sCurrentBlock = NULL;

   return 1;
}

/******************************************************************************
* cell
   - defines a group of cluster that have the same functionality
*/
static int cell()
{
   int res;
   if ((res = GetString(sCellName)) < 0)
      return res;
         
	CEdifCell* cell = edifStruct.getCurrentLibrary()->addCell(sCellName);
   if ((res = LoopCommand(cell_lst, SIZ_CELL_LST)) < 0)
      return res;
   
   return res;
}

/******************************************************************************
* cluster
   - defines a group of schematic symbols that share the same interface
*/
static int cluster()
{
   int res;
   if ((res = GetString(sClusterName)) < 0)
      return res;

   CEdifCell* cell = edifStruct.getCurrentLibrary()->getCurrentCell();
	CEdifCluster* cluster = cell->addCluster(sClusterName);
   cell->setCurrentCluster(cluster);

   if ((res = LoopCommand(cluster_lst, SIZ_CLUSTER_LST)) < 0)
      return res;

	cluster->attachHierarchySymbolToView();
   cell->setCurrentCluster(NULL);

   return 1;
}

/******************************************************************************
* Interface
   - defines a group of ports that are used by the schemtic symbols within the same cluster
*/
static int Interface()
{
   // remove all the ports for previous interface
   sPortMap.empty();

   sInterfaceDes.Empty();
   sDesList.AddTail(sInterfaceDes);

   int res;
   if ((res = LoopCommand(interface_lst, SIZ_INTERFACE_LST)) < 0)
      return res;

   sInterfaceDes = sDesList.RemoveTail();
   return 1;
}

/******************************************************************************
* designator
   - get the designator and assign it to the last item of the stack(list)
*/
static int designator()
{
   CString desValue;
   int res;

   if ((res = GetString(desValue)) < 0)
      return res;

   POSITION pos = sDesList.GetTailPosition();
   sDesList.SetAt(pos, desValue);

   return 1;
}

/******************************************************************************
* port
   - Define the master port
*/
static int port()
{
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   sPortRefArr.RemoveAll();

   CString portName;
   int res;

   if ((res = GetString(portName)) < 0)
      return res;

   Attrib* attrib;

   // Add the port name as DisplayName attribute, this can be change later to something else
   int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, portName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   // Add the port name as PortName attribute, so the port name is saved
   keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, portName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   // By default put the port name as the designator, it can be overwrite later with a really desginator
   CString designator; // = portName;
   sDesList.AddTail(designator);

   if ((res = LoopCommand(port_lst, SIZ_PORT_LST)) < 0)
      return res;

   attribMap = sAttribMapList.RemoveTail();
   designator = sDesList.RemoveTail(); // get & remove the designator from stack

   if (!designator.IsEmpty())
   {
      keyword = sDoc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
      sDoc->SetAttrib(&attribMap, keyword, valueTypeString, designator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
      attrib->setFlagBits(ATTRIB_CHANGE_VALUE);
   }

   CEdifPort* port = new CEdifPort();
   port->isBundle = FALSE;
   port->attribMap = NULL;

   CopyANDRemoveAttribMap(&port->attribMap, attribMap);
   delete attribMap;
   attribMap = NULL;

   if (sPortRefArr.GetSize() > 0)
   {
      // There is a list of port ref; therefore, this port is a bundle 
      port->portRefArr.Copy(sPortRefArr);
      port->isBundle = TRUE;
      sPortRefArr.RemoveAll();
   }

   CEdifPort* tmpPort = NULL;
   if (sPortMap.Lookup(portName, tmpPort))
   {
      delete tmpPort;
		tmpPort = NULL;
   }
   sPortMap.SetAt(portName, port);

   return 1;
}

/******************************************************************************
* clusterHeader
*/
static int clusterHeader()
{
   return LoopCommand(clusterHeader_lst, SIZ_CLUSTERHEADER_LST);
}

/******************************************************************************
* clusterConfiguration
   - Get the cluster configuration which provides: 
      - reference of schematic view within the cluster
      - indication that this cluster is a leaf, which mean there is no further expansion
      - indication that this cluster is unconfigugred
   - currently we oly care about cluster configuration that references a schematic view
*/
static int clusterConfiguration()
{
   CString name;
   int res;

   if ((res = GetString(name)) < 0)
      return res;

   CEdifCluster* cluster = edifStruct.getCurrentCluster();
	CClusterConfiguration* configuration = cluster->addClusterConfiguration(name);
   cluster->setCurrentClusterConfiguration(configuration);

   if ((res = LoopCommand(clusterConfiguration_lst, SIZ_CLUSTERCONFIGURATION_LST)) < 0)
      return res;

	if (!sViewRef.IsEmpty())
	{
		configuration->setViewRef(sViewRef);
	}
	else if (edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView() != NULL)
	{
		configuration->setViewRef(edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->getName());
	}

   cluster->setCurrentClusterConfiguration(NULL);

   return 1;
}

/******************************************************************************
* viewRef
   - get the schematic view reference, this is only use by clusterConfiguration()
     so no cluster reference is allow inside the viewRef
*/
static int viewRef()
{
   return GetString(sViewRef);
}

static int instanceConfiguration()
{
   CString name;
   int res;

   if ((res = GetString(name)) < 0)
      return res;

   if ((res = LoopCommand(instanceConfiguration_lst, SIZ_INSTANCECONFIGURATION_LST)) < 0)
      return res;

   CEdifCluster* cluster = edifStruct.getCurrentCluster();
	CClusterConfiguration* clusterConfiguration = cluster->getCurrentClusterConfiguration();
   if (clusterConfiguration != NULL && !sClusterConfigRef.Trim().IsEmpty())
   {
      CInstanceConfiguration* instanceConfiguration = clusterConfiguration->addInstanceConfiguration(name);
      instanceConfiguration->setClusterConfigurationRef(sClusterConfigRef);
   }

   return res;
}

static int defaultClusterConfiguration()
{
   CString name;
   int res;

   if ((res = GetString(name)) < 0)
      return res;

   CEdifCluster* cluster = edifStruct.getCurrentCluster();
   cluster->setDefaultConfiguration(name);
   return res;
}

/******************************************************************************
* schematicSymbol
   - create the schematic geometry
*/
static int schematicSymbol()
{
   CString symbolName;
   int res;

   if ((res = GetString(symbolName)) < 0)
      return res;

	CEdifCluster* cluster = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster();
	CEdifSymbol* symbol = cluster->addSymbol(symbolName);
	cluster->setCurrentSymbol(symbol);

   sCurrentBlock = Graph_Block_On(GBO_APPEND, symbol->generateSymbolBlockName(), sFileStruct->getFileNumber(), 0L);
	symbol->setSymbolBlock(sCurrentBlock);

   // initially set to BLOCKTYPE_UNKNOWN, if there is schematicSymbolPortImplementation later on then change to BLOCKTYPE_SYMBOL
   sCurrentBlock->setBlockType(blockTypeUnknown);   

   sPolyList = new CPolyList();
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   // add the interface's designator to the schematic symbol,
   // this designator could be change later at the instance
   Attrib* attrib;
   int keyword;
   if (!sInterfaceDes.IsEmpty())
   {
      keyword = sDoc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
      sDoc->SetAttrib(&attribMap, keyword, valueTypeString, sInterfaceDes.GetBuffer(0), attributeUpdateOverwrite, &attrib);
      attrib->setFlagBits(ATTRIB_CHANGE_VALUE);                                                       
   }

   // Add the symbol name as DisplayName attribute, this can be change later to something else
   keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, symbolName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   // Add more attributes
   keyword = sDoc->IsKeyWord(SCH_ATT_LIBRARYNAME, 0);
   sDoc->SetAttrib(&sCurrentBlock->getAttributesRef(), keyword, valueTypeString, sLibraryName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);                                                       
   keyword = sDoc->IsKeyWord(SCH_ATT_CELLNAME, 0);
   sDoc->SetAttrib(&sCurrentBlock->getAttributesRef(), keyword, valueTypeString, sCellName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);                                                       
   keyword = sDoc->IsKeyWord(SCH_ATT_CLUSTERNAME, 0);
   sDoc->SetAttrib(&sCurrentBlock->getAttributesRef(), keyword, valueTypeString, sClusterName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);                                                       

   if ((res = LoopCommand(schematicSymbol_lst , SIZ_SCHEMATICSYMBOL_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);

   delete attribMap;
   attribMap = NULL;
   
   DataStruct *data = Graph_PolyStruct(sSymbolGrpLayer, 0L, FALSE);
   delete data->getPolyList();
   data->getPolyList() = sPolyList;
   Graph_Block_Off();

   sPolyList = NULL;
   sCurrentBlock = NULL;

	cluster->setCurrentSymbol(NULL);

   return 1;
}

/******************************************************************************
* schematicSymbolHeader
   - defines the measurement unit and properties for the schematic geometry
   - currenly we only care about the distance and angle unit
*/
static int schematicSymbolHeader()
{
   return LoopCommand(schematicSymbolHeader_lst, SIZ_SCHEMATICSYMBOLHEADER_LST);
}

/******************************************************************************
* cellNameDisplay
   - Display the cell name
*/
static int cellNameDisplay()
{
   return DoStringAttributeDisplay(SCH_ATT_CELLNAME);
}

/******************************************************************************
* instanceNameDisplay
   - Display the instance name which is given by "nameInformation" callout
     so the attribute name is "DisplayName"
*/
static int instanceNameDisplay()
{
   return DoStringAttributeDisplay(SCH_ATT_DISPLAYNAME);
}

/******************************************************************************
* figure
   - defines a CPoly for a certain geometry
   - if the sPolyList is not create, then create it
   - add the CPoly to the sPolyList
*/
static int figure()
{
   int res;
   if (!GetToken())
      return ParsingErr();

   if (sToken[0] != '(')
   {
      CString figureGroupRef = sToken;
      figureGroupRef.MakeUpper();
      if ((res = SetCurrentFigureGroup(figureGroupRef)) < 0)
         return res;
   }
   else
      PushToken();

   if ((res =  LoopCommand(figure_lst, SIZ_FIGURE_LST)) < 0)
      return res;

   return 1;
}

/******************************************************************************
* CreatePoly
   - Anywhere that call this function MUST call AddPoly() at the same place afterward
*/
static void CreatePoly()
{
   sPoly = new CPoly();
   sPoly->setWidthIndex(0);
   sPoly->setFilled(false);
   sPoly->setVoid(false);
   sPoly->setClosed(false);
   sPoly->setThermalLine(false);
   sPoly->setFloodBoundary(false);
   sPoly->setHidden(false);
   sPoly->setHatchLine(false);
}

/******************************************************************************
* AddPoly
   - Anywhere that call this function MUST call CreatePoly() FISRT
*/
static void AddPoly()
{
   sPoly->setWidthIndex(sFigureGroup.GetWidthIndex());
   if (sPolyList == NULL)
      sPolyList = new CPolyList();

   if (sPoly->getPntList().GetCount() == 0)
   {
      delete sPoly;
   }
   else
   {
      sPolyList->AddTail(sPoly);
      sPoly = NULL;
   }
}

/******************************************************************************
* figureGroupOverride
*/
static int figureGroupOverride()
{
   sTextHeight = 0;
   sFont = NULL;

   CString figureGroupName;
   int res;

   if ((res = GetString(figureGroupName)) < 0)
      return res;

   if ((res = SetCurrentFigureGroup(figureGroupName)) < 0)
      return res;

   if ((res = LoopCommand(figureGroup_lst, SIZ_FIGUREGROUP_LST)) < 0)
      return res;

   if (sFont)
   {
      sFigureGroup.SetFontHeight(sFont->fontHeight);
      sFigureGroup.SetFontWidth(sFont->fontWidth);
      sFont = NULL;
   }

   if (sTextHeight > 0)
   {
      if (sFigureGroup.GetFontHeight() > 0)
      {
         //Edif use textheight to scale the text to the desire height
         double textScale = sTextHeight / sFigureGroup.GetFontHeight();

         sFigureGroup.SetFontHeight(sFigureGroup.GetFontHeight() * textScale);
         sFigureGroup.SetFontWidth(sFigureGroup.GetFontWidth() * textScale);
      }
      else
      {
         // There was no font width so use the text height as font height
         sFigureGroup.SetFontHeight(sTextHeight);
         sFigureGroup.SetFontWidth(sTextHeight * 4/7);  // width will be 4/7 of font height
      }
   }

   if (sFontScale > 0)
   {
      sFigureGroup.SetFontHeight(sFigureGroup.GetFontHeight() * sFontScale);
      sFigureGroup.SetFontWidth(sFigureGroup.GetFontWidth() * sFontScale);
   }

   return 1;
}

/******************************************************************************
* complexGeometry
   - place a geometryMacro, which is a set of pre-defined graphical entitie
     into the current geometry
*/
static int complexGeometry()
{                
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   int res = LoopCommand(complexGeometry_lst, SIZ_COMPLEXGEOMETRY_LST);
   if (res < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("geomMacro_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("geomMacro_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, "", INSERTTYPE_UNKNOWN);

   delete transform;
	transform = NULL;

   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

   return 1;
}

/******************************************************************************
* InsertGlobalGeometry
   - Insert a geometry to the current geometry
   - If the scaleX and scaleY are differenct, then copy the inserting geometry and 
     manually scale the X and Y on the copy one and insert it
*/
static DataStruct *InsertGlobalGeometry(CString blockName, CTransform transform, CString refdes, int insertType)
{
   if (transform.GetOrigin() == NULL)
   {
      ErrorMessage("Trying to insert a port, a connector, or a terminator without X and Y location", "Syntax Error");
      return NULL;
   }

   CString insertBlockName = blockName;

   int mirror = 0;
   double rotation = transform.GetRotation();
   if (transform.GetScaleX() < 0)
      mirror ^= MIRROR_FLIP;
   if (transform.GetScaleY() < 0)  
   {
      mirror ^= MIRROR_FLIP;
      rotation += DegToRad(180);
   }

   double scale;
   if (fabs(transform.GetScaleX()) / fabs(transform.GetScaleY()) == 1) // if scaleX and scaleY are the same, then just use either one
      scale = fabs(transform.GetScaleX());
   else // scaleX and scaleY are difference, default it to 1
      scale = 1;


   DataStruct *data;
   insertBlockName.MakeUpper();
   BlockStruct *insertBlock = sDoc->Find_Block_by_Name(insertBlockName, sFileStruct->getFileNumber());
   if (!insertBlock)
      return NULL;

   insertBlockName.MakeUpper();
   if (insertBlock->getBlockType() == BLOCKTYPE_GATEPORT)
   {
      data = Graph_Block_Reference(insertBlockName, refdes, sFileStruct->getFileNumber(), transform.GetOrigin()->x, transform.GetOrigin()->y,
                  rotation, mirror, scale, sPortGrpLayer, TRUE);
   }
   else
   {
      data = Graph_Block_Reference(insertBlockName, refdes, sFileStruct->getFileNumber(), transform.GetOrigin()->x, transform.GetOrigin()->y,
                  rotation, mirror, scale, sSymbolGrpLayer, TRUE);
   }
   data->getInsert()->setInsertType(insertType);

   return data;
}



/******************************************************************************
* annotate
   - get the text and display location
   - insert the text to the current geometry if there is one, else insert to the file
*/
static int annotate()
{
	CDisplay* display = new CDisplay();
	sDisplayList.AddTail(display);

	CString stringValue;
   int res;

   if ((res = GetString(stringValue)) < 0)
   {
		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
		return res;
   }

   if ((res = LoopCommand(getDisplay_lst, SIZ_GETDISPLAY_LST)) < 0)
   {
		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
      return res;
   }

	display = sDisplayList.RemoveTail();
	if (sCurrentBlock != NULL)
		res = GraphTextDisplays(display, sTextLayer, stringValue);
	delete display;
	display = NULL;

   return res;
}

/******************************************************************************
* display
   - defines the current figure group and transform
*/
static int display()
{
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   if (!GetToken())
      return ParsingErr();

   int res;

   if (sToken[0] == '(')
   {
      PushToken();
   }
   else
   {
      CString figureGroupRef = sToken;
      figureGroupRef.MakeUpper();
      if ((res = SetCurrentFigureGroup(figureGroupRef)) < 0)
         return ParsingErr();
   }

   res = LoopCommand(display_lst, SIZ_DISPLAY_LST);

	CDisplay* display = sDisplayList.GetTail();
	if (display->GetTransform() != NULL)
	{
		// There is already transform in the display, which only mean this is an addition display
		// so create a new display and add to link list
		CDisplay* newDisplay = new CDisplay();
		display->SetNextDisplay(newDisplay);
		display = newDisplay;
	}

   transform = sTransformList.RemoveTail();
	display->SetFigureGroup(new CFigureGroup(sFigureGroup));
	display->SetTransform(transform);

   return res;
}

/******************************************************************************
* SetCurrentFigureGroup
   - Find the figure group reference and copy it to the global sFigureGroup
*/
static int SetCurrentFigureGroup(CString figureGroupRef)
{
   CFigureGroup *figureGroup = NULL;
   if (sFigureGroupMap.Lookup(figureGroupRef, figureGroup))
   {
		sFigureGroup = CFigureGroup(*figureGroup);
      return 1;
   }

   return -1;
}

/******************************************************************************
* transform
   - Gives the lotion of an implementation(insert)
   - Each transform is applied in the following order: scaleX, scaleY, rotation and finally translation (origin).
   - The default scaling is 1 for scaleX and scaleY. Negative values indicate mirroring
   - The default rotation is zero radians. Rotation angle is measured in units defined by setAngle. 
   - The default origin is (pt 0 0). It is the translation point of an instance.  
*/
static int transform()
{
   CTransform *transform = sTransformList.GetTail();
	transform->Reset();
   return LoopCommand(transform_lst, SIZ_TRANSFORM_LST);
}

/******************************************************************************
* scaleX
   - get the X coordinate for transform, it is represented by (scaleX num1 num2) 
   - num1 is the numerator, num2 is the denominator
   - The default scaling is 1. Negative values indicate mirroring
*/
static int scaleX()
{
   int res;
   int x = 0;
   if ((res = GetIntegerToken(x)) < 0)
      return res;

   int y = 0;
   if ((res = GetIntegerToken(y)) < 0)
      return res;


   // For some system there X & Y scale are swap, so if sExchangeXYScale TRUE then swap them 
   CTransform *transform = sTransformList.GetTail();
   if (sExchangeXYScale || sEdifProgram == edifProgramC2esch)   
      transform->SetScaleY(((double)x / (double)y));
   else
      transform->SetScaleX(((double)x / (double)y));

   return 1;
}

/******************************************************************************
* scaleY
   - get the Y coordinate for transform, it is represented by (scaleX num1 num2) 
   - num1 is the numerator, num2 is the denominator
   - The default scaling is 1. Negative values indicate mirroring
*/
static int scaleY()
{
   int res;
   int x = 0;
   if ((res = GetIntegerToken(x)) < 0)
      return res;

   int y = 0;
   if ((res = GetIntegerToken(y)) < 0)
      return res;

   // For some system there X & Y scale are swap, so if sExchangeXYScale TRUE then swap them 
   CTransform *transform = sTransformList.GetTail();
   if (sExchangeXYScale || sEdifProgram == edifProgramC2esch)
      transform->SetScaleX(((double)x / (double)y));
   else
      transform->SetScaleY(((double)x / (double)y));

   return 1;
}

/******************************************************************************
* rotation
   - get the rotation for transform
   - The default rotation is zero radians. Rotation angle is measured in units defined by setAngle. 
*/
static int rotation()
{
   CTransform *transform = sTransformList.GetTail();
   int res;
	double rotation;
   if (( res = GetScaleInteger(rotation)) < 0)
      return res;

   transform->SetRotation(ConvertUnit(sAngleUnitRef, rotation, NULL));
   return 1;
}

/******************************************************************************
* origin
   - The default origin is (pt 0 0). It is the translation point of an instance.  
*/
static int origin()
{
   CTransform *transform = sTransformList.GetTail();
   int res;
	CPnt* pnt = new CPnt();
   if ((res = GetPoint(pnt, TRUE)) < 1)
      return -1;

	transform->SetOrigin(*pnt);
	delete pnt;
	pnt = NULL;
   return 1;
}

/******************************************************************************
* portNameDisplay
   - get the display location for port name
*/
static int portNameDisplay()
{
	// Case 1430, do not display the port name, display the primary name
	// return DoStringAttributeDisplay(SCH_ATT_PORTNAME);
	return DoStringAttributeDisplay(SCH_ATT_PRIMARYNAME);
}

/******************************************************************************
* designatorDisplay
   - get the display location for designator
*/
static int designatorDisplay()
{
   return DoStringAttributeDisplay(SCH_ATT_DESIGNATOR);
}

/******************************************************************************
* DoStringAttributeValue
*/
static int DoStringAttributeValue(CString keyword, CString value)
{
   CAttributes* attribMap = sAttribMapList.GetTail();
   int keywordIndex = sDoc->RegisterKeyWord(keyword, 0, valueTypeString);
   Attrib* attrib;

   if (attribMap->Lookup(keywordIndex, attrib))
   {
      attrib->setStringValueIndex(sDoc->RegisterValue(value));
   }
   else 
   {
      sDoc->SetAttrib(&attribMap, keywordIndex, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   }

   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   return 1;
}

/******************************************************************************
* DoStringAttributeDisplay
*/
static int DoStringAttributeDisplay(CString keyword)
{
	CDisplay* display = new CDisplay();
	sDisplayList.AddTail(display);
   sRemoveDisplay = FALSE;

   int res = LoopCommand(getDisplay_lst, SIZ_GETDISPLAY_LST);
   if (res < 0)
   {
		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
      return res;
   }

   int keywordIndex = sDoc->RegisterKeyWord(keyword, 0, valueTypeString);

   CAttributes* attribMap = sAttribMapList.GetTail();
   Attrib* attrib;

   if (!attribMap->Lookup(keywordIndex, attrib))
      sDoc->SetAttrib(&attribMap, keywordIndex, valueTypeString, "", attributeUpdateOverwrite, &attrib);

	display = sDisplayList.RemoveTail();
	res = AddAttributeDisplays(attrib, display, keyword);

	delete display;
	display = NULL;

   return res;
}

/******************************************************************************
* nameDisplay
   - get the location of the display name and add it to the attribute
*/
static int nameDisplay()
{
	CDisplay* display = new CDisplay();
	sDisplayList.AddTail(display);
   sRemoveDisplay = FALSE;

   int res = LoopCommand(getNameDisplay_lst, SIZ_GETNAMEDISPLAY_LST);
   if (res < 0)
   {
		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
      return res;
   }

   CAttributes* attribMap = sAttribMapList.GetTail();
   int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   Attrib* attrib;

   if (!attribMap->Lookup(keyword, attrib))
      sDoc->SetAttrib(&attribMap, keyword, valueTypeString, "", attributeUpdateOverwrite, &attrib);

	display = sDisplayList.RemoveTail();
	res = AddAttributeDisplays(attrib, display, SCH_ATT_DISPLAYNAME);

	delete display;
	display = NULL;

   return res;
}

/******************************************************************************
* addDisplay
*/
static int addDisplay()
{
   if (sOverWriteAttributeDisplay)
   {
      return LoopCommand(getDisplay_lst, SIZ_GETDISPLAY_LST);
   }
   else
   {
      skipCommand();
      return 1;
   }
}

/******************************************************************************
* replaceDisplay
*/
static int replaceDisplay()
{
   return LoopCommand(getDisplay_lst, SIZ_GETDISPLAY_LST);
}

/******************************************************************************
* removeDisplay
   - indicate that the property is remove from display
*/
static int removeDisplay()
{
   sRemoveDisplay = TRUE;
   return 1;
}

/******************************************************************************
* displayNameOverride
*/
static int displayNameOverride()
{
   CString displayName;
   int res;

   if ((res = GetString(displayName)) < 0)
      return res;

   if ((res = DoStringAttributeValue(SCH_ATT_DISPLAYNAME, displayName)) < 0)
      return res;

   return LoopCommand(displayNameOverride_lst, SIZ_DISPLAYNAMEOVERRIDE_LST);
}

/******************************************************************************
* schematicComplexFigure
   - reference and position a schematicFigureMacro which is definition of figures and annotation
*/
static int schematicComplexFigure()
{
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   int res = LoopCommand(schematicComplexFigure_lst, SIZ_SCHEMATICCOMPLEXFIGURE_LST);
   if (res < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("figMacro_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("figMacro_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, "", INSERTTYPE_UNKNOWN);
   delete transform;
   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* schematicSymbolPortImplementation
   - insert a port
*/
static int schematicSymbolPortImplementation()
{
   CString refdes;
   int res;

   if ((res = GetString(refdes)) < 0)
      return res;

	CEdifSymbol* symbol = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentSymbol();
	CSymbolPort* symbolPort = symbol->addPort(refdes);

   sPortRefArr.RemoveAll();
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicSymbolPortImplementation_lst, SIZ_SCHEMATICSYMBOLPORTIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString portRef;

   if (sPortRefArr.GetSize() != 0)
   {
      portRef = sPortRefArr.GetAt(0); // there should only be one port ref
      sPortRefArr.RemoveAll();
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("symbolPort_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("symbolPort_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, refdes, INSERTTYPE_PORTINSTANCE);
   delete transform;

   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

	symbolPort->setPortRef(portRef);
	symbolPort->setPortDataStruct(data);

   // overwrite the block to BLOCKTYPE_SYMBOL since only schematic symbol with port should be BLOCKTYPE_SYMBOL
	BlockStruct *block = symbol->getSymbolBlock();
   block->setBlockType(blockTypeSymbol);

   CEdifPort* port = NULL;
   if (sPortMap.Lookup(portRef, port))
   {
      // find the port reference and copy the attributes
      CopyANDRemoveAttribMap(&data->getAttributesRef(), port->attribMap);
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;
   return 1;
}

/******************************************************************************
* schematicSymbolBorder
*/
static int schematicSymbolBorder()
{
   return LoopCommand(schematicSymbolBorder_lst, SIZ_SCHEMATICSYMBOLBORDER_LST);
}

/******************************************************************************
* portAttributeDisplay
   - get the property display location for port property
*/
static int portAttributeDisplay()
{
   return LoopCommand(portAttributeDisplay_lst, SIZ_PORTATTRIBUTEDISPLAY_LST);
}

/******************************************************************************
* input
*/
static int input()
{
   return DoStringAttributeValue("PortDirection", "input");
}

/******************************************************************************
* output
*/
static int output()
{
   return DoStringAttributeValue("PortDirection", "output");
}

/******************************************************************************
* bidirectional
*/
static int bidirectional()
{
   return DoStringAttributeValue("PortDirection", "bidirectional");
}

/******************************************************************************
* unspecified
*/
static int unspecified()
{
   return DoStringAttributeValue("PortDirection", "unspecified");
}

/******************************************************************************
* unrestricted
*/
static int unrestricted()
{
   return DoStringAttributeValue("PortDirection", "unrestricted");
}

/******************************************************************************
* mixedDirection
*/
static int mixedDirection()
{
   return DoStringAttributeValue("PortDirection", "mixedDirection");
}

/******************************************************************************
* propertyDisplay
*/
static int propertyDisplay()
{
	CDisplay* display = new CDisplay();
	sDisplayList.AddTail(display);
   sRemoveDisplay = FALSE;

   CString propertyName;
   int res;

   if ((res = GetString(propertyName)) < 0)
   {
		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
      return res;
   }
   
   if ((res = LoopCommand(getDisplay_lst, SIZ_GETDISPLAY_LST)) < 0)
   {
		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
      return res;
   }

   int keywordIndex = -1;
   CString ccKeyword;
   if (sEdifKeywordMap.Lookup(propertyName, ccKeyword))
   {
      keywordIndex = sDoc->IsKeyWord(ccKeyword, 0);
      if (keywordIndex < 0)
      {
         fprintf(sFileErr, "Syntax Error : Referencing property \"%s\" that is not previously defined.  Error is close to line %ld\n", propertyName, sFileInLineCnt);
         sDisplayErr++;
       
			display = sDisplayList.RemoveTail();
			delete display;
			display = NULL;
			return -1;
      }
   }
   else
   {
      fprintf(sFileErr, "Syntax Error : Referencing property \"%s\" that is not previously defined.  Error is close to line %ld\n", propertyName, sFileInLineCnt);
      sDisplayErr++;

		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
      return -1;
   }

   CAttributes* attribMap = sAttribMapList.GetTail();
   Attrib* attrib;

   if (!attribMap->Lookup(keywordIndex, attrib))
   {
      int type = sDoc->getKeyWordArray()[keywordIndex]->getValueType();

      if (type == valueTypeString)
         sDoc->SetAttrib(&attribMap, keywordIndex, type, "", attributeUpdateOverwrite, &attrib);
      else if (type == VT_INTEGER)
      {
         int value = 0;
         sDoc->SetAttrib(&attribMap, keywordIndex, type, &value, attributeUpdateOverwrite, &attrib);
      }
      else if (type == VT_DOUBLE)
      {
         double value = 0.0;
         sDoc->SetAttrib(&attribMap, keywordIndex, type, &value, attributeUpdateOverwrite, &attrib);
      }     
   }

	display = sDisplayList.RemoveTail();
	res = AddAttributeDisplays(attrib, display, ccKeyword);

	delete display;
	display = NULL;

   return 1;
}

/******************************************************************************
* schematicView
   - start create schematic board
   - currently the property of schematic view is skipped
*/
static int schematicView()
{
   int res;
   if ((res = GetString(sViewName)) < 0)
      return res;

	CEdifCluster* cluster = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster();
	CEdifView* view = cluster->addView(sViewName);
	cluster->setCurrentView(view);

   if ((res = LoopCommand(schematicView_lst, SIZ_SCHEMATICVIEW_LST)) < 0)
      return res;

   view->applyRipperRulesToHotspot();
	cluster->setCurrentView(NULL);

   return 1;
}

/******************************************************************************
* schematicViewHeader
   - defines the measurement unit for schematic view
   - currently we only care about the distance and angle units
   - the property of schematic view is skipped
*/
static int schematicViewHeader()
{
   return LoopCommand(schematicViewHeader_lst, SIZ_SCHEMATICVIEWHEADER_LST);
}

/******************************************************************************
* logicalConnectivity
   - defines all the instance and signal in a view
*/
static int logicalConnectivity()
{
   return LoopCommand(logicalConnectivity_lst, SIZ_LOGICALCONNECTIVITY_LST);
}

/******************************************************************************
* instance
   - defines which cluster of a cell to use for insert later
   - also provides properties
*/
static int instance()
{
   sLibraryRef.Empty();
   sClusterRef.Empty();
   sObjectRef.Empty();  // cell name
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   sInstanceRef.Empty();
   int res;

   if ((res = GetString(sInstanceRef)) < 0)
      return res;

   // By default put the instance name as the designator, it can be overwrite later with a really desginator
   CString designator;
   sDesList.AddTail(designator);

	CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();

   if (view->findInstance(sInstanceRef) != NULL)
   {
      CString msg;
      msg.Format("Duplicate instance \"%s\" found within the same view at line %d", sInstanceRef, sFileInLineCnt);
      ErrorMessage(msg, "Syntax Error!",  MB_OK | MB_ICONHAND);
      return -1;
   }

   // create new instance here so it can be use later in instancePortAttributes
	CInstance* instance = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->addInstance(sInstanceRef);
	instance->getAttributesRef() = NULL;

   if ((res = LoopCommand(instance_lst, SIZ_INSTANCE_LST)) < 0)
      return res;

   if (sObjectRef == "")
   {
      CString msg;
      msg.Format("Missing cellRef in cluster Ref of instance \"%s\"\n", sInstanceRef);
      ErrorMessage(msg, "Syntax Error!", MB_OK | MB_ICONHAND);

      fprintf(sFileErr, "Syntax Error : Missing cellRef in cluster Ref of instance \"%s\"\n", sInstanceRef);
      sDisplayErr++;
      return  -1;
   }

   attribMap = sAttribMapList.RemoveTail();
   designator = sDesList.RemoveTail(); // get & remove the designator from stack

   if (!designator.IsEmpty())
   {
      Attrib* attrib;
      int keyword = sDoc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
      sDoc->SetAttrib(&attribMap, keyword, valueTypeString, designator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
      attrib->setFlagBits(ATTRIB_CHANGE_VALUE);
   }

   CopyANDRemoveAttribMap(&instance->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

	instance->setLibraryRef(sLibraryRef);
	instance->setCellRef(sObjectRef);
	instance->setClusterRef(sClusterRef);
   if (!designator.IsEmpty())
	   instance->setDesignator(designator);
   else
      instance->setDesignator(sInstanceRef);

   return 1;
}

/******************************************************************************
* clusterRef
*/
static int clusterRef()
{
   int res;
   if ((res = GetString(sClusterRef)) < 0)
      return res;

   return LoopCommand(clusterRef_lst, SIZ_CLUSTERREF_LST);
}

/******************************************************************************
* getObjectRef
   - get the reference of the current object and the library
*/
static int getObjectRef()
{
   int res;
   if ((res = GetString(sObjectRef)) < 0)
      return res;

   sLibraryRef.Empty();
   return LoopCommand(getObjectRef_lst, SIZ_GETOBJECTREF_LST);
}

/******************************************************************************
* libraryRef
   - get the reference of the current library
*/
static int libraryRef()
{
   return GetString(sLibraryRef);
}

/******************************************************************************
* instancePortAttributes
   - Attach or modify attributes of a port within an instance
   - instancePortRef is referencing to the port instance not the master port
   - since instancePortAttributes is only call from the instance construct
     so it is okay to add it to the current instance
   - the attrib map only contain attributes keyword and value but not location,
     the location will be given in instancePortAttributeDisplay
*/
static int instancePortAttributes()
{
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   CString designator;
   sDesList.AddTail(designator);
      
   CString instancePortRef;
   int res;
   if ((res = GetString(instancePortRef)) < 0)
      return res;

   if ((res = LoopCommand(instancePortAttributes_lst, SIZ_INSTANCEPORTATTRIBUTES_LST)) < 0)
      return res;

	CInstance* instance = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->findInstance(sInstanceRef);
	if (instance == NULL)
	{
      fprintf(sFileErr, "Syntax Error : Referencing object \"%s\" that is not previously defined.  Error is at line %ld\n", sInstanceRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
	}
	CInstancePort* instancePort = instance->addInstancePort(instancePortRef);

   attribMap = sAttribMapList.RemoveTail();
   designator = sDesList.RemoveTail(); // get & remove the designator from stack
   if (!designator.IsEmpty())
   {
      Attrib* attrib;
      int keyword = sDoc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
      sDoc->SetAttrib(&attribMap, keyword, valueTypeString, designator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
      attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

		instancePort->setDesignator(designator);
   }


	void* voidPtr;
   if (instance->portAttribMap.Lookup(instancePortRef, voidPtr))
   {
      CAttributes* tmpAttribMap = (CAttributes*)voidPtr;
      delete tmpAttribMap;
   }
   instance->portAttribMap.SetAt(instancePortRef, attribMap);

   return 1;
}

/******************************************************************************
* signal
   - defines the properties and ports that is connected to this signal.
   - the graphical representation of net is given in schematicNet
*/
static int signal()
{
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   CString signalName;
   int res;
   if ((res = GetString(signalName)) < 0)
      return res;

	sPortRefArr.RemoveAll();

	CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();
	CNet* viewNet = view->addNet(signalName);
	view->setCurrentNet(viewNet);

	if ((res = LoopCommand(signal_lst, SIZ_SIGNAL_LST)) < 0)
      return res;

	if (sPortRefArr.GetCount() > 0)
	{
		sPortRefArr.RemoveAll();
	}

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&viewNet->getAttributesRef(), attribMap);

   delete attribMap;
   attribMap = NULL;

	view->setCurrentNet(NULL);
   return 1;               
}

/******************************************************************************
* portPropertyOverride
   - override the value of a property belonging to a port 
*/
static int portPropertyOverride()
{
   CString propertyNameRef;
   int res;

   if ((res = GetString(propertyNameRef)) < 0)
      return res;

   return LoopCommand(property_lst, SIZ_PROPERTY_LST);   
}

/******************************************************************************
* signalJoin
*/
static int signalJoined()
{
   return LoopCommand(signalJoined_lst, SIZ_SIGNALJOINED_LST);
}

/******************************************************************************
* portInstanceRef
   - referencing a port of a certain instance
   - need to get port & instance names refereced
*/
static int portInstanceRef()
{
   CString portNameRef;
   int res;

   if ((res = GetString(portNameRef)) < 0)
      return res;

	// Reset instanceRef before geting signal joint just in case where there are only reference to port and no instance
	sInstanceRef.Empty();

   if ((res = LoopCommand(portInstanceRef_lst, SIZ_PORTINSTANCEREF_LST)) < 0)
      return res;

   if (sInstanceRef == "")
   {
      // Since portRef can be repeat on different view, so need to make the portRef unique 
      CString tmpPortNameRef;
      tmpPortNameRef.Format("%s[%s%c%s%c%s%c%s]", portNameRef, sLibraryName, SCH_DELIMITER, sCellName, SCH_DELIMITER, sClusterName,
            SCH_DELIMITER, sViewName);
   }
   else
   {
      CNet* net = edifStruct.getCurrentNet();
      if (net != NULL)
      {
         CEdifView* view = edifStruct.getCurrentView();
         if (view != NULL)
         {
            CInstance* instance = view->findInstance(sInstanceRef);
            if (instance != NULL)
            {
               CInstancePort* instancePort = instance->addInstancePort(portNameRef);
               if (instancePort != NULL)
               {
                  instancePort->setConnectedToSignal(net);
                  net->addInstancePort(instancePort);
               }
            }
         }
      }
   }


   return 1;
}

/******************************************************************************
* instanceRef
   - get the instance reference
*/
static int instanceRef()
{
   int res;
   if ((res = GetString(sInstanceRef)) < 0)
      return res;

	CInstance* instance = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->findInstance(sInstanceRef);
   if (instance == NULL)
   {
      fprintf(sFileErr, "Syntax Error : Referencing object \"%s\" that is not previously defined.  Error is at line %ld\n", sInstanceRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

   return res;
}

/******************************************************************************
* signalGroup
*/
static int signalGroup()
{
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

	CString signalGroupName;
   int res;

   if ((res = GetString(signalGroupName)) < 0)
      return res;

	CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();
	CBus* viewBus = view->addBus(signalGroupName);
	view->setCurentBus(viewBus);

   if ((res = LoopCommand(signalGroup_lst, SIZ_SIGNALGROUP_LST)) < 0)
      return res;

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&viewBus->getAttributesRef(), attribMap);

	delete attribMap;
   attribMap = NULL;

	view->setCurentBus(NULL);

   return 1;
}

/******************************************************************************
* signalList
*/
static int signalList()
{
   return LoopCommand(signalList_lst, SIZ_SIGNALLIST_LST);
}


/******************************************************************************
* schematicImplementation
   - create the schematic sheets
*/
static int schematicImplementation()
{
   return LoopCommand(schematicImplementation_lst, SIZ_SCHEMATICIMPLEMENTATION_LST);
}

/******************************************************************************
* totalPages
   - get the number of pages
*/
static int totalPages()
{
   int totalCnt;
   return GetIntegerToken(totalCnt);
}

/******************************************************************************
* page
   - defines individual schematic sheet
*/
static int page()
{
	sPageName.Empty();
	int res;

   if ((res = GetString(sPageName)) < 0)
      return res;


	CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();


   // Create a geometry to represent a sheet
   sPageName = view->getUniquePageName(sPageName);
   CString blockName = view->generatePageBlockName(sPageName);
   sCurrentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L);
   sCurrentBlock->setBlockType(blockTypeSheet); 
	view->addPage(sCurrentBlock);
	view->setCurrentPage(sCurrentBlock, sPageName);

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(page_lst, SIZ_PAGE_LST)) < 0)
      return res;

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&sCurrentBlock->getAttributesRef(), attribMap);
   int keyword = sDoc->RegisterKeyWord("SchematicStructure", 0, VT_INTEGER);
   int value = 1;
   sDoc->SetAttrib(&sCurrentBlock->getAttributesRef(), keyword, VT_INTEGER, &value, attributeUpdateOverwrite, NULL);
   delete attribMap;
   attribMap = NULL;

   if (sFileStruct->getBlock() == NULL )
      sFileStruct->setBlock(sCurrentBlock);   // Only set to sCurrentBlock if not already set, it will be override late in SchematicInstanceImplementation()

   Graph_Block_Off();
   sCurrentBlock = NULL;

	view->setCurrentPage(NULL, "");

   return 1;
}

/******************************************************************************
* pageHeader
   - defines the page border and properties
*/
static int pageHeader()
{
   return LoopCommand(pageHeader_lst, SIZ_PAGEHEADER_LST);
}

/******************************************************************************
* pageBorder     
   - insert the page border geometry
*/
static int pageBorder()
{
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   int res = LoopCommand(pageBorder_lst, SIZ_PAGEBORDER_LST);
   if (res < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("pageBorder_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("pageBorder_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, "", INSERTTYPE_UNKNOWN);
   delete transform;
   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* pageCommentGraphics
   - defines comment figures and text for the page
*/
static int pageCommentGraphics()
{
   sPolyList = new CPolyList();

   int res = LoopCommand(pageCommentGraphics_lst, SIZ_PAGECOMMENTGRAPHICS_LST);
   if (res < 0)
   {
      FreePolyList(sPolyList);
      return res;
   }

   DataStruct *data = Graph_PolyStruct(sBorderGrpLayer, 0L, FALSE);
   delete data->getPolyList();
   data->getPolyList() = sPolyList;
   sPolyList = NULL;

   return 1;
}

/******************************************************************************
* pageTitleBlock
*/
static int pageTitleBlock()
{
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   CString implementationName;
   int res;

   if ((res = GetString(implementationName)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   if ((res = LoopCommand(pageTitleBlock_lst, SIZ_PAGETITLEBLOCK_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("pageTitle_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("pageTitle_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, "", INSERTTYPE_UNKNOWN);
   delete transform;

   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* pageTitleBlockAttributeDisplay
*/
static int pageTitleBlockAttributeDisplay()
{
   return LoopCommand(pageTitleBlockAttributeDisplay_lst, SIZ_PAGETITLEBLOCKATTRIBUTEDISPLAY_LST);
}

/******************************************************************************
* approvedDateDisplay
*/
static int approvedDateDisplay()
{ 
   return DoStringAttributeDisplay("ApprovedDate");
}

/******************************************************************************
* checkDateDisplay
*/
static int checkDateDisplay()
{
   return DoStringAttributeDisplay("CheckDate");
}

/******************************************************************************
* companyNameDisplay
*/
static int companyNameDisplay()
{
   return DoStringAttributeDisplay("CompanyName");
}

/******************************************************************************
* contractDisplay
*/
static int contractDisplay()
{
   return DoStringAttributeDisplay("Contract");
}

/******************************************************************************
* drawingDescriptionDisplay
*/
static int drawingDescriptionDisplay()
{
   return DoStringAttributeDisplay("DrawingDescription");
}

/******************************************************************************
* drawingIdentificationDisplay
*/
static int drawingIdentificationDisplay()
{
   return DoStringAttributeDisplay("DrawingID");
}

/******************************************************************************
* drawingSizeDisplay
*/
static int drawingSizeDisplay()
{
   return DoStringAttributeDisplay("DrawingSize");
}

/******************************************************************************
* engineeringDateDisplay
*/
static int engineeringDateDisplay()
{
   return DoStringAttributeDisplay("EngineeringDate");
}

/******************************************************************************
* originalDrawingDateDisplay
*/
static int originalDrawingDateDisplay()
{
   return DoStringAttributeDisplay("OriginalDrawingDate");
}

/******************************************************************************
* pageIdentificationDisplay
*/
static int pageIdentificationDisplay()
{
   return DoStringAttributeDisplay("PageID");
}

/******************************************************************************
* pageTitleDisplay
*/
static int pageTitleDisplay()
{
   return DoStringAttributeDisplay("PageTitle");
}

/******************************************************************************
* revisionDisplay
*/
static int revisionDisplay()
{
   return DoStringAttributeDisplay("Revision");
}

/******************************************************************************
* totalPageDisplay
*/
static int totalPageDisplay()
{
   return DoStringAttributeDisplay("TotalPage");
}

/******************************************************************************
* pageTitleBlockAttributes
*/
static int pageTitleBlockAttributes()
{
   return LoopCommand(pageTitleBlockAttributes_lst, SIZ_PAGETITLEBLOCKATTRIBUTES_LST);
}

/******************************************************************************
* approvedDate
*/
static int approvedDate()
{
   int year = 0;
   int month = 0;
   int date = 0;
   int res = GetDate(year, month, date);  
   if (res < 0)
      return res;

   CString value;
   value.Format("%i/%i/%i", month, date, year);
   return DoStringAttributeValue("ApprovedDate", value);
}

/******************************************************************************
* checkDate
*/
static int checkDate()
{
   int year = 0;
   int month = 0;
   int date = 0;
   int res = GetDate(year, month, date);  
   if (res < 0)
      return res;

   CString value;
   value.Format("%i/%i/%i", month, date, year);
   return DoStringAttributeValue("CheckDate", value);
}

/******************************************************************************
* companyName
*/
static int companyName()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("CompanyName", stringValue);
}

/******************************************************************************
* contract
*/
static int contract()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("Contract", stringValue);
}

/******************************************************************************
* drawingDescription
*/
static int drawingDescription()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("DrawingDescription", stringValue);
}

/******************************************************************************
* drawingIdentification
*/
static int drawingIdentification()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("DrawingID", stringValue);
}

/******************************************************************************
* drawingSize
*/
static int drawingSize()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("DrawingSize", stringValue);
}

/******************************************************************************
* engineeringDate
*/
static int engineeringDate()
{
   int year = 0;
   int month = 0;
   int date = 0;
   int res = GetDate(year, month, date);  
   if (res < 0)
      return res;

   CString value;
   value.Format("%i/%i/%i", month, date, year);
   return DoStringAttributeValue("EngineeringDate", value);
}

/******************************************************************************
* originalDrawingDate
*/
static int originalDrawingDate()
{
   int year = 0;
   int month = 0;
   int date = 0;
   int res = GetDate(year, month, date);  
   if (res < 0)
      return res;

   CString value;
   value.Format("%i/%i/%i", month, date, year);
   return DoStringAttributeValue("OriginalDrawingDate", value);
}

/******************************************************************************
* pageIdentification
*/
static int pageIdentification()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("PageID", stringValue);
}

/******************************************************************************
* pageTitle
*/
static int pageTitle()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("PageTitle", stringValue);
}

/******************************************************************************
* revision
*/
static int revision()
{
   CString stringValue;
   int res = GetString(stringValue);
   if (res < 0)
      return res;

   return DoStringAttributeValue("Revision", stringValue);
}

/******************************************************************************
* schematicGlobalPortImplementation
*/
static int schematicGlobalPortImplementation()
{
   CString refdes;
   int res;

   if ((res = GetString(refdes)) < 0)
      return res;

   sPortRefArr.RemoveAll();
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicGlobalPortImplementation_lst, SIZ_SCHEMATICGLOBALPORTIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("globalPort_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("globalPort_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   CString portRef;

   if (sPortRefArr.GetSize() != 0)
   {
      portRef = sPortRefArr.GetAt(0); // there should only be one port ref
      sPortRefArr.RemoveAll();
   }

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, refdes, INSERTTYPE_PORTINSTANCE);
   delete transform;
   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

	CPort* port = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->addGlobalPort(refdes);
   port->setPortDataStruct(data);
	port->setPortRef(portRef);

   CEdifPort* edifPort = NULL;
   if (sGlobalPortMap.Lookup(portRef, edifPort))
   {
      // find the port reference and copy the attributes
      CopyANDRemoveAttribMap(&data->getAttributesRef(), edifPort->attribMap);
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;
   return 1;
}

/******************************************************************************
* globalPortPropertyDisplay
*/
static int globalPortPropertyDisplay()
{
   CString propertyNameRef;
   int res;

   if ((res = GetString(propertyNameRef)) < 0)
      return res;

   return LoopCommand(globalPortPropertyDisplay_lst, SIZ_GLOBALPORTPROPERTYDISPLAY_LST);  
}

/******************************************************************************
* schematicInstanceImplementation
   - insert of an geometry
*/
static int schematicInstanceImplementation()
{
   CString implementationName;
   int res;

   if ((res = GetString(implementationName)) < 0)
      return res;

   sInstanceRef.Empty();
   sSymbolRef.Empty();
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicInstanceImplementation_lst, SIZ_SCHEMATICINSTANCEIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }
  
   CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();
   view->setFirstSchematicPage(view->getCurrentPage());

	CInstance* instance = view->findInstance(sInstanceRef);
	if (instance == NULL)
   {
      fprintf(sFileErr, "Syntax Error : Referencing object \"%s\" that is not previously defined.  Error is within the structure of SchematicInstanceImplementation \"%s\" close to line %ld\n",
              sInstanceRef, implementationName, sFileInLineCnt);
      sDisplayErr++;

      transform = sTransformList.RemoveTail();
      delete transform;
      return -1;
   }
	instance->setSymbolRef(sSymbolRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertSchematicSymbol(instance, *transform);
   delete transform;
   if (data == NULL)
      return -1;

   if (IsNotLogicSymbol(instance->getLibraryRef(), instance->getCellRef(), instance->getClusterRef(), instance->getSymbolRef()))
      data->getInsert()->setInsertType(insertTypeUnknown);

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), instance->getAttributesRef());
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   sDoc->SetUnknownAttrib(&data->getAttributesRef(), SCH_ATT_PARENTLIBRARY, edifStruct.getCurrentLibrary()->getName(),                                        attributeUpdateOverwrite, NULL);
   sDoc->SetUnknownAttrib(&data->getAttributesRef(), SCH_ATT_PARENTCELL,    edifStruct.getCurrentLibrary()->getCurrentCell()->getName(),                      attributeUpdateOverwrite, NULL);
   sDoc->SetUnknownAttrib(&data->getAttributesRef(), SCH_ATT_PARENTCLUSTER, edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getName(), attributeUpdateOverwrite, NULL);

   if (!sFileStruct->isShown())
   {
      // Overwrite what is already set in page() and make it show.
      // Only the first view that has sheet with component insert will be set to show.
      // This can be overwrite in design()
      sFileStruct->setShow(true);
      sFileStruct->setBlock(sCurrentBlock);
   }

   return 1;
}

/******************************************************************************
* IsNotLogicSymbol
*/
static BOOL IsNotLogicSymbol(CString libraryName, CString cellName, CString clusterName, CString symbolName)
{
   CString tmpString;
   CString key;

   key.Format("%s%c%s%c%s%c%s", libraryName, SCH_DELIMITER, cellName, SCH_DELIMITER, clusterName, SCH_DELIMITER, symbolName);
   if (sNotLogicSymbol.Lookup(key, tmpString))
      return TRUE;

   key.Format("%s%c%s%c%s", libraryName, SCH_DELIMITER, cellName, SCH_DELIMITER, clusterName);
   if (sNotLogicSymbol.Lookup(key, tmpString))
      return TRUE;

   key.Format("%s%c%s", libraryName, SCH_DELIMITER, cellName);
   if (sNotLogicSymbol.Lookup(key, tmpString))
      return TRUE;

   key = libraryName;
   if (sNotLogicSymbol.Lookup(key, tmpString))
      return TRUE;

   return FALSE;
}

/******************************************************************************
* InsertSchematicSymbol
*/
static DataStruct *InsertSchematicSymbol(CInstance *instance, CTransform transform)
{
   if (transform.GetOrigin() == NULL)
   {
      ErrorMessage("Trying to insert a schematic gate without X and Y location", "Syntax Error");
      return NULL;
   }

	CEdifCluster* cluster = edifStruct.findCluster(instance->getLibraryRef(), instance->getCellRef(), instance->getClusterRef());
   if (cluster == NULL)
   {
      fprintf(sFileErr, "Error : Cluster \"%s\" not found in Library [%s], Cell [%s].  Error is close to line %ld\n",
         instance->getClusterRef(), instance->getLibraryRef(), instance->getCellRef(), sFileInLineCnt);
      sDisplayErr++;
      return NULL;
   }

	CEdifSymbol* symbol = cluster->findSymbol(instance->getSymbolRef());
   if (symbol == NULL || symbol->getSymbolBlock() == NULL)
   {
      fprintf(sFileErr, "Syntax Error : instance \"%s\" reference an undefined schematic symbol \"%s\".  Error is close to line %ld\n",
            instance->getName(), instance->getSymbolBlockName(), sFileInLineCnt);
      sDisplayErr++;
      return NULL;
   }


   // If there are changes in attribute values or location on the port then copy the schematic symbol,
   // make the changes to it and insert the copied schematic symbol.
   // Do the same if scaleY of the insert is negative because we want to rotate the attributes on the port
   // so it looks right or we are duplicating the geometry and use a work around

	BlockStruct *block = symbol->getSymbolBlock();
	BlockStruct* newBlock = block;	// initialize to the same pointer as block
	
	if (instance->portAttribMap.GetCount() != 0 || instance->portAttribDisplayMap.GetCount() != 0 || transform.GetScaleY() < 0)
   {
		CEdifCluster* cluster = edifStruct.findCluster(symbol->getLibraryName(), symbol->getCellName(), symbol->getClusterName());
		
      int count = 0;
		CString newSymbolName;
		newSymbolName.Format("%s_%d", symbol->getName(), ++count);
		while (cluster->findSymbol(newSymbolName))
		{
			newSymbolName.Format("%s_%d", symbol->getName(), ++count);
		}

		CEdifSymbol* newSymbol = cluster->addSymbol(newSymbolName);
		instance->setSymbolRef(newSymbolName);

      // Change newBlock to point to a new geometry
      newBlock = Graph_Block_On(GBO_APPEND, newSymbol->generateSymbolBlockName(), sFileStruct->getFileNumber(), 0L);
      newBlock->setBlockType(block->getBlockType());
      Graph_Block_Off();
      sDoc->CopyAttribs(&newBlock->getAttributesRef(), block->getAttributesRef());

      newSymbol->setSymbolBlock(newBlock);

      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *data = block->getDataList().GetNext(pos);
         DataStruct *copy = CopyTransposeEntity(data, 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
         newBlock->getDataList().AddTail(copy);

         if (copy->getDataType() == T_INSERT && copy->getInsert()->getInsertType() == INSERTTYPE_PORTINSTANCE)
         {
            CString portName;
            int keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);
            Attrib* attrib;

            if (!copy->getAttributesRef())
               continue;

            if (transform.GetScaleY() < 0)
            {
               // We want to rotate the attributes on the port so it looks right
               // so we just use this work around
               POSITION pos = copy->getAttributesRef()->GetStartPosition();
               while (pos)
               {
                  WORD key;
                  Attrib* attrib;
                  copy->getAttributesRef()->GetNextAssoc(pos, key, attrib);

                  if (attrib != NULL)
                     attrib->incRotationRadians(DegToRad(180));

                  switch(attrib->getHorizontalPosition())
                  {
                     case horizontalPositionLeft:   attrib->setHorizontalPosition(horizontalPositionRight);  break;
                     case horizontalPositionRight:  attrib->setHorizontalPosition(horizontalPositionLeft);   break;
                  }
               }
            }

            if (copy->getAttributesRef()->Lookup(keyword, attrib))
            {
               if (attrib)
                  portName = GetAttribStringValue(attrib);
            }

            // if there are changes in attribute values on the port, change the value
            if (instance->portAttribMap.GetCount() != 0 && portName != "")
            {
               void *voidMapPtr;

               if (instance->portAttribMap.Lookup(portName, voidMapPtr))
               {
                  CAttributes* attribMap = (CAttributes*)voidMapPtr;
                  CopyANDRemoveAttribMap(&copy->getAttributesRef(), attribMap);
               }
            }

            // if there are changes in attribute location on an inserted port, change the location
            if (instance->portAttribDisplayMap.GetCount() != 0)
            {
               void *voidMapPtr;
               if (instance->portAttribDisplayMap.Lookup(copy->getInsert()->getRefname(), voidMapPtr))
               {
                  CAttributes* attribMap = (CAttributes*)voidMapPtr;
                  
                  for (POSITION pos = attribMap->GetStartPosition();pos != NULL;)
                  {
                     WORD key;
                     Attrib* attrib;
                     attribMap->GetNextAssoc(pos, key, attrib);

                     // if only visiblility is change on the attribute, then don't continue
                     if (attrib->getFlags() == ATTRIB_CHANGE_VISIBLE)
                        continue;

                     attrib->offsetCoordinate(-(copy->getInsert()->getOriginX()),-(copy->getInsert()->getOriginY()));
                     attrib->incRotationRadians(- copy->getInsert()->getAngle());
                  }

                  CopyANDRemoveAttribMap(&copy->getAttributesRef(), attribMap);
               }
            }

				CSymbolPort* symbolPort = newSymbol->addPort(copy->getInsert()->getRefname());
				symbolPort->setPortRef(portName);
				symbolPort->setPortDataStruct(copy);
         }
      }
   }

   int mirror = 0;
   double rotation = transform.GetRotation();
   if (transform.GetScaleX() < 0)
      mirror ^= MIRROR_FLIP;
   if (transform.GetScaleY() < 0)  
   {
      mirror ^= MIRROR_FLIP;
      rotation += DegToRad(180);
   }

   double scale;
   if (fabs(transform.GetScaleX()) / fabs(transform.GetScaleY()) == 1) // if scaleX and scaleY are the same, then just use either one
      scale = fabs(transform.GetScaleX());
   else // scaleX and scaleY are difference, default it to 1
      scale = 1;

   //*rcf CString instanceName(instance->getName());

   DataStruct *data = Graph_Block_Reference(newBlock->getName(), instance->getName(), sFileStruct->getFileNumber(),
							 transform.GetOrigin()->x, transform.GetOrigin()->y, rotation, mirror, scale, sSymbolGrpLayer, TRUE);
   if (data)
   {
      if (block->getBlockType() == BLOCKTYPE_SYMBOL)
      {
         Attrib* attrib;
         int keyword = sDoc->RegisterKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0, valueTypeString);

         if (block->getAttributesRef() && block->getAttributesRef()->Lookup(keyword, attrib))
            data->getInsert()->setInsertType(insertTypeHierarchicalSymbol);
         else
            data->getInsert()->setInsertType(insertTypeSymbol);      
      }
      else 
      {
         data->getInsert()->setInsertType(insertTypeUnknown);
      }
   }
	instance->setInstanceDataStruct(data);

   // Update designator of instancePort from port insert in newBlock
   for (CDataListIterator dataListIterator(*newBlock, insertTypePortInstance); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
      CString portName = getValueFromPortNameAttrib(data->getAttributesRef());
      CString designator = getValueFromDesignatorAttrib(data->getAttributesRef());
      CString primaryName = getValueFromPrimaryNameAttrib(data->getAttributesRef());
      
      CInstancePort* instancePort = instance->addInstancePort(portName);
      instancePort->setDesignator(designator);
      instancePort->setPrimaryName(primaryName);
   }

   return data;
}

/******************************************************************************
* schematicSymbolRef
   - get the symbol reference
*/
static int schematicSymbolRef()
{
   return GetString(sSymbolRef);
}

/******************************************************************************
* instancePortAttributeDisplay  
   - Attach or modify attributes loation of a port within an instance
   - portImplementationRef is referencing to the port implementation instance
   - since instancePortAttributeDisplay is only call from the instance construct
     so it is okay to add it to the current instance
   - the attrib map only contain attributes location but not the value,
     the value is already given in instancePortAttributes
*/
static int instancePortAttributeDisplay()
{
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
      
   CString portImplementationRef;
   int res;

   if ((res = GetString(portImplementationRef)) < 0)
      return res;

   if ((res = LoopCommand(instancePortAttributeDisplay_lst, SIZ_INSTANCEPORTATTRIBUTEDISPLAY_LST)) < 0)
      return res;

	CInstance* instance = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->findInstance(sInstanceRef);
	if (instance == NULL)
	{
      fprintf(sFileErr, "Syntax Error : Referencing object \"%s\" that is not previously defined.  Error is at line %ld\n", sInstanceRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
	}

   attribMap = sAttribMapList.RemoveTail();
	void* voidPtr = NULL;
   if (instance->portAttribDisplayMap.Lookup(portImplementationRef, voidPtr))
   {
      CAttributes* tmpAttribMap = (CAttributes*)voidPtr;
      delete tmpAttribMap;
   }
   instance->portAttribDisplayMap.SetAt(portImplementationRef, attribMap);

   return 1;
}

/******************************************************************************
* schematicMasterPortImplementation
*/
static int schematicMasterPortImplementation()
{
   CString refdes;
   int res;

   if ((res = GetString(refdes)) < 0)
      return res;

   sPortRefArr.RemoveAll();
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   if ((res = LoopCommand(schematicMasterPortImplementation_lst, SIZ_SCHEMATICMASTERPORTIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("masterPort_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("masterPort_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   CString portRef;

   if (sPortRefArr.GetSize() != 0)
   {
      portRef = sPortRefArr.GetAt(0); // there should only be one port ref
      sPortRefArr.RemoveAll();
   }

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, refdes, INSERTTYPE_PORTINSTANCE);
   delete transform;

   if (data == NULL)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;

      return -1;
   }
              
	CPort* port = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->addMasterPort(refdes);
   port->setPortDataStruct(data);
	port->setPortRef(portRef);

   CEdifPort* edifPort = NULL;
   if (sPortMap.Lookup(portRef, edifPort))
   {
      // find the port reference and copy the attributes
      CopyANDRemoveAttribMap(&data->getAttributesRef(), edifPort->attribMap);
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;
   return 1;
}

/******************************************************************************
* schematicNet
   - the net list that connects the ports and tie dots
*/
static int schematicNet()
{
   CString schematicNetName;
   int res;

   if ((res = GetString(schematicNetName)) < 0)
      return res;

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   sPolyList = new CPolyList();

   int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, schematicNetName.GetBuffer(0), attributeUpdateOverwrite, NULL);

   if ((res = LoopCommand(schematicNet_lst, SIZ_SCHEMATICNET_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   DataStruct *data = Graph_PolyStruct(sSignalGrpLayer, 0L, FALSE);
   data->setGraphicClass(graphicClassSignal);
   delete data->getPolyList();
   data->getPolyList() = sPolyList;
   sPolyList = NULL;

   //if (sSignalRef == "")
   //{
   //   fprintf(sFileErr, "Warning: schematic net \"%s\" has no signal\n", schematicNetName);
   //   sDisplayErr++;
   //}
   //else
   //{
   //   NetStruct *net = find_net(sFileStruct, sSignalRef);
   //   if (net != NULL)
   //   {
   //      CopyANDRemoveAttribMap(&data->getAttributesRef(), net->getAttributes());

   //      int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   //      Attrib* attrib = NULL;
   //      if (net->getAttributesRef() && net->getAttributesRef()->Lookup(keyword, attrib))
   //      {
   //         CString netDisplayName = sDoc->ValueArray[attrib->getStringValueIndex()];
   //      
   //         keyword = sDoc->IsKeyWord("NetDisplayName", 0);
   //         sDoc->SetAttrib(&data->getAttributesRef(), keyword, valueTypeString, netDisplayName.GetBuffer(0), attributeUpdateOverwrite, NULL);
   //      }

   //      keyword = sDoc->IsKeyWord(ATT_NETNAME, 0);
   //      sDoc->SetAttrib(&data->getAttributesRef(), keyword, valueTypeString, sSignalRef.GetBuffer(0), attributeUpdateOverwrite, NULL);
   //   }
   //   else
   //   {
   //      fprintf(sFileErr, "Error: signal \"%s\" is undefined\n", sSignalRef);
   //      sDisplayErr++;
   //   }
   //}


	CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();
	CNet* viewNet = view->getCurrentNet();    // current net was set at signalRef()

   attribMap = sAttribMapList.RemoveTail();
   if (viewNet != NULL)
   {
      CopyANDRemoveAttribMap(&viewNet->getAttributesRef(), attribMap);
      CopyANDRemoveAttribMap(&data->getAttributesRef(), viewNet->getAttributesRef());

      int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
      Attrib* attrib = NULL;
      if (viewNet->getAttributesRef() && viewNet->getAttributesRef()->Lookup(keyword, attrib))
      {
         CString netDisplayName = GetAttribStringValue(attrib);
      
         keyword = sDoc->IsKeyWord("NetDisplayName", 0);
         sDoc->SetAttrib(&data->getAttributesRef(), keyword, valueTypeString, netDisplayName.GetBuffer(0), attributeUpdateOverwrite, NULL);
      }

      keyword = sDoc->IsKeyWord(ATT_NETNAME, 0);
      sDoc->SetAttrib(&data->getAttributesRef(), keyword, valueTypeString, viewNet->getName().GetBuffer(0), attributeUpdateOverwrite, NULL);
   }
   else
   {
      fprintf(sFileErr, "Warning: schematic net \"%s\" has no signal\n", schematicNetName);
      sDisplayErr++;

      CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   }

   delete attribMap;
   attribMap = NULL;
    
   view->setCurrentNet(NULL);
   return 1;
}

/******************************************************************************
* signalRef
   - get the name of the signal use
*/
static int signalRef()
{
	sSignalRef.Empty();

   int res = GetString(sSignalRef);

	CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();
	CBus* currentBus = view->getCurrentBus();
	CNet* net= view->addNet(sSignalRef);

	if (currentBus != NULL && net != NULL)
	{
      // called by signalGroup so add it to the bus
		currentBus->addNet(net);
	}
   else
   {
      // called by schematicNet so set as current net in view
      view->setCurrentNet(net);
   }

	return res;
}

/******************************************************************************
* schematicInterconnectHeader
   - Get info such as the display net name amd definition of tie dots and terminators
*/
static int schematicInterconnectHeader()
{
   return LoopCommand(schematicInterconnectHeader_lst, SIZ_SCHEMATICINTERCONNECTHEADER_LST); 
}

/******************************************************************************
* schematicInterconnectTerminatorImplementation
   - insert of terminator
*/
static int schematicInterconnectTerminatorImplementation()
{
   CString implementationName;
   int res;

   if ((res = GetString(implementationName)) < 0)
      return res;

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   if ((res = LoopCommand(schematicInterconnectTerminatorImplementation_lst, SIZ_SCHEMATICINTERCONNECTTERMINATORIMPLEMENTATION_LST)) <  0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("terminator_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("terminator_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);
   
   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, implementationName, INSERTTYPE_TERMINATOR);
   delete transform;

   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);

   delete attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* schematicJunctionImplementation
   - insert of tie dot
*/
static int schematicJunctionImplementation()
{
   CString implementationName;
   int res;

   if ((res = GetString(implementationName)) < 0)
      return res;

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   if ((res = LoopCommand(schematicJunctionImplementation_lst, SIZ_SCHEMATICJUNCTIONIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("tieDot_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("tieDot_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);
   
   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, implementationName, INSERTTYPE_TIEDOT);
   delete transform;

   if (data == NULL)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;

      return -1;
   }

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);

   delete attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* schematicNetJoined
   - get the list of portjoined on the net list
*/
static int schematicNetJoined()
{
   return LoopCommand(schematicNetJoined_lst, SIZ_SCHEMATICNETJOINED_LST); 
}

/******************************************************************************
* portJoined
   - get all the global, master, and instance ports that are jointed the the same schematicNet
*/
static int portJoined()
{
   return LoopCommand(signalJoined_lst, SIZ_SIGNALJOINED_LST); 
}

/******************************************************************************
* ripperHotSpotRef
*/
static int ripperHotSpotRef()
{
   sRipperHotspotRef.Empty();
	int res;
	if ((res = GetString(sRipperHotspotRef)) < 0)
		return res;

   return LoopCommand(ripperHotSpotRef_lst, SIZ_RIPPERHOTSPOTREF_LST); 
}

/******************************************************************************
* schematicRipperImplementationRef
*/
static int schematicRipperImplementationRef()
{
	CString implementationRef;
	int res;

	if ((res = GetString(implementationRef)) < 0)
		return res;

   // New implementation
   CEdifView* view = edifStruct.getCurrentView();
   CRipper* ripper = view->findRipper(implementationRef);
   CRipperHotspot* hotspot = ripper->addRipperHotspot(sRipperHotspotRef);

   CNet* net = view->getCurrentNet();

   if (net != NULL)
   {
      net->addRipper(ripper);
      if (hotspot->getConnectedToSignal() == NULL)
         hotspot->setConnectedToSignal(net);

      //if (ripper->getConnectedToSignal1() == NULL)
      //   ripper->setConnectedToSignal1(net);
      //else
      //   ripper->setConnectedToSignal2(net);
   }

   CBus* bus = edifStruct.getCurrentBus();

   if (bus != NULL)
   {
      bus->addRipper(ripper);

      if (hotspot->getConnectedToSignal() == NULL)
         hotspot->setConnectedToSignal(bus);

      //if (ripper->getConnectedToSignal1() == NULL)
      //   ripper->setConnectedToSignal1(bus);
      //else
      //   ripper->setConnectedToSignal2(bus);
   }
	
	return res;
}

/******************************************************************************
* schematicGlobalPortImplementationRef
*/
static int schematicGlobalPortImplementationRef()
{
   CString implementationRef;
   int res;

   if ((res = GetString(implementationRef)) < 0)
      return res;

   CEdifView* view = edifStruct.getCurrentView();
   CPort* globalPort = view->findGlobalPort(implementationRef);
   
   CNet* net = view->getCurrentNet();

   if (net != NULL)
   {
      net->addGlobalPort(globalPort);
      globalPort->setConnectedToSignal(net);
   }

   CBus* bus = edifStruct.getCurrentBus();

   if (bus != NULL)
   {
      bus->addGlobalPort(globalPort);
      globalPort->setConnectedToSignal(bus);
   }

	return 1;
}

/******************************************************************************
* schematicMasterPortImplementationRef 
*/
static int schematicMasterPortImplementationRef ()
{
   CString implementationRef;
   int res;

   if ((res = GetString(implementationRef)) < 0)
      return res;

   CEdifView* view = edifStruct.getCurrentView();
   CPort* masterPort = view->findMasterPort(implementationRef);

   CNet* net = view->getCurrentNet();

   if (net != NULL)
   {
      net->addMasterPort(masterPort);
      masterPort->setConnectedToSignal(net);
   }

   CBus* bus = view->getCurrentBus();

   if (bus != NULL)
   {
      bus->addMasterPort(masterPort);
      masterPort->setConnectedToSignal(bus);
   }

	return 1;
}

/******************************************************************************
* schematicOnPageConnectorImplementationRef
	- indicate the the net is connect to the specificed page connector
*/
static int schematicOnPageConnectorImplementationRef()
{
   CString implementationRef;
   int res;

   if ((res = GetString(implementationRef)) < 0)
      return res;

   CString blockName;
   blockName.Format("%s%c%s%c%s%c%s%c%s", sLibraryName, SCH_DELIMITER, sCellName, SCH_DELIMITER, sClusterName, SCH_DELIMITER,
                    sViewName, SCH_DELIMITER, sPageName);

	BlockStruct *sheet = sDoc->Find_Block_by_Name(blockName, sFileStruct->getFileNumber());

	if (sheet)
	{
		DataStruct *connector = sheet->FindData(implementationRef);
		WORD netnameKey = sDoc->IsKeyWord(ATT_NETNAME, 0);
#if CamCadMajorMinorVersion > 406  //  > 4.6
      connector->setAttrib(sDoc->getCamCadData(), netnameKey, valueTypeString, (void*)sSignalRef.GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
		connector->setAttrib(sDoc, netnameKey, valueTypeString, (void*)sSignalRef.GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
   }


   // New implementation
   CEdifView* view = edifStruct.getCurrentView();
   CPageConnector* connector = view->findOnPageConnector(implementationRef);

   CNet* net = view->getCurrentNet();

   if (net != NULL)
   {
      net->addOnPageConnector(connector);
      connector->setConnectedToSignal(net);
   }

   CBus* bus = view->getCurrentBus();

   if (bus != NULL)
   {
      bus->addOnPageConnector(connector);
      connector->setConnectedToSignal(net);
   }

	return 1;
}

/******************************************************************************
* schematicOffPageConnectorImplementationRef
	- indicate the the net is connect to the specificed page connector
*/
static int schematicOffPageConnectorImplementationRef()
{
   CString implementationRef;
   int res;

   if ((res = GetString(implementationRef)) < 0)
      return res;

   CString blockName;
   blockName.Format("%s%c%s%c%s%c%s%c%s", sLibraryName, SCH_DELIMITER, sCellName, SCH_DELIMITER, sClusterName, SCH_DELIMITER,
                    sViewName, SCH_DELIMITER, sPageName);

	BlockStruct *sheet = sDoc->Find_Block_by_Name(blockName, sFileStruct->getFileNumber());

	if (sheet)
	{
		DataStruct *connector = sheet->FindData(implementationRef);
		WORD netnameKey = sDoc->IsKeyWord(ATT_NETNAME, 0);
#if CamCadMajorMinorVersion > 406  //  > 4.6
      connector->setAttrib(sDoc->getCamCadData(), netnameKey, valueTypeString, (void*)sSignalRef.GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
		connector->setAttrib(sDoc, netnameKey, valueTypeString, (void*)sSignalRef.GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
   }


   // New implementation
   CEdifView* view = edifStruct.getCurrentView();
   CPageConnector* connector = view->findOffPageConnector(implementationRef);

   CNet* net = view->getCurrentNet();

   if (net != NULL)
   {
      net->addOffPageConnector(connector);
      connector->setConnectedToSignal(net);
   }

   CBus* bus = view->getCurrentBus();

   if (bus != NULL)
   {
      bus->addOffPageConnector(connector);
      connector->setConnectedToSignal(net);
   }

	return 1;
}

/******************************************************************************
* schematicInterconnectAttributeDisplay
   - display thing such as schematicNet name, property, text
*/
static int schematicInterconnectAttributeDisplay()
{
   return LoopCommand(schematicInterconnectAttributeDisplay_lst, SIZ_SCHEMATICINTERCONNECTATTRIBUTEDISPLAY_LST);  
}

/******************************************************************************
* interconnectAttachedText
   - attach thing such as schematicNet name, property, text at the specified attachment point
   - currently we ignore the attachment point and just display things at its display location
*/
static int interconnectAttachedText()
{
   CPnt *pnt = new CPnt();
   int res;
   if ((res = GetPoint(pnt, TRUE)) < 1) 
   {
      delete pnt;
      return -1;
   }

   // do nothing with the attach points for now, so delete it
   delete pnt;
  
   return LoopCommand(interconnectAttachedText_lst, SIZ_INTERCONNECTATTACHEDTEXT_LST); 
}

/******************************************************************************
* interconnectNameDisplay
   - This function almost do the same thing as nameDisplay(), but since
     we can't have attributes on an etch we will attach the attributes as text instead
*/
static int interconnectNameDisplay()
{
	CDisplay* display = new CDisplay();
	sDisplayList.AddTail(display);
   sRemoveDisplay = FALSE;

   int res = LoopCommand(getNameDisplay_lst, SIZ_GETNAMEDISPLAY_LST);
   if (res < 0)
   {
		display = sDisplayList.RemoveTail();
		delete display;
		display = NULL;
      return res;
   }

   CAttributes* attribMap = sAttribMapList.GetTail();
	int primaryNameKey = sDoc->RegisterKeyWord(SCH_ATT_PRIMARYNAME, 0, valueTypeString);
   int displayNameKey = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   Attrib* attrib = NULL;
	if (!attribMap->Lookup(primaryNameKey, attrib) || attrib == NULL)
	{
		// First check for "PrimaryName" attribute, if it is not there
		// then use the "DisplayName" attribute
		attribMap->Lookup(displayNameKey, attrib);
	}

	display = sDisplayList.RemoveTail();
	if (!sRemoveDisplay && attrib != NULL)
	{
		CString stringValue = GetAttribStringValue(attrib);
		res = GraphTextDisplays(display, sTextLayer, stringValue);
	}
	delete display;
	display = NULL;

   return 1;
}

/******************************************************************************
* interconnectAttachedText
   - This function almost do the same thing as propertyDisplay(), but since
     we can't have attributes on an etch we will attach the attributes as text instead
*/
static int interconnectPropertyDisplay()
{
	CDisplay* display = new CDisplay();
	sDisplayList.AddTail(display);
   sRemoveDisplay = FALSE;

   CString propertyName;
   int res;

   if ((res = GetString(propertyName)) < 0)
   {
		display = sDisplayList.RemoveTail();

		delete display;
		display = NULL;

      return res;
   }
   
   if ((res = LoopCommand(getDisplay_lst, SIZ_GETDISPLAY_LST)) < 0)
   {
		display = sDisplayList.RemoveTail();

		delete display;
		display = NULL;

		return res;
   }

   int keywordIndex = -1;
   CString ccKeyword;

   if (sEdifKeywordMap.Lookup(propertyName, ccKeyword))
   {
      keywordIndex = sDoc->IsKeyWord(ccKeyword, 0);

      if (keywordIndex < 0)
      {
         fprintf(sFileErr, "Syntax Error : Referencing property \"%s\" that is not previously defined.  Error is within the structure of InterconnectPropertyDisplay close to line %ld\n", propertyName, sFileInLineCnt);
         sDisplayErr++;

			display = sDisplayList.RemoveTail();

			delete display;
			display = NULL;

         return -1;
      }
   }
   else
   {
      fprintf(sFileErr, "Syntax Error : Referencing property \"%s\" that is not previously defined.  Error is within the structure of InterconnectPropertyDisplay close to line %ld\n", propertyName, sFileInLineCnt);
      sDisplayErr++;

		display = sDisplayList.RemoveTail();

		delete display;
		display = NULL;

      return -1;
   }

   CAttributes* attribMap = sAttribMapList.GetTail();
   Attrib* attrib;

   attribMap->Lookup(keywordIndex, attrib);

	display = sDisplayList.RemoveTail();

	if (!sRemoveDisplay && attrib != NULL)
	{
      CString stringValue;

      if (attrib->getValueType() == valueTypeString)
         stringValue = GetAttribStringValue(attrib);
      else if (attrib->getValueType() == VT_DOUBLE)
         stringValue.Format("%d", attrib->getDoubleValue());
      else if (attrib->getValueType() == VT_INTEGER)
         stringValue.Format("%l", attrib->getIntValue());

		res = GraphTextDisplays(display, sTextLayer, stringValue);
	}

	delete display;
	display = NULL;

   return res;
}

/******************************************************************************
* schematicNetDetails
   - get definition of the schematicNet and schematicSubNetSet
*/
static int schematicNetDetails()
{
   return LoopCommand(schematicNetDetails_lst, SIZ_SCHEMATICNETDETAILS_LST);  
}

/******************************************************************************
* schematicNetGraphics
   - get the graphical representation of schematicNet
*/
static int schematicNetGraphics()
{
   return LoopCommand(schematicNetGraphics_lst, SIZ_SCHEMATICNETGRAPHICS_LST);   
}

/******************************************************************************
* schematicSubNetSet
   - get the sub net of schematicNet
*/
static int schematicSubNetSet()
{
   return LoopCommand(schematicSubNetSet_lst, SIZ_SCHEMATICSUBNETSET_LST); 
}

/******************************************************************************
* schematicSubNet
   - the sub net list that connects the ports and tie dots
*/
static int schematicSubNet()
{
   CString schematicSubNetName;
   int res;

   if ((res = GetString(schematicSubNetName)) < 0)
      return res;

   return LoopCommand(schematicSubNet_lst, SIZ_SCHEMATICSUBNET_LST); 
}

/******************************************************************************
* schematicSubInterconnectHeader
   - Get info such as the display sub net name, property
*/
static int schematicSubInterconnectHeader()
{
   return LoopCommand(schematicSubInterconnectHeader_lst, SIZ_SCHEMATICSUBINTERCONNECTHEADER_LST); 
}

/******************************************************************************
* schematicOffPageConnectorImplementation
   -  a connector representing the fact that a net or bus is continued
      elsewhere on the schematic on another page       
*/
static int schematicOffPageConnectorImplementation()
{
   CString implementationName;
   int res;

   if ((res = GetString(implementationName)) < 0)
      return res;

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   if ((res = LoopCommand(schematicOffPageConnectorImplementation_lst, SIZ_SCHEMATICOFFPAGECONNECTORIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("offPage_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("offPage_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, implementationName, INSERTTYPE_SHEETCONNECTOR);

   delete transform;

   if (data == NULL)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;

      return -1;
   }

	CPageConnector* connector = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->addOffPageConnector(implementationName);
	connector->setConnectorDataStruct(data);

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);

   delete attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* schematicOnPageConnectorImplementation
   - a connector representing the fact that a net or bus is continued 
     elsewhere on the schematic on the same page
*/
static int schematicOnPageConnectorImplementation()
{
   CString implementationName;
   int res;

   if ((res = GetString(implementationName)) < 0)
      return res;

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   if (( res = LoopCommand(schematicOnPageConnectorImplementation_lst, SIZ_SCHEMATICONPAGECONNECTORIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("onPage_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("onPage_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, implementationName, INSERTTYPE_SHEETCONNECTOR);
   delete transform;

   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

	CPageConnector* connector = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->addOnPageConnector(implementationName);
	connector->setConnectorDataStruct(data);

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);

   delete attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* schematicRipperImplementation
   - insert a ripper which is used to associate one interconnector with another
*/
static int schematicRipperImplementation()
{
   CString implementationName;
   int res;

   if ((res = GetString(implementationName)) < 0)
      return res;

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   CTransform *transform = new CTransform();
   sTransformList.AddTail(transform);

   if (( res = LoopCommand(schematicRipperImplementation_lst, SIZ_SCHEMATICRIPPERIMPLEMENTATION_LST)) < 0)
   {
      transform = sTransformList.RemoveTail();
      delete transform;
      return res;
   }

   CString blockName;

   if (sLibraryRef != "")
      blockName.Format("ripper_%s%c%s", sLibraryRef, SCH_DELIMITER, sObjectRef);
   else
      blockName.Format("ripper_%s%c%s", sLibraryName, SCH_DELIMITER, sObjectRef);

   transform = sTransformList.RemoveTail();
   DataStruct *data = InsertGlobalGeometry(blockName, *transform, implementationName, INSERTTYPE_RIPPER);
   delete transform;
   if (!data)
   {
      fprintf(sFileErr, "Syntax Error : Referencing geometry \"%s\" that is not previously defined.  Error is close to line %ld\n", sObjectRef, sFileInLineCnt);
      sDisplayErr++;
      return -1;
   }

	CRipper* ripper = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView()->addRipper(implementationName);

   attribMap = sAttribMapList.RemoveTail();
   CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
   delete attribMap;
   attribMap = NULL;

   if (data->getAttributes() != NULL)
   {
      CopyANDRemoveAttribMap(&ripper->getAttributesRef(), data->getAttributes());
   }

   return 1;
}

/******************************************************************************
* designHeader
   - gather together data relating to the design as a whole
*/
static int designHeader()
{
   return LoopCommand(designHeader_lst, SIZ_DESIGNHEADER_LST); 
}

/******************************************************************************
* designUnits
   - get the units that will be use inside the design construct
*/
static int designUnits()
{
   return LoopCommand(designUnits_lst, SIZ_DESIGNUNITS_LST);   
}

/******************************************************************************
* designHierarchy
   - reference to a cluster configuration and may provide annotation data for the hierarchy
*/
static int designHierarchy()
{
   sClusterRef.Empty();
   sClusterConfigRef.Empty();

   CString hierarchyName;
   int res;

   if ((res = GetString(hierarchyName)) < 0)
      return res;

   return LoopCommand(designHierarchy_lst, SIZ_DESIGNHIERARCHY_LST); 
}

/******************************************************************************
* clusterConfigurationRef
   - get the cluster configuration reference of the current cluster
*/
static int clusterConfigurationRef()
{
   sClusterConfigRef.Empty();

   return GetString(sClusterConfigRef);
}

/******************************************************************************
* designHierarchyHeader
   - get the hierarchy name and property
*/
static int designHierarchyHeader()
{
   return LoopCommand(designHierarchyHeader_lst, SIZ_DESIGNHIERARCHYHEADER_LST); 
}

/******************************************************************************
* occurrenceHierarchyAnnotate
   - provides overriding properties for the top view in the hierarchy tree
     and further annotation data for instances below this are provided 
     using occurrenceAnnotate and leafOccurrenceAnnotate. 
*/
static int occurrenceHierarchyAnnotate()
{
	int res = 0;

	CClusterConfiguration* configuration = edifStruct.findClusterConfiguration(sLibraryRef, sObjectRef, sClusterRef, sClusterConfigRef);
	if (configuration == NULL)
   {
  		fprintf(sFileErr, "Error: Skip occurrenceHierarchyAnnotate, cannot find the clusterConfigurationRef [%s] around line %d\n", sClusterConfigRef, sFileInLineCnt);
		sDisplayErr++;

		skipCommand();
		return res;
   }

	CEdifView* view = edifStruct.findView(sLibraryRef, sObjectRef, sClusterRef, configuration->getViewRef());
   if (view == NULL)
   {
		fprintf(sFileErr, "Error: Skip occurrenceHierarchyAnnotate, cannot find the view that clusterConfigurationRef [%s] is refered to around line %d\n", sClusterConfigRef, sFileInLineCnt);
		sDisplayErr++;

		skipCommand();
   }
	else
	{		
		COccurrenceAnnotate* occurrenceAnnotate = new COccurrenceAnnotate(true);
      occurrenceAnnotate->setClusterConfiguration(configuration);
		sOccurrenceAnnotate.AddTail(occurrenceAnnotate);
		res = occurrenceAnnotate->DoOccurrenceAnnotate();

		occurrenceAnnotate = sOccurrenceAnnotate.RemoveTail();
		delete occurrenceAnnotate;
		occurrenceAnnotate = NULL;
	}

	return res;
}

/******************************************************************************
* leafOccurrenceAnnotate
   - get texts, designator, or properties to be added or override for an instance
   - if there is no instance name, then skip the leafOccurrenAnnotate callout construct
*/
static int leafOccurrenceAnnotate()
{
	CLeafAnnotate* leafAnnotate = new CLeafAnnotate();
	int res = leafAnnotate->DoLeafAnnotate();

	if (res > 0)
	{
		COccurrenceAnnotate* occurrenceAnnotate = sOccurrenceAnnotate.GetTail();
		occurrenceAnnotate->AddLeafAnnotate(leafAnnotate);
	}
	else
	{
		delete leafAnnotate;
		leafAnnotate = NULL;
	}

	return res;
}

/******************************************************************************
* portAnnotate
   - get texts, designator, or properties to be added or override for a port
*/
static int portAnnotate()
{
   CString designator = char(3);
   sDesList.AddTail(designator);
   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);

   CString portName;
   int res;

   if ((res = GetString(portName)) < 0)
      return res;

   if ((res = LoopCommand(portAnnotate_lst, SIZ_PORTANNOTATE_LST)) < 0)
      return res;

   CEdifPort* port = new CEdifPort();
   port->isBundle = FALSE;
   port->attribMap = NULL;
   
   attribMap = sAttribMapList.RemoveTail();
   designator = sDesList.RemoveTail(); // get & remove the designator from stack
   Attrib* attrib;
   int keyword = sDoc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
	if (designator != char(3))
	{
		sDoc->SetAttrib(&attribMap, keyword, valueTypeString, designator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
		attrib->setFlagBits(ATTRIB_CHANGE_VALUE);
	}

   keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, portName.GetBuffer(0), attributeUpdateOverwrite, &attrib);
   attrib->setFlagBits(ATTRIB_CHANGE_VALUE);

   CopyANDRemoveAttribMap(&port->attribMap, attribMap);
   delete attribMap;
   attribMap = NULL;

   CEdifPort* tmpPort = NULL;
   if (sPortMap.Lookup(portName, tmpPort))
   {
      delete tmpPort;
		tmpPort = NULL;
   }
   sPortMap.SetAt(portName, port);

   return 1;
}

/******************************************************************************
* instancePropertyOverride
   - get the property of an instance to be override
*/
static int instancePropertyOverride()
{
   CString propertyNameRef;
   int res;

   if ((res = GetString(propertyNameRef)) < 0)
      return res;

   return LoopCommand(property_lst, SIZ_PROPERTY_LST);   
}

/******************************************************************************
* instancePropertyOverride
*/
static int occurrenceAnnotate()
{ 
   int res = 0;
   CString name;

	if ((res = GetString(name)) < 0)
   {
		return res;
   }
	else if (name == "")
	{
		skipCommand();
		return 0;
	}

   // Get the parent occurrenceAnnotate in the upper hierarchy, and then get its cluster configuration and view
   COccurrenceAnnotate* parentOccurrenceAnnotate = sOccurrenceAnnotate.GetTail();
   CClusterConfiguration* parentClusterConfiguration = parentOccurrenceAnnotate->getClusterConfiguration();
   CEdifView* parentView = parentClusterConfiguration->getView();

   // Find the instanceConfiguration and instance for this occurrenceAnnotate
   CInstanceConfiguration* instanceConfiguration = parentClusterConfiguration->findInstanceConfiguration(name);
   CInstance* instance = parentView->findInstance(name);

   // Find the cluster that the instance referenced to and the clusterConfiguration referenced for the instance
   CEdifCluster* cluster = edifStruct.findCluster(instance->getLibraryRef(), instance->getCellRef(), instance->getClusterRef());
   CClusterConfiguration* clusterConfiguration = NULL;
   if (instanceConfiguration != NULL)
   {
      clusterConfiguration = cluster->findClusterConfiguration(instanceConfiguration->getClusterConfigurationRef());
   }
   else
   {
      clusterConfiguration = cluster->getDefaultConfiguration();
   }

   // Create new occurrenceAnnotate structure and process this annotation
	COccurrenceAnnotate* occurrenceAnnotate = new COccurrenceAnnotate(false);
   occurrenceAnnotate->setClusterConfiguration(clusterConfiguration);
   occurrenceAnnotate->setInstance(instance);
	sOccurrenceAnnotate.AddTail(occurrenceAnnotate);   
   res = occurrenceAnnotate->DoOccurrenceAnnotate();

	occurrenceAnnotate = sOccurrenceAnnotate.RemoveTail();
	delete occurrenceAnnotate;
	occurrenceAnnotate = NULL;

	return res;
}

/******************************************************************************
* pageAnnotate
   - get the annotation data for schematicNet
*/
static int pageAnnotate()
{
   CString pageNameRef;
   int res;

   if ((res = GetString(pageNameRef)) < 0)
      return res;

   return LoopCommand(pageAnnotate_lst, SIZ_PAGEANNOTATE_LST); 
}

/******************************************************************************
* interconnectAnnotate
   - is used to associate physical characteristics with schematicNet
*/
static int interconnectAnnotate()
{
   CString netNameRef;
   int res;

   if ((res = GetString(netNameRef)) < 0)
      return res;

   return LoopCommand(interconnectAnnotate_lst, SIZ_INTERCONNECTANNOTATE_LST);   
}

/******************************************************************************
* signalAnnotate
   - is used to associate physical characteristics with signals 
*/
static int signalAnnotate()
{
	CSignalAnnotate* signalAnnotate = new CSignalAnnotate();
	int res = signalAnnotate->doSignalAnnotate();

	if (res > 0)
	{
		COccurrenceAnnotate* occurrenceAnnotate = sOccurrenceAnnotate.GetTail();
		occurrenceAnnotate->addSignalAnnotate(signalAnnotate);
	}
	else
	{
		delete signalAnnotate;
		signalAnnotate = NULL;
	}

	return res;
}

/******************************************************************************
* schematicBus
*/
static int schematicBus()
{
   CString schematicBusName;
   int res;

   if ((res = GetString(schematicBusName)) < 0)
      return res;

   CAttributes* attribMap = new CAttributes();
   sAttribMapList.AddTail(attribMap);
   sPolyList = new CPolyList();

   int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
   sDoc->SetAttrib(&attribMap, keyword, valueTypeString, schematicBusName.GetBuffer(0), attributeUpdateOverwrite, NULL);

   if ((res = LoopCommand(schematicBus_lst, SIZ_SCHEMATICBUS_LST)) < 0)
   {
      attribMap = sAttribMapList.RemoveTail();
      delete attribMap;
      attribMap = NULL;

      FreePolyList(sPolyList);
      return res;
   }

   
	attribMap = sAttribMapList.RemoveTail();
   CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();

   if (sPolyList->GetCount())
   {
      DataStruct *data = Graph_PolyStruct(sSignalGrpLayer, 0L, FALSE);
      data->setGraphicClass(graphicClassBus);
      delete data->getPolyList();
      data->getPolyList() = sPolyList;


      if (sSignalRef == "")
      {
         fprintf(sFileErr, "Warning: schematic bus \"%s\" has no signal group\n", schematicBusName);
         sDisplayErr++;

			CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
      }
      else
      {
			CBus* viewBus = view->findBus(sSignalRef);

			if (viewBus != NULL)
			{
				CopyANDRemoveAttribMap(&viewBus->getAttributesRef(), attribMap);
				CopyANDRemoveAttribMap(&data->getAttributesRef(), viewBus->getAttributesRef());

				int keyword = sDoc->IsKeyWord(SCH_ATT_DISPLAYNAME, 0);
				Attrib* attrib = NULL;
				if (viewBus->getAttributesRef() != NULL && viewBus->getAttributesRef()->Lookup(keyword, attrib))
				{
					CString netDisplayName = GetAttribStringValue(attrib);
	         
					keyword = sDoc->IsKeyWord("NetDisplayName", 0);
					sDoc->SetAttrib(&data->getAttributesRef(), keyword, valueTypeString, netDisplayName.GetBuffer(0), attributeUpdateOverwrite, NULL);
				}

				keyword = sDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
				sDoc->SetAttrib(&data->getAttributesRef(), keyword, valueTypeString, sSignalRef.GetBuffer(0), attributeUpdateOverwrite, NULL);
			}
			else
			{
				fprintf(sFileErr, "Error: bus \"%s\" is undefined\n", sSignalRef);
				sDisplayErr++;

		      CopyANDRemoveAttribMap(&data->getAttributesRef(), attribMap);
			}
      }
   }
   else
   {
      delete sPolyList;
   }

   sPolyList = NULL;
   delete attribMap;
   attribMap = NULL;

   if (view->getCurrentBus() != NULL)
   {
      view->setCurentBus(NULL);
   }

   return 1;
}

/******************************************************************************
* signalGroupRef
*/
static int signalGroupRef()
{
	sSignalRef.Empty();

	int res = GetString(sSignalRef);

	CEdifView* view = edifStruct.getCurrentLibrary()->getCurrentCell()->getCurrentCluster()->getCurrentView();
	CBus* currentBus = view->getCurrentBus();
	CBus* subBus = view->findBus(sSignalRef);
	if (currentBus != NULL && subBus != NULL)
	{
		currentBus->addBus(subBus);
	}
   else if (currentBus == NULL)
   {
      currentBus = view->findBus(sSignalRef);
      view->setCurentBus(currentBus);
   }

	return res;
}

/******************************************************************************
* schematicBusJoined
*/
static int schematicBusJoined()
{
   return LoopCommand(schematicBusJoined_lst, SIZ_SCHEMATICBUSJOINED_LST); 
}

/******************************************************************************
* schematicBusDetails
*/
static int schematicBusDetails()
{
   return LoopCommand(schematicBusDetails_lst, SIZ_SCHEMATICBUSDETAILS_LST);  
}

/******************************************************************************
* schematicBusGraphics
*/
static int schematicBusGraphics()
{
   return LoopCommand(schematicBusGraphics_lst, SIZ_SCHEMATICBUSGRAPHICS_LST);   
}

/******************************************************************************
* schematicSubBusSet
*/
static int schematicSubBusSet()
{
   return LoopCommand(schematicSubBusSet_lst, SIZ_SCHEMATICSUBBUSSET_LST); 
}

/******************************************************************************
* schematicSubBus
*/
static int schematicSubBus()
{
   CString subBusName;
   int res;

   if ((res = GetString(subBusName)) < 0)
      return res;

   return LoopCommand(schematicSubBus_lst, SIZ_SCHEMATICSUBBUS_LST); 
}

/******************************************************************************
* schematicBusSlice
*/
static int schematicBusSlice()
{
   CString busSliceName;
   int res;

   if ((res = GetString(busSliceName)) < 0)
      return res;

   CString tmpSignalRef = sSignalRef;
   CPolyList *tmpPolyList = sPolyList;

   if ((res = LoopCommand(schematicBus_lst, SIZ_SCHEMATICBUS_LST)) < 0)
      return res;

   sSignalRef = tmpSignalRef;
   sPolyList = tmpPolyList;
   tmpPolyList = NULL;
   return 1;
}

/******************************************************************************
* GraphTextDisplays
*/
static int GraphTextDisplays(CDisplay* display, int textLayerIndex, CString text)
{
	text.Replace("%%", "%");

	while (display!= NULL)
	{
		CTransform* transform = display->GetTransform();
		CFigureGroup* figureGroup = display->GetFigureGroup();

		if (transform != NULL && transform->GetOrigin() != NULL && figureGroup != NULL && figureGroup->GetVisible())
		{
			DataStruct *data = Graph_Text(textLayerIndex, text, transform->GetOrigin()->x, transform->GetOrigin()->y, figureGroup->GetFontHeight(), 
					figureGroup->GetFontWidth(), transform->GetRotation(), 0, 1, 0, 0, FALSE, figureGroup->GetWidthIndex(), 0);

			if (data)
			{
				if (figureGroup->GetHorizontalAlignment() > -1)
					data->getText()->setHorizontalPosition(intToHorizontalPositionTag(figureGroup->GetHorizontalAlignment()));
				if (figureGroup->GetVerticalAlignment() > -1)
					data->getText()->setVerticalPosition(intToVerticalPositionTag(figureGroup->GetVerticalAlignment()));
			}
		}

		display = display->GetNextDisplay();
	}

	return 1;
}

/******************************************************************************
* AddAttributeDisplays
*/
static int AddAttributeDisplays(Attrib* attrib, CDisplay* display, CString layerName)
{
	if (sRemoveDisplay)
	{
		// set flag to indicate there is only overwrite on the visibility
		attrib->setFlags(ATTRIB_CHANGE_VISIBLE);
		attrib->setVisible(false);
		return 1;
	}


	// Currently we haven't implement multiple display of the same attribute;
	// however this function is set up to readily change to support multiple
	// display of attributes.  Only need to change the code to go through link
	// list of display by getting the next display until there is no more

	//while (display != NULL) <== code for multiple display
	if (display != NULL)
	{
		CTransform* transform = display->GetTransform();
		CFigureGroup* figureGroup = display->GetFigureGroup();

		if (transform == NULL || transform->GetOrigin() == NULL || figureGroup == NULL)
		{
			return 1;			

			//display = display->GetNextDisplay(); <== code for multiple display
			//continue;
		}

		// set flag to indicate there is only overwrite on the location and not the value
		attrib->setFlagBits(ATTRIB_CHANGE_LOCATION);

		Point2 point;
		point.x = transform->GetOrigin()->x;
		point.y = transform->GetOrigin()->y;

		double attribRotation = transform->GetRotation();

		if (sTransformList.GetCount() != 0 )
		{
			CTransform *insertTransform = sTransformList.GetTail();
			if (insertTransform->GetOrigin() != NULL)
			{
				attribRotation -= insertTransform->GetRotation();
				double rotation = insertTransform->GetRotation();
				int mirror = 0;

				if (insertTransform->GetScaleX() < 0)
				{
					mirror ^= MIRROR_FLIP;
					if (figureGroup->GetHorizontalAlignment() != 1)
						figureGroup->SetHorizontalAlignment(abs(figureGroup->GetHorizontalAlignment() - 2));
				}
				if (insertTransform->GetScaleY() < 0) 
				{
					rotation += DegToRad(180);

					double insertDeg = RadToDeg(insertTransform->GetRotation());
					while (insertDeg < 0)
						insertDeg += 360;
					while (insertDeg >=360)
						insertDeg -= 360;

					double attribDeg = RadToDeg(attribRotation);
					while (attribDeg < 0)
						attribDeg += 360;
					while (attribDeg >=360)
						attribDeg -= 360;

					if (attribDeg == 0 || abs((int)attribDeg) == 180)
						attribRotation += DegToRad(180);

					mirror ^= MIRROR_FLIP;
					if (figureGroup->GetHorizontalAlignment() != 1)
						figureGroup->SetHorizontalAlignment(abs(figureGroup->GetHorizontalAlignment() - 2));
				}

				double scale;
				if (fabs(insertTransform->GetScaleX()) / fabs(insertTransform->GetScaleY()) == 1) // if scaleX and scaleY are the same, then just use either one
					scale = fabs(insertTransform->GetScaleX());
				else // scaleX and scaleY are difference, default it to 1
					scale = 1;

				SelectStruct transformation;
				transformation.insert_x = insertTransform->GetOrigin()->x;
				transformation.insert_y = insertTransform->GetOrigin()->y;
				transformation.rotation = (DbUnit)rotation;
				transformation.scale = (DbUnit)scale;
				transformation.mirror = mirror;

				Untransform(sDoc, &point, &transformation);
			}
		}

		attrib->setCoordinate(point.x,point.y);
		attrib->setRotationRadians(attribRotation);
		attrib->setHeight(figureGroup->GetFontHeight());
		attrib->setWidth(figureGroup->GetFontWidth());
		attrib->setPenWidthIndex(figureGroup->GetWidthIndex());
		attrib->setVisible(figureGroup->GetVisible());
		attrib->setProportionalSpacing(true);
		attrib->setMirrorDisabled(true);

		if (attrib->isVisible())
			attrib->setLayerIndex( Graph_Level(layerName, "AttrLyr_", FALSE));

		if (figureGroup->GetHorizontalAlignment() > -1)
		{
			attrib->setHorizontalPosition(intToHorizontalPositionTag(figureGroup->GetHorizontalAlignment()));
		}

		if (figureGroup->GetVerticalAlignment() > -1)
			attrib->setVerticalPosition(intToVerticalPositionTag(figureGroup->GetVerticalAlignment()));

		//display = display->GetNextDisplay(); <== code for multiple display
	}

	return 1;
}


////////////////////////////////////////////////////////////////////////////////
// CFigureGroup
////////////////////////////////////////////////////////////////////////////////
CFigureGroup::CFigureGroup()
{
   m_iColor = 0;
   m_iHorizontalAlignment = 0;
   m_iVerticalAlignement = 0;
   m_iWidthIndex = 0;
   m_dFontHeight = 0;
   m_dFontWidth = 0;
	m_bVisible = true;
}

CFigureGroup::CFigureGroup(const CFigureGroup& other)
{
	if (this != &other)
	{
		m_iColor = other.m_iColor;
		m_iHorizontalAlignment = other.m_iHorizontalAlignment;
		m_iVerticalAlignement = other.m_iVerticalAlignement;
		m_iWidthIndex = other.m_iWidthIndex;
		m_dFontHeight = other.m_dFontHeight;
		m_dFontWidth = other.m_dFontWidth;
		m_bVisible = other.m_bVisible;
	}
}

CFigureGroup::~CFigureGroup()
{
}

void CFigureGroup::Reset()
{
   m_iColor = 0;
   m_iHorizontalAlignment = 0;
   m_iVerticalAlignement = 0;

	// Make the width a zero width
   int err;
	double width = 0.0;
   CString widthName;
   widthName.Format("PathWidth_%f", width);
   m_iWidthIndex = Graph_Aperture(widthName, T_ROUND, width, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err); ;

   m_dFontHeight = 0;
   m_dFontWidth = 0;
	m_bVisible = true;
}


////////////////////////////////////////////////////////////////////////////////
// CTransform
////////////////////////////////////////////////////////////////////////////////
CTransform::CTransform()
{
	m_dScaleX = 1.0;
	m_dScaleY = 1.0;
	m_dRotation = 0;
	m_pOrigin = NULL;
}

CTransform::CTransform(const CTransform& other)
{
	m_dScaleX = 1.0;
	m_dScaleY = 1.0;
	m_dRotation = 0;
	m_pOrigin = NULL;

	if (this != &other)
	{
		m_dScaleX = other.m_dScaleX;
		m_dScaleY = other.m_dScaleY;
		m_dRotation = other.m_dRotation;

		if (other.m_pOrigin != NULL)
			m_pOrigin = new CPnt(*other.m_pOrigin);
	}
}

CTransform::~CTransform()
{
	delete m_pOrigin;
	m_pOrigin = NULL;
}

void CTransform::Reset()
{
	m_dScaleX = 1.0;
	m_dScaleY = 1.0;
	m_dRotation = 0;

	delete m_pOrigin;
	m_pOrigin = NULL;
}

void CTransform::SetOrigin(const CPnt& origin)
{ 
	if (m_pOrigin != NULL)
	{
		delete m_pOrigin;
		m_pOrigin = NULL;
	}	

	m_pOrigin = new CPnt(origin);
}


////////////////////////////////////////////////////////////////////////////////
// CDisplay
////////////////////////////////////////////////////////////////////////////////
CDisplay::CDisplay()
{
	m_pFigureGroup = NULL;
	m_pTransform = NULL;
	m_pNext = NULL;
}

CDisplay::CDisplay(const CDisplay& other)
{
	m_pFigureGroup = NULL;
	m_pTransform = NULL;
	m_pNext = NULL;

	if (this != &other)
	{
		if (other.m_pFigureGroup != NULL)
			m_pFigureGroup = new CFigureGroup(*other.m_pFigureGroup);

		if (other.m_pTransform != NULL)
			m_pTransform = new CTransform(*other.m_pTransform);

		if (other.m_pNext != NULL)
			m_pNext = new CDisplay(*other.m_pNext);
	}
}

CDisplay::~CDisplay()
{
	delete m_pFigureGroup;
	m_pFigureGroup = NULL;

	delete m_pTransform;
	m_pTransform = NULL;

	delete m_pNext;
	m_pNext = NULL;
}

void CDisplay::SetNextDisplay(CDisplay* display)
{
	display->m_pNext = m_pNext;
	m_pNext = display;
}

void CDisplay::SetFigureGroup(CFigureGroup* figureGroup)
{
	delete m_pFigureGroup;
	m_pFigureGroup = figureGroup;
}

void CDisplay::SetTransform(CTransform* transform)
{
	delete m_pTransform;
	m_pTransform = transform;
}



DataStruct* findHierarchySymbol(CString geomNums, CString instanceName)
{
	int curPos = 0;
	CString geomNum = geomNums.Tokenize(" ", curPos);
	while (!geomNum.IsEmpty())
	{
      BlockStruct *block = sDoc->getBlockAt(atoi(geomNum));
		geomNum = geomNums.Tokenize(" ", curPos);

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *tmpData = block->getDataList().GetNext(dataPos);
         if (tmpData->getDataType() != T_INSERT)
            continue;

         // Find the insert with the same name as the instance referenced
         if (strcmp(tmpData->getInsert()->getRefname(), instanceName))
            continue;

         return tmpData;
      }
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// CEdifPort
//-----------------------------------------------------------------------------
CEdifPort::CEdifPort()
{
   attribMap = NULL;
	isBundle = false;
   portRefArr.RemoveAll();
}

CEdifPort::~CEdifPort()
{
	if (attribMap != NULL)
		delete attribMap;
   attribMap = NULL;
   portRefArr.RemoveAll();
}

//-----------------------------------------------------------------------------
// CLeafAnnotate
//-----------------------------------------------------------------------------
CLeafAnnotate::CLeafAnnotate() 
{
	m_pAttribMap = NULL;
	reset();
}

CLeafAnnotate::~CLeafAnnotate()
{
	reset();
}

void CLeafAnnotate::reset()
{
	m_sName.Empty();
	m_sDesignator.Empty();
	
	delete m_pAttribMap;	
	m_pAttribMap = NULL;
	m_portMap.empty();
}

int CLeafAnnotate::DoLeafAnnotate()
{
	reset();

   // remove all the ports from previous interface
   sPortMap.empty();

   int res;
   if ((res = GetString(m_sName)) < 0)
      return res;
   else if (m_sName == "")
   {
      // there is no instance name so skip
      skipCommand();
      return 0;
   }

   m_pAttribMap = new CAttributes();
   sAttribMapList.AddTail(m_pAttribMap);
   sDesList.AddTail(m_sDesignator);

   res = LoopCommand(leafOccurrenceAnnotate_lst, SIZ_LEAFOCCURRENCEANNOTATE_LST);

   m_pAttribMap = sAttribMapList.RemoveTail();
   m_sDesignator = sDesList.RemoveTail();

	// Save the port from sPortMap to local m_portMap
	for (POSITION pos = sPortMap.GetStartPosition(); pos != NULL;)
	{
		CString key;
		CEdifPort* port = NULL;
		sPortMap.GetNextAssoc(pos, key, port);

		m_portMap.SetAt(key, port);
	}
	sPortMap.RemoveAll();

	return res;
}

void CLeafAnnotate::UpdateLogicSymbol(CInstance* instance)
{
   if (instance == NULL)
      return;

   // Update designator to instance and to attribute map
   if (!m_sDesignator.Trim().IsEmpty())
   {
      instance->setDesignator(m_sDesignator);

      Attrib* attrib;
      int designatorKw = sDoc->RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString);
      sDoc->SetAttrib(&m_pAttribMap, designatorKw, valueTypeString, m_sDesignator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
      attrib->setFlagBits(ATTRIB_CHANGE_VALUE);
   }

   // Update attribute map to insert
   if (instance->getInstanceDataStruct() != NULL)
   {
      CopyANDRemoveAttribMap(&instance->getInstanceDataStruct()->getAttributesRef(), m_pAttribMap);
   }

	if (m_portMap.GetCount())
	{
	   CEdifCluster* instanceCluster = edifStruct.findCluster(instance->getLibraryRef(), instance->getCellRef(), instance->getClusterRef());
      if (cluster == NULL)
         return;

	   CEdifSymbol* symbol = instanceCluster->findSymbol(instance->getSymbolRef());
      if (symbol == NULL || symbol->getSymbolBlock() == NULL)
         return;
      BlockStruct* symbolBlock = symbol->getSymbolBlock();

      CEdifCluster* symbolCluster = edifStruct.findCluster(symbol->getLibraryName(), symbol->getCellName(), symbol->getClusterName());
      if (symbolCluster == NULL)
         return;

      // Create name for new symbol
      int count = 0;
		CString newSymbolName;
		newSymbolName.Format("%s_%d", symbol->getName(), ++count);
		while (symbolCluster->findSymbol(newSymbolName))
		{
			newSymbolName.Format("%s_%d", symbol->getName(), ++count);
		}

      // Create new symbol
		CEdifSymbol* newSymbol = symbolCluster->addSymbol(newSymbolName);
		instance->setSymbolRef(newSymbolName);

      // Create new symbol block
      BlockStruct* newSymbolBlock = Graph_Block_On(GBO_APPEND, newSymbol->generateSymbolBlockName(), sFileStruct->getFileNumber(), 0L);
      newSymbolBlock->setBlockType(symbolBlock->getBlockType());
      Graph_Block_Off();
      sDoc->CopyAttribs(&newSymbolBlock->getAttributesRef(), symbolBlock->getAttributesRef());

      // Assign newSymbolBlock to newSymbol and instance insert
      newSymbol->setSymbolBlock(newSymbolBlock);
      instance->getInstanceDataStruct()->getInsert()->setBlockNumber(newSymbolBlock->getBlockNumber());

      // Copy all data from symbolBlock to newSymbolBlock
      for (CDataListIterator dataList(*symbolBlock); dataList.hasNext();)
      {
         DataStruct* newData = CopyTransposeEntity(dataList.getNext(), 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
         newSymbolBlock->getDataList().AddTail(newData);
         if (newData->getDataType() != dataTypeInsert || newData->getInsert()->getInsertType() != insertTypePortInstance)
            continue;

         CString portName = getValueFromPortNameAttrib(newData->getAttributes());
			CEdifPort* port = NULL;
         if (m_portMap.Lookup(portName, port))
         {
            CopyANDRemoveAttribMap(&newData->getAttributesRef(), port->attribMap);

            CInstancePort* instancePort = instance->findInstancePort(portName);
            if (instancePort !=  NULL)
            {
               CString portDesignator = getValueFromDesignatorAttrib(port->attribMap);
               if (!portDesignator.Trim().IsEmpty())
                  instancePort->setDesignator(portDesignator);

               CString portPrimaryName = getValueFromPrimaryNameAttrib(port->attribMap);
               if (!portPrimaryName.Trim().IsEmpty())
                  instancePort->setPrimaryName(portPrimaryName);
            }
         }        

			CSymbolPort* symbolPort = newSymbol->addPort(newData->getInsert()->getRefname());
			symbolPort->setPortRef(portName);
			symbolPort->setPortDataStruct(newData);
      }
   }   
}

void CLeafAnnotate::UpdateLogicSymbol(DataStruct* logicSymbol)
{
	// Update name of leafAnnotate
	m_sName = logicSymbol->getInsert()->getRefname();

	if (m_portMap.GetCount())
	{
      BlockStruct *block = sDoc->getBlockAt(logicSymbol->getInsert()->getBlockNumber());

      int count = 1;
      CString newBlockName;
      newBlockName.Format("%s_%d", block->getName(), count);
      newBlockName.MakeUpper();
      while (Graph_Block_Exists(sDoc, newBlockName, block->getFileNumber()))
      {
         newBlockName.Format("%s_%d", block->getName(), ++count);
         newBlockName.MakeUpper();
      }

      BlockStruct *newBlock = Graph_Block_On(GBO_APPEND, newBlockName, block->getFileNumber(), 0L);
      newBlock->setBlockType(block->getBlockType());
      Graph_Block_Off();

      sDoc->CopyAttribs(&newBlock->getAttributesRef(), block->getAttributesRef());

      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *data = block->getDataList().GetNext(pos);
         DataStruct *copy = CopyTransposeEntity(data, 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
         newBlock->getDataList().AddTail(copy);

         if (copy->getDataType() != T_INSERT || copy->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
            continue;

         Attrib* attrib;
         int keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);

         if (!copy->getAttributesRef() || !copy->getAttributesRef()->Lookup(keyword, attrib))
            continue;

         if (attrib)
         {
            CString portName = GetAttribStringValue(attrib);
				CEdifPort* port = NULL;

            if (m_portMap.Lookup(portName, port))
            {
               CopyANDRemoveAttribMap(&copy->getAttributesRef(), port->attribMap);
            }
         }
      }

      if (newBlock->getAttributesRef())
      {
         // loop through block's attributes
         POSITION attribPos = newBlock->getAttributesRef()->GetStartPosition();
         while (attribPos)
         {
            WORD keyword;
            Attrib *blockAttrib;

            newBlock->getAttributesRef()->GetNextAssoc(attribPos, keyword, blockAttrib);

            Attrib* insertAttrib;

            // if insert does not have this attrib, add it
            if (!logicSymbol->getAttributesRef() || !logicSymbol->getAttributesRef()->Lookup(keyword, insertAttrib)) 
            {
               insertAttrib->setInherited(true);     // set the inherited flag

               if (!logicSymbol->getAttributesRef())
                  logicSymbol->getAttributesRef() = new CAttributes();

               logicSymbol->getAttributesRef()->SetAt(keyword, insertAttrib);
            }
         }
      }

      logicSymbol->getInsert()->setBlockNumber(newBlock->getBlockNumber());
	}

   if (m_sDesignator != "")
   {
      Attrib* attrib;
      int designatorKw = sDoc->RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString);
      sDoc->SetAttrib(&m_pAttribMap, designatorKw, valueTypeString, m_sDesignator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
      attrib->setFlagBits(ATTRIB_CHANGE_VALUE);
   }

   CopyANDRemoveAttribMap(&logicSymbol->getAttributesRef(), m_pAttribMap);
}

//-----------------------------------------------------------------------------
// CSignalAnnotate
//-----------------------------------------------------------------------------
CSignalAnnotate::CSignalAnnotate()
{
   m_pAttribMap = NULL;
}

CSignalAnnotate::~CSignalAnnotate()
{
   delete m_pAttribMap;
   m_pAttribMap = NULL;
}

int CSignalAnnotate::doSignalAnnotate()
{
   int res;
   if ((res = GetString(m_sName)) < 0)
      return res;
   else if (m_sName == "")
   {
      // there is no instance name so skip
      skipCommand();
      return 0;
   }

   m_pAttribMap = new CAttributes();
   sAttribMapList.AddTail(m_pAttribMap);

   res = LoopCommand(leafOccurrenceAnnotate_lst, SIZ_LEAFOCCURRENCEANNOTATE_LST);

   m_pAttribMap = sAttribMapList.RemoveTail();

   return res;
}

void CSignalAnnotate::updateSingal(CSignal* signal)
{
   if (signal == NULL)
      return;

   CopyANDRemoveAttribMap(&signal->getAttributesRef(), m_pAttribMap);      
}

//-----------------------------------------------------------------------------
// COccurrenceAnnotate
//-----------------------------------------------------------------------------
COccurrenceAnnotate::COccurrenceAnnotate(bool useAsHierarchyAnnotate) : m_leafAnnotateMap(nextPrime2n(20),false)
{
	m_pAttribMap = NULL;
	reset();
	m_bUseAsHierarchyAnnotate = useAsHierarchyAnnotate;
}

COccurrenceAnnotate::~COccurrenceAnnotate()
{
	reset();
}

void COccurrenceAnnotate::reset()
{
	m_sName.Empty();
	m_sDesignator.Empty();
	m_sHierarchGeomNums.Empty();
	
	delete m_pAttribMap;	
	m_pAttribMap = NULL;
	m_pHierarchySymbol = NULL;

	m_portMap.empty();
	m_leafAnnotateMap.empty();
   m_signalfAnnotateMap.empty();

   m_refView = NULL;
   m_refInstance = NULL;
   m_clusterConfiguration = NULL;

   m_bUseAsHierarchyAnnotate = false;
}

void COccurrenceAnnotate::AddLeafAnnotate(CLeafAnnotate* leafAnnotate)
{
	m_leafAnnotateMap.SetAt(leafAnnotate->GetName(), leafAnnotate);
}

void COccurrenceAnnotate::addSignalAnnotate(CSignalAnnotate* signalAnnotate)
{
   m_signalfAnnotateMap.SetAt(signalAnnotate->GetName(), signalAnnotate);
}

int COccurrenceAnnotate::DoOccurrenceAnnotate()
{
   sPortMap.empty();

	int res = 0;

   m_pAttribMap = new CAttributes();
   sAttribMapList.AddTail(m_pAttribMap);
   sDesList.AddTail(m_sDesignator);

   res = LoopCommand(occurrenceAnnotate_lst, SIZ_OCCURRENCEANNOTATE_LST); 

   m_pAttribMap = sAttribMapList.RemoveTail();
   m_sDesignator = sDesList.RemoveTail();

	// Move the port from sPortMap to local m_portMap
	for (POSITION pos = sPortMap.GetStartPosition(); pos != NULL;)
	{
		CString key;
		CEdifPort* port = NULL;
		sPortMap.GetNextAssoc(pos, key, port);
		m_portMap.SetAt(key, port);
	}
	sPortMap.RemoveAll();

   if (!m_bUseAsHierarchyAnnotate && m_refInstance != NULL)
   {
      applyOccurrenceAnnotateToInstance(m_refInstance);
   }
   else if (m_clusterConfiguration != NULL)
   {      
      applyOccurrenceAnnotateToView(m_clusterConfiguration->getView());
   }


	return res;
}

void COccurrenceAnnotate::applyOccurrenceAnnotateToView(CEdifView* view)
{
   if (view == NULL)
      return;

   // Update annotation to instances
   for (POSITION instancePos = view->getInstanceStartPosition(); instancePos != NULL;)
   {
      CInstance* instance = view->getNextInstance(instancePos);
      if (instance == NULL || instance->isHierarchSymbol())
         continue;

		CLeafAnnotate* leafAnnotate = NULL;
		if (m_leafAnnotateMap.Lookup(instance->getName(), leafAnnotate))
      {
			leafAnnotate->UpdateLogicSymbol(instance);
      }
   }

   // Update annotation to globalPorts
   for (POSITION globalPortPos = view->getGlobalPortStartPosition(); globalPortPos != NULL;)
   {
      CPort* globalPort = view->getNextGlobalPort(globalPortPos);
      if (globalPort == NULL)
         continue;

		CEdifPort* port = NULL;
		if (m_portMap.Lookup(globalPort->getPortRef(), port))
      {
 			CopyANDRemoveAttribMap(&globalPort->getPortDataStruct()->getAttributesRef(), port->attribMap);           
         
         CString designator = getValueFromDesignatorAttrib(port->attribMap);       
         if (!designator.Trim().IsEmpty())
         {
            globalPort->setDesignator(designator);
         }

         CString primaryName = getValueFromPrimaryNameAttrib(port->attribMap);
         if (!primaryName.Trim().IsEmpty())
         {
            globalPort->setPrimaryName(primaryName);
         }
      }
   }

   // Update annotation to masterPorts
   for (POSITION masterPortPos = view->getMasterPortStartPosition(); masterPortPos != NULL;)
   {
      CPort* masterPort = view->getNextMasterPort(masterPortPos);
      if (masterPort == NULL)
         continue;

		CEdifPort* port = NULL;
		if (m_portMap.Lookup(masterPort->getPortRef(), port))
      {
 			CopyANDRemoveAttribMap(&masterPort->getPortDataStruct()->getAttributesRef(), port->attribMap);           
         
         CString designator = getValueFromDesignatorAttrib(port->attribMap);       
         if (!designator.Trim().IsEmpty())
         {
            masterPort->setDesignator(designator);
         }

         CString primaryName = getValueFromPrimaryNameAttrib(port->attribMap);
         if (!primaryName.Trim().IsEmpty())
         {
            masterPort->setPrimaryName(primaryName);
         }
      }
   }

   // Update annotation to nets
   for (POSITION netPos = view->getNetStartPosition(); netPos != NULL;)
   {
      CNet* net = view->getNextNet(netPos);
      if (net == NULL)
         continue;

      CSignalAnnotate* signalAnnotate = NULL;
      if (m_signalfAnnotateMap.Lookup(net->getName(), signalAnnotate))
      {
         signalAnnotate->updateSingal(net);
      }
   }

   // Update annotation to busses
   for (POSITION busPos = view->getBusStartPosition(); busPos != NULL;)
   {
      CBus* bus = view->getNextBus(busPos);
      if (bus == NULL)
         continue;

      CSignalAnnotate* signalAnnotate = NULL;
      if (m_signalfAnnotateMap.Lookup(bus->getName(), signalAnnotate))
      {
         signalAnnotate->updateSingal(bus);
      }
   }
}

void COccurrenceAnnotate::applyOccurrenceAnnotateToInstance(CInstance* instance)
{
   if (instance == NULL)
      return;

   // Update annotation to instance of hierarchy symbol
   if (!m_sDesignator.Trim().IsEmpty())
   {
      instance->setDesignator(m_sDesignator);

      Attrib* attrib;
      int designatorKw = sDoc->RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString);
      sDoc->SetAttrib(&m_pAttribMap, designatorKw, valueTypeString, m_sDesignator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
      attrib->setFlagBits(ATTRIB_CHANGE_VALUE);
   }

   CString primaryName = getValueFromPrimaryNameAttrib(m_pAttribMap);
   if (!primaryName.Trim().IsEmpty())
   {
      instance->setPrimaryName(primaryName);
   }

   if (instance->getInstanceDataStruct() != NULL)
   {
      CopyANDRemoveAttribMap(&instance->getInstanceDataStruct()->getAttributesRef(), m_pAttribMap);
   }

   // Loop through all view and apply annotation
   CEdifCluster* cluster = edifStruct.findCluster(instance->getLibraryRef(), instance->getCellRef(), instance->getClusterRef());
   for (POSITION pos = cluster->getViewStartPosition(); pos != NULL;)
   {
      CEdifView* view = cluster->getNextView(pos);
      applyOccurrenceAnnotateToView(view);
   }
}

#ifdef DEADCODE
void COccurrenceAnnotate::updateHierarchyGeometry()
{
	CMapStringToString netNameMap;
	CMapStringToString refNameMap;
	int netNameKey = sCamCadDatabase->getKeywordIndex(ATT_NETNAME);

	int curPos = 0;
	CString geomNum = m_sHierarchGeomNums.Tokenize(" \n", curPos);
	while (!geomNum.IsEmpty())
	{
      BlockStruct *pageBlock = sDoc->getBlockAt(atoi(geomNum));

		// Get next geometry number
		geomNum = m_sHierarchGeomNums.Tokenize(" ", curPos);

		if (pageBlock == NULL) 
			continue;
	
		int index = pageBlock->getName().ReverseFind('_');
      int count = atoi(pageBlock->getName().Right(pageBlock->getName().GetLength()-index-1));

		POSITION pos = pageBlock->getDataList().GetHeadPosition();
		while (pos)
		{
			DataStruct *data = pageBlock->getDataList().GetNext(pos);
			if (data == NULL)
				continue;

			if (data->getDataType() == dataTypeInsert)
			{
				CString oldRefName = data->getInsert()->getRefname();
				CString newRefName;
				newRefName.Format("%s_%d", oldRefName, count);
				
				refNameMap.SetAt(oldRefName, newRefName);
				data->getInsert()->setRefname(strdup(newRefName));

				if (data->getInsert()->getInsertType() == INSERTTYPE_PORTINSTANCE)
				{
					int keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);
					Attrib* attrib = NULL;
					if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(keyword, attrib) || attrib == NULL)
						continue;

					CString portName = GetAttribStringValue(attrib);
					CEdifPort* port = NULL;
					if (m_portMap.Lookup(portName, port))
					{
						CopyANDRemoveAttribMap(&data->getAttributesRef(), port->attribMap);
					}
				}
				else if (data->getInsert()->getInsertType() == insertTypeSymbol)
				{
					CLeafAnnotate* leafAnnotate = NULL;
					if (m_leafAnnotateMap.Lookup(oldRefName, leafAnnotate))
						leafAnnotate->UpdateLogicSymbol(data);
				}
			}
			else if (data->getDataType() == dataTypePoly && data->getAttributesRef() != NULL)
			{
				CString netName;
				if (!sCamCadDatabase->getAttributeStringValue(netName, &data->getAttributesRef(), netNameKey))
					continue;

				netNameMap.SetAt(netName, netName);
			}
		}
   }

	// Update netlist with new compping
	for (POSITION pos = netNameMap.GetStartPosition(); pos != NULL;)
	{
		CString key, netName;
		netNameMap.GetNextAssoc(pos, key, netName);
		NetStruct* net = sCamCadDatabase->getNet(netName, sFileStruct);
		if (net == NULL)
			continue;

		for(POSITION cpPos = net->getHeadCompPinPosition(); cpPos != NULL;)
		{
			CompPinStruct* cp = net->getNextCompPin(cpPos);
			if (cp == NULL)
				continue;

			CString refName;
			refNameMap.Lookup(cp->getRefDes(), refName);

			if (!refName.IsEmpty())
			{
				CString comppinToRemove;
				comppinToRemove.Format("%s%c%s", cp->getRefDes(), SCH_DELIMITER, cp->getPinName());
			}
		}
	}

	if (!m_bUseAsHierarchyAnnotate)
	{
		if (!m_sHierarchGeomNums.IsEmpty())
		{
			WORD geomNumKeywordIndex = sDoc->IsKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0);
#if CamCadMajorMinorVersion > 406  //  > 4.6
         m_pHierarchySymbol->setAttrib(sDoc->getCamCadData(), geomNumKeywordIndex, valueTypeString, m_sHierarchGeomNums.GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
			m_pHierarchySymbol->setAttrib(sDoc, geomNumKeywordIndex, valueTypeString, m_sHierarchGeomNums.GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
      }

		if (!m_sDesignator.IsEmpty())
		{
			Attrib* attrib;
			int keyword = sDoc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);
			sDoc->SetAttrib(&m_pAttribMap, keyword, valueTypeString, m_sDesignator.GetBuffer(0), attributeUpdateOverwrite, &attrib);
			attrib->setFlagBits(ATTRIB_CHANGE_VALUE);
		}

		CopyANDRemoveAttribMap(&m_pHierarchySymbol->getAttributesRef(), m_pAttribMap);
	}
}
#endif

//-----------------------------------------------------------------------------
// CPort
//-----------------------------------------------------------------------------
CPort::CPort(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString pageName, const CString name, const EConnectorType connectorType)
	: CConnectorObject(edif, libraryName, cellName, clusterName, viewName, pageName, name, connectorType)
{
	reset();
}
CPort::~CPort()
{
	reset();
}

void CPort::reset()
{
	m_portRef.Empty();
	m_portDataStruct = NULL;
	m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CInstancePort
//-----------------------------------------------------------------------------
CInstancePort::CInstancePort(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString instanceName, const CString name, const EConnectorType connectorType)
	: CConnectorObject(edif, libraryName, cellName, clusterName, viewName, "", name, connectorType)
	, m_instanceName(instanceName)
{
	reset();
}

CInstancePort::CInstancePort(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString instanceName, const CInstancePort& instancePort)
	: CConnectorObject(edif, libraryName, cellName, clusterName, viewName, "", instancePort.m_name, instancePort.m_connectorType)
	, m_instanceName(instanceName)
{
   m_designator = instancePort.m_designator;
   m_connectedToSignal = NULL;
}

CInstancePort::~CInstancePort()
{
	reset();
}

void CInstancePort::reset()
{
	m_instanceName.Empty();
	m_designator.Empty();
	m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CPageConnector
//-----------------------------------------------------------------------------
CPageConnector::CPageConnector(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString pageName, const CString name, const EConnectorType connectorType)
: CConnectorObject(edif, libraryName, cellName, clusterName, viewName, pageName, name, connectorType)
{
	reset();
}

CPageConnector::~CPageConnector()
{
	reset();
}

void CPageConnector::reset()
{
	m_connectorDataStruct = NULL;
	m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CRipperHotspot
//-----------------------------------------------------------------------------
CRipperHotspot::CRipperHotspot(const CString name)
{
   reset();
   m_name = name;
}

CRipperHotspot::~CRipperHotspot()
{
   reset();
}

void CRipperHotspot::reset()
{
   m_pin.Empty();
   m_rule.Empty();
   m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CRipper
//-----------------------------------------------------------------------------
CRipper::CRipper(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString pageName, const CString name, const EConnectorType connectorType)
: CConnectorObject(edif, libraryName, cellName, clusterName, viewName, pageName, name, connectorType),
  m_ripperHotspotMap(nextPrime2n(20), true)
{
   m_attribMap = NULL;
	reset();
}

CRipper::~CRipper()
{
	reset();
}

void CRipper::reset()
{
	m_wireRule.Empty();
   m_hotspotConnectedToBus.Empty();
	m_connectedToSignal1 = NULL;
	m_connectedToSignal2 = NULL;
   m_ripperHotspotMap.empty();

   delete m_attribMap;
   m_attribMap = NULL;
}

CRipperHotspot* CRipper::addRipperHotspot(const CString ripperHotspotName)
{  
   CRipperHotspot* hotspot = findRipperHotspot(ripperHotspotName);
   if (hotspot == NULL)
   {
      hotspot = new CRipperHotspot(ripperHotspotName);
      m_ripperHotspotMap.SetAt(ripperHotspotName, hotspot);
   }
   
   return hotspot;
}

CRipperHotspot* CRipper::findRipperHotspot(const CString ripperHotspotName)
{  
   CRipperHotspot* hotspot = NULL;
   m_ripperHotspotMap.Lookup(ripperHotspotName, hotspot);
   return hotspot;
}


CNet* CRipper::getConnectedToNet(CNet* fromNet)
{
   CNet* connectedNet = NULL;
	if (fromNet == NULL || fromNet->getSignalType() != signalTypeNet || m_ripperHotspotMap.GetCount() > 2)
	{
		return NULL;
	}
   else
   {
      for (POSITION pos=getRipperHotspotStartPosition();  pos != NULL;)
      {
         CRipperHotspot* hotspot = getNextRipperHotspot(pos);
         if (hotspot == NULL || hotspot->getConnectedToSignal() == NULL)
            continue;

         if (hotspot->getConnectedToSignal() == fromNet)
            continue;

         if (hotspot->getConnectedToSignal()->getSignalType() != signalTypeNet)
            continue;

         connectedNet = (CNet*)hotspot->getConnectedToSignal();
         break;
      }
   }

   return connectedNet;

	//if (fromNet == NULL || fromNet->getSignalType() != signalTypeNet)
	//{
	//	return NULL;
	//}
	//else if (fromNet == m_connectedToSignal1)
	//{
	//	if (m_connectedToSignal2 == NULL || m_connectedToSignal2->getSignalType() != signalTypeNet)
	//	{
	//		return NULL;
	//	}
	//	else
	//	{
	//		return (CNet*)m_connectedToSignal2;
	//	}
	//}
	//else if (fromNet == m_connectedToSignal2)
	//{
	//	if (m_connectedToSignal1 == NULL || m_connectedToSignal1->getSignalType() != signalTypeNet)
	//	{
	//		return NULL;
	//	}
	//	else
	//	{
	//		return (CNet*)m_connectedToSignal1;
	//	}
	//}
	//else
	//{
	//	return NULL;
	//}
}

//-----------------------------------------------------------------------------
// CSignal
//-----------------------------------------------------------------------------
CSignal::CSignal(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name, const ESignalType signalType)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
	, m_viewName(viewName)
	, m_name(name)
	, m_signalType(signalType)
   , m_instancePortMap(nextPrime2n(20), false)
	, m_globalPortMap(nextPrime2n(20),false)
	, m_masterPortMap(nextPrime2n(20),false)
	, m_onPageConnectorMap(nextPrime2n(20),false)
	, m_offPageConnectorMap(nextPrime2n(20),false)
	, m_ripperMap(nextPrime2n(20),false)
{
   m_attribMap = NULL;
	reset();
}

CSignal::~CSignal()
{
	reset();
}

void CSignal::reset()
{
	m_primaryName.Empty();
	m_annotateName.Empty();
   m_renamedName.Empty();
	m_isNameUserDefined = false;
	m_isRenameDone = false;
	m_renamedBy = renamedByUndefined;

   delete m_attribMap;
   m_attribMap = NULL;

   m_instancePortMap.empty();
	m_globalPortMap.empty();
	m_masterPortMap.empty();
	m_onPageConnectorMap.empty();
	m_offPageConnectorMap.empty();
	m_ripperMap.empty();
}

void CSignal::prepareForRename()
{
	// Net must prepare for rename by applying annotated name or primary name when available
	
	WORD primaryNameKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_PRIMARYNAME, 0, valueTypeString);
	Attrib* attrib = NULL;
	if (m_attribMap != NULL)
	{
      if (m_attribMap->Lookup(primaryNameKw, attrib))
      {
		   m_primaryName = GetAttribStringValue(attrib);		
      }

      if (!sNetBackannotate.IsEmpty())
      {
         WORD netBackannotateKw = sDoc->RegisterKeyWord(sNetBackannotate, 0, valueTypeString);         
         if (m_attribMap->Lookup(netBackannotateKw, attrib))
         {
		      m_annotateName = GetAttribStringValue(attrib);		
         }
      }
	}



   if (!m_annotateName.IsEmpty())
      m_renamedName = m_annotateName;
   else if (!m_primaryName.IsEmpty())
      m_renamedName = m_primaryName; 
   else
      m_renamedName = m_name;
}

void CSignal::addInstancePort(CInstancePort*  instancePort)
{
   if (instancePort != NULL)
   {
      CString key;
      key.Format("%s_%s", instancePort->getInstnaceName(), instancePort->getName());
      m_instancePortMap.SetAt(key, instancePort);
   }
}

void CSignal::addGlobalPort(CPort* port)
{
   if (port != NULL)
	   m_globalPortMap.SetAt(port->getName(), port);
}

void CSignal::addMasterPort(CPort* port)
{
   if (port != NULL)
	   m_masterPortMap.SetAt(port->getName(), port);
}

void CSignal::addOnPageConnector(CPageConnector* connector)
{
   if (connector != NULL)
	   m_onPageConnectorMap.SetAt(connector->getName(), connector);
}

void CSignal::addOffPageConnector(CPageConnector* connector)
{
   if (connector != NULL)
	   m_offPageConnectorMap.SetAt(connector->getName(), connector);
}

void CSignal::addRipper(CRipper* ripper)
{
   if (ripper != NULL)
	   m_ripperMap.SetAt(ripper->getName(), ripper);
}

void CSignal::applyRenamedNameToConnectedNets(CNet* connectedFromNet, const CString renamedName, const ERenamedBy renamedBy)
{
	if (m_isRenameDone && (int)m_renamedBy <= (int)renamedBy)
	{
		return;
	}
	else
	{
		m_isRenameDone = true;
		m_renamedBy = renamedBy;
	}

	CString tempRename = renamedName;
	if (tempRename.Trim().IsEmpty())
		return;
	m_renamedName = tempRename;

	if (m_signalType == signalTypeNet)
	{
		// Need to rename all net connected by rippers

		for (POSITION pos = m_ripperMap.GetStartPosition(); pos != NULL;)
		{
			CRipper* ripper = NULL;
			CString key;
			m_ripperMap.GetNextAssoc(pos, key, ripper);
			if (ripper == NULL || ripper->getRipperHotspotCount() > 2)
				continue;

			CNet* connectedToNet = ripper->getConnectedToNet((CNet*)this);
         if (connectedToNet == NULL || connectedToNet == this || connectedToNet == connectedFromNet)
            continue;

         connectedToNet->applyRenamedNameToConnectedNets((CNet*)this, renamedName, renamedBy);
		}
	}
}

void CSignal::resetRenameDoneOnConnectedNets(CNet* connectedFromNet)
{
	m_isRenameDone = false;

	if (m_signalType == signalTypeNet)
	{
		// Need to set m_isRenameDone of all connected name to false before calling applyRenamedNameToConnectedNets()
		for (POSITION pos = m_ripperMap.GetStartPosition(); pos != NULL;)
		{
			CRipper* ripper = NULL;
			CString key;
			m_ripperMap.GetNextAssoc(pos, key, ripper);
			if (ripper == NULL)
				continue;

			CNet* connectedToNet = ripper->getConnectedToNet((CNet*)this);
			if (connectedToNet == NULL || connectedToNet == this || connectedToNet == connectedFromNet)
				continue;

			connectedToNet->resetRenameDoneOnConnectedNets((CNet*)this);
		}
	}
}

void CSignal::setRenamedName(const CString renamedName, const ERenamedBy renamedBy)
{
	//if (m_renamedBy != renamedByUndefined && m_renamedBy > renamedBy)
	//{
	//	resetRenameDoneOnConnectedNets(NULL);
	//}

	applyRenamedNameToConnectedNets(NULL, renamedName, renamedBy);
}

//-----------------------------------------------------------------------------
// CNet
//-----------------------------------------------------------------------------
CNet::CNet(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name, const ESignalType signalType)
	: CSignal(edif, libraryName, cellName, clusterName, viewName, name, signalType)
{
   reset();
}

CNet::CNet(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CNet& net)
	: CSignal(edif, libraryName, cellName, clusterName, viewName, net.m_name, net.m_signalType)
{
   reset();

   m_primaryName = net.m_primaryName;
   m_annotateName = net.m_annotateName;
   m_isNameUserDefined = net.m_isNameUserDefined;
   m_isRenameDone = net.m_isRenameDone;

	if (net.m_attribMap != NULL)
	{
		m_attribMap = new CAttributes(*net.m_attribMap);
	}
}

CNet::~CNet()
{
}

void CNet::reset()
{
}

bool CNet::isUserDefineName()
{
	// System defined netname always starts with N$

	CString name = m_renamedName;
	name.MakeUpper().Trim();
	if (name.Left(2) == ("N$"))
		return false;
	else
		return true;
}

CNet* CNet::getAliasRenamedNet(CNet* connectedFromNet)
{
	CString curRename = m_renamedName;
	CNet* curRenamedNet = this;

   for (POSITION pos = m_ripperMap.GetStartPosition(); pos != NULL;)
   {
      CRipper* ripper = NULL;
      CString key;
      m_ripperMap.GetNextAssoc(pos, key, ripper);
      if (ripper == NULL)
         continue;

		CNet* connectedToNet = ripper->getConnectedToNet(this);
      if (connectedToNet == NULL || connectedToNet == this || connectedToNet == connectedFromNet)
         continue;

		CNet* aliasRenamedNet = connectedToNet->getAliasRenamedNet(this);
		if (curRenamedNet->isUserDefineName() == aliasRenamedNet->isUserDefineName())
		{
			// Both name are either user-defined or system-defined, then choose the net that is renamed by
			// 1st Global
			// 2nd On/Off page connector
			// 3rd Master port
			// 4th Bus

			// If both nets are renamed by the same importance, then choose name base on the rule of "come earlier in alphabet"
			if (aliasRenamedNet->getRenamedBy() < curRenamedNet->getRenamedBy())
			{
				// The rename of aliasRenamedNet is more importance
				curRenamedNet = aliasRenamedNet;
			}
			else if (aliasRenamedNet->getRenamedBy() == curRenamedNet->getRenamedBy() && 
						aliasRenamedNet->getRenamedName().CompareNoCase(curRenamedNet->getRenamedName()) < 0)
			{
				// aliasRenamedNet comes earlier in alphabet so choose it
				curRenamedNet = aliasRenamedNet;
			}
		}
		else if (aliasRenamedNet->isUserDefineName())
		{
			// only aliasRenamedNet is user-defined so choose it
			curRenamedNet = aliasRenamedNet;
		}
	}

	return curRenamedNet;
}

CString CNet::getAliasRename()
{
	CNet* aliasRenamedNet = getAliasRenamedNet(this);
	CString aliasRename = aliasRenamedNet->getRenamedName();

	if (aliasRename.IsEmpty())
	{
		aliasRename = m_renamedName;
	}

	if (aliasRename.CompareNoCase(aliasRenamedNet->getAnnotateName()) != 0)
	{
		// This net is a local net and its name has not been annotated; therefore, 
		// it need to get a unique net name
		aliasRename = m_edif.getUniqueLocalNetName(aliasRename);
	}

	//CEdifCluster* cluster = m_edif.findCluster(m_libraryName, m_cellName, m_clusterName);
	//if (cluster->getDerivedNumber() > 0 && aliasRename.CompareNoCase(aliasRenamedNet->getAnnotateName()) != 0)
	//{
	//	// This net belong in a cloned or derived cluster and its name has not been annotated;
	//	// therefore, need to create unique net name for it by appending the derived number of the cluster

	//	aliasRename.AppendFormat("_%d", cluster->getDerivedNumber());
	//}

	return aliasRename;
}

//-----------------------------------------------------------------------------
// CBus
//-----------------------------------------------------------------------------
CBus::CBus(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name, const ESignalType signalType)
	: CSignal(edif, libraryName, cellName, clusterName, viewName, name, signalType)
	, m_netMap(nextPrime2n(20),false)
	, m_busMap(nextPrime2n(20),false)
{
   m_enumNameArr.SetSize(0, 20);
	reset();
}

CBus::CBus(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, CEdifView& view,  const CBus& bus)
	: CSignal(edif, libraryName, cellName, clusterName, view.getName(), bus.m_name, bus.m_signalType)
	, m_netMap(nextPrime2n(20),false)
	, m_busMap(nextPrime2n(20),false)
{
   m_enumNameArr.SetSize(0, 20);
	reset();

   m_primaryName = bus.m_primaryName;
   m_annotateName = bus.m_annotateName;
   m_isNameUserDefined = bus.m_isNameUserDefined;
   m_isRenameDone = bus.m_isRenameDone;

	if (bus.m_attribMap != NULL)
	{
		m_attribMap = new CAttributes(*bus.m_attribMap);
	}

   for (POSITION pos = bus.m_netMap.GetStartPosition(); pos != NULL;)
   {
      CNet* otherNet = NULL;
      CString otherNetName;
      
      bus.m_netMap.GetNextAssoc(pos, otherNetName, otherNet);
      if (otherNet == NULL)
         continue;

      // Cannot copy the pointer of the net because it is belong to another map on a different view.
      // Only get the net name and find it on view that this bus belong to.  If the view doesn't have the net, it will creat one.
      CNet* net = view.addNet(otherNetName);
      addNet(net);
   }

   for (POSITION pos = bus.m_busMap.GetStartPosition(); pos != NULL;)
   {
      CBus* otherBus = NULL;
      CString otherBusName;

      bus.m_busMap.GetNextAssoc(pos, otherBusName, otherBus);
      if (otherBus == NULL)
         continue;

      CBus* bus = view.addBus(otherBusName);
      addBus(bus);
   }
}

CBus::~CBus()
{
   reset();
}

void CBus::reset()
{
   m_enumNameArr.RemoveAll();
	m_netMap.empty();
	m_busMap.empty();
};

void CBus::enumerateBusName()
{
	CString leftDelimiter;
	CString rightDelimiter;
	int index = -1;

	CString busName = m_name;
	if (!m_primaryName.IsEmpty())
		busName = m_primaryName;
   if (!m_annotateName.IsEmpty())
      busName = m_annotateName;
	
	if ((index = busName.ReverseFind('[')) >= 0)
	{
		leftDelimiter = "[";
		rightDelimiter = "]";
	}
	else if ((index = busName.ReverseFind(')')) >= 0)
	{
		leftDelimiter = "(";
		rightDelimiter = ")";
	}
	else if (index < 0)
		return;

	int lbIndex = -1;
	int ubIndex = -1;
	int curPos = 0;
	CString baseName = busName.Tokenize(leftDelimiter, curPos);
	if (!baseName.IsEmpty())
	{
		CString range = busName.Tokenize(":", curPos); 
		if (is_number(range))
		{
			lbIndex = atoi(range);
			ubIndex = lbIndex;	// initial it to lower bound

			range = busName.Tokenize(rightDelimiter, curPos);
			if (is_number(range))
				ubIndex = atoi(range);
		}
	}

	if (lbIndex > -1 && ubIndex > -1)
	{
		if (lbIndex > ubIndex)
		{
			int tmp = lbIndex;
			lbIndex = ubIndex;
			ubIndex = tmp;
		}

		CString enumName;

		for (int i=lbIndex; i<=ubIndex; i++)
		{
			enumName.Format("%s%s%d%s", baseName, leftDelimiter, i, rightDelimiter);
			m_enumNameArr.SetAtGrow(i, enumName);
		}
	}
}

CString CBus::getEnumeratedName(CString wireRule)
{
   if (m_enumNameArr.GetCount() == 0)
      enumerateBusName();

	CString enumName;

	if (!wireRule.IsEmpty() && is_number(wireRule) && m_enumNameArr.GetCount() > 0)
	{
		if (atoi(wireRule) < m_enumNameArr.GetCount())
			enumName = m_enumNameArr.GetAt(atoi(wireRule));
	}

	return enumName;
}

void CBus::addBus(CBus* bus)
{
   if (bus != NULL)
		m_busMap.SetAt(bus->getName(), bus);
}

void CBus::addNet(CNet* net)
{
   if (net != NULL)
		m_netMap.SetAt(net->getName(), net);
}

void CBus::applyBusDerivedNametoConnectedNets()
{
   for (POSITION pos = m_ripperMap.GetStartPosition(); pos != NULL;)
   {   
      CRipper* ripper = NULL;
      CString key;
      m_ripperMap.GetNextAssoc(pos, key, ripper);
      if (ripper == NULL)
         continue;

      for (POSITION hotspotPos=ripper->getRipperHotspotStartPosition(); hotspotPos != NULL;)
      {
         CRipperHotspot* hotspot = ripper->getNextRipperHotspot(hotspotPos);
         if (hotspot == NULL)
            continue;
         if (hotspot->getConnectedToSignal() == NULL || hotspot->getConnectedToSignal() == this)
            continue;
         if (hotspot->getConnectedToSignal()->getSignalType() != signalTypeNet)
            continue;
   
         CString derivedBusName = getEnumeratedName(hotspot->getRule());
			if (derivedBusName.IsEmpty())
			{
				hotspot->getConnectedToSignal()->setRenamedName(hotspot->getConnectedToSignal()->getRenamedName(), renamedByBus);
			}
			else
			{
				hotspot->getConnectedToSignal()->setRenamedName(derivedBusName, renamedByBus);
			}

			addNet((CNet*)hotspot->getConnectedToSignal());
       }

   //   CString derivedBusName = getEnumeratedName(ripper->getWireRule());
   //   if (ripper->getConnectedToSignal1() != NULL && ripper->getConnectedToSignal1() != this)
   //   {
			//if (derivedBusName.IsEmpty())
			//{
			//	ripper->getConnectedToSignal1()->setRenamedName(ripper->getConnectedToSignal1()->getRenamedName(), renamedByBus);
			//}
			//else
			//{
			//	ripper->getConnectedToSignal1()->setRenamedName(derivedBusName, renamedByBus);
			//}

			//addNet((CNet*)ripper->getConnectedToSignal1());
   //   }

   //   if (ripper->getConnectedToSignal2() != NULL && ripper->getConnectedToSignal2() != this)
   //   {
			//if (derivedBusName.IsEmpty())
			//{
			//	ripper->getConnectedToSignal2()->setRenamedName(ripper->getConnectedToSignal2()->getRenamedName(), renamedByBus);
			//}
			//else
			//{
	  //       ripper->getConnectedToSignal2()->setRenamedName(derivedBusName, renamedByBus);
			//}

			//addNet((CNet*)ripper->getConnectedToSignal2());
   //   }
   }
}

void CBus::createNetLis()
{
   CBusStruct* busStruct = sFileStruct->getBusList().AddBus(m_renamedName);
	if (busStruct == NULL)
		return;

	if (m_attribMap != NULL)
	{
		sDoc->CopyAttribs(&busStruct->getAttributesRef(), m_attribMap);
	}
   
   for (POSITION pos = m_busMap.GetStartPosition(); pos != NULL;)
   {
      CBus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      if (bus == NULL)
         continue;

      busStruct->AddBusName(bus->getRenamedName());
   }

   for (POSITION pos = m_netMap.GetStartPosition(); pos != NULL;)
   {
      CNet* net = NULL;
      CString key;
      m_netMap.GetNextAssoc(pos, key, net);
      if (net == NULL)
         continue;
      
      busStruct->AddNetName(net->getRenamedName());
   }
}

//-----------------------------------------------------------------------------
// CInstance
//-----------------------------------------------------------------------------
CInstance::CInstance(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
	, m_viewName(viewName)
	, m_name(name)
	, m_instancePortMap(nextPrime2n(20),true)
{
   m_attribMap = NULL;
	reset();
}

CInstance::CInstance(CEdif& edif, const CString libraryName, const CString cellName, const CString clusterName, const CString viewName, const CInstance& instance)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
	, m_viewName(viewName)
	, m_name(instance.m_name)
	, m_instancePortMap(nextPrime2n(20),true)
{
   m_attribMap = NULL;
   *this = instance;
}

CInstance& CInstance::operator =(const CInstance& instance)
{
   if (&instance != this)
   {
      reset();
      
      m_displayName = instance.m_displayName;
      m_designator = instance.m_designator;
      m_primaryName = instance.m_primaryName;
      m_libraryRef = instance.m_libraryRef;
      m_cellRef = instance.m_cellRef;
      m_clusterRef = instance.m_clusterRef;
      m_symbolRef = instance.m_symbolRef;

      //m_cellName = instance.m_cellName;
      //m_libraryName = instance.m_libraryName;

      for (POSITION pos = instance.m_instancePortMap.GetStartPosition(); pos != NULL;)
      {
         CInstancePort* instancePort = NULL;
         CString key;
         instance.m_instancePortMap.GetNextAssoc(pos, key, instancePort);
         if (instancePort == NULL)
            continue;

         CInstancePort* copy = new CInstancePort(m_edif, m_libraryName, m_cellName, m_clusterName, m_viewName, m_name, *instancePort);
         m_instancePortMap.SetAt(key, copy);
      }

		if (instance.m_attribMap != NULL)
		{
			m_attribMap = new CAttributes(*instance.m_attribMap);
		}
   }

   return *this;
}

CInstance::~CInstance()
{
	reset();
}

void CInstance::reset()
{
	m_primaryName.Empty();
	m_displayName.Empty();
	m_designator.Empty();
	m_cellRef.Empty();
	m_clusterRef.Empty();
	m_symbolRef.Empty();
	m_instanceDataStruct = NULL;
	m_instancePortMap.empty();

   delete m_attribMap;
   m_attribMap = NULL;

   if (portAttribMap.GetCount() > 0)
   {
      POSITION mapPos =  portAttribMap.GetStartPosition();
      while (mapPos)
      {
         CString mapKey;
         void *mapVoidPtr;
         portAttribMap.GetNextAssoc(mapPos, mapKey, mapVoidPtr);
         CAttributes* attribMap = (CAttributes*)mapVoidPtr;
         delete attribMap;
         attribMap = NULL;
      }
      portAttribMap.RemoveAll();
   }

   if (portAttribDisplayMap.GetCount() > 0)
   {
      POSITION mapPos =  portAttribDisplayMap.GetStartPosition();
      while (mapPos)
      {  
         CString mapKey;
         void *mapVoidPtr;
         portAttribDisplayMap.GetNextAssoc(mapPos, mapKey, mapVoidPtr);
         CAttributes* attribMap = (CAttributes*)mapVoidPtr;
         delete attribMap;
         attribMap = NULL;
      }
      portAttribDisplayMap.RemoveAll();
   }
}

CInstancePort* CInstance::addInstancePort(CString portRef)
{
	CInstancePort* instancePort = findInstancePort(portRef);
	if (instancePort == NULL)
	{
		instancePort = new CInstancePort(m_edif, m_libraryName, m_cellName, m_clusterName, m_viewName, m_name, portRef, connectorTypeInstancePort);
		m_instancePortMap.SetAt(portRef, instancePort);
	}
	return instancePort;
}

CInstancePort* CInstance::findInstancePort(CString portRef)
{
	CInstancePort* instancePort = NULL;
	m_instancePortMap.Lookup(portRef, instancePort);
	return instancePort;
}

bool CInstance::isHierarchSymbol()
{
   bool retval = false;
   if (m_instanceDataStruct != NULL && m_instanceDataStruct->getInsert() != NULL)
   {
      retval = m_instanceDataStruct->getInsert()->getInsertType()==insertTypeHierarchicalSymbol;
   }
   return retval;
}

//-----------------------------------------------------------------------------
// CEdifView
//-----------------------------------------------------------------------------
CEdifView::CEdifView(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
	, m_name(name)
   , m_pageMap(false)
	, m_instanceMap(nextPrime2n(20),true)
	, m_netMap(nextPrime2n(20),true)
	, m_busMap(nextPrime2n(20),true)
	, m_globalPortMap(nextPrime2n(20),true)
	, m_masterPortMap(nextPrime2n(20), true)
	, m_onPageConnectorMap(nextPrime2n(20),true)
	, m_offPageConnectorMap(nextPrime2n(20),true)
	, m_ripperMap(nextPrime2n(20),true)
{
	reset();
}

CEdifView::CEdifView(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CEdifView& view)
   : m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
   , m_name(view.m_name)
   , m_pageMap(false)
	, m_instanceMap(nextPrime2n(20),true)
	, m_netMap(nextPrime2n(20),true)
	, m_busMap(nextPrime2n(20),true)
	, m_globalPortMap(nextPrime2n(20),true)
	, m_masterPortMap(nextPrime2n(20), true)
	, m_onPageConnectorMap(nextPrime2n(20),true)
	, m_offPageConnectorMap(nextPrime2n(20),true)
	, m_ripperMap(nextPrime2n(20),true)
{
   *this = view;
}

CEdifView& CEdifView::operator =(const CEdifView& view)
{
   if (&view != this)
   {
      reset();

      for (POSITION pos = view.m_netMap.GetStartPosition(); pos != NULL;)
      {
         CNet* net = NULL;
         CString key;
         view.m_netMap.GetNextAssoc(pos, key, net);
         if (net == NULL)
            continue;

         CNet* copy = new CNet(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, *net);
         m_netMap.SetAt(key, copy);
      }

      for (POSITION pos = view.m_busMap.GetStartPosition(); pos != NULL;)
      {
         CBus* bus = NULL;
         CString key;
         view.m_busMap.GetNextAssoc(pos, key, bus);
         if (bus == NULL)
            continue;

         CBus* copy = new CBus(m_edif, m_libraryName, m_cellName, m_clusterName, *this, *bus);
//         CBus* copy = new CBus(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, *bus);
         m_busMap.SetAt(key, copy);
      }

      for (POSITION pos = view.m_pageMap.GetHeadPosition(); pos != NULL;)
      {
         BlockStruct* pageBlock = view.m_pageMap.GetNext(pos);
         if (pageBlock == NULL)
            continue;

         // Get the page name
         CString pageName = pageBlock->getName();
         int index = pageName.ReverseFind(SCH_DELIMITER);
         if (index > -1)
            pageName = pageName.Right(pageName.GetLength()-index-1);

         BlockStruct* copy = Graph_Block_On(GBO_APPEND, generatePageBlockName(pageName), sFileStruct->getFileNumber(), 0L, blockTypeSheet);
         Graph_Block_Copy(pageBlock, 0.0, 0.0, 0.0, 0, 1.0, 0, TRUE);
         Graph_Block_Off();
         m_pageMap.AddTail(copy);

         if (view.m_firstSchematicPageBlock == pageBlock)
         {
            m_firstSchematicPageBlock = copy;
         }

         for (CDataListIterator dataList(*copy, dataTypeInsert); dataList.hasNext();)
         {
            DataStruct* data = dataList.getNext();
            InsertStruct* insert = data->getInsert();

            switch (insert->getInsertType())
            {
            case insertTypePortInstance:
               {
                  CPort* port = NULL;
                  CPort* newPort = NULL;
                  if (view.m_masterPortMap.Lookup(insert->getRefname(), port))
                  {
                     newPort = new CPort(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, pageName, port->getName(), connectorTypeMasterPort);
                     m_masterPortMap.SetAt(insert->getRefname(), newPort);
                  }
                  else if (view.m_globalPortMap.Lookup(insert->getRefname(), port))
                  {
                     newPort = new CPort(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, pageName, port->getName(), connectorTypeGlobalPort);
                     m_globalPortMap.SetAt(insert->getRefname(), newPort);
                  }                

                  // Get the connected to signal name from the port of the copy from pageBlock
                  // Then find that signal in this view and attach it to the master or global port
                  if (port != NULL && newPort != NULL)
                  {
                     CSignal* signal =  NULL;
                     if (port->getConnectedToSignal() != NULL)
                     {
                        if (port->getConnectedToSignal()->getSignalType() == signalTypeNet)
                        {
                           signal = findNet(port->getConnectedToSignal()->getName());
                        }
                        else
                        {
                           signal = findBus(port->getConnectedToSignal()->getName());
                        }

                        if (newPort->getcConnectorType() == connectorTypeGlobalPort)
                           signal->addGlobalPort(newPort);
                        else
                           signal->addMasterPort(newPort);
                     }
 
                     newPort->setConnectedToSignal(signal);
                     newPort->setPortDataStruct(data);
                     newPort->setDesignator(port->getDesignator());
                     newPort->setPortRef(port->getPortRef());
                     newPort->setPrimaryName(port->getPrimaryName());
                   }
               }
               break;

            case insertTypeSymbol:
            case insertTypeHierarchicalSymbol:
               {
                  CInstance* instance = NULL;
                  view.m_instanceMap.Lookup(insert->getRefname(), instance);
                  if (instance == NULL)
                     continue;

                  CInstance* newInstance = new CInstance(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, *instance);
                  m_instanceMap.SetAt(newInstance->getName(), newInstance);
                  newInstance->setInstanceDataStruct(data);
               
                  // Loop through instancePort of the copy from instance to get the connected to signal name
                  // Then find that signal in this view and attach it to the newInstancePort
                  for (POSITION pos = instance->getInstancePortStartPosition(); pos != NULL;)
                  {
                     CInstancePort* instancePort = instance->getNextInstancePort(pos);
                     if (instancePort == NULL || instancePort->getConnectedToSignal() == NULL)
                        continue;

                     CSignal* signal = findNet(instancePort->getConnectedToSignal()->getName());
                     CInstancePort* newInstancePort = newInstance->findInstancePort(instancePort->getName());
                     newInstancePort->setConnectedToSignal(signal);

                     if (signal !=  NULL)
                     {
                        signal->addInstancePort(newInstancePort);
                     }
                  }
               }
               break;

            case insertTypeSheetConnector:
               {
                  CPageConnector* connector = NULL;
                  CPageConnector* newConnector = NULL;
                  if (view.m_onPageConnectorMap.Lookup(insert->getRefname(), connector))
                  {
                     newConnector = new CPageConnector(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, pageName, connector->getName(), connectorTypeOnPageConnector);
                     m_onPageConnectorMap.SetAt(insert->getRefname(), newConnector);
                  }
                  else if (view.m_offPageConnectorMap.Lookup(insert->getRefname(), connector))
                  {
                     newConnector = new CPageConnector(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, pageName, connector->getName(), connectorTypeOffPageConnecotor);
                     m_offPageConnectorMap.SetAt(insert->getRefname(), newConnector);
                  }                

                  // Get the connected to signal name from the port of the copy from pageBlock
                  // Then find that signal in this view and attach it to the master or global port
                  if (connector != NULL && newConnector != NULL)
                  {
                     CSignal* signal =  NULL;
                     if (connector->getConnectedToSignal() != NULL)
                     {
                        if (connector->getConnectedToSignal()->getSignalType() == signalTypeNet)
                        {
                           signal = findNet(connector->getConnectedToSignal()->getName());
                        }
                        else
                        {
                           signal = findBus(connector->getConnectedToSignal()->getName());
                        }

                        if (newConnector->getcConnectorType() == connectorTypeOnPageConnector)
                           signal->addOnPageConnector(newConnector);
                        else
                           signal->addOffPageConnector(newConnector);
                     }
 
                     newConnector->setConnectedToSignal(signal);
                     newConnector->setConnectorDataStruct(data);
                     newConnector->setDesignator(connector->getDesignator());
                     newConnector->setPrimaryName(connector->getPrimaryName());
                   }
               }
               break;

            case insertTypeRipper:
               {
                  CRipper* ripper = NULL;
                  CRipper* newRipper = NULL;
                  if (view.m_ripperMap.Lookup(insert->getRefname(), ripper))
                  {
                     newRipper = new CRipper(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, pageName, ripper->getName(), connectorTypeRipper);
                     CopyANDRemoveAttribMap(&newRipper->getAttributesRef(), ripper->getAttributesRef());
                     m_ripperMap.SetAt(insert->getRefname(), newRipper);
                  }

                  if (ripper != NULL && newRipper != NULL)
                  {
                     for (POSITION pos=ripper->getRipperHotspotStartPosition(); pos != NULL;)
                     {
                        CRipperHotspot* hotspot = ripper->getNextRipperHotspot(pos);
                        if (hotspot == NULL)
                           continue;

                        CRipperHotspot* newHotspot = newRipper->addRipperHotspot(hotspot->getName());
                        newHotspot->setPin(hotspot->getPin());
                        newHotspot->setRule(hotspot->getRule());

                        CSignal* hotspotConnectedSignal = hotspot->getConnectedToSignal();
                        CSignal* newHotspotConnectedSignal = NULL;

                        if (hotspotConnectedSignal != NULL)
                        {
                           if (hotspotConnectedSignal->getSignalType() == signalTypeNet)
                           {
                              newHotspotConnectedSignal = findNet(hotspotConnectedSignal->getName());
                           }
                           else
                           {
                              newHotspotConnectedSignal = findBus(hotspotConnectedSignal->getName());
                           }
                           newHotspotConnectedSignal->addRipper(newRipper);

                        }
                     
                        newHotspot->setConnectedToSignal(newHotspotConnectedSignal);
                     }

                     //CSignal* signal1 =  NULL;
                     //if (ripper->getConnectedToSignal1() != NULL)
                     //{
                     //   if (ripper->getConnectedToSignal1()->getSignalType() == signalTypeNet)
                     //   {
                     //      signal1 = findNet(ripper->getConnectedToSignal1()->getName());
                     //   }
                     //   else
                     //   {
                     //      signal1 = findBus(ripper->getConnectedToSignal1()->getName());
                     //   }
                     //   signal1->addRipper(newRipper);
                     //}

                     //CSignal* signal2 =  NULL;
                     //if (ripper->getConnectedToSignal2() != NULL)
                     //{
                     //   if (ripper->getConnectedToSignal2()->getSignalType() == signalTypeNet)
                     //   {
                     //      signal2 = findNet(ripper->getConnectedToSignal2()->getName());
                     //   }
                     //   else
                     //   {
                     //      signal2 = findBus(ripper->getConnectedToSignal2()->getName());
                     //   }
                     //   signal2->addRipper(newRipper);
                     //}

                     //newRipper->setConnectedToSignal1(signal1);
                     //newRipper->setConnectedToSignal2(signal2);
                     newRipper->setDesignator(ripper->getDesignator());
                     newRipper->setPrimaryName(ripper->getPrimaryName());
                  }
               }
               break;

            default:
               break;
            }
         }
      }
   }

   return *this;
}

CEdifView::~CEdifView()
{
	reset();
}

void CEdifView::reset()
{
	m_pageMap.empty();
	m_instanceMap.empty();
	m_netMap.empty();
	m_busMap.empty();
	m_globalPortMap.empty();
	m_masterPortMap.empty();
	m_onPageConnectorMap.empty();
	m_offPageConnectorMap.empty();
	m_ripperMap.empty();

   m_firstSchematicPageBlock = NULL;
	m_currentPageBlock = NULL;
	m_currentNet = NULL;
	m_currentBus = NULL;
	m_currentPageName.Empty();
}

CString CEdifView::getUniquePageName(const CString pageName)
{
   CString retval;
   retval.Format("Sheet_%d(%s)", m_pageMap.GetCount() + 1, pageName);
   return retval;
}

void CEdifView::addPage(BlockStruct* pageBlock)
{
	m_pageMap.AddTail(pageBlock);
}

BlockStruct* CEdifView::getFirstPage()
{
	BlockStruct* pageBlock = getFirstSchematicPageBlock();      
   if (pageBlock == NULL)
   {
      pageBlock = m_pageMap.GetHead();
   }
	return pageBlock;
}

POSITION CEdifView::getPageStartPosition()
{ 
	return m_pageMap.GetHeadPosition();		
}

BlockStruct* CEdifView::getNextPage(POSITION& pos)			
{
	BlockStruct* pageBlock = m_pageMap.GetNext(pos);
	return pageBlock;
}

CString CEdifView::generatePageBlockName(CString pageName)
{
   CString blockName;
   blockName.Format("%s%c%s%c%s%c%s%c%s", m_libraryName, SCH_DELIMITER, m_cellName, SCH_DELIMITER, m_clusterName, SCH_DELIMITER,
                    m_name, SCH_DELIMITER, pageName);
   blockName.MakeUpper();
	return blockName;
}

CInstance* CEdifView::addInstance(const CString instanceName)
{
	CInstance* instance = findInstance(instanceName);
	if (instance == NULL)
	{
		instance = new CInstance(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, instanceName);
		m_instanceMap.SetAt(instanceName, instance);
	}
	return instance;
}

CInstance* CEdifView::findInstance(const CString instanceName)
{
	CInstance* instance = NULL;
	m_instanceMap.Lookup(instanceName, instance);
	return instance;
}

CPort* CEdifView::addGlobalPort(const CString portInstanceName)
{
	CPort* port = findGlobalPort(portInstanceName);
	if (port == NULL)
	{
		port = new CPort(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, m_currentPageName, portInstanceName, connectorTypeGlobalPort);
		m_globalPortMap.SetAt(portInstanceName, port);
	}
	return port;
}

CPort* CEdifView::findGlobalPort(const CString portInstanceName)
{
	CPort* port = NULL;
	m_globalPortMap.Lookup(portInstanceName, port);
	return port;
}

CPort* CEdifView::findGlobalPortByPortName(const CString portName)
{
   for (POSITION pos = m_globalPortMap.GetStartPosition(); pos != NULL;)
   {
      CPort* port = NULL;
      CString key;
      m_globalPortMap.GetNextAssoc(pos, key, port);
      if (port == NULL)
         continue;

      if (port->getPortRef().CompareNoCase(portName) == 0)
         return port;
   }

   return NULL;
}

CPort* CEdifView::addMasterPort(const CString portInstanceName)
{
	CPort* port = findMasterPort(portInstanceName);
	if (port == NULL)
	{
		port = new CPort(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, m_currentPageName, portInstanceName, connectorTypeMasterPort);
		m_masterPortMap.SetAt(portInstanceName, port);
	}
	return port;
}

CPort* CEdifView::findMasterPort(const CString portInstanceName)
{
	CPort* port = NULL;
	m_masterPortMap.Lookup(portInstanceName, port);
	return port;
}

CPort* CEdifView::findMasterPortByPortName(const CString portName)
{
   for (POSITION pos = m_globalPortMap.GetStartPosition(); pos != NULL;)
   {
      CPort* port = NULL;
      CString key;
      m_masterPortMap.GetNextAssoc(pos, key, port);
      if (port == NULL)
         continue;

      if (port->getPortRef().CompareNoCase(portName) == 0)
         return port;
   }

   return NULL;
}

CPageConnector* CEdifView::addOnPageConnector(const CString connectorName)
{
	CPageConnector* connector = findOnPageConnector(connectorName);
	if (connector == NULL)
	{
		connector = new CPageConnector(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, m_currentPageName, connectorName, connectorTypeOnPageConnector);
		m_onPageConnectorMap.SetAt(connectorName, connector);
	}
	return connector;
}

CPageConnector* CEdifView::findOnPageConnector(const CString connectorName)
{
	CPageConnector* connector = NULL;
	m_onPageConnectorMap.Lookup(connectorName, connector);
	return connector;
}

CPageConnector* CEdifView::addOffPageConnector(const CString connectorName)
{
	CPageConnector* connector = findOffPageConnector(connectorName);
	if (connector == NULL)
	{
		connector = new CPageConnector(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, m_currentPageName, connectorName, connectorTypeOffPageConnecotor);
		m_offPageConnectorMap.SetAt(connectorName, connector);
	}
	return connector;
}

CPageConnector* CEdifView::findOffPageConnector(const CString connectorName)
{
	CPageConnector* connector = NULL;
	m_offPageConnectorMap.Lookup(connectorName, connector);
	return connector;
}

CRipper* CEdifView::addRipper(const CString ripperName)
{
	CRipper* ripper = findRipper(ripperName);
	if (ripper == NULL)
	{
		ripper = new CRipper(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, m_currentPageName, ripperName, connectorTypeRipper);
		m_ripperMap.SetAt(ripperName, ripper);
	}
	return ripper;
}

CRipper* CEdifView::findRipper(const CString ripperName)
{
	CRipper* ripper = NULL;
	m_ripperMap.Lookup(ripperName, ripper);
	return ripper;
}

void CEdifView::applyRipperRulesToHotspot()
{
   for (POSITION pos=m_ripperMap.GetStartPosition(); pos != NULL;)
   {
      CRipper* ripper = NULL;
      CString key;
      m_ripperMap.GetNextAssoc(pos, key, ripper);
      if (ripper ==  NULL || ripper->getAttributesRef() == NULL)
         continue;

      CString pinKeyword, ruleKeyword;
      for (POSITION hotspotPos=ripper->getRipperHotspotStartPosition(); hotspotPos != NULL;)
      {
         CRipperHotspot* hotspot = ripper->getNextRipperHotspot(hotspotPos);
         if (hotspot == NULL)
            continue;

         pinKeyword.Format("%s%s", hotspot->getName(), SCH_HOTSPOT_PIN);
         ruleKeyword.Format("%s%s", hotspot->getName(), SCH_HOTSPOT_RULE);

         int pinKw = sDoc->RegisterKeyWord(pinKeyword, 0, valueTypeString);
         int ruleKw = sDoc->RegisterKeyWord(ruleKeyword, 0, valueTypeString);

         CString pinValue, ruleValue;
         sDoc->getAttributeStringValue(pinValue, *ripper->getAttributesRef(), pinKw);
         sDoc->getAttributeStringValue(ruleValue, *ripper->getAttributesRef(), ruleKw);

         hotspot->setPin(pinValue);
         hotspot->setRule(ruleValue);
      }
   }
}

CNet* CEdifView::addNet(const CString netName)
{
	CNet* net = findNet(netName);
	if (net == NULL)
	{
		net = new CNet(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, netName, signalTypeNet);
		m_netMap.SetAt(netName, net);
	}
	return net;
}

CNet* CEdifView::findNet(const CString netName)
{
	CNet* net = NULL;
	m_netMap.Lookup(netName, net);
	return net;
}

CBus* CEdifView::addBus(const CString busName)
{
	CBus* bus = findBus(busName);
	if (bus == NULL)
	{
		bus = new CBus(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, busName, signalTypeBus);
		m_busMap.SetAt(busName, bus);
	}
	return bus;
}

CBus* CEdifView::findBus(const CString busName)
{
	CBus* bus = NULL;
	m_busMap.Lookup(busName, bus);
	return bus;
}

void CEdifView::convertLogicalSchematicToPhysicalSchematic()
{
   int hierarchGeomNumKw = sDoc->RegisterKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0, valueTypeString);
   for (POSITION instancePos = m_instanceMap.GetStartPosition(); instancePos != NULL;)
   {
      CInstance* instance = NULL;
      CString key;
      m_instanceMap.GetNextAssoc(instancePos, key, instance);
      if (instance == NULL || !instance->isHierarchSymbol())
         continue;

      CEdifCell* cell = m_edif.findCell(instance->getLibraryRef(), instance->getCellRef());
      CEdifCluster* cluster = cell->findCluster(instance->getClusterRef());
      if (cluster == NULL)
         continue;

      CEdifCluster* clonedCluster = new CEdifCluster(*cluster);
      cell->addCluster(clonedCluster);
      instance->setClusterRef(clonedCluster->getName());
#if CamCadMajorMinorVersion > 406  //  > 4.6
      instance->getInstanceDataStruct()->setAttrib(sDoc->getCamCadData(), hierarchGeomNumKw, valueTypeString, clonedCluster->getDelimitedPageBlockNumber(" ").GetBuffer(0), attributeUpdateOverwrite, NULL);  
#else
      instance->getInstanceDataStruct()->setAttrib(sDoc, hierarchGeomNumKw, valueTypeString, clonedCluster->getDelimitedPageBlockNumber(" ").GetBuffer(0), attributeUpdateOverwrite, NULL);      
#endif
   }
}

void CEdifView::removedAllPageBlocks()
{
   for (POSITION pos = getPageStartPosition(); pos != NULL;)
   {
      BlockStruct* pageBlock = getNextPage(pos);
      if (pageBlock == NULL)
         continue;

      sDoc->RemoveBlock(pageBlock);
   }
}

void CEdifView::prepareBusAndNetForRename()
{
   // Prepare every net for net rename
   for (POSITION pos = m_netMap.GetStartPosition(); pos != NULL;)
   {
      CNet* net = NULL;
      CString key;
      m_netMap.GetNextAssoc(pos, key, net);
      if (net == NULL)
         continue;

      net->prepareForRename();
   }

   for (POSITION pos = m_busMap.GetStartPosition(); pos != NULL;)
   {
      CBus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      if (bus == NULL)
         continue;

		bus->prepareForRename();
	}
}

void CEdifView::applyGlobalPortNameToBussesAndNets()
{
   for (POSITION pos = m_globalPortMap.GetStartPosition(); pos != NULL;)
   {
      CPort* globalPort = NULL;
      CString key;
      m_globalPortMap.GetNextAssoc(pos, key, globalPort);
      if (globalPort == NULL || globalPort->getConnectedToSignal() == NULL)
         continue;

      globalPort->getConnectedToSignal()->setRenamedName(globalPort->getPortRef(), renamedByGlobalPort);
   }
}

void CEdifView::applyMasterPortNameToBussesAndNets(CInstance* instance)
{
	if (instance == NULL)
		return;

	for (POSITION pos = m_masterPortMap.GetStartPosition(); pos != NULL;)
	{
		CPort* masterPort = NULL;
		CString key;
		m_masterPortMap.GetNextAssoc(pos, key, masterPort);
		if (masterPort == NULL || masterPort->getConnectedToSignal() == NULL)
			continue;

		CInstancePort* instancePort = instance->findInstancePort(masterPort->getPortRef());
		if (instancePort == NULL ||instancePort->getConnectedToSignal() == NULL)
			continue;

		CString beforeMasterPortRename = masterPort->getConnectedToSignal()->getRenamedName();
		masterPort->getConnectedToSignal()->setRenamedName(instancePort->getConnectedToSignal()->getRenamedName(), renamedByMasterPort);

		for (POSITION netPos = m_netMap.GetStartPosition(); netPos != NULL;)
		{
			CNet* net = NULL;
			CString netKey;
			m_netMap.GetNextAssoc(netPos, netKey, net);
			if (net == NULL || net->getRenamedName().CompareNoCase(beforeMasterPortRename) != 0)
				continue;

			net->setRenamedName(instancePort->getConnectedToSignal()->getRenamedName(), renamedByMasterPort);
		}
	}

}

void CEdifView::applyDoneToBussesAndNetsOnPageConnector()
{
   for (POSITION pos = m_onPageConnectorMap.GetStartPosition(); pos != NULL;)
   {
      CPageConnector* onPageConnector = NULL;
      CString key;
      m_onPageConnectorMap.GetNextAssoc(pos, key, onPageConnector);
      if (onPageConnector == NULL || onPageConnector->getConnectedToSignal() == NULL)
         continue;

      onPageConnector->getConnectedToSignal()->setRenamedName(onPageConnector->getConnectedToSignal()->getRenamedName(), renamedByOnPageConnector);
   }

   for (POSITION pos = m_offPageConnectorMap.GetStartPosition(); pos != NULL;)
   {
      CPageConnector* offPageConnector = NULL;
      CString key;
      m_offPageConnectorMap.GetNextAssoc(pos, key, offPageConnector);
      if (offPageConnector == NULL || offPageConnector->getConnectedToSignal() == NULL)
         continue;

      offPageConnector->getConnectedToSignal()->setRenamedName(offPageConnector->getConnectedToSignal()->getRenamedName(), renamedByOffPageConnector);
   }
}

void CEdifView::applyBussesDerivedNameToNets()
{
   for (POSITION pos = m_busMap.GetStartPosition(); pos != NULL;)
   {
      CBus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      if (bus == NULL)
         continue;

      bus->applyBusDerivedNametoConnectedNets();
   }
}

void CEdifView::applyNetsOnInstanceToHierarchyStructure()
{
   for (POSITION instancePos = m_instanceMap.GetStartPosition(); instancePos != NULL;)
   {
      CInstance* instance = NULL;
      CString key;
      m_instanceMap.GetNextAssoc(instancePos, key, instance);
      if (instance == NULL || !instance->isHierarchSymbol())
         continue;

      CEdifCell* cell = m_edif.findCell(instance->getLibraryRef(), instance->getCellRef());
      if (cell == NULL)
         continue;

      CEdifCluster* cluster = cell->findCluster(instance->getClusterRef());
      if (cluster == NULL)
         continue;

		cluster->propagateNetsFromInstanceToHierarchyStructure(instance);
   }
}

void CEdifView::applyUniqueBusNameToLocalBusses()
{
	CEdifCluster* cluster = m_edif.findCluster(m_libraryName, m_cellName, m_clusterName);

	for (POSITION pos = m_busMap.GetStartPosition(); pos != NULL;)
   {
      CBus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      if (bus == NULL || bus->isRenameDone())
         continue;

		CString rename = bus->getRenamedName();
		if (rename.CompareNoCase(bus->getAnnotateName()) != 0)
		{
			// This bus is a local bus and its name has not been annotated; therefore, 
			// it need to get a unique bus name
			rename = m_edif.getUniqueLocalBusName(rename);
		}

		bus->setRenamedName(rename, renamedByBus);
	}
}

void CEdifView::applyNetAliasNameToNets()
{
   for (POSITION pos = m_netMap.GetStartPosition(); pos != NULL;)
   {
      CNet* net = NULL;
      CString key;
      m_netMap.GetNextAssoc(pos, key, net);
      if (net == NULL || net->isRenameDone())
         continue;

		CString aliasRename = net->getAliasRename();
		net->setRenamedName(aliasRename, renamedByAlias);
   }
}

void CEdifView::createNetListFromRenamedNetsAndBusses()
{
   int compKey = sDoc->RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, 0, valueTypeString);
   int pinKey = sDoc->RegisterKeyWord(SCH_ATT_PINDESIGNATOR, 0, valueTypeString);
   ///int cellKey = sDoc->RegisterKeyWord("CELL_NAME", 0, valueTypeString);

	for (POSITION instancePos = m_instanceMap.GetStartPosition(); instancePos != NULL;)
   {
      CInstance* instance = NULL;
      CString key;
      m_instanceMap.GetNextAssoc(instancePos, key, instance);
      if (instance == NULL || instance->isHierarchSymbol())
         continue;

		// Need to make sure that is an instance insert because sometime there
		// can be an instance without an actuall insert, example case #1885
		DataStruct* instanceData = instance->getInstanceDataStruct();	
		if (instanceData == NULL)
			continue;

		if (instanceData->getInsert()->getInsertType() != insertTypeSymbol)
			continue;

		CEdifCluster* cluster = m_edif.findCluster(instance->getLibraryRef(), instance->getCellRef(), instance->getClusterRef());
		CEdifSymbol* symbol = cluster->findSymbol(instance->getSymbolRef());

		// insert of symbol instance
		CTMatrix matrix = instanceData->getInsert()->getTMatrix();

		for (POSITION symbolPortPos = symbol->getSymbolPortStartPosition(); symbolPortPos != NULL;)
		{
			CSymbolPort* symbolPort = symbol->getNextSymbolPort(symbolPortPos);
			if (symbolPort == NULL || symbolPort->getPortDataStruct() == NULL)
				continue;

			CInstancePort* instancePort = instance->findInstancePort(symbolPort->getPortRef());
			if (instancePort == NULL || instancePort->getConnectedToSignal() == NULL)
				continue;

			// insert of port inside symbol block
			DataStruct* symbolPortData = symbolPort->getPortDataStruct();
			CPoint2d pinLocation = symbolPortData->getInsert()->getOrigin2d();
			matrix.transform(pinLocation);

			NetStruct *othernet = NULL;
			CompPinStruct *othercp = FindCompPin(sFileStruct, instanceData->getInsert()->getRefname(), symbolPortData->getInsert()->getRefname(), &othernet);
         if (othercp != NULL)
         {
            CString msg;
            msg.Format("Trying to add comppin %s.%s to net %s, same comppin already exists on net %s.",
               instanceData->getInsert()->getRefname(), symbolPortData->getInsert()->getRefname(),
               instancePort->getConnectedToSignal()->getRenamedName(), othernet != NULL ? othernet->getNetName() : "NULL");

            //ErrorMessage(msg, "Comp Pin Error");

				fprintf(sFileErr, "%s\n", msg);
				sDisplayErr++;
         }

			NetStruct* netStruct = add_net(sFileStruct, instancePort->getConnectedToSignal()->getRenamedName());

         // These "owner" (aka "parent") attribs are set up so later processing can associate comppins
         // with the correct nets. Note that this reader will make mulitple nets and comppins with
         // exact same names. They originate in EDIF from different library/cell/cluster contexts, but
         // in CAMCAD there is only one netlist in the file, so these attribs are how to tell
         // what goes with what. See RenameCompAndPinDesignator in Sch_Lib.cpp.

         netStruct->attributes(); // The attributes() call creates attrib struct if not already present

         //*rcf get rid of these, can't set this attrib on net cuz comppins come from different cells
         sDoc->SetUnknownAttrib(&netStruct->getAttributesRef(), SCH_ATT_LIBRARYNAME, instance->getParentLibraryName().GetBuffer(0), attributeUpdateOverwrite, NULL);
         sDoc->SetUnknownAttrib(&netStruct->getAttributesRef(), SCH_ATT_CELLNAME,    instance->getParentCellName().GetBuffer(0),    attributeUpdateOverwrite, NULL);
         sDoc->SetUnknownAttrib(&netStruct->getAttributesRef(), SCH_ATT_CLUSTERNAME, instance->getParentClusterName().GetBuffer(0), attributeUpdateOverwrite, NULL);

			CompPinStruct* compPin = add_comppin(sFileStruct, netStruct, instanceData->getInsert()->getRefname(), symbolPortData->getInsert()->getRefname());
         compPin->setOrigin(pinLocation);
         
         sDoc->SetUnknownAttrib(&compPin->getDefinedAttributes(), SCH_ATT_PARENTLIBRARY, instance->getParentLibraryName().GetBuffer(0), attributeUpdateOverwrite, NULL);
         sDoc->SetUnknownAttrib(&compPin->getDefinedAttributes(), SCH_ATT_PARENTCELL,    instance->getParentCellName().GetBuffer(0),    attributeUpdateOverwrite, NULL);
         sDoc->SetUnknownAttrib(&compPin->getDefinedAttributes(), SCH_ATT_PARENTCLUSTER, instance->getParentClusterName().GetBuffer(0), attributeUpdateOverwrite, NULL);

#if CamCadMajorMinorVersion > 406  //  > 4.6
         compPin->setAttrib(sDoc->getCamCadData(), compKey, valueTypeString, instance->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
         compPin->setAttrib(sDoc->getCamCadData(), pinKey, valueTypeString, instancePort->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
         compPin->setAttrib(sDoc, compKey, valueTypeString, instance->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
         compPin->setAttrib(sDoc, pinKey, valueTypeString, instancePort->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
      }
	}

   for (POSITION pos = m_busMap.GetStartPosition(); pos != NULL;)
   {
      CBus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      if (bus == NULL)
         continue;

      bus->createNetLis();
   }
}

void CEdifView::applyRenamedNetAndBusNameToEtch()
{
	WORD netNameKw = sDoc->getStandardAttributeKeywordIndex(standardAttributeNetName);

	for (POSITION pos = getPageStartPosition(); pos != NULL;)
	{
		BlockStruct* pageBlock = getNextPage(pos);
		if (pageBlock == NULL)
			continue;

		for (CDataListIterator dataList(*pageBlock, dataTypePoly); dataList.hasNext();)
		{
			DataStruct* polyData = dataList.getNext();
			Attrib* attrib = NULL;
			if (polyData->getAttributes() == NULL || !polyData->getAttributes()->Lookup(netNameKw, attrib))
				continue;

			CString newNetName;
			CString netName = GetAttribStringValue(attrib);

			CNet* net = findNet(netName);
			if (net != NULL)
			{
#if CamCadMajorMinorVersion > 406  //  > 4.6
            polyData->setAttrib(sDoc->getCamCadData(), (int)netNameKw, valueTypeString, net->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
			   polyData->setAttrib(sDoc, (int)netNameKw, valueTypeString, net->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
            CopyANDRemoveAttribMap(&polyData->getAttributesRef(), net->getAttributesRef());
         }
			else
			{
				CBus* bus = findBus(netName);
				if (bus != NULL)
            {
#if CamCadMajorMinorVersion > 406  //  > 4.6
               polyData->setAttrib(sDoc->getCamCadData(), (int)netNameKw, valueTypeString, bus->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
			      polyData->setAttrib(sDoc, (int)netNameKw, valueTypeString, bus->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
               CopyANDRemoveAttribMap(&polyData->getAttributesRef(), bus->getAttributesRef());
            }
			}
		}
	}
}

void CEdifView::createNetList()
{
   // *** Important note:
	//	1) In this function, we also assume all annotation is already done!
	//	2) Function calls MUST be in the code implemented

	prepareBusAndNetForRename();

		// 1) Apply value of attribute "PortName" as netName to all nets and busses connected to global ports and mark rename as DONE
   applyGlobalPortNameToBussesAndNets();

	// 2) For all nets and busses connected to page connector,  mark rename as DONE
	applyDoneToBussesAndNetsOnPageConnector();

	// 3) Fall all local busses, make sure they all have unique names
	applyUniqueBusNameToLocalBusses();

	if (sEdifProgram == edifProgramMgsiwrite)
	{
		// 4) Apply bus derived name to all nets connected to bus and mark rename as DONE
		applyBussesDerivedNameToNets();

		// 5) For all reminding nets that are not marked as DONE, then derived net name from connecting net(s)
      // Commented out for case dts0100399475
		//applyNetAliasNameToNets();
	}
 
   // 6) Propagate netname to hierarchy structure of the connected hierarchy symbol
	applyNetsOnInstanceToHierarchyStructure();

	// 7) Create net list from renamed nets
	createNetListFromRenamedNetsAndBusses();

	// 8) Apply renamed net and bus name to etches
	applyRenamedNetAndBusNameToEtch();
}

void CEdifView::propagateNetsFromInstanceToHierarchyStructure(CInstance* instance)
{
   // *** Important note:
	//	1) In this function, we also assume all annotation is already done!
	//	2) Function calls MUST be in the code implemented
	// This function is only difference to createNetList() by addition of step #3

	prepareBusAndNetForRename();

	// 1) Apply value of attribute "PortName" as netName to all nets and busses connected to global ports and mark rename as DONE
	applyGlobalPortNameToBussesAndNets();

	// 2) For all nets and busses connected to page connector,  mark rename as DONE
	applyDoneToBussesAndNetsOnPageConnector();

	// 3) Fall all local busses, make sure they all have unique names
	applyUniqueBusNameToLocalBusses();

	if (sEdifProgram == edifProgramMgsiwrite)
	{
		// 4) Apply bus derived name to all nets connected to bus and mark rename as DONE
		applyBussesDerivedNameToNets();

		// 5) For all reminding nets that are not marked as DONE, then derived net name from connecting net(s)
      // Commented out for case dts0100399475
		//applyNetAliasNameToNets();
	}

	// 6) For all nets and busses connect to master port, mark rename as DONE
	applyMasterPortNameToBussesAndNets(instance);

   // 7) Propagate netname to hierarchy structure of the connected hierarchy symbol
	applyNetsOnInstanceToHierarchyStructure();

	// 8) Now, create net list from renamed nets
	createNetListFromRenamedNetsAndBusses();

	// 9) Apply renamed net name to etches
	applyRenamedNetAndBusNameToEtch();
}

//-----------------------------------------------------------------------------
// CSymbolPorts
//-----------------------------------------------------------------------------
CSymbolPort::CSymbolPort(CEdif &edif, CString libraryName, CString cellName, CString clusterName, CString symbolName, CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
	, m_symbolName(symbolName)
	, m_name(name)
{
	m_portDataStruct = NULL;
}

CSymbolPort::CSymbolPort(const CSymbolPort& symbolPort)
   : m_edif(symbolPort.m_edif)
{
   m_libraryName = symbolPort.m_libraryName;
	m_cellName = symbolPort.m_cellName;
	m_clusterName = symbolPort.m_clusterName;
   m_symbolName = symbolPort.m_symbolName;
	m_name = symbolPort.m_name;
   m_portRef = symbolPort.m_portRef;
   m_portDataStruct = symbolPort.m_portDataStruct;
}

CSymbolPort::~CSymbolPort()
{
	m_portDataStruct = NULL;
}

//-----------------------------------------------------------------------------
// CEdifSymbol
//-----------------------------------------------------------------------------
CEdifSymbol::CEdifSymbol(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
	, m_name(name)
	, m_portMap(nextPrime2n(20),true)
{
	reset();
}

CEdifSymbol::CEdifSymbol(const CEdifSymbol& symbol)
   : m_edif(symbol.m_edif)
{
   *this = symbol;
}

CEdifSymbol& CEdifSymbol::operator =(const CEdifSymbol& symbol)
{
   if (&symbol != this)
   {
      reset();

      m_libraryName = symbol.m_libraryName;
	   m_cellName = symbol.m_cellName;
	   m_clusterName = symbol.m_clusterName;
	   m_name = symbol.m_name;
      m_symbolBlock = symbol.m_symbolBlock;

      for (POSITION pos = symbol.m_portMap.GetStartPosition(); pos != NULL;)
      {
         CSymbolPort* symbolPort = NULL;
         CString key;
         symbol.m_portMap.GetNextAssoc(pos, key, symbolPort);
         if (symbolPort == NULL)
            continue;

         CSymbolPort* copy = new CSymbolPort(*symbolPort);
         m_portMap.SetAt(key, copy);
      }
   }

   return *this;
}

CEdifSymbol::~CEdifSymbol()
{
	reset();
}

void CEdifSymbol::reset()
{
	m_portMap.empty();
	m_symbolBlock = NULL;
}

CSymbolPort* CEdifSymbol::addPort(CString portInstanceName)
{
	CSymbolPort* port = findPort(portInstanceName);
	if (port == NULL)
	{
		port = new CSymbolPort(m_edif, m_libraryName, m_cellName, m_clusterName, m_name, portInstanceName);
		m_portMap.SetAt(portInstanceName, port);
	}
	return port;
}

CSymbolPort* CEdifSymbol::findPort(CString portInstanceName)
{
	CSymbolPort* port =  NULL;
	m_portMap.Lookup(portInstanceName, port);
	return port;
}

CString CEdifSymbol::generateSymbolBlockName()
{
   CString blockName;
   blockName.Format("%s%c%s%c%s%c%s", m_libraryName, SCH_DELIMITER, m_cellName, SCH_DELIMITER, m_clusterName, SCH_DELIMITER, m_name);
	blockName.MakeUpper();
	return blockName;
}
//-----------------------------------------------------------------------------
// CClusterConfiguration
//-----------------------------------------------------------------------------
CConfiguration::CConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_clusterName(clusterName)
	, m_name(name)
{
}

CConfiguration::~CConfiguration()
{
}

//-----------------------------------------------------------------------------
// CInstanceConfiguration
//-----------------------------------------------------------------------------
CInstanceConfiguration::CInstanceConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name)
	: CConfiguration(edif, libraryName, cellName, clusterName, name)
{
   m_clusterConfigurationRef.Empty();
}

CInstanceConfiguration::~CInstanceConfiguration()
{
}

//-----------------------------------------------------------------------------
// CClusterConfiguration
//-----------------------------------------------------------------------------
CClusterConfiguration::CClusterConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CString name)
	: CConfiguration(edif, libraryName, cellName, clusterName, name)
	, m_instanceConfigurationMap(nextPrime2n(20),true)
{
	m_viewRef.Empty();
}

CClusterConfiguration::CClusterConfiguration(CEdif &edif, const CString libraryName, const CString cellName, const CString clusterName, const CClusterConfiguration& configuration)
	: CConfiguration(edif, libraryName, cellName, clusterName, configuration.m_name)
	, m_instanceConfigurationMap(nextPrime2n(20),true)
{
   m_viewRef = configuration.m_viewRef;

   for (POSITION pos = configuration.m_instanceConfigurationMap.GetStartPosition(); pos != NULL;)
   {
      CInstanceConfiguration* instanceConfiguration = NULL;
      CString key;
      configuration.m_instanceConfigurationMap.GetNextAssoc(pos, key, instanceConfiguration);
      if (instanceConfiguration == NULL)
         continue;

      CInstanceConfiguration* newInstanceConfiguration = new CInstanceConfiguration(m_edif, m_libraryName, m_cellName, m_clusterName, instanceConfiguration->getName());
      m_instanceConfigurationMap.SetAt(newInstanceConfiguration->getName(), newInstanceConfiguration);
   }
} 

CClusterConfiguration::~CClusterConfiguration()
{
	m_instanceConfigurationMap.empty();
}

CInstanceConfiguration* CClusterConfiguration::addInstanceConfiguration(const CString configurationName)
{
	CInstanceConfiguration* configuration = findInstanceConfiguration(configurationName);
	if (configuration == NULL)
	{
		configuration = new CInstanceConfiguration(m_edif, m_libraryName, m_cellName, m_name, configurationName);
		m_instanceConfigurationMap.SetAt(configurationName, configuration);
	}
	return configuration;
}

CInstanceConfiguration* CClusterConfiguration::findInstanceConfiguration(const CString configurationName)
{
	CInstanceConfiguration* configuration = NULL;
	m_instanceConfigurationMap.Lookup(configurationName, configuration);
	return configuration;
}

CEdifView* CClusterConfiguration::getView()
{
   return m_edif.findView(m_libraryName, m_cellName, m_clusterName, m_viewRef);
}

//-----------------------------------------------------------------------------
// CEdifCluster
//-----------------------------------------------------------------------------
CEdifCluster::CEdifCluster(CEdif &edif, const CString libraryName, const CString cellName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_name(name)
	, m_viewMap(nextPrime2n(20),true)
	, m_symbolMap(nextPrime2n(20),true)
	, m_clusterConfigurationMap(nextPrime2n(20),true)
{
	reset();
}

CEdifCluster::CEdifCluster(CEdifCluster& cluster)
   : m_edif(cluster.m_edif)
{
   m_libraryName = cluster.m_libraryName;
   m_cellName = cluster.m_cellName;
   m_name = cluster.getNextCloneName();
   *this = cluster;
	m_derivedNumber = cluster.getCloneCount();
}

CEdifCluster& CEdifCluster::operator =(const CEdifCluster &cluster)
{
   if (&cluster != this)
   {
      reset();

      m_currentView = NULL;
      m_currentSymbol = NULL;
      m_isHierarchyStructure = cluster.m_isHierarchyStructure;
      m_cloneCount = 0;
      m_defaultConfigurationName = cluster.m_defaultConfigurationName;

      for (POSITION pos = cluster.m_viewMap.GetStartPosition(); pos != NULL;)
      {
         CEdifView* view = NULL;
         CString key;
         cluster.m_viewMap.GetNextAssoc(pos, key, view);
         if (view == NULL)
            continue;

         CEdifView* copy = new CEdifView(m_edif, m_libraryName, m_cellName, m_name, *view);
         m_viewMap.SetAt(key, copy);
      }

      for (POSITION pos = cluster.m_symbolMap.GetStartPosition(); pos != NULL;)
      {
         CEdifSymbol* symbol = NULL;
         CString key;
         cluster.m_symbolMap.GetNextAssoc(pos, key, symbol);
         if (symbol == NULL)
            continue;

         CEdifSymbol* copy = new CEdifSymbol(*symbol);
         m_symbolMap.SetAt(key, copy);
      }

      for (POSITION pos = cluster.m_clusterConfigurationMap.GetStartPosition(); pos != NULL;)
      {
         CClusterConfiguration* configuration = NULL;
         CString key;
         cluster.m_clusterConfigurationMap.GetNextAssoc(pos, key, configuration);
         if (configuration == NULL)
            continue;

         CClusterConfiguration* copy = new CClusterConfiguration(m_edif, m_libraryName, m_cellName, m_name, *configuration);
         m_clusterConfigurationMap.SetAt(key, copy);
      }
    }

   return *this;
}

CEdifCluster::~CEdifCluster()
{
	reset();
}

void CEdifCluster::reset()
{
	m_viewMap.empty();
	m_symbolMap.empty();
	m_clusterConfigurationMap.empty();

	m_currentView = NULL;
	m_currentSymbol = NULL;
	m_isHierarchyStructure = false;
   m_cloneCount = 0;
	m_derivedNumber = 0;
   m_defaultConfigurationName.Empty();
}

CString CEdifCluster::getNextCloneName()
{
   CString retval;
   retval.Format("%s_%d", m_name, ++m_cloneCount);
   return retval;
}

CEdifView* CEdifCluster::addView(const CString viewName)
{
	CEdifView* view = findView(viewName);
	if (view == NULL)
	{
		view = new CEdifView(m_edif, m_libraryName, m_cellName, m_name, viewName);
		m_viewMap.SetAt(viewName, view);
	}
	return view;
}

CEdifView* CEdifCluster::findView(const CString viewName)
{
	CEdifView* view = NULL;
	m_viewMap.Lookup(viewName, view);
	return view;
}

CEdifView* CEdifCluster::getFirstView()
{
	CEdifView* view = NULL;
	POSITION pos = m_viewMap.GetStartPosition();
	if (pos != NULL)
	{
		CString key;
		m_viewMap.GetNextAssoc(pos, key, view);
	}
	return view;
}

CEdifSymbol* CEdifCluster::addSymbol(const CString symbolName)
{
	CEdifSymbol* symbol = findSymbol(symbolName);
	if (symbol == NULL)
	{
		symbol = new CEdifSymbol(m_edif, m_libraryName, m_cellName, m_name, symbolName);
		m_symbolMap.SetAt(symbolName, symbol);
	}
	return symbol;
}

CEdifSymbol* CEdifCluster::findSymbol(const CString symbolName)
{
	CEdifSymbol* symbol = NULL;
	m_symbolMap.Lookup(symbolName, symbol);
	return symbol;
}

CClusterConfiguration* CEdifCluster::addClusterConfiguration(const CString configurationName)
{
	CClusterConfiguration* configuration = findClusterConfiguration(configurationName);
	if (configuration == NULL)
	{
		configuration = new CClusterConfiguration(m_edif, m_libraryName, m_cellName, m_name, configurationName);
		m_clusterConfigurationMap.SetAt(configurationName, configuration);
	}
	return configuration;
}

CClusterConfiguration* CEdifCluster::findClusterConfiguration(const CString configurationName)
{
	CClusterConfiguration* configuration = NULL;
	m_clusterConfigurationMap.Lookup(configurationName, configuration);
	return configuration;
}

CClusterConfiguration* CEdifCluster::getDefaultConfiguration()
{
	CClusterConfiguration* configuration = NULL;
	m_clusterConfigurationMap.Lookup(m_defaultConfigurationName, configuration);
	return configuration;
}

CString CEdifCluster::getDelimitedPageBlockNumber(const CString delimiter) const
{
   CString retval;
   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdifView* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);

      for (POSITION pagePos = view->getPageStartPosition(); pagePos != NULL;)
      {
         BlockStruct* pageBlock = view->getNextPage(pagePos);
         if (pageBlock != NULL)
         {
            if (retval.IsEmpty())
               retval.Format("%d", pageBlock->getBlockNumber());
            else
               retval.AppendFormat("%s%d", delimiter, pageBlock->getBlockNumber());
         }
      }
   }
   return retval;
}

void CEdifCluster::attachHierarchySymbolToView()
{
	if (m_viewMap.GetCount() == 0 || m_symbolMap.GetCount() == 0)
	{
		m_isHierarchyStructure = false;
		return;
	}

   int hierarchyGeomNumKw = sDoc->RegisterKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0, valueTypeString);
   int hierarchyGeomKw = sDoc->RegisterKeyWord(SCH_ATT_HIERARCHGEOM, 0, valueTypeString);
	bool isHierarchyStructure = false;
	for (POSITION viewPos = m_viewMap.GetStartPosition(); viewPos != NULL;)
	{
		CEdifView* view = NULL;
		CString key;
		m_viewMap.GetNextAssoc(viewPos, key, view);
		if (view == NULL)
			continue;

		for (POSITION pagePos = view->getPageStartPosition(); pagePos != NULL;)
		{
			BlockStruct* pageBlock = view->getNextPage(pagePos);
			if (pageBlock == NULL)
				continue;

			CString geomNumString;
			geomNumString.Format("%d", pageBlock->getBlockNumber());

			for (POSITION symbolPos = m_symbolMap.GetStartPosition(); symbolPos != NULL;)
			{
				CEdifSymbol* symbol = NULL;
				m_symbolMap.GetNextAssoc(symbolPos, key, symbol);
				if (symbol == NULL)
					continue;

				BlockStruct* symbolBlock = symbol->getSymbolBlock();
				sDoc->SetAttrib(&symbolBlock->getAttributesRef(), hierarchyGeomNumKw, valueTypeString, geomNumString.GetBuffer(0), attributeUpdateAppend, NULL);
				isHierarchyStructure = true;
			}

			if (isHierarchyStructure)
			{
				CString value;
				sDoc->SetAttrib(&pageBlock->getAttributesRef(), hierarchyGeomKw, valueTypeString, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
			}
		}
	}

	m_isHierarchyStructure = isHierarchyStructure;
}

void CEdifCluster::convertLogicalSchematicToPhysicalSchematic()
{
   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdifView* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      if (view == NULL)
         continue;

      view->convertLogicalSchematicToPhysicalSchematic();
   }
}

void CEdifCluster::removedUnsedViewBlocks()
{
   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdifView* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      if (view == NULL)
         continue;

      view->removedAllPageBlocks();
   }
}

void CEdifCluster::createNetList()
{
	if (m_isHierarchyStructure)
	{
		// Hierarchy clusters does not creat net list here.  They will be call to create netlist from with in view->createNetList()
		return;
	}

   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdifView* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      if (view == NULL)
         continue;
   
      view->createNetList();
   }
}

void CEdifCluster::propagateNetsFromInstanceToHierarchyStructure(CInstance* instance)
{
   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdifView* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      if (view == NULL)
         continue;

		view->propagateNetsFromInstanceToHierarchyStructure(instance);
	}
}

//-----------------------------------------------------------------------------
// CEdifCell
//-----------------------------------------------------------------------------
CEdifCell::CEdifCell(CEdif &edif, const CString libraryName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_name(name)
	, m_clusterMap(nextPrime2n(20),true)
{
	reset();
}

CEdifCell::~CEdifCell()
{
	reset();
}

void CEdifCell::reset()
{
	m_clusterMap.empty();
	m_currentCluster = NULL;
}

CEdifCluster* CEdifCell::addCluster(const CString clusterName)
{
	CEdifCluster* cluster = findCluster(clusterName);
	if (cluster == NULL)
	{
		cluster = new CEdifCluster(m_edif, m_libraryName, m_name, clusterName);
		m_clusterMap.SetAt(clusterName, cluster);
	}
	return cluster;
}

CEdifCluster* CEdifCell::addCluster(CEdifCluster* cluster)
{
   m_clusterMap.SetAt(cluster->getName(), cluster);
   return cluster;
}

CEdifCluster* CEdifCell::findCluster(const CString clusterName)
{
	CEdifCluster* cluster = NULL;
	m_clusterMap.Lookup(clusterName, cluster);
	return cluster;
}

CEdifCluster* CEdifCell::getFirstCluster()
{
	CEdifCluster* cluster = NULL;
	POSITION pos = m_clusterMap.GetStartPosition();
	if (pos != NULL)
	{
		CString key;
		m_clusterMap.GetNextAssoc(pos, key, cluster);
	}
	return cluster;
}

BlockStruct* CEdifCell::getFirstPage()
{
	BlockStruct* page = NULL;
	CEdifCluster* cluster = getFirstCluster();
	if (cluster != NULL)
	{
		CEdifView* view = cluster->getFirstView();
		if (view != NULL)
		{
			 page = view->getFirstPage();
		}
	}

	return page;
}

void CEdifCell::convertLogicalSchematicToPhysicalSchematic()
{
   for (POSITION pos = m_clusterMap.GetStartPosition(); pos != NULL;)
   {
      CEdifCluster* cluster = NULL;
      CString key;
      m_clusterMap.GetNextAssoc(pos, key, cluster);
      if (cluster == NULL)
         continue;

      cluster->convertLogicalSchematicToPhysicalSchematic();
   }
}

void CEdifCell::removedUnusedOriginalCluster()
{
   for (POSITION pos = m_clusterMap.GetStartPosition(); pos != NULL;)
   {
      POSITION curPos = pos;
      CEdifCluster* cluster = NULL;
      CString key;
      m_clusterMap.GetNextAssoc(pos, key, cluster);
      if (cluster == NULL)
         continue;

      if (cluster->getCloneCount() > 0)
      {
         // Original cluster that has been clonsed and no longer used so remove
         cluster->removedUnsedViewBlocks();
         delete cluster;
         cluster = NULL;
         m_clusterMap.RemoveKey(key);
      }
   }
}

void CEdifCell::createNetList()
{
   for (POSITION pos = m_clusterMap.GetStartPosition(); pos != NULL;)
   {
      CEdifCluster* cluster = NULL;
      CString key;
      m_clusterMap.GetNextAssoc(pos, key, cluster);
      if (cluster == NULL || cluster->isHierarchStructure())
         continue;

      cluster->createNetList();
   }
}

//-----------------------------------------------------------------------------
// CEdifLibrary
//-----------------------------------------------------------------------------
CEdifLibrary::CEdifLibrary(CEdif &edif, const CString name)
	: m_edif(edif)
	, m_name(name)
	, m_cellMap(nextPrime2n(20),true)
{
	reset();
}

CEdifLibrary::~CEdifLibrary()
{
	reset();
}

void CEdifLibrary::reset()
{
	m_cellMap.empty();
	m_currentCell = NULL;
}

CEdifCell* CEdifLibrary::addCell(const CString cellName)
{
	CEdifCell* cell = fineCell(cellName);
	if (cell == NULL)
	{
		cell = new CEdifCell(m_edif, m_name, cellName);
		m_cellMap.SetAt(cellName, cell);
	}
	setCurrentCell(cell);
	return cell;
}

CEdifCell* CEdifLibrary::fineCell(const CString cellName)
{
	CEdifCell* cell = NULL;
	m_cellMap.Lookup(cellName, cell);
	return cell;
}

void CEdifLibrary::convertLogicalSchematicToPhysicalSchematic()
{
   for (POSITION pos = m_cellMap.GetStartPosition(); pos != NULL;)
   {
      CEdifCell* cell = NULL;
      CString key;
      m_cellMap.GetNextAssoc(pos, key, cell);
      if (cell == NULL)
         continue;

      cell->convertLogicalSchematicToPhysicalSchematic();
   }
}

void CEdifLibrary::removedUnusedOriginalCluster()
{
   for (POSITION pos = m_cellMap.GetStartPosition(); pos != NULL;)
   {
      CEdifCell* cell = NULL;
      CString key;
      m_cellMap.GetNextAssoc(pos, key, cell);
      if (cell == NULL)
         continue;

      cell->removedUnusedOriginalCluster();
   }
}

void CEdifLibrary::createNetList()
{
   for (POSITION pos = m_cellMap.GetStartPosition(); pos != NULL;)
   {
      CEdifCell* cell = NULL;
      CString key;
      m_cellMap.GetNextAssoc(pos, key, cell);
      if (cell == NULL)
         continue;

      cell->createNetList();
   }
}

//-----------------------------------------------------------------------------
// CEdif
//-----------------------------------------------------------------------------
CEdif::CEdif()
	:m_libraryMap(nextPrime2n(20),true)
{
	reset();
}

CEdif::~CEdif()
{
	reset();
}

void CEdif::reset()
{
	m_libraryMap.empty();
	m_currentLibrary = NULL;
   m_isSchematicConvertionDone = false;

	m_localUniqueNetNameMap.RemoveAll();
	m_localUniqueBusNameMap.RemoveAll();
}

CEdifLibrary* CEdif::addLibrary(const CString libraryName)
{
	CEdifLibrary* library = findLibrary(libraryName);
	if (library == NULL)
	{
		library = new CEdifLibrary(*this, libraryName);
		m_libraryMap.SetAt(libraryName, library);
	}
	setCurrentLibrary(library);
	return library;
}

CEdifLibrary* CEdif::findLibrary(const CString libraryName)
{
	CEdifLibrary* library = NULL;
	m_libraryMap.Lookup(libraryName, library);
	return library;
}

CEdifCell* CEdif::findCell(const CString libraryName, const CString cellName)
{
	CEdifLibrary* library = findLibrary(libraryName);
	if (library == NULL)
		return NULL;
	else
		return library->fineCell(cellName);
}

CEdifCluster* CEdif::findCluster(const CString libraryName, const CString cellName, const CString clusterName)
{
	CEdifCell* cell = findCell(libraryName, cellName);
	if (cell == NULL)
		return NULL;
	else 
		return cell->findCluster(clusterName);
}

CEdifView* CEdif::findView(const CString libraryName, const CString cellName, const CString clusterName, const CString viewName)
{
	CEdifCluster* cluster = findCluster(libraryName, cellName, clusterName);
	if (cluster == NULL)
		return NULL;
	else
		return cluster->findView(viewName);
}

CClusterConfiguration* CEdif::findClusterConfiguration(const CString libraryName, const CString cellName, const CString clusterName, const CString configurationName)
{
	CEdifCluster* cluster = findCluster(libraryName, cellName, clusterName);
	if (cluster == NULL)
		return NULL;
	else
		return cluster->findClusterConfiguration(configurationName);
}

CEdifCell* CEdif::getCurrentCell()
{
   CEdifLibrary* library = getCurrentLibrary();
   if (library != NULL)
      return library->getCurrentCell();
   else
      return NULL;
}

CEdifCluster* CEdif::getCurrentCluster()
{
   CEdifCell* cell = getCurrentCell();
   if (cell != NULL)
      return cell->getCurrentCluster();
   else
      return NULL;
}

CEdifView* CEdif::getCurrentView()
{
   CEdifCluster* cluster = getCurrentCluster();
   if (cluster != NULL)
      return cluster->getCurrentView();
   else
      return NULL;
}

CEdifSymbol* CEdif::getCurrentSymbol()
{
   CEdifCluster* cluster = getCurrentCluster();
   if (cluster != NULL)
      return cluster->getCurrentSymbol();
   else
      return NULL;
}

CNet* CEdif::getCurrentNet()
{
   CEdifView* view = getCurrentView();
   if (view != NULL)
      return view->getCurrentNet();
   else
      return NULL;
}

CBus* CEdif::getCurrentBus()
{
   CEdifView* view = getCurrentView();
   if (view != NULL)
      return view->getCurrentBus();
   else
      return NULL;
}

CString CEdif::getUniqueLocalNetName(const CString netName)
{
	CString uniqueNetName = netName;
	int count = 0;

	while (m_localUniqueNetNameMap.Lookup(uniqueNetName, uniqueNetName))
	{
		uniqueNetName.Format("%s_%d", netName, ++count);
	}
	m_localUniqueNetNameMap.SetAt(uniqueNetName, uniqueNetName);

	return uniqueNetName;
}

CString CEdif::getUniqueLocalBusName(const CString busName)
{
	CString uniqueBusName = busName;
	int count = 0;

	while (m_localUniqueBusNameMap.Lookup(uniqueBusName, uniqueBusName))
	{
		uniqueBusName.Format("%s_%d", busName, ++count);
	}
	m_localUniqueBusNameMap.SetAt(uniqueBusName, uniqueBusName);

	return uniqueBusName;
}

void CEdif::createPhysicalSchematicWithLogicalConnectivity()
{
   //convertLogicalSchematicToPhysicalSchematic();
   //createNetList();
}

void CEdif::convertLogicalSchematicToPhysicalSchematic()
{
   // This function is called in the function design() for the first occurrance of "design" in Edif file

   for (POSITION pos = m_libraryMap.GetStartPosition(); pos != NULL;)
   {
      CEdifLibrary* library = NULL;
      CString key;
      m_libraryMap.GetNextAssoc(pos, key, library);
      if (library == NULL)
         continue;

      library->convertLogicalSchematicToPhysicalSchematic();
   }

   // Now loop through all library and removed original clusters that created clones are no longer used
   for (POSITION pos = m_libraryMap.GetStartPosition(); pos != NULL;)
   {
      CEdifLibrary* library = NULL;
      CString key;
      m_libraryMap.GetNextAssoc(pos, key, library);
      if (library == NULL)
         continue;

      library->removedUnusedOriginalCluster();
   }   

   m_isSchematicConvertionDone = true;
}

void CEdif::createNetList()
{
   // This function is called in the function edif300() after the reading of Edif file is completed

 //  return; // <== take out this when function is completed

   for (POSITION pos = m_libraryMap.GetStartPosition(); pos != NULL;)
   {
      CEdifLibrary* library = NULL;
      CString key;
      m_libraryMap.GetNextAssoc(pos, key, library);
      if (library == NULL)
         continue;

      library->createNetList();
   }
}

//-----------------------------------------------------------------------------
// CAttributeVisibilityMap
//-----------------------------------------------------------------------------
void CAttributeVisibilityMap::addAttributeVisibility(const CString keyword, const double height, const double width)
{
   CAttributeVisibility* attribVisible = NULL;                       
   if (!m_attribVisibleMap.Lookup(keyword, attribVisible))
   {
      attribVisible = new CAttributeVisibility(keyword, height, width);           
      m_attribVisibleMap.SetAt(keyword, attribVisible);
   }
}

void CAttributeVisibilityMap::applyAttributeVisibility(CCEtoODBDoc& doc)
{
   for (POSITION pos = m_attribVisibleMap.GetStartPosition(); pos != NULL;)
   {
      CString key;
      CAttributeVisibility* attributeVisibility = NULL;
      m_attribVisibleMap.GetNextAssoc(pos, key, attributeVisibility);
      if (attributeVisibility == NULL)
         continue;

      WORD keywordIndex = (WORD)doc.IsKeyWord(attributeVisibility->getKeyword(), 0);
      int layerIndex = Graph_Level(attributeVisibility->getKeyword(), "AttrLyr_", FALSE);

      for (int i=0; i<doc.getMaxBlockIndex(); i++)
      {
         BlockStruct* block = doc.getBlockAt(i);
         if (block ==  NULL)
            continue;

         for (CDataListIterator dataList(*block, dataTypeInsert); dataList.hasNext();)
         {
            DataStruct* data = dataList.getNext();
            if (data->getAttributesRef() == NULL)
               continue;

            Attrib* attrib = NULL;
            if (data->getAttributesRef()->Lookup(keywordIndex, attrib) && !attrib->isVisible())
            {
		         attrib->setCoordinate(0.0, 0.0);
		         attrib->setRotationRadians(0.0);
		         attrib->setHeight(attributeVisibility->getHeight());
		         attrib->setWidth(attributeVisibility->getWidth());
		         attrib->setPenWidthIndex(doc.getZeroWidthIndex());
		         attrib->setVisible(true);
		         attrib->setProportionalSpacing(true);
		         attrib->setMirrorDisabled(true);
               attrib->setInherited(false);
			      attrib->setLayerIndex(layerIndex);
			      attrib->setHorizontalPosition(horizontalPositionCenter);
			      attrib->setVerticalPosition(verticalPositionCenter);
            }
         }
      }
   }
}
