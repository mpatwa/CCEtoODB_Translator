// $Header: /CAMCAD/4.3/Fmt3070.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once
#include "afxcmn.h"
#include "data.h"

class CCEtoODBDoc;

// PartNumbersDlg dialog
class PartNumbersDlg : public CDialog
{
   DECLARE_DYNAMIC(PartNumbersDlg)

public:
   PartNumbersDlg(CWnd* pParent = NULL);   // standard constructor
   virtual ~PartNumbersDlg();

// Dialog Data
   enum { IDD = IDD_3070_PARTNUMBERS };

   CDataList *dataList;
   CCEtoODBDoc *doc;
   CMapStringToString partnumberMap;

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog( );

   DECLARE_MESSAGE_MAP()
public:
   CListCtrl m_list;
   afx_msg void OnRename();
   afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
