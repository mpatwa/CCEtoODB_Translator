// $Header: /CAMCAD/5.0/read_wrt/MyData_Out.cpp 9     01/06/09 9:58a Sharry Huang $

#include "StdAfx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "BaseAoiExporter.h"
#include "InFile.h"
#include "centroid.h"
#include "RwUiLib.h"
#include "PcbComponentPinAnalyzer.h"
#include "xform.h"
#include "PrepDlg.h"
#include "MultipleMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString BaseAoiExporter::CreateOutfileName(CString basename, CString filenameSuffix, CString filenameExt)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath(basename, drive, dir, fname, ext );

	if ((strlen(fname) + strlen(filenameSuffix)) >= _MAX_FNAME)
		fname[ _MAX_FNAME - strlen(filenameSuffix) - 1] = '\0';

	strcat(fname, filenameSuffix);

   char constructedName[_MAX_PATH];

   _makepath(constructedName, drive, dir, fname, filenameExt);

	return constructedName;
}

CString BaseAoiExporter::GetDefaultLocation(bool isBottom)
{
   CString Location;
   Location.Format("%s,0.0,0.0,0,0,%d",(isBottom)?"LowerRight":"LowerLeft", m_exporterFileTypeTag);   
   return Location;
}

/*****************************************************************************/
/* 
   Specify the Decimal of MYDATA file 
   //*rcf Maybe should move this to be under Settings, so we can have settings
   // file standard support for number of decimal places.
*/
int BaseAoiExporter::GetDecimals(PageUnitsTag pageUnits)
{
   int decimals = 5; // default. 
   
   switch (pageUnits)
   {
   case pageUnitsInches:         decimals = 3; break;
   case pageUnitsMilliMeters:    decimals = 2; break;
   case pageUnitsMils:           decimals = 0; break;
   case pageUnitsMicroMeters:    decimals = 0; break;

   case pageUnitsHpPlotter:
   case pageUnitsCentiMicroMeters:
   case pageUnitsNanoMeters:
      break;
   }
   return decimals;
}


////////////////////////////////////////////////////////////
// BaseAoiExporterSettings
////////////////////////////////////////////////////////////
BaseAoiExporterSettings::BaseAoiExporterSettings(CString filename, CCEtoODBDoc& camCadDoc)
: m_settingsFilename(filename)
, m_attribMap(camCadDoc)
{
   SetDefaults();
}

void BaseAoiExporterSettings::SetDefaults()
{
   m_PartNumberKW = ATT_PARTNUMBER;
   m_outputPageUnits = pageUnitsMicroMeters;
   m_outputPageUnitsScale = 1.0;
   m_exportUNLoaded = true;

   m_attribMap.SetDefaultAttribAndValue();

   // Don't call SetLocalDefaults() from here. The subclass constructor is not
   // finished, so may not be ready to accept calls and changes.
   // Just make sure it is called before parsing.
}

bool BaseAoiExporterSettings::LoadSettingsFile()
{
   CString FileName( getApp().getExportSettingsFilePath( m_settingsFilename ) );

   CInFile inFile;
   if(!inFile.open(FileName))
      return FALSE;

   SetDefaults();
   SetLocalDefaults();
   while (inFile.getNextCommandLine())
   {
      if (inFile.isCommand(".PARTNUMBER_KEYWORD",2)) 
         m_PartNumberKW = inFile.getParam(1);
      else if (inFile.isCommand(".EXPORT_UNLOADED_PARTS",2))
         inFile.parseYesNoParam(1,m_exportUNLoaded,true);
      else if (inFile.isCommand(".UNITS",2))
         m_outputPageUnits = StringToPageUnitsTag(inFile.getParam(1));
      else if (inFile.isCommand(".OUTPUT_ATTRIBUTE",2)) 
         m_attribMap.SetKeyword(inFile.getParam(1));
      else if (inFile.isCommand(".OUTPUT_VALUE",2))
         m_attribMap.AddValue(inFile.getParam(1));
      else
         ParseLocalCommand(inFile); // None of the above, so allow subclass a go at parsing.
   }

   inFile.close();
   return TRUE;
}

////////////////////////////////////////////////////////////
// BaseAoiExporter
////////////////////////////////////////////////////////////
BaseAoiExporter::BaseAoiExporter(CCEtoODBDoc &doc, double l_Scale, FileTypeTag exporterFileType, BaseAoiExporterSettings *settingsHandler, CString logFilename)
: m_pDoc(doc)
, m_Scale(l_Scale)
, m_exporterFileTypeTag(exporterFileType)
//deadcode , m_attribMap(doc)
, m_dUnitFactor(1.0)
, m_unit_accuracy(0)
, m_useComponentCentroids(true)
, m_minFiducialCount(0)
, m_maxFiducialCount(0)
, m_flatten(true)
, m_convertUnits(true)
, m_outSettings(settingsHandler)
, m_textLogFile(GetLogfilePath(logFilename))
, m_activeFile(NULL)
{
   m_PCBDataList.RemoveAll();

   // If subclass did not provide settings handler then provide default one.
   if (m_outSettings == NULL)
      m_outSettings = new BaseAoiExporterSettings("fake.out", doc); // Not a real settings file, but this provides the interfaces needed in rest of code.

   WriteLogHeader();
}

BaseAoiExporter::~BaseAoiExporter()
{
   m_PCBDataList.empty();
   m_textLogFile.close();

   if (m_outSettings != NULL)
      delete m_outSettings;

   m_usedRefnameMap.RemoveAll();  //*rcf MemLeak, this needs to be a container or we need to delete the content here, not just remove all
}

void BaseAoiExporter::WriteLogHeader()
{
   CTime t;
   t = t.GetCurrentTime();
   CString sTime( t.Format("%m/%d/%Y  %H:%M:%S") );

   CString prettyExporterType( fileTypeTagToPrettyString(m_exporterFileTypeTag) );

   m_textLogFile.writef("# %s Log File Generated By CAMCAD\n", prettyExporterType);
   m_textLogFile.writef("# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   m_textLogFile.writef("# %s\n", sTime);
   m_textLogFile.writef("# ---------------------------------------\n\n");
}


bool BaseAoiExporter::LoadSettingsFile()
{
   m_exportProgress.updateStatus("Loading Setting File");
   return GetSettings()->LoadSettingsFile();
}

int BaseAoiExporter::GetComponentCount(bool isBottom)
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

bool BaseAoiExporter::WriteFiles(CString filename)
{  
   bool relVal = TRUE;
   m_exportProgress.updateStatus("Writing MyData Format"); //*rcf BUG should not be MyData, get name from subclass somehow 
   LoadSettingsFile();

   PageUnitsTag cczPU = m_pDoc.getSettings().getPageUnits();
   PageUnitsTag outPU = GetSettings()->getOutputPageUnits();

   double pageUnitsFactor = Units_Factor(m_pDoc.getSettings().getPageUnits(), GetSettings()->getOutputPageUnits()) * m_Scale;
   m_dUnitFactor =  pageUnitsFactor * GetSettings()->getOutputPageUnitsScale();
   m_unit_accuracy = GetDecimals(GetSettings()->getOutputPageUnits());
   m_FileName = filename; //*rcf fix m_FileName to be more clearly named for Output Filename

   // generate centroid in order to get centroid of the insert.
   if (GetUseComponentCentroids())
      generate_CENTROIDS(&m_pDoc);

   // Clear unique refname map.
   m_usedRefnameMap.RemoveAll();

    // Process PCB Design
   if (!ProcessPCBDesign()) 
   {
      // Process PCB Panel
      if(!ProcessPanel())
         relVal = FALSE;
   }


   return relVal;
}



bool BaseAoiExporter::WritePCBFiles()
{
   for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
   {
      CString pcbName;
      BaseAoiExporterPCBData *pcbData = NULL;
      m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData && pcbData->getGeometryBlock() && pcbData->getBoardType() == blockTypePcb)
      {
         WritePCBFiles(pcbData, pcbData->getGeometryBlock()->getName());
      }
   }

   return TRUE;
}


void BaseAoiExporter::WriteDataLine(CFormatStdioFile* topFile, CFormatStdioFile* botFile, CStringArray &DataList)
{
   for(int idx = 0 ;idx < DataList.GetCount(); idx++)
      WriteDataLine(topFile, botFile, DataList.GetAt(idx));     
}

void BaseAoiExporter::WriteDataLine(CFormatStdioFile* topFile, CFormatStdioFile* botFile, CString DataLine)
{
   WriteDataLine(topFile, DataLine);
   WriteDataLine(botFile, DataLine);
}

void BaseAoiExporter::WriteDataLine(CFormatStdioFile *baseFile, CStringArray &DataList)
{
   for(int idx = 0 ;idx < DataList.GetCount(); idx++)
      WriteDataLine(baseFile, DataList.GetAt(idx));     
}

void BaseAoiExporter::WriteDataLine(CFormatStdioFile *baseFile, CString DataLine)
{
   baseFile->WriteString (DataLine);
}

//-----------------------------------------------------------------------------------------------

FileStruct *BaseAoiExporter::DetermineActiveCCFile(CCEtoODBDoc *doc)
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

//-----------------------------------------------------------------------------------------------

bool BaseAoiExporter::HasLineConfigSettings(FileStruct *file, bool isBottom)
{
   Attrib *attrib =  is_attvalue(&m_pDoc, file->getBlock()->getDefinedAttributes(), GetMachineAttributeName(m_exporterFileTypeTag,isBottom), 0);
   if (attrib != NULL)
   {
      CString val(attrib->getStringValue());
      // Not validating content, if val is not empty we'll accept it as a setting.
      if (!val.IsEmpty())
         return true;
   }

   return false;
}

//-----------------------------------------------------------------------------------------------

bool BaseAoiExporter::EnforceLineConfigSettings(CCEtoODBDoc &doc)
{
   // Check for presence of Line Config settings, if not present then open LC
   // dialog. Return true is both top and bottom LC settings are or become
   // present, otherwise false.

   if (DetermineActiveCCFile(&doc) != NULL)
   {
      FileStruct *file = DetermineActiveCCFile(&doc);
      bool hasTopLC = HasLineConfigSettings(file, false);
      bool hasBotLC = HasLineConfigSettings(file, true);

      if (!hasTopLC || !hasBotLC)
      {
         ShowProgressDialog(false); // Hide it while prepdlg is up.
         CMachine theMachine( this->m_exporterFileTypeTag );
         while (!hasTopLC || !hasBotLC)
         {
            CString mchName(theMachine.GetName());
            bool isBothSide = theMachine.IsBothSideSetting();
            CPrepDlg prepdlg(mchName, isBothSide);
            if(prepdlg.DoModal() != IDOK)
            {
               return false;
            }

            hasTopLC = HasLineConfigSettings(file, false);
            hasBotLC = HasLineConfigSettings(file, true);

            if (!hasTopLC || !hasBotLC)
            {
               CString msg1("Line Configuration settings are required for CyberOptics export.\n");
               CString msg2, msg3;
               msg2.Format("The setting is %s for top side.\n", hasTopLC?"present":"missing");
               msg3.Format("The setting is %s for bottom side.\n", hasBotLC?"present":"missing");
               ErrorMessage(msg1+msg2+msg3);
            }
         }
         ShowProgressDialog(true);
      }

      return hasTopLC && hasBotLC;
   }

   return false;
}

//-----------------------------------------------------------------------------------------------

void BaseAoiExporter::ApplyPrepSettingToFiles(FileStruct* fileStruct, BaseAoiExporterPCBData *pcbData, bool isBottom, bool isPanel)
{
   //For calculating the location, Bottom side is alsways Mirrored to Top
   saveRestoreSettings OriginalSettings(&m_pDoc, fileStruct, isBottom);
   Attrib *attrib =  is_attvalue(&m_pDoc, fileStruct->getBlock()->getDefinedAttributes(), GetMachineAttributeName(m_exporterFileTypeTag,isBottom), 0);

   ExportFileSettings m_PCBLocation;
   m_PCBLocation = (attrib)?attrib->getStringValue():GetDefaultLocation(isBottom);
   m_PCBLocation.ApplyToCCZ(&m_pDoc, fileStruct, OriginalSettings, m_exporterFileTypeTag);   
   
   if (isPanel)
      TraversePanel(fileStruct->getTMatrix(),fileStruct->getBlock(), isBottom);
   else
      TraversePCBDesign(fileStruct->getBlock(), pcbData, fileStruct->getTMatrix(),isBottom);

   OriginalSettings.Restore();
}

bool BaseAoiExporter::ProcessPCBDesign()
{
   //*rcf The GetFirstShown needs to change to getActiveFile style, e.g. a la (original) Saki, maybe Omron.
   FileStruct* fileStruct = m_pDoc.getFileList().GetFirstShown(blockTypePcb);
   if (!fileStruct)  return FALSE;
   
   BlockStruct* block = fileStruct->getBlock();
   if(!block)  return FALSE;

   //only one PCB data
   m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.setPanelFlag(false);   

   BaseAoiExporterPCBData *pcbData = new BaseAoiExporterPCBData(m_pDoc, ConvertToExporterUnits(fileStruct->getInsertX()),
      ConvertToExporterUnits(fileStruct->getInsertY()), blockTypePcb, NULL, block);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(),pcbData);

   ApplyPrepSettingToFiles(fileStruct, pcbData, false, false);          
   ApplyPrepSettingToFiles(fileStruct, pcbData, true, false);          

   if(!VerifyFiducials(blockTypePcb))  return FALSE;

   WriteLayoutFiles();  //*rcf So far this is a MyData-only thing.

   //PCB Data format
   WritePCBFiles(pcbData, m_PCBDataList.getBoardName());

   return TRUE;
}

bool BaseAoiExporter::TraversePCBDesign(BlockStruct* block, BaseAoiExporterPCBData *pcbData, CTMatrix transformMatrix, bool isBottom)
{
   if(!pcbData)
      return false; 

   m_exportProgress.updateStatus("Processing PCB Board.");

   for (POSITION dataPos = (block)?block->getHeadDataInsertPosition():NULL;dataPos;)
   {  
      DataStruct* data = block->getNextDataInsert(dataPos);

      if(data && !data->getInsert()->getGraphicMirrored())
      {
         CBasesVector insertBasesVector = data->getInsert()->getBasesVector();
         insertBasesVector.transform(transformMatrix);

         InsertTypeTag insertType = data->getInsert()->getInsertType();

         if (insertType == insertTypeFiducial || insertType == insertTypeXout)
         {                    
            double x = insertBasesVector.getX();
            double y = insertBasesVector.getY();

            if (isBottom && GetMirrorBottomX())
               x = -x;

            double rot = getRotationDegrees(insertBasesVector.getRotationRadians(),data->getInsert()->getGraphicMirrored());            
            CString refname = data->getInsert()->getRefname();
            CString partNumber = getPartNumber(data);
            switch(insertType)
            {
            case insertTypeFiducial:
               refname = RenameFiducial(refname, pcbData->getFiducialDataList().GetCount(), false);

               pcbData->getFiducialDataList().Add(data, ConvertToExporterUnits(x), ConvertToExporterUnits(y), 
                  rot, refname, partNumber, blockTypePcb, isBottom);
               break;

            case insertTypeXout:
               pcbData->getXoutDataList().Add(data, ConvertToExporterUnits(x), ConvertToExporterUnits(y), 
                  rot, refname, partNumber, blockTypePcb, isBottom);     
               break;
            }

         }
         else if (insertType == insertTypePcbComponent) 
         {
            // Loaded test is "okay" if part is loaded OR we are exporting unloaded parts.
            bool loadedOkay = isLoaded(data) || GetSettings()->getExportUNLoaded();

            if (loadedOkay && isWritePart(data))
            {
               double x = insertBasesVector.getX();
               double y = insertBasesVector.getY();

               if (GetUseComponentCentroids())
               {
                  DataStruct *centroid = centroid_exist_in_block(m_pDoc.getBlockAt(data->getInsert()->getBlockNumber()));;
                  if (centroid)
                  {
                     CBasesVector centroidBasesVector = centroid->getInsert()->getBasesVector();
                     centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());
                     x = centroidBasesVector.getX();
                     y = centroidBasesVector.getY();
                  }
               }

               if (isBottom && GetMirrorBottomX())
                  x = -x;
                
               double rot = getRotationDegrees(insertBasesVector.getRotationRadians(),isBottom);
               pcbData->getComponentDataList().Add(data, ConvertToExporterUnits(x), ConvertToExporterUnits(y),
                  rot, data->getInsert()->getRefname(), getPartNumber(data), blockTypePcb, isBottom);
            }
            
         }

      }//load
   }//while

   return true;
}

bool BaseAoiExporter::ProcessPanel()
{
   bool retval = false;

   switch (m_exporterFileTypeTag)
   {
   case fileTypeCyberOpticsAOI:
   case fileTypeYestechAOI:
   case fileTypeSakiAOI:
      retval = ProcessPanelCyberStyle();
      break;

   default:
      retval = ProcessPanelOriginalStyle();
      break;
   }

   return retval;
}

bool BaseAoiExporter::ProcessPanelOriginalStyle()
{     
   FileStruct * fileStruct = m_pDoc.getFileList().GetOnlyShown(blockTypePanel);     
   if(!fileStruct)   return false;
   
   BlockStruct* panelBlock = fileStruct->getBlock();   
   if(!panelBlock)  return false;

   //only one PCB data
   m_PCBDataList.setBoardName(panelBlock->getName());
   m_PCBDataList.setPanelFlag(true);

   BaseAoiExporterPCBData *panelData = new BaseAoiExporterPCBData(m_pDoc, ConvertToExporterUnits(fileStruct->getInsertX()), 
      ConvertToExporterUnits(fileStruct->getInsertY()), blockTypePanel, NULL, panelBlock);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(), panelData);
  
   ApplyPrepSettingToFiles(fileStruct, panelData, false, true);
   ApplyPrepSettingToFiles(fileStruct, panelData, true, true);

   //Verify Fiducials for Panel
   if(!VerifyFiducials(blockTypePanel))
      return false;

   //Write Layout format
   WriteLayoutFiles();  //*rcf So far this is Mydata only, maybe should move it.

   //Panel format
   bool retval = WritePanelFiles(panelData);  // pcbData is really panel data

   return retval;      
}

bool BaseAoiExporter::ProcessPanelCyberStyle()
{     
   FileStruct * fileStruct = m_pDoc.getFileList().GetOnlyShown(blockTypePanel);     
   if (!fileStruct)
      return false;
   
   BlockStruct* panelBlock = fileStruct->getBlock();   
   if (!panelBlock)
      return false;

   // --------------------------------------------------------------
   // TOP
   //
   m_PCBDataList.empty();
   m_PCBDataList.setBoardName(panelBlock->getName());
   m_PCBDataList.setPanelFlag(true);

   BaseAoiExporterPCBData *panelData = new BaseAoiExporterPCBData(m_pDoc, ConvertToExporterUnits(fileStruct->getInsertX()), 
      ConvertToExporterUnits(fileStruct->getInsertY()), blockTypePanel, NULL, panelBlock);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(), panelData);
  
   ApplyPrepSettingToFiles(fileStruct, panelData, false, true);
 
   //Verify Fiducials for Panel
   if(!VerifyFiducials(blockTypePanel))
      return false;

   //Write Layout format
   WriteLayoutFiles();  //*rcf So far this is Mydata only, maybe should move it.

   //Panel format
   bool retvalTop = WritePanelFile(panelData, false);

   // --------------------------------------------------------------
   // BOTTOM
   //
   m_PCBDataList.empty();
   m_PCBDataList.setBoardName(panelBlock->getName());
   m_PCBDataList.setPanelFlag(true);

   /*BaseAoiExporterPCBData */panelData = new BaseAoiExporterPCBData(m_pDoc, ConvertToExporterUnits(fileStruct->getInsertX()), 
      ConvertToExporterUnits(fileStruct->getInsertY()), blockTypePanel, NULL, panelBlock);
   m_PCBDataList.SetBoardAt(m_PCBDataList.getBoardName(), panelData);
  
   ApplyPrepSettingToFiles(fileStruct, panelData, true, true);

   //Verify Fiducials for Panel
   if(!VerifyFiducials(blockTypePanel))
      return false;

   //Write Layout format
   WriteLayoutFiles();  //*rcf So far this is Mydata only, maybe should move it.

   //Panel format
   bool retvalBot = WritePanelFile(panelData, true);

   return retvalTop && retvalBot;      
}

void BaseAoiExporter::TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block, bool isBottom)
{
   // May need to eventually make this configurable. For now, we do it one way
   // for MyData and another way for all the rest.

   if (m_exporterFileTypeTag == fileTypeMYDATAPCB) //  ||  m_exporterFileTypeTag == fileTypeCyberOpticsAOI)
      TraversePanelMydataStyle(l_panelMatrix, block, isBottom);
   else
      TraversePanelSakiStyle(l_panelMatrix, block, isBottom);

}

void BaseAoiExporter::TraversePanelMydataStyle(CTMatrix &l_panelMatrix, BlockStruct* block, bool isBottom)
{   
   m_exportProgress.updateStatus("Processing Panel Board.");
   CString panelName = m_PCBDataList.getBoardName();   

   for (POSITION dataPos = (block)?block->getHeadDataInsertPosition():NULL;dataPos;)
   {
      DataStruct* data = block->getNextDataInsert(dataPos);
      if(data)
      {
         CBasesVector insertBasesVector = data->getInsert()->getBasesVector();
         insertBasesVector.transform(l_panelMatrix);

         bool isMirror = data->getInsert()->getGraphicMirrored();
         InsertTypeTag insertType = data->getInsert()->getInsertType();
         if ((insertType == insertTypeFiducial || insertType == insertTypeXout)&& !isMirror)
		   {
            BaseAoiExporterPCBData *pcbData = NULL;
            if(m_PCBDataList.Lookup(panelName,pcbData) && pcbData )
            {
               double rot = getRotationDegrees(insertBasesVector.getRotationRadians(), isMirror);      			                 
               CString refname = data->getInsert()->getRefname();
               CString partNumber = getPartNumber(data); 

               switch(insertType)
               {
               case insertTypeFiducial:
                  refname = RenameFiducial(refname, pcbData->getFiducialDataList().GetCount(), true);
                  partNumber = "FID"; 	

                  pcbData->getFiducialDataList().Add(data,ConvertToExporterUnits(insertBasesVector.getX()) * (data->getInsert()->getGraphicMirrored()?-1:1),
                     ConvertToExporterUnits(insertBasesVector.getY()), rot, refname, partNumber, blockTypePanel, isBottom);
                  break;
               case insertTypeXout:                  	
                  pcbData->getXoutDataList().Add(data,ConvertToExporterUnits(insertBasesVector.getX()) * (data->getInsert()->getGraphicMirrored()?-1:1),
                  ConvertToExporterUnits(insertBasesVector.getY()), rot, refname, partNumber, blockTypePanel, isBottom);
                  break;
               }
            }
		   }         
         else if (insertType == insertTypePcb)
         { 
            CString l_pcbName = data->getInsert()->getRefname();
            BlockStruct* pcb_Block = m_pDoc.getBlockAt(data->getInsert()->getBlockNumber());
            
            BaseAoiExporterPCBData *pcbData = NULL;
            if(!m_PCBDataList.Lookup(l_pcbName,pcbData) || !pcbData)
            {
               pcbData = new BaseAoiExporterPCBData(m_pDoc, ConvertToExporterUnits(insertBasesVector.getX()) * ((data->getInsert()->getGraphicMirrored())?-1:1),
                  ConvertToExporterUnits(insertBasesVector.getY()), blockTypePcb, data, pcb_Block);
               m_PCBDataList.SetBoardAt(l_pcbName,pcbData);
            }

            for (POSITION pcb_dataPos = (pcb_Block)?pcb_Block->getHeadDataInsertPosition():NULL;pcb_dataPos;)
            {        
               // Inserts of PCB are not mirrored when Panel is mirrored 
               DataStruct* insertdata = pcb_Block->getNextDataInsert(pcb_dataPos);
               if (insertdata != NULL && (insertdata->getInsert()->getGraphicMirrored() == isBottom))
               {
                  if (insertdata->getInsert()->getInsertType() == insertTypeFiducial)
                  {      	
                     CTMatrix l_boardMatrix(l_panelMatrix);
                     TranslateBoard(l_panelMatrix, l_boardMatrix, data);         

                     CString refname = insertdata->getInsert()->getRefname();
                     refname = RenameFiducial(refname, pcbData->getFiducialDataList().GetCount(), true);
                     bool isMirror = insertdata->getInsert()->getGraphicMirrored();

                     double rot = getRotationDegrees(insertdata->getInsert()->getAngle(),isMirror) + data->getInsert()->getAngleDegrees();
                     CPoint2d fidCord(insertdata->getInsert()->getOriginX(), insertdata->getInsert()->getOriginY());

                     l_boardMatrix.mirrorAboutYAxis(isMirror);
                     l_boardMatrix.transform(fidCord);

                     pcbData->getFiducialDataList().Add(insertdata, ConvertToExporterUnits(fidCord.x) * (isMirror?-1:1),
                        ConvertToExporterUnits(fidCord.y), rot, l_pcbName + "_" + refname, getPartNumber(data), blockTypePcb, isBottom);                  

                  }
               }//insertdata != NULL

            }//for

         }//if
      }// if data
   }//while
}

void BaseAoiExporter::TraversePanelSakiStyle(CTMatrix &l_panelMatrix, BlockStruct* block, bool isBottom)
{  
   CString partNumber = "";
   CString refName = "";

   CString panelName = m_PCBDataList.getBoardName();
   int intFidID = 0;

   POSITION dataPos = block->getHeadDataInsertPosition();
   while (dataPos)
   {     
      DataStruct* data = block->getNextDataInsert(dataPos);

      if (data != NULL)
      {
         if (data->isInsertType(insertTypeFiducial))
         {
            BaseAoiExporterPCBData *panelData = NULL;
            if(m_PCBDataList.Lookup(panelName, panelData) && panelData)
            {
               bool isMirror = data->getInsert()->getGraphicMirrored();

               // For bottom side the whole panel has already been mirrored. So top fids are on
               // top for isBottom==false, and top fids are mirrored for isBottom=true;
               // The result is we always want only to take fids that are not mirrored.
               // And we want to tag them with the isBottom flag, not isMirrored.

               if (!isMirror)
               {
                  double rot = getRotationDegrees(data->getInsert()->getAngle(),isMirror);

                  double xStr = ConvertToExporterUnits(data->getInsert()->getOriginX());
                  double yStr = ConvertToExporterUnits(data->getInsert()->getOriginY());

                  if (isBottom && GetMirrorBottomX())
                     xStr = -xStr;

                  refName = data->getInsert()->getRefname();
                  refName = RenameFiducial(refName, ++intFidID, true);

                  partNumber = getPartNumber(data); 	
                  
                  panelData->getFiducialDataList().Add(data,xStr,yStr,rot,refName,partNumber,blockTypePanel,isBottom);      
               }
            }
         }      
         else if (data->isInsertType(insertTypePcb))
         {		               
            BaseAoiExporterPCBData *pcbData = NULL;
            CPoint2d compCentroid; 
            CString l_pcbName = "";

            BlockStruct* pcb_Block = m_pDoc.getBlockAt(data->getInsert()->getBlockNumber());         
            if (pcb_Block)
            {         
               l_pcbName = data->getInsert()->getRefname();
               bool isMirror = data->getInsert()->getGraphicMirrored();

               if (isMirror) //*rcf debug
               {
                  int jj = 0;
               }

               double xStr = ConvertToExporterUnits(data->getInsert()->getOriginX());
               double yStr = ConvertToExporterUnits(data->getInsert()->getOriginY());

               if (isBottom && GetMirrorBottomX())
                  xStr = -xStr;

               double rot = getRotationDegrees(data->getInsert()->getAngle(),isMirror);

               pcbData = new BaseAoiExporterPCBData( m_pDoc, xStr, yStr,  blockTypePcb, data, pcb_Block);
               //*rcf ORIGINAL m_PCBDataList.SetAt(l_pcbName,pcbData);
               m_PCBDataList.SetBoardAt(l_pcbName, pcbData);
            }

            for (POSITION insertdataPos = (pcb_Block)?pcb_Block->getHeadDataInsertPosition():NULL;insertdataPos;)
            {            
               CTMatrix l_boardMatrix(l_panelMatrix);
               TranslateBoard(l_panelMatrix, l_boardMatrix, data);   // This prepares l_boardMatrix                   

               DataStruct* insertdata = pcb_Block->getNextDataInsert(insertdataPos);

               //*rcf debug
               double insertX = insertdata->getInsert()->getOriginX();
               double insertY = insertdata->getInsert()->getOriginY();
               if (insertdata->getInsert()->getRefname().CompareNoCase("R9901") == 0)
               {
                  int jj = 0;
               }

               bool flatten = this->Flatten();

               bool insertIsComponent = (insertdata != NULL) ? insertdata->isInsertType(insertTypePcbComponent) : false;
               bool insertIsFiducial  = (insertdata != NULL) ? insertdata->isInsertType(insertTypeFiducial)     : false;

               bool exportThisComponent = false;
               if (insertIsComponent)
                  exportThisComponent = (isLoaded(insertdata) || GetSettings()->getExportUNLoaded()) && isWritePart(insertdata);

               if ((insertIsComponent && exportThisComponent) || insertIsFiducial)
               {
                 
                  if (insertIsFiducial)
                     refName = RenameFiducial(insertdata->getInsert()->getRefname(), ++intFidID, false);
                  else
                     refName  = insertdata->getInsert()->getRefname();

                  bool isMirror = insertdata->getInsert()->getGraphicMirrored();

                  CPoint2d testVect(1., 1.);
                  l_boardMatrix.transform(testVect);

                  // Default location is insert location.
                  CPoint2d compInsertXY = insertdata->getInsert()->getOrigin2d();
                  if (flatten)
                     l_boardMatrix.transform(compInsertXY);

                  double xStr = ConvertToExporterUnits(compInsertXY.x);
                  double yStr = ConvertToExporterUnits(compInsertXY.y);

                  // If using centroids and centroid exists then override insertXY with centroid XY.
                  if (insertIsComponent && GetUseComponentCentroids())
                  {
                     if (insertdata->getInsert()->getCentroidLocation(m_pDoc.getCamCadData(), compCentroid))
                     {
                        if (flatten)
                           l_boardMatrix.transform(compCentroid);
                        xStr = ConvertToExporterUnits(compCentroid.x);
                        yStr = ConvertToExporterUnits(compCentroid.y);
                     }
                  }

                  if (isBottom && GetMirrorBottomX())
                     xStr = -xStr;

                  double rot = getRotationDegrees(insertdata->getInsert()->getAngle() + data->getInsert()->getAngle(),isMirror) ;
                  partNumber = insertIsComponent ? getPartNumber(insertdata) : ""; 

                  if (insertIsComponent)
                     pcbData->getComponentDataList().Add(insertdata,xStr,yStr,rot,refName,partNumber,blockTypePcb,isMirror);
                  else if (insertIsFiducial)
                     pcbData->getFiducialDataList().Add(insertdata,xStr,yStr,rot,refName,partNumber,blockTypePcb,isMirror);

               }              
            }
         }//if
      } // if data != NULL
   }   

}

CString BaseAoiExporter::RenameFiducial(CString fidname, int intFidID, bool isPanelFid)
{
   if (fidname.IsEmpty())
      fidname.Format("%sFID%d", isPanelFid ? "P" : "B", intFidID);
   
   return fidname;
}

bool BaseAoiExporter::TraversePCBsInPanel(BlockStruct *targetBlock)
{
   if(!targetBlock) return FALSE;

   m_exportProgress.updateStatus("Processing Stencil PCB Board.");
   m_exportProgress.setLength(targetBlock->getDataCount());

   //reset PCBData list for Stencil boards
   m_PCBDataList.empty();
   m_PCBDataList.setPanelFlag(true);
   m_PCBDataList.setBoardName(targetBlock->getName());

   //traverse all PCB boards in the panel
   CTypedMapIntToPtrContainer<BlockStruct*> stencilBlockMap;
   for (POSITION dataPos = targetBlock->getHeadDataInsertPosition();dataPos;)
   {
      m_exportProgress.incrementProgress();

      DataStruct* data = targetBlock->getNextDataInsert(dataPos);
      if (data && data->getInsert() && data->getInsert()->getInsertType() == insertTypePcb)
      {
         FileStruct * fileStruct = m_pDoc.getFileList().FindByBlockNumber(data->getInsert()->getBlockNumber());
         BlockStruct *fileBolck = (fileStruct)?fileStruct->getBlock():NULL;
         CString l_pcbName = data->getInsert()->getRefname();

         BlockStruct *block = NULL;
         bool flatten = false;
         if(fileBolck && (flatten || !stencilBlockMap.Lookup(fileBolck->getBlockNumber(),block)))
         {
            stencilBlockMap.SetAt(fileBolck->getBlockNumber(),fileBolck);
            double filestructX = fileStruct->getInsertX();
            double filestructY = fileStruct->getInsertY();//*rcf debug
            BaseAoiExporterPCBData *pcbData = new BaseAoiExporterPCBData(m_pDoc, ConvertToExporterUnits(fileStruct->getInsertX()), ConvertToExporterUnits(fileStruct->getInsertY()), 
               blockTypePcb,data,fileBolck);
            
            ApplyPrepSettingToFiles(fileStruct, pcbData, false, false);
            ApplyPrepSettingToFiles(fileStruct, pcbData, true, false);          

            m_PCBDataList.SetBoardAt(l_pcbName,pcbData);
         }
      }
   }//for

   stencilBlockMap.RemoveAll();
   return TRUE;
}

bool BaseAoiExporter::VerifyFiducials(BlockTypeTag blockType)
{
   // If min count is zero then verification is turned off.
   if (GetMinFiducialCount() <= 0)
      return true; // Verification is off, return success result.

   CTypedMapIntToPtrContainer<BlockStruct*> stencilBlockMap;
   bool reval = TRUE;

   for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
   {
      CString pcbName;
      BaseAoiExporterPCBData *pcbData =NULL;
      m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
      
      BlockStruct *block = NULL;
      if(pcbData && pcbData->getGeometryBlock() && pcbData->getBoardType() == blockType &&
         !stencilBlockMap.Lookup(pcbData->getGeometryBlock()->getBlockNumber(), block))
      {
         block = pcbData->getGeometryBlock();
         int blockNumber = block->getBlockNumber();
         stencilBlockMap.SetAt(blockNumber,block);

         for(int isBottom = 0; isBottom < 2; isBottom++)
         {
            int fidCnt = (pcbData)?pcbData->getFiducialDataList().getInsertsCountBySide(isBottom):0;
            int cmpCnt = (pcbData->getBoardType() == blockTypePcb)?(pcbData?pcbData->getComponentDataList().getInsertsCountBySide(isBottom):0):1;

            if(fidCnt < GetMinFiducialCount() && cmpCnt)
            {
               CString boardName;
               boardName.Format("%s of %s", isBottom?"Bottom":"Top", (pcbData->getBoardType() == blockTypePanel)?"Panel":"PCB "+ block->getName());
               
               if(formatMessageBox(MB_ICONQUESTION | MB_YESNO,
                  "Found %d Fiducials on %s when %d or %d are required. Do you want to continue ?",fidCnt,boardName
                  , GetMinFiducialCount(), GetMaxFiducialCount()) == IDNO)
               {
                  reval = FALSE;
                  break;
               }
            }      
         }//for
      }
   }

   stencilBlockMap.RemoveAll();
   return reval;
}

void BaseAoiExporter::TranslateFile(CTMatrix &l_panelMatrix, FileStruct *fileStruct)
{   
   l_panelMatrix.scale(fileStruct->getScale() * (fileStruct->getResultantMirror(False)?-1:1), fileStruct->getScale());

   l_panelMatrix.rotateRadians(fileStruct->getRotation());

   l_panelMatrix.translate(fileStruct->getInsertX(), fileStruct->getInsertY());
}

void BaseAoiExporter::TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data)
{
   CPoint2d boardXY = data->getInsert()->getOrigin2d();
   l_panelMatrix.transform(boardXY);

   l_boardMatrix.scale(data->getInsert()->getScale() * (data->getInsert()->getGraphicMirrored()?-1:1), data->getInsert()->getScale());
   
   l_boardMatrix.rotateRadians(data->getInsert()->getAngle());

   l_boardMatrix.translate(boardXY);
}

CString BaseAoiExporter::GetConfigurableValue(BaseAoiExporterInsertData *cmpData, CString dataSourceName)
{
   CString value("NotSet");

   if (cmpData != NULL)
   {
      if (dataSourceName.CompareNoCase(QGEOM_NAME) == 0)
      {
         // Use inserted geometry's name
         value = cmpData->getInsertedBlockName();
      }
      else if (cmpData->getInsertData() != NULL)
      {
         // macroKeyword is assumed to be an attribute, look it up and return value
         Attrib *attrib = NULL;
         attrib = is_attvalue(&m_pDoc, cmpData->getInsertData()->getAttributesRef(), dataSourceName, 0);
         value = attrib ? attrib->getStringValue() : "NotSet";
      }
   }

   return value;
}



bool BaseAoiExporter::isLoaded(DataStruct* data)
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

bool BaseAoiExporter::isWritePart(DataStruct* data)
{
   bool writePart = true;
   //Check for optional filter command from OUT file settings
   if(GetSettings()->GetOutputAttribMap().IsValid()) // if using OUT file command
   {
      //Check filter, part must have a value that is present in set of filter values
      writePart = (data->getAttributesRef() != NULL && GetSettings()->GetOutputAttribMap().HasValue(&data->getAttributesRef()));
   }
   return writePart;
}

double BaseAoiExporter::getRotationDegrees(double radians, int mirror)
{
   double rot = 0.;
   rot = round(normalizeDegrees(RadToDeg(radians))); 

   if (mirror && GetMirrorBottomRotation())
      rot = 360 - rot;

   return normalizeDegrees(rot); 
}

CString BaseAoiExporter::getPartNumber(DataStruct* data)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_pDoc, data->getAttributeMap(), GetSettings()->getPartNumberKW(), 0);
   CString partNumber = attrib?attrib->getStringValue():"No_Part_Number";

   if ((data->getInsert()->getInsertType() == insertTypeFiducial) && partNumber == "No_Part_Number")
      partNumber = "NC";

   return partNumber;
}

ApertureShapeTag BaseAoiExporter::getFiducialShape(DataStruct* data)
{
   ApertureShapeTag shapeTag = apertureUndefined;

  if(data && data->getInsert()) 
   {
      BlockStruct *apertureblock = m_pDoc.getBlockAt(data->getInsert()->getBlockNumber());
      if(apertureblock)
      {
         if ( apertureblock->getShape() != apertureUndefined )
            return apertureblock->getShape();
         else
         {
            for(POSITION aperturePos = apertureblock->getDataList().GetHeadPosition();aperturePos;)
            {
               DataStruct *apertureData = apertureblock->getDataList().GetNext(aperturePos);
               if(apertureData && apertureData->getInsert())
               {
                  shapeTag = getFiducialShape(apertureData);
                  if(shapeTag != apertureUndefined)
                     break;
               }
            }//for
         }
      }//if
   }//if

   return shapeTag;
}

//*rcf Looks like this should be MyData specific, not in base
CString BaseAoiExporter::getUnitString(PageUnitsTag Pageunits)
{
   CString unitFactor = "";
   if(isMetric(Pageunits)) // base unit is MicroMeters   
   {      
      double factor = getUnitsFactor(Pageunits,pageUnitsMicroMeters);
      if(Pageunits != pageUnitsMicroMeters)
         unitFactor.Format("%.2f",factor);
   }
   else if(isEnglish(Pageunits)) //base unit is MILS
   {
      unitFactor = "I"; 
      double factor = getUnitsFactor(Pageunits,pageUnitsMils);
      if(Pageunits != pageUnitsMils)
         unitFactor.Format("%.2fI",factor);
   }   

   return unitFactor;
}

CString BaseAoiExporter::GetUniqueName(CString refname)
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


CExtent BaseAoiExporter::GetOutlineExtent(FileStruct *activeFile)
{
   Mat2x2 m;
   RotMat2(&m, 0);

	ExtentRect fileExtents;

   fileExtents.left = fileExtents.bottom = FLT_MAX;
   fileExtents.right = fileExtents.top = -FLT_MAX;

   bool OutlineFound = false;

   if (activeFile != NULL)
   {
      POSITION dataPos = activeFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = activeFile->getBlock()->getDataList().GetNext(dataPos);

         if ((data->getDataType() == T_POLY) &&
            ((activeFile->getBlockType() == BLOCKTYPE_PANEL && data->getGraphicClass() == GR_CLASS_PANELOUTLINE) ||
            (activeFile->getBlockType() == BLOCKTYPE_PCB && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)))
         {
            OutlineFound = true;

            ExtentRect polyExtents;
            if (PolyExtents(&m_pDoc, data->getPolyList(), &polyExtents, 1, 0, 0, 0, &m, FALSE))
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

   if (!OutlineFound && activeFile != NULL)
   {
      activeFile->getBlock()->calculateVisibleBlockExtents(m_pDoc.getCamCadData());		
      extent = activeFile->getBlock()->getExtent();
   }

   return extent;
}

/******************************************************************************
*/

BaseAoiExporterOutputAttrib::BaseAoiExporterOutputAttrib(CCEtoODBDoc& camCadDoc) 
:  m_camCadDatabase(camCadDoc)
,  m_attribKeyword(-1)
{
	SetDefaultAttribAndValue();
}

BaseAoiExporterOutputAttrib::~BaseAoiExporterOutputAttrib()
{
	m_valuesMap.RemoveAll();
}

int BaseAoiExporterOutputAttrib::GetKeyword() const
{
	return m_attribKeyword;
}

bool BaseAoiExporterOutputAttrib::HasValue(CAttributes** attributes)
{
	CString value, tmpValue;
	m_camCadDatabase.getAttributeStringValue(value, attributes, m_attribKeyword);
	value.MakeLower();
	return (m_valuesMap.Lookup(value, tmpValue)==TRUE)?true:false;
}

void BaseAoiExporterOutputAttrib::SetKeyword(const CString keyword)
{
	m_attribKeyword = m_camCadDatabase.getKeywordIndex(keyword);
	m_valuesMap.RemoveAll();
}

void BaseAoiExporterOutputAttrib::AddValue(const CString value)
{
	CString tmpValue = value;
	m_valuesMap.SetAt(tmpValue.MakeLower(), tmpValue.MakeLower());
}

void BaseAoiExporterOutputAttrib::SetDefaultAttribAndValue()
{
   // No default?
   m_attribKeyword = -1;
   m_valuesMap.RemoveAll();
}



////////////////////////////////////////////////////////////
// BaseAoiExporterInsertData
////////////////////////////////////////////////////////////
BaseAoiExporterInsertData::BaseAoiExporterInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data)
: m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
, m_InsertData(data)
, m_insertedBlock(NULL)
{
   m_posX = 0.;
   m_posY = 0.;
   m_rotation = 0.;
   m_PartNumber.Empty();
   m_BoardType = blockTypeUndefined;
}

void BaseAoiExporterInsertData::setRotation(double rotation)
{
   m_rotation = /*rcf DEGREESCALE * */ rotation;
}

////////////////////////////////////////////////////////////
// BaseAoiExporterInsertDataList
////////////////////////////////////////////////////////////
BaseAoiExporterInsertDataList::BaseAoiExporterInsertDataList(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_topInserts(0)
, m_botInserts(0)
//*rcf , m_compAnalyzer(camCadDoc)
{
}

BaseAoiExporterInsertDataList::~BaseAoiExporterInsertDataList()
{
   m_topInserts = 0;
   m_botInserts = 0;
   empty();
}

void BaseAoiExporterInsertDataList::Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, 
                                CString partNumber, BlockTypeTag boardType, bool placedBottom)
{
   BaseAoiExporterInsertData *data = new BaseAoiExporterInsertData(m_camCadDoc, InsertData);
   data->setPartNumber(partNumber);
   data->setRefName(refName);
   data->setX(posX);
   data->setY(posY);
   data->setBoardType(boardType);
   data->setPlacedBottom(placedBottom);
   data->setRotation(rotation);

   // Not sure this if test is really needed, but it was here already, so leaving it jic.
   if(InsertData && InsertData->getInsert())
   {
      BlockStruct *geometryBlock = m_camCadDoc.getBlockAt(InsertData->getInsert()->getBlockNumber());
      data->setInsertedBlock(geometryBlock);

      if(placedBottom)
         m_botInserts ++;
      else
         m_topInserts ++;
   }
   else
   {
      int jj = 0; //*rcf Debug, just want to see if we ever get here.
   }
   

   SetAtGrow(GetCount(), data);
}

////////////////////////////////////////////////////////////
// BaseAoiExporterPCBData
////////////////////////////////////////////////////////////
BaseAoiExporterPCBData::BaseAoiExporterPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry)
: m_originX(x)
, m_originY(y)
, m_boardType(boardType)
, m_FidDataList(camCadDoc)
, m_CompDataList(camCadDoc)
, m_XoutDataList(camCadDoc)
{
   m_FidDataList.RemoveAll();
   m_CompDataList.RemoveAll();
   
   m_insertData = data;
   m_geometryBlock = geometry;
}

BaseAoiExporterPCBData::~BaseAoiExporterPCBData()
{
   m_FidDataList.empty();
   m_CompDataList.empty();
}

////////////////////////////////////////////////////////////
// BaseAoiExporterPCBDataList
////////////////////////////////////////////////////////////
BaseAoiExporterPCBDataList::BaseAoiExporterPCBDataList()
{
   m_Panelflag = false;
   m_BoardName = QDEFAULT_BOARDNAME;
}

BaseAoiExporterPCBDataList::~BaseAoiExporterPCBDataList()
{
   empty();
}

void BaseAoiExporterPCBDataList::empty()
{
   m_Panelflag = false;
   m_BoardName = QDEFAULT_BOARDNAME;

   for(POSITION pcbPos = GetStartPosition(); pcbPos; )
   {
      CString pcbName;
      BaseAoiExporterPCBData *pcbData =NULL;
      GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData)
         delete pcbData;
   }
   RemoveAll();
}

void BaseAoiExporterPCBDataList::SetBoardAt(CString boardName, BaseAoiExporterPCBData *pcbdata)
{
   BaseAoiExporterPCBData *existdata = NULL;
   
   if(boardName.IsEmpty()) 
   {
      boardName.Format("%s_$%d", QDEFAULT_BOARDNAME, GetCount());
   }
   else if(Lookup(boardName,existdata))
   {
      boardName.Format("%s_$%d", boardName, GetCount());
   }

   SetAt(boardName,pcbdata);
}



//=======================================================
