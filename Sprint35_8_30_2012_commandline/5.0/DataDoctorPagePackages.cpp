// $Header: /CAMCAD/4.6/DataDoctorPagePackages.cpp 13    5/03/07 5:44p Rick Faltersack $

#include "stdafx.h"

#include "CCEtoODB.h"

#include "edit.h" // for point editor, for bulge



#include "PcbUtil.h" // for GetPinExtents
#include "GenerateCentroidAndOutline.h"  // for #define ALG_NAME_UNKNOWN, want to use same as Access Analysis

#include "ODBC_Lib.h"

//#include "DataDoctorPagePackages.h"
#include "DataDoctorDialog.h"  // Includes DataDoctorPagePackages.h

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

IMPLEMENT_DYNAMIC(CDataDoctorPackagesPage, CDataDoctorPropertyPageType)

int CDataDoctorPackagesPage::m_outlineFill = 0;

CDataDoctorPackagesPage::CDataDoctorPackagesPage(CDataDoctorPropertySheet& parent)
	: CDataDoctorPropertyPageType(CDataDoctorPackagesPage::IDD),
     m_parent(parent)
     //, m_gridViewMode(PkgGridViewGeometries)
     , m_pkgGeomGrid(this)
     , m_pkgPartGrid(this)
     , m_outlineEditGrid(this)
     , m_overwriteValues(1)
     , m_radioApplyAll(1)  // Apply to parts with no outline
	  , m_storeOption(0)  // Case 2058, init to "save not in database"  ///!!!!*rcf Case 2058 has this as 1, TEMPORARILY set to 0 for outline fussing
     , m_toolbar(*this)
     , m_columnDefMap_GeomGrid("DD Geometry Packages Grid")
     , m_columnDefMap_PartGrid("DD Part Number Packages Grid")
     , m_updatePending(false)
     , m_updateOtherGrid(false)
     , m_viewModePartNum(0)
     /////, m_outlineFill(0)  // Changed to static so can access outside of this tab, needed to Parts tab which also builds outlines
     , m_chkHideSinglePinComps(0)
     , m_outlineChamferRound(0)
     , m_highlightPntInsertData(NULL)
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

CDataDoctorPackagesPage::~CDataDoctorPackagesPage()
{
}

/******************************************************************************
*/
void CDataDoctorPackagesPage::DefineGridColumnMaps()
{
   // Set up the column definitions.
   // Default column order left to right is order they get defined here.
   m_columnDefMap_GeomGrid.empty();
   m_columnDefMap_GeomGrid.AddColDef( QGeometryName,  &m_colGeometry_GeomGrid );
   m_columnDefMap_GeomGrid.AddColDef( QOutlineMethod, &m_colOutlineMethod_GeomGrid );
   m_columnDefMap_GeomGrid.AddColDef( QPackageAlias,  &m_colPkgAlias_GeomGrid );
   m_columnDefMap_GeomGrid.AddColDef( QDPMO,          &m_colDPMO_GeomGrid );
   m_columnDefMap_GeomGrid.AddColDef( QHeight,        &m_colPkgHeight_GeomGrid );
   m_columnDefMap_GeomGrid.AddColDef( QFamily,        &m_colFamily_GeomGrid );
   m_columnDefMap_GeomGrid.AddColDef( QComments,      &m_colComments_GeomGrid );
   m_columnDefMap_GeomGrid.AddColDef( QRefDes,        &m_colRefnames_GeomGrid );
   m_columnDefMap_GeomGrid.AddCustomAttributes(m_customColumnMap_GeomGrid, this->getParent().getDataDoctor().GetPackageLibraryAttribMap());
   m_columnDefMap_GeomGrid.AddColDef( QMessage,       &m_colMessage_GeomGrid );

   // Load possible user overrides (column swaps)
   m_columnDefMap_GeomGrid.LoadSettings();


   m_columnDefMap_PartGrid.empty();
   m_columnDefMap_PartGrid.AddColDef( QPartNumber,    &m_colPartNumber_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QGeometryName,  &m_colGeometry_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QPackageSource, &m_colPkgSource_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QOutlineMethod, &m_colOutlineMethod_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QPackageAlias,  &m_colPkgAlias_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QDPMO,          &m_colDPMO_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QHeight,        &m_colPkgHeight_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QFamily,        &m_colFamily_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QComments,      &m_colComments_PartGrid );
   m_columnDefMap_PartGrid.AddColDef( QRefDes,        &m_colRefnames_PartGrid );
   m_columnDefMap_PartGrid.AddCustomAttributes(m_customColumnMap_PartGrid, this->getParent().getDataDoctor().GetPackageLibraryAttribMap());
   m_columnDefMap_PartGrid.AddColDef( QMessage,       &m_colMessage_PartGrid );

   // Load possible user overrides (column swaps)
   m_columnDefMap_PartGrid.LoadSettings();
}


/******************************************************************************
*/

void CDataDoctorPackagesPage::DoDataExchange(CDataExchange* pDX)
{
	CDataDoctorPropertyPageType::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_COMBO_DEF, m_cboDefault);
   DDX_Control(pDX, IDC_COMBO_TEMPLATE, m_cboTemplate);
   DDX_Radio(pDX, IDC_RADIO_APPLYALL, m_radioApplyAll);
	DDX_Check(pDX, IDC_OverwriteValuesCheckBox, m_overwriteValues);
	DDX_Radio(pDX, IDC_StoreAllOption, m_storeOption);
   DDX_Check(pDX, IDC_CHK_VIEW_PARTNUM, m_viewModePartNum);
   DDX_Check(pDX, IDC_CHK_FILL_OUTLINES, m_outlineFill);
   DDX_Check(pDX, IDC_CHK_HIDE_SINGLE_PIN_COMPS, m_chkHideSinglePinComps);
   DDX_Check(pDX, IDC_CHK_CHAMFER_ROUND, m_outlineChamferRound);
   ///DDX_Control(pDX, IDC_EDIT_A1, m_templateEditA1);

}

/******************************************************************************
*/

BEGIN_MESSAGE_MAP(CDataDoctorPackagesPage, CDataDoctorPropertyPageType)
   ON_BN_CLICKED(IDC_CHK_VIEW_PARTNUM, OnBnClickedGridViewMode)
   ON_BN_CLICKED(IDC_CHK_FILL_OUTLINES, OnBnClickedFillOutlines)
   ON_BN_CLICKED(IDC_CHK_HIDE_SINGLE_PIN_COMPS, OnBnClickedChkHideSinglePinComps)
   ON_CBN_SELCHANGE(IDC_COMBO_TEMPLATE, OnCboSelchangeTemplate)
   ON_BN_CLICKED(IDC_BTN_CREATE_TEMPLATE_OUTLINE, OnBnClickedCreateCustomTemplateOutline)
   ON_BN_CLICKED(IDC_BTN_APPLY_OFFSET, OnBnClickedApplyOffset)
   ON_BN_CLICKED(IDC_BTN_ROTATE90,     OnBnClickedRotate90)
   ON_BN_CLICKED(IDC_LoadButton, OnBnClickedLoadFromLibrary)
   ON_BN_CLICKED(IDC_StoreButton, OnBnClickedStoreToLibrary)
   ON_COMMAND(ID_DataDoctorCommand_LoadPart, OnLoadPackage)
   ON_COMMAND(ID_DataDoctorCommand_SavePart, OnSavePackage)
   ON_COMMAND(ID_DataDoctorCommand_OutlineInsertAfter,  OnBnClickedInsertVertexAfter)
   ON_COMMAND(ID_DataDoctorCommand_OutlineInsertBefore, OnBnClickedInsertVertexBefore)
   ON_COMMAND(ID_DataDoctorCommand_OutlineDeleteVertex, OnBnClickedDeleteVertex)
   ON_COMMAND(ID_DataDoctorCommand_OutlineCurveToNext,  OnBnClickedCurveToNext)
   ON_BN_CLICKED(ID_VL_ZOOM_ZOOMIN,       OnTbBnClickedZoomIn)
   ON_BN_CLICKED(ID_VL_COLORSETS_CUSTOM,  OnTbBnClickedColorsetsCustom)
   ON_BN_CLICKED(ID_LAYERTYPES,           OnTbBnClickedLayerTypes)
   ON_BN_CLICKED(ID_SHOW_FILLS,           OnTbBnClickedShowPolyFill)
   ON_BN_CLICKED(ID_SHOW_AP_FILLS,        OnTbBnClickedShowApertureFill)
   ON_MESSAGE(ID_GRIDROWEDITFINISHED, OnRowEditFinished)
   ON_BN_CLICKED(IDC_BTN_GENOUTLINES, OnBnClickedBtnGenOutlines)
   ON_EN_UPDATE(IDC_EDIT_A1, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_A2, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_A3, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_A4, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_B1, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_B2, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_B3, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_B4, OnOutlineTemplateValueChanged)
   ON_EN_CHANGE(IDC_EDIT_B5, OnOutlineTemplateValueChanged)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/******************************************************************************
*/

CCamCadDatabase& CDataDoctorPackagesPage::getCamCadDatabase() 
{ 
   return getParent().getCamCadDatabase(); 
}

/******************************************************************************
*/

CDataDoctorPackages& CDataDoctorPackagesPage::getPackages()
{ 
   return this->getParent().getDataDoctor().getPackages(); 
}

/******************************************************************************
*/

BOOL CDataDoctorPackagesPage::OnInitDialog()
{
   m_toolbar.createAndLoad(IDR_ColorsLiteToolbar);
   CDataDoctorPropertyPageType::OnInitDialog();

   m_pkgGeomGrid.AttachGrid(this, IDC_PackagesGridStatic);
   m_pkgPartGrid.AttachGrid(this, IDC_PackagesGridStatic2);

   m_outlineEditGrid.AttachGrid(this, IDC_EditGridStatic);

   m_pkgPartGrid.ShowWindow(SW_HIDE);

   //*rcf Could do some more work on colors to simplify this setup
   m_pkgGeomGrid.SetBackgroundColors(m_pkgGeomGrid.m_redBackgroundColor, m_pkgGeomGrid.m_greenBackgroundColor, m_pkgGeomGrid.m_lightGrayBackgroundColor); //*rcf  make this part of base grid ?
   m_pkgPartGrid.SetBackgroundColors(m_pkgPartGrid.m_redBackgroundColor, m_pkgPartGrid.m_greenBackgroundColor, m_pkgPartGrid.m_lightGrayBackgroundColor);

   FillGrid();
   ResetRPOutlines();  //*rcf Is this a good place?
   EnableGridView();
   getParent().getDataDoctor().getPackages().PurgeOldStyleDFTPackageOutlines();

   getParent().EnableDisableDatabaseControls();
   
   // Option menu for outline method in Group assign operation. The order here, i.e.
   // association of choice name with choice number does not matter. We operate later
   // on the name, not the choice number.
   // We do NOT support the "Custom" outline method for group outline generation.
	m_cboDefault.InsertString(0, QNone); // this choice removes package outline
   m_cboDefault.InsertString(1, outlineAlgTagToString( algBodyOutline ));
	m_cboDefault.InsertString(2, outlineAlgTagToString( algPinCenters ));
	m_cboDefault.InsertString(3, outlineAlgTagToString( algPinExtents ));
	m_cboDefault.InsertString(4, outlineAlgTagToString( algInsidePads ));
   
	m_cboDefault.SetCurSel( 1 );

   m_cboTemplate.InsertString(0, TEMPLATE_RECTANGULAR);
   m_cboTemplate.InsertString(1, TEMPLATE_CIRCULAR);
   m_cboTemplate.InsertString(2, TEMPLATE_QFP);
   m_cboTemplate.InsertString(3, TEMPLATE_SO);
   m_cboTemplate.InsertString(4, TEMPLATE_SOT23);
   m_cboTemplate.SetCurSel(0);

   EnableCustomOutlineEditing(false);

   m_templateEditA1.SubclassDlgItem(IDC_EDIT_A1, this);
   m_templateEditA2.SubclassDlgItem(IDC_EDIT_A2, this);
   m_templateEditA3.SubclassDlgItem(IDC_EDIT_A3, this);
   m_templateEditA4.SubclassDlgItem(IDC_EDIT_A4, this);
   m_templateEditB1.SubclassDlgItem(IDC_EDIT_B1, this);
   m_templateEditB2.SubclassDlgItem(IDC_EDIT_B2, this);
   m_templateEditB3.SubclassDlgItem(IDC_EDIT_B3, this);
   m_templateEditB4.SubclassDlgItem(IDC_EDIT_B4, this);
   m_templateEditB5.SubclassDlgItem(IDC_EDIT_B5, this);

   m_editOffsetX.SubclassDlgItem(IDC_EDIT_OFFSETX, this);
   m_editOffsetY.SubclassDlgItem(IDC_EDIT_OFFSETY, this);
   m_editOffsetX.SetNumberType(ddFloat);
   m_editOffsetY.SetNumberType(ddFloat);

   m_dialogIsInitialized = true;

   return TRUE;  // return TRUE unless you set the focus to a control
}

//***************************************************************************************

void CDataDoctorPackagesPage::SaveStickySettings()
{
   // Save setting in registry, return true if is appears to work, otherwise false
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      settings.setValue("Grid View", this->GetGridMode() == PkgGridViewGeometries ? QGeometry : QPartNumber);
      settings.setValue("Fill Outlines", this->GetOutlineFillMode() ? "Yes" : "No");
      settings.setValue("HideSinglePinComponents", this->m_chkHideSinglePinComps ? "Yes" : "No");
   }

   //return false;   
}

//***************************************************************************************

void CDataDoctorPackagesPage::LoadStickySettings()
{
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey( GetDialogProfileEntry() );

   if (settings.isOpen())
   {
      CString val;

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

      if (settings.getValue("HideSinglePinComponents", val))
      {
         if (val.CompareNoCase("No") == 0)
            m_chkHideSinglePinComps = 0;
         else
            m_chkHideSinglePinComps = 1;
      }
   }

}

/******************************************************************************
*/
void CDataDoctorPackagesPage::OnBnClickedGridViewMode()
{
   UpdateData(TRUE);

   EnableGridView();

   SaveStickySettings();
}

/******************************************************************************
*/
void CDataDoctorPackagesPage::OnBnClickedChkHideSinglePinComps()
{
   UpdateData(TRUE);  
   FillGrid();
   SaveStickySettings();
}

/******************************************************************************
*/
void CDataDoctorPackagesPage::OnBnClickedFillOutlines()
{
   UpdateData(TRUE);

   SaveStickySettings();

   CCamCadDatabase& camCadDatabase = getParent().getDataDoctor().getCamCadDatabase();

   POSITION pos = this->getPackages().GetStartPosition();
   while (pos != NULL)
   {
      CString name;
      CDataDoctorPackage *ddpkg;
      this->getPackages().GetNextAssoc(pos, name, ddpkg);
      if (ddpkg != NULL)
      {
         ddpkg->GetRealPart().SetOutlineFill(camCadDatabase, this->GetOutlineFillMode());
      }
   }


   CDataDoctorParts& parts         = getParent().getDataDoctor().getParts();
   
   CDataDoctorPart* part;
   int rowIndx = 0;
   int index;

   for (parts.rewind(index);parts.next(part,index);)
   {
      if (part->isLoaded(camCadDatabase))
      {
         part->GetRealPart().SetOutlineFill(camCadDatabase, this->GetOutlineFillMode());
      }
   }


   m_doc->OnRedraw(); // To refresh display with new outline
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::EnableGridView()
{
   if (GetGridMode() == PkgGridViewGeometries)
   {
      this->m_pkgPartGrid.ShowWindow(SW_HIDE);
      if (m_updateOtherGrid)
         this->FillGeomGrid();
      this->m_pkgGeomGrid.ShowWindow(SW_SHOW);
      m_updateOtherGrid = false;
   }
   else
   {
      this->m_pkgGeomGrid.ShowWindow(SW_HIDE);
      if (m_updateOtherGrid)
         this->FillPartGrid();
      this->m_pkgPartGrid.ShowWindow(SW_SHOW);
      m_updateOtherGrid = false;
   }
}

/******************************************************************************
*/
void CDataDoctorPackagesPage::OnBnClickedBtnGenOutlines()
{
   UpdateData();
   this->m_pkgGeomGrid.ClearSelections();

   CString outlineMethod;
   m_cboDefault.GetWindowText(outlineMethod);

   // Convert "None" to blank
   if (outlineMethod.CompareNoCase(QNone) == 0)
      outlineMethod.Empty();

   bool overwrite = (m_radioApplyAll == 0);
   bool outlineFill = GetOutlineFillMode();

   if (GetGridMode() == PkgGridViewGeometries)
   {
      for (int gridrowi = 0; gridrowi < this->m_pkgGeomGrid.GetNumberRows(); gridrowi++)
      {
         CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(gridrowi);
         if (overwrite || !ddpkg->GetRealPart().HasOutlineData(this->getCamCadDatabase())) // overwrite or has no outline
         {
            this->m_pkgGeomGrid.QuickSetText( m_colOutlineMethod_GeomGrid, gridrowi, outlineMethod );
            ddpkg->SetOutlineMethod(outlineMethod, outlineFill, true);  // blank method will remove existing outline, this is what we want
            this->m_pkgGeomGrid.UpdateStatus(gridrowi);
         }
      }
      this->m_pkgGeomGrid.ResizeAll();
   }
   else
   {
      // Part Number
      for (int gridrowi = 0; gridrowi < this->m_pkgPartGrid.GetNumberRows(); gridrowi++)
      {
         CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(gridrowi);
         if (overwrite || !ddpart->GetRealPart().HasOutlineData(this->getCamCadDatabase())) // overwrite or has no outline 
         {
            this->m_pkgPartGrid.QuickSetText( m_colOutlineMethod_PartGrid, gridrowi, outlineMethod );
            ddpart->setOutlineMethod(this->getCamCadDatabase(), outlineMethod, outlineFill, true);  // blank method will remove existing outline, this is what we want
            this->m_pkgPartGrid.UpdateStatus(gridrowi);
         }
      }
      this->m_pkgPartGrid.ResizeAll();
   }

   
   // If doc->GeometryEditing is true then some row has been selected and the geomtry for
   // that row is currently displayed, call DoEditGeom to update it, this will also update
   // enable/disable for Custom Outline Editing controls. If not in editing mode then
   // just make sure custom controls are off/disabled.
   if (m_doc->GeometryEditing)
      this->DoEditGeom();
   else
      EnableCustomOutlineEditing(false);
   
   // The following would un-zoom if there is a zoomed package (from a row select). But this could be
   // annoyingly lengthy, if pcb is big, so maybe it is better to just leave the leftover zoom and
   // just amke sure what is visible is updated?
   //getCamCadDatabase().getCamCadDoc().OnDoneEditing();
   m_doc->UpdateAllViews(NULL);
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::ResetRPOutlines()
{
   // Per partnumber
   for (int gridrowi = 0; gridrowi < this->m_pkgPartGrid.GetNumberRows(); gridrowi++)
   {
      CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(gridrowi);
      ddpart->CreateOutlineInserts(this->getCamCadDatabase(), this->getParent().getDataDoctor().getPcbBlock());    
   }

   // Per geometry
   for (int gridrowi = 0; gridrowi < this->m_pkgGeomGrid.GetNumberRows(); gridrowi++)
   {
      CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(gridrowi);
      ddpkg->CreateOutlineInserts(this->getCamCadDatabase(), this->getParent().getDataDoctor().getPcbBlock());    
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::AddFieldControls()
{
#if defined (DataDoctorResizingDialogFlag)
   addFieldControl(IDC_PackagesGridStatic,anchorLeft,growBoth);     // One for Geom Name grid
   addFieldControl(IDC_PackagesGridStatic2,anchorLeft,growBoth);    // One for PN grid

   addFieldControl(IDC_CHK_VIEW_PARTNUM            ,anchorBottomLeft);
   addFieldControl(IDC_CHK_FILL_OUTLINES           ,anchorBottomLeft);
   addFieldControl(IDC_CHK_HIDE_SINGLE_PIN_COMPS   ,anchorBottomLeft);

   
   //CResizingDialogField& geometryDiagramField = addFieldControl(IDC_GeometryDiagram, anchorBottomLeft);
   //geometryDiagramField.getOperations().addOperation(glueBottomRightCorner   ,toTopRightCorner,&apertureDiagramField);

   addFieldControl(IDC_GRP_CUSTOM           ,anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_EditGridStatic	     ,anchorBottomLeft, growHorizontal);

   addFieldControl(IDC_GRP_TEMPLATE             ,anchorBottomRight);
   addFieldControl(IDC_COMBO_TEMPLATE           ,anchorBottomRight);
   addFieldControl(IDC_LABEL_A1                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_A2                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_A3                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_A4                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_B1                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_B2                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_B3                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_B4                 ,anchorBottomRight);
   addFieldControl(IDC_LABEL_B5                 ,anchorBottomRight);
   addFieldControl(IDC_EDIT_A1                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_A2                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_A3                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_A4                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_B1                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_B2                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_B3                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_B4                  ,anchorBottomRight);
   addFieldControl(IDC_EDIT_B5                  ,anchorBottomRight);
   addFieldControl(IDC_CHK_CHAMFER_ROUND        ,anchorBottomRight);
   addFieldControl(IDC_BTN_CREATE_TEMPLATE_OUTLINE  ,anchorBottomRight);

   addFieldControl(IDC_GRP_OUTLINE_OFFSETXY     ,anchorBottomRight);
   addFieldControl(IDC_EDIT_OFFSETX             ,anchorBottomRight);
   addFieldControl(IDC_EDIT_OFFSETY             ,anchorBottomRight);
   addFieldControl(IDC_BTN_APPLY_OFFSET         ,anchorBottomRight);

   addFieldControl(IDC_GRP_OUTLINE_ROTATE       ,anchorBottomRight);
   addFieldControl(IDC_BTN_ROTATE90             ,anchorBottomRight);

   addFieldControl(IDC_CreateGroup            ,anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_BTN_GENOUTLINES        ,anchorBottomLeft);

   addFieldControl(IDC_COMBO_DEF              ,anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_RADIO_APPLYALL         ,anchorBottomRight);
   addFieldControl(IDC_RADIO_APPLYNONE        ,anchorBottomRight);

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
void CDataDoctorPackagesPage::EnableDatabaseControls(bool flag)
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

void CDataDoctorPackagesPage::OnBnClickedLoadFromLibrary()
{
   bool returnValue = false;
   UpdateData(true);

   CDataDoctorLibraryAttribMap& activeAttribMap = getParent().getDataDoctor().GetPackageLibraryAttribMap();

   if (GetGridMode() == PkgGridViewGeometries)
   {
      CDataDoctorPackages& ddpkgs  = getParent().getDataDoctor().getPackages();    
      returnValue = ddpkgs.loadPackageDataFromLibrary(getCamCadDatabase(), GetOutlineFillMode(), activeAttribMap, m_overwriteValues != 0, true);
   }
   else
   {
      CDataDoctorParts& ddparts  = getParent().getDataDoctor().getParts();
      returnValue = ddparts.loadPackageDataFromLibrary(getCamCadDatabase(), GetOutlineFillMode(), activeAttribMap, m_overwriteValues != 0, true);
   }

   Update();
   getParent().updatePropertyPages(this);

   if (!returnValue)
      formatMessageBox(MB_ICONASTERISK,"Load All Packages from Library did not find any matching Packages");

}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedStoreToLibrary()
{
   UpdateData(true);

   if (GetGridMode() == PkgGridViewGeometries)
   {
      CDataDoctorPackages& ddpkgs = getParent().getDataDoctor().getPackages();
      ddpkgs.storePackageDataToLibrary(getCamCadDatabase(),(m_storeOption==0)?true:false);
   }
   else
   {
      CDataDoctorParts& ddparts  = getParent().getDataDoctor().getParts();
      ddparts.storePackageDataToLibrary(getCamCadDatabase(), (m_storeOption==0)?true:false);
   }
}

/******************************************************************************
*/

CDataDoctorComponent* CDataDoctorPackagesPage::GetActiveSampleComponent()
{
   CDataDoctorComponent* comp = NULL;

   if (GetGridMode() == PkgGridViewGeometries)
   {
      long row = m_pkgGeomGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
      {
         CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(row);
         if (ddpkg != NULL)
         {
            comp = ddpkg->GetExampleComponent();
         }
      }
   }
   else
   {
      long row = m_pkgPartGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
      {
         CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(row);
         if (ddpart != NULL)
         {
            comp = ddpart->getHeadComponent();
         }
      }
   }

   return comp;
}

/******************************************************************************
*/

CDataDoctorRealPart* CDataDoctorPackagesPage::GetActiveRealPart()
{
   CDataDoctorRealPart* realpart = NULL;

   if (GetGridMode() == PkgGridViewGeometries)
   {
      long row = m_pkgGeomGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
      {
         CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(row);
         if (ddpkg != NULL)
         {
            realpart = &ddpkg->GetRealPart();
         }
      }
   }
   else
   {
      long row = m_pkgPartGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
      {
         CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(row);
         if (ddpart != NULL)
         {
            realpart = &ddpart->GetRealPart();
         }
      }
   }

   return realpart;
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnCboSelchangeTemplate()
{
   UpdateData();
   CString templateName;
   m_cboTemplate.GetLBText(m_cboTemplate.GetCurSel(), templateName);

   // Enable/disable appropriate controls, set up labels for template values
   //*rcf

   // Go by template name, not selection number, so position in menu does not matter

   if (templateName.CompareNoCase(TEMPLATE_RECTANGULAR) == 0)
   {
      SetTemplate("Length (X)", "Width (Y)", "Chamfer", true);
   }
   else if (templateName.CompareNoCase(TEMPLATE_CIRCULAR) == 0)
   {
      SetTemplate("Diameter", "", "", false);
   }
   else if (templateName.CompareNoCase(TEMPLATE_QFP) == 0)
   {
      SetTemplate("Body Length", "Body Width", "Pin Pitch", "Pin Width", "Pin Count L", "Pin Count W", "Pin Toe", "Overall Length", "Overall Width");
   }
   else if (templateName.CompareNoCase(TEMPLATE_SO) == 0)
   {
      SetTemplate("Body Length", "Body Width", "Pin Pitch", "Pin Width", "Pin Toe", "Pin Count", "Overall Width", "", "");
   }
   else if (templateName.CompareNoCase(TEMPLATE_SOT23) == 0)
   {
      SetTemplate("Body Length", "Body Width", "Pin Pitch", "Pin Width", "Pin Toe", "Overall Width", "", "", "");
   }
   else
   {
      // ?
      SetTemplate("", "", "", false);
   }
}

//-----------------------------------------------------------------------------

void CDataDoctorPackagesPage::SetTemplate(CString labelA1, CString labelA2, CString labelA3, bool showRoundCheckbox)
{
   // In GUI, columns are A, B, C ..., rows are 1, 2, 3, ...

   SetTemplate(labelA1, labelA2, labelA3, "", "", "", "", "", "");

   GetDlgItem(IDC_CHK_CHAMFER_ROUND)->EnableWindow( showRoundCheckbox);
   GetDlgItem(IDC_CHK_CHAMFER_ROUND)->ShowWindow(   showRoundCheckbox?SW_SHOW:SW_HIDE);
}

void CDataDoctorPackagesPage::SetTemplate(CString a1, CString a2, CString a3, CString a4, CString b1, CString b2, CString b3, CString b4, CString b5)
{
   GetDlgItem(IDC_CHK_CHAMFER_ROUND)->EnableWindow( false );
   GetDlgItem(IDC_CHK_CHAMFER_ROUND)->ShowWindow( SW_HIDE );

   GetDlgItem(IDC_LABEL_A1)->SetWindowText(a1);               // Set the edit box label
   GetDlgItem(IDC_LABEL_A1)->EnableWindow(!a1.IsEmpty());     // Gray out if label is blank (field not used for this template)
   GetDlgItem(IDC_EDIT_A1)->SetWindowText("");                // Clear any previous entry (mainly because number type might change)
   GetDlgItem(IDC_EDIT_A1)->EnableWindow( !a1.IsEmpty());     // Gray out is label is blank (field not used for this template)
   this->m_templateEditA1.SetNumberType(a1.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);   // Set up for appropriate keystroke filter

   GetDlgItem(IDC_LABEL_A2)->SetWindowText(a2);
   GetDlgItem(IDC_LABEL_A2)->EnableWindow(!a2.IsEmpty());
   GetDlgItem(IDC_EDIT_A2)->SetWindowText("");
   GetDlgItem(IDC_EDIT_A2)->EnableWindow( !a2.IsEmpty());
   this->m_templateEditA2.SetNumberType(a2.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

   GetDlgItem(IDC_LABEL_A3)->SetWindowText(a3);
   GetDlgItem(IDC_LABEL_A3)->EnableWindow(!a3.IsEmpty());
   GetDlgItem(IDC_EDIT_A3)->SetWindowText("");
   GetDlgItem(IDC_EDIT_A3)->EnableWindow( !a3.IsEmpty());
   this->m_templateEditA3.SetNumberType(a3.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

   GetDlgItem(IDC_LABEL_A4)->SetWindowText(a4);
   GetDlgItem(IDC_LABEL_A4)->EnableWindow(!a4.IsEmpty());
   GetDlgItem(IDC_EDIT_A4)->SetWindowText("");
   GetDlgItem(IDC_EDIT_A4)->EnableWindow( !a4.IsEmpty());
   this->m_templateEditA4.SetNumberType(a4.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

   GetDlgItem(IDC_LABEL_B1)->SetWindowText(b1);
   GetDlgItem(IDC_LABEL_B1)->EnableWindow(!b1.IsEmpty());
   GetDlgItem(IDC_EDIT_B1)->SetWindowText("");
   GetDlgItem(IDC_EDIT_B1)->EnableWindow( !b1.IsEmpty());
   this->m_templateEditB1.SetNumberType(b1.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

   GetDlgItem(IDC_LABEL_B2)->SetWindowText(b2);
   GetDlgItem(IDC_LABEL_B2)->EnableWindow(!b2.IsEmpty());
   GetDlgItem(IDC_EDIT_B2)->SetWindowText("");
   GetDlgItem(IDC_EDIT_B2)->EnableWindow( !b2.IsEmpty());
   this->m_templateEditB2.SetNumberType(b2.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

   GetDlgItem(IDC_LABEL_B3)->SetWindowText(b3);
   GetDlgItem(IDC_LABEL_B3)->EnableWindow(!b3.IsEmpty());
   GetDlgItem(IDC_EDIT_B3)->SetWindowText("");
   GetDlgItem(IDC_EDIT_B3)->EnableWindow( !b3.IsEmpty());
   this->m_templateEditB3.SetNumberType(b3.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

   GetDlgItem(IDC_LABEL_B4)->SetWindowText(b4);
   GetDlgItem(IDC_LABEL_B4)->EnableWindow(!b4.IsEmpty());
   GetDlgItem(IDC_EDIT_B4)->SetWindowText("");
   GetDlgItem(IDC_EDIT_B4)->EnableWindow( !b4.IsEmpty());
   this->m_templateEditB4.SetNumberType(b4.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

   GetDlgItem(IDC_LABEL_B5)->SetWindowText(b5);
   GetDlgItem(IDC_LABEL_B5)->EnableWindow(!b5.IsEmpty());
   GetDlgItem(IDC_EDIT_B5)->SetWindowText("");
   GetDlgItem(IDC_EDIT_B5)->EnableWindow( !b5.IsEmpty());
   this->m_templateEditB5.SetNumberType(b5.Find("Count") > -1 ? ddUnsignedInteger : ddUnsignedFloat);

}

/******************************************************************************
*/

bool CDataDoctorPackagesPage::ValidateTemplateValue(int controlID, bool integer)
{
   // The "enable" value has to do with whether or not the CREATE button
   // should be enabled or disabled based on this value. It is not
   // necessarilly an indicator that this control contains a valid value.
   // E.g. is the control is not enabled then the control's value does not
   // matter, so it is okay to enable the CREATE button based on this
   // particular control's state. The control's value is not valid so much
   // as it does not matter. If a control is enabled then it must contain
   // a positive number.

   // Separate tests for integer versus float

   bool enable = true;

   if (integer)
   {
      if (GetDlgItem(controlID)->IsWindowEnabled() && GetInteger(controlID) <= 0)
         enable = false;
   }
   else
   {
      if (GetDlgItem(controlID)->IsWindowEnabled() && GetDouble(controlID) <= 0.)
         enable = false;
   }

   return enable;
}

/******************************************************************************
*/
void CDataDoctorPackagesPage::OnOutlineTemplateValueChanged()
{
   // Enable/disable "create" button based on validity of collection of params
   bool enable = true;

   if (!ValidateTemplateValue(IDC_EDIT_A1))
      enable = false;

   if (!ValidateTemplateValue(IDC_EDIT_A2))
      enable = false;

   CString str;
   GetDlgItem(IDC_LABEL_A3)->GetWindowText(str);
   bool isChamfer = str.CompareNoCase("Chamfer") == 0;
   if (!isChamfer &&
      !ValidateTemplateValue(IDC_EDIT_A3))
      enable = false;

   if (!ValidateTemplateValue(IDC_EDIT_A4))
      enable = false;

   if (!ValidateTemplateValue(IDC_EDIT_B1))
      enable = false;

   if (!ValidateTemplateValue(IDC_EDIT_B2))
      enable = false;

   if (!ValidateTemplateValue(IDC_EDIT_B3))
      enable = false;

   if (!ValidateTemplateValue(IDC_EDIT_B4))
      enable = false;

   if (!ValidateTemplateValue(IDC_EDIT_B5))
      enable = false;
      
   GetDlgItem(IDC_BTN_CREATE_TEMPLATE_OUTLINE)->EnableWindow(enable);

}

/******************************************************************************
*/
double CDataDoctorPackagesPage::GetDouble(int dlgItem)
{
   CString str;
   GetDlgItem(dlgItem)->GetWindowText(str);
   double d = atof(str);
   return d;
}

/******************************************************************************
*/
int CDataDoctorPackagesPage::GetInteger(int dlgItem)
{
   CString str;
   GetDlgItem(dlgItem)->GetWindowText(str);
   int n = atoi(str);
   return n;
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedCreateCustomTemplateOutline()
{
   UpdateData(true);

   double a1 = GetDouble(IDC_EDIT_A1);
   double a2 = GetDouble(IDC_EDIT_A2);
   double a3 = GetDouble(IDC_EDIT_A3);
   double a4 = GetDouble(IDC_EDIT_A4);
   double b1 = GetDouble(IDC_EDIT_B1);
   double b2 = GetDouble(IDC_EDIT_B2);
   double b3 = GetDouble(IDC_EDIT_B3);
   double b4 = GetDouble(IDC_EDIT_B4);
   double b5 = GetDouble(IDC_EDIT_B5);

   bool roundChamfer = this->m_outlineChamferRound?true:false;

   CString templateName;
   m_cboTemplate.GetLBText(m_cboTemplate.GetCurSel(), templateName);

   // Go by template name, not selection number, so position in menu does not matter

   if (templateName.CompareNoCase(TEMPLATE_RECTANGULAR) == 0)
   {
      CreateCustomRectangularOutline(a1, a2, a3, roundChamfer);
   }
   else if (templateName.CompareNoCase(TEMPLATE_CIRCULAR) == 0)
   {
      CreateCustomCircularOutline(a1);
   }
   else if (templateName.CompareNoCase(TEMPLATE_QFP) == 0)
   {
      //SetTemplate("Body Length", "Body Width", "Pin Pitch", "Pin Width", "Pin Count L", "Pin Count W", "Pin Toe", "Overall Length", "Overall Width");
      CreateCustomQFPOutline(a1, a2, a3, a4, (int)b1, (int)b2, b3, b4, b5);
   }
   else if (templateName.CompareNoCase(TEMPLATE_SO) == 0)
   {
      //                 a1              a2          a3            a4          b1         b2            b3
      // SetTemplate("Body Length", "Body Width", "Pin Pitch", "Pin Width", "Pin Toe", "Pin Count", "Overall Width", "", "");
      CreateCustomSOICOutline(a1, a2, a3, a4, (int)b2, b1, b3);
   }
   else if (templateName.CompareNoCase(TEMPLATE_SOT23) == 0)
   {
      //                  a1            a2           a3           a4           b1          b2
      // SetTemplate("Body Length", "Body Width", "Pin Pitch", "Pin Width", "Pin Toe", "Overall Width", "", "", "");
      CreateCustomSOT23Outline(a1, a2, a3, a4, b1, b2);
   }
   else
   {
      CString msg;
      msg.Format("Outline creation for template %s not implemented.", templateName);
      ErrorMessage(msg, "Custom Outline Creation");
   }

   m_outlineEditGrid.Fill(this);
   UpdateStatusActiveRow();

}

/******************************************************************************
*/

void CDataDoctorPackagesPage::UpdateStatusActiveRow()
{
   // Figure out which grid is in use, and which row is selected, then update status in that row

   if (GetGridMode() == PkgGridViewGeometries)
   {
      long row = m_pkgGeomGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
      {
         m_pkgGeomGrid.UpdateStatus(row);
         m_pkgGeomGrid.RedrawRow(row);
      }
   }
   else
   {
      long row = m_pkgPartGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
      {
         m_pkgPartGrid.UpdateStatus(row);
         m_pkgPartGrid.RedrawRow(row);
      }
   }
}

/******************************************************************************
*/

CDataDoctorRealPart *CDataDoctorPackagesPage::BeginOutline(CPoint2d &origin)
{
   // Defines the outline, opens the block, adds the poly, so caller can just start adding vertices

   CDataDoctorRealPart *realpart = GetActiveRealPart();
   origin.x = 0.;
   origin.y = 0.;

   if (realpart != NULL && realpart->GetBlock() != NULL)
   {
      //BlockStruct *rpblk = realpart->GetBlock();
      BlockStruct *rppkgblk = realpart->GetPkgBlock(this->getCamCadDatabase());
      if (rppkgblk != NULL)
      {
         Graph_Block_On(GBO_OVERWRITE, rppkgblk->getName(), rppkgblk->getFileNumber(), 0);

         int packageOutlineTopLayerIndex = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
         int widthIndex = this->getCamCadDatabase().getZeroWidthIndex(); //Graph_Aperture("", T_ROUND, penWidth, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);

         DataStruct *outlineData = Graph_PolyStruct(packageOutlineTopLayerIndex, 0, FALSE);
         outlineData->setGraphicClass(graphicClassPackageBody);
         int keyword = m_doc->RegisterKeyWord(ATT_OUTLINE_METHOD, 0, valueTypeString);
         m_doc->SetAttrib(&outlineData->getDefinedAttributes(), keyword, valueTypeString, outlineAlgTagToString(algCustom).GetBuffer(0), attributeUpdateOverwrite, NULL);
         Graph_Poly(NULL, widthIndex, GetOutlineFillMode(), 0, TRUE /*closed*/);

         // Determine origin
         CDataDoctorComponent *sampleComp = this->GetActiveSampleComponent();
         if (sampleComp != NULL) 
         {
            origin = sampleComp->GetPinsCentroid( getCamCadDatabase() );
         }

         return realpart;
      }
   }

   return NULL;
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::EndOutline(CDataDoctorRealPart *realpart)
{
   // Close active outline def and align the outline

   if (realpart != NULL)
   {
      Graph_Block_Off();

      // Align
      CDataDoctorComponent *sampleComp = this->GetActiveSampleComponent();
      if (sampleComp != NULL) 
      {
         CPoint2d pinsCenter = sampleComp->GetPinsCentroid( getCamCadDatabase() );
         DataStruct *pkgInsertData = realpart->GetBlock()->GetPackage();

         if (pkgInsertData != NULL && pkgInsertData->getDataType() == dataTypeInsert)
         {
#define ALIGN_BY_PIN_CENTER
#ifdef  ALIGN_BY_PIN_CENTER
            pkgInsertData->getInsert()->setOrigin(pinsCenter);
#else
            // Simply align same as comp
            pkgInsertData->getInsert()->setOrigin(0.,0.);// sampleComp->getOrigin() );
#endif
         }

      }
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::CreateCustomQFPOutline(double bodylen, double bodywid, double pinpitch, double pinwid, int pincountL, int pincountW, double pintoe, double overalllen, double overallwid)
{
   // QFP template pattern from Mark Laing

   if (bodylen <= 0. || bodywid <= 0. || pinpitch <= 0. || pinwid <= 0. || pincountL <= 0. || pincountW <= 0. || pintoe <= 0. || overalllen <= 0. || overallwid <= 0.)
   {
      ErrorMessage("All parameters must be greater than zero.", "");
      return;
   }

   CPoint2d origin;
   CDataDoctorRealPart *realpart = BeginOutline(origin);

   if (realpart != NULL) // && realpart->GetBlock() != NULL)
   {
      // $x = $bl/-2;
      // $y = $bw/2;
      Graph_Vertex( bodylen/-2.0, bodywid/2.0, 0);

      // $x = (($pcl/-2)+0.5)*$pp-($pw/2);
      // $y = $bw/2;
      Graph_Vertex( (((pincountL/-2.)+0.5)*pinpitch)-(pinwid/2.), bodywid/2., 0);

      // $x = (($pcl/-2)+0.5)*$pp-($pw/2);
      // $y = ($ow/2)-($pt/2);
      Graph_Vertex( (((pincountL/-2.)+0.5)*pinpitch)-(pinwid/2.), (overallwid/2.)-(pintoe/2.), 0);

      // $x = (($pcl/2)-0.5)*$pp+($pw/2);
      // $y = ($ow/2)-($pt/2);
      Graph_Vertex( (((pincountL/2.)-0.5)*pinpitch)+(pinwid/2.), (overallwid/2.)-(pintoe/2.), 0);

      // $x = (($pcl/2)-0.5)*$pp+($pw/2);
      // $y = $bw/2;
      Graph_Vertex( (((pincountL/2.)-0.5)*pinpitch)+(pinwid/2.), bodywid/2., 0);

      // $x = $bl/2;
      // $y = $bw/2;
      Graph_Vertex( bodylen/2., bodywid/2., 0);

      // $x = $bl/2;
      // $y = (($pcw/2)-0.5)*$pp+($pw/2);
      Graph_Vertex( bodylen/2., (((pincountW/2.)-0.5)*pinpitch)+(pinwid/2.), 0);

      // $x = ($ol/2)-($pt/2);
      // $y = (($pcw/2)-0.5)*$pp+($pw/2);
      Graph_Vertex( (overalllen/2.)-(pintoe/2.), (((pincountW/2.)-0.5)*pinpitch)+(pinwid/2.), 0);

      // $x = ($ol/2)-($pt/2);
      // $y = (($pcw/-2)+0.5)*$pp-($pw/2);
      Graph_Vertex( (overalllen/2.)-(pintoe/2.), (((pincountW/-2.)+0.5)*pinpitch)-(pinwid/2.), 0);

      // $x = $bl/2;
      // $y = (($pcw/-2)+0.5)*$pp-($pw/2);
      Graph_Vertex( (bodylen/2.), (((pincountW/-2.)+0.5)*pinpitch)-(pinwid/2.), 0);

      // $x = $bl/2;
      // $y = $bw/-2;
      Graph_Vertex( bodylen/2., bodywid/-2., 0);

      // $x = (($pcl/2)-0.5)*$pp+($pw/2);
      // $y = $bw/-2;
      Graph_Vertex( (((pincountL/2.)-0.5)*pinpitch)+(pinwid/2.),(bodywid/-2.), 0);

      // $x = (($pcl/2)-0.5)*$pp+($pw/2);
      // $y = ($ow/-2)+($pt/2);
      Graph_Vertex( (((pincountL/2.)-0.5)*pinpitch)+(pinwid/2.), (overallwid/-2.)+(pintoe/2.), 0);

      // $x = (($pcl/-2)+0.5)*$pp-($pw/2);
      // $y = ($ow/-2)+($pt/2);
      Graph_Vertex( (((pincountL/-2.)+0.5)*pinpitch)-(pinwid/2.), (overallwid/-2.)+(pintoe/2.), 0);

      // $x = (($pcl/-2)+0.5)*$pp-($pw/2);
      // $y = $bw/-2;
      Graph_Vertex( (((pincountL/-2.)+0.5)*pinpitch)-(pinwid/2.), (bodywid/-2.), 0);

      // $x = $bl/-2;
      // $y = $bw/-2;
      Graph_Vertex( (bodylen/-2.), (bodywid/-2.), 0);

      // $x = $bl/-2;
      // $y = (($pcw/-2)+0.5)*$pp-($pw/2);
      Graph_Vertex( (bodylen/-2.), (((pincountW/-2.)+0.5)*pinpitch)-(pinwid/2.), 0);

      // $x = ($ol/-2)+($pt/2);
      // $y = (($pcw/-2)+0.5)*$pp-($pw/2);
      Graph_Vertex( (overalllen/-2.)+(pintoe/2.), (((pincountW/-2.)+0.5)*pinpitch)-(pinwid/2.), 0);

      // $x = ($ol/-2)+($pt/2);
      // $y = (($pcw/2)-0.5)*$pp+($pw/2);
      Graph_Vertex( (overalllen/-2.)+(pintoe/2.), (((pincountW/2.)-0.5)*pinpitch)+(pinwid/2.), 0);

      // $x = $bl/-2;
      // $y = (($pcw/2)-0.5)*$pp+($pw/2);
      Graph_Vertex( (bodylen/-2.), (((pincountW/2.)-0.5)*pinpitch)+(pinwid/2.), 0);

      // $x = $bl/-2;
      // $y = $bw/2;
      Graph_Vertex( (bodylen/-2.), (bodywid/2.), 0);


      EndOutline(realpart);

      m_doc->OnRedraw();
   }
   else
   {
      ErrorMessage("No package currently selected", "");
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::CreateCustomSOICOutline(double bodylen, double bodywid, double pinpitch, double pinwid, int pincount, double pintoe, double overallwid)
{
   // SO template pattern from Mark Laing

   if (bodylen <= 0. || bodywid <= 0. || pinpitch <= 0. || pinwid <= 0. || pincount <= 0. || pintoe <= 0. || overallwid <= 0.)
   {
      ErrorMessage("All parameters must be greater than zero.", "");
      return;
   }

   CPoint2d origin;
   CDataDoctorRealPart *realpart = BeginOutline(origin);

   if (realpart != NULL) // && realpart->GetBlock() != NULL)
   {
      // $x = $bw/-2;
      // $y = $bl/2;
      double x0 = bodywid/-2.0;
      double y0 = bodylen/2.0;
      Graph_Vertex( x0, y0, 0);

      // $x = $bw/2;
      // $y = $bl/2;
      double x1 = bodywid/2.0;
      double y1 = y0;
      Graph_Vertex( x1, y1, 0);

      // $x = $bw/2;
      // $y = (($pc/4)-0.5)*$pp+($pw/2);
      double x2 = x1;
      double y2 = (((pincount/4.)-0.5)*pinpitch)+(pinwid/2.);
      Graph_Vertex( x2, y2, 0);

      // $x = ($ow/2)-$pt/2;
      // $y = (($pc/4)-0.5)*$pp+($pw/2);
      double x3 = (overallwid/2.)-(pintoe/2.);
      double y3 = y2;
      Graph_Vertex( x3, y3, 0);

      // $x = ($ow/2)-$pt/2;
      // $y = (($pc/-4)+0.5)*$pp-($pw/2);
      double x4 = x3;
      double y4 = (((pincount/-4.)+0.5)*pinpitch)-(pinwid/2.);
      Graph_Vertex( x4, y4, 0);

      // $x = $bw/2;
      // $y = (($pc/-4)+0.5)*$pp-($pw/2);
      double x5 = x1;
      double y5 = y4;
      Graph_Vertex( x5, y5, 0);

      // $x = $bw/2;
      // $y = $bl/-2;
      double x6 = x5;
      double y6 = bodylen/-2.;
      Graph_Vertex( x6, y6, 0);

      // $x = $bw/-2;
      // $y = $bl/-2;
      double x7 = x0;
      double y7 = y6;
      Graph_Vertex( x7, y7, 0);

      // $x = $bw/-2;
      // $y = (($pc/-4)+0.5)*$pp-($pw/2);
      double x8 = x7;
      double y8 = y4;
      Graph_Vertex( x8, y8, 0);

      // $x = ($ow/-2)+$pt/2;
      // $y = (($pc/-4)+0.5)*$pp-($pw/2);
      double x9 = (overallwid/-2.)+(pintoe/2.);
      double y9 = y8;
      Graph_Vertex( x9, y9, 0);

      // $x = ($ow/-2)+$pt/2;
      // $y = (($pc/4)-0.5)*$pp+($pw/2);
      double x10 = x9;
      double y10 = y3;
      Graph_Vertex( x10, y10, 0);

      // $x = $bw/-2;
      // $y = (($pc/4)-0.5)*$pp+($pw/2);
      double x11 = x0;
      double y11 = y10;
      Graph_Vertex( x11, y11, 0);

      // $x = $bw/-2;
      // $y = $bl/2;
      // Close - back to start
      Graph_Vertex( x0, y0, 0);


      EndOutline(realpart);

      m_doc->OnRedraw();
   }
   else
   {
      ErrorMessage("No package currently selected", "");
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::CreateCustomSOT23Outline(double bodylen, double bodywid, double pinpitch, double pinwid, double pintoe, double overallwid)
{
   // SOT23 template pattern from Mark Laing

   if (bodylen <= 0. || bodywid <= 0. || pinpitch <= 0. || pinwid <= 0. || pintoe <= 0. || overallwid <= 0.)
   {
      ErrorMessage("All parameters must be greater than zero.", "");
      return;
   }

   CPoint2d origin;
   CDataDoctorRealPart *realpart = BeginOutline(origin);

   if (realpart != NULL) // && realpart->GetBlock() != NULL)
   {
      // $x = $bl/-2;
      // $y = $bw/2;
      double x0 = bodylen/-2.;
      double y0 = bodywid/2.;
      Graph_Vertex( x0, y0, 0);

      // $x = $pw/-2;
      // $y = $bw/2;
      double x1 = pinwid/-2.;
      double y1 = y0;
      Graph_Vertex( x1, y1, 0);

      // $x = $pw/-2;
      // $y = ($ow/2)-($pt/2);
      double x2 = x1;
      double y2 = (overallwid/2.)-(pintoe/2.);
      Graph_Vertex( x2, y2, 0);

      // $x = $pw/2;
      // $y = ($ow/2)-($pt/2);
      double x3 = pinwid/2.;
      double y3 = y2;
      Graph_Vertex( x3, y3, 0);

      // $x = $pw/2;
      // $y = $bw/2;
      double x4 = x3;
      double y4 = y1;
      Graph_Vertex( x4, y4, 0);

      // $x = $bl/2;
      // $y = $bw/2;
      double x5 = bodylen/2.;
      double y5 = y4;
      Graph_Vertex( x5, y5, 0);

      // $x = $bl/2;
      // $y = $bw/-2;
      double x6 = x5;
      double y6 = -y5;
      Graph_Vertex( x6, y6, 0);

      // $x = ($pp/2)+($pw/2);
      // $y = $bw/-2;
      double x7 = (pinpitch/2.)+(pinwid/2.);
      double y7 = y6;
      Graph_Vertex( x7, y7, 0);

      // $x = ($pp/2)+($pw/2);
      // $y = ($ow/-2)+($pt/2);
      double x8 = x7;
      double y8 = -y3;
      Graph_Vertex( x8, y8, 0);

      // $x = ($pp/2)-($pw/2);
      // $y = ($ow/-2)+($pt/2);
      double x9 = (pinpitch/2.)-(pinwid/2.);
      double y9 = y8;
      Graph_Vertex( x9, y9, 0);

      // $x = ($pp/2)-($pw/2);
      // $y = $bw/-2;
      double x10 = x9;
      double y10 = y7;
      Graph_Vertex( x10, y10, 0);

      // $x = ($pp/-2)+($pw/2);
      // $y = $bw/-2;
      double x11 = -x10;
      double y11 = y10;
      Graph_Vertex( x11, y11, 0);

      // $x = ($pp/-2)+($pw/2);
      // $y = ($ow/-2)+($pt/2);
      double x12 = x11;
      double y12 = y8;
      Graph_Vertex( x12, y12, 0);

      // $x = ($pp/-2)-($pw/2);
      // $y = ($ow/-2)+($pt/2);
      double x13 = (pinpitch/-2.)-(pinwid/2.);
      double y13 = y12;
      Graph_Vertex( x13, y13, 0);

      // $x = ($pp/-2)-($pw/2);
      // $y = $bw/-2;
      double x14 = x13;
      double y14 = y11;
      Graph_Vertex( x14, y14, 0);

      // $x = $bl/-2;
      // $y = $bw/-2;
      double x15 = x0;
      double y15 = y14;
      Graph_Vertex( x15, y15, 0);

      // $x = $bl/-2;
      // $y = $bw/2;
      // Close, back to start
      Graph_Vertex( x0, y0, 0);


      EndOutline(realpart);

      m_doc->OnRedraw();
   }
   else
   {
      ErrorMessage("No package currently selected", "");
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::CreateCustomRectangularOutline(double length, double width, double chamfer, bool roundChamfer)
{
   if (length <= 0.0 || width <= 0.0)
   {
      ErrorMessage("Length and width must be greater than zero", "");
      return;
   }
   if (chamfer >= (length / 2.) || chamfer >= width / 2.)
   {
      ErrorMessage("Chamfer must be less than one half of the lessor of length or width, i.e. must be less than one half of shortest side.", "");
      return;
   }

   CPoint2d originX; //*rcf This origin stuff turned out to be bogus?
   CDataDoctorRealPart *realpart =  BeginOutline(originX);
   CPoint2d origin;

   if (realpart != NULL) // && realpart->GetBlock() != NULL)
   {
      double halfLength = length / 2.;
      double halfWidth = width / 2.;

      if (chamfer <= 0.0)
      {
         // Plain rectangle
         Graph_Vertex(-halfLength - origin.x, -halfWidth - origin.y, 0.0);
         Graph_Vertex(+halfLength - origin.x, -halfWidth - origin.y, 0.0);
         Graph_Vertex(+halfLength - origin.x, +halfWidth - origin.y, 0.0);
         Graph_Vertex(-halfLength - origin.x, +halfWidth - origin.y, 0.0);
         Graph_Vertex(-halfLength - origin.x, -halfWidth - origin.y, 0.0);
      }
      else
      {
         // Chamfered rectangle
         double chamferBulge = 0.0;
         if (roundChamfer)
            chamferBulge = 0.414; // 90 degree arc

         Graph_Vertex(-halfLength + chamfer, -halfWidth          , 0.0);          // LL bottom
         Graph_Vertex(+halfLength - chamfer, -halfWidth          , chamferBulge); // LR bottom
         Graph_Vertex(+halfLength          , -halfWidth + chamfer, 0.0);          // LR side
         Graph_Vertex(+halfLength          , +halfWidth - chamfer, chamferBulge); // UR side
         Graph_Vertex(+halfLength - chamfer, +halfWidth          , 0.0);          // UR top
         Graph_Vertex(-halfLength + chamfer, +halfWidth          , chamferBulge); // UL top
         Graph_Vertex(-halfLength          , +halfWidth - chamfer, 0.0);          // UL side
         Graph_Vertex(-halfLength          , -halfWidth + chamfer, chamferBulge); // LL side
         Graph_Vertex(-halfLength + chamfer, -halfWidth          , 0.0);          // LL bottom
      }

      EndOutline(realpart);

      m_doc->OnRedraw();
   }
   else
   {
      ErrorMessage("No package currently selected", "");
   }

}

/******************************************************************************
*/

void CDataDoctorPackagesPage::CreateCustomCircularOutline(double diameter)
{
   UpdateData(true);

   if (diameter <= 0.0)
   {
      ErrorMessage("Diameter must be greater than zero", "");
      return;
   }

   CDataDoctorRealPart *realpart = GetActiveRealPart();

   if (realpart != NULL && realpart->GetBlock() != NULL)
   {
      BlockStruct *rpblk = realpart->GetBlock();
      BlockStruct *rppkgblk = realpart->GetPkgBlock(this->getCamCadDatabase());
      if (rppkgblk != NULL)
      {
         Graph_Block_On(GBO_OVERWRITE, rppkgblk->getName(), rppkgblk->getFileNumber(), 0);

         int packageOutlineTopLayerIndex = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
         int widthIndex = this->getCamCadDatabase().getZeroWidthIndex(); //Graph_Aperture("", T_ROUND, penWidth, 0.0, 0.0, 0.0, 0.0, 0, 0L, FALSE, NULL);

         DataStruct *outlineData = Graph_Circle(packageOutlineTopLayerIndex, 0., 0., diameter/2., 0, widthIndex, 0, GetOutlineFillMode());
         outlineData->setGraphicClass(graphicClassPackageBody);
         int keyword = m_doc->RegisterKeyWord(ATT_OUTLINE_METHOD, 0, valueTypeString);
         m_doc->SetAttrib(&outlineData->getDefinedAttributes(), keyword, valueTypeString, outlineAlgTagToString(algCustom).GetBuffer(0), attributeUpdateOverwrite, NULL);
         
         Graph_Block_Off();

         // Align
         CDataDoctorComponent *sampleComp = this->GetActiveSampleComponent();
         if (sampleComp != NULL) 
         {
            CPoint2d pinsCenter = sampleComp->GetPinsCentroid( getCamCadDatabase() );
            DataStruct *pkgInsertData = realpart->GetBlock()->GetPackage();
            if (pkgInsertData != NULL && pkgInsertData->getDataType() == dataTypeInsert)
            {      
#ifdef ALIGN_BY_PIN_CENTER
               pkgInsertData->getInsert()->setOrigin(pinsCenter);
#else
               // Simply align same as comp
               pkgInsertData->getInsert()->setOrigin(0.,0.);//sampleComp->getOrigin() );
#endif
            }
         }

         m_doc->OnRedraw();
      }
   }
   else
   {
      ErrorMessage("No package currently selected", "");
   }

}

/******************************************************************************
*/

void CDataDoctorPackagesPage::InsertNewVertexAfter(int row)
{
   {
      if (row >= 0 && row < m_outlineEditGrid.GetNumberRows())
      {
         CPnt *selectedpnt = (CPnt*)m_outlineEditGrid.GetRowData(row);
         CPoly *poly = m_outlineEditGrid.GetActivePoly();

         if (poly != NULL && selectedpnt != NULL)
         {
#ifdef FIRST_TRY
            POSITION pos = poly->getPntList().GetHeadPosition();
            while (pos != NULL)
            {
               CPnt *curpnt = poly->getPntList().GetAt(pos);

               if (curpnt == selectedpnt)
               {
                  POSITION insertPos = pos;
                  poly->getPntList().GetNext(pos); // increments pos but gets same pt as before
                  CPnt *nextpnt = poly->getPntList().GetAt(pos); // this is the next pt

                  CPnt *newpnt = new CPnt(0.,0.);
                  newpnt->x = (DbUnit)((curpnt->x + nextpnt->x) / 2.); // put new pt half way down segment
                  newpnt->y = (DbUnit)((curpnt->y + nextpnt->y) / 2.);

                  poly->getPntList().InsertAfter(insertPos, newpnt);
                  pos = NULL; // quit
               }

               if (pos != NULL)
                  poly->getPntList().GetNext(pos); 
            }
#else
            POSITION insertPos = poly->getPntList().Find(selectedpnt);
            //POSITION pos = poly->getPntList().GetHeadPosition();
            if (insertPos != NULL)
            {
               CPnt *curpnt = selectedpnt;//poly->getPntList().GetAt(insertPos);

               POSITION pos = insertPos;
               poly->getPntList().GetNext(pos); // increments pos but gets same pt as before
               CPnt *nextpnt = poly->getPntList().GetAt(pos); // this is the next pt

               CPnt *newpnt = new CPnt(0.,0.);
               newpnt->x = (DbUnit)((curpnt->x + nextpnt->x) / 2.); // put new pt half way down segment
               newpnt->y = (DbUnit)((curpnt->y + nextpnt->y) / 2.);

               poly->getPntList().InsertAfter(insertPos, newpnt);
            }
#endif
         }

         m_outlineEditGrid.Fill(this, (row+1));
      }
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedInsertVertexAfter()
{
   UpdateData(true);

   CDataDoctorRealPart *realpart = GetActiveRealPart(); // just to ensure outine exists

   if (realpart != NULL)
   {
      long row = m_outlineEditGrid.GetCurrentRow();

      // First and last are same (closed poly), if last row then change to first row
      if (row == (m_outlineEditGrid.GetNumberRows() - 1))
         row = 0;

      InsertNewVertexAfter(row);
      //doesn't work   m_outlineEditGrid.SelectRow(row+1); // The new one
      this->getCamCadDatabase().getCamCadDoc().OnRedraw();
   }

   UpdateStatusActiveRow();
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedInsertVertexBefore()
{
   // Inserting before this vertex is same as inserting after previous vertex

   UpdateData(true);

   CDataDoctorRealPart *realpart = GetActiveRealPart(); // just to ensure outine exists

   if (realpart != NULL)
   {
      long row = m_outlineEditGrid.GetCurrentRow();

      // First and last are same (closed poly), if first row then change to last row
      if (row == 0)
         row = m_outlineEditGrid.GetNumberRows() - 1;

      // Now back up one row, to insert after previous
      row--;

      InsertNewVertexAfter(row);
      //doesn't work  m_outlineEditGrid.SelectRow(row+1); // The new one
      this->getCamCadDatabase().getCamCadDoc().OnRedraw();
   }

   UpdateStatusActiveRow();
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedDeleteVertex()
{
   UpdateData(true);

   CDataDoctorRealPart *realpart = GetActiveRealPart();

   if (realpart != NULL)
   {
      long row = m_outlineEditGrid.GetCurrentRow();

      if (row >= 0 && row < m_outlineEditGrid.GetNumberRows())
      {
         //*rcf BUG is first or last pnt then need to fix closed poly, changes two vertices not just one

         if (row == 0 || row == (m_outlineEditGrid.GetNumberRows() - 1))
         {
            // Special case for first/last point, maintain closed poly.
            // There has to be at least two other points, so we end up with three pt closed poly.
            CPoly *poly = m_outlineEditGrid.GetActivePoly();
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
            CPnt *selectedpnt = (CPnt*)m_outlineEditGrid.GetRowData(row);
            CPoly *poly = m_outlineEditGrid.GetActivePoly();

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

         m_outlineEditGrid.Fill(this);
         this->SetHighlight(0., 0., false); // Point is gone, turn off highlight
         this->getCamCadDatabase().getCamCadDoc().OnRedraw();
      }
   }

   UpdateStatusActiveRow();
}


/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedCurveToNext()
{
   UpdateData(true);

   CDataDoctorRealPart *realpart = GetActiveRealPart();

   if (realpart != NULL)
   {
      long row = m_outlineEditGrid.GetCurrentRow();

      int numberRows = m_outlineEditGrid.GetNumberRows();

      if (row >= 0 && row < numberRows && numberRows > 1)
      {
         EditPntItem dlg(AfxGetMainWnd());

         if (row == m_outlineEditGrid.GetNumberRows()-1) // change from last pnt to first pnt
            row = 0;

         CPnt *thisPnt = (CPnt*)this->m_outlineEditGrid.GetRowData(row);
         CPnt *nextPnt = (CPnt*)this->m_outlineEditGrid.GetRowData(row+1);

         if (dlg.pnt != NULL)
         {
            dlg.pnt = thisPnt;
            dlg.next = nextPnt;
            dlg.decimals = 3;//decimals;
            dlg.doc = &this->getCamCadDatabase().getCamCadDoc();
            dlg.s = NULL;//s;

            dlg.SetValues();
            dlg.m_type = 0;

            if (dlg.DoModal() == IDOK)
            {
            }

            //m_outlineEditGrid.Fill(this);
            this->getCamCadDatabase().getCamCadDoc().OnRedraw();
         }
      }
   }

   UpdateStatusActiveRow();
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedApplyOffset()
{
   UpdateData(true);

   CDataDoctorRealPart *realpart = GetActiveRealPart();

   if (realpart != NULL)
   {
      DataStruct *outlinedata = realpart->GetOutlineData(this->getCamCadDatabase());
      if (outlinedata != NULL)
      {
         double offx = GetDouble(IDC_EDIT_OFFSETX);
         double offy = GetDouble(IDC_EDIT_OFFSETY);

         CTMatrix mat;
         mat.translate(offx, offy);

         outlinedata->transform(mat);
         this->getCamCadDatabase().getCamCadDoc().OnRedraw();

         this->m_outlineEditGrid.Fill(this); // coords changed, refill grid

         // Clear offset in GUI, since it is an action, not a setting.
         GetDlgItem(IDC_EDIT_OFFSETX)->SetWindowText("0.0");
         GetDlgItem(IDC_EDIT_OFFSETY)->SetWindowText("0.0");   
      }

   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnBnClickedRotate90()
{
   UpdateData(true);

   CDataDoctorRealPart *realpart = GetActiveRealPart();

   if (realpart != NULL)
   {
      DataStruct *outlinedata = realpart->GetOutlineData(this->getCamCadDatabase());
      if (outlinedata != NULL)
      {
         CTMatrix mat;
         mat.rotateDegrees(90.0);

         outlinedata->transform(mat);
         this->getCamCadDatabase().getCamCadDoc().OnRedraw();

         this->m_outlineEditGrid.Fill(this); // coords changed, refill grid
      }

   }
}

/******************************************************************************
*/

BOOL CDataDoctorPackagesPage::OnSetActive()
{
   this->getParent().SaveAsInitialPage(DdPackagesPage);

   if (IsUpdatePending())
      Update();

   return CDataDoctorPropertyPageType::OnSetActive();

}

/******************************************************************************
*/

void CDataDoctorPackagesPage::Update()
{
   // Update entire grid from cc/datadoc data

   //*rcf Potentially need to redefine grid due to custom attrib set changes
   //*rcf Check if attribs changed to avoid needless full update
   if (true)
   {
      DefineGridColumnMaps();
      if (this->m_pkgGeomGrid.IsSetUp())
         this->m_pkgGeomGrid.OnSetup();  // re-setup
      if (this->m_pkgPartGrid.IsSetUp())
         this->m_pkgPartGrid.OnSetup();  // re-setup
   }

   FillGrid();
   m_updatePending = false;
   m_updateOtherGrid = false;
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::Update(int row)
{
   // Update one row from package already attached to row
   // (I.e. update the package elsewhere first, then update the grid display here.)

   if (GetGridMode() == PkgGridViewGeometries)
   {
      if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
      {
         CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(row);
         FillGeomGridRow(row, ddpkg);
         m_pkgGeomGrid.ResizeAll();
      }
   }
   else
   {
      if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
      {
         CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(row);
         FillPartGridRow(row, ddpart);
         m_pkgPartGrid.ResizeAll();
      }
   }
}

/******************************************************************************
*/

// Would be nice to move to grid class, need to resolve use of ColumnDefMap and m_colXxxYyy

void CDataDoctorPackagesPage::FillGeomGridRow(int row, CDataDoctorPackage *ddpkg)
{
   if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
   {
      this->m_pkgGeomGrid.QuickSetText(this->m_colGeometry_GeomGrid,      row, ddpkg->GetGeometryName());
      this->m_pkgGeomGrid.QuickSetText(this->m_colPkgAlias_GeomGrid,      row, ddpkg->GetPackageAlias());
      this->m_pkgGeomGrid.QuickSetText(this->m_colDPMO_GeomGrid,          row, ddpkg->GetDPMO());
      this->m_pkgGeomGrid.QuickSetText(this->m_colOutlineMethod_GeomGrid, row, ddpkg->GetOutlineMethod());
      this->m_pkgGeomGrid.QuickSetText(this->m_colPkgHeight_GeomGrid,     row, ddpkg->GetPackageHeight());
      this->m_pkgGeomGrid.QuickSetText(this->m_colFamily_GeomGrid,        row, ddpkg->GetFamily());
      this->m_pkgGeomGrid.QuickSetText(this->m_colComments_GeomGrid,      row, ddpkg->GetPackageComments());


      // User defined custom attribs
      //*rcf, same operaiton for both grids, on different grids with different maps,make a func with params
      POSITION customPos = this->m_customColumnMap_GeomGrid.GetStartPosition();
      while (customPos != NULL)
      {
         CString customColName;
         CDDSpecialColumn *custCol = NULL;
         this->m_customColumnMap_GeomGrid.GetNextAssoc(customPos, customColName, custCol);
         CString cellval = ddpkg->GetGenericAttrib(custCol->GetName());
         this->m_pkgGeomGrid.QuickSetText(custCol->GetColumnIndexRef(), row, cellval);
      }


      CString refnames( ddpkg->GetRefNameList() );
      this->m_pkgGeomGrid.QuickSetText(this->m_colRefnames_GeomGrid, row, refnames);

      this->m_pkgGeomGrid.SetRowData(row, ddpkg);

      this->m_pkgGeomGrid.UpdateStatus(row);
   }
}


void CDataDoctorPackagesPage::FillPartGridRow(int row, CDataDoctorPart *ddpart)
{
   if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
   {
      CCamCadDatabase& camCadDatabase = getParent().getDataDoctor().getCamCadDatabase();

      CDataDoctorComponent *headDdComp = ddpart->getHeadComponent();
      CString geomName = headDdComp->getGeometryName(camCadDatabase);
      if (ddpart->getGeometryCount(camCadDatabase) > 1)
         geomName += " (*)";

      this->m_pkgPartGrid.QuickSetText(this->m_colPartNumber_PartGrid,    row, ddpart->getPartNumber());
      this->m_pkgPartGrid.QuickSetText(this->m_colGeometry_PartGrid,      row, geomName);
      this->m_pkgPartGrid.QuickSetText(this->m_colPkgAlias_PartGrid,      row, ddpart->GetPackageAlias());
      this->m_pkgPartGrid.QuickSetText(this->m_colDPMO_PartGrid,          row, ddpart->GetDPMO());
      this->m_pkgPartGrid.QuickSetText(this->m_colPkgSource_PartGrid,     row, ddpart->GetPackageSource());
      this->m_pkgPartGrid.QuickSetText(this->m_colOutlineMethod_PartGrid, row, ddpart->GetOutlineMethod());
      this->m_pkgPartGrid.QuickSetText(this->m_colPkgHeight_PartGrid,     row, ddpart->GetPackageHeight(camCadDatabase.getPageUnits()));
      this->m_pkgPartGrid.QuickSetText(this->m_colFamily_PartGrid,        row, ddpart->GetFamily());
      this->m_pkgPartGrid.QuickSetText(this->m_colComments_PartGrid,      row, ddpart->GetPackageComments());

      CString refnames = ddpart->getLoadedReferenceDesignators(camCadDatabase);
      this->m_pkgPartGrid.QuickSetText(this->m_colRefnames_PartGrid, row, refnames);

      // User defined custom attribs
      POSITION customPos = this->m_customColumnMap_PartGrid.GetStartPosition();
      while (customPos != NULL)
      {
         CString customColName;
         CDDSpecialColumn *custCol = NULL;
         this->m_customColumnMap_PartGrid.GetNextAssoc(customPos, customColName, custCol);
         CString cellval = ddpart->GetPackageGenericAttrib(custCol->GetName());
         this->m_pkgPartGrid.QuickSetText(custCol->GetColumnIndexRef(), row, cellval);
      }

      this->m_pkgPartGrid.SetRowData(row, ddpart);

      ddpart->calculateStatus(camCadDatabase);

      this->m_pkgPartGrid.UpdateStatus(row);
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::FillGrid()
{
   CWaitCursor pardonMeThisMightTakeAMoment;

   FillGeomGrid();
   FillPartGrid();
}

//------------------------------------------------------------

void CDataDoctorPackagesPage::FillGeomGrid()
{
   if (!m_pkgGeomGrid.IsSetUp())
      return;

   CWaitCursor pardonMeThisMightTakeAMoment;

   CDataDoctor& doctor = getParent().getDataDoctor();
   this->getPackages().GatherPackages(getCamCadDatabase(), doctor.getComponents(), doctor.GetPackageLibraryAttribMap(), this->GetOutlineFillMode());

   this->m_pkgGeomGrid.RemoveAllRows();
   int rowIndx = 0;

   POSITION pos = this->getPackages().GetStartPosition();
   while (pos != NULL)
   {
      CString name;
      CDataDoctorPackage *ddpkg;
      this->getPackages().GetNextAssoc(pos, name, ddpkg);
      if (ddpkg != NULL)
      {
         int pinCount = ddpkg->GetExampleComponent()->getPinCount();
         if (pinCount > 1 || !m_chkHideSinglePinComps)
         {
            this->m_pkgGeomGrid.SetNumberRows(rowIndx+1);
            FillGeomGridRow(rowIndx, ddpkg);
            rowIndx++;
         }
      }
   }

   m_pkgGeomGrid.ResizeAll(); //m_pkgGeomGrid.BestFit(0, m_pkgGeomGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
}

//------------------------------------------------------------

void CDataDoctorPackagesPage::FillPartGrid()
{
   if (!m_pkgPartGrid.IsSetUp())
      return;

   this->m_pkgPartGrid.RemoveAllRows();

   CWaitCursor pardonMeThisMightTakeAMoment;

   CDataDoctorParts& parts         = getParent().getDataDoctor().getParts();
   CCamCadDatabase& camCadDatabase = getParent().getDataDoctor().getCamCadDatabase();

   parts.sync(camCadDatabase,getParent().getDataDoctor().getComponents(), getParent().getDataDoctor().GetPackageLibraryAttribMap());

   CDataDoctorPart* part;
   int rowIndx = 0;
   int index;

   // The parts list contains, and the parts tab will show, an entry for blank partnumber.
   // That is not useful on Packages tab, since usually one wants to make settings and save to the database.
   // If operating by partnumber then partnumber is the DB key, and blank is a no-go.
   // So for Packages tab in partnumber mode we omit parts with blank partnumber.

   for (parts.rewind(index);parts.next(part,index);)
   {
      gatherFamily(camCadDatabase, part, getParent().getDataDoctor().getPackages());
      if (part->isLoaded(camCadDatabase) && !part->getPartNumber().IsEmpty())
      {
         int pinCount = part->getHeadComponent()->getPinCount();
         if (pinCount > 1 || !m_chkHideSinglePinComps)
         {
            this->m_pkgPartGrid.SetNumberRows(rowIndx+1);
            FillPartGridRow(rowIndx, part);
            rowIndx++;
         }
      }
   }

   m_pkgPartGrid.ResizeAll(); //m_pkgPartGrid.BestFit(0, m_pkgPartGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);

   //*rcf This will likely turn out too slow, but it is correct.
   // Issue is PN might have changed, so Real Part block to insert might have changed.
   // Tends to be a problem in the PN grid, but not in the Geom grid, since DD can't change the comp geom that is inserted.
   ResetRPOutlines();
}

void CDataDoctorPackagesPage::gatherFamily(CCamCadDatabase& camCadDatabase, CDataDoctorPart* part, CDataDoctorPackages &pkages)
{
   if (!part->GetPackageSource().CompareNoCase(QPartNumber))
      part->addFamilyFromAttribute(camCadDatabase);   
   else
   {
      CDataDoctorComponent *headDdComp = part->getHeadComponent();
      CDataDoctorPackage *ddpkg = pkages.FindPkgUsingGeomBlk(headDdComp->getGeometryBlockNum());
      if(ddpkg) 
      {
         ddpkg->AddFamilyFromAttribute();
         part->setFamily(camCadDatabase, ddpkg->GetFamily());
      }
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::EnableCustomOutlineEditing(bool enable)
{
   this->m_outlineEditGrid.SetColumnTitles(enable);  // Can't just gray easily, so hide/show for enable

   if (!enable)
   {
      ClearTemplate();
      m_outlineEditGrid.RemoveAllRows();
      //doesnt work m_outlineEditGrid.EnableWindow(false);
   }
   else
   {
      OnCboSelchangeTemplate();
      //doesn't work m_outlineEditGrid.EnableWindow(true);
      m_outlineEditGrid.Fill(this);
   }

   GetDlgItem(IDC_GRP_CUSTOM)->EnableWindow(enable);
   GetDlgItem(IDC_EditGridStatic)->EnableWindow(enable);
   GetDlgItem(IDC_COMBO_TEMPLATE)->EnableWindow(enable);
   GetDlgItem(IDC_BTN_CREATE_TEMPLATE_OUTLINE)->EnableWindow(false); // Not enable until params are validated
   GetDlgItem(IDC_GRP_OUTLINE_OFFSETXY)->EnableWindow(enable);
   GetDlgItem(IDC_EDIT_OFFSETX)->EnableWindow(enable);
   GetDlgItem(IDC_EDIT_OFFSETY)->EnableWindow(enable);
   GetDlgItem(IDC_BTN_APPLY_OFFSET)->EnableWindow(enable);
   GetDlgItem(IDC_GRP_OUTLINE_ROTATE)->EnableWindow(enable);
   GetDlgItem(IDC_BTN_ROTATE90)->EnableWindow(enable);
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::DoEditGeom(CDataDoctorComponent *ddcomp, BlockStruct *realpartBlk, bool enableCustom, int hightlightPtIndx)
{
   if (ddcomp != NULL)
   {
      EnableCustomOutlineEditing(enableCustom);

      BlockStruct *insertedCompBlk = ddcomp->getGeometryBlock( getCamCadDatabase() );
      
      if (insertedCompBlk != NULL)
      {
         BlockStruct *fakeBlock = NULL;

         fakeBlock = Graph_Block_On(GBO_OVERWRITE, "OutlineEditBlock", insertedCompBlk->getFileNumber(), 0);
         fakeBlock->setBlockType(blockTypeGeometryEdit);

         Graph_Block_Reference(insertedCompBlk->getBlockNumber(), "ReferenceBackground", 0., 0., 0., 0, 1., -1);

         if (realpartBlk != NULL)
            Graph_Block_Reference(realpartBlk->getBlockNumber(), "PackageOutline", 0., 0., 0., 0, 1., -1);


         //*rcf Experimental
         if (realpartBlk != NULL && enableCustom)
         {
            CExtent rpblkExt = realpartBlk->getExtent(getCamCadDatabase().getCamCadData());
            double size = rpblkExt.getMaxSize();
            if (size > 0.0)
               size *= 0.035; // 3.5 %
            else
               size = 0.035;  // punt, but still 3.5% (was .01, too small)

            int packageOutlineTopIndex = Graph_Level(RP_LAYER_PKG_BODY_TOP, "", FALSE);
            int zeroWidth = getCamCadDatabase().getZeroWidthIndex();

            BlockStruct *highlightBlk = Graph_Block_On(GBO_OVERWRITE, "HighlightBlock", insertedCompBlk->getFileNumber(), 0);

            DataStruct *data = Graph_PolyStruct(packageOutlineTopIndex, 0, 0);
            Graph_Poly(NULL, zeroWidth, 0 , 0, 0);   // fillflag , not negative, not closed
            Graph_Vertex(-size, -size, 0.0);
            Graph_Vertex(  .00,   .00, 0.0);
            Graph_Vertex(-size,  size, 0.0);
            Graph_Vertex(  .00,   .00, 0.0);
            Graph_Vertex( size,  size, 0.0);
            Graph_Vertex(  .00,   .00, 0.0);
            Graph_Vertex( size, -size, 0.0);
            Graph_Block_Off();

            m_highlightPntInsertData = Graph_Block_Reference(highlightBlk->getBlockNumber(), "HighlightPoint", 0., .0, 0., 0, 1., packageOutlineTopIndex);
            m_highlightPntInsertData->setHidden(true);
         }
         
         Graph_Block_Off();


         EditGeometry( &getCamCadDatabase().getCamCadDoc(), fakeBlock );

         //*rcf experimental, not working
         //if (false) //enableCustom)
         //{
         //   this->m_outlineEditGrid.Highlight(getCamCadDatabase().getCamCadDoc(), this);
         //}
      }
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::SetHighlight(double x, double y, bool show)
{
   if (m_highlightPntInsertData != NULL)
   {
      m_highlightPntInsertData->setHidden(!show);
      m_highlightPntInsertData->getInsert()->setOriginX(x);
      m_highlightPntInsertData->getInsert()->setOriginY(y);

      this->getCamCadDatabase().getCamCadDoc().OnRedraw();
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::DoEditGeom()
{
   if (GetGridMode() == PkgGridViewGeometries)
   {
      long row = m_pkgGeomGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
      {
         CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(row);
         if (ddpkg != NULL)
         {
            HACKRP = &ddpkg->GetRealPart();
            BlockStruct *outlineBlk = ddpkg->GetRealPart().GetBlock(); //ddpkg->GetRealPart().GetPkgBlock(camcaddatabase)
            DoEditGeom( ddpkg->GetExampleComponent(), outlineBlk, (ddpkg->GetOutlineMethodTag() == algCustom), -1 );
         }
      }
   }
   else
   {
      long row = m_pkgPartGrid.GetCurrentRow();

      if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
      {
         CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(row);
         if (ddpart != NULL)
         {
            BlockStruct *outlineBlk = ddpart->GetRealPart().GetBlock();
            DoEditGeom( ddpart->getHeadComponent(), outlineBlk, (ddpart->GetOutlineMethodTag() == algCustom), -1 );
         }
      }
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnCancel()
{
   getCamCadDatabase().getCamCadDoc().OnDoneEditing();

   CDataDoctorPropertyPageType::OnCancel();
}

/******************************************************************************
*/

BOOL CDataDoctorPackagesPage::OnKillActive()
{
   getCamCadDatabase().getCamCadDoc().OnDoneEditing();

   return CDataDoctorPropertyPageType::OnKillActive();
}

/******************************************************************************
*/

LRESULT CDataDoctorPackagesPage::OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   int row = wParamROW;
   int col = lParamCOL;

   // Thsi grid has changed, may effect contents of other grid, set flag to update other grid if view is changed
   m_updateOtherGrid = true;

   if (GetGridMode() == PkgGridViewGeometries)
   {
      if (row >= 0 && row < this->m_pkgGeomGrid.GetNumberRows())
      {
         CString val;
         m_pkgGeomGrid.QuickGetText(col, row, &val);
         CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(row);

         if (ddpkg != NULL)
         {
            if (col == m_colPkgAlias_GeomGrid)
            {
               ddpkg->SetPackageAlias(val);
            }
            else if (col == m_colDPMO_GeomGrid)
            {
               ddpkg->SetDPMO(val);
            }
            else if (col == m_colPkgHeight_GeomGrid)
            {
               ddpkg->SetPackageHeight(val);
            }
            else if (col == m_colOutlineMethod_GeomGrid)
            {
               ddpkg->SetOutlineMethod(val, this->GetOutlineFillMode());
               DoEditGeom(); // To refresh display with new outline
            }
            else if (col == m_colFamily_GeomGrid)
            {
               ddpkg->SetFamily(val);
            }
            else if (col == m_colComments_GeomGrid)
            {
               ddpkg->SetPackageComments(val);
            }
            else if (this->IsCustomGeomGridColumn(col))
            {
               CDDSpecialColumn *usercol = m_customColumnMap_GeomGrid.GetSpecialColumn(col);
               if (usercol != NULL)
                  ddpkg->SetGenericAttrib(usercol->GetName(), val);
               // These attribs may appear in another page, so notify other pages of change
               getParent().updatePropertyPages(this);
            }
         }

         m_pkgGeomGrid.UpdateStatus(row);
         m_pkgGeomGrid.ResizeAll();
      }
   }
   else
   {
      // Part Number
      if (row >= 0 && row < this->m_pkgPartGrid.GetNumberRows())
      {
         CString val;
         m_pkgPartGrid.QuickGetText(col, row, &val);
         CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(row);

         if (ddpart != NULL)
         {
            if (col == m_colPkgAlias_PartGrid)
            {
               ddpart->setPackageAlias(getCamCadDatabase(), val);
            }
            else if (col == m_colDPMO_PartGrid)
            {
               ddpart->setDPMO(getCamCadDatabase(), val);
            }
            else if (col == m_colPkgHeight_PartGrid)
            {
               ddpart->setPackageHeight(getCamCadDatabase(), val);
            }
            else if (col == m_colFamily_PartGrid)
            {
               ddpart->setFamily(getCamCadDatabase(), val);
            }
            else if (col == m_colComments_PartGrid)
            {
               ddpart->setPackageComments(getCamCadDatabase(), val);
            }
            else if (col == this->m_colPkgSource_PartGrid)
            {
               ddpart->setPackageSource(getCamCadDatabase(), val);
               gatherFamily(getParent().getCamCadDatabase(), ddpart, getParent().getDataDoctor().getPackages());
               
               // If package source just got set to geometry then update values to geometry vals
               if (val.CompareNoCase(QPartNumber) != 0)
               {
                  CDataDoctorPackage *ddpkg = this->getPackages().FindPkg(ddpart->getHeadComponent()->getGeometryName(getCamCadDatabase()));
                  if (ddpkg != NULL)
                  {
                     ddpart->setPackageAlias(getCamCadDatabase(),  ddpkg->GetPackageAlias());
                     ddpart->setDPMO(getCamCadDatabase(),          ddpkg->GetDPMO());
                     ddpart->setPackageHeight(getCamCadDatabase(), ddpkg->GetPackageHeight());
                     ddpart->setOutlineMethod(getCamCadDatabase(), ddpkg->GetOutlineMethod(), this->GetOutlineFillMode());
                     this->FillPartGridRow(row, ddpart);
                  }
               }
            }
            else if (col == m_colOutlineMethod_PartGrid)
            {
               ddpart->setOutlineMethod(getCamCadDatabase(), val, this->GetOutlineFillMode());
              DoEditGeom(); // To refresh display with new outline
            }
            else if (this->IsCustomPartGridColumn(col))
            {
               CDDSpecialColumn *usercol = m_customColumnMap_PartGrid.GetSpecialColumn(col);
               if (usercol != NULL)
                  ddpart->setGenericAttrib(getCamCadDatabase(), usercol->GetName(), val);
               // These attribs may appear in another page, so notify other pages of change
               getParent().updatePropertyPages(this);
            }
         }

         m_pkgPartGrid.UpdateStatus(row);
         m_pkgPartGrid.ResizeAll();
      }
   }

   return 0;
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnLoadPackage()
{
   if (GetGridMode() == PkgGridViewGeometries)
   {
      OnLoadGeometryPackage();
   }
   else
   {
      OnLoadPartNumberPackage();
   }
}


void CDataDoctorPackagesPage::OnSavePackage()
{
   if (GetGridMode() == PkgGridViewGeometries)
   {
      OnSaveGeometryPackage();
   }
   else
   {
      OnSavePartNumberPackage();
   }
}

/******************************************************************************
*/

void CDataDoctorPackagesPage::OnLoadGeometryPackage()
{
   // Load one package from DB, for selected row

   int matchCount = 0;

   UpdateData(true);
   
   int row = m_pkgGeomGrid.GetCurrentRow(); // Currently selected row

   if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
   {
      CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(row);
      
      if (ddpkg != NULL)
      {
         if (ddpkg->LoadFromDB( this->GetOutlineFillMode(), getParent().getDataDoctor().GetPackageLibraryAttribMap() ))
         {
            matchCount = 1;
            Update(row);
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

void CDataDoctorPackagesPage::OnSaveGeometryPackage()
{
   // Save one package to DB, from selected row

   UpdateData(true);
   
   int row = m_pkgGeomGrid.GetCurrentRow(); // Currently selected row

   if (row >= 0 && row < m_pkgGeomGrid.GetNumberRows())
   {
      CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)m_pkgGeomGrid.GetRowData(row);
      
      if (ddpkg != NULL)
      {
         ddpkg->SaveToDB();
      }
   }
}
/******************************************************************************
*/

void CDataDoctorPackagesPage::OnLoadPartNumberPackage()
{
   // Load one package from DB, for selected row

   int matchCount = 0;

   UpdateData(true);
   
   int row = m_pkgPartGrid.GetCurrentRow(); // Currently selected row

   CDataDoctorLibraryAttribMap& activeAttribMap = getParent().getDataDoctor().GetPackageLibraryAttribMap();

   if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
   {
      CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(row);
      
      if (ddpart != NULL)
      {
         if (ddpart->LoadPackageInfoFromDB(this->getCamCadDatabase(), this->GetOutlineFillMode(), activeAttribMap))
         {
            matchCount = 1;
            Update(row);
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

void CDataDoctorPackagesPage::OnSavePartNumberPackage()
{
   // Save one package to DB, from selected row

   UpdateData(true);
   
   int row = m_pkgPartGrid.GetCurrentRow(); // Currently selected row

   if (row >= 0 && row < m_pkgPartGrid.GetNumberRows())
   {
      CDataDoctorPart *ddpart = (CDataDoctorPart*)m_pkgPartGrid.GetRowData(row);
      
      if (ddpart != NULL)
      {
         ddpart->SavePackageInfoToDB(this->getCamCadDatabase());
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CDDPkgsByGeometryGrid::CDDPkgsByGeometryGrid(CDataDoctorPackagesPage *page)
: m_packagesPage(page)
{
}

/******************************************************************************
*/

void CDDPkgsByGeometryGrid::SetRowStatusColor(int row, DataDoctorStatusTag status)
{
   switch (status)
   {
   case dataDoctorStatusOk:  // case dataDoctorStatusOk:       
      this->SetRowBackColor(row,                m_greenBackgroundColor);
      //getGrid().QuickSetTextColor(m_colStatus, rowIndx, colorDkGreen);  // move to base grid, use if (m_colStatus != -1) maybe?  Parts tab uses this
      break;
   case dataDoctorStatusError:  // case dataDoctorStatusError:    
      this->SetRowBackColor(row,                m_redBackgroundColor);
      //getGrid().QuickSetTextColor(m_colStatus, rowIndx, colorRed);      
      break;
   }
}

/******************************************************************************
*/

void CDDPkgsByGeometryGrid::UpdateStatus(int row)
{
   // Update one status row from package already attached to row.
   // Caller takes care of grid redraw, e.g. in FillGrid we don't want to redraw on every
   // status update, but in an Edit update we do.

   if (row >= 0 && row < this->GetNumberRows())
   {
      CString msg;
      CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)this->GetRowData(row);
      if (ddpkg != NULL)
      {
         DataDoctorStatusTag rowstatus = ddpkg->GetStatus(msg);
         this->QuickSetText(this->m_packagesPage->m_colMessage_GeomGrid, row, msg);
         this->SetRowStatusColor(row, rowstatus);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void CDDPkgsByGeometryGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	

   if (this->m_packagesPage != NULL)
   {
      int colcount = this->m_packagesPage->m_columnDefMap_GeomGrid.getSize();

      SetNumberCols(colcount);
      SetNumberRows(0);
      
      for (int indx = 0; indx < m_packagesPage->m_columnDefMap_GeomGrid.getSize(); indx++)
      {
         CColumnDef *cd = m_packagesPage->m_columnDefMap_GeomGrid.getAt(indx);
         CString colname = cd->GetTitle();
         int *colindxptr = cd->GetColIndexPtr();

         QuickSetText(*colindxptr, -1, colname);
      }

      // Outline Method option menu, same as offered by Access Analysis
      CUGCell cell;
      GetColDefault(this->m_packagesPage->m_colOutlineMethod_GeomGrid, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      CString alglist;
      alglist.Format("%s\n%s\n%s\n%s\n%s\n%s\n", 
         QNone,
         outlineAlgTagToString(algBodyOutline),
         outlineAlgTagToString(algPinCenters), 
         outlineAlgTagToString(algPinExtents), 
         outlineAlgTagToString(algInsidePads),
         outlineAlgTagToString(algCustom)
         );
      cell.SetLabelText( alglist ); 
      SetColDefault(this->m_packagesPage->m_colOutlineMethod_GeomGrid, &cell);

      EnableColSwapping(TRUE);

      BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      SetColWidth(-1, 0); // get rid of "row heading"

      // Enable Right-click menu
      EnableMenu(TRUE);

      SetSetUp(true);
   }
}


/////////////////////////////////////////////////////////////////////////////
//	OnColSwapped
//		Called just after column-swap operation was completed.
//	Params:
//		fromCol - where the col originated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		<none>
void CDDPkgsByGeometryGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   this->m_packagesPage->m_columnDefMap_GeomGrid.Update(this);

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

int CDDPkgsByGeometryGrid::OnEditStart(int col, long row, CWnd **edit)
{
   m_packagesPage->DoEditGeom();  // zoom to component

   *edit = &m_myCUGEdit;

   // Only Package Alias, Height, and DPMO are text-box edit enabled

   if (
      col == m_packagesPage->m_colPkgAlias_GeomGrid  ||
      col == m_packagesPage->m_colPkgHeight_GeomGrid ||
      col == m_packagesPage->m_colDPMO_GeomGrid      ||
      col == m_packagesPage->m_colComments_GeomGrid    ||
      m_packagesPage->IsCustomGeomGridColumn(col)
      )
   {
      return true;
   }
   else if(col == m_packagesPage->m_colFamily_GeomGrid)
   {
      CDataDoctorPackage *ddpkg = (CDataDoctorPackage*)this->GetRowData(row);
      if(ddpkg)
      {
         CString msg;
         DataDoctorStatusTag rowstatus = ddpkg->GetStatus(msg);
         return (msg.MakeLower().Find("outline") < 0);      
      }
   }

	return false;
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
int CDDPkgsByGeometryGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	//UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	//UNREFERENCED_PARAMETER(*vcKey);

   // Only integer numbers allowed in DPMO.
   // Movement keys, backspace, left and right arrows, and Delete key are okay too.
   if (col == m_packagesPage->m_colDPMO_GeomGrid)
   {
      if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE ||
         isdigit(*vcKey))
         return TRUE;
      else
         return FALSE;
   }
   else if (col == m_packagesPage->m_colPkgHeight_GeomGrid)
   {
      return OnEditVerifyFloatCell(col, row, edit, vcKey);
   }

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
int CDDPkgsByGeometryGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   BOOL pm = this->m_packagesPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);

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
int CDDPkgsByGeometryGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	UNREFERENCED_PARAMETER(param);

   if (msg == UGCT_DROPLISTSTART)
   {
      // The droplist is being opened, zoom to component in row
      this->m_packagesPage->DoEditGeom();
   }
   else if (msg == UGCT_DROPLISTPOSTSELECT)
   {
      // Convert "None" to blank
      CString celltext;
      this->QuickGetText(col, row, &celltext);
      if (celltext.CompareNoCase(QNone) == 0)
      {
         celltext.Empty();
         this->QuickSetText(col, row, celltext);
      }

      BOOL pm = this->m_packagesPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);
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
void CDDPkgsByGeometryGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	//UNREFERENCED_PARAMETER(item);

   this->m_packagesPage->SendMessage(WM_COMMAND, item);
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
int CDDPkgsByGeometryGrid::OnMenuStart(int col,long row,int section)
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

   CString name = QuickGetText(this->m_packagesPage->m_colGeometry_GeomGrid, row);

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CDDPkgsByPartNumberGrid::CDDPkgsByPartNumberGrid(CDataDoctorPackagesPage *page)
: m_packagesPage(page)
{
}

/******************************************************************************
*/

void CDDPkgsByPartNumberGrid::SetRowStatusColor(int row, DataDoctorStatusTag status)
{
   switch (status)
   {
   case dataDoctorStatusOk:       
      this->SetRowBackColor(row,                m_greenBackgroundColor, m_greenBackgroundColor);
      //getGrid().QuickSetTextColor(m_colStatus, rowIndx, colorDkGreen);  // move to base grid, use if (m_colStatus != -1) maybe?  Parts tab uses this
      break;

   case dataDoctorStatusError:   
      this->SetRowBackColor(row,                m_redBackgroundColor, m_redBackgroundColor);
      //getGrid().QuickSetTextColor(m_colStatus, rowIndx, colorRed);      
      break;

   case dataDoctorStatusIgnore:   
      this->SetRowBackColor(row,                m_lightGrayBackgroundColor, m_greenBackgroundColor);
      //getGrid().QuickSetTextColor(m_colStatus, rowIndx, colorRed);      
      break;
   }
}

/******************************************************************************
*/
void CDDPkgsByPartNumberGrid::SetRowBackColor(int rowIndx, COLORREF rowColor, COLORREF pkgSrcColColor)
{
   for (int colIndx = 0; colIndx < this->GetNumberCols(); colIndx++)
   {
      if (colIndx == this->m_packagesPage->m_colPkgSource_PartGrid)
         this->QuickSetBackColor(colIndx, rowIndx, pkgSrcColColor);
      else
         this->QuickSetBackColor(colIndx, rowIndx, rowColor);
   }
}

/******************************************************************************
*/

void CDDPkgsByPartNumberGrid::UpdateStatus(int row)
{
   // Update one status row from package already attached to row.
   // Caller takes care of grid redraw, e.g. in FillGrid we don't want to redraw on every
   // status update, but in an Edit update we do.

   if (row >= 0 && row < this->GetNumberRows())
   {
      CString msg;
      CDataDoctorPart *ddpart = (CDataDoctorPart*)this->GetRowData(row);
      if (ddpart != NULL)
      {
         DataDoctorStatusTag rowstatus = ddpart->GetPackageStatus(this->m_packagesPage->getCamCadDatabase(), msg);
         this->QuickSetText(this->m_packagesPage->m_colMessage_PartGrid, row, msg);
         this->SetRowStatusColor(row, rowstatus);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void CDDPkgsByPartNumberGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	

   if (this->m_packagesPage != NULL)
   {
      int colcount = this->m_packagesPage->m_columnDefMap_PartGrid.getSize();

      SetNumberCols(colcount);
      SetNumberRows(0);
      
      for (int indx = 0; indx < m_packagesPage->m_columnDefMap_PartGrid.getSize(); indx++)
      {
         CColumnDef *cd = m_packagesPage->m_columnDefMap_PartGrid.getAt(indx);
         CString colname = cd->GetTitle();
         int *colindxptr = cd->GetColIndexPtr();

         QuickSetText(*colindxptr, -1, colname);
      }

      // Outline Method option menu, same as offered by Access Analysis
      CUGCell cell;
      GetColDefault(this->m_packagesPage->m_colOutlineMethod_PartGrid, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      CString alglist;
      alglist.Format("%s\n%s\n%s\n%s\n%s\n%s\n", 
         QNone,
         outlineAlgTagToString(algBodyOutline),
         outlineAlgTagToString(algPinCenters), 
         outlineAlgTagToString(algPinExtents), 
         outlineAlgTagToString(algInsidePads),
         outlineAlgTagToString(algCustom)
         );
      cell.SetLabelText( alglist ); 
      SetColDefault(this->m_packagesPage->m_colOutlineMethod_PartGrid, &cell);

      // Package Source option menu
      GetColDefault(this->m_packagesPage->m_colPkgSource_PartGrid, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      CString pkgsrclist;
      pkgsrclist.Format("%s\n%s\n", 
         "Geometry",
         "Part Number"
         );
      cell.SetLabelText( pkgsrclist ); 
      SetColDefault(this->m_packagesPage->m_colPkgSource_PartGrid, &cell);

      EnableColSwapping(TRUE);

      BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      SetColWidth(-1, 0); // get rid of "row heading"

      // Enable Right-click menu
      EnableMenu(TRUE);

      SetSetUp(true);
   }
}


/////////////////////////////////////////////////////////////////////////////
//	OnColSwapped
//		Called just after column-swap operation was completed.
//	Params:
//		fromCol - where the col originated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		<none>
void CDDPkgsByPartNumberGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   this->m_packagesPage->m_columnDefMap_PartGrid.Update(this);

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

int CDDPkgsByPartNumberGrid::OnEditStart(int col, long row, CWnd **edit)
{
   m_packagesPage->DoEditGeom();  // zoom to component

   *edit = &m_myCUGEdit;

   // Only Package Alias, Height, and DPMO are text-box edit enabled

   CDataDoctorPart *ddpart = (CDataDoctorPart*)this->GetRowData(row);

   if (ddpart != NULL && ddpart->GetPackageSource().CompareNoCase("part number") == 0) //*rcf do better than string compare, make enum of bool func in pkg info
   {
      if (
         col == m_packagesPage->m_colPkgAlias_PartGrid  ||
         col == m_packagesPage->m_colPkgHeight_PartGrid ||
         col == m_packagesPage->m_colDPMO_PartGrid      ||
         col == m_packagesPage->m_colComments_PartGrid  ||
         m_packagesPage->IsCustomPartGridColumn(col)
         )
      {
         return true;
      }
      else if(col == m_packagesPage->m_colFamily_PartGrid)
      {
         CDataDoctorPart *ddpart = (CDataDoctorPart*)this->GetRowData(row);
         if(ddpart)
         {
            CString msg;
            DataDoctorStatusTag rowstatus = ddpart->GetPackageStatus(this->m_packagesPage->getCamCadDatabase(), msg);
            return (msg.MakeLower().Find("outline") < 0);      
         }
      }
   }

	return false;
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
int CDDPkgsByPartNumberGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	//UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	//UNREFERENCED_PARAMETER(*vcKey);

   // Only integer numbers allowed in DPMO.
   // Movement keys, backspace, left and right arrows, and Delete key are okay too.
   if (col == m_packagesPage->m_colDPMO_PartGrid)
   {
      if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE ||
         isdigit(*vcKey))
         return TRUE;
      else
         return FALSE;
   }
   else if (col == m_packagesPage->m_colPkgHeight_PartGrid)
   {
      return OnEditVerifyFloatCell(col, row, edit, vcKey);
   }

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
int CDDPkgsByPartNumberGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   BOOL pm = this->m_packagesPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);

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
int CDDPkgsByPartNumberGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	UNREFERENCED_PARAMETER(param);

   if (msg == UGCT_DROPLISTSTART)
   {
      // The droplist is being opened, zoom to component in row
      this->m_packagesPage->DoEditGeom();

      // Disallow changing outline method is package source is not Part Number
      CDataDoctorPart *ddpart = (CDataDoctorPart*)this->GetRowData(row);
      if (ddpart->GetPackageSource().CompareNoCase(QPartNumber) != 0 &&
         col == this->m_packagesPage->m_colOutlineMethod_PartGrid)
      {
         return FALSE;
      }

   }
   else if (msg == UGCT_DROPLISTPOSTSELECT)
   {
      // Convert "None" to blank
      CString celltext;
      this->QuickGetText(col, row, &celltext);
      if (celltext.CompareNoCase(QNone) == 0)
      {
         celltext.Empty();
         this->QuickSetText(col, row, celltext);
      }

      BOOL pm = this->m_packagesPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);
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
void CDDPkgsByPartNumberGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	//UNREFERENCED_PARAMETER(item);

   this->m_packagesPage->SendMessage(WM_COMMAND, item);
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
int CDDPkgsByPartNumberGrid::OnMenuStart(int col,long row,int section)
{
   // If R click is not within grid-proper, then cancel the menu.
   // e.g. no menu for top heading or scroll bars
   if (section != UG_GRID)
      return FALSE;

   // Might be in the grid section but not in a grid cell.
   // Reject these too.
   if (row < 0 || col < 0)
      return FALSE;

   // Allow Save/Load only if Package Source is Part Number
   CDataDoctorPart *ddpart = (CDataDoctorPart*)this->GetRowData(row);
   if (ddpart == NULL || ddpart->GetPackageSource().CompareNoCase(QPartNumber) != 0)
      return FALSE;

   
   // Proceed with menu

   CString name = QuickGetText(this->m_packagesPage->m_colPartNumber_PartGrid, row);

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CDDOutlineEditGrid::CDDOutlineEditGrid(CDataDoctorPackagesPage *page)
: m_packagesPage(page)
{
}

//----------------------------------------------------------------------------------


void CDDOutlineEditGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	


   SetNumberCols(2);
   SetNumberRows(0);

   // Right justify X and Y coords
   CUGCell cell;
   GetColDefault(0, &cell);
   cell.SetAlignment(UG_ALIGNRIGHT);
   SetColDefault(0, &cell);
   GetColDefault(1, &cell);
   cell.SetAlignment(UG_ALIGNRIGHT);
   SetColDefault(1, &cell);

   // Don't do this here, as is the usual practice.
   // Column titles come and go with enable/disable of custom editing
   ////QuickSetText(0, -1, "X");
   ////QuickSetText(1, -1, "Y");

   //*rcf   SetColWidth(-1, 0); // get rid of "row heading"
      
   // Enable Right-click menu
   EnableMenu(TRUE);

}

void CDDOutlineEditGrid::SetColumnTitles(bool show)
{
   QuickSetText(0, -1, (show ? "X" : ""));
   QuickSetText(1, -1, (show ? "Y" : ""));

   RedrawRow(-1);
}

void CDDOutlineEditGrid::Fill(int rowIndx, CPnt *pnt)
{
   if (rowIndx >= 0 && pnt != NULL)
   {
      this->SetNumberRows(rowIndx+1);

      this->QuickSetNumber(0, rowIndx, pnt->x, 3); //*rcf find GetDecimals
      this->QuickSetNumber(1, rowIndx, pnt->y, 3); //*rcf find GetDecimals

      this->SetRowData(rowIndx, (void*)pnt);
   }
}

void CDDOutlineEditGrid::Fill(CDataDoctorPackagesPage *pkgpage, int markRow)
{
   //this->SetNumberRows(0);
   this->RemoveAllRows();
   m_activePoly = NULL;

   if (pkgpage != NULL)
   {
      CDataDoctorRealPart *realpart = pkgpage->GetActiveRealPart();
      if (realpart != NULL && realpart->GetBlock() != NULL)
      {
         DataStruct *data = realpart->GetOutlineData(pkgpage->getCamCadDatabase());
         if (data != NULL)
         {
            int rowIndx = 0;

            CPolyList *polylist = data->getPolyList();
            if (polylist != NULL)
            {
               POSITION polypos = polylist->GetHeadPosition(); 
               while (polypos != NULL)
               {
                  CPoly *poly = polylist->GetNext(polypos);

                  // We're only expecting ONE !
                  if (m_activePoly == NULL)
                     m_activePoly = poly;
                  else
                     ErrorMessage("Not expecting multiple polys in outline.", "Fill Edit Grid");

                  POSITION pntpos = poly->getPntList().GetHeadPosition();
                  while (pntpos != NULL)
                  {
                     if (rowIndx == markRow)
                        this->QuickSetText(-1, rowIndx, "*");
                     else
                        this->QuickSetText(-1, rowIndx, "");

                     CPnt *pnt = poly->getPntList().GetNext(pntpos);
                     Fill(rowIndx++, pnt);
                  }
               }
            }
         }
      }

      //*rcf Get back to this, as is it causes too much grid jumping about
      ///if (markRow > 0)
      ///{
      ///   AdjustTopRow();
      ///}

      this->RedrawAll();
   }
}

#ifdef MIMIC_ELSEWHERE___DIDNT_WORK
void CDDOutlineEditGrid::Highlight(CCEtoODBDoc &doc, CDataDoctorPackagesPage *pkgpage)
{
   //*rcf Is Not Working 

   if (pkgpage != NULL)
   {
      CDataDoctorRealPart *realpart = pkgpage->GetActiveRealPart();
      if (realpart != NULL && realpart->GetBlock() != NULL)
      {
         DataStruct *data = realpart->GetOutlineData(pkgpage->getCamCadDatabase());
         if (data != NULL)
         {
            int rowIndx = 0;

            CPolyList *polylist = data->getPolyList();
            if (polylist != NULL)
            {
               POSITION polypos = polylist->GetHeadPosition(); 
               while (polypos != NULL)
               {
                  CPoly *poly = polylist->GetNext(polypos);
                  POSITION pntpos = poly->getPntList().GetHeadPosition();
                  while (pntpos != NULL)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntpos);

                     //CPoly *highlightPoly = (CPoly*)m_tree.GetItemData(parent);
                     //highlightPnt = (CPnt*)voidPtr;
                     //doc.HighlightPolyItem(s, highlightPoly, highlightPnt, FALSE);
                     SelectStruct s;
                     s.setData(data);
                     s.getData()->setSelected(!true);
                     s.filenum  = -1;
                     s.insert_x = (DbUnit)0.;
                     s.insert_y = (DbUnit)0.;
                     s.scale    = (DbUnit)1.;
                     s.rotation = (DbUnit)0.;
                     s.mirror   = 0;
                     s.layer    = -1;
                     doc.HighlightPolyItem(&s, poly, pnt, FALSE);
                     return;

                  }
               }
            }
         }
      }

      this->RedrawAll();
   }
}
#endif

void CDDOutlineEditGrid::Highlight(CDataDoctorPackagesPage *pkgpage)
{
   if (pkgpage != NULL)
   {
      int row = this->GetCurrentRow();

      Highlight(pkgpage, row);
   }
}

void CDDOutlineEditGrid::Highlight(CDataDoctorPackagesPage *pkgpage, int row)
{
   if (pkgpage != NULL)
   {
      if (row >= 0 && row < this->GetNumberRows())
      {
         CPnt *pnt = (CPnt*)this->GetRowData(row);

         if (pnt != NULL)
         {
            CPoint2d origin;
            CDataDoctorRealPart *rp = this->m_packagesPage->GetActiveRealPart();
            if (rp != NULL)
            {
               DataStruct *pkg = rp->GetPkgInsertData(this->m_packagesPage->getCamCadDatabase());
               if (pkg != NULL)
               {
                  origin = pkg->getInsert()->getOrigin2d();
               }
            }
            pkgpage->SetHighlight(pnt->x + origin.x, pnt->y + origin.y, true);
         }
      }
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

int CDDOutlineEditGrid::OnEditStart(int col, long row, CWnd **edit)
{
   //m_packagesPage->DoEditGeom();  // zoom to component
   this->Highlight(this->m_packagesPage, row);

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
int CDDOutlineEditGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	return OnEditVerifyFloatCell(col, row, edit, vcKey);
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
int CDDOutlineEditGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   // If ya want to get the value from the grid then have to post a message here and get
   // value later. Notify is saying editing is finished, but grid itself is not yet
   // updated. Use string param if you want to act on new val now.
  /// BOOL pm = this->m_packagesPage->PostMessage(ID_OUTLINE_GRIDROWEDITFINISHED, row, col);

   // Manage first and last point, given that poly is always closed then first
   // and last point should always be the same. Allow user to edit either, but update both.

   if (!cancelFlag)
   {
      ////CString val;
      ////QuickGetText(col, row, &val);
      ////double dval = atof(val);
      double dval = atof(string);
      CPnt *pnt = (CPnt*)GetRowData(row);
      if (pnt != NULL)
      {
         if (col == 0)
            pnt->x = (DbUnit)dval;
         else if (col == 1)
            pnt->y = (DbUnit)dval;
      }

      if (row == 0) // first row, update last row to match
      {
         pnt = (CPnt*)GetRowData(GetNumberRows() - 1);
         if (pnt != NULL)
         {
            if (col == 0)
               pnt->x = (DbUnit)dval;
            else if (col == 1)
               pnt->y = (DbUnit)dval;
         }
      }
      else if (row == (GetNumberRows() - 1)) // last row, update first to match
      {
         pnt = (CPnt*)GetRowData(0);
         if (pnt != NULL)
         {
            if (col == 0)
               pnt->x = (DbUnit)dval;
            else if (col == 1)
               pnt->y = (DbUnit)dval;
         }
      }


      this->m_packagesPage->getCamCadDatabase().getCamCadDoc().OnRedraw();
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
void CDDOutlineEditGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	//UNREFERENCED_PARAMETER(item);

   this->m_packagesPage->SendMessage(WM_COMMAND, item);

}

void CDDOutlineEditGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
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

   this->Highlight(this->m_packagesPage, row);

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
int CDDOutlineEditGrid::OnMenuStart(int col,long row,int section)
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
   this->ClearSelections();
   SetMultiSelectMode(FALSE); // this is the default, but seems to be getting turned on somehow, not on purpose by our code
   this->Select(col, row);
   this->RedrawAll(); // maybe for speed should get old selection and just redraw that row

   EmptyMenu();

   // Insert Before and After are always allowed
   this->AddMenuItem(ID_DataDoctorCommand_OutlineInsertBefore, "Insert Vertex Before");
   this->AddMenuItem(ID_DataDoctorCommand_OutlineInsertAfter,  "Insert Vertex After");

   // Delete is allowed only if here are 4 or more vertices.
   // I.e. min size for a closed poly is 3 pts.
   // If deleting first or last then there that doubles the delete, so must be at least 5 pts in that case.
   bool firstOrLastPnt = (row == 0 || row == (GetNumberRows() - 1));
   int minPntCount = firstOrLastPnt ? 4 : 3;
   if (GetNumberRows() > minPntCount)
      this->AddMenuItem(ID_DataDoctorCommand_OutlineDeleteVertex, "Delete Vertex");

   // Bulge
   // Not allowed on last pt (cuz next is first pt, i.e. same pt)
   if (row != GetNumberRows()-1)
      this->AddMenuItem(ID_DataDoctorCommand_OutlineCurveToNext, "Curve To Next");

	return TRUE;
}
