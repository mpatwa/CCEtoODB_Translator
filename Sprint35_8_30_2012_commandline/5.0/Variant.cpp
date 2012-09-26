
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "Ccdoc.h"
#include "File.h"
#include "mainfrm.h"
#include "DeviceType.h"
#include "Variant.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/******************************************************************************
* CCEtoODBDoc::OnVariantNavigator
*/
void CCEtoODBDoc::OnVariantNavigator()
{
	// TODO: Add your command handler code here
	FileStruct *file = getFileList().GetFirstShown(blockTypePcb);

   // DR 789417  Hack fix for 
   // 0xC0150010: The activation context being deactivated is not active for the current thread of execution.
   // Searched web for error message, found this solution on a web page.
   // Others commenting on solution say it isn't safe, and is just masking the real problem.
   // Since I can't find the real problem, let's go with the mask.
   // The page put the = FALSE setting just after InitInstance, so active for whole app.
   // I found it works well enough just encompassing this feature, so I think that minimizes the danger of it.
   // There is somethign wrong with vsflexgrid usage in this dialog, but can't find what it is yet.
   // If worked fine when built with vs2003, does not work when built with vs2008.
   BOOL oldAmbient = afxAmbientActCtx;
   afxAmbientActCtx = FALSE;

	if (file != NULL)
	{
		CVariantNavigatorDlg dlg(this, file);
		dlg.DoModal();
	}
	else
	{
		ErrorMessage("No visible PCB file detected.\n\nVariant Navigator requires a visible PCB file.", "Variant Navigator");
	}

   afxAmbientActCtx = oldAmbient;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CVariantUpdateDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CVariantUpdateDlg, CDialog)
CVariantUpdateDlg::CVariantUpdateDlg(CCEtoODBDoc* doc, FileStruct* file, CWnd* pParent /*=NULL*/)
	: CResizingDialog(CVariantUpdateDlg::IDD, pParent)
{
	m_pDoc = doc;
	m_pFile = file;

   addFieldControl(IDC_UPDATE_VARIANT_LIST		, anchorLeft, growBoth);
	addFieldControl(IDOK				               , anchorBottomLeft);
   addFieldControl(IDCANCEL                     , anchorBottomRight);
}

BOOL CVariantUpdateDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

   loadListBox();
	return TRUE; 
}

CVariantUpdateDlg::~CVariantUpdateDlg()
{
}

void CVariantUpdateDlg::loadListBox()
{
 	m_cmbUpdateVariantList.ResetContent();

	if (m_pFile->getVariantList().GetDefaultVariant())
	{
		// Add the default variant first
		m_cmbUpdateVariantList.AddString(m_pFile->getVariantList().GetDefaultVariant()->GetName());
	}

	POSITION pos = m_pFile->getVariantList().GetHeadPosition();
	while(pos != NULL)
	{
		CVariant* variant = m_pFile->getVariantList().GetNext(pos);
		if (variant == NULL)
			continue;

		m_cmbUpdateVariantList.AddString(variant->GetName());
	}
   
   m_cmbUpdateVariantList.SetCurSel(0);
}

void CVariantUpdateDlg::OnBnClickedOk()
{
   CString message;
      
   m_cmbUpdateVariantList.GetText(m_cmbUpdateVariantList.GetCurSel(), UpdateVariantName);
   OnOK();
}

void CVariantUpdateDlg::OnBnClickedCancel()
{
   OnCancel();
}

void CVariantUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_UPDATE_VARIANT_LIST, m_cmbUpdateVariantList);
}


BEGIN_MESSAGE_MAP(CVariantUpdateDlg, CResizingDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CVariantUpdateDlg message handlers

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CVariantNavigatorDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
CVariantNavigatorDlg::CVariantNavigatorDlg(CCEtoODBDoc* doc, FileStruct* file, CWnd* pParent /*=NULL*/)
   : CResizingDialog(CVariantNavigatorDlg::IDD, pParent)
{
	setEnableResize(false);
	SetRememberState(false);

	m_pDoc = doc;
	m_pFile = file;
	m_pActBrdDataVariant = NULL;
	m_pCurVarDataVariant = NULL;

	addFieldControl(IDC_STATIC_LIST		, anchorLeft);
	addFieldControl(IDC_VARIANT_LIST		, anchorLeft,growHorizontal);
	addFieldControl(IDC_EDIT				, anchorRight);
	addFieldControl(IDC_SAVE				, anchorRight);
	addFieldControl(IDC_SAVE_AS			, anchorRight);
	addFieldControl(IDC_SET_AS_DEFAULT	, anchorRight);
	addFieldControl(IDC_UPDATE				, anchorRight);
	addFieldControl(IDC_COMPARE			, anchorRight);
	addFieldControl(IDC_DELETE				, anchorRight);


	CResizingDialogField& actBrdGridField = addFieldControl(IDC_STATIC_ACT_BRD_DATA, anchorBottom, growHorizontal);

   CResizingDialogField& staticField = addFieldControl(IDC_STATIC_ACTIVE_BOARD, anchorLeft);
	staticField.getOperations().addOperation(glueTopEdge, toTopEdge, &actBrdGridField);
	staticField.getOperations().addOperation(glueBottomEdge, toTopEdge, &actBrdGridField);

	CResizingDialogField& curVarGridField = addFieldControl(IDC_STATIC_CUR_VAR_DATA, anchorLeft, growHorizontal);
	curVarGridField.getOperations().addOperation(glueBottomEdge, toTopEdge, &staticField);

   CResizingDialogField& createField = addFieldControl(IDC_CREATE_VARIANT, anchorRight);
	createField.getOperations().addOperation(glueTopEdge, toTopEdge, &actBrdGridField);
	createField.getOperations().addOperation(glueBottomEdge, toTopEdge, &actBrdGridField);

	addFieldControl(IDCANCEL, anchorBottomRight);
}

CVariantNavigatorDlg::~CVariantNavigatorDlg()
{
	delete m_pActBrdDataVariant;
	m_pActBrdDataVariant = NULL;

	// no need to delete this one because it is in the list
	m_pCurVarDataVariant = NULL;
}

void CVariantNavigatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VARIANT_LIST, m_cmbVariantList);
	DDX_Control(pDX, IDC_STATIC_CUR_VAR_DATA, m_ocxStaCurVarData);
	DDX_Control(pDX, IDC_STATIC_ACT_BRD_DATA, m_ocxStaActBrdData);
}

BEGIN_MESSAGE_MAP(CVariantNavigatorDlg, CResizingDialog)
	ON_CBN_SELCHANGE(IDC_VARIANT_LIST, OnCbnSelchangeVariantList)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_SAVE_AS, OnBnClickedSaveAs)
	ON_BN_CLICKED(IDC_SET_AS_DEFAULT, OnBnClickedSetAsDefault)
	ON_BN_CLICKED(IDC_COMPARE, OnBnClickedCompare)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CREATE_VARIANT, OnBnClickedCopyToVariantWindow)
	ON_BN_CLICKED(IDC_EDIT, OnBnClickedEdit)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
   ON_BN_CLICKED(IDC_UPDATE, OnBnClickedUpdate)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CVariantNavigatorDlg, CResizingDialog)
	ON_EVENT(CVariantNavigatorDlg, IDC_CUR_VAR_DATA_GRID, 43, CellChangedCurDataGrid, VTS_I4 VTS_I4)
	ON_EVENT(CVariantNavigatorDlg, IDC_CUR_VAR_DATA_GRID,  3, EnterCellCurDataGrid, VTS_NONE)
	ON_EVENT(CVariantNavigatorDlg, IDC_CUR_VAR_DATA_GRID, DISPID_CLICK, ClickCurDataGrid, VTS_NONE)
	ON_EVENT(CVariantNavigatorDlg, IDC_ACT_BRD_DATA_GRID, DISPID_MOUSEDOWN, MouseDownBrdDataGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4)
	ON_EVENT(CVariantNavigatorDlg, IDC_CUR_VAR_DATA_GRID, DISPID_MOUSEDOWN, MouseDownCurDataGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4)
END_EVENTSINK_MAP()

BOOL CVariantNavigatorDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

   makeLoadedAttribExplicit();

	// Temperary make the followings invisible until we implement them
   //	GetDlgItem(IDC_DELETE)->ShowWindow(SW_HIDE);


	// Create the grids
   m_ocxStaCurVarData.substituteForDlgItem(IDC_CUR_VAR_DATA_GRID, IDC_STATIC_CUR_VAR_DATA, "FlexGrid1", *this);
   m_ocxStaActBrdData.substituteForDlgItem(IDC_ACT_BRD_DATA_GRID, IDC_STATIC_ACT_BRD_DATA, "FlexGrid2", *this);
	initGrids();
	loadVisibleHeadingMap();

	// Set active board data in CAMCAD as active variant and load it in to the grid
	m_pActBrdDataVariant = new CVariant(getCamCadData(),BOARD_DATA_VARIANT_NAME);
   m_pActBrdDataVariant->PopulateVariant(m_pDoc->getCamCadData(), *m_pFile);

	CExtendedFlexGrid *actBrdDataGrid = m_ocxStaActBrdData.getGrid();
	loadVariant(m_pActBrdDataVariant, actBrdDataGrid);


	// Load the variant in variant list into listbox and enable buttons
	loadListBox();
	enableButtons();
	return TRUE; 
}

CCamCadData& CVariantNavigatorDlg::getCamCadData() const
{
   return m_pDoc->getCamCadData();
}

void CVariantNavigatorDlg::initGrids()
{
	// Initialize current variant data grid
	CExtendedFlexGrid* variantGrid = m_ocxStaCurVarData.getGrid();
	variantGrid->put_AllowBigSelection(FALSE);
	variantGrid->put_AllowSelection(TRUE);
	variantGrid->put_AllowUserResizing(CFlexGrid::flexResizeColumns);
	variantGrid->put_Appearance(CFlexGrid::flex3D);
	variantGrid->put_BackColor(GRID_COLOR_WHITE);			
	variantGrid->put_BackColorAlternate(GRID_COLOR_IVORY);
	variantGrid->put_BackColorBkg(GRID_COLOR_GRAY);		
	variantGrid->put_CellFloodColor(GRID_COLOR_LIGHT_GRAY);
	variantGrid->put_CellFloodPercent(100);
	variantGrid->put_ColAlignment(-1, CFlexGrid::flexAlignLeftCenter);
	variantGrid->put_Cols(1);
	variantGrid->put_ColWidthMin(1100);
	variantGrid->put_Editable(CFlexGrid::flexEDNone);
	variantGrid->put_ExplorerBar(CFlexGrid::flexExSort);
	variantGrid->put_ExtendLastCol(TRUE);
	variantGrid->put_FixedCols(0);
	variantGrid->put_FrozenCols(1);
	variantGrid->put_FocusRect(CFlexGrid::flexFocusInset);
	variantGrid->put_HighLight(CFlexGrid::flexHighlightAlways);
	variantGrid->put_SelectionMode(CFlexGrid::flexSelectionByRow);
	variantGrid->put_Rows(1);


	// Initialize active board data grid
	variantGrid = m_ocxStaActBrdData.getGrid();
	variantGrid->put_AllowBigSelection(FALSE);
	variantGrid->put_AllowSelection(TRUE);
	variantGrid->put_AllowUserResizing(CFlexGrid::flexResizeColumns);
	variantGrid->put_Appearance(CFlexGrid::flex3D);
	variantGrid->put_BackColor(GRID_COLOR_WHITE);			
	variantGrid->put_BackColorAlternate(GRID_COLOR_IVORY);
	variantGrid->put_BackColorBkg(GRID_COLOR_GRAY);		
	variantGrid->put_CellFloodColor(GRID_COLOR_LIGHT_GRAY);
	variantGrid->put_CellFloodPercent(100);
	variantGrid->put_ColAlignment(-1, CFlexGrid::flexAlignLeftCenter);
	variantGrid->put_Cols(1);
	variantGrid->put_ColWidthMin(1100);
	variantGrid->put_Editable(CFlexGrid::flexEDNone);
	variantGrid->put_ExplorerBar(CFlexGrid::flexExSort);
	variantGrid->put_ExtendLastCol(TRUE);
	variantGrid->put_FixedCols(0);
	variantGrid->put_FrozenCols(1);
	variantGrid->put_FocusRect(CFlexGrid::flexFocusInset);
	variantGrid->put_HighLight(CFlexGrid::flexHighlightAlways);
	variantGrid->put_SelectionMode(CFlexGrid::flexSelectionByRow);
	variantGrid->put_Rows(1);
}

void CVariantNavigatorDlg::loadListBox()
{
	m_cmbVariantList.ResetContent();

	if (m_pFile->getVariantList().GetDefaultVariant())
	{
		// Add the default variant first
		m_cmbVariantList.AddString(m_pFile->getVariantList().GetDefaultVariant()->GetName());
	}

	POSITION pos = m_pFile->getVariantList().GetHeadPosition();
	while(pos != NULL)
	{
		CVariant* variant = m_pFile->getVariantList().GetNext(pos);
		
      if (variant != NULL)
		   m_cmbVariantList.AddString(variant->GetName());
	}
}

void CVariantNavigatorDlg:: enableButtons()
{
	CString buttonName = "";
	GetDlgItem(IDC_EDIT)->GetWindowText(buttonName);


	// Always enable listbox except in edit mode
	m_cmbVariantList.EnableWindow(TRUE);
	GetDlgItem(IDC_CREATE_VARIANT)->EnableWindow(TRUE);

	// Diable all buttons except "Close"
	GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVE_AS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SET_AS_DEFAULT)->EnableWindow(FALSE);
	GetDlgItem(IDC_UPDATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMPARE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);


	CExtendedFlexGrid* curVarGrid = m_ocxStaCurVarData.getGrid();

	if (curVarGrid->get_Rows() > 1 && buttonName.CompareNoCase("Enable Edit") == 0)
	{
		// Current not in edit mode

		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVE_AS)->EnableWindow(TRUE);
		GetDlgItem(IDC_SET_AS_DEFAULT)->EnableWindow(TRUE);
      GetDlgItem(IDC_UPDATE)->EnableWindow(TRUE);
		
		// Only enable when the data in the grid is already in a variant
		if (m_pCurVarDataVariant != NULL)
		{
			GetDlgItem(IDC_COMPARE)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);

			// Will enable followings when we implement them
			// GetDlgItem(IDC_UPDATE)->EnableWindow(TRUE);
		}
	}
	else if (curVarGrid->get_Rows() > 1 && buttonName.CompareNoCase("Enable Edit") != 0)
	{
		// Current in edit mode

		m_cmbVariantList.EnableWindow(FALSE);
		GetDlgItem(IDC_CREATE_VARIANT)->EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVE_AS)->EnableWindow(TRUE);
	}
}

void CVariantNavigatorDlg::loadVisibleHeadingMap()
{
	int count = 0;
	m_visibleHeadingArray.SetSize(0, 20);
	m_visibleHeadingArray.SetAtGrow(0, "");	// reserved for the zero column for refdes
	m_visibleHeadingArray.SetAtGrow(++count, "geom_name");
	m_visibleHeadingArray.SetAtGrow(++count, "partnumber");
	m_visibleHeadingArray.SetAtGrow(++count, "loaded");
	m_visibleHeadingArray.SetAtGrow(++count, "realpart_name");
	m_visibleHeadingArray.SetAtGrow(++count, "devicetype");
	m_visibleHeadingArray.SetAtGrow(++count, "value");
	m_visibleHeadingArray.SetAtGrow(++count, "+tol");
	m_visibleHeadingArray.SetAtGrow(++count, "-tol");
	m_visibleHeadingArray.SetAtGrow(++count, "subclass");
}

void CVariantNavigatorDlg::loadVariant(CVariant* variant, CExtendedFlexGrid *variantGrid)
{
	variantGrid->clear();
	variantGrid->put_Cols(1);
	variantGrid->put_Rows(1);

	if (variant == NULL)
		return;


	COleVariant floorColor((GRID_CNT_TYPE)GRID_COLOR_LIGHT_GRAY);
	COleVariant floor100Percent((GRID_CNT_TYPE)100);
	COleVariant floorZeroPercent((GRID_CNT_TYPE)0);
	COleVariant rowOne((GRID_CNT_TYPE)1);
	COleVariant colOne((GRID_CNT_TYPE)1);
	CMapStringToString keywordToColMap;

	POSITION itemPos = variant->GetStartPosition();
	while (itemPos != NULL)
	{
		CVariantItem* item = variant->GetNext(itemPos);
		if (item == NULL)
			continue;


		// Add refdes to new row
		variantGrid->AddItem(item->GetName(), COleVariant(variantGrid->get_Rows()));

		// Initially flood the cells of new row in light gray
		COleVariant rowLast(variantGrid->get_Rows() - 1);
		COleVariant colLast(variantGrid->get_Cols() - 1);
		if (variantGrid->get_Cols() > 1)
		{
			// Only flood columns that are not the first column
			variantGrid->put_Cell(CFlexGrid::flexcpFloodColor, rowLast, colOne, rowLast, colLast, floorColor);
			variantGrid->put_Cell(CFlexGrid::flexcpFloodPercent, rowLast, colOne, rowLast, colLast, floor100Percent);
		}

      if (item->getAttributes() != NULL)
      {
         // Add attributes into cells
         POSITION attribPos = item->getAttributes()->GetStartPosition();
         while (attribPos != NULL)
         {
            Attrib* attrib = NULL;
            WORD keywordIndex = -1;
            item->getAttributes()->GetNextAssoc(attribPos, keywordIndex, attrib);
            if (keywordIndex < 0 || attrib == NULL)
               continue;

            // Get keyword and value of attribute
            CString value = get_attvalue_string(m_pDoc, attrib);
            const KeyWordStruct *keyword = m_pDoc->getKeyWordArray()[keywordIndex];
            CString colKey;
            COleVariant colCur;

            // Create keyword column if not already there
            if (!keywordToColMap.Lookup(keyword->getCCKeyword(), colKey))
            {
               colKey = keyword->getCCKeyword();
               keywordToColMap.SetAt(colKey, colKey);

               // Add the column and column header
               variantGrid->put_Cols(variantGrid->get_Cols() + 1);
               variantGrid->put_ColKey(variantGrid->get_Cols() - 1, colKey);
               variantGrid->setCell(0, variantGrid->get_Cols() - 1, colKey);

               // Initizially flood the cell of new column in light gray
               colCur = COleVariant((variantGrid->get_Cols() - 1));
               variantGrid->put_Cell(CFlexGrid::flexcpFloodColor, rowOne, colCur, rowLast, colCur, floorColor);
               variantGrid->put_Cell(CFlexGrid::flexcpFloodPercent, rowOne, colCur, rowLast, colCur, floor100Percent);
            }
            else
            {
               colCur = COleVariant((GRID_CNT_TYPE)variantGrid->get_ColIndex(colKey));
            }


            // Set value, and set percent of flood zero to indicate cell has value
            variantGrid->put_Cell(CFlexGrid::flexcpText, rowLast, colCur, rowLast, colCur, (COleVariant)value);
            variantGrid->put_Cell(CFlexGrid::flexcpFloodPercent, rowLast, colCur, rowLast, colCur, floorZeroPercent);
         }
      }
	}



	// First hide all columns
	variantGrid->put_ColHidden(-1, TRUE);	

	// Second always show the zero columns
	variantGrid->put_ColHidden(0, FALSE);

	// Third show those that are in the map
	long colPos = 0;
	for (long i=1; i<m_visibleHeadingArray.GetCount(); i++)
	{
		CString header = m_visibleHeadingArray.GetAt(i);
		if (header.IsEmpty())
			continue;

		long colIndex = variantGrid->get_ColIndex(header);
		if (colIndex < 1)
			continue;

		if (header.CompareNoCase(ATT_LOADED) == 0)
			variantGrid->put_ColComboList(colIndex, "FALSE|TRUE");
		
		if (header.CompareNoCase(ATT_DEVICETYPE) == 0)
			variantGrid->put_ColComboList(colIndex, getDevceTypeListAsDelimitedFriendlyString("|"));

		variantGrid->put_ColHidden(colIndex, FALSE);
		variantGrid->put_ColPosition(colIndex, ++colPos);
	}
}

void CVariantNavigatorDlg::saveVariant(CVariant *variant, CExtendedFlexGrid *variantGrid)
{
	if (variant == NULL)
		return;

	COleVariant floorZeroPercent((GRID_CNT_TYPE)0);
	for (long i=1; i<variantGrid->get_Rows(); i++)
	{
		CVariantItem* item = variant->AddItem(variantGrid->getCellText(i, 0));

		for (long k=1; k<variantGrid->get_Cols(); k++)
		{         
			COleVariant row(i);
			COleVariant col(k);

			CString keyword = variantGrid->getCellText(0, col.lVal);
			CString value = variantGrid->getCellText(row.lVal, col.lVal);
			long floodPercent = COleVariant(variantGrid->get_Cell(CFlexGrid::flexcpFloodPercent, row, col, row, col)).lVal;

			// If flood percent is 100 and no value then it means the cells doesn't have that attribute
			if (floodPercent == 100 && value.IsEmpty())
				continue;

			variantGrid->put_Cell(CFlexGrid::flexcpFloodPercent, row, col, row, col, floorZeroPercent);
			m_pDoc->SetUnknownAttrib(&item->getAttributesRef(), keyword, value, SA_OVERWRITE, NULL);
		}
	}
}

CVariant* CVariantNavigatorDlg::createVariant(CString errMessageCaption, CExtendedFlexGrid *variantGrid)
{
	CString variantName = "";
	while (true)
	{
		// Prompt for variant name
		InputDlg dlg;
		dlg.m_prompt = "Enter Variant Name:\n\nVariant must start with a letter.";
		if (dlg.DoModal() != IDOK)
			return NULL;
		variantName = dlg.m_input.Trim();


		// Check if variant is valid
		CString message = "";
		if (toupper(variantName[0]) < 65 || toupper(variantName[0]) > 90)
			message.Format("Variant name \"%s\" starts with \"%c\" is not allowed, it must starts with a letter.  Please enter another name.", variantName, variantName[0]);
		else if (variantName.CompareNoCase("Default") == 0)
			message.Format("Variant name \"%s\" is reserved for the default variant in CAMCAD.  Please enter another name.", variantName);
		else
			break;

		MessageBox(message, errMessageCaption, MB_ICONINFORMATION);
	}

	// Check if variant is alread existed
	CVariant* variant = m_pFile->getVariantList().FindVariant(variantName);
	if (variant != NULL)
	{
		CString message = "";
		message.Format("Variant \"%s\" already exists.  Do you want to replace it?", variantName);
		if (MessageBox(message, errMessageCaption, MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
			return NULL;
	}
	else
	{
		variant = m_pFile->getVariantList().AddVariant(variantName);
		int index = m_cmbVariantList.AddString(variant->GetName());
	}

	saveVariant(variant, variantGrid);
	return variant;
}

CVariant* CVariantNavigatorDlg::updateVariant(const CString& variantName, CExtendedFlexGrid *variantGrid)
{
	// Check if variant is alread existed
	CVariant* variant = m_pFile->getVariantList().FindVariant(variantName);

   if ( variant == NULL )
   {
      MessageBox("Update failed!");
   }
   else
   {
	   saveVariant(variant, variantGrid);
   }
	return variant;
}

void CVariantNavigatorDlg::OnCbnSelchangeVariantList()
{
	CString variantName = "";
	m_cmbVariantList.GetLBText(m_cmbVariantList.GetCurSel(), variantName);

	CString defaultVariantName = "";
	if (m_pFile->getVariantList().GetDefaultVariant() != NULL)
		defaultVariantName = m_pFile->getVariantList().GetDefaultVariant()->GetName();


	m_pCurVarDataVariant = NULL;
	if (!defaultVariantName.IsEmpty() && variantName.CompareNoCase(defaultVariantName) == 0)
		m_pCurVarDataVariant = m_pFile->getVariantList().GetDefaultVariant();
	else
		m_pCurVarDataVariant = m_pFile->getVariantList().FindVariant(variantName);

	CExtendedFlexGrid* variantGrid = m_ocxStaCurVarData.getGrid();
	loadVariant(m_pCurVarDataVariant, variantGrid);

	enableButtons();
}


void CVariantNavigatorDlg::OnBnClickedEdit()
{
	CButton* editButton = (CButton*)GetDlgItem(IDC_EDIT);
	CString buttonName = "";
	editButton->GetWindowText(buttonName);

	CExtendedFlexGrid *variantGrid = m_ocxStaCurVarData.getGrid();
	if (buttonName.CompareNoCase("Enable Edit") == 0)
	{
		// Enter edit mode
		variantGrid->put_GridLines(CFlexGrid::flexGridInset);
		variantGrid->put_SelectionMode(CFlexGrid::flexSelectionFree);
		editButton->SetWindowText("Cancel and\nExit Edit");
	}
	else
	{
		// Exit edit mode
		if (m_pCurVarDataVariant != NULL)
		{
			// Reload the current selected variant and clear change
			loadVariant(m_pCurVarDataVariant, variantGrid);
		}
		else
		{
			CString message = "Data in the grid was never saved into a variant so \"Exit Edit Don't Save\" will clear the grid.  Do you want to continue?";
			if (MessageBox(message, "Warning", MB_ICONWARNING|MB_YESNO) == IDNO)
				return;

			loadVariant(NULL, variantGrid);
		}

		variantGrid->put_GridLines(CFlexGrid::flexGridFlat);
		variantGrid->put_SelectionMode(CFlexGrid::flexSelectionByRow);
		editButton->SetWindowText("Enable Edit");

	}

	enableButtons();
}

void CVariantNavigatorDlg::OnBnClickedSave()
{
	// Get current variant name in listbox
	CString variantName = "";
	if (m_cmbVariantList.GetCurSel() > -1)
	{
		m_cmbVariantList.GetLBText(m_cmbVariantList.GetCurSel(), variantName);

		if (!variantName.IsEmpty())
		{
			// Check if variant is alread existed, if not create one and save the data to the variant
			if (m_pCurVarDataVariant == NULL)
				m_pCurVarDataVariant = m_pFile->getVariantList().AddVariant(m_pDoc->getCamCadData(), *m_pFile, variantName);

			CExtendedFlexGrid *variantGrid = m_ocxStaCurVarData.getGrid();
			saveVariant(m_pCurVarDataVariant, variantGrid);
		}
	}

	if (variantName.IsEmpty())
		OnBnClickedSaveAs();

	OnBnClickedEdit();
}

void CVariantNavigatorDlg::OnBnClickedSaveAs()
{
	CExtendedFlexGrid *variantGrid = m_ocxStaCurVarData.getGrid();
	m_pCurVarDataVariant = createVariant("Save As", variantGrid);

	if (m_pCurVarDataVariant != NULL)
	{
		int index = m_cmbVariantList.FindString(-1, m_pCurVarDataVariant->GetName());
		m_cmbVariantList.SetCurSel(index);
		enableButtons();
	}
}

void CVariantNavigatorDlg::OnBnClickedSetAsDefault()
{
	// Update the default variant in file with the current variant
	if (m_pCurVarDataVariant != NULL)
	{
		m_pFile->getVariantList().UpdateDefaultVariant(*m_pCurVarDataVariant);
		if (m_cmbVariantList.FindString(-1, DEFAULT_VARIANT_NAME) < 0)
			m_cmbVariantList.AddString(DEFAULT_VARIANT_NAME);
	}
	else
	{
		CExtendedFlexGrid *variantGrid = m_ocxStaCurVarData.getGrid();
		m_pCurVarDataVariant = m_pFile->getVariantList().CreateDefaultVariant();
		
		if (m_cmbVariantList.FindString(-1, DEFAULT_VARIANT_NAME) < 0)
		{
			m_cmbVariantList.AddString(DEFAULT_VARIANT_NAME);
			m_cmbVariantList.SelectString(-1, DEFAULT_VARIANT_NAME);
		}
	
		saveVariant(m_pCurVarDataVariant, variantGrid);
		enableButtons();
	}
}

void CVariantNavigatorDlg::OnBnClickedCompare()
{
	CExtendedFlexGrid* curVarGrid = m_ocxStaCurVarData.getGrid();
	CExtendedFlexGrid* actBrdGrid = m_ocxStaActBrdData.getGrid();
	if (curVarGrid == NULL || actBrdGrid == NULL || m_pCurVarDataVariant == NULL || m_pActBrdDataVariant == NULL)
		return;


	COleVariant colZero((GRID_CNT_TYPE)0);
	COleVariant colOne((GRID_CNT_TYPE)1);
	COleVariant colLast(curVarGrid->get_Cols() - 1);

	// Sort the rows of both grid in order to do comparison
	curVarGrid->put_Col((GRID_CNT_TYPE)0);
	curVarGrid->put_Sort(CFlexGrid::flexSortGenericAscending);
	actBrdGrid->put_Col((GRID_CNT_TYPE)0);
	actBrdGrid->put_Sort(CFlexGrid::flexSortGenericAscending);


	for (long i=1; i<curVarGrid->get_Rows(); i++)
	{
		COleVariant row(i);

		curVarGrid->put_Cell(CFlexGrid::flexcpBackColor, row, colZero, row, colLast, colZero);


		CString refdes = (CString)actBrdGrid->get_Cell(CFlexGrid::flexcpText, row, colZero, row, colZero);
		CVariantItem* curVarDataItem =  m_pCurVarDataVariant->FindItem(refdes);
		CVariantItem* actBrdDataItem =  m_pActBrdDataVariant->FindItem(refdes);

		if (curVarDataItem == NULL)
			continue;

		bool isEqual = false;
		if (actBrdDataItem != NULL)
		{
			CAttributes* curVarDataAttributes = curVarDataItem->getAttributes();
			CAttributes* actBrdDataAttributes = actBrdDataItem->getAttributes();

			isEqual = curVarDataAttributes->IsEqual(*actBrdDataAttributes, attribCompareByValue);
		}

		if (!isEqual)
		{
			// Highlight the row that is different in currant variant grid
			curVarGrid->put_Cell(CFlexGrid::flexcpBackColor, row, colZero, row, colLast, (COleVariant)GRID_COLOR_ORANGE);
		}
	}
}

void CVariantNavigatorDlg::makeLoadedAttribExplicit()
{
   // If the LOADED attrib is not present, then it defaults to true.
   // Make it explicit. If already present then leave it be.

   if (m_pDoc != NULL && m_pFile != NULL && m_pFile->getBlock() != NULL)
   {
      WORD loadedKW = m_pDoc->RegisterKeyWord(ATT_LOADED, 0, valueTypeString);     
	   CDataList *DataList = &(m_pFile->getBlock()->getDataList());

      POSITION pos = DataList->GetHeadPosition();
      while (pos != NULL)
      {
         DataStruct *data = DataList->GetNext(pos);
         if (data->isInsertType(insertTypePcbComponent))
         {
            Attrib *attrib;
            if (!data->lookUpAttrib(loadedKW, attrib))
               m_pDoc->SetUnknownAttrib(&data->getDefinedAttributes(), ATT_LOADED, "TRUE", SA_OVERWRITE, NULL);
         }
      }
   }
}

void CVariantNavigatorDlg::OnBnClickedDelete()
{
	CString variantName = "";
	m_cmbVariantList.GetWindowText(variantName);

	// Remove variant from listbox and clear grid
	m_cmbVariantList.DeleteString(m_cmbVariantList.GetCurSel());
	m_ocxStaCurVarData.getGrid()->clear();
	m_ocxStaCurVarData.getGrid()->put_Rows(1);


	// Remove variant from list and deleting it
	CVariant* variant = NULL;
	if (variantName.CompareNoCase(DEFAULT_VARIANT_NAME) == 0)
		variant = m_pFile->getVariantList().RemoveDefaultVariant();
	else
		variant = m_pFile->getVariantList().RemoveVariant(variantName);

	delete variant;
	variant = NULL;

	enableButtons();
}

void CVariantNavigatorDlg::OnBnClickedRefreshBrdData()
{
	// TODO: Add your control notification handler code here
}

void CVariantNavigatorDlg::OnBnClickedCopyToVariantWindow()
{
	CExtendedFlexGrid* curVarGrid = m_ocxStaCurVarData.getGrid();
	if (m_pActBrdDataVariant == NULL || curVarGrid == NULL)
		return;

	// Load the board data into the current variant grid
	loadVariant(m_pActBrdDataVariant, curVarGrid);

	// Set the pointer to the current variant NULL because the data in the grid is not yet a variant
	m_pCurVarDataVariant = NULL;

	// Set variant list selection to none and enable buttons
	m_cmbVariantList.Clear();
	m_cmbVariantList.SetCurSel(-1);
	enableButtons();
}

void CVariantNavigatorDlg::CellChangedCurDataGrid(long Row, long Col)
{
	// TODO: Add your message handler code here
}

void CVariantNavigatorDlg::EnterCellCurDataGrid()
{
	// TODO: Add your message handler code here
}

void CVariantNavigatorDlg::ClickCurDataGrid()
{
	CButton* editButton = (CButton*)GetDlgItem(IDC_EDIT);
	CString buttonName = "";
	editButton->GetWindowText(buttonName);
	if (buttonName.CompareNoCase("Enable Edit") == 0)
		return;

	CExtendedFlexGrid* variantGrid = m_ocxStaCurVarData.getGrid();
	long mouseRow = variantGrid->get_MouseRow();
	long mouseCol = variantGrid->get_MouseCol();
	if (mouseCol > 0 && mouseCol < variantGrid->get_Cols() &&
		 mouseRow > 0 && mouseRow < variantGrid->get_Rows())
	{
		CString header = variantGrid->getCellText(0, mouseCol);
		if (header.CompareNoCase("geom_name") != 0)
		{
			variantGrid->Select(mouseRow, mouseCol, COleVariant(mouseRow), COleVariant(mouseCol));
			variantGrid->EditCell();
		}
	}
}

void CVariantNavigatorDlg::MouseDownBrdDataGrid()
{
	m_ocxStaActBrdData.restoreWindowPlacement();
}

void CVariantNavigatorDlg::MouseDownCurDataGrid()
{
	m_ocxStaCurVarData.restoreWindowPlacement();
}

void CVariantNavigatorDlg::OnBnClickedCancel()
{
	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(m_pDoc);

	CDialog::OnCancel();
}



void CVariantNavigatorDlg::OnBnClickedUpdate()
{
   CVariantUpdateDlg dlg(m_pDoc, m_pFile);

   // If the users press the OK button, save variant
   if (dlg.DoModal() == IDOK)
   {
      CExtendedFlexGrid *variantGrid = m_ocxStaCurVarData.getGrid();
      updateVariant(dlg.getVariantName(), variantGrid);      
   } 
}
