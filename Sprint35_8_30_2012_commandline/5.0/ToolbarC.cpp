// $Header: /CAMCAD/4.6/ToolbarC.cpp 11    4/04/07 4:07p Rick Faltersack $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ToolBarC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar
CCustomToolBar::CCustomToolBar(CFrameWnd& parentFrame)
: CMyToolBar(parentFrame)
{
   maxButtonArray = 0;
}

CCustomToolBar::~CCustomToolBar()
{
}

BEGIN_MESSAGE_MAP(CCustomToolBar, CMyToolBar)
   //{{AFX_MSG_MAP(CCustomToolBar)
   ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_TB_CUSTOMIZE, OnCustomize)
   ON_WM_DESTROY()
   //}}AFX_MSG_MAP
   ON_NOTIFY_REFLECT(TBN_BEGINADJUST, OnToolBarBeginAdjust)
   ON_NOTIFY_REFLECT(TBN_BEGINDRAG, OnToolBarBeginDrag)
   ON_NOTIFY_REFLECT(TBN_CUSTHELP, OnToolBarCustomHelp)
   ON_NOTIFY_REFLECT(TBN_ENDADJUST, OnToolBarEndAdjust)
   ON_NOTIFY_REFLECT(TBN_ENDDRAG, OnToolBarEndDrag)
   ON_NOTIFY_REFLECT(TBN_GETBUTTONINFO, OnToolBarGetButtonInfo)
   ON_NOTIFY_REFLECT(TBN_QUERYDELETE, OnToolBarQueryDelete)
   ON_NOTIFY_REFLECT(TBN_QUERYINSERT, OnToolBarQueryInsert)
   ON_NOTIFY_REFLECT(TBN_RESET, OnToolBarReset)
   ON_NOTIFY_REFLECT(TBN_TOOLBARCHANGE, OnToolBarChange)
END_MESSAGE_MAP()

/******************************************************************************
* Create
*/
BOOL CCustomToolBar::Create(CWnd *pParentWnd, DWORD dwStyle, UINT nID)
{
   BOOL success; // indicates if the toolbar was created

   // if default processing is ok
   if (CMyToolBar::Create(pParentWnd, dwStyle, nID))
   {
      // indicate success
      success = TRUE;

      // modify the style to include adjustable
      ModifyStyle(0, CCS_ADJUSTABLE);
   }

   // else default processing failed
   else
   {
      TRACE0("Failed to create toolbar\n");
      success = FALSE;
   }

   // indicate success
   return success;
}


/******************************************************************************
* OnDestroy
*/
void CCustomToolBar::OnDestroy() 
{
   free(buttonArray);

   // default processing
   CMyToolBar::OnDestroy();
}


/******************************************************************************
* SaveState
*
* This function saves the state (visible buttons, toolbar position, etc.)
* of the toolbar, using the registry.
*/
void CCustomToolBar::SaveState()
{
   // save the toolbar state to the registry
   CString toolbar;
   if (tbNum == 1)
      toolbar = "Custom Toolbar1";
   else
      toolbar = "Custom Toolbar2";

   CString keyname = "Software\\" + (CString)REGISTRY_COMPANY + "\\CAMCAD\\Custom Toolbar";

   GetToolBarCtrl().SaveState(HKEY_CURRENT_USER, keyname, toolbar);
}


/******************************************************************************
* RestoreState
*
* This function restores the state (visible buttons, toolbar position, etc.)
* of the toolbar, using the registry.
*/
void CCustomToolBar::RestoreState()
{
   // restore the toolbar state from the registry
   CString toolbar;
   if (tbNum == 1)
      toolbar = "Custom Toolbar1";
   else
      toolbar = "Custom Toolbar2";

   CString keyname = "Software\\" +(CString)REGISTRY_COMPANY + "\\CAMCAD\\Custom Toolbar";

   GetToolBarCtrl().RestoreState(HKEY_CURRENT_USER, keyname, toolbar);
   GetToolBarCtrl().AutoSize();
}


/******************************************************************************
* OnContextMenu
*/
void CCustomToolBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   CMenu     menu;      // toolbar right-click menu

   // load the menu from resources
   VERIFY(menu.LoadMenu(IDR_TOOLBAR_MENU));

   /// track the menu as a pop-up
   CMenu* pPopup = menu.GetSubMenu(0);
   ASSERT(pPopup != NULL);

   pPopup->AppendMenu(0, ID_TB_CUSTOMIZE, "&Customize");

   // force all message in this menu to be sent here
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


/******************************************************************************
* OnCustomize
*/
void CCustomToolBar::OnCustomize() 
{
   // let user play with customization dialog
   GetToolBarCtrl().Customize();
   GetToolBarCtrl().SetRows(1, FALSE, NULL);
   GetToolBarCtrl().AutoSize();
}



/******************************************************************************
* Notification Functions                                                      *
*******************************************************************************/


// This function is called when the user begins dragging a toolbar
// button or when the customization dialog is being populated with
// toolbar information.  Basically, *result should be populated with
// your answer to the question, "is the user allowed to delete this
// button?".
void CCustomToolBar::OnToolBarQueryDelete(NMHDR *notify, LRESULT *result)
{
   *result = TRUE;
}


// This function is called when the user begins dragging a toolbar
// button or when the customization dialog is being populated with
// toolbar information.  Basically, *result should be populated with
// your answer to the question, "is the user allowed to insert a
// button to the left of this one?".
void CCustomToolBar::OnToolBarQueryInsert(NMHDR *notify, LRESULT *result)
{
   *result = TRUE;
}


// This function is called whenever the user makes a change to the
// layout of the toolbar.  Calling the mainframe's RecalcLayout forces
// the toolbar to repaint itself.
void CCustomToolBar::OnToolBarChange(NMHDR *notify, LRESULT *result)
{
   // force the frame window to recalculate the size
   GetParentFrame()->RecalcLayout();
}


// This function is called when the user begins dragging a toolbar button.
void CCustomToolBar::OnToolBarBeginDrag(NMHDR *notify, LRESULT *result)
{
}


// This function is called when the user has completed a dragging operation.
void CCustomToolBar::OnToolBarEndDrag(NMHDR *notify, LRESULT *result)
{
}


// This function is called when the user initially calls up the toolbar
// customization dialog box.
void CCustomToolBar::OnToolBarBeginAdjust(NMHDR *notify, LRESULT *result)
{
}


// This function is called when the user clicks on the help button on the
// toolbar customization dialog box.
void CCustomToolBar::OnToolBarCustomHelp(NMHDR *notify, LRESULT *result)
{
}


// This function is called when the user dismisses the toolbar customization
// dialog box.
void CCustomToolBar::OnToolBarEndAdjust(NMHDR *notify, LRESULT *result)
{
   // save the state of the toolbar for reinitialization
// SaveState();
}


//TBBUTTON button = {0, ID_EMPTY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0};
// This function is called to populate the toolbar customization dialog box
// with information regarding all of the possible toolbar buttons.
void CCustomToolBar::OnToolBarGetButtonInfo(NMHDR *notify, LRESULT *result)
{
   NMTOOLBAR *p; // data needed by customize dialog box

   // init the pointer
   p = (NMTOOLBAR *)notify;

   // if the index is valid
   if (p->iItem < maxButtonArray)
   {
      // copy the stored button structure
      p->tbButton = buttonArray[p->iItem];

      // copy the text for the button label in the dialog
//    strcpy(p->pszText, "test");

      // indicate valid data was sent
      *result = TRUE;
   }

   // else there is no button for this index
   else
   {
      *result = FALSE;
   }
}


// This function is called when the user clicks on the reset button on the
// toolbar customization dialog box.
void CCustomToolBar::OnToolBarReset(NMHDR *notify, LRESULT *result)
{
   // restore the toolbar to the way it was before entering customization
// RestoreState();

   int count = GetToolBarCtrl().GetButtonCount();
   while (count--)
      GetToolBarCtrl().DeleteButton(0);

   GetToolBarCtrl().AddButtons(maxButtonArray, buttonArray);
}


void CCustomToolBar::RemeberButtons()
{
   maxButtonArray = GetToolBarCtrl().GetButtonCount();

   buttonArray = (TBBUTTON*)calloc(maxButtonArray, sizeof(TBBUTTON));

   for (int i=0; i<maxButtonArray; i++)
   {
      TBBUTTON tbButton;
      GetToolBarCtrl().GetButton(i, &tbButton);
      memcpy(&buttonArray[i], &tbButton, sizeof(TBBUTTON));
   }
}
