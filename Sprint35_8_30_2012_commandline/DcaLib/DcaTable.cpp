// $Header: /CAMCAD/DcaLib/DcaTable.cpp 4     3/23/07 5:19a Rick Faltersack $


#include "StdAfx.h"
#include "DcaTable.h"

//*****************************************************************************

bool CGTabRow::GetCellValueAt(int colIndx, CString &value)
{
   // Always at least clears return value.
   // Returns true if colIndx is within range of row, meaning explicit value was found.
   // Returns false otherwise.
   // Note that rows may be "short" of full table def, e.g. if trailing cells were blank
   // and so left out of source data. So false return does not mean colIndx is out
   // of range for table, just that this row has no data there.
   // Note also that return string value may still be blank even when true is returned.
   // This means the cell was explicitly blank.

   value.Empty();

   if (colIndx >= 0 && colIndx < this->m_cellValues.GetCount())
   {
      value = this->m_cellValues.GetAt(colIndx);
      return true;
   }

   return false;
}

//-----------------------------------------------------------------------------

void CGTabRow::SetCellValueAt(int colIndx, CString value)
{
   // Extends row as needed to add this column.
   // Only out of range is less than zero.
   // Perhaps there ought to be an upper bound for sanity.

   if (colIndx >= 0)
   {
      this->m_cellValues.SetAtGrow(colIndx, value);
   }
}

//*****************************************************************************

CGTabTable::CGTabTable()
: m_attributes(NULL)
{
}


CGTabTable::CGTabTable(CString name)
: m_attributes(NULL)
, m_name(name)
{
}


bool CGTabTable::SetCellValueAt(int row, int col, CString value)
{
   CGTabRow *r = this->GetRowAt(row);

   if (r != NULL)
   {
      r->SetCellValueAt(col, value);
      return true;
   }

   return false;
}

bool CGTabTable::GetCellValueAt(int row, int col, CString &value) const
{
   value.Empty();

   CGTabRow *r = this->GetRowAt(row);

   if (r != NULL)
   {
      return r->GetCellValueAt(col, value);
   }

   return false;
}

int CGTabTable::GetColCount() const
{
   // Column count is number of column names defined or number of
   // cells in longest row, which ever is greater.

   int colCnt = this->m_colHeadings.GetCount();

   for (int i = 0; i < GetRowCount(); i++)
   {
      CGTabRow *r = this->GetRowAt(i);

      if (r != NULL && r->GetCellCount() > colCnt)
         colCnt = r->GetCellCount();
   }

   return colCnt;
}

int CGTabTable::GetRowCount() const
{
   return this->m_rows.GetCount();
}

CGTabRow *CGTabTable::GetRowAt(int row) const
{
   if (row >= 0 && row < GetRowCount())
      return m_rows.GetAt(row);

   return NULL;
}

CAttributes*  CGTabTable::getAttributes() const
{
   return m_attributes;
}

CAttributes*& CGTabTable::getDefinedAttributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return m_attributes;
}

CAttributes*& CGTabTable::getAttributesRef()
{
   return m_attributes;
}

CGTabTable *CGTabTableList::GetTable(CString tableName)
{
   // Gets table by name if it exists.

   for (int i = 0; i < this->GetCount(); i++)
   {
      CGTabTable *table = this->GetTableAt(i);
      if (table->GetName().CompareNoCase(tableName) == 0)
      {
         return table;
      }
   }

   return NULL;
}

int CGTabTableList::GetTableIndex(CString tableName)
{
   // Gets index of table if table of given name exists, returns -1 if not found.

   for (int i = 0; i < this->GetCount(); i++)
   {
      CGTabTable *table = this->GetTableAt(i);
      if (table->GetName().CompareNoCase(tableName) == 0)
      {
         return i;
      }
   }

   return -1;
}

void CGTabTableList::ClearTable(CString tableName)
{
   // Leaves the table, but makes it empty.

   int tableIndx = this->GetTableIndex(tableName);

   if (tableIndx > -1)
   {
      CGTabTable *oldTable = this->GetTableAt(tableIndx);
      CGTabTable *newTable = new CGTabTable(tableName);
      this->m_tables.SetAt(tableIndx, newTable);
      delete oldTable;
   }
}
void CGTabTableList::DeleteTable(CString tableName)
{
   // Make all tables with this name vanish.

   while (GetTableIndex(tableName) > -1)
   {
      this->m_tables.RemoveAt(GetTableIndex(tableName));
   }
}

