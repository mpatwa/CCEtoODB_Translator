// $Header: /CAMCAD/4.6/BgrndBmp.cpp 13    12/07/06 12:00p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "BgrndBmp.h"
#include "ccdoc.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HBITMAP LoadJPEG(LPCTSTR lpszPathName);

static BOOL Proportional = TRUE;

/******************************************************************************
* LoadBackgroundBitmap
*/
void LoadBackgroundBitmap(CCEtoODBDoc *doc, CString filename, BOOL Top)
{
   CWaitCursor wait;

   HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   if (!hBitmap)
      hBitmap = LoadJPEG(filename);

   BackgroundBitmapStruct *bgBmp;
   if (Top)
      bgBmp = &doc->getTopBackgroundBitMap();
   else
      bgBmp = &doc->getBottomBackgroundBitMap();


   if (!hBitmap)
   {
      bgBmp->show = FALSE;

      ErrorMessage(filename, "Bitmap Load Failed");
      return;
   }

   filename = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);

   bgBmp->bitmap.Attach(hBitmap);
   bgBmp->filename = filename;
   bgBmp->show = TRUE;

   BITMAP bmpInfo;
   bgBmp->bitmap.GetBitmap(&bmpInfo);
   double aspectRatio = 1.0 * bmpInfo.bmWidth / bmpInfo.bmHeight;

   bgBmp->placementRect.xMin = doc->getSettings().getXmin();
   bgBmp->placementRect.yMin = doc->getSettings().getYmin();

   if (aspectRatio > (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->getSettings().getYmax() - doc->getSettings().getYmin()))
   {
      bgBmp->placementRect.xMax = doc->getSettings().getXmax();
      bgBmp->placementRect.yMax = doc->getSettings().getYmin() + round(1.0 * (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / aspectRatio);
   }
   else
   {
      bgBmp->placementRect.yMax = doc->getSettings().getYmax();
      bgBmp->placementRect.xMax = doc->getSettings().getXmin() + round(1.0 * (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * aspectRatio);
   }

   doc->UpdateAllViews(NULL);
}

/******************************************************************************
* OnOpenBgBmpFile
*/
void CCEtoODBDoc::OnOpenBgBmpFile() 
{
   CFileDialog FileDialog(TRUE, NULL, "*.BMP;*.JPG;*.JPEG",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "All Background Formats ((*.BMP;*.JPG;*.JPEG)|*.BMP;*.JPG;*.JPEG|Bitmap Files (*.BMP)|*.BMP|JPEG File Interchange Format (*.jpg;*.jpeg)|*.jpg;*.jpeg|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   BackgroundBitmapStruct *bgBmp;
   if (getBottomView())
      bgBmp = &getBottomBackgroundBitMap();
   else
      bgBmp = &getTopBackgroundBitMap();

   if (!bgBmp->filename.IsEmpty())
      bgBmp->bitmap.DeleteObject();
   bgBmp->show = FALSE;

   LoadBackgroundBitmap(this, FileDialog.GetFileName(), !getBottomView());
}

/******************************************************************************
* OnSetBgBmpCoords
*/
void CCEtoODBDoc::OnSetBgBmpCoords() 
{
   if ((getBottomView() && !getBottomBackgroundBitMap().show) || (!getBottomView() && !getTopBackgroundBitMap().show))
      return;

   BackgroundBitmap dlg;
   dlg.Started = FALSE;
   dlg.doc = this;
   dlg.decimals = GetDecimals(getSettings().getPageUnits());
   dlg.m_aspectRatio = !Proportional;

   dlg.DoModal();

   Proportional = !dlg.m_aspectRatio;
}

/******************************************************************************
* OnBgBmpShow
*/
void CCEtoODBDoc::OnBgBmpShow() 
{
   BackgroundBitmapStruct *bgBmp;
   if (getBottomView())
      bgBmp = &getBottomBackgroundBitMap();
   else
      bgBmp = &getTopBackgroundBitMap();

   if (bgBmp->filename.IsEmpty())
      OnOpenBgBmpFile();
   else
   {
      bgBmp->show = !bgBmp->show;
      UpdateAllViews(NULL);
   }  
}

/////////////////////////////////////////////////////////////////////////////
// BackgroundBitmap dialog
BackgroundBitmap::BackgroundBitmap(CWnd* pParent /*=NULL*/)
   : CDialog(BackgroundBitmap::IDD, pParent)
{
   //{{AFX_DATA_INIT(BackgroundBitmap)
   m_aspectRatio = -1;
   m_onTop = FALSE;
   m_bottom = _T("");
   m_left = _T("");
   m_width = _T("");
   m_height = _T("");
   //}}AFX_DATA_INIT
}

void BackgroundBitmap::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(BackgroundBitmap)
   DDX_Control(pDX, IDC_WIDTH_SPIN, m_widthSpin);
   DDX_Control(pDX, IDC_HEIGHT_SPIN, m_heightSpin);
   DDX_Control(pDX, IDC_BOTTOM_SPIN, m_bottomSpin);
   DDX_Control(pDX, IDC_LEFT_SPIN, m_leftSpin);
   DDX_Radio(pDX, IDC_ASPECT_RATIO, m_aspectRatio);
   DDX_Check(pDX, IDC_ON_TOP, m_onTop);
   DDX_Text(pDX, IDC_BOT, m_bottom);
   DDX_Text(pDX, IDC_LEFT, m_left);
   DDX_Text(pDX, IDC_WIDTH, m_width);
   DDX_Text(pDX, IDC_HEIGHT, m_height);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(BackgroundBitmap, CDialog)
   //{{AFX_MSG_MAP(BackgroundBitmap)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   ON_BN_CLICKED(IDC_FIT_TO_PAGE, OnFitToPage)
   ON_NOTIFY(UDN_DELTAPOS, IDC_HEIGHT_SPIN, OnDeltaposHeightSpin)
   ON_NOTIFY(UDN_DELTAPOS, IDC_WIDTH_SPIN, OnDeltaposWidthSpin)
   ON_NOTIFY(UDN_DELTAPOS, IDC_BOTTOM_SPIN, OnDeltaposBottomSpin)
   ON_NOTIFY(UDN_DELTAPOS, IDC_LEFT_SPIN, OnDeltaposLeftSpin)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL BackgroundBitmap::OnInitDialog() 
{
   CDialog::OnInitDialog();

   UDACCEL* accel = (UDACCEL*)calloc(6, sizeof(UDACCEL));
   accel[0].nSec = 0;
   accel[0].nInc = 1;

   accel[1].nSec = 1;
   accel[1].nInc = 10;

   accel[2].nSec = 2;
   accel[2].nInc = 50;
   
   accel[3].nSec = 3;
   accel[3].nInc = 100;

   accel[4].nSec = 4;
   accel[4].nInc = 200;

   accel[5].nSec = 5;
   accel[5].nInc = 400;

   m_heightSpin.SetRange(UD_MINVAL, UD_MAXVAL);
   m_heightSpin.SetAccel(6, accel);
   m_bottomSpin.SetRange(UD_MINVAL, UD_MAXVAL);
   m_bottomSpin.SetAccel(6, accel);
   m_leftSpin.SetRange(UD_MINVAL, UD_MAXVAL);
   m_leftSpin.SetAccel(6, accel);
   m_widthSpin.SetRange(UD_MINVAL, UD_MAXVAL);
   m_widthSpin.SetAccel(6, accel);

   free(accel);

   Started = TRUE;

   BackgroundBitmapStruct *bgBmp;
   if (doc->getBottomView())
      bgBmp = &doc->getBottomBackgroundBitMap();
   else
      bgBmp = &doc->getTopBackgroundBitMap();

   BITMAP bmpInfo;
   bgBmp->bitmap.GetBitmap(&bmpInfo);
   aspectRatio = 1.0 * bmpInfo.bmWidth / bmpInfo.bmHeight;

   m_left.Format("%.*lf", decimals, bgBmp->placementRect.xMin);
   m_bottom.Format("%.*lf", decimals, bgBmp->placementRect.yMin);
   m_width.Format("%.*lf", decimals, fabs(bgBmp->placementRect.Width()));
   m_height.Format("%.*lf", decimals, fabs(bgBmp->placementRect.Height()));

   m_onTop = doc->bgBmpOnTop;

   factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);
   if (Platform == WINNT)
      factor *= MAX_32BIT / MAX_16BIT;

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void BackgroundBitmap::OnFitToPage() 
{
   UpdateData();

   m_left.Format("%.*lf", decimals, doc->getSettings().getXmin());
   m_bottom.Format("%.*lf", decimals, doc->getSettings().getYmin());
   m_width.Format("%.*lf", decimals, doc->getSettings().getXmax() - doc->getSettings().getXmin());
   m_height.Format("%.*lf", decimals, doc->getSettings().getYmax() - doc->getSettings().getYmin());

   if (!m_aspectRatio)
   {
      if (aspectRatio > (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->getSettings().getYmax() - doc->getSettings().getYmin()))
      {
         m_width.Format("%.*lf", decimals, doc->getSettings().getXmax() - doc->getSettings().getXmin());
         m_height.Format("%.*lf", decimals, (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / aspectRatio);
      }
      else
      {
         m_height.Format("%.*lf", decimals, doc->getSettings().getYmax() - doc->getSettings().getYmin());
         m_width.Format("%.*lf", decimals, (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * aspectRatio);
      }
   }
   
   UpdateData(FALSE);

   OnApply();
}

void BackgroundBitmap::OnApply() 
{
   if (!Started)
      return;

   UpdateData();

   BackgroundBitmapStruct *bgBmp;
   if (doc->getBottomView())
      bgBmp = &doc->getBottomBackgroundBitMap();
   else
      bgBmp = &doc->getTopBackgroundBitMap();

   bgBmp->placementRect.xMin = atof(m_left);
   bgBmp->placementRect.yMin = atof(m_bottom);
   bgBmp->placementRect.xMax = atof(m_left) + atof(m_width);
   bgBmp->placementRect.yMax = atof(m_bottom) + atof(m_height);

   doc->bgBmpOnTop = m_onTop;

   doc->UpdateAllViews(NULL);
}

void BackgroundBitmap::OnOK() 
{
   OnApply();
   
   CDialog::OnOK();
}

void BackgroundBitmap::OnDeltaposHeightSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   UpdateData();
   m_height.Format("%.*lf", decimals, atof(m_height) + factor * pNMUpDown->iDelta);
   if (!m_aspectRatio)
      m_width.Format("%.*lf", decimals, atof(m_height) * aspectRatio);
   UpdateData(FALSE);

   *pResult = 0;

   OnApply();
}

void BackgroundBitmap::OnDeltaposWidthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   UpdateData();
   m_width.Format("%.*lf", decimals, 1.0 * atof(m_width) + factor * pNMUpDown->iDelta);
   if (!m_aspectRatio)
      m_height.Format("%.*lf", decimals, atof(m_width) / aspectRatio);
   
   UpdateData(FALSE);

   *pResult = 0;

   OnApply();
}

void BackgroundBitmap::OnDeltaposBottomSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   
   UpdateData();
   m_bottom.Format("%f", atof(m_bottom) + factor * pNMUpDown->iDelta);
   UpdateData(FALSE);

   *pResult = 0;

   OnApply();
}

void BackgroundBitmap::OnDeltaposLeftSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   
   UpdateData();
   m_left.Format("%.*lf", decimals, atof(m_left) + factor * pNMUpDown->iDelta);
   UpdateData(FALSE);

   *pResult = 0;

   OnApply();
}
