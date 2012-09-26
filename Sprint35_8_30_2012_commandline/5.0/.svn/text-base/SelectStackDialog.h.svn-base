// $Header: /CAMCAD/4.5/SelectStackDialog.h 6     6/29/06 7:46p Kurt Van Ness $

#pragma once

#include "ResizingDialog.h"
#include "Resource.h"
#include "CcDoc.h"
#include "UGCtrl.h"

#define ImplementSelectStackDialog

class CSelectStackDialog;

//_____________________________________________________________________________
class CSelectStackGrid : public CUGCtrl
{
private:
   CSelectStackDialog& m_parentDialog;
   int m_currentCol;
   int m_currentRow;

public:
   CSelectStackGrid(CSelectStackDialog& parentDialog);

   //virtual void ExpandLastColumnToFit();
   //virtual void SizeLastColumnToFit(bool expandOnlyFlag=false);

   //int GetSH_NumberCols() const { return m_GI->m_numberSideHdgCols; }

   //// old
   //virtual void OnEditCell(int nRow, int nCol, UINT nChar);
   //virtual void OnNonFixedCellClick(int nRow, int nCol);

   //// UG
   //virtual void OnLClicked(int col,long row,int updn,RECT* rect,POINT* point,BOOL processed);
   //virtual int OnEditStart(int col, long row, CWnd **edit);
};

//_____________________________________________________________________________
class CSelectStackDialog : public CResizingDialog
{
private:
   const static int m_colLevel          = 0;
   const static int m_colDescription    = 1;
   const static int m_colEntityNumber   = 2;
   const static int m_colDataCount      = 3;
   const static int m_colDocPosition    = 4;
   const static int m_colInsertPosition = 5;
   const static int m_colCount          = 6;

   static CSelectStackDialog* m_selectStackDialog;

   CCEtoODBDoc* m_camCadDoc;
   CSelectStackGrid m_grid;

public:
	CSelectStackDialog(CCEtoODBDoc* camCadDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectStackDialog();

// Dialog Data
	enum { IDD = IDD_SelectStackDialog };

   virtual CString GetDialogProfileEntry() { return CString("SelectStackDialog"); }

   void initGrid();
   void fillGrid();
   void showDialog();

   static CSelectStackDialog* getSelectStackDialog(CCEtoODBDoc* camCadDoc);
   static void release();
   static void update(CCEtoODBDoc* camCadDoc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedCancel();
};
