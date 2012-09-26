
#include "stdafx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "InFile.h"
#include "centroid.h"
#include "PcbComponentPinAnalyzer.h"
#include "xform.h"
#include "PrepDlg.h"
#include "RwUiLib.h"
#include "Extents.h"
#include "DeviceType.h"
#include "OmronAOI_Out.h"

/*****************************************************************************/

void OmronAOI_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
      NewOmronAOIWriter writer(doc, format->Scale);

      if (writer.DetermineActiveCCFile(&doc) != NULL)
      {
         if(!writer.WriteFiles(filename))
         {
            ErrorMessage("CAMCAD could not finish writing the Omron AOI output files.","", MB_OK);
         }
      }
}

/*****************************************************************************/
/* 
   Specify the Decimal of OmronAOI file 
*/
static int OmronAOIGetDecimals(PageUnitsTag units)

{
   // Omron output is required to be 1/10 mm units.
   // Numbers output are integers, no decimals.

   return 0;
}

static CString getOmronAOIDefaultLocation(bool isBottom)
{
   CString Location;
   Location.Format("%s,0.0,0.0,0,0,%d",(isBottom)?"LowerRight":"LowerLeft",fileTypeOmronAOI);   
   return Location;
}

////////////////////////////////////////////////////////////
// OmronAOIOutputAttrib
////////////////////////////////////////////////////////////
OmronAOIOutputAttrib::OmronAOIOutputAttrib(CCEtoODBDoc& camCadDoc) 
:  m_camCadDoc(camCadDoc)
,  m_attribKeyword(-1)
{
	SetDefaultAttribAndValue();
}

OmronAOIOutputAttrib::~OmronAOIOutputAttrib()
{
	m_valuesMap.RemoveAll();
}

int OmronAOIOutputAttrib::GetKeyword() const
{
	return m_attribKeyword;
}

bool OmronAOIOutputAttrib::HasValue(CAttributes** attributes)
{
	CString value, tmpValue;
   if(m_attribKeyword > -1)
   {
	   m_camCadDoc.getAttributeStringValue(value, attributes, m_attribKeyword);
	   value.MakeLower();
      return (m_valuesMap.GetCount() && m_valuesMap.Lookup(value, tmpValue))?true:false;
   }
   return false;
}

void OmronAOIOutputAttrib::SetKeyword(CString keyword)
{
   if(!keyword.IsEmpty())
   {
      m_attribKeyword = m_camCadDoc.RegisterKeyWord(keyword, 0, VT_STRING); 
      m_valuesMap.RemoveAll();
   }
}

void OmronAOIOutputAttrib::AddValue(CString value)
{
	CString tmpValue = value;
	m_valuesMap.SetAt(tmpValue.MakeLower(), tmpValue.MakeLower());
}

void OmronAOIOutputAttrib::SetDefaultAttribAndValue()
{

}

////////////////////////////////////////////////////////////
// COmronAOISetting
////////////////////////////////////////////////////////////
COmronAOISetting::COmronAOISetting()
{
   LoadDefaultSettings();
}

COmronAOISetting::~COmronAOISetting()
{
}

void COmronAOISetting::LoadDefaultSettings()
{
   m_PartNumberKW = ATT_PARTNUMBER;
   m_outputPageUnits = pageUnitsMilliMeters;
   m_outputPageUnitsScale = 10.0;  // Units are 1/10 of mm, so scale mm page units by 10 to get Omron units
   m_exportUNLoaded = false;
   m_attribKeyword.Empty();
   m_attribValue.RemoveAll();
   m_originOutputUnitsX = 0.;
   m_originOutputUnitsY = 0.;
   m_componentNumberValueFrom = QGEOM_NAME;

   m_railWidth = -1;       // Means not set, only positive values are considered set.
   m_railWidthMargin = 0;  // Means no margin, margin may be pos or neg, but expected usually pos.

}

bool COmronAOISetting::LoadStandardSettingsFile(CString FileName)
{
   CInFile inFile;
   if(!inFile.open(FileName))
      return FALSE;

   while (inFile.getNextCommandLine(", "))
   {
      if (inFile.isCommand(".PARTNUMBER_KEYWORD",2))
      {
         m_PartNumberKW = inFile.getParam(1);
      }
      else if (inFile.isCommand(".EXPORT_UNLOADED_PARTS",2))
      {
         inFile.parseYesNoParam(1,m_exportUNLoaded,false);
      }
      else if (inFile.isCommand(".UNITS",2))
      {
         m_outputPageUnits = StringToPageUnitsTag(inFile.getParam(1));
      }
      else if (inFile.isCommand(".OUTPUT_ATTRIBUTE",2)) 
      {
         m_attribKeyword = inFile.getParam(1);
      }
      else if (inFile.isCommand(".OUTPUT_VALUE",2))
      {
         m_attribValue.Add(inFile.getParam(1));
      }
      else if (inFile.isCommand(".COMPONENT_NUMBER_KEYWORD",2))
      {
         m_componentNumberValueFrom = inFile.getParam(1);
      }
      else if (inFile.isCommand(".BOARD_ORIGIN", 3))
      {
         CString xStr( inFile.getParam(1) );
         CString yStr( inFile.getParam(2) );
         m_originOutputUnitsX = atof(xStr);
         m_originOutputUnitsY = atof(yStr);
      }
      else if (inFile.isCommand(".RAIL_WIDTH", 2))
      {
         m_railWidth = inFile.getIntParam(1);
      }
      else if (inFile.isCommand(".RAIL_WIDTH_MARGIN", 2))
      {
         m_railWidthMargin = inFile.getIntParam(1);
      }
   }

   inFile.close();
   return TRUE;
}

////////////////////////////////////////////////////////////
// COmronAOIInsertData
////////////////////////////////////////////////////////////
COmronAOIInsertData::COmronAOIInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data)
: m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
, m_InsertData(data)
, m_posX(0.)
, m_posY(0.)
, m_rotation(0.)
, m_insertedBlock(NULL)
, m_BoardType(blockTypeUndefined)
{
}

void COmronAOIInsertData::setRotation(double rotation)
{
   m_rotation = rotation;
}

////////////////////////////////////////////////////////////
// COmronAOIInsertDataList
////////////////////////////////////////////////////////////
COmronAOIInsertDataList::COmronAOIInsertDataList(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_topInserts(0)
, m_botInserts(0)
{
}

COmronAOIInsertDataList::~COmronAOIInsertDataList()
{
   m_topInserts = 0;
   m_botInserts = 0;
   empty();
}

void COmronAOIInsertDataList::Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, 
                                CString partNumber, BlockTypeTag boardType, bool placedBottom)
{
   COmronAOIInsertData *data = new COmronAOIInsertData(m_camCadDoc, InsertData);
   data->setPartNumber(partNumber);
   data->setRefName(refName);
   data->setX(posX);
   data->setY(posY);
   data->setBoardType(boardType);
   data->setPlacedBottom(placedBottom);

   if(InsertData && InsertData->getInsert())
   {     
      //Calculate centroid rotation
      BlockStruct *geometryBlock = m_camCadDoc.getBlockAt(InsertData->getInsert()->getBlockNumber());
      data->setInsertedBlock(geometryBlock);
      DataStruct *centroidData = centroid_exist_in_block(geometryBlock);  
      double CentroidRotation = 0.;
      if(centroidData)
      {
         //GenerateCentroidRotationOffset(geometryBlock, centroidData, CentroidRotation);
         rotation += centroidData->getInsert()->getAngleDegrees();
      }

      if(placedBottom)
         m_botInserts ++;
      else
         m_topInserts ++;
   }  

   data->setRotation(normalizeDegrees(rotation));
   
   SetAtGrow(GetCount(), data);
}

////////////////////////////////////////////////////////////
// COmronAOIPCBData
////////////////////////////////////////////////////////////
COmronAOIPCBData::COmronAOIPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry)
: m_originX(x)
, m_originY(y)
, m_boardType(boardType)
, m_FidDataList(camCadDoc)
, m_CompDataList(camCadDoc)
{
   m_FidDataList.empty();
   m_CompDataList.empty();
   
   m_insertData = data;
   m_geometryBlock = geometry;
}

COmronAOIPCBData::~COmronAOIPCBData()
{
   m_FidDataList.empty();
   m_CompDataList.empty();
}

////////////////////////////////////////////////////////////
// COmronAOIPCBDataList
////////////////////////////////////////////////////////////
COmronAOIPCBDataList::COmronAOIPCBDataList()
{
   m_Panelflag = false;
   m_BoardName = QOMRON_BOARDNAME;
}

COmronAOIPCBDataList::~COmronAOIPCBDataList()
{
   empty();
}

void COmronAOIPCBDataList::empty()
{
   m_Panelflag = false;
   m_BoardName = QOMRON_BOARDNAME;

   for(POSITION pcbPos = GetStartPosition(); pcbPos; )
   {
      CString pcbName;
      COmronAOIPCBData *pcbData =NULL;
      GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData)
         delete pcbData;
   }
   RemoveAll();
}

void COmronAOIPCBDataList::SetBoardAt(CString boardName, COmronAOIPCBData *pcbdata)
{
   COmronAOIPCBData *existdata = NULL;

   if(boardName.IsEmpty()) 
   {
      boardName.Format("%s_$%d", QOMRON_BOARDNAME, GetCount());
   }
   else if(Lookup(boardName,existdata))
   {
      boardName.Format("%s_$%d", boardName, GetCount());
   }

   SetAt(boardName,pcbdata);
}

////////////////////////////////////////////////////////////
// OmronAOIWriter
////////////////////////////////////////////////////////////
OmronAOIWriter::OmronAOIWriter(CCEtoODBDoc &doc)
: m_doc(doc)
, m_dUnitFactor(1.0)
, m_unit_accuracy(3)
, m_attribMap(doc)
, m_textLogFile(GetLogfilePath("OmronAOI.log"))
, m_compRecordNum(1)
{
   // Cache this KW so we don't have to keep looking it up during output
   m_deviceTypeKW = doc.getCamCadData().getAttributeKeywordIndex(standardAttributeDeviceType);
}

OmronAOIWriter::~OmronAOIWriter(void)
{
   m_textLogFile.close();
   m_usedRefnameMap.RemoveAll();
}

bool OmronAOIWriter::LoadSettingsFile()
{
   m_writeProcess.updateStatus("Loading Setting File");
   CString settingsFilePath( getApp().getExportSettingsFilePath("OmronAOI.out") );
   bool retVal = GetSettings().LoadStandardSettingsFile(settingsFilePath);
   if(retVal)
   {
      m_attribMap.SetKeyword(GetSettings().getAttribKeyword());
      for(int i = 0; i < GetSettings().getAttribValueArray().GetCount(); i++)
      {
         m_attribMap.AddValue(GetSettings().getAttribValueArray().GetAt(i));
      }
   }
   else
   {
      m_textLogFile.writef("Load Settings File: Can't load file OmronAOI.out.\nFull path: [%s]\n", settingsFilePath);
   }
   return retVal;
}  

void OmronAOIWriter::ApplyPrepSettingToFiles(FileStruct* fileStruct, COmronAOIPCBData *pcbData, bool isBottom, bool isPanel)
{
   //For calculating the location, Bottom side is alsways Mirrored to Top
   saveRestoreSettings OriginalSettings(&m_doc, fileStruct, isBottom);
   Attrib *attrib =  is_attvalue(&m_doc, fileStruct->getBlock()->getDefinedAttributes(), GetMachineAttributeName(fileTypeOmronAOI,isBottom), 0);

   ExportFileSettings m_PCBLocation;
   m_PCBLocation = (attrib)?attrib->getStringValue():getOmronAOIDefaultLocation(isBottom);
   m_PCBLocation.ApplyToCCZ(&m_doc, fileStruct, OriginalSettings, fileTypeOmronAOI);   
   
   if(isPanel)
      TraversePanel(fileStruct->getTMatrix(),fileStruct->getBlock());
   else
      TraversePCBDesign(fileStruct->getBlock(), pcbData, fileStruct->getTMatrix());
   
   OriginalSettings.Restore();
}

void OmronAOIWriter::GenerateOutputFilenames(CString incomingName, CString &topSideName, CString &botSideName)
{
   // The incomingName is file chosen by user. Generate names with top and bot suffixes.
   // If incomingName has top or bot suffix already, keep it and make matching other name.
   // Otherwise apply top and bot suffixes to get names.

   CFilePath incomingNamePath(incomingName, false);  // false turns "beautify" off, keeps name as-is

   CString baseFilename( incomingNamePath.getBaseFileName() );

   if (baseFilename.Right(4).CompareNoCase("_top") == 0)
   {
      topSideName = incomingName;
      CString botSideBasename(baseFilename);
      botSideBasename.Truncate( baseFilename.GetLength() - 4 );
      botSideBasename += "_bot";
      incomingNamePath.setBaseFileName(botSideBasename);
      botSideName = incomingNamePath.getPath();
   } 
   else if (baseFilename.Right(4).CompareNoCase("_bot") == 0)
   {
      botSideName = incomingName;
      CString topSideBasename(baseFilename);
      topSideBasename.Truncate( baseFilename.GetLength() - 4 );
      topSideBasename += "_top";
      incomingNamePath.setBaseFileName(topSideBasename);
      topSideName = incomingNamePath.getPath();
   }
   else
   {
      incomingNamePath.setBaseFileName( baseFilename + "_top" );
      topSideName = incomingNamePath.getPath();
      incomingNamePath.setBaseFileName( baseFilename + "_bot" );
      botSideName = incomingNamePath.getPath();
   }
}

bool OmronAOIWriter::WriteFiles(CString filename)
{  
   bool retVal = TRUE;
   m_writeProcess.updateStatus("Writing Omron AOI Files");  
   LoadSettingsFile();

   double pageUnitsFactor = Units_Factor(m_doc.getSettings().getPageUnits(), GetSettings().getOutputPageUnits());
   m_dUnitFactor =  pageUnitsFactor * GetSettings().getOutputPageUnitsScale();
   m_unit_accuracy = OmronAOIGetDecimals(GetSettings().getOutputPageUnits());

   GenerateOutputFilenames(filename, m_topSideFilename, m_botSideFilename);

   m_compRecordNum = 1;  // Output record number, counting starts at 1
   m_usedRefnameMap.RemoveAll();

   // generate centroid in order to get centroid of the insert.
   //generate_CENTROIDS(&m_doc);

   if (getActiveFile() != NULL && getActiveFile()->getBlockType() == blockTypePanel)
   {
      retVal = ProcessPanel();
   }
   else
   {
      retVal = ProcessPCBDesign();
   }

   return retVal;
}

bool OmronAOIWriter::WritePCBFiles(COmronAOIPCBData *pcbData, CString boardName)
{
   if(!pcbData) return FALSE;

   m_writeProcess.updateStatus("Writing PCB Files.");

   bool stat1 = WritePCBFile(pcbData, boardName, false); // top
   bool stat2 = WritePCBFile(pcbData, boardName, true ); // bot

   return (stat1 && stat2);
}

bool OmronAOIWriter::WritePCBFile(COmronAOIPCBData *pcbData, CString boardName, bool isBottom)
{   
   if(!pcbData) return FALSE;

   CString fileName( this->getOutputFilename(isBottom) );

   CFormatStdioFile File;
   if (!File.Open(fileName, CFile::modeCreate|CFile::modeWrite))
	{      
      ErrorMessage(fileName, "Output file could not be opened", MB_OK);
      return FALSE;
   }

   // Write the header
   WriteHeader(File, isBottom);

   // Write Fids and Components
   m_compRecordNum = 1;  // reset output index
   m_usedRefnameMap.RemoveAll();
   WriteFiducialSetList(File, 1, pcbData, blockTypePcb, isBottom);
   WriteComponentList(File, pcbData, isBottom);
      
   File.Close();

   return TRUE;
}

bool OmronAOIWriter::WritePanelFiles(COmronAOIPCBData *pcbData)
{
   m_writeProcess.updateStatus("Writing Panel Files.");

   bool stat1 = WritePanelFile(pcbData, false); // top
   bool stat2 = WritePanelFile(pcbData, true ); // bot

   return (stat1 && stat2);
}

bool OmronAOIWriter::WritePanelFile(COmronAOIPCBData *pcbData, bool isBottom)
{  
   if(!m_PCBDataList.IsPanel())
      return FALSE;

   CString fileName( this->getOutputFilename(isBottom) );

   CFormatStdioFile File;
   if (!File.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage(fileName, "Output file could not be opened.", MB_OK);
      return FALSE;
   }

   // Write the header
   WriteHeader(File, isBottom);
   
   // Write Fids and Components
   m_compRecordNum = 1;  // reset output index
   m_usedRefnameMap.RemoveAll();
   WriteFiducialSetList(File, 1, pcbData, blockTypePanel, isBottom);
   WriteComponentList(File, NULL, isBottom);   

   File.Close();

   return TRUE;
}
   
double OmronAOIWriter::applyOmronOriginX(double rawSakiX)
{
   // Incoming rawSaki coord is in Saki units, but relative to original CCZ origin.
   // Make it relative to specified Saki origin, which itself is relative to panel corner.

   // Panel corner in Saki units plus specific origin (offset) in settings file is origin for output.
   double sakiOrigin = this->m_outlineLLX + this->GetSettings().getBoardOriginX();
   double finalVal = rawSakiX - sakiOrigin;

   return finalVal;
}

double OmronAOIWriter::applyOmronOriginY(double rawSakiY)
{
   // Incoming rawSaki coord is in Saki units, but relative to original CCZ origin.
   // Make it relative to specified Saki origin, which itself is relative to panel corner.

   // Panel corner in Saki units plus specific origin (offset) in settings file is origin for output.
   double sakiOrigin = this->m_outlineLLY + this->GetSettings().getBoardOriginY();
   double finalVal = rawSakiY - sakiOrigin;

   return finalVal;
}

void  OmronAOIWriter::WriteHeader(CFormatStdioFile &File, bool isBottom)
{
   CTime dateTime = CTime::GetCurrentTime();
   
   double boardSizeX = 0.;
   double boardSizeY = 0.;
   double boardOriginX = 0.;
   double boardOriginY = 0.;
   CExtent boardExtent = this->GetOutlineExtent();
   if (boardExtent.isValid())
   {
      boardSizeX = boardExtent.getXsize();
      boardSizeY = boardExtent.getYsize();

      this->setOutlineLowerLeft(getOmronAOIUnits(boardExtent.getXmin()), getOmronAOIUnits(boardExtent.getYmin()));
   }

   File.WriteString("1:%16.16s\n", this->m_PCBDataList.getBoardName());
   File.WriteString("2:%3.3s\n", "FLS");

   // PCB Size, no sign, up to 5 digits, no delimiter between values (not even white space)

   // Issue warning in log if board size exceeds apparant maximum.
   // The spec does not appear to contain explicit statement of maximum, may be that it varies by
   // machine, not specific to format. Format limit is 5 digits, so use 99999 for now.
   if (getOmronAOIUnits(boardSizeX) > 99999 || getOmronAOIUnits(boardSizeY) > 99999)
   {
      CString msg;
      msg.Format("Warning: Board size %d x %d exceeds Omron format maximum of 5 digits. [99999 x 99999]",
         getOmronAOIUnits(boardSizeX), getOmronAOIUnits(boardSizeY));
      m_textLogFile.writef(msg);
   }

   File.WriteString("3:%5d%5d\n", getOmronAOIUnits(boardSizeX), getOmronAOIUnits(boardSizeY));

   
   // Rail Width can be specificed explicitly in settings file using .RAIL_WIDTH xx command.
   // That value would be used as-is.
   // Or a margin can be specified using .RAIL_WIDTH_MARGIN, this value is added to the board X
   // dimension to get Rail Width for output. (So it is specific already in output units.)
   // Use of RAIL_WIDTH takes precedence over RAIL_WIDTH_MARGIN.
   // If neither are set the default rail width is same as PCB X dimension.
   int railWidth = getOmronAOIUnits(boardSizeX); // Default
   if (GetSettings().getRailWidth() > 0)
      railWidth = GetSettings().getRailWidth();
   else
      railWidth += GetSettings().getRailWidthMargin();
   File.WriteString("4:%5d\n", railWidth); 

   // Spec says blank lines are ignored. Samples have a blank line between header
   // and the rest, so we write a blank line at the end of the header too.
   File.WriteString("\n");
}

void OmronAOIWriter::WriteComponentList(CFormatStdioFile &File, COmronAOIPCBData *pcbData, bool isBottom)
{
   if(pcbData)
   {
      WriteComponents(File, 1, pcbData,isBottom);
   }
   else
   {
      int boardIdx = 0;
      for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
      {
         CString pcbName;
         m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
         if(pcbData->getBoardType() == blockTypePcb)
         {
            WriteComponents(File, ++boardIdx, pcbData, isBottom);         
         }
      }
   }
}

void OmronAOIWriter::WriteComponents(CFormatStdioFile &File, int boardIdx, COmronAOIPCBData *pcbData, bool isBottom)
{
   int compCnt = (pcbData)?pcbData->getComponentDataList().GetCount():0;
   CString PCBSide = (isBottom)?"Bottom":"Top";

   for(int compIndex = 0; compIndex < compCnt; compIndex++)
   {
      COmronAOIInsertData *cmpdata = pcbData->getComponentDataList().GetAt(compIndex);
      if(cmpdata && cmpdata->getInsertData() && cmpdata->getPlacedBottom() == isBottom)
      {
         WriteComponentRecord(File, boardIdx, cmpdata, isBottom);
      }
   }
}

void  OmronAOIWriter::WriteComponentRecord(CFormatStdioFile &File, int boardIdx, COmronAOIInsertData *sakiCmpData, bool isBottom)
{
   if (sakiCmpData != NULL)
   {
      CString xStr,yStr,rotStr,attribute;

      // X and Y, up to 5 digits, plus sign, for total of 6 chars output.
      xStr.Format("%6d", round(applyOmronOriginX(sakiCmpData->getX())));
      yStr.Format("%6d", round(applyOmronOriginY(sakiCmpData->getY())));

      // Rotation, spec says 3 digits, range 000 to 300. Surely they meant 000 to 360.
      rotStr.Format("%3d", round(sakiCmpData->getRotation()));
      
      //int flg = GetFlg(sakiCmpData);

      File.WriteString("%s", xStr);       // X
      File.WriteString("%s", yStr);       // Y
      File.WriteString("%s", rotStr);     // Theta

      File.WriteString("%-10.10s", getUniqueName(sakiCmpData->getRefName()));  // Comp ref up to 10 chars

      // Component Number
      // Value output is either inserted geomtry's name or attrib val selected by user.
      // Up to 16 chars, left justified.
      File.WriteString("%-16.16s\n", getComponentNumberValue(sakiCmpData));   
   }
}
  
CString OmronAOIWriter::getConfigurableValue(COmronAOIInsertData *sakiCmpData, CString dataSourceName)
{
   CString value("NotSet");

   if (sakiCmpData != NULL)
   {
      if (dataSourceName.CompareNoCase(QGEOM_NAME) == 0)
      {
         // Use inserted geometry's name
         value = sakiCmpData->getInsertedBlockName();
      }
      else if (sakiCmpData->getInsertData() != NULL)
      {
         // macroKeyword is assumed to be an attribute, look it up and return value
         Attrib *attrib = NULL;
         attrib = is_attvalue(&m_doc, sakiCmpData->getInsertData()->getAttributesRef(), dataSourceName, 0);
         value = attrib ? attrib->getStringValue() : "NotSet";
      }
   }

   return value;
}

CString OmronAOIWriter::getComponentNumberValue(COmronAOIInsertData *sakiCmpData)
{
   CString macroKeyword = GetSettings().getComponentNumberKeyword();
   return getConfigurableValue(sakiCmpData, macroKeyword);
}
  
int OmronAOIWriter::GetFlg(COmronAOIInsertData *sakiCmpData)
{
   // 98=fid, 2=IC 1=chip

   int flg = 1;  // Default to chip

   if (sakiCmpData != NULL)
   {
      if (sakiCmpData->getInsertType() == insertTypeFiducial)
      {
         flg = 98;
      }
      else if (sakiCmpData->getInsertData() != NULL)
      {
         // Use DeviceType, if one of the IC dev types return 2, for all else return 1
         DeviceTypeTag devType = deviceTypeUnknown;
         Attrib *attrib;
         if (sakiCmpData->getInsertData()->lookUpAttrib(m_deviceTypeKW, attrib))
            devType = stringToDeviceTypeTag(attrib->getStringValue());

         switch (devType) 
         {
         case deviceTypeIC:
         case deviceTypeICDigital:
         case deviceTypeICLinear:
            flg = 2;
            break;
         default:
            // already set to 1
            break;
         }
      }
   }
   
   return flg;
}

CString OmronAOIWriter::getUniqueName(CString refname)
{
   int counter = 0;

   CString adjustedName( refname );

   void *junk;
   while (m_usedRefnameMap.Lookup(adjustedName, junk))
   {
      adjustedName.Format("%s~%d", refname, ++counter);
   }

   m_usedRefnameMap.SetAt(adjustedName, NULL);

   return adjustedName;
}

void OmronAOIWriter::WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, COmronAOIPCBData *pcbData, BlockTypeTag boardType, bool isBottom)
{
   int fidCnt = (pcbData)?pcbData->getFiducialDataList().GetCount():0;

   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex++)
   {
      COmronAOIInsertData *fiddata = pcbData->getFiducialDataList().GetAt(fidIndex);
      if(fiddata && fiddata->getInsertData() && fiddata->getBoardType() == boardType && fiddata->getPlacedBottom() == isBottom)
      {         
         WriteComponentRecord(File, boardIdx, fiddata, isBottom);
      }
   }   
}

bool OmronAOIWriter::ProcessPCBDesign()
{
   FileStruct* fileStruct = this->getActiveFile();
   if (!fileStruct)  return FALSE;
   
   BlockStruct* block = fileStruct->getBlock();
   if(!block)  return FALSE;

   if(!block->getName().IsEmpty()) m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.setPanelFlag(false);   

   COmronAOIPCBData *pcbData = new COmronAOIPCBData(m_doc, getOmronAOIUnits(fileStruct->getInsertX()),
      getOmronAOIUnits(fileStruct->getInsertY()), blockTypePcb, NULL, block);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(),pcbData);

   ApplyPrepSettingToFiles(fileStruct, pcbData, false, false);          

   //PCB Data format
   WritePCBFiles(pcbData, m_PCBDataList.getBoardName());

   return TRUE;
}

int OmronAOIWriter::GetComponentCount(bool isBottom)
{
   int count = 0;

   if (this->m_PCBDataList.IsPanel())
   {
      // Panel, sum counts for all boards
      for(POSITION pcbPos = m_PCBDataList.GetStartPosition(); pcbPos; )
      {
         CString pcbName;
         COmronAOIPCBData *pcbData;
         m_PCBDataList.GetNextAssoc(pcbPos, pcbName, pcbData);
         if(pcbData->getBoardType() == blockTypePcb)
         {
            count += pcbData->getCompCount(isBottom);         
         }
         else
         {
            int jj = 0;
         }
      }
   }
   else
   {
      // One board
      POSITION pos = this->m_PCBDataList.GetStartPosition();
      while (pos != NULL)
      {
         CString pcbName;
         COmronAOIPCBData *pcbData;
         this->m_PCBDataList.GetNextAssoc(pos, pcbName, pcbData);

         count += pcbData->getCompCount(isBottom);
      }
   }

   return count;
}

bool OmronAOIWriter::ProcessPanel()
{     
   FileStruct * fileStruct = this->getActiveFile();     
   if(!fileStruct)   return FALSE;
   
   BlockStruct* block = fileStruct->getBlock();   
   if(!block)  return FALSE;

   //if(!isUniqueBoardNameOnPanel(block)) 
   //{
   //   //  This was a Fuji Flexa limitation, inherited by cloning code. Probably does not apply, is not needed, for Saki.
   //   ErrorMessage("OmronAOI Writer does not support multiple boards or mirrored board in panel.", "Panel Board", MB_OK);
   //   return FALSE;
   //}

   //only one PCB data
   if(!block->getName().IsEmpty()) m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.setPanelFlag(true);

   COmronAOIPCBData *pcbData = new COmronAOIPCBData(m_doc, getOmronAOIUnits(fileStruct->getInsertX()), 
      getOmronAOIUnits(fileStruct->getInsertY()), blockTypePanel, NULL, block);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(),pcbData);
  
   ApplyPrepSettingToFiles(fileStruct, pcbData, false, true);

   //Panel format
   WritePanelFiles(pcbData);
   
   return TRUE;      
}

CExtent OmronAOIWriter::GetOutlineExtent()
{
   Mat2x2 m;
   RotMat2(&m, 0);

	ExtentRect fileExtents;

   fileExtents.left = fileExtents.bottom = FLT_MAX;
   fileExtents.right = fileExtents.top = -FLT_MAX;

   bool OutlineFound = false;

   if (m_activeFile != NULL)
   {
      POSITION dataPos = m_activeFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = m_activeFile->getBlock()->getDataList().GetNext(dataPos);

         if ((data->getDataType() == T_POLY) &&
            ((m_activeFile->getBlockType() == BLOCKTYPE_PANEL && data->getGraphicClass() == GR_CLASS_PANELOUTLINE) ||
            (m_activeFile->getBlockType() == BLOCKTYPE_PCB && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)))
         {
            OutlineFound = true;

            ExtentRect polyExtents;
            if (PolyExtents(&m_doc, data->getPolyList(), &polyExtents, 1, 0, 0, 0, &m, FALSE))
            {
               if (polyExtents.left < fileExtents.left)
                  fileExtents.left = polyExtents.left;
               if (polyExtents.right > fileExtents.right)
                  fileExtents.right = polyExtents.right;
               if (polyExtents.bottom < fileExtents.bottom)
                  fileExtents.bottom = polyExtents.bottom;
               if (polyExtents.top > fileExtents.top)
                  fileExtents.top = polyExtents.top;
            }
         }
      }
   }


   CExtent extent(fileExtents.left, fileExtents.bottom, fileExtents.right, fileExtents.top);

   if (!OutlineFound && m_activeFile != NULL)
   {
      m_activeFile->getBlock()->calculateVisibleBlockExtents(m_doc.getCamCadData());		
      extent = m_activeFile->getBlock()->getExtent();
   }

   return extent;
}

bool OmronAOIWriter::TraversePCBDesign(BlockStruct* block, COmronAOIPCBData *pcbData, CTMatrix transformMatrix)
{
   if(!pcbData) return FALSE;     
   m_writeProcess.updateStatus("Processing PCB Board.");

   for (POSITION dataPos = (block)?block->getHeadDataInsertPosition():NULL;dataPos;)
   {  
      DataStruct* data = block->getNextDataInsert(dataPos);
      if(data)
      {
         CBasesVector insertBasesVector = data->getInsert()->getBasesVector();
         insertBasesVector.transform(transformMatrix);
         bool isMirror = data->getInsert()->getGraphicMirrored();

         if (data->getInsert()->getInsertType() == insertTypeFiducial)
         { 
#ifdef MIRROR_BOTTOM_X
            double x = insertBasesVector.getX() * ((isMirror)?(-1):1);
#else
            double x = insertBasesVector.getX();
#endif
            double y = insertBasesVector.getY();

            double rot = getRotationDegree(insertBasesVector.getRotationRadians(),isMirror);
            
            CString refname = data->getInsert()->getRefname();
            refname = ReNameFiducial(refname, pcbData->getFiducialDataList().GetCount());

            pcbData->getFiducialDataList().Add(data, getOmronAOIUnits(x), getOmronAOIUnits(y), 
               rot, refname, getPartNumber(data), blockTypePcb, isMirror);            
         }// if insertTypeFiducial
         else if (data->getInsert()->getInsertType() == insertTypePcbComponent && IsWritePart(data)) 
         {    
            if((IsLoaded(data) || GetSettings().getExportUNLoaded()) && IsWritePart(data))
            {
#ifdef MIRROR_BOTTOM_X
               double x = insertBasesVector.getX() * ((isMirror)?(-1):1);
#else
               double x = insertBasesVector.getX();
#endif
               double y = insertBasesVector.getY();

               DataStruct *centroid = centroid_exist_in_block(m_doc.getBlockAt(data->getInsert()->getBlockNumber()));;
               if (centroid)
               {
                  CBasesVector centroidBasesVector = centroid->getInsert()->getBasesVector();
                  centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());
#ifdef MIRROR_BOTTOM_X
                  x = centroidBasesVector.getX() * ((isMirror)?(-1):1);
#else
                  x = centroidBasesVector.getX();
#endif
                  y = centroidBasesVector.getY();
               }
                  
               double rot = getRotationDegree(insertBasesVector.getRotationRadians(),isMirror);
               pcbData->getComponentDataList().Add(data, getOmronAOIUnits(x), getOmronAOIUnits(y),
                  rot, data->getInsert()->getRefname(), getPartNumber(data), blockTypePcb, isMirror);
            }
            
         }//insertTypePcbComponent

      }//load
   }//while

   return TRUE;
}

bool OmronAOIWriter::TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block)
{  
   CString partNumber = "";
	CString refName = "";
   
   CString panelName = m_PCBDataList.getBoardName();
   int intFidID = 0;

   POSITION dataPos = block->getHeadDataInsertPosition();
   while (dataPos)
   {     
      DataStruct* data = block->getNextDataInsert(dataPos);
      // if i used 'data != NULL' instead of !data, nothing is written.
      InsertTypeTag insertType = (data && data->getInsert())?data->getInsert()->getInsertType():insertTypeUnknown;
      if (insertType == insertTypeFiducial)
		{
         COmronAOIPCBData *pcbData = NULL;
         if(m_PCBDataList.Lookup(panelName,pcbData) && pcbData)
         {
            bool isMirror = data->getInsert()->getGraphicMirrored();
            double rot = getRotationDegree(data->getInsert()->getAngle(),isMirror);
#ifdef MIRROR_BOTTOM_X
            double xStr = ((!isMirror)?1:-1)* getOmronAOIUnits(data->getInsert()->getOriginX());
#else
            double xStr = getOmronAOIUnits(data->getInsert()->getOriginX());
#endif
            double yStr = getOmronAOIUnits(data->getInsert()->getOriginY());
            refName = data->getInsert()->getRefname();

            switch(insertType)
            {
            case insertTypeFiducial:
               partNumber = getPartNumber(data); 	
               refName = ReNameFiducial(refName, ++intFidID);
               pcbData->getFiducialDataList().Add(data,xStr,yStr,rot,refName,partNumber,blockTypePanel,isMirror);      
               break;
            }//switch
         }
		}      
      else if(insertType == insertTypePcb)
      {		               
         COmronAOIPCBData *pcbData = NULL;
      	CPoint2d compCentroid; 
         CString l_pcbName = "";
   
         BlockStruct* pcb_Block = m_doc.getBlockAt(data->getInsert()->getBlockNumber());         
         if (pcb_Block)
         {         
            l_pcbName = data->getInsert()->getRefname();
            bool isMirror = data->getInsert()->getGraphicMirrored();
#ifdef MIRROR_BOTTOM_X
            double xStr = getOmronAOIUnits(data->getInsert()->getOriginX()) * ((isMirror)?-1:1);
#else
            double xStr = getOmronAOIUnits(data->getInsert()->getOriginX());
#endif
            double yStr = getOmronAOIUnits(data->getInsert()->getOriginY());
            double rot = getRotationDegree(data->getInsert()->getAngle(),isMirror);
            
            pcbData = new COmronAOIPCBData( m_doc, xStr, yStr,  blockTypePcb, data, pcb_Block);
            m_PCBDataList.SetAt(l_pcbName,pcbData);
         }
         
         for (POSITION insertdataPos = (pcb_Block)?pcb_Block->getHeadDataInsertPosition():NULL;insertdataPos;)
         {            
            CTMatrix l_boardMatrix(l_panelMatrix);
            TranslateBoard(l_panelMatrix, l_boardMatrix, data);                      
      
            DataStruct* insertdata = pcb_Block->getNextDataInsert(insertdataPos);
            // if i used 'insertdata != NULL' instead of !insertdata, nothing is written.
            InsertTypeTag compInsertType = (insertdata && insertdata->getInsert())?insertdata->getInsert()->getInsertType():insertTypeUnknown;
            
            if (compInsertType == insertTypePcbComponent)
			   {
               CPoint2d compXY = insertdata->getInsert()->getOrigin2d();         			
               refName  = insertdata->getInsert()->getRefname();

               if((IsLoaded(insertdata) || GetSettings().getExportUNLoaded()) && IsWritePart(insertdata))
               {
                  bool isMirror = insertdata->getInsert()->getGraphicMirrored();

                  insertdata->getInsert()->getCentroidLocation(m_doc.getCamCadData(), compCentroid);
				      l_boardMatrix.transform(compCentroid);

#ifdef MIRROR_BOTTOM_X
                  double xStr = ((!isMirror)?1:-1) * getOmronAOIUnits(compCentroid.x);
#else
                  double xStr = getOmronAOIUnits(compCentroid.x);
#endif
                  double yStr = getOmronAOIUnits(compCentroid.y);
            	   
                  double rot = getRotationDegree(insertdata->getInsert()->getAngle() + data->getInsert()->getAngle(),isMirror) ;
                  partNumber = getPartNumber(insertdata); 

                  pcbData->getComponentDataList().Add(insertdata,xStr,yStr,rot,refName,partNumber,blockTypePcb,isMirror);     
               }
			   }//insert               
         }
      }//if
   }   
   
   return TRUE;      
}

bool OmronAOIWriter::IsLoaded(DataStruct* data)
{
   bool loaded = true;

   Attrib *attrib = NULL;
	if ((attrib = is_attvalue(&m_doc, data->getAttributesRef(), ATT_LOADED, 1)) && attrib)
	{
		CString value = attrib->getStringValue();
      loaded = (value.CompareNoCase("true") == 0);
	}

   return loaded;
}

bool OmronAOIWriter::IsWritePart(DataStruct* data)
{
   bool writePart = true;
   //Check for optional filter command from OUT file settings
   if(m_attribMap.IsValid()) // if using OUT file command
   {
      //Check filter, part must have a value that is present in set of filter values
      writePart = (data->getAttributesRef() != NULL && m_attribMap.HasValue(&data->getAttributesRef()));
   }
   return writePart;
}

double OmronAOIWriter::getRotationDegree(double radious, int mirror)
{
   // Saki doc says do not alter rotation for mirror. Incoming mirror param is good, just ignore it.
   // Override incoming mirror with "not mirrored" to disable conversion is calc.
   mirror = 0;

   double insertRot = round(normalizeDegrees(RadToDeg(radious)));
   return normalizeDegrees(((mirror)?(360 -  insertRot):insertRot));
}

CString OmronAOIWriter::getPartNumber(DataStruct* data)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_doc, data->getAttributeMap(), GetSettings().getPartNumberKW(), 0);
   CString partNumber = attrib?attrib->getStringValue():"No_Part_Number";

   if ((data->getInsert()->getInsertType() == insertTypeFiducial))
      partNumber = "MARKE";

   return partNumber;
}

void OmronAOIWriter::TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data)
{
   CPoint2d boardXY = data->getInsert()->getOrigin2d();
   l_panelMatrix.transform(boardXY);

   l_boardMatrix.scale(data->getInsert()->getScale() * (data->getInsert()->getGraphicMirrored()?-1:1), data->getInsert()->getScale());
   
   l_boardMatrix.rotateRadians(data->getInsert()->getAngle());

   l_boardMatrix.translate(boardXY);
}

bool OmronAOIWriter::isUniqueBoardNameOnPanel(BlockStruct* panelblock)
{
   int uniboardcnt = 0;
   CTypedMapIntToPtrContainer<BlockStruct*>  m_boardMap;

   for(POSITION boardPos = panelblock->getHeadDataInsertPosition();boardPos;)
   {
      DataStruct* insertdata = panelblock->getNextDataInsert(boardPos);
      if(insertdata && insertdata->getInsert() && insertdata->getInsert()->getInsertType() == insertTypePcb)
      {
         BlockStruct* pcb_Block = NULL; 
         int blockNumber = insertdata->getInsert()->getBlockNumber();
         if(!m_boardMap.Lookup(blockNumber,pcb_Block))
         {
            uniboardcnt++;            
            pcb_Block = m_doc.getBlockAt(blockNumber);
            m_boardMap.SetAt(blockNumber,pcb_Block);
         }
         
         if(insertdata->getInsert()->getGraphicMirrored())
         {
            uniboardcnt++;  
         }
      }
   }  

   m_boardMap.RemoveAll();
   return (uniboardcnt > 1)?false:true;
}

CString OmronAOIWriter::ReNameFiducial(CString originalName, int intFidID)
{
   if(originalName.IsEmpty())
      originalName.Format("PFID%d", intFidID);
   
   if(originalName.GetLength() && originalName.GetAt(0) != 'P')
      originalName.Insert(0,'P');

   return originalName;
}

FileStruct *OmronAOIWriter::DetermineActiveCCFile(CCEtoODBDoc *doc)
{
	FileStruct *filefound = NULL;

	if (doc != NULL)
	{
      // check if panel or single board,
      int panelfound = 0;
      int pcbfound   = 0;


      POSITION pos = doc->getFileList().GetHeadPosition();

      while (pos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(pos);

         if (file->getBlockType() == BLOCKTYPE_PANEL)   
         {
            if (file->isShown())
				{
					filefound = file;
					panelfound++;
				}
         }

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            if (file->isShown())
				{
					filefound = file;
					pcbfound++;
				}
         }
      }

      if (panelfound > 1)
      {
         ErrorMessage("Multiple Visible Panel files found. Only one visible Panel allowed.","Too many Panels");
         return NULL;
      }

      if (pcbfound > 1)
      {
         ErrorMessage("Multiple Visible PCB files found. Only one visible PCB allowed.","Too many PCBs");
         return NULL;
      }

      if (!panelfound && !pcbfound)
      {
         ErrorMessage("No Visible Panel or PCB file found.", "No Visible File");
         return NULL;
      }

      if (panelfound && pcbfound)
      {
         ErrorMessage("Exporter requires one visible Panel or PCB File.", "Too many visible Files");
         return NULL;
      }
	}

   // If still here then we found the one we need.
   this->m_activeFile = filefound;

	return filefound;
}


//==========================================================================


NewOmronAOIWriter::NewOmronAOIWriter(CCEtoODBDoc &doc, double l_Scale)
: BaseAoiExporter(doc, l_Scale, fileTypeOmronAOI, new OmronAOISettings(doc), "Omron.log")
, m_activeFile(NULL)
, m_compRecordNum(1)
//*rcf , m_compAnalyzer(doc)
{
   SetUseComponentCentroids(true);  // For component locations, instead of insert location.

   SetMirrorBottomX(true);
   SetMirrorBottomRotation(false);

   // For fiducial verification -- turn off.
   SetMinFiducialCount(0);
   SetMaxFiducialCount(0);

   // Cache this KW so we don't have to keep looking it up during output
   m_deviceTypeKW = doc.getCamCadData().getAttributeKeywordIndex(standardAttributeDeviceType);
}

NewOmronAOIWriter::~NewOmronAOIWriter()
{
}

int NewOmronAOIWriter::GetDecimals(PageUnitsTag units)

{
   // Omron output is required to be 1/10 mm units.
   // Numbers output are integers, no decimals.

   return 0;
}

bool NewOmronAOIWriter::WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName)
{
   if(!pcbData) return FALSE;

   GetProgressBar().updateStatus("Writing PCB Files.");

   bool stat1 = WritePCBFile(pcbData, boardName, false); // top
   bool stat2 = WritePCBFile(pcbData, boardName, true ); // bot

   return (stat1 && stat2);
}

bool NewOmronAOIWriter::WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom)
{   
   if(!pcbData) return FALSE;

   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName = CreateOutfileName(m_FileName, sideSuffix, ".txt");

   CFormatStdioFile File;
   if (!File.Open(fileName, CFile::modeCreate|CFile::modeWrite))
	{      
      ErrorMessage(fileName, "Output file could not be opened", MB_OK);
      return FALSE;
   }

   // Write the header
   WriteHeader(File, isBottom);

   // Write Fids and Components
   m_compRecordNum = 1;  // reset output index
   m_usedRefnameMap.RemoveAll();
   WriteFiducialSetList(File, 1, pcbData, blockTypePcb, isBottom);
   WriteComponentList(File, pcbData, isBottom);
      
   File.Close();

   return TRUE;
}

void  NewOmronAOIWriter::WriteHeader(CFormatStdioFile &File, bool isBottom)
{
   CTime dateTime = CTime::GetCurrentTime();
   
   double boardSizeX = 0.;
   double boardSizeY = 0.;
   double boardOriginX = 0.;
   double boardOriginY = 0.;
   FileStruct* fileStruct = this->getActiveFile();
   CExtent boardExtent = this->GetOutlineExtent(fileStruct);
   if (boardExtent.isValid())
   {
      boardSizeX = boardExtent.getXsize();
      boardSizeY = boardExtent.getYsize();

      this->setOutlineLowerLeft(getOmronAOIUnits(boardExtent.getXmin()), getOmronAOIUnits(boardExtent.getYmin()));
   }

   File.WriteString("1:%16.16s\n", this->m_PCBDataList.getBoardName());
   File.WriteString("2:%3.3s\n", "FLS");

   // PCB Size, no sign, up to 5 digits, no delimiter between values (not even white space)

   // Issue warning in log if board size exceeds apparant maximum.
   // The spec does not appear to contain explicit statement of maximum, may be that it varies by
   // machine, not specific to format. Format limit is 5 digits, so use 99999 for now.
   if (getOmronAOIUnits(boardSizeX) > 99999 || getOmronAOIUnits(boardSizeY) > 99999)
   {
      CString msg;
      msg.Format("Warning: Board size %d x %d exceeds Omron format maximum of 5 digits. [99999 x 99999]",
         getOmronAOIUnits(boardSizeX), getOmronAOIUnits(boardSizeY));
      m_textLogFile.writef(msg);
   }

   File.WriteString("3:%5d%5d\n", getOmronAOIUnits(boardSizeX), getOmronAOIUnits(boardSizeY));

   
   // Rail Width can be specificed explicitly in settings file using .RAIL_WIDTH xx command.
   // That value would be used as-is.
   // Or a margin can be specified using .RAIL_WIDTH_MARGIN, this value is added to the board X
   // dimension to get Rail Width for output. (So it is specific already in output units.)
   // Use of RAIL_WIDTH takes precedence over RAIL_WIDTH_MARGIN.
   // If neither are set the default rail width is same as PCB X dimension.
   int railWidth = (int)getOmronAOIUnits(boardSizeX); // Default
   if (GetSettings()->getRailWidth() > 0)
      railWidth = (int)GetSettings()->getRailWidth();
   else
      railWidth += (int)GetSettings()->getRailWidthMargin();
   File.WriteString("4:%5d\n", railWidth); 

   // Spec says blank lines are ignored. Samples have a blank line between header
   // and the rest, so we write a blank line at the end of the header too.
   File.WriteString("\n");
}

void NewOmronAOIWriter::WriteComponentList(CFormatStdioFile &File, BaseAoiExporterPCBData *pcbData, bool isBottom)
{
   if(pcbData)
   {
      WriteComponents(File, 1, pcbData,isBottom);
   }
   else
   {
      int boardIdx = 0;
      for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
      {
         CString pcbName;
         m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
         if(pcbData->getBoardType() == blockTypePcb)
         {
            WriteComponents(File, ++boardIdx, pcbData, isBottom);         
         }
      }
   }//if
}


void NewOmronAOIWriter::WriteComponents(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, bool isBottom)
{
   int compCnt = (pcbData)?pcbData->getComponentDataList().GetCount():0;
   CString PCBSide = (isBottom)?"Bottom":"Top";

   for(int compIndex = 0; compIndex < compCnt; compIndex++)
   {
      BaseAoiExporterInsertData *cmpdata = pcbData->getComponentDataList().GetAt(compIndex);
      if(cmpdata && cmpdata->getInsertData() && cmpdata->getPlacedBottom() == isBottom)
      {
         WriteComponentRecord(File, boardIdx, cmpdata, isBottom);
      }
   }//for 
}

void  NewOmronAOIWriter::WriteComponentRecord(CFormatStdioFile &File, int boardIdx, BaseAoiExporterInsertData *sakiCmpData, bool isBottom)
{
   if (sakiCmpData != NULL)
   {
      CString xStr,yStr,rotStr,attribute;

      // X and Y, up to 5 digits, plus sign, for total of 6 chars output.
      xStr.Format("%6d", round(sakiCmpData->getX()));
      yStr.Format("%6d", round(sakiCmpData->getY()));

      // Rotation, spec says 3 digits, range 000 to 300. Surely they meant 000 to 360.
      rotStr.Format("%3d", round(sakiCmpData->getRotation()));
      
      File.WriteString("%s", xStr);       // X
      File.WriteString("%s", yStr);       // Y
      File.WriteString("%s", rotStr);     // Theta

      File.WriteString("%-10.10s", GetUniqueName(sakiCmpData->getRefName()));  // Comp ref up to 10 chars

      // Component Number
      // Value output is either inserted geomtry's name or attrib val selected by user.
      // Up to 16 chars, left justified.
      File.WriteString("%-16.16s\n", getComponentNumberValue(sakiCmpData));   
   }
}

void NewOmronAOIWriter::WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom)
{
   int fidCnt = (pcbData)?pcbData->getFiducialDataList().GetCount():0;

   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex++)
   {
      BaseAoiExporterInsertData *fiddata = pcbData->getFiducialDataList().GetAt(fidIndex);
      if(fiddata && fiddata->getInsertData() && fiddata->getBoardType() == boardType && fiddata->getPlacedBottom() == isBottom)
      {         
         WriteComponentRecord(File, boardIdx, fiddata, isBottom);
      }
   }   
}

//*rcf This activeFile support needs moving to base class
FileStruct *NewOmronAOIWriter::DetermineActiveCCFile(CCEtoODBDoc *doc)
{
	FileStruct *filefound = NULL;

	if (doc != NULL)
	{
      // check if panel or single board,
      int panelfound = 0;
      int pcbfound   = 0;


      POSITION pos = doc->getFileList().GetHeadPosition();

      while (pos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(pos);

         if (file->getBlockType() == BLOCKTYPE_PANEL)   
         {
            if (file->isShown())
				{
					filefound = file;
					panelfound++;
				}
         }

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            if (file->isShown())
				{
					filefound = file;
					pcbfound++;
				}
         }
      }

      if (panelfound > 1)
      {
         ErrorMessage("Multiple Visible Panel files found. Only one visible Panel allowed.","Too many Panels");
         return NULL;
      }

      if (pcbfound > 1)
      {
         ErrorMessage("Multiple Visible PCB files found. Only one visible PCB allowed.","Too many PCBs");
         return NULL;
      }

      if (!panelfound && !pcbfound)
      {
         ErrorMessage("No Visible Panel or PCB file found.", "No Visible File");
         return NULL;
      }

      if (panelfound && pcbfound)
      {
         ErrorMessage("Exporter requires one visible Panel or PCB File.", "Too many visible Files");
         return NULL;
      }
	}

   // If still here then we found the one we need.
   this->m_activeFile = filefound;

	return filefound;
}



CString NewOmronAOIWriter::RenameFiducial(CString fidname, int intFidID, bool isPanelFid)
{
   if (fidname.IsEmpty())
      fidname.Format("PFID%d", intFidID);
   
   if (fidname.GetLength() && fidname.GetAt(0) != 'P')
      fidname.Insert(0,'P');

   return fidname;
}

CString NewOmronAOIWriter::getComponentNumberValue(BaseAoiExporterInsertData *sakiCmpData)
{
   CString macroKeyword = GetSettings()->getComponentNumberKeyword();
   return GetConfigurableValue(sakiCmpData, macroKeyword);
}
  

bool NewOmronAOIWriter::WritePanelFiles(BaseAoiExporterPCBData *panelData)
{
   GetProgressBar().updateStatus("Writing Panel Files.");

   BlockStruct *panelBlock = panelData->getGeometryBlock();

   //Find mixed PCB boards in Panel
   // bool stat3 = TraversePCBsInPanel(panelBlock);

   //Verify Fiducials for PCB
   if(!VerifyFiducials(blockTypePcb))
      return false;

   bool stat1 = WritePanelFile(panelData, false); // top
   bool stat2 = WritePanelFile(panelData, true ); // bot

   return stat1 && stat2;// && stat3;
   //-----------

   return (stat1 && stat2);
}

bool NewOmronAOIWriter::WritePanelFile(BaseAoiExporterPCBData *pcbData, bool isBottom)
{  
   if(!m_PCBDataList.IsPanel())
      return FALSE;

   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName = CreateOutfileName(m_FileName, sideSuffix, ".txt");

   CFormatStdioFile File;
   if (!File.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage(fileName, "Output file could not be opened.", MB_OK);
      return FALSE;
   }

   // Write the header
   WriteHeader(File, isBottom);
   
   // Write Fids and Components
   m_compRecordNum = 1;  // reset output index
   m_usedRefnameMap.RemoveAll();
   WriteFiducialSetList(File, 1, pcbData, blockTypePanel, isBottom);
   WriteComponentList(File, NULL, isBottom);   

   File.Close();

   return TRUE;
}
 

////////////////////////////////////////////////////////////
// CSakiAOISettings
////////////////////////////////////////////////////////////
OmronAOISettings::OmronAOISettings(CCEtoODBDoc& doc)
: BaseAoiExporterSettings("OmronAOI.out", doc)
{
   SetLocalDefaults();
}

OmronAOISettings::~OmronAOISettings()
{
}

void OmronAOISettings::SetLocalDefaults()
{
   // From base settings
   m_PartNumberKW = ATT_PARTNUMBER;
   m_outputPageUnits = pageUnitsMilliMeters;
   m_outputPageUnitsScale = 10.0;  // Units are 1/10 of mm, so scale mm page units by 10 to get Omron units
   m_exportUNLoaded = false;

   // Local settings
   m_attribKeyword.Empty();
   m_attribValue.RemoveAll();

   m_attribKeyword.Empty();
   m_attribValue.RemoveAll();
   m_componentNumberValueFrom = QGEOM_NAME;

   m_railWidth = -1;       // Means not set, only positive values are considered set.
   m_railWidthMargin = 0;  // Means no margin, margin may be pos or neg, but expected usually pos.
}

bool OmronAOISettings::ParseLocalCommand(CInFile &inFile)
{
   bool isLocalCmd = false;

   if (inFile.isCommand(".OUTPUT_ATTRIBUTE",2)) 
   {
      m_attribKeyword = inFile.getParam(1);
      isLocalCmd = true;
   }
   else if (inFile.isCommand(".OUTPUT_VALUE",2))
   {
      m_attribValue.Add(inFile.getParam(1));
      isLocalCmd = true;
   }
   else if (inFile.isCommand(".COMPONENT_NUMBER_KEYWORD",2))
   {
      m_componentNumberValueFrom = inFile.getParam(1);
      isLocalCmd = true;
   }
   else if (inFile.isCommand(".RAIL_WIDTH", 2))
   {
      m_railWidth = inFile.getIntParam(1);
      isLocalCmd = true;
   }
   else if (inFile.isCommand(".RAIL_WIDTH_MARGIN", 2))
   {
      m_railWidthMargin = inFile.getIntParam(1);
      isLocalCmd = true;
   }

   return isLocalCmd;
}
