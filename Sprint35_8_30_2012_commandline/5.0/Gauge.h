// $Header: /CAMCAD/4.4/Gauge.h 10    3/29/04 1:48p Lynn Phung $

// CG: This file was added by the Progress Dialog component

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

#pragma once

#include "resource.h"

class CProgressDlg : public CDialog
{
// Construction / Destruction
public:
    CProgressDlg(CString Caption = "", BOOL showGauge = TRUE);   // standard constructor
    ~CProgressDlg();

    BOOL Create(CWnd *pParent=NULL);

    // Progress Dialog manipulation
    void SetStatus(LPCTSTR lpszMessage);
    void SetRange(int nLower,int nUpper);
    void SetRange32(long nLower,long nUpper);
    int  SetStep(long nStep);
    int  SetPos(long nPos);
    int  OffsetPos(long nPos);
    int  StepIt();
	 CString GetPercent()		{ UpdateData(TRUE); return m_percent;	};
        
// Dialog Data
    //{{AFX_DATA(CProgressDlg)
   enum { IDD = CG_IDD_PROGRESS1 };
    CProgressCtrl m_Progress;
   CString  m_percent;
   CString  m_status;
   //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
public:
    CString caption;
protected:
   BOOL ShowGauge;
   long m_nLower;
   long m_nUpper;
   long m_nStep;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 
    void UpdatePercent(int nCurrent);
    void PumpMessages();

    // Generated message map functions
    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

