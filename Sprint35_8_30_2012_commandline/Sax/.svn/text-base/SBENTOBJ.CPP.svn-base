//FILE: SbentObj.cpp

// Copyright 1993-1997 Sax Software, Inc.
// Copyright 1993-1997 Polar Engineering and Consulting.
// All rights reserved.

#include "sbentobj.h"

#ifndef DISPID_HandlerCall
	#define DISPID_HandlerCall 1
	#define DISPID_HandlerEvaluate 2
	#define DISPID_HandlerStart 3
	#define DISPID_HandlerExists 0
	#define DISPID_HandlerStepInto 4
	#define DISPID_HandlerCallPA 5
	#define DISPID_HandlerEvaluatePA 6
	#define DISPID_HandlerStartPA 7
	#define DISPID_HandlerPrototype 8
	#define DISPID_HandlerProcName 9
	#define DISPID_HandlerIsModuleLoaded 10

	#define DISPID_HandlersItem 0
	#define DISPID_HandlersCount 1
#endif

static const CLSID CLSID_SaxBasicEntObj =
{0x799FBA55,0xF241,0x11D1,{0xA1,0xC7,0x76,0x74,0x98,0x09,0x5F,0xF0}};
// {799FBA55-F241-11D1-A1C7-767498095FF0}

static const IID DIID_ISBENTOBJEVENTS =
{0xAD11711F,0x0847,0x101C,{0xA5,0x80,0x76,0x74,0x98,0x09,0x5F,0xF0}};
// {AD11711F-0847-101C-A580-767498095FF0}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicErrorObject

class CSaxBasicErrorObject : public CCmdTarget
{
	CSaxBasicErrorObject(CException* e);

// Attributes
public:

// Operations
public:
	static LPDISPATCH Create(CException* e);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicErrorObject)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSaxBasicErrorObject();

	// Generated message map functions
	//{{AFX_MSG(CSaxBasicErrorObject)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CSaxBasicErrorObject)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
private:
	long m_lNumber;
	CString m_strSource;
	CString m_strDescription;
	CString m_strHelpFile;
	long m_lHelpContext;
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicErrorObject

CSaxBasicErrorObject::CSaxBasicErrorObject(CException* e) :
	m_lHelpContext(0)
{
	EnableAutomation();

	if (e->IsKindOf(RUNTIME_CLASS(COleDispatchException)))
	{
		COleDispatchException* ode = (COleDispatchException*)e;
		m_lNumber = (unsigned short)ode->m_wCode;
		m_strDescription = ode->m_strDescription;
		m_strSource = ode->m_strSource;
		m_lHelpContext = ode->m_dwHelpContext;
		m_strHelpFile = ode->m_strHelpFile;
	}
	else
	{
		m_lNumber = COleException::Process(e);
		char sz[256];
		if (e->GetErrorMessage(sz,sizeof(sz)))
			m_strDescription = sz;
	}
}

LPDISPATCH CSaxBasicErrorObject::Create(CException* e)
{
	CSaxBasicErrorObject* p = new CSaxBasicErrorObject(e);
	return p->GetIDispatch(FALSE);
}

CSaxBasicErrorObject::~CSaxBasicErrorObject()
{
}

void CSaxBasicErrorObject::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CSaxBasicErrorObject, CCmdTarget)
	//{{AFX_MSG_MAP(CSaxBasicErrorObject)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSaxBasicErrorObject, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CSaxBasicErrorObject)
	DISP_PROPERTY_ID(CSaxBasicErrorObject, "Number", 0, m_lNumber, VT_I4)
	DISP_PROPERTY_ID(CSaxBasicErrorObject, "Source", 0x60020002, m_strSource, VT_BSTR)
	DISP_PROPERTY_ID(CSaxBasicErrorObject, "Description", 0x60020004, m_strDescription, VT_BSTR)
	DISP_PROPERTY_ID(CSaxBasicErrorObject, "HelpFile", 0x60020006, m_strHelpFile, VT_BSTR)
	DISP_PROPERTY_ID(CSaxBasicErrorObject, "HelpContext", 0x60020008, m_lHelpContext, VT_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjEventSink command target
// this class is for private use by CSaxBasicObj

class CSaxBasicObjEventSink : public CCmdTarget
{
	CSaxBasicObjEventSink(CSaxBasicObj* pSbobj);
	CSaxBasicObj* m_pSbobj;

// Attributes
public:

// Operations
public:
	static LPDISPATCH Create(CSaxBasicObj* pSbobj);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaxBasicObjEventSink)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSaxBasicObjEventSink();

	// Generated message map functions
	//{{AFX_MSG(CSaxBasicObjEventSink)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CSaxBasicObjEventSink)
	afx_msg void DebugPrint(LPCTSTR Text);
	afx_msg void DoEvents();
	afx_msg void ErrorAlert();
	afx_msg void Begin();
	afx_msg void End();
	afx_msg void Pause();
	afx_msg void Resume();
	afx_msg void Pending();
	afx_msg void ReadMacro(LPCTSTR FileName, BSTR FAR* Code, short FAR* Cancel);
	afx_msg void MacroBegin(LPCTSTR FileName);
	afx_msg void MacroEnd(LPCTSTR FileName);
	afx_msg void NewInstance(LPCTSTR FileName, LPDISPATCH FAR* Instance);
	afx_msg void ParseProperties(LPCTSTR FileName, LPCTSTR Properties, BOOL FAR* Cancel);
	afx_msg void WindowHandle(long FAR* hWnd);
	afx_msg void ModalDialog(BOOL Displayed);
	afx_msg void DebugTrace(LPCTSTR Text);
	afx_msg void OnBasicDialogIdle();
	//}}AFX_DISPATCH
	// *** new event *** (Xxx())
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjEventSink

CSaxBasicObjEventSink::CSaxBasicObjEventSink(CSaxBasicObj* pSbobj) :
	m_pSbobj(pSbobj)
{
	EnableAutomation();
}

LPDISPATCH CSaxBasicObjEventSink::Create(CSaxBasicObj* pSbobj)
{
	CSaxBasicObjEventSink* p = new CSaxBasicObjEventSink(pSbobj);
	return p->GetIDispatch(FALSE);
}

CSaxBasicObjEventSink::~CSaxBasicObjEventSink()
{
}

void CSaxBasicObjEventSink::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CSaxBasicObjEventSink, CCmdTarget)
	//{{AFX_MSG_MAP(CSaxBasicObjEventSink)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSaxBasicObjEventSink, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CSaxBasicObjEventSink)
	DISP_FUNCTION(CSaxBasicObjEventSink, "DebugPrint", DebugPrint, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CSaxBasicObjEventSink, "DoEvents", DoEvents, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSaxBasicObjEventSink, "ErrorAlert", ErrorAlert, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSaxBasicObjEventSink, "Begin", Begin, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSaxBasicObjEventSink, "End", End, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSaxBasicObjEventSink, "Pause", Pause, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSaxBasicObjEventSink, "Resume", Resume, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSaxBasicObjEventSink, "Pending", Pending, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CSaxBasicObjEventSink, "ReadMacro", ReadMacro, VT_EMPTY, VTS_BSTR VTS_PBSTR VTS_PI2)
	DISP_FUNCTION(CSaxBasicObjEventSink, "MacroBegin", MacroBegin, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CSaxBasicObjEventSink, "MacroEnd", MacroEnd, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CSaxBasicObjEventSink, "NewInstance", NewInstance, VT_EMPTY, VTS_BSTR VTS_PDISPATCH)
	DISP_FUNCTION(CSaxBasicObjEventSink, "ParseProperties", ParseProperties, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_PBOOL)
	DISP_FUNCTION(CSaxBasicObjEventSink, "WindowHandle", WindowHandle, VT_EMPTY, VTS_PI4)
	DISP_FUNCTION(CSaxBasicObjEventSink, "ModalDialog", ModalDialog, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CSaxBasicObjEventSink, "DebugTrace", DebugTrace, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CSaxBasicObjEventSink, "OnBasicDialogIdle", OnBasicDialogIdle, VT_EMPTY, VTS_NONE)
	// *** new event *** (DISP_FUNCTION(...,"Xxx",...)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISaxBasicObjEventSink to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

BEGIN_INTERFACE_MAP(CSaxBasicObjEventSink, CCmdTarget)
	INTERFACE_PART(CSaxBasicObjEventSink, DIID_ISBENTOBJEVENTS, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjEventSink message handlers

// *** new event *** (Xxx())

void CSaxBasicObjEventSink::DebugPrint(LPCTSTR Text) 
{
	m_pSbobj->OnBasicDebugPrint(Text);
}

void CSaxBasicObjEventSink::DoEvents() 
{
	m_pSbobj->OnBasicDoEvents();
}

void CSaxBasicObjEventSink::ErrorAlert() 
{
	m_pSbobj->OnBasicErrorAlert();
}

void CSaxBasicObjEventSink::Begin() 
{
	m_pSbobj->OnBasicBegin();
}

void CSaxBasicObjEventSink::End() 
{
	m_pSbobj->OnBasicEnd();
}

void CSaxBasicObjEventSink::Pause() 
{
	m_pSbobj->OnBasicPause();
}

void CSaxBasicObjEventSink::Resume() 
{
	m_pSbobj->OnBasicResume();
}

void CSaxBasicObjEventSink::Pending() 
{
	m_pSbobj->OnBasicPending();
}

void CSaxBasicObjEventSink::ReadMacro(LPCTSTR FileName, BSTR FAR* Code, short FAR* Cancel) 
{
	SysFreeString(*Code);
	*Code = NULL;
	*Cancel = m_pSbobj->OnReadMacro(FileName,Code);
}

void CSaxBasicObjEventSink::MacroBegin(LPCTSTR FileName) 
{
	m_pSbobj->OnBasicMacroBegin(FileName);
}

void CSaxBasicObjEventSink::MacroEnd(LPCTSTR FileName) 
{
	m_pSbobj->OnBasicMacroEnd(FileName);
}

void CSaxBasicObjEventSink::NewInstance(LPCTSTR FileName, LPDISPATCH FAR* Instance) 
{
	*Instance = m_pSbobj->OnNewInstance(FileName);
}

void CSaxBasicObjEventSink::ParseProperties(LPCTSTR FileName, LPCTSTR Properties, BOOL FAR* Cancel) 
{
	*Cancel = m_pSbobj->OnParseProperties(FileName,Properties);
}

void CSaxBasicObjEventSink::WindowHandle(long FAR* hWnd) 
{
	*hWnd = (long)m_pSbobj->OnBasicWindowHandle();
}

void CSaxBasicObjEventSink::ModalDialog(BOOL Displayed) 
{
	m_pSbobj->OnBasicModalDialog(Displayed);
}

void CSaxBasicObjEventSink::DebugTrace(LPCTSTR Text) 
{
	m_pSbobj->OnBasicDebugTrace(Text);
}

void CSaxBasicObjEventSink::OnBasicDialogIdle() 
{
	m_pSbobj->OnBasicDialogIdle();
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObj properties

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObj operations

CSaxBasicObj::CSaxBasicObj() :
	m_psink(NULL),
	m_pcp(0),
	m_dwCookie(0)
{
	if (m_engine.CreateDispatch(CLSID_SaxBasicEntObj))
	{
		m_psink = CSaxBasicObjEventSink::Create(this);
		Advise();
		TRY
		{
			// SetDialogIdle
			static BYTE parms[] =
				VTS_DISPATCH;
			m_engine.InvokeHelper(0x49, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
				m_psink);
		}
		END_TRY
	}
}

CSaxBasicObj::~CSaxBasicObj()
{
	Unadvise();
}

// *** new property *** (SetXxx(), GetXxx())

void CSaxBasicObj::SetCaption(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(DISPID_CAPTION, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString CSaxBasicObj::GetCaption()
{
	CString result;
	m_engine.InvokeHelper(DISPID_CAPTION, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::Set_Caption(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString CSaxBasicObj::Get_Caption()
{
	CString result;
	m_engine.InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::SetBlockedKeywords(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x39, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString CSaxBasicObj::GetBlockedKeywords()
{
	CString result;
	m_engine.InvokeHelper(0x39, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::SetCode(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString CSaxBasicObj::GetCode()
{
	CString result;
	m_engine.InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CSaxBasicObj::GetErrorFile()
{
	CString result;
	m_engine.InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

long CSaxBasicObj::GetErrorHelpContext()
{
	long result;
	m_engine.InvokeHelper(0x45, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString CSaxBasicObj::GetErrorHelpFile()
{
	CString result;
	m_engine.InvokeHelper(0x46, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

short CSaxBasicObj::GetErrorLine()
{
	short result;
	m_engine.InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

long CSaxBasicObj::GetErrorNumber()
{
	long result;
	m_engine.InvokeHelper(0x47, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

short CSaxBasicObj::GetErrorOffset()
{
	short result;
	m_engine.InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

CString CSaxBasicObj::GetErrorSource()
{
	CString result;
	m_engine.InvokeHelper(0x48, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CSaxBasicObj::GetErrorText()
{
	CString result;
	m_engine.InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::SetEvaluate(LPCTSTR Expression, LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	m_engine.InvokeHelper(0x1d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Expression, lpszNewValue);
}

CString CSaxBasicObj::GetEvaluate(LPCTSTR Expression)
{
	CString result;
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x1d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms,
		Expression);
	return result;
}

void CSaxBasicObj::SetEventMode(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	m_engine.InvokeHelper(0x20, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL CSaxBasicObj::GetEventMode()
{
	BOOL result;
	m_engine.InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::SetHiddenCode(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x12, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString CSaxBasicObj::GetHiddenCode()
{
	CString result;
	m_engine.InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::SetPause(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	m_engine.InvokeHelper(0x13, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL CSaxBasicObj::GetPause()
{
	BOOL result;
	m_engine.InvokeHelper(0x13, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::SetRun(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	m_engine.InvokeHelper(0x14, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL CSaxBasicObj::GetRun()
{
	BOOL result;
	m_engine.InvokeHelper(0x14, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL CSaxBasicObj::GetSyntaxCheck()
{
	BOOL result;
	m_engine.InvokeHelper(0x17, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

// *** new method *** (Xxx())

BOOL CSaxBasicObj::AddExtension(LPCTSTR Prefix, LPDISPATCH Ext)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_DISPATCH;
	m_engine.InvokeHelper(0x18, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Prefix, Ext);
	return result;
}

BOOL CSaxBasicObj::AddExtensionWithEvents(LPCTSTR Name, LPDISPATCH Ext)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_DISPATCH;
	m_engine.InvokeHelper(0x41, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Name, Ext);
	return result;
}

LPDISPATCH CSaxBasicObj::CreateHandler(LPCTSTR Prototype)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x26, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		Prototype);
	return result;
}

LPDISPATCH CSaxBasicObj::CreateHandlers(LPCTSTR Prefix, LPDISPATCH EventObj)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_DISPATCH;
	m_engine.InvokeHelper(0x34, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		Prefix, EventObj);
	return result;
}

BOOL CSaxBasicObj::Disconnect()
{
	if (m_engine.m_lpDispatch == NULL)
		return TRUE;

	BOOL result;
	m_engine.InvokeHelper(0x27, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	if (result)
	{
		Unadvise();
		m_engine.ReleaseDispatch();
	}

	return result;
}

void CSaxBasicObj::Initialize(LPCTSTR SerialNumber)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x63, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 SerialNumber);
}

BOOL CSaxBasicObj::IsModuleLoaded(LPCTSTR FileName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x2f, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		FileName);
	return result;
}

BOOL CSaxBasicObj::LoadModule(LPCTSTR FileName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x28, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		FileName);
	return result;
}

LPDISPATCH CSaxBasicObj::ModuleInstance(LPCTSTR FileName, BOOL New)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL;
	m_engine.InvokeHelper(0x37, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		FileName, New);
	return result;
}

BOOL CSaxBasicObj::RemoveExtensions(LPCTSTR FileName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x30, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		FileName);
	return result;
}

BOOL CSaxBasicObj::ReportError(CException* e)
{
	LPDISPATCH pErrObj = CSaxBasicErrorObject::Create(e);
	BOOL result;
	TRY
	{
		static BYTE parms[] =
			VTS_DISPATCH;
		m_engine.InvokeHelper(0x29, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
			pErrObj);
	}
	CATCH_ALL(e)
	{
		// ignore error
		result = FALSE;
	}
	END_CATCH_ALL;

	pErrObj->Release();
	return result;
}

void CSaxBasicObj::RunFile(LPCTSTR FileName)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName);
}

void CSaxBasicObj::RunThis(LPCTSTR Code)
{
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Code);
}

short CSaxBasicObj::Shutdown()
{
	if (m_engine.m_lpDispatch == NULL)
		return 0;

	short result;
	m_engine.InvokeHelper(0x2b, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

void CSaxBasicObj::Trace(short Categories)
{
	static BYTE parms[] =
		VTS_I2;
	m_engine.InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Categories);
}

BOOL CSaxBasicObj::UnloadModule(LPCTSTR FileName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	m_engine.InvokeHelper(0x2c, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		FileName);
	return result;
}

void CSaxBasicObj::OnBasicBegin()
{
}

void CSaxBasicObj::OnBasicDebugPrint(LPCTSTR lpText)
{
	TRACE1("OnBasicDebugPrint: %s\n",lpText);
}

void CSaxBasicObj::OnBasicDebugTrace(LPCTSTR lpText)
{
	TRACE1("%s\n",lpText);
}

void CSaxBasicObj::OnBasicDialogIdle()
{
}

void CSaxBasicObj::OnBasicDoEvents()
{
	CWinThread* pThread = AfxGetThread();
	pThread->OnIdle(0);
	MSG msg;
    for (int i = 0;
    	 i < 100 && ::PeekMessage(&msg,0,0,0,PM_NOREMOVE);
    	 ++i)
    {
    	if (msg.message == WM_QUIT)
    		break;

		pThread->PumpMessage();
	}
}

void CSaxBasicObj::OnBasicErrorAlert()
{
	TRACE1("OnBasicErrorAlert: %s\n",GetErrorText());
}

void CSaxBasicObj::OnBasicEnd()
{
}

void CSaxBasicObj::OnBasicMacroBegin(LPCTSTR lpFileName)
{
}

void CSaxBasicObj::OnBasicMacroEnd(LPCTSTR lpFileName)
{
}

void CSaxBasicObj::OnBasicModalDialog(BOOL bDisplayed)
{
	AfxGetApp()->EnableModeless(!bDisplayed);
}

void CSaxBasicObj::OnBasicPause()
{
}

void CSaxBasicObj::OnBasicPending()
{
}

void CSaxBasicObj::OnBasicResume()
{
}

HWND CSaxBasicObj::OnBasicWindowHandle()
{
	// get parent window for modal dialogs and message boxes.
	CWnd* pParent = AfxGetMainWnd();
	if (pParent == NULL || pParent->m_hWnd == NULL)
		return NULL;
	ASSERT_VALID(pParent);
	return pParent->m_hWnd;
}

LPDISPATCH CSaxBasicObj::OnNewInstance(LPCTSTR lpFileName)
{
	return NULL;
}

BOOL CSaxBasicObj::OnParseProperties(LPCTSTR lpFileName, LPCTSTR lpProperties)
{
	return FALSE;
}

short CSaxBasicObj::OnReadMacro(LPCTSTR lpFileName, BSTR* pbstrCode)
{
	return 1; // nothing to read
}

void CSaxBasicObj::Advise()
{
	if (m_psink != NULL)
	{
		if (m_pcp == NULL)
		{
			LPCONNECTIONPOINTCONTAINER pcpc = 0;
			HRESULT hr = m_engine.m_lpDispatch->QueryInterface(
				IID_IConnectionPointContainer,(void**)&pcpc);

			if (SUCCEEDED(hr))
			{
				LPCONNECTIONPOINT pcp = 0;
				hr = pcpc->FindConnectionPoint(DIID_ISBENTOBJEVENTS,&pcp);
				pcpc->Release();
				if (SUCCEEDED(hr))
					m_pcp = pcp;
			}
		}

		if (m_pcp != NULL)
			m_pcp->Advise(m_psink,&m_dwCookie);
	}
}

void CSaxBasicObj::Unadvise()
{
	if (m_psink != NULL)
	{
		if (m_pcp != NULL)
		{
			m_pcp->Unadvise(m_dwCookie);
			m_dwCookie = 0;
			m_pcp->Release();
			m_pcp = NULL;
		}

		m_psink->Release();
		m_psink = NULL;
	}
}

// Begin Shared Handler, Handlers and Module Section
#ifndef SB_SHARED_CLASSES_H
#define SB_SHARED_CLASSES_H

/////////////////////////////////////////////////////////////////////////////
// CSaxHandler

CSaxHandler::CSaxHandler()
{
}

CSaxHandler::CSaxHandler(const CSaxHandler& handler)
{
	*this = handler;
}

CSaxHandler::~CSaxHandler()
{
}

CSaxHandler& CSaxHandler::operator=(const CSaxHandler& handler)
{
	if (this != &handler)
	{
		if (handler.m_lpDispatch == NULL)
			ReleaseDispatch();
		else
		{
			handler.m_lpDispatch->AddRef();
			AttachDispatch(handler.m_lpDispatch);
		}
	}

	return *this;
}

BOOL CSaxHandler::GetExists() const
{
	BOOL bResult = FALSE;
	GetProperty(DISPID_HandlerExists,VT_BOOL,&bResult);
	return bResult;
}

BOOL CSaxHandler::GetIsModuleLoaded() const
{
	BOOL bResult = FALSE;
	GetProperty(DISPID_HandlerIsModuleLoaded,VT_BOOL,&bResult);
	return bResult;
}

void CSaxHandler::GetProcName(CString& sResult) const
{
	GetProperty(DISPID_HandlerProcName,VT_BSTR,&sResult);
}

void CSaxHandler::GetPrototype(CString& sResult) const
{
	GetProperty(DISPID_HandlerPrototype,VT_BSTR,&sResult);
}

void CSaxHandler::SetStepInto(BOOL bStepInto)
{
	SetProperty(DISPID_HandlerStepInto,VT_BOOL,bStepInto);
}

BOOL CSaxHandler::GetStepInto() const
{
	BOOL bResult = FALSE;
	GetProperty(DISPID_HandlerStepInto,VT_BOOL,&bResult);
	return bResult;
}

void AFX_CDECL CSaxHandler::Call(const BYTE* pbParamInfo, ...)
{
	va_list argList;
	va_start(argList, pbParamInfo);

	InvokeHelperV(DISPID_HandlerCall, DISPATCH_METHOD, VT_EMPTY, NULL, pbParamInfo, argList);

	va_end(argList);
}

VARIANT AFX_CDECL CSaxHandler::Evaluate(const BYTE* pbParamInfo, ...)
{
	va_list argList;
	va_start(argList, pbParamInfo);

	VARIANT var;
	VariantInit(&var);
	InvokeHelperV(DISPID_HandlerEvaluate, DISPATCH_METHOD, VT_VARIANT, &var, pbParamInfo, argList);

	va_end(argList);
	return var;
}

void AFX_CDECL CSaxHandler::Start(const BYTE* pbParamInfo, ...)
{
	va_list argList;
	va_start(argList, pbParamInfo);

	InvokeHelperV(DISPID_HandlerStart, DISPATCH_METHOD, VT_EMPTY, NULL, pbParamInfo, argList);

	va_end(argList);
}

void CSaxHandler::CallPA(const VARIANT& var)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(DISPID_HandlerCallPA, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		var);
}

VARIANT CSaxHandler::EvaluatePA(const VARIANT& var)
{
	VARIANT result;
	VariantInit(&result);
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(DISPID_HandlerEvaluatePA, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		var);
	return result;
}

void CSaxHandler::StartPA(const VARIANT& var)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(DISPID_HandlerStartPA, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		var);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxHandlers

CSaxHandlers::CSaxHandlers()
{
}

CSaxHandlers::CSaxHandlers(const CSaxHandlers& handlers)
{
	*this = handlers;
}

CSaxHandlers::~CSaxHandlers()
{
}

CSaxHandlers& CSaxHandlers::operator=(const CSaxHandlers& handlers)
{
	if (this != &handlers)
	{
		if (handlers.m_lpDispatch == NULL)
			ReleaseDispatch();
		else
		{
			handlers.m_lpDispatch->AddRef();
			AttachDispatch(handlers.m_lpDispatch);
		}
	}

	return *this;
}

short CSaxHandlers::GetCount() const
{
	short nResult = 0;
	GetProperty(DISPID_HandlersCount,VT_I2,&nResult);
	return nResult;
}

CSaxHandler CSaxHandlers::GetItem(LPCTSTR pszHandlerName) const
{
	LPDISPATCH lpResult = 0;
	static BYTE parms[] =
		VTS_BSTR;
	((CSaxHandlers*)this)->InvokeHelper(DISPID_HandlersItem, DISPATCH_PROPERTYGET, VT_DISPATCH,
		(void*)&lpResult, parms, pszHandlerName);

	CSaxHandler handler;
	handler.AttachDispatch(lpResult);
	return handler;
}

CSaxHandler CSaxHandlers::GetItem(short iIndex) const
{
	LPDISPATCH lpResult = 0;
	static BYTE parms[] =
		VTS_I2;
	((CSaxHandlers*)this)->InvokeHelper(DISPID_HandlersItem, DISPATCH_PROPERTYGET, VT_DISPATCH,
		(void*)&lpResult, parms, iIndex);

	CSaxHandler handler;
	handler.AttachDispatch(lpResult);
	return handler;
}

/////////////////////////////////////////////////////////////////////////////
// CSaxModule

CSaxModule::CSaxModule()
{
}

CSaxModule::CSaxModule(const CSaxModule& module)
{
	*this = module;
}

CSaxModule::~CSaxModule()
{
}

CSaxModule& CSaxModule::operator=(const CSaxModule& module)
{
	if (this != &module)
	{
		if (module.m_lpDispatch == NULL)
			ReleaseDispatch();
		else
		{
			module.m_lpDispatch->AddRef();
			AttachDispatch(module.m_lpDispatch);
		}
	}

	return *this;
}

DISPID CSaxModule::GetDispId(LPCTSTR pszMemberName) const
{
	if (m_lpDispatch == NULL)
	{
		TRACE0("Warning: attempt to call Invoke with NULL m_lpDispatch!\n");
		return DISPID_UNKNOWN;
	}

	DISPID dispidMember = DISPID_UNKNOWN;
	CString sMember = pszMemberName;
	BSTR bstrMember = sMember.AllocSysString();
	HRESULT hr = m_lpDispatch->GetIDsOfNames(IID_NULL,&bstrMember,1,
											 LOCALE_USER_DEFAULT,&dispidMember);
	SysFreeString(bstrMember);
	if (FAILED(hr))
		return DISPID_UNKNOWN;

	return dispidMember;
}

void AFX_CDECL CSaxModule::Call(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...)
{
	DISPID dispidMember = GetDispId(pszMemberName);
	if (dispidMember == DISPID_UNKNOWN)
		AfxThrowOleException(DISP_E_MEMBERNOTFOUND);

	va_list argList;
	va_start(argList, pbParamInfo);

	InvokeHelperV(dispidMember, DISPATCH_METHOD, VT_EMPTY, NULL, pbParamInfo, argList);

	va_end(argList);
}

VARIANT AFX_CDECL CSaxModule::Evaluate(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...)
{
	DISPID dispidMember = GetDispId(pszMemberName);
	if (dispidMember == DISPID_UNKNOWN)
		AfxThrowOleException(DISP_E_MEMBERNOTFOUND);

	va_list argList;
	va_start(argList, pbParamInfo);

	VARIANT var;
	VariantInit(&var);
	InvokeHelperV(dispidMember, DISPATCH_METHOD, VT_VARIANT, &var, pbParamInfo, argList);

	va_end(argList);
	return var;
}

void AFX_CDECL CSaxModule::Let(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...)
{
	DISPID dispidMember = GetDispId(pszMemberName);
	if (dispidMember == DISPID_UNKNOWN)
		AfxThrowOleException(DISP_E_MEMBERNOTFOUND);

	va_list argList;
	va_start(argList, pbParamInfo);

	InvokeHelperV(dispidMember, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, pbParamInfo, argList);

	va_end(argList);
}

void AFX_CDECL CSaxModule::Set(LPCTSTR pszMemberName, const BYTE* pbParamInfo, ...)
{
	DISPID dispidMember = GetDispId(pszMemberName);
	if (dispidMember == DISPID_UNKNOWN)
		AfxThrowOleException(DISP_E_MEMBERNOTFOUND);

	va_list argList;
	va_start(argList, pbParamInfo);

	InvokeHelperV(dispidMember, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, pbParamInfo, argList);

	va_end(argList);
}
#endif
// End Shared Handler, Handlers and Module Section

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjFrame

IMPLEMENT_DYNCREATE(CSaxBasicObjFrame, CFrameWnd)

CSaxBasicObjFrame::CSaxBasicObjFrame() :
	m_pSbobj(0)
{
}

CSaxBasicObjFrame::~CSaxBasicObjFrame()
{
}

BOOL CSaxBasicObjFrame::DestroyWindow() 
{
	return (!m_pSbobj || m_pSbobj->Disconnect()) && CFrameWnd::DestroyWindow();
}


BEGIN_MESSAGE_MAP(CSaxBasicObjFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CSaxBasicObjFrame)
	ON_WM_CLOSE()
	ON_WM_QUERYENDSESSION()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjFrame message handlers

void CSaxBasicObjFrame::OnClose() 
{
	if (m_pSbobj)
		// make sure that all macros are halted
		switch (m_pSbobj->Shutdown())
		{
		case -1:
			SetTimer(1, 500, NULL);
			break; // can't close now (try again later)
		case 0:
			CFrameWnd::OnClose();
		}
}

BOOL CSaxBasicObjFrame::OnQueryEndSession() 
{
	if (!CFrameWnd::OnQueryEndSession())
		return FALSE;
	
	if (m_pSbobj)
		// make sure that all macros are halted
		switch (m_pSbobj->Shutdown())
		{
		case -1:
			AfxMessageBox(_T("Macro still active.\n\nEnd all macros before quitting Windows."),
						  MB_ICONEXCLAMATION);
		case 1:
			return FALSE;
		}

	return TRUE;
}

void CSaxBasicObjFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
		PostMessage(WM_CLOSE, 0, 0l);

	KillTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjMDIFrame

IMPLEMENT_DYNCREATE(CSaxBasicObjMDIFrame, CMDIFrameWnd)

CSaxBasicObjMDIFrame::CSaxBasicObjMDIFrame() :
	m_pSbobj(0)
{
}

CSaxBasicObjMDIFrame::~CSaxBasicObjMDIFrame()
{
}

BOOL CSaxBasicObjMDIFrame::DestroyWindow() 
{
	return (!m_pSbobj || m_pSbobj->Disconnect()) && CMDIFrameWnd::DestroyWindow();
}


BEGIN_MESSAGE_MAP(CSaxBasicObjMDIFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CSaxBasicObjMDIFrame)
	ON_WM_CLOSE()
	ON_WM_QUERYENDSESSION()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicObjMDIFrame message handlers

void CSaxBasicObjMDIFrame::OnClose() 
{
	if (m_pSbobj)
		// make sure that all macros are halted
		switch (m_pSbobj->Shutdown())
		{
		case -1:
			SetTimer(1, 500, NULL);
			break; // can't close now (try again later)
		case 0:
			CMDIFrameWnd::OnClose();
		}
}

BOOL CSaxBasicObjMDIFrame::OnQueryEndSession() 
{
	if (!CMDIFrameWnd::OnQueryEndSession())
		return FALSE;
	
	if (m_pSbobj)
		switch (m_pSbobj->Shutdown())
		{
		case -1:
			AfxMessageBox(_T("Macro still active.\n\nEnd all macros before quitting Windows."),
						  MB_ICONEXCLAMATION);
		case 1:
			return FALSE;
		}

	return TRUE;
}

void CSaxBasicObjMDIFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
		PostMessage(WM_CLOSE, 0, 0l);

	KillTimer(nIDEvent);
}
