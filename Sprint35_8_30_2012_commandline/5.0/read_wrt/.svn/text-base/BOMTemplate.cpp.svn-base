
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// BOMTemplate.cpp


#include "stdafx.h"
#include "InFile.h"
#include "BOMTemplate.h"
#include ".\BOMTemplate.h"
#include "DirDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CBOMTemplateData 
/////////////////////////////////////////////////////////////////////////////
static char *delimiterTable[] = {DEL_TAB,DEL_SPACE,DEL_COMMA,DEL_PIPE,DEL_SEMICOLON,DEL_COLUMN,DEL_OTHERS};
CBOMTemplateData::CBOMTemplateData()
: m_TemplateName(QTEMPLATENAME)
, m_StartLine(2)
, m_HeadLine(0)
, m_ColumnNum(1)
, m_StartCharIdx(1)
, m_EndCharIdx(2)
, m_CommontChar("~")
, m_Delimiter(DEL_COLUMN)
, m_TemplateFileName(TEMPLATE_FILENAME)
, m_AttributeName("")
{
   m_TemplateFolderPath = DEFAULT_TEMPLATE_FOLDER;
   m_columnAttribute.RemoveAll(); 

   for(int i = 0; i < sizeof(delimiterTable)/sizeof(char*); i++)
      m_DelimiterTable.SetAt(delimiterTable[i],DelimiterStringToChar(delimiterTable[i]));
}

CBOMTemplateData::~CBOMTemplateData()
{
   ClearAttributes();
}

void CBOMTemplateData::ResetTemplateData()
{
   m_StartLine = 2;
   m_HeadLine = 0;
   m_ColumnNum = 1;
   m_StartCharIdx = 1;
   m_EndCharIdx = 2;
   m_CommontChar = "~";
   m_Delimiter = DEL_COLUMN;
   m_TemplateName = QTEMPLATENAME;
   m_AttributeName = "";

   ClearAttributes();

}

char CBOMTemplateData::getDelimiterChar()
{
   return DelimiterStringToChar(m_Delimiter);
}

char CBOMTemplateData::DelimiterStringToChar(CString DelimiterStr)
{
   if(DelimiterStr.IsEmpty())
      return DELCODE_UNKNOWN;

   switch(DelimiterStr.GetAt(0))
   {
   case 'C':
      if(DelimiterStr.Find(DEL_COLUMN) != -1)
         return DELCODE_COLUMN;
      else if(DelimiterStr.Find(DEL_COMMA) != -1)
         return DELCODE_COMMA;
      break;
   case 'P':
      return DELCODE_PIPE;
   case 'S':
      if(DelimiterStr.Find(DEL_SPACE) != -1)
         return DELCODE_SPACE;
      else if(DelimiterStr.Find(DEL_SEMICOLON) != -1)
         return DELCODE_SEMICOLON;
      break;
   case 'T':
      return DELCODE_TAB;
   }
   return DelimiterStr.GetAt(0);
}

bool CBOMTemplateData::LoadTemplateFile(CString &TemplateFileName)
{
   CInFile inFile;

   if (!TemplateFileName.IsEmpty() && ! inFile.open(TemplateFileName))
   {
      formatMessageBox("Could not open .in file '%s'\n",TemplateFileName);

      return false;
   }

   ClearAttributes();
   while (inFile.getNextCommandLine('='))
   {
      if      (inFile.isCommand(TTAG_STARTLINE    ,2))  m_StartLine = inFile.getIntParam(1);
      else if (inFile.isCommand(TKEY_TEMPLATENAME ,2))  m_TemplateName = inFile.getParam(1); 
      else if (inFile.isCommand(TTAG_COMMENTCHAR  ,2))  m_CommontChar = inFile.getParam(1);
      else if (inFile.isCommand(TTAG_DELIMITER    ,2))  m_Delimiter = inFile.getParam(1); 
      else if (inFile.isCommand(TTAG_COLUMN       ,5)) 
      {
         ColumnAttribute *columnVal = new ColumnAttribute;
         if(columnVal)
         {
            columnVal->columnNum       = inFile.getIntParam(1);
            columnVal->AttributeName   = inFile.getParam(2);
            columnVal->startcharIdx    = inFile.getIntParam(3);
            columnVal->endcharIdx      = inFile.getIntParam(4);            
            m_columnAttribute.Add(columnVal);
         }
      }
      else if (inFile.isCommand(TTAG_COLUMN       ,3))
      {
         ColumnAttribute *columnVal = new ColumnAttribute;
         if(columnVal)
         {
            columnVal->columnNum    = inFile.getIntParam(1);
            columnVal->AttributeName      = inFile.getParam(2);
            columnVal->startcharIdx = 0;
            columnVal->endcharIdx = 0;
            SortedAdd(m_columnAttribute,columnVal);
         }
      }
   }
   inFile.close();
   
   return true;
}

bool CBOMTemplateData::SaveTemplateFile(CString &TemplateFileName)
{
   CStdioFileWriteFormat stdioFileWriteFormat;
   if (!stdioFileWriteFormat.open(TemplateFileName))
   {
      formatMessageBox("Could not open .in file '%s'\n",TemplateFileName);

      return false;
   }

   stdioFileWriteFormat.writef("%s=%s\n",TKEY_TEMPLATENAME,    m_TemplateName);   
   stdioFileWriteFormat.writef("%s=%d\n",TTAG_STARTLINE,       m_StartLine);   
   stdioFileWriteFormat.writef("%s=%s\n",TTAG_COMMENTCHAR,     m_CommontChar);   
   stdioFileWriteFormat.writef("%s=%s\n",TTAG_DELIMITER,       m_Delimiter);   

   //Write column
   for(int i = 0 ; i < m_columnAttribute.GetCount(); i++)
   {
      ColumnAttribute *columnVal = m_columnAttribute.GetAt(i);
      if(columnVal)
      {
         if(m_Delimiter.Find(DEL_COLUMN) != -1)
            stdioFileWriteFormat.writef("%s %i %s %i %i\n",TTAG_COLUMN, columnVal->columnNum,columnVal->AttributeName,
               columnVal->startcharIdx,columnVal->endcharIdx);  
         else
            stdioFileWriteFormat.writef("%s %i %s\n",TTAG_COLUMN, columnVal->columnNum,columnVal->AttributeName);  
      }         
   }
   stdioFileWriteFormat.close(); 

   return true;
}

void CBOMTemplateData::ClearAttributes()
{
   if(m_columnAttribute.GetCount())
   {
      for(int i = 0; i < m_columnAttribute.GetCount(); i++)
      {
         ColumnAttribute* attribute = m_columnAttribute.GetAt(i);
         if(attribute)
            delete attribute;
      }
      m_columnAttribute.RemoveAll();
   }
}

int CBOMTemplateData::AddNewAttribute(ColumnAttribute newparam)
{
   ColumnAttribute *columnVal = new ColumnAttribute;
   if(!columnVal)
      return -1;

   columnVal->columnNum    = newparam.columnNum;
   columnVal->AttributeName      = newparam.AttributeName;
   columnVal->startcharIdx = newparam.startcharIdx;
   columnVal->endcharIdx   = newparam.endcharIdx;            
   SortedAdd(m_columnAttribute,columnVal);
     
   return m_columnAttribute.GetCount() - 1;
}

void CBOMTemplateData::SortedAdd(ColumnAttributeArray &columnAttribute, ColumnAttribute *columnVal)
{
   int size = columnAttribute.GetCount();
   int idx = 0;

   for(idx = 0; idx < size; idx++)
   {
      ColumnAttribute *curAttr = columnAttribute.GetAt(idx);
      if(curAttr->columnNum > columnVal->columnNum)
         break;
   }
   if(idx < size)
      columnAttribute.InsertAt(idx,columnVal);
   else
      columnAttribute.Add(columnVal);
}

int CBOMTemplateData::ValidateHeaderData()
{
   int errorCode = Err_OK;
   if(m_TemplateName.IsEmpty())
      errorCode = Err_TemplateName_Value;
   else if(m_CommontChar.IsEmpty())
      errorCode = Err_CommentChar_Value;  
   else if(m_Delimiter.IsEmpty())
      errorCode = Err_Delimiter_Value;  

   return errorCode;
}

int CBOMTemplateData::ValidateInputAttribute(int exclrow, int columnNo, CString AttrName, int startIdx, int endIdx)
{
   int errorCode = Err_OK;

   if(AttrName.IsEmpty())
      errorCode = Err_AttrName_Value;
   else if(startIdx > endIdx || startIdx <= 0 || endIdx <= 0)
      errorCode = Err_Range_Value;
   else if(columnNo <= 0)
      errorCode = Err_ColumnNo_Value;

   for(int row = 0; row < m_columnAttribute.GetCount() && !errorCode; row++)
   {
      if(row != exclrow)
      {
         ColumnAttribute *attribute = m_columnAttribute.GetAt(row);
         
         if(columnNo == attribute->columnNum)
            errorCode = Err_ColumnNo_Duplicate;
         else if(!AttrName.Compare(attribute->AttributeName))
            errorCode = Err_AttrName_Duplicate;
         if(startIdx >= attribute->startcharIdx && startIdx <= attribute->endcharIdx)
            errorCode = Err_Range_Value;
         else if(endIdx >= attribute->startcharIdx && endIdx <= attribute->endcharIdx)
            errorCode = Err_Range_Value;
         else if(startIdx <= attribute->startcharIdx && endIdx >= attribute->endcharIdx)
            errorCode = Err_Range_Value;
      }
   }

   return errorCode;
}

void CBOMTemplateData::ErrorMessage(int errorCode)
{
   CString ErrorMsg = "";

   switch(errorCode)
   {
   case Err_AttrName_Value:
      ErrorMsg = "Attribute Name can't be empty!";
      break;
   case Err_AttrName_Duplicate:
      ErrorMsg = "Attribute Name can't be duplicate!";
      break;
   case Err_ColumnNo_Value:
      ErrorMsg = "Column number can't be smaller than zero!!";
      break;
   case Err_ColumnNo_Duplicate:
      ErrorMsg = "Column number can't be duplicate!!";
      break;
   case Err_Range_Value:
      ErrorMsg = "Range is not correct!!";
      break;
   case Err_TemplateName_Value:
      ErrorMsg = "Template Name can't be empty!";
      break;
   case Err_CommentChar_Value:
      ErrorMsg = "Comment Char can't be empty!";
      break;
   case Err_Delimiter_Value:
      ErrorMsg = "Delimiter can't be empty!";
      break;
   case Err_StartLine_Value:
      ErrorMsg = "Start line can't be empty!";
      break;
   }

   formatMessageBox(ErrorMsg);
}

CBOMTemplateData &CBOMTemplateData::operator=(CBOMTemplateData &other)
{
   if(this != &other)
   {
      m_Delimiter = other.getDelimiter();
      m_CommontChar = other.getCommontChars();
      m_TemplateName = other.getTemplateName();
      m_TemplateFileName = other.getTemplateFileName();
      m_TemplateFolderPath = other.getTemplateFolderPath();
      m_StartLine = other.getStartLine();

      if(m_columnAttribute.GetCount())
         ClearAttributes();

      int tablesize = other.getColumnAttriButeList().GetCount();
      for(int i = 0; i < tablesize; i++)
      {
         ColumnAttribute *otherAttr = other.getColumnAttriButeList().GetAt(i);
         ColumnAttribute *curAttr = new ColumnAttribute;

         curAttr->AttributeName = otherAttr->AttributeName;
         curAttr->columnNum = otherAttr->columnNum;
         curAttr->startcharIdx = otherAttr->startcharIdx;
         curAttr->endcharIdx = otherAttr->endcharIdx;
         m_columnAttribute.Add(curAttr);
      }      
   }

   return *this;
}

bool CBOMTemplateData::Compare(CBOMTemplateData &other)
{
   bool retval = true;

   if(this != &other)
   {
      if(m_Delimiter.Compare(other.getDelimiter()))
         retval = false;
      else if(m_CommontChar.Compare(other.getCommontChars()))
         retval = false;
      else if(m_TemplateName.Compare(other.getTemplateName()))
         retval = false;
      else if(m_StartLine != other.getStartLine())
         retval = false;
      else if(m_columnAttribute.GetCount() != other.getColumnAttriButeList().GetCount())
         retval = false;
      /*else if (CompareAttributes(other.getColumnAttriButeList()))
         retval = false;*/
   }
   return !(retval);
}

bool CBOMTemplateData::CompareAttributes(ColumnAttributeArray &other)
{
   bool retval = true;

   int tablesize = m_columnAttribute.GetCount();
   for(int i = 0; i < tablesize && retval; i++)
   {
      ColumnAttribute *otherAttr = other.GetAt(i);
      ColumnAttribute *curAttr = m_columnAttribute.GetAt(i);
      retval = (!curAttr->AttributeName.Compare(otherAttr->AttributeName))?
               ((curAttr->columnNum == otherAttr->columnNum)?
               ((curAttr->startcharIdx == otherAttr->startcharIdx)?
               (curAttr->endcharIdx == otherAttr->endcharIdx):false):false):false;
   }

   return !(retval);
}

/////////////////////////////////////////////////////////////////////////////
// CBOMTemplateDirectory dialog
/////////////////////////////////////////////////////////////////////////////
bool CBOMTemplateDirectory::FindTemplateRegValue(CString &Value)
{  

   if(m_cadDoc == NULL)
      return false;

   //Search CAMCAD Profile Settings
   Value = m_cadDoc->GetProfileString("Settings", KEY_TEMPLATEDIR, "");
   if(Value.IsEmpty())
   {
      if(FindStringValueByKey(KEY_SOFTWARE + (CString)KEY_VBPROGRAM + (CString)KEY_BOMEXPLORER, KEY_TEMPLATEDIR, Value)
         && !Value.IsEmpty())
      {
          // Search Default BOM Explorer 

      }
      else if(FindStringValueByKey(KEY_SOFTWARE + (CString)REGISTRY_COMPANY + (CString)KEY_CAMCADSETTING, KEY_TEMPLATEDIR, Value) 
         && !Value.IsEmpty())
      {
         // Search CAMCAD Settings
      }
      else
      {
         // Search for Directory
         Value = m_cadDoc->GetProfileString("Settings", "Directory", DEFAULT_TEMPLATE_FOLDER);
         if(Value.IsEmpty())
         {
            Value = DEFAULT_TEMPLATE_FOLDER;
         }      
      }

      //Normalize Value
      CString NormailizeVal = Value.Mid(0,1).MakeUpper();
      Value.SetAt(0,NormailizeVal.GetAt(0));
      m_cadDoc->WriteProfileString("Settings", KEY_TEMPLATEDIR, Value);
   }

   return true;
}

void CBOMTemplateDirectory::SetTemplateDirectory(CString Value)
{
   if(m_cadDoc)
      m_cadDoc->WriteProfileString("Settings", KEY_TEMPLATEDIR, Value);
}

bool CBOMTemplateDirectory::FindStringValueByKey(CString keyname, CString RegName,CString &strValue)
{
   HRESULT res;
   HKEY settingsKey;

   strValue.Empty();
   res = RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_READ, &settingsKey);
   if (res == S_OK)
   {  
      DWORD dwCount, dwType;
      LONG lResult = RegQueryValueEx(settingsKey, RegName, NULL, &dwType,
			NULL, &dwCount);        
      if (res == S_OK)
      {
         res = RegQueryValueEx(settingsKey, RegName, NULL, &dwType, 
            (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);   

         strValue.ReleaseBuffer();
         RegCloseKey(settingsKey);
         return true;
      }
   }
   RegCloseKey(settingsKey);

   return false;
}

/////////////////////////////////////////////////////////////////////////////
// CBOMTemplateGrid
/////////////////////////////////////////////////////////////////////////////
CBOMTemplateGrid::CBOMTemplateGrid()
{
   m_ColumnSize = 0;
   m_RowSize = 0;
   
   m_gridHeader.RemoveAll();
   m_gridHeader.Add(QSELECT);
   m_gridHeader.Add(QCOLNUMS);
   m_gridHeader.Add(QATTRNAME);
   m_gridHeader.Add(QSTARTAT);
   m_gridHeader.Add(QENDAT);
}

CBOMTemplateGrid::~CBOMTemplateGrid()
{
   m_gridHeader.RemoveAll();
}

//------------------------------------------------------------------
// CBOMTemplateGrid: Edit fields in Grid
//------------------------------------------------------------------
void CBOMTemplateGrid::initGrid(ColumnType colStype)
{
   m_DefalutType =  colStype;
   if (GetNumberCols() < m_gridHeader.GetCount())
   {     
      int defaultColSize = m_gridHeader.GetCount();
      SetSH_Width(0);
      SetNumberRows(0,false);
      SetNumberCols(defaultColSize,false);

      for(int idx = 0; idx < defaultColSize; idx++)
      {
         if( m_gridHeader.GetAt(idx))
            QuickSetText(idx, -1, m_gridHeader.GetAt(idx));
      }
   }

   SetCellOption(T_Select,UGCT_CHECKBOX);
   SetNumberRows(0);
   ReColSize(colStype);
}

void CBOMTemplateGrid::ReColSize(ColumnType colStype)
{
    if(colStype == T_NoRange)
      m_ColumnSize = 3;
   else if(colStype == T_HasRange)
      m_ColumnSize = 5;

   SetNumberCols(m_ColumnSize);  

   //Extending Column needs to be initialized
   if(m_DefalutType == T_NoRange && colStype == T_HasRange)
   {
      for(int row = 0 ; row< GetNumberRows(); row++)
      {
         CString data;
         int base = 2;
         data.Format("%i",(row * base) + 1);
         QuickSetText(T_StartsAt, row, data);     
         data.Format("%i",(row + 1) * base);
         QuickSetText(T_EndsAt, row, data); 
      }
   }

   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
   m_DefalutType = colStype;
}

void CBOMTemplateGrid::SetCellOption(int col, int option)
{
   //First Column is a checkbox
   CUGCell cell;
   GetColDefault(col, &cell); 
   cell.SetCellType(option); 
   SetColDefault(col, &cell);
}

int CBOMTemplateGrid::getSelectValue(int row)
{
   CUGCell cell;
   int num = false;

   GetCell(T_Select,row,&cell);
   cell.GetNumber(&num);

   return num;
}

void CBOMTemplateGrid::AddRowData(ColumnAttribute *attribute)
{
   int row = GetNumberRows();
   if(attribute)
   {
      CString data="";
      data.Format("%i",attribute->columnNum);
      QuickSetText(T_ColumnIndex, row, data);     
      QuickSetText(T_AttributeName, row, attribute->AttributeName); 
      
      if(m_DefalutType == T_HasRange)
      {
         data.Format("%i",attribute->startcharIdx);
         QuickSetText(T_StartsAt, row, data);     

         data.Format("%i",attribute->endcharIdx);
         QuickSetText(T_EndsAt, row, data); 
      }
      BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
   }
}

void CBOMTemplateGrid::AddRowData(int TemplateColumnIdx, CString AttrName)
{
   int row = GetNumberRows();
   if(row >= 0)
   {
      SetNumberRows(row + 1);
      
      CString data="";
      data.Format("%i",TemplateColumnIdx);
      QuickSetText(T_ColumnIndex, row, data);     
      QuickSetText(T_AttributeName, row, AttrName);      
   
      BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
   }
}

void CBOMTemplateGrid::AddRowData(int TemplateColumnIdx, CString AttrName, int StartAt, int EndAt)
{
   int row = GetNumberRows();
   if(row >= 0)
   {
      SetNumberRows(row + 1);
      
      CString data="";
      data.Format("%i",TemplateColumnIdx);
      QuickSetText(T_ColumnIndex, row, data);
      
      QuickSetText(T_AttributeName, row, AttrName);
      
      data.Format("%i",StartAt);
      QuickSetText(T_StartsAt, row, data);
      
      data.Format("%i",EndAt);
      QuickSetText(T_EndsAt, row, data);
   
      BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
   }
}


bool CBOMTemplateGrid::GetRowData(int row, ColumnAttribute *attribute)
{
   if(row >= GetNumberRows())
      return false;

   attribute->columnNum = (QuickGetText(T_ColumnIndex, row))?atoi(QuickGetText(T_ColumnIndex, row)):0;
   attribute->AttributeName = QuickGetText(T_AttributeName, row);
   
   if(m_DefalutType == T_HasRange)
   {
      attribute->startcharIdx = (QuickGetText(T_StartsAt, row))?atoi(QuickGetText(T_StartsAt, row)):0;
      attribute->endcharIdx = (QuickGetText(T_EndsAt, row))?atoi(QuickGetText(T_EndsAt, row)):0;
   }

   return true;
}

bool CBOMTemplateGrid::GetRowData(int row, int &TemplateColumnIdx, CString &AttrName)
{
   if(row < GetNumberRows())
   {
      TemplateColumnIdx = atoi(QuickGetText(T_ColumnIndex, row));
      AttrName = QuickGetText(T_AttributeName, row);
      return true;
   }   

   return false;
}

bool CBOMTemplateGrid::GetRowData(int row, int &TemplateColumnIdx, CString &AttrName, int &StartAt, int &EndAt)
{
   if(row < GetNumberRows())
   {
      TemplateColumnIdx = (QuickGetText(T_EndsAt, row))?atoi(QuickGetText(T_ColumnIndex, row)):0;
      AttrName = QuickGetText(T_AttributeName, row);
      StartAt = (QuickGetText(T_StartsAt, row))?atoi(QuickGetText(T_StartsAt, row)):0;
      EndAt = (QuickGetText(T_EndsAt, row))?atoi(QuickGetText(T_EndsAt, row)):0;

      return true;
   }   

   return false;
}

void CBOMTemplateGrid::DeleteRowBySelectValue()
{
   int row = 0;

   //delete all data with TemplateColumnIdx
   while(row < GetNumberRows())
   {
      if(getSelectValue(row))
         DeleteRow(row);
      else
         row++;
   }
   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CBOMTemplateGrid::ClearAllData()
{
   int rowNum = this->GetNumberRows();
   
   while(rowNum > 0)
   {      
      DeleteRow(0);
      rowNum --;
   }
}

int CBOMTemplateGrid::ValidateInputAttribute(int exclrow, int columnNo, CString AttrName, int startIdx, int endIdx)
{
   int errorCode = Err_OK;

   if(AttrName.IsEmpty())
      errorCode = Err_AttrName_Value;
   else if((m_DefalutType == T_HasRange) && (startIdx > endIdx || startIdx <= 0 || endIdx <= 0))
      errorCode = Err_Range_Value;
   else if(columnNo <= 0)
      errorCode = Err_ColumnNo_Value;

   for(int row = 0; row < GetNumberRows() && !errorCode; row++)
   {
      if(row != exclrow)
      {
         ColumnAttribute attribute;
         GetRowData(row,&attribute);
         if(columnNo == attribute.columnNum)
            errorCode = Err_ColumnNo_Duplicate;
         else if(!AttrName.Compare(attribute.AttributeName))
            errorCode = Err_AttrName_Duplicate;
         if(m_DefalutType == T_HasRange && startIdx && endIdx)
         {
            if(startIdx >= attribute.startcharIdx && startIdx <= attribute.endcharIdx)
               errorCode = Err_Range_Value;
            else if(endIdx >= attribute.startcharIdx && endIdx <= attribute.endcharIdx)
               errorCode = Err_Range_Value;
            else if(startIdx <= attribute.startcharIdx && endIdx >= attribute.endcharIdx)
               errorCode = Err_Range_Value;
         }
      }
   }

   return errorCode;
}

int CBOMTemplateGrid::ValidateInputAttribute(int exclrow, ColumnAttribute validattr)
{   
   return ValidateInputAttribute(exclrow,validattr.columnNum,validattr.AttributeName,
      validattr.startcharIdx,validattr.endcharIdx);
}

bool CBOMTemplateGrid::CompareAttributes(ColumnAttributeArray &others)
{
   bool retval = true;

   int tablesize = GetNumberRows();
   if(tablesize != others.GetCount())
      retval = false;

   for(int row = 0; row < tablesize && retval; row++)
   {
      ColumnAttribute *otherAttr = others.GetAt(row);
      ColumnAttribute curAttr;     
      GetRowData(row, &curAttr);
      if((m_DefalutType == T_HasRange))
      {
         retval = (!curAttr.AttributeName.Compare(otherAttr->AttributeName))?
                  ((curAttr.columnNum == otherAttr->columnNum)?
                  ((curAttr.startcharIdx == otherAttr->startcharIdx)?
                  (curAttr.endcharIdx == otherAttr->endcharIdx):false):false):false;
      }
      else
      { 
          retval = (!curAttr.AttributeName.Compare(otherAttr->AttributeName))?
                   (curAttr.columnNum == otherAttr->columnNum):false;
      }
   }

   return !(retval);
}

//------------------------------------------------------------------
// CBOMTemplateGrid: functions to process event 
//------------------------------------------------------------------
int CBOMTemplateGrid::OnEditVerifyDigitalCell(int col, long row,CWnd *edit,UINT *vcKey)
{
   // Cursor movement keys are okay
   //if (*vcKey == VK_BACK || *vcKey == VK_LEFT || *vcKey == VK_RIGHT || *vcKey == VK_DELETE)
   //   return TRUE;
   if (*vcKey == VK_BACK || *vcKey == 0x7F /*Delete*/ 
      || *vcKey == VK_CANCEL/*Ctrl+C*/ || *vcKey == 0x16/*Ctrl+V*/)
      return TRUE;

   // Check digital number
   return isdigit(*vcKey);
}

int CBOMTemplateGrid::OnEditVerifyAttributeCell(int col, long row,CWnd *edit)
{
   CString curTxt, restoreText;
   
   CEdit *cedit = (CEdit*)edit;
   edit->GetWindowText(curTxt);

   ColumnAttribute attribute;
   GetRowData(row,&attribute);

   switch(col)
   {
   case T_ColumnIndex:
      restoreText.Format("%i",attribute.columnNum);
      attribute.columnNum = atoi(curTxt.GetBuffer());
      break;
   case T_AttributeName:
      restoreText = attribute.AttributeName ;
      attribute.AttributeName = curTxt;
      break;
   case T_StartsAt:
      restoreText.Format("%i",attribute.startcharIdx);
      attribute.startcharIdx = atoi(curTxt.GetBuffer());
      break;
   case T_EndsAt:
      restoreText.Format("%i",attribute.endcharIdx);
      attribute.endcharIdx = atoi(curTxt.GetBuffer());
      break;
   }   
   curTxt.ReleaseBuffer();
   
   int errorCode = ValidateInputAttribute(row, attribute);   

   if(errorCode)
   {
      CBOMTemplateData t_data;
      t_data.ErrorMessage(errorCode);
      edit->SetWindowText(restoreText);
   }

   return errorCode;
}

//------------------------------------------------------------------
// CBOMTemplateGrid: extended Grid Control event
//------------------------------------------------------------------
void CBOMTemplateGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320341/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;	

}

void CBOMTemplateGrid::OnCharDown(UINT* vcKey,BOOL processed)
{
   // start editing when the user hits a character key on a cell in the grid.  
   // Pass that key to the edit control, so it doesn't get lost
   StartEdit(*vcKey); 
} 


int CBOMTemplateGrid::OnEditStart(int col, long row, CWnd **edit)
{
   *edit = &m_myCUGEdit;
   return true;
}

int CBOMTemplateGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
   switch(col)
   {
   case T_ColumnIndex: //only allow digital number
   case T_StartsAt:
   case T_EndsAt:
      return OnEditVerifyDigitalCell(col, row, edit, vcKey);  
   } 

   // All others
	return TRUE;
}

int CBOMTemplateGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);
     
   return (OnEditVerifyAttributeCell(col,row,edit))?FALSE:TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CApplyBOMTemplateDlg dialog
/////////////////////////////////////////////////////////////////////////////
CApplyBOMTemplateDlg::CApplyBOMTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CApplyBOMTemplateDlg::IDD, pParent)
{
}

CApplyBOMTemplateDlg::~CApplyBOMTemplateDlg()
{
}

void CApplyBOMTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CApplyBOMTemplateDlg)
   DDX_Control(pDX, IDC_LIST_TEMPLATEFILES, m_fileList);
   DDX_Control(pDX, IDC_COMBO_COLUMNS, m_columnCB);
   DDX_Text(pDX, IDC_EDIT_STARTLINE, m_templateData.getStartLine());   
   DDX_Text(pDX, IDC_STATIC_TEMPNAME, m_templateData.getTemplateName());   
   DDX_Text(pDX, IDC_EDIT_ATTRIBUTENAME, m_templateData.getAttributeName());   
   DDX_Text(pDX, IDC_EDIT_STARTCHARIDX, m_templateData.getStartCharIndex());   
   DDX_Text(pDX, IDC_EDIT_ENDCHARIDX, m_templateData.getEndCharIndex()); 
   DDX_Text(pDX, IDC_TEMPLATE_DELIMITERS, m_templateData.getDelimiter());
   DDX_Text(pDX, IDC_TEMPLATE_FOLDER, m_templateData.getTemplateFolderPath());
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CApplyBOMTemplateDlg, CDialog)
  //{{AFX_DATA_MAP(CApplyBOMTemplateDlg)
   ON_CBN_SELCHANGE(IDC_COMBO_COLUMNS, OnCbSelchangeColumnNumber)
   ON_CBN_SELCHANGE(IDC_LIST_TEMPLATEFILES, onListSelectTemplateFile)
   ON_BN_CLICKED(IDC_SET_COLUMNS, OnCbSetColumnNumber)
   ON_BN_CLICKED(IDOK, OnBntCheckNext)
   ON_EN_KILLFOCUS(IDC_EDIT_STARTLINE, OnEnKillfocusEditStartline)
   ON_EN_KILLFOCUS(IDC_EDIT_STARTCHARIDX, OnEnKillfocusEditStartcharidx)
   ON_EN_KILLFOCUS(IDC_EDIT_ENDCHARIDX, OnEnKillfocusEditEndcharidx)
   ON_EN_KILLFOCUS(IDC_EDIT_ATTRIBUTENAME, OnEnKillfocusEditAttributename)
      //}}AFX_DATA_MAP
END_MESSAGE_MAP()

BOOL CApplyBOMTemplateDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();  
 
   //Initialize Template Folder
   m_Templatedir.FindTemplateRegValue(m_templateData.getTemplateFolderPath());

   //Initialize Components
   FillTemplateFileList();   
   LoadTemplateFile(0);
   EnableAttributeRange();

   UpdateData(FALSE);
   return TRUE;
}

void CApplyBOMTemplateDlg::OnCbSelchangeColumnNumber()
{
   UpdateData(TRUE);

   ColumnAttribute *columnVal = NULL;
   int Index = m_columnCB.GetCurSel();

   if(Index != -1)
      columnVal = (ColumnAttribute *)m_columnCB.GetItemData(Index);

   if(columnVal)
   {
      m_templateData.setAttributeName(columnVal->AttributeName);
      m_templateData.setStartCharIndex(columnVal->startcharIdx);
      m_templateData.setEndCharIndex(columnVal->endcharIdx);
   }


   UpdateData(FALSE);
}

void CApplyBOMTemplateDlg::OnCbSetColumnNumber()
{
   UpdateData(TRUE);

   ColumnAttribute *columnVal = NULL;
   int Index = m_columnCB.GetCurSel();

   if(Index != -1)
      columnVal = (ColumnAttribute *)m_columnCB.GetItemData(Index);

   if(columnVal)
   {
      int errorCode = m_templateData.ValidateInputAttribute(Index,columnVal->columnNum,m_templateData.getAttributeName(),
                  m_templateData.getStartCharIndex(),m_templateData.getEndCharIndex());
      if(!errorCode)
      {
         columnVal->AttributeName = m_templateData.getAttributeName();
         columnVal->startcharIdx = m_templateData.getStartCharIndex();
         columnVal->endcharIdx = m_templateData.getEndCharIndex();
      }
      else
         m_templateData.ErrorMessage(errorCode);
   }

   UpdateData(FALSE);
}

void CApplyBOMTemplateDlg::OnBntCheckNext()
{
   UpdateData(TRUE);

   if(!m_templateData.getTemplateFileName().IsEmpty())
   {
      int errorCode = m_templateData.ValidateHeaderData();
      if(!errorCode)
         OnOK();
      else
         m_templateData.ErrorMessage(errorCode);
   }
   else
      formatMessageBox("Please Select a Template File!!");

   UpdateData(FALSE);
}

void CApplyBOMTemplateDlg::onListSelectTemplateFile()
{
   UpdateData(TRUE);

   int SeletcedIndex = m_fileList.GetCurSel();
   if(SeletcedIndex >= 0)
   {
      m_fileList.GetText(SeletcedIndex,m_templateData.getTemplateFileName());
      LoadTemplateFile(SeletcedIndex);
   }

   EnableAttributeRange();

   UpdateData(FALSE);
}

void CApplyBOMTemplateDlg::OnEnKillfocusEditStartline()
{
   // TODO: Add your control notification handler code here
   CString data;
   data.Format("%i",m_templateData.getStartLine());
   OnVerifyEmptyValue(IDC_EDIT_STARTLINE,Err_StartLine_Value,data);
}

void CApplyBOMTemplateDlg::OnEnKillfocusEditStartcharidx()
{
   // TODO: Add your control notification handler code here
   CString data;
   data.Format("%i",m_templateData.getStartCharIndex());
   OnVerifyEmptyValue(IDC_EDIT_STARTCHARIDX,Err_Range_Value,data);
}

void CApplyBOMTemplateDlg::OnEnKillfocusEditEndcharidx()
{
   // TODO: Add your control notification handler code here
   CString data;
   data.Format("%i",m_templateData.getEndCharIndex());
   OnVerifyEmptyValue(IDC_EDIT_ENDCHARIDX,Err_Range_Value,data);
}

void CApplyBOMTemplateDlg::OnEnKillfocusEditAttributename()
{
   // TODO: Add your control notification handler code here
   OnVerifyEmptyValue(IDC_EDIT_ATTRIBUTENAME,Err_AttrName_Value,m_templateData.getAttributeName());
}

void CApplyBOMTemplateDlg::OnVerifyEmptyValue(int uid, int errorCode, CString restoreVal)
{
   CEdit *curEdit = reinterpret_cast<CEdit *>(GetDlgItem(uid));   
   CString curText;
   curEdit->GetWindowText(curText);
   
   //recover the original data if start line is empty
   if(curText.IsEmpty())
   {     
      m_templateData.ErrorMessage(errorCode);
      curEdit->SetWindowText(restoreVal);
      curEdit->SetFocus();
      curEdit->SetSel(0,restoreVal.GetLength());
   }

}

void CApplyBOMTemplateDlg::FillTemplateFileList()
{
   CFileFind finder;
   CString FileName;

   m_fileList.ResetContent();

   FileName.Format("%s\\*%s",m_templateData.getTemplateFolderPath(),TEMPLATE_EXTNAME);   
   BOOL bWorking = finder.FindFile(FileName);
   while (bWorking)
   {
      bWorking = finder.FindNextFile();
      FileName = finder.GetFileName();
      m_fileList.AddString(FileName);
   }  

   //Select First File
   if(m_fileList.GetCount())
   {
      m_fileList.SetCurSel(0);
      m_fileList.GetText(0,m_templateData.getTemplateFileName());
   }
}

void CApplyBOMTemplateDlg::ClearColumnNumber()
{
   int combosize = m_columnCB.GetCount();
   while(combosize > 0)
   {
      m_columnCB.DeleteItem(0);
      m_columnCB.DeleteString(0);
      combosize --;
   }
   m_columnCB.Clear();
}

void CApplyBOMTemplateDlg::FillColumnNumbers()
{
   ClearColumnNumber();

   for(int i = 0; i < m_templateData.getColumnAttriButeList().GetCount() ; i++)
   {
      ColumnAttribute *columnVal =  m_templateData.getColumnAttriButeList().GetAt(i);
      if(columnVal)
      {
         CString pIndex;
         pIndex.Format("%i",columnVal->columnNum);
         m_columnCB.SetItemData(m_columnCB.AddString(pIndex), (DWORD)columnVal);
      }
   }
   
   //show first column
   if(m_columnCB.GetCount())
   {
      m_columnCB.SetCurSel(0);
      ColumnAttribute *columnVal = (ColumnAttribute *)m_columnCB.GetItemData(0);
      if(columnVal)
      {
         m_templateData.setAttributeName(columnVal->AttributeName);
         m_templateData.setStartCharIndex(columnVal->startcharIdx);
         m_templateData.setEndCharIndex(columnVal->endcharIdx);
      }
   }
   else
      m_columnCB.SetCurSel(-1);
}

void CApplyBOMTemplateDlg::LoadTemplateFile(int index)
{
   CString FileName; 
   if(index < m_fileList.GetCount())
   {
      m_fileList.GetText(index,FileName);
      FileName.Insert(0,m_templateData.getTemplateFolderPath()+"\\");

      if(m_templateData.LoadTemplateFile(FileName))
      {         
         FillColumnNumbers();       
      }
   }
}

void CApplyBOMTemplateDlg::EnableAttributeRange()
{
   if(m_templateData.getDelimiter().Find(DEL_COLUMN) != -1)
   {
      GetDlgItem(IDC_EDIT_STARTCHARIDX)->EnableWindow(TRUE);
      GetDlgItem(IDC_EDIT_ENDCHARIDX)->EnableWindow(TRUE);
   }
   else
   {
      GetDlgItem(IDC_EDIT_STARTCHARIDX)->EnableWindow(FALSE);
      GetDlgItem(IDC_EDIT_ENDCHARIDX)->EnableWindow(FALSE);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CEditBOMTemplateDlg dialog
/////////////////////////////////////////////////////////////////////////////
CEditBOMTemplateDlg::CEditBOMTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditBOMTemplateDlg::IDD, pParent)
{
}

CEditBOMTemplateDlg::~CEditBOMTemplateDlg()
{
}

void CEditBOMTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CEditBOMTemplateDlg)
   DDX_Control(pDX, IDC_LIST_TEMPLATEFILES, m_fileList);
   DDX_Control(pDX, IDC_COMBO_DELIMITERCHAR, m_delimiterCB);
   DDX_Text(pDX, IDC_STATIC_TEMPNAME, m_templateData.getTemplateName());      
   DDX_Text(pDX, IDC_EDIT_STARTLINE, m_templateData.getStartLine());   
   DDX_Text(pDX, IDC_EDIT_COMMENTCHAR, m_templateData.getCommontChars());
   DDX_Text(pDX, IDC_EDIT_ATTRIBUTENAME, m_templateData.getAttributeName());   
   DDX_Text(pDX, IDC_EDIT_STARTCHARIDX, m_templateData.getStartCharIndex());   
   DDX_Text(pDX, IDC_EDIT_ENDCHARIDX, m_templateData.getEndCharIndex()); 
   DDX_Text(pDX, IDC_EDIT_COLUMN_NO, m_templateData.getColumnNumber()); 
   DDX_Text(pDX, IDC_DELIMITER_OTHERS, m_templateData.getDelimiter()); 
   DDX_Text(pDX, IDC_TEMPLATE_FOLDER, m_templateData.getTemplateFolderPath());
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditBOMTemplateDlg, CDialog)
  //{{AFX_DATA_MAP(CEditBOMTemplateDlg)
   ON_CBN_SELCHANGE(IDC_LIST_TEMPLATEFILES, OnSelectTemplateFile)
   ON_CBN_SELCHANGE(IDC_COMBO_DELIMITERCHAR, onSelectDelimiter)
   ON_BN_CLICKED(IDC_ADD_COLUMNS, OnBnClickedAddTemplateColumns)
   ON_BN_CLICKED(IDC_REMOVE_COLUMNS, OnBnClickedRemoveTemplateColumns)
   ON_BN_CLICKED(ID_SAVE_TEMPLATEFILE, OnBnClickedSaveTemplateFile)
   ON_BN_CLICKED(IDC_DELETE_TEMPLATEFILE, OnBnClickedDeleteTemplateFile)
   ON_BN_CLICKED(IDC_NEW_TEMPLATEFILE, OnBnClickedNewTemplateFile)
   ON_BN_CLICKED(IDCLOSE, OnClickedClose)
   ON_EN_KILLFOCUS(IDC_EDIT_STARTLINE, OnStartLineKillFocus)
   ON_EN_KILLFOCUS(IDC_EDIT_COMMENTCHAR, OnCommentCharKillFocus)
   ON_EN_KILLFOCUS(IDC_STATIC_TEMPNAME, OnTemplateNameKillFocus)
   ON_EN_KILLFOCUS(IDC_DELIMITER_OTHERS, OnDelimiterOthersKillFocus)
   //}}AFX_DATA_MAP
END_MESSAGE_MAP()


BOOL CEditBOMTemplateDlg::OnInitDialog()
{
   CDialog::OnInitDialog();  

   //Set Template Folder
   m_Templatedir.FindTemplateRegValue(m_templateData.getTemplateFolderPath());

   //Initialize delimiter
   InitializeDelimiters();

   //Initialize grid
   m_BOMTemplateGrid.AttachGrid(this,IDC_Static_TemplateGrid);
   m_BOMTemplateGrid.initGrid((m_templateData.getDelimiter().Find(DEL_COLUMN) == -1)?T_NoRange:T_HasRange);

   //Select a Template file and show its content
   FillTemplateFile();

   //Set maximun character size in  delimiter  and comment
   CEdit *DelimiterOthers = reinterpret_cast<CEdit *>(GetDlgItem(IDC_DELIMITER_OTHERS));
   DelimiterOthers->SetLimitText(1);

   CEdit *CommentChar = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_COMMENTCHAR));
   CommentChar->SetLimitText(1);

   UpdateData(FALSE);
   return TRUE;
}

bool CEditBOMTemplateDlg::OnAlertChangedFile()
{
 
   UpdateData(TRUE);
   
   bool reval = true;
   if ((m_backupData.Compare(m_templateData) || m_BOMTemplateGrid.CompareAttributes(m_backupData.getColumnAttriButeList())) 
      && m_fileList.GetCount()
      && formatMessageBox(MB_ICONQUESTION | MB_YESNO,
      "Do you want to save changes to %s ?",m_templateData.getTemplateFileName()) == IDYES)
   {
      reval = SaveTemplateFile(m_currentFileIndex);
      if(!reval)
         m_fileList.SetCurSel(m_currentFileIndex);
   }
   
   UpdateData(FALSE);   

   return reval;
}

void CEditBOMTemplateDlg::OnSelectTemplateFile()
{
   if(OnAlertChangedFile())
   {
      int SelectedIndex = m_fileList.GetCurSel();
      if(SelectedIndex > -1)
      {
         LoadTemplateFile(SelectedIndex);
      }  

   }
   UpdateData(FALSE);
}


void CEditBOMTemplateDlg::onSelectDelimiter()
{
   // TODO: Add your control notification handler code here
   UpdateData(TRUE);

   int selectedDelimiter = m_delimiterCB.GetCurSel();
   if(selectedDelimiter > -1)
   {
      m_delimiterCB.GetLBText(selectedDelimiter,m_templateData.getDelimiter());

      //enable/disable range
      if(m_templateData.getDelimiter().Find(DEL_COLUMN) != -1)
      {
         GetDlgItem(IDC_EDIT_STARTCHARIDX)->EnableWindow(TRUE);
         GetDlgItem(IDC_EDIT_ENDCHARIDX)->EnableWindow(TRUE);
         m_BOMTemplateGrid.ReColSize(T_HasRange);
      }
      else
      {
         GetDlgItem(IDC_EDIT_STARTCHARIDX)->EnableWindow(FALSE);
         GetDlgItem(IDC_EDIT_ENDCHARIDX)->EnableWindow(FALSE);
         m_BOMTemplateGrid.ReColSize(T_NoRange);
      }

      //enable/disable other delimuiter 
      if(m_templateData.getDelimiter().Find(DEL_OTHERS) != -1)
      {
         GetDlgItem(IDC_DELIMITER_OTHERS)->ShowWindow(TRUE);
         m_templateData.setDelimiter(",");
      }
      else
      {
         GetDlgItem(IDC_DELIMITER_OTHERS)->ShowWindow(FALSE);
      }
   }

   UpdateData(FALSE);
}

void CEditBOMTemplateDlg::OnSetDelimiter(CString delimiter)
{
   int index = m_delimiterCB.FindString(0,delimiter);
   bool isOthers = false;

   if(index < 0)
      isOthers = true;
   else
   {
       CString DelimiterLab;
       m_delimiterCB.GetLBText(index,DelimiterLab);
       if(DelimiterLab.Find(DEL_OTHERS) != -1)
         isOthers = true;
   }

   //enable/disable range
   if(m_templateData.getDelimiter().Find(DEL_COLUMN) != -1)
   {
      GetDlgItem(IDC_EDIT_STARTCHARIDX)->EnableWindow(TRUE);
      GetDlgItem(IDC_EDIT_ENDCHARIDX)->EnableWindow(TRUE);
      m_BOMTemplateGrid.ReColSize(T_HasRange);
   }
   else
   {
      GetDlgItem(IDC_EDIT_STARTCHARIDX)->EnableWindow(FALSE);
      GetDlgItem(IDC_EDIT_ENDCHARIDX)->EnableWindow(FALSE);
      m_BOMTemplateGrid.ReColSize(T_NoRange);
   }

   if(!isOthers)
   {    
      m_delimiterCB.SetCurSel(index);
      GetDlgItem(IDC_DELIMITER_OTHERS)->ShowWindow(FALSE);

   }
   else //Others
   {
      GetDlgItem(IDC_DELIMITER_OTHERS)->ShowWindow(TRUE);

      index = m_delimiterCB.FindString(0,DEL_OTHERS);
      m_delimiterCB.SetCurSel(index); 
   }
}

void CEditBOMTemplateDlg::OnBnClickedAddTemplateColumns()
{
   UpdateData(TRUE);
   
   // TODO: Add your control notification handler code here  
   int errorCode = ValidateInputAttribute(&m_templateData);
   if(!errorCode)
   {
      //Add row based on delimiter
      if(m_templateData.getDelimiter().Find(DEL_COLUMN) != -1) 
      {
         m_BOMTemplateGrid.AddRowData(m_templateData.getColumnNumber(),m_templateData.getAttributeName(),
            m_templateData.getStartCharIndex(),m_templateData.getEndCharIndex());
      }
      else
      {
         m_BOMTemplateGrid.AddRowData(m_templateData.getColumnNumber(),m_templateData.getAttributeName());
      }

      //set next field data
      m_templateData.setColumnNumber(m_templateData.getColumnNumber() + 1);
      m_templateData.setStartCharIndex(m_templateData.getEndCharIndex() + 1);
      m_templateData.setEndCharIndex(m_templateData.getStartCharIndex() + 1);
      m_templateData.setAttributeName("");
   }
   else
   {
      m_templateData.ErrorMessage(errorCode);
   }

   UpdateData(FALSE);
}


void CEditBOMTemplateDlg::OnBnClickedRemoveTemplateColumns()
{
   UpdateData(TRUE);

   // TODO: Add your control notification handler code here
    m_BOMTemplateGrid.DeleteRowBySelectValue();
}

void CEditBOMTemplateDlg::OnBnClickedNewTemplateFile()
{
   // TODO: Add your control notification handler code here
   //Save Previous setting
   if(!OnAlertChangedFile())
      return;

   CNewBOMTemplateFilenameDlg filenameDlg;
   int retval;

   //Enter File Name
   while(1)
   {   
      retval = filenameDlg.DoModal();  
      if((retval == IDOK && m_fileList.FindString(0,filenameDlg.getTemplateFileName() + TEMPLATE_EXTNAME) == -1)
         || retval == IDCANCEL)
         break;
      else
         formatMessageBox("Filename has existed. Please enter a new name");

   }

   if(retval == IDOK)
   {
      //Clear input data
      m_templateData.ResetTemplateData();      
      m_BOMTemplateGrid.ClearAllData();
      m_templateData.setgetTemplateName(filenameDlg.getTemplateFileName());
      m_templateData.getTemplateFileName().SetString(m_templateData.getTemplateName() + TEMPLATE_EXTNAME);
      UpdateData(FALSE);
         
      //Create a new file
      CString FileName;
      FileName.Format("%s\\%s",m_templateData.getTemplateFolderPath(),m_templateData.getTemplateFileName());      
      m_templateData.SaveTemplateFile(FileName);

      m_backupData = m_templateData;

      //refresh file list
      RefreshFileList();

      int selectIndex = m_fileList.FindString(0,m_templateData.getTemplateFileName());
      if(selectIndex > -1)
      {
         m_fileList.SetCurSel(selectIndex);
         LoadTemplateFile(selectIndex);
         UpdateData(FALSE);
      }

   }

}

void CEditBOMTemplateDlg::OnBnClickedSaveTemplateFile()
{
   UpdateData(TRUE);
   SaveTemplateFile(m_fileList.GetCurSel());
   UpdateData(FALSE);
 
}

void CEditBOMTemplateDlg::OnBnClickedDeleteTemplateFile()
{
   // TODO: Add your control notification handler code here

   int SeletcedIndex = m_fileList.GetCurSel();
   if(SeletcedIndex > -1)
   {
      CString FileName;
      m_fileList.GetText(SeletcedIndex,m_templateData.getTemplateFileName());
      FileName.Format("%s\\%s",m_templateData.getTemplateFolderPath(),m_templateData.getTemplateFileName());
      
      if(formatMessageBox(MB_ICONQUESTION | MB_YESNO,
         "Do you want to delete %s ?",m_templateData.getTemplateFileName()) == IDYES
         && DeleteFile(FileName))
      {
         m_fileList.DeleteString(SeletcedIndex);

         //Set next index
         if(!m_fileList.GetCount())
            SeletcedIndex = -1;
         else
            SeletcedIndex = (SeletcedIndex >= m_fileList.GetCount())?0:SeletcedIndex;
         
         m_fileList.SetCurSel(SeletcedIndex);
         if(SeletcedIndex > -1)
         {
            m_fileList.GetText(SeletcedIndex,m_templateData.getTemplateName());
            LoadTemplateFile(SeletcedIndex);
            UpdateData(FALSE);
         }  
      }
   }
}

void CEditBOMTemplateDlg::OnClickedClose()
{
   // TODO: Add your control notification handler code here   
   if(OnAlertChangedFile())
      OnOK();
}


void CEditBOMTemplateDlg::OnStartLineKillFocus()
{
   // TODO: Add your control notification handler code here
   CString data;
   data.Format("%i",m_templateData.getStartLine());
   OnVerifyEmptyValue(IDC_EDIT_STARTLINE,Err_StartLine_Value, data);
}

void CEditBOMTemplateDlg::OnCommentCharKillFocus()
{
   // TODO: Add your control notification handler code here
   OnVerifyEmptyValue(IDC_EDIT_COMMENTCHAR,Err_CommentChar_Value,m_templateData.getCommontChars());
}

void CEditBOMTemplateDlg::OnTemplateNameKillFocus()
{
   // TODO: Add your control notification handler code here
   OnVerifyEmptyValue(IDC_STATIC_TEMPNAME,Err_TemplateName_Value,m_templateData.getTemplateName());
}

void CEditBOMTemplateDlg::OnDelimiterOthersKillFocus()
{
   // TODO: Add your control notification handler code here
   OnVerifyEmptyValue(IDC_DELIMITER_OTHERS,Err_Delimiter_Value,m_templateData.getDelimiter());
}

void CEditBOMTemplateDlg::OnVerifyEmptyValue(int uid, int errorCode, CString restoreVal)
{
   CEdit *curEdit = reinterpret_cast<CEdit *>(GetDlgItem(uid));   
   CString curText;
   curEdit->GetWindowText(curText);
   
   //recover the original data if start line is empty
   if(curText.IsEmpty())
   {     
      m_templateData.ErrorMessage(errorCode);
      curEdit->SetWindowText(restoreVal);
      curEdit->SetFocus();
      curEdit->SetSel(0,restoreVal.GetLength());
   }
   
}

void CEditBOMTemplateDlg::FillTemplateFile()
{
   int selectedIndex = m_fileList.GetCurSel();
   RefreshFileList();
   
   if(selectedIndex < 0 && m_fileList.GetCount())
      selectedIndex = 0;

   m_fileList.SetCurSel(selectedIndex);
   if(selectedIndex > -1)
   {
      m_fileList.GetText(0,m_templateData.getTemplateFileName());
      LoadTemplateFile(selectedIndex);
      UpdateData(FALSE);   
   }
}

void CEditBOMTemplateDlg::RefreshFileList()
{
   CFileFind finder;
   CString FileName;

   m_fileList.ResetContent();

   FileName.Format("%s\\*%s",m_templateData.getTemplateFolderPath(),TEMPLATE_EXTNAME);   
   BOOL bWorking = finder.FindFile(FileName);
   while (bWorking)
   {
      bWorking = finder.FindNextFile();
      FileName = finder.GetFileName();
      m_fileList.AddString(FileName);
   }
}

void CEditBOMTemplateDlg::LoadTemplateFile(int index)
{
   CString FileName; 

   if(index > -1 && index < m_fileList.GetCount())
   {
      m_currentFileIndex = index;
      m_fileList.GetText(index,m_templateData.getTemplateFileName());
      FileName.Format("%s\\%s",m_templateData.getTemplateFolderPath(),m_templateData.getTemplateFileName());
      m_BOMTemplateGrid.ClearAllData();

      if(m_templateData.LoadTemplateFile(FileName))
      {
         
         // Set Delimiter
         OnSetDelimiter(m_templateData.getDelimiter());

         // Set Attributes
         int tablesize =  m_templateData.getColumnAttriButeList().GetCount();
         for(int i = 0; i < tablesize; i++)
         {
            ColumnAttribute *columnVal = m_templateData.getColumnAttriButeList().GetAt(i);
            if(columnVal)
            {
               if(m_templateData.getDelimiter().Find(DEL_COLUMN) != -1)
               {
                  m_BOMTemplateGrid.ReColSize(T_HasRange);
                  m_BOMTemplateGrid.AddRowData(columnVal->columnNum,columnVal->AttributeName,
                     columnVal->startcharIdx,columnVal->endcharIdx);
               }
               else
               {
                  m_BOMTemplateGrid.ReColSize(T_NoRange);
                  m_BOMTemplateGrid.AddRowData(columnVal->columnNum,columnVal->AttributeName);
               }
            }/*if*/
         }
      }
   }
   m_backupData = m_templateData;
}

bool CEditBOMTemplateDlg::SaveTemplateFile(int index)
{
   bool retVal = true;
   // TODO: Add your control notification handler code here
   int errorCode = m_templateData.ValidateHeaderData();
   if(!errorCode)
   {
      m_templateData.ClearAttributes();
      int rouCnt = m_BOMTemplateGrid.GetNumberRows();
      
      //save grid data template data
      for(int i = 0; i < rouCnt; i++)
      {
         ColumnAttribute newParam;

         if(m_templateData.getDelimiter().Find(DEL_COLUMN) != -1)
            m_BOMTemplateGrid.GetRowData(i, newParam.columnNum, newParam.AttributeName, newParam.startcharIdx, newParam.endcharIdx);
         else
            m_BOMTemplateGrid.GetRowData(i, newParam.columnNum, newParam.AttributeName);

         m_templateData.AddNewAttribute(newParam);
      }

      //save to file
      if(index > -1)
      {
         CString FileName;
         m_fileList.GetText(index,m_templateData.getTemplateFileName());

         FileName.Format("%s\\%s",m_templateData.getTemplateFolderPath(),m_templateData.getTemplateFileName());
         m_templateData.SaveTemplateFile(FileName);
         m_backupData = m_templateData;

      }
      else //no file is selected, create a new file
      {
         CNewBOMTemplateFilenameDlg filenameDlg;
         if(filenameDlg.DoModal() == IDOK)
         {
            CString FileName;

            m_templateData.getTemplateFileName().SetString(filenameDlg.getTemplateFileName()+ TEMPLATE_EXTNAME);
            FileName.Format("%s\\%s",m_templateData.getTemplateFolderPath(),m_templateData.getTemplateFileName());      
            m_templateData.SaveTemplateFile(FileName);

            //refresh file list
            m_fileList.SetCurSel(0);
            FillTemplateFile();            

         }
      }
   }
   else 
   {
      m_templateData.ErrorMessage(errorCode);
      retVal = false;
   }

   return retVal;
}

void CEditBOMTemplateDlg::InitializeDelimiters()
{
   m_delimiterCB.Clear();
   //fill out delimiter combo box
   POSITION pos = m_templateData.getDelimiterTable().GetStartPosition();
   while(pos)
   {
      CString delimiterStr;
      int delimiterChar;

      m_templateData.getDelimiterTable().GetNextAssoc(pos,delimiterStr,delimiterChar);
      m_delimiterCB.AddString(delimiterStr);
   }

   //select first item
   if(m_delimiterCB.GetCount())
   {
      m_delimiterCB.SetCurSel(0);
      m_delimiterCB.GetLBText(0,m_templateData.getDelimiter());
   }
   else
      m_delimiterCB.SetCurSel(-1);

}

int CEditBOMTemplateDlg::ValidateInputAttribute(CBOMTemplateData *tdata)
{
   return m_BOMTemplateGrid.ValidateInputAttribute(-1, tdata->getColumnNumber(), tdata->getAttributeName(),
      tdata->getStartCharIndex(), tdata->getEndCharIndex());
}

/////////////////////////////////////////////////////////////////////////////
// CBOMTemplateDirectoryDlg dialog
/////////////////////////////////////////////////////////////////////////////
CBOMTemplateDirectoryDlg::CBOMTemplateDirectoryDlg(CWnd* pParent /*=NULL*/)	
   : CDialog(CBOMTemplateDirectoryDlg::IDD, pParent)
{

}

CBOMTemplateDirectoryDlg::~CBOMTemplateDirectoryDlg()
{
}

void CBOMTemplateDirectoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CBOMTemplateDirectoryDlg)
   DDX_Text(pDX, IDC_TEMPLATE_FOLDER, m_TemplatePath);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBOMTemplateDirectoryDlg, CDialog)
  //{{AFX_DATA_MAP(CBOMTemplateDirectoryDlg)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowseTemplatePath)
   //}}AFX_DATA_MAP
END_MESSAGE_MAP()

BOOL CBOMTemplateDirectoryDlg::OnInitDialog()
{
   CDialog::OnInitDialog();  

   m_Templatedir.FindTemplateRegValue(m_TemplatePath);

   UpdateData(FALSE);
   return TRUE;
}

void CBOMTemplateDirectoryDlg::OnBrowseTemplatePath()
{
   // TODO: Add your control notification handler code here
   CBrowse dlg;
   dlg.m_strSelDir = m_TemplatePath;
   dlg.m_strTitle = "Select Project Path";
   if (dlg.DoBrowse())
   {
		// 0 is the current project path
      m_TemplatePath = dlg.m_strPath;
		UpdateData(FALSE);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CNewBOMTemplateFilenameDlg dialog
/////////////////////////////////////////////////////////////////////////////
CNewBOMTemplateFilenameDlg::CNewBOMTemplateFilenameDlg(CWnd* pParent /*=NULL*/)	
   : CDialog(CNewBOMTemplateFilenameDlg::IDD, pParent)
{
    m_TemplateFilename = TEMPLATE_FILENAME;
}

CNewBOMTemplateFilenameDlg::~CNewBOMTemplateFilenameDlg()
{
}

void CNewBOMTemplateFilenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNewBOMTemplateFilenameDlg)
   DDX_Text(pDX, IDC_TEMPLATE_FILENAME, m_TemplateFilename);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewBOMTemplateFilenameDlg, CDialog)
  //{{AFX_DATA_MAP(CNewBOMTemplateFilenameDlg)
   ON_BN_CLICKED(IDOK, OnBnClickedSetFileName)
   //}}AFX_DATA_MAP
END_MESSAGE_MAP()

BOOL CNewBOMTemplateFilenameDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   UpdateData(FALSE);
   return TRUE;
}

void CNewBOMTemplateFilenameDlg::OnBnClickedSetFileName()
{
   UpdateData(TRUE);
   // TODO: Add your control notification handler code here
   if(m_TemplateFilename.IsEmpty())
   {
      formatMessageBox("Please enter template file name!!");
   }
   else
   {     
      OnOK();
   }
   UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CCEtoODBApp
/////////////////////////////////////////////////////////////////////////////
void CCEtoODBApp::OnEditAttrTemplate()
{
   CEditBOMTemplateDlg bomTemplateDlg;
   bomTemplateDlg.DoModal();
}

void CCEtoODBApp::OnSetTemplateDirectory()
{
   CBOMTemplateDirectoryDlg bomTemplateFolderDbg;
   if(bomTemplateFolderDbg.DoModal() == IDOK)
   {
      bomTemplateFolderDbg.SetTemplateDirectory();
   }
}

