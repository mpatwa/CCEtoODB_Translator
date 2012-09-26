// $Header: /CAMCAD/4.6/SelectStackDialog.cpp 5     6/29/06 4:21p Kurt Van Ness $

#include "StdAfx.h"
#include "SelectStackDialog.h"
#include ".\selectstackdialog.h"

#if !defined(__SelectStackDialog_h__)
#define __SelectStackDialog_h__

#define QLevel          "Level"
#define QDescription    "Description"
#define QEntityNumber   "Entity Number"
#define QDataCount      "Data Count"
#define QDocPosition    "Doc Position"
#define QInsertPosition "Insert Position"

//_____________________________________________________________________________
CSelectStackGrid::CSelectStackGrid(CSelectStackDialog& parentDialog)
: m_parentDialog(parentDialog)
{
}

//_____________________________________________________________________________
CSelectStackDialog* CSelectStackDialog::m_selectStackDialog = NULL;

CSelectStackDialog* CSelectStackDialog::getSelectStackDialog(CCEtoODBDoc* camCadDoc)
{
   if (camCadDoc != NULL)
   {
      if (m_selectStackDialog == NULL || m_selectStackDialog->m_camCadDoc != camCadDoc)
      {
         release();

         m_selectStackDialog = new CSelectStackDialog(camCadDoc);
         m_selectStackDialog->Create(IDD);
      }
   }

   return m_selectStackDialog;
}

void CSelectStackDialog::update(CCEtoODBDoc* camCadDoc)
{
   if (m_selectStackDialog != NULL)
   {
      getSelectStackDialog(camCadDoc)->showDialog();
      getSelectStackDialog(camCadDoc)->fillGrid();
   }
}

void CSelectStackDialog::release()
{
   if (m_selectStackDialog != NULL)
   {
      m_selectStackDialog->DestroyWindow();
      delete m_selectStackDialog;
      m_selectStackDialog = NULL;
   }
}

CSelectStackDialog::CSelectStackDialog(CCEtoODBDoc* camCadDoc,CWnd* pParent /*=NULL*/)
: CResizingDialog(CSelectStackDialog::IDD, pParent)
, m_camCadDoc(camCadDoc)
, m_grid(*this)
{
   addFieldControl(IDOK,anchorBottom);
   addFieldControl(IDCANCEL,anchorBottom);
   addFieldControl(IDC_GridStatic,anchorTop,growBoth);
}

CSelectStackDialog::~CSelectStackDialog()
{
}

void CSelectStackDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
}

void CSelectStackDialog::showDialog()
{
   ShowWindow(SW_SHOW);
}

void CSelectStackDialog::initGrid()
{
   if (m_grid.GetNumberCols() != m_colCount)
   {
      int height;
      m_grid.GetRowHeight(-1,&height);

      height = (height * 3)/2;

      m_grid.SetTH_Height(height);
      m_grid.SetSH_Width(0);
      m_grid.SetNumberRows(0,false);
      m_grid.SetNumberCols(m_colCount,false);
      m_grid.QuickSetText(m_colLevel          ,-1,QLevel);
      m_grid.QuickSetText(m_colDescription    ,-1,QDescription);      
      m_grid.QuickSetText(m_colEntityNumber   ,-1,QEntityNumber);
      m_grid.QuickSetText(m_colDataCount      ,-1,QDataCount);
      m_grid.QuickSetText(m_colDocPosition    ,-1,QDocPosition);
      m_grid.QuickSetText(m_colInsertPosition ,-1,QInsertPosition);
   }

   m_grid.SetNumberRows(0);
}

void CSelectStackDialog::fillGrid()
{
   initGrid();

   CSelectStack& selectStack = m_camCadDoc->getSelectStack();

   for (int levelIndex = 0;levelIndex < selectStack.getSize();levelIndex++)
   {
      SelectStruct* selectStruct = selectStack.getAt(levelIndex);
      DataStruct* data = selectStruct->getData();
      int dataCount = selectStruct->getParentDataList()->GetSize();
      bool atLevelFlag = (levelIndex == selectStack.getLevelIndex());
      CString description = dataTypeTagToString(data->getDataType());
      CString docPosition;
      CString insertPosition;

      if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct* geometry = m_camCadDoc->getBlockAt(data->getInsert()->getBlockNumber());
         description.AppendFormat(": %s",geometry->getName());

         CTMatrix matrix;

         if (levelIndex > 0)
         {
            matrix = selectStack.getTMatrixForLevel(levelIndex - 1);
         }

         CBasesVector basesVector = data->getInsert()->getBasesVector();
         insertPosition = basesVector.getDescriptor(4);
         basesVector.transform(matrix);
         docPosition = basesVector.getDescriptor(4);
      }

      int rowIndex = m_grid.GetNumberRows();
      m_grid.AppendRow();

      CUGCell cell;
      CString displayString;

      m_grid.GetCell(m_colLevel,rowIndex,&cell);
      displayString.Format("%d",levelIndex);
      cell.SetText(displayString);
      cell.SetReadOnly(true);
      cell.SetTextColor(atLevelFlag ? colorRed : colorBlack);
      m_grid.SetCell(m_colLevel,rowIndex,&cell);

      m_grid.GetCell(m_colDescription,rowIndex,&cell);
      cell.SetText(description);
      cell.SetReadOnly(true);
      cell.SetTextColor(atLevelFlag ? colorRed : colorBlack);
      m_grid.SetCell(m_colDescription,rowIndex,&cell);

      m_grid.GetCell(m_colEntityNumber,rowIndex,&cell);
      displayString.Format("%d",data->getEntityNumber());
      cell.SetText(displayString);
      cell.SetReadOnly(true);
      cell.SetTextColor(atLevelFlag ? colorRed : colorBlack);
      m_grid.SetCell(m_colEntityNumber,rowIndex,&cell);

      m_grid.GetCell(m_colDataCount,rowIndex,&cell);
      displayString.Format("%d",dataCount);
      cell.SetText(displayString);
      cell.SetReadOnly(true);
      cell.SetTextColor(atLevelFlag ? colorRed : colorBlack);
      m_grid.SetCell(m_colDataCount,rowIndex,&cell);

      m_grid.GetCell(m_colDocPosition,rowIndex,&cell);
      cell.SetText(docPosition);
      cell.SetReadOnly(true);
      cell.SetTextColor(atLevelFlag ? colorRed : colorBlack);
      m_grid.SetCell(m_colDocPosition,rowIndex,&cell);

      m_grid.GetCell(m_colInsertPosition,rowIndex,&cell);
      cell.SetText(insertPosition);
      cell.SetReadOnly(true);
      cell.SetTextColor(atLevelFlag ? colorRed : colorBlack);
      m_grid.SetCell(m_colInsertPosition,rowIndex,&cell);

   }

   m_grid.BestFit(0,2,selectStack.getSize(),UG_BESTFIT_TOPHEADINGS);
   //m_grid.SizeLastColumnToFit();
   m_grid.RedrawAll();
}

BEGIN_MESSAGE_MAP(CSelectStackDialog, CResizingDialog)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

#endif

BOOL CSelectStackDialog::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   m_grid.AttachGrid(this,IDC_GridStatic);

   fillGrid();

   return TRUE;  
}

void CSelectStackDialog::OnBnClickedCancel()
{
   release();
}
