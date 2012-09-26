// $Header: /CAMCAD/4.6/DataDoctorPageParts.h 6     4/06/07 4:47p Rick Faltersack $

#if ! defined (__DataDoctorPageParts_h__)
#define __DataDoctorPageParts_h__

#pragma once

#include "DataDoctorDialog.h"

class CDataDoctorPartsPage;

//----------------------------------------------------------------------------

class CDDPartsGrid : public CDDBaseGrid
{
private:
   CDataDoctorPartsPage *m_partsPage;

 public:
    CDDPartsGrid(CDataDoctorPartsPage *partsPage)  { m_partsPage = partsPage; }
   ~CDDPartsGrid() { }

   virtual void OnSetup();

	//cell type notifications
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);

   //movement
   virtual void OnCellChange(int oldcol,int newcol,long oldrow,long newrow);

	//editing
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual int OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey);
	virtual int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

   // mouse
   virtual void OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed);

   //column swapping
	virtual void OnColSwapped(int fromCol,int toCol);

};

//----------------------------------------------------------------------------


class CDataDoctorPartsPage : public CDataDoctorPropertyPageType
{
	DECLARE_DYNAMIC(CDataDoctorPartsPage)

private:
   BOOL m_overwriteValues;
	BOOL m_storeOption;

   CDataDoctorPropertySheet& m_parent;
   CString m_deviceTypeComboList;
   CDDPartsGrid m_partsGrid;

	void setRowStyleForPinMap(int rowIndex,CDataDoctorPart& part);

   bool m_updatePending;
   bool m_dialogIsInitialized;

public:
   // Column index vars
	int m_colFromLibrary;
   int m_colPartNumber;
   int m_colStatus;
   int m_colDeviceType;
   int m_colValue;
   int m_colPTolerance;
   int m_colNTolerance;
   int m_colNumPins;
	int m_colPinMap;
   int m_colSubclass;
	int m_colDescription;
   int m_colErrors;
   int m_colRefDes;
   int m_colComments;

   CColumnDefMap m_columnDefMap;

   CDDSpecialColumnMap m_customColumnMap_PartGrid;

	CDataDoctorPartsPage(CDataDoctorPropertySheet& parent);
	virtual ~CDataDoctorPartsPage();

   CDataDoctorPropertySheet& getParent() { return m_parent; }
   CCamCadDatabase& getCamCadDatabase();

   bool DialogIsInitialized()    { return m_dialogIsInitialized; }

   void DefineGridColumnMap();

   void fillGrid();
   CString getDeviceTypeComboList();
   void setRowStyle(int rowIndex,CDataDoctorPart& part);
   void updatePartInGrid(long row,long col,CString value);
   void update();

// Dialog Data
	enum { IDD = IDD_DataDoctorPartsPage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual afx_msg void OnSize(UINT nType, int cx, int cy);

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();

   void QueueUpdate()      { m_updatePending = true; }
   bool IsUpdatePending()  { return m_updatePending; }

   void EnableDatabaseControls(bool flag);

   void AfterEditPartsGrid(long Row, long Col);
   void MouseDownGrid(short Button, short Shift, float X, float Y);
   void AfterRowColChangePartsGrid(long OldRow, long OldCol, long NewRow, long NewCol);
   void CellButtonClickOnGrid(long Row, long Col);

   bool IsCustomPartGridColumn(int colIndx)  { return m_customColumnMap_PartGrid.GetSpecialColumn(colIndx) != NULL; }

   DECLARE_EVENTSINK_MAP()
   
   void OnRButtonDown(CPoint windowRelativePoint);

   DECLARE_MESSAGE_MAP()
   afx_msg void OnBnClickedLoadFromLibrary();
   afx_msg void OnBnClickedStoreToLibrary();
   afx_msg void OnLoadPart();
   afx_msg void OnSavePart();
	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
   afx_msg LRESULT OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL);
};


#endif