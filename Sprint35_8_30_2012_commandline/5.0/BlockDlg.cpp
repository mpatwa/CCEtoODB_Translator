
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/           

#include "StdAfx.h"
#include "Graph.h"
#include "BlockDlg.h"
#include "CCEtoODB.h"
#include "MainFrm.h"

#include ".\blockdlg.h"

extern CStatusBar *StatusBar;          // from MAINFRM.CPP

void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);
//void FillCurrentSettings(CCEtoODBDoc *doc);

static SelectGeometry *selectGeometryDlg = NULL;

/*******************************************************************************
* EditSelectedBlock
*/
void CCEtoODBDoc::OnEditSelectedBlock() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   if (nothingIsSelected())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected!");
      return;
   }

   SelectStruct *s = getSelectStack().getAtLevel(); 

   // if stepped into a block down to entities, edit block directly above that
   //if (s->getData()->getDataType() != T_INSERT && getSelectStackLevelIndex() > 0)
   //   s = getSelectionAt(getSelectStackLevelIndex() - 1);
   if (s->getData()->getDataType() != T_INSERT && getSelectStack().isLevelParentValid())
   {
      s = getSelectStack().getAtLevelParent();
   }

   // if top level non-insert
   if (s->getData()->getDataType() != T_INSERT)
   {
      if (StatusBar)
         StatusBar->SetWindowText("Selected entity is not a geometry.");
      return;
   }

   s->getData()->setSelected(false);

   BlockStruct *block = Find_Block_by_Num(s->getData()->getInsert()->getBlockNumber());

   if (!selectGeometryDlg)
   {
      CWaitCursor wait;

      selectGeometryDlg = new SelectGeometry;
      selectGeometryDlg->doc = this;
      selectGeometryDlg->Create(IDD_GEOMETRY_SELECT, NULL);
      selectGeometryDlg->ShowWindow(SW_SHOW);
   }

   //for (int i=0; i <= getSelectStackLevelIndex(); i++)
   for (int levelIndex=0; levelIndex <= getSelectStack().getLevelIndex(); levelIndex++)
   {
      SelectStruct* selectStruct = getSelectStack().getAt(levelIndex);

      if (selectStruct->getData()->getDataType() == dataTypeInsert)
      {
         BlockStruct *block = Find_Block_by_Num(selectStruct->getData()->getInsert()->getBlockNumber());

         if ((block->getFlags() & BL_APERTURE) || 
             (block->getFlags() & BL_BLOCK_APERTURE) && block->getShape() == T_COMPLEX)
         {
            block = Find_Block_by_Num((int)(block->getSizeA()));
         }

         selectGeometryDlg->SelectBlock(block);
      }
   }

   EditGeometry(this, block);
}

/*******************************************************************************
* OnSelectAndEditBlock
*
*  - Open SelectGeometry Dialog to pick block to edit
*/
void CCEtoODBDoc::OnSelectAndEditBlock() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }

   if (selectGeometryDlg)
      return;

   CWaitCursor wait;

   selectGeometryDlg = new SelectGeometry;
   selectGeometryDlg->doc = this;
   selectGeometryDlg->Create(IDD_GEOMETRY_SELECT, NULL);
   selectGeometryDlg->ShowWindow(SW_SHOW);*/
}

/******************************************************************************
* EditGeometryAndShowDlg
*/
void EditGeometryAndShowDlg(CCEtoODBDoc *doc, BlockStruct *block)
{
   if (!selectGeometryDlg)
   {
      CWaitCursor wait;

      selectGeometryDlg = new SelectGeometry;
      selectGeometryDlg->doc = doc;
      selectGeometryDlg->Create(IDD_GEOMETRY_SELECT, NULL);
      selectGeometryDlg->ShowWindow(SW_SHOW);
   }

   selectGeometryDlg->SelectBlock(block);

   EditGeometry(doc, block);
}

/*******************************************************************************
* EditGeometry
*
* DESCRIPTION:
*  - Modify Database so editing geometry
*     - hides all files
*     - makes a new file which is an insert of the block to be edited
*     - (removes an old block-edit file if there was one)
*/
void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block)
{
   if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE))
   {
      if (block->getShape() == T_COMPLEX)
         block = doc->Find_Block_by_Num((int)(block->getSizeA()));
      else
      {
         ErrorMessage("Can not edit a standard aperture.", "Edit Geometry");
         return;
      }
   }

   if (doc->docTitle.IsEmpty())
      doc->docTitle = doc->GetTitle();

   CString title = "Editing Geometry \"";
   title += block->getName();
   title += "\"";
   doc->SetTitle(title);

   doc->GeometryEditing = TRUE;

   doc->ClearUndo();

   doc->UnselectAll(FALSE);

   doc->getFileList().popFile();

   // hide all other files
   for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = doc->getFileList().GetNext(filePos);

      file->setHidden(true);
   }

   // Disable Navigator during geometry edit. It is actually already effectively
   // disabled because any PCB file was hidden above, but it does not yet look disabled.
  	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(NULL);

   // make a new file (not hidden) to edit this block
   FileStruct* file = &(doc->getCamCadData().getFileList().pushNewFile(block->getName(),fileTypeUnknown,NULL));

   // Can't identify temp geom edit file by filenum anymore (used to use -1) because DCA does not allow
   // changing filenum. So we added a specific flag for temp edit use. Set it.
   file->setTempGeomEdit(true);

   // And the rest of the relevant stuff...
   file->setInsertX(0.);
   file->setInsertY(0.);
   file->setRotation((DbUnit)0.0);
   file->setScale((DbUnit)1.0);
   file->setMirrored(false);
   file->setShow(true);
   file->setHidden(false);
   file->setBlock(block);
   file->setOriginalBlock(block);
   file->setNotPlacedYet(false);

   // before changing the view, save the previous zoom level and location
   POSITION pos;
   CCEtoODBView *view;
   pos = doc->GetFirstViewPosition();
   if (pos != NULL)
   {
      view = (CCEtoODBView *)doc->GetNextView(pos);
      if (view->lastScaleNum <= 0)
      {
         // Save the scale factors
         view->lastScaleDenom = view->ScaleDenom;
         view->lastScaleNum = view->ScaleNum;

         // Save the page size
         view->lastPageSize.xMin = doc->getSettings().getXmin();
         view->lastPageSize.xMax = doc->getSettings().getXmax();
         view->lastPageSize.yMin = doc->getSettings().getYmin();
         view->lastPageSize.yMax = doc->getSettings().getYmax();

         // Save the scroll bar positions
         view->lastLocation.SetPoint(view->GetXPos(), view->GetYPos());
      }
   }

   //FillCurrentSettings(doc);

   doc->OnFitPageToImage();
}

/*******************************************************************************
* OnDoneEditing
*
*  - Modify Database so back to normal
*/
void CCEtoODBDoc::OnDoneEditing() 
{                        
   CWaitCursor wait;

   if (selectGeometryDlg)
   {
      selectGeometryDlg->DestroyWindow();
      delete selectGeometryDlg;
      selectGeometryDlg = NULL;
   }

   if (!GeometryEditing)
      return;

   FlushQueue();

   UnselectAll(FALSE);
   ClearUndo();

   GeometryEditing = FALSE;

   //// get rid of old geometry edit file (if exists)
   //// hide all other files
   //POSITION filePos = getFileList().GetHeadPosition();
   //while (filePos != NULL)
   //{
   //   POSITION tempPos = filePos;
   //   FileStruct *file = getFileList().GetNext(filePos);
   //   if (file->getBlockType() == blockTypeGeometryEdit)
   //   {
   //      getFileList().RemoveAt(tempPos);
   //      delete file;
   //   }
   //   else
   //   {
   //      file->setHidden(false);
   //   }
   //}

   // pop off old geometry edit file
   getCamCadData().getFileList().popFile();

   // unhide all other files
   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = getFileList().GetNext(filePos);

      file->setHidden(false);
   }

   //OnFitPageToImage();

   //FillCurrentSettings(this);

   // set the view back to the saved zoom level and location
   POSITION pos;
   CCEtoODBView *view;
   pos = GetFirstViewPosition();
   if (pos != NULL)
   {
      view = (CCEtoODBView *)GetNextView(pos);
      
      // Set the scale factors
      view->ScaleDenom = view->lastScaleDenom;
      view->ScaleNum = view->lastScaleNum;
      
      // Set the page size
      getSettings().setXmin(view->lastPageSize.xMin);
      getSettings().setXmax(view->lastPageSize.xMax);
      getSettings().setYmin(view->lastPageSize.yMin);
      getSettings().setYmax(view->lastPageSize.yMax);
      SetMaxCoords();

      // Set up the scroll bars
      view->SetXPos(view->lastLocation.x);
      view->SetYPos(view->lastLocation.y);

      // Reset the position
      view->lastScaleNum = -1;
   }

   UpdateAllViews(NULL);

   SetTitle(docTitle);

   // Wake up the Navigator (update enabled status)
   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(this);
}

/////////////////////////////////////////////////////////////////////////////
// SelectGeometry dialog
SelectGeometry::SelectGeometry(CWnd* pParent /*=NULL*/)
   : CResizingDialog(SelectGeometry::IDD, pParent)
{
   //{{AFX_DATA_INIT(SelectGeometry)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   currentItem = NULL;

   addFieldControl(IDC_TREE,anchorLeft,growBoth);
   addFieldControl(IDC_EDIT,anchorBottom);
   addFieldControl(IDCANCEL,anchorBottom);
   addFieldControl(IDC_DuplicateGeometry,anchorBottom);
   addFieldControl(IDC_RenameGeometry,anchorBottom);
}

SelectGeometry::~SelectGeometry()
{
   if (m_imageList)
      delete m_imageList;
}

void SelectGeometry::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectGeometry)
   DDX_Control(pDX, IDC_TREE, m_tree);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectGeometry, CResizingDialog)
   //{{AFX_MSG_MAP(SelectGeometry)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_DuplicateGeometry, OnBnClickedDuplicateGeometry)
   ON_BN_CLICKED(IDC_RenameGeometry, OnBnClickedRenameGeometry)
END_MESSAGE_MAP()

#define iFolder      0
#define iOpenFolder  1
#define iRound       2
#define iBlock       3
#define iFile        4  
#define iCompBlk     5
#define iPadStk      6
#define iPad         7
#define iCompLocal   8
#define iTool        9

/////////////////////////////////////////////////////////////////////////////
// SelectGeometry message handlers
BOOL SelectGeometry::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 10, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_ROUND));
   m_imageList->Add(app->LoadIcon(IDI_BLOCK));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_COMPBLK));
   m_imageList->Add(app->LoadIcon(IDI_PADSTK));
   m_imageList->Add(app->LoadIcon(IDI_PAD));
   m_imageList->Add(app->LoadIcon(IDI_COMPLOCAL));
   m_imageList->Add(app->LoadIcon(IDI_DRILL));

   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

#if !defined(_RDEBUG)
   GetDlgItem(IDC_DuplicateGeometry)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_RenameGeometry)->ShowWindow(SW_HIDE);
#endif

   FillTree();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectGeometry::AddItem(BlockStruct *block, HTREEITEM parent) 
{
   if ((block->getFlags() & BL_APERTURE) 
      || (block->getFlags() & BL_TOOL) 
      || (block->getFlags() & BL_WIDTH)
      || (block->getFlags() & BL_COMPLEX_TOOL))
      return;

   int icon = iBlock;
   if (block->getFlags() & BL_FILE)
      icon = iFile;
   else if (block->getFlags() & BL_BLOCK_TOOL)
      icon = iTool;
   else if (block->getFlags() & BL_BLOCK_APERTURE)
      icon = iRound;
   else
   {
      switch (block->getBlockType())
      {
      case BLOCKTYPE_PCBCOMPONENT:
         icon = iCompBlk;
      break;
      case BLOCKTYPE_PADSTACK:
         icon = iPadStk;
      break;
      case BLOCKTYPE_PADSHAPE:
         icon = iPad;
      break;
      case BLOCKTYPE_LOCALPCBCOMP:
         icon = iCompLocal;
      break;
      case BLOCKTYPE_TOOLING:
         icon = iTool;
      break;
      }
   }

   CString name = block->getName();
   if (block->getFileNumber() != -1 && !(block->getFlags() & BL_FILE))
   {
      FileStruct *file = doc->Find_File(block->getFileNumber());
      if (file)
      {
         name += " [";
         name += file->getName();
         name += "]";
      }
   }

   HTREEITEM item = m_tree.InsertItem(name, icon, icon, parent, TVI_SORT);
   m_tree.SetItemData(item, (DWORD)block);
}

void SelectGeometry::FillTree() 
{
   m_tree.DeleteAllItems();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
   
      AddItem(block, TVI_ROOT);
   }
}

void SelectGeometry::OnEdit() 
{
   HTREEITEM item;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   DWORD dword = m_tree.GetItemData(item); 
   if (!dword)
      return;

   currentItem = item;
   ExpandItem(item);

   EditGeometry(doc, (BlockStruct*)dword);
}

CString getDuplicateBlockName(CCEtoODBDoc& camCadDoc,BlockStruct& block)
{
   CString duplicateBlockName = "Duplicate of " + block.getName();

   for (int cardinal = 2;Graph_Block_Exists(&camCadDoc,duplicateBlockName,block.getFileNumber());cardinal++)
   {
      duplicateBlockName.Format("Duplicate %d of %s",cardinal,block.getName());
   }

   return duplicateBlockName;
}

BlockStruct* duplicateBlock(CCEtoODBDoc& camCadDoc,BlockStruct& block)
{
   CString duplicateBlockName = getDuplicateBlockName(camCadDoc,block);
   BlockStruct* duplicatedBlock = camCadDoc.Add_Blockname(duplicateBlockName,block.getFileNumber(),0,0);
   duplicatedBlock->setProperties(block);
   duplicatedBlock->setName(duplicateBlockName);

   return duplicatedBlock;
}

void SelectGeometry::OnBnClickedDuplicateGeometry()
{
   HTREEITEM item;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   DWORD dword = m_tree.GetItemData(item);

   if (dword != NULL && doc != NULL)
   {
      BlockStruct* block = (BlockStruct*)dword;
      BlockStruct* duplicatedBlock = duplicateBlock(*doc,*block);

      AddItem(duplicatedBlock, TVI_ROOT);
   }
}

void SelectGeometry::OnBnClickedRenameGeometry()
{
   HTREEITEM item;
   
   if ((item = m_tree.GetSelectedItem()) == NULL) return; // nothing selected

   DWORD dword = m_tree.GetItemData(item);

   if (dword != NULL)
   {
      BlockStruct* block = (BlockStruct*)dword;
      CRenameGeometryDialog renameGeometryDialog(block->getName());

      if (renameGeometryDialog.DoModal() == IDOK)
      {
         block->setName(renameGeometryDialog.getNewGeometryName());

         m_tree.DeleteItem(item);

         AddItem(block, TVI_ROOT);
      }
   }
}

void SelectGeometry::OnCancel() 
{
   doc->OnDoneEditing();
}

void SelectGeometry::SelectBlock(BlockStruct *block)
{
   if (currentItem)
   {
      HTREEITEM item = m_tree.GetChildItem(currentItem);
      while (item)
      {
         BlockStruct *temp = (BlockStruct*)m_tree.GetItemData(item); 

         if (temp == block)
         {
            m_tree.Select(item, TVGN_CARET);
            ExpandItem(item);
            currentItem = item;
            return;
         }
            
         item = m_tree.GetNextItem(item, TVGN_NEXT);
      }
   }

   HTREEITEM item = m_tree.GetRootItem();
   while (item)
   {
      BlockStruct *temp = (BlockStruct*)m_tree.GetItemData(item); 

      if (temp == block)
      {
         m_tree.Select(item, TVGN_CARET);
         currentItem = item;
         ExpandItem(item);
         return;
      }
         
      item = m_tree.GetNextItem(item, TVGN_NEXT);
   }
}

void SelectGeometry::ExpandItem(HTREEITEM item)
{
   BlockStruct *block = (BlockStruct*)m_tree.GetItemData(item);

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *subblock = doc->getBlockAt(data->getInsert()->getBlockNumber());
		if (!subblock)
			continue;	// If no child block found, continue (TLS -case# 1019)
		
      if ((subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) && subblock->getShape() == T_COMPLEX)
         subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));
		

      // look to see if we've added it
      BOOL Found = FALSE;
      HTREEITEM child = m_tree.GetChildItem(item);
      while (child != NULL)
      {
         BlockStruct *temp = (BlockStruct*)m_tree.GetItemData(child);
         if (temp == subblock)
         {
            Found = TRUE;
            break;
         }

         child = m_tree.GetNextSiblingItem(child);
      }

      if (!Found)
         AddItem(subblock, item);
   }  

   m_tree.Expand(item, TVE_EXPAND);
}

void SelectGeometry::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnEdit();
   
   *pResult = 0;
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CRenameGeometryDialog, CDialog)

CRenameGeometryDialog::CRenameGeometryDialog(const CString& oldGeometryName)
: CDialog(CRenameGeometryDialog::IDD, NULL)
, m_oldGeometryName(oldGeometryName)
, m_newGeometryName(_T(""))
{
}

CRenameGeometryDialog::~CRenameGeometryDialog()
{
}

void CRenameGeometryDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_NewGeometryName, m_newGeometryName);
   DDX_Control(pDX, IDC_GeometryName, m_oldGeometryNameStatic);
}

BEGIN_MESSAGE_MAP(CRenameGeometryDialog, CDialog)
END_MESSAGE_MAP()


// CRenameGeometryDialog message handlers

BOOL CRenameGeometryDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_oldGeometryNameStatic.SetWindowText(m_oldGeometryName);
   m_newGeometryName = m_oldGeometryName;

   UpdateData(false);

   return TRUE;  
}
