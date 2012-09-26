// $Header: /CAMCAD/4.4/LyrType.h 13    10/11/04 11:47a Alvin $

#if !defined(AFX_LYRTYPE_H__A70B9643_2AB4_11D2_BA40_0080ADB36DBB__INCLUDED_)
#define AFX_LYRTYPE_H__A70B9643_2AB4_11D2_BA40_0080ADB36DBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ccdoc.h"
#include "ResizingDialog.h"
#include "UltimateGrid.h"

#define LT_LAYERTYPEDOC		"LAYERTYPEDOC"
#define LT_COLORSET			"COLORSET"
#define LT_LAYERTYPE			"LAYERTYPE"

//////////////////////////////////////////////////////////////////////////////////
// Ultimate Grid based LayerTypes Grid
//

class CLayerTypeGrid : public CDDBaseGrid
{
private:
   int m_expandedRowHeight;

public:
    CLayerTypeGrid();
    ~CLayerTypeGrid() { }

    void SetExpandedRowHeight(int h)      { m_expandedRowHeight = h; }
    int GetExpandedRowHeight()            { return m_expandedRowHeight; }

    void ExpandLayerGroup(int groupNameRowIndx);
    void CollapseLayerGroup(int groupNameRowIndx);

    void SetRowCellsToCheckBox(int row);

    void OnDClicked(int col,long row, RECT *rect,POINT *point,BOOL processed);
    void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);

    virtual void OnSetup();
};

//////////////////////////////////////////////////////////////////////////////////
// CLayerTypeDlg dialog
class CLayerTypeDlg : public CResizingDialog
{
	//DECLARE_DYNAMIC(CLayerTypeDlg)

public:
	CLayerTypeDlg(CCEtoODBDoc *doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayerTypeDlg();

// Dialog Data
	enum { IDD = IDD_LAYERTYPES };

private:
	CCEtoODBDoc *pDoc;
	CComboBox m_layerSets;
   CLayerTypeGrid m_layerTypeGrid;

	bool layerSetNameChanged;

	void fillColorSetNameColumnTitles();
   void fillColorSetsComboBox();
	void fillLayerTypesInGrid();
	void fillLayerColorsInGrid();

	void showColorSet(int colorSet);

	void applyChanges();
   virtual CString GetDialogProfileEntry() { return "LayerTypeDlg"; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeLayersets();
	afx_msg void OnBnClickedCollapseAll();
	afx_msg void OnBnClickedExpandAll();
	afx_msg void OnBnClickedRename();
	afx_msg void OnBnClickedCopyfrom();
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LYRTYPE_H__A70B9643_2AB4_11D2_BA40_0080ADB36DBB__INCLUDED_)
