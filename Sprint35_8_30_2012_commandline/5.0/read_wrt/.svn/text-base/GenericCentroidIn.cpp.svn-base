// $Header: /CAMCAD/5.0/read_wrt/GenericCentroidIn.cpp 18    3/12/07 12:52p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GenericCentroidIn.cpp

#include "stdafx.h"
#include "ccdoc.h"
#include "SelectDelimiterDlg.h"
#include "Centroid.h"
#include "GerberEducator.h"
#include "GenericCentroidIn.h"
#include ".\genericcentroidin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static double euro_atof(const char *s)
{
	CString cs(s);
	cs.Replace(',', '.');

	return atof(cs);
}

void CCEtoODBDoc::OnGerberEducatorGenericCentroidImport()
{
	//DoFileImport(fileTypeGenericCentroid);
}

void ReadGenericCentroid (const CString fileName , CCEtoODBDoc& camcadDoc, FormatStruct& format)
{
	bool doCentroidImport = true;
	while (doCentroidImport)
	{
		CSelectDelimiterDlg dlgDelimiter(0);
		if (dlgDelimiter.DoModal() != IDOK)
			return;

		char delimiter = dlgDelimiter.GetDelimiter();
		if (delimiter == '\0')
			delimiter = ',';	
		
		int dataStartLine = dlgDelimiter.IgnoreLines();
		int headerLine = dlgDelimiter.GetHeaderStartsLine();

		CGenericCentroidImportDlg centroidImportDlg(camcadDoc, fileName, delimiter, headerLine, dataStartLine);

		doCentroidImport = (centroidImportDlg.DoModal() != IDOK && centroidImportDlg.isImportRejected());		
	} 
}


//-----------------------------------------------------------------------------
// CGenericCentroidImportDlg dialog
//-----------------------------------------------------------------------------
CGenericCentroidImportDlg::CGenericCentroidImportDlg(CCEtoODBDoc& camcadDoc, const CString fileName , const char delimiter, int headerLine, int dataStartLine)
	: CResizingDialog(CGenericCentroidImportDlg::IDD)
	, m_camCadDatabase(camcadDoc)
	, m_fileName(fileName)
	, m_surfaceOption(surfaceBoth)
   , m_createGeomTypeOption(eCentroidsOnly)
{
	m_delimiter = delimiter;
	m_headerLineNumber = headerLine;
	m_dataStartLineNumber = dataStartLine;
	m_initialized = false;
	m_rejected = false;

	if (m_delimiter == '\0')
		m_delimiter = ';';

	if (m_headerLineNumber < 0)
		m_headerLineNumber = 0;

	if (m_dataStartLineNumber < 1)
		m_dataStartLineNumber = 1;


	addFieldControl(IDC_RefdesStatic							, anchorLeft);
	addFieldControl(IDC_RefdesCombo							, anchorLeft);
	addFieldControl(IDC_PartnumberStatic					, anchorLeft);
	addFieldControl(IDC_PartnumberCombo						, anchorLeft);
	addFieldControl(IDC_xCoordinateStatic					, anchorLeft);
	addFieldControl(IDC_xCoordinateCombo					, anchorLeft);
	addFieldControl(IDC_yCoordinateStatic					, anchorLeft);
	addFieldControl(IDC_yCoordinateCombo					, anchorLeft);
	addFieldControl(IDC_RotationStatic						, anchorLeft);
	addFieldControl(IDC_RotationCombo						, anchorLeft);

	addFieldControl(IDC_SurfaceStatic						, anchorLeft);
	addFieldControl(IDC_SurfaceTopRadio						, anchorLeft);
	addFieldControl(IDC_SurfaceBottomRadio					, anchorLeft);
	addFieldControl(IDC_SurfaceBothRadio					, anchorLeft);
	addFieldControl(IDC_SurfaceCombo							, anchorLeft);
	addFieldControl(IDC_TopStatic								, anchorLeft);
	addFieldControl(IDC_TopCombo								, anchorLeft);

	addFieldControl(IDC_GeomTypeGroup						, anchorLeft);
	addFieldControl(IDC_CentroidsOnlyRadio					, anchorLeft);
	addFieldControl(IDC_PcbComponentsRadio					, anchorLeft);

	addFieldControl(IDC_CentroidGridStatic					, anchorLeft, growBoth);
	addFieldControl(IDC_Reject									, anchorBottomRight);
	addFieldControl(IDOK											, anchorBottomRight);
	addFieldControl(IDCANCEL									, anchorBottomRight);
}

CGenericCentroidImportDlg::~CGenericCentroidImportDlg()
{
}

void CGenericCentroidImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RefdesCombo, m_refdesCombo);
	DDX_Control(pDX, IDC_PartnumberCombo, m_partnumberCombo);
	DDX_Control(pDX, IDC_xCoordinateCombo, m_xCoordinateCombo);
	DDX_Control(pDX, IDC_yCoordinateCombo, m_yCoordinateCombo);
	DDX_Control(pDX, IDC_RotationCombo, m_rotationCombo);
	DDX_Control(pDX, IDC_SurfaceCombo, m_surfaceCombo);
	DDX_Control(pDX, IDC_CentroidGridStatic, m_centroidGrid);
	DDX_Control(pDX, IDC_TopCombo, m_topCombo);
	DDX_Control(pDX, IDC_unitsCombo, m_unitsCombo);
	DDX_Radio(pDX, IDC_SurfaceTopRadio, m_surfaceOption);
   DDX_Radio(pDX, IDC_CentroidsOnlyRadio, m_createGeomTypeOption);
}

BEGIN_MESSAGE_MAP(CGenericCentroidImportDlg, CResizingDialog)
	ON_CBN_SELCHANGE(IDC_SurfaceCombo, OnCbnSelchangeSurfaceCombo)
	ON_BN_CLICKED(IDC_SurfaceTopRadio, OnBnClickedSurfaceOption)
	ON_BN_CLICKED(IDC_SurfaceBottomRadio, OnBnClickedSurfaceOption)
	ON_BN_CLICKED(IDC_SurfaceBothRadio, OnBnClickedSurfaceOption)
   ON_BN_CLICKED(IDC_CentroidsOnlyRadio, OnBnClickedCreateGeomTypeOption)
   ON_BN_CLICKED(IDC_PcbComponentsRadio, OnBnClickedCreateGeomTypeOption)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_Reject, OnBnClickedReject)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGenericCentroidImportDlg, CResizingDialog)
//	ON_EVENT(CGenericCentroidImportDlg, IDC_CentroidGrid, 0x18, AfterEditGrid, VTS_I4 VTS_I4)
	ON_EVENT(CGenericCentroidImportDlg, IDC_CentroidGrid, 0x15, BeforeEditGrid, VTS_I4 VTS_I4 VTS_BOOL)
	ON_EVENT(CGenericCentroidImportDlg, IDC_CentroidGrid, 0x5, BeforeMouseDownGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4 VTS_BOOL)
   ON_EVENT(CGenericCentroidImportDlg, IDC_CentroidGrid, 0xd, BeforeSortGrid, VTS_I4 VTS_PI2)
//	ON_EVENT(CGenericCentroidImportDlg, IDC_CentroidGrid, DISPID_DBLCLICK, DoubleClickGrid, VTS_NONE)
	ON_EVENT(CGenericCentroidImportDlg, IDC_CentroidGrid, 0x3, EnterCellGrid, VTS_NONE)
	ON_EVENT(CGenericCentroidImportDlg, IDC_CentroidGrid, 0x17, ValidateEditGrid, VTS_I4 VTS_I4 VTS_BOOL)
END_EVENTSINK_MAP()

BOOL CGenericCentroidImportDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

	m_centroidGrid.substituteForDlgItem(IDC_CentroidGrid,IDC_CentroidGridStatic,"FlexGrid",*this);
	initGrid();
	loadGrid();

	// Add units to combo
	m_unitsCombo.AddString(unitStringAbbreviation(pageUnitsInches));
	m_unitsCombo.AddString(unitStringAbbreviation(pageUnitsMils));
	m_unitsCombo.AddString(unitStringAbbreviation(pageUnitsMilliMeters));
	m_unitsCombo.AddString(unitStringAbbreviation(pageUnitsHpPlotter));
	m_unitsCombo.AddString(unitStringAbbreviation(pageUnitsCentiMicroMeters));
	m_unitsCombo.AddString(unitStringAbbreviation(pageUnitsMicroMeters));
	m_unitsCombo.AddString(unitStringAbbreviation(pageUnitsNanoMeters));
	m_unitsCombo.SelectString(0, unitStringAbbreviation(pageUnitsInches));

	m_initialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGenericCentroidImportDlg::initGrid()
{
	// Initialize grid
   getFlexGrid().clear();
	getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	getFlexGrid().put_AllowSelection(FALSE);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);
   getFlexGrid().put_Cols(0);
	getFlexGrid().put_ExplorerBar(CFlexGrid::flexExSortShow);
   getFlexGrid().put_ExtendLastCol(true);
   getFlexGrid().put_FixedCols(0);
   getFlexGrid().put_FixedRows(2);
	getFlexGrid().put_FocusRect(CFlexGrid::flexFocusInset);
	getFlexGrid().put_HighLight(CFlexGrid::flexHighlightAlways);
   getFlexGrid().put_Rows(2);
	getFlexGrid().put_ScrollBars(CFlexGrid::flexScrollBarBoth);
	getFlexGrid().put_SelectionMode(CFlexGrid::flexSelectionByRow);
}


void CGenericCentroidImportDlg::loadGrid()
{
	// Open file form reading
	CFormatStdioFile file;
	if (!file.Open(m_fileName, CFile::modeRead))
	{
      CString tmp;
      tmp.Format("Can not open centroid file [%s]", m_fileName);
      ErrorMessage(tmp);
      return;
	}

	// Read from file and load into grid
	int colCount = 0;
	int lineCount = 0;
	CString headerLine;
	CString line;
	CString delimitList = m_delimiter;

	CArray<int, int> numericColumnsArray;
	numericColumnsArray.SetSize(0, 100);

	while (file.ReadString(line))
	{
		lineCount++;
		line.Trim();	//.Replace(m_delimiter, '\t');
		if (line.IsEmpty())
			continue;

		CStringArray params;
		CSupString supString(line);
		supString.ParseQuote(params, delimitList);

		// Increase number of columns
		if (params.GetCount() > colCount)
		{
			colCount = params.GetCount();
         getFlexGrid().put_Cols(colCount);
		}

		// Parse the line of input and check to see if the value of each column is numeric
		for (int i=0; i<params.GetCount(); i++)
		{
			if (i == 0)
				line = params.GetAt(i);
			else
				line.AppendFormat("\t%s", params.GetAt(i));

			if (lineCount < m_dataStartLineNumber)
				continue;

         // Case 2165
         // Allow a blank field to be accepted as a number, it will default to zero later.
			if (i >= numericColumnsArray.GetCount() || numericColumnsArray.GetAt(i) > 0)
            numericColumnsArray.SetAtGrow(i, (is_number(params.GetAt(i)) || params.GetAt(i).IsEmpty()));
		}


		if (lineCount == m_headerLineNumber)
		{
			headerLine = line;
		}
		else if (lineCount >= m_dataStartLineNumber)
		{
			getFlexGrid().addItem(line, (int)getFlexGrid().get_Rows());
		}
	}

	// Add header to grid and combo boxes
	headerLine.Trim();
	int startIndex=0;
	for (int i=0; i<colCount; i++)
	{
		CString header;
		if (!headerLine.IsEmpty())
		{
			header = headerLine.Tokenize("\t", startIndex);
			header.Trim();
		}

		if (header.IsEmpty())
			header.Format("Header_%d", i+1);

		getFlexGrid().setCell(0, i, header);
		m_columnNameToIndexMap.SetAt(header, i);

		CString lowerCaseHeader = header;
		lowerCaseHeader.MakeLower();

		m_refdesCombo.AddString(header);
		if (lowerCaseHeader.Find("ref") > -1)
			m_refdesCombo.SelectString(0, header);

		m_partnumberCombo.AddString(header);
		if (lowerCaseHeader.Find("part") > -1)
			m_partnumberCombo.SelectString(0, header);

		m_surfaceCombo.AddString(header);
		if (lowerCaseHeader.Find("surface") > -1	|| lowerCaseHeader.Find("side") > -1	||
			 lowerCaseHeader.Find("top") > -1		|| lowerCaseHeader.Find("bottom") > -1	||
			 lowerCaseHeader.Find("mirror") > -1	)
			m_surfaceCombo.SelectString(0, header);

		if (i < numericColumnsArray.GetCount() && numericColumnsArray.GetAt(i) > 0)
		{
			m_xCoordinateCombo.AddString(header);
			if (lowerCaseHeader.GetAt(0) == 'x')
				m_xCoordinateCombo.SelectString(0, header);

			m_yCoordinateCombo.AddString(header);
			if (lowerCaseHeader.GetAt(0) == 'y')
				m_yCoordinateCombo.SelectString(0, header);

			m_rotationCombo.AddString(header);
			if (lowerCaseHeader.Find("rot") > -1 || lowerCaseHeader.Find("angle") > -1)
				m_rotationCombo.SelectString(0, header);
		}
	}
	OnCbnSelchangeSurfaceCombo();
	OnBnClickedSurfaceOption();

	if (getFlexGrid().get_Rows() > 1)
	{
		getFlexGrid().autoSizeColumns();
		getFlexGrid().select(1, 0, 1, 0);
	}

	file.Close();
}

void CGenericCentroidImportDlg::loadFieldNameToCombo()
{
}

void CGenericCentroidImportDlg::AfterEditGrid(long Row, long Col)
{
	getFlexGrid().autoSizeColumns();
}

void CGenericCentroidImportDlg::BeforeEditGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
	if (Row == 0)
	{
		getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
	}
	else
	{
		getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	}
}

void CGenericCentroidImportDlg::BeforeMouseDownGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel)
{
	m_centroidGrid.restoreWindowPlacement();
}

void CGenericCentroidImportDlg::BeforeSortGrid(long col, short* order)
{
	if (getFlexGrid().get_MouseRow() == 1)
	{
      getFlexGrid().put_ColSort(getFlexGrid().get_MouseCol(), (long)*order);
      getFlexGrid().select(0,col,0,col);
      getFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);
	}
	else
	{
		*order = CFlexGrid::flexSortNone;

		if (getFlexGrid().get_MouseRow() == 0)
		{
			getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
			getFlexGrid().select(0, getFlexGrid().get_MouseCol()); 
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText((int)getFlexGrid().get_Row(), (int)getFlexGrid().get_Col()).GetLength());
		}
	}
}

void CGenericCentroidImportDlg::DoubleClickGrid()
{
	//if (getFlexGrid().get_MouseRow() == 0)
	//{
	//	getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
	//	getFlexGrid().select(0, getFlexGrid().get_MouseCol()); 
	//	getFlexGrid().EditCell();
	//	getFlexGrid().put_EditSelStart(0);
	//	getFlexGrid().put_EditSelLength(getFlexGrid().getCellText((int)getFlexGrid().get_Row(), (int)getFlexGrid().get_Col()).GetLength());
	//}
}

void CGenericCentroidImportDlg::EnterCellGrid()
{
	//if (getFlexGrid().get_MouseRow() == 0)
	//{
	//	getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
	//	getFlexGrid().select(0, getFlexGrid().get_MouseCol()); 
	//	getFlexGrid().EditCell();
	//	getFlexGrid().put_EditSelStart(0);
	//	getFlexGrid().put_EditSelLength(getFlexGrid().getCellText((int)getFlexGrid().get_Row(), (int)getFlexGrid().get_Col()).GetLength());
	//}
	//else
	//{
	//	getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	//}
}

void CGenericCentroidImportDlg::ValidateEditGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
	if (!m_initialized || Row != 0)
		return;

	CString oldHeader = getFlexGrid().getCellText((int)Row, (int)Col);
	CString newHeader = getFlexGrid().get_EditText();
	if (newHeader.IsEmpty())
	{
		ErrorMessage("Field name cannot be blank.  Please enter other field name.", "Field Name");
		getFlexGrid().put_EditText(oldHeader);
		getFlexGrid().EditCell();
		getFlexGrid().put_EditSelStart(0);
		getFlexGrid().put_EditSelLength(getFlexGrid().getCellText((int)Row, (int)Col).GetLength());

		*Cancel = VARIANT_TRUE;
	}
	else if (newHeader.CompareNoCase(oldHeader) != 0)
	{
		int headerIndex;
		if (m_columnNameToIndexMap.Lookup(newHeader, headerIndex))
		{
			ErrorMessage("Field name already used.  Please enter other field name.", "Field Name");
			getFlexGrid().put_EditText(oldHeader);
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText((int)Row, (int)Col).GetLength());

			*Cancel = VARIANT_TRUE;
		}
		else
		{
			int curSelIndex, oldIndex, newIndex;

			// Update Refdes combo
			curSelIndex = m_refdesCombo.GetCurSel();
			oldIndex = m_refdesCombo.FindString(0, oldHeader);
			m_refdesCombo.DeleteString(oldIndex);
			newIndex = m_refdesCombo.AddString(newHeader);
			if (oldIndex == curSelIndex)
			{
				m_refdesCombo.SetCurSel(newIndex);
			}

			// Update Partnumber combo
			curSelIndex = m_partnumberCombo.GetCurSel();
			oldIndex = m_partnumberCombo.FindString(0, oldHeader);
			m_partnumberCombo.DeleteString(oldIndex);
			newIndex = m_partnumberCombo.AddString(newHeader);
			if (oldIndex == curSelIndex)
			{
				m_partnumberCombo.SetCurSel(newIndex);
			}


			// Update x-Coordinate combo
			curSelIndex = m_xCoordinateCombo.GetCurSel();
			oldIndex = m_xCoordinateCombo.FindString(0, oldHeader);
			m_xCoordinateCombo.DeleteString(oldIndex);
			newIndex = m_xCoordinateCombo.AddString(newHeader);
			if (oldIndex == curSelIndex)
			{
				m_xCoordinateCombo.SetCurSel(newIndex);
			}

			// Update y-Coordinate combo
			curSelIndex = m_yCoordinateCombo.GetCurSel();
			oldIndex = m_yCoordinateCombo.FindString(0, oldHeader);
			m_yCoordinateCombo.DeleteString(oldIndex);
			newIndex = m_yCoordinateCombo.AddString(newHeader);
			if (oldIndex == curSelIndex)
			{
				m_yCoordinateCombo.SetCurSel(newIndex);
			}

			// Update Rotation combo
			curSelIndex = m_rotationCombo.GetCurSel();
			oldIndex = m_rotationCombo.FindString(0, oldHeader);
			m_rotationCombo.DeleteString(oldIndex);
			newIndex = m_rotationCombo.AddString(newHeader);
			if (oldIndex == curSelIndex)
			{
				m_rotationCombo.SetCurSel(newIndex);
			}

			// Update Surface combo
			curSelIndex = m_surfaceCombo.GetCurSel();
			oldIndex = m_surfaceCombo.FindString(0, oldHeader);
			m_surfaceCombo.DeleteString(oldIndex);
			newIndex = m_surfaceCombo.AddString(newHeader);
			if (oldIndex == curSelIndex)
			{
				m_surfaceCombo.SetCurSel(newIndex);
			}

			m_columnNameToIndexMap.RemoveKey(oldHeader);
			m_columnNameToIndexMap.SetAt(newHeader, (int)Col);
		}
	}
}

void CGenericCentroidImportDlg::OnCbnSelchangeSurfaceCombo()
{
	CString colName;
	int col = -1;

	m_surfaceCombo.GetWindowText(colName);
	if (colName.IsEmpty())
		return;

	m_columnNameToIndexMap.Lookup(colName, col);
	if (col < 0)
		return;

	for (int row=2; row<getFlexGrid().get_Rows(); row++)
	{
		CString value = getFlexGrid().getCellText(row, col);
		if (m_topCombo.FindString(0, value) == CB_ERR)
			m_topCombo.AddString(value);

		value.MakeLower();
		if (value.GetAt(0) == 't')
			m_topCombo.SelectString(0, value);
	}
}

FileStruct* CGenericCentroidImportDlg::getCentroidFile()
{
	CFilePath filePath(m_fileName);

   FileStruct* centroidFile = m_camCadDatabase.getCamCadDoc().getFileList().FindByName(filePath.getBaseFileName());

   if (centroidFile == NULL)
   {
	   centroidFile = m_camCadDatabase.getCamCadDoc().Add_File(filePath.getBaseFileName(), fileTypeGenericCentroid); //fileTypeUnknown);
	   centroidFile->setBlockType(blockTypePcb);
	   centroidFile->setNotPlacedYet(false);
	   centroidFile->setShow(true);
   }

	return centroidFile;
}

bool CGenericCentroidImportDlg::areAllFieldsAssigned()
{
	CString refdesField, partNumberField, xField, yField, rotationField, surfaceField, topField, bottomField;

	m_refdesCombo.GetWindowText(refdesField);
	m_partnumberCombo.GetWindowText(partNumberField);
	m_xCoordinateCombo.GetWindowText(xField);
	m_yCoordinateCombo.GetWindowText(yField);
	m_rotationCombo.GetWindowText(rotationField);
	m_surfaceCombo.GetWindowText(surfaceField);
	m_topCombo.GetWindowText(topField);

	if (refdesField.IsEmpty())
	{
		ErrorMessage("Refdes field is missing.  Please select value for the field.", "Missing Refdes");
		m_refdesCombo.SetFocus();
	}
	else if(partNumberField.IsEmpty())
	{
		ErrorMessage("Partnumber field is missing.  Please select value for the field.", "Missing Partnumber");
		m_partnumberCombo.SetFocus();
	}
	else if(xField.IsEmpty())
	{
		ErrorMessage("X-coordinate field is missing.  Please select value for the field.", "Missing X-coordinate");
		m_xCoordinateCombo.SetFocus();
	}
	else if(yField.IsEmpty())
	{
		ErrorMessage("Y-coordinate is missing.  Please select value for the field.", "Missing Y-coordinate");
		m_yCoordinateCombo.SetFocus();
	}
	else if(rotationField.IsEmpty())
	{
		ErrorMessage("Rotation field is missing.  Please select value for the field.", "Missing Rotation");
		m_rotationCombo.SetFocus();
	}
	else if(surfaceField.IsEmpty() && m_surfaceOption == surfaceBoth)
	{
		ErrorMessage("Surface field is missing.  Please select value for the field.", "Missing Surface");
		m_surfaceCombo.SetFocus();
	}
	else if(topField.IsEmpty() && m_surfaceOption == surfaceBoth)
	{
		ErrorMessage("Top value field is missing.  Please select value for the field.", "Missing Top Value");
		m_topCombo.SetFocus();
	}
	else
	{
		return true;
	}

	return false;
}

void CGenericCentroidImportDlg::OnBnClickedSurfaceOption()
{
	UpdateData(TRUE);

	if (m_surfaceOption == surfaceBoth)
	{
		m_surfaceCombo.EnableWindow(TRUE);
		m_topCombo.EnableWindow(TRUE);
		GetDlgItem(IDC_TopCombo)->EnableWindow(TRUE);
	}
	else
	{
		m_surfaceCombo.EnableWindow(FALSE);
		m_topCombo.EnableWindow(FALSE);
		GetDlgItem(IDC_TopCombo)->EnableWindow(FALSE);
	}
}

void CGenericCentroidImportDlg::OnBnClickedCreateGeomTypeOption()
{
	UpdateData(TRUE);
/*
	if (m_surfaceOption == surfaceBoth)
	{
		m_surfaceCombo.EnableWindow(TRUE);
		m_topCombo.EnableWindow(TRUE);
		GetDlgItem(IDC_TopCombo)->EnableWindow(TRUE);
	}
	else
	{
		m_surfaceCombo.EnableWindow(FALSE);
		m_topCombo.EnableWindow(FALSE);
		GetDlgItem(IDC_TopCombo)->EnableWindow(FALSE);
	}
   */
}

void CGenericCentroidImportDlg::OnBnClickedOk()
{
	if (!areAllFieldsAssigned())
		return;

	CResizingDialog::OnOK();

	FileStruct* centroidFile = getCentroidFile();
	BlockStruct* centroidBlock = m_camCadDatabase.getDefinedCentroidGeometry(centroidFile->getFileNumber());

   BlockStruct* fakecompBlock = m_camCadDatabase.copyBlock("SurrogateComponent", centroidBlock);
   fakecompBlock->setBlockType(blockTypePcbComponent);

	// Keyword index
	int dataTypeKw   = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);
	int dataSourceKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);
	int refdesKw     = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeRefName);
	int partNumberKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributePartNumber);

	// Get centroid layers and add "DataSource" and "DataType" to them
   CString dataSource = gerberEducatorDataSourceToString(gerberEducatorDataSourceCentroid);
   CString dataType   = gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeTop);

	LayerStruct* centroidTopLayer = m_camCadDatabase.getLayer(ccLayerCentroidTop);
   m_camCadDatabase.addAttribute(centroidTopLayer->attributes(),dataSourceKw,dataSource);
   m_camCadDatabase.addAttribute(centroidTopLayer->attributes(),dataTypeKw  ,dataType);

   dataType   = gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeBottom);

	LayerStruct* centrodBottomLayer = m_camCadDatabase.getLayer(ccLayerCentroidBottom);
   m_camCadDatabase.addAttribute(centrodBottomLayer->attributes(),dataSourceKw,dataSource);
   m_camCadDatabase.addAttribute(centrodBottomLayer->attributes(),dataTypeKw  ,dataType);

   centroidTopLayer->setMirroredLayerIndex(centrodBottomLayer->getLayerIndex());
   centrodBottomLayer->setMirroredLayerIndex(centroidTopLayer->getLayerIndex());

	// Get neccessary header and column index
	CString refdesHeader, partNumberHeader, xHeader, yHeader, rotationHeader, surfaceHeader, topValue, bottomValue;
	int refdesIndex, partNumberIndex, xIndex, yIndex, rotationIndex, surfaceIndex;

	m_refdesCombo.GetWindowText(refdesHeader);
	m_partnumberCombo.GetWindowText(partNumberHeader);
	m_xCoordinateCombo.GetWindowText(xHeader);
	m_yCoordinateCombo.GetWindowText(yHeader);
	m_rotationCombo.GetWindowText(rotationHeader);
	m_surfaceCombo.GetWindowText(surfaceHeader);
	m_topCombo.GetWindowText(topValue);

	m_columnNameToIndexMap.Lookup(refdesHeader, refdesIndex);
	m_columnNameToIndexMap.Lookup(partNumberHeader, partNumberIndex);
	m_columnNameToIndexMap.Lookup(xHeader, xIndex);
	m_columnNameToIndexMap.Lookup(yHeader, yIndex);
	m_columnNameToIndexMap.Lookup(rotationHeader, rotationIndex);
	m_columnNameToIndexMap.Lookup(surfaceHeader, surfaceIndex);	

	// TODO: Need to allow user to select unit
	CString xyUnitValue;
	m_unitsCombo.GetWindowText(xyUnitValue);
	PageUnitsTag xyUnits = unitStringAbbreviationToTag(xyUnitValue);
	double unitFactor = Units_Factor(xyUnits, m_camCadDatabase.getCamCadDoc().getSettings().getPageUnits());
	
	for (int row=2; row<getFlexGrid().get_Rows(); row++)
	{
		CString refdes = getFlexGrid().getCellText(row, refdesIndex);
		CString partNumber = getFlexGrid().getCellText(row, partNumberIndex);
		double x = unitFactor * (is_number(getFlexGrid().getCellText(row, xIndex))?euro_atof(getFlexGrid().getCellText(row, xIndex)):0.0);
		double y = unitFactor * (is_number(getFlexGrid().getCellText(row, yIndex))?euro_atof(getFlexGrid().getCellText(row, yIndex)):0.0);
		double rotation = is_number(getFlexGrid().getCellText(row, rotationIndex))?euro_atof(getFlexGrid().getCellText(row, rotationIndex)):0.0;
		double angleRadians = DegToRad(rotation);

		bool mirror = true;

		if (m_surfaceOption == surfaceBoth)
		{
			if (getFlexGrid().getCellText(row, surfaceIndex).CompareNoCase(topValue) == 0)
				mirror = false;
		}
		else if (m_surfaceOption == surfaceTop)
		{
			mirror = false;
		}

		Attrib* attrib = NULL;
      DataStruct* centroid = NULL;
      DataStruct* fakecomp = NULL; //*rcf change var names to indicate these are insert data
      if (m_createGeomTypeOption == eCentroidsOnly)
      {
         centroid = m_camCadDatabase.insertBlock(centroidBlock, insertTypeCentroid, refdes, mirror?centrodBottomLayer->getLayerIndex():centroidTopLayer->getLayerIndex(), x, y, angleRadians, 0, 1.0);
         centroid->getInsert()->setPlacedBottom(mirror);
         centroid->getInsert()->setGraphicsMirrorFlag(mirror);
         centroid->setAttrib(m_camCadDatabase.getCamCadDoc().getCamCadData(), refdesKw, valueTypeString, refdes.GetBuffer(0), attributeUpdateOverwrite, NULL);
         centroid->setAttrib(m_camCadDatabase.getCamCadDoc().getCamCadData(), partNumberKw, valueTypeString, partNumber.GetBuffer(0), attributeUpdateOverwrite, NULL);

         centroidFile->getBlock()->getDataList().AddTail(centroid);
      }

      if (m_createGeomTypeOption == ePcbComponents)
      {
         CString pcbCompGeomName( "FakeComp" ); // default
         if (!partNumber.IsEmpty())
            pcbCompGeomName = partNumber;
         BlockStruct *fakecompBlk = GetDefinedPcbComponent(pcbCompGeomName);
         if (fakecompBlk != NULL)
         {
            // Add centroid
            DataStruct* compCentroid = m_camCadDatabase.insertBlock(centroidBlock, insertTypeCentroid, "$$centroid$$", centroidTopLayer->getLayerIndex(), 0., 0., 0., false, 1.0);
            ///compCentroid->setAttrib(m_camCadDatabase.getCamCadDoc().getCamCadData(), refdesKw, valueTypeString, refdes.GetBuffer(0), attributeUpdateOverwrite, NULL);
		      ///compCentroid->setAttrib(m_camCadDatabase.getCamCadDoc().getCamCadData(), partNumberKw, valueTypeString, partNumber.GetBuffer(0), attributeUpdateOverwrite, NULL);
		      fakecompBlk->getDataList().AddTail(compCentroid);

            fakecomp = m_camCadDatabase.insertBlock(fakecompBlk, insertTypePcbComponent, refdes, /*m_camCadDatabase.getFloatingLayerIndex()*/ -1 /*mirror?centrodBottomLayer->getLayerIndex():centroidTopLayer->getLayerIndex()*/, x, y, angleRadians, 0, 1.0);
            fakecomp->getInsert()->setPlacedBottom(mirror);
            fakecomp->getInsert()->setGraphicsMirrorFlag(mirror);
            fakecomp->getInsert()->setLayerMirrorFlag(mirror);
            fakecomp->setAttrib(m_camCadDatabase.getCamCadDoc().getCamCadData(), refdesKw, valueTypeString, refdes.GetBuffer(0), attributeUpdateOverwrite, NULL);
            fakecomp->setAttrib(m_camCadDatabase.getCamCadDoc().getCamCadData(), partNumberKw, valueTypeString, partNumber.GetBuffer(0), attributeUpdateOverwrite, NULL);
            centroidFile->getBlock()->getDataList().AddTail(fakecomp);
         }
      }

      for (int col=0; col<getFlexGrid().get_Cols(); col++)
      {
         if (col != refdesIndex		&& col != partNumberIndex	&&
            col != xIndex				&& col != yIndex				&&
            col != rotationIndex	&& col != surfaceIndex		)
         {
            CString keyword = getFlexGrid().getCellText(0, col);
            CString value = getFlexGrid().getCellText(row, col);

            if (centroid != NULL)
               m_camCadDatabase.getCamCadDoc().SetUnknownAttrib(&centroid->getAttributesRef(), keyword, value, attributeUpdateOverwrite, NULL);

            if (fakecomp != NULL)
               m_camCadDatabase.getCamCadDoc().SetUnknownAttrib(&fakecomp->getAttributesRef(), keyword, value, attributeUpdateOverwrite, NULL);
         }
      }
	}
}

BlockStruct *CGenericCentroidImportDlg::GetDefinedPcbComponent(CString geometryName)
{
   BlockStruct *compBlk = NULL;

   if (!geometryName.IsEmpty())
   {
      int fileNum = getCentroidFile() != NULL ? getCentroidFile()->getFileNumber() : -1;
      compBlk = m_camCadDatabase.getBlock(geometryName, fileNum);

      if (compBlk == NULL)  // If does not exist then create it
      {
         compBlk = m_camCadDatabase.getDefinedBlock(geometryName, blockTypePcbComponent, fileNum);

         if (compBlk != NULL)
         {
            BlockStruct *padstackBlk = m_camCadDatabase.getBlock("ArbitraryPadstack", fileNum);
            if (padstackBlk == NULL)  // If padstack does not exist then create it
            {
               padstackBlk = m_camCadDatabase.getDefinedBlock("ArbitraryPadstack", blockTypeUnknown, fileNum);

               if (padstackBlk != NULL)
               {
                  // Just get aperture, will get created if not yet existing
                  double sizeA = m_camCadDatabase.convertToPageUnits(pageUnitsInches, 0.010); // Default 10 mils pad dia
                  BlockStruct *apBlk = m_camCadDatabase.getDefinedAperture(fileNum, "ArbitraryAperture", apertureRound, sizeA, 0., 0., 0., 0.);
                  if (apBlk != NULL)
                  {
                     // Insert aperture into padstack
                     LayerStruct* centroidTopLayer = m_camCadDatabase.getLayer(ccLayerPadTop);
                     int padtopLayerIndex = m_camCadDatabase.getLayerIndex(ccLayerPadTop);
                     DataStruct* padInsertData = m_camCadDatabase.insertBlock(apBlk, insertTypeAperture, "", padtopLayerIndex, 0., 0., 0., false, 1.0);
                     padstackBlk->getDataList().AddTail(padInsertData);
                  }
               }
            }
            if (padstackBlk != NULL)
            {
               // Insertpadstack (aka pin) into component
               DataStruct* pinInsertData = m_camCadDatabase.insertBlock(padstackBlk, insertTypePin, "1", -1 /*m_camCadDatabase.getFloatingLayerIndex()*/, 0., 0., 0., false, 1.0);
               compBlk->getDataList().AddTail(pinInsertData);
            }
         }
      }
   }

   return compBlk;
}

void CGenericCentroidImportDlg::OnBnClickedCancel()
{
	CResizingDialog::OnClose();

   EndDialog(IDCANCEL);
}

void CGenericCentroidImportDlg::OnBnClickedReject()
{
	m_rejected = true;
	OnBnClickedCancel();
}
