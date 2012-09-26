// $Header: /CAMCAD/4.5/PersistantColorDialog.cpp 1     5/02/06 3:14p Kurt Van Ness $

#include "StdAfx.h"
#include "PersistantColorDialog.h"
#include "RwLib.h"
#include "Colors.h"

//_____________________________________________________________________________
CPersistantColorDialog::CPersistantColorDialog(COLORREF initialColor,DWORD flags,CWnd* parentWnd)
: CColorDialog(initialColor,flags,parentWnd)
{
   init();
}

void CPersistantColorDialog::init()
{
   for (int index = 0;index < m_customColorCount;index++)
   {
      m_customColors[index] = colorWhite;
   }

   restoreState();

   m_cc.lpCustColors = m_customColors;
}

void CPersistantColorDialog::saveState() 
{
   CString dialogName("CPersistantColorDialog");
   CString customColorsParameter;

   for (int index = 0;index < m_customColorCount;index++)
   {
      customColorsParameter.AppendFormat("%d ",m_customColors[index]);
   }

   AfxGetApp()->WriteProfileString(dialogName,"Custom Colors",customColorsParameter);   
}

void CPersistantColorDialog::restoreState() 
{
   CString dialogName("CPersistantColorDialog");
   CSupString customColorsParameter = AfxGetApp()->GetProfileString(dialogName,"Custom Colors","");
   CStringArray params;

   int numPar = customColorsParameter.Parse(params);

   customColorsParameter.Parse(params);

   for (int index = 0;index < m_customColorCount;index++)
   {
      COLORREF color = colorWhite;

      if (index < numPar)
      {
         color = atoi(params.GetAt(index));
      }

      m_customColors[index] = color;
   }
}

BEGIN_MESSAGE_MAP(CPersistantColorDialog,CColorDialog)
   //{{AFX_MSG_MAP(CResizingDialog)
   ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
// CPersistantColorDialog message handlers


BOOL CPersistantColorDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   return TRUE;
}

void CPersistantColorDialog::OnDestroy() 
{
   saveState();

   CColorDialog::OnDestroy();
}


