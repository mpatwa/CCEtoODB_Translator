// $Header: /CAMCAD/5.0/FileList.cpp 32    6/30/07 2:27a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "StdAfx.h"
#include "GeneralDialog.h"
#include "CCEtoODB.h"
#include "mainfrm.h"
#include "geomlib.h"
#include "response.h"
#include "General.h"
#include "File.h"
#include "DcaEnumIterator.h"
#include "Filelist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ICONCOUNT 2


static int current_file;
static int cnt;
static int selItem;
static CFileListDialog *globalFileListDlg = NULL;

//void FillCurrentSettings(CCEtoODBDoc *doc);
void UpdateDRCFile();

void CCEtoODBDoc::OnFileList() 
{
   CFileListDialog dlg;
   dlg.doc = this;

   if (dlg.DoModal() == IDOK)
   {
		PanelBoardView = 0;

		if (getFileList().GetVisibleCount(blockTypeGeometryEdit) == 1)
		{
			if (getFileList().GetOnlyShown(blockTypePcb))
				PanelBoardView = 2;
			else if (getFileList().GetOnlyShown(blockTypePanel))
				PanelBoardView = 1;
		}

		UpdateAllViews(NULL);
      //FillCurrentSettings(this);
      UpdateDRCFile();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFileListDialog dialog
CFileListDialog::CFileListDialog(CWnd* pParent /*=NULL*/)
   : CResizingDialog(CFileListDialog::IDD, pParent)
{
   globalFileListDlg = this;
   //{{AFX_DATA_INIT(CFileListDialog)
   //}}AFX_DATA_INIT


   addFieldControl(IDC_FILELC,  anchorLeft, growBoth);
   addFieldControl(IDOK,        anchorBottomLeft);
   addFieldControl(ID_APPLY,    anchorBottomLeft);
   addFieldControl(IDC_EDIT,    anchorBottomLeft);
   addFieldControl(IDC_REMOVE,  anchorBottomLeft);
   addFieldControl(IDC_ALL_ON,  anchorBottomLeft);
   addFieldControl(IDC_ALL_OFF, anchorBottomLeft);
   addFieldControl(IDHELP,      anchorBottomLeft);

}

CFileListDialog::~CFileListDialog()
{ 
   if (m_imageList)
      delete m_imageList;

   globalFileListDlg = NULL;
}

void CFileListDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFileListDialog)
   DDX_Control(pDX, IDC_FILELC, m_fileLC);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileListDialog, CResizingDialog)
   //{{AFX_MSG_MAP(CFileListDialog)
   ON_BN_CLICKED(IDHELP, OnHelp)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_BN_CLICKED(ID_APPLY, OnApply)
   ON_BN_CLICKED(IDC_ALL_ON, OnAllOn)
   ON_BN_CLICKED(IDC_ALL_OFF, OnAllOff)
   ON_BN_CLICKED(IDC_REMOVE, OnRemove)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CFileListDialog message handlers
BOOL CFileListDialog::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   decimals = GetDecimals(doc->getSettings().getPageUnits());

   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, ICONCOUNT, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_OFF));
   m_imageList->Add(app->LoadIcon(IDI_ON));
   
   m_fileLC.SetImageList(m_imageList, LVSIL_SMALL);

   // columns
   LV_COLUMN column;
   CRect rect;
   m_fileLC.GetWindowRect(&rect);
   int width = rect.Width() / 9;  // 7 actual columns, 2 are set to double-width, so divide rect to 9 column-width pieces
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

   column.cx = width * 2;   // Double column-width
   column.pszText = "File";
   column.iSubItem = 0;
   m_fileLC.InsertColumn(0, &column);

   column.cx = width * 2;  // Double column-width
   column.pszText = "Type  (CAD Source)";
   column.iSubItem = 1;
   m_fileLC.InsertColumn(1, &column);

   column.cx = width;     // The rest are single column-width
   column.pszText = "X";
   column.iSubItem = 2;
   m_fileLC.InsertColumn(2, &column);

   column.pszText = "Y";
   column.iSubItem = 3;
   m_fileLC.InsertColumn(3, &column);

   column.pszText = "Scale";
   column.iSubItem = 4;
   m_fileLC.InsertColumn(4, &column);

   column.pszText = "Rotation";
   column.iSubItem = 5;
   m_fileLC.InsertColumn(5, &column);

   column.pszText = "Mirror";
   column.iSubItem = 6;
   m_fileLC.InsertColumn(6, &column);

   FillListCtrl();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileListDialog::FillListCtrl() 
{
   FileStruct *p;
   char fl[_MAX_FNAME], ext[_MAX_EXT], buf[_MAX_FNAME + _MAX_EXT + 1];
   LV_ITEM item;
   int actualItem;

   m_fileLC.DeleteAllItems();
   selItem = -1;

   POSITION pos = doc->getFileList().GetHeadPosition();
   int j = 0;
   while (pos != NULL)
   {
      p = doc->getFileList().GetNext(pos);

      if (p->isHidden())
         continue;

      _splitpath(p->getName(), NULL, NULL, fl, ext);
      if (p->getRefName().IsEmpty())
         sprintf(buf, "%s%s", fl, ext);      
      else
         sprintf(buf, "%s%s;%s", fl, ext, p->getRefName());      

      item.mask = LVIF_TEXT | LVIF_IMAGE;
      item.iItem = j++;
      item.iSubItem = 0;
      item.pszText = buf;
      item.iImage = p->isShown();
      actualItem = m_fileLC.InsertItem(&item);
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_fileLC.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_fileLC.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_fileLC.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_fileLC.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_fileLC.InsertItem(&item);
               }
            }
         }
      }

      m_fileLC.SetItemData(actualItem, (LPARAM)p);

      item.mask = LVIF_TEXT;
      item.iItem = actualItem;
      item.iSubItem = 1;
      CString filetype( blockTypeToDisplayString(p->getBlockType()) ); // Need local copy or it displays nonsense in Debug mode because return CString val is temporary (stack) var
      CString sourceCAD( fileTypeTagToString(p->getCadSourceFormat()) );
      if (!sourceCAD.IsEmpty())
         filetype += " (" + sourceCAD + ")";
      item.pszText = filetype.GetBuffer(0);
      m_fileLC.SetItem(&item);

      item.iSubItem = 2;
      sprintf(buf, "%.*lf", decimals, p->getInsertX());
      item.pszText = buf;
      m_fileLC.SetItem(&item);

      item.iSubItem = 3;
      sprintf(buf, "%.*lf", decimals, p->getInsertY());
      item.pszText = buf;
      m_fileLC.SetItem(&item);

      item.iSubItem = 4;
      sprintf(buf, "%.2lf", p->getScale());
      item.pszText = buf;
      m_fileLC.SetItem(&item);

      item.iSubItem = 5;
      sprintf(buf, "%.2lf", RadToDeg(p->getRotation()));
      item.pszText = buf;
      m_fileLC.SetItem(&item);

      item.iSubItem = 6;
      if (p->isMirrored())
         item.pszText = "M";
      else
         item.pszText = "";
      m_fileLC.SetItem(&item);
   }
}

void CFileListDialog::OnEdit() 
{
   if (selItem == -1) return;

   FileStruct *file = (FileStruct *)m_fileLC.GetItemData(selItem);
   EditFilePlacement dlg;
   CString buf;
   dlg.m_file = file->getName();
   dlg.m_show = file->isShown();
   buf.Format("%.*lf", decimals, file->getInsertX()); 
   dlg.m_x = buf;
   buf.Format("%.*lf", decimals, file->getInsertY()); 
   dlg.m_y = buf;
   buf.Format("%.2lf", file->getScale()); 
   dlg.m_scale = buf;
   buf.Format("%.2lf", RadToDeg(file->getRotation())); 
   dlg.m_angle = buf;

   dlg.m_mirror = file->isMirrored();
   dlg.filetype = file->getBlockType();

   // The File List dialog has an OK and Apply button, but they are misleading.
   // The real change to files happens here, and they are permanent. If one closes
   // the File List dialog without OK or Apply, these changes have still already
   // been applied. The OK and Apply buttons really only effect the graphic display,
   // not the data. This was noticed when dealing with case dts0100377159, which is
   // asking to autorun RestructureFileAroundOrigin if mirror is changed.

   if (dlg.DoModal() == IDOK)
   {
      file->setShow(dlg.m_show);
      file->setInsertX((DbUnit)atof(dlg.m_x));
      file->setInsertY((DbUnit)atof(dlg.m_y));
      file->setScale((DbUnit)atof(dlg.m_scale));
      file->setRotation((DbUnit)DegToRad(atof(dlg.m_angle)));
      file->setMirrored(dlg.m_mirror);

      file->setBlockType(intToBlockTypeTag((dlg).filetype));

      FillListCtrl();
   }
}

void CFileListDialog::OnRemove() 
{
   if (selItem == -1) return;

   if (ErrorMessage("Are you sure you want to remove this file", "Warning", MB_YESNO) != IDYES)
      return;

   FileStruct *file = (FileStruct *)m_fileLC.GetItemData(selItem);

   // remove file from filelist
   //POSITION pos = doc->getFileList().Find(file);
   //doc->FreeFile(file);
   //doc->getFileList().RemoveAt(pos);  
   doc->getFileList().deleteFile(file); 

   // refresh FileList Dlg
   FillListCtrl();
}

void CFileListDialog::OnAllOn() 
{
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
      doc->getFileList().GetNext(pos)->setShow(1);
   
   FillListCtrl();
}

void CFileListDialog::OnAllOff() 
{
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
      doc->getFileList().GetNext(pos)->setShow(0);
   
   FillListCtrl();
}

void CFileListDialog::RestructureFiles()
{
   // Case dts0100377159
   // Restructure any PCB or Panel files that have non-zero xy offset, rotation, scale, or are mirrored.

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      if (file != NULL && (file->getBlockType() == blockTypePcb || file->getBlockType() == blockTypePanel))
      {
         if (file->getInsertX() != 0.0 ||
            file->getInsertY() != 0.0 ||
            file->getScale() != 1.0 ||
            file->getRotation() != 0.0 ||
            file->isMirrored())
         {
            bool normalizeRefNameRotation = false;
            if(blockTypePcb == file->getBlockType() && doc->getSettings().getNormalizeRefNameRotations())
               normalizeRefNameRotation = true;
            RestructureFile(doc, file, normalizeRefNameRotation);
         }
      }
   }
   
   FillListCtrl();   
}

void CFileListDialog::OnApply() 
{
   RestructureFiles();
   doc->UpdateAllViews(NULL); 
   doc->UnselectAll(FALSE);   
   doc->OnFitPageToImage();
}

void CFileListDialog::OnOK() 
{
   RestructureFiles();
   doc->UnselectAll(FALSE);
   doc->OnFitPageToImage();
   CDialog::OnOK();
}


void CFileListDialog::OnCancel() 
{
   // The settings have been applied already in the Edit sub-dialog, the OK or Apply in this dialog
   // have nothing to do with that, they only cause a redraw. So if cancelling then we still need
   // to do the Restructure to keep all in sync with the new convention.
   RestructureFiles();
   doc->UnselectAll(FALSE);
   doc->OnFitPageToImage();
   CResizingDialog::OnCancel();
}

void CFileListDialog::OnHelp() 
{ 
   //getApp().OnHelpIndex();
}

/////////////////////////////////////////////////////////////////////////////
// FileListCtrl
FileListCtrl::FileListCtrl()
{
}

FileListCtrl::~FileListCtrl()
{
}


BEGIN_MESSAGE_MAP(FileListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(FileListCtrl)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void FileListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
   LV_HITTESTINFO info;
   info.pt.x = point.x;
   info.pt.y = point.y;

   HitTest(&info);

   if (info.flags & LVHT_ONITEM)
   {
//    if (selItem == info.iItem)
      {
         if (info.flags & LVHT_ONITEMICON)
         {
            FileStruct *file = (FileStruct *)GetItemData(info.iItem);
            file->setShow(!file->isShown());
            SetItem(info.iItem, 0, LVIF_IMAGE, NULL, file->isShown(), 0, 0, 0);
         }
      }

      selItem = info.iItem;
   }
   else selItem = -1;
   
   CListCtrl::OnLButtonDown(nFlags, point);
}

void FileListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   LV_HITTESTINFO info;
   info.pt.x = point.x;
   info.pt.y = point.y;

   HitTest(&info);

   if (info.flags & LVHT_ONITEM)
   {
      selItem = info.iItem;

      if (globalFileListDlg != NULL && info.flags & LVHT_ONITEMLABEL)
         globalFileListDlg->OnEdit();
   }
   else selItem = -1;
   
   //CListCtrl::OnLButtonDblClk(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
// EditFilePlacement dialog
EditFilePlacement::EditFilePlacement(CWnd* pParent /*=NULL*/)
   : CDialog(EditFilePlacement::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditFilePlacement)
   m_mirror = FALSE;
   m_angle = _T("");
   m_scale = _T("");
   m_show = FALSE;
   m_x = _T("");
   m_y = _T("");
   m_file = _T("");
   //}}AFX_DATA_INIT
}

void EditFilePlacement::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditFilePlacement)
   DDX_Control(pDX, IDC_FILETYPE_CB, m_filetypeCB);
   DDX_Check(pDX, IDC_MIRROR, m_mirror);
   DDX_Text(pDX, IDC_ROTATION, m_angle);
   DDX_Text(pDX, IDC_SCALE, m_scale);
   DDX_Check(pDX, IDC_SHOW, m_show);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_FILE, m_file);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditFilePlacement, CDialog)
   //{{AFX_MSG_MAP(EditFilePlacement)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditFilePlacement::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (EnumIterator(BlockTypeTag,blockTypeTagIterator);blockTypeTagIterator.hasNext();)
   {
      BlockTypeTag blockType = blockTypeTagIterator.getNext();

      if (blockType != blockTypePadstackGraphic)
      {
         int index = m_filetypeCB.AddString(blockTypeToDisplayString(blockType));
         m_filetypeCB.SetItemData(index,blockType);

         if (blockType == filetype)
            m_filetypeCB.SetCurSel(index);
      }
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditFilePlacement::OnOK() 
{
   filetype = m_filetypeCB.GetItemData(m_filetypeCB.GetCurSel());
   
   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// SelectFileDlg dialog
SelectFileDlg::SelectFileDlg(CWnd* pParent /*=NULL*/)
   : CDialog(SelectFileDlg::IDD, pParent)
{
	// Initialize to unknown to indicate all type of file and source cad
	m_eFileType = blockTypeUnknown;
	m_iSourceCad = Type_Unknown;
}

void SelectFileDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectFileDlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectFileDlg, CDialog)
   //{{AFX_MSG_MAP(SelectFileDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectFileDlg message handlers
BOOL SelectFileDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
		if (m_eFileType != BLOCKTYPE_UNKNOWN && file->getBlockType() != m_eFileType)
			continue;
		if (m_iSourceCad != Type_Unknown && file->getCadSourceFormat() != m_iSourceCad)
			continue;

      if (file->isShown())
         m_list.SetItemDataPtr(m_list.AddString(file->getName()), file);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectFileDlg::OnOK() 
{
   int sel = m_list.GetCurSel();

   if (sel == CB_ERR)
      return;

   file = (FileStruct*)m_list.GetItemDataPtr(sel);
   
   CDialog::OnOK();
}

/******************************************************************************
* CCEtoODBDoc::OnPanelView
*/
void CCEtoODBDoc::OnPanelView() 
{
	int count = 0;
	CVoidPtrSelItemList panelList;	// List of pointer to panel file

   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);

      if (file->getBlockType() == blockTypePanel)
		{
			CVoidPtrSelItem* fileItem = new CVoidPtrSelItem(file->getName(), (void*)file, false);			
			panelList.AddTail(fileItem);
			count ++;
		}
	}

   if (count < 1) // no panel files
   {
      ErrorMessage("No Panel Files");
      return;
   }

   FileStruct *panelFile;
   if (count > 1) // multiple panel files
   {
		CSelectDlg dlg("Select Panel File", false, true);
		POSITION pos = panelList.GetHeadPosition();
		while (pos != NULL)
		{
		  	CVoidPtrSelItem* fileItem = panelList.GetNext(pos);
			dlg.AddItem(fileItem);
		}
      if (dlg.DoModal() != IDOK)
		{
			panelList.RemoveAll();
			return;
		}

		panelFile = (FileStruct*)((CVoidPtrSelItem*)dlg.GetSelItemHead())->GetVoidPtrItem();
		panelList.RemoveAll();
   }
   else
      panelFile = (FileStruct*)panelList.GetHead()->GetVoidPtrItem();

   UnselectAll(FALSE);

   // turn off all files except selected panel file
   pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      file->setShow(file == panelFile);
   }

   //FillCurrentSettings(this);

   OnFitPageToImage();  
   PanelBoardView = 1;

	CMainFrame *m_pMainFrm = (CMainFrame*)AfxGetMainWnd();
	m_pMainFrm->getNavigator().setDoc(this);
   UpdateDRCFile();

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   pApp->getSchematicLinkController().stopSchematicLink();

   SendResponse("View", "Panel");
}

/******************************************************************************
* CCEtoODBDoc::OnBoardView
*/
void CCEtoODBDoc::OnBoardView() 
{
   BOOL turnOffSchLink = FALSE;
	int count = 0;
	CVoidPtrSelItemList boardList;	// List of pointer to panel file

   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);

      if (file->getBlockType() == blockTypePcb)
		{
			CVoidPtrSelItem* fileItem = new CVoidPtrSelItem(file->getName(), (void*)file, false);			
			boardList.AddTail(fileItem);
			count ++;
		}
	}

   if (count < 1) // no board files
   {
      ErrorMessage("No Board Files");
      return;
   }

   FileStruct *boardFile;

   if (count > 1) // multiple panel files
   {
		FileStruct* curShownFile = getFileList().GetFirstShown(blockTypePcb);

		CSelectDlg dlg("Select Board File", false, true);
		POSITION pos = boardList.GetHeadPosition();
		while (pos != NULL)
		{
		  	CVoidPtrSelItem* fileItem = boardList.GetNext(pos);
			dlg.AddItem(fileItem);
		}

      if (dlg.DoModal() != IDOK)
		{
			boardList.RemoveAll();
			return;
		}

		boardFile = (FileStruct*)((CVoidPtrSelItem*)dlg.GetSelItemHead())->GetVoidPtrItem();
		boardList.RemoveAll();
		turnOffSchLink = (curShownFile == boardFile);
   }
   else
      boardFile = (FileStruct*)boardList.GetHead()->GetVoidPtrItem();


   UnselectAll(FALSE);

   // turn off all files except selected panel file
   pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      file->setShow(file == boardFile);
   }

   //FillCurrentSettings(this);

   OnFitPageToImage();  
   PanelBoardView = 2;

	CMainFrame *m_pMainFrm = (CMainFrame*)AfxGetMainWnd();
	m_pMainFrm->getNavigator().setDoc(this);
   UpdateDRCFile();

   if (turnOffSchLink)
   {
      CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
      pApp->getSchematicLinkController().stopSchematicLink();
   }

   SendResponse("View", "Board");
}
