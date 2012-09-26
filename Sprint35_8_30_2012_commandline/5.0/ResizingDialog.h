// $Header: /CAMCAD/4.5/ResizingDialog.h 22    12/23/05 7:00p Lynn Phung $

#if !defined(__ResizingDialog_H__)
#define __ResizingDialog_H__

#pragma once

#include "TypedContainer.h"

enum DialogFieldOperationTag
{
   anchorLeft,
   anchorRight,
   anchorTop,
   anchorBottom,
   anchorBottomLeft,
   anchorBottomRight,
   anchorProportionalBoth,
   anchorProportionalHorizontal,
   anchorProportionalVertical,
   anchorRelativeToField,

   growStatic,
   growBoth,
   growHorizontal,
   growVertical,
   growProportionalBoth,
   growProportionalHorizontal,
   growProportionalVertical,
   growRelativeToFieldBoth,
   growRelativeToFieldHorizontal,
   growRelativeToFieldVertical,

   glueLeftEdge,
   glueRightEdge,
   glueTopEdge,
   glueBottomEdge,
   glueTopLeftCorner,
   glueTopRightCorner,
   glueBottomLeftCorner,
   glueBottomRightCorner,

   operationUndefined
};

enum DialogFieldOperationModifierTag
{
   toLeftEdge,
   toRightEdge,
   toTopEdge,
   toBottomEdge,
   toTopLeftCorner,
   toTopRightCorner,
   toBottomLeftCorner,
   toBottomRightCorner,

   modifierUndefined

};

class CResizingDialogField;

//_____________________________________________________________________________
class CDialogFieldOperation
{
private:
   DialogFieldOperationTag m_operation;
   DialogFieldOperationModifierTag m_modifier;
   CResizingDialogField* m_relatedField;

public:
   CDialogFieldOperation(DialogFieldOperationTag operation);
   CDialogFieldOperation(DialogFieldOperationTag operation,
      CResizingDialogField* relatedField);
   CDialogFieldOperation(DialogFieldOperationTag operation,
      DialogFieldOperationModifierTag modifier,
      CResizingDialogField* relatedField);

   DialogFieldOperationTag getOperation() { return m_operation; }
   DialogFieldOperationModifierTag getModifier() { return m_modifier; }
   CResizingDialogField* getRelatedField() { return m_relatedField; }
};

//_____________________________________________________________________________
class CDialogFieldOperations : public CTypedPtrListContainer<CDialogFieldOperation*>
{
public:
   CDialogFieldOperations();

   CDialogFieldOperation& addOperation(DialogFieldOperationTag operation);
   CDialogFieldOperation& addOperation(DialogFieldOperationTag operation,
      CResizingDialogField* relatedField);
   CDialogFieldOperation& addOperation(DialogFieldOperationTag operation,
      DialogFieldOperationModifierTag modifier,
      CResizingDialogField* relatedField);
};

//_____________________________________________________________________________
class CResizingDialogField
{
private:
   int m_controlId;
   CRect m_originalRect;  // client coordinates
   CRect m_rect;
   bool m_rectIsValid;
   bool m_isHidden;

   CDialogFieldOperations m_operations;

public:
   CResizingDialogField(CDialog& dialog,int controlId,
      DialogFieldOperationTag anchorValue=operationUndefined,
      DialogFieldOperationTag growValue=operationUndefined);

   int getControlId() { return m_controlId; }
   void setControlId(int controlId) { m_controlId = controlId; }
   CDialogFieldOperations& getOperations() { return m_operations; }

   CRect getOriginalRect() { return m_originalRect; }
   void setOriginalRect(const CRect& rect) { m_originalRect = rect; }
   void offsetOriginalRect(const CSize& size) { m_originalRect.OffsetRect(size); }
   CRect getRect() { return m_rect; }
   void setRect(CRect rect);
   void invalidateRect() { m_rectIsValid = false; }
   bool isRectValid() { return m_rectIsValid; }
   bool isHidden() { return m_isHidden; }
   void setHidden(bool hiddenFlag) { m_isHidden = hiddenFlag; }
   bool areDependenciesValid();
};

//_____________________________________________________________________________
class CResizingDialogFields : public CTypedPtrListContainer<CResizingDialogField*>
{
public:
   CResizingDialogFields();

   CResizingDialogField& addField(CDialog& dialog,int controlId,
      DialogFieldOperationTag anchorValue=anchorLeft,DialogFieldOperationTag growValue=growStatic);
   void offsetOriginalRects(const CSize& size);

   void invalidateRects();
};

#if _MSC_VER >= 1400
typedef LRESULT NCHITTEST_UNIT; 
#else
typedef UINT NCHITTEST_UNIT;
#endif

//_____________________________________________________________________________
class CDialogGripper : public CScrollBar
{
// Construction
public:
   CDialogGripper();

// Implementation
public:
   virtual ~CDialogGripper();

// Generated message map functions
protected:
   //{{AFX_MSG(CGripper)
   afx_msg NCHITTEST_UNIT OnNcHitTest(CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//_____________________________________________________________________________
class CResizingDialog : public CDialog
{
protected:
   CResizingDialogFields m_fields;
   CDialogGripper m_gripper;
   CSize m_originalWindowSize;
   bool m_initializedWindowDataFlag;
   bool m_stateChanged;
   CSize m_originalClientSize;
   CSize m_previousClientSize;

// Dialog Data
protected:
   // Set rememberState to FALSE if you don't want the dialog box 
   // to remember its size between sessions.
   bool m_allowResize;
   bool m_rememberState;
   // Set m_bDrawGipper if you don't want a Resizing Gripper
   bool m_drawGripper;

private:
   int m_minWidth;
   int m_minHeight;
   UINT m_nIDTemplate;

// Construction
public:
   CResizingDialog( UINT nIDTemplate, CWnd* pParentWnd = NULL );

   CResizingDialogField& addFieldControl(int controlId,
      DialogFieldOperationTag anchorValue=anchorLeft,DialogFieldOperationTag growValue=growStatic);
   CResizingDialogField& addFieldControl(int controlId,
      DialogFieldOperationTag anchorValue,CResizingDialogField* anchorRelatedField,
      DialogFieldOperationTag growValue=growStatic);

   bool GetRememberState() { return  m_rememberState;}
   bool GetInitializedWindowDataFlag() { return  m_initializedWindowDataFlag;}
   void SetRememberState(bool rememberSize) { m_rememberState = rememberSize;}

   CSize getOriginalWindowSize() { return m_originalWindowSize; }
   void setOriginalWindowSize(const CSize& size) { m_originalWindowSize = size; }

   CSize getOriginalClientSize() { return m_originalClientSize; }
   void setOriginalClientSize(const CSize& size) { m_originalClientSize = size; }

   CSize getPreviousClientSize() { return m_previousClientSize; }
   void setPreviousClientSize(const CSize& size) { m_previousClientSize = size; }

   void initDialog();
   virtual void saveWindowState();
   virtual void restoreWindowState();
   CSize getMinMaxSize();
   void setMinMaxSize(CSize& size);
   void adjustSizeForToolBar(int toolbarHeight);
   void setEnableResize(bool enableFlag);
   virtual CString GetDialogProfileEntry();
   virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
   // Generated message map functions
   //{{AFX_MSG(CResizingDialog)
   virtual BOOL OnInitDialog();

   virtual afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnMove(int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
   afx_msg void OnDestroy();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg NCHITTEST_UNIT OnNcHitTest(CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif

