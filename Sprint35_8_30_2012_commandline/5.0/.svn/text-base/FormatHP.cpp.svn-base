// $Header: /CAMCAD/4.5/FormatHP.cpp 11    4/28/06 6:48p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "FormatHP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FormatHPin dialog

FormatHPin::FormatHPin(CWnd* pParent /*=NULL*/)
   : CDialog(FormatHPin::IDD, pParent)
{
   //{{AFX_DATA_INIT(FormatHPin)
   m_circ2ap = FALSE;
   m_layer1 = _T("");
   m_layer2 = _T("");
   m_layer3 = _T("");
   m_layer4 = _T("");
   m_layer5 = _T("");
   m_layer6 = _T("");
   m_layer7 = _T("");
   m_layer8 = _T("");
   m_pen1 = _T("");
   m_pen2 = _T("");
   m_pen3 = _T("");
   m_pen4 = _T("");
   m_pen5 = _T("");
   m_pen6 = _T("");
   m_pen7 = _T("");
   m_pen8 = _T("");
   m_prefix = _T("");
   m_prop_text = TRUE;
   m_width1 = _T("");
   m_width2 = _T("");
   m_width3 = _T("");
   m_width4 = _T("");
   m_width5 = _T("");
   m_width6 = _T("");
   m_width7 = _T("");
   m_width8 = _T("");
   m_scale = 1.0;
   m_space = TRUE;
   m_ignorePenWidth = FALSE;
   //}}AFX_DATA_INIT
}

void FormatHPin::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(FormatHPin)
   DDX_Control(pDX, IDC_SCROLLBAR1, m_scroll);
   DDX_Control(pDX, IDC_UNITS, m_unitsCB);
   DDX_Check(pDX, IDC_CIRCLES2APS, m_circ2ap);
   DDX_Text(pDX, IDC_LAYER1, m_layer1);
   DDX_Text(pDX, IDC_LAYER2, m_layer2);
   DDX_Text(pDX, IDC_LAYER3, m_layer3);
   DDX_Text(pDX, IDC_LAYER4, m_layer4);
   DDX_Text(pDX, IDC_LAYER5, m_layer5);
   DDX_Text(pDX, IDC_LAYER6, m_layer6);
   DDX_Text(pDX, IDC_LAYER7, m_layer7);
   DDX_Text(pDX, IDC_LAYER8, m_layer8);
   DDX_Text(pDX, IDC_PEN1, m_pen1);
   DDX_Text(pDX, IDC_PEN2, m_pen2);
   DDX_Text(pDX, IDC_PEN3, m_pen3);
   DDX_Text(pDX, IDC_PEN4, m_pen4);
   DDX_Text(pDX, IDC_PEN5, m_pen5);
   DDX_Text(pDX, IDC_PEN6, m_pen6);
   DDX_Text(pDX, IDC_PEN7, m_pen7);
   DDX_Text(pDX, IDC_PEN8, m_pen8);
   DDX_Text(pDX, IDC_PREFIX, m_prefix);
   DDX_Check(pDX, IDC_PROPORTIONAL_TEXT, m_prop_text);
   DDX_Text(pDX, IDC_WIDTH1, m_width1);
   DDX_Text(pDX, IDC_WIDTH2, m_width2);
   DDX_Text(pDX, IDC_WIDTH3, m_width3);
   DDX_Text(pDX, IDC_WIDTH4, m_width4);
   DDX_Text(pDX, IDC_WIDTH5, m_width5);
   DDX_Text(pDX, IDC_WIDTH6, m_width6);
   DDX_Text(pDX, IDC_WIDTH7, m_width7);
   DDX_Text(pDX, IDC_WIDTH8, m_width8);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-024, 1.e+025);
   DDX_Check(pDX, IDC_SPACE, m_space);
   DDX_Check(pDX, IDC_IGNORE_PENWIDTH, m_ignorePenWidth);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FormatHPin, CDialog)
   //{{AFX_MSG_MAP(FormatHPin)
   ON_WM_VSCROLL()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FormatHPin message handlers

BOOL FormatHPin::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i=0; i < MAX_UNITS; i++)
      m_unitsCB.AddString(GetUnitName(i));
   m_unitsCB.SetCurSel(3);
   
   m_scroll.SetScrollRange(0, 8);
   m_scroll.SetScrollPos(0);
   pennum = 0;
   m_space = format->HP.HPspace;
   WritePens();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void FormatHPin::WritePens()
{
   m_pen1.Format("%d.", pennum+1);
   m_pen2.Format("%d.", pennum+2);
   m_pen3.Format("%d.", pennum+3);
   m_pen4.Format("%d.", pennum+4);
   m_pen5.Format("%d.", pennum+5);
   m_pen6.Format("%d.", pennum+6);
   m_pen7.Format("%d.", pennum+7);
   m_pen8.Format("%d.", pennum+8);

   m_width1.Format("%.*lf", decimals, format->HP.HpPens[pennum+0].width);
   m_width2.Format("%.*lf", decimals, format->HP.HpPens[pennum+1].width);
   m_width3.Format("%.*lf", decimals, format->HP.HpPens[pennum+2].width);
   m_width4.Format("%.*lf", decimals, format->HP.HpPens[pennum+3].width);
   m_width5.Format("%.*lf", decimals, format->HP.HpPens[pennum+4].width);
   m_width6.Format("%.*lf", decimals, format->HP.HpPens[pennum+5].width);
   m_width7.Format("%.*lf", decimals, format->HP.HpPens[pennum+6].width);
   m_width8.Format("%.*lf", decimals, format->HP.HpPens[pennum+7].width);

   m_layer1 = format->HP.HpPens[pennum+0].layer;
   m_layer2 = format->HP.HpPens[pennum+1].layer;
   m_layer3 = format->HP.HpPens[pennum+2].layer;
   m_layer4 = format->HP.HpPens[pennum+3].layer;
   m_layer5 = format->HP.HpPens[pennum+4].layer;
   m_layer6 = format->HP.HpPens[pennum+5].layer;
   m_layer7 = format->HP.HpPens[pennum+6].layer;
   m_layer8 = format->HP.HpPens[pennum+7].layer;

   UpdateData(FALSE);
}

void FormatHPin::ReadPens()
{
   UpdateData();

   format->HP.HpPens[pennum+0].width = atof(m_width1);
   format->HP.HpPens[pennum+1].width = atof(m_width2);
   format->HP.HpPens[pennum+2].width = atof(m_width3);
   format->HP.HpPens[pennum+3].width = atof(m_width4);
   format->HP.HpPens[pennum+4].width = atof(m_width5);
   format->HP.HpPens[pennum+5].width = atof(m_width6);
   format->HP.HpPens[pennum+6].width = atof(m_width7);
   format->HP.HpPens[pennum+7].width = atof(m_width8);

   format->HP.HpPens[pennum+0].layer = m_layer1;
   format->HP.HpPens[pennum+1].layer = m_layer2;
   format->HP.HpPens[pennum+2].layer = m_layer3;
   format->HP.HpPens[pennum+3].layer = m_layer4;
   format->HP.HpPens[pennum+4].layer = m_layer5;
   format->HP.HpPens[pennum+5].layer = m_layer6;
   format->HP.HpPens[pennum+6].layer = m_layer7;
   format->HP.HpPens[pennum+7].layer = m_layer8;
}

void FormatHPin::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int temp;

   temp = pennum;
   switch (nSBCode)
   {
   case SB_TOP:
      temp = 0;
      break;
   case SB_BOTTOM:
      temp = 8;
      break;

   case SB_PAGEDOWN:
      temp += 8;
      break;
   case SB_PAGEUP:
      temp -= 8;
      break;

   case SB_LINEDOWN:
      temp++;
      break;
   case SB_LINEUP:
      temp--;
      break;

   case SB_THUMBPOSITION:
      temp = nPos;
      break;

   case SB_THUMBTRACK:
      temp = nPos;
      break;

   case SB_ENDSCROLL:
      break;
   }

   if (temp < 0) temp = 0;
   if (temp > 8) temp = 8;

   ReadPens();
   if (pennum != temp)
   {
      pennum = temp;
      WritePens();
      m_scroll.SetScrollPos(pennum);
   }

   CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void FormatHPin::OnOK() 
{
   ReadPens();
   format->PortFileUnits = m_unitsCB.GetCurSel();
   format->HP.HPspace = m_space;
   
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CDI_Out dialog
CDI_Out::CDI_Out(CWnd* pParent /*=NULL*/)
   : CDialog(CDI_Out::IDD, pParent)
{
   //{{AFX_DATA_INIT(CDI_Out)
   m_size1 = _T("");
   m_size2 = _T("");
   m_size3 = _T("");
   m_size4 = _T("");
   m_size5 = _T("");
   m_size6 = _T("");
   m_size7 = _T("");
   m_size8 = _T("");
   m_width1 = 0;
   m_width2 = 0;
   m_width3 = 0;
   m_width4 = 0;
   m_width5 = 0;
   m_width6 = 0;
   m_width7 = 0;
   m_width8 = 0;
   m_scale = 0.0;
   //}}AFX_DATA_INIT
}

void CDI_Out::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDI_Out)
   DDX_Control(pDX, IDC_SCROLLBAR1, m_scroll);
   DDX_Text(pDX, IDC_STATIC1, m_size1);
   DDX_Text(pDX, IDC_STATIC2, m_size2);
   DDX_Text(pDX, IDC_STATIC3, m_size3);
   DDX_Text(pDX, IDC_STATIC4, m_size4);
   DDX_Text(pDX, IDC_STATIC5, m_size5);
   DDX_Text(pDX, IDC_STATIC6, m_size6);
   DDX_Text(pDX, IDC_STATIC7, m_size7);
   DDX_Text(pDX, IDC_STATIC8, m_size8);
   DDX_Text(pDX, IDC_WIDTH1, m_width1);
   DDV_MinMaxInt(pDX, m_width1, 0, 7);
   DDX_Text(pDX, IDC_WIDTH2, m_width2);
   DDV_MinMaxInt(pDX, m_width2, 0, 7);
   DDX_Text(pDX, IDC_WIDTH3, m_width3);
   DDV_MinMaxInt(pDX, m_width3, 0, 7);
   DDX_Text(pDX, IDC_WIDTH4, m_width4);
   DDV_MinMaxInt(pDX, m_width4, 0, 7);
   DDX_Text(pDX, IDC_WIDTH5, m_width5);
   DDV_MinMaxInt(pDX, m_width5, 0, 7);
   DDX_Text(pDX, IDC_WIDTH6, m_width6);
   DDV_MinMaxInt(pDX, m_width6, 0, 7);
   DDX_Text(pDX, IDC_WIDTH7, m_width7);
   DDV_MinMaxInt(pDX, m_width7, 0, 7);
   DDX_Text(pDX, IDC_WIDTH8, m_width8);
   DDV_MinMaxInt(pDX, m_width8, 0, 7);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDI_Out, CDialog)
   //{{AFX_MSG_MAP(CDI_Out)
   ON_WM_VSCROLL()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDI_Out message handlers

BOOL CDI_Out::OnInitDialog() 
{
   CDialog::OnInitDialog();

   format->WidthCodes = (int *)calloc(widths, sizeof(int));
   // default all widths to width codes 0..7
   int code = 0;
   for (int i=0; i<widths; i++)
   {
      if (doc->getWidthTable()[i]->getSizeA() == 0.0)
         format->WidthCodes[i] = 0;
      else
         format->WidthCodes[i] = (++code%8);
   }

   if (widths < 8)
      m_scroll.SetScrollRange(0, 0);
   else
      m_scroll.SetScrollRange(0, widths-8);
   m_scroll.SetScrollPos(0);
   widthNum = 0;

   WriteWidths();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CDI_Out::WriteWidths()
{
   if (widths > 0) 
   {
      m_size1.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+0]->getSizeA());
      m_width1 = format->WidthCodes[widthNum+0];
   }
   if (widths > 1) 
   {
      m_size2.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+1]->getSizeA());
      m_width2 = format->WidthCodes[widthNum+1];
   }
   if (widths > 2) 
   {
      m_size3.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+2]->getSizeA());
      m_width3 = format->WidthCodes[widthNum+2];
   }
   if (widths > 3) 
   {
      m_size4.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+3]->getSizeA());
      m_width4 = format->WidthCodes[widthNum+3];
   }
   if (widths > 4) 
   {
      m_size5.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+4]->getSizeA());
      m_width5 = format->WidthCodes[widthNum+4];
   }
   if (widths > 5) 
   {
      m_size6.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+5]->getSizeA());
      m_width6 = format->WidthCodes[widthNum+5];
   }
   if (widths > 6) 
   {
      m_size7.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+6]->getSizeA());
      m_width7 = format->WidthCodes[widthNum+6];
   }
   if (widths > 7) 
   {
      m_size8.Format("%.*lg", decimals, doc->getWidthTable()[widthNum+7]->getSizeA());
      m_width8 = format->WidthCodes[widthNum+7];
   }

   UpdateData(FALSE);
}

void CDI_Out::ReadWidths()
{
   UpdateData();

   if (widths > 0)
      format->WidthCodes[widthNum+0] = m_width1;
   if (widths > 1)
      format->WidthCodes[widthNum+1] = m_width2;
   if (widths > 2)
      format->WidthCodes[widthNum+2] = m_width3;
   if (widths > 3)
      format->WidthCodes[widthNum+3] = m_width4;
   if (widths > 4)
      format->WidthCodes[widthNum+4] = m_width5;
   if (widths > 5)
      format->WidthCodes[widthNum+5] = m_width6;
   if (widths > 6)
      format->WidthCodes[widthNum+6] = m_width7;
   if (widths > 7)
      format->WidthCodes[widthNum+7] = m_width8;
}

void CDI_Out::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int temp;

   temp = widthNum;
   switch (nSBCode)
   {
   case SB_TOP:
      temp = 0;
      break;
   case SB_BOTTOM:
      temp = widths-8;
      break;

   case SB_PAGEDOWN:
      temp += 8;
      break;
   case SB_PAGEUP:
      temp -= 8;
      break;

   case SB_LINEDOWN:
      temp++;
      break;
   case SB_LINEUP:
      temp--;
      break;

   case SB_THUMBPOSITION:
      temp = nPos;
      break;

   case SB_THUMBTRACK:
      temp = nPos;
      break;

   case SB_ENDSCROLL:
      break;
   }

   if (temp < 0) temp = 0;
   if (widths > 8)
   {
      if (temp > widths-8) temp = widths-8;
   }
   else temp = 0;
      

   ReadWidths();
   if (widthNum != temp)
   {
      widthNum = temp;
      WriteWidths();
      m_scroll.SetScrollPos(widthNum);
   }

   CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDI_Out::OnOK() 
{
   ReadWidths();
   
   CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// Format_HP_Out dialog


Format_HP_Out::Format_HP_Out(CWnd* pParent /*=NULL*/)
   : CDialog(Format_HP_Out::IDD, pParent)
{
   //{{AFX_DATA_INIT(Format_HP_Out)
   m_HPGL2 = FALSE;
   m_pageFeed = _T("");
   m_pen1 = _T("");
   m_pen2 = _T("");
   m_pen3 = _T("");
   m_pen4 = _T("");
   m_pen5 = _T("");
   m_pen6 = _T("");
   m_pen7 = _T("");
   m_pen8 = _T("");
   m_width1 = _T("");
   m_width2 = _T("");
   m_width3 = _T("");
   m_width4 = _T("");
   m_width5 = _T("");
   m_width6 = _T("");
   m_width7 = _T("");
   m_width8 = _T("");
   m_pageFeedOn = FALSE;
   m_scale = 0.0;
   m_realWidth = 0.0;
   m_hollow = FALSE;
   //}}AFX_DATA_INIT
}


void Format_HP_Out::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Format_HP_Out)
   DDX_Control(pDX, IDC_SCROLLBAR1, m_scroll);
   DDX_Check(pDX, IDC_HPGL2, m_HPGL2);
   DDX_Text(pDX, IDC_PAGE_FEED, m_pageFeed);
   DDX_Text(pDX, IDC_PEN1, m_pen1);
   DDX_Text(pDX, IDC_PEN2, m_pen2);
   DDX_Text(pDX, IDC_PEN3, m_pen3);
   DDX_Text(pDX, IDC_PEN4, m_pen4);
   DDX_Text(pDX, IDC_PEN5, m_pen5);
   DDX_Text(pDX, IDC_PEN6, m_pen6);
   DDX_Text(pDX, IDC_PEN7, m_pen7);
   DDX_Text(pDX, IDC_PEN8, m_pen8);
   DDX_Text(pDX, IDC_WIDTH1, m_width1);
   DDX_Text(pDX, IDC_WIDTH2, m_width2);
   DDX_Text(pDX, IDC_WIDTH3, m_width3);
   DDX_Text(pDX, IDC_WIDTH4, m_width4);
   DDX_Text(pDX, IDC_WIDTH5, m_width5);
   DDX_Text(pDX, IDC_WIDTH6, m_width6);
   DDX_Text(pDX, IDC_WIDTH7, m_width7);
   DDX_Text(pDX, IDC_WIDTH8, m_width8);
   DDX_Check(pDX, IDC_PAGE_FEED_ON, m_pageFeedOn);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDV_MinMaxDouble(pDX, m_scale, 1.e-011, 99999999999999.);
   DDX_Text(pDX, IDC_REAL_WIDTH, m_realWidth);
   DDX_Check(pDX, IDC_HOLLOW, m_hollow);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Format_HP_Out, CDialog)
   //{{AFX_MSG_MAP(Format_HP_Out)
   ON_WM_VSCROLL()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Format_HP_Out message handlers

BOOL Format_HP_Out::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   m_scroll.SetScrollRange(0, 8);
   m_scroll.SetScrollPos(0);
   pennum = 0;

   WritePens();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void Format_HP_Out::WritePens()
{
   m_pen1.Format("%d.", pennum+1);
   m_pen2.Format("%d.", pennum+2);
   m_pen3.Format("%d.", pennum+3);
   m_pen4.Format("%d.", pennum+4);
   m_pen5.Format("%d.", pennum+5);
   m_pen6.Format("%d.", pennum+6);
   m_pen7.Format("%d.", pennum+7);
   m_pen8.Format("%d.", pennum+8);

   m_width1.Format("%.*lf", decimals, format->HP.HpPens[pennum+0].width);
   m_width2.Format("%.*lf", decimals, format->HP.HpPens[pennum+1].width);
   m_width3.Format("%.*lf", decimals, format->HP.HpPens[pennum+2].width);
   m_width4.Format("%.*lf", decimals, format->HP.HpPens[pennum+3].width);
   m_width5.Format("%.*lf", decimals, format->HP.HpPens[pennum+4].width);
   m_width6.Format("%.*lf", decimals, format->HP.HpPens[pennum+5].width);
   m_width7.Format("%.*lf", decimals, format->HP.HpPens[pennum+6].width);
   m_width8.Format("%.*lf", decimals, format->HP.HpPens[pennum+7].width);

   UpdateData(FALSE);
}

void Format_HP_Out::ReadPens()
{
   UpdateData();

   format->HP.HpPens[pennum+0].width = atof(m_width1);
   format->HP.HpPens[pennum+1].width = atof(m_width2);
   format->HP.HpPens[pennum+2].width = atof(m_width3);
   format->HP.HpPens[pennum+3].width = atof(m_width4);
   format->HP.HpPens[pennum+4].width = atof(m_width5);
   format->HP.HpPens[pennum+5].width = atof(m_width6);
   format->HP.HpPens[pennum+6].width = atof(m_width7);
   format->HP.HpPens[pennum+7].width = atof(m_width8);
}

void Format_HP_Out::OnOK() 
{
   ReadPens();
   
   CDialog::OnOK();
}

void Format_HP_Out::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int temp;

   temp = pennum;
   switch (nSBCode)
   {
   case SB_TOP:
      temp = 0;
      break;
   case SB_BOTTOM:
      temp = 8;
      break;

   case SB_PAGEDOWN:
      temp += 8;
      break;
   case SB_PAGEUP:
      temp -= 8;
      break;

   case SB_LINEDOWN:
      temp++;
      break;
   case SB_LINEUP:
      temp--;
      break;

   case SB_THUMBPOSITION:
      temp = nPos;
      break;

   case SB_THUMBTRACK:
      temp = nPos;
      break;

   case SB_ENDSCROLL:
      break;
   }

   if (temp < 0) temp = 0;
   if (temp > 8) temp = 8;

   ReadPens();
   if (pennum != temp)
   {
      pennum = temp;
      WritePens();
      m_scroll.SetScrollPos(pennum);
   }

   CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
