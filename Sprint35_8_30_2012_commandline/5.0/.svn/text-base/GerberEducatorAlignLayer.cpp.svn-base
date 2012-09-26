// $Header: /CAMCAD/5.0/GerberEducatorAlignLayer.cpp 16    3/12/07 12:40p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GerberEducatorAlignLayer.cpp

#include "stdafx.h"
#include "GerberEducatorAlignLayer.h"
#include "GerberEducatorToolbar.h"
#include "draw.h"
#include ".\gerbereducatoralignlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//-----------------------------------------------------------------------------
// CGEAlignLayerDlg dialog
//-----------------------------------------------------------------------------
//CGEAlignLayerDlg::CGEAlignLayerDlg(CGerberEducatorUi& gerberEducatorUi)
//	:  CResizingDialog(CGEAlignLayerDlg::IDD) // CMeasureBaseDialog(CGEAlignLayerDlg::IDD, measure)
//   , m_camCadDatabase(gerberEducatorUi.getCamCadDoc())
//   , m_fileStruct(*gerberEducatorUi.getFileStruct())
CGEAlignLayerDlg::CGEAlignLayerDlg(CMeasure& measure)
	: CMeasureBaseDialog(CGEAlignLayerDlg::IDD, measure)
   , m_toolBar(*this)
   , m_snapToInsertOrigin(TRUE)
   , m_snapToLineEnd(FALSE)
{
   m_temporaryCentroidLayer = NULL;
   m_gerberEducatorUi = NULL;
   m_fileStruct = NULL;
   m_initialized = false;

   addFieldControl(IDC_SelectedLayerStatic      , anchorLeft, growBoth);
   addFieldControl(IDC_LayerGridStatic          , anchorLeft, growBoth);
   addFieldControl(IDC_RotateMirrorStatic       , anchorBottomLeft, growHorizontal);
   addFieldControl(IDC_ShiftStatic              , anchorBottomLeft, growHorizontal);

   addFieldControl(IDC_RotateLayer              , anchorBottomLeft);
   addFieldControl(IDC_MirrorLayer              , anchorBottomLeft);

   addFieldControl(IDC_DefineOffset             , anchorBottomLeft);
   addFieldControl(IDC_ApplyShift               , anchorBottomLeft);
   addFieldControl(IDC_CreateTemporaryCentroid  , anchorBottomLeft);
   addFieldControl(IDC_InsertOriginCheck        , anchorBottomLeft);
   addFieldControl(IDC_LineEndCheck             , anchorBottomLeft);
   addFieldControl(IDC_NoneRadio                , anchorBottomLeft);
   addFieldControl(IDC_InsertOriginRadio        , anchorBottomLeft);
   addFieldControl(IDC_LineEndRadio             , anchorBottomLeft);
   addFieldControl(IDC_SnapToGroup              , anchorBottomLeft);
  
   addFieldControl(IDCANCEL                    , anchorBottomRight);
}

CGEAlignLayerDlg::~CGEAlignLayerDlg()
{
   m_temporaryCentroidLayer = NULL;
   m_gerberEducatorUi = NULL;
   m_fileStruct = NULL;
}

void CGEAlignLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX); //CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LayerGridStatic, m_layerGrid);
   DDX_Check(pDX, IDC_InsertOriginCheck, m_snapToInsertOrigin);
   DDX_Check(pDX, IDC_LineEndCheck, m_snapToLineEnd);
   DDX_Control(pDX, IDC_RotateLayer, m_rotateLayerButton);
   DDX_Control(pDX, IDC_MirrorLayer, m_mirrorLayerButton);
   DDX_Control(pDX, IDC_DefineOffset, m_defineOffsetButton);
   DDX_Control(pDX, IDC_ApplyShift, m_applyShiftButton);
   DDX_Control(pDX, IDC_CreateTemporaryCentroid, m_createTemporaryCentroidButton);
   DDX_Control(pDX, IDC_NoneRadio, m_noneRadio);
   DDX_Control(pDX, IDC_InsertOriginRadio, m_insertOriginRadio);
   DDX_Control(pDX, IDC_LineEndRadio, m_lineEndRadio);
}

BEGIN_MESSAGE_MAP(CGEAlignLayerDlg, CResizingDialog) //CResizingDialog)
   ON_WM_CLOSE()
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)

   ON_COMMAND(IDC_MarkByWindow, OnBnClickedMarkByWindow)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindow, OnUpdateMarkByWindow)
   ON_COMMAND(IDC_MarkByWindowCross, OnBnClickedMarkByWindowCross)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindowCross, OnUpdateMarkByWindowCross)
   ON_COMMAND(IDC_ClearMarked, OnBnClickedClearMarked)
   ON_UPDATE_COMMAND_UI(IDC_ClearMarked, OnUpdateClearMarked)
   ON_BN_CLICKED(IDC_Repaint, OnBnClickedRepaint)
   ON_UPDATE_COMMAND_UI(IDC_Repaint, OnUpdateRepaint)

   ON_COMMAND(IDC_ZOOMIN, OnBnClickedZoomIn)
   ON_UPDATE_COMMAND_UI(IDC_ZOOMIN, OnUpdateZoomIn)
   ON_COMMAND(IDC_ZoomOut, OnBnClickedZoomOut)
   ON_UPDATE_COMMAND_UI(IDC_ZoomOut, OnUpdateZoomOut)
   ON_COMMAND(IDC_ZOOMWINDOW, OnBnClickedZoomWindow)
   ON_UPDATE_COMMAND_UI(IDC_ZOOMWINDOW, OnUpdateZoomWindow)
   ON_COMMAND(IDC_ZoomExtents, OnBnClickedZoomExtents)
   ON_UPDATE_COMMAND_UI(IDC_ZoomExtents, OnUpdateZoomExtents)
   ON_COMMAND(IDC_ZoomFull, OnBnClickedZoomFull)
   ON_UPDATE_COMMAND_UI(IDC_ZoomFull, OnUpdateZoomFull)
   ON_COMMAND(IDC_PanCenter, OnBnClickedPanCenter)
   ON_UPDATE_COMMAND_UI(IDC_PanCenter, OnUpdatePanCenter)
   ON_COMMAND(IDC_QueryItem, OnBnClickedQuery)
   ON_UPDATE_COMMAND_UI(IDC_QueryItem, OnUpdateQuery)
	ON_BN_CLICKED(IDC_RotateLayer, OnBnClickedRotateLayer)
	ON_BN_CLICKED(IDC_MirrorLayer, OnBnClickedMirrorLayer)
	ON_BN_CLICKED(IDC_DefineOffset, OnBnClickedDefineOffset)
	ON_BN_CLICKED(IDC_ApplyShift, OnBnClickedApplyShift)
	ON_BN_CLICKED(IDC_CreateTemporaryCentroid, OnBnClickedCreateTemporaryCentroid)
   ON_BN_CLICKED(IDC_NoneRadio, OnBnClickedNoneRadio)
   ON_BN_CLICKED(IDC_InsertOriginRadio, OnBnClickedInsertOriginRadio)
   ON_BN_CLICKED(IDC_LineEndRadio, OnBnClickedLineEndRadio)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGEAlignLayerDlg, CResizingDialog) //CResizingDialog)  
	ON_EVENT(CGEAlignLayerDlg, ID_LayerGrid, 0x18, AfterEditLayerGrid, VTS_I4 VTS_I4)
   ON_EVENT(CGEAlignLayerDlg, ID_LayerGrid, 0x5, BeforeMouseDownLayerGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4 VTS_BOOL)
END_EVENTSINK_MAP()

BOOL CGEAlignLayerDlg::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg->wParam == VK_ESCAPE)   
   {
      if (m_measure.isMeasureOn())
      {
         // This will exist measure mode and repaint to remove ghost
         existDefineOffsetMode();
         //OnBnClickedRepaint();
      }

      //return TRUE;
   }

   return CMeasureBaseDialog::PreTranslateMessage(pMsg);
}

BOOL CGEAlignLayerDlg::OnInitDialog()
{
   if (m_initialized)
      return TRUE;

	// Call OnInitDialog(), then createAndLoad(), then restoreWindowState()
	CResizingDialog::OnInitDialog();
   m_toolBar.createAndLoad(IDR_GerberEducatorCreateGeometryToolbar);
	CResizingDialog::restoreWindowState();
   
   // Initially disable these buttons
   m_rotateLayerButton.EnableWindow(FALSE);
   m_mirrorLayerButton.EnableWindow(FALSE);
   m_applyShiftButton.EnableWindow(FALSE);
   m_createTemporaryCentroidButton.EnableWindow(FALSE);
   m_noneRadio.SetCheck(BST_UNCHECKED);
   m_insertOriginRadio.SetCheck(BST_CHECKED);
   m_lineEndRadio.SetCheck(BST_UNCHECKED);

   initGrid();
   addLayerToGrid();

   m_initialized = true;
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CGEAlignLayerDlg::initGrid()
{
   // Create grid
   m_layerGrid.substituteForDlgItem(ID_LayerGrid,IDC_LayerGridStatic,"FlexGrid",*this);

   // Initialize geometries grid
   getFlexGrid().clear();
   getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
   getFlexGrid().put_AllowSelection(TRUE);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);
   getFlexGrid().put_Cols(m_colCount);
   getFlexGrid().put_ExplorerBar(CFlexGrid::flexExSort);
   getFlexGrid().put_ExtendLastCol(true);
   getFlexGrid().put_FixedCols(2);
   getFlexGrid().put_FixedRows(1);
   getFlexGrid().put_FocusRect(CFlexGrid::flexFocusInset);
   getFlexGrid().put_HighLight(CFlexGrid::flexHighlightAlways);
   getFlexGrid().put_Rows(1);
   getFlexGrid().put_ScrollBars(CFlexGrid::flexScrollBarBoth);
   getFlexGrid().put_SelectionMode(CFlexGrid::flexSelectionByRow);

   getFlexGrid().put_ColDataType(m_colSelect, CFlexGrid::flexDTBoolean);
   getFlexGrid().setCell(0, m_colColor, CONST_COLOR);
   getFlexGrid().setCell(0, m_colSelect, CONST_SELECT);
   getFlexGrid().setCell(0, m_colLayerName, CONST_LAYERNAME);
   getFlexGrid().setCell(0, m_colDataSource, CONST_DATASOURCE);
   getFlexGrid().setCell(0, m_colDataType, CONST_DATATYPE);
   getFlexGrid().autoSizeColumns();

   // Hide the column because it is only user to store the list of degrees that should be allowed for each insert
   // The data is filled in loadGrid()
   getFlexGrid().put_ColHidden((long)m_colLayerIndex, TRUE);
}

CGerberEducatorUi& CGEAlignLayerDlg::getGerberEducatorUi() 
{ 
   if (m_gerberEducatorUi == NULL)
   {
      m_gerberEducatorUi = &CGerberEducatorUi::getGerberEducatorUi(m_measure.getCamCadDoc());   
   }

   return *m_gerberEducatorUi; 
}

void CGEAlignLayerDlg::setGerberEducatorUi(CGerberEducatorUi* gerberEducatorUi) 
{ 
   m_gerberEducatorUi = gerberEducatorUi; 
   if (m_gerberEducatorUi !=  NULL)
   {
      m_fileStruct = m_gerberEducatorUi->getFileStruct();
   }
   else
   {
      m_fileStruct = NULL;
   }
}

FileStruct& CGEAlignLayerDlg::getFileStruct()
{
   if (m_fileStruct == NULL)
   {
      getGerberEducatorUi().getFileStruct();
   }

   return *m_fileStruct;
}

void CGEAlignLayerDlg::addLayerToGrid()
{
   int dataTypeKw   = m_measure.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);
   int dataSourceKw = m_measure.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);

   CString dataSource, dataType;

   for (int i=0; i<getGerberEducatorUi().getAlignLayerCount(); i++)
   {
      LayerStruct* layer = getGerberEducatorUi().getAlignLayer(i);
      if (layer == NULL)
         continue;

      m_measure.getCamCadDatabase().getAttributeStringValue(dataSource,layer->attributes(),dataSourceKw);
      m_measure.getCamCadDatabase().getAttributeStringValue(dataType  ,layer->attributes(),dataTypeKw);

      if (dataSource.CompareNoCase(QDataSourceGerberEducatorInternal) != 0)
      {
         GerberEducatorDataSourceTag dataSourceTag         = stringToGerberEducatorDataSource(dataSource);
         GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(dataType);

         CString item;
         item.AppendFormat("%d\t\t0\t%s\t%s\t%s",  layer->getLayerIndex(), layer->getName(), dataSource, dataType);
         getFlexGrid().addItem(item, (int)getFlexGrid().get_Rows());
         getFlexGrid().setCellBackroundColor((int)getFlexGrid().get_Rows()-1, m_colColor, layer->getColor());      
      }
   }

   if (getFlexGrid().get_Rows() > 1)
   {
      getFlexGrid().select(1, m_colSelect, 1, m_colSelect);
   }
}

void CGEAlignLayerDlg::AfterEditLayerGrid(long Row, long Col)
{
   if (Col == m_colSelect)
   {
      bool selected = false;
      for (long row=1; row<getFlexGrid().get_Rows(); row++)
      {
	      selected = getFlexGrid().getCellChecked(row, m_colSelect);
         if (selected)
            break;
      }

      m_rotateLayerButton.EnableWindow(selected?TRUE:FALSE);
      m_mirrorLayerButton.EnableWindow(selected?TRUE:FALSE);      
   }
}

void CGEAlignLayerDlg::BeforeMouseDownLayerGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel)
{
   m_layerGrid.restoreWindowPlacement();
}

void CGEAlignLayerDlg::updateMeasure()
{
   if (m_measure.isMeasureValid())
   {
      m_applyShiftButton.EnableWindow(TRUE);
   }
}

void CGEAlignLayerDlg::updateSelectedEntity()
{
   if ( m_measure.getCamCadDoc().SelectList.GetCount() > 1)
   {
      m_createTemporaryCentroidButton.EnableWindow(TRUE);
   }
   else
   {
      m_createTemporaryCentroidButton.EnableWindow(FALSE);
   }
}

void CGEAlignLayerDlg::drawOffsetArrow()
{
   //if (m_offsetFromPoint == NULL || m_offsetToPoint == NULL)
   //   return;

   //double offsetX = m_offsetFromPoint->x - m_offsetToPoint->x;
   //double offsetY = m_offsetFromPoint->y - m_offsetToPoint->y;

   //double arrowHeadSize = 0.5 * sqrt(offsetX*offsetX + offsetY*offsetY);

   //if (arrowHeadSize > m_measure.getCamCadDoc().Settings.DRC_MAX_ARROWSIZE)
   //   arrowHeadSize = m_measure.getCamCadDoc().Settings.DRC_MAX_ARROWSIZE;
   //if (arrowHeadSize < m_measure.getCamCadDoc().Settings.DRC_MIN_ARROWSIZE)
   //   arrowHeadSize = m_measure.getCamCadDoc().Settings.DRC_MIN_ARROWSIZE;

   //if (arrowHeadSize < SMALLNUMBER)
   //   return;

   //CClientDC dc(getActiveView());
   //getActiveView()->OnPrepareDC(&dc);

   //COLORREF color = RGB(255, 255, 255);
   //HPEN pen = CreatePen(PS_SOLID, 0, color);
   //HPEN origPen = (HPEN)SelectObject(dc.m_hDC, pen);
   //DrawArrow(&dc, m_offsetFromPoint->x, m_offsetFromPoint->y, m_offsetToPoint->x, m_offsetToPoint->y, getActiveView()->scaleFactor, arrowHeadSize, color);

   //DeleteObject(SelectObject(dc.m_hDC, origPen));
}

void CGEAlignLayerDlg::OnBnClickedMarkByWindow()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_QUERY_MARKRECT);
}

void CGEAlignLayerDlg::OnUpdateMarkByWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedMarkByWindowCross()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_QUERY_MARK_CROSSCHECK);
}

void CGEAlignLayerDlg::OnUpdateMarkByWindowCross(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedClearMarked()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_QUERY_CLEAR_SELECTED);
}

void CGEAlignLayerDlg::OnUpdateClearMarked(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedRepaint()
{
   existDefineOffsetMode();
   m_measure.getCamCadDoc().OnRedraw();
}

void CGEAlignLayerDlg::OnUpdateRepaint(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedZoomIn()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_ZOOM_ZOOMIN);
}

void CGEAlignLayerDlg::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedZoomOut()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_ZOOM_ZOOMOUT);
}

void CGEAlignLayerDlg::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedZoomWindow()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_ZOOM_ZOOMWINDOW);
}

void CGEAlignLayerDlg::OnUpdateZoomWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedZoomExtents()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_ZOOM_ZOOMEXTENTS);
}

void CGEAlignLayerDlg::OnUpdateZoomExtents(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedZoomFull()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_ZOOM_ZOOM11);
}

void CGEAlignLayerDlg::OnUpdateZoomFull(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedPanCenter()
{
   existDefineOffsetMode();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_PAN);
}

void CGEAlignLayerDlg::OnUpdatePanCenter(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::OnBnClickedQuery()
{
   existDefineOffsetMode();
   m_measure.getCamCadDoc().OnEditEntity();
}
void CGEAlignLayerDlg::OnUpdateQuery(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGEAlignLayerDlg::transformLayer(CTMatrix matrix)
{
   for (long row=1; row<getFlexGrid().get_Rows(); row++)
   {
	   if (!getFlexGrid().getCellChecked(row, m_colSelect))
			continue;

		int layerIndex = atoi(getFlexGrid().getCellText(row, m_colLayerIndex));
		for (CDataListIterator dataList(*m_fileStruct->getBlock()); dataList.hasNext();)
		{
			DataStruct* data = dataList.getNext();			
			if (data->getLayerIndex() != layerIndex)
				continue;

			data->transform(matrix, &m_measure.getCamCadData(), true);
		}
	}

	m_measure.getCamCadDoc().OnFitPageToImage();
}

void CGEAlignLayerDlg::OnBnClickedRotateLayer()
{
   existDefineOffsetMode();

	CTMatrix matrix;
	matrix.rotateDegrees(90);
   transformLayer(matrix);
}

void CGEAlignLayerDlg::OnBnClickedMirrorLayer()
{
   existDefineOffsetMode();

   CTMatrix matrix;
	matrix.scale(-1, 1);
   transformLayer(matrix);
}

void CGEAlignLayerDlg::OnBnClickedDefineOffset()
{
   enterDefineOffsetMode();
}

void CGEAlignLayerDlg::enterDefineOffsetMode()
{
   //// Disable "MarkByWindow" and "MarkByWindowCross"
   //CToolBarCtrl& toolBarControl = m_toolBar.GetToolBarCtrl();
   //toolBarControl.EnableButton(IDC_MarkByWindow, FALSE);
   //toolBarControl.EnableButton(IDC_MarkByWindowCross, FALSE);

   //// Disable the following buttons
   //m_rotateLayerButton.EnableWindow(FALSE);
   //m_mirrorLayerButton.EnableWindow(FALSE);
   //m_applyShiftButton.EnableWindow(FALSE);
   //m_createTemporaryCentroidButton.EnableWindow(FALSE);

   OnBnClickedClearMarked();

   if (m_noneRadio.GetCheck() == BST_CHECKED)
   {
      m_measure.getCamCadDoc().getSettings().FilterOn = FALSE;
      m_measure.getCamCadDoc().getSettings().m_filterOnSingleTime = true;
   }
	else if (m_insertOriginRadio.GetCheck() == BST_CHECKED)
	{
		m_measure.getCamCadDoc().getSettings().FilterInserts = TRUE;
		m_measure.getCamCadDoc().getSettings().FilterMode = Filter_Center;
	   m_measure.getCamCadDoc().getSettings().FilterOn = TRUE;
      m_measure.getCamCadDoc().getSettings().m_filterOnSingleTime = false;
	}
   else if (m_lineEndRadio.GetCheck() == BST_CHECKED)
	{
		m_measure.getCamCadDoc().getSettings().FilterLines = TRUE;
		m_measure.getCamCadDoc().getSettings().FilterMode = Filter_Endpoint;
	   m_measure.getCamCadDoc().getSettings().FilterOn = TRUE;
      m_measure.getCamCadDoc().getSettings().m_filterOnSingleTime = false;
	}

   // Turn on measure first, then set objects
   m_measure.setMeasureOn(true);
   m_measure.getFromObject().setType(measureObjectPoint);
   m_measure.getToObject().setType(measureObjectPoint);
}

void CGEAlignLayerDlg::existDefineOffsetMode()
{
   //// Enable buttons after "Apply Shift"
   //m_toolBar.enable(true);
   //AfterEditLayerGrid(1, m_colSelect);

   // disable "Apply Shift" button

   m_measure.setMeasureOn(false);
   m_measure.getCamCadDoc().getSettings().FilterOn = FALSE;
   m_measure.getCamCadDoc().getSettings().m_filterOnSingleTime = true;

   m_applyShiftButton.EnableWindow(FALSE);
}

void CGEAlignLayerDlg::OnBnClickedNoneRadio()
{
   m_noneRadio.SetCheck(BST_CHECKED);
   m_insertOriginRadio.SetCheck(BST_UNCHECKED);
   m_lineEndRadio.SetCheck(BST_UNCHECKED);

   m_measure.getCamCadDoc().getSettings().FilterOn = FALSE;
   m_measure.getCamCadDoc().getSettings().m_filterOnSingleTime = true;
}

void CGEAlignLayerDlg::OnBnClickedInsertOriginRadio()
{
   m_noneRadio.SetCheck(BST_UNCHECKED);
   m_insertOriginRadio.SetCheck(BST_CHECKED);
   m_lineEndRadio.SetCheck(BST_UNCHECKED);

	m_measure.getCamCadDoc().getSettings().FilterInserts = TRUE;
	m_measure.getCamCadDoc().getSettings().FilterMode = Filter_Center;
}

void CGEAlignLayerDlg::OnBnClickedLineEndRadio()
{
   m_noneRadio.SetCheck(BST_UNCHECKED);
   m_insertOriginRadio.SetCheck(BST_UNCHECKED);
   m_lineEndRadio.SetCheck(BST_CHECKED);

	m_measure.getCamCadDoc().getSettings().FilterLines = TRUE;
	m_measure.getCamCadDoc().getSettings().FilterMode = Filter_Endpoint;
}

void CGEAlignLayerDlg::OnBnClickedApplyShift()
{
   CTMatrix matrix;
	matrix.translate( m_measure.getDeltaX(),  m_measure.getDeltaY());
   transformLayer(matrix);
   
   existDefineOffsetMode();
}

void CGEAlignLayerDlg::OnBnClickedCreateTemporaryCentroid()
{
   existDefineOffsetMode();
   m_createTemporaryCentroidButton.EnableWindow(FALSE);

   // Add selected data to DataList to get extent and find centroid X and Y
   CDataList dataList(false);
   CSelectList& selectList = m_measure.getCamCadDoc().SelectList;
   for (POSITION pos = selectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* selectStruct = selectList.GetNext(pos);
      DataStruct* data = selectStruct->getData();
      dataList.AddHead(data);
   }

   CExtent extent = dataList.getExtent();
   dataList.RemoveAll();

   // Insert centroid
	BlockStruct* centroidBlock = m_measure.getCamCadDatabase().getDefinedCentroidGeometry(getFileStruct().getFileNumber());
   DataStruct* centroidData = m_measure.getCamCadDatabase().insertBlock(centroidBlock, insertTypeCentroid, "", 
         getDefinedTemporaryCentroidLayer()->getLayerIndex(), extent.getCenter().x, extent.getCenter().y, 0.0, false);
   getFileStruct().getBlock()->getDataList().AddTail(centroidData);

   // Redraw centroid to show it
	SelectStruct *selectStruct = m_measure.getCamCadDoc().InsertInSelectList(centroidData,  getFileStruct().getBlock()->getFileNumber(),
			getFileStruct().getInsertX(), getFileStruct().getInsertY(),  getFileStruct().getScale(), 
         getFileStruct().getRotation(), 0, &(getFileStruct().getBlock()->getDataList()), FALSE, TRUE);
   if (selectStruct != NULL)
   {
      m_measure.getCamCadDoc().DrawEntity(*selectStruct, drawStateNormal);
   }

   //m_measure.getCamCadDoc().clearSelected(true, true);
}

LayerStruct* CGEAlignLayerDlg::getDefinedTemporaryCentroidLayer()
{
   if (m_temporaryCentroidLayer == NULL)
   {
      m_temporaryCentroidLayer = m_measure.getCamCadDatabase().getDefinedLayer("AlignLayerTemporaryCentroidLayer");
      m_temporaryCentroidLayer->setColor(colorYellow);
   }

   return m_temporaryCentroidLayer;
}

void CGEAlignLayerDlg::deleteTemporaryCentroidLayer()
{
   if (m_temporaryCentroidLayer != NULL)
   {
      m_measure.getCamCadDatabase().deleteLayer(*m_temporaryCentroidLayer);
      m_temporaryCentroidLayer = NULL;
   }
}

void CGEAlignLayerDlg::OnBnClickedCancel()
{
   OnClose();
}

void CGEAlignLayerDlg::OnClose()
{
   CResizingDialog::OnClose(); //CResizingDialog::OnClose();

   deleteTemporaryCentroidLayer();
	m_measure.getCamCadDoc().getSettings().FilterOn = FALSE;

   if (m_gerberEducatorUi != NULL)
   {
	   m_gerberEducatorUi->generateEvent(ID_GerberEducatorCommand_TerminateSession);
   }
   else
   {
      m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_TerminateMeasure,0);
   }

   EndDialog(IDCANCEL);
}
