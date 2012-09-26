// $Header: /CAMCAD/4.3/Coords.cpp 7     8/12/03 3:29p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "coords.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
* OnPickCoord()
*/
void CCEtoODBView::OnPickCoord() 
{
   CoordsDlg dlg;
   dlg.view = this;
   dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CoordsDlg dialog
CoordsDlg::CoordsDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CoordsDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CoordsDlg)
   m_x = _T("0.000");
   m_y = _T("0.000");
   m_absRel = 0;
   //}}AFX_DATA_INIT
}

void CoordsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CoordsDlg)
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Radio(pDX, IDC_ABS_REL, m_absRel);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CoordsDlg, CDialog)
   //{{AFX_MSG_MAP(CoordsDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CoordsDlg message handlers
void CoordsDlg::OnOK() 
{
   UpdateData();
   double x = atof(m_x);
   double y = atof(m_y);
   if (m_absRel)
   {
      x += view->lastClickX; 
      y += view->lastClickY; 
   }

   //view->DoLButtonDown(0, x, y);

   CDialog::OnOK();
}

