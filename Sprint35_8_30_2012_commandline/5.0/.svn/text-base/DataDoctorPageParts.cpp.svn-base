// $Header: /CAMCAD/4.6/DataDoctorPageParts.cpp 13    5/03/07 5:44p Rick Faltersack $

#include "stdafx.h"
#include "PopupMenu.h"
#include "Response.h"

#include "DataDoctorDialog.h"
#include "DataDoctorPageParts.h"

#include "ODBC_Lib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



IMPLEMENT_DYNAMIC(CDataDoctorPartsPage, CDataDoctorPropertyPageType)

CDataDoctorPartsPage::CDataDoctorPartsPage(CDataDoctorPropertySheet& parent)
	: CDataDoctorPropertyPageType(CDataDoctorPartsPage::IDD),
     m_parent(parent)
     , m_partsGrid(this)
     , m_overwriteValues(0)  
	  , m_storeOption(1)  // Case 2058, init to "save not in database"
     , m_columnDefMap("DD Parts Grid")
     , m_updatePending(false)
     , m_dialogIsInitialized(false)
{

   DefineGridColumnMap();

#if defined (DataDoctorResizingDialogFlag)
   addFieldControl(IDC_PartsGridStatic        ,anchorLeft,growBoth);

   addFieldControl(IDC_LoadButton             ,anchorBottomLeft);
   addFieldControl(IDC_OverwriteValuesCheckBox,anchorBottomLeft);

   addFieldControl(IDC_ActiveLibraryGroup     ,anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_StoreButton            ,anchorBottomLeft);
   addFieldControl(IDC_StoreAllOption         ,anchorBottomLeft);
   addFieldControl(IDC_StoreNotInLibraryOption,anchorBottomLeft);
#endif

   //CDebugWriteFormat::setFilePath("c:\\DataDoctorDebug.txt");
}

/******************************************************************************
*/
void CDataDoctorPartsPage::EnableDatabaseControls(bool flag)
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
void CDataDoctorPartsPage::DefineGridColumnMap()
{
   // Set up the column definitions.
   // Default column order left to right is order they get defined here.
   m_columnDefMap.empty();

   m_columnDefMap.AddColDef( QFromLibrary, &m_colFromLibrary );
   m_columnDefMap.AddColDef( QPartNumber, &m_colPartNumber );
   m_columnDefMap.AddColDef( QStatus, &m_colStatus );
   m_columnDefMap.AddColDef( QDeviceType, &m_colDeviceType );
   m_columnDefMap.AddColDef( QValue, &m_colValue );
   m_columnDefMap.AddColDef( QPTolerance, &m_colPTolerance );
   m_columnDefMap.AddColDef( QNTolerance, &m_colNTolerance );
   m_columnDefMap.AddColDef( QPinCount, &m_colNumPins );
   m_columnDefMap.AddColDef( QPinMap, &m_colPinMap );
   m_columnDefMap.AddColDef( QSubclass, &m_colSubclass );
   m_columnDefMap.AddColDef( QDescription, &m_colDescription );
   m_columnDefMap.AddColDef( QComments, &m_colComments );
   m_columnDefMap.AddCustomAttributes(m_customColumnMap_PartGrid, this->getParent().getDataDoctor().GetPartLibraryAttribMap());
   m_columnDefMap.AddColDef( QErrors, &m_colErrors );
   m_columnDefMap.AddColDef( QRefDes, &m_colRefDes );

   // Load possible user overrides (column swaps)
   m_columnDefMap.LoadSettings();

}


CDataDoctorPartsPage::~CDataDoctorPartsPage()
{
}

BEGIN_MESSAGE_MAP(CDataDoctorPartsPage, CDataDoctorPropertyPageType)
   ON_BN_CLICKED(IDC_LoadButton, OnBnClickedLoadFromLibrary)
   ON_BN_CLICKED(IDC_StoreButton, OnBnClickedStoreToLibrary)
   ON_COMMAND(ID_DataDoctorCommand_LoadPart, OnLoadPart)
   ON_COMMAND(ID_DataDoctorCommand_SavePart, OnSavePart)
   ON_MESSAGE(ID_GRIDROWEDITFINISHED, OnRowEditFinished)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDataDoctorPartsPage, CDataDoctorPropertyPageType)
END_EVENTSINK_MAP()

CCamCadDatabase& CDataDoctorPartsPage::getCamCadDatabase() 
{ 
   return getParent().getCamCadDatabase(); 
}

void CDataDoctorPartsPage::DoDataExchange(CDataExchange* pDX)
{
	CDataDoctorPropertyPageType::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PartsGridStatic, m_partsGrid);
	DDX_Check(pDX, IDC_OverwriteValuesCheckBox, m_overwriteValues);
	DDX_Radio(pDX, IDC_StoreAllOption, m_storeOption);
}


void CDataDoctorPartsPage::setRowStyle(int rowIndx,CDataDoctorPart& part)
{
   switch (part.getStatus())
   {
   case dataDoctorStatusOk:       
      m_partsGrid.SetRowBackColor(rowIndx,                m_partsGrid.m_greenBackgroundColor);
      m_partsGrid.QuickSetTextColor(m_colStatus, rowIndx, colorDkGreen);
      break;
   case dataDoctorStatusError:    
      m_partsGrid.SetRowBackColor(rowIndx,                m_partsGrid.m_redBackgroundColor);
      m_partsGrid.QuickSetTextColor(m_colStatus, rowIndx, colorRed);
      break;
   case dataDoctorStatusIgnore:   
      m_partsGrid.SetRowBackColor(rowIndx,                m_partsGrid.m_lightGrayBackgroundColor);
      m_partsGrid.QuickSetTextColor(m_colStatus, rowIndx, colorDkYellow);
      break;
   }

   CString fromLibVal( m_partsGrid.QuickGetText(m_colFromLibrary, rowIndx) );
	if (fromLibVal.CompareNoCase(QFromLibraryExist) == 0)
	{
      m_partsGrid.QuickSetTextColor(m_colFromLibrary, rowIndx, colorRed);
	}
	else if (fromLibVal.CompareNoCase(QFromLibraryLoaded) == 0)
	{
      m_partsGrid.QuickSetTextColor(m_colFromLibrary, rowIndx, colorDkGreen);
	}

	setRowStyleForPinMap(rowIndx, part);
}

void CDataDoctorPartsPage::setRowStyleForPinMap(int rowIndx,CDataDoctorPart& part)
{
   CString pinMapVal( m_partsGrid.QuickGetText(m_colPinMap, rowIndx) );

	if (pinMapVal.CompareNoCase(QPinMapUnmap) == 0)
	{
      m_partsGrid.QuickSetTextColor(m_colPinMap, rowIndx, colorRed);
	}
	else if (pinMapVal.CompareNoCase(QPinMapMapped) == 0)
	{
      m_partsGrid.QuickSetTextColor(m_colPinMap, rowIndx, colorDkGreen);
	}
}

void CDataDoctorPartsPage::fillGrid()
{
   if (!m_partsGrid.IsSetUp())
      return;

   CWaitCursor pardonMeThisMightTakeAMoment;

   CDataDoctorParts& parts         = getParent().getDataDoctor().getParts();
   CCamCadDatabase& camCadDatabase = getParent().getDataDoctor().getCamCadDatabase();

   parts.sync(camCadDatabase,getParent().getDataDoctor().getComponents(), getParent().getDataDoctor().GetPartLibraryAttribMap());

   m_partsGrid.RemoveAllRows();
   m_partsGrid.SetNumberRows(parts.GetCount() - parts.GetDiePartCount());

   CDataDoctorPart* part;
   int rowIndx = 0;
   int index;

   for (parts.rewind(index);parts.next(part,index);)
   {
      if (part->isLoaded(camCadDatabase) && !part->IsDiePart())
      {
         part->calculateStatus(camCadDatabase);

         m_partsGrid.SetRowData(rowIndx,(void*)part);

         m_partsGrid.QuickSetTextAndColor( m_colFromLibrary, rowIndx    ,part->getFromLibraryString(),                         colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colPartNumber, rowIndx     ,part->getPartNumber(),                                colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colStatus, rowIndx         ,part->getStatusString(),                              colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colDeviceType, rowIndx     ,part->getDeviceType(camCadDatabase),                  colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colValue, rowIndx          ,part->getDisplayValue(camCadDatabase),                colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colPTolerance, rowIndx     ,part->getNormalizedPTolerance(camCadDatabase),        colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colNTolerance, rowIndx     ,part->getNormalizedNTolerance(camCadDatabase),        colorBlack);
         m_partsGrid.QuickSetNumber( m_colNumPins, rowIndx      ,part->getPinCount());
         m_partsGrid.QuickSetTextAndColor( m_colPinMap, rowIndx         ,part->getPinMapStatus(),                              colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colSubclass, rowIndx       ,part->getSubclass(camCadDatabase),                    colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colDescription, rowIndx    ,part->getDescription(camCadDatabase),                 colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colErrors, rowIndx         ,part->getErrors(),                                    colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colRefDes, rowIndx         ,part->getLoadedReferenceDesignators(camCadDatabase),  colorBlack);
         m_partsGrid.QuickSetTextAndColor( m_colComments, rowIndx       ,part->getPartComments(camCadDatabase),  colorBlack);


         // User defined custom attribs
         POSITION customPos = this->m_customColumnMap_PartGrid.GetStartPosition();
         while (customPos != NULL)
         {
            CString customColName;
            CDDSpecialColumn *custCol = NULL;
            this->m_customColumnMap_PartGrid.GetNextAssoc(customPos, customColName, custCol);
            CString cellval = part->getGenericAttrib(camCadDatabase, custCol->GetName());
            m_partsGrid.QuickSetTextAndColor(custCol->GetColumnIndexRef(), rowIndx, cellval, colorBlack);
         }

         setRowStyle(rowIndx,*part);

         rowIndx++;
      }
   }

   m_partsGrid.ReSort(QPartNumber);
   m_partsGrid.BestFit(0, m_partsGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
}

void CDataDoctorPartsPage::update()
{
   // Potentially need to redefine grid due to custom attrib set changes
   // Check if attribs changed to avoid needless full update
   if (true)
   {
      DefineGridColumnMap();
      if (this->m_partsGrid.IsSetUp())
         this->m_partsGrid.OnSetup();  // re-setup
   }

   fillGrid();
   m_updatePending = false;
}

CString CDataDoctorPartsPage::getDeviceTypeComboList()
{
   if (m_deviceTypeComboList.IsEmpty())
   {
      CString deviceTypeString;
      DeviceTypeTag deviceType;

      for (deviceType = (DeviceTypeTag)0;deviceType < deviceTypeUndefined;
             deviceType = (DeviceTypeTag)(deviceType + 1))
      {
         deviceTypeString = deviceTypeTagToFriendlyString(deviceType);

         m_deviceTypeComboList += deviceTypeString + "\n";
      }
   }

   return m_deviceTypeComboList;
}

void CDataDoctorPartsPage::updatePartInGrid(long row,long col,CString value)
{
   int nr = m_partsGrid.GetNumberRows();
   if (row >= 0 && row < m_partsGrid.GetNumberRows())
   {
      bool resync = false;
      CDataDoctorPart* part = (CDataDoctorPart*)m_partsGrid.GetRowData(row);
      CString normalizedStringValue(value);

      if (col == m_colValue)
      {
         ComponentValues componentValue(value,getDefaultValueUnitForDeviceType(part->getDeviceTypeTag()));

         if (componentValue.IsValid())
         {
            normalizedStringValue = componentValue.GetPrintableString();
         }

         part->setValue(getCamCadDatabase(),normalizedStringValue);
      }
      else if (col == m_colPTolerance)
      {
         part->setPTolerance(getCamCadDatabase(),value);

         if (! value.IsEmpty())
         {
            normalizedStringValue = part->getNormalizedPTolerance(getCamCadDatabase());
         }
      }
      else if (col == m_colNTolerance)
      {
         part->setNTolerance(getCamCadDatabase(),value);

         if (! value.IsEmpty())
         {
            normalizedStringValue = part->getNormalizedNTolerance(getCamCadDatabase());
         }
      }
      else if (col == m_colPartNumber)
      {
         // Updating the part number causes a problem. The parts are in the Data Doc's
         // parts list, which is indexed by part number. So we have changed where this
         // part should be stored in teh list, and made the current list invalid since
         // the old part number index points to this thing with a new part number.
         // And on top of that, the parts list is a container, so if we use the
         // standard calls here ot manipulate the list (e.g. removeKey()) we will
         // delete the part we are using here and now. 
         // SO it seems the best thing to do is just re-sync the list. But first,
         // let all the updates that happen here finish happening. Just set a 
         // flag here to cause resync at the end.

         part->setPartNumber(getCamCadDatabase(), value);
         resync = true;
      }
      else if (col == m_colSubclass)
      {
         part->setSubclass(getCamCadDatabase(),value);
      }
      else if (col == m_colDeviceType)
      {
         part->setDeviceType(getCamCadDatabase(),value);
      }
		else if (col == m_colDescription)
		{
			part->setDescription(getCamCadDatabase(),value);
		}
		else if (col == m_colComments)
		{
			part->setPartComments(getCamCadDatabase(),value);
		}
      else if (this->IsCustomPartGridColumn(col))
      {
         CDDSpecialColumn *usercol = m_customColumnMap_PartGrid.GetSpecialColumn(col);
         if (usercol != NULL)
            part->setGenericAttrib(getCamCadDatabase(), usercol->GetName(), value);
      }

      m_partsGrid.QuickSetText(col, row, normalizedStringValue);

      part->calculateStatus(getCamCadDatabase());

      m_partsGrid.QuickSetText(m_colPinMap, row, part->getPinMapStatus()); 
      m_partsGrid.QuickSetText(m_colStatus, row, part->getStatusString());
      m_partsGrid.QuickSetText(m_colErrors, row, part->getErrors());

      setRowStyle(row,*part);
      m_partsGrid.BestFit(0, m_partsGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      m_partsGrid.RedrawRow(row);

      if (resync)
         fillGrid();  // will resync and refill the grid
   }     
}

LRESULT CDataDoctorPartsPage::OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   AfterEditPartsGrid(wParamROW, lParamCOL);

   return 0;

}

BOOL CDataDoctorPartsPage::OnSetActive()
{
   getParent().SaveAsInitialPage(DdPartsPage);

   if (IsUpdatePending())
      update();

   return CDataDoctorPropertyPageType::OnSetActive();
}

BOOL CDataDoctorPartsPage::OnInitDialog()
{
   CDataDoctorPropertyPageType::OnInitDialog();

   CWaitCursor pardonMeThisMightTakeAMoment;

   m_partsGrid.AttachGrid(this, IDC_PartsGridStatic);

   fillGrid();
	
   CDataDoctorParts& parts = getParent().getDataDoctor().getParts();
   CDataDoctorLibraryAttribMap& activeAttribMap = getParent().getDataDoctor().GetPartLibraryAttribMap();
   parts.loadPartDataFromLibrary(getCamCadDatabase(), activeAttribMap, m_overwriteValues != 0, false);
   update();
   getParent().updatePropertyPages(this);

   getParent().EnableDisableDatabaseControls();

   m_dialogIsInitialized = true;

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CDataDoctorPartsPage::OnLoadPart()
{
   int row = m_partsGrid.GetCurrentRow();
   CString partnumber = m_partsGrid.QuickGetText(m_colPartNumber, row);

   CDataDoctorParts& parts = getParent().getDataDoctor().getParts();
   CDataDoctorLibraryAttribMap& activeAttribMap = getParent().getDataDoctor().GetPartLibraryAttribMap();
   CDataDoctorPart *part = parts.findPart(partnumber);
   bool retval = parts.loadOnePartFromLibrary(getCamCadDatabase(), activeAttribMap, part, true /*overwriteCamcad*/, false /*displayMsg*/);

   update();
   getParent().updatePropertyPages(this);

	if (!retval)
		formatMessageBox(MB_ICONASTERISK,"Load Part from Library did not find a matching Part");

   m_partsGrid.RedrawRow(row);
}

void CDataDoctorPartsPage::OnSavePart()
{
   int row = m_partsGrid.GetCurrentRow();
   CString partnumber = m_partsGrid.QuickGetText(m_colPartNumber, row);

   CDataDoctorParts& parts = getParent().getDataDoctor().getParts();
   CDataDoctorPart *part = parts.findPart(partnumber);
   parts.storeOnePartToLibrary(getCamCadDatabase(), part);
}

void CDataDoctorPartsPage::OnBnClickedLoadFromLibrary()
{
   bool returnValue = false;
	UpdateData(true);

   CWaitCursor pardonMeThisMightTakeAMoment;

   CDataDoctorParts& parts = getParent().getDataDoctor().getParts();
   CDataDoctorLibraryAttribMap& activeAttribMap = getParent().getDataDoctor().GetPartLibraryAttribMap();

   returnValue = parts.loadPartDataFromLibrary(getCamCadDatabase(), activeAttribMap, m_overwriteValues != 0, true);
	
   update();
   getParent().updatePropertyPages(this);

	if (!returnValue)
		formatMessageBox(MB_ICONASTERISK,"Load All Parts from Library did not find any matching Parts");
}

void CDataDoctorPartsPage::OnBnClickedStoreToLibrary()
{
   UpdateData(true);
   CDataDoctorParts& parts = getParent().getDataDoctor().getParts();

   CWaitCursor pardonMeThisMightTakeAMoment;
	parts.storePartDataToLibrary(getCamCadDatabase(),(m_storeOption==0)?true:false);
}

afx_msg void CDataDoctorPartsPage::OnSize(UINT nType, int cx, int cy)
{
   CDataDoctorPropertyPageType::OnSize(nType,cx,cy);

   // probably don't need this anymore, it was part of kludge to make flexgrid size tolerable
}

void CDataDoctorPartsPage::CellButtonClickOnGrid(long Row, long Col)
{
	if (Col == m_colPinMap)
	{
      CDataDoctorPart* part = (CDataDoctorPart*)m_partsGrid.GetRowData(Row);

      CDataDoctorPinMappingDlg pinMapDlg(getParent().getCamCadDatabase(), part);
		
      if (pinMapDlg.DoModal() == IDOK)
      {
         CString errMsg;
		   part->setPinRefToPinNamesMapString(getCamCadDatabase(), pinMapDlg.getPinRefsToPinNamesMapString(), errMsg);
         part->calculateStatus(getCamCadDatabase());

         m_partsGrid.QuickSetText(m_colPinMap, Row, part->getPinMapStatus());
         m_partsGrid.QuickSetText(m_colStatus, Row, part->getStatusString());
         m_partsGrid.QuickSetText(m_colErrors, Row, part->getErrors());

		   setRowStyle(Row,*part);
		   m_partsGrid.BestFit(0, m_partsGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);

		   getParent().updatePropertyPages(this);
      }
      // Not sure what is up, but if popup (above) is resized, it sometimes leaves a gray 
      // shadow after it is closed. This redraw seems to clear it up.
      getParent().RedrawWindow();
	}
}

void CDataDoctorPartsPage::AfterRowColChangePartsGrid(long oldRow, long oldCol, long newRow, long newCol)
{
   // No op
}

void CDataDoctorPartsPage::AfterEditPartsGrid(long row, long col)
{
   if (row >= 0 && row < m_partsGrid.GetNumberRows())
   {
      CString value = m_partsGrid.QuickGetText(col, row);

      updatePartInGrid(row,col,value);

      // Process any multiple selected cells
      int firstTargetCol;
      long firstTargetRow;
      int endTargetCol;
      long endTargetRow;

      if (m_partsGrid.GetMultiSelectStart(&firstTargetCol, &firstTargetRow) &&
         m_partsGrid.GetMultiSelectEnd(&endTargetCol, &endTargetRow))
      {
         for (long targetRow = min(firstTargetRow, endTargetRow); targetRow <= max(firstTargetRow, endTargetRow); targetRow++)
         {
            if (targetRow != row) // "row" is already updated, don't need to do it twice
            {
               updatePartInGrid(targetRow, firstTargetCol, value);
            }
         }
      }

      m_partsGrid.ClearSelections();
      m_partsGrid.BestFit(0, m_partsGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      getParent().updatePropertyPages(this);
   }
}


void CDataDoctorPartsPage::OnRButtonDown(CPoint windowRelativePoint)
{
   int row = m_partsGrid.GetCurrentRow();
   CString partnumber = m_partsGrid.QuickGetText(m_colPartNumber, row);

   CPopupMenu popupMenu;
   CString option;

   option.Format("Load %s from Library", partnumber);
   popupMenu.addItem(ID_DataDoctorCommand_LoadPart, option);

   option.Format("Save %s to Library", partnumber);
   popupMenu.addItem(ID_DataDoctorCommand_SavePart, option);

   popupMenu.trackMenu(TPM_LEFTALIGN,windowRelativePoint,this);
}

void CDataDoctorPartsPage::MouseDownGrid(short Button, short Shift, float X, float Y)
{
   if (Button == 2) // right button
   {
      CRect winrect;
      GetWindowRect(&winrect);

      CPoint cursorpos;
      GetCursorPos(&cursorpos);
         
      CPoint windcorner(winrect.left, winrect.top);
      CPoint windowRelativeCursorPoint = cursorpos - windcorner;

      OnRButtonDown(windowRelativeCursorPoint);
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
void CDDPartsGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	

   this->SetMultiSelectMode(TRUE);
   if (this->m_partsPage != NULL)
   {
      SetNumberCols(m_partsPage->m_columnDefMap.getSize());
      SetNumberRows(0);
      
      for (int indx = 0; indx < m_partsPage->m_columnDefMap.getSize(); indx++)
      {
         CColumnDef *cd = m_partsPage->m_columnDefMap.getAt(indx);
         CString colname = cd->GetTitle();
         int *colindxptr = cd->GetColIndexPtr();

         QuickSetText(*colindxptr, -1, colname);

         // This might not be the first call to OnSetup, so the grid may have lingering
         // non-default settings. Clear them out.
         CUGCell colDefCell;
         GetColDefault(*colindxptr, &colDefCell); 
         colDefCell.SetCellType(UGCT_NORMAL); 
         colDefCell.SetAlignment(UG_ALIGNLEFT);
         colDefCell.SetTextColor(colorBlack);
         SetColDefault(*colindxptr, &colDefCell);
      }


      // Device Type option menu, same for all cells in column
      CUGCell cell;
      GetColDefault(this->m_partsPage->m_colDeviceType, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      CString devlist(m_partsPage->getDeviceTypeComboList());
      cell.SetLabelText( devlist ); 
      SetColDefault(this->m_partsPage->m_colDeviceType, &cell);

      // Right justify some cols, to match old flex-grid setup
      GetColDefault(this->m_partsPage->m_colPartNumber, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(this->m_partsPage->m_colPartNumber, &cell);

      GetColDefault(this->m_partsPage->m_colPTolerance, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(this->m_partsPage->m_colPTolerance, &cell);

      GetColDefault(this->m_partsPage->m_colNTolerance, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(this->m_partsPage->m_colNTolerance, &cell);

      GetColDefault(this->m_partsPage->m_colNumPins, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(this->m_partsPage->m_colNumPins, &cell);

      EnableColSwapping(TRUE);

      BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
      SetColWidth(-1, 0); // get rid of "row heading"

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

int CDDPartsGrid::OnEditStart(int col, long row, CWnd **edit)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(**edit);

   *edit = &m_myCUGEdit;

   if (col == m_partsPage->m_colPartNumber)
      return true;
   else if (col == m_partsPage->m_colValue)
      return true;
   else if (col == m_partsPage->m_colPTolerance)
      return true;
   else if (col == m_partsPage->m_colNTolerance)
      return true;
   else if (col == m_partsPage->m_colSubclass)
      return true;
   else if (col == m_partsPage->m_colPinMap)
   {
      this->m_partsPage->CellButtonClickOnGrid(row, col);
      this->m_partsPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);
      return false;
   }
   else if (col == m_partsPage->m_colDeviceType)
      return false;
   else if (col == m_partsPage->m_colDescription)
      return true;
   else if (m_partsPage->IsCustomPartGridColumn(col))
      return true;
   else if (col == m_partsPage->m_colComments)
      return true;


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
int CDDPartsGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
   if (col == m_partsPage->m_colPTolerance ||
      col == m_partsPage->m_colNTolerance)
   {
      return this->OnEditVerifyPercentCell(col, row, edit, vcKey);
   }

   // All others
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
int CDDPartsGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   BOOL ddd2 = this->m_partsPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);


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
int CDDPartsGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(param);

   //if (msg == UGCT_DROPLISTSELECT)
   //{
   //   do something
   //}

   // Handy msg code that is not in the html help files
   // UGCT_DROPLISTPOSTSELECT
   if (msg == UGCT_DROPLISTPOSTSELECT)
   {
      this->m_partsPage->AfterEditPartsGrid(row, col);
   }
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCellChange
//		Sent whenever the current cell changes
//	Params:
//		oldcol, oldrow - coordinates of cell that is loosing the focus
//		newcol, newrow - coordinates of cell that is gaining the focus
//	Return:
//		<none>
void CDDPartsGrid::OnCellChange(int oldcol,int newcol,long oldrow,long newrow)
{
	//UNREFERENCED_PARAMETER(oldcol);
	//UNREFERENCED_PARAMETER(newcol);
	//UNREFERENCED_PARAMETER(oldrow);
	//UNREFERENCED_PARAMETER(newrow);

   this->m_partsPage->AfterRowColChangePartsGrid(oldrow, oldcol, newrow, newcol);
}

		
/////////////////////////////////////////////////////////////////////////////
//	OnRClicked
//		Sent whenever the user clicks the right mouse button within the grid
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void CDDPartsGrid::OnRClicked(int col,long row,int updn,RECT *rect,POINT *point,int processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);

   if (updn == TRUE) // button down
   {
      // Works fine, but is not using the Ult Grid support for menu. (SO what?)
      this->m_partsPage->MouseDownGrid(2, 0, 0, 0);
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
void CDDPartsGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   this->m_partsPage->m_columnDefMap.Update(this);

}
