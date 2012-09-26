// $Header: /CAMCAD/5.0/GerberEducatorSetupDataType.cpp 39    2/27/07 1:58p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GerberEducatorSetupDataType.cpp 

#include "stdafx.h"
#include "ccdoc.h"
#include "MainFrm.h"
#include "GerberEducatorSetupDataType.h"
#include "GerberEducatorToolbar.h"
#include "FlashRecognizer.h"
#include "GerberEducatorAlignLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------------------
// CGESetupDataType dialog
//-----------------------------------------------------------------------------
CGESetupDataType::CGESetupDataType(CGerberEducatorUi& gerberEducatorUi)
   : CResizingDialog(CGESetupDataType::IDD)
   , m_gerberEducatorUi(gerberEducatorUi)
   , m_camCadDatabase(gerberEducatorUi.getCamCadDoc())
   , m_toolBar(*this) 
{
   m_initialized = false;

   addFieldControl(IDC_LayerGridStatic       , anchorLeft, growBoth);
   addFieldControl(IDC_AlignLayers           , anchorBottomLeft);
   addFieldControl(IDC_DrawToFlash           , anchorBottomLeft);
   addFieldControl(IDC_CLOSE                 , anchorBottomRight);
}

CGESetupDataType::~CGESetupDataType()
{
}

void CGESetupDataType::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LayerGridStatic, m_layerGrid);
}

BEGIN_MESSAGE_MAP(CGESetupDataType, CResizingDialog)
   ON_BN_CLICKED(IDC_AlignLayers, OnBnClickedAlignLayers)
   ON_BN_CLICKED(IDC_DrawToFlash, OnBnClickedDrawToFlash)
   ON_WM_CLOSE()
   ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)

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
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGESetupDataType, CResizingDialog)  
   //ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x6, BeforeRowColChangeLayerGrid, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL)
   //ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x8, BeforeSelChangeLayerGrid, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL)
   //ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x7, AfterRowColChangeLayerGrid, VTS_I4 VTS_I4 VTS_I4 VTS_I4)

   ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x9, AfterSelChangeLayerGrid, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
   ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x15, BeforeEditLayerGrid, VTS_I4 VTS_I4 VTS_BOOL)
   ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x1c, ChangeEditLayerGrid, VTS_NONE)
   ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x5, BeforeMouseDownLayerGrid, VTS_I2 VTS_I2 VTS_R4 VTS_R4 VTS_BOOL)
   ON_EVENT(CGESetupDataType, ID_LayerGrid, 0x3, EnterLayerGrid, VTS_NONE)
	//ON_EVENT(CGESetupDataType, ID_LayerGrid, DISPID_CLICK, OnClickLayerGrid, VTS_NONE)
	//ON_EVENT(CGESetupDataType, ID_LayerGrid, DISPID_DBLCLICK, OnDblClickLayerGrid, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CGESetupDataType::OnInitDialog()
{
	// Call OnInitDialog(), then createAndLoad(), then restoreWindowState()
	CResizingDialog::OnInitDialog();
   m_toolBar.createAndLoad(IDR_GerberEducatorReviewComponentToolbar);
	CResizingDialog::restoreWindowState();

   // Create grid
   m_layerGrid.substituteForDlgItem(ID_LayerGrid,IDC_LayerGridStatic,"FlexGrid",*this);
   
   // Initially disable both buttons
   GetDlgItem(IDC_AlignLayers)->EnableWindow(FALSE);
   GetDlgItem(IDC_DrawToFlash)->EnableWindow(FALSE);

   m_camCadDatabase.getCamCadDoc().SetShowAllFile(true);
   initGrid();
   loadGrid();
   moveLayersToGerberPCB();
   updateDrawFlashLayerAndCount();


	// Turn off grid
	m_camCadDatabase.getCamCadDoc().getSettings().Grid = FALSE;

   // Need to update view so show overrided colors
   m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);

   m_initialized = true;
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CGESetupDataType::initGrid()
{
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
   getFlexGrid().put_SelectionMode(CFlexGrid::flexSelectionListBox); //:flexSelectionByRow);

   getFlexGrid().setCell(0, m_colColor, CONST_COLOR);
   getFlexGrid().setCell(0, m_colLayerName, CONST_LAYERNAME);
   getFlexGrid().setCell(0, m_colDataSource, CONST_DATASOURCE);
   getFlexGrid().setCell(0, m_colDataType, CONST_DATATYPE);
   getFlexGrid().setCell(0, m_colDraws, CONST_DRAWS);
   getFlexGrid().setCell(0, m_colFlashes, CONST_FLASHES);
   getFlexGrid().autoSizeColumns();

   // Hide the column because it is only user to store the list of degrees that should be allowed for each insert
   // The data is filled in loadGrid()
   getFlexGrid().put_ColHidden((long)m_colLayerIndex, TRUE);
}

void CGESetupDataType::loadGrid()
{
   int dataTypeKw   = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);
   int dataSourceKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);

   for (int i=0; i<m_camCadDatabase.getCamCadDoc().getMaxLayerIndex(); i++)
   {
      LayerStruct* layer = m_camCadDatabase.getCamCadDoc().getLayerAt(i);

      if (layer == NULL)
         continue;

      // Initially turn off visibility of all layers;
      layer->setVisible(false);

      CString layerName, dataSource, dataType;
      m_camCadDatabase.getAttributeStringValue(dataSource,layer->attributes(),dataSourceKw);
      m_camCadDatabase.getAttributeStringValue(dataType  ,layer->attributes(),dataTypeKw);

      if (dataSource.CompareNoCase(QDataSourceGerberEducatorInternal) != 0)
      {
         GerberEducatorDataSourceTag dataSourceTag         = stringToGerberEducatorDataSource(dataSource);
         GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(dataType);
         if (dataSourceTag == gerberEducatorDataSourceUndefined)
            continue;

         if (dataSourceTypeTag == gerberEducatorDataSourceTypeUndefined)
         {
            // If not Data Type then auto assign Data Type base on layer type
            dataSourceTypeTag = autoAssignDataType(layer, dataSourceTag);
         }

         layerName  = layer->getName();
         dataSource = gerberEducatorDataSourceToString(dataSourceTag);
         dataType   = gerberEducatorDataSourceTypeToString(dataSourceTypeTag);

         CString item;
         item.AppendFormat("%d\t\t%s\t%s\t%s\tn/a\tn/a",  i, layerName, dataSource, dataType);
         getFlexGrid().addItem(item, (int)getFlexGrid().get_Rows());
         getFlexGrid().setCellBackroundColor((int)getFlexGrid().get_Rows()-1, m_colColor, layer->getColor());
         
         updateLayerTypeAndColor((int)getFlexGrid().get_Rows()-1);
      }
   }

   if (getFlexGrid().get_Rows() > 1)
   {
      COleVariant zero((long)0);
      COleVariant one((long)1);
      getFlexGrid().put_Cell(CFlexGrid::flexcpAlignment, 
         zero, zero, (COleVariant)(long)(getFlexGrid().get_Rows()-1), (COleVariant)(long)(m_colCount -1),
         (COleVariant)(long)CFlexGrid::flexAlignLeftCenter);

      CString dataSource;

      for (GerberEducatorDataSourceTag dataSourceTag = gerberEducatorDataSourceLowerBound;
           dataSourceTag < gerberEducatorDataSourceUpperBound;
           dataSourceTag = (GerberEducatorDataSourceTag)(dataSourceTag + 1))
      {
         dataSource += (dataSource.IsEmpty() ? "" : "|") + gerberEducatorDataSourceToString(dataSourceTag);
      }

      getFlexGrid().put_ColComboList((long)m_colDataSource, dataSource);
      getFlexGrid().autoSizeColumns();

      getFlexGrid().put_ColSort(m_colDataSource, CFlexGrid::flexSortGenericAscending);
      getFlexGrid().select(1, m_colDataSource, 1, m_colDataSource);
      getFlexGrid().put_Sort(CFlexGrid::flexSortUseColSort);
   
      // Clear selection
      getFlexGrid().select(0, 0, 0, 0);

      if (m_gerberEducatorUi.getAlignLayerCount() > 0)
      {
         for (int i=0; i<m_gerberEducatorUi.getAlignLayerCount(); i++)
         {
            LayerStruct* layer = m_gerberEducatorUi.getAlignLayer(i);
            if (layer == NULL)
               continue;

            long row = isLayerInGrid(layer->getLayerIndex());
            if (row > 0)
            {
               getFlexGrid().put_IsSelected(row, TRUE);
            }
         }

         m_gerberEducatorUi.emptyAlignLayerArry();
      }
      else
      {
         getFlexGrid().select(0, 0, 0, 0);
      }

      setVisibleLayer();
   }
}

void CGESetupDataType::setVisibleLayer()
{
   for (int i=1; i<getFlexGrid().get_Rows(); i++)
   {
      LayerStruct* layer = getLayerByRow(i);
      if (layer != NULL)
         layer->setVisible(false);
   }

   for (int i=0; i<getFlexGrid().get_SelectedRows(); i++)
   {
      long row = getFlexGrid().get_SelectedRow(i);
      LayerStruct* layer = getLayerByRow(row);
      if (layer != NULL)
         layer->setVisible(true);
   }

   m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
}

void CGESetupDataType::setActionButtons()
{
   // Initially disable both buttons
   GetDlgItem(IDC_AlignLayers)->EnableWindow(FALSE);
   GetDlgItem(IDC_DrawToFlash)->EnableWindow(FALSE);

   if (getFlexGrid().get_SelectedRows() == 1)
   {
      long row = getFlexGrid().get_SelectedRow(0);
      CString dataType = getFlexGrid().getCellText(row, m_colDataType);
      CString drawsCount = getFlexGrid().getCellText(row, m_colDraws); 

      GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(dataType);
      if (dataSourceTypeTag == gerberEducatorDataSourceTypePadBottom || dataSourceTypeTag == gerberEducatorDataSourceTypePadTop)
      {
         if (drawsCount.CompareNoCase("n/a") != 0 && atoi(drawsCount) > 0)
         {
            GetDlgItem(IDC_DrawToFlash)->EnableWindow(TRUE);
         }
      }
   }
   else if (getFlexGrid().get_SelectedRows() > 1)
   {
      GetDlgItem(IDC_AlignLayers)->EnableWindow(TRUE);
   }
}

void CGESetupDataType::moveLayersToGerberPCB()
{
   //LayerStruct* gerberEudcatorCentroidTopLayer = NULL;
   //LayerStruct* gerberEudcatorCentroidBottomLayer = NULL;

   FileStruct* gerberFile = getGerberPCBFile();
   CFileList& fileList = m_camCadDatabase.getCamCadDoc().getFileList();
   for (POSITION pos = fileList.GetHeadPosition(); pos != NULL;)
   {
      FileStruct* file = fileList.GetNext(pos);
      if (file == NULL || file == gerberFile || file->getBlock() == NULL)
         continue;

      for (POSITION pos = file->getBlock()->getHeadDataPosition(); pos != NULL;)
      {
         POSITION curPos = pos;
         DataStruct* data = file->getBlock()->getNextData(pos);
         if (data == NULL)
            continue;

         long row = isLayerInGrid(data->getLayerIndex());
         if (row < 1)
            continue;

         //LayerStruct* layer = m_camCadDatabase.getLayerAt(data->getLayerIndex());
         //if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidTop)) == 0 )
         //{
         //   int layerIndex = m_camCadDatabase.getDefinedLayerIndex(m_camCadDatabase.getCamCadLayerName(ccLayerEducatorCentroidTop), false, layerTypeCentroidTop);
         //   data->setLayerIndex(layerIndex);               
         //   updateLayerTypeAndColor(row);

         //}
         //else if (layer->getName().CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidBottom)) == 0 )
         //{
         //   int layerIndex = m_camCadDatabase.getDefinedLayerIndex(m_camCadDatabase.getCamCadLayerName(ccLayerEducatorCentroidBottom), false, layerTypeCentroidBottom);
         //   data->setLayerIndex(layerIndex); 
         //   updateLayerTypeAndColor(row);
         //}

         //LayerStruct* layer = NULL;
         //if (!gerberLayerIndexMap->Lookup(data->getLayerIndex(), layer))
         //   continue;
         
         // Remove data from file
         file->getBlock()->getDataList().RemoveAt(curPos);

         // Add data to Gerber Educator file
         gerberFile->getBlock()->getDataList().AddTail(data);
      }
   }

      //// Replace the layer information with destination layer
      //CString layerIndex;
      //layerIndex.Format("%d", destinationLayer->getLayerIndex());
      //getFlexGrid().setCell(getFlexGrid().get_Row(), m_colLayerIndex, layerIndex);
      //getFlexGrid().setCell(getFlexGrid().get_Row(), m_colLayerName, destinationLayer->getName());

}

GerberEducatorDataSourceTypeTag CGESetupDataType::autoAssignDataType(LayerStruct* layer, GerberEducatorDataSourceTag dataSourceTag)
{
   if (layer == NULL || dataSourceTag == gerberEducatorDataSourceUndefined)
      return gerberEducatorDataSourceTypeUndefined;

   int dataTypeKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);

   CString dataType;
   m_camCadDatabase.getAttributeStringValue(dataType  ,layer->attributes(),dataTypeKw);

   GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(dataType);
   if (dataSourceTypeTag == gerberEducatorDataSourceTypeUndefined)
   {
      if (dataSourceTag == gerberEducatorDataSourceCentroid)
      {
         if (layer->getLayerType() == layerTypeCentroidTop)
         {
            dataSourceTypeTag = gerberEducatorDataSourceTypeTop;
         }
         else if (layer->getLayerType() == layerTypeCentroidBottom)
         {
            dataSourceTypeTag = gerberEducatorDataSourceTypeBottom;
         }
      }
      else if (dataSourceTag == gerberEducatorDataSourceGerber)
      {
         if (layer->getLayerType() == layerTypePadTop)
         {
            dataSourceTypeTag = gerberEducatorDataSourceTypePadTop;
         }
         else if (layer->getLayerType() == layerTypePadBottom)
         {
            dataSourceTypeTag = gerberEducatorDataSourceTypePadBottom;
         }
         else if (layer->getLayerType() == layerTypeSilkTop)
         {
            dataSourceTypeTag = gerberEducatorDataSourceTypeSilkscreenTop;
         }
         else if (layer->getLayerType() == layerTypeSilkBottom)
         {
            dataSourceTypeTag = gerberEducatorDataSourceTypeSilkscreenBottom;
         }
      }

      m_camCadDatabase.addAttribute(layer->attributes(),dataTypeKw  ,gerberEducatorDataSourceTypeToString(dataSourceTypeTag));
   }

   return dataSourceTypeTag;
}

void CGESetupDataType::updateLayerTypeAndColor(long row)
{
   int dataTypeKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);
   int dataSourceKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);

   LayerStruct* layer = getLayerByRow(row);
   if (layer == NULL)
      return;

   CString dataSource = getFlexGrid().getCellText(row, m_colDataSource);
   CString dataType   = getFlexGrid().getCellText(row, m_colDataType);

   GerberEducatorDataSourceTag dataSourceTag         = stringToGerberEducatorDataSource(dataSource);
   GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(dataType);

   m_camCadDatabase.addAttribute(layer->attributes(),dataSourceKw,gerberEducatorDataSourceToString(dataSourceTag));
   m_camCadDatabase.addAttribute(layer->attributes(),dataTypeKw  ,gerberEducatorDataSourceTypeToString(dataSourceTypeTag));

   // Initially set it to unknown color
   COLORREF layerColor = m_layerColor.getColor(gerberEducatorSetupDataUnknownColor);
   LayerTypeTag layerType = layerTypeUnknown;

   if (dataSourceTag == gerberEducatorDataSourceCentroid)
   {
      if (dataSourceTypeTag == gerberEducatorDataSourceTypeTop)
      {
         layerType = layerTypeCentroidTop;
         layerColor = m_layerColor.getColor(gerberEducatorSetupDataCentroidTopColor);
      }
      else if (dataSourceTypeTag == gerberEducatorDataSourceTypeBottom)
      {
         layerType = layerTypeCentroidBottom;
         layerColor = m_layerColor.getColor(gerberEducatorSetupDataCentroidBottomColor);
      }
   }
   else if (dataSourceTag == gerberEducatorDataSourceGerber)
   {
      if (dataSourceTypeTag == gerberEducatorDataSourceTypePadTop)
      {
         layerType = layerTypePadTop;
         layerColor = m_layerColor.getColor(gerberEducatorSetupDataPadTopFlashesColor);
      }
      else if (dataSourceTypeTag == gerberEducatorDataSourceTypePadBottom)
      {
         layerType = layerTypePadBottom;
         layerColor = m_layerColor.getColor(gerberEducatorSetupDataPadBottomFlashesColor);
      }
      else if (dataSourceTypeTag == gerberEducatorDataSourceTypeSilkscreenTop)
      {
         layerType = layerTypeSilkTop;
         layerColor = m_layerColor.getColor(gerberEducatorSetupDataSilkscreenTopColor);
      }
      else if (dataSourceTypeTag == gerberEducatorDataSourceTypeSilkscreenBottom)
      {
         layerType = layerTypeSilkBottom;
         layerColor = m_layerColor.getColor(gerberEducatorSetupDataSilkscreenBottomColor);
      }
      else if (dataSourceTypeTag == gerberEducatorDataSourceTypeBoardOutline)
      {
         layerType = layerTypeBoardOutline;
      }
   }

   layer->setColor(layerColor);
   layer->setLayerType(layerType);

   getFlexGrid().setCellBackroundColor(row, m_colColor, layerColor);
}

void CGESetupDataType::updateDrawFlashLayerAndCount()
{
   CMapStringToInt drawsOnLayerMap;
   CMapStringToInt flashesOnLayerMap;
   int dataSourceKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);
 
   FileStruct* gerberFile = getGerberPCBFile();
   for (CDataListIterator dataList(*gerberFile->getBlock()); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();
      if (data->getDataType() != dataTypePoly && data->getDataType() != dataTypeInsert)
         continue;

      LayerStruct* layer = m_camCadDatabase.getLayerAt(data->getLayerIndex());
      if (layer == NULL)
         continue;

      CString dataSource;
      m_camCadDatabase.getAttributeStringValue(dataSource,layer->attributes(),dataSourceKw);
      GerberEducatorDataSourceTag dataSourceTag = stringToGerberEducatorDataSource(dataSource);

      COLORREF overrideColor = layer->getColor();
      bool colorOverride = false;

      if (data->getDataType() == dataTypePoly)
      {
         int drawCount = 0;
         drawsOnLayerMap.Lookup(layer->getName(), drawCount);
         drawsOnLayerMap.SetAt(layer->getName(), ++drawCount);         

         if (layer->getLayerType() == layerTypePadTop)
         {
            overrideColor = m_layerColor.getColor(gerberEducatorSetupDataPadTopDrawsColor);
		      colorOverride = true;
         }
         else if (layer->getLayerType() == layerTypePadBottom)
         {
            overrideColor = m_layerColor.getColor(gerberEducatorSetupDataPadBottomDrawsColor);
		      colorOverride = true;
         }
      }
      else if (data->getDataType() == dataTypeInsert)
      {
         int flashCount = 0;
         flashesOnLayerMap.Lookup(layer->getName(), flashCount);
         flashesOnLayerMap.SetAt(layer->getName(), ++flashCount);         

         if (layer->getLayerType() == layerTypePadTop)
         {
            overrideColor = m_layerColor.getColor(gerberEducatorSetupDataPadTopFlashesColor);
		      colorOverride = true;
         }
         else if (layer->getLayerType() == layerTypePadBottom)
         {
            overrideColor = m_layerColor.getColor(gerberEducatorSetupDataPadBottomFlashesColor);
		      colorOverride = true;
         }
      }
      
      data->setOverrideColor(overrideColor);
      data->setColorOverride(colorOverride);
   }

   // Update the grid with the count of draws and flashes
   CString drawCountString;
   CString flashCountString;
   for (int row=1; row<getFlexGrid().get_Rows(); row++)
   {
      CString layerName = getFlexGrid().getCellText(row, m_colLayerName);        
      CString dataSource = getFlexGrid().getCellText(row, m_colDataSource);         
      GerberEducatorDataSourceTag dataSourceTag = stringToGerberEducatorDataSource(dataSource);

      if (dataSourceTag == gerberEducatorDataSourceGerber)
      {
         int drawCount =0;
         int flashCount = 0;

         drawsOnLayerMap.Lookup(layerName, drawCount);
         flashesOnLayerMap.Lookup(layerName, flashCount);

         drawCountString.Format("%d", drawCount);
         flashCountString.Format("%d", flashCount);

         getFlexGrid().setCell(row, m_colDraws, drawCountString);
         getFlexGrid().setCell(row, m_colFlashes, flashCountString);
      }
      else
      {
         getFlexGrid().setCell(row, m_colDraws, "n/a");
         getFlexGrid().setCell(row, m_colFlashes, "n/a");
      }
   }
}

long CGESetupDataType::isLayerInGrid(int layerIndex)
{
   for (int row=1; row<getFlexGrid().get_Rows(); row++)
   {
     if (layerIndex == atoi(getFlexGrid().getCellText(row, m_colLayerIndex)))
        return row;
   }

   return 0;
}

LayerStruct* CGESetupDataType::getLayerByRow(long row)
{
   if (row > 0 && row < getFlexGrid().get_Rows())
   {
      int layerIndex = atoi(getFlexGrid().getCellText(row, m_colLayerIndex));
      LayerStruct* layer = m_camCadDatabase.getCamCadDoc().getLayerAt(layerIndex);
      
      return layer;
   }

   return NULL;
}

FileStruct* CGESetupDataType::getGerberPCBFile()
{
   //FileStruct* gerberFile = NULL;

   //for(POSITION pos = m_camCadDatabase.getCamCadDoc().getFileList().GetHeadPosition(); pos != NULL;)
   //{
   // FileStruct* file = m_camCadDatabase.getCamCadDoc().getFileList().GetNext(pos);
   // if (file == NULL)
   //    continue;

   // if (file->getName().CompareNoCase(QGerberEducatorPCB) == 0)
   // {
   //    gerberFile = file;
   //    gerberFile->setShow(true);
   //    break;
   // }
   // else
   // {
   //    file->setShow(false);
   // }
   //}


   FileStruct* gerberFile = m_camCadDatabase.getCamCadDoc().FileFileByName(QGerberEducatorPCB); 
   if (gerberFile == NULL)
   {
      gerberFile =  m_camCadDatabase.getCamCadDoc().Add_File(QGerberEducatorPCB, fileTypeGerber);
      gerberFile->setBlockType(blockTypePcb);
      gerberFile->setNotPlacedYet(false);
   }

   if (m_gerberEducatorUi.getFileStruct() != gerberFile)
   {
      m_gerberEducatorUi.setFileStruct(gerberFile);
   }

   m_camCadDatabase.getCamCadDoc().SetShowAllFile(false);
   gerberFile->setShow(true);

   return gerberFile;
}

void CGESetupDataType::BeforeRowColChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol, VARIANT_BOOL* Cancel)
{
}

void CGESetupDataType::BeforeSelChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol, VARIANT_BOOL* Cancel)
{
}

void CGESetupDataType::AfterRowColChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol)
{
}

void CGESetupDataType::AfterSelChangeLayerGrid(long OldRow, long OldCol, long NewRow, long NewCol)
{
   setVisibleLayer();
   setActionButtons();
}

void CGESetupDataType::BeforeEditLayerGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
   if (Col == m_colDataType)
   {
      CString dataSource = getFlexGrid().getCellText((int)Row, m_colDataSource);
      if (dataSource.CompareNoCase(QDataSourceUnknown) == 0)
      {
         getFlexGrid().put_ComboList("");
         getFlexGrid().setCell(Row, m_colDataType, QDataTypeUnknown);
      }
      else
      {
         CString dataType;
         if (dataSource.CompareNoCase(QDataSourceGerber) == 0)
         {
            dataType.Format("%s|%s|%s|%s|%s", 
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeUndefined),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypePadTop),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypePadBottom),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeSilkscreenTop),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeSilkscreenBottom)  );
         }
         else if (dataSource.CompareNoCase(QDataSourceCentroid) == 0)
         {
            dataType.Format("%s|%s|%s", 
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeUndefined),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeTop),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeBottom)  );
         }
         else if (dataSource.CompareNoCase(QDataSourceDrill) == 0)
         {
            dataType.Format("%s|%s|%s|%s", 
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeUndefined),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeThru),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeBlind),
               gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeBuried)  );
         }

         getFlexGrid().put_ComboList(dataType);
         getFlexGrid().put_ComboIndex(0);
      }
   }
   else 
   {
      getFlexGrid().put_ComboList("");
   }
}

void CGESetupDataType::ChangeEditLayerGrid()
{
   long row = getFlexGrid().get_Row();
   if (getFlexGrid().get_Col() == m_colDataSource)
   {
      CString curDataSource = getFlexGrid().getCellText(row, m_colDataSource);
      CString editDataSource = getFlexGrid().get_EditText();

      if (editDataSource.CompareNoCase(curDataSource) != 0)
      {
         //LayerStruct* layer = getLayerByRow(row);
         GerberEducatorDataSourceTag dataSourceTag = stringToGerberEducatorDataSource(editDataSource);
         //GerberEducatorDataSourceTypeTag dataSourceTypeTag = autoAssignDataType(layer, dataSourceTag);
         CString dataType = gerberEducatorDataSourceTypeToString(gerberEducatorDataSourceTypeUndefined);

         getFlexGrid().setCell(row, m_colDataSource, editDataSource);
         getFlexGrid().setCell(row, m_colDataType, dataType);

         updateLayerTypeAndColor(row);
         updateDrawFlashLayerAndCount();

         m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
      }

      setActionButtons();
   }
   else if (getFlexGrid().get_Col() == m_colDataType)
   {
      CString curDataType = getFlexGrid().getCellText(row, m_colDataType);
      CString editDataType = getFlexGrid().get_EditText();

      if (editDataType.CompareNoCase(curDataType) != 0)
      {
         getFlexGrid().setCell(row, m_colDataType, editDataType);

         updateLayerTypeAndColor(row);
         updateDrawFlashLayerAndCount();

         GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(editDataType);
         if (dataSourceTypeTag == gerberEducatorDataSourceTypePadBottom || dataSourceTypeTag == gerberEducatorDataSourceTypePadTop)
         {
            GetDlgItem(IDC_DrawToFlash)->EnableWindow(TRUE);
         }
         else
         {
            GetDlgItem(IDC_DrawToFlash)->EnableWindow(FALSE);
         }
      
         m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
      }

//      setAction();
   }

   getFlexGrid().autoSizeColumns();
}

void CGESetupDataType::BeforeMouseDownLayerGrid(short Button, short Shift, float X, float Y, VARIANT_BOOL* Cancel)
{
   m_layerGrid.restoreWindowPlacement();
}

void CGESetupDataType::EnterLayerGrid()
{
   long mouseCol = getFlexGrid().get_MouseCol();
   long col = getFlexGrid().get_Col();

   if (mouseCol == m_colDataSource  || col == m_colDataSource ||
       mouseCol == m_colDataType    || col == m_colDataType)
   {
      getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
   }
   else
   {
      getFlexGrid().put_Editable(CFlexGrid::flexEDNone);
   }
}

void CGESetupDataType::OnBnClickedZoomIn()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMIN);
   }
}

void CGESetupDataType::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnBnClickedZoomOut()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMOUT);
   }
}

void CGESetupDataType::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnBnClickedZoomWindow()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMWINDOW);
   }
}

void CGESetupDataType::OnUpdateZoomWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnBnClickedZoomExtents()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMEXTENTS);
   }
}

void CGESetupDataType::OnUpdateZoomExtents(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnBnClickedZoomFull()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOM11);
   }
}

void CGESetupDataType::OnUpdateZoomFull(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnBnClickedPanCenter()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_PAN);
   }
}

void CGESetupDataType::OnUpdatePanCenter(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnBnClickedQueryItem()
{
   m_camCadDatabase.getCamCadDoc().OnEditEntity();
}

void CGESetupDataType::OnUpdateQueryItem(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnBnClickedRepaint()
{
   m_camCadDatabase.getCamCadDoc().OnRedraw();
}

void CGESetupDataType::OnUpdateRepaint(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGESetupDataType::OnDblClickLayerGrid()
{
}
void CGESetupDataType::OnBnClickedClose()
{
   OnClose();
}

void CGESetupDataType::OnClose()
{
   // Change layers that are ccLayerCentroidTop and ccLayerCentroidBottom to ccLayerEducatorCentroidTop and ccLayerEducatorCentroidBottom
   for (long row=1; row<getFlexGrid().get_Rows(); row++)
   {
      LayerStruct* layer = getLayerByRow(row);
      if (layer == NULL)
         continue;
      
      CString layerName = layer->getName();
      if (layerName.CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidTop))    == 0 ||
          layerName.CompareNoCase(m_camCadDatabase.getCamCadLayerName(ccLayerCentroidBottom)) == 0 )
      {
         layerName.Remove(' ');
         layer->setName(layerName);
      }
   }   

	m_layerColor.setDefaultViewColor(m_camCadDatabase, *m_gerberEducatorUi.getFileStruct());

   CResizingDialog::OnClose();

   m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
   m_gerberEducatorUi.generateEvent(ID_GerberEducatorCommand_TerminateSession);

   EndDialog(IDCANCEL);
}
void CGESetupDataType::OnBnClickedAlignLayers()
{
   m_gerberEducatorUi.emptyAlignLayerArry();

   for (int i=0; i<getFlexGrid().get_SelectedRows(); i++)
   {
      long row = getFlexGrid().get_SelectedRow(i);
      LayerStruct* layer = getLayerByRow(row);
      if (layer != NULL)
      {
         m_gerberEducatorUi.addAlignLayer(layer);
      }
   }

   m_gerberEducatorUi.OnEventAlignLayer();
}

void CGESetupDataType::OnBnClickedDrawToFlash()
{
   CFlashRecognizerParameters flashRecognizerParameters(m_camCadDatabase);
   flashRecognizerParameters.loadFromRegistry();
   flashRecognizerParameters.setGenerateClusterDisplayFlag(false);
   flashRecognizerParameters.setGenerateDiagnosticLayersFlag(false);
   flashRecognizerParameters.setSegregateClusterAperturesFlag(false);

   LayerStruct* sourceLayer = getLayerByRow(getFlexGrid().get_Row());

   CFlashRecognizer flashRecognizer(m_camCadDatabase,flashRecognizerParameters,sourceLayer->getLayerIndex());
   flashRecognizer.scanData();
   flashRecognizer.clusterSegments();

   LayerStruct* destinationLayer = flashRecognizer.convertClustersToApertures();

   // Copy attribute from source layer to destination layer
   if (sourceLayer != NULL && destinationLayer != NULL)
   {
      int dataSourceKw = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);
      int dataTypeKw   = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);

      CString dataSource, dataType;
      m_camCadDatabase.getAttributeStringValue(dataSource,sourceLayer->attributes(),dataSourceKw);
      m_camCadDatabase.getAttributeStringValue(dataType  ,sourceLayer->attributes(),dataTypeKw);

      // Add the Data Source and Data Type to destination layer
      destinationLayer->setAttrib(m_camCadDatabase.getCamCadData(), dataSourceKw, valueTypeString, dataSource.GetBuffer(0), attributeUpdateOverwrite, NULL);
      destinationLayer->setAttrib(m_camCadDatabase.getCamCadData(), dataTypeKw, valueTypeString, dataType.GetBuffer(0), attributeUpdateOverwrite, NULL);
      destinationLayer->setLayerType(sourceLayer->getLayerType());

      // Replace the layer information with destination layer
      CString layerIndex;
      layerIndex.Format("%d", destinationLayer->getLayerIndex());
      getFlexGrid().setCell(getFlexGrid().get_Row(), m_colLayerIndex, layerIndex);
      getFlexGrid().setCell(getFlexGrid().get_Row(), m_colLayerName, destinationLayer->getName());

      // Remove the Data Source and Data Type from destination layer
      RemoveAttrib((WORD)dataSourceKw, &sourceLayer->getAttributesRef());
      RemoveAttrib((WORD)dataTypeKw, &sourceLayer->getAttributesRef());
      sourceLayer->setVisible(false);
   }

   setVisibleLayer();
   updateLayerTypeAndColor(getFlexGrid().get_Row());
   updateDrawFlashLayerAndCount();
   m_camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
}
