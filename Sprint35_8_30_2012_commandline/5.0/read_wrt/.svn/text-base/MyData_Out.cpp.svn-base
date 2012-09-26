// $Header: /CAMCAD/5.0/read_wrt/MyData_Out.cpp 9     01/06/09 9:58a Sharry Huang $

#include "StdAfx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "MyData_Out.h"
#include "InFile.h"
#include "centroid.h"
#include "RwUiLib.h"
#include "PcbComponentPinAnalyzer.h"
#include "xform.h"
#include "PrepDlg.h"
#include "DeviceType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//---------------------------------------------------------------------------------------
//Mydata Commands
static char *MyDataCommands[MydataCmd_Last] = 
{  
   "S","P","F", //General
   "S2","S3P","S3B","S4","S4M", // Layout
   "P2","P21","P3","P5","P6","P6B","P7","P8","P9", // Panel
   "F2","F21","F3","F5","F6","F7","F8","F9","F12", // PCB
};
#define  MYDATACMDSIZE   sizeof(MyDataCommands)/sizeof(char *)
#define  GetMydataCommand(cmdIndex) ((cmdIndex > MydataCmd_Unknown && cmdIndex < MYDATACMDSIZE)?MyDataCommands[cmdIndex]:"")
//---------------------------------------------------------------------------------------

void MyData_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
      NewMyDataWriter writer(doc,format->Scale);

      if(!writer.WriteFiles(filename))
	   {
		   ErrorMessage("CAMCAD could not finish writing the PCB file.!","", MB_OK);
	   }
}

////////////////////////////////////////////////////////////
// CMydataComponentAnalyzer
////////////////////////////////////////////////////////////
MydataComponentAnalyzer::MydataComponentAnalyzer(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
{
}

MydataComponentAnalyzer::~MydataComponentAnalyzer()
{
   //m_ComponentStructMap.empty();
}

bool MydataComponentAnalyzer::AnalyzeCentroidRotationOffset(DataStruct *InsertData, BlockStruct *geometryBlock, double &outputRotation, bool& isSingleRow, int& pinCount, CString &outMessage)
{
   // This is for Diodes and Capacitors only, as MyData does not conform to IPC-7351 for those.

   // MyData has its own way of looking at Caps (polarized) and Diodes, all the rest use
   // normal IPC-7351 rotation.
   // For diode pin 1 is the Cathode, and pin 1/cathode at top is orient 0.
   // For caps pin 1 is Negative, and pin 1/negative at the top is orient 0.
   // Also the special Diode and Cap handling is restricted to 2-pin parts.
   // Zero orientation for these is vertical, rather than horzontal like IPC-7351.

   outMessage.Empty();

   DeviceTypeTag devType = deviceTypeUnknown;
   Attrib *attrib = NULL;
   WORD devTypeKW = m_camCadDoc.RegisterKeyWord(ATT_DEVICETYPE, 0, valueTypeString);
   if (InsertData->lookUpAttrib(devTypeKW, attrib))
      devType = stringToDeviceTypeTag( attrib->getStringValue() );

   bool isDiode = (devType == deviceTypeDiode) || (devType == deviceTypeDiodeLed) || (devType == deviceTypeDiodeZener);
   bool isPolarCap = (devType == deviceTypeCapacitorPolarized) || (devType == deviceTypeCapacitorTantalum);
   if (!isDiode && !isPolarCap)
      return false;

   pinCount = geometryBlock->getPinCount();
   if (pinCount != 2)
      return false;

   isSingleRow = true;

   // We don't know in advance which is pin 1 and pin 2, so we'll call them A and Z for now.
   DataStruct *pinAData = NULL;
   DataStruct *pinZData = NULL;
   POSITION dataPos = geometryBlock->getHeadDataInsertPosition();
   while (dataPos != NULL && pinZData == NULL)
   {
      DataStruct *pinData = geometryBlock->getNextDataInsert(dataPos);
      if (pinData != NULL && pinData->isInsertType(insertTypePin))
      {
         if (pinAData == NULL)
            pinAData = pinData;
         else 
            pinZData = pinData;
      }
   }

   if (pinAData == NULL || pinZData == NULL)
      return false;

   CompPinStruct *cpA = findCompPin(InsertData->getInsert()->getRefname(), pinAData->getInsert()->getRefname());
   CompPinStruct *cpZ = findCompPin(InsertData->getInsert()->getRefname(), pinZData->getInsert()->getRefname());

   if (cpA == NULL || cpZ == NULL)
      return false;

   //int devToPkgPinMapKW = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeDeviceToPackagePinMap);
   CString cpaPinMapping;
   m_camCadDoc.getCamCadData().getAttributeStringValue(cpaPinMapping, *cpA->getAttributes(), standardAttributeDeviceToPackagePinMap);
   CString cpzPinMapping;
   m_camCadDoc.getCamCadData().getAttributeStringValue(cpzPinMapping, *cpZ->getAttributes(), standardAttributeDeviceToPackagePinMap);

   double pinAX = pinAData->getInsert()->getOriginX();
   double pinAY = pinAData->getInsert()->getOriginY();

   double pinZX = pinZData->getInsert()->getOriginX();
   double pinZY = pinZData->getInsert()->getOriginY();

   bool sameX = fpeq(pinAX, pinZX);
   bool sameY = fpeq(pinAY, pinZY);


   CString targetPinMapping( isDiode ? "Cathode" : "Negative" );

   InsertStruct *pin1 = NULL;
   InsertStruct *pin2 = NULL;
   if (cpaPinMapping.CompareNoCase(targetPinMapping) == 0)
   {
      // Pin A is Pin 1
      pin1 = pinAData->getInsert();
      pin2 = pinZData->getInsert();
   }
   else if (cpzPinMapping.CompareNoCase(targetPinMapping) == 0)
   {
      // Pin Z is Pin 1
      pin1 = pinZData->getInsert();
      pin2 = pinAData->getInsert();
   }
   else
   {
      return false;
   }

   // In MyData terms, 0 orientation is with pin 1 at top, pin 2 at bottom.
   // What we want here is the CAMCAD angle from the current as-defined 
   // 0 orient geometry (by definition geometries are defined at 0) to the
   // direction that 0 should be. I.e. relative to current geometry where is
   // the MyData 0 direction.

   if (sameX)
   {
      // Then different Y's, Definition is vertical. Is 0 or 180 case in MyData terms.
      if (pin1->getOriginY() > pin2->getOriginY())
         outputRotation = 0; // Pin 1 up top, Is already defined at MyData 0.
      else
         outputRotation = 180;  // Pin 1 at the bottom. Is opposite of MyData. +/- does not really make a difference for 180, this is actually -180.
   }
   else if (sameY)
   {
      // Then different X's, Definition is horizontal. Is 90 or 270 case in MyData terms.
      if (pin1->getOriginX() < pin2->getOriginX())
         outputRotation = -270; // Pin 1 to left, MayData 0 is CAMCAD +90 from here. Same as -270.
      else
         outputRotation = -90;  // Pin 1 to right, MyData 0 is CAMCAD -90 from here. Same as +270.
   }
   else
   {
      return false;  // Off angle geometry definition not currently supported.
   }

   return true;
}

CompPinStruct *MydataComponentAnalyzer::findCompPin(CString compRefname, CString pinName)
{
   CFileList &fileList = m_camCadDoc.getFileList();

   POSITION filePos = fileList.GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = fileList.GetNext(filePos);
      CompPinStruct *cp = file->getNetList().getCompPin(compRefname, pinName);
      if (cp != NULL)
         return cp;
   }

   return NULL;
}

double MydataComponentAnalyzer::getMyDataCentroidRotation(DataStruct* InsertData, double rotation)
{
   if(!InsertData || !InsertData->getInsert())
      return rotation;

   BlockStruct *geometryBlock = m_camCadDoc.getBlockAt(InsertData->getInsert()->getBlockNumber());   
   if(!geometryBlock)
      return rotation;

   MydataComponentInfo *compdata = NULL;
   // DO NOT CACHE !  Same geometry can be used by more than one part number, parts may be different
   // device type and/or different pin mapping. Caching just by geometry can lead to incorrect output.
   // So just don't bother, recalc each time it is needed.
   //if(!m_ComponentStructMap.Lookup(geometryBlock->getBlockNumber(), compdata) || !compdata)
   {
      // Covert the Zero Orientation from IPC7351 to Mydata 
      compdata = new MydataComponentInfo;

      DataStruct *centroidData = centroid_exist_in_block(geometryBlock);

      if (AnalyzeCentroidRotationOffset(InsertData, geometryBlock, compdata->CentroidRotation, compdata->isSingleRow, compdata->pinCount, m_message))
      {
         compdata->packageType = getPackageType(compdata->pinCount);
         compdata->Orientation = 0.;
      }
      else if(centroidData)
      {
         // Calculate IPC-7351 centroid rotation
         ::AnalyzeCentroidRotationOffset(geometryBlock, centroidData, compdata->CentroidRotation, compdata->isSingleRow, compdata->pinCount, m_message);
         compdata->packageType = getPackageType(compdata->pinCount);
         compdata->Orientation = getOrientationOffset(compdata->packageType, compdata->isSingleRow);
      }
      else
      {
         compdata->isSingleRow = true;
         compdata->CentroidRotation = 0.;
         compdata->pinCount = 0;
         compdata->packageType = getPackageType(compdata->pinCount);
         compdata->Orientation = getOrientationOffset(compdata->packageType, compdata->isSingleRow);
      }

      //m_ComponentStructMap.SetAt(geometryBlock->getBlockNumber(), compdata);
   }

   if (InsertData->getInsert()->getPlacedBottom())
      rotation = -rotation;

   // Mydata rotation is clockwise while IPC7351 is counter clockwise
    double ShiftedRotation = normalizeDegrees(360 - (rotation + compdata->Orientation + compdata->CentroidRotation));
   
   // Mydata program has drawn a line at 270 degree, so 0 <= rot < 270 is positive 
   // and 270 <= rot < 360 is negative. Based on Mydata spec, we stay at 0 <= rot < 360.
   // return (ShiftedRotation >= 270)?(ShiftedRotation - 360):ShiftedRotation
   return ShiftedRotation;
}

double MydataComponentAnalyzer::getOrientationOffset(MydataPackageType packageType, bool isSingleRow)
{   
   // Only bipolar components are 90 degree different from IPC7351
   // Please compare IPC7351 with Mydata spec
   return (packageType == MydataPkgBipolarComponent || isSingleRow)?90:0;
}

int MydataComponentAnalyzer::getComponentPinCount(BlockStruct* geometryBlock)
{
   int pinCount = 0;
   for(POSITION dataPos = (geometryBlock)?geometryBlock->getHeadDataInsertPosition():NULL;dataPos;)
   {
      DataStruct *data = geometryBlock->getNextDataInsert(dataPos);
      if (data != NULL && data->isInsertType(insertTypePin))
         pinCount ++;
   }

   return pinCount;
}

MydataPackageType MydataComponentAnalyzer::getPackageType(int pinCount)
{
   MydataPackageType packageType = MydataPkgMisc;
   switch(pinCount)
   {
   case 1: packageType = MydataPkgSinglePinComponent; break;
   case 2: packageType = MydataPkgBipolarComponent; break;
   case 3: packageType = MydataPkgTripolarComponent; break;
   default: packageType = MydataPkgMultipolarComponent; break;
   }

   return packageType;
}

//---------------------------------------------------------------------

MydataSettings::MydataSettings(CCEtoODBDoc& doc)
: BaseAoiExporterSettings("mydata.out", doc)
{
   SetLocalDefaults();
}

MydataSettings::~MydataSettings()
{
}

void MydataSettings::SetLocalDefaults()
{
}

bool MydataSettings::ParseLocalCommand(CInFile &inFile)
{
   return false;
}

//----------------------------------------------------------------------

NewMyDataWriter::NewMyDataWriter(CCEtoODBDoc &doc, double l_Scale)
: BaseAoiExporter(doc, l_Scale, fileTypeMYDATAPCB, new MydataSettings(doc), "MyData.log")
, m_compAnalyzer(doc)
{
   SetUseComponentCentroids(true);  // For component locations, instead of insert location.
   SetMirrorBottomX(false);
   SetMirrorBottomRotation(true);

   // For fiducial verification
   SetMinFiducialCount(2);
   SetMaxFiducialCount(3);
}

NewMyDataWriter::~NewMyDataWriter()
{
}

int NewMyDataWriter::GetDecimals(PageUnitsTag pageUnits)
{
   // The decimal can be adjusted based on 
   // the accurancy of the calculation in various unit
   // It will affect the result of MYDATA file
   int decimals = 5; // default. 
   
   switch (pageUnits)
   {
   case pageUnitsInches:         decimals = 5; break;
   case pageUnitsMilliMeters:    decimals = 5; break;
   case pageUnitsMils:           decimals = 0; break;
   case pageUnitsMicroMeters:    decimals = 0; break;

   case pageUnitsHpPlotter:
   case pageUnitsCentiMicroMeters:
   case pageUnitsNanoMeters:
      break;
   }
   return decimals;
}

bool NewMyDataWriter::WriteLayoutFiles()
{
   GetProgressBar().updateStatus("Writing Layout File.");

   CString topFileName = CreateOutfileName(m_FileName, QSUFFIX_TOP, QFILEEXT_LAYOUT);
   CString botFileName = CreateOutfileName(m_FileName, QSUFFIX_BOT, QFILEEXT_LAYOUT);

   //*rcf I see the following is bad form. topFile could open and botFile fail, and we
   // would return with topFile left open. Fix this later.
   CFormatStdioFile topFile, botFile;
   if (!topFile.Open(topFileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage(topFileName, "File could not be opened.", MB_OK);
      return FALSE;
   }
   if (!botFile.Open(botFileName, CFile::modeCreate|CFile::modeWrite))
   {
      topFile.Close();
      ErrorMessage(botFileName, "File could not be opened.", MB_OK);
      return FALSE;
   }

   BlockTypeTag blockType = (m_PCBDataList.IsPanel())?blockTypePanel:blockTypePcb;
   MydataCommadTag NameCmdIndx = (m_PCBDataList.IsPanel())?LayoutCmd_PanelName:LayoutCmd_BoardName;

   WriteHeader(GetMydataCommand(LayoutCmd), &topFile, topFile.GetFileName());
   WriteHeader(GetMydataCommand(LayoutCmd), &botFile, botFile.GetFileName());
   WriteBoardandFiducialMarks(LayoutCmd_Fid, NameCmdIndx, blockType, topFile, botFile);

   topFile.Close();
   botFile.Close();

   return TRUE;
}

bool NewMyDataWriter::WritePanelFiles(BaseAoiExporterPCBData *panelData)
{
   bool retvalOne = WriteMydataPNLFiles();

   BlockStruct *panelBlock = panelData->getGeometryBlock();

   //Find mixed PCB boards in Panel
   bool retvalTwo = TraversePCBsInPanel(panelBlock);

   //Verify Fiducials for PCB
   if(!VerifyFiducials(blockTypePcb))
      return false;

   //PCB Data format
   bool retvalThree = BaseAoiExporter::WritePCBFiles();

   return retvalOne && retvalTwo && retvalThree;
}

bool NewMyDataWriter::WriteMydataPNLFiles()
{  
   GetProgressBar().updateStatus("Writing Panel File.");

   CString topFileName = CreateOutfileName(m_FileName, QSUFFIX_TOP, QFILEEXT_PANEL);
   CString botFileName = CreateOutfileName(m_FileName, QSUFFIX_BOT, QFILEEXT_PANEL);

   if(!m_PCBDataList.IsPanel())
      return FALSE;

   CFormatStdioFile topFile, botFile;
   if (!topFile.Open(topFileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage(topFileName, "File could not be opened.", MB_OK);
      return false;
   }
   if (!botFile.Open(botFileName, CFile::modeCreate|CFile::modeWrite))
   {
      topFile.Close();
      ErrorMessage(botFileName, "File could not be opened.", MB_OK);
      return false;
   }

   WriteHeader(GetMydataCommand(PanelCmd),&topFile,&botFile, m_PCBDataList.getBoardName());
   WriteTools(GetMydataCommand(PanelCmd),&topFile,&botFile);

   BaseAoiExporterPCBData *pcbData = NULL;
   if(m_PCBDataList.Lookup(m_PCBDataList.getBoardName(), pcbData) && pcbData)
      WriteFiducialSetList(GetMydataCommand(PanelCmd_PanelFid),pcbData, topFile,botFile,blockTypePanel,true);  

   WritePanelTestPosition(&topFile);
   WritePanelTestPosition(&botFile);
   WriteBadBoardMarks(topFile, botFile, GetMydataCommand(PanelCmd_BadTestPos), pcbData);

   WriteBoardandFiducialMarks(PanelCmd_BoardFid, PanelCmd_BoardName, blockTypePcb, topFile,botFile);

   topFile.Close();
   botFile.Close();
   return TRUE;
}

void  NewMyDataWriter::WriteBadBoardMarks(CFormatStdioFile& topFile, CFormatStdioFile& botFile, CString command, BaseAoiExporterPCBData *pcbData)
{
   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};
   int XoutCnt[2] = {0,0};
   int count = (pcbData)?pcbData->getXoutDataList().GetCount():0;

   for(int idx = 0; idx < count; idx ++)
   {
      BaseAoiExporterInsertData *xoutdata = pcbData->getXoutDataList().GetAt(idx);         
      if(xoutdata)
      {
         bool isBottom = xoutdata->getPlacedBottom();
         if(XoutCnt[xoutdata->getPlacedBottom()] < 1)
         {
            m_inputFile[isBottom]->WriteString("%s %.*lf %.*lf\n", command, m_unit_accuracy, xoutdata->getX(), m_unit_accuracy, xoutdata->getY());
         }
         XoutCnt[isBottom] ++;
      }         
   }//for

   // If no bad board marks, set 0,0
   if(!XoutCnt[0]) m_inputFile[0]->WriteString("%s 0 0\n", command);
   if(!XoutCnt[1]) m_inputFile[1]->WriteString("%s 0 0\n", command);
}

void NewMyDataWriter::WriteFiducialSetList(CString Command, BaseAoiExporterPCBData *pcbData, CFormatStdioFile &topFile,CFormatStdioFile &botFile, 
                                        BlockTypeTag boardType, bool isPair)
{
   CFormatStdioFile *inputFile[2] = {&topFile, &botFile};
   
   int Fidcnt[2] = {0,0};
   int fidCnt = (pcbData)?pcbData->getFiducialDataList().GetCount():0;
  
   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex ++)
   {
      BaseAoiExporterInsertData *fiddata = pcbData->getFiducialDataList().GetAt(fidIndex);
      if(fiddata && fiddata->getInsertData() && fiddata->getBoardType() == boardType)
      {
         //F3 only allows 2-3 lines
         bool isBottom = (fiddata->getPlacedBottom()); 
         if(isPair)
         {
            if(Fidcnt[isBottom] < GetMaxFiducialCount())
            {
               inputFile[isBottom]->WriteString("%s %.*lf %.*lf %s\n",Command,
                  m_unit_accuracy, fiddata->getX(), m_unit_accuracy, fiddata->getY(), fiddata->getRefName());
            }
         }
         else
         {              
            int maxFids = pcbData->getFiducialDataList().getInsertsCountBySide(isBottom);
            if(maxFids > GetMinFiducialCount()) maxFids = GetMaxFiducialCount();

            if(Fidcnt[isBottom] < maxFids)
            {
               if(!(Fidcnt[isBottom] % maxFids))
                  inputFile[isBottom]->WriteString("%s",Command);
               
               inputFile[isBottom]->WriteString(" %.*lf %.*lf", m_unit_accuracy, fiddata->getX(), m_unit_accuracy, fiddata->getY());    
               
               if((Fidcnt[isBottom] % maxFids) == maxFids - 1)
                  inputFile[isBottom]->WriteString("\n");
            }
         }
         Fidcnt[isBottom]++;
      }
   }  
}


bool NewMyDataWriter::WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName)
{
   if(!pcbData) return FALSE;

   CString message;
   message.Format("Writing PCB File %s.", m_FileName);
   GetProgressBar().updateStatus(message);

   CString topFileName = CreateOutfileName(m_FileName, "_" + boardName + QSUFFIX_TOP, QFILEEXT_PCB);
   CString botFileName = CreateOutfileName(m_FileName, "_" + boardName + QSUFFIX_BOT, QFILEEXT_PCB);

   CFormatStdioFile topFile, botFile;
   if (!topFile.Open(topFileName, CFile::modeCreate|CFile::modeWrite))
	{      
      ErrorMessage(topFileName, "File could not be opened.", MB_OK);
      return FALSE;
   }
   if (!botFile.Open(botFileName, CFile::modeCreate|CFile::modeWrite))
	{
      topFile.Close();
      ErrorMessage(botFileName, "File could not be opened.", MB_OK);
      return FALSE;
   }

   //Write to File
   WriteHeader(GetMydataCommand(PCBCmd), &topFile, &botFile, boardName);
   WriteTools(GetMydataCommand(PCBCmd),&topFile,&botFile);

   WriteFiducialSetList(GetMydataCommand(PCBCmd_Fid), pcbData, topFile, botFile, blockTypePcb, true);   
   
   WritePCBTestPosition(&topFile);
   WritePCBTestPosition(&botFile);
   WriteBadBoardMarks(topFile,botFile, GetMydataCommand(PCBCmd_BadTestPos), pcbData);

   WriteComponentList(pcbData, topFile,botFile);
   
   topFile.Close();
   botFile.Close();

   return TRUE;
}

void NewMyDataWriter::WriteComponentList(BaseAoiExporterPCBData *pcbData, CFormatStdioFile &topFile, CFormatStdioFile &botFile)
{
   CFormatStdioFile *inputFile[2] = {&topFile, &botFile};
   int compCnt = (pcbData)?pcbData->getComponentDataList().GetCount():0;

   GetProgressBar().updateStatus("Writing Component List.");
   GetProgressBar().setLength(compCnt);

   for(int compIndex = 0; compIndex < compCnt; compIndex++)
   {
      GetProgressBar().incrementProgress();

      BaseAoiExporterInsertData *cmpdata = pcbData->getComponentDataList().GetAt(compIndex);
      if(cmpdata && cmpdata->getInsertData())
      {
         bool isBottom = (cmpdata->getPlacedBottom());
         CString mount_skip =    isLoaded(cmpdata->getInsertData())?"N":"Y";
         CString dispense_skip = "Y"; // Previously: mount_skip;   Per email from Mark 5 June 2012 "Set second default N in Mydata output to Y "

         double mydataRotation = m_compAnalyzer.getMyDataCentroidRotation(cmpdata->getInsertData(), cmpdata->getRotation());
         if (!m_compAnalyzer.getMessage().IsEmpty())
            this->m_textLogFile.writef("Component Analyzer reports: %s encountered: %s\n", 
            cmpdata->getInsertData()->getInsert()->getRefname(), m_compAnalyzer.getMessage());
         mydataRotation *= DEGREESCALE;

         inputFile[isBottom]->WriteString("%s %.*lf %.*lf %.0f 0 %s %s %s\n", GetMydataCommand(PCBCmd_MountPosition), m_unit_accuracy, cmpdata->getX(), 
            m_unit_accuracy, cmpdata->getY(), mydataRotation, mount_skip, dispense_skip, cmpdata->getPartNumber());

         inputFile[isBottom]->WriteString("%s %s\n",GetMydataCommand(PCBCmd_MountPosName), cmpdata->getRefName());
      }
   }   
}

void  NewMyDataWriter::WriteHeader(CString Command, CFormatStdioFile* topFile, CFormatStdioFile* botFile, CString boardName)
{
   WriteHeader(Command,topFile,boardName + "_TOP");
   WriteHeader(Command,botFile,boardName + "_BOT");
}

void  NewMyDataWriter::WriteHeader(CString Command, CFormatStdioFile *m_inputFile, CString boardName)
{
   m_inputFile->WriteString("%s1 %s\n",Command, boardName);
   
   CCEtoODBApp  *m_cadApp = (CCEtoODBApp*)(AfxGetApp());
   m_inputFile->WriteString("%s2 Created by CCE to ODB++ Version %s\n", Command, m_cadApp->getVersionString());
   
   PageUnitsTag pageUnits = GetSettings()->getOutputPageUnits();
   CString unitText = getUnitString(pageUnits);
   if(!unitText.IsEmpty())
      m_inputFile->WriteString("U %s\n",unitText);
}

void  NewMyDataWriter::WriteTools(CString Command, CFormatStdioFile* topFile, CFormatStdioFile* botFile)
{
   CString dataline;
   
   dataline.Format("%s21 All_Tools\n",Command);
   WriteDataLine(topFile,botFile,dataline);
}

void  NewMyDataWriter::WritePCBTestPosition(CFormatStdioFile *outFile)
{   
   outFile->WriteString("%s 0 0\n", GetMydataCommand(PCBCmd_GlueTestPos));
   outFile->WriteString("%s 0 0\n", GetMydataCommand(PCBCmd_MountTestPos));
}

void  NewMyDataWriter::WritePanelTestPosition(CFormatStdioFile *outFile)
{
   outFile->WriteString("%s 0 0\n", GetMydataCommand(PanelCmd_GlueTestPos));
   outFile->WriteString("%s 0 0\n", GetMydataCommand(PanelCmd_MountTestPos));
}

void NewMyDataWriter::WriteAuxiliaryPCBData(CFormatStdioFile *outFile)
{
   outFile->WriteString("%s 0.200000 0 0\n", GetMydataCommand(PCBCmd_AuxiliaryData));
}

void  NewMyDataWriter::WriteAuxiliaryPanelData(CFormatStdioFile *m_inputFile)
{
   m_inputFile->WriteString("%s OFF N 0 N 2000 2000 0.200000 0 Y\n", GetMydataCommand(PanelCmd_AuxiliaryData));
}

void  NewMyDataWriter::WriteBoardandFiducialMarks(MydataCommadTag fidCmdIdx, MydataCommadTag boardCmdIdx, BlockTypeTag boardType, CFormatStdioFile &topFile,CFormatStdioFile &botFile)
{     
   CString fidCmd = GetMydataCommand(fidCmdIdx);
   CString boardNameCmd = GetMydataCommand(boardCmdIdx);

   CTypedMapIntToPtrContainer<BlockStruct*> stencilBlockMap;
      // skip the panel information   
   for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
   {
      CString pcbName;
      BaseAoiExporterPCBData *pcbData =NULL;
      m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData && pcbData->getGeometryBlock() && pcbData->getBoardType() == boardType)
      {
         BlockStruct *block = NULL;
         int blockNumber = pcbData->getGeometryBlock()->getBlockNumber();
                 
         if(!stencilBlockMap.Lookup(blockNumber,block))
         {
            stencilBlockMap.SetAt(blockNumber,pcbData->getGeometryBlock());
            
            CString dataLine;
            dataLine.Format("%s %s_TOP\n",boardNameCmd, pcbData->getGeometryBlock()->getName());
            WriteDataLine(&topFile,dataLine);
            dataLine.Format("%s %s_BOT\n",boardNameCmd, pcbData->getGeometryBlock()->getName());
            WriteDataLine(&botFile,dataLine);
         }

         WriteFiducialSetList(fidCmd, pcbData, topFile,botFile,boardType,false);
      }
   }//for

   stencilBlockMap.RemoveAll();
}


