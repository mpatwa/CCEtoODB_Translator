
#include "StdAfx.h"
#include "PanelizerDialog.h"
#include ".\panelizerdialog.h"
#include "CcDoc.h"
#include "CCEtoODB.h"
#include "Color.h"
#include "FiducialGenerator.h"
#include "ODBC_Lib.h"

#define MenuIdCopyRows   1001
#define MenuIdCutRows    1002
#define MenuIdInsertRows 1003

//_________________________________________________________________________________________________
void CCEtoODBDoc::OnPanelizer() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   CPanelizer panelizer(*this);
   CPanelizerDialog panelizerDialog(panelizer);

   panelizerDialog.DoModal();
}

//_________________________________________________________________________________________________
CPanelEntityClipBoard::CPanelEntityClipBoard(CPanelizer& panelizer)
: m_panelTemplate(panelizer)
{
}

CPanelTemplate& CPanelEntityClipBoard::getPanelTemplate()
{
   return m_panelTemplate;
}

int CPanelEntityClipBoard::getCount(PanelEntityTypeTag entityType)
{
   return m_panelTemplate.getEntityArray(entityType).getSize();
}

void CPanelEntityClipBoard::empty()
{
   m_panelTemplate.empty();
}

void CPanelEntityClipBoard::addClipItem(const CPanelTemplateEntity& templateEntity)
{
   m_panelTemplate.addCopyOfEntity(templateEntity);
}

//void CPanelEntityClipBoard::addClipItem(const CPanelTemplateEntityOutline& templateEntityOutline)
//{
//   m_panelTemplate.getOutlines().add()->operator=(templateEntityOutline);
//}
//
//void CPanelEntityClipBoard::addClipItem(const CPanelTemplateEntityPcbInstance& templateEntityPcbInstance)
//{
//   m_panelTemplate.getPcbInstances().add()->operator=(templateEntityPcbInstance);
//}
//
//void CPanelEntityClipBoard::addClipItem(const CPanelTemplateEntityPcbArray& templateEntityPcbArray)
//{
//   m_panelTemplate.getPcbArrays().add()->operator=(templateEntityPcbArray);
//}
//
//void CPanelEntityClipBoard::addClipItem(const CPanelTemplateEntityFiducial& templateEntityFiducial)
//{
//   m_panelTemplate.getFicucials().add()->operator=(templateEntityFiducial);
//}
//
//void CPanelEntityClipBoard::addClipItem(const CPanelTemplateEntityToolingHole& templateEntityToolingHole)
//{
//   m_panelTemplate.getToolingHoles().add()->operator=(templateEntityToolingHole);
//}

//_________________________________________________________________________________________________
CPanelEntitySheet::CPanelEntitySheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& name,COLORREF cellBackgroundColor)
: m_panelEntityGrid(panelEntityGrid)
, m_index(index)
, m_name(name)
, m_cellBackgroundColor(cellBackgroundColor)
, m_multiSelect(NULL)
, m_dropListType(NULL)
, m_dropListTypeId(-1)
, m_validFlag(false)
{
}

CPanelEntitySheet::~CPanelEntitySheet()
{
   delete m_dropListType;
}

void CPanelEntitySheet::init()
{
   m_panelEntityGrid.SetNumberSheets(m_index + 1);
   m_panelEntityGrid.AddTab(m_name,m_index);

   m_panelEntityGrid.SetSheetNumber(m_index);
   m_panelEntityGrid.EnableMenu(true);

   COLORREF highlightColor          = getHighlightColor();
   COLORREF tabBackgroundColor      = getTabBackgroundColor();
   COLORREF defaultBackgroundColor  = getDefaultBackgroundColor();

   CUGCell cell;
   m_panelEntityGrid.GetGridDefault(&cell);
   cell.SetBackColor(m_cellBackgroundColor);
   cell.SetHBackColor(highlightColor);
   m_panelEntityGrid.SetGridDefault(&cell);

   m_panelEntityGrid.SetTabBackColor(m_index,tabBackgroundColor);

   m_panelEntityGrid.m_CUGTab->SetTabHBackColor(m_index,defaultBackgroundColor);

   m_panelEntityGrid.m_GI->m_hdgDefaults->SetBackColor(getHeadingBackgroundColor());

   m_panelEntityGrid.SetMultiSelectMode(true);
   m_panelEntityGrid.SetCurrentCellMode(3);

   if (m_multiSelect == NULL)
   {
      m_multiSelect = new CPanelEntityGridMultiSelect();

      m_panelEntityGrid.SetNewMultiSelectClass(m_multiSelect);
   }

   if (m_dropListType == NULL)
   {
      m_dropListType = new CPanelEntityGridDropListType(highlightColor);
      m_dropListTypeId = m_panelEntityGrid.AddCellType(m_dropListType);
   }

   m_panelEntityGrid.SetDefFont(m_panelEntityGrid.getDefaultFont());

   m_panelEntityGrid.GetHeadingDefault(&cell);
   cell.SetFont(m_panelEntityGrid.getDefaultHeadingFont());
   m_panelEntityGrid.SetHeadingDefault(&cell);
}

CPanelEntityGridMultiSelect* CPanelEntitySheet::getMultiSelect()
{
   return m_multiSelect;
}

CPanelEntityGrid& CPanelEntitySheet::getPanelEntityGrid()
{
   return m_panelEntityGrid;
}

CPanelizer& CPanelEntitySheet::getPanelizer()
{
   return m_panelEntityGrid.getPanelizer();
}

CPanelEntityGridSelectionController& CPanelEntitySheet::getSelectionController()
{
   return getPanelEntityGrid().getSelectionController();
}

CPanelEntityClipBoard& CPanelEntitySheet::getClipBoard()
{
   return getPanelEntityGrid().getClipBoard();
}

int CPanelEntitySheet::getDropListTypeId() const
{
   return m_dropListTypeId;
}

bool CPanelEntitySheet::canAddRow()
{
   return true;
}

int CPanelEntitySheet::getIndex() const
{
   return m_index;
}

CString CPanelEntitySheet::getName() const
{
   return m_name;
}

COLORREF CPanelEntitySheet::getCellBackgroundColor() const
{
   return m_cellBackgroundColor;
}

COLORREF CPanelEntitySheet::getTabBackgroundColor() const
{
   CColor defaultBackgroundColor(m_cellBackgroundColor);

   double hue,sat,val;
   defaultBackgroundColor.getHsv(hue,sat,val);

   sat = sat + (1. - sat)/2.;

   defaultBackgroundColor.setHsv(hue,sat,val);

   return defaultBackgroundColor.getColor();
}

COLORREF CPanelEntitySheet::getDefaultBackgroundColor() const
{
   CColor defaultBackgroundColor(m_cellBackgroundColor);

   double hue,sat,val;
   defaultBackgroundColor.getHsv(hue,sat,val);

   sat *= .7;

   defaultBackgroundColor.setHsv(hue,sat,val);

   return defaultBackgroundColor.getColor();
}

COLORREF CPanelEntitySheet::getHeadingBackgroundColor() const
{
   return getTabBackgroundColor();
}

COLORREF CPanelEntitySheet::getHighlightColor() const
{
   CColor highlightColor(m_cellBackgroundColor);

   double hue,sat,val;
   highlightColor.getHsv(hue,sat,val);

   sat = 1.;

   highlightColor.setHsv(hue,sat,val);

   return highlightColor.getColor();
}

int CPanelEntitySheet::addColumn(const CString& columnName)
{
   int columnIndex = m_columnNames.Add(columnName);

   m_panelEntityGrid.SetSheetNumber(m_index);
   m_panelEntityGrid.SetNumberCols(columnIndex + 1);
   m_panelEntityGrid.QuickSetText(columnIndex,-1,columnName);

   return columnIndex;
}

bool CPanelEntitySheet::onEditVerify(int col,long row,CWnd* edit,UINT* vcKey)
{
   return true;
}

bool CPanelEntitySheet::onEditFinish(int col,long row,CWnd* edit,CString& string,BOOL cancelFlag)
{
   bool retval = true;

   if (!cancelFlag)
   {
      CPanelTemplateEntityArray& entityArray = getEntityArray();
      CPanelTemplateEntity* entity = entityArray.getEntityAt(row);
      int rowCount = getPanelEntityGrid().GetNumberRows();

      if (entity == NULL && row == rowCount - 1)
      {
         entity = entityArray.addEntity();
      }

      if (entity != NULL)
      {
         retval = entity->setValue(col,string);
         string = entity->getText(col);
         updateRow(row);
      }
      else
      {
         retval = false;
      }

      if (col == getBoardPhysicalNameColumn())
      {
         //entity->mapLogicalNameToPhysicalName();
         getPanelEntityGrid().invalidateBoardMapping();
      }

      if (retval && row == rowCount - 1 && canAddRow())
      {
         getPanelEntityGrid().SetNumberRows(rowCount + 1);
      }
   }

   fillInvalidGrid();
   getPanelizer().updateModifiedPanel();

   return retval;
}

bool CPanelEntitySheet::onEditStart(int col,long row,CWnd** edit)
{
   return true;
}

void CPanelEntitySheet::updateRow(long row)
{
   CPanelTemplateEntity* entity = getEntityArray().getEntityAt(row);

   if (entity != NULL)
   {
      CString rowValue;
      rowValue.Format("%d",row + 1);
      getPanelEntityGrid().QuickSetText(-1,row,rowValue);
      getPanelEntityGrid().RedrawCell(-1,row);

      for (int col = 0;col < entity->getParamCount();col++)
      {
         getPanelEntityGrid().QuickSetText(col,row,entity->getText(col));
      }

      getPanelEntityGrid().RedrawRow(row);
   }
}

void CPanelEntitySheet::emptyClipboard()
{
}

int CPanelEntitySheet::onMenuStart(int row,long col,int section)
{
   return 0;
}

int CPanelEntitySheet::startMenu(const CString& entityDescription)
{
   getPanelEntityGrid().EmptyMenu();

   if (getSelectionController().getSelectionState() == panelEntitySelectionStateRows)
   {
      int selectedRowCount = getSelectionController().getSelectedRowCount();

      addMenuItem(MenuIdCopyRows,"Copy %s%s",entityDescription,selectedRowCount > 1 ? "s" : "");
      addMenuItem(MenuIdCutRows ,"Cut %s%s" ,entityDescription,selectedRowCount > 1 ? "s" : "");
   }

   int clipCount = getClipBoard().getCount(getSheetType());

   if (getSelectionController().getHighlightedRow() >= 0 && clipCount > 0)
   {
      addMenuItem(MenuIdInsertRows ,"Insert %s%s" ,entityDescription,clipCount > 1 ? "s" : "");
   }

   return 0;
}

void CPanelEntitySheet::onMenuCommand(int row,long col,int section,int item)
{
   if (item == MenuIdCopyRows || item == MenuIdCutRows)
   {
      getClipBoard().empty();

      for (int loopRow = 0;loopRow < getPanelEntityGrid().GetNumberRows();loopRow++)
      {
         if (getSelectionController().isRowSelected(loopRow))
         {
            CPanelTemplateEntity* entity = getEntityArray().getEntityAt(loopRow);

            // Watch out for NULL entity, a row may exist and be selected, but still be empty
            if (entity != NULL)
               getClipBoard().addClipItem(*entity);
         }
      }

      if (item == MenuIdCutRows)
      {
         for (int loopRow =  getPanelEntityGrid().GetNumberRows() - 1;loopRow >= 0;loopRow--)
         {
            if (getSelectionController().isRowSelected(loopRow))
            {
               getPanelEntityGrid().DeleteRow(loopRow);
               getEntityArray().deleteAt(loopRow);
            }
         }

         fillGrid();
         getPanelizer().getPanelTemplate().setModifiedFlag(true);
         getPanelizer().updateModifiedPanel();
      }
   }
   else if (item == MenuIdInsertRows)
   {
      int insertRow = getSelectionController().getHighlightedRow();

      getPanelizer().getPanelTemplate().copyEntities(getClipBoard().getPanelTemplate(),getSheetType(),insertRow);
      fillGrid();
      getPanelizer().getPanelTemplate().setModifiedFlag(true);
      getPanelizer().updateModifiedPanel();
   }
}

int CPanelEntitySheet::addMenuItem(int itemId,const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString itemString;
   itemString.FormatV(format, args);

   int retval = getPanelEntityGrid().AddMenuItem(itemId,itemString);

   return retval;
}

void CPanelEntitySheet::deleteAllRows()
{
   getPanelEntityGrid().SetSheetNumber(getIndex());

   int rowCount    = getPanelEntityGrid().GetNumberRows();
   int columnCount = getPanelEntityGrid().GetNumberCols();

   for (int row = rowCount - 1;row >= 0;row--)
   {
      //for (int col = 0;col < columnCount;col++)
      //{
      //   getPanelEntityGrid().QuickSetText(row,col,"");
      //}

      getPanelEntityGrid().DeleteRow(row);
   }
}

int CPanelEntitySheet::getBoardLogicalNameColumn() const
{
   return -1;
}

int CPanelEntitySheet::getBoardPhysicalNameColumn() const
{
   return -1;
}

CString CPanelEntitySheet::booleanLabelString() const
{
   CString labelString("yes\n" "no\n");

   return labelString;
}

void CPanelEntitySheet::invalidate()
{
   m_validFlag = false;
}

void CPanelEntitySheet::invalidateBoardMapping()
{
}

void CPanelEntitySheet::fillInvalidGrid()
{
   if (!m_validFlag)
   {
      fillGrid();
   }
}

void CPanelEntitySheet::fillGrid()
{
   CPanelTemplateEntityArray& entityArray = getPanelEntityGrid().getPanelizer().getPanelTemplate().getEntityArray(this->getSheetType());

   int entityCount = entityArray.getSize();

   getPanelEntityGrid().SetSheetNumber(getIndex());
   //getPanelEntityGrid().SetNumberRows(0);
   deleteAllRows();
   getPanelEntityGrid().SetNumberRows(max(entityCount + (canAddRow() ? 1 : 0),1));

   for (int row = 0;row < entityCount;row++)
   {
      updateRow(row);
   }

   m_validFlag = true;
}

void CPanelEntitySheet::clearEntityHighlights()
{
   CPanelTemplateEntityArray& entityArray = getEntityArray();
   int rowCount    = getPanelEntityGrid().GetNumberRows();

   for (int row = 0;row < rowCount;row++)
   {
      CPanelTemplateEntity* entity = entityArray.getEntityAt(row);

      if (entity != NULL)
      {
         entity->highlightEntity(false);
      }
   }
}

void CPanelEntitySheet::highlightEntityFromRow(int row)
{
   CPanelTemplateEntityArray& entityArray = getEntityArray();
   CPanelTemplateEntity* entity = entityArray.getEntityAt(row);

   if (entity != NULL)
   {
      entity->highlightEntity(true);
   }
}

void CPanelEntitySheet::highlightEntitiesFromRows(int rowStart,int rowEnd)
{
   if (rowStart > rowEnd)
   {
      int temp = rowStart;
      rowStart = rowEnd;
      rowEnd   = temp;
   }

   for (int row = rowStart;row <= rowEnd;row++)
   {
      highlightEntityFromRow(row);
   }
}

//_________________________________________________________________________________________________
CPanelEntityOutlineSheet::CPanelEntityOutlineSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& tabName,COLORREF cellBackgroundColor)
: CPanelEntitySheet(panelEntityGrid,index,tabName,cellBackgroundColor)
{
   init();

   m_xOriginColumnIndex = addColumn("X Origin");
   m_yOriginColumnIndex = addColumn("Y Origin");
   m_xSizeColumnIndex   = addColumn("X Size");
   m_ySizeColumnIndex   = addColumn("Y Size");

   CUGCell cell;

   panelEntityGrid.GetColDefault(m_xOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_yOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_yOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_xSizeColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xSizeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_ySizeColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_ySizeColumnIndex,&cell);

   panelEntityGrid.SetNumberRows(1);
}

PanelEntityTypeTag CPanelEntityOutlineSheet::getSheetType() const
{
   return panelEntityTypeOutline;
}

CPanelTemplateEntityArray& CPanelEntityOutlineSheet::getEntityArray()
{
   return getPanelizer().getPanelTemplate().getOutlines();
}

int CPanelEntityOutlineSheet::onMenuStart(int row,long col,int section)
{
   getPanelEntityGrid().EmptyMenu();

   CMenu* popupMenu = getPanelEntityGrid().GetPopupMenu();
   int itemCount = popupMenu->GetMenuItemCount();

   return (itemCount > 0);
}

//void CPanelEntityOutlineSheet::onMenuCommand(int row,long col,int section,int item)
//{
//}

bool CPanelEntityOutlineSheet::canAddRow()
{
   return false;
}

//_________________________________________________________________________________________________
CPanelEntityPcbInstanceSheet::CPanelEntityPcbInstanceSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& tabName,COLORREF cellBackgroundColor)
: CPanelEntitySheet(panelEntityGrid,index,tabName,cellBackgroundColor)
{
   init();

   m_nameColumnIndex         = addColumn("PCB Name");
   m_xOriginColumnIndex      = addColumn("X Origin");
   m_yOriginColumnIndex      = addColumn("Y Origin");
   m_angleColumnIndex        = addColumn("Angle");
   m_mirroredColumnIndex     = addColumn("Mirrored");
   m_originTypeColumnIndex   = addColumn("Origin Type");
   m_physicalNameColumnIndex = addColumn("Physical PCB");

   CUGCell cell;

   panelEntityGrid.GetColDefault(m_nameColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_STRING);
   panelEntityGrid.SetColDefault(m_nameColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_xOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_yOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_yOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_angleColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_angleColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_mirroredColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   cell.SetLabelText(booleanLabelString());
   panelEntityGrid.SetColDefault(m_mirroredColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_originTypeColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   EnumLabelString(PanelizerOriginTypeTag,originTypeLabelString);
   cell.SetLabelText(originTypeLabelString.getLabelString());
   panelEntityGrid.SetColDefault(m_originTypeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_physicalNameColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   cell.SetLabelText(getPanelizer().getPcbFilesLabelString());
   panelEntityGrid.SetColDefault(m_physicalNameColumnIndex,&cell);

   int width = panelEntityGrid.GetColWidth(m_originTypeColumnIndex);
   width = DcaRound(width * 1.1);
   panelEntityGrid.SetColWidth(m_originTypeColumnIndex,width);

   width = panelEntityGrid.GetColWidth(m_physicalNameColumnIndex);
   width = DcaRound(width * 1.8);
   panelEntityGrid.SetColWidth(m_physicalNameColumnIndex,width);

   panelEntityGrid.SetNumberRows(1);
}

PanelEntityTypeTag CPanelEntityPcbInstanceSheet::getSheetType() const
{
   return panelEntityTypePcbInstance;
}

CPanelTemplateEntityArray& CPanelEntityPcbInstanceSheet::getEntityArray()
{
   return getPanelizer().getPanelTemplate().getPcbInstances();
}

int CPanelEntityPcbInstanceSheet::onMenuStart(int row,long col,int section)
{
   startMenu("PCB");

   //getPanelEntityGrid().EmptyMenu();

   //int rowCount = getPanelEntityGrid().GetNumberRows();

   //if (getSelectionController().getSelectionState() == panelEntitySelectionStateRows)
   //{
   //   int selectedRowCount = getSelectionController().getSelectedRowCount();

   //   addMenuItem(MenuIdCopyRows,"Copy PCB%s",selectedRowCount > 1 ? "s" : "");
   //   addMenuItem(MenuIdCutRows ,"Cut PCB%s" ,selectedRowCount > 1 ? "s" : "");

   //   //int clipCount = getClipBoard().getCount(getSheetType());

   //   //if (selectedRowCount == 1 && clipCount > 0)
   //   //{
   //   //   addMenuItem(MenuIdInsertRows ,"Insert PCB%s" ,clipCount > 1 ? "s" : "");
   //   //}
   //}

   //int clipCount = getClipBoard().getCount(getSheetType());

   //if (getSelectionController().getHighlightedRow() >= 0 && clipCount > 0)
   //{
   //   addMenuItem(MenuIdInsertRows ,"Insert PCB%s" ,clipCount > 1 ? "s" : "");
   //}

   return 0;
}

//void CPanelEntityPcbInstanceSheet::onMenuCommand(int row,long col,int section,int item)
//{
//}

int CPanelEntityPcbInstanceSheet::getBoardLogicalNameColumn() const
{
   return m_nameColumnIndex;
}

int CPanelEntityPcbInstanceSheet::getBoardPhysicalNameColumn() const
{
   return m_physicalNameColumnIndex;
}

void CPanelEntityPcbInstanceSheet::invalidateBoardMapping()
{
   invalidate();
}

//_________________________________________________________________________________________________
CPanelEntityPcbArraySheet::CPanelEntityPcbArraySheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& tabName,COLORREF cellBackgroundColor)
: CPanelEntitySheet(panelEntityGrid,index,tabName,cellBackgroundColor)
{
   init();

   m_nameColumnIndex         = addColumn("PCB Name");
   m_xOriginColumnIndex      = addColumn("X Origin");
   m_yOriginColumnIndex      = addColumn("Y Origin");
   m_angleColumnIndex        = addColumn("Angle");
   m_mirroredColumnIndex     = addColumn("Mirrored");
   m_originTypeColumnIndex   = addColumn("Origin Type");
   m_xCountColumnIndex       = addColumn("X Count");
   m_yCountColumnIndex       = addColumn("Y Count");
   m_xSizeColumnIndex        = addColumn("X Step");
   m_ySizeColumnIndex        = addColumn("Y Step");
   m_physicalNameColumnIndex = addColumn("Physical PCB");

   CUGCell cell;

   panelEntityGrid.GetColDefault(m_nameColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_STRING);
   panelEntityGrid.SetColDefault(m_nameColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_xOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_yOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_yOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_angleColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_angleColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_mirroredColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   cell.SetLabelText(booleanLabelString());
   panelEntityGrid.SetColDefault(m_mirroredColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_originTypeColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   EnumLabelString(PanelizerOriginTypeTag,originTypeLabelString);
   cell.SetLabelText(originTypeLabelString.getLabelString());
   panelEntityGrid.SetColDefault(m_originTypeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_xCountColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xCountColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_yCountColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_yCountColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_xSizeColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xSizeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_ySizeColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_ySizeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_physicalNameColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   cell.SetLabelText(getPanelizer().getPcbFilesLabelString());
   panelEntityGrid.SetColDefault(m_physicalNameColumnIndex,&cell);

   int width = panelEntityGrid.GetColWidth(m_originTypeColumnIndex);
   width = DcaRound(width * 1.1);
   panelEntityGrid.SetColWidth(m_originTypeColumnIndex,width);

   width = panelEntityGrid.GetColWidth(m_physicalNameColumnIndex);
   width = DcaRound(width * 1.8);
   panelEntityGrid.SetColWidth(m_physicalNameColumnIndex,width);

   panelEntityGrid.SetNumberRows(1);
}

PanelEntityTypeTag CPanelEntityPcbArraySheet::getSheetType() const
{
   return panelEntityTypePcbArray;
}

CPanelTemplateEntityArray& CPanelEntityPcbArraySheet::getEntityArray()
{
   return getPanelizer().getPanelTemplate().getPcbArrays();
}

int CPanelEntityPcbArraySheet::onMenuStart(int row,long col,int section)
{
   startMenu("PCB Array");

   //getPanelEntityGrid().EmptyMenu();

   //int rowCount = getPanelEntityGrid().GetNumberRows();

   //if (getSelectionController().getSelectionState() == panelEntitySelectionStateRows)
   //{
   //   int selectedRowCount = getSelectionController().getSelectedRowCount();

   //   addMenuItem(MenuIdCopyRows,"Copy PCB Array%s",selectedRowCount > 1 ? "s" : "");
   //   addMenuItem(MenuIdCutRows ,"Cut PCB Array%s" ,selectedRowCount > 1 ? "s" : "");

   //   int clipCount = getClipBoard().getCount(getSheetType());

   //   if (selectedRowCount == 1 && clipCount > 0)
   //   {
   //      addMenuItem(MenuIdInsertRows ,"Insert PCB Array%s" ,clipCount > 1 ? "s" : "");
   //   }
   //}

   return 0;
}

//void CPanelEntityPcbArraySheet::onMenuCommand(int row,long col,int section,int item)
//{
//}

int CPanelEntityPcbArraySheet::getBoardLogicalNameColumn() const
{
   return m_nameColumnIndex;
}

int CPanelEntityPcbArraySheet::getBoardPhysicalNameColumn() const
{
   return m_physicalNameColumnIndex;
}

void CPanelEntityPcbArraySheet::invalidateBoardMapping()
{
   invalidate();
}

//_________________________________________________________________________________________________
CPanelEntityFiducialSheet::CPanelEntityFiducialSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& tabName,COLORREF cellBackgroundColor)
: CPanelEntitySheet(panelEntityGrid,index,tabName,cellBackgroundColor)
{
   init();

   m_typeColumnIndex       = addColumn("Type");
   m_sizeColumnIndex       = addColumn("Size");
   m_xOriginColumnIndex    = addColumn("X Origin");
   m_yOriginColumnIndex    = addColumn("Y Origin");
   m_angleColumnIndex      = addColumn("Angle");
   m_mirroredColumnIndex   = addColumn("Mirrored");

   CUGCell cell;

   panelEntityGrid.GetColDefault(m_typeColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   EnumLabelString(FiducialTypeTag,fiducialTypeLabelString);
   cell.SetLabelText(fiducialTypeLabelString.getLabelString());
   panelEntityGrid.SetColDefault(m_typeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_sizeColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_sizeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_xOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_yOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_yOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_angleColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_angleColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_mirroredColumnIndex,&cell);
   cell.SetCellType(getDropListTypeId());
   cell.SetLabelText(booleanLabelString());
   panelEntityGrid.SetColDefault(m_mirroredColumnIndex,&cell);

   panelEntityGrid.SetNumberRows(1);
}

PanelEntityTypeTag CPanelEntityFiducialSheet::getSheetType() const
{
   return panelEntityTypeFiducial;
}

CPanelTemplateEntityArray& CPanelEntityFiducialSheet::getEntityArray()
{
   return getPanelizer().getPanelTemplate().getFicucials();
}

int CPanelEntityFiducialSheet::onMenuStart(int row,long col,int section)
{
   startMenu("Fiducial");

   //getPanelEntityGrid().EmptyMenu();

   //int rowCount = getPanelEntityGrid().GetNumberRows();

   //if (getSelectionController().getSelectionState() == panelEntitySelectionStateRows)
   //{
   //   int selectedRowCount = getSelectionController().getSelectedRowCount();

   //   addMenuItem(MenuIdCopyRows,"Copy Fiducial%s",selectedRowCount > 1 ? "s" : "");
   //   addMenuItem(MenuIdCutRows ,"Cut Fiducial%s" ,selectedRowCount > 1 ? "s" : "");

   //   int clipCount = getClipBoard().getCount(getSheetType());

   //   if (selectedRowCount == 1 && clipCount > 0)
   //   {
   //      addMenuItem(MenuIdInsertRows ,"Insert Fiducial%s" ,clipCount > 1 ? "s" : "");
   //   }
   //}

   return 0;
}

//void CPanelEntityFiducialSheet::onMenuCommand(int row,long col,int section,int item)
//{
//}

//_________________________________________________________________________________________________
CPanelEntityToolingHoleSheet::CPanelEntityToolingHoleSheet(CPanelEntityGrid& panelEntityGrid,int index,const CString& tabName,COLORREF cellBackgroundColor)
: CPanelEntitySheet(panelEntityGrid,index,tabName,cellBackgroundColor)
{
   init();

   m_sizeColumnIndex       = addColumn("Size");
   m_xOriginColumnIndex    = addColumn("X Origin");
   m_yOriginColumnIndex    = addColumn("Y Origin");

   CUGCell cell;

   panelEntityGrid.GetColDefault(m_sizeColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_sizeColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_xOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_xOriginColumnIndex,&cell);

   panelEntityGrid.GetColDefault(m_yOriginColumnIndex,&cell);
   cell.SetDataType(UGCELLDATA_NUMBER);
   cell.SetAlignment(UG_ALIGNRIGHT);
   panelEntityGrid.SetColDefault(m_yOriginColumnIndex,&cell);

   panelEntityGrid.SetNumberRows(1);
}

PanelEntityTypeTag CPanelEntityToolingHoleSheet::getSheetType() const
{
   return panelEntityTypeToolingHole;
}

CPanelTemplateEntityArray& CPanelEntityToolingHoleSheet::getEntityArray()
{
   return getPanelizer().getPanelTemplate().getToolingHoles();
}

int CPanelEntityToolingHoleSheet::onMenuStart(int row,long col,int section)
{
   startMenu("Tooling Hole");

   //getPanelEntityGrid().EmptyMenu();

   //int rowCount = getPanelEntityGrid().GetNumberRows();

   //if (getSelectionController().getSelectionState() == panelEntitySelectionStateRows)
   //{
   //   int selectedRowCount = getSelectionController().getSelectedRowCount();

   //   addMenuItem(MenuIdCopyRows,"Copy Tooling Hole%s",selectedRowCount > 1 ? "s" : "");
   //   addMenuItem(MenuIdCutRows ,"Cut Tooling Hole%s" ,selectedRowCount > 1 ? "s" : "");

   //   int clipCount = getClipBoard().getCount(getSheetType());

   //   if (selectedRowCount == 1 && clipCount > 0)
   //   {
   //      addMenuItem(MenuIdInsertRows ,"Insert Tooling Hole%s" ,clipCount > 1 ? "s" : "");
   //   }
   //}

   return 0;
}

//void CPanelEntityToolingHoleSheet::onMenuCommand(int row,long col,int section,int item)
//{
//}

//_________________________________________________________________________________________________
CPanelEntitySheets::CPanelEntitySheets(CPanelEntityGrid& panelEntityGrid)
: m_panelEntityGrid(panelEntityGrid)
{
}

int CPanelEntitySheets::getCount() const
{
   return m_panelEntitySheets.GetCount();
}

CPanelEntitySheet* CPanelEntitySheets::getAt(int sheetIndex)
{
   CPanelEntitySheet* sheet = NULL;
   
   if (sheetIndex >= 0 && sheetIndex < m_panelEntitySheets.GetSize())
   {
      sheet = m_panelEntitySheets.GetAt(sheetIndex);
   }

   return sheet;
}

CPanelEntitySheet* CPanelEntitySheets::addSheet(PanelEntityTypeTag sheetType,const CString& name,COLORREF cellBackgroundColor)
{
   int sheetIndex = m_panelEntitySheets.GetCount();
   CPanelEntitySheet* sheet = NULL;
   
   switch (sheetType)
   {
   case panelEntityTypeOutline:      sheet = new CPanelEntityOutlineSheet(    m_panelEntityGrid,sheetIndex,name,cellBackgroundColor);  break;
   case panelEntityTypePcbInstance:  sheet = new CPanelEntityPcbInstanceSheet(m_panelEntityGrid,sheetIndex,name,cellBackgroundColor);  break;
   case panelEntityTypePcbArray:     sheet = new CPanelEntityPcbArraySheet(   m_panelEntityGrid,sheetIndex,name,cellBackgroundColor);  break;
   case panelEntityTypeFiducial:     sheet = new CPanelEntityFiducialSheet(   m_panelEntityGrid,sheetIndex,name,cellBackgroundColor);  break;
   case panelEntityTypeToolingHole:  sheet = new CPanelEntityToolingHoleSheet(m_panelEntityGrid,sheetIndex,name,cellBackgroundColor);  break;
   }

   m_panelEntitySheets.Add(sheet);

   //COLORREF tabBackgroundColor  = sheet->getTabBackgroundColor();
   //m_panelEntityGrid.SetSheetNumber(sheetIndex);
   //m_panelEntityGrid.SetTabBackColor(sheetIndex,tabBackgroundColor);

   return sheet;
}

//_________________________________________________________________________________________________
CPanelEntityGridSelectionController::CPanelEntityGridSelectionController(CPanelEntityGrid& panelEntityGrid)
: m_panelEntityGrid(panelEntityGrid)
, m_state(panelEntitySelectionStateEmpty)
, m_currentlyProcessingFlag(false)
, m_ctrlKeyDown(false)
, m_shiftKeyDown(false)
, m_lastClickedRow(-1)
, m_lastFocusedCol(-1)
, m_highlightedRow(-1)
{
}

void CPanelEntityGridSelectionController::reset()
{
   m_state                   = panelEntitySelectionStateEmpty;
   m_currentlyProcessingFlag = false;
   m_ctrlKeyDown             = false;
   m_shiftKeyDown            = false;
   m_lastClickedRow          = -1;
   m_lastFocusedCol          = -1;

   m_panelEntityGrid.ClearSelections();
   clearRowHighlights();
}

PanelEntitySelectionStateTag CPanelEntityGridSelectionController::getSelectionState() const
{
   return m_state;
}

bool CPanelEntityGridSelectionController::isRowSelected(int row) const
{
   bool retval = false;

   if (m_state == panelEntitySelectionStateColumn)
   {
      int col = m_panelEntityGrid.GetCurrentCol();

      retval = (m_panelEntityGrid.IsSelected(col,row,NULL) != 0);
   }
   else if (m_state == panelEntitySelectionStateRows)
   {
      int col = 0;

      retval = (m_panelEntityGrid.IsSelected(col,row,NULL) != 0);
   }

   return retval;
}

int CPanelEntityGridSelectionController::getSelectedRowCount() const
{
   // All selected rows, even empty ones

   int selectedRowCount = 0;

   for (int row = 0;row < m_panelEntityGrid.GetNumberRows();row++)
   {
      if (isRowSelected(row))
      {
         selectedRowCount++;
      }
   }

   return selectedRowCount;
}

int CPanelEntityGridSelectionController::getHighlightedRow() const
{
   return m_highlightedRow;
}

void CPanelEntityGridSelectionController::selectRow(long row)
{
   m_panelEntityGrid.GotoRow(row);
   m_panelEntityGrid.SelectRange(0,row,m_panelEntityGrid.GetNumberCols() - 1,row);
   m_panelEntityGrid.highlightEntityFromRow(row);
}

void CPanelEntityGridSelectionController::selectRows(long rowStart,long rowEnd)
{
   m_panelEntityGrid.GotoRow(rowEnd);
   m_panelEntityGrid.SelectRange(0,rowStart,m_panelEntityGrid.GetNumberCols() - 1,rowEnd);
   m_panelEntityGrid.highlightEntitiesFromRows(rowStart,rowEnd);
}

void CPanelEntityGridSelectionController::clearSelections()
{
   m_panelEntityGrid.ClearSelections();
   m_panelEntityGrid.RedrawAll();
   m_panelEntityGrid.clearEntityHighlights();
}

void CPanelEntityGridSelectionController::OnSH_LClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed)
{
   if (updn)  // true is down
   {
      m_ctrlKeyDown  = ((nFlags & MK_CONTROL) != 0);
      m_shiftKeyDown = ((nFlags & MK_SHIFT  ) != 0);
   }
   else
   {
      m_ctrlKeyDown  = false;
      m_shiftKeyDown = false;
   }

   if (!m_currentlyProcessingFlag && updn)
   {
      m_currentlyProcessingFlag = true;

      switch (m_state)
      {
      case panelEntitySelectionStateEmpty:
      case panelEntitySelectionStateColumn:
         {
            clearSelections();
            selectRow(row);
            m_lastClickedRow = row;

            m_state = panelEntitySelectionStateRows;
         }

         break;
      case panelEntitySelectionStateRows:
         {
            if (! m_ctrlKeyDown)
            {
               clearSelections();
            }

            if (m_shiftKeyDown)
            {
               selectRows(m_lastClickedRow,row);
            }
            else
            {
               selectRow(row);
            }

            m_lastClickedRow = row;
         }

         break;
      }

      m_currentlyProcessingFlag = false;
   }
}

void CPanelEntityGridSelectionController::OnSH_RClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed)
{
   if (updn)  // true is down
   {
      m_ctrlKeyDown  = ((nFlags & MK_CONTROL) != 0);
      m_shiftKeyDown = ((nFlags & MK_SHIFT  ) != 0);
   }
   else
   {
      m_ctrlKeyDown  = false;
      m_shiftKeyDown = false;
   }

   if (!m_currentlyProcessingFlag && updn)
   {
      m_currentlyProcessingFlag = true;

      if (getSelectedRowCount() > 0)
      {
         //m_panelEntityGrid.GotoCell(-1,row);
         highlightRow(row);
      }
      else
      {
         clearRowHighlights();
      }

      //switch (m_state)
      //{
      //case panelEntitySelectionStateEmpty:
      //case panelEntitySelectionStateColumn:
      //   {
      //      clearRowHighlights();
      //   }

      //   break;
      //case panelEntitySelectionStateRows:
      //   {
      //      highlightRow(row);
      //   }

      //   break;
      //}

      m_currentlyProcessingFlag = false;
   }
}

void CPanelEntityGridSelectionController::OnSelectionChanged(int startCol,long startRow,int endCol,long endRow,int blockNum)
{
   //if (!m_currentlyProcessingFlag)
   //{
   //   int col = m_panelEntityGrid.GetCurrentCol();
   //   m_panelEntityGrid.getMultiSelect()->filterInColFromSelection(col);
   //   m_panelEntityGrid.RedrawAll();
   //}
}

void CPanelEntityGridSelectionController::OnCellChange(int oldcol,int newcol,long oldrow,long newrow)
{
   if (!m_currentlyProcessingFlag)
   {
      m_state = panelEntitySelectionStateColumn;
   }
}

void CPanelEntityGridSelectionController::OnLClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,int processed)
{
   if (updn)  // true is down
   {
      m_ctrlKeyDown  = ((nFlags & MK_CONTROL) != 0);
      m_shiftKeyDown = ((nFlags & MK_SHIFT  ) != 0);
   }
   else
   {
      m_ctrlKeyDown  = false;
      m_shiftKeyDown = false;

      if (m_state == panelEntitySelectionStateColumn)
      {
         int col = m_panelEntityGrid.GetCurrentCol();
         m_panelEntityGrid.getCurrentSheet()->getMultiSelect()->filterInColFromSelection(col);
         m_panelEntityGrid.RedrawAll();
      }
   }
}

void CPanelEntityGridSelectionController::clearRowHighlights()
{
   if (m_highlightedRow >= 0)
   {
      undrawRowHighlight(m_highlightedRow);

      m_highlightedRow = -1;
   }
}

void CPanelEntityGridSelectionController::highlightRow(long row)
{
   clearRowHighlights();

   m_highlightedRow = row;

   drawRowHighlight(row);
}

void CPanelEntityGridSelectionController::drawRowHighlight(long row)
{
   CUGCell cell;
   short borderStyle;
   CPen borderPen;
   borderPen.CreatePen(PS_SOLID,1,colorBlack);

   for (int col = -1;col < m_panelEntityGrid.GetNumberCols();col++)
   {
      CUGCell cell;
      m_panelEntityGrid.GetCell(col,row,&cell);

      borderStyle = UG_BDR_TTHICK | UG_BDR_BTHICK;

      if (col < 0)
      {
         borderStyle |= UG_BDR_LTHICK;
      }

      if (col == m_panelEntityGrid.GetNumberCols() - 1)
      {
         borderStyle |= UG_BDR_RTHICK;
      }

      cell.SetBorder(borderStyle);
      cell.SetBorderColor(&borderPen);

      m_panelEntityGrid.SetCell(col,row,&cell);
   }

   m_panelEntityGrid.RedrawRow(row);
}

void CPanelEntityGridSelectionController::undrawRowHighlight(long row)
{
   CUGCell cell;
   short borderStyle = 0;

   for (int col = -1;col < m_panelEntityGrid.GetNumberCols();col++)
   {
      CUGCell cell;
      m_panelEntityGrid.GetCell(col,row,&cell);

      cell.SetBorder(borderStyle);

      m_panelEntityGrid.SetCell(col,row,&cell);
   }

   m_panelEntityGrid.RedrawRow(row);
}

//_________________________________________________________________________________________________
void CPanelEntityGridMultiSelect::filterInColFromSelection(int col)
{
   UG_MSList* originalList = m_list;
   m_list = NULL;

   ClearAll();

	UG_MSList* list = NULL;
   for (list = originalList;list != NULL;list = list->next)
   {
      if (list->loCol <= col && list->hiCol >= col)
      {
         StartBlock(col,list->loRow);
         EndBlock(col,list->hiRow);
      }
   }

	UG_MSList* next;

	while (list != NULL)
	{
		next = list->next;

		delete list;

		list = next;
	}
}

//_________________________________________________________________________________________________
CPanelEntityGridDropListType::CPanelEntityGridDropListType(COLORREF buttonColor)
: m_buttonColor(buttonColor)
{
}

COLORREF CPanelEntityGridDropListType::GetButtonColor()
{
   return m_buttonColor;
}

//_________________________________________________________________________________________________
CPanelEntityGrid::CPanelEntityGrid(CPanelizer& panelizer)
: m_panelizer(panelizer)
, m_panelEntitySheets(*this)
, m_selectionController(*this)
, m_editControl(NULL)
, m_clipBoard(panelizer)
, m_defaultFont(NULL)
, m_defaultHeadingFont(NULL)
{
}

CPanelEntityGrid::~CPanelEntityGrid()
{
   //if (m_editControl != NULL && m_editControl->GetSafeHwnd() != NULL)
   //{
   //   m_editControl->DestroyWindow();
   //}

   delete m_editControl;
}

CPanelizer& CPanelEntityGrid::getPanelizer()
{
   return m_panelizer;
}

CPanelEntityGridSelectionController& CPanelEntityGrid::getSelectionController()
{
   return m_selectionController;
}

CPanelEntityClipBoard& CPanelEntityGrid::getClipBoard()
{
   return m_clipBoard;
}

CFont* CPanelEntityGrid::getDefaultFont() const
{
   return m_defaultFont;
}

CFont* CPanelEntityGrid::getDefaultHeadingFont() const
{
   return m_defaultHeadingFont;
}

void CPanelEntityGrid::OnSetup()
{
   m_defaultFont    = m_GI->m_defFont;

   CUGCell cell;
   GetHeadingDefault(&cell);
   m_defaultHeadingFont = cell.GetFont();

   m_dropListTypeId = AddCellType(&m_dropListType);

   COLORREF color0 = RGB(255,195,195);
   COLORREF color1 = RGB(255,245,195);
   COLORREF color2 = RGB(213,255,195);
   COLORREF color3 = RGB(195,255,240);
   COLORREF color4 = RGB(195,231,255);
   COLORREF color5 = RGB(197,195,255);
   COLORREF color6 = RGB(235,195,255);
   COLORREF color7 = RGB(173,105,255);  // added later

   m_panelEntitySheets.addSheet(panelEntityTypeOutline    ,"Outline"      ,color0);
   m_panelEntitySheets.addSheet(panelEntityTypePcbInstance,"PCB Instances",color1);
   m_panelEntitySheets.addSheet(panelEntityTypePcbArray   ,"PCB Arrays"   ,color2);
   m_panelEntitySheets.addSheet(panelEntityTypeFiducial   ,"Fiducials"    ,color3);
   m_panelEntitySheets.addSheet(panelEntityTypeToolingHole,"Tooling Holes",color4);

   SetTabWidth(300);

   SetSheetNumber(0);
}

CUGEdit* CPanelEntityGrid::getEditControl()
{
   if (m_editControl == NULL)
   {
      m_editControl = new CUGEdit();

      DWORD dwStyle = WS_CHILD | WS_VISIBLE;

      // create the edit control with specified style
      m_editControl->Create(dwStyle,CRect(0,0,0,0),this,320001/*somerandom_ID*/ );
      m_editControl->SetAutoSize(TRUE);
      m_editControl->m_ctrl = this;
      //m_editControl->SetBottomExtension(0);

      CWnd* editCtrl = GetEditClass();

      //if (editCtrl != NULL)
      //{
      //   CFont* font = editCtrl->GetFont();

      //   m_editControl->SetFont(font,0);
      //}
   }

   return m_editControl;
}

void CPanelEntityGrid::OnTabSelected(int tabId)
{
   m_selectionController.reset();

   SetSheetNumber(tabId);

   CPanelEntitySheet* sheet = getCurrentSheet();

   if (sheet != NULL)
   {
      sheet->emptyClipboard();
      sheet->fillInvalidGrid();
   }
}

int CPanelEntityGrid::OnMenuStart(int row,long col,int section)
{
   int retval = 0;

   CPanelEntitySheet* sheet = getCurrentSheet();
   row = GetCurrentRow();
   col = GetCurrentCol();

   if (sheet != NULL)
   {
      retval = sheet->onMenuStart(row,col,section);

      CMenu* popupMenu = GetPopupMenu();
      int itemCount = popupMenu->GetMenuItemCount();

      retval = (itemCount > 0);
   }

   return retval;
}

void CPanelEntityGrid::OnMenuCommand(int row,long col,int section,int item)
{
   CPanelEntitySheet* sheet = getCurrentSheet();
   row = GetCurrentRow();
   col = GetCurrentCol();

   if (sheet != NULL)
   {
      sheet->onMenuCommand(row,col,section,item);
   }
}

int CPanelEntityGrid::OnEditVerify( int col, long row, CWnd* edit, UINT* vcKey )
{
   bool retval = true;

   CPanelEntitySheet* sheet = getCurrentSheet();

   if (sheet != NULL)
   {
      retval = sheet->onEditVerify(col,row,edit,vcKey);
   }

   return retval;
} 

int CPanelEntityGrid::OnEditFinish(int col,long row,CWnd* edit,CString& string,BOOL cancelFlag)
{
   bool retval = true;

   CPanelEntitySheet* sheet = getCurrentSheet();

   //CRect rect;
   //getEditControl()->GetWindowRect(&rect);
   //int iii = 3;

   if (sheet != NULL)
   {
      retval = sheet->onEditFinish(col,row,edit,string,cancelFlag);

      //retval = false;  // don't overwrite the nicely formated row with the raw data from the string parameter
   }

   return retval;
}

int CPanelEntityGrid::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
   bool retval = true;

   CPanelEntitySheet* sheet = getCurrentSheet();

   if (sheet != NULL)
   {
      if (msg == UGCT_DROPLISTPOSTSELECT)
      {
         CUGCell cell;
         GetCell(col,row,&cell);

         CString cellText = cell.GetText();
         retval = sheet->onEditFinish(col,row,NULL,cellText,false);

         GotoCell(col,row);
      }
   }

   return retval;
}

void CPanelEntityGrid::OnSH_LClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed)
{
   m_selectionController.OnSH_LClicked(col,row,updn,nFlags,rect,point,processed);
}

void CPanelEntityGrid::OnSH_RClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,BOOL processed)
{
   m_selectionController.OnSH_RClicked(col,row,updn,nFlags,rect,point,processed);
}

void CPanelEntityGrid::OnSelectionChanged(int startCol,long startRow,int endCol,long endRow,int blockNum)
{
   m_selectionController.OnSelectionChanged(startCol,startRow,endCol,endRow,blockNum);
}

void CPanelEntityGrid::OnCellChange(int oldcol,int newcol,long oldrow,long newrow)
{
   m_selectionController.OnCellChange(oldcol,newcol,oldrow,newrow);
}

void CPanelEntityGrid::OnLClicked(int col,long row,int updn,UINT nFlags,RECT* rect,POINT* point,int processed)
{
   m_selectionController.OnLClicked(col,row,updn,nFlags,rect,point,processed);
}

COLORREF CPanelEntityGrid::OnGetDefBackColor(int section)
{
   COLORREF backColor;
   CPanelEntitySheet* sheet = getCurrentSheet();

	if (section == UG_GRID)
   {
		backColor = GetSysColor(COLOR_WINDOW);

      if (sheet != NULL)
      {
         backColor = sheet->getDefaultBackgroundColor();
      }
   }
	else
   {
		backColor = GetSysColor(COLOR_BTNFACE);

      if (sheet != NULL)
      {
         backColor = sheet->getHeadingBackgroundColor();
      }
   }

   return backColor;
}

CPanelEntitySheet* CPanelEntityGrid::getCurrentSheet()
{
   int sheetNumber = GetSheetNumber();

   return getSheet(sheetNumber);
}

CPanelEntitySheet* CPanelEntityGrid::getSheet(int sheetIndex)
{
   CPanelEntitySheet* sheet = m_panelEntitySheets.getAt(sheetIndex);

   return sheet;
}

void CPanelEntityGrid::OnCharDown(UINT* vcKey,BOOL processed)
{
   // start editing when the user hits a character key on a cell in the grid.  
   // Pass that key to the edit control, so it doesn't get lost
   StartEdit(*vcKey); 
} 

int CPanelEntityGrid::OnEditStart(int col,long row,CWnd** edit)
{ 
   bool retval = true;

   if (edit != NULL)
   {
      //(*edit)->ModifyStyle(0,ES_MULTILINE | ES_AUTOVSCROLL);
      *edit = getEditControl();

      CUGCell cell;
      GetCell(col,row,&cell);

      (*edit)->SetFont(cell.GetFont(),0);

      //CRect rect;
      //(*edit)->GetWindowRect(&rect);
      //int iii = 3;
   }

   CPanelEntitySheet* sheet = getCurrentSheet();

   if (sheet != NULL)
   {
      retval = sheet->onEditStart(col,row,edit);
   }

   return retval;
} 

void CPanelEntityGrid::invalidateBoardMapping()
{
   int sheetCount = m_panelEntitySheets.getCount();

   for (int sheetIndex = 0;sheetIndex < sheetCount;sheetIndex++)
   {
      CPanelEntitySheet* sheet = m_panelEntitySheets.getAt(sheetIndex);

      sheet->invalidateBoardMapping();
   }
}

void CPanelEntityGrid::fillGrid()
{
   int sheetCount = m_panelEntitySheets.getCount();

   for (int sheetIndex = 0;sheetIndex < sheetCount;sheetIndex++)
   {
      CPanelEntitySheet* sheet = m_panelEntitySheets.getAt(sheetIndex);

      sheet->fillGrid();
   }

   this->SetSheetNumber(0);
}

void CPanelEntityGrid::clearEntityHighlights()
{
   CPanelEntitySheet* sheet = getCurrentSheet();

   if (sheet != NULL)
   {
      sheet->clearEntityHighlights();
   }
}

void CPanelEntityGrid::highlightEntityFromRow(int row)
{
   CPanelEntitySheet* sheet = getCurrentSheet();

   if (sheet != NULL)
   {
      sheet->highlightEntityFromRow(row);
   }
}

void CPanelEntityGrid::highlightEntitiesFromRows(int rowStart,int rowEnd)
{
   CPanelEntitySheet* sheet = getCurrentSheet();

   if (sheet != NULL)
   {
      sheet->highlightEntitiesFromRows(rowStart,rowEnd);
   }
}

//void CPanelEntityGrid::OnRClicked(int col,long row,int updn,RECT *rect,POINT* point,BOOL processed)
//{
//}

//_________________________________________________________________________________________________
CPanelizerDialog::CPanelizerDialog(CPanelizer& panelizer)
: CResizingDialog(CPanelizerDialog::IDD,NULL)
, m_panelizer(panelizer)
, m_panelEntityGrid(panelizer)
, m_mostRecentlyCreatedPanelFile(NULL) 
{
   addFieldControl(IDC_CurrentTemplateGroup     ,anchorLeft,growHorizontal);
   addFieldControl(IDC_CurrentTemplateDescriptor,anchorLeft,growHorizontal);
   addFieldControl(IDC_PanelEntityGrid          ,anchorLeft,growBoth);
   addFieldControl(IDC_LoadButton               ,anchorRight);
   addFieldControl(IDC_SaveButton               ,anchorRight);
   addFieldControl(IDC_SaveAsButton             ,anchorRight);
   addFieldControl(IDC_InstantiateButton        ,anchorRight);
   addFieldControl(IDCANCEL                     ,anchorRight);
}

CPanelizerDialog::~CPanelizerDialog()
{
}

void CPanelizerDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_CurrentTemplateGroup, m_currentTemplateGroup);
   DDX_Control(pDX, IDC_CurrentTemplateDescriptor, m_currentTemplateDescriptorStatic);
   DDX_Control(pDX, IDC_PanelEntityGrid, m_panelEntityGrid);
   DDX_Control(pDX, IDC_LoadButton, m_loadButton);
   DDX_Control(pDX, IDC_SaveButton, m_saveButton);
   DDX_Control(pDX, IDC_SaveAsButton, m_saveAsButton);
   DDX_Control(pDX, IDC_InstantiateButton, m_instantiateButton);
   DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CPanelizerDialog, CResizingDialog)
   ON_BN_CLICKED(IDC_LoadButton, OnBnClickedLoad)
   ON_BN_CLICKED(IDC_SaveButton, OnBnClickedSave)
   ON_BN_CLICKED(IDC_SaveAsButton, OnBnClickedSaveAs)
   ON_BN_CLICKED(IDC_InstantiateButton, OnBnClickedApply)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CPanelizerDialog message handlers

void CPanelizerDialog::OnBnClickedLoad()
{
#ifdef FILE_BASED
   CFileDialog dialog(true);

   if (dialog.DoModal() == IDOK)
   {
      CString filePath = dialog.GetPathName();

      m_panelizer.getPanelTemplate().readFile(filePath);
      m_panelEntityGrid.fillGrid();
      m_panelizer.updateModifiedPanel();
   }
#endif
   CStringArray existingNames;
   CDBInterface db;
   if (db.Connect(false /*disable error popup*/))
   {
      CDBPanelTemplateList *list = db.LookupPanelTemplates(true /*names only*/);
      list->GetNameArray(existingNames);
      delete list;


      LoadChooserDlg chooserDlg(existingNames);

      CString name;

      if (chooserDlg.DoModal() == IDOK)
      {
         name = chooserDlg.GetTemplateName();
         if (name.IsEmpty())
         {
            ErrorMessage("Clicked OK with no name selected, that should not have been possible.", "Panel Template Load");
         }
         else
         {
            // LOAD IT !
            CWaitCursor pardonMeThisMightTakeAMoment;

            m_panelizer.getPanelTemplate().LoadFromDB(name);
            m_panelizer.AutoAssignPhysicalPcb();
            m_panelEntityGrid.fillGrid();
            m_panelizer.updateModifiedPanel();

            CString label( "Name: " + name );
            m_currentTemplateDescriptorStatic.SetWindowText(label);

            
         }
      }
   }
   else
   {
      CString errmsg( db.GetErrorMessage() );
      errmsg += "\n\nUse Data Doctor Library Tab to select Database File.";
      ErrorMessage(errmsg, "Panel Template Load");
   }

}

void CPanelizerDialog::OnBnClickedSave()
{
   if (m_panelizer.getPanelTemplate().GetName().IsEmpty())
   {
      OnBnClickedSaveAs();
   }
   else
   {
      // This is just to check the connection will work, so we can report error here instead of downstream
      CDBInterface db;
      if (db.Connect(false /*disable error popup*/))
      {
         CWaitCursor pardonMeThisMightTakeAMoment;
         db.Disconnect();
         m_panelizer.getPanelTemplate().SaveToDB();
      }
      else
      {
         CString errmsg( db.GetErrorMessage() );
         errmsg += "\n\nUse Data Doctor Library Tab to select Database File.";
         ErrorMessage(errmsg, "Panel Template Save");
      }
   }
}

void CPanelizerDialog::OnBnClickedSaveAs()
{
#ifdef FILE_BASED
   CFileDialog dialog(false);

   if (dialog.DoModal() == IDOK)
   {
      CString filePath = dialog.GetPathName();

      CStdioFileWriteFormat stdioFileWriteFormat;

      if (stdioFileWriteFormat.open(filePath))
      {
         m_panelizer.getPanelTemplate().write(stdioFileWriteFormat);
      }
   }
#endif
   CStringArray existingNames;
   CDBInterface db;
   if (db.Connect(false /*disable error popup*/))
   {
      CDBPanelTemplateList *list = db.LookupPanelTemplates(true /*names only*/);
      list->GetNameArray(existingNames);
      delete list;


      SaveAsChooserDlg chooserDlg(existingNames);

      CString name;

      while (name.IsEmpty() && chooserDlg.DoModal() == IDOK)
      {
         name = chooserDlg.GetTemplateName();
         if (name.IsEmpty())
            ErrorMessage("Must enter non-blank template name.", "Panel Template Save As");
      }

      ///m_panelizer.getCamCadDoc().OnClearSelected();

      if (!name.IsEmpty())  // Local panelName var can only be non-empty is user clicked OK
      {
         CWaitCursor pardonMeThisMightTakeAMoment;

         m_panelizer.getPanelTemplate().SetName(name);
         m_panelizer.getPanelTemplate().SaveToDB();

         CString label( "Name: " + name );
         m_currentTemplateDescriptorStatic.SetWindowText(label);
      }
   } 
   else
   {
      CString errmsg( db.GetErrorMessage() );
      errmsg += "\n\nUse Data Doctor Library Tab to select Database File.";
      ErrorMessage(errmsg, "Panel Template Save");
   }
}

void CPanelizerDialog::OnBnClickedApply()
{
   int proceed = IDOK;

   CString msg;
   if (!m_panelizer.getPanelTemplate().ValidateLayout(msg))
   {
      proceed = ErrorMessage("\nProceed with panel creation?", msg, MB_OKCANCEL);
   }

   if (proceed == IDOK)
   {
      CreatePanelDlg panelDlg(m_panelizer.getCamCadDoc());

      CString panelName;

      while (panelName.IsEmpty() && panelDlg.DoModal() == IDOK)
      {
         panelName = panelDlg.GetPanelName();
         if (panelName.IsEmpty())
            ErrorMessage("Must enter non-blank panel name.", "Create Panel");
      }

      //m_panelizer.getCamCadDoc().OnClearSelected();

      if (!panelName.IsEmpty())  // Local panelName var can only be non-empty if user clicked OK
      {
         // If panel by this name already exists, delete it.
         // CAMCAD in general supports multiple panel files with same exact name, names are not required to be unique.
         // Per Mark, and per DR dts0100530280, this Panel Template utility is going to enforce unique names
         // in created panels, and be non-case-sensitive. So delete ALL panel files that match this new
         // name in non-case-sensitive comparison.
         for (FileStruct *existingPanel = m_panelizer.getCamCadData().getFileList().FindByName(panelName);
            existingPanel != NULL;
            existingPanel = m_panelizer.getCamCadData().getFileList().FindByName(panelName))
         {
            m_panelizer.getCamCadData().getFileList().deleteFile(existingPanel);
         }

         // Create the new panel
         m_mostRecentlyCreatedPanelFile = m_panelizer.instantiateFile(panelName);

         // Hope this doesn't get too annoying. This pop-up is just to cause a pause so
         // the created panel can be momentarily looked ad. Upon close of this we
         // hide the panel file, otherwise it obscures the template display.
         // I believe we want to keep showing the created panel, it is flashy and shows
         // that something happened.
         ////Not Needed?   ErrorMessage("", "Panel Created", MB_OK | MB_ICONINFORMATION);
         // Mark file to be hidden, but do not redraw.
         // Next change to template will cause redraw.
         // So user can look at panel until they make a change to the template.

         {
            // Flush Events to cause draw before file is set to hidden
            MSG msg;
            BOOL LButtonUp = FALSE;
            while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
            }
         }

         m_mostRecentlyCreatedPanelFile->setShow(false);
      }
   }
}

void CPanelizerDialog::OnBnClickedCancel()
{
   // Really it is Close button

   // Close ... but first ...
   if (m_panelizer.getPanelTemplate().getModifiedSinceLoadFlag())
   {
      CString msg;
      CString templateName( !m_panelizer.getPanelTemplate().GetName().IsEmpty() ? m_panelizer.getPanelTemplate().GetName() : "<Unnamed>" );
      msg.Format("Template %s has been modified, do you want to save it?", templateName);
      int rspns = ErrorMessage(msg, "Panel Template", MB_YESNOCANCEL);
      if (rspns == IDYES || rspns == IDOK)
      {
         OnBnClickedSaveAs();
      }
      else if (rspns == IDCANCEL)
      {
         return; // Cancels close of Panelizer
      }
      // else == no or close, just proceed
   }

   // Proceed with close
   CResizingDialog::OnCancel();

   // The template will vanish, leaving a blank screen is unsightly. If one or more panels
   // have been created then show the most recent one. If not, then show something that
   // is already in the file list.
   if (this->GetMostRecentPanelFile() != NULL)
   {
      GetMostRecentPanelFile()->setShow(true);
   }
   else
   {
      FileStruct *firstPanel = NULL;
      FileStruct *firstPcb = NULL;
      POSITION pos = m_panelizer.getCamCadData().getFileList().GetHeadPosition();
      while (pos != NULL && firstPanel == NULL) // Don't check firstPcb, this causes it to favor a panel file
      {
         FileStruct *someFile = m_panelizer.getCamCadData().getFileList().GetNext(pos);
         if (someFile != NULL)
         {
            if (someFile->getBlockType() == blockTypePanel)
               firstPanel = someFile;
            else if (someFile->getBlockType() == blockTypePcb && firstPcb == NULL)
               firstPcb = someFile;
         }
      }
      
      if (firstPanel != NULL)
         firstPanel->setShow(true);
      else if (firstPcb != NULL)
         firstPcb->setShow(true);
   }

   m_panelizer.getCamCadDoc().OnFitPageToImage();
   getActiveView()->PostMessage(WM_COMMAND,ID_ZOOM_ZOOMEXTENTS);
   ////redundant... m_panelizer.getCamCadDoc().OnRedraw();
}

BOOL CPanelizerDialog::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   CString label( "Name: <Unnamed>" );
   m_currentTemplateDescriptorStatic.SetWindowText(label);

   m_panelEntityGrid.AttachGrid(this,IDC_PanelEntityGrid);

#ifdef FILE_BASED
   // Do nothing
#else
   // Disable database interaction buttons if DB connection is
   // not allowed. Note that DB connection might still be disabled.
   // Connection allowed or not is a system thing, user has no choice.
   // Connection attempt or not (when allowed) is a user choice.
   // Disable only for allowed. If allowed but not enabled, attempt
   // to use a button will give a message about using DataDoc to select
   // a DB, we want that to happen. But we don't want to go there if
   // the connection is not allowed at all.
   CDBInterface db;
   bool dbAllowed = db.GetConnectionAttemptAllowed();
   GetDlgItem(IDC_LoadButton)->EnableWindow(dbAllowed);
   GetDlgItem(IDC_SaveButton)->EnableWindow(dbAllowed);
   GetDlgItem(IDC_SaveAsButton)->EnableWindow(dbAllowed);
#endif

   return TRUE;

}


/////////////////////////////////////////////////////////////////////////////
// Create Panel Dialog

CreatePanelDlg::CreatePanelDlg(CCEtoODBDoc &doc, CWnd* pParent /*=NULL*/)
   : CDialog(CreatePanelDlg::IDD, pParent)
   , m_replaceExisting(FALSE)
{
   //{{AFX_DATA_INIT(CreatePanelDlg)
   m_panelName = _T("");
   //}}AFX_DATA_INIT

   // Collect names of all panels into map for quick lookup.
   // Actual names of files may be mixed case, there even may be multiple of same exact name.
   // Per Mark, we are stepping away from that sort of support, and instead are enforcing
   // unique names that are not case sensitive.
   // So for purposes of map filling and lookup, upcase the name.
   // If the user does an overwrite/replace for a given name, then all instances that match
   // that name in non-case-sensitive mode will be replaced by the single new panel.

   POSITION pos = doc.getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc.getFileList().GetNext(pos);
      if (file != NULL && file->getBlockType() == blockTypePanel)
      {
         // count keeps track of how many individual file names match this upper case name

         CString upperName( file->getName() );
         upperName.MakeUpper();

         int count = 1;
         if (m_nameMap.Lookup(upperName, count))
            count++;

         m_nameMap.SetAt(upperName, count);
      }
   }

}

CreatePanelDlg::~CreatePanelDlg()
{
   m_nameMap.RemoveAll();
}

void CreatePanelDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Text(pDX, IDC_PANEL_NAME, m_panelName);
   DDX_Check(pDX, IDC_CHK_REPLACE_EXISTING_PANEL, m_replaceExisting);
}


BEGIN_MESSAGE_MAP(CreatePanelDlg, CDialog)
   ON_EN_CHANGE(IDC_PANEL_NAME, OnEnChangePanelName)
   ON_BN_CLICKED(IDC_CHK_REPLACE_EXISTING_PANEL, OnBnClickedReplaceExisting)
END_MESSAGE_MAP()

BOOL CreatePanelDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   OnEnChangePanelName();

   return TRUE;
}

void CreatePanelDlg::EnableOkButton()
{
   int junk = 0; // Don't care about mapped-to value, presence or not in map is all that matters

   CString upperName( m_panelName );
   upperName.MakeUpper();

   if (m_panelName.IsEmpty() || 
      (m_nameMap.Lookup(upperName, junk) && !m_replaceExisting))
   {
      GetDlgItem(IDOK)->EnableWindow(FALSE);
   }
   else
   {
      GetDlgItem(IDOK)->EnableWindow(TRUE);
   }

}

void CreatePanelDlg::OnEnChangePanelName()
{
   UpdateData();

   // Don't allow leading blanks, but we have to allow trailing blanks because user
   // might not be finished entering name.
   CString cleanName( m_panelName );
   cleanName.TrimLeft(" ");
   if (cleanName.Compare(m_panelName) != 0)
   {
      m_panelName = cleanName;
      UpdateData(FALSE);
   }

   int count = 0;
   CString upperName( m_panelName );
   upperName.MakeUpper();

   // Init as standard singular label
   CString label("Replace existing panel");

   if (m_nameMap.Lookup(upperName, count))
   {
      // Adjust checkbox label to indicate number of panels being replaced
      if (count != 1)
      {
         // Make plural and add count
         label.Format("Replace existing panels (%d)", count);
      }

      // Set label
      GetDlgItem(IDC_CHK_REPLACE_EXISTING_PANEL)->SetWindowText(label);

      // Enable overwrite checkbox
      GetDlgItem(IDC_CHK_REPLACE_EXISTING_PANEL)->EnableWindow(TRUE);
   }
   else
   {
      // (Re)Set label
      GetDlgItem(IDC_CHK_REPLACE_EXISTING_PANEL)->SetWindowText(label);

      // Disable overwrite checkbox
      GetDlgItem(IDC_CHK_REPLACE_EXISTING_PANEL)->EnableWindow(FALSE);

      // Clear value, so if it becomes enabled again user has to take action to overwrite existing
      m_replaceExisting = FALSE;
      UpdateData(FALSE);
   }

   EnableOkButton();
}


void CreatePanelDlg::OnBnClickedReplaceExisting()
{
   UpdateData();

   EnableOkButton();
}


/////////////////////////////////////////////////////////////////////////////
// SaveAs Chooser Dialog

SaveAsChooserDlg::SaveAsChooserDlg(CStringArray &existingNames, CWnd* pParent /*=NULL*/)
   : CDialog(SaveAsChooserDlg::IDD, pParent)
   , m_replaceExisting(FALSE)
{
   //{{AFX_DATA_INIT(SaveAsChooserDlg)
   m_templateName = _T("");
   //}}AFX_DATA_INIT

   // Collect names into map for quick lookup
   for (int i = 0; i < existingNames.GetSize(); i++)
   {
      CString name(existingNames.GetAt(i));
      if (!name.IsEmpty())
      {
         m_nameMap.SetAt(name, 0);
      }
   }
}

SaveAsChooserDlg::~SaveAsChooserDlg()
{
   m_nameMap.RemoveAll();
}

void SaveAsChooserDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_templateName);
   DDX_Check(pDX, IDC_CHK_REPLACE_EXISTING, m_replaceExisting);
   DDX_Control(pDX, IDC_LIST_EXISTING_NAMES, m_lstExistingNames);
}


BEGIN_MESSAGE_MAP(SaveAsChooserDlg, CDialog)
   ON_EN_CHANGE(IDC_EDIT_TEMPLATE_NAME, OnEnChangePanelName)
   ON_BN_CLICKED(IDC_CHK_REPLACE_EXISTING, OnBnClickedReplaceExisting)
   ON_LBN_SELCHANGE(IDC_LIST_EXISTING_NAMES, OnLbnSelchangeExistingNamesList)
END_MESSAGE_MAP()

BOOL SaveAsChooserDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // Put names in listbox for user selection.
   POSITION pos = m_nameMap.GetStartPosition();
   while (pos != NULL)
   {
      CString name;
      int junk;
      m_nameMap.GetNextAssoc(pos, name, junk);
      m_lstExistingNames.AddString(name);
   }

   OnEnChangePanelName();

   return TRUE;
}

void SaveAsChooserDlg::EnableOkButton()
{
   int junk = 0; // Don't care about mapped-to value, presence or not in map is all that matters

   if (m_templateName.IsEmpty() || 
      (m_nameMap.Lookup(m_templateName, junk) && !m_replaceExisting))
   {
      GetDlgItem(IDOK)->EnableWindow(FALSE);
   }
   else
   {
      GetDlgItem(IDOK)->EnableWindow(TRUE);
   }

}

void SaveAsChooserDlg::OnEnChangePanelName()
{
   UpdateData();

   // Don't allow leading blanks, but we have to allow trailing blanks because user
   // might not be finished entering name.
   CString cleanName( m_templateName );
   cleanName.TrimLeft(" ");
   if (cleanName.Compare(m_templateName) != 0)
   {
      m_templateName = cleanName;
      UpdateData(FALSE);
   }

   int junk = 0; // Don't care about mapped-to value, presence or not in map is all that matters

   if (m_nameMap.Lookup(m_templateName, junk))
   {
      // Enable overwrite checkbox
      GetDlgItem(IDC_CHK_REPLACE_EXISTING)->EnableWindow(TRUE);
   }
   else
   {
      // Disable overwrite checkbox
      GetDlgItem(IDC_CHK_REPLACE_EXISTING)->EnableWindow(FALSE);

      // Clear value, so if it becomes enabled again user has to take action to overwrite existing
      m_replaceExisting = FALSE;
      UpdateData(FALSE);
   }

   EnableOkButton();
}

void SaveAsChooserDlg::OnLbnSelchangeExistingNamesList()
{
   int indx = m_lstExistingNames.GetCurSel();
   if (indx != LB_ERR)
   {
      CString selectedName;
      m_lstExistingNames.GetText(indx, selectedName);
      m_templateName = selectedName;
      UpdateData(FALSE);
      OnEnChangePanelName();
   }
}

void SaveAsChooserDlg::OnBnClickedReplaceExisting()
{
   UpdateData();

   EnableOkButton();
}


/////////////////////////////////////////////////////////////////////////////
// Load Chooser Dialog

LoadChooserDlg::LoadChooserDlg(CStringArray &existingNames, CWnd* pParent /*=NULL*/)
   : CDialog(LoadChooserDlg::IDD, pParent)
   ////, m_replaceExisting(FALSE)
{
   //{{AFX_DATA_INIT(SaveAsChooserDlg)
   m_templateName = _T("");
   //}}AFX_DATA_INIT

   // Collect names into map for quick lookup
   for (int i = 0; i < existingNames.GetSize(); i++)
   {
      CString name(existingNames.GetAt(i));
      if (!name.IsEmpty())
      {
         m_nameMap.SetAt(name, 0);
      }
   }
}

LoadChooserDlg::~LoadChooserDlg()
{
   m_nameMap.RemoveAll();
}

void LoadChooserDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_LIST_EXISTING_NAMES, m_lstExistingNames);
}


BEGIN_MESSAGE_MAP(LoadChooserDlg, CDialog)
   ON_LBN_SELCHANGE(IDC_LIST_EXISTING_NAMES, OnLbnSelchangeExistingNamesList)
   //ON_LBN_DBLCLK(IDC_LIST_EXISTING_NAMES, OnLButtonDblClk)
END_MESSAGE_MAP()

BOOL LoadChooserDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // Put names in listbox for user selection.
   POSITION pos = m_nameMap.GetStartPosition();
   while (pos != NULL)
   {
      CString name;
      int junk;
      m_nameMap.GetNextAssoc(pos, name, junk);
      m_lstExistingNames.AddString(name);
   }

   EnableOkButton();

   return TRUE;
}

/*void LoadChooserDlg::OnLButtonDblClk()
{
   int indx = m_lstExistingNames.GetCurSel();
   if (indx != LB_ERR)
   {
      CString selectedName;
      m_lstExistingNames.GetText(indx, selectedName);
      m_templateName = selectedName;

      if (!m_templateName.IsEmpty())
      {
         EndDialog(IDOK);
      }
   }
}*/

void LoadChooserDlg::EnableOkButton()
{
   int junk = 0; // Don't care about mapped-to value, presence or not in map is all that matters

   if (m_templateName.IsEmpty())
   {
      GetDlgItem(IDOK)->EnableWindow(FALSE);
   }
   else
   {
      GetDlgItem(IDOK)->EnableWindow(TRUE);
   }

}

void LoadChooserDlg::OnLbnSelchangeExistingNamesList()
{
   int indx = m_lstExistingNames.GetCurSel();
   if (indx != LB_ERR)
   {
      CString selectedName;
      m_lstExistingNames.GetText(indx, selectedName);
      m_templateName = selectedName;
      EnableOkButton();
   }
}

