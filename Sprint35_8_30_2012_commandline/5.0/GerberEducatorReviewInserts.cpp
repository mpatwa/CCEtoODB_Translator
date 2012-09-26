// $Header: /CAMCAD/5.0/GerberEducatorReviewInserts.cpp 62    6/17/07 8:52p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "PanRef.h"
#include "MainFrm.h"
#include "Colors.h"
#include "CCEtoODB.h"
#include "GerberEducatorReviewInserts.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);
int PanReference(CCEtoODBView *view, const char *ref);


//-----------------------------------------------------------------------------
// CGEReviewInsertsDlg dialog
//-----------------------------------------------------------------------------
CGEReviewInsertsDlg::CGEReviewInsertsDlg(CGerberEducatorUi& gerberEducatorUi)
	: CResizingDialog(CGEReviewInsertsDlg::IDD)
	, m_gerberEducatorUi(gerberEducatorUi)
	, m_camCadDoc(gerberEducatorUi.getCamCadDoc())
	, m_camCadDatabase(gerberEducatorUi.getCamCadDoc())
	, m_fileStruct(*gerberEducatorUi.getFileStruct())
	, m_viewOption(m_viewTop)
	, m_curZoomMargin(1)
   , m_maxZoomMargin(20)
	, m_cellEditError(cellEditErrorNone)
   , m_toolBar(*this)
	, m_turnOnPinLabel(FALSE)
{
	m_camCadDatabase.associateExistingLayers();
	m_initialized = false;
	m_pinLabelTopWasOn = false;
	m_pinLabelBottomWasOn = false;

	addFieldControl(IDC_TopViewRadio							, anchorBottomLeft);
	addFieldControl(IDC_BottomViewRadio						, anchorBottomLeft);
	addFieldControl(IDC_BothViewRadio						, anchorBottomLeft);
	addFieldControl(IDC_ViewGroupStatic						, anchorBottomLeft);
	addFieldControl(IDC_TurnOnPinLabel						, anchorBottomLeft);

	addFieldControl(IDC_ZoomSlider							, anchorBottomLeft);
	addFieldControl(IDC_ZoomStatic							, anchorBottomLeft);

	addFieldControl(IDC_InsertGridStatic					, anchorLeft, growBoth);
	addFieldControl(IDC_ReviewCurrentGeometry				, anchorBottomLeft);
	addFieldControl(IDC_CLOSE									, anchorBottomLeft);

	addFieldControl(IDC_SummaryGroup							, anchorBottomLeft);
	addFieldControl(IDC_ReviewQtyLabel						, anchorBottomLeft);
	addFieldControl(IDC_GeomTypeLabel						, anchorBottomLeft);
	addFieldControl(IDC_PcbCompLabel							, anchorBottomLeft);
	addFieldControl(IDC_MechCompLabel						, anchorBottomLeft);
	addFieldControl(IDC_ToolingLabel							, anchorBottomLeft);
	addFieldControl(IDC_FiducialLabel						, anchorBottomLeft);

	addFieldControl(IDC_TopReviewQtyLabel					, anchorBottomLeft);
	addFieldControl(IDC_TopPcbCompReviewQtyLabel			, anchorBottomLeft);
	addFieldControl(IDC_TopMechCompReviewQtyLabel		, anchorBottomLeft);
	addFieldControl(IDC_TopToolingReviewQtyLabel			, anchorBottomLeft);
	addFieldControl(IDC_TopFiducialReviewQtyLabel		, anchorBottomLeft);

	addFieldControl(IDC_BottomReviewQtyLabel				, anchorBottomLeft);
	addFieldControl(IDC_BottomPcbCompReviewQtyLabel		, anchorBottomLeft);
	addFieldControl(IDC_BottomMechCompReviewQtyLabel	, anchorBottomLeft);
	addFieldControl(IDC_BottomToolingReviewQtyLabel		, anchorBottomLeft);
	addFieldControl(IDC_BottomFiducialReviewQtyLabel	, anchorBottomLeft);
}

CGEReviewInsertsDlg::~CGEReviewInsertsDlg()
{
}

void CGEReviewInsertsDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_InsertGridStatic, m_insertGrid);
	DDX_Radio(pDX, IDC_TopViewRadio, m_viewOption);
	DDX_Control(pDX, IDC_ZoomSlider, m_zoomSlider);
	DDX_Check(pDX, IDC_TurnOnPinLabel, m_turnOnPinLabel);

	DDX_Control(pDX, IDC_GeomTypeLabel, m_geomTypeLabel);
	DDX_Control(pDX, IDC_ReviewQtyLabel, m_reviewQtyLabel);
	DDX_Control(pDX, IDC_PcbCompLabel, m_pcbCompLabel);
	DDX_Control(pDX, IDC_MechCompLabel, m_mechCompLabel);
	DDX_Control(pDX, IDC_ToolingLabel, m_toolingLabel);
	DDX_Control(pDX, IDC_FiducialLabel, m_fiducialLabel);

	DDX_Control(pDX, IDC_TopReviewQtyLabel, m_topReviewQtyLabel);
	DDX_Control(pDX, IDC_TopPcbCompReviewQtyLabel, m_topReviewQuantitySummary.getPcbCompReviewQtyLabel());
	DDX_Control(pDX, IDC_TopMechCompReviewQtyLabel, m_topReviewQuantitySummary.getMechCompReviewQtyLabel());
	DDX_Control(pDX, IDC_TopToolingReviewQtyLabel, m_topReviewQuantitySummary.getToolingReviewQtyLabel());
	DDX_Control(pDX, IDC_TopFiducialReviewQtyLabel, m_topReviewQuantitySummary.getFiducialReviewQtyLabel());

	DDX_Control(pDX, IDC_BottomReviewQtyLabel, m_bottomReviewQtyLabel);
	DDX_Control(pDX, IDC_BottomPcbCompReviewQtyLabel, m_bottomReviewQuantitySummary.getPcbCompReviewQtyLabel());
	DDX_Control(pDX, IDC_BottomMechCompReviewQtyLabel, m_bottomReviewQuantitySummary.getMechCompReviewQtyLabel());
	DDX_Control(pDX, IDC_BottomToolingReviewQtyLabel, m_bottomReviewQuantitySummary.getToolingReviewQtyLabel());
	DDX_Control(pDX, IDC_BottomFiducialReviewQtyLabel, m_bottomReviewQuantitySummary.getFiducialReviewQtyLabel());
}

BEGIN_MESSAGE_MAP(CGEReviewInsertsDlg, CResizingDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TopViewRadio, OnBnClickedViewOpton)
	ON_BN_CLICKED(IDC_BottomViewRadio, OnBnClickedViewOpton)
	ON_BN_CLICKED(IDC_BothViewRadio, OnBnClickedViewOpton)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_ZoomSlider, OnNMReleasedcaptureZoomslider)
	ON_BN_CLICKED(IDC_ReviewCurrentGeometry, OnBnClickedReviewcurrentgeometry)
	ON_BN_CLICKED(IDC_TurnOnPinLabel, OnBnClickedTurnonpinlabel)

   ON_COMMAND(IDC_ReviewComponentZoomIn, OnBnClickedZoomIn)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomIn, OnUpdateZoomIn)
   ON_COMMAND(IDC_ReviewComponentZoomOut, OnBnClickedZoomOut)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomOut, OnUpdateZoomOut)
   ON_COMMAND(IDC_ReviewComponentZoomWindow, OnBnClickedZoomWindow)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomWindow, OnUpdateZoomWindow)
   ON_COMMAND(IDC_ReviewComponentZoomExtents, OnBnClickedZoomExtents)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomExtents, OnUpdateZoomExtents)
   ON_COMMAND(IDC_ReviewComponentZoomOneToOne, OnBnClickedZoomFull)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentZoomOneToOne, OnUpdateZoomFull)
   ON_COMMAND(IDC_ReviewComponentPanCenter, OnBnClickedPanCenter)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentPanCenter, OnUpdatePanCenter)

   ON_COMMAND(IDC_ReviewComponentQueryItem, OnBnClickedQueryItem)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentQueryItem, OnUpdateQueryItem)
   ON_COMMAND(IDC_ReviewComponentRepaint, OnBnClickedRepaint)
   ON_UPDATE_COMMAND_UI(IDC_ReviewComponentRepaint, OnUpdateRepaint)

	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGEReviewInsertsDlg, CResizingDialog)
	//ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, DISPID_CLICK, OnClickInsertGrid, VTS_NONE)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x1a, KeyDownEditInsertGrid, VTS_I4 VTS_I4 VTS_PI2 VTS_I2)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x7, AfterRowColChangeOnInsertGrid, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x18, AfterEditInsertGrid, VTS_I4 VTS_I4)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x3, EnterCellInsertGrid, VTS_NONE)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x15, BeforeEditInsertGrid, VTS_I4 VTS_I4 VTS_BOOL)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x1c, ChangeEditInsertGrid, VTS_NONE)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x17, ValidateEditInsertGrid, VTS_I4 VTS_I4 VTS_BOOL)
	ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0x5, BeforeMouseDownInsertGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4 VTS_BOOL)
   ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0xd, BeforeSortInsertGrid, VTS_I4 VTS_PI2)
   ON_EVENT(CGEReviewInsertsDlg, ID_InsertGrid, 0xe, AfterSortInsertGrid, VTS_I4 VTS_PI2)
END_EVENTSINK_MAP()

BOOL CGEReviewInsertsDlg::OnInitDialog()
{
	// Call OnInitDialog(), then createAndLoad(), then restoreWindowState()
	CResizingDialog::OnInitDialog();
   m_toolBar.createAndLoad(IDR_GerberEducatorReviewComponentToolbar);
	CResizingDialog::restoreWindowState();

	loadDialogSettings();

	m_zoomSlider.SetRange(0, m_maxZoomMargin);
	m_zoomSlider.SetPos(m_curZoomMargin);
	
	// Turn on pin label
	m_pinLabelTopWasOn = m_camCadDoc.showPinnrsTop==TRUE;
	m_pinLabelBottomWasOn = m_camCadDoc.showPinnrsBottom==TRUE;


	m_reviewQtyLabel.SetBkColor(colorHtmlLightSkyBlue);
   m_topReviewQtyLabel.SetBkColor(colorHtmlLightSkyBlue);
   m_bottomReviewQtyLabel.SetBkColor(colorHtmlLightSkyBlue);
   m_geomTypeLabel.SetBkColor(colorHtmlLightSkyBlue);
   m_pcbCompLabel.SetBkColor(colorWhite);
   m_mechCompLabel.SetBkColor(colorWhite);
   m_toolingLabel.SetBkColor(colorWhite);
	m_fiducialLabel.SetBkColor(colorWhite);

	m_topReviewQuantitySummary.setBackGroundColor(colorWhite);
	m_bottomReviewQuantitySummary.setBackGroundColor(colorWhite);

	m_insertGrid.substituteForDlgItem(ID_InsertGrid,IDC_InsertGridStatic,"FlexGrid",*this);
	setView();
	loadGrid();
	
	if (m_turnOnPinLabel)
		m_camCadDatabase.getCamCadDoc().generatePinLocations(false);
	turnOnPinLabel();

	// Set layer selection filter
   CLayerFilter layerFilter(true);
	layerFilter.removeAll();
	LayerStruct* layer = m_camCadDatabase.getLayer(m_camCadDatabase.getCamCadLayerName(ccLayerPadTop));
	if (layer != NULL)
		layerFilter.add(layer->getLayerIndex());
	layer = m_camCadDatabase.getLayer(m_camCadDatabase.getCamCadLayerName(ccLayerPadBottom));
	if (layer != NULL)
		layerFilter.add(layer->getLayerIndex());
	m_camCadDoc.getSelectLayerFilterStack().push(layerFilter);

	m_initialized = true;

	// Need to update view so show overrided colors
	m_camCadDoc.UpdateAllViews(NULL);
	return TRUE;
}

void CGEReviewInsertsDlg::initGrid()
{
	// Initialize geometries grid
   getFlexGrid().clear();
   getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
	getFlexGrid().put_AllowSelection(FALSE);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);
   getFlexGrid().put_Cols(m_colCount);
   getFlexGrid().put_ExplorerBar(CFlexGrid::flexExSort);
   getFlexGrid().put_ExtendLastCol(true);
   getFlexGrid().put_FixedCols(0);
   getFlexGrid().put_FixedRows(1);
	getFlexGrid().put_FocusRect(CFlexGrid::flexFocusInset);
	getFlexGrid().put_HighLight(CFlexGrid::flexHighlightAlways);
   getFlexGrid().put_Rows(1);
	getFlexGrid().put_ScrollBars(CFlexGrid::flexScrollBarBoth);
	getFlexGrid().put_SelectionMode(CFlexGrid::flexSelectionByRow);

	getFlexGrid().put_ColDataType((long)m_colReview, CFlexGrid::flexDTBoolean);
   getFlexGrid().setCell(0, m_colReview, CONST_REVIEW);
   getFlexGrid().setCell(0, m_colRefdes, CONST_REFDES);
   getFlexGrid().setCell(0, m_colType, CONST_TYPE);
   getFlexGrid().setCell(0, m_colRotation, CONST_ROTATION);
   getFlexGrid().setCell(0, m_colSurface, CONST_SURFACE);
	getFlexGrid().setCell(0, m_colGeometryName, CONST_GEOMETRYNAME);


	COleVariant zero((long)0);
	getFlexGrid().put_Cell(CFlexGrid::flexcpAlignment, 
		zero, zero, zero, (COleVariant)(long)(m_colCount -1), (COleVariant)(long)CFlexGrid::flexAlignCenterCenter);

   getFlexGrid().autoSizeColumns();
	getFlexGrid().put_ColDataType(m_colRotation, CFlexGrid::flexDTString);
	getFlexGrid().put_ColAlignment(m_colRotation, CFlexGrid::flexAlignRightCenter);
	getFlexGrid().put_ColAlignment(m_colReview, CFlexGrid::flexAlignCenterCenter);
	getFlexGrid().put_ColAlignment(m_colRefdes, CFlexGrid::flexAlignLeftCenter);
	getFlexGrid().put_ColAlignment(m_colGeometryName, CFlexGrid::flexAlignLeftCenter);

	// Hide the column because it is only user to store the list of degrees that should be allowed for each insert
	// The data is filled in loadGrid()
	getFlexGrid().put_ColHidden((long)m_colDegrees, TRUE);
	getFlexGrid().put_ColHidden((long)m_colSortableRefDes, TRUE);
}

void CGEReviewInsertsDlg::loadGrid()
{
	BlockStruct* block = m_fileStruct.getBlock();
	if (block == NULL)
		return;

	initGrid();
	m_topReviewQuantitySummary.clearSummary();
	m_bottomReviewQuantitySummary.clearSummary();


	// Create log file
	bool logFileCreated = true;
	bool displayLog = false;
	CString logFileName = GetLogfilePath("GerberEducatorReviewComponent.log");
	CFormatStdioFile logFile;
	if (!logFile.Open(logFileName, CFile::modeCreate|CFile::modeWrite))   // rewrite file
   {
      CString tmp;
      tmp.Format("Can not open log file [%s]", logFileName);
      ErrorMessage(tmp, "Error Opening File");
		logFileCreated = false;
   }
	

   CMapStringToString appliedColorSetBlockMap;
	for (POSITION pos=block->getHeadDataPosition(); pos != NULL;)
	{
		DataStruct* data = block->getNextDataInsert(pos);
		if (data == NULL)
			continue;

		// Initally remove color override on all data, incase there is override color on draws
		data->setColorOverride(false);

		if (data->getInsert() == NULL)
			continue;

		InsertTypeTag insertType = data->getInsert()->getInsertType();
		if (insertType != insertTypePcbComponent	&& insertType != insertTypeFiducial		&&
			 insertType != insertTypeDrillTool		&& insertType != insertTypeMechanicalComponent)
			 continue;

      //BlockStruct* componentBlock = m_camCadDatabase.getCamCadDoc().getBlockAt(data->getInsert()->getBlockNumber());
      //CString blockName = componentBlock->getName();
      //if (!appliedColorSetBlockMap.Lookup(blockName, blockName))
      //{
      //   CString technology;
      //   int technologyKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeTechnology);
      //   m_camCadDatabase.getAttributeStringValue(technology, &block->getAttributesRef(), technologyKw);

      //   // Override color of pins for THRU geometries
      //   if (technology.CompareNoCase("THRU") == 0)
      //   {
      //      CInsertTypeMask insertTypeMask(insertTypePin, insertTypeMechanicalPin, insertTypeFiducial);
      //      for (CDataListIterator dataList(*componentBlock, insertTypeMask); dataList.hasNext();)
      //      {
      //         DataStruct* pinData = dataList.getNext();
      //         if (pinData == NULL)
      //            continue;

      //         // Override the color of data
      //         pinData->setOverrideColor(m_layerColor.getReviewComponentTHPadTopColor());
      //         pinData->setColorOverride(true);
      //      }
      //   }

      //   appliedColorSetBlockMap.SetAt(blockName, blockName);
      //}

		if (data->getInsert()->getRefname().IsEmpty())
			continue;

		if (!m_gerberEducatorComponents.Lookup(data->getInsert()->getRefname(), data))
		{
			m_gerberEducatorComponents.SetAt(data->getInsert()->getRefname(), data);
		}
	}

	long selectedRow = -1;
	CMapStringToString refdesMap;
	for (POSITION pos=block->getHeadDataInsertPosition(); pos != NULL;)
	{
		POSITION curPos = pos;
		DataStruct* data = block->getNextData(pos);
		if (data == NULL || data->getInsert() == NULL)
			continue;

		InsertStruct* insert = data->getInsert();
		InsertTypeTag insertType = insert->getInsertType();
		if (insertType != insertTypePcbComponent	&& insertType != insertTypeFiducial		&&
			 insertType != insertTypeDrillTool		&& insertType != insertTypeMechanicalComponent)
			 continue;

		bool reviewed = isReviewed(data);
		CString refDes = insert->getRefname();

		if (refdesMap.Lookup(refDes, refDes))
		{
			// duplicate refdes
			refDes = m_gerberEducatorComponents.getRefDesMap().defineCeiling("GE_",QRefDesSuffixFormat);
			refdesMap.SetAt(refDes, refDes);

			logFile.WriteString("Duplicate refdes [%s] found and renamed to [%s]\n", insert->getRefname(), refDes);
			displayLog = true;
		}
		else if(refDes.IsEmpty() && insertType == insertTypePcbComponent)
		{
			// refdes is empty for PCB Component
			refDes = m_gerberEducatorComponents.getRefDesMap().defineCeiling("GE_",QRefDesSuffixFormat);
			refdesMap.SetAt(refDes, refDes);

			logFile.WriteString("PCB Component with empty refdes found and renamed to [%s]\n", refDes);
			displayLog = true;
		}

		// Update summary table
		if (!insert->getGraphicMirrored())
			m_topReviewQuantitySummary.addReviewQuantityTo(insertType, reviewed?1:0, 1);
		else
			m_bottomReviewQuantitySummary.addReviewQuantityTo(insertType, reviewed?1:0, 1);

		if (m_viewOption == m_viewTop		&& insert->getGraphicMirrored() || 
			 m_viewOption == m_viewBottom	&& !insert->getGraphicMirrored())
			continue;

		BlockStruct* compBlock = m_camCadDoc.getBlockAt(insert->getBlockNumber());
		CString geomName = compBlock->getName();
		COleVariant rowData((long)curPos);

		CString item = getAllowDegrees(data);
		item.AppendFormat("\t%s", insert->getSortableRefDes());
		item.AppendFormat("\t%d", reviewed?1:0);
		item.AppendFormat("\t%s", refDes);
		item.AppendFormat("\t%s", insertTypeToDisplayString(insertType));
		item.AppendFormat("\t%0.2f", normalizeDegrees(insert->getAngleDegrees()));
		item.AppendFormat("\t%s", insert->getGraphicMirrored()?"Bottom":"Top");
		item.AppendFormat("\t%s", geomName);

		getFlexGrid().addItem(item, (int)getFlexGrid().get_Rows());
		getFlexGrid().put_RowData(getFlexGrid().get_Rows() - 1, rowData);

		// Set overrid colors to indicate review status of data
		setReviewedDataColor(data, getFlexGrid().get_Rows() - 1, reviewed);
	}

	if (getFlexGrid().get_Rows() > 1)
	{
		getFlexGrid().put_ColComboList((long)m_colType, "PCB Component|Fiducial|Tooling|Mechanical Component");
		getFlexGrid().autoSizeColumns();

		getFlexGrid().put_ColSort(m_colSortableRefDes, CFlexGrid::flexSortGenericAscending);
		getFlexGrid().select(1, m_colSortableRefDes, 1, m_colSortableRefDes);
		getFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);

		// Find selected row
		long selectedRow = 1;
		if (!m_gerberEducatorUi.getCurrentEditComponentRefdes().IsEmpty())
		{
			for (int row=1; row<getFlexGrid().get_Rows(); row++)
			{
				CString refdes = getFlexGrid().getCellText(row, m_colRefdes);
				if (m_gerberEducatorUi.getCurrentEditComponentRefdes().CompareNoCase(refdes) == 0)
				{
					selectedRow = row;
					break;
				}
			}

         m_gerberEducatorUi.setCurrentEditComponentRefdes("");

		   getFlexGrid().select(selectedRow, m_colRefdes, selectedRow, m_colRefdes);
		   getFlexGrid().ShowCell(selectedRow, 0);

		}

      OnClickInsertGrid();
	}

	if (logFileCreated)
	{
		logFile.Close();

		if (displayLog)
		{
			Logreader(logFileName);
		}
	}
}

void CGEReviewInsertsDlg::setView()
{
	UpdateData(TRUE);

	if (m_viewOption == m_viewBoth || m_viewOption == m_viewTop)
	{
		m_camCadDatabase.getCamCadDoc().OnTopview();
//		m_camCadDatabase.getCamCadDoc().OnOriginalColors();
	}	
	else if (m_viewOption == m_viewBottom)
	{
		m_camCadDatabase.getCamCadDoc().OnBottomview();
//		m_camCadDatabase.getCamCadDoc().OnOriginalColors();
	}

	int dataTypeKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);

	// Turn off all layers, except the only necessary top layers
	LayerStruct* layer;
	for (int i=0; i<m_camCadDatabase.getNumLayers(); i++)
	{
		layer = m_camCadDatabase.getLayerAt(i);
		if (layer == NULL)
			continue;

		CString dataType;
		m_camCadDatabase.getAttributeStringValue(dataType, &layer->getAttributesRef(), dataTypeKw);

      COLORREF layerColor = layer->getColor();
		bool visible = false;
		
		if ((m_viewOption == m_viewBoth || m_viewOption == m_viewTop))
		{
		   if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerAssemblyTop)) == 0)
         {
            layerColor = m_layerColor.getColor(gerberEducatorReviewComponentComponentOutlineTopColor);
			   visible = true;
         }
         else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidTop)) == 0)
         {
            layerColor = m_layerColor.getColor(gerberEducatorReviewComponentCentroidTopColor);
			   visible = true;
         }
	      else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerPadTop)) == 0)
         {
            layerColor = m_layerColor.getColor(gerberEducatorReviewComponentSmdPadTopColor);
			   visible = true;
         }
	      else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerDrillHoles)) == 0)
         {
			   visible = true;
         }
			else if (dataType.CompareNoCase(QDataTypeTopPad) == 0)
			{
				layerColor = m_layerColor.getColor(gerberEducatorSetupDataPadTopFlashesColor);
				visible = true;
			}
			else if (dataType.CompareNoCase(QDataTypeTopSilkscreen) == 0)
			{
				layerColor = m_layerColor.getColor(gerberEducatorSetupDataSilkscreenTopColor);
				visible = true;
			}
		}

		if ((m_viewOption == m_viewBoth || m_viewOption == m_viewBottom))
		{
         if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerAssemblyBottom)) == 0)
         {
            layerColor = m_layerColor.getColor(gerberEducatorReviewComponentComponentOutlineBottomColor);
			   visible = true;
         }
         else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidBottom))	== 0)
         {
            layerColor = m_layerColor.getColor(gerberEducatorReviewComponentCentroidBottomColor);
			   visible = true;
         }
         else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerPadBottom)) == 0)
         {
            layerColor = m_layerColor.getColor(gerberEducatorReviewComponentSmdPadBottomColor);
			   visible = true;
         }
	      else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerDrillHoles)) == 0)
         {
			   visible = true;
         }

			else if (dataType.CompareNoCase(QDataTypeBottomPad) == 0)
			{
				layerColor = m_layerColor.getColor(gerberEducatorSetupDataPadBottomFlashesColor);
				visible = true;
			}
			else if (dataType.CompareNoCase(QDataTypeBottomSilkscreen) == 0)
			{
				layerColor = m_layerColor.getColor(gerberEducatorSetupDataSilkscreenBottomColor);
				visible = true;
			}
		}

      layer->setColor(layerColor);
		layer->setVisible(visible);
	}
}

bool CGEReviewInsertsDlg::isReviewed(DataStruct* data)
{
	bool reviewed = false;

	if (data != NULL && data->getAttributes())
	{
		Attrib* attrib = NULL;
		int keywordIndex = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeGeomReviewStatus);
		if (data->getAttributes()->Lookup(keywordIndex, attrib))
		{
			CStringArray records;
			CSupString dataReviewStatus = attrib->getStringValue();
			dataReviewStatus.ParseQuote(records, "|");

			for (int index=0; index<records.GetCount(); index++)
			{
				CStringArray params;
				CSupString record = records.GetAt(index);
				record.ParseQuote(params, "=");

				if (params.GetCount() < 2)
					continue;

				if (params[0].CompareNoCase("Reviewed") == 0)
					reviewed = params[1].CompareNoCase("TRUE")==0?true:false;				
			}
		}
	}

	return reviewed;
}

CString CGEReviewInsertsDlg::getAllowDegrees(DataStruct* data)
{
	CString allowDegrees = "0.00";
	if (data == NULL)
		return allowDegrees;

	BlockStruct* block = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());
	if (block == NULL)
		return allowDegrees;

   double searchTolerance = m_camCadDatabase.convertToPageUnits(pageUnitsMils,.5);
	int symmetryCount = block->getDataList().getPinSymmetryCount(symmetryCriteriaPadstack,searchTolerance);

	if (symmetryCount > 0)
	{
		double insertDegree = normalizeDegrees(data->getInsert()->getAngleDegrees());
		double degreeIncrement = 360./symmetryCount;

      COrderedDoubleArray degrees(.1,symmetryCount);
      for (int index = 0;index < symmetryCount;index++)
      {
         degrees.add(normalizeDegrees(data->getInsert()->getAngleDegrees() + index*degreeIncrement));
      }

      allowDegrees.Empty();
		for (int count=0; count<symmetryCount; count++)
      {
         if (count > 0)
         {
            allowDegrees += "|";
         }

			allowDegrees.AppendFormat("%0.2f", degrees.getAt(count));
      }
	}
	else
	{
		allowDegrees.Format("%0.2f", normalizeDegrees(data->getInsert()->getAngleDegrees()));
	}

	return allowDegrees;
}

bool CGEReviewInsertsDlg::checkDuplicateRefdes(int editRow, CString newRefdes)
{
	bool retval = false;

	for (int i=0; i<getFlexGrid().get_Rows(); i++)
	{
		if (i == editRow)
			continue;

		CString existingRefdes = getFlexGrid().getCellText(i, m_colRefdes);
		if (existingRefdes.CompareNoCase(newRefdes) == 0)
		{
			retval = true;
			break;
		}
	}

	return retval;
}

DataStruct* CGEReviewInsertsDlg::getSelectedData()
{
	return getDataByRow(getFlexGrid().get_Row());
}

DataStruct* CGEReviewInsertsDlg::getDataByRow(long row)
{
	if (row > 0 && row < getFlexGrid().get_Rows())
	{
		long num = COleVariant(getFlexGrid().get_RowData(row)).lVal;
		POSITION pos = (POSITION)num;
		
		return m_fileStruct.getBlock()->getDataList().GetAt(pos);
	}

	return NULL;
}

DataStruct* CGEReviewInsertsDlg::getSelectedDataFromCAMCAD()
{
   if (m_camCadDoc.somethingIsSelected())
   {
		SelectStruct* selected = m_camCadDoc.getSelectStack().getAtLevel();

		if (selected != NULL)
			return selected->getData();
	}

	return NULL;
}

void CGEReviewInsertsDlg::setReviewedDataColor(DataStruct* data, long row, bool reviewed)
{
	COleVariant row1((long)row);
	COleVariant row2((long)row);
	COleVariant col1((long)0);
	COleVariant col2((long)m_colCount-1);

	if (reviewed)
	{
      if (data->getInsert()->getGraphicMirrored())
      {
		   data->setOverrideColor(m_layerColor.getColor(gerberEducatorReviewComponentApprovedBottomColor));
		   getFlexGrid().put_Cell(CFlexGrid::flexcpBackColor, row1, col1, row2, col2, 
            (COleVariant)(long)m_layerColor.getColor(gerberEducatorReviewComponentApprovedBottomColor));
      }
      else
      {
		   data->setOverrideColor(m_layerColor.getColor(gerberEducatorReviewComponentApprovedTopColor));
		   getFlexGrid().put_Cell(CFlexGrid::flexcpBackColor, row1, col1, row2, col2,
            (COleVariant)(long)m_layerColor.getColor(gerberEducatorReviewComponentApprovedTopColor));
      }
		data->setColorOverride(true);

	}
	else
	{
		data->setColorOverride(false);

		getFlexGrid().put_Cell(CFlexGrid::flexcpBackColor, row1, col1, row2, col2, (COleVariant)(long)RGB(255, 255, 214));
	}
}

void CGEReviewInsertsDlg::drawData(DataStruct* data, int state)
{
	SelectStruct *selectStruct = m_camCadDoc.InsertInSelectList(data, m_fileStruct.getBlock()->getFileNumber(),
			m_fileStruct.getInsertX(), m_fileStruct.getInsertY(),  m_fileStruct.getScale(), 
         m_fileStruct.getRotation(), m_viewOption==m_viewBottom?1:0, &(m_fileStruct.getBlock()->getDataList()), FALSE, TRUE);
	m_camCadDoc.DrawEntity(selectStruct, state, FALSE);
}

void CGEReviewInsertsDlg::setComponentSelectionInGrid(DataStruct* componentData, long col)
{
	getFlexGrid().clearSelection();

	if (componentData == NULL)
		return;

	for (long row=1; row<getFlexGrid().get_Rows(); row++)
	{
		long num = COleVariant(getFlexGrid().get_RowData(row)).lVal;
		POSITION pos = (POSITION)num;
		
		DataStruct* data = m_fileStruct.getBlock()->getDataList().GetAt(pos);
		if (data == componentData)
		{
         getFlexGrid().select(row, col, row, col);
			getFlexGrid().ShowCell(row, col);
			break;
		}
	}
}

void CGEReviewInsertsDlg::updateSelectedEntity()
{
	if (m_cellEditError != cellEditErrorNone)
	{
		m_cellEditError = cellEditErrorNone;
		OnClickInsertGrid();
		return;
	}

	DataStruct* selectedData = getSelectedDataFromCAMCAD();
	setComponentSelectionInGrid(selectedData, 0);
}

void CGEReviewInsertsDlg::OnClickInsertGrid()
{
	DataStruct* selectedData = getSelectedData();
	if (selectedData == NULL)
		return;

   BlockStruct* block = m_camCadDoc.getBlockAt(selectedData->getInsert()->getBlockNumber());
   if (block == NULL)
      return;

   if (!block->extentIsValid())
      m_camCadDoc.CalcBlockExtents(block);

   CTMatrix matrix = selectedData->getInsert()->getTMatrix();
   if (m_viewOption == m_viewBottom)
      matrix.scale(-1, 1);

   CExtent extent = block->getExtent();
   extent.transform(matrix);

	CDblRect rect;
   rect.xMax = extent.getXmax();
   rect.yMax = extent.getYmax();
   rect.xMin = extent.getXmin();
   rect.yMin = extent.getYmin();

   // Mark et al wants the left end of the slider to be less zoom and right end to be
   // more zoom. That is the opposite of what the margin itself represents, i.e. more margin is
   // less zoom, less margin is more zoom. Can't seem to just set the slider to have a range
   // 20 to 0 (left to right) so reverse the value of the setting after the fact, right here.
   // The formula used here also smooths out the zoom rate, without it values between 0 and 2 jump
   // to far.
   double zoomMargin = m_maxZoomMargin - (double)m_zoomSlider.GetPos();
   zoomMargin = (zoomMargin * zoomMargin) / m_maxZoomMargin; // smooths out the slider effect
	double marginSizeX = (m_camCadDoc.getSettings().getXmax() - m_camCadDoc.getSettings().getXmin()) * zoomMargin / 100.0;
	double marginSizeY = (m_camCadDoc.getSettings().getYmax() - m_camCadDoc.getSettings().getYmin()) * zoomMargin / 100.0;
	double marginSize = max(marginSizeX, marginSizeY);

	rect.xMin -= marginSize;
	rect.xMax += marginSize;		
	rect.yMin -= marginSize;
	rect.yMax += marginSize;     

	getActiveView()->ZoomBox(rect);
	drawData(selectedData, 3);
}

void CGEReviewInsertsDlg::KeyDownEditInsertGrid(long Row, long Col, short * KeyCode, short Shift)
{
	int tmp = *KeyCode;
	if (tmp == 13 && getFlexGrid().get_Col() == m_colRefdes)
	{
		if (Row > 0 && Row < getFlexGrid().get_Rows())
		{
			if (getFlexGrid().get_Row() == getFlexGrid().get_Rows() -1)
				getFlexGrid().put_Row(1);
			else
				getFlexGrid().put_Row(Row + 1);
			
			if (!getFlexGrid().get_RowIsVisible(Row))
				getFlexGrid().ShowCell(Row, Col);
		}
	}
}

void CGEReviewInsertsDlg::AfterRowColChangeOnInsertGrid(long OldRow, long OldCol, long NewRow, long NewCol)
{
	if (NewCol == m_colRefdes)
	{
		if (m_cellEditError != cellEditErrorNone)
		{
			m_cellEditError = cellEditErrorNone;
			if (NewCol > 0 && NewCol < getFlexGrid().get_Rows())
			{
				getFlexGrid().put_Row(NewCol);
				getFlexGrid().put_Col(NewCol);
				
				if (!getFlexGrid().get_RowIsVisible(NewCol))
					getFlexGrid().ShowCell(NewCol, NewCol);
			}
		}
	}

	EnterCellInsertGrid();
	if (OldRow != NewRow)
		OnClickInsertGrid();
}

void CGEReviewInsertsDlg::AfterEditInsertGrid(long Row, long Col)
{
	if (Col == m_colReview)
	{
		DataStruct* data = getSelectedData();
		setReviewedDataColor(data, Row, getFlexGrid().getCellChecked(Row, Col));

		if (data != NULL && data->getInsert() != NULL)
		{
			bool review = getFlexGrid().getCellChecked((int)Row, (int)Col);
			if (review)
			{
				if (!data->getInsert()->getGraphicMirrored())
				{
					m_topReviewQuantitySummary.addReviewQuantityTo(data->getInsert()->getInsertType(), 1, 0);
				}
				else
				{
					m_bottomReviewQuantitySummary.addReviewQuantityTo(data->getInsert()->getInsertType(), 1, 0);
				}
			}
			else 
			{
				if (!data->getInsert()->getGraphicMirrored())
				{
					m_topReviewQuantitySummary.removeReviewQuantityFrom(data->getInsert()->getInsertType(), 1, 0);
				}
				else
				{
					m_bottomReviewQuantitySummary.removeReviewQuantityFrom(data->getInsert()->getInsertType(), 1, 0);
				}
			}
		}
	}
}

void CGEReviewInsertsDlg::EnterCellInsertGrid()
{
	bool reviewed = getFlexGrid().getCellChecked(getFlexGrid().get_Row(), m_colReview);

	if (reviewed && getFlexGrid().get_MouseCol() != m_colReview && getFlexGrid().get_Col() != m_colReview)
	{
		getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
	}
	else
	{
		getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);

		if (getFlexGrid().get_MouseCol() == m_colRefdes)
		{
			getFlexGrid().put_ComboList("");
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText(getFlexGrid().get_Row(), m_colRefdes).GetLength());
		}
	}
}

void CGEReviewInsertsDlg::BeforeEditInsertGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
	if (Col == m_colRotation)
	{
		CString allowDegrees = getFlexGrid().getCellText((int)Row, m_colDegrees);
		if (allowDegrees.Find("|") > -1)
		{
			getFlexGrid().put_ComboList(allowDegrees);
			getFlexGrid().put_ComboIndex(0);
		}
		else
		{
			getFlexGrid().put_ComboList("");
			getFlexGrid().setCell(Row, Col, allowDegrees);
			getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
		}
	}
	else if (Col == m_colSurface)
	{
		DataStruct* data = getSelectedData();
		if (data != NULL && data->getInsert() != NULL && data->getAttributes() != NULL)
		{
			CString technology;
			if (m_camCadDatabase.getAttributeStringValue(technology, *data->getAttributes(), m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeTechnology)))
			{
				if (technology.CompareNoCase("SMD") == 0)
				{
					// It is an SMD component so don't allow surface option
					CString surface = getFlexGrid().getCellText((int)Row, m_colSurface);
					getFlexGrid().put_ComboList("");
					getFlexGrid().setCell(Row, Col, surface);
					getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
					return;
				}
			}
		}

		getFlexGrid().put_ComboList("Top|Bottom");
		getFlexGrid().put_ComboIndex(0);
	}
	else
	{
		getFlexGrid().put_ComboList("");
	}
}

void CGEReviewInsertsDlg::ChangeEditInsertGrid()
{
	int col = (int)getFlexGrid().get_Col();
	DataStruct* data = getSelectedData();
	if (data != NULL && data->getInsert() != NULL)
	{
		InsertStruct* insert = data->getInsert();
		CString editText = getFlexGrid().get_EditText();

		if (col == m_colType)
		{
			bool review = isReviewed(data);
			if (!insert->getGraphicMirrored())
			{
				m_topReviewQuantitySummary.removeReviewQuantityFrom(insert->getInsertType(), review?1:0, 1);
				m_topReviewQuantitySummary.addReviewQuantityTo(insertDisplayStringToTypeTag(editText), review?1:0, 1);
			}
			else
			{
				m_bottomReviewQuantitySummary.removeReviewQuantityFrom(insert->getInsertType(), review?1:0, 1);
				m_bottomReviewQuantitySummary.addReviewQuantityTo(insertDisplayStringToTypeTag(editText), review?1:0, 1);
			}

			insert->setInsertType(insertDisplayStringToTypeTag(editText));

			if (insert->getInsertType() == insertTypePcbComponent)
			{
		      if(insert->getRefname().IsEmpty())
		      {
			      // refdes is empty for PCB Component
			      CString refDes = m_gerberEducatorComponents.getRefDesMap().defineCeiling("GE_",QRefDesSuffixFormat);
               insert->setRefname(refDes);

               getFlexGrid().setCell((int)getFlexGrid().get_Row(), m_colSortableRefDes, insert->getSortableRefDes());
               getFlexGrid().setCell((int)getFlexGrid().get_Row(), m_colRefdes, insert->getRefname());
            }

				addComppin(data);
			}
			else
			{
				removeCompPin(data);
			}

			m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
			return;
		}
		

		// Erase the draw of the data
		drawData(data, -1);

		if (col == m_colRotation)
		{
			if (!editText.IsEmpty())
			{
				insert->setAngle(DegToRad(atof(editText)));
				updateComppinLocation(data);
			}
		}
		else if (col == m_colSurface)
		{
			bool review = isReviewed(data);
			if (editText.Compare("Top") == 0)
			{
				m_bottomReviewQuantitySummary.removeReviewQuantityFrom(insert->getInsertType(), review?1:0, 1);
				m_topReviewQuantitySummary.addReviewQuantityTo(insert->getInsertType(), review?1:0, 1);

				insert->setMirrorFlags(0);
				insert->setPlacedBottom(false);
			}
			else
			{
				m_topReviewQuantitySummary.removeReviewQuantityFrom(insert->getInsertType(), review?1:0, 1);
				m_bottomReviewQuantitySummary.addReviewQuantityTo(insert->getInsertType(), review?1:0, 1);

				insert->setMirrorFlags(MIRROR_ALL);
				insert->setPlacedBottom(true);
			}

			if (m_viewOption == m_viewTop		&& insert->getGraphicMirrored() || 
				m_viewOption == m_viewBottom	&& !insert->getGraphicMirrored())
			{
				int row = getFlexGrid().get_Row();

				getFlexGrid().clearSelection();
				getFlexGrid().removeItem(row);
			}

			getFlexGrid().autoSizeColumns();
			updateComppinLocation(data);
		}

		// Redraw the date after it is modified
		drawData(data, 3);
	}
}

void CGEReviewInsertsDlg::ValidateEditInsertGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
	m_cellEditError = cellEditErrorNone;

   // Don't validate if dialog is not yet initialized or is doing sort
	if (!m_initialized)
		return;

	if (Row > 0 && Col == m_colRefdes)
	{
		CString curRefdes = getFlexGrid().getCellText((int)Row, (int)Col);
		CString newRefdes = getFlexGrid().get_EditText();
		newRefdes.Trim();

		InsertTypeTag insertType = insertDisplayStringToTypeTag(getFlexGrid().getCellText((int)Row, m_colType));

		if (newRefdes.IsEmpty() && insertType == insertTypePcbComponent)
		{
			ErrorMessage("Refdes cannot be blank.  Please enter another refdes.", "Refdes");
			getFlexGrid().put_EditText(curRefdes);
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText(Row, m_colRefdes).GetLength());

			m_cellEditError = cellEditErrorEmpty;
			*Cancel = VARIANT_TRUE;
		}
		else if (!newRefdes.IsEmpty() && checkDuplicateRefdes(Row, newRefdes))
		{
         CString message;
         message.Format("The entered refdes [%s] is already used.  Please enter another refdes.", newRefdes);
			ErrorMessage(message, "Refdes");
			getFlexGrid().put_EditText(curRefdes);
			getFlexGrid().EditCell();
			getFlexGrid().put_EditSelStart(0);
			getFlexGrid().put_EditSelLength(getFlexGrid().getCellText(Row, m_colRefdes).GetLength());

			m_cellEditError = cellEditErrorDuplicate;
			*Cancel = VARIANT_TRUE;
		}
		else
		{
			DataStruct* selectedData = getSelectedData();
			if (selectedData == NULL || selectedData->getInsert() == NULL)
			{
				ErrorMessage("Cannot find selected component", "Error");
				*Cancel = VARIANT_TRUE;
				return;
			}

			// Erase the draw of the data
			drawData(selectedData, -1);

			// Rename the insert from curRefdesName to newRefdesName
			Attrib* attrib = NULL;
			int keyword = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeRefName);

			if (selectedData->getAttributes() != NULL && selectedData->getAttributes()->Lookup((WORD)keyword, attrib))
				attrib->setStringValueIndex(m_camCadDoc.RegisterValue(newRefdes));
			else
				selectedData->setAttrib(m_camCadDoc.getCamCadData(), keyword, valueTypeString, newRefdes.GetBuffer(0), attributeUpdateOverwrite, NULL);

			selectedData->getInsert()->setRefname(newRefdes);
         
         // udpate sortable refdes in grid
         getFlexGrid().setCell((int)Row, m_colSortableRefDes, selectedData->getInsert()->getSortableRefDes());

			updateComppinRefdes(curRefdes, newRefdes);

			// Redraw the date after it is modified
			drawData(selectedData, 3);
		}
	}
}

void CGEReviewInsertsDlg::BeforeMouseDownInsertGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel)
{
	m_insertGrid.restoreWindowPlacement();
}

void CGEReviewInsertsDlg::BeforeSortInsertGrid(long col, short* order)
{
   // Get the currently selected data before clearing selction
   // so the dialog can set the selection back to the same data after sort
	m_selectedDataBeforeSort = getSelectedData();
   getFlexGrid().clearSelection();
}

void CGEReviewInsertsDlg::AfterSortInsertGrid(long col, short* order)
{
   if (col == m_colRefdes)
   {
      getFlexGrid().put_ColSort(m_colSortableRefDes, (long)*order);
      getFlexGrid().select(0, m_colSortableRefDes);
      getFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);
   }

	setComponentSelectionInGrid(m_selectedDataBeforeSort, col);
   m_selectedDataBeforeSort = NULL;
}

void CGEReviewInsertsDlg::OnBnClickedZoomIn()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMIN);
   }
}

void CGEReviewInsertsDlg::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedZoomOut()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMOUT);
   }
}

void CGEReviewInsertsDlg::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedZoomWindow()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMWINDOW);
   }
}

void CGEReviewInsertsDlg::OnUpdateZoomWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedZoomExtents()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMEXTENTS);
   }
}

void CGEReviewInsertsDlg::OnUpdateZoomExtents(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedZoomFull()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOM11);
   }
}

void CGEReviewInsertsDlg::OnUpdateZoomFull(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedPanCenter()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_PAN);
   }
}

void CGEReviewInsertsDlg::OnUpdatePanCenter(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedQueryItem()
{
   m_camCadDatabase.getCamCadDoc().OnEditEntity();
}

void CGEReviewInsertsDlg::OnUpdateQueryItem(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedRepaint()
{
   m_camCadDatabase.getCamCadDoc().OnRedraw();
}

void CGEReviewInsertsDlg::OnUpdateRepaint(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEReviewInsertsDlg::OnBnClickedViewOpton()
{
	UpdateData(TRUE);

	updateReviewStatusToInsertData();
	setView();
	loadGrid();
}

void CGEReviewInsertsDlg::OnBnClickedClose()
{
	OnClose();
}

void CGEReviewInsertsDlg::OnClose()
{
	CResizingDialog::OnClose();

	updateReviewStatusToInsertData();
	saveDialogSettings();

	m_layerColor.setDefaultViewColor(m_camCadDatabase, m_fileStruct);

	m_camCadDoc.showPinnrsTop = m_pinLabelTopWasOn?TRUE:FALSE;
	m_camCadDoc.showPinnrsBottom = m_pinLabelBottomWasOn?TRUE:FALSE;
	m_camCadDoc.getSelectLayerFilterStack().pop();
   //m_camCadDoc.OnClearSelected();
	m_gerberEducatorUi.generateEvent(ID_GerberEducatorCommand_TerminateSession);

   EndDialog(IDCANCEL);
}

void CGEReviewInsertsDlg::loadDialogSettings()
{
	// Load settings

   CString dialogName(GetDialogProfileEntry());
	CString view;

	view = AfxGetApp()->GetProfileString(dialogName, "View", view);
	m_curZoomMargin = AfxGetApp()->GetProfileInt(dialogName, "ZoomMargin", m_curZoomMargin);
	m_turnOnPinLabel = AfxGetApp()->GetProfileInt(dialogName, "PinLabel", m_turnOnPinLabel);

	m_viewOption = m_viewTop;
	if (view.CompareNoCase(CONST_VIEWBOTTOM) == 0)
	{
		m_viewOption = m_viewBottom;
	}
	else if (view.CompareNoCase(CONST_VIEWBOTH) == 0)
	{
		m_viewOption =  m_viewBoth;
	}

	UpdateData(FALSE);
}

void CGEReviewInsertsDlg::saveDialogSettings()
{
	// Save settings
	UpdateData(TRUE);

   CString dialogName(GetDialogProfileEntry());
	CString view = CONST_VIEWTOP;
	if (m_viewOption == m_viewBottom)
	{
		view = CONST_VIEWBOTTOM;
	}
	else if (m_viewOption == m_viewBoth)
	{
		view = CONST_VIEWBOTH;
	}

	AfxGetApp()->WriteProfileString(dialogName, "View", view);   
	AfxGetApp()->WriteProfileInt(dialogName, "ZoomMargin", m_curZoomMargin);   
	AfxGetApp()->WriteProfileInt(dialogName, "PinLabel", m_turnOnPinLabel);   
}
	
void CGEReviewInsertsDlg::updateComppinRefdes(CString oldRefdes, CString newRefdes)
{
	DataStruct* data = getSelectedData();
	if (data == NULL)
		return;

	InsertStruct* insert = data->getInsert();
	if (insert == NULL)
		return;

	BlockStruct* block = m_camCadDatabase.getBlock(insert->getBlockNumber());
	if (block == NULL)
		return;

	for (CDataListIterator dataList(*block, insertTypePin); dataList.hasNext();)
	{
		DataStruct* pinData = dataList.getNext();
		if (pinData == NULL)
			continue;

		InsertStruct* pinInsert = pinData->getInsert();
		if (pinInsert == NULL)
			continue;

		CompPinStruct* comppin = m_camCadDatabase.getPin(&m_fileStruct, oldRefdes, pinInsert->getRefname());
		if (comppin != NULL)
			comppin->setRefDes(newRefdes);
	}
}

void CGEReviewInsertsDlg::updateComppinLocation(DataStruct* data)
{
	if (data == NULL)
		return;

	InsertStruct* insert = data->getInsert();
	if (insert == NULL)
		return;

	BlockStruct* block = m_camCadDatabase.getBlock(insert->getBlockNumber());
	if (block == NULL)
		return;

	double tolerance = getUnitsFactor(pageUnitsMils, m_camCadDoc.getSettings().getPageUnits()) *.01;
	CTMatrix insertMatrix = insert->getBasesVector().getTransformationMatrix();

	for (CDataListIterator dataList(*block, insertTypePin); dataList.hasNext();)
	{
		DataStruct* pinData = dataList.getNext();
		if (pinData == NULL)
			continue;

		InsertStruct* pinInsert = pinData->getInsert();
		if (pinInsert == NULL)
			continue;

		CBasesVector pinBasesVector = pinInsert->getBasesVector();
		CTMatrix pinMatrix = pinInsert->getBasesVector().getTransformationMatrix();

		CPoint2d newPoint(0.0, 0.0);
		CTMatrix comppinMatrix = pinMatrix * insertMatrix;
		comppinMatrix.transform(newPoint);

		CNetList& netList = m_fileStruct.getNetList();
		for (POSITION netPos = netList.GetHeadPosition(); netPos != NULL;)
		{
			NetStruct* net = netList.GetNext(netPos);
			if (net == NULL)
				continue;

			for (POSITION comppinPos = net->getHeadCompPinPosition(); comppinPos != NULL;)
			{
				CompPinStruct* comppin = net->getNextCompPin(comppinPos);
				if (comppin == NULL || comppin->getRefDes().CompareNoCase(insert->getRefname()) != 0)
					continue;

				if (!newPoint.fpeq(comppin->getOrigin(), tolerance))
					continue;

				int mirrorFlag = insert->getMirrorFlags() ^ pinInsert->getMirrorFlags();

				comppin->setPinName(pinInsert->getRefname());
				comppin->setMirror(mirrorFlag);

				if (comppin->getVisible() == visibleTop && mirrorFlag != 0)
					comppin->setVisible(visibleBottom);
				else if (comppin->getVisible() == visibleBottom && mirrorFlag == 0)
					comppin->setVisible(visibleTop);
			}
		}
	}
}

void CGEReviewInsertsDlg::addComppin(DataStruct* data)
{
	if (data == NULL)
		return;

	InsertStruct* insert = data->getInsert();
	if (insert == NULL)
		return;

	BlockStruct* block = m_camCadDatabase.getBlock(insert->getBlockNumber());
	if (block == NULL)
		return;

	NetStruct* net = m_camCadDatabase.getNet(NET_UNUSED_PINS, &m_fileStruct);
	if (net == NULL)
		return;

	for (CDataListIterator dataList(*block, insertTypePin); dataList.hasNext();)
	{
		DataStruct* pinData = dataList.getNext();
		if (pinData == NULL)
			continue;

		InsertStruct* pinInsert = pinData->getInsert();
		if (pinInsert == NULL)
			continue;

		CompPinStruct* comppin = m_camCadDatabase.addCompPin(net, insert->getRefname(), pinInsert->getRefname());
	}

	m_camCadDatabase.getCamCadDoc().generatePinLocations(false);
}

void CGEReviewInsertsDlg::removeCompPin(DataStruct* data)
{
	if (data == NULL)
		return;

	InsertStruct* insert = data->getInsert();
	if (insert == NULL)
		return;

	m_camCadDatabase.deletePins(&m_fileStruct, insert->getRefname());
}

void CGEReviewInsertsDlg::updateReviewStatusToInsertData()
{
	// Update the review status of data and also remove color override
	int keywordIndex = m_camCadDoc.getStandardAttributeKeywordIndex(standardAttributeGeomReviewStatus);
	for (long row=1; row<getFlexGrid().get_Rows(); row++)
	{
		DataStruct* data = getDataByRow(row);
		if (data == NULL)
			continue;

		CString reviewedStatus;
		reviewedStatus.Format("Reviewed=%s", getFlexGrid().getCellChecked(row, m_colReview)?"TRUE":"FALSE");

		data->setColorOverride(false);
		data->setAttrib(m_camCadDoc.getCamCadData(), keywordIndex, valueTypeString, reviewedStatus.GetBuffer(0), attributeUpdateOverwrite, NULL);
	}

	m_camCadDoc.UpdateAllViews(NULL);
}

void CGEReviewInsertsDlg::OnNMReleasedcaptureZoomslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	if (m_curZoomMargin != m_zoomSlider.GetPos())
	{
      /// The tooltip has been disabled. The value it would show is not particularly meaningful
      /// for users, and perhaps is just a source of possible confusion. All the user really needs
      /// is is to know moving slider to left is less zoom and to right is more zoom.
      /// If you are reactivating this code, the go to RC file and set ToolTips True for this slider.
		///CString zoomMargin;
		///zoomMargin.Format("Component Extext Margin of %d%%", m_curZoomMargin);
		///m_zoomSlider.GetToolTips()->SetWindowText(zoomMargin);
		m_curZoomMargin = m_zoomSlider.GetPos();
		OnClickInsertGrid();
	}

	*pResult = 0;
}

void CGEReviewInsertsDlg::OnBnClickedReviewcurrentgeometry()
{
	DataStruct* data = getSelectedData();
	if (data == NULL)
		return;

	InsertStruct* insert = data->getInsert();
	if (insert == NULL)
		return;

	updateReviewStatusToInsertData();
	saveDialogSettings();
	m_camCadDoc.getSelectLayerFilterStack().pop();

	m_gerberEducatorUi.setCurrentEditGeometryName(m_camCadDatabase.getCamCadDoc().getBlockAt(insert->getBlockNumber())->getName());
	m_gerberEducatorUi.setCurrentEditComponentRefdes(insert->getRefname());
	m_gerberEducatorUi.generateEvent(ID_GerberEducatorCommand_EditGeometries);
}

void CGEReviewInsertsDlg::OnBnClickedTurnonpinlabel()
{
	// TODO: Add your control notification handler code here
	m_turnOnPinLabel = !m_turnOnPinLabel;
	turnOnPinLabel();
}

void CGEReviewInsertsDlg::turnOnPinLabel()
{
	m_camCadDoc.showPinnrsTop = FALSE;
	m_camCadDoc.showPinnrsBottom = FALSE;
	if (m_turnOnPinLabel)
	{
		if (m_viewOption == m_viewBoth || m_viewOption == m_viewTop)
				m_camCadDoc.showPinnrsTop = TRUE;
		if (m_viewOption == m_viewBoth || m_viewOption == m_viewBottom)
				m_camCadDoc.showPinnrsBottom = TRUE;
	}

	m_camCadDoc.UpdateAllViews(NULL);
}

void CGEReviewInsertsDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//lpwndpos->flags &= ~SWP_HIDEWINDOW;	
	//CResizingDialog::OnWindowPosChanging(lpwndpos);
}
