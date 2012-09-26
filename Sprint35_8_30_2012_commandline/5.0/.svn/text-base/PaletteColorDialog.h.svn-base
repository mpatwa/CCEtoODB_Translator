// $Header: /CAMCAD/4.5/PaletteColorDialog.h 12    7/24/06 10:19a Kurt Van Ness $

/*
History: PaletteColorDialog.h $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 10/20/00   Time: 11:46p
 * Updated in $/PadsDRC
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 6/12/99    Time: 11:31a
 * Created in $/PadsDRC
 * Initial add.
*/

#if !defined(__PaletteColorDialog_H__)
#define __PaletteColorDialog_H__

#include "ColorPaletteButton.h"
#include "AppPalette.h"
#include "Color.h"
#include "Resource.h"
#include "PcbComponentPinAnalyzer.h"
#include "ResizingDialog.h"
#include "afxwin.h"
#include "afxcmn.h"

//_____________________________________________________________________________
class CPaletteRampEntry : public CObject
{
private:
   int m_index;
   COLORREF m_color;
   bool m_nodeFlag;

public:
   CPaletteRampEntry(int index = -1);
   virtual ~CPaletteRampEntry();

   int getIndex() { return m_index; }
   COLORREF getColor() { return m_color; }
   BYTE getRed() { return GetRValue(m_color); }
   BYTE getGrn() { return GetGValue(m_color); }
   BYTE getBlu() { return GetBValue(m_color); }
   void setColor(COLORREF color) { m_color = color; }
   bool isNode() { return m_nodeFlag; }
   void setNodeFlag(bool flag);
};

//_____________________________________________________________________________
class CPaletteRamp : public CObject
{
private:
   ColorModelTag m_colorModel;
   CTypedPtrArray<CObArray,CPaletteRampEntry*> m_paletteEntries;
   bool m_paletteValid;
   CPaletteRampEntry* m_outOfBoundsEntry;
   double m_nslLin;

public:
   CPaletteRamp(ColorModelTag colorModel=colorModelRgb);
   virtual ~CPaletteRamp();
   void empty();

   int getSize() { return m_paletteEntries.GetSize(); }
   void setSize(int size,int growBy = -1);
   COLORREF getColor(int index);
   void setColor(int index,COLORREF color);
   void setNslLin(double nslLin);
   bool isNode(int index);
   void setNodeFlag(int index,bool nodeFlag);
   bool isPaletteValid() { return m_paletteValid; }
   void validatePalette();
   void invalidatePalette();

protected:
   void calculatePalette();
   CPaletteRampEntry& getEntry(int index);
   virtual CPaletteRampEntry* newPaletteRampEntry(int index);
};

//_____________________________________________________________________________
class CPaletteItem : public CPaletteRampEntry
{
private:
   CPoint m_topLeft;
   int m_sideLength;
   int m_spacing;
   int m_xCount;
   int m_yCount;
   BookReadingOrderTag m_bookReadingOrder;

   bool m_hoveredFlag;
   bool m_selectedFlag;
   bool m_showNodeFlag;

public:
   CPaletteItem(int index,const CPoint& topLeft,int sideLength,int spacing,int xCount,int yCount,
      BookReadingOrderTag bookReadingOrder);
   virtual ~CPaletteItem();

   bool isHovered() { return m_hoveredFlag; }
   void setHoveredFlag(bool flag) { m_hoveredFlag = flag; }
   bool isSelected() { return m_selectedFlag; }
   void setSelectedFlag(bool flag) { m_selectedFlag = flag; }
   bool getShowNodeFlag() const { return m_showNodeFlag; }
   void setShowNodeFlag(bool flag) { m_showNodeFlag = flag; }
   CRect getRect();
   CRect getInflatedRect(int inc);
   int getIndexX();
   int getIndexY();

   void draw(CDC& dc);
   void drawItem(CDC& dc,COLORREF outerBorderColor,COLORREF innerBorderColor,COLORREF fillColor,CRect rect,int penWidth=1);

   int getPixelArraySizeX() { return m_xCount * (m_sideLength + m_spacing); }
   int getPixelArraySizeY() { return m_yCount * (m_sideLength + m_spacing); }
};

//_____________________________________________________________________________
class CPaletteItemArray : public CPaletteRamp
{
private:
   CPoint m_topLeft;
   int m_itemSideLength;
   int m_itemSpacing;
   int m_itemXCount;
   int m_itemYCount;
   BookReadingOrderTag m_bookReadingOrder;

public:
   CPaletteItemArray(const CPoint& topLeft,int itemSideLength,int itemSpacing,int itemXCount,int itemYCount,
      ColorModelTag colorModel,BookReadingOrderTag bookReadingOrder);
   CPaletteItemArray(const CRect& rect,int itemSideLength,int itemSpacing,
      ColorModelTag colorModel,BookReadingOrderTag bookReadingOrder);
   virtual ~CPaletteItemArray();

   void initialize();

   int hitTest(CPoint point);
   void draw(int index,CDC& dc) { getPaletteItem(index).draw(dc); }
   CRect getInflatedRect(int index,int inc) { return getPaletteItem(index).getInflatedRect(inc); }
   void setSelectedFlag(int index,bool flag) { getPaletteItem(index).setSelectedFlag(flag); }
   void setHoveredFlag(int index,bool flag) { getPaletteItem(index).setHoveredFlag(flag); }
   BYTE getRed(int index) { return getPaletteItem(index).getRed(); }
   BYTE getGrn(int index) { return getPaletteItem(index).getGrn(); }
   BYTE getBlu(int index) { return getPaletteItem(index).getBlu(); }

   CPoint getTopLeft()     const { return m_topLeft; }
   int getItemSideLength() const { return m_itemSideLength; }
   int getItemSpacing()    const { return m_itemSpacing; }
   int getGridSpacing()    const { return m_itemSideLength + m_itemSpacing; }
   int getItemXCount()     const { return m_itemXCount; }
   int getItemYCount()     const { return m_itemYCount; }
   int getItemCount()      const { return m_itemXCount * m_itemYCount; }
   CSize getItemCounts() const;
   CSize getItemCounts(const CRect& rect) const;

   CPaletteItem& getPaletteItem(int index) { return static_cast<CPaletteItem&>(getEntry(index)); }

protected:
   virtual CPaletteRampEntry* newPaletteRampEntry(int index);
};

//_____________________________________________________________________________
class CColorPaletteStatic : public CStatic
{
private:
   CPaletteItemArray* m_paletteItemArray;
   ColorModelTag m_colorModel;
   int m_componentIndex0;
   int m_componentIndex1;
   int m_componentIndex2;
   double m_minComponent0;
   double m_minComponent1;
   double m_minComponent2;
   double m_maxComponent0;
   double m_maxComponent1;
   double m_maxComponent2;
   double m_nslLin;

   int m_selectedItemIndex;

public:
   CColorPaletteStatic();
   ~CColorPaletteStatic();

   CPaletteItemArray& getPaletteItemArray();
   CPaletteItemArray& getPaletteItemArrayOld();
   void invalidatePalette();
   bool isPaletteValid() const { return m_paletteItemArray != NULL; }
   CSize getItemCounts() { return getPaletteItemArray().getItemCounts(); }
   CSize getItemCounts(const CRect& rect) { return getPaletteItemArray().getItemCounts(rect); }
   bool itemCountsChange(const CRect& rect);

   void setColorModel(ColorModelTag colorModel);
   void setNslLin(double nslLin);
   void setComponentIndices(int componentIndex0,int componentIndex1);
   void setMinComponent(int componentIndex,double component);
   void setMaxComponent(int componentIndex,double component);
   void getMappedComponent(double& val0,double& val1,double& val2,double comp,int compIndex);
   void getMappedComponents(double& val0,double& val1,double& val2,double comp0,double comp1,double comp2);

   int getSelectedItemIndex() const { return m_selectedItemIndex; }
   void setSelectedItemIndex(int selectedIndex = -1);
   int setSelected(const CPoint& point);
   CColor getSelectedItemColor();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
class CPaletteColorDialog : public CResizingDialog
{
private:
   CAppPalette& m_appPalette;
   bool m_initialized;

   CColor m_color;
   CColorPaletteStatic m_colorPaletteStatic;
   int m_selectedItemIndex;

// Construction
public:
	CPaletteColorDialog(CAppPalette& appPalette,CWnd* pParent = NULL);   // standard constructor

   COLORREF getColor() { return m_color.getColor(); }
   void setColor(COLORREF color) { m_color.setColor(color); }

   ColorModelTag getColorModel() const;

// Dialog Data
	//{{AFX_DATA(CPaletteColorDialog)
	enum { IDD = IDD_PaletteColors };

	CEdit	m_hsvHueValue;
	CEdit	m_hsvSatValue;
	CEdit	m_hsvValValue;

	CEdit	m_hslHueValue;
	CEdit	m_hslSatValue;
	CEdit	m_hslLumValue;
   CEdit m_hslLinValue;

	CEdit	m_redValue;
	CEdit	m_grnValue;
	CEdit	m_bluValue;

   CEdit m_nslHueValue;
   CEdit m_nslSatValue;
   CEdit m_nslLumValue;
   CEdit m_nslLinValue;

	CColorPaletteButton	m_colorButton;

	CSliderCtrl	m_hsvValPos;
	CSliderCtrl	m_hsvSatPos;
	CSliderCtrl	m_hsvHuePos;

	CSliderCtrl	m_redPos;
	CSliderCtrl	m_grnPos;
	CSliderCtrl	m_bluPos;

	CSliderCtrl	m_hslLumPos;
	CSliderCtrl	m_hslSatPos;
	CSliderCtrl	m_hslHuePos;

   CSliderCtrl m_nslHuePos;
   CSliderCtrl m_nslSatPos;
   CSliderCtrl m_nslLumPos;
   CSliderCtrl m_nslLinPos;

   int m_colorModelValue;
   int m_xyAxisValue;
   int m_sliderComponentValue;
   CSliderCtrl m_independantVariablePos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteColorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

   virtual CString GetDialogProfileEntry() { return CString("PaletteColorDialog"); }

// Implementation
protected:
   void updatePaletteSettings();

	// Generated message map functions
	//{{AFX_MSG(CPaletteColorDialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusRedValue();
	afx_msg void OnKillFocusGrnValue();
	afx_msg void OnKillFocusBluValue();
	afx_msg void OnKillFocusHsvHueValue();
	afx_msg void OnKillFocusHsvSatValue();
	afx_msg void OnKillFocusHsvValValue();
	afx_msg void OnKillFocusHslHueValue();
	afx_msg void OnKillFocusHslSatValue();
	afx_msg void OnKillFocusHslLumValue();
   afx_msg void OnKillFocusNslHueValue();
   afx_msg void OnKillFocusNslSatValue();
   afx_msg void OnKillFocusNslLumValue();
   afx_msg void OnKillFocusNslLinValue();
	//afx_msg void OnDestroy();
	afx_msg void OnHelp();
	afx_msg void OnPaletteTest();
	//}}AFX_MSG

   // Handler for the WM_COMMANDHELP message
   afx_msg LRESULT OnCommandHelp(WPARAM,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedRgbModel();
   afx_msg void OnBnClickedHslModel();
   afx_msg void OnBnClickedHsvModel();
   afx_msg void OnBnClickedNslModel();
   afx_msg void OnBnClickedXyAxis0();
   afx_msg void OnBnClickedXyAxis1();
   afx_msg void OnBnClickedSliderComponent0();
   afx_msg void OnBnClickedSliderComponent1();
   afx_msg void OnBnClickedSliderComponent2();
   afx_msg void OnMove(int x, int y);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   //afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__PaletteColorDialog_H__)
