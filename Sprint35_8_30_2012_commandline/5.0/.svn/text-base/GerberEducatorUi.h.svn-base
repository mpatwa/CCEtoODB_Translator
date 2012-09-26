// $Header: /CAMCAD/4.5/GerberEducatorUi.h 28    2/16/06 2:29p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if ! defined (__GerberEducatorUi_h__)
#define __GerberEducatorUi_h__

#include "CcDoc.h"
#include "ResizingDialog.h"
#include "StaticLabel.h"
#include "PaletteColorDialog.h"
#include "afxwin.h"

enum GerberEducatorUiStateTag
{
   GerberEducatorUiStateInactive,
   GerberEducatorUiStateTypingData,
   GerberEducatorUiStateAlignLayer,
   GerberEducatorUiStateCreatingGeometries,
   GerberEducatorUiStateEditingGeometries,
   GerberEducatorUiStateSubEditingGeometries,
   GerberEducatorUiStateEditingComponents,
   GerberEducatorUiStateCleaningUpData,
   GerberEducatorUiStateUndefined
};

enum ArrangeWindowsMethodTag
{
   arrangeWindowsMethodBestFit,
   arrangeWindowsMethodMaximumNormal,
   arrangeWindowsMethodUndefined
};

enum GerberEducatorColorTag
{
   // Colors used in Setup Data dialog
   gerberEducatorSetupDataPadTopFlashesColor = 0,
   gerberEducatorSetupDataPadTopDrawsColor,
   gerberEducatorSetupDataPadBottomFlashesColor,
   gerberEducatorSetupDataPadBottomDrawsColor,
   gerberEducatorSetupDataSilkscreenTopColor,
   gerberEducatorSetupDataSilkscreenBottomColor,
   gerberEducatorSetupDataCentroidTopColor,
   gerberEducatorSetupDataCentroidBottomColor,
   gerberEducatorSetupDataUnknownColor,

   // Colors used in Create Geometries dialog
   gerberEducatorCreateGeometriesUnmatchedPadTopColor,
   gerberEducatorCreateGeometriesUnmatchedPadBottomColor,
   gerberEducatorCreateGeometriesUnmatchedCentroidTopColor,
   gerberEducatorCreateGeometriesUnmatchedCentroidBottomColor,
   gerberEducatorCreateGeometriesSilkscreenTopColor,
   gerberEducatorCreateGeometriesSilkscreenBottomColor,
   gerberEducatorCreateGeometriesCentroidTopColor,
   gerberEducatorCreateGeometriesCentroidBottomColor,
   gerberEducatorCreateGeometriesSmdPadTopColor,
   gerberEducatorCreateGeometriesSmdPadBottomColor,
   gerberEducatorCreateGeometriesThPadTopColor,
   gerberEducatorCreateGeometriesThPadBottomColor,
   gerberEducatorCreateGeometriesComponentOutlineTopColor,
   gerberEducatorCreateGeometriesComponentOutlineBottomColor,

   // Colors used in Review Geometries dialog
   gerberEducatorReviewGeometriesCentroidTopColor,
   gerberEducatorReviewGeometriesCentroidBottomColor,
   gerberEducatorReviewGeometriesSmdPadTopColor,
   gerberEducatorReviewGeometriesSmdPadBottomColor,
   gerberEducatorReviewGeometriesThPadTopColor,
   gerberEducatorReviewGeometriesThPadBottomColor,
   gerberEducatorReviewGeometriesComponentOutlineTopColor,
   gerberEducatorReviewGeometriesComponentOutlineBottomColor,

   // Colors used in Review Component dialog
   gerberEducatorReviewComponentCentroidTopColor,
   gerberEducatorReviewComponentCentroidBottomColor,
   gerberEducatorReviewComponentSmdPadTopColor,
   gerberEducatorReviewComponentSmdPadBottomColor,
   gerberEducatorReviewComponentThPadTopColor,
   gerberEducatorReviewComponentThPadBottomColor,
   gerberEducatorReviewComponentComponentOutlineTopColor,
   gerberEducatorReviewComponentComponentOutlineBottomColor,

   gerberEducatorReviewComponentApprovedTopColor,
   gerberEducatorReviewComponentApprovedBottomColor,

   GerberEducatorColorTagMax,
   GerberEducatorColorTagMin = 0,
};

GerberEducatorColorTag stringToGerberEducatorColor(const CString& tagValue);
CString gerberEducatorColorToString(GerberEducatorColorTag tagValue);

//-----------------------------------------------------------------------------
// CGerberEducatorColorScheme
//-----------------------------------------------------------------------------
class CGerberEducatorColorScheme
{
public:
   CGerberEducatorColorScheme();
   ~CGerberEducatorColorScheme();

   CGerberEducatorColorScheme& operator=(const CGerberEducatorColorScheme& other);

private:
   COLORREF m_colors[GerberEducatorColorTagMax];

public:
   COLORREF getColor(GerberEducatorColorTag colorTag) const;
   void setColor(GerberEducatorColorTag colorTag,COLORREF color);

   void loadFromRegistry();
   void storeInRegistry();

	void setDefaultViewColor(CCamCadDatabase& m_camCadDatabase, FileStruct& pcbFile);
   void initializeColors();

};

typedef CGerberEducatorColorScheme CGEStateLayerColor;

class CGerberEducatorGeometryCreator;
class CGESetupDataType;
class CGEAlignLayerDlg;
class CGEReviewInsertsDlg;
class CGEReviewGeometriesDlg;

//_____________________________________________________________________________
class CGerberEducatorUi
{
private:
   static CGerberEducatorUi* m_gerberEducatorUi;

   CCEtoODBDoc& m_camCadDoc;
   FileStruct* m_fileStruct;
   GerberEducatorUiStateTag m_previousUiState;
   GerberEducatorUiStateTag m_uiState;
   CString m_currentEditGeometryName;
   CString m_currentEditComponentRefdes;
   CTypedPtrArrayContainer<LayerStruct*> m_alignLayerArray;

   CGESetupDataType* m_dataTyperDialog;
   CGEAlignLayerDlg* m_alignLayerDialog;
   CGEReviewGeometriesDlg* m_reviewGeometriesDialog;
   CGEReviewInsertsDlg* m_reviewComponentsDialog;

public:
   CGerberEducatorUi(CCEtoODBDoc& camCadDoc);

public:
   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
	FileStruct* getFileStruct();
   void setFileStruct(FileStruct* fileStruct) { m_fileStruct = fileStruct; }
   CGerberEducator& getGerberEducator() ;

   CString getCurrentEditGeometryName() const { return m_currentEditGeometryName; }
   void setCurrentEditGeometryName(const CString& currentEditGeometryName) { m_currentEditGeometryName = currentEditGeometryName; }

   CString getCurrentEditComponentRefdes() const { return m_currentEditComponentRefdes; }
   void setCurrentEditComponentRefdes(const CString& refDes) { m_currentEditComponentRefdes = refDes; }

   void addAlignLayer(LayerStruct* layer);
   void emptyAlignLayerArry();
   int getAlignLayerCount() const;
   LayerStruct* getAlignLayer(int index); 

public:
   void generateEvent(int commandId);
	void updateSelectedEntity();
   BOOL PreTranslateMessage(MSG* pMsg);

   bool isInAlignLayerDialog() const { return m_uiState == GerberEducatorUiStateAlignLayer; }

 //void OnEventConnectToSession();  // ID_GerberEducatorCommand_ConnectToSession
	void OnEventSetupDataType();		// ID_GerberEducatorCommand_SetupDataType
   void OnEventAlignLayer();        
   void OnEventCreateGeometries();  // ID_GerberEducatorCommand_CreateGeometries
   void OnEventEditGeometries();    // ID_GerberEducatorCommand_EditGeometries
   void OnEventEditComponents();    // ID_GerberEducatorCommand_EditComponents
   void OnEventCleanUpData();       // ID_GerberEducatorCommand_CleanUp
   void OnEventTerminateSession();  // ID_GerberEducatorCommand_TerminateSession

private:
   GerberEducatorUiStateTag getPreviousUiState() { return m_previousUiState; }
   GerberEducatorUiStateTag getUiState() { return m_uiState; }
   void setUiState(GerberEducatorUiStateTag uiState);
   void closeGerberEducator();
   void closeDataTyperDialog();
   void closeAlignLayerDialog();
   void closeReviewGeometriesDialog();
   void closeReviewComponentsDialog();
   void arrangeWindows(CDialog& dialog,ArrangeWindowsMethodTag arrangeWindowsMethod);

public:
   static CGerberEducatorUi& getGerberEducatorUi(CCEtoODBDoc& camCadDoc);
   static CGerberEducatorUi* getGerberEducatorUi();
   static bool deleteGerberEducatorUi();
};

//_____________________________________________________________________________
class CGerberEducatorColorsDialog : public CResizingDialog
{
private:
   bool m_initializedFlag;
   CGerberEducatorColorScheme m_layerColors;
   CStaticLabel* m_colorBox[GerberEducatorColorTagMax];

public:
	CGerberEducatorColorsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGerberEducatorColorsDialog();

// Dialog Data
	enum { IDD = IDD_GerberEducatorColorsDialog };

   virtual CString GetDialogProfileEntry() { return CString("GerberEducatorColorsDialog"); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedRestore();
   afx_msg void OnBnClickedRestoreDefault();

   // Colors used in Setup Data dialog
   CStaticLabel m_suGerberPadTopFlashesStatic;
   CStaticLabel m_suGerberPadTopDrawsStatic;
   CStaticLabel m_suGerberPadBottomFlashesStatic;
   CStaticLabel m_suGerberPadBottomDrawsStatic;
   CStaticLabel m_suGerberSilkscreenTopStatic;
   CStaticLabel m_suGerberSilkscreenBottomStatic;
   CStaticLabel m_suCentroidTopStatic;
   CStaticLabel m_suCentroidBottomStatic;
   CStaticLabel m_suUnknownStatic;

   // Colors used in Create Geometries dialog
   CStaticLabel m_geUnmatchedPadTopStatic;
   CStaticLabel m_geUnmatchedPadBottomStatic;
   CStaticLabel m_geUnmatchedCentroidTopStatic;
   CStaticLabel m_geUnmatchedCentroidBottomStatic;
   CStaticLabel m_geSilkscreenTopStatic;
   CStaticLabel m_geSilkscreenBottomStatic;
   CStaticLabel m_geCentroidTopStatic;
   CStaticLabel m_geCentroidBottomStatic;
   CStaticLabel m_geSmdCompPadTopStatic;
   CStaticLabel m_geSmdCompPadBottomStatic;
   CStaticLabel m_geThCompPadTopStatic;
   CStaticLabel m_geThCompPadBottomStatic;
   CStaticLabel m_geCompOutlineTopStatic;
   CStaticLabel m_geCompOutlineBottomStatic;

   // Colors used in Review Geometries dialog
   CStaticLabel m_rgCentroidTopStatic;
   CStaticLabel m_rgCentroidBottomStatic;
   CStaticLabel m_rgSmdCompPadTopStatic;
   CStaticLabel m_rgSmdCompPadBottomStatic;
   CStaticLabel m_rgThCompPadTopStatic;
   CStaticLabel m_rgThCompPadBottomStatic;
   CStaticLabel m_rgCompOutlineTopStatic;
   CStaticLabel m_rgCompOutlineBottomStatic;

   // Colors used in Review Component dialog
   CStaticLabel m_uaCentroidTopStatic;
   CStaticLabel m_uaCentroidBottomStatic;
   CStaticLabel m_uaSmdCompPadTopStatic;
   CStaticLabel m_uaSmdCompPadBottomStatic;
   CStaticLabel m_uaThCompPadTopStatic;
   CStaticLabel m_uaThCompPadBottomStatic;
   CStaticLabel m_uaCompOutlineTopStatic;
   CStaticLabel m_uaCompOutlineBottomStatic;

   CStaticLabel m_apCentroidTopStatic;
   CStaticLabel m_apCentroidBottomStatic;
   CStaticLabel m_apSmdCompPadTopStatic;
   CStaticLabel m_apSmdCompPadBottomStatic;
   CStaticLabel m_apThCompPadTopStatic;
   CStaticLabel m_apThCompPadBottomStatic;
   CStaticLabel m_apCompOutlineTopStatic;
   CStaticLabel m_apCompOutlineBottomStatic;

   CStaticLabel m_currentColorStatic;
   CStaticLabel m_dummyDefault;

   CColorPaletteStatic m_colorPaletteStatic;
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   virtual BOOL OnInitDialog();

public:
   CStaticLabel* getColorBox(CPoint point);
   CStaticLabel& getColorBox(GerberEducatorColorTag colorTag);
   void setColors(const CGerberEducatorColorScheme& layerColors);
   CGerberEducatorColorScheme getColors();
};

//_____________________________________________________________________________
class CGerberEducatorCleanUpDataDialog : public CDialog
{
	DECLARE_DYNCREATE(CGerberEducatorCleanUpDataDialog)

private:
   BOOL m_removeUnusedLayers;
   BOOL m_removeNonGerberEducatorLayers;
   BOOL m_changeFileTypes;
   BOOL m_removeStandAloneCentroids;

public:
	CGerberEducatorCleanUpDataDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGerberEducatorCleanUpDataDialog();
// Overrides

// Dialog Data
	enum { IDD = IDD_GerberEducatorCleanUpDataDialog };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
   bool getRemoveUnusedLayersFlag() const { return (m_removeUnusedLayers != 0); }
   bool getRemoveNonGerberEducatorLayersFlag() const { return (m_removeNonGerberEducatorLayers != 0); }
   bool getChangeFileTypesFlag() const { return (m_changeFileTypes != 0); }
   bool getRemoveStandAloneCentroidsFlag() const { return (m_removeStandAloneCentroids != 0); }
};

#endif
