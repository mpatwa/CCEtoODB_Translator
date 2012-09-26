//FILE: SaxBasic.h

// Copyright 1993-1997 Sax Software, Inc.
// Copyright 1993-1997 Polar Engineering and Consulting.
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////

#ifndef xxSaxBasic_h
#define xxSaxBasic_h

#ifndef SAXBASIC_TOOLBAR
	#define SAXBASIC_TOOLBAR			0x0001
	#define SAXBASIC_STATUSBAR			0x0002
	#define SAXBASIC_USERFILEIO			0x0004
	#define SAXBASIC_EDITBUTTONS		0x0008
	#define SAXBASIC_ALWAYSSPLIT		0x0010
	#define SAXBASIC_MULTISHEET			0x0020
	#define SAXBASIC_NOHIGHLIGHT		0x0040
	#define SAXBASIC_NOSAVECHANGEDIR	0x0080
	#define SAXBASIC_TITLED				0x0100
	#define SAXBASIC_NOOPENCHANGEDIR	0x0200
	#define SAXBASIC_NOCHANGEDIR		0x0280
	//used internally					0x0400
	//#define ES_READONLY	    		0x0800
	#define SAXBASIC_PROJECT			0x1000
	#define SAXBASIC_NOINITFONT			0x2000
	#define SAXBASIC_NOPOPUPMENU		0x4000
	#define SAXBASIC_NOCODEUNIT			0x8000
#endif

#ifndef SB_NOTIFYERRORS
	#define SB_NOTIFYERRORS				0
	#define SB_NOTIFYBEGIN				1
	#define SB_NOTIFYEND				2
	#define SB_NOTIFYPAUSE				3
	#define SB_NOTIFYRESUME				4
	#define SB_NOTIFYERRORLOG			5
	#define SB_NOTIFYPREMODAL			6
	#define SB_NOTIFYPOSTMODAL			7
	#define SB_NOTIFYPENDING			8
	#define SB_NOTIFYMACROBEGIN			9
	#define SB_NOTIFYMACROEND			10
	#define SB_NOTIFYEDITORPREMODAL		11
	#define SB_NOTIFYEDITORPOSTMODAL	12
	#define SB_NOTIFYHALTED				13
	#define SB_NOTIFYPROTOTYPECHANGE	14
	#define SB_NOTIFYCHANGE				15
	#define SB_NOTIFYFONTCHANGE			16
	#define SB_NOTIFYSHOWFORM			17
	#define SB_NOTIFYDIALOGIDLE			18
	#define SB_NOTIFYHANDLEERROR		19

	#define SB_TRACEENTER				0
	#define SB_TRACELEAVE				1
	#define SB_TRACEINPUT				2
	#define SB_TRACEOUTPUT				3
	#define SB_TRACECODE				4
#endif

class CSaxBasicLoader;
class CSaxBasicInstance;

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic class

class CSaxBasic
{
	friend CSaxBasicLoader;
// Constructors
public:
	CSaxBasic();
	virtual ~CSaxBasic();

// Commands
	enum CmdNum
	{
		cmdFileNew,cmdFileOpen,cmdFileSave,cmdFileSaveAs,cmdFilePrint,
		cmdFilePrintSetup,
		cmdMacroRun,cmdMacroPause,cmdMacroEnd,
		cmdDebugStepInto,cmdDebugStepOver,cmdDebugStepTo,cmdDebugBreak,
		cmdDebugQuickWatch,cmdDebugAddWatch,cmdDebugBrowse,cmdDebugSetNext,
		cmdDebugShowNext,
		cmdHelpApp,cmdHelpLanguage,cmdHelpTopic,cmdHelpAbout,
		cmdEditUndo,cmdEditCut,cmdEditCopy,cmdEditPaste,
		cmdEditFind,cmdEditReplace,cmdEditAgain,
		cmdViewFont,
		cmdEditDelete,cmdEditSelectAll,cmdEditUserDialog,
		cmdFileClose,cmdFileSaveAll,
		cmdDebugStepOut,
		cmdSheetOpenUses,cmdSheetCloseAll,cmdSheet1,cmdSheet2,cmdSheet3,
		cmdSheet4,cmdSheet5,cmdSheet6,cmdSheet7,cmdSheet8,cmdSheet9,
		cmdFileNewCodeModule,cmdFileNewObjectModule,cmdFileNewClassModule,
		cmdEditProperties,
		cmdEditIndent,cmdEditOutdent,
		cmdDebugClearAllBreaks,
		cmdViewMacro,cmdViewImmediate,cmdViewWatch,cmdViewStack,cmdViewLoaded,
		cmdViewToolbar,cmdViewStatusBar,cmdViewEditButtons,cmdViewAlwaysSplit,
		cmdEditRedo,
		cmdEditReferences,
		cmdEditTabAsSpaces,
		cmdViewTabWidth1,cmdViewTabWidth2,cmdViewTabWidth3,cmdViewTabWidth4,
		cmdViewTabWidth5,cmdViewTabWidth6,cmdViewTabWidth7,cmdViewTabWidth8,
		// cmd<MenuItem> add new cmds here
		cmdCount,
		cmdEditFont = 29, // cmdViewFont
		cmdPopupNoMouse = 0x777c,
		cmdShowError = 0x7ffd,
		cmdNewModulePopup = 0x7ffe,
		cmdPopup = 0x7fff
	};

// Trace Categories
	enum TraceCategory
	{
		catNone = 0,
		catAction = 1,
		catQuery = 2,
		catActionEvent = 4,
		catActionCallback = 4,
		catQueryEvent = 8,
		catQueryCallback = 8,
		catInternal =  64,
		catExecution = 128,
		catAll = 255
	};

// Module Kinds
	enum ModuleKinds
	{
		modMacro,
		modCode,
		modObject,
		modClass
	};

// Special Operations
	void Activate();
	static short ClosingFrame(CFrameWnd* pFrame); // -1:nested, 0:okay, 1:canceled by user
	void DebugTrace(short nType, LPCSTR pszText);
	static CSaxBasic* FindBasic(CFrameWnd* pFrame,
								CSaxBasic* pBasicPrev = NULL);
	CEdit* GetAttachedWindow() const { return m_pEdit; }
	CSaxBasicInstance* GetInstance() const { return m_pInstance; }
	static void Load();
	static void Free();

// Operations
	// *** new property *** (GetXxx(), SetXxx())
	BOOL SetActiveSheet(short iSheet);
	short GetActiveSheet() const;
	void SetAlwaysSplit(BOOL bAlwaysSplit);
	BOOL GetAlwaysSplit() const;
	void SetAttributeName(LPCTSTR pszAttributeName, short iSheet = 0);
	CString GetAttributeName(short iSheet = 0) const { CString s; GetAttributeName(s,iSheet); return s; }
	void SetBlockedKeywords(LPCTSTR pszKeywords);
	CString GetBlockedKeywords() const { CString s; GetBlockedKeywords(s); return s; }
	VARIANT GetBreakPoints(short iSheet = 0) const { VARIANT v; VariantInit(&v); GetBreakPoints(v,iSheet); return v; }
	void SetBreakPoints(const VARIANT& var, short iSheet = 0);
	void SetCaption(LPCTSTR pszCaption);
	CString GetCaption() const { CString s; GetCaption(s); return s; }
	void SetChanged(BOOL bModified, short iSheet = 0);
	BOOL GetChanged(short iSheet = 0) const;
	void SetCode(LPCTSTR pszCode, short iSheet = 0);
	CString GetCode(short iSheet = 0) const { CString s; GetCode(s,iSheet); return s; }
	void SetDebugHeight(short iHeight);
	short GetDebugHeight() const;
	void SetDefaultDataType(LPCTSTR pszDataType);
	CString GetDefaultDataType() const { CString s; GetDefaultDataType(s); return s; }
	void SetDefaultMacroName(LPCTSTR pszMacroName);
	CString GetDefaultMacroName() const { CString s; GetDefaultMacroName(s); return s; }
	void SetDefaultObjectName(LPCTSTR pszObjectName);
	CString GetDefaultObjectName() const { CString s; GetDefaultObjectName(s); return s; }
	void SetEditTools(BOOL bEditTools);
	BOOL GetEditTools() const;
	void SetEnabled(BOOL bEnable);
	BOOL GetEnabled() const;
	CString GetErrorFile() const { CString s; GetErrorFile(s); return s; }
	long GetErrorHelpContext() const;
	CString GetErrorHelpFile() const { CString s; GetErrorHelpFile(s); return s; }
	short GetErrorLine() const;
	long GetErrorNumber() const;
	short GetErrorOffset() const;
	CString GetErrorSource() const { CString s; GetErrorSource(s); return s; }
	CString GetErrorText() const { CString s; GetErrorText(s); return s; }
	void SetEvaluate(LPCTSTR pszExpression, LPCTSTR pszResult);
	CString GetEvaluate(LPCTSTR pszExpression) const;
	void SetEventMode(BOOL bEventMode);
	BOOL GetEventMode() const;
	void SetFileChangeDir(BOOL bFileChangeDir);
	BOOL GetFileChangeDir() const;
	void SetFileDesc(LPCTSTR pszDescription);
	CString GetFileDesc() const { CString s; GetFileDesc(s); return s; }
	void SetFileExt(LPCTSTR pszExtension);
	CString GetFileExt() const { CString s; GetFileExt(s); return s; }
	BOOL SetFileName(LPCTSTR pszFileName, short iSheet = 0) { return SetFilename(pszFileName,iSheet); }
	CString GetFileName(short iSheet = 0) const { CString s; GetFilename(s,iSheet); return s; }
	void SetFileTools(BOOL bFileTools);
	BOOL GetFileTools() const;
	void SetFullPopupMenu(BOOL bFullPopupMenu);
	BOOL GetFullPopupMenu() const;
	short GetHeaderLineCount(short iSheet = 0) const;
	void SetHiddenCode(LPCTSTR pszHiddenCode);
	CString GetHiddenCode() const { CString s; GetHiddenCode(s); return s; }
	void SetHighlightBreak(unsigned long lColor);
	unsigned long GetHighlightBreak() const;
	void SetHighlightBuiltin(unsigned long lColor);
	unsigned long GetHighlightBuiltin() const;
	void SetHighlightComment(unsigned long lColor);
	unsigned long GetHighlightComment() const;
	void SetHighlightError(unsigned long lColor);
	unsigned long GetHighlightError() const;
	void SetHighlightExec(unsigned long lColor);
	unsigned long GetHighlightExec() const;
	void SetHighlightExtension(unsigned long lColor);
	unsigned long GetHighlightExtension() const;
	void SetHighlightReserved(unsigned long lColor);
	unsigned long GetHighlightReserved() const;
	void SetLocked(BOOL bLocked);
	BOOL GetLocked() const;
	CString GetMenuCommandDesc(CmdNum cmd) const;
	CString GetMenuCommandTooltip(CmdNum cmd) const;
	short GetModuleKind(short iSheet = 0) const;
	void SetMultiSheet(BOOL bMultiSheet);
	BOOL GetMultiSheet() const;
	void SetPause(BOOL bPause);
	BOOL GetPause() const;
	void SetProcDisplayMode(short iProcDisplayMode);
	short GetProcDisplayMode() const;
	void SetRun(BOOL bRun);
	BOOL GetRun() const;
	void SetSelLength(long lLength);
	long GetSelLength() const;
	void SetSelStart(long lStart);
	long GetSelStart() const;
	void SetSelText(LPCTSTR pszText);
	CString GetSelText() const;
	short GetSheetCount() const { return SheetCount(); }
	void SetStatusVisible(BOOL bStatusVisible);
	BOOL GetStatusVisible() const;
	BOOL GetSyntaxCheck() const { return SyntaxCheck(); }
	void SetTabAsSpaces(BOOL bTabAsSpaces);
	BOOL GetTabAsSpaces() const;
	void SetTabWidth(short iTabWidth);
	short GetTabWidth() const;
	void SetText(LPCTSTR pszText);
	CString GetText() const;
	void SetToolbarVisible(BOOL bToolbarVisible);
	BOOL GetToolbarVisible() const;

	// *** new method *** (Xxx())
	BOOL AddExtension(LPCTSTR pszPrefix, LPDISPATCH pDisp = NULL)
	{ return AddExtension(pDisp,pszPrefix); }
	BOOL AddExtensionWithEvents(LPCTSTR pszName, LPDISPATCH pDisp);
	BOOL CloseSheet(short iSheet = 0);
	LPDISPATCH CreateHandler(LPCTSTR pszPrototype);
	LPDISPATCH CreateHandlers(LPCTSTR pszPrefix, LPDISPATCH pEventObj);
	BOOL Disconnect();
	BOOL ExecuteMenuCommand(CmdNum cmd);
	void FindOrCreateProc(LPCTSTR pszProcName);
	short FindSheet(LPCTSTR pszFileName);
	long IndexFromLine(short iLine) const;
	BOOL IsMenuCommandChecked(CmdNum cmd) const;
	BOOL IsMenuCommandEnabled(CmdNum cmd) const;
	BOOL IsModuleLoaded(LPCTSTR pszFileName) const;
	short LineFromIndex(long lIndex) const;
	BOOL LoadModule(LPCTSTR pszFileName);
	BSTR MacroCaption(LPCTSTR pszFileName) const
	{ CString s = pszFileName; GetMacroCaption(s); return s.AllocSysString(); }
	LPDISPATCH ModuleInstance(LPCTSTR pszFileName, BOOL bNewInstance);
	BOOL RemoveExtensions(LPCTSTR pszFileName);
	BOOL ReportError(CException* e);
	void RunFile(LPCTSTR pszFileName);
	void RunThis(LPCTSTR pszRunThis);
	short Shutdown();
	void Trace(short categories);
	BOOL UnloadModule(LPCTSTR pszFileName);

// no OCX equivalent
	// like FileChangeDir, but only for File Open dialogs
	void SetFileOpenChangeDir(BOOL bFileOpenChangeDir);
	BOOL GetFileOpenChangeDir() const;
	// like FileChangeDir, but only for File Save dialogs
	void SetFileSaveChangeDir(BOOL bFileSaveChangeDir);
	BOOL GetFileSaveChangeDir() const;
	// currently executing macro name
	void GetMacroName(CString &rString) const;
	// load/save files
	BOOL LoadFilename(LPCTSTR pszFileName, short iSheet = 0);
	BOOL SaveFilename(short iSheet = 0) { return SaveFilenameAs(0,iSheet); }
	BOOL SaveFilenameAs(LPCTSTR pszFileName, short iSheet = 0);

// Virtual call-back functions
	// *** new event *** (OnBasicXxx())
	virtual void OnBasicAboutBox();
	virtual void OnBasicBegin();
	virtual void OnBasicDebugPrint(LPCSTR pszText);
	virtual void OnBasicDebugTrace(LPCTSTR pszText);
	virtual void OnBasicDialogIdle();
	virtual void OnBasicDoEvents();
	virtual void OnBasicEnd();
	virtual void OnBasicErrorAlert();
	virtual void OnBasicHandleError();
	virtual BOOL OnBasicHelp(LPCSTR pszKeyword);
	virtual void OnBasicMacroBegin(LPCTSTR pszFileName);
	virtual void OnBasicMacroEnd(LPCTSTR pszFileName);
	virtual void OnBasicModalDialog(BOOL bDisplayed);
	virtual void OnBasicNotify(short nNotify);
	virtual void OnBasicPause();
	virtual void OnBasicPending();
	virtual void OnBasicResume();
	virtual void OnBasicShowForm();
	virtual void OnBasicStatusbar(LPCSTR pszText, BOOL bAlert);
	virtual HWND OnBasicWindowHandle();
	virtual void OnChange();
	virtual BOOL OnCloseSheet(short iSheet, BOOL* pbCancel);
	virtual void OnGetMacroCaption(LPSTR pszCaption, UINT cbLen);
	virtual BOOL OnGetMacroName(BOOL bOpen, LPSTR pszFileName, UINT cbLen);
	virtual LPDISPATCH OnNewInstance(LPCSTR pszFileName);
	virtual BOOL OnOverrideMenuCommand(CmdNum cmd);
	virtual BOOL OnParseProperties(LPCSTR pszFileName, LPCSTR pszProperties);
	virtual UINT OnReadMacro(LPCSTR pszFileName, LPSTR pszCode);
	virtual void OnUpdateMFCUI(BOOL bForDoEvents);
	virtual void OnUpdateProperties(LPCSTR pszFileName, CString& sProperties);
	virtual BOOL OnWriteMacro(LPCSTR pszFileName, LPCSTR pszCode);

// backward compatibility (prior to v3.5)
	enum ColorNum
	{
		colorReserved,
		colorExtension,
		colorBuiltin,
		colorComment,
		colorError,
		colorExec,
		colorBreak,
		colorTextForeground,
		colorTextBackground,
		colorSelectionForeground,
		colorSelectionBackground,
		colorCount
	};
	BOOL AddExtension(LPDISPATCH pDisp, LPCTSTR pszPrefix = NULL);
	BOOL AddExtensionWide(LPDISPATCH pDisp, LPCTSTR pszPrefix = NULL);
	void GetAttributeName(CString &rString, short iSheet = 0) const;
	void GetBlockedKeywords(CString &rString) const;
	void GetBreakPoints(VARIANT& var, short iSheet = 0) const;
	void GetCaption(CString &rString) const;
	void GetCode(CString &rString, short iSheet = 0) const;
	void SetColor(ColorNum color, COLORREF rgb);
	COLORREF GetColor(ColorNum color) const;
	void GetDefaultDataType(CString &rString) const;
	void GetDefaultMacroName(CString &rString) const;
	void GetDefaultObjectName(CString &rString) const;
	void GetErrorFile(CString &rString) const;
	void GetErrorHelpFile(CString &rString) const;
	void GetErrorSource(CString &rString) const;
	void GetErrorText(CString &rString) const;
	void SetExpression(LPCTSTR pszExpression);
	void GetExpression(CString &rString) const;
	void GetFileDesc(CString &rString) const;
	void GetFileExt(CString &rString) const;
	BOOL SetFilename(LPCTSTR pszFileName, short iSheet = 0);
	void GetFilename(CString &rString, short iSheet = 0) const;
	void GetHiddenCode(CString &rString) const;
	void GetMacroCaption(CString& sMacroCaption) const;
	void SetResult(LPCTSTR pszResult);
	void GetResult(CString &rString) const;
	short SheetCount() const;
	BOOL SyntaxCheck() const;

// Implementation
protected:
	void AttachWindow(CEdit* pWnd);
	void DetachWindow();
	void Initialize();
	BOOL IsWindowAttached() const { return m_pEdit != NULL; }

private:
#ifdef WIN32
	BOOL AddExtensionWrap(LPDISPATCH pDisp, LPCTSTR pszPrefix);
#endif
	unsigned long GetHighlightColor(short iColor) const;
	void GetStringValue(WORD msg, CString &rString, short iSheet = 0) const;
	BOOL GetStyleBit(WORD wStyle) const;
	void SetHighlightColor(short iColor, unsigned long lColor);
	void SetStyleBit(WORD wStyle, BOOL bSet);
	BOOL SheetSelect(short iSheet) const;
	CSaxBasic* m_pBasicPrev;
	CSaxBasic* m_pBasicNext;
protected:
	CSaxBasicInstance* m_pInstance;
	CEdit* m_pEdit;
};

// MFC creates an IDispatch using ANSI strings if OLE2ANSI defined
// (This means that WIN32 is defined and _UNICODE is not defined.)
// WIN32 requires an IDispatch using _UNICODE strings
// Create a wrapper IDispatch that converts ANSI to/from _UNICODE
// if WIN32 is defined and OLE2ANSI defined

inline BOOL CSaxBasic::AddExtension(LPDISPATCH pDisp, LPCTSTR pszPrefix)
{
#if defined(WIN32) && defined(OLE2ANSI)
	return AddExtensionWrap(pDisp,pszPrefix);
#else
	return AddExtensionWide(pDisp,pszPrefix);
#endif
}

// Begin Shared Handler, Handlers and Module Section
#ifndef SB_SHARED_CLASSES_CPP
#define SB_SHARED_CLASSES_CPP
/////////////////////////////////////////////////////////////////////////////
// CSaxHandler class

#ifndef AFX_CDECL
	#define AFX_CDECL
#endif

class CSaxHandler : public COleDispatchDriver
{
// Constructors
public:
	CSaxHandler();
	CSaxHandler(const CSaxHandler& handler);
	~CSaxHandler();
	CSaxHandler& operator=(const CSaxHandler& handler);

// Properties
	BOOL GetExists() const;
	BOOL GetIsModuleLoaded() const;
	CString GetProcName() const { CString s; GetProcName(s); return s; }
	CString GetPrototype() const { CString s; GetPrototype(s); return s; }
	void SetStepInto(BOOL fNewValue);
	BOOL GetStepInto() const;

// Methods
	void AFX_CDECL Call(const BYTE* pbParamInfo, ...);
	VARIANT AFX_CDECL Evaluate(const BYTE* pbParamInfo, ...);
	void AFX_CDECL Start(const BYTE* pbParamInfo, ...);
	void CallPA(const VARIANT& var);
	VARIANT EvaluatePA(const VARIANT& var);
	void StartPA(const VARIANT& var);

// backward compatibility (prior to v3.5)
	void GetProcName(CString& rString) const;
	void GetPrototype(CString& rString) const;
};

/////////////////////////////////////////////////////////////////////////////
// CSaxHandlers class

class CSaxHandlers : public COleDispatchDriver
{
// Constructors
public:
	CSaxHandlers();
	CSaxHandlers(const CSaxHandlers& handlers);
	~CSaxHandlers();
	CSaxHandlers& operator=(const CSaxHandlers& handlers);

// Properties
	short GetCount() const;
	CSaxHandler GetItem(LPCTSTR pszHandlerName) const;
	CSaxHandler GetItem(short iIndex) const;
};

/////////////////////////////////////////////////////////////////////////////
// CSaxModule class

class CSaxModule : public COleDispatchDriver
{
// Constructors
public:
	CSaxModule();
	CSaxModule(const CSaxModule& module);
	~CSaxModule();
	CSaxModule& operator=(const CSaxModule& module);

// Properties
	DISPID GetDispId(LPCTSTR pszMemberName) const;

// Methods
	void AFX_CDECL Call(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...);
	VARIANT AFX_CDECL Evaluate(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...);
	void AFX_CDECL Let(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...);
	void AFX_CDECL Set(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...);
};

#endif
// End Shared Handler, Handlers and Module Section

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicEdit control

class CSaxBasicEdit : public CEdit, public CSaxBasic
{
	DECLARE_DYNAMIC(CSaxBasicEdit)

// Constructors
public:
	CSaxBasicEdit();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicEdit)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSaxBasicEdit();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

#if defined(_AFXDLL) && _MFC_VER >= 0x0400
	AFX_MODULE_STATE* ModuleState() { return m_pModuleState; }
#endif

	// Generated message map functions
protected:
	// Subclassed control support
	WNDPROC* GetSuperWndProcAddr(void);

	//{{AFX_MSG(CSaxBasicEdit)
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView view

class CSaxBasicDoc;

class CSaxBasicView : public CEditView, public CSaxBasic
{
	friend CSaxBasicDoc;
protected: // create from serialization only
	CSaxBasicView();
	DECLARE_DYNCREATE(CSaxBasicView)

// Implementation
public:
	virtual ~CSaxBasicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL ExecuteMenuCommand(CmdNum cmd);

#if _MFC_VER >= 0x0400 && defined(_AFXDLL)
	AFX_MODULE_STATE* ModuleState() { return m_pModuleState; }
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
	protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

protected:
	virtual void Serialize(CArchive& ar);

	// Generated message map functions
protected:
	// Subclassed control support
	WNDPROC* GetSuperWndProcAddr(void);

	//{{AFX_MSG(CSaxBasicView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnUpdateFilePrinting(CCmdUI* pCmdUI);
	afx_msg LRESULT OnFindReplaceCmd(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDocTemplate document

class CSaxBasicDocTemplate : public CMultiDocTemplate
{
// Constructors
public:
	CSaxBasicDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	~CSaxBasicDocTemplate();

// Implementation
public:
	CDocument* OpenDocumentFile(LPCTSTR lpszPathName,
		BOOL bMakeVisible = TRUE);
	CDocument* CreateNewDocument();
private:
	BOOL m_bLoaded;
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDoc document

class CSaxBasicDoc : public CDocument
{
protected:
	CSaxBasicDoc();                 // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSaxBasicDoc)

// Attributes
public:

// Operations
public:
	CSaxBasic* FindBasic();

// Overrides
protected:

// Implementation
public:
	virtual ~CSaxBasicDoc();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSaxBasicDoc)
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintSetup();
	afx_msg void OnUpdateFilePrintSetup(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicFrame frame

class CSaxBasicFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CSaxBasicFrame)
protected:
	CSaxBasicFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CSaxBasic* FindBasic();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSaxBasicFrame();

	// Generated message map functions
	//{{AFX_MSG(CSaxBasicFrame)
	afx_msg void OnClose();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMDIFrame frame

class CSaxBasicMDIFrame : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CSaxBasicMDIFrame)
protected:
	CSaxBasicMDIFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CSaxBasic* FindBasic();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicMDIFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSaxBasicMDIFrame();

	// Generated message map functions
	//{{AFX_MSG(CSaxBasicMDIFrame)
	afx_msg void OnClose();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMDIChild frame

class CSaxBasicMDIChild : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSaxBasicMDIChild)
protected:
	CSaxBasicMDIChild();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CSaxBasic* FindBasic();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicMDIChild)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSaxBasicMDIChild();

	// Generated message map functions
	//{{AFX_MSG(CSaxBasicMDIChild)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
