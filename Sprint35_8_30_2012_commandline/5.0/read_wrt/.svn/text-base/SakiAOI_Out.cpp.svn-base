
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
#include "SakiAOI_Out.h"

/*****************************************************************************/

void SakiAOI_WriteFiles(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   SakiAOIWriter writer(doc, format->Scale);

   if (writer.DetermineActiveCCFile(&doc) != NULL)
   {
      if (writer.EnforceLineConfigSettings(doc))
      {
         if(!writer.WriteFiles(filename))
         {
            ErrorMessage("CAMCAD could not finish writing the Saki AOI output files.","", MB_OK);
         }
      }
   }
}

//==========================================================================


SakiAOIWriter::SakiAOIWriter(CCEtoODBDoc &doc, double l_Scale)
: BaseAoiExporter(doc, l_Scale, fileTypeSakiAOI, new CSakiAOISettings(doc), "Saki.log")
, m_compRecordNum(1)
//*rcf , m_compAnalyzer(doc)
{
   SetUseComponentCentroids(true);  // For component locations, instead of insert location.

   //SetMirrorBottomX(true);

   //*rcf Experimental...
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


   SetMirrorBottomRotation(true);

   // For fiducial verification -- turn off.
   SetMinFiducialCount(0);
   SetMaxFiducialCount(0);

   // Cache this KW so we don't have to keep looking it up during output
   m_deviceTypeKW = doc.getCamCadData().getAttributeKeywordIndex(standardAttributeDeviceType);
}

SakiAOIWriter::~SakiAOIWriter()
{
}

int SakiAOIWriter::GetDecimals(PageUnitsTag units)

{
   // The decimal can be adjusted based on 
   // the accurancy of the calculation in various unit
   // It will affect the result of SakiAOI file
   int decimals = 5; // default. 
   
   switch (units)
   {
   case pageUnitsInches:         decimals = 3; break;
   case pageUnitsMilliMeters:    decimals = 3; break;
   case pageUnitsMils:           decimals = 0; break;
   case pageUnitsMicroMeters:    decimals = 0; break;
   case pageUnitsHpPlotter:
   case pageUnitsCentiMicroMeters:
   case pageUnitsNanoMeters:
      break;
   }
   return decimals;
}

bool SakiAOIWriter::WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName)
{
   if(!pcbData) return FALSE;

   GetProgressBar().updateStatus("Writing PCB Files.");

   bool stat1 = WritePCBFile(pcbData, boardName, false); // top
   bool stat2 = WritePCBFile(pcbData, boardName, true ); // bot

   return (stat1 && stat2);
}

bool SakiAOIWriter::WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom)
{   
   if(!pcbData) return FALSE;

   //*rcf CString fileName( this->getOutputFilename(isBottom) );
   //*rcf Can change filename getter such that side suffix and extension are
   // set in constructor, and getter has only isBottom type param, or separate
   // top and bot functions. No need to reference m_FileName from here, it is
   // in the base.
   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName = CreateOutfileName(m_FileName, sideSuffix, ".txt"); //*rcf bug, fix to correct extension.

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
   //*rcf Does saki use this?  m_usedRefnameMap.RemoveAll();
   WriteFiducialSetList(File, 1, pcbData, blockTypePcb, isBottom);
   WriteComponentList(File, pcbData, isBottom);
      
   File.Close();

   return TRUE;
}

void  SakiAOIWriter::WriteHeader(CFormatStdioFile &File, bool isBottom)
{
   CTime dateTime = CTime::GetCurrentTime();
   
   double boardSizeX = 0.;
   double boardSizeY = 0.;
   double boardOriginX = 0.;
   double boardOriginY = 0.;

   FileStruct* fileStruct = GetActiveFile();
   CExtent boardExtent = this->GetOutlineExtent(fileStruct);
   if (boardExtent.isValid())
   {
      boardSizeX = boardExtent.getXsize();
      boardSizeY = boardExtent.getYsize();
   }

   File.WriteString("Saki+\n");
   File.WriteString("Version,6.0.06.50.25xu\n");  // From a sample Saki file, no idea what it really means.
   File.WriteString("CreateDate,%s\n", dateTime.Format("%m/%d/%y")); // Sample shows just date, not time.

   CString outputUnitsStr = unitStringAbbreviation( this->GetSettings()->getOutputPageUnits() ).MakeLower();
   File.WriteString("Unit,%s\n", outputUnitsStr);

   File.WriteString("Board+\n");
   File.WriteString("BoardName,%s\n", this->m_PCBDataList.getBoardName()); // Is panel name or board name, depending on what pcbdatalist is.
   File.WriteString("GroupName,%s\n", this->GetSettings()->getGroupName()); //*rcf BUG What should this be?

   // Issue warning in log if board size exceeds apparant maximum (maximum is hearsay from field rep).
   if (getSakiAOIUnits(boardSizeX) > 330.0 || getSakiAOIUnits(boardSizeY) > 250.0)
   {
      CString msg;
      msg.Format("Warning: Board size %.3f x %.3f exceeds Saki maximum 330 x 250.",
         getSakiAOIUnits(boardSizeX), getSakiAOIUnits(boardSizeY));
      m_textLogFile.writef(msg);
   }

   File.WriteString("BoardSize_X,%.3f\n", getSakiAOIUnits(boardSizeX));
   File.WriteString("BoardSize_Y,%.3f\n", getSakiAOIUnits(boardSizeY));
   File.WriteString("BoardOrigin_X,%.3f\n", this->GetSettings()->getBoardOriginX());  // Already in output units from settings file.
   File.WriteString("BoardOrigin_Y,%.3f\n", this->GetSettings()->getBoardOriginY());  // Already in output units from settings file.
   
   File.WriteString("TotalComponents,%d\n", GetComponentCount(isBottom));

   File.WriteString("Component+\n");
   File.WriteString("Num,REF,Macro,Lib,X,Y,Theta,Sub,Flg\n");   
}

void SakiAOIWriter::WriteComponentList(CFormatStdioFile &File, BaseAoiExporterPCBData *pcbData, bool isBottom)
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

void SakiAOIWriter::WriteComponents(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, bool isBottom)
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

void  SakiAOIWriter::WriteComponentRecord(CFormatStdioFile &File, int boardIdx, BaseAoiExporterInsertData *sakiCmpData, bool isBottom)
{
   if (sakiCmpData != NULL)
   {
      //*rcf debug
      double originalX = sakiCmpData->getX();
      double originalY = sakiCmpData->getY();
      double adjustedX = applySakiOriginX(originalX);
      double adjustedY = applySakiOriginY(originalY);
      CString refname(sakiCmpData->getRefName());
      if (refname.CompareNoCase("R9901") == 0 || refname.CompareNoCase("X9906") == 0)
      {
         int jj = 0;
      }

      CString xStr,yStr,rotStr,attribute;
      xStr.Format("%.*lf", 3, applySakiOriginX(sakiCmpData->getX()));
      yStr.Format("%.*lf", 3, applySakiOriginY(sakiCmpData->getY()));
      rotStr.Format("%.*lf", 1, sakiCmpData->getRotation());
      
      int flg = GetFlg(sakiCmpData);

      // Record order to match header:  Num,REF,Macro,Lib,X,Y,Theta,Sub,Flg

      File.WriteString("%d,", m_compRecordNum++);            // Num
      File.WriteString("%s,", sakiCmpData->getRefName());    // REF
      File.WriteString("%s,", getMacroValue(sakiCmpData));   // Macro
      File.WriteString("%s,", getLibValue(sakiCmpData));     // Lib
      File.WriteString("%s,", xStr);       // X
      File.WriteString("%s,", yStr);       // Y
      File.WriteString("%s,", rotStr);     // Theta
      File.WriteString("%d,", boardIdx);   // Sub
      File.WriteString("%d\n", flg);       // Flg   
   }
}

void SakiAOIWriter::WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom)
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


CString SakiAOIWriter::getMacroValue(BaseAoiExporterInsertData *sakiCmpData)
{
   CString macroKeyword = GetSettings()->getMacroKeyword();
   return GetConfigurableValue(sakiCmpData, macroKeyword);
}
   
CString SakiAOIWriter::getLibValue(BaseAoiExporterInsertData *sakiCmpData)
{
   CString libKeyword = GetSettings()->getLibKeyword();
   return GetConfigurableValue(sakiCmpData, libKeyword);
}

int SakiAOIWriter::GetFlg(BaseAoiExporterInsertData *sakiCmpData)
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

double SakiAOIWriter::applySakiOriginX(double rawSakiX)
{
   // Incoming rawSaki coord is in Saki units, but relative to original CCZ origin.
   // Make it relative to specified Saki origin, which itself is relative to panel corner.

   // Panel corner in Saki units plus specific origin (offset) in settings file is origin for output.
   double sakiOrigin = /*this->m_outlineLLX +*/ this->GetSettings()->getBoardOriginX();
   double finalVal = rawSakiX - sakiOrigin;

   return finalVal;
}

double SakiAOIWriter::applySakiOriginY(double rawSakiY)
{
   // Incoming rawSaki coord is in Saki units, but relative to original CCZ origin.
   // Make it relative to specified Saki origin, which itself is relative to panel corner.

   // Panel corner in Saki units plus specific origin (offset) in settings file is origin for output.
   double sakiOrigin = /*this->m_outlineLLY +*/ this->GetSettings()->getBoardOriginY();
   double finalVal = rawSakiY - sakiOrigin;

   return finalVal;
}

CString SakiAOIWriter::RenameFiducial(CString fidname, int intFidID, bool isPanelFid)
{
   if (fidname.IsEmpty())
      fidname.Format("PFID%d", intFidID);
   
   if (fidname.GetLength() && fidname.GetAt(0) != 'P')
      fidname.Insert(0,'P');

   return fidname;
}

bool SakiAOIWriter::WritePanelFile(BaseAoiExporterPCBData *pcbData, bool isBottom)
{  
   if(!m_PCBDataList.IsPanel())
      return FALSE;

   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName = CreateOutfileName(m_FileName, sideSuffix, ".txt"); //*rcf bug, fix to correct extension.

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
CSakiAOISettings::CSakiAOISettings(CCEtoODBDoc &doc)
: BaseAoiExporterSettings("SakiAOI.out", doc)
{
   SetLocalDefaults();
}

CSakiAOISettings::~CSakiAOISettings()
{
}

void CSakiAOISettings::SetLocalDefaults()
{
   // From base settings
   m_PartNumberKW = ATT_PARTNUMBER;
   m_outputPageUnits = pageUnitsMilliMeters;
   m_exportUNLoaded = false;

   // Local settings
   m_originSakiUnitsX = 0.;
   m_originSakiUnitsY = 0.;
   m_macroValueFrom = QGEOM_NAME;
   m_libValueFrom   = QGEOM_NAME;

}

bool CSakiAOISettings::ParseLocalCommand(CInFile &inFile)
{
   bool isLocalCmd = false;

   if (inFile.isCommand(".MACRO_KEYWORD",2))
   {
      m_macroValueFrom = inFile.getParam(1);
      isLocalCmd = true;
   }
   else if (inFile.isCommand(".LIB_KEYWORD",2))
   {
      m_libValueFrom = inFile.getParam(1);
      isLocalCmd = true;
   }
   else if (inFile.isCommand(".GROUPNAME",2))
   {
      m_groupName = inFile.getParam(1);
      isLocalCmd = true;
   }
   else if (inFile.isCommand(".BOARD_ORIGIN", 3))
   {
      CString xStr( inFile.getParam(1) );
      CString yStr( inFile.getParam(2) );
      m_originSakiUnitsX = atof(xStr);
      m_originSakiUnitsY = atof(yStr);
      isLocalCmd = true;
   }

   return isLocalCmd;
}
