
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2003. All Rights Reserved.
*/

#if !defined(__Variant_h__)
#define __Variant_h__

//#include "flexgrid.h"
#include "Entity.h"
#include "FlexGridLib.h"  // Use the CFlexGridStatic class from it
//#include "afxcmn.h"
//#include "afxwin.h"
#include "DcaVariant.h"

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CVariantUpdateDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CVariantUpdateDlg : public CResizingDialog
{
	DECLARE_DYNAMIC(CVariantUpdateDlg)

public:
	CVariantUpdateDlg(CCEtoODBDoc* doc, FileStruct* file, CWnd* pParent = NULL);   // standard constructor
	virtual ~CVariantUpdateDlg();

// Dialog Data
	enum { IDD = IDD_UPDATE_VARIANT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   void loadListBox();

private:
	CCEtoODBDoc* m_pDoc;
	FileStruct* m_pFile;	
   CListBox m_cmbUpdateVariantList;
   CString UpdateVariantName;
public:
   afx_msg void OnBnClickedOk();
   afx_msg void OnBnClickedCancel();
   CString getVariantName(){return UpdateVariantName;}
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CVariantNavigatorDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CVariantNavigatorDlg : public CResizingDialog
{
public:
	CVariantNavigatorDlg(CCEtoODBDoc* doc, FileStruct* file, CWnd* pParent = NULL);   // standard constructor
	~CVariantNavigatorDlg();
	enum { IDD = IDD_VARIANT_NAVIGATOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual CString GetDialogProfileEntry() { return CString("VariantNavigatorDialog"); }
	DECLARE_MESSAGE_MAP()

private:
	CCEtoODBDoc* m_pDoc;
	FileStruct* m_pFile;	
	CVariant* m_pCurVarDataVariant;
	CVariant* m_pActBrdDataVariant;
	CStringArray m_visibleHeadingArray;

   CFlexGridStatic m_ocxStaCurVarData;
   CFlexGridStatic m_ocxStaActBrdData;
	CComboBox m_cmbVariantList;

	void initGrids();
	void loadListBox();
	void enableButtons();
	void loadVisibleHeadingMap();
   void makeLoadedAttribExplicit();
	void loadVariant(CVariant* variant, CExtendedFlexGrid *variantGrid);
	void saveVariant(CVariant *variant, CExtendedFlexGrid *variantGrid);
   CVariant* createVariant(CString errMessageCaption, CExtendedFlexGrid *variantGrid);
	CVariant* updateVariant(const CString& variantName, CExtendedFlexGrid *variantGrid);

public:
	virtual BOOL OnInitDialog();

   CCamCadData& getCamCadData() const;

	afx_msg void OnCbnSelchangeVariantList();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedSaveAs();
	afx_msg void OnBnClickedSetAsDefault();
	afx_msg void OnBnClickedCompare();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedRefreshBrdData();
	afx_msg void OnBnClickedCopyToVariantWindow();
	afx_msg void OnBnClickedCancel();

	DECLARE_EVENTSINK_MAP()
	void CellChangedCurDataGrid(long Row, long Col);
	void EnterCellCurDataGrid();
	void ClickCurDataGrid();
	void MouseDownBrdDataGrid();
	void MouseDownCurDataGrid();
   afx_msg void OnBnClickedUpdate();
};


class CVariantSelItem : public CSelItem
{
public:
	CVariantSelItem(CString name = "", CVariant *variant = NULL, BOOL selected = FALSE):CSelItem(name, selected)
	{
		m_pVariant = variant;
	}
	~CVariantSelItem()
	{
		m_pVariant = NULL;
	}

	CVariant *GetVariant() { return m_pVariant; }

private:
	CVariant *m_pVariant;
};


#endif