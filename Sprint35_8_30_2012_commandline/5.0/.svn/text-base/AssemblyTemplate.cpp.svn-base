
#include "stdafx.h"
#include "DirDlg.h"
#include "AssemblyTemplate.h"
#include "CustomAssembly_writer.h"
#include "MultipleMachine.h"
#include ".\assemblytemplate.h"

extern CView *activeView; // from CCVIEW.CPP

///////////////////////////////////////////////////////////////////////////
// CDelimiterConverter
/////////////////////////////////////////////////////////////////////////////
static char *delimiterTable[] = {DEL_TAB,DEL_COMMA,DEL_PIPE,DEL_SEMICOLON,DEL_OTHERS};
static DelimiterCode delimiterCode[] = {DELCODE_TAB,DELCODE_COMMA,DELCODE_PIPE,DELCODE_SEMICOLON,DELCODE_OTHERS};
CDelimiterConverter::CDelimiterConverter()
{
   int count = sizeof(delimiterTable)/sizeof(char*);   
   for(int idx = 0; idx < count; idx++)
   {
      m_delimiterTable.SetAt(delimiterCode[idx],delimiterTable[idx]);
      m_delimiterCode.setAt(delimiterTable[idx],&delimiterCode[idx]);
   }   
      
}

CDelimiterConverter::~CDelimiterConverter()
{
   m_delimiterTable.RemoveAll(); 
   m_delimiterCode.RemoveAll();
}

CString CDelimiterConverter::delimiterCharToName(CString delimiter)
{
   int code = delimiter.GetAt(0);
   char *delimiterName = NULL;
   if(m_delimiterTable.Lookup(code,delimiterName) && delimiterName)
      return delimiterName;

   return DEL_OTHERS;
}

CString CDelimiterConverter::delimiterCodeToName(DelimiterCode code)
{  
   char *delimiterName = NULL;
   if(m_delimiterTable.Lookup(code,delimiterName) && delimiterName)
      return delimiterName;

   return DEL_OTHERS;
}

DelimiterCode CDelimiterConverter::delimiterNameToCode(CString delimiter)
{  
   DelimiterCode *delcode = NULL;
   if(m_delimiterCode.Lookup(delimiter, delcode) && delcode)
      return *delcode;

   return DELCODE_OTHERS;
}

CString CDelimiterConverter::delimiterCodeToString(DelimiterCode code)
{
   CString delimiter;
   delimiter.Format("%c",code);

   return delimiter;
}

/////////////////////////////////////////////////////////////////////////////
// CReportParameters 
/////////////////////////////////////////////////////////////////////////////
CReportParameters::CReportParameters(ParameterTag paramIdx)
: m_ParamIdx(paramIdx)
{
   m_sizeofRepValue = sizeof(DefaultReportValue)/sizeof(ReportInitValue);
   if(paramIdx >= Param_Min && paramIdx < m_sizeofRepValue)
   {
      m_Command = DefaultReportValue[m_ParamIdx].Command;
      m_FieldName = DefaultReportValue[m_ParamIdx].Command;
      m_Options = DefaultReportValue[m_ParamIdx].Options;
   }
}

CReportParameters::CReportParameters()
{
   m_Command.Empty();
   m_FieldName.Empty();
   m_Options.Empty();
}

CReportParameters::~CReportParameters()
{
}

///////////////////////////////////////////////////////////////////////////
// CReportParametersArray
/////////////////////////////////////////////////////////////////////////////
CReportParametersArray::~CReportParametersArray()
{
   for(POSITION paramPos = GetStartPosition();paramPos;)
   {
      CReportParameters *rptParam = NULL;
      CString paramName;
      GetNextAssoc(paramPos,paramName,rptParam);
      if(rptParam) delete rptParam;
   }
   RemoveAll();
}

///////////////////////////////////////////////////////////////////////////
// CGeneralSettings
/////////////////////////////////////////////////////////////////////////////
static GeneralInitValue DefaultSettingValue[] =
{
   {Option_SeparationCharacter,"SeparationCharacter",",",ParamTypeString},
   {Option_WriteHeader,"WriteHeader","True",ParamTypeBoolean},
   {Option_ExplodePanel,"ExplodePanel","False",ParamTypeBoolean},
   {Option_WriteTopHeader,"WriteTopHeader","False",ParamTypeBoolean},
   {Option_WriteBottomHeader,"WriteBottomHeader","False",ParamTypeBoolean},
   {Option_TopHeaderFilePath,"TopHeaderFilePath","",ParamTypeString},
   {Option_BottomHeaderFilePath,"BottomHeaderFilePath","",ParamTypeString},
   {Option_ReportOrigin,"ReportOrigin","CAMCAD",ParamTypeString},   
   {Option_OutputUnits,"OutputUnits",unitString(pageUnitsInches),ParamTypeUnit},   
   {Option_NumberOfDecimals,"NumberOfDecimals","3",ParamTypeInteger},
   {Option_MirrorCoordinates,"MirrorCoordinates","None",ParamTypeString},
   {Option_WriteSurface,"WriteSurface",testSurfaceTagToString(testSurfaceBoth),ParamTypeSurface},
   {Option_SuppressInsertAttributeKeyword,"SuppressInsertAttributeKeyword","",ParamTypeString},
   {Option_SuppressInsertAttributeValue,"SuppressInsertAttributeValue","",ParamTypeString},
   {Option_IncludeInsertAttributeKeyword,"IncludeInsertAttributeKeyword","",ParamTypeString},
   {Option_IncludeInsertAttributeValue,"IncludeInsertAttributeValue","",ParamTypeString},
   //---------Insert Filter Start--------------
   // Please put filter options last
   {Option_WriteBoards,"WriteBoards","True",ParamTypeBoolean},
   {Option_WriteSmdComponents,"WriteSmdComponents","True",ParamTypeBoolean},
   {Option_WriteThruComponents,"WriteThruComponents","True",ParamTypeBoolean},
   {Option_WriteFiducials,"WriteFiducials","True",ParamTypeBoolean},
   {Option_WriteToolingHoles,"WriteToolingHoles","False",ParamTypeBoolean},
   {Option_WriteMechanicalComponents,"WriteMechanicalComponents","False",ParamTypeBoolean},
   {Option_WriteGenericComponents,"WriteGenericComponents","False",ParamTypeBoolean},
   {Option_WriteXouts,"WriteXouts","False",ParamTypeBoolean},
   {Option_WriteTestPoints,"WriteTestPoints","False",ParamTypeBoolean},
   {Option_WriteTestProbes,"WriteTestProbes","False",ParamTypeBoolean},
   //---------Insert Filter End--------------
};

CGeneralSettings::CGeneralSettings(SettingTag settingIdx)
: m_settingIndex(settingIdx)
{
   if(getFieldDefaultValue(settingIdx,m_fieldName,m_InitValue,m_paramType))
   {
      InitParameters();
      ResetParameters();
   }
}

CGeneralSettings::~CGeneralSettings()
{
   delete m_Parameter;
}

bool CGeneralSettings::getFieldDefaultValue(SettingTag settingIdx, CString &FileName, CString &defaltValue, ParamTypeTag& ParamType)
{
   m_sizeofSetValue = sizeof(DefaultSettingValue)/sizeof(GeneralInitValue);
   if(settingIdx < Option_Min || settingIdx >= m_sizeofSetValue)
      return false;

   FileName = DefaultSettingValue[settingIdx].FieldName;
   defaltValue = DefaultSettingValue[settingIdx].InitValue;
   ParamType = DefaultSettingValue[settingIdx].paramType;
   return true;
}

void CGeneralSettings::InitParameters()
{
   m_Parameter = NULL;
   switch(m_paramType)
   {
   case ParamTypeString:
      m_Parameter = new CString;
      break;
   case ParamTypeBoolean:
   case ParamTypeInteger:
      m_Parameter = new int;
      break;
   case ParamTypeUnit:
      m_Parameter = new PageUnitsTag;
      break;
   case ParamTypeSurface:
      m_Parameter = new ETestSurface;
      break;
   }    
}

void CGeneralSettings::ResetParameters()
{
   switch(m_paramType)
   {
   case ParamTypeString:
      setParam(m_InitValue);
      break;
   case ParamTypeBoolean:
      setYesNoParam(m_InitValue.CompareNoCase("False"));
      break;
   case ParamTypeInteger:
      setIntParam(atoi(m_InitValue));
      break;
   case ParamTypeUnit:
      setUnitsParam(m_InitValue);
      break;
   case ParamTypeSurface:
      setSurfaceParam(m_InitValue);
      break;
   };   
}

CString CGeneralSettings::ConvertParamToString()
{
   CString Value ="";

   switch(m_paramType)
   {
   case ParamTypeBoolean:
      Value = getYestNoParamStr();
      break;
   case ParamTypeInteger:
      Value.Format("%d",getIntParam());
      break;
   case ParamTypeString:
      Value = getParam();
      break;
   case ParamTypeSurface:
      Value = testSurfaceTagToString((ETestSurface)getIntParam());
      break;
   case ParamTypeUnit:
      Value = PageUnitsTagToString(intToPageUnitsTag(getIntParam()));
      break;
   }

   return Value;
}

void CGeneralSettings::ConvertStringToParamType(CString Value)
{
   switch(m_paramType)
   {
   case ParamTypeBoolean:
      setYesNoParam(!Value.CompareNoCase("true")?true:false);
      break;
   case ParamTypeInteger:
      setIntParam(atoi(Value));
      break;
   case ParamTypeString:
      setParam(Value);
      break;
   case ParamTypeSurface:
      setSurfaceParam(Value);
      break;
   case ParamTypeUnit:
      setUnitsParam(Value);
      break;
   }
}

void CGeneralSettings::setUnitsParam(CString units)
{
   *((PageUnitsTag*)m_Parameter) = unitStringToTag(units);
}

void CGeneralSettings::setSurfaceParam(CString surface)
{
   if(surface.IsEmpty())
      return;

   for(int idx = testSurfaceMIN; idx < testSurfaceMAX; idx++)
   {
      if(!surface.CompareNoCase(testSurfaceTagToString((ETestSurface)idx)))
         *((int*)m_Parameter) = idx;
   }
}


/////////////////////////////////////////////////////////////////////////////
// CGeneralSettingsArray 
/////////////////////////////////////////////////////////////////////////////
CGeneralSettingsArray::CGeneralSettingsArray()
{
   m_SettingMap.RemoveAll();
}

CGeneralSettingsArray::~CGeneralSettingsArray()
{
   m_SettingMap.RemoveAll();
   for(int idx = 0; idx < GetCount(); idx++)
      if(GetAt(idx)) delete GetAt(idx);

   RemoveAll();
}

void CGeneralSettingsArray::Set(CGeneralSettings *genSetting)
{
   if(genSetting)
   {
      Add(genSetting);
      m_SettingMap.setAt(genSetting->getFieldName(),genSetting);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplat 
/////////////////////////////////////////////////////////////////////////////
CAssemblyTemplat::CAssemblyTemplat()
{
   InitDataParameter();
   InitGeneralSettings();
   InitMirrorCoordinateArray();
   InitDelimiterArray();
}

CAssemblyTemplat::~CAssemblyTemplat()
{
   m_MirrorCoordinateArray.RemoveAll();
}

void CAssemblyTemplat::InitDataParameter()
{
   for(int idx = Param_Min; idx < Param_Last; idx++)
   {
      CReportParameters *rptParam = new CReportParameters((ParameterTag)idx);
      m_ParameterArray.SetAt(rptParam->getCommand(),rptParam);
   }  
}

void CAssemblyTemplat::InitGeneralSettings()
{
   for(int idx = Option_Min; idx < Option_Last; idx++)
   {
      CGeneralSettings *genSetting = new CGeneralSettings((SettingTag)idx);
      m_generalSettings.Set(genSetting);
   }  
}

void CAssemblyTemplat::InitMirrorCoordinateArray()
{
   char *coordinates[] = {"None","X", "Y","Both"};
   
   m_MirrorCoordinateArray.RemoveAll();
   for(int i = 0; i < sizeof(coordinates)/sizeof(char*); i++)
      m_MirrorCoordinateArray.Add(coordinates[i]);
}

void CAssemblyTemplat::InitDelimiterArray()
{
   char *delimiterTable[] = {DEL_TAB,DEL_COMMA,DEL_PIPE,DEL_SEMICOLON,DEL_OTHERS};
   
   m_DelimiterArray.RemoveAll();
   for(int i = 0; i < sizeof(delimiterTable)/sizeof(char*); i++)
      m_DelimiterArray.Add(delimiterTable[i]);
}

/////////////////////////////////////////////////////////////////////////////
// CTemplateOutputAttrib 
/////////////////////////////////////////////////////////////////////////////
CTemplateOutputAttrib::CTemplateOutputAttrib(CCEtoODBDoc& camCadDoc) 
:  m_camCadDatabase(camCadDoc)
,  m_attribKeyword(-1)
{
	SetDefaultAttribAndValue();
}

CTemplateOutputAttrib::~CTemplateOutputAttrib()
{
	m_valuesMap.RemoveAll();
}

int CTemplateOutputAttrib::GetKeyword() const
{
	return m_attribKeyword;
}

bool CTemplateOutputAttrib::HasValue(CAttributes** attributes)
{
	CString value, tmpValue;
	m_camCadDatabase.getAttributeStringValue(value, attributes, m_attribKeyword);
	return (m_valuesMap.Lookup(value, tmpValue)==TRUE)?true:false;
}

void CTemplateOutputAttrib::SetKeyword(const int keyword)
{
	m_attribKeyword = keyword;
	m_valuesMap.RemoveAll();
}

void CTemplateOutputAttrib::AddValue(const CString value)
{
	CString tmpValue = value;
	m_valuesMap.SetAt(tmpValue, tmpValue);
}

void CTemplateOutputAttrib::SetDefaultAttribAndValue()
{

}

/////////////////////////////////////////////////////////////////////////////
// CTemplateKeywrodMap
/////////////////////////////////////////////////////////////////////////////
CTemplateKeywordMap::CTemplateKeywordMap(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
{
}

CTemplateKeywordMap::~CTemplateKeywordMap()
{
   for(POSITION attribPos = GetStartPosition();attribPos;)
   {
      int keyword;
      CTemplateOutputAttrib *attributes = NULL;
      GetNextAssoc(attribPos,keyword,attributes);
      if(attributes) delete attributes;
   }
   RemoveAll();
}

void CTemplateKeywordMap::Add(int keyWord, CString value)
{
   if(keyWord == -1)
      return;

   CTemplateOutputAttrib *m_outAttrib = NULL;
   if(!Lookup(keyWord,m_outAttrib) || !m_outAttrib)
   {
      m_outAttrib = new CTemplateOutputAttrib(m_camCadDoc);
      m_outAttrib->SetKeyword(keyWord);
      SetAt(keyWord,m_outAttrib);
   }

   if(!value.IsEmpty())m_outAttrib->AddValue(value);
}

void CTemplateKeywordMap::Add(CAttributes *attributes)
{
   if(!attributes)
      return;

   for(POSITION attribPos = attributes->GetStartPosition();attribPos;)
   {
      WORD keyword;
      CAttribute *attrib = NULL;
      attributes->GetNextAssoc(attribPos,keyword,attrib);
      if(attrib) Add(attrib->getKeywordIndex(),attrib->getStringValue());
   }
}

bool CTemplateKeywordMap::LookupValues(CString Keyword, CStringArray &ValueList)
{
   int keyIndex = m_camCadDatabase.getKeywordIndex(Keyword);
   
   CTemplateOutputAttrib *m_outAttrib = NULL;
   if(Lookup(keyIndex,m_outAttrib) && m_outAttrib)
   {
      for(POSITION valuePos = m_outAttrib->getValuesMap().GetStartPosition();valuePos;)
      {
         CString key, value;
         m_outAttrib->getValuesMap().GetNextAssoc( valuePos,key,value);
         if(!value.IsEmpty())ValueList.Add(value);
      }

      return true;
   }

   return false;
}

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateDirectory dialog
/////////////////////////////////////////////////////////////////////////////
bool CAssemblyTemplateDirectory::FindTemplateRegValue(CString &Value)
{  

   if(m_cadDoc == NULL)
      return false;

   //Search CAMCAD Profile Settings
   Value = m_cadDoc->GetProfileString("Settings", KEY_CUSTOMTEMPLATE_DIR, "");
   if(Value.IsEmpty())
   {
      if(FindStringValueByKey(KEY_SOFTWARE + (CString)REGISTRY_COMPANY + (CString)KEY_CAMCADSETTING, KEY_CUSTOMTEMPLATE_DIR, Value) 
         && !Value.IsEmpty())
      {
         // Search CAMCAD Settings
      }
      else
      {
         // Search for Directory
         Value = m_cadDoc->GetProfileString("Settings", "Directory", DEFAULT_CUSTOMTEMPLATE_FOLDER);
         if(Value.IsEmpty())
         {
            Value = DEFAULT_CUSTOMTEMPLATE_FOLDER;
         }      
      }

      //Normalize Value
      CString NormailizeVal = Value.Mid(0,1).MakeUpper();
      Value.SetAt(0,NormailizeVal.GetAt(0));
      m_cadDoc->WriteProfileString("Settings", KEY_CUSTOMTEMPLATE_DIR, Value);
   }

   return true;
}

void CAssemblyTemplateDirectory::SetTemplateDirectory(CString Value)
{
   if(m_cadDoc)
      m_cadDoc->WriteProfileString("Settings", KEY_CUSTOMTEMPLATE_DIR, Value);
}

bool CAssemblyTemplateDirectory::FindStringValueByKey(CString keyname, CString RegName,CString &strValue)
{
   HRESULT res;
   HKEY settingsKey;

   strValue.Empty();
   res = RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_READ, &settingsKey);
   if (res == S_OK)
   {  
      DWORD dwCount, dwType;
      LONG lResult = RegQueryValueEx(settingsKey, RegName, NULL, &dwType,
			NULL, &dwCount);        
      if (res == S_OK)
      {
         res = RegQueryValueEx(settingsKey, RegName, NULL, &dwType, 
            (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);   

         strValue.ReleaseBuffer();
         RegCloseKey(settingsKey);
         return true;
      }
   }
   RegCloseKey(settingsKey);

   return false;
}

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateDirectoryDlg dialog
/////////////////////////////////////////////////////////////////////////////
CAssemblyTemplateDirectoryDlg::CAssemblyTemplateDirectoryDlg(CWnd* pParent /*=NULL*/)	
   : CDialog(CAssemblyTemplateDirectoryDlg::IDD, pParent)
{

}

CAssemblyTemplateDirectoryDlg::~CAssemblyTemplateDirectoryDlg()
{
}

void CAssemblyTemplateDirectoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CBOMTemplateDirectoryDlg)
   DDX_Text(pDX, IDC_TEMPLATE_FOLDER, m_TemplatePath);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAssemblyTemplateDirectoryDlg, CDialog)
  //{{AFX_DATA_MAP(CBOMTemplateDirectoryDlg)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowseTemplatePath)
   //}}AFX_DATA_MAP
END_MESSAGE_MAP()

BOOL CAssemblyTemplateDirectoryDlg::OnInitDialog()
{
   CDialog::OnInitDialog();  

   m_Templatedir.FindTemplateRegValue(m_TemplatePath);

   UpdateData(FALSE);
   return TRUE;
}

void CAssemblyTemplateDirectoryDlg::OnBrowseTemplatePath()
{
   // TODO: Add your control notification handler code here
   CBrowse dlg;
   dlg.m_strSelDir = m_TemplatePath;
   dlg.m_strTitle = "Select Project Path";
   if (dlg.DoBrowse())
   {
		// 0 is the current project path
      m_TemplatePath = dlg.m_strPath;
		UpdateData(FALSE);
   }
}

///////////////////////////////////////////////////////////////////////////
// CExtendComboBox
/////////////////////////////////////////////////////////////////////////////

void CExtendComboBox::ClearAll()
{
   for(int itemCnt = GetCount(); itemCnt > 0 ; itemCnt --)
   {
      DeleteString(0);  
      DeleteItem(0);
   }
   SetWindowText("");
   Clear();
}

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateGrid
/////////////////////////////////////////////////////////////////////////////
CAssemblyTemplateGrid::CAssemblyTemplateGrid()
{
   m_ColumnSize = 3;
   m_SelectRow = -1;
   m_SelectCol = -1;

   m_gridHeader.RemoveAll();
   m_gridHeader.Add(QCTCOMMAND);
   m_gridHeader.Add(QCTPARAMETER);
   m_gridHeader.Add(QCTFIELDNAME);
}

CAssemblyTemplateGrid::~CAssemblyTemplateGrid()
{
   m_gridHeader.RemoveAll();
}

//------------------------------------------------------------------
// CAssemblyTemplateGrid: Edit fields in Grid
//------------------------------------------------------------------
void CAssemblyTemplateGrid::initGrid()
{
   if (GetNumberCols() < m_gridHeader.GetCount())
   {     
      int defaultColSize = m_gridHeader.GetCount();
      SetSH_Width(0);
      SetNumberRows(0,false);
      SetNumberCols(defaultColSize,false);

      for(int idx = 0; idx < defaultColSize; idx++)
      {
         if( m_gridHeader.GetAt(idx))
            QuickSetText(idx, -1, m_gridHeader.GetAt(idx));
      }
   }

   SetCellOption(T_Parameter,UGCT_DROPLIST);   
   SetNumberRows(0);
   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CAssemblyTemplateGrid::SetCellOption(int col, int option)
{
   //First Column is a droplist
   CUGCell cell;
   GetColDefault(col, &cell); 
   cell.SetCellType(option); 
   SetColDefault(col, &cell);
}

void CAssemblyTemplateGrid::QuickSetDropList(int col, int row, CString options)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   CString optionlist(options); 
   optionlist.Replace("|", "\n");
   cell.SetLabelText( optionlist );

   CSupString supOption(options);
   CStringArray optionArray;   
   supOption.ParseQuote(optionArray,"|");
   cell.SetText(optionArray.GetCount()?optionArray.GetAt(0):"");
   SetCell(col, row, &cell);
}

void CAssemblyTemplateGrid::SetCellReadOnly(int col, int row, bool readonly)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   cell.SetReadOnly(readonly);
   SetCell(col, row, &cell);
}

CString CAssemblyTemplateGrid::getSelectValue(int col, int row)
{
   CUGCell cell;

   GetCell(col,row,&cell);
   return cell.GetText();
}

void CAssemblyTemplateGrid::AddRowData(CReportParameters *rptParameter)
{
   int RowSize = GetNumberRows();
   if(rptParameter)
   {
      SetNumberRows(RowSize + 1);
      SetCellReadOnly(T_Command, RowSize, true);

      QuickSetText(T_Command, RowSize, rptParameter->getCommand());  
      QuickSetDropList(T_Parameter, RowSize, rptParameter->getOptions());
      QuickSetText(T_FieldName, RowSize, rptParameter->getFieldName());
   }

   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CAssemblyTemplateGrid::SetRowData(int col, int row, CString data)
{
   if(row > -1 && row < GetNumberRows() && col > -1 && col < GetNumberCols())
   {
      QuickSetText(col, row, data);  
      BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
   }
}

void CAssemblyTemplateGrid::GetRowData(int row, CReportParameters &rptParameter)
{
   if(row > -1 && row < GetNumberRows())
   {
      QuickGetText(T_Command,row,&rptParameter.getCommand());
      QuickGetText(T_Parameter,row,&rptParameter.getOptions());
      QuickGetText(T_FieldName,row,&rptParameter.getFieldName());
   }
}

void CAssemblyTemplateGrid::GetRowData(int col, int row, CString& data)
{
   data.Empty();
   if(row > -1 && row < GetNumberRows() && col > -1 && col < GetNumberCols())
      QuickGetText(col,row,&data);
}

void CAssemblyTemplateGrid::DeleteSelectedRow()
{
   if(m_SelectRow > -1)
   {
      DeleteRow(m_SelectRow);

      if(GetNumberRows())
         OnUpdateSelection((m_SelectRow == 0)?0:m_SelectRow - 1);      

      m_SelectRow = (m_SelectRow == 0 && GetNumberRows())?0:m_SelectRow - 1;
   }

   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CAssemblyTemplateGrid::DeleteAll()
{
   for(m_SelectRow = GetNumberRows();m_SelectRow > -1; )
      DeleteSelectedRow();
}

void CAssemblyTemplateGrid::ExchaneRowStringData(int Col, int srcRow, int dstRow)
{
   CString srcdata = QuickGetText(Col,srcRow);
   CString dstdata = QuickGetText(Col,dstRow);

   QuickSetText(Col,srcRow,dstdata);
   QuickSetText(Col,dstRow,srcdata);
}

void CAssemblyTemplateGrid::ExchaneRowDropList(int Col, int srcRow, int dstRow) 
{
   CUGCell srcCell, dstCell;
   CString srcLableText, dstLableText;

   GetCell(Col, srcRow, &srcCell);
   GetCell(Col, dstRow, &dstCell);
  
   SetCell(Col,srcRow,&dstCell);
   SetCell(Col,dstRow,&srcCell);

}

void CAssemblyTemplateGrid::MoveUpSelectedRow()
{
   if(m_SelectRow > 0)
   {
      ExchaneRowStringData(T_Command, m_SelectRow,m_SelectRow - 1);
      ExchaneRowStringData(T_FieldName, m_SelectRow,m_SelectRow - 1);
      ExchaneRowDropList(T_Parameter, m_SelectRow,m_SelectRow - 1);

      OnUpdateSelection(m_SelectRow - 1);
      m_SelectRow --;
   }

   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CAssemblyTemplateGrid::MoveDownSelectedRow()
{
   if(m_SelectRow > -1 && m_SelectRow < GetNumberRows() - 1)
   {
      ExchaneRowStringData(T_Command,m_SelectRow,m_SelectRow + 1);
      ExchaneRowStringData(T_FieldName,m_SelectRow,m_SelectRow + 1);
      ExchaneRowDropList(T_Parameter, m_SelectRow,m_SelectRow + 1);
      
      OnUpdateSelection(m_SelectRow + 1);
      m_SelectRow ++;
   }

   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

//------------------------------------------------------------------
// CAssemblyTemplateGrid: functions to process event 
//------------------------------------------------------------------
int CAssemblyTemplateGrid::OnEditVerifyDigitalCell(int col, long row,CWnd *edit,UINT *vcKey)
{
   // Cursor movement keys are okay
   //if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE)
   //   return TRUE;
   if (*vcKey == VK_BACK || *vcKey == 0x7F /*Delete*/ 
      || *vcKey == VK_CANCEL/*Ctrl+C*/ || *vcKey == 0x16/*Ctrl+V*/)
      return TRUE;

   // Check digital number
   return isdigit(*vcKey);
}

int CAssemblyTemplateGrid::OnEditVerifyAttributeCell(int col, long row,CWnd *edit)
{
   CString curTxt, restoreText;
   
   CEdit *cedit = (CEdit*)edit;
   edit->GetWindowText(curTxt);

   
   int errorCode = 0;
   return errorCode;
}

void CAssemblyTemplateGrid::OnUpdateSelection(int curRow)
{
   if (curRow >= 0 && curRow < GetNumberRows() && m_ColumnSize > 0)
   {
      SetMultiSelectMode(TRUE);
      GotoRow(curRow);
      SelectRange(0, curRow, m_ColumnSize, curRow);
   }
}

//------------------------------------------------------------------
// CAssemblyTemplateGrid: extended Grid Control event
//------------------------------------------------------------------
void CAssemblyTemplateGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320341/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(FALSE);
   m_myCUGEdit.m_ctrl = this;	

}

void CAssemblyTemplateGrid::OnCharDown(UINT* vcKey,BOOL processed)
{
   // start editing when the user hits a character key on a cell in the grid.  
   // Pass that key to the edit control, so it doesn't get lost
   StartEdit(*vcKey); 
} 


int CAssemblyTemplateGrid::OnEditStart(int col, long row, CWnd **edit)
{
   *edit = &m_myCUGEdit;
   return true;
}

int CAssemblyTemplateGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	return TRUE;
}

int CAssemblyTemplateGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);
     
   return TRUE;
}

void CAssemblyTemplateGrid::OnSelectionChanged(int startCol,long startRow,int endCol,long endRow,int blockNum)
{
}

void CAssemblyTemplateGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   m_SelectRow = row;
   m_SelectCol = col;

   OnUpdateSelection(row);
}

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateFilenameDlg dialog
/////////////////////////////////////////////////////////////////////////////
CAssemblyTemplateFilenameDlg::CAssemblyTemplateFilenameDlg(CWnd* pParent /*=NULL*/)	
   : CDialog(CAssemblyTemplateFilenameDlg::IDD, pParent)
{
   m_TemplateFilename.Format("%s",TEMPLATE_FILENAME);
}

CAssemblyTemplateFilenameDlg::~CAssemblyTemplateFilenameDlg()
{
}

void CAssemblyTemplateFilenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAssemblyTemplateFilenameDlg)
   DDX_Text(pDX, IDC_TEMPLATE_FILENAME, m_TemplateFilename);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAssemblyTemplateFilenameDlg, CDialog)
  //{{AFX_DATA_MAP(CAssemblyTemplateFilenameDlg)
   ON_BN_CLICKED(IDOK, OnBnClickedSetFileName)
   //}}AFX_DATA_MAP
END_MESSAGE_MAP()

BOOL CAssemblyTemplateFilenameDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   UpdateData(FALSE);
   return TRUE;
}

void CAssemblyTemplateFilenameDlg::OnBnClickedSetFileName()
{
   UpdateData(TRUE);
   // TODO: Add your control notification handler code here
   if(m_TemplateFilename.IsEmpty())
   {
      formatMessageBox("Template filename must not be blank, please enter template filename.");
   }
   else
   {     
      OnOK();
   }
   UpdateData(FALSE);
}

///////////////////////////////////////////////////////////////////////////
// CTemplateFileManager
/////////////////////////////////////////////////////////////////////////////
void CTemplateFileManager::Init(CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid)
{
   m_AssemblyTempate = AssemblyTempate;
   m_ParameterGrid = ParameterGrid;
   m_cadDoc = (CCEtoODBApp*)AfxGetApp();
}  

int CTemplateFileManager::SaveTemplateFile(CString FilePath)
{
   CExtFileException* exception = NULL;
   CStdioFileWriteFormat writeFormat;

   bool retval = writeFormat.open(FilePath, exception);

   if( retval )
   {
      writeFormat.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
      writeFormat.writef("<Report Type=\"Insert\" Version=\"%s\" Writer=\"%s\">\n",DOC_VERSION, m_cadDoc->getCamCadTitle());
      writeFormat.pushHeader(QIndent);

      writeXml(writeFormat,*m_AssemblyTempate);
      writeXml(writeFormat,*m_ParameterGrid);

      writeFormat.popHeader();
      writeFormat.writef("</Report>\n");
   }

   writeFormat.close();
   return retval;
}

void CTemplateFileManager::writeXml(CStdioFileWriteFormat& writeFormat,const CAssemblyTemplat&   AssemblyTempate)
{
   writeFormat.writef("<General>\n");
   writeFormat.pushHeader(QIndent);
   for(int idx = 0; idx < m_AssemblyTempate->getSettingArray().GetCount(); idx++)
   {
      CGeneralSettings *settings = m_AssemblyTempate->getSettingArray().GetAt(idx);
      if(settings)
         writeFormat.writef("<%s Parameters=\"%s\" />\n",settings->getFieldName(), settings->ConvertParamToString());
   }

   writeFormat.popHeader();
   writeFormat.writef("</General>\n");
}

void CTemplateFileManager::writeXml(CStdioFileWriteFormat& writeFormat,const CAssemblyTemplateGrid&   arameterGrid)
{
   writeFormat.writef("<Data>\n");
   writeFormat.pushHeader(QIndent);
   for(int idx = 0; idx < m_ParameterGrid->GetNumberRows(); idx++)
   {
      CReportParameters rptParameter;
      m_ParameterGrid->GetRowData(idx, rptParameter);
      writeFormat.writef("<%s Parameters=\"%s\" FieldName=\"%s\" />\n",
         rptParameter.getCommand(),rptParameter.getOptions(),rptParameter.getFieldName());
   }

   writeFormat.popHeader();
   writeFormat.writef("</Data>\n");
}

int CTemplateFileManager::LoadTemplateFile(CString FilePath)
{
   CXMLDocument xmlDoc;
   if(!xmlDoc.LoadXMLFile(FilePath))
      return false;

   CXMLNodeList *xmlNode = xmlDoc.GetElementsByTagName("General");
   LoadXML(xmlNode,m_AssemblyTempate);  
   xmlNode = xmlDoc.GetElementsByTagName("Data");
   LoadXML(xmlNode, m_AssemblyTempate, m_ParameterGrid);  

   return true;

   
}

void CTemplateFileManager::LoadXML(CXMLNodeList *generalNode, CAssemblyTemplat *AssemblyTempate)
{
   if(!generalNode)
      return;

   generalNode->Reset();
   while (CXMLNode *styleListNode = generalNode->NextNode())
	{
		styleListNode->ResetChildList();
		while (CXMLNode *styleNode = styleListNode->NextChild())
		{
         CString fieldName;
         CGeneralSettings *settings = NULL;
         if(styleNode->GetName(fieldName) && m_AssemblyTempate->getSettingArray().getSettingMap().Lookup(fieldName,settings) && settings)
         {
            CString Parameters;
            if (styleNode->GetAttrValue("Parameters", Parameters))
               settings->ConvertStringToParamType(Parameters);
         }
      }//while
   }   
}

void CTemplateFileManager::LoadXML(CXMLNodeList *dataNode, CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid)
{
   if(!dataNode)
      return;

   ParameterGrid->DeleteAll();

   dataNode->Reset();
   while (CXMLNode *styleListNode = dataNode->NextNode())
	{
		styleListNode->ResetChildList();
		while (CXMLNode *styleNode = styleListNode->NextChild())
		{
         CString Command;
         CReportParameters *rptParameter = NULL;
         if(styleNode->GetName(Command) && AssemblyTempate->getParameterArray().Lookup(Command,rptParameter) && rptParameter)
         {
            ParameterGrid->AddRowData(rptParameter);
            int row = ParameterGrid->GetNumberRows() - 1;

            CString Parameters,FieldName;   
            if (styleNode->GetAttrValue("Parameters", Parameters))
               ParameterGrid->SetRowData(T_Parameter,row,Parameters);   
            if (styleNode->GetAttrValue("FieldName", FieldName) )
               ParameterGrid->SetRowData(T_FieldName,row,FieldName);   

         }
      }//while
   }   
}

bool CTemplateFileManager::CompareSavedFile(CString FilePath)
{
   CXMLDocument xmlDoc;
   if(!xmlDoc.LoadXMLFile(FilePath))
      return false;

   CXMLNodeList *xmlNode = xmlDoc.GetElementsByTagName("General");
   if(!CompareGeneral(xmlNode,m_AssemblyTempate))
      return false;
   
   xmlNode = xmlDoc.GetElementsByTagName("Data");
   if(!CompareData(xmlNode,m_AssemblyTempate, m_ParameterGrid))
      return false;

   return true;
}

bool CTemplateFileManager::CompareGeneral(CXMLNodeList *generalNode, CAssemblyTemplat *AssemblyTempate)
{
   if(!generalNode)
      return false;

   generalNode->Reset();
   while (CXMLNode *styleListNode = generalNode->NextNode())
	{
		styleListNode->ResetChildList();
		while (CXMLNode *styleNode = styleListNode->NextChild())
		{
         CString fieldName;
         CGeneralSettings *settings = NULL;
         if(!styleNode->GetName(fieldName) || !m_AssemblyTempate->getSettingArray().getSettingMap().Lookup(fieldName,settings) || !settings)
            return false;

         CString fileParameters;
         if (!styleNode->GetAttrValue("Parameters", fileParameters))
            return false;

         CString dlgParameters = settings->ConvertParamToString();
         if(dlgParameters.Compare(fileParameters))
            return false;
      }
   }
   return true;
}

bool CTemplateFileManager::CompareData(CXMLNodeList *dataNode, CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid)
{
   if(!dataNode)
      return false;

   dataNode->Reset();
   while (CXMLNode *styleListNode = dataNode->NextNode())
	{
		styleListNode->ResetChildList();
      if(styleListNode->GetChildNodeCount() != ParameterGrid->GetNumberRows())
         return false;
      
      int row = 0;
		while (CXMLNode *styleNode = styleListNode->NextChild())
		{
         CString Command;
         if(!styleNode->GetName(Command))
            return false;

         CReportParameters rptParameter;
         ParameterGrid->GetRowData(row, rptParameter);

         if(rptParameter.getCommand().Compare(Command))
            return false;

         CString Parameters,FieldName;   
         if (!styleNode->GetAttrValue("Parameters", Parameters) || rptParameter.getOptions().Compare(Parameters))
            return false;   
         if (!styleNode->GetAttrValue("FieldName", FieldName) || rptParameter.getFieldName().Compare(FieldName))
            return false;   

         row ++;

      }//while
   }   

   return true;
}

///////////////////////////////////////////////////////////////////////////
// CCustomTemplateDBManager
/////////////////////////////////////////////////////////////////////////////
void CCustomTemplateDBManager::Init(CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid)
{
   m_AssemblyTempate = AssemblyTempate;
   m_ParameterGrid = ParameterGrid;
   m_cadDoc = (CCEtoODBApp*)AfxGetApp();
   
   if(!m_database.IsConnected())
      m_database.Connect();
} 

int CCustomTemplateDBManager::getTemplateList(CStringArray& templateArray)
{
   int templateCnt = 0;
   CDBCustomAssemblyTemplateList *templateList = m_database.LookupCustomAssemblyTemplates("");
   if(templateList)
   {
      templateCnt = templateList->GetCount();
      for(int tmpIdx = 0; tmpIdx < templateCnt; tmpIdx++)
      {
         CDBCustomAssemblyTemplate *cdbSetting = templateList->GetAt(tmpIdx);
         if(cdbSetting) templateArray.Add(cdbSetting->getTemplateName());
      }

      delete templateList;
   }

   return templateCnt;
}

bool CCustomTemplateDBManager::deleteTemplate(CString TemplateName)
{
   if(m_database.IsConnected())
   {      
      CDBCustomMachineList *machineList = m_database.LookupCustomMachines("");
      if(machineList && machineList->containsTemplate(TemplateName)) 
      {
         formatMessageBox("Can't delete Template %s is used by machine!!", TemplateName);
         return false;
      }

      m_database.DeleteCustomAssemblyTemplate(TemplateName);
      m_database.DeleteCustomReportParameter(TemplateName);
      
      return true;
   }
   return false;
}

int CCustomTemplateDBManager::SaveTemplateFile(CString TemplateName)
{
   if(m_database.IsConnected())
   {      
      writeDatabse(TemplateName, m_AssemblyTempate);
      writeDatabse(TemplateName, m_ParameterGrid);
      return true;
   }

   return false;
}

void CCustomTemplateDBManager::writeDatabse(CString TemplateName, CAssemblyTemplat *AssemblyTempate)
{
   CDBCustomAssemblyTemplateList *templateList = m_database.LookupCustomAssemblyTemplates(TemplateName);
   if(templateList)
   {
      if(templateList->GetCount())
         m_database.DeleteCustomAssemblyTemplate(TemplateName);
      delete templateList;

      CStringArray settingArray;      
      settingArray.Add(TemplateName);
      for(int idx = 0; idx < AssemblyTempate->getSettingArray().GetCount(); idx++)
      {
         CGeneralSettings *settings = AssemblyTempate->getSettingArray().GetAt(idx);
         if(settings)
         {
            settingArray.Add(settings->ConvertParamToString());
         }
      }

      CDBCustomAssemblyTemplate cdbSettings(settingArray);
      m_database.SaveCustomAssemblyTemplate(&cdbSettings);

   }

}

void CCustomTemplateDBManager::writeDatabse(CString TemplateName, CAssemblyTemplateGrid* parameterGrid)
{
   CDBCustomReportParameterList *cdprptParamList = m_database.LookupCDBCustomReportParameter(TemplateName);
   if(cdprptParamList)
   {
      if(cdprptParamList->GetCount())
         m_database.DeleteCustomReportParameter(TemplateName);
      delete cdprptParamList;

      for(int idx = 0; idx < parameterGrid->GetNumberRows(); idx++)
      {
         CReportParameters rptParameter;
         m_ParameterGrid->GetRowData(idx, rptParameter);
         
         CDBCustomReportParameter cdbrptParam(TemplateName, idx, rptParameter.getCommand(),rptParameter.getOptions(),rptParameter.getFieldName());
         m_database.SaveCustomReportParameter(&cdbrptParam);
      }
   }
}

int CCustomTemplateDBManager::LoadTemplateFile(CString TemplateName)
{
   if(m_database.IsConnected())
   {      
      LoadDatabase(TemplateName, m_AssemblyTempate);
      LoadDatabase(TemplateName, m_AssemblyTempate, m_ParameterGrid);
      return true;
   }

   return false;
}

void CCustomTemplateDBManager::LoadDatabase(CString TemplateName, CAssemblyTemplat *AssemblyTempate)
{
   CDBCustomAssemblyTemplateList *templateList = m_database.LookupCustomAssemblyTemplates(TemplateName);
   if(templateList)
   {
      for(int tmpIdx = 0; tmpIdx < templateList->GetCount(); tmpIdx++)
      {
         CDBCustomAssemblyTemplate *cdbSetting = templateList->GetAt(tmpIdx);
         if(cdbSetting && AssemblyTempate->getSettingArray().GetCount() < cdbSetting->getTemplateSettings().GetCount())
         {
            for(int setIdx = 0; setIdx < AssemblyTempate->getSettingArray().GetCount(); setIdx ++)
            {
               CString setValue = cdbSetting->getTemplateSettings().GetAt(setIdx + 1);
               CGeneralSettings *settings = AssemblyTempate->getSettingArray().GetAt(setIdx);
               if(settings) settings->ConvertStringToParamType(setValue);               
            }//for
         }
      }//for

      delete templateList;
   }   
}

void CCustomTemplateDBManager::LoadDatabase(CString TemplateName, CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid)
{
   CDBCustomReportParameterList *cdprptParamList = m_database.LookupCDBCustomReportParameter(TemplateName);
   if(cdprptParamList)
   {
      ParameterGrid->DeleteAll();

      for(int idx = 0; idx < cdprptParamList->GetCount(); idx++)
      {
         CDBCustomReportParameter *cdprptParam = cdprptParamList->GetAt(idx);
         CReportParameters *rptParameter = NULL;
         if(cdprptParam && AssemblyTempate->getParameterArray().Lookup(cdprptParam->getCommand(),rptParameter) && rptParameter)
         {
            ParameterGrid->AddRowData(rptParameter);            
            int row = ParameterGrid->GetNumberRows() - 1;

            ParameterGrid->SetRowData(T_Parameter,row,cdprptParam->getParameter());
            ParameterGrid->SetRowData(T_FieldName,row,cdprptParam->getFieldName());   
         }
      }         
      delete cdprptParamList;
   }   
}

int CCustomTemplateDBManager::CompareSavedFile(CString TemplateName)
{
   if(m_database.IsConnected())
   {
      if(!CompareGeneral(TemplateName, m_AssemblyTempate))
         return false;
      if(!CompareData(TemplateName, m_ParameterGrid))
         return false;
   }

   return true;
}

bool CCustomTemplateDBManager::CompareGeneral(CString TemplateName, CAssemblyTemplat *AssemblyTempate)
{
   bool retVal = true;
   CDBCustomAssemblyTemplateList *templateList = m_database.LookupCustomAssemblyTemplates(TemplateName);
   if(templateList)
   {
      for(int tmpIdx = 0; tmpIdx < templateList->GetCount() && retVal; tmpIdx++)
      {
          CDBCustomAssemblyTemplate *cdbSetting = templateList->GetAt(tmpIdx);
          if(cdbSetting && AssemblyTempate->getSettingArray().GetCount() < cdbSetting->getTemplateSettings().GetCount())
          {
            for(int setIdx = 0; setIdx < AssemblyTempate->getSettingArray().GetCount(); setIdx ++)
            {
               CGeneralSettings *settings = AssemblyTempate->getSettingArray().GetAt(setIdx);
               CString dlgParameters = (settings)?settings->ConvertParamToString():"";
               
               if(dlgParameters.Compare(cdbSetting->getTemplateSettings().GetAt(setIdx + 1)))
               {
                  retVal = false;
                  break;
               }
            }
          }
      }

      delete templateList;
   }   
   return retVal;
}

bool CCustomTemplateDBManager::CompareData(CString TemplateName, CAssemblyTemplateGrid *ParameterGrid)
{
   bool retVal = true;

   CDBCustomReportParameterList *cdprptParamList = m_database.LookupCDBCustomReportParameter(TemplateName);
   if(cdprptParamList)
   {
      if(cdprptParamList->GetCount() != ParameterGrid->GetNumberRows())
         retVal = false;

      for(int idx = 0; idx < cdprptParamList->GetCount() && retVal; idx++)
      {
         CDBCustomReportParameter *cdprptParam = cdprptParamList->GetAt(idx);
         if(!cdprptParam)
         {
            retVal = false;  
            break;
         }

         CReportParameters rptParameter;
         ParameterGrid->GetRowData(idx, rptParameter);
         if(rptParameter.getCommand() != cdprptParam->getCommand()
            || rptParameter.getFieldName() != cdprptParam->getFieldName()
            || rptParameter.getOptions() != cdprptParam->getParameter()) 
         {
            retVal = false;  
            break;
         }
      }

      delete cdprptParamList;
   }   

   return retVal;
}

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplatesDlg 
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CAssemblyTemplatesDlg, CDialog)
CAssemblyTemplatesDlg::CAssemblyTemplatesDlg(TemplatePattern pattern, CCEtoODBDoc* doc, FormatStruct *format, CWnd* pParent /*=NULL*/)
	: CDialog(CAssemblyTemplatesDlg::IDD, pParent)
   , m_Pattern(pattern)
   , m_format(format)
{
   CCEtoODBView *view = (CCEtoODBView*)activeView;
   m_cadDoc = (doc)?doc:((view)?view->GetDocument():NULL);  
}

CAssemblyTemplatesDlg::~CAssemblyTemplatesDlg()
{
   delete m_attribMap;
}

void CAssemblyTemplatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
   //Text
   DDX_Text(pDX, IDC_CT_DELIMITER, m_delimiterOthers);   
   DDX_Text(pDX, IDC_STATIC_ORIGIN, m_AssemblyTempate.getSettingArray().GetAt(Option_ReportOrigin)->getParam());
   DDX_Text(pDX, IDC_EDIT_TOP_HEADERFILE, m_AssemblyTempate.getSettingArray().GetAt(Option_TopHeaderFilePath)->getParam());   
   DDX_Text(pDX, IDC_EDIT_BOT_HEADERFILE, m_AssemblyTempate.getSettingArray().GetAt(Option_BottomHeaderFilePath)->getParam());
   DDX_Text(pDX, IDC_TEMPLATE_FOLDER, m_FilePath);   
   
   //ComboBox
   DDX_Control(pDX, IDC_COMBO_DECIMALS, m_decimalCB);
   DDX_Control(pDX, IDC_COMBO_UNITS, m_unitsCB);
   DDX_Control(pDX, IDC_COMBO_SURFACE, m_surfaceCB);
   DDX_Control(pDX, IDC_COMBO_MIRROR, m_mirrorCB);
   DDX_Control(pDX, IDC_COMBO_EXC_KEYWROD, m_exckeywordCB);
   DDX_Control(pDX, IDC_COMBO_EXC_VALUE, m_excvalueCB);
   DDX_Control(pDX, IDC_COMBO_INC_KEYWROD, m_inckeywordCB);
   DDX_Control(pDX, IDC_COMBO_INC_VALUE, m_incvalueCB);
   DDX_Control(pDX, IDC_COMBO_DELIMITER, m_delimiterCB);
   
   //List
   DDX_Control(pDX, IDC_LIST_PARAMETERS, m_parameterList);
   DDX_Control(pDX, IDC_LIST_TEMPLATEFILES, m_fileList);
   DDX_Control(pDX, IDC_LIST_INSERTFILTER, m_insertList);

   //Check box
   DDX_Check(pDX, IDC_WRITE_HEADER, m_AssemblyTempate.getSettingArray().GetAt(Option_WriteHeader)->getIntParam());
   DDX_Check(pDX, IDC_EXPLODE_PANEL, m_AssemblyTempate.getSettingArray().GetAt(Option_ExplodePanel)->getIntParam());
   DDX_Check(pDX, IDC_CHECK_TOPHEADER, m_AssemblyTempate.getSettingArray().GetAt(Option_WriteTopHeader)->getIntParam());
   DDX_Check(pDX, IDC_CHECK_BOTHEADER, m_AssemblyTempate.getSettingArray().GetAt(Option_WriteBottomHeader)->getIntParam());

}


BEGIN_MESSAGE_MAP(CAssemblyTemplatesDlg, CDialog)
   ON_CBN_SELCHANGE(IDC_COMBO_EXC_KEYWROD, OnCbnSelchangeComboExcKeywrod)
   ON_CBN_SELCHANGE(IDC_COMBO_INC_KEYWROD, OnCbnSelchangeComboIncKeywrod)
   ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
   ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
   ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
   ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
   ON_BN_CLICKED(IDC_TEMPLATE_BTN1, OnBnClickedTemplateBtn1)
   ON_BN_CLICKED(IDC_TEMPLATE_BTN2, OnBnClickedTemplateBtn2)
   ON_BN_CLICKED(IDC_TEMPLATE_BTN3, OnBnClickedTemplateBtn3)
   ON_BN_CLICKED(IDC_TEMPLATE_BTN4, OnBnClickedTemplateBtn4)
   ON_CBN_SELCHANGE(IDC_COMBO_EXC_VALUE, OnCbnSelchangeComboExcValue)
   ON_CBN_SELCHANGE(IDC_COMBO_INC_VALUE, OnCbnSelchangeComboIncValue)
   ON_CBN_SELCHANGE(IDC_COMBO_DECIMALS, OnCbnSelchangeComboDecimals)
   ON_CBN_SELCHANGE(IDC_COMBO_UNITS, OnCbnSelchangeComboUnits)
   ON_CBN_SELCHANGE(IDC_COMBO_SURFACE, OnCbnSelchangeComboSurface)
   ON_CBN_SELCHANGE(IDC_COMBO_MIRROR, OnCbnSelchangeComboMirror)
   ON_LBN_SELCHANGE(IDC_LIST_TEMPLATEFILES, OnLbnSelchangeListTemplatefiles)
   ON_CBN_SELCHANGE(IDC_COMBO_DELIMITER, OnCbnSelchangeComboDelimiter)
   ON_BN_CLICKED(IDC_CHECK_BOTHEADER, OnBnClickedCheckBotheader)
   ON_BN_CLICKED(IDC_CHECK_TOPHEADER, OnBnClickedCheckTopheader)
   ON_BN_CLICKED(IDC_BUTTON_TOPFILE, OnBnClickedButtonTopfile)
   ON_BN_CLICKED(IDC_BUTTON_BOTFILE, OnBnClickedButtonBotfile)
   ON_WM_CLOSE()
   ON_LBN_SELCHANGE(IDC_LIST_INSERTFILTER, OnLbnSelchangeListInsertfilter)
END_MESSAGE_MAP()

BOOL CAssemblyTemplatesDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();  

   //Initial Dialog Pattern
   InitDialogPattern();

   //Initialize default options
   InitOptions();
   
   UpdateData(FALSE);
   return TRUE;
}

void CAssemblyTemplatesDlg::InitOptions()
{
   //get CAMCAD tittle
   m_AssemblyTempate.getSettingArray().GetAt(Option_ReportOrigin)->setParam(((CCEtoODBApp*)AfxGetApp())->getCamCadTitle());

   //Initialize Decimals 0-7
   char value[20];
   for(int idx = 0; idx < 7; idx++)
      m_decimalCB.AddString(_itoa(idx,value,10));
   m_decimalCB.SetCurSel(m_AssemblyTempate.getSettingArray().GetAt(Option_NumberOfDecimals)->getIntParam());

   // Initialize Units
   for(int idx = PageUnitsTagMin; idx < pageUnitsLast; idx++)
      m_unitsCB.AddString(PageUnitsTagToString(intToPageUnitsTag(idx)));
   m_unitsCB.SetCurSel(m_AssemblyTempate.getSettingArray().GetAt(Option_OutputUnits)->getIntParam());

   //Initialize Surface
   for(int idx = testSurfaceMIN; idx < testSurfaceMAX; idx++)
      m_surfaceCB.AddString(testSurfaceTagToString((ETestSurface)idx));
   m_surfaceCB.SetCurSel(m_AssemblyTempate.getSettingArray().GetAt(Option_WriteSurface)->getIntParam());

   //Initialize Mirror Coordinate
   for(int idx = 0; idx < m_AssemblyTempate.getMirrorCoordinateArray().GetCount(); idx++)
      m_mirrorCB.AddString(m_AssemblyTempate.getMirrorCoordinateArray().GetAt(idx));
   m_mirrorCB.SelectString(0,m_AssemblyTempate.getSettingArray().GetAt(Option_MirrorCoordinates)->getParam());

   //Initialize Delimiter
   for(int idx = 0; idx < m_AssemblyTempate.getDelimiterArray().GetCount(); idx++)
      m_delimiterCB.AddString(m_AssemblyTempate.getDelimiterArray().GetAt(idx));
   CString delimitercode = m_AssemblyTempate.getSettingArray().GetAt(Option_SeparationCharacter)->getParam();
   m_delimiterCB.SelectString(0,m_delConvert.delimiterCharToName(delimitercode));

   //Initialize Report Parameters
   for(POSITION paramPos = m_AssemblyTempate.getParameterArray().GetStartPosition();paramPos;paramPos)
   {
      CString command = "";
      CReportParameters *parameter = NULL;
      m_AssemblyTempate.getParameterArray().GetNextAssoc(paramPos,command,parameter);

      if(parameter && !command.IsEmpty()) m_parameterList.AddString(command);
   }
   
   //Initialize grid
   m_ParameterGrid.AttachGrid(this,IDC_STATIC_PARAMETERGRID);
   m_ParameterGrid.initGrid();

   //Initialize Attributes Section
   m_attribMap = new CTemplateKeywordMap(*m_cadDoc);
   AddKeyWordToComboBox();

   //Initialize insert list
   for(int option = Option_WriteBoards;  option < Option_Last; option++)
   {
      CGeneralSettings *generalSetting =  m_AssemblyTempate.getSettingArray().GetAt(option);
      if(generalSetting)
         m_insertList.AddString(generalSetting->getFieldName());
   }   

   //initialize File List Section
   m_databaseManager.Init(&m_AssemblyTempate,&m_ParameterGrid);
   m_databaseManager.getLibraryDatabaseName(m_FilePath);

   m_currentFileIndex = -1;  
   RefreshFileList();
   OnLoadTemplateFile();

}

void CAssemblyTemplatesDlg::InitDialogPattern()
{
   if(m_Pattern == TemplatePattern_Edit)
   {
      GetDlgItem(IDC_TEMPLATE_BTN1)->SetWindowText(QACTION_NEW);
      GetDlgItem(IDC_TEMPLATE_BTN2)->SetWindowText(QACTION_SAVE);
      GetDlgItem(IDC_TEMPLATE_BTN3)->SetWindowText(QACTION_DELETE);
      GetDlgItem(IDC_TEMPLATE_BTN4)->SetWindowText(QACTION_CLOSE);
      
      GetDlgItem(IDC_TEMPLATE_BTN3)->ShowWindow(true);
      GetDlgItem(IDC_TEMPLATE_BTN4)->ShowWindow(true);
   }
   else
   {
      GetDlgItem(IDC_TEMPLATE_BTN1)->SetWindowText(QACTION_EXPORT);
      GetDlgItem(IDC_TEMPLATE_BTN2)->SetWindowText(QACTION_EXIT);
      GetDlgItem(IDC_TEMPLATE_BTN3)->ShowWindow(false);
      GetDlgItem(IDC_TEMPLATE_BTN4)->ShowWindow(false);
   }
}

void CAssemblyTemplatesDlg::AddKeyWordToComboBox()
{
   BuildAttributeMap();

   for(POSITION attribPos = m_attribMap->GetStartPosition();attribPos;)
   {
      int keywordIdx = -1;
      CTemplateOutputAttrib *attributes = NULL;
      m_attribMap->GetNextAssoc(attribPos,keywordIdx, attributes);

      if(attributes && (keywordIdx == attributes->GetKeyword()))
      {
         // add keyword to comboBox
         if(attributes->getValuesMap().GetCount())
         {
            m_exckeywordCB.AddString(m_cadDoc->getKeyword(keywordIdx)->getCCKeyword());
            m_inckeywordCB.AddString(m_cadDoc->getKeyword(keywordIdx)->getCCKeyword());
         }
         else //remove keywords that do not have values
         {
            m_attribMap->RemoveKey(keywordIdx);
			   delete attributes;
			   attributes = NULL;            
         }
      }//if      
   }//for   
}

void CAssemblyTemplatesDlg::BuildAttributeMap()
{
   FileStruct *pcbFile = NULL;
   if((pcbFile = m_cadDoc->getFileList().GetFirstShown(blockTypePcb))) 
   {
      BuildAttributeMap(pcbFile);
   }
   else if((pcbFile = m_cadDoc->getFileList().GetFirstShown(blockTypePanel)))
   {
      for (POSITION pcbPos = pcbFile->getBlock()->getHeadDataInsertPosition(); pcbPos != NULL; pcbFile->getBlock()->getNextDataInsert(pcbPos))
	   {
		   DataStruct *pcbData = pcbFile->getBlock()->getAtData(pcbPos);
         if(pcbData && pcbData->getAttributes()) m_attribMap->Add(pcbData->getAttributes());

         InsertStruct *pcbInsert = pcbData->getInsert();
		   FileStruct *pFile = m_cadDoc->getFileList().FindByBlockNumber(pcbInsert->getBlockNumber());
         if (pFile) BuildAttributeMap(pFile);		   
      }
   }   
}

void CAssemblyTemplatesDlg::BuildAttributeMap(FileStruct *pfile)
{
   if(pfile && pfile->getBlock())
   {
      for(POSITION dataPos = pfile->getBlock()->getHeadDataInsertPosition(); dataPos; )
      {
         DataStruct *data = pfile->getBlock()->getNextDataInsert(dataPos);
         if(data && data->getAttributes()) m_attribMap->Add(data->getAttributes());
      }
   }
}

void CAssemblyTemplatesDlg::AddValuesToComboBox(CExtendComboBox &keywordCB, CExtendComboBox &valueCB)
{
   int currentSel = keywordCB.GetCurSel();
   if(currentSel > -1)  
   {
      CStringArray valueArray;
      CString keyWord;
   
      keywordCB.GetLBText(currentSel, keyWord);   
      m_attribMap->LookupValues(keyWord,valueArray);
      
      //Clear string
      valueCB.ClearAll();
   
      for(int valueIdx = 0; valueIdx < valueArray.GetCount(); valueIdx++)
         valueCB.AddString(valueArray.GetAt(valueIdx));
   }
}

void CAssemblyTemplatesDlg::RefreshFileList()
{
   m_fileList.ResetContent();

   CStringArray templateArray;
   m_databaseManager.getTemplateList(templateArray);
   for(int fIdx = 0; fIdx < templateArray.GetCount(); fIdx++)
      m_fileList.AddString(templateArray.GetAt(fIdx));

   //Select First File
   if(m_fileList.GetCount())
   {
      m_fileList.SetCurSel(0);
      m_fileList.GetText(0,m_FileName);
   }
}

bool CAssemblyTemplatesDlg::DeleteTemplate(CString TemplateName)
{
   return m_databaseManager.deleteTemplate(TemplateName);
}

void CAssemblyTemplatesDlg::GetComboBoxData(CExtendComboBox &srcComboBox,SettingTag tag)
{
   CString parameter = "";

   int curSelect = srcComboBox.GetCurSel();
   if(curSelect != -1)
       srcComboBox.GetLBText(curSelect, parameter);

   m_AssemblyTempate.getSettingArray().GetAt(tag)->ConvertStringToParamType(parameter);
}

void CAssemblyTemplatesDlg::SetComboBoxData(CExtendComboBox &srcComboBox, SettingTag tag)
{
   CString parameter = m_AssemblyTempate.getSettingArray().GetAt(tag)->ConvertParamToString();
   
   if(!parameter.IsEmpty())
      srcComboBox.SelectString(0,parameter);
   else
      srcComboBox.SetCurSel(-1);
}

void CAssemblyTemplatesDlg::GetDelimiter(CExtendComboBox &srcComboBox, SettingTag tag)
{
   CString parameter = "";

   int curSelect = srcComboBox.GetCurSel();
   if(curSelect != -1)
       srcComboBox.GetLBText(curSelect, parameter);

   if(!parameter.CompareNoCase(DEL_OTHERS))
      m_AssemblyTempate.getSettingArray().GetAt(tag)->setParam(m_delimiterOthers);
   else
   {
      DelimiterCode code = m_delConvert.delimiterNameToCode(parameter);
      m_AssemblyTempate.getSettingArray().GetAt(tag)->setParam(m_delConvert.delimiterCodeToString(code));
   }

}

void CAssemblyTemplatesDlg::SetDelimiter(CExtendComboBox &srcComboBox, SettingTag tag)
{
   CString parameter = m_AssemblyTempate.getSettingArray().GetAt(tag)->getParam();
   CString delimiterName = m_delConvert.delimiterCharToName(parameter);

   bool isOthers = false;
   if(!delimiterName.CompareNoCase(DEL_OTHERS))
   {
      m_delimiterOthers = parameter;
      isOthers = true;
   }

   srcComboBox.SelectString(0,delimiterName);
   (GetDlgItem(IDC_CT_DELIMITER))->ShowWindow(isOthers);
}

void CAssemblyTemplatesDlg::EnableHeaderFile(int itemID, SettingTag tag)
{
   bool chkHeader = m_AssemblyTempate.getSettingArray().GetAt(tag)->getYesNoParam();
   GetDlgItem(itemID)->EnableWindow(chkHeader);
}

void CAssemblyTemplatesDlg::UpdateComboBoxes(bool reverse)
{
   if(reverse)
   {
      GetComboBoxData(m_decimalCB,      Option_NumberOfDecimals);
      GetComboBoxData(m_unitsCB,        Option_OutputUnits);
      GetComboBoxData(m_surfaceCB,      Option_WriteSurface);
      GetComboBoxData(m_mirrorCB,       Option_MirrorCoordinates);
      GetComboBoxData(m_exckeywordCB,   Option_SuppressInsertAttributeKeyword);
      GetComboBoxData(m_excvalueCB,     Option_SuppressInsertAttributeValue);
      GetComboBoxData(m_inckeywordCB,   Option_IncludeInsertAttributeKeyword);
      GetComboBoxData(m_incvalueCB,     Option_IncludeInsertAttributeValue);
      GetDelimiter(m_delimiterCB,       Option_SeparationCharacter);
   }
   else
   {
      SetComboBoxData(m_decimalCB,      Option_NumberOfDecimals);
      SetComboBoxData(m_unitsCB,        Option_OutputUnits);
      SetComboBoxData(m_surfaceCB,      Option_WriteSurface);
      SetComboBoxData(m_mirrorCB,       Option_MirrorCoordinates);
      SetComboBoxData(m_exckeywordCB,   Option_SuppressInsertAttributeKeyword);
      SetComboBoxData(m_inckeywordCB,   Option_IncludeInsertAttributeKeyword);
      SetDelimiter(m_delimiterCB,       Option_SeparationCharacter);

      AddValuesToComboBox(m_exckeywordCB,m_excvalueCB);
      AddValuesToComboBox(m_inckeywordCB,m_incvalueCB);
      SetComboBoxData(m_excvalueCB,     Option_SuppressInsertAttributeValue);
      SetComboBoxData(m_incvalueCB,     Option_IncludeInsertAttributeValue);
   
      EnableHeaderFile(IDC_EDIT_TOP_HEADERFILE,Option_WriteTopHeader);
      EnableHeaderFile(IDC_EDIT_BOT_HEADERFILE,Option_WriteBottomHeader);
      EnableHeaderFile(IDC_BUTTON_TOPFILE,Option_WriteTopHeader);
      EnableHeaderFile(IDC_BUTTON_BOTFILE,Option_WriteBottomHeader);
   }
}

void CAssemblyTemplatesDlg::UpdateInsertList(bool reverse)
{
   int minCnt = min(m_insertList.GetCount(), Option_Last - Option_WriteBoards);
   for(int option = 0; option < minCnt; option++)
   {
      CGeneralSettings *generalSetting =  m_AssemblyTempate.getSettingArray().GetAt(option + Option_WriteBoards);
      if(generalSetting)
      {
         if(reverse)
            m_insertList.SetSel(option, (generalSetting->getIntParam())?TRUE:FALSE);
         else
            generalSetting->setIntParam(m_insertList.GetSel(option));
      }
   }   
}

void CAssemblyTemplatesDlg::ResetData()
{
   //Reset General Settings
   for(int idx = 0; idx < m_AssemblyTempate.getSettingArray().GetCount(); idx++)
   {
      CGeneralSettings *settings = m_AssemblyTempate.getSettingArray().GetAt(idx);
      if(settings) settings->ResetParameters();
   }

   //Reset Grid
   m_ParameterGrid.DeleteAll();
   
   //Reset ComboBox
   UpdateComboBoxes(false);   
}

ActionTag CAssemblyTemplatesDlg::getFileActionTag(CString Options)
{
   ActionTag action = Action_Unknown;

   if(!Options.CompareNoCase(QACTION_NEW))
      action = Action_New;
   else if(!Options.CompareNoCase(QACTION_SAVE))
      action = Action_Save;
   else if(!Options.CompareNoCase(QACTION_DELETE))
      action = Action_Delete;
   else if(!Options.CompareNoCase(QACTION_EXPORT))
      action = Action_Export;
   else if(!Options.CompareNoCase(QACTION_EXIT))
      action = Action_Exit;
   else if(!Options.CompareNoCase(QACTION_CLOSE))
      action = Action_Close;

   return action;
}

int CAssemblyTemplatesDlg::DoFileAction(CString Options)
{
   switch(getFileActionTag(Options))
   {
   case Action_New:
      OnNewTemplateFile();
      break;
   case Action_Save:
      OnSaveTemplateFile();
      break;
   case Action_Delete:
      OnDeleteTemplateFile();
      break;
   case Action_Close:
      OnClickedClose();
      break;
   case Action_Export:
      OnGenerateReport();
      break;
   case Action_Exit:
      OnClickedExit();
      break;
   }

   return true;
}

bool CAssemblyTemplatesDlg::OnNewFileName(CString &fileName)
{
   CAssemblyTemplateFilenameDlg filenameDlg;
   int retval = !IDOK;

   //Enter File Name
   while(1)
   {   
      retval = filenameDlg.DoModal();  
      if((retval == IDOK && m_fileList.FindString(0,filenameDlg.getTemplateFileName()) == -1)
         || retval == IDCANCEL)
         break;
      else
         formatMessageBox("Filename has existed. Please enter a new name");
   }

   if(retval == IDOK)
   {
      fileName = filenameDlg.getTemplateFileName();
      return true;
   }

   return false;
}

void CAssemblyTemplatesDlg::OnNewTemplateFile()
{        
   if(!OnAlertDataChanged())
      return;

   CString FileName;
   if(OnNewFileName(FileName))
   {
      //Reset Data
      ResetData();
      UpdateData(FALSE);
  
      m_databaseManager.SaveTemplateFile(FileName);
      RefreshFileList();   
      
      int selectIndex = m_fileList.FindString(0,FileName);
      if(selectIndex > -1)
      {
         m_fileList.SetCurSel(selectIndex);
         OnLoadTemplateFile();
         UpdateData(FALSE);
      }
   }
}

void CAssemblyTemplatesDlg::OnDeleteTemplateFile()
{
   int SeletcedIndex = m_fileList.GetCurSel();
   if(SeletcedIndex > -1)
   {
      CString FileName, FilePath;
      m_fileList.GetText(SeletcedIndex,FileName);

      if(formatMessageBox(MB_ICONQUESTION | MB_YESNO,
         "Do you want to delete %s ?",FileName) == IDYES
         && DeleteTemplate(FileName))
      {         
         m_fileList.DeleteString(SeletcedIndex);

         //Set next index
         if(!m_fileList.GetCount())
            SeletcedIndex = -1;
         else
            SeletcedIndex = (SeletcedIndex >= m_fileList.GetCount())?0:SeletcedIndex;
         
         m_fileList.SetCurSel(SeletcedIndex);
         if(SeletcedIndex > -1)
         {
            OnLoadTemplateFile();
            UpdateData(FALSE);
         }
         else
         {
            ResetData();
            UpdateData(FALSE);
         }
      }//if
   }

   UpdateCurrentFileIndex();
}

void CAssemblyTemplatesDlg::OnSaveTemplateFile()
{
   UpdateData(TRUE);
   
   //No file exist   
   if(!m_fileList.GetCount())
   {   
      CString FileName;
      if(OnNewFileName(FileName))
      {
         m_databaseManager.SaveTemplateFile(FileName);
         RefreshFileList();   
         m_fileList.SetCurSel(0);
      }
   }
   else 
   {
      UpdateComboBoxes(true);
      SaveTemplateFile(m_fileList.GetCurSel());
   }

   UpdateData(FALSE);
}

bool CAssemblyTemplatesDlg::SaveTemplateFile(int fileIndex)
{   
   int errorCode = OnVerifyData();
   if(errorCode)
   {
      showErrorMessage(errorCode);
      return false;
   }

   if(fileIndex > -1)
   {
      CString FileName; 
      m_fileList.GetText(fileIndex, FileName);
      if(!m_databaseManager.SaveTemplateFile(FileName))
      {
         formatMessageBox("Save File %s fail!!",FileName);
         return false;
      }
      
   }
   return true;
}

void CAssemblyTemplatesDlg::OnClickedClose()
{
   if(!OnAlertDataChanged())
      return;
   OnOK();
}

void CAssemblyTemplatesDlg::OnClickedExit()
{
   if(!OnAlertDataChanged())
      return;
   OnCancel();
}

void CAssemblyTemplatesDlg::OnLoadTemplateFile()
{
   int selectIndex = m_fileList.GetCurSel();
   if(selectIndex == -1 && m_fileList.GetCount())
      selectIndex = 0;

   LoadTemplateFile(selectIndex);
}

bool CAssemblyTemplatesDlg::LoadTemplateFile(int fileIndex)
{
   bool reval = true;
   if(fileIndex > -1)
   {
      CString FileName;
      m_fileList.GetText(fileIndex, FileName);
      if(!m_databaseManager.LoadTemplateFile(FileName))
      {
         formatMessageBox("Load File %s fail!!",FileName);
         reval = false;
      }
      else
      {
         UpdateComboBoxes(false);
         UpdateInsertList(true);
      }
   }
   UpdateCurrentFileIndex();

   return reval;
}

void CAssemblyTemplatesDlg::OnGenerateReport()
{
  UpdateData(TRUE);
  UpdateComboBoxes(true);

   int errCode = OnVerifyData();
   if(errCode)
      showErrorMessage(errCode);
   else 
   {
      //Write report
   }
}

int CAssemblyTemplatesDlg::OnVerifyData()
{
   CString paramValue ="";

   //Verify delimiter
   CString delimiter = m_AssemblyTempate.getSettingArray().GetAt(Option_SeparationCharacter)->getParam();

   if(delimiter.IsEmpty())
      return Err_Delimiter;

   //Verify Header file
   bool chkHeader = m_AssemblyTempate.getSettingArray().GetAt(Option_WriteTopHeader)->getYesNoParam();
   paramValue = m_AssemblyTempate.getSettingArray().GetAt(Option_TopHeaderFilePath)->getParam();
   if(chkHeader && paramValue.IsEmpty())
      return Err_HeaderFile;

   chkHeader = m_AssemblyTempate.getSettingArray().GetAt(Option_WriteBottomHeader)->getYesNoParam();
   paramValue = m_AssemblyTempate.getSettingArray().GetAt(Option_BottomHeaderFilePath)->getParam();
   if(chkHeader && paramValue.IsEmpty())
      return Err_HeaderFile;

   //Check Parameter missing
   for(int row = 0; row < m_ParameterGrid.GetNumberRows(); row++)
   {
      CReportParameters gridParameters;
      CReportParameters *parameters = NULL;
      
      m_ParameterGrid.GetRowData(row,gridParameters);
      if(m_AssemblyTempate.getParameterArray().Lookup(gridParameters.getCommand(),parameters) && parameters)
      {
         if(parameters->getOptions().Find("|") > -1)
         {
            if(gridParameters.getOptions().IsEmpty())
               return Err_Parameters_Start + parameters->getParamIndex();
            else if(parameters->getParamIndex() != Param_Attribute && parameters->getOptions().Find(gridParameters.getOptions()) < 0)
               return Err_Parameters_Start + parameters->getParamIndex();
         }//if
      }
   }//for

   return Err_OK;
}

void CAssemblyTemplatesDlg::showErrorMessage(int errorCode)
{
   CString ErrMessages;
   switch(errorCode)
   {
   case Err_Delimiter:
      ErrMessages = "Delimiter missing!!";
      break;
   case Err_HeaderFile:
      ErrMessages = "Header file missing!!";
      break;
   }
   
   if(errorCode >= Err_Parameters_Start &&  errorCode < Err_Parameters_End)
   {
      ErrMessages.Format("Parameter %s missing!!",DefaultReportValue[errorCode - Err_Parameters_Start].Command);
   }

   if(errorCode)
      formatMessageBox(ErrMessages);
}

bool CAssemblyTemplatesDlg::OnAlertDataChanged()
{
   if(m_currentFileIndex < 0)
      return true;
   
   UpdateData(TRUE);

   CString FileName;
   m_fileList.GetText(m_currentFileIndex,FileName);

   bool reval = true;

   UpdateComboBoxes(true);
   if ((!m_databaseManager.CompareSavedFile(FileName)) 
      && m_fileList.GetCount()
      && formatMessageBox(MB_ICONQUESTION | MB_YESNO,
      "Do you want to save changes to %s ?",FileName) == IDYES)
   {
      reval = SaveTemplateFile(m_currentFileIndex);
      if(!reval)
         m_fileList.SetCurSel(m_currentFileIndex);
   }

   return reval;
}

bool CAssemblyTemplatesDlg::OnSelectHeaderFile(CStringArray &fileNames)
{
   CFileDialog FileDialog(TRUE, QHDRFILEEXT, QHDRFILE, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, QHDRFILTER, NULL);

   fileNames.RemoveAll();
   if(FileDialog.DoModal() == IDOK)
   {  
      for (POSITION fileNamePos = FileDialog.GetStartPosition();fileNamePos;)
         fileNames.Add(FileDialog.GetNextPathName(fileNamePos));
      return (fileNames.GetCount() > 0);
   }
   return false;
}

// CAssemblyTemplatesDlg message handlers

void CAssemblyTemplatesDlg::OnClose()
{
   if(!OnAlertDataChanged())
      return;
   CDialog::OnClose();
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboDecimals()
{
   GetComboBoxData(m_decimalCB, Option_NumberOfDecimals);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboUnits()
{
   GetComboBoxData(m_unitsCB, Option_OutputUnits);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboSurface()
{
   GetComboBoxData(m_surfaceCB, Option_WriteSurface);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboMirror()
{
   GetComboBoxData(m_mirrorCB, Option_MirrorCoordinates);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboExcKeywrod()
{
   AddValuesToComboBox(m_exckeywordCB,m_excvalueCB);
   GetComboBoxData(m_exckeywordCB, Option_SuppressInsertAttributeKeyword);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboIncKeywrod()
{
   AddValuesToComboBox(m_inckeywordCB,m_incvalueCB);
   GetComboBoxData(m_inckeywordCB, Option_IncludeInsertAttributeKeyword);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboExcValue()
{
   GetComboBoxData(m_excvalueCB, Option_SuppressInsertAttributeValue);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboIncValue()
{
   GetComboBoxData(m_incvalueCB, Option_IncludeInsertAttributeValue);
}

void CAssemblyTemplatesDlg::OnBnClickedButtonAdd()
{
   int currentSel = m_parameterList.GetCurSel();
   if(currentSel > -1)
   {
      CString SelParameter;
      m_parameterList.GetText(currentSel,SelParameter);

      CReportParameters *parameters = NULL;
      if(!SelParameter.IsEmpty() && m_AssemblyTempate.getParameterArray().Lookup(SelParameter,parameters) && parameters)
         m_ParameterGrid.AddRowData(parameters);
   }
}

void CAssemblyTemplatesDlg::OnBnClickedButtonRemove()
{
   m_ParameterGrid.DeleteSelectedRow();

}

void CAssemblyTemplatesDlg::OnBnClickedButtonUp()
{
   m_ParameterGrid.MoveUpSelectedRow();

}

void CAssemblyTemplatesDlg::OnBnClickedButtonDown()
{
   m_ParameterGrid.MoveDownSelectedRow();

}

void CAssemblyTemplatesDlg::OnBnClickedTemplateBtn1()
{
   CString Caption;
   GetDlgItem(IDC_TEMPLATE_BTN1)->GetWindowText(Caption);
   DoFileAction(Caption);
}

void CAssemblyTemplatesDlg::OnBnClickedTemplateBtn2()
{
   CString Caption;
   GetDlgItem(IDC_TEMPLATE_BTN2)->GetWindowText(Caption);
   DoFileAction(Caption);
}

void CAssemblyTemplatesDlg::OnBnClickedTemplateBtn3()
{
   CString Caption;
   GetDlgItem(IDC_TEMPLATE_BTN3)->GetWindowText(Caption);
   DoFileAction(Caption);
}

void CAssemblyTemplatesDlg::OnBnClickedTemplateBtn4()
{
   CString Caption;
   GetDlgItem(IDC_TEMPLATE_BTN4)->GetWindowText(Caption);
   DoFileAction(Caption);
}

void CAssemblyTemplatesDlg::OnLbnSelchangeListTemplatefiles()
{
   OnAlertDataChanged();
   OnLoadTemplateFile();
   
   UpdateData(FALSE);
}

void CAssemblyTemplatesDlg::OnCbnSelchangeComboDelimiter()
{
   UpdateData(TRUE);

   CString delimiter = "";
   int curSelect = m_delimiterCB.GetCurSel();
   if(curSelect != -1)
      m_delimiterCB.GetLBText(curSelect, delimiter);

   (GetDlgItem(IDC_CT_DELIMITER))->ShowWindow((!delimiter.Compare(DEL_OTHERS)));
   GetDelimiter(m_delimiterCB,       Option_SeparationCharacter);

   UpdateData(FALSE);

}

void CAssemblyTemplatesDlg::OnBnClickedCheckTopheader()
{
   UpdateData(TRUE);

   EnableHeaderFile(IDC_EDIT_TOP_HEADERFILE,Option_WriteTopHeader);
   EnableHeaderFile(IDC_BUTTON_TOPFILE,Option_WriteTopHeader);

}

void CAssemblyTemplatesDlg::OnBnClickedCheckBotheader()
{
   UpdateData(TRUE);

   EnableHeaderFile(IDC_EDIT_BOT_HEADERFILE,Option_WriteBottomHeader);
   EnableHeaderFile(IDC_BUTTON_BOTFILE,Option_WriteBottomHeader);
}

void CAssemblyTemplatesDlg::OnBnClickedButtonTopfile()
{
   CStringArray FileNames;
   if(OnSelectHeaderFile(FileNames))
      GetDlgItem(IDC_EDIT_TOP_HEADERFILE)->SetWindowText(FileNames.GetAt(0));
   
}

void CAssemblyTemplatesDlg::OnBnClickedButtonBotfile()
{
   CStringArray FileNames;

   if(OnSelectHeaderFile(FileNames))
      GetDlgItem(IDC_EDIT_BOT_HEADERFILE)->SetWindowText(FileNames.GetAt(0));
}

void CAssemblyTemplatesDlg::OnLbnSelchangeListInsertfilter()
{
   /*int minCnt = min(m_insertList.GetCount(), Option_Last - Option_WriteBoards);
   for(int option = 0; option < minCnt; option++)
   {
      CGeneralSettings *generalSetting = m_AssemblyTempate.getSettingArray().GetAt(option + Option_WriteBoards);
      if(generalSetting)
         generalSetting->setIntParam((m_insertList.GetSel(option)));
   }*/

   UpdateInsertList(false);
}

/////////////////////////////////////////////////////////////////////////////
// CCEtoODBApp
/////////////////////////////////////////////////////////////////////////////
void CCEtoODBApp::OnEditCustomTemplate()
{
   CAssemblyTemplatesDlg assemblyTemplateDlg(TemplatePattern_Edit);
   assemblyTemplateDlg.DoModal();
}

void CCEtoODBApp::OnSetCustomTemplateDirectory()
{
   CAssemblyTemplateDirectoryDlg AssemblyTemplateDirDlg;
   if(AssemblyTemplateDirDlg.DoModal() == IDOK)
   {
      AssemblyTemplateDirDlg.SetTemplateDirectory();
   }
  
}


