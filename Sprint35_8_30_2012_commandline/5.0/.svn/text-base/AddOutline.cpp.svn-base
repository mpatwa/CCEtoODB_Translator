// AddOutline.CPP

#include "stdafx.h"
#include "AddOutline.h"
#include "Graph.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool IsOutlinePresent( CCEtoODBDoc *doc, CDataList *DataList,BlockTypeTag BlockType);
//-----------------------------------------------------------------------------
// CAddOutlineDlg
//-----------------------------------------------------------------------------
//IMPLEMENT_DYNAMIC(CAddOutlineDlg, CResizingDialog)
CAddOutlineDlg::CAddOutlineDlg(CCEtoODBDoc& doc, FileStruct& pcbFile, const double x, const double y)
: CResizingDialog(CAddOutlineDlg::IDD, NULL)
, m_doc(doc)
, m_File(pcbFile)
, m_xLocation(x)
, m_yLocation(y)
{     
}

CAddOutlineDlg::~CAddOutlineDlg()
{
}

void CAddOutlineDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_XLocation, m_xLocationEditbox);
   DDX_Control(pDX, IDC_YLocation, m_yLocationEditbox);
   DDX_Control(pDX, IDC_XOFFSET, m_xOffsetEditbox);
   DDX_Control(pDX, IDC_YOFFSET, m_yOffsetEditbox);
}


BEGIN_MESSAGE_MAP(CAddOutlineDlg, CResizingDialog)
   ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
   ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
   ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CAddOutlineDlg::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   CString xLocation, yLocation,xOffset,yOffset;
   xLocation.Format("%0.3f", m_xLocation);
   xOffset.Format("%0.3f", m_xOffset);
   yLocation.Format("%0.3f", m_yLocation);
   yOffset.Format("%0.3f", m_yOffset);
   m_xLocationEditbox.SetWindowText(xLocation);
   m_yLocationEditbox.SetWindowText(yLocation);
   m_xOffsetEditbox.SetWindowText(xOffset);
   m_yOffsetEditbox.SetWindowText(yOffset);
   if(m_File.getBlockType() == blockTypePanel)
      SetWindowText("Add Panel Outline");
   else if(m_File.getBlockType() == blockTypePcb)
      SetWindowText("Add Board Outline");
   return FALSE; 
}

void CAddOutlineDlg::OnBnClickedAdd()
{
   UpdateData(TRUE);

   CString tmpXLoc, tmpYLoc, tmpXOff, tmpYOff;
   m_xLocationEditbox.GetWindowText(tmpXLoc);
   m_yLocationEditbox.GetWindowText(tmpYLoc);
   m_xOffsetEditbox.GetWindowText(tmpXOff);
   m_yOffsetEditbox.GetWindowText(tmpYOff);
   double XOff, YOff, XLoc, YLoc;
   XLoc = atof(tmpXLoc);
   YLoc = atof(tmpYLoc);
   XOff = atof(tmpXOff);
   YOff = atof(tmpYOff);
   if(XLoc == 0.0 || YLoc == 0.0)
   {
      if(m_File.getBlockType() == blockTypePanel)
         AfxMessageBox("Incorrect Panel Outline dimensions.");
      else if(m_File.getBlockType() == blockTypePcb)
         AfxMessageBox("Incorrect Board Outline dimensions.");
      return;
   }

   int layer = 0;
   LayerStruct *ll = NULL;
   //Layer Name
   if(m_File.getBlockType() == blockTypePanel)
   {
      ll = FindLayerByLayerType(LAYTYPE_PANEL_OUTLINE);
      if(NULL == ll)
      {
         int i = 0;
         CString LayerName = "PANEL_OUTLINE";
         while(IsLayerPresent(LayerName))
         {
            CString temp;
            temp.Format("%d",++i);
            LayerName += "_" + temp;
         }
         layer = Graph_Level(LayerName,"",0);
         LayerStruct *l = m_doc.FindLayer(layer);
         l->setLayerType(LAYTYPE_PANEL_OUTLINE);
      }
      else
      {
         layer = ll->getLayerIndex();
      }
   }
   else if(m_File.getBlockType() == blockTypePcb)
   {
      ll = FindLayerByLayerType(LAYTYPE_BOARD_OUTLINE);
      if(NULL == ll)
      {
         int i = 0;
         CString LayerName = "BOARD_OUTLINE";
         while(IsLayerPresent(LayerName))
         {
            CString temp;
            temp.Format("%d",++i);
            LayerName += "_" + temp;
         }
         layer = Graph_Level(LayerName,"",0);
         LayerStruct *l = m_doc.FindLayer(layer);
         l->setLayerType(LAYTYPE_BOARD_OUTLINE);
      }
      else
      {
         layer = ll->getLayerIndex();
      }
   }

   m_doc.PrepareAddEntity(&m_File);
   DataStruct *data = Graph_PolyStruct(layer, 0, 0);
   Graph_Poly(NULL, 0, 0, 0, 1);
   Graph_Vertex(XOff, YOff,0.0);
   Graph_Vertex(XOff, YOff + YLoc,0.0);
   Graph_Vertex(XOff + XLoc, YOff + YLoc,0.0);
   Graph_Vertex(XOff + XLoc, YOff, 0.0);
   Graph_Vertex(XOff, YOff,0.0);

   //Set the Graphic Class
   if(m_File.getBlockType() == blockTypePanel)
      data->setGraphicClass(GR_CLASS_PANELOUTLINE);
   else if(m_File.getBlockType() == blockTypePcb)
      data->setGraphicClass(GR_CLASS_BOARDOUTLINE);
   
   //Redraw
   m_doc.OnRedraw();
   OnClose();
}

void CAddOutlineDlg::OnBnClickedClose()
{
   OnClose();
}

void CAddOutlineDlg::OnClose()
{
   CResizingDialog::OnClose();
   EndDialog(IDCANCEL);
}

//--------------------------------------------------------------
/*
*/
bool IsOutlinePresent( CCEtoODBDoc *doc, CDataList *DataList,BlockTypeTag BlockType)
{
   DataStruct *np;
   int         mirror = 0;
   int         layer;
   LayerStruct *ll;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         layer = np->getLayerIndex();
         ll = doc->FindLayer(layer);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            if (((np->getGraphicClass() == GR_CLASS_BOARDOUTLINE) && (BlockType == blockTypePcb))
               ||((np->getGraphicClass() == GR_CLASS_PANELOUTLINE) && (BlockType == blockTypePanel)))
            {               
               return true;
            }
         }
         break;
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (!block->isAperture() && IsOutlinePresent(doc, &(block->getDataList()),BlockType))
                  return true;
         }
         break;                                                                
      } // end switch
   } // end IsOutlinePresent 
   return false;
}

LayerStruct * CAddOutlineDlg::FindLayerByLayerType(int LayerType)
{
   LayerStruct *layer = NULL;
   for (int i=0; i<m_doc.getMaxLayerIndex(); i++)
   {  
      LayerStruct *layer = m_doc.getLayer(i);
      if(NULL != layer)
      {
         if(layer->getLayerType() == LayerType)
            return layer;
      }
   }
   return NULL;
}

bool CAddOutlineDlg::IsLayerPresent(CString LayerName)
{
   bool found = false;
   for (int i=0; i<m_doc.getMaxLayerIndex(); i++)
   {  
      LayerStruct *layer = m_doc.getLayer(i);
      if(NULL != layer)
      {
         if(!LayerName.Compare(layer->getName()))
         {
            found = true;
            return found;
         }
      }
   }
   return found;
}
