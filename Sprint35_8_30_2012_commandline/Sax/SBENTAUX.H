// SbentAux.h

// Copyright 1993-1997 Sax Software, Inc.
// Copyright 1993-1997 Polar Engineering and Consulting.
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////

#ifndef xxSbentAux_h
#define xxSbentAux_h

class CSBENTCTL; // Sax Basic Engine - Enterprise Control

/////////////////////////////////////////////////////////////////////////////

class CSbentAux
{
public:
// Add/Remove
	static void Add(CSBENTCTL& sbent);
	static void Remove(CSBENTCTL& sbent);

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
		catQueryEvent = 8,
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
	static void ClearDocument(CDocument* pDoc);
	static void ClosedDocument(CDocument* pDoc);
	static int ClosingFrame(CFrameWnd* pFrame); // -1:nested, 0:okay, 1:canceled by user
	static void DoEvents();
	static CSbentAux* FindSbentAux(CDocument* pDoc);
	static CSbentAux* FindSbentAux(CFrameWnd* pFrame);
	static CSbentAux* FindSbentAux(CSBENTCTL& sbent);
	BOOL ReportError(CException* e);
	static void SerializeDocument(CDocument* pDoc, CArchive& ar);
// Accessors
	CSBENTCTL& GetSbent() const { return m_sbent; }
// Operations
private:
	CSbentAux(CSBENTCTL& sbent);
	~CSbentAux();

	CSBENTCTL& m_sbent;
	CSbentAux* m_pSbentAuxPrev;
	CSbentAux* m_pSbentAuxNext;
};

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
// CSaxBasicDocTemplate document

class CSaxBasicDocTemplate : public CMultiDocTemplate
{
// Constructors
public:
	CSaxBasicDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

// Implementation
public:
	CDocument* OpenDocumentFile(LPCTSTR lpszPathName,
		BOOL bMakeVisible = TRUE);
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
	CSbentAux* FindSbentAux();

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
	CSbentAux* FindSbentAux();

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
	CSbentAux* FindSbentAux();

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
