// $Header: /CAMCAD/4.6/CAMCAD.H 59    5/24/07 11:09p Rick Faltersack $

// CAMCAD.h : main header file for the CAMCAD application
//

#if !defined(__CamCad_h__)
#define __CamCad_h__

#pragma once

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "flexlm.h"
#include "sch_link.h"   // for schematic linking
#include "MruFileManager.h"
#include "SchematicLink.h"
#include "CCEtoODBlicense.h"
#include <string>

class CViewSynchronizer;

enum RunModeTag
{
   runModeNormal,
   runModeDataProtectedModelessDialog,
   runModeUndefined
};

enum CamcadExitCode
{ 
   ExitCodeSuccess            = 0, 
   ExitCodeNoBaseLicense      = 1, 
   ExitCodeNoImporterLicense  = 2,
   ExitCodeImportFailed       = 3,
   ExitCodeExportFailed       = 4,
   ExitCodeProbeOffsetError   = 5,
   ExitCodeGeneralError       = 99 
};

//_____________________________________________________________________________
class CMyMultiDocTemplate : public CMultiDocTemplate
{
public:
   CMyMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
      : CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass) {}
   virtual ~CMyMultiDocTemplate() {};

   //virtual CDocument* CreateNewDocument();
   virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
};

//-----------------------------------------------------------------------------
class CTXPJob
{
private:
   bool m_activated;
   FileTypeTag m_inputFiletype;
   FileTypeTag m_outputFiletype;

   CString m_workingDir;           // working directory full path
   CString m_sourceFilename;       // relative path name, to be appended to working dir for full path
   CString m_destinationFilename;  // relative path name, to be appended to working dir for full path

public:
   CTXPJob();

   void SetInputFileType(FileTypeTag filetype)    { m_inputFiletype = filetype; }
   FileTypeTag GetRawInputFileType()              { return m_inputFiletype; }

   void SetOutputFileType(FileTypeTag filetype)   { m_outputFiletype = filetype; }
   FileTypeTag GetRawOutputFileType()             { return m_outputFiletype; }

   void SetWorkingDir(CString name)               { m_workingDir = name; }
   CString GetWorkingDir()                        { return m_workingDir; }

   void SetSourceFilename(CString name)           { m_sourceFilename = name; }
   CString GetSourceFilename()                    { return m_sourceFilename; }

   void SetDestinationFilename(CString name)      { m_destinationFilename = name; }
   CString GetDestinationFilename()               { return m_destinationFilename; }

   void SetActivated()                            { m_activated = true; }
   bool IsActivated()                             { return m_activated; }

   CString GetResolvedDestinationFilePath();  // Combines working dir and filename
   CString GetResolvedSourceFilePath();       // Combines working dir and filename
};

//-----------------------------------------------------------------------------
class CReadWriteJob
{
private:
   bool m_activated;
   FileTypeTag m_inputFiletype;
   FileTypeTag m_outputFiletype;
   CString m_sourceFilename;
   CString m_destinationFilename;
   CString m_workingDir;
   CString m_flippedPcbNameSuffix;

   CArray<int,int> m_supportedInputFileTypes;
   CArray<int,int> m_supportedOutputFileTypes;
   bool IsSupportedInputFileType(FileTypeTag filetype);
   bool IsSupportedOutputFileType(FileTypeTag filetype);
   bool IsSchematicNetlistFileType(FileTypeTag filetype);

   bool InputDirectoryOnly(FileTypeTag filetype);

   bool ImportCadFiles();
   short ExportCadFiles();
   short ExportCamcadFile();

   void ApplyDftSolution(CCAMCADDoc *doc, FileStruct *dftSlnFile);
   void AdjustFlippedPCBs(CCAMCADDoc *doc, FileStruct *dftSlnFile);

   void WriteUsage();
   void WriteReaderSymbols();
   void WriteWriterSymbols();

   CString GetResolvedDestinationFilename();
   CString GetResolvedSourceFilename();

public:
   CReadWriteJob();

   void SetInputFileType(FileTypeTag filetype)    { m_inputFiletype = filetype; }
   FileTypeTag GetRawInputFileType()              { return m_inputFiletype; }

   void SetOutputFileType(FileTypeTag filetype)   { m_outputFiletype = filetype; }
   FileTypeTag GetRawOutputFileType()             { return m_outputFiletype; }

   FileTypeTag GetMappedFileType(FileTypeTag ft)  { return IsSchematicNetlistFileType(ft) ? fileTypeSchematicNetlist : ft; }

   void SetSourceFilename(CString name)           { m_sourceFilename = name; }
   CString GetSourceFilename()                    { return m_sourceFilename; }

   void SetDestinationFilename(CString name)      { m_destinationFilename = name; }
   CString GetDestinationFilename()               { return m_destinationFilename; }

   void SetActivated()                            { m_activated = true; }
   bool IsActivated()                             { return m_activated; }

   void PerformImportAndExport();
   void PerformImportOnly()                        { this->ImportCadFiles(); }

   void SetWorkingDir(CString name)               { m_workingDir = name; }
   CString GetWorkingDir()                        { return m_workingDir; }
};

//_____________________________________________________________________________
/////////////////////////////////////////////////////////////////////////////
// CCAMCADApp:
//
class CCAMCADApp : public CWinApp
{
public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);

   HWND m_hwndDialog;   // TOOLTIPS
   CToolTipCtrl *m_gpToolTip; // TOOLTIPS
   
   // Command Line Settings
   BOOL MostlyHideCamcad;  // For backward compatibility, original HideCamcad command line arg does this
   BOOL SilentRunning;   // Fully hide, absolutely no popups, no dialogs, no progress bars, no anything.
   BOOL CheckoutLics;
   BOOL LicenseTimerEnabled;
   int ViewMode;
   BOOL UsingAutomation;
   BOOL OnlyRegister;
   bool CopyToTestFixLocation;

   SchLink schLink;
   BOOL schLinkState;
   CSchematicSettings schSettings;

   CTXPJob m_txpJob;
   CReadWriteJob m_readWriteJob;

private:
   CMultiDocTemplate* m_docTemplate;
   bool m_useDialogsFlag;
   CStringArray m_titleSuffixes;
	CString m_userPath;
   CString m_camcadExeFolderPath;
   CString m_logfileName;
   CString m_cmdlineImportSettingsFilePath;
   CString m_cmdlineExportSettingsFilePath;
   RunModeTag m_runMode;
   CamcadExitCode m_exitCode;
   CMruFileManager m_mruFileManager;
   CMessageFilter m_messageFilterTypeMessage;
   CMessageFilter m_messageFilterTypeFormat;
   SchematicLinkController m_schematicLinkController;
   FILE *m_logFp;

   bool m_cceDecryptionAllowed;

   CamcadLicense m_camcadLicense;
   CViewSynchronizer* m_viewSynchronizer;

public:
   CCAMCADApp();
   virtual ~CCAMCADApp();

   CViewSynchronizer& getViewSynchronizer();
   void releaseViewSynchronizer();

   // m_cceDecryptionAllowed and SetDescryptionAllowed() is for support of users enabling access 
   // through API, the other is for enabling through regular product license key. Either one true is good to go.
   bool GetDecryptionAllowed()         { return (m_cceDecryptionAllowed || getCamcadLicense().isLicensed(camcadProductCpCceRead)); }
   void SetDecryptionAllowed(bool b)   { m_cceDecryptionAllowed = b; }

private:
	void CopyCAMCADToLocation();
   bool CopyCAMCADToLocation(const char* destinationDirectory);
	CString GetProductKey();
	CString GetCWDKey();
   void updateProductRegistry();
   void updateInterfaceRegistry(CString clsID, CString interfaceID);
   void showMainWindow();
   bool runTool(CString toolName, CString toolRegName, CString toolPath = "", CString commandLine = "");
   void ShowTipAtStartup(void);
   void ShowTipOfTheDay(void);
   void RegisterInstanceInROT();
   void UnRegisterInstanceFromROT();
   IMoniker* m_pMoniker;
   DWORD m_dwRegisterMoniker;

public:
   SchematicLinkController& getSchematicLinkController() { return m_schematicLinkController; }

   CamcadLicense& getCamcadLicense() { return m_camcadLicense; }
//   CamcadLicenseNew& getCamcadLicense() { return m_camcadLicense; }

   int CheckAndSetSchematiclink(CString &errMsg);
   int RequestSchLinkClose();
   int OpenCadFile(CString filename);
   void SetCAMCADTitle();
   CString getCamCadSubtitle() const;
   CString getCamCadTitle() const;
   void setTitleSuffix(const CString& suffix);
   void pushTitleSuffix(const CString& suffix);
   CString popTitleSuffix();
   CString getVersionString() const;
   CString getCopyrightString() const;
   CString getFlowIdString() const;
   CString getAllRightsReservedString() const { return "All rights reserved."; }
   CString getCompanyNameString() const { return "Mentor Graphics Corporation"; }
   CString getUserPath() const;
   CString getCamcadExeFolderPath() const;
   CString getCmdLineImportSettingsFilePath() const;
   CString getCmdLineExportSettingsFilePath() const;
   void    setCmdLineImportSettingsFilePath(CString filepath);
   void    setCmdLineExportSettingsFilePath(CString filepath);

   CamcadExitCode GetExitCode()              { return m_exitCode; }
   void SetExitCode(CamcadExitCode code)     { m_exitCode = code; }
   void UpdateExitCode(CamcadExitCode code)  { if (m_exitCode == ExitCodeSuccess) m_exitCode = code; } // Set only if not already set to some error

   // Convenience function that performs the standard pattern of using command line settings file
   // setting if present, otherwise default to camcad install path plus default file name.
   // There is a separate one for import and export because the command line param
   // for import and export are separate.
   CString getImportSettingsFilePath(CString defaultFilename);
   CString getExportSettingsFilePath(CString defaultFilename);

   // The one above is specifically for CAD Importers.
   // This one is for system files like default.set, default.lt, default.fnt.
   CString getSystemSettingsFilePath(CString settingsFilename);

   void SetLogFileName(CString name)  { m_logfileName = name; }
   void OpenCamcadLogFile();
   void CloseCamcadLogFile();
   // LogMessage returns true if message is written, otherwise false.
   bool LogMessage(CString msg)  
   { 
	   if (m_logFp != NULL) 
		   fprintf(m_logFp, "%s", msg); 
	   return (m_logFp != NULL);
   }
   FILE *GetSystemLogFp() { return m_logFp; }
   FILE *OpenOperationLogFile(CString filename, CString& filepath);

   bool getUseDialogsFlag() const;
   void setUseDialogsFlag(bool flag);

   CMessageFilter& getMessageFilterTypeMessage() { return m_messageFilterTypeMessage; }
   CMessageFilter& getMessageFilterTypeFormat()  { return m_messageFilterTypeFormat;  }

   RunModeTag getRunMode() const;
   void setRunMode(RunModeTag runMode);

   BOOL ProcessMessageFilter(int code, LPMSG lpMsg);   // TOOLTIPS

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CCAMCADApp)
   public:
   virtual BOOL InitInstance();
   virtual int ExitInstance();
   virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
   virtual BOOL PumpMessage();
   virtual BOOL OnCmdMsg(UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
   //}}AFX_VIRTUAL

// Implementation
// COleTemplateServer m_server;

   BOOL GetLandscape();
   void SetLandscape(BOOL Landscape);

   //{{AFX_MSG(CCAMCADApp)
   //afx_msg void OnAppAbout();
   afx_msg void OnSettingsLoadGlobal();
   afx_msg void OnLoadDataFile();
   afx_msg void OnLoadProjectFile();
   afx_msg void OnLoadFont();
   afx_msg void OnHelpIndex();
   afx_msg void OnMacros();
   afx_msg void OnRunMacro();
   afx_msg void OnSetProjectPath();
   afx_msg void OnCamcadCom();
   afx_msg void OnRsiCom();
   afx_msg void OnOpenCADFile();
   afx_msg void OnEditAttrTemplate();
   afx_msg void OnSetTemplateDirectory();
   afx_msg void OnEditCustomTemplate();
   afx_msg void OnSetCustomTemplateDirectory();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnFileNew();
   afx_msg void OnAppExit();
	afx_msg void OnOnlineHelp();
   afx_msg void OnHelpInfoHub();
   afx_msg void OnToolsSchematiclink();
   afx_msg void OnLinkNets();
   afx_msg void OnLinkComps();
   afx_msg void OnSchematicSettings();
   afx_msg void OnUpdateLinkNets(CCmdUI *pCmdUI);
   afx_msg void OnUpdateLinkComps(CCmdUI *pCmdUI);
   afx_msg void OnUpdateSchematicSettings(CCmdUI *pCmdUI);
   afx_msg void OnDfmRunner();
   afx_msg void OnDfmScripter();
   afx_msg void OnToolsRsiExchange();
   afx_msg void OnToolsBomExplorer();
   afx_msg void OnToolsRealpart();
};

CCAMCADApp& getApp();
CWnd* getMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CMyCommandLineInfo
//
class CMyCommandLineInfo : public CCommandLineInfo
{
public:
   CMyCommandLineInfo() : CCommandLineInfo() 
   {
	   validInput = false;
	   validOutput = false;
   }

   void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
   void PrintErrorMessgeToLogFile();
   CString ParseParameterValue(CString cmdLineParam);
   BOOL validParameters();

private:
	BOOL validInput;
	BOOL validOutput;
};

#endif
