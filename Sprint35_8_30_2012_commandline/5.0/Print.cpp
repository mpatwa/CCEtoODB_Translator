// $Header: /CAMCAD/4.6/Print.cpp 22    5/24/07 5:23p Lynn Phung $

/*                        
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/ 

#include "stdafx.h"
#include "CCview.h" 
#include "print.h"
#include <math.h>
#include "CCEtoODB.h"
#include "mainfrm.h"
#include "license.h"
#include "crypt.h"

static BOOL    Print_by_Layer = FALSE;
static BOOL    Print_Banner = TRUE;
static BOOL    Print_Extents = TRUE;
BOOL           Print_Black = TRUE;
BOOL           Print_Reverse = FALSE;
BOOL           Print_Grid = FALSE;
BOOL           Fit_to_Page = TRUE;
double         Print_Scale = 1.0;
double         PrintFactor;
static int     mapMode;
static CToolBarVisibilityState tbVis;

static double  h, v;
static int     pagesHigh, pagesWide;
static int     row, column;
static CRect   reversePage;

struct layer
{
   BOOL show;
   LayerStruct *lp;
};

static layer *layerArray = NULL;
static int layerCount;

static double xmin, xmax, ymin, ymax;

/////////////////////////////////////////////////////////////////////////////
BOOL CCEtoODBApp::GetLandscape()
{
   BOOL Landscape = FALSE;
   PRINTDLG pd;
   pd.lStructSize = (DWORD)sizeof(PRINTDLG);
   if (GetPrinterDeviceDefaults(&pd))
   {
      DEVMODE *pDevMode = (DEVMODE *)::GlobalLock(m_hDevMode);
      if (pDevMode)
      {
         Landscape = (pDevMode->dmOrientation == DMORIENT_LANDSCAPE);
         ::GlobalUnlock(m_hDevMode);
      }
   }

   return Landscape;
}
 
void CCEtoODBApp::SetLandscape(BOOL Landscape)
{
   PRINTDLG pd;
   pd.lStructSize = (DWORD)sizeof(PRINTDLG);
   if (GetPrinterDeviceDefaults(&pd))
   {
      DEVMODE *pDevMode = (DEVMODE *)::GlobalLock(m_hDevMode);
      if (pDevMode)
      {
         if (Landscape)
            pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
         else
            pDevMode->dmOrientation = DMORIENT_PORTRAIT;
         ::GlobalUnlock(m_hDevMode);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CCEtoODBView printing
/******************************************************************************
* OnPreparePrinting
*/
BOOL CCEtoODBView::OnPreparePrinting(CPrintInfo* pInfo)
{
   CCEtoODBDoc *doc = GetDocument();

   /*if (!pInfo->m_bPreview && !getApp().getCamcadLicense().isLicensed(camcadProductPrint)) 
   {
#ifdef SHAREWARE // do not open or read or process license file
      ErrorMessage("To purchase a copy of CAMCAD please contact you reseller or email Rsi_sales@mentor.com", "CAMCAD Shareware does not support Printing!");
      CString file = getApp().getCamcadExeFolderPath();
      file += "order.txt";
      Notepad(file);
#else
      ErrorAccess("You do not have a License to Print!");
#endif
      return 0;
   }*/

	CMainFrame *mainFrm = (CMainFrame*)AfxGetMainWnd();
	// get all visible tool bars and menu states
	tbVis = mainFrm->GetToolBarVisiblity();

	PrintDialog pd(pInfo, this);

	pd.m_portrait = ((CCEtoODBApp *)(AfxGetApp()))->GetLandscape();

	if (pd.DoModal() != IDOK) return 0;


	((CCEtoODBApp *)(AfxGetApp()))->SetLandscape(pd.m_portrait);

   if (Print_Extents)
   {
      doc->get_extents(&xmin, &xmax, &ymin, &ymax);
      if (xmin >= xmax) 
      {
         ErrorMessage("Can not print extents when there is no drawing!");
         return 0;
      }
   }

   if (Print_by_Layer)
   {
      // count layers
      layerCount = 0;
		int j=0;
      for (j=0; j< doc->getMaxLayerIndex(); j++)
      {
         if (doc->getLayerArray()[j] == NULL)  continue;
         if (doc->getLayerArray()[j]->isVisible())
            layerCount++;
      }
      // save layer information
      layerArray = (layer*)calloc(layerCount, sizeof(layer));

      int i = 0;
      for (j=0; j< doc->getMaxLayerIndex(); j++)
      {
         LayerStruct *layer = doc->getLayerArray()[j];
         if (layer == NULL)   continue;
         if (layer->isVisible())
         {
            layerArray[i].show = layer->isVisible();
            layerArray[i].lp = layer;
            i++;
         }
      }
   }
   else 
      layerCount = 1;

	BOOL retVal = DoPreparePrinting(pInfo);

	if (retVal && pInfo->m_bPreview)
	{
		// hide all visible tool bars and menu
		mainFrm->SetToolBarVisiblity(CToolBarVisibilityState());	// defaults to all off
	}

   return retVal;
}

/******************************************************************************
* OnEndPrintPreview
*/
void CCEtoODBView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
	CMainFrame *mainFrm = (CMainFrame*)AfxGetMainWnd();

	// TOM - Case 1187 - Added begin and end update to prevent an extra redraw from occuring
	//BeginUpdate();
	// show all visible tool bars and menu
	mainFrm->SetToolBarVisiblity(tbVis);
	tbVis = CToolBarVisibilityState();

	CView::OnEndPrintPreview(pDC, pInfo, point, pView);
	//EndUpdate();
}

/******************************************************************************
* OnBeginPrinting
*/
void CCEtoODBView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
   CCEtoODBDoc *doc = GetDocument();

   // ___SIZE in mm
   h = 1.0 * pDC->GetDeviceCaps(HORZSIZE) * Units_Factor(UNIT_MM, doc->getSettings().getPageUnits());
   v = 1.0 * pDC->GetDeviceCaps(VERTSIZE) * Units_Factor(UNIT_MM, doc->getSettings().getPageUnits());

   PrintFactor = 1.0;

   if (!Fit_to_Page)
   {
      int max = MAX_COORD / 1000;
      if (Print_Scale * xmax > max || Print_Scale * xmin < -max || Print_Scale * ymax > max || Print_Scale * ymin < -max)
      {
         mapMode = MM_LOENGLISH;
         PrintFactor = 100.0;
      }
      else
      {
         mapMode = MM_HIENGLISH;
         PrintFactor = 1000.0;
      }

      CCEtoODBDoc *doc = GetDocument();
      if (Print_Extents) // print EXTENTS to SCALE 
      {
         // minimum 1x1
         pagesWide = (int)ceil(Print_Scale * (xmax - xmin) / h); 
         pagesHigh = (int)ceil(Print_Scale * (ymax - ymin) / v);
      }
      else // print CONTENTS to SCALE
      {
         CCEtoODBDoc *doc = GetDocument();

         // minimum 1x1
         pagesWide = (int)ceil(Print_Scale * (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / ScaleDenom / h 
            * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES)); 
         pagesHigh = (int)ceil(Print_Scale * (doc->getSettings().getYmax() - doc->getSettings().getYmin()) / ScaleDenom / v
            * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));
      }
   }
   else
      pagesWide = pagesHigh = 1;

   pInfo->SetMaxPage(pagesWide * pagesHigh * layerCount);
}

/******************************************************************************
* PreparePrintDC
*/
void CCEtoODBView::PreparePrintDC(CDC* pDC, CPrintInfo* pInfo, CCEtoODBDoc *doc)
{
   if (Print_by_Layer)
   {
      CCEtoODBDoc *doc = GetDocument();

      // clear all other layers
      for (int i=0; i< doc->getMaxLayerIndex(); i++)
      {
         if (doc->getLayerArray()[i] == NULL)  continue;
         doc->getLayerArray()[i]->setVisible(false);
      }

      layerArray[(pInfo->m_nCurPage - 1) / (pagesHigh * pagesWide)].lp->setVisible( 
            layerArray[(pInfo->m_nCurPage - 1) / (pagesHigh * pagesWide)].show);
   }

   if (Fit_to_Page)
   {
      pDC->SetMapMode(MM_ISOTROPIC);

      if (Print_Extents) // fit EXTENTS to PAGE 
      {
         double ratio = 1.0 * (doc->maxXCoord - doc->minXCoord) / (doc->getSettings().getXmax() - doc->getSettings().getXmin());

			if (ScaleNum / ScaleDenom - 1.0 >= SMALLNUMBER)
				ratio /= ScaleNum / ScaleDenom;

			reversePage.left = round(xmin * ratio);
         reversePage.right = round(xmax * ratio);
         reversePage.top = round(ymax * ratio);
         reversePage.bottom = round(ymin * ratio);
      }
      else // fit CONTENTS to PAGE
      {
         long unitsX, unitsY;
         CRect clientRect;
         GetClientRect(&clientRect);

         Window_Shape(clientRect.Width(), clientRect.Height(), &unitsX, &unitsY, 
               GetDocument()->maxXCoord - GetDocument()->minXCoord, 
               GetDocument()->maxYCoord - GetDocument()->minYCoord);

         reversePage.left = GetXPos();
         reversePage.right = reversePage.left + round(1.0*unitsX / ScaleDenom);
         reversePage.top = GetYPos();
         reversePage.bottom = reversePage.top - round(1.0*unitsY / ScaleDenom);
      }

      pDC->SetWindowOrg(reversePage.left, reversePage.top); 
      pDC->SetWindowExt(reversePage.right - reversePage.left, reversePage.bottom - reversePage.top);

      pDC->SetViewportOrg(0, 0); 
      pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES)); // set size of what it is to be shown within
   }
   else // print to SCALE
   {
      pDC->SetMapMode(mapMode);

      row = (pInfo->m_nCurPage-1) % (pagesHigh * pagesWide) / pagesWide + 1;
      column = (pInfo->m_nCurPage-1) % (pagesHigh * pagesWide) % pagesWide + 1;  

      double factor = PrintFactor * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES);

      if (Print_Extents) // print EXTENTS to SCALE 
      {
         reversePage.left = round((Print_Scale * xmin + h*(column-1)) * factor);
         reversePage.right = round((Print_Scale * xmax + h*(column-1)) * factor);
         reversePage.top = round((Print_Scale * ymax - v*(row-1)) * factor);
         reversePage.bottom = round((Print_Scale * ymin - v*(row-1)) * factor);
      }
      else // print CONTENTS to SCALE
      {
         double ratio = factor * (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);

         long unitsX, unitsY;
         CRect clientRect;
         GetClientRect(&clientRect);

         Window_Shape(clientRect.Width(), clientRect.Height(), &unitsX, &unitsY, 
               GetDocument()->maxXCoord - GetDocument()->minXCoord, 
               GetDocument()->maxYCoord - GetDocument()->minYCoord);

         reversePage.left = round(ratio * Print_Scale * GetXPos() + factor*h*(column-1));
         reversePage.right = reversePage.left + round(ratio * Print_Scale * unitsX / ScaleDenom);
         reversePage.top = round(ratio * Print_Scale * GetYPos() - factor*v*(row-1));
         reversePage.bottom = reversePage.top - round(ratio * Print_Scale * unitsY / ScaleDenom);
      }

      pDC->SetWindowOrg(reversePage.left, reversePage.top); 
      pDC->SetWindowExt(reversePage.right - reversePage.left, reversePage.top - reversePage.bottom);
   }
}

/******************************************************************************
* PrintPageFooter
*/
void PrintPageFooter(CDC *pDC, CPrintInfo *pInfo, CString &footer)
{
   TEXTMETRIC textMetric;
   int y;

   pDC->GetTextMetrics(&textMetric);
   y = pInfo->m_rectDraw.bottom + textMetric.tmHeight + 1;

   // text
   pDC->SetTextAlign(TA_LEFT);
   pDC->TextOut(pInfo->m_rectDraw.left, y, footer);

   // line
   pDC->MoveTo(pInfo->m_rectDraw.left, y);
   pDC->LineTo(pInfo->m_rectDraw.right, y);

   // subtract from drawing rect the space used by footer
   pInfo->m_rectDraw.bottom = y;

   pDC->IntersectClipRect(pInfo->m_rectDraw); // make clip rgn this page
}

/******************************************************************************
* OnPrint
*/
void CCEtoODBView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{

   if (Print_Banner)
   {
      CString footer;

      footer = "Printed by ";
      footer += getApp().getCamCadTitle();
      footer += " : ";

      CTime t = CTime::GetCurrentTime();
      footer += t.Format("%a %b %d %I:%M%p");

      if (!Fit_to_Page && (pagesWide > 1 || pagesHigh > 1))
      {
         CString buf;
         buf.Format(" (%d,%d)", column, row);
         footer += buf;
      }

      if (!Fit_to_Page)
      {
         CString buf;
         buf.Format(" Scale:%.3lf", Print_Scale);
         footer += buf;
      }

      if (Print_by_Layer)
      {
         CString buf;
         buf.Format(" Layer:%s", layerArray[(pInfo->m_nCurPage - 1) / (pagesHigh * pagesWide)].lp->getName());
         footer += buf;
      }
      else // put file names if not printing by layer
      {
         CFrameWnd *frame = GetParentFrame();
         if (frame)
         {
            CString title;
            frame->GetWindowText(title);
            footer += " (";
            footer += title;
            footer += ")";
         }
      }
      PrintPageFooter(pDC, pInfo, footer);
   }

   pDC->IntersectClipRect(reversePage);

   if (Print_Reverse) // draw page
   {
      double factor = PrintFactor * Units_Factor(GetDocument()->getSettings().getPageUnits(), UNIT_INCHES);
      HBRUSH oldBrush = (HBRUSH) ::SelectObject(pDC->m_hDC, GetStockObject(BLACK_BRUSH));
      pDC->Rectangle(reversePage.left, reversePage.top, reversePage.right, reversePage.bottom);
      ::DeleteObject(::SelectObject(pDC->m_hDC, oldBrush));
   }

   CView::OnPrint(pDC, pInfo);
}

/******************************************************************************
* OnEndPrinting
*/
void CCEtoODBView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
   if (Print_by_Layer && layerArray)
   {
      // reset show to originals
      for (int i=0; i<layerCount; i++)
         layerArray[i].lp->setVisible( layerArray[i].show);

      free(layerArray);
      layerArray = NULL;
   }

   CView::OnEndPrinting(pDC, pInfo);
}


/////////////////////////////////////////////////////////////////////////////
// PrintDialog dialog
PrintDialog::PrintDialog(CPrintInfo *PInfo, CWnd* pParent /*=NULL*/)
   : CDialog(PrintDialog::IDD, pParent)
{
   pInfo = PInfo;
   //{{AFX_DATA_INIT(PrintDialog)
   m_fit = !Fit_to_Page;
   m_extents = !Print_Extents;
   m_banner = Print_Banner;
   m_print_by_layer = Print_by_Layer;
   m_scale = Print_Scale;
   m_portrait = -1;
   m_Black = 0;
   m_grid = Print_Grid;
   //}}AFX_DATA_INIT
   if (Print_Reverse)
      m_Black = 1;
   else if (Print_Black)
      m_Black = 0;
   else
      m_Black = 2;
}

void PrintDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PrintDialog)
   DDX_Radio(pDX, IDC_FIT, m_fit);
   DDX_Radio(pDX, IDC_EXTENTS, m_extents);
   DDX_Check(pDX, IDC_BANNER, m_banner);
   DDX_Check(pDX, IDC_PRINT_BY_LAYER, m_print_by_layer);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDX_Radio(pDX, IDC_PORTRAIT, m_portrait);
   DDX_Radio(pDX, IDC_BLACK, m_Black);
   DDX_Check(pDX, IDC_GRID, m_grid);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PrintDialog, CDialog)
   //{{AFX_MSG_MAP(PrintDialog)
   ON_BN_CLICKED(IDHELP, OnHelp)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PrintDialog message handlers
void PrintDialog::OnOK() 
{
   CDialog::OnOK();

   Fit_to_Page = !m_fit;
   Print_Extents = !m_extents;
   Print_Banner = m_banner;
   Print_by_Layer = m_print_by_layer;
   Print_Grid = m_grid;
   Print_Black = !(m_Black == 2);
   Print_Reverse = (m_Black == 1);
   if (Fit_to_Page)
      Print_Scale = 1.0;
   else
      Print_Scale = m_scale;
}

void PrintDialog::OnHelp() 
{ 
	//getApp().OnHelpIndex();
}
