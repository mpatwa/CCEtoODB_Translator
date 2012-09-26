// $Header: /CAMCAD/4.6/DataDoctorPageComps.cpp 14    4/11/07 1:50p Rick Faltersack $

#include "stdafx.h"

#include "DataDoctorDialog.h"
#include "DataDoctorPageComps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CDataDoctorComponentsPage, CDataDoctorPropertyPageType)

CDataDoctorComponentsPage::CDataDoctorComponentsPage(CDataDoctorPropertySheet& parent)
	: CDataDoctorPropertyPageType(CDataDoctorComponentsPage::IDD)
    , m_parent(parent)
    , m_componentsGrid(this)
    , m_toolbar(*this)
    , m_updatePending(false)
{

#if defined (DataDoctorResizingDialogFlag)
   addFieldControl(IDC_ComponentsGridStatic,anchorLeft,growBoth);

   CDebugWriteFormat::setFilePath("c:\\DataDoctorDebug.txt");
#endif
}

CDataDoctorComponentsPage::~CDataDoctorComponentsPage()
{
}


BEGIN_MESSAGE_MAP(CDataDoctorComponentsPage, CDataDoctorPropertyPageType)
   ON_WM_ACTIVATE()
   ON_MESSAGE(ID_GRIDROWEDITFINISHED, OnRowEditFinished)
   ON_BN_CLICKED(ID_VL_ZOOM_ZOOMIN,       OnTbBnClickedZoomIn)
   ON_BN_CLICKED(ID_VL_COLORSETS_CUSTOM,  OnTbBnClickedColorsetsCustom)
   ON_BN_CLICKED(ID_LAYERTYPES,           OnTbBnClickedLayerTypes)
   ON_BN_CLICKED(ID_SHOW_FILLS,           OnTbBnClickedShowPolyFill)
   ON_BN_CLICKED(ID_SHOW_AP_FILLS,        OnTbBnClickedShowApertureFill)
END_MESSAGE_MAP()


CCamCadDatabase& CDataDoctorComponentsPage::getCamCadDatabase() 
{ 
   return getParent().getCamCadDatabase(); 
}

void CDataDoctorComponentsPage::DoDataExchange(CDataExchange* pDX)
{
   CDataDoctorPropertyPageType::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ComponentsGridStatic, m_componentsGrid);
}

void CDataDoctorComponentsPage::setRowStyleFromStatus(int rowIndx, CDataDoctorComponent& component)
{
   //*rcf need to mess with fonts to set BOLD

   switch (component.getStatus())
   {
   case dataDoctorStatusOk:       
      m_componentsGrid.SetRowBackColor(rowIndx,                m_componentsGrid.m_greenBackgroundColor);
      m_componentsGrid.QuickSetTextColor(m_componentsGrid.m_colStatus, rowIndx, colorDkGreen);
      break;
   case dataDoctorStatusError:    
      m_componentsGrid.SetRowBackColor(rowIndx,                m_componentsGrid.m_redBackgroundColor);
      m_componentsGrid.QuickSetTextColor(m_componentsGrid.m_colStatus, rowIndx, colorRed);
      break;
   case dataDoctorStatusIgnore:   
      m_componentsGrid.SetRowBackColor(rowIndx,                m_componentsGrid.m_lightGrayBackgroundColor);
      m_componentsGrid.QuickSetTextColor(m_componentsGrid.m_colStatus, rowIndx, colorDkYellow);
      break;
   }
}

void CDataDoctorComponentsPage::fillGrid()
{
   if (!m_componentsGrid.IsSetUp())
      return;

   CWaitCursor pardonMeThisMightTakeAMoment;

   CDataDoctorComponents& components = getParent().getDataDoctor().getComponents();
   CCamCadDatabase& camCadDatabase   = getParent().getDataDoctor().getCamCadDatabase();
   CDataDoctorComponent* component;
   m_componentsGrid.RemoveAllRows();
   m_componentsGrid.SetNumberRows(components.GetCount() - components.GetDieCount(), FALSE /*no redraw*/);

   int rowIndx = 0;
   int index;

   for (components.rewind(index);components.next(component,index);)
   {
      if(!component->IsDie())
      {
         component->calculateStatus(camCadDatabase);

         m_componentsGrid.SetRowData(rowIndx, component);

         m_componentsGrid.QuickSetText( m_componentsGrid.m_colRefDes, rowIndx         ,component->getRefDes());
         m_componentsGrid.QuickSetParam( m_componentsGrid.m_colRefDes, rowIndx        ,m_componentsGrid.DD_CELLTYPE_REFNAME);
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colStatus, rowIndx         ,component->getStatusString());
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colPartNumber, rowIndx     ,component->getPartNumber(camCadDatabase));
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colDeviceType, rowIndx     ,component->getDeviceType(camCadDatabase));
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colValue , rowIndx         ,component->getDisplayValue(camCadDatabase));
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colPTolerance, rowIndx     ,component->getNormalizedPTolerance(camCadDatabase));
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colNTolerance, rowIndx     ,component->getNormalizedNTolerance(camCadDatabase));
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colCapacitiveOpens, rowIndx, component->getDefinedCapacitiveOpens(camCadDatabase));
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colDiodeOpens, rowIndx     ,component->getDefinedDiodeOpens(camCadDatabase));
         m_componentsGrid.QuickSetNumber( m_componentsGrid.m_colNumPins, rowIndx      ,component->getPinCount());
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colSubclass, rowIndx       ,component->getSubclass(camCadDatabase));
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colLoaded, rowIndx         ,component->getLoaded(camCadDatabase) ? "True" : "False");
         m_componentsGrid.QuickSetText( m_componentsGrid.m_colErrors, rowIndx         ,component->getErrors());

         setRowStyleFromStatus(rowIndx, *component);
         
         rowIndx++;
      }
   }

   m_componentsGrid.ReSort(QRefDes);
   m_componentsGrid.BestFit(0, m_componentsGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);

}

void CDataDoctorComponentsPage::update()
{
   fillGrid();
   m_updatePending = false;
}

CString CDataDoctorComponentsPage::getDeviceTypeComboList()
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

void CDataDoctorComponentsPage::updateComponentInGrid(long row,long col,CString value)
{
   if (row >= 0 && row < m_componentsGrid.GetNumberRows())
   {
      CDataDoctorComponent* component = (CDataDoctorComponent*)m_componentsGrid.GetRowData(row);
      CString normalizedStringValue(value);

      if (col == m_componentsGrid.m_colPartNumber)
      {
         component->setPartNumber(getCamCadDatabase(),value);
      }
      else if (col == m_componentsGrid.m_colValue)
      {
         ComponentValues componentValue(value,getDefaultValueUnitForDeviceType(component->getDeviceTypeTag()));

         if (componentValue.IsValid())
         {
            normalizedStringValue = componentValue.GetPrintableString();
         }

         component->setValue(getCamCadDatabase(),normalizedStringValue);
      }
      else if (col == m_componentsGrid.m_colPTolerance)
      {
         component->setPTolerance(getCamCadDatabase(),value);

         if (! value.IsEmpty())
         {
            normalizedStringValue = component->getNormalizedPTolerance(getCamCadDatabase());
         }
      }
      else if (col == m_componentsGrid.m_colNTolerance)
      {
         component->setNTolerance(getCamCadDatabase(),value);

         if (! value.IsEmpty())
         {
            normalizedStringValue = component->getNormalizedNTolerance(getCamCadDatabase());
         }
      }
      else if (col == m_componentsGrid.m_colCapacitiveOpens)
      {
         component->setCapacitiveOpens(getCamCadDatabase(),value);
      }
      else if (col == m_componentsGrid.m_colDiodeOpens)
      {
         component->setDiodeOpens(getCamCadDatabase(),value);
      }
      else if (col == m_componentsGrid.m_colSubclass)
      {
         component->setSubclass(getCamCadDatabase(),value);
      }
      else if (col == m_componentsGrid.m_colLoaded)
      {
         component->setLoaded(getCamCadDatabase(),value);
      }
      else if (col == m_componentsGrid.m_colDeviceType)
      {
         component->setDeviceType(getCamCadDatabase(),value);
      }

      m_componentsGrid.QuickSetText(col, row, normalizedStringValue);

      component->calculateStatus(getCamCadDatabase());

      m_componentsGrid.QuickSetText(m_componentsGrid.m_colStatus, row, component->getStatusString());
      m_componentsGrid.QuickSetText(m_componentsGrid.m_colErrors, row, component->getErrors());

      setRowStyleFromStatus(row, *component);
   }
}

BOOL CDataDoctorComponentsPage::OnSetActive()
{
   getParent().SaveAsInitialPage(DdComponentsPage);

   if (IsUpdatePending())
      update();

   return CDataDoctorPropertyPageType::OnSetActive();
}

BOOL CDataDoctorComponentsPage::OnInitDialog()
{
   m_toolbar.createAndLoad(IDR_ColorsLiteToolbar);

   CDataDoctorPropertyPageType::OnInitDialog();

   m_componentsGrid.AttachGrid(this, IDC_ComponentsGridStatic);  

   fillGrid();

   return TRUE;  // return TRUE unless you set the focus to a control
}

afx_msg void CDataDoctorComponentsPage::OnSize(UINT nType, int cx, int cy)
{
   CDataDoctorPropertyPageType::OnSize(nType,cx,cy);
}

void CDataDoctorComponentsPage::FixPan()
{
   int row = this->m_componentsGrid.GetCurrentRow();

   if (row >= 0 && row < m_componentsGrid.GetNumberRows())
   {
      CDataDoctorComponent* component = (CDataDoctorComponent*)m_componentsGrid.GetRowData(row);

      CCEtoODBView* view = getActiveView();
      if (component != NULL && view != NULL)
      {
         PointStruct origin = component->getOrigin();
         view->PanCoordinate(origin.x, origin.y, FALSE);
      }
   }
}

void CDataDoctorComponentsPage::AfterRowColChangeComponentsGrid(long oldRow, long oldCol, long newRow, long newCol)
{
   if (newRow >= 0 && newRow < m_componentsGrid.GetNumberRows() && newCol == m_componentsGrid.m_colRefDes)
   {
      CDataDoctorComponent* component = (CDataDoctorComponent*)m_componentsGrid.GetRowData(newRow);

      if (component != NULL)
      {
         CString refDes = component->getRefDes();
         getCamCadDatabase().getCamCadDoc().zoomToComponent(refDes);
      }
   }
}

void CDataDoctorComponentsPage::AfterEditComponentsGrid(long row, long col)
{
   if (row >= 0 && row < m_componentsGrid.GetNumberRows())
   {
      CString value = m_componentsGrid.QuickGetText(col, row);

      // Always just update the current cell
      updateComponentInGrid(row,col,value);
      //m_componentsGrid.RedrawRow(row);

      // Process any multiple selected cells
      int firstTargetCol;
      long firstTargetRow;
      int endTargetCol;
      long endTargetRow;

      if (m_componentsGrid.GetMultiSelectStart(&firstTargetCol, &firstTargetRow) &&
         m_componentsGrid.GetMultiSelectEnd(&endTargetCol, &endTargetRow))
      {
         for (long targetRow = min(firstTargetRow, endTargetRow); targetRow <= max(firstTargetRow, endTargetRow); targetRow++)
         {
            if (targetRow != row) // "row" is already updated, don't need to do it twice
            {
               updateComponentInGrid(targetRow, firstTargetCol, value);
             //  m_componentsGrid.RedrawRow(targetRow);
            }
         }
      }

      m_componentsGrid.ClearSelections();

      m_componentsGrid.BestFit(0, m_componentsGrid.GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);

      getParent().updatePropertyPages(this);
   }
}

void CDataDoctorComponentsPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CDataDoctorPropertyPageType::OnActivate(nState, pWndOther, bMinimized);

   // TODO: Add your message handler code here
}

LRESULT CDataDoctorComponentsPage::OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   AfterEditComponentsGrid(wParamROW, lParamCOL);

   return 0;

}

//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////
// 
// COMPONENTS GRID
//

CDDCompsGrid::CDDCompsGrid(CDataDoctorComponentsPage *compsPage)
: CDDBaseGrid("DD Components Grid")
{
   m_compsPage = compsPage;

   // Column definition map, maps column name to local column number index.
   // Order of definitions here is default left to right order of columns.
   m_columnDefMap.AddColDef( QRefDes, &m_colRefDes );
   m_columnDefMap.AddColDef( QStatus, &m_colStatus );
   m_columnDefMap.AddColDef( QPartNumber, &m_colPartNumber );
   m_columnDefMap.AddColDef( QDeviceType, &m_colDeviceType );
   m_columnDefMap.AddColDef( QValue, &m_colValue );
   m_columnDefMap.AddColDef( QPTolerance, &m_colPTolerance );
   m_columnDefMap.AddColDef( QNTolerance, &m_colNTolerance );
   m_columnDefMap.AddColDef( QCapacitiveOpens, &m_colCapacitiveOpens );
   m_columnDefMap.AddColDef( QDiodeOpens, &m_colDiodeOpens );
   m_columnDefMap.AddColDef( QPinCount, &m_colNumPins );
   m_columnDefMap.AddColDef( QSubclass, &m_colSubclass );
   m_columnDefMap.AddColDef( QLoaded, &m_colLoaded );
   m_columnDefMap.AddColDef( QErrors, &m_colErrors );
   // Load possible user overrides (column swaps)
   m_columnDefMap.LoadSettings();
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void CDDCompsGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.m_ctrl = this;	

   this->SetMultiSelectMode(TRUE);

   if (this->m_compsPage != NULL)
   {
      SetNumberCols(m_columnDefMap.getSize());
      SetNumberRows(0);
      
      QuickSetText(m_colRefDes,           -1, QRefDes);
      QuickSetText(m_colStatus,           -1, QStatus);
      QuickSetText(m_colPartNumber,       -1, QPartNumber);
      QuickSetText(m_colDeviceType,       -1, QDeviceType);
      QuickSetText(m_colValue,            -1, QValue);
      QuickSetText(m_colPTolerance,       -1, QPTolerance);
      QuickSetText(m_colNTolerance,       -1, QNTolerance);
      QuickSetText(m_colCapacitiveOpens,  -1, QCapacitiveOpens);
      QuickSetText(m_colDiodeOpens,       -1, QDiodeOpens);
      QuickSetText(m_colNumPins,          -1, QPinCount);
      QuickSetText(m_colSubclass,         -1, QSubclass);
      QuickSetText(m_colLoaded,           -1, QLoaded);
      QuickSetText(m_colErrors,           -1, QErrors);

      // Set up true/false option menus as default for these columns.
      // It strikes me a spinner that toggles the setting would be faster for user.
      // Or maybe just a check box.
      // But for now we are going for "true to the original", improvements later.
      CUGCell cell;

      GetColDefault(m_colCapacitiveOpens, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      cell.SetLabelText("True\nFalse\n"); 
      SetColDefault(m_colCapacitiveOpens, &cell);

      GetColDefault(m_colDiodeOpens, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      cell.SetLabelText("True\nFalse\n"); 
      SetColDefault(m_colDiodeOpens, &cell);

      GetColDefault(m_colLoaded, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      cell.SetLabelText("True\nFalse\n"); 
      SetColDefault(m_colLoaded, &cell);

      // Device Type option menu
      GetColDefault(m_colDeviceType, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      CString devlist(m_compsPage->getDeviceTypeComboList());
      cell.SetLabelText( devlist );
      SetColDefault(m_colDeviceType, &cell);

      // Right justify some cols, to match old flex-grid setup
      GetColDefault(m_colPartNumber, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(m_colPartNumber, &cell);

      GetColDefault(m_colPTolerance, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(m_colPTolerance, &cell);

      GetColDefault(m_colNTolerance, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(m_colNTolerance, &cell);

      GetColDefault(m_colNumPins, &cell);
      cell.SetAlignment(UG_ALIGNRIGHT);
      SetColDefault(m_colNumPins, &cell);


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

int CDDCompsGrid::OnEditStart(int col, long row, CWnd **edit)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(**edit);

   *edit = &m_myCUGEdit;


   if (col == m_colRefDes)
      return false;
   else if (col == m_colStatus)
      return false;
   else if (col == m_colPartNumber)
      return true;
   else if (col == m_colDeviceType)
      return false;
   else if (col == m_colValue)
      return true;
   else if (col == m_colPTolerance)
      return true;
   else if (col == m_colNTolerance)
      return true;
   else if (col == m_colCapacitiveOpens)
      return false;
   else if (col == m_colDiodeOpens)
      return false;
   else if (col == m_colNumPins)
      return false;
   else if (col == m_colSubclass)
      return true;
   else if (col == m_colLoaded)
      return false;//true;
   else if (col == m_colErrors)
      return false;

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
int CDDCompsGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
   if (col == m_colPTolerance ||
      col == m_colNTolerance)
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
int CDDCompsGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   if (!cancelFlag)
   {
      BOOL ddd2 = this->m_compsPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);
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
int CDDCompsGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(param);


   // Handy msg code that is not in the html help files
   // UGCT_DROPLISTPOSTSELECT
   if (msg == UGCT_DROPLISTPOSTSELECT)
   {
      this->m_compsPage->AfterEditComponentsGrid(row, col);
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
void CDDCompsGrid::OnCellChange(int oldcol,int newcol,long oldrow,long newrow)
{
	//UNREFERENCED_PARAMETER(oldcol);
	//UNREFERENCED_PARAMETER(newcol);
	//UNREFERENCED_PARAMETER(oldrow);
	//UNREFERENCED_PARAMETER(newrow);

   this->m_compsPage->AfterRowColChangeComponentsGrid(oldrow, oldcol, newrow, newcol);
}


/////////////////////////////////////////////////////////////////////////////
//	OnColSwapped
//		Called just after column-swap operation was completed.
//	Params:
//		fromCol - where the col originated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		<none>
void CDDCompsGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   //this->m_compsPage->m_columnDefMapOLD.Update(this);
   this->m_columnDefMap.Update(this);

}
