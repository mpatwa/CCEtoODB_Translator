// $Header: /CAMCAD/4.6/GerberEducatorDialog.cpp 84    2/14/07 4:03p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "Resource.h"

#if defined(EnableGerberEducator)

#include "GerberEducatorDialog.h"
#include "GerberEducator.h"
#include "Colors.h"
#include ".\gerbereducatordialog.h"
#include "GerberEducatorUi.h"
#include "Edit.h"

//_____________________________________________________________________________
CGerberEducatorBaseDialog::CGerberEducatorBaseDialog(int dialogId,CGerberEducator& gerberEducator)
: CResizingDialog(dialogId,NULL)
, m_gerberEducator(gerberEducator)
, m_gerberEducatorUi(NULL)
{
}

CGerberEducatorBaseDialog::~CGerberEducatorBaseDialog()
{
}

CCEtoODBDoc& CGerberEducatorBaseDialog::getCamCadDoc() 
{ 
   return m_gerberEducator.getCamCadDoc(); 
}

CCamCadDatabase& CGerberEducatorBaseDialog::getCamCadDatabase() 
{ 
   return m_gerberEducator.getCamCadDatabase(); 
}

void CGerberEducatorBaseDialog::setGerberEducatorUi(CGerberEducatorUi* gerberEducatorUi)
{
   m_gerberEducatorUi = gerberEducatorUi;
}

bool CGerberEducatorBaseDialog::isWorkingSurfaceTop()
{
   return false;
}

bool CGerberEducatorBaseDialog::getDisplayWorkingSurfaceFlag()
{
   return false;
}

bool CGerberEducatorBaseDialog::getDisplayBothSurfacesFlag()
{
   return false;
}

bool CGerberEducatorBaseDialog::isSurfaceDisplayed(bool topFlag)
{
   bool retval = ((isWorkingSurfaceTop() == topFlag) || getDisplayBothSurfacesFlag());

   return retval;
}

//_____________________________________________________________________________
CGerberEducatorCreateGeometryDialog::CGerberEducatorCreateGeometryDialog(CGerberEducator& gerberEducator)
: CGerberEducatorBaseDialog(CGerberEducatorCreateGeometryDialog::IDD,gerberEducator)
, m_pinList(CGerberEducator::getSearchTolerance())
//, m_toolBar(*this)
, m_toolBar1(*this)
, m_toolBar2(*this)
, m_defaultSelectedEducatorLayerFilter(educatorLayerFilterPinsCentroids)
, m_educatorLayerFilter(m_defaultSelectedEducatorLayerFilter)
, m_lastSelectedEducatorLayerFilter(m_defaultSelectedEducatorLayerFilter)
{
   //m_status = selectListStatusUndefined;
   //addFieldControl(IDC_GenerationStatus,anchorLeft,growHorizontal);

   m_activeState                 = WA_INACTIVE;
   m_topViewFlag                 = true;
   m_thMountFlag                 = true;
   m_lastUserThMount             = boolUnknown;

   m_toolBarRepaintIndex         =
   m_toolBarMarkByRectangleIndex = -1;
   m_centroidsLockedFlag         = true;
   //m_selectFilterId              = -1;

   m_centroid                    = NULL;
   m_previousCentroid            = NULL;

   setEnableResize(false);

   initializeFilterIds();
}

CGerberEducatorCreateGeometryDialog::~CGerberEducatorCreateGeometryDialog()
{
   for (EducatorLayerFilterTag filter = educatorLayerFilterLowerBound;filter <= educatorLayerFilterUpperBound;
        filter = (EducatorLayerFilterTag)(filter + 1))
   {
      removeSelectFilter(filter);
   }
}

void CGerberEducatorCreateGeometryDialog::initializeFilterIds()
{
   for (EducatorLayerFilterTag filter = educatorLayerFilterLowerBound;filter <= educatorLayerFilterUpperBound;
        filter = (EducatorLayerFilterTag)(filter + 1))
   {
      m_selectFilterIds[getSelectFilterIndex(filter,false,false)] = 0;
      m_selectFilterIds[getSelectFilterIndex(filter,false, true)] = 0;
      m_selectFilterIds[getSelectFilterIndex(filter, true,false)] = 0;
      m_selectFilterIds[getSelectFilterIndex(filter, true, true)] = 0;
   }
}

int CGerberEducatorCreateGeometryDialog::getToolBarIndex(int id)
{
   int* retval = NULL;

   switch (id)
   {
   case IDC_Repaint:         retval = &m_toolBarRepaintIndex;          break;
   case IDC_MarkByWindow:    retval = &m_toolBarMarkByRectangleIndex;  break;
   }

   if (retval == NULL)
   {
      return -1;
   }

   if (*retval < 0)
   {
      *retval = m_toolBar1.CommandToIndex(id);
   }

   return *retval;
}

void CGerberEducatorCreateGeometryDialog::DoDataExchange(CDataExchange* pDX)
{
   CGerberEducatorBaseDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_AddIndividualComponent, m_addIndividualComponentButton);
   DDX_Control(pDX, IDC_AddComponentsByPatternMatching, m_addComponentsByPatternButton);
   //DDX_Control(pDX, IDC_AddComponentsByOrthogonalPatternMatching, m_addComponentsByOrthogonalPatternButton);
   //DDX_Control(pDX, IDC_AddComponentsAutomatically, m_addComponentsAutomaticallyByCentroidButton);
   DDX_Control(pDX, IDC_DeleteIndividualComponent, m_deleteIndividualComponentButton);
   DDX_Control(pDX, IDC_DeleteComponentsByGeometry, m_deleteComponentsByGeometryButton);
   DDX_Control(pDX, IDC_MountTypeStatic, m_mountGroup);
   DDX_Control(pDX, IDC_ThMount, m_thMountButton);
   DDX_Control(pDX, IDC_SmdMount, m_smdMountButton);
   DDX_Control(pDX, IDC_TopView, m_topViewButton);
   DDX_Control(pDX, IDC_BottomView, m_bottomViewButton);
   DDX_Control(pDX, IDC_ViewStatic, m_viewSurfaceGroup);

   DDX_Control(pDX, IDC_DataTypeLabel, m_dataTypeLabelStatic);
   DDX_Control(pDX, IDC_GerberPadsLabel, m_gerberPadsLabelStatic);
   DDX_Control(pDX, IDC_CentroidsLabel, m_centroidsLabelStatic);
   DDX_Control(pDX, IDC_ComponentsLabel, m_componentsLabelStatic);
   DDX_Control(pDX, IDC_GeometriesLabel, m_geometriesLabelStatic);
   DDX_Control(pDX, IDC_TopLabel, m_topLabelStatic);
   DDX_Control(pDX, IDC_BottomLabel, m_bottomLabelStatic);
   DDX_Control(pDX, IDC_TopGerberPadsStatic, m_gerberPadsTopCountStatic);
   DDX_Control(pDX, IDC_BottomGerberPadsStatic, m_gerberPadsBottomCountStatic);
   DDX_Control(pDX, IDC_TopCentroidsStatic, m_centroidsTopCountStatic);
   DDX_Control(pDX, IDC_BottomCentroidsStatic, m_centroidsBottomCountStatic);
   DDX_Control(pDX, IDC_TopComponentsStatic, m_componentsTopCountStatic);
   DDX_Control(pDX, IDC_BottomComponentsStatic, m_componentsBottomCountStatic);
   DDX_Control(pDX, IDC_GeometriesStatic, m_geometriesCountStatic);
   DDX_Control(pDX, IDC_GerberPadsLabel2, m_selectedGerberPadsLabelStatic);
   DDX_Control(pDX, IDC_CentroidsLabel2, m_selectedCentroidsLabelStatic);
   DDX_Control(pDX, IDC_DrillsLabel, m_selectedDrillsLabelStatic);
   DDX_Control(pDX, IDC_SelectedSmPadsStatic, m_selectedPinPadsCountStatic);
   DDX_Control(pDX, IDC_SelectedThPadsStatic, m_derivedDrillsCountStatic);
   DDX_Control(pDX, IDC_SelectedCentroidsStatic, m_selectedCentroidsCountStatic);
   DDX_Control(pDX, IDC_PcbComponentsLabel, m_pcbCompsLabelStatic);
   DDX_Control(pDX, IDC_MechComponentsLabel, m_mechCompsLabelStatic);
   DDX_Control(pDX, IDC_ToolingLabel, m_toolingLabelStatic);
   DDX_Control(pDX, IDC_FiducialLabel, m_fiducialLabelStatic);
   DDX_Control(pDX, IDC_PcbComponentsStatic, m_selectedPcbComponentCountStatic);
   DDX_Control(pDX, IDC_MechComponentsStatic, m_selectedMechComponentCountStatic);
   DDX_Control(pDX, IDC_ToolingStatic, m_selectedToolingCountStatic);
   DDX_Control(pDX, IDC_FiducialStatic, m_selectedFiducialCountStatic);
   DDX_Control(pDX, IDC_CreateComponents, m_createComponentsButton);
   DDX_Control(pDX, IDC_DeleteComponents, m_deleteComponentsButton);
   DDX_Control(pDX, IDC_DisplayComponents, m_displayComponentsButton);
   DDX_Control(pDX, IDC_DisplayPads, m_displayPadsButton);
   DDX_Control(pDX, IDC_DisplaySilkscreen, m_displaySilkscreenButton);

   DDX_Control(pDX, IDC_DisplayWorkingSurface, m_displayWorkingSurfaceButton);
   DDX_Control(pDX, IDC_DisplayBothSurfaces, m_displayBothSurfacesButton);
   DDX_Control(pDX, IDC_AllowSelection, m_allowIdleSurfaceSelectionButton);
   DDX_Control(pDX, IDC_GenerateCentroidLayers, m_generateCentroidLayersButton);
   DDX_Control(pDX, IDC_AddAutomatically, m_addAutomaticallyButton);
}

SelectListStatusTag CGerberEducatorCreateGeometryDialog::setFencedStatus(SelectListStatusTag status)
{
   if (m_status == selectListStatusOk)
   {
      m_status = status;
   }

   return m_status;
}

bool CGerberEducatorCreateGeometryDialog::getOkToAdd()
{
   bool okToAdd = (m_status == selectListStatusOk) ||
                  (m_status == selectListStatusPadsFromBothLayers);

   return okToAdd;
}

bool CGerberEducatorCreateGeometryDialog::isWorkingSurfaceTop()
{
   return m_topViewFlag;
}

bool CGerberEducatorCreateGeometryDialog::getDisplayWorkingSurfaceFlag()
{
   return (m_displayWorkingSurfaceButton.GetCheck() != 0);
}

bool CGerberEducatorCreateGeometryDialog::getDisplayBothSurfacesFlag()
{
   return (m_displayBothSurfacesButton.GetCheck() != 0);
}

bool CGerberEducatorCreateGeometryDialog::getAllowIdleSurfaceSelectionFlag()
{
   return (m_allowIdleSurfaceSelectionButton.GetCheck() != 0 &&
           m_displayBothSurfacesButton.GetCheck() != 0);
}

int CGerberEducatorCreateGeometryDialog::getSelectFilterIndex(EducatorLayerFilterTag educatorLayerFilter,bool topViewFlag,bool bothSurfacesFlag)
{
   int selectFilterIndex = (educatorLayerFilter * 4) + topViewFlag*2 + bothSurfacesFlag;

   return selectFilterIndex;
}

void CGerberEducatorCreateGeometryDialog::setLayerFilter(EducatorLayerFilterTag educatorLayerFilter)
{
   m_educatorLayerFilter = educatorLayerFilter;

   bool allowIdleSurfaceSelectionFlag = getAllowIdleSurfaceSelectionFlag();
   int selectFilterIndex = getSelectFilterIndex(educatorLayerFilter,m_topViewFlag,allowIdleSurfaceSelectionFlag);
   int filterId = m_selectFilterIds[selectFilterIndex];

   if (getCamCadDoc().getSelectLayerFilterStack().moveToTop(filterId) < 0)
   {
      CLayerFilter layerFilter(true);

      if (educatorLayerFilter == educatorLayerFilterEverything)
      {
         layerFilter.addAll();
      }
      else
      {
         layerFilter.removeAll();

         // pins
         if (educatorLayerFilter == educatorLayerFilterPinsCentroidsComponents ||
             educatorLayerFilter == educatorLayerFilterPinsCentroids           ||
             educatorLayerFilter == educatorLayerFilterPins                      )
         {
            if (getAllowIdleSurfaceSelectionFlag())
            {
               layerFilter.add(getGerberEducator().getTopUnmatchedLayerIndex());
               layerFilter.add(getGerberEducator().getBottomUnmatchedLayerIndex());
            }
            else if (m_topViewFlag)
            {
               layerFilter.add(getGerberEducator().getTopUnmatchedLayerIndex());
            }
            else
            {
               layerFilter.add(getGerberEducator().getBottomUnmatchedLayerIndex());
            }
         }

         // components
         if (educatorLayerFilter == educatorLayerFilterPinsCentroidsComponents ||
             educatorLayerFilter == educatorLayerFilterComponents                 )
         {
            if (m_topViewFlag)
            {
               layerFilter.add(getCamCadDatabase().getLayer(ccLayerPadTop        )->getLayerIndex());
               layerFilter.add(getCamCadDatabase().getLayer(ccLayerAssemblyTop   )->getLayerIndex());
            }
            else
            {
               layerFilter.add(getCamCadDatabase().getLayer(ccLayerPadBottom     )->getLayerIndex());
               layerFilter.add(getCamCadDatabase().getLayer(ccLayerAssemblyBottom)->getLayerIndex());
            }
         }
      }

      getCamCadDoc().getSelectLayerFilterStack().push(layerFilter);
      m_selectFilterIds[selectFilterIndex] = getCamCadDoc().getSelectLayerFilterStack().getTop()->getId();
   }

   m_createComponentsButton.SetCheck(m_educatorLayerFilter == educatorLayerFilterPins || 
                                     m_educatorLayerFilter == educatorLayerFilterPinsCentroids);

   m_deleteComponentsButton.SetCheck(m_educatorLayerFilter == educatorLayerFilterComponents);
}

void CGerberEducatorCreateGeometryDialog::removeSelectFilter(EducatorLayerFilterTag educatorLayerFilter)
{
   int filterId00 = m_selectFilterIds[getSelectFilterIndex(educatorLayerFilter,false,false)];

   getCamCadDoc().getSelectLayerFilterStack().remove(filterId00);

   int filterId01 = m_selectFilterIds[getSelectFilterIndex(educatorLayerFilter,false, true)];

   getCamCadDoc().getSelectLayerFilterStack().remove(filterId01);

   int filterId10 = m_selectFilterIds[getSelectFilterIndex(educatorLayerFilter, true,false)];

   getCamCadDoc().getSelectLayerFilterStack().remove(filterId10);

   int filterId11 = m_selectFilterIds[getSelectFilterIndex(educatorLayerFilter, true, true)];

   getCamCadDoc().getSelectLayerFilterStack().remove(filterId11);
}

void CGerberEducatorCreateGeometryDialog::update()
{
   BringWindowToTop();
   ShowWindow(SW_SHOW);
   UpdateData(true);

   CSelectList& selectList = getCamCadDoc().SelectList;
   m_pinList.empty();

   m_centroid = NULL;
   DataStruct* component = NULL;
   DataStruct* pin1 = NULL;
   m_status = selectListStatusOk;
   bool multipleCentroidsFlag = false;

   int sourceLayerIndex = -1;
   bool matchedLayer   = false;
   bool unmatchedLayer = false;
   bool thFlag         = false;
   int selectedUnmatchedCentroidCount   = 0;
   int selectedComponentCount           = 0;
   int selectedPinCount                 = 0;
   int illegalDataCount                 = 0;
   int selectedPcbComponentCount        = 0;
   int selectedMechanicalComponentCount = 0;
   int selectedFiducialCount            = 0;
   int selectedDrillToolCount           = 0;

   ComponentAttributeStatus componentAttributeStatus(*this);

   // one component selected - edit component and centroid attributes
   // one centroid selected  - edit component and centroid attributes

   for (POSITION pos = selectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* selectStruct = selectList.GetNext(pos);
      DataStruct* data = selectStruct->getData();

      if (data->getDataType() != dataTypeInsert)
      {
         illegalDataCount++;
         setFencedStatus(selectListStatusIllegalData);
         continue;
      }

      bool pcbComponentFlag = true;

      switch (data->getInsert()->getInsertType())
      {
      case insertTypePcbComponent:         selectedPcbComponentCount++;         break;
      case insertTypeMechanicalComponent:  selectedMechanicalComponentCount++;  break;
      case insertTypeFiducial:             selectedFiducialCount++;             break;
      case insertTypeDrillTool:            selectedDrillToolCount++;            break;
      default:
         pcbComponentFlag = false;
         break;
      }

      if (pcbComponentFlag)
      {
         selectedComponentCount++;
         componentAttributeStatus.update(data);

         if (component == NULL)
         {
            component = data;
         }

         continue;
      }

      InsertStruct* insert = data->getInsert();
      BlockStruct* block = getCamCadDoc().getBlockAt(insert->getBlockNumber());

      bool isCentroidFlag = (block->getBlockType() == blockTypeCentroid);
      bool isApertureFlag = block->isAperture();

      if (!isCentroidFlag && !isApertureFlag)
      {
         illegalDataCount++;
         setFencedStatus(selectListStatusIllegalData);
         continue;
      }

      bool dataOnMatchedLayer = (
             (data->getLayerIndex() == getGerberEducator().getTopMatchedLayerIndex())      ||
             (data->getLayerIndex() == getGerberEducator().getBottomMatchedLayerIndex())       );

      bool dataOnUnmatchedLayer = (
            (data->getLayerIndex() == getGerberEducator().getTopUnmatchedLayerIndex())    ||
            (data->getLayerIndex() == getGerberEducator().getBottomUnmatchedLayerIndex())   );

      bool dataOnEducatorLayer = (dataOnMatchedLayer || dataOnUnmatchedLayer);

      if (!dataOnUnmatchedLayer)
      {
         illegalDataCount++;
         setFencedStatus(selectListStatusNonEducatorLayer);
         continue;
      }

      if (sourceLayerIndex == -1)
      {
         sourceLayerIndex = data->getLayerIndex();
         matchedLayer     = dataOnMatchedLayer;
         unmatchedLayer   = dataOnUnmatchedLayer;
      }
      else if (data->getLayerIndex() != sourceLayerIndex)
      {
         //if (m_centroidsLockedFlag)
         //{
         //   setFencedStatus(selectListStatusMultipleLayers);
         //}
         //else 
         if (!isCentroidFlag)
         {
            thFlag = true;
         }
      }

      if (isCentroidFlag)
      {
         selectedUnmatchedCentroidCount++;
         componentAttributeStatus.update(data);

         if (m_centroid == NULL)
         {
            m_centroid = data;
         }

         if (selectedUnmatchedCentroidCount > 1)
         {
            setFencedStatus(selectListStatusMultipleCentroids);
            multipleCentroidsFlag = true;
         }
      }
      else  // is aperture
      {
         if (pin1 == NULL)
         {
            pin1 = data;
         }

         addPinToList(data);
         selectedPinCount++;
      }
   }

   m_selectedCentroidsLabelStatic.SetTextColor(multipleCentroidsFlag ? colorRed : colorBlack);
   m_selectedCentroidsCountStatic.SetTextColor(multipleCentroidsFlag ? colorRed : colorBlack);

   m_selectedPcbComponentCountStatic.setCount(selectedPcbComponentCount);
   m_selectedMechComponentCountStatic.setCount(selectedMechanicalComponentCount);
   m_selectedToolingCountStatic.setCount(selectedFiducialCount);
   m_selectedFiducialCountStatic.setCount(selectedDrillToolCount);

   int topPadCount,bottomPadCount,bothPadCount,neitherPadCount;
   m_pinList.getPadCounts(topPadCount,bottomPadCount,bothPadCount,neitherPadCount);

   int augmentedPinCount = getGerberEducator().augmentPins(m_pinList);
   m_pinList.getPadCounts(topPadCount,bottomPadCount,bothPadCount,neitherPadCount);

   //int showCmd = ((augmentedPinCount == 0) ? SW_HIDE : SW_SHOW);
   //int showCmd = SW_SHOW;

   //m_mountGroup.ShowWindow(SW_SHOW);
   //m_smdMountButton.ShowWindow(SW_SHOW);
   //m_thMountButton.ShowWindow(SW_SHOW);

   int workingPadCount = (m_topViewFlag ? topPadCount    : bottomPadCount);
   int idlePadCount    = (m_topViewFlag ? bottomPadCount : topPadCount);

   bool thDetectedFlag    = (augmentedPinCount != 0 || bothPadCount != 0 || idlePadCount != 0);  
   bool pinsSelectedFlag  = (topPadCount > 0 || bottomPadCount > 0);
   bool allowIdleSurfaceSelectionFlag = getAllowIdleSurfaceSelectionFlag();

   if (m_enableSingleSurfaceThPins)
   {
      if (allowIdleSurfaceSelectionFlag && thDetectedFlag)
      {
         m_thMountFlag = true;
      }
      else if (m_lastUserThMount != boolUnknown)
      {
         m_thMountFlag = (m_lastUserThMount == boolTrue);
      }
      else
      {
         m_thMountFlag = thDetectedFlag;
      }

      m_lastUserThMount = boolUnknown;

      m_thMountButton.SetCheck(m_thMountFlag);
      m_smdMountButton.SetCheck(!m_thMountFlag);

      m_thMountButton.EnableWindow(true);
   }
   else
   {
      if (thDetectedFlag)
      {
         if (!m_thMountButton.IsWindowEnabled())
         {
            m_thMountFlag = m_lastThMountFlag;
         }

         m_lastThMountFlag = m_thMountFlag;
      }
      else
      {
         m_thMountFlag = false;
      }

      m_thMountButton.SetCheck(m_thMountFlag && pinsSelectedFlag);
      m_smdMountButton.SetCheck(!m_thMountFlag && pinsSelectedFlag);

      m_thMountButton.EnableWindow(thDetectedFlag);
   }

   if (thDetectedFlag && !m_thMountFlag && !allowIdleSurfaceSelectionFlag)
   {
      m_pinList.removeSurface(!m_topViewFlag);
   }

   m_pinList.getPadCounts(topPadCount,bottomPadCount,bothPadCount,neitherPadCount);

   int thPinCount = bothPadCount;
   int smPinCount = topPadCount + bottomPadCount - 2*thPinCount;

   m_selectedPinPadsCountStatic.setCount(smPinCount + thPinCount);
   m_derivedDrillsCountStatic.setCount(thPinCount + (m_thMountFlag ? smPinCount : 0));
   m_selectedCentroidsCountStatic.setCount(selectedUnmatchedCentroidCount);

   // set up layer filtering
   EducatorLayerFilterTag educatorLayerFilter = m_lastSelectedEducatorLayerFilter;

   if (illegalDataCount  == 0)
   {
      if (selectedComponentCount > 0)
      {
         if (selectedUnmatchedCentroidCount == 0 && selectedPinCount == 0)
         {
            educatorLayerFilter = educatorLayerFilterComponents;
         }
         else
         {
            educatorLayerFilter = educatorLayerFilterPinsCentroidsComponents;
         }
      }
      else if (selectedUnmatchedCentroidCount > 0)
      {
         if (selectedUnmatchedCentroidCount > 1)
         {
            educatorLayerFilter = educatorLayerFilterPinsCentroidsComponents;
         }
         else 
         {
            educatorLayerFilter = educatorLayerFilterPins;
         }
      }
      else if (selectedPinCount > 0)
      {
         educatorLayerFilter = educatorLayerFilterPinsCentroids;
      }
   }

   setLayerFilter(educatorLayerFilter);

   double rotationDegrees=0.;

   if (m_centroid != NULL)
   {
      if (m_centroidsLockedFlag || m_centroid != m_previousCentroid)
      {
         m_refDes     = getGerberEducator().getRefDes(*m_centroid);
         m_shape      = getGerberEducator().getShape(*m_centroid);
         m_partNumber = getGerberEducator().getPartNumber(*m_centroid);

         rotationDegrees = radiansToDegrees(m_centroid->getInsert()->getAngle());
      }
   }
   else if (component != NULL)
   {
      m_refDes     = getGerberEducator().getRefDes(*component);
      m_shape      = getGerberEducator().getShape(*component);
      m_partNumber = getGerberEducator().getPartNumber(*component);

      rotationDegrees = radiansToDegrees(component->getInsert()->getAngle());
   }
   else if (m_centroidsLockedFlag)
   {
      m_refDes = getGerberEducator().getRefDesMap().defineCeiling("GE_",QRefDesSuffixFormat);

      m_shape.Empty();
      m_partNumber.Empty();
   }

   m_rotationDegrees.Format("%.1f",rotationDegrees);

   if (m_status == selectListStatusOk)
   {
      if (selectedComponentCount > 0)
      {
         if (selectedPinCount == 0 && selectedUnmatchedCentroidCount == 0)
         {
            //if (selectedMatchedCentroidCount > 0)
            //{
            //   m_status = selectListStatusComponentsAndCentroids;
            //}
            //else
            //{
               m_status = selectListStatusComponents;
            //}
         }
         else
         {
            m_status = selectListStatusIllegalUnmatchedData;
         }
      }
      else
      {
         if (getEnableCentroidEditingFlag() && m_centroid == NULL)
         {
            if (pin1 == NULL)
            {
               m_status = selectListStatusNoPins;
            }       
            else if (pin1 != NULL && matchedLayer)
            {
               m_status = selectListStatusIllegalMatchedData;
            }       
            //else if (pin1 != NULL && unmatchedLayer && pin1->isSelected() == 0)
            //{
            //   m_status = selectListStatusNoPin1;
            //}
            else if (m_refDes.IsEmpty())
            {
               m_status = selectListStatusNoRefDes;
            }
            else if (getGerberEducator().getComponent(m_refDes) != NULL)
            {
               m_status = selectListStatusDuplicateRefDes;
            }
            else if (thFlag)
            {
               m_fhs = fpfmt(getCamCadDoc().convertToPageUnits(pageUnitsMils,15.));
               m_status = selectListStatusPadsFromBothLayers;

               //m_status = selectListStatusNoFhs;
               //m_fhs.Trim();

               //if (!m_fhs.IsEmpty())
               //{
               //   char* endPtr;

               //   double fhsValue = strtod(m_fhs,&endPtr);

               //   if (*endPtr == '\0')
               //   {
               //      m_status = selectListStatusPadsFromBothLayers;
               //   }
               //}
            }
         }
         else
         {
            //if (m_centroid == NULL)
            //{
            //   m_status = selectListStatusNoCentroid;
            //}
            //else
            if (pin1 == NULL && unmatchedLayer)
            {
               m_status = selectListStatusNoPins;
            }       
            else if (pin1 == NULL && matchedLayer)
            {
               m_status = selectListStatusMatchedCentroids;
            }       
            else if (pin1 != NULL && matchedLayer)
            {
               m_status = selectListStatusIllegalMatchedData;
            }       
            //else if (pin1 != NULL && unmatchedLayer && pin1->isSelected() == 0)
            //{
            //   m_status = selectListStatusNoPin1;
            //}
         }
      }
   }

   bool matchedCentroids = (m_status == selectListStatusMatchedCentroids ||
                            m_status == selectListStatusComponents       ||
                            m_status == selectListStatusComponentsAndCentroids );
   bool ok               = (m_status == selectListStatusOk || 
                            m_status == selectListStatusPadsFromBothLayers);
   bool centroidSelected = (m_centroid != NULL);

   m_addIndividualComponentButton.EnableWindow(ok);
   m_addComponentsByPatternButton.EnableWindow(ok);
   m_addComponentsByOrthogonalPatternButton.EnableWindow(ok);
   //m_addComponentsAutomaticallyByCentroidButton.EnableWindow(false);
   m_deleteIndividualComponentButton.EnableWindow(matchedCentroids);
   m_deleteComponentsByGeometryButton.EnableWindow(matchedCentroids);

   //m_allowIdleSurfaceSelectionButton.EnableWindow(m_thMountFlag);

   CString statusDescription("Status: ");
   CString description;
   COLORREF statusColor = colorRed;

   switch (m_status)
   {
   case selectListStatusNoCentroid:
      statusDescription += "No centroids are marked.";
      break;
   case selectListStatusMultipleCentroids:
      statusDescription += "Multiple centroids are marked and at least one of them is unmatched.";
      break;
   case selectListStatusNoPins:
      statusDescription += "At least one pin must be marked.";
      break;
   case selectListStatusMultipleLayers:
      statusDescription += "Marked centroids and pins must be on the same layer.";
      break;
   case selectListStatusNonEducatorLayer:
      statusDescription.AppendFormat("Marked centroids and pins must be on a \"%s\" layer.",
         getGerberEducator().getToolName());
      break;
   case selectListStatusIllegalData:
      statusDescription += "Only centroids, apertures, and components may be marked.";
      break;
   case selectListStatusIllegalUnmatchedData:
      statusDescription += "When adding components, only centroids and apertures may be marked.";
      break;
   case selectListStatusIllegalMatchedData:
      statusDescription += "Only centroids may be selected on matched layers.";
      break;
   case selectListStatusNoPin1:
      statusDescription += "Pin 1 must be selected.";
      statusColor = colorYellow;
      break;
   case selectListStatusNoRefDes:
      statusDescription += "A refDes must be specified.";
      break;
   case selectListStatusDuplicateRefDes:
      statusDescription += "Specified refDes already exists as a component.";
      statusColor = colorYellow;
      break;
   case selectListStatusMatchedCentroids:
      statusDescription += "One or more marked matched centroids.";
      statusColor = colorCyan;
      break;
   case selectListStatusComponents:
      statusDescription += "One or more components.";
      statusColor = colorCyan;
      break;
   case selectListStatusComponentsAndCentroids:
      statusDescription += "One or more components and marked matched centroids.";
      statusColor = colorCyan;
      break;
   case selectListStatusNoFhs:
      statusDescription += "A Fhs must be specified";
      break;
   case selectListStatusPadsFromBothLayers:
      statusDescription += "OK - Thru hole component";
      statusColor = colorGreen;
      break;
   case selectListStatusOk:
      statusDescription += "OK";
      statusColor = colorGreen;
      break;
   default:
      statusDescription += "Unknown";
      statusColor = colorYellow;
      break;
   }

   //m_generationStatusStatic.SetWindowText(statusDescription);
   //m_generationStatusStatic.SetTextColor(statusColor);

   CGerberEducatorProgressSummary& gerberEducatorProgressSummary = getGerberEducator().getGerberEducatorProgressSummary();

   m_gerberPadsTopCountStatic.setCount   (gerberEducatorProgressSummary.getPadCount(true ,false));
   m_gerberPadsBottomCountStatic.setCount(gerberEducatorProgressSummary.getPadCount(false,false));
   m_centroidsTopCountStatic.setCount    (gerberEducatorProgressSummary.getCentroidCount(true ,false));
   m_centroidsBottomCountStatic.setCount (gerberEducatorProgressSummary.getCentroidCount(false,false));
   m_componentsTopCountStatic.setCount   (gerberEducatorProgressSummary.getComponentCount(true ));
   m_componentsBottomCountStatic.setCount(gerberEducatorProgressSummary.getComponentCount(false));
   m_geometriesCountStatic.setCount      (gerberEducatorProgressSummary.getComponentGeometryCount());

   UpdateData(false);

   m_previousCentroid = m_centroid;
}

void CGerberEducatorCreateGeometryDialog::addPinToList(DataStruct* pin)
{
   bool topFlag = (pin->getLayerIndex() == getGerberEducator().getTopUnmatchedLayerIndex());

   CPinData* pinData = m_pinList.add(pin,topFlag);
}

void CGerberEducatorCreateGeometryDialog::restoreOriginalView()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL && view->GetDocument() == &(getCamCadDoc()))
   {
      CNamedViewList& namedViewList = getCamCadDoc().getNamedViewList();

      CNamedView* originalNamedView = namedViewList.getAt(QPreGerberEducator);

      if (originalNamedView != NULL)
      {
         getCamCadDoc().setLayerViewData(*originalNamedView);
         view->recallView(*originalNamedView);
      }
   }
}

bool CGerberEducatorCreateGeometryDialog::calcNamedView()
{
   bool retval = false;

   CCEtoODBView* view = getActiveView();

   if (view != NULL && view->GetDocument() == &(getCamCadDoc()))
   {
      m_namedView.init();
      view->getLayerViewData(m_namedView);

      //if (m_allLayerButton.GetCheck() != 0)
      //{
      //   m_namedView.showAll();
      //   view->recallView(m_namedView);
      //}
      //else if (m_originalLayerButton.GetCheck() != 0)
      //{
      //   restoreOriginalView();
      //}
      //else
      //{
         m_namedView.hideAll();

         bool workingSurfaceTopFlag = (m_topViewButton.GetCheck()             != 0);
         bool viewBothSurfacesFlag  = (m_displayBothSurfacesButton.GetCheck() != 0);
         bool displayTopFlag        =  workingSurfaceTopFlag || viewBothSurfacesFlag;
         bool displayBottomFlag     = !workingSurfaceTopFlag || viewBothSurfacesFlag;

         bool displayComponentsFlag = (m_displayComponentsButton.GetCheck()   != 0);
         bool displayPadsFlag       = (m_displayPadsButton.GetCheck()         != 0);
         bool displaySilkscreenFlag = (m_displaySilkscreenButton.GetCheck()   != 0);

         CGerberEducatorColorScheme layerColor;

         // board outline
         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorBoardOutline )->getLayerIndex(),colorYellow   ,true);

         // unmatched
         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorUnmatchedTop   )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesUnmatchedPadTopColor)    ,
            displayTopFlag && displayPadsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorUnmatchedBottom)->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesUnmatchedPadBottomColor),
            displayBottomFlag && displayPadsFlag );

         // unmatched centroid
         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorCentroidTop   )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesUnmatchedCentroidTopColor)    ,
            displayTopFlag && displayPadsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorCentroidBottom   )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesUnmatchedCentroidBottomColor)    ,
            displayBottomFlag && displayPadsFlag );

         // silkscreen
         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorSilkTop     )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesSilkscreenTopColor)   ,
            displayTopFlag && displaySilkscreenFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorSilkBottom  )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesSilkscreenBottomColor)   ,
            displayBottomFlag && displaySilkscreenFlag );


         // component
         COLORREF topComponentColor    = colorHtmlYellowGreen;
         COLORREF bottomComponentColor = colorOrange;

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerPadTop     )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesSmdPadTopColor)  ,
            displayTopFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerPadBottom  )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesSmdPadBottomColor) ,
            displayBottomFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerPasteTop   )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesSmdPadTopColor)  ,
            displayTopFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerPasteBottom)->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesSmdPadBottomColor) ,
            displayBottomFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerAssemblyTop)->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesComponentOutlineTopColor)  ,
            displayTopFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerAssemblyBottom  )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesComponentOutlineBottomColor) ,
            displayBottomFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerCentroidTop)->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesCentroidTopColor)  ,
            displayTopFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerCentroidBottom  )->getLayerIndex(),
            layerColor.getColor(gerberEducatorCreateGeometriesCentroidBottomColor) ,
            displayBottomFlag && displayComponentsFlag );

         m_namedView.setAt(getCamCadDatabase().getLayer(ccLayerDrillHoles  )->getLayerIndex(),colorWhite ,
            displayPadsFlag  );

         //view->recallView(namedView);
         retval = true;
      //}
   }

   return retval;
}

void CGerberEducatorCreateGeometryDialog::filterSelectList()
{
   CSelectList& selectList = getCamCadDoc().SelectList;
   CSelectList keepSelectList;
   bool selectBothSurfacesFlag      = getAllowIdleSurfaceSelectionFlag();
   bool topSurfaceSelectableFlag    = ( m_topViewFlag || selectBothSurfacesFlag);
   bool bottomSurfaceSelectableFlag = (!m_topViewFlag || selectBothSurfacesFlag);

   for (POSITION pos = selectList.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      SelectStruct* selectStruct = selectList.GetNext(pos);
      DataStruct* data = selectStruct->getData();
      bool selectFlag = false;

      while (true)
      {
         if (data->getDataType() != dataTypeInsert)
         {
            break;
         }

         InsertStruct* insert = data->getInsert();

         bool pcbComponentFlag = true;

         switch (insert->getInsertType())
         {
         case insertTypePcbComponent:
         case insertTypeMechanicalComponent:
         case insertTypeFiducial:
         case insertTypeDrillTool:
            pcbComponentFlag = true;
            break;
         default:
            pcbComponentFlag = false;
            break;
         }

         if (pcbComponentFlag)
         {
            selectFlag = (m_educatorLayerFilter == educatorLayerFilterEverything              ||
                          m_educatorLayerFilter == educatorLayerFilterPinsCentroidsComponents ||
                          m_educatorLayerFilter == educatorLayerFilterComponents                  );
            break;
         }

         BlockStruct* block   = getCamCadDoc().getBlockAt(insert->getBlockNumber());

         bool isCentroidFlag = (block->getBlockType() == blockTypeCentroid);
         bool isApertureFlag = block->isAperture();

         if (!isCentroidFlag && !isApertureFlag)
         {
            break;
         }

         bool dataOnTopUnmatchedLayer    = (data->getLayerIndex() == getGerberEducator().getTopUnmatchedLayerIndex());
         bool dataOnBottomUnmatchedLayer = (data->getLayerIndex() == getGerberEducator().getBottomUnmatchedLayerIndex());
         bool dataOnUnmatchedLayer       = (dataOnTopUnmatchedLayer || dataOnBottomUnmatchedLayer);

         if (!dataOnUnmatchedLayer)
         {
            break;
         }

         if (dataOnTopUnmatchedLayer    && !topSurfaceSelectableFlag    ||
             dataOnBottomUnmatchedLayer && !bottomSurfaceSelectableFlag    )
         {
            break;
         }

         if (isCentroidFlag)
         {
            selectFlag = (m_educatorLayerFilter == educatorLayerFilterEverything              ||
                          m_educatorLayerFilter == educatorLayerFilterPinsCentroidsComponents ||
                          m_educatorLayerFilter == educatorLayerFilterPinsCentroids              );
            break;
         }

         if (isApertureFlag)
         {
            selectFlag = (m_educatorLayerFilter == educatorLayerFilterEverything              ||
                          m_educatorLayerFilter == educatorLayerFilterPinsCentroidsComponents ||
                          m_educatorLayerFilter == educatorLayerFilterPins                    ||
                          m_educatorLayerFilter == educatorLayerFilterPinsCentroids              );
            break;
         }

         break;
      }

      if (selectFlag)
      {
         SelectStruct* keepSelectStruct = new SelectStruct(*selectStruct);
         keepSelectList.AddTail(keepSelectStruct);
      }
   }

   getCamCadDoc().UnselectAll(true);

   for (POSITION pos = keepSelectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* keepSelectStruct = keepSelectList.GetNext(pos);
      SelectStruct* newSelectStruct  = getCamCadDoc().InsertInSelectList(*keepSelectStruct,true,false);
      getCamCadDoc().DrawEntity(newSelectStruct,-3,false);
   }
}

void CGerberEducatorCreateGeometryDialog::updateLayers()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL && calcNamedView())
   {
      view->recallView(m_namedView);
   }
}

void CGerberEducatorCreateGeometryDialog::enableLayerRadioButtons(bool enableFlag)
{
   //m_unmatchedTopLayerButton.EnableWindow(enableFlag);
   //m_unmatchedBottomLayerButton.EnableWindow(enableFlag);
   //m_unmatchedBothLayerButton.EnableWindow(enableFlag);

   //m_SilkscreenTopLayerButton.EnableWindow(enableFlag);
   //m_SilkscreenBottomLayerButton.EnableWindow(enableFlag);
   //m_SilkscreenBothLayerButton.EnableWindow(enableFlag);

   //m_compPinTopLayerButton.EnableWindow(enableFlag);
   //m_compPinBottomLayerButton.EnableWindow(enableFlag);
   //m_compPinBothLayerButton.EnableWindow(enableFlag);
}

void CGerberEducatorCreateGeometryDialog::addComponentsByPatternMatching(bool orthogonalFlag)
{
   if (getOkToAdd())
   {
      //CString refDes("U"),shape,partNumber,rotation("0"),fhs("0");
      double rotationRadians = degreesToRadians(atof(m_rotationDegrees));

      //m_refDesControl.GetWindowText(refDes);
      //m_shapeControl.GetWindowText(shape);
      //m_partNumberControl.GetWindowText(partNumber);
      //m_rotationControl.GetWindowText(rotation);
      //m_fhsControl.GetWindowText(fhs);
      bool preferredCcwOrthoRotationFlag = true;

      double fhsValue = 10. * getGerberEducator().getSearchTolerance();
      int topPadCount,bottomPadCount;
      bool topFlag = true;

      m_pinList.getPadCounts(topPadCount,bottomPadCount);

      //if (topPadCount > 0)
      //{
      //   if (bottomPadCount > 0)
      //   {
      //      topFlag = m_topViewFlag;

      //      //if (!m_placeThTopFlag)
      //      //{
      //      //   m_pinList.swapSurfaces();
      //      //}
      //   }
      //}
      //else
      //{
      //   topFlag = false;
      //}

      topFlag = m_topViewFlag;

      if (! m_fhs.IsEmpty())
      {
         fhsValue = atof(m_fhs);
      }

      //if (!m_placeThTopFlag)
      //{
      //   m_pinList.swapSurfaces();
      //}

      getGerberEducator().addByPinPattern(m_refDes,m_shape,m_partNumber,topFlag,m_thMountFlag,fhsValue,
         rotationRadians,preferredCcwOrthoRotationFlag,m_pinList,m_centroid,orthogonalFlag);

      //CGerberEducatorRefDes refdes(refDes);
      //m_refDesControl.SetWindowText(getGerberEducator().getRefDesMap().queryNextUndefined(refDes));

      //getGerberEducator().getCamCadDoc().clearSelected(false,false);
      //getGerberEducator().getCamCadDoc().OnRedraw();
      getGerberEducator().drawRedrawList();
   }
}

void CGerberEducatorCreateGeometryDialog::updateViewSurface()
{
   m_topViewButton.SetCheck(m_topViewFlag);
   m_bottomViewButton.SetCheck(!m_topViewFlag);

   if (calcNamedView())
   {
      getCamCadDoc().setLayerViewData(m_namedView);
   }

   getGerberEducator().getCamCadDoc().FitPageKeepingZoom(!m_topViewFlag);
}

void CGerberEducatorCreateGeometryDialog::updateMount()
{
   m_thMountButton.SetCheck(m_thMountFlag);
   m_smdMountButton.SetCheck(!m_thMountFlag);

   update();
}

BEGIN_MESSAGE_MAP(CGerberEducatorCreateGeometryDialog, CGerberEducatorBaseDialog)
   ON_WM_CLOSE()
   ON_WM_ACTIVATE()

   // toolbar commands
   ON_COMMAND(IDC_MarkByWindow, OnBnClickedMarkByWindow)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindow, OnUpdateMarkByWindow)
   ON_COMMAND(IDC_MarkByWindowCross, OnBnClickedMarkByWindowCross)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindowCross, OnUpdateMarkByWindowCross)
   ON_COMMAND(IDC_ClearMarked, OnBnClickedClearMarked)
   ON_UPDATE_COMMAND_UI(IDC_ClearMarked, OnUpdateClearMarked)

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

   ON_BN_CLICKED(IDC_Repaint, OnBnClickedRepaint)
   ON_BN_CLICKED(IDC_AddIndividualComponent, OnBnClickedAddIndividualComponent)
   ON_BN_CLICKED(IDC_AddComponentsByPatternMatching, OnBnClickedAddComponentsByPatternMatching)
   //ON_BN_CLICKED(IDC_AddComponentsAutomatically, OnBnClickedAddComponentsAutomatically)
   ON_BN_CLICKED(IDC_DeleteIndividualComponent, OnBnClickedDeleteIndividualComponent)
   ON_BN_CLICKED(IDC_DeleteComponentsByGeometry, OnBnClickedDeleteComponentsByGeometry)
   //ON_BN_CLICKED(IDC_AddComponentsByOrthogonalPatternMatching, OnBnClickedAddComponentsByOrthogonalPatternMatching)
   ON_BN_CLICKED(IDC_ThMount, OnBnClickedThMount)
   ON_BN_CLICKED(IDC_SmdMount, OnBnClickedSmdMount)
   ON_BN_CLICKED(IDC_TopView, OnBnClickedTopView)
   ON_BN_CLICKED(IDC_BottomView, OnBnClickedBottomView)
   ON_WM_WINDOWPOSCHANGING()
   ON_BN_CLICKED(IDC_CreateComponents, OnBnClickedCreateComponents)
   ON_BN_CLICKED(IDC_DeleteComponents, OnBnClickedDeleteComponents)
   ON_BN_CLICKED(IDC_DisplayWorkingSurface, OnBnClickedDisplayWorkingSurface)
   ON_BN_CLICKED(IDC_DisplayBothSurfaces, OnBnClickedDisplayBothSurfaces)
   ON_BN_CLICKED(IDC_DisplayComponents, OnBnClickedDisplayComponents)
   ON_BN_CLICKED(IDC_DisplayPads, OnBnClickedDisplayPads)
   ON_BN_CLICKED(IDC_DisplaySilkscreen, OnBnClickedDisplaySilkScreen)
   ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
   ON_BN_CLICKED(IDC_AllowSelection, OnBnClickedAllowSelection)
   ON_BN_CLICKED(IDC_AddAutomatically, OnBnClickedAddAutomatically)
   ON_BN_CLICKED(IDC_GenerateCentroidLayers, OnBnClickedGenerateCentroidLayers)
   ON_STN_DBLCLK(IDC_GeometriesLabel, OnStnDblClickGeometriesLabel)
END_MESSAGE_MAP()

// CGerberEducatorCreateGeometryDialog message handlers

BOOL CGerberEducatorCreateGeometryDialog::OnInitDialog()
{
   CGerberEducatorBaseDialog::OnInitDialog();

   //m_toolBar.createAndLoad(IDR_GerberEducatorCreateGeometryToolbar);
   m_toolBar2.createAndLoad(IDR_GerberEducatorCreateGeometryToolbar2);
   m_toolBar1.createAndLoad(IDR_GerberEducatorCreateGeometryToolbar1);

   CGerberEducatorBaseDialog::initDialog();

   m_lastThMountFlag = true;

   m_addComponentsByOrthogonalPatternButton.ShowWindow(SW_HIDE);
   //m_addComponentsAutomaticallyByCentroidButton.ShowWindow(SW_HIDE);

   m_displayWorkingSurfaceButton.SetCheck(1);
   m_displayBothSurfacesButton.SetCheck(0);
   m_allowIdleSurfaceSelectionButton.SetCheck(0);
   m_allowIdleSurfaceSelectionButton.EnableWindow(false);

   m_displayComponentsButton.SetCheck(1);
   m_displayPadsButton.SetCheck(1);
   m_displaySilkscreenButton.SetCheck(1);

   const COLORREF headerBkColor = colorHtmlLightSkyBlue;
   const COLORREF cellBkColor   = colorWhite;

   // summary 
   m_dataTypeLabelStatic.SetBkColor(headerBkColor);
   m_topLabelStatic.SetBkColor(headerBkColor);
   m_bottomLabelStatic.SetBkColor(headerBkColor);

   m_gerberPadsLabelStatic.SetBkColor(cellBkColor);
   m_centroidsLabelStatic.SetBkColor(cellBkColor);
   m_componentsLabelStatic.SetBkColor(cellBkColor);
   m_geometriesLabelStatic.SetBkColor(cellBkColor);
   m_gerberPadsTopCountStatic.SetBkColor(cellBkColor);
   m_gerberPadsBottomCountStatic.SetBkColor(cellBkColor);
   m_centroidsTopCountStatic.SetBkColor(cellBkColor);
   m_centroidsBottomCountStatic.SetBkColor(cellBkColor);
   m_componentsTopCountStatic.SetBkColor(cellBkColor);
   m_componentsBottomCountStatic.SetBkColor(cellBkColor);
   m_geometriesCountStatic.SetBkColor(cellBkColor);

   m_selectedGerberPadsLabelStatic.SetBkColor(cellBkColor);
   m_selectedCentroidsLabelStatic.SetBkColor(cellBkColor);
   m_selectedDrillsLabelStatic.SetBkColor(cellBkColor);
   m_selectedPinPadsCountStatic.SetBkColor(cellBkColor);
   m_derivedDrillsCountStatic.SetBkColor(cellBkColor);
   m_selectedCentroidsCountStatic.SetBkColor(cellBkColor);

   m_pcbCompsLabelStatic.SetBkColor(cellBkColor);
   m_mechCompsLabelStatic.SetBkColor(cellBkColor);
   m_toolingLabelStatic.SetBkColor(cellBkColor);
   m_fiducialLabelStatic.SetBkColor(cellBkColor);
   m_selectedPcbComponentCountStatic.SetBkColor(cellBkColor);
   m_selectedMechComponentCountStatic.SetBkColor(cellBkColor);
   m_selectedToolingCountStatic.SetBkColor(cellBkColor);
   m_selectedFiducialCountStatic.SetBkColor(cellBkColor);

   // updates
   m_lastSelectedEducatorLayerFilter = m_defaultSelectedEducatorLayerFilter;
   getCamCadDoc().getInsertTypeFilter().setToDefault();
   getCamCadDoc().getInsertTypeFilter().remove(insertTypeVia);
   getCamCadDoc().getGraphicsClassFilter().setToDefault();

//#ifdef _RDEBUG
//      m_generateCentroidLayersButton.ShowWindow(SW_SHOW);
//      m_addAutomaticallyButton.ShowWindow(SW_SHOW);
//#else
      m_generateCentroidLayersButton.ShowWindow(SW_HIDE);
      m_addAutomaticallyButton.ShowWindow(SW_HIDE);
//#endif

   updateViewSurface();
   updateLayers();
   updateMount();
   update();
   //updateBaseNameFrom();
   //updatePlaceTh();

   FlushQueue();
   OnBnClickedZoomFull();

   setEnableResize(false);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CGerberEducatorCreateGeometryDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CGerberEducatorBaseDialog::OnActivate(nState, pWndOther, bMinimized);

   m_activeState = nState;
}

void CGerberEducatorCreateGeometryDialog::OnClose()
{
   saveWindowState();

   StopEditing();
   CGerberEducatorBaseDialog::OnClose();

   CGerberEducatorUi* gerberEducatorUi = getGerberEducatorUi();

	CGerberEducatorColorScheme layerColor;
	layerColor.setDefaultViewColor(getCamCadDatabase(), *gerberEducatorUi->getFileStruct());

   if (gerberEducatorUi != NULL)
   {
	   gerberEducatorUi->generateEvent(ID_GerberEducatorCommand_TerminateSession);
   }
   else
   {
      getActiveView()->PostMessage(WM_COMMAND,ID_TerminateGerberEducator,0);
   }

   EndDialog(IDCANCEL);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDisplayWorkingSurface()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_displayWorkingSurfaceButton.SetCheck(1);
      m_displayBothSurfacesButton.SetCheck(0);
      m_allowIdleSurfaceSelectionButton.EnableWindow(false);

      updateLayers();
      setLayerFilter(m_lastSelectedEducatorLayerFilter);
      filterSelectList();
      update();
   }
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDisplayBothSurfaces()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_displayWorkingSurfaceButton.SetCheck(0);
      m_displayBothSurfacesButton.SetCheck(1);
      m_allowIdleSurfaceSelectionButton.EnableWindow(true);

      updateLayers();
      setLayerFilter(m_lastSelectedEducatorLayerFilter);
      filterSelectList();
      update();
   }
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedAllowSelection()
{
   setLayerFilter(m_lastSelectedEducatorLayerFilter);
   filterSelectList();
   update();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDisplayComponents()
{
      updateLayers();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDisplayPads()
{
      updateLayers();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDisplaySilkScreen()
{
      updateLayers();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedThMount()
{
   if (m_activeState != WA_INACTIVE)
   {
      if (m_thMountButton.GetCheck() == 0)
      {
         m_thMountFlag = true;
      }

      m_lastUserThMount = boolTrue;

      updateMount();
   }
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedSmdMount()
{
   if (m_activeState != WA_INACTIVE)
   {
      if (m_smdMountButton.GetCheck() == 0)
      {
         m_thMountFlag = false;
      }

      m_lastUserThMount = boolFalse;

      updateMount();
   }
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedTopView()
{
   if (m_activeState != WA_INACTIVE)
   {
      if (m_topViewButton.GetCheck() == 0)
      {
         m_topViewFlag = true;
      }

      //getCamCadDoc().OnClearSelected();
      updateViewSurface();
      updateLayers();
      update();
   }
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedBottomView()
{
   if (m_activeState != WA_INACTIVE)
   {
      if (m_bottomViewButton.GetCheck() == 0)
      {
         m_topViewFlag = false;
      }

      //getCamCadDoc().OnClearSelected();
      updateViewSurface();
      updateLayers();
      update();
   }
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedMarkByWindow()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      //addSelectFilter();

      // CCEtoODBView::OnMarkRect()
      view->PostMessage(WM_COMMAND,ID_QUERY_MARKRECT);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateMarkByWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedMarkByWindowCross()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      // CCEtoODBView::OnMarkRect_CrossCheck()
      view->PostMessage(WM_COMMAND,ID_QUERY_MARK_CROSSCHECK);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateMarkByWindowCross(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedClearMarked()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_QUERY_CLEAR_SELECTED);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateClearMarked(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedZoomIn()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMIN);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedZoomOut()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMOUT);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedZoomWindow()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMWINDOW);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateZoomWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedZoomExtents()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMEXTENTS);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateZoomExtents(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedZoomFull()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_ZOOM_ZOOM11);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdateZoomFull(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedPanCenter()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_PAN);
   }
}

void CGerberEducatorCreateGeometryDialog::OnUpdatePanCenter(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedQuery()
{
   getCamCadDoc().OnEditEntity();
}

void CGerberEducatorCreateGeometryDialog::OnUpdateQuery(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedRepaint()
{
   getGerberEducator().getCamCadDoc().OnRedraw();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedAddIndividualComponent()
{
   if (getOkToAdd())
   {
      double rotationRadians = degreesToRadians(atof(m_rotationDegrees));

      bool preferredCcwOrthoRotationFlag = true;

      double fhsValue = 10. * getGerberEducator().getSearchTolerance();
      int topPadCount,bottomPadCount;
      bool topFlag = true;

      m_pinList.getPadCounts(topPadCount,bottomPadCount);

      //if (topPadCount > 0)
      //{
      //   if (bottomPadCount > 0)
      //   {
      //      topFlag = m_topViewFlag;
      //   }
      //}
      //else
      //{
      //   topFlag = false;
      //}

      topFlag = m_topViewFlag;

      if (! m_fhs.IsEmpty())
      {
         fhsValue = atof(m_fhs);
      }

      if (m_pinList.GetCount() > 0)
      {
         DataStruct* component = getGerberEducator().addComponent(m_refDes,m_shape,m_partNumber,fhsValue,topFlag,m_thMountFlag,
                                                        m_pinList,m_centroid,gerberEducationMethodByPinPattern);
      }

      //getGerberEducator().getCamCadDoc().OnClearSelected();
      getGerberEducator().drawRedrawList();
   }

   update();
}

//void CGerberEducatorCreateGeometryDialog::OnBnClickedAddComponentsByOrthogonalPatternMatching()
//{
//   addComponentsByPatternMatching(true);
//   update();
//}

void CGerberEducatorCreateGeometryDialog::OnBnClickedAddComponentsByPatternMatching()
{
   addComponentsByPatternMatching(false);
   update();
}

//void CGerberEducatorCreateGeometryDialog::OnBnClickedAddComponentsAutomatically()
//{
//   // TODO: Add your control notification handler code here
//   update();
//}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDeleteIndividualComponent()
{
   getGerberEducator().deleteComponents(true,false,false,false);

   m_lastSelectedEducatorLayerFilter = educatorLayerFilterPinsCentroids;

   update();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDeleteComponentsByGeometry()
{
   getGerberEducator().deleteComponents(false,false,false,true);

   m_lastSelectedEducatorLayerFilter = educatorLayerFilterPinsCentroids;

   update();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedCreateComponents()
{
   int checked = m_createComponentsButton.GetCheck();

   if (m_lastSelectedEducatorLayerFilter == educatorLayerFilterPinsCentroids)
   {
      m_lastSelectedEducatorLayerFilter = m_defaultSelectedEducatorLayerFilter;
   }
   else
   {
      m_lastSelectedEducatorLayerFilter = educatorLayerFilterPinsCentroids;
   }

   setLayerFilter(m_lastSelectedEducatorLayerFilter);
   filterSelectList();
   update();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedDeleteComponents()
{
   int checked = m_deleteComponentsButton.GetCheck();

   if (m_lastSelectedEducatorLayerFilter == educatorLayerFilterComponents)
   {
      m_lastSelectedEducatorLayerFilter = m_defaultSelectedEducatorLayerFilter;
   }
   else
   {
      m_lastSelectedEducatorLayerFilter = educatorLayerFilterComponents;
   }

   setLayerFilter(m_lastSelectedEducatorLayerFilter);
   filterSelectList();
   update();
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedAddAutomatically()
{
   getGerberEducator().addAutomatically(gerberEducatorPackageIdentifierPartNumber);
   // TODO: Add your control notification handler code here
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedGenerateCentroidLayers()
{
   FileStruct* pcbFile = getCamCadDatabase().getSingleVisiblePcb();

   if (pcbFile != NULL)
   {
      LayerStruct* topCentroidLayer    = getCamCadDatabase().getNewLayer("Top Component Centroid"   ," (%d)",false,layerTypeCentroidTop   ,false);
      LayerStruct* bottomCentroidLayer = getCamCadDatabase().getNewLayer("Bottom Component Centroid"," (%d)",false,layerTypeCentroidBottom,false);
      BlockStruct* centroidGeometry    = getCamCadDatabase().getDefinedCentroidGeometry(pcbFile->getFileNumber());

      BlockStruct* pcbBlock = pcbFile->getBlock();
      CInsertTypeMask componentTypeMask(insertTypePcbComponent,insertTypeFiducial,insertTypeMechanicalComponent,insertTypeDrillTool);
      CDataList centroidList(true);

      for (CDataListIterator componentIterator(*pcbBlock,componentTypeMask);componentIterator.hasNext();)
      {
         DataStruct* component = componentIterator.getNext();
         BlockStruct* componentGeometry = getCamCadDatabase().getBlock(component->getInsert()->getBlockNumber());

         DataStruct* componentCentroid = getGerberEducator().getSingleComponentCentroid(*componentGeometry);

         if (componentCentroid != NULL)
         {
            CTMatrix matrix = component->getInsert()->getTMatrix();
            int layerIndex = (component->getInsert()->getPlacedTop() ? topCentroidLayer->getLayerIndex() : bottomCentroidLayer->getLayerIndex());
            DataStruct* centroid = getCamCadDatabase().insertBlock(centroidGeometry,insertTypeCentroid,"",layerIndex);

            CBasesVector basesVector;
            basesVector.transform(matrix);
            centroid->getInsert()->setBasesVector(basesVector);

            CString refDes     = getGerberEducator().getRefDes(*component);
            CString partNumber = getGerberEducator().getPartNumber(*component);

            getCamCadDatabase().addAttribute(centroid->attributes(),getGerberEducator().getRefDesKeywordIndex()    ,refDes         );
            getCamCadDatabase().addAttribute(centroid->attributes(),getGerberEducator().getPartNumberKeywordIndex(),partNumber     );

            centroidList.AddTail(centroid);
         }
      }

      pcbBlock->getDataList().takeData(centroidList);
   }
}

void CGerberEducatorCreateGeometryDialog::OnBnClickedClose()
{
   OnClose();
}

void CGerberEducatorCreateGeometryDialog::OnStnDblClickGeometriesLabel()
{
   m_generateCentroidLayersButton.ShowWindow(SW_SHOW);
   m_addAutomaticallyButton.ShowWindow(SW_SHOW);

   CGerberEducatorColorsDialog dialog;
   dialog.DoModal();
}

void CGerberEducatorCreateGeometryDialog::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
   //lpwndpos->flags &= ~SWP_HIDEWINDOW;

   //CGerberEducatorBaseDialog::OnWindowPosChanging(lpwndpos);
}

//_____________________________________________________________________________
CGerberEducatorDialog::CGerberEducatorDialog(CGerberEducator& gerberEducator)
: CGerberEducatorBaseDialog(CGerberEducatorDialog::IDD,gerberEducator)
, m_pinList(CGerberEducator::getSearchTolerance())
, m_toolBar(*this)
{
   m_status = selectListStatusUndefined;

   addFieldControl(IDC_GenerationStatus,anchorLeft,growHorizontal);
   addFieldControl(IDC_PinList         ,anchorLeft,growBoth      );
   addFieldControl(IDC_HideButton      ,anchorBottomRight        );
   
   //addFieldControl(IDOK                ,anchorBottom);
   //addFieldControl(IDCANCEL            ,anchorBottom);

   m_centroid                    = NULL;
   m_previousCentroid            = NULL;

   m_centroidsLockedFlag           = !getEnableCentroidEditingFlag();
   m_preferredCcwOrthoRotationFlag = true;
   m_placeThTopFlag                = true;

   m_toolBarRepaintIndex         =
   m_toolBarMarkByRectangleIndex =
   m_toolBarLockCentroidsIndex   =
   m_toolBarDeleteCentroidIndex  =
   m_toolBarNextMatchIndex       =
   m_toolBarPrevMatchIndex       = -1;

   m_activeState                 = WA_INACTIVE;
   m_baseNameFrom                = baseNameFromShape;
   m_rotationDegrees             = 0.;
}

CGerberEducatorDialog::~CGerberEducatorDialog()
{
}

int CGerberEducatorDialog::getToolBarIndex(int id)
{
   int* retval = NULL;

   switch (id)
   {
   case IDC_Repaint:         retval = &m_toolBarRepaintIndex;          break;
   case IDC_MarkByWindow:    retval = &m_toolBarMarkByRectangleIndex;  break;
   case IDC_LockCentroids:   retval = &m_toolBarLockCentroidsIndex;    break;
   case IDC_DeleteCentroid:  retval = &m_toolBarDeleteCentroidIndex;   break;
   case IDC_NextMatch:       retval = &m_toolBarNextMatchIndex;        break;
   case IDC_PrevMatch:       retval = &m_toolBarPrevMatchIndex;        break;
   }

   if (retval == NULL)
   {
      return -1;
   }

   if (*retval < 0)
   {
      *retval = m_toolBar.CommandToIndex(id);
   }

   return *retval;
}

bool CGerberEducatorDialog::getEnableCentroidEditingFlag() 
{ 
   return getGerberEducator().getEnableCentroidEditingFlag(); 
}

void CGerberEducatorDialog::DoDataExchange(CDataExchange* pDX)
{
   CGerberEducatorBaseDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_UpdateAttributes, m_updateAttributesButton);
   DDX_Control(pDX, IDC_AddByRefDes, m_addByRefDesButton);
   DDX_Control(pDX, IDC_AddByShape, m_addByShapeButton);
   DDX_Control(pDX, IDC_AddByPartNumber, m_addByPartNumberButton);
   DDX_Control(pDX, IDC_AddByPinPattern, m_addByPatternButton);
   DDX_Control(pDX, IDC_DeleteByRefDes, m_deleteByRefDesButton);
   DDX_Control(pDX, IDC_DeleteByShape, m_deleteByShapeButton);
   DDX_Control(pDX, IDC_DeleteByPartNumber, m_deleteByPartNumberButton);
   DDX_Control(pDX, IDC_DeleteByGeometry, m_deleteByGeometryButton);
   DDX_Control(pDX, IDC_GenerationStatus, m_generationStatusStatic);
   DDX_Control(pDX, IDC_RefDes, m_refDesControl);
   DDX_Control(pDX, IDC_SHAPE, m_shapeControl);
   DDX_Control(pDX, IDC_PartNumber, m_partNumberControl);
   DDX_Control(pDX, IDC_PinList, m_pinsListControl);
   DDX_Control(pDX, IDC_UnmatchedTop, m_unmatchedTopLayerButton);
   DDX_Control(pDX, IDC_UnmatchedBottom, m_unmatchedBottomLayerButton);
   DDX_Control(pDX, IDC_UnmatchedBoth, m_unmatchedBothLayerButton);
   DDX_Control(pDX, IDC_MatchedTop, m_matchedTopLayerButton);
   DDX_Control(pDX, IDC_MatchedBottom, m_matchedBottomLayerButton);
   DDX_Control(pDX, IDC_MatchedBoth, m_matchedBothLayerButton);
   DDX_Control(pDX, IDC_CompPinTop, m_compPinTopLayerButton);
   DDX_Control(pDX, IDC_CompPinBottom, m_compPinBottomLayerButton);
   DDX_Control(pDX, IDC_CompPinBoth, m_compPinBothLayerButton);
   DDX_Control(pDX, IDC_ALL, m_allLayerButton);
   DDX_Control(pDX, IDC_Original, m_originalLayerButton);
   DDX_Control(pDX, IDC_AddAutomatically, m_addAutomaticallyButton);

   DDX_Control(pDX, IDC_ROTATION, m_rotationControl);
   DDX_Control(pDX, IDC_Fhs, m_fhsControl);
   DDX_Control(pDX, IDC_RotationSpinner, m_rotationSpinControl);
   DDX_Control(pDX, IDC_HideButton, m_hideButton);
   DDX_Control(pDX, IDC_OrthoRotationCcw, m_orthoRotationCcwButton);
   DDX_Control(pDX, IDC_OrthoRotationCw, m_orthoRotationCwButton);
   DDX_Control(pDX, IDC_RefDesSpinner, m_refDesSpinControl);
}

SelectListStatusTag CGerberEducatorDialog::setFencedStatus(SelectListStatusTag status)
{
   if (m_status == selectListStatusOk)
   {
      m_status = status;
   }

   return m_status;
}

bool CGerberEducatorDialog::getOkToAdd()
{
   bool okToAdd = (m_status == selectListStatusOk) ||
                  (m_status == selectListStatusPadsFromBothLayers);

   return okToAdd;
}

void CGerberEducatorDialog::update()
{
   UpdateData(true);

   CSelectList& selectList = getCamCadDoc().SelectList;
   m_pinsListControl.DeleteAllItems();
   m_pinList.empty();

   m_centroid = NULL;
   DataStruct* component = NULL;
   DataStruct* pin1 = NULL;
   m_status = selectListStatusOk;
   int sourceLayerIndex = -1;
   bool matchedLayer   = false;
   bool unmatchedLayer = false;
   bool thFlag         = false;
   int selectedMatchedCentroidCount = 0;
   int selectedUnmatchedCentroidCount = 0;
   int selectedComponentCount = 0;
   int selectedPinCount = 0;
   ComponentAttributeStatus componentAttributeStatus(*this);

   // one component selected - edit component and centroid attributes
   // one centroid selected  - edit component and centroid attributes

   for (POSITION pos = selectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* selectStruct = selectList.GetNext(pos);
      DataStruct* data = selectStruct->getData();

      if (data->getDataType() != T_INSERT)
      {
         setFencedStatus(selectListStatusIllegalData);
         continue;
      }

      if (data->getInsert()->getInsertType() == insertTypePcbComponent)
      {
         selectedComponentCount++;
         componentAttributeStatus.update(data);

         if (component == NULL)
         {
            component = data;
         }

         continue;
      }

      InsertStruct* insert = data->getInsert();
      BlockStruct* block = getCamCadDoc().getBlockAt(insert->getBlockNumber());

      bool isCentroidFlag = (block->getBlockType() == BLOCKTYPE_CENTROID);
      bool isApertureFlag = block->isAperture();

      if (!isCentroidFlag && !isApertureFlag)
      {
         setFencedStatus(selectListStatusIllegalData);
         continue;
      }

      bool dataOnMatchedLayer = (
             (data->getLayerIndex() == getGerberEducator().getTopMatchedLayerIndex())      ||
             (data->getLayerIndex() == getGerberEducator().getBottomMatchedLayerIndex())       );

      bool dataOnUnmatchedLayer = (
            (data->getLayerIndex() == getGerberEducator().getTopUnmatchedLayerIndex())    ||
            (data->getLayerIndex() == getGerberEducator().getBottomUnmatchedLayerIndex())   );

      bool dataOnEducatorLayer = (dataOnMatchedLayer || dataOnUnmatchedLayer);

      if (!dataOnEducatorLayer)
      {
         setFencedStatus(selectListStatusNonEducatorLayer);
         continue;
      }

      if (sourceLayerIndex == -1)
      {
         sourceLayerIndex = data->getLayerIndex();
         matchedLayer     = dataOnMatchedLayer;
         unmatchedLayer   = dataOnUnmatchedLayer;
      }
      else if (data->getLayerIndex() != sourceLayerIndex)
      {
         if (m_centroidsLockedFlag)
         {
            setFencedStatus(selectListStatusMultipleLayers);
         }
         else if (!isCentroidFlag)
         {
            thFlag = true;
         }
      }

      if (isCentroidFlag)
      {
         if (dataOnMatchedLayer) 
         {
            selectedMatchedCentroidCount++;
         }
         else 
         {
            selectedUnmatchedCentroidCount++;
            componentAttributeStatus.update(data);
         }

         if (m_centroid == NULL)
         {
            m_centroid = data;
         }

         if ((selectedUnmatchedCentroidCount > 0 && selectedMatchedCentroidCount > 0) ||
             (selectedUnmatchedCentroidCount > 1)  )
         {
            setFencedStatus(selectListStatusMultipleCentroids);
         }
      }
      else  // is aperture
      {
         if (pin1 == NULL)
         {
            pin1 = data;
         }

         addPinToList(data);
         selectedPinCount++;
      }
   }

   CString refDes,shape,partNumber,rotation,fhs;
   m_refDesControl.GetWindowText(refDes);
   m_shapeControl.GetWindowText(shape);
   m_partNumberControl.GetWindowText(partNumber);
   //m_rotationControl.GetWindowText(rotation);
   m_fhsControl.GetWindowText(fhs);

   rotation.Format("%.1f",m_rotationDegrees);

   if (m_centroid != NULL)
   {
      if (m_centroidsLockedFlag || m_centroid != m_previousCentroid)
      {
         refDes     = getGerberEducator().getRefDes(*m_centroid);
         shape      = getGerberEducator().getShape(*m_centroid);
         partNumber = getGerberEducator().getPartNumber(*m_centroid);

         m_rotationDegrees = radiansToDegrees(m_centroid->getInsert()->getAngle());
      }

      rotation.Format("%.1f",m_rotationDegrees);
   }
   else if (component != NULL)
   {
      refDes     = getGerberEducator().getRefDes(*component);
      shape      = getGerberEducator().getShape(*component);
      partNumber = getGerberEducator().getPartNumber(*component);

      m_rotationDegrees = radiansToDegrees(component->getInsert()->getAngle());

      rotation.Format("%.1f",m_rotationDegrees);
   }
   else if (m_centroidsLockedFlag)
   {
      refDes.Empty();
      shape.Empty();
      partNumber.Empty();
      rotation.Empty();
      fhs.Empty();
   }

   if (m_status == selectListStatusOk)
   {
      if (selectedComponentCount > 0)
      {
         if (selectedPinCount == 0 && selectedUnmatchedCentroidCount == 0)
         {
            if (selectedMatchedCentroidCount > 0)
            {
               m_status = selectListStatusComponentsAndCentroids;
            }
            else
            {
               m_status = selectListStatusComponents;
            }
         }
         else
         {
            m_status = selectListStatusIllegalUnmatchedData;
         }
      }
      else
      {
         if (getEnableCentroidEditingFlag() && m_centroid == NULL)
         {
            if (pin1 == NULL)
            {
               m_status = selectListStatusNoPins;
            }       
            else if (pin1 != NULL && matchedLayer)
            {
               m_status = selectListStatusIllegalMatchedData;
            }       
            else if (pin1 != NULL && unmatchedLayer && pin1->isSelected() == 0)
            {
               m_status = selectListStatusNoPin1;
            }
            else if (refDes.IsEmpty())
            {
               m_status = selectListStatusNoRefDes;
            }
            else if (getGerberEducator().getComponent(refDes) != NULL)
            {
               m_status = selectListStatusDuplicateRefDes;
            }
            else if (thFlag)
            {
               m_status = selectListStatusNoFhs;
               fhs.Trim();

               if (!fhs.IsEmpty())
               {
                  char* endPtr;

                  double fhsValue = strtod(fhs,&endPtr);

                  if (*endPtr == '\0')
                  {
                     m_status = selectListStatusPadsFromBothLayers;
                  }
               }
            }
         }
         else
         {
            if (m_centroid == NULL)
            {
               m_status = selectListStatusNoCentroid;
            }
            else if (pin1 == NULL && unmatchedLayer)
            {
               m_status = selectListStatusNoPins;
            }       
            else if (pin1 == NULL && matchedLayer)
            {
               m_status = selectListStatusMatchedCentroids;
            }       
            else if (pin1 != NULL && matchedLayer)
            {
               m_status = selectListStatusIllegalMatchedData;
            }       
            else if (pin1 != NULL && unmatchedLayer && pin1->isSelected() == 0)
            {
               m_status = selectListStatusNoPin1;
            }
         }
      }
   }

   m_refDesControl.SetWindowText(refDes);
   m_shapeControl.SetWindowText(shape);
   m_partNumberControl.SetWindowText(partNumber);
   m_rotationControl.SetWindowText(rotation);
   m_fhsControl.SetWindowText(fhs);

   bool matchedCentroids = (m_status == selectListStatusMatchedCentroids ||
                            m_status == selectListStatusComponents       ||
                            m_status == selectListStatusComponentsAndCentroids );
   bool ok               = (m_status == selectListStatusOk || 
                            m_status == selectListStatusPadsFromBothLayers);
   bool centroidSelected = (m_centroid != NULL);
   bool enableAttributeEditing = getEnableCentroidEditingFlag() && 
      componentAttributeStatus.getCount() > 0;

   m_addByRefDesButton.EnableWindow(ok);
   m_addByShapeButton.EnableWindow(centroidSelected && (matchedCentroids || ok));
   m_addByPartNumberButton.EnableWindow(centroidSelected && (matchedCentroids || ok));
   m_addByPatternButton.EnableWindow(!centroidSelected && (matchedCentroids || ok));
   m_deleteByRefDesButton.EnableWindow(matchedCentroids);
   m_deleteByShapeButton.EnableWindow(matchedCentroids);
   m_deleteByPartNumberButton.EnableWindow(matchedCentroids);
   m_deleteByGeometryButton.EnableWindow(matchedCentroids);
   m_updateAttributesButton.EnableWindow(enableAttributeEditing);
   m_addAutomaticallyButton.EnableWindow(selectList.GetCount() == 0);

   CString statusDescription("Status: ");
   CString description;
   COLORREF statusColor = colorRed;

   switch (m_status)
   {
   case selectListStatusNoCentroid:
      statusDescription += "No centroids are marked.";
      break;
   case selectListStatusMultipleCentroids:
      statusDescription += "Multiple centroids are marked and at least one of them is unmatched.";
      break;
   case selectListStatusNoPins:
      statusDescription += "At least one pin must be marked.";
      break;
   case selectListStatusMultipleLayers:
      statusDescription += "Marked centroids and pins must be on the same layer.";
      break;
   case selectListStatusNonEducatorLayer:
      description.Format("Marked centroids and pins must be on a \"%s\" layer.",
         getGerberEducator().getToolName());

      statusDescription += description;
      break;
   case selectListStatusIllegalData:
      statusDescription += "Only centroids, apertures, and components may be marked.";
      break;
   case selectListStatusIllegalUnmatchedData:
      statusDescription += "When adding components, only centroids and apertures may be marked.";
      break;
   case selectListStatusIllegalMatchedData:
      statusDescription += "Only centroids may be selected on matched layers.";
      break;
   case selectListStatusNoPin1:
      statusDescription += "Pin 1 must be selected.";
      statusColor = colorYellow;
      break;
   case selectListStatusNoRefDes:
      statusDescription += "A refDes must be specified.";
      break;
   case selectListStatusDuplicateRefDes:
      statusDescription += "Specified refDes already exists as a component.";
      statusColor = colorYellow;
      break;
   case selectListStatusMatchedCentroids:
      statusDescription += "One or more marked matched centroids.";
      statusColor = colorCyan;
      break;
   case selectListStatusComponents:
      statusDescription += "One or more components.";
      statusColor = colorCyan;
      break;
   case selectListStatusComponentsAndCentroids:
      statusDescription += "One or more components and marked matched centroids.";
      statusColor = colorCyan;
      break;
   case selectListStatusNoFhs:
      statusDescription += "A Fhs must be specified";
      break;
   case selectListStatusPadsFromBothLayers:
      statusDescription += "OK - Thru hole component";
      statusColor = colorGreen;
      break;
   case selectListStatusOk:
      statusDescription += "OK";
      statusColor = colorGreen;
      break;
   default:
      statusDescription += "Unknown";
      statusColor = colorYellow;
      break;
   }

   m_generationStatusStatic.SetWindowText(statusDescription);
   m_generationStatusStatic.SetTextColor(statusColor);

   COLORREF faceColor     = GetSysColor(COLOR_3DFACE);
   COLORREF grayTextColor = GetSysColor(COLOR_GRAYTEXT);   

   m_refDesControl.setBackGroundColor(m_centroidsLockedFlag ? faceColor : colorWhite);
   m_refDesControl.setTextColor(enableAttributeEditing && 
      !componentAttributeStatus.getSameRefDesFlag() ? grayTextColor : colorBlack);
   m_refDesControl.SetReadOnly(m_centroidsLockedFlag);

   m_shapeControl.setBackGroundColor(m_centroidsLockedFlag ? faceColor : colorWhite);
   m_shapeControl.setTextColor(enableAttributeEditing && 
      !componentAttributeStatus.getSameShapeFlag() ? grayTextColor : colorBlack);
   m_shapeControl.SetReadOnly(m_centroidsLockedFlag);

   m_partNumberControl.setBackGroundColor(m_centroidsLockedFlag ? faceColor : colorWhite);
   m_partNumberControl.setTextColor(enableAttributeEditing && 
      !componentAttributeStatus.getSamePartNumberFlag() ? grayTextColor : colorBlack);
   m_partNumberControl.SetReadOnly(m_centroidsLockedFlag);

   m_fhsControl.setBackGroundColor(m_centroidsLockedFlag ? faceColor : colorWhite);
   m_fhsControl.SetReadOnly(m_centroidsLockedFlag);

   m_rotationControl.setBackGroundColor(m_centroidsLockedFlag ? faceColor : colorWhite);
   m_rotationControl.setTextColor(enableAttributeEditing && 
      !componentAttributeStatus.getSameRotationFlag() ? grayTextColor : colorBlack);
   m_rotationControl.SetReadOnly(m_centroidsLockedFlag);

   m_rotationSpinControl.ShowWindow(m_centroidsLockedFlag ? SW_HIDE : SW_SHOW);
   m_refDesSpinControl.ShowWindow(m_centroidsLockedFlag ? SW_HIDE : SW_SHOW);
   m_orthoRotationCcwButton.ShowWindow(!m_centroidsLockedFlag && m_preferredCcwOrthoRotationFlag ? 
      SW_SHOW : SW_HIDE);
   m_orthoRotationCwButton.ShowWindow(!m_centroidsLockedFlag && !m_preferredCcwOrthoRotationFlag ? 
      SW_SHOW : SW_HIDE);

   m_toolBar.GetToolBarCtrl().PressButton(IDC_LockCentroids,m_centroidsLockedFlag);

   UpdateData(false);

   m_previousCentroid = m_centroid;
}

void CGerberEducatorDialog::addPinToList(DataStruct* pin)
{
   bool topFlag = (pin->getLayerIndex() == getGerberEducator().getTopUnmatchedLayerIndex());

   CPinData* pinData = m_pinList.add(pin,topFlag);

   if (pinData != NULL)
   {
      CString thIndicator;
      
      if (pinData->isTh())
      {
         thIndicator = "th";
      }
      else if (pinData->getBottomPad() != NULL && m_placeThTopFlag)
      {
         thIndicator = "b";
      }
      else if (pinData->getTopPad() != NULL && !m_placeThTopFlag)
      {
         thIndicator = "t";
      }

      LVFINDINFO findInfo;
      findInfo.flags = LVFI_PARAM;
      findInfo.lParam = (LPARAM)pinData;
      findInfo.vkDirection = VK_UP;

      int itemIndex = m_pinsListControl.FindItem(&findInfo,-1);

      if (itemIndex != -1)
      {
         m_pinsListControl.SetItemText(itemIndex,1,thIndicator);
      }
      else
      {
         int pinCount  = m_pinsListControl.GetItemCount();
         int pinNumber = pinCount + 1;
         CPoint2d origin = pinData->getOrigin();
         CPoint2d d;

         if (pinCount > 0)
         {
            CPinData* prevPinData = (CPinData*)m_pinsListControl.GetItemData(pinCount - 1);

            d = origin - prevPinData->getOrigin();
         }

         PageUnitsTag units = getGerberEducator().getCamCadDatabase().getPageUnits();
         CString value;

         value.Format("%d",pinNumber);

         m_pinsListControl.InsertItem(pinCount,value);
         m_pinsListControl.SetItemText(pinCount,1,thIndicator);
         m_pinsListControl.SetItemText(pinCount,2,formatUnits(origin.x,units));
         m_pinsListControl.SetItemText(pinCount,3,formatUnits(origin.y,units));
         m_pinsListControl.SetItemText(pinCount,4,formatUnits(d.x     ,units));
         m_pinsListControl.SetItemText(pinCount,5,formatUnits(d.y     ,units));
         m_pinsListControl.SetItemData(pinCount,(DWORD_PTR)pinData);
      }
   }
}

void CGerberEducatorDialog::addByRefDes(GerberEducationMethodTag educationMethod)
{
   if (getOkToAdd() && m_centroid != NULL)
   {
      getGerberEducator().addComponentByCentroid(*m_centroid,m_pinList,false,educationMethod);
      //getGerberEducator().getCamCadDoc().OnClearSelected();
   }
}

void CGerberEducatorDialog::updateLayers()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL && view->GetDocument() == &(getCamCadDoc()))
   {
      CNamedView namedView;
      view->getLayerViewData(namedView);

      if (m_allLayerButton.GetCheck() != 0)
      {
         namedView.showAll();
         view->recallView(namedView);
      }
      else if (m_allLayerButton.GetCheck() != 0)
      {
         CNamedViewList& namedViewList = getCamCadDoc().getNamedViewList();

         CNamedView* originalNamedView = namedViewList.getAt(QPreGerberEducator);

         if (originalNamedView != NULL)
         {
            getCamCadDoc().setLayerViewData(*originalNamedView);
            view->recallView(*originalNamedView);
         }
      }
      else
      {
         namedView.hideAll();

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorUnmatchedTop   )->getLayerIndex(),colorRed    ,
            (m_unmatchedTopLayerButton.GetCheck() != 0) || (m_unmatchedBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorUnmatchedBottom)->getLayerIndex(),colorMagenta,
            (m_unmatchedBottomLayerButton.GetCheck() != 0) || (m_unmatchedBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorMatchedTop     )->getLayerIndex(),colorBlue   ,
            (m_matchedTopLayerButton.GetCheck() != 0) || (m_matchedBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorMatchedBottom  )->getLayerIndex(),colorCyan   ,
            (m_matchedBottomLayerButton.GetCheck() != 0) || (m_matchedBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerPadTop     )->getLayerIndex(),colorYellow  ,
            (m_compPinTopLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerPadBottom  )->getLayerIndex(),colorOrange ,
            (m_compPinBottomLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerPasteTop   )->getLayerIndex(),colorYellow  ,
            (m_compPinTopLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerPasteBottom)->getLayerIndex(),colorOrange ,
            (m_compPinBottomLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerAssemblyTop)->getLayerIndex(),colorYellow  ,
            (m_compPinTopLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerAssemblyBottom  )->getLayerIndex(),colorOrange ,
            (m_compPinBottomLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorCentroidTop)->getLayerIndex(),colorYellow  ,
            (m_compPinTopLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerEducatorCentroidBottom  )->getLayerIndex(),colorOrange ,
            (m_compPinBottomLayerButton.GetCheck() != 0) || (m_compPinBothLayerButton.GetCheck() != 0) );

         namedView.setAt(getCamCadDatabase().getLayer(ccLayerDrillHoles  )->getLayerIndex(),colorWhite ,
            (m_compPinTopLayerButton.GetCheck()    != 0) || 
            (m_compPinBottomLayerButton.GetCheck() != 0) || 
            (m_compPinBothLayerButton.GetCheck()   != 0)    );

         view->recallView(namedView);
      }
   }
}

void CGerberEducatorDialog::enableLayerRadioButtons(bool enableFlag)
{
   m_unmatchedTopLayerButton.EnableWindow(enableFlag);
   m_unmatchedBottomLayerButton.EnableWindow(enableFlag);
   m_unmatchedBothLayerButton.EnableWindow(enableFlag);
   m_matchedTopLayerButton.EnableWindow(enableFlag);
   m_matchedBottomLayerButton.EnableWindow(enableFlag);
   m_matchedBothLayerButton.EnableWindow(enableFlag);
   m_compPinTopLayerButton.EnableWindow(enableFlag);
   m_compPinBottomLayerButton.EnableWindow(enableFlag);
   m_compPinBothLayerButton.EnableWindow(enableFlag);
}

void CGerberEducatorDialog::updateBaseNameFrom()
{
   m_toolBar.GetToolBarCtrl().HideButton(IDC_PartNumberBaseName,m_baseNameFrom != baseNameFromPartNumber);
   m_toolBar.GetToolBarCtrl().HideButton(IDC_ShapeBaseName     ,m_baseNameFrom != baseNameFromShape);
}

void CGerberEducatorDialog::updatePlaceTh()
{
   m_toolBar.GetToolBarCtrl().HideButton(IDC_PlaceThTop   ,!m_placeThTopFlag);
   m_toolBar.GetToolBarCtrl().HideButton(IDC_PlaceThBottom, m_placeThTopFlag);
}

BEGIN_MESSAGE_MAP(CGerberEducatorDialog, CGerberEducatorBaseDialog)
   ON_BN_CLICKED(IDC_AddByRefDes, OnBnClickedAddByRefDes)
   ON_BN_CLICKED(IDC_AddByShape, OnBnClickedAddByShape)
   ON_BN_CLICKED(IDC_AddByPartNumber, OnBnClickedAddByPartNumber)
   ON_BN_CLICKED(IDC_DeleteByRefDes, OnBnClickedDeleteByRefDes)
   ON_BN_CLICKED(IDC_DeleteByShape, OnBnClickedDeleteByShape)
   ON_BN_CLICKED(IDC_DeleteByPartNumber, OnBnClickedDeleteByPartNumber)
   //ON_BN_CLICKED(IDOK, OnBnClickedOk)
   //ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
   ON_BN_CLICKED(IDC_UnmatchedTop, OnBnClickedUnmatchedTop)
   ON_BN_CLICKED(IDC_UnmatchedBottom, OnBnClickedUnmatchedBottom)
   ON_BN_CLICKED(IDC_UnmatchedBoth, OnBnClickedUnmatchedBoth)
   ON_BN_CLICKED(IDC_MatchedTop, OnBnClickedMatchedTop)
   ON_BN_CLICKED(IDC_MatchedBottom, OnBnClickedMatchedBottom)
   ON_BN_CLICKED(IDC_MatchedBoth, OnBnClickedMatchedBoth)
   ON_BN_CLICKED(IDC_CompPinTop, OnBnClickedCompPinTop)
   ON_BN_CLICKED(IDC_CompPinBottom, OnBnClickedCompPinBottom)
   ON_BN_CLICKED(IDC_CompPinBoth, OnBnClickedCompPinBoth)
   ON_WM_CLOSE()
   ON_BN_CLICKED(IDC_ALL, OnBnClickedAll)
   ON_BN_CLICKED(IDC_Original, OnBnClickedOriginal)
   ON_BN_CLICKED(IDC_AddAutomatically, OnBnClickedAddAutomatically)
   //ON_BN_CLICKED(IDC_MarkByWindow, OnBnClickedMarkByWindow)
   ON_BN_CLICKED(IDC_Repaint, OnBnClickedRepaint)
   ON_BN_CLICKED(IDC_LockCentroids, OnBnClickedLockCentroids)
   ON_BN_CLICKED(IDC_PartNumberBaseName, OnBnClickedPartNumberBaseName)
   ON_BN_CLICKED(IDC_ShapeBaseName, OnBnClickedShapeBaseName)
   ON_BN_CLICKED(IDC_PlaceThTop, OnBnClickedPlaceThTop)
   ON_BN_CLICKED(IDC_PlaceThBottom, OnBnClickedPlaceThBottom)
   ON_BN_CLICKED(IDC_HideButton, OnBnClickedHideButton)
   ON_NOTIFY(UDN_DELTAPOS, IDC_RotationSpinner, OnDeltaPosRotationSpinner)

   ON_COMMAND(IDC_MarkByWindow, OnBnClickedMarkByWindow)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindow, OnUpdateMarkByWindow)
   ON_COMMAND(IDC_MarkByWindowCross, OnBnClickedMarkByWindowCross)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindowCross, OnUpdateMarkByWindowCross)
   ON_COMMAND(IDC_ClearMarked, OnBnClickedClearMarked)
   ON_UPDATE_COMMAND_UI(IDC_ClearMarked, OnUpdateClearMarked)
   ON_WM_KILLFOCUS()
   ON_WM_ACTIVATE()
   ON_WM_SETFOCUS()
   ON_BN_CLICKED(IDC_DeleteByGeometry, OnBnClickedDeleteByGeometry)
   ON_BN_CLICKED(IDC_AddByPinPattern, OnBnClickedAddByPinPattern)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
   ON_EN_KILLFOCUS(IDC_RefDes, OnEnKillfocusRefDes)
   ON_EN_KILLFOCUS(IDC_SHAPE, OnEnKillfocusShape)
   ON_EN_KILLFOCUS(IDC_PartNumber, OnEnKillfocusPartNumber)
   ON_EN_KILLFOCUS(IDC_ROTATION, OnEnKillfocusRotation)
   ON_EN_KILLFOCUS(IDC_Fhs, OnEnKillfocusFhs)
   ON_BN_CLICKED(IDC_UpdateAttributes, OnBnClickedUpdateAttributes)
   ON_BN_CLICKED(IDC_OrthoRotationCcw, OnBnClickedOrthoRotationCcw)
   ON_BN_CLICKED(IDC_OrthoRotationCw, OnBnClickedOrthoRotationCw)
   ON_NOTIFY(UDN_DELTAPOS, IDC_RefDesSpinner, OnDeltaPosRefDesSpinner)
END_MESSAGE_MAP()


// CGerberEducatorDialog message handlers

BOOL CGerberEducatorDialog::OnInitDialog()
{
   CGerberEducatorBaseDialog::OnInitDialog();

   SetWindowText(getGerberEducator().getToolName());

   m_toolBar.createAndLoad(IDR_GerberEducatorToolbar);

   //// Create the Toolbar and attach the resource
   //if (m_toolBar.Create(this) != 0 && m_toolBar.LoadToolBar(IDR_GerberEducatorToolbar) != 0)
   //{
   //   m_toolBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY);

   //   CRect	rcClientOld; // Old Client Rect
   //   CRect	rcClientNew; // New Client Rect with Tollbar Added
   //   GetClientRect(rcClientOld); // Retrive the Old Client WindowSize

   //   // Called to reposition and resize control bars in the client 
   //   // area of a window. The reposQuery FLAG does not really draw the 
   //   // Toolbar.  It only does the calculations and puts the new 
   //   // ClientRect values in rcClientNew so we can do the rest of the 
   //   // Math.
   //   RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery,rcClientNew);

   //   // All of the Child Windows (Controls) now need to be moved so 
   //   // the Tollbar does not cover them up. Offest to move all child 
   //   // controls after adding Toolbar
   //   CPoint ptOffset(rcClientNew.left - rcClientOld.left,rcClientNew.top - rcClientOld.top);

   //   CRect	rcChild;

   //   HDWP hDwp = ::BeginDeferWindowPos(20);

   //   // Cycle through all child controls
   //   for (CWnd* pwndChild = GetWindow(GW_CHILD);pwndChild != NULL;) 
   //   {
   //      pwndChild->GetWindowRect(rcChild); // Get the child control RECT
   //      ScreenToClient(rcChild); 

   //      // Changes the Child Rect by the values of the calculated offset
   //      rcChild.OffsetRect(ptOffset); 
   //      //pwndChild->MoveWindow(rcChild,FALSE); // Move the Child Control
   //      hDwp = ::DeferWindowPos(hDwp,*pwndChild,0,rcChild.left,rcChild.top,rcChild.Width(),rcChild.Height(),SWP_NOZORDER);
   //      pwndChild = pwndChild->GetNextWindow();
   //   }

   //   ::EndDeferWindowPos(hDwp);

   //   CRect	rcWindow;
   //   GetWindowRect(rcWindow); // Get the RECT of the Dialog

   //   // Increase width to new Client Width
   //   rcWindow.right += rcClientOld.Width() - rcClientNew.Width(); 

   //   // Increase height to new Client Height
   //   rcWindow.bottom += rcClientOld.Height() - rcClientNew.Height(); 

   //   adjustSizeForToolBar(ptOffset.y);
   //   //CSize minMaxSize = getMinMaxSize();
   //   //minMaxSize.cy += ptOffset.y;
   //   //setMinMaxSize(minMaxSize);

   //   MoveWindow(rcWindow,FALSE); // Redraw Window

   //   // Now we REALLY Redraw the Toolbar
   //   RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

   //   //m_toolTips.Create(&m_toolBar,TTS_ALWAYSTIP );
   //   //m_toolTips.Activate(TRUE);
   //   //m_toolTips.AddTool(GetDlgItem(IDC_LockCentroids),"Lock Centroids");
   //}

   CGerberEducatorBaseDialog::initDialog();

   m_generationStatusStatic.SetBorder(true);
   m_generationStatusStatic.SetBkColor(colorBlack);
   m_generationStatusStatic.SetFontBold(true);

   GetDlgItem(IDOK)->ShowWindow(SW_HIDE);

//#if ! defined(_RDEBUG)
//   m_addAutomaticallyButton.ShowWindow(SW_HIDE);
//   //m_addByPatternButton.ShowWindow(SW_HIDE);
//#endif

   if (! getEnableCentroidEditingFlag())
   {
      m_toolBar.GetToolBarCtrl().HideButton(IDC_LockCentroids);
      m_toolBar.GetToolBarCtrl().HideButton(IDC_DeleteCentroid);
      m_toolBar.GetToolBarCtrl().HideButton(IDC_NextMatch);
      m_toolBar.GetToolBarCtrl().HideButton(IDC_PrevMatch);
   }

   // List control
   //long style = ::GetWindowLong(m_pinsListControl,GWL_STYLE);
   //style |= LVS_LIST;

   int column = 0;
   int width = 50;

   m_pinsListControl.InsertColumn(column,"#" ,LVCFMT_RIGHT,width/2,column);
   column++;

   m_pinsListControl.InsertColumn(column,"th" ,LVCFMT_RIGHT,width/2,column);
   column++;

   m_pinsListControl.InsertColumn(column,"x" ,LVCFMT_RIGHT,width,column);
   column++;

   m_pinsListControl.InsertColumn(column,"y" ,LVCFMT_RIGHT,width,column);
   column++;

   m_pinsListControl.InsertColumn(column,"dx",LVCFMT_RIGHT,width,column);
   column++;

   m_pinsListControl.InsertColumn(column,"dy",LVCFMT_RIGHT,width,column);
   column++;

   m_unmatchedBothLayerButton.SetCheck(1);
   m_matchedBothLayerButton.SetCheck(1);

   //m_repaintButton.SetIcon(AfxGetApp()->LoadIcon(IDI_Repaint));
   //m_markByWindowButton.SetIcon(AfxGetApp()->LoadIcon(IDI_MarkByRect));
   //m_lockCentroidsButton.SetIcon(AfxGetApp()->LoadIcon(IDI_Lock));

   //m_lockCentroidsButton.SetState(true);

   updateLayers();
   update();
   updateBaseNameFrom();
   updatePlaceTh();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CGerberEducatorDialog::OnBnClickedAddByRefDes()
{
   addByRefDes(gerberEducationMethodByRefDes);

   getGerberEducator().getCamCadDoc().OnRedraw();
}

void CGerberEducatorDialog::OnBnClickedAddByShape()
{
   if (m_centroid != NULL)
   {
      DataStruct& centroidTemplate = *m_centroid;

      if (getOkToAdd())
      {
         addByRefDes(gerberEducationMethodByShape);
      }

      getGerberEducator().addByShape(centroidTemplate);

      getGerberEducator().getCamCadDoc().OnRedraw();
   }
}

void CGerberEducatorDialog::OnBnClickedAddByPartNumber()
{
   if (m_centroid != NULL)
   {
      DataStruct& centroidTemplate = *m_centroid;

      if (getOkToAdd())
      {
         addByRefDes(gerberEducationMethodByPartNumber);
      }

      getGerberEducator().addByPartNumber(centroidTemplate);

      getGerberEducator().getCamCadDoc().OnRedraw();
   }
}

void CGerberEducatorDialog::OnBnClickedAddByPinPattern()
{
   if (getOkToAdd())
   {
      CString refDes,shape,partNumber,rotation,fhs;

      m_refDesControl.GetWindowText(refDes);
      m_shapeControl.GetWindowText(shape);
      m_partNumberControl.GetWindowText(partNumber);
      m_rotationControl.GetWindowText(rotation);
      m_fhsControl.GetWindowText(fhs);
      bool preferredCcwOrthoRotationFlag = true;

      double fhsValue = 10. * getGerberEducator().getSearchTolerance();
      int topPadCount,bottomPadCount;
      bool topFlag = true;

      m_pinList.getPadCounts(topPadCount,bottomPadCount);

      if (topPadCount > 0)
      {
         if (bottomPadCount > 0)
         {
            topFlag = m_placeThTopFlag;

            if (!m_placeThTopFlag)
            {
               m_pinList.swapSurfaces();
            }
         }
      }
      else
      {
         topFlag = false;
      }

      if (! fhs.IsEmpty())
      {
         fhsValue = atof(fhs);
      }

      if (!m_placeThTopFlag)
      {
         m_pinList.swapSurfaces();
      }

      getGerberEducator().addByPinPattern(refDes,shape,partNumber,topFlag,false,fhsValue,
         degreesToRadians(m_rotationDegrees),preferredCcwOrthoRotationFlag,m_pinList);

      CGerberEducatorRefDes refdes(refDes);
      m_refDesControl.SetWindowText(getGerberEducator().getRefDesMap().queryNextUndefined(refDes));

      //getGerberEducator().getCamCadDoc().OnClearSelected();
      getGerberEducator().getCamCadDoc().OnRedraw();
   }
}

void CGerberEducatorDialog::OnBnClickedDeleteByRefDes()
{
   getGerberEducator().deleteComponents(true,false,false,false);
}

void CGerberEducatorDialog::OnBnClickedDeleteByShape()
{
   getGerberEducator().deleteComponents(false,true,false,false);
}

void CGerberEducatorDialog::OnBnClickedDeleteByPartNumber()
{
   getGerberEducator().deleteComponents(false,false,true,false);
}

void CGerberEducatorDialog::OnBnClickedDeleteByGeometry()
{
   getGerberEducator().deleteComponents(false,false,false,true);
}

void CGerberEducatorDialog::OnBnClickedAddAutomatically()
{
   getGerberEducator().addAutomatically(gerberEducatorPackageIdentifierShape);
}

//void CGerberEducatorDialog::OnBnClickedOk()
//{
//   // TODO: Add your control notification handler code here
//}
//
//void CGerberEducatorDialog::OnBnClickedCancel()
//{
//   // TODO: Add your control notification handler code here
//   getActiveView()->PostMessage(WM_COMMAND,ID_TerminateGerberEducator,0);
//   EndDialog(IDCANCEL);
//}

void CGerberEducatorDialog::OnClose()
{
   CGerberEducatorBaseDialog::OnClose();

   getActiveView()->PostMessage(WM_COMMAND,ID_TerminateGerberEducator,0);
   EndDialog(IDCANCEL);
}

void CGerberEducatorDialog::OnBnClickedUnmatchedTop()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_unmatchedTopLayerButton.SetCheck(m_unmatchedTopLayerButton.GetCheck() == 0);
      m_unmatchedBottomLayerButton.SetCheck(0);
      m_unmatchedBothLayerButton.SetCheck(0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedUnmatchedBottom()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_unmatchedTopLayerButton.SetCheck(0);
      m_unmatchedBottomLayerButton.SetCheck(m_unmatchedBottomLayerButton.GetCheck() == 0);
      m_unmatchedBothLayerButton.SetCheck(0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedUnmatchedBoth()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_unmatchedTopLayerButton.SetCheck(0);
      m_unmatchedBottomLayerButton.SetCheck(0);
      m_unmatchedBothLayerButton.SetCheck(m_unmatchedBothLayerButton.GetCheck() == 0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedMatchedTop()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_matchedTopLayerButton.SetCheck(m_matchedTopLayerButton.GetCheck() == 0);
      m_matchedBottomLayerButton.SetCheck(0);
      m_matchedBothLayerButton.SetCheck(0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedMatchedBottom()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_matchedTopLayerButton.SetCheck(0);
      m_matchedBottomLayerButton.SetCheck(m_matchedBottomLayerButton.GetCheck() == 0);
      m_matchedBothLayerButton.SetCheck(0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedMatchedBoth()
{
   //bool checkedFlag = (m_matchedBothLayerButton.GetCheck() == 0);
   //TRACE("CGerberEducatorDialog::OnBnClickedMatchedBoth() - checkedFlag=%d, m_activeState=%d\n",
   //   (int)checkedFlag,m_activeState);

   if (m_activeState != WA_INACTIVE)
   {
      m_matchedTopLayerButton.SetCheck(0);
      m_matchedBottomLayerButton.SetCheck(0);
      m_matchedBothLayerButton.SetCheck(m_matchedBothLayerButton.GetCheck() == 0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedCompPinTop()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_compPinTopLayerButton.SetCheck(m_compPinTopLayerButton.GetCheck() == 0);
      m_compPinBottomLayerButton.SetCheck(0);
      m_compPinBothLayerButton.SetCheck(0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedCompPinBottom()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_compPinTopLayerButton.SetCheck(0);
      m_compPinBottomLayerButton.SetCheck(m_compPinBottomLayerButton.GetCheck() == 0);
      m_compPinBothLayerButton.SetCheck(0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedCompPinBoth()
{
   if (m_activeState != WA_INACTIVE)
   {
      m_compPinTopLayerButton.SetCheck(0);
      m_compPinBottomLayerButton.SetCheck(0);
      m_compPinBothLayerButton.SetCheck(m_compPinBothLayerButton.GetCheck() == 0);

      updateLayers();
   }
}

void CGerberEducatorDialog::OnBnClickedAll()
{
   enableLayerRadioButtons(m_allLayerButton.GetCheck() == 0);

   m_originalLayerButton.SetCheck(0);

   updateLayers();
}

void CGerberEducatorDialog::OnBnClickedOriginal()
{
   enableLayerRadioButtons(m_originalLayerButton.GetCheck() == 0);

   m_allLayerButton.SetCheck(0);

   updateLayers();
}

void CGerberEducatorDialog::OnBnClickedUpdateAttributes()
{
   // TODO: Add your control notification handler code here
}

void CGerberEducatorDialog::OnBnClickedMarkByWindow()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      // CCEtoODBView::OnMarkRect()
      view->PostMessage(WM_COMMAND,ID_QUERY_MARKRECT);
   }
}

void CGerberEducatorDialog::OnUpdateMarkByWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorDialog::OnBnClickedMarkByWindowCross()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      // CCEtoODBView::OnMarkRect_CrossCheck()
      view->PostMessage(WM_COMMAND,ID_QUERY_MARK_CROSSCHECK);
   }
}

void CGerberEducatorDialog::OnUpdateMarkByWindowCross(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorDialog::OnBnClickedClearMarked()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_QUERY_CLEAR_SELECTED);
   }
}

void CGerberEducatorDialog::OnUpdateClearMarked(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CGerberEducatorDialog::OnBnClickedRepaint()
{
   getGerberEducator().getCamCadDoc().OnRedraw();
}

void CGerberEducatorDialog::OnBnClickedPartNumberBaseName()
{
   m_baseNameFrom = baseNameFromShape;
   updateBaseNameFrom();
}

void CGerberEducatorDialog::OnBnClickedShapeBaseName()
{
   m_baseNameFrom = baseNameFromPartNumber;
   updateBaseNameFrom();
}

void CGerberEducatorDialog::OnBnClickedPlaceThTop()
{
   m_placeThTopFlag = false;
   updatePlaceTh();
}

void CGerberEducatorDialog::OnBnClickedPlaceThBottom()
{
   m_placeThTopFlag = true;
   updatePlaceTh();
}

void CGerberEducatorDialog::OnOK() 
{
   //m_addAutomaticallyButton.GetWind

}

void CGerberEducatorDialog::OnBnClickedLockCentroids()
{
   m_centroidsLockedFlag = !m_centroidsLockedFlag;
   update();
}

void CGerberEducatorDialog::OnBnClickedHideButton()
{
   // TODO: Add your control notification handler code here
}

void CGerberEducatorDialog::OnBnClickedOrthoRotationCcw()
{
   m_preferredCcwOrthoRotationFlag = false;
   update();
}

void CGerberEducatorDialog::OnBnClickedOrthoRotationCw()
{
   m_preferredCcwOrthoRotationFlag = true;
   update();
}

void CGerberEducatorDialog::OnDeltaPosRotationSpinner(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

   // TODO: Add your control notification handler code here
   double deltaDegrees = 0.;

   if (pNMUpDown->iDelta != 0)
   {
      deltaDegrees = ((pNMUpDown->iDelta < 0) ? 90. : -90.);
   }

   int degrees = DcaRound(m_rotationDegrees + deltaDegrees);
   degrees = (degrees/90) * 90;

   m_rotationDegrees = normalizeDegrees(degrees);

   update();

   *pResult = 1;
}

void CGerberEducatorDialog::OnDeltaPosRefDesSpinner(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

   if (pNMUpDown->iDelta != 0)
   {
      CString refDesString;
      m_refDesControl.GetWindowText(refDesString);

      CGerberEducatorRefDes refDes(refDesString);
      int increment = ((pNMUpDown->iDelta < 0) ? 1 : -1);

      if (refDes.getSuffix() < 0)
      {
         refDes.setRefDes(getGerberEducator().getRefDesMap().queryCeiling(refDes.getPrefix()));

         if (refDes.getSuffix() == 0) refDes.setSuffix(1);
      }
      else if (increment > 0)
      {
         refDes.setRefDes(getGerberEducator().getRefDesMap().queryNextUndefined(refDesString));
      }
      else
      {
         refDes.setRefDes(getGerberEducator().getRefDesMap().queryPrevUndefined(refDesString));
      }

      m_refDesControl.SetWindowText(refDes.getRefDes());
   }

   m_refDesControl.SetFocus();

   *pResult = 1;
}

void CGerberEducatorDialog::OnKillFocus(CWnd* pNewWnd)
{
   CGerberEducatorBaseDialog::OnKillFocus(pNewWnd);
   m_hideButton.SetFocus();

   // TODO: Add your message handler code here
}

void CGerberEducatorDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   //static count=0;
   CGerberEducatorBaseDialog::OnActivate(nState, pWndOther, bMinimized);

   // TODO: Add your message handler code here
   m_activeState = nState;

   //count++;
   //TRACE("%d - m_activeState=%d\n",count,m_activeState);
}

void CGerberEducatorDialog::OnSetFocus(CWnd* pOldWnd)
{
   CGerberEducatorBaseDialog::OnSetFocus(pOldWnd);

   // TODO: Add your message handler code here
}
BOOL CGerberEducatorDialog::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

   // if there is a top level routing frame then let it handle the message
   if (GetRoutingFrame() != NULL) return FALSE;

   // to be thorough we will need to handle UNICODE versions of the message also !!
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   //TCHAR szFullText[512];
   CString strTipText;
   UINT nID = pNMHDR->idFrom;

   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool 
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   if (nID != 0) // will be zero on a separator
   {
      strTipText.LoadString(nID);
      //AfxLoadString(nID, szFullText);
      //strTipText = szFullText;

   #ifndef _UNICODE
      if (pNMHDR->code == TTN_NEEDTEXTA)
      {
         lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
      }
      else
      {
         _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
      }
   #else
      if (pNMHDR->code == TTN_NEEDTEXTA)
      {
         _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
      }
      else
      {
         lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
      }
   #endif

      *pResult = 0;

      // bring the tooltip window above other popup windows
      ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
         SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
      
      return TRUE;
   }

   return FALSE;
}

void CGerberEducatorDialog::OnEnKillfocusRefDes()
{
   CString refDesString;
   m_refDesControl.GetWindowText(refDesString);

   CGerberEducatorRefDes refDes(refDesString);

   if (refDes.getSuffix() < 0)
   {
      refDes.setSuffix(0);
      refDes.setRefDes(getGerberEducator().getRefDesMap().queryNextUndefined(refDes.getRefDes()));

      if (refDes.getSuffix() == 0) refDes.setSuffix(1);

      m_refDesControl.SetWindowText(refDes.getRefDes());
   }

   update();
}

void CGerberEducatorDialog::OnEnKillfocusShape()
{
   update();
}

void CGerberEducatorDialog::OnEnKillfocusPartNumber()
{
   update();
}

void CGerberEducatorDialog::OnEnKillfocusRotation()
{
   CString rotation;
   m_rotationControl.GetWindowText(rotation);
   m_rotationDegrees = atof(rotation);

   update();
}

void CGerberEducatorDialog::OnEnKillfocusFhs()
{
   update();
}

//_____________________________________________________________________________
ComponentAttributeStatus::ComponentAttributeStatus(CGerberEducatorBaseDialog& gerberEducatorDialog) :
   m_gerberEducatorDialog(gerberEducatorDialog)
{
   m_count = 0;
   m_sameRefDesFlag     = true;
   m_samePartNumberFlag = true;
   m_sameShapeFlag      = true;
   m_sameRotationFlag   = true;
}

ComponentAttributeStatus::~ComponentAttributeStatus()
{
}

void ComponentAttributeStatus::update(DataStruct* data)
{
   if (data != NULL && data->isInsert())
   {
      CString refDes     = m_gerberEducatorDialog.getGerberEducator().getRefDes(*data);
      CString partNumber = m_gerberEducatorDialog.getGerberEducator().getPartNumber(*data);
      CString shape      = m_gerberEducatorDialog.getGerberEducator().getShape(*data);
      InsertStruct* insert = data->getInsert();
      double rotation = insert->getAngle();

      if (m_count == 0)
      {
         m_refDes     = refDes;
         m_partNumber = partNumber;
         m_shape      = shape;
         m_rotation   = rotation;
      }
      else
      {
         m_sameRefDesFlag     = (m_sameRefDesFlag     && (m_refDes.CompareNoCase(refDes) == 0));
         m_samePartNumberFlag = (m_samePartNumberFlag && (m_partNumber.CompareNoCase(partNumber) == 0));
         m_sameShapeFlag      = (m_sameShapeFlag      && (m_shape.CompareNoCase(shape) == 0));
         m_sameRotationFlag   = (m_sameRotationFlag   && fpeq(m_rotation,rotation));
      }

      m_count++;
   }
}

#endif  // defined(EnableGerberEducator)

