// $Header: /CAMCAD/4.6/read_wrt/Xxxxx.cpp 2     11/30/06 9:26p Kurt Van Ness $

#include "StdAfx.h"
#include "ViewSynchronizer.h"
#include "CcView.h"

//_________________________________________________________________________________________________
CViewList::CViewList(bool isContainer)
: m_viewList(isContainer)
{
}

CViewList::~CViewList()
{
}

void CViewList::empty()
{
   m_viewList.empty();
}

void CViewList::addView(CCEtoODBView* view)
{
   POSITION pos = m_viewList.Find(view);

   if (pos == NULL)
   {
      m_viewList.AddTail(view);
   }
}

void CViewList::removeView(CCEtoODBView* view)
{
   POSITION pos = m_viewList.Find(view);

   if (pos != NULL)
   {
      m_viewList.RemoveAt(pos);
   }
}

POSITION CViewList::getHeadPosition() const
{
   return m_viewList.GetHeadPosition();
}

CCEtoODBView* CViewList::getNext(POSITION& pos) const
{
   return m_viewList.GetNext(pos);
}

int CViewList::getCount() const
{
   return m_viewList.GetCount();
}

//_________________________________________________________________________________________________
CViewSynchronizer::CViewSynchronizer()
: m_viewList(false)
, m_enabled(false)
{
}

CViewSynchronizer::~CViewSynchronizer()
{
}

void CViewSynchronizer::empty()
{
   m_viewList.empty();
}

bool CViewSynchronizer::getEnabled() const
{
   return m_enabled;
}

void CViewSynchronizer::setEnabled(bool flag)
{
   m_enabled = flag;
}

CViewList& CViewSynchronizer::getViewList()
{
   return m_viewList;
}

void CViewSynchronizer::registerView(CCEtoODBView* view)
{
   m_viewList.addView(view);
}

void CViewSynchronizer::unregisterView(CCEtoODBView* view)
{
   m_viewList.removeView(view);
}

void CViewSynchronizer::synchronize()
{
   if (m_viewList.getCount() > 1)
   {
      CCEtoODBView* firstView = NULL;
      int cx,cy,xpos,ypos,scaleNumerator;
      double scaleDenominator;
      CExtent extent;

      for (POSITION pos = m_viewList.getHeadPosition();pos != NULL;)
      {
         CCEtoODBView* viewItem = m_viewList.getNext(pos);
         CExtent viewExtent = viewItem->GetDocument()->getSettings().getExtent();

         extent.update(viewExtent);
      }

      for (POSITION pos = m_viewList.getHeadPosition();pos != NULL;)
      {
         CCEtoODBView* viewItem = m_viewList.getNext(pos);

         //viewItem->GetDocument()->getSettings().setExtent(extent);
         SetPageSize(viewItem->GetDocument(),extent.getXmin(),extent.getXmax(),extent.getYmin(),extent.getYmax());
      }

      for (POSITION pos = m_viewList.getHeadPosition();pos != NULL;)
      {
         CCEtoODBView* viewItem = m_viewList.getNext(pos);

         if (firstView == NULL)
         {
            firstView = viewItem;

            xpos = firstView->GetXPos();
            ypos = firstView->GetYPos();
            scaleNumerator = firstView->ScaleNum;
            scaleDenominator = firstView->ScaleDenom;

            WINDOWPLACEMENT windowPlacement;

            firstView->GetWindowPlacement(&windowPlacement);
            cx = windowPlacement.rcNormalPosition.right  - windowPlacement.rcNormalPosition.left;
            cy = windowPlacement.rcNormalPosition.bottom - windowPlacement.rcNormalPosition.top;

            extent = firstView->GetDocument()->getSettings().getExtent();
         }
         else
         {
            //setSize(firstView,cx,cy);
            setZoom(firstView,xpos,ypos,scaleNumerator,scaleDenominator);
         }
      }
   }
}

void CViewSynchronizer::synchronizeLayers(CCEtoODBView* view)
{
   if (getEnabled() && m_viewList.getCount() > 1)
   {
      for (POSITION pos = m_viewList.getHeadPosition();pos != NULL;)
      {
         CCEtoODBView* viewItem = m_viewList.getNext(pos);

         if (viewItem != view)
         {
            synchronizeLayers(viewItem,view);
         }
      }
   }
}

void CViewSynchronizer::synchronizeLayers(CCEtoODBView* viewToEdit,CCEtoODBView* sourceView)
{
   CCEtoODBDoc* docToEdit = viewToEdit->GetDocument();
   CCEtoODBDoc* sourceDoc = sourceView->GetDocument();

   if (docToEdit != sourceDoc)
   {
      const CLayerArray& layersToEdit = docToEdit->getLayerArray();
      const CLayerArray& sourceLayers = sourceDoc->getLayerArray();

      for (int layerToEditIndex = 0;layerToEditIndex < layersToEdit.GetSize();layerToEditIndex++)
      {
         LayerStruct* layerToEdit = layersToEdit.GetAt(layerToEditIndex);

         if (layerToEdit != NULL)
         {
            layerToEdit->setVisible(false);
         }
      }

      for (int sourceLayerIndex = 0;sourceLayerIndex < sourceLayers.GetSize();sourceLayerIndex++)
      {
         LayerStruct* sourceLayer = sourceLayers.GetAt(sourceLayerIndex);

         if (sourceLayer != NULL)
         {
            LayerStruct* layerToEdit = layersToEdit.getLayer(sourceLayer->getName());

            if (layerToEdit != NULL)
            {
               layerToEdit->setVisible(sourceLayer->getVisible());
               layerToEdit->setColor(sourceLayer->getColor());
            }
         }
      }

      viewToEdit->Invalidate();
   }
}

void CViewSynchronizer::setSize(CCEtoODBView* view,int cx,int cy)
{
   if (getEnabled() && view != NULL)
   {
      for (POSITION pos = m_viewList.getHeadPosition();pos != NULL;)
      {
         CCEtoODBView* viewItem = m_viewList.getNext(pos);

         if (viewItem != view)
         {
            WINDOWPLACEMENT windowPlacement;

            viewItem->GetWindowPlacement(&windowPlacement);

            windowPlacement.rcNormalPosition.right  = windowPlacement.rcNormalPosition.left + cx;
            windowPlacement.rcNormalPosition.bottom = windowPlacement.rcNormalPosition.top  + cy;
            windowPlacement.showCmd = SW_SHOWNA;

            viewItem->SetWindowPlacement(&windowPlacement);
         }
      }
   }
}

void CViewSynchronizer::setZoom(CCEtoODBView* view,int xPos,int yPos,int scaleNumerator,double scaleDenominator)
{
   if (getEnabled())
   {
      for (POSITION pos = m_viewList.getHeadPosition();pos != NULL;)
      {
         CCEtoODBView* viewItem = m_viewList.getNext(pos);

         if (viewItem != view)
         {
            viewItem->setZoom(xPos,yPos,scaleNumerator,scaleDenominator);
         }
      }
   }
}

