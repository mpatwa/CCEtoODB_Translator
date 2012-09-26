// $Header: /CAMCAD/4.6/UltimateGrid2005/UGDrgDrp.h 1     5/08/06 8:28p Kurt Van Ness $

/*************************************************************************
				Class Declaration : CUGDropTarget
**************************************************************************
	Source file : UGDrgDrp.cpp
	Header file : UGDrgDrp.h
	Copyright © The Code Project 1994 - 2002, All Rights Reserved

	Purpose
		The CUGDropTarget class is used by the Ultimate
		Grid to provide the drag-and-drop functionality.
		An instance of this object is only created when
		the 'AfxOle.h' is included and OLE initialized.

		This class receives messages related to the
		drag-and-drop and passed them to the CUGCtrl
		derived class for further processing.  It will
		also take care of the copy and paste functionality
		if needed.
*************************************************************************/
#include "ugctrl.h"

#ifndef _UGDrgDrp_H_
#define _UGDrgDrp_H_

#ifdef UG_ENABLE_DRAGDROP

class UG_CLASS_DECL CUGDropTarget: public COleDropTarget
{
public:
	CUGDropTarget();
	~CUGDropTarget();

	//pointer to the main class
	CUGCtrl	*m_ctrl;

	virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDropEx( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point );

	virtual DROPEFFECT OnDragScroll( CWnd* pWnd, DWORD dwKeyState, CPoint point );
};

#endif // UG_ENABLE_DRAGDROP
#endif // _UGDrgDrp_H_