// SbproAux.cpp

// Copyright 1993-1997 Sax Software, Inc.
// Copyright 1993-1997 Polar Engineering and Consulting.
// All rights reserved.

#include "Sbpro.h"
#include "SbproAux.h"

#include <stdarg.h>

#define SB_MCM_REPORTERROR			(WM_USER+0x152)

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

#include <malloc.h>
#if _MFC_VER >= 0x0420
	#include <afxconv.h>
#elif _MFC_VER >= 0x0400
	#include <afxpriv.h>
#endif
#ifndef USES_CONVERSION
	#define USES_CONVERSION
#endif
#ifndef T2A
	#define T2A(s) (s)
#endif
#ifndef T2CA
	#define T2CA(s) (LPCSTR)T2A(s)
#endif

typedef struct SbproReportErrorTag
{
	DWORD scode;
	WORD wCode;
	LPCSTR lpszDescription;
	LPCSTR lpszSource;
	DWORD dwHelpContext;
	LPCSTR lpszHelpFile;
} SbproReportError;

struct CSbproThreadData : public CNoTrackObject
{
	CSbproThreadData() : pSbproAuxFirst(0), pSbproAuxLast(0) { }
	CSbproAux* pSbproAuxFirst;
	CSbproAux* pSbproAuxLast;
};

static CThreadLocal<CSbproThreadData> g_threaddata;

void CSbproAux::Add(CSbpro& sbpro)
{
	new CSbproAux(sbpro);
}

void CSbproAux::Remove(CSbpro& sbpro)
{
	CSbproAux* pSbproAux = g_threaddata->pSbproAuxFirst;
	while (pSbproAux && &pSbproAux->m_sbpro != &sbpro)
		pSbproAux = pSbproAux->m_pSbproAuxNext;

	if (pSbproAux)
		delete pSbproAux;
}

/////////////////////////////////////////////////////////////////////////////
// CSbproAux operations

// called by OnNewDocument
void CSbproAux::ClearDocument(CDocument* pDoc)
{
	// assumes one CSbproAux instance per document
	CSbproAux* pSbproAux = FindSbproAux(pDoc);
	if (pSbproAux != NULL)
		// new file
		pSbproAux->m_sbpro.ExecuteMenuCommand(CSbproAux::cmdFileNew);
}

// called by OnCloseDocument
void CSbproAux::ClosedDocument(CDocument* pDoc)
{
	// assumes one CSbproAux instance per document
	CSbproAux* pSbproAux = FindSbproAux(pDoc);
	if (pSbproAux != NULL)
		// disconnect language extensions
		VERIFY(pSbproAux->m_sbpro.Disconnect());
}

// return -1:nested, 0:okay, 1:canceled by user
int CSbproAux::ClosingFrame(CFrameWnd* pFrame)
{
	BOOL fNested = FALSE;
	CSbproAux* pSbproAux = g_threaddata->pSbproAuxFirst;
	while (pSbproAux)
	{
		if (pFrame == NULL || pFrame->IsChild(&pSbproAux->m_sbpro))
		{
			// make sure that the SbproAux CodeUnit is not executing (halt it)
			int iRes = pSbproAux->m_sbpro.Shutdown();
			if (iRes > 0)
				return 1; // canceled by user

			if (iRes < 0)
				fNested = TRUE;
		}

		pSbproAux = pSbproAux->m_pSbproAuxNext;
	}

	if (fNested)
		return -1; // need to defer close (inside a peek message loop)

	// document::CloseDocument must Disconnect the CSaxSbpro window
	return 0; // closing now is okay
}

void CSbproAux::DoEvents()
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

CSbproAux* CSbproAux::FindSbproAux(CDocument* pDoc)
{
	POSITION pos = pDoc->GetFirstViewPosition();
	CView* pView = pDoc->GetNextView(pos);
	if (pView != NULL)
	{
		// assumes one view per document
		CFrameWnd* pFrame = pView->GetParentFrame();
		if (pFrame != NULL)
		{
			// assumes one CSbproAux instance per document
			CSbproAux* pSbproAux = FindSbproAux(pFrame);
			return pSbproAux; // might be NULL
		}
	}

	return 0;
}

CSbproAux* CSbproAux::FindSbproAux(CFrameWnd* pFrame)
{
	CSbproAux* pSbproAux = g_threaddata->pSbproAuxFirst;
	while (pSbproAux && pFrame != NULL &&
		   !pFrame->IsChild(&pSbproAux->m_sbpro))
		pSbproAux = pSbproAux->m_pSbproAuxNext;

	return pSbproAux;
}

CSbproAux* CSbproAux::FindSbproAux(CSbpro& sbpro)
{
	CSbproAux* pSbproAux = g_threaddata->pSbproAuxFirst;
	while (pSbproAux && &pSbproAux->m_sbpro != &sbpro)
		pSbproAux = pSbproAux->m_pSbproAuxNext;

	return pSbproAux;
}

BOOL CSbproAux::ReportError(CException* e)
{
	HWND hwnd = (HWND)m_sbpro.GetHWnd();
	if (hwnd != NULL)
	{
		SbproReportError re;
		memset(&re,0,sizeof(re));
		if (e->IsKindOf(RUNTIME_CLASS(COleException)))
		{
			re.scode = ((COleException*)e)->m_sc;
			return (BOOL)::SendMessage(hwnd,SB_MCM_REPORTERROR,0,(LPARAM)&re);
		}

		if (e->IsKindOf(RUNTIME_CLASS(COleDispatchException)))
		{
			USES_CONVERSION;
			COleDispatchException* ode = (COleDispatchException*)e;
			re.scode = 0;
			re.wCode = ode->m_wCode;
			re.lpszDescription = T2CA((LPCTSTR)ode->m_strDescription);
			re.lpszSource = T2CA((LPCTSTR)ode->m_strSource);
			re.dwHelpContext = ode->m_dwHelpContext;
			re.lpszHelpFile = T2CA((LPCTSTR)ode->m_strHelpFile);
			return (BOOL)::SendMessage(hwnd,SB_MCM_REPORTERROR,0,(LPARAM)&re);
		}
	}

	// this should never happen?
	e->ReportError();
	return FALSE;
}

void CSbproAux::SerializeDocument(CDocument* pDoc, CArchive& ar)
{
	// assumes one CSbproAux instance per document
	CSbproAux* pSbproAux = FindSbproAux(pDoc);
	if (pSbproAux != NULL)
	{
		if (ar.IsStoring())
		{
			// not implemented
		}
		else
		{
			// not implemented
		}
	}
}

CSbproAux::CSbproAux(CSbpro& sbpro) :
	m_sbpro(sbpro)
{
	if (g_threaddata->pSbproAuxLast)
		g_threaddata->pSbproAuxLast->m_pSbproAuxNext = this;
	else
		g_threaddata->pSbproAuxFirst = this;

	m_pSbproAuxPrev = g_threaddata->pSbproAuxLast;
	m_pSbproAuxNext = 0;
	g_threaddata->pSbproAuxLast = this;
}

CSbproAux::~CSbproAux()
{
	// remove control from linked list
	if (m_pSbproAuxPrev)
		m_pSbproAuxPrev->m_pSbproAuxNext = m_pSbproAuxNext;
	else
		g_threaddata->pSbproAuxFirst = m_pSbproAuxNext;

	if (m_pSbproAuxNext)
		m_pSbproAuxNext->m_pSbproAuxPrev = m_pSbproAuxPrev;
	else
		g_threaddata->pSbproAuxLast = m_pSbproAuxPrev;
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
// CSaxBasicDocTemplate

CSaxBasicDocTemplate::CSaxBasicDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CDocument* CSaxBasicDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName,
												  BOOL bMakeVisible)
{
	POSITION pos = GetFirstDocPosition();
	CDocument* pDoc;
	if (pos == NULL)
	{
		pDoc = CMultiDocTemplate::OpenDocumentFile(NULL);
		if (lpszPathName == NULL)
			return pDoc;
	}
	else
		pDoc = GetNextDoc(pos);

	if (lpszPathName == NULL)
		pDoc->OnNewDocument();
	else
		pDoc->OnOpenDocument(lpszPathName);

	return pDoc;
}


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDoc

IMPLEMENT_DYNCREATE(CSaxBasicDoc, CDocument)

CSaxBasicDoc::CSaxBasicDoc()
{
}

CSaxBasicDoc::~CSaxBasicDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDoc diagnostics

#ifdef _DEBUG
void CSaxBasicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSaxBasicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDoc document management

BOOL CSaxBasicDoc::OnNewDocument() 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	if (pSbproAux)
		pSbproAux->GetSbpro().ExecuteMenuCommand(CSbproAux::cmdFileNew);

	return TRUE;
}

BOOL CSaxBasicDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	if (pSbproAux)
	{
		OnNewDocument();
		pSbproAux->GetSbpro().SetFileName(lpszPathName);
	}

	return TRUE;
}

void CSaxBasicDoc::OnCloseDocument()
{
	CSbproAux::ClosedDocument(this);

	CDocument::OnCloseDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDoc serialization

void CSaxBasicDoc::Serialize(CArchive& ar)
{
	// document is stored and loaded via CSaxBasic
}

BEGIN_MESSAGE_MAP(CSaxBasicDoc, CDocument)
	//{{AFX_MSG_MAP(CSaxBasicDoc)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDoc message handlers

void CSaxBasicDoc::OnFileClose() 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	if (pSbproAux)
		pSbproAux->GetSbpro().ExecuteMenuCommand(CSbproAux::cmdFileClose);
}

void CSaxBasicDoc::OnUpdateFileClose(CCmdUI* pCmdUI) 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	pCmdUI->Enable(pSbproAux &&
		pSbproAux->GetSbpro().IsMenuCommandEnabled(CSbproAux::cmdFileClose));
}

void CSaxBasicDoc::OnFileSave() 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	if (pSbproAux)
		pSbproAux->GetSbpro().ExecuteMenuCommand(CSbproAux::cmdFileSave);
}

void CSaxBasicDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	pCmdUI->Enable(pSbproAux &&
		pSbproAux->GetSbpro().IsMenuCommandEnabled(CSbproAux::cmdFileSave));
}

void CSaxBasicDoc::OnFileSaveAs() 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	if (pSbproAux)
		pSbproAux->GetSbpro().ExecuteMenuCommand(CSbproAux::cmdFileSaveAs);
}

void CSaxBasicDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	pCmdUI->Enable(pSbproAux &&
		pSbproAux->GetSbpro().IsMenuCommandEnabled(CSbproAux::cmdFileSaveAs));
}

void CSaxBasicDoc::OnFilePrint() 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	if (pSbproAux)
		pSbproAux->GetSbpro().ExecuteMenuCommand(CSbproAux::cmdFilePrint);
}

void CSaxBasicDoc::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	pCmdUI->Enable(pSbproAux &&
		pSbproAux->GetSbpro().IsMenuCommandEnabled(CSbproAux::cmdFilePrint));
}

void CSaxBasicDoc::OnFilePrintSetup() 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	if (pSbproAux)
		pSbproAux->GetSbpro().ExecuteMenuCommand(CSbproAux::cmdFilePrintSetup);
}

void CSaxBasicDoc::OnUpdateFilePrintSetup(CCmdUI* pCmdUI) 
{
	CSbproAux* pSbproAux = CSbproAux::FindSbproAux(this);
	ASSERT(pSbproAux != NULL);
	pCmdUI->Enable(pSbproAux &&
		pSbproAux->GetSbpro().IsMenuCommandEnabled(CSbproAux::cmdFilePrintSetup));
}


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicFrame

IMPLEMENT_DYNCREATE(CSaxBasicFrame, CFrameWnd)

CSaxBasicFrame::CSaxBasicFrame()
{
}

CSaxBasicFrame::~CSaxBasicFrame()
{
}

CSbproAux* CSaxBasicFrame::FindSbproAux()
{
	return CSbproAux::FindSbproAux(this);
}


BEGIN_MESSAGE_MAP(CSaxBasicFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CSaxBasicFrame)
	ON_WM_CLOSE()
	ON_WM_QUERYENDSESSION()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicFrame message handlers

void CSaxBasicFrame::OnClose() 
{
	// make sure that all macros are halted
	switch (CSbproAux::ClosingFrame(this))
	{
	case -1:
		SetTimer(1, 500, NULL);
		break; // can't close now (try again later)
	case 0:
		CFrameWnd::OnClose();
	}
}

BOOL CSaxBasicFrame::OnQueryEndSession() 
{
	if (!CFrameWnd::OnQueryEndSession())
		return FALSE;
	
	// make sure that all macros are halted
	switch (CSbproAux::ClosingFrame(this))
	{
	case -1:
		AfxMessageBox(_T("Macro still active.\n\nEnd all macros before quitting Windows."),
					  MB_ICONEXCLAMATION);
	case 1:
		return FALSE;
	}

	return TRUE;
}

void CSaxBasicFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
		PostMessage(WM_CLOSE, 0, 0l);

	KillTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMDIFrame

IMPLEMENT_DYNCREATE(CSaxBasicMDIFrame, CMDIFrameWnd)

CSaxBasicMDIFrame::CSaxBasicMDIFrame()
{
}

CSaxBasicMDIFrame::~CSaxBasicMDIFrame()
{
}

CSbproAux* CSaxBasicMDIFrame::FindSbproAux()
{
	return CSbproAux::FindSbproAux(this);
}


BEGIN_MESSAGE_MAP(CSaxBasicMDIFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CSaxBasicMDIFrame)
	ON_WM_CLOSE()
	ON_WM_QUERYENDSESSION()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMDIFrame message handlers

void CSaxBasicMDIFrame::OnClose() 
{
	// make sure that all macros are halted
	switch (CSbproAux::ClosingFrame(this))
	{
	case -1:
		SetTimer(1, 500, NULL);
		break; // can't close now (try again later)
	case 0:
		CMDIFrameWnd::OnClose();
	}
}

BOOL CSaxBasicMDIFrame::OnQueryEndSession() 
{
	if (!CMDIFrameWnd::OnQueryEndSession())
		return FALSE;
	
	switch (CSbproAux::ClosingFrame(this))
	{
	case -1:
		AfxMessageBox(_T("Macro still active.\n\nEnd all macros before quitting Windows."),
					  MB_ICONEXCLAMATION);
	case 1:
		return FALSE;
	}

	return TRUE;
}

void CSaxBasicMDIFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
		PostMessage(WM_CLOSE, 0, 0l);

	KillTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMDIChild

IMPLEMENT_DYNCREATE(CSaxBasicMDIChild, CMDIChildWnd)

CSaxBasicMDIChild::CSaxBasicMDIChild()
{
}

CSaxBasicMDIChild::~CSaxBasicMDIChild()
{
}

CSbproAux* CSaxBasicMDIChild::FindSbproAux()
{
	return CSbproAux::FindSbproAux(this);
}


BEGIN_MESSAGE_MAP(CSaxBasicMDIChild, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSaxBasicMDIChild)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMDIChild message handlers

void CSaxBasicMDIChild::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID&0xfff0) == SC_CLOSE)
		ShowWindow(SW_MINIMIZE);
	else
		CMDIChildWnd::OnSysCommand(nID, lParam);
}

void CSaxBasicMDIChild::OnClose() 
{
	switch (CSbproAux::ClosingFrame(this))
	{
	case -1:
		SetTimer(1, 500, NULL);
		break; // can't close now (try again later)
	case 0:
		CMDIChildWnd::OnClose();
	}
}

void CSaxBasicMDIChild::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
		PostMessage(WM_CLOSE, 0, 0L);

	KillTimer(nIDEvent);
}
