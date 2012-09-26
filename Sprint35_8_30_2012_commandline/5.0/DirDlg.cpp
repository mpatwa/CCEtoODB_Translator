// $Header: /CAMCAD/4.5/DirDlg.cpp 18    1/01/07 1:32p Lynn Phung $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           
                       
// DIRDLG.CPP

#include "StdAfx.h"
#include "General.h"
#include "DirDlg.h"
#include "shlobj.h"
#include "RwLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*** CDirDlg ********************************/

/////////////////////////////////////////////////////////////////////////////
// CDirDlg dialog
CDirDlg::CDirDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CDirDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CDirDlg)
   m_title = _T("");
   m_dir = _T("");
   //}}AFX_DATA_INIT
}

void CDirDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDirDlg)
   DDX_Text(pDX, IDC_TITLE, m_title);
   DDX_Text(pDX, IDC_DIR, m_dir);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDirDlg, CDialog)
   //{{AFX_MSG_MAP(CDirDlg)
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirDlg message handlers
void CDirDlg::OnBrowse() 
{
   UpdateData();

   CBrowse dlg;
   dlg.m_strTitle = m_title;
   dlg.m_strSelDir = m_dir;
   if (dlg.DoBrowse())
   {
      m_dir = dlg.m_strPath;
      UpdateData(FALSE);
   }  
}

/*** CBrowse ********************************/

// Callback function called by SHBrowseForFolder's browse control
// after initialization and when selection changes
static int __stdcall BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  CBrowse* pDirDialogObj = (CBrowse*)lpData;

   if (uMsg == BFFM_INITIALIZED && !pDirDialogObj->m_strSelDir.IsEmpty())
      ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)(pDirDialogObj->m_strSelDir));

   if (uMsg == BFFM_SELCHANGED)
   {
      LPITEMIDLIST pidl = (LPITEMIDLIST)lParam;
      CString path;
      if (::SHGetPathFromIDList(pidl, path.GetBuffer(MAX_PATH)))
         ::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)(LPCTSTR)(path));
   }

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBrowse::CBrowse()
{
   m_strTitle = "";
}

int CBrowse::DoBrowse()
{
   LPMALLOC pMalloc;
   if (SHGetMalloc (&pMalloc)!= NOERROR)
      return 0;

   BROWSEINFO bInfo;
   LPITEMIDLIST pidl;
   ZeroMemory((PVOID) &bInfo,sizeof (BROWSEINFO));

   // set up InitDir as Root
   if (!m_strInitDir.IsEmpty())
   {
      OLECHAR       olePath[MAX_PATH];
      ULONG         chEaten;
      ULONG         dwAttributes;
      HRESULT       hr;
      LPSHELLFOLDER pDesktopFolder;
      // 
      // Get a pointer to the Desktop's IShellFolder interface. 
      //
      if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
      {
         //
         // IShellFolder::ParseDisplayName requires the file name be in Unicode.
         //
         MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_strInitDir.GetBuffer(MAX_PATH), -1,
               olePath, MAX_PATH);

         m_strInitDir.ReleaseBuffer (-1);
         //
         // Convert the path to an ITEMIDLIST.
         //
         hr = pDesktopFolder->ParseDisplayName(NULL,
               NULL,
               olePath,
               &chEaten,
               &pidl,
               &dwAttributes);
         if (FAILED(hr))
         {
            pMalloc ->Free (pidl);
            pMalloc ->Release ();
            return 0;
         }
         bInfo.pidlRoot = pidl;
      }
   }
   bInfo.hwndOwner = AfxGetMainWnd()->m_hWnd;
   bInfo.pszDisplayName = m_strPath.GetBuffer(MAX_PATH);
   bInfo.lpszTitle = m_strTitle;
   bInfo.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
   bInfo.lpfn = BrowseCtrlCallback;  // address of callback function
   bInfo.lParam = (LPARAM)this;      // pass address of object to callback function

   if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
      return 0;

   m_strPath.ReleaseBuffer();
   m_iImageIndex = bInfo.iImage;

   if (::SHGetPathFromIDList(pidl,m_strPath.GetBuffer(MAX_PATH)) == FALSE)
   {
      pMalloc->Free(pidl);
      pMalloc->Release();
      return 0;
   }

   m_strPath.ReleaseBuffer();

   pMalloc->Free(pidl);
   pMalloc->Release();

   return 1;
}

/////////////////////////////////////////////////////////////////////////////
// ODBDirDlg dialog
ODBDirDlg::ODBDirDlg(CWnd* pParent /*=NULL*/)
   : CResizingDialog(ODBDirDlg::IDD, pParent)
{
   m_archiveFileOpened = false;
   m_enableArchiveOpen = false;

   //{{AFX_DATA_INIT(ODBDirDlg)
   m_dir = _T("");
   m_filter = _T("");
   //}}AFX_DATA_INIT

   addFieldControl(IDOK           ,anchorBottom);
   addFieldControl(IDCANCEL       ,anchorBottom);
   addFieldControl(IDC_OpenArchive,anchorBottom);
   addFieldControl(IDC_UNZIP      ,anchorBottom);
   addFieldControl(IDC_STATIC2    ,anchorBottom);

   addFieldControl(IDC_TITLE      ,anchorLeft);
   addFieldControl(IDC_DIR        ,anchorLeft,growHorizontal);
   addFieldControl(IDC_BROWSE     ,anchorRight);
   addFieldControl(IDC_FILTER     ,anchorLeft);
   addFieldControl(IDC_FILES      ,anchorRight,growBoth);
}

void ODBDirDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ODBDirDlg)
   DDX_Control(pDX, IDC_FILES, m_filesLB);
   DDX_Text(pDX, IDC_DIR, m_dir);
   DDX_Text(pDX, IDC_FILTER, m_filter);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ODBDirDlg, CResizingDialog)
   //{{AFX_MSG_MAP(ODBDirDlg)
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   ON_BN_CLICKED(IDC_UNZIP, OnUnzip)
   ON_EN_CHANGE(IDC_DIR, OnChangeDir)
   ON_EN_CHANGE(IDC_FILTER, OnChangeFilter)
   ON_LBN_DBLCLK(IDC_FILES, OnDblclkFiles)
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_OpenArchive, OnBnClickedOpenArchive)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ODBDirDlg message handlers
BOOL ODBDirDlg::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();
   
   FillFileList();

   HWND hWnd = ::GetDlgItem(*this,IDC_OpenArchive);
   ::EnableWindow(hWnd,m_enableArchiveOpen);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ODBDirDlg::OnChangeDir() 
{
   FillFileList();
}

void ODBDirDlg::OnChangeFilter() 
{
   FillFileList();
}

void ODBDirDlg::FillFileList() 
{
   UpdateData();

   char oldPath[MAX_PATH];
   ::GetCurrentDirectory(MAX_PATH, oldPath);

   m_filesLB.ResetContent();

   if (m_dir.IsEmpty())
   {
      m_filesLB.Dir(0x4000, "*.");
      return;
   }

   if (!::SetCurrentDirectory(m_dir))
      return;

   m_filesLB.Dir(0x8010, "*.*");

   if (m_filter.IsEmpty())
      m_filesLB.Dir(0, "*.*");
   else
   {
      char temp[MAX_PATH];
      strcpy(temp, m_filter);
      char *tok = strtok(temp, ",;");
      while (tok != NULL)
      {
         m_filesLB.Dir(0, tok);
         tok = strtok(NULL, ",;");
      }
   }

   ::SetCurrentDirectory(oldPath);
}

static char *make_unix_name(const char *dir)
{
   static char temp[MAX_PATH];
   int   i;

   strcpy(temp, dir);
   for (i=0;i<(int)strlen(temp);i++)
      if (temp[i] == '\\') temp[i] = '/';

   return &temp[0];
}

void ODBDirDlg::OnBrowse() 
{
   UpdateData();

   CBrowse dlg;
   dlg.m_strTitle = "Select Directory";
   dlg.m_strSelDir = m_dir;
   if (dlg.DoBrowse())
   {
      m_dir = dlg.m_strPath;
      UpdateData(FALSE);
      OnChangeDir();
   }  
}

void ODBDirDlg::OnUnzip() 
{
   CString fullPath, filename;

   int sel = m_filesLB.GetCurSel();
   if (sel == LB_ERR)
      return;

   m_filesLB.GetText(sel, filename);

   if (filename.Left(1) == "[")
   {
      filename.Remove('[');
      filename.Remove(']');

      if (filename.Left(1) == "-")
      {
         filename.Remove('-');
         m_dir = filename;
         m_dir += ":\\";
         UpdateData(FALSE);
         FillFileList();
         return;
      }

      fullPath = m_dir;
      if (fullPath.Right(1) != "\\")
         fullPath += "\\";
      fullPath += filename;

      char oldPath[MAX_PATH];
      ::GetCurrentDirectory(MAX_PATH, oldPath);
      if (!::SetCurrentDirectory(fullPath))
         return;
      char newPath[MAX_PATH];
      ::GetCurrentDirectory(MAX_PATH, newPath);
      m_dir = newPath;
      ::SetCurrentDirectory(oldPath);

      UpdateData(FALSE);
      FillFileList();
      return;
   }

   fullPath = m_dir;
   if (fullPath.Right(1) != "\\")
      fullPath += "\\";
   fullPath += filename;

   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   _splitpath( fullPath, drive, dir, fname, ext );
// path in quotes to allow blank names etc...   
   CString  commandLine;
   if (!STRCMPI(ext,".zip") || !STRCMPI(ext, ".gz"))
   {
		// By default gunzip does not recognize .zip files as valid input.
		// The -S param tells gunzip to recognize the given extension.
		// The default extension .gz remains recognized as well.

      commandLine.Format("gunzip -f -S .zip \"%s\" ", fullPath);
		DWORD exitCode;
      int stat = ExecAndWait(commandLine, SW_HIDE, &exitCode);
		if (stat != 0 || exitCode != 0)
      {
         CString  tmp;
         tmp.Format("There was an error using gunzip.exe.\nUse Windows Explorer to unzip this archive.\n");
         ErrorMessage(tmp, "Gunzip Error");
         return;
      }
   }
   else
   if (!STRCMPI(ext,".tar"))
   {
      commandLine.Format("tar -x -f \"%s\" -C \"%s%s\"", fullPath, drive, make_unix_name(dir));
      if (ExecAndWait(commandLine, SW_HIDE, NULL))
      {
         CString  tmp;
         tmp.Format("%s\nThere was an error using tar.exe.", commandLine);
         ErrorMessage(tmp, "Tar Error");
         return;
      }
   }
   else
   if (!STRCMPI(ext,".tgz"))
   {
      // this unzips from tgz to tar
      commandLine.Format("gunzip -f \"%s\"", fullPath);
      if (ExecAndWait(commandLine, SW_HIDE, NULL))
      {
         CString  tmp;
         tmp.Format("%s\nThere was an error using gunzip.exe.", commandLine);
         ErrorMessage(tmp, "Gtz Error");
         return;
      }
      commandLine.Format("tar -xf \"%s%s%s.tar\" -C \"%s%s\"", drive, dir, fname, drive, make_unix_name(dir));
      if (ExecAndWait(commandLine, SW_HIDE, NULL))
      {
         CString  tmp;
         tmp.Format("%s\nThere was an error using tar.exe.", commandLine);
         ErrorMessage(tmp, "Tar Error");
         return;
      }
   }
   else
   {
      CString  tmp;
      tmp.Format("The file extension [%s] is unknown to the supported extract functions!", ext);
      ErrorMessage(tmp, "Can not Extraxt Extension");
   }
   
   FillFileList();
   return;
}

void ODBDirDlg::OnDblclkFiles() 
{
   OnUnzip();  
}

void ODBDirDlg::OnBnClickedOpenArchive()
{
   // TODO: Add your control notification handler code here
   CString fullPath,fileName;

   int sel = m_filesLB.GetCurSel();

   if (sel == LB_ERR)
   {
      return;
   }

   m_filesLB.GetText(sel,fileName);

   if (fileName.Left(1) == "[")
   {
      fileName.Remove('[');
      fileName.Remove(']');

      if (fileName.Left(1) == "-")
      {
         fileName.Remove('-');
      }
   }

   CFilePath archivePath(m_dir);
   archivePath.pushLeaf(fileName);
   archivePath.setDelimiterBackSlash();

   if ((archivePath.getExtension().CompareNoCase("gz" )  == 0) ||
       (archivePath.getExtension().CompareNoCase("tar")  == 0) ||
       (archivePath.getExtension().CompareNoCase("tgz")  == 0)    )
   {
      m_archiveFilePath = archivePath.getPath();
      m_archiveFileOpened = true;

      CResizingDialog::OnOK();
   }
}
