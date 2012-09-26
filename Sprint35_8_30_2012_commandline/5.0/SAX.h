// $Header: /CAMCAD/4.3/SAX.h 7     8/12/03 9:06p Kurt Van Ness $

//{{AFX_INCLUDES()
#include "sbpro.h"
//}}AFX_INCLUDES

#if !defined(AFX_SAX_H__1B5BDD43_6F14_11D2_9866_004005408E44__INCLUDED_)
#define AFX_SAX_H__1B5BDD43_6F14_11D2_9866_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define sbcFileNew              0       // create a new macro 
#define sbcFileNewCodeModule    47      // create a new code module 
#define sbcFileNewObjectModule  48      // create a new object module 
#define sbcFileNewClassModule   49      // create a new class module 
#define sbcFileOpen             1       // open an existing macro/module 
#define sbcFileClose            33      // close the current macro/module 
#define sbcFileSave             2       // save the current macro/module 
#define sbcFileSaveAs           3       // save the current macro/module with a new name 
#define sbcFileSaveAll          34      // save all the macros/modules 
#define sbcFilePrint            4       // print the current macro/module 
#define sbcFilePrintSetup       5       // set the printer and options 
#define sbcEditUndo             22      // undo the last edit 
#define sbcEditRedo             63      // undo the last edit 
#define sbcEditCut              23      // cut the selected text to the clipboard 
#define sbcEditCopy             24      // copy the selected text to the clipboard 
#define sbcEditPaste            25      // paste the clipboard text 
#define sbcEditDelete           30      // delete the selected text 
#define sbcEditSelectAll        31      // select all of the text 
#define sbcEditIndent           51      // move the selected lines right 
#define sbcEditOutdent          52      // move the selected lines left 
#define sbcEditTabAsSpaces      65      // toggle tab as spaces on/off 
#define sbcEditFind             26      // find a text string 
#define sbcEditReplace          27      // find and replace a text string 
#define sbcEditAgain            28      // repeat the last find/replace 
#define sbcEditUserDialog       32      // edit a UserDialog 
#define sbcEditReferences       64      // edit references 
#define sbcEditProperties       50      // edit the object or class module properties 
#define sbcViewMacro            54      // activate the macro editing window 
#define sbcViewImmediate        55      // show the immediate evaluation window 
#define sbcViewWatch            56      // show the watch expression window 
#define sbcViewStack            57      // show the call stack window 
#define sbcViewLoaded           58      // show the loaded macros/modules window 
#define sbcViewToolbar          59      // toggle toolbar on/off 
#define sbcViewStatusBar        60      // toggle status bar on/off 
#define sbcViewEditButtons      61      // toggle edit buttons on/off 
#define sbcViewAlwaysSplit      62      // toggle the split on/off 
#define sbcViewFont             29      // put up the font selection dialog 
#define sbcViewTabWidth1        66      // put up the font selection dialog
#define sbcViewTabWidth2        67      // put up the font selection dialog
#define sbcViewTabWidth3        68      // put up the font selection dialog
#define sbcViewTabWidth4        69      // put up the font selection dialog
#define sbcViewTabWidth5        70      // put up the font selection dialog
#define sbcViewTabWidth6        71      // put up the font selection dialog
#define sbcViewTabWidth7        72      // put up the font selection dialog
#define sbcViewTabWidth8        73      // put up the font selection dialog
#define sbcMacroRun             6       // start or resume the current macro/module 
#define sbcMacroPause           7       // pause the current macro/module (can resume) 
#define sbcMacroEnd             8       // end the current macro/module (can't resume) 
#define sbcDebugStepInto        9       // step into the next statement 
#define sbcDebugStepOver        10      // step over the next statement 
#define sbcDebugStepOut         35      // step out of the current procedure 
#define sbcDebugStepTo          11      // step to the statement under the cursor 
#define sbcDebugToggleBreak     12      // toggle break point on the current line 
#define sbcDebugClearAllBreaks  53      // clear all the break points 
#define sbcDebugQuickWatch      13      // view variables and expressions in the immediate window 
#define sbcDebugAddWatch        14      // add variables and expressions to the watch window 
#define sbcDebugBrowse          15      // view the selected object's methods and properties 
#define sbcDebugSetNext         16      // set current line as the next statement 
#define sbcDebugShowNext        17      // show the next statement 
#define sbcSheetOpenUses        36      // open '#Uses modules in the current macro/module 
#define sbcSheetCloseAll        37      // close all the macros/modules 
#define sbcSheet1               38      // show this macro/module 
#define sbcSheet2               39      // show this macro/module 
#define sbcSheet3               40      // show this macro/module 
#define sbcSheet4               41      // show this macro/module 
#define sbcSheet5               42      // show this macro/module 
#define sbcSheet6               43      // show this macro/module 
#define sbcSheet7               44      // show this macro/module 
#define sbcSheet8               45      // show this macro/module 
#define sbcSheet9               46      // show this macro/module 
#define sbcHelpApp              18      // display help 
#define sbcHelpLanguage         19      // display help for the basic language 
#define sbcHelpTopic            20      // display help for the selected word 
#define sbcHelpAbout            21      // about box 
#define sbcPopupNoMouse         0xH7FFC // popup menu at top left 
#define sbcShowError            0xH7FFD // show the current error line 
#define sbcNewModulePopup       0xH7FFE // new module popup menu 
#define sbcPopup                0xH7FFF // popup menu at mouse 

/////////////////////////////////////////////////////////////////////////////
// SAX dialog
class SAX : public CDialog
{
// Construction
public:
   SAX(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(SAX)
   enum { IDD = IDD_SAX_BASIC };
   CSbpro   m_sbpro;
   //}}AFX_DATA
   LPDISPATCH dispatch;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SAX)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(SAX)
   virtual BOOL OnInitDialog();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   virtual void OnCancel();
   afx_msg void OnEditReferences();
   afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
   afx_msg void OnCmdExit();
   //}}AFX_MSG
   afx_msg void OnUpdateMenu(CCmdUI* pCmdUI = NULL);
   afx_msg void OnCommand(UINT id);
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAX_H__1B5BDD43_6F14_11D2_9866_004005408E44__INCLUDED_)
