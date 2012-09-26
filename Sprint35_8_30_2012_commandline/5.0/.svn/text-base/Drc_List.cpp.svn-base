// $Header: /CAMCAD/5.0/Drc_List.cpp 44    6/30/07 2:26a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.
*/           

#include "stdafx.h"
#include <math.h>
#include "CCEtoODB.h"
#include "DRC_List.h"
#include "graph.h"
#include "draw.h"
#include "pcbutil.h"
#include "drc.h"
#include "attrib.h"
#include "find.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CView *activeView; // from CCVIEW.CPP
extern SelectNets *selectNetsDlg; // from NETS.CPP
extern char *DrcCodes[]; // from DRC.CPP
extern char *FailureRanges[]; // from DRC.CPP

static DRC_List *drcListDlg = NULL;

/******************************************************************************
* OnDrcList
*/
void CCEtoODBDoc::OnDrcList() 
{
   if (!drcListDlg)
   {
      drcListDlg = new DRC_List;
      drcListDlg->doc = this;
      drcListDlg->Create(IDD_LIST_DRC);
      drcListDlg->ShowWindow(SW_SHOW);
   }
}

/******************************************************************************
* CloseDRCList
*/
void CloseDRCList()
{
   if (drcListDlg)
      drcListDlg->OnCancel();
}

#define iFolder         0
#define iOpenFolder     1  
#define iDotHollow      2
#define iYellowDot      3
#define iRedDot         4
/////////////////////////////////////////////////////////////////////////////
// DRC_List dialog
DRC_List::DRC_List(CWnd* pParent /*=NULL*/)
   : CResizingDialog(DRC_List::IDD, pParent)
   , m_status(0)
{
   //{{AFX_DATA_INIT(DRC_List)
   m_tAlg = _T("");
   m_tString = _T("");
   m_tText = _T("");
   m_tXY = _T("");
   m_comment = _T("");
   m_count = _T("");
   m_hideDRCs = TRUE;
   m_hideTests = TRUE;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_HIDE_DRCS         ,anchorBottom);
   addFieldControl(IDC_HIDE_TESTS        ,anchorBottom);
   addFieldControl(IDC_PAN_TO            ,anchorBottom);
   addFieldControl(IDC_CLEAR_ALL         ,anchorBottom);
   addFieldControl(IDC_LOW               ,anchorBottomRight);
   addFieldControl(IDC_MEDIUM            ,anchorBottomRight);
   addFieldControl(IDC_HIGH              ,anchorBottomRight);
   addFieldControl(IDC_CLEAR             ,anchorBottomRight);

   CResizingDialogField *commentLabel = &(addFieldControl(IDC_COMMENT_LABEL ,anchorProportionalVertical, growStatic));
   CResizingDialogField *drcTextBox = &(addFieldControl(IDC_T_TEXT          ,anchorRight));
   CResizingDialogField *commentBox = &(addFieldControl(IDC_COMMENT_TEXTBOX ,anchorBottomRight, growVertical));

   commentLabel->getOperations().addOperation(anchorRight);
   drcTextBox->getOperations().addOperation(glueBottomEdge ,toTopEdge    ,commentLabel);
   commentBox->getOperations().addOperation(glueTopEdge    ,toBottomEdge ,commentLabel);

   addFieldControl(IDC_FILECB      ,anchorLeft  ,growHorizontal);
   addFieldControl(IDC_ALGORITHM_CB,anchorLeft  ,growHorizontal);

   addFieldControl(IDC_COUNT       ,anchorLeft  ,growHorizontal);
   addFieldControl(IDC_DRC_TREE    ,anchorLeft  ,growBoth);

   addFieldControl(IDC_T_ALG       ,anchorRight);
   addFieldControl(IDC_T_STRING    ,anchorRight);
   addFieldControl(IDC_T_XY        ,anchorRight);
   addFieldControl(IDC_T_ALG       ,anchorRight);
   addFieldControl(IDC_DRC_TREE    ,anchorLeft ,growBoth);
   

}

DRC_List::~DRC_List()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void DRC_List::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DRC_List)
   DDX_Control(pDX, IDC_ALGORITHM_CB, m_algorithmCB);
   DDX_Control(pDX, IDC_DRC_TREE, m_tree);
   DDX_Text(pDX, IDC_T_ALG, m_tAlg);
   DDX_Text(pDX, IDC_T_STRING, m_tString);
   DDX_Text(pDX, IDC_T_TEXT, m_tText);
   DDX_Text(pDX, IDC_T_XY, m_tXY);
   DDX_Text(pDX, IDC_COMMENT_TEXTBOX, m_comment);
   DDX_Text(pDX, IDC_COUNT, m_count);
   DDX_Check(pDX, IDC_HIDE_DRCS, m_hideDRCs);
   DDX_Check(pDX, IDC_HIDE_TESTS, m_hideTests);
   //}}AFX_DATA_MAP
   DDX_Control(pDX, IDC_FILECB, m_fileCB);
   DDX_Radio(pDX, IDC_LOW, m_status);
}

BEGIN_MESSAGE_MAP(DRC_List, CResizingDialog)
   //{{AFX_MSG_MAP(DRC_List)
   ON_BN_CLICKED(IDC_PAN_TO, OnPanTo)
   ON_BN_CLICKED(IDC_CLEAR, OnClear)
   ON_NOTIFY(TVN_SELCHANGED, IDC_DRC_TREE, OnSelchangedDrcTree)
   ON_BN_CLICKED(IDC_CLEAR_ALL, OnClearAll)
   ON_NOTIFY(NM_DBLCLK, IDC_DRC_TREE, OnDblclkDrcTree)
   ON_EN_CHANGE(IDC_COMMENT_TEXTBOX, OnCommentChanged)
   ON_CBN_SELCHANGE(IDC_ALGORITHM_CB, FillTree)
   ON_BN_CLICKED(IDC_HIDE_DRCS, OnHideDrcs)
   ON_BN_CLICKED(IDC_HIDE_TESTS, OnHideTests)
   //}}AFX_MSG_MAP
   ON_CBN_SELCHANGE(IDC_FILECB, OnCbnSelchangeFilecb)
   ON_BN_CLICKED(IDC_LOW, OnStatusChanged)
   ON_BN_CLICKED(IDC_MEDIUM, OnStatusChanged)
   ON_BN_CLICKED(IDC_HIGH, OnStatusChanged)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DRC_List message handlers

BOOL DRC_List::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      int sel;
      switch (nID)
      {
      case IDC_ALGORITHM_CB:
         sel = m_algorithmCB.GetCurSel();
         if (sel != LB_ERR)
         {
            int i = m_algorithmCB.GetItemData(sel);
            hint = doc->getDfmAlgorithmNamesArray()[i];
            pTTT->lpszText = hint.GetBuffer(0);
         }
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* OnInitDialog
*/
BOOL DRC_List::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   Expanded = FALSE;
   WINDOWPLACEMENT wp;
   GetWindowPlacement(&wp);
   width = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
   wp.showCmd = SW_SHOWNORMAL;
   wp.rcNormalPosition.right = wp.rcNormalPosition.left + width/2;
   SetWindowPlacement(&wp);   

   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_ALGORITHM_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 6, 1);
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_DOT_HOLLOW));
   m_imageList->Add(app->LoadIcon(IDI_YELLOW_DOT));
   m_imageList->Add(app->LoadIcon(IDI_RED_DOT));
   
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getDRCList().IsEmpty())
         continue;

      m_fileCB.SetItemDataPtr(m_fileCB.AddString(file->getName()), file);
   }

   curFile = NULL;
   SelectVisibleFile();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
* OnCancel
*/
void DRC_List::OnCancel() 
{
/* doc->OnSelectNets();    // need to open first
   if (selectNetsDlg)      // clear the last hilited net, which some DRCs do.
   {
      selectNetsDlg->OnClear();
      selectNetsDlg->OnCancel();
   }*/

   ShowAllDRCs(doc, TRUE);
   doc->UpdateAllViews(NULL);

   DestroyWindow();  
}

/******************************************************************************
* PostNcDestroy
*/
void DRC_List::PostNcDestroy() 
{
   delete m_imageList;
   delete this;
   drcListDlg = NULL;   
   CResizingDialog::PostNcDestroy();
}

/******************************************************************************
* FillAlgorithms
*/
void DRC_List::FillAlgorithms() 
{
   m_algorithmCB.ResetContent();
   m_tree.DeleteAllItems();

   CWordArray arr;

   if (m_hideTests)
   {
      arr.SetSize(doc->getDfmAlgorithmArraySize());
      for (int i=0; i<doc->getDfmAlgorithmArraySize(); i++)
         arr[i] = FALSE;

      if (curFile)
      {
         POSITION drcPos = curFile->getDRCList().GetHeadPosition();
         while (drcPos)
         {
            DRCStruct *drc = curFile->getDRCList().GetNext(drcPos);

            arr[drc->getAlgorithmIndex()] = TRUE;
         }
      }
   }

   for (int i=0; i<doc->getDfmAlgorithmArraySize(); i++)
      if (!m_hideTests || arr[i])
         m_algorithmCB.SetItemData(m_algorithmCB.AddString(doc->getDfmAlgorithmNamesArray()[i]), i);
}

/******************************************************************************
* FillTree
*/
void DRC_List::FillTree() 
{
   CWaitCursor wait;

   UpdateData();

   m_tree.DeleteAllItems();
   m_comment = "";
   m_count = "";
   UpdateData(FALSE);

   int sel = m_algorithmCB.GetCurSel();
   if (sel == CB_ERR)
      return;
   int algorithm = m_algorithmCB.GetItemData(sel);

   HTREEITEM rootArray[3];
   rootArray[0] = rootArray[1] = rootArray[2] = NULL;
   int marginalCount = 0, criticalCount = 0;

   if (curFile)
   {
      POSITION drcPos = curFile->getDRCList().GetHeadPosition();
      while (drcPos)
      {
         DRCStruct *drc = curFile->getDRCList().GetNext(drcPos);

         if (drc->getAlgorithmIndex() != algorithm)
            continue;

         int icon;
         switch (drc->getPriority())
         {
         case 0:
            icon = iDotHollow;
            break;
         case 1:
            icon = iYellowDot;
            break;
         case 2:
            icon = iRedDot;
            break;
         }

         if (!rootArray[drc->getFailureRange()])
         {
            HTREEITEM insertAfter;

            switch (drc->getFailureRange())
            {
            case 0:
               insertAfter = TVI_FIRST;
               break;
            case 1:
               if (rootArray[0])
                  insertAfter = rootArray[0];
               else
                  insertAfter = TVI_FIRST;
               break;
            case 2:
               insertAfter = TVI_LAST;
               break;
            }

            rootArray[drc->getFailureRange()] = m_tree.InsertItem(FailureRanges[drc->getFailureRange()], iFolder, iOpenFolder, TVI_ROOT, insertAfter); 
            m_tree.SetItemData(rootArray[drc->getFailureRange()], NULL);
         }

         HTREEITEM item = m_tree.InsertItem(drc->getString(), icon, icon, rootArray[drc->getFailureRange()], TVI_SORT);
         m_tree.SetItemData(item, (DWORD)drc);
         if (drc->getFailureRange())
            marginalCount++;
         else
            criticalCount++;
      }
   }

   m_count.Format("Critical = %d,  Marginal = %d", criticalCount, marginalCount);
   UpdateData(FALSE);
}

/******************************************************************************
* DRC_List::OnCbnSelchangeFilecb
*/
void DRC_List::OnCbnSelchangeFilecb()
{
   curFile = (FileStruct*)m_fileCB.GetItemDataPtr(m_fileCB.GetCurSel());

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      file->setShow(file == curFile);
   }

   doc->OnFitPageToImage();

   FillAlgorithms();
}

/******************************************************************************
* DRC_List::OnHideTests
*/
void DRC_List::OnHideTests() 
{
   int algIndex = m_algorithmCB.GetItemData(m_algorithmCB.GetCurSel());

   UpdateData();

   FillAlgorithms();

   SelectAlgorithm(algIndex);
}

/******************************************************************************
* DRC_List::OnHideDrcs
*/
void DRC_List::OnHideDrcs() 
{
   UpdateData();

   ShowAllDRCs(doc, !m_hideDRCs);

   doc->UpdateAllViews(NULL);
}

/******************************************************************************
* GetSelectedDRC 
*/
DRCStruct* DRC_List::GetSelectedDRC()
{
   HTREEITEM item = m_tree.GetSelectedItem();
   
   if (item == NULL) 
      return NULL;

   DRCStruct *drc = (DRCStruct*)m_tree.GetItemData(item); 

   return drc;
}

/******************************************************************************
* OnSelchangedDrcTree
*/
void DRC_List::OnSelchangedDrcTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

   *pResult = 0;

   HTREEITEM item; 

   m_comment = "";

   if (pNMTreeView->itemNew.state & TVIS_SELECTED)
   {
      item = pNMTreeView->itemNew.hItem;

      DRCStruct *drc = (DRCStruct*)m_tree.GetItemData(item); 

      if (!drc)
         return;

      m_tAlg.Format("Algorithm = %s", doc->getDfmAlgorithmNamesArray()[drc->getAlgorithmIndex()]);
      m_tString.Format("DRC = %s", drc->getString());
      int decimals = GetDecimals(doc->getSettings().getPageUnits());
      m_tXY.Format("Location = ( %.*lf , %.*lf )", decimals, drc->getOrigin().x, decimals, drc->getOrigin().y);
      m_tText = "";
      m_comment = drc->getComment();
      m_status = drc->getPriority();

      if (drc->getAttributesRef())
      {
         for (POSITION pos = drc->getAttributesRef()->GetStartPosition();pos != NULL;)
         {
            WORD keyword;
            Attrib* attrib;

            drc->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

            m_tText += doc->getKeyWordArray()[keyword]->cc;
            m_tText += " -> ";

            switch (attrib->getValueType())
            {
            case VT_STRING:
            case VT_EMAIL_ADDRESS:
            case VT_WEB_ADDRESS:
               m_tText += attrib->getStringValue();
               break;
            case VT_INTEGER:
               {
                  CString buf;
                  buf.Format("%d", attrib->getIntValue());
                  m_tText += buf;
               }
               break;
            case VT_DOUBLE:
            case VT_UNIT_DOUBLE:
               {
                  CString buf;
                  buf.Format("%.*lf", decimals, attrib->getDoubleValue());
                  m_tText += buf;
               }
               break;
            }

            m_tText += "\n";
         }
      }
   }

   UpdateData(FALSE);
}

/******************************************************************************
* DRC_List::OnDblclkDrcTree
*/
void DRC_List::OnDblclkDrcTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnPanTo();
   
   *pResult = 0;
}

/******************************************************************************
* OnPanTo
*/
void DRC_List::OnPanTo() 
{
   DRCStruct *drc = GetSelectedDRC();
   if (!drc)
      return;

   if (!curFile->isShown())
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);
         file->setShow(file == curFile);
      }

      doc->OnFitPageToImage();
   }

   FlushQueue();

   UpdateData();
   if (m_hideDRCs)
   {
      ShowAllDRCs(doc, FALSE);
      DataStruct *data = FindDataEntity(doc, drc->getInsertEntityNumber(), NULL, NULL);
      if (data)
         data->setHidden(false);
   }

   switch (drc->getDrcClass())
   {
   case DRC_CLASS_NO_MARKER:
      break;

   case DRC_CLASS_SIMPLE:
		panToSelectedDRC();
      break;

   case DRC_CLASS_MEASURE:
		panToSelectedDRC();
      FlushQueue();
      DrawDRCArrow(drc);
      break;

   case DRC_CLASS_NETS:
      {
         if (!drc->getVoidPtr())
            return;

         CStringList *stringList = (CStringList*)drc->getVoidPtr();

         doc->OnSelectNets();
         if (!selectNetsDlg)
            return;
         selectNetsDlg->OnClear();

         if (stringList->IsEmpty())
            return;

         selectNetsDlg->UpdateData();
         selectNetsDlg->m_show_selected_only = TRUE;
         selectNetsDlg->UpdateData(FALSE);
         int i=0;
         POSITION pos = stringList->GetHeadPosition();
         while (pos)
         {
            CString string = stringList->GetNext(pos);

            if (curFile)
            {
               POSITION netPos = curFile->getNetList().GetHeadPosition();
               while (netPos)
               {
                  NetStruct *net = curFile->getNetList().GetNext(netPos);
                  if (!string.Compare(net->getNetName()))
                  {
                     selectNetsDlg->MarkNet(net, -1); // original color
   /*                   switch(i++%6) // every net gets next color
                     {
                        case 0:
                           selectNetsDlg->MarkNet(net, RGB(255,0,0)); // RED
                        break;

                        case 1:
                           selectNetsDlg->MarkNet(net, RGB(255,255,0)); // YELLOW
                        break;

                        case 2:
                           selectNetsDlg->MarkNet(net, RGB(0,255,0)); // GREEN
                        break;

                        case 3:
                           selectNetsDlg->MarkNet(net, RGB(0,255,255)); // CYAN
                        break;

                        case 4:
                           selectNetsDlg->MarkNet(net, RGB(0,0,255)); // BLUE
                        break;

                        case 5:
                           selectNetsDlg->MarkNet(net, RGB(255,0,255)); // MAGENTA
                        break;
                     } // end switch(color) */
                  }
               }
            }
         }

         selectNetsDlg->OnZoomTo();
      }     
      break;
   }   
}

void DRC_List::panToSelectedDRC()
{
   DRCStruct *drc = GetSelectedDRC();

	double fileRot = curFile->getRotation();
	int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (curFile->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

	Point2 pnt;
	pnt.x = drc->getOrigin().x * curFile->getScale();
	pnt.y = drc->getOrigin().y * curFile->getScale();
	if (fileMirror & MIRROR_FLIP)
	{
		pnt.x = -pnt.x;
		fileRot = -fileRot;
	}

	Mat2x2 m;
	RotMat2(&m, fileRot);
	TransPoint2(&pnt, 1, &m, curFile->getInsertX(), curFile->getInsertY());

	((CCEtoODBView*)(activeView))->PanCoordinate(pnt.x, pnt.y, TRUE);
}

/******************************************************************************
* DrawDRCArrow
*/
void DRC_List::DrawDRCArrow(DRCStruct *drc)
{
   if (!drc->getVoidPtr())
      return;

   if (drc->getDrcClass() != DRC_CLASS_MEASURE)
      return;

   DRC_MeasureStruct *meas = (DRC_MeasureStruct*)drc->getVoidPtr();

   CCEtoODBView *view = (CCEtoODBView*)activeView;

   double mult = 1.0;
   if (doc->getBottomView())
      mult = -1.0;

   double dx1 = drc->getOrigin().x - meas->x1;
   double dy1 = drc->getOrigin().y - meas->y1;
   double dx2 = drc->getOrigin().x - meas->x2;
   double dy2 = drc->getOrigin().y - meas->y2;
   double size1 = 0.5 * sqrt(dx1*dx1 + dy1*dy1);
   double size2 = 0.5 * sqrt(dx2*dx2 + dy2*dy2);

   if (size1 > doc->getSettings().DRC_MAX_ARROWSIZE)
      size1 = doc->getSettings().DRC_MAX_ARROWSIZE;
   if (size2 > doc->getSettings().DRC_MAX_ARROWSIZE)
      size2 = doc->getSettings().DRC_MAX_ARROWSIZE;
   if (size1 < doc->getSettings().DRC_MIN_ARROWSIZE)
      size1 = doc->getSettings().DRC_MIN_ARROWSIZE;
   if (size2 < doc->getSettings().DRC_MIN_ARROWSIZE)
      size2 = doc->getSettings().DRC_MIN_ARROWSIZE;

   double arrowheadSize = min(size1, size2);

   if (arrowheadSize < SMALLNUMBER)
      return;

   CClientDC dc(view);
   //view->OnPrepareDC(&dc);

   COLORREF color = RGB(255, 255, 255);
   HPEN pen = CreatePen(PS_SOLID, 0, color);
   HPEN origPen = (HPEN)SelectObject(dc.m_hDC, pen);
   DrawArrow(&dc, mult*drc->getOrigin().x, drc->getOrigin().y, mult*meas->x1, meas->y1, view->getScaleFactor(), arrowheadSize, color);
   DrawArrow(&dc, mult*drc->getOrigin().x, drc->getOrigin().y, mult*meas->x2, meas->y2, view->getScaleFactor(), arrowheadSize, color);

   DeleteObject(SelectObject(dc.m_hDC, origPen));
}

/******************************************************************************
* DRC_List::OnCommentChanged
*/
void DRC_List::OnCommentChanged() 
{
   UpdateData();

   DRCStruct *drc = GetSelectedDRC();
   if (!drc)
   {
      m_comment = "";
      UpdateData(FALSE);
      return;
   }

   drc->setComment(m_comment);  
}
   
/******************************************************************************
* DRC_List::OnStatusChanged
*/
void DRC_List::OnStatusChanged() 
{
   UpdateData();

   DRCStruct *drc = GetSelectedDRC();
   if (!drc)
      return;

   drc->setPriority(m_status);  

   HTREEITEM item = m_tree.GetSelectedItem();
   int icon;

   switch (drc->getPriority())
   {
   case 0:
      icon = iDotHollow;
      break;
   case 1:
      icon = iYellowDot;
      break;
   case 2:
      icon = iRedDot;
      break;
   }

   m_tree.SetItem(item, TVIF_IMAGE | TVIF_SELECTEDIMAGE, NULL, icon, icon, 0, 0, 0);
}
   
/******************************************************************************
* DRC_List::SelectAlgorithm
*/
void DRC_List::SelectAlgorithm(int algIndex)
{
   int count = m_algorithmCB.GetCount();
   for (int i=0; i<count; i++)
   {
      if (m_algorithmCB.GetItemData(i) == algIndex)
      {
         m_algorithmCB.SetCurSel(i);
         FillTree();
         return;
      }
   }
}

/******************************************************************************
* Clear
*/
void DRC_List::OnClear() 
{
   if (ErrorMessage("Are you sure?", "Remove this DRC", MB_YESNO) != IDYES)
      return;

   DRCStruct *drc = GetSelectedDRC();

   if (!RemoveOneDRC(doc, drc, curFile))
   {
      // remove from tree
      HTREEITEM item = m_tree.GetSelectedItem();
      m_tree.DeleteItem(item);
   }
}

/******************************************************************************
* DRC_List::OnClearAll
*/
void DRC_List::OnClearAll() 
{
   if (ErrorMessage("Are you sure?", "Remove all DRC's", MB_YESNO) != IDYES)
      return;

   ClearDRCList(doc);

   doc->getDfmAlgorithmNamesArray().RemoveAll();
   //doc->getDfmAlgorithmArraySize() = 0;

   FillTree();
}


/******************************************************************************
* SelectDRCMarker
*
*  - Called by SELECT.CPP to select a DRC in the DRC LIST
*/
void SelectDRCMarker(CCEtoODBDoc *doc, int fileNum, int entityNum)
{
   if (!drcListDlg)
      return;

   FileStruct *file = doc->Find_File(fileNum);

   POSITION drcPos = file->getDRCList().GetHeadPosition();
   while (drcPos != NULL)
   {
      DRCStruct *drc = file->getDRCList().GetNext(drcPos);

      if (drc->getInsertEntityNumber() == entityNum)
      {
         drcListDlg->SelectDRC(file, drc);
         return;
      }
   }
}

/******************************************************************************
* DRC_List::SelectDRC
*/
void DRC_List::SelectDRC(FileStruct *file, DRCStruct *drc)
{
   if (curFile != file)
   {
      curFile = file;

      int count = m_fileCB.GetCount();
      for (int i=0; i<count; i++)
      {
         FileStruct *f = (FileStruct*)m_fileCB.GetItemDataPtr(i);

         if (f == file)
         {
            m_fileCB.SetCurSel(i);
            break;
         }
      }

      FillAlgorithms();
   }

   SelectAlgorithm(drc->getAlgorithmIndex());

   HTREEITEM root = m_tree.GetRootItem();
   while (root)
   {
      HTREEITEM child = m_tree.GetChildItem(root);
      while (child)
      {
         DRCStruct *dp = (DRCStruct*)m_tree.GetItemData(child); 
         if (dp == drc)
         {
            m_tree.Select(child, TVGN_CARET);
            break;
         }
      
         child = m_tree.GetNextSiblingItem(child);
      }

      root = m_tree.GetNextSiblingItem(root);
   }
}

/******************************************************************************
* DRC_List::SelectVisibleFile
*/
void DRC_List::SelectVisibleFile()
{
   int count = m_fileCB.GetCount();

   for (int i=0; i<count; i++)
   {
      FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(i);

      if (file->isShown())
      {
         curFile = file;

         m_fileCB.SetCurSel(i);

         FillAlgorithms();

         return;
      }
   }
}

/******************************************************************************
* UpdateDRCFile
*/
void UpdateDRCFile()
{
   if (!drcListDlg)
      return;

   drcListDlg->SelectVisibleFile();
}
