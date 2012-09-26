// $Header: /CAMCAD/4.5/AssignGraphicClassToLayer.cpp 11    4/28/06 2:12p Kurt Van Ness $

// AssignGraphicClassToLayer.cpp : implementation file
//

#include "stdafx.h"
#include "AssignGraphicClassToLayer.h"
#include "dbutil.h"
#include "ccdoc.h"
#include "DcaEnumIterator.h"

/******************************************************************************
* OnAssignGrClass
*/
void CCEtoODBDoc::OnAssignGrClass() 
{
   AssignGraphicClassToLayer dlg(this);
   if (dlg.DoModal() != IDOK)
   {
      return;
   }

}
// AssignGraphicClassToLayer dialog

IMPLEMENT_DYNAMIC(AssignGraphicClassToLayer, CDialog)
AssignGraphicClassToLayer::AssignGraphicClassToLayer(CWnd* pParent /*=NULL*/)
   : CDialog(AssignGraphicClassToLayer::IDD, pParent)
{
}
AssignGraphicClassToLayer::AssignGraphicClassToLayer(CCEtoODBDoc *document)
   : CDialog(AssignGraphicClassToLayer::IDD, NULL)
{
   doc = document;
}

AssignGraphicClassToLayer::~AssignGraphicClassToLayer()
{
}

void AssignGraphicClassToLayer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_COMBO_LYRS, m_layerCombo);
   DDX_Control(pDX, IDC_COMBO_GR_CLASSES, m_graphicCombo);
}


BEGIN_MESSAGE_MAP(AssignGraphicClassToLayer, CDialog)
   ON_BN_CLICKED(IDC_BTN_ASSIGN, OnBnClickedBtnAssign)
END_MESSAGE_MAP()


// AssignGraphicClassToLayer message handlers

BOOL AssignGraphicClassToLayer::OnInitDialog() 
{
   CDialog::OnInitDialog(); 
   for (int i=0; i < doc->getLayerArray().GetCount(); i++)
   {
      LayerStruct* layer = doc->getLayerArray()[i];
      if (!layer)
         continue;
      m_layerCombo.AddString(layer->getName());
   }  


   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      m_graphicCombo.InsertString(grclass, graphicClassToString(grclass));
   }
   
   return TRUE;
}

void AssignGraphicClassToLayer::OnBnClickedBtnAssign()
{
   for (int i=0; i< doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      
      if (!block)
         continue;
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         CString layerName;

         if (m_layerCombo.GetCurSel() < 0 || m_graphicCombo.GetCurSel() < 0)
            return;

         m_layerCombo.GetLBText(m_layerCombo.GetCurSel(), layerName);
               
         LayerStruct *layer = doc->FindLayer_by_Name(layerName);
         if (!layer)
            continue;
         if (data->getDataType() == T_POLY && data->getLayerIndex() == layer->getLayerIndex())
         {
            data->setGraphicClass(m_graphicCombo.GetCurSel());           
         }
      }
   }
   ErrorMessage("Graphic class successfully assigned to the selected layer.","",0);
}

