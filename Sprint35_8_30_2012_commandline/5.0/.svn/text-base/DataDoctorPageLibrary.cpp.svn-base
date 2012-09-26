// $Header: /CAMCAD/5.0/DataDoctorPageCentroids.cpp 38    6/17/07 8:50p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "ccview.h"
#include <math.h>
#include "graph.h"
#include "DFT_Outl.h"
#include <float.h>
#include "outline.h"
#include "extents.h"
#include "dft_func.h"
#include "pcbutil.h"
#include "Attrib.h"
#include "polylib.h"
#include "api.h"
#include "Centroid.h"
#include "response.h"
#include "PopupMenu.h"
#include "ODBC_Lib.h"

#include "DataDoctorDialog.h"
#include "DataDoctorPageLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
//_____________________________________________________________________________

IMPLEMENT_DYNAMIC(CDataDoctorLibraryPage, CDataDoctorPropertyPageType)

CDataDoctorLibraryPage::CDataDoctorLibraryPage(CDataDoctorPropertySheet& parent)
	: CDataDoctorPropertyPageType(CDataDoctorLibraryPage::IDD)
     , m_parent(parent)
     , m_updatePending(false)
     , m_dialogIsInitialized(false)
{

   m_doc = &(getCamCadDatabase().getCamCadDoc());
	m_activeFile = m_doc->getFileList().GetOnlyShown(blockTypePcb);

   m_chkUseDatabaseConnection = TRUE;

   addFieldControls();


}

CDataDoctorLibraryPage::~CDataDoctorLibraryPage()
{
}

CCamCadDatabase& CDataDoctorLibraryPage::getCamCadDatabase() 
{ 
   return getParent().getCamCadDatabase(); 
}

void CDataDoctorLibraryPage::addFieldControls()
{
#if defined (DataDoctorResizingDialogFlag)

   addFieldControl(IDC_ActiveLibraryGroup           ,anchorLeft, growHorizontal);
   addFieldControl(IDC_LibraryDatabaseName	       ,anchorLeft, growHorizontal);
   addFieldControl(IDC_BrowseButton                 ,anchorRight);
   addFieldControl(IDC_ChkBxUseDatabaseConnection	 ,anchorLeft);


   CResizingDialogField *groupboxprt, *groupboxpkg, *labelL, *labelR, *leftfield, *rightfield, *centerfield, *centerfield2;

   // For parts tab config
   groupboxprt  = &(addFieldControl(IDC_PRT_CONFIG_GROUP      ,anchorLeft, growHorizontal));
   leftfield  =   &(addFieldControl(IDC_LIST_AVAILABLE_PRT    ,anchorLeft, growBoth));
   rightfield  =  &(addFieldControl(IDC_LIST_SELECTED_PRT     ,anchorRight, growBoth));
   centerfield  = &(addFieldControl(IDC_BTN_ADD_PRT           ,anchorProportionalHorizontal));
   centerfield2 = &(addFieldControl(IDC_BTN_REMOVE_PRT        ,anchorProportionalHorizontal));
   labelL  =      &(addFieldControl(IDC_LBL_AVAILABLE_PRT     ,anchorRelativeToField, leftfield));
   labelR  =      &(addFieldControl(IDC_LBL_SELECTED_PRT      ,anchorRelativeToField, rightfield));

   centerfield->getOperations().addOperation( glueTopEdge, toTopEdge,  groupboxprt);
   centerfield2->getOperations().addOperation(glueTopEdge, toTopEdge,  groupboxprt);

   leftfield->getOperations().addOperation(glueRightEdge, toLeftEdge,  centerfield);
   rightfield->getOperations().addOperation(glueLeftEdge, toRightEdge, centerfield);

   leftfield->getOperations().addOperation(glueTopEdge, toTopEdge,  groupboxprt);
   leftfield->getOperations().addOperation(glueBottomEdge, toBottomEdge, groupboxprt);

   rightfield->getOperations().addOperation(glueTopEdge, toTopEdge,  groupboxprt);
   rightfield->getOperations().addOperation(glueBottomEdge, toBottomEdge, groupboxprt);

   //labelL->getOperations().addOperation(glueLeftEdge, toLeftEdge,  leftfield);
   //labelR->getOperations().addOperation(glueLeftEdge, toLeftEdge,  rightfield);



   // For package tab config
   groupboxpkg  = &(addFieldControl(IDC_PKG_CONFIG_GROUP2     ,anchorLeft, growHorizontal));
   leftfield  =   &(addFieldControl(IDC_LIST_AVAILABLE_PKG    ,anchorLeft, growBoth));
   rightfield  =  &(addFieldControl(IDC_LIST_SELECTED_PKG     ,anchorRight, growBoth));
   centerfield  = &(addFieldControl(IDC_BTN_ADD_PKG           ,anchorProportionalHorizontal));
   centerfield2 = &(addFieldControl(IDC_BTN_REMOVE_PKG        ,anchorProportionalHorizontal));
   labelL  =      &(addFieldControl(IDC_LBL_AVAILABLE_PKG     ,anchorRelativeToField, leftfield));
   labelR  =      &(addFieldControl(IDC_LBL_SELECTED_PKG      ,anchorRelativeToField, rightfield));

   groupboxpkg->getOperations().addOperation(glueTopEdge, toBottomEdge,  groupboxprt);

   centerfield->getOperations().addOperation( glueTopEdge, toTopEdge,  groupboxpkg);
   centerfield2->getOperations().addOperation(glueTopEdge, toTopEdge,  groupboxpkg);

   leftfield->getOperations().addOperation(glueRightEdge, toLeftEdge,  centerfield);
   rightfield->getOperations().addOperation(glueLeftEdge, toRightEdge, centerfield);

   leftfield->getOperations().addOperation(glueTopEdge, toTopEdge,  groupboxpkg);
   leftfield->getOperations().addOperation(glueBottomEdge, toBottomEdge, groupboxpkg);

   rightfield->getOperations().addOperation(glueTopEdge, toTopEdge,  groupboxpkg);
   rightfield->getOperations().addOperation(glueBottomEdge, toBottomEdge, groupboxpkg);


#endif
}

void CDataDoctorLibraryPage::DoDataExchange(CDataExchange* pDX)
{
   CDataDoctorPropertyPageType::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_LIST_AVAILABLE_PRT, m_lstAvailableAttribsPrt);
   DDX_Control(pDX, IDC_LIST_SELECTED_PRT,  m_lstSelectedAttribsPrt);
   DDX_Control(pDX, IDC_LIST_AVAILABLE_PKG, m_lstAvailableAttribsPkg);
   DDX_Control(pDX, IDC_LIST_SELECTED_PKG,  m_lstSelectedAttribsPkg);

   DDX_Check(pDX, IDC_ChkBxUseDatabaseConnection, m_chkUseDatabaseConnection);

   //{{AFX_DATA_MAP(CDataDoctorCentroidsPage)
   //}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CDataDoctorLibraryPage, CDataDoctorPropertyPageType)
   //{{AFX_MSG_MAP(CDataDoctorCentroidsPage)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
   ON_WM_ACTIVATE()


   ON_BN_CLICKED(IDC_LoadButton, OnBnClickedLoadFromLibrary)
   //ON_BN_CLICKED(IDC_StoreButton, OnBnClickedStoreToLibrary)
	ON_BN_CLICKED(IDC_BrowseButton, OnBnClickedBrowseButton)
   ON_BN_CLICKED(IDC_ChkBxUseDatabaseConnection, OnBnClickedChkUseDatabaseConnection)

   ON_BN_CLICKED(IDC_BTN_ADD_PRT,    OnBnClickedAddPrtAttrib)
   ON_BN_CLICKED(IDC_BTN_REMOVE_PRT, OnBnClickedRemovePrtAttrib)
   ON_BN_CLICKED(IDC_BTN_ADD_PKG,    OnBnClickedAddPkgAttrib)
   ON_BN_CLICKED(IDC_BTN_REMOVE_PKG, OnBnClickedRemovePkgAttrib)


END_MESSAGE_MAP()

/******************************************************************************
*/
void CDataDoctorLibraryPage::OnBnClickedAddPrtAttrib()
{
	int nCount = m_lstAvailableAttribsPrt.GetSelCount();
	if (nCount < 1)
		return;

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount, nCount);
	m_lstAvailableAttribsPrt.GetSelItems(nCount, aryListBoxSel.GetData());

	// Removed the item from Available listbox and add it into Selected listbox
	for (int i = nCount-1; i > -1; i--)
	{
		CDataDoctorLibraryAttrib *item = (CDataDoctorLibraryAttrib *)m_lstAvailableAttribsPrt.GetItemData( aryListBoxSel.GetAt(i) );
      item->SetActive(true);

		int index = m_lstSelectedAttribsPrt.AddString(item->GetName());
		m_lstSelectedAttribsPrt.SetItemData(index, (DWORD)item);

		m_lstAvailableAttribsPrt.DeleteString( aryListBoxSel.GetAt(i) );
	}

   this->getParent().getDataDoctor().GetPartLibraryAttribMap().SaveSettings();
}

void CDataDoctorLibraryPage::OnBnClickedRemovePrtAttrib()
{
	int nCount = m_lstSelectedAttribsPrt.GetSelCount();
	if (nCount < 1)
		return;

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount, nCount);
	m_lstSelectedAttribsPrt.GetSelItems(nCount, aryListBoxSel.GetData());

	// Removed the item from Selected listbox and put it back into Available listbox
	for (int i = nCount-1; i > -1; i--)
	{
		CDataDoctorLibraryAttrib *item = (CDataDoctorLibraryAttrib *)m_lstSelectedAttribsPrt.GetItemData( aryListBoxSel.GetAt(i) );
      item->SetActive(false);
		
      int index = m_lstAvailableAttribsPrt.AddString(item->GetName());
		m_lstAvailableAttribsPrt.SetItemData(index, (DWORD)item);

		m_lstSelectedAttribsPrt.DeleteString( aryListBoxSel.GetAt(i) );
	}

   this->getParent().getDataDoctor().GetPartLibraryAttribMap().SaveSettings();
}


/******************************************************************************
*/
void CDataDoctorLibraryPage::OnBnClickedAddPkgAttrib()
{
	int nCount = m_lstAvailableAttribsPkg.GetSelCount();
	if (nCount < 1)
		return;

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount, nCount);
	m_lstAvailableAttribsPkg.GetSelItems(nCount, aryListBoxSel.GetData());

	// Removed the item from Available listbox and add it into Selected listbox
	for (int i = nCount-1; i > -1; i--)
	{
		CDataDoctorLibraryAttrib *item = (CDataDoctorLibraryAttrib *)m_lstAvailableAttribsPkg.GetItemData( aryListBoxSel.GetAt(i) );
      item->SetActive(true);

		int index = m_lstSelectedAttribsPkg.AddString(item->GetName());
		m_lstSelectedAttribsPkg.SetItemData(index, (DWORD)item);

		m_lstAvailableAttribsPkg.DeleteString( aryListBoxSel.GetAt(i) );
	}

   this->getParent().getDataDoctor().GetPackageLibraryAttribMap().SaveSettings();
}

void CDataDoctorLibraryPage::OnBnClickedRemovePkgAttrib()
{
	int nCount = m_lstSelectedAttribsPkg.GetSelCount();
	if (nCount < 1)
		return;

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount, nCount);
	m_lstSelectedAttribsPkg.GetSelItems(nCount, aryListBoxSel.GetData());

	// Removed the item from Selected listbox and put it back into Available listbox
	for (int i = nCount-1; i > -1; i--)
	{
		CDataDoctorLibraryAttrib *item = (CDataDoctorLibraryAttrib *)m_lstSelectedAttribsPkg.GetItemData( aryListBoxSel.GetAt(i) );
      item->SetActive(false);
		
      int index = m_lstAvailableAttribsPkg.AddString(item->GetName());
		m_lstAvailableAttribsPkg.SetItemData(index, (DWORD)item);

		m_lstSelectedAttribsPkg.DeleteString( aryListBoxSel.GetAt(i) );
	}

   this->getParent().getDataDoctor().GetPackageLibraryAttribMap().SaveSettings();
}



/******************************************************************************
*/

void CDataDoctorLibraryPage::OnBnClickedLoadFromLibrary()
{
   // Group load

   ErrorMessage("Load From Library Not Yet Implemented", "");

}

/******************************************************************************
*/


/******************************************************************************
*/
void CDataDoctorLibraryPage::OnBnClickedBrowseButton()
{
   //getGrid().ClearSelections();

   CDBInterface db;
   db.PresentDBFileChooser();
   GetDlgItem(IDC_LibraryDatabaseName)->SetWindowText( db.GetLibraryDatabaseName() );


}

/******************************************************************************
*/
void CDataDoctorLibraryPage::OnBnClickedChkUseDatabaseConnection()
{
   UpdateData(TRUE); 

   // Update sticky setting, the DB interface owns this setting
   CDBInterface db;
   db.SetConnectionAttemptEnabled(m_chkUseDatabaseConnection?true:false);

   // Enable/Disable DB related controls in Data Doc
   this->getParent().EnableDisableDatabaseControls();
}

/******************************************************************************
*/
void CDataDoctorLibraryPage::EnableDatabaseControls(bool attempt, bool allowed)
{
   if (DialogIsInitialized())
   {
      // These are set according to whether or not the user wants to attempt connection.
      GetDlgItem(IDC_LibraryDatabaseName)->EnableWindow( attempt );
      GetDlgItem(IDC_BrowseButton)->EnableWindow( attempt );

      // These are set according to whether or not the system is going to allow the user to attempt connection.
      GetDlgItem(IDC_ChkBxUseDatabaseConnection)->EnableWindow( allowed );
      GetDlgItem(IDC_ActiveLibraryGroup)->EnableWindow( allowed );

      // This is set to show if system will allow connection and user wishes to attempt connection.
      m_chkUseDatabaseConnection = attempt && allowed;

      UpdateData(FALSE);
   }
}

/******************************************************************************
*/
void CDataDoctorLibraryPage::update()
{
   // Nothing to do, but we're ready should something arise
   m_updatePending = false;
}

/******************************************************************************
*/

BOOL CDataDoctorLibraryPage::OnSetActive()
{
   CDBInterface db;
	CString partLibraryDatabaseName = db.GetLibraryDatabaseName();
	GetDlgItem(IDC_LibraryDatabaseName)->SetWindowText(partLibraryDatabaseName);
   this->EnableDatabaseControls(db.GetConnectionAttemptEnabled(), db.GetConnectionAttemptAllowed());

   this->getParent().SaveAsInitialPage(DdLibraryPage);

   if (IsUpdatePending())
      update();

   return CDataDoctorPropertyPageType::OnSetActive();
}

/******************************************************************************
*/
BOOL CDataDoctorLibraryPage::OnInitDialog() 
{

   CDataDoctorPropertyPageType::OnInitDialog();

   // Database name
   {
      // Within scope parens so DB will be closed/deleted immediately after use
      CDBInterface db;
	   GetDlgItem(IDC_LibraryDatabaseName)->SetWindowText( db.GetLibraryDatabaseName() );
      this->EnableDatabaseControls(db.GetConnectionAttemptEnabled(), db.GetConnectionAttemptAllowed());
   }

   CDataDoctorLibraryAttrib *libattr = NULL;
   CString* attrname;

   CDataDoctorLibraryAttribMap &attmap = this->getParent().getDataDoctor().GetPartLibraryAttribMap();

   for (attmap.GetFirstSorted(attrname, libattr); libattr != NULL; attmap.GetNextSorted(attrname, libattr))
   {
      if (libattr->IsActive())
      {
         int indx = m_lstSelectedAttribsPrt.AddString(libattr->GetName());
         m_lstSelectedAttribsPrt.SetItemData(indx, (DWORD)libattr);
      }
      else
      {
         int indx = m_lstAvailableAttribsPrt.AddString(libattr->GetName());
         m_lstAvailableAttribsPrt.SetItemData(indx, (DWORD)libattr);
      }
   }


   CDataDoctorLibraryAttribMap &attmap2 = this->getParent().getDataDoctor().GetPackageLibraryAttribMap();

   for (attmap2.GetFirstSorted(attrname, libattr); libattr != NULL; attmap2.GetNextSorted(attrname, libattr))
   {
      if (libattr->IsActive())
      {
         int indx = m_lstSelectedAttribsPkg.AddString(libattr->GetName());
         m_lstSelectedAttribsPkg.SetItemData(indx, (DWORD)libattr);
      }
      else
      {
         int indx = m_lstAvailableAttribsPkg.AddString(libattr->GetName());
         m_lstAvailableAttribsPkg.SetItemData(indx, (DWORD)libattr);
      }
   }

   m_dialogIsInitialized = true;
	
   return TRUE;
}

/******************************************************************************
*/
///void CDataDoctorCentroidsPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
///{
///   CDataDoctorPropertyPageType::OnActivate(nState, pWndOther, bMinimized);

   // TODO: Add your message handler code here
///}


/******************************************************************************
*/
BEGIN_EVENTSINK_MAP(CDataDoctorLibraryPage, CResizingDialog)
END_EVENTSINK_MAP()




/******************************************************************************
*/
void CDataDoctorLibraryPage::OnBnClickedBtnClose()
{
   OnCancel();
}

/******************************************************************************
*/
void CDataDoctorLibraryPage::OnCancel()
{
   m_doc->OnDoneEditing();

   CDataDoctorPropertyPageType::OnCancel();
}

/******************************************************************************
*/

BOOL CDataDoctorLibraryPage::OnKillActive()
{
   //m_doc->OnDoneEditing();

   getParent().updatePropertyPages(this);

   return CDataDoctorPropertyPageType::OnKillActive();
}

/******************************************************************************
*/



