// $Header: /CAMCAD/4.6/CcView.h 35    4/18/07 7:18p Kurt Van Ness $

// CCview.h : interface of the CCEtoODBView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "geomlib.h"
#include "CCdoc.h"
#include "select.h"
#include "font.h"
#include "dblrect.h"

class CSchematicList;
class CNamedView;
class CMeasureBaseDialog;
class CMeasure;
class CEditBar;

enum CursorMode {
   Search,
   Measure,
   MarkRect,
   MarkRect_CrossCheck,
   NewOrg,
   Move,
   MoveFiles,
   RotateSelected,
   AddLineSegment,
   AddPolyline,
   AddArcCRSE,
   AddArcCounterCRSE,
   AddArcSEC,
   AddCircle,
   AddPoint,
   AddBlob,
   AddInsert,
   AddText,
   AddDRCMarker,
   EditPoly_MoveVertex,
   EditPoly_AddVertex,
   EditPoly_DeleteVertex, 
   EditPoly_MoveSegment,
   EditPoly_DeleteSegment,
   EditPoly_EditBulge,
   GenerateBlock,
   Join_Snap,
   Join_Extend,
   Join_Chamfer,
   Join_Fillet,
   LinkPolys,
   UnlinkPoly,
   Redlining,
   MoveAttrib,
   GetCoord,
   TAPlaceOffset,
   LayerOffset,
};

enum ZoomMode {
   NoZoom,
   ZoomWindow,
   ZoomIn,
   Pan,
};

enum LastCommand{
   LC_Nothing,
   LC_Move, 
   LC_Copy,
   LC_Rotate,
   LC_Delete,
   LC_AddInsert,
   LC_AddDRCMarker,
   LC_AssignAttrib,
   LC_SeqFirst,
   LC_SeqLast,
   LC_SeqBehindHead,
   LC_MoveVisAttrib,
   LC_LayerOffset,
};

enum EMeasureDialogType
{
   measureDialogTypeDefault,
	measureDialogTypeAlignLayer,
};

//_____________________________________________________________________________
class CCEtoODBView : public CView
{
private:
   bool m_contextMenuEnabled;
   CMeasure* m_measure;
   double m_scaleFactor;
   CEditBar* m_rotationAngleEditBar;

protected: // create from serialization only
   //afx_msg void OnContextMenu(CWnd*, CPoint point);
   CCEtoODBView();
   DECLARE_DYNCREATE(CCEtoODBView)

	short m_beginUpdateRequests;

// Attributes
public:
   CCEtoODBDoc* GetDocument();
   CMeasure* getMeasure() { return m_measure; }

   CMeasure& getDefinedMeasure(EMeasureDialogType dialogType);
   void OnMeasureTerminate();

   int ScaleNum;
   double ScaleDenom;
   //double scaleFactor;

   CRect visibleRect;
   ExtentRect pageRect;

   CursorMode cursorMode;

   ZoomMode zoomMode;

   LastCommand lastCommand;

   BOOL Drawing;
   BOOL DrawVariablesInitialized;
   BOOL FirstDrag;
   BOOL HideCursor;
   CPoint prevPnt;
   double lastClickX, lastClickY;
	double lastRightClickX, lastRightClickY;
   BOOL showSchNav;

   SelectStruct FilterEntity;
   int FilterX, FilterY;
   BOOL FilterFirst;

   BOOL ThumbTracking;
   BOOL MovingDatas;

   // variables used for zoom modes
   BOOL zoomFirstDrag;
   BOOL zoomAnchored;
   CPoint zoomAnchorPnt, zoomPrevPnt;

   // variables used for cursor modes
   BOOL cursorFirstDrag;
   int cursorAnchored;
   CPoint cursorAnchor, cursorAnchor2, cursorPrevPnt;
   double anchor_x, anchor_y, anchor2_x, anchor2_y;
   BOOL CounterClockwise;
   double radius;
   int _x, _y;
   SelectStruct s;

   POSITION textFilePos, textDataPos;
   CSchematicList* schListDlg;

   // variables for saving the last page size, zoom level, and pan location
   // when editing a geometry
   int lastScaleNum;
   double lastScaleDenom;
   CPoint lastLocation;
   CDblRect lastPageSize;

// access
   double getScaleFactor() const { return m_scaleFactor; }
   //void setScaleFactor(double scaleFactor);

// Operations
public:
   void PanCoordinate(double x, double y, BOOL ShowCross);
	int PanToRefName(CString refName);
   void showCross(double x,double y);
	void ZoomBox(CDblRect rect)						{ ZoomBox(rect.xMin, rect.xMax, rect.yMin, rect.yMax);	};
   void ZoomBox(double xmin, double xmax, double ymin, double ymax);

	//void BeginUpdate();
	//void EndUpdate();

   //bool isContextMenuEnabled();
   //void enableContextMenu(bool enableFlag);

   void UpdateScale();
   void SetScroll();

   void SetXPos(int pos);
   void SetYPos(int pos);
   int GetXPos();
   int GetYPos();

   //void EraseSearchCursor();
   //void HideSearchCursor();
   //void ShowSearchCursor();
   //void DrawSearchCursor(CDC *dc, BOOL Search, BOOL Editing);
   //void DrawCross(CDC *dc, int x, int y, float scale);
   //int GetCursorSize();
   //BOOL Searching();
   //BOOL IsEditing();
   void PanToText();
   int DoTextPan(const char *text, DataStruct *data, FileStruct *file, BOOL Attrib);

   //void Draw(CDC* pDC,double scaleFactor,bool printModeFlag);
   void DrawFiles(CDC *pDC, double ViewWidth, double ViewHeight);
   //void DrawGrid(CDC* pDC, int minXCoord, int maxXCoord, 
         //int minYCoord, int maxYCoord, SettingsStruct *Settings);
   int DrawBlock(CDC *pDC, BlockStruct *block, double insert_x, double insert_y, double rotation, 
         int mirror, double scale, int insertLayer, int highlight, int recurseLevel, 
         BOOL ColorOverride, COLORREF overrideColor, BOOL negative);
   BOOL DrawPolyStruct(CDC *pDC, CPolyList *polylist, double scale, int mirror, 
         double insert_x, double insert_y, Mat2x2 *m, int layer, BOOL showFills, BOOL MemDCVoids);
   void DrawCircle(CDC *pDC, DataStruct *np, double scale, int mirror, double insert_x, double insert_y,
         Mat2x2 *m);
   void DrawArc(CDC *pDC, DataStruct *np, double scale, int mirror, double insert_x, double insert_y,
         Mat2x2 *m, double rotation);
   void DrawTextEntity(CDC *pDC, DataStruct *np, double scale, int mirror, double insert_x, double insert_y,
         Mat2x2 *m, double rotation);
   void DrawText(CDC *pDC, const char *text, FontStruct *fs, Point2 *point, double scale,
         double text_rot, double height, double width, BOOL text_mirror, 
         int oblique, BOOL proportional, BOOL never_mirror, int layer,
         HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition);
   void DrawBlob(CDC *dc, DataStruct *data, double scale, int mirror, double insert_x, double insert_y, Mat2x2 *m);
   void DrawTestAccess(CDC *pDC, DataStruct *data, BlockStruct *subblock, 
         double insert_x, double insert_y, double scale, double angle, int mirror);
   void DrawAttribs(CDC *pDC, DataStruct *data, double scale, int mirror,
                          double insert_x, double insert_y, double angle, int highlight);
   void DrawUndefined(CDC *pDC, double x, double y, double radius, int code);
   void DrawAperture(CDC *pDC, int type, double sizeA, double sizeB, 
         double x, double y, double rotation, double xoffset, double yoffset, int ApNum);
   void DrawChar(CDC *pDC, CPolyList *PolyList, double insert_x, double insert_y,
         double rotation, double scale, double charwidthfactor, char mirror, int oblique, 
         int textmode, double barwidth, int layer = -1);

   BOOL SearchFilter(CPoint point, SelectStruct *s);
   void recallView(CNamedView& namedView);
   void getLayerViewData(CNamedView& namedView);
   CNamedView* saveView(const CString& viewName);

   //CEditBar& getDefinedRotationAngleEditBar();
   //void releaseRotationAngleEditBar();

   void setZoom(int xPos,int yPos,int scaleNumerator,double scaleDenominator);

public:
   //void DoLButtonDown(UINT nFlags, double x, double y);

protected:
   //void OnMouseMove_MoveSelected(CDC *dc, CPoint *point);
   //void OnLButtonDown_MoveSelected(CDC *dc, CPoint *pnt);

   void OnMouseMove_MoveAttrib(CDC *dc, CPoint *point);
   void OnLButtonDown_MoveAttrib(CDC *dc, CPoint *pnt);

   //void OnMouseMove_MoveFile(CDC *dc, CPoint *point);
   //void OnLButtonDown_MoveFile(CDC *dc, CPoint *pnt);

   //void OnMouseMove_RotateSelected(CDC *dc, CPoint *point);
   //void OnLButtonDown_RotateSelected(CDC *dc, CPoint *pnt);

   void OnMouseMove_MoveOrigin(CDC *dc, CPoint *point);
   void OnLButtonDown_MoveOrigin(CDC *dc, CPoint *pnt);

   void OnMouseMove_ZoomIn(CDC *dc, CPoint *point);
   void OnLButtonDown_ZoomIn(CDC *dc, CPoint *pnt);

   void OnLButtonDown_Pan(CDC *dc, CPoint *pnt);

   void OnLButtonDown_GenerateBlock(CDC *dc, CPoint *pnt);

   void OnMouseMove_ZoomWindow(CDC *dc, CPoint *point);
   void OnLButtonDown_ZoomWindow(CDC *dc, CPoint *pnt);
   void OnLButtonUp_ZoomWindow(CDC *dc, CPoint *pnt);

   void OnMouseMove_AddLineSegment(CDC *dc, CPoint *point);
   void OnLButtonDown_AddLineSegment(CDC *dc, double x, double y);

   void OnMouseMove_AddPolyline(CDC *dc, CPoint *point);
   void OnLButtonDown_AddPolyline(CDC *dc, double x, double y);

   void OnMouseMove_AddCircle(CDC *dc, CPoint *point);
   void OnLButtonDown_AddCircle(CDC *dc, double x, double y);

   void OnMouseMove_AddArcCRSE(CDC *dc, CPoint *point);
   void OnLButtonDown_AddArcCRSE(CDC *dc, double x, double y);

   void OnMouseMove_AddArcSEC(CDC *dc, CPoint *point);
   void OnLButtonDown_AddArcSEC(CDC *dc, double x, double y);

   void OnMouseMove_AddPoint(CDC *dc, CPoint *point);
   void OnLButtonDown_AddPoint(CDC *dc, double x, double y);

   void OnMouseMove_AddBlob(CDC *dc, CPoint *point);
   void OnLButtonDown_AddBlob(CDC *dc, double x, double y);

   void OnMouseMove_AddText(CDC *dc, CPoint *point);
   void OnLButtonDown_AddText(CDC *dc, double x, double y);

   void AddAnotherInsert();
   void OnMouseMove_AddInsert(CDC *dc, CPoint *point);
   void OnLButtonDown_AddInsert(CDC *dc, double x, double y);

   void OnLButtonDown_AddDRCMarker(CDC *dc, double x, double y);

   void OnMouseMove_Redlining(CDC *dc, CPoint *point, double x, double y);
   void OnLButtonDown_Redlining(CDC *dc, double x, double y);

   void OnMouseMove_MarkRect(CDC *dc, CPoint *point);
   void OnLButtonDown_MarkRect(CDC *dc, CPoint *pnt);
   void OnLButtonUp_MarkRect(CDC *dc, CPoint *pnt);

   void OnMouseMove_MoveVertex(CDC *dc, CPoint *point);
   void OnLButtonDown_MoveVertex(CDC *dc, double x, double y);

   void OnMouseMove_MoveSegment(CDC *dc, CPoint *point);
   void OnLButtonDown_MoveSegment(CDC *dc, double x, double y);

   void OnMouseMove_AddVertex(CDC *dc, CPoint *point);
   void OnLButtonDown_AddVertex(CDC *dc, double x, double y);
   
   void OnMouseMove_EditBulge(CDC *dc, CPoint *point);
   void OnLButtonDown_EditBulge(CDC *dc, double x, double y);
   
   void OnLButtonDown_DeleteVertex(CDC *dc, double x, double y);

   void OnLButtonDown_TAPlaceOffset(CDC *dc, double x, double y);

   //void OnMouseMove_LayerOffset(CDC *dc, CPoint *point);
   //void OnLButtonDown_LayerOffset(CDC *dc, CPoint *point);

   void TaPlaceOffsets();

   void PreparePrintDC(CDC* pDC, CPrintInfo* pInfo, CCEtoODBDoc *doc); 

   void getScreenPalette(CPalette& palette,int bitsPerPixel);
   HBITMAP GetClientBitmap(); 
   HBITMAP getClientBitmap(double scaleFactor,int bitsPerPixel); 
   void PrepareBitmapDC(CDC& dc,double factor);
   bool WriteImage(const CString& filename, double imageFactor,int bitsPerPixel,bool compressFlag);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CCEtoODBView)
   public:
   virtual void OnDraw(CDC* pDC);  // overridden to draw this view
   virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
   //virtual void OnInitialUpdate();
   //virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
   //virtual BOOL PreTranslateMessage(MSG* pMsg);
   protected:
   virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
   virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
   virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
   virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
   virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CCEtoODBView();

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
   //{{AFX_MSG(CCEtoODBView)
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnZoomIn();
   afx_msg void OnZoomOut();
   //afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   //afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   //afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnZoomWindow();
   afx_msg void OnZoom1To1();
   afx_msg void OnZoomExtents();
   //afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   //afx_msg void OnDestroy();
   afx_msg void OnMoveOrigin();
   afx_msg void OnAddLineSegment();
   afx_msg void OnAddCircle();
   afx_msg void OnAddArcSEC();
   afx_msg void OnAddArcCRSE();
   afx_msg void OnAddArcCRSECounter();
   afx_msg void OnAddText();
   afx_msg void OnAddFiducial();
	afx_msg void OnPopupAddFiducial();
   afx_msg void OnAddInsert();
   afx_msg void OnAddLinePolyline();
   afx_msg void OnPolylineClose();
   afx_msg void OnPolylineEnd();
   //afx_msg void OnPopupCancel();
   afx_msg void OnPolylineCloseAndFill();
   //afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   //afx_msg void OnMoveSelected();
   //afx_msg void OnClipboard();
   //afx_msg void OnWriteBitmap();
   afx_msg void OnMarkRect();
   afx_msg void OnPan();
   afx_msg void OnPanToText();
   afx_msg void OnNextText();
   afx_msg void OnMarkRect_CrossCheck();
   afx_msg void OnMoveVertex();
   afx_msg void OnMoveSegment();
   afx_msg void OnAddVertex();
   afx_msg void OnDeleteVertex();
   afx_msg void OnMeasure();
   afx_msg void OnDeleteSegment();
   afx_msg void OnEditBulge();
   afx_msg void OnGenerateBlock();
   afx_msg void OnLinkPolys();
   afx_msg void OnUnlinkPoly();
   afx_msg void OnPanCoordinate();
   afx_msg void OnPanReference();
   //afx_msg void OnExecuteLastCommand();
   afx_msg void OnRlLeader();
   afx_msg void OnRlBox();
   afx_msg void OnRlCloud();
   afx_msg void OnRlCrossOut();
   afx_msg void OnJoinSnap();
   afx_msg void OnJoinExtend();
   afx_msg void OnJoinChamfer();
   afx_msg void OnJoinFillet();
   afx_msg void OnRlSketch();
   afx_msg void OnRlNote();
   //afx_msg void OnCopySelected();
   afx_msg void OnRlSmall();
   afx_msg void OnRlMedium();
   afx_msg void OnRlLarge();
   afx_msg void OnAddPoint();
   //afx_msg void OnRotateSelected();
   afx_msg void OnAddDRCMarker();
   afx_msg void OnZoominHotkey();
   //afx_msg void OnMoveFile();
   afx_msg void OnAddBlob();
   afx_msg void OnAutojoinPanHead();
   afx_msg void OnAutojoinPanTail();
   afx_msg void OnMoveAttrib();
   afx_msg void OnPickCoord();
   afx_msg void OnTaPlaceOffsetTop();
   afx_msg void OnTaPlaceOffsetBottom();
   afx_msg void OnTaPlaceOffsetBoth();
   //afx_msg void OnLayerOffset();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   //afx_msg void OnMoveSelectedToAnotherFile();
   //afx_msg void OnCopySelectedToAnotherFile();
   //}}AFX_MSG

   afx_msg void OnUpdateJoinSnap(CCmdUI* pCmdUI);
   afx_msg void OnUpdateJoinExtend(CCmdUI* pCmdUI);
   afx_msg void OnUpdateJoinChamfer(CCmdUI* pCmdUI);
   afx_msg void OnUpdateJoinFillet(CCmdUI* pCmdUI);

   afx_msg void OnAddToSynchronizedViews();
   afx_msg void OnDisableViewSynchronization();
   afx_msg void OnAddOutline();

   DECLARE_MESSAGE_MAP()
   afx_msg void OnSchematicList();
   afx_msg void OnUpdateSchematicList(CCmdUI* pCmdUI);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

//#ifndef _DEBUG  // debug version in CCview.cpp
/*inline CCEtoODBDoc* CCEtoODBView::GetDocument()
{ 
	return (CCEtoODBDoc*)m_pDocument; 
}*/
//#endif

CCEtoODBView* getActiveView();

/////////////////////////////////////////////////////////////////////////////

// end CCVIEW.H
