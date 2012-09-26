// $Header: /CAMCAD/4.6/DataDoctorPageSubclasses.cpp 13    5/03/07 5:44p Rick Faltersack $

#include "stdafx.h"

#include "CCEtoODB.h"

//#include "edit.h" // for point editor, for bulge
//#include "PcbUtil.h" // for GetPinExtents
//#include "GenerateCentroidAndOutline.h"  // for #define ALG_NAME_UNKNOWN, want to use same as Access Analysis

#include "ODBC_Lib.h"

#include "DataDoctorDialog.h"
#include "DataDoctorPageSubclasses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TEMPLATE_RECTANGULAR  "Rectangular"
#define TEMPLATE_CIRCULAR     "Circular"
#define TEMPLATE_QFP          "QFP"
#define TEMPLATE_SO           "SO"
#define TEMPLATE_SOT23        "SOT23"

extern void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block); // someday maybe someone will put this in some .h file

static CDataDoctorRealPart *HACKRP = NULL;

IMPLEMENT_DYNAMIC(CDataDoctorSubclassesPage, CDataDoctorPropertyPageType)

CDataDoctorSubclassesPage::CDataDoctorSubclassesPage(CDataDoctorPropertySheet& parent)
	: CDataDoctorPropertyPageType(CDataDoctorSubclassesPage::IDD), m_parent(parent)
     , m_subclassesGrid(this)
     , m_elementsGrid(this)
     , m_overwriteValues(1)
	  , m_storeOption(0)  // Case 2058, init to "save not in database"  ///!!!!*rcf Case 2058 has this as 1, TEMPORARILY set to 0 for outline fussing
     , m_columnDefMap_SubclassesGrid("DD Subclasses Grid")
     , m_columnDefMap_ElementsGrid("DD Subclass Elements Grid")
     , m_updatePending(false)
     , m_dialogIsInitialized(false)
{
   AddFieldControls();

   DefineGridColumnMaps();

   // Check boxes and such
   LoadStickySettings();

   m_doc = &(getCamCadDatabase().getCamCadDoc());
	m_activeFile = m_doc->getFileList().GetOnlyShown(blockTypePcb);

   CDataDoctorRealPart::DefineLayers(this->getCamCadDatabase());
}

CDataDoctorSubclassesPage::~CDataDoctorSubclassesPage()
{
}

/******************************************************************************
*/
   CSubclassList &CDataDoctorSubclassesPage::getSubclassList()       { return getParent().getDataDoctor().getSubclassList(); }
   //CSubclass *CDataDoctorSubclassesPage::findSubclass(CString name)  { return this->getSubclassList().findSubclass(name); }

/******************************************************************************
*/
void CDataDoctorSubclassesPage::DefineGridColumnMaps()
{
   // Set up the column definitions.
   // Default column order left to right is order they get defined here.

   m_columnDefMap_SubclassesGrid.empty();
   m_columnDefMap_SubclassesGrid.AddColDef( QSubclass,  &m_colSubclass_SubclassesGrid );
   m_columnDefMap_SubclassesGrid.AddColDef( QElements,  &m_colElements_SubclassesGrid );
   //m_columnDefMap_GeomGrid.AddColDef( QMessage,       &m_colMessage_SubtypesGrid );

   // Load possible user overrides (column swaps)
   m_columnDefMap_SubclassesGrid.LoadSettings();


   m_columnDefMap_ElementsGrid.empty();
   m_columnDefMap_ElementsGrid.AddColDef( QName,         &m_colElementName_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinFunction,  &m_colPin1Function_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinName,      &m_colPin1Name_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinFunction,  &m_colPin2Function_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinName,      &m_colPin2Name_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinFunction,  &m_colPin3Function_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinName,      &m_colPin3Name_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinFunction,  &m_colPin4Function_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPinName,      &m_colPin4Name_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QValue,        &m_colValue_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QPTolerance,   &m_colPlusTol_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QNTolerance,   &m_colMinusTol_ElementsGrid );
   m_columnDefMap_ElementsGrid.AddColDef( QDeviceType,   &m_colDevType_ElementsGrid );

   // Load possible user overrides (column swaps)
   m_columnDefMap_ElementsGrid.LoadSettings();
}


/******************************************************************************
*/

void CDataDoctorSubclassesPage::DoDataExchange(CDataExchange* pDX)
{
	CDataDoctorPropertyPageType::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_EDIT_NEW_SUBCLASS    , m_newSubclassEditBox);
   DDX_Control(pDX, IDC_BTN_ADD_SUBCLASS     , m_addSubclassButton);

	DDX_Check(pDX, IDC_OverwriteValuesCheckBox, m_overwriteValues);
	DDX_Radio(pDX, IDC_StoreAllOption, m_storeOption);
}

/******************************************************************************
*/

BEGIN_MESSAGE_MAP(CDataDoctorSubclassesPage, CDataDoctorPropertyPageType)
   ON_BN_CLICKED(IDC_LoadButton, OnBnClickedLoadFromLibrary)
   ON_BN_CLICKED(IDC_StoreButton, OnBnClickedStoreToLibrary)

   ON_COMMAND(ID_DataDoctorCommand_LoadSubclass, OnLoadSubclass)
   ON_COMMAND(ID_DataDoctorCommand_SaveSubclass, OnSaveSubclass)

   ON_EN_CHANGE(IDC_EDIT_NEW_SUBCLASS, OnNewSubclassNameChanged)
   ON_BN_CLICKED(IDC_BTN_ADD_SUBCLASS, OnBnClickedAddSubclass)

   ON_COMMAND(ID_DataDoctorCommand_SubclassCopy,  OnBnClickedSubclassCopy)
   ON_COMMAND(ID_DataDoctorCommand_SubclassDelete, OnBnClickedSubclassDelete)
   //ON_COMMAND(ID_DataDoctorCommand_SubclassPaste, OnBnClickedSubclassPaste)

   ON_MESSAGE(ID_DATADOCTOR_SUBCLASSGRIDROWEDITFINISHED, OnSubclassGridRowEditFinished)
   ON_MESSAGE(ID_DATADOCTOR_ELEMENTGRIDROWEDITFINISHED, OnElementGridRowEditFinished)

	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/******************************************************************************
*/

CCamCadDatabase& CDataDoctorSubclassesPage::getCamCadDatabase() 
{ 
   return getParent().getCamCadDatabase(); 
}

/******************************************************************************
*/

CDataDoctorPackages& CDataDoctorSubclassesPage::getPackages()
{ 
   return this->getParent().getDataDoctor().getPackages(); 
}

/******************************************************************************
*/

BOOL CDataDoctorSubclassesPage::OnInitDialog()
{
   CDataDoctorPropertyPageType::OnInitDialog();

   m_subclassesGrid.AttachGrid(this, IDC_SubclassesGridStatic);
   m_elementsGrid.AttachGrid(this, IDC_ElementsGridStatic);

   //m_pkgPartGrid.ShowWindow(SW_HIDE);

   this->m_subclassesGrid.SetBackgroundColors(m_subclassesGrid.m_greenBackgroundColor, m_subclassesGrid.m_redBackgroundColor, m_subclassesGrid.m_lightGrayBackgroundColor);
   this->m_elementsGrid.SetBackgroundColors(m_elementsGrid.m_greenBackgroundColor, m_elementsGrid.m_redBackgroundColor, m_elementsGrid.m_lightGrayBackgroundColor);

   FillGrids();
   EnableGridView();
   getParent().getDataDoctor().getPackages().PurgeOldStyleDFTPackageOutlines();

   getParent().EnableDisableDatabaseControls();

   enableAddSubclassButton();

   m_dialogIsInitialized = true;

   return TRUE;  // return TRUE unless you set the focus to a control
}

//***************************************************************************************

void CDataDoctorSubclassesPage::SaveStickySettings()
{
   // Save setting in registry, return true if is appears to work, otherwise false
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      /*
      settings.setValue("Grid View", this->GetGridMode() == PkgGridViewGeometries ? QGeometry : QPartNumber);
      settings.setValue("Fill Outlines", this->GetOutlineFillMode() ? "Yes" : "No");
      */
   }

   //return false;   
}

//***************************************************************************************

void CDataDoctorSubclassesPage::LoadStickySettings()
{
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      CString val;
      /* saved for settings code template .........
      if (settings.getValue("Grid View", val))
      {
         if (val.CompareNoCase(QGeometry) == 0)
            m_viewModePartNum = 0;
         else
            m_viewModePartNum = 1;
      }

      if (settings.getValue("Fill Outlines", val))
      {
         if (val.CompareNoCase("No") == 0)
            m_outlineFill = 0;
         else
            m_outlineFill = 1;
      }
      ................ */
   }

}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::EnableGridView()
{

}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::AddFieldControls()
{
#if defined (DataDoctorResizingDialogFlag)

   CResizingDialogField &f1 = addFieldControl(IDC_SubclassesGridStatic   ,anchorLeft, growProportionalHorizontal);
   f1.getOperations().addOperation(growVertical);

   CResizingDialogField &f2 = addFieldControl(IDC_ElementsGridStatic     ,anchorRight, growVertical);
   f2.getOperations().addOperation(glueLeftEdge, toRightEdge, &f1);

   CResizingDialogField &f3 = addFieldControl(IDC_LBL_SUBCLASS,     anchorTop, growStatic);
   f3.getOperations().addOperation(glueLeftEdge, toRightEdge, &f1);

   ///CResizingDialogField &f4 = addFieldControl(IDC_LBL_CUR_SUBCLASS, anchorTop, growStatic);
   ///f4.getOperations().addOperation(glueLeftEdge, toRightEdge, &f3);


   addFieldControl(IDC_LBL_NEW_SUBCLASS              ,anchorBottomLeft);
   addFieldControl(IDC_EDIT_NEW_SUBCLASS             ,anchorBottomLeft);
   addFieldControl(IDC_BTN_ADD_SUBCLASS              ,anchorBottomLeft);

   addFieldControl(IDC_LoadButton             ,anchorBottomLeft);
   addFieldControl(IDC_OverwriteValuesCheckBox,anchorBottomLeft);

   addFieldControl(IDC_StoreButton            ,anchorBottomLeft);
   addFieldControl(IDC_StoreAllOption         ,anchorBottomLeft);
   addFieldControl(IDC_StoreNotInLibraryOption,anchorBottomLeft);

   addFieldControl(IDC_ActiveLibraryGroup     ,anchorBottomLeft, growHorizontal);


#endif
}


/******************************************************************************
*/
void CDataDoctorSubclassesPage::EnableDatabaseControls(bool flag)
{
   // Just need to check the first one.
   // If dialog is not init'd then none exist, if is init'd then all exist
   if (DialogIsInitialized())
   {
      GetDlgItem(IDC_LoadButton)->EnableWindow( flag );
      GetDlgItem(IDC_OverwriteValuesCheckBox)->EnableWindow( flag );
      GetDlgItem(IDC_StoreButton)->EnableWindow( flag );
      GetDlgItem(IDC_StoreAllOption)->EnableWindow( flag );
      GetDlgItem(IDC_StoreNotInLibraryOption)->EnableWindow( flag );
      GetDlgItem(IDC_ActiveLibraryGroup)->EnableWindow( flag );
   }
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnBnClickedAddSubclass()
{
   UpdateData(true);

   // User chould have altered a name in the list to match the last one
   // that was checked in the New Name box, so re-check.

   CString name;
   m_newSubclassEditBox.GetWindowText(name);

   if (name.IsEmpty())
   {
      // Do nothing
   }
   else if (getSubclassList().findSubclass(name) != NULL)
   {
      CString msg;
      msg.Format("Sublcass %s is already in the list, must enter a new unique name.", name);
      ErrorMessage(msg, "");
   }
   else
   {
      // Okay, add it
      CSubclass *sc = new CSubclass();
      sc->setName(name);
      sc->setNumElements(1);
      getSubclassList().Add(sc);
      this->m_subclassesGrid.Fill(getSubclassList());
   }

   enableAddSubclassButton();
}
/******************************************************************************
*/

void CDataDoctorSubclassesPage::enableAddSubclassButton()
{
   // Add button is enabled is name is not blank and name does not already exist in list

   UpdateData(true);

   CString name;
   m_newSubclassEditBox.GetWindowText(name);

   m_addSubclassButton.EnableWindow( !name.IsEmpty() && getSubclassList().findSubclass(name) == NULL );
}

/******************************************************************************
*/
void CDataDoctorSubclassesPage::OnNewSubclassNameChanged()
{
   enableAddSubclassButton();
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnBnClickedLoadFromLibrary()
{
   // Button operation on whole subclass table

   UpdateData(true);

   int loadedCount = getSubclassList().LoadFromDB();

   Update();
   getParent().updatePropertyPages(this);

   if (loadedCount < 1)
      formatMessageBox(MB_ICONASTERISK,"Load All Subclasses from Library did not find any Subclasses");

}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnBnClickedStoreToLibrary()
{
   // Button operation on whole subclass table

   //*rcf This functionality should be made into a func over in Element.cpp/h

   UpdateData(true);

   CDBInterface db;
   if (db.Connect())
   {
      bool overwriteExisting = (m_storeOption == 0);

      CDBSubclassElementList *existingSubclassList = NULL;
      if (!overwriteExisting)
      {
         existingSubclassList = db.LookupSubclassElements(""); // Get all
      }

      for (int i = 0; i < getSubclassList().GetCount(); i++)
      {
         CSubclass *sc = getSubclassList().GetAt(i);

         if (sc != NULL)
         {
            CString subclassName( sc->getName() );

            if (overwriteExisting || !existingSubclassList->containsSubclass(subclassName))
            {
               CDBSubclassElementList dbelementList;

               for (int j = 0; j < sc->getNumElements(); j++)
               {
                  CSubclassElement *el = sc->getElementAt(j);

                  if (el != NULL)
                  {
                     CDBSubclassElement *dbel = new CDBSubclassElement(subclassName,
                        el->getName(),
                        deviceTypeTagToFriendlyString(el->getDeviceType()),
                        el->getPin1Name(), el->getPin2Name(), el->getPin3Name(), el->getPin4Name(),
                        el->getValue(), el->getPlusTol(), el->getMinusTol());

                     dbelementList.Add(dbel);
                  }
               }

               db.SaveSubclass(subclassName, &dbelementList);
            }
         }
      }

      if (existingSubclassList != NULL)
         delete existingSubclassList;
   }
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnLoadSubclass()
{
   // Single subclass row operation (right click menu)
   // Load one package from DB, for selected row

   int matchCount = 0;

   UpdateData(true);
   
   int row = m_subclassesGrid.GetCurrentRow(); // Currently selected row

   if (row >= 0 && row < m_subclassesGrid.GetNumberRows())
   {
      CSubclass *sc = (CSubclass*)m_subclassesGrid.GetRowData(row);
      
      if (sc != NULL)
      {
         if (sc->LoadFromDB())
         {
            matchCount = 1;
            Update();
         }
      }
   }

   if (matchCount == 0)
   {
      ErrorMessage("No matching entries found in library.", "", MB_OK);
   }
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnSaveSubclass()
{
   // Single subclass row operation (right click menu)

   int row = m_subclassesGrid.GetCurrentRow(); // Currently selected row

   if (row >= 0 && row < m_subclassesGrid.GetNumberRows())
   {
      CSubclass *sc = (CSubclass*)m_subclassesGrid.GetRowData(row);
      
      if (sc != NULL)
      {
         CDBInterface db;
         if (db.Connect())
         {
            CDBSubclassElementList dblist;

            CDBSubclassElementList dbelementList;
            CString subclassName( sc->getName() );

            for (int j = 0; j < sc->getNumElements(); j++)
            {
               CSubclassElement *el = sc->getElementAt(j);

               if (el != NULL)
               {
                  CDBSubclassElement *dbel = new CDBSubclassElement(subclassName,
                     el->getName(),
                     deviceTypeTagToFriendlyString(el->getDeviceType()),
                     el->getPin1Name(), el->getPin2Name(), el->getPin3Name(), el->getPin4Name(),
                     el->getValue(), el->getPlusTol(), el->getMinusTol());

                  dbelementList.Add(dbel);
               }
            }

            db.SaveSubclass(subclassName, &dbelementList);

         }
      }
   }
}

/******************************************************************************
*/

CDataDoctorComponent* CDataDoctorSubclassesPage::GetActiveSampleComponent()
{
   CDataDoctorComponent* comp = NULL;



   return comp;
}

/******************************************************************************
*/

CDataDoctorRealPart* CDataDoctorSubclassesPage::GetActiveRealPart()
{
   CDataDoctorRealPart* realpart = NULL;



   return realpart;
}

/******************************************************************************
*/



/******************************************************************************
*/
double CDataDoctorSubclassesPage::GetDouble(int dlgItem)
{
   CString str;
   GetDlgItem(dlgItem)->GetWindowText(str);
   double d = atof(str);
   return d;
}

/******************************************************************************
*/


/******************************************************************************
*/

LRESULT CDataDoctorSubclassesPage::OnSubclassGridRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   int row = wParamROW;
   int col = lParamCOL;


   if (row >= 0 && row < this->m_subclassesGrid.GetNumberRows())
   {
      CString val;
      m_subclassesGrid.QuickGetText(col, row, &val);
      CSubclass *sc = (CSubclass*)m_subclassesGrid.GetRowData(row);

      if (sc != NULL)
      {
         if (col == m_colSubclass_SubclassesGrid)
         {
            sc->setName(val);
            m_subclassesGrid.ResizeAll();
         }
         else if (m_colElements_SubclassesGrid)
         {
            int n = atoi(val);
            sc->setNumElements(n);
            // Note that we do not alter the element list even if num elements was made smaller.
            // The numElements setting controls what is displayed, but the element list is allowed to have excess.
            // This allows user to reduce the count (oops didn't mean to) and increase the count again and they are still there.
         }

         DisplayElements(sc);
      }
   }


   return 0;
}

CString CDataDoctorSubclassesPage::getNormalizedTolerance(CString randomTolerance)
{
   ComponentValues val;
   val.SetValue(randomTolerance);
   val.SetUnits(valueUnitPercent);

   return val.GetPrintableString();
}

/******************************************************************************
*/

LRESULT CDataDoctorSubclassesPage::OnElementGridRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   int row = wParamROW;
   int col = lParamCOL;


   if (row >= 0 && row < this->m_elementsGrid.GetNumberRows())
   {
      CString val;
      m_elementsGrid.QuickGetText(col, row, &val);
      CSubclassElement *el = (CSubclassElement*)m_elementsGrid.GetRowData(row);

      if (el != NULL)
      {
         if (col == m_colElementName_ElementsGrid)
         {
            el->setName(val);   
         }
         else if (col == m_colPin1Name_ElementsGrid)
         {
            el->setPin1Name(val);
         }
         else if (col == m_colPin2Name_ElementsGrid)
         {
            el->setPin2Name(val);
         }
         else if (col == m_colPin3Name_ElementsGrid)
         {
            el->setPin3Name(val);
         }
         else if (col == m_colPin4Name_ElementsGrid)
         {
            el->setPin4Name(val);
         }
         else if (col == m_colValue_ElementsGrid)
         {
            CString normalizedVal( val );
            ValueUnitTag devTypeUnits = getDefaultValueUnitForDeviceType( el->getDeviceType() );
            ComponentValues componentValue(val, devTypeUnits);
            componentValue.SetUnits(devTypeUnits); // To force a conversion
            if (componentValue.IsValid())
            {
               normalizedVal = componentValue.GetPrintableString();
            }

            el->setValue(normalizedVal);
            m_elementsGrid.QuickSetText(col, row, normalizedVal);
            m_elementsGrid.RedrawCell(col, row);
         }
         else if (col == m_colPlusTol_ElementsGrid)
         {
            CString normalizedVal( this->getNormalizedTolerance(val) );
            el->setPlusTol(normalizedVal);
            m_elementsGrid.QuickSetText(col, row, normalizedVal);
            m_elementsGrid.RedrawCell(col, row);
         }
         else if (col == m_colMinusTol_ElementsGrid)
         {
            CString normalizedVal( this->getNormalizedTolerance(val) );
            el->setMinusTol(normalizedVal);
            m_elementsGrid.QuickSetText(col, row, normalizedVal);
            m_elementsGrid.RedrawCell(col, row);
         }
         else if (col == m_colDevType_ElementsGrid)
         {
            //  Menu handler already updated all
         }  
      }

      m_elementsGrid.ResizeAll();
   }

   return 0;
}


/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnBnClickedSubclassCopy()
{
   UpdateData(true);

   int row = this->m_subclassesGrid.GetCurrentRow();

   if (row >= 0 && row < m_subclassesGrid.GetNumberRows())
   {
      CSubclass *sourceSc = (CSubclass*)m_subclassesGrid.GetRowData(row);

      if (sourceSc != NULL)
      {
         CSubclass *destSc = new CSubclass(*sourceSc);
         
         CString destName;
         destName.Format("CopyOf%s", sourceSc->getName());
         int copyNum = 1;
         while (getSubclassList().findSubclass(destName) != NULL)
         {
            destName.Format("Copy%dOf%s", ++copyNum, sourceSc->getName());
         }
         destSc->setName(destName);

         getSubclassList().InsertAt(row+1, destSc);
         this->m_subclassesGrid.Fill(getSubclassList());
      }
   }
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnBnClickedSubclassDelete()
{
   // Inserting before this vertex is same as inserting after previous vertex

   UpdateData(true);

   int row = this->m_subclassesGrid.GetCurrentRow();

   if (row >= 0 && row < m_subclassesGrid.GetNumberRows())
   {
      CSubclass *victimSc = (CSubclass*)m_subclassesGrid.GetRowData(row);

      if (victimSc != NULL)
      {
         DeleteSubclassConfirmDlg confirmDlg(victimSc->getName());

         int rspns = IDOK;
         if (!confirmDlg.GetJustDoIt())   // If JustDoIt is on then user previously turned off confirmation
            rspns = confirmDlg.DoModal();

         if (rspns == IDOK)
         {
            getSubclassList().deleteSubclass(victimSc);  // DANGER !!! victimSc ptr is BAD after this call

            this->m_subclassesGrid.Fill(getSubclassList());
            this->DisplayElements(NULL);
         }
      }
   }
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnBnClickedSubclassPaste()
{
   UpdateData(true);

   ErrorMessage("Not yet implemented", "");
   /*
   CDataDoctorRealPart *realpart = GetActiveRealPart();

   if (realpart != NULL)
   {
      long row = m_subclassesGrid.GetCurrentRow();

      if (row >= 0 && row < m_subclassesGrid.GetNumberRows())
      {
         //*rcf BUG is first or last pnt then need to fix closed poly, changes two vertices not just one

         if (row == 0 || row == (m_subclassesGrid.GetNumberRows() - 1))
         {
            // Special case for first/last point, maintain closed poly.
            // There has to be at least two other points, so we end up with three pt closed poly.
            CPoly *poly = m_subclassesGrid.GetActivePoly();
            if (poly != NULL && poly->getPntList().GetCount() > 3)
            {
               // Don't need grid, just remove first and last pts
               delete poly->getPntList().GetHead();
               poly->getPntList().RemoveHead();

               delete poly->getPntList().GetTail();
               poly->getPntList().RemoveTail();

               // Now make it closed again, dup new first pt to tail
               CPnt *firstpnt = poly->getPntList().GetHead();
               CPnt *newpnt = new CPnt(*firstpnt);
               poly->getPntList().AddTail(newpnt);
            }
         }
         else
         {
            // Some regular embedded point
            CPnt *selectedpnt = (CPnt*)m_subclassesGrid.GetRowData(row);
            CPoly *poly = m_subclassesGrid.GetActivePoly();

            if (poly != NULL && selectedpnt != NULL)
            {
               POSITION pos = poly->getPntList().Find(selectedpnt);
               if (pos != NULL)
               {
                  delete poly->getPntList().GetAt(pos);
                  poly->getPntList().RemoveAt(pos);
               }
            }
         }

      }
   }
   */
}



/******************************************************************************
*/

BOOL CDataDoctorSubclassesPage::OnSetActive()
{
   this->getParent().SaveAsInitialPage(DdSubclassesPage);

   if (IsUpdatePending())
      Update();

   return CDataDoctorPropertyPageType::OnSetActive();

}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::Update()
{
   // Update display

   FillGrids();
   m_updatePending = false;
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::DisplayElements(CSubclass *sc)
{
   this->m_elementsGrid.Fill(sc);

   CString label;
   label.Format("Elements for Subclass: %s", sc != NULL ? sc->getName() : "<No Subclass Selected>");

   GetDlgItem(IDC_LBL_SUBCLASS)->SetWindowText(label);
}


/******************************************************************************
*/

void CDataDoctorSubclassesPage::FillGrids()
{
   CWaitCursor pardonMeThisMightTakeAMoment;

   this->m_subclassesGrid.Fill(getSubclassList());

   if (getSubclassList().GetCount() > 0)
   {
      CSubclass *sc = getSubclassList().GetAt(0);
      DisplayElements(sc);
   }
   else
   {
      DisplayElements(NULL);
   }
}

/******************************************************************************
*/

void CDataDoctorSubclassesPage::OnCancel()
{
   getCamCadDatabase().getCamCadDoc().OnDoneEditing();

   CDataDoctorPropertyPageType::OnCancel();
}

/******************************************************************************
*/

BOOL CDataDoctorSubclassesPage::OnKillActive()
{
   getCamCadDatabase().getCamCadDoc().OnDoneEditing();

   return CDataDoctorPropertyPageType::OnKillActive();
}


/******************************************************************************
*/

bool CDataDoctorSubclassesPage::isSupportedElementDevType(DeviceTypeTag devtype)
{
   // This is the whole enum just pasted in.
   // The ones that are not supported are commented out.
   // These are supported ELEMENT device types, not overall subclass dev types.
   // E.g. as a subclass Capacitor_Array is supported, but not as an element dev type.

   switch (devtype)
   {
   //case deviceTypeUnknown:
   //case deviceTypeBattery:
   case deviceTypeCapacitor:
   //case deviceTypeCapacitorArray:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
   //case deviceTypeConnector:
   //case deviceTypeCrystal:
   case deviceTypeDiode:
   //case deviceTypeDiodeArray:
   case deviceTypeDiodeLed:
   //case deviceTypeDiodeLedArray:
   case deviceTypeDiodeZener:
   case deviceTypeFilter:
   case deviceTypeFuse:
   //case deviceTypeIC:
   //case deviceTypeICDigital:
   //case deviceTypeICLinear:
   case deviceTypeInductor:
   case deviceTypeJumper:
   //case deviceTypeNoTest:
   //case deviceTypeOscillator:
   //case deviceTypePotentiometer:
   //case deviceTypePowerSupply:
   //case deviceTypeRelay:
   case deviceTypeResistor:
   //case deviceTypeResistorArray:
   //case deviceTypeSpeaker:
   case deviceTypeSwitch:
   //case deviceTypeTestPoint:
   case deviceTypeTransformer:
   case deviceTypeTransistor:
   //case deviceTypeTransistorArray:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
   case deviceTypeTransistorScr:
   case deviceTypeTransistorTriac:
   //case deviceTypeVoltageRegulator:
   case deviceTypeOpto:
   case deviceTypeUndefined:
      return true;
   }

   return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Delete Subclass Confirmation Dialog

DeleteSubclassConfirmDlg::DeleteSubclassConfirmDlg(CString subclassName, CWnd* pParent /*=NULL*/)
   : CDialog(DeleteSubclassConfirmDlg::IDD, pParent)
   , m_subclassName(subclassName)
   , m_justDoIt(FALSE)
{
   // Load Sticky Setting
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      CString val;

      if (settings.getValue("Just Do It", val))
      {
         if (val.CompareNoCase("Yes") == 0)
            m_justDoIt = TRUE;
         else
            m_justDoIt = FALSE;
      }
   }
}

DeleteSubclassConfirmDlg::~DeleteSubclassConfirmDlg()
{
}

void DeleteSubclassConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Check(pDX, IDC_CHK_JUST_DO_IT, m_justDoIt);
}

BEGIN_MESSAGE_MAP(DeleteSubclassConfirmDlg, CDialog)
   ON_BN_CLICKED(IDC_CHK_REPLACE_EXISTING_PANEL, OnBnClickedJustDoIt)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL DeleteSubclassConfirmDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   CString msg;
   msg.Format("Permanently delete %s from the library?", m_subclassName);
   this->GetDlgItem(IDC_MSG1)->SetWindowText(msg);

   return TRUE;
}

void DeleteSubclassConfirmDlg::OnBnClickedOk()
{
   UpdateData();


   // Save Sticky Setting
   // If item is checked on then next time dialog will not ask for confirmation.

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      settings.setValue("Just Do It", m_justDoIt ? "Yes" : "No");
   }

   CDialog::OnOK();
}

void DeleteSubclassConfirmDlg::OnBnClickedJustDoIt()
{
   UpdateData();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CDDSubclassesGrid::CDDSubclassesGrid(CDataDoctorSubclassesPage *page)
: m_subclassesPage(page)
{
}

//----------------------------------------------------------------------------------


void CDDSubclassesGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	


   if (this->m_subclassesPage != NULL)
   {
      int colcount = this->m_subclassesPage->m_columnDefMap_SubclassesGrid.getSize();

      SetNumberCols(colcount);
      SetNumberRows(0);
      
      for (int indx = 0; indx < m_subclassesPage->m_columnDefMap_SubclassesGrid.getSize(); indx++)
      {
         CColumnDef *cd = m_subclassesPage->m_columnDefMap_SubclassesGrid.getAt(indx);
         CString colname = cd->GetTitle();
         int *colindxptr = cd->GetColIndexPtr();

         QuickSetText(*colindxptr, -1, colname);
      }
   }
   else
   {
      SetNumberCols(0);
      SetNumberCols(0);
   }


   BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   SetColWidth(-1, 0); // get rid of "row heading"


   // Enable Right-click menu
   EnableMenu(TRUE);

}

void CDDSubclassesGrid::AddRow(CSubclass *sc)
{
   if (sc != NULL)
   {
      int rowCnt = this->GetNumberRows();
      this->SetNumberRows(rowCnt+1);
      this->QuickSetText(  this->m_subclassesPage->m_colSubclass_SubclassesGrid, rowCnt, sc->getName());
      this->QuickSetNumber(this->m_subclassesPage->m_colElements_SubclassesGrid, rowCnt, sc->getNumElements());
      this->SetRowData( rowCnt, sc);

      this->SetRowBackColor( rowCnt, this->m_backgroundColorOK );
   }
}


void CDDSubclassesGrid::Fill(CSubclassList &subclassList)
{
   //*rcf This will become a db connection

   this->RemoveAllRows();

   for (int i = 0; i < subclassList.GetCount(); i++)
   {
      CSubclass *sc = subclassList.GetAt(i);
      AddRow(sc);
   }

   this->ResizeAll();
   this->RedrawAll();
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditStart
//		This message is sent whenever the grid is ready to start editing a cell
//	Params:
//		col, row - location of the cell that edit was requested over
//		edit -	pointer to a pointer to the edit control, allows for swap of edit control
//				if edit control is swapped permanently (for the whole grid) is it better
//				to use 'SetNewEditClass' function.
//	Return:
//		TRUE - to allow the edit to start
//		FALSE - to prevent the edit from starting

/// SURPRISE - Return FALSE for cells with Option Menus, else it will go into text edit mode !

int CDDSubclassesGrid::OnEditStart(int col, long row, CWnd **edit)
{
   *edit = &m_myCUGEdit;

   return true;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditVerify
//		This notification is sent every time the user hits a key while in edit mode.
//		It is mostly used to create custom behavior of the edit control, because it is
//		so easy to allow or disallow keys hit.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int CDDSubclassesGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	//UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	//UNREFERENCED_PARAMETER(*vcKey);

   // Only integer numbers allowed in DPMO.
   // Movement keys, backspace, left and right arrows, and Delete key are okay too.
   /*
   if (col == m_packagesPage->m_colDPMO_PartGrid)
   {
      if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE ||
         isdigit(*vcKey))
         return TRUE;
      else
         return FALSE;
   }
   */

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finished
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being canceled
//	Return:
//		TRUE - to allow the edit to proceed
//		FALSE - to force the user back to editing of that same cell
int CDDSubclassesGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   if (!cancelFlag)
   {
      // Do not allow rename to cause a duplicate

      if (col == this->m_subclassesPage->m_colSubclass_SubclassesGrid)
      {
         CSubclass *thisSubclass = (CSubclass*)this->GetRowData(row);
         CSubclass *thatSubclass = this->m_subclassesPage->getSubclassList().findSubclass(string);
         if (thatSubclass != NULL && thisSubclass != thatSubclass)
         {
            CString msg;
            msg.Format("Subclass %s already exists, cannot rename to an existing name.", string);
            ErrorMessage(msg, "");
            return FALSE;
         }
      }

   }

   // If ya want to get the value from the grid then have to post a message here and get
   // value later. Notify is saying editing is finished, but grid itself is not yet
   // updated. Use string param if you want to act on new val now.
  
   BOOL pm = this->m_subclassesPage->PostMessage(ID_DATADOCTOR_SUBCLASSGRIDROWEDITFINISHED, row, col);


	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuCommand
//		This notification is called when the user has selected a menu item
//		in the pop-up menu.
//	Params:
//		col, row - the cell coordinates of where the menu originated from
//		section - identify for which portion of the gird the menu is for.
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//		item - ID of the menu item selected
//	Return:
//		<none>
void CDDSubclassesGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	//UNREFERENCED_PARAMETER(item);

   this->m_subclassesPage->SendMessage(WM_COMMAND, item);

}

void CDDSubclassesGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   /*
   // Some grids update via a PostMessage, because OnEditFinish is called when editing is about
   // to finish,not after editting is finished. So the update message is posted, edit allowed to 
   // finish, then the grid is updated. This LClicked event might trigger OnEditFinish, but then
   // the update that is posted comes after this LClick we have in hand. If multi-select is active
   // then the multi-select params get wiped out, and the grid update fails to update the whole
   // selection. So flush the queue before continuing with the LClick event.
   FlushEvents();

   CaptureSelection();
   */
   CDDBaseGrid::OnLClicked(col, row, updn, rect, point, processed); 

   if (updn == TRUE)  // button down
   {
      CSubclass *sc = (CSubclass*)this->GetRowData(row);
      this->m_subclassesPage->DisplayElements(sc);

      //*rcf Experimental
      ///this->ClearSelections();
      ///SetMultiSelectMode(FALSE); // this is the default, but seems to be getting turned on somehow, not on purpose by our code
      ///this->Select(col, row);
   }

   if (updn == FALSE) // button up
   {
   }


}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuStart
//		Is called when the pop up menu is about to be shown
//	Params:
//		col, row	- the cell coordinates of where the menu originated from
//		setcion		- identify for which portion of the gird the menu is for.
//					possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//	Return:
//		TRUE - to allow menu to show
//		FALSE - to prevent the menu from poping up
int CDDSubclassesGrid::OnMenuStart(int col,long row,int section)
{
   // If R click is not within grid-proper, then cancel the menu.
   // e.g. no menu for top heading or scroll bars
   if (section != UG_GRID)
      return FALSE;

   // Might be in the grid section but not in a grid cell.
   // Reject these too.
   if (row < 0 || col < 0)
      return FALSE;

   
   // Proceed with menu

   // The popup choices are connected to commands, the commands
   // will operate on the currently selected row, so set the selection.
   ///this->ClearSelections();
   ///SetMultiSelectMode(FALSE); // this is the default, but seems to be getting turned on somehow, not on purpose by our code
   ///this->Select(col, row);
   this->RedrawAll(); // maybe for speed should get old selection and just redraw that row

   EmptyMenu();

   CString name = QuickGetText(this->m_subclassesPage->m_colSubclass_SubclassesGrid, row);
   CString option;

   this->AddMenuItem(ID_DataDoctorCommand_SubclassCopy,   "Make Copy"); // just does it, not usual copy/paste

   option.Format("Delete %s from Library", name);
   this->AddMenuItem(ID_DataDoctorCommand_SubclassDelete, option);

   option.Format("Load %s from Library", name);
   this->AddMenuItem(ID_DataDoctorCommand_LoadSubclass, option);

   option.Format("Save %s to Library", name);
   this->AddMenuItem(ID_DataDoctorCommand_SaveSubclass, option);


	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CDDElementsGrid::CDDElementsGrid(CDataDoctorSubclassesPage *page)
: m_subclassesPage(page)
{
}

//----------------------------------------------------------------------------------


void CDDElementsGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	


   if (this->m_subclassesPage != NULL)
   {
      int colcount = this->m_subclassesPage->m_columnDefMap_ElementsGrid.getSize();

      SetNumberCols(colcount);
      SetNumberRows(0);
      
      for (int indx = 0; indx < m_subclassesPage->m_columnDefMap_ElementsGrid.getSize(); indx++)
      {
         CColumnDef *cd = m_subclassesPage->m_columnDefMap_ElementsGrid.getAt(indx);
         CString colname = cd->GetTitle();
         int *colindxptr = cd->GetColIndexPtr();

         QuickSetText(*colindxptr, -1, colname);
      }

      // Device Type option menu
      CUGCell cell;
      GetColDefault(this->m_subclassesPage->m_colDevType_ElementsGrid, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      CString optionlist;

      for (DeviceTypeTag dt = deviceTypeUnknown; dt < deviceTypeUndefined; dt = (DeviceTypeTag)(dt + 1))
      {
         if (this->m_subclassesPage->isSupportedElementDevType(dt))
         {
            optionlist += (deviceTypeTagToFriendlyString(dt) + (CString)"\n");
         }
      }

      cell.SetLabelText( optionlist ); 
      SetColDefault(this->m_subclassesPage->m_colDevType_ElementsGrid, &cell);
   }
   else
   {
      SetNumberCols(0);
      SetNumberCols(0);
   }


   //*rcf Column Swapping disabled 23 Jan 09, this grid has mulitple columns with same
   // title, the columnDefMap.Update() function works only when all column names are unique.
   // Probably should make a CDDBaseGrid function for enable column swap that check on this
   // and refused to enable if grid is not "compatible".
   /////EnableColSwapping(TRUE);

   BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   SetColWidth(-1, 0); // get rid of "row heading"


   // Enable Right-click menu
#ifdef ELEMENT_GRID_HAS_MENU  
   EnableMenu(TRUE);
#else
   EnableMenu(FALSE);
#endif

}


/////////////////////////////////////////////////////////////////////////////
//	OnColSwapped
//		Called just after column-swap operation was completed.
//	Params:
//		fromCol - where the col originated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		<none>
void CDDElementsGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   this->m_subclassesPage->m_columnDefMap_ElementsGrid.Update(this);

}

void CDDElementsGrid::AddRow(CSubclassElement *el)
{
   if (el != NULL)
   {
      int rowCnt = this->GetNumberRows();
      this->SetNumberRows(rowCnt+1);

      SetRow(rowCnt, el);
   }
}

void CDDElementsGrid::SetRow(int row, CSubclassElement *el)
{
   if (el != NULL && row >= 0 && row < this->GetNumberRows())
   {
      DeviceTypeTag devType = el->getDeviceType();
      CString devTypeStr( deviceTypeTagToFriendlyString(devType) );

      int pinsReq = el->getNumPinsRequired();

      this->QuickSetText(  this->m_subclassesPage->m_colElementName_ElementsGrid,  row, el->getName() );

      this->QuickSetText(  this->m_subclassesPage->m_colPin1Function_ElementsGrid, row, el->getPinFunction(1) );
      this->QuickSetText(  this->m_subclassesPage->m_colPin1Name_ElementsGrid,     row, el->getPin1Name() );

      this->QuickSetText(  this->m_subclassesPage->m_colPin2Function_ElementsGrid, row, el->getPinFunction(2) );
      this->QuickSetText(  this->m_subclassesPage->m_colPin2Name_ElementsGrid,     row, el->getPin2Name() );

      this->QuickSetText(  this->m_subclassesPage->m_colPin3Function_ElementsGrid, row, pinsReq > 2 ? el->getPinFunction(3) : "---" );
      this->QuickSetText(  this->m_subclassesPage->m_colPin3Name_ElementsGrid,     row, pinsReq > 2 ? el->getPin3Name() : "---" );

      this->QuickSetText(  this->m_subclassesPage->m_colPin4Function_ElementsGrid, row, pinsReq > 3 ? el->getPinFunction(4) : "---" );
      this->QuickSetText(  this->m_subclassesPage->m_colPin4Name_ElementsGrid,     row, pinsReq > 3 ? el->getPin4Name() : "---" );

      this->QuickSetText(  this->m_subclassesPage->m_colValue_ElementsGrid, row, el->getValue() );
      this->QuickSetText(  this->m_subclassesPage->m_colPlusTol_ElementsGrid, row, el->getPlusTol() );
      this->QuickSetText(  this->m_subclassesPage->m_colMinusTol_ElementsGrid, row, el->getMinusTol() );

      this->QuickSetText(  this->m_subclassesPage->m_colDevType_ElementsGrid, row, devTypeStr );

      this->SetRowData( row, el );

      this->SetRowBackColor( row, this->m_backgroundColorOK );
   }
}

void CDDElementsGrid::Fill(CSubclass *sc)
{
   //*rcf This will become a db connection ?

   this->RemoveAllRows();

   if (sc != NULL)
   {
      for (int i = 0; i < sc->getNumElements(); i++)
      {
         CSubclassElement *el = sc->getElementAt(i);

         if (el == NULL)
         {
            el = new CSubclassElement();
            sc->setElementAt(i, el);
         }

         if (el != NULL)
            this->AddRow(el);
      }
   }

   this->ResizeAll();
   this->RedrawAll();
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditStart
//		This message is sent whenever the grid is ready to start editing a cell
//	Params:
//		col, row - location of the cell that edit was requested over
//		edit -	pointer to a pointer to the edit control, allows for swap of edit control
//				if edit control is swapped permanently (for the whole grid) is it better
//				to use 'SetNewEditClass' function.
//	Return:
//		TRUE - to allow the edit to start
//		FALSE - to prevent the edit from starting

/// SURPRISE - Return FALSE for cells with Option Menus, else it will go into text edit mode !

int CDDElementsGrid::OnEditStart(int col, long row, CWnd **edit)
{
   *edit = &m_myCUGEdit;

   CSubclassElement *el = (CSubclassElement*)this->GetRowData(row);
   int pinsReq = (el != NULL) ? el->getNumPinsRequired() : 2;

   if (
      col == m_subclassesPage->m_colPin1Function_ElementsGrid ||
      col == m_subclassesPage->m_colPin2Function_ElementsGrid ||
      col == m_subclassesPage->m_colPin3Function_ElementsGrid ||
      col == m_subclassesPage->m_colPin4Function_ElementsGrid
      )
   {
      return false;  // Can't edit these
   }

   // Can't edit 3rd pin name if only 2 pins required
   if (col == m_subclassesPage->m_colPin3Name_ElementsGrid && pinsReq < 3)
   {
      return false;
   }

   // Can't edit 4th pin name if only 2 or 3 pins required
   if (col == m_subclassesPage->m_colPin4Name_ElementsGrid && pinsReq < 4)
   {
      return false;
   }

   // Can edit the rest

   return true;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditVerify
//		This notification is sent every time the user hits a key while in edit mode.
//		It is mostly used to create custom behavior of the edit control, because it is
//		so easy to allow or disallow keys hit.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int CDDElementsGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
   if (col == this->m_subclassesPage->m_colPlusTol_ElementsGrid ||
     col == this->m_subclassesPage->m_colMinusTol_ElementsGrid)
   {
      return this->OnEditVerifyPercentCell(col, row, edit, vcKey);
   }

   // For those edit grid cells we're not being so picky about...
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finished
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being canceled
//	Return:
//		TRUE - to allow the edit to proceed
//		FALSE - to force the user back to editing of that same cell
int CDDElementsGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   // If ya want to get the value from the grid then have to post a message here and get
   // value later. Notify is saying editing is finished, but grid itself is not yet
   // updated. Use string param if you want to act on new val now.
  
   BOOL pm = this->m_subclassesPage->PostMessage(ID_DATADOCTOR_ELEMENTGRIDROWEDITFINISHED, row, col);

   if (!cancelFlag)
   {

   }


	return TRUE;
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
int CDDElementsGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	UNREFERENCED_PARAMETER(param);

   if (msg == UGCT_DROPLISTSTART)
   {
      /*
      // The droplist is being opened, zoom to component in row
      this->m_packagesPage->DoEditGeom();

      // Disallow changing outline method is package source is not Part Number
      CDataDoctorPart *ddpart = (CDataDoctorPart*)this->GetRowData(row);
      if (ddpart->GetPackageSource().CompareNoCase(QPartNumber) != 0 &&
         col == this->m_packagesPage->m_colOutlineMethod_PartGrid)
      {
         return FALSE;
      }
      */

   }
   else if (msg == UGCT_DROPLISTPOSTSELECT)
   {
      /*
      // Convert "None" to blank
      CString celltext;
      this->QuickGetText(col, row, &celltext);
      if (celltext.CompareNoCase(QNone) == 0)
      {
         celltext.Empty();
         this->QuickSetText(col, row, celltext);
      }

      BOOL pm = this->m_packagesPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);
      */
      CSubclassElement *el = (CSubclassElement*)this->GetRowData(row);
      if (el != NULL)
      {
         CString celltext;
         this->QuickGetText(col, row, &celltext);
         DeviceTypeTag devtype = stringToDeviceTypeTag(celltext);
         el->setDeviceType(devtype);

         // Update value to match new dev type
         CString normalizedStringValue( el->getValue() );
         ValueUnitTag devTypeUnits = getDefaultValueUnitForDeviceType( el->getDeviceType() );
         ComponentValues componentValue(el->getValue(), devTypeUnits);
         componentValue.SetUnits(devTypeUnits); // To force a conversion
         if (componentValue.IsValid())
         {
            normalizedStringValue = componentValue.GetPrintableString();
         }
         el->setValue(normalizedStringValue);

         this->SetRow(row, el);
         this->ResizeAll();
         this->RedrawRow(row);
      }

   }
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuCommand
//		This notification is called when the user has selected a menu item
//		in the pop-up menu.
//	Params:
//		col, row - the cell coordinates of where the menu originated from
//		section - identify for which portion of the gird the menu is for.
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//		item - ID of the menu item selected
//	Return:
//		<none>
void CDDElementsGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	//UNREFERENCED_PARAMETER(item);

   this->m_subclassesPage->SendMessage(WM_COMMAND, item);

}

void CDDElementsGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   /*
   // Some grids update via a PostMessage, because OnEditFinish is called when editing is about
   // to finish,not after editting is finished. So the update message is posted, edit allowed to 
   // finish, then the grid is updated. This LClicked event might trigger OnEditFinish, but then
   // the update that is posted comes after this LClick we have in hand. If multi-select is active
   // then the multi-select params get wiped out, and the grid update fails to update the whole
   // selection. So flush the queue before continuing with the LClick event.
   FlushEvents();

   CaptureSelection();
   */
   CDDBaseGrid::OnLClicked(col, row, updn, rect, point, processed); 

#ifdef BUTTON_ACTIONS
   if (updn == TRUE)  // button down
   {
   }

   if (updn == FALSE) // button up
   {
   }
#endif

}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuStart
//		Is called when the pop up menu is about to be shown
//	Params:
//		col, row	- the cell coordinates of where the menu originated from
//		setcion		- identify for which portion of the gird the menu is for.
//					possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//	Return:
//		TRUE - to allow menu to show
//		FALSE - to prevent the menu from poping up
int CDDElementsGrid::OnMenuStart(int col,long row,int section)
{
   // If R click is not within grid-proper, then cancel the menu.
   // e.g. no menu for top heading or scroll bars
   if (section != UG_GRID)
      return FALSE;

   // Might be in the grid section but not in a grid cell.
   // Reject these too.
   if (row < 0 || col < 0)
      return FALSE;

#ifdef ELEMENT_GRID_HAS_MENU  
   // Proceed with menu

   // The popup choices are connected to commands, the commands
   // will operate on the currently selected row, so set the selection.
   this->ClearSelections();
   SetMultiSelectMode(FALSE); // this is the default, but seems to be getting turned on somehow, not on purpose by our code
   this->Select(col, row);
   this->RedrawAll(); // maybe for speed should get old selection and just redraw that row

   EmptyMenu();

   this->AddMenuItem(ID_DataDoctorCommand_SubclassCopy,   "Copy");
   this->AddMenuItem(ID_DataDoctorCommand_SubclassDelete, "Delete");
   this->AddMenuItem(ID_DataDoctorCommand_SubclassPaste,  "Paste");
#endif

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

