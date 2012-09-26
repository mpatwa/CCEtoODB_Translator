// $Header: /CAMCAD/4.6/StencilStepper.cpp 20    2/14/07 4:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "StdAfx.h"
#include "Resource.h"
#include "CcDoc.h"
#include "StencilStepper.h"
#include ".\stencilstepper.h"
#include "PersistantColorDialog.h"

#define QThickness    "Thickness"
#define QColor        "Color"
#define QDepositCount "Deposit Count"

//_____________________________________________________________________________
CStencilDepositMap::CStencilDepositMap(bool isContainer) :
   CTypedMapPtrToPtrContainer<DataStruct*,double*>(10,isContainer)
{
}

//_____________________________________________________________________________
CStencilStep::CStencilStep(double thickness,COLORREF color) :
   m_thickness(thickness),
   m_color(color),
   m_depositMap(false)
{
   m_depositMap.InitHashTable(nextPrime2n(5000));
}

void CStencilStep::empty()
{
   m_depositMap.empty();
}

CString CStencilStep::getThicknessString() const
{
   CString retval = fpfmt(m_thickness,4);

   return retval;
}

int CStencilStep::getDepositCount() const
{
   return m_depositMap.GetCount();
}

CString CStencilStep::getDepositCountString() const
{
   CString retval;

   retval.Format("%d",getDepositCount());

   return retval;
}

void CStencilStep::setColor(COLORREF color)
{
   m_color = color;

   DataStruct* deposit;
   double* thickness;

   for (POSITION pos = m_depositMap.GetStartPosition();pos != NULL;)
   {
      m_depositMap.GetNextAssoc(pos,deposit,thickness);

      deposit->setOverrideColor(m_color);
   }
}

void CStencilStep::setAt(DataStruct* deposit)
{
   m_depositMap.SetAt(deposit,NULL);
}

void CStencilStep::removeAt(DataStruct* deposit)
{
   m_depositMap.RemoveKey(deposit);
}

//_____________________________________________________________________________
CStencilSteps::CStencilSteps()
{
   m_colorMap.InitHashTable(nextPrime2n(100));
}

void CStencilSteps::empty()
{
   m_stepList.empty();
   m_colorMap.RemoveAll();
}

void CStencilSteps::emptySteps()
{
   for (POSITION pos = m_stepList.GetHeadPosition();pos != NULL;)
   {
      CStencilStep* step = m_stepList.GetNext(pos);
      step->empty();
   }
}

CStencilStep& CStencilSteps::updateThickness(DataStruct* deposit,double thickness)
{
   double* depositThickness;

   if (m_deposits.Lookup(deposit,depositThickness))
   {
      getStep(*depositThickness).removeAt(deposit);
      *depositThickness = thickness;
      m_deposits.SetAt(deposit,depositThickness);
   }
   else
   {
      depositThickness = new double(thickness);
      m_deposits.SetAt(deposit,depositThickness);
   }

   CStencilStep& step = getStep(thickness);
   step.setAt(deposit);

   return step;
}

int CStencilSteps::getStepCount()
{
   return m_stepList.GetCount();
}

POSITION CStencilSteps::getHeadStepPosition()
{
   return m_stepList.GetHeadPosition();
}

CStencilStep* CStencilSteps::getNextStep(POSITION& pos)
{
   return m_stepList.GetNext(pos);
}

COLORREF CStencilSteps::getNextStepColor()
{
   COLORREF colorTable[] =
   {
      colorBlue       ,
      colorGreen      ,
      colorCyan       ,
      colorRed        ,
      colorMagenta    ,
      colorYellow     ,
      colorDkBlue     ,
      colorDkGreen    ,
      colorDkCyan     ,
      colorDkRed      ,
      colorDkMagenta  ,
      colorDkYellow   ,
      0
   };

   COLORREF nextStepColor = colorRed;
   int minColorCount = 1000;
   void* voidColorCount;

   for (int index = 0;;index++)
   {
      COLORREF color = colorTable[index];
      int colorCount = 0;
      
      if (m_colorMap.Lookup((void*)color,voidColorCount))
      {
         colorCount = (unsigned int)voidColorCount;
      }

      if (colorCount < minColorCount)
      {
         minColorCount = colorCount;
         nextStepColor = color;
      }

      if (color == 0 || minColorCount == 0)
      {
         break;
      }
   }

   if (m_colorMap.Lookup((void*)nextStepColor,voidColorCount))
   {
      voidColorCount = (void*)((int)voidColorCount + 1);
   }
   else
   {
      voidColorCount = (void*)(1);
   }

   m_colorMap.SetAt((void*)nextStepColor,voidColorCount);

   return nextStepColor;
}

CStencilStep* CStencilSteps::getNewStep(double thickness)
{
   COLORREF color = getNextStepColor();

   CStencilStep* step = new CStencilStep(thickness,color);

   return step;
}

CStencilStep& CStencilSteps::getStep(double thickness)
{
   CStencilStep* step = NULL;

   for (POSITION pos = m_stepList.GetHeadPosition();;)
   {
      if (pos == NULL)
      {
         step = getNewStep(thickness);
         m_stepList.AddTail(step);
         break;
      }         
      else
      {
         POSITION oldPos = pos;
         step = m_stepList.GetNext(pos);

         if (fpeq(step->getThickness(),thickness))
         {
            break;
         }
         else if (step->getThickness() > thickness)
         {
            step = getNewStep(thickness);
            m_stepList.InsertBefore(oldPos,step);
            break;
         }
      }
   }

   return *step;
}

void CStencilSteps::setColorOverride(bool colorOverrideFlag)
{
   double* thickness;
   DataStruct* deposit;

   for (POSITION pos = m_deposits.GetStartPosition();pos != NULL;)
   {
      m_deposits.GetNextAssoc(pos,deposit,thickness);

      deposit->setColorOverride(colorOverrideFlag);
   }
}

void CStencilSteps::removeStencilStepperAttributes(CCamCadDatabase& camCadDatabase,int stencilThicknessKeywordIndex)
{
   double* thickness;
   DataStruct* deposit;

   for (POSITION pos = m_deposits.GetStartPosition();pos != NULL;)
   {
      m_deposits.GetNextAssoc(pos,deposit,thickness);

      camCadDatabase.removeAttribute(&(deposit->getAttributesRef()),stencilThicknessKeywordIndex);
   }
}

//_____________________________________________________________________________
CStencilStepperGrid::CStencilStepperGrid(CStencilStepperDialog& parentDialog)
: m_parentDialog(parentDialog)
, m_currentCol(-1)
, m_currentRow(-1)
{
}

void CStencilStepperGrid::ExpandLastColumnToFit()
{
   SizeLastColumnToFit(true);
}

void CStencilStepperGrid::SizeLastColumnToFit(bool expandOnlyFlag)
{
   CRect clientRect;
   GetClientRect(&clientRect);

   int gridWidth = clientRect.right - clientRect.left;
   int columnWidth;
   int columnsWidth = 0;

   for (int columnIndex = -GetSH_NumberCols();columnIndex <  GetNumberCols();columnIndex++)
   {
      GetColWidth(columnIndex,&columnWidth);
      columnsWidth += columnWidth;
   }

   int deltaWidth = gridWidth - columnsWidth;

   if (!expandOnlyFlag || deltaWidth > 0)
   {
      int lastColumnIndex = GetNumberCols() - 1;
      GetColWidth(lastColumnIndex,&columnWidth);
      columnWidth += deltaWidth;

      if (columnWidth < 0)
      {
         columnWidth = 0;
      }

      SetColWidth(lastColumnIndex,columnWidth);
   }
}

void CStencilStepperGrid::OnLClicked(int col,long row,int updn,RECT* rect,POINT* point,BOOL processed)
{
   if (updn == 1)
   {
      int dCol = col - m_currentCol;
      int dRow = row - m_currentRow;

      if (col == m_currentCol && row == m_currentRow)
      {
         StartEdit();
      }
      else if (col >= 0 && row >= 0)
      {
         m_parentDialog.nonFixedCellClick(row,col);
      }

      m_currentCol = col;
      m_currentRow = row;
   }
}

int CStencilStepperGrid::OnEditStart(int col,long row,CWnd** edit)
{
   if (m_parentDialog.editCell(row,col))
   {
      RedrawCell(col,row);
   }

   return false;
}

//_____________________________________________________________________________
//IMPLEMENT_DYNAMIC(CStencilStepperDialog, CResizingDialog)
//CStencilStepperDialog::CStencilStepperDialog(CWnd* pParent /*=NULL*/)
// : CResizingDialog(CStencilStepperDialog::IDD, pParent)
//{
//}

CStencilStepperDialog::CStencilStepperDialog(CCEtoODBDoc& camCadDoc)
: CResizingDialog(CStencilStepperDialog::IDD)
, m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
, m_stepGrid(*this)
, m_toolBar(*this)
{
   m_pcbFile = m_camCadDatabase.getSingleVisibleFile();
   m_stencilBaseThicknessKeywordIndex = -1;
   m_stencilThicknessKeywordIndex     = -1;
   m_stencilColorsKeywordIndex        = -1;
   m_selectedLayerIndex = -1;
   m_showColorFlag      = true;

   addFieldControl(IDC_StencilStepsStatic,anchorLeft,growBoth);
   addFieldControl(IDC_StepGrid          ,anchorLeft,growBoth);
}

CStencilStepperDialog::~CStencilStepperDialog()
{
}

void CStencilStepperDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_AddPadsToStep, m_addPadsToStepButton);
   DDX_Control(pDX, IDC_SelectedPasteDepositsStatic, m_depositCountStatic);
   DDX_Control(pDX, IDC_BaseStencilThicknessStatic, m_baseThicknessStatic);
   DDX_Control(pDX, IDC_PasteLayers, m_pasteLayersComboBox);
   DDX_Control(pDX, IDC_BaseStencilThickness, m_baseThicknessEditBox);
   DDX_Control(pDX, IDC_StencilThickness, m_stencilThicknessEditBox);
   //DDX_GridControl(pDX, IDC_StepGrid, m_stepGrid);
}

CCamCadDatabase& CStencilStepperDialog::getCamCadDatabase()
{
   return m_camCadDatabase;
}

int CStencilStepperDialog::getStencilBaseThicknessKeywordIndex()
{
   if (m_stencilBaseThicknessKeywordIndex < 0)
   {
      CMessageBoxWriteFormat errorLog;

      m_stencilBaseThicknessKeywordIndex = 
         getCamCadDatabase().registerKeyWord("StencilBaseThickness",0,valueTypeUnitDouble,errorLog);
   }

   return m_stencilBaseThicknessKeywordIndex;
}

int CStencilStepperDialog::getStencilThicknessKeywordIndex()
{
   if (m_stencilThicknessKeywordIndex < 0)
   {
      CMessageBoxWriteFormat errorLog;

      m_stencilThicknessKeywordIndex = 
         getCamCadDatabase().registerKeyWord("StencilThickness",0,valueTypeUnitDouble,errorLog);
   }

   return m_stencilThicknessKeywordIndex;
}

int CStencilStepperDialog::getStencilColorsKeywordIndex()
{
   if (m_stencilColorsKeywordIndex < 0)
   {
      CMessageBoxWriteFormat errorLog;

      m_stencilColorsKeywordIndex = 
         getCamCadDatabase().registerKeyWord("StencilColors",0,valueTypeString,errorLog);
   }

   return m_stencilColorsKeywordIndex;
}

void CStencilStepperDialog::readParameters()
{
   if (getPcbFile() != NULL)
   {
      BlockStruct* pcbBlock = getPcbFile()->getBlock();
      Attrib* attrib = NULL;

      if (pcbBlock->attributes().Lookup(getStencilBaseThicknessKeywordIndex(),attrib))
      {
         m_baseThickness = attrib->getDoubleValue();
      }

      if (pcbBlock->attributes().Lookup(getStencilColorsKeywordIndex(),attrib))
      {
         m_stepColors = m_camCadDatabase.getAttributeStringValue(attrib);
      }
   }
}

void CStencilStepperDialog::writeParameters()
{
   if (getPcbFile() != NULL)
   {
      CMessageBoxWriteFormat errorLog;

      BlockStruct* pcbBlock = getPcbFile()->getBlock();

      m_camCadDatabase.addAttribute(&(pcbBlock->getAttributesRef()),
                                     getStencilBaseThicknessKeywordIndex(),(float)m_baseThickness,
                                     errorLog);

      m_camCadDatabase.addAttribute(&(pcbBlock->getAttributesRef()),
                                     getStencilColorsKeywordIndex(),getStepColors());
   }
}

CString CStencilStepperDialog::getStepColors()
{
   m_stepColors.Empty();

   for (POSITION pos = m_stencilSteps.getHeadStepPosition();pos != NULL;)
   {
      CStencilStep* step = m_stencilSteps.getNextStep(pos);

      //if (step->getDepositCount() > 0)
      //{
         CString colorString;
         colorString.Format("%s=0x%08x",step->getThicknessString(),step->getColor());

         if (! m_stepColors.IsEmpty())
         {
            m_stepColors += "|";
         }

         m_stepColors += colorString;
      //}
   }

   return m_stepColors;
}

CString CStencilStepperDialog::setStepColors(const CString& stepColors)
{
   CSupString colorsString(stepColors);
   CStringArray params;
   char* endChar;
   int colorIndex = 0;

   int numPar = colorsString.Parse(params,"|");

   m_stepColors = stepColors;

   for (int colorIndex = 0;colorIndex < numPar;colorIndex++)
   {
      CSupString colorString(params.GetAt(colorIndex));
      CStringArray colorParams;

      int numColorPar = colorString.Parse(colorParams,"=");

      if (numColorPar > 1)
      {
         double thickness = atof(colorParams.GetAt(0));
         COLORREF color = strtoul(colorParams.GetAt(1),&endChar,16);

         if (*endChar == '\0')
         {
            CStencilStep& step = m_stencilSteps.getStep(thickness);
            step.setColor(color);
         }
      }
   }

   return m_stepColors;
}

void CStencilStepperDialog::scanForLayers()
{
   int selectedIndex = -1;

   for (int layerIndex = 0;layerIndex < m_camCadDoc.getLayerCount();layerIndex++)
   {
      LayerStruct* layer = m_camCadDoc.getLayerAt(layerIndex);

      if (layer != NULL)
      {
         if (layer->getType() == layerTypePasteTop    ||
             layer->getType() == layerTypePasteBottom ||
             layer->getType() == layerTypeStencilTop  ||
             layer->getType() == layerTypeStencilBottom ||
             layer->getType() == layerTypePasteAll       )
         {
            int itemIndex = m_pasteLayersComboBox.AddString(layer->getName());
            m_pasteLayersComboBox.SetItemData(itemIndex,(DWORD_PTR)layer->getLayerIndex());

            if (selectedIndex < 0 && layer->isVisible())
            {
               selectedIndex = itemIndex;
            }
         }
      }
   }

   m_pasteLayersComboBox.SetCurSel(selectedIndex);
}

void CStencilStepperDialog::updateDepositData(bool repaintFlag)
{
   changePasteLayer(getSelectedLayerIndex(),repaintFlag);
}

void CStencilStepperDialog::changePasteLayer(int pasteLayerIndex,bool repaintFlag)
{
   int depositCount = 0;
   m_stencilSteps.emptySteps();
   bool encounteredOverrideOnFlag  = false;
   bool encounteredOverrideOffFlag = false;

   if (pasteLayerIndex >= 0)
   {
      for (CDataListIterator depositIterator(*(getPcbFile()->getBlock()),dataTypeInsert);
              depositIterator.hasNext();)
      {
         DataStruct* deposit = depositIterator.getNext();

         if (deposit->getLayerIndex() == pasteLayerIndex)
         {
            Attrib* attrib;
            double thickness = m_baseThickness;

            if (deposit->lookUpAttrib(getStencilThicknessKeywordIndex(),attrib))
            {
               thickness = attrib->getDoubleValue();
            }

            CStencilStep& step = m_stencilSteps.updateThickness(deposit,thickness);

            if (deposit->getColorOverride())
            {
               encounteredOverrideOnFlag = true;
            }
            else
            {
               encounteredOverrideOffFlag = true;
            }

            deposit->setColorOverride(m_showColorFlag);
            deposit->setOverrideColor(step.getColor());
         }
      }
   }

   if (m_showColorFlag && encounteredOverrideOffFlag ||
      !m_showColorFlag && encounteredOverrideOnFlag  ||
       repaintFlag)
   {
      repaint();
   }

   fillGrid();
}

void CStencilStepperDialog::setVisibleLayer(int layerindex)
{
   for (int idx = 0; idx < m_camCadDoc.getMaxLayerIndex(); idx++)
   {
      LayerStruct *layer = m_camCadDoc.getLayer(idx);

      if (layer != NULL)
      {
         layer->setVisible((layerindex == layer->getLayerIndex()));
      }
   }
   m_camCadDoc.UpdateAllViews(NULL);
}

void CStencilStepperDialog::initGrid()
{
   UINT headerFormat = DT_LEFT|DT_WORDBREAK;

   //if (m_stepGrid.GetColumnCount() != 3)
   //{
   //   m_stepGrid.DeleteAllItems();
   //   m_stepGrid.SetColumnCount(3);
   //   m_stepGrid.SetRowCount(1);
   //   m_stepGrid.SetItemText(0,m_colThickness   ,QThickness);
   //   m_stepGrid.SetItemText(0,m_colColor       ,QColor);
   //   m_stepGrid.SetItemText(0,m_colDepositCount,QDepositCount);
   //   m_stepGrid.SetFixedRowCount(1);
   //}

   //m_stepGrid.DeleteNonFixedRows();

   if (m_stepGrid.GetNumberCols() != 3)
   {
      int height;
      m_stepGrid.GetRowHeight(-1,&height);

      height = (height * 3)/2;

      m_stepGrid.SetTH_Height(height);
      m_stepGrid.SetSH_Width(0);
      m_stepGrid.SetNumberRows(0,false);
      m_stepGrid.SetNumberCols(3,false);
      m_stepGrid.QuickSetText(m_colThickness   ,-1,QThickness);
      m_stepGrid.QuickSetText(m_colColor       ,-1,QColor);
      m_stepGrid.QuickSetText(m_colDepositCount,-1,QDepositCount);
      m_stepGrid.QuickSetAlignment(m_colDepositCount,-1,UG_ALIGNLEFT | UG_ALIGNVCENTER);
      //m_stepGrid.BestFit(0,2,1,UG_BESTFIT_TOPHEADINGS);
      //m_stepGrid.LockRows(1);
   }

   m_stepGrid.SetNumberRows(0);
}

void CStencilStepperDialog::OnUgGridDClicked(CStencilStepperGrid* grid,int col,long row,RECT* rect,POINT* point,BOOL processed)
{
   if (row >= 0)
   {
      if (col == m_colColor)
      {
      }
   }
}

void CStencilStepperDialog::fillGrid()
{
   initGrid();
   //UINT readOnlyState = GVIS_READONLY;

   for (POSITION pos = m_stencilSteps.getHeadStepPosition();pos != NULL;)
   {
      CStencilStep* step = m_stencilSteps.getNextStep(pos);

      if (step->getDepositCount() > 0)
      {
         //int rowIndex = m_stepGrid.InsertRow("xxx");

         //m_stepGrid.SetItemText(rowIndex,m_colThickness,step->getThicknessString());
         //m_stepGrid.SetItemState(rowIndex,m_colThickness,readOnlyState);

         //m_stepGrid.SetItemBkColour(rowIndex,m_colColor,step->getColor());

         //m_stepGrid.SetItemText(rowIndex,m_colDepositCount,step->getDepositCountString());
         //m_stepGrid.SetItemState(rowIndex,m_colDepositCount,readOnlyState);

         int rowIndex = m_stepGrid.GetNumberRows();
         m_stepGrid.AppendRow();

         CUGCell cell;

         m_stepGrid.GetCell(m_colThickness,rowIndex,&cell);
         cell.SetText(step->getThicknessString());
         cell.SetReadOnly(true);
         m_stepGrid.SetCell(m_colThickness,rowIndex,&cell);

         m_stepGrid.GetCell(m_colDepositCount,rowIndex,&cell);
         cell.SetText(step->getDepositCountString());
         cell.SetReadOnly(true);
         m_stepGrid.SetCell(m_colDepositCount,rowIndex,&cell);

         m_stepGrid.GetCell(m_colColor,rowIndex,&cell);
         cell.SetText("");
         cell.SetReadOnly(false);
         cell.SetBackColor(step->getColor());
         m_stepGrid.SetCell(m_colColor,rowIndex,&cell);
      }
   }

   m_stepGrid.BestFit(0,2,m_stencilSteps.getStepCount(),UG_BESTFIT_TOPHEADINGS);
   m_stepGrid.SizeLastColumnToFit();
   m_stepGrid.RedrawAll();

   //m_stepGrid.AutoSize();
   //m_stepGrid.SetRowHeight(0, 3*m_stepGrid.GetRowHeight(0)/2);
   //m_stepGrid.ExpandLastColumnToFit();
}

int CStencilStepperDialog::getSelectedLayerIndex()
{
   int selectedLayerIndex = -1;

   int itemIndex = m_pasteLayersComboBox.GetCurSel();

   if (itemIndex >= 0)
   {
      selectedLayerIndex = m_pasteLayersComboBox.GetItemData(itemIndex);
   }

   return selectedLayerIndex;
}

bool CStencilStepperDialog::editCell(int row,int col)
{
   bool retval = false;

   if (col == m_colColor && row >= 0)
   {
      retval = true;

      CPersistantColorDialog colorDialog;
      colorDialog.m_cc.Flags |= CC_FULLOPEN;

      if (colorDialog.DoModal() == IDOK)
      {
         COLORREF color = colorDialog.GetColor();

         //m_stepGrid.SetItemBkColour(row,col,color);
         m_stepGrid.QuickSetBackColor(col,row,color);

         //double thickness = atof(m_stepGrid.GetItemText(row,m_colThickness));
         CUGCell cell;
         m_stepGrid.GetCell(m_colThickness,row,&cell);
         double thickness = atof(cell.GetText());
         CStencilStep& step = m_stencilSteps.getStep(thickness);
         step.setColor(color);

         repaint();
         writeParameters();
      }
   }

   return retval;
}

void CStencilStepperDialog::nonFixedCellClick(int row, int col)
{
   //double thickness = atof(m_stepGrid.GetItemText(row,m_colThickness));
   CUGCell cell;
   m_stepGrid.GetCell(m_colThickness,row,&cell);
   double thickness = atof(cell.GetText());
   CStencilStep& step = m_stencilSteps.getStep(thickness);

   m_stencilThicknessEditBox.SetWindowText(step.getThicknessString());
}

void CStencilStepperDialog::updateSelection()
{
   CSelectList& selectList = m_camCadDoc.SelectList;
   int selectedDepositCount = 0;
   int selectedLayerIndex   = getSelectedLayerIndex();

   for (POSITION pos = selectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* selectStruct = selectList.GetNext(pos);
      DataStruct* data = selectStruct->getData();

      if (data->getDataType()   == dataTypeInsert &&
          data->getLayerIndex() == selectedLayerIndex)
      {
         selectedDepositCount++;
      }
   }

   CString countString;
   countString.Format("Selected Paste Deposits: %d",selectedDepositCount);
   m_depositCountStatic.SetWindowText(countString);
}

void CStencilStepperDialog::setBaseStencilThickness(CString baseStencilThicknessString)
{
   baseStencilThicknessString.Trim();
   char* endChar;

   double baseStencilThickness = strtod(baseStencilThicknessString,&endChar);

   if (baseStencilThicknessString.IsEmpty() || *endChar != '\0')
   {
      baseStencilThickness = -1.;
   }

   setBaseStencilThickness(baseStencilThickness);
}

void CStencilStepperDialog::setBaseStencilThickness(double baseStencilThickness)
{
   m_baseThickness = baseStencilThickness;

   if (m_baseThickness < m_minBaseStencilThickness)
   {
      m_baseThickness = m_minBaseStencilThickness;
   }
   else if (m_baseThickness > m_maxBaseStencilThickness)
   {
      m_baseThickness = m_maxBaseStencilThickness;
   }

   m_baseThicknessEditBox.SetWindowText(fpfmt(m_baseThickness,4));

   updateDepositData();
}

void CStencilStepperDialog::updateShowOverrideColor()
{
   m_toolBar.GetToolBarCtrl().HideButton(IDC_ShowColorOverride, m_showColorFlag);
   m_toolBar.GetToolBarCtrl().HideButton(IDC_HideColorOverride,!m_showColorFlag);
}

void CStencilStepperDialog::repaint()
{
   m_camCadDoc.OnRedraw();
}

void CStencilStepperDialog::removeStencilStepperAttributes()
{
   if (getPcbFile() != NULL)
   {
      CMessageBoxWriteFormat errorLog;

      BlockStruct* pcbBlock = getPcbFile()->getBlock();

      m_camCadDatabase.removeAttribute(&(pcbBlock->getAttributesRef()),
                                     getStencilBaseThicknessKeywordIndex());

      m_camCadDatabase.removeAttribute(&(pcbBlock->getAttributesRef()),
                                     getStencilColorsKeywordIndex());
   }

   m_stencilSteps.removeStencilStepperAttributes(m_camCadDatabase,getStencilThicknessKeywordIndex());
}

void CStencilStepperDialog::clearMarked()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_QUERY_CLEAR_SELECTED);
   }
}

BEGIN_MESSAGE_MAP(CStencilStepperDialog, CResizingDialog)
   ON_BN_CLICKED(IDC_AddPadsToStep, OnBnClickedAddPadsToStep)
   ON_WM_CLOSE()
   ON_EN_KILLFOCUS(IDC_BaseStencilThickness, OnEnKillFocusBaseStencilThickness)

   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)

   // toolbar buttons
   ON_BN_CLICKED(IDC_Repaint, OnBnClickedRepaint)
   ON_COMMAND(IDC_MarkByWindow, OnBnClickedMarkByWindow)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindow, OnUpdateMarkByWindow)
   ON_COMMAND(IDC_MarkByWindowCross, OnBnClickedMarkByWindowCross)
   ON_UPDATE_COMMAND_UI(IDC_MarkByWindowCross, OnUpdateMarkByWindowCross)
   ON_COMMAND(IDC_ClearMarked, OnBnClickedClearMarked)
   ON_UPDATE_COMMAND_UI(IDC_ClearMarked, OnUpdateClearMarked)
   ON_COMMAND(IDC_ShowColorOverride, OnBnClickedShowOverrideColor)
   ON_COMMAND(IDC_HideColorOverride, OnBnClickedHideOverrideColor)
   ON_CBN_SELCHANGE(IDC_PasteLayers, OnCbnSelchangePasteLayers)
   ON_BN_CLICKED(IDC_RemoveStencilStepperAttributes, OnBnClickedRemoveStencilStepperAttributes)
   ON_WM_SIZE()
END_MESSAGE_MAP()

// CStencilStepperDialog message handlers

BOOL CStencilStepperDialog::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   m_stepGrid.AttachGrid(this,IDC_StepGrid);

   m_toolBar.createAndLoad(IDR_StencilStepperToolbar);

   if (isEnglish(getCamCadDatabase().getPageUnits()))
   {
      m_baseThickness           = getCamCadDatabase().convertToPageUnits(pageUnitsMils, 10.0);
      m_minBaseStencilThickness = getCamCadDatabase().convertToPageUnits(pageUnitsMils,  1.0);
      m_maxBaseStencilThickness = getCamCadDatabase().convertToPageUnits(pageUnitsMils,100.0);
   }
   else
   {
      m_baseThickness           = getCamCadDatabase().convertToPageUnits(pageUnitsMilliMeters, .25);
      m_minBaseStencilThickness = getCamCadDatabase().convertToPageUnits(pageUnitsMilliMeters, .025);
      m_maxBaseStencilThickness = getCamCadDatabase().convertToPageUnits(pageUnitsMilliMeters,2.5);
   }

   setBaseStencilThickness(m_baseThickness);

   readParameters();
   scanForLayers();

   // initialize base thickness
   m_baseThicknessStatic.SetWindowText("Base Stencil Thickness (" + unitsString(getCamCadDatabase().getPageUnits()) + ")");
   m_baseThicknessEditBox.SetWindowText(fpfmt(m_baseThickness,4));

   // initialize paste layers

   // initialize step grid
   // sanity checks
   if (getPcbFile() == NULL)
   {
      formatMessageBox("One and only One file must be visible");
      PostMessage(WM_CLOSE);
      return TRUE;
   }
   else if (m_pasteLayersComboBox.GetCount() < 1)
   {
      formatMessageBox("No paste layers found!");
      PostMessage(WM_CLOSE);
      return TRUE;
   }

   int selectedLayerIndex = getSelectedLayerIndex();

   if (selectedLayerIndex >= 0)
   {
      setStepColors(m_stepColors);
      updateDepositData();
      setVisibleLayer(selectedLayerIndex);
   }

   //m_pasteLayersComboBox.SetEditSel(-1,-1);

   updateShowOverrideColor();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CStencilStepperDialog::OnBnClickedAddPadsToStep()
{
   CString stencilThicknessString;
   m_stencilThicknessEditBox.GetWindowText(stencilThicknessString);
   stencilThicknessString.Trim();
   char* endChar;

   double stencilThickness = strtod(stencilThicknessString,&endChar);

   if (stencilThicknessString.IsEmpty() || *endChar != '\0')
   {
      formatMessageBox("Unrecognizable stencil thickness");
   }
   else
   {
      CSelectList& selectList = m_camCadDoc.SelectList;
      int selectedLayerIndex   = getSelectedLayerIndex();
      bool defaultThicknessFlag = fpeq(stencilThickness,m_baseThickness);
      COLORREF stepColor = m_stencilSteps.getStep(stencilThickness).getColor();

      for (POSITION pos = selectList.GetHeadPosition();pos != NULL;)
      {
         SelectStruct* selectStruct = selectList.GetNext(pos);
         DataStruct* data = selectStruct->getData();

         if (data->getDataType()   == dataTypeInsert &&
             data->getLayerIndex() == selectedLayerIndex)
         {
            if (defaultThicknessFlag)
            {
               m_camCadDatabase.removeAttribute(&(data->getAttributesRef()),
                  getStencilThicknessKeywordIndex());
            }
            else
            {
               m_camCadDatabase.addAttribute(&(data->getAttributesRef()),
                  getStencilThicknessKeywordIndex(),stencilThicknessString);
            }

            data->setColorOverride(m_showColorFlag);
            data->setOverrideColor(stepColor);
         }
      }

      clearMarked();
      updateDepositData();
      writeParameters();
   }
}

void CStencilStepperDialog::OnBnClickedRemoveStencilStepperAttributes()
{
   if (formatMessageBox(MB_YESNO | MB_ICONQUESTION,
       "OK to erase all stencil thickness data from the File?") == IDYES)
   {
      removeStencilStepperAttributes();
      updateDepositData(true);
   }
}

void CStencilStepperDialog::OnCbnSelchangePasteLayers()
{
   int selectedIndex = m_pasteLayersComboBox.GetCurSel();

   if (selectedIndex >= 0)
   {
      int pasteLayerIndex = m_pasteLayersComboBox.GetItemData(selectedIndex);
      changePasteLayer(pasteLayerIndex,false);
      setVisibleLayer(pasteLayerIndex);
   }

   //m_pasteLayersComboBox.SetEditSel(-1,-1);
}

void CStencilStepperDialog::OnClose()
{
   saveWindowState();

   CResizingDialog::OnClose();

   getActiveView()->PostMessage(WM_COMMAND,ID_TerminateStencilStepper,0);
   EndDialog(IDCANCEL);
}

void CStencilStepperDialog::OnEnKillFocusBaseStencilThickness()
{
   CString baseStencilThicknessString;
   m_baseThicknessEditBox.GetWindowText(baseStencilThicknessString);

   double newStencilBaseThickness = atof(baseStencilThicknessString);

   if (!fpeq(newStencilBaseThickness,m_baseThickness))
   {
      setBaseStencilThickness(baseStencilThicknessString);
      repaint();
      writeParameters();
   }
}

void CStencilStepperDialog::OnBnClickedMarkByWindow()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_QUERY_MARKRECT);
   }
}

void CStencilStepperDialog::OnUpdateMarkByWindow(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CStencilStepperDialog::OnBnClickedMarkByWindowCross()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      view->PostMessage(WM_COMMAND,ID_QUERY_MARK_CROSSCHECK);
   }
}

void CStencilStepperDialog::OnUpdateMarkByWindowCross(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CStencilStepperDialog::OnBnClickedClearMarked()
{
   clearMarked();
}

void CStencilStepperDialog::OnUpdateClearMarked(CCmdUI* pCmdUI) 
{ 
   pCmdUI->Enable(true);
}

void CStencilStepperDialog::OnBnClickedRepaint()
{
   repaint();
}

void CStencilStepperDialog::OnBnClickedShowOverrideColor()
{
   m_showColorFlag = true;
   updateShowOverrideColor();
   m_stencilSteps.setColorOverride(m_showColorFlag);

   repaint();
}

void CStencilStepperDialog::OnBnClickedHideOverrideColor()
{
   m_showColorFlag = false;
   updateShowOverrideColor();
   m_stencilSteps.setColorOverride(m_showColorFlag);

   repaint();
}

BOOL CStencilStepperDialog::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

   // if there is a top level routing frame then let it handle the message
   if (GetRoutingFrame() != NULL) return FALSE;

   // to be thorough we will need to handle UNICODE versions of the message also !!
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   //TCHAR szFullText[512];
   CString strTipText;
   UINT nID = pNMHDR->idFrom;

   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool 
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   if (nID != 0) // will be zero on a separator
   {
      strTipText.LoadString(nID);
      //AfxLoadString(nID, szFullText);
      //strTipText = szFullText;

   #ifndef _UNICODE
      if (pNMHDR->code == TTN_NEEDTEXTA)
      {
         lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
      }
      else
      {
         _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
      }
   #else
      if (pNMHDR->code == TTN_NEEDTEXTA)
      {
         _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
      }
      else
      {
         lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
      }
   #endif

      *pResult = 0;

      // bring the tooltip window above other popup windows
      ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
         SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
      
      return TRUE;
   }

   return FALSE;
}

void CStencilStepperDialog::OnSize(UINT nType, int cx, int cy)
{
   CResizingDialog::OnSize(nType, cx, cy);

   if (m_stepGrid.GetNumberCols() > 0)
   {
      m_stepGrid.SizeLastColumnToFit();
   }
}
