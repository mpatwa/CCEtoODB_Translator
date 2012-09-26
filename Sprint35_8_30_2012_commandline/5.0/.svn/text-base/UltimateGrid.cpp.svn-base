// $Header: /CAMCAD/4.6/read_wrt/Xxxxx.cpp 2     11/30/06 9:26p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaInsert.h"
#include "UltimateGrid.h"

//=====================================================================================
// Kurt probably had plans for this, but apparantly they were never fulfilled.
// Or maybe he just wanted to put "include CUltimateGrid.h" instead of CUGCtrl.h.
// But as it stands, this is a frivolous useless indirection.
CUltimateGrid::CUltimateGrid()
{
}

CUltimateGrid::~CUltimateGrid()
{
}
//=====================================================================================

///////////////////////////////////////////////////////////////////////////////////////////
//
// CDDBaseGrid - Base Grid support shared by all DataDoctor grids originally, and now
//               becoming the standard base grid throughout CAMCAD.
//

CDDBaseGrid::CDDBaseGrid()
: m_columnDefMap("GridNameNotSet")
{ 
   m_sortEnabled = true;
   m_sortAscending = true;
   m_THButtonDownCol = myUG_NOT_A_COLUMN;
   m_isSetUp = false;
   m_selStartCol = m_selEndCol = 0;
   m_selStartRow = m_selEndRow = 0;

   m_redBackgroundColor       = RGB(0xff,0xf0,0xf5);
   m_greenBackgroundColor     = RGB(0xf0,0xff,0xf0);
   m_lightGrayBackgroundColor = RGB(226,226,226);
   m_whiteBackgroundColor     = RGB(0xff,0xff,0xff);
}

CDDBaseGrid::CDDBaseGrid(CString gridNameForStickySettings)
: m_columnDefMap(gridNameForStickySettings)
{ 
   m_sortEnabled = true;
   m_sortAscending = true;
   m_THButtonDownCol = myUG_NOT_A_COLUMN;
   m_isSetUp = false;
   m_selStartCol = m_selEndCol = 0;
   m_selStartRow = m_selEndRow = 0;

   m_redBackgroundColor       = RGB(0xff,0xf0,0xf5);
   m_greenBackgroundColor     = RGB(0xf0,0xff,0xf0);
   m_lightGrayBackgroundColor = RGB(226,226,226);
   m_whiteBackgroundColor     = RGB(0xff,0xff,0xff);
}



void CDDBaseGrid::SetRowBackColor(int rowIndx, COLORREF color)
{
   for (int colIndx = 0; colIndx < this->GetNumberCols(); colIndx++)
   {
      this->QuickSetBackColor(colIndx, rowIndx, color);
   }
}



void CDDBaseGrid::CaptureSelection()
{
   // Going into cell edit modes clears multiple selection in UG. Very inconvenient for us.
   // So we have to manage multiple selections ourselves.
   // This func captures the current state of selection for the grid.

   // DD grid edit supports multi-select in one column only.
   // The UG grid does not have a column limitation mode, so may return any selection.
   // We use the column of the first selected cell as a column anchor, ignoring column on any others.
      
   this->m_selStartCol = this->m_selEndCol = -1;
   this->m_selStartRow = this->m_selEndRow = -1;

   int lowRow, lowCol;
   int highRow, highCol;

   int nCol, nRet;
   long nRow;

   // determine the first selected cell
   nRet = this->EnumFirstSelected( &nCol, &nRow );

   if (nRet == UG_SUCCESS)
   {
      this->m_selStartCol = this->m_selEndCol = nCol;
      this->m_selStartRow = this->m_selEndRow = nRow;

      lowCol = highCol = nCol;
      lowRow = highRow = nRow; 
   }

   // keep looping until we've run out of selected cells
   while( nRet == UG_SUCCESS )
   {
     if (nRow < lowRow)
        lowRow = nRow;
     if (nRow > highRow)
        highRow = nRow;

     nRet = this->EnumNextSelected( &nCol, &nRow );
   }

   this->m_selStartRow = lowRow;
   this->m_selEndRow = highRow;
}

void CDDBaseGrid::SelectRow(long row)
{
   if (row >= 0 && row < GetNumberRows() && GetNumberCols() > 0)
   {
      SetMultiSelectMode(TRUE);
      GotoRow(row);
      SelectRange(0, row, (GetNumberCols() - 1), row);
   }
}

void CDDBaseGrid::RefreshSelection()
{
   // Actions like going into cell edit mode clear multi-select, but we don't really
   // want it to. That is why we track the selection ourselves. This function will
   // re-select the last multi-selection.

   int scol;
   long srow;
   if (GetMultiSelectStart(&scol, &srow))
   {
      int ecol;
      long erow;
      if (GetMultiSelectEnd(&ecol, &erow))
      {
         //this->RedrawRange(scol, srow, ecol, erow);
         this->SelectRange(scol, srow, ecol, erow);
      }
   }
}

void CDDBaseGrid::OnCharDown(UINT* vcKey, BOOL processed)
{ 
   // Save the current selection, starting edit will clear a multi-select but we want it.
   CaptureSelection();

   // Pressing a char key starts edit in cell.
   // Passing in a char will wipe out current cell contents, ie char replaces contents,
   // because edit starts with whole cell content selected.
   // If char is Enter then start edit but send no chars, i.e. do not update cell, just start editing.

   if (*vcKey == 13)
   {
      StartEdit( );
   }
   else if (*vcKey == 22) // ctrl-v, aka Paste
   {
      int col = this->GetCurrentCol();
      int row = this->GetCurrentRow();
      CWnd *peditctrl;
      if (this->OnEditStart(col, row, &peditctrl))   // dis-allows pasting into non-edit cells
      {
         this->Paste();
         this->BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
         CString value;
         this->QuickGetText(col, row, &value);
         this->OnEditFinish(col, row, peditctrl, value, FALSE);
      }

      //StartEdit();
   }
   else
   {
      StartEdit(*vcKey);
   }

   RefreshSelection();
}

void CDDBaseGrid::OnKeyDown(UINT *vcKey, BOOL processed)
{
   // This is EVERY KEY - do not start editing, e.g. might be shift key 
   //int jj = 0;
}

void CDDBaseGrid::FlushEvents()
{
   MSG msg;
   BOOL LButtonUp = FALSE;
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

void CDDBaseGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   // Some grids update via a PostMessage, because OnEditFinish is called when editing is about
   // to finish,not after editting is finished. So the update message is posted, edit allowed to 
   // finish, then the grid is updated. This LClicked event might trigger OnEditFinish, but then
   // the update that is posted comes after this LClick we have in hand. If multi-select is active
   // then the multi-select params get wiped out, and the grid update fails to update the whole
   // selection. So flush the queue before continuing with the LClick event.
   FlushEvents();

   CaptureSelection();

#ifdef BUTTON_ACTIONS
   if (updn == TRUE)  // button down
   {
   }

   if (updn == FALSE) // button up
   {
   }
#endif

}

/////////////////////////////////////////////////////////////////////////////
//	OnDClicked
//		Sent whenever the user double clicks the left mouse button within the grid
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void CDDBaseGrid::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);

   // Double click terminates/undoes any multi-select
   // Double click is always a single-cell edit

   this->ClearSelections();
   CaptureSelection();
 

   StartEdit(col, row, 0);
}

#ifdef NEED_MOUSE_MOUVE
void CDDBaseGrid::OnMouseMove(int col,long row,POINT *point,UINT nFlags,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(processed);
}
#endif


int CDDBaseGrid::OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow)
{
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(*newcol);
	UNREFERENCED_PARAMETER(*newrow);

   // Usually get here by arrow key movement during cell edit.
   // Let it continue editing in next cell if it can.
   // Will be stopped by OnEditStart(..nextcell..) if can't edit next cell.

   // Continue if not in multi-select mode
   int col;
   long row;
   if (this->GetMultiSelectStart(&col, &row))
      return FALSE;
   
   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//	OnTH_LClicked
//		Sent whenever the user clicks the left mouse button within the top heading
//		this message is sent when the button goes down then again when the button goes up
//	Params:
//		col, row	- coordinates of a cell that received mouse click event
//		updn		- is TRUE if mouse button is 'down' and FALSE if button is 'up'
//		processed	- indicates if current event was processed by other control in the grid.
//		rect		- represents the CDC rectangle of cell in question
//		point		- represents the screen point where the mouse event was detected
//	Return:
//		<none>
void CDDBaseGrid::OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	//UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(updn);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);

   // Sort by selected column when user clicks in column heading and
   // did not do a column swap.
   // If the button-up col does not match the button-down col, then a col swap occurred.
   // Column sort can be turned on/off by sbuclass grids. Sort only if on.

   if (m_sortEnabled)
   {
      if (updn == TRUE) // Button Down
      {
         m_THButtonDownCol = col;
      }
      else if (col == m_THButtonDownCol) // Button Up, down & up cols match?
      {
         SortBy(col);
         RedrawAll();
      }
   }
}

int CDDBaseGrid::SortBy(int col)
{
   CString colname = this->QuickGetText(col, -1);
   return SortBy(colname);
}

int CDDBaseGrid::SortBy(CString col)
{
   // The usual sort, if the col is the same as last time then toggle
   // the direction.

   int retval = UG_ERROR;

   if (m_lastSortCol.Compare(col) != 0)
      m_sortAscending = true; // re-init for new col

   m_lastSortCol = col;
   int colnum;
   
   if (GetColFromName(col, &colnum) == UG_SUCCESS)
   {
      int ug_sort_flag = m_sortAscending ? UG_SORT_ASCENDING : UG_SORT_DESCENDING;
      m_sortAscending = !m_sortAscending; // prep for next time, if col doesn't change

      retval = CUGCtrl::SortBy(colnum, ug_sort_flag);
   }

   return retval;
}

int CDDBaseGrid::ReSort(CString defaultcol)
{
   // Re-perform the previous sort. If there was no previous sort then sort
   // by defaultcol.

   if (m_lastSortCol.IsEmpty())
   {
      return SortBy(defaultcol);
   }

   // else
   // Fool the sort direction toggle by toggling back, then
   // do regular sort.
   m_sortAscending = !m_sortAscending;
   return SortBy(m_lastSortCol);
}

/////////////////////////////////////////////////////////////////////////////

void CDDBaseGrid::QuickSetParam(int col, long row, long paramval)
{
   CUGCell cell;
   this->GetCell(col, row, &cell);
   cell.SetParam(paramval);
   this->SetCell(col, row, &cell);
}

long CDDBaseGrid::QuickGetParam(int col, long row)
{
   CUGCell cell;
   this->GetCell(col, row, &cell);
   
   return cell.GetParam();
}

int CDDBaseGrid::QuickSetBool(int col, long row, bool val)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   cell.SetBool(val);
   SetCell(col, row, &cell);

   return UG_SUCCESS;
}

int CDDBaseGrid::QuickSetNumber(int col, long row, int number)
{
   // The standard set number takes double args, and one can set number of
   // decimal places. This convenience func just puts them together in one place.
   CUGCell cell;
   this->GetCell(col, row, &cell);
   cell.SetPropertyFlags( cell.GetPropertyFlags() | UGCELL_DONOT_LOCALIZE );
   cell.SetNumber( (double)number );
   cell.SetNumberDecimals( 0 );
   cell.SetTextColor(colorBlack);
   this->SetCell(col, row, &cell);

   return UG_SUCCESS;
}

int CDDBaseGrid::QuickSetNumber(int col, long row, double number, int decimals)
{
   // This convenience func lets us set the cell value and number of decimal
   // places to display, in one call.

   if (decimals < 0 || decimals > 20 /*arbitrary*/)
      decimals = 3; // also arbitrary

   CUGCell cell;
   this->GetCell(col, row, &cell);
   cell.SetPropertyFlags( cell.GetPropertyFlags() | UGCELL_DONOT_LOCALIZE ); // stop turning period into comma for decimal pt in Gemany
   cell.SetNumber( number );
   cell.SetNumberDecimals( decimals );
   cell.SetTextColor(colorBlack);
   this->SetCell(col, row, &cell);

   return UG_SUCCESS;
}

int CDDBaseGrid::QuickSetTextAndColor(int col, long row, LPCTSTR string, COLORREF color)
{
   // A convenient consolidation of QuickSetText and QuickSetColor, in order to
   // reduce the overall GetCell/SetCell calls, but also to reduce the number
   // of grid func calls in the application.

	///m_cell.ClearAll();
   CUGCell cell;
	GetCell(col, row, &cell);
   cell.SetText(string);
	cell.SetTextColor(color);
	SetCell(col, row, &cell);

	return UG_SUCCESS;
}

int CDDBaseGrid::QuickAddColumn(LPCTSTR columnTitle)
{
   // A convenient way to add a column and set column name.
   // Returns new column index.

   int colCnt = this->GetNumberCols();
   this->SetNumberCols(colCnt+1, FALSE);  // False turns off auto-redraw for added column.

   if (columnTitle != NULL && strlen(columnTitle) > 0)
      QuickSetText(colCnt, -1, columnTitle);

	return colCnt;  // Is now the index of added column
}

//----------------------------------------------------------------------------------

void CDDBaseGrid::SetMultiColHeading(int col1, int row1, int col2, int row2, CString headingStr)
{
   CUGCell cell;

   int res = JoinCells(col1, row1, col2, row2);
	GetCell(col1, row1, &cell);
   cell.SetCellTypeEx(UGCT_NORMALMULTILINE);
   SetCell(col1, row1, &cell);
   QuickSetText(col1, row1, headingStr);
}

//----------------------------------------------------------------------------------

void CDDBaseGrid::SetCellRangeBackColor(int col1, int row1, int col2, int row2, COLORREF backColor)
{
   for (int col = col1; col <= col2; col++)
      for (int row = row1; row <= row2; row++)
         QuickSetBackColor(col, row, backColor);

}



/////////////////////////////////////////////////////////////////////////////
//	OnSortEvaluate
//		Sent as a result of the 'SortBy' call, this is called for each cell
//		comparison and allows for customization of the sorting routines.
//		We provide following code as example of what could be done here,
//		you might have to modify it to give your application customized sorting.
//
//    WHAT'S SPECIAL here is that if one sets the "param" DD_CELLTYPE_REFNAME then
//    we use the "sortableRefDes" style value for sorting.
//
//	Params:
//		cell1, cell2	- pointers to cells that are compared
//		flags			- identifies sort direction
//	Return:
//		value less than zero to identify that the cell1 comes before cell2
//		value equal to zero to identify that the cell1 and cell2 are equal
//		value greater than zero to identify that the cell1 comes after cell2
int CDDBaseGrid::OnSortEvaluate(CUGCell *cell1,CUGCell *cell2,int flags)
{
	// if one of the cells is NULL, do not compare its text
	if ( cell1 == NULL && cell2 == NULL )
		return 0;
	else if ( cell1 == NULL )
		return 1;
	else if ( cell2 == NULL )
		return -1;

	if(flags&UG_SORT_DESCENDING)
	{
		CUGCell *ptr = cell1;
		cell1 = cell2;
		cell2 = ptr;
	}

   CString str1;
   CString str2;

   if (cell1->GetParam() == this->DD_CELLTYPE_REFNAME)
   {
      // Assume if one is from refname col then they both are
      str1 = InsertStruct::getSortableRefDes(cell1->GetText());
      str2 = InsertStruct::getSortableRefDes(cell2->GetText());
   }
   else
   {
      str1 = cell1->GetText();
      str2 = cell2->GetText();
   }

	// initialize variables for numeric check
	double num1, num2;
	// compare the cells, with respect to the cell's datatype
	switch(cell1->GetDataType())
	{
		case UGCELLDATA_STRING:
			//if(NULL == cell1->GetText() && NULL == cell2->GetText())
			//	return 0;
			//if(NULL == cell1->GetText())
			//	return 1;
			//if(NULL == cell2->GetText())
			//	return -1;
			//return _tcscmp(cell1->GetText(),cell2->GetText());
         return str1.CompareNoCase(str2);
		case UGCELLDATA_NUMBER:
		case UGCELLDATA_BOOL:
		case UGCELLDATA_CURRENCY:
			num1 = cell1->GetNumber();
			num2 = cell2->GetNumber();
			if(num1 < num2)
				return -1;
			if(num1 > num2)
				return 1;
			return 0;
		default:
			// if datatype is not recognized, compare cell's text
			//if(NULL == cell1->GetText())
			//	return 1;
			//if(NULL == cell2->GetText())
			//	return -1;
			//return _tcscmp(cell1->GetText(),cell2->GetText());
         return str1.CompareNoCase(str2);
	}
}

int CDDBaseGrid::GetColFromName(LPCTSTR name, int *col)
{
   // Return Values 
   // UG_NA - not available.
   // UG_SUCCESS - success.
   // 1, 2, etc. Error codes (data source dependent).  

   for (int i = 0; i < this->GetNumberCols(); i++)
   {
      CString heading( this->QuickGetText(i, -1) );
      if (heading.CompareNoCase(name) == 0)
      {
         *col = i;
         return UG_SUCCESS;
      }
   }

   return UG_NA;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditVerifyPercentCell
//		This is a handy utility to keep user entry in percent cells valid as they type.
//    The caller should determine that the cell is for percent entry, and if so
//    can pass verification of the input key to this function.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int CDDBaseGrid::OnEditVerifyPercentCell(int col, long row,CWnd *edit,UINT *vcKey)
{
   // Cursor movement keys are okay
   // But these symbols don't work ... ?
   //if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE)
   //   return TRUE;
   if (*vcKey == VK_BACK || *vcKey == 0x7F /*Delete*/ 
      || *vcKey == VK_CANCEL/*Ctrl+C*/ || *vcKey == 0x16/*Ctrl+V*/)
      return TRUE;

   // Current selection will be replaced, so chars in there don't matter, remove them from
   // consideration during validation tests.
   CString curTxt;
   CEdit *cedit = (CEdit*)edit;
   edit->GetWindowText(curTxt);
   int startIndx = -1, endIndx = -1;
   cedit->GetSel(startIndx, endIndx);
   if (startIndx > -1 && endIndx > startIndx)
      curTxt.Delete(startIndx, endIndx - startIndx);

   int curPercentIndx = curTxt.Find("%");

   // Alpha chars '.' and '%' accepted, but only one per customer
   if (*vcKey == '%' || *vcKey == '.')
   {
      // Possibly okay, make sure no duplicate of percent or decimal pt.

      int curDecimalIndx = curTxt.Find(".");

      // Can only have one of these
      if (*vcKey == '%' && curPercentIndx > -1)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }
      if (*vcKey == '.' && curDecimalIndx > -1)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Can't put decimal after percent
      // startIndx is where new char will go
      if (*vcKey == '.' && curPercentIndx > -1 && startIndx > curPercentIndx)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Percent can only go at end
      if (*vcKey == '%' && startIndx < curTxt.GetLength())
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Okay, we'll accept the char
      return TRUE;
   }

   // We're accepting digits, but can't be after percent
   if (isdigit(*vcKey))
   {
      // Can't put digits after percent
      // startIndx is where new char will go
      if (curPercentIndx > -1 && startIndx > curPercentIndx)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Okay
      return TRUE;
   }

   // All else
   MessageBeep((UINT)-1);
   return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
//	OnEditVerifyFloatCell
//		This is a handy utility to keep user entry in floating point number cells valid as they type.
//    The caller should determine that the cell is for percent entry, and if so
//    can pass verification of the input key to this function.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int CDDBaseGrid::OnEditVerifyFloatCell(int col, long row,CWnd *edit,UINT *vcKey)
{
   // Cursor movement keys are okay
   // But these symbols don't work ... ?
   //if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE)
   //   return TRUE;
   if (*vcKey == VK_BACK || *vcKey == 0x7F /*Delete*/)
      return TRUE;

   // Current selection will be replaced, so chars in there don't matter, remove them from
   // consideration during validation tests.
   CString curTxt;
   CEdit *cedit = (CEdit*)edit;
   edit->GetWindowText(curTxt);
   int startIndx = -1, endIndx = -1;
   cedit->GetSel(startIndx, endIndx);
   if (startIndx > -1 && endIndx > startIndx)
      curTxt.Delete(startIndx, endIndx - startIndx);

   int curMinusIndx = curTxt.Find("-");

   // Alpha chars '.' and '-' accepted, but only one per customer
   if (*vcKey == '.' || *vcKey == '-')
   {
      // Possibly okay, make sure no duplicate of percent or decimal pt.

      int curDecimalIndx = curTxt.Find(".");

      // Can only have one of these
      if (*vcKey == '-' && curMinusIndx > -1)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }
      if (*vcKey == '.' && curDecimalIndx > -1)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Can't put decimal before minus
      if (*vcKey == '.' && curMinusIndx > -1 && startIndx <= curMinusIndx)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Minus can only go at beginning
      if (*vcKey == '-' && startIndx != 0)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Okay, we'll accept the char
      return TRUE;
   }

   // We're accepting digits, but not in front of minus sign
   if (isdigit(*vcKey))
   {
      // Can't put digits in front of minus
      if (curMinusIndx > -1 && startIndx <= curMinusIndx)
      {
         MessageBeep((UINT)-1);
         return FALSE;
      }

      // Okay
      return TRUE;
   }

   // All else
   MessageBeep((UINT)-1);
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditVerifyUnsignedIntegerCell
//		This is a handy utility to keep user entry in unsigned integer number cells valid as they type.
//    The caller should determine that the cell is for percent entry, and if so
//    can pass verification of the input key to this function.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int CDDBaseGrid::OnEditVerifyUnsignedIntegerCell(int col, long row,CWnd *edit,UINT *vcKey)
{
   // Cursor movement keys are okay
   // But these symbols don't work ... ?
   //if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE)
   //   return TRUE;
   if (*vcKey == VK_BACK || *vcKey == 0x7F /*Delete*/)
      return TRUE;

   // We're accepting digits, and that's all
   if (isdigit(*vcKey))
   {
      // Okay
      return TRUE;
   }

   // All else
   MessageBeep((UINT)-1);
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCanColSwap
//		Called just before col swap to see if it is allowed.
//	Params:
//		fromCol - where the col originated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		TRUE if allowed, otherwise FALSE.
BOOL CDDBaseGrid::OnCanColSwap(int fromCol,int toCol)
{
   // Swap is allowed if columnDefMap is set up.
   // Except oops, that breaks DD that are not converted.
   //*rcf bug fix this

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnColSwapped
//		Called just after column-swap operation was completed.
//	Params:
//		fromCol - where the col originated from
//		toCol	- where the col will be located if the swap is allowed
//	Return:
//		<none>
void CDDBaseGrid::OnColSwapped(int fromCol,int toCol)
{
	UNREFERENCED_PARAMETER(fromCol);
	UNREFERENCED_PARAMETER(toCol);

   // This does the sticky settings stuff.
   this->m_columnDefMap.Update(this);

}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

CDDSpecialColumn* CDDSpecialColumnMap::Add(CString columnName)
{
   if (!columnName.IsEmpty())
   {
      LPCSTR existingkey;
      if (!this->LookupKey(columnName, existingkey))
      {
         CDDSpecialColumn *newcolumn = new CDDSpecialColumn(columnName);
         this->SetAt(columnName, newcolumn);
         return newcolumn;
      }
   }

   return NULL;
}

CDDSpecialColumn* CDDSpecialColumnMap::GetSpecialColumn(int colIndx)
{
   // If colIndx is index to a special column then get it, otherwise return NULL

   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      CString columnName;
      CDDSpecialColumn *column;
      this->GetNextAssoc(pos, columnName, column);
      if (column->GetColumnIndexRef() == colIndx)
         return column;
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
*/
void CColumnDefMap::AddCustomAttributes(CDDSpecialColumnMap &customColumnMapToConfig, CDataDoctorLibraryAttribMap &customAttMapFromUser)
{
   customColumnMapToConfig.empty();

   //IN arg CDataDoctorLibraryAttribMap &attmap = this->getParent().getDataDoctor().GetLibraryAttribMap();

   CDataDoctorLibraryAttrib *libattr = NULL;
   CString* attrname;

   for (customAttMapFromUser.GetFirstSorted(attrname, libattr); libattr != NULL; customAttMapFromUser.GetNextSorted(attrname, libattr))
   {
      if (libattr->IsActive())
      {
         CDDSpecialColumn *column = customColumnMapToConfig.Add(libattr->GetName());
         if (column != NULL)
            this->AddColDef(column->GetName(), &column->GetColumnIndexRef());
      }
   }
}


void CColumnDefMap::Update(CDDBaseGrid *grid)
{
   if (grid != NULL)
   {
      for (int i = 0; i < this->getSize(); i++)
      {
         CColumnDef *cd = this->getAt(i);
         CString title = cd->GetTitle();
         int *ptr = cd->GetColIndexPtr();

         int col;
         if (grid->GetColFromName(title, &col) == UG_SUCCESS)
            *ptr = col;
         else
            ErrorMessage("Error in column swap.", "");
      }
   }

   SaveSettings();
}

void CColumnDefMap::SaveSettings()
{
   // Write column positions to registry
   // ColumnDefMap must have non-blank name and one (1) or more entries.
   // If no name or no entries then skip it.

   if (!GetName().IsEmpty() && this->getSize() > 0)
   {
      CAppRegistryKey appKey;
      CRegistryKey settings = appKey.createSubKey("Settings");

      if (settings.isOpen())
      {
         CRegistryKey registryKey = settings.createSubKey( GetName() );  // Grid Name

         if (registryKey.isOpen())
         {
            for (int i = 0; i < this->getSize(); i++)
            {
               CColumnDef *cd = this->getAt(i);
               CString title = cd->GetTitle();     // Column Name
               int *ptr = cd->GetColIndexPtr();
               int value = *ptr;

               if (registryKey.setValue(title, value))
               {
                  // okay
               }
               // else we silently suffer
            }
         }
      }
   }
}

void CColumnDefMap::LoadSettings()
{
   if (!GetName().IsEmpty())
   {
      CAppRegistryKey appKey;
      CRegistryKey settings = appKey.createSubKey("Settings");

      if (settings.isOpen())
      {
         CRegistryKey registryKey = settings.createSubKey( GetName() );

         if (registryKey.isOpen())
         { 
            for (int i = 0; i < this->getSize(); i++)
            {
               CColumnDef *cd = this->getAt(i);
               CString title = cd->GetTitle();     // Column Name
               int *ptr = cd->GetColIndexPtr();
               int value;

               if (registryKey.getValue(title, value))
               {
                  *ptr = value;
               }
            }

            // Compensate
            // The number of fields present may have been reduced. This could
            // result in a hole in the numbering of active fields, as well as a column
            // number that is out of range for current list. Try to spot this and
            // if spotted then shuffle the fields down so we might preserve the
            // order for the rest of the fields. Failing this, teh Validate section
            // will just reassign everything to defaults.
            ///*rcf WELL ???

            // Validate
            // The values are column indices, nothing should be less
            // than zero, and not greater than the number of columns
            // minus 1. And no two should be alike. If there is an
            // error, then just reset the whole thing to sequentially
            // numbered.
            bool error = false;

            for (int i = 0; i < this->getSize() && !error; i++)
            {
               CColumnDef *cdI = this->getAt(i);
               int *valI = cdI->GetColIndexPtr();

               if (*valI < 0 || *valI >= this->getSize())
                  error = true;

               // Check no two alike
               for (int j = i+1; j < this->getSize() && !error; j++)
               {
                  CColumnDef *cdJ = this->getAt(j);
                  int *valJ = cdJ->GetColIndexPtr();

                  if (*valI == *valJ)
                     error = true;
               }
            }

            // If error then reset to sequential numbering
            if (error)
            {
               for (int i = 0; i < this->getSize(); i++)
               {
                  CColumnDef *cd = this->getAt(i);
                  int *valI = cd->GetColIndexPtr();
                  *valI = i;
               }
            }


         }
      }
   }
}

