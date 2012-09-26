// $Header: /CAMCAD/4.3/PopupMenu.h 1     10/15/03 6:34p Kurt Van Ness $

/*
History: PopupMenu.h $
 * 
 * *****************  Version 3  *****************
 * User: Kvanness     Date: 5/24/03    Time: 10:06a
 * Updated in $/CdControl
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

#if !defined(__PopupMenu_H__)
#define __PopupMenu_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//___________________________________________________________________________________________
class CPopupMenu : public CMenu
{
   DECLARE_DYNCREATE(CPopupMenu)

public:
   CPopupMenu();           
   ~CPopupMenu();           

// Attributes
public:

// Operations
public:
   bool addItem(UINT messageId,const CString& itemString,bool enabled=true);
   bool addSeparator();
   bool trackMenu(UINT flags,CPoint clientPoint,CWnd* parentWnd);
   
};

/////////////////////////////////////////////////////////////////////////////
#endif
