// $Header: /CAMCAD/4.3/PopupMenu.cpp 1     10/15/03 6:34p Kurt Van Ness $

/*
History: PopupMenu.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 1/03/00    Time: 12:35a
 * Updated in $/CdControl
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 10/08/98   Time: 4:17a
 * Created in $/LayerCopy
 * Initial Add
*/

#include "StdAfx.h"
#include "PopupMenu.h"

IMPLEMENT_DYNCREATE(CPopupMenu,CMenu)

//___________________________________________________________________________________________
CPopupMenu::CPopupMenu()
{
   CreatePopupMenu();
}

CPopupMenu::~CPopupMenu()
{
}

bool CPopupMenu::addItem(UINT messageId,const CString& itemString,bool enabled)
{
   bool retval = true;
   UINT flags = MF_UNCHECKED | MF_STRING;

   flags |= (enabled ? MF_ENABLED : MF_GRAYED);

   if (!AppendMenu(flags,messageId,(const char*)itemString))
   {
      AfxThrowUserException();
      retval = false;
   }

   return retval;
}

bool CPopupMenu::addSeparator()
{
   bool retval = true;

   if (!AppendMenu(MF_SEPARATOR))
   {
      AfxThrowUserException();
      retval = false;
   }

   return retval;
}

bool CPopupMenu::trackMenu(UINT flags,CPoint clientPoint,CWnd* parentWnd)
{
   bool retval = true;

   CPoint screenPoint = clientPoint;
   parentWnd->ClientToScreen(&screenPoint);

   //if (!popupMenu.TrackPopupMenu(TPM_LEFTALIGN,screenPoint.x,screenPoint.y,GetParent()))
   if (!TrackPopupMenu(TPM_LEFTALIGN,screenPoint.x,screenPoint.y,parentWnd))
   {
      AfxThrowUserException();
      retval = false;
   }

   return retval;
}

