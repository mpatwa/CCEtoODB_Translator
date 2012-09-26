// $Header: /CAMCAD/4.3/BgrndBmp.h 7     8/12/03 9:05p Kurt Van Ness $

#if !defined(AFX_BGRNDBMP_H__E0E3A96F_F0BC_11D2_9866_004005408E44__INCLUDED_)
#define AFX_BGRNDBMP_H__E0E3A96F_F0BC_11D2_9866_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCEtoODBDoc;

void LoadBackgroundBitmap(CCEtoODBDoc *doc, CString filename, BOOL Top);

/////////////////////////////////////////////////////////////////////////////
// BackgroundBitmap dialog

class BackgroundBitmap : public CDialog
{
// Construction
public:
   BackgroundBitmap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(BackgroundBitmap)
   enum { IDD = IDD_BACKGROUND_BITMAP };
   CSpinButtonCtrl   m_widthSpin;
   CSpinButtonCtrl   m_heightSpin;
   CSpinButtonCtrl   m_bottomSpin;
   CSpinButtonCtrl   m_leftSpin;
   int      m_aspectRatio;
   BOOL  m_onTop;
   CString  m_bottom;
   CString  m_left;
   CString  m_width;
   CString  m_height;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   BOOL Started;
   int decimals;
   double factor;
   double aspectRatio;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(BackgroundBitmap)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(BackgroundBitmap)
   afx_msg void OnApply();
   virtual void OnOK();
   afx_msg void OnFitToPage();
   virtual BOOL OnInitDialog();
   afx_msg void OnDeltaposHeightSpin(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeltaposWidthSpin(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeltaposBottomSpin(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDeltaposLeftSpin(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BGRNDBMP_H__E0E3A96F_F0BC_11D2_9866_004005408E44__INCLUDED_)
