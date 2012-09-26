// $Header: /CAMCAD/4.5/NormalizeValuesDlg.h 6     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#include "flexgrid.h"

typedef long GRID_CNT_TYPE;

class CCEtoODBDoc;

// NormalizeValuesDlg dialog
class NormalizeValuesDlg : public CDialog
{
	DECLARE_DYNAMIC(NormalizeValuesDlg)

public:
	NormalizeValuesDlg(CWnd* pParent = NULL);   // standard constructor
	NormalizeValuesDlg(CCEtoODBDoc *document, CWnd* pParent = NULL);   
	virtual ~NormalizeValuesDlg();
	DECLARE_EVENTSINK_MAP()
	void MouseUpVsflex(short Button, short Shift, float X, float Y);
	
// Dialog Data
	enum { IDD = IDD_NORMALIZE_VALUES };

private:
	CFlexGrid m_flexGrid;
	CCEtoODBDoc *doc;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void setItemToGrid(GRID_CNT_TYPE row, GRID_CNT_TYPE col, CString itemValue);
	void fillGrid();
	virtual BOOL OnInitDialog();
	


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
