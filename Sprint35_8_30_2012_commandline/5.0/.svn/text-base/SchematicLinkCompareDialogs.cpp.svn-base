// $Header: /CAMCAD/4.6/SchematicLinkCompareDialogs.cpp 9     4/12/07 3:35p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// SchematicLinkCompareDialogs.cpp

#include "stdafx.h"
#include "SchematicLink.h"
#include "SchematicLinkCompareDialogs.h"
#include "CCEtoODB.h"
#include "ResizingDialog.h"
#include ".\schematiclinkcomparedialogs.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void updateGridAndListbox(CListBox& schematicListbox, CListBox& pcbListbox, CSchematicGrid& schematicGrid, 
                                                       CMapStringToString& nonMatchSchematicMap, CMapStringToString& nonMatchPcbMap, CMapStringToString& matchSchematicToPcbMap)
{
   schematicGrid.SetRedraw(FALSE);
   schematicGrid.initGrid();

   schematicListbox.SetRedraw(FALSE);
   schematicListbox.ResetContent();

   pcbListbox.SetRedraw(FALSE);
   pcbListbox.ResetContent();

   for (POSITION pos=nonMatchSchematicMap.GetStartPosition(); pos != NULL;)
   {
      CString key, schematicName;
      nonMatchSchematicMap.GetNextAssoc(pos, key, schematicName);

   if (schematicName.IsEmpty())
      int a = 0;

      schematicListbox.AddString(schematicName);
   }

   for (POSITION pos=nonMatchPcbMap.GetStartPosition(); pos != NULL;)
   {
      CString key, pcbName;
      nonMatchPcbMap.GetNextAssoc(pos, key, pcbName);

      pcbListbox.AddString(pcbName);
   }

   for (POSITION pos=matchSchematicToPcbMap.GetStartPosition(); pos != NULL;)
   {
      CString schematicName, pcbName;
      matchSchematicToPcbMap.GetNextAssoc(pos, schematicName, pcbName);

      schematicGrid.addNames(schematicName, pcbName);
   }

   schematicGrid.sort(schematicGrid.getColSchematic(), sortByAscending);
   schematicGrid.SetRedraw(TRUE);
   schematicGrid.RedrawAll();

   schematicListbox.SetRedraw(TRUE);
   schematicListbox.UpdateWindow();

   pcbListbox.SetRedraw(TRUE);
   pcbListbox.UpdateWindow();
}

static void createReport(const CListBox& schematicListbox, const CListBox& pcbListbox, CSchematicGrid& schematicGrid, const CString reportFileName, const bool isComponent)
{
   CFormatStdioFile file;
   CFileException err;

   if (file.Open(reportFileName, CFile::modeCreate|CFile::modeWrite, &err))
   {
      CTime t = t.GetCurrentTime();

      // Write header
      file.WriteString("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
      if (isComponent)
         file.WriteString("Schematic Link Component Comparision Report\n");
      else
         file.WriteString("Schematic Link Net Comparision Report\n");
      file.WriteString("\n");
      file.WriteString("Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
      file.WriteString("Version     : %s\n", getApp().getVersionString());
      file.WriteString("Date & Time : %s\n", t.Format("%A, %B %d, %Y at %H:%M:%S"));
      file.WriteString("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

      // Write Schematic non-match component
      file.WriteString("\n\n\n");
      file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      if (isComponent)
         file.WriteString("Schematic Component with no Match\n");
      else
         file.WriteString("Schematic Net with no Match\n");
      file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

      for (int i=0; i<schematicListbox.GetCount(); i++)
      {
         CString name;
         schematicListbox.GetText(i, name);
         file.WriteString("\t%s\n", name);
      }

      // Write Pcb non-match component
      file.WriteString("\n\n\n");
      file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      if (isComponent)
         file.WriteString("Pcb Component with no Match\n");
      else
         file.WriteString("Pcb Net with no Match\n");
      file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

      for (int i=0; i<pcbListbox.GetCount(); i++)
      {
         CString name;
         pcbListbox.GetText(i, name);
         file.WriteString("\t%s\n", name);
      }

      // Write Schematic to Pcb component match
      file.WriteString("\n\n\n");
      file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      if (isComponent)
         file.WriteString("Schematic to Pcb Component Match\n");
      else
         file.WriteString("Schematic to Pcb Net Match\n");
      file.WriteString("\n");
      file.WriteString("\t%-*s%s\n", 20, "Schematic", "Pcb");
      file.WriteString("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

      for (long i=0; i<schematicGrid.GetNumberRows(); i++)
      {
         CUGCell schCell, pcbCell;

         schematicGrid.GetCell(schematicGrid.getColSchematic(), i, &schCell);
         schematicGrid.GetCell(schematicGrid.getColPcb(), i, &pcbCell);

         file.WriteString("\t%-*s%s\n", 20, schCell.GetText(), pcbCell.GetText());
      }

      file.Close();
   }   
}

//---------------------------------------------------------------------------------------
// CSchematicGrid
//---------------------------------------------------------------------------------------
CSchematicGrid::CSchematicGrid()
{
   m_initiated = false;
   m_editMode = false;
   m_colSchematicSortOrder = sortByAscending;
   m_colPcbSortOrder = sortByAscending;
}

CSchematicGrid::~CSchematicGrid()
{
}

void CSchematicGrid::initGrid()
{
   if (!m_initiated)
   {
      m_initiated = true;

      SetNumberRows(0,false);
      SetNumberCols(2,false);
      SetSH_Width(0);
      SetColWidth(0, 140);
      SetColWidth(1, 150);
      QuickSetText(m_colSchematic, -1, "Schematic");
      QuickSetText(m_colPcb, -1, "Pcb");

      //SetPaintMode(FALSE);
      //FitToWindow(0, 1);
      //SetPaintMode(TRUE);
      //QuickSetAlignment(m_colDepositCount,-1,UG_ALIGNLEFT | UG_ALIGNVCENTER);
   }

   SetNumberRows(0);
}

void CSchematicGrid::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
   if (m_editMode && col == m_colPcb && row > -1)
   {
      CString comboString = "Test1|test2|test3";      

      CUGCell cell;
      GetCell(col,row,&cell);
      cell.SetCellType(XPCellTypeCombo);
      cell.SetText(comboString);
      SetCell(col,row,&cell);
   }
}

void CSchematicGrid::OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
   if (col == m_colSchematic && updn == 1)
   {
      m_colSchematicSortOrder = (m_colSchematicSortOrder==sortByAscending)?sortByDescending:sortByAscending;
      SortBy(col, (int)m_colSchematicSortOrder);
   }
   else if (col == m_colPcb && updn == 1)
   {
      m_colPcbSortOrder = (m_colPcbSortOrder==sortByAscending)?sortByDescending:sortByAscending;
      SortBy(col, (int)m_colPcbSortOrder);
   }

   this->RedrawAll();
}

void CSchematicGrid::addSchematicName(const CString schematicName)
{
   // Add to grid
   int rowIndex = GetNumberRows();
   AppendRow();

   CUGCell cell;

   GetCell(m_colSchematic,rowIndex,&cell);
   cell.SetText(schematicName);
   //cell.SetReadOnly(true);
   SetCell(m_colSchematic,rowIndex,&cell);
}

void CSchematicGrid::addNames(const CString schematicName, const CString pcbName)
{
   // Add to grid
   int rowIndex = GetNumberRows();
   AppendRow();

   CUGCell cell;

   GetCell(m_colSchematic,rowIndex,&cell);
   cell.SetText(schematicName);
   //cell.SetReadOnly(true);
   SetCell(m_colSchematic,rowIndex,&cell);

   GetCell(m_colPcb,rowIndex,&cell);
   cell.SetText(pcbName);
   //cell.SetReadOnly(true);
   SetCell(m_colPcb,rowIndex,&cell);
}

void CSchematicGrid::sort(const int col, const GridSortOrderTag sortOrder)
{
   SortBy(col, (int)sortOrder);

   if (col == m_colSchematic)
   {
      m_colSchematicSortOrder = sortOrder;
   }
   else if (col == m_colPcb)
   {
      m_colPcbSortOrder = sortOrder;
   }
}

//---------------------------------------------------------------------------------------
// CSchematicLinkCompareRefdesDlg
//---------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CSchematicLinkCompareRefdesDlg, CPropertyPage)
CSchematicLinkCompareRefdesDlg::CSchematicLinkCompareRefdesDlg(SchematicLink& schematicLink)
	: CPropertyPage(CSchematicLinkCompareRefdesDlg::IDD)
   , m_schematicLink(schematicLink)
	, m_caseSensitive(FALSE)
	, m_killTrailing((int)killTrailingAlpha)
	, m_clearMatchType((int)clearMatchByCrossReference)
	, m_showResultType((int)showAllMatch)
	, m_generateReport(TRUE)
	, m_killTrialingSuffix(_T(""))
{
}

CSchematicLinkCompareRefdesDlg::~CSchematicLinkCompareRefdesDlg()
{
}

void CSchematicLinkCompareRefdesDlg::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Radio(pDX, IDC_CaseSensitiveOff, m_caseSensitive);
   DDX_Radio(pDX, IDC_KillTrailingNone, m_killTrailing);
   DDX_Radio(pDX, IDC_CLEARALL, m_clearMatchType);
   DDX_Radio(pDX, IDC_ShowAll, m_showResultType);
   DDX_Check(pDX, IDC_GenerateReport, m_generateReport);
   DDX_Control(pDX, IDC_EditMatchResult, m_editMatch);
   DDX_Text(pDX, IDC_TrailingSurffix, m_killTrialingSuffix);
   DDX_Control(pDX, IDC_SchematicList, m_schematicListbox);
   DDX_Control(pDX, IDC_PCBLIST, m_pcbListbox);
}

BEGIN_MESSAGE_MAP(CSchematicLinkCompareRefdesDlg, CPropertyPage)
   ON_BN_CLICKED(IDC_KillTrailingNone, OnBnClickedKillTrailing)
   ON_BN_CLICKED(IDC_KillTrailingByAlphas, OnBnClickedKillTrailing)
   ON_BN_CLICKED(IDC_KillTrailingBySurffix, OnBnClickedKillTrailing)
   ON_BN_CLICKED(IDC_LoadCrossReference, OnBnClickedLoadCrossReference)
   ON_BN_CLICKED(IDC_SaveCrossReference, OnBnClickedSaveCrossReference)
   ON_BN_CLICKED(IDC_ClearMatch, OnBnClickedClearMatch)
   ON_BN_CLICKED(IDC_EditMatchResult, OnBnClickedEditMatchResult)
   ON_BN_CLICKED(IDC_CompareItem, OnBnClickedCompareItem)
END_MESSAGE_MAP()

BOOL CSchematicLinkCompareRefdesDlg::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   //  For now disable all these controls
   GetDlgItem(IDC_LoadCrossReference)->EnableWindow(FALSE);
   GetDlgItem(IDC_SaveCrossReference)->EnableWindow(FALSE);
   GetDlgItem(IDC_CLEARALL)->EnableWindow(FALSE);
   GetDlgItem(IDC_ClearAutoMatch)->EnableWindow(FALSE);
   GetDlgItem(IDC_ClearCrossReference)->EnableWindow(FALSE);
   GetDlgItem(IDC_EditMatchResult)->EnableWindow(FALSE);

   m_editMode = false;
   m_schematicGrid.AttachGrid(this, IDC_MatchStatic);

   initialDataLoad();
   updateMatchCount();
   UpdateData(FALSE);

   return FALSE;
}

BOOL CSchematicLinkCompareRefdesDlg::OnSetActive()
{
   // TODO:
   CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_NEXT);

   return CPropertyPage::OnSetActive();
}

void CSchematicLinkCompareRefdesDlg::initialDataLoad()
{
   m_schematicGrid.initGrid();
   m_schematicListbox.ResetContent();
   m_pcbListbox.ResetContent();
   m_schematicNameMap.RemoveAll();
   m_pcbNameMap.RemoveAll();

   for (POSITION pos = m_schematicLink.getSchematicCollection().getComponentStartPosition(); pos != NULL;)
   {
      ComponentItem* componentItem = m_schematicLink.getSchematicCollection().getNextComponent(pos);
      if (componentItem != NULL)
      {
         CString value;
         CString compareName = componentItem->getCompareName();

         CString lowerCaseCompareName = compareName;
         lowerCaseCompareName.MakeLower();

         if (!compareName.IsEmpty() && !m_schematicNameMap.Lookup(lowerCaseCompareName, value))
         {
            m_schematicListbox.AddString(compareName);
            m_schematicNameMap.SetAt(lowerCaseCompareName, compareName);
         }
      }
   }

   for (POSITION pos = m_schematicLink.getPcbCollection().getComponentStartPosition(); pos != NULL;)
   {
      ComponentItem* componentItem = m_schematicLink.getPcbCollection().getNextComponent(pos);
      if (componentItem != NULL)
      {
         CString value;
         CString compareName = componentItem->getCompareName();

         CString lowerCaseCompareName = compareName;
         lowerCaseCompareName.MakeLower();

         if (!compareName.IsEmpty() && !m_pcbNameMap.Lookup(lowerCaseCompareName, value))
         {
            m_pcbListbox.AddString(compareName);
            m_pcbNameMap.SetAt(lowerCaseCompareName, compareName);
         }
      }
   }

}

void CSchematicLinkCompareRefdesDlg::updateMatchCount()
{
   CString count;

   count.Format("%d", m_schematicGrid.GetNumberRows() + m_schematicListbox.GetCount());
   GetDlgItem(IDC_SchematicAllCount)->SetWindowText(count);

   count.Format("%d",  m_schematicGrid.GetNumberRows() + m_pcbListbox.GetCount());
   GetDlgItem(IDC_PcbAllCount)->SetWindowText(count);

   count.Format("%d", m_schematicGrid.GetNumberRows());
   GetDlgItem(IDC_SchematicMatchCount)->SetWindowText(count);
   GetDlgItem(IDC_PcbMatchCount)->SetWindowText(count);

   count.Format("%d", m_schematicListbox.GetCount());
   GetDlgItem(IDC_SchematicNonMatchCount)->SetWindowText(count);

   count.Format("%d", m_pcbListbox.GetCount());
   GetDlgItem(IDC_PcbNonMatchCount)->SetWindowText(count);
}

void CSchematicLinkCompareRefdesDlg::OnBnClickedKillTrailing()
{
   UpdateData(TRUE);

   if (m_killTrailing == killTrailingSuffix)
   {
      GetDlgItem(IDC_TrailingSurffix)->EnableWindow(TRUE);      
   }
   else
   {
      GetDlgItem(IDC_TrailingSurffix)->EnableWindow(FALSE);
   }
}

void CSchematicLinkCompareRefdesDlg::OnBnClickedLoadCrossReference()
{
   // TODO: Add your control notification handler code here
}

void CSchematicLinkCompareRefdesDlg::OnBnClickedSaveCrossReference()
{
   // TODO: Add your control notification handler code here
}

void CSchematicLinkCompareRefdesDlg::OnBnClickedClearMatch()
{
   m_schematicLink.clearComponentItemMatch();
   
   CMapStringToString matchSchematicToPcbMap;
   updateGridAndListbox(m_schematicListbox, m_pcbListbox, m_schematicGrid, m_schematicNameMap, m_pcbNameMap, matchSchematicToPcbMap);
   updateMatchCount();
}

void CSchematicLinkCompareRefdesDlg::OnBnClickedEditMatchResult()
{
   m_editMode = !m_editMode;
   m_schematicGrid.setEditMode(m_editMode);
}

void CSchematicLinkCompareRefdesDlg::OnBnClickedCompareItem()
{
   UpdateData(TRUE);

   CMapStringToString nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap;
   m_schematicLink.processComponentItemMatch(m_caseSensitive?true:false, (KillTrailingType)m_killTrailing, m_killTrialingSuffix, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);

   updateGridAndListbox(m_schematicListbox, m_pcbListbox, m_schematicGrid, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
   updateMatchCount();
}


LRESULT CSchematicLinkCompareRefdesDlg::OnWizardNext()
{
   CString reportFileName = GetLogfilePath("SchematicLinkComponentCompare.log");
   createReport(m_schematicListbox, m_pcbListbox, m_schematicGrid, reportFileName, true);

   return CPropertyPage::OnWizardNext();
}

//---------------------------------------------------------------------------------------
// SchematicLinkCompareNetDlg
//---------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CSchematicLinkCompareNetDlg, CPropertyPage)
CSchematicLinkCompareNetDlg::CSchematicLinkCompareNetDlg(SchematicLink& schematicLink)
	: CPropertyPage(CSchematicLinkCompareNetDlg::IDD)
   , m_schematicLink(schematicLink)
	, m_caseSensitive(FALSE)
	, m_compareByType((int)netCompareByContent)
	, m_clearMatchType((int)clearMatchByCrossReference)
	, m_showResultType((int)showAllMatch)
	, m_generateReport(TRUE)
{
   m_matchPercentage = "70";
}

CSchematicLinkCompareNetDlg::~CSchematicLinkCompareNetDlg()
{
}

void CSchematicLinkCompareNetDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_CaseSensitiveOff, m_caseSensitive);
	DDX_Radio(pDX, IDC_CompareByName, m_compareByType);
	DDX_Control(pDX, IDC_CompareFollowByName, m_followByName);
	DDX_Radio(pDX, IDC_CLEARALL, m_clearMatchType);
	DDX_Radio(pDX, IDC_ShowAll, m_showResultType);
	DDX_Check(pDX, IDC_GenerateReport, m_generateReport);
	DDX_Control(pDX, IDC_EditMatchResult, m_editMatch);
   DDX_Control(pDX, IDC_SchematicList, m_schematicListbox);
   DDX_Control(pDX, IDC_PCBLIST, m_pcbListbox);
}

BEGIN_MESSAGE_MAP(CSchematicLinkCompareNetDlg, CPropertyPage)
   ON_BN_CLICKED(IDC_CompareByName, OnBnClickCompareBy)
   ON_BN_CLICKED(IDC_CompareByContent, OnBnClickCompareBy)
   ON_BN_CLICKED(IDC_CompareByPcbAsMaster, OnBnClickCompareBy)
   ON_BN_CLICKED(IDC_LoadCrossReference, OnBnClickedLoadCrossReference)
   ON_BN_CLICKED(IDC_SaveCrossReference, OnBnClickedSaveCrossReference)
   ON_BN_CLICKED(IDC_ClearMatch, OnBnClickedClearMatch)
   ON_BN_CLICKED(IDC_EditMatchResult, OnBnClickedEditMatchResult)
   ON_BN_CLICKED(IDC_CompareItem, OnBnClickedCompareItem)
END_MESSAGE_MAP()

BOOL CSchematicLinkCompareNetDlg::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   //  For now disable all these controls
   GetDlgItem(IDC_CompareByPcbAsMaster)->EnableWindow(FALSE);
   GetDlgItem(IDC_LoadCrossReference)->EnableWindow(FALSE);
   GetDlgItem(IDC_SaveCrossReference)->EnableWindow(FALSE);
   GetDlgItem(IDC_CLEARALL)->EnableWindow(FALSE);
   GetDlgItem(IDC_ClearAutoMatch)->EnableWindow(FALSE);
   GetDlgItem(IDC_ClearCrossReference)->EnableWindow(FALSE);
   GetDlgItem(IDC_EditMatchResult)->EnableWindow(FALSE);

   m_schematicGrid.AttachGrid(this, IDC_MatchStatic);

   initialDataLoad();
   updateMatchCount();
   UpdateData(FALSE);

   return TRUE;
}

BOOL CSchematicLinkCompareNetDlg::OnSetActive()
{
   // TODO:
   CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

   return CPropertyPage::OnSetActive();
}

void CSchematicLinkCompareNetDlg::initialDataLoad()
{
   m_schematicGrid.initGrid();
   m_schematicListbox.ResetContent();
   m_pcbListbox.ResetContent();
   m_schematicNameMap.RemoveAll();
   m_pcbNameMap.RemoveAll();

   for (POSITION pos = m_schematicLink.getSchematicCollection().getNetItemStartPosition(); pos != NULL;)
   {
      NetItem* netItem = m_schematicLink.getSchematicCollection().getNextNetItem(pos);
      if (netItem != NULL)
      {
         CString value;
         CString netName = netItem->getName();
         CString lowerCaseNetName = netName;
         lowerCaseNetName.MakeLower();

         if (!netName.IsEmpty() && !m_schematicNameMap.Lookup(lowerCaseNetName, value))
         {
            m_schematicListbox.AddString(netName);
            m_schematicNameMap.SetAt(lowerCaseNetName, netName);
         }
      }
   }

   for (POSITION pos = m_schematicLink.getPcbCollection().getNetItemStartPosition(); pos != NULL;)
   {
      NetItem* netItem = m_schematicLink.getPcbCollection().getNextNetItem(pos);
      if (netItem != NULL)
      {
         CString value;
         CString netName = netItem->getName();
         CString lowerCaseNetName = netName;
         lowerCaseNetName.MakeLower();

         if (!netName.IsEmpty() && !m_pcbNameMap.Lookup(lowerCaseNetName, value))
         {
            m_pcbListbox.AddString(netName);
            m_pcbNameMap.SetAt(lowerCaseNetName, netName);
         }
      }
   }
}

void CSchematicLinkCompareNetDlg::updateMatchCount()
{
   CString count;

   count.Format("%d", m_schematicGrid.GetNumberRows() + m_schematicListbox.GetCount());
   GetDlgItem(IDC_SchematicAllCount)->SetWindowText(count);

   count.Format("%d",  m_schematicGrid.GetNumberRows() + m_pcbListbox.GetCount());
   GetDlgItem(IDC_PcbAllCount)->SetWindowText(count);

   count.Format("%d", m_schematicGrid.GetNumberRows());
   GetDlgItem(IDC_SchematicMatchCount)->SetWindowText(count);
   GetDlgItem(IDC_PcbMatchCount)->SetWindowText(count);

   count.Format("%d", m_schematicListbox.GetCount());
   GetDlgItem(IDC_SchematicNonMatchCount)->SetWindowText(count);

   count.Format("%d", m_pcbListbox.GetCount());
   GetDlgItem(IDC_PcbNonMatchCount)->SetWindowText(count);
}

void CSchematicLinkCompareNetDlg::OnBnClickCompareBy()
{
   // TODO: Add your control notification handler code here
	if (m_compareByType == (int)netCompareByName)
	{
		m_followByName.EnableWindow(FALSE);
	}
	else
	{
		m_followByName.EnableWindow(TRUE);
	}
}

void CSchematicLinkCompareNetDlg::OnBnClickedLoadCrossReference()
{
   // TODO: Add your control notification handler code here
}

void CSchematicLinkCompareNetDlg::OnBnClickedSaveCrossReference()
{
   // TODO: Add your control notification handler code here
}

void CSchematicLinkCompareNetDlg::OnBnClickedClearMatch()
{
   m_schematicLink.clearNetItemMatch();
   
   CMapStringToString matchSchematicToPcbMap;
   updateGridAndListbox(m_schematicListbox, m_pcbListbox, m_schematicGrid, m_schematicNameMap, m_pcbNameMap, matchSchematicToPcbMap);
   updateMatchCount();
}

void CSchematicLinkCompareNetDlg::OnBnClickedEditMatchResult()
{
   // TODO: Add your control notification handler code here
}

void CSchematicLinkCompareNetDlg::OnBnClickedCompareItem()
{
   UpdateData(TRUE);

   CMapStringToString nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap;
   double matchPercentage =  atof(m_matchPercentage);

   switch (m_compareByType)
   {
   case netCompareByName:           
      m_schematicLink.processNetItemMatchByName(m_caseSensitive?true:false, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
      break;

   case netCompareByContent:        
      m_schematicLink.processNetItemMatchByContent(m_caseSensitive?true:false, m_followByName.GetCheck()==BST_CHECKED, matchPercentage, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
      break;

   case netCompareByPcbAsMaster:    
      m_schematicLink.processNetItemMatchByContentPcbAsMaster(m_caseSensitive?true:false, m_followByName.GetCheck()==BST_CHECKED, matchPercentage, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
      break;

   default:                                                          
      break;
   }

   updateGridAndListbox(m_schematicListbox, m_pcbListbox, m_schematicGrid, nonMatchSchematicMap, nonMatchPcbMap, matchSchematicToPcbMap);
   updateMatchCount();
}

BOOL CSchematicLinkCompareNetDlg::OnWizardFinish()
{
   CString reportFileName = GetLogfilePath("SchematicLinkNetCompare.log");
   createReport(m_schematicListbox, m_pcbListbox, m_schematicGrid, reportFileName, false);

   return CPropertyPage::OnWizardFinish();
}

//---------------------------------------------------------------------------------------
// CSelectMatchedDlg
//---------------------------------------------------------------------------------------
CSelectMatchedDlg::CSelectMatchedDlg(SchematicLink& schematicLink, const bool isComponent, const bool isSchematic)
	: CResizingDialog(CSelectMatchedDlg::IDD, NULL)
   , m_schematicLink(schematicLink)
   , m_isComponent(isComponent)
   , m_isSchematic(isSchematic)
   , m_matchedItemDataList(false)
{
   m_lastSelectedItemData = NULL;
   m_netItem = NULL;

	addFieldControl(IDCANCEL, anchorBottom);
	CResizingDialogField& okField = addFieldControl(IDOK, anchorBottomRight);
	CResizingDialogField& matchListField = addFieldControl(IDC_MatchList, anchorLeft);
	matchListField.getOperations().addOperation(glueBottomEdge, toTopEdge, &okField);
	matchListField.getOperations().addOperation(glueRightEdge, toRightEdge, &okField);
}

CSelectMatchedDlg::~CSelectMatchedDlg()
{
   m_matchedItemDataList.empty();
   m_lastSelectedItemData = NULL;
   m_netItem = NULL;
}

void CSelectMatchedDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_MatchList, m_matchedListbox);
}

BEGIN_MESSAGE_MAP(CSelectMatchedDlg, CDialog)
   ON_LBN_SELCHANGE(IDC_MatchList, OnLbnSelChangeMatchList)
END_MESSAGE_MAP()

BOOL CSelectMatchedDlg::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   fillListBox();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectMatchedDlg::fillListBox()
{
   m_matchedListbox.ResetContent();

   for (POSITION pos=m_matchedItemDataList.GetHeadPosition(); pos != NULL;)
   {
      ItemData* itemData = m_matchedItemDataList.GetNext(pos);
      if (itemData !=  NULL)
      {
         CString name;
         BlockStruct* block = NULL;

         if (m_isComponent)
         {
            if (m_isSchematic)
               block = m_schematicLink.getSchematicCamcadDoc().getBlockAt(itemData->getSheetBlockNumber());
            else
               block = m_schematicLink.getPcbCamcadDoc().getBlockAt(itemData->getSheetBlockNumber());

            name.Format("%s in %s", ((ComponentItem*)itemData)->getCompareName(), block->getName());

            int index = m_matchedListbox.AddString(name);
            m_matchedListbox.SetItemDataPtr(index, (void*)itemData);

         }
         else if (m_netItem !=  NULL)
         {
            if (m_isSchematic)
            {
               block = m_schematicLink.getSchematicCamcadDoc().getBlockAt(itemData->getSheetBlockNumber());
            }
            else
            {
               block = m_schematicLink.getPcbCamcadDoc().getBlockAt(itemData->getSheetBlockNumber());
            }

            name.AppendFormat("%s in %s", m_netItem->getName(), block->getName());

            int index = m_matchedListbox.FindString(-1, name);
            if (index < 0)
            {
               int index = m_matchedListbox.AddString(name);
               m_matchedListbox.SetItemDataPtr(index, (void*)itemData);
            }
         }
      }
   }
}

void CSelectMatchedDlg::addMatchItemData(ItemData* itemData)
{
   if (itemData != NULL && itemData->getDataStruct() != NULL)
   {
      m_matchedItemDataList.AddTail(itemData);
   }
}

void CSelectMatchedDlg::setNetItem(NetItem* netItem)
{
   m_netItem = netItem;
}

void CSelectMatchedDlg::OnLbnSelChangeMatchList()
{
   // TODO: Add your control notification handler code here

   int index = m_matchedListbox.GetCurSel();
   ItemData* itemData = (ItemData*)m_matchedListbox.GetItemDataPtr(index);
   if (itemData != m_lastSelectedItemData && itemData != NULL && itemData->getDataStruct() != NULL)
   {
      if (m_isComponent)
      {
         if (m_isSchematic)
         {
            m_schematicLink.zoomToComponent(m_schematicLink.getSchematicCamcadDoc(), m_schematicLink.getSchematicFileStruct(), 
                  *itemData->getDataStruct(), itemData->getSheetBlockNumber(), m_isSchematic);
         }
         else
         {
            m_schematicLink.zoomToComponent(m_schematicLink.getPcbCamcadDoc(), m_schematicLink.getPcbFileStruct(),
                  *itemData->getDataStruct(), itemData->getSheetBlockNumber(), m_isSchematic);
         }
      }
      else if (m_netItem !=  NULL)
      {
         if(m_isSchematic)
         {
            m_schematicLink.zoomToNet(m_schematicLink.getSchematicCamcadDoc(), m_schematicLink.getSchematicFileStruct(), 
                  *m_netItem, itemData->getSheetBlockNumber(), m_isSchematic);
         }
         else
         {
            m_schematicLink.zoomToNet(m_schematicLink.getPcbCamcadDoc(), m_schematicLink.getPcbFileStruct(), 
                  *m_netItem, itemData->getSheetBlockNumber(), m_isSchematic);
         }
      }
   }
}

