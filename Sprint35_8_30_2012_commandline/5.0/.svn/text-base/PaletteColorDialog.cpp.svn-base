// $Header: /CAMCAD/4.5/PaletteColorDialog.cpp 13    7/24/06 10:19a Kurt Van Ness $

/*
History: PaletteColorDialog.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 10/20/00   Time: 11:46p
 * Updated in $/PadsDRC
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 6/12/99    Time: 11:31a
 * Created in $/PadsDRC
 * Initial add.
*/

#include "StdAfx.h"
#include "PaletteColorDialog.h"
#include "Colors.h"
#include "RwLib.h"
#include ".\palettecolordialog.h"
#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//_____________________________________________________________________________
CPaletteRampEntry::CPaletteRampEntry(int index)
{
   m_index    = index;
   m_color    = colorBlack;
   m_nodeFlag = false;
}

CPaletteRampEntry::~CPaletteRampEntry()
{
}

void CPaletteRampEntry::setNodeFlag(bool flag)
{
   m_nodeFlag = flag;
}

//_____________________________________________________________________________
CPaletteRamp::CPaletteRamp(ColorModelTag colorModel)
: m_colorModel(colorModel)
, m_nslLin(1.)
{
   m_outOfBoundsEntry = NULL;
   invalidatePalette();
}

CPaletteRamp::~CPaletteRamp()
{
   empty();
}

void CPaletteRamp::empty()
{
   for (int ind = 0;ind < m_paletteEntries.GetSize();ind++)
   {
      CPaletteRampEntry* paletteEntry = m_paletteEntries.GetAt(ind);
      delete paletteEntry;
   }

   m_paletteEntries.RemoveAll();

   delete m_outOfBoundsEntry;
   m_outOfBoundsEntry = NULL;

   invalidatePalette();
}

CPaletteRampEntry* CPaletteRamp::newPaletteRampEntry(int index)
{
   CPaletteRampEntry* paletteRampEntry = new CPaletteRampEntry(index);

   return paletteRampEntry;
}

void CPaletteRamp::invalidatePalette()
{
   m_paletteValid = false;
}

void CPaletteRamp::validatePalette()
{
   if (! m_paletteValid)
   {
      calculatePalette();
   }
}

CPaletteRampEntry& CPaletteRamp::getEntry(int index)
{
   CPaletteRampEntry* paletteEntry;

   if (index < 0 || index >= m_paletteEntries.GetSize())
   {
      if (m_outOfBoundsEntry == NULL)
      {
         m_outOfBoundsEntry = newPaletteRampEntry(-1);
      }

      paletteEntry = m_outOfBoundsEntry;
   }
   else
   {
      paletteEntry = m_paletteEntries.GetAt(index);

      if (paletteEntry == NULL)
      {
         paletteEntry = newPaletteRampEntry(index);
         m_paletteEntries.SetAt(index,paletteEntry);
      }
   }

   return *paletteEntry;
}

void CPaletteRamp::setSize(int size,int growBy)
{
   m_paletteEntries.SetSize(size,growBy);
   invalidatePalette();
}

COLORREF CPaletteRamp::getColor(int index)
{
   validatePalette();

   COLORREF retval = getEntry(index).getColor();

   return retval;
}

void CPaletteRamp::setColor(int index,COLORREF color)
{
   getEntry(index).setColor(color);
   invalidatePalette();
}

void CPaletteRamp::setNslLin(double nslLin)
{
   m_nslLin = nslLin;
}

bool CPaletteRamp::isNode(int index)
{
   bool retval = getEntry(index).isNode();

   return retval;
}

void CPaletteRamp::setNodeFlag(int index,bool nodeFlag)
{
   getEntry(index).setNodeFlag(nodeFlag);
   invalidatePalette();
}

void CPaletteRamp::calculatePalette()
{
   int size = getSize();
   CPaletteRampEntry *thisPaletteEntryNode,*prevPaletteEntryNode;
   int thisNodeIndex,prevNodeIndex = -1;

   for (thisNodeIndex = 0;thisNodeIndex < size;thisNodeIndex++)
   {
      thisPaletteEntryNode = &(getEntry(thisNodeIndex));
      CColor thisColor(thisPaletteEntryNode->getColor(),m_nslLin);
      thisColor.setNslLin(m_nslLin);
      COLORREF thisColorRef = thisColor.getColor();

      if (thisPaletteEntryNode->isNode())
      {
         if (prevNodeIndex >= 0)
         {
            prevPaletteEntryNode = &(getEntry(prevNodeIndex));
            CColor prevColor(prevPaletteEntryNode->getColor(),m_nslLin);
            COLORREF prevColorRef = prevColor.getColor();
            double span = thisNodeIndex - prevNodeIndex;

            for (int index = prevNodeIndex + 1;index < thisNodeIndex;index++)
            {
               double parameterMetric = (index - prevNodeIndex) / span;
               CColor interpolatedColor = prevColor.interpolate(thisColor,parameterMetric,m_colorModel);

               COLORREF colorRef = interpolatedColor.getColor();
               getEntry(index).setColor(colorRef);
            }
         }

         prevNodeIndex = thisNodeIndex;
      }
   }

   m_paletteValid = true;
}

//_____________________________________________________________________________
CPaletteItem::CPaletteItem(int index,const CPoint& topLeft,int sideLength,int spacing,int xCount,int yCount,
      BookReadingOrderTag bookReadingOrder)
: CPaletteRampEntry(index)
, m_topLeft(topLeft)
, m_sideLength(sideLength)
, m_spacing(spacing)
, m_xCount(xCount)
, m_yCount(yCount)
, m_bookReadingOrder(bookReadingOrder)
, m_showNodeFlag(false)
, m_hoveredFlag(false)
, m_selectedFlag(false)
{
}

CPaletteItem::~CPaletteItem()
{
}

CRect CPaletteItem::getRect()
{
   int gridSpacing = m_sideLength + m_spacing;
   int xGrid = m_topLeft.x + getIndexX() * gridSpacing + m_spacing/2;
   int yGrid = m_topLeft.y + getIndexY() * gridSpacing + m_spacing/2;

   CRect itemRect(xGrid,yGrid,xGrid + m_sideLength,yGrid + m_sideLength);

   return itemRect;
}

int CPaletteItem::getIndexX()
{
   int index = getIndex();

   switch (m_bookReadingOrder)
   {
   case bookReadingTopBottomLeftRight:
   case bookReadingBottomTopLeftRight:
      index = index/m_yCount;
      break;
   case bookReadingTopBottomRightLeft:
   case bookReadingBottomTopRightLeft:
      index = (m_xCount - 1) - index/m_yCount;
      break;
   case bookReadingLeftRightTopBottom:
   case bookReadingLeftRightBottomTop:
      index = index%m_xCount;
      break;
   case bookReadingRightLeftTopBottom:
   case bookReadingRightLeftBottomTop:
      index = (m_xCount - 1) - index%m_xCount;
      break;
   }

   return index;
}

int CPaletteItem::getIndexY()
{
   int index = getIndex();

   switch (m_bookReadingOrder)
   {
   case bookReadingTopBottomLeftRight:
   case bookReadingTopBottomRightLeft:
      index = index%m_yCount;
      break;
   case bookReadingBottomTopLeftRight:
   case bookReadingBottomTopRightLeft:
      index = (m_yCount - 1) - index%m_yCount;
      break;
   case bookReadingLeftRightTopBottom:
   case bookReadingRightLeftTopBottom:
      index = index/m_xCount;
      break;
   case bookReadingLeftRightBottomTop:
   case bookReadingRightLeftBottomTop:
      index = (m_yCount - 1) - index/m_xCount;
      break;
   }

   return index;
}

CRect CPaletteItem::getInflatedRect(int inc) 
{
   CRect itemRect = getRect();
   itemRect.InflateRect(inc,inc,inc,inc);

   return itemRect;
}

void CPaletteItem::draw(CDC& dc) 
{
   // Selected item
   COLORREF outerBorderColor,innerBorderColor = ::GetSysColor(COLOR_3DFACE);

   bool nodeFlag = isNode() && getShowNodeFlag();

   if (m_hoveredFlag)
   {
      outerBorderColor = (nodeFlag ? colorMagenta : colorBlue);
   }
   else if (m_selectedFlag)
   {
      outerBorderColor = (nodeFlag ? colorOrange : colorWhite);
   }
   else if (nodeFlag)
   {
      outerBorderColor = colorRed;
   }
   else
   {
      outerBorderColor = ::GetSysColor(COLOR_3DFACE);
      innerBorderColor = getColor();
   }

   //TRACE("CPaletteItem::draw() - OUTLINE index=%d, borderColor = 0x%06x\n",m_index,borderColor);

   drawItem(dc,outerBorderColor,innerBorderColor,getColor(),getRect());
}

void CPaletteItem::drawItem(CDC& dc,COLORREF outerBorderColor,COLORREF innerBorderColor,COLORREF fillColor,CRect rect,int penWidth) 
{
   //TRACE("CPaletteItem::drawItem()\n");
   rect.InflateRect(1,1);
   dc.FillSolidRect(rect,outerBorderColor);

   rect.DeflateRect(2,2);
   dc.FillSolidRect(rect,innerBorderColor);

   rect.DeflateRect(2,2);
   dc.FillSolidRect(rect,fillColor);
}

//_____________________________________________________________________________
CPaletteItemArray::CPaletteItemArray(const CPoint& topLeft,int itemSideLength,int itemSpacing,int itemXCount,int itemYCount,
      ColorModelTag colorModel,BookReadingOrderTag bookReadingOrder)
: CPaletteRamp(colorModel)
, m_topLeft(topLeft)
, m_itemSideLength(itemSideLength)
, m_itemSpacing(itemSpacing)
, m_itemXCount(itemXCount)
, m_itemYCount(itemYCount)
, m_bookReadingOrder(bookReadingOrder)
{
   initialize();
}

CPaletteItemArray::CPaletteItemArray(const CRect& rect,int itemSideLength,int itemSpacing,
       ColorModelTag colorModel,BookReadingOrderTag bookReadingOrder)
: CPaletteRamp(colorModel)
, m_topLeft(rect.TopLeft())
, m_itemSideLength(itemSideLength)
, m_itemSpacing(itemSpacing)
, m_bookReadingOrder(bookReadingOrder)
{
   CSize itemCounts = getItemCounts(rect);
   m_itemXCount = itemCounts.cx;
   m_itemYCount = itemCounts.cy;

   initialize();
}

CPaletteItemArray::~CPaletteItemArray()
{
}

void CPaletteItemArray::initialize()
{
   int itemCount = getItemCount();
   setSize(itemCount);

   if (itemCount > 0)
   {
      CPaletteItem& firstItem = getPaletteItem(0);
      firstItem.setColor(colorRed);
      firstItem.setNodeFlag(true);

      if (itemCount > 1)
      {
         CPaletteItem& lastItem  = getPaletteItem(itemCount - 1);
         lastItem.setColor(colorBlue);
         lastItem.setNodeFlag(true);
      }
   }
}

CPaletteRampEntry* CPaletteItemArray::newPaletteRampEntry(int index)
{
   CPaletteRampEntry* paletteRampEntry = 
      new CPaletteItem(index,getTopLeft(),getItemSideLength(),getItemSpacing(),
          getItemXCount(),getItemYCount(),m_bookReadingOrder);

   return paletteRampEntry;
}

int CPaletteItemArray::hitTest(CPoint point) 
{
   int retval = -1;

   for (int ind = 0;ind < getSize();ind++)
   {
      CPaletteItem& paletteItem = getPaletteItem(ind);

      if (paletteItem.getRect().PtInRect(point))
      {
         retval = ind;
         break;
      }
   }

   TRACE("CPaletteItemArray::hitTest(%d,%d) - retval = %d\n",point.x,point.y,retval);

   return retval;
}

CSize CPaletteItemArray::getItemCounts() const
{
   CSize itemCounts;

   itemCounts.cx = m_itemXCount;
   itemCounts.cy = m_itemYCount;

   return itemCounts;
}

CSize CPaletteItemArray::getItemCounts(const CRect& rect) const
{
   CSize itemCounts;
   const int bevelWidth = 2;

   itemCounts.cx = (rect.Width()  - 2*bevelWidth) / getGridSpacing();
   itemCounts.cy = (rect.Height() - 2*bevelWidth) / getGridSpacing();

   return itemCounts;
}

//_____________________________________________________________________________
CColorPaletteStatic::CColorPaletteStatic()
: m_paletteItemArray(NULL)
, m_colorModel(colorModelHsl)
, m_componentIndex0(0)
, m_componentIndex1(1)
, m_componentIndex2(2)
, m_selectedItemIndex(-1)
, m_nslLin(1.)
{
}

CColorPaletteStatic::~CColorPaletteStatic()
{
   delete m_paletteItemArray;
}

void CColorPaletteStatic::invalidatePalette()
{
   delete m_paletteItemArray;
   m_paletteItemArray = NULL;

   m_selectedItemIndex = -1;
}

void CColorPaletteStatic::setColorModel(ColorModelTag colorModel)
{
   invalidatePalette();

   m_colorModel = colorModel;
}

void CColorPaletteStatic::setNslLin(double nslLin)
{
   invalidatePalette();

   m_nslLin = nslLin;
}

void CColorPaletteStatic::setComponentIndices(int componentIndex0,int componentIndex1)
{
   if (componentIndex0 != componentIndex1 &&
       componentIndex0 >= 0 && componentIndex0 <= 2 &&
       componentIndex1 >= 0 && componentIndex1 <= 2 )
   {
      invalidatePalette();

      m_componentIndex0 = componentIndex0;
      m_componentIndex1 = componentIndex1;
      m_componentIndex2 = 3 - (m_componentIndex0 + m_componentIndex1);
   }
}

void CColorPaletteStatic::setMinComponent(int componentIndex,double component)
{
   invalidatePalette();

   switch (componentIndex)
   {
   case 0:  m_minComponent0 = component;  break;
   case 1:  m_minComponent1 = component;  break;
   case 2:  m_minComponent2 = component;  break;
   }
}

void CColorPaletteStatic::setMaxComponent(int componentIndex,double component)
{
   invalidatePalette();

   switch (componentIndex)
   {
   case 0:  m_maxComponent0 = component;  break;
   case 1:  m_maxComponent1 = component;  break;
   case 2:  m_maxComponent2 = component;  break;
   }
}

void CColorPaletteStatic::getMappedComponent(double& val0,double& val1,double& val2,double comp,int compIndex)
{
   switch (compIndex)
   {
   case 0:  val0 = comp;  break;
   case 1:  val1 = comp;  break;
   case 2:  val2 = comp;  break;
   }
}

void CColorPaletteStatic::getMappedComponents(double& val0,double& val1,double& val2,double comp0,double comp1,double comp2)
{
   getMappedComponent(val0,val1,val2,comp0,m_componentIndex0);
   getMappedComponent(val0,val1,val2,comp1,m_componentIndex1);
   getMappedComponent(val0,val1,val2,comp2,m_componentIndex2);
}

CPaletteItemArray& CColorPaletteStatic::getPaletteItemArray()
{
   if (m_paletteItemArray == NULL)
   {
      CRect clientRect;
      GetClientRect(&clientRect);

      m_paletteItemArray = new CPaletteItemArray(clientRect,20,2,m_colorModel,bookReadingTopBottomLeftRight);
      m_paletteItemArray->setNslLin(m_nslLin);

      CColor color;
      double minCompX = m_minComponent0;
      double maxCompX = m_maxComponent0;
      double minCompY = m_minComponent1;
      double maxCompY = m_maxComponent1;
      double dx = (maxCompX - minCompX)/m_paletteItemArray->getItemXCount();
      double dy = (maxCompY - minCompY)/m_paletteItemArray->getItemYCount();
      
      int yIndexTop = 0;
      int yIndexBottom = m_paletteItemArray->getItemYCount() - 1;
      double val0,val1,val2;

      for (int xIndex = 0;xIndex < m_paletteItemArray->getItemXCount();xIndex++)
      {
         double xComp = minCompX + xIndex * dx;
         int topIndex    = xIndex * m_paletteItemArray->getItemYCount();
         int bottomIndex = topIndex + yIndexBottom;

         CPaletteItem& topPaletteItem    = m_paletteItemArray->getPaletteItem(topIndex);
         CPaletteItem& bottomPaletteItem = m_paletteItemArray->getPaletteItem(bottomIndex);

         getMappedComponents(val0,val1,val2,xComp,minCompY,m_minComponent2);
         color.set(val0,val1,val2,m_colorModel);
         COLORREF topColorRef = color.getColor();
         topPaletteItem.setColor(topColorRef);
         topPaletteItem.setNodeFlag(true);

         getMappedComponents(val0,val1,val2,xComp,maxCompY,m_minComponent2);
         color.set(val0,val1,val2,m_colorModel);
         COLORREF bottomColorRef = color.getColor();
         bottomPaletteItem.setColor(bottomColorRef);
         bottomPaletteItem.setNodeFlag(true);
      }

      m_selectedItemIndex = -1;
   }

   return *m_paletteItemArray;
}

CPaletteItemArray& CColorPaletteStatic::getPaletteItemArrayOld()
{
   if (m_paletteItemArray == NULL)
   {
      CRect clientRect;
      GetClientRect(&clientRect);

      m_paletteItemArray = new CPaletteItemArray(clientRect,20,2,colorModelHsl,bookReadingTopBottomLeftRight);

      CColor color;
      double minLum = .2;
      double maxLum = .9;
      double dLum = (maxLum - minLum)/m_paletteItemArray->getItemXCount();
      double dHue = 1. / (m_paletteItemArray->getItemYCount() + 1);
      
      for (int xIndex = 0;xIndex < m_paletteItemArray->getItemXCount();xIndex++)
      {
         double lum = minLum + xIndex * dLum;
         int topIndex    = xIndex * m_paletteItemArray->getItemYCount();
         int bottomIndex = topIndex + m_paletteItemArray->getItemYCount() - 1;
   
         CPaletteItem& topPaletteItem    = m_paletteItemArray->getPaletteItem(topIndex);
         CPaletteItem& bottomPaletteItem = m_paletteItemArray->getPaletteItem(bottomIndex);

         color.set(0.,1.,lum,colorModelHsl);
         COLORREF topColorRef = color.getColor();
         topPaletteItem.setColor(topColorRef);
         topPaletteItem.setNodeFlag(true);

         color.set(1. - dHue,1.,lum,colorModelHsl);
         COLORREF bottomColorRef = color.getColor();
         bottomPaletteItem.setColor(bottomColorRef);
         bottomPaletteItem.setNodeFlag(true);
      }
   }

   return *m_paletteItemArray;
}

void CColorPaletteStatic::setSelectedItemIndex(int selectedIndex) 
{
   CRect rect;
   //bool redraw = (getSelectedItemIndex() >= 0 || selectedIndex > 0);

   CDC* pDC = GetDC();

   if (pDC != NULL)
   {
      if (getSelectedItemIndex() >= 0)
      {
         getPaletteItemArray().setSelectedFlag(getSelectedItemIndex(),false);
         getPaletteItemArray().draw(getSelectedItemIndex(),*pDC);

         //rect = getPaletteItemArray().getInflatedRect(getSelectedItemIndex(),1);
         //rect.OffsetRect(m_bitmapOffset);
         //InvalidateRect(rect);
      }

      m_selectedItemIndex = selectedIndex;

      if (getSelectedItemIndex() >= 0)
      {
         getPaletteItemArray().setSelectedFlag(getSelectedItemIndex(),true);
         getPaletteItemArray().draw(getSelectedItemIndex(),*pDC);

         //rect = getPaletteItemArray().getInflatedRect(getSelectedItemIndex(),1);
         //rect.OffsetRect(m_bitmapOffset);
         //InvalidateRect(rect);
      }
   }
}

int CColorPaletteStatic::setSelected(const CPoint& point)
{
   int itemIndex = getPaletteItemArray().hitTest(point);

   setSelectedItemIndex(itemIndex);

   return itemIndex;
}

CColor CColorPaletteStatic::getSelectedItemColor()
{
   CColor selectedColor;

   if (getSelectedItemIndex() >= 0)
   {
      selectedColor.setColor(getPaletteItemArray().getPaletteItem(m_selectedItemIndex).getColor());
   }

   return selectedColor;
}

bool CColorPaletteStatic::itemCountsChange(const CRect& rect)
{
   bool changeFlag = true;
   CSize itemCounts,newItemCounts;

   if (isPaletteValid())
   {
      CSize itemCounts    = getItemCounts();
      CSize newItemCounts = getItemCounts(rect);

      changeFlag = (newItemCounts.cx != itemCounts.cx || newItemCounts.cy != itemCounts.cy);
   }

   return changeFlag;
}

BEGIN_MESSAGE_MAP(CColorPaletteStatic, CStatic)
	//{{AFX_MSG_MAP(CKCBusyProgressCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorPaletteStatic::OnPaint() 
{
   PAINTSTRUCT ps;
   //CDC* pDC = BeginPaint(&ps);
   CDC* dc = BeginPaint(&ps);

   CRect clientRect;
   GetClientRect(&clientRect);
   CMemDC* memDC = new CMemDC(dc,&clientRect);
   CMemDC& pDC = *memDC;

   //m_memDC.FillSolidRect(0,0,CPaletteItem::getPixelArraySizeX(),CPaletteItem::getPixelArraySizeY() + 30,::GetSysColor(COLOR_3DFACE));
   CRect rect;
   GetClientRect(&rect);
   pDC->FillSolidRect(&rect,::GetSysColor(COLOR_3DFACE));

   // force a palette validation
   getPaletteItemArray().validatePalette();
   int itemCount = getPaletteItemArray().getItemCount();

   for (int itemIndex = 0;itemIndex < itemCount;itemIndex++)
   {
      //getPaletteItemArray().draw(itemIndex,*pDC);
      getPaletteItemArray().draw(itemIndex,pDC);
   }

   //int left = 0;
   //int top = CPaletteItem::getPixelArraySizeY() + 10;

   //for (int i = 0; i < m_numPaletteItems; i++)
   //{
   //   drawLine(m_memDC,left + i,top,left + i,top + 20,m_paletteItems.getColor(i));
   //}

   delete memDC;

   EndPaint(&ps);
}

//_____________________________________________________________________________
void forceDomain(int& value,int min,int max)
{
   if (min > max)
   {
      int tmp = min;
      min = max;
      max = tmp;
   }

   if (value < min) value = min;
   if (value > max) value = max;
}

void forceDomain(double& value,double min,double max)
{
   if (min > max)
   {
      double tmp = min;
      min = max;
      max = tmp;
   }

   if (value < min) value = min;
   if (value > max) value = max;
}

int toInt(double value)
{
   return (int)(value * 255.);
}

double toFloat(int value)
{
   return value / 255.;
}

#undef min
#undef max

double min(double num1,double num2, double num3)
{
   double retval = num1;

   if (num2 < retval) retval = num2;
   if (num3 < retval) retval = num3;

   return retval;
}

double max(double num1,double num2, double num3)
{
   double retval = num1;

   if (num2 > retval) retval = num2;
   if (num3 > retval) retval = num3;

   return retval;
}

//_____________________________________________________________________________
CPaletteColorDialog::CPaletteColorDialog(CAppPalette& appPalette,CWnd* pParent /*=NULL*/)
: CResizingDialog(CPaletteColorDialog::IDD, pParent)
, m_appPalette(appPalette)
, m_colorButton(appPalette)
, m_color(colorBlack)
, m_colorModelValue(1)
, m_xyAxisValue(1)
, m_sliderComponentValue(1)
{
   m_initialized = false;

   addFieldControl(IDC_ColorPalette,anchorLeft,growBoth);
   addFieldControl(IDC_IndependantAxis,anchorBottom);
   addFieldControl(IDC_SliderComponent0,anchorBottom);
   addFieldControl(IDC_SliderComponent1,anchorBottom);
   addFieldControl(IDC_SliderComponent2,anchorBottom);
   addFieldControl(IDC_XYAxisGroup,anchorBottom);
   addFieldControl(IDC_XYAxis0,anchorBottom);
   addFieldControl(IDC_XYAxis1,anchorBottom);
   addFieldControl(IDC_ColorModelGroup,anchorBottom);
   addFieldControl(IDC_RgbModel,anchorBottom);
   addFieldControl(IDC_HsvModel,anchorBottom);
   addFieldControl(IDC_HslModel,anchorBottom);
   addFieldControl(IDC_NslModel,anchorBottom);
   addFieldControl(IDC_IndependantAxisPos,anchorBottom);
}

void CPaletteColorDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);

   CString string;

   if (m_initialized && !(pDX->m_bSaveAndValidate))
   {
      //forceDomains();

      //m_color.setColor(RGB(toInt(m_red),toInt(m_grn),toInt(m_blu)));
      //m_color.set(m_red,m_grn,m_blu);
      //TRACE("CPaletteColorDialog::DoDataExchange() - m_color = %08x\n",m_color);
      m_appPalette.animatePaletteEntry(m_appPalette.getEditIndex(),m_color.getColor());

      // check pixel value


      if (!m_appPalette.isPaletteDevice())
      {
         m_colorButton.Invalidate();
      }
      else 
      {
         m_colorButton.verifyColor();
      }

      string.Format("%d",toInt(m_color.getRed()));  m_redValue.SetWindowText(string);
      string.Format("%d",toInt(m_color.getGrn()));  m_grnValue.SetWindowText(string);
      string.Format("%d",toInt(m_color.getBlu()));  m_bluValue.SetWindowText(string);

      string.Format("%.3f",m_color.getHsvHue());    m_hsvHueValue.SetWindowText(string);
      string.Format("%.3f",m_color.getHsvSat());    m_hsvSatValue.SetWindowText(string);
      string.Format("%.3f",m_color.getHsvVal());    m_hsvValValue.SetWindowText(string);

      string.Format("%.3f",m_color.getHslHue());    m_hslHueValue.SetWindowText(string);
      string.Format("%.3f",m_color.getHslSat());    m_hslSatValue.SetWindowText(string);
      string.Format("%.3f",m_color.getHslLum());    m_hslLumValue.SetWindowText(string);

      string.Format("%.3f",m_color.getNslHue());    m_nslHueValue.SetWindowText(string);
      string.Format("%.3f",m_color.getNslSat());    m_nslSatValue.SetWindowText(string);
      string.Format("%.3f",m_color.getNslLum());    m_nslLumValue.SetWindowText(string);
      string.Format("%.3f",m_color.getNslLin());    m_nslLinValue.SetWindowText(string);

      m_redPos.SetPos(toInt(m_color.getRed()));
      m_grnPos.SetPos(toInt(m_color.getGrn()));
      m_bluPos.SetPos(toInt(m_color.getBlu()));

      m_hsvHuePos.SetPos(toInt(m_color.getHsvHue()));
      m_hsvSatPos.SetPos(toInt(m_color.getHsvSat()));
      m_hsvValPos.SetPos(toInt(m_color.getHsvVal()));

      m_hslHuePos.SetPos(toInt(m_color.getHslHue()));
      m_hslSatPos.SetPos(toInt(m_color.getHslSat()));
      m_hslLumPos.SetPos(toInt(m_color.getHslLum()));

      m_nslHuePos.SetPos(toInt(m_color.getNslHue()));
      m_nslSatPos.SetPos(toInt(m_color.getNslSat()));
      m_nslLumPos.SetPos(toInt(m_color.getNslLum()));
      m_nslLinPos.SetPos(toInt(m_color.getNslLinPos()));
   }

   //{{AFX_DATA_MAP(CPaletteColorDialog)
   DDX_Control(pDX, IDC_HueValue, m_hsvHueValue);
   DDX_Control(pDX, IDC_SatValue, m_hsvSatValue);
   DDX_Control(pDX, IDC_ValValue, m_hsvValValue);

   DDX_Control(pDX, IDC_HslHueValue, m_hslHueValue);
   DDX_Control(pDX, IDC_HslSatValue, m_hslSatValue);
   DDX_Control(pDX, IDC_HslLumValue, m_hslLumValue);

   DDX_Control(pDX, IDC_RedValue, m_redValue);
   DDX_Control(pDX, IDC_GrnValue, m_grnValue);
   DDX_Control(pDX, IDC_BluValue, m_bluValue);

   DDX_Control(pDX, IDC_NslHueValue, m_nslHueValue);
   DDX_Control(pDX, IDC_NslSatValue, m_nslSatValue);
   DDX_Control(pDX, IDC_NslLumValue, m_nslLumValue);
   DDX_Control(pDX, IDC_NslLinValue, m_nslLinValue);

   DDX_Control(pDX, IDC_Color, m_colorButton);

   DDX_Control(pDX, IDC_ValPos, m_hsvValPos);
   DDX_Control(pDX, IDC_SatPos, m_hsvSatPos);
   DDX_Control(pDX, IDC_HuePos, m_hsvHuePos);

   DDX_Control(pDX, IDC_HslLumPos, m_hslLumPos);
   DDX_Control(pDX, IDC_HslSatPos, m_hslSatPos);
   DDX_Control(pDX, IDC_HslHuePos, m_hslHuePos);

   DDX_Control(pDX, IDC_RedPos, m_redPos);
   DDX_Control(pDX, IDC_GrnPos, m_grnPos);
   DDX_Control(pDX, IDC_BluPos, m_bluPos);

   DDX_Control(pDX, IDC_NslHuePos, m_nslHuePos);
   DDX_Control(pDX, IDC_NslSatPos, m_nslSatPos);
   DDX_Control(pDX, IDC_NslLumPos, m_nslLumPos);
   DDX_Control(pDX, IDC_NslLinPos, m_nslLinPos);

   DDX_Control(pDX, IDC_ColorPalette, m_colorPaletteStatic);
   DDX_Radio(pDX, IDC_RgbModel, m_colorModelValue);
   DDX_Radio(pDX, IDC_XYAxis0, m_xyAxisValue);
   DDX_Radio(pDX, IDC_SliderComponent0, m_sliderComponentValue);
   DDX_Control(pDX, IDC_IndependantAxisPos, m_independantVariablePos);
   //}}AFX_DATA_MAP

   if (m_initialized && pDX->m_bSaveAndValidate)
   {
      //m_redValue.GetWindowText(string);  m_red = toFloat(atoi(string));
      //m_grnValue.GetWindowText(string);  m_grn = toFloat(atoi(string));
      //m_bluValue.GetWindowText(string);  m_blu = toFloat(atoi(string));
      //m_hsvHueValue.GetWindowText(string);  m_hsvHue = atof(string);
      //m_hsvSatValue.GetWindowText(string);  m_hsvSat = atof(string);
      //m_hsvValValue.GetWindowText(string);  m_hsvVal = atof(string);
      //m_hslHueValue.GetWindowText(string);  m_hslHue = atof(string);
      //m_hslSatValue.GetWindowText(string);  m_hslSat = atof(string);
      //m_hslLumValue.GetWindowText(string);  m_hslLum = atof(string);

      //forceDomains();
   }
}

ColorModelTag CPaletteColorDialog::getColorModel() const
{
   ColorModelTag colorModel = colorModelHsl;

   switch (m_colorModelValue)
   {
   case 0:  colorModel = colorModelRgb;  break;
   case 1:  colorModel = colorModelHsl;  break;
   case 2:  colorModel = colorModelHsv;  break;
   case 3:  colorModel = colorModelNsl;  break;
   }

   return colorModel;
}

void CPaletteColorDialog::updatePaletteSettings()
{
   if (m_initialized)
   {
      ColorModelTag colorModel = getColorModel();
      CString component0,component1,component2;

      switch (colorModel)
      {
      case colorModelRgb:
         component0 = "R";
         component1 = "G";
         component2 = "B";
         break;
      case colorModelHsl:
         component0 = "H";
         component1 = "S";
         component2 = "L";
         break;
      case colorModelHsv:
         component0 = "H";
         component1 = "S";
         component2 = "V";
         break;
      case colorModelNsl:
         component0 = "N";
         component1 = "S";
         component2 = "L";
         break;
      }

      GetDlgItem(IDC_SliderComponent0)->SetWindowText(component0);
      GetDlgItem(IDC_SliderComponent1)->SetWindowText(component1);
      GetDlgItem(IDC_SliderComponent2)->SetWindowText(component2);

      CString axisLabel0,axisLabel1;
      int componentIndex2 = m_sliderComponentValue;
      int componentIndex0,componentIndex1;
      bool xyAxis0Flag = (m_xyAxisValue == 0);

      switch (m_sliderComponentValue)
      {
      case 0:
         axisLabel0 = component1 + component2;
         axisLabel1 = component2 + component1;
         componentIndex0 = (xyAxis0Flag ? 1 : 2);
         componentIndex1 = (xyAxis0Flag ? 2 : 1);
         break;
      case 1:
      case 3:
         axisLabel0 = component0 + component2;
         axisLabel1 = component2 + component0;
         componentIndex0 = (xyAxis0Flag ? 0 : 2);
         componentIndex1 = (xyAxis0Flag ? 2 : 0);
         break;
      case 2:
         axisLabel0 = component0 + component1;
         axisLabel1 = component1 + component0;
         componentIndex0 = (xyAxis0Flag ? 0 : 1);
         componentIndex1 = (xyAxis0Flag ? 1 : 0);
         break;
      }

      GetDlgItem(IDC_XYAxis0)->SetWindowText(axisLabel0);
      GetDlgItem(IDC_XYAxis1)->SetWindowText(axisLabel1);

      double independantVariableValue = toFloat(m_independantVariablePos.GetPos());

      m_colorPaletteStatic.setColorModel(colorModel);
      m_colorPaletteStatic.setComponentIndices(componentIndex0,componentIndex1);
      m_colorPaletteStatic.setMinComponent(2,independantVariableValue);
      m_colorPaletteStatic.Invalidate();
   }
}

//void CPaletteColorDialog::forceDomains()
//{
//   forceDomain(m_red,0.,1.);
//   forceDomain(m_grn,0.,1.);
//   forceDomain(m_blu,0.,1.);
//   forceDomain(m_hsvHue,0.,1.);
//   forceDomain(m_hsvSat,0.,1.);
//   forceDomain(m_hsvVal,0.,1.);
//   forceDomain(m_hslHue,0.,1.);
//   forceDomain(m_hslSat,0.,1.);
//   forceDomain(m_hslLum,0.,1.);
//}

//void CPaletteColorDialog::calcRGB()
//{
//   forceDomains();
//
//   if (m_hsvSat == 0.)
//   {
//      m_red = m_grn = m_blu = m_hsvVal;
//   }
//   else
//   {
//      int index = (int)(m_hsvHue * 6.);
//      double fraction = (m_hsvHue * 6.) - index;
//      double pComp = m_hsvVal * (1. - m_hsvSat);
//      double qComp = m_hsvVal * (1. - m_hsvSat * fraction);
//      double tComp = m_hsvVal * (1. - m_hsvSat * (1. - fraction));
//
//      switch(index % 6)
//      {
//      case 0:  m_red = m_hsvVal;  m_grn = tComp;  m_blu = pComp;  break;
//      case 1:  m_red = qComp;  m_grn = m_hsvVal;  m_blu = pComp;  break;
//      case 2:  m_red = pComp;  m_grn = m_hsvVal;  m_blu = tComp;  break;
//      case 3:  m_red = pComp;  m_grn = qComp;  m_blu = m_hsvVal;  break;
//      case 4:  m_red = tComp;  m_grn = pComp;  m_blu = m_hsvVal;  break;
//      default: m_red = m_hsvVal;  m_grn = pComp;  m_blu = qComp;  break;
//      }
//   }
//}

//void CPaletteColorDialog::calcHSV()
//{
//   forceDomains();
//
//	double cMin = min(m_red,m_grn,m_blu);
//	double cMax = max(m_red,m_grn,m_blu);
//
//   m_hsvVal = cMax;
//   double delta = cMax - cMin;
//
//   if (cMax == 0.)
//   {
//      m_hsvSat = 0.;
//      m_hsvHue = 0.;
//   }
//   else
//   {
//      m_hsvSat = delta / cMax;
//
//      if (delta == 0.)
//      {
//         m_hsvHue = 3.;
//      }
//      else if (cMax == m_red)
//      {
//         m_hsvHue = (m_grn - m_blu) / delta;
//      }
//      else if (cMax == m_grn)
//      {
//         m_hsvHue = 2. + (m_blu - m_red) / delta;
//      }
//      else
//      {
//         m_hsvHue = 4. + (m_red - m_grn) / delta;
//      }
//
//      m_hsvHue /= 6.;
//
//      if (m_hsvHue < 0.) m_hsvHue += 1.;
//   }
//}

BEGIN_MESSAGE_MAP(CPaletteColorDialog, CResizingDialog)
	//{{AFX_MSG_MAP(CPaletteColorDialog)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_RedValue, OnKillFocusRedValue)
	ON_EN_KILLFOCUS(IDC_GrnValue, OnKillFocusGrnValue)
	ON_EN_KILLFOCUS(IDC_BluValue, OnKillFocusBluValue)
	ON_EN_KILLFOCUS(IDC_HueValue, OnKillFocusHsvHueValue)
	ON_EN_KILLFOCUS(IDC_SatValue, OnKillFocusHsvSatValue)
	ON_EN_KILLFOCUS(IDC_ValValue, OnKillFocusHsvValValue)
	ON_EN_KILLFOCUS(IDC_HslHueValue, OnKillFocusHslHueValue)
	ON_EN_KILLFOCUS(IDC_HslSatValue, OnKillFocusHslSatValue)
	ON_EN_KILLFOCUS(IDC_HslLumValue, OnKillFocusHslLumValue)
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(ID_PaletteTest, OnPaletteTest)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_RgbModel, OnBnClickedRgbModel)
   ON_BN_CLICKED(IDC_XYAxis0, OnBnClickedXyAxis0)
   ON_BN_CLICKED(IDC_SliderComponent0, OnBnClickedSliderComponent0)
   ON_BN_CLICKED(IDC_HslModel, OnBnClickedHslModel)
   ON_BN_CLICKED(IDC_HsvModel, OnBnClickedHsvModel)
   ON_BN_CLICKED(IDC_XYAxis1, OnBnClickedXyAxis1)
   ON_BN_CLICKED(IDC_SliderComponent1, OnBnClickedSliderComponent1)
   ON_BN_CLICKED(IDC_SliderComponent2, OnBnClickedSliderComponent2)
   ON_WM_MOVE()
   ON_WM_SIZE()
   ON_WM_LBUTTONDOWN()
   ON_EN_KILLFOCUS(IDC_NslHueValue, OnKillFocusNslHueValue)
   ON_EN_KILLFOCUS(IDC_NslSatValue, OnKillFocusNslSatValue)
   ON_EN_KILLFOCUS(IDC_NslLumValue, OnKillFocusNslLumValue)
   ON_EN_KILLFOCUS(IDC_NslLinValue, OnKillFocusNslLinValue)
   ON_BN_CLICKED(IDC_NslModel, OnBnClickedNslModel)
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteColorDialog message handlers

/*BOOL CPaletteColorDialog::OnInitDialog() 
{
	CResizingDialog::OnInitDialog();

   CSupString windowParameters = AfxGetApp()->GetProfileString("Window Parameters","PaletteColorDialog","");

   if (!windowParameters.IsEmpty())
   {
      CStringArray param;
      windowParameters.Parse(param);

      if (param.GetSize() >= 4)
      {
         WINDOWPLACEMENT windowPlacement;
         BOOL success = GetWindowPlacement(&windowPlacement);

         CRect rcNormalBefore = windowPlacement.rcNormalPosition;

         windowPlacement.rcNormalPosition.left   = atoi(param[0]);
         windowPlacement.rcNormalPosition.top    = atoi(param[1]);
         windowPlacement.rcNormalPosition.right  = atoi(param[2]);
         windowPlacement.rcNormalPosition.bottom = atoi(param[3]);

         CRect rcNormalAfter = windowPlacement.rcNormalPosition;

         if (rcNormalAfter.Height() < rcNormalBefore.Height())
         {
            windowPlacement.rcNormalPosition.bottom = windowPlacement.rcNormalPosition.top + rcNormalBefore.Height();
         }

         if (rcNormalAfter.Width() < rcNormalBefore.Width())
         {
            windowPlacement.rcNormalPosition.right = windowPlacement.rcNormalPosition.left + rcNormalBefore.Width();
         }

         windowPlacement.showCmd = SW_SHOW;
         success = SetWindowPlacement(&windowPlacement);
      }
   }

   m_redPos.SetRange(0,255);
   m_grnPos.SetRange(0,255);
   m_bluPos.SetRange(0,255);

   m_hsvHuePos.SetRange(0,255);
   m_hsvSatPos.SetRange(0,255);
   m_hsvValPos.SetRange(0,255);

   m_hslHuePos.SetRange(0,255);
   m_hslSatPos.SetRange(0,255);
   m_hslLumPos.SetRange(0,255);

   m_nslHuePos.SetRange(0,255);
   m_nslSatPos.SetRange(0,255);
   m_nslLumPos.SetRange(0,255);
   m_nslLinPos.SetRange(0,255);

   m_independantVariablePos.SetRange(0,255);

   m_independantVariablePos.SetPos(255);
   
   //m_red = toFloat(GetRValue(m_color));
   //m_grn = toFloat(GetGValue(m_color));
   //m_blu = toFloat(GetBValue(m_color));

   //calcHSV();
   m_appPalette.setEditPaletteEntry(m_color.getColor());

   m_colorPaletteStatic.setColorModel(colorModelHsl);
   m_colorPaletteStatic.setComponentIndices(2,0);
   m_colorPaletteStatic.setMinComponent(0,.2);
   m_colorPaletteStatic.setMaxComponent(0,.9);
   m_colorPaletteStatic.setMinComponent(1,0.);
   m_colorPaletteStatic.setMaxComponent(1,.99);
   m_colorPaletteStatic.setMinComponent(2,.5);
   m_colorPaletteStatic.setMaxComponent(2,.5);

   m_initialized = true;
   UpdateData(false);
   updatePaletteSettings();

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}*/

void CPaletteColorDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

   //CSliderCtrl* sliderCtrl = VERIFY_DOWNCAST(CSliderCtrl,(CObject*)pScrollBar);
   CSliderCtrl* sliderCtrl = (CSliderCtrl*)((CObject*)pScrollBar);
   CString string;

   //TRACE("CPaletteColorDialog::OnHScroll() - nSBCode=%d, nPos = %d\n",nSBCode,nPos);

   if (nSBCode == TB_THUMBTRACK)
   {
      if (sliderCtrl == &m_redPos)
      {
         m_color.setRed(toFloat(nPos));
      }
      else if (sliderCtrl == &m_grnPos)
      {
         m_color.setGrn(toFloat(nPos));
      }
      else if (sliderCtrl == &m_bluPos)
      {
         m_color.setBlu(toFloat(nPos));
      }
      else if (sliderCtrl == &m_hsvHuePos)
      {
         m_color.setHsvHue(toFloat(nPos));
      }
      else if (sliderCtrl == &m_hsvSatPos)
      {
         m_color.setHsvSat(toFloat(nPos));
      }
      else if (sliderCtrl == &m_hsvValPos)
      {
         m_color.setHsvVal(toFloat(nPos));
      }
      else if (sliderCtrl == &m_hslHuePos)
      {
         m_color.setHslHue(toFloat(nPos));
      }
      else if (sliderCtrl == &m_hslSatPos)
      {
         m_color.setHslSat(toFloat(nPos));
      }
      else if (sliderCtrl == &m_hslLumPos)
      {
         m_color.setHslLum(toFloat(nPos));
      }
      else if (sliderCtrl == &m_nslHuePos)
      {
         m_color.setNslHue(toFloat(nPos));
      }
      else if (sliderCtrl == &m_nslSatPos)
      {
         m_color.setNslSat(toFloat(nPos));
      }
      else if (sliderCtrl == &m_nslLumPos)
      {
         m_color.setNslLum(toFloat(nPos));
      }
      else if (sliderCtrl == &m_nslLinPos)
      {
         m_color.setNslLinFromPos(toFloat(nPos));

         if (getColorModel() == colorModelNsl)
         {
            m_colorPaletteStatic.setNslLin(m_color.getNslLin());

            updatePaletteSettings();
         }
      }
      else if (sliderCtrl == &m_independantVariablePos)
      {
         updatePaletteSettings();
      }
   }

   UpdateData(false);

	//CResizingDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPaletteColorDialog::OnKillFocusRedValue() 
{
   CString string;
   m_redValue.GetWindowText(string);
   m_color.setRed(toFloat(atoi(string)));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusGrnValue() 
{
   CString string;
   m_grnValue.GetWindowText(string);
   m_color.setGrn(toFloat(atoi(string)));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusBluValue() 
{
   CString string;
   m_bluValue.GetWindowText(string);
   m_color.setBlu(toFloat(atoi(string)));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusHsvHueValue() 
{
   CString string;
   m_hsvHueValue.GetWindowText(string);
   m_color.setHsvHue(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusHsvSatValue() 
{
   CString string;
   m_hsvSatValue.GetWindowText(string);
   m_color.setHsvSat(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusHsvValValue() 
{
   CString string;
   m_hsvValValue.GetWindowText(string);
   m_color.setHsvVal(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusHslHueValue() 
{
   CString string;
   m_hslHueValue.GetWindowText(string);
   m_color.setHslHue(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusHslSatValue() 
{
   CString string;
   m_hslSatValue.GetWindowText(string);
   m_color.setHslSat(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusHslLumValue() 
{
   CString string;
   m_hslLumValue.GetWindowText(string);
   m_color.setHslLum(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusNslHueValue()
{
   CString string;
   m_nslHueValue.GetWindowText(string);
   m_color.setNslHue(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusNslSatValue()
{
   CString string;
   m_nslSatValue.GetWindowText(string);
   m_color.setNslSat(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusNslLumValue()
{
   CString string;
   m_nslLumValue.GetWindowText(string);
   m_color.setNslLum(atof(string));

   UpdateData(false);
}

void CPaletteColorDialog::OnKillFocusNslLinValue()
{
   CString string;
   m_nslLinValue.GetWindowText(string);
   m_color.setNslLin(atof(string));

   UpdateData(false);
}

/*void CPaletteColorDialog::OnDestroy() 
{
   WINDOWPLACEMENT windowPlacement;
   BOOL success = GetWindowPlacement(&windowPlacement);

   CString windowParameters;
   windowParameters.Format("%d %d %d %d",
      windowPlacement.rcNormalPosition.left,
      windowPlacement.rcNormalPosition.top,
      windowPlacement.rcNormalPosition.right,
      windowPlacement.rcNormalPosition.bottom   );

   AfxGetApp()->WriteProfileString("Window Parameters","PaletteColorDialog",windowParameters);

   CResizingDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}*/

void CPaletteColorDialog::OnHelp() 
{
	// TODO: Add your control notification handler code here
   //displayHelpID(CPaletteColorDialog::IDD);
}

void yield()
{
   MSG message;
   
   while (::PeekMessage(&message,NULL,0,0,PM_REMOVE))
   {
      ::TranslateMessage(&message);
      ::DispatchMessage(&message);
   }
}

void CPaletteColorDialog::OnPaletteTest() 
{
	// TODO: Add your control notification handler code here
   //m_hsvSat = m_hsvVal = 1.;
   m_color.setHsvSat(1.);
   m_color.setHsvVal(1.);
   int hue;

	for (hue = 255;hue >= 0;hue--)
   {
      //m_hsvHue = toFloat(hue);
      m_color.setHsvHue(toFloat(hue));
      //calcRGB();

      m_redValue.Invalidate();
      m_grnValue.Invalidate();
      m_bluValue.Invalidate();
      m_hsvHueValue.Invalidate();
      m_hsvSatValue.Invalidate();
      m_hsvValValue.Invalidate();
      m_hslHueValue.Invalidate();
      m_hslSatValue.Invalidate();
      m_hslLumValue.Invalidate();
      m_nslHueValue.Invalidate();
      m_nslSatValue.Invalidate();
      m_nslLumValue.Invalidate();

      UpdateData(false);

      yield();
      Sleep(10);
   }

	for (hue = 0;hue <= 255;hue++)
   {
      //m_hsvHue = toFloat(hue);
      m_color.setHsvHue(toFloat(hue));
      //calcRGB();

      m_redValue.Invalidate();
      m_grnValue.Invalidate();
      m_bluValue.Invalidate();
      m_hsvHueValue.Invalidate();
      m_hsvSatValue.Invalidate();
      m_hsvValValue.Invalidate();
      m_hslHueValue.Invalidate();
      m_hslSatValue.Invalidate();
      m_hslLumValue.Invalidate();
      m_nslHueValue.Invalidate();
      m_nslSatValue.Invalidate();
      m_nslLumValue.Invalidate();

      UpdateData(false);

      yield();
      Sleep(10);
   }
}

void CPaletteColorDialog::OnBnClickedRgbModel()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedHslModel()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedNslModel()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedHsvModel()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedXyAxis1()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedXyAxis0()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedSliderComponent0()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedSliderComponent1()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnBnClickedSliderComponent2()
{
   UpdateData(true);
   updatePaletteSettings();
}

void CPaletteColorDialog::OnMove(int x, int y)
{
   CResizingDialog::OnMove(x, y);

   // TODO: Add your message handler code here
}

void CPaletteColorDialog::OnSize(UINT nType, int cx, int cy)
{
   CResizingDialog::OnSize(nType, cx, cy);

   CRect rect;
   m_colorPaletteStatic.GetClientRect(&rect);

   if (m_colorPaletteStatic.itemCountsChange(rect))
   {
      updatePaletteSettings();
   }
}

void CPaletteColorDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
   ClientToScreen(&point);
   m_colorPaletteStatic.ScreenToClient(&point);
   m_colorPaletteStatic.setSelected(point);

   int selectedIndex = m_colorPaletteStatic.getSelectedItemIndex();

   if (selectedIndex >= 0)
   {
      m_color = m_colorPaletteStatic.getSelectedItemColor();
      UpdateData(false);
   }

   //int hitItemIndex = m_colorPaletteStatic.hitTest(point);
   //m_isDragging = (hitItemIndex >= 0);

   //if (m_isDragging)
   //{
   //   setSelectedItemIndex(hitItemIndex);

   //   SetDlgItemInt(IDC_EDIT_SELECTED,getSelectedItemIndex());
   //   SetDlgItemInt(IDC_EDIT_RED,m_paletteItems.getRed(getSelectedItemIndex()));
   //   SetDlgItemInt(IDC_EDIT_GRN,m_paletteItems.getGrn(getSelectedItemIndex()));
   //   SetDlgItemInt(IDC_EDIT_BLU,m_paletteItems.getBlu(getSelectedItemIndex()));

   //   TRACE("CDialogPalette::OnLButtonDown() - selected %d\n",getSelectedItemIndex());
   //}
   //else
   //{
   //   setSelectedItemIndex();
   //}
      
   CResizingDialog::OnLButtonDown(nFlags, point);
}

/*BOOL CPaletteColorDialog::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default

   return CResizingDialog::OnEraseBkgnd(pDC);
   //return FALSE;
}*/
