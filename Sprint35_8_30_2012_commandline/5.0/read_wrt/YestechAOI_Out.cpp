
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
#include "YestechAOI_Out.h"

/*****************************************************************************/

void YestechAOI_WriteFiles(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
      NewYestechAOIWriter writer(doc, format->Scale);

      if (writer.DetermineActiveCCFile(&doc) != NULL)
      {
         if(!writer.WriteFiles(filename))
         {
            ErrorMessage("CAMCAD could not finish writing the Yestech AOI output files.","", MB_OK);
         }
      }
}


//==========================================================================


NewYestechAOIWriter::NewYestechAOIWriter(CCEtoODBDoc &doc, double l_Scale)
: BaseAoiExporter(doc, l_Scale, fileTypeYestechAOI, new YestechAOISettings(doc), "YestechAOI.log")
, m_activeFile(NULL)
, m_compRecordNum(1)
//*rcf , m_compAnalyzer(doc)
{
   SetUseComponentCentroids(true);  // For component locations, instead of insert location.

   SetMirrorBottomX(false);
   SetMirrorBottomRotation(false);

   // For fiducial verification -- turn off.
   SetMinFiducialCount(0);
   SetMaxFiducialCount(0);

   // Cache this KW so we don't have to keep looking it up during output
   m_deviceTypeKW = doc.getCamCadData().getAttributeKeywordIndex(standardAttributeDeviceType);
}

NewYestechAOIWriter::~NewYestechAOIWriter()
{
}

int NewYestechAOIWriter::GetDecimals(PageUnitsTag units)

{
   // The decimal can be adjusted based on 
   // the accurancy of the calculation in various unit
   // It will affect the result of YestechAOI file
   int decimals = 4; // default. 
   
   switch (units)
   {
   case pageUnitsInches:         decimals = 3; break;
   case pageUnitsMilliMeters:    decimals = 3; break;
   case pageUnitsMils:           decimals = 1; break;
   case pageUnitsMicroMeters:    decimals = 1; break;
   case pageUnitsHpPlotter:
   case pageUnitsCentiMicroMeters:
   case pageUnitsNanoMeters:
      break;
   }
   return decimals;
}

bool NewYestechAOIWriter::WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName)
{
   if(!pcbData) return FALSE;

   GetProgressBar().updateStatus("Writing PCB Files.");

   bool stat1 = WritePCBFile(pcbData, boardName, false); // top
   bool stat2 = WritePCBFile(pcbData, boardName, true ); // bot

   return (stat1 && stat2);
}

bool NewYestechAOIWriter::WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom)
{   
   if(!pcbData) return FALSE;

   //*rcf CString fileName( this->getOutputFilename(isBottom) );
   //*rcf Can change filename getter such that side suffix and extension are
   // set in constructor, and getter has only isBottom type param, or separate
   // top and bot functions. No need to reference m_FileName from here, it is
   // in the base.
   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName = CreateOutfileName(m_FileName, sideSuffix, ".ycd"); //*rcf bug, fix to correct extension.

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

void  NewYestechAOIWriter::WriteHeader(CFormatStdioFile &File, bool isBottom)
{
   CTime dateTime = CTime::GetCurrentTime();
   File.WriteString("#------------------------------------------------------------------\n");
   File.WriteString("# YESTECH AOI FILE\n");
   File.WriteString("# Created by %s\n", getApp().getCamCadTitle());  // this title includes version number
   File.WriteString("# Created %s\n", dateTime.Format("%A, %B %d, %Y at %H:%M:%S"));
   File.WriteString("#------------------------------------------------------------------\n");
   File.WriteString("\n");
   
   double boardSizeX = 0.;
   double boardSizeY = 0.;
   double boardOriginX = 0.;
   double boardOriginY = 0.;
   FileStruct *fileStruct = this->getActiveFile();
   CExtent boardExtent = this->GetOutlineExtent(fileStruct);
   if (boardExtent.isValid())
   {
      boardSizeX = boardExtent.getXsize();
      boardSizeY = boardExtent.getYsize();

      //this->setOutlineLowerLeft(getYestechAOIUnits(boardExtent.getXmin()), getYestechAOIUnits(boardExtent.getYmin()));
   }


   File.WriteString(".YESTECH_CAD\n");
   File.WriteString("[Header]\n");
   File.WriteString("Version=           1\n");
   File.WriteString("RecipeName=        %s\n", this->m_PCBDataList.getBoardName());
   File.WriteString("ScaleFactor=       %s\n", getYestechOutputUnitsValue(this->GetSettings()->getOutputPageUnits()));
   File.WriteString("InvertXPos=        0\n");
   File.WriteString("InvertYPos=        0\n");
   File.WriteString("RotationDir=       1\n");   // From Tom:  Also, regarding rotation:  CCW = 1, CW = -1
   File.WriteString("RotationOffset=    0\n");
   File.WriteString("BoardWidth=        0\n");
   File.WriteString("BoardHeight=       0\n");
   File.WriteString("BoardRotation=     0\n");
   File.WriteString("PNLib=             %s\n", this->GetSettings()->getPnLibName());
   File.WriteString("CentralLib=        %s\n", this->GetSettings()->getCentralLibName());
   File.WriteString("IsSharedPNLib=     0\n");
   File.WriteString("PkgLib=            %s\n", this->GetSettings()->getPkgLibName());
   File.WriteString("PkgNameMap=        %s\n", this->GetSettings()->getPkgNameMapName());
   File.WriteString("IsTopSide=         %d\n", (isBottom)?0:1);
   File.WriteString("\n");
}


void NewYestechAOIWriter::WriteComponentList(CFormatStdioFile &File, BaseAoiExporterPCBData *pcbData, bool isBottom)
{
   File.WriteString("[PartList]\n");
   WriteComponentRecord(File, "RefID.", "P/N", "XPos", "YPos", "Rtn", "Pkg", "Extension");

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

   File.WriteString("[PartListEnd]\n");
}


void NewYestechAOIWriter::WriteComponents(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, bool isBottom)
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

void  NewYestechAOIWriter::WriteComponentRecord(CFormatStdioFile &File, int boardIdx, BaseAoiExporterInsertData *YestechCmpData, bool isBottom)
{
   if (YestechCmpData != NULL)
   {
      CString instanceRefname( GetUniqueName(YestechCmpData->getRefName()) );

      CString xStr,yStr,rotStr;
      xStr.Format("%.*lf", m_unit_accuracy, YestechCmpData->getX());
      yStr.Format("%.*lf", m_unit_accuracy, YestechCmpData->getY());
      rotStr.Format("%.*lf", 1, YestechCmpData->getRotation());

      CString partnumber( YestechCmpData->getPartNumber() );
      CString extension( this->getDescription(YestechCmpData->getInsertData()) );

      // Package Name - Changes per Mark 24 April 2012, no DR.
      // Previously export always used the geometry name for package name, now has option to give and attribute name
      // by way of .PACKAGE_NAME command in .out file.
      CString packagename;
      CString pkgNameAttribName( this->GetSettings()->getPkgNameAttrib() );
      // If attrib name setting is not empty and not GEOMETRY then lookup the attrib.
      if (!pkgNameAttribName.IsEmpty() && pkgNameAttribName.CompareNoCase("GEOMETRY") != 0)
      {
         packagename = this->getGenericAttributeValue(YestechCmpData->getInsertData(), pkgNameAttribName);
         packagename.Trim(); // Clean it up.
         // If it is still empty then log message that attrib is not present on this part.
         if (packagename.IsEmpty())
         {
            this->m_textLogFile.writef(".PACKAGE_NAME attribute [%s] not set on component [%s], defaulting to geometry name [%s] for package name.\n",
               pkgNameAttribName, instanceRefname, YestechCmpData->getInsertedBlockName());
         }
      }

      // If attribute lookup didn't get us a package name then use geometry name.
      if (packagename.IsEmpty())
      {
         packagename = YestechCmpData->getInsertedBlockName();
      }
      
      // Record order to match header:
      // RefID.  P/N  XPos  YPos  Rtn  Pkg  Extension

      WriteComponentRecord(File,
         instanceRefname,
         partnumber,
         xStr,
         yStr,
         rotStr,
         packagename,
         extension
         );
    }
}

CString NewYestechAOIWriter::getDescription(DataStruct* data)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_pDoc, data->getAttributeMap(), ATT_PART_DESCRIPTION, 0);
   CString strval( attrib?attrib->getStringValue():"" );

   // If description attrib not set then build description from
   // device type and value.
   if (strval.IsEmpty())
   {
      CString devtype( getDeviceType(data) );
      CString value( getValue(data) );

      if (!devtype.IsEmpty())
      {
         strval = devtype;
         if (!value.IsEmpty())
            strval += " " + value;
      }
      else
      {
         // WI 20073 says spaces in fields cause problems, so "No DESCRIPTION attribute" as was here
         // before is not a good choice. The WI suggests four dashes as the common way to fill this field.
         // The example YCD files provided for implementation pattern had spaces in this field, and was
         // made up of device type and value and some other stuff, that is why we do the device type thing
         // above. So spaces in this field probably are not really a problem.
         strval = "----";
      }
   }

   return strval;
}

CString NewYestechAOIWriter::getDeviceType(DataStruct* data)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_pDoc, data->getAttributeMap(), ATT_DEVICETYPE, 0);
   CString strval( attrib?attrib->getStringValue():"" );

   return strval;
}

CString  NewYestechAOIWriter::getGenericAttributeValue(DataStruct* data, CString attributeName)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_pDoc, data->getAttributeMap(), attributeName, 0);
   CString strval( attrib?attrib->getStringValue():"" );

   return strval;
}

CString NewYestechAOIWriter::getValue(DataStruct* data)
{
   Attrib *attrib = NULL;
   attrib = is_attvalue(&m_pDoc, data->getAttributeMap(), ATT_VALUE, 0);
   CString strval( attrib?attrib->getStringValue():"" );

   return strval;
}

void  NewYestechAOIWriter::WriteComponentRecord(CFormatStdioFile &File, CString f1, CString f2, CString f3, CString f4, CString f5, CString f6, CString f7)
{
   // Used to write column headings and component data.
   // This controls size and justification of fields.
   // If we are using space delimiter then pad fields to those certain sizes.
   // If using tab delimiter then do not pad the fields, use only the tab.

   // Get column delimiter, might be tab or space.
   CString delimiter( this->GetSettings()->getFieldDelimiter() );

   // RefID P/N  XPos  YPos  Rtn  Pkg  Extension

   if (this->GetSettings()->getUseTabDelimiter())
   {
      File.WriteString("%s%s",  f1, delimiter);  // Field 1, RefId, left justified
      File.WriteString("%s%s",  f2, delimiter);  // Field 2, P/N,   left justified
      File.WriteString("%s%s",  f3, delimiter);  // Field 3, XPos,  right justified
      File.WriteString("%s%s",  f4, delimiter);  // Field 4, YPos,  right justified
      File.WriteString("%s%s",  f5, delimiter);  // Field 5, Rtn,   right justified
      File.WriteString("%s%s",  f6, delimiter);  // Field 6, Pkg,   left justified
      File.WriteString("%s\n",  f7);             // Field 7, Extension, left justified
   }
   else
   {
      File.WriteString("%-12s%s", f1, delimiter);  // Field 1, RefId, left justified
      File.WriteString("%-22s%s", f2, delimiter);  // Field 2, P/N,   left justified
      File.WriteString("%11s%s",  f3, delimiter);  // Field 3, XPos,  right justified
      File.WriteString("%11s%s",  f4, delimiter);  // Field 4, YPos,  right justified
      File.WriteString("%10s%s",  f5, delimiter);  // Field 5, Rtn,   right justified
      File.WriteString("%-35s%s", f6, delimiter);  // Field 6, Pkg,   left justified
      File.WriteString("%-s\n",   f7);             // Field 7, Extension, left justified
   }
}


void NewYestechAOIWriter::WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom)
{
   // This function kept in place because it works, and we might want it back.
   // Current info does not indicate any fid support in Yestech, though surely there is some.
   // So until we find out otherwise, just do nothing in here.
#ifdef FID_OUTPUT_SUPPORT
   int fidCnt = (pcbData)?pcbData->getFiducialDataList().GetCount():0;

   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex++)
   {
      BaseAoiExporterInsertData *fiddata = pcbData->getFiducialDataList().GetAt(fidIndex);
      if(fiddata && fiddata->getInsertData() && fiddata->getBoardType() == boardType && fiddata->getPlacedBottom() == isBottom)
      {         
         WriteComponentRecord(File, boardIdx, fiddata, isBottom);
      }
   } 
#endif
}

//*rcf This activeFile support needs moving to base class
FileStruct *NewYestechAOIWriter::DetermineActiveCCFile(CCEtoODBDoc *doc)
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

/*deadcode
CString NewYestechAOIWriter::getMacroValue(BaseAoiExporterInsertData *sakiCmpData)
{
   CString macroKeyword = GetSettings()->getMacroKeyword();
   return GetConfigurableValue(sakiCmpData, macroKeyword);
}
   
CString NewYestechAOIWriter::getLibValue(BaseAoiExporterInsertData *sakiCmpData)
{
   CString libKeyword = GetSettings()->getLibKeyword();
   return GetConfigurableValue(sakiCmpData, libKeyword);
}

int NewYestechAOIWriter::GetFlg(BaseAoiExporterInsertData *sakiCmpData)
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
*/

CString NewYestechAOIWriter::RenameFiducial(CString fidname, int intFidID, bool isPanelFid)
{
   if (fidname.IsEmpty())
      fidname.Format("%sFID%d", isPanelFid ? "P" : "B", intFidID);

   return fidname;
}

CString NewYestechAOIWriter::getYestechOutputUnitsValue(PageUnitsTag outputUnits)
{
   // From Tom: 1 for um, 25.4 for mils, 1000 for mm and 25400 for inches
   switch (outputUnits)
   {
   case pageUnitsMicroMeters:
      return "1";
   case pageUnitsMilliMeters:
      return "1000";
   case pageUnitsMils:
      return "25.4";
   case pageUnitsInches:
      return "25400";
   }

   return "XXX";
}

bool NewYestechAOIWriter::WritePanelFiles(BaseAoiExporterPCBData *panelData)
{
   GetProgressBar().updateStatus("Writing Panel Files.");

   BlockStruct *panelBlock = panelData->getGeometryBlock();

   //Verify Fiducials for PCB
   if(!VerifyFiducials(blockTypePcb))
      return false;

   bool stat1 = WritePanelFile(panelData, false); // top
   bool stat2 = WritePanelFile(panelData, true ); // bot

   return stat1 && stat2;
}

bool NewYestechAOIWriter::WritePanelFile(BaseAoiExporterPCBData *pcbData, bool isBottom)
{  
   if(!m_PCBDataList.IsPanel())
      return FALSE;

   //deadcode CString fileName( this->getOutputFilename(isBottom) );
   CString sideSuffix( isBottom?QSUFFIX_BOT:QSUFFIX_TOP );
   CString fileName = CreateOutfileName(m_FileName, sideSuffix, ".ycd"); //*rcf Fix up so extension is set in constructor

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
YestechAOISettings::YestechAOISettings(CCEtoODBDoc& doc)
: BaseAoiExporterSettings("YestechAOI.out", doc)
{
   SetLocalDefaults();
}

YestechAOISettings::~YestechAOISettings()
{
}

void YestechAOISettings::SetLocalDefaults()
{
   // From base settings
   m_PartNumberKW = ATT_PARTNUMBER;
   m_outputPageUnits = pageUnitsMils;
   m_exportUNLoaded = false;

   // Local settings
   m_pnLibName.Empty();
   m_centralLibName.Empty();
   m_pkgLibName.Empty();
   m_pkgNameMapName.Empty();
   m_pkgNameAttrib.Empty();
   m_columnDelimiterTab = false;

}

bool YestechAOISettings::ParseLocalCommand(CInFile &inFile)
{
   bool isLocalCmd = true;

   if (inFile.isCommand(".PNLib",1))
   {
      if (inFile.getParamCount() > 1)
         this->m_pnLibName = inFile.getParam(1);
      else
         this->m_pnLibName.Empty();
   }
   else if (inFile.isCommand(".CentralLib",1))
   {
      if (inFile.getParamCount() > 1)
         this->m_centralLibName = inFile.getParam(1);
      else
         this->m_centralLibName.Empty();
   }
   else if (inFile.isCommand(".PkgLib",1))
   {
      if (inFile.getParamCount() > 1)
         this->m_pkgLibName = inFile.getParam(1);
      else
         this->m_pkgLibName.Empty();
   }
   else if (inFile.isCommand(".PkgNameMap",1))
   {
      if (inFile.getParamCount() > 1)
         this->m_pkgNameMapName = inFile.getParam(1);
      else
         this->m_pkgNameMapName.Empty();
   }
   else if (inFile.isCommand(".PACKAGE_NAME",1))
   {
      if (inFile.getParamCount() > 1)
         this->m_pkgNameAttrib = inFile.getParam(1);
      else
         this->m_pkgNameAttrib.Empty();
   }
   else if (inFile.isCommand(".COLUMN_DELIMITER",1))
   {
      this->m_columnDelimiterTab = false; // Default, use space delimiter.
      if (inFile.getParamCount() > 1)
      {
         CString val( inFile.getParam(1) );
         if (val.CompareNoCase("TAB") == 0)
            this->m_columnDelimiterTab = true;
      }
   }
   else
   {
      isLocalCmd = false;
   }

   return isLocalCmd;
}
