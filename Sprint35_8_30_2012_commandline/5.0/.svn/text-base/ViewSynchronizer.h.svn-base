// $Header: /CAMCAD/4.6/read_wrt/Xxxxx.h 2     11/30/06 9:26p Kurt Van Ness $

#if !defined(__ViewSynchonizer_h__)
#define __ViewSynchonizer_h__

#pragma once

#include "DcaContainer.h"

class CCEtoODBView;

//_________________________________________________________________________________________________
class CViewList
{
private:
   CTypedPtrListContainer<CCEtoODBView*> m_viewList;

public:
   CViewList(bool isContainer);
   virtual ~CViewList();
   void empty();

   void addView(CCEtoODBView* view);
   void removeView(CCEtoODBView* view);

   POSITION getHeadPosition() const;
   CCEtoODBView* getNext(POSITION& pos) const;
   int getCount() const;
};

//_________________________________________________________________________________________________
class CViewSynchronizer
{
private:
   CViewList m_viewList;
   bool m_enabled;

public:
   CViewSynchronizer();
   virtual ~CViewSynchronizer();

   bool getEnabled() const;
   void setEnabled(bool flag);

   CViewList& getViewList();

   void registerView(CCEtoODBView* view);
   void unregisterView(CCEtoODBView* view);

   void setSize(CCEtoODBView* view,int cx,int cy);
   void setZoom(CCEtoODBView* view,int xPos,int yPos,int scaleNumerator,double scaleDenominator);
   void synchronizeLayers(CCEtoODBView* view);
   void synchronizeLayers(CCEtoODBView* viewToEdit,CCEtoODBView* sourceView);

   void synchronize();
   void empty();
};

#endif
