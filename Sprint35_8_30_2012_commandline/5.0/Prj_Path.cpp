// $Header: /CAMCAD/4.5/Prj_Path.cpp 16    6/07/05 5:36p Lynn Phung $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "prj_path.h"
#include "dirdlg.h"
#include "mainfrm.h"
#include ".\prj_path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SECTION "Paths"

extern CMultiDocTemplate*   pDocTemplate;


/******************************************************************************
* OnSetProjectPath
*/
void CCEtoODBApp::OnSetProjectPath() 
{
   LPCTSTR profileName = m_pszProfileName;
   m_pszProfileName = _tcsdup("Projects");

	CString projectPath;
	if (getActiveView() != NULL)
		projectPath = (getActiveView()->GetDocument()!=NULL)?getActiveView()->GetDocument()->GetProjectPath():"";

   ProjectPath dlg(this); 
	dlg.SetPathByFirstFileLoaded(projectPath);
	dlg.SetPathByCurrentSelection(this->GetProfileString("Paths", "0", getUserPath()));
	dlg.SetProjectPathOption(this->GetProfileInt("Paths", "Option", 0));

   if (dlg.DoModal() == IDOK)
	{
		POSITION pos = pDocTemplate->GetFirstDocPosition();
		while (pos != NULL)
		{
			CCEtoODBDoc *pDoc = (CCEtoODBDoc*)pDocTemplate->GetNextDoc(pos);
			if (pDoc == NULL)
				continue;

			pDoc->SetProjectPathOption(dlg.GetProjectPathOption());
			pDoc->SetProjectPathByUserSelection(dlg.GetPathByCurrentSelection());
		}

		CString projectPath;
		if (getActiveView() != NULL)
			projectPath = (getActiveView()->GetDocument()!=NULL)?getActiveView()->GetDocument()->GetProjectPath():"";

		CString title = getCamCadTitle();
		if (!projectPath.IsEmpty())
		{
			title += " - {";
			title += projectPath;
			title += "}";
		}
		getMainFrame()->SetFrameTitle(title);
		getMainFrame()->OnUpdateFrameTitle(TRUE);
	}

   free((void*)m_pszProfileName);
   m_pszProfileName = profileName;  
}

/////////////////////////////////////////////////////////////////////////////
// ProjectPath dialog
ProjectPath::ProjectPath(CWinApp *app, CWnd* pParent /*=NULL*/)
   : CDialog(ProjectPath::IDD, pParent)
	, m_projectPathOption(FALSE)
{
	m_app = app;
	m_pathByfirstFileLoaded = "";
   m_pathByCurrentSelection = "";
}

void ProjectPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROJECT_PATH, m_list);
	DDX_Radio(pDX, IDC_RADIO_FIRST_FILE_LOADED, m_projectPathOption);
	DDX_Text(pDX, IDC_TXT_DIR_FIRST_FILE, m_pathByfirstFileLoaded);
	DDX_Text(pDX, IDC_TXT_CURRENT_SELECTION, m_pathByCurrentSelection);
}

BEGIN_MESSAGE_MAP(ProjectPath, CDialog)
   ON_LBN_DBLCLK(IDC_LIST_PROJECT_PATH, OnSelect)
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_REMOVE, OnRemove)
   ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_BN_CLICKED(IDC_RESET_PATH, OnReset)
	ON_BN_CLICKED(IDC_RADIO_FIRST_FILE_LOADED, OnProjectPathOptionClicked)
	ON_BN_CLICKED(IDC_RADIO_CURRENT_SELECTION, OnProjectPathOptionClicked)
END_MESSAGE_MAP()

BOOL ProjectPath::OnInitDialog() 
{
   CDialog::OnInitDialog();

	fillList();

	UpdateData(FALSE);
	OnProjectPathOptionClicked();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ProjectPath::fillList() 
{
   m_list.ResetContent();

   int i = 1;
   CString num, path;

   num.Format("%d", i);
   path = m_app->GetProfileString(SECTION, num);

   while (!path.IsEmpty())
   {
      m_list.SetItemData(m_list.AddString(num + ") " + path), i++);
      num.Format("%d", i);
      path = m_app->GetProfileString(SECTION, num);
   }
}
   
void ProjectPath::OnBrowse() 
{
   CBrowse dlg;
   dlg.m_strSelDir = m_pathByCurrentSelection;
   dlg.m_strTitle = "Select Project Path";
   if (dlg.DoBrowse())
   {
		// 0 is the current project path
		m_app->WriteProfileString(SECTION, "0", dlg.m_strPath);
      m_pathByCurrentSelection = dlg.m_strPath;
		UpdateData(FALSE);
   }
}

void ProjectPath::OnAdd() 
{
   UpdateData();

   if (m_pathByCurrentSelection.IsEmpty())
      return;

   int i = 1;
   CString num, path;

   num.Format("%d", i);
   path = m_app->GetProfileString(SECTION, num);

	// Get the largest number in registry
   while (!path.IsEmpty())
   {
      num.Format("%d", ++i);
      path = m_app->GetProfileString(SECTION, num);
   }
   m_app->WriteProfileString(SECTION, num, m_pathByCurrentSelection);

   fillList();
}

void ProjectPath::OnRemove() 
{
   int sel = m_list.GetCurSel();
   if (sel == LB_ERR)
      return;

   int i = sel+1;

   CString num, nextNum, temp;
   num.Format("%d", i);
   nextNum.Format("%d", i+1);

   temp = m_app->GetProfileString(SECTION, nextNum);

   while (!temp.IsEmpty())
   {
      m_app->WriteProfileString(SECTION, num, temp);
      i++;
      num.Format("%d", i);
      nextNum.Format("%d", i+1);
      temp = m_app->GetProfileString(SECTION, nextNum);
   }

   HKEY projectsKey, pathsKey;
   projectsKey = m_app->GetAppRegistryKey();
   RegOpenKey(projectsKey, SECTION, &pathsKey);
   RegDeleteValue(pathsKey, num);
   RegCloseKey(pathsKey);
   RegCloseKey(projectsKey);

   fillList();
}

void ProjectPath::OnSelect() 
{
   int sel = m_list.GetCurSel();
   if (sel == LB_ERR)
      return;

   CString num;
   num.Format("%d", sel+1);
   m_pathByCurrentSelection = m_app->GetProfileString(SECTION, num);

	// 0 is the current project path
	m_app->WriteProfileString(SECTION, "0", m_pathByCurrentSelection);

   UpdateData(FALSE);
}

void ProjectPath::OnReset()
{
	m_pathByCurrentSelection = "";

	// 0 is the current project path
	m_app->WriteProfileString(SECTION, "0", m_pathByCurrentSelection);

   UpdateData(FALSE);
}

void ProjectPath::OnProjectPathOptionClicked()
{
	UpdateData(TRUE);
	if (m_projectPathOption == 0)
	{
		GetDlgItem(IDC_TXT_CURRENT_SELECTION)->EnableWindow(FALSE);
		GetDlgItem(IDC_LIST_PROJECT_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESET_PATH)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_TXT_CURRENT_SELECTION)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIST_PROJECT_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RESET_PATH)->EnableWindow(TRUE);
	}

	m_app->WriteProfileInt(SECTION, "Option", m_projectPathOption);
}
