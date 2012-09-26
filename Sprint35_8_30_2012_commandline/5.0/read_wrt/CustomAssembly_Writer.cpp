

#include "StdAfx.h"
#include "ccdoc.h"  
#include "CCEtoODB.h"
#include "PcbUtil.h"
#include "CCEtoODB.h"
#include "gauge.h"
#include "RwUiLib.h"
#include "centroid.h"
#include "InFile.h"
#include "DirDlg.h"
#include "RwUiLib.h"
#include "PrepDlg.h"

#include "AssemblyTemplate.h"
#include "CustomAssembly_writer.h"

#define QCUSTOM_ASSEMBLY_WRITE "Custom Assembly Write"

void CustomAssembly_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   extern CProgressDlg *progress; // from PORT.CPP
   FileStruct *fileStruct = doc.getFileList().GetOnlyShown(blockTypePcb);
   if(!fileStruct) fileStruct = doc.getFileList().GetOnlyShown(blockTypePanel);

	if (!fileStruct)
	{
      ErrorMessage("Requires a single visible PCB or Panel file.", QCUSTOM_ASSEMBLY_WRITE); // show message box and return
		return;
	}

   if(progress) 
   {
      CString msg;
      msg.Format("Process %s ...", QCUSTOM_ASSEMBLY_WRITE);
      progress->SetStatus(msg);
   }

   CSelectMultiMachinesDlg mmdlg(filename);
   if(mmdlg.isMachinesExist())
   {
      if(mmdlg.DoModal() == IDOK)
      {
         for(int idx = 0 ; idx < mmdlg.getSelMachineCount(); idx++)
         {
            if(mmdlg.getSelMachineAt(idx))
            {
               CCustomAssemblyWriter writer(mmdlg.getDirectory(), doc, fileStruct, format, *mmdlg.getSelMachineAt(idx));
               writer.writeFile();
            }
         }
      }
   }
   else
   {
      ErrorMessage("Please define a machine in Tools | Line Configuration first.", "No machine is defined", MB_OK | MB_ICONHAND);      
   }
}

/////////////////////////////////////////////////////////////////////////////
// CSelectMultiMachinesDlg
/////////////////////////////////////////////////////////////////////////////
CSelectMultiMachinesDlg::CSelectMultiMachinesDlg(CString filename, CWnd* pParent /*=NULL*/)
   : CDialog(CSelectMultiMachinesDlg::IDD, pParent)
   , m_directory(filename)
{
   m_cdbmachineList = NULL;
}

CSelectMultiMachinesDlg::~CSelectMultiMachinesDlg()
{
   m_selmachineList.RemoveAll();
   delete m_cdbmachineList;
}

void CSelectMultiMachinesDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSelectMultiMachinesDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   DDX_Text(pDX, IDC_DIR, m_directory);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectMultiMachinesDlg, CDialog)
   //{{AFX_MSG_MAP(CSelectMultiMachinesDlg)
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CSelectMultiMachinesDlg::isMachinesExist()
{
   CDBInterface m_database;
   if(!m_database.IsConnected())
      m_database.Connect();
   
   m_cdbmachineList = m_database.LookupCustomMachines("");
   return (m_cdbmachineList && m_cdbmachineList->GetCount());
}

void CSelectMultiMachinesDlg::LoadMachines()
{
   if(m_cdbmachineList && m_cdbmachineList->GetCount())
   {
      m_machineCount = m_cdbmachineList->GetCount();
      for (int idx = 0; idx < m_machineCount; idx++)
      {
         CDBCustomMachine* cdbmachine = m_cdbmachineList->GetAt(idx);
         if(cdbmachine && !cdbmachine->getMachineName().IsEmpty())
         {
            CString machinetmpStr;
            machinetmpStr.Format("%s(%s)",cdbmachine->getMachineName(), cdbmachine->getTemplateName());

            m_list.AddString(machinetmpStr);
            m_list.SetItemData(idx,(DWORD_PTR)cdbmachine);
         }
      }//for
   }   
}

void CSelectMultiMachinesDlg::getSelMachineDataAt(int idx, CString &MachineName, CString &TemplateName)
{
   MachineName.Empty();
   TemplateName.Empty();

   if(m_selmachineList.GetAt(idx))
   {
      MachineName = m_selmachineList.GetAt(idx)->getMachineName();
      TemplateName = m_selmachineList.GetAt(idx)->getTemplateName();
   }
}

// Event handler
void CSelectMultiMachinesDlg::OnBrowse() 
{
   UpdateData();

   SetWindowText(QCUSTOM_ASSEMBLY_WRITE);
   CBrowse dlg;
   dlg.m_strSelDir = m_directory;
   dlg.m_strTitle = "Select path for custom assembly ourput";
   if (dlg.DoBrowse())
   {
      m_directory = dlg.m_strPath;
      UpdateData(FALSE);
   }
}

BOOL CSelectMultiMachinesDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   SetWindowText("Select Assembly Machine");
   LoadMachines();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectMultiMachinesDlg::OnOK() 
{
   if(!m_list.GetSelCount())
   {
      ErrorMessage("Please select a machine.", "No machine is selected", MB_OK);
   }
   else
   {
      for (int idx = 0; idx < m_machineCount; idx++)
      {
         if(m_list.GetSel(idx))
         {
            CDBCustomMachine* cdbmachine = (CDBCustomMachine*)m_list.GetItemData(idx);
            if(cdbmachine) m_selmachineList.Add(cdbmachine);
         }
      }
      CDialog::OnOK();
   }

}

/////////////////////////////////////////////////////////////////////////////
// CCustomAssemblyWriter
/////////////////////////////////////////////////////////////////////////////
CCustomAssemblyWriter::CCustomAssemblyWriter(CString filename, CCEtoODBDoc &doc, FileStruct *pcbfile, FormatStruct *format, CDBCustomMachine &cdbmachine)
: m_progressDlg(NULL)
, m_cadDoc(doc)
, m_pcbfile(pcbfile)
, m_camCadData(doc.getCamCadData())
, m_realpartMap(NULL)
, m_cdbmachine(cdbmachine)
, m_textLogFile(GetLogfilePath("customassembly.log"))
{
   if(!m_database.IsConnected())
      m_database.Connect();

   m_reportFile.Format("%s%s_Report.csv",filename, m_cdbmachine.getMachineName());

   InitSettings(m_cdbmachine.getTemplateName());
   InitParameters(m_cdbmachine.getTemplateName());
}

CCustomAssemblyWriter::~CCustomAssemblyWriter()
{
   if(m_progressDlg)
   {
      m_progressDlg->DestroyWindow();
      delete m_progressDlg;
      m_progressDlg = NULL;
   }

   if(m_realpartMap)
      delete m_realpartMap;

   m_textLogFile.close();
}

CProgressDlg &CCustomAssemblyWriter::getProgressDlg()
{
   if(!m_progressDlg)
   {
      m_progressDlg = new CProgressDlg(QCUSTOM_ASSEMBLY_WRITE, FALSE);
      m_progressDlg->Create();
      m_progressDlg->GetDlgItem(CG_IDC_PROGDLG_PROGRESS)->ShowWindow(SW_SHOW);
      m_progressDlg->GetDlgItem(CG_IDC_PROGDLG_PERCENT)->ShowWindow(SW_SHOW);
      m_progressDlg->SetRange(0,100);
      m_progressDlg->SetPos(0);
   }

   return *m_progressDlg;
}

int CCustomAssemblyWriter::UpdateProcess(int currentpos, int length)
{
   int processPos = (100 * currentpos)/length;
   getProgressDlg().SetPos(processPos);

   return currentpos;
}

void CCustomAssemblyWriter::InitParameters(CString TemplateName)
{
   CDBCustomReportParameterList* cdsparamterlist = m_database.LookupCDBCustomReportParameter(TemplateName);
   if(cdsparamterlist)
   {
      m_ParamList.SetSize(cdsparamterlist->GetCount());
      for(int idx = 0; idx < cdsparamterlist->GetCount(); idx++)
      {
         CDBCustomReportParameter* cdsparamter = cdsparamterlist->GetAt(idx);
         if(cdsparamter)
         {
            CReportParameters *rptparam = new CReportParameters;
            rptparam->getCommand() = cdsparamter->getCommand();
            rptparam->getFieldName() = cdsparamter->getFieldName();
            rptparam->getOptions() = cdsparamter->getParameter();
            m_ParamList.setAt(idx,rptparam);
         }
      }

      delete cdsparamterlist;
   }   
}

void CCustomAssemblyWriter::InitSettings(CString TemplateName)
{
   CDBCustomAssemblyTemplateList *cdbtemplateList = m_database.LookupCustomAssemblyTemplates(TemplateName);
   CAssemblyTemplat m_tmplateData;

   if(cdbtemplateList && cdbtemplateList->GetCount())
   {
      CDBCustomAssemblyTemplate *cdbtemplate = cdbtemplateList->GetAt(0);
      for(int idx = 0; idx < m_tmplateData.getSettingArray().GetCount(); idx++)
      {
         m_tmplateData.getSettingArray().GetAt(idx)->ConvertStringToParamType(cdbtemplate->getTemplateSettings().GetAt(idx + 1));
      } 

      delete cdbtemplateList;
   }
   else  return;

   m_targetUnit = m_tmplateData.getSettingArray().getUnitValue(Option_OutputUnits);
   m_unitFactor = Units_Factor(m_cadDoc.getSettings().getPageUnits(), m_targetUnit);
   
   m_decimals   = m_tmplateData.getSettingArray().getIntValue(Option_NumberOfDecimals);
   m_surface    = m_tmplateData.getSettingArray().getSurfaceValue(Option_WriteSurface);
   m_writeHeader = m_tmplateData.getSettingArray().getYesNoValue(Option_WriteHeader);
   m_explodePanel = m_tmplateData.getSettingArray().getYesNoValue(Option_ExplodePanel);
   m_delimiter = m_tmplateData.getSettingArray().getValue(Option_SeparationCharacter);

   CString mirrorCoordinate = m_tmplateData.getSettingArray().getValue(Option_MirrorCoordinates);
   m_mirrorX = (!mirrorCoordinate.CompareNoCase("None") || !mirrorCoordinate.CompareNoCase("Y"))?1:-1;
   m_mirrorY = (!mirrorCoordinate.CompareNoCase("None") || !mirrorCoordinate.CompareNoCase("X"))?1:-1;

   //Attribute filter
   m_incKeyword = m_tmplateData.getSettingArray().getValue(Option_IncludeInsertAttributeKeyword);
   m_excKeyword = m_tmplateData.getSettingArray().getValue(Option_SuppressInsertAttributeKeyword);
   m_incValue = m_tmplateData.getSettingArray().getValue(Option_IncludeInsertAttributeValue);
   m_excValue = m_tmplateData.getSettingArray().getValue(Option_SuppressInsertAttributeValue);
   
   //Header file
   bool hasTopFile = m_tmplateData.getSettingArray().getYesNoValue(Option_WriteTopHeader);
   m_topHeader =  (hasTopFile)?m_tmplateData.getSettingArray().getValue(Option_TopHeaderFilePath):"";
   bool hasBotFile = m_tmplateData.getSettingArray().getYesNoValue(Option_WriteBottomHeader);
   m_botHeader =  (hasBotFile)?m_tmplateData.getSettingArray().getValue(Option_BottomHeaderFilePath):"";
   
   m_boardName.Empty();
   BuildInsertTypeMap(m_tmplateData);
}

void CCustomAssemblyWriter::BuildInsertTypeMap(CAssemblyTemplat &m_tmplateData)
{
   //Insert Type from setting
   SettingTag InserTypeFilter[] =
   {
      Option_WriteBoards, 
      Option_WriteSmdComponents, 
      Option_WriteThruComponents, 
      Option_WriteFiducials , 
      Option_WriteToolingHoles, 
      Option_WriteMechanicalComponents , 
      Option_WriteGenericComponents, 
      Option_WriteXouts, 
      Option_WriteTestPoints, 
      Option_WriteTestProbes, 
   };

   //CAMCAD define insert type
   InsertTypeTag InsertTypeIndex[] =
   {
      insertTypePcb,
      insertTypePcbComponent, //SMD
      insertTypePcbComponent, //THRU
      insertTypeFiducial,
      insertTypeDrillTool,
      insertTypeMechanicalComponent,
      insertTypeGenericComponent,
      insertTypeXout,
      insertTypeTestPoint,
      insertTypeTestProbe,
   };

   int maxInserTypeFilter = sizeof(InserTypeFilter)/sizeof(SettingTag);
   int maxInsertTypeIndex = sizeof(InsertTypeIndex)/sizeof(InsertTypeTag);

   m_InsertTypeMap.RemoveAll();
   for(int idx = 0; idx < maxInsertTypeIndex; idx++)
   {
      int isChecked = (idx < maxInserTypeFilter)?m_tmplateData.getSettingArray().getYesNoValue(InserTypeFilter[idx]):FALSE;
      
      int orgValue;
      if(m_InsertTypeMap.Lookup(InsertTypeIndex[idx],orgValue))
         isChecked = orgValue << 1 | isChecked;

      m_InsertTypeMap.SetAt(InsertTypeIndex[idx],isChecked);      
   }
}



void CCustomAssemblyWriter::writeFile()
{
   getProgressDlg().SetStatus("Export Custom Assembly Data ...");

   CFileException err;
	if (!m_fReport.Open(m_reportFile, CFile::modeCreate|CFile::modeWrite, &err))
   {
      CString tmp;
      tmp.Format("Can not open CSV File %s", m_reportFile);
      ErrorMessage(tmp, "File Create Error.", MB_OK | MB_ICONHAND);
   }
  

   writeReport((m_surface == DFT_SURFACE_BOT));
   m_fReport.Close();  

   //Opened by NotePad
   Notepad(m_reportFile); 
}

void CCustomAssemblyWriter::writeReport(bool isBottom)
{
   //For calculating the location, Bottom side is alsways Mirrored to Top
   m_attrib =  is_attvalue(&m_cadDoc, m_pcbfile->getBlock()->getDefinedAttributes(), GetMachineAttributeName(m_cdbmachine.getMachineName(),isBottom), 0);
   
   if(m_attrib)
   {
      saveRestoreSettings OriginalSettings(&m_cadDoc, m_pcbfile, isBottom);
      ExportFileSettings m_PCBLocation;
      m_PCBLocation = m_attrib->getStringValue();
      m_PCBLocation.ApplyToCCZ(&m_cadDoc, m_pcbfile, OriginalSettings, fileTypeCustomAssembly);           
   
      writeReport();
      OriginalSettings.Restore();
   }
   else
      writeReport();
   
}

void CCustomAssemblyWriter::writeReport()
{
   //Write Top Header file
   writeReport(m_topHeader);

   //Write Header
   writeReport(m_writeHeader,m_ParamList);

   //Write File List
   writeReport(m_cadDoc.getFileList());

   //Write Bottom Header file
   writeReport(m_botHeader);
}

void CCustomAssemblyWriter::writeReport(CString HeaderFileName)
{
   if(HeaderFileName.IsEmpty()) return;

   CFormatStdioFile headerfile;
   CFileException err;
   if (!headerfile.Open(HeaderFileName, CFile::modeRead | CFile::typeText, &err))
   {
      CString tmp;
      tmp.Format("Can not open header File %s", HeaderFileName);
      ErrorMessage(tmp, "File Create Error.", MB_OK | MB_ICONHAND);
      return;
   }

   CString line;
   while(headerfile.ReadString(line))
      m_fReport.WriteString(line + QNextLine);

   headerfile.Close();
}

void CCustomAssemblyWriter::writeReport(bool writeHeader, CReportParameterList &ParamList)
{
   if(!writeHeader) return;

   int paramCount = ParamList.GetCount();
   for(int idx = 0; idx < paramCount; idx++)
   {
      CReportParameters *rptParam = ParamList.GetAt(idx);
      if(rptParam)
      {
         CString tittle = csv_string(rptParam->getFieldName(),false) + ((idx < paramCount - 1)?m_delimiter:QNextLine);
         m_fReport.WriteString("%s",tittle);
      }
   }
}

void CCustomAssemblyWriter::writeReport(CFileList &fileList)
{
   for(POSITION filepos = fileList.GetHeadPosition();filepos;)
   {
      FileStruct *file = fileList.GetNext(filepos);
      if (file->isShown())
	   {
		   if (file->getBlockType() == blockTypePcb)
         {           
            m_isRestructure = (m_attrib)?true:false;
            writeReport(file, file->getTMatrix());
         }
         else if (file->getBlockType() == blockTypePanel)
         {
            writeReport(file);
         }
      }
   }//for
}

void CCustomAssemblyWriter::writeReport(FileStruct *file)
{
   // Write Panel Level
   m_boardName.Empty();
   m_isRestructure = (m_attrib)?true:false;
   writeReport(file, file->getTMatrix());
   if(!m_explodePanel) return;

   //Write Board Level
   m_isRestructure = false; // Apply CCZ oly affects Panel level not PCB level
   for (POSITION pcbPos = file->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; file->getBlock()->getNextDataInsert(pcbPos))
	{
		DataStruct *pcbData = file->getBlock()->getAtData(pcbPos);
		InsertStruct *pcbInsert = pcbData->getInsert();
		FileStruct *pcbFile = m_cadDoc.getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
		if (pcbFile != NULL)
		{
         m_boardName = pcbInsert->getRefname() + "_";
         CTMatrix pcbMatrix = pcbInsert->getTMatrix() * pcbFile->getTMatrix() * file->getTMatrix();  
         
         writeReport(pcbFile, pcbMatrix);
		}
	}
}

void CCustomAssemblyWriter::writeReport(FileStruct *file, CTMatrix transformMatrix)
{
   resetRealpartMap(file);
   CDataList& dataList = file->getBlock()->getDataList();
   int processPos = 0;

   for(POSITION insertPos = m_InsertTypeMap.GetStartPosition();insertPos;)
   {
      int insertType = -1;
      int option = 0;

      UpdateProcess(processPos ++, m_InsertTypeMap.GetCount());
      m_InsertTypeMap.GetNextAssoc(insertPos,insertType,option);

      for (CDataListIterator componentIterator(dataList, (InsertTypeTag)insertType); option && componentIterator.hasNext();)
      {
		   DataStruct *data = componentIterator.getNext();
         if(data && data->getInsert() && data->getInsert()->getInsertType() == insertType)         
            writeReport(data, transformMatrix, option);
      }
   }//for
}

void CCustomAssemblyWriter::writeReport(DataStruct *data, CTMatrix transformMatrix, int option)
{
   if(!data || !data->getInsert() || data->isHidden()) return;   
   
   //Verify technology
   if(data->isInsertType(insertTypePcbComponent) && !IsMatchedTechnology(data, option)) return;
   
   //kick out data has exclusive keyword
   if(!m_excValue.IsEmpty() && !m_excValue.CompareNoCase(getAttribStringValue(data, m_excKeyword)))
      return;

   //kick out data not has include keyword
   if(!m_incValue.IsEmpty() && m_incValue.CompareNoCase(getAttribStringValue(data, m_incKeyword)))
      return;
   
   //kick out the data with different surface
   if(m_isRestructure)
   {
      if((m_surface == testSurfaceBottom || m_surface == testSurfaceTop ) && data->getInsert()->getPlacedBottom()) return;
   }
   else
   {
      bool hasBottom = (m_surface == testSurfaceBoth || m_surface == testSurfaceBottom);
      bool hasTop = (m_surface == testSurfaceBoth || m_surface == testSurfaceTop );

      if(data->getInsert()->getPlacedBottom() != hasBottom && !data->getInsert()->getPlacedBottom() != hasTop) return;
   }

   writeReport(data, transformMatrix);
}

void CCustomAssemblyWriter::writeReport(DataStruct *data, CTMatrix transformMatrix)
{
   int paramCnt = m_ParamList.GetCount();
   for(int idx = 0; idx < paramCnt; idx++)
   {      
      CReportParameters *rptParam = m_ParamList.GetAt(idx);
      writeReport(data, transformMatrix, rptParam);
      m_fReport.WriteString("%s",(idx < paramCnt - 1)?m_delimiter:QNextLine);
   }
}

void CCustomAssemblyWriter::writeReport(DataStruct *data, CTMatrix transformMatrix, CReportParameters* param)
{
   if(!param || !data || !data->getInsert()) return;

   int ParamIndex = getParamIndex(param->getCommand());
   switch(ParamIndex)
   {
   case Param_RefDes:
      {
         CString refName = data->getInsert()->getRefname();
         refName.Insert(0,(!refName.IsEmpty()?m_boardName:""));
         writeReport_String(refName); 
      }
      break;
   case Param_Surface:     writeReport_Surface(data,param->getOptions());   break;
   case Param_Technology:  writeReport_String(getAttribStringValue(data,ATT_TECHNOLOGY)); break;
   case Param_Geometry:    writeReport_String(getGeometryName(data)); break;
   case Param_Partnumber:  writeReport_String(getAttribStringValue(data,ATT_PARTNUMBER)); break;
   case Param_Device:      writeReport_String(getAttribStringValue(data,ATT_TYPELISTLINK)); break;
   case Param_Type:        writeReport_String(getAttribStringValue(data,ATT_DEVICETYPE)); break;
   case Param_Value:       writeReport_Value(data,param->getOptions());  break;
   case Param_Tolerance:   writeReport_Tolerance(data,param->getOptions());   break;
   case Param_Attribute:   writeReport_String(getAttribStringValue(data,param->getOptions()));break;
   case Param_Xloc:          
   case Param_Yloc:        
   case Param_Rotation:
      writeReport_Location(data, transformMatrix, param->getOptions(),ParamIndex); 
      break;
   }
}

void CCustomAssemblyWriter::writeReport_Surface(DataStruct *data, CString options)
{
   ETestSurface surface = testSurfaceUnset;
   if(!m_isRestructure)
   {
      if(data->getInsert()->getPlacedTop() && data->getInsert()->getPlacedBottom())
         surface = testSurfaceBoth;
      else if(data->getInsert()->getPlacedBottom())
         surface = testSurfaceBottom;
      else if(data->getInsert()->getPlacedTop())
         surface = testSurfaceTop;
   }
   else
   {
      if(m_surface == testSurfaceBoth)
         surface = data->getInsert()->getPlacedBottom()?testSurfaceBottom:testSurfaceTop;
      else 
         surface = m_surface;
   }

   if(!options.CompareNoCase(OPT_NUMBER))
      writeReport_Number(surface);
   else   //String 
      writeReport_String(testSurfaceTagToString(surface));
}

void CCustomAssemblyWriter::writeReport_Tolerance(DataStruct *data, CString options)
{
   double tolerance = 0.;

   if(!options.CompareNoCase(OPT_PLUS) && getAttribDoubleValue(data,ATT_PLUSTOLERANCE,tolerance))
      writeReport_Double(tolerance);
   else if(!options.CompareNoCase(OPT_MINUS) && getAttribDoubleValue(data,ATT_MINUSTOLERANCE,tolerance))
      writeReport_Double(tolerance);
   else if(!options.CompareNoCase(OPT_STRING))//String
      writeReport_String(getAttribStringValue(data,ATT_TOLERANCE));    
   else
      writeReport_String("");
}

void CCustomAssemblyWriter::writeReport_Value(DataStruct *data, CString options)
{
   CString value = getAttribStringValue(data,ATT_VALUE);   
   
   if(!options.CompareNoCase(OPT_UNITS) && !value.IsEmpty())
   {
      int idx = 0;
      while((idx = value.FindOneOf(QDIGITS)) > -1)
         value.Remove(value.GetAt(idx));   

      writeReport_String(value);
   }
   else if(!options.CompareNoCase(OPT_NUMBER) && !value.IsEmpty())
   {
      if(value.Find(".") > -1)
         writeReport_Double(atof(value));
      else
         writeReport_Number(atoi(value));
   }
   else
      writeReport_String(value);
}

void CCustomAssemblyWriter::writeReport_Location(DataStruct *data, CTMatrix transformMatrix, CString options, int ParamIndex)
{   
   if(!options.CompareNoCase(OPT_INSERT))
   {
      CBasesVector insertBasesVector = data->getInsert()->getBasesVector();
      insertBasesVector.transform(transformMatrix);

      writeReport_Double(getLocationValue(insertBasesVector,ParamIndex));
   }
   else if(!options.CompareNoCase(OPT_REALPART))
   {
      CString refName = data->getInsert()->getRefname();
      
      CComponentRealpartLink *crpLink = NULL;
      if(m_realpartMap->Lookup(refName,crpLink) && crpLink)
      {
         DataStruct *realpartData = crpLink->getRealPartData();
         if(realpartData && realpartData->getInsert())
         {
            CBasesVector realpartBasesVector = realpartData->getInsert()->getBasesVector();
            realpartBasesVector.transform(transformMatrix);
            writeReport_Double(getLocationValue(realpartBasesVector,ParamIndex));
         }
      }
   }
   else //Centroid
   {
      CBasesVector insertBasesVector = data->getInsert()->getBasesVector();
      insertBasesVector.transform(transformMatrix);

      if (!options.CompareNoCase(OPT_CENTROID))
      {
         DataStruct *centroid = getCentroidInsertData(data);
         if(centroid)
         {
            CBasesVector centroidBasesVector = centroid->getInsert()->getBasesVector();
            centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());         
            writeReport_Double(getLocationValue(centroidBasesVector,ParamIndex));
         }
      }
      else //Recaluculate centroid based on method
      {
         BlockStruct *block = m_cadDoc.getBlockAt(data->getInsert()->getBlockNumber());
         if(!block) return;

         int centAlg = getCentroidMethod(options);
         if(centAlg == Centroid_XyValue) // Extent of geometry block of an insert
         {
            CExtent compExtent = block->getExtent(m_cadDoc.getCamCadData());            
            CPoint2d centroid = compExtent.getCenter();
         
            CBasesVector centroidBasesVector(centroid,block->getRotation());
            centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());
   
            writeReport_Double(getLocationValue(centroidBasesVector,ParamIndex));
         }
         else if (centAlg != Centroid_None) //Centroid_PinCenters, Centroid_PinExtents, Centroid_CompOutline
         {
            double centX =0., centY = 0.;
            if(CalculateCentroid(&m_cadDoc, block, centAlg, &centX, &centY))
            {
               CBasesVector centroidBasesVector(centX,centY);
               centroidBasesVector.transform(insertBasesVector.getTransformationMatrix());
            
               writeReport_Double(getLocationValue(centroidBasesVector,ParamIndex));
            }
         }//if
      }
   }//if  
}

void CCustomAssemblyWriter::writeReport_String(CString value)
{
   m_fReport.WriteString("%s",csv_string(value));
}

void CCustomAssemblyWriter::writeReport_Number(int value)
{
   m_fReport.WriteString("%ld",value);
}

void CCustomAssemblyWriter::writeReport_Double(double value)
{
   m_fReport.WriteString("%.*lf",m_decimals,value);
}

DataStruct *CCustomAssemblyWriter::getCentroidInsertData(DataStruct *data)
{
   BlockStruct *block = m_cadDoc.getBlockAt(data->getInsert()->getBlockNumber());
   DataStruct *centroid = (block)?block->GetCentroidData():NULL;

   return centroid;
}

bool CCustomAssemblyWriter::IsMatchedTechnology(DataStruct *data, int option)
{
   CAttribute* attrib = NULL;
   int keyWord = -1;

   if((keyWord = m_camCadData.getAttributeKeywordIndex(standardAttributeTechnology)) > -1 
      && data->getAttributesRef()->Lookup(keyWord,attrib))
   {
      if(attrib) 
      {
         bool isSMD = !attrib->getStringValue().CompareNoCase(QTECHNOLOGY_SMD);
         bool isTHRU = !attrib->getStringValue().CompareNoCase(QTECHNOLOGY_THRU);

         if((option & 0x02) &&  isSMD)
            return true;         
         else if((option & 0x01) && isTHRU)
            return true;  
      }
   }

   return false;
      
}

int CCustomAssemblyWriter::getCentroidMethod(CString option)
{
   int centroidAlg[] = {Centroid_PinCenters, Centroid_PinExtents, Centroid_XyValue, Centroid_CompOutline};
   CStringArray optionList; 
   CSupString options(DefaultReportValue[Param_Xloc].Options);
   options.ParseQuote(optionList,"|");

   for(int idx = 0; idx < sizeof(centroidAlg)/sizeof(int); idx++)
   {
      if(!option.CompareNoCase(optionList.GetAt(idx + 2)))
         return centroidAlg[idx];
   }

   return Centroid_None;   
}

CString CCustomAssemblyWriter::getAttribStringValue(DataStruct *data, CString keywordString)
{
   CAttribute* attrib = NULL;
   int keyWord = -1;
   CString value = "";

   if(!keywordString.IsEmpty() && (keyWord = m_camCadData.getAttributeKeywordIndex(keywordString)) > -1) 
       if(data->getAttributesRef() && data->getAttributesRef()->Lookup(keyWord,attrib) && attrib)
         value =attrib->getStringValue();

   return value;
}

bool CCustomAssemblyWriter::getAttribDoubleValue(DataStruct *data, CString keywordString, double &value)
{
   CAttribute* attrib = NULL;
   int keyWord = -1;

   if(!keywordString.IsEmpty() && (keyWord = m_camCadData.getAttributeKeywordIndex(keywordString)) > -1)
   {  
      if(data->getAttributesRef() && data->getAttributesRef()->Lookup(keyWord,attrib) && attrib)
      {
         value = attrib->getDoubleValue();
         return true;
      }
   }
   return false;
}

void CCustomAssemblyWriter::resetRealpartMap(FileStruct* file)
{
   if(m_realpartMap) delete m_realpartMap;
   m_realpartMap = new CComponentRealpartLinkMap(m_cadDoc, *file);
}

CString CCustomAssemblyWriter::getGeometryName(DataStruct *data)
{
   BlockStruct *block = m_cadDoc.getBlockAt(data->getInsert()->getBlockNumber());
   return (block)?block->getName():"";
}

int CCustomAssemblyWriter::getParamIndex(CString Commands)
{
   for(int idx = 0; idx < sizeof(DefaultReportValue)/sizeof(ReportInitValue);idx++)
   {
      if(!Commands.CompareNoCase(DefaultReportValue[idx].Command))
         return DefaultReportValue[idx].paramIndex;
   }
   return -1;
}

double CCustomAssemblyWriter::getLocationValue(CBasesVector insertBasesVector, int ParamIndex)
{
   double value = 0.;

   switch(ParamIndex)
   {
   case Param_Xloc:
      value = getLocationValue(insertBasesVector.getX(), m_mirrorX, m_unitFactor);
      break;
   case Param_Yloc:
      value = getLocationValue(insertBasesVector.getY(), m_mirrorY, m_unitFactor);
      break;
   case Param_Rotation:
      value = normalizeDegrees(round(insertBasesVector.getRotationDegrees()));
      break;
   }

   return value;
}