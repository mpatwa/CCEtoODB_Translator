// $Header: /CAMCAD/4.6/DataDoctorPageLibrary.h 20    4/06/07 4:46p Rick Faltersack $

#if ! defined (__DataDoctorPageLibrary_h__)
#define __DataDoctorPageLibrary_h__

#pragma once

#include "DataDoctorDialog.h"

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

class CDataDoctorLibraryPage : public CDataDoctorPropertyPageType
{
	DECLARE_DYNAMIC(CDataDoctorLibraryPage)

private:
   CDataDoctorPropertySheet& m_parent;

   // List boxes for display
   CListBox m_lstAvailableAttribsPrt;
   CListBox m_lstSelectedAttribsPrt;
   CListBox m_lstAvailableAttribsPkg;
   CListBox m_lstSelectedAttribsPkg;

   int m_chkUseDatabaseConnection;

   bool m_updatePending;
   bool m_dialogIsInitialized;

public:
   // Dialog Data
	enum { IDD = IDD_DataDoctorLibraryPage };

   CDataDoctorLibraryPage(CDataDoctorPropertySheet& parent);
   virtual ~CDataDoctorLibraryPage();

   CDataDoctorPropertySheet& getParent() { return m_parent; }
   CCamCadDatabase& getCamCadDatabase();

   void QueueUpdate()      { m_updatePending = true; }
   bool IsUpdatePending()  { return m_updatePending; }

   void EnableDatabaseControls(bool attempt, bool allowed);

   bool DialogIsInitialized()    { return m_dialogIsInitialized; }


private:

// Dialog Data
   //{{AFX_DATA(GenerateCentroidDlg)
  // enum { IDD = IDD_GEN_CENTROID };
   int m_radioApplyAll;
   int m_radioShowPins;
   //}}AFX_DATA
   CCEtoODBDoc *m_doc;
	FileStruct *m_activeFile;


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(GenerateCentroidDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(GenerateCentroidDlg)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
   //afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   
   afx_msg void OnBnClickedLoadFromLibrary();
   afx_msg void OnBnClickedBrowseButton();

   afx_msg void OnBnClickedChkUseDatabaseConnection();

   /*
   afx_msg void OnBnClickedStoreToLibrary();
	
   afx_msg void OnBnClickedPinsShowAll();
   afx_msg void OnBnClickedPinsShowNamed();
   afx_msg void OnBnClickedPinsShowNumbered();
   afx_msg void OnBnClickedClearCentroids();
   afx_msg void OnTbBnClickedZoomIn()           { m_toolbar.OnBnClickedZoomInOrigin(); }
   afx_msg void OnTbBnClickedColorsetsCustom()  { m_toolbar.OnBnClickedColorsetsCustom(); }
   afx_msg void OnTbBnClickedLayerTypes()       { m_toolbar.OnBnClickedLayerTypes(); }
   afx_msg void OnTbBnClickedShowPolyFill()     { m_toolbar.OnBnClickedShowPolyFills(); }
   afx_msg void OnTbBnClickedShowApertureFill() { m_toolbar.OnBnClickedShowApertureFills(); }
   afx_msg LRESULT OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
   */
   
public:
   


public:
   //afx_msg void OnBnClickedBtnShowgeom();
   //afx_msg void OnBnClickedBtnGenCentroids();
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();
   void update();
   virtual CString GetDialogProfileEntry() { return "DataDoctorLibraryTab"; }

   //void DoEditGeom();

   //void AfterEditGrid(long Row, long Col);

   DECLARE_EVENTSINK_MAP()
   //void StartEditGrid(long Row, long Col, BOOL* Cancel);
	//void MouseDownGrid(short Button, short Shift, float X, float Y);
   //void OnRButtonDown(CPoint windowRelativePoint);

   

private:
   //CFlexGrid m_flexGrid;
	//int m_priority[3];
	//*rcf CFlexGridStatic m_flexGridStatic;
   //CDDCentroidsGrid m_grid;
   //int m_chkHideSinglePinComps;
   //int m_chkZeroRotation;

private:
   void addFieldControls();

public:
   CComboBox m_cboDefault;
   afx_msg void OnBnClickedBtnClose();
   afx_msg void OnBnClickedAddPrtAttrib();
   afx_msg void OnBnClickedRemovePrtAttrib();
   afx_msg void OnBnClickedAddPkgAttrib();
   afx_msg void OnBnClickedRemovePkgAttrib();
   afx_msg void OnLoadAll(); // from DB library

};

//-----------------------------------------------------------------------------

#endif