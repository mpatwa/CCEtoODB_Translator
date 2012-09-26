// $Header: /CAMCAD/4.6/GeomList.cpp 21    12/07/06 12:05p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "StdAfx.h"
#include "CCEtoODB.h"
#include "Geomlist.h"
#include "graph.h"
#include "pcbutil.h"
#include "flags.h"
#include "DcaEnumIterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CCEtoODBDoc::OnGeometryList() 
{ 
   if (CGeometryListDialog(this).DoModal() == IDOK)
      UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CGeometryListDialog dialog
CGeometryListDialog::CGeometryListDialog(CCEtoODBDoc *Doc, CWnd* pParent /*=NULL*/)
   : CResizingDialog(CGeometryListDialog::IDD, pParent)
{
   doc = Doc;

   //{{AFX_DATA_INIT(CGeometryListDialog)
   m_ShowApertures = TRUE;
   m_ShowBlocks = TRUE;
   m_ShowFiles = FALSE;
   m_ShowWidths = TRUE;
   m_ShowTools = TRUE;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_STATIC5             ,anchorBottomLeft );
   addFieldControl(IDC_COPY                ,anchorBottomLeft );
   addFieldControl(IDC_FLAGS               ,anchorBottomLeft );
   addFieldControl(IDOK                    ,anchorBottomLeft );

   CResizingDialogField& f1 = addFieldControl(IDC_FILES               ,anchorLeft   ,growProportionalHorizontal);
   f1.getOperations().addOperation(growVertical);

   CResizingDialogField& f2 = addFieldControl(IDC_GEOMETRIES          ,anchorRight  ,growVertical    );
   f2.getOperations().addOperation(glueLeftEdge,toRightEdge,&f1);

   CResizingDialogField& f3 = addFieldControl(IDC_TYPE_CB             ,anchorBottomLeft);
   f3.getOperations().addOperation(glueRightEdge,toRightEdge,&f1);

   CResizingDialogField& f4 = addFieldControl(IDC_IS_APERTURE         ,anchorBottom);
   f4.getOperations().addOperation(glueLeftEdge ,toRightEdge,&f3);
   f4.getOperations().addOperation(glueRightEdge,toRightEdge,&f3);

   addFieldControl(IDC_STATIC2             ,anchorRelativeToField,&f4);
   addFieldControl(IDC_LOAD_FROM_FILE      ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SAVE_APERTURE_ASSIGN,anchorRelativeToField,&f4);
   addFieldControl(IDC_IS_TOOL             ,anchorRelativeToField,&f4);
   addFieldControl(IDC_STATIC3             ,anchorRelativeToField,&f4);
   addFieldControl(IDC_LOAD_TOOL_ASSIGN    ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SAVE_TOOL_ASSIGN    ,anchorRelativeToField,&f4);
   addFieldControl(IDC_STATIC4             ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SHOW_FILES          ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SHOW_BLOCKS         ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SHOW_APERTURES      ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SHOW_TOOLS          ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SHOW_WIDTHS         ,anchorRelativeToField,&f4);
   addFieldControl(IDC_SHOW_FILES          ,anchorRelativeToField,&f4);
}

void CGeometryListDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CGeometryListDialog)
   DDX_Control(pDX, IDC_TYPE_CB, m_typeCB);
   DDX_Control(pDX, IDC_IS_TOOL, m_IsTool);
   DDX_Control(pDX, IDC_GEOMETRIES, m_geomLC);
   DDX_Control(pDX, IDC_IS_APERTURE, m_IsAperture);
   DDX_Control(pDX, IDC_FILES, m_FilesLB);
   DDX_Check(pDX, IDC_SHOW_APERTURES, m_ShowApertures);
   DDX_Check(pDX, IDC_SHOW_BLOCKS, m_ShowBlocks);
   DDX_Check(pDX, IDC_SHOW_FILES, m_ShowFiles);
   DDX_Check(pDX, IDC_SHOW_WIDTHS, m_ShowWidths);
   DDX_Check(pDX, IDC_SHOW_TOOLS, m_ShowTools);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGeometryListDialog, CResizingDialog)
   //{{AFX_MSG_MAP(CGeometryListDialog)
   ON_BN_CLICKED(IDC_IS_APERTURE, OnIsAperture)
   ON_BN_CLICKED(IDC_SHOW_APERTURES, FillGeometryLC)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_GEOMETRIES, OnItemchangedGeometries)
   ON_NOTIFY(NM_DBLCLK, IDC_GEOMETRIES, OnDblclkGeometries)
   ON_BN_CLICKED(IDC_IS_TOOL, OnIsTool)
   ON_BN_CLICKED(IDC_COPY, OnCopy)
   ON_CBN_SELCHANGE(IDC_TYPE_CB, OnSelchangeTypeCb)
   ON_BN_CLICKED(IDC_LOAD_APERTURE_ASSIGN, OnLoadApertureAssign)
   ON_BN_CLICKED(IDC_SAVE_APERTURE_ASSIGN, OnSaveApertureAssign)
   ON_BN_CLICKED(IDC_LOAD_TOOL_ASSIGN, OnLoadToolAssign)
   ON_BN_CLICKED(IDC_SAVE_TOOL_ASSIGN, OnSaveToolAssign)
   ON_BN_CLICKED(IDC_SHOW_BLOCKS, FillGeometryLC)
   ON_BN_CLICKED(IDC_SHOW_FILES, FillGeometryLC)
   ON_BN_CLICKED(IDC_SHOW_WIDTHS, FillGeometryLC)
   ON_LBN_SELCHANGE(IDC_FILES, FillGeometryLC)
   ON_LBN_DBLCLK(IDC_BLOCKS, OnIsAperture)
   ON_BN_CLICKED(IDC_SHOW_TOOLS, FillGeometryLC)
   ON_BN_CLICKED(IDC_FLAGS, OnFlags)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeometryListDialog message handlers
BOOL CGeometryListDialog::OnInitDialog() 
{
   decimals = GetDecimals(doc->getSettings().getPageUnits());

   CResizingDialog::OnInitDialog();

   for (EnumIterator(BlockTypeTag,blockTypeTagIterator);blockTypeTagIterator.hasNext();)
   {
      BlockTypeTag blockType = blockTypeTagIterator.getNext();

      m_typeCB.AddString(blockTypeToDisplayString(blockType));
   }
   
   // columns
   LV_COLUMN column;
   CRect rect;
   m_geomLC.GetWindowRect(&rect);
   int width = (rect.Width() - 70) / 4;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.cx = width;

   column.pszText = "Name";
   column.iSubItem = 0;
   m_geomLC.InsertColumn(0, &column);

   column.pszText = "Type";
   column.iSubItem = 1;
   m_geomLC.InsertColumn(1, &column);

   column.cx = 35;
   column.pszText = "Ap";
   column.iSubItem = 2;
   m_geomLC.InsertColumn(2, &column);

   column.pszText = "Tool";
   column.iSubItem = 3;
   m_geomLC.InsertColumn(3, &column);

   column.cx = width;
   column.pszText = "Ap Size";
   column.iSubItem = 4;
   m_geomLC.InsertColumn(4, &column);

   column.cx = width;
   column.pszText = "Tool Size";
   column.iSubItem = 5;
   m_geomLC.InsertColumn(5, &column);

   // add files
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *npf = doc->getFileList().GetNext(pos);
      m_FilesLB.SetItemDataPtr(m_FilesLB.AddString(npf->getName()), npf);
   }
   m_FilesLB.SetCurSel(0);

   m_FilesLB.SetHorizontalExtent(1000);

   FillGeometryLC();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeometryListDialog::AddGeometry(BlockStruct *ptr,const char* type,const char* apSize,const char* toolSize)
{
   LV_ITEM item;
   int actualItem;

   item.mask = LVIF_TEXT | LVIF_IMAGE;
   item.iItem = itemnum++;
   item.iSubItem = 0;
   item.pszText = ptr->getNameRef().GetBuffer(0);
// item.iImage = ;
   actualItem = m_geomLC.InsertItem(&item);
   // ugly, dirty way to try to work around microsoft bug
   if (actualItem == -1)
   {
      actualItem = m_geomLC.InsertItem(&item);
      if (actualItem == -1)
      {
         actualItem = m_geomLC.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_geomLC.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_geomLC.InsertItem(&item);
               if (actualItem == -1)
                  actualItem = m_geomLC.InsertItem(&item);
            }
         }
      }
   }

   m_geomLC.SetItemData(actualItem, (LPARAM)ptr);

   item.mask = LVIF_TEXT;
   item.iItem = actualItem;
   item.iSubItem = 1;
   item.pszText = (char*)type;
   m_geomLC.SetItem(&item);

   item.iSubItem = 2;

   if (ptr->getFlags() & BL_APERTURE 
      || ptr->getFlags() & BL_BLOCK_APERTURE
      || ptr->getFlags() & BL_COMPLEX_TOOL)
      item.pszText = "A";
   else
      item.pszText = "";

   m_geomLC.SetItem(&item);

   item.iSubItem = 3;

   if (ptr->getFlags() & BL_TOOL || ptr->getFlags() & BL_BLOCK_TOOL)
      item.pszText = "T";
   else
      item.pszText = "";

   m_geomLC.SetItem(&item);

   item.iSubItem = 4;
   item.pszText = (char*)apSize;
   m_geomLC.SetItem(&item);

   item.iSubItem = 5;
   item.pszText = (char*)toolSize;
   m_geomLC.SetItem(&item);
}

void CGeometryListDialog::FillGeometryLC() 
{
   int filenum;
   CString apSize, toolSize;

   UpdateData();

   if (m_FilesLB.GetCount())
      filenum = ((FileStruct*)(m_FilesLB.GetItemDataPtr(m_FilesLB.GetCurSel())))->getFileNumber();
   else 
      filenum = 0;

   m_geomLC.DeleteAllItems();

   itemnum = 0;
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (m_ShowFiles) 
      {
         if ( (block->getFileNumber() == filenum) &&    // if it belongs to the selected file
              (block->getFlags() & BL_FILE) )         // AND is a file block
         {
            AddGeometry(block, "File", "", "");
            continue;     // don't add a string twice
         }
      }

      if (m_ShowBlocks)
      {
         if ( (block->getFileNumber() == filenum || block->getFileNumber() == -1) && // this file or global
              !(block->getFlags() & BL_FILE) &&          // AND not a file block
              !(block->getFlags() & BL_APERTURE) &&      // AND not a loaded aperature
              !(block->getFlags() & BL_TOOL) )           // AND not a tool
         {
            AddGeometry(block, blockTypeToDisplayString(block->getBlockType()), "", "");
            continue;     // don't add a string twice
         }
      }

      if (m_ShowApertures)
      {
         if ((block->getFlags() & BL_APERTURE ||   // loaded aperature
             (block->getFlags() & BL_BLOCK_APERTURE && 
             (block->getFileNumber() == filenum || block->getFileNumber() == -1) ) ) )
         {
            apSize = toolSize = "";

            if (block->getShape() != T_COMPLEX)
               apSize.Format("%.*lf", decimals, block->getSizeA());

            if ((block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL) && !block->getToolDisplay())
               toolSize.Format("%.*lf", decimals, block->getToolSize()); 

            if (block->getFlags() & BL_WIDTH)
               AddGeometry(block, "Width", apSize.GetBuffer(0), toolSize.GetBuffer(0));
            else if (block->getFlags() & BL_APERTURE)
               AddGeometry(block, "Aperture", apSize.GetBuffer(0), toolSize.GetBuffer(0));
            else // block aperture
               AddGeometry(block, blockTypeToDisplayString(block->getBlockType()), "", toolSize.GetBuffer(0));
            continue;
        }
      }

      if (m_ShowTools)
      {
         if ((block->getFlags() & BL_TOOL || 
             (block->getFlags() & BL_BLOCK_TOOL && 
             (block->getFileNumber() == filenum || block->getFileNumber() == -1) ) ) )
         {
            apSize = toolSize = "";

            if (!block->getToolDisplay())
               toolSize.Format("%.*lf", decimals, block->getToolSize());

            if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE) 
               apSize.Format("%.*lf", decimals, block->getSizeA()); 

            AddGeometry(block, blockTypeToDisplayString(block->getBlockType()), apSize.GetBuffer(0), toolSize.GetBuffer(0));
            continue;
        }
      }
   }

   if (m_ShowWidths)
   {
      for (int i=0; i < doc->getNextWidthIndex(); i++)
      {
         BlockStruct *width = doc->getWidthTable()[i];
         if (!width) continue;

         if (width->getFlags() & BL_WIDTH && 
            !(m_ShowApertures && width->getFlags() & BL_APERTURE) && // and didn't already show as an aperture
            !(m_ShowTools && width->getFlags() & BL_TOOL) ) // and didn't already show as a tool
         {
            apSize.Format("%.*lf", decimals, width->getSizeA());
            AddGeometry(width, "Width", apSize.GetBuffer(0), "");
         }
      }
   }
}

void CGeometryListDialog::OnIsAperture() 
{
   // if nothing selected
   if (!m_geomLC.GetSelectedCount())
   {
      m_IsAperture.SetCheck(2); 
      return;
   }

   // get selected geometry
   int count = m_geomLC.GetItemCount();
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_geomLC.GetItemState(selItem, LVIS_SELECTED))
         break;
   }
   BlockStruct *block = (BlockStruct*)m_geomLC.GetItemData(selItem);

   // FILE
   if (block->getFlags() & BL_FILE)
   {
      m_IsAperture.SetCheck(2); // IsAperature does not apply to file-blocks
      return;
   }

   // loaded aperture 
   if (block->getFlags() & BL_APERTURE)
   {
      m_IsAperture.SetCheck(1); // Loaded aperatures are always checked
      return;
   }

   // width
   if (block->getFlags() & BL_WIDTH)
   {
      m_IsAperture.SetCheck(1); // Width are always checked
      return;
   }

   // generic block
   if (block->getFlags() & BL_BLOCK_APERTURE) // if was a block-aperture
   {
      block->clearFlagBits(BL_BLOCK_APERTURE);
      m_IsAperture.SetCheck(0);

      // make width table entry a hole if it should no longer be there
      if (!(block->getFlags() & BL_TOOL) && !(block->getFlags() & BL_BLOCK_TOOL))
      {
         for (int i = 0; i < doc->getNextWidthIndex(); i++)
            if (doc->getWidthTable()[i] == block)
            {
               doc->getWidthTable().SetAt(i,NULL);
               break;
            }
      }

      // special case
      // if only listing aperatures then
      // when this block-aperature is no longer an aperture, it should not be in list any more
      if (!m_ShowBlocks && m_ShowApertures)
      {
         m_geomLC.DeleteItem(selItem);
         return;
      }
   
      CString buf = "";
      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = selItem;
      item.iSubItem = 2;
      item.pszText = buf.GetBuffer(0);
      m_geomLC.SetItem(&item);

      item.iSubItem = 4;
      m_geomLC.SetItem(&item);
   }
   else // Block that is not set to block-aperature
   {
      if (!(block->getFlags() & BL_TOOL) && !(block->getFlags() & BL_BLOCK_TOOL))
      {
         // doc->widthTable[doc->nextWidthIndex++] = block;
         //doc->getWidthTable().SetAtGrow(doc->getNextWidthIndex()++, block);
         doc->getWidthTable().Add(block);
      }

      block->setFlagBits(BL_BLOCK_APERTURE);
      m_IsAperture.SetCheck(1);

      CString buf = "A";
      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = selItem;
      item.iSubItem = 2;
      item.pszText = buf.GetBuffer(0);
      m_geomLC.SetItem(&item);

      item.iSubItem = 4;
      if (block->getShape() != T_COMPLEX)
         buf.Format("%.*lf", decimals, block->getSizeA());
      else
         buf = "";
      item.pszText = buf.GetBuffer(0);
      m_geomLC.SetItem(&item);
   }
}

void CGeometryListDialog::OnIsTool() 
{
   // if nothing selected
   if (!m_geomLC.GetSelectedCount())
   {
      m_IsTool.SetCheck(2); 
      return;
   }

   // get selected geometry
   int count = m_geomLC.GetItemCount();
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_geomLC.GetItemState(selItem, LVIS_SELECTED))
         break;
   }
   BlockStruct *block = (BlockStruct*)m_geomLC.GetItemData(selItem);

   // FILE
   if (block->getFlags() & BL_FILE)
   {
      m_IsTool.SetCheck(2); // IsTool does not apply to file-blocks
      return;
   }

   // tool
   if (block->getFlags() & BL_TOOL)
   {
      m_IsTool.SetCheck(1); // Tools are always checked
      return;
   }

   // generic block
   if (block->getFlags() & BL_BLOCK_TOOL) // if was a block-aperture
   {
      block->clearFlagBits(BL_BLOCK_TOOL);
      m_IsTool.SetCheck(0);

      // make width table entry a hole if it should no longer be there
      if (!(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_BLOCK_APERTURE))
      {
         for (int i = 0; i < doc->getNextWidthIndex(); i++)
            if (doc->getWidthTable()[i] == block)
            {
               doc->getWidthTable().SetAt(i,NULL);
               break;
            }
      }

      // special case
      // if only listing aperatures then
      // when this block-tool is no longer a tool, it should not be in list any more
      if (!m_ShowBlocks && m_ShowTools)
      {
         m_geomLC.DeleteItem(selItem);
         return;
      }
   
      CString buf = "";
      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = selItem;
      item.iSubItem = 3;
      item.pszText = buf.GetBuffer(0);
      m_geomLC.SetItem(&item);

      item.iSubItem = 5;
      m_geomLC.SetItem(&item);
   }
   else // Block that is not set to block-aperature
   {
      if (!(block->getFlags() & BL_APERTURE) && !(block->getFlags() & BL_BLOCK_APERTURE))
      {
         //doc->getWidthTable().SetAtGrow(doc->getNextWidthIndex()++, block);
         doc->getWidthTable().Add(block);
      }

      block->setFlagBits(BL_BLOCK_TOOL);
      m_IsTool.SetCheck(1);

      CString buf = "T";
      LV_ITEM item;
      item.mask = LVIF_TEXT;
      item.iItem = selItem;
      item.iSubItem = 3;
      item.pszText = buf.GetBuffer(0);
      m_geomLC.SetItem(&item);

      block->setToolSize(block->getSizeA());

      item.iSubItem = 5;
      if (!block->getToolDisplay())
         buf.Format("%.*lf", decimals, block->getToolSize());
      else
         buf = "";
      item.pszText = buf.GetBuffer(0);
      m_geomLC.SetItem(&item);
   }
}

void CGeometryListDialog::OnItemchangedGeometries(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   
   *pResult = 0;

   // if nothing selected
   if (!m_geomLC.GetSelectedCount())
   {
      m_IsAperture.SetCheck(2); 
      m_IsTool.SetCheck(2); 
      return;
   }

   // get selected geometry
   int count = m_geomLC.GetItemCount();
	int selItem=0; 
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_geomLC.GetItemState(selItem, LVIS_SELECTED))
         break;
   }
   BlockStruct *block = (BlockStruct*)m_geomLC.GetItemData(selItem);

   m_typeCB.SetCurSel(block->getBlockType());

   // Loaded Aperture
   if (block->getFlags() & BL_APERTURE) // do not let user change loaded aperature to "NOT AN APERATIRE"
   {
      m_IsAperture.SetCheck(1);
   }
   // File Block
   else if (block->getFlags() & BL_FILE)  // file blocks can not be aperatures
   {
      m_IsAperture.SetCheck(2);
   }
   // generic block
   else // if found in list, and not loaded aperature, and not file, then it is checked according to flag
   {
      m_IsAperture.SetCheck((block->getFlags() & BL_BLOCK_APERTURE) ? 1 : 0);
   }

   // Tool
   if (block->getFlags() & BL_TOOL) // do not let user change tool to "NOT A TOOL"
   {
      m_IsTool.SetCheck(1);
   }
   // File Block
   else if (block->getFlags() & BL_FILE)  // file blocks can not be aperatures
   {
      m_IsTool.SetCheck(2);
   }
   // generic block
   else // if found in list, and not tool, and not file, then it is checked according to flag
   {
      m_IsTool.SetCheck((block->getFlags() & BL_BLOCK_TOOL) ? 1 : 0);
   }
}

void CGeometryListDialog::OnDblclkGeometries(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnIsAperture();   
   *pResult = 0;
}

void CGeometryListDialog::OnCopy() 
{
   // if nothing selected
   if (!m_geomLC.GetSelectedCount())
      return;

   // get selected geometry
   int count = m_geomLC.GetItemCount();
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_geomLC.GetItemState(selItem, LVIS_SELECTED))
         break;
   }
   BlockStruct *block = (BlockStruct*)m_geomLC.GetItemData(selItem);

   // make sure ok to copy this block (for example, not a file, not an aperture)
//WOLF

   if (block->getFlags() & BL_APERTURE)
   {
      MessageBox("Can not copy an Aperture!");
      return;
   }

   if (block->getFlags() & BL_TOOL)
   {
      MessageBox("Can not copy a Tool!");
      return;
   }

   if (block->getFlags() & BL_FILE)
   {
      MessageBox("Can not copy a File Block!");
      return;
   }

   if (block->getFlags() & (BL_WIDTH || BL_SMALLWIDTH))
   {
      MessageBox("Can not copy a Width Block!");
      return;
   }

   // get name of copy
   InputDlg dlg;
   dlg.m_prompt = "Enter geometry name for copy:";
   if (dlg.DoModal() != IDOK)
      return;

   if (Graph_Block_Exists(doc, dlg.m_input, -1))
   {
      MessageBox("Can not copy : Block Name already exist!");
      return;
   }

   // graph new block
   BlockStruct *newBlock;

//WOLF

   newBlock = Graph_Block_On(GBO_APPEND,dlg.m_input,-1,0);

   newBlock->setToolType(block->getToolType());
   newBlock->setFileNumber(block->getFileNumber());
   newBlock->setFlags(block->getFlags());
   newBlock->setBlockType(block->getBlockType());

   // loop through every pinnr and update the pinname
   Graph_Block_Copy(block, 0, 0, 0, 0, 1, 0, TRUE);
   Graph_Block_Off();


   // add geometry to geometry list
   AddGeometry(newBlock, blockTypeToDisplayString(block->getBlockType()), "", "");
}

void CGeometryListDialog::OnFlags() 
{
   // if nothing selected
   if (!m_geomLC.GetSelectedCount())
      return;

   // get selected geometry
   int count = m_geomLC.GetItemCount();
	int selItem=0; 
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_geomLC.GetItemState(selItem, LVIS_SELECTED))
         break;
   }
   BlockStruct *block = (BlockStruct*)m_geomLC.GetItemData(selItem);

   FlagsDlg dlg;
   dlg.group = Block;
   dlg.item = block;
   dlg.DoModal();
}

void CGeometryListDialog::OnSelchangeTypeCb() 
{
   // if nothing selected
   if (!m_geomLC.GetSelectedCount())
      return;

   // get selected geometry
   int count = m_geomLC.GetItemCount();
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_geomLC.GetItemState(selItem, LVIS_SELECTED))
         break;
   }
   BlockStruct *block = (BlockStruct*)m_geomLC.GetItemData(selItem);

   block->setBlockType(m_typeCB.GetCurSel());
   
   // update list
   m_geomLC.SetItem(selItem, 1, LVIF_TEXT, blockTypeToDisplayString(block->getBlockType()), 0, 0, 0, 0);

   if (IDYES == ErrorMessage("Do you want to change the Insert Types of the same Geometry?", "Block Type Change", MB_YESNO))
   {
      ChangeInsertTypes dlg;
      dlg.m_blockName = block->getName();
      dlg.inserttype = default_inserttype(block->getBlockType()); // default insert type for dlg (-1 = none)
      if (dlg.DoModal() == IDOK)
      {
         // function ( block , dlg.inserttype );
         change_inserttypes_by_block(doc, block, dlg.inserttype); 
      }
   }
}

void CGeometryListDialog::OnLoadApertureAssign() 
{
   // TODO: Add your control notification handler code here
   ErrorMessage("Not Implemented.", "Load Aperture Assign");
   
}

void CGeometryListDialog::OnSaveApertureAssign() 
{
   // TODO: Add your control notification handler code here
   ErrorMessage("Not Implemented.", "Save Aperture Assign");
   
}

void CGeometryListDialog::OnLoadToolAssign() 
{
   // TODO: Add your control notification handler code here
   ErrorMessage("Not Implemented.", "Load Tool Assign");
   
}

void CGeometryListDialog::OnSaveToolAssign() 
{
   // TODO: Add your control notification handler code here
   ErrorMessage("Not Implemented.", "Save Tool Assign");
   
}


/////////////////////////////////////////////////////////////////////////////
// ChangeInsertTypes dialog
ChangeInsertTypes::ChangeInsertTypes(CWnd* pParent /*=NULL*/)
   : CDialog(ChangeInsertTypes::IDD, pParent)
{
   //{{AFX_DATA_INIT(ChangeInsertTypes)
   m_blockName = _T("");
   //}}AFX_DATA_INIT
}

void ChangeInsertTypes::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ChangeInsertTypes)
   DDX_Control(pDX, IDC_LIST1, m_list);
   DDX_Text(pDX, IDC_BLOCKNAME, m_blockName);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ChangeInsertTypes, CDialog)
   //{{AFX_MSG_MAP(ChangeInsertTypes)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ChangeInsertTypes message handlers
BOOL ChangeInsertTypes::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      m_list.AddString(insertTypeToDisplayString(insertType));
   }

   m_list.SetCurSel(inserttype);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ChangeInsertTypes::OnOK() 
{
   inserttype = m_list.GetCurSel();
   
   CDialog::OnOK();
}
