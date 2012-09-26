// $Header: /CAMCAD/4.5/Pin1OrientationDfm.cpp 14    8/04/06 8:55p Kurt Van Ness $

#include "StdAfx.h"
#include "CamCadDatabase.h"
#include "RwLib.h"

int roundRadiansToNearestDegree(double radians)
{
   double degrees = radiansToDegrees(radians);
   
   int retval = (((round(degrees) % 360) + 360) % 360);

   return retval;
}

//_____________________________________________________________________________
typedef CTypedPtrList<CPtrList,InsertStruct*> CInsertStructList;

//_____________________________________________________________________________
class CComponentOrientation : public CObject
{
private:
   int m_blockNumber;
   CTypedPtrMap<CMapWordToPtr,WORD,CInsertStructList*> m_orientations;

public:
   CComponentOrientation(int blockNum);
   ~CComponentOrientation();

   void addOrientation(InsertStruct* component);
   void addDrcs(CCamCadDatabase& camCadDatabase,FileStruct& file,int algorithmIndex,int occuranceFence);
};

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
CComponentOrientation::CComponentOrientation(int blockNum)
{
   m_orientations.InitHashTable(17);
}

CComponentOrientation::~CComponentOrientation()
{
   WORD orientation;
   CInsertStructList* componentList;

   for (POSITION pos = m_orientations.GetStartPosition();pos != NULL;)
   {
      m_orientations.GetNextAssoc(pos,orientation,componentList);

      delete componentList;
   }
}

void CComponentOrientation::addOrientation(InsertStruct* component)
{
   int orientation = roundRadiansToNearestDegree(component->getAngle());

   CInsertStructList* componentList = NULL;

   if (! m_orientations.Lookup(orientation,componentList))
   {
      componentList = new CInsertStructList();
      m_orientations.SetAt(orientation,componentList);
   }

   componentList->AddTail(component);
}

void CComponentOrientation::addDrcs(CCamCadDatabase& camCadDatabase,FileStruct& file,int algorithmIndex,int occuranceFence)
{
   WORD orientation;
   CInsertStructList* componentList;
   CInsertStructList* longestComponentList = NULL;
   int count = 0;
   int modalOrientation = -1;

   for (POSITION pos = m_orientations.GetStartPosition();pos != NULL;)
   {
      m_orientations.GetNextAssoc(pos,orientation,componentList);

      if (longestComponentList == NULL ||
          longestComponentList->GetCount() < componentList->GetCount())
      {
         longestComponentList = componentList;
         InsertStruct* component = longestComponentList->GetHead();
      }

      count += componentList->GetCount();
   }

   if (longestComponentList != NULL)
   {
      InsertStruct* component = longestComponentList->GetHead();
      modalOrientation = roundRadiansToNearestDegree(component->getAngle());
   }

   if (count >= occuranceFence)
   {
      for (POSITION pos = m_orientations.GetStartPosition();pos != NULL;)
      {
         m_orientations.GetNextAssoc(pos,orientation,componentList);

         if (componentList != longestComponentList)
         {
            for (POSITION drcPos = componentList->GetHeadPosition();drcPos != NULL;)
            {
               InsertStruct* component = componentList->GetNext(drcPos);
               CString description;
               description.Format("Irregular component orientation(modal=%d); RefDes=%s",
                  modalOrientation,(const char*)component->getRefname());

               camCadDatabase.addDrc(file,component->getOriginX(),component->getOriginY(),
                  description,drcSimple,drcCriticalFailure,algorithmIndex,drcGeneric);
            }
         }
      }
   }
}

//_____________________________________________________________________________
class CComponentOrientations : public CObject
{
private:
   CTypedPtrMap<CMapPtrToPtr,void*,CComponentOrientation*> m_componentOrientations;

public:
   CComponentOrientations();
   ~CComponentOrientations();

   void addComponentOrientation(InsertStruct* component);
   void addDrcs(CCamCadDatabase& camCadDatabase,FileStruct& file,int algorithmIndex,int occuranceFence);
};

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
CComponentOrientations::CComponentOrientations()
{
   m_componentOrientations.InitHashTable(nextPrime2n(1000));
}

CComponentOrientations::~CComponentOrientations()
{
   void* blockNumber;
   CComponentOrientation* componentOrientation;

   for (POSITION pos = m_componentOrientations.GetStartPosition();pos != NULL;)
   {
      m_componentOrientations.GetNextAssoc(pos,blockNumber,componentOrientation);

      delete componentOrientation;
   }
}

void CComponentOrientations::addComponentOrientation(InsertStruct* component)
{
   CComponentOrientation* componentOrientation = NULL;

   if (! m_componentOrientations.Lookup((void*)(component->getBlockNumber()),componentOrientation))
   {
      componentOrientation = new CComponentOrientation(component->getBlockNumber());
      m_componentOrientations.SetAt((void*)(component->getBlockNumber()),componentOrientation);
   }

   componentOrientation->addOrientation(component);
}

void CComponentOrientations::addDrcs(CCamCadDatabase& camCadDatabase,FileStruct& file,int algorithmIndex,int occuranceFence)
{
   void* blockNumber;
   CComponentOrientation* componentOrientation;

   for (POSITION pos = m_componentOrientations.GetStartPosition();pos != NULL;)
   {
      m_componentOrientations.GetNextAssoc(pos,blockNumber,componentOrientation);

      componentOrientation->addDrcs(camCadDatabase,file,algorithmIndex,occuranceFence);
   }
}

//_____________________________________________________________________________
bool DFM_Pin1OrientationCheck(CCEtoODBDoc& camCadDoc,BlockStruct& block,
   int surfaceMask,int componentTypeMask,int occuranceFence)
{
   bool retval = false;

   while (true)
   {
      if (block.getBlockType() != BLOCKTYPE_PCB)
      {
         break;
      }

      int smdKeywordIndex = camCadDoc.IsKeyWord(ATT_SMDSHAPE,0);
      CComponentOrientations componentOrientationsTop;
      CComponentOrientations componentOrientationsBottom;

      CCamCadDatabase camCadDatabase(camCadDoc);

      for (POSITION dataPos = block.getDataList().GetHeadPosition();dataPos != NULL;)
      {
         DataStruct* data = block.getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT)
         {
            InsertStruct* component = data->getInsert();

            if (component->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               SurfaceTag surface = (component->getPlacedBottom() ? bottomSurface : topSurface);
               CComponentOrientations& componentOrientations = 
                  (component->getPlacedBottom() ? componentOrientationsBottom : componentOrientationsTop);

               if ((surface & surfaceMask) != 0)
               {
                  Attrib* value;

                  ComponentMountTypeTag componentMountType = 
                     (data->getAttributesRef()->Lookup(smdKeywordIndex,value) ? 
                        surfaceMountComponent : thruHoleMountComponent);

                  if ((componentMountType & componentTypeMask) != 0)
                  {
                     componentOrientations.addComponentOrientation(component);
                  }
               }
            }
         }
      }

      int algorithmIndexTop    = GetAlgorithmNameIndex(&camCadDoc,"Irregular Component Orientation Check (Top)");
      int algorithmIndexBottom = GetAlgorithmNameIndex(&camCadDoc,"Irregular Component Orientation Check (Bottom)");
      FileStruct* file = camCadDoc.Find_File(block.getFileNumber());
      componentOrientationsTop.addDrcs(camCadDatabase,*file,algorithmIndexTop,occuranceFence);
      componentOrientationsBottom.addDrcs(camCadDatabase,*file,algorithmIndexBottom,occuranceFence);

      retval = true;

      break;
   }

   return retval;
}