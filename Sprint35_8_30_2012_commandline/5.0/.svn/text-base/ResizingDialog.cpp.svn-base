// $Header: /CAMCAD/4.5/ResizingDialog.cpp 27    12/23/05 7:00p Lynn Phung $

////////////////////////////////////////////////////////////////////////
// ResizingDialog.cpp : implementation file
// 
// Extensively modified and enhanced by: Kurt N. Van Ness

/*
   Original Implementation: Eli Vingot (elivingt@internet-zahav.net)

   Ideas for improving the class are always appreciated


   The base class for the dialog box you want to allow resizing
   Use SetConrolInfo() to determine how each control behaves when
   the user resize the dialog box.
   (The "Windows default" is ANCHOR_TOP | ANCHOR_LEFT)

   e.g. For a right aligned OK button you'll probably call:
   SetControlInfo(IDOK, ANCHOR_RIGHT)
   For a text control that needs to resize with the dialog you may do:
   SetControlInfo(IDD_MYEDITOR, RESIZE_BOTH)

   Note: The dialog box "remebers" its size on destroy and the next time
   you launch it, it'll set the dialog size back to the previous size.
   If you don't like this behavior, call SetRememberState(FALSE)

   LIMITATIONS:
   1) This class does not handle overlapping controls, 
      e.g., you cannot place two controls one (RESIZE_VER) and the other
      with (RESIZE_VER | ANCHOR_BOTTOM) one below the other, they may ovelapp.

   2) This class does not remember the mode of the dialog (Maximized/Minimized)
      it would be easy to add this feature, though.

_______________________________________________________________________________
CApertureListDialog  IDD_LIST_APERTURE
ListAttribs          IDD_LIST_ATTRIBS
EditAttribs          IDD_EDIT_ATTRIBUTES
KeywordMap           IDD_KEYWORD_MAP
SelectGeometry       IDD_GEOMETRY_SELECT
ListCompPin          IDD_LIST_COMP_PIN
DRC_List             IDD_LIST_DRC
ODBDirDlg            IDD_ODB_DIRECTORY
EditLine             IDD_EDIT_LINE         CEditDialog ***
EditPoly             IDD_EDIT_POLYSTRUCT   CEditDialog ***
EditArc              IDD_EDIT_ARC          CEditDialog ***
EditInsert           IDD_EDIT_INSERT       CEditDialog ***
EditCircle           IDD_EDIT_CIRCLE       CEditDialog ***
EditText             IDD_EDIT_TEXT         CEditDialog ***
EditNothing          IDD_EDIT_NOTHING      CEditDialog ***
EditApInsert         IDD_EDIT_AP_INSERT    CEditDialog ***
EditToolInsert       IDD_EDIT_TOOL_INSERT  CEditDialog ***
EditPoint            IDD_EDIT_POINT        CEditDialog ***
EditBlob             IDD_EDIT_BLOB         CEditDialog
CFileTypeDialog      IDD_FILE_TYPE_DIALOG ***
FileExportType       IDD_FILE_EXPORT_TYPE ***
MentorFileType       IDD_MENTOR_FILE_TYPE
CGeometryListDialog  IDD_LIST_GEOMETRY ***
LayerListDlg         IDD_LIST_LAYERS
LayerStackup         IDD_LAYER_STACKUP
NamedViewDlg         IDD_LIST_NAMED_VIEWS
PanToInsertDialog    IDD_PAN_TO_INSERT
PanelDlg             IDD_PANEL
CSchematicList       IDD_LIST_SCHEMATIC
ListTools            IDD_LIST_TOOL
*/

#include "StdAfx.h"
#include "ResizingDialog.h"
#include "RwLib.h"

CWnd* getMainWnd();

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________
CDialogFieldOperation::CDialogFieldOperation(DialogFieldOperationTag operation)
{
   m_operation    = operation;
   m_modifier     = modifierUndefined;
   m_relatedField = NULL;
}

CDialogFieldOperation::CDialogFieldOperation(DialogFieldOperationTag operation,
      CResizingDialogField* relatedField)
{
   m_operation    = operation;
   m_modifier     = modifierUndefined;
   m_relatedField = relatedField;
}

CDialogFieldOperation::CDialogFieldOperation(DialogFieldOperationTag operation,
      DialogFieldOperationModifierTag modifier,
      CResizingDialogField* relatedField)
{
   m_operation    = operation;
   m_modifier     = modifier;
   m_relatedField = relatedField;
}

//_____________________________________________________________________________
CDialogFieldOperations::CDialogFieldOperations()
{
}

CDialogFieldOperation& CDialogFieldOperations::addOperation(DialogFieldOperationTag operation)
{
   CDialogFieldOperation* fieldOperation = new CDialogFieldOperation(operation);
   AddTail(fieldOperation);

   return *fieldOperation;
}

CDialogFieldOperation& CDialogFieldOperations::addOperation(DialogFieldOperationTag operation,
      CResizingDialogField* relatedField)
{
   CDialogFieldOperation* fieldOperation = new CDialogFieldOperation(operation,relatedField);
   AddTail(fieldOperation);

   return *fieldOperation;
}

CDialogFieldOperation& CDialogFieldOperations::addOperation(DialogFieldOperationTag operation,
      DialogFieldOperationModifierTag modifier,
      CResizingDialogField* relatedField)
{
   CDialogFieldOperation* fieldOperation = new CDialogFieldOperation(operation,modifier,relatedField);
   AddTail(fieldOperation);

   return *fieldOperation;
}

//_____________________________________________________________________________
CResizingDialogField::CResizingDialogField(CDialog& dialog,int controlId,
      DialogFieldOperationTag operation1,DialogFieldOperationTag operation2)
{
   m_controlId           = controlId;

   if (operation1 != operationUndefined)
   {
      m_operations.addOperation(operation1);
   }

   if (operation2 != operationUndefined)
   {
      m_operations.addOperation(operation2);
   }

   m_rectIsValid = false;
   m_isHidden    = false;
}

void CResizingDialogField::setRect(CRect rect) 
{ 
   m_rect = rect; 
   m_rectIsValid = true;
}

bool CResizingDialogField::areDependenciesValid()
{
   bool retval = true;

   for (POSITION pos = m_operations.GetHeadPosition();pos != NULL && retval;)
   {
      CDialogFieldOperation* fieldOperation = m_operations.GetNext(pos);

      CResizingDialogField* relatedField = fieldOperation->getRelatedField();

      if (relatedField != NULL)
      {
         retval = relatedField->isRectValid();
      }
   }

   return retval;
}

//_____________________________________________________________________________
CResizingDialogFields::CResizingDialogFields()
{
}

CResizingDialogField& CResizingDialogFields::addField(CDialog& dialog,int controlId,
   DialogFieldOperationTag anchorValue,DialogFieldOperationTag growValue)
{
   CResizingDialogField* field = new CResizingDialogField(dialog,controlId,
                                        anchorValue,growValue);

   AddTail(field);

   return *field;
}

void CResizingDialogFields::invalidateRects()
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CResizingDialogField* field = GetNext(pos);

      field->invalidateRect();
   }
}

void CResizingDialogFields::offsetOriginalRects(const CSize& size)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CResizingDialogField* field = GetNext(pos);

      field->offsetOriginalRect(size);;
   }
}

//_____________________________________________________________________________
CDialogGripper::CDialogGripper()
{
}

CDialogGripper::~CDialogGripper()
{
}

BEGIN_MESSAGE_MAP(CDialogGripper, CScrollBar)
   //{{AFX_MSG_MAP(CGripper)
   ON_WM_NCHITTEST()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGripper message handlers
NCHITTEST_UNIT CDialogGripper::OnNcHitTest(CPoint point) 
{
   NCHITTEST_UNIT ht = CScrollBar::OnNcHitTest(point);

   if (ht == HTCLIENT)
   {
      ht = HTBOTTOMRIGHT;
   }

   return ht;
}

//_____________________________________________________________________________
CResizingDialog::CResizingDialog(UINT nIDTemplate, CWnd* pParentWnd) : 
            CDialog(nIDTemplate,pParentWnd)
{
   m_minWidth = m_minHeight = 0; // flag that GetMinMax wasn't called yet
   m_stateChanged = false;
   m_nIDTemplate  = nIDTemplate;
   m_originalWindowSize = CSize(0,0);
   m_originalClientSize = CSize(0,0);
   m_previousClientSize = CSize(0,0);
   m_initializedWindowDataFlag = false;

   m_allowResize   = true;
   m_rememberState = m_allowResize;
   m_drawGripper   = m_allowResize;
   //m_toolBarHeight = 0;
}

void CResizingDialog::setEnableResize(bool enableFlag)
{
   m_allowResize   = enableFlag;
   //m_rememberState = enableFlag;
   m_drawGripper   = enableFlag;
}

CResizingDialogField& CResizingDialog::addFieldControl(int controlId,
   DialogFieldOperationTag anchorValue,DialogFieldOperationTag growValue)
{
   CResizingDialogField& field = m_fields.addField(*this,controlId,anchorValue,growValue);

   return field;
}

CResizingDialogField& CResizingDialog::addFieldControl(int controlId,
   DialogFieldOperationTag anchorValue,CResizingDialogField* anchorRelatedField,DialogFieldOperationTag growValue)
{
   CResizingDialogField& field = m_fields.addField(*this,controlId,anchorValue,growValue);

   if (anchorRelatedField != NULL)
   {
      field.getOperations().addOperation(anchorValue,anchorRelatedField);
   }

   return field;
}

void CResizingDialog::saveWindowState() 
{
   CWnd* mainWnd = getMainWnd();
   CRect rc;
   GetWindowRect(&rc);
   mainWnd->ScreenToClient(&rc);

   CString dialogName(GetDialogProfileEntry());
   CString windowParameters;

   windowParameters.Format("%d %d %d %d",
      rc.left,
      rc.top,
      rc.Width(),
      rc.Height()  );

   AfxGetApp()->WriteProfileString(dialogName,"Window Parameters",windowParameters);   
}

void CResizingDialog::restoreWindowState() 
{
   CString dialogName(GetDialogProfileEntry());
   CSupString windowParameters = AfxGetApp()->GetProfileString(dialogName,"Window Parameters","");

   if (!windowParameters.IsEmpty())
   {
      CStringArray param;
      windowParameters.Parse(param);

      if (param.GetSize() >= 4)
      {
         CWnd* mainWnd = getMainWnd();
         CRect mainWndRect;
         mainWnd->GetWindowRect(mainWndRect);

         // Load the previous size of the dialog box from the INI/Registry
         int x  = atoi(param[0]);
         int y  = atoi(param[1]);
         int cx = atoi(param[2]);
         int cy = atoi(param[3]);

         if (m_originalWindowSize.cx > 0 && (cx < m_originalWindowSize.cx || !m_allowResize))
         {
            cx = m_originalWindowSize.cx;
            m_previousClientSize.cx = cx;
         }

         if (m_originalWindowSize.cy > 0 && (cy < m_originalWindowSize.cy || !m_allowResize))
         {
            cy = m_originalWindowSize.cy;
            m_previousClientSize.cy = cy;
         }

         CRect rect(x,y,x + cx,y + cy);
         mainWnd->ClientToScreen(rect);

         if (rect.right > mainWndRect.right)
         {
            rect.OffsetRect(mainWndRect.right - rect.right,0);
         }

         if (rect.bottom > mainWndRect.bottom)
         {
            rect.OffsetRect(0,mainWndRect.bottom - rect.bottom);
         }

         if (rect.left < mainWndRect.left)
         {
            rect.OffsetRect(mainWndRect.left - rect.left,0);

            if (rect.right > mainWndRect.right)
            {
               rect.right = mainWndRect.right;

               if (rect.Width() < m_minWidth)
               {
                  rect.right = rect.left + m_minWidth;
               }
            }
         }

         if (rect.top < mainWndRect.top)
         {
            rect.OffsetRect(0,mainWndRect.top - rect.top);

            if (rect.bottom > mainWndRect.bottom)
            {
               rect.bottom = mainWndRect.bottom;

               if (rect.Height() < m_minHeight)
               {
                  rect.bottom = rect.top + m_minHeight;
               }
            }
         }
         
         if (cx != 0 && cy != 0)
         {
            SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),0);
         }
      }
   }
}

void CResizingDialog::initDialog()
{
   CRect originalWindowRect;
   GetWindowRect(originalWindowRect);
   m_originalWindowSize = originalWindowRect.Size();

	for (POSITION pos = m_fields.GetHeadPosition();pos != NULL;)
   {
      CResizingDialogField* field = m_fields.GetNext(pos);

		CWnd* pWnd = GetDlgItem(field->getControlId());

		CRect rect;
      pWnd->GetWindowRect(&rect);
      ScreenToClient(&rect);
      field->setOriginalRect(rect);
   }

   m_initializedWindowDataFlag = true;

   if (m_drawGripper)
   {
      CRect initRect;
      GetClientRect(initRect);
      initRect.left = initRect.right  - GetSystemMetrics(SM_CXHSCROLL);
      initRect.top  = initRect.bottom - GetSystemMetrics(SM_CYVSCROLL);

      m_gripper.Create(WS_CHILD | SBS_SIZEBOX | SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEGRIP |
         WS_VISIBLE,initRect, this, AFX_IDW_SIZE_BOX);
   }

   if (m_rememberState)
   {
      restoreWindowState();
	}
}

BEGIN_MESSAGE_MAP(CResizingDialog, CDialog)
   //{{AFX_MSG_MAP(CResizingDialog)
   ON_WM_MOVE()
   ON_WM_SIZE()
   ON_WM_PAINT()
   ON_WM_NCHITTEST()
   ON_WM_GETMINMAXINFO()
   ON_WM_DESTROY()
   ON_WM_CREATE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// CResizingDialog message handlers

//////////////////////////////////////////////////////////////////////////
// OnInitDialog()
//
BOOL CResizingDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   initDialog();
      
   return FALSE;  // return TRUE  unless you set the focus to a control
}

BOOL CResizingDialog::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg->wParam == VK_ESCAPE)   
   {
      return TRUE;
   }
   else
   {
      return CDialog::PreTranslateMessage(pMsg);
   }
}

CPoint calcRectCenter(CRect rect)
{
   CPoint center(rect.left + rect.Width()/2,rect.top + rect.Height()/2);

   return center;
}

//
// OnSize()
// Set the dialog controls new position and size
//
void CResizingDialog::OnSize(UINT nType, int cx, int cy) 
{
   if (m_originalClientSize.cx == 0 && m_originalClientSize.cy == 0)
   {
      m_originalClientSize = CSize(cx,cy);
   }

   if (m_allowResize)
   {
      CRect currentWindowRect;
      CSize currentWindowSize;

      if (::IsWindow(*this))
      {
         GetWindowRect(&currentWindowRect);
         currentWindowSize = currentWindowRect.Size();
      }

      CDialog::OnSize(nType, cx, cy);
      
      if (nType == SIZE_MINIMIZED)
      {
         return;
      }

      if (m_initializedWindowDataFlag)
      {
         int dx  = cx - m_originalClientSize.cx;
         int dy  = cy - m_originalClientSize.cy;
         int dx2 = cx - m_previousClientSize.cx;
         int dy2 = cy - m_previousClientSize.cy;
         double fx = (double)(currentWindowSize.cx) / m_originalWindowSize.cx;
         double fy = (double)(currentWindowSize.cy) / m_originalWindowSize.cy;

         //TRACE("dx = %d, dy = %d, currentWindowSize(%d,%d)\n",
         //   dx,dy,currentWindowSize.cx,currentWindowSize.cy);

         bool drawGripper = (m_drawGripper && ::IsWindow(m_gripper));

         HDWP hDwp = ::BeginDeferWindowPos(m_fields.GetCount() + (drawGripper ? 1 : 0));

         // Move and Size the controls using the information
         // we got in SetControlInfo()
         //
         m_stateChanged = true;
         CRect WndRect;
         CWnd *pWnd;

         if (drawGripper && ::IsWindow(m_gripper))
         {
            m_gripper.GetWindowRect(&WndRect);  
            ScreenToClient(&WndRect);
            
            WndRect.OffsetRect(dx2,dy2);

            hDwp = ::DeferWindowPos(hDwp,m_gripper,0,WndRect.left,WndRect.top,WndRect.Width(),WndRect.Height(),SWP_NOZORDER);
         }

         m_fields.invalidateRects();
         int validatedCount = 1;
         bool emptyFlag = false;

         for (int pass=0;validatedCount > 0 && !emptyFlag;pass++)
         {
            validatedCount = 0;
            emptyFlag = true;

            for (POSITION pos = m_fields.GetHeadPosition();pos != NULL;)
            {
               CResizingDialogField* field = m_fields.GetNext(pos);

               if (field->isRectValid())
               {
                  continue;
               }

               if (! field->areDependenciesValid())
               {
                  continue;
               }

               if (field->isHidden())
               {
                  continue;
               }

               emptyFlag = false;

               //CString message;
               //message.Format("pass=%d, controlId=%d\n",pass,field->getControlId());
               //OutputDebugString(message);

               pWnd = GetDlgItem(field->getControlId());

               if (pWnd == NULL || ! ::IsWindow(*pWnd))
               {
                  continue;
               }

               validatedCount++;

               // for debugging
               //CRect originalWindowRect;
               //pWnd->GetWindowRect(&originalWindowRect);
               //ScreenToClient(&originalWindowRect);

               CRect originalFieldRect = field->getOriginalRect();
               CRect fieldRect = originalFieldRect;

               CSize newSize = originalFieldRect.Size();
               POSITION opPos;

               for (opPos = field->getOperations().GetHeadPosition();opPos != NULL;)
               {
                  CDialogFieldOperation* fieldOperation = field->getOperations().GetNext(opPos);

                  switch (fieldOperation->getOperation())
                  {
                  case growBoth:
                     newSize.cx += dx;
                     newSize.cy += dy;
                     break;
                  case growHorizontal:
                     newSize.cx += dx;
                     break;
                  case growVertical:
                     newSize.cy += dy;
                     break;
                  case growProportionalBoth:
                     newSize.cx = (int)((fx * newSize.cx) + .5);
                     newSize.cy = (int)((fy * newSize.cy) + .5);
                     break;
                  case growProportionalHorizontal:
                     newSize.cx = (int)((fx * newSize.cx) + .5);
                     break;
                  case growProportionalVertical:
                     newSize.cy = (int)((fy * newSize.cy) + .5);
                     break;
                  }
               }

               for (opPos = field->getOperations().GetHeadPosition();opPos != NULL;)
               {
                  CDialogFieldOperation* fieldOperation = field->getOperations().GetNext(opPos);

                  switch (fieldOperation->getOperation())
                  {
                  case anchorLeft:     
                     fieldRect.right  = fieldRect.left   + newSize.cx;
                     fieldRect.bottom = fieldRect.top    + newSize.cy;
                     break;
                  case anchorRight:     
                     fieldRect.OffsetRect(dx,0);  
                     fieldRect.left   = fieldRect.right  - newSize.cx;
                     fieldRect.bottom = fieldRect.top    + newSize.cy;
                     break;
                  case anchorTop:
                     fieldRect.right  = fieldRect.left   + newSize.cx;
                     fieldRect.bottom = fieldRect.top    + newSize.cy;
                     break;
                  case anchorBottom:
                     fieldRect.OffsetRect(0,dy);  
                     fieldRect.right  = fieldRect.left   + newSize.cx;
                     fieldRect.top    = fieldRect.bottom - newSize.cy;
                     break;
                  case anchorBottomLeft:
                     fieldRect.OffsetRect(0,dy);  
                     fieldRect.right  = fieldRect.left   + newSize.cx;
                     fieldRect.top    = fieldRect.bottom - newSize.cy;
                     break;
                  case anchorBottomRight:
                     fieldRect.OffsetRect(dx,dy);  
                     fieldRect.left   = fieldRect.right  - newSize.cx;
                     fieldRect.top    = fieldRect.bottom - newSize.cy;
                     break;
                  case anchorProportionalBoth:
                  case anchorProportionalHorizontal:
                  case anchorProportionalVertical:
                     {
                        CPoint rectCenter(originalFieldRect.left + originalFieldRect.Width()  / 2,
                                          originalFieldRect.top  + originalFieldRect.Height() / 2 );
                        CPoint newRectCenter(rectCenter);

                        newRectCenter.x = (int)((fx * newRectCenter.x) + .5);
                        newRectCenter.y = (int)((fy * newRectCenter.y) + .5);

                        if (fieldOperation->getOperation() == anchorProportionalBoth ||
                            fieldOperation->getOperation() == anchorProportionalHorizontal     )
                        {
                           fieldRect.left   = newRectCenter.x  - newSize.cx/2;
                           fieldRect.right  = fieldRect.left   + newSize.cx;
                        }

                        if (fieldOperation->getOperation() == anchorProportionalBoth ||
                            fieldOperation->getOperation() == anchorProportionalVertical     )
                        {
                           fieldRect.top    = newRectCenter.y  - newSize.cy/2;
                           fieldRect.bottom = fieldRect.top    + newSize.cy;
                        }
                     }

                     break;
                  case anchorRelativeToField:
                     {
                        CResizingDialogField* relatedField = fieldOperation->getRelatedField();

                        if (relatedField != NULL)
                        {
                           CPoint originalFieldCenter = calcRectCenter(field->getOriginalRect());
                           CPoint originalRelatedFieldCenter = calcRectCenter(relatedField->getOriginalRect());
                           CSize offset = originalFieldCenter - originalRelatedFieldCenter;

                           CPoint relatedFieldCenter = calcRectCenter(relatedField->getRect());
                           CPoint newFieldCenter = relatedFieldCenter + offset;

                           fieldRect.left   = newFieldCenter.x  - newSize.cx/2;
                           fieldRect.top    = newFieldCenter.y  - newSize.cy/2;
                           fieldRect.right  = fieldRect.left    + newSize.cx;
                           fieldRect.bottom = fieldRect.top     + newSize.cy;
                        }
                     }

                     break;
                  }

                  field->setRect(fieldRect);

                  CResizingDialogField* relatedField = fieldOperation->getRelatedField();

                  if (relatedField != NULL)
                  {
                     if (fieldOperation->getOperation() == growRelativeToFieldHorizontal || 
                         fieldOperation->getOperation() == growRelativeToFieldBoth          )
                     {
                        CPoint originalFieldCenter = calcRectCenter(field->getOriginalRect());
                        CPoint originalRelatedFieldCenter = calcRectCenter(relatedField->getOriginalRect());
                        CSize originalOffset = originalFieldCenter - originalRelatedFieldCenter;

                        CPoint fieldCenter = calcRectCenter(field->getRect());
                        CPoint relatedFieldCenter = calcRectCenter(relatedField->getRect());
                        CSize offset = fieldCenter - relatedFieldCenter;

                        CSize deltaOffset = offset - originalOffset;

                        if (deltaOffset.cx < 0)
                        {
                           fieldRect.right -= deltaOffset.cx;
                        }
                        else
                        {
                           fieldRect.left  -= deltaOffset.cx;
                        }
                     }

                     if (fieldOperation->getOperation() == growRelativeToFieldVertical || 
                         fieldOperation->getOperation() == growRelativeToFieldBoth          )
                     {
                        CPoint originalFieldCenter = calcRectCenter(field->getOriginalRect());
                        CPoint originalRelatedFieldCenter = calcRectCenter(relatedField->getOriginalRect());
                        CSize originalOffset = originalFieldCenter - originalRelatedFieldCenter;

                        CPoint fieldCenter = calcRectCenter(field->getRect());
                        CPoint relatedFieldCenter = calcRectCenter(relatedField->getRect());
                        CSize offset = fieldCenter - relatedFieldCenter;

                        CSize deltaOffset = offset - originalOffset;

                        if (deltaOffset.cy < 0)
                        {
                           fieldRect.bottom -= deltaOffset.cy;
                        }
                        else
                        {
                           fieldRect.top    -= deltaOffset.cy;
                        }
                     }

                     if (fieldOperation->getModifier() != modifierUndefined)
                     {
                        int originalValue,value;
                        CPoint originalPoint,point;

                        switch (fieldOperation->getModifier())
                        {
                        case toLeftEdge:    
                           originalValue = relatedField->getOriginalRect().left;    
                           value         = relatedField->getRect().left;    
                           break;
                        case toRightEdge:   
                           originalValue = relatedField->getOriginalRect().right;   
                           value         = relatedField->getRect().right;   
                           break;
                        case toTopEdge:     
                           originalValue = relatedField->getOriginalRect().top;     
                           value         = relatedField->getRect().top;     
                           break;
                        case toBottomEdge:  
                           originalValue = relatedField->getOriginalRect().bottom;  
                           value         = relatedField->getRect().bottom;  
                           break;
                        case toTopLeftCorner:  
                           originalPoint = relatedField->getOriginalRect().TopLeft();  
                           point         = relatedField->getRect().TopLeft();  
                           break;
                        case toTopRightCorner:  
                           originalPoint = CPoint(relatedField->getOriginalRect().right,relatedField->getOriginalRect().top);  
                           point         = CPoint(relatedField->getRect().right,relatedField->getRect().top);  
                           break;
                        case toBottomLeftCorner:  
                           originalPoint = CPoint(relatedField->getOriginalRect().left,relatedField->getOriginalRect().bottom);  
                           point         = CPoint(relatedField->getRect().left,relatedField->getRect().bottom);  
                           break;
                        case toBottomRightCorner:  
                           originalPoint = relatedField->getOriginalRect().BottomRight();  
                           point         = relatedField->getRect().BottomRight();  
                           break;
                        }

                        switch (fieldOperation->getOperation())
                        {
                        case glueLeftEdge:
                           fieldRect.left   = (field->getOriginalRect().left   - originalValue) + value;
                           break;
                        case glueRightEdge:
                           fieldRect.right  = (field->getOriginalRect().right  - originalValue) + value;
                           break;
                        case glueTopEdge:
                           fieldRect.top    = (field->getOriginalRect().top    - originalValue) + value;
                           break;
                        case glueBottomEdge:
                           fieldRect.bottom = (field->getOriginalRect().bottom - originalValue) + value;
                           break;
                        case glueTopLeftCorner:
                           fieldRect.left   = (field->getOriginalRect().left   - originalPoint.x) + point.x;
                           fieldRect.top    = (field->getOriginalRect().top    - originalPoint.y) + point.y;
                           break;
                        case glueTopRightCorner:
                           fieldRect.right  = (field->getOriginalRect().right  - originalPoint.x) + point.x;
                           fieldRect.top    = (field->getOriginalRect().top    - originalPoint.y) + point.y;
                           break;
                        case glueBottomLeftCorner:
                           fieldRect.left   = (field->getOriginalRect().left   - originalPoint.x) + point.x;
                           fieldRect.bottom = (field->getOriginalRect().bottom - originalPoint.y) + point.y;
                           break;
                        case glueBottomRightCorner:
                           fieldRect.right  = (field->getOriginalRect().right  - originalPoint.x) + point.x;
                           fieldRect.bottom = (field->getOriginalRect().bottom - originalPoint.y) + point.y;
                           break;
                        }
                     }

                     field->setRect(fieldRect);
                  }
               }

               hDwp = ::DeferWindowPos(hDwp,*pWnd,0,fieldRect.left,fieldRect.top,
                  fieldRect.Width(),fieldRect.Height(),SWP_NOZORDER);
            }
         }

         ::EndDeferWindowPos(hDwp);
      }

      m_previousClientSize = CSize(cx,cy);
   }
}

//
// OnSize()
// Set the dialog controls new position and size
//
void CResizingDialog::OnMove(int x, int y) 
{
   CDialog::OnMove(x, y);
   m_stateChanged = true;
}

void CResizingDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
   if (m_minWidth == 0) // first time
   {
      CDialog::OnGetMinMaxInfo(lpMMI);
      return;
   }

   lpMMI->ptMinTrackSize.x = m_minWidth;
   lpMMI->ptMinTrackSize.y = m_minHeight;
}

void CResizingDialog::OnDestroy() 
{
   // Save the size of the dialog box, so next time
   // we'll start with this size
   if (m_rememberState && m_stateChanged && 
       m_previousClientSize.cx != 0 && m_previousClientSize.cy != 0)
   {
      saveWindowState();
   }

   // Important: Reset the internal values in case of reuse of the dialog
   // with out deleting.
   m_minWidth = m_minHeight = m_previousClientSize.cx = m_previousClientSize.cy = 0;
   m_stateChanged = false;

   CDialog::OnDestroy();
}

//
// OnCreate()
//
int CResizingDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CDialog::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   if (m_allowResize)
   {
      ModifyStyle(0,WS_THICKFRAME);
   }
   else
   {
      ModifyStyle(WS_THICKFRAME,0);
   }

   // Remember the original size so later we can calculate
   // how to place the controls on dialog Resize
   m_minWidth  = lpCreateStruct->cx;
   m_minHeight = lpCreateStruct->cy;

   return 0;
}

CSize CResizingDialog::getMinMaxSize()
{
   CSize size(m_minWidth,m_minHeight);

   return size;
}

void CResizingDialog::setMinMaxSize(CSize& size)
{
   m_minWidth  = size.cx;
   m_minHeight = size.cy;
}

void CResizingDialog::adjustSizeForToolBar(int toolBarHeight)
{
   //m_toolBarHeight = toolBarHeight;

   m_minHeight += toolBarHeight;
   //m_originalWindowSize.cy += toolbarHeight;
   m_originalClientSize.cy += toolBarHeight;

   CSize offset;
   offset.cx = 0;
   offset.cy = toolBarHeight;

   m_fields.offsetOriginalRects(offset);
}

//
// OnNcHitTest
// Handle mouse over the gripper
//
// Credit: Tommy Svensson
//
NCHITTEST_UNIT CResizingDialog::OnNcHitTest(CPoint point)
{
   NCHITTEST_UNIT ht = CDialog::OnNcHitTest(point);

   if (ht == HTCLIENT && m_drawGripper)
   {
      CRect rc;
      GetWindowRect( rc );
      rc.left = rc.right  - GetSystemMetrics(SM_CXHSCROLL);
      rc.top  = rc.bottom - GetSystemMetrics(SM_CYVSCROLL);

      if( rc.PtInRect(point))
      {
         ht = HTBOTTOMRIGHT;
      }
   }

   return ht;
}

//
// GetDialogProfileEntry()
// Override this (virtual) function in your derived class
// if you want to store the dialog info under a different entry name.
//
// Credit: Ari Greenberg
CString CResizingDialog::GetDialogProfileEntry()
{
   // By default store the size under the Dialog ID value (Hex)
   CString retval;
   retval.Format("Dialog %x",m_nIDTemplate);

   return retval;
}
