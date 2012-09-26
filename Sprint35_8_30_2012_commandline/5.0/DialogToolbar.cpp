// $Header: /CAMCAD/4.5/DialogToolbar.cpp 7     6/06/06 2:28p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "StdAfx.h"
#include "Resource.h"
#include "DialogToolbar.h"

//_____________________________________________________________________________
CResizingDialogToolBar::CResizingDialogToolBar(CResizingDialog& parentDialog) :
   m_parentDialog(parentDialog)
{
}

bool CResizingDialogToolBar::createAndLoad(UINT toolBarResourceId)
{
   bool retval = (Create(&m_parentDialog) != 0 && LoadToolBar(toolBarResourceId) != 0);

   if (retval)
   {
      SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY);

      CRect	rcClientOld; // Old Client Rect
      CRect	rcClientNew; // New Client Rect with Tollbar Added
      m_parentDialog.GetClientRect(rcClientOld); // Retrive the Old Client WindowSize

      // Called to reposition and resize control bars in the client 
      // area of a window. The reposQuery FLAG does not really draw the 
      // Toolbar.  It only does the calculations and puts the new 
      // ClientRect values in rcClientNew so we can do the rest of the 
      // Math.
      m_parentDialog.RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery,rcClientNew);

      // All of the Child Windows (Controls) now need to be moved so 
      // the Tollbar does not cover them up. Offest to move all child 
      // controls after adding Toolbar
      CPoint ptOffset(rcClientNew.left - rcClientOld.left,rcClientNew.top - rcClientOld.top);

      CRect	rcChild;

      HDWP hDwp = ::BeginDeferWindowPos(40);

      // Cycle through all child controls
      for (CWnd* pwndChild = m_parentDialog.GetWindow(GW_CHILD);pwndChild != NULL;) 
      {
         int id = pwndChild->GetDlgCtrlID();

         pwndChild->GetWindowRect(rcChild); // Get the child control RECT
         m_parentDialog.ScreenToClient(rcChild); 

         // Changes the Child Rect by the values of the calculated offset
         rcChild.OffsetRect(ptOffset); 
         //pwndChild->MoveWindow(rcChild,FALSE); // Move the Child Control
         hDwp = ::DeferWindowPos(hDwp,*pwndChild,0,rcChild.left,rcChild.top,rcChild.Width(),rcChild.Height(),SWP_NOZORDER);
         pwndChild = pwndChild->GetNextWindow();
      }

      ::EndDeferWindowPos(hDwp);

      CRect	rcWindow;
      m_parentDialog.GetWindowRect(rcWindow); // Get the RECT of the Dialog

      // Increase width to new Client Width
      rcWindow.right += rcClientOld.Width() - rcClientNew.Width(); 

      // Increase height to new Client Height
      rcWindow.bottom += rcClientOld.Height() - rcClientNew.Height(); 

      m_parentDialog.adjustSizeForToolBar(ptOffset.y);
      //CSize minMaxSize = getMinMaxSize();
      //minMaxSize.cy += ptOffset.y;
      //setMinMaxSize(minMaxSize);

      m_parentDialog.MoveWindow(rcWindow,FALSE); // Redraw Window

      // Now we REALLY Redraw the Toolbar
      m_parentDialog.RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

      //m_toolTips.Create(&m_toolBar,TTS_ALWAYSTIP );
      //m_toolTips.Activate(TRUE);
      //m_toolTips.AddTool(GetDlgItem(IDC_LockCentroids),"Lock Centroids");
   }

   return retval;
}

void CResizingDialogToolBar ::enable(bool enableFlag)
{
   CToolBarCtrl& toolBarControl = GetToolBarCtrl();
   TBBUTTON buttonInfo;

   for (int buttonIndex = 0; buttonIndex < toolBarControl.GetButtonCount() ;buttonIndex++)
   {
      toolBarControl.GetButton(buttonIndex,&buttonInfo);
      int commandId = buttonInfo.idCommand;

      if (commandId != ID_SEPARATOR)
      {
         toolBarControl.EnableButton(commandId, enableFlag?TRUE:FALSE);
      }
   }
}

//_____________________________________________________________________________

CResizingPropertyPageToolBar::CResizingPropertyPageToolBar(CResizingPropertyPage& parentPage) :
   m_parentPropPage(parentPage)
{
}

bool CResizingPropertyPageToolBar::createAndLoad(UINT toolBarResourceId)
{
   bool retval = (Create(&m_parentPropPage) != 0 && LoadToolBar(toolBarResourceId) != 0);

   if (retval)
   {
      SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY);

      CRect	rcClientOld; // Old Client Rect
      CRect	rcClientNew; // New Client Rect with Tollbar Added
      m_parentPropPage.GetClientRect(rcClientOld); // Retrive the Old Client WindowSize

      // Called to reposition and resize control bars in the client 
      // area of a window. The reposQuery FLAG does not really draw the 
      // Toolbar.  It only does the calculations and puts the new 
      // ClientRect values in rcClientNew so we can do the rest of the 
      // Math.
      m_parentPropPage.RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery,rcClientNew);

      // All of the Child Windows (Controls) now need to be moved so 
      // the Tollbar does not cover them up. Offest to move all child 
      // controls after adding Toolbar
      CPoint ptOffset(rcClientNew.left - rcClientOld.left,rcClientNew.top - rcClientOld.top);

      CRect	rcChild;

      HDWP hDwp = ::BeginDeferWindowPos(40);

      // Cycle through all child controls
      for (CWnd* pwndChild = m_parentPropPage.GetWindow(GW_CHILD);pwndChild != NULL;) 
      {
         int id = pwndChild->GetDlgCtrlID();

         pwndChild->GetWindowRect(rcChild); // Get the child control RECT
         m_parentPropPage.ScreenToClient(rcChild); 

         // Changes the Child Rect by the values of the calculated offset
         rcChild.OffsetRect(ptOffset); 
         //pwndChild->MoveWindow(rcChild,FALSE); // Move the Child Control
         hDwp = ::DeferWindowPos(hDwp,*pwndChild,0,rcChild.left,rcChild.top,rcChild.Width(),rcChild.Height(),SWP_NOZORDER);
         pwndChild = pwndChild->GetNextWindow();
      }

      ::EndDeferWindowPos(hDwp);

      CRect	rcWindow;
      m_parentPropPage.GetWindowRect(rcWindow); // Get the RECT of the Dialog

      // Increase width to new Client Width
      rcWindow.right += rcClientOld.Width() - rcClientNew.Width(); 

      // Increase height to new Client Height
      rcWindow.bottom += rcClientOld.Height() - rcClientNew.Height(); 

      m_parentPropPage.adjustSizeForToolBar(ptOffset.y);
      //CSize minMaxSize = getMinMaxSize();
      //minMaxSize.cy += ptOffset.y;
      //setMinMaxSize(minMaxSize);

      m_parentPropPage.MoveWindow(rcWindow,FALSE); // Redraw Window

      // Now we REALLY Redraw the Toolbar
      m_parentPropPage.RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

      //m_toolTips.Create(&m_toolBar,TTS_ALWAYSTIP );
      //m_toolTips.Activate(TRUE);
      //m_toolTips.AddTool(GetDlgItem(IDC_LockCentroids),"Lock Centroids");
   }

   return retval;
}

void CResizingPropertyPageToolBar ::enable(bool enableFlag)
{
   CToolBarCtrl& toolBarControl = GetToolBarCtrl();
   TBBUTTON buttonInfo;

   for (int buttonIndex = 0; buttonIndex < toolBarControl.GetButtonCount() ;buttonIndex++)
   {
      toolBarControl.GetButton(buttonIndex,&buttonInfo);
      int commandId = buttonInfo.idCommand;

      if (commandId != ID_SEPARATOR)
      {
         toolBarControl.EnableButton(commandId, enableFlag?TRUE:FALSE);
      }
   }
}
