// $Header: /CAMCAD/4.5/LyrStackup.cpp 16    4/28/06 2:14p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "LyrStackup.h"
#include "pcbutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
* OnLayerStackup
*/
void CCEtoODBDoc::OnLayerStackup() 
{
   LayerStackup dlg;
   dlg.doc = this;
   dlg.DoModal(); 
}

/******************************************************************************
* ShowStackup
*/
void ShowStackup(CCEtoODBDoc *doc, int stackNum, int maxStackup)
{
   for (int i=0;i < doc->getMaxLayerIndex();i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

      if (layer == NULL) continue;

      if (layer->getElectricalStackNumber() != 0)
      {
         if (layer->getElectricalStackNumber() == stackNum)
         {
            layer->setVisible(true);
            layer->setEditable(true);
         }
      }
      else
      {
         switch (layer->getLayerType())
         {
         case LAYTYPE_PAD_INNER:
         case LAYTYPE_SIGNAL_INNER:
            if (stackNum > 1 && stackNum < maxStackup)
            {
               layer->setVisible(true);
               layer->setEditable(true);
            }
            break;

         case LAYTYPE_SIGNAL_TOP:
         case LAYTYPE_PAD_TOP:
            if (stackNum == 1)
            {
               layer->setVisible(true);
               layer->setEditable(true);
            }
            break;

         case LAYTYPE_SIGNAL_BOT:
         case LAYTYPE_PAD_BOTTOM:
            if (stackNum == maxStackup)
            {
               layer->setVisible(true);
               layer->setEditable(true);
            }
            break;

         case LAYTYPE_SIGNAL_ALL:
         case LAYTYPE_PAD_ALL:
            layer->setVisible(true);
            layer->setEditable(true);
            break;

         case LAYTYPE_PAD_OUTER:
         case LAYTYPE_SIGNAL_OUTER:
            if (stackNum == 1 || stackNum == maxStackup)
            {
               layer->setVisible(true);
               layer->setEditable(true);
            }
            break;

         case LAYTYPE_BOARD_OUTLINE:
            layer->setVisible(true);
            layer->setEditable(true);
            break;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// LayerStackup dialog
LayerStackup::LayerStackup(CWnd* pParent /*=NULL*/)
   : CResizingDialog(LayerStackup::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerStackup)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   addFieldControl(IDC_APPLY ,anchorBottom);
   addFieldControl(IDCANCEL  ,anchorBottom);
   addFieldControl(IDC_LIST1 ,anchorLeft  ,growBoth);
}

void LayerStackup::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerStackup)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LayerStackup, CResizingDialog)
   //{{AFX_MSG_MAP(LayerStackup)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   ON_LBN_DBLCLK(IDC_LIST1, OnApply)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerStackup message handlers
BOOL LayerStackup::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   CString layerIdentifier;   
   maxStackup = GetMaxStackupNum(doc);

   for (int stackUpIndex=1;stackUpIndex <= maxStackup;stackUpIndex++)
   {
      if (stackUpIndex == 1)
      {
         layerIdentifier = "Top";
      }
      else if (stackUpIndex == maxStackup)
      {
         layerIdentifier = "Bottom";
      }
      else
      {
         layerIdentifier.Format("%d",stackUpIndex);
      }

      m_list.SetItemData(m_list.AddString(layerIdentifier),stackUpIndex);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerStackup::OnApply() 
{
   int selectionCount = m_list.GetSelCount();

   for (int layerIndex=0;layerIndex < doc->getMaxLayerIndex();layerIndex++)
   {
      LayerStruct* layer = doc->getLayerArray()[layerIndex];

      if (layer == NULL) continue;

      layer->setVisible(false);
      layer->setEditable(false);
   }

   if (selectionCount != 0)
   {
      int* sel = (int*)calloc(selectionCount, sizeof(int));

      if (sel == NULL)
      {
         ErrorMessage("Memory allocation failure", "LayerStackup::OnApply");
      }
      else
      {
         m_list.GetSelItems(selectionCount, sel);

         for (int selectionIndex=0;selectionIndex < selectionCount;selectionIndex++)
         {
            ShowStackup(doc, m_list.GetItemData(sel[selectionIndex]), maxStackup);
         }

         free(sel);
      }
   }

   doc->OnRegen();
   doc->UpdateAllViews(NULL);
}

