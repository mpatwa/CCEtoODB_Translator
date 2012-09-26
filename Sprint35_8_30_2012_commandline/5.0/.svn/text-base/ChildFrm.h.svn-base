// $Header: /CAMCAD/4.3/ChildFrm.h 3     8/12/03 9:05p Kurt Van Ness $

#pragma once

// CChildFrame frame

class CChildFrame : public CMDIChildWnd
{
   DECLARE_DYNCREATE(CChildFrame)

private:
   WINDOWPLACEMENT lastWindowPlacement;

protected:
   CChildFrame();           // protected constructor used by dynamic creation
   virtual ~CChildFrame();

protected:
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnMove(int x, int y);
};


