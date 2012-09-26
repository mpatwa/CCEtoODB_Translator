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
#include "DataDoctorPageCentroids.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static short UNIT = 1;
static short DEFAULT_ALGORITHM = 0;
static CMapPtrToPtr map; // map used to check if rotation offset is already done

extern void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);


/////////////////////////////////////////////////////////////////////////////
//_____________________________________________________________________________

IMPLEMENT_DYNAMIC(CDataDoctorCentroidsPage, CDataDoctorPropertyPageType)

CDataDoctorCentroidsPage::CDataDoctorCentroidsPage(CDataDoctorPropertySheet& parent)
	: CDataDoctorPropertyPageType(CDataDoctorCentroidsPage::IDD)
     , m_parent(parent)
     , m_overwriteValues(1)
     , m_storeOption(1)    // case 2058, init to "save not in database" aka "save new"
     , m_radioShowPins(0)
     , m_toolbar(*this)
     , m_grid(this)
     , m_columnDefMap("DD Centroids Grid")
     , m_updatePending(false)
     , m_CentroidCommentKeywordIndex(-1)
     , m_dialogIsInitialized(false)
{

   m_doc = &(getCamCadDatabase().getCamCadDoc());
	m_activeFile = m_doc->getFileList().GetOnlyShown(blockTypePcb);

   m_radioApplyAll = 1;  // Apply to parts with no centroid
   m_chkHideSinglePinComps = FALSE;
   m_chkZeroRotation = FALSE;

   addFieldControls();

   DefineGridColumnMap();

   // Check boxes and such
   LoadStickySettings();
}

CDataDoctorCentroidsPage::~CDataDoctorCentroidsPage()
{
}

CCamCadDatabase& CDataDoctorCentroidsPage::getCamCadDatabase() 
{ 
   return getParent().getCamCadDatabase(); 
}

void CDataDoctorCentroidsPage::DefineGridColumnMap()
{
   // Set up the column definitions.
   // Default column order left to right is order they get defined here.
   m_columnDefMap.empty();

   m_columnDefMap.AddColDef( QStatus, &m_colStatus );
   m_columnDefMap.AddColDef( QGeometryName, &m_colGeomName );
   m_columnDefMap.AddColDef( QPins, &m_colPinCnt );
   m_columnDefMap.AddColDef( QCentroidX, &m_colCentX );
   m_columnDefMap.AddColDef( QCentroidY, &m_colCentY );
   m_columnDefMap.AddColDef( QCentroidRot, &m_colCentR );
   m_columnDefMap.AddColDef( QMethod, &m_colMethod );
   m_columnDefMap.AddColDef( QRefDes,  &m_colRefnames );
   m_columnDefMap.AddColDef( QComments,&m_colComments );
   m_columnDefMap.AddCustomAttributes(m_customColumnMap_CentroidGrid, this->getParent().getDataDoctor().GetPackageLibraryAttribMap());
   m_columnDefMap.AddColDef( QMessage, &m_colMessage );
   // Load possible user overrides
   m_columnDefMap.LoadSettings();

   // Load possible user overrides (column swaps)
   m_columnDefMap.LoadSettings();
}

void CDataDoctorCentroidsPage::addFieldControls()
{
#if defined (DataDoctorResizingDialogFlag)
   addFieldControl(IDC_OCXBOX,anchorLeft,growBoth);

   addFieldControl(IDC_CHK_HIDE_SINGLE_PIN_COMPS   ,anchorTop);
   addFieldControl(IDC_CHK_ZERO_ROTATION           ,anchorTop);

   addFieldControl(IDC_PINS_GROUP                ,anchorTop);
   addFieldControl(IDC_RADIO_PINS_SHOW_ALL       ,anchorTop);
   addFieldControl(IDC_RADIO_PINS_SHOW_NAMED     ,anchorTop);
   addFieldControl(IDC_RADIO_PINS_SHOW_NUMBERED  ,anchorTop);

   addFieldControl(IDC_ClearCentroids            ,anchorTop);

   addFieldControl(IDC_CreateGroup            ,anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_BTN_GENCENTROIDS       ,anchorBottomLeft);
   //addFieldControl(IDC_DefaultAlgLabel        ,anchorBottomLeft); not in CentroidspageToo
   addFieldControl(IDC_COMBO_DEF              ,anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_RADIO_APPLYALL         ,anchorBottomRight);
   addFieldControl(IDC_RADIO_APPLYNONE        ,anchorBottomRight);

   //addFieldControl(IDC_LoadGroup              ,anchorBottomLeft);  not in CentroidsPageToo
   addFieldControl(IDC_LoadButton             ,anchorBottomLeft);
   addFieldControl(IDC_OverwriteValuesCheckBox,anchorBottomLeft);

   //addFieldControl(IDC_StoreGroup             ,anchorBottomLeft);  not in CentroidsPageToo
   addFieldControl(IDC_ActiveLibraryGroup     ,anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_StoreButton            ,anchorBottomLeft);
   addFieldControl(IDC_StoreAllOption         ,anchorBottomLeft);
   addFieldControl(IDC_StoreNotInLibraryOption,anchorBottomLeft);

#endif
}


/******************************************************************************
*/
void CDataDoctorCentroidsPage::EnableDatabaseControls(bool flag)
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
void CDataDoctorCentroidsPage::DoDataExchange(CDataExchange* pDX)
{
   CDataDoctorPropertyPageType::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDataDoctorCentroidsPage)
   //}}AFX_DATA_MAP
   DDX_Control(pDX, IDC_COMBO_DEF, m_cboDefault);
   DDX_Radio(pDX, IDC_RADIO_APPLYALL, m_radioApplyAll);
   DDX_Radio(pDX, IDC_RADIO_PINS_SHOW_ALL, m_radioShowPins);
   DDX_Check(pDX, IDC_CHK_HIDE_SINGLE_PIN_COMPS, m_chkHideSinglePinComps);
   DDX_Check(pDX, IDC_CHK_ZERO_ROTATION, m_chkZeroRotation);
   DDX_Check(pDX, IDC_OverwriteValuesCheckBox, m_overwriteValues);
   DDX_Radio(pDX, IDC_StoreAllOption, m_storeOption);
}

BEGIN_MESSAGE_MAP(CDataDoctorCentroidsPage, CDataDoctorPropertyPageType)
   //{{AFX_MSG_MAP(CDataDoctorCentroidsPage)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
   ON_WM_ACTIVATE()
   ON_MESSAGE(ID_GRIDROWEDITFINISHED, OnRowEditFinished)
   ON_BN_CLICKED(IDC_RADIO_PINS_SHOW_ALL, OnBnClickedPinsShowAll)
   ON_BN_CLICKED(IDC_RADIO_PINS_SHOW_NAMED, OnBnClickedPinsShowNamed)
   ON_BN_CLICKED(IDC_RADIO_PINS_SHOW_NUMBERED, OnBnClickedPinsShowNumbered)
   ON_BN_CLICKED(IDC_BTN_SHOWGEOM, OnBnClickedBtnShowgeom)
   ON_BN_CLICKED(IDC_BTN_GENCENTROIDS, OnBnClickedBtnGenCentroids)

   ON_BN_CLICKED(IDC_LoadButton, OnBnClickedLoadFromLibrary)
   ON_BN_CLICKED(IDC_StoreButton, OnBnClickedStoreToLibrary)

   ON_BN_CLICKED(ID_VL_ZOOM_ZOOMIN,       OnTbBnClickedZoomIn)
   ON_BN_CLICKED(ID_VL_COLORSETS_CUSTOM,  OnTbBnClickedColorsetsCustom)
   ON_BN_CLICKED(ID_LAYERTYPES,           OnTbBnClickedLayerTypes)
   ON_BN_CLICKED(ID_SHOW_FILLS,           OnTbBnClickedShowPolyFill)
   ON_BN_CLICKED(ID_SHOW_AP_FILLS,        OnTbBnClickedShowApertureFill)

   ON_BN_CLICKED(IDC_ClearCentroids, OnBnClickedClearCentroids)

   ON_BN_CLICKED(IDC_CHK_HIDE_SINGLE_PIN_COMPS, OnBnClickedChkHideSinglePinComps)
   ON_BN_CLICKED(IDC_CHK_ZERO_ROTATION,         OnBnClickedChkUseZeroRotation)

   ON_COMMAND(ID_DataDoctorCommand_LoadPart, OnLoadCentroid)
   ON_COMMAND(ID_DataDoctorCommand_SavePart, OnSaveCentroid)  

   // Don't need any particular action taken for these controls when they are
   // interacted with by user. Their values will be used later when something
   // else happens. But focus is lost from grid, so to remove ambiguity of
   // leftover row selection in grid, we clear the selection.
   ON_BN_CLICKED(IDC_OverwriteValuesCheckBox,  ClearGridSelection)
   ON_BN_CLICKED(IDC_StoreAllOption,           ClearGridSelection)
   ON_BN_CLICKED(IDC_StoreNotInLibraryOption,  ClearGridSelection)
   ON_BN_CLICKED(IDC_RADIO_APPLYALL,           ClearGridSelection)
   ON_BN_CLICKED(IDC_RADIO_APPLYNONE,          ClearGridSelection)
   ON_CBN_SETFOCUS(IDC_COMBO_DEF,              ClearGridSelection)
   

END_MESSAGE_MAP()


/******************************************************************************
*/

void CDataDoctorCentroidsPage::OnLoadCentroid()
{
   // Load one centroid from DB, for selected row

   int matchCount = 0;

   int row = getGrid().GetCurrentRow(); // Currently selected row
   
   if (row >= 0 && row < getGrid().GetNumberRows())
   {
      CString geomName = getGrid().QuickGetText(m_colGeomName, row);

      UpdateData(true);

      CDBInterface db;

      if (db.Connect())
      {
         // Load custom attribs even if centroid itself ends up not being in DB.
         BlockStruct *block = (BlockStruct*)getGrid().GetRowData(row);
         CDataDoctorPackage *ddpkg = (block == NULL) ? NULL : getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(block->getBlockNumber());
         LoadCustomAttribsFromDB(db, ddpkg);
         setGridRow(row, ddpkg);

         // Load the centroid if present
         CDBCentroid *dbcent = db.LookupCentroid(geomName);

         if (dbcent != NULL && block != NULL)
         {
            matchCount++;

            PageUnitsTag docPageUnits = this->m_doc->getPageUnits();

            if (true/*this->m_overwriteValues*/)
            {
               // Apply change to ccz geom, update grid

               if (block != NULL)
               {
                  m_doc->SetCentroid(*block, dbcent->getX(docPageUnits), dbcent->getY(docPageUnits), dbcent->getRot(), dbcent->getAlgorithm());

                  DataStruct *centroidData = block->GetCentroidData();
                  setGridRow(row, centroidData, dbcent->getAlgorithm(), "Set From Library", dbcent->getComment());
               }
            }
#ifdef ALLOW_LOAD_WITHOUT_APPLY
            // Load without apply (aka Overwrite not checked for group load) is not
            // supported for right-click menu options, but it could be added easily.
            else
            {
               // Set data in grid, not actually applied to ccz data yet.
               // Don't use setGridRow, because we do not want to change status of the row.
               getGrid()->setCell(row, m_colMethod, dbcent->getAlgorithm());
               getGrid()->setCell(row, m_colCentX, dbcent->getXStr(docPageUnits));
               getGrid()->setCell(row, m_colCentY, dbcent->getYStr(docPageUnits));
               getGrid()->setCell(row, m_colCentR, dbcent->getRotStr(docPageUnits));
               getGrid()->setCell(row, m_colMessage, "Settings from Library shown but NOT applied.");
            }
#endif

            resizeGrid();

            delete dbcent;
         }

      }
   }

   if (matchCount == 0)
   {
      ErrorMessage("No matching centroid found in library.", "", MB_OK);
   }

   // Always send update notification, Attributes may have been loaded even if no 
   // centroids were loaded.
   getParent().updatePropertyPages(this); // inform the world that data changed
}

void CDataDoctorCentroidsPage::OnSaveCentroid()
{
   // Save one centroid to DB, from selected row

   int rowi = getGrid().GetCurrentRow(); // Currently selected row

   UpdateData(true);

   if (rowi >= 0 && rowi < getGrid().GetNumberRows())
   {
      // Always save custom attribs, depends only on ability to connect to db.
      BlockStruct *block = (BlockStruct*)getGrid().GetRowData(rowi);
      CDataDoctorPackage *ddpkg = (block == NULL) ? NULL : getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(block->getBlockNumber());
      CDBInterface db;
      if (db.Connect())
         SaveCustomAttribsToDB(db, ddpkg);

      // Saving of centroids depends on suitable params being set.
      // What we are saving is the method for determining the centroid, not the centroid itself (unless method is XY_Value).
      // For pre-existing centroids with unknown method, there is nothing we can save.

      CString geomName = getGrid().QuickGetText(m_colGeomName, rowi);
      CString xstr     = getGrid().QuickGetText(m_colCentX, rowi);
      CString ystr     = getGrid().QuickGetText(m_colCentY, rowi);
      CString rotstr   = getGrid().QuickGetText(m_colCentR, rowi);
      CString alg      = getGrid().QuickGetText(m_colMethod, rowi);
      CString comment  = getGrid().QuickGetText(m_colComments, rowi);;

      // If xy is set and algorithm setting is acceptably legit ...
      if (xstr != "-" && ystr != "-" &&
         !(alg.IsEmpty() || alg.CompareNoCase(Q_NONE) == 0 || alg.CompareNoCase("Pre-Existing") == 0) 
         )
      {
         CDBCentroid dbcent(geomName, alg, xstr, ystr, rotstr, this->m_doc->getPageUnits(), "", "", comment);

         
         if (db.IsConnected() && db.SaveCentroid(dbcent))
         {
            this->setItemToGrid(rowi, m_colMessage, "Added To Library");
            getGrid().BestFit(m_colMessage, m_colMessage, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
            ErrorMessage("Centroid saved in database.", "", MB_ICONINFORMATION);
         }
         else
         {
            ErrorMessage("Database operation failed.", "Centroid not saved.");
         }
      }
      else
      {
         // If xy is set and algorithm setting is acceptably legit ...
         if (xstr == "-" || ystr == "-")
            ErrorMessage("X and/or Y value not set.", "Centroid not saved.", MB_OK);
         else if (alg.IsEmpty() || alg.CompareNoCase(Q_NONE) == 0 )
            ErrorMessage("Blank (or NONE)  Method setting not allowed in library.", "Centroid  not saved.", MB_OK);
         else if (alg.CompareNoCase("Pre-Existing") == 0)
            ErrorMessage("Method setting \"Pre-Existing\" not allowed in library.", "Centroid  not saved.", MB_OK);
         else
            ErrorMessage("Not sure why.", "Centroid not saved.", MB_OK);
      }
   }
}

/******************************************************************************
*/

void CDataDoctorCentroidsPage::OnBnClickedLoadFromLibrary()
{
   // Group load

   getGrid().ClearSelections();

   UpdateData(true);

   PageUnitsTag docPageUnits = this->m_doc->getPageUnits();

   int matchCount = 0;

   CDBInterface db;
   if (db.Connect())
   {
      for (int rowi = 0; rowi < getGrid().GetNumberRows(); rowi++)
      {
         // Load custom attribs, even if centroid itself is not in DB.
         BlockStruct *block = (BlockStruct*)getGrid().GetRowData(rowi);
         CDataDoctorPackage *ddpkg = (block == NULL) ? NULL : getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(block->getBlockNumber());
         LoadCustomAttribsFromDB(db, ddpkg);
         setGridRow(rowi, ddpkg);

         // Load centroid if present
         CString geomName = getGrid().QuickGetText(m_colGeomName, rowi);
         CDBCentroid *dbCent = db.LookupCentroid(geomName);
         if (dbCent != NULL && block != NULL)
         {
            matchCount++;

            if (this->m_overwriteValues)
            {
               // Apply change to ccz geom, update grid         
               if (block != NULL)
               {
                  m_doc->SetCentroid(*block, dbCent->getX(docPageUnits), dbCent->getY(docPageUnits), dbCent->getRot(), dbCent->getAlgorithm());

                  DataStruct *centroidData = block->GetCentroidData();
                  setGridRow(rowi, centroidData, dbCent->getAlgorithm(), "Set From Library", dbCent->getComment());
               }
            }
            else
            {
               // Set data in grid, not actually applied to ccz data yet.
               // Don't use setGridRow, because we do not want to change status of the row.
               getGrid().QuickSetText(m_colMethod, rowi, dbCent->getAlgorithm()); 
               getGrid().QuickSetText(m_colCentX, rowi, dbCent->getXStr(docPageUnits));
               getGrid().QuickSetText(m_colCentY, rowi, dbCent->getYStr(docPageUnits)); 
               getGrid().QuickSetText(m_colCentR, rowi, dbCent->getRotStr()); 
               getGrid().QuickSetText(m_colMessage, rowi, "Settings from Library shown but NOT applied.");
               getGrid().QuickSetText(m_colComments, rowi, dbCent->getComment());
            }

            SetCentroidComment(rowi, dbCent->getComment());

            delete dbCent;
         }
      }

      resizeGrid();
   }

   if (matchCount == 0)
   {
      ErrorMessage("No matching centroids found in library.", "", MB_OK);
   }
   else
   {
      CString msg;
      msg.Format("%d matching entries found in library.", matchCount);
      ErrorMessage(msg, "", MB_OK | MB_ICONINFORMATION);
   }

   // Always send update notification, Attributes may have been loaded even if no 
   // centroids were loaded.
   getParent().updatePropertyPages(this); // Inform other tabs that data changed
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedStoreToLibrary()
{
   // Group save

   getGrid().ClearSelections();

   UpdateData(true);

   CDBInterface db;
   if (db.Connect())
   {
      int savedCount = 0;

      PageUnitsTag docPageUnits = this->m_doc->getPageUnits();
      bool overwriteExisting = (m_storeOption == 0); // aka Store All Centroids

      for (int rowi = 0; rowi < getGrid().GetNumberRows(); rowi++)
      {
         // Save custom attribs, always goes even if centroid itself ends up not being saved.
         BlockStruct *block = (BlockStruct*)getGrid().GetRowData(rowi);
         CDataDoctorPackage *ddpkg = (block == NULL) ? NULL : getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(block->getBlockNumber());
         SaveCustomAttribsToDB(db, ddpkg);

         // Save the centroid if it ends up qualifying
         CString geomName = getGrid().QuickGetText(m_colGeomName, rowi);
         CDBCentroid *existingCent = db.LookupCentroid(geomName, false /*look in new table only*/);

         if (existingCent == NULL || overwriteExisting)
         {
            CString xstr     = getGrid().QuickGetText(m_colCentX, rowi);
            CString ystr     = getGrid().QuickGetText(m_colCentY, rowi);
            CString rotstr   = getGrid().QuickGetText(m_colCentR, rowi);
            CString alg      = getGrid().QuickGetText(m_colMethod, rowi);
            CString comments = getGrid().QuickGetText(m_colComments, rowi);;

            // If xy is set and algorithm setting is acceptably legit ...
            if (xstr != "-" && ystr != "-" &&
               !(alg.IsEmpty() || alg.CompareNoCase(Q_NONE) == 0 || alg.CompareNoCase("Pre-Existing") == 0) 
               )
            {
               CDBCentroid *dbcent = new CDBCentroid(geomName, alg, xstr, ystr, rotstr, docPageUnits, "", "", comments);

               if (db.SaveCentroid(dbcent))
                  savedCount++;

               delete dbcent;
            }
         }
         // Else already exists and not overwriting.
         // But do clean up possible object from existing entry lookup.
         if (existingCent != NULL)
            delete existingCent;
      }

      UINT iconType = savedCount > 0 ? MB_ICONINFORMATION : MB_ICONWARNING;
      CString msg;
      msg.Format("%d centroid%s saved to database.", savedCount, savedCount == 1 ? "" : "s");
      ErrorMessage(msg, "", iconType);
   }

}

//----------------------------------------------------------------------------------------

bool CDataDoctorCentroidsPage::SaveCustomAttribsToDB(CDBInterface &db, CDataDoctorPackage *ddpkg)
{
   if (ddpkg != NULL)
   {
      CDBGenericAttributeMap *dbAttribMap = ddpkg->AllocDBAttribMap();
      if (dbAttribMap->GetCount() > 0)
      {
         db.SaveGenericAttributes(ddpkg->GetGeometryName(), dbnameGeometry, dbAttribMap);
      }
      delete dbAttribMap;
   }


   return true;
}

//----------------------------------------------------------------------------------------

bool CDataDoctorCentroidsPage::LoadCustomAttribsFromDB(CDBInterface &db, CDataDoctorPackage *ddpkg)
{
   if (ddpkg != NULL)
   {
      CDBGenericAttributeMap *attmap = db.LookupGenericAttributes(ddpkg->GetGeometryName(), dbnameGeometry);
      if (attmap != NULL && attmap->GetCount() > 0)
      {
         ddpkg->LoadDBAttribs(getParent().getDataDoctor().GetPackageLibraryAttribMap(), attmap, true);
      }
      delete attmap;
   }

   return true;
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::update()
{
   if (true)
   {
      DefineGridColumnMap();
      if (this->m_grid.IsSetUp())
         this->m_grid.OnSetup();  // re-setup
   }

   fillGrid();

   m_updatePending = false;
}

/******************************************************************************
*/

BOOL CDataDoctorCentroidsPage::OnSetActive()
{
   this->getParent().SaveAsInitialPage(DdCentroidsPage);

   if (IsUpdatePending())
      update();

   return CDataDoctorPropertyPageType::OnSetActive();
}

/******************************************************************************
*/
BOOL CDataDoctorCentroidsPage::OnInitDialog() 
{
   m_toolbar.createAndLoad(IDR_ColorsLiteToolbar);

   CDataDoctorPropertyPageType::OnInitDialog();

   getGrid().AttachGrid(this, IDC_OCXBOX);

	//set default priority
	m_priority[0] = Centroid_CompOutline;
	m_priority[1] = Centroid_PinCenters;
	m_priority[2] = Centroid_PinExtents;


	m_cboDefault.InsertString(0, outlineAlgTagToString(algBodyOutline));
	m_cboDefault.InsertString(1, outlineAlgTagToString(algPinCenters));
	m_cboDefault.InsertString(2, outlineAlgTagToString(algPinExtents));
	m_cboDefault.InsertString(3, outlineAlgTagToString(algUserXY));
	m_cboDefault.SetCurSel(DEFAULT_ALGORITHM);


	fillGrid();

	getParent().EnableDisableDatabaseControls();

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
void CDataDoctorCentroidsPage::OnBnClickedClearCentroids()
{
   getGrid().ClearSelections();

   for (int gridrowi = 0; gridrowi < getGrid().GetNumberRows(); gridrowi++)
   {
      setCentroidInRow(gridrowi, Centroid_None, true /*overwrite*/);
   }
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedPinsShowAll()
{
   UpdateData(TRUE);
   fillGrid();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedPinsShowNamed()
{
   UpdateData(TRUE);
   fillGrid();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedPinsShowNumbered()
{
   UpdateData(TRUE);
   fillGrid();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedChkHideSinglePinComps()
{
   UpdateData(TRUE);  
   fillGrid();
   SaveStickySettings();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedChkUseZeroRotation()
{
   UpdateData(TRUE);  
   ClearGridSelection();
   SaveStickySettings();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::ClearGridSelection()
{
   getGrid().ClearSelections();
}

/******************************************************************************
*/
bool CDataDoctorCentroidsPage::isRecognizedMethodName(CString name)
{
   if (stringToOutlineAlgTag(name) != algUnknown)
      return true;

   if (name.CompareNoCase(Q_NONE)) return true;

   return false;
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::fillGrid()
{
	// Must have a single active PCB file.
	// If one is not set, then do not fill the grid.
	// This is just a safety check, user should not have been able to open the 
	// Generate Centroids dialog unless a single PCB file was visible.
	if (m_activeFile == NULL)
		return;

   if (!getGrid().IsSetUp())
      return;

   CWaitCursor pardonMeThisMightTakeAMoment;

   CDataDoctor& doctor = getParent().getDataDoctor();
   doctor.getPackages().GatherPackages(getCamCadDatabase(), doctor.getComponents(), doctor.GetPackageLibraryAttribMap(), CDataDoctorPackagesPage::GetOutlineFillMode());

	int activeFileNum = m_activeFile->getFileNumber();

	BlockStruct *activeFileBlock = m_activeFile->getBlock();
	if (activeFileBlock == NULL)
		return;

   getGrid().RemoveAllRows();

	CMapWordToPtr map;

	int nextRow = 0;
	CString CenXStr, CenYStr, CenRStr;

	POSITION dataPos = activeFileBlock->getDataList().GetHeadPosition();
	while (dataPos)
	{
		DataStruct *data = activeFileBlock->getDataList().GetNext(dataPos);
		if (data != NULL && data->getDataType() == T_INSERT && data->getInsert() != NULL)
		{
			int insertedBlockNum = data->getInsert()->getBlockNumber();
			void *temp = NULL;
			if (!map.Lookup(insertedBlockNum, temp))
			{
				map.SetAt(insertedBlockNum, temp);

				BlockStruct *block = m_doc->getBlockAt(insertedBlockNum);

				if (block != NULL && block->getBlockType() == blockTypePcbComponent)	
				{
               int pinCount = block->getPinCount();
               bool hasNum = block->hasNumberedPin();
               bool hasNam = block->hasNamedPin();

               if ((pinCount > 1 || !m_chkHideSinglePinComps) &&
                  ((m_radioShowPins == 0) || // show all
                  (m_radioShowPins == 1 && hasNam) || (m_radioShowPins == 2 && hasNum))) // show named or show numbered
               {
                  CString itemName = block->getName();

                  CDataDoctorPackage *ddpkg = getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(block->getBlockNumber());
                  CString refnames( ddpkg != NULL ? ddpkg->GetRefNameList() : "");

                  long row = nextRow++;

                  getGrid().SetNumberRows(row+1);

                  setItemToGrid(row, m_colGeomName, itemName);
                  getGrid().SetRowData(row, (void*)block);

                  setItemToGrid(row, m_colRefnames, refnames);

                  CString pinCountStr;
                  pinCountStr.Format("%d", pinCount);
                  setItemToGrid(row, m_colPinCnt, pinCountStr);

                  // User defined custom attribs
                  setGridRow(row, ddpkg);


                  DataStruct *centroidData = block->GetCentroidData();
                  CString comment = GetCentroidComment(block);

                  if (!centroidData)
                  {
                     setGridRow(row, "-", "-", "-", Q_NONE, dataDoctorStatusError, "No centroid", comment);
                  }
                  else
                  {
                     CString alg = "Pre-Existing"; // Default, backward compat for pre-METHOD-attrib ccz
                     if (centroidData->getAttributesRef() != NULL)
                     {
                        WORD key = m_doc->RegisterKeyWord("METHOD", 0, valueTypeString);
                        Attrib *value = NULL;
                        if (centroidData->getAttributesRef()->Lookup(key, value) && value != NULL)
                           alg = value->getStringValue();
                     }
                     if (!isRecognizedMethodName(alg))
                        alg = "Pre-Existing"; // reset


                     this->setGridRow(row, centroidData, alg, "", comment);
                  }

               }
				}
			}
		}
	}

	resizeGrid();

   // Initial grid order sorted on geometry name column.
   // Weird bug: The sort does nto work completely the first time. The grid is all sorted
   // except for the first row, which comeos out out of place. A second call to sortby
   // seems to fix it. Havent' looked into underlying situation yet, but doing the
   // sort twice seems to fix it.
   ///getGrid().SortBy(m_colGeomName, UG_SORT_ASCENDING);
   ///getGrid().SortBy(m_colGeomName, UG_SORT_ASCENDING);
   getGrid().ReSort(QGeometryName);
   getGrid().RedrawAll();

}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::setItemToGrid(GRID_CNT_TYPE gridrow, GRID_CNT_TYPE gridcol, CString itemValue)
{
   if (gridrow >= getGrid().GetNumberRows())
      getGrid().SetNumberRows(gridrow+1);

   // Could do this to other numeric column types too, but would have to put
   // special handling in because x/y/rot values can be "-" when centroid
   // is not set. Maybe some other time. The main reason to do this is
   // so if user selects column for sorting, they get a numeric sort
   // instead of lexical sort. But there are a number of ways to deal
   // with that, e.g. specialized cell comparator, or special GetCell and SetCell.

   if (gridcol == this->m_colPinCnt)
      getGrid().QuickSetNumber(gridcol, gridrow, atoi(itemValue));
   else
      getGrid().QuickSetText((int)gridcol, (long)gridrow, itemValue);

}

void CDataDoctorCentroidsPage::setRowStyle(int rowIndx, DataDoctorStatusTag status)
{
   switch (status)
   {
   case dataDoctorStatusOk:  // case dataDoctorStatusOk:       
      getGrid().SetRowBackColor(rowIndx,                getGrid().m_greenBackgroundColor);
      getGrid().QuickSetTextColor(m_colStatus, rowIndx, colorDkGreen);
      break;
   case dataDoctorStatusError:  // case dataDoctorStatusError:    
      getGrid().SetRowBackColor(rowIndx,                getGrid().m_redBackgroundColor);
      getGrid().QuickSetTextColor(m_colStatus, rowIndx, colorRed);      
      break;
   }
}


/******************************************************************************
*/
void CDataDoctorCentroidsPage::setGridRow(GRID_CNT_TYPE row, CDataDoctorPackage *ddpkg)
{
   POSITION customPos = this->m_customColumnMap_CentroidGrid.GetStartPosition();
   while (customPos != NULL)
   {
      CString customColName;
      CDDSpecialColumn *custCol = NULL;
      this->m_customColumnMap_CentroidGrid.GetNextAssoc(customPos, customColName, custCol);
      if (custCol != NULL)
      {
         CString cellval = ddpkg != NULL ? ddpkg->GetGenericAttrib(custCol->GetName()) : "";
         setItemToGrid(row, custCol->GetColumnIndexRef(), cellval);
         /////getGrid().QuickSetText(custCol->GetColumnIndexRef(), row, cellval);
      }
   }
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::setGridRow(GRID_CNT_TYPE row, CString centXstr, CString centYstr, CString centRstr, CString algorithm, DataDoctorStatusTag status, CString message, CString comment)
{
	setItemToGrid(row, (GRID_CNT_TYPE) m_colCentX, centXstr);
	setItemToGrid(row, (GRID_CNT_TYPE) m_colCentY, centYstr);
	setItemToGrid(row, (GRID_CNT_TYPE) m_colCentR, centRstr);
	setItemToGrid(row, (GRID_CNT_TYPE) m_colMethod, algorithm);
	setItemToGrid(row, (GRID_CNT_TYPE) m_colComments, comment);

   CString statusStr(status == dataDoctorStatusOk ? "OK" : "Errors");
   setItemToGrid((int)row, (GRID_CNT_TYPE) m_colStatus,  statusStr);
   setItemToGrid(row, (GRID_CNT_TYPE) m_colMessage, message);
   setRowStyle(row, status);
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::setGridRow(GRID_CNT_TYPE row, double centX, double centY, int centR, CString algorithm, CString message, CString comment)
{
	int decimals = GetDecimals(m_doc->getSettings().getPageUnits());

	CString centXstr, centYstr, centRstr;

	centXstr.Format("%.*lf", decimals, centX);
	
	centYstr.Format("%.*lf", decimals, centY);
	
	centRstr.Format("%d", normalizeDegrees(centR));

	setGridRow(row, centXstr, centYstr, centRstr, algorithm, dataDoctorStatusOk, message, comment);

}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::setGridRow(GRID_CNT_TYPE row, double centX, double centY, double centR, CString algorithm, CString message, CString comment)
{
	setGridRow(row, centX, centY, DcaRound(centR), algorithm, message, comment);
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::setGridRow(GRID_CNT_TYPE row, DataStruct *centroidData, CString algorithm, CString message, CString comment)
{
	if (centroidData != NULL && centroidData->getInsert() != NULL) 
	{
		setGridRow(row, centroidData->getInsert()->getOriginX(), centroidData->getInsert()->getOriginY(),
			DcaRound(centroidData->getInsert()->getAngleDegrees()), algorithm, message, comment);
	}
}

/******************************************************************************
*/
bool CDataDoctorCentroidsPage::calculateCentroid(BlockStruct *block, int method, double *x, double *y, CString &curAlg, CString &message)
{
   message = "";

	if (block != NULL)
	{
		// Name of preferred algorithm (aka method)
		switch(method)
		{
		case Centroid_PinCenters:
			curAlg = outlineAlgTagToString(algPinCenters);
			break;
		case Centroid_PinExtents:
			curAlg = outlineAlgTagToString(algPinExtents);
			break;
		case Centroid_CompOutline:
			curAlg = outlineAlgTagToString(algBodyOutline);
			break;
		case Centroid_PinBodyExtents:
			curAlg = outlineAlgTagToString(algPinBodyExtents);
			break;
		}

		// Try preferred method first...
		if (CalculateCentroid(m_doc, block, method, x, y))
      {
         return true; // Got centroid first try using preferred algorithm
      }
      else
		{
         // Preferred algorithm failed. Try other algorithms.
         
         //Look for special case of Component Outline as preferred algorithm, and
         // provide special message (per Mark, Centroid Makeover spec, May/June 2006)
         CExtent compExtents;
         if (method == Centroid_CompOutline &&
            !GetComponentOutlineExtents(m_doc, block, false/*includePins*/, true /*includeBody*/, compExtents))
         {
            message += " No component outline.";
         }

			for (int cnt = 0; cnt < 3; cnt++)
			{
				if (CalculateCentroid(m_doc, block, m_priority[cnt], x, y))
				{
					switch(m_priority[cnt])
					{
					case Centroid_PinCenters:
						curAlg = outlineAlgTagToString(algPinCenters);
						break;
					case Centroid_PinExtents:
						curAlg = outlineAlgTagToString(algPinExtents);
						break;
					case Centroid_CompOutline:
						curAlg = outlineAlgTagToString(algBodyOutline);
						break;
					case Centroid_PinBodyExtents:
						curAlg = outlineAlgTagToString(algPinBodyExtents);
						break;
					}

					return true; // We have a centroid
				}
            else
            {
               // Algorithm failed
					switch(m_priority[cnt])
					{
					case Centroid_PinCenters:
						message += " Pin centers method failed.";
						break;
					case Centroid_PinExtents:
						message += " Pin extents method failed.";
						break;
					case Centroid_CompOutline:
						message += " Component outline method failed.";
						break;
					case Centroid_PinBodyExtents:
						message += " Pin & Body extents method failed.";
						break;
					}
            }

			}

			// If still here, all algorithms failed. Message(s) already set.
			curAlg = "None";
		}
	}
   else
   {
      message = "No geometry block.";
   }

   return false; // Could not make a centroid
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::setCentroidInRow(int rowIndex, int method, bool overwrite)
{
   int centAlg = method;
   // int retVal = 0;

   BlockStruct *block = (BlockStruct*)getGrid().GetRowData(rowIndex);

   if (IsCentroidCalculatable(centAlg))
   {
      //'set the centroid per the specified algorithm

      if (!block)
      {
         //retVal = RC_HOLE_IN_ARRAY;
      }
      else
      {
         CString comment = GetCentroidComment(block);
         if (block->GetCentroidData() != NULL && !overwrite)
         {
            //retVal = RC_ITEM_ALREADY_EXISTS;
         }
         else
         {
            CString curAlg;   // output arg, what algorithm actually got applied
            CString message;  // status, errors, etc.
            double x=0, y=0;
            if (calculateCentroid(block, centAlg, &x, &y, curAlg, message))
            {
               // Get rotation, may involve a rotation offset
               double angleDegrees = 0.0;
               if (!this->m_chkZeroRotation)
               {
                  // Update the centroid data with current xy, 
                  // get the rotation offset, centroid will be updated again.
                  // Don't need to bother with method attr here, save a microsecond.
                  m_doc->SetCentroid(*block, x, y, angleDegrees);
                  angleDegrees += GenerateRotationOffset(block, block->GetCentroidData());
               }

               m_doc->SetCentroid(*block, x, y, angleDegrees, curAlg);

               setGridRow(rowIndex, x, y, angleDegrees, curAlg, message, comment);

               //retVal = RC_SUCCESS;
            }
            else
            {
               // All automatic algorithms failed, remove bogus centroid if there is one
               DataStruct *centroidData = block->GetCentroidData();
               if (centroidData)
                  RemoveOneEntityFromDataList(m_doc, &block->getDataList(), centroidData);
               setGridRow(rowIndex, "-", "-", "-", Q_NONE, dataDoctorStatusError, message, comment);
            }
         }
      }
   }
   else if (centAlg == Centroid_None)
   {
      DataStruct *centroidData = block->GetCentroidData();
      if (centroidData)
         RemoveOneEntityFromDataList(m_doc, &block->getDataList(), centroidData);

      CString comment = GetCentroidComment(block);
      setGridRow(rowIndex, "-", "-", "-", Q_NONE, dataDoctorStatusError, "No centroid", comment);

   }
   else if (centAlg = Centroid_XyValue) 
   {
      ApplyXyValue(rowIndex);
   }

   resizeGrid();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::setByDefaultAllRows()
{
   UpdateData();

   short centAlg;

   // Default algorithm from GUI

   if (m_cboDefault.GetCurSel() == 0)
      centAlg = Centroid_CompOutline;
   else if (m_cboDefault.GetCurSel() == 1)
      centAlg = Centroid_PinCenters;
   else if (m_cboDefault.GetCurSel() == 2)
      centAlg = Centroid_PinExtents;
   else if (m_cboDefault.GetCurSel() == 3)
      centAlg = Centroid_XyValue;
   else
      centAlg = Centroid_None;


   bool overwrite = (m_radioApplyAll == 0);

   for (int gridrowi = 0; gridrowi < getGrid().GetNumberRows(); gridrowi++)
   {
      setCentroidInRow(gridrowi, centAlg, overwrite);
   }

   resizeGrid();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::resizeGrid()
{
   getGrid().BestFit(0, getGrid().GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedBtnGenCentroids()
{
   getGrid().ClearSelections();
   setByDefaultAllRows();
   m_doc->UpdateAllViews(NULL);
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedBtnShowgeom()
{
   fillGrid();
}

/******************************************************************************
*/
BEGIN_EVENTSINK_MAP(CDataDoctorCentroidsPage, CResizingDialog)
	ON_EVENT(CDataDoctorCentroidsPage, IDC_VSFLEX_GEN_CEN, 22, StartEditGrid, VTS_I4 VTS_I4 VTS_PBOOL)
END_EVENTSINK_MAP()



/******************************************************************************
*/

void CDataDoctorCentroidsPage::OnRButtonDown(CPoint windowRelativePoint)
{
   // No op for rbutton today.
}

/******************************************************************************
*/

void CDataDoctorCentroidsPage::StartEditGrid(long row, long col, BOOL* cancel)
{
	// This controls whether user keyboard input to cell is accepted

	// Default, editing cell not allowed
   *cancel = TRUE;


   // Allow editing for certain cells and conditions

   if (col == m_colMethod) 
   {
      // Method change always allowed
      *cancel = FALSE;
   }
   else if (col == m_colCentR)
   {
      // Can change rotation so long as centroid exists
      CString method = getGrid().QuickGetText(m_colMethod, row);
      BlockStruct *block = (BlockStruct*)getGrid().GetRowData(row);
      if (method.CompareNoCase(Q_NONE) != 0 && block->GetCentroidData() != NULL)
         *cancel = FALSE;
   }
   else if (col == this->m_colCentX || col == m_colCentY)
   {
      // X & Y editable only for XY-Value method
      CString method = getGrid().QuickGetText(m_colMethod, row);
      if (stringToOutlineAlgTag(method) == algUserXY)
      {
         *cancel = FALSE;
      }
   }
   else if (this->IsCustomCentroidGridColumn(col))
   {
      *cancel = FALSE;
   }
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::AfterEditGrid(long row, long col)
{
   if (row >= 0 && row < getGrid().GetNumberRows())
   {
      if (col == this->m_colMethod)
      {
         // Apply new method selection, resets all of x, y, rot
         ApplyOneRow(row);
      }
      else if (col == this->m_colCentR)
      {
         // Just change rotation on existing centroid
         ApplyUserRotation(row);
      }
      else if (col == m_colCentX || col == m_colCentY)
      {
         // Should only have been able to edit X or Y if method was
         // already XY-Value, but we'll double check. Apply new grid
         // values if XY-Value.
         CString method = getGrid().QuickGetText(m_colMethod, row);
         if (stringToOutlineAlgTag(method) == algUserXY)
         {
            ApplyXyValue(row);
         }
      }
      else if (this->IsCustomCentroidGridColumn(col))
      {
         CDDSpecialColumn *usercol = m_customColumnMap_CentroidGrid.GetSpecialColumn(col);
         BlockStruct *block = (BlockStruct*)getGrid().GetRowData(row);

         if (usercol != NULL && block != NULL)
         {  
            CString val = getGrid().QuickGetText(col, row);
            CDataDoctorPackage *ddpkg = getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(block->getBlockNumber());
            if (ddpkg != NULL)
            {
               ddpkg->SetGenericAttrib(usercol->GetName(), val);
               getParent().updatePropertyPages(this);  // Notify other pages, custom attribs are only things on this page that effect other pages
            }
         }
      }
      else if (col == this->m_colComments)
      {
         CString val = getGrid().QuickGetText(col, row);
         SetCentroidComment(row, val);         
      }

      // Zoom again, to cause redraw of component
      DoEditGeom();
   }
}

int CDataDoctorCentroidsPage::GetCentroidCommentKeywordIndex()
{
   CCamCadDatabase camcadDatabase(*m_doc);

   m_CentroidCommentKeywordIndex = camcadDatabase.getKeywordIndex("CentroidComment");

	if (m_CentroidCommentKeywordIndex < 0)
   {
      m_CentroidCommentKeywordIndex = camcadDatabase.registerKeyword("CentroidComment", valueTypeString);
   }

   return m_CentroidCommentKeywordIndex;
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::SetCentroidComment(int row, CString comment)
{
   int keyword = GetCentroidCommentKeywordIndex();

   // Add Comment Attribute to Geometry
   BlockStruct *block = (BlockStruct*)getGrid().GetRowData(row);
   if(block != NULL && block->getAttributesRef())
   {
      block->getAttributesRef()->setAttribute(m_doc->getCamCadData(),keyword, comment, attributeUpdateOverwrite);
   }
   
   // Add Comment Attribute to Component
   CDataDoctorPackage *ddpkg = getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(block->getBlockNumber());
   if(ddpkg)
   {
      for (int j = 0; j < ddpkg->m_allInsertsOfThisPkg.GetCount(); j++)
      {
         CDataDoctorComponent *ddcomp = ddpkg->m_allInsertsOfThisPkg.GetAt(j);
         if(ddcomp && ddcomp->getComponent())
         {
            DataStruct *data = ddcomp->getComponent();
            data->getAttributesRef()->setAttribute(m_doc->getCamCadData(),keyword, comment, attributeUpdateOverwrite);
         }
      }
   } 
}

CString CDataDoctorCentroidsPage::GetCentroidComment(BlockStruct *block)
{
   int keyword = GetCentroidCommentKeywordIndex();

   CString comment = "";
   if(block && block->getAttributesRef())
   {
      CAttribute *attrib = block->getAttributesRef()->lookupAttribute(keyword);
      if(attrib) comment = attrib->getStringValue();
   }
   
   return comment;
}

/******************************************************************************
*/
LRESULT CDataDoctorCentroidsPage::OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   AfterEditGrid(wParamROW, lParamCOL);

   return 0;

}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::ApplyUserRotation(int rowIndex)
{
   // Change only the rotation, leave rest of centroid as-is

   double x = 0.0;
   double y = 0.0;
   double rot = 0.0;

   if ((CString)(getGrid().QuickGetText(m_colCentR, rowIndex)) == "-")
      rot = 0.0;
   else 
      rot = atoi((CString)getGrid().QuickGetText(m_colCentR, rowIndex));

   BlockStruct *block = (BlockStruct*)getGrid().GetRowData(rowIndex);
   DataStruct *centB4 = block->GetCentroidData();
   if (centB4 != NULL)
   {
      x = centB4->getPoint()->x;
      y = centB4->getPoint()->y;
   }

   DataStruct& cent = m_doc->SetCentroid(*block, x, y, rot);
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::ApplyXyValue(int rowIndex)
{
   double x, y, rot;

   if ((CString)getGrid().QuickGetText(m_colCentX, rowIndex) == "-" || 
      (CString)getGrid().QuickGetText(m_colCentY, rowIndex) == "-")
      x = y = 0.0;
   else
   {
      x = atof((CString)getGrid().QuickGetText(m_colCentX, rowIndex));
      y = atof((CString)getGrid().QuickGetText(m_colCentY, rowIndex));
   }

   // Not clear this setting should be active for XY-Value method
   //if (m_chkZeroRotation)
   //{
   //   // GUI setting says force rotation to zero
   //   rot = 0.0;
   //}
   //else
   {
      if ((CString)getGrid().QuickGetText(m_colCentR, rowIndex) == "-")
         rot = 0.0;
      else 
         rot = atoi((CString)getGrid().QuickGetText(m_colCentR, rowIndex));
   }

   BlockStruct *block = (BlockStruct*)getGrid().GetRowData(rowIndex);

   DataStruct& cent = m_doc->SetCentroid(*block, x, y, rot, outlineAlgTagToString(algUserXY));
   CString comment = GetCentroidComment(block);

   this->setGridRow(rowIndex, x, y, rot, outlineAlgTagToString(algUserXY), "", comment);
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::ApplyOneRow(int rowIndex)
{
   // Apply current algrithm setting in row to get new centroid

   int method;

   DoEditGeom();
   CString alg = getGrid().QuickGetText(m_colMethod, rowIndex);
   if (stringToOutlineAlgTag(alg) == algBodyOutline)
      method = Centroid_CompOutline;
   else if (stringToOutlineAlgTag(alg) == algPinCenters)
      method = Centroid_PinCenters;
   else if (stringToOutlineAlgTag(alg) == algPinExtents)
      method = Centroid_PinExtents;
   else if (stringToOutlineAlgTag(alg) == algUserXY)
      method = Centroid_XyValue;
   else
      method = Centroid_None;


   setCentroidInRow(getGrid().GetCurrentRow(), method, true /*overwrite*/);

}

/******************************************************************************
* DoEditGeom
*/
void CDataDoctorCentroidsPage::DoEditGeom()
{
   long row = getGrid().GetCurrentRow();

   if (row >= 0 && row < getGrid().GetNumberRows())
   {
	   BlockStruct *insertedCompBlk = (BlockStruct*)getGrid().GetRowData(row);
      
      if (insertedCompBlk != NULL)
      {
         BlockStruct *fakeBlock = NULL;

         fakeBlock = Graph_Block_On(GBO_OVERWRITE, "OutlineEditBlock", insertedCompBlk->getFileNumber(), 0);
         fakeBlock->setBlockType(blockTypeGeometryEdit);

         Graph_Block_Reference(insertedCompBlk->getBlockNumber(), "ReferenceBackground", 0., 0., 0., 0, 1., -1);

         // Getting the realpart block is a roundabout affair.
         // This is mostly because this tab/grid is based on geomtry blocks in the file->block, as opposed to the
         // various part/coomponent/realpart lists that are in DD. So what we have in the GetRowData is a component
         // geometry block with no hint as to what component/part it goes with.
         // That ought to be changed, but it is a big change, a bigger change than I want to pursue this close to release time. 
         // So we are going to have to hunt down the realpart block to get the outline for display.
         // Note that we are finding one outline to show, first CDataDoctorPart found is the one used.
         // There can be more than one part using this geometry, so more than one outline shape!
         // We are accepting that as a minor flaw for now, since there is no way to tell from user's selection of
         // a geometry name which part number they have in mind.

         // Based on Package data
         // This is the best choice because package names are geometry names, and geometry names are what centroids grid
         // is based on. So we can explain the correlation, and perhaps justify why part number based outlines
         // are ignored for centroids display usage.
         CDataDoctorPackage *ddpkg = getParent().getDataDoctor().getPackages().FindPkgUsingGeomBlk(insertedCompBlk->getBlockNumber());

         if (ddpkg != NULL)
         {
            BlockStruct *realpartBlk = ddpkg->GetRealPart().GetBlock();
            if (realpartBlk != NULL)
               Graph_Block_Reference(realpartBlk->getBlockNumber(), "PackageOutline", 0., 0., 0., 0, 1., -1);
         }

#ifdef NOT_USED // But could be alternative, e.g. if ddpkg is NULL or otherwise fails to display something
         // Based on Part data
         CDataDoctorPart *ddpart = getParent().getDataDoctor().getParts().FindPartUsingGeomBlk(insertedCompBlk->getBlockNumber());

         if (ddpart != NULL)
         {
            BlockStruct *realpartBlk = ddpart->GetRealPart().GetBlock();
            if (realpartBlk != NULL)
               Graph_Block_Reference(realpartBlk->getBlockNumber(), "PackageOutline", 0., 0., 0., 0, 1., -1);
         }
#endif

         Graph_Block_Off();

         EditGeometry(m_doc, fakeBlock );
      }
   }

}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnBnClickedBtnClose()
{
   OnCancel();
}

/******************************************************************************
*/
void CDataDoctorCentroidsPage::OnCancel()
{
   m_doc->OnDoneEditing();

   CDataDoctorPropertyPageType::OnCancel();
}

/******************************************************************************
*/

BOOL CDataDoctorCentroidsPage::OnKillActive()
{
   m_doc->OnDoneEditing();

   return CDataDoctorPropertyPageType::OnKillActive();
}

/******************************************************************************
*/
double CDataDoctorCentroidsPage::GenerateRotationOffset(BlockStruct* block, DataStruct* centroid)
{   
   double rot = 0.;
   if(GenerateCentroidRotationOffset(block,centroid,rot))
	   map.SetAt((void*)block, (void*)centroid); //flag that this centroid has the rot offset already

   return rot;   
}

//***************************************************************************************

void CDataDoctorCentroidsPage::SaveStickySettings()
{
   // Save setting in registry, return true if is appears to work, otherwise false
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      settings.setValue("HideSinglePinComponents", this->m_chkHideSinglePinComps ? "Yes" : "No");
      settings.setValue("UseZeroRotation", this->m_chkZeroRotation ? "Yes" : "No");
   }
}

//***************************************************************************************

void CDataDoctorCentroidsPage::LoadStickySettings()
{
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      CString val;

      if (settings.getValue("HideSinglePinComponents", val))
      {
         if (val.CompareNoCase("No") == 0)
            m_chkHideSinglePinComps = 0;
         else
            m_chkHideSinglePinComps = 1;
      }

      if (settings.getValue("UseZeroRotation", val))
      {
         if (val.CompareNoCase("No") == 0)
            m_chkZeroRotation = 0;
         else
            m_chkZeroRotation = 1;
      }
   }

}

//***************************************************************************************


///////////////////////////////////////////////////////////////////////////////////////////
// 
// CENTROIDS GRID
//

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void CDDCentroidsGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	

   if (this->m_centroidsPage != NULL)
   {
      SetNumberCols(m_centroidsPage->m_columnDefMap.getSize());
      SetNumberRows(0);

      for (int indx = 0; indx < m_centroidsPage->m_columnDefMap.getSize(); indx++)
      {
         CColumnDef *cd = m_centroidsPage->m_columnDefMap.getAt(indx);
         CString colname = cd->GetTitle();
         int *colindxptr = cd->GetColIndexPtr();

         QuickSetText(*colindxptr, -1, colname);
      }

      // Method option menu, same for all cells in column
      CUGCell cell;
      GetColDefault(this->m_centroidsPage->m_colMethod, &cell); 
      cell.SetCellType(UGCT_DROPLIST);
      CString methodlist;
      methodlist.Format("%s\n%s\n%s\n%s\n%s\n", 
         QNone,
         outlineAlgTagToString(algBodyOutline),
         outlineAlgTagToString(algPinCenters), 
         outlineAlgTagToString(algPinExtents), 
         outlineAlgTagToString(algUserXY));
      cell.SetLabelText( methodlist ); 
      SetColDefault(this->m_centroidsPage->m_colMethod, &cell);

      // Rotation option menu, same for all cells in column
      GetColDefault(this->m_centroidsPage->m_colCentR, &cell); 
      cell.SetCellType(UGCT_DROPLIST);
      CString rotlist("0\n45\n90\n135\n180\n225\n270\n315\n");
      cell.SetLabelText( rotlist ); 
      SetColDefault(this->m_centroidsPage->m_colCentR, &cell);

      EnableColSwapping(TRUE);

      BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      SetColWidth(-1, 0); // get rid of "row heading"

      SetHighlightRow(TRUE); // highlight entire selected row, not just selected cell

      // Enable Right-click menu
      EnableMenu(TRUE);

      SetSetUp(true);
   }
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

int CDDCentroidsGrid::OnEditStart(int col, long row, CWnd **edit)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(**edit);
   
   // possibly there is a better place for this, eg maybe in whole grid key-up
   this->m_centroidsPage->DoEditGeom();

   *edit = &m_myCUGEdit;

   if (
      // These are data or feedback, not user alterable here
      col == m_centroidsPage->m_colStatus   ||
      col == m_centroidsPage->m_colGeomName ||
      col == m_centroidsPage->m_colPinCnt   ||
      col == m_centroidsPage->m_colMessage  ||
      col == m_centroidsPage->m_colRefnames ||
      // These are controlled by option menus, no text entry allowed
      col == m_centroidsPage->m_colCentR    ||
      col == m_centroidsPage->m_colMethod  )
   {
      return false;
   }
   else if (col == m_centroidsPage->m_colCentX ||
      col == m_centroidsPage->m_colCentY)
   {
      // Edit allowed when method is XY-Value only,
      // otherwise no edit.
      {
         CString mthd = this->QuickGetText(this->m_centroidsPage->m_colMethod, row);
         if (stringToOutlineAlgTag(mthd) == algUserXY)
            return true;
         else
            return false;
      }
   }
   else if (this->m_centroidsPage->IsCustomCentroidGridColumn(col) ||
      col == m_centroidsPage->m_colComments)
   {
      return true;
   }

	return false;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCellTypeNotify
//    HANDLES OPTION MENUS
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
int CDDCentroidsGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	UNREFERENCED_PARAMETER(param);

   // This works for both the Rotation and Method droplists.

   if (msg == UGCT_DROPLISTSTART)
   {
      // The droplist is being opened, zoom to component in row
      this->m_centroidsPage->DoEditGeom();
   }
   else if (msg == UGCT_DROPLISTPOSTSELECT)
   {
      // Droplist selection has been made, update component to data in grid
      this->m_centroidsPage->AfterEditGrid(row, col);
   }
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//    HANDLES TEXT CELL EDIT -- See CellTypeNotify() for Option Menus.
//		This notification is sent when the edit is being finished
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being canceled
//	Return:
//		TRUE - to allow the edit to proceed
//		FALSE - to force the user back to editing of that same cell
int CDDCentroidsGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   this->m_centroidsPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);

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
void CDDCentroidsGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	UNREFERENCED_PARAMETER(item);

   int jj = 0;

   this->m_centroidsPage->SendMessage(WM_COMMAND, item);
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
int CDDCentroidsGrid::OnMenuStart(int col,long row,int section)
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

   CString name = QuickGetText(this->m_centroidsPage->m_colGeomName, row);

   // The popup choices are connected to commands, the commands
   // will operate on the currently selected row, so set the selection.
   this->ClearSelections();
   SetMultiSelectMode(FALSE); // this is the default, but seems to be getting turned on somehow, not on purpose by our code
   this->Select(col, row);
   this->RedrawAll(); // maybe for speed should get old selection and just redraw that row

   EmptyMenu();

   CString option;

   option.Format("Load %s from Library", name);
   this->AddMenuItem(ID_DataDoctorCommand_LoadPart, option);

   option.Format("Save %s to Library", name);
   this->AddMenuItem(ID_DataDoctorCommand_SavePart, option);

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
void CDDCentroidsGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   this->m_centroidsPage->m_columnDefMap.Update(this);

}
