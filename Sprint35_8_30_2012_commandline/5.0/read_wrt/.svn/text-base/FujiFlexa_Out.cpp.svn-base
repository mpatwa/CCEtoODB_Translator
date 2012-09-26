
#include "stdafx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "InFile.h"
#include "centroid.h"
#include "PcbComponentPinAnalyzer.h"
#include "xform.h"
#include "PrepDlg.h"
#include "RwUiLib.h"
#include "FujiFlexa_Out.h"

void FujiFlexa_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
      FujiFlexaWriter writer(doc,format->Scale);
      if(!writer.WriteFiles(filename))
	   {
		   ErrorMessage("CAMCAD could not finish writing the Panel/PCB file.","", MB_OK);
	   }
}

/*****************************************************************************/
/* 
   Specify the Decimal of FujiFlexa file 
*/
static int FujiFlexaGetDecimals(PageUnitsTag units)

{
   // The decimal can be adjusted based on 
   // the accurancy of the calculation in various unit
   // It will affect the result of FujiFlexa file
   int decimals = 5; // default. 
   
   switch (units)
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

static CString getFujiFlexaDefaultLocation(bool isBottom)
{
   CString Location;
   Location.Format("%s,0.0,0.0,0,0,%d",(isBottom)?"LowerRight":"LowerLeft",fileTypeFujiFlexa);   
   return Location;
}

////////////////////////////////////////////////////////////
// FujiFlexaOutputAttrib
////////////////////////////////////////////////////////////
FujiFlexaOutputAttrib::FujiFlexaOutputAttrib(CCEtoODBDoc& camCadDoc) 
:  m_camCadDoc(camCadDoc)
,  m_attribKeyword(-1)
{
	SetDefaultAttribAndValue();
}

FujiFlexaOutputAttrib::~FujiFlexaOutputAttrib()
{
	m_valuesMap.RemoveAll();
}

int FujiFlexaOutputAttrib::GetKeyword() const
{
	return m_attribKeyword;
}

bool FujiFlexaOutputAttrib::HasValue(CAttributes** attributes)
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

void FujiFlexaOutputAttrib::SetKeyword(CString keyword)
{
   if(!keyword.IsEmpty())
   {
      m_attribKeyword = m_camCadDoc.RegisterKeyWord(keyword, 0, VT_STRING); 
      m_valuesMap.RemoveAll();
   }
}

void FujiFlexaOutputAttrib::AddValue(CString value)
{
	CString tmpValue = value;
	m_valuesMap.SetAt(tmpValue.MakeLower(), tmpValue.MakeLower());
}

void FujiFlexaOutputAttrib::SetDefaultAttribAndValue()
{

}

////////////////////////////////////////////////////////////
// CFujiFlexaSetting
////////////////////////////////////////////////////////////
CFujiFlexaSetting::CFujiFlexaSetting()
{
   LoadDefaultSettings();
}

CFujiFlexaSetting::~CFujiFlexaSetting()
{
}

void CFujiFlexaSetting::LoadDefaultSettings()
{
   m_PartNumberKW = ATT_PARTNUMBER;
   m_pageUnits = pageUnitsMilliMeters;
   m_exportUNLoaded = false;
   m_attribKeyword.Empty();
   m_attribValue.RemoveAll();
   m_fiducialPartnumber = "MARKE"; // Set to match constant in original implementation, i.e. is backward compatible setting.
}

bool CFujiFlexaSetting::LoadStandardSettingsFile(CString FileName)
{
   CInFile inFile;
   if(!inFile.open(FileName))
      return FALSE;

   while (inFile.getNextCommandLine())
   {
      if (inFile.isCommand(".PARTNUMBER_KEYWORD",2))
         m_PartNumberKW = inFile.getParam(1);
      else if (inFile.isCommand(".EXPORT_UNLOADED_PARTS",2))
         inFile.parseYesNoParam(1,m_exportUNLoaded,false);
      else if (inFile.isCommand(".UNITS",2))
         m_pageUnits = StringToPageUnitsTag(inFile.getParam(1));
      else if (inFile.isCommand(".FIDUCIAL_NAME",2))
         m_fiducialPartnumber = inFile.getParam(1);
      else if (inFile.isCommand(".OUTPUT_ATTRIBUTE",2)) 
         m_attribKeyword = inFile.getParam(1);
      else if (inFile.isCommand(".OUTPUT_VALUE",2))
         m_attribValue.Add(inFile.getParam(1));
   }

   inFile.close();
   return TRUE;
}

////////////////////////////////////////////////////////////
// CFujiFlexaInsertData
////////////////////////////////////////////////////////////
CFujiFlexaInsertData::CFujiFlexaInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data)
: m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
, m_InsertData(data)
{
   m_posX = 0.;
   m_posY = 0.;
   m_rotation = 0.;
   m_PartNumber.Empty();
   m_BoardType = blockTypeUndefined;
}

void CFujiFlexaInsertData::setRotation(double rotation)
{
   m_rotation = rotation;
}

////////////////////////////////////////////////////////////
// CFujiFlexaInsertDataList
////////////////////////////////////////////////////////////
CFujiFlexaInsertDataList::CFujiFlexaInsertDataList(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_topInserts(0)
, m_botInserts(0)
{
}

CFujiFlexaInsertDataList::~CFujiFlexaInsertDataList()
{
   m_topInserts = 0;
   m_botInserts = 0;
   empty();
}

void CFujiFlexaInsertDataList::Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, 
                                CString partNumber, BlockTypeTag boardType, bool placedBottom)
{
   CFujiFlexaInsertData *data = new CFujiFlexaInsertData(m_camCadDoc, InsertData);
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
// CFujiFlexaPCBData
////////////////////////////////////////////////////////////
CFujiFlexaPCBData::CFujiFlexaPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry)
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

CFujiFlexaPCBData::~CFujiFlexaPCBData()
{
   m_FidDataList.empty();
   m_CompDataList.empty();
}

////////////////////////////////////////////////////////////
// CFujiFlexaPCBDataList
////////////////////////////////////////////////////////////
CFujiFlexaPCBDataList::CFujiFlexaPCBDataList()
{
   m_Panelflag = false;
   m_BoardName = QFUJI_BOARDNAME;
}

CFujiFlexaPCBDataList::~CFujiFlexaPCBDataList()
{
   empty();
}

void CFujiFlexaPCBDataList::empty()
{
   m_Panelflag = false;
   m_BoardName = QFUJI_BOARDNAME;

   for(POSITION pcbPos = GetStartPosition(); pcbPos; )
   {
      CString pcbName;
      CFujiFlexaPCBData *pcbData =NULL;
      GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData)
         delete pcbData;
   }
   RemoveAll();
}

void CFujiFlexaPCBDataList::SetBoardAt(CString boardName, CFujiFlexaPCBData *pcbdata)
{
   CFujiFlexaPCBData *existdata = NULL;

   if(boardName.IsEmpty()) 
   {
      boardName.Format("%s_$%d", QFUJI_BOARDNAME, GetCount());
   }
   else if(Lookup(boardName,existdata))
   {
      boardName.Format("%s_$%d", boardName, GetCount());
   }

   SetAt(boardName,pcbdata);
}

////////////////////////////////////////////////////////////
// FujiFlexaWriter
////////////////////////////////////////////////////////////
FujiFlexaWriter::FujiFlexaWriter(CCEtoODBDoc &doc, double l_Scale)
: m_pDoc(doc)
, m_Scale(l_Scale)
, m_dUnitFactor(1)
, m_unit_accuracy(2)
, m_attribMap(doc)
, m_textLogFile(GetLogfilePath("fuji.log"))
{
}

FujiFlexaWriter::~FujiFlexaWriter(void)
{
   m_textLogFile.close();
}

bool FujiFlexaWriter::LoadSettingsFile()
{
   m_writeProcess.updateStatus("Loading Setting File");
   bool retVal = GetSettings().LoadStandardSettingsFile(getApp().getExportSettingsFilePath("Fuji.out"));
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
      m_textLogFile.writef("Load Settings File: Can't load file Fuji.out.\n");
   }
   return retVal;
}  

void FujiFlexaWriter::ApplyPrepSettingToFiles(FileStruct* fileStruct, CFujiFlexaPCBData *pcbData, bool isBottom, bool isPanel)
{
   //For calculating the location, Bottom side is alsways Mirrored to Top
   saveRestoreSettings OriginalSettings(&m_pDoc, fileStruct, isBottom);
   Attrib *attrib =  is_attvalue(&m_pDoc, fileStruct->getBlock()->getDefinedAttributes(), GetMachineAttributeName(fileTypeFujiFlexa,isBottom), 0);

   ExportFileSettings m_PCBLocation;
   m_PCBLocation = (attrib)?attrib->getStringValue():getFujiFlexaDefaultLocation(isBottom);
   m_PCBLocation.ApplyToCCZ(&m_pDoc, fileStruct, OriginalSettings, fileTypeFujiFlexa);   
   
   if(isPanel)
      TraversePanel(fileStruct->getTMatrix(),fileStruct->getBlock());
   else
      TraversePCBDesign(fileStruct->getBlock(), pcbData, fileStruct->getTMatrix());
   
   OriginalSettings.Restore();
}

bool FujiFlexaWriter::WriteFiles(CString filename)
{  
   bool retVal = TRUE;
   m_writeProcess.updateStatus("Writing Fuji Flexa File Format");  
   LoadSettingsFile();

   m_dUnitFactor = Units_Factor(m_pDoc.getSettings().getPageUnits(), GetSettings().getDefaultPageUnits()) * m_Scale;
   m_unit_accuracy = FujiFlexaGetDecimals(GetSettings().getDefaultPageUnits());
   m_FileName = filename;

   // generate centroid in order to get centroid of the insert.
   //generate_CENTROIDS(&m_pDoc);

    // Process PCB Design
   if (!ProcessPCBDesign()) 
   {
      // Process PCB Panel
      if(!ProcessPanel())
         retVal = FALSE;
   }

   return retVal;
}

bool FujiFlexaWriter::WritePCBFiles(CFujiFlexaPCBData *pcbData, CString boardName)
{   
   if(!pcbData) return FALSE;

   CString message;
   message.Format("Writing PCB File %s.", m_FileName);
   m_writeProcess.updateStatus(message);

   CFormatStdioFile File;
   if (!File.Open(m_FileName, CFile::modeCreate|CFile::modeWrite))
	{      
      ErrorMessage("File could not be opened", "", MB_OK);
      return FALSE;
   }

   //Write to File
   WriteHeader(File);

   //Write Top data
   WriteComponentList(File, pcbData, false);
   WriteFiducialSetList(File, pcbData, blockTypePcb,false);   
      
   //Write Bottom data
   WriteComponentList(File, pcbData, true);
   WriteFiducialSetList(File, pcbData, blockTypePcb,true); 

   File.Close();

   return TRUE;
}

bool FujiFlexaWriter::WritePanelFiles(CFujiFlexaPCBData *pcbData)
{  
   m_writeProcess.updateStatus("Writing Panel File.");

   if(!m_PCBDataList.IsPanel())
      return FALSE;

   CFormatStdioFile File;
   if (!File.Open(m_FileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage("File could not be opened", "", MB_OK);
      return FALSE;
   }

   //Write to File
   WriteHeader(File);
   
   //Write Top data
   WriteComponentList(File, NULL, false);
   WriteFiducialSetList(File, pcbData, blockTypePanel,false);   
      
   //Write Bottom data
   WriteComponentList(File, NULL, true);
   WriteFiducialSetList(File, pcbData, blockTypePanel,true);   

   File.Close();

   return TRUE;
}

void  FujiFlexaWriter::WriteHeader(CFormatStdioFile &File)
{
   File.WriteString("Board;Part Number;Pos X;Pos Y;Rotation;Ref.;Side\n");   
}

void FujiFlexaWriter::WriteComponentList(CFormatStdioFile &File, CFujiFlexaPCBData *pcbData, bool isBottom)
{
   if(pcbData)
   {
      WriteComponent(File, 1, pcbData,isBottom);
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
            WriteComponent(File, ++boardIdx, pcbData, isBottom);         
         }
      }
   }//if
}

void FujiFlexaWriter::WriteComponent(CFormatStdioFile &File, int boardIdx, CFujiFlexaPCBData *pcbData, bool isBottom)
{
   int compCnt = (pcbData)?pcbData->getComponentDataList().GetCount():0;
   CString PCBSide = (isBottom)?"Bottom":"Top";

   for(int compIndex = 0; compIndex < compCnt; compIndex++)
   {
      CFujiFlexaInsertData *cmpdata = pcbData->getComponentDataList().GetAt(compIndex);
      if(cmpdata && cmpdata->getInsertData() && cmpdata->getPlacedBottom() == isBottom)
      {
         CString xStr,yStr,rot,attribute;
         xStr.Format("%.*lf", 2, cmpdata->getX());
         yStr.Format("%.*lf", 2, cmpdata->getY());
         rot.Format("%.*lf", 1, cmpdata->getRotation());

         File.WriteString("%d;%s;%s;%s;%s;%s;%s\n", boardIdx, cmpdata->getPartNumber(), xStr, yStr, rot, cmpdata->getRefName(), PCBSide);
      }
   }//for 
}

void FujiFlexaWriter::WriteFiducialSetList(CFormatStdioFile &File, CFujiFlexaPCBData *pcbData, BlockTypeTag boardType, bool isBottom)
{
   int fidCnt = (pcbData)?pcbData->getFiducialDataList().GetCount():0;
   CString PCBSide = (isBottom)?"Bottom":"Top";

   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex++)
   {
      CFujiFlexaInsertData *fiddata = pcbData->getFiducialDataList().GetAt(fidIndex);
      if(fiddata && fiddata->getInsertData() && fiddata->getBoardType() == boardType && fiddata->getPlacedBottom() == isBottom)
      {         
         CString xStr,yStr,rot,attribute;
         xStr.Format("%.*lf", 2, fiddata->getX());
         yStr.Format("%.*lf", 2, fiddata->getY());
         rot.Format("%.*lf",1,fiddata->getRotation());
            
         File.WriteString("1;%s;%s;%s;%s;%s;%s\n", fiddata->getPartNumber(),xStr, yStr, rot, fiddata->getRefName(), PCBSide);
      }
   }   
}

bool FujiFlexaWriter::ProcessPCBDesign()
{
   FileStruct* fileStruct = m_pDoc.getFileList().GetFirstShown(blockTypePcb);
   if (!fileStruct)  return FALSE;
   
   BlockStruct* block = fileStruct->getBlock();
   if(!block)  return FALSE;

   //only one PCB data
   if(!block->getName().IsEmpty()) m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.setPanelFlag(false);   

   CFujiFlexaPCBData *pcbData = new CFujiFlexaPCBData(m_pDoc, getFujiFlexaUnits(fileStruct->getInsertX()),
      getFujiFlexaUnits(fileStruct->getInsertY()), blockTypePcb, NULL, block);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(),pcbData);

   ApplyPrepSettingToFiles(fileStruct, pcbData, false, false);          

   //PCB Data format
   WritePCBFiles(pcbData, m_PCBDataList.getBoardName());

   return TRUE;
}

bool FujiFlexaWriter::ProcessPanel()
{     
   FileStruct * fileStruct = m_pDoc.getFileList().GetOnlyShown(blockTypePanel);     
   if(!fileStruct)   return FALSE;
   
   BlockStruct* block = fileStruct->getBlock();   
   if(!block)  return FALSE;

   if(!isUniqueBoardNameOnPanel(block)) 
   {
      ErrorMessage("FujiFlexa Writer does not support multiple boards or mirrored board in panel.", "Panel Board", MB_OK);
      return FALSE;
   }

   //only one PCB data
   if(!block->getName().IsEmpty()) m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.setPanelFlag(true);

   CFujiFlexaPCBData *pcbData = new CFujiFlexaPCBData(m_pDoc, getFujiFlexaUnits(fileStruct->getInsertX()), 
      getFujiFlexaUnits(fileStruct->getInsertY()), blockTypePanel, NULL, block);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(),pcbData);
  
   ApplyPrepSettingToFiles(fileStruct, pcbData, false, true);

   //Panel format
   WritePanelFiles(pcbData);
   
   return TRUE;      
}

bool FujiFlexaWriter::TraversePCBDesign(BlockStruct* block, CFujiFlexaPCBData *pcbData, CTMatrix transformMatrix)
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
            double x = insertBasesVector.getX() * ((isMirror)?(-1):1);
            double y = insertBasesVector.getY();

            double rot = getRotationDegree(insertBasesVector.getRotationRadians(),isMirror);
            
            CString refname = data->getInsert()->getRefname();
            refname = ReNameFiducial(refname, pcbData->getFiducialDataList().GetCount());

            pcbData->getFiducialDataList().Add(data, getFujiFlexaUnits(x), getFujiFlexaUnits(y), 
               rot, refname, getPartNumber(data), blockTypePcb, isMirror);            
         }// if insertTypeFiducial
         else if (data->getInsert()->getInsertType() == insertTypePcbComponent && IsWritePart(data)) 
         {    
            if((IsLoaded(data) || GetSettings().getExportUNLoaded()) && IsWritePart(data))
            {
               double x = insertBasesVector.getX() * ((isMirror)?(-1):1);
               double y = insertBasesVector.getY();

               DataStruct *centroid = centroid_exist_in_block(m_pDoc.getBlockAt(data->getInsert()->getBlockNumber()));;
               if (centroid)
               {
                  CBasesVector centroidBasesVector = centroid->getInsert()->getBasesVector();
                  centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());
                  x = centroidBasesVector.getX() * ((isMirror)?(-1):1);
                  y = centroidBasesVector.getY();
               }
                  
               double rot = getRotationDegree(insertBasesVector.getRotationRadians(),isMirror);
               pcbData->getComponentDataList().Add(data, getFujiFlexaUnits(x), getFujiFlexaUnits(y),
                  rot, data->getInsert()->getRefname(), getPartNumber(data), blockTypePcb, isMirror);
            }
            
         }//insertTypePcbComponent

      }//load
   }//while

   return TRUE;
}

bool FujiFlexaWriter::TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block)
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
         CFujiFlexaPCBData *pcbData = NULL;
         if(m_PCBDataList.Lookup(panelName,pcbData) && pcbData)
         {
            bool isMirror = data->getInsert()->getGraphicMirrored();
            double rot = getRotationDegree(data->getInsert()->getAngle(),isMirror);
            double xStr = ((!isMirror)?1:-1)* getFujiFlexaUnits(data->getInsert()->getOriginX());
            double yStr = getFujiFlexaUnits(data->getInsert()->getOriginY());
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
         CFujiFlexaPCBData *pcbData = NULL;
      	CPoint2d compCentroid; 
         CString l_pcbName = "";
   
         BlockStruct* pcb_Block = m_pDoc.getBlockAt(data->getInsert()->getBlockNumber());         
         if (pcb_Block)
         {         
            l_pcbName = data->getInsert()->getRefname();
            bool isMirror = data->getInsert()->getGraphicMirrored();
            double xStr = getFujiFlexaUnits(data->getInsert()->getOriginX()) * ((isMirror)?-1:1);
            double yStr = getFujiFlexaUnits(data->getInsert()->getOriginY());
            double rot = getRotationDegree(data->getInsert()->getAngle(),isMirror);
            
            pcbData = new CFujiFlexaPCBData( m_pDoc, xStr, yStr,  blockTypePcb, data, pcb_Block);
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

                  insertdata->getInsert()->getCentroidLocation(m_pDoc.getCamCadData(), compCentroid);
				      l_boardMatrix.transform(compCentroid);

                  double xStr = ((!isMirror)?1:-1) * getFujiFlexaUnits(compCentroid.x);
                  double yStr = getFujiFlexaUnits(compCentroid.y);
            	   
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

bool FujiFlexaWriter::IsLoaded(DataStruct* data)
{
   bool loaded = true;

   Attrib *attrib = NULL;
	if ((attrib = is_attvalue(&m_pDoc, data->getAttributesRef(), ATT_LOADED, 1)) && attrib)
	{
		CString value = attrib->getStringValue();
      loaded = (value.CompareNoCase("true") == 0);
	}

   return loaded;
}

bool FujiFlexaWriter::IsWritePart(DataStruct* data)
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

double FujiFlexaWriter::getRotationDegree(double radious, int mirror)
{
   double insertRot = round(normalizeDegrees(RadToDeg(radious)));
   return normalizeDegrees(((mirror)?(360 -  insertRot):insertRot));
}

CString FujiFlexaWriter::getPartNumber(DataStruct* data)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_pDoc, data->getAttributeMap(), GetSettings().getPartNumberKW(), 0);
   CString partNumber = attrib?attrib->getStringValue():"No_Part_Number";

   if ((data->getInsert()->getInsertType() == insertTypeFiducial))
      partNumber = GetSettings().getFiducialPartnumber();

   return partNumber;
}

void FujiFlexaWriter::TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data)
{
   CPoint2d boardXY = data->getInsert()->getOrigin2d();
   l_panelMatrix.transform(boardXY);

   l_boardMatrix.scale(data->getInsert()->getScale() * (data->getInsert()->getGraphicMirrored()?-1:1), data->getInsert()->getScale());
   
   l_boardMatrix.rotateRadians(data->getInsert()->getAngle());

   l_boardMatrix.translate(boardXY);
}

bool FujiFlexaWriter::isUniqueBoardNameOnPanel(BlockStruct* panelblock)
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
            pcb_Block = m_pDoc.getBlockAt(blockNumber);
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

CString FujiFlexaWriter::ReNameFiducial(CString originalName, int intFidID)
{
   if(originalName.IsEmpty())
      originalName.Format("PFID%d", intFidID);
   
   if(originalName.GetLength() && originalName.GetAt(0) != 'P')
      originalName.Insert(0,'P');

   return originalName;
}