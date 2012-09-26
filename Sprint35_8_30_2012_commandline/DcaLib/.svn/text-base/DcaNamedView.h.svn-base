// $Header: /CAMCAD/5.0/dca/DcaNamedView.h 4     3/19/07 4:34a Kurt Van Ness $

#if !defined(__DcaNamedView_h__)
#define __DcaNamedView_h__

#pragma once

#include "DcaContainer.h"

//_____________________________________________________________________________
class CViewLayerData
{
private:
   COLORREF m_color;
   bool m_show;

public:
   CViewLayerData();


   COLORREF getColor() { return m_color; }
   bool getShow()      { return m_show; }

   void setColor(COLORREF color) { m_color = color; }
   void setShow(bool show)       { m_show = show; }
};

//_____________________________________________________________________________
class CViewLayerDataArray : public CTypedPtrArrayContainer<CViewLayerData*>
{
};

//_____________________________________________________________________________
class CNamedView
{
private:
   CString m_name;
   int m_scaleNum;
   double m_scaleDenom;
   int m_scrollX;
   int m_scrollY;
   CViewLayerDataArray m_layerDataArray;

public:
   CNamedView(const CString& name="");

public:
   void init();

   CString getName()      const;
   int getScaleNum()      const;
   double getScaleDenom() const;
   int getScrollX()       const;
   int getScrollY()       const;
   int getLayerCount()    const;

   void setName(const CString& name);
   void setScaleNum(int scaleNum);
   void setScaleDenom(double scaleDenom);
   void setScrollX(int scrollX);
   void setScrollY(int scrollY);

   COLORREF getColorAt(int layerIndex);
   bool getShowAt(int layerIndex);
   void setAt(int layerIndex,COLORREF color,bool show);
   void setAt(int layerIndex,CViewLayerData viewLayerData) 
      { setAt(layerIndex,viewLayerData.getColor(),viewLayerData.getShow()); }
   void hideAll();
   void showAll();

   const CViewLayerDataArray& getViewLayerDataArray() const;

   CViewLayerData getLayerDataAt(int index);

	//void WriteXML(CWriteFormat& writeFormat);
};

//_____________________________________________________________________________
class CNamedViewList : public CTypedPtrList<CPtrList, CNamedView*>
{
public:
   CNamedView* getAt(const CString& viewName);
   CNamedView* getDefinedAt(const CString& viewName);
   bool deleteAt(const CString& viewName);

	//void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
};

#endif
