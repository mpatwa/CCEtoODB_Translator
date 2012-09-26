// $Header: /CAMCAD/5.0/StandardApertureDialog.h 25    3/12/07 12:45p Kurt Van Ness $

#if ! defined (__StandardApertureDialog_h__)
#define __StandardApertureDialog_h__

#pragma once

#include "AfxWin.h"
#include "TypedContainer.h"
#include "CamCadDatabase.h"
#include "StandardAperture.h"
#include "ResizingDialog.h"

class CStandardApertureShape;

//_____________________________________________________________________________
class CFramelessCamCadDoc : public CCEtoODBDoc
{
public:
   CFramelessCamCadDoc();
   ~CFramelessCamCadDoc();
};

//_____________________________________________________________________________
class CFramelessCamCadView : public CCEtoODBView
{
public:
   CFramelessCamCadView();
   ~CFramelessCamCadView();
};

//_____________________________________________________________________________
class CFramelessCamCadFrame
{
private:
   CFramelessCamCadDoc* m_camCadDoc;
   CFramelessCamCadView* m_camCadView;
   CCamCadDatabase* m_camCadDatabase;
   FileStruct* m_pcbFile;
   BlockStruct* m_pcbBlock;
   CArray<int,int> m_displayedLayerIndices;

public:
   CFramelessCamCadFrame();
   ~CFramelessCamCadFrame();

   CCamCadDatabase& getCamCadDatabase() { return *m_camCadDatabase; }
   CFramelessCamCadDoc& getCamCadDoc() { return *m_camCadDoc; }
   CFramelessCamCadView& getCamCadView() { return *m_camCadView; }
   BlockStruct& getPcbBlock() { return *m_pcbBlock; }

   void defineLayer(const CString layerName,COLORREF layerDisplayColor);
   int getDisplayedLayerCount() const { return m_displayedLayerIndices.GetCount(); }
   int getDisplayedLayerIndex(int index) const { return m_displayedLayerIndices.GetAt(index); }
};

//_____________________________________________________________________________
class CCamCadDocGraph
{
private:
   CCEtoODBDoc* m_oldCamCadDoc;

public:
   CCamCadDocGraph(CCEtoODBDoc* camCadDoc);
   ~CCamCadDocGraph();
};

//_____________________________________________________________________________
class CStandardApertureViewStatic : public CStatic
{
	DECLARE_DYNAMIC(CStandardApertureViewStatic)

private:
   static int m_nextId;
   int m_id;

protected:
   CFramelessCamCadFrame* m_camCadFrame;
   CCamCadDatabase* m_camCadDatabase;
   CStandardApertureShape* m_standardApertureShape;
   double m_scaleFactor;

public:
	CStandardApertureViewStatic();
	//CStandardApertureViewStatic(CCamCadDatabase& camCadDatabase);
	virtual ~CStandardApertureViewStatic();

protected:
	DECLARE_MESSAGE_MAP()

public:
   void PrepareDC(CDC* dc);
   virtual void Draw(CDC* dc);
   CCEtoODBDoc* getCamCadDoc()           { return ((m_camCadDatabase == NULL) ? NULL : &(m_camCadDatabase->getCamCadDoc())); }
   CCamCadDatabase* getCamCadDatabase() { return m_camCadDatabase; }
   CCamCadData* getCamCadData()         { return ((m_camCadDatabase == NULL) ? NULL : &(m_camCadDatabase->getCamCadData())); }

   virtual void empty();
   virtual CExtent getExtent();
   virtual void calcScaling(double scale=1.6);
   virtual void setStandardApertureShape(CStandardApertureShape& standardApertureShape);
   virtual void drawText(CDC* dc,TextStruct* textStruct);

   virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
   afx_msg void OnPaint();
   afx_msg void OnMove(int x,int y);
   afx_msg void OnSize(UINT nType,int cx,int cy);
};

//_____________________________________________________________________________
class CStandardApertureDialog : public CResizingDialog
{
	//DECLARE_DYNAMIC(CStandardApertureDialog)

private:
   CStandardApertureDialogParameters m_parameters;
   CStandardApertureShape* m_standardApertureShape;
   CStandardApertureShape* m_genericStandardApertureShape;

public:
	//CStandardApertureDialog(CWnd* pParent = NULL);   // standard constructor
	CStandardApertureDialog(PageUnitsTag pageUnits);   
	virtual ~CStandardApertureDialog();

// Dialog Data
	enum { IDD = IDD_StandardAperture };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
   //
   CString m_initialDescriptor;
   StandardApertureTypeTag m_initialApertureType;
   PageUnitsTag m_pageUnits;
   CenterTag m_center;

   // controls
   CStatic m_genericParameterCaption0;
   CStatic m_genericParameterCaption1;
   CStatic m_genericParameterCaption2;
   CStatic m_genericParameterCaption3;
   CStatic m_genericParameterCaption4;
   CStatic m_genericParameterCaption5;

   CStatic m_parameterCaption0;
   CStatic m_parameterCaption1;
   CStatic m_parameterCaption2;
   CStatic m_parameterCaption3;
   CStatic m_parameterCaption4;
   CStatic m_parameterCaption5;

   CEdit m_valueEditBox0;
   CEdit m_valueEditBox1;
   CEdit m_valueEditBox2;
   CEdit m_valueEditBox3;
   CEdit m_valueEditBox4;
   CEdit m_valueEditBox5;

   CStatic m_parameterDescriptionStatic0;
   CStatic m_parameterDescriptionStatic1;
   CStatic m_parameterDescriptionStatic2;
   CStatic m_parameterDescriptionStatic3;
   CStatic m_parameterDescriptionStatic4;
   CStatic m_parameterDescriptionStatic5;

   CEdit m_descriptorEditBox;
   CStatic m_genericDescriptorStatic;
   CComboBox m_apertureTypeComboBox;
   CStatic m_apertureUnitsStatic;
   CStandardApertureViewStatic m_genericApertureViewStatic;
   CStandardApertureViewStatic m_apertureViewStatic;
   CStatic m_originGroupBox;
   CComboBox m_originOptionsComboBox;
   CButton m_displayCenterAnnotationsCheckBox;

public:
   CStandardApertureDialogParameters& getParameters() { return m_parameters; }
   CStandardApertureDialogParameter& getParameter(int index) { return *(m_parameters.getAt(index)); }

   const CStandardApertureShape& getStandardApertureShape();
   CString getApertureDescriptor() const;
   void setInitialDescriptor(const CString& descriptor) { m_initialDescriptor = descriptor; }
   PageUnitsTag getPageUnits() const { return m_pageUnits; }
   bool getDisplayCenterAnnotations() const { return (m_displayCenterAnnotationsCheckBox.GetCheck() != 0); }

   virtual CString GetDialogProfileEntry() { return CString("StandardApertureDialog"); }

private:
   void setApertureType(StandardApertureTypeTag apertureType);
   bool setApertureFromDescriptor(const CString& descriptor);
   void setStandardApertureShape(CStandardApertureShape* apertureShape);
   void setGenericStandardApertureShape(CStandardApertureShape* apertureShape);

   void drawApertureView();
   void updateParameters();

public:
   virtual BOOL OnInitDialog();
   afx_msg void OnEditChangeApertureTypeComboBox();
   afx_msg void OnSelChangeApertureTypeComboBox();
   afx_msg void OnEnKillFocusParameterValue0();
   afx_msg void OnEnKillFocusParameterValue1();
   afx_msg void OnEnKillFocusParameterValue2();
   afx_msg void OnEnKillFocusParameterValue3();
   afx_msg void OnEnKillFocusParameterValue4();
   afx_msg void OnEnKillFocusParameterValue5();
   afx_msg void OnEnKillFocusDescriptor();
   afx_msg void OnBnClickedDisplayCenterAnnotations();
   afx_msg void OnCbnSelchangeOriginOptions();
};

#endif



