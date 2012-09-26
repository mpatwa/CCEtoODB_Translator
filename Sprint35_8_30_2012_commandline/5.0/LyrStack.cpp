// $Header: /CAMCAD/4.6/LyrStack.cpp 23    12/07/06 12:06p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "LyrStack.h"
#include "layerlst.h"
#include "pcbutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern LayerTypeInfoStruct layertypes[];
static int SortBy;
static BOOL Reverse;

static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
//void UpdateLayers(CCEtoODBDoc *doc);

/******************************************************************************
* OnLayerStack
*/
void CCEtoODBDoc::OnLayerStack() 
{
   LayerStack dlg;
   dlg.doc = this;
   dlg.DoModal(); 
   UpdateAllViews(NULL);
   //UpdateLayers(this);
}

/////////////////////////////////////////////////////////////////////////////
// LayerStack dialog
LayerStack::LayerStack(CWnd* pParent /*=NULL*/)
   : CDialog(LayerStack::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerStack)
   m_show = 2;
   m_layersFrom = 3;
   //}}AFX_DATA_INIT
}

void LayerStack::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerStack)
   DDX_Control(pDX, IDC_LIST1, m_list);
   DDX_Radio(pDX, IDC_SHOW, m_show);
   DDX_Radio(pDX, IDC_PADSTACKS, m_layersFrom);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LayerStack, CDialog)
   //{{AFX_MSG_MAP(LayerStack)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnClick)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList)
   ON_BN_CLICKED(IDC_SHOW, OnLayertype)
   ON_BN_CLICKED(IDC_PADSTACKS, OnPadstacks)
   ON_BN_CLICKED(IDC_COMPONENTS, OnComponents)
   ON_BN_CLICKED(IDC_ALL_LAYERS, OnAllLayers)
   ON_BN_CLICKED(IDC_BOARDS, OnBoards)
   ON_BN_CLICKED(IDC_RADIO2, OnLayertype)
   ON_BN_CLICKED(IDC_RADIO3, OnLayertype)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerStack message handlers
BOOL LayerStack::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   Reverse = FALSE;
   SortBy = -1;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 10;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width * 3;
   column.pszText = "Name";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width;
   column.pszText = "Artwork";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Electrical";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.pszText = "Physical";
   column.iSubItem = 3;
   m_list.InsertColumn(3, &column);

   column.cx = width * 3;
   column.pszText = "Type";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);

   column.cx = width;
   column.pszText = "Z Height";
   column.iSubItem = 5;
   m_list.InsertColumn(5, &column);


   OnAllLayers();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerStack::FillListCtrl()
{
   UpdateData();

   LV_ITEM item;
   int actualItem;
   CString buf;
   LayerStruct *layer;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   m_list.DeleteAllItems();

   // items
   int j = 0;
   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {  
      layer = doc->getLayerArray()[i];

      if (layer == NULL)   continue;

      if (!layer->isSubset())
         continue;

      // floating layer has nothing to do in layerStack list.
      if (layer->isFloating()) continue;

      switch (m_show)
      {
         case 0: // electrical
            if (!is_electricallayer(layer->getLayerType()))
               continue;
         break;
         case 1: // documentation
            if (is_electricallayer(layer->getLayerType()))
               continue;
         break;
         case 2: // all
         break;
      }

      CString layerName(layer->getName());

      item.mask = LVIF_TEXT;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = layerName.GetBuffer(1);
      actualItem = m_list.InsertItem(&item);

//    if (actualItem == -1) ErrorMessage("Insert Failed");
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_list.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_list.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_list.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_list.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_list.InsertItem(&item);
               }
            }
         }
      }

      //layer->getNameRef().ReleaseBuffer();

      m_list.SetItemData(actualItem, (LPARAM)layer);

      item.iItem = actualItem;
      item.iSubItem = 1;
      buf.Format("%d", layer->getArtworkStackNumber());
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      buf.Format("%d", layer->getElectricalStackNumber());
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 3;
      buf.Format("%d", layer->getPhysicalStackNumber());
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 4;
      buf.Format("%s", layerTypeToString(layer->getLayerType()));
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);

      item.iSubItem = 5;
      buf.Format("%.*lf", decimals, layer->getZHeight());
      item.pszText = (char*)(LPCTSTR)buf;
      m_list.SetItem(&item);
   }
   
   m_list.SortItems(CompareFunc, 0);
}

void LayerStack::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   if (SortBy == pNMListView->iSubItem)
      Reverse = !Reverse;
   else 
      Reverse = FALSE;
   SortBy = pNMListView->iSubItem;

   m_list.SortItems(CompareFunc, 0);
   
   *pResult = 0;
}

void LayerStack::OnEdit() 
{
   // get selected geometry
   int count = m_list.GetItemCount();

   if (!count)
      return;

   BOOL Selected = FALSE;

	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
      {
         Selected = TRUE;
         break;
      }
   }
   if (!Selected)
      return;

   LayerStruct *layer = (LayerStruct*)m_list.GetItemData(selItem);

   EditLayer dlg;
   dlg.doc = doc;
   dlg.m_layername   = layer->getName();
   dlg.m_visible     = layer->isVisible();
   dlg.m_editable    = layer->isEditable();
   dlg.m_colorButton.tempColor = layer->getColor();
   dlg.m_floating    = layer->isFloating();
   dlg.m_negative    = layer->isNegative();
   dlg.layertype     = layer->getLayerType();
   dlg.m_artwork     = layer->getArtworkStackNumber();
   dlg.m_electrical  = layer->getElectricalStackNumber();
   dlg.m_physical    = layer->getPhysicalStackNumber();
   dlg.m_zheight.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), layer->getZHeight());

   if (layer->getNeverMirror())
      dlg.m_mirror = 2;
   else if (layer->getMirrorOnly())
      dlg.m_mirror = 1;
   else
      dlg.m_mirror = 0;

   if (dlg.DoModal() == IDOK)
   {
      layer->setVisible(dlg.m_visible);
      layer->setEditable( dlg.m_editable);
      layer->setColor( dlg.m_colorButton.tempColor);
      layer->setFloating( dlg.m_floating);
      layer->setNegative(dlg.m_negative);
      layer->setLayerType(dlg.layertype);
      layer->setArtworkStackNumber( dlg.m_artwork);
      layer->setElectricalStackNumber(dlg.m_electrical);
      layer->setPhysicalStackNumber( dlg.m_physical);
      layer->setZHeight( atof(dlg.m_zheight));

      // set layer flag without messing up other flags
      layer->clearFlagBits(LY_NEVERMIRROR | LY_MIRRORONLY);

      if (dlg.m_mirror == 1)
         layer->setFlagBits(LY_MIRRORONLY);
      else if (dlg.m_mirror == 2)
         layer->setFlagBits(LY_NEVERMIRROR);

      FillListCtrl();
   }
}

// return negative if 1 comes before 2
static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   int swap = (Reverse?-1:1);

   switch (SortBy)
   {
   case 1: // Art
      return swap * (((LayerStruct *)lParam1)->getArtworkStackNumber() - ((LayerStruct *)lParam2)->getArtworkStackNumber());
   case 2: // Electrical
      return swap * (((LayerStruct *)lParam1)->getElectricalStackNumber() - ((LayerStruct *)lParam2)->getElectricalStackNumber());
   case 3: // Physical
      return swap * (((LayerStruct *)lParam1)->getPhysicalStackNumber() - ((LayerStruct *)lParam2)->getPhysicalStackNumber());
   case 4: // Type
      return swap * (((LayerStruct *)lParam1)->getLayerType() - ((LayerStruct *)lParam2)->getLayerType());
   case 5: // ZHeight
      return swap * ((((LayerStruct *)lParam1)->getZHeight() - ((LayerStruct *)lParam2)->getZHeight()) < -SMALLNUMBER ? -1 : 1);
   case 0: // name
   default:
      return swap * STRICMP(((LayerStruct *)lParam1)->getName(), ((LayerStruct *)lParam2)->getName());
   }
}


void LayerStack::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   
   *pResult = 0;
}

void LayerStack::OnPadstacks() 
{
	int i=0;
   for (i=0; i < doc->getMaxLayerIndex(); i++)
   {
      if (doc->getLayerArray()[i] == NULL)  continue;
      doc->getLayerArray()[i]->setSubset(false);
   }
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
         LoopBlock(block, TRUE, -1);
   }     

   FillListCtrl();
}

void LayerStack::OnComponents() 
{
	int i=0;
   for (i=0; i < doc->getMaxLayerIndex(); i++)
   {
      if (doc->getLayerArray()[i] == NULL)  continue;
      doc->getLayerArray()[i]->setSubset(false);
   }
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
         LoopBlock(block, FALSE, -1);
   }     

   FillListCtrl();
}

void LayerStack::OnBoards() 
{
	int i=0;
   for (i=0; i < doc->getMaxLayerIndex(); i++)
   {
      if (doc->getLayerArray()[i] == NULL)  continue;
      doc->getLayerArray()[i]->setSubset(false);
   }
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getBlockType() == BLOCKTYPE_PCB)
         LoopBlock(block, FALSE, -1);
   }     

   FillListCtrl();
}

void LayerStack::OnAllLayers() 
{
   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      if (doc->getLayerArray()[i] == NULL)  continue;
      doc->getLayerArray()[i]->setSubset(true);
   }
   FillListCtrl();
}

void LayerStack::OnLayertype() 
{
   FillListCtrl();
}

void LayerStack::LoopBlock(BlockStruct *block, BOOL AllInsertTypes, int insertlayer)
{
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      int layer;
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertlayer != -1)
         layer = insertlayer;
      else
         layer = data->getLayerIndex();

      if (data->getDataType() == T_INSERT)
      {
         BlockStruct *subblock = doc->getBlockAt(data->getInsert()->getBlockNumber());

         if (subblock != NULL)
         {
            if (subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE || subblock->getFlags() & BL_TOOL || subblock->getFlags() & BL_BLOCK_TOOL)
            {
               if (layer > -1)   doc->getLayerArray()[layer]->setSubset(true);
               if (subblock->getShape() == T_COMPLEX)
               {
                  subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));
                  LoopBlock(subblock, AllInsertTypes, layer);
               }
            }
            else if (AllInsertTypes || data->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)
            {
               LoopBlock(subblock, AllInsertTypes, layer);
            }
         }
      }
      else
      {
         if (layer > -1)   doc->getLayerArray()[layer]->setSubset(true);
      }
   }
}

void LayerStack::OnApply() 
{
   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {  
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      layer->setVisible(false);

      if (!layer->isSubset())
         continue;

      switch (m_show)
      {
         case 0: // electrical
            if (!is_electricallayer(layer->getLayerType()))
               continue;
         break;
         case 1: // documentation
            if (is_electricallayer(layer->getLayerType()))
               continue;
         break;
         case 2: // all
         break;
      }

      layer->setVisible(true);
   }

   doc->UpdateAllViews(NULL);
}

