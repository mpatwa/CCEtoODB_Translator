// $Header:

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// BOMTemplate.h

#if ! defined (__BOMTemplate_h__)
#define __BOMTemplate_h__

#pragma once

#include "resource.h"
#include "afxwin.h"
#include "CCEtoODB.h"

#undef __AFXOLE_H__
#include "DataDoctorDialog.h"

//Attribute Header 
#define QSELECT      "Select" 
#define QCOLNUMS     "Column Number" 
#define QATTRNAME    "Attribute Name"
#define QSTARTAT     "Starts At"
#define QENDAT       "Ends At"
#define QTEMPLATENAME   "CPL Template"

//Template Keys
#define TKEY_TEMPLATENAME     ".TemplateName"
#define TTAG_STARTLINE        ".startLine"
#define TTAG_COMMENTCHAR      ".CommentChar"
#define TTAG_DELIMITER        ".Delimiter"
#define TTAG_COLUMN           ".Column"

//Delimiter
#define DEL_TAB         "Tab(s)"
#define DEL_SPACE       "Space(s)"
#define DEL_COMMA       "Comma"
#define DEL_PIPE        "Pipe"
#define DEL_SEMICOLON   "Semi Colon"
#define DEL_COLUMN      "Column"
#define DEL_OTHERS      "Others"

enum DELCODE
{
   DELCODE_UNKNOWN = 0,
   DELCODE_COLUMN,
   DELCODE_COMMA = ',',
   DELCODE_SPACE = ' ',
   DELCODE_PIPE = '|',
   DELCODE_SEMICOLON = ';',
   DELCODE_TAB = '\t',
};

//File
#define KEY_SOFTWARE             "Software\\"
#define KEY_VBPROGRAM            "VB and VBA Program Settings\\"
#define KEY_BOMEXPLORER          "BOM Explorer\\BOMExplorer"
#define KEY_CAMCADSETTING        "\\CAMCAD\\Settings"
#define KEY_TEMPLATEDIR          "TemplateDir"
#define DEFAULT_TEMPLATE_FOLDER  "C:\\MentorGraphics"
#define TEMPLATE_FILENAME        "Template"
#define TEMPLATE_EXTNAME         ".bet"

enum FieldTag
{
   T_Select,
   T_ColumnIndex,
   T_AttributeName,
   T_StartsAt,
   T_EndsAt,
};/*FieldTag*/

enum ColumnType
{
   T_NoRange,
   T_HasRange
};/*ColumnType*/

enum TemplateErrorCode
{
   Err_OK,
   Err_AttrName_Value,
   Err_AttrName_Duplicate,
   Err_ColumnNo_Value,
   Err_ColumnNo_Duplicate,
   Err_Range_Value,
   Err_TemplateName_Value,
   Err_CommentChar_Value,
   Err_Delimiter_Value,
   Err_StartLine_Value,
};/*TemplateErrorCode*/

typedef struct             
{
   CString  AttributeName;
   int      columnNum;
   int      startcharIdx;
   int      endcharIdx;
} ColumnAttribute;
typedef CTypedPtrArray<CPtrArray, ColumnAttribute*> ColumnAttributeArray;

/////////////////////////////////////////////////////////////////////////////
// CBOMTemplateData
/////////////////////////////////////////////////////////////////////////////
class CBOMTemplateData
{
public:
   CBOMTemplateData();
   ~CBOMTemplateData();

private:
   CString  m_TemplateFolderPath;
   CString  m_TemplateFileName;
   CString  m_TemplateName;
   CString  m_AttributeName;
   CString  m_Delimiter;
   CString  m_CommontChar;

   int      m_HeadLine;
   int      m_StartLine;
   int      m_StartCharIdx;
   int      m_EndCharIdx;
   int      m_ColumnNum;

   CMapStringToInt  m_DelimiterTable;
   ColumnAttributeArray m_columnAttribute; 


public:
   //set parameter
   void setStartCharIndex(int val){m_StartCharIdx = val;}
   void setEndCharIndex(int val){m_EndCharIdx = val;}
   void setStartLine(int val){m_StartLine = val;}
   void setHeadLine(int val){m_HeadLine = val;}
   void setColumnNumber(int val){ m_ColumnNum = val;}
   void setAttributeName(CString val){m_AttributeName = val;}
   void setgetTemplateName(CString val){m_TemplateName = val;}
   void setDelimiter(CString val){ m_Delimiter = val;}
   void setTemplateFolderPath(CString val){m_TemplateFolderPath = val;}
   void setTemplateFileName(CString val){m_TemplateFileName = val;}

   //get parameter
   char getDelimiterChar();
   char getCommontChar(){return m_CommontChar.GetAt(0);}
   char DelimiterStringToChar(CString DelimiterStr);

   int& getHeadLine(){return m_HeadLine;}
   int& getStartLine(){return m_StartLine;}
   int& getStartCharIndex(){return m_StartCharIdx;}
   int& getEndCharIndex(){return m_EndCharIdx;}
   int& getColumnNumber(){return m_ColumnNum;}
   
   CString& getCommontChars(){return m_CommontChar;}
   CString& getDelimiter(){return m_Delimiter;}
   CString& getAttributeName(){return m_AttributeName;}
   CString& getTemplateName(){return m_TemplateName;}
   CString& getTemplateFolderPath(){return m_TemplateFolderPath;}
   CString& getTemplateFileName(){return m_TemplateFileName;}

   ColumnAttributeArray& getColumnAttriButeList() {return m_columnAttribute;}
   CMapStringToInt& getDelimiterTable() {return m_DelimiterTable;}

   bool LoadTemplateFile(CString &TemplateFileName);
   bool SaveTemplateFile(CString &TemplateFileName);
   
   int AddNewAttribute(ColumnAttribute newparam);
   void ClearAttributes();
   void ResetTemplateData();
   void SortedAdd(ColumnAttributeArray &columnAttribute, ColumnAttribute *columnVal);
   
   int ValidateHeaderData();
   int ValidateInputAttribute(int exclrow, int columnNo, CString AttrName, int startIdx, int endIdx);
   void ErrorMessage(int errorCode);
   
   //operator
   CBOMTemplateData &operator=(CBOMTemplateData &other);
   bool Compare(CBOMTemplateData &other);
   bool CompareAttributes(ColumnAttributeArray &other);

};

///////////////////////////////////////////////////////////////////////////
// CBOMTemplateDirectory dialog
/////////////////////////////////////////////////////////////////////////////
class CBOMTemplateDirectory
{
private:
   CWinApp *m_cadDoc;
   CString m_TemplatePath;

public:
   CBOMTemplateDirectory(){m_cadDoc = AfxGetApp();}
   void setCamcadDoc(CCEtoODBApp *cadDoc){m_cadDoc = cadDoc;}

   bool FindTemplateRegValue(CString &Value);
   bool FindStringValueByKey(CString keyname, CString RegName,CString &strValue);
   void SetTemplateDirectory(CString Value);
   
   CString &getTemplateFolder(){return m_TemplatePath;}
};

/////////////////////////////////////////////////////////////////////////////
// CBOMTemplateGrid dialog
/////////////////////////////////////////////////////////////////////////////
class CBOMTemplateGrid : public CDDBaseGrid
{
private:
   ColumnAttributeArray *m_columnAttribute; 
   CStringArray   m_gridHeader;
   CUGEdit m_myCUGEdit;    
   int m_ColumnSize;
   int m_RowSize;
   ColumnType m_DefalutType;

private:
   int OnEditVerifyDigitalCell(int col, long row,CWnd *edit,UINT *vcKey);
   int OnEditVerifyAttributeCell(int col, long row,CWnd *edit);
public:
   CBOMTemplateGrid();
   ~CBOMTemplateGrid();

   //control event
   void OnSetup();
   void OnCharDown(UINT* vcKey,BOOL processed);
   int OnEditStart(int col, long row, CWnd **edit);
   int OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey);
   int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);
   
   //edit grid content
   void initGrid(ColumnType colStype);
   void ReColSize(ColumnType colStype);
   void SetCellOption(int col, int option);
   int getSelectValue(int row);

   void AddRowData(ColumnAttribute *attribute);
   void AddRowData(int TemplateColumnIdx, CString AttrName);
   void AddRowData(int TemplateColumnIdx, CString AttrName, int StartAt, int EndAt);
   bool GetRowData(int row, ColumnAttribute *attribute);
   bool GetRowData(int row, int &TemplateColumnIdx, CString &AttrName);
   bool GetRowData(int row, int &TemplateColumnIdx, CString &AttrName, int &StartAt, int &EndAt);
   void DeleteRowBySelectValue();
   void ClearAllData();

   int ValidateInputAttribute(int exclrow, int columnNo, CString AttrName, int startIdx, int endIdx);
   int ValidateInputAttribute(int exclrow, ColumnAttribute validattr);
   bool CompareAttributes(ColumnAttributeArray &others);

};


/////////////////////////////////////////////////////////////////////////////
// CApplyBOMTemplateDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CApplyBOMTemplateDlg : public CDialog
{
public:
	CApplyBOMTemplateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CApplyBOMTemplateDlg();
   //{{AFX_DATA(CApplyBOMTemplateDlg)
	enum { IDD = IDD_BOM_TEMPLATE_APPLY };
   CComboBox m_columnCB;
   CListBox m_fileList;
   //}}AFX_DATA

protected:
   //{{AFX_DATA(CApplyBOMTemplateDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   //}}AFX_DATA
	DECLARE_MESSAGE_MAP()

protected:
   CBOMTemplateData m_templateData;
   CBOMTemplateDirectory m_Templatedir;

protected:
   void FillColumnNumbers();
   void FillTemplateFileList();
   void LoadTemplateFile(int index);
   void ClearColumnNumber();
   void EnableAttributeRange();

public:
   char getDelimiterChar(){return m_templateData.getDelimiterChar();}
   int getStartLine(){return m_templateData.getStartLine();}
   CString& getDelimiter(){return m_templateData.getDelimiter();}
   ColumnAttributeArray& getColumnAttriButeList() {return m_templateData.getColumnAttriButeList();}
   bool isAttributColumn() {return (m_templateData.getDelimiter().Find(DEL_COLUMN) != -1)?true:false;}

private:
   void OnVerifyEmptyValue(int uid, int errorCode, CString restoreVal);

public:
   afx_msg void OnCbSelchangeColumnNumber();
   afx_msg void OnCbSetColumnNumber();
   afx_msg void onListSelectTemplateFile();
   afx_msg void OnBntCheckNext();
   afx_msg void OnEnKillfocusEditStartline();
   afx_msg void OnEnKillfocusEditStartcharidx();
   afx_msg void OnEnKillfocusEditEndcharidx();
   afx_msg void OnEnKillfocusEditAttributename();
};

////////////////////////////////////////////////////////////////////////////
// CEditBOMTemplateDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CEditBOMTemplateDlg : public CDialog
{
public:
	CEditBOMTemplateDlg(CWnd* pParent = NULL);   // standard constructor
   ~CEditBOMTemplateDlg();
   //{{AFX_DATA(CEditBOMTemplateDlg)
	enum { IDD = IDD_BOM_TEMPLATE_Edit };
   CListBox m_fileList;
   CComboBox m_delimiterCB;
   //}}AFX_DATA

protected:
   //{{AFX_DATA(CEditBOMTemplateDlg)
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   //}}AFX_DATA
	DECLARE_MESSAGE_MAP()

protected:
   CBOMTemplateData        m_templateData;
   CBOMTemplateData        m_backupData;
   CBOMTemplateGrid        m_BOMTemplateGrid;
   CBOMTemplateDirectory   m_Templatedir;
   int                     m_currentFileIndex;

private:
   void  FillTemplateFile();
   void  RefreshFileList();
   void  InitializeDelimiters();
   void  LoadTemplateFile(int index);
   bool  SaveTemplateFile(int index);      
   int   ValidateInputAttribute(CBOMTemplateData *tdata);

   bool OnAlertChangedFile();
   void OnSetDelimiter(CString delimiter);
   void OnVerifyEmptyValue(int uid, int errorCode, CString restoreVal);

public:
   afx_msg void OnSelectTemplateFile();
   afx_msg void onSelectDelimiter();
   afx_msg void OnBnClickedAddTemplateColumns();
   afx_msg void OnBnClickedRemoveTemplateColumns();
   afx_msg void OnBnClickedSaveTemplateFile();
   afx_msg void OnBnClickedDeleteTemplateFile();
   afx_msg void OnBnClickedNewTemplateFile();
   afx_msg void OnClickedClose();
   afx_msg void OnErrSpace();
   afx_msg void OnStartLineUpdate();
   afx_msg void OnStartLineKillFocus();
   afx_msg void OnCommentCharKillFocus();
   afx_msg void OnTemplateNameKillFocus();
   afx_msg void OnDelimiterOthersKillFocus();
};


///////////////////////////////////////////////////////////////////////////
// CBOMTemplateDirectoryDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CBOMTemplateDirectoryDlg : public CDialog
{
public:
	CBOMTemplateDirectoryDlg(CWnd* pParent = NULL);   // standard constructor
   ~CBOMTemplateDirectoryDlg();
   //{{AFX_DATA(CEditBOMTemplateDlg)
	enum { IDD = IDD_BOM_TEMPLATE_PATH };
   CString m_TemplatePath;
   //}}AFX_DATA

protected:
   //{{AFX_DATA(CEditBOMTemplateDlg)
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   //}}AFX_DATA
	DECLARE_MESSAGE_MAP()

private:
   CBOMTemplateDirectory m_Templatedir;

public:
   void SetTemplateDirectory(){m_Templatedir.SetTemplateDirectory(m_TemplatePath);}
public:
   afx_msg void OnBrowseTemplatePath();
};


/////////////////////////////////////////////////////////////////////////////
// CNewBOMTemplateFilenameDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CNewBOMTemplateFilenameDlg : public CDialog
{
public:
	CNewBOMTemplateFilenameDlg(CWnd* pParent = NULL);   // standard constructor
   ~CNewBOMTemplateFilenameDlg();
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

#endif /*__BOMTemplate_h__*/
