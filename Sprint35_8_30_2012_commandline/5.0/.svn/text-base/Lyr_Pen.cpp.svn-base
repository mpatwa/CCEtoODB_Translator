// $Header: /CAMCAD/4.5/Lyr_Pen.cpp 12    4/28/06 2:14p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "lyr_pen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct LayerArrStruct 
{
   LayerStruct *layer;
   int pen;
};

static LayerArrStruct *LayerArr;
static int count;

/****************************************************************************
* Get_Layer_PenNum
*   function to be used in HP OUT.CPP to get the pen num of each layer
*/
int Get_Layer_PenNum(int layerNum)
{
   for (int i=0; i<count; i++)
      if (layerNum == LayerArr[i].layer->getLayerIndex())
         return LayerArr[i].pen;
   return 0;
}

/****************************************************************************
* Free_Layer_PenMap */
void Free_Layer_PenMap()
{
   free(LayerArr);
}

/////////////////////////////////////////////////////////////////////////////
// LayerPenMap dialog
LayerPenMap::LayerPenMap(CWnd* pParent /*=NULL*/)
   : CDialog(LayerPenMap::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerPenMap)
   m_layer1 = _T("");
   m_layer2 = _T("");
   m_layer3 = _T("");
   m_layer4 = _T("");
   m_layer5 = _T("");
   m_layer6 = _T("");
   m_layer7 = _T("");
   m_layer8 = _T("");
   //}}AFX_DATA_INIT
}

void LayerPenMap::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerPenMap)
   DDX_Control(pDX, IDC_COMBO8, m_pen8);
   DDX_Control(pDX, IDC_COMBO7, m_pen7);
   DDX_Control(pDX, IDC_COMBO6, m_pen6);
   DDX_Control(pDX, IDC_COMBO5, m_pen5);
   DDX_Control(pDX, IDC_COMBO4, m_pen4);
   DDX_Control(pDX, IDC_COMBO3, m_pen3);
   DDX_Control(pDX, IDC_COMBO2, m_pen2);
   DDX_Control(pDX, IDC_COMBO1, m_pen1);
   DDX_Control(pDX, IDC_SCROLLBAR1, m_scroll);
   DDX_Text(pDX, IDC_LAYER1, m_layer1);
   DDX_Text(pDX, IDC_LAYER2, m_layer2);
   DDX_Text(pDX, IDC_LAYER3, m_layer3);
   DDX_Text(pDX, IDC_LAYER4, m_layer4);
   DDX_Text(pDX, IDC_LAYER5, m_layer5);
   DDX_Text(pDX, IDC_LAYER6, m_layer6);
   DDX_Text(pDX, IDC_LAYER7, m_layer7);
   DDX_Text(pDX, IDC_LAYER8, m_layer8);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LayerPenMap, CDialog)
   //{{AFX_MSG_MAP(LayerPenMap)
   ON_WM_VSCROLL()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerPenMap message handlers

BOOL LayerPenMap::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_pen1.AddString("Pen1");
   m_pen1.AddString("Pen2");
   m_pen1.AddString("Pen3");
   m_pen1.AddString("Pen4");
   m_pen1.AddString("Pen5");
   m_pen1.AddString("Pen6");
   m_pen1.AddString("Pen7");
   m_pen1.AddString("Pen8");

   m_pen2.AddString("Pen1");
   m_pen2.AddString("Pen2");
   m_pen2.AddString("Pen3");
   m_pen2.AddString("Pen4");
   m_pen2.AddString("Pen5");
   m_pen2.AddString("Pen6");
   m_pen2.AddString("Pen7");
   m_pen2.AddString("Pen8");

   m_pen3.AddString("Pen1");
   m_pen3.AddString("Pen2");
   m_pen3.AddString("Pen3");
   m_pen3.AddString("Pen4");
   m_pen3.AddString("Pen5");
   m_pen3.AddString("Pen6");
   m_pen3.AddString("Pen7");
   m_pen3.AddString("Pen8");

   m_pen4.AddString("Pen1");
   m_pen4.AddString("Pen2");
   m_pen4.AddString("Pen3");
   m_pen4.AddString("Pen4");
   m_pen4.AddString("Pen5");
   m_pen4.AddString("Pen6");
   m_pen4.AddString("Pen7");
   m_pen4.AddString("Pen8");

   m_pen5.AddString("Pen1");
   m_pen5.AddString("Pen2");
   m_pen5.AddString("Pen3");
   m_pen5.AddString("Pen4");
   m_pen5.AddString("Pen5");
   m_pen5.AddString("Pen6");
   m_pen5.AddString("Pen7");
   m_pen5.AddString("Pen8");

   m_pen6.AddString("Pen1");
   m_pen6.AddString("Pen2");
   m_pen6.AddString("Pen3");
   m_pen6.AddString("Pen4");
   m_pen6.AddString("Pen5");
   m_pen6.AddString("Pen6");
   m_pen6.AddString("Pen7");
   m_pen6.AddString("Pen8");
         
   m_pen7.AddString("Pen1");
   m_pen7.AddString("Pen2");
   m_pen7.AddString("Pen3");
   m_pen7.AddString("Pen4");
   m_pen7.AddString("Pen5");
   m_pen7.AddString("Pen6");
   m_pen7.AddString("Pen7");
   m_pen7.AddString("Pen8");

   m_pen8.AddString("Pen1");
   m_pen8.AddString("Pen2");
   m_pen8.AddString("Pen3");
   m_pen8.AddString("Pen4");
   m_pen8.AddString("Pen5");
   m_pen8.AddString("Pen6");
   m_pen8.AddString("Pen7");
   m_pen8.AddString("Pen8");

   // do not use doc->getMaxLayerIndex(), there may be switched off layers
   count = 0;
	int j=0;
   for (j=0; j< doc->getMaxLayerIndex(); j++)
   {
      if (doc->getLayerArray()[j] == NULL)  continue;
      if (!doc->getLayerArray()[j]->isVisible())   continue;
      count++;
   }

   if (count > 8)
      m_scroll.SetScrollRange(0, count - 8);
   else 
      m_scroll.SetScrollRange(0, 0);
   m_scroll.SetScrollPos(0);

   LayerArr = (LayerArrStruct *)calloc(count, sizeof(LayerArrStruct));

   if (LayerArr == NULL)
   {
      ErrorMessage("Memory allocation failure", "LayerPenMap::OnInitDialog");
      return TRUE;
   }

   int i = 0;
   for (j=0; j< doc->getMaxLayerIndex(); j++)
   {
      if (doc->getLayerArray()[j] == NULL)  continue;
      if (!doc->getLayerArray()[j]->isVisible())   continue;

      LayerArr[i].layer = doc->getLayerArray()[j];
      LayerArr[i].pen = (i%8)+1;
      i++;
   }
   WriteData(0);
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerPenMap::WriteData(int pos) 
{
   if (count < 1) { UpdateData(FALSE); return; }
   m_layer1.Format("%2d. %s", pos+1, LayerArr[pos].layer->getName());
   m_pen1.SetCurSel(LayerArr[pos].pen-1);

   if (count < 2) { UpdateData(FALSE); return; }
   m_layer2.Format("%2d. %s", pos+2, LayerArr[pos+1].layer->getName());
   m_pen2.SetCurSel(LayerArr[pos+1].pen-1);

   if (count < 3) { UpdateData(FALSE); return; }
   m_layer3.Format("%2d. %s", pos+3, LayerArr[pos+2].layer->getName());
   m_pen3.SetCurSel(LayerArr[pos+2].pen-1);

   if (count < 4) { UpdateData(FALSE); return; }
   m_layer4.Format("%2d. %s", pos+4, LayerArr[pos+3].layer->getName());
   m_pen4.SetCurSel(LayerArr[pos+3].pen-1);

   if (count < 5) { UpdateData(FALSE); return; }
   m_layer5.Format("%2d. %s", pos+5, LayerArr[pos+4].layer->getName());
   m_pen5.SetCurSel(LayerArr[pos+4].pen-1);

   if (count < 6) { UpdateData(FALSE); return; }
   m_layer6.Format("%2d. %s", pos+6, LayerArr[pos+5].layer->getName());
   m_pen6.SetCurSel(LayerArr[pos+5].pen-1);

   if (count < 7) { UpdateData(FALSE); return; }
   m_layer7.Format("%2d. %s", pos+7, LayerArr[pos+6].layer->getName());
   m_pen7.SetCurSel(LayerArr[pos+6].pen-1);

   if (count < 8) { UpdateData(FALSE); return; }
   m_layer8.Format("%2d. %s", pos+8, LayerArr[pos+7].layer->getName());
   m_pen8.SetCurSel(LayerArr[pos+7].pen-1);

   UpdateData(FALSE);
}

void LayerPenMap::ReadData()
{
   int pos = m_scroll.GetScrollPos();

   UpdateData();

   if (count < 1) return; 
   LayerArr[pos].pen = m_pen1.GetCurSel()+1;

   if (count < 2) return; 
   LayerArr[pos+1].pen = m_pen2.GetCurSel()+1;

   if (count < 3) return;
   LayerArr[pos+2].pen = m_pen3.GetCurSel()+1;

   if (count < 4) return; 
   LayerArr[pos+3].pen = m_pen4.GetCurSel()+1;

   if (count < 5) return; 
   LayerArr[pos+4].pen = m_pen5.GetCurSel()+1;

   if (count < 6) return; 
   LayerArr[pos+5].pen = m_pen6.GetCurSel()+1;

   if (count < 7) return; 
   LayerArr[pos+6].pen = m_pen7.GetCurSel()+1;

   if (count < 8) return; 
   LayerArr[pos+7].pen = m_pen8.GetCurSel()+1;
}

void LayerPenMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   ReadData();

   int pos = m_scroll.GetScrollPos();

   switch (nSBCode)
   {
      case SB_TOP:
         pos = 0;
      break;
      case SB_BOTTOM:
         pos = count - 8;
      break;
      case SB_PAGEDOWN:
         pos += 8;
      break;
      case SB_PAGEUP:
         pos -= 8;
      break;
      case SB_LINEDOWN:
         pos++;
      break;
      case SB_LINEUP:
         pos--;
      break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
         pos = nPos;
      break;
   }
   m_scroll.SetScrollPos(pos);
   WriteData(m_scroll.GetScrollPos());
}

void LayerPenMap::OnOK() 
{
   ReadData();
   
   CDialog::OnOK();
}
