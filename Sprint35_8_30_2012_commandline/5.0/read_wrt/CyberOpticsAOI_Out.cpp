
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
#include "MultipleMachine.h"
#include "CyberOpticsAOI_Out.h"

#define MIRROR_BOTTOM_X

/*****************************************************************************/

void CyberOpticsAOI_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
      CyberOpticsAOIWriter writer(doc, format->Scale);

      if (writer.DetermineActiveCCFile(&doc) != NULL)
      {
         if (writer.EnforceLineConfigSettings(doc))
         {
            if(!writer.WriteFiles(filename))
            {
               ErrorMessage("CAMCAD could not finish writing the CyberOptics AOI output files.","", MB_OK);
            }
         }
      }
}

/*****************************************************************************/

static CString getCyberOpticsAOIDefaultLocation(bool isBottom)
{
   CString Location;
   Location.Format("%s,0.0,0.0,0,0,%d",(isBottom)?"LowerRight":"LowerLeft",fileTypeCyberOpticsAOI);   
   return Location;
}

////////////////////////////////////////////////////////////
// CyberOpticsAOIOutputAttrib
////////////////////////////////////////////////////////////
CyberOpticsAOIOutputAttrib::CyberOpticsAOIOutputAttrib(CCEtoODBDoc& camCadDoc) 
:  m_camCadDoc(camCadDoc)
,  m_attribKeyword(-1)
{
	SetDefaultAttribAndValue();
}

CyberOpticsAOIOutputAttrib::~CyberOpticsAOIOutputAttrib()
{
	m_valuesMap.RemoveAll();
}

int CyberOpticsAOIOutputAttrib::GetKeyword() const
{
	return m_attribKeyword;
}

bool CyberOpticsAOIOutputAttrib::HasValue(CAttributes** attributes)
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

void CyberOpticsAOIOutputAttrib::SetKeyword(CString keyword)
{
   if(!keyword.IsEmpty())
   {
      m_attribKeyword = m_camCadDoc.RegisterKeyWord(keyword, 0, VT_STRING); 
      m_valuesMap.RemoveAll();
   }
}

void CyberOpticsAOIOutputAttrib::AddValue(CString value)
{
	CString tmpValue = value;
	m_valuesMap.SetAt(tmpValue.MakeLower(), tmpValue.MakeLower());
}

void CyberOpticsAOIOutputAttrib::SetDefaultAttribAndValue()
{

}

////////////////////////////////////////////////////////////
// CCyberOpticsAOISetting
////////////////////////////////////////////////////////////
CCyberOpticsAOISettings::CCyberOpticsAOISettings(CCEtoODBDoc& doc)
: BaseAoiExporterSettings("CyberOpticsAOI.out", doc)
{
   SetLocalDefaults();
}

CCyberOpticsAOISettings::~CCyberOpticsAOISettings()
{
}

void CCyberOpticsAOISettings::SetLocalDefaults()
{
   m_PartNumberKW = ATT_PARTNUMBER;
   m_outputPageUnits = pageUnitsMilliMeters;
   m_exportUNLoaded = false;

   m_scopeIndentSize = INDENT_SIZE_DEFAULT;
}

bool CCyberOpticsAOISettings::ParseLocalCommand(CInFile &inFile)
{
   bool isLocalCmd = true;

   if (inFile.isCommand(".INDENT",2))
   {
      m_scopeIndentSize = inFile.getIntParam(1);
   }
   else
   {
      isLocalCmd = false;
   }

   return isLocalCmd;
}

////////////////////////////////////////////////////////////
// CCyberOpticsAOIInsertData
////////////////////////////////////////////////////////////
CCyberOpticsAOIInsertData::CCyberOpticsAOIInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data)
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

void CCyberOpticsAOIInsertData::setRotation(double rotation)
{
   m_rotation = rotation;
}

////////////////////////////////////////////////////////////
// CCyberOpticsAOIInsertDataList
////////////////////////////////////////////////////////////
CCyberOpticsAOIInsertDataList::CCyberOpticsAOIInsertDataList(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_topInserts(0)
, m_botInserts(0)
{
}

CCyberOpticsAOIInsertDataList::~CCyberOpticsAOIInsertDataList()
{
   m_topInserts = 0;
   m_botInserts = 0;
   empty();
}

void CCyberOpticsAOIInsertDataList::Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, 
                                CString partNumber, BlockTypeTag boardType, bool placedBottom)
{
   CCyberOpticsAOIInsertData *data = new CCyberOpticsAOIInsertData(m_camCadDoc, InsertData);
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
#ifdef USE_CENTROID
      DataStruct *centroidData = centroid_exist_in_block(&m_camCadDoc, geometryBlock);  
      double CentroidRotation = 0.;
      if(centroidData)
      {
         //GenerateCentroidRotationOffset(geometryBlock, centroidData, CentroidRotation);
         rotation += centroidData->getInsert()->getAngleDegrees();
      }
#endif

      if(placedBottom)
         m_botInserts ++;
      else
         m_topInserts ++;
   }  

   data->setRotation(normalizeDegrees(rotation));
   
   SetAtGrow(GetCount(), data);
}

////////////////////////////////////////////////////////////
// CCyberOpticsAOIPCBData
////////////////////////////////////////////////////////////
CCyberOpticsAOIPCBData::CCyberOpticsAOIPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry)
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

CCyberOpticsAOIPCBData::~CCyberOpticsAOIPCBData()
{
   m_FidDataList.empty();
   m_CompDataList.empty();
}

////////////////////////////////////////////////////////////
// CCyberOpticsAOIPCBDataList
////////////////////////////////////////////////////////////
CCyberOpticsAOIPCBDataList::CCyberOpticsAOIPCBDataList()
{
   m_Panelflag = false;
   m_BoardName = QCYBERO_BOARDNAME;
}

CCyberOpticsAOIPCBDataList::~CCyberOpticsAOIPCBDataList()
{
   empty();
}

void CCyberOpticsAOIPCBDataList::empty()
{
   m_Panelflag = false;
   m_BoardName = QCYBERO_BOARDNAME;

   for(POSITION pcbPos = GetStartPosition(); pcbPos; )
   {
      CString pcbName;
      CCyberOpticsAOIPCBData *pcbData =NULL;
      GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData)
         delete pcbData;
   }
   RemoveAll();
}

void CCyberOpticsAOIPCBDataList::SetBoardAt(CString boardName, CCyberOpticsAOIPCBData *pcbdata)
{
   CCyberOpticsAOIPCBData *existdata = NULL;

   if(boardName.IsEmpty()) 
   {
      boardName.Format("%s_$%d", QCYBERO_BOARDNAME, GetCount());
   }
   else if(Lookup(boardName,existdata))
   {
      boardName.Format("%s_$%d", boardName, GetCount());
   }

   SetAt(boardName,pcbdata);
}

////////////////////////////////////////////////////////////
// CyberOpticsAOIWriter
////////////////////////////////////////////////////////////
CyberOpticsAOIWriter::CyberOpticsAOIWriter(CCEtoODBDoc &doc, double l_Scale)
: BaseAoiExporter(doc, l_Scale, fileTypeCyberOpticsAOI, new CCyberOpticsAOISettings(doc), "CyberOpticsAOI.log")
, m_doc(doc)
, m_Scale(l_Scale)
, m_srffTop(doc)
, m_srffBot(doc)
{
   FileStruct *activeFile = this->DetermineActiveCCFile(&doc);
   if (activeFile != NULL)
   {
      // I don't know why,, but this seems to make it work.
      // Would be good to figure out why, probably it is some bug, seems the setting should
      // be the same for top and bottom.
      if (activeFile->getBlockType() == blockTypePcb)
         this->SetMirrorBottomX(true);
      else
         this->SetMirrorBottomX(false);
   }
   
   this->Flatten(false);  // SRF retains hierarchy
   this->SetUseComponentCentroids(false);  // Use the comp origin and insert as-is, don't adjust to centroid location.
   this->DoUnitsConversion(false); // Units conversion is handled locally, turn off units conversion in base.

   // Cache this KW so we don't have to keep looking it up during output
   m_deviceTypeKW = doc.getCamCadData().getAttributeKeywordIndex(standardAttributeDeviceType);
}

CyberOpticsAOIWriter::~CyberOpticsAOIWriter(void)
{
   m_textLogFile.close();
   m_usedRefnameMap.RemoveAll();
}

void CyberOpticsAOIWriter::GetLineConfigSettings(FileStruct* fileStruct, bool isBottom, ExportFileSettings &exportSettings)
{
   Attrib *attrib =  is_attvalue(&m_doc, fileStruct->getBlock()->getDefinedAttributes(), GetMachineAttributeName(fileTypeCyberOpticsAOI,isBottom), 0);
   exportSettings = (attrib)?attrib->getStringValue():getCyberOpticsAOIDefaultLocation(isBottom);
}

void CyberOpticsAOIWriter::GenerateOutputFilenames(CString incomingName, CString &topSideName, CString &botSideName)
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

int CyberOpticsAOIWriter::GetDecimals(PageUnitsTag pageUnits)
{
   // Decimal places for output in given units.

   int decimals = 5; // default. 
   
   switch (pageUnits)
   {
   case pageUnitsInches:         decimals = 3; break;
   case pageUnitsMilliMeters:    decimals = 2; break;
   case pageUnitsMils:           decimals = 0; break;
   case pageUnitsMicroMeters:    decimals = 0; break;
      break;
   }
   return decimals;
}


CString CyberOpticsAOIWriter::RenameFiducial(CString fidname, int intFidID, bool isPanelFid)
{
   if (fidname.IsEmpty())
      fidname.Format("%sFID%d", isPanelFid ? "P" : "B", intFidID);

   return fidname;
}


bool CyberOpticsAOIWriter::WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName)
{
   if(!pcbData) return FALSE;
   FileStruct* fileStruct = this->GetActiveFile();

   m_writeProcess.updateStatus("Writing PCB Files.");

   //ApplyPrepSettingToFiles(fileStruct, pcbData, false, false);
   bool stat1 = WritePCBFile(pcbData, boardName, false); // top

   //ApplyPrepSettingToFiles(fileStruct, pcbData, true, false);
   bool stat2 = WritePCBFile(pcbData, boardName, true ); // bot

   return (stat1 && stat2);
}

bool CyberOpticsAOIWriter::WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom)
{   
   if(!pcbData) return FALSE;

   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName( CreateOutfileName(m_FileName, sideSuffix, ".srf") );

   CFormatStdioFile File;
   if (!File.Open(fileName, CFile::modeCreate|CFile::modeWrite))
	{      
      ErrorMessage(fileName, "Output file could not be opened", MB_OK);
      return FALSE;
   }

   InitSrffWriter(File, isBottom);

   // SRF Image List - One image for a single PCB output.
   FillImageList(NULL, pcbData, isBottom);

   // Write Fids and Components
   m_usedRefnameMap.RemoveAll();
   
   GetSrffWriter(isBottom).WriteFile(File);
   File.Close();

   return TRUE;
}

bool CyberOpticsAOIWriter::WritePanelFile(BaseAoiExporterPCBData *panelData, bool isBottom)
{  
   if(!m_PCBDataList.IsPanel())
      return FALSE;

   //*rcf Put a GetOutputFilename func in base class   CString fileName( this->getOutputFilename(isBottom) );
   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName( CreateOutfileName(m_FileName, sideSuffix, ".srf") );

   CFormatStdioFile File;
   if (!File.Open(fileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage(fileName, "Output file could not be opened.", MB_OK);
      return FALSE;
   }

   InitSrffWriter(File, isBottom);

   // SRFF Image List
   FillImageList(panelData, NULL, isBottom);
   
   // Write Fids and Components
   m_usedRefnameMap.RemoveAll();

   GetSrffWriter(isBottom).WriteFile(File);
   File.Close();

   return TRUE;
}
   
double CyberOpticsAOIWriter::applySakiOriginX(double rawSakiX)
{
   // Incoming rawSaki coord is in Saki units, but relative to original CCZ origin.
   // Make it relative to specified Saki origin, which itself is relative to panel corner.

   // Panel corner in Saki units plus specific origin (offset) in settings file is origin for output.
   double sakiOrigin = this->m_outlineLLX;// + this->GetSettings().getBoardOriginX();
   double finalVal = rawSakiX - sakiOrigin;

   return finalVal;
}

double CyberOpticsAOIWriter::applySakiOriginY(double rawSakiY)
{
   // Incoming rawSaki coord is in Saki units, but relative to original CCZ origin.
   // Make it relative to specified Saki origin, which itself is relative to panel corner.

   // Panel corner in Saki units plus specific origin (offset) in settings file is origin for output.
   double sakiOrigin = this->m_outlineLLY;// + this->GetSettings().getBoardOriginY();
   double finalVal = rawSakiY - sakiOrigin;

   return finalVal;
}

void  CyberOpticsAOIWriter::InitSrffWriter(CFormatStdioFile &File, bool isBottom)
{
   SRFFWriter &srffWriter( this->GetSrffWriter(isBottom) );

   srffWriter.SetScopeIndentSize( GetSettings()->GetScopeIndentSize() );
   srffWriter.SetOutputUnits( GetSettings()->getOutputPageUnits() );
   
   srffWriter.SetProductName(GetActiveFile()->getName());
   srffWriter.SetProductNote("No product note.");
   
   double boardSizeX = 0.;
   double boardSizeY = 0.;
   double boardOriginX = 0.;
   double boardOriginY = 0.;
   CExtent boardExtent = this->GetOutlineExtent();
   if (boardExtent.isValid())
   {
      boardSizeX = boardExtent.getXsize();
      boardSizeY = boardExtent.getYsize();
   }
/*
   CExtent boardExtent = this->GetOutlineExtent();
   if (boardExtent.isValid())
   {
      boardSizeX = boardExtent.getXsize();
      boardSizeY = boardExtent.getYsize();

      double outlineMinX = boardExtent.getXmin();
      double outlineMaxX = boardExtent.getXmax();

      // For top side use outline LL at face value.
      // For bottom side use outline LR. We will mirror X for bottom later.
      if (!isBottom)
         this->setOutlineLowerLeft(getCyberOpticsAOIUnits(boardExtent.getXmin()), getCyberOpticsAOIUnits(boardExtent.getYmin()));
      else
         this->setOutlineLowerLeft(getCyberOpticsAOIUnits(-boardExtent.getXmax()), getCyberOpticsAOIUnits(boardExtent.getYmin()));
   }
   else
   {
      int jj = 0; //*rcf Extent failure
   }
*/

   srffWriter.SetPanelSize(boardSizeX, boardSizeY);

   // Issue warning in log if board size exceeds apparant maximum (maximum is hearsay from field rep).
#ifdef BOARD_SIZE_LIMIT_CHECK // Limit for Cyberoptics is not known. Once known, could activate this check.
   if (getCyberOpticsAOIUnits(boardSizeX) > 330.0 || getCyberOpticsAOIUnits(boardSizeY) > 250.0)
   {
      CString msg;
      msg.Format("Warning: Board size %.3f x %.3f exceeds Saki maximum 330 x 250.",
         getCyberOpticsAOIUnits(boardSizeX), getCyberOpticsAOIUnits(boardSizeY));
      m_textLogFile.writef(msg);
   }
#endif

}

void CyberOpticsAOIWriter::FillImageList(BaseAoiExporterPCBData *panel, BaseAoiExporterPCBData *pcbData, bool isBottom)
{
   SRFFWriter &srffWriter( this->GetSrffWriter(isBottom) );

   if(pcbData)
   {
      AddImageForPanelData(srffWriter, NULL, isBottom);
      AddImageForPcbData(srffWriter, pcbData, isBottom);
   }
   else
   {
      AddImageForPanelData(srffWriter, panel, isBottom);
      int boardIdx = 0;
      for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
      {
         CString pcbName;
         m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
         AddImageForPcbData(srffWriter, pcbData, isBottom);
      }
   }
}

void CyberOpticsAOIWriter::AddImageForPcbData(SRFFWriter &srffWriter, BaseAoiExporterPCBData *pcbData, bool isBottom)
{
   if(pcbData->getBoardType() == blockTypePcb)
   {
      BlockStruct *block = pcbData->getGeometryBlock();

      bool insertMirrored = pcbData->getMirrored();
      bool resultingMirror = insertMirrored ^ isBottom;

      // Add image definition first, we need it's ID for the image instance.
      SrffImageDefinition *imageDef = srffWriter.LookupImageDefinition(block, resultingMirror);
      if (imageDef == NULL)
      {
         imageDef = srffWriter.AddImageDefinition(block, resultingMirror); // Use resultingMirror to get actual visible pcb side.
         CollectComponentDefinitions(pcbData, isBottom); // Use isBottom to convey which srff output file.
         AddImageDefCompLocations(imageDef, pcbData, isBottom); // Use isBottom to convey which srff output file.

         BlockTypeTag brdTyp = pcbData->getBoardType();  //*rcf Extra, not used

         CollectFiducials(imageDef, pcbData, pcbData->getBoardType(), isBottom);
      }

      //FileStruct *activeFile = this->getActiveFile();
      ///ExportFileSettings pcbLocation;
      ////m_PCBLocation = (attrib)?attrib->getStringValue():getCyberOpticsAOIDefaultLocation(isBottom);
      ///GetLineConfigSettings(activeFile, isBottom, pcbLocation);


      // Add image instance.
      CString refname( pcbData->getRefname() );
      double raw_pcbX = pcbData->getOriginX();
      double raw_pcbY = pcbData->getOriginY();
      double pcbX = raw_pcbX;
      double pcbY = raw_pcbY;
      double pcbRot = pcbData->getRotDeg();
      if (isBottom)
         pcbRot = 360. - pcbRot;

      srffWriter.AddImage(block->getBlockNumber(), refname, pcbX, pcbY, pcbRot, imageDef->GetId());
   }
}

void CyberOpticsAOIWriter::AddImageForPanelData(SRFFWriter &srffWriter, BaseAoiExporterPCBData *panelData, bool isBottom)
{
   if (panelData != NULL)
   {
      if(panelData->getBoardType() == blockTypePanel)
      {
         BlockStruct *block = panelData->getGeometryBlock();

         // Add image definition first, we need it's ID for the image instance.
         SrffImageDefinition *imageDef = srffWriter.LookupImageDefinition(block, false);
         if (imageDef == NULL)
         {
            imageDef = srffWriter.AddImageDefinition(block, false);
         }

         // Add image instance.
         CString refname( panelData->getRefname() );
         if (refname.IsEmpty())
            refname = "Panel";
         double pcbX = panelData->getOriginX();
         double pcbY = panelData->getOriginY();
         double pcbRot = panelData->getRotDeg();
         srffWriter.AddImage(block->getBlockNumber(), refname, pcbX, pcbY, pcbRot, imageDef->GetId());

         CollectFiducials(imageDef, panelData, panelData->getBoardType(), isBottom);
      }
   }
   else
   {
      // Probably single PCB output, a panel image is still required, so fake it.
      int fakeBlockNumber = -10203;
      SrffImageDefinition *imageDef = srffWriter.AddImageDefinition(fakeBlockNumber, "Panel", false);
      srffWriter.AddImage(fakeBlockNumber, "Panel", 0., 0., 0., imageDef->GetId());
   }
}

void CyberOpticsAOIWriter::AddImageDefCompLocations(SrffImageDefinition *imageDef, BaseAoiExporterPCBData *pcbData, bool isBottom)
{
   SRFFWriter &srffWriter( GetSrffWriter(isBottom) );
   int compCnt = (pcbData)?pcbData->getComponentDataList().GetCount():0;
   //CString PCBSide = (isBottom)?"Bottom":"Top";

   // The isBottom identifies which srff output file we're processing.
   // The resultingMirror tells us which PCB side is visible for this srff file.
   // E.g. hangles mirrored pcb insert in panel.
   bool pcbInsertMirrored = pcbData->getMirrored();
   bool resultingMirror = pcbInsertMirrored ^ isBottom;

   for(int compIndex = 0; compIndex < compCnt; compIndex++)
   {
      BaseAoiExporterInsertData *cmpdata = pcbData->getComponentDataList().GetAt(compIndex);
      if(cmpdata && cmpdata->getInsertData() && cmpdata->getPlacedBottom() == resultingMirror)
      {
         double x = applySakiOriginX(cmpdata->getX());  //*rcf todo Coords are messed up, wrong units, etc.
         double y = applySakiOriginY(cmpdata->getY());
         double rot = cmpdata->getRotation();
         CString refname(cmpdata->getRefName());

         if (resultingMirror)
            x = -x;
         if (resultingMirror)
            rot = 360. - rot;

         int compDefId = -1;
         SrffComponentDefinition *compDef = srffWriter.LookupComponentDefinition(cmpdata->getPartNumber());
         if (compDef != NULL)
            compDefId = compDef->GetId();

         imageDef->AddLocation((compIndex+1), refname, x, y, rot, compDefId);
      }
   } 
}

void CyberOpticsAOIWriter::CollectComponentDefinitions(BaseAoiExporterPCBData *pcbData, bool isBottom)
{
   // Builds component definition and component link lists.
   // The component definition references the component link, so create new
   // links first, so new defs can reference them.

   SRFFWriter &srffWriter( GetSrffWriter(isBottom) );
   int compCnt = (pcbData)?pcbData->getComponentDataList().GetCount():0;

   // The isBottom identifies which srff output file we're processing.
   // The resultingMirror tells us which PCB side is visible for this srff file.
   // E.g. hangles mirrored pcb insert in panel.
   bool pcbInsertMirrored = pcbData->getMirrored();
   bool resultingMirror = pcbInsertMirrored ^ isBottom;

   for(int compIndex = 0; compIndex < compCnt; compIndex++)
   {
      BaseAoiExporterInsertData *cmpdata = pcbData->getComponentDataList().GetAt(compIndex);
      if(cmpdata && cmpdata->getInsertData() && cmpdata->getPlacedBottom() == resultingMirror)
      {
         CString partnumber( cmpdata->getPartNumber() );
         BlockStruct *insertedBlock = cmpdata->GetInsertedBlock();
         
         SrffComponentLink *compLink = srffWriter.AddComponentLink(insertedBlock);
         int compLinkId = (compLink == NULL) ? -1 : compLink->GetId();
         //*rcf Add log message for missing compLink

         SrffComponentDefinition *compDef = srffWriter.AddComponentDefinition(partnumber, compLinkId);

         CString patternName( compLink->GetPackageName() );
         SrffPatternDefinition *patDef = srffWriter.AddPatternDefinition(insertedBlock, patternName, compLinkId);
      }
   } 
}

void CyberOpticsAOIWriter::CollectFiducials(SrffImageDefinition *imageDef, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom)
{
   int fidCnt = (pcbData)?pcbData->getFiducialDataList().GetCount():0;

   // The isBottom identifies which srff output file we're processing.
   // The resultingMirror tells us which PCB side is visible for this srff file.
   // E.g. hangles mirrored pcb insert in panel.
   bool pcbInsertMirrored = pcbData->getMirrored();
   bool resultingMirror = pcbInsertMirrored ^ isBottom;

   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex++)
   {
      BaseAoiExporterInsertData *fiddata = pcbData->getFiducialDataList().GetAt(fidIndex);
      //*rcf debug ---
      BlockTypeTag bt = fiddata->getBoardType();
      bool eqs = (bt == boardType);
      if (eqs)
      {
         int jj = 0;
      }
      else
      {
         int jj = 0;
      }
      //-----------end-debug---

      if(fiddata && fiddata->getInsertData() && fiddata->getBoardType() == boardType && fiddata->getPlacedBottom() == resultingMirror)
      { 
         int nextId = this->GetSrffWriter(isBottom).GetNextImageFiducialId(); // imageDef->GetFiducialCount() + 1;

         CString fidRefname(fiddata->getRefName());
         if (fidRefname.IsEmpty())
            fidRefname.Format("Fiducial_%03d", nextId);

         DataStruct *data = fiddata->getInsertData();
         InsertStruct *insert = (data) ? (data->getInsert()) : NULL;
         BlockStruct *insertedBlock = (insert) ? (m_doc.getBlockAt(insert->getBlockNumber())) : NULL;
         int shapeId = -1;
         if (insertedBlock != NULL)
         {
            if (insertedBlock->isAperture())
            {
               SrffShape *shape = this->GetSrffWriter(isBottom).AddShape(insertedBlock);
               if (shape != NULL)
                  shapeId = shape->GetId();
            }
            else
            {
               POSITION insertpos = insertedBlock->getHeadDataInsertPosition();
               while (insertpos != NULL && shapeId < 0)
               {
                  DataStruct *subdata = insertedBlock->getNextDataInsert(insertpos);
                  BlockStruct *subblock = m_doc.getBlockAt( subdata->getInsert()->getBlockNumber() );
                  if (subblock != NULL && subblock->isAperture())
                  {
                     // this is ignoring layers, just looking for first aperture
                     SrffShape *shape = this->GetSrffWriter(isBottom).AddShape(subblock);
                     if (shape != NULL)
                        shapeId = shape->GetId();
                  }
               }
            }
            // If that didn't work then get extents and make square shape.
            if (shapeId < 0)
            {
               CExtent extent = insertedBlock->getExtent(m_doc.getCamCadData());
               if (extent.isValid())
               {
                  SrffShape *shape = this->GetSrffWriter(isBottom).AddShape(srffShapeRectangle, extent.getXsize(), extent.getYsize());
                  if (shape != NULL)
                     shapeId = shape->GetId();
                  
               }
            }
         }

         imageDef->AddFiducial(nextId, fidRefname, applySakiOriginX(fiddata->getX()), applySakiOriginY(fiddata->getY()), fiddata->getRotation(), shapeId);
      }
   }   
}

CString CyberOpticsAOIWriter::getUniqueName(CString refname)
{
#ifdef NEED_UNIQUE_NAMES

   int counter = 0;

   CString adjustedName( refname );

   void *junk;
   while (m_usedRefnameMap.Lookup(adjustedName, junk))
   {
      adjustedName.Format("%s~%d", refname, ++counter);
   }

   m_usedRefnameMap.SetAt(adjustedName, NULL);

   return adjustedName;
#else

   return refname;
#endif
}


CExtent CyberOpticsAOIWriter::GetOutlineExtent()
{
   Mat2x2 m;
   RotMat2(&m, 0);

	ExtentRect fileExtents;

   fileExtents.left = fileExtents.bottom = FLT_MAX;
   fileExtents.right = fileExtents.top = -FLT_MAX;

   bool OutlineFound = false;

   if (GetActiveFile() != NULL)
   {
      POSITION dataPos = GetActiveFile()->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = GetActiveFile()->getBlock()->getDataList().GetNext(dataPos);

         if ((data->getDataType() == T_POLY) &&
            ((GetActiveFile()->getBlockType() == BLOCKTYPE_PANEL && data->getGraphicClass() == GR_CLASS_PANELOUTLINE) ||
            (GetActiveFile()->getBlockType() == BLOCKTYPE_PCB && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)))
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

   if (!OutlineFound && GetActiveFile() != NULL)
   {
      GetActiveFile()->getBlock()->calculateVisibleBlockExtents(m_doc.getCamCadData());		
      extent = GetActiveFile()->getBlock()->getExtent();
   }

   return extent;
}

int CyberOpticsAOIWriter::GetComponentCount(bool isBottom)
{
   int count = 0;

   if (this->m_PCBDataList.IsPanel())
   {
      // Panel, sum counts for all boards
      for(POSITION pcbPos = m_PCBDataList.GetStartPosition(); pcbPos; )
      {
         CString pcbName;
         BaseAoiExporterPCBData *pcbData;
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
         BaseAoiExporterPCBData *pcbData;
         this->m_PCBDataList.GetNextAssoc(pos, pcbName, pcbData);

         count += pcbData->getCompCount(isBottom);
      }
   }

   return count;
}

double CyberOpticsAOIWriter::getRotationDegree(double radious, int mirror)
{
   // Saki doc says do not alter rotation for mirror. Incoming mirror param is good, just ignore it.
   // Override incoming mirror with "not mirrored" to disable conversion is calc.
   mirror = 0;

   double insertRot = round(normalizeDegrees(RadToDeg(radious)));
   return normalizeDegrees(((mirror)?(360 -  insertRot):insertRot));
}

CString CyberOpticsAOIWriter::getPartNumber(DataStruct* data)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_doc, data->getAttributeMap(), GetSettings()->getPartNumberKW(), 0);
   CString partNumber = attrib?attrib->getStringValue():"No_Part_Number";

   if ((data->getInsert()->getInsertType() == insertTypeFiducial))
      partNumber = "MARKE";

   return partNumber;
}

void CyberOpticsAOIWriter::TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data)
{
   CPoint2d boardXY = data->getInsert()->getOrigin2d();
   l_panelMatrix.transform(boardXY);

   l_boardMatrix.scale(data->getInsert()->getScale() * (data->getInsert()->getGraphicMirrored()?-1:1), data->getInsert()->getScale());
   
   l_boardMatrix.rotateRadians(data->getInsert()->getAngle());

   l_boardMatrix.translate(boardXY);
}

//=============================================================================================
// SRFF Support
// With some mods to accomodate different vendor-specific sections, this could
// be a generalized SRFF support lib. As it is, it is CyberOptics-specific, but only
// because of CyberOptics vendor-specific portions being built-in, not configurable.
//

SRFFWriter::SRFFWriter(CCEtoODBDoc &doc)
: m_doc(doc)
{
   m_indentLevel = 0;
   m_indentSize = INDENT_SIZE_DEFAULT;
   m_dateTime = CTime::GetCurrentTime();
   m_outputUnits = pageUnitsMicroMeters; // AKA Microns
   m_panelSizeX = 0.;
   m_panelSizeY = 0.;
   m_lastCyberSegmentId = 0;
   m_lastImageFiducialId = 0;
   m_writeProcessSection = true; // No data, but CyberOptics Teach s/w requires this schema be present.

   SetOutputUnits(m_outputUnits);
}

void SRFFWriter::SetOutputUnits(PageUnitsTag units)
{
   // Use specified units if one of the acceptable values, otherwise
   // reset to default.
   switch (units)
   {
   case pageUnitsMilliMeters:
   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsMicroMeters:
      // These are the acceptabe units, save setting.
      m_outputUnits = units;
      break;
   default:
      // Not acceptable units, reset to default.
      m_outputUnits = pageUnitsMicroMeters;
      break;
   }

   // Cache units conversion factor.
   PageUnitsTag incomingUnits = m_doc.getPageUnits();
   m_outputUnitsFactor = Units_Factor(incomingUnits, m_outputUnits);
}

int SRFFWriter::GetDecimals()
{
   // Decimal places for output in given units.

   int decimals = 5; // default. 
   
   switch (m_outputUnits)
   {
   case pageUnitsInches:         decimals = 3; break;
   case pageUnitsMilliMeters:    decimals = 2; break;
   case pageUnitsMils:           decimals = 0; break;
   case pageUnitsMicroMeters:    decimals = 0; break;
      break;
   }
   return decimals;
}

CString SRFFWriter::GetSrffOutputUnitsName()
{
   // These names come from SRFF spec.
   // We're supporting four choices, there are two others we chose not to support.

   CString srffUnitsName("Invalid"); // This makes invalid output. A sure indicator something is amiss.

   switch (m_outputUnits)
   {
   case pageUnitsMilliMeters:
      srffUnitsName = "Millimeters";
      break;
   case pageUnitsInches:
      srffUnitsName = "Inches";
      break;
   case pageUnitsMils:
      srffUnitsName = "Inches/1000";
      break;
   case pageUnitsMicroMeters:
      srffUnitsName = "Microns";
      break;
   }

   return srffUnitsName;
}

CString SRFFWriter::FormatValueStr(double val)
{
   int decimals = GetDecimals();
   CString valStr;
   valStr.Format("%.*f", decimals, val);
   return valStr;
}

void SRFFWriter::PushIndent()
{
   if (m_indentLevel < 0)
      m_indentLevel = 0;

   m_indentLevel++;
   m_indent.Format("%*s", (m_indentLevel * m_indentSize), "");
}

void SRFFWriter::PopIndent()
{
   m_indentLevel--;
   if (m_indentLevel <= 0)
   {
      m_indentLevel = 0;
      m_indent.Empty();
   }
   else
   {
      m_indent.Format("%*s", (m_indentLevel * m_indentSize), "");
   }
}

void SRFFWriter::SetPanelSize(double sizeX, double sizeY)
{
   m_panelSizeX = sizeX;
   m_panelSizeY = sizeY;
}

void SRFFWriter::AddImage(int cczBlockNum, CString refname, double locX, double locY, double rotDeg, int imageDefinitionId)
{
   // Panel is image 0, always. So counting in this ID list starts at zero (0).
   // 
   int nextId = this->m_imageList.GetCount(); // not + 1;
   SrffImage *image = new SrffImage(cczBlockNum, nextId, refname, locX, locY, rotDeg, imageDefinitionId);
   this->m_imageList.AddTail(image);
}

void SRFFWriter::WriteFile(CFormatStdioFile &file)
{
   // Date/Time is set in constructor, just so it is initialized. 
   // Reset it here  since it conceivably is some time between constructor activation
   // and actual output. We save it in writer so date/time written in this header comment
   // will match that written in Data inside {Header} record.
   m_dateTime = CTime::GetCurrentTime();

   file.WriteString("%s#------------------------------------------------------------------\n", Indent());
   file.WriteString("%s# Standard Recipe Format File for CyberOptics\n", Indent());
   file.WriteString("%s# Created by %s\n", Indent(), getApp().getCamCadTitle());  // this title includes version number
   file.WriteString("%s# Created %s\n", Indent(), m_dateTime.Format("%A, %B %d, %Y at %H:%M:%S"));
   file.WriteString("%s#------------------------------------------------------------------\n", Indent());

   WriteSchema(file);
   file.WriteString("%s\n", Indent()); // Blank line between schema and data
   WriteData(file);
}

void SRFFWriter::WriteData(CFormatStdioFile &file)
{
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin Data\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Data\n", Indent());

   PushIndent();
   WriteProductSectionData(file);
   if (m_writeProcessSection)
      WriteProcessSectionData(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End Data\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteSchema(CFormatStdioFile &file)
{
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin Schema\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Schema\n", Indent());

   PushIndent();
   WriteProductSectionSchema(file);
   if (m_writeProcessSection)
      WriteProcessSectionSchema(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End Schema\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteProductSectionSchema(CFormatStdioFile &file)
{
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin Product Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Product\n", Indent());

   PushIndent();
   WriteSmemaProductSchema(file);
   WriteCyberProductSchema(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End Product Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteProductSectionData(CFormatStdioFile &file)
{
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin Product Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Product\n", Indent());

   PushIndent();
   WriteSmemaProductData(file);
   WriteCyberProductData(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End Product Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteSmemaProductSchema(CFormatStdioFile &file)
{
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin SMEMA Product Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Organization SMEMA\n", Indent());

   PushIndent();
   WriteComponentDefinitionSchema(file);
   WriteComponentLinkSchema(file);
   WriteBarcodeSchema(file);
   WriteFeatureSchema(file);
   WriteHeaderSchema(file);
   WriteImageSchema(file);
   WriteImageDefinitionSchema(file);
   WriteImageFiducialSchema(file);
   WriteLocalFiducialSchema(file);
   WriteLocationSchema(file);
   WritePanelSchema(file);
   WritePatternSchema(file);
   WritePatternDefinitionSchema(file);
   WriteShapeSchema(file);
   WriteSkipMarkSchema(file);
   WriteSRFFVersionSchema(file);
   WriteVendorShapeLinkSchema(file);
   WritePrintAreaSchema(file);
   WriteScreenPropertiesSchema(file);
   WriteScreenFiducialSchema(file);
   WriteCrossSchema(file);
   WriteDiamondSchema(file);
   WriteDiscSchema(file);
   WriteDonutSchema(file);
   WriteRectangleSchema(file);
   WriteTriangleSchema(file);
   WriteAccelerationUnitsSchema(file);
   WriteAngleUnitsSchema(file);
   WriteAngularAccelerationUnitsSchema(file);
   WriteAngularVelocityUnitsSchema(file);
   WriteDistanceUnitsSchema(file);
   WriteFlowUnitsSchema(file);
   WriteForceUnitsSchema(file);
   WriteHumidityUnitsSchema(file);
   WriteMassUnitsSchema(file);
   WritePowerUnitsSchema(file);
   WritePressureUnitsSchema(file);
   WriteTemperatureUnitsSchema(file);
   WriteTimeUnitsSchema(file);
   WriteTorqueUnitsSchema(file);
   WriteVelocityUnitsSchema(file);
   WriteVolumeUnitsSchema(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End SMEMA Product Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteSmemaProductData(CFormatStdioFile &file)
{
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin SMEMA Product Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Organization SMEMA\n", Indent());

   PushIndent();
   WriteHeader(file);
   WriteSRFFVersion(file);
   WriteAngleUnits(file);
   WriteDistanceUnits(file);
   WritePanel(file);
   WriteImageList(file);
   WriteImageDefinitionList(file);  // Also writes Location list for image definition
   WriteComponentDefinitionList(file);
   WriteComponentLinkList(file);
   WritePatternDefinitionList(file);
   WriteShapeList(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End SMEMA Product Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}


void SRFFWriter::WriteComponentDefinitionSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{ComponentDefinition\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ComponentDefinitionId}\n", Indent());
   file.WriteString("%s{String PartName}\n", Indent());
   file.WriteString("%s{Id ReferenceComponentLinkId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteComponentDefinition(CFormatStdioFile &file, SrffComponentDefinition *compDef)
{
   file.WriteString("%s{ComponentDefinition %d \"%s\" %d }\n",
      Indent(),
      compDef->GetId(),
      compDef->GetPartName(),
      compDef->GetReferenceComponentLinkId() );
}

void SRFFWriter::WriteComponentDefinitionList(CFormatStdioFile &file)
{
   POSITION pos = this->m_componentDefinitionMap.GetStartPosition();
   while (pos != NULL)
   {
      CString key;
      SrffComponentDefinition *compDef = NULL;
      this->m_componentDefinitionMap.GetNextAssoc(pos, key, compDef);
      if (compDef != NULL)
      {
         WriteComponentDefinition(file, compDef);
      }
   }
}

void SRFFWriter::WriteComponentLinkSchema(CFormatStdioFile &file)
{

   file.WriteString("%s{ComponentLink\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ComponentLinkId}\n", Indent());
   file.WriteString("%s{String PackageName}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteComponentLink(CFormatStdioFile &file, SrffComponentLink *compLink)
{
   file.WriteString("%s{ComponentLink %d \"%s\" }\n",
      Indent(),
      compLink->GetId(),
      compLink->GetPackageName());
}

void SRFFWriter::WriteComponentLinkList(CFormatStdioFile &file)
{
#ifdef FIRST_MAP_TYPE
   POSITION pos = this->m_componentLinkMap.GetStartPosition();
   while (pos != NULL)
   {
      int key = 0;
      SrffComponentLink *compLink = NULL;
      this->m_componentLinkMap.GetNextAssoc(pos, key, compLink);
      if (compLink != NULL)
      {
         WriteComponentLink(file, compLink);
      }
   }
#else
   int count = this->m_componentLinkMap.getSize();
   for (int i = 0; i < count; i++)
   {
      //int key = 0;
      SrffComponentLink *compLink = this->m_componentLinkMap.getAt(i);
      //this->m_componentLinkMap.GetNextAssoc(pos, key, compLink);
      if (compLink != NULL)
      {
         WriteComponentLink(file, compLink);
      }
   }
#endif
}

void SRFFWriter::WriteBarcodeSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Barcode\n", Indent());
   PushIndent();
   file.WriteString("%s{Id BarcodeId}\n", Indent());
   file.WriteString("%s{String Barcode}\n", Indent());
   file.WriteString("%s{Id ReferenceComponentLinkId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteFeatureSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Feature\n", Indent());
   PushIndent();
   file.WriteString("%s{Id FeatureId}\n", Indent());
   file.WriteString("%s{String FeatureName}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance PositionZ}\n", Indent());
   file.WriteString("%s{Angle RotationZ}\n", Indent());
   file.WriteString("%s{Id ReferenceShapeId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteHeaderSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Header\n", Indent());
   PushIndent();
   file.WriteString("%s{Id HeaderId}\n", Indent());
   file.WriteString("%s{String ProductName}\n", Indent());
   file.WriteString("%s{String ProductNotes}\n", Indent());
   file.WriteString("%s{DateTime Created}\n", Indent());
   file.WriteString("%s{DateTime LastModified}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteHeader(CFormatStdioFile &file)
{
   // Format date/time like "2010-01-08T15:55:22.0+00:00"
   CString timeStr;
   timeStr.Format("%s.0+00:00", m_dateTime.Format("%Y-%m-%dT%H:%M:%S"));

   // Safe copies of of product name and note. Could escape the double quotes,
   // we will if someone asks. In meantime just replace with blank.
   CString safeProductName(m_productName);
   CString safeProductNote(m_productNote);
   safeProductName.Replace("\"", " ");
   safeProductNote.Replace("\"", " ");

   file.WriteString("%s{Header 1\n", Indent());
   PushIndent();
   file.WriteString("%s\"%s\"\n", Indent(), safeProductName);
   file.WriteString("%s\"%s\"\n", Indent(), safeProductNote);
   file.WriteString("%s\"%s\"\n", Indent(), timeStr);
   file.WriteString("%s\"%s\"\n", Indent(), timeStr);
   PopIndent();
   file.WriteString("%s}\n",      Indent());
}

void SRFFWriter::WriteImageSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Image\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ImageId}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance PositionZ}\n", Indent());
   file.WriteString("%s{Angle RotationX}\n", Indent());
   file.WriteString("%s{Angle RotationY}\n", Indent());
   file.WriteString("%s{Angle RotationZ}\n", Indent());
   file.WriteString("%s{Id ReferenceImageDefinitionId}\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   file.WriteString("%s{Id ReferenceSkipMarkId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteImage(CFormatStdioFile &file, SrffImage *image)
{
   file.WriteString("%s{Image %d %s %s 0 0 0 %.1f %d %d * }\n",
      Indent(),
      image->GetImageId(),
      FormatValueStr( image->GetPositionX(GetUnitsFactor()) ),
      FormatValueStr( image->GetPositionY(GetUnitsFactor()) ),
      image->GetRotationDeg(),
      image->GetReferenceImageDefinitionId(),
      image->GetReferenceImageId() );//,
      //image->GetReferenceSkipMarkId() );
}

void SRFFWriter::WriteImageList(CFormatStdioFile &file)
{
   POSITION pos = this->m_imageList.GetHeadPosition();
   while (pos != NULL)
   {
      SrffImage *image = this->m_imageList.GetNext(pos);
      WriteImage(file, image);
   }
}

void SRFFWriter::WriteImageDefinitionSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{ImageDefinition\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ImageDefinitionId}\n", Indent());
   file.WriteString("%s{String ImageDefinitionName}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteImageDefinition(CFormatStdioFile &file, SrffImageDefinition *imageDef)
{
   file.WriteString("%s{ImageDefinition %d \"%s\" }\n", 
      Indent(),
      imageDef->GetId(),
      imageDef->GetName() );
}


void SRFFWriter::WriteImageDefinitionList(CFormatStdioFile &file)
{
#ifdef FIRST_MAP_TYPE
   POSITION pos = this->m_imageDefinitionMap.GetStartPosition();
   while (pos != NULL)
   {
      int key = 0;
      SrffImageDefinition *imageDef = NULL;
      this->m_imageDefinitionMap.GetNextAssoc(pos, key, imageDef);
      if (imageDef != NULL)
      {
         WriteImageDefinition(file, imageDef);
         WriteImageDefinitionLocationList(file, imageDef);
      }
   }
#else
   int count = this->m_imageDefinitionMap.getSize();
   for (int i = 0; i < count; i++)
   {
      SrffImageDefinition *imageDef = this->m_imageDefinitionMap.getAt(i);
      if (imageDef != NULL)
      {
         WriteImageDefinition(file, imageDef);
         PushIndent();
         WriteImageDefinitionFiducialList(file, imageDef);
         WriteImageDefinitionLocationList(file, imageDef);
         PopIndent();
      }
   }
#endif
}

void SRFFWriter::WriteImageDefinitionLocationList(CFormatStdioFile &file, SrffImageDefinition *imageDef)
{
   POSITION pos = imageDef->GetHeadLocationPosition();
   while (pos != NULL)
   {
      SrffLocation *srffLoc = imageDef->GetNextLocation(pos);
      WriteLocation(file, srffLoc);
   }
}

void SRFFWriter::WriteImageDefinitionFiducialList(CFormatStdioFile &file, SrffImageDefinition *imageDef)
{
   POSITION pos = imageDef->GetHeadFiducialPosition();
   while (pos != NULL)
   {
      SrffImageFiducial *srffFid = imageDef->GetNextFiducial(pos);
      WriteImageFiducial(file, srffFid);
   }
}


void SRFFWriter::WriteImageFiducialSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{ImageFiducial\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ImageFiducialId}\n", Indent());
   file.WriteString("%s{String ReferenceDesignator}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance PositionZ}\n", Indent());
   file.WriteString("%s{Angle RotationZ}\n", Indent());
   file.WriteString("%s{Id ReferenceImageDefinitionId}\n", Indent());
   file.WriteString("%s{Id ReferenceShapeId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteImageFiducial(CFormatStdioFile &file, SrffImageFiducial *srffLoc)
{
   file.WriteString("%s{ImageFiducial %d \"%s\" %s %s 0 %.1f %d %d }\n",
      Indent(),
      srffLoc->GetId(),
      srffLoc->GetRefname(),
      FormatValueStr( srffLoc->GetPositionX(GetUnitsFactor()) ),
      FormatValueStr( srffLoc->GetPositionY(GetUnitsFactor()) ),
      srffLoc->GetRotationDeg(),
      srffLoc->GetReferenceImageDefinitionId(),
      srffLoc->GetReferenceShapeId() );
}


void SRFFWriter::WriteLocalFiducialSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{LocalFiducial\n", Indent());
   PushIndent();
   file.WriteString("%s{Id LocalFiducialId}\n", Indent());
   file.WriteString("%s{String ReferenceDesignator}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance PositionZ}\n", Indent());
   file.WriteString("%s{Angle RotationZ}\n", Indent());
   file.WriteString("%s{ID ReferenceLocationId}\n", Indent());
   file.WriteString("%s{ID ReferenceShapeId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteLocationSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Location\n", Indent());
   PushIndent();
   file.WriteString("%s{Id LocationId}\n", Indent());
   file.WriteString("%s{String ReferenceDesignator}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance PositionZ}\n", Indent());
   file.WriteString("%s{Angle RotationZ}\n", Indent());
   file.WriteString("%s{Id ReferenceComponentDefinitionId}\n", Indent());
   file.WriteString("%s{Id ReferenceImageDefinitionId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteLocation(CFormatStdioFile &file, SrffLocation *srffLoc)
{
   file.WriteString("%s{Location %d \"%s\" %s %s 0 %.1f %d %d }\n",
      Indent(),
      srffLoc->GetLocationId(),
      srffLoc->GetRefname(),
      FormatValueStr( srffLoc->GetPositionX(GetUnitsFactor()) ),
      FormatValueStr( srffLoc->GetPositionY(GetUnitsFactor()) ),
      srffLoc->GetRotationDeg(),
      srffLoc->GetReferenceComponentDefinitionId(),
      srffLoc->GetReferenceImageDefinitionId() );
}

void SRFFWriter::WritePanelSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Panel\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PanelId}\n", Indent());
   file.WriteString("%s{Distance LengthX}\n", Indent());
   file.WriteString("%s{Distance LengthY}\n", Indent());
   file.WriteString("%s{Distance LengthZ}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WritePanel(CFormatStdioFile &file)
{
   file.WriteString("%s{Panel 1 %s %s * }\n", 
      Indent(), 
      FormatValueStr( ConvertUnits(m_panelSizeX) ), 
      FormatValueStr( ConvertUnits(m_panelSizeY) ));
}

void SRFFWriter::WritePatternSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Pattern\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PatternId}\n", Indent());
   file.WriteString("%s{Id ReferenceComponentLinkId}\n", Indent());
   file.WriteString("%s{Id ReferencePatternDefinitionId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WritePatternDefinitionSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{PatternDefinition\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PatternDefinitionId}\n", Indent());
   file.WriteString("%s{String PatternDefinitionName}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{Object Feature}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WritePatternDefinition(CFormatStdioFile &file, SrffPatternDefinition *patDef)
{
   // Actually writes Pattern and PatternDefinition together

   file.WriteString("%s{Pattern %d %d %d }\n",
      Indent(),
      patDef->GetId(),
      patDef->GetReferenceComponentLinkId(),
      patDef->GetId() );

   file.WriteString("%s{PatternDefinition %d \"%s\" {\n",
      Indent(),
      patDef->GetId(),
      patDef->GetPatternName() );

   PushIndent();
   int featureCount = patDef->GetFeatureCount();
   for (int i = 0; i < featureCount; i++)
   {
      SrffFeature *f = patDef->GetFeatureAt(i);
      if (f != NULL)
      {
         file.WriteString("%s{Feature %d \"%s\" %s %s 0 %.1f %d }\n",
            Indent(),
            f->GetId(),
            f->GetName(),
            FormatValueStr( f->GetPositionX(GetUnitsFactor()) ),
            FormatValueStr( f->GetPositionY(GetUnitsFactor()) ),
            f->GetRotationDeg(),
            f->GetRefShapeId() );
      }
   }
   PopIndent();

   file.WriteString("%s} }\n", Indent());
}

void SRFFWriter::WritePatternDefinitionList(CFormatStdioFile &file)
{
   POSITION pos = this->m_patternDefinitionMap.GetStartPosition();
   while (pos != NULL)
   {
      CString key;
      SrffPatternDefinition *patDef = NULL;
      this->m_patternDefinitionMap.GetNextAssoc(pos, key, patDef);
      if (patDef != NULL)
      {
         WritePatternDefinition(file, patDef);
      }
   }
}

void SRFFWriter::WriteShapeSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Shape\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ShapeId}\n", Indent());
   file.WriteString("%s{SELECT    {\n", Indent());
   file.WriteString("%s{Object Cross}\n", Indent());
   file.WriteString("%s{Object Diamond}\n", Indent());
   file.WriteString("%s{Object Disc}\n", Indent());
   file.WriteString("%s{Object Donut}\n", Indent());
   file.WriteString("%s{Object Rectangle}\n", Indent());
   file.WriteString("%s{Object Triangle}\n", Indent());
   file.WriteString("%s{Object VendorShapeLink}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WriteShape(CFormatStdioFile &file, SrffShape *shape)
{
   file.WriteString("%s{Shape %d {%s} }\n",
      Indent(),
      shape->GetId(),
      shape->GetShapeObjectDescription(this));
}

void SRFFWriter::WriteShapeList(CFormatStdioFile &file)
{
   POSITION pos = this->m_shapeList.GetHeadPosition();
   while (pos != NULL)
   {
      SrffShape *shape = this->m_shapeList.GetNext(pos);
      WriteShape(file, shape);
   }
}

void SRFFWriter::WriteSkipMarkSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{SkipMark\n", Indent());
   PushIndent();
   file.WriteString("%s{Id SkipMarkId}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance PositionZ}\n", Indent());
   file.WriteString("%s{Angle RotationZ}\n", Indent());
   file.WriteString("%s{Id ReferenceShapeId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteSRFFVersionSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{SRFFVersion\n", Indent());
   PushIndent();
   file.WriteString("%s{Id SRFFVersionId}\n", Indent());
   file.WriteString("%s{String VersionName}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteSRFFVersion(CFormatStdioFile &file)
{
   // Per SRFF spec, there is only one valid form for this data record.
   file.WriteString("%s{SRFFVersion 1 \"1.0\" }\n", Indent());
}

void SRFFWriter::WriteVendorShapeLinkSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{VendorShapeLink\n", Indent());
   PushIndent();
   file.WriteString("%s{Id VendorShapeLinkId}\n", Indent());
   file.WriteString("%s{Id ReferenceVendorShapeId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WritePrintAreaSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{PrintArea\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PrintAreaId}\n", Indent());
   file.WriteString("%s{Distance PrintAreaLengthX}\n", Indent());
   file.WriteString("%s{Distance PrintAreaLengthY}\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteScreenPropertiesSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{ScreenProperties\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ScreenPropertiesId}\n", Indent());
   file.WriteString("%s{String ScreenRecordName}\n", Indent());
   file.WriteString("%s{Distance FrameOuterLengthX}\n", Indent());
   file.WriteString("%s{Distance FrameOuterLengthY}\n", Indent());
   file.WriteString("%s{Distance StencilOriginOffsetX}\n", Indent());
   file.WriteString("%s{Distance StencilOriginOffetY}\n", Indent());
   file.WriteString("%s{Distance StencilOuterLengthX}\n", Indent());
   file.WriteString("%s{Distance StencilOuterLengthY}\n", Indent());
   file.WriteString("%s{Distance MeshOriginOffsetX}\n", Indent());
   file.WriteString("%s{Distance MeshOriginOffetY}\n", Indent());
   file.WriteString("%s{Distance MeshOuterLengthX}\n", Indent());
   file.WriteString("%s{Distance MeshOuterLengthY}\n", Indent());
   file.WriteString("%s{Distance ScreenImageOriginOffsetX}\n", Indent());
   file.WriteString("%s{Distance ScreenImageOriginOffetY}\n", Indent());
   file.WriteString("%s{Distance ScreenImageOuterLengthX}\n", Indent());
   file.WriteString("%s{Distance ScreenImageOuterLengthY}\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   file.WriteString("%s{Id ReferenceBarCodeId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteScreenFiducialSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{ScreenFiducial\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ScreenFiducialId}\n", Indent());
   file.WriteString("%s{String ReferenceDesignator}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance PositionZ}\n", Indent());
   file.WriteString("%s{Angle RotationZ}\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   file.WriteString("%s{Id ReferenceShapeId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteCrossSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Cross\n", Indent());
   PushIndent();
   file.WriteString("%s{Id CrossId}\n", Indent());
   file.WriteString("%s{Distance Base}\n", Indent());
   file.WriteString("%s{Distance Height}\n", Indent());
   file.WriteString("%s{Distance BaseLegWidth}\n", Indent());
   file.WriteString("%s{Distance HeightLegWidth}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteDiamondSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Diamond\n", Indent());
   PushIndent();
   file.WriteString("%s{Id DiamondId}\n", Indent());
   file.WriteString("%s{Distance Base}\n", Indent());
   file.WriteString("%s{Distance Height}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteDiscSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Disc\n", Indent());
   PushIndent();
   file.WriteString("%s{Id DiscId}\n", Indent());
   file.WriteString("%s{Distance Diameter}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteDonutSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Donut\n", Indent());
   PushIndent();
   file.WriteString("%s{Id DonutId}\n", Indent());
   file.WriteString("%s{Distance InnerDiameter}\n", Indent());
   file.WriteString("%s{Distance OuterDiameter}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteRectangleSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Rectangle\n", Indent());
   PushIndent();
   file.WriteString("%s{Id RectangleId}\n", Indent());
   file.WriteString("%s{Distance Base}\n", Indent());
   file.WriteString("%s{Distance Height}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteTriangleSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Triangle\n", Indent());
   PushIndent();
   file.WriteString("%s{Id TriangleId}\n", Indent());
   file.WriteString("%s{Distance Base}\n", Indent());
   file.WriteString("%s{Distance Height}\n", Indent());
   file.WriteString("%s{Distance Offset}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteAccelerationUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{AccelerationUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id AccelerationUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfAcceleration}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteAngleUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{AngleUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id AngleUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfAngularMeasurement}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteAngleUnits(CFormatStdioFile &file)
{
   file.WriteString("%s{AngleUnits 1 \"Degrees\" }\n", Indent());
}

void SRFFWriter::WriteAngularAccelerationUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{AngularAccelerationUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id AngularAccelerationId}\n", Indent());
   file.WriteString("%s{String UnitsOfAngularAcceleration}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteAngularVelocityUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{AngularVelocityUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id AngularVelocityId}\n", Indent());
   file.WriteString("%s{String UnitsOfAngularVelocity}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteDistanceUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{DistanceUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id DistanceUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfDistance}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteDistanceUnits(CFormatStdioFile &file)
{
   file.WriteString("%s{DistanceUnits 1 \"%s\" }\n", Indent(), GetSrffOutputUnitsName());
}

void SRFFWriter::WriteFlowUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{FlowUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id FlowUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfFlow}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteForceUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{ForceUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id ForceUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfForce}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteHumidityUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{HumidityUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id HumidityUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfHumidity}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteMassUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{MassUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id MassUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfMass}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WritePowerUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{PowerUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PowerUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfPower}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WritePressureUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{PressureUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PressureUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfPressure}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteTemperatureUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{TemperatureUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id TemperatureUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfTemperature}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteTimeUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{TimeUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id TimeUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfTime}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteTorqueUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{TorqueUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id TorqueUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfTorque}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteVelocityUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{VelocityUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id VelocityUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfVelocity}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteVolumeUnitsSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{VolumeUnits\n", Indent());
   PushIndent();
   file.WriteString("%s{Id VolumeUnitsId}\n", Indent());
   file.WriteString("%s{String UnitsOfVolume}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteCyberProductSchema(CFormatStdioFile &file)
{                           
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin CyberOptics Product Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Organization CyberOptics\n", Indent());

   PushIndent();
   WriteCyberShapeSchema(file);
   WriteCyberSegmentSchema(file);
   WriteCyberImageNameSchema(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End CyberOptics Product Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteCyberProductData(CFormatStdioFile &file)
{                           
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin CyberOptics Product Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Organization CyberOptics\n", Indent());

   PushIndent();
   WriteCyberShapeList(file);
   WriteCyberImageNameList(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End CyberOptics Product Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteCyberShapeSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{CyberShape\n", Indent());
   PushIndent();
   file.WriteString("%s{Id CyberShapeId}\n", Indent());
   file.WriteString("%s{String CyberShapeName}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{Object CyberSegment}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WriteCyberSegmentSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{CyberSegment\n", Indent());
   PushIndent();
   file.WriteString("%s{Id CyberSegmentId}\n", Indent());
   file.WriteString("%s{Bool Line}\n", Indent());
   file.WriteString("%s{Bool PenDown}\n", Indent());
   file.WriteString("%s{Bool ClockwiseArc}\n", Indent());
   file.WriteString("%s{Distance PositionX}\n", Indent());
   file.WriteString("%s{Distance PositionY}\n", Indent());
   file.WriteString("%s{Distance ArcX}\n", Indent());
   file.WriteString("%s{Distance ArcY}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}


void SRFFWriter::WriteCyberShape(CFormatStdioFile &file, CyberShape *cyberShape)
{
   file.WriteString("%s{CyberShape %d \"%s\" {\n", Indent(), cyberShape->GetId(), cyberShape->GetName());
   PushIndent();

   int segCount = cyberShape->GetSegmentCount();
   for (int i = 0; i < segCount; i++)
   {
      CyberSegment *seg = cyberShape->GetSegmentAt(i);
      if (seg != NULL)
      {
         CString centerXStr( FormatValueStr( seg->GetCenterX(GetUnitsFactor()) ) );
         CString centerYStr( FormatValueStr( seg->GetCenterY(GetUnitsFactor()) ) );

         file.WriteString("%s{CyberSegment %d %s %s %s %s %s %s %s }\n",
            Indent(),
            seg->GetId(),
            seg->IsArc() ? "F" : "T",  // reverse!  Output is IsLine()
            seg->GetPenDown() ? "T" : "F",
            seg->IsArc() ? (seg->GetClockwise() ? "T" : "F") : "*",
            this->FormatValueStr( seg->GetX(GetUnitsFactor()) ),
            this->FormatValueStr( seg->GetY(GetUnitsFactor()) ),
            seg->IsArc() ? centerXStr : "*",
            seg->IsArc() ? centerYStr : "*");

      }
   }

   PopIndent();
   file.WriteString("%s}}\n", Indent());
}

void SRFFWriter::WriteCyberShapeList(CFormatStdioFile &file)
{
   int count = m_cyberShapeMap.getSize();
   for (int i = 0; i < count; i++)
   {
      CyberShape *cyberShape = m_cyberShapeMap.getAt(i);
      WriteCyberShape(file, cyberShape);
   }
}

void SRFFWriter::WriteCyberImageNameSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{CyberImageName\n", Indent());
   PushIndent();
   file.WriteString("%s{Id CyberImageNameId}\n", Indent());
   file.WriteString("%s{String ImageName}\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteCyberImageName(CFormatStdioFile &file, SrffImage *image, int id)
{
   file.WriteString("%s{CyberImageName %d \"%s\" %d }\n",
      Indent(),
      id,
      image->GetRefname(),
      image->GetImageId());
}

void SRFFWriter::WriteCyberImageNameList(CFormatStdioFile &file)
{
   POSITION pos = this->m_imageList.GetHeadPosition();
   int id = 0;
   while (pos != NULL)
   {
      SrffImage *image = this->m_imageList.GetNext(pos);
      if (!image->GetRefname().IsEmpty())
         WriteCyberImageName(file, image, ++id);
   }
}

void SRFFWriter::WriteProcessSectionSchema(CFormatStdioFile &file)
{                                          
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin Process Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Process\n", Indent());

   PushIndent();
   WriteSmemaProcessSectionSchema(file);
   WriteCyberProcessSchema(file);
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End Process Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteProcessSectionData(CFormatStdioFile &file)
{                                          
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin Process Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Process\n", Indent());

   PushIndent();
   // We have not process data at present.
   // If we did, we would write it from here.
   PopIndent();

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End Process Data Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteSmemaProcessSectionSchema(CFormatStdioFile &file)
{                                         
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin SMEMA Process Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Organization SMEMA\n", Indent());

   PushIndent();
   WriteFeatureGroupSchema(file);
   WriteFeatureGroupOrderedSchema(file);
   WriteLocationGroupSchema(file);
   WriteLocationGroupOrderedSchema(file);
   WriteDispenseOrderSchema(file);
   WriteInspectOrderSchema(file);
   WritePlacementOrderSchema(file);
   WritePrintSchema(file);
   WritePrinterAlignmentSchema(file);
   WriteSqueegeeSchema(file);
   WriteSqueegeeDefinitionSchema(file);
   PopIndent();


   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End SMEMA Process Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteFeatureGroupSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{FeatureGroup\n", Indent());
   PushIndent();
   file.WriteString("%s{Id FeatureGroupId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   file.WriteString("%s{Id ReferenceLocationId}\n", Indent());
   file.WriteString("%s{Id ReferencePatternDefinitionId}\n", Indent());
   file.WriteString("%s{Id ReferenceFeatureId}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WriteFeatureGroupOrderedSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{FeatureGroupOrdered\n", Indent());
   PushIndent();
   file.WriteString("%s{Id FeatureGroupOrderedId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   file.WriteString("%s{Id ReferenceLocationId}\n", Indent());
   file.WriteString("%s{Id ReferencePatternDefinitionId}\n", Indent());
   file.WriteString("%s{Id ReferenceFeatureId}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WriteLocationGroupSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{LocationGroup\n", Indent());
   PushIndent();
   file.WriteString("%s{Id LocationGroupId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   file.WriteString("%s{Id ReferenceLocationId}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WriteLocationGroupOrderedSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{LocationGroupOrdered\n", Indent());
   PushIndent();
   file.WriteString("%s{Id LocationGroupOrderedId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   file.WriteString("%s{Id ReferenceLocationId}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WriteDispenseOrderSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{DispenseOrder\n", Indent());
   PushIndent();
   file.WriteString("%s{Id DispenseOrderId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{SELECT    {\n", Indent());
   file.WriteString("%s{Object FeatureGroup}\n", Indent());
   file.WriteString("%s{Object FeatureGroupOrdered}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}}}\n", Indent());
}

void SRFFWriter::WriteInspectOrderSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{InspectOrder\n", Indent());
   PushIndent();
   file.WriteString("%s{Id InspectOrderId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{SELECT    {\n", Indent());
   file.WriteString("%s{Object FeatureGroup}\n", Indent());
   file.WriteString("%s{Object FeatureGroupOrdered}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}}}\n", Indent());
}

void SRFFWriter::WritePlacementOrderSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{PlacementOrder\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PlacementOrderId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{SELECT    {\n", Indent());
   file.WriteString("%s{Object LocationGroup}\n", Indent());
   file.WriteString("%s{Object LocationGroupOrdered}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}}}\n", Indent());
}

void SRFFWriter::WritePrintSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Print\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PrintId}\n", Indent());
   file.WriteString("%s{String PrintOperation}\n", Indent());
   file.WriteString("%s{Int PrintDeposits}\n", Indent());
   file.WriteString("%s{Distance SnapOffZ}\n", Indent());
   file.WriteString("%s{Velocity SeparationSpeed}\n", Indent());
   file.WriteString("%s{Distance SeparationDistance}\n", Indent());
   file.WriteString("%s{Distance MarginAreaLengthX}\n", Indent());
   file.WriteString("%s{Distance MarginAreaLengthY}\n", Indent());
   file.WriteString("%s{Distance MarginAreaOffsetX}\n", Indent());
   file.WriteString("%s{Distance MarginAreaOffsetY}\n", Indent());
   file.WriteString("%s{Id ReferenceImageId}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WritePrinterAlignmentSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{PrinterAlignment\n", Indent());
   PushIndent();
   file.WriteString("%s{Id PrinterAlignmentId}\n", Indent());
   file.WriteString("%s{LIST    {\n", Indent());
   file.WriteString("%s{Id ReferenceImageFiducialId}\n", Indent());
   file.WriteString("%s{Id ReferenceScreenFiducialId}\n", Indent());
   PopIndent();
   file.WriteString("%s}}}\n", Indent());
}

void SRFFWriter::WriteSqueegeeSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{Squeegee\n", Indent());
   PushIndent();
   file.WriteString("%s{Id SqueegeeId}\n", Indent());
   file.WriteString("%s{Force SqueegeePressure}\n", Indent());
   file.WriteString("%s{Velocity PrintSpeed}\n", Indent());
   file.WriteString("%s{Velocity FloodSpeed}\n", Indent());
   file.WriteString("%s{Velocity SqueegeeLiftSpeed}\n", Indent());
   file.WriteString("%s{Distance SqueegeeLiftDistance}\n", Indent());
   file.WriteString("%s{Angle SqueegeeRake}\n", Indent());
   file.WriteString("%s{Distance FloodHeight}\n", Indent());
   file.WriteString("%s{Time SqueegeeLiftDelay}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteSqueegeeDefinitionSchema(CFormatStdioFile &file)
{
   file.WriteString("%s{SqueegeeDefinition\n", Indent());
   PushIndent();
   file.WriteString("%s{Id SqueegeeDefinitionId}\n", Indent());
   file.WriteString("%s{String SqueegeeRecordName}\n", Indent());
   file.WriteString("%s{Distance SqueegeeBladeHeight}\n", Indent());
   file.WriteString("%s{Angle SqueegeeRecordAngle}\n", Indent());
   file.WriteString("%s{Distance SqueegeeBladeOffset}\n", Indent());
   file.WriteString("%s{Distance SqueegeeLength}\n", Indent());
   PopIndent();
   file.WriteString("%s}\n", Indent());
}

void SRFFWriter::WriteCyberProcessSchema(CFormatStdioFile &file)
{                                                     
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    Begin CyberOPtics Process Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s{Organization CyberOptics\n", Indent());

   // Empty section, so why bother with this schema at all?
   // It is in pattern we are following, that is only reason so far.

   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s#    End CyberOPtics Process Schema Section\n", Indent());
   file.WriteString("%s#---------------------------------------------\n", Indent());
   file.WriteString("%s}\n", Indent());

}

//----------------------------------------------------------------------

SrffImageDefinition *SRFFWriter::LookupImageDefinition(BlockStruct *cczBlock, bool mirrored)
{
   SrffImageDefinition *imageDef = NULL;

   int cczBlockNum = cczBlock->getBlockNumber();
   CString cczBlockNumStr;
   cczBlockNumStr.Format("%d_[%s]", cczBlockNum, mirrored?"Bottom":"Top");
   this->m_imageDefinitionMap.lookup(cczBlockNumStr, imageDef);

   return imageDef;
}

//----------------------------------------------------------------------

SrffImageDefinition *SRFFWriter::AddImageDefinition(BlockStruct *cczBlock, bool mirrored)
{
   SrffImageDefinition *imageDef = LookupImageDefinition(cczBlock, mirrored);

   if (imageDef == NULL)
   {
      int cczBlockNum = cczBlock->getBlockNumber();
      int imageDefId = this->m_imageDefinitionMap.getSize();// + 1;
      CString imageDefName;
      if (cczBlock->getName().IsEmpty())
      {
         CString blockTypeStr( blockTypeToString(cczBlock->getBlockType()) );
         imageDefName.Format("%s_%d_[%s]", blockTypeStr, imageDefId, mirrored?"Bottom":"Top");
      }
      else
      {
         imageDefName.Format("%s_[%s]", cczBlock->getName(), mirrored?"Bottom":"Top");
      }
      imageDef = AddImageDefinition(cczBlockNum, imageDefName, mirrored);
   }

   return imageDef;
}

//----------------------------------------------------------------------

SrffImageDefinition *SRFFWriter::AddImageDefinition(int cczBlockNum, CString imageDefName, bool mirrored)
{
   int imageDefId = this->m_imageDefinitionMap.getSize();
   SrffImageDefinition *imageDef = new SrffImageDefinition(imageDefId, imageDefName);
   CString cczBlockNumStr;
   cczBlockNumStr.Format("%d_[%s]", cczBlockNum, mirrored?"Bottom":"Top");
   this->m_imageDefinitionMap.add(cczBlockNumStr, imageDef);
   return imageDef;
}

//----------------------------------------------------------------------

SrffComponentLink *SRFFWriter::AddComponentLink(BlockStruct *pcbComponentBlock)
{
   SrffComponentLink *compLink = NULL;

   if (pcbComponentBlock != NULL)
   {
      int blocknum = pcbComponentBlock->getBlockNumber();
      CString blocknumStr;
      blocknumStr.Format("%d", blocknum);
      m_componentLinkMap.lookup(blocknumStr, compLink);

      if (compLink == NULL)
      {
         int nextId = m_componentLinkMap.getSize() + 1;//.GetCount() + 1;
         CString packageName( pcbComponentBlock->getName() );
         if (packageName.IsEmpty())
            packageName.Format("Unknown%d", nextId);
         compLink = new SrffComponentLink(nextId, packageName);
         m_componentLinkMap.add(blocknumStr, compLink);
      }
   }

   return compLink;
}

//----------------------------------------------------------------------

SrffComponentDefinition *SRFFWriter::AddComponentDefinition(CString partnumber, int componentLinkId)
{
   SrffComponentDefinition *compDef = NULL;

   if (!partnumber.IsEmpty())
   {
      m_componentDefinitionMap.Lookup(partnumber, compDef);

      if (compDef == NULL)
      {
         int nextId = m_componentDefinitionMap.GetCount() + 1;
         compDef = new SrffComponentDefinition(nextId, partnumber, componentLinkId);
         m_componentDefinitionMap.SetAt(partnumber, compDef);
      }
   }

   return compDef;
}

//----------------------------------------------------------------------

SrffComponentDefinition *SRFFWriter::LookupComponentDefinition(CString partnumber)
{
   SrffComponentDefinition *compDef = NULL;
   m_componentDefinitionMap.Lookup(partnumber, compDef);
   return compDef;
}

//----------------------------------------------------------------------

SrffShape *SRFFWriter::AddShape(SrffShapeTag shapeTag, double sizeA, double sizeB)
{
   return this->m_shapeList.AddShape(shapeTag, sizeA, sizeB);
}

//----------------------------------------------------------------------

SrffShape *SRFFWriter::AddShape(BlockStruct *apBlock)
{
   SrffShape *shape = NULL;

   if (apBlock->isAperture())
   {
      ApertureShapeTag padApShape = apBlock->getShape();
      switch (padApShape)
      {
      case apertureRound:
         {
            shape = AddShape(srffShapeDisc, apBlock->getSizeA(), -1.);
         }
         break;
      case apertureSquare:
         {
            shape = AddShape(srffShapeRectangle, apBlock->getSizeA(), apBlock->getSizeA());
         }
         break;
      case apertureRectangle:
         {
            shape = AddShape(srffShapeRectangle, apBlock->getSizeA(), apBlock->getSizeB());
         }
         break;
      case apertureDonut:
         {
            shape = AddShape(srffShapeDonut, apBlock->getSizeA(), apBlock->getSizeB());
         }
         break;
      case apertureTarget:
      case apertureThermal:
         {
            shape = AddShape(srffShapeDisc, apBlock->getSizeA(), -1.);
         }
         break;


      case apertureComplex:
      case apertureOctagon:
      case apertureOblong:
         {
            // Step 1, add CyberShape aka user-defined shape.
            int cczBlockNum = apBlock->getBlockNumber();
            CString cczBlockNumStr;
            cczBlockNumStr.Format("%d", cczBlockNum);

            CyberShape *cyberShape = NULL;
            m_cyberShapeMap.lookup(cczBlockNumStr, cyberShape);

            if (cyberShape == NULL)
            {
               int nextCsId = this->m_cyberShapeMap.getSize() + 1;
               CString shapeName( apBlock->getName() );
               if (shapeName.IsEmpty())
                  shapeName.Format("VendorShape_%03d", nextCsId);

               cyberShape = new CyberShape(nextCsId, shapeName, cczBlockNum);
               this->m_cyberShapeMap.add(cczBlockNumStr, cyberShape);

               bool convertToOutline = false;
               CPolyList *tempPolyList = apBlock->getAperturePolys(m_doc.getCamCadData(), convertToOutline);
               if (tempPolyList != NULL)
               {
                  POSITION polypos = tempPolyList->GetHeadPosition();
                  while (polypos != NULL)
                  {
                     CPoly *tempPoly = tempPolyList->GetNext(polypos);
                     AddPolyToCyberShape(tempPoly, cyberShape);
                  } 
               }
               delete tempPolyList;
            }

            // Step 2, create shape with ref to CyberShape
            shape = AddShape(srffShapeVendorShapeLink, cyberShape->GetId(), 0);
         }

         break;

      case apertureBlank:
      default:
         break;
      }
   }
   else
   {
      int jj = 0;  // Not an aperture.
   }

   return shape;
}

//----------------------------------------------------------------------

SrffPatternDefinition *SRFFWriter::AddPatternDefinition(BlockStruct *geomBlock, CString patternName, int componentLinkId)
{
   SrffPatternDefinition *patDef = NULL;

   if (!patternName.IsEmpty())
   {
      m_patternDefinitionMap.Lookup(patternName, patDef);

      if (patDef == NULL)
      {
         int nextId = m_patternDefinitionMap.GetCount() + 1;
         patDef = new SrffPatternDefinition(nextId, patternName, componentLinkId);
         m_patternDefinitionMap.SetAt(patternName, patDef);

         // Add features
         POSITION insertPos = geomBlock->getHeadDataInsertPosition();
         while (insertPos != NULL)
         {
            DataStruct *insertedData = geomBlock->getNextDataInsert(insertPos);
            if (insertedData != NULL && insertedData->isInsertType(insertTypePin))
            {
               InsertStruct *pinInsert = insertedData->getInsert();

               // Put surface pad shape in shape list.
               int padShapeRef = 1; // Is defaulting to first shape as crummy failsafe.
               BlockStruct *padstack = this->GetBlockAt( pinInsert->getBlockNumber() );
               BlockStruct *padApBlock = NULL;
               InsertStruct *padInsert = NULL;
               if (padstack != NULL)
               {
                  POSITION padPos = padstack->getHeadDataInsertPosition();
                  while (padPos != NULL && padApBlock == NULL)
                  {
                     DataStruct *padData = padstack->getNextDataInsert(padPos);
                     if (padData != NULL && padData->getDataType() == dataTypeInsert)
                     {
                        int lyrIndx = padData->getLayerIndex();
                        LayerStruct *layer = m_doc.getLayerAt(lyrIndx);
                        if (layer != NULL && layer->isElectricalTop())
                        {
                           padInsert = padData->getInsert();
                           padApBlock = GetBlockAt( padInsert->getBlockNumber() );
                        }
                     }
                  }
               }
               if (padApBlock != NULL)
               {
                  SrffShape *shape = AddShape(padApBlock);
                  if (shape != NULL)
                     padShapeRef = shape->GetId();
               }


               // Put feature in feature lists.
               CString refname( pinInsert->getRefname() );
               double pinX = pinInsert->getOriginX();  //*rcf bug Not accounting for xy offset of padInsert in padstack.
               double pinY = pinInsert->getOriginY();  //* Also not the xy and rotation that may be in padApBlock
               double rotDeg = pinInsert->getAngleDegrees() + ((padInsert != NULL)?(padInsert->getAngleDegrees()):(0.));
               
               // Add to master list (of all features).
               SrffFeature *f = this->AddFeature(refname, pinX, pinY, rotDeg, padShapeRef);
               // Add to pattern-specific list.
               patDef->AddFeature(f);
            }
         }
      }
   }

   return patDef;
}

void SRFFWriter::AddPolyToCyberShape(CPoly *poly, CyberShape *shape)
{
   if (poly != NULL && shape != NULL)
   {
      // P1 gets set only when dealing with an arc, it will be the start
      // point of the arc, and have non-zero bulge.
      // Note that P2 might be start of an arc also.

      bool closed = poly->isClosed();
      bool filled = poly->isFilled();
      int ptcnt = poly->getVertexCount();

      double width = m_doc.getCamCadData().getWidth( poly->getWidthIndex() );


      // Use convert-to-outline for square widths, use SysGL width for rest (rest == round)
#ifdef WE_WILL_WANT_THIS_I_THINK
      BlockStruct *widthBlock = m_camCadData->getWidthBlock( poly->getWidthIndex() );
      if (widthBlock->getShape() == apertureSquare)
      {
         bool convertResult = poly->convertToOutline(width, 0.0, m_camCadData->getZeroWidthIndex(), widthBlock->getShape());
      }
#endif

      // Reset working width after possible conversion of outline above, since conversion of thick poly
      // to outline usually changes pen width setting.
      width = m_doc.getCamCadData().getWidth( poly->getWidthIndex() );


      if (ptcnt != poly->getVertexCount()) // poly changed
      {
         if (poly->isClosed())
            poly->setFilled(true);
      }

      CPnt *firstPt = NULL;

      CPnt *p1 = NULL;

      bool penDown = false; // Pen is up for move to 1st vertex.

      POSITION vertexPos = poly->getHeadVertexPosition();
      while (vertexPos != NULL)
      {
         CPnt *p2 = poly->getNextVertex(vertexPos);
         if (p2 != NULL)
         {
            if (firstPt == NULL)
               firstPt = p2;

            int nextSegId = this->GetNextCyberSegmentId();

            if (p1 != NULL && p1->bulge != 0.)
            {
               // Arc from p1 to p2

               double da, sa, r, cx, cy;
               da = atan(p1->bulge) * 4;
               ArcPoint2Angle(p1->x, p1->y, p2->x, p2->y, da, &cx, &cy, &r, &sa);
               //double ea = sa + da;

               // Sign of radius indicated CW/CCW, in
               // CAMCAD, sign of bulge indicates same, transfer the cw-ccw-ness.
               // For both, CCW is positive, CW is negative
               bool ccw = (p1->bulge >= 0.0);

               shape->AddSegment(nextSegId, penDown, p2->x, p2->y, ccw, cx, cy);
               penDown = true; // Pen is down from here to the end.
            }
            else
            {
               // Straight from p1 to p2, p1 is already output
               shape->AddSegment(nextSegId, penDown, p2->x, p2->y);
               penDown = true; // Pen is down, draw to all vertices following first vertex.
            }

            if (p2->bulge != 0.0)
               p1 = p2;
            else
               p1 = NULL;
         }
      }

      // guarantee perfect closure
      //if (poly->isClosed())
      //{
      //  GL_Point  ct2;
      //  ct2.x = DcaRound(firstPt->x * this->GetConversionFactor());
      //   ct2.y = DcaRound(firstPt->y * this->GetConversionFactor());
      //  crn.AddCrnToTail(ct2);
      // }

   }

}

//----------------------------------------------------------------------

SrffFeature *SRFFWriter::AddFeature(CString name, double posx, double posy, double rotDeg, int refShapeId)
{
   int nextId = this->m_featureMasterArray.GetCount() + 1;
   SrffFeature *feature = new SrffFeature(nextId, name, posx, posy, rotDeg, refShapeId);
   this->m_featureMasterArray.Add(feature);
   return feature;
}

//----------------------------------------------------------------------

SrffImage::SrffImage(int blockNum, int imageId, CString refname, double locX, double locY, double rotDeg, int imageDefinitionId)
: m_imageId(imageId)
, m_cczBlockNum(blockNum)
, m_refname(refname)
, m_positionX(locX)
, m_positionY(locY)
, m_positionZ(0.0)
, m_rotationDegreesX(0.0)
, m_rotationDegreesY(0.0)
, m_rotationDegreesZ(rotDeg)
, m_referenceImageDefinitionId(imageDefinitionId)
, m_referenceImageId(0)
, m_referenceSkipMarkId(0)
{
}

//-------------------------------------------------------------

SrffLocation::SrffLocation(int id, CString refname, double locX, double locY, double rotDeg, int componentDefinitionId, int imageDefinitionId)
: m_locationId(id)
, m_refName(refname)
, m_positionX(locX)
, m_positionY(locY)
, m_positionZ(0.0)
, m_rotationDegreesX(0.0)
, m_rotationDegreesY(0.0)
, m_rotationDegreesZ(rotDeg)
, m_referenceComponentDefinitionId(componentDefinitionId)
, m_referenceImageDefinitionId(imageDefinitionId)
{
}

//-------------------------------------------------------------

SrffImageFiducial::SrffImageFiducial(int id, CString refname, double locX, double locY, double rotDeg, int shapeId, int imageDefinitionId)
: m_id(id)
, m_refName(refname)
, m_positionX(locX)
, m_positionY(locY)
, m_rotationDegreesZ(rotDeg)
, m_referenceShapeId(shapeId)
, m_referenceImageDefinitionId(imageDefinitionId)
{
}

//-------------------------------------------------------------

SrffImageDefinition::SrffImageDefinition(int imageDefId, CString imageDefName)
: m_imageDefId(imageDefId)
, m_imageDefName(imageDefName)
{
}

void SrffImageDefinition::AddLocation(int id, CString refname, double locX, double locY, double rotDeg, int componentDefinitionId)
{
   SrffLocation *srffLoc = new SrffLocation(id, refname, locX, locY, rotDeg, componentDefinitionId, this->GetId());
   this->m_locationList.AddTail(srffLoc);
}

void SrffImageDefinition::AddFiducial(int id, CString refname, double locX, double locY, double rotDeg, int shapeId)
{
   SrffImageFiducial *srffFid = new SrffImageFiducial(id, refname, locX, locY, rotDeg, shapeId, this->GetId());
   this->m_fiducialList.AddTail(srffFid);
}


//-------------------------------------------------------------

SrffShape *SrffShapeList::Lookup(SrffShape *lookingForShape)
{
   POSITION pos = this->GetHeadPosition();
   while (pos != NULL)
   {
      SrffShape *candidate = this->GetNext(pos);
      if (*candidate == *lookingForShape)
         return candidate;
   }

   return NULL;
}

//----------------------------------------------------------------------

SrffShape *SrffShapeList::AddShape(SrffShapeTag shapeTag, double sizeA, double sizeB)
{
   SrffShape *keeper = NULL;

   int nextId = this->GetCount() + 1;
   SrffShape *shape = new SrffShape(nextId, shapeTag, sizeA, sizeB);
   SrffShape *existingShape = this->Lookup(shape);
   if (existingShape == NULL)
   {
      keeper = shape;
      this->AddTail(shape);
   }
   else
   {
      keeper = existingShape;
      delete shape;
   }

   return keeper;
}

//-------------------------------------------------------------

CString SrffShape::GetShapeObjectDescription(SRFFWriter *srffWriter)
{
   double outputUnitsFactor = srffWriter->GetUnitsFactor();

   CString desc;

   switch (this->m_shapeTag)
   {
   case srffShapeCross:
      desc = "Cross not implemented"; // No CCZ shape maps to this.
      break;
   case srffShapeDiamond:
      desc = "Diamond not implemented"; // No CCZ shape maps to this.
      break;
   case srffShapeDisc:
      desc.Format("Disc %d %s", GetId(), srffWriter->FormatValueStr(GetSizeA() * outputUnitsFactor));
      break;
   case srffShapeDonut:
      {
         double innerD = min(GetSizeA(), GetSizeB());
         double outerD = max(GetSizeA(), GetSizeB());
         desc.Format("Donut %d %s %s", GetId(), srffWriter->FormatValueStr(innerD * outputUnitsFactor), srffWriter->FormatValueStr(outerD * outputUnitsFactor));
      }
      break;
   case srffShapeRectangle:
      desc.Format("Rectangle %d %s %s", GetId(), srffWriter->FormatValueStr(GetSizeA() * outputUnitsFactor), srffWriter->FormatValueStr(GetSizeB() * outputUnitsFactor));
      break;
   case srffShapeTriangle:
      desc = "Triangle not implemented"; // No CCZ shape maps to this
      break;
   case srffShapeVendorShapeLink:
      desc.Format("VendorShapeLink %d %d", GetId(), (int)GetSizeA()); // Is an ID, don't convert units by factor.
      break;
   }

   return desc;
}

CyberSegment::CyberSegment(int id, bool penDown, double x, double y)
: m_id(id)
, m_line(true)  // is straight segment
, m_penDown(penDown)
, m_clockwise(false)
, m_positionX(x)
, m_positionY(y)
, m_arcCenterX(0.)
, m_arcCenterY(0.)
{
   // For creating straight line segment vertex.
}

CyberSegment::CyberSegment(int id, bool penDown, double x, double y, bool ccw, double centerX, double centerY)
: m_id(id)
, m_line(false)  // is arc segment
, m_penDown(penDown)
, m_clockwise(!ccw)
, m_positionX(x)
, m_positionY(y)
, m_arcCenterX(centerX)
, m_arcCenterY(centerY)
{
   // For creating straight line segment vertex.
}

void CyberSegmentList::AddSegment(int id, bool penDown, double x, double y)
{
   // For creating straight line segment vertex.
   CyberSegment *seg = new CyberSegment(id, penDown, x, y);
   this->Add(seg);
}

void CyberSegmentList::AddSegment(int id, bool penDown, double x, double y, bool ccw, double centerX, double centerY)
{
   // For creating arc segment.
   CyberSegment *seg = new CyberSegment(id, penDown, x, y, ccw, centerX, centerY);
   this->Add(seg);
}
