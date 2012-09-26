// $Header: /CAMCAD/5.0/Dca/DcaQfe.h 4     3/12/07 12:47p Kurt Van Ness $

#if !defined(__DcaQfe_h__)
#define __DcaQfe_h__

#pragma once

#include "DcaQfeLib.h"
#include "DcaExtent.h"
#include "DcaLib.h"

//_____________________________________________________________________________
template<class OBJECT>
class CTypedQfe : public CQfe
{
private:
   CPoint2d m_origin;
   OBJECT* m_object;

public:
   CTypedQfe(const CPoint2d& origin,OBJECT* object);
   virtual ~CTypedQfe();

public:
   // CQfe overrides
   virtual CPoint2d getOrigin() const { return m_origin; }
   virtual CExtent getExtent() const;
   virtual CString getInfoString() const;

   // CObject2d overrides
   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const { return 0; }
   virtual bool isValid() const;

   // contents
   virtual OBJECT* getObject() const { return m_object; }
   virtual void setObject(OBJECT* object) { m_object = object; }
};

//_____________________________________________________________________________
template<class OBJECT>
class CTypedQfeList : public CTypedPtrList<CQfeList,CTypedQfe<OBJECT>* >
{
private:
   bool m_isContainer;

public:
   CTypedQfeList(bool isContainer=false,int nBlockSize=200);
   ~CTypedQfeList();
   void empty();
};

//_____________________________________________________________________________
template<class OBJECT>
class CTypedQfeTree : public CQfeExtentLimitedContainer
{
private:

public:
   CTypedQfeTree(bool isContainer=true);
   ~CTypedQfeTree();

public:
   virtual int search(const CExtent& extent,CTypedQfeList<OBJECT>& foundList)
      { return CQfeExtentLimitedContainer::search(extent,foundList); }

   virtual CTypedQfe<OBJECT>* findFirst(const CExtent& extent)
      { return (CTypedQfe<OBJECT>*)CQfeExtentLimitedContainer::findFirst(extent); }

   virtual CTypedQfe<OBJECT>* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (CTypedQfe<OBJECT>*)CQfeExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual CTypedQfe<OBJECT>* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (CTypedQfe<OBJECT>*)CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual CTypedQfe<OBJECT>* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (CTypedQfe<OBJECT>*)CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(CTypedQfe<OBJECT>* qfe)
      { CQfeExtentLimitedContainer::setAt(qfe); }
};

//_____________________________________________________________________________
template<class OBJECT>
class CDerivedQfeList : public CTypedPtrList<CQfeList,OBJECT* >
{
private:
   bool m_isContainer;

public:
   CDerivedQfeList(bool isContainer=false,int nBlockSize=200);
   ~CDerivedQfeList();
   void empty();
};

//_____________________________________________________________________________
template<class OBJECT>
class CDerivedQfeTree : public CQfeExtentLimitedContainer
{
private:

public:
   CDerivedQfeTree(bool isContainer=true);
   ~CDerivedQfeTree();

public:
   virtual int search(const CExtent& extent,CDerivedQfeList<OBJECT>& foundList)
      { return CQfeExtentLimitedContainer::search(extent,foundList); }

   virtual OBJECT* findFirst(const CExtent& extent)
      { return (OBJECT*)CQfeExtentLimitedContainer::findFirst(extent); }

   virtual OBJECT* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (OBJECT*)CQfeExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual OBJECT* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (OBJECT*)CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual OBJECT* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (OBJECT*)CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(OBJECT* qfe)
      { CQfeExtentLimitedContainer::setAt(qfe); }
};

//_____________________________________________________________________________
template<class OBJECT>
class CDerivedGraduatedQfeTree : public CQfeGraduatedExtentLimitedContainer
{
private:

public:
   CDerivedGraduatedQfeTree(double granularity,double maxFeatureSize);
   ~CDerivedGraduatedQfeTree();

public:
   virtual int search(const CExtent& extent,CDerivedQfeList<OBJECT>& foundList)
      { return CQfeGraduatedExtentLimitedContainer::search(extent,foundList); }

   virtual OBJECT* findFirst(const CExtent& extent)
      { return (OBJECT*)CQfeGraduatedExtentLimitedContainer::findFirst(extent); }

   virtual OBJECT* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (OBJECT*)CQfeGraduatedExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual OBJECT* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (OBJECT*)CQfeGraduatedExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual OBJECT* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (OBJECT*)CQfeGraduatedExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(OBJECT* qfe)
      { CQfeGraduatedExtentLimitedContainer::setAt(qfe); }
};

//_____________________________________________________________________________
template<class OBJECT>
CTypedQfe<OBJECT>::CTypedQfe(const CPoint2d& origin,OBJECT* object) :
   m_origin(origin),
   m_object(object)
{
}

template<class OBJECT>
CTypedQfe<OBJECT>::~CTypedQfe()
{
}

template<class OBJECT>
CExtent CTypedQfe<OBJECT>::getExtent() const
{
   return CExtent(getOrigin(),getOrigin());
}

template<class OBJECT>
bool CTypedQfe<OBJECT>::isPointWithinDistance(const CPoint2d& point,double distance)
{
   double edgeDistance = _hypot(getOrigin().x - point.x,
                                getOrigin().y - point.y);

   bool retval = (edgeDistance < distance);

   return retval;
}

template<class OBJECT>
bool CTypedQfe<OBJECT>::isExtentIntersecting(const CExtent& extent)
{
   bool retval = (extent.distanceTo(getOrigin()) == 0.);

   return retval;
}

template<class OBJECT>
bool CTypedQfe<OBJECT>::isInViolation(CObject2d& otherObject)
{
   ASSERT(0);
   return false;
}

template<class OBJECT>
CString CTypedQfe<OBJECT>::getInfoString() const
{
   CString retval;

   retval.Format("CTypedQfe: origin=(%s,%s), extent=(%s,%s),(%s,%s)",
      fpfmt(getOrigin().x),
      fpfmt(getOrigin().y),
      fpfmt(getExtent().getXmin()),
      fpfmt(getExtent().getYmin()),
      fpfmt(getExtent().getXmax()),
      fpfmt(getExtent().getYmax())   );

   return retval;
}

template<class OBJECT>
bool CTypedQfe<OBJECT>::isValid() const
{
   bool retval = (AfxIsValidAddress(m_object,sizeof(OBJECT),true) != 0);

   return retval;
}

//_____________________________________________________________________________
template<class OBJECT>
CTypedQfeList<OBJECT>::CTypedQfeList(bool isContainer,int nBlockSize) : 
   CTypedPtrList<CQfeList,CTypedQfe<OBJECT>* >(nBlockSize)
{
   m_isContainer = isContainer;
}

template<class OBJECT>
CTypedQfeList<OBJECT>::~CTypedQfeList()
{
   empty();
}

template<class OBJECT>
void CTypedQfeList<OBJECT>::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CTypedQfe<OBJECT>* qfe = GetNext(pos);
         delete qfe;
      }
   }

   RemoveAll();
}

//_____________________________________________________________________________
template<class OBJECT>
CTypedQfeTree<OBJECT>::CTypedQfeTree(bool isContainer) : 
   CQfeExtentLimitedContainer(CSize2d(0.,0.),false,isContainer)
{
}

template<class OBJECT>
CTypedQfeTree<OBJECT>::~CTypedQfeTree()
{
}

//_____________________________________________________________________________
template<class OBJECT>
CDerivedQfeList<OBJECT>::CDerivedQfeList(bool isContainer,int nBlockSize) : 
   CTypedPtrList<CQfeList,OBJECT* >(nBlockSize)
{
   m_isContainer = isContainer;
}

template<class OBJECT>
CDerivedQfeList<OBJECT>::~CDerivedQfeList()
{
   empty();
}

template<class OBJECT>
void CDerivedQfeList<OBJECT>::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         OBJECT* qfe = GetNext(pos);
         delete qfe;
      }
   }

   RemoveAll();
}

//_____________________________________________________________________________
template<class OBJECT>
CDerivedQfeTree<OBJECT>::CDerivedQfeTree(bool isContainer) : 
   CQfeExtentLimitedContainer(CSize2d(0.,0.),false,isContainer)
{
}

template<class OBJECT>
CDerivedQfeTree<OBJECT>::~CDerivedQfeTree()
{
}

//_____________________________________________________________________________
template<class OBJECT>
CDerivedGraduatedQfeTree<OBJECT>::CDerivedGraduatedQfeTree(double granularity,double maxFeatureSize) :
   CQfeGraduatedExtentLimitedContainer(granularity,maxFeatureSize)
{
}

template<class OBJECT>
CDerivedGraduatedQfeTree<OBJECT>::~CDerivedGraduatedQfeTree()
{
}

#endif
