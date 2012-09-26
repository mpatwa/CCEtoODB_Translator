// $Header: /CAMCAD/4.3/Undo.h 7     8/12/03 9:06p Kurt Van Ness $

/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/           

#pragma once

#include "data.h"

// size of undo array, and thus number of undos or redos (MAX_UNDO_LEVEL - 1)
#define MAX_UNDO_LEVEL  20

// type of action to undo
enum EditType { Edit_Delete, Edit_Add, Edit_Modify };

// structure for an item affected by undo action
//    - an item is (1 line) or (1 polyline) etc.
struct UndoItemStruct
{
   EditType type;
   CDataList  *DataList; // ptr to the database list this entity is in
   long       num;      // entity number (previous node for delete, node for modify, not used for add)
   DataStruct *data;
};

// linked list of all items affected by one undo action
typedef CTypedPtrList<CPtrList, UndoItemStruct*> UndoItemList;

class CCEtoODBDoc;
UndoItemStruct *InsertAddUndo(SelectStruct *s, CCEtoODBDoc *doc);
void InsertModifyUndo(CCEtoODBDoc *doc, BOOL Head);

// end UNDO.H