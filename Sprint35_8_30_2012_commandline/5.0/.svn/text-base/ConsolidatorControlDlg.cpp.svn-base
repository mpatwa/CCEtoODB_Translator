// onsolidatorControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ConsolidatorControlDlg.h"


// ConsolidatorControlDlg dialog

IMPLEMENT_DYNAMIC(ConsolidatorControlDlg, CDialog)

ConsolidatorControlDlg::ConsolidatorControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConsolidatorControlDlg::IDD, pParent)
   , m_considerOriginalName(TRUE)
   , m_pinsOnlyRadio(0)
   , m_toleranceStr("0.00001")
{
   int jj = 0;

}

ConsolidatorControlDlg::~ConsolidatorControlDlg()
{
}

void ConsolidatorControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

   DDX_Check(pDX, IDC_CHK_ORIGINAL_NAME, m_considerOriginalName);
   DDX_Radio(pDX, IDC_PINS_ONLY, m_pinsOnlyRadio);
   DDX_Text(pDX, IDC_TOLERANCE, m_toleranceStr);
}


BEGIN_MESSAGE_MAP(ConsolidatorControlDlg, CDialog)
END_MESSAGE_MAP()


// ConsolidatorControlDlg message handlers
