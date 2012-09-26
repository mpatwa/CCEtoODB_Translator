// $Header: /CAMCAD/5.0/Select.cpp 67    6/17/07 8:54p Kurt Van Ness $

/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2004. All Rights Reserved.
*/
 
#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "ccview.h"
#include "geomlib.h"
#include "math.h"
#include "font.h"
#include "edit.h"
#include "license.h"
#include "cedit.h"
#include "measure.h"
#include "crypt.h"
#include "attrib.h"
#include "draw.h"
#include "Sch_List.h"
#include "CAMCADNavigator.h"
#include "Debug.h"

//#define DebuggingCase1953

#if defined(_RDEBUG)
#define AssertValid() assertValid()
#define Assert(a) assert(a)
#else
#define AssertValid()
#define Assert(a)
#endif

extern CView *activeView; // from CCVIEW.CPP
extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern bool MarkLast;
//extern MeasureStruct measure; // from MEASURE.CPP
//extern CMeasureDlg *measureDlg; // from MEASURE.CPP
extern BOOL Editing;
void ShowNothingDlg();
extern CPoly *selPoly; // from SEARCH.CPP
extern CEditDialog *editDlg;

static double range;
static double X, Y;
static int polyEntities;
static DataStruct *lastPoly;

void SelectDRCMarker(CCEtoODBDoc *doc, int fileNum, int entityNum);
void JoinPolyline(SelectStruct *selectStruct, CPoly *poly, CCEtoODBDoc *doc);
void LinkPolylines(SelectStruct *selectStruct, CCEtoODBDoc *doc);
void UnlinkPolyline(SelectStruct *selectStruct, CPoly *poly, CCEtoODBDoc *doc);
void InsertModifyUndo(CCEtoODBDoc *doc, BOOL Head);
void MeasureDistance(int decimals);
void CleanGhosts();
void API_EntitySelected(CCEtoODBDoc *doc); // see ccdoc.h
//void NavInstSelectionChanged(DataStruct *data);
//void NavNetSelectionChanged(NetStruct *net);

NetStruct *FindNet(FileStruct *file, CString netname);

//_____________________________________________________________________________
int SelectStruct::m_nextId = 0;

SelectStruct::SelectStruct()
: m_data(NULL)
, m_parentDataList(NULL)
{
   filenum  = -1;
   scale    = 1.;
   insert_x = 0.;
   insert_y = 0.;
   rotation = 0.;
   mirror   = false;
   layer    = -1;

   m_id = m_nextId++;
}

SelectStruct::SelectStruct(const SelectStruct& other)
{
   *this = other;

   m_id = m_nextId++;
}

SelectStruct::~SelectStruct()
{
#ifdef EnableDataStructTracker
   getDataStructTracker().decrementReference(m_data);
#endif

#ifdef EnableSelectStructTracker
   if (getSelectStructTracker().contains(this))
   {
      formatMessageBox("%s.%d - delete of referenced SelectStruct %p",__FILE__,__LINE__,this);
   }
#endif

   m_id = -1;
}

SelectStruct& SelectStruct::operator=(const SelectStruct& other)
{
   if (&other != this)
   {
#ifdef EnableDataStructTracker
   getDataStructTracker().decrementReference(m_data);
   getDataStructTracker().incrementReference(other.m_data);
#endif

      m_data   = other.m_data;
      filenum  = other.filenum;
      scale    = other.scale;
      insert_x = other.insert_x;
      insert_y = other.insert_y;
      rotation = other.rotation;
      mirror   = other.mirror;
      layer    = other.layer;
      m_parentDataList = other.m_parentDataList;
   }

   return *this;
}

DataStruct* SelectStruct::getData()
{
#ifdef DebuggingCase1953
   if (m_data == NULL)
   {
      int iii = 3;
   }
   else if (!AfxIsValidAddress(m_data,sizeof(DataStruct),true))
   {
      int iii = 3;
   }
#endif

   return m_data;
}

void SelectStruct::setData(DataStruct* data)
{
#ifdef EnableDataStructTracker
   getDataStructTracker().decrementReference(m_data);
   getDataStructTracker().incrementReference(data);
#endif

   m_data = data;

#ifdef DebuggingCase1953
   if (m_data != NULL)
   {
      if (!AfxIsValidAddress(m_data,sizeof(DataStruct),true))
      {
         int iii = 3;
      }
      else if (!m_data->isValid())
      {
         int iii = 3;
      }
   }
#endif
}

CDataList* SelectStruct::getParentDataList()
{
   return m_parentDataList;
}

void SelectStruct::setParentDataList(CDataList* dataList)
{
   m_parentDataList = dataList;
}

bool SelectStruct::isValid() const
{
   bool retval = true;

   if (!AfxIsValidAddress(this,sizeof(SelectStruct),true))
   {
      retval = false;
   }
   else if (m_id < 0 || m_id >= m_nextId)
   {
      retval = false;
   }
   else if (m_data != NULL && 
             (!AfxIsValidAddress(m_data,sizeof(DataStruct),true) || !m_data->isValid()))
   {
      retval = false;
   }
   else if (m_parentDataList != NULL && 
              !AfxIsValidAddress(m_parentDataList,sizeof(CDataList),true))
   {
      retval = false;
   }

   return retval;
}

void SelectStruct::assertValid() const
{
   if (!isValid())
   {
      formatMessageBox("SelectStruct::assertValid()");
   }
}

//_____________________________________________________________________________
CSelectStructListContainer::CSelectStructListContainer()
: m_list(true)
{
}

CSelectStructListContainer::~CSelectStructListContainer()
{
}

void CSelectStructListContainer::empty()
{
   m_list.empty();
}

void CSelectStructListContainer::addTail(SelectStruct* selectStruct)
{
   m_list.AddTail(selectStruct);
}

void CSelectStructListContainer::addHead(SelectStruct* selectStruct)
{
   m_list.AddHead(selectStruct);
}

SelectStruct* CSelectStructListContainer::addHead(CDataList* dataList,DataStruct* data,const int fileNumber,
   const double xOrigin,const double yOrigin,const double scale,const double rotation,const int mirror)
{
   SelectStruct* selectStruct = new SelectStruct();

   selectStruct->setData(data);
   selectStruct->filenum  = fileNumber;
   selectStruct->insert_x = (DbUnit)xOrigin;
   selectStruct->insert_y = (DbUnit)yOrigin;
   selectStruct->scale    = (DbUnit)scale;
   selectStruct->rotation = (DbUnit)rotation;
   selectStruct->mirror   = mirror;
   selectStruct->setParentDataList(dataList);

   addHead(selectStruct);

   return selectStruct;
}

POSITION CSelectStructListContainer::getHeadPosition() const
{
   return m_list.GetHeadPosition();
}

SelectStruct* CSelectStructListContainer::getNext(POSITION& pos) const
{
   return m_list.GetNext(pos);
}

SelectStruct* CSelectStructListContainer::takeHead()
{
   SelectStruct* selectStruct = NULL;

   if (m_list.GetCount() > 0)
   {
      selectStruct = m_list.RemoveHead();
   }

   return selectStruct;
}

int CSelectStructListContainer::getCount() const
{
   return m_list.GetCount();
}

//_____________________________________________________________________________
CSelectList::CSelectList(INT_PTR nBlockSize) :
   m_selectList(nBlockSize)
{
   m_modificationCount = 0;
}

int CSelectList::getModificationCount() const
{
   return m_modificationCount;
}

int CSelectList::incrementModificationCount()
{
   return ++m_modificationCount;
}

INT_PTR CSelectList::GetCount() const
{
   return m_selectList.GetCount();
}

INT_PTR CSelectList::GetSize() const
{
   return m_selectList.GetSize();
}

BOOL CSelectList::IsEmpty() const
{
   return m_selectList.IsEmpty();
}

SelectStruct*& CSelectList::GetHead()
{
   return m_selectList.GetHead();
}

const SelectStruct* CSelectList::GetHead() const
{
   return m_selectList.GetHead();
}

SelectStruct*& CSelectList::GetTail()
{
   return m_selectList.GetTail();
}

const SelectStruct* CSelectList::GetTail() const
{
   return m_selectList.GetTail();
}

SelectStruct* CSelectList::getSelected()
{
   SelectStruct* selected = NULL;

   if (! m_selectList.IsEmpty())
   {
      selected = m_selectList.GetHead();

      if (! selected->getData()->isSelected())
      {
         selected = NULL;
      }
   }

   return selected;
}

SelectStruct* CSelectList::RemoveHead()
{
   m_modificationCount++;

   return m_selectList.RemoveHead();
}

SelectStruct* CSelectList::RemoveTail()
{
   m_modificationCount++;

   return m_selectList.RemoveTail();
}

POSITION CSelectList::AddHead(SelectStruct* newElement)
{
   m_modificationCount++;

   checkReference(newElement);

   return m_selectList.AddHead(newElement);
}

POSITION CSelectList::AddTail(SelectStruct* newElement)
{
   m_modificationCount++;

   checkReference(newElement);

   return m_selectList.AddTail(newElement);
}

void CSelectList::RemoveAll()
{
   m_modificationCount++;

   m_selectList.RemoveAll();
}

POSITION CSelectList::GetHeadPosition() const
{
   return m_selectList.GetHeadPosition();
}

POSITION CSelectList::GetTailPosition() const
{
   return m_selectList.GetTailPosition();
}

SelectStruct*& CSelectList::GetNext(POSITION& rPosition)
{
   return m_selectList.GetNext(rPosition);
}

const SelectStruct* CSelectList::GetNext(POSITION& rPosition) const
{
   return m_selectList.GetNext(rPosition);
}

SelectStruct*& CSelectList::GetPrev(POSITION& rPosition)
{
   return m_selectList.GetPrev(rPosition);
}

const SelectStruct* CSelectList::GetPrev(POSITION& rPosition) const
{
   return m_selectList.GetPrev(rPosition);
}

SelectStruct*& CSelectList::GetAt(POSITION position)
{
   return m_selectList.GetAt(position);
}

const SelectStruct* CSelectList::GetAt(POSITION position) const
{
   return m_selectList.GetAt(position);
}

void CSelectList::SetAt(POSITION pos,SelectStruct* newElement)
{
   m_modificationCount++;

   m_selectList.SetAt(pos,newElement);
}

void CSelectList::RemoveAt(POSITION position)
{
   m_modificationCount++;

   m_selectList.RemoveAt(position);
}

void CSelectList::DeleteAt(POSITION position)
{
   m_modificationCount++;

   SelectStruct* selectStruct = m_selectList.GetAt(position);

   m_selectList.RemoveAt(position);

   delete selectStruct;
}

POSITION CSelectList::InsertBefore(POSITION position,SelectStruct* newElement)
{
   m_modificationCount++;

   checkReference(newElement);

   return m_selectList.InsertBefore(position,newElement);
}

POSITION CSelectList::InsertAfter(POSITION position,SelectStruct* newElement)
{
   m_modificationCount++;

   checkReference(newElement);

   return m_selectList.InsertAfter(position,newElement);
}

POSITION CSelectList::Find(SelectStruct* searchValue,POSITION startAfter) const
{
   return m_selectList.Find(searchValue,startAfter);
}

POSITION CSelectList::FindIndex(INT_PTR nIndex) const
{
   return m_selectList.FindIndex(nIndex);
}

bool CSelectList::checkReference(SelectStruct* selectStruct) const
{
   bool retval = false;

#ifdef EnableSelectStructTracker
   retval = getSelectStructTracker().contains(selectStruct);

   if (retval)
   {
      formatMessageBox("CSelectList::checkReference()");
   }
#endif

   return retval;
}

//_____________________________________________________________________________
CSelectStack::CSelectStack()
: m_levelIndex(-1)
{
   m_stack.SetSize(0,10);
}

CSelectStack::~CSelectStack()
{
}

void CSelectStack::empty()
{
   AssertValid();

#ifdef EnableSelectStructTracker
   for (int index = 0;index < m_stack.GetSize();index++)
   {
      SelectStruct* selectStruct = m_stack.GetAt(index);

      getSelectStructTracker().decrementReference(selectStruct);
   }
#endif

   m_stack.empty();
   m_stack.SetSize(0,10);

   m_levelIndex = -1;

   AssertValid();
}

int CSelectStack::getSize() const
{
   AssertValid();

   return m_stack.GetSize();
}

int CSelectStack::getTopIndex() const
{
   AssertValid();

   return m_stack.GetSize() - 1;
}

//void CSelectStack::setTopIndex(int index)
//{
//   m_stack.SetSize(index + 1,10);
//}

int CSelectStack::getLevelIndex() const
{
   AssertValid();

   return m_levelIndex;
}

//void CSelectStack::setLevelIndex(int index)
//{
//   m_levelIndex = index;
//}

bool CSelectStack::incrementLevelIndex()
{
   AssertValid();

   bool retval = (m_levelIndex < (m_stack.GetSize() - 1));

   if (retval)
   {
      m_levelIndex++;
   }

   AssertValid();

   return retval;
}

bool CSelectStack::decrementLevelIndex()
{
   AssertValid();

   bool retval = (m_levelIndex > 0);

   if (retval)
   {
      m_levelIndex--;
   }

   AssertValid();

   return retval;
}

bool CSelectStack::isValidLevelIndex(int index)
{
   AssertValid();

   bool validFlag = (index >= 0 && index < m_stack.GetSize());

   return validFlag;
}

bool CSelectStack::isLevelParentValid() const
{
   AssertValid();

   bool validFlag = (m_levelIndex > 0);

   return validFlag;
}

void CSelectStack::limitLevelIndex(int maxIndex)
{
   AssertValid();

   Assert(maxIndex >= 0);

   if (maxIndex > getTopIndex())
   {
      maxIndex = getTopIndex();
   }

   if (maxIndex >= 0 && m_levelIndex > maxIndex)
   {
      m_levelIndex = maxIndex;
   }

   AssertValid();
}

bool CSelectStack::somethingIsSelected() const
{
   //AssertValid();

   bool retval = (m_stack.GetSize() > 0);

   return retval;
}

bool CSelectStack::nothingIsSelected() const
{
   return !somethingIsSelected();
}

CTMatrix CSelectStack::getTMatrixForLevel(int levelIndex)
{
   AssertValid();

   Assert(isValidLevelIndex(levelIndex));

   CTMatrix matrix;

   for (int index = levelIndex;index >= 0;index--)
   {
      SelectStruct* selectStruct = m_stack.GetAt(index);
      DataStruct* data = selectStruct->getData();

      if (data != NULL && data->getDataType() == dataTypeInsert)
      {
         CTMatrix insertMatrix = data->getInsert()->getTMatrix();

         // use premultiplication
         matrix = matrix * insertMatrix;
      }
   }

   return matrix;
}

CTMatrix CSelectStack::getTMatrixForLevelIndex()
{
   return getTMatrixForLevel(m_levelIndex);
}

void CSelectStack::push(SelectStruct* selectStruct)
{
   AssertValid();

   Assert(!contains(selectStruct));

   m_stack.Add(selectStruct);

   if (m_levelIndex < 0)
   {
      m_levelIndex = 0;
   }

#ifdef EnableSelectStructTracker
   getSelectStructTracker().incrementReference(selectStruct);
#endif

   AssertValid();
}

void CSelectStack::push(CSelectStructListContainer& selectStructList)
{
   while (selectStructList.getCount() > 0)
   {
      SelectStruct* selectStruct = selectStructList.takeHead();

      push(selectStruct);
   }
}

//void CSelectStack::pushOnBottom(SelectStruct* selectStruct)
//{
//   AssertValid();
//
//   Assert(!contains(selectStruct));
//
//   m_stack.InsertAt(0,selectStruct);
//
//   if (m_levelIndex < 0)
//   {
//      m_levelIndex = 0;
//   }
//
//#ifdef EnableSelectStructTracker
//   getSelectStructTracker().incrementReference(selectStruct);
//#endif
//
//   AssertValid();
//}

//void CSelectStack::popAllAndPush(SelectStruct* selectStruct)
//{
//   AssertValid();
//
//   Assert(selectStruct->isValid());
//
//   empty();
//
//   push(selectStruct);
//
//   AssertValid();
//}

bool CSelectStack::contains(SelectStruct* querySelectStruct)
{
   bool retval = false;

   for (int index = 0;index < m_stack.GetSize();index++)
   {
      SelectStruct* selectStruct = m_stack.GetAt(index);

      if (selectStruct == querySelectStruct)
      {
         retval = true;
         break;
      }
   }

   return retval;
}

SelectStruct* CSelectStack::getAt(int index) const
{
   AssertValid();

   Assert(index >= 0 && index < m_stack.GetSize());

   SelectStruct* selectStruct = m_stack.GetAt(index);

   Assert(selectStruct->isValid());

   AssertValid();

   return selectStruct;
}

SelectStruct* CSelectStack::getAtLevel() const
{
   AssertValid();

   SelectStruct* selectStruct = getAt(m_levelIndex);

   AssertValid();

   return selectStruct;
}

SelectStruct* CSelectStack::getAtLevelParent() const
{
   AssertValid();

   SelectStruct* selectStruct = getAt(m_levelIndex - 1);

   AssertValid();

   return selectStruct;
}

void CSelectStack::pop()
{
   AssertValid();

   int stackSize = m_stack.GetSize();

   Assert(stackSize > 0);

   if (stackSize > 0)
   {
      SelectStruct* selectStruct = m_stack.GetAt(stackSize - 1);

#ifdef EnableSelectStructTracker
   getSelectStructTracker().decrementReference(selectStruct);
#endif

      delete selectStruct;

      m_stack.SetSize(stackSize - 1,10);
   }

   AssertValid();
}

void CSelectStack::popLevel(int levelIndex)
{
   AssertValid();

   Assert(levelIndex >= 0);

   while (m_stack.GetSize() > levelIndex)
   {
      pop();
   }

   //m_levelIndex = m_stack.GetSize(); // why did ClearSubSelectArray() set levelIndex above topStackIndex
   m_levelIndex = m_stack.GetSize() - 1;

   AssertValid();
}

void CSelectStack::popData(DataStruct* data)
{
   AssertValid();

   for (int index = 0;index < m_stack.GetSize();index++)
   {
      SelectStruct* selectStruct = m_stack.GetAt(index);

      if (selectStruct->getData() == data)
      {
         popLevel(index);
         break;
      }
   }

   AssertValid();
}

void CSelectStack::popLevelIndex()
{
   AssertValid();

   popLevel(m_levelIndex);

   AssertValid();
}

void CSelectStack::popAboveLevelIndex()
{
   AssertValid();

   popLevel(m_levelIndex + 1);

   AssertValid();
}

bool CSelectStack::assert(bool condition) const
{
   if (! condition)
   {
      formatMessageBox("Assertion failed CSelectStack::assert()");
   }

   return condition;
}

bool CSelectStack::isValid() const
{
   bool validFlag = true;

   if (m_levelIndex < -1)
   {
      validFlag = false;
   }
   else if (m_levelIndex >= m_stack.GetSize())
   {
      validFlag = false;
   }
   else
   {
      for (int index = 0;validFlag && index < m_stack.GetSize();index++)
      {
         SelectStruct* selectStruct = m_stack.GetAt(index);

         if (selectStruct == NULL)
         {
            validFlag = false;
         }
         else if (! selectStruct->isValid())
         {
            validFlag = false;
         }

         for (int index2 = 0;validFlag && index2 < index;index2++)
         {
            SelectStruct* selectStruct2 = m_stack.GetAt(index2);

            if (selectStruct2->getData() == selectStruct->getData())
            {
               validFlag = false;
            }
         }
      }
   }

   return validFlag;
}

void CSelectStack::assertValid() const
{
   if (!isValid())
   {
      isValid();
      formatMessageBox("CSelectStack::assertValid()");
   }
}

/*****************************************************************************
* UnselectAll 
*/
void CCEtoODBDoc::UnselectAll(BOOL Redraw)
{
   unselectAll(Redraw != 0,true);
}

void CCEtoODBDoc::unselectAll(bool redrawFlag,bool checkVisibleEntitiesOnlyFlag)
{
   if (Editing)
   {
		ShowNothingDlg();
		//editDlg->ShowWindow(SW_HIDE);    
   }

   getSelectStack().empty();
   
   for (POSITION selPos = SelectList.GetHeadPosition();selPos != NULL;)
   {
      SelectStruct *selectStruct = SelectList.GetNext(selPos);
      selectStruct->getData()->setSelected(false);
      selectStruct->getData()->setMarked(false);

      int layer = selectStruct->getData()->getLayerIndex();
      BlockStruct *subblock;

      if (redrawFlag && 
          (!checkVisibleEntitiesOnlyFlag || IsEntityVisible(selectStruct->getData(), &subblock, this, (selectStruct->mirror?MIRROR_FLIP:0), &layer, FALSE)))
      {
         DrawEntity(selectStruct, 0, FALSE);
      }

      delete selectStruct;
   }

   SelectList.RemoveAll();
   
	/*if (Editing)
      ShowNothingDlg();*/
}

/*****************************************************************************
* MakeThisEntitySelected 
*/
SelectStruct *CCEtoODBDoc::MakeThisEntitySelected(DataStruct *p, int filenum, double insert_x, double insert_y, double scale, double rotation, int mirror, int layer, CDataList *DataList)
{
   UnselectAll(FALSE);

   // select entity
   p->setSelected(true);
   p->setMarked(false);

   // allocate select structs
   SelectStruct *s1, *s2;
   s1 = new SelectStruct;
   s2 = new SelectStruct;
   s1->setData(p);
   s2->setData(p);
   s1->filenum = s2->filenum = filenum;
   s1->insert_x = s2->insert_x = (DbUnit)insert_x;
   s1->insert_y = s2->insert_y = (DbUnit)insert_y;
   s1->scale = s2->scale = (DbUnit)scale;
   s1->rotation = s2->rotation = (DbUnit)rotation;
   s1->mirror = s2->mirror = mirror;
   s1->layer = s2->layer = layer;
   s1->setParentDataList(DataList);
   s2->setParentDataList(DataList);

   // put in SubSelectArray
   getSelectStack().empty();
   getSelectStack().push(s1);
   //getSelectStack()[0] = s1;
   //setSelectStackTopIndex(0);
   //setSelectStackLevelIndex(0);

   // put in SelectList
   SelectList.AddHead(s2);

   return s2;
}

/*****************************************************************************
* SelectEntity 
*
* DESCRIPTION
*  - Called after SubSelectArray is filled
*
*  - Updates Current Settings
*  - Draws entity as selected
*  - Handles Measure, Edit Entity, some Polyedit, etc.
*
*/
void CCEtoODBDoc::SelectEntity(BOOL SearchAgain)
{
#ifndef SHAREWARE

   ::SendMessage(StatusBar->m_hWnd, WM_SETTEXT, 0, (LPARAM)"");

   if (getSelectStack().nothingIsSelected())
      return; // must have deleted selected item

   API_EntitySelected(this);

   SelectStruct *s = getSelectStack().getAtLevel();

   int matchResult = 0;
   CString matchedName;
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   if (s != NULL && s->getData() !=  NULL)
   {
      if (schLinkPCBDoc)
         matchResult = pApp->getSchematicLinkController().getMatchForPcb(*s->getData(), matchedName);
      else
         matchResult = pApp->getSchematicLinkController().getMatchForSchematic(*s->getData(), matchedName);
   }

   /*if (s != NULL && getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      SetCurrentFile(s->filenum);
      SetCurrentLayer(s->getData()->getLayerIndex());

      switch (s->getData()->getDataType()) 
      { 
      case T_POLY: 
         if (selPoly)
         {
				short widthIndex = selPoly->getWidthIndex();
            if (widthIndex > -1)
               SetCurrentWidth(getWidthTable()[widthIndex]); 
         }
         else
         {
            CPoly *temp = s->getData()->getPolyList()->GetHead();
				short widthIndex = temp->getWidthIndex();
            if (widthIndex > -1)
               SetCurrentWidth(getWidthTable()[widthIndex]);
         }

         if (s->getData()->getAttributesRef())
         {
            Attrib* attrib;

            if (s->getData()->getAttributesRef()->Lookup(IsKeyWord(ATT_NETNAME, 0), attrib))
            {
               CString buf = "NetName => ";
               buf += attrib->getStringValue();

               bool isSchematicLinkInSession = pApp->getSchematicLinkController().isSchematicLinkInSession();

               if (isSchematicLinkInSession)
               {
                  switch (matchResult)
                  {
                     case ERR_NOERROR:
                        buf += " [Matched with " + matchedName + "]";
                        break;
                     case ERR_NOTFOUND:
                     case ERR_NODATA:
                        buf += " [No Match]";
                        break;
                  }
               }

               ::SendMessage(StatusBar->m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)buf);

               // if the compin editor is open, select it
               FileStruct *file = ((CCEtoODBDoc*)(activeView->GetDocument()))->Find_File(s->filenum);
               CString netName = attrib->getStringValue();

               if (((CCEtoODBView*)activeView)->schListDlg &&
                     ((CCEtoODBView*)activeView)->schListDlg->IsWindowVisible() && file)
               {
						if (s->getData()->getGraphicClass() == graphicClassSignal)
						{
							NetStruct *net = FindNet(file, netName);

							if (net)
								((CCEtoODBView*)activeView)->schListDlg->NetSelectionChanged(net);                   
						}
						else if (s->getData()->getGraphicClass() == graphicClassBus)
						{
							CBusStruct *bus = file->getBusList().Find(netName);

							if (bus)
								((CCEtoODBView*)activeView)->schListDlg->BusSelectionChanged(bus);                   
						}
               }

               NetStruct *net = FindNet(file, netName);

               if (net)
                  NavNetSelectionChanged(net);                   
            }
         }

         break;
      case T_TEXT: 
         SetCurrentWidth(getWidthTable()[s->getData()->getText()->getPenWidthIndex()]); 
         SetCurrentTextHeight(s->getData()->getText()->getHeight());
         SetCurrentMirror(s->getData()->getText()->isMirrored());

         break;
      case T_INSERT:
         SetCurrentScale(s->getData()->getInsert()->getScale());
         SetCurrentRotation(s->getData()->getInsert()->getAngle());
         SetCurrentMirror(s->getData()->getInsert()->getMirrorFlags());
         DataStruct *data = s->getData();

         if (s->getData()->getInsert()->getRefname())
         {
            CString buf = "RefDes => ";
            buf +=  s->getData()->getInsert()->getRefname();

            bool isSchematicLinkInSession = pApp->getSchematicLinkController().isSchematicLinkInSession();

            if (isSchematicLinkInSession)
            {
               switch (matchResult)
               {
                  case ERR_NOERROR:
                     buf += " [Matched with " + matchedName + "]";
                     break;
                  case ERR_NOTFOUND:
                  case ERR_NODATA:
                     buf += " [No Match]";
                     break;
               }
            }

            int keyword = IsKeyWord(ATT_PARTNUMBER, 0);

            if (keyword >= 0 && s->getData()->getAttributesRef())
            {
               Attrib* attrib;

               if (s->getData()->getAttributesRef()->Lookup(keyword, attrib))
               {
                  buf += (CString)"   PartNumber => " + get_attvalue_string(this, attrib);
               }
            }

            ::SendMessage(StatusBar->m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)buf);
            
            if (((CCEtoODBView*)activeView)->schListDlg &&
                  ((CCEtoODBView*)activeView)->schListDlg->IsWindowVisible())
               ((CCEtoODBView*)activeView)->schListDlg->InstSelectionChanged(data);
				NavInstSelectionChanged(data);
         }

         // overwrite with netname
         if (data->getAttributesRef())
         {
            Attrib* attrib;

            if (data->getAttributesRef()->Lookup(IsKeyWord(ATT_NETNAME, 0), attrib))
            {
               CString buf = "NetName => ";
               buf += attrib->getStringValue();

               bool isSchematicLinkInSession = pApp->getSchematicLinkController().isSchematicLinkInSession();

               if (isSchematicLinkInSession)
               {
                  switch (matchResult)
                  {
                     case ERR_NOERROR:
                        buf += " [Matched with " + matchedName + "]";
                        break;
                     case ERR_NOTFOUND:
                     case ERR_NODATA:
                        buf += " [No Match]";
                        break;
                  }
               }

               ::SendMessage(StatusBar->m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)buf);
            }
         }

         if (data->getInsert()->getInsertType() == INSERTTYPE_DRCMARKER)
         {
            SelectDRCMarker(this, s->filenum, data->getEntityNumber());
         }

         break;
      }
   }*/

   FlushQueue();
   POSITION pos;
   pos = GetFirstViewPosition();
   //while (pos != NULL)
      //((CCEtoODBView *)GetNextView(pos))->HideSearchCursor();
   FlushQueue();

   CMeasure* measure = ((CCEtoODBView*)activeView)->getMeasure();
   if (measure != NULL && (measure->getToObject().isSet() || measure->getFromObject().isSet()))
      measure->cleanGhosts();

   DrawSubSelected();

   if (Editing)
      OnEditEntity();

   // if this is the top level entity
   if (s != NULL && getSelectStack().getLevelIndex() == 0)
   {
      switch (((CCEtoODBView*)activeView)->cursorMode)
      {
      case Join_Snap:
      case Join_Extend:
      case Join_Chamfer:
      case Join_Fillet:
         if (s->getData()->getDataType() == T_POLY)
            JoinPolyline(s, selPoly, this);
         else
            ErrorMessage("Not a poly!", "Ignoring Selection");
         break;

      case LinkPolys:
         if (s->getData()->getDataType() == T_POLY)
            LinkPolylines(s, this);
         else
            ErrorMessage("Not a poly!", "Ignoring Selection");
         break;

      case UnlinkPoly:
         if (s->getData()->getDataType() == T_POLY)
            UnlinkPolyline(s, selPoly, this);
         else
            ErrorMessage("Not a poly!", "Ignoring Selection");
         break;
      }
   }

   if (s != NULL && measure != NULL)
   {
      if (!measure->getFromObject().isSet() || (SearchAgain && !measure->getToObject().isSet()))
      {
         if (measure->getFromObject().getType() != measureObjectPoint)
         {
            measure->getFromObject().setEntity(s->getData());
            measure->setFromInsertX(s->insert_x);
            measure->setFromInsertY(s->insert_y);
            measure->setFromScale(s->scale);
            measure->setFromRotation(s->rotation);
            measure->setFromMirror(s->mirror);
            measure->setFromDataList(s->getParentDataList());

            measure->getFromObject().set(true);
            measure->getToObject().set(false);
         }
      }
      else if (measure->getToObject().getType() != measureObjectPoint)
      {
         double distance;
         double factor = (getSettings().getXmax() - getSettings().getXmin()) / (maxXCoord - minXCoord);

         CClientDC dc(&measure->getCamCadView());
         //measure->getCamCadView().OnPrepareDC(&dc);

         //if (measure->getToObject().getType() != measureObjectPoint) // if To Entity, set ToEntity to this data
         //{
            measure->getToObject().setEntity(s->getData());
            measure->getToObject().set(true);
         //}

         if (measure->getFromObject().getType() == measureObjectPoint) // measure from point to this data
         {
            Point2 p, pointOnData;

            DTransform pointXform, dataXform;
            dataXform.x = s->insert_x;
            dataXform.y = s->insert_y;
            dataXform.scale = s->scale;
            dataXform.mirror = s->mirror;
            dataXform.SetRotation(s->rotation);

            p.x = measure->getFromObject().getPoint().x;
            p.y = measure->getFromObject().getPoint().y;

            BOOL isCenter = (measure->getToObject().getType()==measureObjectEntityCenter)?TRUE:FALSE;

            distance = FindClosestPoint(this, &p, &pointXform, measure->getToObject().getEntity(), &dataXform, isCenter, &pointOnData);

            measure->getToObject().setPoint(CPoint2d(pointOnData.x, pointOnData.y));
            measure->getToObject().set(true);

            //measure->getCamCadView().DrawCross(&dc, round(measure->getFromObject().getPoint().x / factor), round(measure->getFromObject().getPoint().y / factor), 1.0);
            /*if (distance > 0.0)
               measure->getCamCadView().DrawCross(&dc, round(measure->getToObject().getPoint().x/ factor), round(measure->getToObject().getPoint().y / factor), 1.0);*/
         }
         else // measure from entity to this data
         {
            DTransform xform1, xform2;
            
            xform1.x = measure->getFromInsertX();
            xform1.y = measure->getFromInsertY();
            xform1.scale = measure->getFromScale();
            xform1.mirror = measure->getFromMirror();
            xform1.SetRotation(measure->getFromRotation());

            xform2.x = s->insert_x;
            xform2.y = s->insert_y;
            xform2.scale = s->scale;
            xform2.mirror = s->mirror;
            xform2.SetRotation(s->rotation);

            bool fromCenter = (measure->getFromObject().getType()==measureObjectEntityCenter)?TRUE:FALSE;
            bool toCenter = (measure->getToObject().getType()==measureObjectEntityCenter)?TRUE:FALSE;

            Point2 p1, p2;
            distance = FindClosestPoints(this, 
                  measure->getFromObject().getEntity(), &xform1, fromCenter, 
                  measure->getToObject().getEntity(), &xform2, toCenter, 
                  &p1, &p2);

            measure->getFromObject().setPoint(CPoint2d(p1.x, p1.y));
            measure->getFromObject().set(true);

            measure->getToObject().setPoint(CPoint2d(p2.x, p2.y));
            measure->getToObject().set(true);

            /*measure->getCamCadView().DrawCross(&dc, round(measure->getFromObject().getPoint().x / factor), round(measure->getFromObject().getPoint().y / factor), 1.0);
            if (distance > 0.0)
               measure->getCamCadView().DrawCross(&dc, round(measure->getToObject().getPoint().x/ factor), round(measure->getToObject().getPoint().y / factor), 1.0);*/
         }
         
         measure->updateMeasure();

         if (distance > 0.0)
         {
            dc.MoveTo(round(measure->getFromObject().getPoint().x / factor), round(measure->getFromObject().getPoint().y / factor));
            dc.LineTo(round(measure->getToObject().getPoint().x / factor), round(measure->getToObject().getPoint().y / factor));
         }
      }
   }

   FlushQueue();
   
   for (pos = GetFirstViewPosition();pos != NULL;)
   {
      //((CCEtoODBView *)GetNextView(pos))->ShowSearchCursor();
   }

   FlushQueue();

#endif
}

bool CCEtoODBDoc::somethingIsSelected() const
{
   bool retval = getSelectStack().somethingIsSelected();
   //bool retval = SubSelectArray.somethingIsSelected();

   return retval;
}

bool CCEtoODBDoc::nothingIsSelected() const
{
   return !somethingIsSelected();
}

//int CCEtoODBDoc::getSelectStackTopIndex() const
//{
//   return m_selectStack.getTopIndex();
//}

//void CCEtoODBDoc::setSelectStackTopIndex(int index)
//{
//   m_selectStack.setTopIndex(index);
//}

//int CCEtoODBDoc::getSelectStackLevelIndex() const
//{
//   return m_selectStack.getLevelIndex();
//}

//void CCEtoODBDoc::setSelectStackLevelIndex(int index)
//{
//   m_selectStack.setLevelIndex(index);
//}

//SelectStruct* CCEtoODBDoc::getSelectionAtSelectStackLevel()
//{
//   SelectStruct* selection = getSelectStack().getAtLevel();
//
//   return selection;
//}
//
//SelectStruct* CCEtoODBDoc::getSelectionAt(int index)
//{
//   SelectStruct* selection = getSelectStack().getAt(index);
//
//   return selection;
//}

//CTMatrix CCEtoODBDoc::getTMatrixAtSelectStackLevel()
//{
//   CTMatrix matrix = getSelectStack().getTMatrixForLevel(getSelectStackLevelIndex());
//
//   return matrix;
//}

//CTMatrix CCEtoODBDoc::getTMatrixForSelectionAt(int index)
//{
//   CTMatrix matrix = getSelectStack().getTMatrixForLevel(index);
//
//   return matrix;
//}

CSelectStack& CCEtoODBDoc::getSelectStack()
{
   return m_selectStack;
}

const CSelectStack& CCEtoODBDoc::getSelectStack() const
{
   return m_selectStack;
}

/*****************************************************************************
* InsertInSelectList 
*
* DESCRIPTION
*  - Puts Entity at head of SelectList
*
*/
SelectStruct *CCEtoODBDoc::InsertInSelectList(DataStruct *p, int filenum, double insert_x, double insert_y, 
                        double scale, double rotation, BOOL mirror, CDataList *DataList, BOOL Mark, BOOL ClearSelectedHiearchy,
                        bool addToSelectStack)
{
   // if this will be the new top level item, and hierarchy was not built by Search routine
   if (ClearSelectedHiearchy)
   {
      getSelectStack().empty();

      //if (!Mark)
      //   setSelectStackTopIndex(0);
   }

   // SelectList is a STACK of selected entities
   SelectStruct *selectStruct;
   POSITION lastPos, pos;

   // make sure selected entity is not already in list
   // if it is, remove it now and continue like normal to add it to head
   // because selected entity can only be head   
   for (pos = SelectList.GetHeadPosition();pos != NULL;) 
   {
      lastPos = pos;
      selectStruct = SelectList.GetNext(pos);

      if (selectStruct->getData() == p) // found this item in Select List
      {
         SelectList.RemoveAt(lastPos); // lastPos is the position of selectStruct (pos before calling GetNext)
         delete selectStruct;

         break;
      }
   }

   if (Mark)
   {
      selectStruct = new SelectStruct; 
      SelectList.AddTail(selectStruct);
   }
   else
   {
      if (SelectList.IsEmpty())
      {
         selectStruct = new SelectStruct; // if empty, create new node
         SelectList.AddHead(selectStruct);
      }
      else // items in Select List
      {
         selectStruct = SelectList.GetHead();

         if (selectStruct->getData()->isSelected()) // if head was selected
         {
            selectStruct->getData()->setSelected(false); // unselect (1 selected item at a time)

            if (selectStruct->getData()->isMarked() || MarkLast) // if head was marked 
            {
               selectStruct->getData()->setMarked(true);
               DrawEntity(selectStruct, 2, FALSE); // redraw marked item as marked and no longer selected

               selectStruct = new SelectStruct; // if first item is marked, create new node
               SelectList.AddHead(selectStruct);
            }
            else 
            {
               DrawEntity(selectStruct, 0, FALSE); // if last selected item is not marked, reuse its node after repainting entity
            }
         }
         else // no last selected
         {
            selectStruct = new SelectStruct; 
            SelectList.AddHead(selectStruct);
         }
      }
   }

   selectStruct->setData(p);
   selectStruct->getData()->setSelected(!Mark);
   selectStruct->filenum  = filenum;
   selectStruct->insert_x = (DbUnit)insert_x;
   selectStruct->insert_y = (DbUnit)insert_y;
   selectStruct->scale    = (DbUnit)scale;
   selectStruct->rotation = (DbUnit)rotation;
   selectStruct->mirror   = mirror;
   selectStruct->layer    = -1;
   selectStruct->setParentDataList(DataList);

   if (Mark)
   {
      selectStruct->getData()->setMarked(true);
   }
   else if (addToSelectStack)
   {
      SelectStruct* selectStructCopy = new SelectStruct(*selectStruct);
      getSelectStack().empty();
      getSelectStack().push(selectStructCopy);
   }

   return selectStruct;
}

SelectStruct* CCEtoODBDoc::InsertInSelectList(SelectStruct& selectStruct,bool mark,bool clearSelectedHierarchy,bool addToSelectStack)
{
   SelectStruct* newSelectStruct = InsertInSelectList(selectStruct.getData(),selectStruct.filenum,
                                      selectStruct.insert_x,selectStruct.insert_y,selectStruct.scale,
                                      selectStruct.rotation,selectStruct.mirror,
                                      selectStruct.getParentDataList(),mark,clearSelectedHierarchy,addToSelectStack);

   return newSelectStruct;
}

// end SELECT.CPP