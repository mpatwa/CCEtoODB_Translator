// $Header: /CAMCAD/4.6/ToolList.cpp 23    5/29/07 10:47p Rick Faltersack $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "toollist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define iUndefined   0
#define iDrill       1

void CCEtoODBDoc::OnToolList() 
{
   ListTools dlg;
   dlg.doc = this;
   if (dlg.DoModal() == IDOK) 
      UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// ListTools dialog
ListTools::ListTools(CWnd* pParent /*=NULL*/)
   : CResizingDialog(ListTools::IDD, pParent)
{
   //{{AFX_DATA_INIT(ListTools)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   addFieldControl(IDOK         ,anchorBottom);
   addFieldControl(IDC_EDIT     ,anchorBottom);
   addFieldControl(IDC_ADD      ,anchorBottom);
   addFieldControl(IDC_SETTCODES,anchorBottom);
   addFieldControl(IDC_PRINT    ,anchorBottom);
   addFieldControl(IDC_LIST     ,anchorLeft  ,growBoth);
}

ListTools::~ListTools()
{
   if (m_imageList)
      delete m_imageList;
}

void ListTools::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ListTools)
   DDX_Control(pDX, IDC_LIST, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ListTools, CResizingDialog)
   //{{AFX_MSG_MAP(ListTools)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_SETTCODES, OnSettcodes)
   ON_BN_CLICKED(IDC_PRINT, OnPrint)
   ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ListTools message handlers
BOOL ListTools::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   m_decimals = GetDecimals(doc->getSettings().getPageUnits());

   LoadToolIconImages();

   // columns
   LV_COLUMN column;
   CRect rect;
   m_list.GetWindowRect(&rect);
   int width = (rect.Width() - 25) / 8;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width*2;
   column.pszText = "Tool Name";
   column.iSubItem = 0;
   m_list.InsertColumn(0, &column);

   column.cx = width;
   column.pszText = "Exported As";
   column.iSubItem = 1;
   m_list.InsertColumn(1, &column);

   column.pszText = "Size";
   column.iSubItem = 2;
   m_list.InsertColumn(2, &column);

   column.pszText = "Type";
   column.iSubItem = 3;
   m_list.InsertColumn(4, &column);

   column.cx = 25;
   column.pszText = "Display";
   column.iSubItem = 4;
   m_list.InsertColumn(4, &column);

   column.pszText = "Plated";
   column.iSubItem = 5;
   m_list.InsertColumn(5, &column);

   column.cx = width*2;
   column.pszText = "Geometry";
   column.iSubItem = 6;
   m_list.InsertColumn(6, &column);

   column.cx = width*6;
   column.pszText = "Comment";
   column.iSubItem = 7;
   m_list.InsertColumn(7, &column);

   FillListCtrl();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

//
/////////////////////////////////////////////////////////////////////////
//
// This LoadToolIconImages and the GetToolIconImageIndex need to be
// kept in sync. The index depends on the order in which they are loaded.
//

void ListTools::LoadToolIconImages()
{
   CWinApp *app = AfxGetApp();

   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 3, 0);

   m_imageList->Add(app->LoadIcon(IDI_UNDEFINED));  // Image 0
   m_imageList->Add(app->LoadIcon(IDI_DRILL));      // Image 1
   m_imageList->Add(app->LoadIcon(IDI_COMPLEX));    // Image 2

   m_list.SetImageList(m_imageList, LVSIL_SMALL);
}

//------------------------------------------------------------

int ListTools::GetToolIconImageIndex(BlockStruct *toolBlk)
{
   int imageIndx = 0; // default to undefined

   // If block is "regular tool" and size > min size then use DRILL icon
   if (((toolBlk->getFlags() & BL_TOOL) || (toolBlk->getFlags() & BL_BLOCK_TOOL)) &&
      (toolBlk->getToolSize() > SMALLNUMBER))
      imageIndx = 1;

   // If block is complex tool, set COMPLEX icon, don't check irrelvant toolSize
   if (toolBlk->getFlags() & BL_COMPLEX_TOOL)
         imageIndx = 2;

   return imageIndx;
}

//
/////////////////////////////////////////////////////////////////////////
//

void ListTools::FillListCtrl()
{
   LV_ITEM item;
   int actualItem;
   char buf[20];

   m_list.DeleteAllItems();

   // items
   int j = 0;
   for (int i=0; i<doc->getNextWidthIndex(); i++)
   {
      BlockStruct *block = doc->getWidthTable()[i];

      if (!block)
         continue;

      if (!(block->getFlags() & BL_TOOL) && !(block->getFlags() & BL_BLOCK_TOOL)
         && !(block->getFlags() & BL_COMPLEX_TOOL))
         continue;

      item.mask = LVIF_TEXT | LVIF_IMAGE;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = block->getNameRef().GetBuffer(0);
      item.iImage = GetToolIconImageIndex(block);
      actualItem = m_list.InsertItem(&item);
//    if (actualItem == -1) ErrorMessage("Insert Failed");
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_list.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_list.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_list.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_list.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_list.InsertItem(&item);
               }
            }
         }
      }

      m_list.SetItemData(actualItem, (LPARAM)block);

      item.mask = LVIF_TEXT;
      item.iItem = actualItem;
      item.iSubItem = 1;
      sprintf(buf, "T%d", block->getTcode());
      item.pszText = buf;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      sprintf(buf, "%.*lf", m_decimals, block->getToolSize());
      item.pszText = buf;
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = "";
      m_list.SetItem(&item);

      item.iSubItem = 4;
      if (block->getToolDisplay())
         item.pszText = "X";
      else
         item.pszText = "";
      m_list.SetItem(&item);

      item.iSubItem = 5;
      if (block->getToolHolePlated())
         item.pszText = "X";
      else
         item.pszText = "";
      m_list.SetItem(&item);

      item.iSubItem = 6;
      item.pszText = "";
      if (block->getToolBlockNumber())
         item.pszText = doc->Find_Block_by_Num(block->getToolBlockNumber())->getNameRef().GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 7;
      item.pszText = "";
      m_list.SetItem(&item);
   }

// m_list.SortItems(CompareFunc, 0);
}

void ListTools::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   
   *pResult = 0;
}

void ListTools::OnEdit() 
{
   // get selected geometry
   int count = m_list.GetItemCount();

   if (!count)
      return;

   BOOL Selected = FALSE;

	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_list.GetItemState(selItem, LVIS_SELECTED))
      {
         Selected = TRUE;
         break;
      }
   }
   if (!Selected)
      return;

   BlockStruct *block = (BlockStruct*)m_list.GetItemData(selItem);

   EditTool dlg;
   dlg.doc = doc;
   dlg.m_name = block->getName();
   dlg.m_size.Format("%.*lf", GetDecimals(doc->getSettings().getPageUnits()), block->getToolSize());
   dlg.m_tcode = block->getTcode();
   dlg.m_display = block->getToolDisplay();
   dlg.m_plated = block->getToolHolePlated();
   dlg.displayBlockNum = block->getToolBlockNumber();
   if (dlg.DoModal() == IDOK)
   {
      block->setName(dlg.m_name);
      block->setToolSize((DbUnit)atof(dlg.m_size));
      block->setTcode(dlg.m_tcode);
      block->setToolDisplay(dlg.m_display);
      block->setToolHolePlated(dlg.m_plated);
      block->setToolBlockNumber(dlg.displayBlockNum);

      LV_ITEM item;
      item.mask = LVIF_TEXT | LVIF_IMAGE;
      item.iItem = selItem;
      item.iSubItem = 0;
      item.iImage = GetToolIconImageIndex(block);
      item.pszText = block->getNameRef().GetBuffer(1);
      m_list.SetItem(&item);

      char buf[20];
      item.mask = LVIF_TEXT;
      item.iSubItem = 1;
      sprintf(buf, "T%d", block->getTcode());
      item.pszText = buf;
      m_list.SetItem(&item);

      item.iSubItem = 2;
      sprintf(buf, "%.*lf", m_decimals, block->getToolSize());
      item.pszText = buf;
      m_list.SetItem(&item);

      item.iSubItem = 3;
      item.pszText = "";
      m_list.SetItem(&item);

      item.iSubItem = 4;
      if (block->getToolDisplay())
         item.pszText = "X";
      else
         item.pszText = "";
      m_list.SetItem(&item);

      item.iSubItem = 5;
      if (block->getToolHolePlated())
         item.pszText = "X";
      else
         item.pszText = "";
      m_list.SetItem(&item);

      item.iSubItem = 6;
      item.pszText = "";
      if (block->getToolBlockNumber())
         item.pszText = doc->Find_Block_by_Num(block->getToolBlockNumber())->getNameRef().GetBuffer(0);
      m_list.SetItem(&item);

      item.iSubItem = 7;
      item.pszText = "";
      m_list.SetItem(&item);
   }
}

void ListTools::OnAdd() 
{
   static int toolnum = 1;
   CString buf;
   buf.Format("NewTool_%d", toolnum++);

   // create a new tool
   BlockStruct *block;
   block = doc->Add_Blockname(buf, -1, BL_TOOL | BL_GLOBAL, TRUE);
   block->setBlockType(BLOCKTYPE_TOOLING);
   //int i = doc->getNextWidthIndex()++;
   //doc->getWidthTable().SetAtGrow(i, block);
   doc->getWidthTable().Add(block);

   FillListCtrl();
}

void ListTools::OnSettcodes() 
{
   // set default toolcodes
   char used[1000];

   // default toolcodes 
   int default_TCode = 1;

   memset(used, 0, 1000);

   // set which toolcodes have been used
	int i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (!(block->getFlags() & BL_TOOL 
         || block->getFlags() & BL_BLOCK_TOOL
         || block->getFlags() & BL_COMPLEX_TOOL)) continue;

      if (used[block->getTcode()]) // if already used
         block->setTcode(0); // so it will get set to an unused toolcode
      else
         used[block->getTcode()] = TRUE; // set as used
   }

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (!(block->getFlags() & BL_TOOL 
         || block->getFlags() & BL_BLOCK_TOOL
         || block->getFlags() & BL_COMPLEX_TOOL)) continue;

      if (block->getTcode() == 0)
      {
         // find next unused dcode
         while (default_TCode < 999 && used[default_TCode])
            default_TCode++;
         block->setTcode(default_TCode);
         if (default_TCode < 999) default_TCode++;
      }
   }

   FillListCtrl();
}

// Column offsets
#define COL1      0
#define COL2     0.25
#define COL3     0.35
#define COL4     0.45
#define COL5     0.55
#define COL6     0.65
#define PAGE      0.9
#define START     20

void ListTools::PrintHeader(CDC *dc, int hRes, int vRes, int space, int page)
{
   dc->TextOut(round(COL1       ), 0, "Tool Name");
   dc->TextOut(round(COL2 * hRes), 0, "Export As");
   dc->TextOut(round(COL3 * hRes), 0, "Size");
   dc->TextOut(round(COL4 * hRes), 0, "Display");
   dc->TextOut(round(COL5 * hRes), 0, "Plated");
   dc->TextOut(round(COL6 * hRes), 0, "Geometry");
   CString buf;
   buf.Format("Page %d", page);
   dc->TextOut(round(PAGE * hRes), 0, buf);

   dc->MoveTo(0, space); // header line
   dc->LineTo(hRes, space);   

   dc->MoveTo(0, vRes - space - START); // footer line
   dc->LineTo(hRes, vRes - space - START);   

   CTime t = t.GetCurrentTime(); // footer text
   buf.Format("Created by CCE to ODB++ on %s", t.Format("%A, %B %d, %Y at %I:%M:%S %p"));
   dc->TextOut(0, vRes - space, buf);
}
  
void ListTools::OnPrint() 
{
   // Stolen from/patterned after Apreture List OnPrint()

   CDC dc;
   CPrintDialog dlg(FALSE); // FALSE - not print setup dialog
   
   if (dlg.DoModal() != IDOK) return;

   dc.Attach(dlg.GetPrinterDC());

   if (!dc.m_hDC) return;

   DOCINFO di;
   ::ZeroMemory(&di, sizeof(DOCINFO));
   di.cbSize = sizeof(DOCINFO);
   di.lpszDocName = "Tool List Printout"; // title of print document

   dc.StartDoc(&di);
   dc.StartPage();

   {
      CString buf;
      int y = START; 
      int page = 1;
      int items;
      int decimals = GetDecimals(doc->getSettings().getPageUnits());
      int hRes = dc.GetDeviceCaps(HORZRES); // pixels wide
      int vRes = dc.GetDeviceCaps(VERTRES); // pixels high

      TEXTMETRIC met;
      dc.GetTextMetrics(&met);
      int space = met.tmHeight;

      PrintHeader(&dc, hRes, vRes, space, page++);

      if (items = m_list.GetItemCount())
      {
         int i = m_list.GetNextItem(-1, LVNI_ALL); // get first item on list

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

            BlockStruct *b = (BlockStruct *)m_list.GetItemData(i); // get Block ptr for this item

            dc.TextOut(COL1, y, b->getName());

            CString tcode;
            tcode.Format("T%d", b->getTcode());
            dc.TextOut(round(COL2 * hRes), y, tcode);

            CString toolsize;
            toolsize.Format("%0.*f", m_decimals, (double)b->getToolSize());
            dc.TextOut(round(COL3 * hRes), y, toolsize);

            CString tooldisplay;
            tooldisplay.Format("%s", b->getToolDisplay() ? "X" : " ");
            dc.TextOut(round(COL4 * hRes), y, tooldisplay);

            CString toolplated;
            toolplated.Format("%s", b->getToolHolePlated() ? "X" : " ");
            dc.TextOut(round(COL5 * hRes), y, toolplated);

            CString toolgeometry;
            if (b->getToolBlockNumber() != 0)
            {
               BlockStruct *toolblk = doc->getBlockAt(b->getToolBlockNumber());
               toolgeometry.Format("%s", toolblk != NULL ? toolblk->getName() : " ");
            }
            dc.TextOut(round(COL6 * hRes), y, toolgeometry);

            if (items > 1)
               i = m_list.GetNextItem(i, LVNI_BELOW); // get next item on list
            else 
               break;
         }
      }
   }

   dc.EndPage();
   dc.EndDoc();
}

void ListTools::OnOK() 
{
   // TODO: Add extra validation here
   
   CResizingDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// EditTool dialog
EditTool::EditTool(CWnd* pParent /*=NULL*/)
   : CDialog(EditTool::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditTool)
   m_display = FALSE;
   m_name = _T("");
   m_size = _T("");
   m_tcode = 0;
   //}}AFX_DATA_INIT
}

void EditTool::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditTool)
   DDX_Control(pDX, IDC_BLOCKS, m_blockCB);
   DDX_Check(pDX, IDC_DISPLAY, m_display);
   DDX_Text(pDX, IDC_NAME, m_name);
   DDX_Text(pDX, IDC_TOOL_SIZE, m_size);
   DDX_Text(pDX, IDC_TCODE, m_tcode);
   //}}AFX_DATA_MAP
   DDX_Check(pDX, IDC_PLATED, m_plated);
}

BEGIN_MESSAGE_MAP(EditTool, CDialog)
   //{{AFX_MSG_MAP(EditTool)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditTool message handlers
BOOL EditTool::OnInitDialog() 
{
   CDialog::OnInitDialog();

   int cur;
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_FILE || block->getFlags() & BL_WIDTH)
         continue;
      cur = m_blockCB.AddString(block->getName());
      m_blockCB.SetItemData(cur, block->getBlockNumber());
      if (block->getBlockNumber() == displayBlockNum)
         m_blockCB.SetCurSel(cur);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditTool::OnOK() 
{
   int sel = m_blockCB.GetCurSel();
   if (sel != -1)
      displayBlockNum = m_blockCB.GetItemData(sel);
   else
      displayBlockNum = 0;
   
   CDialog::OnOK();
}
