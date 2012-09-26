// $Header: /CAMCAD/5.0/AsemblyTemplate.cpp    03/20/09 2:48a Sharry Huang $
// AsemblyTemplate.h

#if ! defined (__AssemblyTemplate_h__)
#define __AssemblyTemplate_h__

#pragma once

#include "resource.h"
#include "afxwin.h"
#include "CCEtoODB.h"
#include "Dcadftcommon.h"
#include "CamcadDatabase.h"
#include "xmldomwrapper.h"

#undef __AFXOLE_H__
#include "DataDoctorDialog.h"

//Parameter Grid Header
#define  QCTCOMMAND     "Command"
#define  QCTPARAMETER   "Parameter"
#define  QCTFIELDNAME   "Field Name"

//Parameter String
#define  QPARAM_REFDES     "RefDes"
#define  QPARAM_XLOC       "Xloc"
#define  QPARAM_YLOC       "Yloc"
#define  QPARAM_ROTATION   "Rotation"
#define  QPARAM_SURFACE    "Surface"
#define  QPARAM_TECHNOLOGY "Technology"
#define  QPARAM_GEOMETRY   "Geometry"
#define  QPARAM_PARTNUMBER "Partnumber"
#define  QPARAM_DEVICE     "Device"
#define  QPARAM_TYPE       "Type"
#define  QPARAM_VALUE      "Value"
#define  QPARAM_TOLERANCE  "Tolerance"
#define  QPARAM_ATTRIBUTE  "Attribute"

//File Actions
#define  QACTION_NEW          "New"
#define  QACTION_SAVE         "Save"
#define  QACTION_DELETE       "Delete"
#define  QACTION_EXPORT       "Export"
#define  QACTION_EXIT         "Exit"
#define  QACTION_CLOSE        "Close"

//CSV Writer
#define QCSVFILEEXT     "CSV"
#define QCSVFILE        "Custom Assembly"
#define QCSVFILTER      "CustomAssemblyReport Write (*.csv)|*.CSV|All Files (*.*)|*.*||"

//XML content
#define QIndent "  "

//File
#define  DOC_VERSION                         "7.1"  
#define  KEY_SOFTWARE                        "Software\\"
#define  KEY_CAMCADSETTING                   "\\CAMCAD\\Settings"
#define  KEY_CUSTOMTEMPLATE_DIR              "AssemblyTemplateDir"
#define  DEFAULT_CUSTOMTEMPLATE_FOLDER       "C:\\MentorGraphics"
#define  TEMPLATE_FILENAME                   "Template"
#define  TEMPLATE_EXTNAME                    ".rpt"
#define  QHDRFILEEXT       "hdr"
#define  QHDRFILE          "*.hdr"
#define  QHDRFILTER   "Header File (*.hdr)|*.HDR|All Files (*.*)|*.*||"

//Delimiter
#define DEL_TAB         "Tab(s)"
#define DEL_SPACE       "Space(s)"
#define DEL_COMMA       "Comma"
#define DEL_PIPE        "Pipe"
#define DEL_SEMICOLON   "Semi Colon"
#define DEL_OTHERS      "Others"
#define DEL_UNKNOWN     "UnKnown"


///////////////////////////////////////////////////////////////////////////
// CDelimiterConverter
/////////////////////////////////////////////////////////////////////////////
enum DelimiterCode
{
   DELCODE_OTHERS = 'O',
   DELCODE_COMMA = ',',
   DELCODE_SPACE = ' ',
   DELCODE_PIPE = '|',
   DELCODE_SEMICOLON = ';',
   DELCODE_TAB = '\t',
};
class CDelimiterConverter
{
private:
   CTypedMapIntToPtrContainer<char*> m_delimiterTable;
   CTypedMapStringToPtrContainer<DelimiterCode*> m_delimiterCode;

public:
   CDelimiterConverter();
   ~CDelimiterConverter();
   
   CString delimiterCharToName(CString delimiter);
   CString delimiterCodeToName(DelimiterCode code);
   DelimiterCode delimiterNameToCode(CString delimiter);
   CString delimiterCodeToString(DelimiterCode code);

};

///////////////////////////////////////////////////////////////////////////
// CReportParameters
/////////////////////////////////////////////////////////////////////////////
//Update DefaultReportValue when ParameterTag is modified
enum ParameterTag
{
   Param_Min = 0,
   Param_RefDes = Param_Min,
   Param_Xloc,
   Param_Yloc,
   Param_Rotation,
   Param_Surface,
   Param_Technology,
   Param_Geometry,
   Param_Partnumber,
   Param_Device,
   Param_Type,
   Param_Value,
   Param_Tolerance,
   Param_Attribute,
   Param_Last,
};

typedef struct 
{
   ParameterTag paramIndex;
   CString Command;
   CString Options;
}ReportInitValue;

#define DEFAULTREPORT_SIZE sizeof(DefaultReportValue)/sizeof(ReportInitValue)
static ReportInitValue DefaultReportValue[] =
{
   {Param_RefDes,"RefDes",""},
   {Param_Xloc,"Xloc","Insert|Centroid|PadCenters|PadExtents|CompExtents|OutlineExtents|RealPart|"},
   {Param_Yloc,"Yloc","Insert|Centroid|PadCenters|PadExtents|CompExtents|OutlineExtents|RealPart|"},
   {Param_Rotation,"Rotation","Insert|Centroid|RealPart|"},
   {Param_Surface,"Surface","String|Number|"},
   {Param_Technology,"Technology",""},
   {Param_Geometry,"Geometry",""},
   {Param_Partnumber,"Partnumber",""},
   {Param_Device,"Device",""},
   {Param_Type,"Type",""},
   {Param_Value,"Value","String|Number|Units|"},
   {Param_Tolerance,"Tolerance","String|+|-|"},
   {Param_Attribute,"Attribute","Custom_Attributes|"}
};

class CReportParameters
{
private:
   CString  m_Command;
   CString  m_FieldName;
   CString  m_Options;
   ParameterTag  m_ParamIdx;
   int      m_sizeofRepValue;
public:
   CReportParameters();
   CReportParameters(ParameterTag paramIdx);
   ~CReportParameters();

   CString getParameterOptions(ParameterTag paramIdx);
   CString getParameterCommand(ParameterTag paramIdx);

   CString& getCommand(){return m_Command;}
   CString& getFieldName(){return m_FieldName;}
   CString& getOptions(){return m_Options;}
   ParameterTag getParamIndex() {return m_ParamIdx;}
};

class CReportParametersArray: public CTypedPtrMap<CMapStringToPtr,CString,CReportParameters*>
{
public:
   ~CReportParametersArray();
};

///////////////////////////////////////////////////////////////////////////
// CGeneralSettings
/////////////////////////////////////////////////////////////////////////////
//Update DefaultSettingValue when SettingTag is modified
enum SettingTag
{
   Option_Min = 0,
   Option_SeparationCharacter = Option_Min, 
   Option_WriteHeader, 
   Option_ExplodePanel ,
   Option_WriteTopHeader,
   Option_WriteBottomHeader,
   Option_TopHeaderFilePath,
   Option_BottomHeaderFilePath,
   Option_ReportOrigin , 
   Option_OutputUnits , 
   Option_NumberOfDecimals, 
   Option_MirrorCoordinates, 
   Option_WriteSurface, 
   Option_SuppressInsertAttributeKeyword, 
   Option_SuppressInsertAttributeValue, 
   Option_IncludeInsertAttributeKeyword, 
   Option_IncludeInsertAttributeValue, 
   //---------Insert Filter Start--------------
   // Please put filter options last
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
   //---------Insert Filter End--------------
   Option_Last
};

enum ParamTypeTag
{
   ParamTypeBoolean,
   ParamTypeInteger,
   ParamTypeString,
   ParamTypeSurface,
   ParamTypeUnit,
};

typedef struct 
{
   SettingTag paramIndex;
   CString FieldName;
   CString InitValue;
   ParamTypeTag paramType;
}GeneralInitValue;

class CGeneralSettings
{
private:
   SettingTag  m_settingIndex;
   CString     m_fieldName;
   void        *m_Parameter;
   ParamTypeTag m_paramType;
   CString     m_InitValue;
   int         m_sizeofSetValue;

public:
   CGeneralSettings(SettingTag settingIdx);
   ~CGeneralSettings();
   void InitParameters();
   void ResetParameters();
   
   int& getIntParam(){return *((int*)m_Parameter);}
   CString& getParam(){return *((CString*)m_Parameter);}
   bool getYesNoParam(){return (*(int*)m_Parameter)?true:false;}
   CString getYestNoParamStr(){ return (*(int*)m_Parameter)?"True":"False";}
   int getCharParam(int i){return (*((CString*)m_Parameter)).GetAt(i);}
   CString ConvertParamToString();
   PageUnitsTag getUnitParam(){ return intToPageUnitsTag(*((int*)m_Parameter));}
   ETestSurface getSurfaceParam() { return *((ETestSurface*)m_Parameter);}

   void setParam(CString val){*((CString*)m_Parameter) = val;}
   void setIntParam(int val){*((int*)m_Parameter) = val;}
   void setYesNoParam(int val){*((int*)m_Parameter) = val;}
   void setUnitsParam(PageUnitsTag units){*((PageUnitsTag*)m_Parameter) = units;}
   void setUnitsParam(CString units);
   void setSurfaceParam(ETestSurface surface){*((ETestSurface*)m_Parameter) = surface;}
   void setSurfaceParam(CString surface);
   void setDelimiterParam(DelimiterCode delimiter){*((DelimiterCode*)m_Parameter) = delimiter;}
   void ConvertStringToParamType(CString Value);

   CString& getFieldName(){return m_fieldName;}
   bool getFieldDefaultValue(SettingTag settingIdx, CString &FileName, CString &defaltValue, ParamTypeTag& ParamType);

};

/////////////////////////////////////////////////////////////////////////////
// CGeneralSettingsArray 
/////////////////////////////////////////////////////////////////////////////
class CGeneralSettingsArray : public CTypedPtrArrayContainer<CGeneralSettings*> 
{
private:
   CTypedMapStringToPtrContainer<CGeneralSettings*> m_SettingMap;

public:
   CGeneralSettingsArray();
   ~CGeneralSettingsArray();

   CTypedMapStringToPtrContainer<CGeneralSettings*> &getSettingMap(){return m_SettingMap;}
   void Set(CGeneralSettings *genSetting);

   PageUnitsTag getUnitValue(int idx){return GetAt(idx)->getUnitParam();}
   ETestSurface getSurfaceValue(int idx){return GetAt(idx)->getSurfaceParam();}
   int getIntValue(int idx){return GetAt(idx)->getIntParam();}
   CString getValue(int idx){ return GetAt(idx)->getParam();}
   bool getYesNoValue(int idx){return GetAt(idx)->getYesNoParam();}
};

///////////////////////////////////////////////////////////////////////////
// CAssemblyTemplat
/////////////////////////////////////////////////////////////////////////////
class CAssemblyTemplat
{
private:
   
   //data
   CReportParametersArray m_ParameterArray;  
   CGeneralSettingsArray m_generalSettings;

   //variable
   CStringArray m_MirrorCoordinateArray;
   CStringArray m_DelimiterArray;

public:
   CAssemblyTemplat();
   ~CAssemblyTemplat();
   
   CReportParametersArray &getParameterArray(){return m_ParameterArray;}
   CGeneralSettingsArray &getSettingArray(){return m_generalSettings;}
   CStringArray &getMirrorCoordinateArray(){return m_MirrorCoordinateArray;}
   CStringArray &getDelimiterArray(){return m_DelimiterArray;}

private:
   void InitDataParameter();
   void InitGeneralSettings();
   void InitMirrorCoordinateArray();
   void InitDelimiterArray();
};

////////////////////////////////////////////////////////////
// CTemplateOutputAttrib
////////////////////////////////////////////////////////////
class CTemplateOutputAttrib
{
public:
	CTemplateOutputAttrib(CCEtoODBDoc& camCadDoc);
	~CTemplateOutputAttrib();

private:
	CCamCadDatabase m_camCadDatabase; 
	int m_attribKeyword;
	CMapStringToString m_valuesMap;

public:
	int GetKeyword() const;
	bool HasValue(CAttributes** attributes);
	void SetKeyword(const int keyword);
	void AddValue(const CString value);
	void SetDefaultAttribAndValue();
   bool IsValid() {return ( (m_attribKeyword > -1) && (!m_valuesMap.IsEmpty()) );}
   CMapStringToString& getValuesMap(){ return m_valuesMap;}
};

class CTemplateKeywordMap : public CTypedMapIntToPtrContainer<CTemplateOutputAttrib*>
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase; 

public:
   CTemplateKeywordMap(CCEtoODBDoc& camCadDoc);
   ~CTemplateKeywordMap();

   void Add(int keyWord, CString value);
   void Add(CAttributes *attributes);
   bool LookupValues(CString Keyword, CStringArray &ValueList);
};

///////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateDirectory dialog
/////////////////////////////////////////////////////////////////////////////
class CAssemblyTemplateDirectory
{
private:
   CWinApp *m_cadDoc;
   CString m_TemplatePath;

public:
   CAssemblyTemplateDirectory(){m_cadDoc = AfxGetApp();}
   void setCamcadDoc(CCEtoODBApp *cadDoc){m_cadDoc = cadDoc;}

   bool FindTemplateRegValue(CString &Value);
   bool FindStringValueByKey(CString keyname, CString RegName,CString &strValue);
   void SetTemplateDirectory(CString Value);
   
   CString &getTemplateFolder(){return m_TemplatePath;}
};

///////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateDirectoryDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CAssemblyTemplateDirectoryDlg : public CDialog
{
public:
	CAssemblyTemplateDirectoryDlg(CWnd* pParent = NULL);   // standard constructor
   ~CAssemblyTemplateDirectoryDlg();
   //{{AFX_DATA(CAssemblyTemplateDirectoryDlg)
	enum { IDD = IDD_BOM_TEMPLATE_PATH };
   CString m_TemplatePath;
   //}}AFX_DATA

protected:
   //{{AFX_DATA(CAssemblyTemplateDirectoryDlg)
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   //}}AFX_DATA
	DECLARE_MESSAGE_MAP()

private:
   CAssemblyTemplateDirectory m_Templatedir;

public:
   void SetTemplateDirectory(){m_Templatedir.SetTemplateDirectory(m_TemplatePath);}
public:
   afx_msg void OnBrowseTemplatePath();
};

///////////////////////////////////////////////////////////////////////////
// CExtendComboBox
/////////////////////////////////////////////////////////////////////////////
class CExtendComboBox : public CComboBox
{
public:
   void ClearAll();

};

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateGrid 
/////////////////////////////////////////////////////////////////////////////
enum FieldTag
{
   T_Command,
   T_Parameter,
   T_FieldName,
};/*FieldTag*/

class CAssemblyTemplateGrid : public CDDBaseGrid
{
private:
   CStringArray   m_gridHeader;
   CUGEdit m_myCUGEdit;    
   int   m_ColumnSize;
   int   m_SelectRow;
   int   m_SelectCol;

private:
   int OnEditVerifyDigitalCell(int col, long row,CWnd *edit,UINT *vcKey);
   int OnEditVerifyAttributeCell(int col, long row,CWnd *edit);

public:
   CAssemblyTemplateGrid();
   ~CAssemblyTemplateGrid();

   //data
   void AddRowData(CReportParameters *rptParameter);
   void SetRowData(int col, int row, CString data);
   void GetRowData(int row, CReportParameters &rptParameter);
   void GetRowData(int col, int row, CString& data);
   void DeleteSelectedRow();
   void DeleteAll();
   void MoveUpSelectedRow();
   void MoveDownSelectedRow();
   void ExchaneRowStringData(int Col, int srcRow, int dstRow);
   void ExchaneRowDropList(int Col, int srcRow, int dstRow) ;

   //control event
   void OnSetup();
   void OnCharDown(UINT* vcKey,BOOL processed);
   int OnEditStart(int col, long row, CWnd **edit);
   int OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey);
   int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
   void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);
   void OnUpdateSelection(int curRow);
   void OnSelectionChanged(int startCol,long startRow,int endCol,long endRow,int blockNum);

   //edit grid content
   void initGrid();
   void SetCellOption(int col, int option);
   void QuickSetDropList(int col, int row, CString options);
   void SetCellReadOnly(int col, int row, bool readonly);

   CString getSelectValue(int col, int row);
};

/////////////////////////////////////////////////////////////////////////////
// CAssemblyTemplateFilenameDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CAssemblyTemplateFilenameDlg : public CDialog
{
public:
	CAssemblyTemplateFilenameDlg(CWnd* pParent = NULL);   // standard constructor
   ~CAssemblyTemplateFilenameDlg();
   //{{AFX_DATA(CEditBOMTemplateDlg)
	enum { IDD = IDD_BOM_TEMPLATE_NEWNAME };
   CString m_TemplateFilename;
   //}}AFX_DATA

protected:
   //{{AFX_DATA(CEditBOMTemplateDlg)
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   //}}AFX_DATA
	DECLARE_MESSAGE_MAP()

public:
   CString &getTemplateFileName(){return m_TemplateFilename;}
public:
   afx_msg void OnBnClickedSetFileName();
};

///////////////////////////////////////////////////////////////////////////
// CTemplateFileManager
/////////////////////////////////////////////////////////////////////////////
class CTemplateFileManager
{
private:
   CAssemblyTemplat        *m_AssemblyTempate;
   CAssemblyTemplateGrid   *m_ParameterGrid;
   CCEtoODBApp              *m_cadDoc;
public:
   void Init(CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid);

   int SaveTemplateFile(CString FilePath);
   int LoadTemplateFile(CString FilePath);
   bool CompareSavedFile(CString FilePath);

private:
   void writeXml(CStdioFileWriteFormat& writeFormat,const CAssemblyTemplat&   AssemblyTempate);
   void writeXml(CStdioFileWriteFormat& writeFormat,const CAssemblyTemplateGrid&   arameterGrid);
   void LoadXML(CXMLNodeList *generalNode, CAssemblyTemplat *AssemblyTempate);
   void LoadXML(CXMLNodeList *dataNode, CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid);
   bool CompareGeneral(CXMLNodeList *generalNode, CAssemblyTemplat *AssemblyTempate);
   bool CompareData(CXMLNodeList *dataNode, CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid);

};

///////////////////////////////////////////////////////////////////////////
// CCustomTemplateDBManager
/////////////////////////////////////////////////////////////////////////////
class CCustomTemplateDBManager
{
private:
   CAssemblyTemplat        *m_AssemblyTempate;
   CAssemblyTemplateGrid   *m_ParameterGrid;
   CDBInterface            m_database;
   CCEtoODBApp              *m_cadDoc;

public:
   void Init(CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid *ParameterGrid);

   int SaveTemplateFile(CString TemplateName);
   int LoadTemplateFile(CString TemplateName);
   int CompareSavedFile(CString TemplateName);
   int getTemplateList(CStringArray& templateList);
   bool deleteTemplate(CString TemplateName);
   void getLibraryDatabaseName(CString& library){ library = m_database.GetLibraryDatabaseName();}

private:
   void writeDatabse(CString TemplateName, CAssemblyTemplat *AssemblyTempate);
   void writeDatabse(CString TemplateName, CAssemblyTemplateGrid* parameterGrid);
   void LoadDatabase(CString TemplateName, CAssemblyTemplat *AssemblyTempate);
   void LoadDatabase(CString TemplateName, CAssemblyTemplat *AssemblyTempate, CAssemblyTemplateGrid* parameterGrid);
   bool CompareGeneral(CString TemplateName, CAssemblyTemplat *AssemblyTempate);
   bool CompareData(CString TemplateName, CAssemblyTemplateGrid *ParameterGrid);
};

///////////////////////////////////////////////////////////////////////////
// CAssemblyTemplatesDlg dialog
/////////////////////////////////////////////////////////////////////////////
enum TemplatePattern
{
   TemplatePattern_Edit,
   TemplatePattern_Apply
};

enum ActionTag
{
   Action_Unknown,
   Action_New,
   Action_Save,
   Action_Delete,
   Action_Export,
   Action_Exit,
   Action_Close,
};

enum TemplateErrorCode
{
   Err_OK,
   Err_Delimiter,
   Err_HeaderFile,
   Err_Parameters_Start,
   Err_Parameters_End = (Err_Parameters_Start + Param_Last),
   Err_End
};

class CAssemblyTemplatesDlg : public CDialog
{
	DECLARE_DYNAMIC(CAssemblyTemplatesDlg)

private:
   CCEtoODBDoc           *m_cadDoc;
   CTemplateKeywordMap  *m_attribMap;
   FormatStruct         *m_format;

   TemplatePattern      m_Pattern;
   CAssemblyTemplat     m_AssemblyTempate;
   CTemplateFileManager m_fileManager;
   CDelimiterConverter  m_delConvert;
   CCustomTemplateDBManager m_databaseManager;

   CExtendComboBox         m_decimalCB;
   CExtendComboBox         m_unitsCB;
   CExtendComboBox         m_surfaceCB;
   CExtendComboBox         m_mirrorCB;
   CExtendComboBox         m_exckeywordCB;
   CExtendComboBox         m_excvalueCB;
   CExtendComboBox         m_inckeywordCB;
   CExtendComboBox         m_incvalueCB;
   CExtendComboBox         m_delimiterCB;
   CListBox                m_fileList;   
   CListBox                m_parameterList;
   CListBox                m_insertList;   
   CAssemblyTemplateGrid   m_ParameterGrid;
   CAssemblyTemplateDirectory m_Templatedir;
   
   CString  m_FilePath;
   CString  m_FileName;
   CString  m_delimiterOthers;
   int m_currentFileIndex;

public:
   CAssemblyTemplatesDlg(TemplatePattern pattern, CCEtoODBDoc* doc = NULL, FormatStruct *format = NULL, CWnd* pParent = NULL);
	virtual ~CAssemblyTemplatesDlg();
   
// Dialog Data
	enum { IDD = IDD_CUSTOM_TEMPLATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
   void InitDialogPattern();
   void InitOptions();
   void AddKeyWordToComboBox();
   void BuildAttributeMap();
   void BuildAttributeMap(FileStruct *pfile);
   void ResetData();
   void showErrorMessage(int errorCode);

   void AddValuesToComboBox(CExtendComboBox &keywordCB, CExtendComboBox &valueCB);
   void RefreshFileList();
   bool SaveTemplateFile(int fileIndex);
   bool LoadTemplateFile(int fileIndex);
   bool DeleteTemplate(CString TemplateName);

   void GetComboBoxData(CExtendComboBox &srcComboBox, SettingTag tag);
   void SetComboBoxData(CExtendComboBox &srcComboBox, SettingTag tag);
   void UpdateComboBoxes(bool reverse);
   void UpdateInsertList(bool reverse);
   void UpdateCurrentFileIndex(){m_currentFileIndex = m_fileList.GetCurSel();}
   void GetDelimiter(CExtendComboBox &srcComboBox, SettingTag tag);
   void SetDelimiter(CExtendComboBox &srcComboBox, SettingTag tag);
   void EnableHeaderFile(int itemID, SettingTag tag);

   ActionTag getFileActionTag(CString Options);
   int DoFileAction(CString Options);
   CString getFullFileName(CString FileName) { return m_FilePath + "\\" + FileName + ((FileName.Find(TEMPLATE_EXTNAME) > -1)?"":TEMPLATE_EXTNAME);}

   void OnNewTemplateFile();
   void OnSaveTemplateFile();
   void OnDeleteTemplateFile();
   void OnClickedClose();
   void OnLoadTemplateFile();
   void OnGenerateReport();
   void OnClickedExit();

   int  OnVerifyData();
   bool OnNewFileName(CString &fileName);
   bool OnAlertDataChanged();
   bool OnSelectHeaderFile(CStringArray &fileNames);

public:
   CAssemblyTemplat  &getAssemblyTemplate(){return m_AssemblyTempate;}
   CAssemblyTemplateGrid   &getParamGrid(){return m_ParameterGrid;}

public:
   afx_msg void OnCbnSelchangeComboExcKeywrod();
   afx_msg void OnCbnSelchangeComboIncKeywrod();
   afx_msg void OnBnClickedButtonAdd();
   afx_msg void OnBnClickedButtonRemove();
   afx_msg void OnBnClickedButtonUp();
   afx_msg void OnBnClickedButtonDown();
   afx_msg void OnBnClickedTemplateBtn1();
   afx_msg void OnBnClickedTemplateBtn2();
   afx_msg void OnBnClickedTemplateBtn3();
   afx_msg void OnBnClickedTemplateBtn4();
   afx_msg void OnCbnSelchangeComboExcValue();
   afx_msg void OnCbnSelchangeComboIncValue();
   afx_msg void OnCbnSelchangeComboDecimals();
   afx_msg void OnCbnSelchangeComboUnits();
   afx_msg void OnCbnSelchangeComboSurface();
   afx_msg void OnCbnSelchangeComboMirror();
   afx_msg void OnLbnSelchangeListTemplatefiles();
   afx_msg void OnCbnSelchangeComboDelimiter();
   afx_msg void OnBnClickedCheckBotheader();
   afx_msg void OnBnClickedCheckTopheader();
   afx_msg void OnBnClickedButtonTopfile();
   afx_msg void OnBnClickedButtonBotfile();
   afx_msg void OnClose();
   afx_msg void OnLbnSelchangeListInsertfilter();
};

#endif /*__AssemblyTemplate_h__*/