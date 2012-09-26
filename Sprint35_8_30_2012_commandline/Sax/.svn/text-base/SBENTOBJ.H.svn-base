// SbentObj.h

// Copyright 1993-1997 Sax Software, Inc.
// Copyright 1993-1997 Polar Engineering and Consulting.
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////

#ifndef xxSbentObj_h
#define xxSbentObj_h

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObj class

class CSaxBasicObj
{
public:
// Constructors
	CSaxBasicObj();
	~CSaxBasicObj();

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

// Attributes
public:

// Operations
public:
	// *** new property *** (GetXxx(), SetXxx())
	void SetCaption(LPCTSTR lpszNewValue);
	CString GetCaption();
	void Set_Caption(LPCTSTR lpszNewValue);
	CString Get_Caption();
	void SetBlockedKeywords(LPCTSTR lpszNewValue);
	CString GetBlockedKeywords();
	void SetCode(LPCTSTR lpszNewValue);
	CString GetCode();
	CString GetErrorFile();
	long GetErrorHelpContext();
	CString GetErrorHelpFile();
	short GetErrorLine();
	long GetErrorNumber();
	short GetErrorOffset();
	CString GetErrorSource();
	CString GetErrorText();
	void SetEvaluate(LPCTSTR Expression, LPCTSTR lpszNewValue);
	CString GetEvaluate(LPCTSTR Expression);
	void SetEventMode(BOOL bNewValue);
	BOOL GetEventMode();
	void SetHiddenCode(LPCTSTR lpszNewValue);
	CString GetHiddenCode();
	void SetPause(BOOL bNewValue);
	BOOL GetPause();
	void SetRun(BOOL bNewValue);
	BOOL GetRun();
	BOOL GetSyntaxCheck();

	// *** new method *** (Xxx())
	BOOL AddExtension(LPCTSTR Prefix, LPDISPATCH Ext);
	BOOL AddExtensionWithEvents(LPCTSTR Name, LPDISPATCH Ext);
	LPDISPATCH CreateHandler(LPCTSTR Prototype);
	LPDISPATCH CreateHandlers(LPCTSTR Prefix, LPDISPATCH EventObj);
	BOOL Disconnect();
	void Initialize(LPCTSTR SerialNumber);
	BOOL IsModuleLoaded(LPCTSTR FileName);
	BOOL LoadModule(LPCTSTR FileName);
	LPDISPATCH ModuleInstance(LPCTSTR FileName, BOOL New);
	BOOL RemoveExtensions(LPCTSTR FileName);
	BOOL ReportError(CException* e);
	void RunFile(LPCTSTR FileName);
	void RunThis(LPCTSTR Code);
	short Shutdown();
	void Trace(short Categories);
	BOOL UnloadModule(LPCTSTR FileName);

// virtual Callbacks
	// *** new event *** (OnBasicXxx())
	virtual void OnBasicBegin();
	virtual void OnBasicDebugPrint(LPCTSTR lpText);
	virtual void OnBasicDebugTrace(LPCTSTR lpText);
	virtual void OnBasicDialogIdle();
	virtual void OnBasicDoEvents();
	virtual void OnBasicErrorAlert();
	virtual void OnBasicEnd();
	virtual void OnBasicMacroBegin(LPCTSTR lpFileName);
	virtual void OnBasicMacroEnd(LPCTSTR lpFileName);
	virtual void OnBasicModalDialog(BOOL bDisplayed);
	virtual void OnBasicPause();
	virtual void OnBasicPending();
	virtual void OnBasicResume();
	virtual HWND OnBasicWindowHandle();
	virtual LPDISPATCH OnNewInstance(LPCTSTR lpFileName);
	virtual BOOL OnParseProperties(LPCTSTR lpFileName, LPCTSTR lpProperties);
	virtual short OnReadMacro(LPCTSTR lpFileName, BSTR* pbstrCode);

// private operations and data
private:
	void Advise();
	void Unadvise();
	COleDispatchDriver m_engine; // Sax Basic Engine - Enterprise Object
	LPDISPATCH m_psink; // Sax Basic Engine - Enterprise Object's Event Sink
	LPCONNECTIONPOINT m_pcp;
	DWORD m_dwCookie;
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
// CSaxBasicObjFrame frame

class CSaxBasicObjFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CSaxBasicObjFrame)
protected:
	CSaxBasicObjFrame();           // protected constructor used by dynamic creation

// Sax Basic Engine
protected:
	CSaxBasicObj* m_pSbobj;

// Attributes
public:
	void Attach(CSaxBasicObj* pSbobj) { m_pSbobj = pSbobj; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicObjFrame)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSaxBasicObjFrame();

	// Generated message map functions
	//{{AFX_MSG(CSaxBasicObjFrame)
	afx_msg void OnClose();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjMDIFrame frame

class CSaxBasicObjMDIFrame : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CSaxBasicObjMDIFrame)
protected:
	CSaxBasicObjMDIFrame();           // protected constructor used by dynamic creation

// Sax Basic Engine
private:
	CSaxBasicObj* m_pSbobj;

// Attributes
public:

// Operations
public:
	void Attach(CSaxBasicObj* pSbobj) { m_pSbobj = pSbobj; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicObjMDIFrame)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSaxBasicObjMDIFrame();

	// Generated message map functions
	//{{AFX_MSG(CSaxBasicObjMDIFrame)
	afx_msg void OnClose();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
