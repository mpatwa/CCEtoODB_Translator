// $Header: /CAMCAD/4.3/RedBlackTree.h 2     12/23/03 1:41p Kurt Van Ness $

/*
   Reference:

   Algorithms in C++
   Robert Sedgewick
   ISBN 0-201-51059-6
   1992

   Chapter 15 - Balanced Trees

*/

/*
   CTypedRedBlackTreeNode
   CTypedPtrRedBlackTreeContainer
      CTypedRedBlackTreeContainer
*/

#if !defined(__RedBlackTree_h__)
#define __RedBlackTree_h__

#include "TypedContainer.h"
#include "WriteFormat.h"

#pragma once

template<class KEY,class OBJECT>
class CTypedRedBlackTreeNodeList;

//_____________________________________________________________________________
template<class KEY,class OBJECT>
class CTypedRedBlackTreeNode 
{
private:
   CTypedRedBlackTreeNode<KEY,OBJECT>* m_l;  // left son node
   CTypedRedBlackTreeNode<KEY,OBJECT>* m_r;  // right son node
   bool m_red;
   KEY m_key;
   CTypedPtrListContainer<OBJECT*> m_list;

public:
   CTypedRedBlackTreeNode(const KEY& key,OBJECT* object,bool isContainer);
   ~CTypedRedBlackTreeNode();

   // access
   CTypedRedBlackTreeNode<KEY,OBJECT>* getLeft();
   CTypedRedBlackTreeNode<KEY,OBJECT>* getRight();
   CTypedPtrListContainer<OBJECT*>& getList();

   // Inserts object at key keyValue and returns true,  
   // If there is already a key with this keyValue in the tree, then the insertion 
   // fails and false is returned 
   bool setAtUnique(const KEY& key,OBJECT* object,CTypedRedBlackTreeNode<KEY,OBJECT>* ggf);
   bool setAt(const KEY& key,OBJECT* object,CTypedRedBlackTreeNode<KEY,OBJECT>* ggf,bool uniqueFlag=true);
   bool lookup(const KEY& key,CPtrList& list);
   void getAscendingList(CPtrList& list);
   void getAscendingNodeList(CTypedRedBlackTreeNodeList<KEY,OBJECT>& list);
   bool isValid();
   bool isNodeValid();
   bool isRedValid();

   CTypedRedBlackTreeNode<KEY,OBJECT>* rotate(const KEY& key);
   CTypedRedBlackTreeNode<KEY,OBJECT>* split(const KEY& key,CTypedRedBlackTreeNode<KEY,OBJECT>* head,CTypedRedBlackTreeNode<KEY,OBJECT>* ggf,
      CTypedRedBlackTreeNode<KEY,OBJECT>* gf,CTypedRedBlackTreeNode<KEY,OBJECT>* f);
   void deleteData();
   void deleteSubtrees();

   void trace(CWriteFormat* writeFormat);
};

template<class KEY,class OBJECT>
CTypedRedBlackTreeNode<KEY,OBJECT>::CTypedRedBlackTreeNode(const KEY& key,OBJECT* object,bool isContainer) :
   m_list(isContainer)
{
   m_key = key;

   if (object != NULL)
   {
      m_list.AddTail(object);
   }

   m_l = NULL;
   m_r = NULL;
   m_red = false;
}

template<class KEY,class OBJECT>
CTypedRedBlackTreeNode<KEY,OBJECT>::~CTypedRedBlackTreeNode()
{
   delete m_l;
   delete m_r;
}

template<class KEY,class OBJECT>
CTypedRedBlackTreeNode<KEY,OBJECT>* CTypedRedBlackTreeNode<KEY,OBJECT>::getLeft()
{
   return m_l;
}

template<class KEY,class OBJECT>
CTypedRedBlackTreeNode<KEY,OBJECT>* CTypedRedBlackTreeNode<KEY,OBJECT>::getRight()
{
   return m_r;
}

template<class KEY,class OBJECT>
CTypedPtrListContainer<OBJECT*>& CTypedRedBlackTreeNode<KEY,OBJECT>::getList()
{
   return m_list;
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeNode<KEY,OBJECT>::getAscendingList(CPtrList& list)
{
   if (m_l != NULL)
   {
      m_l->getAscendingList(list);
   }

   list.AddTail(m_list);

   if (m_r != NULL)
   {
      m_r->getAscendingList(list);
   }
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeNode<KEY,OBJECT>::getAscendingNodeList(CTypedRedBlackTreeNodeList<KEY,OBJECT>& list)
{
   if (m_l != NULL)
   {
      m_l->getAscendingNodeList(list);
   }

   list.AddTail(this);

   if (m_r != NULL)
   {
      m_r->getAscendingNodeList(list);
   }
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeNode<KEY,OBJECT>::lookup(const KEY& key,CPtrList& list)
{
   bool retval = false;
   CTypedRedBlackTreeNode<KEY,OBJECT>* n = this;

   object = NULL;

   while (n != NULL)
   {
      if (key < n->m_key)
      {
         n = n->m_l;
      }
      else if (key > n->m_key)
      {
         n = n->m_r;
      }
      else
      {
         list.AddTail(n->m_list);
         retval = true;
         break;
      }
   }

   return retval;
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeNode<KEY,OBJECT>::setAtUnique(const KEY& key,OBJECT* object,CTypedRedBlackTreeNode<KEY,OBJECT>* head)
{
   return setAt(key,object,head,true);
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeNode<KEY,OBJECT>::setAt(const KEY& key,OBJECT* object,CTypedRedBlackTreeNode<KEY,OBJECT>* head,bool uniqueFlag)
{
   bool retval  = true;
   CTypedRedBlackTreeNode<KEY,OBJECT>* n  = this;
   CTypedRedBlackTreeNode<KEY,OBJECT>* f  = n;
   CTypedRedBlackTreeNode<KEY,OBJECT>* gf = f;
   CTypedRedBlackTreeNode<KEY,OBJECT>* ggf;

   while (true)
   {
      if (n == NULL)
      {
         n = new CTypedRedBlackTreeNode<KEY,OBJECT>(key,object,m_list.isContainer());

         if (key < f->m_key)
         {
            f->m_l = n;
         }
         else
         {
            f->m_r = n;
         }

         n->split(key,head,ggf,gf,f);

         break;
      }

      ggf = gf;
      gf  = f;
      f   = n;

      if (key < n->m_key)
      {
         n = n->m_l;  
      }
      else if (key > n->m_key)
      {
         n = n->m_r;  
      }
      else
      {
         retval = !uniqueFlag;

         if (!uniqueFlag)
         {
            n->m_list.AddTail(object);
         }
         else
         {
            retval = false;
         }

         break;
      }

      if (n != NULL && n->m_l != NULL && n->m_l->m_red && n->m_r != NULL && n->m_r->m_red)
      {
         n = n->split(key,head,ggf,gf,f);
      }
   }
   
   return retval;
}

template<class KEY,class OBJECT>
CTypedRedBlackTreeNode<KEY,OBJECT>* CTypedRedBlackTreeNode<KEY,OBJECT>::rotate(const KEY& key)
{
   CTypedRedBlackTreeNode<KEY,OBJECT> *s,*gs,**ps; // son, grandson, son pointer

   if (key < m_key)
   {
      s = m_l;
      ps = &m_l;
   }
   else
   {
      s = m_r;
      ps = &m_r;
   }

   if (key < s->m_key)
   {
      gs      = s->m_l;
      s->m_l  = gs->m_r;
      gs->m_r = s;
   }
   else
   {
      gs      = s->m_r;
      s->m_r  = gs->m_l;
      gs->m_l = s;
   }

   *ps = gs;

#ifdef __TestMapCoordToOb__
   bool v0 = isNodeValid();
   bool v1 = s->isNodeValid();
   bool v2 = gs->isNodeValid();

   ASSERT(v0 && v1 && v2);
#endif

   return gs;
}

template<class KEY,class OBJECT>
CTypedRedBlackTreeNode<KEY,OBJECT>* CTypedRedBlackTreeNode<KEY,OBJECT>::split(const KEY& key,
   CTypedRedBlackTreeNode<KEY,OBJECT>* head,
   CTypedRedBlackTreeNode<KEY,OBJECT>* ggf,
   CTypedRedBlackTreeNode<KEY,OBJECT>* gf,
   CTypedRedBlackTreeNode<KEY,OBJECT>* f)
{
   CTypedRedBlackTreeNode<KEY,OBJECT>* retval = this;

   m_red = true;

   if (m_l != NULL) m_l->m_red = false;
   if (m_r != NULL) m_r->m_red = false;

   if (f->m_red)
   {
      gf->m_red = true;

      if ((key < gf->m_key) != (key < f->m_key))
      {
         f = gf->rotate(key);
      }

      retval = ggf->rotate(key);
      retval->m_red = false;
   }

   head->m_r->m_red = false;

#ifdef __TestMapCoordToOb__
   bool v0 =      isNodeValid();
   bool v1 =   f->isNodeValid();
   bool v2 =  gf->isNodeValid();
   bool v3 = ggf->isNodeValid();
   bool r0 =      isRedValid();
   bool r1 =   f->isRedValid();
   bool r2 =  gf->isRedValid();
   bool r3 = ggf->isRedValid();

   ASSERT(v0 && v1 && v2 && v3 && r0 && r1 && r2 && r3);
#endif

   return retval;
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeNode<KEY,OBJECT>::deleteData()
{
   if (m_l != NULL)
   {
      m_l->deleteData();
   }

   if (m_r != NULL)
   {
      m_r->deleteData();
   }

   delete m_l;
   m_l = NULL;

   delete m_r;
   m_r = NULL;

   //delete m_object;
   //m_object = NULL;
   m_list.empty();
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeNode<KEY,OBJECT>::deleteSubtrees()
{
   if (m_l != NULL)
   {
      m_l->deleteSubtrees();
   }

   if (m_r != NULL)
   {
      m_r->deleteSubtrees();
   }

   delete m_l;
   m_l = NULL;

   delete m_r;
   m_r = NULL;
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeNode<KEY,OBJECT>::isValid()
{
   bool retval = isNodeValid();
   retval = (isRedValid() && retval);

   if (m_l != NULL)
   {
      retval = (m_l->isValid() && retval);
   }

   if (m_r != NULL)
   {
      retval = (m_r->isValid() && retval);
   }

   if (!retval)
   {
      int iii = 3;
   }

   return retval;
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeNode<KEY,OBJECT>::isNodeValid()
{
   bool retval = true;

   if (m_l != NULL)
   {
      retval = (m_l->m_key < m_key && retval);
   }

   if (m_r != NULL)
   {
      retval = (m_r->m_key >= m_key && retval);
   }

   if (!retval)
   {
      int iii = 3;
   }

   return retval;
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeNode<KEY,OBJECT>::isRedValid()
{
   bool retval = true;

   if (m_red)
   {
      if (m_l != NULL)
      {
         retval = (!m_l->m_red && retval);
      }

      if (m_r != NULL)
      {
         retval = (!m_r->m_red && retval);
      }
   }

   if (!retval)
   {
      int iii = 3;
   }

   return retval;
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeNode<KEY,OBJECT>::trace(CWriteFormat* writeFormat)
{
   if (writeFormat == NULL)
   {
      return;
   }

   bool isValid = true;

   if (m_l != NULL) isValid = (isValid && m_l->m_key <  m_key);
   if (m_r != NULL) isValid = (isValid && m_r->m_key >= m_key);

writeFormat->writef(
"(%p) - ",this);

   m_key.trace(writeFormat);

writeFormat->writef(
", m_red=%d %s\n",m_red,isValid ? "" : "*** Invalid ***");

writeFormat->pushHeader(".");

   if (m_l != NULL)
   {
writeFormat->writef("[Left of (%p) ]\n",this);

      m_l->trace(writeFormat);
   }
   else
   {
writeFormat->writef(
"[Left of (%p) ] - NULL\n",this);
   }

   if (m_r != NULL)
   {
writeFormat->writef("[Right of (%p) ]\n",this);

      m_r->trace(writeFormat);
   }
   else
   {
writeFormat->writef(
"[Right of (%p) ] - NULL\n",this);
   }

writeFormat->popHeader();
}

//_____________________________________________________________________________
template<class KEY,class OBJECT>
class CTypedRedBlackTreeNodeList : public CTypedPtrList<CPtrList,CTypedRedBlackTreeNode<KEY,OBJECT>* > 
{
};

//_____________________________________________________________________________
template<class KEY,class OBJECT>
class CTypedRedBlackTreeContainer
{ 
private:
   bool m_isContainer;
   int m_count;
   CTypedRedBlackTreeNode<KEY,OBJECT>* m_head;
   CTypedPtrArray<CPtrArray,CTypedRedBlackTreeNode<KEY,OBJECT>* >* m_nodes;
   bool m_locked;

public:
   CTypedRedBlackTreeContainer(const KEY& rootKey,bool isContainer=false);
   ~CTypedRedBlackTreeContainer();

   // implementation
   bool setAtUnique(const KEY& key,OBJECT* object);
   bool setAt(const KEY& key,OBJECT* object);
   bool lookup(const KEY& key,CPtrList& list);
   bool isValid();
   void setLocked(bool locked);
   void getAscendingList(CPtrList& list);
   void getAscendingNodeList(CTypedRedBlackTreeNodeList<KEY,OBJECT>& list);

   // iteration
   //OBJECT* getAt(int index);
   int getUpperBound();
   void clearNodes();
   void setNodes();
   void setNodes(CTypedRedBlackTreeNode<KEY,OBJECT>* node);

   //
   int getCount() { return m_count; }
   void empty();

   void trace(CWriteFormat* writeFormat);
   //void test(CWriteFormat* writeFormat);
};

template<class KEY,class OBJECT>
CTypedRedBlackTreeContainer<KEY,OBJECT>::CTypedRedBlackTreeContainer(const KEY& rootKey,bool isContainer)
{
   m_isContainer = isContainer;
   m_count = 0;
   m_head  = new CTypedRedBlackTreeNode<KEY,OBJECT>(rootKey,NULL,isContainer);
   m_nodes = NULL;
   m_locked = false;
}

template<class KEY,class OBJECT>
CTypedRedBlackTreeContainer<KEY,OBJECT>::~CTypedRedBlackTreeContainer()
{
   clearNodes();
   empty();
   delete m_head;
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::clearNodes()
{
   if (m_nodes != NULL)
   {
      ASSERT(!m_locked);

      delete m_nodes;
      m_nodes = NULL;
   }
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::setNodes()
{
   if (m_nodes == NULL)
   {
      m_nodes = new CTypedPtrArray<CPtrArray,CTypedRedBlackTreeNode<KEY,OBJECT>* >();
      m_nodes->SetSize(0,m_count);

      setNodes(m_head);
   }
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::setNodes(CTypedRedBlackTreeNode<KEY,OBJECT>* node)
{
   if (node != NULL)
   {
      setNodes(node->getLeft());
      m_nodes->Add(node);
      setNodes(node->getRight());
   }
}

template<class KEY,class OBJECT>
int CTypedRedBlackTreeContainer<KEY,OBJECT>::getUpperBound()
{
   setNodes();

   return m_nodes->GetUpperBound();
}

//template<class KEY,class OBJECT>
//CObject* CTypedRedBlackTreeContainer<KEY,OBJECT>::getAt(int index)
//{
//   setNodes();
//
//   return m_nodes->GetAt(index);
//}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::setLocked(bool locked)
{
   if (!m_locked && locked)
   {
      setNodes();
   }

   m_locked = locked;
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::empty()
{
   clearNodes();

   if (m_isContainer)
   {
      m_head->deleteData();
   }
   else
   {
      m_head->deleteSubtrees();
   }

   m_count = 0;
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeContainer<KEY,OBJECT>::setAtUnique(const KEY& key,OBJECT* object)
{
   bool retval = false;

   if (!m_locked)
   {
      retval = m_head->setAtUnique(key,object,m_head);

      if (retval)
      {
         m_count++;

         if (m_nodes != NULL)
         {
            clearNodes();
         }
      }
   }
   else
   {
      ASSERT(0);
   }

   return retval;
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeContainer<KEY,OBJECT>::setAt(const KEY& key,OBJECT* object)
{
   bool retval = false;

   if (!m_locked)
   {
      retval = m_head->setAt(key,object,m_head);

      if (retval)
      {
         m_count++;

         if (m_nodes != NULL)
         {
            clearNodes();
         }
      }
   }
   else
   {
      ASSERT(0);
   }

   return retval;
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeContainer<KEY,OBJECT>::lookup(const KEY& key,CPtrList& list)
{
   bool retval = false;

   if (m_head != NULL)
   {
      retval = m_head->lookup(key,list);
   }

   return retval;
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::getAscendingList(CPtrList& list)
{
   if (m_head != NULL)
   {
      m_head->getAscendingList(list);
   }
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::getAscendingNodeList(CTypedRedBlackTreeNodeList<KEY,OBJECT>& list)
{
   if (m_head != NULL)
   {
      m_head->getAscendingNodeList(list);
   }
}

template<class KEY,class OBJECT>
bool CTypedRedBlackTreeContainer<KEY,OBJECT>::isValid()
{
   bool retval = true;

   if (m_head != NULL)
   {
      retval = m_head->isValid();
   }

   return retval;
}

template<class KEY,class OBJECT>
void CTypedRedBlackTreeContainer<KEY,OBJECT>::trace(CWriteFormat* writeFormat)
{
   if (writeFormat == NULL)
   {
      return;
   }

writeFormat->writef(
"{  CMapCoordToOb: (%p) count=%d\n",this,m_count);
writeFormat->pushHeader(".  ");

   if (m_head != NULL)
   {
      m_head->trace(writeFormat);
   }

writeFormat->popHeader();
writeFormat->writef(
"}  CMapCoordToOb\n");
}
//
//template<class KEY,class OBJECT>
//void CTypedRedBlackTreeContainer<KEY,OBJECT>::test(CWriteFormat* writeFormat)
//{
////  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z
////  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
////
////  a  s  e  a  r  c  h  i  n  g  e  x  a  m  p  l  e
////  1 19  5  1 18  3  8  9 14  7  5 24  1 13 16 12  5
//   if (writeFormat == NULL)
//   {
//      return;
//   }
//
//   int testVals[] = {1,19, 5, 1,18, 3, 8, 9,14, 7, 5,24, 1,13,16,12, 5};
//
//   CMapCoordToOb testMap;
//   CPoint2d point(0,0);
//
//   for (int ind = 0;ind < sizeof(testVals)/sizeof(int);ind++)
//   {
//      point.x = testVals[ind];
//      testMap.setAtUnique(point,NULL);
//      bool isValid = testMap.isValid();
//      writeFormat->writef(
//         "CMapCoordToOb::test() - after inserting (%s,%s) %s -------------------------------------------------\n",
//         fpfmt(point.x),fpfmt(point.y),isValid ? "" : "*** Invalid ***");
//      testMap.trace(writeFormat);
//   }
//}

#endif
