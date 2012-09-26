// $Header: /CAMCAD/4.6/GerberEducatorToolbar.cpp 15    2/14/07 4:03p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/


#include "stdafx.h"
#include "GerberEducator.h"
#include "GerberEducatorDialog.h"
#include "GerberEducatorToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//-----------------------------------------------------------------------------
// CGerberEducatorToolbar dialog
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CGerberEducatorToolbar, CDialogBar)
CGerberEducatorToolbar::CGerberEducatorToolbar(CWnd* pParent /*=NULL*/)
	: CDialogBar()
{
	m_reviewGeometriesDlg = NULL;
	m_insertGeometriesDlg = NULL;
	m_setupDataTypeDlg = NULL;
}

CGerberEducatorToolbar::~CGerberEducatorToolbar()
{
	delete m_reviewGeometriesDlg;
	delete m_insertGeometriesDlg;
}

void CGerberEducatorToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGerberEducatorToolbar, CDialogBar)
	ON_COMMAND(ID_TerminateSetupDataType, OnTerminateSetupDataType)
   ON_COMMAND(ID_TerminateReviewGeometries, OnTerminateReviewGeometries)
	ON_COMMAND(IC_TerminateReviewInserts, OnTerminateReviewInserts)

	ON_BN_CLICKED(IDC_LoadDataFiles, OnBnClickedLoadDataFiles)
	ON_BN_CLICKED(IDC_SetupDataType, OnBnClickedSetupDataType)
	ON_BN_CLICKED(IDC_CleanUpGerber, OnBnClickedCleanUpGerber)
	ON_BN_CLICKED(IDC_CreateGeometries, OnBnClickedCreateGeometries)
	ON_BN_CLICKED(IDC_ReviewGeometries, OnBnClickedReviewGeometries)
	ON_BN_CLICKED(IDC_ReviewInserts, OnBnClickedReviewInserts)
END_MESSAGE_MAP()

void CGerberEducatorToolbar::OnInitDialog()
{
	m_bEnableLoadDataFile     = false;
	m_bEnableSetupDataType	  = true;
	m_bEnableCleanUpGerber    = false;
	m_bEnableCreateGeometries = true;
	m_bEnableReviewGeometries = true;
	m_bEnableReviewInserts    = true;
	m_bEnableDeriveNetlist    = false;

	m_reviewGeometriesDlg = NULL;
	m_insertGeometriesDlg = NULL;
	m_setupDataTypeDlg = NULL;
}

void CGerberEducatorToolbar::InitializeButtons()
{
	OnInitDialog();
}

void CGerberEducatorToolbar::updateSelectedEntity()
{
	if (m_reviewGeometriesDlg != NULL)
		m_reviewGeometriesDlg->updateSelectedEntity();

	if (m_insertGeometriesDlg != NULL)
		m_insertGeometriesDlg->updateSelectedEntity();
}

void CGerberEducatorToolbar::OnBnClickedLoadDataFiles()
{
	CGELoadDataDlg dlg;
	dlg.DoModal();

	m_bEnableCleanUpGerber = true;
}

bool CGerberEducatorToolbar::checkStartTool(bool mustHavePCBFile)
{
   bool startToolFlag = true;

	if (getActiveView() == NULL || getActiveView()->GetDocument() == NULL ||
		 (mustHavePCBFile && getActiveView()->GetDocument()->getFileList().GetFirstShown(blockTypePcb) == NULL))
	{
		ErrorMessage("No PCB File is loaded.  Cannot do Create Geometries");
      startToolFlag = false;
   }
   else
   {
      CCEtoODBDoc* camCadDoc = getActiveView()->GetDocument();

      if (camCadDoc->getGerberEducator() != NULL)
      {
		   ErrorMessage("A Gerber Educator Create Geometry session already exists.");
         startToolFlag = false;
         camCadDoc->getGerberEducator()->getDialog().ShowWindow(SW_SHOW);
      }

      if (m_setupDataTypeDlg != NULL)
      {
		   ErrorMessage("A Gerber Educator Setup Data Type session already exists.");
         startToolFlag = false;
         m_setupDataTypeDlg->ShowWindow(SW_SHOW);
      }

      if (m_reviewGeometriesDlg != NULL)
      {
		   ErrorMessage("A Gerber Educator Review Geometry session already exists.");
         startToolFlag = false;
         m_reviewGeometriesDlg->ShowWindow(SW_SHOW);
      }

      if (m_insertGeometriesDlg != NULL)
      {
		   ErrorMessage("A Gerber Educator Review Insert session already exists.");
         startToolFlag = false;
         m_insertGeometriesDlg->ShowWindow(SW_SHOW);
      }
   }

   return startToolFlag;
}

void CGerberEducatorToolbar::OnBnClickedSetupDataType()
{
   if (checkStartTool(false))
   {
		//m_setupDataTypeDlg = new CGESetupDataType(*getActiveView()->GetDocument(), *getActiveView()->GetDocument()->getFileList().GetFirstShown());
		//m_setupDataTypeDlg->Create(IDD_GE_SetupDataType);
		//m_setupDataTypeDlg->ShowWindow(SW_SHOW);
		//m_setupDataTypeDlg->UpdateWindow();
		//
		//if (!m_setupDataTypeDlg->isInitialized())
		//	OnTerminateSetupDataType();
   }
}

void CGerberEducatorToolbar::OnBnClickedCleanUpGerber()
{
	CGECleanUpGerberDlg dlg;
	dlg.DoModal();

	m_bEnableCreateGeometries = true;
}

void CGerberEducatorToolbar::OnBnClickedCreateGeometries()
{
   if (checkStartTool(true))
   {
      CCEtoODBDoc* camCadDoc = getActiveView()->GetDocument();
      camCadDoc->gerberEducatorCreateGeometries();
   }
}

void CGerberEducatorToolbar::OnBnClickedReviewGeometries()
{
   if (checkStartTool(true))
   {
		//m_reviewGeometriesDlg = new CGEReviewGeometriesDlg(*getActiveView()->GetDocument(), *getActiveView()->GetDocument()->getFileList().GetFirstShown());
		//m_reviewGeometriesDlg->Create(IDD_GE_ReviewGeometries);
		//m_reviewGeometriesDlg->ShowWindow(SW_SHOW);
		//m_reviewGeometriesDlg->UpdateWindow();
		//
		//if (!m_reviewGeometriesDlg->isInitialized())
		//	OnTerminateReviewGeometries();
   }
}

void CGerberEducatorToolbar::OnBnClickedReviewInserts()
{
   if (checkStartTool(true))
   {
		//m_insertGeometriesDlg = new CGEReviewInsertsDlg(*getActiveView()->GetDocument(), *getActiveView()->GetDocument()->getFileList().GetFirstShown());
		//m_insertGeometriesDlg->Create(IDD_GE_ReviewInserts);
		//m_insertGeometriesDlg->ShowWindow(SW_SHOW);
		//m_insertGeometriesDlg->UpdateWindow();
		//
		//if (!m_insertGeometriesDlg->isInitialized())
		//	OnTerminateReviewInserts();
   }
}

void CGerberEducatorToolbar::OnTerminateSetupDataType()
{
	if (m_setupDataTypeDlg != NULL)
		m_setupDataTypeDlg->DestroyWindow();

	delete m_setupDataTypeDlg;
	m_setupDataTypeDlg = NULL;
}

void CGerberEducatorToolbar::OnTerminateReviewGeometries()
{
	if (m_reviewGeometriesDlg != NULL)
		m_reviewGeometriesDlg->DestroyWindow();

	delete m_reviewGeometriesDlg;
	m_reviewGeometriesDlg = NULL;
}

void CGerberEducatorToolbar::OnTerminateReviewInserts()
{
	if (m_insertGeometriesDlg != NULL)
		m_insertGeometriesDlg->DestroyWindow();

	delete m_insertGeometriesDlg;
	m_insertGeometriesDlg = NULL;
}


//-----------------------------------------------------------------------------
// CGELoadDataDlg dialog
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CGELoadDataDlg, CDialog)
CGELoadDataDlg::CGELoadDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGELoadDataDlg::IDD, pParent)
{
}

CGELoadDataDlg::~CGELoadDataDlg()
{
}

void CGELoadDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGELoadDataDlg, CDialog)
END_MESSAGE_MAP()

BOOL CGELoadDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//-----------------------------------------------------------------------------
// CGECleanUpGerberDlg dialog
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CGECleanUpGerberDlg, CDialog)
CGECleanUpGerberDlg::CGECleanUpGerberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGECleanUpGerberDlg::IDD, pParent)
{
}

CGECleanUpGerberDlg::~CGECleanUpGerberDlg()
{
}

void CGECleanUpGerberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGECleanUpGerberDlg, CDialog)
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
// CGECreateGeometriesDlg dialog
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CGECreateGeometriesDlg, CDialog)
CGECreateGeometriesDlg::CGECreateGeometriesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGECreateGeometriesDlg::IDD, pParent)
{
}

CGECreateGeometriesDlg::~CGECreateGeometriesDlg()
{
}

void CGECreateGeometriesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGECreateGeometriesDlg, CDialog)
END_MESSAGE_MAP()
