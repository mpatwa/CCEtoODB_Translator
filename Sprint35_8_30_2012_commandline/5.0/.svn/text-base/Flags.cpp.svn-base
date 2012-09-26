// $Header: /CAMCAD/4.4/Flags.cpp 10    3/15/04 7:10a Kurt Van Ness $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Flags.h"
#include "dbutil.h"
#include "data.h"
#include "block.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FlagsDlg dialog
FlagsDlg::FlagsDlg(CWnd* pParent /*=NULL*/)
   : CDialog(FlagsDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(FlagsDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void FlagsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FlagsDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FlagsDlg, CDialog)
   //{{AFX_MSG_MAP(FlagsDlg)
   ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FlagsDlg message handlers
BOOL FlagsDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   FillList();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FlagsDlg::FillList()
{
   int top = m_list.GetTopIndex();

   m_list.ResetContent();

   switch (group)
   {
   case Data:
      {
         DataStruct *data = (DataStruct*)item;

         m_list.SetSel(m_list.AddString("Data Sequence Head"), data->getFlags() & DATA_SEQ_HEAD?1:0);
         m_list.SetSel(m_list.AddString("Data Sequence Continue"), data->getFlags() & DATA_SEQ_CONT?1:0);
      }
      break;

   case Block:
      {
         BlockStruct *block = (BlockStruct*)item;

         m_list.SetSel(m_list.AddString("Aperture"), block->getFlags() & BL_APERTURE?1:0);
         m_list.SetSel(m_list.AddString("Block Aperture"), block->getFlags() & BL_BLOCK_APERTURE?1:0);
         m_list.SetSel(m_list.AddString("File Block"), block->getFlags() & BL_FILE?1:0);
         m_list.SetSel(m_list.AddString("Width"), block->getFlags() & BL_WIDTH?1:0);
         m_list.SetSel(m_list.AddString("Tool"), block->getFlags() & BL_TOOL?1:0);
         m_list.SetSel(m_list.AddString("Block Tool"), block->getFlags() & BL_BLOCK_TOOL?1:0);
         m_list.SetSel(m_list.AddString("Global"), block->getFlags() & BL_GLOBAL?1:0);
         //m_list.SetSel(m_list.AddString("Not True Shape Aperture"), block->getFlags() & BL_NOT_TRUESHAPE_APERTURE?1:0);
         m_list.SetSel(m_list.AddString("Local Block"), block->getFlags() & BL_LOCALBLOCK?1:0);
         m_list.SetSel(m_list.AddString("Small Width"), block->getFlags() & BL_SMALLWIDTH?1:0);
         m_list.SetSel(m_list.AddString("Special"), block->getFlags() & BL_SPECIAL?1:0);
         m_list.SetSel(m_list.AddString("Access Top"), block->getFlags() & BL_ACCESS_TOP?1:0);
         m_list.SetSel(m_list.AddString("Access Bottom"), block->getFlags() & BL_ACCESS_BOTTOM?1:0);
         
         // outer is top | bottom
         //m_list.SetSel(m_list.AddString("Access Outer"), ((block->getFlags() & BL_ACCESS_OUTER) == BL_ACCESS_OUTER)?1:0);

         m_list.SetSel(m_list.AddString("Access None"), block->getFlags() & BL_ACCESS_NONE?1:0);
         m_list.SetSel(m_list.AddString("Access Top Only"), block->getFlags() & BL_ACCESS_TOP_ONLY?1:0);
         m_list.SetSel(m_list.AddString("Access Bottom Only"), block->getFlags() & BL_ACCESS_BOTTOM_ONLY?1:0);
      }
      break;
   }

   m_list.SetTopIndex(top);
}

void FlagsDlg::OnSelchangeList1() 
{
   FillList(); 
}
