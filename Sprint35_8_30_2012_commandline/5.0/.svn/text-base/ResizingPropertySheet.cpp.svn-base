// $Header: /CAMCAD/4.5/ResizingPropertySheet.cpp 9     2/22/07 2:51p Rick Faltersack $

#include "StdAfx.h"
#include "ResizingPropertySheet.h"
#include "RwLib.h"

CWnd* getMainWnd();

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CResizingPropertySheet, CPropertySheet)

CResizingPropertySheet::CResizingPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)	 
	 , m_bNeedInit(true)
	 , m_minWidth(0)
	 , m_minHeight(0)
{
   m_caption.Format("PropertySheet %x",nIDCaption);

   m_allowResize   = true;
   m_rememberState = true;
   m_drawGripper   = m_allowResize;
   m_previousClientSize = CSize(0,0);
}

CResizingPropertySheet::CResizingPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)	 
	 , m_bNeedInit(true)
	 , m_minWidth(0)
	 , m_minHeight(0)
    , m_caption(pszCaption)
{
   m_allowResize   = true;
   m_rememberState = true;
   m_drawGripper   = m_allowResize;
   m_previousClientSize = CSize(0,0);
}

CResizingPropertySheet::~CResizingPropertySheet()
{
}

void CResizingPropertySheet::setAllowResize(bool allowResize)
{
   m_allowResize = allowResize;
   m_drawGripper = allowResize;

   Invalidate();
}

// This function must be a STATIC method. 
// Callback to allow you to set the default window styles 
// for the property sheet.
int CALLBACK CResizingPropertySheet::XmnPropSheetCallback(HWND hWnd, UINT message, LPARAM lParam)
{
   extern int CALLBACK AfxPropSheetCallback(HWND, UINT message, LPARAM lParam);

   // XMN: Call MFC's callback.
   int nRes = AfxPropSheetCallback(hWnd, message, lParam);

   switch (message)
   {
   case PSCB_PRECREATE:
      // Set your own window styles.
      ((LPDLGTEMPLATE)lParam)->style |= (DS_3DLOOK | DS_SETFONT
         | WS_THICKFRAME | WS_SYSMENU | WS_POPUP | WS_VISIBLE | WS_CAPTION);
      break;
   }

   return nRes;
}

// By overriding DoModal, you can hook the callback to
// the prop sheet creation.
INT_PTR CResizingPropertySheet::DoModal(void)
{
   // Hook into property sheet creation code
   m_psh.dwFlags |= PSH_USECALLBACK;
   m_psh.pfnCallback = XmnPropSheetCallback;
   
   return CPropertySheet::DoModal();
}

CString CResizingPropertySheet::GetDialogProfileEntry()
{
   // By default store the size under the Dialog ID value (Hex)
   CString retval;
   retval.Format("PropertySheet %s",m_caption);

   return retval;
}

void CResizingPropertySheet::saveWindowState() 
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

void CResizingPropertySheet::restoreWindowState() 
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

         if (! m_allowResize)
         {
            CRect clientRect;
            GetClientRect(&clientRect);

            cx = clientRect.Width();
            cy = clientRect.Height();
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

BEGIN_MESSAGE_MAP(CResizingPropertySheet, CPropertySheet)
   ON_WM_SIZE()
   ON_WM_MOVE()
   ON_WM_GETMINMAXINFO()
   ON_WM_DESTROY()
END_MESSAGE_MAP()


// CResizingPropertySheet message handlers

BOOL CResizingPropertySheet::OnInitDialog()
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   CRect r;  
   GetWindowRect(&r);

   // Init m_minWidth/Y
   m_minWidth = r.Width();
   m_minHeight = r.Height();

   // After this point, the resize code runs.
   m_bNeedInit = false;
   GetClientRect(&m_rCrt);

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

   return bResult;
}

// Handle WM_SIZE events by resizing the tab control and by 
// moving all of the buttons on the property sheet.
void CResizingPropertySheet::OnSize(UINT nType, int cx, int cy) 
{
   if (!m_allowResize)
   {
      CRect clientRect;
      GetClientRect(&clientRect);
      CPropertySheet::OnSize(nType,clientRect.Width(),clientRect.Height());
   }
   else
   {
      CRect r1; 
      CPropertySheet::OnSize(nType, cx, cy);

      if (m_bNeedInit)
         return;

      CTabCtrl *pTab = GetTabControl();

      if (pTab != NULL && IsWindow(pTab->m_hWnd))
      {
         //ASSERT(NULL != pTab && IsWindow(pTab->m_hWnd));
          
         int dx = cx - m_rCrt.Width();
         int dy = cy - m_rCrt.Height();
         GetClientRect(&m_rCrt);

         HDWP hDWP = ::BeginDeferWindowPos(5);

         pTab->GetClientRect(&r1); 
         r1.right += dx; r1.bottom += dy;

         ::DeferWindowPos(hDWP, pTab->m_hWnd, NULL,
                        0, 0, r1.Width(), r1.Height(),
                        SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);

         // Move all of the buttons with the lower and right sides.
         for (CWnd *pChild = GetWindow(GW_CHILD);
            pChild != NULL;
            pChild = pChild->GetWindow(GW_HWNDNEXT))
         {

            if (pChild->SendMessage(WM_GETDLGCODE) & DLGC_BUTTON)
            {
               pChild->GetWindowRect(&r1); ScreenToClient(&r1); 
               r1.top += dy; r1.bottom += dy; r1.left+= dx; r1.right += dx;
               ::DeferWindowPos(hDWP, pChild->m_hWnd, NULL,
                              r1.left, r1.top, 0, 0,
                              SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
            }      
            else // Resize everything else.
            {
               pChild->GetClientRect(&r1); 

	            r1.right += dx; 
               r1.bottom += dy;

	            ::DeferWindowPos(hDWP, pChild->m_hWnd, NULL, 0, 0, 
                           r1.Width(), r1.Height(),
                           SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
            }
         }

         ::EndDeferWindowPos(hDWP);

         m_previousClientSize = CSize(cx,cy);
      }
   }
}

void CResizingPropertySheet::OnMove(int x,int y) 
{
   CPropertySheet::OnMove(x,y);

   CRect clientRect;
   GetClientRect(clientRect);

   m_previousClientSize.cx = clientRect.Width();
   m_previousClientSize.cy = clientRect.Height();
}

void CResizingPropertySheet::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
   CPropertySheet::OnGetMinMaxInfo(lpMMI);

   lpMMI->ptMinTrackSize.x = m_minWidth;
   lpMMI->ptMinTrackSize.y = m_minHeight;
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CResizingPropertyPage, CPropertyPage)

CResizingPropertyPage::CResizingPropertyPage(UINT nIDTemplate) : 
   CPropertyPage(nIDTemplate)
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
   //m_drawGripper   = m_allowResize;
   //m_toolBarHeight = 0;
}

CResizingDialogField& CResizingPropertyPage::addFieldControl(int controlId,
   DialogFieldOperationTag anchorValue,DialogFieldOperationTag growValue)
{
   CResizingDialogField& field = m_fields.addField(*this,controlId,anchorValue,growValue);

   return field;
}

CResizingDialogField& CResizingPropertyPage::addFieldControl(int controlId,
   DialogFieldOperationTag anchorValue,CResizingDialogField* anchorRelatedField,DialogFieldOperationTag growValue)
{
   CResizingDialogField& field = m_fields.addField(*this,controlId,anchorValue,growValue);

   if (anchorRelatedField != NULL)
   {
      field.getOperations().addOperation(anchorValue,anchorRelatedField);
   }

   return field;
}

void CResizingPropertyPage::initDialog()
{
   CRect originalWindowRect;
   GetWindowRect(originalWindowRect);
   m_originalWindowSize = originalWindowRect.Size();

	for (POSITION pos = m_fields.GetHeadPosition();pos != NULL;)
   {
      CResizingDialogField* field = m_fields.GetNext(pos);

		CWnd* pWnd = GetDlgItem(field->getControlId());

      if (pWnd != NULL)
      {
		   CRect rect;
         pWnd->GetWindowRect(&rect);
         ScreenToClient(&rect);
         field->setOriginalRect(rect);
      }
      else
      {
         formatMessageBox("Could not find CWnd for control id %d in CResizingPropertyPage::initDialog()",field->getControlId());
      }
   }

   m_initializedWindowDataFlag = true;

   //if (m_rememberState)
   //{
   //   restoreWindowState();
	//}
}

BEGIN_MESSAGE_MAP(CResizingPropertyPage, CPropertyPage)
   //{{AFX_MSG_MAP(CResizingPropertyPage)
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
// CResizingPropertyPage message handlers

//////////////////////////////////////////////////////////////////////////
// OnInitDialog()
//
BOOL CResizingPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   initDialog();
      
   return FALSE;  // return TRUE  unless you set the focus to a control
}

//
// OnSetActive()
//
BOOL CResizingPropertyPage::OnSetActive()
{
   return CPropertyPage::OnSetActive();
}

//
// OnSize()
// Set the dialog controls new position and size
//
void CResizingPropertyPage::OnSize(UINT nType, int cx, int cy) 
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

      CPropertyPage::OnSize(nType, cx, cy);
      
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

         //bool drawGripper = (m_drawGripper && ::IsWindow(m_gripper));

         //HDWP hDwp = ::BeginDeferWindowPos(m_fields.GetCount() + (drawGripper ? 1 : 0));
         HDWP hDwp = ::BeginDeferWindowPos(m_fields.GetCount());

         // Move and Size the controls using the information
         // we got in SetControlInfo()
         //
         m_stateChanged = true;
         CRect WndRect;
         CWnd *pWnd;

         //if (drawGripper && ::IsWindow(m_gripper))
         //{
         //   m_gripper.GetWindowRect(&WndRect);  
         //   ScreenToClient(&WndRect);
         //   
         //   WndRect.OffsetRect(dx2,dy2);

         //   hDwp = ::DeferWindowPos(hDwp,m_gripper,0,WndRect.left,WndRect.top,WndRect.Width(),WndRect.Height(),SWP_NOZORDER);
         //}

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

               CRect fieldRect;
               pWnd->GetWindowRect(&fieldRect);
               ScreenToClient(&fieldRect);

               CRect originalFieldRect = field->getOriginalRect();
               fieldRect = originalFieldRect;

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
void CResizingPropertyPage::OnMove(int x, int y) 
{
   CPropertyPage::OnMove(x, y);
   m_stateChanged = true;
}

void CResizingPropertyPage::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
   if (m_minWidth == 0) // first time
   {
      CPropertyPage::OnGetMinMaxInfo(lpMMI);
      return;
   }

   lpMMI->ptMinTrackSize.x = m_minWidth;
   lpMMI->ptMinTrackSize.y = m_minHeight;
}

void CResizingPropertyPage::OnDestroy() 
{
   // Save the size of the dialog box, so next time
   // we'll start with this size
   //if (m_rememberState && m_stateChanged && 
   //    m_previousClientSize.cx != 0 && m_previousClientSize.cy != 0)
   //{
   //   saveWindowState();
   //}

   // Important: Reset the internal values in case of reuse of the dialog
   // with out deleting.
   m_minWidth = m_minHeight = m_previousClientSize.cx = m_previousClientSize.cy = 0;
   m_stateChanged = false;

   CPropertyPage::OnDestroy();
}

//
// OnCreate()
//
int CResizingPropertyPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   //if (m_allowResize)
   //{
   //   ModifyStyle(0,WS_THICKFRAME);
   //}
   //else
   //{
   //   ModifyStyle(WS_THICKFRAME,0);
   //}

   // Remember the original size so later we can calculate
   // how to place the controls on dialog Resize
   m_minWidth  = lpCreateStruct->cx;
   m_minHeight = lpCreateStruct->cy;

   return 0;
}

CSize CResizingPropertyPage::getMinMaxSize()
{
   CSize size(m_minWidth,m_minHeight);

   return size;
}

void CResizingPropertyPage::setMinMaxSize(CSize& size)
{
   m_minWidth  = size.cx;
   m_minHeight = size.cy;
}

void CResizingPropertyPage::adjustSizeForToolBar(int toolBarHeight)
{
   //m_toolBarHeight = toolBarHeight;

   m_minHeight += toolBarHeight;
   //m_originalWindowSize.cy += toolbarHeight;
   m_originalClientSize.cy += toolBarHeight;
}

//
// OnNcHitTest
// Handle mouse over the gripper
//
// Credit: Tommy Svensson
//
NCHITTEST_UNIT CResizingPropertyPage::OnNcHitTest(CPoint point)
{
   NCHITTEST_UNIT ht = CPropertyPage::OnNcHitTest(point);

   //if (ht == HTCLIENT && m_drawGripper)
   //{
   //   CRect rc;
   //   GetWindowRect( rc );
   //   rc.left = rc.right  - GetSystemMetrics(SM_CXHSCROLL);
   //   rc.top  = rc.bottom - GetSystemMetrics(SM_CYVSCROLL);

   //   if( rc.PtInRect(point))
   //   {
   //      ht = HTBOTTOMRIGHT;
   //   }
   //}

   return ht;
}

//
// GetDialogProfileEntry()
// Override this (virtual) function in your derived class
// if you want to store the dialog info under a different entry name.
//
// Credit: Ari Greenberg
CString CResizingPropertyPage::GetDialogProfileEntry()
{
   // By default store the size under the Dialog ID value (Hex)
   CString retval;
   retval.Format("Dialog %x",m_nIDTemplate);

   return retval;
}

void CResizingPropertySheet::OnDestroy()
{
   // Save the size of the dialog box, so next time
   // we'll start with this size
   if (m_rememberState && /*m_stateChanged && */
       m_previousClientSize.cx != 0 && m_previousClientSize.cy != 0)
   {
      saveWindowState();
   }

   // Important: Reset the internal values in case of reuse of the dialog
   // with out deleting.
   m_minWidth = m_minHeight = m_previousClientSize.cx = m_previousClientSize.cy = 0;
   m_bNeedInit = true;
   //m_stateChanged = false;

   CPropertySheet::OnDestroy();
}
