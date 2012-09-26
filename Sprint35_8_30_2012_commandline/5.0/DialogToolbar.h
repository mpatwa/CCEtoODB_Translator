// $Header: /CAMCAD/4.5/DialogToolbar.h 5     6/06/06 2:28p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#if ! defined (__DialogToolbar_h__)
#define __DialogToolbar_h__

#pragma once

#include "ResizingDialog.h"
#include "ResizingPropertySheet.h"

//_____________________________________________________________________________
class CResizingDialogToolBar : public CToolBar
{
private:
   CResizingDialog& m_parentDialog;

public:
   CResizingDialogToolBar(CResizingDialog& parentDialog);

   bool createAndLoad(UINT toolBarResourceId);
   void enable(bool enableFlag);
};

//_____________________________________________________________________________

class CResizingPropertyPageToolBar : public CToolBar
{
private:
   CResizingPropertyPage& m_parentPropPage;

public:
   CResizingPropertyPageToolBar(CResizingPropertyPage& parentPropPage);

   bool createAndLoad(UINT toolBarResourceId);
   void enable(bool enableFlag);
};

#endif
