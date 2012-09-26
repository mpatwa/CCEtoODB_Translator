
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "attrib.h"
#include "crypt.h"
#include "net_hilt.h"
#include "net_util.h"
#include "draw.h"
#include "pcbutil.h"
#include "extents.h"
#include "PersistantColorDialog.h"
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CView   *activeView; // from CCVIEW.CPP

SelectNets *selectNetsDlg = NULL;

// I considered adding this to CCADCADDoc, alongside all those "show" vars.
// But this is not used in the Draw method, and isn't really a global state.
// Actual viewable state is controlled per item (apparantly) with the "hidden"
// var, and this is only to get the GUI checkbox back to how the user left it.
// The dialog itself comes and goes, so the member var won't keep the value.
static int showSelectedNetsOnly = false;

void HighlightPins(CDC *pDC, CCEtoODBView *view);

/******************************************************************************
* ColorNet
*/
void CCEtoODBDoc::ColorNet(NetStruct *net, COLORREF color)
{
   WORD netnameString = IsKeyWord(ATT_NETNAME, TRUE);
   int value = RegisterValue(net->getNetName());

   // set color in HighlightedNetsMap
   HighlightedNetsMap.SetAt(value, color);
   net->setHighlighted(true);

   // draw pins
   CCEtoODBView *view;
   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      view = (CCEtoODBView *)GetNextView(pos);

      CClientDC dc(view);
      //view->OnPrepareDC(&dc);

      dc.SetTextAlign(TA_CENTER);
      dc.SetBkColor(color);
      dc.SetBkMode(TRANSPARENT);

      InitPens(&dc, getSettings().FillStyle);
      HighlightPins(&dc, view);
      FreePens(&dc);
   }

   // draw net entities
   HighlightByAttrib(FALSE, netnameString, VT_STRING, value);
}

/******************************************************************************
* OnSelectNets
*/
void CCEtoODBDoc::OnSelectNets() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   if (selectNetsDlg)
      selectNetsDlg->Refresh();
   else
   {
      selectNetsDlg = new SelectNets;

      selectNetsDlg->doc = this;
		selectNetsDlg->m_show_selected_only = showSelectedNetsOnly;
		selectNetsDlg->Create(IDD_SELECT_NETS);
		selectNetsDlg->ShowWindow(SW_SHOW);
   }
   return;
}

/////////////////////////////////////////////////////////////////////////////
// SelectNets dialog
SelectNets::SelectNets(CWnd* pParent /*=NULL*/)
   : CDialog(SelectNets::IDD, pParent)
{
   //{{AFX_DATA_INIT(SelectNets)
   m_show_selected_only = FALSE;
   //}}AFX_DATA_INIT
}

void SelectNets::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectNets)
   DDX_Control(pDX, IDC_NETS, m_netLB);
   DDX_Control(pDX, IDC_FILE, m_fileCB);
   DDX_Check(pDX, IDC_NET_SHOW_SELECTED_ONLY, m_show_selected_only);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectNets, CDialog)
   //{{AFX_MSG_MAP(SelectNets)
   ON_CBN_SELCHANGE(IDC_FILE, OnSelchangeFile)
   ON_BN_CLICKED(IDC_CLEAR, OnClear)
   ON_WM_DRAWITEM()
   ON_BN_CLICKED(IDC_ZOOM_TO, OnZoomTo)
   ON_COMMAND(ID_NET_OFF, OnNetUnhighlight)
   ON_COMMAND(ID_NET_WHITE, OnNetWhite)
   ON_COMMAND(ID_NET_RED, OnNetRed)
   ON_COMMAND(ID_NET_ORANGE, OnNetOrange)
   ON_COMMAND(ID_NET_YELLOW, OnNetYellow)
   ON_COMMAND(ID_NET_GREEN, OnNetGreen)
   ON_COMMAND(ID_NET_LTBLUE, OnNetLtBlue)
   ON_COMMAND(ID_NET_BLUE, OnNetBlue)
   ON_COMMAND(ID_NET_PURPLE, OnNetPurple)
   ON_COMMAND(ID_NET_CUSTOM, OnNetCustom)
   ON_COMMAND(ID_NET_ORIG, OnNetOrig)
   ON_BN_CLICKED(IDC_NET_SHOW_SELECTED_ONLY, OnNetShowSelectedOnly)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_REFRESH, Refresh)
   ON_BN_CLICKED(IDC_COLORS, OnColors)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectNets message handlers
void SelectNets::PostNcDestroy() 
{
   delete this;
   selectNetsDlg = NULL;   
   CDialog::PostNcDestroy();
}

void SelectNets::OnCancel() 
{
   DestroyWindow();  
}

BOOL SelectNets::OnInitDialog() 
{
   CDialog::OnInitDialog();

   AutoColor = FALSE;
   Refresh();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectNets::Refresh() // re-fill CB & LB with current view's doc
{
   doc = (CCEtoODBDoc*)activeView->GetDocument();

   FillFileCB();
   
   FillNetLB();
}

void SelectNets::FillFileCB() 
{
   m_fileCB.ResetContent();

   POSITION pos = doc->getFileList().GetTailPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetPrev(pos);

      if (!file->isShown() || file->getNetList().IsEmpty())
         continue;

      int i = m_fileCB.AddString(file->getName());
      m_fileCB.SetItemDataPtr(i, file); // add filename to SelectNets Dlg once so it doesn't always refresh
      m_fileCB.SetCurSel(i);
   }

   if (!m_fileCB.GetCount())
      ErrorMessage("No Visible Files have Nets", "No Nets");
}

void SelectNets::FillNetLB() 
{
   m_netLB.ResetContent();
   int sel = m_fileCB.GetCurSel();
   if (sel == LB_ERR) return;
   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(sel);
   NetStruct *net;
   int i;

   POSITION pos = file->getNetList().GetHeadPosition();
   while (pos != NULL)
   {
      net = file->getNetList().GetNext(pos);

      i = m_netLB.AddString(net->getNetName());
      m_netLB.SetItemDataPtr(i, net);

      m_netLB.SetSel(i, net->isHighlighted());
   }

   // Find the longest string in the list box.
   CString str;
   CSize   size;
   width = 0;
   CDC*    dc = m_netLB.GetDC();
   for (i=0;i < m_netLB.GetCount();i++)
   {
      m_netLB.GetText(i, str);
      size = dc->GetTextExtent(str);
   
      if (size.cx > width)
         width = size.cx;
   }
   m_netLB.ReleaseDC(dc);

   // Set the horizontal extent so every character of all strings 
   // can be scrolled to.
   m_netLB.SetHorizontalExtent(width);
}

void SelectNets::OnSelchangeFile() 
{
   FillNetLB();
}

void SelectNets::OnClear() 
{
	if (m_show_selected_only)
		Hide_Nets_All(doc, TRUE);
	else
		Hide_Nets_All(doc, FALSE);


   UnhighlightNets();
   m_netLB.SetSel(-1, FALSE);
   doc->UpdateAllViews(NULL);

	Refresh();
}

void SelectNets::UnhighlightNets() 
{
   doc->HighlightedNetsMap.RemoveAll();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
         file->getNetList().GetNext(netPos)->setHighlighted(false);
   }

   m_netLB.Invalidate();
}

/* MarkNet()
*     used to mark net in dlg when Highlight Selected Item's Net
*/
void SelectNets::MarkNet(NetStruct *Net, COLORREF color) 
{
   net = Net;
   ColorNet(color);

   int count = m_netLB.GetCount();
   for (int i=0; i<count; i++)
   {
      if (m_netLB.GetItemDataPtr(i) == Net)
      {
         m_netLB.SetTopIndex(i);
         break;
      }
   }
}

#define MAX_ZOOM 10
void SelectNets::OnZoomTo() 
{
   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

   POSITION pos = doc->HighlightedNetsMap.GetStartPosition();
   while (pos)
   {
      int net;
      COLORREF color;
      doc->HighlightedNetsMap.GetNextAssoc(pos, net, color);
     
      double left, right, top, bottom;
      NetExtents(doc, doc->getCamCadData().getAttributeValueDictionary().getAt(net), &left, &right, &bottom, &top);

      if (left < xmin) xmin = left;
      if (right > xmax) xmax = right;
      if (bottom < ymin) ymin = bottom;
      if (top > ymax) ymax = top;
   }

   if (xmax > (xmin - SMALLNUMBER))
   {
      double zoomBoxSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / MAX_ZOOM;
      double zoomBoxSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) / MAX_ZOOM;

      double xMargin, yMargin;

      if (xmax - xmin < zoomBoxSizeX)
         xMargin = (zoomBoxSizeX - (xmax - xmin)) /2;
      else
         xMargin = (xmax - xmin) * doc->getSettings().ExtentMargin / 100;

      if (ymax - ymin < zoomBoxSizeY)
         yMargin = (zoomBoxSizeY - (ymax - ymin)) /2;
      else
         yMargin = (ymax - ymin) * doc->getSettings().ExtentMargin / 100;

      xmin -= xMargin;
      xmax += xMargin;

      ymin -= yMargin;
      ymax += yMargin;

      CCEtoODBView *view = (CCEtoODBView*)activeView;
      view->ZoomBox(xmin, xmax, ymin, ymax);
   }
}

void SelectNets::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT di) 
{
   if (nIDCtl == IDC_NETS)
   {
      if (!di->itemData)
         return;

      net = (NetStruct*)di->itemData;

      switch (di->itemAction)
      {
         case ODA_SELECT:
         {
            CMenu popup;
            popup.CreatePopupMenu();
            popup.AppendMenu(0, ID_NET_OFF, "&Not Highlighted");
            popup.AppendMenu(MF_SEPARATOR);
            popup.AppendMenu(0, ID_NET_WHITE, "&White");
            popup.AppendMenu(0, ID_NET_RED, "&Red");
            popup.AppendMenu(0, ID_NET_ORANGE, "&Orange");
            popup.AppendMenu(0, ID_NET_YELLOW, "&Yellow");
            popup.AppendMenu(0, ID_NET_GREEN, "&Green");
            popup.AppendMenu(0, ID_NET_LTBLUE, "&Lt Blue");
            popup.AppendMenu(0, ID_NET_BLUE, "&Blue");
            popup.AppendMenu(0, ID_NET_PURPLE, "&Purple");
            popup.AppendMenu(MF_SEPARATOR);
            popup.AppendMenu(0, ID_NET_CUSTOM, "&Custom Color...");
            popup.AppendMenu(MF_SEPARATOR);
            popup.AppendMenu(0, ID_NET_ORIG, "&Original Color");

            m_netLB.SetCaretIndex(di->itemID);
            CRect ctrlRect;
            m_netLB.GetWindowRect(ctrlRect);
            popup.TrackPopupMenu(TPM_LEFTALIGN, ctrlRect.right, ctrlRect.top + di->rcItem.top, this);
            
            return;
         } // end SELECT

         case ODA_DRAWENTIRE:
         default:
         {
            COLORREF color;
            if (!doc->HighlightedNetsMap.Lookup(doc->RegisterValue(net->getNetName()), color))
               color = RGB(0, 0, 0);

            if (color == -1)
               SetTextColor(di->hDC, RGB(255, 255, 255));
            else if (GetRValue(color) + GetGValue(color) + GetBValue(color) < 250)
               SetTextColor(di->hDC, RGB(255, 255, 255));
            else
               SetTextColor(di->hDC, RGB(0, 0, 0));

            SetTextAlign(di->hDC, TA_LEFT);
            SetBkMode(di->hDC, OPAQUE);
            SetBkColor(di->hDC, color);

            CBrush brush;
            if (color == -1)
               brush.CreateHatchBrush(HS_DIAGCROSS, RGB(255, 255, 255));
            else
               brush.CreateSolidBrush(color);

            HBRUSH orgBrush = (HBRUSH)SelectObject(di->hDC, brush);
            CPen pen;
            pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            HPEN orgPen = (HPEN)SelectObject(di->hDC, pen);

            int right = di->rcItem.right;
            int scroll = m_netLB.GetScrollPos(SB_HORZ);
            if (scroll < m_netLB.GetScrollLimit(SB_HORZ))
               right += 10;

            Rectangle(di->hDC, di->rcItem.left, di->rcItem.top, right, di->rcItem.bottom);

            TextOut(di->hDC, di->rcItem.left + 10, di->rcItem.top + 1, net->getNetName(), net->getNetName().GetLength());

            DeleteObject(SelectObject(di->hDC, orgBrush));
            DeleteObject(SelectObject(di->hDC, orgPen));
            
            return;
         } // end DRAWENTIRE
      } // end switch action
   } 
   
   CDialog::OnDrawItem(nIDCtl, di);
}

void SelectNets::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   m_netLB.Invalidate();
   
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void SelectNets::OnNetUnhighlight() 
{
   WORD netnameString = doc->IsKeyWord(ATT_NETNAME, TRUE);
   int value = doc->RegisterValue(net->getNetName());

   doc->HighlightedNetsMap.RemoveKey(value);
   net->setHighlighted(false);

   CCEtoODBView *view;
   POSITION viewPos = doc->GetFirstViewPosition();
   while (viewPos != NULL)
   {
      view = (CCEtoODBView *)doc->GetNextView(viewPos);

      CClientDC dc(view);
      //view->OnPrepareDC(&dc);

      dc.SetTextAlign(TA_CENTER);
      dc.SetBkColor(RGB(0,0,0));
      dc.SetBkMode(TRANSPARENT);

      InitPens(&dc, doc->getSettings().FillStyle);

      FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(m_fileCB.GetCurSel());
      Mat2x2 m;
      RotMat2(&m, file->getRotation());

      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos)
      {
         CompPinStruct *cp = net->getNextCompPin(cpPos);
         if (!cp->getPinCoordinatesComplete() || cp->getPadstackBlockNumber() < 0)
            continue;

         Point2 point2;
         point2.x = cp->getOriginX() * file->getScale();
         point2.y = cp->getOriginY() * file->getScale();
         if (file->getResultantMirror(doc->getBottomView())) point2.x = -point2.x;
         TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

         BlockStruct *block = doc->getBlockAt(cp->getPadstackBlockNumber());
         view->DrawBlock(&dc, block, point2.x, point2.y, cp->getRotationRadians(), cp->getMirror(), 1, -1, 0, 1, FALSE, 0, FALSE);
      }

      FreePens(&dc);
   }

   doc->HighlightByAttrib(FALSE, netnameString, VT_STRING, value);

   m_netLB.Invalidate();

   do_show_selected_only();
}

void SelectNets::do_show_selected_only()
{
	showSelectedNetsOnly = m_show_selected_only;

   if (m_show_selected_only)
   {
      // hide all nets excepted selected ones
      Hide_Nets_All(doc, TRUE);

      POSITION pos = doc->HighlightedNetsMap.GetStartPosition();  // previous selected
      while (pos)
      {
         int netValueIndex;
         COLORREF color;
         doc->HighlightedNetsMap.GetNextAssoc(pos, netValueIndex, color);
         Hide_Net(doc, netValueIndex, FALSE);
      }
      //Hide_Net(doc, net->getNetName(), FALSE);  // current selected
   }
   else
   {
      // unhide all nets
      Hide_Nets_All(doc, FALSE);
   }
   doc->UpdateAllViews(NULL);
}

void SelectNets::ColorNet(COLORREF color) 
{
   doc->ColorNet(net, color);
   do_show_selected_only();

   m_netLB.Invalidate();
}

void SelectNets::OnNetWhite()    { ColorNet( RGB (255, 255, 255) ); }
void SelectNets::OnNetRed()      { ColorNet( RGB (255,   0,   0) ); }
void SelectNets::OnNetOrange()   { ColorNet( RGB (255, 128,   0) ); }
void SelectNets::OnNetYellow()   { ColorNet( RGB (255, 255,   0) ); }
void SelectNets::OnNetGreen()    { ColorNet( RGB (  0, 255,   0) ); }
void SelectNets::OnNetLtBlue()   { ColorNet( RGB (  0, 255, 255) ); }
void SelectNets::OnNetBlue()     { ColorNet( RGB (  0,   0, 255) ); }
void SelectNets::OnNetPurple()   { ColorNet( RGB (255,   0, 255) ); }
void SelectNets::OnNetOrig()  { ColorNet( -1 ); }

void SelectNets::OnNetCustom()   
{
   static COLORREF currentColor = RGB(255, 255, 255);
   CPersistantColorDialog dialog(currentColor);
   if (dialog.DoModal())
   {
      currentColor = dialog.GetColor();
      ColorNet(currentColor);
   }
}

void SelectNets::OnColors() 
{
   AutoColor = !AutoColor;

   int i = 0;

   FileStruct *file = (FileStruct*)m_fileCB.GetItemDataPtr(m_fileCB.GetCurSel());

   POSITION pos = doc->HighlightedNetsMap.GetStartPosition();  // previous selected
   while (pos)
   {
      int netValueIndex;
      COLORREF color;
      doc->HighlightedNetsMap.GetNextAssoc(pos, netValueIndex, color);
      NetStruct *net = FindNet(file, doc->getCamCadData().getAttributeValueDictionary().getAt(netValueIndex));

      if (!AutoColor)
      {
         GetDlgItem(IDC_COLORS)->SetWindowText("Auto Color");
         color = -1;
      }
      else
      {
         GetDlgItem(IDC_COLORS)->SetWindowText("Orig Color");

         switch(i++%6) // every net gets next color
         {
            case 0:
               color = RGB(255,0,0); // RED
            break;

            case 1:
               color = RGB(255,255,0); // YELLOW
            break;

            case 2:
               color = RGB(0,255,0); // GREEN
            break;

            case 3:
               color = RGB(0,255,255); // CYAN
            break;

            case 4:
               color = RGB(0,0,255); // BLUE
            break;

            case 5:
               color = RGB(255,0,255); // MAGENTA
            break;
         }
      }

      doc->ColorNet(net, color);
   }

   Invalidate();
}

void SelectNets::OnNetShowSelectedOnly()  
{
   UpdateData();
   do_show_selected_only();
}


/******************************************************************************
* Hide_Nets_All
*/
void Hide_Nets_All(CCEtoODBDoc *doc, int hide)
{
   WORD keyword = doc->IsKeyWord(ATT_NETNAME, TRUE);
   Attrib* attrib;

   for (int i=0; i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() == dataTypeInsert || (data->getGraphicClass() != graphicClassEtch && data->getGraphicClass() != graphicClassSignal))
            continue;

         if (!data->getAttributesRef())
            continue;
         
         if (!data->getAttributesRef()->Lookup(keyword, attrib))
            continue;

         data->setHidden(hide);
      }
   }
}

/******************************************************************************
* Hide_Net
*  return true if net found
*/
int Hide_Net(CCEtoODBDoc *doc, int netValueIndex, int hide)
{
   WORD keyword = doc->IsKeyWord(ATT_NETNAME, TRUE);
   Attrib* attrib;
   int found = 0; 

   for (int i=0; i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() == dataTypeInsert || (data->getGraphicClass() != graphicClassEtch && data->getGraphicClass() != graphicClassSignal))
            continue;

         if (!data->getAttributesRef())
            continue;
         
         if (!data->getAttributesRef()->Lookup(keyword, attrib))
            continue;

         if (attrib->getStringValueIndex() != netValueIndex)
            continue;

         data->setHidden(hide);
         found++;
      }
   }

   return found;
}


/******************************************************************************
* HighlightPins
*/
void HighlightPins(CDC *pDC, CCEtoODBView *view) 
{
   CCEtoODBDoc *doc = view->GetDocument();

   if (!doc->HighlightedNetsMap.IsEmpty())
   {
      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         FileStruct *npf = doc->getFileList().GetNext(pos);

         if (!npf->isShown() || npf->isHidden() || npf->notPlacedYet())
            continue;

         if (npf->getBlock() == NULL)
            continue;

         Mat2x2 m;
         RotMat2(&m, npf->getRotation());

         POSITION netPos = npf->getNetList().GetHeadPosition();
         while (netPos != NULL)
         {
            NetStruct *net = npf->getNetList().GetNext(netPos);

            COLORREF color;
            if (doc->HighlightedNetsMap.Lookup(doc->RegisterValue(net->getNetName()), color))
            {
               POSITION cpPos = net->getHeadCompPinPosition();
               while (cpPos != NULL)
               {
                  CompPinStruct *cp = net->getNextCompPin(cpPos);
                  if (!cp->getPinCoordinatesComplete() || cp->getPadstackBlockNumber() < 0)
                     continue;

                  Point2 point2;
                  point2.x = cp->getOriginX() * npf->getScale();
                  point2.y = cp->getOriginY() * npf->getScale();
                  if (npf->getResultantMirror(doc->getBottomView())) point2.x = -point2.x;
                  TransPoint2(&point2, 1, &m, npf->getInsertX(), npf->getInsertY());

                  BlockStruct *block = doc->getBlockAt(cp->getPadstackBlockNumber());
                  if (!block)
                     continue;
                  int fileMirror = (npf->getResultantMirror(doc->getBottomView()) ? MIRROR_FLIP : 0);
                  int blockMirror = (cp->getMirror() ? MIRROR_FLIP|MIRROR_LAYERS : 0);
                  if (color != -1)
                     view->DrawBlock(pDC, block, point2.x, point2.y, cp->getRotationRadians(), fileMirror ^ blockMirror, 1, -1, 0, 1, TRUE, color, FALSE);
                  else
                     view->DrawBlock(pDC, block, point2.x, point2.y, cp->getRotationRadians(), fileMirror ^ blockMirror, 1, -1, 0, 1, FALSE, 0, FALSE);
               }
            }
         }
      }
   }

   if (!doc->HighlightedPinsList.IsEmpty())
   {
      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         FileStruct *npf = doc->getFileList().GetNext(pos);

         if (!npf->isShown() || npf->isHidden() || npf->notPlacedYet())
            continue;

         if (npf->getBlock() == NULL)
            continue;

         Mat2x2 m;
         RotMat2(&m, npf->getRotation());

         POSITION netPos = npf->getNetList().GetHeadPosition();
         while (netPos != NULL)
         {
            NetStruct *net = npf->getNetList().GetNext(netPos);

            POSITION cpPos = net->getHeadCompPinPosition();
            while (cpPos != NULL)
            {
               CompPinStruct *cp = net->getNextCompPin(cpPos);
               if (!cp->getPinCoordinatesComplete())
                  continue;

               POSITION hpPos = doc->HighlightedPinsList.GetHeadPosition();
               while (hpPos != NULL)
               {
                  HighlightedPinStruct *hp = doc->HighlightedPinsList.GetNext(hpPos);
                  
                  if (!cp->getRefDes().CompareNoCase(hp->comp) && !cp->getPinName().CompareNoCase(hp->pin))
                  {
                     Point2 point2;
                     point2.x = cp->getOriginX() * npf->getScale();
                     point2.y = cp->getOriginY() * npf->getScale();
                     if (npf->getResultantMirror(doc->getBottomView())) point2.x = -point2.x;
                     TransPoint2(&point2, 1, &m, npf->getInsertX(), npf->getInsertY());

                     // It is allowed to have a location without a padstack
                     if (cp->getPadstackBlockNumber() > -1)
                     {
                        BlockStruct *block = doc->getBlockAt(cp->getPadstackBlockNumber());
                        int fileMirror = (npf->getResultantMirror(doc->getBottomView()) ? MIRROR_FLIP : 0);
                        int blockMirror = (cp->getMirror() ? MIRROR_FLIP|MIRROR_LAYERS : 0);
                        double blockRot;
                        if (fileMirror & MIRROR_FLIP)
                           blockRot = npf->getRotation() - cp->getRotationRadians();
                        else
                           blockRot = npf->getRotation() + cp->getRotationRadians();
                        view->DrawBlock(pDC, block, point2.x, point2.y, blockRot, fileMirror ^ blockMirror, 1, -1, 0, 1, TRUE, hp->color, FALSE);
                     }
                     break;
                  }
               }
            }
         }
      }
   }
}

/******************************************************************************
* NetExtents
*/
bool NetExtents(CCEtoODBDoc *doc, const char *netName, CDblRect *rect)
{
	double xmin, xmax, ymin, ymax;
	bool retVal = NetExtents(doc, netName, &xmin, &xmax, &ymin, &ymax);

	rect->xMin = xmin;
	rect->xMax = xmax;
	rect->yMin = ymin;
	rect->yMax = ymax;

	return retVal;
}

bool NetExtents(CCEtoODBDoc *doc, const char *netName, double *xmin, double *xmax, double *ymin, double *ymax)
{
   CExtent extent;

   while (true)
   {
      WORD keyword = doc->IsKeyWord(ATT_NETNAME, TRUE);
      
      for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
            continue;

         Mat2x2 m;
         RotMat2(&m, file->getRotation());

         int value = doc->getCamCadData().getAttributeValueDictionary().getValueIndex(netName);
         Attrib* attrib;         
         
         if (value >= 0)
         {
            for (POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();dataPos != NULL;)
            {
               DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

               if (!data->getAttributesRef())
                  continue;
               
               if (!data->getAttributesRef()->Lookup(keyword, attrib))
                  continue;

               if (attrib->getStringValueIndex() != value)
                  continue;

               switch (data->getDataType())
               {
               case dataTypePoly:
                  {                  
                     for (POSITION polyPos = data->getPolyList()->GetHeadPosition();polyPos != NULL;)
                     {
                        CPoly *poly = data->getPolyList()->GetNext(polyPos);
                        
                        for (POSITION pntPos = poly->getPntList().GetHeadPosition();pntPos != NULL;)
                        {
                           CPnt *pnt = poly->getPntList().GetNext(pntPos);

                           Point2 point2;
                           point2.x = pnt->x * file->getScale();
                           point2.y = pnt->y * file->getScale();

                           if (file->getResultantMirror(doc->getBottomView() ))  point2.x = -point2.x;

                           TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

                           extent.update(point2.x,point2.y);
                        }
                     }
                  }
                  break;

               case dataTypeInsert:
                  if (data->getInsert()->getInsertType() == insertTypeVia)
                  {
                     BlockStruct *subblock;

                     if (subblock = doc->getBlockAt(data->getInsert()->getBlockNumber()))
                     {
                        Mat2x2 m;
                        RotMat2(&m, file->getRotation());

                        //case 1040: check for bottom was incorrect, use getResultantMirror function instead
						      Point2 point2;
                        point2.x = data->getInsert()->getOriginX() * file->getScale();

                        if (file->getResultantMirror(doc->getBottomView() ))  point2.x = -point2.x;

                        point2.y = data->getInsert()->getOriginY() * file->getScale();
                        TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

                        double _rot;

                        if (file->isMirrored() & MIRROR_FLIP)
                           _rot = file->getRotation() - data->getInsert()->getAngle();
                        else
                           _rot = file->getRotation() + data->getInsert()->getAngle();

                        double txmax, txmin, tymax, tymin;

                        if (block_extents(doc, &txmin, &txmax, &tymin, &tymax, &subblock->getDataList(),
                              point2.x, point2.y, _rot, 
                              file->getResultantMirror(data->getInsert()->getMirrorFlags()), file->getScale() * data->getInsert()->getScale(), -1, TRUE))
                        {
                           extent.update(txmin,tymin);
                           extent.update(txmax,tymax);
                        }
                     }
                     else
                     {
                        Point2 point2;
                        point2.x = data->getInsert()->getOriginX() * file->getScale();
                        point2.y = data->getInsert()->getOriginY() * file->getScale();

                        if (file->getResultantMirror(doc->getBottomView()))  point2.x = -point2.x;

                        TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

                        extent.update(point2.x,point2.y);
                     }
                  }

                  break;
               }
            }
         }
         
         for (POSITION netPos = file->getNetList().GetHeadPosition();netPos != NULL;)
         {
            NetStruct *net = file->getNetList().GetNext(netPos);

            if (!net->getNetName().Compare(netName))
            {               
               for (POSITION cpPos = net->getHeadCompPinPosition();cpPos != NULL;)
               {
                  CompPinStruct *cp = net->getNextCompPin(cpPos);
                  
                  if (!cp->getPinCoordinatesComplete())
                     continue;

					   //case 1040: did not consider if the view is bottom
					   Point2 point2;
					   point2.x = cp->getOriginX() * file->getScale();
					   point2.y = cp->getOriginY() * file->getScale();

					   if (file->getResultantMirror(doc->getBottomView() ))  point2.x = -point2.x;

					   TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());  					

                  BlockStruct *subblock;

                  if (cp->getPadstackBlockNumber() >= 0 && (subblock = doc->getBlockAt(cp->getPadstackBlockNumber())))
                  {
                     double txmax, txmin, tymax, tymin;

                     if (block_extents(doc, &txmin, &txmax, &tymin, &tymax, &subblock->getDataList(),
							   point2.x, point2.y, cp->getRotationRadians(), cp->getMirror(), 1, -1, TRUE))
                     {
                        extent.update(txmin,tymin);
                        extent.update(txmax,tymax);
                     }
                  }
                  else
                  {
                     Point2 point2;
                     point2.x = cp->getOriginX() * file->getScale();
                     point2.y = cp->getOriginY() * file->getScale();

                     if (file->getResultantMirror(doc->getBottomView()))  point2.x = -point2.x;

                     TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

                     extent.update(point2.x,point2.y);
                  }
               }
            }
         }
      }

      break;
   }

   bool retval = extent.isValid();

   if (retval)
   {
      *xmin = extent.getXmin();
      *ymin = extent.getYmin();
      *xmax = extent.getXmax();
      *ymax = extent.getYmax();
   }
   else
   {
      *xmin = *ymin =  DBL_MAX;
      *xmax = *ymax = -DBL_MAX;
   }

   return retval;
}


/******************************************************************************
* OnRemoveNetnames
*/
void CCEtoODBDoc::OnRemoveNetnames() 
{
   Netlist_RemoveNetnames dlg;
   dlg.doc = this;
   dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// Netlist_RemoveNetnames dialog
Netlist_RemoveNetnames::Netlist_RemoveNetnames(CWnd* pParent /*=NULL*/)
   : CDialog(Netlist_RemoveNetnames::IDD, pParent)
{
   //{{AFX_DATA_INIT(Netlist_RemoveNetnames)
   m_unusedPins = FALSE;
   m_allFeatures = FALSE;
   m_allPins = FALSE;
   m_allTraces = FALSE;
   m_selFeatures = FALSE;
   m_selPins = FALSE;
   m_selTraces = FALSE;
   //}}AFX_DATA_INIT
}

void Netlist_RemoveNetnames::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Netlist_RemoveNetnames)
   DDX_Control(pDX, IDC_NETS_LB, m_netsLB);
   DDX_Check(pDX, IDC_UNUSED_PINS, m_unusedPins);
   DDX_Check(pDX, IDC_ALL_FEATURES, m_allFeatures);
   DDX_Check(pDX, IDC_ALL_PINS, m_allPins);
   DDX_Check(pDX, IDC_ALL_TRACES, m_allTraces);
   DDX_Check(pDX, IDC_SEL_FEATURES, m_selFeatures);
   DDX_Check(pDX, IDC_SEL_PINS, m_selPins);
   DDX_Check(pDX, IDC_SEL_TRACES, m_selTraces);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(Netlist_RemoveNetnames, CDialog)
   //{{AFX_MSG_MAP(Netlist_RemoveNetnames)
   ON_BN_CLICKED(IDC_ALL_FEATURES, OnAllFeatures)
   ON_BN_CLICKED(IDC_ALL_PINS, OnAllPins)
   ON_BN_CLICKED(IDC_ALL_TRACES, OnAllTraces)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL Netlist_RemoveNetnames::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   int count = 0;
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->isShown())
         count++;
   }

   filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->isShown())
      {
         POSITION netPos = file->getNetList().GetHeadPosition();
         while (netPos)
         {
            NetStruct *net = file->getNetList().GetNext(netPos);

            if (net->getFlags() & NETFLAG_NONET || net->getFlags() & NETFLAG_UNUSEDNET)
               continue;

            CString buf = net->getNetName();
            if (count > 1)
            {
               buf += " [";
               buf += file->getName();
               buf += "]";
            }
            m_netsLB.SetItemDataPtr(m_netsLB.AddString(buf), net);
         }
      }
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void Netlist_RemoveNetnames::OnAllFeatures() 
{
   UpdateData();
   GetDlgItem(IDC_SEL_FEATURES)->EnableWindow(!m_allFeatures);
}

void Netlist_RemoveNetnames::OnAllPins() 
{
   UpdateData();
   GetDlgItem(IDC_UNUSED_PINS)->EnableWindow(!m_allPins);
   GetDlgItem(IDC_SEL_PINS)->EnableWindow(!m_allPins);
}

void Netlist_RemoveNetnames::OnAllTraces() 
{
   UpdateData();
   GetDlgItem(IDC_SEL_TRACES)->EnableWindow(!m_allTraces);
}

void Netlist_RemoveNetnames::OnOK() 
{
   CWaitCursor wait;

   UpdateData();

   if (m_allPins) m_selPins = FALSE;
   if (m_allTraces) m_selTraces = FALSE;
   if (m_allFeatures) m_selFeatures = FALSE;

   WORD kw = doc->RegisterKeyWord(ATT_NETNAME, 1, VT_STRING);
   Attrib* attrib;

   int count = m_netsLB.GetSelCount();
   int *arr = NULL;

   if (count)
   {
      arr = (int*)calloc(count, sizeof(int));
      m_netsLB.GetSelItems(count, arr);
   }

   // features and traces
   if (m_allFeatures || m_allTraces || m_unusedPins || (count && (m_selFeatures || m_selTraces)))
   {
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);
         if (!block) continue;
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data->getAttributesRef() && data->getAttributesRef()->Lookup(kw, attrib))
            {
               BOOL Remove = (m_allFeatures && data->getGraphicClass() != GR_CLASS_ETCH) || (m_allTraces && data->getGraphicClass() == GR_CLASS_ETCH);

               if (!Remove)
               {
                  CString netname = attrib->getStringValue();

                  if (m_unusedPins && netname == NET_UNUSED_PINS)
                     Remove = TRUE;
                  else for (int i=0; i<count; i++)
                  {
                     NetStruct *net = (NetStruct*)m_netsLB.GetItemDataPtr(arr[i]);

                     if (netname == net->getNetName())
                     {
                        Remove = TRUE;
                        break;
                     }
                  }
               }

               if (Remove)
                  RemoveAttrib(kw, &data->getAttributesRef());
            }
         }
      }
   }

   // pins
   if (m_allPins || m_unusedPins || (count && (m_selPins)))
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         generate_PINLOC(doc, file, 0);

         // find or add nonet
         NetStruct *nonet = NULL;
         POSITION netPos = file->getNetList().GetHeadPosition();
         while (netPos)
         {
            NetStruct *net = file->getNetList().GetNext(netPos);
            if (net->getFlags() & NETFLAG_NONET)
            {
               nonet = net;
               break;
            }
         }
         if (!nonet)
         {
            nonet = add_net(file, NET_NO_NET_PINS); 
            nonet->setFlagBits(NETFLAG_NONET);
         }

         // move pins to nonet, removing net from netlist
         netPos = file->getNetList().GetHeadPosition();
         while (netPos)
         {
            POSITION tempPos = netPos;
            NetStruct *net = file->getNetList().GetNext(netPos);
            if (net->getFlags() & NETFLAG_NONET)
               continue;

            BOOL Remove = m_allPins || (m_unusedPins && net->getFlags() & NETFLAG_UNUSEDNET); 

            if (!Remove && m_selPins)
            {
               for (int i=0; i<count; i++)
               {
                  if (m_netsLB.GetItemDataPtr(arr[i]) == net)
                  {
                     Remove = TRUE;
                     break;
                  }
               }
            }

            if (Remove)
            {
               nonet->takeCompPins(*net);
               file->getNetList().deleteAt(tempPos);

               //while (!net->getCompPinList().IsEmpty())
               //   nonet->getCompPinList().AddTail(net->getCompPinList().RemoveHead());

               //if (net->getAttributesRef())
               //{
               //   delete net->getAttributesRef();
               //   net->getAttributesRef() = NULL;
               //}

               //delete net;
            }

            //if (Remove)
            //   file->getNetList().RemoveAt(tempPos);
         }
      }
   }  
   
   if (count)
      delete arr;

   CDialog::OnOK();
}
