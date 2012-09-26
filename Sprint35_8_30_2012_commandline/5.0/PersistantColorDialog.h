// $Header: /CAMCAD/4.5/PersistantColorDialog.h 1     5/02/06 3:14p Kurt Van Ness $

#if !defined(__PersistantColorDialog_h__)
#define __PersistantColorDialog_h__

#pragma once

//_____________________________________________________________________________
class CPersistantColorDialog : public CColorDialog
{
private:
   static const int m_customColorCount = 16;
   COLORREF m_customColors[m_customColorCount];

// Construction
public:
	CPersistantColorDialog(COLORREF initialColor = 0,DWORD flags = 0,CWnd* parentWnd = NULL);

   void init();

	// Generated message map functions
protected:
   void saveState();
   void restoreState();

   // Generated message map functions
   //{{AFX_MSG(CResizingDialog)
   virtual BOOL OnInitDialog();
   afx_msg void OnDestroy();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

#endif // !defined(__PersistantColorDialog_h__)

