// $Header: /CAMCAD/4.6/ApList.cpp 28    5/24/07 5:23p Lynn Phung $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Aplist.h"
#include "math.h"
#include "pcbutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_STRINGLEN 120
#define ICONCOUNT 11

extern char *shapes[];

static CString units;
static int selItem;
static int SortBy = 0;

static void EditAperture(CListCtrl *listCtrl, BlockStruct *block, CCEtoODBDoc *doc)
{
   CEditAperture dlg;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   dlg.m_sizeA.Format("%.*lf", decimals, block->getSizeA());
   dlg.m_sizeB.Format("%.*lf", decimals, block->getSizeB());
   dlg.m_xoffset.Format("%.*lf", decimals, block->getXoffset());
   dlg.m_yoffset.Format("%.*lf", decimals, block->getYoffset());
   dlg.m_rotation.Format("%.2lf", RadToDeg(block->getRotation()));
   dlg.m_sizeC.Format("%.*lf", decimals, block->getSizeC());
   dlg.m_sizeD.Format("%.*lf", decimals, block->getSizeD());
   dlg.m_spokes.Format("%d", block->getSpokeCount());
   dlg.m_DCode = block->getDcode();
   dlg.m_Name = block->getName();
   dlg.m_shape = block->getShape();
   dlg.doc = doc;
   dlg.m_blockNum = (int)block->getSizeA();

   if (dlg.DoModal() != IDOK) return;
   
   block->setDcode(dlg.m_DCode);
   block->setName(dlg.m_Name);
   block->setSizeA((DbUnit)(atof(dlg.m_sizeA)));
   block->setSizeB((DbUnit)(atof(dlg.m_sizeB)));
   block->setRotation((DbUnit)DegToRad(atof(dlg.m_rotation)));
   block->setXoffset((DbUnit)(atof(dlg.m_xoffset)));
   block->setYoffset((DbUnit)(atof(dlg.m_yoffset)));
   block->setSizeC((DbUnit)(atof(dlg.m_sizeC)));
   block->setSizeD((DbUnit)(atof(dlg.m_sizeD)));
   block->setSpokeCount(atoi(dlg.m_spokes));
   block->setShape(dlg.m_shape);
   if (dlg.m_shape == T_COMPLEX)
      block->setSizeA((DbUnit)dlg.m_blockNum);

   LV_ITEM item;
   CString buf;
   item.mask = LVIF_TEXT | LVIF_IMAGE;
   item.iItem = selItem;
   item.iSubItem = 0;
   item.pszText = block->getNameRef().GetBuffer(1);
   item.iImage = block->getShape();
   listCtrl->SetItem(&item);
   block->getNameRef().ReleaseBuffer();

   item.mask = LVIF_TEXT;
   item.iSubItem = 1;
   buf.Format("D%d", block->getDcode());
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);

   item.iSubItem = 2;
   if (block->getShape() == T_COMPLEX)
   {
      BlockStruct *temp = doc->Find_Block_by_Num((int)(block->getSizeA()));
      if (temp) item.pszText = temp->getNameRef().GetBuffer(1);
      else item.pszText = "";
      listCtrl->SetItem(&item);
      if (temp) temp->getNameRef().ReleaseBuffer();
   }
   else
   {
      buf.Format("%.*lf", decimals, block->getSizeA());
      item.pszText = (char*)(LPCTSTR)buf;
      listCtrl->SetItem(&item);
   }

   switch (block->getShape())
   {
      case T_RECTANGLE:
      case T_DONUT:
      case T_THERMAL:
      case T_OBLONG:
         item.iSubItem = 3;
         buf.Format("%.*lf", decimals, block->getSizeB());
         item.pszText = (char*)(LPCTSTR)buf;
         listCtrl->SetItem(&item);
   }

   item.iSubItem = 4;
   if (block->getRotation())
      buf.Format("%.2lf", RadToDeg(block->getRotation()));
   else 
      buf = "";
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);

   item.iSubItem = 5;
   if (block->getXoffset())
      buf.Format("%.*lf", decimals, block->getXoffset());
   else
      buf = "";
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);

   item.iSubItem = 6;
   if (block->getYoffset())
      buf.Format("%.*lf", decimals, block->getYoffset());
   else
      buf = "";
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);

   item.iSubItem = 7;
   if (block->getSizeC() && block->getShape() == T_THERMAL)
      buf.Format("%.*lf", decimals, block->getSizeC());
   else
      buf = "";
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);

   item.iSubItem = 8;
   if (block->getSizeD() && block->getShape() == T_THERMAL)
      buf.Format("%.*lf", decimals, block->getSizeD());
   else
      buf = "";
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);

   item.iSubItem = 9;
   if (block->getSpokeCount() && block->getShape() == T_THERMAL)
      buf.Format("%d", block->getSpokeCount());
   else
      buf = "";
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);

   item.iSubItem = 10;
   if (block->getShape() == T_COMPLEX )
      buf = "X";
   else
      buf = "";
   item.pszText = (char*)(LPCTSTR)buf;
   listCtrl->SetItem(&item);
}

// return negative if 1 comes before 2
static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   double a, b;
   switch (SortBy)
   {
   case 0: // name
   default:
      return compare_name(((BlockStruct *)lParam1)->getName(), ((BlockStruct *)lParam2)->getName());
   case 1: // dcode
      return (((BlockStruct *)lParam1)->getDcode() - ((BlockStruct *)lParam2)->getDcode());
   case 2: // sizeA
      return (( ((BlockStruct *)lParam1)->getSizeA() - ((BlockStruct *)lParam2)->getSizeA()) < 0.0) ? -1 : 1;
   case 3: // sizeB
      {
         BOOL hasBSize1, hasBSize2;
         int shape1, shape2;
         shape1 = ((BlockStruct *)lParam1)->getShape();
         shape2 = ((BlockStruct *)lParam2)->getShape();
         hasBSize1 = (shape1 == T_RECTANGLE || shape1 == T_DONUT || shape1 == T_THERMAL || shape1 == T_OBLONG);
         hasBSize2 = (shape2 == T_RECTANGLE || shape2 == T_DONUT || shape2 == T_THERMAL || shape2 == T_OBLONG);
         if (!hasBSize2)
            return -1;
         if (!hasBSize1)
            return 1;
         return (( ((BlockStruct *)lParam1)->getSizeB() - ((BlockStruct *)lParam2)->getSizeB()) < 0.0) ? -1 : 1;
      }
   case 4: // rotation
      a = ((BlockStruct *)lParam1)->getRotation();
      b = ((BlockStruct *)lParam2)->getRotation();
      if (fabs(b) < SMALLNUMBER)
         return -1;
      if (fabs(a) < SMALLNUMBER)
         return 1;
      return (a - b < 0.0) ? -1 : 1;
   case 5: // xoffset
      a = ((BlockStruct *)lParam1)->getXoffset();
      b = ((BlockStruct *)lParam2)->getXoffset();
      if (fabs(b) < SMALLNUMBER)
         return -1;
      if (fabs(a) < SMALLNUMBER)
         return 1;
      return (a - b < 0.0) ? -1 : 1;
   case 6: // yoffset
      a = ((BlockStruct *)lParam1)->getYoffset();
      b = ((BlockStruct *)lParam2)->getYoffset();
      if (fabs(b) < SMALLNUMBER)
         return -1;
      if (fabs(a) < SMALLNUMBER)
         return 1;
      return (a - b < 0.0) ? -1 : 1;
   case 7: // sizeC
      a = ((BlockStruct *)lParam1)->getSizeC();
      b = ((BlockStruct *)lParam2)->getSizeC();
      if (fabs(b) < SMALLNUMBER)
         return -1;
      if (fabs(a) < SMALLNUMBER)
         return 1;
      return (a - b < 0.0) ? -1 : 1;
   case 8: // sizeD
      a = ((BlockStruct *)lParam1)->getSizeD();
      b = ((BlockStruct *)lParam2)->getSizeD();
      if (fabs(b) < SMALLNUMBER)
         return -1;
      if (fabs(a) < SMALLNUMBER)
         return 1;
      return (a - b < 0.0) ? -1 : 1;
   case 9: // spokes
      a = ((BlockStruct *)lParam1)->getSpokeCount();
      b = ((BlockStruct *)lParam2)->getSpokeCount();
      if (fabs(b) < SMALLNUMBER)
         return -1;
      if (fabs(a) < SMALLNUMBER)
         return 1;
      return (a - b < 0.0) ? -1 : 1;
   case 10: // True Shape
      {
         BOOL a, b;
         a = ((BlockStruct *)lParam1)->getShape() == T_COMPLEX;
         b = ((BlockStruct *)lParam2)->getShape() == T_COMPLEX;
      
         if (!b)
            return -1;
         if (!a)
            return 1;
         return 0;
      }
   }
}


////////////////////////////////////////////////////////////////////////////
// CApertureListDialog dialog
CApertureListDialog::CApertureListDialog(CCEtoODBDoc *Doc, CWnd* pParent)
   : CResizingDialog(CApertureListDialog::IDD, pParent)
{
   doc = Doc;
   decimals = GetDecimals(doc->getSettings().getPageUnits());
   units = GetUnitName(doc->getSettings().getPageUnits());
   //{{AFX_DATA_INIT(CApertureListDialog)
   m_units = _T("");
   //}}AFX_DATA_INIT

   addFieldControl(IDC_LISTCNTRL  ,anchorLeft,growBoth);
   addFieldControl(IDOK           ,anchorBottom);
   addFieldControl(ID_EDIT        ,anchorBottom);
   addFieldControl(IDC_ADD        ,anchorBottom);
   addFieldControl(ID_PRINT       ,anchorBottom);
   addFieldControl(IDC_HELP2      ,anchorBottom);
   addFieldControl(ID_SET_DCODES  ,anchorBottom);
   addFieldControl(ID_CLEAR_DCODES,anchorBottom);
   addFieldControl(IDC_UNIT_STATIC,anchorBottom);
   addFieldControl(ID_SQ_RECT     ,anchorBottom);
   addFieldControl(ID_RECT_SQ     ,anchorBottom);
   addFieldControl(IDC_UNDEFINE   ,anchorBottom);
}

CApertureListDialog::~CApertureListDialog()
{
   if (m_imageList)
      delete m_imageList;
}

void CApertureListDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CApertureListDialog)
   DDX_Control(pDX, IDC_LISTCNTRL, m_listCtrl);
   DDX_Text(pDX, IDC_UNIT_STATIC, m_units);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CApertureListDialog, CResizingDialog)
   //{{AFX_MSG_MAP(CApertureListDialog)
   ON_BN_CLICKED(ID_SET_DCODES, OnSetDcodes)
   ON_BN_CLICKED(ID_EDIT, OnEdit)
   ON_BN_CLICKED(IDHELP, OnHelp)
   ON_BN_CLICKED(ID_PRINT, OnPrint)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(ID_SQ_RECT, OnSquaresToRects)
   ON_BN_CLICKED(ID_RECT_SQ, OnRectsToSquares)
   ON_BN_CLICKED(IDC_UNDEFINE, OnUndefine)
   ON_LBN_DBLCLK(ID_APERTURELIST, OnEdit)
   ON_BN_CLICKED(ID_CLEAR_DCODES, OnClearDcodes)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApertureListDialog message handlers
BOOL CApertureListDialog::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, ICONCOUNT, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_UNDEFINED));
   m_imageList->Add(app->LoadIcon(IDI_ROUND));
   m_imageList->Add(app->LoadIcon(IDI_SQUARE));
   m_imageList->Add(app->LoadIcon(IDI_RECTANGLE));
   m_imageList->Add(app->LoadIcon(IDI_TARGET));
   m_imageList->Add(app->LoadIcon(IDI_THERMAL));
   m_imageList->Add(app->LoadIcon(IDI_COMPLEX));
   m_imageList->Add(app->LoadIcon(IDI_DONUT));
   m_imageList->Add(app->LoadIcon(IDI_OCTAGON));
   m_imageList->Add(app->LoadIcon(IDI_OBLONG));
   m_imageList->Add(app->LoadIcon(IDI_BLANK));

   m_listCtrl.decimals = decimals;
   m_listCtrl.doc = doc;
   m_listCtrl.SetImageList(m_imageList, LVSIL_SMALL);

   // columns
   LV_COLUMN column;
   CRect rect;
   m_listCtrl.GetWindowRect(&rect);
   int width = rect.Width() / 11;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.cx = width;

   column.pszText = "Aperture Name";
   column.iSubItem = 0;
   m_listCtrl.InsertColumn(0, &column);

   column.pszText = "Exported As";
   column.iSubItem = 1;
   m_listCtrl.InsertColumn(1, &column);

   column.pszText = "Size A";
   column.iSubItem = 2;
   m_listCtrl.InsertColumn(2, &column);

   column.pszText = "Size B";
   column.iSubItem = 3;
   m_listCtrl.InsertColumn(3, &column);

   column.pszText = "Rotation";
   column.iSubItem = 4;
   m_listCtrl.InsertColumn(4, &column);

   column.pszText = "Offset X";
   column.iSubItem = 5;
   m_listCtrl.InsertColumn(5, &column);

   column.pszText = "Offset Y";
   column.iSubItem = 6;
   m_listCtrl.InsertColumn(6, &column);

   column.pszText = "Size C";
   column.iSubItem = 7;
   m_listCtrl.InsertColumn(7, &column);

   column.pszText = "Size D";
   column.iSubItem = 8;
   m_listCtrl.InsertColumn(8, &column);

   column.pszText = "# of Spokes";
   column.iSubItem = 9;
   m_listCtrl.InsertColumn(9, &column);

   column.pszText = "True Shape";
   column.iSubItem = 10;
   m_listCtrl.InsertColumn(10, &column);

   FillListCtrl();

   selItem = -1;

   m_units.Format("%s:\t%s", "Units", units);
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CApertureListDialog::FillListCtrl()
{
   LV_ITEM item;
   int actualItem;
   CString buf;
   BlockStruct* block;
   BlockStruct* smallWidthBlock = NULL;
   int smallWidthIndex = doc->getSmallWidthIndex();
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   m_listCtrl.DeleteAllItems();

   // items
   int j = 0;
   for (int i = 0; i < doc->getNextWidthIndex(); i++)
   {  
      block = doc->getWidthTable()[i];

      if (block == NULL || block == smallWidthBlock)
      {
         continue;
      }

      if (i == smallWidthIndex)
      {
         smallWidthBlock = block;
      }

      if (!(block->getFlags() & BL_WIDTH || block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE))
         continue;

      item.mask = LVIF_TEXT | LVIF_IMAGE;
      item.iItem = j++;
      item.iSubItem = 0;
//#define DEBUG__INCLUDE_FILE_IN_NAME
#ifndef DEBUG__INCLUDE_FILE_IN_NAME
      item.pszText = block->getNameRef().GetBuffer(1);  // for release build, only use aperture name
#else
      CString filename = "Global"; // for filenum = -1
      int filenum = block->getFileNumber();
      FileStruct *apfile = this->doc->getFileList().FindByFileNumber(filenum);
      if (apfile != NULL)
         filename = apfile->getName();
      CString displayname = block->getName() + "  (" + filename + ")";
      item.pszText = displayname.GetBuffer(1);
#endif
      item.iImage = block->getShape();
      actualItem = m_listCtrl.InsertItem(&item);
//    if (actualItem == -1) ErrorMessage("Insert Failed");
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_listCtrl.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_listCtrl.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_listCtrl.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_listCtrl.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_listCtrl.InsertItem(&item);
               }
            }
         }
      }
      block->getNameRef().ReleaseBuffer();

      m_listCtrl.SetItemData(actualItem, (LPARAM)block);

      item.mask = LVIF_TEXT;
      item.iItem = actualItem;
      item.iSubItem = 1;
      buf.Format("D%d", block->getDcode());
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);

      item.iSubItem = 2;
      if (block->getShape() == T_COMPLEX)
      {
         BlockStruct *temp = doc->Find_Block_by_Num((int)(block->getSizeA()));
         if (temp) item.pszText = temp->getNameRef().GetBuffer(1);
         else item.pszText = "";
         m_listCtrl.SetItem(&item);
         if (temp) temp->getNameRef().ReleaseBuffer();
      }
      else
      {
         buf.Format("%.*lf", decimals, block->getSizeA());
         item.pszText = (char*)(LPCTSTR)buf;
         m_listCtrl.SetItem(&item);
      }

      switch (block->getShape())
      {
         case T_RECTANGLE:
         case T_DONUT:
         case T_THERMAL:
         case T_OBLONG:
            item.iSubItem = 3;
            buf.Format("%.*lf", decimals, block->getSizeB());
            item.pszText = (char*)(LPCTSTR)buf;
            m_listCtrl.SetItem(&item);
      }

      item.iSubItem = 4;
      if (block->getRotation())
         buf.Format("%.2lf", RadToDeg(block->getRotation()));
      else 
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);
   
      item.iSubItem = 5;
      if (block->getXoffset())
         buf.Format("%.*lf", decimals, block->getXoffset());
      else
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);
   
      item.iSubItem = 6;
      if (block->getYoffset())
         buf.Format("%.*lf", decimals, block->getYoffset());
      else
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);
   
      item.iSubItem = 7;
      if (block->getSizeC() && block->getShape() == T_THERMAL)
         buf.Format("%.*lf", decimals, block->getSizeC());
      else
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);
   
      item.iSubItem = 8;
      if (block->getSizeD() && block->getShape() == T_THERMAL)
         buf.Format("%.*lf", decimals, block->getSizeD());
      else
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);

      item.iSubItem = 9;
      if (block->getSpokeCount() && block->getShape() == T_THERMAL)
         buf.Format("%d", block->getSpokeCount());
      else
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);

      item.iSubItem = 10;
      if (block->getShape() == T_COMPLEX)
         buf = "X";
      else
         buf = "";
      item.pszText = (char*)(LPCTSTR)buf;
      m_listCtrl.SetItem(&item);
   }
   
   m_listCtrl.SortItems(CompareFunc, 0);
}

void CApertureListDialog::OnAdd() 
{
   static int i = 10/*, j*/;

   CString buf;
   buf.Format("NewAp_%d", i++);

   //j = doc->getNextWidthIndex()++;
   BlockStruct* apertureBlock = doc->Add_Blockname(buf, -1, BL_APERTURE | BL_GLOBAL, TRUE);

   //doc->getWidthTable().SetAtGrow(j,apertureBlock);
   int widthIndex = doc->getWidthTable().Add(apertureBlock);

   EditAperture(&m_listCtrl, doc->getWidthTable()[widthIndex], doc);
   FillListCtrl();
}

void CApertureListDialog::OnSetDcodes() 
{
   Generate_Unique_DCodes(doc);

   FillListCtrl();
}

void CApertureListDialog::OnClearDcodes() 
{
   // TODO: Add your control notification handler code here
   Clear_DCodes(doc);

   FillListCtrl();   
}

void CApertureListDialog::OnEdit() 
{
   if (selItem == -1) return;

   BlockStruct *block = (BlockStruct *)m_listCtrl.GetItemData(selItem);
   EditAperture(&m_listCtrl, block, doc);
}

#define NAME      0
#define SHAPE     0.35* hRes
#define DCODE     0.5 * hRes
#define SIZE      0.6 * hRes
#define PAGE      0.9 * hRes
#define START     20
static void PrintHeader(CDC *dc, int hRes, int vRes, int space, int page)
{
   dc->TextOut(NAME, 0, "Aperture Name");
   dc->TextOut(round(SHAPE), 0, "Shape");
   dc->TextOut(round(DCODE), 0, "DCode");
   dc->TextOut(round(SIZE), 0, "Size A  Size B");
   CString buf;
   buf.Format("Page %d", page);
   dc->TextOut(round(PAGE), 0, buf);

   dc->MoveTo(0, space); // header line
   dc->LineTo(hRes, space);   

   dc->MoveTo(0, vRes - space - START); // footer line
   dc->LineTo(hRes, vRes - space - START);   

   CTime t = t.GetCurrentTime(); // footer text
   buf.Format("Created by CCE to ODB++ on %s", t.Format("%A, %B %d, %Y at %I:%M:%S %p"));
   dc->TextOut(0, vRes - space, buf);
}
                                                  
void CApertureListDialog::OnPrint() 
{ 
   CDC dc;
   CPrintDialog dlg(FALSE); // FALSE - not print setup dialog
   
   if (dlg.DoModal() != IDOK) return;

   dc.Attach(dlg.GetPrinterDC());

   if (!dc.m_hDC) return;

   DOCINFO di;
   ::ZeroMemory(&di, sizeof(DOCINFO));
   di.cbSize = sizeof(DOCINFO);
   di.lpszDocName = "Aperture List Printout"; // title of print document

   dc.StartDoc(&di);
   dc.StartPage();

   {
      CString buf;
      int y = START, 
            page = 1,
            items,
            decimals = GetDecimals(doc->getSettings().getPageUnits()),
            hRes = dc.GetDeviceCaps(HORZRES), // pixels wide
            vRes = dc.GetDeviceCaps(VERTRES); // pixels high

      TEXTMETRIC met;
      dc.GetTextMetrics(&met);
      int space = met.tmHeight;

      PrintHeader(&dc, hRes, vRes, space, page++);

      if (items = m_listCtrl.GetItemCount())
      {
         int i = m_listCtrl.GetNextItem(-1, LVNI_ALL); // get first item on list

         // loop items
         while (i != -1)
         {
            y += space;
            if (y > vRes - 3 * space) // if at end of page
            {
               dc.EndPage();
               dc.StartPage();
               y = START + space;
               PrintHeader(&dc, hRes, vRes, space, page++);
            }

            BlockStruct *b = (BlockStruct *)m_listCtrl.GetItemData(i); // get Block ptr for this item

            dc.TextOut(NAME, y, b->getName());
            dc.TextOut(round(SHAPE), y, shapes[b->getShape()]);
            buf.Format("D%d", b->getDcode());
            dc.TextOut(round(DCODE), y, buf);
            switch (b->getShape())
            {
               case T_ROUND:
               case T_SQUARE:
               case T_OCTAGON:
               case T_TARGET:
               case T_BLANK:
                  buf.Format("%.*lf", decimals, b->getSizeA());
                  break;
               case T_RECTANGLE:
               case T_DONUT:
               case T_THERMAL:
               case T_OBLONG:
                  buf.Format("%.*lf   %.*lf", decimals, b->getSizeA(), decimals, b->getSizeB());
                  break;
               case T_COMPLEX:
                  {
                     BlockStruct *bb = doc->Find_Block_by_Num((int)(b->getSizeA()));
                     if (bb)
                        buf = bb->getName();
                     else buf = "";
                  }
                  break;
               case T_UNDEFINED:
               default:
                  break;
            }
            dc.TextOut(round(SIZE), y, buf);  

            if (items > 1)
               i = m_listCtrl.GetNextItem(i, LVNI_BELOW); // get next item on list
            else 
               break;
         }
      }
   }

   dc.EndPage();
   dc.EndDoc();
}

void CApertureListDialog::OnSquaresToRects() 
{
   for (int i=0; i < doc->getNextWidthIndex(); i++)
      if (doc->getWidthTable()[i])
      {
         BlockStruct *b = doc->getWidthTable()[i];
         if (b->getShape() == T_SQUARE)
         {
            b->setShape(T_RECTANGLE);
            b->setSizeB(b->getSizeA());
         }
      }

   FillListCtrl();
}

void CApertureListDialog::OnRectsToSquares() 
{
   for (int i=0; i < doc->getNextWidthIndex(); i++)
      if (doc->getWidthTable()[i])
      {
         BlockStruct *b = doc->getWidthTable()[i];
         if (b->getShape() == T_RECTANGLE && fabs(b->getSizeA() - b->getSizeB()) < SMALLNUMBER)
            b->setShape(T_SQUARE);
      }

   FillListCtrl();
}

void CApertureListDialog::OnUndefine() 
{
   if (ErrorMessage("Are you sure?", "Unload is not reversable", MB_YESNO | MB_DEFBUTTON2) == IDNO)
      return;

   for (int i=0; i < doc->getNextWidthIndex(); i++)
   {
      if (doc->getWidthTable()[i])
      {
         BlockStruct *b = doc->getWidthTable()[i];
         b->setShape(T_UNDEFINED);
         b->setSizeA(0.);
         b->setSizeB(0.);
         b->setRotation(0.);
         b->setXoffset(0.);
         b->setYoffset(0.);
         b->setDcode(0);
      }
   }
   FillListCtrl();
}

void CApertureListDialog::OnHelp() 
{ 
	//getApp().OnHelpIndex();
}

/////////////////////////////////////////////////////////////////////////////
// DListCtrl
DListCtrl::DListCtrl()
{
}

DListCtrl::~DListCtrl()
{
}

BEGIN_MESSAGE_MAP(DListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(DListCtrl)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
   ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
   ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DListCtrl message handlers
void DListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
   LV_HITTESTINFO info;
   info.pt.x = point.x;
   info.pt.y = point.y;

   HitTest(&info);

   if (info.flags & LVHT_ONITEM)
   {
      if (selItem == info.iItem)
      {
         if (info.flags & LVHT_ONITEMICON)
         {
            char buf[20];
            BlockStruct *block = (BlockStruct *)GetItemData(info.iItem);
            if (block->getShape() == T_COMPLEX) block->setSizeA(0);
            block->setShape(block->getShape() + 1);
            if (block->getShape() >= MAX_SHAPES) block->setShape(0);
            if (block->getShape() == T_COMPLEX) block->setSizeA(0);
            SetItem(info.iItem, 0, LVIF_IMAGE, NULL, block->getShape(), 0, 0, 0);
            if (block->getShape() == T_COMPLEX)
            {
               BlockStruct *temp = doc->Find_Block_by_Num((int)(block->getSizeA()));
               if (temp)
                  SetItem(info.iItem, 2, LVIF_TEXT, temp->getName(), 0, 0, 0, 0);
               else
                  SetItem(info.iItem, 2, LVIF_TEXT, "", 0, 0, 0, 0);
            }
            else
            {
               sprintf(buf, "%.*lf  %s", decimals, block->getSizeA(), units);
               SetItem(info.iItem, 2, LVIF_TEXT, buf, 0, 0, 0, 0);
            }
            switch (block->getShape())
            {
               case T_RECTANGLE:
               case T_DONUT:
               case T_THERMAL:
               case T_OBLONG:
                  sprintf(buf, "%.*lf  %s", decimals, block->getSizeB(), units);
                  SetItem(info.iItem, 3, LVIF_TEXT, buf, 0, 0, 0, 0);
                  break;
               default:
                  SetItem(info.iItem, 3, LVIF_TEXT, "", 0, 0, 0, 0);
            }
         }
      }

      selItem = info.iItem;
   }
   else selItem = -1;
   
   CListCtrl::OnLButtonDown(nFlags, point);
}

void DListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   LV_HITTESTINFO info;
   info.pt.x = point.x;
   info.pt.y = point.y;

   HitTest(&info);

   if (info.flags & LVHT_ONITEM)
   {
      selItem = info.iItem;

      if (info.flags & LVHT_ONITEMLABEL)
      {
         BlockStruct *block = (BlockStruct *)GetItemData(info.iItem);

         EditAperture(this, block, doc);
      }

   }
   else selItem = -1;
   
   //CListCtrl::OnLButtonDblClk(nFlags, point);
}

void DListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

   BlockStruct *block;
   block = (BlockStruct *)GetItemData(pDispInfo->item.iItem);
   GetEditControl()->GetWindowText(block->getNameRef());
   SetItem(pDispInfo->item.iItem, 0, LVIF_TEXT, block->getName(), 0, 0, 0, 0);

   *pResult = 0;
}

void DListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   if (pNMListView->uNewState == 3) selItem = pNMListView->iItem;

   *pResult = 0;
}

void DListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   SortBy = pNMListView->iSubItem;
   SortItems(CompareFunc, 0);

   *pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CEditAperture dialog
CEditAperture::CEditAperture(CWnd* pParent /*=NULL*/)
   : CDialog(CEditAperture::IDD, pParent)
{
   //{{AFX_DATA_INIT(CEditAperture)
   m_DCode = 0;
   m_Name = _T("");
   m_sizeA = _T("");
   m_sizeB = _T("");
   m_rotation = _T("");
   m_xoffset = _T("");
   m_yoffset = _T("");
   m_sizeC = _T("");
   m_sizeD = _T("");
   m_spokes = _T("");
   //}}AFX_DATA_INIT
}

CEditAperture::~CEditAperture()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void CEditAperture::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CEditAperture)
   DDX_Control(pDX, IDC_BLOCK_CB, m_blockCB);
   DDX_Control(pDX, IDC_SHAPE_CB, m_ShapeCB);
   DDX_Text(pDX, IDC_DCODE, m_DCode);
   DDV_MinMaxInt(pDX, m_DCode, 0, 2147483647);
   DDX_Text(pDX, IDC_NAME, m_Name);
   DDX_Text(pDX, IDC_SIZEA, m_sizeA);
   DDX_Text(pDX, IDC_SIZEB, m_sizeB);
   DDX_Text(pDX, IDC_ROTATION, m_rotation);
   DDX_Text(pDX, IDC_XOFFSET, m_xoffset);
   DDX_Text(pDX, IDC_YOFFSET, m_yoffset);
   DDX_Text(pDX, IDC_SIZEC, m_sizeC);
   DDX_Text(pDX, IDC_SIZED, m_sizeD);
   DDX_Text(pDX, IDC_SPOKES, m_spokes);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditAperture, CDialog)
   //{{AFX_MSG_MAP(CEditAperture)
   ON_CBN_SELCHANGE(IDC_SHAPE_CB, OnSelchangeShapeCb)
   ON_BN_CLICKED(IDHELP, OnHelp)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEditAperture::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_BLOCK_CB:
         {
            int sel = m_blockCB.GetCurSel();
            if (sel == LB_ERR)
               break;
            m_blockCB.GetLBText(sel, hint);
            pTTT->lpszText = hint.GetBuffer(0);
         }
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CEditAperture message handlers
BOOL CEditAperture::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_BLOCK_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

	int i = 0;
   for (i = 0; i < MAX_SHAPES; i++)
      m_ShapeCB.AddString(shapes[i]);
   m_ShapeCB.SetCurSel(m_shape);

   int n;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (!(block->getFlags() & BL_FILE) && !(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_WIDTH))
      {
         n = m_blockCB.AddString(block->getName());
         m_blockCB.SetItemData(n, block->getBlockNumber());
         if (block->getBlockNumber() == m_blockNum)
            m_blockCB.SetCurSel(n);
      }
   }

   if (m_shape == T_COMPLEX)
   {
      GetDlgItem(IDC_SIZEA)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEB)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZED)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SPOKES)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEA_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEB_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEC_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZED_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SPOKES_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_ROTATION)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_ROTATION_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_XOFFSET)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_XOFFSET_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_YOFFSET)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_YOFFSET_STATIC)->ShowWindow(SW_HIDE);
   }
   else
   {
      GetDlgItem(IDC_BLOCK_CB)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_BLOCK_STATIC)->ShowWindow(SW_HIDE);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditAperture::OnSelchangeShapeCb() 
{
   m_shape = m_ShapeCB.GetCurSel(); 

   if (m_shape == T_COMPLEX)
   {
      GetDlgItem(IDC_SIZEA)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEB)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZED)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SPOKES)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEA_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEB_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEC_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZED_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SPOKES_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_ROTATION)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_ROTATION_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_XOFFSET)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_XOFFSET_STATIC)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_YOFFSET)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_YOFFSET_STATIC)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_BLOCK_CB)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_BLOCK_STATIC)->ShowWindow(SW_SHOW);
   }
   else
   {
      GetDlgItem(IDC_BLOCK_CB)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_BLOCK_STATIC)->ShowWindow(SW_HIDE);

      GetDlgItem(IDC_SIZEA)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZEB)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZEC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZED)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SPOKES)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZEA_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZEB_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZEC_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZED_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SPOKES_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ROTATION)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ROTATION_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_XOFFSET)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_XOFFSET_STATIC)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_YOFFSET)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_YOFFSET_STATIC)->ShowWindow(SW_SHOW);
   }
}

void CEditAperture::OnOK() 
{
   m_shape = m_ShapeCB.GetCurSel(); 

   if (m_shape == T_COMPLEX) m_blockNum = (int)(m_blockCB.GetItemData(m_blockCB.GetCurSel()));

   CDialog::OnOK();
}

void CEditAperture::OnHelp() 
{ 
	//getApp().OnHelpIndex(); 
}




