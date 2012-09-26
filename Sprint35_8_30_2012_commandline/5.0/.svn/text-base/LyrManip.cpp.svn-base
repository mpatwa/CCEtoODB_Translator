// $Header: /CAMCAD/5.0/LyrManip.cpp 37    6/15/07 5:33p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "LyrManip.h"
#include "graph.h"
#include "attrib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CloseEditDlg();

void SendLayerToBack(CCEtoODBDoc *doc, int layerNum)
{
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden()) continue;

      BlockStruct *block = file->getBlock();

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         POSITION curDataPos = dataPos;
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getLayerIndex() == layerNum)
         {
            // Bring this data to the front of the list
            block->getDataList().RemoveAt(curDataPos);
            block->getDataList().AddHead(data);
         }
      }
   }
}

void BringLayerToFront(CCEtoODBDoc *doc, int layerNum)
{
   DataStruct *stopData = NULL;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden()) continue;

      BlockStruct *block = file->getBlock();

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         POSITION curDataPos = dataPos;
         DataStruct *data = block->getDataList().GetNext(dataPos);

         // Check if we need to stop
         if (stopData == data) break;

         if (data->getLayerIndex() == layerNum)
         {
            // Send this data to the end of the list
            block->getDataList().RemoveAt(curDataPos);
            block->getDataList().AddTail(data);

            // Set the last data to stop at
            //  If we don't check this condition, and infinate loop will occur
            if (!stopData)
               stopData = data;
         }
      }
   }
}

/******************************************************************************
* OnLayerManipulation
*/
void CCEtoODBDoc::OnLayerManipulation() 
{
   LayerManipulation dlg;
   dlg.doc = this;
   dlg.DoModal();
   UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// LayerManipulation dialog
LayerManipulation::LayerManipulation(CWnd* pParent /*=NULL*/)
   : CDialog(LayerManipulation::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerManipulation)
   m_function = -1;
   m_removeLayer = FALSE;
   //}}AFX_DATA_INIT
}

LayerManipulation::~LayerManipulation()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void LayerManipulation::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerManipulation)
   DDX_Control(pDX, IDC_NEW_CB, m_newCB);
   DDX_Control(pDX, IDC_ORIG_LB, m_origLB);
   DDX_Radio(pDX, IDC_MOVE, m_function);
   DDX_Check(pDX, IDC_REMOVE_LAYER, m_removeLayer);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LayerManipulation, CDialog)
   //{{AFX_MSG_MAP(LayerManipulation)
   //ON_BN_CLICKED(IDC_MOVE, OnMoveSelected)
   //ON_BN_CLICKED(IDC_COPY, OnCopySelected)
   ON_BN_CLICKED(IDC_DELETE, OnDeleteSelected)
   ON_BN_CLICKED(IDOK, OnGo)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BUTTON1, OnPurgeUnusedLayers)
END_MESSAGE_MAP()

BOOL LayerManipulation::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      int sel;
      switch (nID)
      {
      case IDC_ORIG_LB:
         sel = m_origLB.GetCurSel();
         if (sel != LB_ERR && m_origLB.GetCount() != 0)
         {
            m_origLB.GetText(sel, hint); 
            pTTT->lpszText = hint.GetBuffer(0);
         }
         break;
      case IDC_NEW_CB:
         sel = m_newCB.GetCurSel();
         if (sel != LB_ERR)
         {
            hint = ((LayerStruct*)(m_newCB.GetItemDataPtr(sel)))->getName();
            pTTT->lpszText = hint.GetBuffer(0);
         }
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// LayerManipulation message handlers
BOOL LayerManipulation::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_ORIG_LB), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_NEW_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   m_newCB.EnableWindow(FALSE);

   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL) continue;

      m_origLB.SetItemDataPtr(m_origLB.AddString(layer->getName()), layer);
      m_newCB.SetItemDataPtr(m_newCB.AddString(layer->getName()), layer);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerManipulation::OnMoveSelected() 
{
   m_newCB.EnableWindow();
}

void LayerManipulation::OnCopySelected() 
{
   m_newCB.EnableWindow();
}

void LayerManipulation::OnDeleteSelected() 
{
   m_newCB.EnableWindow(FALSE);
}

void LayerManipulation::OnGo() 
{
   UpdateData();

   if (m_function < 0)
   {
      ErrorMessage("Pick a Function", "No Function Selected");
      return;
   }

   int sel;
   int selCount = m_origLB.GetSelCount();
   if (selCount > 1 && m_function != 2)
   {
      ErrorMessage("Please select one original layer only." ,
         "The function you chose doesn't support multiple selection of layers.");
      return ;
   }

   sel = m_origLB.GetCurSel();
   if (sel == CB_ERR)
   {
      ErrorMessage("Select the Original Layer", "No Original Layer Selected");
      return;
   }
   origLayer = (LayerStruct*)m_origLB.GetItemDataPtr(sel);

   if (m_function != 2)
   {
      sel = m_newCB.GetCurSel();
      if (sel != CB_ERR)
      {
         newLayer = (LayerStruct*)m_newCB.GetItemDataPtr(sel);
      }
      else
      {
         CString buf;
         m_newCB.GetWindowText(buf);
         if (buf.IsEmpty())
         {
            ErrorMessage("Select the New Layer", "No New Layer Selected");
            return;
         }
         newLayer = doc->Add_Layer(buf);
      }
   }
         
   doc->UnselectAll(FALSE);

   CString  f = "Unknown Layer Manipulation Function";

   switch (m_function)
   {
      case 0: // move layer
         f = "Move Layer";
      break;
      case 1: // copy layer 
         f = "Copy Layer";
      break;
      case 2: // delete layer
         f = "Delete Layer";
      break;
   } // switch

   if (ErrorMessage("Warning - Layer Manipulation can't be undone, and the undo buffer will be cleared. Prior steps will be cleared and can't be undone.\nDo you wish to continue?", f, MB_YESNO | MB_DEFBUTTON2)==IDYES)
   {
      switch (m_function)
      {
         case 0: // move layer
            MoveLayer(doc, origLayer, newLayer, m_removeLayer);
            if (m_removeLayer)
               m_origLB.DeleteString(m_origLB.GetCurSel());
         break;
         case 1: // copy layer 
            CopyLayer(doc, origLayer, newLayer);
         break;
         case 2: // delete layer
            int nCount = m_origLB.GetSelCount();
            CArray<int,int> aryListBoxSel;
            //put selected items in the array
            aryListBoxSel.SetSize(nCount);
            m_origLB.GetSelItems(nCount, aryListBoxSel.GetData()); 
            //go through the array of selected items and delete them.
            for (int i = aryListBoxSel.GetCount()-1; i >=0; i--)
            {
               int sel = aryListBoxSel.ElementAt(i);
               origLayer = (LayerStruct*)m_origLB.GetItemDataPtr(sel);

               if (m_removeLayer)
               {
                  m_origLB.DeleteString(sel);
                  m_newCB.DeleteString(sel);
               }

               DeleteLayer(doc, origLayer, m_removeLayer);
            }
            //m_origLB.SetSel(-1,0);
         break;
      } // switch
   } // not undone
}

/******************************************************************************
* MoveLayer
*/
void MoveLayer(CCEtoODBDoc *doc, LayerStruct *origLayer, LayerStruct *newLayer, BOOL Remove)
{
   CWaitCursor wait;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getAttributesRef())
      {
         for (POSITION pos = block->getAttributesRef()->GetStartPosition();pos != NULL;)
         {
            Attrib* attrib;
            WORD keyword;

            block->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

            if (attrib->getLayerIndex() == origLayer->getLayerIndex())
               attrib->setLayerIndex( newLayer->getLayerIndex());
         }
      }

      for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getLayerIndex() == origLayer->getLayerIndex())
            data->setLayerIndex(newLayer->getLayerIndex());

         if (data->getAttributesRef())
         {
            for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
            {
               Attrib* attrib;
               WORD keyword;

               data->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

					AttribIterator attribIterator(attrib);
               Attrib tmpAttrib(attrib->getCamCadData());

					while (attribIterator.getNext(tmpAttrib))
					{
						if (tmpAttrib.getLayerIndex() == origLayer->getLayerIndex())
							attrib->updateLayerOfInstanceLike(tmpAttrib, newLayer->getLayerIndex());
					}

               //if (attrib->getLayerIndex() == origLayer->getLayerIndex())
               //   attrib->setLayerIndex( newLayer->getLayerIndex());
            }
         }
      }
   }

   if (Remove)
      doc->RemoveLayer(origLayer);
}

/******************************************************************************
* CopyLayer
*/
void CopyLayer(CCEtoODBDoc *doc, LayerStruct *origLayer, LayerStruct *newLayer)
{
   CCamCadData& camCadData = doc->getCamCadData();

   CWaitCursor wait;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getLayerIndex() == origLayer->getLayerIndex())
         {
            DataStruct *newData = camCadData.getNewDataStruct(*data);
            newData->setLayerIndex(newLayer->getLayerIndex());
            block->getDataList().AddTail(newData);
         }
      }
   }
}

/******************************************************************************
* DeleteLayer
*/
void DeleteLayer(CCEtoODBDoc *doc, LayerStruct *layer, BOOL Remove)
{
   if (!layer)
      return;

   CWaitCursor wait;

   CloseEditDlg();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;
      
      for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
      {
         POSITION curdataPos = dataPos;
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getLayerIndex() == layer->getLayerIndex())
         {
            // case dts0100580823 - Optimize RemoveOneEntityFromDataList by adding the forth 
            // parameter (the position of the data). It can avoid to search for the position 
            // of the data in  RemoveOneEntityFromDataList.
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data, curdataPos);
         }
         else if (data->getAttributesRef())
         {
            for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
            {
               Attrib* attrib;
               WORD keyword;
               data->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

               if (attrib->isVisible() && attrib->getLayerIndex() == layer->getLayerIndex())
               {
                  attrib->setVisible(false);
                  attrib->setLayerIndex(-1);
               }
            }
         }
      }
   }

   if (Remove)
      doc->RemoveLayer(layer);
}

void CCEtoODBDoc::RemoveLayer(LayerStruct *layer)
{
   if (layer != NULL)
   {
      getLayerArray().removeLayer(layer->getLayerIndex());

      //if (layer->getMirroredLayerIndex() > 0 && layer->getMirroredLayerIndex() <= getMaxLayerIndex())
      //{
      //   LayerStruct *mirrorLayer = getLayerArray()[layer->getMirroredLayerIndex()];

      //   if (mirrorLayer)
      //      mirrorLayer->setMirroredLayerIndex( mirrorLayer->getLayerIndex());
      //}

      //getLayerArray()[layer->getLayerIndex()] = NULL;
      //FreeLayer(layer);
   }
}

/*****************************************************************************/
/*
   Mirrored layers are always marked as used !!!
*/
void CCEtoODBDoc::MarkUnusedLayers()
{
   CCEtoODBDoc* doc = this;

   // mark all as unused
   int i;
   for (i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL) continue;
      layer->setUsed(false);
      if (layer->isFloating()) layer->setUsed(true); // do not delete the floating layer !!!
   }

   // mark used layers
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block) continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_BLOB && data->getLayerIndex() != -1)
         {
            LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];
            if (layer)
               layer->setUsed(true);
         }

         // visible attributes
         if (data->getAttributesRef())
         {
            for (POSITION pos = data->getAttributesRef()->GetStartPosition();pos != NULL;)
            {
               Attrib* attrib;
               WORD keyword;

               data->getAttributesRef()->GetNextAssoc(pos, keyword, attrib);

               if (attrib->isVisible()&&attrib->getLayerIndex()>-1)
               {
                  LayerStruct *layer = doc->getLayerArray()[attrib->getLayerIndex()];

                  if (layer)
                     layer->setUsed(true);
               }
            }
         }
      }
   }
   
   // mark mirrored layers as used if their mirror layer is used
   for (i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

      if (layer == NULL) continue;

      if (layer->isUsed())
         doc->getLayerArray()[layer->getMirroredLayerIndex()]->setUsed(true);
   }
}

/*****************************************************************************/
/*
   Mirrored Layers and electrical stackup Layers are kept intact.
   It is not advised to purge layers which are part of the electrical stackup.
*/
void CCEtoODBDoc::RemoveUnusedLayers(bool purge_electricalstackup)
{
   // here delete unused layers
   MarkUnusedLayers();

   for (int i=0; i < getMaxLayerIndex(); i++)
   {
      if (getLayerArray()[i] == NULL)  continue;

      LayerStruct *layer = getLayerArray()[i];

      if (!purge_electricalstackup && layer->getElectricalStackNumber())  continue;

      if (!layer->isUsed()) RemoveLayer(layer);
   }
}

void CCEtoODBDoc::OnPurgeUnusedLayers() 
{
   CWaitCursor wait;

   MarkUnusedLayers();

   for (int i=0; i < getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = getLayerArray()[i];

      if (layer == NULL) continue;

      if (!layer->isUsed())
      {
         PurgeUnusedLayers dlg;

         dlg.doc = this;
         dlg.m_purge_electrical = FALSE;
         dlg.DoModal();

         return;
      }
   }

   ErrorMessage("All Layers Used", "Purge Unused Layers");
}

/////////////////////////////////////////////////////////////////////////////
// PurgeUnusedLayers dialog
PurgeUnusedLayers::PurgeUnusedLayers(CWnd* pParent /*=NULL*/)
   : CDialog(PurgeUnusedLayers::IDD, pParent)
{
   //{{AFX_DATA_INIT(PurgeUnusedLayers)
   m_purge_electrical = FALSE;
   //}}AFX_DATA_INIT
}

void PurgeUnusedLayers::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PurgeUnusedLayers)
   DDX_Control(pDX, IDC_LIST1, m_layersLB);
   DDX_Check(pDX, IDC_PURGE_ELECTRICAL, m_purge_electrical);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PurgeUnusedLayers, CDialog)
   //{{AFX_MSG_MAP(PurgeUnusedLayers)
   ON_BN_CLICKED(IDC_PURGE_ELECTRICAL, OnPurgeElectrical)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PurgeUnusedLayers message handlers
BOOL PurgeUnusedLayers::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_purge_electrical = FALSE;
   
   // remove unused layers
   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL) continue;
      if (!layer->isUsed())
         m_layersLB.SetItemDataPtr(m_layersLB.AddString(layer->getName()), layer);
   }

   int count = m_layersLB.GetCount();
   if (count > 1)
      m_layersLB.SelItemRange(TRUE, 0, count-1);
   else
      m_layersLB.SetSel(0);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void PurgeUnusedLayers::OnOK() 
{
   int count = m_layersLB.GetSelCount(); // number of files to merge
   if (!count)
      return;
   
   int *indexArray = (int*)calloc(count, sizeof(int)); // array of indices of files to merge

   if (indexArray == NULL)
   {
      ErrorMessage("Memory allocation failure", "PurgeUnusedLayers::OnOK");
   }
   else
   {
      m_layersLB.GetSelItems(count, indexArray);

      // loop files to merge
      for (int i=0; i<count; i++)
      {
         LayerStruct *layer = (LayerStruct*)m_layersLB.GetItemDataPtr(indexArray[i]);
         if (!m_purge_electrical && layer->getElectricalStackNumber()) continue;
         doc->RemoveLayer(layer);
      }                
      free(indexArray);
   }
   
   CDialog::OnOK();
}

void PurgeUnusedLayers::OnPurgeElectrical() 
{
   // TODO: Add your control notification handler code here
   m_purge_electrical = ~m_purge_electrical;
}

void LayerManipulation::OnPurgeUnusedLayers()
{
   if (doc)
        doc->OnPurgeUnusedLayers();
}
