// $Header: /CAMCAD/4.6/DataDoctorPageComps.h 7     4/11/07 1:50p Rick Faltersack $

#if ! defined (__DataDoctorPageComps_h__)
#define __DataDoctorPageComps_h__

#pragma once

#include "DataDoctorDialog.h"


class CDataDoctorComponentsPage;

//------------------------------------------------------------------------------

class CDDCompsGrid : public CDDBaseGrid
{
private:
   CDataDoctorComponentsPage *m_compsPage;

 public:
    CDDCompsGrid(CDataDoctorComponentsPage *compsPage);
   ~CDDCompsGrid() { }

   virtual void OnSetup();

	//cell type notifications
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

   //movement
   virtual void OnCellChange(int oldcol,int newcol,long oldrow,long newrow);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   //column swapping
	virtual void OnColSwapped(int fromCol,int toCol);

   // Column indices for grid
   // Local column index vars
   int m_colRefDes;
   int m_colStatus;
   int m_colPartNumber;
   int m_colDeviceType;
   int m_colValue;
   int m_colPTolerance;
   int m_colNTolerance;
   int m_colCapacitiveOpens;
   int m_colDiodeOpens;
   int m_colNumPins;
   int m_colSubclass;
   int m_colLoaded;
   int m_colErrors;
};

//------------------------------------------------------------------------------

class CDataDoctorComponentsPage : public CDataDoctorPropertyPageType
{
	DECLARE_DYNAMIC(CDataDoctorComponentsPage)

private:
   CDataDoctorPropertySheet& m_parent;
   CString m_deviceTypeComboList;
   CDDCompsGrid  m_componentsGrid;
   CViewLiteToolbar m_toolbar;

   bool m_updatePending;

public:
	CDataDoctorComponentsPage(CDataDoctorPropertySheet& parent);
	virtual ~CDataDoctorComponentsPage();

   CDataDoctorPropertySheet& getParent() { return m_parent; }
   CCamCadDatabase& getCamCadDatabase();
   void fillGrid();
   CString getDeviceTypeComboList();
   void setRowStyleFromStatus(int rowIndex,CDataDoctorComponent& component);
   void updateComponentInGrid(long row,long col,CString value);
   void update();

   void FixPan();

// Dialog Data
	enum { IDD = IDD_DataDoctorComponentsPage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual afx_msg void OnSize(UINT nType, int cx, int cy);

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();

   void QueueUpdate()      { m_updatePending = true; }
   bool IsUpdatePending()  { return m_updatePending; }

   void AfterEditComponentsGrid(long Row, long Col);

   //DECLARE_EVENTSINK_MAP()
   void AfterRowColChangeComponentsGrid(long OldRow, long OldCol, long NewRow, long NewCol);

   DECLARE_MESSAGE_MAP()
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg LRESULT OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
   afx_msg void OnTbBnClickedZoomIn()           { m_toolbar.OnBnClickedZoomIn(); this->FixPan(); }
   afx_msg void OnTbBnClickedColorsetsCustom()  { m_toolbar.OnBnClickedColorsetsCustom(); }
   afx_msg void OnTbBnClickedLayerTypes()       { m_toolbar.OnBnClickedLayerTypes(); }
   afx_msg void OnTbBnClickedShowPolyFill()     { m_toolbar.OnBnClickedShowPolyFills(); }
   afx_msg void OnTbBnClickedShowApertureFill() { m_toolbar.OnBnClickedShowApertureFills(); }
};



#endif