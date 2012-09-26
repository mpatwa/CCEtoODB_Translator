// $Header: /CAMCAD/4.5/LayerLst.cpp 40    12/18/06 3:46p Rick Faltersack $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "dbutil.h"
#include "layerlst.h"
#include "attrib.h"
#include "lyrmanip.h"
#include "dft.h"
#include "PersistantColorDialog.h"
#include "PaletteColorDialog.h"
#include "ViewSynchronizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern LayerTypeInfoStruct layertypes[];

//void FillCurrentSettings(CCEtoODBDoc *doc);
//void UpdateLayers(CCEtoODBDoc *doc);
int CALLBACK LLCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam);

#define QName                   "Name"
#define QNumber                 "Number"
#define QArtworkStackNumber     "Artwork Stack Number"
#define QElectricalStackNumber  "Electrical Stack Number"
#define QPhysicalStackNumber    "Physical Stack Number"
#define QType                   "Type"
#define QMirror                 "Mirror"
#define QComment                "Comment"
#define QAscending              "Ascending"
#define QDescending             "Descending"

//_____________________________________________________________________________
CString layerListColumnTagToString(LayerListColumnTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case LayerListColumnName:                   retval = QName;                   break;
   case LayerListColumnNumber:                 retval = QNumber;                 break;
   case LayerListColumnArtworkStackNumber:     retval = QArtworkStackNumber;     break;
   case LayerListColumnElectricalStackNumber:  retval = QElectricalStackNumber;  break;
   case LayerListColumnPhysicalStackNumber:    retval = QPhysicalStackNumber;    break;
   case LayerListColumnType:                   retval = QType;                   break;
   case LayerListColumnMirroredLayer:          retval = QMirror;                 break;
   case LayerListColumnComment:                retval = QComment;                break;
   }

   return retval;
}

LayerListColumnTag intToLayerListColumnTag(int columnValue)
{
   LayerListColumnTag retval = LayerListColumnUndefined;

   switch (columnValue)
   {
   case LayerListColumnName:                   retval = LayerListColumnName;                   break;
   case LayerListColumnNumber:                 retval = LayerListColumnNumber;                 break;
   case LayerListColumnArtworkStackNumber:     retval = LayerListColumnArtworkStackNumber;     break;
   case LayerListColumnElectricalStackNumber:  retval = LayerListColumnElectricalStackNumber;  break;
   case LayerListColumnPhysicalStackNumber:    retval = LayerListColumnPhysicalStackNumber;    break;
   case LayerListColumnType:                   retval = LayerListColumnType;                   break;
   case LayerListColumnMirroredLayer:          retval = LayerListColumnMirroredLayer;          break;
   case LayerListColumnComment:                retval = LayerListColumnComment;                break;
   }

   return retval;
}

LayerListColumnTag stringToLayerListColumnTag(const CString& columnName)
{
   LayerListColumnTag retval = LayerListColumnUndefined;

   if (columnName.CompareNoCase(QName) == 0)
   {
      retval = LayerListColumnName;
   }
   else if (columnName.CompareNoCase(QNumber) == 0)
   {
      retval = LayerListColumnNumber;
   }
   else if (columnName.CompareNoCase(QArtworkStackNumber) == 0)
   {
      retval = LayerListColumnArtworkStackNumber;
   }
   else if (columnName.CompareNoCase(QElectricalStackNumber) == 0)
   {
      retval = LayerListColumnElectricalStackNumber;
   }
   else if (columnName.CompareNoCase(QPhysicalStackNumber) == 0)
   {
      retval = LayerListColumnPhysicalStackNumber;
   }
   else if (columnName.CompareNoCase(QType) == 0)
   {
      retval = LayerListColumnType;
   }
   else if (columnName.CompareNoCase(QMirror) == 0)
   {
      retval = LayerListColumnMirroredLayer;
   }
   else if (columnName.CompareNoCase(QComment) == 0)
   {
      retval = LayerListColumnComment;
   }

   return retval;
}

/******************************************************************************
* OnLayerList
*/
void CCEtoODBDoc::OnLayerList() 
{
   LayerListDlg dlg;
   dlg.doc = this;
   dlg.DoModal();

	dlg.ApplyExposedMetalChanges();
	OnRegen();
   UpdateAllViews(NULL);
   //UpdateLayers(this);

   getApp().getViewSynchronizer().synchronizeLayers(getActiveView());
}

/////////////////////////////////////////////////////////////////////////////
// LayerListDlg dialog
LayerListDlg::LayerListDlg(CWnd* pParent /*=NULL*/)
   : CResizingDialog(LayerListDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerListDlg)
   //}}AFX_DATA_INIT

	m_bExposedMetalTopOn = false;
	m_bExposedMetalBotOn = false;
	m_pExposedMetalTopLayer = NULL;
	m_pExposedMetalBotLayer = NULL;

   m_descendingSort = false;

   addFieldControl(IDC_LIST1   ,anchorLeft,growBoth);
   addFieldControl(IDC_ALL_ON  ,anchorBottom);
   addFieldControl(IDC_ALL_OFF ,anchorBottom);
   addFieldControl(IDC_ALL_SHOW,anchorBottom);
   addFieldControl(IDC_USED_ON ,anchorBottom);
   addFieldControl(IDC_EDIT    ,anchorBottom);
   addFieldControl(IDC_APPLY   ,anchorBottom);
   addFieldControl(IDOK        ,anchorBottom);
}

LayerListDlg::~LayerListDlg()
{
}

void LayerListDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerListDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LayerListDlg, CResizingDialog)
   //{{AFX_MSG_MAP(LayerListDlg)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
   ON_BN_CLICKED(IDC_USED_ON, OnUsedOn)
   ON_BN_CLICKED(IDC_ALL_OFF, OnAllOff)
   ON_BN_CLICKED(IDC_ALL_ON, OnAllOn)
   ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
   ON_BN_CLICKED(IDC_ALL_SHOW, OnAllShow)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList1)
   ON_COMMAND(ID_BRINGLAYERTOFRONT, OnBringLayerToFront)
   ON_COMMAND(ID_SENDLAYERTOBACK, OnSendLayerToBack)
   ON_COMMAND(ID_EDITLAYER, OnEdit)
   ON_COMMAND(IDC_ReassignUniqueColors, OnReassignUniqueColors)
   //}}AFX_MSG_MAP
   ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerListDlg message handlers
BOOL LayerListDlg::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   m_list.doc = doc;
   m_sortColumn = LayerListColumnUndefined;

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = rect.Width() / 14;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width * 3;
   column.pszText = "Name";
   column.iSubItem = 0;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width;
   column.pszText = "Num";
   column.iSubItem = 1;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width;
   column.pszText = "Artwork Stack Number";
   column.iSubItem = 2;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width;
   column.pszText = "Electrical Stack Number";
   column.iSubItem = 3;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width;
   column.pszText = "Physical Stack Number";
   column.iSubItem = 4;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 3;
   column.pszText = "Type";
   column.iSubItem = 5;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 2;
   column.pszText = "Mirror";
   column.iSubItem = 6;
   m_list.InsertColumn(column.iSubItem, &column);

   column.cx = width * 6;
   column.pszText = "Comment";
   column.iSubItem = 7;
   m_list.InsertColumn(column.iSubItem, &column);

   LV_ITEM item;
   CString buf;

   // items
   int j = 0;
   int actualItem;

   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {  
      LayerStruct *layer = doc->getLayer(i);

      if (layer == NULL)   continue;

		CString layerName = layer->getName();

		if (layerName == DFT_LAYER_EXPOSE_METAL_TOP)
		{
			m_bExposedMetalTopOn = layer->getVisible();
			m_pExposedMetalTopLayer = layer;
		}

		if (layerName == DFT_LAYER_EXPOSE_METAL_BOT)
		{
			m_bExposedMetalBotOn = layer->getVisible();
			m_pExposedMetalBotLayer = layer;
		}

      item.mask = LVIF_TEXT;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = layerName.GetBuffer(0);
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

      m_list.SetItemData(actualItem, (LPARAM)layer);
   }

   restoreColumnWidths();
   restoreSortOrder();

   if (m_sortColumn != LayerListColumnUndefined)
   {
      m_list.SortItems(LLCompareFunc,(LPARAM)this);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerListDlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NMITEMACTIVATE *nm = (NMITEMACTIVATE*)pNMHDR;

   *pResult = 0;
   
   if (nm->iItem == -1)
      return;
   
   OnBox = nm->ptAction.x < 20;

   if (OnBox)
   {
      LayerStruct *layer = (LayerStruct*)m_list.GetItemData(nm->iItem);
      layer->setVisible( !layer->isVisible());
      m_list.Invalidate();

      if ((nm->uKeyFlags & LVKF_CONTROL) != 0)
      {
         OnApply();
      }
   }     
}

void LayerListDlg::OnEdit() 
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
   dlg.layer = layer->getLayerIndex();
   dlg.m_comment     = layer->getComment();
   dlg.mirror        = layer->getMirroredLayerIndex();
   dlg.m_layername   = layer->getName();
   dlg.m_visible     = layer->isVisible();
   dlg.m_editable    = layer->isEditable();
   dlg.m_worldView   = layer->isWorldView();
   dlg.m_colorButton.tempColor = layer->getColor();
   dlg.m_floating    = layer->isFloating();
   dlg.m_negative    = layer->isNegative();
   dlg.layertype     = layer->getLayerType();
   dlg.m_artwork     = layer->getArtworkStackNumber();
   dlg.m_electrical  = layer->getElectricalStackNumber();
   dlg.m_physical    = layer->getPhysicalStackNumber();
   dlg.m_zheight.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), layer->getZHeight());

   if (layer->getMirroredLayerIndex() != layer->getLayerIndex())
   {
      LayerStruct *mirlp = doc->getLayer(layer->getMirroredLayerIndex());
      if (mirlp != NULL)
         dlg.m_mirrorLayer = mirlp->getName();
   }
   if (layer->getNeverMirror())
      dlg.m_mirror = 2;
   else if (layer->getMirrorOnly())
      dlg.m_mirror = 1;
   else
      dlg.m_mirror = 0;

   if (dlg.DoModal() == IDOK)
   {
      layer->setName( dlg.m_layername);
      layer->setComment( dlg.m_comment);
      layer->setVisible( dlg.m_visible);
      layer->setEditable( dlg.m_editable);
      layer->setWorldView( dlg.m_worldView);
      layer->setColor( dlg.m_colorButton.tempColor);
      layer->setFloating( dlg.m_floating);
      layer->setNegative(dlg.m_negative);
      layer->setLayerType(dlg.layertype);
      layer->setArtworkStackNumber( dlg.m_artwork);
      layer->setElectricalStackNumber(dlg.m_electrical);
      layer->setPhysicalStackNumber( dlg.m_physical);
      layer->setZHeight( atof(dlg.m_zheight));
      layer->setMirroredLayerIndex(dlg.mirror);

      // set layer flag without messing up other flags
      layer->clearFlagBits(LY_NEVERMIRROR | LY_MIRRORONLY);

      if (dlg.m_mirror == 1)
         layer->setFlagBits(LY_MIRRORONLY);
      else if (dlg.m_mirror == 2)
         layer->setFlagBits(LY_NEVERMIRROR);

      m_list.Invalidate();
   }
}

void LayerListDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (OnBox)
      return;

   OnEdit();
   
   *pResult = 0;
}

void LayerListDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
   if (pWnd->m_hWnd != m_list.m_hWnd)
      return;

   CMenu menu;
   VERIFY(menu.LoadMenu(CG_IDR_POPUP_CAMCADVIEW));

   CMenu* pPopup = menu.GetSubMenu(2);
   ASSERT(pPopup != NULL);

   CWnd* pWndPopupOwner = this;
   while (pWndPopupOwner->GetStyle() & WS_CHILD)
      pWndPopupOwner = pWndPopupOwner->GetParent();

   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
      pWndPopupOwner);
}

void LayerListDlg::OnApply() 
{
	ApplyExposedMetalChanges();
	doc->OnRegen();
   doc->UpdateAllViews(NULL);

   getApp().getViewSynchronizer().synchronizeLayers(getActiveView());
}

void LayerListDlg::ApplyExposedMetalChanges()
{
	if (m_pExposedMetalTopLayer != NULL && m_pExposedMetalBotLayer != NULL && 
	      (m_bExposedMetalTopOn != m_pExposedMetalTopLayer->getVisible() || 
          m_bExposedMetalBotOn != m_pExposedMetalBotLayer->getVisible()    )   )
	{
		for (int i=0; i<doc->getMaxBlockIndex(); i++)
		{  
			BlockStruct *block= doc->getBlockAt(i);
			if (block == NULL)   continue;

			for (POSITION pos=block->getHeadDataPosition(); pos!=NULL; block->getNextData(pos))
			{
				DataStruct *data = block->getAtData(pos);
				if (data->getLayerIndex() < 0)
					continue;

				LayerStruct *layer = doc->getLayerAt(data->getLayerIndex());
				if (layer == m_pExposedMetalTopLayer || layer == m_pExposedMetalBotLayer)
					data->setHidden(!layer->getVisible());
			}
		}
	}
}

void LayerListDlg::saveWindowState()
{
   CResizingDialog::saveWindowState();

   CString dialogName(GetDialogProfileEntry());

   CHeaderCtrl* headerControl = m_list.GetHeaderCtrl();

   if (headerControl != NULL)
   {
      CString columnWidths;
      int columnCount = headerControl->GetItemCount();

      for (int columnIndex = 0;columnIndex < columnCount;columnIndex++)
      {
         int columnWidth = m_list.GetColumnWidth(columnIndex);

         columnWidths.AppendFormat("%s%d",(columnIndex > 0) ? "," : "",columnWidth);
      }

      AfxGetApp()->WriteProfileString(dialogName,"ColumnWidths",columnWidths);
   }

   AfxGetApp()->WriteProfileString(dialogName,"SortBy",
      layerListColumnTagToString(m_sortColumn) + CString(" ") + (m_descendingSort ? QDescending : QAscending));
}

void LayerListDlg::restoreColumnWidths()
{
   CString dialogName(GetDialogProfileEntry());
   CSupString columnWidths = AfxGetApp()->GetProfileString(dialogName,"ColumnWidths","");

   if (!columnWidths.IsEmpty())
   {
      CStringArray param;
      int savedColumnCount = columnWidths.Parse(param);

      CHeaderCtrl* headerControl = m_list.GetHeaderCtrl();
      int columnCount = headerControl->GetItemCount();
      int columnWidth;

      for (int columnIndex = 0;columnIndex < columnCount;columnIndex++)
      {
         if (columnIndex < savedColumnCount)
         {
            columnWidth = atoi(param.GetAt(columnIndex));
         }
         else
         {
            columnIndex = 10;
         }

         m_list.SetColumnWidth(columnIndex,columnWidth);
      }
   }
}

void LayerListDlg::restoreSortOrder()
{
   CSupString sortDescriptor = AfxGetApp()->GetProfileString(GetDialogProfileEntry(),"SortBy","");

   if (!sortDescriptor.IsEmpty())
   {
      CStringArray param;
      int numParams = sortDescriptor.Parse(param);

      if (numParams > 0)
      {
         m_sortColumn = stringToLayerListColumnTag(param.GetAt(0));
      }

      m_descendingSort = false;

      if (numParams > 1)
      {
         CString ascendingString = param.GetAt(1);

         m_descendingSort = (ascendingString.CompareNoCase(QDescending) == 0);
      }

   }
}

/******************************************************************************
* OnAllOn
*/
void LayerListDlg::OnAllOn() 
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayer(i);

      if (layer != NULL)
      {
         layer->setVisible(true);
         layer->setEditable(true);
      }
   }

   m_list.Invalidate();
}

/******************************************************************************
* OnAllOff
*/
void LayerListDlg::OnAllOff() 
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayer(i);

      if (layer != NULL)
      {
         layer->setVisible(false);
      }
   }

   m_list.Invalidate();
}

/******************************************************************************
* OnAllShow
*/
void LayerListDlg::OnAllShow() 
{
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayer(i);

      if (layer != NULL)
      {
         layer->setVisible(true);
         layer->setEditable(false);
      }
   }

   m_list.Invalidate();
}

/******************************************************************************
* OnUsedOn
*/
void LayerListDlg::OnUsedOn() 
{
	int i=0;
   for (i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayer(i);

      if (layer != NULL)
      {
         layer->setVisible(false);
         layer->setEditable(false);
      }
   }
   
   // loop Blocklist, loop Datas, mark layers as used 
   // protect mirror layers.
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)   continue;

      for (POSITION datapos = block->getDataList().GetHeadPosition();datapos != NULL;)
      {
         DataStruct *data = block->getDataList().GetNext(datapos);
         
         if (data->getAttributesRef())
         {
            for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
            {
               Attrib* attrib;
               WORD keyword;

               data->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

               if (attrib->isVisible())
               {
                  LayerStruct *layer = doc->getLayer(attrib->getLayerIndex());
                  if (layer != NULL)
                  {
                     layer->setVisible(true);
                     layer->setEditable(true);

                     if (layer->getMirroredLayerIndex()) // protect mirror layers
                     {
                        layer = doc->getLayer(layer->getMirroredLayerIndex());
                        if (layer != NULL)
                        {
                           layer->setVisible(true);
                           layer->setEditable(true);
                        }
                     }
                  }
               }
            }
         }

         if (data->getLayerIndex() < 0)
            continue;   // insert layer can be -1.
         
         LayerStruct *layer = doc->getLayer(data->getLayerIndex());
         if (layer != NULL)
         {
            layer->setVisible(true);
            layer->setEditable(true);

            if (layer->getMirroredLayerIndex()) // protect mirror layers
            {
               layer = doc->getLayer(layer->getMirroredLayerIndex());
               if (layer != NULL)
               {
                  layer->setVisible(true);
                  layer->setEditable(true);
               }
            }
         }
      }
   }

   m_list.Invalidate();
}

int CALLBACK LLCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam)
{
   LayerListDlg *dlg = (LayerListDlg*)lParam;

   int result;

   LayerStruct *layer1 = (LayerStruct*)lParam1, *layer2 = (LayerStruct*)lParam2;

   switch (dlg->m_sortColumn)
   {
   case LayerListColumnNumber: // num
      result = layer1->getLayerIndex() - layer2->getLayerIndex();
      break;
   case LayerListColumnArtworkStackNumber: // artwork
      result = layer1->getArtworkStackNumber() - layer2->getArtworkStackNumber();
      break;
   case LayerListColumnElectricalStackNumber: // electrical
      result = layer1->getElectricalStackNumber() - layer2->getElectricalStackNumber();
      break;
   case LayerListColumnPhysicalStackNumber: // physical
      result = layer1->getPhysicalStackNumber() - layer2->getPhysicalStackNumber();
      break;
   case LayerListColumnType: // layertype
      result = strcmp(layerTypeToString(layer1->getLayerType()), layerTypeToString(layer2->getLayerType()));
      break;
   case LayerListColumnMirroredLayer: // mirror
      {
         CString name1, name2;
         
         if (layer1->getMirroredLayerIndex() != layer1->getLayerIndex())
         {
            LayerStruct *mirlp = dlg->doc->getLayer(layer1->getMirroredLayerIndex());
            if (mirlp != NULL)
               name1 = mirlp->getName();
         }
         if (layer2->getMirroredLayerIndex() != layer2->getLayerIndex())
         {
            LayerStruct *mirlp = dlg->doc->getLayer(layer2->getMirroredLayerIndex());
            if (mirlp != NULL)
               name2 = mirlp->getName();
         }

         result = name1.CompareNoCase(name2);
      }
      break;
   case LayerListColumnComment: // comment
      result = layer1->getComment().CompareNoCase(layer2->getComment());
      break;
   case LayerListColumnName: // name
   default:
      result = layer1->getName().CompareNoCase(layer2->getName());
      break;
   }

   if (dlg->m_descendingSort)
      return -result;
   else
      return result;
}

void LayerListDlg::OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   if (m_sortColumn == pNMListView->iSubItem)
   {
      m_descendingSort = !m_descendingSort;
   }
   else
   {
      m_sortColumn = intToLayerListColumnTag(pNMListView->iSubItem);
      m_descendingSort = false;
   }

   m_list.SortItems(LLCompareFunc, (LPARAM)this);

   *pResult = 0;
}

void LayerListDlg::OnBringLayerToFront()
{
   // get selected layer
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

   BringLayerToFront(doc, layer->getLayerIndex());

   doc->UpdateAllViews(NULL);
}

void LayerListDlg::OnSendLayerToBack()
{
   // get selected layer
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

   SendLayerToBack(doc, layer->getLayerIndex());

   doc->UpdateAllViews(NULL);
}

void LayerListDlg::OnReassignUniqueColors()
{
   int visibleCount = 0;
   int layerCount = m_list.GetItemCount();

   for (int itemIndex=0;itemIndex < layerCount;itemIndex++)
   {
      LayerStruct* layer = (LayerStruct*)m_list.GetItemData(itemIndex);

      if (layer != NULL && layer->isVisible())
      {
         visibleCount++;
      }
   }

   if (visibleCount > 0)
   {
      double deltaHue = 1. / visibleCount;

      CColor color0(0.,1.,.5,3.,colorModelNsl);
      CColor color1(1. - deltaHue,1.,.5,3.,colorModelNsl);     

      double metric = 0.;

      for (int itemIndex=0;itemIndex < layerCount;itemIndex++)
      {
         LayerStruct* layer = (LayerStruct*)m_list.GetItemData(itemIndex);

         if (layer != NULL && layer->isVisible())
         {
            CColor color = color0.interpolate(color1,metric,colorModelNsl);

            layer->setColor(color.getColor());

            metric += deltaHue;
         }
      }

      m_list.Invalidate();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CLayerColorButton
CLayerColorButton::CLayerColorButton()
{
}

CLayerColorButton::~CLayerColorButton()
{
}


BEGIN_MESSAGE_MAP(CLayerColorButton, CButton)
   //{{AFX_MSG_MAP(CLayerColorButton)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerColorButton message handlers
void CLayerColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   HBRUSH hBrush, orgBrush;
   unsigned long color;

   if (lpDrawItemStruct->itemAction != ODA_DRAWENTIRE && lpDrawItemStruct->itemAction != ODA_SELECT)
      return;

   color = tempColor;

   if (lpDrawItemStruct->itemAction == ODA_DRAWENTIRE) // draw button color
   {
      hBrush = CreateSolidBrush(color);
      orgBrush = (HBRUSH) SelectObject(lpDrawItemStruct->hDC, hBrush);
      Rectangle(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, 
            lpDrawItemStruct->rcItem.top, lpDrawItemStruct->rcItem.right, 
            lpDrawItemStruct->rcItem.bottom);
      DeleteObject(SelectObject(lpDrawItemStruct->hDC, orgBrush));
   }
   else if (lpDrawItemStruct->itemAction == ODA_SELECT) // button clicked
   {
      if (lpDrawItemStruct->itemState & ODS_SELECTED) // button state changes twice(down then up) - only answer once
      {
         CPersistantColorDialog dialog(color);
         dialog.DoModal();
         color = dialog.GetColor();
         Invalidate();
         tempColor = color;
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
// MirrorWith dialog
MirrorWith::MirrorWith(CWnd* pParent /*=NULL*/)
   : CDialog(MirrorWith::IDD, pParent)
{
   //{{AFX_DATA_INIT(MirrorWith)
   //}}AFX_DATA_INIT
}

void MirrorWith::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MirrorWith)
   DDX_Control(pDX, IDC_LIST1, m_listbox);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MirrorWith, CDialog)
   //{{AFX_MSG_MAP(MirrorWith)
   ON_LBN_DBLCLK(IDC_LIST1, OnOK)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MirrorWith message handlers
void MirrorWith::OnOK() 
{
   int sel = m_listbox.GetCurSel();
   if (sel == LB_ERR)
      return;

   // clear mirror of layer that was mirrored to this by setting mirror to self
   LayerStruct *lp = m_doc->getLayer(m_mirror);
   if (lp != NULL)
      lp->setMirroredLayerIndex(m_mirror);

   m_mirror = m_listbox.GetItemData(sel);

   // clear mirror of layer that new mirror was mirrored to
   lp = m_doc->getLayer(m_mirror);
   if (lp != NULL)
   {
      // Clear by setting mirror to self
      int newMirrorWasMirroredTo = lp->getMirroredLayerIndex();
      LayerStruct *newMirWasMirToLp = m_doc->getLayer(newMirrorWasMirroredTo);
      if (newMirWasMirToLp != NULL)
         newMirWasMirToLp->setMirroredLayerIndex(newMirrorWasMirroredTo);
   }

   // Set up new mirror combo
   lp = m_doc->getLayer(m_layer);
   if (lp != NULL)
      lp->setMirroredLayerIndex(m_mirror);

   lp = m_doc->getLayer(m_mirror);
   if (lp != NULL)
      lp->setMirroredLayerIndex(m_layer);
   
   CDialog::OnOK();
}

int MirrorWith::AscendingNameSortFunc(const void *a, const void *b)
{
   LayerStruct *AA = (LayerStruct*)(((SElement*) a )->pObject->m_object);
   LayerStruct *BB = (LayerStruct*)(((SElement*) b )->pObject->m_object);

   return (AA->getName().CompareNoCase(BB->getName()));
}

BOOL MirrorWith::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   CMapSortedStringToOb<LayerStruct> Layers(10, false); // don't make it a container, something else owns the LayerStructs

   for (int i = 0; i < m_doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer;
      if ((layer = m_doc->getLayer(i)) != NULL)
      {
         Layers.SetAt(layer->getName(), layer);
      }
   }

   Layers.setSortFunction(&MirrorWith::AscendingNameSortFunc);
   Layers.Sort();

   CString *layerName;
   LayerStruct *layer = NULL;
   for (Layers.GetFirstSorted(layerName, layer); layer != NULL; Layers.GetNextSorted(layerName, layer)) 
	{
      m_listbox.SetItemData(m_listbox.AddString(layer->getName()), layer->getLayerIndex());
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// EditLayer dialog
EditLayer::EditLayer(CWnd* pParent /*=NULL*/)
   : CDialog(EditLayer::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditLayer)
   m_floating = FALSE;
   m_negative = FALSE;
   m_artwork = 0;
   m_electrical = 0;
   m_physical = 0;
   m_mirror = -1;
   m_zheight = _T("");
   m_layername = _T("");
   m_worldView = FALSE;
   m_mirrorLayer = _T("");
   m_visible = FALSE;
   m_editable = FALSE;
   m_comment = _T("");
   //}}AFX_DATA_INIT
}

void EditLayer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditLayer)
   DDX_Control(pDX, IDC_COLOR, m_colorButton);
   DDX_Control(pDX, IDC_LAYERTYPE_CB, m_layerTypeCB);
   DDX_Check(pDX, IDC_FLOATING, m_floating);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_Text(pDX, IDC_ARTWORK_SN, m_artwork);
   DDX_Text(pDX, IDC_ELECTRICAL_SN, m_electrical);
   DDX_Text(pDX, IDC_PHYSICAL_SN, m_physical);
   DDX_Radio(pDX, IDC_MIRROR, m_mirror);
   DDX_Text(pDX, IDC_ZHEIGHT, m_zheight);
   DDX_Text(pDX, IDC_LAYERNAME, m_layername);
   DDX_Check(pDX, IDC_WORLDVIEW, m_worldView);
   DDX_Text(pDX, IDC_MIRROR_LAYER2, m_mirrorLayer);
   DDX_Check(pDX, IDC_VISIBLE, m_visible);
   DDX_Check(pDX, IDC_EDITABLE, m_editable);
   DDX_Text(pDX, IDC_COMMENT, m_comment);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditLayer, CDialog)
   //{{AFX_MSG_MAP(EditLayer)
   ON_BN_CLICKED(IDC_CHANGE_MIRROR_LAYER, OnChangeMirrorLayer)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditLayer::OnInitDialog() 
{
   CDialog::OnInitDialog();
         
	int i=0;
   for (i=0; i<MAX_LAYTYPE; i++)
      m_layerTypeCB.SetItemData(m_layerTypeCB.AddString(layerTypeToString(i)), i);

   for (i=0; i<MAX_LAYTYPE; i++)
   {
      int lt = m_layerTypeCB.GetItemData(i);
      if (lt == layertype)
      {
         m_layerTypeCB.SetCurSel(i);
         break;
      }
   }

	if (!m_layername.CompareNoCase(DFT_LAYER_EXPOSE_METAL_TOP) || !m_layername.CompareNoCase(DFT_LAYER_EXPOSE_METAL_BOT))
		((CEdit*)GetDlgItem(IDC_LAYERNAME))->SetReadOnly();

	return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditLayer::OnChangeMirrorLayer() 
{
   MirrorWith mirrorDlg;
   mirrorDlg.m_doc = doc; 
   mirrorDlg.m_layer = layer;
   mirrorDlg.m_mirror = mirror;

   if (mirrorDlg.DoModal() == IDOK)
   {
      mirror = mirrorDlg.m_mirror;
      LayerStruct *lp = doc->getLayer(mirror);
      m_mirrorLayer = lp != NULL ? lp->getName() : "";
      UpdateData(FALSE);
   }
}

void EditLayer::OnOK() 
{
   layertype = (short)m_layerTypeCB.GetItemData(m_layerTypeCB.GetCurSel());
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// LayerListCtrl
LayerListCtrl::LayerListCtrl()
{
}

LayerListCtrl::~LayerListCtrl()
{
}


BEGIN_MESSAGE_MAP(LayerListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(LayerListCtrl)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerListCtrl message handlers
void LayerListCtrl::DrawItem(LPDRAWITEMSTRUCT di) 
{
   LayerStruct *layer = (LayerStruct*)di->itemData;
   
   ::SetTextColor(di->hDC, RGB(0, 0, 0));

   CBrush brush;
   CPen pen;

   HBRUSH orgBrush = (HBRUSH)SelectObject(di->hDC, brush);
   HPEN orgPen = (HPEN)SelectObject(di->hDC, GetStockObject(WHITE_PEN));

   int hscroll = GetScrollPos(SB_HORZ);

   CString buf;
   int offset, namePos, numPos, artworkPos, electricalPos, physicalPos, typePos, mirrorPos, commentPos;
   offset = 5;
   namePos = 19 - hscroll;
   numPos = GetColumnWidth(0) - hscroll;
   artworkPos = numPos + GetColumnWidth(1);
   electricalPos = artworkPos + GetColumnWidth(2);
   physicalPos = electricalPos + GetColumnWidth(3);
   typePos = physicalPos + GetColumnWidth(4);
   mirrorPos = typePos + GetColumnWidth(5);
   commentPos = mirrorPos + GetColumnWidth(6);

   if (di->itemState & ODS_SELECTED)
   {
      SelectObject(di->hDC, GetStockObject(LTGRAY_BRUSH));
      CPen pen;
      pen.CreatePen(PS_SOLID, 0, RGB(180,180,180));
      SelectObject(di->hDC, pen);
      Rectangle(di->hDC, namePos + 1, di->rcItem.top, GetColumnWidth(0), di->rcItem.bottom);
   }
   else
   {
      SelectObject(di->hDC, GetStockObject(WHITE_BRUSH));
      Rectangle(di->hDC, namePos, di->rcItem.top, GetColumnWidth(0), di->rcItem.bottom);
   }

   if (layer->isVisible() && layer->getColor() == RGB(255, 255, 255))
   {
      SelectObject(di->hDC, GetStockObject(BLACK_PEN));
      SelectObject(di->hDC, GetStockObject(BLACK_BRUSH));
      Rectangle(di->hDC, di->rcItem.left + 2 - hscroll, di->rcItem.top + 2, 18 - hscroll, di->rcItem.bottom - 1);
   }

   if (layer->isVisible())
   {
      if (layer->isEditable())
         brush.CreateSolidBrush(layer->getColor());
      else
         brush.CreateHatchBrush(HS_DIAGCROSS, layer->getColor());
   }
   else
      brush.CreateSolidBrush(RGB(255, 255, 255));

   if (layer->isVisible() && layer->getColor() == RGB(255, 255, 255))
      pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
   else
      pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
   SelectObject(di->hDC, pen);
   SelectObject(di->hDC, brush);
   Rectangle(di->hDC, di->rcItem.left + 2 - hscroll, di->rcItem.top + 2, 18 - hscroll, di->rcItem.bottom - 1);
   TextOut(di->hDC, namePos + offset, di->rcItem.top, layer->getName(), layer->getName().GetLength());

   SelectObject(di->hDC, GetStockObject(WHITE_PEN));
   SelectObject(di->hDC, GetStockObject(WHITE_BRUSH));

   Rectangle(di->hDC, numPos, di->rcItem.top, typePos, di->rcItem.bottom);
   buf.Format("%d", layer->getLayerIndex());
   TextOut(di->hDC, numPos + offset, di->rcItem.top, buf, buf.GetLength());

   Rectangle(di->hDC, artworkPos, di->rcItem.top, typePos, di->rcItem.bottom);
   buf.Format("%d", layer->getArtworkStackNumber());
   TextOut(di->hDC, artworkPos + offset, di->rcItem.top, buf, buf.GetLength());

   Rectangle(di->hDC, electricalPos, di->rcItem.top, typePos, di->rcItem.bottom);
   buf.Format("%d", layer->getElectricalStackNumber());
   TextOut(di->hDC, electricalPos + offset, di->rcItem.top, buf, buf.GetLength());

   Rectangle(di->hDC, physicalPos, di->rcItem.top, typePos, di->rcItem.bottom);
   buf.Format("%d", layer->getPhysicalStackNumber());
   TextOut(di->hDC, physicalPos + offset, di->rcItem.top, buf, buf.GetLength());

   Rectangle(di->hDC, typePos, di->rcItem.top, mirrorPos, di->rcItem.bottom);
   buf = layerTypeToString(layer->getLayerType());
   TextOut(di->hDC, typePos + offset, di->rcItem.top, buf, buf.GetLength());

   Rectangle(di->hDC, mirrorPos, di->rcItem.top, commentPos, di->rcItem.bottom);
   buf = "";
   if (layer->getMirroredLayerIndex() != layer->getLayerIndex())
   {
      LayerStruct *mirlp = doc->getLayer(layer->getMirroredLayerIndex());
      if (mirlp != NULL)
         buf = mirlp->getName();
   }
   TextOut(di->hDC, mirrorPos + offset, di->rcItem.top, buf, buf.GetLength());

   Rectangle(di->hDC, commentPos, di->rcItem.top, di->rcItem.right, di->rcItem.bottom);
   TextOut(di->hDC, commentPos + offset, di->rcItem.top, layer->getComment(), layer->getComment().GetLength());
}

/*
 * Owner draw control types
 
#define ODT_MENU        1
#define ODT_LISTBOX     2
#define ODT_COMBOBOX    3
#define ODT_BUTTON      4
#if(WINVER >= 0x0400)
#define ODT_STATIC      5
#endif /* WINVER >= 0x0400 */

/*
 * Owner draw actions
 
#define ODA_DRAWENTIRE  0x0001
#define ODA_SELECT      0x0002
#define ODA_FOCUS       0x0004

/*
 * Owner draw state

#define ODS_SELECTED    0x0001
#define ODS_GRAYED      0x0002
#define ODS_DISABLED    0x0004
#define ODS_CHECKED     0x0008
#define ODS_FOCUS       0x0010
#if(WINVER >= 0x0400)
#define ODS_DEFAULT         0x0020
#define ODS_COMBOBOXEDIT    0x1000
#endif /* WINVER >= 0x0400 
#if(WINVER >= 0x0500)
#define ODS_HOTLIGHT        0x0040
#define ODS_INACTIVE        0x0080
#endif /* WINVER >= 0x0500 

/*
 * MEASUREITEMSTRUCT for ownerdraw

typedef struct tagMEASUREITEMSTRUCT {
    UINT       CtlType;
    UINT       CtlID;
    UINT       itemID;
    UINT       itemWidth;
    UINT       itemHeight;
    DWORD      itemData;
} MEASUREITEMSTRUCT, NEAR *PMEASUREITEMSTRUCT, FAR *LPMEASUREITEMSTRUCT;


/*
 * DRAWITEMSTRUCT for ownerdraw

typedef struct tagDRAWITEMSTRUCT {
    UINT        CtlType;
    UINT        CtlID;
    UINT        itemID;
    UINT        itemAction;
    UINT        itemState;
    HWND        hwndItem;
    HDC         hDC;
    RECT        rcItem;
    DWORD       itemData;
} DRAWITEMSTRUCT, NEAR *PDRAWITEMSTRUCT, FAR *LPDRAWITEMSTRUCT;

/*
 * DELETEITEMSTRUCT for ownerdraw

typedef struct tagDELETEITEMSTRUCT {
    UINT       CtlType;
    UINT       CtlID;
    UINT       itemID;
    HWND       hwndItem;
    UINT       itemData;
} DELETEITEMSTRUCT, NEAR *PDELETEITEMSTRUCT, FAR *LPDELETEITEMSTRUCT;

/*
 * COMPAREITEMSTUCT for ownerdraw sorting

typedef struct tagCOMPAREITEMSTRUCT {
    UINT        CtlType;
    UINT        CtlID;
    HWND        hwndItem;
    UINT        itemID1;
    DWORD       itemData1;
    UINT        itemID2;
    DWORD       itemData2;
    DWORD       dwLocaleId;
} COMPAREITEMSTRUCT, NEAR *PCOMPAREITEMSTRUCT, FAR *LPCOMPAREITEMSTRUCT;

*/
