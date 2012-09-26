// $Header: /CAMCAD/4.5/read_wrt/BomCSV.h 17    6/20/06 4:50p Moustafa Noureddine $

#pragma once

#include "afxwin.h"
#include "FlexGridLib.h"

// CBOMCSVImportDlg dialog

class CBOMCSVImportDlg : public CDialog
{
//	DECLARE_DYNAMIC(CBOMCSVImportDlg)

private:
	CFlexGridStatic m_fgBom;
	CComboBox m_cboBomImport;
	CButton m_chkBomLoadedFlag;
	CButton m_cmdBomImport;
	CButton m_cmdCancelBomCSV;
	CEdit m_defaultPN;

	CString m_fileName;
	CCEtoODBDoc *m_Doc;
	FormatStruct *m_format;
	FileStruct *m_fileStruct;
   CApplyBOMTemplateDlg *m_dbgTemplate;

	char m_delimiter;
	int m_refDesCol;
	int m_ignoreLines;
	
	int m_headerStartsLine;
	int m_RowNumber;
   int m_TemplateColumnLen;
	
	void initGrid();
	void populateGrid();
   void populateGridbyTemplate();
	void add_GridRow(CString line);
   void add_GridRow(CString line,ColumnAttributeArray& columnparamlist);
	int  set_refDesCol(int col);
	void set_CompsLoaded(bool loaded);
	void importFromGrid();
	void refreshRefDesCol();
	void set_ComponentNotLoadedDataColorOverride(CCEtoODBDoc *doc, FileStruct *file);
   void ClearAttributes(BlockStruct *block, CExtendedFlexGrid *fgBom);

	void setdefaultPNValueToRegistry(CString defaultValue);
	CString getdefaultPNValueFromRegistry();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

public:
	CBOMCSVImportDlg(CString fileName , CCEtoODBDoc *Doc, FormatStruct *format, char delimiter = ',', int ignoreLines = 0, int headerStarts = 1, int m_RowNumber = 0, CApplyBOMTemplateDlg *dlgtemplate = NULL, CWnd* pParent = NULL);   // standard constructor
	CBOMCSVImportDlg(CString fileName , CCEtoODBDoc *Doc, FormatStruct *format, CApplyBOMTemplateDlg *dlgtemplate, CWnd* pParent = NULL);   // standard constructor
   virtual ~CBOMCSVImportDlg();

// Dialog Data
	enum { IDD = IDD_dlgBOMCSV };

	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	
	void fgBom_Click();
	afx_msg void cboBomImportRefDes_OnCbnSelchange();
	afx_msg void cmdBomImport_OnBnClicked(); 
	void fgBom_KeyDown(short* KeyCode, short Shift);
	void fgBom_KeyDownEdit(long Row, long Col, short* KeyCode, short Shift);
	afx_msg void OnBnClickedBomcancel();
	void fgBom_ValidateEdit(long Row, long Col, BOOL* Cancel);
	
	CButton m_EnableDefault;
	afx_msg void OnBnClickedChkenabledefault();
	afx_msg void OnBnClickedchkbomloadedflag();
};

/////////////////////////////////////////////////////////////////////////////
// CSelectBOMDataTypeDlg dialog
/////////////////////////////////////////////////////////////////////////////
enum OPT_BOMDATATYPE{
   OPT_DELIMITED,
   OPT_TEMPLATE
}; //OPT_BOMDATATYPE  
class CSelectBOMDataTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectBOMDataTypeDlg)
public:
	CSelectBOMDataTypeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectBOMDataTypeDlg();
	enum { IDD = IDD_SELECT_BOM_DATATYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_optDelimited;

public:
   BOOL getDataTypeOption(){return m_optDelimited;}

public:
	afx_msg void OnBnClickedRadioBOMDataType();
	afx_msg void OnBnClickedOk();

};


