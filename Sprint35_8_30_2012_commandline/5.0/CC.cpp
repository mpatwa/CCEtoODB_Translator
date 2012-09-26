// $Header: /CAMCAD/5.0/CAMCAD.CPP 156   6/14/07 1:18p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

#include "StdAfx.h"
#include "CAMCAD.h"
#include "mainfrm.h"
#include "CCdoc.h"
#include "CCview.h"
#include "Splash.h"
#include "font.h"
#include "license.h"
#include "attrib.h"
#include <direct.h>
#include "SAX.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <fcntl.h>
#include "edit.h"
#include "wrldview.h"
#include "api_hide.h"
#include "api.h"
#include <ctype.h>
#include "childfrm.h"
#include "TreeListFrame.h"
#include "CAMCADNavigator.h"
#include "CamCad.h"
#include "SoftwareExpiration.h"
#include "ViewSynchronizer.h"
#include <afxpriv.h>
#include "olhapi.h"
#include "ProfileLib.h"
#include "DcaLib.h"
#include "DcaFileType.h"
#include "DcaEnumIterator.h"
#include "ODBC_Lib.h"
#include "element.h"
#include "DFT.h"
#include "MergeFiles.h"
#include "Encryption/InfrasecUtility.h"
#include <iostream>

#include "RwUiLib.h" // for GetLogfilePath()
#include <string>



#ifdef _DEBUG
	//*rcf This no longer works, is unresolved reference: void AFXAPI _AfxTraceMsg(LPCTSTR lpszPrefix, const MSG* pMsg);
	//*rcf This is apparantly unused:  BOOL AFXAPI _AfxCheckDialogTemplate(LPCTSTR lpszResource,
	//                              	BOOL bInvisibleChild);
#endif

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#include "bbsdefs.h"
}

#include "staticlabel.h"
#include "afxwin.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif   

//*rcf BUG this no longer works, need replacement:
//*rcf AFX_STATIC BOOL AFXAPI _AfxSetRegKey(LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL);

//_____________________________________________________________________________
#define CcMruFileCount  10  // must match number of IDs defined in Resource.h
#define CadMruFileCount 10  // must match number of IDs defined in Resource.h

static BOOL CALLBACK CcFileRecognizerFunction(LPCTSTR lpszPathName)
{
   CFilePath filePath(lpszPathName);

   bool retval = (filePath.getExtension().CompareNoCase("cc" ) == 0 ||
                  filePath.getExtension().CompareNoCase("ccz") == 0    );

   return retval;
}

static BOOL CALLBACK CadFileRecognizerFunction(LPCTSTR lpszPathName)
{
   return !CcFileRecognizerFunction(lpszPathName);
}

//_____________________________________________________________________________
extern BOOL    LoadingDataFile; // CCDOC.CPP
extern BOOL    LoadingProjectFile; // from PORT.CPP
extern License *licenses;
//extern CString user, company, serialNum;  // from LICENSE.CPP
extern BOOL    SecurityKeyUser; // 
extern CView   *activeView; // from CCVIEW.CPP
extern CEditDialog *editDlg; // SELECT.CPP
extern WorldView *worldView; // from WRLDVIEW.CPP
extern HideCAMCADDlg *hideCAMCADDlg; // from API_HIDE.CPP
                    
CMultiDocTemplate*   pDocTemplate;
CCAMCADView          *mouseView = NULL;
CString              CAMCAD_File; 
CString              NotePadProgram;
CString              LogReaderProgram;
SettingsStruct       GlSettings;
BOOL                 NoUI = -1;

char           realpartPathName[_MAX_PATH]; // this is the full path the the realpart library !!!

void init_Units();
void SetProductHelpMenuItem();
BOOL CheckWin32();

int camCadNewHandler(size_t numBytes)
{
   static bool inHandlerFlag = false;

   if (!inHandlerFlag)
   {
      inHandlerFlag = true;

      try
      {
         formatMessageBox("No memory available for request size of %I64d, fragmented or out of memory.",(__int64)numBytes);
      }
      catch (CMemoryException* memoryException)
      {
         CRuntimeClass* rtc = memoryException->GetRuntimeClass();
      }
   }

   inHandlerFlag = false;

   return 0;
}

/******************************************************************************
* MemErrorMessage
*/
void MemErrorMessage(const char* file, int line)
{
   CCAMCADApp *pApp = (CCAMCADApp*)AfxGetApp();
   CString buf;
   buf.Format("Not enough Memory\nFile:%s, Line:%d", file, line);

   if (pApp->getUseDialogsFlag())
      AfxMessageBox(buf, MB_OK | MB_ICONEXCLAMATION);

   exit(EXIT_FAILURE);
}

/******************************************************************************
* ErrorMessage
*/
int ErrorMessage(const char *text, const char *caption, UINT type)
{ 
   CCAMCADApp *pApp = (CCAMCADApp*)AfxGetApp();
   CString buf;
   buf.Format("%s\n%s", caption, text);
   
   int retval = IDOK;
   
   // If message type gets an actual choice from user, as opposed to informational
   // message that is just dismissed with OK, then we want to log the response.
   // Mainly this is for "silent running" mode, e.g. Read?Write aka ImportExport jobs,
   // such as used for vPlan DFT export. For other message types there OK is used
   // to just dismiss the popup, we don't need to log the response, it would look silly.
   bool includeResponseInLog = false;  // Assume we won't.

   // If dialogs are in use then issue message, otherwise determine default answer
   if (pApp->getUseDialogsFlag())
   {
      retval = AfxMessageBox(buf, type);
   }
   else
   {
      // If not using dialogs then auto-answer affirmative (OK or YES)
      if (type & MB_OK || type & MB_OKCANCEL)
         retval = IDOK;
      else if (type & MB_YESNOCANCEL || type & MB_YESNO)
         retval = IDYES;

      includeResponseInLog = (type & MB_OKCANCEL) || (type & MB_YESNOCANCEL) || (type & MB_YESNO); // But not MB_OK.
   }

   // Append text version of response to message.
   // The LogMessage func checks for itself if log is open, here just send the message.
   if (includeResponseInLog)
   {
      CString response;
      switch (retval) 
      {
      case IDOK:        response = "OK";     break;
      case IDCANCEL:    response = "CANCEL"; break;
      case IDABORT:     response = "ABORT";  break;
      case IDRETRY:     response = "RETRY";  break;
      case IDIGNORE:    response = "IGNORE"; break;
      case IDYES:       response = "YES";    break;
      case IDNO:        response = "NO";     break;
      }
      if (!response.IsEmpty())
         buf += "\n(Answered: " + response + ")";
   }

   // For log, add a final newline plus newline for white space before possible future message.
   buf += "\n\n";
   pApp->LogMessage(buf);

   return retval;
}

/*******************************************************************************
* Notepad()
*/
void Notepad(const char *file)
{
   CCAMCADApp *pApp = (CCAMCADApp*)AfxGetApp();

   if (NotePadProgram.IsEmpty() || !NotePadProgram.CompareNoCase("NONE") || !pApp->getUseDialogsFlag())
      return;

   CString commandLine = NotePadProgram;
   commandLine += " ";
   commandLine += "\"";
   commandLine += file; // make a quote for long filenames
   commandLine += "\"";

   int res = WinExec(commandLine, SW_SHOW);
   switch (res)
   {
   case 0:
      MessageBox(NULL, "The system is out of memory or resources!","LogReader", MB_OK | MB_ICONHAND);
      break;
   case ERROR_BAD_FORMAT:
      MessageBox(NULL, "The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)!","Notepad", MB_OK | MB_ICONHAND);
      break;
   case ERROR_FILE_NOT_FOUND:
      {
         CString buf;
         buf.Format("The specified file [%s] was not found!", commandLine);
         MessageBox(NULL, buf, "Notepad", MB_OK | MB_ICONHAND);
      }
      break;
   case ERROR_PATH_NOT_FOUND:
      {
         CString buf;
         buf.Format("The specified path [%s] was not found!", commandLine);
         MessageBox(NULL, buf, "Notepad", MB_OK | MB_ICONHAND);
      }
      break;
   }
}

/*******************************************************************************
* Logreader()
*/
void Logreader(const char *file)
{
   CCAMCADApp *pApp = (CCAMCADApp*)AfxGetApp();
   if (LogReaderProgram.IsEmpty() || !LogReaderProgram.CompareNoCase("NONE") || !pApp->getUseDialogsFlag())
      return;

   int done = 0;
   CString commandLine = LogReaderProgram;
   commandLine += " ";
   commandLine += file;

   while (done < 3) // 0 try in general path, 1 = try in exe path, 2 = try in userpath
   {
      int res = WinExec(commandLine, SW_SHOW);

      switch (res)
      {
      case 0:
         MessageBox(NULL, "The system is out of memory or resources!","LogReader", MB_OK | MB_ICONHAND);
         return;

      case ERROR_BAD_FORMAT:
         MessageBox(NULL, "The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)!","LogReader", MB_OK | MB_ICONHAND);
         return;

      case ERROR_FILE_NOT_FOUND:
         {
            // here if the command could not be found, see if the current CAMCAD EXE path contains it.
            CString  t;
            done++;

            if (done == 1)
            {
               t = pApp->getCamcadExeFolderPath();
               t += LogReaderProgram;
               t += " ";
               t += file;
               commandLine = t;
               break;
            }
            else if (done == 2)
            {
               t = pApp->getUserPath();
               t += LogReaderProgram;
               t += " ";
               t += file;
               commandLine = t;
               break;
            }
   
            t.Format("The specified file [%s] was not found!",commandLine);
            MessageBox(NULL, t, "LogReader", MB_OK | MB_ICONHAND);
            return;
         }
         break;

      case ERROR_PATH_NOT_FOUND:
         {  
            CString buf;
            buf.Format("The specified path [%s] was not found!", commandLine);
            MessageBox(NULL, buf, "LogReader", MB_OK | MB_ICONHAND);
            return;
         }

      default:
         return;
      }
   }
}

//_____________________________________________________________________________
CDocument* CMyMultiDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible)
{
   CCAMCADApp *pApp = (CCAMCADApp*)AfxGetApp();

   if (pApp->getRunMode() == runModeDataProtectedModelessDialog)
   {
      return NULL;
   }

   if (!pApp->getSchematicLinkController().requestDeleteOfSchematicLink())
   {
      return NULL;
   }

   return CMultiDocTemplate::OpenDocumentFile(lpszPathName, bMakeVisible);
}

/////////////////////////////////////////////////////////////////////////////
// CCAMCADApp

BEGIN_MESSAGE_MAP(CCAMCADApp, CWinApp)
   ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
   //{{AFX_MSG_MAP(CCAMCADApp)
   //ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
   ON_COMMAND(ID_SETTINGS_LOADGLOBALSETTINGS, OnSettingsLoadGlobal)
   ON_COMMAND(ID_LOAD_DATAFILE, OnLoadDataFile)
   ON_COMMAND(ID_LOADPROJECTFILE, OnLoadProjectFile)
   ON_COMMAND(ID_LOAD_FONT, OnLoadFont)
   ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
   ON_COMMAND(ID_MACROS, OnMacros)
   ON_COMMAND(ID_RUN_MACRO, OnRunMacro)
   ON_COMMAND(ID_SET_PROJECT_PATH, OnSetProjectPath)
   ON_COMMAND(ID_SETATTRIBUTE_EDIT,OnEditAttrTemplate)
   ON_COMMAND(ID_SETTEMPLATEDIRECTORY,OnSetTemplateDirectory)
   ON_COMMAND(ID_Menu33879,OnEditCustomTemplate)
   ON_COMMAND(ID_CAMCAD_COM, OnCamcadCom)
   ON_COMMAND(ID_RSI_COM, OnRsiCom)
   //}}AFX_MSG_MAP
   // Standard file based document commands
   ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
   ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
   // Standard print setup command
   ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
   ON_COMMAND(ID_ONLINE_HELP, OnOnlineHelp)
   ON_COMMAND(ID_OPEN_CAD_FILE, OnOpenCADFile)
   ON_COMMAND(ID_SCHEMATICLINK_SCHEMATICLINK, OnToolsSchematiclink)
   ON_COMMAND(ID_LINK_NETS, OnLinkNets)
   ON_COMMAND(ID_LINK_COMPS, OnLinkComps)
   ON_COMMAND(ID_SCHEMATIC_SETTINGS, OnSchematicSettings)
   ON_UPDATE_COMMAND_UI(ID_LINK_NETS, OnUpdateLinkNets)
   ON_UPDATE_COMMAND_UI(ID_LINK_COMPS, OnUpdateLinkComps)
   ON_UPDATE_COMMAND_UI(ID_SCHEMATIC_SETTINGS, OnUpdateSchematicSettings)
   ON_COMMAND(ID_DFM_RUNNER, OnDfmRunner)
   ON_COMMAND(ID_DFM_SCRIPTER, OnDfmScripter)
   ON_COMMAND(ID_TOOLS_RSI_EXCHANGE, OnToolsRsiExchange)
   ON_COMMAND(ID_TOOLS_BOM_EXP, OnToolsBomExplorer)
   ON_COMMAND(ID_TOOLS_REALPART, OnToolsRealpart)
   ON_COMMAND(ID_HELP_INFOHUB, OnHelpInfoHub)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCAMCADApp construction
void InitTrace();
void FreeTrace();

CCAMCADApp::CCAMCADApp()
: m_mruFileManager(this)
, m_docTemplate(NULL)
, m_messageFilterTypeMessage(messageFilterTypeMessage)
, m_messageFilterTypeFormat(messageFilterTypeFormat)
, m_viewSynchronizer(NULL)
, m_logFp(NULL)
, m_cceDecryptionAllowed(false)
, m_exitCode(ExitCodeSuccess)
{
   m_hwndDialog = NULL; // TOOLTIPS
   m_gpToolTip = NULL;  // TOOLTIPS
   schLinkState = FALSE;   // Schematic linking
   MostlyHideCamcad = FALSE;
   SilentRunning = FALSE;
   CheckoutLics = TRUE;
   LicenseTimerEnabled = FALSE;
   UsingAutomation = FALSE;
   OnlyRegister = FALSE;
	CopyToTestFixLocation = false;

   setUseDialogsFlag(true);

	// ViewMode will only be set throught command line in the function CMyCommandLineInfo::ParseParam().
	// Therefore initilize to -1 instead of SW_SHOWMAXIMIZED so later on the function
	// CMainFrame::restoreWindowState() will be able to tell that ViewMode is set by commandline if 
	// it is greater than -1
   ViewMode = -1;	

   m_runMode = runModeNormal;
}

CCAMCADApp::~CCAMCADApp()
{
   CloseCamcadLogFile();

   delete m_viewSynchronizer;
}

CViewSynchronizer& CCAMCADApp::getViewSynchronizer()
{
   if (m_viewSynchronizer == NULL)
   {
      m_viewSynchronizer = new CViewSynchronizer();
   }

   return *m_viewSynchronizer;
}

void CCAMCADApp::releaseViewSynchronizer()
{
   delete m_viewSynchronizer;
   m_viewSynchronizer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCAMCADApp object

CCAMCADApp theApp;

CCAMCADApp& getApp()
{
   return theApp;
}

CWnd* getMainWnd()
{
   return getApp().m_pMainWnd;
}

CMainFrame* getMainFrame()
{
   return ((CMainFrame*)getMainWnd());
}

static bool s_showHiddenAttributes = true;

bool getShowHiddenAttributes()
{
   return s_showHiddenAttributes;
}

bool setShowHiddenAttributes(bool flag)
{
   bool retval = s_showHiddenAttributes;

   s_showHiddenAttributes = flag;

   //getApp().WriteProfileInt("Settings","ShowHiddenAttributes",s_showHiddenAttributes);

   return retval;
}

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {5BECA79B-EF3B-11D1-BA40-0080ADB36DBB}
static const CLSID clsid =
{ 0x5beca79b, 0xef3b, 0x11d1, { 0xba, 0x40, 0x0, 0x80, 0xad, 0xb3, 0x6d, 0xbb } };


/////////////////////////////////////////////////////////////////////////////
// CCAMCADApp initialization
LRESULT CALLBACK LocWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RegisterWindow(CWinApp *pApp)
{
   WNDCLASS   wndClass;
   ATOM wndAtom;
   CString clsName;
   clsName.LoadString(IDR_MAINFRAME);

   wndClass.lpfnWndProc   = (WNDPROC)LocWndProc;
   wndClass.style         = 0;   //CS_HREDRAW | CS_VREDRAW;
   wndClass.cbClsExtra    = 0;
   wndClass.cbWndExtra    = 0;
   wndClass.hInstance     = pApp->m_hInstance;
   wndClass.hIcon         = NULL;   //pApp->LoadIcon( "IDR_MAINFRAME" );
   wndClass.hCursor       = NULL;   //LoadCursor( NULL, IDC_ARROW);
   wndClass.hbrBackground = NULL;
   wndClass.lpszMenuName  = (LPSTR)NULL;
   wndClass.lpszClassName = clsName;         /* Class Name is App Name*/

   /* register the window class */
   wndAtom = RegisterClass( &wndClass );
   if (!wndAtom)
   {
      CString msg;
      LPVOID lpMsgBuf;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), 0, (LPTSTR) &lpMsgBuf, 0, NULL);
      msg.Format("Error registering application window.\n\n%s", (LPCTSTR)lpMsgBuf);
      LocalFree(lpMsgBuf);

      AfxMessageBox(msg);
   }
}

/******************************************************************************
* AlreadyRunning
*/
BOOL AlreadyRunning()
{
   // this function only works on NT and 2000, it does not work on 95, 98, ME !!!
   if (Platform != WINNT)
      return FALSE;

   HWND wnd;
   CString clsName;

   clsName.LoadString(IDR_MAINFRAME);
   if (!(wnd = FindWindow(clsName, NULL)))
   {
      DWORD errCode = GetLastError();
      return !errCode;
   }
   return TRUE;
}

//_____________________________________________________________________________
RunModeTag CCAMCADApp::getRunMode() const
{
   return m_runMode;
}

void CCAMCADApp::setRunMode(RunModeTag runMode)
{
   m_runMode = runMode;
}

bool CCAMCADApp::getUseDialogsFlag() const
{
   return m_useDialogsFlag;
}

void CCAMCADApp::setUseDialogsFlag(bool flag)
{
   m_useDialogsFlag = flag;

   CMessageFilter::setUseDialogsFlag(m_useDialogsFlag);
}

BOOL CCAMCADApp::ProcessMessageFilter(int code, LPMSG lpMsg)   // TOOLTIPS
{
   if (m_hwndDialog != NULL)
      if (lpMsg->hwnd == m_hwndDialog || ::IsChild(m_hwndDialog, lpMsg->hwnd))
         if (NULL != m_gpToolTip)
            m_gpToolTip->RelayEvent(lpMsg);

   return CWinApp::ProcessMessageFilter(code, lpMsg);
}

/******************************************************************************
* GetCAMCADTitle
*/
CString CCAMCADApp::getCamCadSubtitle() const
{
   // The one and only original app name, now the default app name.
   CString title = "CAMCAD ";

   // For vPlan integration we're wanting the final title bar to be
   // "vPlan Test and Inspection Engineering".
   // We might shorten that, e.g. 
   // "Test and Inspection Session" was suggested. It is a little shorter.
   // The goofy type casting is because this is a const function. Maybe should get rid of that.
   if (((CTXPJob&)(this->m_txpJob)).IsActivated())
   {
      title = "vPlan Test and Inspection Engineering "; // Don't forget space after name.
   }
   else
   {
      // Normal CAMCAD, get specific CAMCAD product.

#ifdef SHAREWARE // Shareware or Professional in Frame Title
      title += "Shareware ";
#else
      switch (Product)
      {
      case PRODUCT_PROFESSIONAL:
         title += "Professional ";
         break;
      case PRODUCT_PCB_TRANSLATOR:
         title += "PCB Translator ";
         break;
      case PRODUCT_VISION:
         title += "Vision ";  // no CAMCAD in title
         break;
      default:
         title += "Graphic ";
         break;
      }
#endif
   }

	CString curVersion = getVersionString().Mid(0, getApp().getVersionString().ReverseFind('.'));
   title += curVersion;
   
   return title;
}

CString CCAMCADApp::getCamCadTitle() const
{
   CString title = getCamCadSubtitle();

   for (int index = 0;index < m_titleSuffixes.GetCount();index++)
   {
      title += m_titleSuffixes.GetAt(index);
   }

   return title;
}

void CCAMCADApp::setTitleSuffix(const CString& suffix)
{
   m_titleSuffixes.RemoveAll();
   pushTitleSuffix(suffix);
}

void CCAMCADApp::pushTitleSuffix(const CString& suffix)
{
   m_titleSuffixes.Add(suffix);
}

CString CCAMCADApp::popTitleSuffix()
{
   CString suffix;
   int count = m_titleSuffixes.GetCount();

   if (count > 0)
   {
      suffix = m_titleSuffixes.GetAt(count - 1);
      m_titleSuffixes.RemoveAt(count - 1);
   } 

   return suffix;
}

CString CCAMCADApp::getUserPath() const
{
   return m_userPath;
}

CString CCAMCADApp::getCamcadExeFolderPath() const
{
   return m_camcadExeFolderPath;
}

CString CCAMCADApp::getCmdLineImportSettingsFilePath() const
{
   return m_cmdlineImportSettingsFilePath;
}

CString CCAMCADApp::getCmdLineExportSettingsFilePath() const
{
   return m_cmdlineExportSettingsFilePath;
}

void CCAMCADApp::setCmdLineImportSettingsFilePath(CString filepath)
{
   m_cmdlineImportSettingsFilePath = filepath;
}

void CCAMCADApp::setCmdLineExportSettingsFilePath(CString filepath)
{
   m_cmdlineExportSettingsFilePath = filepath;
}

CString CCAMCADApp::getImportSettingsFilePath(CString defaultFilename)
{
   // Convenience function that performs the standard pattern of using command line settings file
   // setting if present, otherwise default to camcad install path plus default file name.

	// First try app setting, was optionally set using command line param.
   CString settingsFile = getApp().getCmdLineImportSettingsFilePath();

   // If blank then do standard system file folder lookup
   if (settingsFile.IsEmpty())
      settingsFile = getApp().getSystemSettingsFilePath(defaultFilename);

   return settingsFile;
}

CString CCAMCADApp::getExportSettingsFilePath(CString defaultFilename)
{
   // Convenience function that performs the standard pattern of using command line settings file
   // setting if present, otherwise default to camcad install path plus default file name.

	// First try app setting, was optionally set using command line param.
   CString settingsFile = getApp().getCmdLineExportSettingsFilePath();

   // If blank then do standard system file folder lookup
   if (settingsFile.IsEmpty())
      settingsFile = getApp().getSystemSettingsFilePath(defaultFilename);

   return settingsFile;
}



CString CCAMCADApp::getSystemSettingsFilePath(CString settingsFilename)
{
   // Favor userpath, user might have set something up.
   // If desired file is not there then set to camcad path.

   // This is the form most places in CAMCAD used before this function
   // existed. Use it as the first try.
   CString settingsFile = getUserPath() + settingsFilename;

   if (!fileExists(settingsFile))
   {
      // Build a path that goes to camcad exe location.
      // Return it without bothering to check, this is the last ditch, exists or not.
      settingsFile = getCamcadExeFolderPath() + settingsFilename;
   }

   return settingsFile;
}

/******************************************************************************
* CCAMCADApp::CopyCAMCADToLocation
*/
void CCAMCADApp::CopyCAMCADToLocation()
{
#if defined NDEBUG
   CString releaseDirectory;
   releaseDirectory.Format("\\\\Cob\\dfs\\sms-build\\Developers\\CamCad\\%d.%d",CamCadMajorVersion,CamCadMinorVersion);

   CopyCAMCADToLocation(releaseDirectory);

   //if (!CopyCAMCADToLocation("\\\\Debug\\c$\\Development\\CAMCADTestFix"))
   //{
   //   //CopyCAMCADToLocation("\\\\Rsi001\\M\\CAMCADTestFix");
   //}
#endif
}

DWORD CALLBACK camCadCopyProgressRoutine(
  LARGE_INTEGER TotalFileSize,
  LARGE_INTEGER TotalBytesTransferred,
  LARGE_INTEGER StreamSize,
  LARGE_INTEGER StreamBytesTransferred,
  DWORD dwStreamNumber,
  DWORD dwCallbackReason,
  HANDLE hSourceFile,
  HANDLE hDestinationFile,
  LPVOID lpData
)
{
   if (lpData != NULL)
   {
      COperationProgress* progress = (COperationProgress*)lpData;

      double position = ((double)TotalBytesTransferred.QuadPart)/TotalFileSize.QuadPart;
      progress->updateProgress(position);
   }

   return PROGRESS_CONTINUE;
}


bool CCAMCADApp::CopyCAMCADToLocation(const char* destinationDirectory)
{
   bool retval = false;

#if defined NDEBUG
   //formatMessageBox("Copying CamCad to '%s'",destinationDirectory);

   CFileStatus fileStatus;
   retval = (CFile::GetStatus(destinationDirectory,fileStatus) != 0);

   if (retval)
   {
	   CString toPath, fromPath;

      // find executable's path
	   char fullPath[_MAX_PATH], drive[_MAX_DRIVE], path[_MAX_DIR], filename[_MAX_FNAME], ext[_MAX_EXT], tempPath[_MAX_PATH];
	   strcpy(fullPath, m_pszHelpFilePath);
	   _splitpath(fullPath, drive, path, filename, ext);
	   _makepath(tempPath, drive, path, NULL, NULL);
   	
	   fromPath = tempPath;
	   toPath.Format("%s\\%s",destinationDirectory,getApp().getVersionString());

	   CFileFind findFile;
	   CString cmd;

	   // make sure the path exists
	   if (!findFile.FindFile(toPath))
	   {
		   SECURITY_ATTRIBUTES sAtt;
		   sAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
		   sAtt.lpSecurityDescriptor = NULL;
		   sAtt.bInheritHandle = FALSE;

		   // create the directory on test fix location per version
		   if (!CreateDirectory(toPath, &sAtt))
			   return false;
	   }
	   toPath += "\\";

	   CString fromFile, toFile;
      COperationProgress progress;
      CString progressMessage;
      BOOL copyCancel=0;
      DWORD copyFlags=0;

	   // Copy camcad exe to destination
	   fromFile.Format("%s%s.%s", fromPath, m_pszExeName, "EXE");
	   toFile.Format("%s%s.%s", toPath, m_pszExeName, "EXE");

      progressMessage.Format("Copying to '%s'",toFile);
      progress.updateStatus(progressMessage,1.);

	   CopyFileEx(fromFile, toFile,camCadCopyProgressRoutine,&progress,&copyCancel,copyFlags);

	   // Copy the tlb to destination
	   fromFile.Format("%s%s.%s", fromPath, m_pszExeName, "TLB");
	   toFile.Format("%s%s.%s", toPath, m_pszExeName, "TLB");

      progressMessage.Format("Copying to '%s'",toFile);
      progress.updateStatus(progressMessage,1.);

	   CopyFileEx(fromFile, toFile,camCadCopyProgressRoutine,&progress,&copyCancel,copyFlags);

	   // Copy the version file to destination
	   CString curVersion, sourcePath;
#ifdef OLD_STYLE__BASED_ON_VERSION_IN_BUILD_NAME
      // Presumptuous, does not work if source/build folder is not named after build version.
		curVersion = getApp().getVersionString().Mid(0, getApp().getVersionString().ReverseFind('.'));
		curVersion = curVersion.Mid(0, curVersion.ReverseFind('.'));
	   sourcePath.Format("%s%s\\", fromPath, curVersion);
      fromFile.Format("%s%s.%s", sourcePath, "Version", "CPP");
#else
      // Look for it, also presumptuous, based on standard camcad source/build folder hierarchy.
      fromFile = RecursiveFindFilePath(fromPath, "Version.cpp");
#endif

      if (!fromFile.IsEmpty())
      {
         toFile.Format("%s%s.%s", toPath, "Version", "CPP");

         progressMessage.Format("Copying to '%s'",toFile);
         progress.updateStatus(progressMessage,1.);

         CopyFileEx(fromFile, toFile,camCadCopyProgressRoutine,&progress,&copyCancel,copyFlags);
      }
   }
#endif

   return retval;
}

/******************************************************************************
* CCAMCADApp::SetCAMCADTitle
*/
void CCAMCADApp::SetCAMCADTitle()
{
   CString title = getCamCadSubtitle();

   // Reset the title suffix
   getApp().setTitleSuffix("");

#if defined _RELTEST
   getApp().pushTitleSuffix(" (Release Test)");
#endif

   if (getApp().getCamcadLicense().hasBaseLicense())
   {
      getApp().pushTitleSuffix(" - Licensed");      
   }
   else
   {
      getApp().pushTitleSuffix(" - Unlicensed"); 
   }


	CString projectPath;
	CCAMCADView* view = getActiveView();
	if (view != NULL)
		projectPath =  (view->GetDocument()!=NULL)?view->GetDocument()->GetProjectPath():"";

   if (projectPath.GetLength())
   {
      getApp().pushTitleSuffix(" - {" + projectPath + "}");
   }

   getMainFrame()->SetFrameTitle(getApp().getCamCadTitle());
   getMainWnd()->SetWindowText(getApp().getCamCadTitle());
}
 
/******************************************************************************
* CCAMCADApp::GetProductKey
*/
CString CCAMCADApp::GetProductKey()
{
   CString keyname = "Software\\" + (CString)REGISTRY_COMPANY + "\\CAMCAD\\Settings";

   HRESULT res;
   HKEY settingsKey;
   res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_READ, &settingsKey);
   if (res != ERROR_SUCCESS)
      return "";  // no settings key in the local machine

   char product[15];

   DWORD bufLen = 15, lType;
   res = RegQueryValueEx(settingsKey, "Product", NULL, &lType, (LPBYTE)product, &bufLen);
   if (res != ERROR_SUCCESS)
   {
      RegCloseKey(settingsKey);
      return "";  // no product setting in the local machine
   }
   RegCloseKey(settingsKey);

	// push the value to current user
	res = RegCreateKeyEx(HKEY_CURRENT_USER, keyname, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &settingsKey, NULL);
	if (res == ERROR_SUCCESS)	// TypeLib found
	{
		res = RegSetValueEx(settingsKey, "Product", NULL, REG_SZ, (LPBYTE)product, bufLen);
	
		RegCloseKey(settingsKey);
	}

   return product;
}

/******************************************************************************
* CCAMCADApp::GetCWDKey
* 
* CAMCAD reads directory setting from Local Machine and writes to Current User
* CAMCAD reads directory setting from Local Machine Product and write to Current User Product
* If CAMCAD cannot find Local Machine Product, then reads from Current User Product.
*/
CString CCAMCADApp::GetCWDKey()
{
   CString settingsLocation;
	HRESULT res;
   HKEY settingsKey;
   char dir[_MAX_PATH];
   DWORD bufLen = _MAX_PATH, lType;
	CString ccDir, ccProductDir;

	// CAMCAD reads directory setting from Local Machine and writes to Current User
	settingsLocation = "Software\\" + (CString)REGISTRY_COMPANY + "\\CAMCAD\\Settings";
   res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, settingsLocation, 0, KEY_READ, &settingsKey);
	if (res == S_OK)
	{
		// Read CAMCAD directory setting from Local Machine
		res = RegQueryValueEx(settingsKey, "Directory", NULL, &lType, (LPBYTE)dir, &bufLen);
		if (res != S_OK)
			dir[0] = '\0';
		dir[bufLen] = '\0';
		RegCloseKey(settingsKey);

		if (strlen(dir) > 0)
		{
			// push the value to current user under CAMCAD
			res = RegCreateKeyEx(HKEY_CURRENT_USER, settingsLocation, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &settingsKey, NULL);
			if (res == S_OK)
			{
				res = RegSetValueEx(settingsKey, "Directory", NULL, REG_SZ, (LPBYTE)dir, bufLen);
			
				RegCloseKey(settingsKey);
			}
		}
	}
	ccDir = dir;

	// CAMCAD reads directory setting from Local Machine Product and write to Current User Product
	settingsLocation = "Software\\" + (CString)REGISTRY_COMPANY + "\\" + m_pszProfileName + "\\Settings";
   res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, settingsLocation, 0, KEY_READ, &settingsKey);
	if (res == S_OK)
	{
		// Read CAMCAD product directory setting from Local Machine
		res = RegQueryValueEx(settingsKey, "Directory", NULL, &lType, (LPBYTE)dir, &bufLen);
		if (res != S_OK)
			dir[0] = '\0';
		dir[bufLen] = '\0';
		RegCloseKey(settingsKey);

		if (strlen(dir) > 0)
		{
			// push the value to current user under the specific CAMCAD product
			res = RegCreateKeyEx(HKEY_CURRENT_USER, settingsLocation, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &settingsKey, NULL);
			if (res == S_OK)
			{
				res = RegSetValueEx(settingsKey, "Directory", NULL, REG_SZ, (LPBYTE)dir, bufLen);
			
				RegCloseKey(settingsKey);
			}
		}
	}
	ccProductDir = dir;

	if (ccProductDir.IsEmpty())
		return ccDir;
	else
		return ccProductDir;
}

/******************************************************************************
* CCAMCADApp::updateProductRegistry
*/
void CCAMCADApp::updateProductRegistry()
{
   CString localMachineProduct;
   CString keyname = "Software\\" + (CString)REGISTRY_COMPANY + "\\CAMCAD\\Settings";

   HRESULT res;
   HKEY settingsKey;
   res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_READ, &settingsKey);
   if (res != ERROR_SUCCESS)
      return;  // no settings key in the local machine

   char product[15];
   DWORD bufLen = 15;
   res = RegQueryValueEx(settingsKey, "Product", NULL, NULL, (LPBYTE)product, &bufLen);
   if (res != ERROR_SUCCESS)
   {
      RegCloseKey(settingsKey);
      return;  // no product setting in the local machine
   }

   localMachineProduct = product;
   WriteProfileString("Settings", "Product", localMachineProduct);

   RegCloseKey(settingsKey);
}

/******************************************************************************
* CCAMCADApp::updateInterfaceRegistry
*/
void CCAMCADApp::updateInterfaceRegistry(CString clsID, CString interfaceID)
{
   CString cwd, interfaceLoc;

   HRESULT res;
   HKEY interfaceKey;
   interfaceLoc.Format("Interface", m_pszProfileName);
   res = RegOpenKeyEx(HKEY_CLASSES_ROOT, interfaceLoc, 0, KEY_WRITE, &interfaceKey);
   if (res != ERROR_SUCCESS)
      return;  // no interface key in the classes root

   HKEY clsIDKey;
   res = RegCreateKeyEx(interfaceKey, clsID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &clsIDKey, NULL);
   if (res == ERROR_SUCCESS)	// clsid found
   {
		CString value = interfaceID;
		DWORD bufLen = value.GetLength();
		res = RegSetValueEx(clsIDKey, "", NULL, REG_SZ, (LPBYTE)value.GetBuffer(0), bufLen);

		HKEY subKey;
		res = RegCreateKeyEx(clsIDKey, "TypeLib", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &subKey, NULL);
		if (res == ERROR_SUCCESS)	// TypeLib found
		{
			value = clsID;
			bufLen = value.GetLength();
			res = RegSetValueEx(subKey, "", NULL, REG_SZ, (LPBYTE)value.GetBuffer(0), bufLen);

			value = "1.0";
			bufLen = value.GetLength();
			res = RegSetValueEx(subKey, "Version", NULL, REG_SZ, (LPBYTE)value.GetBuffer(0), bufLen);

			RegCloseKey(subKey);
		}

		res = RegCreateKeyEx(clsIDKey, "ProxyStubClsid32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &subKey, NULL);
		if (res == ERROR_SUCCESS)	// ProxyStubClsid32 found
		{
			value = "{00020420-0000-0000-C000-000000000046}";
			bufLen = value.GetLength();
			res = RegSetValueEx(subKey, "", NULL, REG_SZ, (LPBYTE)value.GetBuffer(0), bufLen);

			RegCloseKey(subKey);
		}

		res = RegCreateKeyEx(clsIDKey, "ProxyStubClsid", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &subKey, NULL);
		if (res == ERROR_SUCCESS)	// ProxyStubClsid found
		{
			value = "{00020420-0000-0000-C000-000000000046}";
			bufLen = value.GetLength();
			res = RegSetValueEx(subKey, "", NULL, REG_SZ, (LPBYTE)value.GetBuffer(0), bufLen);

			RegCloseKey(subKey);
		}

		RegCloseKey(clsIDKey);
   }
   
   RegCloseKey(interfaceKey);
}

/******************************************************************************
* CCAMCADApp::showMainWindow
*/
void CCAMCADApp::showMainWindow()
{
   if (!MostlyHideCamcad && !SilentRunning)
   {
		((CMainFrame*)m_pMainWnd)->restoreWindowState();
      m_pMainWnd->UpdateWindow();
   }
   else
   {
      if (hideCAMCADDlg)
         return;

      hideCAMCADDlg = new HideCAMCADDlg;

      m_pMainWnd->ShowWindow(SW_MINIMIZE);

      hideCAMCADDlg->Create(IDD_HIDE_CAMCAD);
      hideCAMCADDlg->ShowWindow( SilentRunning ? SW_HIDE : SW_SHOW);
      hideCAMCADDlg->SetWindowPos(&hideCAMCADDlg->wndTopMost, 0, 0, 1, 1, SWP_NOMOVE | SWP_NOSIZE);

      FlushQueue();
   }
}

BOOL CCAMCADApp::OnCmdMsg(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo)
{
   BOOL retval;

   if (m_mruFileManager.OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
   {
      retval = TRUE;
   }
   else
   {
      retval = CWinApp::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
   }

   return retval;
}

void CCAMCADApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
   if (!m_mruFileManager.AddToRecentFileList(lpszPathName))
   {
      CWinApp::AddToRecentFileList(lpszPathName);
   }
}

void CCAMCADApp::CloseCamcadLogFile()
{
   // m_logFp might be to stdout, but that is okay. One can't actually close stdout
   // but also it does no harm to try it (right?) so we don't need to check.

   if (m_logFp != NULL)
   {
      fclose(m_logFp);
      m_logFp = NULL;  // So destructor does not try to do it again.
   }

}

void CCAMCADApp::OpenCamcadLogFile()
{
   if (m_logFp == NULL)
   {
      // If log file name is set (by command line) then if it specifies a full path use it as-is.
      // UNLESS it is the special name "stdout", in which case we just use stdout as the log file fp.
      // Otherwise apply the standard log file path location using name for leaf.
      // If not set then default to "camcad.log".
      // If user log file name is set but fails to open, try default log file.

      CString defaultLogFilePath( GetLogfilePath("cce2odbb.log") );
      CString userLogFilePath;

      // Get the fully resolved log file path unless the name is "stdout".
      if (!this->m_logfileName.IsEmpty() && this->m_logfileName.CompareNoCase("stdout") != 0)
      {
         CFilePath filepath(this->m_logfileName);
         if (filepath.isAbsolutePath())
            userLogFilePath = this->m_logfileName; // is absolute path, use as-is
         else
            userLogFilePath = GetLogfilePath(filepath.getFileName()); // use file name leaf and default log location

      }
      
      FILE *userLogFp = NULL; // used to track success of user log file open

      // Special case of stdout
      if (this->m_logfileName.CompareNoCase("stdout") == 0)
         m_logFp = userLogFp = stdout;

      // If not stdout then try user log file
      if (m_logFp == NULL && !userLogFilePath.IsEmpty())
      {
         // Ensure directory path to location of log exists.
         CString logDirPath = dirname(userLogFilePath);
         if (!fileExists(logDirPath))
            mkdirtree(logDirPath);
         // Open it
         m_logFp = userLogFp = fopen(userLogFilePath, "wt");
      }

      // If log fp is still NULL then user path is blank, not stdout, or failed, try default.
      if (m_logFp == NULL)
      {
         // Ensure directory path to location of log exists.
         CString logDirPath = dirname(defaultLogFilePath);
         if (!fileExists(logDirPath))
            mkdirtree(logDirPath);
         // Open it
         m_logFp = m_logFp = fopen(defaultLogFilePath, "wt");
      }

      // If log managed to open then write header
      if (m_logFp != NULL)
      {
         CTime time = CTime::GetCurrentTime();
         CString createdBy;
         createdBy.Format("Created by %s v.%s", getApp().m_pszProfileName, getApp().getVersionString());

         fprintf(m_logFp, "\n"); // Initial blank line to make it look better if sent to vPlan system log
         fprintf(m_logFp, "*--------------------------------------------------------*\n");
         fprintf(m_logFp, "*  %-53s *\n", "CCE to ODB++ Log File");
         //fprintf(m_logFp, "*  %-53s *\n", createdBy);
         fprintf(m_logFp, "*  %-53s *\n", time.Format("%A, %B %d, %Y"));
         fprintf(m_logFp, "*--------------------------------------------------------*\n\n");

         // If command line is not blank then write it to the log file
         CString cmdLine( GetCommandLine() );
         if (!cmdLine.IsEmpty())
         {
            fprintf(m_logFp, "Command Line:\n");
            fprintf(m_logFp, "%s\n\n", cmdLine);
         }

         // If userLogFilePath is not empty and userLogFp is NULL, then user specified
         // log file failed to open and we are writing in default log file.
         // Make note of this failure.
         if (!userLogFilePath.IsEmpty() && userLogFp == NULL)
         {
            fprintf(m_logFp, "Could not open user log file for writing.\n[%s]\n\n", userLogFilePath);
         }
      }
   }
}


FILE * CCAMCADApp::OpenOperationLogFile(CString filename, CString& filepath)
{
   // Open log file for operations like CAD Import, CAD Export, or anything else
   // that opens a log file, with the exception of the overall system log file, which
   // is opened above in OpenCamcadLogFile().

   // If OpenCamcadLogFile has previously been called, then this function returns
   // a ptr to that same FP, so all goes into the same log file. If it has not, then
   // the "traditional" log file is opened.
   // The traditional log file name is passed in here in filename.
   // If it turns out that gets used, then the resulting complete file path is
   // returned in filepath. If filename doesn't get used then filepath is returned empty.

   // Clear the result filepath.
   filepath.Empty();

   // Try system logfile fp. If not NULL then system log file is in play already, just use it.
   FILE *fp = getApp().GetSystemLogFp();

   // If fp still NULL then use older classic traditional (etc) style log.
   if (fp == NULL && !filename.IsEmpty())
   {
      filepath = GetLogfilePath(filename);

      if ((fp = fopen(filepath,"wt")) == NULL)
      {
         CString t;
         t.Format("Error opening log file [%s].", filepath);
         ErrorMessage(t, "Log File Error");
      }
   }

   return fp;
}

/******************************************************************************
* CCAMCADApp::InitInstance
*/
BOOL CCAMCADApp::InitInstance()
{
   _set_new_handler(camCadNewHandler);
   _set_new_mode(1);

   CMyCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   this->OpenCamcadLogFile();

   if ( !cmdInfo.validParameters())
   {
	   getApp().LogMessage("\nCheck parameters");
       this->CloseCamcadLogFile();
	   return false;
   }


   // In this mode all error messages are routed to log file, so open the log file.
	this->OpenCamcadLogFile();

   //std::cout <<"hello";

   m_camcadLicense.initializeLicenses();
   if (m_camcadLicense.checkOutLicenses() == false )
   {
	   getApp().LogMessage("\nLicense to translate from CCE to ODB++ is missing.\n\n");
       this->CloseCamcadLogFile();
	   return false;
   }

   this->SilentRunning = TRUE;

   // Working Directory from Registry
   CString cwd = GetCWDKey();	//GetProfileString("Settings", "Directory");
   if (!cwd.IsEmpty())
      _chdir(cwd);

   // find executable's path
   char fullPath[_MAX_PATH], drive[_MAX_DRIVE], path[_MAX_DIR], exePathName[_MAX_PATH]; 
   strcpy(fullPath, m_pszHelpFilePath);
   _splitpath(fullPath, drive, path, NULL, NULL);
   _makepath(exePathName, drive, path, NULL, NULL);
   m_camcadExeFolderPath = exePathName;

	char userPathName[_MAX_PATH];
   // Set userPathName to cwd if not in TXP mode. Note that if regisstry Directory was set
   // then cwd is already set to that registry setting.
   // If in TXP mode then set userPathName to the CAMCAD exe location.
   // The main use of this setting is to find auxiliary CAMCAD files, like default.fnt, as
   // well as things like import/export settings files.
   if (!this->m_txpJob.IsActivated())
   {
      _getcwd(userPathName, _MAX_PATH);
   }
   else
   {
      strncpy(userPathName, m_camcadExeFolderPath, _MAX_PATH);
   }

	if (userPathName[strlen(userPathName)-1] != '\\')
		strcat(userPathName, "\\");	
	m_userPath = userPathName;

   realpartPathName[0] = 0;   // 

   // Miten -- Check if we need this
   m_docTemplate = new CMyMultiDocTemplate(
      IDR_CAMCADTYPE,
      RUNTIME_CLASS(CCAMCADDoc),
      RUNTIME_CLASS(CChildFrame),          // standard MDI child frame
      RUNTIME_CLASS(CCAMCADView));
   AddDocTemplate(m_docTemplate);

   pDocTemplate = m_docTemplate;

   // create main MDI Frame window
   CMainFrame* pMainFrame = new CMainFrame;

   if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
      return FALSE;

   m_pMainWnd = pMainFrame;


   

   init_Units();

   GlSettings.initializeSettings();









   
   /*{ // added for 7.0
      // InitCommonControls() is required on Windows XP if an application
      // manifest specifies use of ComCtl32.dll version 6 or later to enable
      // visual styles.  Otherwise, any window creation will fail.
      InitCommonControls();

      CWinApp::InitInstance();
   }*/

   // Initialize OLE libraries
   /*if (!AfxOleInit())
   {
      AfxMessageBox(IDP_OLE_INIT_FAILED);
      return FALSE;
   }
   
   AfxEnableControlContainer();
   CFilePath::setDefaultDelimiterSlash(false);

   // Backward compatibility registry update.
   // Propagate settings from old Router Solutions registry area to new Mentor Graphics area.
   CBackwardCompatRegistry().PropagateRegistry(HKEY_LOCAL_MACHINE, "software\\Router Solutions");
   CBackwardCompatRegistry().PropagateRegistry(HKEY_CURRENT_USER,  "software\\Router Solutions");

   // Product, Command Line, and Registry
   SetRegistryKey(REGISTRY_COMPANY);

   CString product = GetProductKey();
   if (product.IsEmpty())
   {
      //updateProductRegistry();
      product = GetProfileString("Settings", "Product");
   }

   // Default product setting
   Product = PRODUCT_PROFESSIONAL;
   //product = "PROFESSIONAL" ;
   // Override default from setting found in registry (if recognized setting)
   if (product.IsEmpty())
      WriteProfileString("Settings", "Product", "");
   else if (product.CompareNoCase("PROFESSIONAL") == 0)
      Product = PRODUCT_PROFESSIONAL;
   else if (product.CompareNoCase("PCBTRANSLATOR") == 0 || product.CompareNoCase("PCB_TRANSLATOR") == 0)
      Product = PRODUCT_PCB_TRANSLATOR;
   else if (product.CompareNoCase("VISION") == 0)
      Product = PRODUCT_VISION;
   else if (product.CompareNoCase("GRAPHIC") == 0)
      Product = PRODUCT_GRAPHIC;

   // Parse command line for standard shell commands, DDE, file open
  
   // If command line gave us an Import/Export job then run silently.
   //if (m_readWriteJob.IsActivated() && !m_txpJob.IsActivated())
   //{
      // Spread the message: Be QUIET!
      this->SilentRunning = TRUE;
      // Turn off popup messages
      this->setUseDialogsFlag(false);
   //}

	if (CopyToTestFixLocation)
	{
		CopyCAMCADToLocation();
		return FALSE;
	}

	free((void*)m_pszProfileName);

#ifdef SHAREWARE // Shareware or Professional in Frame Title
      m_pszProfileName = _tcsdup("CAMCAD Shareware");
#else
      switch (Product)
      {
         case PRODUCT_PROFESSIONAL:
            m_pszProfileName = _tcsdup("CAMCAD Professional");
         break;
         case PRODUCT_PCB_TRANSLATOR:
            m_pszProfileName = _tcsdup("CAMCAD PCB Translator");
         break;
         case PRODUCT_VISION:
            m_pszProfileName = _tcsdup("CAMCAD Vision");
         break;
         default:
            m_pszProfileName = _tcsdup("CAMCAD Graphic");
         break;
      }
#endif
                 
   if (NoUI == -1)
      NoUI = GetProfileInt("Settings", "NoUI", 0);
*/
   
   //else if (!this->m_txpJob.IsActivated()) // update registry only if not in txp mode
      //WriteProfileString("Settings", "Directory", "");

   

   // Check to see if launched as OLE server
   /*if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
   {
      // Application was run with /Embedding or /Automation.

      // Using these flags allow for checking if CAMCAD was started through automation
      // (added due to problem running in WIN98)
      UsingAutomation = TRUE;
   }

   if (MostlyHideCamcad || SilentRunning || ViewMode == SW_HIDE || ViewMode == SW_SHOWMINIMIZED ||
       ViewMode == SW_MINIMIZE || ViewMode == SW_SHOWMINNOACTIVE || ViewMode == SW_SHOWNA ||
       ViewMode == SW_FORCEMINIMIZE)
   {
      CSplashWnd::EnableSplashScreen(FALSE);
   }
   else
	{
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	}*/

   //LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

   //m_mruFileManager.Add(ID_MY_RECENT_CC_FILE1 ,_T("RecentCcFiles") ,_T("File%d"),CcFileRecognizerFunction ,CcMruFileCount );
   //m_mruFileManager.Add(ID_MY_RECENT_CAD_FILE1,_T("RecentCadFiles"),_T("File%d"),CadFileRecognizerFunction,CadMruFileCount);
   
   // Register the application's document templates.  Document templates
   //  serve as the connection between documents, frame windows and views.



   /*if (NoUI)
   {
      pDocTemplate->m_hMenuShared = NULL;
      pDocTemplate->m_hAccelTable = NULL;
   }     */

   // Connect the COleTemplateServer to the document template.
   //  The COleTemplateServer creates new documents on behalf
   //  of requesting OLE containers by using information
   //  specified in the document template.
//   m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

   // Register all OLE server factories as running.  This enables the
   //  OLE libraries to create objects from other applications.
   //COleTemplateServer::RegisterAll();
      // Note: MDI applications register all server objects without regard
      //  to the /Embedding or /Automation on the command line.


   

   //CheckWin32(); // needs to be before AlreadyRunning

/* if (AlreadyRunning())
   {
      if ((ErrorMessage("A Version of CAMCAD is already running.\nAre you sure you want to start another CAMCAD?", 
            "CAMCAD already running!", MB_YESNO | MB_DEFBUTTON2) != IDYES))
         return FALSE;
   }*/

   /*RegisterWindow(this);

   // TreeListCtrl registration
   CTreeListFrame::RegisterClass();

   // Enable drag/drop open
   m_pMainWnd->DragAcceptFiles();

   EnableShellOpen();
   RegisterShellFileTypes(false);
   //*rcf BUG this no longer works, need replacement: _AfxSetRegKey(".ccp", "CAMCAD.Document");
   //*rcf BUG this no longer works, need replacement: _AfxSetRegKey(".ccz", "CAMCAD.Document");

   //*rcf BUG Tried this as replacement, it does not work either.
#ifdef NICE_TRY
   LPCTSTR lpszKey = ".ccp";
   LPCTSTR lpszValue = "CAMCAD.Document";
		if (AfxRegSetValue(HKEY_CLASSES_ROOT, lpszKey, REG_SZ,
			  lpszValue, lstrlen(lpszValue) * sizeof(TCHAR)) != ERROR_SUCCESS)
		{
			TRACE(traceAppMsg, 0, _T("Warning: registration database update failed for key '%s'.\n"),
				lpszKey);
			return FALSE;
		}
#endif

	// Manually update the interface registries for proxy stubs
	updateInterfaceRegistry("{F8DE4B06-FBC2-11D1-BA40-0080ADB36DBB}", "ICAMCAD");
	updateInterfaceRegistry("{C3DB3A2F-0B71-11D2-BA40-0080ADB36DBB}", "IAPI");
	updateInterfaceRegistry("{DAD9B785-A34F-4786-8111-9E6B9DDA6DC1}", "IDFM");

	// When a server application is launched stand-alone, it is a good idea
   //  to update the system registry in case it has been damaged.
//   m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
   COleObjectFactory::UpdateRegistryAll();
   if (OnlyRegister)
   {
      m_pMainWnd->SendMessage(WM_QUIT);      
   }

   // Make sure all flags are set correctly (exp. UsingAutomation set from API's constructor)
   FlushQueue();

   if (OnlyRegister)
   {
      m_pMainWnd->PostMessage(WM_QUIT);      
   }

   // The main window has been initialized, so show and update it.
   if (!OnlyRegister && !SilentRunning)
      showMainWindow();

   if (hideCAMCADDlg)
   {
      hideCAMCADDlg->m_message = "Checking out licenses...";
      hideCAMCADDlg->UpdateData(FALSE);
   }

   // From here on down error messages may be generated.
   // We want popups silenced and messages directed to log file if running command line import/exxport.
   // TXP mode (aka vPlan Test Session) uses the readWriteJob to get input, but should not
   // get the silent treatment, so if in TXP mode then don't do this.
   if (m_readWriteJob.IsActivated() && !m_txpJob.IsActivated())
   {
      // Spread the message: Be QUIET!
      this->SilentRunning = TRUE;

      // // Turn off popup messages
      this->setUseDialogsFlag(false);

      // In this mode all error messages are routed to log file, so open the log file.
      this->OpenCamcadLogFile();
   }*/

   

   /*if (this->LicenseTimerEnabled)
   {
      GetLicenseTimer().SetEnabled(true);

      // If monitoring license timing, need to keep process on one CPU, else
      // the times stamps come out nuts. But we don't want to limit multi-CPU usage
      // for normal operation.
      DWORD prevAffinity = SetThreadAffinityMask(GetCurrentThread(), (ULONG)1);
   }
   
   if (CheckoutLics && !UsingAutomation)
	{
      
      if (this->LicenseTimerEnabled)
         GetLicenseTimer().DumpToFile( GetLogfilePath("CCLicenseTimerLog.txt") );

		CString errMsg;
		CString num_days;
		bool doNotShowAgain = (GetProfileInt("Settings", "MaintenanceDialog", 0) != 0);
	}

   if (hideCAMCADDlg)
   {
      hideCAMCADDlg->m_message.Empty();
      hideCAMCADDlg->UpdateLicInfo();
      hideCAMCADDlg->UpdateData(FALSE);
   }

   SetCAMCADTitle();*/

   //InitTrace();

   // set buildingblock smallest value to 1e-6 <== see float <> double problem
   //bbs_put_world_size((1.0E5));  // worldtol is 10 resulting will be -6

   // initialize settings
   
   //NotePadProgram = "notepad";      // loaded in default.set
   //LogReaderProgram = "notepad";    // loaded in default.set

   //CString settingsFile = getApp().getSystemSettingsFilePath("default.set");
   //GlSettings.LoadSettings(settingsFile);

   // Load schematic settings
   //schSettings.LoadSchSettings();

   // Load default font file
   //CString fontFile = getApp().getSystemSettingsFilePath("default.fnt");
   //CFontList::initFontList(fontFile);

   // CCE Encrypted Data Access, Init support utility.
   // Initialize the XML, XSLT, and XML Sec subsystems.
   if ( !CInfrasecUtility::InitializeXMLSec() )
   {
      int jj = 0;  // Init failed !
   }

   // If TXP Job is activated then disable use of CAMCAD Database (DataDoctor style MDB file).
   if (this->m_txpJob.IsActivated())
   {
      CDBInterface::SetConnectionAttemptAllowed(false);
   }

   // We are letting this stand for the moment. That is, if cc file or cad file
   // is on command line in "unescorted" parameter, it will be taken as a file to
   // open/import. This perhaps ought to be disabled (an else) in case of
   // readWriteJob isActivated. But it could actually be useful to load a cc and
   // then import another and then write the whole thing to another ccz.
   // That is not a "supported feature" and we are not advertising it, have not
   // tested it yet, but at this point we are not disallowing it either.

   // create a new (empty) document
   if (cmdInfo.m_strFileName.IsEmpty())
      OnFileNew();
   else
      if (!OpenDocumentFile(cmdInfo.m_strFileName))
         OnFileNew();

   // Auto-import-cad-export-cc(z) mode is considered active if any one of the
   // three relevant parameters is set.
   if (m_readWriteJob.IsActivated())
   {
      if (!m_txpJob.IsActivated())
      {
         // Classic Command Line Import/Export Job
         if (this->getCamcadLicense().hasBaseLicense())
         {
            // Do the job
            m_readWriteJob.PerformImportAndExport(); // Miten -- call the import export function
         }
         else
         {
            // No base license
            this->SetExitCode(ExitCodeNoBaseLicense);
         }

         // That is the full job, send a quit message
         m_pMainWnd->PostMessage(WM_QUIT);

         // That should be it for the log, go ahead and close it now.
         this->CloseCamcadLogFile();

         // Already done, don't need the rest of InitInstance
         return TRUE;
      }
      else
      {
         // TXP aka vPlan Test Session - uses import half of read/write
         // job to read the input. We do this because it supports more formats on
         // command line than are supported by only giving the filename and relying
         // on the format recognizer to figure out what to do. In particular, ODB++ is
         // the expected input and ODB++ is not supported as regular command line
         // startup cad file.

         //*rcf bug I think this branch is dead, confirm, delete excess code if not needed, it is confusing.

         this->m_readWriteJob.PerformImportOnly();
      }
   }
   else
   {
		 m_pMainWnd->PostMessage(WM_QUIT);
   }

   if (this->m_txpJob.IsActivated())
   {
      // If the input file type is CCZ then the CCZ file will have been on the command line and loaded in
      // the normal fashion for CCZ files. If the input file type is something else, then borrow
      // the read/write job to do the import.
      if (m_txpJob.GetRawInputFileType() != fileTypeCamcadData)
      {
         this->m_readWriteJob.SetSourceFilename( m_txpJob.GetResolvedSourceFilePath() );
         this->m_readWriteJob.SetInputFileType( m_txpJob.GetRawInputFileType() );
         this->m_readWriteJob.PerformImportOnly();
      }
      else
      {
         // Was a CCZ file load. Compensate for possible lack of WDIR and DESTFILENAME
         // params on command line. These are used to build desitination path for Save
         // and such. So extract them from the loaded file.
         CCAMCADDoc *activeDoc = getActiveDocument();
         if (activeDoc != NULL)
         {
            CString title(activeDoc->GetTitle());
            //CString ccfile(activeDoc->getCcPath());  this is apparantly not being set up, it is blank.

            CFilePath ccfilePath(title, false);
            CString wdir(ccfilePath.getDirectoryPath());
            CString filename(ccfilePath.getFileName());

            m_txpJob.SetWorkingDir(wdir);
            m_txpJob.SetDestinationFilename(filename);
         }

      }

      // Set TXP flag on the active doc, either the txp ccz we loaded earlier
      // or the ODB++ job ccz we just imported/created.
      CCAMCADDoc *activeDoc = getActiveDocument();
      if (activeDoc != NULL)
         activeDoc->IsTxpTestSessionDoc(true);
   }

   // If file "subclass.xml" exists in working directory then load it.
   // Content ends up saved as Tables in CCZ, so this has to be after CCZ has been loaded, and
   // that includes loading the ODB++ for a TXP job.
   // Use working dir from WDIR command line param.
   if (!this->m_txpJob.GetWorkingDir().IsEmpty())   // checked in txpjob even when txpjob is not "activated"
   {
      CFilePath subclassXmlFilePath = this->m_txpJob.GetWorkingDir();
      subclassXmlFilePath.setFileName( "subclass.xml" );
      CString filestr = subclassXmlFilePath.getPath();

      if (fileExists(filestr))
      {
         CSubclassList subclassList;
         subclassList.LoadFromXML(filestr);
         
         CCAMCADDoc *doc = getActiveView()->GetDocument();
         if (doc != NULL)
         {
            CGTabTableList &ccDocTableList = doc->getCamCadData().getTableList();
            subclassList.SaveToTables(ccDocTableList);
         }
      }
   }


   // load foriegn language menu file
   CString menufile = this->getSystemSettingsFilePath("menu.txt");
   pMainFrame->LoadMenuText(menufile);

   // Set Product Help Text
// SetProductHelpMenuItem();

   ((CCAMCADView*)activeView)->OnRlSmall();

   if (NoUI)
      pMainFrame->SetMenu(NULL);

#ifdef SHAREWARE
   CFile file;
   CFileStatus status;
   CString filename;

   filename = (CString)m_pszExeName + ".exe";
   _chdir(this->getCamcadExeFolderPath());
   file.GetStatus(filename, status);
   _chdir(getUserPath());

   CTime fileTime(status.m_mtime);
   CTime currentTime = CTime::GetCurrentTime();
   CTimeSpan span = currentTime - fileTime;

   if (span.GetDays() > 90)
      ErrorMessage("Visit http://www.camcad.com to download an update.", "CAMCAD Shareware out of date");
#endif

   // world view
   worldView = new WorldView;
   worldView->Create(IDD_WORLD_VIEW, AfxGetMainWnd());
   LoadWorldViewState();

   ShowTipAtStartup();

   CSplashWnd::SetClosable();
   RegisterInstanceInROT();

   return TRUE;
}

static IUnknown *punk = NULL; // IUnknown of automation object

void CCAMCADApp::RegisterInstanceInROT()
{
   if( punk != NULL )
   {
      return;
   }

   CLSID clsid;
   HRESULT hr( CLSIDFromProgID(L"CAMCAD.Application", &clsid) );
   if( FAILED(hr) )
   {
      return;
   }

   // Create an instance of the automation object
   LPCLASSFACTORY pClassFactory( NULL );
   hr = CoGetClassObject( clsid, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&pClassFactory );
   if( FAILED(hr) )
   {
      return;
   }
   ASSERT(pClassFactory != NULL);

   IID IIID_API = { 0xc3db3a2f, 0xb71, 0x11d2, { 0xba, 0x40, 0x0, 0x80, 0xad, 0xb3, 0x6d, 0xbb } };
   hr = pClassFactory->CreateInstance( NULL, IIID_API, (void**) &punk );
   pClassFactory->Release();


   // extended registration in ROT ensures that all objects are uniquely named    
   if( SUCCEEDED(hr) && punk != NULL )
   {
      LPRUNNINGOBJECTTABLE pROT = NULL;

      HRESULT hRes = GetRunningObjectTable(0, &pROT);
      if ( hRes == S_OK && pROT != NULL )
      {                                                                       
         CString sMonikerName;
         sMonikerName.Format("%s:%d", "CAMCAD.Application", GetCurrentProcessId());

         // create a moniker for ROT
         USES_CONVERSION;

         hRes = CreateItemMoniker(T2OLE("!"), T2OLE(sMonikerName.GetBuffer(39)), &m_pMoniker);
         if( hRes == S_OK )
         {
            // register application object in ROT
            // m_pMoniker and m_dwRegisterMoniker need to be store, they are needed to unregister application from ROT
            hRes = pROT->Register(0, punk, m_pMoniker, &m_dwRegisterMoniker);
            if( hRes != S_OK && hRes != MK_S_MONIKERALREADYREGISTERED )
            {                             
               m_pMoniker->Release();                          
               m_pMoniker = NULL;
               m_dwRegisterMoniker = NULL;
            }
            else
            {
               // Set date/time for new object in ROT
               FILETIME FileTime;
               CoFileTimeNow(&FileTime);
               pROT->NoteChangeTime(m_dwRegisterMoniker, &FileTime);
            }
         }
         pROT->Release();
      }
   }
}


/******************************************************************************
* CCAMCADApp::ExitInstance
*/
int CCAMCADApp::ExitInstance() 
{
   // Save schematic settings
   UnRegisterInstanceFromROT();
   schSettings.SaveSchSettings();

   CFontList::empty();

   if (editDlg)
   {
      editDlg->DestroyWindow();
      delete editDlg;
   }

   delete [] licenses;
   //free(licenses);

   //delete GlSettings.Fileextensions.Gerber;
   //delete GlSettings.Fileextensions.HPGL;
   //delete GlSettings.Fileextensions.IGES;
   //delete GlSettings.Fileextensions.APERTURE;

   // Devin -- Why is this here ??????? (and not where it belongs)
   FreeTrace();

   int defaultExitCode = CWinApp::ExitInstance();

   int finalAnswer = (this->GetExitCode() != ExitCodeSuccess) ? this->GetExitCode() : defaultExitCode;

   return finalAnswer;
}

void CCAMCADApp::UnRegisterInstanceFromROT()
{
   if( m_dwRegisterMoniker != 0 )
   {                 
      LPRUNNINGOBJECTTABLE pROT = NULL;
      HRESULT hRes = GetRunningObjectTable(0, &pROT);
      if ( hRes == S_OK && pROT != NULL )
      {
         // remove moniker
         hRes = pROT->Revoke(m_dwRegisterMoniker);
         if ( hRes == S_OK )
         {                             
            m_pMoniker->Release();
            m_pMoniker = NULL;                              
            m_dwRegisterMoniker = 0;
         }
         pROT->Release();                    
      }                 
   }
}

/////////////////////////////////////////////////////////////////////////////
// CCAMCADApp commands
BOOL CCAMCADApp::PreTranslateMessage(MSG* pMsg)
{
   CSplashWnd::PreTranslateAppMessage(pMsg);

   if (pMsg->message == WM_MOUSEMOVE)
   {
      if (mouseView)
      {
         if (pMsg->hwnd != mouseView->m_hWnd)
         {
            mouseView->EraseSearchCursor();
            mouseView = NULL;
         }
      }
   }

   return CWinApp::PreTranslateMessage(pMsg);
}

/******************************************************************************
* CheckWin32
*/
BOOL CheckWin32()
{
   OSVERSIONINFO ver;

   MAX_COORD = MAX_16BIT;

   ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   if (GetVersionEx(&ver))
   {
      switch (ver.dwPlatformId)
      {
      case VER_PLATFORM_WIN32s:
         Platform = WIN32S;
         break;
      case VER_PLATFORM_WIN32_WINDOWS:
         Platform = WIN9x;
         break;
      case VER_PLATFORM_WIN32_NT:
         Platform = WINNT;
         MAX_COORD = MAX_32BIT;
         break;
      }

//CString platformString;
//platformString.Format("%d.%d.%d", ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber);
//ErrorMessage(ver.szCSDVersion, platformString);

   }
   return TRUE;
}

/******************************************************************************
* CCAMCADApp::OnSettingsLoadGlobal
*/
void CCAMCADApp::OnSettingsLoadGlobal() 
{
   CFileDialog FileDialog(TRUE, "SET", "*.set",
         OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
         "CAMCAD Settings (*.set)|*.set|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   
   GlSettings.LoadSettings(FileDialog.GetPathName());
}

/******************************************************************************
* CCAMCADApp::OnLoadDataFile
*/
void CCAMCADApp::OnLoadDataFile() 
{
   CString defExt("cc");
   CString extOptions( this->GetDecryptionAllowed() ? "*.cc;*.ccz;*.cce" : "*.cc;*.ccz" );
   CString prompt;
   prompt.Format("CAMCAD Data File (%s)|%s|All Files (*.*)|*.*||", extOptions, extOptions);
   // "CAMCAD Data File (*.cc;*.ccz)|*.cc;*.ccz|All Files (*.*)|*.*||"

   CFileDialog FileDialog(TRUE,
      defExt, extOptions, 
      OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
      prompt, NULL);

   if (FileDialog.DoModal() != IDOK) return;

   CAMCAD_File = FileDialog.GetPathName();
	CString path = CAMCAD_File.Left(CAMCAD_File.GetLength() - FileDialog.GetFileName().GetLength());

   if (!getSchematicLinkController().requestDeleteOfSchematicLink())
   {
      return;
   }

   LoadingDataFile = TRUE;
   OnFileNew();
   LoadingDataFile = FALSE;

   CCAMCADView *view = getActiveView();
   if (view)
   {

      CCAMCADDoc *doc = view->GetDocument();
      if (!doc || !doc->isFileLoaded())
      {
         doc->OnCloseDocument();
         return;
      }

      doc->SetTitle(CAMCAD_File);
      doc->docTitle = CAMCAD_File;
      doc->GenerateSmdComponentTechnologyAttribs(NULL,false);

      AddToRecentFileList(CAMCAD_File);

      CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
      frame->getNavigator().setDoc(doc);
      doc->SetProjectPathByFirstFile(path);
   }
}

/******************************************************************************
* OnLoadProjectFile
*/
void CCAMCADApp::OnLoadProjectFile() 
{
   CFileDialog FileDialog(TRUE, "CCP", "*.CCP",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "CAMCAD Project File (*.CCP)|*.CCP|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   CAMCAD_File = FileDialog.GetPathName();

   if (!getSchematicLinkController().requestDeleteOfSchematicLink())
   {
      return;
   }

   LoadingProjectFile = TRUE;
   OnFileNew();
   LoadingProjectFile = FALSE;

   CCAMCADView *view = getActiveView();
   if (view)
   {
      CCAMCADDoc *doc = view->GetDocument();
      doc->SetTitle(CAMCAD_File);
      doc->docTitle = CAMCAD_File;
		doc->GenerateSmdComponentTechnologyAttribs(NULL,false);
   }
	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
	if (view)
   {
		frame->getNavigator().setDoc(view->GetDocument());
   }
}

void CCAMCADApp::OnFileNew()
{ CWinApp::OnFileNew(); }

void CCAMCADApp::OnAppExit()
{  CWinApp::OnAppExit(); }

/////////////////////////////////////////////////////////////////////////////

CTXPJob::CTXPJob()
: m_inputFiletype(fileTypeCamcadData)  // Default is CCZ, may be set to ODB++ on command line.
, m_outputFiletype(fileTypeUnknown)    // Default is CCZ, no other option at present but ready should new options come down the road.
, m_activated(false)
{
}

//--------------------------------------------------------------------------

CString CTXPJob::GetResolvedDestinationFilePath()
{
   // Resolve destination file name.
   // If output is cc/ccz then ensure extension is cc or ccz.
   // If other format then leave it as-is.

   // First combine working dir and output filename (which may have additional relative folder names prpefixed).
   CFilePath resolvedPath( this->GetWorkingDir(), false );  // false = beautify off, to stop changes to upper/lower case, leave name as-is.
   resolvedPath.setFileName( this->GetDestinationFilename() );

   // Now ensure CCZ extension if CCZ file is to be output.
   CString resolvedName( resolvedPath.getPath() );

   if (this->GetRawOutputFileType() == fileTypeUnknown ||
      this->GetRawOutputFileType() == fileTypeCamcadData)
   {
      CFilePath path( resolvedName, false );
      CString extension = path.getExtension();

      if (extension.CompareNoCase("cc") == 0 || extension.CompareNoCase("ccz") == 0)
      {
         // Okay as-is
      }
      else
      {
         // File has other extension or no extension.
         // Default to ccz.
         // Leave what ever is there as-is, append new extension.

         // Add dot if it does not already end with dot
         if (resolvedName.Right(1).Compare(".") != 0)
            resolvedName += ".";

         // Add the ccz
         resolvedName += "ccz";
      }
   }

   return resolvedName;
}


//--------------------------------------------------------------------------

CString CTXPJob::GetResolvedSourceFilePath()
{
   // Resolve source file path.
   // Combine working dir and filename.
   // Note that filename may contain additional folders that are relative to working dir.

   CFilePath resolvedPath( this->GetWorkingDir() );
   resolvedPath.setFileName( this->GetSourceFilename() );

   return resolvedPath.getPath();
}

/////////////////////////////////////////////////////////////////////////////

CReadWriteJob::CReadWriteJob()
: m_inputFiletype(fileTypeUnknown)  // required to be set in command line param
, m_outputFiletype(fileTypeUnknown) // optional in command line, unknown defaults to ouput ccz
, m_activated(false)
, m_flippedPcbNameSuffix("_$$Flipped")
{
   // Not all readers available in CAMCAD are supported by this feature (by design).
   // E.g. some readers read formats that are not layout files.
   // E.g. some readers read test probe solutions that are intended to be appened to some other active CCZ file.
   // For what ever reason, some readers are exluded, see dts0100626598.
   // There are two choices to implement the limitation.
   // The first is to make a list of those that are to be excluded and allow ones not in the list through.
   // That has the future effect of automatically including support for some new reader that has not been excluded.
   // The other choice is to make list of what is accepted and exlude all others.
   // That will exclude future readers unless specifically added to supported list.
   // Which is better?  My inclination is to go with the former, so nothing extra has to be done to support a new reader.
   // BUT!  Something extra DOES have to be done... it has to be TESTED with this feature !!!
   // Since whether that will be thought of or not is not predictable, we are going with the safe route.
   // Only those tested and deemed appropriate are supported, anything new that comes along has to be
   // added to this list to become supported.

   m_supportedInputFileTypes.Add(1);  //	Autocad DXF
   m_supportedInputFileTypes.Add(2);  //	HPGL
   m_supportedInputFileTypes.Add(3);  //	Gerber
   m_supportedInputFileTypes.Add(5);  //	Unicam
   m_supportedInputFileTypes.Add(7);  //	PADS PCB
   m_supportedInputFileTypes.Add(8);  //	PDIF PCB
   m_supportedInputFileTypes.Add(10); //	Protel PCB
   m_supportedInputFileTypes.Add(12); //	Mentor Boardstation
   m_supportedInputFileTypes.Add(16); //	IGES V3
   m_supportedInputFileTypes.Add(17); //	IPC 350/356
   m_supportedInputFileTypes.Add(18); //	EDIF
   m_supportedInputFileTypes.Add(19); //	Expedition HKP ASCII
   m_supportedInputFileTypes.Add(21); //	GenCAD
   m_supportedInputFileTypes.Add(22); //	Barco
   m_supportedInputFileTypes.Add(23); //	OrCAD PCB
   m_supportedInputFileTypes.Add(25); //	Cadence Allegro
   m_supportedInputFileTypes.Add(26); //	Mentor Neutral
   m_supportedInputFileTypes.Add(29); //	Redac CADIF
   m_supportedInputFileTypes.Add(30); //	Accel EDA
   m_supportedInputFileTypes.Add(35); //	Theda
   m_supportedInputFileTypes.Add(36); //	HP EGS
   m_supportedInputFileTypes.Add(38); //	Scicards CII
   m_supportedInputFileTypes.Add(41); //	CR3000
   m_supportedInputFileTypes.Add(45); //	GenCAM
   m_supportedInputFileTypes.Add(48); //	ODB++
   m_supportedInputFileTypes.Add(52); //	Fabmaster
   m_supportedInputFileTypes.Add(57); //	DDE Supermax
   m_supportedInputFileTypes.Add(60); //	CR5000
   m_supportedInputFileTypes.Add(63); //	Calay Prisma
   m_supportedInputFileTypes.Add(64); //	Unidat
   // Don't put 106 in here, see special handling for the Schematic Netlist stuff.
   // For Schematic Netlist in here we put each specific Netlist File type.
   m_supportedInputFileTypes.Add(107); // CAMCAD Netlist
   m_supportedInputFileTypes.Add(108); // Keyin Netlist
   m_supportedInputFileTypes.Add(109); // Nets.nets Netlist
   m_supportedInputFileTypes.Add(110); // DxDesigner Netlist
   m_supportedInputFileTypes.Add(111); // PADS Logic Netlist
   // And of course we support CC/CCZ (useful for when output is not CC/CCZ, otherwise silly to import CCZ and write CCZ)
   m_supportedInputFileTypes.Add(122); // CAMCAD CC and CCZ


   // Supported Writers
   // The two originally supported formats.
   m_supportedOutputFileTypes.Add(fileTypeCamcadData);       // 122  aka CC and CCZ
   m_supportedOutputFileTypes.Add(fileTypeOdbPlusPlus);      // 48

   // And for vPlan 11.2 we have added a dozen test exports. But then it became vPlan 11.3.  
   m_supportedOutputFileTypes.Add(fileTypeAeroflexCb);       // 80
   m_supportedOutputFileTypes.Add(fileTypeHp3070);           // 28
   m_supportedOutputFileTypes.Add(fileTypeIpl);              // 73
   m_supportedOutputFileTypes.Add(fileTypeTeradyne228xCkt);  // 75
   m_supportedOutputFileTypes.Add(fileTypeTeradyne228xNav);  // 100
   m_supportedOutputFileTypes.Add(fileTypeTeradyneSpectrum); // 90
   m_supportedOutputFileTypes.Add(fileTypeAgilentI1000ATD);  // 101
   m_supportedOutputFileTypes.Add(fileTypeHiokiICT);         // 124
   m_supportedOutputFileTypes.Add(fileTypeScorpion);         // 84
   m_supportedOutputFileTypes.Add(fileTypeSeicaParNod);      // 87
   m_supportedOutputFileTypes.Add(fileTypeSpea4040);         // 72
   m_supportedOutputFileTypes.Add(fileTypeTakaya9);          // 67

   // After Jun 2012 DFT meeting in Yavne we added these.
   m_supportedOutputFileTypes.Add(fileTypeAcculogic);        // 99
   m_supportedOutputFileTypes.Add(fileTypeFixture);          // 76
   m_supportedOutputFileTypes.Add(fileTypeHuntron);          // 66
   m_supportedOutputFileTypes.Add(fileTypeKonradICT);        // 98
   m_supportedOutputFileTypes.Add(fileTypeTestronicsICT);    // 102
   m_supportedOutputFileTypes.Add(fileTypeTriMda);           // 53


   // Added for the "new definition" of vPlan 11.2.
   // DR 885673 added Unicam PDW export support.
   m_supportedOutputFileTypes.Add(fileTypeUnicam);           //  5


}

//--------------------------------------------------------------------------

bool CReadWriteJob::IsSupportedInputFileType(FileTypeTag filetype)
{
   for (int i = 0; i < m_supportedInputFileTypes.GetCount(); i++)
   {
      int candidate = m_supportedInputFileTypes.GetAt(i);
      if (candidate == (int)filetype)
         return true;
   }

   return false;
}

//--------------------------------------------------------------------------

bool CReadWriteJob::IsSupportedOutputFileType(FileTypeTag filetype)
{
   // I don't want to put this one in the supported types list, but when file type
   // is Unknown export defaults to CCZ, this is a leftover from the days when
   // CCZ was the only exported type and writer param didn't exist.
   if (filetype == fileTypeUnknown)
      return true;

   for (int i = 0; i < m_supportedOutputFileTypes.GetCount(); i++)
   {
      int candidate = m_supportedOutputFileTypes.GetAt(i);
      if (candidate == (int)filetype)
         return true;
   }

   return false;
}

//--------------------------------------------------------------------------

// A nicer solution would be to put the capability in the CamcadProduct to check
// multiple file types. But it is not there now and too much to add for now.
// So just handle it locally, right here.

bool CReadWriteJob::IsSchematicNetlistFileType(FileTypeTag filetype)
{
   if (
      filetype == 107 || // CAMCAD Netlist
      filetype == 108 || // Keyin Netlist
      filetype == 109 || // Nets.nets Netlist
      filetype == 110 || // DxDesigner Netlist
      filetype == 111    // PADS Logic Netlist
      )
   {
      return true;
   }

   return false;
}

//--------------------------------------------------------------------------

bool CReadWriteJob::InputDirectoryOnly(FileTypeTag filetype)
{
   // These readers can accept directory only as input specifier.
   // This is being checked/enforced in too many little separate code chunks.
   // (Search for "dironly" variable to see.)
   // Could use some centralization. Until that day comes, it is repeated here too.

   switch (filetype)
   {
   case fileTypeHp5dx:
   case fileTypeOdbPlusPlus:
   case fileTypeVb99Layout:
      return true;
   }

   return false;
}

//--------------------------------------------------------------------------

void CReadWriteJob::WriteUsage()
{
   // Write usage and reader symbols, aka FileTypeTag enum, to log file.

   CString commandUsed( GetCommandLine() );
   if (commandUsed.Find(" ") > -1)
      commandUsed.Truncate(commandUsed.Find(" "));

   CString usageStr;
   usageStr.Format("Usage:  %s /reader=n /input=<File or Folder Name> /output=<CC or CCZ File Name> /logfile=<logfile name>\n",
      commandUsed);

   getApp().LogMessage(usageStr);
   getApp().LogMessage("Where n for reader is one of the numbers from list below.\nExample: /reader=6\n");
   getApp().LogMessage("\n");
   getApp().LogMessage("The input file or folder and output file are absolute paths.\n");
   getApp().LogMessage("That means they should start with either the slash or the drive name.\n");
   getApp().LogMessage("File or folder names with embedded blanks must be enclosed in double quotes.\n");
   getApp().LogMessage("The input file may be a comma separated list of file names.\n");
   getApp().LogMessage("Examples: /input=c:/caddata/pads.asc\n");
   getApp().LogMessage("          /input=/caddata/pads.asc\n");
   getApp().LogMessage("          /input=\"c:/cad data/pads.asc\"\n");
   getApp().LogMessage("          /input=\"/test/Brd.txt,/test/Pad.txt,/test/Rte.txt,/test/Sym.txt\"\n");
   getApp().LogMessage("\n");
   getApp().LogMessage("The output file should be a single file, full absolute path specified.\n");
   getApp().LogMessage("If the file has extension \".cc\" then a CC file will be written.\n");
   getApp().LogMessage("Otherwise, a CCZ file is written. If the file name specified includes an\n");
   getApp().LogMessage("extension that is not \".cc\" or \".ccz\", the file name will be used as-is and\n");
   getApp().LogMessage("\".ccz\" extenstion will be added. Specifying any extension is optional.\n");
   getApp().LogMessage("Examples: /output=c:/result/myfile.ccz\n");
   getApp().LogMessage("          /output=/result/myfile\n");
   getApp().LogMessage("\n");
   getApp().LogMessage("The logfile parameter is optional. If not specified the log is written to \"camcad.log\"\n");
   getApp().LogMessage("The logfile value may be an absolute that that redirects the log file to some specific folder.\n");
   getApp().LogMessage("The logfile value may be only the log file name, in which case the log file is written to the standard log file folder.\n");
   getApp().LogMessage("If the log file specified can not be opened for writing, the standard log file \"camcad.log\" is used instead\n");
   getApp().LogMessage("and shall contain an error message about the specified log file.\n");
   getApp().LogMessage("Examples: /logfile=/myjob/mylog.txt\n");
   getApp().LogMessage("          /logfile=mylog.txt\n");
   getApp().LogMessage("\n");

   usageStr.Format("Usage:  %s /reader=n /input=<File or Folder Name> /writer=m /output=<CC or CCZ File Name> /logfile=<logfile name>\n",
      commandUsed);
   getApp().LogMessage(usageStr);
   getApp().LogMessage("When no /writer parameter is provided the output defaults to CCZ file. This is the form shown in first Usage above.\n");
   getApp().LogMessage("When /writer=m is supplied, m should be one of the writer numbers shown below.\n");
   getApp().LogMessage("\n");

   WriteReaderSymbols();
   getApp().LogMessage("\n");
   WriteWriterSymbols();
}


//--------------------------------------------------------------------------

void CReadWriteJob::WriteReaderSymbols()
{
   // Write reader symbols, aka FileTypeTag enum mapped to product names, to log file.

   // CAMCAD Readers have product numbers, but that is not the value this feature runs on.
   // This feature runs on the "file type" number, mainly because that is the number the API has always run on.
   // Hence, for consistency, we do the same.
   // But the difference it not particularly important to users, so we do not try to explain it here.
   // We just list the number and the importer product with which it is associated.

   getApp().LogMessage("Supported Readers (also known as Importers)\n");
   getApp().LogMessage("The integer values shown are used for n in the /reader=n command line parameter.\n");
   getApp().LogMessage("The associated Reader (Importer) product name is shown next to it's number.\n");
   getApp().LogMessage("\n");

   for (EnumIterator(FileTypeTag, filetypeIterator); filetypeIterator.hasNext(); )
   {
      FileTypeTag ft = filetypeIterator.getNext();

      if (IsSupportedInputFileType(ft))
      {
         FileTypeTag mappedFt = GetMappedFileType(ft);
         CString addendum;
         
         if (IsSchematicNetlistFileType(ft))
         {
            // Schematic Netlist individual file types all map to a "parent" (or "family" file type
            // for producr mapping purposes. In an addendum for the line in the logfile add
            // the specific file type's format name.
            addendum.Format("  [%s]", fileTypeTagToString(ft)); // original ft
         }

         CamcadProduct *readerprod = getApp().getCamcadLicense().getImporterProduct(mappedFt);

         if (readerprod != NULL)
         {
            CString msg;
            msg.Format("%4d  %s%s\n", ft, readerprod->getCamcadDisplayName(), addendum);
            getApp().LogMessage(msg);
         }
      }
   }

}

//--------------------------------------------------------------------------

void CReadWriteJob::WriteWriterSymbols()
{
   // Write writer symbols, aka FileTypeTag enum mapped to product names, to log file.

   // CAMCAD Writers have product numbers, but that is not the value this feature runs on.
   // This feature runs on the "file type" number, mainly because that is the number the API has always run on.
   // Hence, for consistency, we do the same.
   // But the difference it not particularly important to users, so we do not try to explain it here.
   // We just list the number and the exporter product with which it is associated.

   getApp().LogMessage("Supported Writers (also known as Exporters)\n");
   getApp().LogMessage("The integer values shown are used for n in the /writer=n command line parameter.\n");
   getApp().LogMessage("The associated Writer (Exporter) product name is shown next to it's number.\n");
   getApp().LogMessage("\n");

   for (EnumIterator(FileTypeTag, filetypeIterator); filetypeIterator.hasNext(); )
   {
      FileTypeTag ft = filetypeIterator.getNext();

      if (IsSupportedOutputFileType(ft))
      {
         FileTypeTag mappedFt = GetMappedFileType(ft);
         CString addendum;
         
         if (IsSchematicNetlistFileType(ft))
         {
            // Schematic Netlist individual file types all map to a "parent" (or "family" file type
            // for producer mapping purposes. In an addendum for the line in the logfile add
            // the specific file type's format name.
            addendum.Format("  [%s]", fileTypeTagToString(ft)); // original ft
         }

         CamcadProduct *writerprod = getApp().getCamcadLicense().getExporterProduct(mappedFt);

         if (writerprod != NULL)
         {
            CString msg;
            msg.Format("%4d  %s%s\n", ft, writerprod->getCamcadDisplayName(), addendum);
            getApp().LogMessage(msg);
         }
      }
   }

}

//--------------------------------------------------------------------------

bool CReadWriteJob::ImportCadFiles()  
{
   //*rcf Pending - use expanded file list if /input=folder
	// Miten -- The start of import function
   FileTypeTag mappedFileType = GetMappedFileType(GetRawInputFileType());

   if (mappedFileType == fileTypeCamcadData)
   {
      // Read cc/ccz file if specified in /input, otherwise assume it was specified as
      // ordinary ccz file on command line.
      if (!GetSourceFilename().IsEmpty())
      {
         // Not clear whether OpenCadFile or OpenDocumentFile is better  here.
         getApp().OpenDocumentFile(GetSourceFilename());
      }
      // else Should check that a CCZ was already loaded...
   }
   else
   {
      // Import foreign CAD format
      API api;
      int status = api.Import(mappedFileType, GetResolvedSourceFilename(), "" /*LPCTSTR formatString*/);

      if (status == RC_NO_LICENSE)
      {
         getApp().LogMessage("No license for CAD Reader.\n");  // reader, importer, this feature tends to use reader instead of importer
         getApp().UpdateExitCode(ExitCodeNoImporterLicense);
         return false; // Import utterly failed
      }
      else if (status != RC_SUCCESS)
      {
         if (this->InputDirectoryOnly(GetMappedFileType(GetRawInputFileType())))
         {
            getApp().LogMessage("This reader accepts directories only in /input parameter, not individual files.\n");
         }
         getApp().LogMessage("Error(s) encountered during import.\n");
         ////can't do this yet:   getApp().UpdateExitCode(ExitCodeGeneralError);
         ////because vplan has no gray area, only catastrophic failure or success, no "success but with issues".
         ////if camcad returns non-zero exit code then vplan will not import the result.

         ////the following, on the other hand, is just within camcad, and remains correct
         // But don't make it fatal, don't stop ccz write with return false.
         // Let ccz write go ahead and try, might be useful partial result for user, might be minor errors (hopefully reported in log file)
      }
      else
      {
         // Apparant success from CAD import. Load any auxiliary data files we find present.
         // If file "DftSolution.xml" exists in working directory then load it.
         if (!this->GetWorkingDir().IsEmpty())
         {
            CFilePath dftXmlFilePath = this->GetWorkingDir();
            dftXmlFilePath.setFileName( "DftSolution.xml" ); // Possibly make this name controllable by command line param?
            CString filestr = dftXmlFilePath.getPath();

            if (fileExists(filestr))
            {
               getApp().LogMessage("\n"); // This is to put a blank line in log between what came before and DFT Solution processing.

               CCAMCADDoc *doc = getActiveView()->GetDocument();
               if (doc != NULL)
               {
                  FileStruct *dftSlnFile = doc->getFileList().addNewFile(DFT_SOLUTION_XML_FILESTRUCT_NAME, fileTypeUnknown);
                  dftSlnFile->setShow(false); // Don't want this one visible when export is all done, hide it now.

                  bool setCurrentSolution = false;
                  doc->LoadDFTSolutionFile(filestr, *dftSlnFile, setCurrentSolution);

                  // Maybe this ought to be a CAMCADDoc method?
                  this->ApplyDftSolution(doc, dftSlnFile);
               }
            }
         }
      }
   }

   return true;
}

//--------------------------------------------------------------------------

void CReadWriteJob::ApplyDftSolution(CCAMCADDoc *doc, FileStruct *dftSlnFile)
{
   // Apply the solution.

   // First apply incoming DFT Solutions to original PCBs.

   // Create flipped PCB for flipped DFT Solution.
   // We process the panel, creating the flipped PCBs and change the pcb inserts
   // that are mirrored to insert the flipped pcb instead. We do not unmirror or 
   // unflip anything. It is just that the flipped and non-flipped
   // dft solutions can be different.
   AdjustFlippedPCBs(doc, dftSlnFile);

   CDFTSolutionList &dftSlnList = doc->getDFTSolutions(*dftSlnFile);
   POSITION dftSlnPos = dftSlnList.GetHeadPosition();
   while (dftSlnPos != NULL)
   {
      CDFTSolution *dftSln = dftSlnList.GetNext(dftSlnPos);
      CString pcaName( dftSln->GetName() );

      if (dftSln->IsFlipped())
         pcaName += m_flippedPcbNameSuffix;

      if (!pcaName.IsEmpty())
      {
         FileStruct *pcaFile = doc->getFileList().FindByName(pcaName);
         if (pcaFile == NULL)
         {
            // Error
            CString msg;
            msg.Format("PCB [%s] named in DFT Solution not found in CAD data.\n", pcaName);
            getApp().LogMessage(msg);
         }
         else
         {
            // Okay. Apply AA and PP solutions.
            CAccessAnalysisSolution *aaSln = dftSln->GetAccessAnalysisSolution();
            if (aaSln != NULL)
            {
               aaSln->ApplyToPcb(pcaFile);
            }

            CProbePlacementSolution *ppSln = dftSln->GetProbePlacementSolution();
            if (ppSln != NULL)
            {
               ppSln->ApplyToPcb(pcaFile);
            }
         }
      }
   }

   // Create the SuperBoard.
   // For now assume up to one panel, just process the first panel we find.

   FileStruct *panelFile = doc->getFileList().GetFirst(blockTypePanel);
   if (panelFile != NULL)
   {
      CSuperBoard superBoard(*doc, panelFile);
      FileStruct *superBoardFile = superBoard.GetSuperBoard();

      // Make the superboard the only visible file. This is the one
      // we want exporters to find and export. First make all not shown
      // then set superboard to shown. And fit page size so if CCZ is
      // opened it looks nice.
      doc->getFileList().SetShowAll(false);
      superBoardFile->setShow(true);
      doc->OnFitPageToImage();

      // Convey probe offset error through app return code.
      if (superBoard.HasProbeOffsetError())
      {
         getApp().SetExitCode(ExitCodeProbeOffsetError);

         CString msg;
         msg  = "\nProbe Number Offset Error\n";
         msg += "Probe Number Offset has three basic modes, two of these are special purpose, one is a specific offset.\n";
         msg += "Offset of 0 means to use the probe numbers in the base PCA Probe Placement Solutions unchanged.\n";
         msg += "Offset of 1 means to increment sequentially from the previously processed PCA image to the next PCA image.\n";
         msg += "Offset of n (greater than 1) means to directly apply this offset to PCA images as they are processed.\n";
         msg += "In this last case, the value of the offset must be greater than or equal to the highest probe number in all base PCAs within the panel.\n";
         CString msg2;
         msg2.Format("Highest probe number in base PCAs: %d.\nProbe Number Offset setting: %d.\n",
            superBoard.GetHighestOriginalProbeNumber(), superBoard.GetUserProbeNumberOffset());

         getApp().LogMessage(msg + msg2);
      }
   }
}

//--------------------------------------------------------------------------

void CReadWriteJob::AdjustFlippedPCBs(CCAMCADDoc *doc, FileStruct *dftSlnFile)
{
   // Make a separete PCB <File> and adjust bottom side inserts of these
   // PCBs. A "normal" and and a "flipped" Dft Solution for same PCB name
   // can be entirely different, so we need separate PCB files to hold the
   // AA and PP solutions.

   // For now assume up to one panel, just process the first panel we find.
   // If there is no panel (might be just a PCA job) then nothing to do here, just leave.
   FileStruct *panelFile = doc->getFileList().GetFirst(blockTypePanel);
   if (panelFile == NULL)
      return;

   BlockStruct *panelBlock = panelFile->getBlock();
   if (panelBlock != NULL)
   {
      POSITION pos = panelBlock->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *data = panelBlock->getNextDataInsert(pos);
         if (data != NULL && data->isInsertType(insertTypePcb))
         {
            InsertStruct *insert = data->getInsert();
            if (insert->getPlacedBottom())
            {
               // PCB Inserted on Bottom, check for flipped Dft Solution.
               // First we need the <File> name for the inserted PCB block. The block's own
               // name might match, but can't be depended on. It is the file's name that matters.
               BlockStruct *insertedPcbBlock = doc->getBlockAt(insert->getBlockNumber());
               if (insertedPcbBlock == NULL)
               {
                  // That shouldn't happen.
                  CString msg;
                  msg.Format("Inserted block [%d] for PCB [%s] not found.\n", insert->getBlockNumber(), insert->getRefname());
                  getApp().LogMessage(msg);
               }
               else
               {
                  int pcbFileNumber = insertedPcbBlock->getFileNumber();
                  FileStruct *normalPcbFile = doc->getFileList().FindByFileNumber(pcbFileNumber);
                  if (normalPcbFile == NULL)
                  {
                     // That shouldn't happen.
                     CString msg;
                     msg.Format("File [%d] for inserted block [%d] for PCB [%s] not found.\n", pcbFileNumber, insert->getBlockNumber(), insert->getRefname());
                     getApp().LogMessage(msg);
                  }
                  else
                  {
                     // OKAY... so now we have the <file> of the inserted PCB. Get the name, find out
                     // if there is a flipped Dft Solution for it.
                     CString normalPcbFileName( normalPcbFile->getName() );

                     CDFTSolutionList &dftSlnList = doc->getDFTSolutions(*dftSlnFile);
                     CDFTSolution *dftSln = dftSlnList.FindDFTSolution(normalPcbFileName, true/*isFlipped*/);
                     if (dftSln != NULL)
                     {
                        // Has flipped solution.

                        CString flippedPcbFileName = normalPcbFileName + m_flippedPcbNameSuffix;
                        FileStruct *flippedPcbFile = doc->getFileList().FindByName(flippedPcbFileName);
                        if (flippedPcbFile != NULL)
                        {
                           // Already exists.
                        }
                        else
                        {
                           // Does not exist, make it.
                           CCamCadData &camcadData = doc->getCamCadData();

                           flippedPcbFile = camcadData.getFileList().addNewFile(flippedPcbFileName, fileTypeCamcadUser);
                           flippedPcbFile->setBlockType(blockTypePcb);
                           flippedPcbFile->setShow(false);

                           // Save refs to original non-flipped PCB.
                           flippedPcbFile->setOriginalBlock(insertedPcbBlock);
                           flippedPcbFile->getBlock()->setOriginalName(insertedPcbBlock->getName());

                           // Make new one a copy of the old one. We do not really flip anything here.
                           CopyFileContents(normalPcbFile, flippedPcbFile, doc);
                        }

                        if (flippedPcbFile == NULL || flippedPcbFile->getBlock() == NULL)
                        {
                           int jj = 0; // should not happen now.
                        }
                        else
                        {
                           // Change the block number for this insert to the flipped PCB.
                           // Note that the PCB itself is not different, what may be different is
                           // the Dft Solution, later.
                           int flippedPcbFileBlockNumber = flippedPcbFile->getBlock()->getBlockNumber();
                           insert->setBlockNumber(flippedPcbFileBlockNumber);
                        }
                     }
                     else
                     {
                        // No flipped solution. Do nothing.
                     }

                  }
               }
            }
         }
      }
   }

}

//--------------------------------------------------------------------------

short CReadWriteJob::ExportCadFiles()
{
   if (this->GetRawOutputFileType() == fileTypeUnknown ||
      this->GetRawOutputFileType() == fileTypeCamcadData)
   {
      return ExportCamcadFile();
   }

   if (this->IsSupportedOutputFileType(this->GetRawOutputFileType()))
   {
      API api;
	  return api.Export(this->GetRawOutputFileType(), GetResolvedDestinationFilename()); // Miten -- file exporting function is called here
   }

   return RC_GENERAL_ERROR;
}

//--------------------------------------------------------------------------

short CReadWriteJob::ExportCamcadFile()
{
   // Save to cc if file extension is specifically cc, otherwise save to ccz

   API api;
   short res;

   CFilePath outputFilePath( GetResolvedDestinationFilename() );
   CString extension = outputFilePath.getExtension();

   if (extension.CompareNoCase("CC") == 0)
      res = api.SaveDataFileAs( GetResolvedDestinationFilename() );
   else
      res = api.SaveCompressedDataFileAs( GetResolvedDestinationFilename() );
 
   return res;
}

//--------------------------------------------------------------------------

void CReadWriteJob::PerformImportAndExport()
{
   // Do not use isLicensed() check here. If we do, then we have to check both styles of
   // base license as well as CPCMD license, as this feature should run if any of these
   // are licensed. That seems like too much license thrashing, since licenses were already
   // thrashed once upon startup. During initial license checked the "is allowed" field is
   // set, to enable this feature for each license that should allow access.
   // 
   // So take a shortcut, just check if feature "is allowed"
   //

   /*CamcadProduct* camcadProduct = getApp().getCamcadLicense().findCamcadProduct(camcadProductCommandLineCPCMD);
   if (camcadProduct == NULL || !camcadProduct->getAllowed()) // if can't find or is not allowed
   {
      if (camcadProduct == NULL)
         getApp().LogMessage("Command line import/export product not found.\n");
      else
         getApp().LogMessage("Command line import/export not licensed.\n");

      return;
   }
   else
   {
      // Supported check here means we have a product we allow to be used, does not
      // mean it is licensed for particular user to use, that is tested elswhere.*/
      if (!this->IsSupportedInputFileType(GetRawInputFileType()) || !this->IsSupportedOutputFileType(GetRawOutputFileType()))
      {
         getApp().LogMessage("Error in command line parameter for /reader or /writer.\n\n");
         WriteUsage();
         getApp().UpdateExitCode(ExitCodeGeneralError);
      }
      else
      {
         // Write parameters to log file
         CString msg;

		 // Miten - We get the input file name and type at this point
         msg.Format("Reader=(%d)[%s]\n", GetRawInputFileType(), fileTypeTagToString(GetRawInputFileType()));
         getApp().LogMessage(msg);

         CString resolvedSource( GetSourceFilename() );  // Starts with basic name as given by user
         bool isDirectory = false;

         // Determine if resolved input is a directory
         CFileFind finder;
         BOOL found = finder.FindFile( resolvedSource );
         if (found)
         {
            BOOL more = finder.FindNextFile();
            isDirectory = finder.IsDirectory()?true:false;
         }

         // Echo parameter to log file, add designator note if it is a directory
         msg.Format("\nInput=(%s)", GetSourceFilename());
         if (isDirectory)
            msg += " [Directory]";
         msg += "\n";
         getApp().LogMessage(msg);

         bool requiresDirectory = InputDirectoryOnly(GetMappedFileType(GetRawInputFileType()));
         bool proceedWithOperation = true;

         if (requiresDirectory && !isDirectory)
         {
            // Importer requires a directory and name is not a directory, report an error
            getApp().LogMessage("\nThis reader requires a directory path for /input value.\nImport aborted.\n");
            proceedWithOperation = false;
         }
         else if (isDirectory && !requiresDirectory)
         {
            // User named a directory, but importer does not require a directory. Such importers (usually) want
            // to import each file individually. So resolve the input named to be all files in the directory.
            resolvedSource = GetResolvedSourceFilename();
            // This turns comma separate list into column with [ and ] start and end chars
            resolvedSource.Replace(",", "]\n[");
            msg.Format("[Files being imported ...]\n[%s]\n", resolvedSource);
            getApp().LogMessage(msg);
         }

         // We still write the output param, so it can be inspected in log file, even if operation is
         // already known to be aborted.
		 // Miten - We get the output file name and type at this point
         if (GetRawOutputFileType() != fileTypeUnknown)
         {
            msg.Format("\nWriter=(%d)[%s]\n", GetRawOutputFileType(), fileTypeTagToString(GetRawOutputFileType()));
            getApp().LogMessage(msg);
         }
         msg.Format("\nOutput=(%s)", GetDestinationFilename());
         if (GetDestinationFilename().Compare(GetResolvedDestinationFilename()) != 0)
            msg += " [" + GetResolvedDestinationFilename() + "]";
         msg += "\n\n";
         getApp().LogMessage(msg);

         // Import cad file(s) then export cc/ccz
		 // Miten -- first perform the import and on success perform the export
		 // Miten -- CCE reader would be form this point
         if (proceedWithOperation && ImportCadFiles())
         {
            // Put a separator in the log, to separate Import from Export records
            getApp().LogMessage("\n-------------------------------------------------------------------\n\n");

            if (ExportCadFiles() != RC_SUCCESS)
            {
               CString msg;
               msg.Format("CAD Export failed.\n\n");
               getApp().LogMessage(msg);
               //// probably don't want this fatal error sent until gray area
               //// error is supported in vplan.
               /////getApp().UpdateExitCode(ExitCodeGeneralError);
            }
         }
         else
         {
            CString fileTypeStr("CC/CCZ"); // CC/CCZ is the default output type
            if (this->GetRawOutputFileType() != fileTypeUnknown)
            {
               // Output file type was specified on command line, update the msg string accordingly.
               fileTypeStr = fileTypeTagToString(this->GetRawOutputFileType());
            }
            CString msg;
            msg.Format("CAD Import failed, %s file creation has been aborted.\n\n", fileTypeStr);
            getApp().LogMessage(msg);
            getApp().UpdateExitCode(ExitCodeGeneralError);
         }
      }
   //}
}

//--------------------------------------------------------------------------

CString CReadWriteJob::GetResolvedDestinationFilename()
{
   // Resolve destination file name.
   // If output is cc/ccz then ensure extension is cc or ccz.
   // If other format then leave it as-is.

   CString resolvedName( GetDestinationFilename() );

   // If file type is not set to other than unknown, then output defaults to ccz.
   if (this->GetRawOutputFileType() == fileTypeUnknown)
   {
      CFilePath path( resolvedName );
      CString extension = path.getExtension();

      if (extension.CompareNoCase("cc") == 0 || extension.CompareNoCase("ccz") == 0)
      {
         // Okay as-is
      }
      else
      {
         // File has other extension or no extension.
         // Default to ccz.
         // Leave what ever is there as-is, append new extension.

         // Add dot if it does not already end with dot
         if (resolvedName.Right(1).Compare(".") != 0)
            resolvedName += ".";

         // Add the ccz
         resolvedName += "ccz";
      }
   }

   return resolvedName;
}


//--------------------------------------------------------------------------

CString CReadWriteJob::GetResolvedSourceFilename()
{
   // Resolve source file name.
   // If user specified a directory then resolved name is comma separated list of dir contents.

   CString resolvedName( GetSourceFilename() );

   // Nuke trailing folder delimiters
   while (resolvedName.Right(1) == "\\" || resolvedName.Right(1) == "/")
      resolvedName.Truncate(resolvedName.GetLength()-1);

   CFileFind finder;
   BOOL found = finder.FindFile( resolvedName );

   // Resolve directory to individual files unless reader accepts Dirs only as input
   if (found && !InputDirectoryOnly(GetMappedFileType(GetRawInputFileType())))
   {
      BOOL more = finder.FindNextFile();

      if (finder.IsDirectory())
      {
         resolvedName.Empty();  // Clear/Reset result
         CString fileDirectory = finder.GetFilePath() + "\\";  // Save full dir path
         BOOL moreFiles = finder.FindFile(fileDirectory + "*.*");  // Find all files in dir

         while (moreFiles)
         {
            moreFiles = finder.FindNextFile();
           CString nextFileName = finder.GetFileName();

           if (finder.IsDirectory())
           {
              // Skip, not supporting recursive drill-down of folders in folders
           }
           else if (finder.IsDots())
           {
              // Skip self (.) and parent (..)
           }
           else
           {
              // Looks like a keeper
              CString fullFilePath( fileDirectory + nextFileName );
              if (!resolvedName.IsEmpty())
                 resolvedName += ",";
              resolvedName += fullFilePath;
           }
         }
      }
      // else assume okay as-is
   }

   return resolvedName;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

// Dialog Data
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   CString  m_User;
   CString  m_Company;
   CString  m_SerialNumber;
   CString  m_product;
   CString  m_version;
   CString  m_flowId;
	CString m_hostIDString;
	CString m_serialNumberString;
	CString m_NewExpirationString;
   //}}AFX_DATA
   CBitmapButton bitmap;

// Implementation
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //{{AFX_MSG(CAboutDlg)
   afx_msg void OnLicenses();
   afx_msg void OnHelp();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked9();
	CStaticLabel m_rsiCompanyName;
	afx_msg void OnStnClickedRsiCompanyName();
private:
	CStaticLabel m_integrationTools;
   CStaticLabel m_companyWebSite;
   CStaticLabel m_supportWebSite;
	
public:
	afx_msg void OnStnClickedVersion();
   afx_msg void OnStnClickedCompanyWebSite();
   afx_msg void OnStnClickedSupportWebSite();

private:
	CString m_warningMsg;
public:
	CString m_hostOrDongleLabel;
	CString m_hostOrDongleID;
	CString m_ssaExpirationDate;
	afx_msg void OnStnClickedToolsDescription();
	CEdit m_editHostID;
	CEdit m_SerialNumberText;
	CEdit m_NewExpiration;
	afx_msg void OnStnClickedWarningMsg();
	afx_msg void OnBnClickedBitmap();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_warningMsg(_T(""))
, m_hostOrDongleLabel(_T(""))
, m_hostOrDongleID(_T(""))
, m_ssaExpirationDate(_T(""))
{
   //{{AFX_DATA_INIT(CAboutDlg)
   m_User = "";
   m_Company = "";
   m_SerialNumber = "";
   m_product = _T("");
   m_version = _T("");
   m_flowId = _T("");
   //}}AFX_DATA_INIT

   m_warningMsg  = "Warning: This computer program is protected by U.S. copyright law";
   m_warningMsg += " and international treaties. Unauthorized reproduction or";
   m_warningMsg += " distribution of this program, or any portion of it, may result";
   m_warningMsg += " in severe civil and criminal penalties, and will be prosecuted";
   m_warningMsg += " to the maximum extent possible under the law. The use of this";
   m_warningMsg += " software is subject to the terms of the license agreement";
   m_warningMsg += " included with it and agreed to during the installation process.";

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_USER, m_User);
	DDX_Text(pDX, IDC_COMPANY, m_Company);
	//DDX_Text(pDX, IDC_SERIAL_NUMBER, m_SerialNumber);
	DDX_Text(pDX, IDC_PRODUCT, m_product);
	DDX_Text(pDX, IDC_VERSION, m_version);
   DDX_Text(pDX, IDC_FLOWID, m_flowId);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RSI_COMPANY_NAME, m_rsiCompanyName);
	DDX_Control(pDX, IDC_TOOLS_DESCRIPTION, m_integrationTools);
   DDX_Control(pDX, IDC_COMPANY_WEB_SITE, m_companyWebSite);
   DDX_Control(pDX, IDC_SUPPORT_WEB_SITE, m_supportWebSite);
	DDX_Text(pDX, IDC_WARNING_MSG, m_warningMsg);
	DDX_Text(pDX, IDC_HOST_OR_DONGLE_LABEL, m_hostOrDongleLabel);
	//DDX_Text(pDX, IDC_HOST_OR_DONGLE_ID, m_hostOrDongleID);
	//DDX_Text(pDX, IDC_SSA_EXPIRATION_DATE, m_ssaExpirationDate);
	DDX_Control(pDX, IDC_dongleID, m_editHostID);
	DDX_Control(pDX, IDC_dongleID2, m_SerialNumberText);
	DDX_Control(pDX, IDC_dongleID3, m_NewExpiration);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
   //{{AFX_MSG_MAP(CAboutDlg)
   ON_BN_CLICKED(IDC_LICENSES, OnLicenses)
   ON_BN_CLICKED(IDHELP, OnHelp)
   //}}AFX_MSG_MAP
	ON_BN_CLICKED(9, OnBnClicked9)
	ON_STN_CLICKED(IDC_RSI_COMPANY_NAME, OnStnClickedRsiCompanyName)
	ON_STN_CLICKED(IDC_VERSION, OnStnClickedVersion)
   ON_STN_CLICKED(IDC_COMPANY_WEB_SITE, OnStnClickedCompanyWebSite)
   ON_STN_CLICKED(IDC_SUPPORT_WEB_SITE, OnStnClickedSupportWebSite)
	ON_STN_CLICKED(IDC_TOOLS_DESCRIPTION, OnStnClickedToolsDescription)
	ON_STN_CLICKED(IDC_WARNING_MSG, OnStnClickedWarningMsg)
	ON_BN_CLICKED(IDC_BITMAP, OnBnClickedBitmap)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
#ifdef SHAREWARE // Shareware or Professional in Frame Title
   m_product = "Shareware";
#else
   switch (Product)
   {
      case PRODUCT_GRAPHIC:
         m_product = "CAMCAD Graphic";
      break;
      case PRODUCT_PROFESSIONAL:
         m_product = "CAMCAD Professional";
      break;
      case PRODUCT_PCB_TRANSLATOR:
         m_product = "CAMCAD PCB Translator";
      break;
      case PRODUCT_VISION:
         m_product = "CAMCAD VISION";
      break;
   }
#endif
   m_version = "Version: " + getApp().getVersionString();

   m_flowId = getApp().getFlowIdString();

   m_rsiCompanyName.SetFontSize(22);
   m_rsiCompanyName.SetFontBold(true);

   m_integrationTools.SetFontSize(17);
   m_integrationTools.SetFontBold(true);

   m_companyWebSite.SetFontUnderline(true);
   m_companyWebSite.SetTextColor(colorBlue);

   m_supportWebSite.SetFontUnderline(true);
   m_supportWebSite.SetTextColor(colorBlue);

	m_editHostID.SetWindowText (m_hostIDString);
	m_SerialNumberText.SetWindowText(m_serialNumberString);
	m_NewExpiration.SetWindowText(m_NewExpirationString);

   bitmap.AutoLoad(IDC_BITMAP, this);
   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnLicenses() 
{
   CString _user;
   CString _company;
   CString _serial;

   LicenseDialog dlg;
   dlg.m_User = getApp().getCamcadLicense().getUser();
   dlg.m_Company = getApp().getCamcadLicense().getCompany();;
   dlg.m_dongle = getApp().getCamcadLicense().getUseDongle()?TRUE:FALSE;
   dlg.DoModal();
   
   // USER NAME
   if (!getApp().getCamcadLicense().getUser().IsEmpty())
      _user = getApp().getCamcadLicense().getUser();
   else 
      _user = "Unregistered CAMCAD";

   // COMPANY NAME
   if (!getApp().getCamcadLicense().getCompany().IsEmpty())
      _company = getApp().getCamcadLicense().getCompany();
   else 
      _company = "";



   // SERIAL NUMBER
   if (!getApp().getCamcadLicense().getSerialNubmer().IsEmpty())
      _serial = getApp().getCamcadLicense().getSerialNubmer();
   else 
      _serial = "";

   m_User = _user;
   m_Company = _company;
   m_SerialNumber = _serial;


   UpdateData(FALSE);
}

void CAboutDlg::OnHelp() 
{ 	
	getApp().OnHelpIndex();
}

///////////////////////////////////////////////////////////////////////////////
/*void CCAMCADApp::OnAppAbout()
{
   CCAMCADApp &app = getApp();
   CamcadLicense& camcadLicense = getApp().getCamcadLicense();

   CAboutDlg dlg;
   dlg.m_User = camcadLicense.getUser();
   dlg.m_Company = camcadLicense.getCompany();
	dlg.m_serialNumberString = camcadLicense.getSerialNubmer();
	
   if (!camcadLicense.getUseDongle())
	   dlg.m_hostOrDongleLabel = "Host ID:";
   else
	   dlg.m_hostOrDongleLabel = "Dongle ID:";

	dlg.m_hostIDString = camcadLicense.getHostId();
	dlg.m_NewExpirationString = camcadLicense.getExpirationDateString();

   dlg.DoModal();
}*/

typedef int (APIENTRY FUNC)(int, int);
/******************************************************************************
* CCAMCADApp::OnHelpIndex
*/
void CCAMCADApp::OnHelpIndex() 
{
   // Mentor standard Online Help 

   /* perform help call:  "Help > Open CAMCAD user guide" */
   int status = OlhHelpCall( "camcad_useref", "CAMCAD User's Guide and Reference Manual", OLH_DISPLAY_NORMAL );
   /* check for errors */
   if ( status != OLH_OK ) 
   {	
      // These message details are based on comments in header file olhapi.h 

      CString detail;
      switch (status)
      {
      case OLH_FAILED:           detail = "Operation could not be performed."; break;
      case OLH_INVALID_PARAM:    detail = "Invalid parameter.";                break;
      case OLH_DOC_NOT_FOUND:    detail = "Document not found.";               break;
      case OLH_OPEN_FAIL:        detail = "Unable to open document.";          break;
      case OLH_TAG_NOT_FOUND:    detail = "Tag not in document.";              break;
      case OLH_VIEWER_NOT_FOUND: detail = "Viewer not found.";                 break;
      case OLH_TEXT_NOT_FOUND:   detail = "Text not found in document.";       break;
      case OLH_INVALID_PAGE:     detail = "Page does not exist in document.";  break;
      }

      ErrorMessage( detail, "Help call failed!");
   }
}

/******************************************************************************
* CCAMCADApp::OnHelpInfoHub
*/
void CCAMCADApp::OnHelpInfoHub() 
{
   // Mentor standard Online Help 

   /* invoke the infohub:  "Help > Help & Manuals" */
   int status = OlhInfoHub( "camcad_ih", "doc" );
   /* check for errors */
   if ( status != OLH_OK ) 
   {	
      // These message details are based on comments in header file olhapi.h 

      CString detail;
      switch (status)
      {
      case OLH_FAILED:           detail = "Operation could not be performed."; break;
      case OLH_INVALID_PARAM:    detail = "Invalid parameter.";                break;
      case OLH_DOC_NOT_FOUND:    detail = "Document not found.";               break;
      case OLH_OPEN_FAIL:        detail = "Unable to open document.";          break;
      case OLH_TAG_NOT_FOUND:    detail = "Tag not in document.";              break;
      case OLH_VIEWER_NOT_FOUND: detail = "Viewer not found.";                 break;
      case OLH_TEXT_NOT_FOUND:   detail = "Text not found in document.";       break;
      case OLH_INVALID_PAGE:     detail = "Page does not exist in document.";  break;
      }

      ErrorMessage( detail, "InfoHub call failed!");
   }
}
/******************************************************************************
* CCAMCADApp::OnOnlineHelp
*/
void CCAMCADApp::OnOnlineHelp()
{
   ShellExecute(NULL, "open", "http://help.camcad.com", NULL, NULL, SW_SHOWNORMAL); 
}

/******************************************************************************
* CCAMCADApp::OnCamcadCom
*/
void CCAMCADApp::OnCamcadCom() 
{
   ShellExecute(NULL, "open", "http://www.camcad.com", NULL, NULL, SW_SHOWNORMAL); 
}

/******************************************************************************
* CCAMCADApp::OnRsiCom
*/
void CCAMCADApp::OnRsiCom() 
{
   ShellExecute(NULL, "open", "http://www.mentor.com", NULL, NULL, SW_SHOWNORMAL); 
}

//******************************************************************************

CString CMyCommandLineInfo::ParseParameterValue(CString cmdLineParam)
{
   CString val;

   int indx = cmdLineParam.Find("=");

   if (indx != -1)
   {
      val = cmdLineParam.Mid(indx+1);
   }

   return val;
}

// To check if both input and output parameters are valid
BOOL CMyCommandLineInfo::validParameters()
{
	if (validInput && validOutput)
		return true;
	else
	{
		PrintErrorMessgeToLogFile();
		return false;
	}
}

// Error message to be printed to the log file
void CMyCommandLineInfo::PrintErrorMessgeToLogFile()
{
	//CString log = "\\log.txt";
	CCAMCADApp *pApp = (CCAMCADApp*)AfxGetApp();
	//pApp->SetLogFileName(log);
	pApp->LogMessage("\n\nERROR !!!!Command line input should be in the format\n\n");
	pApp->LogMessage("input = < CC/CCZ/CCE filename > output = < Ouput Directory >\n");
	pApp->LogMessage("eg : input = C:\\CCE\\Example.cce output = C:\\OutputDirectory\n");
}
//******************************************************************************
void CMyCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
   // to run camcad in flavored mode :
   // camcad /xxxx
   // this will diplay the correct title and select the right license.
   // this means our license generator needs a different license for 
   // PADS PCB read for ANSOFT and PROFESSIONAL.

   // Get the appliction
   CCAMCADApp *pApp = (CCAMCADApp*)AfxGetApp();

   Product = PRODUCT_PROFESSIONAL;
   this->m_bShowSplash = false;

   
   //else if (!STRNICMP(lpszParam, "INPUT", 5))
   if (!STRNICMP(lpszParam, "INPUT", 5))
   {
      pApp->m_readWriteJob.SetActivated();
      CString val(this->ParseParameterValue(lpszParam));
      pApp->m_readWriteJob.SetSourceFilename(val);

	  FileTypeTag filetype = intToFileTypeTag(122);
      pApp->m_readWriteJob.SetInputFileType(filetype);


	  //CString val;  // Starts with basic name as given by user
      bool isDirectory = false;

      // Determine if resolved input is a directory
      CFileFind finder;
      BOOL found = finder.FindFile( val ); // Checking whether file exists

	  // Checking for valid extension of the file
	  int index = val.ReverseFind('.');
	  CString fileExtension;
	  if (index != -1)
		  fileExtension = val.Mid(index+1);
	  BOOL isValidExtension = false;
	  if ( ( fileExtension == "cce" ) || ( fileExtension == "ccz" ) || ( fileExtension == "cc" ) )
		  isValidExtension = true;

	  // Checking if the input param value is a directory
	  isDirectory = finder.IsDirectory()?true:false;

	  //Check if the input param
	  // 1) Exists 2) Is not a Directory 3) Has a valid Extension
	  if ( found && !isDirectory && isValidExtension )
      {
		  validInput = true;
      }
   }
   else if (!STRNICMP(lpszParam, "OUTPUT", 6))
   {
      pApp->m_readWriteJob.SetActivated();
      CString val(this->ParseParameterValue(lpszParam));
      pApp->m_readWriteJob.SetDestinationFilename(val);

	  pApp->m_txpJob.SetWorkingDir(val);
      pApp->m_readWriteJob.SetWorkingDir(val);

	  FileTypeTag filetype = intToFileTypeTag(48);
      pApp->m_readWriteJob.SetOutputFileType(filetype);

	  pApp->SetDecryptionAllowed(true);

	  if ( ! (val.IsEmpty()))
	  {
		  CString log = val;
		  log += "\\log.txt";
		  pApp->SetLogFileName(log);
		  validOutput = true;
	  }
   }
   else
   {
	   PrintErrorMessgeToLogFile();
   }

   // figure out which product (Graphic is default)
   // only one product, first parameter
   // Remove
   /*if (!STRCMPI(lpszParam, "GRAPHIC"))
   {
      Product = PRODUCT_GRAPHIC;
   }
   else if (!STRCMPI(lpszParam, "PROFESSIONAL"))
   {
      Product = PRODUCT_PROFESSIONAL;
   }
   else if (!STRCMPI(lpszParam, "PCB_TRANSLATOR"))
   {
      Product = PRODUCT_PCB_TRANSLATOR;
   }
   else if (!STRCMPI(lpszParam, "VISION"))
   {
      Product = PRODUCT_VISION;
   }
   else if (!STRCMPI(lpszParam, "NoUI") || !STRCMPI(lpszParam, "NoGUI"))   // wolf always gets it wrong GUI and UI
   {
      NoUI = TRUE;
   }
   else if (!STRCMPI(lpszParam, "UI") || !STRCMPI(lpszParam, "GUI"))
   {
      NoUI = FALSE;
   }
   else if (!STRCMPI(lpszParam, "NOLIC"))
   {
      pApp->CheckoutLics = FALSE;
   }
   else if (!STRCMPI(lpszParam, "LICTIME"))
   {
      pApp->LicenseTimerEnabled = TRUE;
   }
   else if (!STRCMPI(lpszParam, "HIDE"))
   {
      pApp->MostlyHideCamcad = TRUE;
   }
   else if (!STRNICMP(lpszParam, "LOGFILE", 7))
   {
      CString val(this->ParseParameterValue(lpszParam));
      pApp->SetLogFileName(val); // does not matter if it is empty string
   }
   else if (!STRNICMP(lpszParam, "READER", 6))
   {
      pApp->m_readWriteJob.SetActivated();
      CString val(this->ParseParameterValue(lpszParam));
      int ival = atoi(val);
      FileTypeTag filetype = intToFileTypeTag(ival);
      pApp->m_readWriteJob.SetInputFileType(filetype);
   }
   else if (!STRNICMP(lpszParam, "WRITER", 6))
   {
      pApp->m_readWriteJob.SetActivated();
      CString val(this->ParseParameterValue(lpszParam));
      int ival = atoi(val);
      FileTypeTag filetype = intToFileTypeTag(ival);
      pApp->m_readWriteJob.SetOutputFileType(filetype);
   }*/
   // Remove
   /*else if (!STRNICMP(lpszParam, "IMSETTINGS", 10))
   {
      CString val(this->ParseParameterValue(lpszParam));
      pApp->setCmdLineImportSettingsFilePath(val);
   }
   else if (!STRNICMP(lpszParam, "EXSETTINGS", 10))
   {
      CString val(this->ParseParameterValue(lpszParam));
      pApp->setCmdLineExportSettingsFilePath(val);
   }
   else if (!STRNICMP(lpszParam, "WDIR", 4))
   {
      CString val(this->ParseParameterValue(lpszParam));
      pApp->m_txpJob.SetWorkingDir(val);
      pApp->m_readWriteJob.SetWorkingDir(val);
   }
   else if (!STRNICMP(lpszParam, "TXPODB", 6))
   {
      CString val(this->ParseParameterValue(lpszParam));
      pApp->m_txpJob.SetSourceFilename(val);
      pApp->m_txpJob.SetInputFileType(fileTypeOdbPlusPlus);
   }
   else if (!STRNICMP(lpszParam, "TXPCCZ", 6))
   {
      CString val(this->ParseParameterValue(lpszParam));
      pApp->m_txpJob.SetDestinationFilename(val);
   }
   else if (!STRNICMP(lpszParam, "TXP", 3))  // DANGER! Make sure this test stays after longer params that start with TXP
   {
      pApp->m_txpJob.SetActivated();
   }
   else if (!STRNICMP(lpszParam, "REGISTERONLY", 5))
   {
      pApp->OnlyRegister = pApp->MostlyHideCamcad = TRUE;
   }
   else if (!STRNICMP(lpszParam, "{D959A3FC-B6D6-4c91-B1F9-B58A6DDA5D74}", 38))
   {
      pApp->MostlyHideCamcad = pApp->CopyToTestFixLocation = TRUE;
   }
   else if (!STRNICMP(lpszParam, "NoSplash",8))
   {
      this->m_bShowSplash = false;
   }
   else if (!STRNICMP(lpszParam, "VIEWMODE=", 9))
   {
      CString cmdMode = lpszParam;
      cmdMode = cmdMode.Mid(9);

      for (int i=0; i<cmdMode.GetLength(); i++)
      {
         if (!isdigit(cmdMode[i]))
            return;
      }

      if (cmdMode.IsEmpty())
         return;

      pApp->ViewMode = atoi(cmdMode);

      if (pApp->ViewMode < 0 || pApp->ViewMode > SW_MAX)
         pApp->ViewMode = SW_SHOWMAXIMIZED;
   }
   else if (!STRNICMP(lpszParam, "FILE:", 5))
   {
      CString filename = lpszParam;
      m_strFileName = filename.Mid(5);
      CFileFind findFile;

      if (!findFile.FindFile(m_strFileName))
      {
         CString buf;
         buf.Format("Cannot open file: %s [File not found]", m_strFileName);
         AfxMessageBox(buf);
         m_strFileName.Empty();
      }
   }
   else CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);*/
}

CDocument* CCAMCADApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
   CAMCAD_File = lpszFileName;

   CString ext = CAMCAD_File.Right(3);

   if (!ext.CompareNoCase("ccp"))
   {
      LoadingProjectFile = TRUE;
      CCAMCADDoc *doc = (CCAMCADDoc*) CWinApp::OpenDocumentFile(lpszFileName);
      LoadingProjectFile = FALSE;

      if (doc)
      {
         doc->SetTitle(lpszFileName);
         doc->docTitle = lpszFileName;
      }
      return doc;
   }

   if (OpenCadFile(lpszFileName))
      return NULL;

   CCAMCADView *view = getActiveView();
   if (!view)
      return NULL;

   CCAMCADDoc *doc = view->GetDocument();
   if (doc)
   {
      doc->SetTitle(lpszFileName);
      doc->docTitle = lpszFileName;
   }
   return doc;
}

int CCAMCADApp::CheckAndSetSchematiclink(CString &errMsg)
{
   CCAMCADDoc *pcbDoc = NULL, *schDoc = NULL;
   int compResult = 0, netResult = 0;

   { // Get the two and only two documents (one schematic and one PCB)
      int docCnt = 0;
      POSITION docPos = pDocTemplate->GetFirstDocPosition();
      while (docPos)
      {
         CCAMCADDoc *tempDoc = (CCAMCADDoc*)pDocTemplate->GetNextDoc(docPos);
         POSITION filePos = tempDoc->getFileList().GetHeadPosition();
         while (filePos)
         {
            FileStruct *file = tempDoc->getFileList().GetNext(filePos);
            if (!pcbDoc && file->isShown() && file->getBlockType() == BLOCKTYPE_PCB)
            {
               pcbDoc = tempDoc;
               break;   // go to the next document
            }

            if (!schDoc && file->isShown() && file->getBlockType() == BLOCKTYPE_SHEET)
            {
               schDoc = tempDoc;
               break;
            }
         }

         // Count the number of documents open
         docCnt++;
      }

      // Only 2 documents can be open for schematic linking
      if (docCnt != 2 || !pcbDoc || !schDoc)
      {
         errMsg = "Only 2 documents (a PCB and schematic) can and must be open when using schematic linking.";
         return -1;
      }

   }

   { // Tile the windows with the schematic on the left side
      POSITION viewPos = schDoc->GetFirstViewPosition();
      if (viewPos)
      {
         CView *view = schDoc->GetNextView(viewPos);
         view->GetParentFrame()->ActivateFrame();
      }
      ((CMDIFrameWnd*)m_pMainWnd)->MDITile(MDITILE_VERTICAL);
   }

   schLink.ClearAllData();
   if (schLink.SetDocuments(pcbDoc, schDoc) < 0)
   {
      errMsg = schLink.GetLastError();
      return -1;
   }

   return 0;
}

void CCAMCADApp::OnToolsSchematiclink()
{
   getSchematicLinkController().toggerSchematicLink();
   return;
}

void CCAMCADApp::OnLinkNets()
{
   getSchematicLinkController().setNetCrossProbeOn(!getSchematicLinkController().getNetCrossProbeOn());
}

void CCAMCADApp::OnLinkComps()
{
   getSchematicLinkController().setComponentCrossProbeOn(!getSchematicLinkController().getComponentCrossProbeOn());
}

void CCAMCADApp::OnSchematicSettings()
{
   CSchLinkSettings dlg;
   dlg.m_schMargin.Format("%i", schSettings.SchMargin);
   dlg.m_pcbMargin.Format("%i", schSettings.PcbMargin);
   dlg.m_schHighlightColorBtn.tempColor = schSettings.SchHightlightColor;
   dlg.m_schBusHighlightColorBtn.tempColor = schSettings.SchBusHightlightColor;
   dlg.m_pcbHighlightColorBtn.tempColor = schSettings.PcbHightlightColor;
   
   if (dlg.DoModal() == IDCANCEL)
      return;

   schSettings.SchMargin = atoi(dlg.m_schMargin);
   schSettings.PcbMargin = atoi(dlg.m_pcbMargin);
   schSettings.SchHightlightColor = dlg.m_schHighlightColorBtn.tempColor;
   schSettings.SchBusHightlightColor = dlg.m_schBusHighlightColorBtn.tempColor;
   schSettings.PcbHightlightColor = dlg.m_pcbHighlightColorBtn.tempColor;
}

void CCAMCADApp::OnUpdateLinkNets(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(getSchematicLinkController().getNetCrossProbeOn());
   pCmdUI->SetCheck(getSchematicLinkController().getNetCrossProbeOn());
}

void CCAMCADApp::OnUpdateLinkComps(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(getSchematicLinkController().getComponentCrossProbeOn());
   pCmdUI->SetCheck(getSchematicLinkController().getComponentCrossProbeOn());
}

void CCAMCADApp::OnUpdateSchematicSettings(CCmdUI *pCmdUI)
{
   //pCmdUI->SetCheck(FALSE);
   //pCmdUI->Enable(schLinkState);
}

int CCAMCADApp::RequestSchLinkClose()
{
   if (schLinkState)
   {
      CString msg = (CString)"Schematic Link is currently running.\n\n" +
         "Continuing the requested action will render cross probing\n" +
         "unusable and will be disabled.  Re-scaning of the files will\n" +
         "be required to begin Schematic Link again.\n\n" + 
         "Do you want to continue?";
      int result = ErrorMessage(msg, "Shematic Link", MB_YESNO);
      
      if (result == IDYES)
         OnToolsSchematiclink();

      return result;
   }
 
   return IDYES;
}

void CCAMCADApp::OnToolsRsiExchange()
{
#ifdef SHAREWARE
   return;
#endif

   CString  path;
   path = this->getCamcadExeFolderPath();
   path += "../DataExchange/DataExchange.exe";
	runTool("Data Exchange", "RSI_Exchange", path, " NoSplash");
}

void CCAMCADApp::OnToolsBomExplorer()
{
#ifdef SHAREWARE
   return;
#endif

   CString  path;
   path = this->getCamcadExeFolderPath();
   path += "../BOMExplorer/be.exe";
	runTool("BOM Explorer", "Bom_Explorer", path, " NoSplash");
}

void CCAMCADApp::OnToolsRealpart()
{
#ifdef SHAREWARE
   return;
#endif

   CString  path;
   path = this->getCamcadExeFolderPath();
   path += "Realpart.exe";
	runTool("Realpart", "Realpart", path, "");
}

BOOL CCAMCADApp::PumpMessage()
{
	_AFX_THREAD_STATE *pState = AfxGetThreadState();

	if (!::GetMessage(&(pState->m_msgCur), NULL, NULL, NULL))
	{
#ifdef _DEBUG
		TRACE(traceAppMsg, 1, "CCAMCADApp::PumpMessage - Received WM_QUIT.\n");
			pState->m_nDisablePumpCount++; // application must die
#endif
		// Note: prevents calling message loop things in 'ExitInstance'
		// will never be decremented
		return FALSE;
	}

#ifdef _DEBUG
  if (pState->m_nDisablePumpCount != 0)
	{
	  TRACE(traceAppMsg, 0, "Error: CCAMCADApp::PumpMessage called when not permitted.\n");
	  ASSERT(FALSE);
	}
#endif

#ifdef _DEBUG
	//*rcf This no longer works, is unresolved reference: _AfxTraceMsg(_T("PumpMessage"), &(pState->m_msgCur));
#endif

  // process this message

   if (pState->m_msgCur.message != WM_KICKIDLE && !AfxPreTranslateMessage(&(pState->m_msgCur)))
   {
	   ::TranslateMessage(&(pState->m_msgCur));
	   ::DispatchMessage(&(pState->m_msgCur));
   }

   return TRUE;
}

bool CCAMCADApp::runTool(CString toolName, CString toolRegName, CString toolPath, CString commandLine)
{
	if (toolName.IsEmpty() || toolRegName.IsEmpty())
   {
      ErrorMessage("Internal Error, toolName and toolRegName must both be non-blank.", "");
		return false;
   }

	// Get the path from the registry.
   // If the registry entry is altogether absent then we will get back the toolPath value as default.
   // But if the registry entry is present but value is blank, we get back BLANK!
   // So we need two level default handling.
   // First level is to go ahead and pass the default to GetProfileString.
   // Second level is if return from GetProfileString is blank then reset again to default.
   // And after all that if it is still blank, something is wrong in the code because our default 
   // should not be blank.
	CString path = GetProfileString("Settings", toolRegName, toolPath);

   if (path.IsEmpty())  // Empty registry setting, reset to default
      path = toolPath;

	CString buf;
	buf.Format("%s Executable (*.EXE)|*.exe|All Files (*.*)|*.*||", toolName);
	CFileDialog dlg(TRUE, "exe", NULL, NULL, buf, AfxGetMainWnd());
	do
	{
		int msgRetVal = IDNO;

		if (!dlg.GetPathName().IsEmpty())
			path = dlg.GetPathName();

		int res = ERROR_FILE_NOT_FOUND;
      if (!path.IsEmpty())
         res = WinExec(path + commandLine, SW_SHOW);
		if (res > 31)
			break;

		switch (res)
		{
		case 0:
			ErrorMessage("The system is out of memory or resources!", toolName, MB_OK | MB_ICONHAND);
			break;
		case ERROR_BAD_FORMAT:
			{
				CString buf = "The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)!"; 
				ErrorMessage(buf, toolName, MB_OK | MB_ICONHAND);
			}
			break;
		case ERROR_FILE_NOT_FOUND:
			{
				buf.Format("The specified file [%s] was not found!\n\nWould you like to browse for %s?", path, toolName);
				msgRetVal = ErrorMessage(buf, toolName, MB_YESNO | MB_ICONHAND);
			}
			break;
		case ERROR_PATH_NOT_FOUND:
			{
				buf.Format("The specified path [%s] was not found!\n\nWould you like to browse for %s?", path, toolName);
				msgRetVal = ErrorMessage(buf, toolName, MB_YESNO | MB_ICONHAND);
			}
			break;
		}

		if (msgRetVal == IDNO)
			return false;

		if (dlg.DoModal() == IDCANCEL)
			return false;
	}
	while (true);

	WriteProfileString("Settings", toolRegName, path);

	return true;
}
void CAboutDlg::OnBnClicked9()
{
	

	// TODO: Add your control notification handler code here
}

void CAboutDlg::OnStnClickedRsiCompanyName()
{
	// TODO: Add your control notification handler code here
   // Have to set Notify=True in resources to make this active.
}

void CAboutDlg::OnStnClickedVersion()
{
	// TODO: Add your control notification handler code here
   // Have to set Notify=True in resources to make this active.
}

void CAboutDlg::OnStnClickedToolsDescription()
{
	// TODO: Add your control notification handler code here
   // Have to set Notify=True in resources to make this active.
}

void CAboutDlg::OnStnClickedWarningMsg()
{
	// TODO: Add your control notification handler code here
   // Have to set Notify=True in resources to make this active.
}


void CAboutDlg::OnStnClickedCompanyWebSite()
{
   // Open support web site
   ShellExecute(NULL, "open", "http://www.mentor.com/", NULL, NULL, SW_SHOWNORMAL);
}

void CAboutDlg::OnStnClickedSupportWebSite()
{
   // Open support web site
   ShellExecute(NULL, "open", "http://supportnet.mentor.com/", NULL, NULL, SW_SHOWNORMAL);
}

//------------------------------------------------------------------------------
// CProjectPath
CProjectPath::CProjectPath()
{
	SetDefault();
}

CProjectPath::~CProjectPath()
{
}

void CProjectPath::SetDefault()
{
	m_pathByFirstFile = "";
	m_pathByUserSelection = "";
	
	// 0 = Set project path by the directory of the first file loaded into CAMCAD
	// 1 = Set project path manually
	m_projectPathOption = 0;	
}

CString CProjectPath::GetPathByFirstFile() const
{
	return m_pathByFirstFile;
}

CString CProjectPath::GetPathByUserSelection() const
{
	return m_pathByUserSelection;
}

int CProjectPath::GetOption() const
{
	return m_projectPathOption;
}

void CProjectPath::SetPathByFirstFile(const CString path)
{
	m_pathByFirstFile = path;
	if (m_pathByFirstFile.Right(1) != '\\')
		m_pathByFirstFile += "\\";
}

void CProjectPath::SetPathByUserSelection(const CString path)
{
	m_pathByUserSelection = path;
	if (m_pathByUserSelection.Right(1) != '\\')
		m_pathByUserSelection += "\\";
}

void CProjectPath::SetOption(const int option)
{
	if (option < 0 || m_projectPathOption > 1)
		return;

	m_projectPathOption = option;
}

void CAboutDlg::OnBnClickedBitmap()
{
	// TODO: Add your control notification handler code here
}
