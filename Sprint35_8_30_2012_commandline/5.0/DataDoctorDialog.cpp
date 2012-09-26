// $Header: /CAMCAD/4.6/DataDoctorDialog.cpp 82    5/08/07 2:48p Rick Faltersack $

#include "StdAfx.h"
#include "DataDoctorDialog.h"
#include "afxdisp.h"
#include "Response.h"
#include "DialogToolbar.h"
#include "MainFrm.h"
#include "PopupMenu.h"


FilterSchemeTag intToFilterScheme(int tagValue)
{
   FilterSchemeTag retval = filterSchemeUndefined;

   switch (tagValue)
   {
   case filterSchemeFiducial:         retval = filterSchemeFiducial;         break;
   case filterSchemeTooling :         retval = filterSchemeTooling;          break;
   case filterSchemeFiducialDefault:  retval = filterSchemeFiducialDefault;  break;
   case filterSchemeToolingDefault:   retval = filterSchemeToolingDefault;   break;
   }

   return retval;
}

//_____________________________________________________________________________
CDataDoctorFilterSchemeTerm::CDataDoctorFilterSchemeTerm(const CString& regularExpression,const CString& searchFlags) :
   m_regularExpression(regularExpression),
   m_searchFlagsString(searchFlags)
{
}

void CDataDoctorFilterSchemeTerm::setSearchFlags(const CString& searchFlags)
{
   m_searchFlagsString = searchFlags;
   m_searchFlagsString.MakeLower();

   m_searchRefDesFlag       = (searchFlags.Find("refdes"  ) >= 0);
   m_searchGeometryNameFlag = (searchFlags.Find("geometry") >= 0);
   m_searchPadstackNameFlag = (searchFlags.Find("padstack") >= 0);
}

bool CDataDoctorFilterSchemeTerm::matches(CCamCadDatabase& camCadDatabase,DataStruct& data)
{
   bool retval = false;

   if (!retval && m_searchRefDesFlag)
   {
      retval = m_regularExpression.matches(data.getInsert()->getRefname());
   }

   if (!retval)
   {
      BlockStruct* geometry = camCadDatabase.getBlock(data.getInsert()->getBlockNumber());

      if (m_searchGeometryNameFlag)
      {
         retval = m_regularExpression.matches(geometry->getName());
      }

      if (!retval && m_searchPadstackNameFlag && geometry->getBlockType() == blockTypePadstack)
      {
         retval = m_regularExpression.matches(geometry->getName());
      }
   }

   return retval;
}

//_____________________________________________________________________________
CDataDoctorFilterScheme::CDataDoctorFilterScheme(FilterSchemeTag filterSchemeTag) :
   m_filterSchemeTag(filterSchemeTag)
{
}

void CDataDoctorFilterScheme::addTerm(const CString& regularExpression,const CString& searchFlags)
{
   CDataDoctorFilterSchemeTerm* term = new CDataDoctorFilterSchemeTerm(regularExpression,searchFlags);
   m_terms.Add(term);
}

bool CDataDoctorFilterScheme::matches(CCamCadDatabase& camCadDatabase,DataStruct& data)
{
   bool retval = false;

   for (int index = 0;index < m_terms.GetCount();index++)
   {
      CDataDoctorFilterSchemeTerm* term = m_terms.GetAt(index);

      if (term->matches(camCadDatabase,data))
      {
         retval = true;
         break;
      }
   }

   return retval;
}

void CDataDoctorFilterScheme::loadFromRegistry()
{
}

//_____________________________________________________________________________
CDataDoctorFilterSchemes::CDataDoctorFilterSchemes()
{
}

CDataDoctorFilterScheme& CDataDoctorFilterSchemes::getScheme(FilterSchemeTag filterSchemeTag)
{
   CDataDoctorFilterScheme* filterScheme = NULL;

   if (filterSchemeTag < m_schemes.GetSize())
   {
      filterScheme = m_schemes.GetAt(filterSchemeTag);
   }

   if (filterScheme == NULL)
   {
      filterScheme = new CDataDoctorFilterScheme(filterSchemeTag);
      m_schemes.SetAtGrow(filterSchemeTag,filterScheme);

      switch (filterSchemeTag)
      {
      case filterSchemeFiducialDefault:     
         filterScheme->addTerm(".*FID.*","refdes,geometry,padstack");
         filterScheme->addTerm(".*FUD.*","refdes,geometry,padstack");
         filterScheme->addTerm(".*OPT.*","refdes,geometry,padstack");
         break;
      case filterSchemeToolingDefault :       
         filterScheme->addTerm(".*H.*","refdes,geometry,padstack");
         filterScheme->addTerm(".*MH.*","refdes,geometry,padstack");
         break;
      case filterSchemeFiducial:     
      case filterSchemeTooling :       
         filterScheme->loadFromRegistry();
         break;
      }
   }

   return *filterScheme;
}

//_____________________________________________________________________________

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CDataDoctorPropertySheet, CDataDoctorPropertySheetType)

CDataDoctorPropertySheet::CDataDoctorPropertySheet(CDataDoctor& dataDoctor)
	: CDataDoctorPropertySheetType("Data Doctor"),
     m_dataDoctor(dataDoctor),
     m_componentsPage(*this),
     m_partsPage(*this),
     m_parallelAnalysisPage(*this),
     m_centroidsPage(*this),
     m_packagesPage(*this),
     m_subclassesPage(*this),
     m_libraryPage(*this),
     m_initialPage(DdComponentsPage)
{
   init();
}

void CDataDoctorPropertySheet::init()
{
   AddPage(&m_componentsPage);
   AddPage(&m_partsPage);

   AddPage(&m_parallelAnalysisPage);

   AddPage(&m_centroidsPage);

   AddPage(&m_packagesPage);

   AddPage(&m_subclassesPage);
   //CGTabTableList &tableList = this->getDataDoctor().getCamCadDatabase().getCamCadData().getTableList();
   //m_subclassesPage.m_subclassList.LoadFromTables(tableList);

   AddPage(&m_libraryPage);

   m_psh.dwFlags |= PSH_NOAPPLYNOW;
   m_psh.dwFlags &= ~PSH_HASHELP;

   DdPageTag usersLastVisitedPage = LookupInitialPage();
   if (usersLastVisitedPage != DdUnknownPage)
      m_initialPage = usersLastVisitedPage;
}

//***************************************************************************************
//
// Keep the next three functions in sync with the DdPageTag definitions in DataDoctor.h.
// StringToDdPageTag(..)
// DdPageTagToString(..)
// GetInitialPagePtr()
//

DdPageTag CDataDoctorPropertySheet::StringToDdPageTag(CString str)
{
   if (str.CompareNoCase("Components") == 0)
      return DdComponentsPage;

   if (str.CompareNoCase("Parts") == 0)
      return DdPartsPage;

   if (str.CompareNoCase("Centroids") == 0)
      return DdCentroidsPage;

   if (str.CompareNoCase("Packages") == 0)
      return DdPackagesPage;

   if (str.CompareNoCase("Subclasses") == 0)
      return DdSubclassesPage;

   if (str.CompareNoCase("Parallel Analysis") == 0)
      return DdParallelAnalysisPage;

   if (str.CompareNoCase("Library") == 0)
      return DdLibraryPage;

   return DdUnknownPage;
}

CString CDataDoctorPropertySheet::DdPageTagToString(DdPageTag tag)
{
   switch (tag)
   {
   case DdComponentsPage:
      return "Components";

   case DdPartsPage:
      return "Parts";

   case DdCentroidsPage:
      return "Centroids";

   case DdPackagesPage:
      return "Packages";

   case DdSubclassesPage:
      return "Subclasses";

   case DdParallelAnalysisPage:
      return "Parallel Analysis";

   case DdLibraryPage:
      return "Library";
   }

   return "Unknown";
}

CDataDoctorPropertyPageType *CDataDoctorPropertySheet::GetInitialPagePtr()
{
   void *initpageptr = NULL;

   switch (m_initialPage)
   {
   case DdComponentsPage:
      initpageptr = &m_componentsPage;
      break;
   case DdPartsPage:
      initpageptr = &m_partsPage;
      break;
   case DdCentroidsPage:
      initpageptr = &m_centroidsPage;
      break;
   case DdPackagesPage:
      initpageptr = &m_packagesPage;
      break;
   case DdSubclassesPage:
      initpageptr = &m_subclassesPage;
      break;
   case DdParallelAnalysisPage:
      initpageptr = &m_parallelAnalysisPage;
      break;
   case DdLibraryPage:
      initpageptr = &m_libraryPage;
      break;
   }

   return (CDataDoctorPropertyPageType*)initpageptr;
}

//***************************************************************************************

void CDataDoctorPropertySheet::SaveAsInitialPage(DdPageTag page)
{
   // Save setting in registry, return true if is appears to work, otherwise false
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      if (settings.setValue("Initial Page", DdPageTagToString(page)))
      {
         // return true;
      }
   }

   //return false;   
}

DdPageTag CDataDoctorPropertySheet::LookupInitialPage()
{
   CString pagename;

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      if (settings.getValue("Initial Page", pagename))
      {
         // Ok, got it.
      }
   }

   return StringToDdPageTag(pagename);  // Ok if pagename is still blank

}

void CDataDoctorPropertySheet::updatePropertyPages(CPropertyPage* sendingPage)
{
   if (sendingPage != &m_partsPage)
   {
      m_partsPage.QueueUpdate();
   }

   if (sendingPage != &m_componentsPage)
   {
      m_componentsPage.QueueUpdate();
   }

   if (sendingPage != &m_centroidsPage)
   {
      m_centroidsPage.QueueUpdate();
   }

   if (sendingPage != &m_packagesPage)
   {
      m_packagesPage.QueueUpdate();
   }

   if (sendingPage != &m_subclassesPage)
   {
      m_subclassesPage.QueueUpdate();
   }

   if (sendingPage != &m_libraryPage)
   {
      m_libraryPage.QueueUpdate();
   }
}

void CDataDoctorPropertySheet::EnableDisableDatabaseControls()
{
   // Gets on/off setting from DB itself, updates controls in GUI
   // on pages that have such controls.

   CDBInterface db;
   bool attempt = db.GetConnectionAttemptEnabled();
   bool allowed = db.GetConnectionAttemptAllowed();

   m_partsPage.EnableDatabaseControls(attempt);
   m_centroidsPage.EnableDatabaseControls(attempt);
   m_packagesPage.EnableDatabaseControls(attempt);
   m_subclassesPage.EnableDatabaseControls(attempt);
   m_libraryPage.EnableDatabaseControls(attempt, allowed);
}

BEGIN_MESSAGE_MAP(CDataDoctorPropertySheet, CDataDoctorPropertySheetType)
   ON_WM_CLOSE()
END_MESSAGE_MAP()

// CStencilGeneratorPropertySheet message handlers

BOOL CDataDoctorPropertySheet::OnInitDialog()
{
   GetDlgItem(IDCANCEL)->SetWindowText("Close");
   GetDlgItem(IDHELP  )->ShowWindow(SW_HIDE);
   GetDlgItem(IDOK    )->ShowWindow(SW_HIDE);
   BOOL bResult = CDataDoctorPropertySheetType::OnInitDialog();

   // Select initial active page
   CDataDoctorPropertyPageType *initpageptr = GetInitialPagePtr();
   if (initpageptr != NULL)
      SetActivePage(initpageptr);

   return bResult;
}

void CDataDoctorPropertySheet::OnClose()
{
   // TODO: Add your message handler code here and/or call default

   CDataDoctorPropertySheetType::OnClose();
}


//_____________________________________________________________________________
// CDataDoctorPinMappingDlg dialog
IMPLEMENT_DYNAMIC(CDataDoctorPinMappingDlg, CDialog)
CDataDoctorPinMappingDlg::CDataDoctorPinMappingDlg(CCamCadDatabase& camCadDatabase, CDataDoctorPart *ddpart, CWnd* pParent /*= NULL*/)
: CResizingDialog(CDataDoctorPinMappingDlg::IDD, pParent)
   , m_ddpart(ddpart)
   , m_unmapped_pin_option_name("Unmapped")
   , m_pinMappingGrid(this)
   , m_columnDefMap("DD Pin Mapping Grid")
   , m_camCadDatabase(camCadDatabase)
{
   m_pFile = m_camCadDatabase.getSingleVisiblePcb();

   // Set up the column definitions.
   m_columnDefMap.AddColDef( QPin,     &m_colPinRefName );
   m_columnDefMap.AddColDef( QMapping, &m_colPinMappedName );
   m_columnDefMap.AddColDef( QNet,     &m_colPinSampleNetName );
   m_columnDefMap.AddColDef( QPins,    &m_colPinsDescriptor );

   // Load possible user overrides (column swaps)
   m_columnDefMap.LoadSettings();

   addFieldControl(IDC_LABEL1                 , anchorLeft); // "Nets shown for component:"
   addFieldControl(IDC_COMBO_COMPONENTS       , anchorLeft, growVertical); 
	addFieldControl(IDC_PinMappingGridStatic   , anchorLeft, growBoth);
	addFieldControl(IDOK                       , anchorBottomRight);
   addFieldControl(IDCANCEL                   , anchorBottomRight);
}

CDataDoctorPinMappingDlg::~CDataDoctorPinMappingDlg()
{
}

void CDataDoctorPinMappingDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);//CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PinMappingGridStatic, m_pinMappingGrid);
	DDX_Control(pDX, IDC_COMBO_COMPONENTS, m_ComponentCombox);
}

BEGIN_MESSAGE_MAP(CDataDoctorPinMappingDlg, CResizingDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_CBN_SELCHANGE(IDC_COMBO_COMPONENTS, OnCbnSelchangeRefName)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDataDoctorPinMappingDlg, CResizingDialog)
END_EVENTSINK_MAP()

BOOL CDataDoctorPinMappingDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

   AddComponents(m_ComponentCombox, m_ddpart->getReferenceDesignators(), "," , AddCompOperation_OverWrite);

   m_pinMappingGrid.AttachGrid(this, IDC_PinMappingGridStatic);

	fillGrid();

   CString pinNameOptionsString;
   if (m_ddpart != NULL)
      pinNameOptionsString = m_ddpart->getPinNameOptionsString("|");
   pinNameOptionsString += ("|No Connect|" + m_unmapped_pin_option_name + "|");

   m_pinMappingGrid.SetMappingOptions(pinNameOptionsString);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDataDoctorPinMappingDlg::AddComponents(CComboBox &destItem, CString srcCompName, 
                                             CString delimiter, AddCompOperationTag operation)
{   
   int startpos, endpos;
   int strlength = srcCompName.GetLength();
   
   if(operation == AddCompOperation_OverWrite)
      destItem.Clear();

   for(startpos = 0, endpos = 0; startpos < strlength; startpos++)
   {
      endpos = srcCompName.Find(delimiter,startpos);
      if(endpos >= startpos)
      {
         destItem.AddString(srcCompName.Mid(startpos,endpos - startpos));
         startpos = endpos;
      }
      else //last component
      {
         if(startpos < strlength)
            destItem.AddString(srcCompName.Mid(startpos,strlength - startpos));
         break;
      }
   }

   destItem.SetCurSel((destItem.GetCount())?0:-1);
}

void CDataDoctorPinMappingDlg::AfterEditGrid(long row, long col)
{
   if (row >= 0 && row < m_pinMappingGrid.GetNumberRows() && col == m_colPinMappedName)
   {
      CString value = m_pinMappingGrid.QuickGetText(col, row);

      if (value.Compare(m_unmapped_pin_option_name) == 0)
         m_pinMappingGrid.QuickSetText(col, row, "");

      // When no settings are in name column, the column will be very narrow.
      // Resize/redraw grid to make sure new setting is visible.
      m_pinMappingGrid.BestFit(0, m_pinMappingGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      m_pinMappingGrid.RedrawAll();
   }
}

void CDataDoctorPinMappingDlg::fillGrid()
{
   // This fillGrid() should be called only one time at init time of dialog.
   // Because settings are not saved until OK is clicked, calling this again
   // will overwrite settings changed by user with original initial settings.

   m_pinMappingGrid.RemoveAllRows();

   if (m_ddpart != NULL)
   {
      // Pins are supposed to be the same on all comps for same part, so just use first
      // comp in list to get pin refnames. Will also be used to get initial settings
      // for current pin name mapping.
      CDataDoctorComponent *ddcomp = m_ddpart->getHeadComponent();
      if (ddcomp != NULL)
      {
         int pinfunctionKW = ddcomp->getDeviceToPackagePinmapKeywordIndex(m_camCadDatabase);

         m_pinMappingGrid.SetNumberRows(ddcomp->getPinCount()); 

         POSITION pinpos = ddcomp->getPinStartPosition();
         for (int index = 0; pinpos != NULL; index++)
         {
            CDataDoctorPin *ddpin;
            CString pinSortablePinref;
            ddcomp->getPinNextAssoc(pinpos, pinSortablePinref, ddpin);

            CString pinRefname = ddpin->getCompPin().getPinName();

            CString pinFunctionName;
            Attrib *attrib;
            if (ddpin->getCompPin().lookUpAttrib(pinfunctionKW, attrib))
               pinFunctionName = attrib->getStringValue();

            m_pinMappingGrid.QuickSetText( this->m_colPinRefName, index, pinRefname.Trim());
            m_pinMappingGrid.QuickSetParam(this->m_colPinRefName, index, m_pinMappingGrid.DD_CELLTYPE_REFNAME);
            m_pinMappingGrid.QuickSetText(this->m_colPinMappedName, index, pinFunctionName.Trim());
         }
      }
   }

   augmentGrid(false);

   m_pinMappingGrid.ReSort(QPin);
   m_pinMappingGrid.BestFit(0, m_pinMappingGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
}

CString CDataDoctorPinMappingDlg::getNetCompPinNames(NetStruct *net)
{
   // Comma separated list of comppin refs on net
   CString compPinNames;

   if (net != NULL)
   {
      //search for associated pins
      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos != NULL)
      {
         CompPinStruct *assocPin = net->getNextCompPin(cpPos);

         if (assocPin != NULL)
         {
            if (!compPinNames.IsEmpty())
               compPinNames += ",";

            compPinNames += assocPin->getPinRef('.');
         }
      }
   }

   return compPinNames;
}

void CDataDoctorPinMappingDlg::OnBnClickedOk()
{
	CResizingDialog::OnOK();

	CString pinRefsToPinNamesMapString;

   for (int row = 0; row < m_pinMappingGrid.GetNumberRows(); row++)
	{
      CString pinRef = m_pinMappingGrid.QuickGetText(m_colPinRefName, row);
		CString pinName = m_pinMappingGrid.QuickGetText(m_colPinMappedName, row);

		if (!pinRefsToPinNamesMapString.IsEmpty())
			pinRefsToPinNamesMapString += "|";
		pinRefsToPinNamesMapString.AppendFormat("%s=%s ", pinRef, pinName);
	}

	m_pinRefsToPinNamesMapString = pinRefsToPinNamesMapString;
}

void CDataDoctorPinMappingDlg::OnCbnSelchangeRefName()
{
   augmentGrid(true);
}

void CDataDoctorPinMappingDlg::augmentGrid(bool resizeAndRedraw)
{
   // Augment grid with additional info based on selected component.
   // Includes netnames and other comppins on nets.
   // User can't change this stuff, it is reference only.
   // This is separate from fillGrid() because calling fillGrid()
   // where only the augmentation info is needing update causes 
   // users settings that are not yet OK'd to be reset to originals.
   // So we want fillGrid() to happen only once, but this augmentation
   // can happen many times.

   CString selectedCompRefname = "";
   int selectedIndex = m_ComponentCombox.GetCurSel();
   if( selectedIndex != CB_ERR )
      m_ComponentCombox.GetLBText(selectedIndex, selectedCompRefname);

   if (!selectedCompRefname.IsEmpty())
   {
      for (int row = 0; row < m_pinMappingGrid.GetNumberRows(); row++)
      {
         CString pinRef = m_pinMappingGrid.QuickGetText(m_colPinRefName, row);
         ///CString pinName = m_pinMappingGrid.QuickGetText(m_colPinMappedName, row);

         CString netName;
         CString compPinsOnNet;

         NetStruct *net = NULL;
         FindCompPin(m_pFile, selectedCompRefname, pinRef, &net);
         if (net != NULL)
         {
            netName = net->getNetName();
            compPinsOnNet = getNetCompPinNames(net);
         }

         m_pinMappingGrid.QuickSetText(this->m_colPinSampleNetName, row, netName);
         m_pinMappingGrid.QuickSetText(this->m_colPinsDescriptor, row, compPinsOnNet);
      }
   }

   if (resizeAndRedraw)
   {
      m_pinMappingGrid.BestFit(0, m_pinMappingGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      m_pinMappingGrid.RedrawAll();
   }
}

///////////////////////////////////////////////////////////////////////////////////////////
// 
// PARTS GRID
//

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void CDDPinMapGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	

   if (this->m_parentDlg != NULL)
   {
      SetNumberCols(m_parentDlg->m_columnDefMap.getSize());
      SetNumberRows(0);
     
      for (int indx = 0; indx < m_parentDlg->m_columnDefMap.getSize(); indx++)
      {
         CColumnDef *cd = m_parentDlg->m_columnDefMap.getAt(indx);
         CString colname = cd->GetTitle();
         int *colindxptr = cd->GetColIndexPtr();

         QuickSetText(*colindxptr, -1, colname);
      }


      // Mapped Name option menu, same for all cells in column
      SetMappingOptions(""); // Don't know options yet, but let it set cell types anyway

      // Center net name and pin name
      CUGCell cell;
      GetColDefault(this->m_parentDlg->m_colPinSampleNetName, &cell);
      cell.SetAlignment(UG_ALIGNCENTER);
      SetColDefault(this->m_parentDlg->m_colPinSampleNetName, &cell);

      GetColDefault(this->m_parentDlg->m_colPinRefName, &cell);
      cell.SetAlignment(UG_ALIGNCENTER);
      SetColDefault(this->m_parentDlg->m_colPinRefName, &cell);

      EnableColSwapping(TRUE);

      BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      SetColWidth(-1, 0); // get rid of "row heading"

      SetSetUp(true);
   }
}

void CDDPinMapGrid::SetMappingOptions(CString options)
{
   CUGCell cell;
   GetColDefault(this->m_parentDlg->m_colPinMappedName, &cell); 
   cell.SetCellType(UGCT_DROPLIST); 
   CString optionlist(options);
   optionlist.Replace("|", "\n");
   cell.SetLabelText( optionlist ); 
   SetColDefault(this->m_parentDlg->m_colPinMappedName, &cell);
}

int CDDPinMapGrid::OnEditStart(int col, long row, CWnd **edit)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(**edit);

   //*edit = &m_myCUGEdit;

   // No cells are text-box editable

   return false;
}


/////////////////////////////////////////////////////////////////////////////
//	OnCellTypeNotify
//		This notification is sent by the celltype and it is different from cell-type
//		to celltype and even from notification to notification.  It is usually used to
//		provide the developer with some feed back on the cell events and sometimes to
//		ask the developer if given event is to be accepted or not
//	Params:
//		ID			- celltype ID
//		col, row	- co-ordinates cell that is processing the message
//		msg			- message ID to identify current process
//		param		- additional information or object that might be needed
//	Return:
//		TRUE - to allow celltype event
//		FALSE - to disallow the celltype event
int CDDPinMapGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(param);

   //if (msg == UGCT_DROPLISTSELECT)
   //{
   //   int jj = 0;
   //}

   // Handy msg code that is not in the html help files
   // UGCT_DROPLISTPOSTSELECT
   if (msg == UGCT_DROPLISTPOSTSELECT)
   {
      this->m_parentDlg->AfterEditGrid(row, col);
   }
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//	OnColSwapped
//		Called just after column-swap operation was completed.
//	Params:
//		fromCol - where the col originated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		<none>
void CDDPinMapGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   this->m_parentDlg->m_columnDefMap.Update(this);
}

//void CDataDoctorPartsPage::OnShowWindow(BOOL bShow, UINT nStatus)
//{
//	
//}


///////////////////////////////////////////////////////////////////////////////////////////
// 
// CViewLiteToolbar
//

CViewLiteToolbar::CViewLiteToolbar(CResizingPropertyPage& parentPropPage)
  : CResizingPropertyPageToolBar(parentPropPage)
  , m_tbhandler(getActiveView()->GetDocument(), this)
  , m_doc(getActiveView()->GetDocument())
{
}


CViewLiteToolbarAA::CViewLiteToolbarAA()
  : m_tbhandler(getActiveView()->GetDocument(), this)
  , m_doc(getActiveView()->GetDocument())
{
}

///////////////////////////////////////////////////////////////////////////////////////////
// 
// CViewLiteToolbarHandler
//

void CViewLiteToolbarHandler::OnBnClickedZoomIn()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->ScaleDenom *= 2.0;

      view->UpdateScale();

      view->Invalidate();
   }
}

void CViewLiteToolbarHandler::OnBnClickedZoomInOrigin()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->ScaleDenom *= 2.0;

      view->UpdateScale();

      view->Invalidate();

      view->PanCoordinate(0.0, 0.0, FALSE);
   }
}

void CViewLiteToolbarHandler::OnBnClickedColorsetsCustom()
{
   if (m_doc != NULL)
   {
      CWnd *toolbarWnd = m_myBoss->GetParent();
      CWnd *dialogWnd = m_myBoss->GetParentOwner();

      CRect rc1;
      toolbarWnd->GetWindowRect(&rc1);
      CRect rc2;
      dialogWnd->GetWindowRect(&rc2);

      CPoint windowRelativePoint;
      windowRelativePoint.x = rc1.left - rc2.left;
      windowRelativePoint.y = rc1.top - rc2.top;

      CPopupMenu popupMenu;

      for (int i = 0; i < MAX_CUSTOM_COLORSETS; i++)
      {
         popupMenu.addItem(m_doc->CustomColorsetButtonIDs[i], m_doc->CustomLayersetNames.GetAt(i));
      }


      popupMenu.trackMenu(TPM_LEFTALIGN, windowRelativePoint, dialogWnd);
   }
}

void CViewLiteToolbarHandler::OnBnClickedLayerTypes()
{
   if (m_doc != NULL)
   {
      m_doc->OnLayertypes();
   }
}

/******************************************************************************
*/
void CViewLiteToolbarHandler::OnBnClickedShowPolyFills()
{
   if (m_doc != NULL)
   {
      m_doc->OnShowFills();
      m_doc->OnRedraw();
   }
}

/******************************************************************************
*/
void CViewLiteToolbarHandler::OnBnClickedShowApertureFills()
{
   if (m_doc != NULL)
   {
      m_doc->OnShowApFills();
      m_doc->OnRedraw();
   }
}

//******************************************************************************
//******************************************************************************

void CEditNumber::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // Current selection will be replaced, so chars in there don't matter, remove them from
   // consideration during validation tests.
   CString curTxt;
   this->GetWindowText(curTxt);
   int startIndx = -1, endIndx = -1;
   this->GetSel(startIndx, endIndx);
   if (startIndx > -1 && endIndx > startIndx)
      curTxt.Delete(startIndx, endIndx - startIndx);


   if ((isdigit(nChar) && startIndx > curTxt.Find("-")) || 
      (m_numberType == ddUnsignedFloat && nChar == '.' && curTxt.Find(".") < 0) ||
      (m_numberType == ddFloat && ((nChar == '.' && curTxt.Find(".") < 0) || (nChar == '-' && startIndx == 0 && curTxt.Find("-") < 0))) ||
      nChar == VK_BACK || nChar == 0x7F /*Delete*/)
   {
      CEdit::OnChar(nChar, nRepCnt, nFlags);
   }
   else
   {
      MessageBeep((UINT)-1);
   }


   return;
}

BEGIN_MESSAGE_MAP(CEditNumber, CEdit)
  //{{AFX_MSG_MAP(CEditNumber)
       ON_WM_CHAR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
