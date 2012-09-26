// $Header: /CAMCAD/4.5/GerberEducatorDialog.h 38    10/10/05 6:48p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if ! defined (__GerberEducatorDialog_h__)
#define __GerberEducatorDialog_h__

#pragma once

#include "StaticLabel.h"
#include "ResizingDialog.h"
#include "DialogToolbar.h"
#include "CamCadDatabase.h"
#include "ColorEditBox.h"
#include "GerberEducator.h"
#include "PinChain.h"
#include "afxwin.h"
#include "afxcmn.h"

//_____________________________________________________________________________
enum SelectListStatusTag
{
   selectListStatusNoCentroid            ,  // A single centroid must be selected.
   selectListStatusMultipleCentroids     ,  // More than one centroid is selected.
   selectListStatusNoPins                ,  // At least one pad must be selected.
   selectListStatusMultipleLayers        ,  // Selected centroid and pads must be on the same layer.
   selectListStatusNonEducatorLayer      ,  // Selected centroid and pads must be on an unmatched layer.
   selectListStatusIllegalData           ,  // Only centroids and aperture inserts may be selected
   selectListStatusIllegalMatchedData    ,  // Only centroids may be selected on matched layers
   selectListStatusIllegalUnmatchedData  ,  // "When adding components, only centroids and apertures may be marked.
   selectListStatusNoPin1                ,  // Pin 1 must be selected
   selectListStatusMatchedCentroids      ,  // One or more matched centroids
   selectListStatusComponents            ,  // One or more components selected
   selectListStatusComponentsAndCentroids,  // One or more components and matched centroids selected
   selectListStatusNoRefDes              ,  // A refDes must be specified
   selectListStatusDuplicateRefDes       ,  // Specified refDes already exists as a component
   selectListStatusPadsFromBothLayers    ,  // Pads from both unmatched layers selected
   selectListStatusNoFhs                 ,  // A Fhs must be specified
   selectListStatusOk                    ,
   selectListStatusUndefined
};

enum BaseNameFromTag
{
   baseNameFromPartNumber,
   baseNameFromShape,
   baseNameFromUndefined
};

enum EducatorLayerFilterTag
{
   educatorLayerFilterEverything,
   educatorLayerFilterPinsCentroidsComponents,
   educatorLayerFilterPinsCentroids,
   educatorLayerFilterPins,
   educatorLayerFilterComponents,
   educatorLayerFilterUndefined,
   educatorLayerFilterUpperBound = educatorLayerFilterUndefined - 1,
   educatorLayerFilterLowerBound = educatorLayerFilterEverything
};

//_____________________________________________________________________________
class CGerberEducatorBaseDialog : public CResizingDialog
{
private:
   CGerberEducator& m_gerberEducator;
   CGerberEducatorUi* m_gerberEducatorUi;

public:
	CGerberEducatorBaseDialog(int dialogId,CGerberEducator& gerberEducator);
	virtual ~CGerberEducatorBaseDialog();

   CGerberEducator& getGerberEducator() { return m_gerberEducator; }
   CCEtoODBDoc& getCamCadDoc();
   CCamCadDatabase& getCamCadDatabase();

   CGerberEducatorUi* getGerberEducatorUi() const { return m_gerberEducatorUi; }
   void setGerberEducatorUi(CGerberEducatorUi* gerberEducatorUi);

   virtual void update() = 0;
   virtual BaseNameFromTag getBaseNameFrom() = 0;
   virtual bool isWorkingSurfaceTop();
   virtual bool getDisplayWorkingSurfaceFlag();
   virtual bool getDisplayBothSurfacesFlag();
   virtual bool isSurfaceDisplayed(bool topFlag);
};

//_____________________________________________________________________________
class CGerberEducatorCreateGeometryDialog : public CGerberEducatorBaseDialog
{
private:
   CPinDataTreeList m_pinList;
   int m_selectFilterIds[(educatorLayerFilterUpperBound + 1)*2*2];
   bool m_thMountFlag;
   Bool3Tag m_lastUserThMount;
   bool m_topViewFlag;
   bool m_lastThMountFlag;
   CNamedView m_namedView;
   static const bool m_enableSingleSurfaceThPins = true;
   EducatorLayerFilterTag m_defaultSelectedEducatorLayerFilter;
   EducatorLayerFilterTag m_educatorLayerFilter;
   EducatorLayerFilterTag m_lastSelectedEducatorLayerFilter;

   SelectListStatusTag m_status;
   DataStruct* m_centroid;
   DataStruct* m_previousCentroid;
   bool m_centroidsLockedFlag;
   CString m_refDes;
   CString m_shape;
   CString m_partNumber;
   CString m_rotationDegrees;
   CString m_fhs;

   int m_toolBarRepaintIndex;
   int m_toolBarMarkByRectangleIndex;
   int m_activeState;

   // controls
   CResizingDialogToolBar m_toolBar1;
   CResizingDialogToolBar m_toolBar2;

   // Selection Mode
   CButton m_createComponentsButton;
   CButton m_deleteComponentsButton;

   CStaticLabel   m_selectedGerberPadsLabelStatic;
   CStaticLabel   m_selectedCentroidsLabelStatic;
   CStaticLabel   m_selectedDrillsLabelStatic;
   CStaticCounter m_selectedPinPadsCountStatic;
   CStaticCounter m_derivedDrillsCountStatic;
   CStaticCounter m_selectedCentroidsCountStatic;

   CStaticLabel   m_pcbCompsLabelStatic;
   CStaticLabel   m_mechCompsLabelStatic;
   CStaticLabel   m_toolingLabelStatic;
   CStaticLabel   m_fiducialLabelStatic;
   CStaticCounter m_selectedPcbComponentCountStatic;
   CStaticCounter m_selectedMechComponentCountStatic;
   CStaticCounter m_selectedToolingCountStatic;
   CStaticCounter m_selectedFiducialCountStatic;

   // Create Component(s)
   CButton m_addIndividualComponentButton;
   CButton m_addComponentsByPatternButton;
   CButton m_addComponentsByOrthogonalPatternButton;
   //CButton m_addComponentsAutomaticallyByCentroidButton;

   CStatic m_mountGroup;
   CButton m_thMountButton;
   CButton m_smdMountButton;

   // Delete Component(s)
   CButton m_deleteIndividualComponentButton;
   CButton m_deleteComponentsByGeometryButton;

   // View
   CStatic m_viewSurfaceGroup;
   CButton m_topViewButton;
   CButton m_bottomViewButton;

   CButton m_displayWorkingSurfaceButton;
   CButton m_displayBothSurfacesButton;
   CButton m_allowIdleSurfaceSelectionButton;

   CButton m_displayComponentsButton;
   CButton m_displayPadsButton;
   CButton m_displaySilkscreenButton;

   // Summary
   CStaticLabel   m_dataTypeLabelStatic;
   CStaticLabel   m_gerberPadsLabelStatic;
   CStaticLabel   m_centroidsLabelStatic;
   CStaticLabel   m_componentsLabelStatic;
   CStaticLabel   m_geometriesLabelStatic;
   CStaticLabel   m_topLabelStatic;
   CStaticLabel   m_bottomLabelStatic;
   CStaticCounter m_gerberPadsTopCountStatic;
   CStaticCounter m_gerberPadsBottomCountStatic;
   CStaticCounter m_centroidsTopCountStatic;
   CStaticCounter m_centroidsBottomCountStatic;
   CStaticCounter m_componentsTopCountStatic;
   CStaticCounter m_componentsBottomCountStatic;
   CStaticCounter m_geometriesCountStatic;

   // Debug release accessable
   CButton m_generateCentroidLayersButton;
   CButton m_addAutomaticallyButton;

public:
	CGerberEducatorCreateGeometryDialog(CGerberEducator& gerberEducator);
	virtual ~CGerberEducatorCreateGeometryDialog();

// Dialog Data
	enum { IDD = IDD_GerberEducatorCreateGeometryDialog };

   void addPinToList(DataStruct* pin);
   bool getOkToAdd();
   bool getEnableCentroidEditingFlag() { return true; }
   bool getAllowIdleSurfaceSelectionFlag();

   virtual CString GetDialogProfileEntry() { return CString("GerberEducatorCreateGeometryDialog"); }
   virtual void update();
   virtual BaseNameFromTag getBaseNameFrom() { return baseNameFromPartNumber; }
   virtual bool isWorkingSurfaceTop();
   virtual bool getDisplayWorkingSurfaceFlag();
   virtual bool getDisplayBothSurfacesFlag();

private:
   void initializeFilterIds();
   int getToolBarIndex(int id);
   bool calcNamedView();
   void updateLayers();
   void enableLayerRadioButtons(bool enableFlag);
   SelectListStatusTag setFencedStatus(SelectListStatusTag status);
   void addComponentsByPatternMatching(bool orthogonalFlag);
   void setLayerFilter(EducatorLayerFilterTag educatorLayerFilter);
   void removeSelectFilter(EducatorLayerFilterTag educatorLayerFilter);
   void updateViewSurface();
   void updateMount();
   void restoreOriginalView();
   void filterSelectList();
   int getSelectFilterIndex(EducatorLayerFilterTag,bool topViewFlag,bool bothSurfacesFlag);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();

   // other dialog control events
   afx_msg void OnClose();
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

   // dialog button clicks
   //afx_msg void OnBnClickedUnmatchedTop();
   //afx_msg void OnBnClickedUnmatchedBottom();
   //afx_msg void OnBnClickedUnmatchedBoth();
   //afx_msg void OnBnClickedSilkscreenTop();
   //afx_msg void OnBnClickedSilkscreenBottom();
   //afx_msg void OnBnClickedSilkscreenBoth();
   //afx_msg void OnBnClickedCompPinTop();
   //afx_msg void OnBnClickedCompPinBottom();
   //afx_msg void OnBnClickedCompPinBoth();

   //afx_msg void OnBnClickedAll();
   //afx_msg void OnBnClickedOriginal();

   // toolbar button events
   afx_msg void OnBnClickedMarkByWindow();
   afx_msg void OnUpdateMarkByWindow(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedMarkByWindowCross();
   afx_msg void OnUpdateMarkByWindowCross(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedClearMarked();
   afx_msg void OnUpdateClearMarked(CCmdUI* pCmdUI);

   afx_msg void OnBnClickedZoomIn();
   afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomOut();
   afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomWindow();
   afx_msg void OnUpdateZoomWindow(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomExtents();
   afx_msg void OnUpdateZoomExtents(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedZoomFull();
   afx_msg void OnUpdateZoomFull(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedPanCenter();
   afx_msg void OnUpdatePanCenter(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedQuery();
   afx_msg void OnUpdateQuery(CCmdUI* pCmdUI);

   afx_msg void OnBnClickedRepaint();
   afx_msg void OnBnClickedAddIndividualComponent();
   afx_msg void OnBnClickedAddComponentsByPatternMatching();
   //afx_msg void OnBnClickedAddComponentsAutomatically();
   afx_msg void OnBnClickedDeleteIndividualComponent();
   afx_msg void OnBnClickedDeleteComponentsByGeometry();
   //afx_msg void OnBnClickedAddComponentsByOrthogonalPatternMatching();
   afx_msg void OnBnClickedThMount();
   afx_msg void OnBnClickedSmdMount();
   afx_msg void OnBnClickedTopView();
   afx_msg void OnBnClickedBottomView();
   afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
   afx_msg void OnBnClickedCreateComponents();
   afx_msg void OnBnClickedDeleteComponents();

   afx_msg void OnBnClickedDisplayWorkingSurface();
   afx_msg void OnBnClickedDisplayBothSurfaces();
   afx_msg void OnBnClickedAllowSelection();
   afx_msg void OnBnClickedDisplayComponents();
   afx_msg void OnBnClickedDisplayPads();
   afx_msg void OnBnClickedDisplaySilkScreen();
   afx_msg void OnBnClickedClose();

   afx_msg void OnBnClickedAddAutomatically();
   afx_msg void OnBnClickedGenerateCentroidLayers();
   afx_msg void OnStnDblClickGeometriesLabel();
};

//_____________________________________________________________________________
class CGerberEducatorDialog : public CGerberEducatorBaseDialog
{
private:
   CPinDataTreeList m_pinList;

   CStaticLabel m_generationStatusStatic;
   CButton m_updateAttributesButton;
   CButton m_addByRefDesButton;
   CButton m_addByShapeButton;
   CButton m_addByPartNumberButton;
   CButton m_addByPatternButton;
   CButton m_addAutomaticallyButton;
   CButton m_deleteByRefDesButton;
   CButton m_deleteByShapeButton;
   CButton m_deleteByPartNumberButton;
   CButton m_deleteByGeometryButton;
   CButton m_orthoRotationCcwButton;
   CButton m_orthoRotationCwButton;

   CColorEditBox m_refDesControl;
   CColorEditBox m_shapeControl;
   CColorEditBox m_partNumberControl;
   CColorEditBox m_fhsControl;
   CColorEditBox m_rotationControl;
   CSpinButtonCtrl m_rotationSpinControl;
   CSpinButtonCtrl m_refDesSpinControl;
   CListCtrl m_pinsListControl;

   CButton m_unmatchedTopLayerButton;
   CButton m_unmatchedBottomLayerButton;
   CButton m_unmatchedBothLayerButton;
   CButton m_matchedTopLayerButton;
   CButton m_matchedBottomLayerButton;
   CButton m_matchedBothLayerButton;
   CButton m_compPinTopLayerButton;
   CButton m_compPinBottomLayerButton;
   CButton m_compPinBothLayerButton;
   CButton m_allLayerButton;
   CButton m_originalLayerButton;
   CButton m_hideButton;

   CResizingDialogToolBar m_toolBar;
   int m_toolBarRepaintIndex;
   int m_toolBarMarkByRectangleIndex;
   int m_toolBarLockCentroidsIndex;
   int m_toolBarDeleteCentroidIndex;
   int m_toolBarNextMatchIndex;
   int m_toolBarPrevMatchIndex;
   int m_activeState;

   bool m_centroidsLockedFlag;
   bool m_preferredCcwOrthoRotationFlag;
   bool m_placeThTopFlag;
   double m_rotationDegrees;
   SelectListStatusTag m_status;
   DataStruct* m_centroid;
   DataStruct* m_previousCentroid;
   BaseNameFromTag m_baseNameFrom;

public:
	CGerberEducatorDialog(CGerberEducator& gerberEducator);
	virtual ~CGerberEducatorDialog();

   bool getEnableCentroidEditingFlag();
   void addPinToList(DataStruct* pin);
   bool getOkToAdd();

// Dialog Data
	enum { IDD = IDD_GerberEducatorDialog };

   virtual CString GetDialogProfileEntry() { return CString("GerberEducatorDialog"); }
   virtual void update();
   virtual BaseNameFromTag getBaseNameFrom() { return m_baseNameFrom; }

private:
   SelectListStatusTag setFencedStatus(SelectListStatusTag status);
   void addByRefDes(GerberEducationMethodTag educationMethod);
   void updateLayers();
   void enableLayerRadioButtons(bool enableFlag);
   int getToolBarIndex(int id);
   void updateBaseNameFrom();
   void updatePlaceTh();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   virtual void OnOK();

   // dialog button clicks
   afx_msg void OnBnClickedAddByRefDes();
   afx_msg void OnBnClickedAddByShape();
   afx_msg void OnBnClickedAddByPartNumber();
   afx_msg void OnBnClickedAddByPinPattern();
   afx_msg void OnBnClickedDeleteByRefDes();
   afx_msg void OnBnClickedDeleteByShape();
   afx_msg void OnBnClickedDeleteByPartNumber();
   afx_msg void OnBnClickedDeleteByGeometry();
   //afx_msg void OnBnClickedOk();
   //afx_msg void OnBnClickedCancel();
   afx_msg void OnBnClickedUnmatchedTop();
   afx_msg void OnBnClickedUnmatchedBottom();
   afx_msg void OnBnClickedUnmatchedBoth();
   afx_msg void OnBnClickedMatchedTop();
   afx_msg void OnBnClickedMatchedBottom();
   afx_msg void OnBnClickedMatchedBoth();
   afx_msg void OnBnClickedCompPinTop();
   afx_msg void OnBnClickedCompPinBottom();
   afx_msg void OnBnClickedCompPinBoth();
   afx_msg void OnBnClickedAll();
   afx_msg void OnBnClickedOriginal();
   afx_msg void OnBnClickedAddAutomatically();
   afx_msg void OnBnClickedUpdateAttributes();
   afx_msg void OnBnClickedOrthoRotationCcw();
   afx_msg void OnBnClickedOrthoRotationCw();

   // other dialog control events
   afx_msg void OnBnClickedHideButton();
   afx_msg void OnClose();
   afx_msg void OnDeltaPosRotationSpinner(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnDeltaPosRefDesSpinner(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg void OnSetFocus(CWnd* pOldWnd);
   afx_msg void OnEnKillfocusRefDes();
   afx_msg void OnEnKillfocusShape();
   afx_msg void OnEnKillfocusPartNumber();
   afx_msg void OnEnKillfocusRotation();
   afx_msg void OnEnKillfocusFhs();

   // toolbar button events
   afx_msg void OnBnClickedMarkByWindow();
   afx_msg void OnUpdateMarkByWindow(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedMarkByWindowCross();
   afx_msg void OnUpdateMarkByWindowCross(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedClearMarked();
   afx_msg void OnUpdateClearMarked(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedRepaint();
   afx_msg void OnBnClickedPartNumberBaseName();
   afx_msg void OnBnClickedShapeBaseName();
   afx_msg void OnBnClickedPlaceThTop();
   afx_msg void OnBnClickedPlaceThBottom();
   afx_msg void OnBnClickedLockCentroids();
   afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
};

//_____________________________________________________________________________
class ComponentAttributeStatus
{
private:
   CGerberEducatorBaseDialog& m_gerberEducatorDialog;
   int m_count;

   CString m_refDes;
   CString m_partNumber;
   CString m_shape;
   double m_rotation;

   bool m_sameRefDesFlag;
   bool m_samePartNumberFlag;
   bool m_sameShapeFlag;
   bool m_sameRotationFlag;
   //bool m_sameFhsFlag;

public:
   ComponentAttributeStatus(CGerberEducatorBaseDialog& gerberEducatorDialog);
   ~ComponentAttributeStatus();

   int getCount()               { return m_count; }
   bool getSameRefDesFlag()     { return m_sameRefDesFlag; }
   bool getSamePartNumberFlag() { return m_samePartNumberFlag; }
   bool getSameShapeFlag()      { return m_sameShapeFlag; }
   bool getSameRotationFlag()   { return m_sameRotationFlag; }

   void update(DataStruct* data);
};

#endif
