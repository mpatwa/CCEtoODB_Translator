// $Header: /CAMCAD/DcaLib/DcaNamedView.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaNamedView.h"

//_____________________________________________________________________________
CViewLayerData::CViewLayerData()
{
   m_color = RGB(255,0,0);
   m_show  = true;
}

//_____________________________________________________________________________
CNamedView::CNamedView(const CString& name)
: m_name(name)
{
   init();
}

void CNamedView::init()
{
   m_scaleNum   = 1;
   m_scaleDenom = 1;
   m_scrollX    = 0;
   m_scrollY    = 0;

   m_layerDataArray.empty();
}

CString CNamedView::getName() const 
{
   return m_name;
}

int CNamedView::getScaleNum() const
{
   return m_scaleNum;
}

double CNamedView::getScaleDenom() const
{
   return m_scaleDenom;
}

int CNamedView::getScrollX() const
{
   return m_scrollX;
}

int CNamedView::getScrollY() const
{
   return m_scrollY;
}

int CNamedView::getLayerCount() const
{
   return (int) m_layerDataArray.GetSize();
}

void CNamedView::setName(const CString& name)
{
   m_name = name;
}

void CNamedView::setScaleNum(int scaleNum)
{
   m_scaleNum = scaleNum;
}

void CNamedView::setScaleDenom(double scaleDenom)
{
   m_scaleDenom = scaleDenom;
}

void CNamedView::setScrollX(int scrollX)
{
   m_scrollX = scrollX;
}

void CNamedView::setScrollY(int scrollY)
{
   m_scrollY = scrollY;
}

void CNamedView::setAt(int layerIndex,COLORREF color,bool show)
{
   if (layerIndex >= 0)
   {
      CViewLayerData* layerData = NULL;

      if (layerIndex < m_layerDataArray.GetSize())
      {
         layerData = m_layerDataArray.GetAt(layerIndex);
      }

      if (layerData == NULL)
      {
         layerData = new CViewLayerData();
         m_layerDataArray.SetAtGrow(layerIndex,layerData);
      }

      layerData->setColor(color);
      layerData->setShow(show);
   }
}

COLORREF CNamedView::getColorAt(int layerIndex)
{
   return getLayerDataAt(layerIndex).getColor();
}

bool CNamedView::getShowAt(int layerIndex)
{
   return getLayerDataAt(layerIndex).getShow();
}

//void CNamedView::setColorAt(int layerIndex,COLORREF color)
//{
//}
//
//void CNamedView::setShowAt(int layerIndex,bool show)
//{
//}

CViewLayerData CNamedView::getLayerDataAt(int index)
{ 
   CViewLayerData retval;

   if (index >= 0 && index < m_layerDataArray.GetSize())
   {
      CViewLayerData* layerData = m_layerDataArray.GetAt(index); 

      if (layerData != NULL)
      {
         retval = *layerData;
      }
   }

   return retval; 
}

void CNamedView::hideAll()
{ 
   for (int layerIndex = 0;layerIndex < m_layerDataArray.GetSize();layerIndex++)
   {
      CViewLayerData* layerData = m_layerDataArray.GetAt(layerIndex); 

      if (layerData != NULL)
      {
         layerData->setShow(false);
      }
   }
}

void CNamedView::showAll()
{ 
   for (int layerIndex = 0;layerIndex < m_layerDataArray.GetSize();layerIndex++)
   {
      CViewLayerData* layerData = m_layerDataArray.GetAt(layerIndex); 

      if (layerData != NULL)
      {
         layerData->setShow(true);
      }
   }
}

const CViewLayerDataArray& CNamedView::getViewLayerDataArray() const
{
   return m_layerDataArray;
}

//_____________________________________________________________________________
CNamedView* CNamedViewList::getAt(const CString& viewName)
{
   CNamedView* retval = NULL;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CNamedView* view = GetNext(pos);

      if (view != NULL && view->getName() == viewName)
      {
         retval = view;
         break;
      }
   }

   return retval;
}

bool CNamedViewList::deleteAt(const CString& viewName)
{
   bool retval = false;

   CNamedView* view = getAt(viewName);

   if (view != NULL)
   {
      POSITION pos = Find(view);

      RemoveAt(pos);
      delete view;

      retval = true;
   }

   return retval;
}
