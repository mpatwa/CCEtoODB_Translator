// $Header: /CAMCAD/DcaLib/DcaTable.h 4     3/23/07 5:19a Rick Faltersack $

#if !defined(__DcaTable_h__)
#define __DcaTable_h__


#pragma once

#include "DcaLib.h"
#include "DcaAttributes.h"

//-------------------------------------------------------------------

class CGTabRow
{
   // Cell data is stored in string array in same order as columns in table.
   // Hence string array index is exact match to column index.

private:
   CString m_name;
   CStringArray m_cellValues;

public:
   bool GetCellValueAt(int colIndx, CString &value);
   void SetCellValueAt(int colIndx, CString value);

   void AddCellValue(CString value) { m_cellValues.Add(value); }

   int GetCellCount()   { return (int)m_cellValues.GetCount(); }

   CString GetName()          { return m_name; }
   void SetName(CString val)  { m_name = val;  }

};

//-------------------------------------------------------------------

class CGTabTable
{
private:
   CString m_name;
   CStringSupArray m_colHeadings;
   CTypedPtrArrayContainer<CGTabRow*> m_rows;  // array presents problem for row removal, list might be better but this is okay for now
   CAttributes* m_attributes;

public:
   CGTabTable();
   CGTabTable(CString name);

   // Don't need destructor, container classes automatically delete all contents in their 
   // own destructors.

   CString GetName() const    { return m_name; }
   void SetName(CString name) { m_name = name; }

   int GetColumnHeadingCount()          const   { return (int)m_colHeadings.GetCount(); }
   CString GetColumnHeadingAt(int indx) const   { return (indx >= 0 && indx < m_colHeadings.GetCount()) ? m_colHeadings.GetAt(indx) : ""; }
   void AddColumnHeading(CString txt)           { m_colHeadings.Add(txt); }

   bool SetCellValueAt(int row, int col, CString value);
   bool GetCellValueAt(int row, int col, CString &value) const;

   void AddRow(CGTabRow *rowDat)    { if (rowDat != NULL) m_rows.Add(rowDat); }
   CGTabRow *GetRowAt(int row) const;

   int GetColCount() const;  // Longest row or number of col titles, whichever is greater
   int GetRowCount() const;

   // Name style differs from above for attrib funcs, done to
   // match standard func names in other objects (inserts, etc).
   CAttributes*  getAttributes() const;
   CAttributes*& getDefinedAttributes();
   CAttributes*& getAttributesRef();

   /*  
    * Things that could easily be added for access convenience
    *
   CGTabRow *GetRowAt(int row);  // return row ptr or NULL if no such row

   bool SetRowAt(int row, CGTabRow *rowData);  // Can reset existing row in one call.
                                               // Return true if successful.
                                               // Can not be used to add rows, since may designate
                                               // a skip in row sequence, which is not allowed.
                                               // (Rows are contiguous, can have a row of empty
                                               // cells but not a skipped row.)
                                               // Return false if row is out of range.

   void AddRow(CGTabRow *rowData);  // Add this row to end of table

   Note that one can subclass CGTabRow for some specific kind of table.
   Say, table has 4 columns.
   Subclass can have constructor that takes all 4 fields, and then adds them to base class
   cells (so "new" can give back the fully filled row). Then that row could be added
   with AddRow(CGTabRow).
   */
};

//-------------------------------------------------------------------

class CGTabTableList
{
private:
   CTypedPtrArrayContainer<CGTabTable*> m_tables;

public:
   void AddTable(CGTabTable *table)   { m_tables.Add(table); }

   // Could easily make these convenience lookup funcs
   // CGTabTable *GetDefinedTable(CString tableName);   // Gets existing or creates new table

   CGTabTable *GetTableAt(int indx) const  { return (indx >= 0 && indx < GetCount()) ? m_tables.GetAt(indx) : NULL; }

   CGTabTable *GetTable(CString tableName);          // Gets table by name if it exists.
   int GetTableIndex(CString tableName);             // Gets index of table if table of given name exists, returns -1 if not found.
   void ClearTable(CString tableName);               // Leaves the table, but makes it empty. 
   void DeleteTable(CString tableName);              // Make all tables with this name vanish from list.

   int GetCount() const   { return (int)m_tables.GetCount(); }

   /*
    * Should have facilities to remove tables as well.
    * CAMCAD does not need this now so not implemented yet.
    */

   
};


#endif
