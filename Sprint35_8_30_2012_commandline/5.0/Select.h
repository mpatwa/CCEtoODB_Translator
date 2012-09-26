// $Header: /CAMCAD/5.0/Select.h 27    6/21/07 8:27p Kurt Van Ness $

/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/

#if !defined(__Select_h__)
#define __Select_h__

#pragma once

#include <afxtempl.h>
#include "data.h"

/********************************************************************************
*
* DEFINITIONS:
*
*  - SelectList         : List of Top Level Entities for Group Editing
*  - SubSelectArray     : Array of Levels of Hierarchy of Selected Entity 
*                          (from top level insert, down through hierarchical inserts, down to bottom level entity)
*
*  - Selected Entity    : Top Level Item found in Search
*                          If exists, is always 1st item in SelectList
*  - Marked Entity      : Rest of Top Level Entites in SelectList for Group Editing
*  - SubSelected Entity : SubEntity in Hierarchy of Selected Entity
*
*/

class SelectStruct
{
private:
   static int m_nextId; // for debugging
   int m_id;

   DataStruct* m_data;
   CDataList* m_parentDataList;

public:
   int filenum;
   double scale;
   double insert_x;
   double insert_y;
   double rotation;
   BOOL mirror;
   int layer;

public:
   SelectStruct();
   SelectStruct(const SelectStruct& other);
   ~SelectStruct();
   SelectStruct& operator=(const SelectStruct& other);

   DataStruct* getData();
   void setData(DataStruct* data);

   CDataList* getParentDataList();
   void setParentDataList(CDataList* dataList);

   bool isValid() const;
   void assertValid() const;
};

//_____________________________________________________________________________
class CSelectStructListContainer
{
private:
   CTypedPtrListContainer<SelectStruct*> m_list;

public:
   CSelectStructListContainer();
   ~CSelectStructListContainer();
   void empty();

   void addTail(SelectStruct* selectStruct);
   void addHead(SelectStruct* selectStruct);
   SelectStruct* addHead(CDataList* dataList,DataStruct* data,const int fileNumber,
      const double xOrigin,const double yOrigin,const double scale,const double rotation,const int mirror);

   POSITION getHeadPosition() const;
   SelectStruct* getNext(POSITION& pos) const;
   SelectStruct* takeHead();
   int getCount() const;
};

//_____________________________________________________________________________
class CSelectList
{
private:
   CTypedPtrList<CPtrList, SelectStruct*> m_selectList;
   mutable int m_modificationCount;

public:
   CSelectList(INT_PTR nBlockSize = 10);

// new functionality
   int getModificationCount() const;
   int incrementModificationCount();

// Attributes (head and tail)
	// count of elements
	INT_PTR GetCount() const;
	INT_PTR GetSize() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	SelectStruct*& GetHead();
	const SelectStruct* GetHead() const;
	SelectStruct*& GetTail();
	const SelectStruct* GetTail() const;

   SelectStruct* getSelected();

// Operations
	// get head or tail (and remove it) - don't call on empty list!
	SelectStruct* RemoveHead();
	SelectStruct* RemoveTail();

	// add before head or after tail
	POSITION AddHead(SelectStruct* newElement);
	POSITION AddTail(SelectStruct* newElement);

	// add another list of elements before head or after tail
	//void AddHead(CPtrList* pNewList);
	//void AddTail(CPtrList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	SelectStruct*& GetNext(POSITION& rPosition); // return *Position++
	const SelectStruct* GetNext(POSITION& rPosition) const; // return *Position++
	SelectStruct*& GetPrev(POSITION& rPosition); // return *Position--
	const SelectStruct* GetPrev(POSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	SelectStruct*& GetAt(POSITION position);
	const SelectStruct* GetAt(POSITION position) const;
	void SetAt(POSITION pos, SelectStruct* newElement);

	void RemoveAt(POSITION position);
	void DeleteAt(POSITION position);

	// inserting before or after a given position
	POSITION InsertBefore(POSITION position, SelectStruct* newElement);
	POSITION InsertAfter(POSITION position, SelectStruct* newElement);

	// helper functions (note: O(n) speed)
	POSITION Find(SelectStruct* searchValue, POSITION startAfter = NULL) const;
						// defaults to starting at the HEAD
						// return NULL if not found
	POSITION FindIndex(INT_PTR nIndex) const;
						// get the 'nIndex'th element (may return NULL)

private:
   bool checkReference(SelectStruct* selectStruct) const;
};

//_____________________________________________________________________________
class CSelectStack
{
private:
   CTypedPtrArrayContainer<SelectStruct*> m_stack;
   int m_levelIndex;

public:
   CSelectStack();
   ~CSelectStack();

   void empty();

   int getTopIndex() const;
   int getLevelIndex() const;
   bool incrementLevelIndex();
   bool decrementLevelIndex();
   void limitLevelIndex(int maxIndex);
   bool isValidLevelIndex(int index);
   bool isLevelParentValid() const;

   int getSize() const;

   CTMatrix getTMatrixForLevel(int levelIndex);
   CTMatrix getTMatrixForLevelIndex();

   bool somethingIsSelected() const;
   bool nothingIsSelected() const;

   void push(SelectStruct* selectStruct);
   void push(CSelectStructListContainer& selectStructList);
   //void pushOnBottom(SelectStruct* selectStruct);
   //void popAllAndPush(SelectStruct* selectStruct);
   bool contains(SelectStruct* selectStruct);
   void pop();
   void popLevelIndex();
   void popAboveLevelIndex();
   void popLevel(int levelIndex);
   void popData(DataStruct* data);

   SelectStruct* getAt(int index) const;
   SelectStruct* getAtLevel() const;
   SelectStruct* getAtLevelParent() const;

   // debug
   bool assert(bool condition) const;
   bool isValid() const;
   void assertValid() const;

};

// end SELECT.H
#endif
