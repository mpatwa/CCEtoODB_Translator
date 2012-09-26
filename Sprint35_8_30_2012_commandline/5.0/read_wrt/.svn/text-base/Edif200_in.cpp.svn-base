// $Header: /CAMCAD/4.6/read_wrt/Edif200_in.cpp 128   4/12/07 3:36p Lynn Phung $

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
# include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <string.h>    
#include "gauge.h"
#include "attrib.h"
#include "lyrmanip.h"
#include "Polylib.h"
#include "API.h"	// Use this to return the error code RC_INVALID_FORMAT
#include "CCEtoODB.h"
#include "Rwlib.h"
#include "Sch_Lib.h"
#include "Edif200_in.h"
#include "DcaLibGraphics.h"
#include "RwUiLib.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


extern CProgressDlg *progress;
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);


/* Static Variables *********************************************************/
static CCEtoODBDoc          *sDoc;
static Parser              *sParser;            // instance of Edif file parser
static FileStruct				*sFileStruct;        // CAMCAD file that use to store everything
static EdifAttrib          *sEdifAttrib;   
static CEdif200				sEdif200;


static CMapStringToString	sInstanceNameMap;		// used to keep find out if instance name is repeated
static CMapStringToPtr     sLibraryCellMap;     // Maps the library + cell name to pointer of cellToPageMap;
                                                // DoDesign use this to find the correct cell to show all the 
                                                // pages as top level schematic design

static CMapPtrToPtr		sPortRenameMap;		// use to keep track of ports that have their name renamed so  
																// at the end of the read we can change the value of the attribute
																// SCH_ATT_PORTNAME to the renamed name

static CMapStringToString  sCellTypeMap;        // Maps the library + cell name to the cell type
static int                 sPageUnit;
                           
static FILE                *sFileErr;
static long                sDisplayErr;         // current number of errors
static BOOL                sHasHierarchy;       // indicate the schematic has hierarchy structure
                                                      
// There variables store the layer index for the difference kind of graphics and attributes display
static int                 sTextLayer;
static int                 sPortDesLayer;
static int                 sSymbolDesLayer;
static int                 sPortGrpLayer;
static int                 sSymbolGrpLayer;
static int                 sSignalGrpLayer;
static int                 sBorderGrpLayer;
static int                 sAttribLayer;                 

// These are varible used for setting file
static BOOL                sOverWriteAttributeDisplay;   // indicate how if attribute display should be overwrite so always use the last display
static CStringArray        sDesignatorBackannotateArray; // name of attribute that will be use as designator
static CStringArray        sPinNumBackannotateArray;     // name of attribute that will be use as pin number
static CString             sDesignatorLocBackannotate;   // name of attribute's location that will be use as designator location
static CString             sPinNumLocBackannotate;       // name of attribute's location that will be use as pin number location
static double              sFontScale;                   // number to scale Edif text 
static BOOL                sExchangeXYScale;             // indicate whether to swap scaleX to scaleY and scaleY to scaleX
static CString             sNetBackannotate;             // name of attribute that will be use as the net annotated name
static bool                sAttribRotationLimit_0_90;    // limit attrib rotation to "readable" angles, 0 and 90
static bool                sCreateTieDot;                // Create TieDot
static double              sTieDotRadius;                // TieDot Radius
/* Function Prototypes *********************************************************/

static void InitializeMembers();
static void FreeMembers();
static void LoadSettingFile();
static void CreateRequiredLayer();
static void CreateDefaultGeometry();

static int StartEdif200();
static int GetScaleInteger(double &scaleValue);
static int GetUnitType(CString &unitType);
static int GetBoolean(bool &boolValue);
static int GetArray(CString &name, CString &renameString, Display &display, CString &arrayName, int &lbIndex, int &ubIndex, int &size);
static int GetNameDef(CString &name, CString &renameString, Display &display);
static int GetName(CString &name, Display &display);
static int GetString(CString &stringValue, const bool allowMultipleLine = false);
static int GetPoint(double &x, double &y);
static int GetPoints(CPntList &pntList);
static int GetCurve(CPntList &pntList);
static int GetCurve(CPntList &pntList);

static double ConvertEdifUnit(CString unitRef, double edifValue, void *basicUnitName);
static void convert_special_char(char *t);
static int FreeAttribMap(CAttributes** attribMap);
static int AttribUntransform(Attrib *attrib, Transform insertTransform);

static int GetKeywordDisplay(CAttributes** attribMap);
static int GetProperty(CAttributes** attribMap);
static int GetTypedValue(int &valueType, CString &value, Display &display);
static int GetBooleanPropertyValue(CString &value, Display &display);
static int GetIntegerPropertyValue(CString &value, Display &display);
static int GetMiNoMaxPropertyValue(CString &value, Display &display);
static int GetMinimalNonimalMaximal(CString &value);
static int GetNumberPropertyValue(CString &value, Display &display);
static int GetPointPropertyValue(CString &value, Display &display);
static int GetStringPropertyValue(CString &value, Display &display);
static int GetPropertyDisplay(CAttributes** attribMap);

static int CopyAttribMap(CAttributes** copyToMap, CAttributes* fromMap, bool overwrite = false);
static void CheckUniqueInstanceName();
static void RenamePortName();
static void AddAttribToCompPin();

static int doPortImplementation(CEdif200& edif, CString& portName, DataStruct** portDataStruct, CTypedMapStringToPtrContainer<CAttributes*>& portAttributeMap);
static int doConnectLocation(DataStruct** data);
static int doAnnotate();
static int doCommentGraphics(CEdif200& edif);
static int doInstance(CEdif200& edif, CEdif200Instance** instance);
static int doViewRef(CString& viewRef, CString& cellRef, CString& libraryRef);
static int doCellRef(CString& cellRef, CString& libraryRef);
static int doLibraryRef(CString& libraryRef);
static DataStruct *InsertSchematicSymbol(CEdif200& edif, CEdif200Instance& instance, Transform transform);
void CreateTieDots(double tieDotRadius);

/******************************************************************************
* ReadEDIF200
*/
int ReadEDIF200(CString fileName, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
	int res = 1;
   Doc->getSettings().Crosshairs = FALSE;
   sDoc = Doc;
   sPageUnit = pageunits;

   sParser = new Parser;
   if (!sParser->OpenFile(fileName))
      return res;

   // Open error log file
   CString edifLogFile = GetLogfilePath("edif.log");
   sFileErr = fopen(edifLogFile, "wt");
   sDisplayErr = 0;
   if (sFileErr == NULL)
   {
      CString msg = "";
      msg.Format("Error open [%s] file", edifLogFile);
      ErrorMessage(msg, "Error");
      return res;
   }
   fprintf(sFileErr, "File : %s\n", fileName);

   // Perpare for reader
   InitializeMembers();
   LoadSettingFile();
   CreateRequiredLayer();
   CreateDefaultGeometry();

   // Start reading file
   sEdifAttrib = new EdifAttrib;

	sEdif200.reset();
   res = sEdif200.startEdif();
   if (res == 1)
   {
      // If there is no view set to show and there is only one view, then set that one view to show
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
      else
      {
         if (!sFileStruct->isShown())
            sFileStruct->setShow(true);	   

         CString designatorBackAnnotate, pinNumBackAnnotate;
         AutoDetectSchematicDesignator(*sDoc, *sFileStruct, designatorBackAnnotate, pinNumBackAnnotate, sDesignatorBackannotateArray, sPinNumBackannotateArray);
			
         if (!designatorBackAnnotate.IsEmpty())
         {
            if (!pinNumBackAnnotate.IsEmpty() && pinNumBackAnnotate != SCH_ATT_DESIGNATOR)
            {
               // If pin designator Keyword is not the same as "DESIGNATOR" keyword, then rename
		         RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), designatorBackAnnotate, pinNumBackAnnotate, sDesignatorLocBackannotate, sPinNumLocBackannotate);
            }
            else if (designatorBackAnnotate != SCH_ATT_DESIGNATOR)
            {
               // If symbol designator Keyword is not the same as "DESIGNATOR" keyword, then rename
		         RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), designatorBackAnnotate, "", sDesignatorLocBackannotate, sPinNumLocBackannotate);
            }
            else if (!sDesignatorLocBackannotate.IsEmpty() || !sPinNumLocBackannotate.IsEmpty())
            {
               // If there is symbol or pin designator lcoation backAnnotate, then process it
		         RenameCompAndPinDesignator(sDoc, sFileStruct->getFileNumber(), designatorBackAnnotate, pinNumBackAnnotate, sDesignatorLocBackannotate, sPinNumLocBackannotate);
            }
         }


		   int err = CheckMissingOrDuplicateDesignator(sDoc, sFileStruct, sFileErr, sDisplayErr);
		   if (err == SCH_ERR_DESIGNATOR_BLANK || err == SCH_ERR_DESIGNATOR_DUPLICATE)
		   {
			   fprintf(sFileErr, "NOTE: This schematic might not be suitable for use with Schematic Link because some instances are either missing designator or has duplicate designator.\n");
			   sDisplayErr++;
		   }

         sDoc->OnFitPageToImage();
      }
   }
   else if (res == -1)
   {
      CString msg = "";
      msg.Format("Import of file terminated because EDIF syntax error encountered close to %ld\n", sParser->lineCount);
      
      if (sDisplayErr)
         msg.Format("See %s for messages.", edifLogFile );
      ErrorMessage(msg, "EDIF Read Error");
   }

   // remove empty EDIF file block
   if(NULL != sFileStruct)
   {
      BlockStruct *originalBlock = sFileStruct->getOriginalBlock();
      if(originalBlock && originalBlock->getDataCount() == 0)
      {
         sDoc->FreeBlock(originalBlock);
         sFileStruct->setOriginalBlock(sFileStruct->getBlock());
      }
   }

   // Release resource
   sParser->CloseFile();
   fclose(sFileErr);
   FreeMembers();

   if (sDisplayErr)
      Logreader(edifLogFile);

   return res;
}

static void InitializeMembers()
{
   sFileStruct = NULL;
	sInstanceNameMap.RemoveAll();
   sLibraryCellMap.RemoveAll();
   sCellTypeMap.RemoveAll();
	sPortRenameMap.RemoveAll();
   sHasHierarchy = FALSE;
}

static void FreeMembers()
{
   delete sParser;
   sParser = NULL;

   delete sEdifAttrib;
   sEdifAttrib = NULL;

   sFileStruct = NULL;
	sInstanceNameMap.RemoveAll();

	CString key = "";
	void *voidPtr;
   POSITION pos = sLibraryCellMap.GetStartPosition();
   while (pos)
   {
      sLibraryCellMap.GetNextAssoc(pos, key, voidPtr);
      CMapStringToPtr *viewPageMap = (CMapStringToPtr*)voidPtr;
      viewPageMap->RemoveAll();
      delete viewPageMap;
   }
   sLibraryCellMap.RemoveAll();

   sCellTypeMap.RemoveAll();

	void *renamePtr;
	pos = sPortRenameMap.GetStartPosition();
	while (pos)
	{
		sPortRenameMap.GetNextAssoc(pos, voidPtr, renamePtr);
		delete renamePtr;
	}
	sPortRenameMap.RemoveAll();
}

static CString getValueFromPortNameAttrib(CAttributes* attributeMap)
{
   CString retval;
   Attrib* attrib = NULL;
   WORD portNameKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_PORTNAME, 0, valueTypeString);

   if (attributeMap != NULL && attributeMap->Lookup(portNameKw, attrib) && attrib != NULL)
   {
#if CamCadMajorMinorVersion > 406  //  > 4.6
      retval = attrib->getStringValue();
#else
      retval = sDoc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
   }

   return retval;
}

static CString getValueFromDesignatorAttrib(CAttributes* attributeMap)
{
   CString retval;
   Attrib* attrib = NULL;
   WORD designatorKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, valueTypeString);

   if (attributeMap != NULL && attributeMap->Lookup(designatorKw, attrib) && attrib != NULL)
   {
#if CamCadMajorMinorVersion > 406  //  > 4.6
      retval = attrib->getStringValue();
#else
      retval = sDoc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
   }

   return retval;
}

static CString getValueFromPrimaryNameAttrib(CAttributes* attributeMap)
{
   CString retval;
   Attrib* attrib = NULL;
   WORD primaryNamerKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_PRIMARYNAME, 0, valueTypeString);

   if (attributeMap != NULL && attributeMap->Lookup(primaryNamerKw, attrib) && attrib != NULL)
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
   sDesignatorLocBackannotate = "";
   sPinNumLocBackannotate = "";
   sFontScale = 1.0;
   sExchangeXYScale = FALSE;
   sAttribRotationLimit_0_90 = true;
   sCreateTieDot = false;
   sTieDotRadius = 0.0;

   FILE *fp;
   char line[255];
   char *lp;

   CString settingsFile( getApp().getImportSettingsFilePath("Edif200.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nEDIF200 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);

   if ((fp = fopen(settingsFile, "rt")) == NULL)
   {
      // no settings file found 
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      ErrorMessage(tmp, "Edif 200 Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".OVERWRITE_ATTRIBUTE_DISPLAY"))
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
         else if (!STRICMP(lp, ".LIMIT_ATTRIB_ROTATION_0_90"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            
            sAttribRotationLimit_0_90 = (toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".CREATE_TIE_DOT"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            
            sCreateTieDot = (toupper(lp[0]) == 'Y');
         }
         else if (!STRICMP(lp, ".TIE_DOT_RADIUS"))
         {
            int unit  = sPageUnit;
            char *lp1;
            
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            sTieDotRadius = atof(lp);
            if ((lp1 = get_string(NULL, " \t\n")) != NULL)
            {
               if (!STRNICMP(lp1, "IN", 2))
                  unit = UNIT_INCHES;
               else if (!STRNICMP(lp1, "MM", 2))
                  unit = UNIT_MM;
               else if (!STRNICMP(lp1, "MI", 2))
                  unit = UNIT_MILS;
            }
            sTieDotRadius = sTieDotRadius * Units_Factor(unit, sPageUnit);
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
   double x = 0.5 * Units_Factor(UNIT_MM, sPageUnit);
   double y = 0.5 * Units_Factor(UNIT_MM, sPageUnit);

   // create default geometry for ports which as a graphic of an X
   BlockStruct *block = Graph_Block_On(GBO_APPEND, "DEFAULTPORT", -1, 0L);
   block->setBlockType(blockTypeGatePort);
   Graph_PolyStruct(sPortGrpLayer, 0L, FALSE);

   Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
   Graph_Vertex(-x, -y, 0.0);
   Graph_Vertex(x, y, 0.0);

   Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
   Graph_Vertex(-x, y, 0.0);
   Graph_Vertex(x, -y, 0.0);  
   Graph_Block_Off();
}

/******************************************************************************
* GetDouble
*/
static int GetScaleInteger(double &scaleValue)
{
   scaleValue = 0.0;
   if (!sParser->GetToken())     // get the '('
      return sParser->ParsingErr();

   if (sParser->token[0] == '(')
   {
      if (!sParser->GetToken())  // get the 'e'
         return sParser->ParsingErr();
      if (sParser->token[0] != 'e' && sParser->token[0] != 'E')
         return sParser->ParsingErr();

      if (!sParser->GetToken())  // get the mantissa
         return sParser->ParsingErr();
      double mantissa = atof(sParser->token);

      if (!sParser->GetToken()) // get the exponent
         return sParser->ParsingErr();
      double exponent = atof(sParser->token);

      if (!sParser->GetToken()) // get the ")"
         return sParser->ParsingErr();

      scaleValue = mantissa * pow(10., exponent);
   }
   else
      scaleValue = atof(sParser->token);

   return 1;   
}

/******************************************************************************
* GetString
*/
static int GetUnitType(CString &unitType)
{
   if (!sParser->GetToken())     // get the '('
      return sParser->ParsingErr();
   if (sParser->token[0] != '(')
      return sParser->ParsingErr();

   if (!sParser->GetToken())     // get the word 'unit'
      return sParser->ParsingErr();
   if (STRCMPI(sParser->token, "unit"))
      return sParser->ParsingErr();

   if (!sParser->GetToken())     // get the unit type
      return sParser->ParsingErr();
   unitType = sParser->token;

   if (!sParser->GetToken())     // get the ')'
      return sParser->ParsingErr();
   if (sParser->token[0] != ')')
      return sParser->ParsingErr();

   return 1;   
}

/******************************************************************************
* GetBoolean
*/
static int GetBoolean(bool &boolValue)
{
   if (!sParser->GetToken())     // get the '('
      return sParser->ParsingErr();
   if (sParser->token[0] != '(')
      return sParser->ParsingErr();

   if (!sParser->GetToken())     // get the word 'true' or 'false'
      return sParser->ParsingErr();
   if (!STRCMPI(sParser->token, "false"))
      boolValue = false;
   else if (!STRCMPI(sParser->token, "true"))
      boolValue = true;
   else
   {
      fprintf(sFileErr, "Syntax Error : Token \"%s\" on line %ld for boolean value.  Expecting TRUE or FALSE.\n", sParser->token, sParser->lineCount);
      sDisplayErr++;
      return -1;
   }

   if (!sParser->GetToken())     // get the ')'
      return sParser->ParsingErr();
   if (sParser->token[0] != ')')
      return sParser->ParsingErr();

   return 1;
}

/******************************************************************************
* GetString
*/
static int GetString(CString &stringValue, const bool allowMultipleLine)
{
   stringValue = "";
   if (!sParser->GetToken())
      return sParser->ParsingErr();

   if (sParser->token[0] == '(' && !sParser->isTokenName)
   {
      sParser->PushToken();
      return 0;
   }
   else
   {
      stringValue = sParser->token;
      //dts0100388737 - remove this because it causes incorrect display in many places 
      //stringValue.Replace("%", "%%");

      // Added support of multiple line in text and attributes
      if (!allowMultipleLine)
         stringValue.Remove('\n');

      stringValue.Trim();

      // Make the string upper case if it is not inside a pair quotes
      if (!sParser->isTokenName)
         stringValue.MakeUpper();
      return 1;
   }
}

/******************************************************************************
* GetArray
	
	- Get the array name and the size of the first index
	- Current only support one dimension array

	Parameter:
	name				= original name given to the array
	renameString	= renamed string given to the array
	display			= display information for the given name to the array
	arrayName		= the name without the set of "[]" or "()", which is used as the member name
	size				= the size of the array, always zero base, so size is one greater than highest index
*/
static int GetArray(CString &name, CString &renameString, Display &display, CString &arrayName, int &lbIndex, int &ubIndex, int &size)
{
	lbIndex = 0;
	ubIndex = 0;
	size = -1;
	int res = 0;

   // Look at next token to see if it is the begining of a construct
   if (sParser->peekToken[0] == '(')
   {   
      if (!sParser->GetToken())        // Get the '('
         return sParser->ParsingErr();

      // Look at next token to see if it is a "array" construct
      if (!STRCMPI(sParser->peekToken, "array"))   
      {
         if (!sParser->GetToken())     // Get the word 'array'
            return sParser->ParsingErr();

         // Get the name
         if ((res = GetNameDef(name, renameString, display)) < 0)
            return res;

			CString tmpArrayName = name;
			if (!renameString.IsEmpty())
				tmpArrayName = renameString;

			int index = -1;
			if ((index = tmpArrayName.ReverseFind('[')) > -1)
			{
				// Find the "[" and Remove the set of "[]"
				int curPos = 0;
				arrayName = tmpArrayName.Tokenize("[", curPos);
				if (!arrayName.IsEmpty())
				{
					CString tmp = tmpArrayName.Tokenize(".:", curPos); // some array name is TEST[1:2] or TEST[1..2]
					if (is_number(tmp))
					{
						lbIndex = atoi(tmp);
						tmp = tmpArrayName.Tokenize("]", curPos);
						tmp.Replace(".", "");	// drop the ".", just in case the array name is TEST[1..2];
						if (is_number(tmp))
							ubIndex = atoi(tmp);
					}
				}
			}
			else if ((index = tmpArrayName.ReverseFind('(')) > -1)
			{
				// Find the "(" and Remove the set of "()"
				int curPos = 0;
				arrayName = tmpArrayName.Tokenize("(", curPos);
				if (!arrayName.IsEmpty())
				{
					CString tmp = tmpArrayName.Tokenize(".:", curPos);	// some array name is TEST(1:2) or TEST(1..2)
					if (is_number(tmp))
					{
						lbIndex = atoi(tmp);
						tmp = tmpArrayName.Tokenize(")", curPos);
						tmp.Replace(",", "");	// drop the ".", just in case the array name is TEST(1..2);
						if (is_number(tmp))
							ubIndex = atoi(tmp);
					}
				}
			}


			// Get the size
			double tmpSize = -1.0;
			if ((res = GetScaleInteger(tmpSize)) < 0)
				return res;

			if (tmpSize > -1)
				size = (int)tmpSize;
			else
				size = -1;

			if (abs(ubIndex - lbIndex) + 1 != size)
			{
            lbIndex = 1;
            ubIndex = size;

				//fprintf(sFileErr, "Syntax Error : array \"%s\" close to line %ld, lower and upper bound is invalid, reset to [ZERO to SIZE - 1]==>[0:%d]\n", tmpArrayName, sParser->lineCount, size-1);
				//sDisplayErr++;
			}

         if (!sParser->GetToken())     // Get the ')'
            return sParser->ParsingErr();
         if (STRCMPI(sParser->token, ")"))
            return sParser->ParsingErr();       
		}
      // The next token is other construct
		else
		{
         sParser->PushToken();
         return 0;
		}
	}
	else
	{
		// Not a begining of a possible array construct
		return 0;
	}

	return 1;
}

/******************************************************************************
* GetName

   - Get the reference name and it display information if it is available
   - If display is NULL, then don't get display information, skip it
*/
static int GetNameDef(CString &name, CString &renameString, Display &display)
{
   int res = 0;

   // Look at next token to see if it is the begining of a construct
   if (sParser->peekToken[0] == '(' || STRCMPI(sParser->token, "(") == 0)
   {   
      if (!sParser->GetToken())        // Get the '('
         return sParser->ParsingErr();

      // Look at next token to see if it is a "name" construct
      if (!STRCMPI(sParser->peekToken, "name") || !STRCMPI(sParser->peekToken, "stringDisplay"))   
      {
         if (!sParser->GetToken())     // Get the word 'name' or 'stringDisplay'
            return sParser->ParsingErr();

         if ((res = GetName(name, display)) < 0)
            return res; 
      
         if (!sParser->GetToken())     // Get the ')'
            return sParser->ParsingErr();
         if (STRCMPI(sParser->token, ")"))
            return sParser->ParsingErr(); 
      
      }
      // Look at next token to see if it is a "member" construct
      else if (!STRCMPI(sParser->peekToken, "member"))
      {
         if (!sParser->GetToken())     // Get the word 'member'
            return sParser->ParsingErr();

         CString tmpString = "";

         // Get the name
         if ((res = GetNameDef(name, tmpString, display)) < 0)
            return res;

         // Loop through any integer number and get the ')'
         while (TRUE)
         {
            if (!sParser->GetToken())
               return sParser->ParsingErr();
            if (!STRCMPI(sParser->token, ")"))
               break;
            else
            {
               if ((res = sParser->SkipCommand()) < 0)
                  return res;
            }
         }
      }
      // Look at next token to see if it is a "rename" construct
      else if (!STRCMPI(sParser->peekToken, "rename"))
      {
         if (!sParser->GetToken())     // Get the word 'rename'
            return sParser->ParsingErr();

         CString tmpString = "";

         // Get the name
         if ((res = GetNameDef(name, tmpString, display)) < 0)
            return res;
         
         // Get the rename string
         if (sParser->peekToken[0] == '(')
         {
            if (!sParser->GetToken())        // Get the '('
               return sParser->ParsingErr();
            if (!STRCMPI(sParser->peekToken, "name") || !STRCMPI(sParser->peekToken, "stringDisplay"))
            {
               sParser->PushToken();
               if ((res = GetNameDef(renameString, tmpString, display)) < 0)
                  return res;
            }
            else
            {
               sParser->PushToken();
               if ((res = GetName(renameString, display)) < 0)
                  return res;
            }
         }
         else
         {
            if ((res = GetNameDef(renameString, tmpString, display)) < 0)
               return res;
         }


         if (!sParser->GetToken())     // Get the ')'
            return sParser->ParsingErr();
         if (STRCMPI(sParser->token, ")"))
            return sParser->ParsingErr(); 
      }
      // The next token is other construct
      else
      {
         sParser->PushToken();
         return 0;
      }
   }
   else if (sParser->peekToken[0] != ')')
   {
      //The name is just a simple identifier
      if ((res = GetString(name)) < 0)
         return res;
   }

   return 1;
}

/******************************************************************************
* GetName
*/
static int GetName(CString &name, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   if ((res = GetString(name)) < 0)
      return res;
   else if (res == 0)   // There is no string, not even an empty string
      return sParser->ParsingErr();

   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            if ((res = display.DoDisplay()) < 0)
               return res;
            alreadyDisplay = TRUE;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
   }

   return 1;
}

/******************************************************************************
* GetPoint
*/
static int GetPoint(double &x, double &y)
{
   if (!sParser->GetToken())     // get the '('
      return sParser->ParsingErr();
   if (sParser->token[0] != '(')
   {
      sParser->PushToken();
      return 0;
   }

   if (!sParser->GetToken())     // get the word 'pt'
      return sParser->ParsingErr();
   if (STRCMPI(sParser->token, "pt"))
   {
      fprintf(sFileErr, "Syntax Error : Token \"%s\" on line %ld.  Excepting \"pt\" for point.\n", sParser->token, sParser->lineCount);
      sDisplayErr++;
      return -1;
   }

   if (!sParser->GetToken())     // get X value
      return sParser->ParsingErr();
   x = ConvertEdifUnit("distance", atof(sParser->token), NULL);

   if (!sParser->GetToken())     // get Y value
      return sParser->ParsingErr();
   y = ConvertEdifUnit("distance", atof(sParser->token), NULL);
   
   if (!sParser->GetToken())     // get the ')'
      return sParser->ParsingErr();
   if (sParser->token[0] != ')')
      return sParser->ParsingErr();

   return 1;
}

/******************************************************************************
* GetPoints
   Get the list of points within the same construct such
*/
static int GetPoints(CPntList &pntList)
{
   int res = 0;
   while (TRUE)
   {
      double x = 0;
      double y = 0;
      if ((res = GetPoint(x, y)) > 0)
      {
         CPnt *pnt = new CPnt();
         pnt->x = (DbUnit)x;
         pnt->y = (DbUnit)y;
         pnt->bulge = 0;
         pntList.AddTail(pnt);
      }
      else
         return res;
   }
}

/******************************************************************************
* GetPointList
   - This is similar to GetPoints except that it is inside a "pointList" construct
*/
static int GetPointList(CPntList &pntList)
{
   if (!sParser->GetToken())  // get the "("
      return sParser->ParsingErr();
   if (!sParser->GetToken())  // get the word "pointList"
      return sParser->ParsingErr();
   if (STRCMPI(sParser->token, "pointList"))
      return sParser->ParsingErr();

   int res;
   if ((res = GetPoints(pntList)) < 0)
      return res;

   if (!sParser->GetToken())  // Get the ")"
      return sParser->ParsingErr();

   return 1;
}

/******************************************************************************
* GetCurve
*/
static int GetCurve(CPntList &pntList)
{
   if (!sParser->GetToken())  // get the "("
      return sParser->ParsingErr();
   if (!sParser->GetToken())  // get the word "curve"
      return sParser->ParsingErr();
   if (STRCMPI(sParser->token, "curve"))
      return sParser->ParsingErr();
   if (!sParser->GetToken())  // get the "("
      return sParser->ParsingErr();
          
   BOOL isArc = FALSE;
   CPnt *pntOnArc = NULL;
   CPnt *pnt = NULL;

   int arcPointCnt = 0;
   while(sParser->token[0] == '(')
   {
      if (!sParser->GetToken())  // Get the word "pt", "numberPoint", or "arc"
      {  
         if (pntOnArc != NULL)
            delete pntOnArc;
         return sParser->ParsingErr();
      }

      if (!STRCMPI(sParser->token, "arc"))
      {
         if (!sParser->GetToken())  // Get the "("
         {  
            if (pntOnArc != NULL)
               delete pntOnArc;
            return sParser->ParsingErr();
         }
         isArc = TRUE;
         arcPointCnt = 0;
         continue;
      }
      else if (!STRCMPI(sParser->token, "pt"))
      {
         double x = 0.0;
         double y = 0.0;
         int res;
         if ((res = GetScaleInteger(x)) < 0)
         {  
            if (pntOnArc != NULL)
               delete pntOnArc;
            return res;
         }
         if ((res = GetScaleInteger(y)) < 0)
         {  
            if (pntOnArc != NULL)
               delete pntOnArc;
            return res;
         }

         if (isArc)
            arcPointCnt++;

         if (arcPointCnt == 2)
         {
            pntOnArc = new CPnt();
            pntOnArc->x = (float)ConvertEdifUnit("distance", x, NULL);
            pntOnArc->y = (float)ConvertEdifUnit("distance", y, NULL);
            pntOnArc->bulge = 0;
         }
         else
         {
            pnt = new CPnt();
            pnt->x = (float)ConvertEdifUnit("distance", x, NULL);
            pnt->y = (float)ConvertEdifUnit("distance", y, NULL);
            pnt->bulge = 0;

            if (arcPointCnt == 3 && pntOnArc != NULL)   // there is an arc so find the bulge for the starting point
            {
               double startAngle;
               double deltaAngle;
               double centerX;
               double centerY;
               double radius;
               CPnt *pntStart = pntList.GetTail();

               ArcPoint3(pntStart->x, pntStart->y, pntOnArc->x, pntOnArc->y, pnt->x, pnt->y, &centerX, &centerY, &radius, &startAngle, &deltaAngle);
               pntStart->bulge = (float)tan(deltaAngle/4);

               if (pntOnArc != NULL)
                  delete pntOnArc;
               pntOnArc = NULL;

               if (!sParser->GetToken())  // Get the ")"
               {  
                  delete pnt;
                  return sParser->ParsingErr();
               }
            }
            pntList.AddTail(pnt);
         }

         if (!sParser->GetToken())  // Get the ")"
         {  
            if (pntOnArc != NULL)
               delete pntOnArc;
            return sParser->ParsingErr();
         }
         if (!sParser->GetToken())  // Get the "("
         {  
            if (pntOnArc != NULL)
               delete pntOnArc;
            return sParser->ParsingErr();
         }
      }
      else
      {  
         if (pntOnArc != NULL)
            delete pntOnArc;
         return sParser->ParsingErr();
      }
   }

   // In case there was a dangling point
   if (pntOnArc != NULL)
      delete pntOnArc;

   return 1;
}

/******************************************************************************
* ConvertEdifUnit
*/
static double ConvertEdifUnit(CString unitRef, double edifValue, void *basicUnitName)
{
	CEdif200Library* library = sEdif200.getCurrentLibrary();
	if (library != NULL)
	{
		Unit* unit = library->findUnit("distance");
      basicUnitName = (void*)&(unit->unitName);
      return unit->GetExternalValue(edifValue);
	}

   basicUnitName = "";
   return edifValue;
}

/******************************************************************************
* static void  append_linefeed
   extend carriage return to carriage return + line feed.
*/
static void append_linefeed(CString &target, int code)
{
   if(code == '\r')
      target += '\n';
}

/******************************************************************************
* static void  convert_special_char
   name or rename
*/
static void convert_special_char(char *t)
{
   int esc = 0;
   CString n = "";
   CString m = "";

   for (int i=0; i<(int)strlen(t); i++)
   {
      if (t[i] == '%')
      {
         if (esc)
         {
            //char tmp[255];
            char *tmp;
            char *lp;
            tmp = STRDUP(m);
            if (lp = strtok(tmp, " \t"))
            {
               n += (char)atoi(lp);
               append_linefeed(n, atoi(lp));

               while (lp = strtok(NULL, " \t"))
               {
                  n += (char)atoi(lp);
                  append_linefeed(n, atoi(lp));
               }
            }
            if (tmp)
               free(tmp);
            m = "";
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
* FreeAttribMap
*/
static int FreeAttribMap(CAttributes** attribMap)
{
   delete *attribMap;
   attribMap = NULL;

   return 1;
}

/******************************************************************************
* AttribUntransform
*/
static int AttribUntransform(Attrib *attrib, Transform insertTransform)
{
   if (!attrib || insertTransform.isEmpty)
      return 1;

   // if scaleX and scaleY are the same, then just use either one
   double scale = 1;
   if (fabs(insertTransform.scaleX) / fabs(insertTransform.scaleY) == 1) 
      scale = fabs(insertTransform.scaleX);

   SelectStruct transformation;
   transformation.insert_x = (DbUnit)insertTransform.x;
   transformation.insert_y = (DbUnit)insertTransform.y;
   transformation.rotation = (DbUnit)(DegToRad(insertTransform.rotation));
   transformation.scale = (DbUnit)scale;
   transformation.mirror = insertTransform.mirror;

   AttribIterator attribIterator(attrib);
#if CamCadMajorMinorVersion > 406  //  > 4.6
   Attrib tmpAttrib(attrib->getCamCadData());
#else
   Attrib tmpAttrib;
#endif
   while (attribIterator.getNext(tmpAttrib))
   {
		Point2 point;
		point.x = tmpAttrib.getX();
		point.y = tmpAttrib.getY();
		Untransform(sDoc, &point, &transformation);

		Attrib *newAttrib = tmpAttrib.allocateCopy();

		if (insertTransform.mirror)
      {
         switch(newAttrib->getHorizontalPosition())
         {
            case horizontalPositionLeft:   newAttrib->setHorizontalPosition(horizontalPositionRight);  break;
            case horizontalPositionRight:  newAttrib->setHorizontalPosition(horizontalPositionLeft);   break;
         }
      }

      double attribDeg = 0.;

      if (!sAttribRotationLimit_0_90)
      {
         // Allow all attrib rotations (0, 90, 180, 270)
         // Default, assume top
         attribDeg = normalizeDegrees( newAttrib->getRotationDegrees() - insertTransform.rotation );

         // Bottom / Mirrored
         // Allows 0, 90, 180, 270 angles for attrib display in mirrored insert
         if (insertTransform.mirror & MIRROR_FLIP )
         {
            attribDeg = normalizeDegrees( 360.0 - attribDeg );
         }
      }
      else
      {
         // Limit attrib rotation to "readable" angles, 0 and 90
         int attribRotDeg = round(normalizeDegrees(newAttrib->getRotationDegrees()));
         if (attribRotDeg > 90)
            attribRotDeg  -= 180;  // Limit to 0 and 90 rotations

         // Default, assume top
         attribDeg = normalizeDegrees( (double)attribRotDeg - insertTransform.rotation );

         // Bottom / Mirrored
         // Limit to 0 and 90 for mirrored insert
         if (insertTransform.mirror & MIRROR_FLIP)
         {
            attribDeg = normalizeDegrees( (double)attribRotDeg + insertTransform.rotation );

            int transformRotDeg = round(insertTransform.rotation);

            if (transformRotDeg == 90 || transformRotDeg == 270)
            {
               int tempDeg = round(  attribDeg );

               if (tempDeg == 0 || tempDeg == 180)
                  attribDeg += 180;
            }
            else if (transformRotDeg == 0 || transformRotDeg == 180)
            {
               int tempDeg = round( attribDeg );

               if (tempDeg == 90 || tempDeg == 270)
                  attribDeg += 180;
            }
         }
      }

      double attribRad = normalizeRadians( DegToRad(attribDeg) );
		newAttrib->setRotationRadians(attribRad);
		newAttrib->setCoordinate(point.x, point.y);

		attrib->updatePropertiesOfInstanceLike(tmpAttrib, *newAttrib);
		delete newAttrib;
	}

   return 1;
}

/******************************************************************************
* GetKeywordDisplay
*/
static int GetKeywordDisplay(CAttributes** attribMap)
{
   int res = 0;
   int parenthesisCount = 0;

   CString keywordName;
   if ((res = GetString(keywordName)) < 0)
      return res;

   if (*attribMap == NULL)
      *attribMap = new CAttributes();

   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            Display display;
            if ((res = display.DoDisplay()) < 0)
               return res;

            if (!keywordName.CompareNoCase("designator"))
               sEdifAttrib->AddDefinedAttrib(&(*attribMap), SCH_ATT_DESIGNATOR, "", ATTRIB_CHANGE_LOCATION, &display, NULL);
            else if (!keywordName.CompareNoCase("cell"))
               sEdifAttrib->AddDefinedAttrib(&(*attribMap), SCH_ATT_CELL, "", ATTRIB_CHANGE_LOCATION, &display, NULL);
            else if (!keywordName.CompareNoCase("instance"))
               sEdifAttrib->AddDefinedAttrib(&(*attribMap), SCH_ATT_INSTANCE, "", ATTRIB_CHANGE_LOCATION, &display, NULL);

            alreadyDisplay = TRUE;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
   }

   return 1;
}

/******************************************************************************
* GetProperty

   This function will do the following commands:
   - typedValue
   - unit

   Skip the following commands:
   - owner
   - comment
   - property  : Skip because we don't support complex property

   Return code:
    1 - no error
   -1 - syntax error
*/
static int GetProperty(CAttributes** attribMap)
{
   int res = 0;
   int parenthesisCount = 0;

   CString propertyName = "";
   CString renameString = "";
   Display nameDisplay;
   if ((res = GetNameDef(propertyName, renameString, nameDisplay)) < 0)
      return res;

   int valueType = VT_STRING;
   CString value = "";
   Display valueDisplay;
   if ((res = GetTypedValue(valueType, value, valueDisplay)) < 0)
      return res;

   if (*attribMap == NULL)
      *attribMap = new CAttributes();

   CString unitName = "";
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "owner") || !STRCMPI(sParser->token, "property"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "unit"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
   }

   sEdifAttrib->AddUndefinedAttrib(&(*attribMap), propertyName, valueType, value, ATTRIB_CHANGE_VALUE, &valueDisplay, NULL);
   return 1;
}

/******************************************************************************
* GetTypedValue

   This function will do the following commands:
   - boolean
   - integer
   - miNoMax
   - number
   - point
   - string

   Return code:
    1 - no error
   -1 - syntax error
*/
static int GetTypedValue(int &valueType, CString &value, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "boolean"))
      {
         valueType = VT_STRING;
         if ((res = GetBooleanPropertyValue(value, display)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "integer"))   // need review
      {
         valueType = VT_INTEGER;
         if ((res = GetIntegerPropertyValue(value, display)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "miNoMax"))   // need review
      {
         valueType = VT_STRING;
         if ((res = GetMiNoMaxPropertyValue(value, display)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "number"))    // need review
      {
         valueType = VT_DOUBLE;
         if ((res = GetNumberPropertyValue(value, display)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "point"))
      {
         valueType = VT_STRING;
         if ((res = GetPointPropertyValue(value, display)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "string"))
      {
         valueType = VT_STRING;
         if ((res = GetStringPropertyValue(value, display)) < 0)
            return res;
      }
   }

   return 1;
}

/******************************************************************************
* GetBooleanPropertyValue
   - get the boolean value of a property
   - if there is more than one value, skip it since we don't do array of values
*/
static int GetBooleanPropertyValue(CString &value, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   value.Empty();
   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "true"))
      {
         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = "TRUE";
      }
      else if (!STRCMPI(sParser->token, "false"))
      {
         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = "FALSE";
      }
      else if (!STRCMPI(sParser->token, "boolean"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "booleanDisplay"))
      {
         // Get the boolean value
         bool boolValue = false;
         if ((res = GetBoolean(boolValue)) < 0)
            return res;

         if (value.IsEmpty())
            value = (boolValue ? "TRUE" : "FALSE");
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            if ((res = display.DoDisplay()) < 0)
               return res;
            alreadyDisplay = TRUE;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
   }

   if (value.IsEmpty())
      value = "FALSE";

   return 1;
}


/******************************************************************************
* GetIntegerPropertyValue
   - get the integer value of a property
   - if there is more than one value, skip it since we don't do array of values
*/
static int GetIntegerPropertyValue(CString &value, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   value.Empty();
   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "integer"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "integerDisplay"))
      {
         // Get the integer value
         if (value.IsEmpty())
            value = sParser->token;
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            if ((res = display.DoDisplay()) < 0)
               return res;
            alreadyDisplay = TRUE;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
      else // this has to be an integer number
      {
         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = sParser->token;
      }
   }

   if (value.IsEmpty())
      value = "0";

   return 1;
}

/******************************************************************************
* GetMiNoMaxPropertyValue
   - get the minimal-nonimal-maximal value of a property
   - if there is more than one value, skip it since we don't do array of values
*/
static int GetMiNoMaxPropertyValue(CString &value, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   value.Empty();
   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "miNoMax"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "miNoMaxDisplay"))
      {
         // Get the 'mnm' construction
         if (!sParser->GetToken())     // Get the '('
            return sParser->ParsingErr();
         if (!sParser->GetToken())     // Get the 'mnm'
            return sParser->ParsingErr();
         if (STRCMPI(sParser->token, "mnm"))
         {
            fprintf(sFileErr, "Syntax Error : Expecting \"mnm\" but get token \"%s\" on line %ld\n", sParser->token, sParser->lineCount);
            sDisplayErr++;
            return -1;
         }

         CString tmpValue = "";
         if ((res = GetMinimalNonimalMaximal(tmpValue)) < 0)
            return res;

         if (!sParser->GetToken())     // Get the ')'
            return sParser->ParsingErr();

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = sParser->token;
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            if ((res = display.DoDisplay()) < 0)
               return res;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
      else if (!STRCMPI(sParser->token, "mnm"))
      {
         CString tmpValue = "";
         if ((res = GetMinimalNonimalMaximal(tmpValue)) < 0)
            return res;

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = tmpValue;
      }
      else // this has to be an integer number
      {
         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value.Format("min=undefined nom=%s max=undefined", sParser->token);
      }
   }

   return 1;
}

/******************************************************************************
* GetMinimalNonimalMaximal
   - it is a single regular number or three number or string for the minimal, nominal, and maximal
     ex: (mnm 25 30 (e 335 -1))
         (mnm (unconstrained) (undefined) 60), .. etc.
   - the resulting value wil be a string
     ex: "min=25 nom=30 max=0.0029"
         "min=unconstrained nom=undefined max=60"
*/
static int GetMinimalNonimalMaximal(CString &value)
{
   value.Empty();

   // Get the minimal value
   if (sParser->peekToken[0] == '(')
   {
      if (!sParser->GetToken())     // Get the '('
         return sParser->ParsingErr();

      if (!sParser->GetToken())     // Get the word
         return sParser->ParsingErr();
      value.Format("min=%s", sParser->token);            

      if (!sParser->GetToken())     // Get the ')'
         return sParser->ParsingErr();
   }
   else 
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();
      value.Format("min=%s", sParser->token);
   }

   // Get the nominal value
   if (sParser->peekToken[0] == '(')
   {
      if (!sParser->GetToken())     // Get the '('
         return sParser->ParsingErr();

      if (!sParser->GetToken())     // Get the word
         return sParser->ParsingErr();
      value.Format("%s nom=%s", value, sParser->token);

      if (!sParser->GetToken())     // Get the ')'
         return sParser->ParsingErr();
   }
   else 
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();
      value.Format("%s nom=%s", value, sParser->token);
   }

   // Get the maximal value 
   if (sParser->peekToken[0] == '(')
   {
      if (!sParser->GetToken())     // Get the '('
         return sParser->ParsingErr();

      if (!sParser->GetToken())     // Get the word
         return sParser->ParsingErr();
      value.Format("%s max=%s", value, sParser->token);

      if (!sParser->GetToken())     // Get the ')'
         return sParser->ParsingErr();
   }
   else 
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();
      value.Format("%s max=%s", value, sParser->token);
   }

   return 1;
}

/******************************************************************************
* GetNumberPropertyValue
   - get the numeric value of a property
   - if there is more than one value, skip it since we don't do array of values
*/
static int GetNumberPropertyValue(CString &value, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   value.Empty();
   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "(") && !STRCMPI(sParser->peekToken, "e"))
      {
         if (!sParser->GetToken())  // get the 'e'
            return sParser->ParsingErr();
         if (sParser->token[0] != 'e' && sParser->token[0] != 'E')
            return sParser->ParsingErr();

         if (!sParser->GetToken())  // get the mantissa
            return sParser->ParsingErr();
         double mantissa = atof(sParser->token);

         if (!sParser->GetToken()) // get the exponent
            return sParser->ParsingErr();
         double exponent = atof(sParser->token);

         if (!sParser->GetToken()) // get the ")"
            return sParser->ParsingErr();

         double dblValue = mantissa * pow(10., exponent);

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value.Format("%f", dblValue);
      }
      else if (!STRCMPI(sParser->token, "("))
      {
         ++parenthesisCount;
      }
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "number"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "numberDisplay"))
      {
         // Get the scaled integer
         double dblValue = 0;
         if ((res = GetScaleInteger(dblValue)) < 0)
            return res;

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value.Format("%f", dblValue);
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            if ((res = display.DoDisplay()) < 0)
               return res;
            alreadyDisplay = TRUE;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
      else // this has to be an integer number
      {
         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = sParser->token;
      }
   }

   if (value.IsEmpty())
      value = "0";

   return 1;
}

/******************************************************************************
* GetPointPropertyValue
   - get the point value of a property
   - if there is more than one value, skip it since we don't do array of values
*/
static int GetPointPropertyValue(CString &value, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   value.Empty();
   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
      {
         ++parenthesisCount;
      }
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "point"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "pointDisplay"))
      {
         // Get the scaled integer
         double x = 0;
         double y = 0;
         if ((res = GetPoint(x, y)) < 0)
            return res;

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value.Format("x=%f y=%f", x, y);
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            if ((res = display.DoDisplay()) < 0)
               return res;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
      else if (!STRCMPI(sParser->token, "pt"))// this has to be an integer number
      {
         if (!sParser->GetToken())
            return sParser->ParsingErr();
         CString x = sParser->token;

         if (!sParser->GetToken())
            return sParser->ParsingErr();
         CString y = sParser->token;

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value.Format("x=%s y=%s", x, y);
      }
   }

   return 1;
}

/******************************************************************************
* GetStringPropertyValue
   - get the string value of a property
   - if there is more than one value, skip it since we don't do array of values
*/
static int GetStringPropertyValue(CString &value, Display &display)
{
   int res = 0;
   int parenthesisCount = 0;

   value.Empty();
   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (sParser->isPeekTokenName)
      {
         CString tmpValue;
         if ((res = GetString(tmpValue, true)) < 0)
            return res;

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = tmpValue;
      }

      if (!sParser->GetToken())
         return sParser->ParsingErr();
         
      if (!STRCMPI(sParser->token, "("))
      {
         ++parenthesisCount;
      }
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "string"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "stringDisplay"))
      {
         // Get the string
         CString tmpValue;
         if ((res = GetString(tmpValue, true)) < 0)
            return res;

         // only assign the value when value is empty because we only care about the first value
         if (value.IsEmpty())
            value = tmpValue;
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            if ((res = display.DoDisplay()) < 0)
               return res;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
   }

   return 1;
}

/******************************************************************************
* GetPropertyDisplay
*/
static int GetPropertyDisplay(CAttributes** attribMap)
{
   int res = 0;
   int parenthesisCount = 0;

   CString keywordName;
   if ((res = GetString(keywordName)) < 0)
      return res;

   if (*attribMap == NULL)
      *attribMap = new CAttributes();

   BOOL alreadyDisplay = FALSE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "display"))
      {
         if (alreadyDisplay == FALSE || sOverWriteAttributeDisplay)
         {
            Display display;
            if ((res = display.DoDisplay()) < 0)
               return res;

            sEdifAttrib->AddUndefinedAttrib(&(*attribMap), keywordName, -1, "", ATTRIB_CHANGE_LOCATION, &display, NULL);
            alreadyDisplay = TRUE;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
   }

   return 1;
}

/******************************************************************************
* CopyAttribMap
   - copy fromMap to copyToMap 
   - delete fromMap
*/
static int CopyAttribMap(CAttributes** copyToMap, CAttributes* fromMap, bool overwrite)
{
   if (fromMap == NULL || fromMap->GetCount() == 0)
      return -1;

   if (*copyToMap == NULL) // copyToMap is NULL so just assign fromMap to it  
   {
      sDoc->CopyAttribs(&(*copyToMap), fromMap);
	}
	else
	{
		// copyToMap is not NULL, so copy fromMap to it		
		for (POSITION pos = fromMap->GetStartPosition();pos != NULL;)
		{                                                          
			WORD keyword;
			Attrib* fromAttrib;

			fromMap->GetNextAssoc(pos, keyword, fromAttrib);

			Attrib* toAttrib = NULL;

			if ((*copyToMap)->Lookup(keyword, toAttrib))   // the same attribute exist in copyToMap
			{
				// Add the display location(s) in fromAttrib to toAttrib
				if (fromAttrib->getFlags() & ATTRIB_CHANGE_LOCATION)
				{
					if (toAttrib->isInherited() || overwrite == true)
					{
						// Attrib is inherited, so overwrite it
						toAttrib->setProperties(*fromAttrib);
					}
					else
					{
						Attrib *newToAttrib = fromAttrib->allocateCopy();
						toAttrib->addInstance(newToAttrib);
					}
				}

				// Update the value from fromAttrib to toAttrib if it has changed
				if (fromAttrib->getFlags() & ATTRIB_CHANGE_VALUE)
				{
					AttribIterator attribIterator(toAttrib);
#if CamCadMajorMinorVersion > 406  //  > 4.6
               Attrib tmpAttrib(toAttrib->getCamCadData());
#else
					Attrib tmpAttrib;
#endif
					while (attribIterator.getNext(tmpAttrib))
						toAttrib->updateValueOfInstanceLike(tmpAttrib, *fromAttrib);
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
	}

	// We need to find out if the port name has been renamed, if it has
	// then need to add the SCH_ATT_PORTNAME attrib of the copy to the map too
	// so it will get rename properly at the end of the reader
	Attrib *attrib = NULL;
   int keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);
   if ((*copyToMap)->Lookup(keyword, attrib))
	{   
		Attrib *dataAttrib = NULL;
		if (fromMap->Lookup(keyword, dataAttrib))
		{
			CString *renamePtr = NULL;
			if (sPortRenameMap.Lookup(dataAttrib, (void*&)renamePtr))
			{
				if (renamePtr != NULL && attrib != NULL)
				{
					CString *newName = new CString(*renamePtr);
					sPortRenameMap.SetAt(attrib, newName);
				}
			}
		}
	}


   return 1;
}

/******************************************************************************
* RenamePortName
*/
static void RenamePortName()
{
	POSITION pos = sPortRenameMap.GetStartPosition();
	while (pos)
	{
		Attrib *attrib = NULL;
		CString *rename = NULL;
		sPortRenameMap.GetNextAssoc(pos, (void*&)attrib, (void*&)rename);
		if (attrib == NULL || rename == NULL)
			continue;

		attrib->setStringValueIndex(sDoc->RegisterValue(*rename));
	}
}

/******************************************************************************
* CheckUniqueInstanceName
*/
static void CheckUniqueInstanceName()
{

   POSITION filePos = sDoc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = sDoc->getFileList().GetNext(filePos);

		CMapStringToString refNameMap;
		for (int i=0; i<sDoc->getMaxBlockIndex(); i++)
		{
			BlockStruct *sheet = sDoc->getBlockAt(i);

			if (sheet == NULL)
				continue;
			if (sheet->getFileNumber() != file->getFileNumber())
				continue;

			POSITION pos = sheet->getDataList().GetHeadPosition();
			while (pos)
			{
				DataStruct *data = sheet->getDataList().GetNext(pos);

				// Make sure we have a component insert
				if (data->getDataType() != T_INSERT || data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL)
					continue;

				CString instanceName = data->getInsert()->getRefname();
				if (refNameMap.Lookup(instanceName, instanceName))
				{
					fprintf(sFileErr, "Warning : Not unique instance name \"%s\"\n", instanceName);
					sDisplayErr++;
				}
				else
				{
					// Add the refname to the map
					refNameMap.SetAt(instanceName, instanceName);
				}
			}
		}
		refNameMap.RemoveAll();
	}
}

/******************************************************************************
* AddAttribToCompPin
*/
static void AddAttribToCompPin()
{
   int compKey = sDoc->RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, 0, VT_STRING);
   int pinKey = sDoc->RegisterKeyWord(SCH_ATT_PINDESIGNATOR, 0, VT_STRING);
   int designatorKey = sDoc->IsKeyWord(SCH_ATT_DESIGNATOR, 0);

   POSITION filePos = sDoc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = sDoc->getFileList().GetNext(filePos);
      if (file->getNetList().GetCount() <= 0)
         continue;

      //(1) Find the shematic sheet
      for (int i=0; i<sDoc->getMaxBlockIndex(); i++)
      {
         BlockStruct *sheetBlock = sDoc->getBlockAt(i);
         if (sheetBlock == NULL)
            continue;

         if (sheetBlock->getBlockType() != blockTypeSheet || sheetBlock->getFileNumber() != file->getFileNumber())
            continue;

         //(2) Find the instance insert
         POSITION dataPos = sheetBlock->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = sheetBlock->getDataList().GetNext(dataPos);
            if (data->getDataType() != T_INSERT)
               continue;
            if (data->getInsert()->getInsertType() != INSERTTYPE_SYMBOL && data->getInsert()->getInsertType() != INSERTTYPE_HIERARCHYSYMBOL)
               continue;
            if (data->getAttributesRef() == NULL)
               continue;

            //(3) Get the designator of the instance
            Attrib *attrib = NULL;
            if (!data->getAttributesRef()->Lookup(designatorKey, attrib))
               continue;
            CString compDes = "";
            if (attrib)
            {
#if CamCadMajorMinorVersion > 406  //  > 4.6
               compDes = attrib->getStringValue();
#else
               compDes = sDoc->ValueArray[attrib->getStringValueIndex()];
#endif
            }

            BlockStruct *insertedBlock = sDoc->getBlockAt(data->getInsert()->getBlockNumber());

            //(5) Find the port insert
            POSITION portDataPos = insertedBlock->getDataList().GetHeadPosition();
            while (portDataPos)
            {
               DataStruct *portData = insertedBlock->getDataList().GetNext(portDataPos);
               if (portData->getDataType() != T_INSERT)
                  continue;
               if (portData->getInsert()->getInsertType() != INSERTTYPE_PORTINSTANCE)
                  continue;
               if (!portData->getAttributesRef())
                  continue;

               //(6) Get the designator of the port
               attrib = NULL;
               if (!portData->getAttributesRef()->Lookup(designatorKey, attrib))
                  continue;
               CString pinDes = "";
               if (attrib)
               {
#if CamCadMajorMinorVersion > 406  //  > 4.6
                  pinDes = attrib->getStringValue();
#else
                  pinDes = sDoc->ValueArray[attrib->getStringValueIndex()];
#endif
               }

               //(7) Find the net
               POSITION netPos = file->getNetList().GetHeadPosition();
               while (netPos)
               {
                  POSITION curNetPos = netPos;
                  NetStruct *net = file->getNetList().GetNext(netPos);
                  
                  //(8) Find the comppin
                  POSITION comppinPos = net->getHeadCompPinPosition();
                  while (comppinPos)
                  {
                     POSITION curComppinPos = comppinPos;
                     CompPinStruct *comppin = net->getNextCompPin(comppinPos);

                     //(8.1) Find the comp name that matches the insert->refname
                     if (comppin->getRefDes().CompareNoCase(data->getInsert()->getRefname()))
                        continue;

                     //(8.1.1) Find the pin name that matches the port name of a pin
                     if (comppin->getPinName().CompareNoCase(portData->getInsert()->getRefname()))
                        continue;

                     //(9) Add the attributes
                     sDoc->SetAttrib(&comppin->getAttributesRef(), compKey, VT_STRING, 
                           compDes.GetBuffer(0), SA_OVERWRITE, NULL);
                     sDoc->SetAttrib(&comppin->getAttributesRef(), pinKey, VT_STRING, 
                           pinDes.GetBuffer(0), SA_OVERWRITE, NULL);
                  }

                  // (10) Remove the net from netlist if there is no more comppin in net 
                  if (net->getCompPinCount() == 0)
                  {
#if CamCadMajorMinorVersion > 406  //  > 4.6
                     file->getNetList().deleteAt(curNetPos);
#else
                     file->getNetList().RemoveAt(curNetPos);
                     FreeNet(net);
#endif
                  }
               }
            }
         }
      }
   }

}



/******************************************************************************/
/*                         CLASS FUNCTIONS DEFINITION                         */
/******************************************************************************/

/******************************************************************************
* Parser::Parser
*/
BOOL Parser::OpenFile(CString fileName)
{
   this->file = fopen(fileName, "rt");
   this->lineCount = 0;
   if (this->file == NULL)
   {
      CString msg = "";
      msg.Format("Error open [%s] file", fileName);
      ErrorMessage(msg, "Error");
      return FALSE;
   }
   else
      return TRUE;
}

/******************************************************************************
* Parser::CloseFile
*/
void Parser::CloseFile()
{
   fclose(this->file);  
}

/******************************************************************************
* Parser::GetToken
*/
BOOL Parser::GetToken()
{
   BOOL firstToken = !(this->lineCount);

   if (!this->isPushToken)
   {
      strcpy(this->token, this->peekToken);
      this->isTokenName = this->isPeekTokenName;
      while (!this->GetNext())
      {
         if (!GetLine())
         {
            if (this->peekToken[0] != '\0')
            {
               this->peekToken[0] = '\0';
               return TRUE;
            }
            else
               return FALSE;
         }
         else
            this->isNewLine = TRUE;
      }
      this->isNewLine = FALSE;

      if (firstToken)
         return this->GetToken();
   }
   else
      this->isPushToken = FALSE;

   return TRUE;
}

/******************************************************************************
* Parser::PustToken
*/
void Parser::PushToken()
{
   this->isPushToken = TRUE;
}

/******************************************************************************
* Parser::SkipCommand
   Skip everything assocated with this command
*/
int Parser::SkipCommand()
{
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!GetToken())
         return ParsingErr();

      if (!STRCMPI(this->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(this->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            this->PushToken();
            return 1;
         }
      }
   }
}

/******************************************************************************
* Parser::ParsingErr
*/
int Parser::ParsingErr()
{
   fprintf(sFileErr, "Syntax Error : Token \"%s\" on line %ld\n", this->token, this->lineCount);
   sDisplayErr++;
   return -1;
}

/******************************************************************************
* Parser::GetNext
*/
BOOL Parser::GetNext()
{
   this->isPeekTokenName = FALSE;

   static char *line;
   if (this->isNewLine)
      line = this->inputLine;

   while(isspace(*line) && *line != '\0')
      ++line;

   int i = 0;
   switch(*line)
   {
   case '\0':
      return FALSE;
   case '(':
   case ')':
      this->peekToken[i++] = *(line++);
      break;
   case '\"':
      {
         this->peekToken[i] = '\0';
         this->isPeekTokenName = TRUE;

         for (++line; *line != '\"'; ++line, ++i)
         {
            if (i >= MAX_TOKEN - 1)
            {
               fprintf(sFileErr, "Error in Get Token - Token too long at %ld\n", this->lineCount);
               sDisplayErr++;
            }

            if (*line == '\n')
            {
               /* text spans over 1 line */
               if (!GetLine())
               {
                  fprintf(sFileErr, "Error in Line Read.\n");
                  sDisplayErr++;
               }

               line = this->inputLine;
               while(isspace(*line) && *line != '\0')
                  ++line;

               if (*line == '\"')   // new line starts with a end_quote.
                  break;
            }

            if (isspace(this->peekToken[i]) && isspace(*line))
               continue;

            this->peekToken[i] = *line;
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
            this->peekToken[i] = *line;
            ++line;
            ++i;
         }
      }
      break;
   }

   if (!i && !this->isPeekTokenName)
      return FALSE;

   this->peekToken[i] = '\0';
   convert_special_char(this->peekToken);

   return TRUE;
}

/******************************************************************************
* Parser::GetLine
*/
BOOL Parser::GetLine()
{
   if (fgets(this->inputLine, this->MAX_LENGTH, this->file) == NULL)
   {
      if (!feof(this->file))
      {
         ErrorMessage("Fatal EDIF Parse Error", "Read Error !", MB_OK | MB_ICONHAND);
         return FALSE;
      }
      else
      {
         inputLine[0] = '\0' ;
         return FALSE;
      }
   }
   else
   {
      if (inputLine[strlen(inputLine)-1] != '\n')
      {
         fprintf(sFileErr, "No end of line found at %ld\n", this->lineCount);
			fprintf(sFileErr, "Probable internal error, buffer overrun. Please go to http://www.mentor.com/supportnet for technical support.\n");
         
         sDisplayErr++;
         return FALSE;
      }
   }
   this->lineCount++;

   return TRUE;
}

/******************************************************************************
* Unit::GetValue
*/
double Unit::GetExternalValue(double edifValue)
{
   double value = (edifValue / this->edifUnit) * this->externalUnit;

   if (this->unitType == "distance")
      value = value * Units_Factor(UNIT_MM, sPageUnit) * 1000;

   return value;
}

/******************************************************************************
* Library::DoFigureGroup

   This function will do the following commands:
   - pathWidth
   - color
   - textHeight
   - visble

   Skip the following commands:
   - cornerType
   - endType
   - borderWidth
   - fillPattern
   - borderPattern
   - property
   - includeFigureGroup
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int FigureGroup::DoFigureGroup()
{
   int parenthesisCount = 0;
   int res = 0;

   CString renameString = "";
   Display display;
   if ((res = GetNameDef(this->m_name, renameString, display)) < 0)
      return res;

   int err;
   this->m_widthIndex = Graph_Aperture("Small Width", T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
   this->m_visible = TRUE;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "cornerType") || !STRCMPI(sParser->token, "endType") ||
               !STRCMPI(sParser->token, "borderWidth") || !STRCMPI(sParser->token, "fillPattern") ||
               !STRCMPI(sParser->token, "borderPattern") || !STRCMPI(sParser->token, "property") ||
               !STRCMPI(sParser->token, "includeFigureGroup") || !STRCMPI(sParser->token, "comment") ||
               !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "pathWidth"))
      {
         double pathWidth = 0;
         if ((res = GetScaleInteger(pathWidth)) < 0)
            return res;
         pathWidth = ConvertEdifUnit("distance", pathWidth, NULL);

         this->m_widthIndex = Graph_Aperture(this->m_name, T_ROUND, pathWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
      }
      else if (!STRCMPI(sParser->token, "color"))
      {
         double red = 0.0;
         double green = 0.0;
         double blue = 0.0;

         if ((res = GetScaleInteger(red)) < 0)
            return res;
         if ((res = GetScaleInteger(green)) < 0)
            return res;
         if ((res = GetScaleInteger(blue)) < 0)
            return res;

         this->m_color = RGB(red, green, blue);
      }
      else if (!STRCMPI(sParser->token, "textHeight"))
      {
         // The number FONT_SCALE is just a trial and error number.  Trying to get the size as nice as possible

         if ((res = GetScaleInteger(this->m_textHeight)) < 0)
            return res;
         this->m_textHeight = ConvertEdifUnit("distance", this->m_textHeight, NULL);

         // The text width is not given in Edif 200 so we choose a fraction of the text height as its width.
         // The number 4/7 of text height seems to look very good.
         this->m_textWidth = this->m_textHeight * 4./7.;

         if (sFontScale > 0)
         {
            this->m_textHeight *= sFontScale;
            this->m_textWidth *= sFontScale;
         }
      }
      else if (!STRCMPI(sParser->token, "visible"))
      {
         if ((res = GetBoolean(this->m_visible)) < 0)
            return res;
      }
   }

   return 1;
}

/******************************************************************************
*/
FigureGroup::FigureGroup()
: m_textHeight(0.0)
, m_textWidth(0.0)
, m_widthIndex(0)
, m_visible(true)
, m_color(RGB(0, 0, 0))
{
}

/******************************************************************************
* FigureGroup& FigureGroup::operator =

   - This overload function creates a copy of the incoming FigureGroup
*/
FigureGroup& FigureGroup::operator=(const FigureGroup &other)
{
   this->m_name       = other.m_name;
   this->m_color      = other.m_color;
   this->m_textHeight = other.m_textHeight;
   this->m_textWidth  = other.m_textWidth;
   this->m_visible    = other.m_visible;
   this->m_widthIndex = other.m_widthIndex;

   return *this;
}

/******************************************************************************
*/
DisplayParam::DisplayParam()
: m_horizontalJust(SCH_JUSTIFY_LEFT)
, m_verticalJust(SCH_JUSTIFY_BOTTOM)
, m_rotation(0)
, m_mirror(0)
, m_x(0)
, m_y(0)
, m_isEmpty(true)
{
}

/******************************************************************************
*/

/******************************************************************************
* Display::DoDisplay

   This function will do the following commands:
   - figureGroupOverride
   - justify
   - orientation
   - origin

   Return code:
    1 - no error
   -1 - syntax error
*/
int Display::DoDisplay()
{
   int parenthesisCount = 0;
   int res = 0;

	DisplayParam *displayParam = new DisplayParam;
   CString figureGroupName = "";
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(figureGroupName, renameString, display)) < 0)
	{
      delete displayParam;
      return res;
	}
   else if (res == 1)
   {
		CEdif200Library* library = sEdif200.getCurrentLibrary();
		if (library != NULL)
		{
         FigureGroup* figureGroup = library->findFigureGroup(figureGroupName);
			if (figureGroup != NULL)
			{
				displayParam->m_figureGroup = *figureGroup;
			}
		}
   }

   while (TRUE)
   {
      if (!sParser->GetToken())
      {
         delete displayParam;
         return sParser->ParsingErr();
      }

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "figureGroupOverride"))
      {
         CString figureGroupName = "";
         CString renameString = "";
         Display display;
         if ((res = GetNameDef(figureGroupName, renameString, display)) < 0)
         {
            delete displayParam;
            return res;
         }
         else if (res == 0)
         {
            fprintf(sFileErr, "Syntax Error : Missing figureGroup name for figureGroupOverride on line %ld\n", sParser->lineCount);
            sDisplayErr++;
            delete displayParam;
            return -1;
         }

			CEdif200Library* library = sEdif200.getCurrentLibrary();
			if (library != NULL)
			{
				FigureGroup* figureGroup = library->findFigureGroup(figureGroupName);
				if (figureGroup != NULL)
				{
					displayParam->m_figureGroup = *figureGroup;
					displayParam->m_figureGroup.DoFigureGroup();
				}
			}
      }
      else if (!STRCMPI(sParser->token, "justify"))
      {
         if (!sParser->GetToken())
         {
            delete displayParam;
            return res;
         }

         CString justify = sParser->token;

         if (!justify.CompareNoCase("LOWERLEFT"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_BOTTOM;
            displayParam->m_horizontalJust = SCH_JUSTIFY_LEFT;
         }
         else if (!justify.CompareNoCase("LOWERCENTER"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_BOTTOM;
            displayParam->m_horizontalJust = SCH_JUSTIFY_CENTER;
         }
         else if (!justify.CompareNoCase("LOWERRIGHT"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_BOTTOM;
            displayParam->m_horizontalJust = SCH_JUSTIFY_RIGHT;
         }
         else if (!justify.CompareNoCase("CENTERLEFT"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_CENTER;
            displayParam->m_horizontalJust = SCH_JUSTIFY_LEFT;
         }
         else if (!justify.CompareNoCase("CENTERCENTER"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_CENTER;
            displayParam->m_horizontalJust = SCH_JUSTIFY_CENTER;
         }
         else if (!justify.CompareNoCase("CENTERRIGHT"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_CENTER;
            displayParam->m_horizontalJust = SCH_JUSTIFY_RIGHT;
         }
         else if (!justify.CompareNoCase("UPPERLEFT"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_TOP;
            displayParam->m_horizontalJust = SCH_JUSTIFY_LEFT;
         }
         else if (!justify.CompareNoCase("UPPERCENTER"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_TOP;
            displayParam->m_horizontalJust = SCH_JUSTIFY_CENTER;
         }
         else if (!justify.CompareNoCase("UPPERRIGHT"))
         {
            displayParam->m_verticalJust = SCH_JUSTIFY_TOP;
            displayParam->m_horizontalJust = SCH_JUSTIFY_RIGHT;
         }
      }
      else if (!STRCMPI(sParser->token, "orientation"))
      {
         if (!sParser->GetToken())
         {
            delete displayParam;
            return res;
         }

         CString orientation = sParser->token;

         if (!orientation.CompareNoCase("R0"))
         {
            displayParam->m_mirror = 0;
            displayParam->m_rotation = 0;
         }  
         else if (!orientation.CompareNoCase("R90"))
         {
            displayParam->m_mirror = 0;
            displayParam->m_rotation = 90;
         }  
         else if (!orientation.CompareNoCase("R180"))
         {
            displayParam->m_mirror = 0;
            displayParam->m_rotation = 180;
         }  
         else if (!orientation.CompareNoCase("R270"))
         {
            displayParam->m_mirror = 0;
            displayParam->m_rotation = 270;
         }  
         else if (!orientation.CompareNoCase("MX"))
         {
            displayParam->m_mirror = 1;
            displayParam->m_rotation = 180;
         }  
         else if (!orientation.CompareNoCase("MY"))
         {
            displayParam->m_mirror = 1;
            displayParam->m_rotation = 0;
         }  
         else if (!orientation.CompareNoCase("MYR90"))
         {
            displayParam->m_mirror = 1;
            displayParam->m_rotation = 90;
         }  
         else if (!orientation.CompareNoCase("MXR90"))
         {
            displayParam->m_mirror = 1;
            displayParam->m_rotation = 270;
         }  
      }
      else if (!STRCMPI(sParser->token, "origin"))
      {
         if ((res = GetPoint(displayParam->m_x, displayParam->m_y)) < 0)
         {
            delete displayParam;
            return res;
         }
      }
   }


   displayParam->m_isEmpty = FALSE;

   if (displayParam->m_mirror)
   {
      if (displayParam->m_horizontalJust != 1)
         displayParam->m_horizontalJust = abs(displayParam->m_horizontalJust -2);
   }

	this->displayList.AddTail(displayParam);

   return 1;
}

/******************************************************************************
* Transform::DoTransform

   This function will do the following commands:
   - scaleX
   - scaleY
   - orientation
   - origin

   Skip the following commands:
   - delta

   Return code:
    1 - no error
   -1 - syntax error
*/
int Transform::DoTransform()
{
   int parenthesisCount = 0;
   int res = 0;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "delta"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "scaleX")  || !STRCMPI(sParser->token, "scaleY"))
      {
         BOOL isScaleX = FALSE;
         if (!STRCMPI(sParser->token, "scaleX"))
            isScaleX = TRUE;

         if (!sParser->GetToken())
            return sParser->ParsingErr();
         int num1 = atoi(sParser->token);

         if (!sParser->GetToken())
            return sParser->ParsingErr();
         int num2 = atoi(sParser->token);

         if (sExchangeXYScale)
         {
            // For some system there X & Y scale are swap, so if sExchangeXYScale TRUE then swap them 
            if (isScaleX)
               this->scaleY = ((double)num1 / (double)num2);
            else
               this->scaleX = ((double)num1 / (double)num2);
         }
         else
         {
            if (isScaleX)
               this->scaleX = ((double)num1 / (double)num2);
            else
               this->scaleY = ((double)num1 / (double)num2);
         }
      }
      else if (!STRCMPI(sParser->token, "orientation"))
      {
         if (!sParser->GetToken())
            return res;
         CString orientation = sParser->token;

         if (!orientation.CompareNoCase("R0"))
         {
            this->mirror = 0;
            this->rotation = 0;
         }  
         else if (!orientation.CompareNoCase("R90"))
         {
            this->mirror = 0;
            this->rotation = 90;
         }  
         else if (!orientation.CompareNoCase("R180"))
         {
            this->mirror = 0;
            this->rotation = 180;
         }  
         else if (!orientation.CompareNoCase("R270"))
         {
            this->mirror = 0;
            this->rotation = 270;
         }  
         else if (!orientation.CompareNoCase("MX"))
         {
            this->mirror = 1;
            this->rotation = 180;
         }  
         else if (!orientation.CompareNoCase("MY"))
         {
            this->mirror = 1;
            this->rotation = 0;
         }  
         else if (!orientation.CompareNoCase("MYR90"))
         {
            this->mirror = 1;
            this->rotation = 90;
         }  
         else if (!orientation.CompareNoCase("MXR90"))
         {
            this->mirror = 1;
            this->rotation = 270;
         }  
      }
      else if (!STRCMPI(sParser->token, "origin"))
      {
         if ((res = GetPoint(this->x, this->y)) < 0)
            return res;

         //this->x = ConvertEdifUnit("distance", this->x, NULL);
         //this->y = ConvertEdifUnit("distance", this->y, NULL);
      }
   }
   this->isEmpty = FALSE;

   return 1;
}

/******************************************************************************
* EdifAttrib::EdifAttrib
*/
EdifAttrib::EdifAttrib()
{
   int index;

   index = sDoc->RegisterKeyWord(SCH_ATT_CELL, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_CELL, SCH_ATT_CELL);
   this->edifKeyToType.SetAt(SCH_ATT_CELL, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_CELL, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_DESIGNATOR, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_DESIGNATOR, SCH_ATT_DESIGNATOR);
   this->edifKeyToType.SetAt(SCH_ATT_DESIGNATOR, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_DESIGNATOR, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_DIRECTION, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_DIRECTION, SCH_ATT_DIRECTION);
   this->edifKeyToType.SetAt(SCH_ATT_DIRECTION, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_DIRECTION, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_INSTANCE, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_INSTANCE, SCH_ATT_INSTANCE);
   this->edifKeyToType.SetAt(SCH_ATT_INSTANCE, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_INSTANCE, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_PORTNAME, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_PORTNAME, SCH_ATT_PORTNAME);
   this->edifKeyToType.SetAt(SCH_ATT_PORTNAME, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_PORTNAME, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_DISPLAYNAME, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_DISPLAYNAME, SCH_ATT_DISPLAYNAME);
   this->edifKeyToType.SetAt(SCH_ATT_DISPLAYNAME, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_DISPLAYNAME, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_UNUNSED, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_UNUNSED, SCH_ATT_UNUNSED);
   this->edifKeyToType.SetAt(SCH_ATT_UNUNSED, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_UNUNSED, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_NETNAME, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_NETNAME, SCH_ATT_NETNAME);
   this->edifKeyToType.SetAt(SCH_ATT_NETNAME, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_NETNAME, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_LIBRARYNAME, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_LIBRARYNAME, SCH_ATT_LIBRARYNAME);
   this->edifKeyToType.SetAt(SCH_ATT_LIBRARYNAME, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_LIBRARYNAME, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_CELLNAME, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_CELLNAME, SCH_ATT_CELLNAME);
   this->edifKeyToType.SetAt(SCH_ATT_CELLNAME, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_CELLNAME, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_CLUSTERNAME, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_CLUSTERNAME, SCH_ATT_CLUSTERNAME);
   this->edifKeyToType.SetAt(SCH_ATT_CLUSTERNAME, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_CLUSTERNAME, index);

   index = sDoc->RegisterKeyWord(SCH_ATT_INSTANCENAME, 0, VT_STRING);
   this->edifToCCKeyword.SetAt(SCH_ATT_INSTANCENAME, SCH_ATT_INSTANCENAME);
   this->edifKeyToType.SetAt(SCH_ATT_INSTANCENAME, VT_STRING);
   this->edifKeyToIndex.SetAt(SCH_ATT_INSTANCENAME, index);
}

/******************************************************************************
* EdifAttrib::~EdifAttrib
*/
EdifAttrib::~EdifAttrib()
{
   this->edifToCCKeyword.RemoveAll();
   this->edifKeyToType.RemoveAll();
   this->edifKeyToIndex.RemoveAll();
}

/******************************************************************************
* EdifAttrib::AddDefinedAttrib
*/
int EdifAttrib::AddDefinedAttrib(CAttributes** attribMap, CString keyword, CString value, DbFlag attribFlag, Display *display, Attrib **attribPtr)
{
   if ((*attribMap) == NULL)
      *attribMap = new CAttributes();

   CString ccKeyword = keyword;
   if (!this->edifToCCKeyword.Lookup(keyword, ccKeyword))
   {
      fprintf(sFileErr, "Reader Error : Referencing a keyword \"%s\" that is not mapped or defined\n", keyword);
      sDisplayErr++;
      return -1;
   }

   int valueType = 0;
   if (!this->edifKeyToType.Lookup(keyword, valueType))
   {
      fprintf(sFileErr, "Reader Error : Referencing a keyword \"%s\" that is not mapped to a value type\n", keyword);
      sDisplayErr++;
      return -1;
   }

   
   int keyIndex = 0;
   if (!this->edifKeyToIndex.Lookup(keyword, keyIndex))
   {
      fprintf(sFileErr, "Reader Error : Referencing a keyword \"%s\" that is not mapped to a keyword index\n", keyword);
      sDisplayErr++;
      return -1;
   }

   Attrib* attrib = NULL;
 
   (*attribMap)->Lookup(keyIndex, attrib);

   switch (valueType)
   {
   case VT_STRING:
      {
         if (attrib && (attribFlag & ATTRIB_CHANGE_VALUE))
            attrib->setStringValueIndex(sDoc->RegisterValue(value));
         else if (attrib == NULL)
            sDoc->SetAttrib(&(*attribMap), keyIndex, valueType, value.GetBuffer(0), SA_OVERWRITE, &attrib);
      }
      break;
   case VT_INTEGER:
      {
         int intValue = atoi(value);

         if (attrib && (attribFlag & ATTRIB_CHANGE_VALUE))
            attrib->setIntValue(intValue);
         else if (attrib == NULL)
            sDoc->SetAttrib(&(*attribMap), keyIndex, valueType, &intValue, SA_OVERWRITE, &attrib);
      }
      break;
   case VT_DOUBLE:
      {
         int dblValue = atoi(value);

         if (attrib && (attribFlag & ATTRIB_CHANGE_VALUE))
            attrib->setDoubleValue(dblValue);
         else if (attrib == NULL)
            sDoc->SetAttrib(&(*attribMap), keyIndex, valueType, &dblValue, SA_OVERWRITE, &attrib);
      }
      break;
   default:
      break;
   }

   if (attrib)
   {
		POSITION pos = display->displayList.GetHeadPosition();
		while (pos)
		{
			DisplayParam *displayParam = display->displayList.GetNext(pos);

			if (!displayParam->m_isEmpty)
			{
				Attrib *newAttrib = NULL;
				if (attrib->getFlags() & ATTRIB_CHANGE_LOCATION)
				{
#if CamCadMajorMinorVersion > 406  //  > 4.6
               newAttrib = attrib->getCamCadData().constructAttribute();
#else
					newAttrib = new Attrib;
#endif
					attrib->addInstance(newAttrib);
				}
				else
				{
					newAttrib = attrib;
				}

				newAttrib->setFlagBits(attribFlag);
				newAttrib->setMirrorDisabled(true);
				newAttrib->setProportionalSpacing(true);
				newAttrib->setFlagBits(ATTRIB_CHANGE_LOCATION);
				newAttrib->setCoordinate(displayParam->m_x, displayParam->m_y);
				newAttrib->setRotationRadians( DegToRad(displayParam->m_rotation));
				newAttrib->setHeight( displayParam->m_figureGroup.m_textHeight);
				newAttrib->setPenWidthIndex( displayParam->m_figureGroup.m_widthIndex); 
				newAttrib->setVisible( displayParam->m_figureGroup.m_visible);
				newAttrib->setWidth( displayParam->m_figureGroup.m_textWidth);
				newAttrib->setHorizontalPosition(intToHorizontalPositionTag( displayParam->m_horizontalJust));
				newAttrib->setVerticalPosition(intToVerticalPositionTag( displayParam->m_verticalJust));

				if (newAttrib->isVisible())
				{
					CString layerName = "";
					layerName.Format("AttrLyr_%s", ccKeyword);
					//layerName.Format("AttrLyr_%s_%d", ccKeyword, attrib->getCount());

					newAttrib->setLayerIndex( Graph_Level(layerName, "", FALSE));
				}
			}
		}

		// Assign the attrib flag again just in case it didn't get into the while loop 
		attrib->setFlagBits(attribFlag);
	}

   if (attribPtr)
      *attribPtr = attrib;

   return 1;
}

/******************************************************************************
* EdifAttrib::AddUndefinedAttrib
*/
int EdifAttrib::AddUndefinedAttrib(CAttributes** attribMap, CString keyword, int valueType, CString value,
                                   DbFlag attribFlag, Display *display, Attrib **attribPtr)
{
   if ((*attribMap) == NULL)
      *attribMap = new CAttributes();

   if (keyword.IsEmpty())
      return -1;

	// This is a property named "Designator" not the reserved keyword "DESIGNATOR" in Edif format,
	// therefore, we rename it to "Property_Designator" so it won't conflict with "DESIGNATOR"
	if (!keyword.CompareNoCase(SCH_ATT_DESIGNATOR))
		keyword = "Property_Designator";

   CString ccKeyword = keyword;
   int keyIndex = 0;
   if (!this->edifToCCKeyword.Lookup(keyword, ccKeyword))
   {
      if (valueType == -1)
      {
         // Keyword is not found and no value type, which means that this keyword was never defined in Edif
         fprintf(sFileErr, "Syntax Error : Reference to property \"%s\" that is not previous defined around line %ld\n", keyword, sParser->lineCount);
         sDisplayErr++;
         return -1;
      }

      // It is not in edifToCCKeyword so check it to see if it is already in CAMCAD keyword
      keyIndex = sDoc->IsKeyWord(keyword, 0);

      if (keyIndex < 0)
      {
         // Not a CAMCAD keyword, so add it
         ccKeyword = keyword;
         keyIndex = sDoc->RegisterKeyWord(keyword, 0, valueType);
      }
      else if (sDoc->getKeyWordArray()[keyIndex]->getValueType() != valueType)
      {
         // Is a CAMCAD keyword, but value type is different so create an new keyword specifically for Edif200
         ccKeyword.Format("Edif200.%d", keyword);
         keyIndex = sDoc->RegisterKeyWord(ccKeyword, 0, valueType);
      }
      this->edifToCCKeyword.SetAt(keyword, ccKeyword);
      this->edifKeyToType.SetAt(keyword, valueType);
      this->edifKeyToIndex.SetAt(keyword, keyIndex);
   }
   else
   {
      if (!this->edifKeyToType.Lookup(keyword, valueType))
      {
         // The keyword is not map to a value type so map it
         this->edifKeyToType.SetAt(keyword, valueType);
      }
      
      if (!this->edifKeyToIndex.Lookup(keyword, keyIndex))
      {
         // the keyword is not map to a keyword index so map it
         keyIndex = sDoc->RegisterKeyWord(keyword, 0, valueType);
         this->edifKeyToIndex.SetAt(keyword, keyIndex);
      }
   }

   Attrib* attrib = NULL;

   (*attribMap)->Lookup(keyIndex, attrib);

   switch (valueType)
   {
   case VT_STRING:
      {
         if (attrib && (attribFlag & ATTRIB_CHANGE_VALUE))
            attrib->setStringValueIndex(sDoc->RegisterValue(value));
         else if (attrib == NULL)
            sDoc->SetAttrib(&(*attribMap), keyIndex, valueType, value.GetBuffer(0), SA_OVERWRITE, &attrib);
      }
      break;
   case VT_INTEGER:
      {
         int intValue = 0;

         if (value != "")
            intValue = atoi(value);

         if (attrib && (attribFlag & ATTRIB_CHANGE_VALUE))
            attrib->setIntValue(intValue);
         else if (attrib == NULL)
            sDoc->SetAttrib(&(*attribMap), keyIndex, valueType, &intValue, SA_OVERWRITE, &attrib);
      }
      break;
   case VT_DOUBLE:
      {
         int dblValue = 0;

         if (value != "")
            dblValue = atoi(value);

         if (attrib && (attribFlag & ATTRIB_CHANGE_VALUE))
            attrib->setDoubleValue(dblValue);
         else if (attrib == NULL)
            sDoc->SetAttrib(&(*attribMap), keyIndex, valueType, &dblValue, SA_OVERWRITE, &attrib);
      }
      break;
   default:
      break;
   }

   if (attrib)
   {
		POSITION pos = display->displayList.GetHeadPosition();
		while (pos)
		{
			DisplayParam *displayParam = display->displayList.GetNext(pos);

			if (!displayParam->m_isEmpty)
			{
				Attrib *newAttrib = NULL;
				if (attrib->getFlags() & ATTRIB_CHANGE_LOCATION)
				{
#if CamCadMajorMinorVersion > 406  //  > 4.6
               newAttrib = attrib->getCamCadData().constructAttribute();
#else
					newAttrib = new Attrib;
#endif
					attrib->addInstance(newAttrib);
				}
				else
				{
					newAttrib = attrib;
				}

				newAttrib->setFlagBits(attribFlag);
				newAttrib->setMirrorDisabled(true);
				newAttrib->setProportionalSpacing(true);
				newAttrib->setFlagBits(ATTRIB_CHANGE_LOCATION);
				newAttrib->setCoordinate(displayParam->m_x, displayParam->m_y);
				newAttrib->setRotationRadians( DegToRad(displayParam->m_rotation));
				newAttrib->setHeight( displayParam->m_figureGroup.m_textHeight);
				newAttrib->setPenWidthIndex( displayParam->m_figureGroup.m_widthIndex); 
				newAttrib->setVisible( displayParam->m_figureGroup.m_visible);
				newAttrib->setWidth( displayParam->m_figureGroup.m_textWidth);
				newAttrib->setHorizontalPosition(intToHorizontalPositionTag(displayParam->m_horizontalJust));
				newAttrib->setVerticalPosition(intToVerticalPositionTag(displayParam->m_verticalJust));

				if (newAttrib->isVisible())
				{
					CString layerName = "";
					layerName.Format("AttrLyr_%s", ccKeyword);
					//layerName.Format("AttrLyr_%s_%d", ccKeyword, attrib->getCount());

					newAttrib->setLayerIndex( Graph_Level(layerName, "", FALSE));
				}
			}
		}

		// Assign the attrib flag again just in case it didn't get into the while loop 
		attrib->setFlagBits(attribFlag);
	}

   if (attribPtr)
      *attribPtr = attrib;

   return 1;
}

/******************************************************************************
* Port::DoPort

   This function will do the following commands:
   - direction
   - unused
   - designator
   - property

   Skip the following commands:
   - dcFaninLoad
   - dcFanoutLoad
   - dcMaxFanin
   - dcMaxFanout
   - adload
   - portDelay
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int Port::DoPort(CAttributes** attribMap)
{
   int res = 0;
   int parenthesisCount = 0;

   CString portName;
   CString renameString = "";
	CString arrayName = "";
   Display display;
	int lbIndex = 0;
	int ubIndex = 0;
	int size = -1;

	if ((res = GetArray(portName, renameString, display, arrayName, lbIndex, ubIndex, size)) < 0)
	{
		return res;
	}
	else if (res == 0)
	{
		// Not an array so get net name
		if ((res = GetNameDef(portName, renameString, display)) < 0)
			return res;
	}

   if (!portName.IsEmpty())
      this->portName = portName;
   if (!arrayName.IsEmpty())
      this->portName = arrayName;

   if (*attribMap == NULL)
      *attribMap = new CAttributes();

	Attrib *portnameAttrib = NULL;
   sEdifAttrib->AddDefinedAttrib(&(*attribMap), SCH_ATT_PORTNAME, this->portName, ATTRIB_CHANGE_VALUE, &display, &portnameAttrib);

	// Only in DoPort can a port name be renamed because here is when the name first defined
	// so add it to the port
	if (!renameString.IsEmpty() && portnameAttrib != NULL)
	{
		CString *newName = new CString(renameString);
		sPortRenameMap.SetAt(portnameAttrib, newName);
	}

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "dcFaninLoad") || !STRCMPI(sParser->token, "dcFanoutLoad") ||
               !STRCMPI(sParser->token, "dcMaxFanin") || !STRCMPI(sParser->token, "dcMaxFanout") ||
               !STRCMPI(sParser->token, "adload") || !STRCMPI(sParser->token, "portDelay") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData") )
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "direction"))
      {
         CString direction;
         Display display;
         if ((res = GetString(direction)) < 0)
            return res;

         sEdifAttrib->AddDefinedAttrib(&(*attribMap), SCH_ATT_DIRECTION, direction, ATTRIB_CHANGE_VALUE, &display, NULL);
      }
      else if (!STRCMPI(sParser->token, "unused"))
      {
         Display display;
         sEdifAttrib->AddDefinedAttrib(&(*attribMap), "Unused", "", ATTRIB_CHANGE_VALUE, &display, NULL);
      }
      else if (!STRCMPI(sParser->token, "designator"))
      {
         CString designator = "";
         Display display;

         if ((res = GetNameDef(designator, renameString, display)) < 0)
            return res;

         if(res > 0 && STRCMPI(sParser->token, "0"))
         {
            CString *renamePtr = NULL;           
            if(!designator.IsEmpty() && renameString.IsEmpty() && !sPortRenameMap.Lookup(portnameAttrib, (void*&)renamePtr)) 
            {
               renamePtr = new CString(designator);
               sPortRenameMap.SetAt(portnameAttrib, renamePtr);
            }
         }

         sEdifAttrib->AddDefinedAttrib(&(*attribMap), SCH_ATT_DESIGNATOR, designator, ATTRIB_CHANGE_VALUE, &display, NULL);
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
         if ((res = GetProperty(&(*attribMap))) < 0)
            return res;
      }
   }

   return 1;
}

/******************************************************************************
* EdifFigure::DoFigure

   This function will do the following commands:
   - circle
   - dot
   - openShape
   - path
   - polygon
   - rectangle
   - shape

   Skip the following commands:
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int EdifFigure::DoFigure(int layerIndex, bool filled)
{
   int res = 0;
   int parenthesisCount = 0;

   CString figureGroupName = "";
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(figureGroupName, renameString, display)) < 0)
      return res;
   else if (res == 1)
   {
		CEdif200Library* library = sEdif200.getCurrentLibrary();
		if (library != NULL)
		{
			FigureGroup* figureGroup = library->findFigureGroup(figureGroupName);
			if (figureGroup != NULL)
			{
				this->figureGroup = *figureGroup;
			}
		}
   }

   DataStruct *dotData = NULL;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData") )
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figureGroupOverride"))
      {
         CString figureGroupName = "";
         CString renameString = "";
         Display display;
         if ((res = GetNameDef(figureGroupName, renameString, display)) < 0)
            return res;
         else if (res == 0)
         {
            sDisplayErr++;
            return -1;
         }

			CEdif200Library* library = sEdif200.getCurrentLibrary();
			if (library != NULL)
			{
				FigureGroup* figureGroup = library->findFigureGroup(figureGroupName);
				if (figureGroup != NULL)
				{
					this->figureGroup = *figureGroup;
					this->figureGroup.DoFigureGroup();
				}
			}
      }
      else if (!STRCMPI(sParser->token, "circle"))
      {
         CPoly *poly = this->CreatePoly();

         if ((res = GetPoints(poly->getPntList())) < 0)
         {
            delete poly;
            return res;
         }

         CPnt *firstPnt = (CPnt*)poly->getPntList().GetHead();
         firstPnt->bulge = 1;

         CPnt *lastPnt = (CPnt*)poly->getPntList().GetTail();
         lastPnt->bulge = 1;

         // Add the last point to close the circle
         lastPnt = new CPnt();
         lastPnt->x = firstPnt->x;
         lastPnt->y = firstPnt->y;
         lastPnt->bulge = 0.0;
         poly->getPntList().AddTail(lastPnt);
         poly->setClosed(true);
         poly->setFilled(filled);

         this->AddPolyToPolyList(poly, layerIndex);
      }
      else if (!STRCMPI(sParser->token, "dot"))
      {
         double x = 0;
         double y = 0;
         if ((res = GetPoint(x, y)) < 0)
            return res;

         if (this->isPort)
         {
            dotData = Graph_Block_Reference("DEFAULTPORT", "", -1, x, y, 0.0, 0, 1, layerIndex, TRUE);
            dotData->getInsert()->setInsertType(insertTypePortInstance);
         }
         //else
         //{
         // dotData = Graph_Point(x, y, layerIndex, 0L, FALSE);
         //}
      }
      else if (!STRCMPI(sParser->token, "openShape"))
      {
         CPoly *poly = this->CreatePoly();

         if ((res = GetCurve(poly->getPntList())) < 0)
         {
            delete poly;
            return res;
         }

         this->AddPolyToPolyList(poly, layerIndex);
      }
      else if (!STRCMPI(sParser->token, "path"))
      {
         CPoly *poly = this->CreatePoly();

         if ((res = GetPointList(poly->getPntList())) < 0)
         {
            delete poly;
            return res;
         }

         this->AddPolyToPolyList(poly, layerIndex);
      }
      else if (!STRCMPI(sParser->token, "polygon"))
      {
         CPoly *poly = this->CreatePoly();

         if ((res = GetPointList(poly->getPntList())) < 0)
         {
            delete poly;
            return res;
         }

         CPnt *firstPnt = (CPnt*)poly->getPntList().GetHead();
         CPnt *lastPnt = (CPnt*)poly->getPntList().GetTail();

         if (fabs(firstPnt->x - lastPnt->x) > SMALLNUMBER || fabs(firstPnt->y - lastPnt->y) > SMALLNUMBER)
         {
            // Add the last point to close the shape
            lastPnt = new CPnt();
            lastPnt->x = firstPnt->x;
            lastPnt->y = firstPnt->y;
            lastPnt->bulge = 0.0;
            poly->getPntList().AddTail(lastPnt);
         }
         poly->setClosed(true);

         this->AddPolyToPolyList(poly, layerIndex);
      }
      else if (!STRCMPI(sParser->token, "rectangle"))
      {
         CPoly *poly = this->CreatePoly();

         CPntList pntList;
         if ((res = GetPoints(pntList)) < 0)
         {
            delete poly;
            return res;
         }

         CPnt *thirdPnt = (CPnt*)pntList.RemoveTail();   // remove second set of points from list
         CPnt *firstPnt = (CPnt*)pntList.RemoveTail();   // remove first set of points from list

         // add the first point of rectangle
         poly->getPntList().AddTail(firstPnt);

         // add the second point of rectangle
         CPnt *pnt = new CPnt();
         pnt->x = firstPnt->x;
         pnt->y = thirdPnt->y;
         pnt->bulge = 0;
         poly->getPntList().AddTail(pnt);

         // add the third point of rectangle
         poly->getPntList().AddTail(thirdPnt);

         // add the forth point of rectangle
         pnt = new CPnt();
         pnt->x = thirdPnt->x;
         pnt->y = firstPnt->y;
         pnt->bulge = 0;
         poly->getPntList().AddTail(pnt);

         // copy the first point to the last point and add it to close the rectangle
         pnt = new CPnt();
         pnt->x = firstPnt->x;
         pnt->y = firstPnt->y;
         pnt->bulge = 0;
         poly->getPntList().AddTail(pnt);
         poly->setClosed(true);

         this->AddPolyToPolyList(poly, layerIndex);
      }
      else if (!STRCMPI(sParser->token, "shape"))
      {
         CPoly *poly = this->CreatePoly();

         if ((res = GetCurve(poly->getPntList())) < 0)
         {
            delete poly;
            return res;
         }

         CPnt *firstPnt = (CPnt*)poly->getPntList().GetHead();
         CPnt *lastPnt = (CPnt*)poly->getPntList().GetTail();

         if (fabs(firstPnt->x - lastPnt->x) > SMALLNUMBER || fabs(firstPnt->y - lastPnt->y) > SMALLNUMBER)
         {
            // Add the last point to close the shape
            lastPnt = new CPnt();
            lastPnt->x = firstPnt->x;
            lastPnt->y = firstPnt->y;
            lastPnt->bulge = 0.0;
            poly->getPntList().AddTail(lastPnt);
         }
         poly->setClosed(true);

         this->AddPolyToPolyList(poly, layerIndex);
      }
   }

   if (this->isPort && this->data == NULL && dotData != NULL)
      this->data = dotData;

   return 1;
}

/******************************************************************************
* EdifFigure::CreatePoly
*/
CPoly* EdifFigure::CreatePoly()
{
   CPoly *poly = new CPoly();
   poly->setWidthIndex(0);
   poly->setFilled(false);
   poly->setVoid(false);
   poly->setClosed(false);
   poly->setThermalLine(false);
   poly->setFloodBoundary(false);
   poly->setHidden(false);
   poly->setHatchLine(false);

   return poly;
}

/******************************************************************************
* EdifFigure::AddPolyToPolyList
*/
int EdifFigure::AddPolyToPolyList(CPoly *poly, int layerIndex)
{
   if (poly->getPntList().GetCount() == 0)
   {
      delete poly;
      return 1;
   }

   if (this->data == NULL)
   {
      // only creat if it is not create yet
      this->data = Graph_PolyStruct(layerIndex, 0L, FALSE);
		if (this->isPort)
         data->getInsert()->setInsertType(insertTypePortInstance);
   }

   if (this->data->getPolyList() == NULL)
      this->data->getPolyList() = new CPolyList();

   poly->setWidthIndex(this->figureGroup.m_widthIndex);
   this->data->getPolyList()->AddTail(poly);
   poly = NULL;
   
   return 1;
}

//-----------------------------------------------------------------------------
// CEdif200Port
//-----------------------------------------------------------------------------
CEdif200Port::CEdif200Port(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200ConnectorType connectorType)
	: CEdif200ConnectorObject(edif, libraryName, cellName, viewName, pageName, name, connectorType)
{
	reset();
}
CEdif200Port::~CEdif200Port()
{
	reset();
}

void CEdif200Port::reset()
{
	m_portRef.Empty();
	m_portDataStruct = NULL;
	m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CEdif200InstancePort
//-----------------------------------------------------------------------------
CEdif200InstancePort::CEdif200InstancePort(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString instanceName, const CString name)
	: CEdif200ConnectorObject(edif, libraryName, cellName, viewName, "", name, connectorTypeInstancePort)
	, m_instanceName(instanceName)
{
	reset();
}

CEdif200InstancePort::~CEdif200InstancePort()
{
	reset();
}

void CEdif200InstancePort::reset()
{
	m_instanceName.Empty();
	m_designator.Empty();
	m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CEdif200PageConnector
//-----------------------------------------------------------------------------
CEdif200PageConnector::CEdif200PageConnector(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200ConnectorType connectorType)
: CEdif200ConnectorObject(edif, libraryName, cellName, viewName, pageName, name, connectorType)
{
	reset();
}

CEdif200PageConnector::~CEdif200PageConnector()
{
	reset();
}

void CEdif200PageConnector::reset()
{
	m_connectorDataStruct = NULL;
	m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CEdif200RipperHotspot
//-----------------------------------------------------------------------------
CEdif200RipperHotspot::CEdif200RipperHotspot(const CString name)
{
   reset();
   m_name = name;
}

CEdif200RipperHotspot::~CEdif200RipperHotspot()
{
   reset();
}

void CEdif200RipperHotspot::reset()
{
   m_pin.Empty();
   m_rule.Empty();
   m_connectedToSignal = NULL;
}

//-----------------------------------------------------------------------------
// CEdif200Ripper
//-----------------------------------------------------------------------------
CEdif200Ripper::CEdif200Ripper(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200ConnectorType connectorType)
: CEdif200ConnectorObject(edif, libraryName, cellName, viewName, pageName, name, connectorType),
  m_ripperHotspotMap(nextPrime2n(20), true)
{
   m_attribMap = NULL;
	reset();
}

CEdif200Ripper::~CEdif200Ripper()
{
	reset();
}

void CEdif200Ripper::reset()
{
	m_wireRule.Empty();
   m_hotspotConnectedToBus.Empty();
	m_connectedToSignal1 = NULL;
	m_connectedToSignal2 = NULL;
   m_ripperHotspotMap.empty();

   delete m_attribMap;
   m_attribMap = NULL;
}

CEdif200RipperHotspot* CEdif200Ripper::addRipperHotspot(const CString ripperHotspotName)
{  
   CEdif200RipperHotspot* hotspot = findRipperHotspot(ripperHotspotName);
   if (hotspot == NULL)
   {
      hotspot = new CEdif200RipperHotspot(ripperHotspotName);
      m_ripperHotspotMap.SetAt(ripperHotspotName, hotspot);
   }
   
   return hotspot;
}

CEdif200RipperHotspot* CEdif200Ripper::findRipperHotspot(const CString ripperHotspotName)
{  
   CEdif200RipperHotspot* hotspot = NULL;
   m_ripperHotspotMap.Lookup(ripperHotspotName, hotspot);
   return hotspot;
}


CEdif200Net* CEdif200Ripper::getConnectedToNet(CEdif200Net* fromNet)
{
   CEdif200Net* connectedNet = NULL;
	if (fromNet == NULL || fromNet->getSignalType() != signalTypeNet || m_ripperHotspotMap.GetCount() > 2)
	{
		return NULL;
	}
   else
   {
      for (POSITION pos=getRipperHotspotStartPosition();  pos != NULL;)
      {
         CEdif200RipperHotspot* hotspot = getNextRipperHotspot(pos);
         if (hotspot == NULL || hotspot->getConnectedToSignal() == NULL)
            continue;

         if (hotspot->getConnectedToSignal() == fromNet)
            continue;

         if (hotspot->getConnectedToSignal()->getSignalType() != signalTypeNet)
            continue;

         connectedNet = (CEdif200Net*)hotspot->getConnectedToSignal();
         break;
      }
   }

   return connectedNet;
}

//-----------------------------------------------------------------------------
// CEdif200Signal
//-----------------------------------------------------------------------------
CEdif200Signal::CEdif200Signal(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200SignalType signalType)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_viewName(viewName)
   , m_pageName(pageName)
	, m_name(name)
	, m_signalType(signalType)
{
   m_attribMap = NULL;
	reset();
}

CEdif200Signal::~CEdif200Signal()
{
	reset();
}

void CEdif200Signal::reset()
{
	m_primaryName.Empty();
	m_annotateName.Empty();
   m_renamedName.Empty();
	m_isNameUserDefined = false;
	m_isRenameDone = false;
	m_renamedBy = renamedByUndefined;

   delete m_attribMap;
   m_attribMap = NULL;
}

void CEdif200Signal::prepareForRename()
{
	// Net must prepare for rename by applying annotated name or primary name when available
	
	WORD primaryNameKw = (WORD)sDoc->RegisterKeyWord(SCH_ATT_PRIMARYNAME, 0, VT_STRING);
	Attrib* attrib = NULL;
	if (m_attribMap != NULL)
	{
      if (m_attribMap->Lookup(primaryNameKw, attrib))
      {
#if CamCadMajorMinorVersion > 406  //  > 4.6
         m_primaryName = attrib->getStringValue();
#else
		   m_primaryName = sDoc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
      }

      if (!sNetBackannotate.IsEmpty())
      {
         WORD netBackannotateKw = sDoc->RegisterKeyWord(sNetBackannotate, 0, valueTypeString);         
         if (m_attribMap->Lookup(netBackannotateKw, attrib))
         {
#if CamCadMajorMinorVersion > 406  //  > 4.6
            m_annotateName = attrib->getStringValue();
#else
		      m_annotateName = sDoc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
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

void CEdif200Signal::setRenamedName(const CString renamedName, const EEdif200RenamedBy renamedBy)
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
}

//-----------------------------------------------------------------------------
// CEdif200Net
//-----------------------------------------------------------------------------
CEdif200Net::CEdif200Net(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name)
	: CEdif200Signal(edif, libraryName, cellName, viewName, pageName, name, signalTypeNet)
{
   reset();
}

CEdif200Net::CEdif200Net(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CEdif200Net& net)
	: CEdif200Signal(edif, libraryName, cellName, viewName, pageName, net.m_name, signalTypeNet)
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

CEdif200Net::~CEdif200Net()
{
}

//-----------------------------------------------------------------------------
// CEdif200Bus
//-----------------------------------------------------------------------------
CEdif200Bus::CEdif200Bus(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name)
	: CEdif200Signal(edif, libraryName, cellName, viewName, pageName, name, signalTypeBus)
	, m_netMap(nextPrime2n(20),false)
	, m_busMap(nextPrime2n(20),false)
{
	reset();
}

CEdif200Bus::CEdif200Bus(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, CEdif200Page& page, const CEdif200Bus& bus)
	: CEdif200Signal(edif, libraryName, cellName, viewName, page.getName(), bus.m_name, signalTypeBus)
	, m_netMap(nextPrime2n(20),false)
	, m_busMap(nextPrime2n(20),false)
{
	reset();

   m_primaryName = bus.m_primaryName;
   m_annotateName = bus.m_annotateName;
   m_isNameUserDefined = bus.m_isNameUserDefined;
   m_isRenameDone = bus.m_isRenameDone;

	if (bus.m_attribMap != NULL)
	{
		m_attribMap = new CAttributes(*bus.m_attribMap);
	}

   POSITION pos;
   CString key;
   for (pos = bus.m_netMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200Net* net = NULL;     
      bus.m_netMap.GetNextAssoc(pos, key, net);
      if (net != NULL)
      {
         // Cannot copy the pointer of the net because it is belong to another map on a different view.
         // Only get the net name and find it on view that this bus belong to.  If the view doesn't have the net, it will creat one.
         CEdif200Net* newNet = page.addNet(key);
         addNet(newNet);
      }
   }

   for (pos = bus.m_busMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200Bus* subBus = NULL;
      bus.m_busMap.GetNextAssoc(pos, key, subBus);
      if (subBus != NULL)
      {
         CEdif200Bus* newBus = page.addBus(key);
         addBus(newBus);
      }
   }
}

CEdif200Bus::~CEdif200Bus()
{
   reset();
}

void CEdif200Bus::reset()
{
   CEdif200Signal::reset();

	m_netMap.empty();
	m_busMap.empty();
};

void CEdif200Bus::addBus(CEdif200Bus* bus)
{
   if (bus != NULL)
		m_busMap.SetAt(bus->getName(), bus);
}

void CEdif200Bus::addNet(CEdif200Net* net)
{
   if (net != NULL)
		m_netMap.SetAt(net->getName(), net);
}

void CEdif200Bus::createNetLis()
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
      CEdif200Bus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      if (bus != NULL)
      {
         busStruct->AddBusName(bus->getRenamedName());
      }
   }

   for (POSITION pos = m_netMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200Net* net = NULL;
      CString key;
      m_netMap.GetNextAssoc(pos, key, net);
      if (net != NULL)
      {
         busStruct->AddNetName(net->getRenamedName());
      }
   }
}

//-----------------------------------------------------------------------------
// CEdif200Instance
//-----------------------------------------------------------------------------
CEdif200Instance::CEdif200Instance(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_viewName(viewName)
	, m_name(name)
	, m_instancePortMap(nextPrime2n(20),true)
{
	reset();
}

CEdif200Instance::~CEdif200Instance()
{
	reset();
}

void CEdif200Instance::reset()
{
	m_primaryName.Empty();
	m_displayName.Empty();
	m_designator.Empty();
   m_libraryRef.Empty();
	m_cellRef.Empty();
   m_viewRef.Empty();
	m_symbolRef.Empty();

	m_instanceDataStruct = NULL;
	m_instancePortMap.empty();

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
}

CEdif200InstancePort* CEdif200Instance::addInstancePort(CString portRef)
{
	CEdif200InstancePort* instancePort = findInstancePort(portRef);
	if (instancePort == NULL)
	{
		instancePort = new CEdif200InstancePort(m_edif, m_libraryName, m_cellName, m_viewName, m_name, portRef);
		m_instancePortMap.SetAt(portRef, instancePort);
	}
	return instancePort;
}

CEdif200InstancePort* CEdif200Instance::findInstancePort(CString portRef)
{
	CEdif200InstancePort* instancePort = NULL;
	m_instancePortMap.Lookup(portRef, instancePort);
	return instancePort;
}

bool CEdif200Instance::isHierarchSymbol()
{
   bool retval = false;
   if (m_instanceDataStruct != NULL && m_instanceDataStruct->getInsert() != NULL)
   {
      retval = m_instanceDataStruct->getInsert()->getInsertType()==insertTypeHierarchicalSymbol;
   }
   return retval;
}

//-----------------------------------------------------------------------------
// CSymbolPorts
//-----------------------------------------------------------------------------
CEdif200SymbolPort::CEdif200SymbolPort(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString symbolName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
   , m_viewName(viewName)
	, m_symbolName(symbolName)
	, m_name(name)
{
	m_portDataStruct = NULL;
}

CEdif200SymbolPort::~CEdif200SymbolPort()
{
	m_portDataStruct = NULL;
}

//-----------------------------------------------------------------------------
// CEdif200Symbol
//-----------------------------------------------------------------------------
CEdif200Symbol::CEdif200Symbol(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
   , m_viewName(viewName)
	, m_name(name)
	, m_portMap(nextPrime2n(20),true)
{
	reset();
}

CEdif200Symbol::CEdif200Symbol(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, CEdif200Symbol& symbol)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
   , m_viewName(viewName)
	, m_name(symbol.m_name)
	, m_portMap(nextPrime2n(20),true)
{
	reset();
   m_symbolBlock = symbol.m_symbolBlock;

   for (POSITION pos = symbol.getSymbolPortStartPosition(); pos != NULL;)
   {
      CEdif200SymbolPort* symbolPort = symbol.getNextSymbolPort(pos);
      if (symbolPort != NULL)
      {
         CEdif200SymbolPort* copy = new CEdif200SymbolPort(m_edif, m_libraryName, m_cellName, m_viewName, m_name, symbolPort->getName());
         m_portMap.SetAt(copy->getName(), copy);
         copy->setPortRef(symbolPort->getPortRef());
         copy->setPortDataStruct(symbolPort->getPortDataStruct());
      }
   }
}

CEdif200Symbol::~CEdif200Symbol()
{
	reset();
}

void CEdif200Symbol::reset()
{
	m_portMap.empty();
	m_symbolBlock = NULL;
}

CEdif200SymbolPort* CEdif200Symbol::addPort(CString portInstanceName)
{
	CEdif200SymbolPort* port = findPort(portInstanceName);
	if (port == NULL)
	{
		port = new CEdif200SymbolPort(m_edif, m_libraryName, m_cellName, m_viewName, m_name, portInstanceName);
		m_portMap.SetAt(portInstanceName, port);
	}
	return port;
}

CEdif200SymbolPort* CEdif200Symbol::findPort(CString portInstanceName)
{
	CEdif200SymbolPort* port =  NULL;
	m_portMap.Lookup(portInstanceName, port);
	return port;
}

CString CEdif200Symbol::generateSymbolBlockName()
{
   CString blockName;
   blockName.Format("%s%c%s%c%s%c%s", m_libraryName, SCH_DELIMITER, m_cellName, SCH_DELIMITER, m_viewName, SCH_DELIMITER, m_name);
	blockName.MakeUpper();
	return blockName;
}

/******************************************************************************
* CEdif200Symbol::doSymbol

   This function will do the following commands:
   - portImplementation
   - figure
   - annotate
   - instance
   - commentGraphics
   - propertyDisplay
   - keywordDisplay
   - property

   Skip the following commands:
   - pageSize
   - boundingBox
   - parameterDisplay
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Symbol::doSymbol()
{
   int res = 0;
   int parenthesisCount = 0;

   BlockStruct* block = Graph_Block_On(GBO_APPEND, m_symbolBlock->getName(), sFileStruct->getFileNumber(), 0L);
   block->setBlockType(blockTypeSymbol);

   // Figure has to be declare here so the Figure constructor only call once
   EdifFigure figure;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "pageSize") || !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData") ||
               !STRCMPI(sParser->token, "parameterDisplay") || !STRCMPI(sParser->token, "boundingBox") )
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "portImplementation"))
      {
         CString portName;
         DataStruct* portData = NULL;
         if ((res = doPortImplementation(m_edif, portName, &portData, m_edif.getCurrentView()->m_portAttribMap)) < 0)
            return res;

         if (portData != NULL)
         {
            CEdif200SymbolPort* symbolPort = addPort(portData->getInsert()->getRefname());
            symbolPort->setPortDataStruct(portData);
            symbolPort->setPortRef(portName);
         }
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sSymbolGrpLayer)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "annotate"))
      {
         if ((res = doAnnotate()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "instance"))
      {
         if ((res = doInstance(m_edif, NULL)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "commentGraphics"))
      {
         if ((res = doCommentGraphics(m_edif)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "propertyDisplay"))
      {
         if ((res = GetPropertyDisplay(&m_symbolBlock->getAttributesRef())) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "keywordDisplay"))
      {
         if ((res = GetKeywordDisplay(&m_symbolBlock->getAttributesRef())) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
         if ((res = GetProperty(&m_symbolBlock->getAttributesRef())) < 0)
            return res;
      }
   }

   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* doPortImplementation

   This function will do the following commands:
   - connectionLocation
   - figure
   - instance
   - commentGraphics
   - propertyDisplay
   - keywordDisplay
   - property

   Skip the following commands:
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int doPortImplementation(CEdif200& edif, CString& portName, DataStruct** portDataStruct, CTypedMapStringToPtrContainer<CAttributes*>& portAttributeMap)
{
   int res = 0;
   int parenthesisCount = 0;

   portName = "";
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(portName, renameString, display)) < 0)
      return res;

   DataStruct* localPortDataStruct = NULL;
	CEdif200Instance* instance = NULL;
   CAttributes* attribMap = new CAttributes();
   sEdifAttrib->AddDefinedAttrib(&attribMap, SCH_ATT_PORTNAME, portName, ATTRIB_CHANGE_VALUE, &display, NULL);

   // Figure has to be declare here so the Figure constructor only call once
   EdifFigure figure;
   figure.isPort = TRUE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sPortGrpLayer)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "connectLocation")) 
      {
         if ((res = doConnectLocation(&localPortDataStruct)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "instance"))
      {
         if ((res = doInstance(edif, &instance)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "commentGraphics"))
      {
         if ((res = doCommentGraphics(edif)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "propertyDisplay"))
      {
         if ((res = GetPropertyDisplay(&attribMap)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "keywordDisplay"))
      {
         if ((res = GetKeywordDisplay(&attribMap)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
         if ((res = GetProperty(&attribMap)) < 0)
            return res;
      }
   }


   if (instance != NULL && instance->getInstanceDataStruct() != NULL)
   {
      // remove the connect location since it already has an instance insert as a port
      if (localPortDataStruct)
      {
         CDataList *dataList = GetCurrentDataList();
         RemoveOneEntityFromDataList(sDoc, dataList, localPortDataStruct, NULL);
      }

      // Set the portDataStruct to instance->getInstanceDataStruct()
      localPortDataStruct = instance->getInstanceDataStruct();
   }

   if (localPortDataStruct != NULL)
   {
      if (instance == NULL)
      {
         // only set refname to portName if portDataStruct is not and instance insert by figure graphic
         localPortDataStruct->getInsert()->setRefname(STRDUP(portName));
      }

      Transform transform;
      transform.isEmpty = FALSE;
      transform.mirror = localPortDataStruct->getInsert()->getMirrorFlags();
      transform.rotation = RadToDeg(localPortDataStruct->getInsert()->getAngle());
      transform.scaleX = localPortDataStruct->getInsert()->getScale();
      transform.scaleY = localPortDataStruct->getInsert()->getScale();
      transform.x = localPortDataStruct->getInsert()->getOriginX();
      transform.y = localPortDataStruct->getInsert()->getOriginY();

      // Apply the offset of the instance insert to the attribs
      WORD word;
      Attrib* attrib;
      
      for (POSITION attribPos = attribMap->GetStartPosition();attribPos != NULL;)
      {
         attribMap->GetNextAssoc(attribPos, word, attrib);

         if (attrib != NULL && (attrib->getFlags() & ATTRIB_CHANGE_LOCATION))
            AttribUntransform(attrib, transform);
      }

      // Copy the attribs defined here
      CopyAttribMap(&localPortDataStruct->getAttributesRef(), attribMap);
   
      // Copy the attribs of the port defined in interface
      CAttributes* fromAttribMap = NULL;

      if (portAttributeMap.Lookup(portName, fromAttribMap))
      {
         CopyAttribMap(&localPortDataStruct->getAttributesRef(), fromAttribMap);
      }

      localPortDataStruct->getInsert()->setInsertType(insertTypePortInstance);      
   }

   // Clean up the allocated memory
   FreeAttribMap(&attribMap);

	delete instance;
	instance = NULL;

   if (portDataStruct)
   {
      *portDataStruct = localPortDataStruct;
   }

   return 1;
}

/******************************************************************************
* View::DoConnectLocation

   This function will do the following commands:
   - figure

   Return code:
    1 - no error
   -1 - syntax error
*/
int doConnectLocation(DataStruct** data)
{
   int res = 0;
   int parenthesisCount = 0;

   // Figure has to be declare here so the Figure constructor only call once
   EdifFigure figure;
   figure.isPort = TRUE;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sPortGrpLayer)) < 0)
            return res;
      }
   }

   if (data)
   {
      *data = figure.data;
   }

   return 1;
}

/******************************************************************************
* doAnnotate

   This function will do the following commands:
   - stringValue
   - stringDisplay

   Return code:
    1 - no error
   -1 - syntax error
*/
int doAnnotate()
{
   int res = 0;
   int parenthesisCount = 0;
   CString stringValue = "";

   if (sParser->peekToken[0] == '(' && !sParser->isPeekTokenName)
   {
      // Expecting "stringDisplay" construct

      if (!sParser->GetToken())        // Get the '('
         return sParser->ParsingErr();

      if (!sParser->GetToken())        // Get the word 'stringDisplay'
         return sParser->ParsingErr();
      if (STRCMPI(sParser->token, "stringDisplay"))   
         return sParser->ParsingErr();

      // Get the display string
      if ((res = GetString(stringValue, true)) < 0)
         return res;

      // Get the display location and display the text
      if (sParser->peekToken[0] == '(' && !sParser->isPeekTokenName)
      {

         if (!sParser->GetToken())        // Get the '('
            return sParser->ParsingErr();

         if (!sParser->GetToken())        // Get the word 'display'
            return sParser->ParsingErr();
         if (STRCMPI(sParser->token, "display"))   
            return sParser->ParsingErr();

         Display display;
         if ((res = display.DoDisplay()) < 0)
            return res;

         if (!sParser->GetToken())        // Get the ')'
            return sParser->ParsingErr();
         if (sParser->token[0] != ')')
            return sParser->ParsingErr();

			POSITION pos = display.displayList.GetHeadPosition();
			while (pos)
			{
				DisplayParam *displayParam = display.displayList.GetNext(pos);

				if (!displayParam->m_isEmpty && displayParam->m_figureGroup.m_visible)
				{
					// Because Graph_Text will add an extra % to it
					stringValue.Replace("%%", "%");
					DataStruct *textdata = Graph_Text(sTextLayer, stringValue, displayParam->m_x, displayParam->m_y, displayParam->m_figureGroup.m_textHeight, 
							displayParam->m_figureGroup.m_textWidth, DegToRad(displayParam->m_rotation), 0L, TRUE, displayParam->m_mirror, 
							0, FALSE, displayParam->m_figureGroup.m_widthIndex, 0, displayParam->m_horizontalJust, displayParam->m_verticalJust);
               if (textdata != NULL)
                  textdata->getText()->setMirrorDisabled(true);
				}
			}
      }

      if (!sParser->GetToken())        // Get the ')'
         return sParser->ParsingErr();
      if (sParser->token[0] != ')')
         return sParser->ParsingErr();
   }
   else
   {
      // If it is only the text and no display location then ignore it
      if ((res = GetString(stringValue)) < 0)
         return res;
   }

   return 1;      
}

/******************************************************************************
* doCommentGraphics

   This function will do the following commands:
   - annotate
   - figure
   - instance

   Skip the following commands:
   - boundingBox
   - property     : no need to get property of a comment graphic
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int doCommentGraphics(CEdif200& edif)
{
   int res = 0;
   int parenthesisCount = 0;

   // Figure has to be declare here so the Figure constructor only call once
   EdifFigure figure;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "boundingBox") || !STRCMPI(sParser->token, "property") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData")) 
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "annotate"))
      {
         if ((res = doAnnotate()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sAttribLayer)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "instance"))
      {
         if ((res = doInstance(edif, NULL)) < 0)
            return res;
      }
   }

   return 1;   
}

/******************************************************************************
* doRenamePortName : 
   This function rename all the portname attribute
*/
static bool doRenamePortName(CEdif200& edif, CEdif200Instance* instance)
{
   if(!instance)
      return false;

   CEdif200View* view = edif.findView(instance->getLibraryRef(), instance->getCellRef(), instance->getViewRef());
   if(!view)
      return false;

   CEdif200Symbol* symbol = view->findSymbol(instance->getSymbolRef());
   if(!symbol)
      return false;

   int keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);  
   POSITION symPos = symbol->getSymbolPortStartPosition();
   while(symPos)
   {
      CEdif200SymbolPort* symbolPort = symbol->getNextSymbolPort(symPos);         
      DataStruct *PortData = symbolPort->getPortDataStruct();

      if(PortData && (PortData->getDataType() == dataTypeInsert) && 
         (PortData->getInsert()->getInsertType() == insertTypePortInstance))
      {
         Attrib *portAttrib = NULL;
         CString targetPortName = symbolPort->getTargetPortName();
         if (PortData->getAttributesRef() && PortData->getAttributesRef()->Lookup(keyword, portAttrib) && 
            !targetPortName.IsEmpty())
		   {									                              
            portAttrib->setValueFromString(targetPortName);
		   }//if Copy
      }

   }//while

   return true;      
}

/******************************************************************************
* doInstance

   This function will do the following commands:
   - viewRef
   - transform
   - portInstance
   - designator
   - property

   Skip the following commands:
   - viewList
   - parameterAssign
   - timing
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int doInstance(CEdif200& edif, CEdif200Instance** instance) // DataStruct* data)
{
   int res = 0;
   int parenthesisCount = 0;

   CString instanceName = "";
   CString renameString = "";
	CString arrayName = "";
   Display display;
	int lbIndex = 0;
	int ubIndex = 0;
	int size = -1;

	if ((res = GetArray(instanceName, renameString, display, arrayName, lbIndex, ubIndex, size)) < 0)
	{
		return res;
	}
	else if (res == 0)
	{
		// Not an array so get net name
		if ((res = GetNameDef(instanceName, renameString, display)) < 0)
			return res;
	}

   if (instanceName.IsEmpty() && !arrayName.IsEmpty())
      instanceName = arrayName;

   //if ((res = GetNameDef(instanceName, renameString, display)) < 0)
   //   return res;

   Transform transform;
   CEdif200Instance* newInstance = new CEdif200Instance(edif, edif.getCurrentLibrary()->getName(), edif.getCurrentCell()->getName(),
					edif.getCurrentView()->getName(), instanceName);

	CAttributes* attribMap = new CAttributes();
   sEdifAttrib->AddDefinedAttrib(&attribMap, SCH_ATT_INSTANCENAME, instanceName, ATTRIB_CHANGE_VALUE, &display, NULL);

   while (TRUE)
   {
      if (!sParser->GetToken())
      {
         FreeAttribMap(&attribMap);
         return sParser->ParsingErr();
      }

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "viewList") || !STRCMPI(sParser->token, "parameterAssign") ||
               !STRCMPI(sParser->token, "timing") || !STRCMPI(sParser->token, "comment") ||
               !STRCMPI(sParser->token, "userData")) 
      {
         if ((res = sParser->SkipCommand()) < 0)
         {
            FreeAttribMap(&attribMap);
            return res;
         }
      }
      else if (!STRCMPI(sParser->token, "viewRef"))
      {
			CString viewRef, cellRef, libraryRef;
         if ((res = doViewRef(viewRef, cellRef, libraryRef)) < 0)
         {
            FreeAttribMap(&attribMap);
            return res;
         }
	
			newInstance->setViewRef(viewRef);
			newInstance->setCellRef(cellRef);
			newInstance->setLibraryRef(libraryRef);
         newInstance->setSymbolRef(viewRef); // Since Edif200 doesn't have symbol name so we use view name as symbol name
		}
      else if (!STRCMPI(sParser->token, "transform"))
      {
         if ((res = transform.DoTransform()) < 0)
         {
            FreeAttribMap(&attribMap);
            return res;
         }
      }
      else if (!STRCMPI(sParser->token, "portInstance"))
      {
         CAttributes* attribMap = new CAttributes();
         Port port;
         if ((res = port.DoPort(&attribMap)) < 0)
         {
            FreeAttribMap(&attribMap);
            return res;
         }

         if (port.portName.IsEmpty())
         {
            FreeAttribMap(&attribMap);            
         }
         else
         {
            void* voidPtr = NULL;
            if (newInstance->portAttribMap.Lookup(port.portName, voidPtr))
            {
               CAttributes* existingAttribMap = (CAttributes*)voidPtr;
               CopyAttribMap(&existingAttribMap, attribMap, true);
               FreeAttribMap(&attribMap);            
            }
            else
            {
			      newInstance->portAttribMap.SetAt(port.portName, attribMap);
            }
         }
      }
      else if (!STRCMPI(sParser->token, "designator"))
      {
         CString designatorRenameString, designator;
         Display display;
         if ((res = GetNameDef(designator, designatorRenameString, display)) < 0)
         {
            FreeAttribMap(&attribMap);
            return res;
         }

         sEdifAttrib->AddDefinedAttrib(&attribMap, SCH_ATT_DESIGNATOR, designator, ATTRIB_CHANGE_VALUE, &display, NULL);
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
         if ((res = GetProperty(&attribMap)) < 0)
         {
            FreeAttribMap(&attribMap);
            return res;
         }
      }
   }

   DataStruct* data = NULL;
   if (!transform.isEmpty)
   {
		data = InsertSchematicSymbol(edif, *newInstance, transform);

      if (data != NULL && attribMap != NULL)
      {
         POSITION attribPos = attribMap->GetStartPosition();
         while (attribPos)
         {
				WORD word;
				Attrib *attrib;
            attribMap->GetNextAssoc(attribPos, word, attrib);
            if (attrib && (attrib->getFlags() & ATTRIB_CHANGE_LOCATION))
               AttribUntransform(attrib, transform);
         }

         CopyAttribMap(&data->getAttributesRef(), attribMap);
		}
      
      //Rename the Portname
      doRenamePortName(edif, newInstance);
   }

   // Clean up the allocated memory
   FreeAttribMap(&attribMap);

	if (instance != NULL)
	{
      if (newInstance != NULL && data != NULL)
      {
         CString designator = getValueFromDesignatorAttrib(data->getAttributesRef());
         newInstance->setDesignator(designator);
         newInstance->setPrimaryName(renameString);
      }

		*instance = newInstance;
	}
	else
	{
		delete newInstance;
		newInstance = NULL;
	}

   return 1;
}

static void LookupTargetPortName(DataStruct *fromData, DataStruct *copyData, CString &targetPortName)
{
   targetPortName.Empty();

   if(!fromData || !copyData)
      return;

   Attrib *attrib = NULL;
	int keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);

	if ( fromData->getAttributesRef() && fromData->getAttributesRef()->Lookup(keyword, attrib))
	{   
		CString *renamePtr = NULL;                     
		if (sPortRenameMap.Lookup(attrib, (void*&)renamePtr))
		{
			Attrib *copyAttrib = NULL;
			if (renamePtr != NULL && copyData->getAttributesRef()->Lookup(keyword, copyAttrib))
			{									                              
				targetPortName = *renamePtr;			
         }//if renamePtr

		}//if sPortRenameMap

	}//if
}

static DataStruct *InsertSchematicSymbol(CEdif200& edif, CEdif200Instance& instance, Transform transform)
{
   if (transform.isEmpty)
   {
      ErrorMessage("Trying to insert a schematic gate without X and Y location", "Syntax Error");
      return NULL;
   }


	CEdif200View* view = edif.findView(instance.getLibraryRef(), instance.getCellRef(), instance.getViewRef());
	if (view == NULL)
	{
      fprintf(sFileErr, "Syntax Error : instance \"%s\" reference an undefined schematic view \"%s\".  Error is close to line %ld\n",
            instance.getName(), instance.getViewRef(), sParser->lineCount);
      sDisplayErr++;
		return NULL;
	}

	CEdif200Symbol* symbol = view->findSymbol(instance.getSymbolRef());
   if (symbol == NULL)
   {
      DataStruct* data = NULL;
      CString instanceContentName;
      instanceContentName.Format("%s%c%s%c%s", instance.getLibraryRef(), SCH_DELIMITER, instance.getCellRef(), SCH_DELIMITER, instance.getViewRef());
      instanceContentName.MakeUpper();
      if (view->getContentBlock() != NULL && view->getContentBlock()->getName().CompareNoCase(instanceContentName) == 0)
      {
	      // if scaleX and scaleY are the same, then just use either one
         double scale = 1;
         if (fabs(transform.scaleX) / fabs(transform.scaleY) == 1) 
            scale = fabs(transform.scaleX);

	      data = Graph_Block_Reference(view->getContentBlock()->getName(), instance.getName(), sFileStruct->getFileNumber(),
							      transform.x, transform.y, DegToRad(transform.rotation), transform.mirror, scale, sSymbolGrpLayer, TRUE);
         
	      instance.setSymbolRef("");
	      instance.setInstanceDataStruct(data);
      }

      return data;
   }      
   else if (symbol->getSymbolBlock() == NULL)
   {
      fprintf(sFileErr, "Syntax Error : instance \"%s\" reference an undefined schematic symbol \"%s\".  Error is close to line %ld\n",
            instance.getName(), instance.getSymbolBlockName(), sParser->lineCount);
      sDisplayErr++;
      return NULL;
   }


   // If there are changes in attribute values or location on the port then copy the schematic symbol,
   // make the changes to it and insert the copied schematic symbol.
   // Do the same if scaleY of the insert is negative because we want to rotate the attributes on the port
   // so it looks right or we are duplicating the geometry and use a work around

	BlockStruct *block = symbol->getSymbolBlock();
	BlockStruct* newBlock = block;	// initialize to the same pointer as block
	CEdif200Symbol* newSymbol = symbol;
   if (instance.portAttribMap.GetCount() != 0)
   {
		view = edif.findView(symbol->getLibraryName(), symbol->getCellName(), symbol->getViewName());
		
      int count = 0;
		CString newSymbolName;
		newSymbolName.Format("%s_%d", symbol->getName(), ++count);
		while (view->findSymbol(newSymbolName))
		{
			newSymbolName.Format("%s_%d", symbol->getName(), ++count);
			newSymbolName.MakeUpper();
		}

		newSymbol = view->addSymbol(newSymbolName);

      // Change newBlock to point to a new geometry
      newBlock = Graph_Block_On(GBO_APPEND, newSymbol->generateSymbolBlockName(), sFileStruct->getFileNumber(), 0L);
      newBlock->setBlockType(block->getBlockType());
      Graph_Block_Off();
      sDoc->CopyAttribs(&newBlock->getAttributesRef(), block->getAttributesRef());
   
      newSymbol->setSymbolBlock(newBlock);
      if(newBlock->getAttributes() && sAttribRotationLimit_0_90)
      {
         POSITION attribPos = newBlock->getAttributes()->GetStartPosition();
         while (attribPos)
         {
            WORD word;
            Attrib *attrib;
            newBlock->getAttributes()->GetNextAssoc(attribPos, word, attrib);
            if (attrib && (attrib->getFlags() & ATTRIB_CHANGE_LOCATION))
            {
               // Limit attrib rotation to "readable" angles, 0 and 90
               int totalRotDeg = round(normalizeDegrees( attrib->getRotationDegrees() + transform.rotation ));
               if (totalRotDeg > 90)
                  totalRotDeg  -= 180;  // Limit to 0 and 90 rotations

               // Default, assume top
               double attrRotDeg = normalizeDegrees( (double)totalRotDeg - transform.rotation );

               // Bottom / Mirrored
               // Limit to 0 and 90 for mirrored insert
               if (transform.mirror & MIRROR_FLIP)
               {
                  attrRotDeg = normalizeDegrees( (double)totalRotDeg + transform.rotation );

                  int transformRotDeg = round(transform.rotation);

                  if (transformRotDeg == 90 || transformRotDeg == 270)
                  {
                     int tempDeg = round(  attrRotDeg );

                     if (tempDeg == 0 || tempDeg == 180)
                        attrRotDeg += 180.;
                  }
                  else if (transformRotDeg == 0 || transformRotDeg == 180)
                  {
                     int tempDeg = round( attrRotDeg );

                     if (tempDeg == 90 || tempDeg == 270)
                        attrRotDeg += 180.;
                  }
               }
               attrib->setRotationDegrees(attrRotDeg);
            }
         }
      }
      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *data = block->getDataList().GetNext(pos);
         DataStruct *copy = CopyTransposeEntity(data, 0.0, 0.0, 0.0, 0, 1, 0, TRUE);
         newBlock->getDataList().AddTail(copy);
         CString targetPortName = "";

         if (copy->getDataType() == dataTypeInsert && copy->getInsert()->getInsertType() == insertTypePortInstance)
         {
            if (!copy->getAttributesRef())
               continue;

            CString portName = "";
            int keyword = sDoc->IsKeyWord(SCH_ATT_PORTNAME, 0);
            Attrib *attrib = NULL;
            if (copy->getAttributesRef()->Lookup(keyword, attrib))
            {
#if CamCadMajorMinorVersion > 406  //  > 4.6
               portName = attrib->getStringValue();
#else
					portName = sDoc->ValueArray[attrib->getStringValueIndex()];
#endif

					// if there are changes in attribute values on the port, change the value
					void* voidMapPtr = NULL;
					if (instance.portAttribMap.Lookup(portName, voidMapPtr))
					{
						CAttributes* attribMap = (CAttributes*)voidMapPtr;
						if (attribMap) 
						{
							WORD word;
							Attrib* attrib;
							
							for (POSITION attribPos = attribMap->GetStartPosition();attribPos != NULL;)
							{
								attribMap->GetNextAssoc(attribPos, word, attrib);

								if (attrib != NULL && (attrib->getFlags() & ATTRIB_CHANGE_LOCATION))
								{
									AttribUntransform(attrib, transform);

									Transform portInsertTransform;
									portInsertTransform.scaleX = copy->getInsert()->getScale();
									portInsertTransform.scaleY = copy->getInsert()->getScale();
									// dts0100588780 - The change is for sAttribRotationLimit_0_90 = true
                           // dts0100625860 - Does not need to do the following step when sAttribRotationLimit_0_90 = false
                           if(sAttribRotationLimit_0_90)
                           {
									   if(copy->getInsert()->getMirrorFlags() && transform.mirror)
									      portInsertTransform.mirror = 0;
									   else if(copy->getInsert()->getMirrorFlags() || transform.mirror)
									      portInsertTransform.mirror = 1;
									   else
									      portInsertTransform.mirror = 0;
									   portInsertTransform.rotation = RadToDeg(copy->getInsert()->getAngle()) + transform.rotation;
                           }
									portInsertTransform.x = copy->getInsert()->getOriginX();
									portInsertTransform.y = copy->getInsert()->getOriginY();
									portInsertTransform.isEmpty = FALSE;

									AttribUntransform(attrib, portInsertTransform);
								}
							}
						}

						CopyAttribMap(&copy->getAttributesRef(), attribMap, true);
						//Search sPortRenameMap to find target portname
						LookupTargetPortName(data,copy,targetPortName);
					}               
            }


				CEdif200SymbolPort* symbolPort = newSymbol->addPort(copy->getInsert()->getRefname());
				symbolPort->setPortRef(portName);
				symbolPort->setPortDataStruct(copy);
				symbolPort->setTargetPortName(targetPortName);
         }
         else if (copy->getDataType() == dataTypeText)
         {
            if (sAttribRotationLimit_0_90)  // using attrib control, might want to make a separate text control
            {
               // Limit attrib rotation to "readable" angles, 0 and 90
               TextStruct *txt = copy->getText();

               int totalRotDeg = round(normalizeDegrees( txt->getRotationDegrees() + transform.rotation ));
               if (totalRotDeg > 90)
                  totalRotDeg  -= 180;  // Limit to 0 and 90 rotations

               // Default, assume top
               double textRotDeg = normalizeDegrees( (double)totalRotDeg - transform.rotation );

               // Bottom / Mirrored
               // Limit to 0 and 90 for mirrored insert
               if (transform.mirror & MIRROR_FLIP)
               {
                  textRotDeg = normalizeDegrees( (double)totalRotDeg + transform.rotation );

                  int transformRotDeg = round(transform.rotation);

                  if (transformRotDeg == 90 || transformRotDeg == 270)
                  {
                     int tempDeg = round(  textRotDeg );

                     if (tempDeg == 0 || tempDeg == 180)
                        textRotDeg += 180.;
                  }
                  else if (transformRotDeg == 0 || transformRotDeg == 180)
                  {
                     int tempDeg = round( textRotDeg );

                     if (tempDeg == 90 || tempDeg == 270)
                        textRotDeg += 180.;
                  }
               }

               copy->getText()->setRotationDegrees(textRotDeg);
            }

         }
      }
   }

	// if scaleX and scaleY are the same, then just use either one
   double scale = 1;

   if (fabs(transform.scaleX) / fabs(transform.scaleY) == 1) 
      scale = fabs(transform.scaleX);

	DataStruct *data = Graph_Block_Reference(newBlock->getName(), instance.getName(), sFileStruct->getFileNumber(),
							 transform.x, transform.y, DegToRad(transform.rotation), transform.mirror, scale, sSymbolGrpLayer, TRUE);

   if (data != NULL)
   {
      CEdif200Cell* cell = edif.findCell(instance.getLibraryRef(), instance.getCellRef());

      if (cell != NULL)
      {
         if (cell->getCellType().CompareNoCase(SCH_CELLTYPE_TIE) == 0)
         {
            data->getInsert()->setInsertType(insertTypeTieDot);
         }
         else if (cell->getCellType().CompareNoCase(SCH_CELLTYPE_RIPPER) == 0)
         {
            data->getInsert()->setInsertType(insertTypeRipper);
         }
         else
         {
            CEdif200View* view = cell->findView(instance.getViewRef());

            if (view != NULL)
            {
               // Check if view is hierarchy symbol or just regular symbol
               if (view->isHierarchyStructure())
               {
                  data->getInsert()->setInsertType(insertTypeHierarchicalSymbol);
               }
               else
               {
                  data->getInsert()->setInsertType(insertTypeSymbol);
               }
            }
         }
      }
	}

	instance.setSymbolRef(newSymbol->getName());
	instance.setInstanceDataStruct(data);

   // Update designator of instancePort from port insert in newBlock
   for (CDataListIterator dataListIterator(*newBlock, insertTypePortInstance); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
      CString portName = getValueFromPortNameAttrib(data->getAttributesRef());
      CString designator = getValueFromDesignatorAttrib(data->getAttributesRef());
      
      CEdif200InstancePort* instancePort = instance.addInstancePort(portName);
      instancePort->setDesignator(designator);
   }

   return data;
}

/******************************************************************************
* doViewRef

   This function will do the following commands:
   - cellRef

   Return code:
    1 - no error
   -1 - syntax error
*/
int doViewRef(CString& viewRef, CString& cellRef, CString& libraryRef)
{
   int res = 0;
   int parenthesisCount = 0;

   if (sEdif200.getCurrentView() != NULL)
      viewRef = sEdif200.getCurrentView()->getName();
   if (sEdif200.getCurrentCell() != NULL)
      cellRef = sEdif200.getCurrentCell()->getName();
   if (sEdif200.getCurrentLibrary() != NULL)
      libraryRef = sEdif200.getCurrentLibrary()->getName();

   CString renameString = "";
   Display display;
   if ((res = GetNameDef(viewRef, renameString, display)) < 0)
      return res;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "cellRef"))
      {
         if ((res = doCellRef(cellRef, libraryRef)) < 0)
            return res;
      }
   }
   return 1;
}

///******************************************************************************
//* doCellRef
//
//   This function will do the following commands:
//   - libraryRef
//
//   Return code:
//    1 - no error
//   -1 - syntax error
//*/
static int doCellRef(CString &cellRef, CString &libraryRef)
{
   int res = 0;
   int parenthesisCount = 0;

   if (sEdif200.getCurrentCell() != NULL)
      cellRef = sEdif200.getCurrentCell()->getName();
   if (sEdif200.getCurrentLibrary() != NULL)
      libraryRef = sEdif200.getCurrentLibrary()->getName();

   CString renameString = "";
   Display display;
   if ((res = GetNameDef(cellRef, renameString, display)) < 0)
      return res;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "libraryRef"))
      {
         if ((res = doLibraryRef(libraryRef)) < 0)
            return res;
      }
   }

   return 1;
}

///******************************************************************************
//* doLibraryRef
//
//   Return code:
//    1 - no error
//   -1 - syntax error
//*/
static int doLibraryRef(CString& libraryRef)
{
   int res = 0;

   CString renameString = "";
   Display display;
   if ((res = GetNameDef(libraryRef, renameString, display)) < 0)
      return res;

   return 1;
}

//-----------------------------------------------------------------------------
// CEdif200Page
//-----------------------------------------------------------------------------
CEdif200Page::CEdif200Page(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString name, const CString generatedName)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
   , m_viewName(viewName)
	, m_name(name)
   , m_generatedName(generatedName)
	, m_offPageConnectorMap(nextPrime2n(20), true)
	, m_instanceMap(nextPrime2n(20),true)
	, m_netMap(nextPrime2n(20),true)
	, m_busMap(nextPrime2n(20),true)
   , m_globalPortMap(nextPrime2n(20), true)
	, m_masterPortMap(nextPrime2n(20), true)
{
   reset();
}

CEdif200Page::CEdif200Page(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString name, const CString generatedName, CEdif200Page& page)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
   , m_viewName(viewName)
	, m_name(name)
   , m_generatedName(generatedName)
	, m_offPageConnectorMap(nextPrime2n(20), true)
	, m_instanceMap(nextPrime2n(20),true)
	, m_netMap(nextPrime2n(20),true)
	, m_busMap(nextPrime2n(20),true)
   , m_globalPortMap(nextPrime2n(20), true)
	, m_masterPortMap(nextPrime2n(20), true)
{
   reset();
   
   m_goodHierarchySheet = page.m_goodHierarchySheet;

   POSITION pos;
   for (pos = page.getNetStartPosition(); pos != NULL;)
   {
      CEdif200Net* net =  page.getNextNet(pos);
      if (net !=  NULL)
      {
         CEdif200Net* copy = new CEdif200Net(m_edif, m_libraryName, m_cellName, m_viewName, m_name, *net);
         m_netMap.SetAt(copy->getName(), copy);
      }
   }

   for (pos = page.getBusStartPosition(); pos != NULL;)
   {
      CEdif200Bus* bus =  page.getNextBus(pos);
      if (bus !=  NULL)
      {
         CEdif200Bus* copy = new CEdif200Bus(m_edif, m_libraryName, m_cellName, m_viewName, *this, *bus);
         m_busMap.SetAt(copy->getName(), copy);
      }
   }

   for (pos = page.getOffPageConnectorStartPosition(); pos != NULL;)
   {
      CEdif200PageConnector* pageConnector = page.getNextOffPageConnector(pos);
      if (pageConnector != NULL)
      {
         CEdif200PageConnector* copy =  addOffPageConnector(pageConnector->getName());
         copy->setDesignator(pageConnector->getDesignator());

         CEdif200Signal* connectedToSignal = pageConnector->getConnectedToSignal();
         if (connectedToSignal != NULL)
         {
            if (connectedToSignal->getSignalType() == signalTypeNet)
            {
               copy->setConnectedToSignal(findNet(connectedToSignal->getName()));
            }
            else
            {
               copy->setConnectedToSignal(findBus(connectedToSignal->getName()));
            }
         }
      }
   }

   for (pos = page.getMasterPortStartPosition(); pos != NULL;)
   {
      CEdif200Port* masterPort = page.getNextMasterPort(pos);
      if (masterPort != NULL)
      {
         CEdif200Port* copy = addMasterPort(masterPort->getName());
         copy->setPortRef(masterPort->getPortRef());
         copy->setDesignator(masterPort->getDesignator());

         CEdif200Signal* connectedToSignal = masterPort->getConnectedToSignal();
         if (connectedToSignal != NULL)
         {
            if (connectedToSignal->getSignalType() == signalTypeNet)
            {
               copy->setConnectedToSignal(findNet(connectedToSignal->getName()));
            }
            else
            {
               copy->setConnectedToSignal(findBus(connectedToSignal->getName()));
            }
         }
      }
   }

   for (pos = page.getInstanceStartPosition(); pos != NULL;)
   {
      CEdif200Instance* instance = page.getNextInstance(pos);
      if (instance != NULL)
      {
         CEdif200Instance* copy = new CEdif200Instance(m_edif, m_libraryName, m_cellName, m_viewName, instance->getName());
         m_instanceMap.SetAt(copy->getName(), copy);
         copy->setLibraryRef(instance->getLibraryRef());
         copy->setCellRef(instance->getCellRef());
         copy->setViewRef(instance->getViewRef());
         copy->setSymbolRef(instance->getSymbolRef());
         copy->setDisplayName(instance->getDisplayName());
         copy->setDesignator(instance->getDesignator());
         copy->setPrimaryName(instance->getPrimaryName());

         for (POSITION instancePortPos = instance->getInstancePortStartPosition(); instancePortPos != NULL;)
         {
            CEdif200InstancePort* instancePort = instance->getNextInstancePort(instancePortPos);
            if (instancePort != NULL)
            {
               CEdif200InstancePort* copyInstancePort = copy->addInstancePort(instancePort->getName());
               copyInstancePort->setDesignator(instancePort->getDesignator());
               copyInstancePort->setPrimaryName(instancePort->getPrimaryName());

               CEdif200Signal* connectedToSignal = instancePort->getConnectedToSignal();
               if (connectedToSignal != NULL)
               {
                  if (connectedToSignal->getSignalType() == signalTypeNet)
                  {
                     copyInstancePort->setConnectedToSignal(findNet(connectedToSignal->getName()));
                  }
                  else
                  {
                     copyInstancePort->setConnectedToSignal(findBus(connectedToSignal->getName()));
                  }
               }
            }
         }
      }
   }

   m_pageBlock = Graph_Block_On(GBO_APPEND, generatePageBlockName(), sFileStruct->getFileNumber(), 0L, blockTypeSheet);
   Graph_Block_Copy(page.getPageBlock(), 0.0, 0.0, 0.0, 0, 1.0, 0, TRUE);
   Graph_Block_Off();

   for (CDataListIterator dataList(*m_pageBlock, dataTypeInsert); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();
      InsertStruct* insert = data->getInsert();

      switch (insert->getInsertType())
      {
      case insertTypePortInstance:
         {
            CEdif200Port* masterPort = findMasterPort(insert->getRefname());
            if (masterPort != NULL)
            {
               masterPort->setPortDataStruct(data);
            }
         }
         break;

      case insertTypeSymbol:
      case insertTypeHierarchicalSymbol:
         {
            CEdif200Instance* instance = findInstance(insert->getRefname());
            if (instance != NULL)
            {
               instance->setInstanceDataStruct(data);
            }
         }
         break;

      case insertTypeSheetConnector:
         {
            CEdif200PageConnector* pageConnector = findOffPageConnector(insert->getRefname());
            if (pageConnector != NULL)
            {
               pageConnector->setConnectorDataStruct(data);
            }
         }
         break;

      default:
         break;
      }
   }
}

CEdif200Page::~CEdif200Page()
{
   reset();
}

void CEdif200Page::reset()
{
   m_goodHierarchySheet = true;

   m_offPageConnectorMap.empty();
	m_instanceMap.empty();
	m_netMap.empty();
	m_busMap.empty();
   m_globalPortMap.empty();
	m_masterPortMap.empty();

   m_pageBlock = NULL;
}

CString CEdif200Page::generatePageBlockName()
{
   CString blockName = "";
   blockName.Format("%s%c%s%c%s%c%s", m_libraryName, SCH_DELIMITER, m_cellName, SCH_DELIMITER, m_viewName, SCH_DELIMITER, m_generatedName);
   blockName.MakeUpper();
   return blockName;
}

CEdif200PageConnector* CEdif200Page::addOffPageConnector(const CString connectorName)
{
	CEdif200PageConnector* connector = findOffPageConnector(connectorName);
	if (connector == NULL)
	{
		connector = new CEdif200PageConnector(m_edif, m_libraryName, m_cellName, m_viewName, m_name, connectorName, connectorTypeOffPageConnecotor);
		m_offPageConnectorMap.SetAt(connectorName, connector);
	}
	return connector;
}

CEdif200PageConnector* CEdif200Page::findOffPageConnector(const CString connectorName)
{
	CEdif200PageConnector* connector = NULL;
	m_offPageConnectorMap.Lookup(connectorName, connector);
	return connector;
}

CEdif200Instance* CEdif200Page::findInstance(const CString instanceName)
{
	CEdif200Instance* instance = NULL;
	m_instanceMap.Lookup(instanceName, instance);
	return instance;
}

CEdif200Net* CEdif200Page::addNet(const CString netName)
{
	CEdif200Net* net = findNet(netName);
	if (net == NULL)
	{
		net = new CEdif200Net(m_edif, m_libraryName, m_cellName, m_viewName, m_name, netName);
		m_netMap.SetAt(netName, net);
	}
	return net;
}

CEdif200Net* CEdif200Page::findNet(const CString netName)
{
	CEdif200Net* net = NULL;
	m_netMap.Lookup(netName, net);
	return net;
}

CEdif200Bus* CEdif200Page::addBus(const CString busName)
{
	CEdif200Bus* bus = findBus(busName);
	if (bus == NULL)
	{
		bus = new CEdif200Bus(m_edif, m_libraryName, m_cellName, m_viewName, m_name, busName);
		m_busMap.SetAt(busName, bus);
	}
	return bus;
}

CEdif200Bus* CEdif200Page::findBus(const CString busName)
{
	CEdif200Bus* bus = NULL;
	m_busMap.Lookup(busName, bus);
	return bus;
}

CEdif200Port* CEdif200Page::addMasterPort(const CString portName)
{
   CEdif200Port* masterPort = findMasterPort(portName);
   if (masterPort == NULL)
   {
      masterPort = new CEdif200Port(m_edif, m_libraryName, m_cellName, m_viewName, m_name, portName, connectorTypeMasterPort);
      m_masterPortMap.SetAt(portName, masterPort);
   }
   return masterPort;
}

CEdif200Port* CEdif200Page::findMasterPort(const CString portName)
{
   CEdif200Port* masterPort = NULL;
   m_masterPortMap.Lookup(portName, masterPort);
   return masterPort;
}

CEdif200Port* CEdif200Page::addGlobalPort(const CString portName)
{
   CEdif200Port* globalPort = findMasterPort(portName);
   if (globalPort == NULL)
   {
      globalPort = new CEdif200Port(m_edif, m_libraryName, m_cellName, m_viewName, m_name, portName, connectorTypeGlobalPort);
      m_globalPortMap.SetAt(portName, globalPort);
   }
   return globalPort;
}

CEdif200Port* CEdif200Page::findGlobalPort(const CString portName)
{
   CEdif200Port* globalPort = NULL;
   m_globalPortMap.Lookup(portName, globalPort);
   return globalPort;
}

void CEdif200Page::convertLogicalSchematicToPhysicalSchematic()
{
   int hierarchGeomNumKw = sDoc->RegisterKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0, VT_STRING);
   for (POSITION pos = getInstanceStartPosition(); pos != NULL;)
   {
      CEdif200Instance* instance = getNextInstance(pos);
      if (instance != NULL && instance->isHierarchSymbol())
      {
         CEdif200Cell* cell = m_edif.findCell(instance->getLibraryRef(), instance->getCellRef());
         if (cell != NULL)
         {

            CEdif200View* newCopyView = cell->copyAndAddView(instance->getViewRef());
            if (newCopyView != NULL)
            {
               CString temp = newCopyView->getDelimitedPageBlockNumber(" ");
               Attrib* attrib = NULL;
               instance->setViewRef(newCopyView->getName());
#if CamCadMajorMinorVersion > 406  //  > 4.6
               instance->getInstanceDataStruct()->setAttrib(sDoc->getCamCadData(), hierarchGeomNumKw, valueTypeString, newCopyView->getDelimitedPageBlockNumber(" ").GetBuffer(0), attributeUpdateOverwrite, &attrib);
#else
               instance->getInstanceDataStruct()->setAttrib(sDoc, hierarchGeomNumKw, valueTypeString, newCopyView->getDelimitedPageBlockNumber(" ").GetBuffer(0), attributeUpdateOverwrite, &attrib); 
#endif
               attrib->setInherited(false);
               newCopyView->convertLogicalSchematicToPhysicalSchematic();
            }
         }
      }
   }
}

void CEdif200Page::createNetList()
{
   // *** Important note:
	//	1) In this function, we also assume all annotation is already done!
	//	2) Function calls MUST be in the order implemented

	prepareBusAndNetForRename();

   // 1) Apply value of attribute "PortName" as netName to all nets and busses connected to global ports and mark rename as DONE
   applyGlobalPortNameToBussesAndNets();

	// 2) For all nets and busses connected to page connector,  mark rename as DONE
	applyDoneToBussesAndNetsOnPageConnector();
 
   // 3) Propagate netname to hierarchy structure of the connected hierarchy symbol
	applyNetsOnInstanceToHierarchyStructure();

	// 4) Create net list from renamed nets
	createNetListFromRenamedNetsAndBusses();

	// 5) Apply renamed net and bus name to etches
	applyRenamedNetAndBusNameToEtch();
}

void CEdif200Page::prepareBusAndNetForRename()
{
   // Prepare every net and bus for net rename
   for (POSITION pos =getNetStartPosition(); pos != NULL;)
   {
      CEdif200Net* net = getNextNet(pos);
      if (net != NULL)
         net->prepareForRename();
   }

   for (POSITION pos = getBusStartPosition(); pos != NULL;)
   {
      CEdif200Bus* bus = getNextBus(pos);
      if (bus != NULL)
		   bus->prepareForRename();
	}
}

void CEdif200Page::applyGlobalPortNameToBussesAndNets()
{
   for (POSITION pos = getGlobalPortStartPosition(); pos != NULL;)
   {
      CEdif200Port* globalPort = getNextGlobalPort(pos);
      if (globalPort != NULL && globalPort->getConnectedToSignal() != NULL)
      {
         globalPort->getConnectedToSignal()->setRenamedName(globalPort->getPortRef(), renamedByGlobalPort);
      }
   }
}

void CEdif200Page::applyDoneToBussesAndNetsOnPageConnector()
{
   for (POSITION pos = getOffPageConnectorStartPosition(); pos != NULL;)
   {
      CEdif200PageConnector* offPageConnector = getNextOffPageConnector(pos);
      if (offPageConnector != NULL && offPageConnector->getConnectedToSignal() != NULL)
      {
         offPageConnector->getConnectedToSignal()->setRenamedName(offPageConnector->getConnectedToSignal()->getRenamedName(), renamedByOffPageConnector);
      }
   }
}

void CEdif200Page::applyNetsOnInstanceToHierarchyStructure()
{
   for (POSITION instancePos = getInstanceStartPosition(); instancePos != NULL;)
   {
      CEdif200Instance* instance = getNextInstance(instancePos);
      if (instance == NULL || !instance->isHierarchSymbol())
         continue;

      CEdif200View* view = m_edif.findView(instance->getLibraryRef(), instance->getCellRef(), instance->getViewRef());
      if (view != NULL)
		{
			view->propagateNetsFromInstanceToHierarchyStructure(instance);
		}
   }
}

void CEdif200Page::createNetListFromRenamedNetsAndBusses()
{
   int compKey = sDoc->RegisterKeyWord(SCH_ATT_COMPDESIGNATOR, 0, VT_STRING);
   int pinKey = sDoc->RegisterKeyWord(SCH_ATT_PINDESIGNATOR, 0, VT_STRING);

	for (POSITION instancePos = getInstanceStartPosition(); instancePos != NULL;)
   {
      CEdif200Instance* instance = getNextInstance(instancePos);
      if (instance == NULL || instance->isHierarchSymbol())
         continue;

	   //Need to make sure that is an instance insert because sometime there
      //can be an instance without an actuall insert, example case #1885
		DataStruct* instanceData = instance->getInstanceDataStruct();	
		if (instanceData == NULL || instanceData->getInsert()->getInsertType() != insertTypeSymbol)
			continue;

		CEdif200View* view = m_edif.findView(instance->getLibraryRef(), instance->getCellRef(), instance->getViewRef());
		CEdif200Symbol* symbol = view->findSymbol(instance->getSymbolRef());
      if (symbol == NULL)
         continue;

		// insert of symbol instance
		CTMatrix matrix = instanceData->getInsert()->getTMatrix();
		for (POSITION symbolPortPos = symbol->getSymbolPortStartPosition(); symbolPortPos != NULL;)
		{
			CEdif200SymbolPort* symbolPort = symbol->getNextSymbolPort(symbolPortPos);
			if (symbolPort == NULL || symbolPort->getPortDataStruct() == NULL)
				continue;

			CEdif200InstancePort* instancePort = instance->findInstancePort(symbolPort->getPortRef());
			if (instancePort == NULL || instancePort->getConnectedToSignal() == NULL)
				continue;

			// insert of port inside symbol block
			DataStruct* symbolPortData = symbolPort->getPortDataStruct();
			CPoint2d pinLocation = symbolPortData->getInsert()->getOrigin2d();
			matrix.transform(pinLocation);

			NetStruct* netStruct = add_net(sFileStruct, instancePort->getConnectedToSignal()->getRenamedName());
         if (instancePort->getConnectedToSignal()->getAttributesRef() != NULL)
         {
      		sDoc->CopyAttribs(&netStruct->getAttributesRef(), instancePort->getConnectedToSignal()->getAttributesRef());
         }
			CompPinStruct* compPin = add_comppin(sFileStruct, netStruct, instanceData->getInsert()->getRefname(), symbolPortData->getInsert()->getRefname());
         if (compPin != NULL)
         {
            compPin->setOrigin(pinLocation);
#if CamCadMajorMinorVersion > 406  //  > 4.6
            compPin->setAttrib(sDoc->getCamCadData(), compKey, valueTypeString, instance->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
            compPin->setAttrib(sDoc->getCamCadData(), pinKey, valueTypeString, instancePort->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
            compPin->setAttrib(sDoc, compKey, valueTypeString, instance->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
            compPin->setAttrib(sDoc, pinKey, valueTypeString, instancePort->getDesignator().GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
         }
      }
	}

   for (POSITION pos = getBusStartPosition(); pos != NULL;)
   {
      CEdif200Bus* bus = getNextBus(pos);
      if (bus != NULL)
      {
         bus->createNetLis();
      }
   }
}

void CEdif200Page::applyRenamedNetAndBusNameToEtch()
{
   if (m_pageBlock == NULL)
      return;

	WORD netNameKw = sDoc->getStandardAttributeKeywordIndex(standardAttributeNetName);
	for (CDataListIterator dataList(*m_pageBlock, dataTypePoly); dataList.hasNext();)
	{
		DataStruct* polyData = dataList.getNext();
		Attrib* attrib = NULL;
		if (polyData->getAttributes() == NULL || !polyData->getAttributes()->Lookup(netNameKw, attrib))
			continue;

#if CamCadMajorMinorVersion > 406  //  > 4.6
      CString netName = attrib->getStringValue();
#else
		CString netName = sDoc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
		CEdif200Net* net = findNet(netName);
		if (net != NULL)
		{
#if CamCadMajorMinorVersion > 406  //  > 4.6
         polyData->setAttrib(sDoc->getCamCadData(), (int)netNameKw, valueTypeString, net->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
			polyData->setAttrib(sDoc, (int)netNameKw, valueTypeString, net->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif         
         CopyAttribMap(&polyData->getAttributesRef(), net->getAttributesRef());
      }
		else
		{
			CEdif200Bus* bus = findBus(netName);
			if (bus != NULL)
         {
#if CamCadMajorMinorVersion > 406  //  > 4.6
            polyData->setAttrib(sDoc->getCamCadData(), (int)netNameKw, valueTypeString, bus->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
			   polyData->setAttrib(sDoc, (int)netNameKw, valueTypeString, bus->getRenamedName().GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
            CopyAttribMap(&polyData->getAttributesRef(), bus->getAttributesRef());
         }
		}
	}
}

void CEdif200Page::propagateNetsFromInstanceToHierarchyStructure(CEdif200Instance* instance)
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

	// 3) For all nets and busses connect to master port, inherite the net name on instance port that match
   //    the master port and mark rename as DONE
	applyMasterPortNameToBussesAndNets(instance);

   // 4) Propagate netname to hierarchy structure of the connected hierarchy symbol
	applyNetsOnInstanceToHierarchyStructure();

	// 5) Now, create net list from renamed nets
	createNetListFromRenamedNetsAndBusses();

	// 6) Apply renamed net name to etches
	applyRenamedNetAndBusNameToEtch();
}

void CEdif200Page::applyMasterPortNameToBussesAndNets(CEdif200Instance* instance)
{
	if (instance == NULL)
		return;

	for (POSITION pos = getMasterPortStartPosition(); pos != NULL;)
	{
		CEdif200Port* masterPort = getNextMasterPort(pos);
		if (masterPort == NULL || masterPort->getConnectedToSignal() == NULL)
			continue;

		CEdif200InstancePort* instancePort = instance->findInstancePort(masterPort->getPortRef());
		if (instancePort == NULL ||instancePort->getConnectedToSignal() == NULL)
			continue;

		CString beforeMasterPortRename = masterPort->getConnectedToSignal()->getRenamedName();
		masterPort->getConnectedToSignal()->setRenamedName(instancePort->getConnectedToSignal()->getRenamedName(), renamedByMasterPort);

		for (POSITION netPos = getNetStartPosition(); netPos != NULL;)
		{
			CEdif200Net* net = getNextNet(netPos);
         if (net != NULL && net->getRenamedName().CompareNoCase(beforeMasterPortRename) == 0)
         {
            net->setRenamedName(instancePort->getConnectedToSignal()->getRenamedName(), renamedByMasterPort);
         }
		}
	}
}

/******************************************************************************
* CEdif200Page::doPage

   This function will do the following commands:
   - instance
   - net
   - commentGraphics
   - portImplementation
   - pageSize

   Skip the following commands:
   - boundingBox
   - netBundle
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Page::doPage()
{
   int res = 0;
   int parenthesisCount = 0;

   // Create a geometry to represent a sheet
   m_pageBlock = Graph_Block_On(GBO_APPEND, generatePageBlockName(), sFileStruct->getFileNumber(), 0L);
   m_pageBlock->setBlockType(blockTypeSheet);

   // To indicate that this is schematic file, not a regular PCB file
   int keyword = sDoc->RegisterKeyWord("SchematicStructure", 0, VT_INTEGER);
   int value = 1;
   sDoc->SetAttrib(&m_pageBlock->getAttributesRef(), keyword, VT_INTEGER, &value, SA_OVERWRITE, NULL);

   // Only set to sCurrentBlock if not already set, it will be override late in "instance" command
   if (sFileStruct->getBlock() == NULL )
      sFileStruct->setBlock(m_pageBlock); 

   CEdif200View* view = m_edif.getCurrentView();
   EdifFigure figure;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "boundingBox") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "instance"))
      {			
         CEdif200Instance *instance = NULL;
         if ((res = doInstance(m_edif, &instance)) < 0)
            return res;

         if (instance != NULL && instance->getInstanceDataStruct() != NULL)
         {
            CEdif200Instance *existingInstance = NULL;
            if (m_instanceMap.Lookup(instance->getName(), existingInstance))
            {
               delete existingInstance;
               existingInstance = NULL;
            }
			   m_instanceMap.SetAt(instance->getName(), instance);

			   // Overwrite what is already set above and make it show.
			   // Only the first sheet with component insert will be set to show.
			   // This can be overwrite later in design()
			   if (!sFileStruct->isShown())
			   {
				   sFileStruct->setShow(true);
				   sFileStruct->setBlock(m_pageBlock);
			   }

			   if (view != NULL)
			   {
               if (view->getFirstSchematicPage() == NULL)
               {
                  view->setFirstSchematicPage(this);
               }

               if (view->getCurrentSymbol() != NULL && instance->getInstanceDataStruct()->getDataType() == dataTypeInsert)
               {
                  // Bad hierarchy sheet if the sheet is inserting its own hierarchy symbol
				      BlockStruct *instanceBlock = sDoc->getBlockAt(instance->getInstanceDataStruct()->getInsert()->getBlockNumber());
                  if (instanceBlock == view->getCurrentSymbol()->getSymbolBlock())
					      m_goodHierarchySheet = false;
               }
			   }
         }
      }
      else if (!STRCMPI(sParser->token, "net"))
      {
         if ((res = doNet()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "netBundle"))
      {
         if ((res = doNetBundle()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "commentGraphics"))
      {
         if ((res = doCommentGraphics(m_edif)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "portImplementation"))
      {
         CString portName;
         DataStruct* portData = NULL;
         if ((res = doPortImplementation(m_edif, portName, &portData, m_edif.getCurrentView()->m_portAttribMap)) < 0)
            return res;

         if (portData !=  NULL && view != NULL)
         {
            CString designator = getValueFromDesignatorAttrib(portData->getAttributesRef());

            if (view->isOffPageConnector(portName))
            {
               portData->getInsert()->setInsertType(insertTypeSheetConnector);

               CEdif200PageConnector* pageConnector = addOffPageConnector(portData->getInsert()->getRefname());
               pageConnector->setConnectorDataStruct(portData);
               pageConnector->setDesignator(designator);
            }
            else
            {
               if (view->isGlobalPort(portName))
               {
                  CEdif200Port* masterPort = addMasterPort(portData->getInsert()->getRefname());
                  masterPort->setPortDataStruct(portData);
                  masterPort->setPortRef(portName);
                  masterPort->setDesignator(designator);
               }
               else
               {
                  CEdif200Port* masterPort = addMasterPort(portData->getInsert()->getRefname());
                  masterPort->setPortDataStruct(portData);
                  masterPort->setPortRef(portName);
                  masterPort->setDesignator(designator);
               }
            }
         }
      }
      else if (!STRCMPI(sParser->token, "pageSize") )
      {
         if ((res = figure.DoFigure(sBorderGrpLayer)) < 0)
            return res;
      }
   }

   Graph_Block_Off();

	bool debugFlag = false;
	if (debugFlag)
	{
		CString fileName;
		fileName.Format("C:\\Development\\Application\\~ Case\\2159 Edif 200 Read\\%s.log", m_generatedName);
		CMessageFilter messageFilter(messageFilterTypeMessage, fileName);

#if CamCadMajorMinorVersion > 406  //  > 4.6
      m_pageBlock->calculateBlockExtents(sDoc->getCamCadData(), true, &messageFilter);
#else
		m_pageBlock->calculateBlockExtents(*sDoc, true, &messageFilter);
#endif

		CExtent extent = m_pageBlock->getExtent();

		if (messageFilter.getMessageFormatCount() > 0)
		{
			messageFilter.flush();
		}
	}

   return 1;
}

/******************************************************************************
* CEdif200Page::doNet

   This function will do the following commands:
   - joined
   - figure
   - net
   - instance
   - commentGraphics
   - property

   Skip the following commands:
   - criticality
   - netDelay
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Page::doNet(CEdif200Net** BusNet, CEdif200Bus** subBus)
{
   int res = 0;
   int parenthesisCount = 0;

   CString netName = "";
   CString renameString = "";
	CString arrayName = "";
   Display display;
	int lbIndex = 0;
	int ubIndex = 0;
	int size = -1;

	// Check to see if net is defined as an array
	if ((res = GetArray(netName, renameString, display, arrayName, lbIndex, ubIndex, size)) < 0)
	{
		return res;
	}
	else if (res == 0)
	{
		// Not an array so get net name
		if ((res = GetNameDef(netName, renameString, display)) < 0)
			return res;
	}

   CString text = netName;
   if (renameString != "")
   {
      netName = renameString;
      text = renameString;
   }

	POSITION pos = display.displayList.GetHeadPosition();
	while (pos)
	{
		DisplayParam *displayParam = display.displayList.GetNext(pos);

		if (!displayParam->m_isEmpty && displayParam->m_figureGroup.m_visible)
		{
			DataStruct *textdata = Graph_Text(sTextLayer, text, displayParam->m_x, displayParam->m_y, displayParam->m_figureGroup.m_textHeight, 
					displayParam->m_figureGroup.m_textWidth, DegToRad(displayParam->m_rotation), 0L, TRUE, displayParam->m_mirror, 
					0, FALSE, displayParam->m_figureGroup.m_widthIndex, 0, displayParam->m_horizontalJust, displayParam->m_verticalJust);
         if (textdata != NULL)
            textdata->getText()->setMirrorDisabled(true);
		}
	}

	
	CEdif200Bus* bus = NULL;
   CEdif200Net* net = NULL;
	if (size > -1)
	{
		// This is a bus (net bundle) then sub net name is like this
		//		Example: netname of GIRQ[0:3] <== This format is from "cap2edi - Engineering DataXpress, Inc."
		//					bus	= GIRQ[0:3]
		//					net1	= GIRQ0
		//					net2	= GIRQ1
		//					net3	= GIRQ2
		//					net4	= GIRQ3
		//
		// or Example: netname of GIRQ(0:3) <== This format is from "esiwrite" of Mentor
		//					bus	= GIRQ(0:3)
		//					net1	= GIRQ(0)
		//					net2	= GIRQ(1)
		//					net3	= GIRQ(2)
		//					net4	= GIRQ(3)
		CString formatString = "";
		if (netName.ReverseFind('[') > -1)
			formatString = "%s%d";
		else
			formatString = "%s(%d)";

		bus = addBus(netName);
		if (lbIndex <= ubIndex)
		{
			for (int i=lbIndex; i<=ubIndex; i++)
			{
				CString subNetName = "";
				subNetName.Format(formatString, arrayName, i);

				CEdif200Net* busNet = addNet(subNetName);
				bus->addNet(busNet);
			}
		}
		else
		{
			for (int i=lbIndex; i>=ubIndex; i--)
			{
				CString subNetName = "";
				subNetName.Format(formatString, arrayName, i);

				CEdif200Net* busNet = addNet(subNetName);
				bus->addNet(busNet);
			}
		}

      bus->setPrimaryName(renameString);
      if(NULL != subBus)
         *subBus = bus;
	}
	else
	{
		// This is just a regular net      
      net = addNet(netName);
      net->setPrimaryName(renameString);
      if(NULL != BusNet)
         *BusNet = net;
	}

   EdifFigure figure;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "criticality") || !STRCMPI(sParser->token, "netDelay") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sSignalGrpLayer, true /*fill circles*/)) < 0)
            return res;

			if (figure.data != NULL)
			{
				if (net != NULL)
					figure.data->setGraphicClass(graphicClassSignal);
				else if (bus != NULL)
					figure.data->setGraphicClass(graphicClassBus);
			}
      }
      else if (!STRCMPI(sParser->token, "instance"))
      {
         if ((res = doInstance(m_edif, NULL)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "net"))
      {
         if ((res = doSubNet(figure)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "commentGraphics"))
      {
         if ((res = doCommentGraphics(m_edif)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "joined"))
      {
			if (net != NULL)
			{
				if ((res = doJoined(net)) < 0)
					return res;
			}
			else
			{
				if ((res = sParser->SkipCommand()) < 0)
					return res;
			}
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
			if (net != NULL)
			{
				if ((res = GetProperty(&net->getAttributesRef())) < 0)
					return res;
			}
			else if (bus != NULL)
			{
				if ((res = GetProperty(&bus->getAttributesRef())) < 0)
					return res;
			}
			else
			{
				if ((res = sParser->SkipCommand()) < 0)
					return res;
			}
      }
   }

   if (figure.data)
   {
      Display display;
      sEdifAttrib->AddDefinedAttrib(&figure.data->getAttributesRef(), SCH_ATT_NETNAME, netName.GetBuffer(0), ATTRIB_CHANGE_VALUE, &display, NULL);
      if (net != NULL)
      {
         CopyAttribMap(&figure.data->getAttributesRef(), net->getAttributesRef());
      }
      else if (bus != NULL)
      {
         CopyAttribMap(&figure.data->getAttributesRef(), bus->getAttributesRef());
      }
   }

   return 1;
}

/******************************************************************************
* CEdif200Page::doSubNet
   This function does almost the same thing as DoNet().  The only difference is
   that it doesn't do 'joined' and 'property' because they are being taking
   care of at DoNet().

   This function will do the following commands:
   - figure
   - net
   - instance
   - commentGraphics

   Skip the following commands:
   - joined
   - property
   - criticality
   - netDelay
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Page::doSubNet(EdifFigure figure)
{
   int res = 0;
   int parenthesisCount = 0;

   CString subNetName = "";
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(subNetName, renameString, display)) < 0)
      return res;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "criticality") || !STRCMPI(sParser->token, "netDelay") ||
               !STRCMPI(sParser->token, "joined") || !STRCMPI(sParser->token, "property") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sSignalGrpLayer)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "instance"))
      {
         if ((res = doInstance(m_edif, NULL)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "net"))
      {
         if ((res = doSubNet(figure)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "commentGraphics"))
      {
         if ((res = doCommentGraphics(m_edif)) < 0)
            return res;
      }
   }

   return 1;
}

/******************************************************************************
* CEdif200Page::doJoined
   This function add the referenced port of an instance to the NetList as 
   comp(instance) and pin(port)

   This function will do the following commands:
   - portRef

   Skip the following commands:
   - portList
   - globalPortRef

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Page::doJoined(CEdif200Net *net)
{
   int res = 0;
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "portList") || !STRCMPI(sParser->token, "globalPortRef"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "portRef"))
      {
         CString portRef = "";
         CString instanceRef = "";
         if ((res = doPortRef(portRef, instanceRef)) < 0)
            return res;

         if (!instanceRef.IsEmpty())
         {
			   CEdif200Instance* instance = findInstance(instanceRef);
            if (instance != NULL)
            {
               CEdif200InstancePort* instancePort = instance->addInstancePort(portRef);
               if (instancePort != NULL)
               {
                  instancePort->setConnectedToSignal(net);
               }
            }
         }
         else 
         {
            CEdif200Port* port = NULL;
            CEdif200PageConnector* pageConnector = NULL;
            if (m_globalPortMap.Lookup(portRef, port) && m_masterPortMap.Lookup(portRef, port))
            {
               port->setConnectedToSignal(net);
            }
            else if (m_offPageConnectorMap.Lookup(portRef, pageConnector))
            {
               pageConnector->setConnectedToSignal(net);
            }
         }
      }
   }

   return 1;
}

/******************************************************************************
* CEdif200Page::doPortRef

   This function will do the following commands:
   - instanceRef

   Skip the following commands:
   - portRef
   - viewRef

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Page::doPortRef(CString &portRef, CString &instanceRef)
{
   int res = 0;
   int parenthesisCount = 0;

   CString renameString = "";
   Display display;

	if ((res = GetNameDef(portRef, renameString, display)) < 0)
		return res;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "portRef") || !STRCMPI(sParser->token, "viewRef"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "instanceRef"))
      {
         if ((res = doInstanceRef(instanceRef)) < 0)
            return res;
      }
   }

   return 1;
}

/******************************************************************************
* CEdif200Page::doInstanceRef

   This function will do the following commands:
   - instanceNameRef

   Skip the following commands:
   - instanceRef
   - viewRef

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Page::doInstanceRef(CString &instanceRef)
{
   int res = 0;
   int parenthesisCount = 0;

   CString renameString = "";
   Display display;
   if ((res = GetNameDef(instanceRef, renameString, display)) < 0)
      return res;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "instanceRef") || !STRCMPI(sParser->token, "viewRef"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
   }

   return 1;
}

//-----------------------------------------------------------------------------
// CEdif200View
//-----------------------------------------------------------------------------
CEdif200View::CEdif200View(CEdif200 &edif, const CString libraryName, const CString cellName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_cellName(cellName)
	, m_name(name)
   , m_viewType("")
   , m_pageMap(true)
	, m_symbolMap(nextPrime2n(20),true)
   , m_portAttribMap(nextPrime2n(20),true)
{
	reset();
}

CEdif200View::~CEdif200View()
{
	reset();
}

void CEdif200View::reset()
{
   m_pageMap.empty();
	m_symbolMap.empty();
   m_portAttribMap.empty();
   m_offPageConnectorNameMap.RemoveAll();

   m_firstSchematicPage = NULL;
	m_currentSymbol = NULL;
   m_contentBlock = NULL;

	m_isHierarchyStructure = false;
	m_cloneCount = 0;
	m_derivedNumber = 0;
}

CEdif200View* CEdif200View::getCopy()
{
   CString copyViewName = getNextCloneName();
   CEdif200View* copyView = new CEdif200View(m_edif, m_libraryName, m_cellName, copyViewName);
   copyView->m_derivedNumber = m_cloneCount;
   copyView->m_isHierarchyStructure = m_isHierarchyStructure;

   CString key;
   POSITION pos;
   for (pos = getPageStartPosition(); pos != NULL;)
   {
      CEdif200Page* page = getNextPage(pos);
      if (page != NULL)
      {  
         CString newPageName;
         newPageName.Format("%s_%d", page->getName(), copyView->m_derivedNumber);
         CString newGeneratedPageName = copyView->getUniquePageName(newPageName);
         CEdif200Page* copy = new CEdif200Page(m_edif, m_libraryName, m_cellName, copyViewName, newPageName, newGeneratedPageName, *page);
         copyView->m_pageMap.AddTail(copy);

         if (m_firstSchematicPage == page)
         {
            copyView->m_firstSchematicPage = copy;
         }
      }
   }

   for (pos = getSymbolStartPosition(); pos != NULL;)
   {
      CEdif200Symbol* symbol = getNextSymbol(pos);
      if (symbol != NULL)
      {
         CEdif200Symbol* copy = new CEdif200Symbol(m_edif, m_libraryName, m_cellName, copyViewName, *symbol);
         copyView->m_symbolMap.SetAt(copy->getName(), copy);
      }
   }

   for (pos = m_offPageConnectorNameMap.GetStartPosition(); pos != NULL;)
   {
      CString pageConnectorName;
      m_offPageConnectorNameMap.GetNextAssoc(pos, key, pageConnectorName);
      if (!pageConnectorName.IsEmpty())
      {
         copyView->m_offPageConnectorNameMap.SetAt(pageConnectorName, pageConnectorName);
      }
   }

   return copyView;
}

CString CEdif200View::getNextCloneName()
{
   CString retval;
   retval.Format("%s_%d", m_name, ++m_cloneCount);
   return retval;
}

CString CEdif200View::getUniquePageName(const CString pageName)
{
   CString retval;
   retval.Format("Sheet_%d(%s)", m_pageMap.GetCount() + 1, pageName);
   return retval;
}

bool CEdif200View::isOffPageConnector(const CString portName) 
{  
   CString offPageConnectorName;
   return m_offPageConnectorNameMap.Lookup(portName, offPageConnectorName)?TRUE:FALSE; 
}

bool CEdif200View::isGlobalPort(const CString portName)
{
   bool retval = true;
   if (m_currentSymbol != NULL && m_currentSymbol->findPort(portName) != NULL)
   {
      retval = false;
   }
   return retval;
}

CEdif200Page* CEdif200View::addPage(const CString pageName)
{
   CEdif200Page* page = findPage(pageName);
   if (page == NULL)
   {
      CString generatedPageName = getUniquePageName(pageName);
      page = new CEdif200Page(m_edif, m_libraryName, m_cellName, m_name, pageName, generatedPageName);
   	m_pageMap.AddTail(page);
   }
   return page;
}

CEdif200Page* CEdif200View::findPage(const CString pageName)
{
   for (POSITION pos = getPageStartPosition(); pos != NULL;)
   {
      CEdif200Page* page = getNextPage(pos);
      if (page != NULL && page->getName().CompareNoCase(pageName) == 0)
      {
         return page;
      }
   }
	return NULL;
}

CEdif200Page* CEdif200View::getFirstPage()
{
	CEdif200Page* page = getFirstSchematicPage();      
   if (page == NULL && !m_pageMap.IsEmpty())
   {
      page = m_pageMap.GetHead();
   }
	return page;
}

CString CEdif200View::getDelimitedPageBlockNumber(const CString delimiter)
{
   CString retval;
   for (POSITION pagePos = getPageStartPosition(); pagePos != NULL;)
   {
      CEdif200Page* page = getNextPage(pagePos);
      if (page != NULL && page->getPageBlock() != NULL)
      {
         if (retval.IsEmpty())
            retval.Format("%d", page->getPageBlock()->getBlockNumber());
         else
            retval.AppendFormat("%s%d", delimiter, page->getPageBlock()->getBlockNumber());
      }
   }
   return retval;
}

CEdif200Symbol* CEdif200View::addSymbol(const CString symbolName)
{
	CEdif200Symbol* symbol = findSymbol(symbolName);
	if (symbol == NULL)
	{
		symbol = new CEdif200Symbol(m_edif, m_libraryName, m_cellName, m_name, symbolName);
		m_symbolMap.SetAt(symbolName, symbol);
	}
	return symbol;
}

CEdif200Symbol* CEdif200View::findSymbol(const CString symbolName)
{
	CEdif200Symbol* symbol = NULL;
	m_symbolMap.Lookup(symbolName, symbol);
	return symbol;
}

void CEdif200View::attachHierarchySymbolToPage(CEdif200Symbol& symbol)
{
   m_isHierarchyStructure = false;

   CString value, hierarchyGeomNums;
   int hierarchyKeyword = sDoc->RegisterKeyWord(SCH_ATT_HIERARCHGEOM, 0, VT_STRING);

   for (POSITION pos = getPageStartPosition(); pos != NULL;)
   {
      CEdif200Page* page = m_pageMap.GetNext(pos);
      if (page !=  NULL && page->getPageBlock() != NULL)
      {
         hierarchyGeomNums.AppendFormat("%d ", page->getPageBlock()->getBlockNumber());

         sDoc->SetAttrib(&page->getPageBlock()->getAttributesRef(), hierarchyKeyword, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);
         m_isHierarchyStructure = true;
      }
   }

   if (m_isHierarchyStructure)
   {
      hierarchyGeomNums.Trim();
		int keyword = sDoc->RegisterKeyWord(SCH_ATT_HIERARCHYGEOMNUM, 0, VT_STRING);
		sDoc->SetAttrib(&symbol.getSymbolBlock()->getAttributesRef(), keyword, VT_STRING, hierarchyGeomNums.GetBuffer(0), SA_APPEND, NULL);   
   }
}

void CEdif200View::convertLogicalSchematicToPhysicalSchematic()
{
   for (POSITION pos = getPageStartPosition(); pos !=  NULL;)
   {
      CEdif200Page* page = getNextPage(pos);
      if (page != NULL)
      {      
         page->convertLogicalSchematicToPhysicalSchematic();
      }
   }
}

void CEdif200View::removedAllPageBlocks()
{
   for (POSITION pos = getPageStartPosition(); pos != NULL;)
   {
      CEdif200Page* page= getNextPage(pos);
      if (page != NULL)
      {
         sDoc->RemoveBlock(page->getPageBlock());
      }
   }
}

void CEdif200View::createNetList()
{
   // Hierarchy clusters does not creat net list here.  They will be call to create netlist from with in view->createNetList()
   if (m_isHierarchyStructure)
      return;

   for (POSITION pos = getPageStartPosition(); pos != NULL;)
   {
      CEdif200Page* page= getNextPage(pos);
      if (page != NULL)
      {
         page->createNetList();
      }
   }
}

void CEdif200View::propagateNetsFromInstanceToHierarchyStructure(CEdif200Instance* instance)
{
   for (POSITION pos = getPageStartPosition(); pos != NULL;)
   {
      CEdif200Page* page= getNextPage(pos);
      if (page != NULL)
      {
         page->propagateNetsFromInstanceToHierarchyStructure(instance);
      }
   }
}

/******************************************************************************
* View::DoView

   This function will do the following commands:
   - interface
   - contents
   - viewType

   Skip the following commands:
   - status
   - property
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200View::doView()
{
   int res = 0;
   int parenthesisCount = 0;

   // Create the block here, the content of the block will be provide in doContent() and doSymbol().
   // In case where nothing is defined in the view but the view is being reference by an instance, we won't insert a NULL block
   CString blockName;
   blockName.Format("%s%c%s%c%s", m_libraryName, SCH_DELIMITER, m_cellName, SCH_DELIMITER, m_name);
   blockName.MakeUpper();
   m_contentBlock = Graph_Block_On(GBO_APPEND, blockName, sFileStruct->getFileNumber(), 0L, blockTypeUnknown);
   Graph_Block_Off();

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "status") || !STRCMPI(sParser->token, "property") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData") )
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "interface"))
      {
         if ((res = doInterface()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "contents"))
      {
         if ((res = doContents()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "viewType"))
      {
         if ((res = GetString(m_viewType)) < 0)
            return res;
         m_viewType.MakeUpper();
      }
   }

   if (m_currentSymbol != NULL)
   {
      attachHierarchySymbolToPage(*m_currentSymbol);
   }

   // empty the Attributes because it is no longer needed
   m_portAttribMap.empty();

   return 1;
}

/******************************************************************************
* Library::DoInterface

   This function will do the following commands:
   - port
   - symbol
   - designator
   - property

   Skip the following commands:
   - portBundle
   - protectionFrame
   - arrayRelatedInfo
   - parameter
   - mustJoin
   - weakJoined
   - permutable
   - timing
   - simulate
   - comment
   - userData
   - joined          : it is skipped because it is not need, it is only implemented inside DoNet() becuase that is the NetList

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200View::doInterface()
{
   int res = 0;
   int parenthesisCount = 0;

   CAttributes* attribMap = new CAttributes();
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "protectionFrame") || !STRCMPI(sParser->token, "arrayRelatedInfo") ||
               !STRCMPI(sParser->token, "parameter") || !STRCMPI(sParser->token, "mustJoin") ||
               !STRCMPI(sParser->token, "weakJoined") || !STRCMPI(sParser->token, "permutable") ||
               !STRCMPI(sParser->token, "timing") || !STRCMPI(sParser->token, "simulate") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData") ||
               !STRCMPI(sParser->token, "portBundle") || !STRCMPI(sParser->token, "joined") )
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "port"))
      {
			// Ports declare here and then use in page might be Master or Global ports

         CAttributes* attribMap = new CAttributes();
         Port port;
         if ((res = port.DoPort(&attribMap)) < 0)
         {
            FreeAttribMap(&attribMap);
            return res;
         }
         
         if (port.portName.IsEmpty())
         {
            if (attribMap)
               FreeAttribMap(&attribMap);
         }
         else
         {
            m_portAttribMap.SetAt(port.portName, attribMap);
         }
      }
      else if (!STRCMPI(sParser->token, "symbol"))
      {
         // Create the block for symbol here
         m_currentSymbol = addSymbol(m_name);
         BlockStruct* block = Graph_Block_On(GBO_APPEND, m_currentSymbol->generateSymbolBlockName(), sFileStruct->getFileNumber(), 0L);
         Graph_Block_Off();
         m_currentSymbol->setSymbolBlock(block);

         // Need to set the attribute map here first so that it can be update or overwrite later in doSymbol()
         if (attribMap)
         {
            FreeAttribMap(&block->getAttributesRef());
            block->getAttributesRef() = attribMap;
            attribMap = NULL;
         }

         if ((res = m_currentSymbol->doSymbol()) < 0)
            return res;

         // There attributes are need by Schematic Navegator
         Display display;
         sEdifAttrib->AddDefinedAttrib(&block->getAttributesRef(), SCH_ATT_LIBRARYNAME,  m_libraryName.GetBuffer(0), ATTRIB_CHANGE_VALUE, &display, NULL);
         sEdifAttrib->AddDefinedAttrib(&block->getAttributesRef(), SCH_ATT_CELLNAME,  m_cellName.GetBuffer(0), ATTRIB_CHANGE_VALUE, &display, NULL);
      }
      else if (!STRCMPI(sParser->token, "designator"))
      {
         CString designator = "";
         CString renameString = "";
         Display display;
         if ((res = GetNameDef(designator, renameString, display)) < 0)
            return res;

         sEdifAttrib->AddDefinedAttrib(&attribMap, SCH_ATT_DESIGNATOR, designator, ATTRIB_CHANGE_VALUE, &display, NULL);
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
         if ((res = GetProperty(&attribMap)) < 0)
            return res;
      }
   }

   if (attribMap)
      FreeAttribMap(&attribMap);

   return 1;
}

/******************************************************************************
* CEdif200View::doContents

   This function will do the following commands:
   - instance
   - offPageConnector
   - figure
   - net
   - page
   - commentGraphics
   - portImplementation

   Skip the following commands:
   - boundingBox
   - section
   - netBundle
   - timing
   - simulate
   - when
   - follow
   - logicPort
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200View::doContents()
{
   int res = 0;
   int parenthesisCount = 0;

   // Reset the map
   m_offPageConnectorNameMap.RemoveAll();
   Graph_Block_On(GBO_APPEND, m_contentBlock->getName(), sFileStruct->getFileNumber(), 0L, blockTypeUnknown);

   EdifFigure figure;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "section") || !STRCMPI(sParser->token, "netBundle") ||
               !STRCMPI(sParser->token, "timing") || !STRCMPI(sParser->token, "simulate") || 
               !STRCMPI(sParser->token, "when") || !STRCMPI(sParser->token, "follow") || 
               !STRCMPI(sParser->token, "logicPort") || !STRCMPI(sParser->token, "boundingBox") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData") ||
               !STRCMPI(sParser->token, "net"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "instance"))
      {
         if ((res = doInstance(m_edif, NULL)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sBorderGrpLayer)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "commentGraphics"))
      {
         if ((res = doCommentGraphics(m_edif)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "portImplementation"))
      {
         CString portName;
         DataStruct* portData = NULL;
         if ((res = doPortImplementation(m_edif, portName, &portData, m_portAttribMap)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "page"))
      {
         CString pageName, renameString;
         Display display;
         if ((res = GetNameDef(pageName, renameString, display)) < 0)
            return res;

         CEdif200Page* page = addPage(pageName);
         if ((page->doPage()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "offPageConnector"))
      {
         if ((res = doOffPageConnector()) < 0)
            return res;
      }
   }

   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* CEdif200View::doOffPageConnector

   This function will only get the name of the port used as offpage connector

   Skip the following commands:
   - unused
   - property
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200View::doOffPageConnector()
{
   int res = 0;
   int parenthesisCount = 0;

   CString portName = "";
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(portName, renameString, display)) < 0)
      return res;

   m_offPageConnectorNameMap.SetAt(portName, portName);

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "unused") || !STRCMPI(sParser->token, "property") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
   }

   return 1;

}

//-----------------------------------------------------------------------------
// CEdif200Cell
//-----------------------------------------------------------------------------
CEdif200Cell::CEdif200Cell(CEdif200 &edif, const CString libraryName, const CString name)
	: m_edif(edif)
	, m_libraryName(libraryName)
	, m_name(name)
   , m_cellType("")
	, m_viewMap(nextPrime2n(20),true)
{
	reset();
}

CEdif200Cell::~CEdif200Cell()
{
	reset();
}

void CEdif200Cell::reset()
{
	m_viewMap.empty();
	m_currentView = NULL;
	m_hasNonHierarchyStructure = false;
}

CEdif200View* CEdif200Cell::addView(const CString viewName)
{
	CEdif200View* view = findView(viewName);
	if (view == NULL)
	{
		view = new CEdif200View(m_edif, m_libraryName, m_name, viewName);
		m_viewMap.SetAt(viewName, view);
	}
	return view;
}

CEdif200View* CEdif200Cell::copyAndAddView(const CString viewName)
{
   CEdif200View* copy = NULL;
	CEdif200View* view = findView(viewName);
	if (view != NULL)
	{
		copy = view->getCopy();
		m_viewMap.SetAt(copy->getName(), copy);
	}

   return copy;
} 

CEdif200View* CEdif200Cell::findView(const CString viewName)
{
	CEdif200View* view = NULL;
	m_viewMap.Lookup(viewName, view);
	return view;
}

CEdif200View* CEdif200Cell::getFirstView()
{
	CEdif200View* view = NULL;
	POSITION pos = m_viewMap.GetStartPosition();
	if (pos != NULL)
	{
		CString key;
		m_viewMap.GetNextAssoc(pos, key, view);
	}
	return view;
}

CEdif200Page* CEdif200Cell::getFirstPage()
{
	CEdif200Page* page = NULL;
	CEdif200View* view = getFirstView();
	if (view != NULL)
	{
		page = view->getFirstPage();
	}

	return page;
}

void CEdif200Cell::convertLogicalSchematicToPhysicalSchematic()
{
   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200View* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      if (view != NULL && !view->isHierarchyStructure())
      {
         // Only need to do convertLogicalSchematicToPhysicalSchematic() if not an hierarchy view in this cell
         view->convertLogicalSchematicToPhysicalSchematic();
      }
   }
}

void CEdif200Cell::removedUnusedOriginalViews()
{
   for (POSITION pos = getViewStartPosition(); pos != NULL;)
   {
      CEdif200View* view = getNextView(pos);
      if (view == NULL || view->getCloneCount() == 0)
         continue;

		m_viewMap.RemoveKey(view->getName());
      view->removedAllPageBlocks();
		delete view;
		view = NULL;
   }
}

void CEdif200Cell::createNetList()
{
   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200View* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      if (view != NULL)
      {
         view->createNetList();
      }
   }
}

void CEdif200Cell::propagateNetsFromInstanceToHierarchyStructure(CEdif200Instance* instance)
{
   for (POSITION pos = m_viewMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200View* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      if (view == NULL)
         continue;

		view->propagateNetsFromInstanceToHierarchyStructure(instance);
	}
}

/******************************************************************************
* CEdif200Cell::doCell

   This function will do the following commands:
   - cellType
   - view

   Skip the following commands:
   - status
   - viewMap
   - comment
   - userData
   - property

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Cell::doCell()
{
   int res = 0;
   int parenthesisCount = 0;
   bool isSchematicConvertionDone = false;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "status") || !STRCMPI(sParser->token, "comment") ||
					!STRCMPI(sParser->token, "userData") || !STRCMPI(sParser->token, "property"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
		else if (!STRCMPI(sParser->token, "viewMap"))
		{
         convertLogicalSchematicToPhysicalSchematic();
         isSchematicConvertionDone = true;

         if ((res = doViewMap()) < 0)
            return res;
		}
      else if (!STRCMPI(sParser->token, "cellType"))
      {
         if ((res = GetString(m_cellType)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "view"))
      {
         CString viewName, renameString;
         Display display;
         if ((res = GetNameDef(viewName, renameString, display)) < 0)
            return res;

         m_currentView = addView(viewName);
         if ((res = m_currentView->doView()) < 0)
            return res;

			if (m_hasNonHierarchyStructure == false)
				m_hasNonHierarchyStructure = !m_currentView->isHierarchyStructure();

         m_currentView = NULL;
      }
   }

   if (!isSchematicConvertionDone)
   {
      convertLogicalSchematicToPhysicalSchematic();
   }

   return 1;
}

int CEdif200Cell::doViewMap()
{
   int res = 0;
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "instanceBackAnnotate"))
      {
         if ((res = doInstanceBackAnnotate()) < 0)
            return res;
      }
      else
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
   }

   return 1;
}

int CEdif200Cell::doInstanceBackAnnotate()
{
   int res = 0;
   int parenthesisCount = 0;

   CString designator;
   CEdif200Instance* instance = NULL;
   CAttributes* attribMap = new CAttributes();
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "instanceRef"))
      {
         if ((res = doInstanceRef(&instance)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "designator"))
      {
         CString renameString = "";
         Display display;
         if ((res = GetNameDef(designator, renameString, display)) < 0)
            return res;

         sEdifAttrib->AddDefinedAttrib(&attribMap, SCH_ATT_DESIGNATOR, designator, ATTRIB_CHANGE_VALUE, &display, NULL);
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
         if ((res = GetProperty(&attribMap)) < 0)
            return res;
      }
      else
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
   }

   if (instance != NULL && instance->getInstanceDataStruct() != NULL)
   {
      if (!designator.IsEmpty())
         instance->setDesignator(designator);

      CopyAttribMap(&instance->getInstanceDataStruct()->getAttributesRef(), attribMap);
   }

   if (attribMap)
      FreeAttribMap(&attribMap);

   return 1;
}

int CEdif200Cell::doInstanceRef(CEdif200Instance** instance)
{
   int res = 0;
   int parenthesisCount = 0;

   CEdif200Instance* localInstance = NULL;
   CString instanceRef, viewRef;
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(instanceRef, renameString, display)) < 0)
      return res;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "instanceRef"))
      {
         if ((res = doInstanceRef(&localInstance)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "viewRef"))
      {

         if ((res = GetNameDef(viewRef, renameString, display)) < 0)
            return res;
      }
      else
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
   }

   CEdif200View* view = NULL;
   if (localInstance != NULL)
   {
      view = m_edif.findView(localInstance->getLibraryRef(), localInstance->getCellRef(), localInstance->getViewRef());
      localInstance = NULL;
   }
   else if (!viewRef.IsEmpty())
   {
      view = findView(viewRef);
   }

   if (view != NULL)
   {
      for (POSITION pos = view->getPageStartPosition(); pos !=  NULL;)
      {
         CEdif200Page* page = view->getNextPage(pos);
         if (page != NULL)
         {
            localInstance = page->findInstance(instanceRef);
            if (localInstance != NULL)
               break;
         }
      }
   }

   if (instance != NULL)
      *instance = localInstance;

   return 1;
}

//-----------------------------------------------------------------------------
// CEdif200Library
//-----------------------------------------------------------------------------
CEdif200Library::CEdif200Library(CEdif200 &edif, const CString name)
	: m_edif(edif)
	, m_name(name)
   , m_unitMap(nextPrime2n(20),true)
   , m_figureGroupMap(nextPrime2n(20),true)
	, m_cellMap(nextPrime2n(20),true)
{
	reset();
}

CEdif200Library::~CEdif200Library()
{
	reset();
}

void CEdif200Library::reset()
{
   m_unitMap.empty();
   m_figureGroupMap.empty();
	m_cellMap.empty();
	m_currentCell = NULL;
	m_hasNonHierarchyStructure = false;
}

CEdif200Cell* CEdif200Library::addCell(const CString cellName)
{
	CEdif200Cell* cell = fineCell(cellName);
	if (cell == NULL)
	{
		cell = new CEdif200Cell(m_edif, m_name, cellName);
		m_cellMap.SetAt(cellName, cell);
	}
	return cell;
}

CEdif200Cell* CEdif200Library::addCell(CEdif200Cell* cell)
{
	m_cellMap.SetAt(cell->getName(), cell);
	return cell;
}

CEdif200Cell* CEdif200Library::fineCell(const CString cellName)
{
	CEdif200Cell* cell = NULL;
	m_cellMap.Lookup(cellName, cell);
	return cell;
}

Unit* CEdif200Library::findUnit(const CString unitName)
{
	Unit* unit = NULL;
	m_unitMap.Lookup(unitName, unit);
	return unit;
}

FigureGroup* CEdif200Library::findFigureGroup(const CString figureGroupName)
{
	FigureGroup* figureGroup = NULL;
	m_figureGroupMap.Lookup(figureGroupName, figureGroup);
	return figureGroup;
}

void CEdif200Library::convertLogicalSchematicToPhysicalSchematic()
{
   //for (POSITION pos = m_cellMap.GetStartPosition(); pos != NULL;)
   //{
   //   CEdif200Cell* cell = NULL;
   //   CString key;
   //   m_cellMap.GetNextAssoc(pos, key, cell);
   //   if (cell == NULL)
   //      continue;

   //   cell->convertLogicalSchematicToPhysicalSchematic();
   //}
}

void CEdif200Library::removedUnusedOriginalViews()
{
   for (POSITION pos = m_cellMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200Cell* cell = NULL;
      CString key;
      m_cellMap.GetNextAssoc(pos, key, cell);
      if (cell == NULL)
         continue;

      cell->removedUnusedOriginalViews();
   }
}

void CEdif200Library::createNetList()
{
   for (POSITION pos = m_cellMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200Cell* cell = NULL;
      CString key;
      m_cellMap.GetNextAssoc(pos, key, cell);
      if (cell != NULL)
      {
         cell->createNetList();
      }
   }
}

/******************************************************************************
* Library::DoLibrary

   This function will do the following commands:
   - technology
   - cell

   Skip the following commands:
   - edifLevel
   - status
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Library::doLibrary()
{
   int res = 0;
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "edifLevel") || !STRCMPI(sParser->token, "status") ||
               !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData")) 
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "technology"))
      {
         if ((res = doTechnology()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "cell"))
      {
         CString cellName, renameString;
         Display display;
         if ((res = GetNameDef(cellName, renameString, display)) < 0)
            return res;
   
         m_currentCell = addCell(cellName);
         if ((res = m_currentCell->doCell()) < 0)
            return res;

			if (m_hasNonHierarchyStructure == false)
				m_hasNonHierarchyStructure = m_currentCell->hasNonHierarchStructure();

         m_currentCell = NULL;
      }
   }

   return 1;
}

/******************************************************************************
* Library::DoTechnology

   This function will do the following commands:
   - numberDefinition
   - figureGroup

   Skip the following commands:
   - fabricate
   - simulationInfo
   - physicalDesignRule
   - constant
   - constraint
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
*/
int CEdif200Library::doTechnology()
{
   int res = 0;
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "fabricate") || !STRCMPI(sParser->token, "simulationInfo") ||
               !STRCMPI(sParser->token, "physicalDesignRule") || !STRCMPI(sParser->token, "constant") ||
               !STRCMPI(sParser->token, "constraint") || !STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "numberDefinition"))
      {
         if ((res = doNumberDefinition()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figureGroup"))
      {
         FigureGroup *figureGroup = new FigureGroup;
         if ((res = figureGroup->DoFigureGroup()) < 0)
         {
            delete figureGroup;
            return res;
         }
         else
         {
            m_figureGroupMap.SetAt(figureGroup->m_name, figureGroup);
         }
      }
   }

   return 1;
}

/******************************************************************************
* Library::DoNumberDefinition

   This function defined a set of unit type that will be use for the entire library

   This function will do the following commands:
   - scale

   Skip the following commands:
   - gridMap
   - comment

   Return code:
    1 - no error
   -1 - syntax error
*/
int  CEdif200Library::doNumberDefinition()
{
   int res = 0;
   int parenthesisCount = 0;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "gridMap") || !STRCMPI(sParser->token, "comment"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "scale"))
      {
         if ((res = doScale()) < 0)
            return res;
      }
   }

   return 1;
}

/******************************************************************************
* Library::DoScale

   This function defined a unit type
*/
int CEdif200Library::doScale()
{
   int res = 0;
   double edifUnit = 0.0;
   double externalUnit = 0.0;
   CString unitType = "";
      
   if ((res = GetScaleInteger(edifUnit)) < 0)
      return res;

   if ((res = GetScaleInteger(externalUnit)) < 0)
      return res;

   if ((res = GetUnitType(unitType)) < 0)
      return res;
   unitType.MakeLower();

   // These are the type of unit definition that Edif allows and their corresponding unit names
   CString unitName = "";
   if (unitType == "angle")
      unitName = "radian";
   else if (unitType == "capacitance")
      unitName =  "farad";
   else if (unitType == "conductance")
      unitName =  "siemen";
   else if (unitType == "charge")
      unitName =  "coulomb";
   else if (unitType == "current")
      unitName =  "ampere";
   else if (unitType == "distance")
      unitName =  "meter";
   else if (unitType == "energy")
      unitName =  "joule";
   else if (unitType == "flux")
      unitName =  "weber";
   else if (unitType == "frequency")
      unitName =  "hertz";
   else if (unitType == "inductance")
      unitName =  "henry";
   else if (unitType == "mass")
      unitName =  "kilogram";
   else if (unitType == "power")
      unitName =  "watts";
   else if (unitType == "resistance")
      unitName =  "ohm";
   else if (unitType == "temperature")
      unitName =  "degree";
   else if (unitType == "time")
      unitName =  "seconds";
   else if (unitType == "voltage")
      unitName =  "volts";
   else
   {
      fprintf(sFileErr, "Syntax Error : Unit type \"%s\" on line %ld is not an allowed unit type in Edif 200.\n", sParser->token, sParser->lineCount);
      sDisplayErr++;
      return -1;
   }

   Unit *unit = new Unit();
   unit->edifUnit = edifUnit;
   unit->externalUnit = externalUnit;
   unit->unitType = unitType;
   unit->unitName = unitName;
   m_unitMap.SetAt(unit->unitType, unit);

   return 1;   
}


//-----------------------------------------------------------------------------
// CEdif200
//-----------------------------------------------------------------------------
CEdif200::CEdif200()
	:m_libraryMap(nextPrime2n(20),true)
{
	reset();
}

CEdif200::~CEdif200()
{
	reset();
}

void CEdif200::reset()
{
	m_libraryMap.empty();
	m_currentLibrary = NULL;
	m_hasNonHierarchyStructure = false;

	m_localUniqueNetNameMap.RemoveAll();
	m_localUniqueBusNameMap.RemoveAll();
}

CEdif200Library* CEdif200::addLibrary(const CString libraryName)
{
	CEdif200Library* library = findLibrary(libraryName);
	if (library == NULL)
	{
		library = new CEdif200Library(*this, libraryName);
		m_libraryMap.SetAt(libraryName, library);
	}
	return library;
}

CEdif200Library* CEdif200::findLibrary(const CString libraryName)
{
	CEdif200Library* library = NULL;
	m_libraryMap.Lookup(libraryName, library);
	return library;
}

CEdif200Cell* CEdif200::findCell(const CString libraryName, const CString cellName)
{
	CEdif200Library* library = findLibrary(libraryName);
	if (library == NULL)
		return NULL;
	else
		return library->fineCell(cellName);
}

CEdif200View* CEdif200::findView(const CString libraryName, const CString cellName, const CString viewName)
{
   CEdif200Cell* cell = findCell(libraryName, cellName);
   if (cell != NULL)
		return cell->findView(viewName);
	else
		return NULL;
}

CEdif200Cell* CEdif200::getCurrentCell()
{
   CEdif200Library* library = getCurrentLibrary();
   if (library != NULL)
      return library->getCurrentCell();
   else
      return NULL;
}

CEdif200View* CEdif200::getCurrentView()
{
   CEdif200Cell* cell = getCurrentCell();
   if (cell != NULL)
      return cell->getCurrentView();
   else
      return NULL;
}

CEdif200Symbol* CEdif200::getCurrentSymbol()
{
   CEdif200View* view = getCurrentView();
   if (view != NULL)
      return view->getCurrentSymbol();
   else
      return NULL;
}

CString CEdif200::getUniqueLocalNetName(const CString netName)
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

CString CEdif200::getUniqueLocalBusName(const CString busName)
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

void CEdif200::removedUnusedOriginalViews()
{
   // Now loop through all library and removed original views that created clones are no longer used
   for (POSITION pos = m_libraryMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200Library* library = NULL;
      CString key;
      m_libraryMap.GetNextAssoc(pos, key, library);
      if (library == NULL)
         continue;

      library->removedUnusedOriginalViews();
   }   
}

void CEdif200::createNetList()
{
   for (POSITION pos = m_libraryMap.GetStartPosition(); pos != NULL;)
   {
      CEdif200Library* library = NULL;
      CString key;
      m_libraryMap.GetNextAssoc(pos, key, library);
      if (library != NULL)
      {
         library->createNetList();
      }
   }
}
/******************************************************************************
* StartEdif200

   This function will do the following commands:
   - edifLevel
   - keywordLevel of keywordMap
   - library
   - external
   - design

   Skip the following commands:
   - edifFileNameDef
   - edifVersion
   - status
   - comment
   - userData

   Return code:
    1 - no error
   -1 - syntax error
   -2 - edif level is greater than zero
   -3 - keyword level is greater than zero
*/ 
int CEdif200::startEdif()
{
   int res = 0;
   int parenthesisCount = 0;

   if (!sParser->GetToken())     // Get the beginning '('
      return sParser->ParsingErr();
   if (STRCMPI(sParser->token, "("))
      return sParser->ParsingErr();

   if (!sParser->GetToken())     // Get the word 'Edif'
      return sParser->ParsingErr();
   if (STRCMPI(sParser->token, "edif"))
      return sParser->ParsingErr();

   CString edifFileName = "";
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(edifFileName, renameString, display)) < 0)
      return res;

   sFileStruct = Graph_File_Start(edifFileName, Type_EDIF);
   //sFileStruct->setBlockType(blockTypeSheet); //Do not set the file type for empty EDIF file
   sFileStruct->setBlock(NULL);   // Initialize to NULL, will be set to sCurrentBlock in page()
   sFileStruct->setShow(false);       // Initialize to FALSE, will be set to true in SchematicInstanceImplementation()

   // Start parsing the rest of the Edif file
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "edifFileNameDef") || !STRCMPI(sParser->token, "edifVersion") || 
               !STRCMPI(sParser->token, "status") || !STRCMPI(sParser->token, "comment") ||
               !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "edifLevel"))
      {
         if (!sParser->GetToken())
            return sParser->ParsingErr();
         int level = atoi(sParser->token);

         if (level > 0) //Currently we only support level 0
         {
            CString msg = "";
            msg.Format("Import of file terminated because EDIF level %d found at %ld\nOnly level 0 is supported!", level, sParser->lineCount);
            ErrorMessage(msg, "Unsupported EDIF Level",  MB_OK | MB_ICONHAND);
            return -2;
         }
      }
      else if (!STRCMPI(sParser->token, "keywordLevel"))
      {
         if (!sParser->GetToken())
            return sParser->ParsingErr();
         int keywordLevel = atoi(sParser->token);

         if (keywordLevel > 0)   //Currently we only support keywordLevel 0
         {
            CString msg = "";
            msg.Format("Import of file terminated because EDIF keyword level %d found at %ld\nOnly keyword level 0 is supported.", keywordLevel, sParser->lineCount);
            ErrorMessage(msg, "Unsupported EDIF Keyword Level",  MB_OK | MB_ICONHAND);
            return -3;
         }
      }
      else if (!STRCMPI(sParser->token, "external"))
      {
         CString libraryName, renameString;
         Display display;
         if ((res = GetNameDef(libraryName, renameString, display)) < 0)
            return res;

         m_currentLibrary = addLibrary(libraryName); 
         if ((res = m_currentLibrary->doLibrary()) < 0)
            return res;

			if (m_hasNonHierarchyStructure == false)
				m_hasNonHierarchyStructure = m_currentLibrary->hasNonHierarchStructure();

         m_currentLibrary = NULL;

         CString msg = "";
         msg.Format("Edif has referenced an external libray, %s, which is known to be incomplete.", libraryName);
         ErrorMessage(msg, "Warning");
      }
      else if (!STRCMPI(sParser->token, "library"))
      {
         CString libraryName, renameString;
         Display display;
         if ((res = GetNameDef(libraryName, renameString, display)) < 0)
            return res;

if (libraryName.CompareNoCase("ab10") == 0)
   int a = 0;

         m_currentLibrary = addLibrary(libraryName); 
         if ((res = m_currentLibrary->doLibrary()) < 0)
            return res;

         m_currentLibrary = NULL;
      }
      else if (!STRCMPI(sParser->token, "design"))
      {
         if ((res = doDesign()) < 0)
            return res;
      }
   }

   if (!sParser->GetToken())     // Get the closing ')'
      return sParser->ParsingErr();
   if (STRCMPI(sParser->token, ")"))
      return sParser->ParsingErr();

	if (m_hasNonHierarchyStructure == true)
		createNetList();

   if(sCreateTieDot)
      CreateTieDots(sTieDotRadius);
   removedUnusedOriginalViews();

   return 1;
}

int CEdif200::doDesign()
{
   int res = 0;
   int parenthesisCount = 0;

   CString designName = "";
   CString renameString = "";
   Display display;
   if ((res = GetNameDef(designName, renameString, display)) < 0)
      return res;

   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData") ||
               !STRCMPI(sParser->token, "property"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "cellRef"))
      {
         CString cellRef, libraryRef;
         if ((res = doCellRef(cellRef, libraryRef)) < 0)
            return res;

         CEdif200Cell* cell = findCell(libraryRef, cellRef);
         if (cell != NULL)
         {
				CEdif200View* view = cell->getFirstView();
				if (view != NULL)
				{
					CEdif200Page* page = view->getFirstPage();
					if (page != NULL && page->getPageBlock() != NULL)
					{
						sFileStruct->setShow(true);
						sFileStruct->setBlock(page->getPageBlock());

						Attrib *attrib = NULL;
						WORD hierarchyKeyword = (WORD)sDoc->RegisterKeyWord(SCH_ATT_HIERARCHGEOM, 0, VT_STRING);
						if (page->getPageBlock()->getAttributes() && page->getPageBlock()->getAttributes()->Lookup(hierarchyKeyword, attrib))
						{
							RemoveAttrib(hierarchyKeyword, &page->getPageBlock()->getAttributesRef());
						}

						if (view->isHierarchyStructure())
						{
							view->resetHierarchyStructure();
						}

						if (m_hasNonHierarchyStructure == false)
						{
							// Now we are going to use the above view as the level view so
							// this Edif will now have a non Hierarchy Structure so we can
							// try to create netlist again
							m_hasNonHierarchyStructure = true;
						}
					}
				}
         }
      }
   }

   return 1;
}

void CreateTieDots(double tieDotRadius)
{
   for (int i = 0; i < sDoc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = sDoc->getBlockAt(i);
      if (block == NULL || block->getBlockType() != blockTypeSheet)
         continue;
      if (block->getFileNumber() != sFileStruct->getFileNumber())
			continue;

      POSITION dataPos,pntPos,polyPos;
      dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if(((graphicClassSignal == data->getGraphicClass()) || (graphicClassBus == data->getGraphicClass()))
            && T_POLY == data->getDataType())
         {
            polyPos = data->getPolyList()->GetHeadPosition();
            CPoly *tmpPoly = new CPoly;
            while (NULL != polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);            
               double cx, cy, r;
               if (!PolyIsCircle(poly, &cx, &cy, &r))
               {
                  // loop points of poly
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (NULL != pntPos)
                  {
                     CPnt *pnt1 = poly->getPntList().GetNext(pntPos);
                     POSITION tmpPos = pntPos;
                     CPnt *pnt2 = poly->getPntList().GetNext(pntPos);
                     if(NULL != pntPos)
                        pntPos = tmpPos;

                     //Find in all other polys
                     POSITION restAll = polyPos;
                     while(NULL != restAll)
                     {
                        CPoly *restAllPoly = data->getPolyList()->GetNext(restAll);
                        double cx, cy, r;
                        POSITION tmpPos;
                        if (!PolyIsCircle(restAllPoly, &cx, &cy, &r))
                        {
                           tmpPos = restAllPoly->getPntList().GetHeadPosition();
                           while(NULL != tmpPos)
                           {
                              CPnt *pnt3 = restAllPoly->getPntList().GetNext(tmpPos);
                              POSITION tmpNextPos = tmpPos;
                              CPnt *pnt4 = restAllPoly->getPntList().GetNext(tmpPos);
                              if(NULL != tmpPos)
                                 tmpPos = tmpNextPos;
                              double sx0,sy0,sx1,sy1;
                              if(1 == intersectLineToLine(sx0,sy0,sx1,sy1,
                                 pnt1->x,pnt1->y,pnt2->x,pnt2->y,
                                 pnt3->x,pnt3->y,pnt4->x,pnt4->y))
                              {
                                 bool found = false;
                                 POSITION tmpPntPos = tmpPoly->getPntList().GetHeadPosition();
                                 //We have to draw only one TieDot per intersection.
                                 while(NULL != tmpPntPos)
                                 {
                                    CPnt *tmpPnt = tmpPoly->getPntList().GetNext(tmpPntPos);
                                    if((tmpPnt->x == (DbUnit)sx0) && (tmpPnt->y == (DbUnit)sy0))
                                    {
                                       found = true;
                                       break;
                                    }
                                 }

                                 if(false == found)
                                 {
                                    CPnt *pnt = new CPnt;
                                    pnt->x = (DbUnit)sx0;
                                    pnt->y = (DbUnit)sy0;
                                    tmpPoly->getPntList().AddTail(pnt);
                                    //Found the point where we have to draw the circle sx0,sy0
                                    CPoly *poly = new CPoly;
                                    poly->setWidthIndex(0);
                                    poly->setHatchLine(false);
                                    poly->setHidden(false); 
                                    poly->setThermalLine(false);
                                    poly->setFilled(false);
                                    poly->setVoid(false);
                                    poly->setFloodBoundary(false);
                                    poly->setClosed(true);
                                    poly->setFilled(true);

                                    data->getPolyList()->AddTail(poly);                                 

                                    pnt = new CPnt;
                                    pnt->x = (DbUnit)sx0;
                                    pnt->y = (DbUnit)(sy0 + tieDotRadius);
                                    pnt->bulge = (DbUnit)1.0;
                                    poly->getPntList().AddTail(pnt);

                                    pnt = new CPnt;
                                    pnt->x = (DbUnit)sx0;
                                    pnt->y = (DbUnit)(sy0 - tieDotRadius);
                                    pnt->bulge = (DbUnit)1.0;
                                    poly->getPntList().AddTail(pnt);

                                    pnt = new CPnt;
                                    pnt->x = (DbUnit)sx0;
                                    pnt->y = (DbUnit)(sy0 + tieDotRadius);
                                    pnt->bulge = (DbUnit)0.0;
                                    poly->getPntList().AddTail(pnt);
                                 }
                              }
                           }
                        }
                     }
                  }               
               }
            }
            if(tmpPoly)
               delete tmpPoly;
         }
      }
   }
}
/******************************************************************************
* CEdif200Page::doNetBundle

This function will do the following commands:
- listofnets
- figure
- net
- commentGraphics
- property

Skip the following commands:
- comment
- userData

Return code:
1 - no error
-1 - syntax error
*/
int CEdif200Page::doNetBundle()
{
   int res = 0;
   int parenthesisCount = 0;

   CString netName;
   CString renameString;
   CString arrayName;
   Display display;
   int lbIndex = 0;
   int ubIndex = 0;
   int size = -1;


   if ((res = GetNameDef(netName, renameString, display)) < 0)
      return res;

   CString text = netName;
   if (!renameString.IsEmpty())
   {
      netName = renameString;
      text = renameString;
   }
   else
   {
      renameString = netName;
   }

   POSITION pos = display.displayList.GetHeadPosition();
   while (pos)
   {
      DisplayParam *displayParam = display.displayList.GetNext(pos);

      if (!displayParam->m_isEmpty && displayParam->m_figureGroup.m_visible)
      {
         DataStruct *textdata = Graph_Text(sTextLayer, text, displayParam->m_x, displayParam->m_y, displayParam->m_figureGroup.m_textHeight, 
            displayParam->m_figureGroup.m_textWidth, DegToRad(displayParam->m_rotation), 0L, TRUE, displayParam->m_mirror, 
            0, FALSE, displayParam->m_figureGroup.m_widthIndex, 0, displayParam->m_horizontalJust, displayParam->m_verticalJust);
         if (textdata != NULL)
            textdata->getText()->setMirrorDisabled(true);
      }
   }


   CEdif200Bus* bus = NULL;
   CEdif200Net* net = NULL;
   bus = addBus(netName);
   bus->setPrimaryName(renameString);

   EdifFigure figure;
   while (TRUE)
   {
      if (!sParser->GetToken())
         return sParser->ParsingErr();

      if (!STRCMPI(sParser->token, "("))
         ++parenthesisCount;
      else if (!STRCMPI(sParser->token, ")"))
      {
         if (parenthesisCount-- == 0)
         {
            sParser->PushToken();
            break;
         }
      }
      else if (!STRCMPI(sParser->token, "comment") || !STRCMPI(sParser->token, "userData"))
      {
         if ((res = sParser->SkipCommand()) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "figure"))
      {
         if ((res = figure.DoFigure(sSignalGrpLayer, true /*fill circles*/)) < 0)
            return res;

         if (figure.data != NULL)
         {
            if (bus != NULL)
               figure.data->setGraphicClass(graphicClassBus);
         }
      }      
      else if (!STRCMPI(sParser->token, "net"))
      {
         net = NULL;
         CEdif200Bus* subBus = NULL;
         if ((res = doNet(&net,&subBus)) < 0)
            return res;
         if(NULL != net)
            bus->addNet(net);
         if(NULL != subBus)
            bus->addBus(subBus);
      }
      else if (!STRCMPI(sParser->token, "commentGraphics"))
      {
         if ((res = doCommentGraphics(m_edif)) < 0)
            return res;
      }
      else if (!STRCMPI(sParser->token, "property"))
      {
         if (net != NULL)
         {
            if ((res = GetProperty(&net->getAttributesRef())) < 0)
               return res;
         }
         else if (bus != NULL)
         {
            if ((res = GetProperty(&bus->getAttributesRef())) < 0)
               return res;
         }
         else
         {
            if ((res = sParser->SkipCommand()) < 0)
               return res;
         }
      }
   }
   net = NULL;

   if (figure.data)
   {
      Display display;
      sEdifAttrib->AddDefinedAttrib(&figure.data->getAttributesRef(), SCH_ATT_NETNAME, netName.GetBuffer(0), ATTRIB_CHANGE_VALUE, &display, NULL);
      if (bus != NULL)
      {
         CopyAttribMap(&figure.data->getAttributesRef(), bus->getAttributesRef());
      }
   }

   return 1;
}
