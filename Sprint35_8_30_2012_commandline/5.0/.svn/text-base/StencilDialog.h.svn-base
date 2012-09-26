// $Header: /CAMCAD/4.6/StencilDialog.h 93    6/01/07 6:55a Kurt Van Ness $

#if ! defined (__StencilDialog_h__)
#define __StencilDialog_h__

#pragma once

#include "TreeListFrame.h"
#include "Resource.h"
#include "StencilGenerator.h"
#include "AfxWin.h"
#include "StaticLabel.h"
#include "ResizingDialog.h"
#include "ResizingPropertySheet.h"
#include "listcontrol.h"
#include "StandardApertureDialog.h" 
#include "ColorEditBox.h" 

class CCamCadDatabase;
class CStencilGeneratorPropertySheet;
class CStencilGenerator;
class CWriteFormat;
class CHtmlFileWriteFormat;
class CMapSortStringToOb;

#define BrowseValidationErrors 1234
#define HidePropertySheet      1235

#define WM_StencilPinChanged (WM_USER + 10)

enum ValidationStatusTag
{
   validationStatusNoStencils,
   validationStatusStale,
   validationStatusUpToDate,
   validationStatusUndefined
};

enum RuleAssignmentTypeTag
{
   ruleAssignmentTypeImplicitlyInherited,
   ruleAssignmentTypeExplicitlyInherited,
   ruleAssignmentTypeAssigned,
   ruleAssignmentTypeUndefined
};

//_____________________________________________________________________________
class CClipboard
{
public:
   static bool putText(const CString& text);
   static bool getText(CString& text);
};

//_____________________________________________________________________________
class CStandardApertureParameterEdit : public CEdit
{
public:
   // message handlers
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

//_____________________________________________________________________________
class CEditDouble : public CColorEditBox
{
public:
   CString getStringValue();
   CString getDoubleStringValue(double defaultValue);
   double getDoubleValue();
   double getDoubleValue(double defaultValue);
   void setDoubleValue(const CString& doubleString,double defaultValue);
   void setDoubleValue(double value);
};

//_____________________________________________________________________________
class CStencilRuleStaticLabel : public CStaticLabel
{
public:
   void setStatus(StencilRuleStatusTag status);
};

//_____________________________________________________________________________
class CColoredButton : public CButton
{
// Construction
public:
   CColoredButton();

// Attributes
public:

// Operations
public:
   void SetColor(COLORREF text_colour);
   void SetColor(COLORREF text_colour,COLORREF background_colour);
   void SetColor(COLORREF text_colour,COLORREF background_colour,COLORREF disabled_background_colour);
   void setTextBackgroundColor(COLORREF new_background_colour);

   void ResetColor();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CColoredButton)
   public:
   virtual  void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual  ~CColoredButton();

private:
   COLORREF text_colour, background_colour, disabled_background_colour;
   COLORREF light,   highlight, shadow, dark_shadow;

   // Generated message map functions
protected:
   //{{AFX_MSG(CColoredButton)
      // NOTE  - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   void DrawFrame(CDC *dc, CRect r, int state);
   void DrawFilledRect(CDC *dc, CRect r, COLORREF colour);
   void DrawLine(CDC *dc, long   sx,   long sy, long ex, long ey, COLORREF colour);
   void DrawButtonText(CDC *dc, CRect r, const  char *buf, COLORREF  text_colour);

   DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
struct CSxLogFont : public LOGFONT
{
public:
//
// constructor 
//
   CSxLogFont
   (
      LONG fontHeight = 80, 
      LONG fontWeight = FW_NORMAL,
      BYTE fontItalic = false,
      LPCTSTR faceName = _T("MS Sans Serif")
   )
   {
      // clear the current object
      memset(this, 0, sizeof(*this));

      // store the height, weight, and italics
      lfHeight = fontHeight;
      lfWeight = fontWeight;
      lfItalic = fontItalic;

      // make sure that font name is not too long
      ASSERT( _tcslen(faceName) < LF_FACESIZE );
      _tcscpy(lfFaceName, faceName);

      // set other default parameters
      lfOutPrecision   = OUT_STROKE_PRECIS;
      lfClipPrecision  = CLIP_STROKE_PRECIS;
      lfQuality        = PROOF_QUALITY;
      lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

   }
//
// sets a new font name
//
   void SetFaceName(LPCTSTR faceName)
   {
      if(!faceName) return;

      ASSERT(_tcslen(faceName) < LF_FACESIZE );
      lstrcpy(lfFaceName, faceName);       
   }
//
// sets a new font rotation angle
//
   int SetFontAngle(int angle)
   {
      int oldangle = lfEscapement;
      lfEscapement = angle;
      return oldangle;
   }
//
// sets the underline style
//
   int SetUnderline(int state)
   {
      int oldstate = lfUnderline;
      lfUnderline = state;
      return oldstate;
   }
//
// sets the strikeout style
//
   int SetStrikeOut(int state)
   {
      int oldstate = lfStrikeOut;
      lfStrikeOut = state;
      return oldstate;
   }
};

//_____________________________________________________________________________
#define BS_3D     11
#define BS_RECT   12
#define BS_DEFAULT BS_RECT

class CGroupBox : public CButton
{
public:
   CGroupBox();
   virtual ~CGroupBox();

   void setBoxColor(COLORREF primaryColor);
   void SetBoxColors(COLORREF color1 = NULL, 
      COLORREF color2 = NULL, bool redraw = false);
   void GetBoxColors(COLORREF& color1, COLORREF& color2);

   void SetText(CString textStr, bool redraw = false);
   CString GetText()
      {return m_txtString;};

   void SetTextColor(COLORREF color, bool redraw = false);
   COLORREF GetTextColor()
      {return m_txtColor;};

   void SetFont(const LOGFONT* lpLogFont = (LOGFONT*)NULL);
   CFont* GetFont()
      {return m_txtFont;};

   void SetTextAlign(int TextAlign);
   int GetTextAlign()
      {return m_txtAlignment;};

   void SetTextOffset(int Voffset, int Hoffset);

   void SetLineStyle(int StyleLine = BS_DEFAULT);
   int GetLineStyle()
      {return m_LineStyle;};

   void SetLineThickness(int thickness, bool redraw = false);
   int GetLineThickness()
      {return m_boxThickness;};

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGroupBox)
   protected:
   virtual void PreSubclassWindow();
   //}}AFX_VIRTUAL

private:
   COLORREF m_boxColor1;
   COLORREF m_boxColor2;
   COLORREF m_boxColor1_d;
   COLORREF m_boxColor2_d;
   bool m_StyleBitmap;
   int m_boxThickness;

   COLORREF m_txtColor;
   CFont *m_txtFont;
   CString m_txtString;
   int m_txtAlignment;    // text alignment (BS_LEFT, BS_CENTER, BS_RIGHT)
   int m_txtHOffset;      // horizontal distance from left/right side to beginning/end of text
   int m_txtVOffset;      // vertical distance to move font up
   int m_LineStyle;       // line style (BS_FLAT, BS_3D, BS_RECT)

   CRect m_boxRect;
   CRect m_thinBoxRect;

   //{{AFX_MSG(CGroupBox)
   afx_msg void OnPaint();
   afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
class CStandardApertureGeometryViewStatic : public CStandardApertureViewStatic
{
	DECLARE_DYNAMIC(CStandardApertureGeometryViewStatic)

protected:
   CExtent m_extent;

public:
	CStandardApertureGeometryViewStatic();
	virtual ~CStandardApertureGeometryViewStatic();

   virtual CExtent getExtent();
   //virtual void Draw(CDC* dc);

protected:
	//DECLARE_MESSAGE_MAP()

public:
   virtual void setGeometryMicrocosm(CStencilGeometryMicrocosm& geometryMicrocosm,bool placedTopFlag);

protected:
   virtual void flattenAndTransmute(CDataList& targetDataList,BlockStruct& sourceGeometry,
      CCamCadDatabase& sourceCamCadDatabase,int targetLayerIndex,CTMatrix matrix);
};

//_____________________________________________________________________________
class CStandardAperturePinViewStatic : public CStandardApertureGeometryViewStatic
{
	DECLARE_DYNAMIC(CStandardAperturePinViewStatic)

private:
   CExtent m_copperExtent;

public:
	CStandardAperturePinViewStatic();
	virtual ~CStandardAperturePinViewStatic();

   //virtual CExtent getExtent();

protected:
	//DECLARE_MESSAGE_MAP()

public:
   CExtent getCopperExtent() const { return m_copperExtent; }

public:
   virtual void setGeometryMicrocosm(CStencilGeometryMicrocosm& geometryMicrocosm,bool placedTopFlag);
   virtual void Draw(CDC* dc);
   virtual void setStandardApertureShape(CStandardApertureShape& standardApertureShape);

private:
   //virtual void flattenAndTransmute(CDataList& targetDataList,BlockStruct& sourceGeometry,
   //   CCamCadDatabase& sourceCamCadDatabase,int targetLayerIndex,CTMatrix matrix);
};

//_____________________________________________________________________________
class CStencilRuleComboBox : public CComboBox
{
public:
   CStencilRuleComboBox();
   virtual ~CStencilRuleComboBox();

   void initialize();

   StencilRuleTag getSelected();
   void setSelected(StencilRuleTag ruleTag);
};

//_____________________________________________________________________________
class CStencilRuleModifierComboBox : public CComboBox
{
public:
   CStencilRuleModifierComboBox();
   virtual ~CStencilRuleModifierComboBox();

   void initialize(StencilRuleTag ruleTag);

   StencilRuleModifierTag getSelected();
   void setSelected(StencilRuleModifierTag modifierTag);
};

//_____________________________________________________________________________
class CStencilRuleValueComboBox : public CComboBox
{
protected:
   CString m_currentEditBoxText;
   bool m_enableEditing;
   bool m_editFlag;

public:
   CStencilRuleValueComboBox();
   virtual ~CStencilRuleValueComboBox();

   //virtual void setSelectedText(const CString& text);
   virtual bool getEditFlag() { return m_editFlag; }
   virtual CString getCurrentEditBoxText() { return m_currentEditBoxText; }
   void initialize(StencilRuleTag ruleTag);

   CString getSelected();
   void setSelected(const CString& value);

   void setEnableEditing(bool flag);

   //{{AFX_MSG(CStencilRuleValueComboBox)
   //virtual afx_msg void   OnCloseUp();
   //virtual afx_msg void   OnKillFocus();
   virtual afx_msg void   OnEditUpdate();
   virtual afx_msg void   OnSelCancel();
   virtual afx_msg void   OnSelChange();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   afx_msg void OnCbnSetfocus();

private:
   void clearEditFlag();
   void setEditFlag(bool flag);
};

//_____________________________________________________________________________
class CTreeListStencilRuleComboBox : public CTreeListComboBox
{
public:
   CTreeListStencilRuleComboBox(int column);
   virtual ~CTreeListStencilRuleComboBox();

   virtual CTreeListComboBox* create(CRect rect,CWnd* parentWnd);
   virtual void setSelectedText(const CString& text);
   virtual afx_msg void   OnCloseUp();
   virtual afx_msg void   OnSelCancel();
};

//_____________________________________________________________________________
class CTreeListStencilRuleModifierComboBox : public CTreeListStencilRuleComboBox
{
public:
   CTreeListStencilRuleModifierComboBox(int column);
   virtual ~CTreeListStencilRuleModifierComboBox();

   virtual CTreeListComboBox* create(CRect rect,CWnd* parentWnd);
   //virtual void setSelectedText(const CString& text);
   virtual afx_msg void   OnCloseUp();
   //virtual afx_msg void   OnSelCancel();
};

//_____________________________________________________________________________
class CTreeListAttributeValueComboBox : public CTreeListComboBox
{
private:
   static CTypedObArrayContainer<CMapSortStringToOb*> m_attributeValuesArray;

public:
   CTreeListAttributeValueComboBox(int column);
   virtual ~CTreeListAttributeValueComboBox();

   //virtual CTreeListComboBox* create(CRect rect,CWnd* parentWnd);
   virtual void setSelectedText(const CString& text);
   //virtual afx_msg void OnCloseUp();
   static void addAttributeValue(StencilRuleTag stencilRule,const CString& attributeValue);
   static void clearAttributeValues();
   static CMapSortStringToOb& getValues(StencilRuleTag ruleTag);
};

//_____________________________________________________________________________
class CTreeListStencilRuleValueComboBox : public CTreeListAttributeValueComboBox
{
public:
   CTreeListStencilRuleValueComboBox(int column);
   virtual ~CTreeListStencilRuleValueComboBox();

   virtual CTreeListComboBox* create(CRect rect,CWnd* parentWnd);
   //virtual void setSelectedText(const CString& text);
   virtual afx_msg void OnCloseUp();
};

//_____________________________________________________________________________
class CStencilTreeListItem : public CTreeListItem
{
private:
   AttributeSourceTag m_sourceTag;
   bool m_topFlag;
   //int m_subPinIndex;

public:
   CStencilTreeListItem();
   CStencilTreeListItem(const CTreeListItem& other);
   CStencilTreeListItem(AttributeSourceTag sourceTag,bool topFlag);

   AttributeSourceTag getAttributeSource() const;
   bool getTopFlag() const;
   //int getSubPinIndex() const;

   virtual CString GetSubstring(CTreeListCtrl& treeListCtrl,HTREEITEM item,int m_nSub) const;
   virtual CStencilSubPin* getSubPinItemData() const;
};

//_____________________________________________________________________________
class CStencilTreeListCtrl : public CTreeListCtrl
{
private:
   CWnd& m_parent;

   int m_pcbIconIndex;           // pcb
   int m_surfaceIconIndex;       // surface
   int m_geomIconIndex;          // component
   int m_padIconIndex;           // padstack
   int m_viaIconIndex;           // via
   int m_fiducialIconIndex;      // fiducial
   int m_compIconIndex;          // refdes
   int m_pinIconIndex;           // pin
   int m_subPinIconIndex;        // subpin
   int m_pinInstanceIconIndex;   // pinInstance
   int m_smdThIconIndex;         // smd/th

   int m_pcbIconAIndex;          // pcb
   int m_surfaceIconAIndex;      // surface
   int m_geomIconAIndex;         // component
   int m_padIconAIndex;          // padstack
   int m_viaIconAIndex;          // via
   int m_fiducialIconAIndex;     // fiducial
   int m_compIconAIndex;         // refdes
   int m_pinIconAIndex;          // pin
   int m_subPinIconAIndex;       // subpin
   int m_pinInstanceIconAIndex;  // pinInstance
   int m_smdThIconAIndex;        // smd/th

   int m_pcbIconBIndex;          // pcb
   int m_surfaceIconBIndex;      // surface
   int m_geomIconBIndex;         // component
   int m_padIconBIndex;          // padstack
   int m_viaIconBIndex;          // via
   int m_fiducialIconBIndex;     // fiducial
   int m_compIconBIndex;         // refdes
   int m_pinIconBIndex;          // pin
   int m_subPinIconBIndex;       // subpin
   int m_pinInstanceIconBIndex;  // pinInstance
   int m_smdThIconBIndex;        // smd/th

public:
   CStencilTreeListCtrl(CWnd& parent);
   virtual ~CStencilTreeListCtrl();

   CWnd& getParent() { return m_parent; }

   int getPcbIconIndex()             { return m_pcbIconIndex;           }
   int getSurfaceIconIndex()         { return m_surfaceIconIndex;       }
   int getGeomIconIndex()            { return m_geomIconIndex;          }
   int getPadIconIndex()             { return m_padIconIndex;           }
   int getViaIconIndex()             { return m_viaIconIndex;           }
   int getFiducialIconIndex()        { return m_fiducialIconIndex;      }
   int getCompIconIndex()            { return m_compIconIndex;          }
   int getPinIconIndex()             { return m_pinIconIndex;           }
   int getSubPadIconIndex()          { return m_subPinIconIndex;        }
   int getPinInstanceIconIndex()     { return m_pinInstanceIconIndex;   }
   int getSubPinInstanceIconIndex()  { return m_pinInstanceIconIndex;   }
   int getSmdThIconIndex()           { return m_smdThIconIndex;         }

   int getPcbIconAIndex()            { return m_pcbIconAIndex;          }
   int getSurfaceIconAIndex()        { return m_surfaceIconAIndex;      }
   int getGeomIconAIndex()           { return m_geomIconAIndex;         }
   int getPadIconAIndex()            { return m_padIconAIndex;          }
   int getViaIconAIndex()            { return m_viaIconAIndex;          }
   int getFiducialIconAIndex()       { return m_fiducialIconAIndex;     }
   int getCompIconAIndex()           { return m_compIconAIndex;         }
   int getPinIconAIndex()            { return m_pinIconAIndex;          }
   int getSubPadIconAIndex()         { return m_subPinIconAIndex;       }
   int getPinInstanceIconAIndex()    { return m_pinInstanceIconAIndex;  }
   int getSubPinInstanceIconAIndex() { return m_pinInstanceIconAIndex;  }
   int getSmdThIconAIndex()          { return m_smdThIconAIndex;        }

   int getPcbIconBIndex()            { return m_pcbIconBIndex;          }
   int getSurfaceIconBIndex()        { return m_surfaceIconBIndex;      }
   int getGeomIconBIndex()           { return m_geomIconBIndex;         }
   int getPadIconBIndex()            { return m_padIconBIndex;          }
   int getViaIconBIndex()            { return m_viaIconBIndex;          }
   int getFiducialIconBIndex()       { return m_fiducialIconBIndex;     }
   int getCompIconBIndex()           { return m_compIconBIndex;         }
   int getPinIconBIndex()            { return m_pinIconBIndex;          }
   int getSubPadIconBIndex()         { return m_subPinIconBIndex;       }
   int getPinInstanceIconBIndex()    { return m_pinInstanceIconBIndex;  }
   int getSubPinInstanceIconBIndex() { return m_pinInstanceIconBIndex;  }
   int getSmdThIconBIndex()          { return m_smdThIconBIndex;        }

   void setImageList();

   virtual CTreeListItem* newTreeListItem();
   virtual CTreeListItem* newTreeListItem(const CTreeListItem& other);
   virtual CTreeListItem* newTreeListItem(AttributeSourceTag sourceTag,bool topFlag);
   virtual HTREEITEM InsertItem(LPCTSTR lpszItem,AttributeSourceTag sourceTag,bool topFlag,
      int nImage,int nSelectedImage,HTREEITEM hParent = TVI_ROOT,HTREEITEM hInsertAfter = TVI_LAST);
   virtual bool setSubPinItemData(HTREEITEM item,CStencilSubPin* subPin);
   virtual CStencilSubPin* getSubPinItemData(HTREEITEM item);

   virtual void expandAll(HTREEITEM item);
   virtual int collapseLowerHierarchies(HTREEITEM item);
   virtual int expandToShowRules(HTREEITEM item);
   virtual void expandAll();
   virtual void collapseLowerHierarchies();
   virtual void expandToShowRules();
   virtual void cutRule();
   virtual void copyRule(bool removeFlag=false);
   virtual void pasteRule();

protected:
   //{{AFX_MSG(CStencilTreeListCtrl)
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnExpandAll();
   afx_msg void OnCollapseLowerHierarchies();
   afx_msg void OnExpandToShowRules();
   afx_msg void OnCutRule();
   afx_msg void OnCopyRule();
   afx_msg void OnPasteRule();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
class CStencilTreeListFrame : public CTreeListFrame
{
private:
   CWnd& m_parent;

public:
   CStencilTreeListFrame(CWnd& parent);

   virtual CTreeListCtrl& getTree();
   virtual CWnd& getParent();

   CStencilTreeListCtrl& getStencilTree() { return (CStencilTreeListCtrl&)getTree(); }

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

//_____________________________________________________________________________
class CLayerComboBox : public CComboBox
{
private:
   CCamCadDatabase& m_camCadDatabase;

public:
   CLayerComboBox(CCamCadDatabase& camCadDatabase);
   void init();

   LayerStruct* getSelectedLayer();
   bool setSelectedLayer(const CString& layerName);
};

//_____________________________________________________________________________
class CStencilConfigurationPropertyPage : public CResizingPropertyPage
{
	DECLARE_DYNAMIC(CStencilConfigurationPropertyPage)

private:
   CEdit m_topStencilThicknessEditBox;
   CEdit m_bottomStencilThicknessEditBox;
   CEdit m_topMaxFeatureEditBox;
   CEdit m_bottomMaxFeatureEditBox;
   CEdit m_topWebWidthEditBox;
   CEdit m_bottomWebWidthEditBox;
   CEdit m_topMinPinPitchEditBox;
   CEdit m_bottomMinPinPitchEditBox;
   CEdit m_mountTechAttribNameEditBox;
   CEdit m_mountTechAttribSMDValueEditBox;
   CButton m_topPasteInHoleCheckBox;
   CButton m_bottomPasteInHoleCheckBox;
   CButton m_topPasteInViaCheckBox;
   CButton m_bottomPasteInViaCheckBox;
   CButton m_useMountTechAttribCheckBox;
   CEdit m_exteriorCornerRadiusEditBox;
   CLayerComboBox m_topStencilSourceLayerComboBox;
   CLayerComboBox m_bottomStencilSourceLayerComboBox;
   int m_chkUseDatabaseConnection;

   void setEditBoxText(CEdit& editBox,StencilAttributeTag attributeTag,double defaultValue);
   bool checkStencilSourceLayers();
   void enableMountTechAttribControls(bool flag);

   void EnableDatabaseControls(bool attempt, bool allowed);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	CStencilConfigurationPropertyPage(CStencilGenerationParameters& stencilGenerationParameters);
	virtual ~CStencilConfigurationPropertyPage();

   CStencilGeneratorPropertySheet* getParent()
      {  return (CStencilGeneratorPropertySheet*)GetParent(); }

   // Dialog Data
	enum { IDD = IDD_StencilPcbPage };

   CStencilGenerationParameters& getStencilGenerationParameters();

   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();

   afx_msg void OnBnClickedBrowseButton();
   afx_msg void OnBnClickedChkUseDatabaseConnection();

   afx_msg void OnKillFocusTopStencilThicknessEditBox();
   afx_msg void OnKillFocusBottomStencilThicknessEditBox();
   afx_msg void OnKillFocusTopMaxFeatureEditBox();
   afx_msg void OnKillFocusBottomMaxFeatureEditBox();
   afx_msg void OnKillFocusTopWebWidthEditBox();
   afx_msg void OnKillFocusBottomWebWidthEditBox();
   afx_msg void OnKillFocusMountTechAttribNameEditBox();
   afx_msg void OnKillFocusMountTechAttribSMDValueEditBox();
   afx_msg void OnBnClickedTopPasteInHoleCheckBox();
   afx_msg void OnBnClickedPropagateRulesCheckBox();
   afx_msg void OnBnClickedUseMountTechAttribCheckBox();
   afx_msg void OnBnClickedBottomPasteInHoleCheckBox();
   afx_msg void OnKillFocusExteriorCornerRadiusEditBox();
   afx_msg void OnSelChangeTopStencilSourceLayer();
   afx_msg void OnSelChangeBottomStencilSourceLayer();

   afx_msg void OnBnClickedSaveSettings();
   afx_msg void OnBnClickedLoadSettings();
   afx_msg void OnBnClickedMergeSettings();
   afx_msg void OnBnClickedRemoveAllRules();
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg void OnBnClickedTopPasteInViaCheckBox();
   afx_msg void OnBnClickedBottomPasteInViaCheckBox();
   afx_msg void OnBnClickedSaveSettingsToDatabase();
   afx_msg void OnBnClickedLoadSettingsFromDatabase();
   afx_msg void OnEnChangeEdit1();

   DECLARE_MESSAGE_MAP()

};

//_____________________________________________________________________________
class CStencilGeomPadPropertyPage : public CResizingPropertyPage
{
	DECLARE_DYNAMIC(CStencilGeomPadPropertyPage)

private:
   CStencilTreeListFrame m_treeListFrame;
   //bool m_treeInitialized;
   bool m_fillingTreeFlag;

public:
	CStencilGeomPadPropertyPage();
	virtual ~CStencilGeomPadPropertyPage();

   CStencilGeneratorPropertySheet* getParent() const
      {  return (CStencilGeneratorPropertySheet*)GetParent(); }

   CStencilTreeListCtrl& getTreeListControl() { return m_treeListFrame.getStencilTree(); }
   CStencilGenerator& getStencilGenerator();

private:
   void fillTree();

public:
   void restoreColumnWidths();
   void saveColumnWidths();

   // Dialog Data
	enum { IDD = IDD_StencilGeomPadPage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   afx_msg void OnBnClickedExpandAll();
   afx_msg void OnBnClickedCollapseAll();
   afx_msg void OnBnClickedShowRules();
   afx_msg void OnBnClickedRemoveAllRules();
};

//_____________________________________________________________________________
class CStencilCompPinPropertyPage : public CResizingPropertyPage
{
	DECLARE_DYNAMIC(CStencilCompPinPropertyPage)

private:
   CStencilTreeListFrame m_treeListFrame;
   //bool m_treeInitialized;
   bool m_fillingTreeFlag;

public:
	CStencilCompPinPropertyPage();
	virtual ~CStencilCompPinPropertyPage();

   CStencilGeneratorPropertySheet* getParent() const
      {  return (CStencilGeneratorPropertySheet*)GetParent(); }

   CStencilTreeListCtrl& getTreeListControl() { return m_treeListFrame.getStencilTree(); }

private:
   void fillTree();

public:
   void restoreColumnWidths();
   void saveColumnWidths();

// Dialog Data
	enum { IDD = IDD_StencilCompPinPage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   afx_msg void OnBnClickedExpandAll();
   afx_msg void OnBnClickedCollapseAll();
   afx_msg void OnBnClickedShowRules();
   afx_msg void OnBnClickedRemoveAllRules();
};

//_____________________________________________________________________________
class CStencilValidationRulesPropertyPage : public CResizingPropertyPage
{
	DECLARE_DYNAMIC(CStencilValidationRulesPropertyPage)

private:
   //CStencilValidationParameters m_stencilValidationParameters;  // local to property page
   bool m_autoOpenFlag;

public:
	CStencilValidationRulesPropertyPage(CStencilValidationParameters& stencilValidationParameters);
	virtual ~CStencilValidationRulesPropertyPage();

   CStencilGeneratorPropertySheet* getParent()
      {  return (CStencilGeneratorPropertySheet*)GetParent(); }

// Dialog Data
	enum { IDD = IDD_StencilRulePage };

public:
   CStencilValidationParameters& getStencilValidationParameters();
   void checkPinReportFile();
   bool getAutoOpenLogFile() { return m_autoOpenFlag; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnKillActive();

	DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();

   CEdit m_holeToWallAreaEditBox;
   CEdit m_minFeatureEditBox;
   CEdit m_maxFeatureEditBox;
   //CEdit m_webWidthEditBox;
   CEdit m_minInsetEditBox;
   CStaticLabel m_generationStatusStatic;
   CStaticLabel m_validationStatusStatic;
   CStaticLabel m_reportStatusStatic;
   CStaticLabel m_validatationErrorCountStatic;
   CButton m_autoOpenLogFileCheckBox;

   afx_msg void OnBnClickedSetToDefaultsButton();
   afx_msg void OnBnClickedProcessValidationRules();
   afx_msg void OnBnClickedBrowseValidationErrors();
   afx_msg void OnBnClickedDisplayPinReportsButton();
   afx_msg void OnBnClickedGenerateStencils();
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg void OnBnClickedAutoOpenLogFile();

   afx_msg void OnEnKillFocusHoleToWallAreaEditBox();
   afx_msg void OnEnKillFocusMinFeatureEditBox();
   afx_msg void OnEnKillFocusMaxFeatureEditBox();
   afx_msg void OnEnKillFocusMinInsetEditBox();
};

//_____________________________________________________________________________
class CStencilRulePreviewPage : public CResizingPropertyPage
{
	DECLARE_DYNAMIC(CStencilRulePreviewPage)

private:
   static const COLORREF m_inheritedColor = colorHtmlPaleGreen;
   static const COLORREF m_explicitlyInheritedColor = colorBlue;
   static const COLORREF m_assignedColor  = colorHtmlTomato;

   CStandardApertureDialogParameters m_parameters;
   CStandardApertureShape* m_standardApertureShape;
   CStandardApertureShape* m_genericStandardApertureShape;

   HTREEITEM m_selectedTreeItem;
   HTREEITEM m_lockedTreeItem;
   HTREEITEM m_lockSelectedTreeItem;
   bool m_treeItemLockedFlag;
   //CStencilPin* m_selectedStencilPin;
   CStencilSubPin* m_selectedStencilSubPin;
   bool m_selectStencilPinTopFlag;
   //int m_selectStencilPinHoleIndex;
   AttributeSourceTag m_selectedStencilPinAttributeSource;
   bool m_enableGeometryViewFlag;
   bool m_stencilRuleChangedFlag;
   bool m_updatingDataFlag;
   bool m_geometryPlacedTopFlag;
   //bool m_assignedFlag;
   RuleAssignmentTypeTag m_ruleAssignmentType;
   int m_stencilRuleComboBoxSelectedIndex;
   CString m_savedEditBoxContents;

   BlockStruct* m_componentViewGeometry;

   //
   CString m_initialDescriptor;
   StandardApertureTypeTag m_initialApertureType;
   CenterTag m_center;

   // controls

   // aperture parameters controls
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

   // shape rule controls
   CGroupBox m_stencilRuleGroup;
   CColoredButton m_assignShapeRuleButton;
   CColoredButton m_inheritShapeRuleButton;
   CStencilRuleComboBox m_stencilRuleComboBox;
   CStencilRuleValueComboBox m_stencilRuleValueComboBox;
   CEdit m_commentEditBox;

   // symmetry axis controls
   CGroupBox m_symmetryAxisGroup;
   CButton m_flippedCheckBox;

   // orientation controls
   CGroupBox m_orientationGroup;
   CButton m_orientTowardCenterRadioButton;
   CButton m_orientAwayFromCenterRadioButton;
   CButton m_orientNormalRadioButton;

   // offset controls
   CGroupBox m_offsetGroup;
   CButton m_offsetTowardCenterRadioButton;
   CButton m_offsetAwayFromCenterRadioButton;
   CButton m_offsetNormalRadioButton;
   CEditDouble m_xOffsetEditBox;
   CEditDouble m_yOffsetEditBox;
   CEditDouble m_angleOffsetEditBox;
   CStatic m_xOffsetStatic;
   CStatic m_yOffsetStatic;
   CStatic m_angleOffsetStatic;

   // split controls
   CGroupBox m_splitGroup;
   CButton m_splitGridRadioButton;
   CButton m_splitStripeRadioButton;
   CEditDouble m_splitMaxFeatureEditBox;
   CEditDouble m_splitWebWidthEditBox;
   CEditDouble m_splitWebAngleEditBox;
   CStatic m_splitMaxFeatureStatic;
   CStatic m_splitWebWidthStatic;
   CStatic m_splitWebAngleStatic;

   // exterior corner radius controls
   CGroupBox m_exteriorCornerRadiusGroup;
   CStatic m_exteriorCornerRadiusStatic;
   CEditDouble m_exteriorCornerRadiusEditBox;

   // origin controls
   CStatic m_originGroupBox;
   CComboBox m_originOptionsComboBox;
   CButton m_displayCenterAnnotationsCheckBox;

   // copper pad extents controls
   CStatic m_copperPadExtentsGroup;
   CStatic m_copperPadExtentsStatic;

   // pin pitch attribute controls
   CStatic m_geometryPinPitchGroup;
   CStatic m_geometryPinPitchStatic;

   // tree controls
   CStencilRuleStaticLabel m_ruleStatusStatic;
   CButton m_applyButton2;

   // view controls
   CStandardApertureViewStatic m_genericApertureViewStatic;
   CStandardAperturePinViewStatic m_apertureViewStatic;
   CStandardApertureGeometryViewStatic m_geometryViewStatic;

   CStatic m_geometryNameStatic;
   CStatic m_apertureUnitsStatic;
   CStatic m_genericDescriptorStatic;
   CButton m_geometryPlacedTopBottomButton;

   // other controls
   CStatic m_descriptorEditBox;
   CComboBox m_apertureTypeComboBox;
   CButton m_applyButton;
   CButton m_swapWidthWithHeightButton;
   CEdit m_inheritedRuleEditBox;

   // obsolete controls
   //CStatic m_selectedEntityStatic;
   //CStatic m_inheritedRuleStatic;
   //CStatic m_stencilRuleModifierStatic;
   //CStencilRuleModifierComboBox m_stencilRuleModifierComboBox;
   //CStatic m_stencilRuleValueStatic;

protected:
   CStencilTreeListFrame m_treeListFrame;
   bool m_fillingTreeFlag;
   int m_treeRevLevel; // used to match against master SMD List rev level, to keep tree in sync

public:
	CStencilRulePreviewPage();
	virtual ~CStencilRulePreviewPage();

   CStencilGeneratorPropertySheet* getParent() const
      {  return (CStencilGeneratorPropertySheet*)GetParent(); }
   CStencilGenerator& getStencilGenerator();

   CStencilTreeListCtrl& getTreeListControl() { return m_treeListFrame.getStencilTree(); }
   CCamCadDatabase& getCamCadDatabase();

   HTREEITEM findTreeItem(AttributeSourceTag itemSource, bool topFlag, CString &itemText, CString &compGeomName, CString &pinGeomName, CString &pinName);
   HTREEITEM findTreeItem(HTREEITEM rootItem, AttributeSourceTag itemSource, bool topFlag, CString& itemText, CString& compGeomName, CString& pinGeomName, CString& pinName);

   CStandardApertureDialogParameters& getParameters() { return m_parameters; }
   CStandardApertureDialogParameter& getParameter(int index) { return *(m_parameters.getAt(index)); }

   const CStandardApertureShape& getStandardApertureShape();
   CString getApertureDescriptor() const;
   void setInitialDescriptor(const CString& descriptor) { m_initialDescriptor = descriptor; }
   PageUnitsTag getPageUnits() const;
   bool getDisplayCenterAnnotations() const { return (m_displayCenterAnnotationsCheckBox.GetCheck() != 0); }

   //void selectStencilPin();
   //void selectStencilPin(HTREEITEM treeItem);

   static COLORREF getInheritedColor()           { return m_inheritedColor; }
   static COLORREF getExplicitlyInheritedColor() { return m_explicitlyInheritedColor; }
   static COLORREF getAssignedColor()            { return m_assignedColor; }
   static COLORREF getButtonColor()              { return GetSysColor(COLOR_BTNFACE); }
   static COLORREF getAssignedInheritedColor(bool assignedFlag)  { return (assignedFlag ? m_assignedColor : m_inheritedColor); }
   static COLORREF getAssignedInheritedColor(RuleAssignmentTypeTag ruleAssignmentType);

private:
   void setApertureType(StandardApertureTypeTag apertureType);
   StandardApertureTypeTag setApertureTypeFromDescriptor(const CString& descriptor);
   bool setApertureFromDescriptor(const CString& descriptor);
   void setStandardApertureShape(const CStandardApertureShape& apertureShape);
   void setGenericStandardApertureShape(const CStandardApertureShape& apertureShape);
   StencilRuleStatusTag setComponentViewGeometry(BlockStruct* componentGeometry,
      CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,bool placedTopFlag,AttributeSourceTag attributeSource);
   StencilRuleStatusTag setComponentViewComponent(DataStruct* componentViewComponent,
      CStencilSubPin& sourceStencilSubPin,bool sourceTopFlag,bool placedTopFlag,AttributeSourceTag attributeSource);

   //void drawApertureView();
   void updateParameters();
   void updateStencilPinAttributeDataControls();
   bool isCurrentRuleValid();

   void propagateCurrentRuleToOppositeSide();

   void onStencilControlChanged();
   void onKillFocusStencilControlEditBox(CEdit& editBox);
   void onSetFocusStencilControlEditBox(CEdit& editBox);
   bool getItemLocked() const;
   void setItemLocked(bool flag);
   HTREEITEM getLockSelectedItem() const;
   void setLockSelectedItem(HTREEITEM item);
   CString getSavedEditBoxContents() const;
   void setSavedEditBoxContents(const CString& contentsString);

   StencilOrientationModifierTag getOrientation() const;
   void setOrientation(StencilOrientationModifierTag orientation);
   StencilOrientationModifierTag getOffsetOrientation() const;
   void setOffsetOrientation(StencilOrientationModifierTag orientation);
   StencilSplitTypeTag getSplitType() const;
   void setSplitType(StencilSplitTypeTag splitType);

   bool isRuleInherited() const;
   bool isRuleImplicitlyInherited() const;
   bool isRuleExplicitlyInherited() const;
   bool isRuleAssigned() const;

   RuleAssignmentTypeTag getRuleAssignmentType() const;
   void setRuleAssignmentType(RuleAssignmentTypeTag ruleAssignmentType);
   COLORREF getRuleAssignmentTypeColor() const;

   int getImageIndex(HTREEITEM treeItem);
   int getIconIndexForSource(AttributeSourceTag attributeSource,bool viaFlag,bool fiducialFlag,int typeIndex);
   void setStencilRuleChanged();
   void invalidateSelectedTreeItem();

protected:
   virtual void fillTree();

// Dialog Data
	enum { IDD = IDD_StencilCompPinPagePreview };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();

   // temp
   virtual afx_msg void OnSize(UINT nType, int cx, int cy);

   // message handlers
   afx_msg void OnBnClickedExpandAll();
   afx_msg void OnBnClickedCollapseAll();
   afx_msg void OnBnClickedShowRules();
   afx_msg void OnBnClickedRemoveAllRules();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   afx_msg void OnCloseupStencilRulesComboBox();
   afx_msg void OnCloseupStencilRuleValuesComboBox();
   afx_msg void OnCloseupStencilRuleModifiersComboBox();
   afx_msg void OnDropdownStencilRulesComboBox();
   afx_msg void OnBnClickedSwapWidthWithHeight();
   afx_msg void OnBnClickedGeometryPlacedTopBottom();

   // standard aperture message handlers
   afx_msg void OnEditChangeApertureTypeComboBox();
   afx_msg void OnSelChangeApertureTypeComboBox();
   afx_msg void OnEnKillFocusParameterValue0();
   afx_msg void OnEnKillFocusParameterValue1();
   afx_msg void OnEnKillFocusParameterValue2();
   afx_msg void OnEnKillFocusParameterValue3();
   afx_msg void OnEnKillFocusParameterValue4();
   //afx_msg void OnEnKillFocusParameterValue5();
   //afx_msg void OnEnKillFocusDescriptor();
   afx_msg void OnBnClickedDisplayCenterAnnotations();
   afx_msg void OnCbnSelchangeOriginOptions();

   afx_msg void OnKillFocusStencilRuleValuesComboBox();
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg void OnBnClickedApply();

   afx_msg void OnKillFocusCommentEditBox();
   afx_msg void OnKillFocusXoffsetEditBox();
   afx_msg void OnKillFocusYoffsetEditBox();
   afx_msg void OnKillFocusAngleOffsetEditBox();
   afx_msg void OnKillFocusMaxFeatureEditBox();
   afx_msg void OnKillFocusWebWidthEditBox();
   afx_msg void OnKillFocusExteriorCornerRadiusEditBox();
   afx_msg void OnKillFocusWebAngleEditBox();

   afx_msg void OnSetFocusCommentEditBox();
   afx_msg void OnSetFocusXoffsetEditBox();
   afx_msg void OnSetFocusYoffsetEditBox();
   afx_msg void OnSetFocusAngleOffsetEditBox();
   afx_msg void OnSetFocusMaxFeatureEditBox();
   afx_msg void OnSetFocusWebWidthEditBox();
   afx_msg void OnSetFocusExteriorCornerRadiusEditBox();
   afx_msg void OnSetFocusWebAngleEditBox();

   afx_msg void OnBnClickedOrientTowardCenter();
   afx_msg void OnBnClickedOrientAwayFromCenter();
   afx_msg void OnBnClickedOrientNormal();
   afx_msg void OnBnClickedOffsetTowardCenter();
   afx_msg void OnBnClickedOffsetAwayFromCenter();
   afx_msg void OnBnClickedOffsetNormal();
   afx_msg void OnBnClickedFlippedCheckBox();
   afx_msg void OnBnClickedInheritShapeRule();
   afx_msg void OnBnClickedAssignShapeRule();

   // custom
   afx_msg LRESULT OnStencilPinChanged(WPARAM wParem,LPARAM lParam);
   afx_msg void OnBnClickedSplitGrid();
   afx_msg void OnBnClickedSplitStripe();
};

//_____________________________________________________________________________
class CStencilGeometryRulePreviewPage : public CStencilRulePreviewPage
{
	DECLARE_DYNAMIC(CStencilGeometryRulePreviewPage)

public:
   CStencilGeometryRulePreviewPage();
   virtual ~CStencilGeometryRulePreviewPage();

   virtual BOOL OnSetActive();

public: //protected:
   virtual void fillTree();
   

public:
   virtual afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

   DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
class CStencilComponentRulePreviewPage : public CStencilRulePreviewPage
{
	DECLARE_DYNAMIC(CStencilComponentRulePreviewPage)

public:
   CStencilComponentRulePreviewPage();
   virtual ~CStencilComponentRulePreviewPage();

protected:
   virtual void fillTree();

public:
   virtual BOOL OnSetActive();
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

//_____________________________________________________________________________
//class CMapStringToAperture : public CTypedMapStringToPtrContainer<BlockStruct*>
//{
//public:
//   CMapStringToAperture();
//};

//_____________________________________________________________________________
class CStencilGeneratorPropertySheet : public CResizingPropertySheet
{
	DECLARE_DYNAMIC(CStencilGeneratorPropertySheet)

private:
   bool m_stencilGeneratorInitialized;

private:
   CStencilUi& m_stencilUi;
   CStencilConfigurationPropertyPage m_configurationPropertyPage;
   CStencilGeomPadPropertyPage m_geomPadPropertyPage;
   CStencilCompPinPropertyPage m_compPinPropertyPage;
   CStencilGeometryRulePreviewPage m_geometryRulePropertyPage;
   CStencilComponentRulePreviewPage m_componentRulePropertyPage;
   CStencilValidationRulesPropertyPage   m_validationRulesPropertyPage;
   bool m_attributeValuesInitializedFlag;
   //CMapStringToAperture m_apertureMap;

   int m_columnStencilObjectWidth;
   int m_columnRuleWidth;
   int m_columnRuleValueWidth;
   int m_columnRuleModifierWidth;

public:
	CStencilGeneratorPropertySheet(CStencilUi& stencilUi);
	virtual ~CStencilGeneratorPropertySheet();

   CStencilUi& getStencilUi() { return m_stencilUi; }
   CCamCadDatabase& getCamCadDatabase() { return getStencilUi().getCamCadDatabase(); }
   CStencilGenerator& getStencilGenerator() { return getStencilUi().getStencilGenerator(); }
   //CStencilGenerationParameters& getStencilGenerationParameters()
   //   { return m_configurationPropertyPage.getStencilGenerationParameters(); }
   CStencilGenerationParameters& getStencilGenerationParameters()
      { return m_configurationPropertyPage.getStencilGenerationParameters(); }
   CStencilValidationParameters& getStencilValidationParameters()
      { return m_validationRulesPropertyPage.getStencilValidationParameters(); }
   bool getAutoOpenLogFile() { return m_validationRulesPropertyPage.getAutoOpenLogFile(); }

   void fillGeometryTree() { if (this->getStencilGenerator().getInitializedFlag()) m_geometryRulePropertyPage.fillTree(); } //*rcf experimental

   BlockStruct* getPcbBlock() { return getStencilUi().getPcbBlock(); }
   void initializeAttributeValues(CStencilPin& stencilPin,AttributeSourceTag sourceTag);
   void initializeAttributeValues(bool resetFlag=false);
   void updateStatus();
   void clearStencilSettings();
   void updateData(bool saveAndValidate);

   int  getColumnStencilObjectWidth() const;
   void setColumnStencilObjectWidth(int width);

   int  getColumnRuleWidth() const;
   void setColumnRuleWidth(int width);

   int  getColumnRuleValueWidth() const;
   void setColumnRuleValueWidth(int width);

   int  getColumnRuleModifierWidth() const;
   void setColumnRuleModifierWidth(int width);

   //CMapStringToAperture& getApertureMap() { return m_apertureMap; }
   virtual CString GetDialogProfileEntry() { return "StencilGeneratorPropertySheet"; }

private:
   void init();

protected:
	DECLARE_MESSAGE_MAP()

public:
   virtual void saveWindowState();
   virtual void restoreWindowState();
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedOK();
   virtual afx_msg void OnSize(UINT nType, int cx, int cy);
};

//_____________________________________________________________________________
class CStencilErrorBrowseDialog : public CResizingDialog
{
	//DECLARE_DYNAMIC(CStencilErrorBrowseDialog)

private:
   CStencilUi& m_stencilUi;

   CListControl m_errorListControl;
   CImageList m_listImageList;
   int m_openIconIndex;
   int m_closedIconIndex;
   int m_infoIconIndex;
   int m_undefinedIconIndex;

   CStencilTreeListFrame m_treeListFrame;

   HTREEITEM m_pcbItem;
   HTREEITEM m_topSurfaceItem;
   HTREEITEM m_topSmdSurfaceItem;
   HTREEITEM m_topThSurfaceItem;
   HTREEITEM m_topViaSurfaceItem;
   HTREEITEM m_topFiducialSurfaceItem;
   HTREEITEM m_bottomSurfaceItem;
   HTREEITEM m_bottomSmdSurfaceItem;
   HTREEITEM m_bottomThSurfaceItem;
   HTREEITEM m_bottomViaSurfaceItem;
   HTREEITEM m_bottomFiducialSurfaceItem;

   CStencilViolation* m_selectedStencilViolation;
   CStencilPin* m_selectedStencilHole;

   bool m_fillingList;
   bool m_initialized;

public:
	CStencilErrorBrowseDialog(CStencilUi& stencilUi,CWnd* pParent = NULL);
	virtual ~CStencilErrorBrowseDialog();

// Dialog Data
	enum { IDD = IDD_BrowseStencilErrors };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

   HTREEITEM getPcbItem();
   HTREEITEM getTopSurfaceItem();
   HTREEITEM getTopSmdSurfaceItem();
   HTREEITEM getTopThSurfaceItem();
   HTREEITEM getTopViaSurfaceItem();
   HTREEITEM getTopFiducialSurfaceItem();
   HTREEITEM getBottomSurfaceItem();
   HTREEITEM getBottomSmdSurfaceItem();
   HTREEITEM getBottomThSurfaceItem();
   HTREEITEM getBottomViaSurfaceItem();
   HTREEITEM getBottomFiducialSurfaceItem();

   CStencilPin* getSelectedStencilHole();
   CStencilSubPin* getSelectedStencilSubPin();
   CStencilTreeListCtrl& getStencilTreeListCtrl();
   void expandTree(HTREEITEM item=TVI_ROOT);
   void emptyTree();
   void fillTree();
   void emptyList();
   void fillList();

public:
   CStencilUi& getStencilUi() { return m_stencilUi; }
   CCamCadDatabase& getCamCadDatabase() { return getStencilUi().getCamCadDatabase(); }
   CStencilViolations& getStencilViolations() { return getStencilUi().getStencilViolations(); }
   CStencilGenerator& getStencilGenerator() { return getStencilUi().getStencilGenerator(); }
   void empty();

   virtual CString GetDialogProfileEntry() { return CString("StencilErrorBrowseDialog"); }

public:
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedZoomToButton();
   afx_msg void OnBnClickedPanToButton();
   afx_msg void OnBnClickedCloseButton();
   afx_msg void OnLvnOdstatechangedListcontrol(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnLvnItemActivateListcontrol(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnLvnItemchangedListcontrol(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnClose();
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   BOOL m_disableHidingChecked;
   afx_msg void OnBnClickedHideButton();
};

//_____________________________________________________________________________
class CStencilWaitDialog : public CResizingDialog
{
private:
   CStencilUi& m_stencilUi;

public:
	CStencilWaitDialog(CStencilUi& stencilUi,CWnd* pParent = NULL);
	virtual ~CStencilWaitDialog();

// Dialog Data
	enum { IDD = IDD_StencilWaitDialog };

public:
   CStencilUi& getStencilUi() { return m_stencilUi; }

   virtual CString GetDialogProfileEntry() { return CString("StencilWaitDialog"); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedContinue();
   afx_msg void OnBnClickedTerminate();
};

//_____________________________________________________________________________
class CApertureGeometryDialog : public CResizingDialog
{
private:
   static CCamCadDatabase* m_camCadDatabase;
   CListBox m_apertureGeometryListBox;
   CString m_apertureName;

public:
	CApertureGeometryDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CApertureGeometryDialog();

// Dialog Data
	enum { IDD = IDD_ApertureGeometry };

   virtual CString GetDialogProfileEntry() { return CString("ApertureGeometryDialog"); }

public:
   CString getApertureName();

   static CCamCadDatabase* getCamCadDatabase() { return m_camCadDatabase; }
   static void setCamCadDatabase(CCamCadDatabase* camCadDatabase) { m_camCadDatabase = camCadDatabase; }

protected:
   virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnSelChangeApertureGeometryList();
};

//_____________________________________________________________________________

#endif
