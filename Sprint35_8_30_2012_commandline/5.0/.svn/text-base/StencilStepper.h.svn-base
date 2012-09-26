// $Header: /CAMCAD/4.6/StencilStepper.h 12    5/09/06 9:03p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#if ! defined (__StencilStepper_h__)
#define __StencilStepper_h__

#pragma once

#include "ResizingDialog.h"
#include "CamCadDatabase.h"
#include "DialogToolbar.h"
#include "afxwin.h"
#include "UGCtrl.h"

class CStencilStepperDialog;

//_____________________________________________________________________________
class CStencilDepositMap : public CTypedMapPtrToPtrContainer<DataStruct*,double*>
{
public:
   CStencilDepositMap(bool isContainer=true);
};

//_____________________________________________________________________________
class CStencilStep
{
private:
   double m_thickness;
   COLORREF m_color;
   CStencilDepositMap m_depositMap;

public:
   CStencilStep(double thickness,COLORREF color);
   void empty();


   double getThickness() const { return m_thickness; }
   CString getThicknessString() const;
   int getDepositCount() const;
   CString getDepositCountString() const;

   COLORREF getColor() const { return m_color; }
   void setColor(COLORREF color);

   void setAt(DataStruct* deposit);
   void removeAt(DataStruct* deposit);
};

//_____________________________________________________________________________
class CStencilSteps
{
private:
   CStencilDepositMap m_deposits;
   CTypedPtrListContainer<CStencilStep*> m_stepList;
   CMapPtrToPtr m_colorMap;

public:
   CStencilSteps();
   void empty();
   void emptySteps();

   CStencilStep& updateThickness(DataStruct* deposit,double thickness);
   int getStepCount();
   POSITION getHeadStepPosition();
   CStencilStep* getNextStep(POSITION& pos);
   CStencilStep& getStep(double thickness);
   void setColorOverride(bool colorOverrideFlag);
   void removeStencilStepperAttributes(CCamCadDatabase& camCadDatabase,int stencilThicknessKeywordIndex);

private:
   COLORREF getNextStepColor();
   CStencilStep* getNewStep(double thickness);


};

//_____________________________________________________________________________
class CStencilStepperGrid : public CUGCtrl
{
private:
   CStencilStepperDialog& m_parentDialog;
   int m_currentCol;
   int m_currentRow;

public:
   CStencilStepperGrid(CStencilStepperDialog& parentDialog);

   virtual void ExpandLastColumnToFit();
   virtual void SizeLastColumnToFit(bool expandOnlyFlag=false);

   int GetSH_NumberCols() const { return m_GI->m_numberSideHdgCols; }

   // UG
   virtual void OnLClicked(int col,long row,int updn,RECT* rect,POINT* point,BOOL processed);
   virtual int OnEditStart(int col, long row, CWnd **edit);
};

//_____________________________________________________________________________
class CStencilStepperDialog : public CResizingDialog
{
	//DECLARE_DYNAMIC(CStencilStepperDialog)
private:
   const static int m_colThickness     =  0;
   const static int m_colColor         =  1;
   const static int m_colDepositCount  =  2;
   const static int m_colCount         =  3;

   CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase;
   FileStruct* m_pcbFile;
   int m_selectedLayerIndex;
   int m_stencilBaseThicknessKeywordIndex;
   int m_stencilThicknessKeywordIndex;
   int m_stencilColorsKeywordIndex;
   CStencilSteps m_stencilSteps;
   double m_minBaseStencilThickness;
   double m_maxBaseStencilThickness;
   bool m_showColorFlag;

   // controls
   CButton m_addPadsToStepButton;
   CStatic m_depositCountStatic;
   CStatic m_baseThicknessStatic;
   CComboBox m_pasteLayersComboBox;
   CEdit m_baseThicknessEditBox;
   CEdit m_stencilThicknessEditBox;
   CStencilStepperGrid m_stepGrid;

   CResizingDialogToolBar m_toolBar;

   // step parameters
   double m_baseThickness;
   CString m_stepColors;

public:
	//CStencilStepperDialog(CWnd* pParent = NULL);   // standard constructor
	CStencilStepperDialog(CCEtoODBDoc& camCadDoc);
	virtual ~CStencilStepperDialog();

// Dialog Data
	enum { IDD = IDD_StencilStepperUg };

   virtual CString GetDialogProfileEntry() { return CString("StencilStepperDialog"); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   CCamCadDatabase& getCamCadDatabase();
   void readParameters();
   void writeParameters();
   void scanForLayers();
   void changePasteLayer(int pasteLayerIndex,bool repaintFlag);
   void updateDepositData(bool repaintFlag=false);
   void setVisibleLayer(int layerindex);

   FileStruct* getPcbFile() { return m_pcbFile; }

   int getStencilBaseThicknessKeywordIndex();
   int getStencilThicknessKeywordIndex();
   int getStencilColorsKeywordIndex();

   void initGrid();
   void fillGrid();
   int getSelectedLayerIndex();
   void setBaseStencilThickness(CString baseStencilThicknessString);
   void setBaseStencilThickness(double baseStencilThickness);
   CString getStepColors();
   CString setStepColors(const CString& stepColors);
   void updateShowOverrideColor();
   void repaint();

public:
   bool editCell(int row,int col);
   void nonFixedCellClick(int row,int col);
   void updateSelection();
   void removeStencilStepperAttributes();
   void clearMarked();

   // grid notifications
   void OnUgGridDClicked(CStencilStepperGrid* grid,int col,long row,RECT* rect,POINT* point,BOOL processed);

	DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedAddPadsToStep();
   afx_msg void OnClose();
   afx_msg void OnEnKillFocusBaseStencilThickness();
   afx_msg void OnCbnSelchangePasteLayers();

   // toolbar button events
   afx_msg void OnBnClickedMarkByWindow();
   afx_msg void OnUpdateMarkByWindow(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedMarkByWindowCross();
   afx_msg void OnUpdateMarkByWindowCross(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedClearMarked();
   afx_msg void OnUpdateClearMarked(CCmdUI* pCmdUI);
   afx_msg void OnBnClickedRepaint();
   afx_msg void OnBnClickedShowOverrideColor();
   afx_msg void OnBnClickedHideOverrideColor();

   afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnBnClickedRemoveStencilStepperAttributes();
   afx_msg void OnSize(UINT nType, int cx, int cy);
};

#endif
