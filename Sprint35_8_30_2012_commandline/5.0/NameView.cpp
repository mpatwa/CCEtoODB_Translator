// $Header: /CAMCAD/5.0/NameView.cpp 22    12/10/06 3:56p Kurt Van Ness $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           
 
#include "StdAfx.h"
#include "ccdoc.h"
#include "nameview.h"

extern CView *activeView; // from CCVIEW.CPP

/******************************************************************************
* OnListNamedViews
*/
void CCEtoODBDoc::OnListNamedViews() 
{
   NamedViewDlg dlg;
   dlg.doc = this;
   dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// NamedViewDlg dialog
NamedViewDlg::NamedViewDlg(CWnd* pParent /*=NULL*/)
   : CResizingDialog(NamedViewDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(NamedViewDlg)
   //}}AFX_DATA_INIT

   addFieldControl(IDC_EXPAND,anchorBottom);
   addFieldControl(IDC_RECALL,anchorRight);
   addFieldControl(IDC_SAVE  ,anchorRight);
   addFieldControl(IDC_DELETE,anchorRight);

   addFieldControl(IDC_LIST1 ,anchorLeft ,growBoth);
}

void NamedViewDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(NamedViewDlg)
   DDX_Control(pDX, IDC_LIST1, m_listLB);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(NamedViewDlg, CResizingDialog)
   //{{AFX_MSG_MAP(NamedViewDlg)
   ON_BN_CLICKED(IDC_RECALL, OnRecall)
   ON_BN_CLICKED(IDC_SAVE, OnSave)
   ON_BN_CLICKED(IDC_EXPAND, OnExpand)
   ON_LBN_DBLCLK(IDC_LIST1, OnRecall)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NamedViewDlg message handlers
BOOL NamedViewDlg::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   POSITION pos = doc->getNamedViewList().GetHeadPosition();
   while (pos != NULL)
   {
      CNamedView *view = doc->getNamedViewList().GetNext(pos);
      m_listLB.SetItemDataPtr(m_listLB.AddString(view->getName()), view);
   }
   
   Expanded = FALSE;
   WINDOWPLACEMENT wp;
   GetWindowPlacement(&wp);
   width = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
   wp.showCmd = SW_SHOWNORMAL;
   wp.rcNormalPosition.right = wp.rcNormalPosition.left + width/2;
   SetWindowPlacement(&wp);   

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CCEtoODBView::recallView(CNamedView& namedView)
{
   ScaleNum   = namedView.getScaleNum();
   ScaleDenom = namedView.getScaleDenom();

   UpdateScale();

   SetXPos(namedView.getScrollX());
   SetYPos(namedView.getScrollY());

   GetDocument()->setLayerViewData(namedView);
   GetDocument()->UpdateAllViews(NULL);
}

void NamedViewDlg::OnRecall() 
{
   int sel = m_listLB.GetCurSel();
   if (sel == LB_ERR)
      return;

   CNamedView *view = (CNamedView*)m_listLB.GetItemDataPtr(sel);

   if (view != NULL)
   {
      ((CCEtoODBView*)activeView)->recallView(*view);
   }

   //((CCEtoODBView*)activeView)->ScaleNum = view->scaleNum;
   //((CCEtoODBView*)activeView)->ScaleDenom = view->scaleDenom;

   //((CCEtoODBView*)activeView)->UpdateScale();

   //((CCEtoODBView*)activeView)->SetXPos(view->scrollX);
   //((CCEtoODBView*)activeView)->SetYPos(view->scrollY);
   //
   //// set layers here
   //if (view->layerdata)
   //{
   // for (int i=0;i<view->layercnt;i++)
   // {
   //    if (doc->LayerArray[i]) // could have been deleted
   //    {
   //       doc->LayerArray[i]->color = view->layerdata[i].color;
   //       doc->LayerArray[i]->visible = view->layerdata[i].show;
   //    }
   // }
   //}

   //doc->UpdateAllViews(NULL);
}

void NamedViewDlg::OnDelete() 
{
   int sel = m_listLB.GetCurSel();
   if (sel == LB_ERR)
      return;

   CNamedView *view = (CNamedView*)m_listLB.GetItemDataPtr(sel);

   POSITION pos = doc->getNamedViewList().Find(view);
   if (pos)
   {
      doc->getNamedViewList().RemoveAt(pos);
      //free(view->layerdata);
      delete view;
      m_listLB.DeleteString(sel);
   }
}

void CCEtoODBDoc::getLayerViewData(CNamedView& namedView)
{
   for (int layerIndex = 0;layerIndex < getMaxLayerIndex();layerIndex++)
   {
      if (getLayerArray()[layerIndex] == NULL)
      {
         continue;
      }

      namedView.setAt(layerIndex,getLayerArray()[layerIndex]->getColor(),
         getLayerArray()[layerIndex]->isVisible());
   }
}

void CCEtoODBDoc::setLayerViewData(CNamedView& namedView)
{
   for (int layerIndex = 0;layerIndex < getMaxLayerIndex();layerIndex++)
   {
      if (getLayerArray()[layerIndex] == NULL) 
      {
         continue;
      }

      getLayerArray()[layerIndex]->setColor( namedView.getColorAt(layerIndex));
      getLayerArray()[layerIndex]->setVisible( namedView.getShowAt(layerIndex));
   }
}

void CCEtoODBView::getLayerViewData(CNamedView& namedView)
{
   namedView.setScaleNum(ScaleNum);
   namedView.setScaleDenom(ScaleDenom);
   namedView.setScrollX(GetXPos());
   namedView.setScrollY(GetYPos());

   GetDocument()->getLayerViewData(namedView);
}

CNamedView* CCEtoODBView::saveView(const CString& viewName)
{
   CNamedView* namedView = new CNamedView(viewName);

   getLayerViewData(*namedView);

   GetDocument()->getNamedViewList().AddTail(namedView);

   return namedView;
}

void NamedViewDlg::OnSave() 
{
   InputDlg dlg;
   dlg.m_prompt = "Enter Name for this View";
   if (dlg.DoModal() != IDOK)
      return;

   CString viewName = dlg.m_input;
   CNamedView *view = ((CCEtoODBView*)activeView)->saveView(viewName);
   m_listLB.SetItemDataPtr(m_listLB.AddString(viewName), view);

   //CNamedView *view = new CNamedView;
   //view->name = dlg.m_input;
   //m_listLB.SetItemDataPtr(m_listLB.AddString(view->name), view);
   //view->scaleNum = ((CCEtoODBView*)activeView)->ScaleNum;
   //view->scaleDenom = ((CCEtoODBView*)activeView)->ScaleDenom;
   //view->scrollX = ((CCEtoODBView*)activeView)->GetXPos();
   //view->scrollY = ((CCEtoODBView*)activeView)->GetYPos();

   //// store layers here
   //if ((view->layerdata = (CViewLayerData *)calloc(doc->getMaxLayerIndex(), sizeof(CViewLayerData))) != NULL)
   //{
   // view->layercnt = doc->getMaxLayerIndex();
   // for (int i=0;i<doc->getMaxLayerIndex();i++)
   // {
   //    if (doc->LayerArray[i] == NULL)  continue;
   //    view->layerdata[i].color = doc->LayerArray[i]->color;
   //    view->layerdata[i].show = doc->LayerArray[i]->visible;
   // }
   //}
   //doc->NamedViewList.AddTail(view);
}

void NamedViewDlg::OnExpand() 
{
   Expanded = !Expanded;

   WINDOWPLACEMENT wp;

   GetWindowPlacement(&wp);

   wp.showCmd = SW_SHOWNORMAL;

   if (Expanded)
      wp.rcNormalPosition.right = wp.rcNormalPosition.left + width;
   else
      wp.rcNormalPosition.right = wp.rcNormalPosition.left + width/2;

   SetWindowPlacement(&wp);   
}

